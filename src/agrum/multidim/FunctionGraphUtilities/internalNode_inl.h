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
 * @brief Inline implementation of the InternalNode class.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 * @author Jean-Christophe Magnan
 */

#include <agrum/multidim/FunctionGraphUtilities/internalNode.h>

namespace gum {

  INLINE
  Parent::Parent( NodeId pi, Idx m )
      : parentId( pi )
      , modality( m ) {}


  INLINE
  Parent::Parent( const Parent& src )
      : parentId( src.parentId )
      , modality( src.modality ) {}

  INLINE
  void* Parent::operator new( size_t s ) {
    return SmallObjectAllocator::instance().allocate( s );
  }

  INLINE
  void Parent::operator delete( void* p ) {
    SmallObjectAllocator::instance().deallocate( p, sizeof( Parent ) );
  }

  INLINE
  bool Parent::operator==( const Parent& comp ) const {
    if ( comp.parentId == this->parentId && comp.modality == this->modality )
      return true;
    return false;
  }

  INLINE
  bool Parent::operator!=( const Parent& comp ) const {
    return !( comp == *this );
  }

  INLINE
  Parent& Parent::operator=( const Parent& src ) {
    this->parentId = src.parentId;
    this->modality = src.modality;
    return *this;
  }

  INLINE
  void* InternalNode::operator new( size_t s ) {
    return SmallObjectAllocator::instance().allocate( s );
  }

  INLINE
  void InternalNode::operator delete( void* p ) {
    SmallObjectAllocator::instance().deallocate( p, sizeof( InternalNode ) );
  }

  INLINE
  const DiscreteVariable* InternalNode::nodeVar() const { return __nodeVar; }

  INLINE
  void InternalNode::setSon( Idx modality, NodeId son ) {
    __nodeSons[modality] = son;
  }

  INLINE
  NodeId InternalNode::son( Idx modality ) const { return __nodeSons[modality]; }

  INLINE
  Idx InternalNode::nbSons() const { return __nodeVar->domainSize(); }

  INLINE
  Link<Parent>* InternalNode::parents() { return __nodeParents.list(); }

  INLINE
  const Link<Parent>* InternalNode::parents() const {
    return __nodeParents.list();
  }
}  // namespace gum
