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
 * @brief Implementation of gum::InterfaceGraph.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/prm/gspan/interfaceGraph.h>

#ifdef GUM_NO_INLINE
#include <agrum/prm/gspan/interfaceGraph.inl>
#endif // GUM_NO_INLINE

namespace gum {
  namespace prm {
    namespace gspan {

      InterfaceGraph::InterfaceGraph ( const System& sys ) :
        __sys ( &sys ), __labels ( new Bijection<Idx, LabelData*>() ),
        __counter ( 0 ), __erase_flag ( true ) {
        GUM_CONSTRUCTOR ( InterfaceGraph );
        HashTable<std::string, LabelData*> label_map;

        // We need to add each instance in __graph
        for ( System::const_iterator iter = sys.begin(); iter != sys.end(); ++iter ) {
          NodeData* node = new NodeData();
          node->n = *iter;
          __label ( node, label_map );
          __graph.insertNode ( iter.key() );
          __idMap.insert ( node->n, iter.key() );
          __nodes.insert ( iter.key(), node );
        }

        NodeData* data = 0;
        NodeData* u = 0;
        NodeData* v = 0;

        for ( NodeProperty<NodeData*>::iterator node = __nodes.begin(); node != __nodes.end(); ++node ) {
          data = *node;

          for ( Set<SlotChain*>::iterator iter = data->n->type().slotChains().begin(); iter != data->n->type().slotChains().end(); ++iter ) {
            for ( Set<Instance*>::iterator jter = data->n->getInstances ( ( **iter ).id() ).begin(); jter != data->n->getInstances ( ( **iter ).id() ).end(); ++jter ) {
              u = ( __nodes[__idMap[*jter]]->l < data->l ) ?__nodes[__idMap[*jter]]:data;
              v = ( u != data ) ?data:__nodes[__idMap[*jter]];

              if ( not __graph.existsEdge ( __idMap[u->n], __idMap[v->n] ) ) {
                EdgeData* edge = new EdgeData();
                edge->u = u->n;
                edge->l_u = u->l;
                edge->v = v->n;
                edge->l_v = v->l;
                __label ( edge, label_map );
                __graph.insertEdge ( __idMap[u->n], __idMap[v->n] );
                __edges.insert ( Edge ( __idMap[u->n], __idMap[v->n] ), edge );
              }
            }
          }
        }
      }

      InterfaceGraph::InterfaceGraph ( const InterfaceGraph& source ) :
        __sys ( source.__sys ), __graph ( source.__graph ), __nodes ( source.__nodes ),
        __idMap ( source.__idMap ), __edges ( source.__edges ),
        __labels ( new Bijection<Idx, LabelData*> ( * ( source.__labels ) ) ),
        __nodeMap ( source.__nodeMap ), __edgeMap ( source.__edgeMap ),
        __counter ( source.__counter ), __erase_flag ( false ) {
        GUM_CONS_CPY ( InterfaceGraph );
      }

      InterfaceGraph::~InterfaceGraph() {
        GUM_DESTRUCTOR ( InterfaceGraph );

        if ( __erase_flag ) {
          for ( NodeProperty<NodeData*>::iterator iter = __nodes.begin();
                iter != __nodes.end(); ++iter ) {
            delete *iter;
          }

          for ( Property<EdgeData*>::onEdges::iterator iter = __edges.begin();
                iter != __edges.end(); ++iter ) {
            delete *iter;
          }

          typedef HashTable<LabelData*, Set<NodeData*>*>::iterator NodeMapIter;

          for ( NodeMapIter iter = __nodeMap.begin(); iter != __nodeMap.end(); ++iter ) {
            delete *iter;
            delete iter.key();
          }

          typedef HashTable<LabelData*, Set<EdgeData*>*>::iterator EdgeMapIter;

          for ( EdgeMapIter iter = __edgeMap.begin(); iter != __edgeMap.end(); ++iter ) {
            delete *iter;
            delete iter.key();
          }
        }

        delete __labels;
      }

      InterfaceGraph&
      InterfaceGraph::operator= ( const InterfaceGraph& source ) {
        GUM_ERROR ( FatalError, "not implemented" );
      }

      void
      InterfaceGraph::__label ( NodeData* node, HashTable<std::string, LabelData*>& label_map ) {
        unsigned long size = 1;
        std::stringstream sBuff;
        sBuff << node->n->type().name();

        // First we search for multiple inputs
        for ( Set<SlotChain*>::iterator iter = node->n->type().slotChains().begin();
              iter != node->n->type().slotChains().end(); ++iter ) {
          if ( ( **iter ).isMultiple() ) {
            sBuff << "-" << node->n->getInstances ( ( **iter ).id() ).size();
            sBuff << ( **iter ).name();
            size *= node->n->getInstances ( ( **iter ).id() ).size() *
                    ( **iter ).lastElt().type().variable().domainSize();
          } else {
            size *= ( **iter ).lastElt().type().variable().domainSize();
          }
        }

        // Second we search for active outputs
        //for ( DAG::NodeIterator iter = node->n->type().dag().beginNodes(); iter != node->n->type().dag().endNodes(); ++iter ) {
        for ( const auto nn : node->n->type().dag().nodes() ) {
          if ( node->n->type().isOutputNode ( node->n->type().get ( nn ) ) ) {
            try {
              sBuff << "-" << node->n->getRefAttr ( nn ).size() << node->n->get ( nn ).name();
              size *= node->n->get ( nn ).type().variable().domainSize();
            } catch ( NotFound& ) {
              // (nn) is an inactive output node
            }
          }
        }

        // Label is ready
        if ( not label_map.exists ( sBuff.str() ) ) {
          LabelData* label = new LabelData();
          label_map.insert ( sBuff.str(), label );
          label->id = ++__counter;
          label->tree_width = size;
          label->l = sBuff.str();
          __labels->insert ( label->id, label );
          __nodeMap.insert ( label, new Set<NodeData*>() );
        }

        node->l = label_map[sBuff.str()];
        __nodeMap[node->l]->insert ( node );
      }

      void
      InterfaceGraph::__label ( EdgeData* edge, HashTable<std::string, LabelData*>& label_map ) {
        unsigned long size = 1;
        std::stringstream sBuff;
        sBuff << edge->u->type().name() << "-" << edge->v->type().name();

        // First looking for edge->u output nodes in v
        for ( Set<SlotChain*>::iterator iter = edge->u->type().slotChains().begin();
              iter != edge->u->type().slotChains().end(); ++iter ) {
          if ( edge->u->getInstances ( ( **iter ).id() ).exists ( edge->v ) ) {
            sBuff << "-" << edge->v->type().name() << "." << ( **iter ).lastElt().name();
            size *= ( **iter ).lastElt().type().variable().domainSize();
          }
        }

        // Second looking for edge->v output nodes in u
        for ( Set<SlotChain*>::iterator iter = edge->v->type().slotChains().begin();
              iter != edge->v->type().slotChains().end(); ++iter ) {
          if ( edge->v->getInstances ( ( **iter ).id() ).exists ( edge->u ) ) {
            sBuff << "-" << edge->u->type().name() << "." << ( **iter ).lastElt().name();
            size *= ( **iter ).lastElt().type().variable().domainSize();
          }
        }

        // Label is ready
        if ( not label_map.exists ( sBuff.str() ) ) {
          LabelData* label = new LabelData();
          label_map.insert ( sBuff.str(), label );
          label->id = ++__counter;
          label->l = sBuff.str();
          label->tree_width = size;
          __labels->insert ( label->id, label );
          __edgeMap.insert ( label, new Set<EdgeData*>() );
        }

        edge->l = label_map[sBuff.str()];
        __edgeMap[edge->l]->insert ( edge );
      }

    } /* namespace gspan */
  } /* namespace prm */
} /* namespace gum */

