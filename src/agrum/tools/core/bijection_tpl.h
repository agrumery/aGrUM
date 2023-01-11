/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Class providing generic double hash tables
 *
 * @author Jean-Philippe DUBUS, Christophe GONZALES(_at_AMU) and Pierre-Henri
 * WUILLEMIN(_at_LIP6)
 */

// To simply IDE parsing
#include <agrum/tools/core/bijection.h>

namespace gum {

  // ===========================================================================
  // ===                NON SCALAR BIJECTION IMPLEMENTATION                  ===
  // ===========================================================================

  // a function that performs a complete copy of another bijection
  template < typename T1, typename T2, bool Gen >
  INLINE void BijectionImplementation< T1, T2, Gen >::_copy_(const HashTable< T1, T2* >& f2s) {
    // parse f2s and perform copies
    for (auto iter = f2s.cbegin(); iter != f2s.cend(); ++iter) {
      typename HashTable12::value_type* val1 = &(_firstToSecond_.insert(iter.key(), nullptr));
      typename HashTable21::value_type* val2;

      try {
        val2 = &(_secondToFirst_.insert(*(iter.val()), nullptr));
      } catch (...) {
        _firstToSecond_.erase(iter.key());
        throw;
      }

      val1->second = &(const_cast< T2& >(val2->first));
      val2->second = &(const_cast< T1& >(val1->first));
    }

    // note that  _iter_end_ is actually a constant, whatever we add/remove
    // to/from  _firstToSecond_. As a consequence, it need not be updated
    // after  _copy_
  }

  // Default constructor: creates a bijection without association
  template < typename T1, typename T2, bool Gen >
  INLINE BijectionImplementation< T1, T2, Gen >::BijectionImplementation(Size size,
                                                                         bool resize_policy) :
      // warning: below, we create the internal hashTables with a key
      // uniqueness
      // policy set to false because we will do the uniqueness tests ourselves
      // (this
      // will speed-up the process)
      _firstToSecond_(size, resize_policy, false),
      _secondToFirst_(size, resize_policy, false) {
    GUM_CONSTRUCTOR(BijectionImplementation);
  }

  // initializer list constructor
  template < typename T1, typename T2, bool Gen >
  INLINE BijectionImplementation< T1, T2, Gen >::BijectionImplementation(
     std::initializer_list< std::pair< T1, T2 > > list) :
      _firstToSecond_(Size(list.size()) / 2, true, false),
      _secondToFirst_(Size(list.size()) / 2, true, false) {
    GUM_CONSTRUCTOR(BijectionImplementation);

    for (const auto& elt: list) {
      insert(elt.first, elt.second);
    }
  }

  // Copy constructor
  template < typename T1, typename T2, bool Gen >
  INLINE BijectionImplementation< T1, T2, Gen >::BijectionImplementation(
     const BijectionImplementation< T1, T2, Gen >& toCopy) :
      _firstToSecond_(toCopy._firstToSecond_.capacity(), true, false),
      _secondToFirst_(toCopy._secondToFirst_.capacity(), true, false) {
    GUM_CONS_CPY(BijectionImplementation);
    _copy_(toCopy._firstToSecond_);
  }

  // move constructor
  template < typename T1, typename T2, bool Gen >
  INLINE BijectionImplementation< T1, T2, Gen >::BijectionImplementation(
     BijectionImplementation< T1, T2, Gen >&& from) noexcept :
      _firstToSecond_(std::move(from._firstToSecond_)),
      _secondToFirst_(std::move(from._secondToFirst_)) {
    GUM_CONS_MOV(BijectionImplementation);
  }

  // destructor
  template < typename T1, typename T2, bool Gen >
  INLINE BijectionImplementation< T1, T2, Gen >::~BijectionImplementation() {
    GUM_DESTRUCTOR(BijectionImplementation);
  }

  // removes all the associations from the bijection
  template < typename T1, typename T2, bool Gen >
  INLINE void BijectionImplementation< T1, T2, Gen >::clear() {
    _firstToSecond_.clear();
    _secondToFirst_.clear();
    // note that  _iter_end_ is actually a constant, whatever we add/remove
    // to/from  _firstToSecond_. As a consequence, it need not be updated
    // after the clear's
  }

  // Copy operator
  template < typename T1, typename T2, bool Gen >
  INLINE BijectionImplementation< T1, T2, Gen >& BijectionImplementation< T1, T2, Gen >::operator=(
     const BijectionImplementation< T1, T2, Gen >& toCopy) {
    // avoid self assignment
    if (this != &toCopy) {
      clear();
      _copy_(toCopy._firstToSecond_);
    }

    // note that  _iter_end_ is actually a constant, whatever we add/remove
    // to/from  _firstToSecond_. As a consequence, it need not be updated
    // after  _copy_
    return *this;
  }

  // move operator
  template < typename T1, typename T2, bool Gen >
  INLINE BijectionImplementation< T1, T2, Gen >& BijectionImplementation< T1, T2, Gen >::operator=(
     BijectionImplementation< T1, T2, Gen >&& from) {
    // avoid self assignment
    if (this != &from) {
      clear();
      _firstToSecond_ = std::move(from._firstToSecond_);
      _secondToFirst_ = std::move(from._secondToFirst_);
    }

    // note that  _iter_end_ is actually a constant, whatever we add/remove
    // to/from  _firstToSecond_. As a consequence, it need not be updated
    // after  _copy_
    return *this;
  }

  // returns the iterator at the beginning of the bijection
  template < typename T1, typename T2, bool Gen >
  INLINE typename BijectionImplementation< T1, T2, Gen >::iterator
     BijectionImplementation< T1, T2, Gen >::begin() const {
    return BijectionIterator< T1, T2 >{*this};
  }

  // returns the iterator at the beginning of the bijection
  template < typename T1, typename T2, bool Gen >
  INLINE typename BijectionImplementation< T1, T2, Gen >::const_iterator
     BijectionImplementation< T1, T2, Gen >::cbegin() const {
    return BijectionIterator< T1, T2 >{*this};
  }

  // returns the iterator to the end of the bijection
  template < typename T1, typename T2, bool Gen >
  INLINE const typename BijectionImplementation< T1, T2, Gen >::iterator&
     BijectionImplementation< T1, T2, Gen >::end() const noexcept {
    return *(reinterpret_cast< const BijectionIterator< T1, T2 >* >(_Bijection_end_));
  }

  // returns the iterator to the end of the bijection
  template < typename T1, typename T2, bool Gen >
  INLINE const typename BijectionImplementation< T1, T2, Gen >::const_iterator&
     BijectionImplementation< T1, T2, Gen >::cend() const noexcept {
    return *(reinterpret_cast< const BijectionIterator< T1, T2 >* >(_Bijection_end_));
  }

  // returns the iterator at the beginning of the bijection
  template < typename T1, typename T2, bool Gen >
  INLINE typename BijectionImplementation< T1, T2, Gen >::iterator_safe
     BijectionImplementation< T1, T2, Gen >::beginSafe() const {
    return BijectionIteratorSafe< T1, T2 >{*this};
  }

  // returns the iterator at the beginning of the bijection
  template < typename T1, typename T2, bool Gen >
  INLINE typename BijectionImplementation< T1, T2, Gen >::const_iterator_safe
     BijectionImplementation< T1, T2, Gen >::cbeginSafe() const {
    return BijectionIteratorSafe< T1, T2 >{*this};
  }

  // returns the iterator to the end of the bijection
  template < typename T1, typename T2, bool Gen >
  INLINE const typename BijectionImplementation< T1, T2, Gen >::iterator_safe&
     BijectionImplementation< T1, T2, Gen >::endSafe() const noexcept {
    return *(reinterpret_cast< const BijectionIteratorSafe< T1, T2 >* >(_Bijection_end_safe_));
  }

  // returns the iterator to the end of the bijection
  template < typename T1, typename T2, bool Gen >
  INLINE const typename BijectionImplementation< T1, T2, Gen >::const_iterator_safe&
     BijectionImplementation< T1, T2, Gen >::cendSafe() const noexcept {
    return *(reinterpret_cast< const BijectionIteratorSafe< T1, T2 >* >(_Bijection_end_safe_));
  }

  // returns the value associated to the element passed in argument
  template < typename T1, typename T2, bool Gen >
  INLINE const T1& BijectionImplementation< T1, T2, Gen >::first(const T2& second) const {
    return *(_secondToFirst_[second]);
  }

  // returns the value associated to the element passed in argument
  template < typename T1, typename T2, bool Gen >
  INLINE const T2& BijectionImplementation< T1, T2, Gen >::second(const T1& first) const {
    return *(_firstToSecond_[first]);
  }

  // Test whether the bijection contains the "first" value
  template < typename T1, typename T2, bool Gen >
  INLINE bool BijectionImplementation< T1, T2, Gen >::existsFirst(const T1& first) const {
    return _firstToSecond_.exists(first);
  }

  // Test whether the bijection contains the "second" value
  template < typename T1, typename T2, bool Gen >
  INLINE bool BijectionImplementation< T1, T2, Gen >::existsSecond(const T2& second) const {
    return _secondToFirst_.exists(second);
  }

  // inserts a new association in the bijection
  template < typename T1, typename T2, bool Gen >
  INLINE typename BijectionImplementation< T1, T2, Gen >::HashTable12::value_type*
     BijectionImplementation< T1, T2, Gen >::_insert_(const T1& first, const T2& second) {
    // check the uniqueness property
    if (existsFirst(first) || existsSecond(second)) {
      GUM_ERROR(DuplicateElement,
                "the bijection contains an element with the same couple (" << first << "," << second
                                                                           << ")");
    }

    // insert copies of first and second
    typename HashTable12::value_type* val1 = &(_firstToSecond_.insert(first, nullptr));
    typename HashTable21::value_type* val2;

    try {
      val2 = &(_secondToFirst_.insert(second, nullptr));
    } catch (...) {
      _firstToSecond_.erase(first);
      throw;
    }

    val1->second = &(const_cast< T2& >(val2->first));
    val2->second = &(const_cast< T1& >(val1->first));

    return val1;
  }

  // inserts a new association in the bijection
  template < typename T1, typename T2, bool Gen >
  INLINE typename BijectionImplementation< T1, T2, Gen >::HashTable12::value_type*
     BijectionImplementation< T1, T2, Gen >::_insert_(T1&& first, T2&& second) {
    // check the uniqueness property
    if (existsFirst(first) || existsSecond(second)) {
      GUM_ERROR(DuplicateElement,
                "the bijection contains an element with the same couple (" << first << "," << second
                                                                           << ")");
    }

    // insert copies of first and second
    typename HashTable12::value_type* val1 = &(_firstToSecond_.insert(std::move(first), nullptr));
    typename HashTable21::value_type* val2;

    try {
      val2 = &(_secondToFirst_.insert(std::move(second), nullptr));
    } catch (...) {
      _firstToSecond_.erase(val1->first);
      throw;
    }

    val1->second = &(const_cast< T2& >(val2->first));
    val2->second = &(const_cast< T1& >(val1->first));

    return val1;
  }

  /* @brief Same method as first, but if the value is not found, a default
   * value is inserted into the bijection */
  template < typename T1, typename T2, bool Gen >
  INLINE const T1& BijectionImplementation< T1, T2, Gen >::firstWithDefault(const T2& second,
                                                                            const T1& val) const {
    try {
      return first(second);
    } catch (NotFound const&) { return _insert_(val, second)->first; }
  }

  /* @brief Same method as second, but if the value is not found, a default
   * value is inserted into the bijection */
  template < typename T1, typename T2, bool Gen >
  INLINE const T2& BijectionImplementation< T1, T2, Gen >::secondWithDefault(const T1& first,
                                                                             const T2& val) const {
    try {
      return second(first);
    } catch (NotFound const&) { return *(_insert_(first, val)->second); }
  }

  // inserts a new association in the bijection
  template < typename T1, typename T2, bool Gen >
  INLINE void BijectionImplementation< T1, T2, Gen >::insert(const T1& first, const T2& second) {
    _insert_(first, second);
  }

  // inserts a new association in the bijection
  template < typename T1, typename T2, bool Gen >
  INLINE void BijectionImplementation< T1, T2, Gen >::insert(T1&& first, T2&& second) {
    _insert_(std::move(first), std::move(second));
  }

  // emplace a new element in the bijection
  template < typename T1, typename T2, bool Gen >
  template < typename... Args >
  INLINE void BijectionImplementation< T1, T2, Gen >::emplace(Args&&... args) {
    std::pair< T1, T2 > new_elt(std::forward< Args >(args)...);
    _insert_(std::move(new_elt.first), std::move(new_elt.second));
  }

  // returns true if the bijection doesn't contain any relation
  template < typename T1, typename T2, bool Gen >
  INLINE bool BijectionImplementation< T1, T2, Gen >::empty() const noexcept {
    GUM_ASSERT(_firstToSecond_.empty() == _secondToFirst_.empty());
    return _firstToSecond_.empty();
  }

  // returns the number of associations stored within the bijection
  template < typename T1, typename T2, bool Gen >
  INLINE Size BijectionImplementation< T1, T2, Gen >::size() const noexcept {
    GUM_ASSERT(_firstToSecond_.size() == _secondToFirst_.size());
    return _firstToSecond_.size();
  }

  // erases an association containing the given first element
  template < typename T1, typename T2, bool Gen >
  INLINE void BijectionImplementation< T1, T2, Gen >::eraseFirst(const T1& first) {
    try {
      _secondToFirst_.erase(*_firstToSecond_[first]);
      _firstToSecond_.erase(first);
    } catch (NotFound const&) {}
  }

  // erase an association containing the given second element
  template < typename T1, typename T2, bool Gen >
  INLINE void BijectionImplementation< T1, T2, Gen >::eraseSecond(const T2& second) {
    try {
      _firstToSecond_.erase(*_secondToFirst_[second]);
      _secondToFirst_.erase(second);
    } catch (NotFound const&) {}
  }

  // returns the number of hashtables' slots used (@sa hashTable's capacity)
  template < typename T1, typename T2, bool Gen >
  INLINE Size BijectionImplementation< T1, T2, Gen >::capacity() const noexcept {
    return _firstToSecond_.capacity();
  }

  // similar to the hashtable's resize
  template < typename T1, typename T2, bool Gen >
  INLINE void BijectionImplementation< T1, T2, Gen >::resize(Size new_size) {
    _firstToSecond_.resize(new_size);
    _secondToFirst_.resize(new_size);
  }

  // enables the user to change dynamically the resizing policy
  template < typename T1, typename T2, bool Gen >
  INLINE void
     BijectionImplementation< T1, T2, Gen >::setResizePolicy(const bool new_policy) noexcept {
    _firstToSecond_.setResizePolicy(new_policy);
    _secondToFirst_.setResizePolicy(new_policy);
  }

  // returns the current resizing policy
  template < typename T1, typename T2, bool Gen >
  INLINE bool BijectionImplementation< T1, T2, Gen >::resizePolicy() const noexcept {
    return _firstToSecond_.resizePolicy();
  }

  // friendly displays the content of the CliqueGraph
  template < typename T1, typename T2, bool Gen >
  std::string BijectionImplementation< T1, T2, Gen >::toString() const {
    std::stringstream stream;
    stream << "{ ";
    bool first = true;

    for (iterator iter = begin(); iter != end(); ++iter) {
      if (!first) stream << ", ";
      else first = false;

      stream << '(' << iter.first() << " <-> " << iter.second() << ')';
    }

    stream << " }";
    return stream.str();
  }

  // ===========================================================================
  // ===                  SCALAR BIJECTION IMPLEMENTATION                    ===
  // ===========================================================================

  // Default constructor: creates a bijection without association
  template < typename T1, typename T2 >
  INLINE BijectionImplementation< T1, T2, true >::BijectionImplementation(Size size,
                                                                          bool resize_policy) :
      // warning: below, we create the internal hashTables with a key
      // uniqueness
      // policy set to false because we will do the uniqueness tests ourselves
      // (this
      // will speed-up the process)
      _firstToSecond_(size, resize_policy, false),
      _secondToFirst_(size, resize_policy, false) {
    GUM_CONSTRUCTOR(BijectionImplementation);
  }

  // initializer list constructor
  template < typename T1, typename T2 >
  INLINE BijectionImplementation< T1, T2, true >::BijectionImplementation(
     std::initializer_list< std::pair< T1, T2 > > list) :
      _firstToSecond_(Size(list.size()) / 2, true, false),
      _secondToFirst_(Size(list.size()) / 2, true, false) {
    GUM_CONSTRUCTOR(BijectionImplementation);

    for (const auto& elt: list) {
      insert(elt.first, elt.second);
    }
  }

  // a function that performs a complete copy of another bijection
  template < typename T1, typename T2 >
  INLINE void BijectionImplementation< T1, T2, true >::_copy_(const HashTable< T1, T2 >& f2s) {
    // parse f2s and perform copies
    for (auto iter = f2s.cbegin(); iter != f2s.cend(); ++iter) {
      _firstToSecond_.insert(iter.key(), iter.val());

      try {
        _secondToFirst_.insert(iter.val(), iter.key());
      } catch (...) {
        _firstToSecond_.erase(iter.key());
        throw;
      }
    }

    // note that  _iter_end_ is actually a constant, whatever we add/remove
    // to/from  _firstToSecond_. As a consequence, it need not be updated
    // after  _copy_
  }

  // Copy constructor
  template < typename T1, typename T2 >
  INLINE BijectionImplementation< T1, T2, true >::BijectionImplementation(
     const BijectionImplementation< T1, T2, true >& toCopy) :
      _firstToSecond_(toCopy._firstToSecond_.capacity(), true, false),
      _secondToFirst_(toCopy._secondToFirst_.capacity(), true, false) {
    GUM_CONS_CPY(BijectionImplementation);
    _copy_(toCopy._firstToSecond_);
  }

  // move constructor
  template < typename T1, typename T2 >
  INLINE BijectionImplementation< T1, T2, true >::BijectionImplementation(
     BijectionImplementation< T1, T2, true >&& toCopy) noexcept :
      _firstToSecond_(std::move(toCopy._firstToSecond_)),
      _secondToFirst_(std::move(toCopy._secondToFirst_)) {
    GUM_CONS_MOV(BijectionImplementation);
  }

  // destructor
  template < typename T1, typename T2 >
  INLINE BijectionImplementation< T1, T2, true >::~BijectionImplementation() {
    GUM_DESTRUCTOR(BijectionImplementation);
  }

  // returns the iterator at the beginning of the bijection
  template < typename T1, typename T2 >
  INLINE typename BijectionImplementation< T1, T2, true >::iterator
     BijectionImplementation< T1, T2, true >::begin() const {
    return BijectionIterator< T1, T2 >{*this};
  }

  // returns the iterator at the beginning of the bijection
  template < typename T1, typename T2 >
  INLINE typename BijectionImplementation< T1, T2, true >::const_iterator
     BijectionImplementation< T1, T2, true >::cbegin() const {
    return BijectionIterator< T1, T2 >{*this};
  }

  // returns the iterator to the end of the bijection
  template < typename T1, typename T2 >
  INLINE const typename BijectionImplementation< T1, T2, true >::iterator&
     BijectionImplementation< T1, T2, true >::end() const noexcept {
    return *(reinterpret_cast< const BijectionIterator< T1, T2 >* >(_Bijection_end_));
  }

  // returns the iterator to the end of the bijection
  template < typename T1, typename T2 >
  INLINE const typename BijectionImplementation< T1, T2, true >::const_iterator&
     BijectionImplementation< T1, T2, true >::cend() const noexcept {
    return *(reinterpret_cast< const BijectionIterator< T1, T2 >* >(_Bijection_end_));
  }

  // returns the iterator at the beginning of the bijection
  template < typename T1, typename T2 >
  INLINE typename BijectionImplementation< T1, T2, true >::iterator_safe
     BijectionImplementation< T1, T2, true >::beginSafe() const {
    return BijectionIteratorSafe< T1, T2 >{*this};
  }

  // returns the iterator at the beginning of the bijection
  template < typename T1, typename T2 >
  INLINE typename BijectionImplementation< T1, T2, true >::const_iterator_safe
     BijectionImplementation< T1, T2, true >::cbeginSafe() const {
    return BijectionIteratorSafe< T1, T2 >{*this};
  }

  // returns the iterator to the end of the bijection
  template < typename T1, typename T2 >
  INLINE const typename BijectionImplementation< T1, T2, true >::iterator_safe&
     BijectionImplementation< T1, T2, true >::endSafe() const noexcept {
    return *(reinterpret_cast< const BijectionIteratorSafe< T1, T2 >* >(_Bijection_end_safe_));
  }

  // returns the iterator to the end of the bijection
  template < typename T1, typename T2 >
  INLINE const typename BijectionImplementation< T1, T2, true >::const_iterator_safe&
     BijectionImplementation< T1, T2, true >::cendSafe() const noexcept {
    return *(reinterpret_cast< const BijectionIteratorSafe< T1, T2 >* >(_Bijection_end_safe_));
  }

  // removes all the associations from the bijection
  template < typename T1, typename T2 >
  INLINE void BijectionImplementation< T1, T2, true >::clear() {
    _firstToSecond_.clear();
    _secondToFirst_.clear();
    // note that  _iter_end_ is actually a constant, whatever we add/remove
    // to/from  _firstToSecond_. As a consequence, it need not be updated
    // after the clear's
  }

  // Copy operator
  template < typename T1, typename T2 >
  INLINE BijectionImplementation< T1, T2, true >&
     BijectionImplementation< T1, T2, true >::operator=(
        const BijectionImplementation< T1, T2, true >& toCopy) {
    // avoid self assignment
    if (this != &toCopy) {
      clear();
      _copy_(toCopy._firstToSecond_);
    }

    // note that  _iter_end_ is actually a constant, whatever we add/remove
    // to/from  _firstToSecond_. As a consequence, it need not be updated
    // after  _copy_
    return *this;
  }

  // move operator
  template < typename T1, typename T2 >
  INLINE BijectionImplementation< T1, T2, true >&
     BijectionImplementation< T1, T2, true >::operator=(
        BijectionImplementation< T1, T2, true >&& toCopy) {
    // avoid self assignment
    if (this != &toCopy) {
      clear();
      _firstToSecond_ = std::move(toCopy._firstToSecond_);
      _secondToFirst_ = std::move(toCopy._secondToFirst_);
    }

    // note that  _iter_end_ is actually a constant, whatever we add/remove
    // to/from  _firstToSecond_. As a consequence, it need not be updated
    // after  _copy_
    return *this;
  }

  // returns the value associated to the element passed in argument
  template < typename T1, typename T2 >
  INLINE const T1& BijectionImplementation< T1, T2, true >::first(T2 second) const {
    return _secondToFirst_[second];
  }

  // returns the value associated to the element passed in argument
  template < typename T1, typename T2 >
  INLINE const T2& BijectionImplementation< T1, T2, true >::second(T1 first) const {
    return _firstToSecond_[first];
  }

  // Test whether the bijection contains the "first" value
  template < typename T1, typename T2 >
  INLINE bool BijectionImplementation< T1, T2, true >::existsFirst(T1 first) const {
    return _firstToSecond_.exists(first);
  }

  // Test whether the bijection contains the "second" value
  template < typename T1, typename T2 >
  INLINE bool BijectionImplementation< T1, T2, true >::existsSecond(T2 second) const {
    return _secondToFirst_.exists(second);
  }

  // inserts a new association in the bijection
  template < typename T1, typename T2 >
  INLINE void BijectionImplementation< T1, T2, true >::_insert_(T1 first, T2 second) {
    // check the uniqueness property
    if (existsFirst(first) || existsSecond(second)) {
      GUM_ERROR(DuplicateElement,
                "the bijection contains an element with the same couple (" << first << "," << second
                                                                           << ")");
    }

    // insert copies of first and second
    _firstToSecond_.insert(first, second);

    try {
      _secondToFirst_.insert(second, first);
    } catch (...) {
      _firstToSecond_.erase(first);
      throw;
    }
  }

  // inserts a new association in the bijection
  template < typename T1, typename T2 >
  INLINE void BijectionImplementation< T1, T2, true >::insert(T1 first, T2 second) {
    _insert_(first, second);
  }

  // emplace a new element in the bijection
  template < typename T1, typename T2 >
  template < typename... Args >
  INLINE void BijectionImplementation< T1, T2, true >::emplace(Args&&... args) {
    std::pair< T1, T2 > new_elt(std::forward< Args >(args)...);
    _insert_(new_elt.first, new_elt.second);
  }

  /* @brief Same method as first, but if the value is not found, a default
   * value is inserted into the bijection */
  template < typename T1, typename T2 >
  INLINE const T1& BijectionImplementation< T1, T2, true >::firstWithDefault(T2 second,
                                                                             T1 val) const {
    try {
      return first(second);
    } catch (NotFound const&) {
      _insert_(val, second);
      return val;
    }
  }

  /* @brief Same method as second, but if the value is not found, a default
   * value is inserted into the bijection */
  template < typename T1, typename T2 >
  INLINE const T2& BijectionImplementation< T1, T2, true >::secondWithDefault(T1 first,
                                                                              T2 val) const {
    try {
      return second(first);
    } catch (NotFound const&) {
      _insert_(first, val);
      return val;
    }
  }

  // returns true if the bijection doesn't contain any relation
  template < typename T1, typename T2 >
  INLINE bool BijectionImplementation< T1, T2, true >::empty() const noexcept {
    GUM_ASSERT(_firstToSecond_.empty() == _secondToFirst_.empty());
    return _firstToSecond_.empty();
  }

  // returns the number of associations stored within the bijection
  template < typename T1, typename T2 >
  INLINE Size BijectionImplementation< T1, T2, true >::size() const noexcept {
    GUM_ASSERT(_firstToSecond_.size() == _secondToFirst_.size());
    return _firstToSecond_.size();
  }

  // erases an association containing the given first element
  template < typename T1, typename T2 >
  INLINE void BijectionImplementation< T1, T2, true >::eraseFirst(T1 first) {
    try {
      _secondToFirst_.erase(_firstToSecond_[first]);
      _firstToSecond_.erase(first);
    } catch (NotFound const&) {}
  }

  // erase an association containing the given second element
  template < typename T1, typename T2 >
  INLINE void BijectionImplementation< T1, T2, true >::eraseSecond(T2 second) {
    try {
      _firstToSecond_.erase(_secondToFirst_[second]);
      _secondToFirst_.erase(second);
    } catch (NotFound const&) {}
  }

  // returns the number of hashtables' slots used (@sa hashTable's capacity)
  template < typename T1, typename T2 >
  INLINE Size BijectionImplementation< T1, T2, true >::capacity() const noexcept {
    return _firstToSecond_.capacity();
  }

  // similar to the hashtable's resize
  template < typename T1, typename T2 >
  INLINE void BijectionImplementation< T1, T2, true >::resize(Size new_size) {
    _firstToSecond_.resize(new_size);
    _secondToFirst_.resize(new_size);
  }

  // enables the user to change dynamically the resizing policy
  template < typename T1, typename T2 >
  INLINE void
     BijectionImplementation< T1, T2, true >::setResizePolicy(const bool new_policy) noexcept {
    _firstToSecond_.setResizePolicy(new_policy);
    _secondToFirst_.setResizePolicy(new_policy);
  }

  // returns the current resizing policy
  template < typename T1, typename T2 >
  INLINE bool BijectionImplementation< T1, T2, true >::resizePolicy() const noexcept {
    return _firstToSecond_.resizePolicy();
  }

  // friendly displays the content of the CliqueGraph
  template < typename T1, typename T2 >
  std::string BijectionImplementation< T1, T2, true >::toString() const {
    std::stringstream stream;
    stream << "{ ";
    bool first = true;

    for (iterator iter = begin(); iter != end(); ++iter) {
      if (!first) stream << ", ";
      else first = false;

      stream << '(' << iter.first() << " <-> " << iter.second() << ')';
    }

    stream << " }";
    return stream.str();
  }

  // ===========================================================================
  // ===                      BIJECTION SAFE ITERATORS                       ===
  // ===========================================================================

  /// Default constructor
  template < typename T1, typename T2 >
  INLINE BijectionIteratorSafe< T1, T2 >::BijectionIteratorSafe() noexcept {
    GUM_CONSTRUCTOR(BijectionIteratorSafe);
  }

  /// Constructor
  template < typename T1, typename T2 >
  template < bool Gen >
  INLINE BijectionIteratorSafe< T1, T2 >::BijectionIteratorSafe(
     const BijectionImplementation< T1, T2, Gen >& bijection) :
      _iter_{bijection._firstToSecond_.cbeginSafe()} {
    GUM_CONSTRUCTOR(BijectionIteratorSafe);
  }

  /// Constructor
  template < typename T1, typename T2 >
  INLINE
     BijectionIteratorSafe< T1, T2 >::BijectionIteratorSafe(const Bijection< T1, T2 >& bijection) :
      _iter_{bijection._firstToSecond_.cbeginSafe()} {
    GUM_CONSTRUCTOR(BijectionIteratorSafe);
  }

  /// Copy constructor
  template < typename T1, typename T2 >
  INLINE BijectionIteratorSafe< T1, T2 >::BijectionIteratorSafe(
     const BijectionIteratorSafe< T1, T2 >& toCopy) :
      _iter_{toCopy._iter_} {
    GUM_CONS_CPY(BijectionIteratorSafe);
  }

  /// move constructor
  template < typename T1, typename T2 >
  INLINE BijectionIteratorSafe< T1, T2 >::BijectionIteratorSafe(
     BijectionIteratorSafe< T1, T2 >&& from) noexcept :
      _iter_{std::move(from._iter_)} {
    GUM_CONS_MOV(BijectionIteratorSafe);
  }

  /// Destructor
  template < typename T1, typename T2 >
  INLINE BijectionIteratorSafe< T1, T2 >::~BijectionIteratorSafe() noexcept {
    GUM_DESTRUCTOR(BijectionIteratorSafe);
  }

  /// Copy operator
  template < typename T1, typename T2 >
  INLINE BijectionIteratorSafe< T1, T2 >&
     BijectionIteratorSafe< T1, T2 >::operator=(const BijectionIteratorSafe< T1, T2 >& toCopy) {
    _iter_ = toCopy._iter_;
    return *this;
  }

  /// move operator
  template < typename T1, typename T2 >
  INLINE BijectionIteratorSafe< T1, T2 >&
     BijectionIteratorSafe< T1, T2 >::operator=(BijectionIteratorSafe< T1, T2 >&& toCopy) noexcept {
    _iter_ = std::move(toCopy._iter_);
    return *this;
  }

  /// Go to the next association (if exists)
  template < typename T1, typename T2 >
  INLINE BijectionIteratorSafe< T1, T2 >& BijectionIteratorSafe< T1, T2 >::operator++() noexcept {
    ++_iter_;
    return *this;
  }

  /// moves the iterator by nb elements
  template < typename T1, typename T2 >
  INLINE BijectionIteratorSafe< T1, T2 >&
     BijectionIteratorSafe< T1, T2 >::operator+=(Size nb) noexcept {
    _iter_ += nb;
    return *this;
  }

  /// returns a new iterator
  template < typename T1, typename T2 >
  INLINE BijectionIteratorSafe< T1, T2 >
     BijectionIteratorSafe< T1, T2 >::operator+(Size nb) noexcept {
    return BijectionIteratorSafe< T1, T2 >{*this} += nb;
  }

  /// Comparison of iterators
  template < typename T1, typename T2 >
  INLINE bool BijectionIteratorSafe< T1, T2 >::operator!=(
     const BijectionIteratorSafe< T1, T2 >& toCompare) const noexcept {
    return _iter_ != toCompare._iter_;
  }

  /// Comparison of iterators
  template < typename T1, typename T2 >
  INLINE bool BijectionIteratorSafe< T1, T2 >::operator==(
     const BijectionIteratorSafe< T1, T2 >& toCompare) const noexcept {
    return _iter_ == toCompare._iter_;
  }

  /// return the first element of the current association
  template < typename T1, typename T2 >
  INLINE const T1& BijectionIteratorSafe< T1, T2 >::first() const {
    return _iter_.key();
  }

  /// return the second element of the current association
  template < typename T1, typename T2 >
  INLINE const T2& BijectionIteratorSafe< T1, T2 >::second() const {
    return Getter::op_second(_iter_.val());
  }

  /* ===========================================================================
   */
  /* ===                     BIJECTION UNSAFE ITERATORS                      ===
   */
  /* ===========================================================================
   */

  /// Default constructor
  template < typename T1, typename T2 >
  INLINE BijectionIterator< T1, T2 >::BijectionIterator() noexcept {
    GUM_CONSTRUCTOR(BijectionIterator);
  }

  /// Constructor
  template < typename T1, typename T2 >
  template < bool Gen >
  INLINE BijectionIterator< T1, T2 >::BijectionIterator(
     const BijectionImplementation< T1, T2, Gen >& bijection) :
      _iter_{bijection._firstToSecond_.cbegin()} {
    GUM_CONSTRUCTOR(BijectionIterator);
  }

  /// Constructor
  template < typename T1, typename T2 >
  INLINE BijectionIterator< T1, T2 >::BijectionIterator(const Bijection< T1, T2 >& bijection) :
      _iter_{bijection._firstToSecond_.cbegin()} {
    GUM_CONSTRUCTOR(BijectionIterator);
  }

  /// Copy constructor
  template < typename T1, typename T2 >
  INLINE BijectionIterator< T1, T2 >::BijectionIterator(const BijectionIterator< T1, T2 >& toCopy) :
      _iter_{toCopy._iter_} {
    GUM_CONS_CPY(BijectionIterator);
  }

  /// move constructor
  template < typename T1, typename T2 >
  INLINE BijectionIterator< T1, T2 >::BijectionIterator(BijectionIterator< T1, T2 >&& from) noexcept
      :
      _iter_{std::move(from._iter_)} {
    GUM_CONS_MOV(BijectionIterator);
  }

  /// Destructor
  template < typename T1, typename T2 >
  INLINE BijectionIterator< T1, T2 >::~BijectionIterator() noexcept {
    GUM_DESTRUCTOR(BijectionIterator);
  }

  /// Copy operator
  template < typename T1, typename T2 >
  INLINE BijectionIterator< T1, T2 >&
     BijectionIterator< T1, T2 >::operator=(const BijectionIterator< T1, T2 >& toCopy) {
    _iter_ = toCopy._iter_;
    return *this;
  }

  /// move operator
  template < typename T1, typename T2 >
  INLINE BijectionIterator< T1, T2 >&
     BijectionIterator< T1, T2 >::operator=(BijectionIterator< T1, T2 >&& toCopy) noexcept {
    _iter_ = std::move(toCopy._iter_);
    return *this;
  }

  /// Go to the next association (if exists)
  template < typename T1, typename T2 >
  INLINE BijectionIterator< T1, T2 >& BijectionIterator< T1, T2 >::operator++() noexcept {
    ++_iter_;
    return *this;
  }

  /// moves the iterator by nb elements
  template < typename T1, typename T2 >
  INLINE BijectionIterator< T1, T2 >& BijectionIterator< T1, T2 >::operator+=(Size nb) noexcept {
    _iter_ += nb;
    return *this;
  }

  /// returns a new iterator
  template < typename T1, typename T2 >
  INLINE BijectionIterator< T1, T2 > BijectionIterator< T1, T2 >::operator+(Size nb) noexcept {
    return BijectionIterator< T1, T2 >{*this} += nb;
  }

  /// Comparison of iterators
  template < typename T1, typename T2 >
  INLINE bool BijectionIterator< T1, T2 >::operator!=(
     const BijectionIterator< T1, T2 >& toCompare) const noexcept {
    return _iter_ != toCompare._iter_;
  }

  /// Comparison of iterators
  template < typename T1, typename T2 >
  INLINE bool BijectionIterator< T1, T2 >::operator==(
     const BijectionIterator< T1, T2 >& toCompare) const noexcept {
    return _iter_ == toCompare._iter_;
  }

  /// return the first element of the current association
  template < typename T1, typename T2 >
  INLINE const T1& BijectionIterator< T1, T2 >::first() const {
    return _iter_.key();
  }

  /// return the second element of the current association
  template < typename T1, typename T2 >
  INLINE const T2& BijectionIterator< T1, T2 >::second() const {
    return Getter::op_second(_iter_.val());
  }

  // ============================================================================
  // BIJECTION
  // ============================================================================

  // Default constructor: creates a bijection without any association
  template < typename T1, typename T2 >
  INLINE Bijection< T1, T2 >::Bijection(Size size, bool resize_policy) :
      BijectionImplementation< T1, T2, std::is_scalar< T1 >::value && std::is_scalar< T2 >::value >(
         size,
         resize_policy) {
    GUM_CONSTRUCTOR(Bijection);
  }

  // initializer list constructor
  template < typename T1, typename T2 >
  INLINE Bijection< T1, T2 >::Bijection(std::initializer_list< std::pair< T1, T2 > > list) :
      BijectionImplementation< T1, T2, std::is_scalar< T1 >::value && std::is_scalar< T2 >::value >(
         list) {
    GUM_CONSTRUCTOR(Bijection);
  }

  // Copy constructor
  template < typename T1, typename T2 >
  INLINE Bijection< T1, T2 >::Bijection(const Bijection< T1, T2 >& toCopy) :
      BijectionImplementation< T1, T2, std::is_scalar< T1 >::value && std::is_scalar< T2 >::value >(
         toCopy) {
    GUM_CONS_CPY(Bijection);
  }

  // move constructor
  template < typename T1, typename T2 >
  INLINE Bijection< T1, T2 >::Bijection(Bijection< T1, T2 >&& from) noexcept :
      BijectionImplementation< T1, T2, std::is_scalar< T1 >::value && std::is_scalar< T2 >::value >(
         std::move(from)) {
    GUM_CONS_MOV(Bijection);
  }

  // destructor
  template < typename T1, typename T2 >
  INLINE Bijection< T1, T2 >::~Bijection() {
    GUM_DESTRUCTOR(Bijection);
  }

  // copy operator
  template < typename T1, typename T2 >
  INLINE Bijection< T1, T2 >& Bijection< T1, T2 >::operator=(const Bijection< T1, T2 >& toCopy) {
    Implementation::operator=(toCopy);
    return *this;
  }

  // move operator
  template < typename T1, typename T2 >
  INLINE Bijection< T1, T2 >& Bijection< T1, T2 >::operator=(Bijection< T1, T2 >&& bij) {
    Implementation::operator=(std::move(bij));
    return *this;
  }

  // for friendly displaying the content of bijections
  template < typename T1, typename T2 >
  std::ostream& operator<<(std::ostream& stream, const Bijection< T1, T2 >& b) {
    stream << b.toString();
    return stream;
  }

} /* namespace gum */
