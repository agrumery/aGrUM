/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Inline implementation of SVED.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/PRM/inference/SVED.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    SVED< GUM_SCALAR >::~SVED() {
      GUM_DESTRUCTOR(SVED);

      for (const auto& elt: _elim_orders_)
        delete elt.second;

      if (_class_elim_order_ != nullptr) delete _class_elim_order_;
    }

    template < typename GUM_SCALAR >
    void SVED< GUM_SCALAR >::_eliminateNodes_(const PRMInstance< GUM_SCALAR >* query,
                                              NodeId                           node,
                                              BucketSet&                       pool,
                                              BucketSet&                       trash) {
      Set< const PRMInstance< GUM_SCALAR >* > ignore;
      ignore.insert(query);
      // Extracting required attributes and slotchains
      Set< NodeId >& attr_set = _getAttrSet_(query);
      Set< NodeId >& sc_set   = _getSCSet_(query);
      // Downward elimination
      List< const PRMInstance< GUM_SCALAR >* > elim_list;

      for (const auto attr: attr_set) {
        try {
          for (auto iter = query->getRefAttr(attr).begin(); iter != query->getRefAttr(attr).end();
               ++iter)
            if ((!ignore.exists(iter->first)) && (_bb_.exists(iter->first)))
              _eliminateNodesDownward_(query, iter->first, pool, trash, elim_list, ignore);
        } catch (NotFound const&) {
          // Ok
        }
      }

      // Eliminating all nodes in query instance, except query
      InstanceBayesNet< GUM_SCALAR >         bn(*query);
      const auto                             moralg = bn.moralGraph();
      DefaultTriangulation                   t(&moralg, &(bn.modalities()));
      std::vector< const DiscreteVariable* > elim_order;

      if (this->hasEvidence(query)) _insertEvidence_(query, pool);

      for (const auto attr: attr_set)
        pool.insert(&(const_cast< Potential< GUM_SCALAR >& >(query->get(attr).cpf())));

      for (size_t idx = 0; idx < t.eliminationOrder().size(); ++idx) {
        if (t.eliminationOrder()[idx] != node) {
          auto        var_id = t.eliminationOrder()[idx];
          const auto& var    = bn.variable(var_id);
          elim_order.push_back(&var);
        }
      }

      eliminateNodes(elim_order, pool, trash);
      // Eliminating instance in elim_list
      List< const PRMInstance< GUM_SCALAR >* > tmp_list;
      _reduceElimList_(query, elim_list, tmp_list, ignore, pool, trash);

      while (!elim_list.empty()) {
        if (_checkElimOrder_(query, elim_list.front())) {
          if ((!ignore.exists(elim_list.front())) && (_bb_.exists(elim_list.front())))
            _eliminateNodesDownward_(query, elim_list.front(), pool, trash, elim_list, ignore);
        } else if (_bb_.exists(elim_list.front())) {
          tmp_list.insert(elim_list.front());
        }

        elim_list.popFront();
      }

      // Upward elimination
      for (const auto chain: sc_set)
        for (const auto parent: query->getInstances(chain))
          if ((!ignore.exists(parent)) && (_bb_.exists(*parent)))
            _eliminateNodesUpward_(parent, pool, trash, tmp_list, ignore);
    }

    template < typename GUM_SCALAR >
    void SVED< GUM_SCALAR >::_eliminateNodesDownward_(
       const PRMInstance< GUM_SCALAR >*          from,
       const PRMInstance< GUM_SCALAR >*          i,
       BucketSet&                                pool,
       BucketSet&                                trash,
       List< const PRMInstance< GUM_SCALAR >* >& elim_list,
       Set< const PRMInstance< GUM_SCALAR >* >&  ignore) {
      ignore.insert(i);
      // Extracting required attributes and slotchains
      Set< NodeId >& attr_set = _getAttrSet_(i);
      Set< NodeId >& sc_set   = _getSCSet_(i);
      // Calling elimination over child instance
      List< const PRMInstance< GUM_SCALAR >* > my_list;

      for (const auto attr: attr_set) {
        try {
          for (auto iter = i->getRefAttr(attr).begin(); iter != i->getRefAttr(attr).end(); ++iter)
            if ((!ignore.exists(iter->first)) && (_bb_.exists(iter->first)))
              _eliminateNodesDownward_(i, iter->first, pool, trash, my_list, ignore);
        } catch (NotFound const&) {
          // Ok
        }
      }

      // Eliminating all nodes in current instance
      if (this->hasEvidence(i)) {
        _eliminateNodesWithEvidence_(i, pool, trash);
      } else {
        _insertLiftedNodes_(i, pool, trash);

        for (const auto agg: i->type().aggregates())
          if (_bb_.requisiteNodes(i).exists(agg->id())) pool.insert(_getAggPotential_(i, agg));

        try {
          InstanceBayesNet< GUM_SCALAR >         bn(*i);
          std::vector< const DiscreteVariable* > elim_order;

          for (auto node: _getElimOrder_(i->type())) {
            const auto& var = bn.variable(node);
            elim_order.push_back(&var);
          }

          eliminateNodes(elim_order, pool, trash);
        } catch (NotFound const&) {
          // Raised if there is no inner nodes to eliminate
        }
      }

      // Calling elimination over child's parents
      while (!my_list.empty()) {
        if (_checkElimOrder_(i, my_list.front())) {
          if ((!ignore.exists(my_list.front())) && (_bb_.exists(my_list.front())))
            _eliminateNodesDownward_(i, my_list.front(), pool, trash, my_list, ignore);
        } else if (_bb_.exists(my_list.front())) {
          elim_list.insert(my_list.front());
        }

        my_list.popFront();
      }

      // Adding parents instance to elim_list
      for (const auto chain: sc_set)
        for (const auto parent: i->getInstances(chain))
          if ((!ignore.exists(parent)) && _bb_.exists(parent) && (parent != from))
            elim_list.insert(parent);
    }

    template < typename GUM_SCALAR >
    void SVED< GUM_SCALAR >::_eliminateNodesUpward_(
       const PRMInstance< GUM_SCALAR >*          i,
       BucketSet&                                pool,
       BucketSet&                                trash,
       List< const PRMInstance< GUM_SCALAR >* >& elim_list,
       Set< const PRMInstance< GUM_SCALAR >* >&  ignore) {
      ignore.insert(i);
      // Extracting required attributes and slotchains
      Set< NodeId >& attr_set = _getAttrSet_(i);
      Set< NodeId >& sc_set   = _getSCSet_(i);

      // Downward elimination
      for (const auto attr: attr_set) {
        try {
          for (auto iter = i->getRefAttr(attr).begin(); iter != i->getRefAttr(attr).end(); ++iter)
            if ((!ignore.exists(iter->first)) && (_bb_.exists(iter->first)))
              _eliminateNodesDownward_(i, iter->first, pool, trash, elim_list, ignore);
        } catch (NotFound const&) {
          // Ok
        }
      }

      // Eliminating all nodes in i instance
      if (this->hasEvidence(i)) {
        _eliminateNodesWithEvidence_(i, pool, trash);
      } else {
        _insertLiftedNodes_(i, pool, trash);

        for (const auto agg: i->type().aggregates())
          if (_bb_.requisiteNodes(i).exists(agg->id())) pool.insert(_getAggPotential_(i, agg));

        try {
          InstanceBayesNet< GUM_SCALAR >         bn(*i);
          std::vector< const DiscreteVariable* > elim_order;

          for (auto node: _getElimOrder_(i->type())) {
            const auto& var = bn.variable(node);
            elim_order.push_back(&var);
          }
          eliminateNodes(elim_order, pool, trash);
        } catch (NotFound const&) {
          // Raised if there is no inner nodes to eliminate
        }
      }

      // Eliminating instance in elim_list
      List< const PRMInstance< GUM_SCALAR >* > tmp_list;

      while (!elim_list.empty()) {
        if (_checkElimOrder_(i, elim_list.front())) {
          if ((!ignore.exists(elim_list.front())) && (_bb_.exists(elim_list.front())))
            _eliminateNodesDownward_(i, elim_list.front(), pool, trash, elim_list, ignore);
        } else if (_bb_.exists(elim_list.front())) {
          ignore.insert(elim_list.front());
        }

        elim_list.popFront();
      }

      // Upward elimination
      for (const auto chain: sc_set)
        for (const auto parent: i->getInstances(chain))
          if ((!ignore.exists(parent)) && (_bb_.exists(parent)))
            _eliminateNodesUpward_(parent, pool, trash, tmp_list, ignore);
    }

    template < typename GUM_SCALAR >
    void SVED< GUM_SCALAR >::_eliminateNodesWithEvidence_(const PRMInstance< GUM_SCALAR >* i,
                                                          BucketSet&                       pool,
                                                          BucketSet&                       trash) {
      // Adding required evidences
      for (const auto& elt: this->evidence(i))
        if (_bb_.requisiteNodes(i).exists(elt.first))
          pool.insert(const_cast< Potential< GUM_SCALAR >* >(elt.second));

      // Adding potentials and eliminating the remaining nodes
      for (const auto& a: *i)
        if (_bb_.requisiteNodes(i).exists(a.first))
          pool.insert(&(const_cast< Potential< GUM_SCALAR >& >(a.second->cpf())));

      InstanceBayesNet< GUM_SCALAR > bn(*i);
      const auto                     moralg = bn.moralGraph();
      DefaultTriangulation           t(&moralg, &(bn.modalities()));
      const std::vector< NodeId >&   full_elim_order = t.eliminationOrder();

      for (auto var = full_elim_order.begin(); var != full_elim_order.end(); ++var)
        eliminateNode(&(i->get(*var).type().variable()), pool, trash);
    }

    template < typename GUM_SCALAR >
    void SVED< GUM_SCALAR >::_insertLiftedNodes_(const PRMInstance< GUM_SCALAR >* i,
                                                 BucketSet&                       pool,
                                                 BucketSet&                       trash) {
      BucketSet* lifted_pool = nullptr;

      try {
        lifted_pool = _lifted_pools_[&(_bb_.requisiteNodes(i))];
      } catch (NotFound const&) {
        _initLiftedNodes_(i, trash);
        lifted_pool = _lifted_pools_[&(_bb_.requisiteNodes(i))];
      }

      for (const auto lifted_pot: *lifted_pool) {
        Potential< GUM_SCALAR >* pot = copyPotential(i->bijection(), *lifted_pot);
        pool.insert(pot);
        trash.insert(pot);
      }
    }

    template < typename GUM_SCALAR >
    void SVED< GUM_SCALAR >::_initLiftedNodes_(const PRMInstance< GUM_SCALAR >* i,
                                               BucketSet&                       trash) {
      PRMClass< GUM_SCALAR >& c           = const_cast< PRMClass< GUM_SCALAR >& >(i->type());
      BucketSet*              lifted_pool = new BucketSet();
      _lifted_pools_.insert(&(_bb_.requisiteNodes(i)), lifted_pool);

      for (const auto node: _bb_.requisiteNodes(i))
        if (PRMClassElement< GUM_SCALAR >::isAttribute(c.get(node)))
          lifted_pool->insert(const_cast< Potential< GUM_SCALAR >* >(&(c.get(node).cpf())));

      NodeSet inners, outers, ignore;

      for (const auto& elt: *i) {
        if (_bb_.requisiteNodes(*i).exists(elt.first)) {
          if (PRMClassElement< GUM_SCALAR >::isAttribute(c.get(elt.first))) {
            if (c.isOutputNode(c.get(elt.first))) outers.insert(elt.first);
            else if (!outers.exists(elt.first)) inners.insert(elt.first);
          } else if (PRMClassElement< GUM_SCALAR >::isAggregate(c.get(elt.first))) {
            outers.insert(elt.first);

            // We need to put in the output_elim_order aggregator's parents
            // which are
            // innner nodes
            for (const auto par: c.containerDag().parents(elt.first))
              if (PRMClassElement< GUM_SCALAR >::isAttribute(i->type().get(par))
                  && i->type().isInnerNode(i->type().get(par))
                  && _bb_.requisiteNodes(i).exists(par)) {
                inners.erase(par);
                outers.insert(par);
              }
          }
        } else {
          ignore.insert(elt.first);
        }
      }

      // Now we proceed with the elimination of inner attributes
      ClassBayesNet< GUM_SCALAR > bn(c);
      List< NodeSet >             partial_ordering;

      if (inners.size()) partial_ordering.pushBack(inners);

      if (outers.size()) partial_ordering.pushBack(outers);

      if (ignore.size()) partial_ordering.pushBack(ignore);

      GUM_ASSERT(inners.size() || outers.size());
      const auto                  moralg = bn.moralGraph();
      PartialOrderedTriangulation t(&moralg, &(bn.modalities()), &partial_ordering);

      for (size_t idx = 0; idx < inners.size(); ++idx)
        eliminateNode(&(c.get(t.eliminationOrder()[idx]).type().variable()), *lifted_pool, trash);

      // If there is not only inner and input Attributes
      if (outers.size()) {
        _elim_orders_.insert(&c,
                             new std::vector< NodeId >(t.eliminationOrder().begin() + inners.size(),
                                                       t.eliminationOrder().end()));
      }
    }

    template < typename GUM_SCALAR >
    void SVED< GUM_SCALAR >::_initElimOrder_() {
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

      _class_elim_order_ = new Sequence< std::string >();
      for (auto c: class_elim_order) {
        std::string name = c->name();
        auto        pos  = name.find_first_of("<");
        if (pos != std::string::npos) { name = name.substr(0, pos); }
        try {
          _class_elim_order_->insert(name);
        } catch (DuplicateElement const&) {}
      }
    }

    template < typename GUM_SCALAR >
    void SVED< GUM_SCALAR >::posterior_(const Chain& chain, Potential< GUM_SCALAR >& m) {
      const PRMInstance< GUM_SCALAR >*  i   = chain.first;
      const PRMAttribute< GUM_SCALAR >* elt = chain.second;
      SVED< GUM_SCALAR >::BucketSet     pool, trash;
      _bb_.compute(i, elt->id());
      _eliminateNodes_(i, elt->id(), pool, trash);

      std::vector< const Potential< GUM_SCALAR >* > result;
      for (auto pot: pool) {
        if (pot->contains(*(m.variablesSequence().atPos(0)))) result.push_back(pot);
      }

      while (result.size() > 1) {
        const auto& p1 = *(result.back());
        result.pop_back();
        const auto& p2 = *(result.back());
        result.pop_back();
        auto mult = new Potential< GUM_SCALAR >(p1 * p2);
        result.push_back(mult);
        trash.insert(mult);
      }

      m = *(result.back());
      m.normalize();

      GUM_ASSERT(m.nbrDim() == (Size)1);

      // cleaning up the mess
      for (const auto pot: trash)
        delete pot;

      for (const auto& elt: _lifted_pools_)
        delete elt.second;

      _lifted_pools_.clear();

      for (const auto& elt: _req_set_) {
        delete elt.second.first;
        delete elt.second.second;
      }

      _req_set_.clear();

      for (const auto& elt: _elim_orders_)
        delete elt.second;

      _elim_orders_.clear();
    }

    template < typename GUM_SCALAR >
    void SVED< GUM_SCALAR >::joint_(const std::vector< Chain >& queries,
                                    Potential< GUM_SCALAR >&    j) {
      GUM_ERROR(FatalError, "Not implemented.")
    }

    template < typename GUM_SCALAR >
    void SVED< GUM_SCALAR >::_initReqSets_(const PRMInstance< GUM_SCALAR >* i) {
      Set< NodeId >* attr_set = new Set< NodeId >();
      Set< NodeId >* sc_set   = new Set< NodeId >();

      for (const auto node: _bb_.requisiteNodes(i)) {
        switch (i->type().get(node).elt_type()) {
          case PRMClassElement< GUM_SCALAR >::prm_aggregate:
          case PRMClassElement< GUM_SCALAR >::prm_attribute: {
            attr_set->insert(node);
            break;
          }

          case PRMClassElement< GUM_SCALAR >::prm_slotchain: {
            sc_set->insert(node);
            break;
          }

          default: {
            GUM_ERROR(FatalError,
                      "There should not be elements other"
                      " than PRMAttribute<GUM_SCALAR> and SlotChain.");
          }
        }
      }

      _req_set_.insert(&(_bb_.requisiteNodes(i)),
                       std::pair< Set< NodeId >*, Set< NodeId >* >(attr_set, sc_set));
    }

    template < typename GUM_SCALAR >
    INLINE SVED< GUM_SCALAR >::SVED(const PRM< GUM_SCALAR >&       prm,
                                    const PRMSystem< GUM_SCALAR >& model) :
        PRMInference< GUM_SCALAR >(prm, model),
        _class_elim_order_(0), _bb_(*this) {
      GUM_CONSTRUCTOR(SVED);
    }


    template < typename GUM_SCALAR >
    INLINE void SVED< GUM_SCALAR >::_insertEvidence_(const PRMInstance< GUM_SCALAR >* i,
                                                     BucketSet&                       pool) {
      for (const auto& elt: this->evidence(i))
        pool.insert(const_cast< Potential< GUM_SCALAR >* >(elt.second));
    }

    template < typename GUM_SCALAR >
    INLINE std::vector< NodeId >&
           SVED< GUM_SCALAR >::_getElimOrder_(const PRMClass< GUM_SCALAR >& c) {
      return *(_elim_orders_[&c]);
    }

    template < typename GUM_SCALAR >
    INLINE std::string SVED< GUM_SCALAR >::_trim_(const std::string& s) {
      auto pos = s.find_first_of("<");
      if (pos != std::string::npos) { return s.substr(0, pos); }
      return s;
    }

    template < typename GUM_SCALAR >
    INLINE bool SVED< GUM_SCALAR >::_checkElimOrder_(const PRMInstance< GUM_SCALAR >* first,
                                                     const PRMInstance< GUM_SCALAR >* second) {
      if (_class_elim_order_ == 0) { _initElimOrder_(); }

      auto first_name  = _trim_(first->type().name());
      auto second_name = _trim_(second->type().name());
      return (_class_elim_order_->pos(first_name) <= _class_elim_order_->pos(second_name));
    }

    template < typename GUM_SCALAR >
    INLINE Potential< GUM_SCALAR >*
           SVED< GUM_SCALAR >::_getAggPotential_(const PRMInstance< GUM_SCALAR >*  i,
                                             const PRMAggregate< GUM_SCALAR >* agg) {
      return &(const_cast< Potential< GUM_SCALAR >& >(i->get(agg->safeName()).cpf()));
    }

    template < typename GUM_SCALAR >
    INLINE void
       SVED< GUM_SCALAR >::evidenceAdded_(const typename SVED< GUM_SCALAR >::Chain& chain) {
      // Do nothing
    }

    template < typename GUM_SCALAR >
    INLINE void
       SVED< GUM_SCALAR >::evidenceRemoved_(const typename SVED< GUM_SCALAR >::Chain& chain) {
      // Do nothing
    }

    template < typename GUM_SCALAR >
    INLINE Set< NodeId >& SVED< GUM_SCALAR >::_getAttrSet_(const PRMInstance< GUM_SCALAR >* i) {
      try {
        return *(_req_set_[&(_bb_.requisiteNodes(i))].first);
      } catch (NotFound const&) {
        _initReqSets_(i);
        return *(_req_set_[&(_bb_.requisiteNodes(i))].first);
      }
    }

    template < typename GUM_SCALAR >
    INLINE Set< NodeId >& SVED< GUM_SCALAR >::_getSCSet_(const PRMInstance< GUM_SCALAR >* i) {
      try {
        return *(_req_set_[&(_bb_.requisiteNodes(i))].second);
      } catch (NotFound const&) {
        _initReqSets_(i);
        return *(_req_set_[&(_bb_.requisiteNodes(i))].second);
      }
    }

    template < typename GUM_SCALAR >
    INLINE void
       SVED< GUM_SCALAR >::_reduceElimList_(const PRMInstance< GUM_SCALAR >*          i,
                                            List< const PRMInstance< GUM_SCALAR >* >& elim_list,
                                            List< const PRMInstance< GUM_SCALAR >* >& reduced_list,
                                            Set< const PRMInstance< GUM_SCALAR >* >&  ignore,
                                            BucketSet&                                pool,
                                            BucketSet&                                trash) {
      while (!elim_list.empty()) {
        if (_checkElimOrder_(i, elim_list.front())) {
          if ((!ignore.exists(elim_list.front())) && (_bb_.exists(elim_list.front()))) {
            _eliminateNodesDownward_(i, elim_list.front(), pool, trash, elim_list, ignore);
          }
        } else if (_bb_.exists(elim_list.front())) {
          reduced_list.insert(elim_list.front());
        }

        elim_list.popFront();
      }
    }

    template < typename GUM_SCALAR >
    INLINE std::string SVED< GUM_SCALAR >::name() const {
      return "SVED";
    }

  } /* namespace prm */
} /* namespace gum */
