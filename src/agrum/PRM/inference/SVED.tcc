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
 * @brief Inline implementation of SVED.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <agrum/PRM/inference/SVED.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR> SVED<GUM_SCALAR>::~SVED() {
      GUM_DESTRUCTOR(SVED);

      for (const auto &elt : __elim_orders)
        delete elt.second;

      if (__class_elim_order != nullptr)
        delete __class_elim_order;
    }

    template <typename GUM_SCALAR>
    void SVED<GUM_SCALAR>::__eliminateNodes(const Instance<GUM_SCALAR> *query,
                                            NodeId node, BucketSet &pool,
                                            BucketSet &trash) {
      Set<const Instance<GUM_SCALAR> *> ignore;
      ignore.insert(query);
      // Extracting required attributes and slotchains
      Set<NodeId> &attr_set = __getAttrSet(query);
      Set<NodeId> &sc_set = __getSCSet(query);
      // Downward elimination
      List<const Instance<GUM_SCALAR> *> elim_list;

      for (const auto attr : attr_set) {
        try {
          for (auto iter = query->getRefAttr(attr).begin();
               iter != query->getRefAttr(attr).end(); ++iter)
            if ((not ignore.exists(iter->first)) and (__bb.exists(iter->first)))
              __eliminateNodesDownward(query, iter->first, pool, trash, elim_list,
                                       ignore);
        } catch (NotFound &) {
          // Ok
        }
      }

      // Eliminating all nodes in query instance, except query
      InstanceBayesNet<GUM_SCALAR> bn(*query);
      DefaultTriangulation t(&(bn.moralGraph()), &(bn.modalities()));
      std::vector<NodeId> elim_order;
      VariableElimination<GUM_SCALAR> inf(bn);

      if (this->hasEvidence(query))
        __insertEvidence(query, pool);

      for (const auto attr : attr_set)
        pool.insert(&(const_cast<Potential<GUM_SCALAR> &>(query->get(attr).cpf())));

      for (size_t idx = 0; idx < t.eliminationOrder().size(); ++idx)
        if (t.eliminationOrder()[idx] != node)
          elim_order.push_back(t.eliminationOrder()[idx]);

      inf.eliminateNodes(elim_order, pool, trash);
      // Eliminating instance in elim_list
      List<const Instance<GUM_SCALAR> *> tmp_list;
      __reduceElimList(query, elim_list, tmp_list, ignore, pool, trash);

      while (not elim_list.empty()) {
        if (__checkElimOrder(query, elim_list.front())) {
          if ((not ignore.exists(elim_list.front())) and
              (__bb.exists(elim_list.front())))
            __eliminateNodesDownward(query, elim_list.front(), pool, trash,
                                     elim_list, ignore);
        } else if (__bb.exists(elim_list.front())) {
          tmp_list.insert(elim_list.front());
        }

        elim_list.popFront();
      }

      // Upward elimination
      for (const auto chain : sc_set)
        for (const auto parent : query->getInstances(chain))
          if ((not ignore.exists(parent)) and (__bb.exists(*parent)))
            __eliminateNodesUpward(parent, pool, trash, tmp_list, ignore);
    }

    template <typename GUM_SCALAR>
    void SVED<GUM_SCALAR>::__eliminateNodesDownward(
        const Instance<GUM_SCALAR> *from, const Instance<GUM_SCALAR> *i,
        BucketSet &pool, BucketSet &trash,
        List<const Instance<GUM_SCALAR> *> &elim_list,
        Set<const Instance<GUM_SCALAR> *> &ignore) {
      ignore.insert(i);
      // Extracting required attributes and slotchains
      Set<NodeId> &attr_set = __getAttrSet(i);
      Set<NodeId> &sc_set = __getSCSet(i);
      // Calling elimination over child instance
      List<const Instance<GUM_SCALAR> *> my_list;

      for (const auto attr : attr_set) {
        try {
          for (auto iter = i->getRefAttr(attr).begin();
               iter != i->getRefAttr(attr).end(); ++iter)
            if ((not ignore.exists(iter->first)) and (__bb.exists(iter->first)))
              __eliminateNodesDownward(i, iter->first, pool, trash, my_list, ignore);
        } catch (NotFound &) {
          // Ok
        }
      }

      // Eliminating all nodes in current instance
      if (this->hasEvidence(i)) {
        __eliminateNodesWithEvidence(i, pool, trash);
      } else {
        __insertLiftedNodes(i, pool, trash);

        for (const auto agg : i->type().aggregates())
          if (__bb.requisiteNodes(i).exists(agg->id()))
            pool.insert(__getAggPotential(i, agg));

        try {
          InstanceBayesNet<GUM_SCALAR> bn(*i);
          VariableElimination<GUM_SCALAR> inf(bn);
          inf.eliminateNodes(__getElimOrder(i->type()), pool, trash);
        } catch (NotFound &) {
          // Raised if there is no inner nodes to eliminate
        }
      }

      // Calling elimination over child's parents
      while (not my_list.empty()) {
        if (__checkElimOrder(i, my_list.front())) {
          if ((not ignore.exists(my_list.front())) and
              (__bb.exists(my_list.front())))
            __eliminateNodesDownward(i, my_list.front(), pool, trash, my_list,
                                     ignore);
        } else if (__bb.exists(my_list.front())) {
          elim_list.insert(my_list.front());
        }

        my_list.popFront();
      }

      // Adding parents instance to elim_list
      for (const auto chain : sc_set)
        for (const auto parent : i->getInstances(chain))
          if ((not ignore.exists(parent)) and __bb.exists(parent) and
              (parent != from))
            elim_list.insert(parent);
    }

    template <typename GUM_SCALAR>
    void SVED<GUM_SCALAR>::__eliminateNodesUpward(
        const Instance<GUM_SCALAR> *i, BucketSet &pool, BucketSet &trash,
        List<const Instance<GUM_SCALAR> *> &elim_list,
        Set<const Instance<GUM_SCALAR> *> &ignore) {
      ignore.insert(i);
      // Extracting required attributes and slotchains
      Set<NodeId> &attr_set = __getAttrSet(i);
      Set<NodeId> &sc_set = __getSCSet(i);

      // Downward elimination
      for (const auto attr : attr_set) {
        try {
          for (auto iter = i->getRefAttr(attr).begin();
               iter != i->getRefAttr(attr).end(); ++iter)
            if ((not ignore.exists(iter->first)) and (__bb.exists(iter->first)))
              __eliminateNodesDownward(i, iter->first, pool, trash, elim_list,
                                       ignore);
        } catch (NotFound &) {
          // Ok
        }
      }

      // Eliminating all nodes in i instance
      if (this->hasEvidence(i)) {
        __eliminateNodesWithEvidence(i, pool, trash);
      } else {
        __insertLiftedNodes(i, pool, trash);

        for (const auto agg : i->type().aggregates())
          if (__bb.requisiteNodes(i).exists(agg->id()))
            pool.insert(__getAggPotential(i, agg));

        try {
          InstanceBayesNet<GUM_SCALAR> bn(*i);
          VariableElimination<GUM_SCALAR> inf(bn);
          inf.eliminateNodes(__getElimOrder(i->type()), pool, trash);
        } catch (NotFound &) {
          // Raised if there is no inner nodes to eliminate
        }
      }

      // Eliminating instance in elim_list
      List<const Instance<GUM_SCALAR> *> tmp_list;

      while (not elim_list.empty()) {
        if (__checkElimOrder(i, elim_list.front())) {
          if ((not ignore.exists(elim_list.front())) and
              (__bb.exists(elim_list.front())))
            __eliminateNodesDownward(i, elim_list.front(), pool, trash, elim_list,
                                     ignore);
        } else if (__bb.exists(elim_list.front())) {
          ignore.insert(elim_list.front());
        }

        elim_list.popFront();
      }

      // Upward elimination
      for (const auto chain : sc_set)
        for (const auto parent : i->getInstances(chain))
          if ((not ignore.exists(parent)) and (__bb.exists(parent)))
            __eliminateNodesUpward(parent, pool, trash, tmp_list, ignore);
    }

    template <typename GUM_SCALAR>
    void SVED<GUM_SCALAR>::__eliminateNodesWithEvidence(
        const Instance<GUM_SCALAR> *i, BucketSet &pool, BucketSet &trash) {
      // Adding required evidences
      for (const auto &elt : this->evidence(i))
        if (__bb.requisiteNodes(i).exists(elt.first))
          pool.insert(const_cast<Potential<GUM_SCALAR> *>(elt.second));

      // Adding potentials and eliminating the remaining nodes
      for (const auto &a : *i)
        if (__bb.requisiteNodes(i).exists(a.first))
          pool.insert(&(a.second->cpf()));

      InstanceBayesNet<GUM_SCALAR> bn(*i);
      DefaultTriangulation t(&(bn.moralGraph()), &(bn.modalities()));
      const std::vector<NodeId> &full_elim_order = t.eliminationOrder();

      for (auto var = full_elim_order.begin(); var != full_elim_order.end(); ++var)
        eliminateNode(&(i->get(*var).type().variable()), pool, trash);
    }

    template <typename GUM_SCALAR>
    void SVED<GUM_SCALAR>::__insertLiftedNodes(const Instance<GUM_SCALAR> *i,
                                               BucketSet &pool, BucketSet &trash) {
      BucketSet *lifted_pool = nullptr;

      try {
        lifted_pool = __lifted_pools[&(__bb.requisiteNodes(i))];
      } catch (NotFound &) {
        __initLiftedNodes(i, trash);
        lifted_pool = __lifted_pools[&(__bb.requisiteNodes(i))];
      }

      for (const auto lifted_pot : *lifted_pool) {
        Potential<GUM_SCALAR> *pot = copyPotential(i->bijection(), *lifted_pot);
        pool.insert(pot);
        trash.insert(pot);
      }
    }

    template <typename GUM_SCALAR>
    void SVED<GUM_SCALAR>::__initLiftedNodes(const Instance<GUM_SCALAR> *i,
                                             BucketSet &trash) {
      Class<GUM_SCALAR> &c = const_cast<Class<GUM_SCALAR> &>(i->type());
      BucketSet *lifted_pool = new BucketSet();
      __lifted_pools.insert(&(__bb.requisiteNodes(i)), lifted_pool);

      for (const auto node : __bb.requisiteNodes(i))
        if (ClassElement<GUM_SCALAR>::isAttribute(c.get(node)))
          lifted_pool->insert(
              const_cast<Potential<GUM_SCALAR> *>(&(c.get(node).cpf())));

      NodeSet inners, outers, ignore;

      for (const auto &elt : *i) {
        if (__bb.requisiteNodes(*i).exists(elt.first)) {
          if (ClassElement<GUM_SCALAR>::isAttribute(c.get(elt.first))) {
            if (c.isOutputNode(c.get(elt.first)))
              outers.insert(elt.first);
            else if (not outers.exists(elt.first))
              inners.insert(elt.first);
          } else if (ClassElement<GUM_SCALAR>::isAggregate(c.get(elt.first))) {
            outers.insert(elt.first);

            // We need to put in the output_elim_order aggregator's parents which are
            // innner nodes
            for (const auto par : c.dag().parents(elt.first))
              if (ClassElement<GUM_SCALAR>::isAttribute(i->type().get(par)) and
                  i->type().isInnerNode(i->type().get(par)) and
                  __bb.requisiteNodes(i).exists(par)) {
                inners.erase(par);
                outers.insert(par);
              }
          }
        } else {
          ignore.insert(elt.first);
        }
      }

      // Now we proceed with the elimination of inner attributes
      ClassBayesNet<GUM_SCALAR> bn(c);
      List<NodeSet> partial_ordering;

      if (inners.size())
        partial_ordering.pushBack(inners);

      if (outers.size())
        partial_ordering.pushBack(outers);

      if (ignore.size())
        partial_ordering.pushBack(ignore);

      GUM_ASSERT(inners.size() or outers.size());
      PartialOrderedTriangulation t(&(bn.moralGraph()), &(bn.modalities()),
                                    &partial_ordering);

      for (size_t idx = 0; idx < inners.size(); ++idx)
        eliminateNode(&(c.get(t.eliminationOrder()[idx]).type().variable()),
                      *lifted_pool, trash);

      // If there is not only inner and input Attributes
      if (outers.size()) {
        __elim_orders.insert(
            &c, new std::vector<NodeId>(t.eliminationOrder().begin() + inners.size(),
                                        t.eliminationOrder().end()));
      }
    }

    template <typename GUM_SCALAR> void SVED<GUM_SCALAR>::__initElimOrder() {
      ClassDependencyGraph<GUM_SCALAR> cdg(*this->_prm);
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
    void SVED<GUM_SCALAR>::_marginal(const Chain &chain, Potential<GUM_SCALAR> &m) {
      const Instance<GUM_SCALAR> *i = chain.first;
      const Attribute<GUM_SCALAR> *elt = chain.second;
      SVED<GUM_SCALAR>::BucketSet pool, trash;
      __bb.compute(i, elt->id());
      __eliminateNodes(i, elt->id(), pool, trash);
      m.fill((GUM_SCALAR)1);

      for (const auto pot : pool) {
        if (pot->contains(*(m.variablesSequence().atPos(0))))
          m.multiplicateBy(*pot);

        if (pot->nbrDim() > 1) {
          std::string dot = ".";
          std::string t = "true";
          std::string f = "false";
          std::string none = "NONE";
          GUM_TRACE_VAR(pot->nbrDim());
          GUM_TRACE(chain.first->name() + dot + chain.second->safeName());

          for (auto jter = this->_sys->begin(); jter != this->_sys->end(); ++jter) {
            for (auto a = (*(jter.val())).begin(); a != (*(jter.val())).end(); ++a) {
              if (pot->contains((*(a.val())).type().variable())) {
                GUM_TRACE((*(jter.val())).name() + dot + (*(a.val())).safeName());

                if (__bb.exists(jter.val())) {
                  GUM_TRACE("should be eliminated");
                } else {
                  GUM_TRACE("should not be here");
                }

                // try {
                //   std::cerr << "Instance<GUM_SCALAR> eliminated in:       " <<
                //   __debug[*jter] << std::endl;
                // } catch (NotFound&) {
                //   std::cerr << "Instance<GUM_SCALAR> eliminated in:       " <<
                //   none << std::endl;
                // }
                try {
                  std::cerr << "Instance<GUM_SCALAR> has requisite nodes: "
                            << __bb.requisiteNodes(jter.val()).size() << std::endl;
                } catch (NotFound &) {
                  std::cerr << "Instance<GUM_SCALAR> has requisite nodes: " << none
                            << std::endl;
                }

                std::cerr << "Attribute<GUM_SCALAR> has refered attr:   "
                          << ((*(jter.val())).hasRefAttr((*(a.val())).id()))
                          << std::endl;

                if ((*(jter.val())).hasRefAttr((*(a.val())).id())) {
                  bool foo = false;

                  const auto &refs = (*(jter.val())).getRefAttr((*(a.val())).id());

                  for (auto ref = refs.begin(); ref != refs.end(); ++ref) {
                    if (__bb.exists(ref->first) and
                        __bb.requisiteNodes(ref->first)
                            .exists(ref->first->type().get(ref->second).id())) {
                      foo = true;
                      break;
                    }
                  }

                  if (foo)
                    std::cerr << "Attribute<GUM_SCALAR> has required child: " << t
                              << std::endl;
                  else
                    std::cerr << "Attribute<GUM_SCALAR> has required child: " << f
                              << std::endl;
                }

                try {
                  std::cerr << "Attribute<GUM_SCALAR> is required:        "
                            << (__bb.requisiteNodes(jter.val())
                                    .exists((*(a.val())).id()));
                } catch (NotFound &) {
                  std::cerr << "Attribute<GUM_SCALAR> is required:        " << none;
                }

                std::cerr << std::endl;
                std::cerr << "Attribute<GUM_SCALAR> has evidence:       "
                          << (this->hasEvidence(jter.val()));
                std::cerr << std::endl;
                std::cerr << "Attribute<GUM_SCALAR> parents number:     "
                          << ((*(jter.val()))
                                  .type()
                                  .dag()
                                  .parents((*(a.val())).id())
                                  .size()) << std::endl;
                std::cerr << "Attribute<GUM_SCALAR> children number:    "
                          << ((*(jter.val()))
                                  .type()
                                  .dag()
                                  .children((*(a.val())).id())
                                  .size()) << std::endl;
              }
            }
          }
        }
      }

      m.normalize();

      // cleaning up the mess
      for (const auto pot : trash)
        delete pot;

      for (const auto &elt : __lifted_pools)
        delete elt.second;

      __lifted_pools.clear();

      for (const auto &elt : __req_set) {
        delete elt.second.first;
        delete elt.second.second;
      }

      __req_set.clear();

      for (const auto elt : __elim_orders)
        delete elt.second;

      __elim_orders.clear();
    }

    template <typename GUM_SCALAR>
    void SVED<GUM_SCALAR>::_joint(const std::vector<Chain> &queries,
                                  Potential<GUM_SCALAR> &j) {
      GUM_ERROR(FatalError, "Not implemented.");
    }

    template <typename GUM_SCALAR>
    void SVED<GUM_SCALAR>::__initReqSets(const Instance<GUM_SCALAR> *i) {
      Set<NodeId> *attr_set = new Set<NodeId>();
      Set<NodeId> *sc_set = new Set<NodeId>();

      for (const auto node : __bb.requisiteNodes(i)) {
        switch (i->type().get(node).elt_type()) {
          case ClassElement<GUM_SCALAR>::prm_aggregate:
          case ClassElement<GUM_SCALAR>::prm_attribute: {
            attr_set->insert(node);
            break;
          }

          case ClassElement<GUM_SCALAR>::prm_slotchain: {
            sc_set->insert(node);
            break;
          }

          default: {
            GUM_ERROR(FatalError, "There should not be elements other"
                                  " than Attribute<GUM_SCALAR> and SlotChain.");
          }
        }
      }

      __req_set.insert(&(__bb.requisiteNodes(i)),
                       std::pair<Set<NodeId> *, Set<NodeId> *>(attr_set, sc_set));
    }

    template <typename GUM_SCALAR>
    INLINE SVED<GUM_SCALAR>::SVED(const PRM<GUM_SCALAR> &prm,
                                  const System<GUM_SCALAR> &model)
        : PRMInference<GUM_SCALAR>(prm, model), __class_elim_order(0), __bb(*this) {
      GUM_CONSTRUCTOR(SVED);
    }

    template <typename GUM_SCALAR>
    INLINE SVED<GUM_SCALAR>::SVED(const PRM<GUM_SCALAR> &prm,
                                  const std::string &model)
        : PRMInference<GUM_SCALAR>(prm, model), __class_elim_order(0), __bb(*this) {
      GUM_CONSTRUCTOR(SVED);
    }

    template <typename GUM_SCALAR>
    INLINE void SVED<GUM_SCALAR>::__insertEvidence(const Instance<GUM_SCALAR> *i,
                                                   BucketSet &pool) {
      for (const auto &elt : this->evidence(i))
        pool.insert(const_cast<Potential<GUM_SCALAR> *>(elt.second));
    }

    template <typename GUM_SCALAR>
    INLINE std::vector<NodeId> &
    SVED<GUM_SCALAR>::__getElimOrder(const Class<GUM_SCALAR> &c) {
      return *(__elim_orders[&c]);
    }

    template <typename GUM_SCALAR>
    INLINE bool
    SVED<GUM_SCALAR>::__checkElimOrder(const Instance<GUM_SCALAR> *first,
                                       const Instance<GUM_SCALAR> *second) {
      if (__class_elim_order == 0) {
        __initElimOrder();
      }

      return (__class_elim_order->pos(&(first->type())) <=
              __class_elim_order->pos(&(second->type())));
    }

    template <typename GUM_SCALAR>
    INLINE Potential<GUM_SCALAR> *
    SVED<GUM_SCALAR>::__getAggPotential(const Instance<GUM_SCALAR> *i,
                                        const Aggregate<GUM_SCALAR> *agg) {
      return &(const_cast<Potential<GUM_SCALAR> &>(i->get(agg->safeName()).cpf()));
    }

    template <typename GUM_SCALAR>
    INLINE void
    SVED<GUM_SCALAR>::_evidenceAdded(const SVED<GUM_SCALAR>::Chain &chain) {
      // Do nothing
    }

    template <typename GUM_SCALAR>
    INLINE void
    SVED<GUM_SCALAR>::_evidenceRemoved(const SVED<GUM_SCALAR>::Chain &chain) {
      // Do nothing
    }

    template <typename GUM_SCALAR>
    INLINE Set<NodeId> &
    SVED<GUM_SCALAR>::__getAttrSet(const Instance<GUM_SCALAR> *i) {
      try {
        return *(__req_set[&(__bb.requisiteNodes(i))].first);
      } catch (NotFound &) {
        __initReqSets(i);
        return *(__req_set[&(__bb.requisiteNodes(i))].first);
      }
    }

    template <typename GUM_SCALAR>
    INLINE Set<NodeId> &SVED<GUM_SCALAR>::__getSCSet(const Instance<GUM_SCALAR> *i) {
      try {
        return *(__req_set[&(__bb.requisiteNodes(i))].second);
      } catch (NotFound &) {
        __initReqSets(i);
        return *(__req_set[&(__bb.requisiteNodes(i))].second);
      }
    }

    template <typename GUM_SCALAR>
    INLINE void SVED<GUM_SCALAR>::__reduceElimList(
        const Instance<GUM_SCALAR> *i, List<const Instance<GUM_SCALAR> *> &elim_list,
        List<const Instance<GUM_SCALAR> *> &reduced_list,
        Set<const Instance<GUM_SCALAR> *> &ignore, BucketSet &pool,
        BucketSet &trash) {
      while (not elim_list.empty()) {
        if (__checkElimOrder(i, elim_list.front())) {
          if ((not ignore.exists(elim_list.front())) and
              (__bb.exists(elim_list.front()))) {
            __eliminateNodesDownward(i, elim_list.front(), pool, trash, elim_list,
                                     ignore);
          }
        } else if (__bb.exists(elim_list.front())) {
          reduced_list.insert(elim_list.front());
        }

        elim_list.popFront();
      }
    }

    template <typename GUM_SCALAR>
    INLINE std::string SVED<GUM_SCALAR>::name() const {
      return "SVED";
    }

  } /* namespace prm */
} /* namespace gum */
