/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Template methods of MultiDimFunctionGraph.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 *
 */

#include <agrum/tools/multidim/utils/FunctionGraphUtilities/link.h>

namespace gum {

  // Constructor
  template < typename T >
  INLINE Link< T >::Link(const T& elem) : element__(elem) {
    GUM_CONSTRUCTOR(Link);
  }

  // Constructor
  template < typename T >
  INLINE Link< T >::Link(const T& elem, Link< T >* nextLink) :
      element__(elem), nextLink__(nextLink) {
    GUM_CONSTRUCTOR(Link);
  }

  // Destructor
  template < typename T >
  INLINE Link< T >::~Link() {
    GUM_DESTRUCTOR(Link);
  }

  template < typename T >
  INLINE void* Link< T >::operator new(size_t s) {
    return SmallObjectAllocator::instance().allocate(Size(s));
  }

  template < typename T >
  INLINE void Link< T >::operator delete(void* p) {
    SmallObjectAllocator::instance().deallocate(p, sizeof(Link< T >));
  }

  template < typename T >
  INLINE const T& Link< T >::element() const {
    return element__;
  }

  template < typename T >
  INLINE T& Link< T >::element() {
    return element__;
  }

  template < typename T >
  INLINE const Link< T >* Link< T >::nextLink() const {
    return nextLink__;
  }

  template < typename T >
  INLINE Link< T >* Link< T >::nextLink() {
    return nextLink__;
  }

  template < typename T >
  INLINE void Link< T >::setNextLink(Link< T >* newLink) {
    nextLink__ = newLink;
  }

  // Constructor
  template < typename T >
  INLINE LinkedList< T >::LinkedList() {
    GUM_CONSTRUCTOR(LinkedList);
    firstLink__ = nullptr;
  }

  // Destructor
  template < typename T >
  INLINE LinkedList< T >::~LinkedList() {
    clear();
    GUM_DESTRUCTOR(LinkedList);
  }

  template < typename T >
  INLINE void* LinkedList< T >::operator new(size_t s) {
    return SmallObjectAllocator::instance().allocate(Size(s));
  }

  template < typename T >
  INLINE void LinkedList< T >::operator delete(void* p) {
    SmallObjectAllocator::instance().deallocate(p, sizeof(LinkedList< T >));
  }

  template < typename T >
  INLINE const Link< T >* LinkedList< T >::list() const {
    return firstLink__;
  }

  template < typename T >
  INLINE Link< T >* LinkedList< T >::list() {
    return firstLink__;
  }

  template < typename T >
  void LinkedList< T >::clear() {
    Link< T >* curLink = firstLink__;
    Link< T >* nl      = nullptr;
    while (curLink) {
      nl = curLink->nextLink();
      delete curLink;
      curLink = nl;
    }
  }

  template < typename T >
  INLINE void LinkedList< T >::addLink(const T& elem) {
    Link< T >* newLink = new Link< T >(elem, firstLink__);
    firstLink__        = newLink;
  }

  template < typename T >
  INLINE void LinkedList< T >::searchAndRemoveLink(const T& elem) {
    Link< T >* curLink  = firstLink__;
    Link< T >* prevLink = nullptr;
    while (curLink && curLink->element() != elem) {
      prevLink = curLink;
      curLink  = curLink->nextLink();
    }
    if (curLink) {
      if (prevLink)
        prevLink->setNextLink(curLink->nextLink());
      else
        firstLink__ = curLink->nextLink();
      delete curLink;
    }
  }

}   // namespace gum
