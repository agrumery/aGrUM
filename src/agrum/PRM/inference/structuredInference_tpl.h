/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Inline implementation of StructuredInference.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/PRM/inference/structuredInference.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    StructuredInference< GUM_SCALAR >::StructuredInference(
       const PRM< GUM_SCALAR >&             prm,
       const PRMSystem< GUM_SCALAR >&       system,
       gspan::SearchStrategy< GUM_SCALAR >* strategy) :
        PRMInference< GUM_SCALAR >(prm, system),
        _gspan_(0), _pdata_(0), _mining_(false), _dot_(".") {
      GUM_CONSTRUCTOR(StructuredInference);
      _gspan_      = new GSpan< GUM_SCALAR >(prm, system, strategy);
      triang_time  = 0.0;
      mining_time  = 0.0;
      pattern_time = 0.0;
      inner_time   = 0.0;
      obs_time     = 0.0;
      full_time    = 0.0;
    }

    template < typename GUM_SCALAR >
    StructuredInference< GUM_SCALAR >::StructuredInference(
       const StructuredInference< GUM_SCALAR >& source) :
        PRMInference< GUM_SCALAR >(source),
        _gspan_(0), _pdata_(0), _mining_(source._mining_), _found_query_(false), _dot_(".") {
      GUM_CONS_CPY(StructuredInference);
      _gspan_ = new GSpan< GUM_SCALAR >(*(this->prm_), *(this->sys_));
    }

    template < typename GUM_SCALAR >
    StructuredInference< GUM_SCALAR >::~StructuredInference() {
      GUM_DESTRUCTOR(StructuredInference);
      delete this->_gspan_;

      for (const auto& elt: _elim_map_)
        delete elt.second;

      for (const auto& elt: _cdata_map_)
        delete elt.second;

      for (const auto elt: _trash_)
        delete (elt);

      for (const auto& elt: _outputs_)
        delete elt.second;

      if (_pdata_) delete _pdata_;
    }

    template < typename GUM_SCALAR >
    StructuredInference< GUM_SCALAR >& StructuredInference< GUM_SCALAR >::operator=(
       const StructuredInference< GUM_SCALAR >& source) {
      this->prm_ = source.prm_;
      this->sys_ = source.sys_;

      if (this->_gspan_) delete this->_gspan_;

      this->_gspan_ = new GSpan< GUM_SCALAR >(*(this->prm_), *(this->sys_));
      return *this;
    }

    template < typename GUM_SCALAR >
    void StructuredInference< GUM_SCALAR >::evidenceAdded_(
       const typename PRMInference< GUM_SCALAR >::Chain& chain) {}

    template < typename GUM_SCALAR >
    void StructuredInference< GUM_SCALAR >::evidenceRemoved_(
       const typename PRMInference< GUM_SCALAR >::Chain& chain) {}

    template < typename GUM_SCALAR >
    void StructuredInference< GUM_SCALAR >::posterior_(
       const typename PRMInference< GUM_SCALAR >::Chain& chain,
       Potential< GUM_SCALAR >&                          m) {
      timer.reset();
      _found_query_ = false;
      _query_       = chain;
      typename StructuredInference< GUM_SCALAR >::RGData data;

      if (!this->hasEvidence() && (chain.second->cpf().nbrDim() == 1)) {
        Instantiation i(m);

        for (i.setFirst(); !i.end(); i.inc())
          m.set(i, chain.second->cpf().get(i));

        return;
      } else if (this->hasEvidence(chain)) {
        Instantiation                  i(m);
        const Potential< GUM_SCALAR >* e = this->evidence(_query_.first)[_query_.second->id()];

        for (i.setFirst(); !i.end(); i.inc())
          m.set(i, e->get(i));

        return;
      }

      _buildReduceGraph_(data);
      Set< const Potential< GUM_SCALAR >* > pots;

      if (data.pool.size() > 1) {
        for (const auto pot: data.pool)
          if (pot->contains(_query_.second->type().variable())) pots.insert(pot);

        if (pots.size() == 1) {
          Potential< GUM_SCALAR >* pot = const_cast< Potential< GUM_SCALAR >* >(*(pots.begin()));
          GUM_ASSERT(pot->contains(_query_.second->type().variable()));
          GUM_ASSERT(pot->variablesSequence().size() == 1);
          Instantiation i(*pot), j(m);

          for (i.setFirst(), j.setFirst(); !i.end(); i.inc(), j.inc())
            m.set(j, pot->get(i));
        } else {
          MultiDimCombinationDefault< Potential< GUM_SCALAR > > Comb(multPotential);
          Potential< GUM_SCALAR >*                              tmp = Comb.execute(pots);
          Instantiation                                         i(m), j(*tmp);

          for (i.setFirst(), j.setFirst(); !i.end(); i.inc(), j.inc())
            m.set(i, tmp->get(j));

          delete tmp;
        }
      } else {
        Potential< GUM_SCALAR >* pot = *(data.pool.begin());
        GUM_ASSERT(pot->contains(_query_.second->type().variable()));
        GUM_ASSERT(pot->variablesSequence().size() == 1);
        Instantiation i(*pot), j(m);

        for (i.setFirst(), j.setFirst(); !i.end(); i.inc(), j.inc())
          m.set(j, pot->get(i));
      }

      m.normalize();

      if (_pdata_) {
        delete _pdata_;
        _pdata_ = 0;
      }

      full_time = timer.step();
    }

    template < typename GUM_SCALAR >
    void StructuredInference< GUM_SCALAR >::joint_(
       const std::vector< typename PRMInference< GUM_SCALAR >::Chain >& queries,
       Potential< GUM_SCALAR >&                                         j) {
      GUM_ERROR(FatalError, "not implemented")
    }

    template < typename GUM_SCALAR >
    std::string StructuredInference< GUM_SCALAR >::info() const {
      std::stringstream s;
      s << "Triangulation time: " << triang_time << std::endl;
      s << "Pattern mining time: " << mining_time << std::endl;
      s << "Pattern elimination time: " << pattern_time << std::endl;
      s << "Inner node elimination time: " << inner_time << std::endl;
      s << "Observed node elimination time: " << obs_time << std::endl;
      s << "Full inference time: " << full_time << std::endl;
      s << "#patterns: " << _gspan_->patterns().size() << std::endl;
      Size count = 0;
      using Iter = std::vector< gspan::Pattern* >::const_iterator;

      for (Iter p = _gspan_->patterns().begin(); p != _gspan_->patterns().end(); ++p) {
        if (_gspan_->matches(**p).size()) {
          s << "Pattern n°" << count++ << " match count: " << _gspan_->matches(**p).size()
            << std::endl;
          s << "Pattern n°" << count++ << " instance count: " << (**p).size() << std::endl;
        }
      }

      return s.str();
    }

    template < typename GUM_SCALAR >
    void StructuredInference< GUM_SCALAR >::_buildReduceGraph_(
       typename StructuredInference< GUM_SCALAR >::RGData& data) {
      // Launch the pattern mining
      plopTimer.reset();

      if (_mining_) _gspan_->discoverPatterns();

      mining_time = plopTimer.step();
      // Reducing each used pattern
      plopTimer.reset();
      using Iter = std::vector< gspan::Pattern* >::const_iterator;

      for (Iter p = _gspan_->patterns().begin(); p != _gspan_->patterns().end(); ++p)
        if (_gspan_->matches(**p).size()) _reducePattern_(*p);

      pattern_time = plopTimer.step();
      // reducing instance not already reduced in a pattern
      _reduceAloneInstances_(data);
      // Adding edges using the pools
      _addEdgesInReducedGraph_(data);
      // Placing the query where it belongs
      NodeId id = data.var2node.second(&(_query_.second->type().variable()));
      data.outputs().erase(id);
      data.queries().insert(id);
      // Triangulating, then eliminating
      PartialOrderedTriangulation  t(&(data.reducedGraph), &(data.mods), &(data.partial_order));
      const std::vector< NodeId >& elim_order = t.eliminationOrder();

      for (size_t i = 0; i < data.outputs().size(); ++i)
        eliminateNode(data.var2node.first(elim_order[i]), data.pool, _trash_);
    }

    template < typename GUM_SCALAR >
    void StructuredInference< GUM_SCALAR >::_reducePattern_(const gspan::Pattern* p) {
      Set< Potential< GUM_SCALAR >* >                   pool;
      typename StructuredInference< GUM_SCALAR >::PData data(*p, _gspan_->matches(*p));
      _buildPatternGraph_(data, pool, **(data.matches.begin()));
      _removeBarrenNodes_(data, pool);
      PartialOrderedTriangulation  t(&(data.graph), &(data.mod), data.partial_order());
      const std::vector< NodeId >& elim_order = t.eliminationOrder();

      for (size_t i = 0; i < data.inners().size(); ++i)
        if (!data.barren.exists(elim_order[i]))
          eliminateNode(data.vars.second(elim_order[i]), pool, _trash_);

      typename GSpan< GUM_SCALAR >::MatchedInstances                 fake_patterns;
      typename GSpan< GUM_SCALAR >::MatchedInstances::const_iterator iter = data.matches.begin();

      for (const auto elt: **iter)
        _reducedInstances_.insert(elt);

      if (data.obs().size())
        _elim_map_.insert(*iter, _eliminateObservedNodesInSource_(data, pool, **iter, elim_order));
      else _elim_map_.insert(*iter, new Set< Potential< GUM_SCALAR >* >(pool));

      ++iter;

      if (data.obs().size()) {
        for (; iter != data.matches.end(); ++iter) {
          try {
            _elim_map_.insert(*iter, _eliminateObservedNodes_(data, pool, **iter, elim_order));
          } catch (OperationNotAllowed const&) { fake_patterns.insert(*iter); }
        }
      } else {
        for (; iter != data.matches.end(); ++iter) {
          try {
            _elim_map_.insert(*iter, _translatePotSet_(data, pool, **iter));
          } catch (OperationNotAllowed const&) { fake_patterns.insert(*iter); }
        }
      }

      for (const auto pat: fake_patterns) {
        for (const auto elt: *pat)
          _reducedInstances_.erase(elt);

        data.matches.erase(pat);
      }

      obs_time += plopTimer.step();

      if (data.queries().size())
        for (const auto m: data.matches)
          if (!(m->exists(const_cast< PRMInstance< GUM_SCALAR >* >(_query_.first))))
            eliminateNode(
               &(m->atPos(_query_data_.first)->get(_query_data_.second).type().variable()),
               *(_elim_map_[m]),
               _trash_);
    }

    template < typename GUM_SCALAR >
    void StructuredInference< GUM_SCALAR >::_insertNodeInElimLists_(
       typename StructuredInference< GUM_SCALAR >::PData& data,
       const Sequence< PRMInstance< GUM_SCALAR >* >&      match,
       PRMInstance< GUM_SCALAR >*                         inst,
       PRMAttribute< GUM_SCALAR >*                        attr,
       NodeId                                             id,
       std::pair< Idx, std::string >&                     v) {
      if ((*inst).hasRefAttr((*inst).get(v.second).id())) {
        std::vector< std::pair< PRMInstance< GUM_SCALAR >*, std::string > >& refs
           = inst->getRefAttr(inst->get(v.second).id());

        for (auto r = refs.begin(); r != refs.end(); ++r) {
          if (!match.exists(r->first)) {
            data.outputs().insert(id);
            break;
          }
        }
      }

      if (!(data.outputs().size() && (data.outputs().exists(id)))) {
        for (const auto m: data.matches) {
          if (this->hasEvidence(std::make_pair((*m)[v.first], &((*m)[v.first]->get(v.second))))) {
            GUM_ASSERT(inst->type().name() == (*m)[v.first]->type().name());
            GUM_ASSERT(inst->get(v.second).safeName() == (*m)[v.first]->get(v.second).safeName());
            data.obs().insert(id);
            break;
          }
        }

        if (!(data.obs().size() && (data.obs().exists(id)))) data.inners().insert(id);
      }
    }

    template < typename GUM_SCALAR >
    void StructuredInference< GUM_SCALAR >::_buildPatternGraph_(
       typename StructuredInference< GUM_SCALAR >::PData& data,
       Set< Potential< GUM_SCALAR >* >&                   pool,
       const Sequence< PRMInstance< GUM_SCALAR >* >&      match) {
      std::pair< Idx, std::string > v;
      Potential< GUM_SCALAR >*      pot = 0;

      for (const auto inst: match) {
        for (const auto& elt: *inst) {
          NodeId id = data.graph.addNode();
          v         = std::make_pair(match.pos(inst), elt.second->safeName());
          data.map.insert(id, v);
          data.node2attr.insert(id, _str_(inst, elt.second));
          data.mod.insert(id, elt.second->type()->domainSize());
          data.vars.insert(id, &(elt.second->type().variable()));
          pool.insert(const_cast< Potential< GUM_SCALAR >* >(&(elt.second->cpf())));
          pot = &(const_cast< Potential< GUM_SCALAR >& >(inst->get(v.second).cpf()));

          for (const auto var: pot->variablesSequence()) {
            try {
              if (id != data.vars.first(var)) data.graph.addEdge(id, data.vars.first(var));
            } catch (DuplicateElement const&) {
            } catch (NotFound const&) {}
          }

          _insertNodeInElimLists_(data, match, inst, elt.second, id, v);

          if (data.inners().exists(id)
              && (inst->type().containerDag().children(elt.second->id()).size() == 0)
              && _allInstanceNoRefAttr_(data, v))
            data.barren.insert(id);
        }
      }

      if (!_found_query_) {
        for (const auto mat: data.matches) {
          if (mat->exists(const_cast< PRMInstance< GUM_SCALAR >* >(_query_.first))) {
            Idx pos = mat->pos(const_cast< PRMInstance< GUM_SCALAR >* >(_query_.first));
            DiscreteVariable& var
               = match.atPos(pos)->get(_query_.second->safeName()).type().variable();
            NodeId id = data.vars.first(&var);
            data.barren.erase(id);
            data.inners().erase(id);
            data.obs().erase(id);
            data.outputs().erase(id);
            data.queries().insert(id);
            _found_query_ = true;
            _query_data_  = std::make_pair(pos, _query_.second->safeName());
            break;
          }
        }
      }
    }

    template < typename GUM_SCALAR >
    bool StructuredInference< GUM_SCALAR >::_allInstanceNoRefAttr_(
       typename StructuredInference< GUM_SCALAR >::PData& data,
       std::pair< Idx, std::string >                      attr) {
      for (const auto mat: data.matches)
        if (mat->atPos(attr.first)->hasRefAttr(mat->atPos(attr.first)->get(attr.second).id()))
          return false;

      return true;
    }

    template < typename GUM_SCALAR >
    void StructuredInference< GUM_SCALAR >::_removeBarrenNodes_(
       typename StructuredInference< GUM_SCALAR >::PData& data,
       Set< Potential< GUM_SCALAR >* >&                   pool) {
      Sequence< NodeId > candidates;

      for (const auto node: data.barren) {
        for (const auto pot: pool)
          if (pot->contains(*data.vars.second(node))) {
            pool.erase(pot);
            break;
          }

        for (const auto nei: data.graph.neighbours(node))
          if (data.inners().exists(nei)) {
            try {
              candidates.insert(nei);
            } catch (DuplicateElement const&) {}
          }
      }

      NodeId                   node;
      Potential< GUM_SCALAR >* my_pot = nullptr;
      short                    count  = 0;

      while (candidates.size()) {
        node = candidates.back();
        candidates.erase(node);
        count = 0;

        for (const auto pot: pool) {
          if (pot->contains(*data.vars.second(node))) {
            ++count;
            my_pot = pot;
          }
        }

        if (count == 1) {
          pool.erase(my_pot);
          data.barren.insert(node);

          for (const auto nei: data.graph.neighbours(node)) {
            if (data.inners().exists(nei)) {
              try {
                candidates.insert(nei);
              } catch (DuplicateElement const&) {}
            }
          }
        }
      }
    }

    template < typename GUM_SCALAR >
    Set< Potential< GUM_SCALAR >* >*
       StructuredInference< GUM_SCALAR >::_eliminateObservedNodesInSource_(
          typename StructuredInference< GUM_SCALAR >::PData& data,
          const Set< Potential< GUM_SCALAR >* >&             pool,
          const Sequence< PRMInstance< GUM_SCALAR >* >&      match,
          const std::vector< NodeId >&                       elim_order) {
      Set< Potential< GUM_SCALAR >* >* my_pool = new Set< Potential< GUM_SCALAR >* >(pool);
      std::pair< Idx, std::string >    target;
      size_t                           end = data.inners().size() + data.obs().size();

      for (size_t idx = data.inners().size(); idx < end; ++idx) {
        target = data.map[data.vars.first(data.vars.second(elim_order[idx]))];
        eliminateNode(&(match[target.first]->get(target.second).type().variable()),
                      *my_pool,
                      _trash_);
      }

      return my_pool;
    }

    template < typename GUM_SCALAR >
    Set< Potential< GUM_SCALAR >* >* StructuredInference< GUM_SCALAR >::_eliminateObservedNodes_(
       typename StructuredInference< GUM_SCALAR >::PData& data,
       const Set< Potential< GUM_SCALAR >* >&             pool,
       const Sequence< PRMInstance< GUM_SCALAR >* >&      match,
       const std::vector< NodeId >&                       elim_order) {
      Set< Potential< GUM_SCALAR >* >* my_pool = _translatePotSet_(data, pool, match);
      std::pair< Idx, std::string >    target;
      size_t                           end = data.inners().size() + data.obs().size();

      for (size_t idx = data.inners().size(); idx < end; ++idx) {
        target = data.map[data.vars.first(data.vars.second(elim_order[idx]))];
        eliminateNode(&(match[target.first]->get(target.second).type().variable()),
                      *my_pool,
                      _trash_);
      }

      return my_pool;
    }

    template < typename GUM_SCALAR >
    Set< Potential< GUM_SCALAR >* >* StructuredInference< GUM_SCALAR >::_translatePotSet_(
       typename StructuredInference< GUM_SCALAR >::PData& data,
       const Set< Potential< GUM_SCALAR >* >&             pool,
       const Sequence< PRMInstance< GUM_SCALAR >* >&      match) {
#ifdef DEBUG

      for (const auto iter = data.matches.begin(); iter != data.matches.end(); ++iter) {
        GUM_ASSERT((**iter).size() == match.size());

        for (Size idx = 0; idx < match.size(); ++idx) {
          GUM_ASSERT((**iter).atPos(idx)->type() == match.atPos(idx)->type());
        }
      }

#endif
      Set< Potential< GUM_SCALAR >* >* my_pool = new Set< Potential< GUM_SCALAR >* >();
      std::pair< Idx, std::string >    target;
      Bijection< const DiscreteVariable*, const DiscreteVariable* > bij;
      const Sequence< PRMInstance< GUM_SCALAR >* >& source = **(data.matches.begin());

      for (Size idx = 0; idx < match.size(); ++idx) {
        _reducedInstances_.insert(match[idx]);
        const auto& chains = source[idx]->type().slotChains();

        for (const auto sc: chains) {
#ifdef DEBUG
          GUM_ASSERT(!(sc->isMultiple()));
#endif

          try {
            bij.insert(&(source[idx]
                            ->getInstance(sc->id())
                            .get(sc->lastElt().safeName())
                            .type()
                            .variable()),
                       &(match[idx]
                            ->getInstance(sc->id())
                            .get(sc->lastElt().safeName())
                            .type()
                            .variable()));
          } catch (DuplicateElement const&) {
            try {
              if (bij.first(&(match[idx]
                                 ->getInstance(sc->id())
                                 .get(sc->lastElt().safeName())
                                 .type()
                                 .variable()))
                  != &(source[idx]
                          ->getInstance(sc->id())
                          .get(sc->lastElt().safeName())
                          .type()
                          .variable())) {
                delete my_pool;
                GUM_ERROR(OperationNotAllowed, "fake pattern")
              }
            } catch (NotFound const&) {
              delete my_pool;
              GUM_ERROR(OperationNotAllowed, "fake pattern")
            }
          }
        }
      }

      for (const auto p: pool) {
        for (const auto v: p->variablesSequence()) {
          try {
            target = data.map[data.vars.first(v)];
            bij.insert(v, &(match[target.first]->get(target.second).type().variable()));
          } catch (NotFound const&) {
            GUM_ASSERT(bij.existsFirst(v));
          } catch (DuplicateElement const&) {}
        }

        try {
          my_pool->insert(copyPotential(bij, *p));
        } catch (Exception const&) {
          for (const auto pot: *my_pool)
            delete pot;

          delete my_pool;
          GUM_ERROR(OperationNotAllowed, "fake pattern")
        }
      }

      return my_pool;
    }

    template < typename GUM_SCALAR >
    void StructuredInference< GUM_SCALAR >::_reduceAloneInstances_(
       typename StructuredInference< GUM_SCALAR >::RGData& rg_data) {
      StructuredInference< GUM_SCALAR >::CData* data = 0;
      Potential< GUM_SCALAR >*                  pot  = nullptr;
      PRMInstance< GUM_SCALAR >*                inst = nullptr;

      for (const auto& elt: *this->sys_) {
        inst = elt.second;

        if (!_reducedInstances_.exists(inst)) {
          // Checking if its not an empty class
          if (inst->size()) {
            Set< Potential< GUM_SCALAR >* > pool;

            try {
              data = _cdata_map_[&(inst->type())];
            } catch (NotFound const&) {
              data = new StructuredInference< GUM_SCALAR >::CData(inst->type());
              _cdata_map_.insert(&(inst->type()), data);
            }

            data->instances.insert(inst);
            // Filling up the partial ordering
            List< NodeSet > partial_order;

            if (data->inners().size()) partial_order.push_back(data->inners());

            if (data->aggregators().size())
              for (const auto agg: data->aggregators())
                partial_order[0].insert(agg);

            if (data->outputs().size()) partial_order.push_back(data->outputs());

            if (_query_.first == inst) {
              // First case, the instance contains the query
              partial_order[0].erase(_query_.second->id());

              if (partial_order[0].empty()) partial_order.erase(0);

              if (partial_order.size() > 1) {
                partial_order[1].erase(_query_.second->id());

                if (partial_order[1].empty()) partial_order.erase(1);
              }

              NodeSet query_set;
              query_set.insert(_query_.second->id());
              partial_order.insert(query_set);

              // Adding the potentials
              for (auto attr = inst->begin(); attr != inst->end(); ++attr)
                pool.insert(&(const_cast< Potential< GUM_SCALAR >& >((*(attr.val())).cpf())));

              // Adding evidences if any
              if (this->hasEvidence(inst))
                for (const auto& elt: this->evidence(inst))
                  pool.insert(const_cast< Potential< GUM_SCALAR >* >(elt.second));

              PartialOrderedTriangulation  t(&(data->moral_graph), &(data->mods), &(partial_order));
              const std::vector< NodeId >& v = t.eliminationOrder();

              if (partial_order.size() > 1)
                for (size_t idx = 0; idx < partial_order[0].size(); ++idx)
                  eliminateNode(&(inst->get(v[idx]).type().variable()), pool, _trash_);
            } else if (this->hasEvidence(inst)) {
              // Second case, the instance has evidences
              // Adding the potentials
              for (const auto& elt: *inst)
                pool.insert(&const_cast< Potential< GUM_SCALAR >& >(elt.second->cpf()));

              // Adding evidences
              for (const auto& elt: this->evidence(inst))
                pool.insert(const_cast< Potential< GUM_SCALAR >* >(elt.second));

              PartialOrderedTriangulation t(&(data->moral_graph), &(data->mods), &(partial_order));

              for (size_t idx = 0; idx < partial_order[0].size(); ++idx)
                eliminateNode(&(inst->get(t.eliminationOrder()[idx]).type().variable()),
                              pool,
                              _trash_);
            } else {
              // Last cast, the instance neither contains evidences nor
              // instances
              // We translate the class level potentials into the instance ones
              // and
              // proceed with elimination
              for (const auto srcPot: data->pool) {
                pot = copyPotential(inst->bijection(), *srcPot);
                pool.insert(pot);
                _trash_.insert(pot);
              }

              for (const auto agg: data->c.aggregates())
                pool.insert(&(const_cast< Potential< GUM_SCALAR >& >(inst->get(agg->id()).cpf())));

              // We eliminate inner aggregators with their parents if necessary
              // (see
              // CData constructor)
              Size size = data->inners().size() + data->aggregators().size();

              for (size_t idx = data->inners().size(); idx < size; ++idx)
                eliminateNode(&(inst->get(data->elim_order()[idx]).type().variable()),
                              pool,
                              _trash_);
            }

            for (const auto pot: pool)
              rg_data.pool.insert(pot);
          }
        }
      }
    }

    template < typename GUM_SCALAR >
    void StructuredInference< GUM_SCALAR >::_addEdgesInReducedGraph_(
       typename StructuredInference< GUM_SCALAR >::RGData& data) {
      // We first add edges between variables already in pool (i.e. those of the
      // reduced instances)
      NodeId id_1, id_2;

      for (const auto pot: data.pool) {
        const Sequence< const DiscreteVariable* >& vars = pot->variablesSequence();

        for (Size var_1 = 0; var_1 < vars.size(); ++var_1) {
          if (data.var2node.existsFirst(vars.atPos(var_1))) {
            id_1 = data.var2node.second(vars.atPos(var_1));
          } else {
            id_1 = data.reducedGraph.addNode();
            data.var2node.insert(vars.atPos(var_1), id_1);
            data.mods.insert(id_1, vars.atPos(var_1)->domainSize());
            data.outputs().insert(id_1);
          }

          for (Size var_2 = var_1 + 1; var_2 < vars.size(); ++var_2) {
            if (data.var2node.existsFirst(vars.atPos(var_2))) {
              id_2 = data.var2node.second(vars.atPos(var_2));
            } else {
              id_2 = data.reducedGraph.addNode();
              data.var2node.insert(vars.atPos(var_2), id_2);
              data.mods.insert(id_2, vars.atPos(var_2)->domainSize());
              data.outputs().insert(id_2);
            }

            try {
              data.reducedGraph.addEdge(id_1, id_2);
            } catch (DuplicateElement const&) {}
          }
        }
      }

      // Adding potentials obtained from reduced patterns
      for (const auto& elt: _elim_map_) {
        // We add edges between variables in the same reduced patterns
        for (const auto pot: *elt.second) {
          data.pool.insert(pot);
          const Sequence< const DiscreteVariable* >& vars = pot->variablesSequence();

          for (Size var_1 = 0; var_1 < vars.size(); ++var_1) {
            if (data.var2node.existsFirst(vars.atPos(var_1))) {
              id_1 = data.var2node.second(vars.atPos(var_1));
            } else {
              id_1 = data.reducedGraph.addNode();
              data.var2node.insert(vars.atPos(var_1), id_1);
              data.mods.insert(id_1, vars.atPos(var_1)->domainSize());
              data.outputs().insert(id_1);
            }

            for (Size var_2 = var_1 + 1; var_2 < vars.size(); ++var_2) {
              if (data.var2node.existsFirst(vars.atPos(var_2))) {
                id_2 = data.var2node.second(vars.atPos(var_2));
              } else {
                id_2 = data.reducedGraph.addNode();
                data.var2node.insert(vars.atPos(var_2), id_2);
                data.mods.insert(id_2, vars.atPos(var_2)->domainSize());
                data.outputs().insert(id_2);
              }

              try {
                data.reducedGraph.addEdge(id_1, id_2);
              } catch (DuplicateElement const&) {}
            }
          }
        }
      }
    }

    template < typename GUM_SCALAR >
    StructuredInference< GUM_SCALAR >::RGData::RGData() {
      GUM_CONSTRUCTOR(StructuredInference< GUM_SCALAR >::RGData);
      partial_order.insert(NodeSet());
      partial_order.insert(NodeSet());
    }

    template < typename GUM_SCALAR >
    StructuredInference< GUM_SCALAR >::PData::PData(
       const gspan::Pattern&                           p,
       typename GSpan< GUM_SCALAR >::MatchedInstances& m) :
        pattern(p),
        matches(m), _real_order_(0) {
      GUM_CONSTRUCTOR(StructuredInference< GUM_SCALAR >::PData);

      for (int i = 0; i < 4; ++i)
        _partial_order_.push_front(NodeSet());
    }

    template < typename GUM_SCALAR >
    StructuredInference< GUM_SCALAR >::PData::PData(
       const typename StructuredInference< GUM_SCALAR >::PData& source) :
        pattern(source.pattern),
        matches(source.matches), graph(source.graph), mod(source.mod), node2attr(source.node2attr),
        vars(source.vars), _partial_order_(source._partial_order_), _real_order_(0) {
      GUM_CONS_CPY(StructuredInference< GUM_SCALAR >::PData);
    }

    template < typename GUM_SCALAR >
    const List< NodeSet >* StructuredInference< GUM_SCALAR >::PData::partial_order() {
      if (!_real_order_) {
        _real_order_ = new List< NodeSet >();

        for (const auto& set: _partial_order_)
          if (set.size() > 0) _real_order_->insert(set);
      }

      return _real_order_;
    }

    template < typename GUM_SCALAR >
    StructuredInference< GUM_SCALAR >::CData::CData(const PRMClass< GUM_SCALAR >& a_class) :
        c(a_class), _elim_order_(0) {
      GUM_CONSTRUCTOR(StructuredInference< GUM_SCALAR >::CData);

      // First step we add Attributes and Aggregators
      for (const auto node: c.containerDag().nodes()) {
        switch (c.get(node).elt_type()) {
          case PRMClassElement< GUM_SCALAR >::prm_attribute: {
            pool.insert(&(const_cast< Potential< GUM_SCALAR >& >(c.get(node).cpf())));
            // break omited : We want to execute the next block
            // for attributes
          }

          case PRMClassElement< GUM_SCALAR >::prm_aggregate: {
            moral_graph.addNodeWithId(node);
            mods.insert(node, c.get(node).type()->domainSize());
            break;
          }

          default: { /* do nothing */
          }
        }
      }

      // Second, we add edges, moralise the graph and build the partial ordering
      for (const auto node: moral_graph.nodes()) {
        const auto& parents = c.containerDag().parents(node);

        // Adding edges and marrying parents
        for (auto tail = parents.begin(); tail != parents.end(); ++tail) {
          if (PRMClassElement< GUM_SCALAR >::isAttribute(c.get(*tail))
              || PRMClassElement< GUM_SCALAR >::isAggregate(c.get(*tail))) {
            moral_graph.addEdge(*tail, node);
            NodeSet::const_iterator marry = tail;
            ++marry;

            while (marry != parents.end()) {
              if (PRMClassElement< GUM_SCALAR >::isAttribute(c.get(*marry))
                  || PRMClassElement< GUM_SCALAR >::isAggregate(c.get(*marry)))
                moral_graph.addEdge(*tail, *marry);

              ++marry;
            }
          }
        }

        // Adding nodes to the partial ordering
        switch (c.get(node).elt_type()) {
          case PRMClassElement< GUM_SCALAR >::prm_aggregate: {
            if (c.isOutputNode(c.get(node))) outputs().insert(node);
            else aggregators().insert(node);

            // If the aggregators is not an output and have parents which are
            // not outputs, we must eliminate the parents after adding the
            // aggregator's CPT
            for (const auto par: c.containerDag().parents(node)) {
              const auto& prnt = c.get(par);

              if ((!c.isOutputNode(prnt))
                  && (PRMClassElement< GUM_SCALAR >::isAttribute(prnt)
                      || PRMClassElement< GUM_SCALAR >::isAggregate(prnt))) {
                inners().erase(prnt.id());
                aggregators().insert(prnt.id());
              }
            }

            break;
          }

          case PRMClassElement< GUM_SCALAR >::prm_attribute: {
            pool.insert(const_cast< Potential< GUM_SCALAR >* >(&(c.get(node).cpf())));

            if (c.isOutputNode(c.get(node))) outputs().insert(node);
            else if (!aggregators().exists(node)) inners().insert(node);

            break;
          }

          default: { /* Do nothing */
          }
        }
      }

      if (inners().size()) partial_order.insert(inners());

      if (aggregators().size()) partial_order.insert(aggregators());

      if (outputs().size()) partial_order.insert(outputs());

      GUM_ASSERT(partial_order.size());
      PartialOrderedTriangulation t(&moral_graph, &mods, &partial_order);
      _elim_order_ = t.eliminationOrder();

      for (size_t i = 0; i < inners().size(); ++i)
        eliminateNode(&(c.get(_elim_order_[i]).type().variable()), pool, _trash_);
    }

    template < typename GUM_SCALAR >
    StructuredInference< GUM_SCALAR >::CData::~CData() {
      GUM_DESTRUCTOR(StructuredInference< GUM_SCALAR >::CData);

      for (const auto pot: _trash_)
        delete pot;
    }

    template < typename GUM_SCALAR >
    void StructuredInference< GUM_SCALAR >::searchPatterns() {
      const PRMInstance< GUM_SCALAR >* i = (this->sys_->begin()).val();
      _query_                            = std::make_pair(i, i->begin().val());
      _found_query_                      = false;
      typename StructuredInference< GUM_SCALAR >::RGData data;
      _buildReduceGraph_(data);
    }

    template < typename GUM_SCALAR >
    INLINE void StructuredInference< GUM_SCALAR >::setPatternMining(bool b) {
      _mining_ = b;
    }

    template < typename GUM_SCALAR >
    INLINE std::string
           StructuredInference< GUM_SCALAR >::_str_(const PRMInstance< GUM_SCALAR >*  i,
                                                const PRMAttribute< GUM_SCALAR >* a) const {
      return i->name() + _dot_ + a->safeName();
    }

    template < typename GUM_SCALAR >
    INLINE std::string
           StructuredInference< GUM_SCALAR >::_str_(const PRMInstance< GUM_SCALAR >*  i,
                                                const PRMAttribute< GUM_SCALAR >& a) const {
      return i->name() + _dot_ + a.safeName();
    }

    template < typename GUM_SCALAR >
    INLINE std::string
           StructuredInference< GUM_SCALAR >::_str_(const PRMInstance< GUM_SCALAR >*  i,
                                                const PRMSlotChain< GUM_SCALAR >& a) const {
      return i->name() + _dot_ + a.lastElt().safeName();
    }

    template < typename GUM_SCALAR >
    INLINE StructuredInference< GUM_SCALAR >::RGData::~RGData() {
      GUM_DESTRUCTOR(StructuredInference< GUM_SCALAR >::RGData);
    }

    template < typename GUM_SCALAR >
    INLINE StructuredInference< GUM_SCALAR >::PData::~PData() {
      GUM_DESTRUCTOR(StructuredInference< GUM_SCALAR >::PData);
    }

    template < typename GUM_SCALAR >
    INLINE std::string StructuredInference< GUM_SCALAR >::name() const {
      return "StructuredInference";
    }

    template < typename GUM_SCALAR >
    INLINE GSpan< GUM_SCALAR >& StructuredInference< GUM_SCALAR >::gspan() {
      return *_gspan_;
    }

    template < typename GUM_SCALAR >
    INLINE const GSpan< GUM_SCALAR >& StructuredInference< GUM_SCALAR >::gspan() const {
      return *_gspan_;
    }

    template < typename GUM_SCALAR >
    INLINE void StructuredInference< GUM_SCALAR >::_removeNode_(
       typename StructuredInference< GUM_SCALAR >::PData& data,
       NodeId                                             id,
       Set< Potential< GUM_SCALAR >* >&                   pool) {
      data.graph.eraseNode(id);
      GUM_ASSERT(!data.graph.exists(id));
      data.mod.erase(id);
      GUM_ASSERT(!data.mod.exists(id));
      data.node2attr.eraseFirst(id);
      GUM_ASSERT(!data.node2attr.existsFirst(id));
      data.map.erase(id);
      GUM_ASSERT(!data.map.exists(id));
      data.vars.eraseFirst(id);
      GUM_ASSERT(!data.vars.existsFirst(id));
      data.inners().erase(id);
      GUM_ASSERT(!data.inners().exists(id));
      pool.erase(data.pots[id]);
      GUM_ASSERT(!pool.exists(data.pots[id]));
      data.pots.erase(id);
      GUM_ASSERT(!data.pots.exists(id));
    }

  } /* namespace prm */
} /* namespace gum */
