/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Inline implementation of SVE.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <agrum/PRM/inference/SVE.h>
#include <agrum/PRM/classDependencyGraph.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR> SVE<GUM_SCALAR>::~SVE() {
      GUM_DESTRUCTOR(SVE);

      for (const auto &elt : __elim_orders)
        delete elt.second;

      for (const auto &elt : __lifted_pools)
        delete elt.second;

      if (__class_elim_order != nullptr)
        delete __class_elim_order;

      for (const auto trash : __lifted_trash)
        delete trash;
    }

    template <typename GUM_SCALAR>
    void SVE<GUM_SCALAR>::__eliminateNodes(const Instance<GUM_SCALAR> *query,
                                           NodeId node, BucketSet &pool,
                                           BucketSet &trash) {
      Set<const Instance<GUM_SCALAR> *> ignore, eliminated;
      Set<NodeId> delayedVars;
      // Downward elimination
      List<const Instance<GUM_SCALAR> *> elim_list;
      ignore.insert(query);

      for (auto iter = query->beginInvRef(); iter != query->endInvRef(); ++iter) {
        for (auto child = (*(iter.val())).begin(); child != (*(iter.val())).end();
             ++child) {
          if (not ignore.exists(child->first)) {
            __eliminateNodesDownward(query, child->first, pool, trash, elim_list,
                                     ignore, eliminated);
          } else if (not eliminated.exists(child->first)) {
            __addDelayedVariable(child->first, query, iter.key());
            delayedVars.insert(iter.key());
          }
        }
      }

      // Eliminating all nodes in query instance, except query
      InstanceBayesNet<GUM_SCALAR> bn(*query);
      DefaultTriangulation t(&(bn.moralGraph()), &(bn.modalities()));
      std::vector<NodeId> elim_order;
      VariableElimination<GUM_SCALAR> inf(bn);

      if (this->hasEvidence(query)) {
        __insertEvidence(query, pool);
      }

      for (auto attr = query->begin(); attr != query->end(); ++attr) {
        pool.insert(&(const_cast<Potential<GUM_SCALAR> &>((*(attr.val())).cpf())));
      }

      for (size_t idx = 0; idx < t.eliminationOrder().size(); ++idx) {
        if ((t.eliminationOrder()[idx] != node) and
            (not delayedVars.exists(t.eliminationOrder()[idx]))) {
          elim_order.push_back(t.eliminationOrder()[idx]);
        }
      }

      inf.eliminateNodes(elim_order, pool, trash);

      // Eliminating delayed variables, if any
      if (__delayedVariables.exists(query)) {
        __eliminateDelayedVariables(query, pool, trash);
      }

      eliminated.insert(query);
      // Eliminating instance in elim_list
      List<const Instance<GUM_SCALAR> *> tmp_list;

      while (not elim_list.empty()) {
        if (__checkElimOrder(query, elim_list.front())) {
          if (not ignore.exists(elim_list.front())) {
            __eliminateNodesDownward(query, elim_list.front(), pool, trash,
                                     elim_list, ignore, eliminated);
          }
        } else {
          tmp_list.insert(elim_list.front());
        }

        elim_list.popFront();
      }

      // Upward elimination
      for (const auto chain : query->type().slotChains())
        for (const auto parent : query->getInstances(chain->id()))
          if (not ignore.exists(parent))
            __eliminateNodesUpward(parent, pool, trash, tmp_list, ignore,
                                   eliminated);
    }

    template <typename GUM_SCALAR>
    void SVE<GUM_SCALAR>::__eliminateDelayedVariables(const Instance<GUM_SCALAR> *i,
                                                      BucketSet &pool,
                                                      BucketSet &trash) {
      Set<Potential<GUM_SCALAR> *> toRemove;

      for (const auto var : *__delayedVariables[i]) {
        MultiDimBucket<GUM_SCALAR> *bucket = new MultiDimBucket<GUM_SCALAR>();

        for (const auto pot : pool)
          if (pot->contains(*var)) {
            bucket->add(*pot);
            toRemove.insert(pot);
          }

        for (const auto pot : toRemove)
          pool.erase(pot);

        for (const auto other : bucket->allVariables())
          if (other != var)
            bucket->add(*other);

        Potential<GUM_SCALAR> *bucket_pot = new Potential<GUM_SCALAR>(bucket);
        trash.insert(bucket_pot);
        pool.insert(bucket_pot);
      }
    }

    template <typename GUM_SCALAR>
    void SVE<GUM_SCALAR>::__eliminateNodesDownward(
        const Instance<GUM_SCALAR> *from, const Instance<GUM_SCALAR> *i,
        BucketSet &pool, BucketSet &trash,
        List<const Instance<GUM_SCALAR> *> &elim_list,
        Set<const Instance<GUM_SCALAR> *> &ignore,
        Set<const Instance<GUM_SCALAR> *> &eliminated) {
      Set<NodeId> delayedVars;
      ignore.insert(i);
      // Calling elimination over child instance
      List<const Instance<GUM_SCALAR> *> my_list;

      for (auto iter = i->beginInvRef(); iter != i->endInvRef(); ++iter) {
        for (auto child = (*(iter.val())).begin(); child != (*(iter.val())).end();
             ++child) {
          if (not ignore.exists(child->first)) {
            __eliminateNodesDownward(i, child->first, pool, trash, my_list, ignore,
                                     eliminated);
          } else if (not eliminated.exists(child->first)) {
            __addDelayedVariable(child->first, i, iter.key());
            delayedVars.insert(iter.key());
          }
        }
      }

      // Eliminating all nodes in current instance
      __variableElimination(i, pool, trash,
                            (delayedVars.empty() ? 0 : &delayedVars));
      eliminated.insert(i);

      // Calling elimination over child's parents
      for (const auto node : my_list) {
        if (__checkElimOrder(i, node) and (node != from)) {
          if (not ignore.exists(node))
            __eliminateNodesDownward(i, node, pool, trash, elim_list, ignore,
                                     eliminated);
        } else if (node != from)
          elim_list.insert(node);
      }

      // Adding parents instance to elim_list
      for (const auto chain : i->type().slotChains())
        for (const auto inst : i->getInstances(chain->id()))
          if (inst != from)
            elim_list.insert(inst);
    }

    template <typename GUM_SCALAR>
    void SVE<GUM_SCALAR>::__variableElimination(const Instance<GUM_SCALAR> *i,
                                                BucketSet &pool, BucketSet &trash,
                                                Set<NodeId> *delayedVars) {
      if (this->hasEvidence(i)) {
        __eliminateNodesWithEvidence(i, pool, trash, delayedVars);
      } else {
        __insertLiftedNodes(i, pool, trash);

        for (const auto agg : i->type().aggregates())
          pool.insert(__getAggPotential(i, agg));

        try {
          InstanceBayesNet<GUM_SCALAR> bn(*i);
          VariableElimination<GUM_SCALAR> inf(bn);

          if (delayedVars) {
            std::vector<NodeId> elim;

            for (const auto node : __getElimOrder(i->type()))
              if (not delayedVars->exists(node))
                elim.push_back(node);

            inf.eliminateNodes(elim, pool, trash);
          } else {
            inf.eliminateNodes(__getElimOrder(i->type()), pool, trash);
          }
        } catch (NotFound &) {
          // Raised if there is no inner nodes to eliminate
        }
      }

      // Eliminating delayed variables, if any
      if (__delayedVariables.exists(i)) {
        __eliminateDelayedVariables(i, pool, trash);
      }
    }

    template <typename GUM_SCALAR>
    void SVE<GUM_SCALAR>::__eliminateNodesUpward(
        const Instance<GUM_SCALAR> *i, BucketSet &pool, BucketSet &trash,
        List<const Instance<GUM_SCALAR> *> &elim_list,
        Set<const Instance<GUM_SCALAR> *> &ignore,
        Set<const Instance<GUM_SCALAR> *> &eliminated) {
      // Downward elimination
      ignore.insert(i);

      for (auto iter = i->beginInvRef(); iter != i->endInvRef(); ++iter)
        for (auto child = (*(iter.val())).begin(); child != (*(iter.val())).end();
             ++child)
          if (not ignore.exists(child->first))
            __eliminateNodesDownward(i, child->first, pool, trash, elim_list, ignore,
                                     eliminated);

      // Eliminating all nodes in i instance
      __variableElimination(i, pool, trash);
      eliminated.insert(i);
      // Eliminating instance in elim_list
      List<const Instance<GUM_SCALAR> *> tmp_list;

      while (not elim_list.empty()) {
        if (__checkElimOrder(i, elim_list.front())) {
          if (not ignore.exists(elim_list.front())) {
            __eliminateNodesDownward(i, elim_list.front(), pool, trash, elim_list,
                                     ignore, eliminated);
          }
        } else {
          tmp_list.insert(elim_list.front());
        }

        elim_list.popFront();
      }

      // Upward elimination
      for (const auto chain : i->type().slotChains())
        for (const auto parent : i->getInstances(chain->id()))
          if (not ignore.exists(parent))
            __eliminateNodesUpward(parent, pool, trash, tmp_list, ignore,
                                   eliminated);
    }

    template <typename GUM_SCALAR>
    void SVE<GUM_SCALAR>::__eliminateNodesWithEvidence(const Instance<GUM_SCALAR> *i,
                                                       BucketSet &pool,
                                                       BucketSet &trash,
                                                       Set<NodeId> *delayedVars) {
      // First we check if evidences are on inner nodes
      bool inner = false;

      for (const auto &elt : this->evidence(i)) {
        inner = i->type().isInputNode(i->get(elt.first)) or
                i->type().isInnerNode(i->get(elt.first));

        if (inner) {
          break;
        }
      }

      // Evidence on inner nodes
      if (inner) {
        BucketSet tmp_pool;
        __insertEvidence(i, tmp_pool);

        // We need a local to not eliminate queried inner nodes of the same class
        for (const auto &elt : *i)
          tmp_pool.insert(&(elt.second->cpf()));

        InstanceBayesNet<GUM_SCALAR> bn(*i);
        DefaultTriangulation t(&(bn.moralGraph()), &(bn.modalities()));
        const std::vector<NodeId> &full_elim_order = t.eliminationOrder();
        VariableElimination<GUM_SCALAR> inf(bn);
        // Removing Output nodes of elimination order
        std::vector<NodeId> inner_elim_order;
        std::vector<NodeId> output_elim_order;

        if (delayedVars) {
          for (size_t idx = 0; idx < full_elim_order.size(); ++idx)
            if (not i->type().isOutputNode(i->get(full_elim_order[idx])))
              inner_elim_order.push_back(full_elim_order[idx]);
            else if (not delayedVars->exists(full_elim_order[idx]))
              output_elim_order.push_back(full_elim_order[idx]);
        } else {
          for (size_t idx = 0; idx < full_elim_order.size(); ++idx)
            if (not i->type().isOutputNode(i->get(full_elim_order[idx])))
              inner_elim_order.push_back(full_elim_order[idx]);
            else
              output_elim_order.push_back(full_elim_order[idx]);
        }

        inf.eliminateNodes(inner_elim_order, tmp_pool, trash);

        // Now we add the new potentials in pool and eliminate output nodes
        for (const auto pot : tmp_pool)
          pool.insert(pot);

        if (not output_elim_order.empty())
          inf.eliminateNodes(output_elim_order, pool, trash);
      } else {
        InstanceBayesNet<GUM_SCALAR> bn(*i);
        VariableElimination<GUM_SCALAR> inf(bn);
        __insertEvidence(i, pool);
        __insertLiftedNodes(i, pool, trash);

        for (const auto agg : i->type().aggregates())
          pool.insert(__getAggPotential(i, agg));

        try {
          if (delayedVars) {
            std::vector<NodeId> elim;

            for (auto iter = __getElimOrder(i->type()).begin();
                 iter != __getElimOrder(i->type()).end(); ++iter) {
              if (not delayedVars->exists(*iter)) {
                elim.push_back(*iter);
              }
            }

            inf.eliminateNodes(elim, pool, trash);
          } else {
            inf.eliminateNodes(__getElimOrder(i->type()), pool, trash);
          }
        } catch (NotFound &) {
          GUM_ERROR(FatalError, "there should be at least one node here.");
        }
      }
    }

    template <typename GUM_SCALAR>
    void SVE<GUM_SCALAR>::__insertLiftedNodes(const Instance<GUM_SCALAR> *i,
                                              BucketSet &pool, BucketSet &trash) {
      SVE<GUM_SCALAR>::BucketSet *lifted_pool = 0;

      try {
        lifted_pool = __lifted_pools[&(i->type())];
      } catch (NotFound &) {
        __initLiftedNodes(i->type());
        lifted_pool = __lifted_pools[&(i->type())];
      }

      for (const auto lifted_pot : *lifted_pool) {
        Potential<GUM_SCALAR> *pot = copyPotential(i->bijection(), *lifted_pot);
        pool.insert(pot);
        trash.insert(pot);
      }
    }

    template <typename GUM_SCALAR>
    void SVE<GUM_SCALAR>::__initLiftedNodes(const Class<GUM_SCALAR> &c) {
      BucketSet *lifted_pool = new BucketSet();
      __lifted_pools.insert(&c, lifted_pool);
      NodeSet inners, outers;

      for (const auto node : c.dag().nodes())
        if (ClassElement<GUM_SCALAR>::isAttribute(c.get(node))) {
          if (c.isOutputNode(c.get(node)))
            outers.insert(node);
          else if (not outers.exists(node))
            inners.insert(node);

          lifted_pool->insert(
              const_cast<Potential<GUM_SCALAR> *>(&(c.get(node).cpf())));
        } else if (ClassElement<GUM_SCALAR>::isAggregate(c.get(node))) {
          outers.insert(node);

          // We need to put in the output_elim_order aggregator's parents which are
          // innner nodes
          for (const auto par : c.dag().parents(node))
            if (ClassElement<GUM_SCALAR>::isAttribute(c.get(par)) and
                c.isInnerNode(c.get(par))) {
              inners.erase(par);
              outers.insert(par);
            }
        }

      // Now we proceed with the elimination of inner attributes
      ClassBayesNet<GUM_SCALAR> bn(c);
      List<NodeSet> partial_ordering;

      if (inners.size())
        partial_ordering.push_back(inners);

      if (outers.size())
        partial_ordering.push_back(outers);

      PartialOrderedTriangulation t(&(bn.moralGraph()), &(bn.modalities()),
                                    &partial_ordering);

      for (size_t idx = 0; idx < inners.size(); ++idx)
        eliminateNode(&(c.get(t.eliminationOrder()[idx]).type().variable()),
                      *lifted_pool, __lifted_trash);

      // If there is not only inner and input Attributes
      if (outers.size()) {
        __elim_orders.insert(
            &c, new std::vector<NodeId>(t.eliminationOrder().begin() + inners.size(),
                                        t.eliminationOrder().end()));
      }
    }

    template <typename GUM_SCALAR> void SVE<GUM_SCALAR>::__initElimOrder() {
      ClassDependencyGraph<GUM_SCALAR> cdg(*(this->_prm));
      __class_elim_order = new Sequence<const ClassElementContainer<GUM_SCALAR> *>();
      std::list<NodeId> l;

      for (const auto node : cdg.dag().nodes())
        if (cdg.dag().parents(node).empty())
          l.push_back(node);

      Set<NodeId> visited_node;

      while (not l.empty()) {
        visited_node.insert(l.front());

        if (not __class_elim_order->exists(cdg.get(l.front()).first))
          __class_elim_order->insert(cdg.get(l.front()).first);

        for (const auto child : cdg.dag().children(l.front()))
          if (not visited_node.contains(child))
            l.push_back(child);

        l.pop_front();
      }
    }

    template <typename GUM_SCALAR>
    void SVE<GUM_SCALAR>::_marginal(const Chain &chain, Potential<GUM_SCALAR> &m) {
      const Instance<GUM_SCALAR> *i = chain.first;
      const Attribute<GUM_SCALAR> *elt = chain.second;
      SVE<GUM_SCALAR>::BucketSet pool, trash;

      __eliminateNodes(i, elt->id(), pool, trash);
      m.fill((GUM_SCALAR)1);

      for (const auto pot : pool)
        if (pot->contains(elt->type().variable()))
          m.multiplicateBy(*pot);

      m.normalize();

      for (const auto pot : trash)
        delete pot;
    }

    template <typename GUM_SCALAR>
    void SVE<GUM_SCALAR>::_joint(const std::vector<Chain> &queries,
                                 Potential<GUM_SCALAR> &j) {
      GUM_ERROR(FatalError, "Not implemented.");
    }

    template <typename GUM_SCALAR>
    INLINE SVE<GUM_SCALAR>::SVE(const PRM<GUM_SCALAR> &prm,
                                const System<GUM_SCALAR> &system)
        : PRMInference<GUM_SCALAR>(prm, system), __class_elim_order(0) {
      GUM_CONSTRUCTOR(SVE);
    }

    template <typename GUM_SCALAR>
    INLINE void SVE<GUM_SCALAR>::__insertEvidence(const Instance<GUM_SCALAR> *i,
                                                  BucketSet &pool) {
      for (const auto &elt : this->evidence(i))
        pool.insert(const_cast<Potential<GUM_SCALAR> *>(elt.second));
    }

    template <typename GUM_SCALAR>
    INLINE std::vector<NodeId> &
    SVE<GUM_SCALAR>::__getElimOrder(const Class<GUM_SCALAR> &c) {
      return *(__elim_orders[&c]);
    }

    template <typename GUM_SCALAR>
    INLINE bool
    SVE<GUM_SCALAR>::__checkElimOrder(const Instance<GUM_SCALAR> *first,
                                      const Instance<GUM_SCALAR> *second) {
      if (__class_elim_order == 0) {
        __initElimOrder();
      }

      return (__class_elim_order->pos(&(first->type())) <=
              __class_elim_order->pos(&(second->type())));
    }

    template <typename GUM_SCALAR>
    INLINE Potential<GUM_SCALAR> *
    SVE<GUM_SCALAR>::__getAggPotential(const Instance<GUM_SCALAR> *i,
                                       const Aggregate<GUM_SCALAR> *agg) {
      return &(const_cast<Potential<GUM_SCALAR> &>(i->get(agg->id()).cpf()));
    }

    template <typename GUM_SCALAR>
    INLINE void SVE<GUM_SCALAR>::_evidenceAdded(const Chain &chain) {
      // Do nothing
    }

    template <typename GUM_SCALAR>
    INLINE void SVE<GUM_SCALAR>::_evidenceRemoved(const Chain &chain) {
      // Do nothing
    }

    template <typename GUM_SCALAR>
    INLINE void SVE<GUM_SCALAR>::__addDelayedVariable(const Instance<GUM_SCALAR> *i,
                                                      const Instance<GUM_SCALAR> *j,
                                                      NodeId id) {
      try {
        __delayedVariables[i]->insert(&(j->get(id).type().variable()));
      } catch (NotFound &) {
        __delayedVariables.insert(i, new Set<const DiscreteVariable *>());
        __delayedVariables[i]->insert(&(j->get(id).type().variable()));
      } catch (DuplicateElement &) {
        // happends if j->get(id) is parent of more than one variable in i
      }

      static std::string dot = ".";

      try {
        __delayedVariablesCounters[i->name() + dot + i->get(id).safeName()] += 1;
      } catch (NotFound &) {
        __delayedVariablesCounters.insert(i->name() + dot + i->get(id).safeName(),
                                          1);
      }
    }

    template <typename GUM_SCALAR> INLINE std::string SVE<GUM_SCALAR>::name() const {
      return "SVE";
    }

  } /* namespace prm */
} /* namespace gum */
