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
 * @brief Implementation of the Set
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <sstream>

#include <agrum/core/set.h>

namespace gum {

  /* =========================================================================== */
  /* ===                          SAFE SET ITERATORS                         === */
  /* =========================================================================== */

  /// default constructor: the iterator points toward nothing
  template <typename Key> INLINE SetIteratorSafe<Key>::SetIteratorSafe() {
    GUM_CONSTRUCTOR(SetIteratorSafe);
  }

  /// creates an iterator for a given set
  template <typename Key>
  template <typename Alloc>
  INLINE SetIteratorSafe<Key>::SetIteratorSafe(const Set<Key, Alloc> &set,
                                               Position pos)
      : __ht_iter{pos == SetIteratorSafe<Key>::END ? set.__inside.cendSafe()
                                                   : set.__inside.cbeginSafe()} {
    GUM_CONSTRUCTOR(SetIteratorSafe);
  }

  /// copy constructor
  template <typename Key>
  INLINE SetIteratorSafe<Key>::SetIteratorSafe(const SetIteratorSafe<Key> &iter)
      : __ht_iter{iter.__ht_iter} {
    GUM_CONS_CPY(SetIteratorSafe);
  }

  /// copy constructor
  template <typename Key>
  INLINE SetIteratorSafe<Key>::SetIteratorSafe(const SetIterator<Key> &iter)
      : __ht_iter{iter.__ht_iter} {
    GUM_CONS_CPY(SetIteratorSafe);
  }

  /// move constructor
  template <typename Key>
  INLINE SetIteratorSafe<Key>::SetIteratorSafe(SetIteratorSafe<Key> &&from)
      : __ht_iter{std::move(from.__ht_iter)} {
    GUM_CONS_MOV(SetIteratorSafe);
  }

  /// destructor
  template <typename Key> INLINE SetIteratorSafe<Key>::~SetIteratorSafe() noexcept {
    GUM_DESTRUCTOR(SetIteratorSafe);
  }

  /// assignment operator
  template <typename Key>
  INLINE SetIteratorSafe<Key> &SetIteratorSafe<Key>::
  operator=(const SetIteratorSafe<Key> &from) {
    __ht_iter = from.__ht_iter;
    return *this;
  }

  /// assignment operator
  template <typename Key>
  INLINE SetIteratorSafe<Key> &SetIteratorSafe<Key>::
  operator=(const SetIterator<Key> &from) {
    __ht_iter = from.__ht_iter;
    return *this;
  }

  /// move operator
  template <typename Key>
  INLINE SetIteratorSafe<Key> &SetIteratorSafe<Key>::
  operator=(SetIteratorSafe<Key> &&from) noexcept {
    __ht_iter = std::move(from.__ht_iter);
    return *this;
  }

  /// increments the iterator
  template <typename Key>
  INLINE SetIteratorSafe<Key> &SetIteratorSafe<Key>::operator++() noexcept {
    // note that, if the hashtable's iterator points toward nothing, the
    // hashtable's iterator incrementation will do nothing. In particular, it
    // will not segfault.
    ++__ht_iter;
    return *this;
  }

  /// makes the iterator point to i elements further in the set
  template <typename Key>
  INLINE SetIteratorSafe<Key> &SetIteratorSafe<Key>::
  operator+=(unsigned int nb) noexcept {
    __ht_iter += nb;
    return *this;
  }

  /// returns a new iterator
  template <typename Key>
  INLINE SetIteratorSafe<Key> SetIteratorSafe<Key>::
  operator+(unsigned int nb) const {
    return SetIteratorSafe<Key>{*this} += nb;
  }

  /// indicates whether two iterators point to different elements or sets
  template <typename Key>
  INLINE bool SetIteratorSafe<Key>::
  operator!=(const SetIteratorSafe<Key> &from) const noexcept {
    return __ht_iter != from.__ht_iter;
  }

  /// indicates whether two iterators point toward the same element of a same set
  template <typename Key>
  INLINE bool SetIteratorSafe<Key>::
  operator==(const SetIteratorSafe<Key> &from) const noexcept {
    return __ht_iter == from.__ht_iter;
  }

  /// returns the element pointed to by the iterator
  template <typename Key> INLINE const Key &SetIteratorSafe<Key>::operator*() const {
    // note that, if the hashtable's iterator points toward nothing, it will
    // raise an UndefinedIteratorValue exception
    return __ht_iter.key();
  }

  /// returns aointer to the element pointed to by the iterator
  template <typename Key>
  INLINE const Key *SetIteratorSafe<Key>::operator->() const {
    // note that, if the hashtable's iterator points toward nothing, it will
    // raise an UndefinedIteratorValue exception
    return &(__ht_iter.key());
  }

  /** @brief makes the iterator point toward nothing (in particular, it is not
   * related anymore to its current set) */
  template <typename Key> INLINE void SetIteratorSafe<Key>::clear() noexcept {
    __ht_iter.clear();
  }

  /* =========================================================================== */
  /* ===                         UNSAFE SET ITERATORS                        === */
  /* =========================================================================== */

  /// default constructor: the iterator points toward nothing
  template <typename Key> INLINE SetIterator<Key>::SetIterator() noexcept {
    GUM_CONSTRUCTOR(SetIterator);
  }

  /// creates an iterator for a given set
  template <typename Key>
  template <typename Alloc>
  INLINE SetIterator<Key>::SetIterator(const Set<Key, Alloc> &set, Position pos)
      : __ht_iter{pos == SetIterator<Key>::END ? set.__inside.cend()
                                               : set.__inside.cbegin()} {
    GUM_CONSTRUCTOR(SetIterator);
  }

  /// copy constructor
  template <typename Key>
  INLINE SetIterator<Key>::SetIterator(const SetIterator<Key> &iter) noexcept
      : __ht_iter{iter.__ht_iter} {
    GUM_CONS_CPY(SetIterator);
  }

  /// move constructor
  template <typename Key>
  INLINE SetIterator<Key>::SetIterator(SetIterator<Key> &&from) noexcept
      : __ht_iter{std::move(from.__ht_iter)} {
    GUM_CONS_MOV(SetIterator);
  }

  /// destructor
  template <typename Key> INLINE SetIterator<Key>::~SetIterator() noexcept {
    GUM_DESTRUCTOR(SetIterator);
  }

  /// assignment operator
  template <typename Key>
  INLINE SetIterator<Key> &SetIterator<Key>::
  operator=(const SetIterator<Key> &from) noexcept {
    __ht_iter = from.__ht_iter;
    return *this;
  }

  /// move operator
  template <typename Key>
  INLINE SetIterator<Key> &SetIterator<Key>::
  operator=(SetIterator<Key> &&from) noexcept {
    __ht_iter = std::move(from.__ht_iter);
    return *this;
  }

  /// increments the iterator
  template <typename Key>
  INLINE SetIterator<Key> &SetIterator<Key>::operator++() noexcept {
    // note that, if the hashtable's iterator points toward nothing, the
    // hashtable's iterator incrementation will do nothing. In particular, it
    // will not segfault.
    ++__ht_iter;
    return *this;
  }

  /// makes the iterator point to i elements further in the set
  template <typename Key>
  INLINE SetIterator<Key> &SetIterator<Key>::operator+=(unsigned int nb) noexcept {
    __ht_iter += nb;
    return *this;
  }

  /// returns a new iterator
  template <typename Key>
  INLINE SetIterator<Key> SetIterator<Key>::operator+(unsigned int nb) const
      noexcept {
    return SetIterator<Key>{*this} += nb;
  }

  /// indicates whether two iterators point to different elements or sets
  template <typename Key>
  INLINE bool SetIterator<Key>::operator!=(const SetIterator<Key> &from) const
      noexcept {
    return __ht_iter != from.__ht_iter;
  }

  /// indicates whether two iterators point toward the same element of a same set
  template <typename Key>
  INLINE bool SetIterator<Key>::operator==(const SetIterator<Key> &from) const
      noexcept {
    return __ht_iter == from.__ht_iter;
  }

  /// returns the element pointed to by the iterator
  template <typename Key> INLINE const Key &SetIterator<Key>::operator*() const {
    // note that, if the hashtable's iterator points toward nothing, it will
    // raise an UndefinedIteratorValue exception
    return __ht_iter.key();
  }

  /// returns aointer to the element pointed to by the iterator
  template <typename Key> INLINE const Key *SetIterator<Key>::operator->() const {
    // note that, if the hashtable's iterator points toward nothing, it will
    // raise an UndefinedIteratorValue exception
    return &(__ht_iter.key());
  }

  /** @brief makes the iterator point toward nothing (in particular, it is not
   * related anymore to its current set) */
  template <typename Key> INLINE void SetIterator<Key>::clear() noexcept {
    __ht_iter.clear();
  }

  /* =========================================================================== */
  /* ===                                 SETS                                === */
  /* =========================================================================== */

  // returns the end iterator for other classes' statics
  template <typename Key, typename Alloc>
  INLINE const SetIteratorSafe<Key> &Set<Key, Alloc>::endSafe4Statics() {
    return *(reinterpret_cast<const SetIteratorSafe<Key> *>(
        SetIteratorStaticEnd::endSafe4Statics()));
  }

  // returns the end iterator for other classes' statics
  template <typename Key, typename Alloc>
  INLINE const SetIteratorSafe<Key> &Set<Key, Alloc>::constEndSafe4Statics() {
    return *(reinterpret_cast<const SetIteratorSafe<Key> *>(
        SetIteratorStaticEnd::constEndSafe4Statics()));
  }

  // returns the end iterator for other classes' statics
  template <typename Key, typename Alloc>
  INLINE const SetIterator<Key> &Set<Key, Alloc>::end4Statics() {
    return *(reinterpret_cast<const SetIterator<Key> *>(
        SetIteratorStaticEnd::end4Statics()));
  }

  // returns the end iterator for other classes' statics
  template <typename Key, typename Alloc>
  INLINE const SetIterator<Key> &Set<Key, Alloc>::constEnd4Statics() {
    return *(reinterpret_cast<const SetIterator<Key> *>(
        SetIteratorStaticEnd::constEnd4Statics()));
  }

  /// default constructor
  template <typename Key, typename Alloc>
  INLINE Set<Key, Alloc>::Set(Size capacity, bool resize_policy)
      : // create the hash table without key uniqueness policy (as we will check
        // ourselves the uniqueness of Keys before inserting new elements)
        __inside(capacity, resize_policy, false) {
    GUM_CONSTRUCTOR(Set);

    // make sure the end() iterator is constructed properly
    endSafe4Statics();
    end4Statics();
  }

  /*
    /// convert a hashtable into a set of keys
    template<typename Key, typename Alloc> INLINE
    Set<Key,Alloc>::Set( const HashTable<Key,bool>& h ) :
      __inside ( h ) {
      GUM_CONSTRUCTOR( Set );
      // ensure that __inside's uniqueness policy is set to false as this
      // will speed-up insertions of new elements
      __inside.setKeyUniquenessPolicy( false );
    }
  */

  /// initializer list constructor
  template <typename Key, typename Alloc>
  INLINE Set<Key, Alloc>::Set(std::initializer_list<Key> list)
      : __inside(list.size() / 2, true, false) {
    GUM_CONSTRUCTOR(Set);
    for (const auto &elt : list) {
      insert(elt);
    }

    // make sure the end() iterator is constructed properly
    endSafe4Statics();
    end4Statics();
  }

  /// copy constructor
  template <typename Key, typename Alloc>
  INLINE Set<Key, Alloc>::Set(const Set<Key, Alloc> &s)
      : __inside(s.__inside) {
    GUM_CONS_CPY(Set);
  }

  /// generalized copy constructor
  template <typename Key, typename Alloc>
  template <typename OtherAlloc>
  INLINE Set<Key, Alloc>::Set(const Set<Key, OtherAlloc> &s)
      : __inside(s.__inside) {
    GUM_CONS_CPY(Set);
  }

  /// move constructor
  template <typename Key, typename Alloc>
  INLINE Set<Key, Alloc>::Set(Set<Key, Alloc> &&s)
      : __inside(std::move(s.__inside)) {
    GUM_CONS_MOV(Set);
  }

  /// destructor
  template <typename Key, typename Alloc> INLINE Set<Key, Alloc>::Set::~Set() {
    GUM_DESTRUCTOR(Set);
  }

  /// removes all the elements, if any, from the set
  template <typename Key, typename Alloc> INLINE void Set<Key, Alloc>::clear() {
    // first we remove all the elements from the hashtable actually containing
    // the elements of the set. Note that, doing so, all the hashtable iterators
    // will be updated as well. In turn, this will imply that, whenever an
    // operation will be performed on a SetIteratorSafe, this will raise an
    // exception.
    __inside.clear();

    // Note that actually there is no need to update the end iterator as this one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }

  /// copy operator
  template <typename Key, typename Alloc>
  Set<Key, Alloc> &Set<Key, Alloc>::operator=(const Set<Key, Alloc> &s) {
    // avoid self assignment
    if (&s != this) {
      // remove the old content of the set. Actually, we remove all the elements
      // from the underlying hashtable. Note that, doing so, all the hashtable
      // iterators will be updated as well. In turn, this will imply that, whenever
      // an operation will be performed on a SetIteratorSafe, this will raise an
      // exception.
      clear();

      // prepare the set for its new data
      resize(s.capacity());
      setResizePolicy(s.resizePolicy());

      // copy the set
      __inside = s.__inside;

      // Note that actually there is no need to update the end iterator as this one
      // is not affected by changes within hashtables (adding/deleting elements).
      // Hence, for speedup, we do not update the end iterator
    }

    return *this;
  }

  /// generalized copy operator
  template <typename Key, typename Alloc>
  template <typename OtherAlloc>
  Set<Key, Alloc> &Set<Key, Alloc>::operator=(const Set<Key, OtherAlloc> &s) {
    // avoid self assignment
    if (this != reinterpret_cast<const Set<Key, Alloc> *>(&s)) {
      // remove the old content of the set. Actually, we remove all the elements
      // from the underlying hashtable. Note that, doing so, all the hashtable
      // iterators will be updated as well. In turn, this will imply that, whenever
      // an operation will be performed on a SetIteratorSafe, this will raise an
      // exception.
      clear();

      // prepare the set for its new data
      resize(s.capacity());
      setResizePolicy(s.resizePolicy());

      // copy the set
      __inside = s.__inside;

      // Note that actually there is no need to update the end iterator as this one
      // is not affected by changes within hashtables (adding/deleting elements).
      // Hence, for speedup, we do not update the end iterator
    }

    return *this;
  }

  /// move operator
  template <typename Key, typename Alloc>
  Set<Key, Alloc> &Set<Key, Alloc>::operator=(Set<Key, Alloc> &&from) {
    __inside = std::move(from.__inside);
    return *this;
  }

  /// mathematical equality between two sets
  template <typename Key, typename Alloc>
  template <typename OtherAlloc>
  bool Set<Key, Alloc>::operator==(const Set<Key, OtherAlloc> &s2) const {
    const HashTable<Key, bool, OtherAlloc> &h2 = s2.__inside;

    // check whether both sets have the same number of elements
    if (size() != h2.size())
      return false;

    // check the content of the sets
    for (HashTableConstIterator<Key, bool> iter = __inside.cbegin();
         iter != __inside.cend(); ++iter) {
      if (!h2.exists(iter.key()))
        return false;
    }

    return true;
  }

  /// mathematical inequality between two sets
  template <typename Key, typename Alloc>
  template <typename OtherAlloc>
  INLINE bool Set<Key, Alloc>::operator!=(const Set<Key, OtherAlloc> &s2) const {
    return !(operator==(s2));
  }

  /// the usual begin iterator to parse the set
  template <typename Key, typename Alloc>
  INLINE typename Set<Key, Alloc>::iterator_safe Set<Key, Alloc>::beginSafe() const {
    return SetIteratorSafe<Key>{*this};
  }

  /// the usual begin iterator to parse the set
  template <typename Key, typename Alloc>
  INLINE typename Set<Key, Alloc>::const_iterator_safe
  Set<Key, Alloc>::cbeginSafe() const {
    return SetIteratorSafe<Key>{*this};
  }

  /// the usual end iterator to parse the set
  template <typename Key, typename Alloc>
  INLINE const typename Set<Key, Alloc>::iterator_safe &
  Set<Key, Alloc>::endSafe() const noexcept {
    return *(reinterpret_cast<const SetIteratorSafe<Key> *>(
        SetIteratorStaticEnd::__SetIterEndSafe));
  }

  /// the usual end iterator to parse the set
  template <typename Key, typename Alloc>
  INLINE const typename Set<Key, Alloc>::const_iterator_safe &
  Set<Key, Alloc>::cendSafe() const noexcept {
    return *(reinterpret_cast<const SetIteratorSafe<Key> *>(
        SetIteratorStaticEnd::__SetIterEndSafe));
  }

  /// the usual begin iterator to parse the set
  template <typename Key, typename Alloc>
  INLINE typename Set<Key, Alloc>::iterator Set<Key, Alloc>::begin() const {
    return SetIterator<Key>{*this};
  }

  /// the usual begin iterator to parse the set
  template <typename Key, typename Alloc>
  INLINE typename Set<Key, Alloc>::const_iterator Set<Key, Alloc>::cbegin() const {
    return SetIterator<Key>{*this};
  }

  /// the usual end iterator to parse the set
  template <typename Key, typename Alloc>
  INLINE const typename Set<Key, Alloc>::iterator &Set<Key, Alloc>::end() const
      noexcept {
    return *(reinterpret_cast<const SetIterator<Key> *>(
        SetIteratorStaticEnd::__SetIterEnd));
  }

  /// the usual end iterator to parse the set
  template <typename Key, typename Alloc>
  INLINE const typename Set<Key, Alloc>::const_iterator &
  Set<Key, Alloc>::cend() const noexcept {
    return *(reinterpret_cast<const SetIterator<Key> *>(
        SetIteratorStaticEnd::__SetIterEnd));
  }

  /// returns the size of the underlying hashtable containing the set
  template <typename Key, typename Alloc>
  INLINE Size Set<Key, Alloc>::capacity() const {
    return __inside.capacity();
  }

  /// changes the size of the underlying hashtable
  template <typename Key, typename Alloc>
  INLINE void Set<Key, Alloc>::resize(Size new_size) {
    __inside.resize(new_size);

    // Note that actually there is no need to update the end iterator as this one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }

  /// enables the user to change dynamically the resizing policy of the
  /// underlying hashtable
  template <typename Key, typename Alloc>
  INLINE void Set<Key, Alloc>::setResizePolicy(const bool new_policy) {
    __inside.setResizePolicy(new_policy);

    // Note that actually there is no need to update the end iterator as this one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }

  /// returns the current resizing policy of the underlying hashtable
  template <typename Key, typename Alloc>
  INLINE bool Set<Key, Alloc>::resizePolicy() const {
    return __inside.resizePolicy();
  }

  /// indicates whether a given elements belong to the set
  template <typename Key, typename Alloc>
  INLINE bool Set<Key, Alloc>::contains(const Key &k) const {
    return __inside.exists(k);
  }

  /// indicates whether a given elements belong to the set
  template <typename Key, typename Alloc>
  INLINE bool Set<Key, Alloc>::exists(const Key &k) const {
    return __inside.exists(k);
  }

  /// inserts a new element in the set
  template <typename Key, typename Alloc>
  INLINE void Set<Key, Alloc>::insert(const Key &k) {
    // WARNING: we shall always test whether k already belongs to the set before
    // trying to insert it because we set __inside's key uniqueness policy to
    // false
    if (!contains(k)) {
      // insert the element
      __inside.insert(k, true);

      // Note that actually there is no need to update the end iterator as this one
      // is not affected by changes within hashtables (adding/deleting elements).
      // Hence, for speedup, we do not update the end iterator
    }
  }

  /// inserts a new element in the set
  template <typename Key, typename Alloc>
  INLINE void Set<Key, Alloc>::insert(Key &&k) {
    // WARNING: we shall always test whether k already belongs to the set before
    // trying to insert it because we set __inside's key uniqueness policy to
    // false
    if (!contains(k)) {
      // insert the element
      __inside.insert(std::move(k), true);

      // Note that actually there is no need to update the end iterator as this one
      // is not affected by changes within hashtables (adding/deleting elements).
      // Hence, for speedup, we do not update the end iterator
    }
  }

  /// emplace a new element in the set
  template <typename Key, typename Alloc>
  template <typename... Args>
  INLINE void Set<Key, Alloc>::emplace(Args &&... args) {
    insert(std::move(Key(std::forward<Args>(args)...)));
  }

  /// erases an element from the set
  template <typename Key, typename Alloc>
  INLINE void Set<Key, Alloc>::erase(const Key &k) {
    // erase the element (if it exists)
    __inside.erase(k);

    // Note that actually there is no need to update the end iterator as this one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }

  /// erases an element from the set
  template <typename Key, typename Alloc>
  INLINE void Set<Key, Alloc>::erase(const SetIteratorSafe<Key> &iter) {
    // erase the element
    __inside.erase(iter.__ht_iter);

    // Note that actually there is no need to update the end iterator as this one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }

  /// adds a new element to the set
  template <typename Key, typename Alloc>
  INLINE Set<Key, Alloc> &Set<Key, Alloc>::operator<<(const Key &k) {
    insert(k);
    return *this;
  }

  /// adds a new element to the set
  template <typename Key, typename Alloc>
  INLINE Set<Key, Alloc> &Set<Key, Alloc>::operator<<(Key &&k) {
    insert(std::move(k));
    return *this;
  }

  /// removes an element from the set
  template <typename Key, typename Alloc>
  INLINE Set<Key, Alloc> &Set<Key, Alloc>::operator>>(const Key &k) {
    erase(k);
    return *this;
  }

  /// returns the number of elements in the set
  template <typename Key, typename Alloc>
  INLINE Size Set<Key, Alloc>::size() const noexcept {
    return __inside.size();
  }

  /// indicates whether the set is the empty set
  template <typename Key, typename Alloc>
  INLINE bool Set<Key, Alloc>::empty() const noexcept {
    return __inside.empty();
  }

  /// Intersection operator
  template <typename Key, typename Alloc>
  template <typename OtherAlloc>
  Set<Key, Alloc> Set<Key, Alloc>::operator*(const Set<Key, OtherAlloc> &s2) const {
    Set<Key, Alloc> res;
    const HashTable<Key, bool, OtherAlloc> &h2 = s2.__inside;
    HashTable<Key, bool, Alloc> &h_r = res.__inside;

    if (size() < h2.size()) {
      for (HashTableConstIterator<Key, bool> iter = __inside.cbegin();
           iter != __inside.cend(); ++iter) {
        if (h2.exists(iter.key()))
          h_r.insert(iter.key(), true);
      }
    } else {
      for (HashTableConstIterator<Key, bool> iter = h2.cbegin(); iter != h2.cend();
           ++iter) {
        if (__inside.exists(iter.key()))
          h_r.insert(iter.key(), true);
      }
    }

    return res;
  }

  /// Union operator
  template <typename Key, typename Alloc>
  template <typename OtherAlloc>
  Set<Key, Alloc> Set<Key, Alloc>::operator+(const Set<Key, OtherAlloc> &s2) const {
    Set<Key, Alloc> res = *this;
    const HashTable<Key, bool, OtherAlloc> &h2 = s2.__inside;
    HashTable<Key, bool, Alloc> &h_r = res.__inside;

    for (HashTableConstIterator<Key, bool> iter = h2.cbegin(); iter != h2.cend();
         ++iter) {
      if (!h_r.exists(iter.key()))
        h_r.insert(iter.key(), true);
    }

    return res;
  }

  /// Disjunction operator
  template <typename Key, typename Alloc>
  template <typename OtherAlloc>
  Set<Key, Alloc> Set<Key, Alloc>::operator-(const Set<Key, OtherAlloc> &s2) const {
    Set<Key, Alloc> res;
    const HashTable<Key, bool, OtherAlloc> &h2 = s2.__inside;
    HashTable<Key, bool, Alloc> &h_r = res.__inside;

    for (HashTableConstIterator<Key, bool> iter = __inside.cbegin();
         iter != __inside.cend(); ++iter)
      if (!h2.exists(iter.key()))
        h_r.insert(iter.key(), true);

    return res;
  }

  /// to display the content of the set
  template <typename Key, typename Alloc>
  INLINE std::string Set<Key, Alloc>::toString() const {
    std::stringstream out;
    bool first = true;
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

  /// to friendly display the content of the set
  template <typename Key, typename Alloc>
  std::ostream &operator<<(std::ostream &stream, const Set<Key, Alloc> &set) {
    stream << set.toString();
    return stream;
  }

  /// creates a hashtable of NewKey from the set
  template <typename Key, typename Alloc>
  template <typename NewKey, typename NewAlloc>
  HashTable<Key, NewKey, NewAlloc> Set<Key, Alloc>::hashMap(NewKey (*f)(const Key &),
                                                            Size size) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without resizing
    if (size == 0)
      size = std::max(2UL, __inside.size() / 2);

    // create a new table
    HashTable<Key, NewKey, NewAlloc> table(size);

    // fill the new hash table
    for (HashTableConstIterator<Key, bool> iter = __inside.cbegin();
         iter != __inside.cend(); ++iter) {
      table.insert(iter.key(), f(iter.key()));
    }

    return table;
  }

  /// creates a hashtable of NewKey from the set
  template <typename Key, typename Alloc>
  template <typename NewKey, typename NewAlloc>
  HashTable<Key, NewKey, NewAlloc> Set<Key, Alloc>::hashMap(const NewKey &val,
                                                            Size size) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without resizing
    if (size == 0)
      size = std::max(2UL, __inside.size() / 2);

    // create a new table
    HashTable<Key, NewKey, NewAlloc> table(size);

    // fill the new hash table
    for (HashTableConstIterator<Key, bool> iter = __inside.cbegin();
         iter != __inside.cend(); ++iter) {
      table.insert(iter.key(), val);
    }

    return table;
  }

  /// a method to create a list of NewKey from the set
  template <typename Key, typename Alloc>
  template <typename NewKey, typename NewAlloc>
  List<NewKey, NewAlloc> Set<Key, Alloc>::listMap(NewKey (*f)(const Key &)) const {
    // create a new list
    List<NewKey, NewAlloc> list;

    // fill the new list
    for (HashTableConstIterator<Key, bool> iter = __inside.cbegin();
         iter != __inside.cend(); ++iter) {
      list.pushBack(f(iter.key()));
    }

    return list;
  }

} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
