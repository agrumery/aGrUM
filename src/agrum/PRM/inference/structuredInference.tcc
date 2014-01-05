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
 * @brief Inline implementation of StructuredInference.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/PRM/inference/structuredInference.h>

namespace gum {
  namespace prm {

    template<typename GUM_SCALAR>
    StructuredInference<GUM_SCALAR>::StructuredInference ( const PRM<GUM_SCALAR>& prm, const System<GUM_SCALAR>& system,
        gspan::SearchStrategy<GUM_SCALAR>* strategy ) :
      PRMInference<GUM_SCALAR> ( prm, system ), __gspan ( 0 ), __pdata ( 0 ), __mining ( false ), __dot ( "." ) {
      GUM_CONSTRUCTOR ( StructuredInference );
      __gspan = new GSpan<GUM_SCALAR> ( prm, system, strategy );
      triang_time = 0.0;
      mining_time = 0.0;
      pattern_time = 0.0;
      inner_time = 0.0;
      obs_time = 0.0;
      full_time = 0.0;
    }

    template<typename GUM_SCALAR>
    StructuredInference<GUM_SCALAR>::StructuredInference ( const StructuredInference<GUM_SCALAR>& source ) :
      PRMInference<GUM_SCALAR> ( source ), __gspan ( 0 ), __pdata ( 0 ), __mining ( source.__mining ),
      __found_query ( false ), __dot ( "." ) {
      GUM_CONS_CPY ( StructuredInference );
      __gspan = new GSpan<GUM_SCALAR> ( * ( this->_prm ), * ( this->_sys ) );
    }

    template<typename GUM_SCALAR>
    StructuredInference<GUM_SCALAR>::~StructuredInference() {
      GUM_DESTRUCTOR ( StructuredInference );
      delete this->__gspan;

      for ( auto iter = __elim_map.beginSafe(); iter != __elim_map.endSafe(); ++iter )
        delete ( *iter );

      for ( auto iter = __cdata_map.beginSafe(); iter != __cdata_map.endSafe(); ++iter )
        delete *iter;

      for ( auto iter = __trash.begin(); iter != __trash.end(); ++iter )
        delete ( *iter );

      for ( auto iter = __outputs.beginSafe(); iter != __outputs.endSafe(); ++iter )
        delete *iter;

      if ( __pdata )
        delete __pdata;
    }

    template<typename GUM_SCALAR>
    StructuredInference<GUM_SCALAR>&
    StructuredInference<GUM_SCALAR>::operator= ( const StructuredInference<GUM_SCALAR>& source ) {
      this->_prm = source._prm;
      this->_sys = source._sys;

      if ( this->__gspan ) delete this->__gspan;

      this->__gspan = new GSpan<GUM_SCALAR> ( * ( this->_prm ), * ( this->_sys ) );
      return *this;
    }

    template<typename GUM_SCALAR>
    void
    StructuredInference<GUM_SCALAR>::_evidenceAdded ( const typename PRMInference<GUM_SCALAR>::Chain& chain ) {

    }

    template<typename GUM_SCALAR>
    void
    StructuredInference<GUM_SCALAR>::_evidenceRemoved ( const typename PRMInference<GUM_SCALAR>::Chain& chain ) {

    }

    template<typename GUM_SCALAR>
    void
    StructuredInference<GUM_SCALAR>::_marginal ( const typename PRMInference<GUM_SCALAR>::Chain& chain, Potential<GUM_SCALAR>& m ) {
      timer.reset();
      __found_query = false;
      __query = chain;
      StructuredInference<GUM_SCALAR>::RGData data;

      if ( not this->hasEvidence() and ( chain.second->cpf().nbrDim() == 1 ) ) {
        Instantiation i ( m );

        for ( i.setFirst(); not i.end(); i.inc() )
          m.set ( i, chain.second->cpf().get ( i ) );

        return;
      } else if ( this->hasEvidence ( chain ) ) {
        Instantiation i ( m );
        const Potential<GUM_SCALAR>* e = this->evidence ( __query.first ) [__query.second->id()];

        for ( i.setFirst(); not i.end(); i.inc() )
          m.set ( i, e->get ( i ) );

        return;
      }

      __buildReduceGraph ( data );
      Set<const Potential<GUM_SCALAR>*> pots;

      if ( data.pool.size() > 1 ) {
        for ( auto pot = data.pool.begin(); pot != data.pool.end(); ++pot )
          if ( ( **pot ).contains ( __query.second->type().variable() ) )
            pots.insert ( *pot );

        if ( pots.size() == 1 ) {
          Potential<GUM_SCALAR>* pot = const_cast<Potential<GUM_SCALAR>*> ( * ( pots.begin() ) );
          GUM_ASSERT ( pot->contains ( __query.second->type().variable() ) );
          GUM_ASSERT ( pot->variablesSequence().size() == 1 );
          Instantiation i ( *pot ), j ( m );

          for ( i.setFirst(), j.setFirst(); not i.end(); i.inc(), j.inc() )
            m.set ( j, pot->get ( i ) );
        } else {
          MultiDimCombinationDefault<GUM_SCALAR, Potential> Comb ( multPotential );
          Potential<GUM_SCALAR>* tmp = Comb.combine ( pots );
          Instantiation i ( m ), j ( *tmp );

          for ( i.setFirst(), j.setFirst(); not i.end(); i.inc(), j.inc() )
            m.set ( i, tmp->get ( j ) );

          delete tmp;
        }
      } else {
        Potential<GUM_SCALAR>* pot = * ( data.pool.begin() );
        GUM_ASSERT ( pot->contains ( __query.second->type().variable() ) );
        GUM_ASSERT ( pot->variablesSequence().size() == 1 );
        Instantiation i ( *pot ), j ( m );

        for ( i.setFirst(), j.setFirst(); not i.end(); i.inc(), j.inc() )
          m.set ( j, pot->get ( i ) );
      }

      m.normalize();

      if ( __pdata ) {
        delete __pdata;
        __pdata = 0;
      }

      full_time = timer.step();
    }

    template<typename GUM_SCALAR>
    void
    StructuredInference<GUM_SCALAR>::_joint ( const std::vector< typename PRMInference<GUM_SCALAR>::Chain >& queries, Potential<GUM_SCALAR>& j ) {
      GUM_ERROR ( FatalError, "not implemented" );
    }

    template<typename GUM_SCALAR>
    std::string
    StructuredInference<GUM_SCALAR>::info() const {
      std::stringstream s;
      s << "Triangulation time: " << triang_time << std::endl;
      s << "Pattern mining time: " << mining_time << std::endl;
      s << "Pattern elimination time: " << pattern_time << std::endl;
      s << "Inner node elimination time: " << inner_time << std::endl;
      s << "Observed node elimination time: " << obs_time << std::endl;
      s << "Full inference time: " << full_time << std::endl;
      s << "#patterns: " << __gspan->patterns().size() << std::endl;
      Size count = 0;
      typedef std::vector<gspan::Pattern*>::const_iterator Iter;

      for ( Iter p = __gspan->patterns().begin(); p != __gspan->patterns().end(); ++p ) {
        if ( __gspan->matches ( **p ).size() ) {
          s << "Pattern n°" << count++ << " match count: " << __gspan->matches ( **p ).size() << std::endl;
          s << "Pattern n°" << count++ << " instance count: " << ( **p ).size() << std::endl;
        }
      }

      return s.str();
    }

    template<typename GUM_SCALAR>
    void
    StructuredInference<GUM_SCALAR>::__buildReduceGraph ( StructuredInference<GUM_SCALAR>::RGData& data ) {
      // Launch the pattern mining
      plopTimer.reset();

      if ( __mining )
        __gspan->discoverPatterns();

      mining_time = plopTimer.step();
      // Reducing each used pattern
      plopTimer.reset();
      typedef std::vector<gspan::Pattern*>::const_iterator Iter;

      for ( Iter p = __gspan->patterns().begin(); p != __gspan->patterns().end(); ++p )
        if ( __gspan->matches ( **p ).size() )
          __reducePattern ( *p );

      pattern_time = plopTimer.step();
      // reducing instance not already reduced in a pattern
      __reduceAloneInstances ( data );
      // Adding edges using the pools
      __addEdgesInReducedGraph ( data );
      // Placing the query where it belongs
      NodeId id = data.var2node.second ( & ( __query.second->type().variable() ) );
      data.outputs().erase ( id );
      data.queries().insert ( id );
      // Triangulating, then eliminating
      PartialOrderedTriangulation t ( & ( data.reducedGraph ), & ( data.mods ), & ( data.partial_order ) );
      const std::vector<NodeId>& elim_order = t.eliminationOrder();

      for ( size_t i = 0; i < data.outputs().size(); ++i )
        eliminateNode ( data.var2node.first ( elim_order[i] ), data.pool, __trash );
    }

    template<typename GUM_SCALAR>
    void
    StructuredInference<GUM_SCALAR>::__reducePattern ( const gspan::Pattern* p ) {
      Set<Potential<GUM_SCALAR>*> pool;
      StructuredInference<GUM_SCALAR>::PData data ( *p, __gspan->matches ( *p ) );
      __buildPatternGraph ( data, pool, ** ( data.matches.begin() ) );
      __removeBarrenNodes ( data, pool );
      PartialOrderedTriangulation t ( & ( data.graph ), & ( data.mod ), data.partial_order() );
      const std::vector<NodeId>& elim_order = t.eliminationOrder();

      for ( size_t i = 0; i < data.inners().size(); ++i )
        if ( not data.barren.exists ( elim_order[i] ) )
          eliminateNode ( data.vars.second ( elim_order[i] ), pool, __trash );

      typename GSpan<GUM_SCALAR>::MatchedInstances fake_patterns;
      typename GSpan<GUM_SCALAR>::MatchedInstances::const_iterator iter = data.matches.begin();

      for ( auto jter = ( **iter ).begin(); jter != ( **iter ).end(); ++jter )
        __reducedInstances.insert ( *jter );

      if ( data.obs().size() )
        __elim_map.insert ( *iter, __eliminateObservedNodesInSource ( data, pool, **iter, elim_order ) );
      else
        __elim_map.insert ( *iter, new Set<Potential<GUM_SCALAR>*> ( pool ) );

      ++iter;

      if ( data.obs().size() ) {
        for ( ; iter != data.matches.end(); ++iter ) {
          try {
            __elim_map.insert ( *iter, __eliminateObservedNodes ( data, pool, **iter, elim_order ) );
          } catch ( OperationNotAllowed& ) {
            fake_patterns.insert ( *iter );
          }
        }
      } else {
        for ( ; iter != data.matches.end(); ++iter ) {
          try {
            __elim_map.insert ( *iter, __translatePotSet ( data, pool, **iter ) );
          } catch ( OperationNotAllowed& ) {
            fake_patterns.insert ( *iter );
          }
        }
      }

      for ( auto iter = fake_patterns.begin(); iter != fake_patterns.end(); ++iter ) {
        for ( auto jter = ( **iter ).begin(); jter != ( **iter ).end(); ++jter )
          __reducedInstances.erase ( *jter );

        data.matches.erase ( *iter );
      }

      obs_time += plopTimer.step();

      if ( data.queries().size() )
        for ( auto iter = data.matches.begin(); iter != data.matches.end(); ++iter )
          if ( not ( **iter ).exists ( const_cast<Instance<GUM_SCALAR>*> ( __query.first ) ) )
            eliminateNode ( & ( ( **iter ).atPos ( __query_data.first )->get ( __query_data.second ).type().variable() ), * ( __elim_map[*iter] ), __trash );
    }


    template<typename GUM_SCALAR>
    void
    StructuredInference<GUM_SCALAR>::__insertNodeInElimLists ( StructuredInference<GUM_SCALAR>::PData& data,
        const Sequence<Instance<GUM_SCALAR>*>& match,
        Instance<GUM_SCALAR>* inst,
        Attribute<GUM_SCALAR>* attr,
        NodeId id,
        std::pair<Idx, std::string>& v ) {
      if ( ( *inst ).hasRefAttr ( ( *inst ).get ( v.second ).id() ) ) {
        std::vector< std::pair < Instance<GUM_SCALAR>*, std::string > >& refs = inst->getRefAttr ( inst->get ( v.second ).id() );

        for ( auto r = refs.begin(); r != refs.end(); ++r ) {
          if ( not match.exists ( r->first ) ) {
            data.outputs().insert ( id );
            break;
          }
        }
      }

      if ( not ( data.outputs().size() and ( data.outputs().exists ( id ) ) ) ) {
        for ( auto m = data.matches.begin(); m != data.matches.end(); ++m ) {
          if ( this->hasEvidence ( std::make_pair ( ( **m ) [v.first], & ( ( **m ) [v.first]->get ( v.second ) ) ) ) ) {
            GUM_ASSERT ( inst->type().name() == ( **m ) [v.first]->type().name() );
            GUM_ASSERT ( inst->get ( v.second ).safeName() == ( **m ) [v.first]->get ( v.second ).safeName() );
            data.obs().insert ( id );
            break;
          }
        }

        if ( not ( data.obs().size() and ( data.obs().exists ( id ) ) ) )
          data.inners().insert ( id );
      }
    }

    template<typename GUM_SCALAR>
    void
    StructuredInference<GUM_SCALAR>::__buildPatternGraph ( StructuredInference<GUM_SCALAR>::PData& data,
        Set<Potential<GUM_SCALAR>*>& pool,
        const Sequence<Instance<GUM_SCALAR>*>& match ) {
      std::pair<Idx, std::string> v;
      Potential<GUM_SCALAR>* pot = 0;

      for ( auto inst = match.begin(); inst != match.end(); ++inst ) {
        for ( auto attr = ( **inst ).begin(); attr != ( **inst ).end(); ++attr ) {
          NodeId id = data.graph.insertNode();
          v = std::make_pair ( inst.pos(), ( **attr ).safeName() );
          data.map.insert ( id, v );
          data.node2attr.insert ( id, __str ( *inst, *attr ) );
          data.mod.insert ( id, ( **attr ).type()->domainSize() );
          data.vars.insert ( id, & ( ( **attr ).type().variable() ) );
          pool.insert ( const_cast<Potential<GUM_SCALAR>*> ( & ( ( **attr ).cpf() ) ) );
          pot = & ( ( **inst ).get ( v.second ).cpf() );

          for ( auto var = pot->variablesSequence().begin();
                var != pot->variablesSequence().end(); ++var ) {
            try {
              if ( id != data.vars.first ( *var ) )
                data.graph.insertEdge ( id, data.vars.first ( *var ) );
            } catch ( DuplicateElement& ) { } catch ( NotFound& ) { }
          }

          __insertNodeInElimLists ( data, match, *inst, *attr, id, v );

          if ( data.inners().exists ( id ) and ( ( **inst ).type().dag().children ( ( **attr ).id() ).size() == 0 ) and __allInstanceNoRefAttr ( data, v ) )
            data.barren.insert ( id );
        }
      }

      if ( not __found_query ) {
        for ( auto iter = data.matches.begin(); iter != data.matches.end(); ++iter ) {
          if ( ( **iter ).exists ( const_cast<Instance<GUM_SCALAR>*> ( __query.first ) ) ) {
            Idx pos = ( **iter ).pos ( const_cast<Instance<GUM_SCALAR>*> ( __query.first ) );
            DiscreteVariable& var = match.atPos ( pos )->get ( __query.second->safeName() ).type().variable();
            NodeId id = data.vars.first ( &var );
            data.barren.erase ( id );
            data.inners().erase ( id );
            data.obs().erase ( id );
            data.outputs().erase ( id );
            data.queries().insert ( id );
            __found_query = true;
            __query_data = std::make_pair ( pos, __query.second->safeName() );
            break;
          }
        }
      }
    }

    template<typename GUM_SCALAR>
    bool
    StructuredInference<GUM_SCALAR>::__allInstanceNoRefAttr ( StructuredInference<GUM_SCALAR>::PData& data,
        std::pair<Idx, std::string> attr ) {
      NodeId id = 0;

      for ( auto iter = data.matches.begin();
            iter != data.matches.end(); ++iter ) {
        id = ( **iter ).atPos ( attr.first )->get ( attr.second ).id();

        if ( ( **iter ).atPos ( attr.first )->hasRefAttr ( id ) )
          return false;
      }

      return true;
    }

    template<typename GUM_SCALAR>
    void
    StructuredInference<GUM_SCALAR>::__removeBarrenNodes ( StructuredInference<GUM_SCALAR>::PData& data,
        Set<Potential<GUM_SCALAR>*>& pool ) {
      Sequence<NodeId> candidates;
      const NodeSet* neighbours = 0;

      for ( auto node = data.barren.begin(); node != data.barren.end(); ++node ) {
        for ( auto pot = pool.begin(); pot != pool.end(); ++pot ) {
          if ( ( **pot ).contains ( * ( data.vars.second ( *node ) ) ) ) {
            pool.erase ( *pot );
            break;
          }
        }

        neighbours = & ( data.graph.neighbours ( *node ) );

        for ( auto n = neighbours->begin(); n != neighbours->end(); ++n ) {
          if ( data.inners().exists ( *n ) ) {
            try {
              candidates.insert ( *n );
            } catch ( DuplicateElement& ) { }
          }
        }
      }

      NodeId node;
      Potential<GUM_SCALAR>* my_pot = 0;
      const DiscreteVariable* var = 0;
      short count = 0;

      while ( candidates.size() ) {
        node = candidates.back();
        candidates.erase ( node );
        neighbours = & ( data.graph.neighbours ( node ) );
        var = data.vars.second ( node );
        count = 0;

        for ( auto pot = pool.begin(); ( pot != pool.end() ) and ( count < 2 ); ++pot ) {
          if ( ( **pot ).contains ( *var ) ) {
            ++count;
            my_pot = *pot;
          }
        }

        if ( count == 1 ) {
          pool.erase ( my_pot );
          data.barren.insert ( node );

          for ( auto n = neighbours->begin(); n != neighbours->end(); ++n ) {
            if ( data.inners().exists ( *n ) ) {
              try {
                candidates.insert ( *n );
              } catch ( DuplicateElement& ) { }
            }
          }
        }
      }
    }

    template<typename GUM_SCALAR>
    Set<Potential<GUM_SCALAR>*>*
    StructuredInference<GUM_SCALAR>::__eliminateObservedNodesInSource ( StructuredInference<GUM_SCALAR>::PData& data,
        const Set<Potential<GUM_SCALAR>*>& pool,
        const Sequence<Instance<GUM_SCALAR>*>& match,
        const std::vector<NodeId>& elim_order ) {
      Set<Potential<GUM_SCALAR>*>* my_pool = new Set<Potential<GUM_SCALAR>*> ( pool );
      std::pair<Idx, std::string> target;
      size_t end = data.inners().size() + data.obs().size();

      for ( size_t idx = data.inners().size(); idx < end; ++idx ) {
        target = data.map[data.vars.first ( data.vars.second ( elim_order[idx] ) )];
        eliminateNode ( & ( match[target.first]->get ( target.second ).type().variable() ), *my_pool, __trash );
      }

      return my_pool;
    }


    template<typename GUM_SCALAR>
    Set<Potential<GUM_SCALAR>*>*
    StructuredInference<GUM_SCALAR>::__eliminateObservedNodes ( StructuredInference<GUM_SCALAR>::PData& data,
        const Set<Potential<GUM_SCALAR>*>& pool,
        const Sequence<Instance<GUM_SCALAR>*>& match,
        const std::vector<NodeId>& elim_order ) {
      Set<Potential<GUM_SCALAR>*>* my_pool = __translatePotSet ( data, pool, match );
      std::pair<Idx, std::string> target;
      size_t end = data.inners().size() + data.obs().size();

      for ( size_t idx = data.inners().size(); idx < end; ++idx ) {
        target = data.map[data.vars.first ( data.vars.second ( elim_order[idx] ) )];
        eliminateNode ( & ( match[target.first]->get ( target.second ).type().variable() ), *my_pool, __trash );
      }

      return my_pool;
    }

    template<typename GUM_SCALAR>
    Set<Potential<GUM_SCALAR>*>*
    StructuredInference<GUM_SCALAR>::__translatePotSet ( StructuredInference<GUM_SCALAR>::PData& data,
        const Set<Potential<GUM_SCALAR>*>& pool,
        const Sequence<Instance<GUM_SCALAR>*>& match ) {
#ifdef DEBUG

      for ( GSpan<GUM_SCALAR>::MatchedInstances::const_iterator iter = data.matches.begin(); iter != data.matches.end(); ++iter ) {
        GUM_ASSERT ( ( **iter ).size() == match.size() );

        for ( Size idx = 0; idx < match.size(); ++idx ) {
          GUM_ASSERT ( ( **iter ).atPos ( idx )->type() == match.atPos ( idx )->type() );
        }
      }

#endif
      Set<Potential<GUM_SCALAR>*>* my_pool = new Set<Potential<GUM_SCALAR>*>();
      std::pair<Idx, std::string> target;
      Bijection<const DiscreteVariable*, const DiscreteVariable*> bij;
      const Sequence<Instance<GUM_SCALAR>*>& source = ** ( data.matches.begin() );

      for ( Size idx = 0; idx < match.size(); ++idx ) {
        __reducedInstances.insert ( match[idx] );
        const auto& chains = source[idx]->type().slotChains();

        for ( auto sc = chains.begin(); sc != chains.end(); ++sc ) {
          GUM_ASSERT ( not ( ( **sc ).isMultiple() ) );

          try {
            bij.insert ( & ( source[idx]->getInstance ( ( **sc ).id() ).get ( ( **sc ).lastElt().safeName() ).type().variable() ),
                         & ( match[idx]->getInstance ( ( **sc ).id() ).get ( ( **sc ).lastElt().safeName() ).type().variable() ) );
          } catch ( DuplicateElement& ) {
            try {
              if ( bij.first ( & ( match[idx]->getInstance ( ( **sc ).id() ).get ( ( **sc ).lastElt().safeName() ).type().variable() ) ) !=
                   & ( source[idx]->getInstance ( ( **sc ).id() ).get ( ( **sc ).lastElt().safeName() ).type().variable() ) ) {
                delete my_pool;
                GUM_ERROR ( OperationNotAllowed, "fake pattern" );
              }
            } catch ( NotFound& ) {
              delete my_pool;
              GUM_ERROR ( OperationNotAllowed, "fake pattern" );
            }
          }
        }
      }

      for ( auto p = pool.begin(); p != pool.end(); ++p ) {
        for ( auto v = ( **p ).begin(); v != ( **p ).end(); ++v ) {
          try {
            target = data.map[data.vars.first ( *v )];
            bij.insert ( *v, & ( match[target.first]->get ( target.second ).type().variable() ) );
          } catch ( NotFound& ) {
            GUM_ASSERT ( bij.existsFirst ( *v ) );
          } catch ( DuplicateElement& ) { }
        }

        try {
          my_pool->insert ( copyPotential ( bij, **p ) );
        } catch ( Exception& ) {
          for ( auto pot = my_pool->begin(); pot != my_pool->end(); ++pot ) {
            delete *pot;
          }

          delete my_pool;
          GUM_ERROR ( OperationNotAllowed, "fake pattern" );
        }
      }

      return my_pool;
    }

    template<typename GUM_SCALAR>
    void
    StructuredInference<GUM_SCALAR>::__reduceAloneInstances ( StructuredInference<GUM_SCALAR>::RGData& rg_data ) {
      StructuredInference<GUM_SCALAR>::CData* data = 0;
      Potential<GUM_SCALAR>* pot = nullptr;
      Instance<GUM_SCALAR>* inst = nullptr;

      for ( auto i = this->_sys->begin(); i != this->_sys->end(); ++i ) {
        inst = *i;

        if ( not __reducedInstances.exists ( inst ) ) {
          // Checking if its not an empty class
          if ( inst->size() ) {
            Set<Potential<GUM_SCALAR>*> pool;

            try {
              data = __cdata_map[& ( inst->type() )];
            } catch ( NotFound& ) {
              data = new StructuredInference<GUM_SCALAR>::CData ( inst->type() );
              __cdata_map.insert ( & ( inst->type() ), data );
            }

            data->instances.insert ( inst );
            // Filling up the partial ordering
            List<NodeSet> partial_order;

            if ( data->inners().size() )
              partial_order.push_back ( data->inners() );

            if ( data->aggregators().size() )
              for ( auto node = data->aggregators().begin(); node != data->aggregators().end(); ++node )
                partial_order[0].insert ( *node );

            if ( data->outputs().size() )
              partial_order.push_back ( data->outputs() );

            if ( __query.first == inst ) {
              // First case, the instance contains the query
              partial_order[0].erase ( __query.second->id() );

              if ( partial_order[0].empty() ) partial_order.erase ( 0 );

              if ( partial_order.size() > 1 ) {
                partial_order[1].erase ( __query.second->id() );

                if ( partial_order[1].empty() ) partial_order.erase ( 1 );
              }

              NodeSet query_set;
              query_set.insert ( __query.second->id() );
              partial_order.insert ( query_set );

              // Adding the potentials
              for ( auto attr = inst->begin(); attr != inst->end(); ++attr )
                pool.insert ( & ( ( **attr ).cpf() ) );

              // Adding evidences if any
              if ( this->hasEvidence ( inst ) ) {
                const auto& evs = this->evidence ( inst );

                for ( auto  e = evs.beginSafe(); e != evs.endSafe(); ++e )
                  pool.insert ( const_cast<Potential<GUM_SCALAR>*> ( *e ) );
              }

              PartialOrderedTriangulation t ( & ( data->moral_graph ), & ( data->mods ), & ( partial_order ) );
              const std::vector<NodeId>& v = t.eliminationOrder();

              if ( partial_order.size() > 1 )
                for ( size_t idx = 0; idx < partial_order[0].size(); ++idx )
                  eliminateNode ( & ( inst->get ( v[idx] ).type().variable() ), pool, __trash );
            } else if ( this->hasEvidence ( inst ) ) {
              // Second case, the instance has evidences
              // Adding the potentials
              for ( auto attr = inst->begin(); attr != inst->end(); ++attr )
                pool.insert ( & ( ( **attr ).cpf() ) );

              // Adding evidences

              const auto& evs = this->evidence ( inst );

              for ( auto e = evs.beginSafe(); e != evs.endSafe(); ++e )
                pool.insert ( const_cast<Potential<GUM_SCALAR>*> ( *e ) );

              PartialOrderedTriangulation t ( & ( data->moral_graph ), & ( data->mods ), & ( partial_order ) );

              for ( size_t idx = 0; idx < partial_order[0].size(); ++idx )
                eliminateNode ( & ( inst->get ( t.eliminationOrder() [idx] ).type().variable() ), pool, __trash );
            } else {
              // Last cast, the instance neither contains evidences nor instances
              // We translate the class level potentials into the instance ones and proceed with elimination
              for ( auto iter = data->pool.begin(); iter != data->pool.end(); ++iter ) {
                pot = copyPotential ( inst->bijection(), **iter );
                pool.insert ( pot );
                __trash.insert ( pot );
              }

              for ( auto agg = data->c.aggregates().begin(); agg != data->c.aggregates().end(); ++agg )
                pool.insert ( & ( inst->get ( ( **agg ).id() ).cpf() ) );

              // We eliminate inner aggregators with their parents if necessary (see CData constructor)
              Size size = data->inners().size() + data->aggregators().size();

              for ( size_t idx = data->inners().size(); idx < size; ++idx )
                eliminateNode ( & ( inst->get ( data->elim_order() [idx] ).type().variable() ), pool, __trash );
            }

            for ( auto iter = pool.begin(); iter != pool.end(); ++iter )
              rg_data.pool.insert ( *iter );
          }
        }
      }
    }

    template<typename GUM_SCALAR>
    void
    StructuredInference<GUM_SCALAR>::__addEdgesInReducedGraph ( StructuredInference<GUM_SCALAR>::RGData& data ) {
      // We first add edges between variables already in pool (i.e. those of the reduced instances)
      const Sequence<const DiscreteVariable*>* vars = 0;
      NodeId id_1, id_2;

      for ( auto pot = data.pool.begin(); pot != data.pool.end(); ++pot ) {
        vars = & ( ( **pot ).variablesSequence() );

        for ( Size var_1 = 0; var_1 < vars->size(); ++var_1 ) {
          if ( data.var2node.existsFirst ( vars->atPos ( var_1 ) ) ) {
            id_1 = data.var2node.second ( vars->atPos ( var_1 ) );
          } else {
            id_1 = data.reducedGraph.insertNode();
            data.var2node.insert ( vars->atPos ( var_1 ), id_1 );
            data.mods.insert ( id_1, vars->atPos ( var_1 )->domainSize() );
            data.outputs().insert ( id_1 );
          }

          for ( Size var_2 = var_1 + 1; var_2 < vars->size(); ++var_2 ) {
            if ( data.var2node.existsFirst ( vars->atPos ( var_2 ) ) ) {
              id_2 = data.var2node.second ( vars->atPos ( var_2 ) );
            } else {
              id_2 = data.reducedGraph.insertNode();
              data.var2node.insert ( vars->atPos ( var_2 ), id_2 );
              data.mods.insert ( id_2, vars->atPos ( var_2 )->domainSize() );
              data.outputs().insert ( id_2 );
            }

            try {
              data.reducedGraph.insertEdge ( id_1, id_2 );
            } catch ( DuplicateElement& ) { }
          }
        }
      }

      // Adding potentials obtained from reduced patterns
      Set<Potential<GUM_SCALAR>*>* pool = 0;

      for ( auto iter = __elim_map.beginSafe(); iter != __elim_map.endSafe(); ++iter ) {
        pool = *iter;

        // We add edges between variables in the same reduced patterns
        for ( auto jter = pool->begin(); jter != pool->end(); ++jter ) {
          data.pool.insert ( *jter );
          vars = & ( ( **jter ).variablesSequence() );

          for ( Size var_1 = 0; var_1 < vars->size(); ++var_1 ) {
            if ( data.var2node.existsFirst ( vars->atPos ( var_1 ) ) ) {
              id_1 = data.var2node.second ( vars->atPos ( var_1 ) );
            } else {
              id_1 = data.reducedGraph.insertNode();
              data.var2node.insert ( vars->atPos ( var_1 ), id_1 );
              data.mods.insert ( id_1, vars->atPos ( var_1 )->domainSize() );
              data.outputs().insert ( id_1 );
            }

            for ( Size var_2 = var_1 + 1; var_2 < vars->size(); ++var_2 ) {
              if ( data.var2node.existsFirst ( vars->atPos ( var_2 ) ) ) {
                id_2 = data.var2node.second ( vars->atPos ( var_2 ) );
              } else {
                id_2 = data.reducedGraph.insertNode();
                data.var2node.insert ( vars->atPos ( var_2 ), id_2 );
                data.mods.insert ( id_2, vars->atPos ( var_2 )->domainSize() );
                data.outputs().insert ( id_2 );
              }

              try {
                data.reducedGraph.insertEdge ( id_1, id_2 );
              } catch ( DuplicateElement& ) { }
            }
          }
        }
      }
    }

    template<typename GUM_SCALAR>
    StructuredInference<GUM_SCALAR>::RGData::RGData() {
      GUM_CONSTRUCTOR ( StructuredInference<GUM_SCALAR>::RGData );
      partial_order.insert ( NodeSet() );
      partial_order.insert ( NodeSet() );
    }

    template<typename GUM_SCALAR>
    StructuredInference<GUM_SCALAR>::PData::PData ( const gspan::Pattern& p,
        typename GSpan<GUM_SCALAR>::MatchedInstances& m ) :
      pattern ( p ), matches ( m ), __real_order ( 0 ) {
      GUM_CONSTRUCTOR ( StructuredInference<GUM_SCALAR>::PData );

      for ( int i = 0; i < 4; ++i )
        __partial_order.push_front ( NodeSet() );
    }

    template<typename GUM_SCALAR>
    StructuredInference<GUM_SCALAR>::PData::PData ( const StructuredInference<GUM_SCALAR>::PData& source ) :
      pattern ( source.pattern ), matches ( source.matches ), graph ( source.graph ),
      mod ( source.mod ), node2attr ( source.node2attr ),
      vars ( source.vars ), __partial_order ( source.__partial_order ), __real_order ( 0 ) {
      GUM_CONS_CPY ( StructuredInference<GUM_SCALAR>::PData );
    }

    template<typename GUM_SCALAR>
    const List<NodeSet>*
    StructuredInference<GUM_SCALAR>::PData::partial_order() {
      if ( not __real_order ) {
        __real_order = new List<NodeSet>();

        for ( auto set = __partial_order.begin(); set != __partial_order.end(); ++set )
          if ( set->size() ) __real_order->insert ( *set );
      }

      return __real_order;
    }

    template<typename GUM_SCALAR>
    StructuredInference<GUM_SCALAR>::CData::CData ( const Class<GUM_SCALAR>& a_class ) :
      c ( a_class ), __elim_order ( 0 ) {
      GUM_CONSTRUCTOR ( StructuredInference<GUM_SCALAR>::CData );

      // First step we add Attributes and Aggregators
      for ( const auto node : c.dag().nodes() ) {
        switch ( c.get ( node ).elt_type() ) {
          case ClassElement<GUM_SCALAR>::prm_attribute: {
            pool.insert ( & ( const_cast<Potential<GUM_SCALAR>&> ( c.get ( node ).cpf() ) ) );
          }

          case ClassElement<GUM_SCALAR>::prm_aggregate: {
            moral_graph.insertNode ( node );
            mods.insert ( node, c.get ( node ).type()->domainSize() );
            break;
          }

          default: { /* do nothing */ }
        }
      }

      // Second, we add edges, moralise the graph and build the partial ordering
      const NodeSet* parents = 0;
      const ClassElement<GUM_SCALAR>* prnt = 0;

      for ( const auto node : moral_graph.nodes() ) {
        parents = & ( c.dag().parents ( node ) );

        // Adding edges and marrying parents
        for ( NodeSet::const_iterator tail = parents->begin(); tail != parents->end(); ++tail ) {
          if ( ClassElement<GUM_SCALAR>::isAttribute ( c.get ( *tail ) ) or ClassElement<GUM_SCALAR>::isAggregate ( c.get ( *tail ) ) ) {
            moral_graph.insertEdge ( *tail, node );
            NodeSet::const_iterator marry = tail;
            ++marry;

            while ( marry != parents->end() ) {
              if ( ClassElement<GUM_SCALAR>::isAttribute ( c.get ( *marry ) ) or ClassElement<GUM_SCALAR>::isAggregate ( c.get ( *marry ) ) )
                moral_graph.insertEdge ( *tail, *marry );

              ++marry;
            }
          }
        }

        // Adding nodes to the partial ordering
        switch ( c.get ( node ).elt_type() ) {
          case ClassElement<GUM_SCALAR>::prm_aggregate: {
            if ( c.isOutputNode ( c.get ( node ) ) )
              outputs().insert ( node );
            else
              aggregators().insert ( node );

            // If the aggregators is not an output and have parents which are not
            // outputs, we must eliminate the parents after adding the
            // aggregator's CPT
            parents =  & ( c.dag().parents ( node ) );

            for ( NodeSet::const_iterator iter = parents->begin(); iter != parents->end(); ++iter ) {
              prnt = & ( c.get ( *iter ) );

              if ( ( not c.isOutputNode ( *prnt ) ) and
                   ( ClassElement<GUM_SCALAR>::isAttribute ( *prnt ) or ClassElement<GUM_SCALAR>::isAggregate ( *prnt ) ) ) {
                inners().erase ( prnt->id() );
                aggregators().insert ( prnt->id() );
              }
            }

            break;
          }

          case ClassElement<GUM_SCALAR>::prm_attribute: {
            pool.insert ( const_cast<Potential<GUM_SCALAR>*> ( & ( c.get ( node ).cpf() ) ) );

            if ( c.isOutputNode ( c.get ( node ) ) )
              outputs().insert ( node );
            else if ( not aggregators().exists ( node ) )
              inners().insert ( node );

            break;
          }

          default: { /* Do nothing */ }
        }
      }

      if ( inners().size() )
        partial_order.insert ( inners() );

      if ( aggregators().size() )
        partial_order.insert ( aggregators() );

      if ( outputs().size() )
        partial_order.insert ( outputs() );

      GUM_ASSERT ( partial_order.size() );
      PartialOrderedTriangulation t ( &moral_graph, &mods, &partial_order );
      __elim_order = t.eliminationOrder();

      for ( size_t i = 0; i < inners().size(); ++i )
        eliminateNode ( & ( c.get ( __elim_order[i] ).type().variable() ), pool, __trash );
    }

    template<typename GUM_SCALAR>
    StructuredInference<GUM_SCALAR>::CData::~CData() {
      GUM_DESTRUCTOR ( StructuredInference<GUM_SCALAR>::CData );

      for ( auto pot = __trash.begin(); pot != __trash.end(); ++pot )
        delete *pot;
    }

    template<typename GUM_SCALAR>
    void
    StructuredInference<GUM_SCALAR>::searchPatterns() {
      const Instance<GUM_SCALAR>* i = * ( this->_sys->begin() );
      __query = std::make_pair ( i, * ( i->begin() ) );
      __found_query = false;
      StructuredInference<GUM_SCALAR>::RGData data;
      __buildReduceGraph ( data );
    }

    template<typename GUM_SCALAR> INLINE
    void
    StructuredInference<GUM_SCALAR>::setPatternMining ( bool b ) {
      __mining = b;
    }

    template<typename GUM_SCALAR> INLINE
    std::string
    StructuredInference<GUM_SCALAR>::__str ( const Instance<GUM_SCALAR>* i, const Attribute<GUM_SCALAR>* a ) const {
      return i->name() + __dot + a->safeName();
    }

    template<typename GUM_SCALAR> INLINE
    std::string
    StructuredInference<GUM_SCALAR>::__str ( const Instance<GUM_SCALAR>* i, const Attribute<GUM_SCALAR>& a ) const {
      return i->name() + __dot + a.safeName();
    }

    template<typename GUM_SCALAR> INLINE
    std::string
    StructuredInference<GUM_SCALAR>::__str ( const Instance<GUM_SCALAR>* i, const SlotChain<GUM_SCALAR>& a ) const {
      return i->name() + __dot + a.lastElt().safeName();
    }

    template<typename GUM_SCALAR> INLINE
    StructuredInference<GUM_SCALAR>::RGData::~RGData() {
      GUM_DESTRUCTOR ( StructuredInference<GUM_SCALAR>::RGData );
    }

    template<typename GUM_SCALAR> INLINE
    StructuredInference<GUM_SCALAR>::PData::~PData() {
      GUM_DESTRUCTOR ( StructuredInference<GUM_SCALAR>::PData );
    }

    template<typename GUM_SCALAR> INLINE
    std::string
    StructuredInference<GUM_SCALAR>::name() const {
      return "StructuredInference";
    }

    template<typename GUM_SCALAR> INLINE
    GSpan<GUM_SCALAR>&
    StructuredInference<GUM_SCALAR>::gspan() {
      return *__gspan;
    }

    template<typename GUM_SCALAR> INLINE
    const GSpan<GUM_SCALAR>&
    StructuredInference<GUM_SCALAR>::gspan() const {
      return *__gspan;
    }

    template<typename GUM_SCALAR> INLINE
    void
    StructuredInference<GUM_SCALAR>::__removeNode ( StructuredInference<GUM_SCALAR>::PData& data,
        NodeId id,
        Set<Potential<GUM_SCALAR>*>& pool ) {
      data.graph.eraseNode ( id );
      GUM_ASSERT ( not data.graph.exists ( id ) );
      data.mod.erase ( id );
      GUM_ASSERT ( not data.mod.exists ( id ) );
      data.node2attr.eraseFirst ( id );
      GUM_ASSERT ( not data.node2attr.existsFirst ( id ) );
      data.map.erase ( id );
      GUM_ASSERT ( not data.map.exists ( id ) );
      data.vars.eraseFirst ( id );
      GUM_ASSERT ( not data.vars.existsFirst ( id ) );
      data.inners().erase ( id );
      GUM_ASSERT ( not data.inners().exists ( id ) );
      pool.erase ( data.pots[id] );
      GUM_ASSERT ( not pool.exists ( data.pots[id] ) );
      data.pots.erase ( id );
      GUM_ASSERT ( not data.pots.exists ( id ) );
    }

  } /* namespace prm */
} /* namespace gum */


