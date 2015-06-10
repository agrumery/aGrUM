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
 * @brief Implementation of the HashTable
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <iostream>
#include <sstream>
#include <string>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to help IDE parser
#include <agrum/core/hashTable.h>

/* ############################################################################# */
/* #                           WARNING:  DEPENDENCIES                          # */
/* ############################################################################# */
/*
 * agrum/core/set.tcc: to speed-up accessors in sets, we rely on the fact (which
 * holds currently) that hashTable's iterators end are never modified by
 * insertions or deletions of elements in hash tables. If this property were to be
 * changed, set.tcc should be updated accordingly
 * agrum/core/bijection.tcc: same as set.tcc but, in addition, bijections assume
 * that once a pair (key,val) has been created in the hashtable, its location in
 * memory will never change, even if the hashtable is resized.
 * agrum/core/sequence.tcc: same as bijection.tcc
 * agrum/core/priorityQueue.tcc: same as bijection.tcc
 * agrum/core/heap.tcc: same as bijection.tcc
 */
/* ############################################################################# */

namespace gum {

  /* =========================================================================== */
  /* ===                SAFE HASH TABLE ITERATORS IMPLEMENTATION             === */
  /* =========================================================================== */

  /// insert in the hashtable's list of safe iterators
  template <typename Key, typename Val>
  INLINE void HashTableConstIteratorSafe<Key, Val>::__insertIntoSafeList() const {
    __table->__safe_iterators.push_back(
        const_cast<HashTableConstIteratorSafe<Key, Val> *>(this));
  }

  /// remove the iterator for its hashtable' safe iterators list
  template <typename Key, typename Val>
  INLINE void HashTableConstIteratorSafe<Key, Val>::__removeFromSafeList() const
      noexcept {
    if (__table == nullptr)
      return;

    // find where the iterator is
    std::vector<HashTableConstIteratorSafe<Key, Val> *> &iter_vect =
        __table->__safe_iterators;

    for (int i = iter_vect.size() - 1; i >= 0; --i) {
      if (iter_vect[i] == this) {
        iter_vect.erase(iter_vect.begin() + i);
        break;
      }
    }
  }

  /// basic constructor: creates an iterator pointing to nothing
  template <typename Key, typename Val>
  INLINE HashTableConstIteratorSafe<Key, Val>::HashTableConstIteratorSafe() {
    // for debugging purposes
    GUM_CONSTRUCTOR(HashTableConstIteratorSafe);
  }

  /// constructor for an iterator pointing to beginning of a hashtable
  template <typename Key, typename Val>
  template <typename Alloc>
  INLINE HashTableConstIteratorSafe<Key, Val>::HashTableConstIteratorSafe(
      const HashTable<Key, Val, Alloc> &tab)
      : __table{reinterpret_cast<const HashTable<Key, Val> *>(&tab)} {
    // for debugging purposes
    GUM_CONSTRUCTOR(HashTableConstIteratorSafe);

    // make the hashtable keep track of this iterator
    __insertIntoSafeList();

    if (__table->__nb_elements) {
      if (__table->__begin_index != std::numeric_limits<Size>::max()) {
        __index = __table->__begin_index;
        __bucket = __table->__nodes[__index].__end_list;
      } else {
        // find the element we shall point to from the start of the hashtable
        for (unsigned int i = __table->__size - 1;; --i) { // no test on i since
          // __nb_elements != 0
          if (__table->__nodes[i].__nb_elements) {
            __index = i;
            __bucket = __table->__nodes[__index].__end_list;
            __table->__begin_index = __index;
            break;
          }
        }
      }
    }
  }

  /// constructor for an iterator pointing to the nth element of a hashtable
  template <typename Key, typename Val>
  template <typename Alloc>
  HashTableConstIteratorSafe<Key, Val>::HashTableConstIteratorSafe(
      const HashTable<Key, Val, Alloc> &tab, Size ind_elt)
      : __table{reinterpret_cast<const HashTable<Key, Val> *>(&tab)} {
    Size i;

    // check if we are looking for a begin() and we know for sure its index
    if ((ind_elt == 0) &&
        (__table->__begin_index != std::numeric_limits<Size>::max())) {
      __index = __table->__begin_index;
      __bucket = __table->__nodes[__index].__end_list;
    } else {
      // check if it is faster to find the ind_eltth element from the start or
      // from the end of the hashtable
      if (ind_elt < (__table->__nb_elements >> 1)) {
        // find the element we shall point to from the start of the hashtable
        for (i = __table->__size - 1;; --i) { // no test on i since
          // ind_elt < _table->__nb_elements
          if (__table->__nodes[i].__nb_elements) {
            if (ind_elt >= __table->__nodes[i].__nb_elements)
              ind_elt -= __table->__nodes[i].__nb_elements;
            else {
              for (__bucket = __table->__nodes[i].__end_list; ind_elt;
                   --ind_elt, __bucket = __bucket->prev) {
              }

              __index = i;
              break;
            }
          }
        }
      } else {
        // ind_elt = the index of the element we should point to
        // check if the index passed as parameter is valid
        if (ind_elt >= __table->__nb_elements) {
          GUM_ERROR(UndefinedIteratorValue, "Not enough elements in the hashtable");
        }

        // find the element we shall point to from the end of the hashtable
        for (i = 0, ind_elt = __table->__nb_elements - ind_elt - 1;; ++i) {
          if (__table->__nodes[i].__nb_elements) {
            if (ind_elt >= __table->__nodes[i].__nb_elements)
              ind_elt -= __table->__nodes[i].__nb_elements;
            else {
              for (__bucket = __table->__nodes[i].__deb_list; ind_elt;
                   --ind_elt, __bucket = __bucket->next) {
              }

              __index = i;
              break;
            }
          }
        }
      }
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(HashTableConstIteratorSafe);

    // make the hashtable keep track of this iterator
    __insertIntoSafeList();
  }

  /// copy constructor
  template <typename Key, typename Val>
  INLINE HashTableConstIteratorSafe<Key, Val>::HashTableConstIteratorSafe(
      const HashTableConstIteratorSafe<Key, Val> &from)
      : __table{from.__table}, __index{from.__index}, __bucket{from.__bucket},
        __next_bucket{from.__next_bucket} {
    // make the hashtable keep track of this iterator
    if (__table != nullptr) {
      __insertIntoSafeList();
    }

    // for debugging purposes
    GUM_CONS_CPY(HashTableConstIteratorSafe);
  }

  /// copy constructor
  template <typename Key, typename Val>
  INLINE HashTableConstIteratorSafe<Key, Val>::HashTableConstIteratorSafe(
      const HashTableConstIterator<Key, Val> &from)
      : __table{from.__table}, __index{from.__index}, __bucket{from.__bucket} {
    // make the hashtable keep track of this iterator
    if (__table != nullptr) {
      __insertIntoSafeList();
    }

    // for debugging purposes
    GUM_CONS_CPY(HashTableConstIteratorSafe);
  }

  /// move constructor
  template <typename Key, typename Val>
  INLINE HashTableConstIteratorSafe<Key, Val>::HashTableConstIteratorSafe(
      HashTableConstIteratorSafe<Key, Val> &&from)
      : __table{from.__table}, __index{from.__index}, __bucket{from.__bucket},
        __next_bucket{from.__next_bucket} {
    GUM_CONS_MOV(HashTableConstIteratorSafe);

    // find "from" in the hashtable's list of safe iterators and substitute
    // it by this
    if (__table != nullptr) {
      std::vector<HashTableConstIteratorSafe<Key, Val> *> &vect =
          __table->__safe_iterators;

      for (auto ptr = vect.rbegin(); ptr != vect.rend(); ++ptr) {
        if (*ptr == &from) {
          *ptr = this;
          from.__table = nullptr;
          break;
        }
      }
    }
  }

  /// destructor
  template <typename Key, typename Val>
  INLINE
      HashTableConstIteratorSafe<Key, Val>::~HashTableConstIteratorSafe() noexcept {
    // for debugging purposes
    GUM_DESTRUCTOR(HashTableConstIteratorSafe);

    // remove the iterator from the table's iterator list
    __removeFromSafeList();
  }

  /// copy operator
  template <typename Key, typename Val>
  HashTableConstIteratorSafe<Key, Val> &HashTableConstIteratorSafe<Key, Val>::
  operator=(const HashTableConstIteratorSafe<Key, Val> &from) {
    // here, no need to avoid self assignment: this would slow down normal
    // assignments and, in any case, this would not result in an iterator in
    // an incoherent state
    // check if the current hashtable is different from that of "from". In such
    // a case, we shall remove the iterator from its current hashtable iterator's
    // list and add it to the new hashtable iterator's list
    if (__table != from.__table) {
      // remove the iterator from its hashtable iterator's list'
      __removeFromSafeList();

      __table = from.__table;

      // add to the new table
      if (__table) {
        __insertIntoSafeList();
      }
    }

    __index = from.__index;
    __bucket = from.__bucket;
    __next_bucket = from.__next_bucket;

    return *this;
  }

  /// copy operator
  template <typename Key, typename Val>
  HashTableConstIteratorSafe<Key, Val> &HashTableConstIteratorSafe<Key, Val>::
  operator=(const HashTableConstIterator<Key, Val> &from) {
    // here, no need to avoid self assignment: this would slow down normal
    // assignments and, in any case, this would not result in an iterator in
    // an incoherent state
    // check if the current hashtable is different from that of "from". In such
    // a case, we shall remove the iterator from its current hashtable iterator's
    // list and add it to the new hashtable iterator's list
    if (__table != from.__table) {
      // remove the iterator from its hashtable iterator's list'
      __removeFromSafeList();

      __table = from.__table;

      // add to the new table
      if (__table) {
        __insertIntoSafeList();
      }
    }

    __index = from.__index;
    __bucket = from.__bucket;
    __next_bucket = nullptr;

    return *this;
  }

  /// move operator
  template <typename Key, typename Val>
  INLINE HashTableConstIteratorSafe<Key, Val> &HashTableConstIteratorSafe<Key, Val>::
  operator=(HashTableConstIteratorSafe<Key, Val> &&from) noexcept {
    // here, no need to avoid self assignment: this would slow down normal
    // assignments and, in any case, this would not result in an iterator in
    // an incoherent state
    // check if the current hashtable is different from that of "from". In such
    // a case, we shall remove the iterator from its current hashtable iterator's
    // list and add it to the new hashtable iterator's list
    if (__table != from.__table) {
      // remove the iterator from its hashtable iterator's list'
      __removeFromSafeList();

      if (from.__table != nullptr) {
        // substitute from by this in the list of safe iterators
        std::vector<HashTableConstIteratorSafe<Key, Val> *> &vect =
            from.__table->__safe_iterators;

        for (auto ptr = vect.rbegin(); ptr != vect.rend(); ++ptr) {
          if (*ptr == &from) {
            *ptr = this;
            break;
          }
        }
      }

      __table = from.__table;
      from.__table = nullptr;
    }

    __index = from.__index;
    __bucket = from.__bucket;
    __next_bucket = from.__next_bucket;

    return *this;
  }

  /// returns the key corresponding to the current iterator's bucket
  template <typename Key, typename Val>
  INLINE const typename HashTableConstIteratorSafe<Key, Val>::key_type &
  HashTableConstIteratorSafe<Key, Val>::key() const {
    if (__bucket)
      return __bucket->key();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object");
    }
  }

  /// returns the mapped value corresponding to the current iterator's bucket
  template <typename Key, typename Val>
  INLINE const typename HashTableConstIteratorSafe<Key, Val>::mapped_type &
  HashTableConstIteratorSafe<Key, Val>::val() const {
    if (__bucket)
      return __bucket->val();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object");
    }
  }

  /** @brief makes the iterator point toward nothing (in particular, it is not
   * related anymore to its current hash table) */
  template <typename Key, typename Val>
  INLINE void HashTableConstIteratorSafe<Key, Val>::clear() noexcept {
    // remove the iterator from the table's iterator list
    __removeFromSafeList();

    // set its table as well as the element it points to to 0
    __table = nullptr;
    __bucket = nullptr;
    __next_bucket = nullptr;
    __index = 0;
  }

  /// makes the iterator point to the next element in the hash table
  // WARNING: never inline this function: this result in g++4.3.3 producing a code
  // that segfaults.
  template <typename Key, typename Val>
  HashTableConstIteratorSafe<Key, Val> &HashTableConstIteratorSafe<Key, Val>::
  operator++() noexcept {
    // if __bucket != nullptr then use it, else use next_bucket
    if (__bucket == nullptr) {
      // note that this case only happens when the iterator pointed to an element
      // that has just been erased. Fortunately, in this case, the Hashtable's
      // erase functions update appropriately the __next_bucket and __index fields.
      __bucket = __next_bucket;
      __next_bucket = nullptr;
    } else {
      // ok, here we can use __bucket as a starting point

      // if we are not pointing on the first element of the chained list, just
      // point to the preceding bucket in this list
      if (__bucket->prev) {
        __bucket = __bucket->prev;
        // here, no need to update __next_bucket, which is compulsorily
        // equal to nullptr, nor __index which has not changed.
      } else {
        // ok, here we are on the beginning of a chained list,
        // so 2 cases can obtain:
        // 1/ index = 0 : then we have reached the end of the hashtable
        // 2/ index != 0 => we must search for a new slot containing elements

        // case 1:
        if (!__index) {
          __bucket = nullptr;
          // we are thus at the end() of the hashTable
        }

        // case 2:
        else {
          // arrived here, we need to parse the hash table until we find a new
          // bucket because we are pointing on a chained list with no more element
          // to the left of the current element
          for (Size i = __index - 1UL; i; --i) {
            if (__table->__nodes[i].__nb_elements) {
              __index = i;
              __bucket = __table->__nodes[i].__end_list;
              return *this;
            }
          }

          if (__table->__nodes[0].__nb_elements)
            __bucket = __table->__nodes[0].__end_list;
          else
            __bucket = nullptr;

          __index = 0;
        }
      }
    }

    return *this;
  }

  /// makes the iterator point to i elements further in the hashtable
  template <typename Key, typename Val>
  HashTableConstIteratorSafe<Key, Val> &HashTableConstIteratorSafe<Key, Val>::
  operator+=(unsigned int nb) noexcept {
    if ((nb == 0) || (__table == nullptr))
      return *this;

    // if __bucket != nullptr then use it, else use next_bucket
    if (__bucket == nullptr) {
      // note that this case only happens when the iterator pointed to an element
      // that has just been erased. Fortunately, in this case, the Hashtable's
      // erase functions update appropriately the __next_bucket and __index fields.
      __bucket = __next_bucket;
      __next_bucket = nullptr;
      --nb;
    }

    // ok, here we can use __bucket as a starting point: parse all the elements
    // of the current chained list
    for (; nb && __bucket != nullptr; --nb, __bucket = __bucket->prev) {
    }

    if (__bucket != nullptr)
      return *this;

    // here, we shall skip all the chained list that have not sufficiently
    // many elements
    --__index;

    for (;
         __index < __table->__size && nb >= __table->__nodes[__index].__nb_elements;
         nb -= __table->__nodes[__index].__nb_elements, --__index) {
    }

    // here: either __index >= __table->__size, which means that we did not find
    // the element we looked for, i.e., we are at the end of the hashtable, or
    // nb < __table->__nodes[__index].__nb_elements, and we should parse the
    // chained list to get the element (which, we know for sure, exists)
    if (__index >= __table->__size) {
      __index = 0;
      return *this;
    }

    for (__bucket = __table->__nodes[__index].__end_list; nb;
         --nb, __bucket = __bucket->prev) {
    }

    return *this;
  }

  /// returns a new iterator
  template <typename Key, typename Val>
  INLINE HashTableConstIteratorSafe<Key, Val> HashTableConstIteratorSafe<Key, Val>::
  operator+(unsigned int nb) const {
    return HashTableConstIteratorSafe<Key, Val>{*this} += nb;
  }

  /// checks whether two iterators are pointing toward different elements
  template <typename Key, typename Val>
  INLINE bool HashTableConstIteratorSafe<Key, Val>::
  operator!=(const HashTableConstIteratorSafe<Key, Val> &from) const noexcept {
    return ((__bucket != from.__bucket) || (__index != from.__index));
  }

  /// checks whether two iterators are pointing toward the same element
  template <typename Key, typename Val>
  INLINE bool HashTableConstIteratorSafe<Key, Val>::
  operator==(const HashTableConstIteratorSafe<Key, Val> &from) const noexcept {
    return ((__bucket == from.__bucket) && (__index == from.__index));
  }

  /// returns the value pointed to by the iterator
  template <typename Key, typename Val>
  INLINE const typename HashTableConstIteratorSafe<Key, Val>::value_type &
      HashTableConstIteratorSafe<Key, Val>::
      operator*() const {
    if (__bucket)
      return __bucket->elt();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object");
    }
  }

  /// returns the current iterator's bucket
  template <typename Key, typename Val>
  INLINE HashTableBucket<Key, Val> *
  HashTableConstIteratorSafe<Key, Val>::__getBucket() const noexcept {
    return __bucket;
  }

  /// returns the index in the hashtable's node vector pointed to by the iterator
  template <typename Key, typename Val>
  INLINE Size HashTableConstIteratorSafe<Key, Val>::__getIndex() const noexcept {
    return __index;
  }

  /* =========================================================================== */
  /* ===                SAFE HASH TABLE ITERATORS IMPLEMENTATION             === */
  /* =========================================================================== */

  /// basic constructor: creates an iterator pointing to nothing
  template <typename Key, typename Val>
  INLINE HashTableIteratorSafe<Key, Val>::HashTableIteratorSafe()
      : HashTableConstIteratorSafe<Key, Val>() {
    GUM_CONSTRUCTOR(HashTableIteratorSafe);
  }

  /// constructor for an iterator pointing to the beginning of a hashtable
  template <typename Key, typename Val>
  template <typename Alloc>
  INLINE HashTableIteratorSafe<Key, Val>::HashTableIteratorSafe(
      const HashTable<Key, Val, Alloc> &tab)
      : HashTableConstIteratorSafe<Key, Val>(tab) {
    GUM_CONSTRUCTOR(HashTableIteratorSafe);
  }

  /// constructor for an iterator pointing to the nth element of a hashtable
  template <typename Key, typename Val>
  template <typename Alloc>
  INLINE HashTableIteratorSafe<Key, Val>::HashTableIteratorSafe(
      const HashTable<Key, Val, Alloc> &tab, Size ind_elt)
      : HashTableConstIteratorSafe<Key, Val>(tab, ind_elt) {
    GUM_CONSTRUCTOR(HashTableIteratorSafe);
  }

  /// copy constructor
  template <typename Key, typename Val>
  INLINE HashTableIteratorSafe<Key, Val>::HashTableIteratorSafe(
      const HashTableIteratorSafe<Key, Val> &from)
      : HashTableConstIteratorSafe<Key, Val>(from) {
    GUM_CONS_CPY(HashTableIteratorSafe);
  }

  /// copy constructor
  template <typename Key, typename Val>
  INLINE HashTableIteratorSafe<Key, Val>::HashTableIteratorSafe(
      const HashTableIterator<Key, Val> &from)
      : HashTableConstIteratorSafe<Key, Val>(from) {
    GUM_CONS_CPY(HashTableIteratorSafe);
  }

  /// move constructor
  template <typename Key, typename Val>
  INLINE HashTableIteratorSafe<Key, Val>::HashTableIteratorSafe(
      HashTableIteratorSafe<Key, Val> &&from) noexcept
      : HashTableConstIteratorSafe<Key, Val>(std::move(from)) {
    GUM_CONS_MOV(HashTableIteratorSafe);
  }

  /// destructor
  template <typename Key, typename Val>
  INLINE HashTableIteratorSafe<Key, Val>::~HashTableIteratorSafe() noexcept {
    GUM_DESTRUCTOR(HashTableIteratorSafe);
  }

  /// returns the mapped value corresponding to the current iterator's bucket
  template <typename Key, typename Val>
  INLINE typename HashTableIteratorSafe<Key, Val>::mapped_type &
  HashTableIteratorSafe<Key, Val>::val() {
    return const_cast<Val &>(HashTableConstIteratorSafe<Key, Val>::val());
  }

  /// copy operator
  template <typename Key, typename Val>
  INLINE HashTableIteratorSafe<Key, Val> &HashTableIteratorSafe<Key, Val>::
  operator=(const HashTableIteratorSafe<Key, Val> &from) {
    GUM_OP_CPY(HashTableIteratorSafe);
    HashTableConstIteratorSafe<Key, Val>::operator=(from);
    return *this;
  }

  /// copy operator
  template <typename Key, typename Val>
  INLINE HashTableIteratorSafe<Key, Val> &HashTableIteratorSafe<Key, Val>::
  operator=(const HashTableIterator<Key, Val> &from) {
    GUM_OP_CPY(HashTableIteratorSafe);
    HashTableConstIteratorSafe<Key, Val>::operator=(from);
    return *this;
  }

  /// move operator
  template <typename Key, typename Val>
  INLINE HashTableIteratorSafe<Key, Val> &HashTableIteratorSafe<Key, Val>::
  operator=(HashTableIteratorSafe<Key, Val> &&from) noexcept {
    HashTableConstIteratorSafe<Key, Val>::operator=(std::move(from));
    return *this;
  }

  /// makes the iterator point to the next element in the hash table
  template <typename Key, typename Val>
  INLINE HashTableIteratorSafe<Key, Val> &HashTableIteratorSafe<Key, Val>::
  operator++() noexcept {
    HashTableConstIteratorSafe<Key, Val>::operator++();
    return *this;
  }

  /// makes the iterator point to i elements further in the hashtable
  template <typename Key, typename Val>
  INLINE HashTableIteratorSafe<Key, Val> &HashTableIteratorSafe<Key, Val>::
  operator+=(unsigned int nb) noexcept {
    HashTableConstIteratorSafe<Key, Val>::operator+=(nb);
    return *this;
  }

  /// returns a new iterator
  template <typename Key, typename Val>
  INLINE HashTableIteratorSafe<Key, Val> HashTableIteratorSafe<Key, Val>::
  operator+(unsigned int nb) const {
    HashTableIteratorSafe<Key, Val> iter{*this};
    iter += nb;
    return iter;
  }

  /// checks whether two iterators are pointing toward different elements
  template <typename Key, typename Val>
  INLINE bool HashTableIteratorSafe<Key, Val>::
  operator!=(const HashTableIteratorSafe<Key, Val> &from) const noexcept {
    return HashTableConstIteratorSafe<Key, Val>::operator!=(from);
  }

  /// checks whether two iterators are pointing toward the same element
  template <typename Key, typename Val>
  INLINE bool HashTableIteratorSafe<Key, Val>::
  operator==(const HashTableIteratorSafe<Key, Val> &from) const noexcept {
    return HashTableConstIteratorSafe<Key, Val>::operator==(from);
  }

  /// returns the value pointed to by the iterator
  template <typename Key, typename Val>
  INLINE typename HashTableIteratorSafe<Key, Val>::value_type &
      HashTableIteratorSafe<Key, Val>::
      operator*() {
    return const_cast<Val &>(HashTableConstIteratorSafe<Key, Val>::operator*());
  }

  /// returns the value pointed to by the iterator
  template <typename Key, typename Val>
  INLINE const typename HashTableIteratorSafe<Key, Val>::value_type &
      HashTableIteratorSafe<Key, Val>::
      operator*() const {
    return HashTableConstIteratorSafe<Key, Val>::operator*();
  }

  /* =========================================================================== */
  /* ===            UNSAFE HASH TABLE CONST ITERATORS IMPLEMENTATION         === */
  /* =========================================================================== */

  /// basic constructor: creates an iterator pointing to nothing
  template <typename Key, typename Val>
  INLINE HashTableConstIterator<Key, Val>::HashTableConstIterator() noexcept {
    GUM_CONSTRUCTOR(HashTableConstIterator);
  }

  /// constructor for an iterator pointing to beginning of a hashtable
  template <typename Key, typename Val>
  template <typename Alloc>
  INLINE HashTableConstIterator<Key, Val>::HashTableConstIterator(
      const HashTable<Key, Val, Alloc> &tab) noexcept
      : __table{reinterpret_cast<const HashTable<Key, Val> *>(&tab)} {
    // for debugging purposes
    GUM_CONSTRUCTOR(HashTableConstIterator);

    if (__table->__nb_elements) {
      if (__table->__begin_index != std::numeric_limits<Size>::max()) {
        __index = __table->__begin_index;
        __bucket = __table->__nodes[__index].__end_list;
      } else {
        // find the element we shall point to from the start of the hashtable
        for (unsigned int i = __table->__size - 1;; --i) { // no test on i since
          // __nb_elements != 0
          if (__table->__nodes[i].__nb_elements) {
            __index = i;
            __bucket = __table->__nodes[__index].__end_list;
            __table->__begin_index = __index;
            break;
          }
        }
      }
    }
  }

  /// constructor for an iterator pointing to the nth element of a hashtable
  template <typename Key, typename Val>
  template <typename Alloc>
  HashTableConstIterator<Key, Val>::HashTableConstIterator(
      const HashTable<Key, Val, Alloc> &tab, Size ind_elt)
      : __table{reinterpret_cast<const HashTable<Key, Val> *>(&tab)} {
    Size i;

    // check if we are looking for a begin() and we know for sure its index
    if ((ind_elt == 0) &&
        (__table->__begin_index != std::numeric_limits<Size>::max())) {
      __index = __table->__begin_index;
      __bucket = __table->__nodes[__index].__end_list;
    } else {
      // check if it is faster to find the ind_eltth element from the start or
      // from the end of the hashtable
      if (ind_elt < (__table->__nb_elements >> 1)) {
        // find the element we shall point to from the start of the hashtable
        for (i = __table->__size - 1;; --i) { // no test on i since
          // ind_elt < _table->__nb_elements
          if (__table->__nodes[i].__nb_elements) {
            if (ind_elt >= __table->__nodes[i].__nb_elements)
              ind_elt -= __table->__nodes[i].__nb_elements;
            else {
              for (__bucket = __table->__nodes[i].__end_list; ind_elt;
                   --ind_elt, __bucket = __bucket->prev) {
              }

              __index = i;
              break;
            }
          }
        }
      } else {
        // ind_elt = the index of the element we should point to
        // check if the index passed as parameter is valid
        if (ind_elt >= __table->__nb_elements) {
          GUM_ERROR(UndefinedIteratorValue, "Not enough elements in the hashtable");
        }

        // find the element we shall point to from the end of the hashtable
        for (i = 0, ind_elt = __table->__nb_elements - ind_elt - 1;; ++i) {
          if (__table->__nodes[i].__nb_elements) {
            if (ind_elt >= __table->__nodes[i].__nb_elements)
              ind_elt -= __table->__nodes[i].__nb_elements;
            else {
              for (__bucket = __table->__nodes[i].__deb_list; ind_elt;
                   --ind_elt, __bucket = __bucket->next) {
              }

              __index = i;
              break;
            }
          }
        }
      }
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(HashTableConstIterator);
  }

  /// copy constructor
  template <typename Key, typename Val>
  INLINE HashTableConstIterator<Key, Val>::HashTableConstIterator(
      const HashTableConstIterator<Key, Val> &from) noexcept
      : __table{from.__table},
        __index{from.__index},
        __bucket{from.__bucket} {
    GUM_CONS_CPY(HashTableConstIterator);
  }

  /// move constructor
  template <typename Key, typename Val>
  INLINE HashTableConstIterator<Key, Val>::HashTableConstIterator(
      HashTableConstIterator<Key, Val> &&from) noexcept : __table{from.__table},
                                                          __index{from.__index},
                                                          __bucket{from.__bucket} {
    GUM_CONS_MOV(HashTableConstIterator);
  }

  /// destructor
  template <typename Key, typename Val>
  INLINE HashTableConstIterator<Key, Val>::~HashTableConstIterator() noexcept {
    // for debugging purposes
    GUM_DESTRUCTOR(HashTableConstIterator);
  }

  /// copy operator
  template <typename Key, typename Val>
  INLINE HashTableConstIterator<Key, Val> &HashTableConstIterator<Key, Val>::
  operator=(const HashTableConstIterator<Key, Val> &from) noexcept {
    // here, no need to avoid self assignment: this would slow down normal
    // assignments and, in any case, this would not result in an iterator in
    // an incoherent state
    __table = from.__table;
    __index = from.__index;
    __bucket = from.__bucket;

    return *this;
  }

  /// move operator
  template <typename Key, typename Val>
  INLINE HashTableConstIterator<Key, Val> &HashTableConstIterator<Key, Val>::
  operator=(HashTableConstIterator<Key, Val> &&from) noexcept {
    // here, no need to avoid self assignment: this would slow down normal
    // assignments and, in any case, this would not result in an iterator in
    // an incoherent state
    __table = from.__table;
    __index = from.__index;
    __bucket = from.__bucket;

    return *this;
  }

  /// returns the key corresponding to the current iterator's bucket
  template <typename Key, typename Val>
  INLINE const typename HashTableConstIterator<Key, Val>::key_type &
  HashTableConstIterator<Key, Val>::key() const {
    if (__bucket)
      return __bucket->pair.first;
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object");
    }
  }

  /// returns the mapped value corresponding to the current iterator's bucket
  template <typename Key, typename Val>
  INLINE const typename HashTableConstIterator<Key, Val>::mapped_type &
  HashTableConstIterator<Key, Val>::val() const {
    if (__bucket)
      return __bucket->val();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object");
    }
  }

  /** @brief makes the iterator point toward nothing (in particular, it is not
   * related anymore to its current hash table) */
  template <typename Key, typename Val>
  INLINE void HashTableConstIterator<Key, Val>::clear() noexcept {
    __table = nullptr;
    __bucket = nullptr;
    __index = 0;
  }

  /// makes the iterator point to the next element in the hash table
  template <typename Key, typename Val>
  HashTableConstIterator<Key, Val> &HashTableConstIterator<Key, Val>::
  operator++() noexcept {
    // if __bucket == nullptr then we are at the end of the hashtable
    if (__bucket == nullptr)
      return *this;

    // if we are not pointing on the first element of the chained list, just
    // point to the next bucket in this list
    if (__bucket->prev) {
      __bucket = __bucket->prev;
      // here, no need to update __index which has not changed.
    } else {
      // ok, here we are on the end of a chained list,
      // so 2 cases can obtain:
      // 1/ index = 0 : then we have reached the end of the hashtable
      // 2/ index != 0 => we must search for a new slot containing elements

      // case 1:
      if (!__index) {
        __bucket = nullptr;
        // we are thus at the end() of the hashTable
      }

      // case 2:
      else {
        // arrived here, we need to parse the hash table until we find a new
        // bucket because we are pointing on a chained list with no more element
        // to the right of the current element
        for (Size i = __index - 1UL; i; --i) {
          if (__table->__nodes[i].__nb_elements) {
            __index = i;
            __bucket = __table->__nodes[i].__end_list;
            return *this;
          }
        }

        if (__table->__nodes[0].__nb_elements)
          __bucket = __table->__nodes[0].__end_list;
        else
          __bucket = nullptr;

        __index = 0;
      }
    }

    return *this;
  }

  /// makes the iterator point to i elements further in the hashtable
  template <typename Key, typename Val>
  HashTableConstIterator<Key, Val> &HashTableConstIterator<Key, Val>::
  operator+=(unsigned int nb) noexcept {
    if ((nb == 0) || (__table == nullptr) || (__bucket == nullptr))
      return *this;

    // ok, here we can use __bucket as a starting point: parse all the elements
    // of the current chained list
    for (; nb && __bucket != nullptr; --nb, __bucket = __bucket->prev) {
    }

    if (__bucket != nullptr)
      return *this;

    // here, we shall skip all the chained list that have not sufficiently
    // many elements
    --__index;

    for (;
         __index < __table->__size && nb >= __table->__nodes[__index].__nb_elements;
         nb -= __table->__nodes[__index].__nb_elements, --__index) {
    }

    // here: either __index >= __table->__size, which means that we did not find
    // the element we looked for, i.e., we are at the end of the hashtable, or
    // nb < __table->__nodes[__index].__nb_elements, and we should parse the
    // chained list to get the element (which, we know for sure, exists)
    if (__index >= __table->__size) {
      __index = 0;
      return *this;
    }

    for (__bucket = __table->__nodes[__index].__end_list; nb;
         --nb, __bucket = __bucket->prev) {
    }

    return *this;
  }

  /// returns a new iterator
  template <typename Key, typename Val>
  INLINE HashTableConstIterator<Key, Val> HashTableConstIterator<Key, Val>::
  operator+(unsigned int nb) const noexcept {
    return HashTableConstIterator<Key, Val>{*this} += nb;
  }

  /// checks whether two iterators are pointing toward different elements
  template <typename Key, typename Val>
  INLINE bool HashTableConstIterator<Key, Val>::
  operator!=(const HashTableConstIterator<Key, Val> &from) const noexcept {
    return (__bucket != from.__bucket);
  }

  /// checks whether two iterators are pointing toward the same element
  template <typename Key, typename Val>
  INLINE bool HashTableConstIterator<Key, Val>::
  operator==(const HashTableConstIterator<Key, Val> &from) const noexcept {
    return (__bucket == from.__bucket);
  }

  /// returns the value pointed to by the iterator
  template <typename Key, typename Val>
  INLINE const typename HashTableConstIterator<Key, Val>::value_type &
      HashTableConstIterator<Key, Val>::
      operator*() const {
    if (__bucket)
      return __bucket->elt();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object");
    }
  }

  /// returns the current iterator's bucket
  template <typename Key, typename Val>
  INLINE typename HashTable<Key, Val>::Bucket *
  HashTableConstIterator<Key, Val>::__getBucket() const noexcept {
    return __bucket;
  }

  /// returns the index in the hashtable's node vector pointed to by the iterator
  template <typename Key, typename Val>
  INLINE Size HashTableConstIterator<Key, Val>::__getIndex() const noexcept {
    return __index;
  }

  /* =========================================================================== */
  /* ===               UNSAFE HASH TABLE ITERATORS IMPLEMENTATION            === */
  /* =========================================================================== */

  /// basic constructor: creates an iterator pointing to nothing
  template <typename Key, typename Val>
  INLINE HashTableIterator<Key, Val>::HashTableIterator() noexcept
      : HashTableConstIterator<Key, Val>() {
    GUM_CONSTRUCTOR(HashTableIterator);
  }

  /// constructor for an iterator pointing to the beginning of a hashtable
  template <typename Key, typename Val>
  template <typename Alloc>
  INLINE HashTableIterator<Key, Val>::HashTableIterator(
      const HashTable<Key, Val, Alloc> &tab) noexcept
      : HashTableConstIterator<Key, Val>{tab} {
    GUM_CONSTRUCTOR(HashTableIterator);
  }

  /// constructor for an iterator pointing to the nth element of a hashtable
  template <typename Key, typename Val>
  template <typename Alloc>
  INLINE HashTableIterator<Key, Val>::HashTableIterator(
      const HashTable<Key, Val, Alloc> &tab, Size ind_elt)
      : HashTableConstIterator<Key, Val>(tab, ind_elt) {
    GUM_CONSTRUCTOR(HashTableIterator);
  }

  /// copy constructor
  template <typename Key, typename Val>
  INLINE HashTableIterator<Key, Val>::HashTableIterator(
      const HashTableIterator<Key, Val> &from) noexcept
      : HashTableConstIterator<Key, Val>(from) {
    GUM_CONS_CPY(HashTableIterator);
  }

  /// move constructor
  template <typename Key, typename Val>
  INLINE HashTableIterator<Key, Val>::HashTableIterator(
      HashTableIterator<Key, Val> &&from) noexcept
      : HashTableConstIterator<Key, Val>(std::move(from)) {
    GUM_CONS_MOV(HashTableIterator);
  }

  /// destructor
  template <typename Key, typename Val>
  INLINE HashTableIterator<Key, Val>::~HashTableIterator() noexcept {
    GUM_DESTRUCTOR(HashTableIterator);
  }

  /// returns the mapped value corresponding to the current iterator's bucket
  template <typename Key, typename Val>
  INLINE typename HashTableIterator<Key, Val>::mapped_type &
  HashTableIterator<Key, Val>::val() {
    if (this->__bucket)
      return this->__bucket->val();
    else {
      GUM_ERROR(UndefinedIteratorValue, "Accessing a nullptr object");
    }
  }

  /// copy operator
  template <typename Key, typename Val>
  INLINE HashTableIterator<Key, Val> &HashTableIterator<Key, Val>::
  operator=(const HashTableIterator<Key, Val> &from) noexcept {
    HashTableConstIterator<Key, Val>::operator=(from);
    return *this;
  }

  /// move operator
  template <typename Key, typename Val>
  INLINE HashTableIterator<Key, Val> &HashTableIterator<Key, Val>::
  operator=(HashTableIterator<Key, Val> &&from) noexcept {
    HashTableConstIterator<Key, Val>::operator=(std::move(from));
    return *this;
  }

  /// makes the iterator point to the next element in the hash table
  template <typename Key, typename Val>
  INLINE HashTableIterator<Key, Val> &HashTableIterator<Key, Val>::
  operator++() noexcept {
    HashTableConstIterator<Key, Val>::operator++();
    return *this;
  }

  /// makes the iterator point to i elements further in the hashtable
  template <typename Key, typename Val>
  INLINE HashTableIterator<Key, Val> &HashTableIterator<Key, Val>::
  operator+=(unsigned int nb) noexcept {
    HashTableConstIterator<Key, Val>::operator+=(nb);
    return *this;
  }

  /// returns a new iterator
  template <typename Key, typename Val>
  INLINE HashTableIterator<Key, Val> HashTableIterator<Key, Val>::
  operator+(unsigned int nb) const noexcept {
    HashTableIterator<Key, Val> iter{*this};
    iter += nb;
    return iter;
  }

  /// checks whether two iterators are pointing toward different elements
  template <typename Key, typename Val>
  INLINE bool HashTableIterator<Key, Val>::
  operator!=(const HashTableIterator<Key, Val> &from) const noexcept {
    return HashTableConstIterator<Key, Val>::operator!=(from);
  }

  /// checks whether two iterators are pointing toward the same element
  template <typename Key, typename Val>
  INLINE bool HashTableIterator<Key, Val>::
  operator==(const HashTableIterator<Key, Val> &from) const noexcept {
    return HashTableConstIterator<Key, Val>::operator==(from);
  }

  /// returns the value pointed to by the iterator
  template <typename Key, typename Val>
  INLINE typename HashTableIterator<Key, Val>::value_type &
      HashTableIterator<Key, Val>::
      operator*() {
    return const_cast<value_type &>(HashTableConstIterator<Key, Val>::operator*());
  }

  /// returns the value pointed to by the iterator
  template <typename Key, typename Val>
  INLINE const typename HashTableIterator<Key, Val>::value_type &
      HashTableIterator<Key, Val>::
      operator*() const {
    return HashTableConstIterator<Key, Val>::operator*();
  }

  /* =========================================================================== */
  /* ===     IMPLEMENTATION OF THE CHAINED LISTS USED IN THE HASH TABLES     === */
  /* =========================================================================== */

  /// a function used to perform copies of HashTableLists
  template <typename Key, typename Val, typename Alloc>
  template <typename OtherAlloc>
  void HashTableList<Key, Val, Alloc>::__copy(
      const HashTableList<Key, Val, OtherAlloc> &from) {
    Bucket *ptr, *old_ptr{nullptr}, *new_elt{nullptr};
    // set the defaults
    __deb_list = nullptr;

    // copy from's list
    try {
      for (ptr = from.__deb_list; ptr != nullptr; ptr = ptr->next) {
        // copy the current from's bucket (may throw an exception either because
        // new cannot allocate the bucket or because the copy constructor of Val
        // throws an exception)
        new_elt = __alloc_bucket->allocate(1);

        try {
          __alloc_bucket->construct(new_elt, *ptr);
        } catch (...) {
          __alloc_bucket->deallocate(new_elt, 1);
          throw;
        }

        // rechain properly the new list
        new_elt->prev = old_ptr;

        if (old_ptr != nullptr)
          old_ptr->next = new_elt;
        else
          __deb_list = new_elt;

        old_ptr = new_elt;
      }

      if (old_ptr != nullptr)
        old_ptr->next = nullptr;

      // update the number of elements stored into the list and the end of the list
      __nb_elements = from.__nb_elements;
      __end_list = new_elt;
    } catch (...) {
      // problem: we could not allocate an element in the list => we delete
      // the elements created so far and we throw an exception
      for (; __deb_list != nullptr; __deb_list = new_elt) {
        new_elt = __deb_list->next;
        __alloc_bucket->destroy(__deb_list);
        __alloc_bucket->deallocate(__deb_list, 1);
      }

      __nb_elements = 0;
      __end_list = nullptr;

      throw;
    }
  }

  /// returns the bucket corresponding to a given key
  template <typename Key, typename Val, typename Alloc>
  INLINE HashTableBucket<Key, Val> *
  HashTableList<Key, Val, Alloc>::bucket(const Key &key) const {
    for (Bucket *ptr = __deb_list; ptr != nullptr; ptr = ptr->next)
      if (ptr->key() == key)
        return ptr;

    return nullptr;
  }

  // removes an element from a chained list
  template <typename Key, typename Val, typename Alloc>
  INLINE void HashTableList<Key, Val, Alloc>::erase(HashTableBucket<Key, Val> *ptr) {
    // check that the pointer is not nullptr
    if (ptr == nullptr) {
      GUM_ERROR(NullElement, "trying to erase a nullptr bucket");
    }

    // relink properly the doubly chained list
    if (ptr->prev != nullptr)
      ptr->prev->next = ptr->next;
    else
      __deb_list = ptr->next;

    if (ptr->next != nullptr)
      ptr->next->prev = ptr->prev;
    else
      __end_list = ptr->prev;

    // remove the current element from the list
    __alloc_bucket->destroy(ptr);
    __alloc_bucket->deallocate(ptr, 1);

    --__nb_elements;
  }

  // basic constructor that creates an empty list
  template <typename Key, typename Val, typename Alloc>
  INLINE HashTableList<Key, Val, Alloc>::HashTableList(
      typename HashTableList<Key, Val, Alloc>::BucketAllocator *allocator) noexcept
      : __alloc_bucket{allocator} {}

  // copy constructor
  template <typename Key, typename Val, typename Alloc>
  INLINE HashTableList<Key, Val, Alloc>::HashTableList(
      const HashTableList<Key, Val, Alloc> &from)
      : __alloc_bucket{from.__alloc_bucket} {
    __copy(from);
  }

  // move constructor
  template <typename Key, typename Val, typename Alloc>
  INLINE HashTableList<Key, Val, Alloc>::HashTableList(
      HashTableList<Key, Val, Alloc> &&from) noexcept
      : __deb_list{from.__deb_list},
        __end_list{from.__end_list},
        __nb_elements{from.__nb_elements},
        __alloc_bucket{from.__alloc_bucket} {
    from.__deb_list = nullptr;
  }

  // destructor
  template <typename Key, typename Val, typename Alloc>
  INLINE HashTableList<Key, Val, Alloc>::~HashTableList() {
    for (Bucket *next_ptr, *ptr = __deb_list; ptr != nullptr; ptr = next_ptr) {
      next_ptr = ptr->next;
      __alloc_bucket->destroy(ptr);
      __alloc_bucket->deallocate(ptr, 1);
    }
  }

  // function for deleting all the elements of a chained list
  template <typename Key, typename Val, typename Alloc>
  INLINE void HashTableList<Key, Val, Alloc>::clear() {
    for (Bucket *next_ptr, *ptr = __deb_list; ptr != nullptr; ptr = next_ptr) {
      next_ptr = ptr->next;
      __alloc_bucket->destroy(ptr);
      __alloc_bucket->deallocate(ptr, 1);
    }

    __nb_elements = 0;
    __deb_list = nullptr;
    __end_list = nullptr;
  }

  // assignment operator
  template <typename Key, typename Val, typename Alloc>
  INLINE HashTableList<Key, Val, Alloc> &HashTableList<Key, Val, Alloc>::
  operator=(const HashTableList<Key, Val, Alloc> &from) {
    // avoid self assignment
    if (this != &from) {
      clear();
      __copy(from);
    }

    return *this;
  }

  // generalized assignment operator
  template <typename Key, typename Val, typename Alloc>
  template <typename OtherAlloc>
  INLINE HashTableList<Key, Val, Alloc> &HashTableList<Key, Val, Alloc>::
  operator=(const HashTableList<Key, Val, OtherAlloc> &from) {
    // avoid self assignment
    if (this != reinterpret_cast<const HashTableList<Key, Val, Alloc> *>(&from)) {
      clear();
      __copy(from);
    }

    return *this;
  }

  // move operator
  template <typename Key, typename Val, typename Alloc>
  INLINE HashTableList<Key, Val, Alloc> &HashTableList<Key, Val, Alloc>::
  operator=(HashTableList<Key, Val, Alloc> &&from) noexcept {
    // avoid self assignment
    if (this != &from) {
      __deb_list = from.__deb_list;
      __end_list = from.__end_list;
      __nb_elements = from.__nb_elements;
      from.__deb_list = nullptr;
    }

    return *this;
  }

  // sets a new allocator
  template <typename Key, typename Val, typename Alloc>
  INLINE void HashTableList<Key, Val, Alloc>::setAllocator(
      typename HashTableList<Key, Val, Alloc>::BucketAllocator &alloc) {
    __alloc_bucket = &alloc;
  }

  // function at returns the ith element in the current chained list
  template <typename Key, typename Val, typename Alloc>
  INLINE typename HashTableList<Key, Val, Alloc>::value_type &
  HashTableList<Key, Val, Alloc>::at(unsigned int i) {
    if (i >= __nb_elements) {
      GUM_ERROR(NotFound, "not enough elements in the chained list");
    }

    Bucket *ptr;

    for (ptr = __deb_list; i; --i, ptr = ptr->next) {
    }

    return ptr->elt();
  }

  // function at returns the ith element in the current chained list
  template <typename Key, typename Val, typename Alloc>
  INLINE const typename HashTableList<Key, Val, Alloc>::value_type &
  HashTableList<Key, Val, Alloc>::at(unsigned int i) const {
    if (i >= __nb_elements) {
      GUM_ERROR(NotFound, "not enough elements in the chained list");
    }

    Bucket *ptr;

    for (ptr = __deb_list; i; --i, ptr = ptr->next) {
    }

    return ptr->elt();
  }

  // operator [] returns the value corresponding to a given key
  template <typename Key, typename Val, typename Alloc>
  INLINE const typename HashTableList<Key, Val, Alloc>::mapped_type &
      HashTableList<Key, Val, Alloc>::
      operator[](const Key &key) const {
    for (Bucket *ptr = __deb_list; ptr != nullptr; ptr = ptr->next)
      if (ptr->key() == key)
        return ptr->val();

    GUM_ERROR(NotFound,
              "hashtable's chained list contains no element with this key");
  }

  // operator [] returns the value corresponding to a given key
  template <typename Key, typename Val, typename Alloc>
  INLINE typename HashTableList<Key, Val, Alloc>::mapped_type &
      HashTableList<Key, Val, Alloc>::
      operator[](const Key &key) {
    for (Bucket *ptr = __deb_list; ptr != nullptr; ptr = ptr->next)
      if (ptr->key() == key)
        return ptr->val();

    GUM_ERROR(NotFound,
              "hashtable's chained list contains no element with this key");
  }

  // check whether there exists an element with a given key in the list
  template <typename Key, typename Val, typename Alloc>
  INLINE bool HashTableList<Key, Val, Alloc>::exists(const Key &key) const {
    for (Bucket *ptr = __deb_list; ptr != nullptr; ptr = ptr->next) {
      if (ptr->key() == key) {
        return true;
      }
    }

    return false;
  }

  // returns a boolean indicating whether the chained list is empty
  template <typename Key, typename Val, typename Alloc>
  INLINE bool HashTableList<Key, Val, Alloc>::empty() const noexcept {
    return (__nb_elements == 0);
  }

  // insertion of a new element in the chained list
  template <typename Key, typename Val, typename Alloc>
  INLINE void HashTableList<Key, Val, Alloc>::insert(
      HashTableBucket<Key, Val> *new_elt) noexcept {
    // place the bucket at the beginning of the list
    new_elt->prev = nullptr;
    new_elt->next = __deb_list;

    if (__deb_list != nullptr)
      __deb_list->prev = new_elt;
    else
      __end_list = new_elt;

    __deb_list = new_elt;

    ++__nb_elements;
  }

  /* =========================================================================== */
  /* ===                   GENERIC HASH TABLE IMPLEMENTATION                 === */
  /* =========================================================================== */

  // a function used to perform copies of HashTables
  template <typename Key, typename Val, typename Alloc>
  template <typename OtherAlloc>
  void
  HashTable<Key, Val, Alloc>::__copy(const HashTable<Key, Val, OtherAlloc> &table) {
    // in debug mode, check that this and table have '__nodes' arrays of the
    // same size
    GUM_ASSERT(table.__size == __size);

    // try to fill the array of chained lists
    for (Size i = 0; i < table.__size; ++i) {
      try {
        __nodes[i] = table.__nodes[i];
      } catch (...) {
        // here we could allocate the __nodes[j], j=0..i-1, so we should
        // deallocate them
        for (Size j = 0; j < __size; ++j)
          __nodes[j].clear();

        __nb_elements = 0;

        // propagate the exception
        throw;
      }
    }

    __nb_elements = table.__nb_elements;
  }

  // returns the end iterator for other classes' statics
  template <typename Key, typename Val, typename Alloc>
  INLINE const typename HashTable<Key, Val, Alloc>::iterator &
  HashTable<Key, Val, Alloc>::end4Statics() {
    return *(reinterpret_cast<const iterator *>(
        HashTableIteratorStaticEnd::end4Statics()));
  }

  // returns the end iterator for other classes' statics
  template <typename Key, typename Val, typename Alloc>
  INLINE const typename HashTable<Key, Val, Alloc>::const_iterator &
  HashTable<Key, Val, Alloc>::constEnd4Statics() {
    return *(reinterpret_cast<const const_iterator *>(
        HashTableIteratorStaticEnd::constEnd4Statics()));
  }

  // returns the safe end iterator for other classes' statics
  template <typename Key, typename Val, typename Alloc>
  INLINE const typename HashTable<Key, Val, Alloc>::iterator_safe &
  HashTable<Key, Val, Alloc>::endSafe4Statics() {
    return *(reinterpret_cast<const iterator_safe *>(
        HashTableIteratorStaticEnd::endSafe4Statics()));
  }

  // returns the end iterator for other classes' statics
  template <typename Key, typename Val, typename Alloc>
  INLINE const typename HashTable<Key, Val, Alloc>::const_iterator_safe &
  HashTable<Key, Val, Alloc>::constEndSafe4Statics() {
    return *(reinterpret_cast<const const_iterator_safe *>(
        HashTableIteratorStaticEnd::constEndSafe4Statics()));
  }

  // Default constructor
  template <typename Key, typename Val, typename Alloc>
  INLINE void HashTable<Key, Val, Alloc>::__create(Size size) {
    // setup the __nodes vector (contains only empty lists)
    __nodes.resize(size);

    for (auto &list : __nodes) {
      list.setAllocator(__alloc);
    }

    // set up properly the hash function
    __hash_func.resize(size);

    // make sure the end() iterator is constructed properly
    end4Statics();
    endSafe4Statics();
  }

  // Default constructor
  template <typename Key, typename Val, typename Alloc>
  HashTable<Key, Val, Alloc>::HashTable(Size size_param, bool resize_pol,
                                        bool key_uniqueness_pol)
      : // size must be >= 2 else we lose all the bits of the hash function
        __size{1UL << __hashTableLog2(std::max(2UL, size_param))},
        __resize_policy{resize_pol}, __key_uniqueness_policy{key_uniqueness_pol} {
    // for debugging purposes
    GUM_CONSTRUCTOR(HashTable);

    // finalize the creation
    __create(__size);
  }

  /// initializer list constructor
  template <typename Key, typename Val, typename Alloc>
  HashTable<Key, Val, Alloc>::HashTable(
      std::initializer_list<std::pair<Key, Val>> list)
      : // size must be >= 2 else we lose all the bits of the hash function
        __size{1UL << __hashTableLog2(std::max<Size>(Size(2), list.size() / 2))} {
    // for debugging purposes
    GUM_CONSTRUCTOR(HashTable);

    // setup the __nodes vector (contains only empty lists)
    __create(__size);

    // insert all the elements
    for (const auto &elt : list) {
      insert(elt);
    }
  }

  // copy constructor
  template <typename Key, typename Val, typename Alloc>
  HashTable<Key, Val, Alloc>::HashTable(const HashTable<Key, Val, Alloc> &table)
      : __size{table.__size}, __resize_policy{table.__resize_policy},
        __key_uniqueness_policy{table.__key_uniqueness_policy},
        __begin_index{table.__begin_index} {
    // for debugging purposes
    GUM_CONS_CPY(HashTable);

    // setup the __nodes vector (contains only empty lists)
    __create(__size);

    // fill with the content of table
    __copy(table);
  }

  // generalized copy constructor
  template <typename Key, typename Val, typename Alloc>
  template <typename OtherAlloc>
  HashTable<Key, Val, Alloc>::HashTable(const HashTable<Key, Val, OtherAlloc> &table)
      : __size{table.__size}, __resize_policy{table.__resize_policy},
        __key_uniqueness_policy{table.__key_uniqueness_policy},
        __begin_index{table.__begin_index} {
    // for debugging purposes
    GUM_CONS_CPY(HashTable);

    // setup the __nodes vector (contains only empty lists)
    __create(__size);

    // fill with the content of table
    __copy(table);
  }

  /// move constructor
  template <typename Key, typename Val, typename Alloc>
  HashTable<Key, Val, Alloc>::HashTable(HashTable<Key, Val, Alloc> &&table)
      : __nodes(std::move(table.__nodes)), __size{table.__size},
        __nb_elements{table.__nb_elements}, __hash_func{table.__hash_func},
        __resize_policy{table.__resize_policy},
        __key_uniqueness_policy{table.__key_uniqueness_policy},
        __begin_index{table.__begin_index},
        __safe_iterators(std::move(table.__safe_iterators)),
        __alloc(std::move(table.__alloc)) {
    // for debugging purposes
    table.__size = 0;
    GUM_CONS_MOV(HashTable);
  }

  /// clear all the safe iterators
  template <typename Key, typename Val, typename Alloc>
  INLINE void HashTable<Key, Val, Alloc>::__clearIterators() {
    for (int i = __safe_iterators.size() - 1; i >= 0; --i) {
      __safe_iterators[i]->clear();
    }
  }

  /// removes all the elements in the hash table
  template <typename Key, typename Val, typename Alloc>
  INLINE void HashTable<Key, Val, Alloc>::clear() {
    // update all the registered iterators: they should now point to nullptr
    // and they are positioned to the end of the hashtable.
    __clearIterators();

    // remove the buckets
    for (Size i = 0; i < __size; ++i)
      __nodes[i].clear();

    __nb_elements = 0;
    __begin_index = std::numeric_limits<Size>::max();
  }

  /// destructor
  template <typename Key, typename Val, typename Alloc>
  INLINE HashTable<Key, Val, Alloc>::~HashTable() {
    // for debugging purposes
    GUM_DESTRUCTOR(HashTable);

    /* update all the registered iterators: they should now point to nullptr
       and their hashtable should be set to nullptr */
    __clearIterators();
  }

  // copy operator
  template <typename Key, typename Val, typename Alloc>
  HashTable<Key, Val, Alloc> &HashTable<Key, Val, Alloc>::
  operator=(const HashTable<Key, Val, Alloc> &from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_CPY(HashTable);

      // first remove the current content of the hashtable and make
      // the iterators point to end
      clear();

      // if sizes of from's and this' __nodes vectors are not the same,
      // we need to remove the current __nodes' array and to create a
      // new array with the correct size
      if (__size != from.__size) {
        __nodes.resize(from.__size);

        for (Size i = 0; i < from.__size; ++i) {
          __nodes[i].setAllocator(__alloc);
        }

        __size = from.__size;

        /* update the hash function : this is important as the computation of
           the hash values heavily depends on the size of the hash table */
        __hash_func.resize(__size);
      }

      __resize_policy = from.__resize_policy;
      __key_uniqueness_policy = from.__key_uniqueness_policy;
      __begin_index = from.__begin_index;

      // perform the copy
      __copy(from);
    }

    return *this;
  }

  // generalized copy operator
  template <typename Key, typename Val, typename Alloc>
  template <typename OtherAlloc>
  HashTable<Key, Val, Alloc> &HashTable<Key, Val, Alloc>::
  operator=(const HashTable<Key, Val, OtherAlloc> &from) {
    // avoid self assignment
    if (this != reinterpret_cast<const HashTable<Key, Val, Alloc> *>(&from)) {
      // for debugging purposes
      GUM_OP_CPY(HashTable);

      // first remove the current content of the hashtable and make
      // the iterators point to end
      clear();

      // if sizes of from's and this' __nodes vectors are not the same,
      // we need to remove the current __nodes' array and to create a
      // new array with the correct size
      if (__size != from.__size) {
        __nodes.resize(from.__size);

        for (Size i = 0; i < from.__size; ++i) {
          __nodes[i].setAllocator(__alloc);
        }

        __size = from.__size;

        /* update the hash function : this is important as the computation of
           the hash values heavily depends on the size of the hash table */
        __hash_func.resize(__size);
      }

      __resize_policy = from.__resize_policy;
      __key_uniqueness_policy = from.__key_uniqueness_policy;
      __begin_index = from.__begin_index;

      // perform the copy
      __copy(from);
    }

    return *this;
  }

  // move operator
  template <typename Key, typename Val, typename Alloc>
  HashTable<Key, Val, Alloc> &HashTable<Key, Val, Alloc>::
  operator=(HashTable<Key, Val, Alloc> &&table) {
    // avoid self assignment
    if (this != &table) {
      // for debugging purposes
      GUM_OP_MOV(HashTable);

      // first remove the current content of the hashtable and make
      // the iterators point to end
      clear();

      __nodes = std::move(table.__nodes);
      __safe_iterators = std::move(table.__safe_iterators);
      __alloc = std::move(table.__alloc);
      __size = table.__size;
      __nb_elements = table.__nb_elements;
      __hash_func = table.__hash_func;
      __resize_policy = table.__resize_policy;
      __key_uniqueness_policy = table.__key_uniqueness_policy;
      __begin_index = table.__begin_index;

      table.__size = 0; // necessary if we wish to perform moves iteratively, i.e.
                        // x = std::move ( y ); y = std::move ( z ); ...
    }

    return *this;
  }

  // the iterator to the end of the hashtable
  template <typename Key, typename Val, typename Alloc>
  INLINE const typename HashTable<Key, Val, Alloc>::iterator &
  HashTable<Key, Val, Alloc>::end() noexcept {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *(reinterpret_cast<const iterator *>(
        HashTableIteratorStaticEnd::__HashTableIterEnd));
  }

  // the iterator to the end of the hashtable
  template <typename Key, typename Val, typename Alloc>
  INLINE const typename HashTable<Key, Val, Alloc>::const_iterator &
  HashTable<Key, Val, Alloc>::end() const noexcept {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *(reinterpret_cast<const const_iterator *>(
        HashTableIteratorStaticEnd::__HashTableIterEnd));
  }

  // the iterator to the end of the hashtable
  template <typename Key, typename Val, typename Alloc>
  INLINE const typename HashTable<Key, Val, Alloc>::const_iterator &
  HashTable<Key, Val, Alloc>::cend() const noexcept {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *(reinterpret_cast<const const_iterator *>(
        HashTableIteratorStaticEnd::__HashTableIterEnd));
  }

  /// the iterator at the beginning of the hashtable
  template <typename Key, typename Val, typename Alloc>
  INLINE typename HashTable<Key, Val, Alloc>::iterator
  HashTable<Key, Val, Alloc>::begin() {
    // if the table is empty, make the begin and end point to the same element
    if (__nb_elements == 0)
      return iterator{end()};
    else
      return iterator{*this};
  }

  /// the iterator at the beginning of the hashtable
  template <typename Key, typename Val, typename Alloc>
  INLINE typename HashTable<Key, Val, Alloc>::const_iterator
  HashTable<Key, Val, Alloc>::begin() const {
    // if the table is empty, make the begin and end point to the same element
    if (__nb_elements == 0)
      return const_iterator{end()};
    else
      return const_iterator{*this};
  }

  /// the iterator at the beginning of the hashtable
  template <typename Key, typename Val, typename Alloc>
  INLINE typename HashTable<Key, Val, Alloc>::const_iterator
  HashTable<Key, Val, Alloc>::cbegin() const {
    // if the table is empty, make the begin and end point to the same element
    if (__nb_elements == 0)
      return const_iterator{cend()};
    else
      return const_iterator{*this};
  }

  // the safe iterator to the end of the hashtable
  template <typename Key, typename Val, typename Alloc>
  INLINE const typename HashTable<Key, Val, Alloc>::iterator_safe &
  HashTable<Key, Val, Alloc>::endSafe() noexcept {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *(reinterpret_cast<const iterator_safe *>(
        HashTableIteratorStaticEnd::__HashTableIterEndSafe));
  }

  // the iterator to the end of the hashtable
  template <typename Key, typename Val, typename Alloc>
  INLINE const typename HashTable<Key, Val, Alloc>::const_iterator_safe &
  HashTable<Key, Val, Alloc>::endSafe() const noexcept {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *(reinterpret_cast<const const_iterator_safe *>(
        HashTableIteratorStaticEnd::__HashTableIterEndSafe));
  }

  // the iterator to the end of the hashtable
  template <typename Key, typename Val, typename Alloc>
  INLINE const typename HashTable<Key, Val, Alloc>::const_iterator_safe &
  HashTable<Key, Val, Alloc>::cendSafe() const noexcept {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *(reinterpret_cast<const const_iterator_safe *>(
        HashTableIteratorStaticEnd::__HashTableIterEndSafe));
  }

  /// the iterator at the beginning of the hashtable
  template <typename Key, typename Val, typename Alloc>
  INLINE typename HashTable<Key, Val, Alloc>::iterator_safe
  HashTable<Key, Val, Alloc>::beginSafe() {
    // if the table is empty, make the begin and end point to the same element
    if (__nb_elements == 0)
      return iterator_safe{endSafe()};
    else
      return iterator_safe{*this};
  }

  /// the iterator at the beginning of the hashtable
  template <typename Key, typename Val, typename Alloc>
  INLINE typename HashTable<Key, Val, Alloc>::const_iterator_safe
  HashTable<Key, Val, Alloc>::beginSafe() const {
    // if the table is empty, make the begin and end point to the same element
    if (__nb_elements == 0)
      return const_iterator_safe{endSafe()};
    else
      return const_iterator_safe{*this};
  }

  /// the iterator at the beginning of the hashtable
  template <typename Key, typename Val, typename Alloc>
  INLINE typename HashTable<Key, Val, Alloc>::const_iterator_safe
  HashTable<Key, Val, Alloc>::cbeginSafe() const {
    // if the table is empty, make the begin and end point to the same element
    if (__nb_elements == 0)
      return const_iterator_safe{cendSafe()};
    else
      return const_iterator_safe{*this};
  }

  /// returns a reference on the value the key of which is passed in argument
  template <typename Key, typename Val, typename Alloc>
  INLINE Val &HashTable<Key, Val, Alloc>::operator[](const Key &key) {
    return __nodes[__hash_func(key)][key];
  }

  /// returns a reference on the value the key of which is passed in argument
  template <typename Key, typename Val, typename Alloc>
  INLINE const Val &HashTable<Key, Val, Alloc>::operator[](const Key &key) const {
    return __nodes[__hash_func(key)][key];
  }

  /// returns the number of elements in the hashtable
  template <typename Key, typename Val, typename Alloc>
  INLINE Size HashTable<Key, Val, Alloc>::size() const noexcept {
    return __nb_elements;
  }

  /// returns the size of the __nodes vector of the hashtable
  template <typename Key, typename Val, typename Alloc>
  INLINE Size HashTable<Key, Val, Alloc>::capacity() const noexcept {
    return __size;
  }

  /// checks whether there exists an element with a given key in the hashtable
  template <typename Key, typename Val, typename Alloc>
  INLINE bool HashTable<Key, Val, Alloc>::exists(const Key &key) const {
    return __nodes[__hash_func(key)].exists(key);
  }

  /// enables the user to change dynamically the resizing policy.
  template <typename Key, typename Val, typename Alloc>
  INLINE void
  HashTable<Key, Val, Alloc>::setResizePolicy(const bool new_policy) noexcept {
    __resize_policy = new_policy;
  }

  /// returns the current resizing policy.
  template <typename Key, typename Val, typename Alloc>
  INLINE bool HashTable<Key, Val, Alloc>::resizePolicy() const noexcept {
    return __resize_policy;
  }

  /** @brief enables the user to change dynamically the policy for checking
   * whether there can exist several elements in the table having identical keys */
  template <typename Key, typename Val, typename Alloc>
  INLINE void HashTable<Key, Val, Alloc>::setKeyUniquenessPolicy(
      const bool new_policy) noexcept {
    __key_uniqueness_policy = new_policy;
  }

  /// returns the current checking policy
  template <typename Key, typename Val, typename Alloc>
  INLINE bool HashTable<Key, Val, Alloc>::keyUniquenessPolicy() const noexcept {
    return __key_uniqueness_policy;
  }

  /// changes the size of the __nodes vector of the hash table
  template <typename Key, typename Val, typename Alloc>
  void HashTable<Key, Val, Alloc>::resize(Size new_size) {
    // new_size must be >= 2 else all the bits of the hash function are lost
    new_size = std::max(2UL, new_size);

    // find the real size for allocation (the smallest power of 2 greater
    // than or equal to new_size) and get its base-2 logarithm
    int log_size = __hashTableLog2(new_size);
    new_size = 1UL << log_size;

    // check if the new size is different from the actual size
    // if not, nothing else need be done

    if (new_size != __size) {
      // under automatic resize policy, check if the new size leaves
      // enough space for storing all the current elements
      if (!__resize_policy ||
          (__nb_elements <= new_size * HashTableConst::default_mean_val_by_slot)) {
        // create a new array of __nodes to store the elements
        std::vector<HashTableList<Key, Val, Alloc>> new_nodes(new_size);

        for (auto &list : new_nodes) {
          list.setAllocator(__alloc);
        }

        // set the new hash function
        __hash_func.resize(new_size);

        // put all the elements of the current __nodes array into the new one
        Bucket *bucket;
        Size new_hashed_key;

        for (Size i = 0; i < __size; ++i) {
          while ((bucket = __nodes[i].__deb_list) != nullptr) {
            // compute the new hashed key
            new_hashed_key = __hash_func(bucket->key());

            // remove the bucket from the list of buckets of the current
            // node vector
            __nodes[i].__deb_list = bucket->next;

            // put the bucket into the new __nodes vector
            new_nodes[new_hashed_key].insert(bucket);
          }
        }

        // update the size of the hash table
        __size = new_size;
        __begin_index = std::numeric_limits<Size>::max();

        // substitute the current __nodes array by the new one
        std::swap(__nodes, new_nodes);

        // update the iterators
        for (auto iter : __safe_iterators) {
          if (iter->__bucket)
            iter->__index = __hash_func(iter->__bucket->key());
          else {
            iter->__next_bucket = nullptr;
            iter->__index = 0;
          }
        }
      }
    }
  }

  /// add a new element (actually a copy of this element) in the hash table
  template <typename Key, typename Val, typename Alloc>
  void HashTable<Key, Val, Alloc>::__insert(HashTableBucket<Key, Val> *bucket) {
    Size hash_key = __hash_func(bucket->key());

    // check that there does not already exist an element with the same key
    if (__key_uniqueness_policy && __nodes[hash_key].exists(bucket->key())) {
      // remove the bucket from memory
      __alloc.destroy(bucket);
      __alloc.deallocate(bucket, 1);
      GUM_ERROR(DuplicateElement,
                "the hashtable contains an element with the same key");
    }

    // check whether there is sufficient space to insert the new pair
    // if not, resize the current hashtable
    if (__resize_policy &&
        (__nb_elements >= __size * HashTableConst::default_mean_val_by_slot)) {
      resize(__size << 1);
      hash_key = __hash_func(bucket->key());
    }

    // add the new pair
    __nodes[hash_key].insert(bucket);
    ++__nb_elements;

    // recompute the index of the beginning of the hashtable if possible
    // WARNING: if __begin_index = std::numeric_limits<Size>::max (), we CANNOT
    // recompute the index because we cannot know whether the current index is
    // equal to max because there was no element in the hashtable or whether a
    // previous __erase() has set the index to max.
    if (__begin_index < hash_key) {
      __begin_index = hash_key;
    }
  }

  /// add a new element (actually a copy of this element) in the hash table
  template <typename Key, typename Val, typename Alloc>
  INLINE typename HashTable<Key, Val, Alloc>::value_type &
  HashTable<Key, Val, Alloc>::insert(const Key &thekey, const Val &theval) {
    Bucket *bucket = __alloc.allocate(1);

    try {
      __alloc.construct(bucket, thekey, theval);
    } catch (...) {
      __alloc.deallocate(bucket, 1);
      throw;
    }

    __insert(bucket);
    return bucket->elt();
  }

  /// moves a new element in the hash table
  template <typename Key, typename Val, typename Alloc>
  INLINE typename HashTable<Key, Val, Alloc>::value_type &
  HashTable<Key, Val, Alloc>::insert(Key &&thekey, Val &&theval) {
    Bucket *bucket = __alloc.allocate(1);

    try {
      __alloc.construct(bucket, std::move(thekey), std::move(theval));
    } catch (...) {
      __alloc.deallocate(bucket, 1);
      throw;
    }

    __insert(bucket);
    return bucket->elt();
  }

  /// add a new element (actually a copy of this element) in the hash table
  template <typename Key, typename Val, typename Alloc>
  INLINE typename HashTable<Key, Val, Alloc>::value_type &
  HashTable<Key, Val, Alloc>::insert(const std::pair<Key, Val> &elt) {
    Bucket *bucket = __alloc.allocate(1);

    try {
      __alloc.construct(bucket, reinterpret_cast<const value_type &>(elt));
    } catch (...) {
      __alloc.deallocate(bucket, 1);
      throw;
    }

    __insert(bucket);
    return bucket->elt();
  }

  /// moves a new element in the hash table
  template <typename Key, typename Val, typename Alloc>
  INLINE typename HashTable<Key, Val, Alloc>::value_type &
  HashTable<Key, Val, Alloc>::insert(std::pair<Key, Val> &&elt) {
    Bucket *bucket = __alloc.allocate(1);

    try {
      __alloc.construct(bucket, std::move(reinterpret_cast<value_type &>(elt)));
    } catch (...) {
      __alloc.deallocate(bucket, 1);
      throw;
    }

    __insert(bucket);
    return bucket->elt();
  }

  /// emplace a new element into the hashTable
  template <typename Key, typename Val, typename Alloc>
  template <typename... Args>
  INLINE typename HashTable<Key, Val, Alloc>::value_type &
  HashTable<Key, Val, Alloc>::emplace(Args &&... args) {
    Bucket *bucket = __alloc.allocate(1);

    try {
      __alloc.construct(bucket, HashTableBucket<Key, Val>::Emplace::EMPLACE,
                        std::forward<Args>(args)...);
    } catch (...) {
      __alloc.deallocate(bucket, 1);
      throw;
    }

    __insert(bucket);
    return bucket->elt();
  }

  /// returns a reference on the value the key of which is passed in argument
  template <typename Key, typename Val, typename Alloc>
  INLINE typename HashTable<Key, Val, Alloc>::mapped_type &
  HashTable<Key, Val, Alloc>::getWithDefault(const Key &key,
                                             const Val &default_value) {
    Bucket *bucket = __nodes[__hash_func(key)].bucket(key);

    if (bucket == nullptr)
      return insert(key, default_value).second;
    else
      return bucket->val();
  }

  /// returns a reference on the value the key of which is passed in argument
  template <typename Key, typename Val, typename Alloc>
  INLINE typename HashTable<Key, Val, Alloc>::mapped_type &
  HashTable<Key, Val, Alloc>::getWithDefault(Key &&key, Val &&default_value) {
    Bucket *bucket = __nodes[__hash_func(key)].bucket(key);

    if (bucket == nullptr)
      return insert(std::move(key), std::move(default_value)).second;
    else
      return bucket->val();
  }

  /// add or update a couple (key,value)
  template <typename Key, typename Val, typename Alloc>
  INLINE void HashTable<Key, Val, Alloc>::set(const Key &key, const Val &value) {
    Bucket *bucket = __nodes[__hash_func(key)].bucket(key);

    if (bucket == nullptr)
      insert(key, value);
    else
      bucket->val() = value;
  }

  /// removes a given element from the hash table
  template <typename Key, typename Val, typename Alloc>
  void HashTable<Key, Val, Alloc>::__erase(HashTableBucket<Key, Val> *bucket,
                                           Size index) {
    if (bucket == nullptr)
      return;

    // update the registered iterators pointing to this bucket
    for (auto iter : __safe_iterators) {
      if (iter->__bucket == bucket) {
        iter->operator++();
        iter->__next_bucket = iter->__bucket;
        iter->__bucket = nullptr;
      } else if (iter->__next_bucket == bucket) {
        iter->__bucket = bucket;
        iter->operator++();
        iter->__next_bucket = iter->__bucket;
        iter->__bucket = nullptr;
      }
    }

    // remove the element from the __nodes vector
    __nodes[index].erase(bucket);

    --__nb_elements;

    if ((index == __begin_index) && __nodes[index].empty()) {
      __begin_index = std::numeric_limits<Size>::max();
    }
  }

  /// removes a given element from the hash table
  template <typename Key, typename Val, typename Alloc>
  INLINE void HashTable<Key, Val, Alloc>::erase(const Key &key) {
    // get the hashed key
    Size hash = __hash_func(key);

    // get the bucket containing the element to erase
    HashTableBucket<Key, Val> *bucket = __nodes[hash].bucket(key);

    __erase(bucket, hash);
  }

  /// removes a given element from the hash table
  template <typename Key, typename Val, typename Alloc>
  INLINE void HashTable<Key, Val, Alloc>::erase(const iterator_safe &iter) {
    __erase(iter.__getBucket(), iter.__getIndex());
  }

  /// removes a given element from the hash table
  template <typename Key, typename Val, typename Alloc>
  INLINE void HashTable<Key, Val, Alloc>::erase(const const_iterator_safe &iter) {
    __erase(iter.__getBucket(), iter.__getIndex());
  }

  /// removes a given element from the hash table
  template <typename Key, typename Val, typename Alloc>
  INLINE void HashTable<Key, Val, Alloc>::eraseByVal(const Val &val) {
    for (auto iter = cbegin(); iter != cend(); ++iter)
      if (iter.__bucket->val() == val) {
        __erase(iter.__getBucket(), iter.__getIndex());
        return;
      }
  }

  /// remove a (key,...) if exists
  template <typename Key, typename Val, typename Alloc>
  INLINE void HashTable<Key, Val, Alloc>::reset(const Key &key) {
    erase(key);
  }

  /// return the (first) key of a given value
  template <typename Key, typename Val, typename Alloc>
  INLINE const Key &HashTable<Key, Val, Alloc>::keyByVal(const Val &val) const {
    for (auto iter = begin(); iter != end(); ++iter)
      if (iter.__bucket->val() == val)
        return iter.key();

    GUM_ERROR(NotFound, "not enough elements in the chained list");
  }

  /// returns a reference on a given key
  template <typename Key, typename Val, typename Alloc>
  INLINE const Key &HashTable<Key, Val, Alloc>::key(const Key &key) const {
    // get the bucket corresponding to the key
    Bucket *bucket = __nodes[__hash_func(key)].bucket(key);

    if (bucket == nullptr) {
      GUM_ERROR(NotFound, "key does not belong to the hashtable");
    }

    return bucket->key();
  }

  /// removes all the elements having a certain value from the hash table
  template <typename Key, typename Val, typename Alloc>
  void HashTable<Key, Val, Alloc>::eraseAllVal(const Val &val) {
    for (auto iterAll = cbeginSafe(); iterAll != cendSafe(); ++iterAll) {
      if (iterAll.__bucket->val() == val) {
        __erase(iterAll.__bucket, iterAll.__index);
      }
    }
  }

  /// indicates whether the hash table is empty
  template <typename Key, typename Val, typename Alloc>
  INLINE bool HashTable<Key, Val, Alloc>::empty() const noexcept {
    return (__nb_elements == 0);
  }

  /// transforms a hashtable of val into a hashtable of mountains
  template <typename Key, typename Val, typename Alloc>
  template <typename Mount, typename OtherAlloc>
  HashTable<Key, Mount, OtherAlloc> INLINE
      HashTable<Key, Val, Alloc>::map(Mount (*f)(Val), Size size, bool resize_pol,
                                      bool key_uniqueness_pol) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without needing to
    // resize in autmatic resizing mode
    if (size == 0)
      size = std::max(2UL, __nb_elements / 2);

    // create a new table
    HashTable<Key, Mount, OtherAlloc> table(size, resize_pol, key_uniqueness_pol);

    // fill the new hash table
    for (auto iter = begin(); iter != end(); ++iter) {
      table.insert(iter.key(), f(iter.val()));
    }

    return table;
  }

  /// transforms a hashtable of val into a hashtable of mountains
  template <typename Key, typename Val, typename Alloc>
  template <typename Mount, typename OtherAlloc>
  HashTable<Key, Mount, OtherAlloc> INLINE
      HashTable<Key, Val, Alloc>::map(Mount (*f)(Val &), Size size, bool resize_pol,
                                      bool key_uniqueness_pol) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without needing to
    // resize in autmatic resizing mode
    if (size == 0)
      size = std::max(2UL, __nb_elements / 2);

    // create a new table
    HashTable<Key, Mount, OtherAlloc> table(size, resize_pol, key_uniqueness_pol);

    // fill the new hash table
    for (auto iter = begin(); iter != end(); ++iter) {
      table.insert(iter.key(), f(const_cast<Val &>(iter.val())));
    }

    return table;
  }

  /// transforms a hashtable of val into a hashtable of mountains
  template <typename Key, typename Val, typename Alloc>
  template <typename Mount, typename OtherAlloc>
  HashTable<Key, Mount, OtherAlloc> INLINE
      HashTable<Key, Val, Alloc>::map(Mount (*f)(const Val &), Size size,
                                      bool resize_pol,
                                      bool key_uniqueness_pol) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without needing to
    // resize in autmatic resizing mode
    if (size == 0)
      size = std::max(2UL, __nb_elements / 2);

    // create a new table
    HashTable<Key, Mount, OtherAlloc> table(size, resize_pol, key_uniqueness_pol);

    // fill the new hash table
    for (auto iter = begin(); iter != end(); ++iter) {
      table.insert(iter.key(), f(iter.val()));
    }

    return table;
  }

  /// creates a hashtable of Val with a given value from a hashtable of Mount
  template <typename Key, typename Val, typename Alloc>
  template <typename Mount, typename OtherAlloc>
  HashTable<Key, Mount, OtherAlloc> INLINE
      HashTable<Key, Val, Alloc>::map(const Mount &val, Size size, bool resize_pol,
                                      bool key_uniqueness_pol) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without needing to
    // resize in autmatic resizing mode
    if (size == 0)
      size = std::max(2UL, __nb_elements / 2);

    // create a new table
    HashTable<Key, Mount, OtherAlloc> table(size, resize_pol, key_uniqueness_pol);

    // fill the new hash table
    for (auto iter = begin(); iter != end(); ++iter) {
      table.insert(iter.key(), val);
    }

    return table;
  }

  /// checks whether two hashtables contain the same elements
  template <typename Key, typename Val, typename Alloc>
  template <typename OtherAlloc>
  bool HashTable<Key, Val, Alloc>::
  operator==(const HashTable<Key, Val, OtherAlloc> &from) const {
    // checks whether the two hashtables contain the same number of elements
    if (from.__nb_elements != __nb_elements)
      return false;

    // parse this and check that each element also belongs to from
    for (auto iter = begin(); iter != end(); ++iter) {
      try {
        if (iter.val() != from[iter.key()])
          return false;
      } catch (NotFound &) {
        return false;
      }
    }

    return true;
  }

  /// checks whether two hashtables contain different sets of elements
  template <typename Key, typename Val, typename Alloc>
  template <typename OtherAlloc>
  INLINE bool HashTable<Key, Val, Alloc>::
  operator!=(const HashTable<Key, Val, OtherAlloc> &from) const {
    return !operator==(from);
  }

  // a << operator for HashTableList
  template <typename Key, typename Val, typename Alloc>
  std::ostream &operator<<(std::ostream &stream,
                           const HashTableList<Key, Val, Alloc> &list) {
    bool deja = false;
    stream << "[";

    for (HashTableBucket<Key, Val> *ptr = list.__deb_list; ptr;
         ptr = ptr->list.next, deja = true) {
      if (deja)
        stream << " , ";

      stream << ptr->key() << "=>" << ptr->val();
    }

    stream << "]";

    return stream;
  }

  // a << operator for HashTableList
  template <typename Key, typename Val, typename Alloc>
  std::ostream &operator<<(std::ostream &stream,
                           const HashTableList<Key *, Val, Alloc> &list) {
    bool deja = false;
    stream << "[";

    for (HashTableBucket<Key, Val> *ptr = list.__deb_list; ptr;
         ptr = ptr->list.next, deja = true) {
      if (deja)
        stream << " , ";

      stream << ptr->key() << "=>" << ptr->val();
    }

    stream << "]";

    return stream;
  }

  /// a \c << operator for HashTable
  template <typename Key, typename Val, typename Alloc>
  std::ostream &operator<<(std::ostream &stream,
                           const HashTable<Key, Val, Alloc> &table) {
    bool deja = false;
    stream << "{";

    for (Size i = 0; i < table.__size; ++i)
      for (auto ptr = table.__nodes[i].__deb_list; ptr; ptr = ptr->next) {
        if (deja)
          stream << " , ";

        stream << ptr->key() << "=>" << ptr->val();

        deja = true;
      }

    stream << "}";

    return stream;
  }

  /// a \c << operator for HashTable
  template <typename Key, typename Val, typename Alloc>
  std::ostream &operator<<(std::ostream &stream,
                           const HashTable<Key *, Val, Alloc> &table) {
    bool deja = false;
    stream << "{";

    for (Size i = 0; i < table.__size; ++i)
      for (auto ptr = table.__nodes[i].__deb_list; ptr; ptr = ptr->next) {
        if (deja)
          stream << " , ";

        stream << ptr->key() << "=>" << ptr->val();

        deja = true;
      }

    stream << "}";

    return stream;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
