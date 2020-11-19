/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Implementation of the Set.
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/tools/core/set.h>

namespace gum {

  // ===========================================================================
  // ===                          SAFE SET ITERATORS                         ===
  // ===========================================================================

  // default constructor: the iterator points toward nothing
  template < typename Key >
  INLINE SetIteratorSafe< Key >::SetIteratorSafe() {
    GUM_CONSTRUCTOR(SetIteratorSafe);
  }

  // creates an iterator for a given set
  template < typename Key >
  template < typename Alloc >
  INLINE SetIteratorSafe< Key >::SetIteratorSafe(const Set< Key, Alloc >& set,
                                                 Position                 pos) :
      ht_iter__{pos == SetIteratorSafe< Key >::END ? set.inside__.cendSafe()
                                                   : set.inside__.cbeginSafe()} {
    GUM_CONSTRUCTOR(SetIteratorSafe);
  }

  // copy constructor
  template < typename Key >
  INLINE
     SetIteratorSafe< Key >::SetIteratorSafe(const SetIteratorSafe< Key >& iter) :
      ht_iter__{iter.ht_iter__} {
    GUM_CONS_CPY(SetIteratorSafe);
  }

  // copy constructor
  template < typename Key >
  INLINE SetIteratorSafe< Key >::SetIteratorSafe(const SetIterator< Key >& iter) :
      ht_iter__{iter.ht_iter__} {
    GUM_CONS_CPY(SetIteratorSafe);
  }

  // move constructor
  template < typename Key >
  INLINE SetIteratorSafe< Key >::SetIteratorSafe(SetIteratorSafe< Key >&& from) :
      ht_iter__{std::move(from.ht_iter__)} {
    GUM_CONS_MOV(SetIteratorSafe);
  }

  // destructor
  template < typename Key >
  INLINE SetIteratorSafe< Key >::~SetIteratorSafe() noexcept {
    GUM_DESTRUCTOR(SetIteratorSafe);
  }

  // assignment operator
  template < typename Key >
  INLINE SetIteratorSafe< Key >&
     SetIteratorSafe< Key >::operator=(const SetIteratorSafe< Key >& from) {
    ht_iter__ = from.ht_iter__;
    return *this;
  }

  // assignment operator
  template < typename Key >
  INLINE SetIteratorSafe< Key >&
     SetIteratorSafe< Key >::operator=(const SetIterator< Key >& from) {
    ht_iter__ = from.ht_iter__;
    return *this;
  }

  // move operator
  template < typename Key >
  INLINE SetIteratorSafe< Key >&
     SetIteratorSafe< Key >::operator=(SetIteratorSafe< Key >&& from) noexcept {
    ht_iter__ = std::move(from.ht_iter__);
    return *this;
  }

  // increments the iterator
  template < typename Key >
  INLINE SetIteratorSafe< Key >& SetIteratorSafe< Key >::operator++() noexcept {
    // note that, if the hashtable's iterator points toward nothing, the
    // hashtable's iterator incrementation will do nothing. In particular, it
    // will not segfault.
    ++ht_iter__;
    return *this;
  }

  // makes the iterator point to i elements further in the set
  template < typename Key >
  INLINE SetIteratorSafe< Key >&
     SetIteratorSafe< Key >::operator+=(Size nb) noexcept {
    ht_iter__ += nb;
    return *this;
  }

  // returns a new iterator
  template < typename Key >
  INLINE SetIteratorSafe< Key > SetIteratorSafe< Key >::operator+(Size nb) const {
    return SetIteratorSafe< Key >{*this} += nb;
  }

  // indicates whether two iterators point to different elements or sets
  template < typename Key >
  INLINE bool SetIteratorSafe< Key >::operator!=(
     const SetIteratorSafe< Key >& from) const noexcept {
    return ht_iter__ != from.ht_iter__;
  }

  // indicates whether two iterators point toward the same element of a same
  // set
  template < typename Key >
  INLINE bool SetIteratorSafe< Key >::operator==(
     const SetIteratorSafe< Key >& from) const noexcept {
    return ht_iter__ == from.ht_iter__;
  }

  // returns the element pointed to by the iterator
  template < typename Key >
  INLINE const Key& SetIteratorSafe< Key >::operator*() const {
    // note that, if the hashtable's iterator points toward nothing, it will
    // raise an UndefinedIteratorValue exception
    return ht_iter__.key();
  }

  // returns aointer to the element pointed to by the iterator
  template < typename Key >
  INLINE const Key* SetIteratorSafe< Key >::operator->() const {
    // note that, if the hashtable's iterator points toward nothing, it will
    // raise an UndefinedIteratorValue exception
    return &(ht_iter__.key());
  }

  // @brief makes the iterator point toward nothing (in particular, it is not
  // related anymore to its current set) */
  template < typename Key >
  INLINE void SetIteratorSafe< Key >::clear() noexcept {
    ht_iter__.clear();
  }

  // ===========================================================================
  // ===                         UNSAFE SET ITERATORS                        ===
  // ===========================================================================

  // default constructor: the iterator points toward nothing
  template < typename Key >
  INLINE SetIterator< Key >::SetIterator() noexcept {
    GUM_CONSTRUCTOR(SetIterator);
  }

  // creates an iterator for a given set
  template < typename Key >
  template < typename Alloc >
  INLINE SetIterator< Key >::SetIterator(const Set< Key, Alloc >& set,
                                         Position                 pos) :
      ht_iter__{pos == SetIterator< Key >::END ? set.inside__.cend()
                                               : set.inside__.cbegin()} {
    GUM_CONSTRUCTOR(SetIterator);
  }

  // copy constructor
  template < typename Key >
  INLINE SetIterator< Key >::SetIterator(const SetIterator< Key >& iter) noexcept :
      ht_iter__{iter.ht_iter__} {
    GUM_CONS_CPY(SetIterator);
  }

  // move constructor
  template < typename Key >
  INLINE SetIterator< Key >::SetIterator(SetIterator< Key >&& from) noexcept :
      ht_iter__{std::move(from.ht_iter__)} {
    GUM_CONS_MOV(SetIterator);
  }

  // destructor
  template < typename Key >
  INLINE SetIterator< Key >::~SetIterator() noexcept {
    GUM_DESTRUCTOR(SetIterator);
  }

  // assignment operator
  template < typename Key >
  INLINE SetIterator< Key >&
     SetIterator< Key >::operator=(const SetIterator< Key >& from) noexcept {
    ht_iter__ = from.ht_iter__;
    return *this;
  }

  // move operator
  template < typename Key >
  INLINE SetIterator< Key >&
     SetIterator< Key >::operator=(SetIterator< Key >&& from) noexcept {
    ht_iter__ = std::move(from.ht_iter__);
    return *this;
  }

  // increments the iterator
  template < typename Key >
  INLINE SetIterator< Key >& SetIterator< Key >::operator++() noexcept {
    // note that, if the hashtable's iterator points toward nothing, the
    // hashtable's iterator incrementation will do nothing. In particular, it
    // will not segfault.
    ++ht_iter__;
    return *this;
  }

  // makes the iterator point to i elements further in the set
  template < typename Key >
  INLINE SetIterator< Key >& SetIterator< Key >::operator+=(Size nb) noexcept {
    ht_iter__ += nb;
    return *this;
  }

  // returns a new iterator
  template < typename Key >
  INLINE SetIterator< Key > SetIterator< Key >::operator+(Size nb) const noexcept {
    return SetIterator< Key >{*this} += nb;
  }

  // indicates whether two iterators point to different elements or sets
  template < typename Key >
  INLINE bool SetIterator< Key >::operator!=(
     const SetIterator< Key >& from) const noexcept {
    return ht_iter__ != from.ht_iter__;
  }

  // indicates whether two iterators point toward the same element of a same
  // set
  template < typename Key >
  INLINE bool SetIterator< Key >::operator==(
     const SetIterator< Key >& from) const noexcept {
    return ht_iter__ == from.ht_iter__;
  }

  // returns the element pointed to by the iterator
  template < typename Key >
  INLINE const Key& SetIterator< Key >::operator*() const {
    // note that, if the hashtable's iterator points toward nothing, it will
    // raise an UndefinedIteratorValue exception
    return ht_iter__.key();
  }

  // returns aointer to the element pointed to by the iterator
  template < typename Key >
  INLINE const Key* SetIterator< Key >::operator->() const {
    // note that, if the hashtable's iterator points toward nothing, it will
    // raise an UndefinedIteratorValue exception
    return &(ht_iter__.key());
  }

  // @brief makes the iterator point toward nothing (in particular, it is not
  // related anymore to its current set) */
  template < typename Key >
  INLINE void SetIterator< Key >::clear() noexcept {
    ht_iter__.clear();
  }

  // ===========================================================================
  // ===                                 SETS                                ===
  // ===========================================================================

  // returns the end iterator for other classes' statics
  template < typename Key, typename Alloc >
  INLINE const SetIteratorSafe< Key >& Set< Key, Alloc >::endSafe4Statics() {
    return *(reinterpret_cast< const SetIteratorSafe< Key >* >(
       SetIteratorStaticEnd::endSafe4Statics()));
  }

  // returns the end iterator for other classes' statics
  template < typename Key, typename Alloc >
  INLINE const SetIteratorSafe< Key >& Set< Key, Alloc >::constEndSafe4Statics() {
    return *(reinterpret_cast< const SetIteratorSafe< Key >* >(
       SetIteratorStaticEnd::constEndSafe4Statics()));
  }

  // returns the end iterator for other classes' statics
  template < typename Key, typename Alloc >
  INLINE const SetIterator< Key >& Set< Key, Alloc >::end4Statics() {
    return *(reinterpret_cast< const SetIterator< Key >* >(
       SetIteratorStaticEnd::end4Statics()));
  }

  // returns the end iterator for other classes' statics
  template < typename Key, typename Alloc >
  INLINE const SetIterator< Key >& Set< Key, Alloc >::constEnd4Statics() {
    return *(reinterpret_cast< const SetIterator< Key >* >(
       SetIteratorStaticEnd::constEnd4Statics()));
  }

  // default constructor
  template < typename Key, typename Alloc >
  INLINE Set< Key, Alloc >::Set(Size capacity, bool resize_policy) :
      // create the hash table without key uniqueness policy (as we will
      // check
      // ourselves the uniqueness of Keys before inserting new elements)
      inside__(capacity, resize_policy, false) {
    GUM_CONSTRUCTOR(Set);

    // make sure the end() iterator is constructed properly
    endSafe4Statics();
    end4Statics();
  }

  // initializer list constructor
  template < typename Key, typename Alloc >
  INLINE Set< Key, Alloc >::Set(std::initializer_list< Key > list) :
      inside__(Size(list.size()) / 2, true, false) {
    GUM_CONSTRUCTOR(Set);
    for (const auto& elt: list) {
      insert(elt);
    }

    // make sure the end() iterator is constructed properly
    endSafe4Statics();
    end4Statics();
  }

  // copy constructor
  template < typename Key, typename Alloc >
  INLINE Set< Key, Alloc >::Set(const Set< Key, Alloc >& s) :
      inside__(s.inside__) {
    GUM_CONS_CPY(Set);
  }

  // generalized copy constructor
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  INLINE Set< Key, Alloc >::Set(const Set< Key, OtherAlloc >& s) :
      inside__(s.inside__) {
    GUM_CONS_CPY(Set);
  }

  // move constructor
  template < typename Key, typename Alloc >
  INLINE Set< Key, Alloc >::Set(Set< Key, Alloc >&& s) :
      inside__(std::move(s.inside__)) {
    GUM_CONS_MOV(Set);
  }

  // destructor
  template < typename Key, typename Alloc >
  INLINE Set< Key, Alloc >::Set::~Set() {
    GUM_DESTRUCTOR(Set);
  }

  // removes all the elements, if any, from the set
  template < typename Key, typename Alloc >
  INLINE void Set< Key, Alloc >::clear() {
    // first we remove all the elements from the hashtable actually containing
    // the elements of the set. Note that, doing so, all the hashtable iterators
    // will be updated as well. In turn, this will imply that, whenever an
    // operation will be performed on a SetIteratorSafe, this will raise an
    // exception.
    inside__.clear();

    // Note that actually there is no need to update the end iterator as this
    // one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }

  // copy operator
  template < typename Key, typename Alloc >
  Set< Key, Alloc >& Set< Key, Alloc >::operator=(const Set< Key, Alloc >& s) {
    // avoid self assignment
    if (&s != this) {
      // remove the old content of the set. Actually, we remove all the elements
      // from the underlying hashtable. Note that, doing so, all the hashtable
      // iterators will be updated as well. In turn, this will imply that,
      // whenever
      // an operation will be performed on a SetIteratorSafe, this will raise an
      // exception.
      clear();

      // prepare the set for its new data
      resize(s.capacity());
      setResizePolicy(s.resizePolicy());

      // copy the set
      inside__ = s.inside__;

      // Note that actually there is no need to update the end iterator as this
      // one
      // is not affected by changes within hashtables (adding/deleting
      // elements).
      // Hence, for speedup, we do not update the end iterator
    }

    return *this;
  }

  // generalized copy operator
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  Set< Key, Alloc >&
     Set< Key, Alloc >::operator=(const Set< Key, OtherAlloc >& s) {
    // avoid self assignment
    if (this != reinterpret_cast< const Set< Key, Alloc >* >(&s)) {
      // remove the old content of the set. Actually, we remove all the elements
      // from the underlying hashtable. Note that, doing so, all the hashtable
      // iterators will be updated as well. In turn, this will imply that,
      // whenever
      // an operation will be performed on a SetIteratorSafe, this will raise an
      // exception.
      clear();

      // prepare the set for its new data
      resize(s.capacity());
      setResizePolicy(s.resizePolicy());

      // copy the set
      inside__ = s.inside__;

      // Note that actually there is no need to update the end iterator as this
      // one
      // is not affected by changes within hashtables (adding/deleting
      // elements).
      // Hence, for speedup, we do not update the end iterator
    }

    return *this;
  }

  // move operator
  template < typename Key, typename Alloc >
  Set< Key, Alloc >& Set< Key, Alloc >::operator=(Set< Key, Alloc >&& from) {
    inside__ = std::move(from.inside__);
    return *this;
  }

  // mathematical equality between two sets
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  bool Set< Key, Alloc >::operator==(const Set< Key, OtherAlloc >& s2) const {
    const HashTable< Key, bool, OtherAlloc >& h2 = s2.inside__;

    // check whether both sets have the same number of elements
    if (size() != h2.size()) return false;

    // check the content of the sets
    for (HashTableConstIterator< Key, bool > iter = inside__.cbegin();
         iter != inside__.cend();
         ++iter) {
      if (!h2.exists(iter.key())) return false;
    }

    return true;
  }

  // mathematical inequality between two sets
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  INLINE bool
     Set< Key, Alloc >::operator!=(const Set< Key, OtherAlloc >& s2) const {
    return !(operator==(s2));
  }

  // the usual begin iterator to parse the set
  template < typename Key, typename Alloc >
  INLINE typename Set< Key, Alloc >::iterator_safe
     Set< Key, Alloc >::beginSafe() const {
    return SetIteratorSafe< Key >{*this};
  }

  // the usual begin iterator to parse the set
  template < typename Key, typename Alloc >
  INLINE typename Set< Key, Alloc >::const_iterator_safe
     Set< Key, Alloc >::cbeginSafe() const {
    return SetIteratorSafe< Key >{*this};
  }

  // the usual end iterator to parse the set
  template < typename Key, typename Alloc >
  INLINE const typename Set< Key, Alloc >::iterator_safe&
     Set< Key, Alloc >::endSafe() const noexcept {
    return *(reinterpret_cast< const SetIteratorSafe< Key >* >(
       SetIteratorStaticEnd::SetIterEndSafe__));
  }

  // the usual end iterator to parse the set
  template < typename Key, typename Alloc >
  INLINE const typename Set< Key, Alloc >::const_iterator_safe&
     Set< Key, Alloc >::cendSafe() const noexcept {
    return *(reinterpret_cast< const SetIteratorSafe< Key >* >(
       SetIteratorStaticEnd::SetIterEndSafe__));
  }

  // the usual begin iterator to parse the set
  template < typename Key, typename Alloc >
  INLINE typename Set< Key, Alloc >::iterator Set< Key, Alloc >::begin() const {
    return SetIterator< Key >{*this};
  }

  // the usual begin iterator to parse the set
  template < typename Key, typename Alloc >
  INLINE typename Set< Key, Alloc >::const_iterator
     Set< Key, Alloc >::cbegin() const {
    return SetIterator< Key >{*this};
  }

  // the usual end iterator to parse the set
  template < typename Key, typename Alloc >
  INLINE const typename Set< Key, Alloc >::iterator&
     Set< Key, Alloc >::end() const noexcept {
    return *(reinterpret_cast< const SetIterator< Key >* >(
       SetIteratorStaticEnd::SetIterEnd__));
  }

  // the usual end iterator to parse the set
  template < typename Key, typename Alloc >
  INLINE const typename Set< Key, Alloc >::const_iterator&
     Set< Key, Alloc >::cend() const noexcept {
    return *(reinterpret_cast< const SetIterator< Key >* >(
       SetIteratorStaticEnd::SetIterEnd__));
  }

  // returns the size of the underlying hashtable containing the set
  template < typename Key, typename Alloc >
  INLINE Size Set< Key, Alloc >::capacity() const {
    return inside__.capacity();
  }

  // changes the size of the underlying hashtable
  template < typename Key, typename Alloc >
  INLINE void Set< Key, Alloc >::resize(Size new_size) {
    inside__.resize(new_size);

    // Note that actually there is no need to update the end iterator as this
    // one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }

  // enables the user to change dynamically the resizing policy of the
  // underlying hashtable
  template < typename Key, typename Alloc >
  INLINE void Set< Key, Alloc >::setResizePolicy(const bool new_policy) {
    inside__.setResizePolicy(new_policy);

    // Note that actually there is no need to update the end iterator as this
    // one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }

  // returns the current resizing policy of the underlying hashtable
  template < typename Key, typename Alloc >
  INLINE bool Set< Key, Alloc >::resizePolicy() const {
    return inside__.resizePolicy();
  }

  // indicates whether a given elements belong to the set
  template < typename Key, typename Alloc >
  INLINE bool Set< Key, Alloc >::contains(const Key& k) const {
    return inside__.exists(k);
  }


  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  INLINE bool
     Set< Key, Alloc >::isProperSubsetOf(const Set< Key, OtherAlloc >& s) const {
    if (this->size() >= s.size()) { return false; }

    for (const auto& elt: *this) {
      if (!s.contains(elt)) { return false; }
    }
    return true;
  }

  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  INLINE bool
     Set< Key, Alloc >::isProperSupersetOf(const Set< Key, OtherAlloc >& s) const {
    return s.isProperSubsetOf(*this);
  }


  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  INLINE bool
     Set< Key, Alloc >::isSubsetOrEqual(const Set< Key, OtherAlloc >& s) const {
    if (this->size() > s.size()) { return false; }

    for (const auto& elt: *this) {
      if (!s.contains(elt)) { return false; }
    }
    return true;
  }

  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  INLINE bool
     Set< Key, Alloc >::isSupersetOrEqual(const Set< Key, OtherAlloc >& s) const {
    return s.isLargeSubsetOf(*this);
  }

  // indicates whether a given elements belong to the set
  template < typename Key, typename Alloc >
  INLINE bool Set< Key, Alloc >::exists(const Key& k) const {
    return inside__.exists(k);
  }

  // inserts a new element in the set
  template < typename Key, typename Alloc >
  INLINE void Set< Key, Alloc >::insert(const Key& k) {
    // WARNING: we shall always test whether k already belongs to the set before
    // trying to insert it because we set inside__'s key uniqueness policy to
    // false
    if (!contains(k)) {
      // insert the element
      inside__.insert(k, true);

      // Note that actually there is no need to update the end iterator as this
      // one
      // is not affected by changes within hashtables (adding/deleting
      // elements).
      // Hence, for speedup, we do not update the end iterator
    }
  }

  // inserts a new element in the set
  template < typename Key, typename Alloc >
  INLINE void Set< Key, Alloc >::insert(Key&& k) {
    // WARNING: we shall always test whether k already belongs to the set before
    // trying to insert it because we set inside__'s key uniqueness policy to
    // false
    if (!contains(k)) {
      // insert the element
      inside__.insert(std::move(k), true);

      // Note that actually there is no need to update the end iterator as this
      // one
      // is not affected by changes within hashtables (adding/deleting
      // elements).
      // Hence, for speedup, we do not update the end iterator
    }
  }

  // emplace a new element in the set
  template < typename Key, typename Alloc >
  template < typename... Args >
  INLINE void Set< Key, Alloc >::emplace(Args&&... args) {
    insert(std::move(Key(std::forward< Args >(args)...)));
  }

  // erases an element from the set
  template < typename Key, typename Alloc >
  INLINE void Set< Key, Alloc >::erase(const Key& k) {
    // erase the element (if it exists)
    inside__.erase(k);

    // Note that actually there is no need to update the end iterator as this
    // one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }

  // erases an element from the set
  template < typename Key, typename Alloc >
  INLINE void Set< Key, Alloc >::erase(const SetIteratorSafe< Key >& iter) {
    // erase the element
    inside__.erase(iter.ht_iter__);

    // Note that actually there is no need to update the end iterator as this
    // one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }

  // adds a new element to the set
  template < typename Key, typename Alloc >
  INLINE Set< Key, Alloc >& Set< Key, Alloc >::operator<<(const Key& k) {
    insert(k);
    return *this;
  }

  // adds a new element to the set
  template < typename Key, typename Alloc >
  INLINE Set< Key, Alloc >& Set< Key, Alloc >::operator<<(Key&& k) {
    insert(std::move(k));
    return *this;
  }

  // removes an element from the set
  template < typename Key, typename Alloc >
  INLINE Set< Key, Alloc >& Set< Key, Alloc >::operator>>(const Key& k) {
    erase(k);
    return *this;
  }

  // returns the number of elements in the set
  template < typename Key, typename Alloc >
  INLINE Size Set< Key, Alloc >::size() const noexcept {
    return inside__.size();
  }

  // indicates whether the set is the empty set
  template < typename Key, typename Alloc >
  INLINE bool Set< Key, Alloc >::empty() const noexcept {
    return inside__.empty();
  }

  // Intersection operator
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  Set< Key, Alloc >
     Set< Key, Alloc >::operator*(const Set< Key, OtherAlloc >& s2) const {
    Set< Key, Alloc >                         res;
    const HashTable< Key, bool, OtherAlloc >& h2 = s2.inside__;
    HashTable< Key, bool, Alloc >&            h_r = res.inside__;

    if (size() < h2.size()) {
      for (HashTableConstIterator< Key, bool > iter = inside__.cbegin();
           iter != inside__.cend();
           ++iter) {
        if (h2.exists(iter.key())) h_r.insert(iter.key(), true);
      }
    } else {
      for (HashTableConstIterator< Key, bool > iter = h2.cbegin();
           iter != h2.cend();
           ++iter) {
        if (inside__.exists(iter.key())) h_r.insert(iter.key(), true);
      }
    }

    return res;
  }


  // Intersection update operator
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  const Set< Key, Alloc >&
     Set< Key, Alloc >::operator*=(const Set< Key, OtherAlloc >& s2) {
    if (&s2 != this) {
      const HashTable< Key, bool, OtherAlloc >& h2 = s2.inside__;
      for (auto iter = inside__.beginSafe(); iter != inside__.endSafe(); ++iter) {
        if (!h2.exists(iter.key())) inside__.erase(iter);
      }
    }

    return *this;
  }


  // Union update operator
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  const Set< Key, Alloc >&
     Set< Key, Alloc >::operator+=(const Set< Key, OtherAlloc >& s2) {
    if (&s2 != this) {
      for (auto pair: s2.inside__) {
        if (!inside__.exists(pair.first)) inside__.insert(pair.first, true);
      }
    }

    return *this;
  }


  // Union operator
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  Set< Key, Alloc >
     Set< Key, Alloc >::operator+(const Set< Key, OtherAlloc >& s2) const {
    Set< Key, Alloc >                         res = *this;
    const HashTable< Key, bool, OtherAlloc >& h2 = s2.inside__;
    HashTable< Key, bool, Alloc >&            h_r = res.inside__;

    for (HashTableConstIterator< Key, bool > iter = h2.cbegin(); iter != h2.cend();
         ++iter) {
      if (!h_r.exists(iter.key())) h_r.insert(iter.key(), true);
    }

    return res;
  }


  // Disjunction operator
  template < typename Key, typename Alloc >
  template < typename OtherAlloc >
  Set< Key, Alloc >
     Set< Key, Alloc >::operator-(const Set< Key, OtherAlloc >& s2) const {
    Set< Key, Alloc >                         res;
    const HashTable< Key, bool, OtherAlloc >& h2 = s2.inside__;
    HashTable< Key, bool, Alloc >&            h_r = res.inside__;

    for (HashTableConstIterator< Key, bool > iter = inside__.cbegin();
         iter != inside__.cend();
         ++iter)
      if (!h2.exists(iter.key())) h_r.insert(iter.key(), true);

    return res;
  }

  // to display the content of the set
  template < typename Key, typename Alloc >
  INLINE std::string Set< Key, Alloc >::toString() const {
    std::stringstream out;
    bool              first = true;
    out << "{";

    for (iterator iter = begin(); iter != end(); ++iter) {
      if (first) {
        out << *iter;
        first = false;
      } else {
        out << "," << *iter;
      }
    }

    out << "}";

    std::string res;
    out >> res;
    return res;
  }

  // to friendly display the content of the set
  template < typename Key, typename Alloc >
  std::ostream& operator<<(std::ostream& stream, const Set< Key, Alloc >& set) {
    stream << set.toString();
    return stream;
  }

  // creates a hashtable of NewKey from the set
  template < typename Key, typename Alloc >
  template < typename NewKey, typename NewAlloc >
  HashTable< Key, NewKey, NewAlloc >
     Set< Key, Alloc >::hashMap(NewKey (*f)(const Key&), Size size) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without resizing
    if (size == 0) size = std::max(Size(2), inside__.size() / 2);

    // create a new table
    HashTable< Key, NewKey, NewAlloc > table(size);

    // fill the new hash table
    for (HashTableConstIterator< Key, bool > iter = inside__.cbegin();
         iter != inside__.cend();
         ++iter) {
      table.insert(iter.key(), f(iter.key()));
    }

    return table;
  }

  // creates a hashtable of NewKey from the set
  template < typename Key, typename Alloc >
  template < typename NewKey, typename NewAlloc >
  HashTable< Key, NewKey, NewAlloc > Set< Key, Alloc >::hashMap(const NewKey& val,
                                                                Size size) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without resizing
    if (size == 0) size = std::max(Size(2), inside__.size() / 2);

    // create a new table
    HashTable< Key, NewKey, NewAlloc > table(size);

    // fill the new hash table
    for (HashTableConstIterator< Key, bool > iter = inside__.cbegin();
         iter != inside__.cend();
         ++iter) {
      table.insert(iter.key(), val);
    }

    return table;
  }

  // a method to create a list of NewKey from the set
  template < typename Key, typename Alloc >
  template < typename NewKey, typename NewAlloc >
  List< NewKey, NewAlloc >
     Set< Key, Alloc >::listMap(NewKey (*f)(const Key&)) const {
    // create a new list
    List< NewKey, NewAlloc > list;

    // fill the new list
    for (HashTableConstIterator< Key, bool > iter = inside__.cbegin();
         iter != inside__.cend();
         ++iter) {
      list.pushBack(f(iter.key()));
    }

    return list;
  }

  // Returns the value of a key as a Size
  template < typename T, typename Alloc >
  INLINE Size HashFunc< Set< T, Alloc > >::castToSize(const Set< T, Alloc >& key) {
    Size h = Size(0);
    for (const auto& k: key) {
      const auto hs = HashFunc< T >::castToSize(k);
      h += hs * (hs ^ HashFuncConst::gold);
    }

    return h;
  }


  // Returns the hashed value of a key.
  template < typename T, typename Alloc >
  INLINE Size
     HashFunc< Set< T, Alloc > >::operator()(const Set< T, Alloc >& key) const {
    return (castToSize(key) * HashFuncConst::gold) & this->hash_mask_;
  }

} /* namespace gum */
