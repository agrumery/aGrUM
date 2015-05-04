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
 * @brief Class providing generic double hash tables
 *
 * @author Jean-Philippe DUBUS, Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* =========================================================================== */
  /* ===                      BIJECTION SAFE ITERATORS                       === */
  /* =========================================================================== */

  /// Default constructor
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2>::BijectionIteratorSafe() noexcept {
    GUM_CONSTRUCTOR(BijectionIteratorSafe);
  }

  /// Constructor
  template <typename T1, typename T2>
  template <typename Alloc, bool Gen>
  INLINE BijectionIteratorSafe<T1, T2>::BijectionIteratorSafe(
      const BijectionImplementation<T1, T2, Alloc, Gen> &bijection)
      : __iter{bijection.__firstToSecond.cbeginSafe()} {
    GUM_CONSTRUCTOR(BijectionIteratorSafe);
  }

  /// Constructor
  template <typename T1, typename T2>
  template <typename Alloc>
  INLINE BijectionIteratorSafe<T1, T2>::BijectionIteratorSafe(
      const Bijection<T1, T2, Alloc> &bijection)
      : __iter{bijection.__firstToSecond.cbeginSafe()} {
    GUM_CONSTRUCTOR(BijectionIteratorSafe);
  }

  /// Copy constructor
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2>::BijectionIteratorSafe(
      const BijectionIteratorSafe<T1, T2> &toCopy)
      : __iter{toCopy.__iter} {
    GUM_CONS_CPY(BijectionIteratorSafe);
  }

  /// move constructor
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2>::BijectionIteratorSafe(
      BijectionIteratorSafe<T1, T2> &&from) noexcept
      : __iter{std::move(from.__iter)} {
    GUM_CONS_MOV(BijectionIteratorSafe);
  }

  /// Destructor
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2>::~BijectionIteratorSafe() noexcept {
    GUM_DESTRUCTOR(BijectionIteratorSafe);
  }

  /// Copy operator
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2> &BijectionIteratorSafe<T1, T2>::
  operator=(const BijectionIteratorSafe<T1, T2> &toCopy) {
    __iter = toCopy.__iter;
    return *this;
  }

  /// move operator
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2> &BijectionIteratorSafe<T1, T2>::
  operator=(BijectionIteratorSafe<T1, T2> &&toCopy) noexcept {
    __iter = std::move(toCopy.__iter);
    return *this;
  }

  /// Go to the next association (if exists)
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2> &BijectionIteratorSafe<T1, T2>::
  operator++() noexcept {
    ++__iter;
    return *this;
  }

  /// moves the iterator by nb elements
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2> &BijectionIteratorSafe<T1, T2>::
  operator+=(unsigned int nb) noexcept {
    __iter += nb;
    return *this;
  }

  /// returns a new iterator
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2> BijectionIteratorSafe<T1, T2>::
  operator+(unsigned int nb) noexcept {
    return BijectionIteratorSafe<T1, T2>{*this} += nb;
  }

  /// Comparison of iterators
  template <typename T1, typename T2>
  INLINE bool BijectionIteratorSafe<T1, T2>::
  operator!=(const BijectionIteratorSafe<T1, T2> &toCompare) const noexcept {
    return __iter != toCompare.__iter;
  }

  /// Comparison of iterators
  template <typename T1, typename T2>
  INLINE bool BijectionIteratorSafe<T1, T2>::
  operator==(const BijectionIteratorSafe<T1, T2> &toCompare) const noexcept {
    return __iter == toCompare.__iter;
  }

  /// return the first element of the current association
  template <typename T1, typename T2>
  INLINE const T1 &BijectionIteratorSafe<T1, T2>::first() const {
    return __iter.key();
  }

  /// return the second element of the current association
  template <typename T1, typename T2>
  INLINE const T2 &BijectionIteratorSafe<T1, T2>::second() const {
    return Getter::op_second(__iter.val());
  }

  /* =========================================================================== */
  /* ===                     BIJECTION UNSAFE ITERATORS                      === */
  /* =========================================================================== */

  /// Default constructor
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2>::BijectionIterator() noexcept {
    GUM_CONSTRUCTOR(BijectionIterator);
  }

  /// Constructor
  template <typename T1, typename T2>
  template <typename Alloc, bool Gen>
  INLINE BijectionIterator<T1, T2>::BijectionIterator(
      const BijectionImplementation<T1, T2, Alloc, Gen> &bijection)
      : __iter{bijection.__firstToSecond.cbegin()} {
    GUM_CONSTRUCTOR(BijectionIterator);
  }

  /// Constructor
  template <typename T1, typename T2>
  template <typename Alloc>
  INLINE BijectionIterator<T1, T2>::BijectionIterator(
      const Bijection<T1, T2, Alloc> &bijection)
      : __iter{bijection.__firstToSecond.cbegin()} {
    GUM_CONSTRUCTOR(BijectionIterator);
  }

  /// Copy constructor
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2>::BijectionIterator(
      const BijectionIterator<T1, T2> &toCopy)
      : __iter{toCopy.__iter} {
    GUM_CONS_CPY(BijectionIterator);
  }

  /// move constructor
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2>::BijectionIterator(
      BijectionIterator<T1, T2> &&from) noexcept : __iter{std::move(from.__iter)} {
    GUM_CONS_MOV(BijectionIterator);
  }

  /// Destructor
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2>::~BijectionIterator() noexcept {
    GUM_DESTRUCTOR(BijectionIterator);
  }

  /// Copy operator
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2> &BijectionIterator<T1, T2>::
  operator=(const BijectionIterator<T1, T2> &toCopy) {
    __iter = toCopy.__iter;
    return *this;
  }

  /// move operator
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2> &BijectionIterator<T1, T2>::
  operator=(BijectionIterator<T1, T2> &&toCopy) noexcept {
    __iter = std::move(toCopy.__iter);
    return *this;
  }

  /// Go to the next association (if exists)
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2> &BijectionIterator<T1, T2>::
  operator++() noexcept {
    ++__iter;
    return *this;
  }

  /// moves the iterator by nb elements
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2> &BijectionIterator<T1, T2>::
  operator+=(unsigned int nb) noexcept {
    __iter += nb;
    return *this;
  }

  /// returns a new iterator
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2> BijectionIterator<T1, T2>::
  operator+(unsigned int nb) noexcept {
    return BijectionIterator<T1, T2>{*this} += nb;
  }

  /// Comparison of iterators
  template <typename T1, typename T2>
  INLINE bool BijectionIterator<T1, T2>::
  operator!=(const BijectionIterator<T1, T2> &toCompare) const noexcept {
    return __iter != toCompare.__iter;
  }

  /// Comparison of iterators
  template <typename T1, typename T2>
  INLINE bool BijectionIterator<T1, T2>::
  operator==(const BijectionIterator<T1, T2> &toCompare) const noexcept {
    return __iter == toCompare.__iter;
  }

  /// return the first element of the current association
  template <typename T1, typename T2>
  INLINE const T1 &BijectionIterator<T1, T2>::first() const {
    return __iter.key();
  }

  /// return the second element of the current association
  template <typename T1, typename T2>
  INLINE const T2 &BijectionIterator<T1, T2>::second() const {
    return Getter::op_second(__iter.val());
  }

  /* =========================================================================== */
  /* ===                NON SCALAR BIJECTION IMPLEMENTATION                  === */
  /* =========================================================================== */

  // returns the end iterator for other classes' statics
  template <typename T1, typename T2, typename Alloc, bool Gen>
  const BijectionIteratorSafe<T1, T2> &
  BijectionImplementation<T1, T2, Alloc, Gen>::endSafe4Statics() {
    return *(reinterpret_cast<const BijectionIteratorSafe<T1, T2> *>(
        BijectionIteratorStaticEnd::endSafe4Statics()));
  }

  // returns the end iterator for other classes' statics
  template <typename T1, typename T2, typename Alloc, bool Gen>
  const BijectionIterator<T1, T2> &
  BijectionImplementation<T1, T2, Alloc, Gen>::end4Statics() {
    return *(reinterpret_cast<const BijectionIterator<T1, T2> *>(
        BijectionIteratorStaticEnd::end4Statics()));
  }

  /// a function that performs a complete copy of another bijection
  template <typename T1, typename T2, typename Alloc, bool Gen>
  template <typename OtherAlloc>
  INLINE void BijectionImplementation<T1, T2, Alloc, Gen>::__copy(
      const HashTable<T1, T2 *, OtherAlloc> &f2s) {
    // parse f2s and perform copies
    for (auto iter = f2s.cbegin(); iter != f2s.cend(); ++iter) {
      typename HashTable12::value_type *val1 =
          &(__firstToSecond.insert(iter.key(), nullptr));
      typename HashTable21::value_type *val2;

      try {
        val2 = &(__secondToFirst.insert(*(iter.val()), nullptr));
      } catch (...) {
        __firstToSecond.erase(iter.key());
        throw;
      }

      val1->second = &(const_cast<T2 &>(val2->first));
      val2->second = &(const_cast<T1 &>(val1->first));
    }

    // note that __iter_end is actually a constant, whatever we add/remove
    // to/from __firstToSecond. As a consequence, it need not be updated
    // after __copy
  }

  /// Default constructor: creates a bijection without association
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE BijectionImplementation<T1, T2, Alloc, Gen>::BijectionImplementation(
      Size size, bool resize_policy)
      : // warning: below, we create the internal hashTables with a key uniqueness
        // policy set to false because we will do the uniqueness tests ourselves
        // (this
        // will speed-up the process)
        __firstToSecond(size, resize_policy, false),
        __secondToFirst(size, resize_policy, false) {
    GUM_CONSTRUCTOR(BijectionImplementation);

    // make sure the end() iterator is constructed properly
    end4Statics();
    endSafe4Statics();
  }

  /// initializer list constructor
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE BijectionImplementation<T1, T2, Alloc, Gen>::BijectionImplementation(
      std::initializer_list<std::pair<T1, T2>> list)
      : __firstToSecond(list.size() / 2, true, false),
        __secondToFirst(list.size() / 2, true, false) {
    GUM_CONSTRUCTOR(BijectionImplementation);

    for (const auto &elt : list) {
      insert(elt.first, elt.second);
    }

    // make sure the end() iterator is constructed properly
    end4Statics();
    endSafe4Statics();
  }

  /// Copy constructor
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE BijectionImplementation<T1, T2, Alloc, Gen>::BijectionImplementation(
      const BijectionImplementation<T1, T2, Alloc, Gen> &toCopy)
      : __firstToSecond(toCopy.__firstToSecond.capacity(), true, false),
        __secondToFirst(toCopy.__secondToFirst.capacity(), true, false) {
    GUM_CONS_CPY(BijectionImplementation);
    __copy(toCopy.__firstToSecond);
  }

  /// Generalized copy constructor
  template <typename T1, typename T2, typename Alloc, bool Gen>
  template <typename OtherAlloc>
  INLINE BijectionImplementation<T1, T2, Alloc, Gen>::BijectionImplementation(
      const BijectionImplementation<T1, T2, OtherAlloc, Gen> &toCopy)
      : __firstToSecond(toCopy.__firstToSecond.capacity(), true, false),
        __secondToFirst(toCopy.__secondToFirst.capacity(), true, false) {
    GUM_CONS_CPY(BijectionImplementation);
    __copy(toCopy.__firstToSecond);
  }

  /// move constructor
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE BijectionImplementation<T1, T2, Alloc, Gen>::BijectionImplementation(
      BijectionImplementation<T1, T2, Alloc, Gen> &&from) noexcept
      : __firstToSecond(std::move(from.__firstToSecond)),
        __secondToFirst(std::move(from.__secondToFirst)) {
    GUM_CONS_MOV(BijectionImplementation);
  }

  /// destructor
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE BijectionImplementation<T1, T2, Alloc, Gen>::~BijectionImplementation() {
    GUM_DESTRUCTOR(BijectionImplementation);
  }

  /// removes all the associations from the bijection
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE void BijectionImplementation<T1, T2, Alloc, Gen>::clear() {
    __firstToSecond.clear();
    __secondToFirst.clear();
    // note that __iter_end is actually a constant, whatever we add/remove
    // to/from __firstToSecond. As a consequence, it need not be updated
    // after the clear's
  }

  /// Copy operator
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE BijectionImplementation<T1, T2, Alloc, Gen> &
      BijectionImplementation<T1, T2, Alloc, Gen>::
      operator=(const BijectionImplementation<T1, T2, Alloc, Gen> &toCopy) {
    // avoid self assignment
    if (this != &toCopy) {
      clear();
      __copy(toCopy.__firstToSecond);
    }

    // note that __iter_end is actually a constant, whatever we add/remove
    // to/from __firstToSecond. As a consequence, it need not be updated
    // after __copy
    return *this;
  }

  /// Generalized copy operator
  template <typename T1, typename T2, typename Alloc, bool Gen>
  template <typename OtherAlloc>
  INLINE BijectionImplementation<T1, T2, Alloc, Gen> &
      BijectionImplementation<T1, T2, Alloc, Gen>::
      operator=(const BijectionImplementation<T1, T2, OtherAlloc, Gen> &toCopy) {
    clear();
    __copy(toCopy.__firstToSecond);

    // note that __iter_end is actually a constant, whatever we add/remove
    // to/from __firstToSecond. As a consequence, it need not be updated
    // after __copy
    return *this;
  }

  /// move operator
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE BijectionImplementation<T1, T2, Alloc, Gen> &
      BijectionImplementation<T1, T2, Alloc, Gen>::
      operator=(BijectionImplementation<T1, T2, Alloc, Gen> &&from) {
    // avoid self assignment
    if (this != &from) {
      clear();
      __firstToSecond = std::move(from.__firstToSecond);
      __secondToFirst = std::move(from.__secondToFirst);
    }

    // note that __iter_end is actually a constant, whatever we add/remove
    // to/from __firstToSecond. As a consequence, it need not be updated
    // after __copy
    return *this;
  }

  /// returns the iterator at the beginning of the bijection
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE typename BijectionImplementation<T1, T2, Alloc, Gen>::iterator
  BijectionImplementation<T1, T2, Alloc, Gen>::begin() const {
    return BijectionIterator<T1, T2>{*this};
  }

  /// returns the iterator at the beginning of the bijection
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE typename BijectionImplementation<T1, T2, Alloc, Gen>::const_iterator
  BijectionImplementation<T1, T2, Alloc, Gen>::cbegin() const {
    return BijectionIterator<T1, T2>{*this};
  }

  /// returns the iterator to the end of the bijection
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE const typename BijectionImplementation<T1, T2, Alloc, Gen>::iterator &
  BijectionImplementation<T1, T2, Alloc, Gen>::end() const noexcept {
    return *(reinterpret_cast<const BijectionIterator<T1, T2> *>(
        BijectionIteratorStaticEnd::__BijectionIterEnd));
  }

  /// returns the iterator to the end of the bijection
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE const typename BijectionImplementation<T1, T2, Alloc, Gen>::const_iterator &
  BijectionImplementation<T1, T2, Alloc, Gen>::cend() const noexcept {
    return *(reinterpret_cast<const BijectionIterator<T1, T2> *>(
        BijectionIteratorStaticEnd::__BijectionIterEnd));
  }

  /// returns the iterator at the beginning of the bijection
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE typename BijectionImplementation<T1, T2, Alloc, Gen>::iterator_safe
  BijectionImplementation<T1, T2, Alloc, Gen>::beginSafe() const {
    return BijectionIteratorSafe<T1, T2>{*this};
  }

  /// returns the iterator at the beginning of the bijection
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE typename BijectionImplementation<T1, T2, Alloc, Gen>::const_iterator_safe
  BijectionImplementation<T1, T2, Alloc, Gen>::cbeginSafe() const {
    return BijectionIteratorSafe<T1, T2>{*this};
  }

  /// returns the iterator to the end of the bijection
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE const typename BijectionImplementation<T1, T2, Alloc, Gen>::iterator_safe &
  BijectionImplementation<T1, T2, Alloc, Gen>::endSafe() const noexcept {
    return *(reinterpret_cast<const BijectionIteratorSafe<T1, T2> *>(
        BijectionIteratorStaticEnd::__BijectionIterEndSafe));
  }

  /// returns the iterator to the end of the bijection
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE const typename BijectionImplementation<T1, T2, Alloc,
                                                Gen>::const_iterator_safe &
  BijectionImplementation<T1, T2, Alloc, Gen>::cendSafe() const noexcept {
    return *(reinterpret_cast<const BijectionIteratorSafe<T1, T2> *>(
        BijectionIteratorStaticEnd::__BijectionIterEndSafe));
  }

  /// returns the value associated to the element passed in argument
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE const T1 &
  BijectionImplementation<T1, T2, Alloc, Gen>::first(const T2 &second) const {
    return *(__secondToFirst[second]);
  }

  /// returns the value associated to the element passed in argument
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE const T2 &
  BijectionImplementation<T1, T2, Alloc, Gen>::second(const T1 &first) const {
    return *(__firstToSecond[first]);
  }

  /// Test whether the bijection contains the "first" value
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE bool
  BijectionImplementation<T1, T2, Alloc, Gen>::existsFirst(const T1 &first) const {
    return __firstToSecond.exists(first);
  }

  /// Test whether the bijection contains the "second" value
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE bool
  BijectionImplementation<T1, T2, Alloc, Gen>::existsSecond(const T2 &second) const {
    return __secondToFirst.exists(second);
  }

  /// inserts a new association in the bijection
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE typename BijectionImplementation<T1, T2, Alloc,
                                          Gen>::HashTable12::value_type *
  BijectionImplementation<T1, T2, Alloc, Gen>::__insert(const T1 &first,
                                                        const T2 &second) {
    // check the uniqueness property
    if (existsFirst(first) || existsSecond(second)) {
      GUM_ERROR(DuplicateElement,
                "the bijection contains an element with the same key");
    }

    // insert copies of first and second
    typename HashTable12::value_type *val1 =
        &(__firstToSecond.insert(first, nullptr));
    typename HashTable21::value_type *val2;

    try {
      val2 = &(__secondToFirst.insert(second, nullptr));
    } catch (...) {
      __firstToSecond.erase(first);
      throw;
    }

    val1->second = &(const_cast<T2 &>(val2->first));
    val2->second = &(const_cast<T1 &>(val1->first));

    return val1;
  }

  /// inserts a new association in the bijection
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE typename BijectionImplementation<T1, T2, Alloc,
                                          Gen>::HashTable12::value_type *
  BijectionImplementation<T1, T2, Alloc, Gen>::__insert(T1 &&first, T2 &&second) {
    // check the uniqueness property
    if (existsFirst(first) || existsSecond(second)) {
      GUM_ERROR(DuplicateElement,
                "the bijection contains an element with the same key");
    }

    // insert copies of first and second
    typename HashTable12::value_type *val1 =
        &(__firstToSecond.insert(std::move(first), nullptr));
    typename HashTable21::value_type *val2;

    try {
      val2 = &(__secondToFirst.insert(std::move(second), nullptr));
    } catch (...) {
      __firstToSecond.erase(val1->first);
      throw;
    }

    val1->second = &(const_cast<T2 &>(val2->first));
    val2->second = &(const_cast<T1 &>(val1->first));

    return val1;
  }

  /** @brief Same method as first, but if the value is not found, a default
   * value is inserted into the bijection */
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE const T1 &BijectionImplementation<T1, T2, Alloc, Gen>::firstWithDefault(
      const T2 &second, const T1 &val) const {
    try {
      return first(second);
    } catch (NotFound &) {
      return __insert(val, second)->first;
    }
  }

  /** @brief Same method as second, but if the value is not found, a default
   * value is inserted into the bijection */
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE const T2 &BijectionImplementation<T1, T2, Alloc, Gen>::secondWithDefault(
      const T1 &first, const T2 &val) const {
    try {
      return second(first);
    } catch (NotFound &) {
      return *(__insert(first, val)->second);
    }
  }

  /// inserts a new association in the bijection
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE void BijectionImplementation<T1, T2, Alloc, Gen>::insert(const T1 &first,
                                                                  const T2 &second) {
    __insert(first, second);
  }

  /// inserts a new association in the bijection
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE void BijectionImplementation<T1, T2, Alloc, Gen>::insert(T1 &&first,
                                                                  T2 &&second) {
    __insert(std::move(first), std::move(second));
  }

  /// emplace a new element in the bijection
  template <typename T1, typename T2, typename Alloc, bool Gen>
  template <typename... Args>
  INLINE void BijectionImplementation<T1, T2, Alloc, Gen>::emplace(Args &&... args) {
    std::pair<T1, T2> new_elt(std::forward<Args>(args)...);
    __insert(std::move(new_elt.first), std::move(new_elt.second));
  }

  /// returns true if the bijection doesn't contain any relation
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE bool BijectionImplementation<T1, T2, Alloc, Gen>::empty() const noexcept {
    GUM_ASSERT(__firstToSecond.empty() == __secondToFirst.empty());
    return __firstToSecond.empty();
  }

  /// returns the number of associations stored within the bijection
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE Size BijectionImplementation<T1, T2, Alloc, Gen>::size() const noexcept {
    GUM_ASSERT(__firstToSecond.size() == __secondToFirst.size());
    return __firstToSecond.size();
  }

  /// erases an association containing the given first element
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE void
  BijectionImplementation<T1, T2, Alloc, Gen>::eraseFirst(const T1 &first) {
    try {
      __secondToFirst.erase(*__firstToSecond[first]);
      __firstToSecond.erase(first);
    } catch (NotFound &) {
    }
  }

  /// erase an association containing the given second element
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE void
  BijectionImplementation<T1, T2, Alloc, Gen>::eraseSecond(const T2 &second) {
    try {
      __firstToSecond.erase(*__secondToFirst[second]);
      __secondToFirst.erase(second);
    } catch (NotFound &) {
    }
  }

  /// returns the number of hashtables' slots used (@sa hashTable's capacity)
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE Size BijectionImplementation<T1, T2, Alloc, Gen>::capacity() const
      noexcept {
    return __firstToSecond.capacity();
  }

  /// similar to the hashtable's resize
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE void BijectionImplementation<T1, T2, Alloc, Gen>::resize(Size new_size) {
    __firstToSecond.resize(new_size);
    __secondToFirst.resize(new_size);
  }

  /// enables the user to change dynamically the resizing policy
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE void BijectionImplementation<T1, T2, Alloc, Gen>::setResizePolicy(
      const bool new_policy) noexcept {
    __firstToSecond.setResizePolicy(new_policy);
    __secondToFirst.setResizePolicy(new_policy);
  }

  /// returns the current resizing policy
  template <typename T1, typename T2, typename Alloc, bool Gen>
  INLINE bool BijectionImplementation<T1, T2, Alloc, Gen>::resizePolicy() const
      noexcept {
    return __firstToSecond.resizePolicy();
  }

  /// friendly displays the content of the CliqueGraph
  template <typename T1, typename T2, typename Alloc, bool Gen>
  std::string BijectionImplementation<T1, T2, Alloc, Gen>::toString() const {
    std::stringstream stream;
    stream << "{ ";
    bool first = true;

    for (iterator iter = begin(); iter != end(); ++iter) {
      if (!first)
        stream << ", ";
      else
        first = false;

      stream << '(' << iter.first() << " <-> " << iter.second() << ')';
    }

    stream << " }";
    return stream.str();
  }

  /* =========================================================================== */
  /* ===                  SCALAR BIJECTION IMPLEMENTATION                    === */
  /* =========================================================================== */

  // returns the end iterator for other classes' statics
  template <typename T1, typename T2, typename Alloc>
  const BijectionIteratorSafe<T1, T2> &
  BijectionImplementation<T1, T2, Alloc, true>::endSafe4Statics() {
    return *(reinterpret_cast<const BijectionIteratorSafe<T1, T2> *>(
        BijectionIteratorStaticEnd::endSafe4Statics()));
  }

  // returns the end iterator for other classes' statics
  template <typename T1, typename T2, typename Alloc>
  const BijectionIterator<T1, T2> &
  BijectionImplementation<T1, T2, Alloc, true>::end4Statics() {
    return *(reinterpret_cast<const BijectionIterator<T1, T2> *>(
        BijectionIteratorStaticEnd::end4Statics()));
  }

  /// Default constructor: creates a bijection without association
  template <typename T1, typename T2, typename Alloc>
  INLINE BijectionImplementation<T1, T2, Alloc, true>::BijectionImplementation(
      Size size, bool resize_policy)
      : // warning: below, we create the internal hashTables with a key uniqueness
        // policy set to false because we will do the uniqueness tests ourselves
        // (this
        // will speed-up the process)
        __firstToSecond(size, resize_policy, false),
        __secondToFirst(size, resize_policy, false) {
    GUM_CONSTRUCTOR(BijectionImplementation);

    // make sure the end() iterator is constructed properly
    end4Statics();
    endSafe4Statics();
  }

  /// initializer list constructor
  template <typename T1, typename T2, typename Alloc>
  INLINE BijectionImplementation<T1, T2, Alloc, true>::BijectionImplementation(
      std::initializer_list<std::pair<T1, T2>> list)
      : __firstToSecond(list.size() / 2, true, false),
        __secondToFirst(list.size() / 2, true, false) {
    GUM_CONSTRUCTOR(BijectionImplementation);

    for (const auto &elt : list) {
      insert(elt.first, elt.second);
    }

    // make sure the end() iterator is constructed properly
    end4Statics();
    endSafe4Statics();
  }

  /// a function that performs a complete copy of another bijection
  template <typename T1, typename T2, typename Alloc>
  template <typename OtherAlloc>
  INLINE void BijectionImplementation<T1, T2, Alloc, true>::__copy(
      const HashTable<T1, T2, OtherAlloc> &f2s) {
    // parse f2s and perform copies
    for (auto iter = f2s.cbegin(); iter != f2s.cend(); ++iter) {
      __firstToSecond.insert(iter.key(), iter.val());

      try {
        __secondToFirst.insert(iter.val(), iter.key());
      } catch (...) {
        __firstToSecond.erase(iter.key());
        throw;
      }
    }

    // note that __iter_end is actually a constant, whatever we add/remove
    // to/from __firstToSecond. As a consequence, it need not be updated
    // after __copy
  }

  /// Copy constructor
  template <typename T1, typename T2, typename Alloc>
  INLINE BijectionImplementation<T1, T2, Alloc, true>::BijectionImplementation(
      const BijectionImplementation<T1, T2, Alloc, true> &toCopy)
      : __firstToSecond(toCopy.__firstToSecond.capacity(), true, false),
        __secondToFirst(toCopy.__secondToFirst.capacity(), true, false) {
    GUM_CONS_CPY(BijectionImplementation);
    __copy(toCopy.__firstToSecond);
  }

  /// Generalized copy constructor
  template <typename T1, typename T2, typename Alloc>
  template <typename OtherAlloc>
  INLINE BijectionImplementation<T1, T2, Alloc, true>::BijectionImplementation(
      const BijectionImplementation<T1, T2, OtherAlloc, true> &toCopy)
      : __firstToSecond(toCopy.__firstToSecond.capacity(), true, false),
        __secondToFirst(toCopy.__secondToFirst.capacity(), true, false) {
    GUM_CONS_CPY(BijectionImplementation);
    __copy(toCopy.__firstToSecond);
  }

  /// move constructor
  template <typename T1, typename T2, typename Alloc>
  INLINE BijectionImplementation<T1, T2, Alloc, true>::BijectionImplementation(
      BijectionImplementation<T1, T2, Alloc, true> &&toCopy) noexcept
      : __firstToSecond(std::move(toCopy.__firstToSecond)),
        __secondToFirst(std::move(toCopy.__secondToFirst)) {
    GUM_CONS_MOV(BijectionImplementation);
  }

  /// destructor
  template <typename T1, typename T2, typename Alloc>
  INLINE BijectionImplementation<T1, T2, Alloc, true>::~BijectionImplementation() {
    GUM_DESTRUCTOR(BijectionImplementation);
  }

  /// returns the iterator at the beginning of the bijection
  template <typename T1, typename T2, typename Alloc>
  INLINE typename BijectionImplementation<T1, T2, Alloc, true>::iterator
  BijectionImplementation<T1, T2, Alloc, true>::begin() const {
    return BijectionIterator<T1, T2>{*this};
  }

  /// returns the iterator at the beginning of the bijection
  template <typename T1, typename T2, typename Alloc>
  INLINE typename BijectionImplementation<T1, T2, Alloc, true>::const_iterator
  BijectionImplementation<T1, T2, Alloc, true>::cbegin() const {
    return BijectionIterator<T1, T2>{*this};
  }

  /// returns the iterator to the end of the bijection
  template <typename T1, typename T2, typename Alloc>
  INLINE const typename BijectionImplementation<T1, T2, Alloc, true>::iterator &
  BijectionImplementation<T1, T2, Alloc, true>::end() const noexcept {
    return *(reinterpret_cast<const BijectionIterator<T1, T2> *>(
        BijectionIteratorStaticEnd::__BijectionIterEnd));
  }

  /// returns the iterator to the end of the bijection
  template <typename T1, typename T2, typename Alloc>
  INLINE const typename BijectionImplementation<T1, T2, Alloc,
                                                true>::const_iterator &
  BijectionImplementation<T1, T2, Alloc, true>::cend() const noexcept {
    return *(reinterpret_cast<const BijectionIterator<T1, T2> *>(
        BijectionIteratorStaticEnd::__BijectionIterEnd));
  }

  /// returns the iterator at the beginning of the bijection
  template <typename T1, typename T2, typename Alloc>
  INLINE typename BijectionImplementation<T1, T2, Alloc, true>::iterator_safe
  BijectionImplementation<T1, T2, Alloc, true>::beginSafe() const {
    return BijectionIteratorSafe<T1, T2>{*this};
  }

  /// returns the iterator at the beginning of the bijection
  template <typename T1, typename T2, typename Alloc>
  INLINE typename BijectionImplementation<T1, T2, Alloc, true>::const_iterator_safe
  BijectionImplementation<T1, T2, Alloc, true>::cbeginSafe() const {
    return BijectionIteratorSafe<T1, T2>{*this};
  }

  /// returns the iterator to the end of the bijection
  template <typename T1, typename T2, typename Alloc>
  INLINE const typename BijectionImplementation<T1, T2, Alloc, true>::iterator_safe &
  BijectionImplementation<T1, T2, Alloc, true>::endSafe() const noexcept {
    return *(reinterpret_cast<const BijectionIteratorSafe<T1, T2> *>(
        BijectionIteratorStaticEnd::__BijectionIterEndSafe));
  }

  /// returns the iterator to the end of the bijection
  template <typename T1, typename T2, typename Alloc>
  INLINE const typename BijectionImplementation<T1, T2, Alloc,
                                                true>::const_iterator_safe &
  BijectionImplementation<T1, T2, Alloc, true>::cendSafe() const noexcept {
    return *(reinterpret_cast<const BijectionIteratorSafe<T1, T2> *>(
        BijectionIteratorStaticEnd::__BijectionIterEndSafe));
  }

  /// removes all the associations from the bijection
  template <typename T1, typename T2, typename Alloc>
  INLINE void BijectionImplementation<T1, T2, Alloc, true>::clear() {
    __firstToSecond.clear();
    __secondToFirst.clear();
    // note that __iter_end is actually a constant, whatever we add/remove
    // to/from __firstToSecond. As a consequence, it need not be updated
    // after the clear's
  }

  /// Copy operator
  template <typename T1, typename T2, typename Alloc>
  INLINE BijectionImplementation<T1, T2, Alloc, true> &
      BijectionImplementation<T1, T2, Alloc, true>::
      operator=(const BijectionImplementation<T1, T2, Alloc, true> &toCopy) {
    // avoid self assignment
    if (this != &toCopy) {
      clear();
      __copy(toCopy.__firstToSecond);
    }

    // note that __iter_end is actually a constant, whatever we add/remove
    // to/from __firstToSecond. As a consequence, it need not be updated
    // after __copy
    return *this;
  }

  /// Generalized copy operator
  template <typename T1, typename T2, typename Alloc>
  template <typename OtherAlloc>
  INLINE BijectionImplementation<T1, T2, Alloc, true> &
      BijectionImplementation<T1, T2, Alloc, true>::
      operator=(const BijectionImplementation<T1, T2, OtherAlloc, true> &toCopy) {
    clear();
    __copy(toCopy.__firstToSecond);

    // note that __iter_end is actually a constant, whatever we add/remove
    // to/from __firstToSecond. As a consequence, it need not be updated
    // after __copy
    return *this;
  }

  /// move operator
  template <typename T1, typename T2, typename Alloc>
  INLINE BijectionImplementation<T1, T2, Alloc, true> &
      BijectionImplementation<T1, T2, Alloc, true>::
      operator=(BijectionImplementation<T1, T2, Alloc, true> &&toCopy) {
    // avoid self assignment
    if (this != &toCopy) {
      clear();
      __firstToSecond = std::move(toCopy.__firstToSecond);
      __secondToFirst = std::move(toCopy.__secondToFirst);
    }

    // note that __iter_end is actually a constant, whatever we add/remove
    // to/from __firstToSecond. As a consequence, it need not be updated
    // after __copy
    return *this;
  }

  /// returns the value associated to the element passed in argument
  template <typename T1, typename T2, typename Alloc>
  INLINE const T1 &
  BijectionImplementation<T1, T2, Alloc, true>::first(T2 second) const {
    return __secondToFirst[second];
  }

  /// returns the value associated to the element passed in argument
  template <typename T1, typename T2, typename Alloc>
  INLINE const T2 &
  BijectionImplementation<T1, T2, Alloc, true>::second(T1 first) const {
    return __firstToSecond[first];
  }

  /// Test whether the bijection contains the "first" value
  template <typename T1, typename T2, typename Alloc>
  INLINE bool
  BijectionImplementation<T1, T2, Alloc, true>::existsFirst(T1 first) const {
    return __firstToSecond.exists(first);
  }

  /// Test whether the bijection contains the "second" value
  template <typename T1, typename T2, typename Alloc>
  INLINE bool
  BijectionImplementation<T1, T2, Alloc, true>::existsSecond(T2 second) const {
    return __secondToFirst.exists(second);
  }

  /// inserts a new association in the bijection
  template <typename T1, typename T2, typename Alloc>
  INLINE void BijectionImplementation<T1, T2, Alloc, true>::__insert(T1 first,
                                                                     T2 second) {
    // check the uniqueness property
    if (existsFirst(first) || existsSecond(second)) {
      GUM_ERROR(DuplicateElement,
                "the bijection contains an element with the same key");
    }

    // insert copies of first and second
    __firstToSecond.insert(first, second);

    try {
      __secondToFirst.insert(second, first);
    } catch (...) {
      __firstToSecond.erase(first);
      throw;
    }
  }

  /// inserts a new association in the bijection
  template <typename T1, typename T2, typename Alloc>
  INLINE void BijectionImplementation<T1, T2, Alloc, true>::insert(T1 first,
                                                                   T2 second) {
    __insert(first, second);
  }

  /// emplace a new element in the bijection
  template <typename T1, typename T2, typename Alloc>
  template <typename... Args>
  INLINE void
  BijectionImplementation<T1, T2, Alloc, true>::emplace(Args &&... args) {
    std::pair<T1, T2> new_elt(std::forward<Args>(args)...);
    __insert(new_elt.first, new_elt.second);
  }

  /** @brief Same method as first, but if the value is not found, a default
   * value is inserted into the bijection */
  template <typename T1, typename T2, typename Alloc>
  INLINE const T1 &
  BijectionImplementation<T1, T2, Alloc, true>::firstWithDefault(T2 second,
                                                                 T1 val) const {
    try {
      return first(second);
    } catch (NotFound &) {
      __insert(val, second);
      return val;
    }
  }

  /** @brief Same method as second, but if the value is not found, a default
   * value is inserted into the bijection */
  template <typename T1, typename T2, typename Alloc>
  INLINE const T2 &
  BijectionImplementation<T1, T2, Alloc, true>::secondWithDefault(T1 first,
                                                                  T2 val) const {
    try {
      return second(first);
    } catch (NotFound &) {
      __insert(first, val);
      return val;
    }
  }

  /// returns true if the bijection doesn't contain any relation
  template <typename T1, typename T2, typename Alloc>
  INLINE bool BijectionImplementation<T1, T2, Alloc, true>::empty() const noexcept {
    GUM_ASSERT(__firstToSecond.empty() == __secondToFirst.empty());
    return __firstToSecond.empty();
  }

  /// returns the number of associations stored within the bijection
  template <typename T1, typename T2, typename Alloc>
  INLINE Size BijectionImplementation<T1, T2, Alloc, true>::size() const noexcept {
    GUM_ASSERT(__firstToSecond.size() == __secondToFirst.size());
    return __firstToSecond.size();
  }

  /// erases an association containing the given first element
  template <typename T1, typename T2, typename Alloc>
  INLINE void BijectionImplementation<T1, T2, Alloc, true>::eraseFirst(T1 first) {
    try {
      __secondToFirst.erase(__firstToSecond[first]);
      __firstToSecond.erase(first);
    } catch (NotFound &) {
    }
  }

  /// erase an association containing the given second element
  template <typename T1, typename T2, typename Alloc>
  INLINE void BijectionImplementation<T1, T2, Alloc, true>::eraseSecond(T2 second) {
    try {
      __firstToSecond.erase(__secondToFirst[second]);
      __secondToFirst.erase(second);
    } catch (NotFound &) {
    }
  }

  /// returns the number of hashtables' slots used (@sa hashTable's capacity)
  template <typename T1, typename T2, typename Alloc>
  INLINE Size BijectionImplementation<T1, T2, Alloc, true>::capacity() const
      noexcept {
    return __firstToSecond.capacity();
  }

  /// similar to the hashtable's resize
  template <typename T1, typename T2, typename Alloc>
  INLINE void BijectionImplementation<T1, T2, Alloc, true>::resize(Size new_size) {
    __firstToSecond.resize(new_size);
    __secondToFirst.resize(new_size);
  }

  /// enables the user to change dynamically the resizing policy
  template <typename T1, typename T2, typename Alloc>
  INLINE void BijectionImplementation<T1, T2, Alloc, true>::setResizePolicy(
      const bool new_policy) noexcept {
    __firstToSecond.setResizePolicy(new_policy);
    __secondToFirst.setResizePolicy(new_policy);
  }

  /// returns the current resizing policy
  template <typename T1, typename T2, typename Alloc>
  INLINE bool BijectionImplementation<T1, T2, Alloc, true>::resizePolicy() const
      noexcept {
    return __firstToSecond.resizePolicy();
  }

  /// friendly displays the content of the CliqueGraph
  template <typename T1, typename T2, typename Alloc>
  std::string BijectionImplementation<T1, T2, Alloc, true>::toString() const {
    std::stringstream stream;
    stream << "{ ";
    bool first = true;

    for (iterator iter = begin(); iter != end(); ++iter) {
      if (!first)
        stream << ", ";
      else
        first = false;

      stream << '(' << iter.first() << " <-> " << iter.second() << ')';
    }

    stream << " }";
    return stream.str();
  }

  // ============================================================================
  // bijections
  // ============================================================================

  /// Default constructor: creates a bijection without any association
  template <typename T1, typename T2, typename Alloc>
  INLINE Bijection<T1, T2, Alloc>::Bijection(Size size, bool resize_policy)
      : BijectionImplementation<T1, T2, Alloc, std::is_scalar<T1>::value &&
                                                   std::is_scalar<T2>::value>(
            size, resize_policy) {
    GUM_CONSTRUCTOR(Bijection);
  }

  /// initializer list constructor
  template <typename T1, typename T2, typename Alloc>
  INLINE
  Bijection<T1, T2, Alloc>::Bijection(std::initializer_list<std::pair<T1, T2>> list)
      : BijectionImplementation<T1, T2, Alloc, std::is_scalar<T1>::value &&
                                                   std::is_scalar<T2>::value>{list} {
    GUM_CONSTRUCTOR(Bijection);
  }

  /// Copy constructor
  template <typename T1, typename T2, typename Alloc>
  INLINE Bijection<T1, T2, Alloc>::Bijection(const Bijection<T1, T2, Alloc> &toCopy)
      : BijectionImplementation<T1, T2, Alloc, std::is_scalar<T1>::value &&
                                                   std::is_scalar<T2>::value>(
            toCopy) {
    GUM_CONS_CPY(Bijection);
  }

  /// Generalized copy constructor
  template <typename T1, typename T2, typename Alloc>
  template <typename OtherAlloc>
  INLINE
  Bijection<T1, T2, Alloc>::Bijection(const Bijection<T1, T2, OtherAlloc> &toCopy)
      : BijectionImplementation<T1, T2, Alloc, std::is_scalar<T1>::value &&
                                                   std::is_scalar<T2>::value>(
            toCopy) {
    GUM_CONS_CPY(Bijection);
  }

  /// move constructor
  template <typename T1, typename T2, typename Alloc>
  INLINE
  Bijection<T1, T2, Alloc>::Bijection(Bijection<T1, T2, Alloc> &&from) noexcept
      : BijectionImplementation<T1, T2, Alloc, std::is_scalar<T1>::value &&
                                                   std::is_scalar<T2>::value>(
            std::move(from)) {
    GUM_CONS_MOV(Bijection);
  }

  /// destructor
  template <typename T1, typename T2, typename Alloc>
  INLINE Bijection<T1, T2, Alloc>::~Bijection() {
    GUM_DESTRUCTOR(Bijection);
  }

  /// copy operator
  template <typename T1, typename T2, typename Alloc>
  INLINE Bijection<T1, T2, Alloc> &Bijection<T1, T2, Alloc>::
  operator=(const Bijection<T1, T2, Alloc> &toCopy) {
    Implementation::operator=(toCopy);
    return *this;
  }

  /// generalized copy operator
  template <typename T1, typename T2, typename Alloc>
  template <typename OtherAlloc>
  INLINE Bijection<T1, T2, Alloc> &Bijection<T1, T2, Alloc>::
  operator=(const Bijection<T1, T2, OtherAlloc> &toCopy) {
    Implementation::operator=(toCopy);
    return *this;
  }

  /// move operator
  template <typename T1, typename T2, typename Alloc>
  INLINE Bijection<T1, T2, Alloc> &Bijection<T1, T2, Alloc>::
  operator=(Bijection<T1, T2, Alloc> &&bij) {
    Implementation::operator=(std::move(bij));
    return *this;
  }

  /// for friendly displaying the content of bijections
  template <typename T1, typename T2, typename Alloc>
  std::ostream &operator<<(std::ostream &stream, const Bijection<T1, T2, Alloc> &b) {
    stream << b.toString();
    return stream;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
