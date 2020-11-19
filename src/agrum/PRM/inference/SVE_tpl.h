/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Inline implementation of SVE.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <agrum/PRM/classDependencyGraph.h>
#include <agrum/PRM/inference/SVE.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    std::string __print_attribute__(const PRMInstance< GUM_SCALAR >&  i,
                                    const PRMAttribute< GUM_SCALAR >& a) {
      std::stringstream s;
      const auto&       class_a = i.type().get(a.safeName());
      s << &(a.type().variable()) << " - ";
      s << i.name() << "." << a.safeName()
        << ": input=" << i.type().isInputNode(class_a);
      s << " output=" << i.type().isOutputNode(class_a)
        << " inner=" << i.type().isInnerNode(class_a);
      return s.str();
    }

    template < typename GUM_SCALAR >
    std::string __print_instance__(const PRMInstance< GUM_SCALAR >& i) {
      std::stringstream s;
      s << i.name() << std::endl;
      s << "Attributes: " << std::endl;
      for (auto a: i) {
        s << __print_attribute__(i, *(a.second));
      }
      if (i.type().slotChains().size()) {
        s << std::endl << "SlotChains: " << std::endl;
        for (auto sc: i.type().slotChains()) {
          s << sc->name() << " ";
        }
      }
      return s.str();
    }

    template < typename GUM_SCALAR >
    std::string __print_system__(const PRMSystem< GUM_SCALAR >& s) {
      std::stringstream str;
      for (auto i: s) {
        str << __print_instance__(*(i.second)) << std::endl;
      }
      return str.str();
    }

    template < typename LIST >
    std::string __print_list__(LIST l) {
      std::stringstream s;
      s << "[";
      for (auto i: l) {
        s << i->name() << " ";
      }
      s << "]";
      return s.str();
    }

    template < typename GUM_SCALAR >
    std::string __print_pot__(const Potential< GUM_SCALAR >& pot) {
      std::stringstream s;
      s << "{";
      for (auto var: pot.variablesSequence()) {
        s << var << ", ";
      }
      s << "}";
      return s.str();
    }

    template < typename SET >
    std::string __print_set__(SET set) {
      std::stringstream s;
      s << "[";
      for (auto p: set) {
        s << __print_pot__(*p) << " ";
      }
      s << "]";
      return s.str();
    }

    template < typename GUM_SCALAR >
    SVE< GUM_SCALAR >::~SVE() {
      GUM_DESTRUCTOR(SVE);

      for (const auto& elt: elim_orders__)
        delete elt.second;

      for (const auto& elt: lifted_pools__)
        delete elt.second;

      if (class_elim_order__ != nullptr) delete class_elim_order__;

      for (const auto trash: lifted_trash__)
        delete trash;

      for (auto set: delayedVariables__)
        delete set.second;
    }

    template < typename GUM_SCALAR >
    void
       SVE< GUM_SCALAR >::eliminateNodes__(const PRMInstance< GUM_SCALAR >* query,
                                           NodeId                           node,
                                           BucketSet&                       pool,
                                           BucketSet& trash) {
      Set< const PRMInstance< GUM_SCALAR >* > ignore, eliminated;
      Set< NodeId >                           delayedVars;
      // Downward elimination
      List< const PRMInstance< GUM_SCALAR >* > elim_list;
      ignore.insert(query);

      for (auto iter = query->beginInvRef(); iter != query->endInvRef(); ++iter) {
        for (auto child = (*(iter.val())).begin(); child != (*(iter.val())).end();
             ++child) {
          if (!ignore.exists(child->first)) {
            eliminateNodesDownward__(
               query, child->first, pool, trash, elim_list, ignore, eliminated);
          } else if (!eliminated.exists(child->first)) {
            addDelayedVariable__(child->first, query, iter.key());
            delayedVars.insert(iter.key());
          }
        }
      }

      // Eliminating all nodes in query instance, except query
      InstanceBayesNet< GUM_SCALAR > bn(*query);
      DefaultTriangulation           t(&(bn.moralGraph()), &(bn.modalities()));
      std::vector< const DiscreteVariable* > elim_order;

      if (this->hasEvidence(query)) { insertEvidence__(query, pool); }

      for (auto attr = query->begin(); attr != query->end(); ++attr) {
        pool.insert(
           &(const_cast< Potential< GUM_SCALAR >& >((*(attr.val())).cpf())));
      }

      for (size_t idx = 0; idx < t.eliminationOrder().size(); ++idx) {
        if ((t.eliminationOrder()[idx] != node)
            && (!delayedVars.exists(t.eliminationOrder()[idx]))) {
          auto        var_id = t.eliminationOrder()[idx];
          const auto& var = bn.variable(var_id);
          elim_order.push_back(&var);
        }
      }

      eliminateNodes(elim_order, pool, trash);

      // Eliminating delayed variables, if any
      if (delayedVariables__.exists(query)) {
        eliminateDelayedVariables__(query, pool, trash);
      }

      eliminated.insert(query);
      // Eliminating instance in elim_list
      List< const PRMInstance< GUM_SCALAR >* > tmp_list;

      while (!elim_list.empty()) {
        if (checkElimOrder__(query, elim_list.front())) {
          if (!ignore.exists(elim_list.front())) {
            eliminateNodesDownward__(query,
                                     elim_list.front(),
                                     pool,
                                     trash,
                                     elim_list,
                                     ignore,
                                     eliminated);
          }
        } else {
          tmp_list.insert(elim_list.front());
        }

        elim_list.popFront();
      }

      // Upward elimination
      for (const auto chain: query->type().slotChains())
        for (const auto parent: query->getInstances(chain->id()))
          if (!ignore.exists(parent))
            eliminateNodesUpward__(
               parent, pool, trash, tmp_list, ignore, eliminated);
    }

    template < typename GUM_SCALAR >
    void SVE< GUM_SCALAR >::eliminateDelayedVariables__(
       const PRMInstance< GUM_SCALAR >* i, BucketSet& pool, BucketSet& trash) {
      Set< Potential< GUM_SCALAR >* > toRemove;

      for (const auto var: *delayedVariables__[i]) {
        MultiDimBucket< GUM_SCALAR >* bucket = new MultiDimBucket< GUM_SCALAR >();

        for (const auto pot: pool)
          if (pot->contains(*var)) {
            bucket->add(*pot);
            toRemove.insert(pot);
          }

        for (const auto pot: toRemove)
          pool.erase(pot);

        for (const auto other: bucket->allVariables())
          if (other != var) bucket->add(*other);

        Potential< GUM_SCALAR >* bucket_pot = new Potential< GUM_SCALAR >(bucket);
        trash.insert(bucket_pot);
        pool.insert(bucket_pot);
      }
    }

    template < typename GUM_SCALAR >
    void SVE< GUM_SCALAR >::eliminateNodesDownward__(
       const PRMInstance< GUM_SCALAR >*          from,
       const PRMInstance< GUM_SCALAR >*          i,
       BucketSet&                                pool,
       BucketSet&                                trash,
       List< const PRMInstance< GUM_SCALAR >* >& elim_list,
       Set< const PRMInstance< GUM_SCALAR >* >&  ignore,
       Set< const PRMInstance< GUM_SCALAR >* >&  eliminated) {
      Set< NodeId > delayedVars;
      ignore.insert(i);
      // Calling elimination over child instance
      List< const PRMInstance< GUM_SCALAR >* > my_list;

      for (auto iter = i->beginInvRef(); iter != i->endInvRef(); ++iter) {
        for (auto child = (*(iter.val())).begin(); child != (*(iter.val())).end();
             ++child) {
          if (!ignore.exists(child->first)) {
            eliminateNodesDownward__(
               i, child->first, pool, trash, my_list, ignore, eliminated);
          } else if (!eliminated.exists(child->first)) {
            addDelayedVariable__(child->first, i, iter.key());
            delayedVars.insert(iter.key());
          }
        }
      }

      // Eliminating all nodes in current instance
      variableElimination__(
         i, pool, trash, (delayedVars.empty() ? 0 : &delayedVars));
      eliminated.insert(i);

      // Calling elimination over child's parents
      for (const auto node: my_list) {
        if (checkElimOrder__(i, node) && (node != from)) {
          if (!ignore.exists(node)) {
            eliminateNodesDownward__(
               i, node, pool, trash, elim_list, ignore, eliminated);
          }
        } else if (node != from) {
          elim_list.insert(node);
        }
      }

      // Adding parents instance to elim_list
      for (const auto chain: i->type().slotChains()) {
        for (const auto inst: i->getInstances(chain->id())) {
          if (inst != from) { elim_list.insert(inst); }
        }
      }
    }

    template < typename GUM_SCALAR >
    void
       SVE< GUM_SCALAR >::variableElimination__(const PRMInstance< GUM_SCALAR >* i,
                                                BucketSet&     pool,
                                                BucketSet&     trash,
                                                Set< NodeId >* delayedVars) {
      if (this->hasEvidence(i)) {
        eliminateNodesWithEvidence__(i, pool, trash, delayedVars);
      } else {
        insertLiftedNodes__(i, pool, trash);

        for (const auto agg: i->type().aggregates())
          pool.insert(getAggPotential__(i, agg));

        try {
          InstanceBayesNet< GUM_SCALAR > bn(*i);

          std::vector< const DiscreteVariable* > elim;

          for (const auto node: getElimOrder__(i->type())) {
            const auto& var = bn.variable(node);
            if (delayedVars != nullptr) {
              if (!delayedVars->exists(node)) {
                const auto& var = bn.variable(node);
                elim.push_back(&var);
              }
            } else {
              elim.push_back(&var);
            }
          }

          eliminateNodes(elim, pool, trash);
        } catch (NotFound&) {
          // Raised if there is no inner nodes to eliminate
        }
      }

      // Eliminating delayed variables, if any
      if (delayedVariables__.exists(i)) {
        eliminateDelayedVariables__(i, pool, trash);
      }
    }

    template < typename GUM_SCALAR >
    void SVE< GUM_SCALAR >::eliminateNodesUpward__(
       const PRMInstance< GUM_SCALAR >*          i,
       BucketSet&                                pool,
       BucketSet&                                trash,
       List< const PRMInstance< GUM_SCALAR >* >& elim_list,
       Set< const PRMInstance< GUM_SCALAR >* >&  ignore,
       Set< const PRMInstance< GUM_SCALAR >* >&  eliminated) {
      // Downward elimination
      ignore.insert(i);

      for (auto iter = i->beginInvRef(); iter != i->endInvRef(); ++iter) {
        for (auto child = (*(iter.val())).begin(); child != (*(iter.val())).end();
             ++child) {
          if (!ignore.exists(child->first)) {
            eliminateNodesDownward__(
               i, child->first, pool, trash, elim_list, ignore, eliminated);
          }
        }
      }

      // Eliminating all nodes in i instance
      variableElimination__(i, pool, trash);
      eliminated.insert(i);
      // Eliminating instance in elim_list
      List< const PRMInstance< GUM_SCALAR >* > tmp_list;

      while (!elim_list.empty()) {
        if (checkElimOrder__(i, elim_list.front())) {
          if (!ignore.exists(elim_list.front())) {
            eliminateNodesDownward__(
               i, elim_list.front(), pool, trash, elim_list, ignore, eliminated);
          }
        } else {
          tmp_list.insert(elim_list.front());
        }

        elim_list.popFront();
      }

      // Upward elimination
      for (const auto chain: i->type().slotChains()) {
        for (const auto parent: i->getInstances(chain->id())) {
          if (!ignore.exists(parent)) {
            eliminateNodesUpward__(
               parent, pool, trash, tmp_list, ignore, eliminated);
          }
        }
      }
    }

    template < typename GUM_SCALAR >
    void SVE< GUM_SCALAR >::eliminateNodesWithEvidence__(
       const PRMInstance< GUM_SCALAR >* i,
       BucketSet&                       pool,
       BucketSet&                       trash,
       Set< NodeId >*                   delayedVars) {
      // First we check if evidences are on inner nodes
      bool inner = false;

      for (const auto& elt: this->evidence(i)) {
        inner = i->type().isInputNode(i->get(elt.first))
                || i->type().isInnerNode(i->get(elt.first));

        if (inner) { break; }
      }

      // Evidence on inner nodes
      if (inner) {
        BucketSet tmp_pool;
        insertEvidence__(i, tmp_pool);

        // We need a local to not eliminate queried inner nodes of the same
        // class
        for (const auto& elt: *i) {
          tmp_pool.insert(
             &(const_cast< Potential< GUM_SCALAR >& >(elt.second->cpf())));
        }

        InstanceBayesNet< GUM_SCALAR > bn(*i);
        DefaultTriangulation           t(&(bn.moralGraph()), &(bn.modalities()));
        const std::vector< NodeId >&   full_elim_order = t.eliminationOrder();
        // Removing Output nodes of elimination order
        std::vector< const DiscreteVariable* > inner_elim_order;
        std::vector< const DiscreteVariable* > output_elim_order;

        for (size_t idx = 0; idx < full_elim_order.size(); ++idx) {
          auto        var_id = full_elim_order[idx];
          const auto& var = bn.variable(var_id);

          if (!i->type().isOutputNode(i->get(full_elim_order[idx]))) {
            inner_elim_order.push_back(&var);
          } else if (delayedVars != nullptr) {
            if (!delayedVars->exists(full_elim_order[idx])) {
              output_elim_order.push_back(&var);
            }
          } else {
            output_elim_order.push_back(&var);
          }
        }

        eliminateNodes(inner_elim_order, tmp_pool, trash);

        // Now we add the new potentials in pool and eliminate output nodes
        for (const auto pot: tmp_pool)
          pool.insert(pot);

        if (!output_elim_order.empty())
          eliminateNodes(output_elim_order, pool, trash);

      } else {
        InstanceBayesNet< GUM_SCALAR > bn(*i);
        insertEvidence__(i, pool);
        insertLiftedNodes__(i, pool, trash);

        for (const auto agg: i->type().aggregates())
          pool.insert(getAggPotential__(i, agg));

        try {
          std::vector< const DiscreteVariable* > elim;

          for (auto iter = getElimOrder__(i->type()).begin();
               iter != getElimOrder__(i->type()).end();
               ++iter) {
            const auto& var = bn.variable(*iter);
            if (delayedVars != nullptr) {
              if (!delayedVars->exists(*iter)) { elim.push_back(&var); }
            } else {
              elim.push_back(&var);
            }
          }

          eliminateNodes(elim, pool, trash);
        } catch (NotFound&) {
          GUM_ERROR(FatalError, "there should be at least one node here.");
        }
      }
    }

    template < typename GUM_SCALAR >
    void SVE< GUM_SCALAR >::insertLiftedNodes__(const PRMInstance< GUM_SCALAR >* i,
                                                BucketSet& pool,
                                                BucketSet& trash) {
      SVE< GUM_SCALAR >::BucketSet* lifted_pool = 0;

      try {
        lifted_pool = lifted_pools__[&(i->type())];
      } catch (NotFound&) {
        initLiftedNodes__(i->type());
        lifted_pool = lifted_pools__[&(i->type())];
      }

      for (const auto lifted_pot: *lifted_pool) {
        Potential< GUM_SCALAR >* pot = copyPotential(i->bijection(), *lifted_pot);
        pool.insert(pot);
        trash.insert(pot);
      }
    }

    template < typename GUM_SCALAR >
    void SVE< GUM_SCALAR >::initLiftedNodes__(const PRMClass< GUM_SCALAR >& c) {
      BucketSet* lifted_pool = new BucketSet();
      lifted_pools__.insert(&c, lifted_pool);
      NodeSet inners, outers;

      for (const auto node: c.containerDag().nodes())
        if (PRMClassElement< GUM_SCALAR >::isAttribute(c.get(node))) {
          if (c.isOutputNode(c.get(node)))
            outers.insert(node);
          else if (!outers.exists(node))
            inners.insert(node);

          lifted_pool->insert(
             const_cast< Potential< GUM_SCALAR >* >(&(c.get(node).cpf())));
        } else if (PRMClassElement< GUM_SCALAR >::isAggregate(c.get(node))) {
          outers.insert(node);

          // We need to put in the output_elim_order aggregator's parents which
          // are
          // innner nodes
          for (const auto par: c.containerDag().parents(node))
            if (PRMClassElement< GUM_SCALAR >::isAttribute(c.get(par))
                && c.isInnerNode(c.get(par))) {
              inners.erase(par);
              outers.insert(par);
            }
        }

      // Now we proceed with the elimination of inner attributes
      ClassBayesNet< GUM_SCALAR > bn(c);
      List< NodeSet >             partial_ordering;

      if (inners.size()) partial_ordering.push_back(inners);

      if (outers.size()) partial_ordering.push_back(outers);

      PartialOrderedTriangulation t(
         &(bn.moralGraph()), &(bn.modalities()), &partial_ordering);

      for (size_t idx = 0; idx < inners.size(); ++idx)
        eliminateNode(&(c.get(t.eliminationOrder()[idx]).type().variable()),
                      *lifted_pool,
                      lifted_trash__);

      // If there is not only inner and input Attributes
      if (outers.size()) {
        elim_orders__.insert(
           &c,
           new std::vector< NodeId >(t.eliminationOrder().begin() + inners.size(),
                                     t.eliminationOrder().end()));
      }
    }

    template < typename GUM_SCALAR >
    void SVE< GUM_SCALAR >::initElimOrder__() {
      ClassDependencyGraph< GUM_SCALAR >                        cdg(*(this->prm_));
      Sequence< const PRMClassElementContainer< GUM_SCALAR >* > class_elim_order;
      std::list< NodeId >                                       l;

      for (const auto node: cdg.dag().nodes()) {
        if (cdg.dag().parents(node).empty()) { l.push_back(node); }
      }

      Set< NodeId > visited_node;

      while (!l.empty()) {
        visited_node.insert(l.front());

        if (!class_elim_order.exists(cdg.get(l.front()).first)) {
          class_elim_order.insert(cdg.get(l.front()).first);
        }

        for (const auto child: cdg.dag().children(l.front())) {
          if (!visited_node.contains(child)) { l.push_back(child); }
        }

        l.pop_front();
      }

      class_elim_order__ = new Sequence< std::string >();
      for (auto c: class_elim_order) {
        std::string name = c->name();
        auto        pos = name.find_first_of("<");
        if (pos != std::string::npos) { name = name.substr(0, pos); }
        try {
          class_elim_order__->insert(name);
        } catch (DuplicateElement&) {}
      }
    }

    template < typename GUM_SCALAR >
    void SVE< GUM_SCALAR >::posterior_(const Chain&             chain,
                                       Potential< GUM_SCALAR >& m) {
      const PRMInstance< GUM_SCALAR >*  i = chain.first;
      const PRMAttribute< GUM_SCALAR >* elt = chain.second;
      SVE< GUM_SCALAR >::BucketSet      pool, trash;

      eliminateNodes__(i, elt->id(), pool, trash);

      std::vector< Potential< GUM_SCALAR >* > result;

      for (const auto pot: pool) {
        if (pot->contains(elt->type().variable())) { result.push_back(pot); }
      }

      while (result.size() > 1) {
        auto& p1 = *(result.back());
        result.pop_back();
        auto& p2 = *(result.back());
        result.pop_back();
        auto mult = new Potential< GUM_SCALAR >(p1 * p2);
        trash.insert(mult);
        result.push_back(mult);
      }

      m = *(result.back());
      m.normalize();

      for (const auto pot: trash) {
        delete pot;
      }
    }

    template < typename GUM_SCALAR >
    void SVE< GUM_SCALAR >::joint_(const std::vector< Chain >& queries,
                                   Potential< GUM_SCALAR >&    j) {
      GUM_ERROR(FatalError, "Not implemented.");
    }

    template < typename GUM_SCALAR >
    INLINE SVE< GUM_SCALAR >::SVE(const PRM< GUM_SCALAR >&       prm,
                                  const PRMSystem< GUM_SCALAR >& system) :
        PRMInference< GUM_SCALAR >(prm, system),
        class_elim_order__(0) {
      GUM_CONSTRUCTOR(SVE);
    }

    template < typename GUM_SCALAR >
    INLINE void
       SVE< GUM_SCALAR >::insertEvidence__(const PRMInstance< GUM_SCALAR >* i,
                                           BucketSet&                       pool) {
      for (const auto& elt: this->evidence(i))
        pool.insert(const_cast< Potential< GUM_SCALAR >* >(elt.second));
    }

    template < typename GUM_SCALAR >
    INLINE std::vector< NodeId >&
           SVE< GUM_SCALAR >::getElimOrder__(const PRMClass< GUM_SCALAR >& c) {
      return *(elim_orders__[&c]);
    }

    template < typename GUM_SCALAR >
    INLINE std::string SVE< GUM_SCALAR >::trim__(const std::string& s) {
      auto pos = s.find_first_of("<");
      if (pos != std::string::npos) { return s.substr(0, pos); }
      return s;
    }

    template < typename GUM_SCALAR >
    INLINE bool SVE< GUM_SCALAR >::checkElimOrder__(
       const PRMInstance< GUM_SCALAR >* first,
       const PRMInstance< GUM_SCALAR >* second) {
      if (class_elim_order__ == 0) { initElimOrder__(); }

      auto first_name = trim__(first->type().name());
      auto second_name = trim__(second->type().name());
      return (class_elim_order__->pos(first_name)
              <= class_elim_order__->pos(second_name));
    }

    template < typename GUM_SCALAR >
    INLINE Potential< GUM_SCALAR >* SVE< GUM_SCALAR >::getAggPotential__(
       const PRMInstance< GUM_SCALAR >* i, const PRMAggregate< GUM_SCALAR >* agg) {
      return &(const_cast< Potential< GUM_SCALAR >& >(i->get(agg->id()).cpf()));
    }

    template < typename GUM_SCALAR >
    INLINE void SVE< GUM_SCALAR >::evidenceAdded_(const Chain& chain) {
      // Do nothing
    }

    template < typename GUM_SCALAR >
    INLINE void SVE< GUM_SCALAR >::evidenceRemoved_(const Chain& chain) {
      // Do nothing
    }

    template < typename GUM_SCALAR >
    INLINE void
       SVE< GUM_SCALAR >::addDelayedVariable__(const PRMInstance< GUM_SCALAR >* i,
                                               const PRMInstance< GUM_SCALAR >* j,
                                               NodeId id) {
      try {
        delayedVariables__[i]->insert(&(j->get(id).type().variable()));
      } catch (NotFound&) {
        delayedVariables__.insert(i, new Set< const DiscreteVariable* >());
        delayedVariables__[i]->insert(&(j->get(id).type().variable()));
      } catch (DuplicateElement&) {
        // happends if j->get(id) is parent of more than one variable in i
      }

      static std::string dot = ".";

      try {
        delayedVariablesCounters__[j->name() + dot + j->get(id).safeName()] += 1;
      } catch (NotFound&) {
        delayedVariablesCounters__.insert(j->name() + dot + j->get(id).safeName(),
                                          1);
      }
    }

    template < typename GUM_SCALAR >
    INLINE std::string SVE< GUM_SCALAR >::name() const {
      return "SVE";
    }

  } /* namespace prm */
} /* namespace gum */
