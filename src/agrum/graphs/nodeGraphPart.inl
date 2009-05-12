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
 * @brief Inline implementation of classes for directed edge sets
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

namespace gum {


  INLINE  NodeId NodeGraphPart::insertNode() {
    // only one tmp
    NodeId newNode=__nextNodeId();
    __nodes.insert( newNode );
    return newNode;
  }

  INLINE  void NodeGraphPart::insertNode( const NodeId id ) {
    __nodes.insert( id );
    if ( __max < id ) __max = id;
  }

  INLINE void NodeGraphPart::eraseNode( const NodeId node ) {
    __nodes.erase( node );
  }

  INLINE bool NodeGraphPart::empty() const {
    return __nodes.empty();
  }

  INLINE bool NodeGraphPart::exists( const NodeId node ) const {
    return __nodes.contains( node );
  }

  INLINE Size NodeGraphPart::size( ) const {
    return __nodes.size();
  }

  INLINE NodeId NodeGraphPart::maxId( ) const {
    return __max;
  }

  INLINE void NodeGraphPart::clear() {
    __nodes.clear();
  }

  INLINE const NodeSetIterator NodeGraphPart::beginNodes() const {
    return __nodes.begin();
  }

  INLINE const NodeSetIterator& NodeGraphPart::endNodes() const {
    return __nodes.end();
  }

  INLINE NodeId NodeGraphPart::__nextNodeId() {
    return ( ++__max );
  }

  INLINE const NodeSet& NodeGraphPart::nodes() const {
    return __nodes;
  }

  INLINE bool NodeGraphPart::operator==( const NodeGraphPart& p ) const {
    return __nodes==p.__nodes;
  }

  INLINE bool NodeGraphPart::operator!=( const NodeGraphPart& p ) const {
    return __nodes!=p.__nodes;
  }


} /* namespace gum */
