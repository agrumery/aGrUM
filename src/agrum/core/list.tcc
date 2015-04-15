/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
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
 * @brief template implementation of chained lists
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <sstream>

// to ease parser
#include <agrum/core/list.h>

namespace gum {

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                         BUCKET IMPLEMENTATION                       === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// default constructor
  template <typename Val>
  INLINE ListBucket<Val>::ListBucket(const Val &v)
      : __val{v} {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListBucket);
  }

  /// construtor for Val rvalues
  template <typename Val>
  INLINE ListBucket<Val>::ListBucket(Val &&v) noexcept : __val{std::move(v)} {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListBucket);
  }

  /// emplace constructor
  template <typename Val>
  template <typename... Args>
  INLINE ListBucket<Val>::ListBucket(ListBucket<Val>::Emplace, Args &&... args)
      : __val(std::forward<Args>(args)...) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListBucket);
  }

  /// copy constructor
  template <typename Val>
  INLINE ListBucket<Val>::ListBucket(const ListBucket<Val> &src)
      : __val{src.__val} {
    // for debugging purposes
    GUM_CONS_CPY(ListBucket);
  }

  /// copy operator
  template <typename Val>
  INLINE ListBucket<Val> &ListBucket<Val>::operator=(const ListBucket<Val> &src) {
    // for debugging purposes
    GUM_OP_CPY(ListBucket);

    // no need to avoid self assignment
    __val = src.__val;
    return *this;
  }

  // WARNING: during its deletion, the bucket does not take care of properly
  // rechaining the chained list. This should be done by the Lists themselves
  template <typename Val> INLINE ListBucket<Val>::~ListBucket() {
    // for debugging purposes
    GUM_DESTRUCTOR(ListBucket);
  }

  // equality check
  template <typename Val>
  INLINE bool ListBucket<Val>::operator==(const ListBucket<Val> &src) const {
    return (src.__val == __val);
  }

  // inequality check
  template <typename Val>
  INLINE bool ListBucket<Val>::operator!=(const ListBucket<Val> &src) const {
    return (src.__val != __val);
  }

  // dereferencing operator
  template <typename Val>
  INLINE const Val &ListBucket<Val>::operator*() const noexcept {
    return __val;
  }

  // dereferencing operator
  template <typename Val> INLINE Val &ListBucket<Val>::operator*() noexcept {
    return __val;
  }

  /// returns the bucket toward the next element
  template <typename Val>
  INLINE const ListBucket<Val> *ListBucket<Val>::next() const noexcept {
    return __next;
  }

  /// returns the bucket toward the preceding element
  template <typename Val>
  INLINE const ListBucket<Val> *ListBucket<Val>::previous() const noexcept {
    return __prev;
  }

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===               UNSAFE_CONST_LIST_ITERATOR IMPLEMENTATION             === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// default constructor
  template <typename Val>
  INLINE ListConstIterator<Val>::ListConstIterator() noexcept {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListConstIterator);
  }

  /// default constructor
  template <typename Val>
  template <typename Alloc>
  INLINE
  ListConstIterator<Val>::ListConstIterator(const List<Val, Alloc> &theList) noexcept
      : __bucket{theList.__deb_list} {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListConstIterator);
  }

  /// copy constructor
  template <typename Val>
  INLINE ListConstIterator<Val>::ListConstIterator(
      const ListConstIterator<Val> &src) noexcept : __bucket{src.__bucket} {
    // for debugging purposes
    GUM_CONS_CPY(ListConstIterator);
  }

  /// move constructor
  template <typename Val>
  INLINE
  ListConstIterator<Val>::ListConstIterator(ListConstIterator<Val> &&src) noexcept
      : __bucket{std::move(src.__bucket)} {
    // for debugging purposes
    GUM_CONS_MOV(ListConstIterator);
  }

  /// Constructor for an iterator pointing to the \e ind_eltth element of a List.
  template <typename Val>
  INLINE ListConstIterator<Val>::ListConstIterator(const List<Val> &theList,
                                                   unsigned int ind_elt) {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListConstIterator);

    // check if the index ind_elt passed as parameter is valid
    if (ind_elt >= theList.__nb_elements) {
      GUM_ERROR(UndefinedIteratorValue, "Not enough elements in the list");
    }

    // check if it is faster to find the indexth element from the start or
    // from the end of the list
    if (ind_elt < (theList.__nb_elements >> 1)) {
      // find the element we shall point to src the start of the list
      for (__bucket = theList.__deb_list; ind_elt;
           --ind_elt, __bucket = __bucket->__next) {
      }
    } else {
      // find the element we shall point to src the end of the list
      for (__bucket = theList.__end_list,
          ind_elt = theList.__nb_elements - ind_elt - 1;
           ind_elt; --ind_elt, __bucket = __bucket->__prev) {
      }
    }
  }

  /// Destructor
  template <typename Val>
  INLINE ListConstIterator<Val>::~ListConstIterator() noexcept {
    // for debugging purposes
    GUM_DESTRUCTOR(ListConstIterator);
  }

  /// Copy operator
  template <typename Val>
  INLINE ListConstIterator<Val> &ListConstIterator<Val>::
  operator=(const ListConstIterator<Val> &src) noexcept {
    // for debugging purposes
    GUM_OP_CPY(ListConstIterator);

    __bucket = src.__bucket;
    return *this;
  }

  /// move operator
  template <typename Val>
  INLINE ListConstIterator<Val> &ListConstIterator<Val>::
  operator=(ListConstIterator<Val> &&src) noexcept {
    // for debugging purposes
    GUM_OP_MOV(ListConstIterator);
    __bucket = src.__bucket;
    return *this;
  }

  /// returns the bucket the iterator is pointing to
  template <typename Val>
  INLINE ListBucket<Val> *ListConstIterator<Val>::__getBucket() const noexcept {
    return __bucket;
  }

  /// Makes the iterator point toward nothing
  template <typename Val> INLINE void ListConstIterator<Val>::clear() noexcept {
    __bucket = nullptr;
  }

  /// positions the iterator to the end of the list
  template <typename Val> INLINE void ListConstIterator<Val>::setToEnd() noexcept {
    __bucket = nullptr;
  }

  /// returns a bool indicating whether the iterator points to the end of the list
  template <typename Val>
  INLINE bool ListConstIterator<Val>::isEnd() const noexcept {
    return (__bucket == nullptr);
  }

  /// makes the iterator point to the next element in the List
  template <typename Val>
  INLINE ListConstIterator<Val> &ListConstIterator<Val>::operator++() noexcept {
    // if we are pointing to an element of the chained list, just
    // point on the next bucket in this list
    if (__bucket != nullptr) {
      __bucket = __bucket->__next;
    }

    return *this;
  }

  /// makes the iterator point to the next element in the List
  template <typename Val>
  INLINE ListConstIterator<Val> &ListConstIterator<Val>::
  operator+=(typename ListConstIterator<Val>::difference_type i) noexcept {
    if (i >= 0) {
      for (; i && (__bucket != nullptr); --i, __bucket = __bucket->__next) {
      }
    } else {
      for (; i && (__bucket != nullptr); ++i, __bucket = __bucket->__prev) {
      }
    }
    return *this;
  }

  /// makes the iterator point to the preceding element in the List
  template <typename Val>
  INLINE ListConstIterator<Val> &ListConstIterator<Val>::operator--() noexcept {
    // if we are pointing to an element of the chained list, just
    // point on the preceding bucket in this list
    if (__bucket != nullptr) {
      __bucket = __bucket->__prev;
    }

    return *this;
  }

  /// makes the iterator point to i elements before in the list
  template <typename Val>
  INLINE ListConstIterator<Val> &ListConstIterator<Val>::
  operator-=(typename ListConstIterator<Val>::difference_type i) noexcept {
    if (i >= 0) {
      for (; i && (__bucket != nullptr); --i, __bucket = __bucket->__prev) {
      }
    } else {
      for (; i && (__bucket != nullptr); ++i, __bucket = __bucket->__next) {
      }
    }
    return *this;
  }

  /// returns a new iterator
  template <typename Val>
  INLINE ListConstIterator<Val> ListConstIterator<Val>::
  operator+(typename ListConstIterator<Val>::difference_type i) noexcept {
    return ListConstIterator<Val>(*this) += i;
  }

  /// returns a new iterator
  template <typename Val>
  INLINE ListConstIterator<Val> ListConstIterator<Val>::
  operator-(typename ListConstIterator<Val>::difference_type i) noexcept {
    return ListConstIterator<Val>(*this) -= i;
  }

  /// checks whether two iterators point toward different elements
  template <typename Val>
  INLINE bool ListConstIterator<Val>::
  operator!=(const ListConstIterator<Val> &src) const noexcept {
    return (__bucket != src.__bucket);
  }

  /// checks whether two iterators point toward the same elements.
  template <typename Val>
  INLINE bool ListConstIterator<Val>::
  operator==(const ListConstIterator<Val> &src) const noexcept {
    return (__bucket == src.__bucket);
  }

  /// dereferences the value pointed to by the iterator
  template <typename Val>
  INLINE const Val *ListConstIterator<Val>::operator->() const {
    if (__bucket != nullptr)
      return &(__bucket->__val);
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a NULL object");
    }
  }

  /// gives access to the content of the iterator
  template <typename Val>
  INLINE const Val &ListConstIterator<Val>::operator*() const {
    if (__bucket != nullptr)
      return __bucket->__val;
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a NULL object");
    }
  }

  /// for STL compliance, a distance operator
  template <typename Val>
  INLINE typename ListConstIterator<Val>::difference_type
  operator-(const ListConstIterator<Val> &iter1,
            const ListConstIterator<Val> &iter2) {
    typename ListConstIterator<Val>::difference_type res = 0;

    for (ListConstIterator<Val> iter3 = iter2; iter1 != iter3; ++iter3, ++res) {
    }

    return res;
  }

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                  UNSAFE_LIST_ITERATOR IMPLEMENTATION                === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// basic constructor
  template <typename Val>
  INLINE ListIterator<Val>::ListIterator() noexcept : ListConstIterator<Val>{} {
    GUM_CONSTRUCTOR(ListIterator);
  }

  /// constructor for a begin
  template <typename Val>
  template <typename Alloc>
  INLINE ListIterator<Val>::ListIterator(const List<Val, Alloc> &theList) noexcept
      : ListConstIterator<Val>{theList} {
    GUM_CONSTRUCTOR(ListIterator);
  }

  /// copy constructor
  template <typename Val>
  INLINE ListIterator<Val>::ListIterator(const ListIterator<Val> &src) noexcept
      : ListConstIterator<Val>{src} {
    GUM_CONS_CPY(ListIterator);
  }

  /// move constructor
  template <typename Val>
  INLINE ListIterator<Val>::ListIterator(ListIterator<Val> &&src) noexcept
      : ListConstIterator<Val>(std::move(src)) {
    GUM_CONS_MOV(ListIterator);
  }

  /// Constructor for an iterator pointing to the \e ind_eltth element of a List.
  template <typename Val>
  INLINE ListIterator<Val>::ListIterator(const List<Val> &theList,
                                         unsigned int ind_elt)
      : ListConstIterator<Val>{theList, ind_elt} {
    GUM_CONSTRUCTOR(ListIterator);
  }

  /// Copy operator
  template <typename Val>
  INLINE ListIterator<Val> &ListIterator<Val>::
  operator=(const ListIterator<Val> &src) noexcept {
    GUM_OP_CPY(ListIterator);
    ListConstIterator<Val>::operator=(src);
    return *this;
  }

  /// move operator
  template <typename Val>
  INLINE ListIterator<Val> &ListIterator<Val>::
  operator=(ListIterator<Val> &&src) noexcept {
    GUM_OP_MOV(ListIterator);
    ListConstIterator<Val>::operator=(std::move(src));
    return *this;
  }

  /// Destructor
  template <typename Val> INLINE ListIterator<Val>::~ListIterator() noexcept {
    GUM_DESTRUCTOR(ListIterator);
  }

  /// makes the iterator point to the next element in the List
  template <typename Val>
  INLINE ListIterator<Val> &ListIterator<Val>::operator++() noexcept {
    ListConstIterator<Val>::operator++();
    return *this;
  }

  /// makes the iterator point to i elements further in the List
  template <typename Val>
  INLINE ListIterator<Val> &ListIterator<Val>::
  operator+=(typename ListIterator<Val>::difference_type i) noexcept {
    ListConstIterator<Val>::operator+=(i);
    return *this;
  }

  /// makes the iterator point to the preceding element in the List
  template <typename Val>
  INLINE ListIterator<Val> &ListIterator<Val>::operator--() noexcept {
    ListConstIterator<Val>::operator--();
    return *this;
  }

  /// makes the iterator point to i elements before in the List
  template <typename Val>
  INLINE ListIterator<Val> &ListIterator<Val>::
  operator-=(typename ListIterator<Val>::difference_type i) noexcept {
    ListConstIterator<Val>::operator-=(i);
    return *this;
  }

  /// returns a new iterator
  template <typename Val>
  INLINE ListIterator<Val> ListIterator<Val>::
  operator+(typename ListIterator<Val>::difference_type i) noexcept {
    return ListIterator<Val>(*this) += i;
  }

  /// returns a new iterator
  template <typename Val>
  INLINE ListIterator<Val> ListIterator<Val>::
  operator-(typename ListIterator<Val>::difference_type i) noexcept {
    return ListIterator<Val>(*this) -= i;
  }

  /// dereferences the value pointed to by the iterator
  template <typename Val> INLINE Val *ListIterator<Val>::operator->() {
    return const_cast<Val *>(ListConstIterator<Val>::operator->());
  }

  /// gives access to the content of the iterator
  template <typename Val> INLINE Val &ListIterator<Val>::operator*() {
    return const_cast<Val &>(ListConstIterator<Val>::operator*());
  }

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===               SAFE LIST CONST ITERATOR IMPLEMENTATION               === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// basic constructor
  template <typename Val>
  INLINE ListConstIteratorSafe<Val>::ListConstIteratorSafe() noexcept {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListConstIteratorSafe);
  }

  /// Constructor for a begin
  template <typename Val>
  template <typename Alloc>
  INLINE
  ListConstIteratorSafe<Val>::ListConstIteratorSafe(const List<Val, Alloc> &theList)
      : __list{reinterpret_cast<const List<Val, std::allocator<Val>> *>(&theList)},
        __bucket{theList.__deb_list} {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListConstIteratorSafe);

    // add the iterator to the list of safe iterators
    theList.__safe_iterators.push_back(this);
  }

  /// copy constructor
  template <typename Val>
  INLINE ListConstIteratorSafe<Val>::ListConstIteratorSafe(
      const ListConstIteratorSafe<Val> &src)
      : __list{src.__list}, __bucket{src.__bucket},
        __next_current_bucket{src.__next_current_bucket},
        __prev_current_bucket{src.__prev_current_bucket},
        __null_pointing{src.__null_pointing} {
    // for debugging purposes
    GUM_CONS_CPY(ListConstIteratorSafe);

    // add the iterator to the list of safe iterators
    if (__list != nullptr)
      __list->__safe_iterators.push_back(this);
  }

  /// Constructor for an iterator pointing to the \e ind_eltth element of a List.
  template <typename Val>
  template <typename Alloc>
  ListConstIteratorSafe<Val>::ListConstIteratorSafe(const List<Val, Alloc> &theList,
                                                    unsigned int ind_elt)
      : __list{reinterpret_cast<const List<Val, std::allocator<Val>> *>(&theList)} {
    // for debugging purposes
    GUM_CONSTRUCTOR(ListConstIteratorSafe);

    // check if the index ind_elt passed as parameter is valid
    if (ind_elt >= __list->__nb_elements) {
      GUM_ERROR(UndefinedIteratorValue, "Not enough elements in the list");
    }

    // check if it is faster to find the indexth element src the start or
    // src the end of the list
    if (ind_elt < (__list->__nb_elements >> 1)) {
      // find the element we shall point to src the start of the list
      for (__bucket = __list->__deb_list; ind_elt;
           --ind_elt, __bucket = __bucket->__next) {
      }
    } else {
      // find the element we shall point to src the end of the list
      for (__bucket = __list->__end_list,
          ind_elt = __list->__nb_elements - ind_elt - 1;
           ind_elt; --ind_elt, __bucket = __bucket->__prev) {
      }
    }

    // add the iterator to the list of safe iterators
    theList.__safe_iterators.push_back(this);
  }

  /// move constructor
  template <typename Val>
  INLINE
  ListConstIteratorSafe<Val>::ListConstIteratorSafe(ListConstIteratorSafe<Val> &&src)
      : __list{src.__list}, __bucket{src.__bucket},
        __next_current_bucket{src.__next_current_bucket},
        __prev_current_bucket{src.__prev_current_bucket},
        __null_pointing{src.__null_pointing} {
    // for debugging purposes
    GUM_CONS_MOV(ListConstIteratorSafe);

    if (__list != nullptr) {
      // substitute src by this in the list of safe iterators
      std::vector<ListConstIteratorSafe<Val> *> &vect = __list->__safe_iterators;

      for (auto ptr = vect.rbegin(); ptr != vect.rend(); --ptr) {
        if (*ptr == &src) {
          *ptr = this;
          break;
        }
      }

      src.__list = nullptr;
      src.__bucket = nullptr;
      src.__null_pointing = false;
    }
  }

  /// remove the iterator for its list' safe iterators list
  template <typename Val>
  INLINE void ListConstIteratorSafe<Val>::__removeFromSafeList() const {
    // find where the iterator is
    std::vector<ListConstIteratorSafe<Val> *> &vect = __list->__safe_iterators;

    for (int i = vect.size() - 1; i >= 0; --i) {
      if (vect[i] == this) {
        vect.erase(vect.begin() + i);
        break;
      }
    }
  }

  /// Copy operator
  template <typename Val>
  ListConstIteratorSafe<Val> &ListConstIteratorSafe<Val>::
  operator=(const ListConstIteratorSafe<Val> &src) {
    // avoid self assignment
    if (this != &src) {
      // for debugging purposes
      GUM_OP_CPY(ListConstIteratorSafe);

      // check if src and this belong to the same list. If this is not
      // the case, we shall remove this from its iterator's list and
      // put it into src's list one.
      if (__list && (src.__list != __list)) {
        __removeFromSafeList();
        __list = nullptr;
      }

      // if necessary, put this into the same list of safe iterators as src
      if (src.__list && (src.__list != __list)) {
        try {
          src.__list->__safe_iterators.push_back(this);
        } catch (...) {
          __list = nullptr;
          __bucket = nullptr;
          __null_pointing = false;
          throw;
        }
      }

      __list = src.__list;
      __bucket = src.__bucket;
      __prev_current_bucket = src.__prev_current_bucket;
      __next_current_bucket = src.__next_current_bucket;
      __null_pointing = src.__null_pointing;
    }

    return *this;
  }

  /// move operator
  template <typename Val>
  ListConstIteratorSafe<Val> &ListConstIteratorSafe<Val>::
  operator=(ListConstIteratorSafe<Val> &&src) {
    // avoid self assignment
    if (this != &src) {
      // for debugging purposes
      GUM_OP_MOV(ListConstIteratorSafe);

      // if the two iterators do not point to the same list, remove
      // the current iterator from its safe iterators list
      if ((__list != nullptr) && (src.__list != __list)) {
        __removeFromSafeList();
        __list = nullptr;
      }

      // now if src points to a list, put this at its location
      if ((src.__list != nullptr)) {
        std::vector<ListConstIteratorSafe<Val> *> &vect =
            src.__list->__safe_iterators;
        int index_src = vect.size() - 1;

        for (;; --index_src) {
          if (vect[index_src] == &src) {
            break;
          }
        }

        if (__list == nullptr) {
          vect[index_src] = this;
        } else {
          vect.erase(vect.begin() + index_src);
        }
      }

      __list = src.__list;
      __bucket = src.__bucket;
      __prev_current_bucket = src.__prev_current_bucket;
      __next_current_bucket = src.__next_current_bucket;
      __null_pointing = src.__null_pointing;

      src.__list = nullptr;
      src.__bucket = nullptr;
      src.__null_pointing = false;
    }

    return *this;
  }

  /// Destructor
  template <typename Val>
  INLINE ListConstIteratorSafe<Val>::~ListConstIteratorSafe() {
    // for debugging purposes
    GUM_DESTRUCTOR(ListConstIteratorSafe);

    // remove the iterator src the table's iterator list
    if (__list)
      __removeFromSafeList();
  }

  /// returns the bucket the iterator is pointing to
  template <typename Val>
  INLINE ListBucket<Val> *ListConstIteratorSafe<Val>::__getBucket() const noexcept {
    return __bucket;
  }

  /// Makes the iterator point toward nothing
  template <typename Val> INLINE void ListConstIteratorSafe<Val>::clear() {
    // remove the iterator src the list's iterator list
    if (__list)
      __removeFromSafeList();

    // set its list as well as the element it points to to nullptr
    __list = nullptr;
    __bucket = nullptr;
    __null_pointing = false;
  }

  /// positions the iterator to the end of the list
  template <typename Val> INLINE void ListConstIteratorSafe<Val>::setToEnd() {
    clear();
  }

  /// returns a bool indicating whether the iterator points to the end of the list
  template <typename Val> INLINE bool ListConstIteratorSafe<Val>::isEnd() const {
    return __null_pointing
               ? (__next_current_bucket == nullptr) &&
                     (__prev_current_bucket == nullptr)
               : (__bucket == nullptr);
  }

  /// makes the iterator point to the next element in the List
  template <typename Val>
  INLINE ListConstIteratorSafe<Val> &ListConstIteratorSafe<Val>::
  operator++() noexcept {
    // check if we are pointing to something that has been deleted
    if (__null_pointing) {
      __null_pointing = false;

      // if we are pointing to an element of the chained list that has been deleted
      // but that has a next element, just point on the latter
      if (__next_current_bucket != nullptr) {
        __bucket = __next_current_bucket->__next;
        return *this;
      }

      // here we were pointing on an extremity of the list (either end or rend)
      // if prev_current_bucket is not null, then we are at rend and doing
      // a ++ shall now point to the beginning of the list
      if (__prev_current_bucket != nullptr) {
        __bucket = __prev_current_bucket;
        return *this;
      }

      // here, we are at the end of the chained list, hence we shall remain at end
      __bucket = nullptr;
      return *this;
    } else {
      // if we are pointing to an element of the chained list, just
      // point on the next bucket in this list
      if (__bucket != nullptr) {
        __bucket = __bucket->__next;
      }

      return *this;
    }
  }

  /// makes the iterator point to i elements before in the List
  template <typename Val>
  INLINE ListConstIteratorSafe<Val> &
  ListConstIteratorSafe<Val>::__opMinus(unsigned int i) noexcept {
    // check if we are pointing to something that has been deleted
    if (__null_pointing) {
      __null_pointing = false;

      // if we are pointing to an element of the chained list that has been deleted
      // but that has a preceding element, just point on the latter
      if (__prev_current_bucket != nullptr) {
        __bucket = __prev_current_bucket->__prev;
      } else {
        // here we were pointing on an extremity of the list (either end or rend)
        // if next_current_bucket is not null, then we are at end and doing
        // a -- shall now point to the beginning of the list
        if (__next_current_bucket != nullptr) {
          __bucket = __next_current_bucket;
        } else {
          // here, we are at the rend of the chained list, hence we shall remain
          // at rend
          __bucket = nullptr;
          return *this;
        }
      }
    } else {
      // if we are pointing to an element of the chained list, just
      // point on the preceding bucket in this list
      if (__bucket != nullptr) {
        __bucket = __bucket->__prev;
      }
    }

    for (--i; i && (__bucket != nullptr); --i, __bucket = __bucket->__prev) {
    }

    return *this;
  }

  /// makes the iterator point to the next element in the List
  template <typename Val>
  INLINE ListConstIteratorSafe<Val> &
  ListConstIteratorSafe<Val>::__opPlus(unsigned int i) noexcept {
    // check if we are pointing to something that has been deleted
    if (__null_pointing) {
      __null_pointing = false;

      // if we are pointing to an element of the chained list that has been deleted
      // but that has a next element, just point on the latter
      if (__next_current_bucket != nullptr) {
        __bucket = __next_current_bucket->__next;
      } else {
        // here we were pointing on an extremity of the list (either end or rend)
        // if prev_current_bucket is not null, then we are at rend and doing
        // a ++ shall now point to the beginning of the list
        if (__prev_current_bucket != nullptr) {
          __bucket = __prev_current_bucket;
        } else {
          // here, we are at the end of the chained list, hence we shall
          // remain at end
          __bucket = nullptr;
          return *this;
        }
      }
    } else {
      // if we are pointing to an element of the chained list, just
      // point on the next bucket in this list
      if (__bucket != nullptr) {
        __bucket = __bucket->__next;
      }
    }

    for (--i; i && (__bucket != nullptr); --i, __bucket = __bucket->__next) {
    }

    return *this;
  }

  /// makes the iterator point to the next element in the List
  template <typename Val>
  INLINE ListConstIteratorSafe<Val> &ListConstIteratorSafe<Val>::
  operator+=(typename ListConstIteratorSafe<Val>::difference_type i) noexcept {
    if (!i)
      return *this;

    if (i < 0)
      return __opMinus(-i);
    else
      return __opPlus(i);
  }

  /// makes the iterator point to the preceding element in the List
  template <typename Val>
  INLINE ListConstIteratorSafe<Val> &ListConstIteratorSafe<Val>::
  operator--() noexcept {
    // check if we are pointing to something that has been deleted
    if (__null_pointing) {
      __null_pointing = false;

      // if we are pointing to an element of the chained list that has been deleted
      // but that has a preceding element, just point on the latter
      if (__prev_current_bucket != nullptr) {
        __bucket = __prev_current_bucket->__prev;
        return *this;
      }

      // here we were pointing on an extremity of the list (either end or rend)
      // if next_current_bucket is not null, then we are at end and doing
      // a -- shall now point to the beginning of the list
      if (__next_current_bucket != nullptr) {
        __bucket = __next_current_bucket;
        return *this;
      }

      // here, we are at the rend of the chained list, hence we shall remain
      // at rend
      __bucket = nullptr;
      return *this;
    } else {
      // if we are pointing to an element of the chained list, just
      // point on the preceding bucket in this list
      if (__bucket != nullptr) {
        __bucket = __bucket->__prev;
      }

      return *this;
    }
  }

  /// makes the iterator point to i elements before in the List
  template <typename Val>
  INLINE ListConstIteratorSafe<Val> &ListConstIteratorSafe<Val>::
  operator-=(typename ListConstIteratorSafe<Val>::difference_type i) noexcept {
    if (!i)
      return *this;

    if (i < 0)
      return __opPlus(-i);
    else
      return __opMinus(i);
  }

  /// returns a new iterator
  template <typename Val>
  INLINE ListConstIteratorSafe<Val> ListConstIteratorSafe<Val>::
  operator+(typename ListConstIteratorSafe<Val>::difference_type i) noexcept {
    return ListConstIteratorSafe<Val>(*this) += i;
  }

  /// returns a new iterator
  template <typename Val>
  INLINE ListConstIteratorSafe<Val> ListConstIteratorSafe<Val>::
  operator-(typename ListConstIteratorSafe<Val>::difference_type i) noexcept {
    return ListConstIteratorSafe<Val>(*this) -= i;
  }

  /// checks whether two iterators point toward different elements
  template <typename Val>
  INLINE bool ListConstIteratorSafe<Val>::
  operator!=(const ListConstIteratorSafe<Val> &src) const {
    return __null_pointing
               ? (__next_current_bucket != src.__next_current_bucket) ||
                     (__prev_current_bucket != src.__prev_current_bucket)
               : (__bucket != src.__bucket);
  }

  /// checks whether two iterators point toward the same elements.
  template <typename Val>
  INLINE bool ListConstIteratorSafe<Val>::
  operator==(const ListConstIteratorSafe<Val> &src) const {
    return __null_pointing
               ? (__next_current_bucket == src.__next_current_bucket) &&
                     (__prev_current_bucket == src.__prev_current_bucket)
               : (__bucket == src.__bucket);
  }

  /// dereferences the value pointed to by the iterator
  template <typename Val>
  INLINE const Val *ListConstIteratorSafe<Val>::operator->() const {
    if (__bucket != nullptr)
      return &(__bucket->__val);
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a NULL object");
    }
  }

  /// gives access to the content of the iterator
  template <typename Val>
  INLINE const Val &ListConstIteratorSafe<Val>::operator*() const {
    if (__bucket != nullptr)
      return __bucket->__val;
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a NULL object");
    }
  }

  /// for STL compliance, a distance operator
  template <typename Val>
  INLINE typename ListConstIteratorSafe<Val>::difference_type
  operator-(const ListConstIteratorSafe<Val> &iter1,
            const ListConstIteratorSafe<Val> &iter2) {
    typename ListConstIteratorSafe<Val>::difference_type res = 0;
    ListConstIteratorSafe<Val> iter3{iter2};

    for (; iter1 != iter3; ++iter3, ++res) {
    }

    return res;
  }

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                     LIST ITERATOR IMPLEMENTATION                    === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// basic constructor
  template <typename Val>
  INLINE ListIteratorSafe<Val>::ListIteratorSafe() noexcept
      : ListConstIteratorSafe<Val>{} {
    GUM_CONSTRUCTOR(ListIteratorSafe);
  }

  /// constructor for a begin
  template <typename Val>
  template <typename Alloc>
  INLINE ListIteratorSafe<Val>::ListIteratorSafe(const List<Val, Alloc> &theList)
      : ListConstIteratorSafe<Val>{theList} {
    GUM_CONSTRUCTOR(ListIteratorSafe);
  }

  /// copy constructor
  template <typename Val>
  INLINE ListIteratorSafe<Val>::ListIteratorSafe(const ListIteratorSafe<Val> &src)
      : ListConstIteratorSafe<Val>{src} {
    GUM_CONS_CPY(ListIteratorSafe);
  }

  /// Constructor for an iterator pointing to the \e ind_eltth element of a List.
  template <typename Val>
  template <typename Alloc>
  INLINE ListIteratorSafe<Val>::ListIteratorSafe(const List<Val, Alloc> &theList,
                                                 unsigned int ind_elt)
      : ListConstIteratorSafe<Val>{theList, ind_elt} {
    GUM_CONSTRUCTOR(ListIteratorSafe);
  }

  /// move constructor
  template <typename Val>
  INLINE ListIteratorSafe<Val>::ListIteratorSafe(ListIteratorSafe<Val> &&src)
      : ListConstIteratorSafe<Val>{std::move(src)} {
    GUM_CONS_MOV(ListIteratorSafe);
  }

  /// Copy operator
  template <typename Val>
  INLINE ListIteratorSafe<Val> &ListIteratorSafe<Val>::
  operator=(const ListIteratorSafe<Val> &src) {
    // for debugging purposes
    GUM_OP_CPY(ListIteratorSafe);
    ListConstIteratorSafe<Val>::operator=(src);
    return *this;
  }

  /// move operator
  template <typename Val>
  INLINE ListIteratorSafe<Val> &ListIteratorSafe<Val>::
  operator=(ListIteratorSafe<Val> &&src) {
    // for debugging purposes
    GUM_OP_MOV(ListIteratorSafe);
    ListConstIteratorSafe<Val>::operator=(std::move(src));
    return *this;
  }

  /// Destructor
  template <typename Val> INLINE ListIteratorSafe<Val>::~ListIteratorSafe() {
    GUM_DESTRUCTOR(ListIteratorSafe);
  }

  /// makes the iterator point to the next element in the List
  template <typename Val>
  INLINE ListIteratorSafe<Val> &ListIteratorSafe<Val>::operator++() noexcept {
    ListConstIteratorSafe<Val>::operator++();
    return *this;
  }

  /// makes the iterator point to the next element in the List
  template <typename Val>
  INLINE ListIteratorSafe<Val> &ListIteratorSafe<Val>::
  operator+=(typename ListIteratorSafe<Val>::difference_type i) noexcept {
    ListConstIteratorSafe<Val>::operator+=(i);
    return *this;
  }

  /// makes the iterator point to the preceding element in the List
  template <typename Val>
  INLINE ListIteratorSafe<Val> &ListIteratorSafe<Val>::operator--() noexcept {
    ListConstIteratorSafe<Val>::operator--();
    return *this;
  }

  /// makes the iterator point to the preceding element in the List
  template <typename Val>
  INLINE ListIteratorSafe<Val> &ListIteratorSafe<Val>::
  operator-=(typename ListIteratorSafe<Val>::difference_type i) noexcept {
    ListConstIteratorSafe<Val>::operator-=(i);
    return *this;
  }

  /// returns a new iterator
  template <typename Val>
  INLINE ListIteratorSafe<Val> ListIteratorSafe<Val>::
  operator+(typename ListIteratorSafe<Val>::difference_type i) noexcept {
    return ListIteratorSafe<Val>(*this) += i;
  }

  /// returns a new iterator
  template <typename Val>
  INLINE ListIteratorSafe<Val> ListIteratorSafe<Val>::
  operator-(typename ListIteratorSafe<Val>::difference_type i) noexcept {
    return ListIteratorSafe<Val>(*this) -= i;
  }

  /// dereferences the value pointed to by the iterator
  template <typename Val> INLINE Val *ListIteratorSafe<Val>::operator->() {
    return const_cast<Val *>(ListConstIteratorSafe<Val>::operator->());
  }

  /// gives access to the content of the iterator
  template <typename Val> INLINE Val &ListIteratorSafe<Val>::operator*() {
    return const_cast<Val &>(ListConstIteratorSafe<Val>::operator*());
  }

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                          LIST IMPLEMENTATION                        === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// a function used to perform copies of elements of Lists
  template <typename Val, typename Alloc>
  template <typename OtherAlloc>
  void List<Val, Alloc>::__copy_elements(const List<Val, OtherAlloc> &src) {
    ListBucket<Val> *ptr;
    ListBucket<Val> *old_ptr = nullptr;
    ListBucket<Val> *new_elt = nullptr;

    // copy src's list
    try {
      for (ptr = src.__deb_list; ptr != nullptr; ptr = ptr->__next) {
        // create a copy bucket
        new_elt = __alloc_bucket.allocate(1);

        try {
          __alloc_bucket.construct(new_elt, *ptr);
        } catch (...) {
          __alloc_bucket.deallocate(new_elt, 1);
          throw;
        }

        // rechain properly the new list (the next field is already initialized
        // with nullptr)
        new_elt->__prev = old_ptr;

        if (old_ptr)
          old_ptr->__next = new_elt;
        else
          __deb_list = new_elt;

        old_ptr = new_elt;
      }
    } catch (...) {
      // problem: we could not allocate an element in the list => we delete
      // the elements created so far and we throw an exception
      for (; __deb_list != nullptr;
           __deb_list = const_cast<ListBucket<Val> *>(ptr)) {
        ptr = __deb_list->__next;
        __alloc_bucket.destroy(__deb_list);
        __alloc_bucket.deallocate(__deb_list, 1);
      }

      __deb_list = nullptr;
      throw;
    }

    // update properly the end of the chained list and the number of elements
    __end_list = old_ptr;
    __nb_elements = src.__nb_elements;
  }

  /// deletes all the elements of a chained list.
  template <typename Val, typename Alloc> void List<Val, Alloc>::clear() {
    // first we update all the safe iterators of the list : they should now
    // point to end/rend
    for (const auto ptr_iter : __safe_iterators) {
      ptr_iter->clear();
    }

    // clear all the values
    for (ListBucket<Val> *ptr = __deb_list, *next_ptr = nullptr; ptr != nullptr;
         ptr = next_ptr) {
      next_ptr = ptr->__next;
      __alloc_bucket.destroy(ptr);
      __alloc_bucket.deallocate(ptr, 1);
    }

    __nb_elements = 0;
    __deb_list = nullptr;
    __end_list = nullptr;
  }

  /// A basic constructor that creates an empty list
  template <typename Val, typename Alloc> INLINE List<Val, Alloc>::List() {
    // for debugging purposes
    GUM_CONSTRUCTOR(List);

    // reserve space for only the default number of iterators
    __safe_iterators.reserve(GUM_DEFAULT_ITERATOR_NUMBER);
  }

  /// Copy constructor
  template <typename Val, typename Alloc>
  INLINE List<Val, Alloc>::List(const List<Val, Alloc> &src) {
    // for debugging purposes
    GUM_CONS_CPY(List);

    // copy the elements
    __copy_elements(src);

    // reserve space for only the default number of iterators
    __safe_iterators.reserve(GUM_DEFAULT_ITERATOR_NUMBER);
  }

  /// generalized copy constructor
  template <typename Val, typename Alloc>
  template <typename OtherAlloc>
  INLINE List<Val, Alloc>::List(const List<Val, OtherAlloc> &src) {
    // for debugging purposes
    GUM_CONS_CPY(List);

    // copy the elements
    __copy_elements(src);

    // reserve space for only the default number of iterators
    __safe_iterators.reserve(GUM_DEFAULT_ITERATOR_NUMBER);
  }

  /// move constructor
  template <typename Val, typename Alloc>
  INLINE List<Val, Alloc>::List(List<Val, Alloc> &&src)
      : __deb_list{std::move(src.__deb_list)}, __end_list{std::move(src.__end_list)},
        __nb_elements{std::move(src.__nb_elements)},
        __safe_iterators{std::move(src.__safe_iterators)},
        __alloc_bucket{std::move(src.__alloc_bucket)} {
    // for debugging purposes
    GUM_CONS_MOV(List);

    src.__deb_list = nullptr;
    src.__end_list = nullptr;
    src.__nb_elements = 0;
    src.__safe_iterators.clear();
  }

  /// initializer_list constructor
  template <typename Val, typename Alloc>
  INLINE List<Val, Alloc>::List(std::initializer_list<Val> list) {
    // for debugging purposes
    GUM_CONSTRUCTOR(List);

    // adding all the elements
    for (const auto &val : list) {
      pushBack(val);
    }

    // reserve space for only the default number of iterators
    __safe_iterators.reserve(GUM_DEFAULT_ITERATOR_NUMBER);
  }

  /// Destructor
  template <typename Val, typename Alloc> INLINE List<Val, Alloc>::~List() {
    // for debugging (although this program is bug-free)
    GUM_DESTRUCTOR(List);

    // we detach all the safe iterators attached to the current List and we
    // remove all the elements from the list
    clear();
  }

  /// Copy operator. The List iterator's list is not shared with that of \e src.
  template <typename Val, typename Alloc>
  INLINE List<Val, Alloc> &List<Val, Alloc>::operator=(const List<Val, Alloc> &src) {
    // avoid self assignment
    if (this != &src) {
      // for debugging purposes
      GUM_OP_CPY(List);

      // remove the old content of 'this' and update accordingly the iterators
      clear();

      // perform the copy
      __copy_elements(src);
    }

    return *this;
  }

  /// Generalized copy operator.
  template <typename Val, typename Alloc>
  template <typename OtherAlloc>
  INLINE List<Val, Alloc> &List<Val, Alloc>::
  operator=(const List<Val, OtherAlloc> &src) {
    // avoid self assignment
    if (this != reinterpret_cast<List<Val, Alloc> *>(&src)) {
      // for debugging purposes
      GUM_OP_CPY(List);

      // remove the old content of 'this' and update accordingly the iterators
      clear();

      // perform the copy
      __copy_elements(src);
    }

    return *this;
  }

  /// move operator
  template <typename Val, typename Alloc>
  INLINE List<Val, Alloc> &List<Val, Alloc>::operator=(List<Val, Alloc> &&src) {
    // avoid self assignment
    if (this != &src) {
      // for debugging purposes
      GUM_OP_MOV(List);

      // remove the old content of 'this' and update accordingly the iterators
      clear();

      // perform the move
      __deb_list = std::move(src.__deb_list);
      __end_list = std::move(src.__end_list);
      __nb_elements = std::move(src.__nb_elements);
      __safe_iterators = std::move(src.__safe_iterators);
      __alloc_bucket = std::move(src.__alloc_bucket);

      src.__deb_list = nullptr;
      src.__end_list = nullptr;
      src.__nb_elements = 0;
      src.__safe_iterators.clear();
    }

    return *this;
  }

  /// the iterator pointing to the end of the List
  template <typename Val, typename Alloc>
  INLINE const ListConstIteratorSafe<Val> &List<Val, Alloc>::cendSafe() const
      noexcept {
    return *(reinterpret_cast<const ListConstIteratorSafe<Val> *>(__list_end_safe));
  }

  /// the iterator pointing to the end of the List
  template <typename Val, typename Alloc>
  INLINE const ListIteratorSafe<Val> &List<Val, Alloc>::endSafe() noexcept {
    return *(reinterpret_cast<const ListIteratorSafe<Val> *>(__list_end_safe));
  }

  /// the iterator pointing to the end of the List
  template <typename Val, typename Alloc>
  INLINE const ListConstIterator<Val> &List<Val, Alloc>::cend() const noexcept {
    return *(reinterpret_cast<const ListConstIterator<Val> *>(__list_end));
  }

  /// the iterator pointing to the end of the List
  template <typename Val, typename Alloc>
  INLINE const ListIterator<Val> &List<Val, Alloc>::end() noexcept {
    return *(reinterpret_cast<const ListIterator<Val> *>(__list_end));
  }

  /// the iterator pointing to the end of the List
  template <typename Val, typename Alloc>
  INLINE const ListConstIterator<Val> &List<Val, Alloc>::end() const noexcept {
    return *(reinterpret_cast<const ListConstIterator<Val> *>(__list_end));
  }

  /// the iterator pointing to the rend (just before the beginning) of the List
  template <typename Val, typename Alloc>
  INLINE const ListConstIteratorSafe<Val> &List<Val, Alloc>::crendSafe() const
      noexcept {
    return *(reinterpret_cast<const ListConstIteratorSafe<Val> *>(__list_end_safe));
  }

  /// the iterator pointing to the rend (just before the beginning) of the List
  template <typename Val, typename Alloc>
  INLINE const ListIteratorSafe<Val> &List<Val, Alloc>::rendSafe() noexcept {
    return *(reinterpret_cast<const ListIteratorSafe<Val> *>(__list_end_safe));
  }

  /// the iterator pointing to the rend (just before the beginning) of the List
  template <typename Val, typename Alloc>
  INLINE const ListConstIterator<Val> &List<Val, Alloc>::crend() const noexcept {
    return *(reinterpret_cast<const ListConstIterator<Val> *>(__list_end));
  }

  /// the iterator pointing to the rend (just before the beginning) of the List
  template <typename Val, typename Alloc>
  INLINE const ListIterator<Val> &List<Val, Alloc>::rend() noexcept {
    return *(reinterpret_cast<const ListIterator<Val> *>(__list_end));
  }

  /// the iterator pointing to the rend (just before the beginning) of the List
  template <typename Val, typename Alloc>
  INLINE const ListConstIterator<Val> &List<Val, Alloc>::rend() const noexcept {
    return *(reinterpret_cast<const ListConstIterator<Val> *>(__list_end));
  }

  /// the iterator pointing to the beginning of the List
  template <typename Val, typename Alloc>
  INLINE ListConstIteratorSafe<Val> List<Val, Alloc>::cbeginSafe() const {
    return ListConstIteratorSafe<Val>{*this};
  }

  /// the iterator pointing to the beginning of the List
  template <typename Val, typename Alloc>
  INLINE ListIteratorSafe<Val> List<Val, Alloc>::beginSafe() {
    return ListIteratorSafe<Val>{*this};
  }

  /// the iterator pointing to the beginning of the List
  template <typename Val, typename Alloc>
  INLINE ListConstIterator<Val> List<Val, Alloc>::cbegin() const {
    return ListConstIterator<Val>{*this};
  }

  /// the iterator pointing to the beginning of the List
  template <typename Val, typename Alloc>
  INLINE ListIterator<Val> List<Val, Alloc>::begin() {
    return ListIterator<Val>{*this};
  }

  /// the iterator pointing to the beginning of the List
  template <typename Val, typename Alloc>
  INLINE ListConstIterator<Val> List<Val, Alloc>::begin() const {
    return ListConstIterator<Val>{*this};
  }

  /// the iterator pointing to the rbegin (the last element) of the List
  template <typename Val, typename Alloc>
  INLINE ListConstIteratorSafe<Val> List<Val, Alloc>::crbeginSafe() const {
    if (__nb_elements)
      return ListConstIteratorSafe<Val>{*this, __nb_elements - 1};
    else
      return ListConstIteratorSafe<Val>{};
  }

  /// the iterator pointing to the rbegin (the last element) of the List
  template <typename Val, typename Alloc>
  INLINE ListIteratorSafe<Val> List<Val, Alloc>::rbeginSafe() {
    if (__nb_elements)
      return ListIteratorSafe<Val>{*this, __nb_elements - 1};
    else
      return ListIteratorSafe<Val>{};
  }

  /// the iterator pointing to the rbegin (the last element) of the List
  template <typename Val, typename Alloc>
  INLINE ListConstIterator<Val> List<Val, Alloc>::crbegin() const {
    if (__nb_elements)
      return ListConstIterator<Val>{*this, __nb_elements - 1};
    else
      return ListConstIterator<Val>{};
  }

  /// the iterator pointing to the rbegin (the last element) of the List
  template <typename Val, typename Alloc>
  INLINE ListIterator<Val> List<Val, Alloc>::rbegin() {
    if (__nb_elements)
      return ListIterator<Val>{*this, __nb_elements - 1};
    else
      return ListIterator<Val>{};
  }

  /// the iterator pointing to the rbegin (the last element) of the List
  template <typename Val, typename Alloc>
  INLINE ListConstIterator<Val> List<Val, Alloc>::rbegin() const {
    if (__nb_elements)
      return ListConstIterator<Val>{*this, __nb_elements - 1};
    else
      return ListConstIterator<Val>{};
  }

  /// create a new bucket with a given value
  template <typename Val, typename Alloc>
  INLINE ListBucket<Val> *List<Val, Alloc>::__createBucket(const Val &val) const {
    // create a new bucket (catch allocation and construction exceptions)
    ListBucket<Val> *new_elt = __alloc_bucket.allocate(1);

    try {
      __alloc_bucket.construct(new_elt, val);
    } catch (...) {
      __alloc_bucket.deallocate(new_elt, 1);
      throw;
    }

    return new_elt;
  }

  /// create a new bucket with a given value
  template <typename Val, typename Alloc>
  INLINE ListBucket<Val> *List<Val, Alloc>::__createBucket(Val &&val) const {
    // create a new bucket (catch allocation and construction exceptions)
    ListBucket<Val> *new_elt = __alloc_bucket.allocate(1);

    try {
      __alloc_bucket.construct(new_elt, std::move(val));
    } catch (...) {
      __alloc_bucket.deallocate(new_elt, 1);
      throw;
    }

    return new_elt;
  }

  /// create an emplace bucket
  template <typename Val, typename Alloc>
  template <typename... Args>
  INLINE ListBucket<Val> *
  List<Val, Alloc>::__createEmplaceBucket(Args &&... args) const {
    // create a new bucket (catch allocation and construction exceptions)
    ListBucket<Val> *new_elt = __alloc_bucket.allocate(1);

    try {
      __alloc_bucket.construct(new_elt, ListBucket<Val>::Emplace::EMPLACE,
                               std::forward<Args>(args)...);
    } catch (...) {
      __alloc_bucket.deallocate(new_elt, 1);
      throw;
    }

    return new_elt;
  }

  /// insert a bucket at the front of the list
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::__pushFront(ListBucket<Val> *new_elt) {
    new_elt->__next = __deb_list;

    if (__deb_list != nullptr)
      __deb_list->__prev = new_elt;
    else
      __end_list = new_elt;

    __deb_list = new_elt;

    // update the number of elements
    ++__nb_elements;

    // return the inserted element
    return new_elt->__val;
  }

  /// insert a bucket at the end of the list
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::__pushBack(ListBucket<Val> *new_elt) {
    // place the bucket at the end of the list
    new_elt->__prev = __end_list;

    if (__end_list != nullptr)
      __end_list->__next = new_elt;
    else
      __deb_list = new_elt;

    __end_list = new_elt;

    // update the number of elements
    ++__nb_elements;

    // returns the current value
    return new_elt->__val;
  }

  /// Insertion of a new element (a copy) at the beginning of the chained list.
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::pushFront(const Val &val) {
    return __pushFront(__createBucket(val));
  }

  /// Insertion of a new element (a copy) at the beginning of the chained list.
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::pushFront(Val &&val) {
    return __pushFront(__createBucket(std::move(val)));
  }

  /// an alias for pushFront used for STL compliance
  template <typename Val, typename Alloc>
  template <typename... Args>
  INLINE Val &List<Val, Alloc>::push_front(Args &&... args) {
    return pushFront(std::forward<Args>(args)...);
  }

  /// emplace elements at the beginning of the chained list
  template <typename Val, typename Alloc>
  template <typename... Args>
  INLINE Val &List<Val, Alloc>::emplaceFront(Args &&... args) {
    return __pushFront(__createEmplaceBucket(std::forward<Args>(args)...));
  }

  /// Insertion of a new element (a copy) at the end of the chained list.
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::pushBack(const Val &val) {
    return __pushBack(__createBucket(val));
  }

  /// pushBack for rvalues
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::pushBack(Val &&val) {
    return __pushBack(__createBucket(std::move(val)));
  }

  /// an alias for pushBack used for STL compliance
  template <typename Val, typename Alloc>
  template <typename... Args>
  INLINE Val &List<Val, Alloc>::push_back(Args &&... args) {
    return pushBack(std::forward<Args>(args)...);
  }

  /// emplace elements at the end of the chained list
  template <typename Val, typename Alloc>
  template <typename... Args>
  INLINE Val &List<Val, Alloc>::emplaceBack(Args &&... args) {
    return __pushBack(__createEmplaceBucket(std::forward<Args>(args)...));
  }

  /// Insertion of a new element at the end of the chained list (alias of pushBack)
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::insert(const Val &val) {
    return pushBack(val);
  }

  /// insert for rvalues
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::insert(Val &&val) {
    return pushBack(std::move(val));
  }

  /// returns the bucket corresponding to the ith position
  template <typename Val, typename Alloc>
  INLINE ListBucket<Val> *List<Val, Alloc>::__getIthBucket(unsigned int i) const
      noexcept {
    ListBucket<Val> *ptr;

    if (i < __nb_elements / 2) {
      for (ptr = __deb_list; i; --i, ptr = ptr->__next) {
      }
    } else {
      for (ptr = __end_list, i = __nb_elements - i - 1; i; --i, ptr = ptr->__prev) {
      }
    }

    return ptr;
  }

  /// insert a new bucket before another one
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::__insertBefore(ListBucket<Val> *new_elt,
                                               ListBucket<Val> *current_elt) {
    new_elt->__next = current_elt;
    new_elt->__prev = current_elt->__prev;
    current_elt->__prev = new_elt;

    if (new_elt->__prev == nullptr)
      __deb_list = new_elt;
    else
      new_elt->__prev->__next = new_elt;

    // update the number of elements
    ++__nb_elements;

    // returns the current value
    return new_elt->__val;
  }

  /// insert a new bucket after another one
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::__insertAfter(ListBucket<Val> *new_elt,
                                              ListBucket<Val> *current_elt) {
    new_elt->__prev = current_elt;
    new_elt->__next = current_elt->__next;
    current_elt->__next = new_elt;

    if (new_elt->__next == nullptr)
      __end_list = new_elt;
    else
      new_elt->__next->__prev = new_elt;

    // update the number of elements
    ++__nb_elements;

    // returns the current value
    return new_elt->__val;
  }

  /// inserts a new element at the ith pos of the chained list
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::insert(unsigned int pos, const Val &val) {
    // if ther are fewer elements than pos, put the value at the end of the list
    if (__nb_elements <= pos) {
      return pushBack(val);
    }

    return __insertBefore(__createBucket(val), __getIthBucket(pos));
  }

  /// insert an rvalue at the ith pos of the chained list
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::insert(unsigned int pos, Val &&val) {
    // if ther are fewer elements than pos, put the value at the end of the list
    if (__nb_elements <= pos) {
      return pushBack(std::move(val));
    }

    return __insertBefore(__createBucket(std::move(val)), __getIthBucket(pos));
  }

  /// inserts a new bucket before or after the location pointed to by an iterator
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::__insert(const const_iterator_safe &iter,
                                         ListBucket<Val> *new_elt, location place) {
    // find the location around which the new element should be inserted
    ListBucket<Val> *ptr;

    if (iter.__null_pointing) {
      if (place == location::BEFORE) {
        ptr = iter.__next_current_bucket;
      } else {
        ptr = iter.__prev_current_bucket;
      }
    } else {
      ptr = iter.__getBucket();
    }

    if (ptr == nullptr) {
      // here, we are at the end of the list
      return __pushBack(new_elt);
    } else {
      switch (place) {
        case location::BEFORE:
          return __insertBefore(new_elt, ptr);

        case location::AFTER:
          return __insertAfter(new_elt, ptr);

        default:
          GUM_ERROR(FatalError, "List insertion for this location unimplemented");
      }
    }
  }

  /// inserts a new bucket before or after the location pointed to by an iterator
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::__insert(const const_iterator &iter,
                                         ListBucket<Val> *new_elt, location place) {
    // find the location around which the new element should be inserted
    ListBucket<Val> *ptr = iter.__getBucket();

    if (ptr == nullptr) {
      // here, we are at the end of the list
      return __pushBack(new_elt);
    } else {
      switch (place) {
        case location::BEFORE:
          return __insertBefore(new_elt, ptr);

        case location::AFTER:
          return __insertAfter(new_elt, ptr);

        default:
          GUM_ERROR(FatalError, "List insertion for this location unimplemented");
      }
    }
  }

  /// inserts a new element before or after the location pointed to by an iterator
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::insert(const const_iterator_safe &iter,
                                       const Val &val, location place) {
    // if the iterator does not point to the list, raise an exception
    if (iter.__list != this) {
      GUM_ERROR(InvalidArgument, "the iterator does not point to the correct list");
    }

    return __insert(iter, __createBucket(val), place);
  }

  /// inserts a new element before or after the location pointed to by an iterator
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::insert(const const_iterator_safe &iter, Val &&val,
                                       location place) {
    // if the iterator does not point to the list, raise an exception
    if (iter.__list != this) {
      GUM_ERROR(InvalidArgument, "the iterator does not point to the correct list");
    }

    return __insert(iter, __createBucket(std::move(val)), place);
  }

  /// inserts a new element before or after the location pointed to by an iterator
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::insert(const const_iterator &iter, const Val &val,
                                       location place) {
    return __insert(iter, __createBucket(val), place);
  }

  /// inserts a new element before or after the location pointed to by an iterator
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::insert(const const_iterator &iter, Val &&val,
                                       location place) {
    return __insert(iter, __createBucket(std::move(val)), place);
  }

  /// emplace a new element before a given iterator
  template <typename Val, typename Alloc>
  template <typename... Args>
  INLINE Val &List<Val, Alloc>::emplace(const const_iterator &iter,
                                        Args &&... args) {
    return __insert(iter, __createEmplaceBucket(std::forward<Args>(args)...),
                    location::BEFORE);
  }

  /// emplace a new element before a given iterator
  template <typename Val, typename Alloc>
  template <typename... Args>
  INLINE Val &List<Val, Alloc>::emplace(const const_iterator_safe &iter,
                                        Args &&... args) {
    return __insert(iter, __createEmplaceBucket(std::forward<Args>(args)...),
                    location::BEFORE);
  }

  /// returns a reference to first element of a list
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::front() const {
    if (__nb_elements == 0) {
      GUM_ERROR(NotFound, "not enough elements in the chained list");
    }

    return __deb_list->__val;
  }

  /// returns a reference to last element of a list
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::back() const {
    if (__nb_elements == 0) {
      GUM_ERROR(NotFound, "not enough elements in the chained list");
    }

    return __end_list->__val;
  }

  /// returns the number of elements in the list.
  template <typename Val, typename Alloc>
  INLINE Size List<Val, Alloc>::size() const noexcept {
    return __nb_elements;
  }

  /// checks whether there exists a given element in the list.
  template <typename Val, typename Alloc>
  INLINE bool List<Val, Alloc>::exists(const Val &val) const {
    for (ListBucket<Val> *ptr = __deb_list; ptr != nullptr; ptr = ptr->__next)
      if (ptr->__val == val)
        return true;

    return false;
  }

  /// suppresses a given bucket from a chained list.
  template <typename Val, typename Alloc>
  INLINE void List<Val, Alloc>::__erase(ListBucket<Val> *bucket) {
    // perform deletion only if there is a bucket to remove
    if (bucket != nullptr) {
      // update the iterators pointing on this element
      for (const auto ptr_iter : __safe_iterators) {
        if (ptr_iter->__bucket == bucket) {
          ptr_iter->__next_current_bucket = bucket->__prev;
          ptr_iter->__prev_current_bucket = bucket->__next;
          ptr_iter->__bucket = nullptr;
          ptr_iter->__null_pointing = true;
        } else {
          if (ptr_iter->__null_pointing) {
            if (ptr_iter->__next_current_bucket == bucket)
              ptr_iter->__next_current_bucket = bucket->__prev;

            if (ptr_iter->__prev_current_bucket == bucket)
              ptr_iter->__prev_current_bucket = bucket->__next;
          }
        }
      }

      // set properly the begin and end of the chained list (the other chainings
      // will be performed by operator delete)
      if (bucket->__prev == nullptr)
        __deb_list = bucket->__next;
      else
        bucket->__prev->__next = bucket->__next;

      if (bucket->__next == nullptr)
        __end_list = bucket->__prev;
      else
        bucket->__next->__prev = bucket->__prev;

      // remove the current element src the list
      __alloc_bucket.destroy(bucket);
      __alloc_bucket.deallocate(bucket, 1);

      --__nb_elements;
    }
  }

  /// erases the ith element of the List (the first one is in position 0)
  template <typename Val, typename Alloc>
  INLINE void List<Val, Alloc>::erase(unsigned int i) {
    if (i >= __nb_elements)
      return;

    // erase the ith bucket
    __erase(__getIthBucket(i));
  }

  /// erases the element of the List pointed to by the iterator
  template <typename Val, typename Alloc>
  INLINE void List<Val, Alloc>::erase(const iterator_safe &iter) {
    __erase(iter.__getBucket());
  }

  /// erases the element of the List pointed to by the iterator
  template <typename Val, typename Alloc>
  INLINE void List<Val, Alloc>::erase(const const_iterator_safe &iter) {
    __erase(iter.__getBucket());
  }

  /// returns the bucket corresponding to a given value.
  template <typename Val, typename Alloc>
  INLINE ListBucket<Val> *List<Val, Alloc>::__getBucket(const Val &val) const
      noexcept {
    for (ListBucket<Val> *ptr = __deb_list; ptr != nullptr; ptr = ptr->__next)
      if (ptr->__val == val)
        return ptr;

    return nullptr;
  }

  /// erases the first element encountered with a given value
  template <typename Val, typename Alloc>
  INLINE void List<Val, Alloc>::eraseByVal(const Val &val) {
    __erase(__getBucket(val));
  }

  /// erases all the elements encountered with a given value
  template <typename Val, typename Alloc>
  INLINE void List<Val, Alloc>::eraseAllVal(const Val &val) {
    for (ListBucket<Val> *iter = __deb_list, *next_bucket = nullptr; iter != nullptr;
         iter = next_bucket) {
      next_bucket = iter->__next;

      if (val == iter->__val)
        __erase(iter);
    }
  }

  /// removes the last element of a List
  template <typename Val, typename Alloc> INLINE void List<Val, Alloc>::popBack() {
    __erase(__end_list);
  }

  /// removes the first element of a List
  template <typename Val, typename Alloc> INLINE void List<Val, Alloc>::popFront() {
    __erase(__deb_list);
  }

  /// returns a boolean indicating whether the chained list is empty
  template <typename Val, typename Alloc>
  INLINE bool List<Val, Alloc>::empty() const noexcept {
    return (__nb_elements == 0);
  }

  /// displays the content of a chained list
  template <typename Val, typename Alloc>
  std::string List<Val, Alloc>::toString() const {
    bool deja = false;
    std::stringstream stream;
    stream << "[";

    for (ListBucket<Val> *ptr = __deb_list; ptr != nullptr;
         ptr = ptr->__next, deja = true) {
      if (deja)
        stream << " --> ";

      stream << ptr->__val;
    }

    stream << "]";

    return stream.str();
  }

  /// creates a list of mountains src a list of val
  template <typename Val, typename Alloc>
  template <typename Mount, typename OtherAlloc>
  List<Mount, OtherAlloc> List<Val, Alloc>::map(Mount (*f)(Val)) const {
    // create a new empty list
    List<Mount, OtherAlloc> list;

    // fill the new list
    for (const_iterator iter = begin(); iter != end(); ++iter) {
      list.pushBack(f(*iter));
    }

    return list;
  }

  /// creates a list of mountains src a list of val
  template <typename Val, typename Alloc>
  template <typename Mount, typename OtherAlloc>
  List<Mount, OtherAlloc> List<Val, Alloc>::map(Mount (*f)(Val &)) const {
    // create a new empty list
    List<Mount, OtherAlloc> list;

    // fill the new list
    for (const_iterator iter = begin(); iter != end(); ++iter) {
      list.pushBack(f(*iter));
    }

    return list;
  }

  /// creates a list of mountains src a list of val
  template <typename Val, typename Alloc>
  template <typename Mount, typename OtherAlloc>
  List<Mount, OtherAlloc> List<Val, Alloc>::map(Mount (*f)(const Val &)) const {
    // create a new empty list
    List<Mount, OtherAlloc> list;

    // fill the new list
    for (const_iterator iter = begin(); iter != end(); ++iter) {
      list.pushBack(f(*iter));
    }

    return list;
  }

  /// creates a list of mountains with a given value src a list of val
  template <typename Val, typename Alloc>
  template <typename Mount, typename OtherAlloc>
  List<Mount, OtherAlloc> List<Val, Alloc>::map(const Mount &mount) const {
    // create a new empty list
    List<Mount, OtherAlloc> list;

    // fill the new list
    for (unsigned int i = 0; i < __nb_elements; ++i)
      list.pushBack(mount);

    return list;
  }

  /// creates and insert a new element at the end of the list (alias of pushBack).
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::operator+=(const Val &val) {
    return pushBack(val);
  }

  /// creates and insert a new element at the end of the list (alias of pushBack).
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::operator+=(Val &&val) {
    return pushBack(std::move(val));
  }

  /// checks whether two lists are identical (same elements in the same order)
  template <typename Val, typename Alloc>
  template <typename OtherAlloc>
  INLINE bool List<Val, Alloc>::operator==(const List<Val, OtherAlloc> &src) const {
    // check if the two lists have at least the same number of elements
    if (__nb_elements != src.__nb_elements)
      return false;

    // parse the two lists
    for (ListBucket<Val> *iter1 = __deb_list, *iter2 = src.__deb_list;
         iter1 != nullptr; iter1 = iter1->__next, iter2 = iter2->__next)
      if (*iter1 != *iter2)
        return false;

    return true;
  }

  /// checks whether two lists are different (different elements or orders)
  template <typename Val, typename Alloc>
  template <typename OtherAlloc>
  INLINE bool List<Val, Alloc>::operator!=(const List<Val, OtherAlloc> &src) const {
    return !operator==(src);
  }

  /// returns the ith element in the current chained list.
  template <typename Val, typename Alloc>
  INLINE Val &List<Val, Alloc>::operator[](const unsigned int i) {
    // check if we can return the element we ask for
    if (i >= __nb_elements) {
      GUM_ERROR(NotFound, "not enough elements in the chained list");
    }

    return **__getIthBucket(i);
  }

  /// returns the ith element in the current chained list.
  template <typename Val, typename Alloc>
  INLINE const Val &List<Val, Alloc>::operator[](const unsigned int i) const {
    // check if we can return the element we ask for
    if (i >= __nb_elements) {
      GUM_ERROR(NotFound, "not enough elements in the chained list");
    }

    return **__getIthBucket(i);
  }

  /// swap the current list with another one
  template <typename Val, typename Alloc>
  INLINE void List<Val, Alloc>::swap(List &other_list) {
    std::swap(__deb_list, other_list.__deb_list);
    std::swap(__end_list, other_list.__end_list);
    std::swap(__nb_elements, other_list.__nb_elements);
    std::swap(__safe_iterators, other_list.__safe_iterators);
    std::swap(__alloc_bucket, other_list.__alloc_bucket);
  }

  /// A \c << operator for List
  template <typename Val>
  std::ostream &operator<<(std::ostream &stream, const List<Val> &list) {
    stream << list.toString();
    return stream;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
