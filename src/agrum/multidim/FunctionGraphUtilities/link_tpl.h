/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                              //
 *                                                                        //
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or    //
 *   (at your option) any later version.                                  //
 *                                                                        //
 *   This program is distributed in the hope that it will be useful,      //
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       //
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
 *   GNU General Public License for more details.                         //
 *                                                                        //
 *   You should have received a copy of the GNU General Public License    //
 *   along with this program; if not, write to the                        //
 *   Free Software Foundation, Inc.,                                      //
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.            //
 ***************************************************************************/
/**
 * @file
 * @brief Template methods of MultiDimFunctionGraph.
 *
 * @author Jean-Christophe Magnan
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 */

#include <agrum/multidim/FunctionGraphUtilities/link.h>

namespace gum {

  // Constructor
  template <typename T>
  INLINE Link<T>::Link( const T& elem )
      : __element( elem ) {
    GUM_CONSTRUCTOR( Link )
  }

  // Constructor
  template <typename T>
  INLINE Link<T>::Link( const T& elem, Link<T>* nextLink )
      : __element( elem )
      , __nextLink( nextLink ) {
    GUM_CONSTRUCTOR( Link )
  }

  // Destructor
  template <typename T>
  INLINE Link<T>::~Link() {
    GUM_DESTRUCTOR( Link )
  }

  template <typename T>
  INLINE void* Link<T>::operator new( size_t s ) {
    return SmallObjectAllocator::instance().allocate( Size( s ) );
  }

  template <typename T>
  INLINE void Link<T>::operator delete( void* p ) {
    SmallObjectAllocator::instance().deallocate( p, sizeof( Link<T> ) );
  }

  template <typename T>
  INLINE const T& Link<T>::element() const {
    return __element;
  }

  template <typename T>
  INLINE T& Link<T>::element() {
    return __element;
  }

  template <typename T>
  INLINE const Link<T>* Link<T>::nextLink() const {
    return __nextLink;
  }

  template <typename T>
  INLINE Link<T>* Link<T>::nextLink() {
    return __nextLink;
  }

  template <typename T>
  INLINE void Link<T>::setNextLink( Link<T>* newLink ) {
    __nextLink = newLink;
  }

  // Constructor
  template <typename T>
  INLINE LinkedList<T>::LinkedList() {
    GUM_CONSTRUCTOR( LinkedList )
    __firstLink = nullptr;
  }

  // Destructor
  template <typename T>
  INLINE LinkedList<T>::~LinkedList() {
    clear();
    GUM_DESTRUCTOR( LinkedList )
  }

  template <typename T>
  INLINE void* LinkedList<T>::operator new( size_t s ) {
    return SmallObjectAllocator::instance().allocate( Size( s ) );
  }

  template <typename T>
  INLINE void LinkedList<T>::operator delete( void* p ) {
    SmallObjectAllocator::instance().deallocate( p, sizeof( LinkedList<T> ) );
  }

  template <typename T>
  INLINE const Link<T>* LinkedList<T>::list() const {
    return __firstLink;
  }

  template <typename T>
  INLINE Link<T>* LinkedList<T>::list() {
    return __firstLink;
  }

  template <typename T>
  void LinkedList<T>::clear() {
    Link<T>* curLink = __firstLink;
    Link<T>* nl = nullptr;
    while ( curLink ) {
      nl = curLink->nextLink();
      delete curLink;
      curLink = nl;
    }
  }

  template <typename T>
  INLINE void LinkedList<T>::addLink( const T& elem ) {
    Link<T>* newLink = new Link<T>( elem, __firstLink );
    __firstLink = newLink;
  }

  template <typename T>
  INLINE void LinkedList<T>::searchAndRemoveLink( const T& elem ) {

    Link<T>* curLink = __firstLink;
    Link<T>* prevLink = nullptr;
    while ( curLink && curLink->element() != elem ) {
      prevLink = curLink;
      curLink = curLink->nextLink();
    }
    if ( curLink ) {
      if ( prevLink )
        prevLink->setNextLink( curLink->nextLink() );
      else
        __firstLink = curLink->nextLink();
      delete curLink;
    }
  }

}  // namespace gum
