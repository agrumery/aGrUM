/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */

#include <agrum/tools/multidim/utils/FunctionGraphUtilities/link.h>

namespace gum {

  // Constructor
  template < typename T >
  INLINE Link< T >::Link(const T& elem) : _element_(elem) {
    GUM_CONSTRUCTOR(Link);
  }

  // Constructor
  template < typename T >
  INLINE Link< T >::Link(const T& elem, Link< T >* nextLink) :
      _element_(elem), _nextLink_(nextLink) {
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
    return _element_;
  }

  template < typename T >
  INLINE T& Link< T >::element() {
    return _element_;
  }

  template < typename T >
  INLINE const Link< T >* Link< T >::nextLink() const {
    return _nextLink_;
  }

  template < typename T >
  INLINE Link< T >* Link< T >::nextLink() {
    return _nextLink_;
  }

  template < typename T >
  INLINE void Link< T >::setNextLink(Link< T >* newLink) {
    _nextLink_ = newLink;
  }

  // Constructor
  template < typename T >
  INLINE LinkedList< T >::LinkedList() {
    GUM_CONSTRUCTOR(LinkedList);
    _firstLink_ = nullptr;
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
    return _firstLink_;
  }

  template < typename T >
  INLINE Link< T >* LinkedList< T >::list() {
    return _firstLink_;
  }

  template < typename T >
  void LinkedList< T >::clear() {
    Link< T >* curLink = _firstLink_;
    Link< T >* nl      = nullptr;
    while (curLink) {
      nl = curLink->nextLink();
      delete curLink;
      curLink = nl;
    }
  }

  template < typename T >
  INLINE void LinkedList< T >::addLink(const T& elem) {
    Link< T >* newLink = new Link< T >(elem, _firstLink_);
    _firstLink_        = newLink;
  }

  template < typename T >
  INLINE void LinkedList< T >::searchAndRemoveLink(const T& elem) {
    Link< T >* curLink  = _firstLink_;
    Link< T >* prevLink = nullptr;
    while (curLink && curLink->element() != elem) {
      prevLink = curLink;
      curLink  = curLink->nextLink();
    }
    if (curLink) {
      if (prevLink) prevLink->setNextLink(curLink->nextLink());
      else _firstLink_ = curLink->nextLink();
      delete curLink;
    }
  }

}   // namespace gum
