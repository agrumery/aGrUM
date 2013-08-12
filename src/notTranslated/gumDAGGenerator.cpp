/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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

#include <agrum/gumDAGGenerator.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

const std::string gumDAGGenerator::__allowedParam[] = {"NBR_NODES", "NBR_ROOT",
    "NBR_LEAF", "NBR_COMP",
    "MIN_NBR_PARENT", "MAX_NBR_PARENT",
    "MIN_NBR_CHILD", "MAX_NBR_CHILD"
                                                      };

//
// Set the configuration of this generator.
//
// A union is made with the current configuration. See the class documentation for
// a list of the parameters.
//
// @param configuration A hastable which key's are parameters and values are those of
// their respective parameter.
//
void
gumDAGGenerator::setConfiguration( const gumHashTable<std::string, std::string>& configuration ) {
  gumSize value; // Used only to check parameters type correctness

  for ( short i = 0; i < gumDAGGenerator::__nbrAllowedParam; i++ ) {
    if ( configuration.exists( gumDAGGenerator::__allowedParam[i] ) ) {
      // Checking if it is a legal value
      std::istringstream input( configuration[gumDAGGenerator::__allowedParam[i]] );

      if ( !( input >> value ) ) {
        GUM_ERROR( gumOperationNotAllowed,"Invalid parameter value."+ gumDAGGenerator::__allowedParam[i] );
      }

      try {
        _param[gumDAGGenerator::__allowedParam[i]] = configuration[gumDAGGenerator::__allowedParam[i]];
      } catch ( gumNotFound ) {
        _param.insert( gumDAGGenerator::__allowedParam[i], configuration[gumDAGGenerator::__allowedParam[i]] );
      }
    }
  }
}

//
// Generates nodes, and dispatch them in a set of root nodes, leaf nodes
// and middle nodes.
//
void
gumDAGGenerator::__generateNodes( gumDAG& dag,
                                  gumNodeSet& rootList,
                                  gumNodeSet& middleList,
                                  gumNodeSet& leafList ) const {
  for ( gumSize i = 0; i < getNbrNodes(); i++ ) {
    std::ostringstream os;
    os << i;
    gumNode aNode( i, os.str() );

    dag.insertNode( aNode );

    if ( i < getNbrRootNodes() ) {
      rootList.insert( aNode );
    } else if ( ( i >= getNbrRootNodes() ) && ( i < getNbrNodes() - getNbrLeafNodes() ) ) {
      middleList.insert( aNode );
    } else {
      leafList.insert( aNode );
    }
  }
}

//
// Generates nodes, for a DAG with several connex component.
//
void
gumDAGGenerator::__generateNodes( gumDAG& /*dag*/,  //@todo ??
                                  gumNodeSet& rootList,
                                  gumNodeSet& middleList,
                                  gumNodeSet& leafList ) const {
  for ( gumSize i = 0; i < getNbrNodes(); i++ ) {
    std::ostringstream os;
    os << i;
    gumNode aNode( i, os.str() );

    if ( i < getNbrRootNodes() ) {
      if ( i < getNbrConnexComponent() ) {
        rootLists[i]->pushBack( aNode );
      } else {
        rootLists[rand() % getNbrConnexComponent()]->pushBack( aNode );
      }
    } else if ( ( i >= getNbrRootNodes() ) && ( i < getNbrNodes() - getNbrLeafNodes() ) ) {
      middleLists[rand() % getNbrConnexComponent()]->pushBack( aNode );
    } else {
      leafLists[rand() % getNbrConnexComponent()]->pushBack( aNode );
    }
  }
}

//
// Generate arcs.
//
void
gumDAGGenerator::__generateArcs( gumDAG& dag,
                                 gumNodeSet& rootList,
                                 gumNodeSet& middleList,
                                 gumNodeSet& leafList ) const {

  if ( rootList.size() + middleList.size() < getMaxParents() ) {
    throw gumOperationNotAllowed( "Invalid parameters.", "NBR_ROOT + NBR_NODES - NBR_LEAF < MAX_NBR_PARENT" );
  }

  // Adding fathers at the leaf nodes
  for ( gumList<gumNode>::iterator iter = leafList.begin(); iter != leafList.end(); ++iter ) {
    gumSize nbrParents = ( rand() % ( getMaxParents() - getMinParents() + 1 ) ) + getMinParents();

    gumSize i = 0;

    do {
      gumSize aParent = ( rand() % ( rootList.size() + middleList.size() ) );
      gumNode aParentNode = ( aParent < rootList.size() ) ?rootList[aParent]:middleList[aParent - rootList.size()];
      gumArc aArc( iter->getID(), aParentNode.getID() );

      if ( ! dag.existsArc( aArc ) ) {
        dag.insertArc( aArc );
        i++;
      }
    } while ( i < nbrParents );
  }

  gumSize nbrParents = 0;

  gumSize idx = 0;
  gumList<gumNode> parents;
  gumList<gumNode> candidates( rootList );

  for ( gumList<gumNode>::iterator iter = middleList.begin(); iter != middleList.end(); ++iter ) {
    candidates.pushBack( *iter );
  }

  // Adding fathers at the middle nodes
  for ( gumList<gumNode>::iterator iter = middleList.begin(); iter != middleList.end(); ++iter ) {
    nbrParents = ( rand() % ( getMaxParents() - getMinParents() + 1 ) ) + getMinParents();
    parents.clear();
    __removeAncestors( *iter, dag, candidates );

    for ( gumSize i = 0; i < nbrParents; i++ ) {
      idx = rand() * candidates.size();
      dag.insertArc( gumArc( candidates[idx].getID(), iter->getID() ) );
      candidates.erase( idx );
    }

    __removeAncestors( *iter, dag, candidates, true );
  }
}

/*
 * Generate arcs, for a DAG with several connex component.
 */
void
gumDAGGenerator::__generateArcs( gumDAG& dag,
                                 gumNodeSet& rootList,
                                 gumNodeSet& middleList,
                                 gumNodeSet& leafList ) const {
  for ( gumSize i = 0; i < getNbrConnexComponent(); i++ ) {
    __generateArcs( dag, *rootLists[i], *middleLists[i], *leafLists[i] );
  }
}

#ifdef GUM_NO_INLINE
#include "gumDAGGenerator.inl"
#endif

#endif  // DOXYGEN_SHOULD_SKIP_THIS

