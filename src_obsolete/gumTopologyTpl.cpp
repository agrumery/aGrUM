/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *   test $Id: $                                                           *
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

/** @file
 * @brief Outlined implementation gumTopologyTpl
 *
 * @author Lionel Torti
 */


#include <agrum/gumTopologyTpl.h>

#ifdef GUM_NO_INLINE
#include "gumTopologyTpl.inl"
#endif /* GUM_NO_INLINE */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


/* ============================================================================== */
/* ===                 CLASS FOR REPRESENTING A TOPOLOGY ON VARIABLE          === */
/* ============================================================================== */

/* ============================================================================== */
/* ===                       CONSTRUCTORS AND DESTRUCTOR                      === */
/* ============================================================================== */
// Copy constructor
gumTopologyTpl::gumTopologyTpl(const gumTopologyTpl& source): gumDiscreteVariableListener(),
  __rebuildTopologicalOrder(true), __topologicalOrder(NULL),
  __rebuildMoralGraph(true), __moralGraph(NULL),
  __rebuiltEndIterator(true), __endIter(NULL)
{
  GUM_CONSTRUCTOR( gumTopologyTpl );

  _dag = source._dag;
  __nextId = source.__nextId;

  gumDiscreteVariable *copyPtr;
  // Copying variables and ids
  for ( gumNodePROPERTY(gumDiscreteVariable*)::iterator iter = source._variableMap.begin();
        iter != source._variableMap.end();
        ++iter ) {
    copyPtr = (*iter)->copyFactory();
    copyPtr->setDiscreteVariableListener(( gumDiscreteVariableListener* ) this );
    _variableMap.insert( iter.key(), copyPtr );
  }
}

// Destructor
gumTopologyTpl::~gumTopologyTpl()
{
  GUM_DESTRUCTOR( gumTopologyTpl );

  for ( gumNodePROPERTY(gumDiscreteVariable*)::iterator iter = _variableMap.begin();
        iter != _variableMap.end();
        ++iter ) {
    delete *iter;
  }

  if ( __topologicalOrder != NULL )
    delete __topologicalOrder;

  if (__moralGraph != NULL)
    delete __moralGraph;

  if ( __endIter != NULL )
    delete __endIter;
}

/* ============================================================================== */
/* ===                         ADD AND ERASE METHODS                          === */
/* ============================================================================== */

//Clear every variables, nodes, and arcs of this topology.
void
gumTopologyTpl::clear() {
  _dag.clearNodes();

  for ( gumNodePROPERTY(gumDiscreteVariable*)::iterator iter = _variableMap.begin();
        iter != _variableMap.end();
        ++iter ) {
    delete *iter;
  }

  _variableMap.clear();

  _topologyChanged();
}

/* ============================================================================== */
/* ===                        CLASS SPECEFIC METHODS                          === */
/* ============================================================================== */
/**
 * The node's id are coherent with the variables and nodes of the topology.
 *
 * @return The moral graph built from this topology.
 */
const gumUndiGraphTpl<gumNode, gumEdge>&
gumTopologyTpl::getMoralGraph() const
{
  if (__moralGraph == NULL) {
    __moralGraph = new gumUndiGraphTpl<gumNode, gumEdge>();
  }

  if (__rebuildMoralGraph)
  {
    __rebuildMoralGraph = false;
    __moralGraph->clearNodes();
    __moralGraph->clearEdges();

    // fill the moral graph with its nodes
    for ( gumNodeListIterator<gumNode> iter = _dag.beginNodes(); iter != _dag.endNodes(); ++iter ) {
      __moralGraph->insertNode(*iter);
    }

    // transform the arcs into edges
    for ( gumArcListIterator<gumArc> iter = _dag.beginArcs(); iter != _dag.endArcs(); ++iter ) {
      __moralGraph->insertEdge( gumEdge( iter->first(), iter->second() ) );
    }

    const gumList< gumRefPtr<gumArc> > *parentsList;
    // mary the parents
    for ( gumNodeListIterator<gumNode> iter = _dag.beginNodes(); iter != _dag.endNodes(); ++iter ) {
      parentsList = &_dag.getParentArcs(iter.getID());

      for (gumIdx i = 0; i < parentsList->size(); i++) {
        for (gumIdx j = i + 1; j < parentsList->size(); j++) {
          if (! __moralGraph->existsEdge((*parentsList)[i]->tail(), (*parentsList)[j]->tail())) {
            __moralGraph->insertEdge(gumEdge((*parentsList)[i]->tail(), (*parentsList)[j]->tail()));
          }
        }
      }

    }
  }
  return *__moralGraph;
}

/**
 * The topological order stays the same as long as no variable or arcs are
 * added or erased from the topology.
 *
 * A lazy instantiation is used, so the topological order is computed only
 * on demand.
 *
 * @return Returns a constant reference on a list of the variable's
 * id in topological order.
 */
const gumList<gumId>&
gumTopologyTpl::getTopologicalOrder() const
{
  // Lazy initialization
  if ( __topologicalOrder == NULL )
    __topologicalOrder = new gumList<gumId>();

  if ( __rebuildTopologicalOrder ) {
    __rebuildTopologicalOrder = false;

    __topologicalOrder->clear();
    gumNodeList<gumNode> nodeList;
    nodeList.shallowCopy( _dag.getNodes() );

    // Add all root nodes in the list
    __getRootTopologyLevel( nodeList );

    // Check if the graph has at least one root node

    if ( __topologicalOrder->empty() )
      GUM_ERROR( gumOperationNotAllowed, "Empty topology can not topological order." );

    // Add nodes in topological order
    while ( ! nodeList.empty() )
      __getNextTopologyLevel( nodeList );
  }

  return *__topologicalOrder;
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

