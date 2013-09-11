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
 * @brief Implementation of StructuredInference.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/prm/structuredInference.h>

#ifdef GUM_NO_INLINE
#include <agrum/prm/structuredInference.inl>
#endif // GUM_NO_INLINE

namespace gum {
  namespace prm {

    StructuredInference::StructuredInference ( const PRM& prm, const System& system,
        gspan::SearchStrategy* strategy ) :
      PRMInference ( prm, system ), __gspan ( 0 ), __pdata ( 0 ), __mining ( false ), __dot ( "." ) {
      GUM_CONSTRUCTOR ( StructuredInference );
      __gspan = new GSpan ( prm, system, strategy );
      triang_time = 0.0;
      mining_time = 0.0;
      pattern_time = 0.0;
      inner_time = 0.0;
      obs_time = 0.0;
      full_time = 0.0;
    }

    StructuredInference::StructuredInference ( const StructuredInference& source ) :
      PRMInference ( source ), __gspan ( 0 ), __pdata ( 0 ), __mining ( source.__mining ),
      __found_query ( false ), __dot ( "." ) {
      GUM_CONS_CPY ( StructuredInference );
      __gspan = new GSpan ( *_prm, *_sys );
    }

    StructuredInference::~StructuredInference() {
      GUM_DESTRUCTOR ( StructuredInference );
      delete __gspan;
      typedef HashTable<const Sequence<Instance*>*, Set<Potential<prm_float>*>*>::iterator Iter;

      for ( Iter iter = __elim_map.begin(); iter != __elim_map.end(); ++iter )
        delete ( *iter );

      typedef HashTable<const Class*, StructuredInference::CData*>::iterator CDataIter;

      for ( CDataIter iter = __cdata_map.begin(); iter != __cdata_map.end(); ++iter )
        delete *iter;

      for ( Set<Potential<prm_float>*>::iterator iter = __trash.begin(); iter != __trash.end(); ++iter )
        delete ( *iter );

      typedef HashTable< const Class*, std::vector<NodeId>* >::iterator IterOut;

      for ( IterOut iter = __outputs.begin(); iter != __outputs.end(); ++iter )
        delete *iter;

      if ( __pdata )
        delete __pdata;
    }

    StructuredInference&
    StructuredInference::operator= ( const StructuredInference& source ) {
      _prm = source._prm;
      _sys = source._sys;
      delete __gspan;
      __gspan = new GSpan ( *_prm, *_sys );
      return *this;
    }

    void
    StructuredInference::_evidenceAdded ( const Chain& chain ) {

    }

    void
    StructuredInference::_evidenceRemoved ( const Chain& chain ) {

    }

    void
    StructuredInference::_marginal ( const Chain& chain, Potential<prm_float>& m ) {
      timer.reset();
      __found_query = false;
      __query = chain;
      StructuredInference::RGData data;

      if ( not hasEvidence() and ( chain.second->cpf().nbrDim() == 1 ) ) {
        Instantiation i ( m );

        for ( i.setFirst(); not i.end(); i.inc() )
          m.set ( i, chain.second->cpf().get ( i ) );

        return;
      } else if ( hasEvidence ( chain ) ) {
        Instantiation i ( m );
        const Potential<prm_float>* e = evidence ( __query.first ) [__query.second->id()];

        for ( i.setFirst(); not i.end(); i.inc() )
          m.set ( i, e->get ( i ) );

        return;
      }

      __buildReduceGraph ( data );
      Set<const Potential<prm_float>*> pots;

      if ( data.pool.size() > 1 ) {
        for ( Set<Potential<prm_float>*>::iterator pot = data.pool.begin(); pot != data.pool.end(); ++pot )
          if ( ( **pot ).contains ( __query.second->type().variable() ) )
            pots.insert ( *pot );

        if ( pots.size() == 1 ) {
          Potential<prm_float>* pot = const_cast<Potential<prm_float>*> ( * ( pots.begin() ) );
          GUM_ASSERT ( pot->contains ( __query.second->type().variable() ) );
          GUM_ASSERT ( pot->variablesSequence().size() == 1 );
          Instantiation i ( *pot ), j ( m );

          for ( i.setFirst(), j.setFirst(); not i.end(); i.inc(), j.inc() )
            m.set ( j, pot->get ( i ) );
        } else {
          MultiDimCombinationDefault<prm_float, Potential> Comb ( multPotential );
          Potential<prm_float>* tmp = Comb.combine ( pots );
          Instantiation i ( m ), j ( *tmp );

          for ( i.setFirst(), j.setFirst(); not i.end(); i.inc(), j.inc() )
            m.set ( i, tmp->get ( j ) );

          delete tmp;
        }
      } else {
        Potential<prm_float>* pot = * ( data.pool.begin() );
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

    void
    StructuredInference::_joint ( const std::vector< Chain >& queries, Potential<prm_float>& j ) {
      GUM_ERROR ( FatalError, "not implemented" );
    }

    std::string
    StructuredInference::info() const {
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

    void
    StructuredInference::__buildReduceGraph ( StructuredInference::RGData& data ) {
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

    void
    StructuredInference::__reducePattern ( const gspan::Pattern* p ) {
      Set<Potential<prm_float>*> pool;
      StructuredInference::PData data ( *p, __gspan->matches ( *p ) );
      __buildPatternGraph ( data, pool, ** ( data.matches.begin() ) );
      __removeBarrenNodes ( data, pool );
      PartialOrderedTriangulation t ( & ( data.graph ), & ( data.mod ), data.partial_order() );
      const std::vector<NodeId>& elim_order = t.eliminationOrder();

      for ( size_t i = 0; i < data.inners().size(); ++i )
        if ( not data.barren.exists ( elim_order[i] ) )
          eliminateNode ( data.vars.second ( elim_order[i] ), pool, __trash );

      GSpan::MatchedInstances fake_patterns;
      GSpan::MatchedInstances::const_iterator iter = data.matches.begin();

      for ( Sequence<Instance*>::iterator jter = ( **iter ).begin(); jter != ( **iter ).end(); ++jter )
        __reducedInstances.insert ( *jter );

      if ( data.obs().size() )
        __elim_map.insert ( *iter, __eliminateObservedNodesInSource ( data, pool, **iter, elim_order ) );
      else
        __elim_map.insert ( *iter, new Set<Potential<prm_float>*> ( pool ) );

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

      for ( Set< Sequence<Instance*>* >::iterator iter = fake_patterns.begin(); iter != fake_patterns.end(); ++iter ) {
        for ( Sequence<Instance*>::iterator jter = ( **iter ).begin(); jter != ( **iter ).end(); ++jter )
          __reducedInstances.erase ( *jter );

        data.matches.erase ( *iter );
      }

      obs_time += plopTimer.step();

      if ( data.queries().size() )
        for ( GSpan::MatchedInstances::const_iterator iter = data.matches.begin(); iter != data.matches.end(); ++iter )
          if ( not ( **iter ).exists ( const_cast<Instance*> ( __query.first ) ) )
            eliminateNode ( & ( ( **iter ).atPos ( __query_data.first )->get ( __query_data.second ).type().variable() ), * ( __elim_map[*iter] ), __trash );
    }


    void
    StructuredInference::__insertNodeInElimLists ( StructuredInference::PData& data,
        const Sequence<Instance*>& match,
        Instance* inst,
        Attribute* attr,
        NodeId id,
        std::pair<Idx, std::string>& v ) {
      typedef std::vector< std::pair < Instance*, std::string > >::iterator RefsIter;

      if ( ( *inst ).hasRefAttr ( ( *inst ).get ( v.second ).id() ) ) {
        std::vector< std::pair < Instance*, std::string > >& refs = inst->getRefAttr ( inst->get ( v.second ).id() );

        for ( RefsIter r = refs.begin(); r != refs.end(); ++r ) {
          if ( not match.exists ( r->first ) ) {
            data.outputs().insert ( id );
            break;
          }
        }
      }

      if ( not ( data.outputs().size() and ( data.outputs().exists ( id ) ) ) ) {
        for ( GSpan::MatchedInstances::iterator m = data.matches.begin(); m != data.matches.end(); ++m ) {
          if ( hasEvidence ( std::make_pair ( ( **m ) [v.first], & ( ( **m ) [v.first]->get ( v.second ) ) ) ) ) {
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

    void
    StructuredInference::__buildPatternGraph ( StructuredInference::PData& data,
        Set<Potential<prm_float>*>& pool,
        const Sequence<Instance*>& match ) {
      std::pair<Idx, std::string> v;
      Potential<prm_float>* pot = 0;

      for ( Sequence<Instance*>::const_iterator inst = match.begin(); inst != match.end(); ++inst ) {
        for ( Instance::const_iterator attr = ( **inst ).begin(); attr != ( **inst ).end(); ++attr ) {
          NodeId id = data.graph.insertNode();
          v = std::make_pair ( inst.pos(), ( **attr ).safeName() );
          data.map.insert ( id, v );
          data.node2attr.insert ( id, __str ( *inst, *attr ) );
          data.mod.insert ( id, ( **attr ).type()->domainSize() );
          data.vars.insert ( id, & ( ( **attr ).type().variable() ) );
          pool.insert ( const_cast<Potential<prm_float>*> ( & ( ( **attr ).cpf() ) ) );
          pot = & ( ( **inst ).get ( v.second ).cpf() );

          for ( Sequence<const DiscreteVariable*>::iterator var = pot->variablesSequence().begin();
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
        for ( GSpan::MatchedInstances::iterator iter = data.matches.begin(); iter != data.matches.end(); ++iter ) {
          if ( ( **iter ).exists ( const_cast<Instance*> ( __query.first ) ) ) {
            Idx pos = ( **iter ).pos ( const_cast<Instance*> ( __query.first ) );
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

    bool
    StructuredInference::__allInstanceNoRefAttr ( StructuredInference::PData& data,
        std::pair<Idx, std::string> attr ) {
      NodeId id = 0;

      for ( GSpan::MatchedInstances::iterator iter = data.matches.begin();
            iter != data.matches.end(); ++iter ) {
        id = ( **iter ).atPos ( attr.first )->get ( attr.second ).id();

        if ( ( **iter ).atPos ( attr.first )->hasRefAttr ( id ) )
          return false;
      }

      return true;
    }

    void
    StructuredInference::__removeBarrenNodes ( StructuredInference::PData& data,
        Set<Potential<prm_float>*>& pool ) {
      Sequence<NodeId> candidates;
      const NodeSet* neighbours = 0;

      for ( Set<NodeId>::iterator node = data.barren.begin(); node != data.barren.end(); ++node ) {
        for ( Set<Potential<prm_float>*>::iterator pot = pool.begin(); pot != pool.end(); ++pot ) {
          if ( ( **pot ).contains ( * ( data.vars.second ( *node ) ) ) ) {
            pool.erase ( *pot );
            break;
          }
        }

        neighbours = & ( data.graph.neighbours ( *node ) );

        for ( NodeSet::iterator n = neighbours->begin(); n != neighbours->end(); ++n ) {
          if ( data.inners().exists ( *n ) ) {
            try {
              candidates.insert ( *n );
            } catch ( DuplicateElement& ) { }
          }
        }
      }

      NodeId node;
      Potential<prm_float>* my_pot = 0;
      const DiscreteVariable* var = 0;
      short count = 0;

      while ( candidates.size() ) {
        node = candidates.back();
        candidates.erase ( node );
        neighbours = & ( data.graph.neighbours ( node ) );
        var = data.vars.second ( node );
        count = 0;

        for ( Set<Potential<prm_float>*>::iterator pot = pool.begin(); ( pot != pool.end() ) and ( count < 2 ); ++pot ) {
          if ( ( **pot ).contains ( *var ) ) {
            ++count;
            my_pot = *pot;
          }
        }

        if ( count == 1 ) {
          pool.erase ( my_pot );
          data.barren.insert ( node );

          for ( NodeSet::iterator n = neighbours->begin(); n != neighbours->end(); ++n ) {
            if ( data.inners().exists ( *n ) ) {
              try {
                candidates.insert ( *n );
              } catch ( DuplicateElement& ) { }
            }
          }
        }
      }
    }

    Set<Potential<prm_float>*>*
    StructuredInference::__eliminateObservedNodesInSource ( StructuredInference::PData& data,
        const Set<Potential<prm_float>*>& pool,
        const Sequence<Instance*>& match,
        const std::vector<NodeId>& elim_order ) {
      Set<Potential<prm_float>*>* my_pool = new Set<Potential<prm_float>*> ( pool );
      std::pair<Idx, std::string> target;
      size_t end = data.inners().size() + data.obs().size();

      for ( size_t idx = data.inners().size(); idx < end; ++idx ) {
        target = data.map[data.vars.first ( data.vars.second ( elim_order[idx] ) )];
        eliminateNode ( & ( match[target.first]->get ( target.second ).type().variable() ), *my_pool, __trash );
      }

      return my_pool;
    }


    Set<Potential<prm_float>*>*
    StructuredInference::__eliminateObservedNodes ( StructuredInference::PData& data,
        const Set<Potential<prm_float>*>& pool,
        const Sequence<Instance*>& match,
        const std::vector<NodeId>& elim_order ) {
      Set<Potential<prm_float>*>* my_pool = __translatePotSet ( data, pool, match );
      std::pair<Idx, std::string> target;
      size_t end = data.inners().size() + data.obs().size();

      for ( size_t idx = data.inners().size(); idx < end; ++idx ) {
        target = data.map[data.vars.first ( data.vars.second ( elim_order[idx] ) )];
        eliminateNode ( & ( match[target.first]->get ( target.second ).type().variable() ), *my_pool, __trash );
      }

      return my_pool;
    }

    Set<Potential<prm_float>*>*
    StructuredInference::__translatePotSet ( StructuredInference::PData& data,
        const Set<Potential<prm_float>*>& pool,
        const Sequence<Instance*>& match ) {
#ifdef DEBUG

      for ( GSpan::MatchedInstances::const_iterator iter = data.matches.begin(); iter != data.matches.end(); ++iter ) {
        GUM_ASSERT ( ( **iter ).size() == match.size() );

        for ( Size idx = 0; idx < match.size(); ++idx ) {
          GUM_ASSERT ( ( **iter ).atPos ( idx )->type() == match.atPos ( idx )->type() );
        }
      }

#endif
      Set<Potential<prm_float>*>* my_pool = new Set<Potential<prm_float>*>();
      std::pair<Idx, std::string> target;
      Bijection<const DiscreteVariable*, const DiscreteVariable*> bij;
      const Sequence<Instance*>& source = ** ( data.matches.begin() );

      for ( Size idx = 0; idx < match.size(); ++idx ) {
        __reducedInstances.insert ( match[idx] );

        for ( Set<SlotChain*>::const_iterator sc = source[idx]->type().slotChains().begin(); sc != source[idx]->type().slotChains().end(); ++sc ) {
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

      typedef Set<Potential<prm_float>*>::const_iterator PoolIter;
      typedef Sequence<const DiscreteVariable*>::const_iterator VarIter;

      for ( PoolIter p = pool.begin(); p != pool.end(); ++p ) {
        for ( VarIter v = ( **p ).begin(); v != ( **p ).end(); ++v ) {
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
          for ( Set<Potential<prm_float>*>::iterator pot = my_pool->begin(); pot != my_pool->end(); ++pot ) {
            delete *pot;
          }

          delete my_pool;
          GUM_ERROR ( OperationNotAllowed, "fake pattern" );
        }
      }

      return my_pool;
    }

    void
    StructuredInference::__reduceAloneInstances ( StructuredInference::RGData& rg_data ) {
      StructuredInference::CData* data = 0;
      Potential<prm_float>* pot = 0;
      Instance* inst = 0;

      for ( System::const_iterator i = _sys->begin(); i != _sys->end(); ++i ) {
        inst = *i;

        if ( not __reducedInstances.exists ( inst ) ) {
          // Checking if its not an empty class
          if ( inst->size() ) {
            Set<Potential<prm_float>*> pool;

            try {
              data = __cdata_map[& ( inst->type() )];
            } catch ( NotFound& ) {
              data = new StructuredInference::CData ( inst->type() );
              __cdata_map.insert ( & ( inst->type() ), data );
            }

            data->instances.insert ( inst );
            // Filling up the partial ordering
            List<NodeSet> partial_order;

            if ( data->inners().size() )
              partial_order.push_back ( data->inners() );

            if ( data->aggregators().size() )
              for ( NodeSet::iterator node = data->aggregators().begin(); node != data->aggregators().end(); ++node )
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
              for ( Instance::iterator attr = inst->begin(); attr != inst->end(); ++attr )
                pool.insert ( & ( ( **attr ).cpf() ) );

              // Adding evidences if any
              if ( hasEvidence ( inst ) )
                for ( PRMInference::EMap::iterator e = evidence ( inst ).begin(); e != evidence ( inst ).end(); ++e )
                  pool.insert ( const_cast<Potential<prm_float>*> ( *e ) );

              PartialOrderedTriangulation t ( & ( data->moral_graph ), & ( data->mods ), & ( partial_order ) );
              const std::vector<NodeId>& v = t.eliminationOrder();

              if ( partial_order.size() > 1 )
                for ( size_t idx = 0; idx < partial_order[0].size(); ++idx )
                  eliminateNode ( & ( inst->get ( v[idx] ).type().variable() ), pool, __trash );
            } else if ( hasEvidence ( inst ) ) {
              // Second case, the instance has evidences
              // Adding the potentials
              for ( Instance::iterator attr = inst->begin(); attr != inst->end(); ++attr )
                pool.insert ( & ( ( **attr ).cpf() ) );

              // Adding evidences
              for ( PRMInference::EMap::iterator e = evidence ( inst ).begin(); e != evidence ( inst ).end(); ++e )
                pool.insert ( const_cast<Potential<prm_float>*> ( *e ) );

              PartialOrderedTriangulation t ( & ( data->moral_graph ), & ( data->mods ), & ( partial_order ) );

              for ( size_t idx = 0; idx < partial_order[0].size(); ++idx )
                eliminateNode ( & ( inst->get ( t.eliminationOrder() [idx] ).type().variable() ), pool, __trash );
            } else {
              // Last cast, the instance neither contains evidences nor instances
              // We translate the class level potentials into the instance ones and proceed with elimination
              for ( Set<Potential<prm_float>*>::iterator iter = data->pool.begin(); iter != data->pool.end(); ++iter ) {
                pot = copyPotential ( inst->bijection(), **iter );
                pool.insert ( pot );
                __trash.insert ( pot );
              }

              for ( Set<Aggregate*>::const_iterator agg = data->c.aggregates().begin(); agg != data->c.aggregates().end(); ++agg )
                pool.insert ( & ( inst->get ( ( **agg ).id() ).cpf() ) );

              // We eliminate inner aggregators with their parents if necessary (see CData constructor)
              Size size = data->inners().size() + data->aggregators().size();

              for ( size_t idx = data->inners().size(); idx < size; ++idx )
                eliminateNode ( & ( inst->get ( data->elim_order() [idx] ).type().variable() ), pool, __trash );
            }

            for ( Set<Potential<prm_float>*>::iterator iter = pool.begin(); iter != pool.end(); ++iter )
              rg_data.pool.insert ( *iter );
          }
        }
      }
    }

    void
    StructuredInference::__addEdgesInReducedGraph ( StructuredInference::RGData& data ) {
      // We first add edges between variables already in pool (i.e. those of the reduced instances)
      const Sequence<const DiscreteVariable*>* vars = 0;
      NodeId id_1, id_2;

      for ( Set<Potential<prm_float>*>::iterator pot = data.pool.begin(); pot != data.pool.end(); ++pot ) {
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
      typedef HashTable<const Sequence<Instance*>*, Set<Potential<prm_float>*>*>::iterator ElimIter;
      Set<Potential<prm_float>*>* pool = 0;

      for ( ElimIter iter = __elim_map.begin(); iter != __elim_map.end(); ++iter ) {
        pool = *iter;

        // We add edges between variables in the same reduced patterns
        for ( Set<Potential<prm_float>*>::iterator jter = pool->begin(); jter != pool->end(); ++jter ) {
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

    StructuredInference::RGData::RGData() {
      GUM_CONSTRUCTOR ( StructuredInference::RGData );
      partial_order.insert ( NodeSet() );
      partial_order.insert ( NodeSet() );
    }

    StructuredInference::PData::PData ( const gspan::Pattern& p,
                                        GSpan::MatchedInstances& m ) :
      pattern ( p ), matches ( m ), __real_order ( 0 ) {
      GUM_CONSTRUCTOR ( StructuredInference::PData );

      for ( int i = 0; i < 4; ++i )
        __partial_order.push_front ( NodeSet() );
    }

    StructuredInference::PData::PData ( const StructuredInference::PData& source ) :
      pattern ( source.pattern ), matches ( source.matches ), graph ( source.graph ),
      mod ( source.mod ), node2attr ( source.node2attr ),
      vars ( source.vars ), __partial_order ( source.__partial_order ), __real_order ( 0 ) {
      GUM_CONS_CPY ( StructuredInference::PData );
    }

    const List<NodeSet>*
    StructuredInference::PData::partial_order() {
      if ( not __real_order ) {
        __real_order = new List<NodeSet>();

        for ( List<NodeSet>::iterator set = __partial_order.begin(); set != __partial_order.end(); ++set )
          if ( set->size() ) __real_order->insert ( *set );
      }

      return __real_order;
    }

    StructuredInference::CData::CData ( const Class& a_class ) :
      c ( a_class ), __elim_order ( 0 ) {
      GUM_CONSTRUCTOR ( StructuredInference::CData );

      // First step we add Attributes and Aggregators
      //for ( DAG::NodeIterator node = c.dag().beginNodes(); node != c.dag().endNodes(); ++node ) {
      for ( const auto node : c.dag().nodes() ) {
        switch ( c.get ( node ).elt_type() ) {
          case ClassElement::prm_attribute: {
            pool.insert ( & ( const_cast<Potential<prm_float>&> ( c.get ( node ).cpf() ) ) );
          }

          case ClassElement::prm_aggregate: {
            moral_graph.insertNode ( node );
            mods.insert ( node, c.get ( node ).type()->domainSize() );
            break;
          }

          default: { /* do nothing */ }
        }
      }

      // Second, we add edges, moralise the graph and build the partial ordering
      const NodeSet* parents = 0;
      const ClassElement* prnt = 0;

      //for ( UndiGraph::NodeIterator node = moral_graph.beginNodes(); node != moral_graph.endNodes(); ++node ) {
      for ( const auto node : moral_graph.nodes() ) {
        parents = & ( c.dag().parents ( node ) );

        // Adding edges and marrying parents
        for ( NodeSet::const_iterator tail = parents->begin(); tail != parents->end(); ++tail ) {
          if ( ClassElement::isAttribute ( c.get ( *tail ) ) or ClassElement::isAggregate ( c.get ( *tail ) ) ) {
            moral_graph.insertEdge ( *tail, node );
            NodeSet::const_iterator marry = tail;
            ++marry;

            while ( marry != parents->end() ) {
              if ( ClassElement::isAttribute ( c.get ( *marry ) ) or ClassElement::isAggregate ( c.get ( *marry ) ) )
                moral_graph.insertEdge ( *tail, *marry );

              ++marry;
            }
          }
        }

        // Adding nodes to the partial ordering
        switch ( c.get ( node ).elt_type() ) {
          case ClassElement::prm_aggregate: {
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
                   ( ClassElement::isAttribute ( *prnt ) or ClassElement::isAggregate ( *prnt ) ) ) {
                inners().erase ( prnt->id() );
                aggregators().insert ( prnt->id() );
              }
            }

            break;
          }

          case ClassElement::prm_attribute: {
            pool.insert ( const_cast<Potential<prm_float>*> ( & ( c.get ( node ).cpf() ) ) );

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

    StructuredInference::CData::~CData() {
      GUM_DESTRUCTOR ( StructuredInference::CData );

      for ( Set<Potential<prm_float>*>::iterator pot = __trash.begin(); pot != __trash.end(); ++pot )
        delete *pot;
    }

    void
    StructuredInference::searchPatterns() {
      const Instance* i = * ( _sys->begin() );
      __query = std::make_pair ( i, * ( i->begin() ) );
      __found_query = false;
      StructuredInference::RGData data;
      __buildReduceGraph ( data );
    }

  } /* namespace prm */
} /* namespace gum */

