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
 * @brief Implementation of the DFSTree class.
 *
 * @author Lionel TORTI
 */

#include <agrum/prm/gspan/DFSTree.h>

#ifdef GUM_NO_INLINE
#include <agrum/prm/gspan/searchStrategy.inl>
#endif

namespace gum {
  namespace prm {
    namespace gspan {

      double
      SearchStrategy::_computeCost( const Pattern& p ) {
        double cost = 0;
        const Sequence<Instance*>& seq = **( _tree->data( p ).iso_map.begin() );
        const Set<SlotChain*>* chains = 0;
        const Set<Instance*>* instances = 0;
        Sequence<ClassElement*> input_set;

        for ( Sequence<Instance*>::iterator iter = seq.begin(); iter != seq.end(); ++iter ) {
          chains = &( ( **iter ).type().slotChains() );

          for ( Set<SlotChain*>::iterator input = chains->begin(); input != chains->end(); ++input ) {
            instances = &( ( **iter ).getInstances( ( **input ).id() ) );

            for ( Set<Instance*>::iterator jter = instances->begin(); jter != instances->end(); ++jter ) {
              if ( ( not seq.exists( *jter ) ) and ( not input_set.exists( &( ( *jter )->get( ( *input )->lastElt().safeName() ) ) ) ) ) {
                cost += std::log( ( *input )->type().variable().domainSize() );
                input_set.insert( &( ( *jter )->get( ( *input )->lastElt().safeName() ) ) );
              }
            }
          }

          for ( Instance::InvRefIterator vec = ( **iter ).beginInvRef(); vec != ( **iter ).endInvRef(); ++vec ) {
            for ( std::vector< std::pair<Instance*, std::string> >::iterator inverse = ( **vec ).begin(); inverse != ( **vec ).end(); ++inverse ) {
              if ( not seq.exists( inverse->first ) ) {
                cost += std::log( ( *iter )->get( vec.key() ).type().variable().domainSize() );
                break;
              }
            }
          }
        }

        return cost;
      }

      void
      StrictSearch::__buildPatternGraph( StrictSearch::PData& data, Set<Potential<prm_float>*>& pool, const Sequence<Instance*>& match ) {
        for ( Sequence<Instance*>::const_iterator inst = match.begin(); inst != match.end(); ++inst ) {
          for ( Instance::const_iterator attr = ( **inst ).begin(); attr != ( **inst ).end(); ++attr ) {
            // Adding the node
            NodeId id = data.graph.insertNode();
            data.node2attr.insert( id, __str( *inst, *attr ) );
            data.mod.insert( id, ( **attr ).type()->domainSize() );
            data.vars.insert( id, &( ( **attr ).type().variable() ) );
            pool.insert( const_cast<Potential<prm_float>*>( &( ( **attr ).cpf() ) ) );
          }
        }

        // Second we add edges and nodes to inners or outputs
        for ( Sequence<Instance*>::const_iterator inst = match.begin(); inst != match.end(); ++inst ) {
          for ( Instance::const_iterator attr = ( **inst ).begin(); attr != ( **inst ).end(); ++attr ) {
            NodeId node = data.node2attr.first( __str( *inst, *attr ) );
            bool found = false; // If this is set at true, then node is an outer node
            // Children existing in the instance type's DAG
            const NodeSet& chldrn = ( **inst ).type().dag().children( ( **attr ).id() );

            for ( NodeSetIterator chld = chldrn.begin(); chld != chldrn.end(); ++chld ) {
              data.graph.insertEdge( node,
                                     data.node2attr.first( __str( *inst, ( **inst ).get( *chld ) ) ) );
            }

            // Parents existing in the instance type's DAG
            const NodeSet& prnts  = ( **inst ).type().dag().parents( ( **attr ).id() );

            for ( NodeSetIterator prnt = prnts.begin(); prnt != prnts.end(); ++prnt ) {
              switch ( ( **inst ).type().get( *prnt ).elt_type() ) {
                case ClassElement::prm_attribute:
                case ClassElement::prm_aggregate: {
                  data.graph.insertEdge( node, data.node2attr.first( __str( *inst, ( **inst ).get( *prnt ) ) ) );
                  break;
                }

                case ClassElement::prm_slotchain: {
                  const Set<Instance*>& ref = ( **inst ).getInstances( *prnt );

                  for ( Set<Instance*>::const_iterator jnst = ref.begin(); jnst != ref.end(); ++jnst )
                    if ( match.exists( *jnst ) )
                      data.graph.insertEdge( node, data.node2attr.first( __str( *jnst, static_cast<const SlotChain&>( ( **inst ).type().get( *prnt ) ) ) ) );

                  break;
                }

                default: { /* Do nothing */ }
              }
            }

            // Referring Attribute
            if ( ( **inst ).hasRefAttr( ( **attr ).id() ) ) {
              const std::vector< std::pair<Instance*, std::string> >& ref_attr = ( **inst ).getRefAttr( ( **attr ).id() );
              typedef std::vector< std::pair<Instance*, std::string> >::const_iterator VecIter;

              for ( VecIter pair = ref_attr.begin(); pair != ref_attr.end(); ++pair ) {
                if ( match.exists( pair->first ) ) {
                  NodeId id = pair->first->type().get( pair->second ).id();
                  const NodeSet& children = pair->first->type().dag().children( id );

                  for ( NodeSet::const_iterator child = children.begin(); child != children.end(); ++child )
                    data.graph.insertEdge( node, data.node2attr.first( __str( pair->first, pair->first->get( *child ) ) ) );
                } else {
                  found = true;
                }
              }
            }

            if ( found )  data.outputs.insert( node );
            else        data.inners.insert( node );
          }
        }
      }

      std::pair<Size, Size>
      StrictSearch::__elimination_cost( StrictSearch::PData& data, Set<Potential<prm_float>*>& pool ) {
        List<NodeSet> partial_order;

        if ( data.inners.size() )
          partial_order.insert( data.inners );

        if ( data.outputs.size() )
          partial_order.insert( data.outputs );

        PartialOrderedTriangulation t( &( data.graph ), &( data.mod ), &partial_order );
        const std::vector<NodeId>& elim_order = t.eliminationOrder();
        Size max( 0 ), max_count( 1 );
        Set<Potential<prm_float>*> trash;
        Potential<prm_float>* pot = 0;

        for ( size_t idx = 0; idx < data.inners.size(); ++idx ) {
          pot = new Potential<prm_float>( new MultiDimSparse<prm_float>( 0 ) );
          pot->add( *( data.vars.second( elim_order[idx] ) ) );
          trash.insert( pot );
          Set<Potential<prm_float>*> toRemove;

          for ( Set<Potential<prm_float>*>::iterator p = pool.begin(); p != pool.end(); ++p ) {
            if ( ( **p ).contains( *( data.vars.second( elim_order[idx] ) ) ) ) {
              for ( Sequence<const DiscreteVariable*>::const_iterator var = ( **p ).variablesSequence().begin();
                    var != ( **p ).variablesSequence().end(); ++var ) {
                try {
                  pot->add( **var );
                } catch ( DuplicateElement& ) { }
              }

              toRemove.insert( *p );
            }
          }

          if ( pot->domainSize() > max ) {
            max = pot->domainSize();
            max_count = 1;
          } else if ( pot->domainSize() == max ) {
            ++max_count;
          }

          for ( Set<Potential<prm_float>*>::iterator p = toRemove.begin(); p != toRemove.end(); ++p )
            pool.erase( *p );

          pot->erase( *( data.vars.second( elim_order[idx] ) ) );
        }

        for ( Set<Potential<prm_float>*>::iterator pot = trash.begin(); pot != trash.end(); ++pot )
          delete *pot;

        return std::make_pair( max, max_count );
      }

    } /* namespace gspan */
  } /* namespace prm */
} /* namespace gum */

