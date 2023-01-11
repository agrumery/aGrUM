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
 * @brief Implementation of the Set.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
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
  INLINE SetIteratorSafe< Key >::SetIteratorSafe(const Set< Key >& set, Position pos) :
      _ht_iter_{pos == SetIteratorSafe< Key >::END ? set._inside_.cendSafe()
                                                   : set._inside_.cbeginSafe()} {
    GUM_CONSTRUCTOR(SetIteratorSafe);
  }

  // copy constructor
  template < typename Key >
  INLINE SetIteratorSafe< Key >::SetIteratorSafe(const SetIteratorSafe< Key >& iter) :
      _ht_iter_{iter._ht_iter_} {
    GUM_CONS_CPY(SetIteratorSafe);
  }

  // copy constructor
  template < typename Key >
  INLINE SetIteratorSafe< Key >::SetIteratorSafe(const SetIterator< Key >& iter) :
      _ht_iter_{iter._ht_iter_} {
    GUM_CONS_CPY(SetIteratorSafe);
  }

  // move constructor
  template < typename Key >
  INLINE SetIteratorSafe< Key >::SetIteratorSafe(SetIteratorSafe< Key >&& from) :
      _ht_iter_{std::move(from._ht_iter_)} {
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
    _ht_iter_ = from._ht_iter_;
    return *this;
  }

  // assignment operator
  template < typename Key >
  INLINE SetIteratorSafe< Key >& SetIteratorSafe< Key >::operator=(const SetIterator< Key >& from) {
    _ht_iter_ = from._ht_iter_;
    return *this;
  }

  // move operator
  template < typename Key >
  INLINE SetIteratorSafe< Key >&
     SetIteratorSafe< Key >::operator=(SetIteratorSafe< Key >&& from) noexcept {
    _ht_iter_ = std::move(from._ht_iter_);
    return *this;
  }

  // increments the iterator
  template < typename Key >
  INLINE SetIteratorSafe< Key >& SetIteratorSafe< Key >::operator++() noexcept {
    // note that, if the hashtable's iterator points toward nothing, the
    // hashtable's iterator incrementation will do nothing. In particular, it
    // will not segfault.
    ++_ht_iter_;
    return *this;
  }

  // makes the iterator point to i elements further in the set
  template < typename Key >
  INLINE SetIteratorSafe< Key >& SetIteratorSafe< Key >::operator+=(Size nb) noexcept {
    _ht_iter_ += nb;
    return *this;
  }

  // returns a new iterator
  template < typename Key >
  INLINE SetIteratorSafe< Key > SetIteratorSafe< Key >::operator+(Size nb) const {
    return SetIteratorSafe< Key >{*this} += nb;
  }

  // indicates whether two iterators point to different elements or sets
  template < typename Key >
  INLINE bool
     SetIteratorSafe< Key >::operator!=(const SetIteratorSafe< Key >& from) const noexcept {
    return _ht_iter_ != from._ht_iter_;
  }

  // indicates whether two iterators point toward the same element of a same
  // set
  template < typename Key >
  INLINE bool
     SetIteratorSafe< Key >::operator==(const SetIteratorSafe< Key >& from) const noexcept {
    return _ht_iter_ == from._ht_iter_;
  }

  // returns the element pointed to by the iterator
  template < typename Key >
  INLINE const Key& SetIteratorSafe< Key >::operator*() const {
    // note that, if the hashtable's iterator points toward nothing, it will
    // raise an UndefinedIteratorValue exception
    return _ht_iter_.key();
  }

  // returns aointer to the element pointed to by the iterator
  template < typename Key >
  INLINE const Key* SetIteratorSafe< Key >::operator->() const {
    // note that, if the hashtable's iterator points toward nothing, it will
    // raise an UndefinedIteratorValue exception
    return &(_ht_iter_.key());
  }

  // @brief makes the iterator point toward nothing (in particular, it is not
  // related anymore to its current set) */
  template < typename Key >
  INLINE void SetIteratorSafe< Key >::clear() noexcept {
    _ht_iter_.clear();
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
  INLINE SetIterator< Key >::SetIterator(const Set< Key >& set, Position pos) :
      _ht_iter_{pos == SetIterator< Key >::END ? set._inside_.cend() : set._inside_.cbegin()} {
    GUM_CONSTRUCTOR(SetIterator);
  }

  // copy constructor
  template < typename Key >
  INLINE SetIterator< Key >::SetIterator(const SetIterator< Key >& iter) noexcept :
      _ht_iter_{iter._ht_iter_} {
    GUM_CONS_CPY(SetIterator);
  }

  // move constructor
  template < typename Key >
  INLINE SetIterator< Key >::SetIterator(SetIterator< Key >&& from) noexcept :
      _ht_iter_{std::move(from._ht_iter_)} {
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
    _ht_iter_ = from._ht_iter_;
    return *this;
  }

  // move operator
  template < typename Key >
  INLINE SetIterator< Key >& SetIterator< Key >::operator=(SetIterator< Key >&& from) noexcept {
    _ht_iter_ = std::move(from._ht_iter_);
    return *this;
  }

  // increments the iterator
  template < typename Key >
  INLINE SetIterator< Key >& SetIterator< Key >::operator++() noexcept {
    // note that, if the hashtable's iterator points toward nothing, the
    // hashtable's iterator incrementation will do nothing. In particular, it
    // will not segfault.
    ++_ht_iter_;
    return *this;
  }

  // makes the iterator point to i elements further in the set
  template < typename Key >
  INLINE SetIterator< Key >& SetIterator< Key >::operator+=(Size nb) noexcept {
    _ht_iter_ += nb;
    return *this;
  }

  // returns a new iterator
  template < typename Key >
  INLINE SetIterator< Key > SetIterator< Key >::operator+(Size nb) const noexcept {
    return SetIterator< Key >{*this} += nb;
  }

  // indicates whether two iterators point to different elements or sets
  template < typename Key >
  INLINE bool SetIterator< Key >::operator!=(const SetIterator< Key >& from) const noexcept {
    return _ht_iter_ != from._ht_iter_;
  }

  // indicates whether two iterators point toward the same element of a same
  // set
  template < typename Key >
  INLINE bool SetIterator< Key >::operator==(const SetIterator< Key >& from) const noexcept {
    return _ht_iter_ == from._ht_iter_;
  }

  // returns the element pointed to by the iterator
  template < typename Key >
  INLINE const Key& SetIterator< Key >::operator*() const {
    // note that, if the hashtable's iterator points toward nothing, it will
    // raise an UndefinedIteratorValue exception
    return _ht_iter_.key();
  }

  // returns aointer to the element pointed to by the iterator
  template < typename Key >
  INLINE const Key* SetIterator< Key >::operator->() const {
    // note that, if the hashtable's iterator points toward nothing, it will
    // raise an UndefinedIteratorValue exception
    return &(_ht_iter_.key());
  }

  // @brief makes the iterator point toward nothing (in particular, it is not
  // related anymore to its current set) */
  template < typename Key >
  INLINE void SetIterator< Key >::clear() noexcept {
    _ht_iter_.clear();
  }

  // ===========================================================================
  // ===                                 SETS                                ===
  // ===========================================================================

  // default constructor
  template < typename Key >
  INLINE Set< Key >::Set(Size capacity, bool resize_policy) :
      // create the hash table without key uniqueness policy (as we will
      // check
      // ourselves the uniqueness of Keys before inserting new elements)
      _inside_(capacity, resize_policy, false) {
    GUM_CONSTRUCTOR(Set);
  }

  // initializer list constructor
  template < typename Key >
  INLINE Set< Key >::Set(std::initializer_list< Key > list) :
      _inside_(Size(list.size()) / 2, true, false) {
    GUM_CONSTRUCTOR(Set);
    for (const auto& elt: list) {
      insert(elt);
    }
  }

  // copy constructor
  template < typename Key >
  INLINE Set< Key >::Set(const Set< Key >& s) : _inside_(s._inside_) {
    GUM_CONS_CPY(Set);
  }

  // move constructor
  template < typename Key >
  INLINE Set< Key >::Set(Set< Key >&& s) : _inside_(std::move(s._inside_)) {
    GUM_CONS_MOV(Set);
  }

  // destructor
  template < typename Key >
  INLINE Set< Key >::Set::~Set() {
    GUM_DESTRUCTOR(Set);
  }

  // removes all the elements, if any, from the set
  template < typename Key >
  INLINE void Set< Key >::clear() {
    // first we remove all the elements from the hashtable actually containing
    // the elements of the set. Note that, doing so, all the hashtable iterators
    // will be updated as well. In turn, this will imply that, whenever an
    // operation will be performed on a SetIteratorSafe, this will raise an
    // exception.
    _inside_.clear();

    // Note that actually there is no need to update the end iterator as this
    // one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }

  // copy operator
  template < typename Key >
  Set< Key >& Set< Key >::operator=(const Set< Key >& s) {
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
      _inside_ = s._inside_;

      // Note that actually there is no need to update the end iterator as this
      // one
      // is not affected by changes within hashtables (adding/deleting
      // elements).
      // Hence, for speedup, we do not update the end iterator
    }

    return *this;
  }

  // move operator
  template < typename Key >
  Set< Key >& Set< Key >::operator=(Set< Key >&& from) {
    _inside_ = std::move(from._inside_);
    return *this;
  }

  // mathematical equality between two sets
  template < typename Key >
  bool Set< Key >::operator==(const Set< Key >& s2) const {
    const HashTable< Key, bool >& h2 = s2._inside_;

    // check whether both sets have the same number of elements
    if (size() != h2.size()) return false;

    // check the content of the sets
    for (HashTableConstIterator< Key, bool > iter = _inside_.cbegin(); iter != _inside_.cend();
         ++iter) {
      if (!h2.exists(iter.key())) return false;
    }

    return true;
  }

  // mathematical inequality between two sets
  template < typename Key >
  INLINE bool Set< Key >::operator!=(const Set< Key >& s2) const {
    return !(operator==(s2));
  }

  // the usual begin iterator to parse the set
  template < typename Key >
  INLINE typename Set< Key >::iterator_safe Set< Key >::beginSafe() const {
    return SetIteratorSafe< Key >{*this};
  }

  // the usual begin iterator to parse the set
  template < typename Key >
  INLINE typename Set< Key >::const_iterator_safe Set< Key >::cbeginSafe() const {
    return SetIteratorSafe< Key >{*this};
  }

  // the usual end iterator to parse the set
  template < typename Key >
  INLINE const typename Set< Key >::iterator_safe& Set< Key >::endSafe() const noexcept {
    return *(reinterpret_cast< const SetIteratorSafe< Key >* >(_Set_end_safe_));
  }

  // the usual end iterator to parse the set
  template < typename Key >
  INLINE const typename Set< Key >::const_iterator_safe& Set< Key >::cendSafe() const noexcept {
    return *(reinterpret_cast< const SetIteratorSafe< Key >* >(_Set_end_safe_));
  }

  // the usual begin iterator to parse the set
  template < typename Key >
  INLINE typename Set< Key >::iterator Set< Key >::begin() const {
    return SetIterator< Key >{*this};
  }

  // the usual begin iterator to parse the set
  template < typename Key >
  INLINE typename Set< Key >::const_iterator Set< Key >::cbegin() const {
    return SetIterator< Key >{*this};
  }

  // the usual end iterator to parse the set
  template < typename Key >
  INLINE const typename Set< Key >::iterator& Set< Key >::end() const noexcept {
    return *(reinterpret_cast< const SetIterator< Key >* >(_Set_end_));
  }

  // the usual end iterator to parse the set
  template < typename Key >
  INLINE const typename Set< Key >::const_iterator& Set< Key >::cend() const noexcept {
    return *(reinterpret_cast< const SetIterator< Key >* >(_Set_end_));
  }

  // returns the size of the underlying hashtable containing the set
  template < typename Key >
  INLINE Size Set< Key >::capacity() const {
    return _inside_.capacity();
  }

  // changes the size of the underlying hashtable
  template < typename Key >
  INLINE void Set< Key >::resize(Size new_size) {
    _inside_.resize(new_size);

    // Note that actually there is no need to update the end iterator as this
    // one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }

  // enables the user to change dynamically the resizing policy of the
  // underlying hashtable
  template < typename Key >
  INLINE void Set< Key >::setResizePolicy(const bool new_policy) {
    _inside_.setResizePolicy(new_policy);

    // Note that actually there is no need to update the end iterator as this
    // one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }

  // returns the current resizing policy of the underlying hashtable
  template < typename Key >
  INLINE bool Set< Key >::resizePolicy() const {
    return _inside_.resizePolicy();
  }

  // indicates whether a given elements belong to the set
  template < typename Key >
  INLINE bool Set< Key >::contains(const Key& k) const {
    return _inside_.exists(k);
  }


  template < typename Key >
  INLINE bool Set< Key >::isProperSubsetOf(const Set< Key >& s) const {
    if (this->size() >= s.size()) { return false; }

    for (const auto& elt: *this) {
      if (!s.contains(elt)) { return false; }
    }
    return true;
  }

  template < typename Key >
  INLINE bool Set< Key >::isProperSupersetOf(const Set< Key >& s) const {
    return s.isProperSubsetOf(*this);
  }


  template < typename Key >
  INLINE bool Set< Key >::isSubsetOrEqual(const Set< Key >& s) const {
    if (this->size() > s.size()) { return false; }

    for (const auto& elt: *this) {
      if (!s.contains(elt)) { return false; }
    }
    return true;
  }

  template < typename Key >
  INLINE bool Set< Key >::isSupersetOrEqual(const Set< Key >& s) const {
    return s.isSubsetOrEqual(*this);
  }

  // indicates whether a given elements belong to the set
  template < typename Key >
  INLINE bool Set< Key >::exists(const Key& k) const {
    return _inside_.exists(k);
  }

  // inserts a new element in the set
  template < typename Key >
  INLINE void Set< Key >::insert(const Key& k) {
    // WARNING: we shall always test whether k already belongs to the set before
    // trying to insert it because we set  _inside_'s key uniqueness policy to
    // false
    if (!contains(k)) {
      // insert the element
      _inside_.insert(k, true);

      // Note that actually there is no need to update the end iterator as this
      // one
      // is not affected by changes within hashtables (adding/deleting
      // elements).
      // Hence, for speedup, we do not update the end iterator
    }
  }

  // inserts a new element in the set
  template < typename Key >
  INLINE void Set< Key >::insert(Key&& k) {
    // WARNING: we shall always test whether k already belongs to the set before
    // trying to insert it because we set  _inside_'s key uniqueness policy to
    // false
    if (!contains(k)) {
      // insert the element
      _inside_.insert(std::move(k), true);

      // Note that actually there is no need to update the end iterator as this
      // one
      // is not affected by changes within hashtables (adding/deleting
      // elements).
      // Hence, for speedup, we do not update the end iterator
    }
  }

  // emplace a new element in the set
  template < typename Key >
  template < typename... Args >
  INLINE void Set< Key >::emplace(Args&&... args) {
    insert(std::move(Key(std::forward< Args >(args)...)));
  }

  // erases an element from the set
  template < typename Key >
  INLINE void Set< Key >::erase(const Key& k) {
    // erase the element (if it exists)
    _inside_.erase(k);

    // Note that actually there is no need to update the end iterator as this
    // one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }

  // erases an element from the set
  template < typename Key >
  INLINE void Set< Key >::erase(const SetIteratorSafe< Key >& iter) {
    // erase the element
    _inside_.erase(iter._ht_iter_);

    // Note that actually there is no need to update the end iterator as this
    // one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }

  // adds a new element to the set
  template < typename Key >
  INLINE Set< Key >& Set< Key >::operator<<(const Key& k) {
    insert(k);
    return *this;
  }

  // adds a new element to the set
  template < typename Key >
  INLINE Set< Key >& Set< Key >::operator<<(Key&& k) {
    insert(std::move(k));
    return *this;
  }

  // removes an element from the set
  template < typename Key >
  INLINE Set< Key >& Set< Key >::operator>>(const Key& k) {
    erase(k);
    return *this;
  }

  // returns the number of elements in the set
  template < typename Key >
  INLINE Size Set< Key >::size() const noexcept {
    return _inside_.size();
  }

  // indicates whether the set is the empty set
  template < typename Key >
  INLINE bool Set< Key >::empty() const noexcept {
    return _inside_.empty();
  }

  // Intersection operator
  template < typename Key >
  Set< Key > Set< Key >::operator*(const Set< Key >& s2) const {
    Set< Key >                    res;
    const HashTable< Key, bool >& h2  = s2._inside_;
    HashTable< Key, bool >&       h_r = res._inside_;

    if (size() < h2.size()) {
      for (HashTableConstIterator< Key, bool > iter = _inside_.cbegin(); iter != _inside_.cend();
           ++iter) {
        if (h2.exists(iter.key())) h_r.insert(iter.key(), true);
      }
    } else {
      for (HashTableConstIterator< Key, bool > iter = h2.cbegin(); iter != h2.cend(); ++iter) {
        if (_inside_.exists(iter.key())) h_r.insert(iter.key(), true);
      }
    }

    return res;
  }


  // Intersection update operator
  template < typename Key >
  const Set< Key >& Set< Key >::operator*=(const Set< Key >& s2) {
    if (&s2 != this) {
      const HashTable< Key, bool >& h2 = s2._inside_;
      for (auto iter = _inside_.beginSafe(); iter != _inside_.endSafe(); ++iter) {
        if (!h2.exists(iter.key())) _inside_.erase(iter);
      }
    }

    return *this;
  }


  // Union update operator
  template < typename Key >
  const Set< Key >& Set< Key >::operator+=(const Set< Key >& s2) {
    if (&s2 != this) {
      for (auto pair: s2._inside_) {
        if (!_inside_.exists(pair.first)) _inside_.insert(pair.first, true);
      }
    }

    return *this;
  }


  // Union operator
  template < typename Key >
  Set< Key > Set< Key >::operator+(const Set< Key >& s2) const {
    Set< Key >                    res = *this;
    const HashTable< Key, bool >& h2  = s2._inside_;
    HashTable< Key, bool >&       h_r = res._inside_;

    for (HashTableConstIterator< Key, bool > iter = h2.cbegin(); iter != h2.cend(); ++iter) {
      if (!h_r.exists(iter.key())) h_r.insert(iter.key(), true);
    }

    return res;
  }


  // Disjunction operator
  template < typename Key >
  Set< Key > Set< Key >::operator-(const Set< Key >& s2) const {
    Set< Key >                    res;
    const HashTable< Key, bool >& h2  = s2._inside_;
    HashTable< Key, bool >&       h_r = res._inside_;

    for (HashTableConstIterator< Key, bool > iter = _inside_.cbegin(); iter != _inside_.cend();
         ++iter)
      if (!h2.exists(iter.key())) h_r.insert(iter.key(), true);

    return res;
  }

  // to display the content of the set
  template < typename Key >
  INLINE std::string Set< Key >::toString() const {
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
  template < typename Key >
  std::ostream& operator<<(std::ostream& stream, const Set< Key >& set) {
    stream << set.toString();
    return stream;
  }

  // creates a hashtable of NewKey from the set
  template < typename Key >
  template < typename NewKey >
  HashTable< Key, NewKey > Set< Key >::hashMap(NewKey (*f)(const Key&), Size size) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without resizing
    if (size == 0) size = std::max(Size(2), _inside_.size() / 2);

    // create a new table
    HashTable< Key, NewKey > table(size);

    // fill the new hash table
    for (HashTableConstIterator< Key, bool > iter = _inside_.cbegin(); iter != _inside_.cend();
         ++iter) {
      table.insert(iter.key(), f(iter.key()));
    }

    return table;
  }

  // creates a hashtable of NewKey from the set
  template < typename Key >
  template < typename NewKey >
  HashTable< Key, NewKey > Set< Key >::hashMap(const NewKey& val, Size size) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without resizing
    if (size == 0) size = std::max(Size(2), _inside_.size() / 2);

    // create a new table
    HashTable< Key, NewKey > table(size);

    // fill the new hash table
    for (HashTableConstIterator< Key, bool > iter = _inside_.cbegin(); iter != _inside_.cend();
         ++iter) {
      table.insert(iter.key(), val);
    }

    return table;
  }

  // a method to create a list of NewKey from the set
  template < typename Key >
  template < typename NewKey >
  List< NewKey > Set< Key >::listMap(NewKey (*f)(const Key&)) const {
    // create a new list
    List< NewKey > list;

    // fill the new list
    for (HashTableConstIterator< Key, bool > iter = _inside_.cbegin(); iter != _inside_.cend();
         ++iter) {
      list.pushBack(f(iter.key()));
    }

    return list;
  }

  // Returns the value of a key as a Size
  template < typename T >
  INLINE Size HashFunc< Set< T > >::castToSize(const Set< T >& key) {
    Size h = Size(0);
    for (const auto& k: key) {
      const auto hs = HashFunc< T >::castToSize(k);
      h += hs * (hs ^ HashFuncConst::gold);
    }

    return h;
  }


  // Returns the hashed value of a key.
  template < typename T >
  INLINE Size HashFunc< Set< T > >::operator()(const Set< T >& key) const {
    return (castToSize(key) * HashFuncConst::gold) & this->hash_mask_;
  }

} /* namespace gum */
