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
 * @brief Implementation of SVE.
 *
 * @author Lionel TORTI
 */

#include <agrum/prm/SVE.h>

#ifdef GUM_NO_INLINE
#include <agrum/prm/SVE.inl>
#endif // GUM_NO_INLINE

namespace gum {
  namespace prm {


    SVE::~SVE() {
      GUM_DESTRUCTOR( SVE );
      typedef HashTable<const Class*, std::vector<NodeId>*>::iterator ElimIter;

      for ( ElimIter iter = __elim_orders.begin(); iter != __elim_orders.end(); ++iter ) {
        delete *iter;
      }

      typedef HashTable<const Class*, BucketSet*>::iterator LiftedIter;

      for ( LiftedIter iter = __lifted_pools.begin(); iter != __lifted_pools.end(); ++iter ) {
        delete *iter;
      }

      if ( __class_elim_order != 0 ) {
        delete __class_elim_order;
      }

      for ( SVE::BucketSetIterator iter = __lifted_trash.begin(); iter != __lifted_trash.end(); ++iter ) {
        delete *iter;
      }
    }

    void
    SVE::__eliminateNodes( const Instance* query, NodeId node, BucketSet& pool, BucketSet& trash ) {
      Set<const Instance*> ignore, eliminated;
      Set<NodeId> delayedVars;
      // Downward elimination
      List<const Instance*> elim_list;
      ignore.insert( query );

      for ( Instance::InvRefConstIterator iter = query->beginInvRef(); iter != query->endInvRef(); ++iter ) {
        typedef std::vector< std::pair<Instance*, std::string> >::const_iterator pair_iter;

        for ( pair_iter child = ( **iter ).begin(); child != ( **iter ).end(); ++child ) {
          if ( not ignore.exists( child->first ) ) {
            __eliminateNodesDownward( query, child->first, pool, trash, elim_list, ignore, eliminated );
          } else if ( not eliminated.exists( child->first ) ) {
            __addDelayedVariable( child->first, query, iter.key() );
            delayedVars.insert( iter.key() );
          }
        }
      }

      // Eliminating all nodes in query instance, except query
      InstanceBayesNet bn( *query );
      DefaultTriangulation t( &( bn.moralGraph() ), &( bn.modalities() ) );
      std::vector<NodeId> elim_order;
      VariableElimination<prm_float> inf( bn );

      if ( hasEvidence( query ) ) {
        __insertEvidence( query, pool );
      }

      for ( Instance::const_iterator attr = query->begin(); attr != query->end(); ++attr ) {
        pool.insert( &( const_cast<Potential<prm_float>&>( ( **attr ).cpf() ) ) );
      }

      for ( size_t idx = 0; idx < t.eliminationOrder().size(); ++idx ) {
        if ( ( t.eliminationOrder()[idx] != node ) and ( not delayedVars.exists( t.eliminationOrder()[idx] ) ) ) {
          elim_order.push_back( t.eliminationOrder()[idx] );
        }
      }

      inf.eliminateNodes( elim_order, pool, trash );

      // Eliminating delayed variables, if any
      if ( __delayedVariables.exists( query ) ) {
        __eliminateDelayedVariables( query, pool, trash );
      }

      eliminated.insert( query );
      // Eliminating instance in elim_list
      List<const Instance*> tmp_list;

      while ( not elim_list.empty() ) {
        if ( __checkElimOrder( query, elim_list.front() ) ) {
          if ( not ignore.exists( elim_list.front() ) ) {
            __eliminateNodesDownward( query, elim_list.front(), pool, trash, elim_list, ignore, eliminated );
          }
        } else {
          tmp_list.insert( elim_list.front() );
        }

        elim_list.popFront();
      }

      // Upward elimination
      for ( Set<SlotChain*>::iterator sc = query->type().slotChains().begin(); sc != query->type().slotChains().end(); ++sc ) {
        for ( Set<Instance*>::iterator parent = query->getInstances( ( **sc ).id() ).begin(); parent != query->getInstances( ( **sc ).id() ).end(); ++parent ) {
          if ( not ignore.exists( *parent ) ) {
            __eliminateNodesUpward( *parent, pool, trash, tmp_list, ignore, eliminated );
          }
        }
      }
    }

    void
    SVE::__eliminateDelayedVariables( const Instance* i, BucketSet& pool, BucketSet& trash ) {
      Set<Potential<prm_float>*> toRemove;
      typedef Set< const DiscreteVariable* >::iterator DelayedIter;

      for ( DelayedIter iter = __delayedVariables[i]->begin(); iter != __delayedVariables[i]->end(); ++iter ) {
        MultiDimBucket<prm_float>* bucket = new MultiDimBucket<prm_float>();

        for ( SetIterator<Potential<prm_float>*> jter = pool.begin(); jter != pool.end(); ++jter ) {
          if ( ( *jter )->contains( **iter ) ) {
            bucket->add( **jter );
            toRemove.insert( *jter );
          }
        }

        for ( SetIterator< Potential<prm_float>* > jter = toRemove.begin(); jter != toRemove.end(); ++jter ) {
          pool.erase( *jter );
        }

        typedef Set<const DiscreteVariable*>::iterator VarIter;

        for ( VarIter jter = bucket->allVariables().begin(); jter != bucket->allVariables().end(); ++jter ) {
          if ( *jter != *iter ) {
            bucket->add( **jter );
          }
        }

        Potential<prm_float>* bucket_pot = new Potential<prm_float>( bucket );
        trash.insert( bucket_pot );
        pool.insert( bucket_pot );
      }
    }

    void
    SVE::__eliminateNodesDownward( const Instance* from, const Instance* i, BucketSet& pool, BucketSet& trash, List<const Instance*>& elim_list, Set<const Instance*>& ignore, Set<const Instance*>& eliminated ) {
      Set<NodeId> delayedVars;
      ignore.insert( i );
      // Calling elimination over child instance
      List<const Instance*> my_list;

      for ( Instance::InvRefConstIterator iter = i->beginInvRef(); iter != i->endInvRef(); ++iter ) {
        typedef std::vector< std::pair<Instance*, std::string> >::const_iterator pair_iter;

        for ( pair_iter child = ( **iter ).begin(); child != ( **iter ).end(); ++child ) {
          if ( not ignore.exists( child->first ) ) {
            __eliminateNodesDownward( i, child->first, pool, trash, my_list, ignore, eliminated );
          } else if ( not eliminated.exists( child->first ) ) {
            __addDelayedVariable( child->first, i, iter.key() );
            delayedVars.insert( iter.key() );
          }
        }
      }

      // Eliminating all nodes in current instance
      __variableElimination( i, pool, trash, ( delayedVars.empty()?0:&delayedVars ) );
      eliminated.insert( i );

      // Calling elimination over child's parents
      for ( List<const Instance*>::iterator iter = my_list.begin(); iter != my_list.end(); ++iter ) {
        if ( __checkElimOrder( i, *iter ) and ( *iter != from ) ) {
          if ( not ignore.exists( *iter ) ) {
            __eliminateNodesDownward( i, *iter, pool, trash, elim_list, ignore, eliminated );
          }
        } else if ( *iter != from ) {
          elim_list.insert( *iter );
        }
      }

      // Adding parents instance to elim_list
      for ( Set<SlotChain*>::iterator sc = i->type().slotChains().begin(); sc != i->type().slotChains().end(); ++sc ) {
        for ( Set<Instance*>::iterator iter = i->getInstances( ( **sc ).id() ).begin(); iter != i->getInstances( ( **sc ).id() ); ++iter ) {
          if ( *iter != from ) {
            elim_list.insert( *iter );
          }
        }
      }
    }

    void
    SVE::__variableElimination( const Instance* i, BucketSet& pool, BucketSet& trash, Set<NodeId>* delayedVars ) {
      if ( hasEvidence( i ) ) {
        __eliminateNodesWithEvidence( i, pool, trash, delayedVars );
      } else {
        __insertLiftedNodes( i, pool, trash );

        for ( Set<Aggregate*>::iterator agg = i->type().aggregates().begin(); agg != i->type().aggregates().end(); ++agg ) {
          pool.insert( __getAggPotential( i, *agg ) );
        }

        try {
          InstanceBayesNet bn( *i );
          VariableElimination<prm_float> inf( bn );

          if ( delayedVars ) {
            std::vector<NodeId> elim;

            for ( std::vector<NodeId>::iterator iter = __getElimOrder( i->type() ).begin(); iter != __getElimOrder( i->type() ).end(); ++iter ) {
              if ( not delayedVars->exists( *iter ) ) {
                elim.push_back( *iter );
              }
            }

            inf.eliminateNodes( elim, pool, trash );
          } else {
            inf.eliminateNodes( __getElimOrder( i->type() ), pool, trash );
          }
        } catch ( NotFound& ) {
          // Raised if there is no inner nodes to eliminate
        }
      }

      // Eliminating delayed variables, if any
      if ( __delayedVariables.exists( i ) ) {
        __eliminateDelayedVariables( i, pool, trash );
      }
    }

    void
    SVE::__eliminateNodesUpward( const Instance* i, BucketSet& pool, BucketSet& trash, List<const Instance*>& elim_list, Set<const Instance*>& ignore, Set<const Instance*>& eliminated ) {
      // Downward elimination
      ignore.insert( i );

      for ( Instance::InvRefConstIterator iter = i->beginInvRef(); iter != i->endInvRef(); ++iter ) {
        typedef std::vector< std::pair<Instance*, std::string> >::const_iterator pair_iter;

        for ( pair_iter child = ( **iter ).begin(); child != ( **iter ).end(); ++child ) {
          if ( not ignore.exists( child->first ) ) {
            __eliminateNodesDownward( i, child->first, pool, trash, elim_list, ignore, eliminated );
          }
        }
      }

      // Eliminating all nodes in i instance
      __variableElimination( i, pool, trash );
      eliminated.insert( i );
      // Eliminating instance in elim_list
      List<const Instance*> tmp_list;

      while ( not elim_list.empty() ) {
        if ( __checkElimOrder( i, elim_list.front() ) ) {
          if ( not ignore.exists( elim_list.front() ) ) {
            __eliminateNodesDownward( i, elim_list.front(), pool, trash, elim_list, ignore, eliminated );
          }
        } else {
          tmp_list.insert( elim_list.front() );
        }

        elim_list.popFront();
      }

      // Upward elimination
      for ( Set<SlotChain*>::iterator sc = i->type().slotChains().begin(); sc != i->type().slotChains().end(); ++sc ) {
        for ( Set<Instance*>::iterator parent = i->getInstances( ( **sc ).id() ).begin(); parent != i->getInstances( ( **sc ).id() ).end(); ++parent ) {
          if ( not ignore.exists( *parent ) ) {
            __eliminateNodesUpward( *parent, pool, trash, tmp_list, ignore, eliminated );
          }
        }
      }
    }

    void
    SVE::__eliminateNodesWithEvidence( const Instance* i, BucketSet& pool, BucketSet& trash, Set<NodeId>* delayedVars ) {
      // First we check if evidences are on inner nodes
      bool inner = false;

      for ( EMapIterator e = evidence( i ).begin(); e != evidence( i ).end(); ++e ) {
        inner = i->type().isInputNode( i->get( e.key() ) ) or i->type().isInnerNode( i->get( e.key() ) );

        if ( inner ) { break; }
      }

      // Evidence on inner nodes
      if ( inner ) {
        BucketSet tmp_pool;
        __insertEvidence( i, tmp_pool );

        // We need a local to not eliminate queried inner nodes of the same class
        for ( Instance::const_iterator attr = i->begin(); attr != i->end(); ++attr ) {
          tmp_pool.insert( &( ( **attr ).cpf() ) );
        }

        InstanceBayesNet bn( *i );
        DefaultTriangulation t( &( bn.moralGraph() ), &( bn.modalities() ) );
        const std::vector<NodeId>& full_elim_order = t.eliminationOrder();
        VariableElimination<prm_float> inf( bn );
        // Removing Output nodes of elimination order
        std::vector<NodeId> inner_elim_order;
        std::vector<NodeId> output_elim_order;

        if ( delayedVars ) {
          for ( size_t idx = 0; idx < full_elim_order.size(); ++idx ) {
            if ( not i->type().isOutputNode( i->get( full_elim_order[idx] ) ) ) {
              inner_elim_order.push_back( full_elim_order[idx] );
            } else if ( not delayedVars->exists( full_elim_order[idx] ) ) {
              output_elim_order.push_back( full_elim_order[idx] );
            }
          }
        } else {
          for ( size_t idx = 0; idx < full_elim_order.size(); ++idx ) {
            if ( not i->type().isOutputNode( i->get( full_elim_order[idx] ) ) ) {
              inner_elim_order.push_back( full_elim_order[idx] );
            } else {
              output_elim_order.push_back( full_elim_order[idx] );
            }
          }
        }

        inf.eliminateNodes( inner_elim_order, tmp_pool, trash );

        // Now we add the new potentials in pool and eliminate output nodes
        for ( BucketSetIterator iter = tmp_pool.begin(); iter != tmp_pool.end(); ++iter ) {
          pool.insert( *iter );
        }

        if ( not output_elim_order.empty() ) {
          inf.eliminateNodes( output_elim_order, pool, trash );
        }
      } else {
        InstanceBayesNet bn( *i );
        VariableElimination<prm_float> inf( bn );
        __insertEvidence( i, pool );
        __insertLiftedNodes( i, pool, trash );

        for ( Set<Aggregate*>::iterator agg = i->type().aggregates().begin();
              agg != i->type().aggregates().end(); ++agg ) {
          pool.insert( __getAggPotential( i, *agg ) );
        }

        try {
          if ( delayedVars ) {
            std::vector<NodeId> elim;

            for ( std::vector<NodeId>::iterator iter = __getElimOrder( i->type() ).begin(); iter != __getElimOrder( i->type() ).end(); ++iter ) {
              if ( not delayedVars->exists( *iter ) ) {
                elim.push_back( *iter );
              }
            }

            inf.eliminateNodes( elim, pool, trash );
          } else {
            inf.eliminateNodes( __getElimOrder( i->type() ), pool, trash );
          }
        } catch ( NotFound& ) {
          GUM_ERROR( FatalError, "there should be at least one node here." );
        }
      }
    }

    void
    SVE::__insertLiftedNodes( const Instance* i, BucketSet& pool, BucketSet& trash ) {
      SVE::BucketSet* lifted_pool = 0;

      try {
        lifted_pool = __lifted_pools[&( i->type() )];
      } catch ( NotFound& ) {
        __initLiftedNodes( i->type() );
        lifted_pool = __lifted_pools[&( i->type() )];
      }

      for ( SVE::BucketSet::iterator iter = lifted_pool->begin(); iter != lifted_pool->end(); ++iter ) {
        Potential<prm_float>* pot = copyPotential( i->bijection(), **iter );
        pool.insert( pot );
        trash.insert( pot );
      }
    }

    void
    SVE::__initLiftedNodes( const Class& c ) {
      BucketSet* lifted_pool = new BucketSet();
      __lifted_pools.insert( &c, lifted_pool );
      NodeSet inners, outers;
      const Set<NodeId>* parents = 0;

      for ( DAG::NodeIterator node = c.dag().beginNodes(); node != c.dag().endNodes(); ++node ) {
        if ( ClassElement::isAttribute( c.get( *node ) ) ) {
          if ( c.isOutputNode( c.get( *node ) ) )
            outers.insert( *node );
          else if ( not outers.exists( *node ) )
            inners.insert( *node );

          lifted_pool->insert( const_cast<Potential<prm_float>*>( &( c.get( *node ).cpf() ) ) );
        } else if ( ClassElement::isAggregate( c.get( *node ) ) ) {
          outers.insert( *node );
          // We need to put in the output_elim_order aggregator's parents which are innner nodes
          parents = &( c.dag().parents( *node ) );

          for ( Set<NodeId>::const_iterator prnt = parents->begin(); prnt != parents->end(); ++prnt ) {
            if ( ClassElement::isAttribute( c.get( *prnt ) ) and c.isInnerNode( c.get( *prnt ) ) ) {
              inners.erase( *prnt );
              outers.insert( *prnt );
            }
          }
        }
      }

      // Now we proceed with the elimination of inner attributes
      ClassBayesNet bn( c );
      List<NodeSet> partial_ordering;

      if ( inners.size() )
        partial_ordering.push_back( inners );

      if ( outers.size() )
        partial_ordering.push_back( outers );

      PartialOrderedTriangulation t( &( bn.moralGraph() ), &( bn.modalities() ), &partial_ordering );

      for ( size_t idx = 0; idx < inners.size(); ++idx )
        eliminateNode( &( c.get( t.eliminationOrder()[idx] ).type().variable() ), *lifted_pool, __lifted_trash );

      // If there is not only inner and input Attributes
      if ( outers.size() ) {
        __elim_orders.insert( &c,
                              new std::vector<NodeId>( t.eliminationOrder().begin() + inners.size(), t.eliminationOrder().end() ) );
      }
    }

    void
    SVE::__initElimOrder() {
      CDG cdg( *_prm );
      __class_elim_order = new Sequence<const ClassElementContainer*>();
      std::list<NodeId> l;

      for ( DAG::NodeIterator node = cdg.dag().beginNodes(); node != cdg.dag().endNodes(); ++node )
        if ( cdg.dag().parents( *node ).empty() ) l.push_back( *node );

      Set<NodeId> visited_node;

      while ( not l.empty() ) {
        visited_node.insert( l.front() );

        if ( not __class_elim_order->exists( cdg.get( l.front() ).first ) )
          __class_elim_order->insert( cdg.get( l.front() ).first );

        const NodeSet& children = cdg.dag().children( l.front() );

        for ( NodeSetIterator child = children.begin();
              child != children.end(); ++child )
          if ( not visited_node.contains( *child ) ) l.push_back( *child );

        l.pop_front();
      }
    }

    void
    SVE::_marginal( const Chain& chain, Potential<prm_float>& m ) {
      const Instance* i = chain.first;
      const Attribute* elt = chain.second;
      SVE::BucketSet pool, trash;
      __eliminateNodes( i, elt->id(), pool, trash );
      m.fill( ( prm_float ) 1 );

      for ( SVE::BucketSetIterator iter = pool.begin(); iter != pool.end(); ++iter ) {
        if ( ( **iter ).contains( elt->type().variable() ) ) {
          m.multiplicateBy( **iter );
        }

        // if ((**iter).nbrDim() > 1) {
        //   std::string dot = ".";
        //   std::string t = "true";
        //   std::string f = "false";
        //   std::string none = "NONE";
        //   GUM_TRACE_VAR((**iter).nbrDim());
        //   GUM_TRACE(chain.first->name() + dot + chain.second->safeName());
        //   for (System::const_iterator jter = _sys->begin(); jter != _sys->end(); ++jter) {
        //     for (Instance::iterator a = (**jter).begin(); a != (**jter).end(); ++a) {
        //       if ((**iter).contains((**a).type().variable())) {
        //         GUM_TRACE((**jter).name() + dot + (**a).safeName());
        //         // try {
        //         //   std::cerr << "Instance eliminated in:       " << __debug[*jter] << std::endl;
        //         // } catch (NotFound&) {
        //         //   std::cerr << "Instance eliminated in:       " << none << std::endl;
        //         // }
        //         std::cerr << "Attribute has refered attr:   " << ((**jter).hasRefAttr((**a).id())) << std::endl;
        //         std::cerr << "Attribute has evidence:       " << (hasEvidence(*jter)) << std::endl;
        //         std::cerr << "Attribute parents number:     " << ((**jter).type().dag().parents((**a).id()).size()) << std::endl;
        //         std::cerr << "Attribute children number:    " << ((**jter).type().dag().children((**a).id()).size()) << std::endl;
        //       }
        //     }
        //   }
        // }
      }

      m.normalize();

      for ( SVE::BucketSetIterator iter = trash.begin(); iter != trash.end(); ++iter ) {
        delete *iter;
      }
    }

    void
    SVE::_joint( const std::vector<Chain>& queries, Potential<prm_float>& j ) {
      GUM_ERROR( FatalError, "Not implemented." );
    }

  } /* namespace prm */
} /* namespace gum */

