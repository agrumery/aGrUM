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
/** @file
 * @brief Inline implementation of the base node set class for graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

namespace gum {


  INLINE NodeGraphPart& NodeGraphPart::operator= ( const NodeGraphPart& p ) {
    // avoid self assignment
    if ( this != &p ) {
      clear (); // "virtual" flush of the nodes set
      __nodes=p.__nodes;
      __max=p.__max;
    }
    return *this;
  }

  INLINE NodeId NodeGraphPart::__nextNodeId() {
    return ( ++__max );
  }

  INLINE NodeId NodeGraphPart::nextNodeId() const {
    return 1+__max;
  }

  INLINE void NodeGraphPart::insertNode ( const NodeId id ) {
    if ( __max < id ) {
      __max = id;
    }
    else {
      if ( exists ( id ) )
        GUM_ERROR ( DuplicateElement,"This id is already used" );
    }

    __nodes.insert ( id );
    GUM_EMIT1 ( onNodeAdded,id );
  }

  // warning: do not try to use function insertNode ( const NodeId id ) within
  // function insertNode(): as both functions are virtual, this may create
  // bugs within the graphs hierarchy (i.e., virtual functions calling recursively
  // each other along the hierarchy) that are not easy to debug.
  INLINE  NodeId NodeGraphPart::insertNode() {
    // only one tmp
    NodeId newNode=__nextNodeId();
    __nodes.insert ( newNode );
    GUM_EMIT1 ( onNodeAdded,newNode );

    return newNode;
  }

  INLINE void NodeGraphPart::eraseNode ( const NodeId node ) {
    __nodes.erase ( node );
    GUM_EMIT1 ( onNodeDeleted,node );
  }

  INLINE bool NodeGraphPart::emptyNodes() const {
    return __nodes.empty();
  }

  INLINE bool NodeGraphPart::empty() const {
    return emptyNodes();
  }

  INLINE bool NodeGraphPart::existsNode ( const NodeId node ) const {
    return __nodes.contains ( node );
  }

  INLINE bool NodeGraphPart::exists ( const NodeId node ) const {
    return existsNode ( node );
  }

  INLINE Size NodeGraphPart::sizeNodes( ) const {
    return __nodes.size();
  }

  INLINE Size NodeGraphPart::size( ) const {
    return sizeNodes();
  }

  INLINE NodeId NodeGraphPart::maxId( ) const {
    return __max;
  }

  INLINE void NodeGraphPart::clearNodes() {
    if ( onNodeDeleted.hasListener() ) {
      NodeSet tmp=__nodes;
      __nodes.clear();

      for ( NodeSet::iterator n=tmp.begin();n!=tmp.end();++n ) {
        GUM_EMIT1 ( onNodeDeleted,*n );
      }
    }
    else {
      __nodes.clear();
    }
    __max = 0;
  }

  // warning: clear is an alias for clearNodes but it should never be the case
  // that the code of clear is just a call to clearNodes: as both methods are
  // virtual, this could induce bugs within the graphs hierarchy (i.e., virtual
  // functions calling recursively each other along the hierarchy) that are not
  // easy to debug. Hence, the code of clearNodes should be duplicated here.
  INLINE void NodeGraphPart::clear() {
    if ( onNodeDeleted.hasListener() ) {
      NodeSet tmp=__nodes;
      __nodes.clear();

      for ( NodeSet::iterator n=tmp.begin();n!=tmp.end();++n ) {
        GUM_EMIT1 ( onNodeDeleted,*n );
      }
    }
    else {
      __nodes.clear();
    }
    __max = 0;
  }
  
  INLINE const NodeSetIterator NodeGraphPart::beginNodes() const {
    return __nodes.begin();
  }

  INLINE const NodeSetIterator& NodeGraphPart::endNodes() const {
    return __nodes.end();
  }

  INLINE const NodeSet& NodeGraphPart::nodes() const {
    return __nodes;
  }

  INLINE bool NodeGraphPart::operator== ( const NodeGraphPart& p ) const {
    return __nodes==p.__nodes;
  }

  INLINE bool NodeGraphPart::operator!= ( const NodeGraphPart& p ) const {
    return __nodes!=p.__nodes;
  }


} /* namespace gum */
