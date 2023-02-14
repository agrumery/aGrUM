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
 * @brief Template implementation file of gum::Sequence, a class for storing
 * (ordered) sequences of objects.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// to ease IDE parser
#include <agrum/tools/core/sequence.h>

namespace gum {

  // returns the size of the sequence
  template < typename Key, bool Gen >
  INLINE Size SequenceImplementation< Key, Gen >::size() const noexcept {
    return _h_.size();
  }

  // return true if empty
  template < typename Key, bool Gen >
  INLINE bool SequenceImplementation< Key, Gen >::empty() const noexcept {
    return _h_.empty();
  }

  // returns the size of the sequence
  template < typename Key >
  INLINE Size SequenceImplementation< Key, true >::size() const noexcept {
    return _h_.size();
  }

  // return true if empty
  template < typename Key >
  INLINE bool SequenceImplementation< Key, true >::empty() const noexcept {
    return _h_.empty();
  }

  // ===========================================================================
  // class SequenceIteratorSafe
  // ===========================================================================

  // default constructor
  template < typename Key >
  template < bool Gen >
  INLINE SequenceIteratorSafe< Key >::SequenceIteratorSafe(
     const SequenceImplementation< Key, Gen >& seq,
     Idx                                       pos) noexcept :
      _seq_{reinterpret_cast< const SequenceImplementation< Key, std::is_scalar< Key >::value >* >(
         &seq)} {
    GUM_CONSTRUCTOR(SequenceIteratorSafe);

    if (pos > _seq_->size()) _iterator_ = _seq_->size();   // make the iterator point to end
    else _iterator_ = pos;
  }

  // default constructor
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >::SequenceIteratorSafe(const Sequence< Key >& seq,
                                                           Idx                    pos) noexcept :
      _seq_{reinterpret_cast< const SequenceImplementation< Key, std::is_scalar< Key >::value >* >(
         &seq)} {
    GUM_CONSTRUCTOR(SequenceIteratorSafe);

    if (pos > _seq_->size()) _iterator_ = _seq_->size();   // make the iterator point to end
    else _iterator_ = pos;
  }

  // copy constructor
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >::SequenceIteratorSafe(
     const SequenceIteratorSafe< Key >& source) noexcept :
      _iterator_{source._iterator_},
      _seq_{source._seq_} {
    GUM_CONS_CPY(SequenceIteratorSafe);
  }

  // move constructor
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >::SequenceIteratorSafe(
     SequenceIteratorSafe< Key >&& source) noexcept :
      _iterator_{source._iterator_},
      _seq_{source._seq_} {
    GUM_CONS_MOV(SequenceIteratorSafe);
  }

  // destructor
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >::~SequenceIteratorSafe() noexcept {
    GUM_DESTRUCTOR(SequenceIteratorSafe);
  }

  // copy operator
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >&
     SequenceIteratorSafe< Key >::operator=(const SequenceIteratorSafe< Key >& source) noexcept {
    _iterator_ = source._iterator_;
    _seq_      = source._seq_;
    return *this;
  }

  // move operator
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >&
     SequenceIteratorSafe< Key >::operator=(SequenceIteratorSafe< Key >&& source) noexcept {
    _iterator_ = source._iterator_;
    _seq_      = source._seq_;
    return *this;
  }

  // point the iterator to the next value in the sequence
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >& SequenceIteratorSafe< Key >::operator++() noexcept {
    if (_iterator_ < _seq_->size()) ++_iterator_;
    else _iterator_ = _seq_->size();

    return *this;
  }

  // point the iterator to the preceding value in the sequence
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >& SequenceIteratorSafe< Key >::operator--() noexcept {
    if (_iterator_ != std::numeric_limits< Idx >::max()) --_iterator_;

    return *this;
  }

  // makes the iterator point to i elements further in the sequence
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >& SequenceIteratorSafe< Key >::operator+=(Size nb) noexcept {
    if (_iterator_ == std::numeric_limits< Idx >::max()) return *this;
    _iterator_ += nb;
    if (_iterator_ > _seq_->size()) _iterator_ = _seq_->size();

    return *this;
  }

  // makes the iterator point to i elements further in the sequence
  template < typename Key >
  INLINE SequenceIteratorSafe< Key >& SequenceIteratorSafe< Key >::operator-=(Size nb) noexcept {
    if (_iterator_ == std::numeric_limits< Idx >::max()) return *this;
    _iterator_ -= nb;
    if (_iterator_ > _seq_->size()) _iterator_ = std::numeric_limits< Idx >::max();

    return *this;
  }

  // returns a new iterator
  template < typename Key >
  INLINE SequenceIteratorSafe< Key > SequenceIteratorSafe< Key >::operator+(Size nb) noexcept {
    return SequenceIteratorSafe< Key >{*this} += nb;
  }

  // returns a new iterator
  template < typename Key >
  INLINE SequenceIteratorSafe< Key > SequenceIteratorSafe< Key >::operator-(Size nb) noexcept {
    return SequenceIteratorSafe< Key >{*this} -= nb;
  }

  // checks whether two iterators are pointing to the same element
  template < typename Key >
  INLINE bool SequenceIteratorSafe< Key >::operator==(
     const SequenceIteratorSafe< Key >& source) const noexcept {
    if (_seq_->empty()) return true;   // all iterators are the same if seq is empty

    if ((_iterator_ != source._iterator_) || (_seq_ != source._seq_)) return false;

    return true;
  }

  // checks whether two iterators are pointing to different elements
  template < typename Key >
  INLINE bool SequenceIteratorSafe< Key >::operator!=(
     const SequenceIteratorSafe< Key >& source) const noexcept {
    return !operator==(source);
  }

  // returns the position of the iterator in the sequence
  template < typename Key >
  INLINE Idx SequenceIteratorSafe< Key >::pos() const {
    if (_iterator_ >= _seq_->size()) {
      GUM_ERROR(UndefinedIteratorValue, "iterator is end() or rend()")
    }

    return _iterator_;
  }

  // the iterator points to the posth element (0 = beginning of the sequence).
  template < typename Key >
  INLINE void SequenceIteratorSafe< Key >::_setPos_(Idx pos) noexcept {
    if (pos > _seq_->size()) _iterator_ = _seq_->size();
    else _iterator_ = pos;
  }

  // the iterator points to the posth element (0 = beginning of the sequence).
  template < typename Key >
  INLINE void SequenceIteratorSafe< Key >::_setAtRend_() noexcept {
    _iterator_ = std::numeric_limits< Idx >::max();
  }

  // the iterator points to the posth element (0 = beginning of the sequence).
  template < typename Key >
  INLINE void SequenceIteratorSafe< Key >::_setAtEnd_() noexcept {
    _iterator_ = _seq_->size();
  }

  // returns the value pointed to by the iterator
  template < typename Key >
  INLINE const Key& SequenceIteratorSafe< Key >::operator*() const {
    return Getter::op_star(_seq_->_v_[pos()]);
  }

  // dereferences the value pointed to by the iterator
  template < typename Key >
  INLINE const Key* SequenceIteratorSafe< Key >::operator->() const {
    return Getter::op_arrow(_seq_->_v_[pos()]);
  }

  // ===========================================================================
  // ===                 NON SCALAR GUM SEQUENCE IMPLEMENTATION              ===
  // ===========================================================================

  // updates const iterators
  template < typename Key, bool Gen >
  INLINE void SequenceImplementation< Key, Gen >::_update_end_() noexcept {
    _end_safe_._setAtEnd_();
  }

  // clear the sequence
  template < typename Key, bool Gen >
  INLINE void SequenceImplementation< Key, Gen >::clear() {
    _h_.clear();
    _v_.clear();
    _update_end_();
  }

  // clears the current sequence and fill it with copies the element of aSeq
  template < typename Key, bool Gen >
  INLINE void
     SequenceImplementation< Key, Gen >::_copy_(const SequenceImplementation< Key, Gen >& aSeq) {
    clear();

    for (Size i = 0; i < aSeq.size(); ++i) {
      Key& new_key = const_cast< Key& >(_h_.insert(*(aSeq._v_[i]), i).first);
      _v_.push_back(&new_key);
    }

    _update_end_();
  }

  // Default constructor
  template < typename Key, bool Gen >
  INLINE SequenceImplementation< Key, Gen >::SequenceImplementation(Size size_param) :
      _h_(size_param), _end_safe_{*this}, _rend_safe_{*this} {
    GUM_CONSTRUCTOR(SequenceImplementation);
    _rend_safe_._setAtRend_();
    _update_end_();
  }

  // initializer list constructor
  template < typename Key, bool Gen >
  INLINE
     SequenceImplementation< Key, Gen >::SequenceImplementation(std::initializer_list< Key > list) :
      _end_safe_{*this},
      _rend_safe_{*this} {
    GUM_CONSTRUCTOR(SequenceImplementation);
    _rend_safe_._setAtRend_();
    for (const auto& elt: list) {
      insert(elt);   // performs the  _update_end_ ()
    }
  }

  // copy constructor
  template < typename Key, bool Gen >
  INLINE SequenceImplementation< Key, Gen >::SequenceImplementation(
     const SequenceImplementation< Key, Gen >& aSeq) :
      _end_safe_{*this},
      _rend_safe_{*this} {
    GUM_CONS_CPY(SequenceImplementation);
    _rend_safe_._setAtRend_();
    _copy_(aSeq);   // performs the  _update_end_ ()
  }

  // move constructor
  template < typename Key, bool Gen >
  INLINE SequenceImplementation< Key, Gen >::SequenceImplementation(
     SequenceImplementation< Key, Gen >&& aSeq) :
      _h_(std::move(aSeq._h_)),
      _v_(std::move(aSeq._v_)), _end_safe_{*this}, _rend_safe_{*this} {
    GUM_CONS_MOV(SequenceImplementation);
    _rend_safe_._setAtRend_();
    _update_end_();
  }

  // destructor
  template < typename Key, bool Gen >
  INLINE SequenceImplementation< Key, Gen >::~SequenceImplementation() noexcept {
    GUM_DESTRUCTOR(SequenceImplementation);
  }

  // copy operator
  template < typename Key, bool Gen >
  INLINE SequenceImplementation< Key, Gen >&
     SequenceImplementation< Key, Gen >::operator=(const SequenceImplementation< Key, Gen >& aSeq) {
    // avoid self assignment
    if (&aSeq != this) {
      _copy_(aSeq);   // performs the  _update_end_ ()
    }

    return *this;
  }

  // move operator
  template < typename Key, bool Gen >
  INLINE SequenceImplementation< Key, Gen >&
     SequenceImplementation< Key, Gen >::operator=(SequenceImplementation< Key, Gen >&& aSeq) {
    // avoid self assignment
    if (&aSeq != this) {
      _h_ = std::move(aSeq._h_);
      _v_ = std::move(aSeq._v_);
      _update_end_();
    }

    return *this;
  }

  // check the existence of k in the sequence
  template < typename Key, bool Gen >
  INLINE bool SequenceImplementation< Key, Gen >::exists(const Key& k) const {
    return _h_.exists(k);
  }

  // insert an element at the end of the sequence
  template < typename Key, bool Gen >
  INLINE void SequenceImplementation< Key, Gen >::insert(const Key& k) {
    // k will be added at the end. Insert the new key into the hashtable
    Key& new_key = const_cast< Key& >(_h_.insert(k, _h_.size()).first);
    _v_.push_back(&new_key);
    _update_end_();
  }

  // insert an element at the end of the sequence
  template < typename Key, bool Gen >
  INLINE void SequenceImplementation< Key, Gen >::insert(Key&& k) {
    // k will be added at the end. Insert the new key into the hashtable
    Key& new_key = const_cast< Key& >(_h_.insert(std::move(k), _h_.size()).first);
    _v_.push_back(&new_key);
    _update_end_();
  }

  // emplace a new element in the sequence
  template < typename Key, bool Gen >
  template < typename... Args >
  INLINE void SequenceImplementation< Key, Gen >::emplace(Args&&... args) {
    Key  key(std::forward< Args >(args)...);
    Key& new_key = const_cast< Key& >(_h_.insert(std::move(key), _h_.size()).first);
    _v_.push_back(&new_key);
    _update_end_();
  }

  // insert k in the sequence (synonym for insert)
  template < typename Key, bool Gen >
  INLINE SequenceImplementation< Key, Gen >&
     SequenceImplementation< Key, Gen >::operator<<(const Key& k) {
    insert(k);
    return *this;
  }

  // insert k in the sequence (synonym for insert)
  template < typename Key, bool Gen >
  INLINE SequenceImplementation< Key, Gen >&
     SequenceImplementation< Key, Gen >::operator<<(Key&& k) {
    insert(std::move(k));
    return *this;
  }

  // remove an element from the sequence
  template < typename Key, bool Gen >
  INLINE void SequenceImplementation< Key, Gen >::erase(const Key& k) {
    // get the position of the element to remove
    Idx pos;

    try {
      pos = _h_[k];
    } catch (NotFound const&) { return; }

    // erase the element
    _v_.erase(_v_.begin() + pos);
    for (Idx i = pos, nb_elts = _h_.size() - 1; i < nb_elts; ++i) {
      --_h_[*(_v_[i])];
    }
    _h_.erase(k);

    _update_end_();
  }

  // remove from the sequence the element pointed to by the iterator
  template < typename Key, bool Gen >
  INLINE void SequenceImplementation< Key, Gen >::erase(const iterator_safe& iter) {
    if (iter.pos() >= size()) return;

    // erase the element
    Idx  pos = iter.pos();
    Key* key = _v_[pos];
    _v_.erase(_v_.begin() + pos);

    for (Idx i = pos, nb_elts = _h_.size() - 1; i < nb_elts; ++i) {
      --_h_[*(_v_[i])];
    }
    _h_.erase(*key);

    _update_end_();
  }

  // remove k in the sequence (synonym for erase)
  template < typename Key, bool Gen >
  INLINE SequenceImplementation< Key, Gen >&
     SequenceImplementation< Key, Gen >::operator>>(const Key& k) {
    erase(k);
    return *this;
  }

  // returns the object at position i ( first elt = index 0 )
  template < typename Key, bool Gen >
  INLINE const Key& SequenceImplementation< Key, Gen >::atPos(Idx i) const {
    if (i >= _h_.size()) {
      GUM_ERROR(OutOfBounds, "index " << i << " for a sequence of size" << _h_.size())
    }

    return *(_v_[i]);
  }

  // returns the element at position i (synonym for atPos)
  template < typename Key, bool Gen >
  INLINE const Key& SequenceImplementation< Key, Gen >::operator[](Idx i) const {
    return atPos(i);
  }

  // returns the position of the object passed in argument (if it exists)
  template < typename Key, bool Gen >
  INLINE Idx SequenceImplementation< Key, Gen >::pos(const Key& key) const {
    return _h_[key];
  }

  // inserts and returns the object at the pos i
  template < typename Key, bool Gen >
  INLINE void SequenceImplementation< Key, Gen >::setAtPos(Idx i, const Key& newKey) {
    if (i >= _h_.size()) { GUM_ERROR(NotFound, "index too large") }

    Key& new_key = const_cast< Key& >(_h_.insert(newKey, i).first);
    _h_.erase(*(_v_[i]));
    _v_[i] = &new_key;
  }

  // inserts and returns the object at the pos i
  template < typename Key, bool Gen >
  INLINE void SequenceImplementation< Key, Gen >::setAtPos(Idx i, Key&& newKey) {
    if (i >= _h_.size()) { GUM_ERROR(NotFound, "index too large") }

    Key& new_key = const_cast< Key& >(_h_.insert(std::move(newKey), i).first);
    _h_.erase(*(_v_[i]));
    _v_[i] = &new_key;
  }

  // replace two elements in the sequence
  template < typename Key, bool Gen >
  INLINE void SequenceImplementation< Key, Gen >::swap(Idx i, Idx j) {
    if (i == j) return;

    Key& ki = const_cast< Key& >(atPos(i));
    Key& kj = const_cast< Key& >(atPos(j));

    _h_[ki] = j;
    _h_[kj] = i;

    _v_[i] = &kj;
    _v_[j] = &ki;
  }

  // returns the first element
  template < typename Key, bool Gen >
  INLINE const Key& SequenceImplementation< Key, Gen >::front() const {
    return atPos(0);
  }

  // returns the last element
  template < typename Key, bool Gen >
  INLINE const Key& SequenceImplementation< Key, Gen >::back() const {
    return atPos(size() - 1);
  }

  // Print a sequence
  template < typename Key, bool Gen >
  std::string SequenceImplementation< Key, Gen >::toString() const {
    std::stringstream stream;
    stream << "[";

    if (!_h_.empty()) {
      stream << 0 << ":" << *_v_[0];

      for (Idx i = 1; i < _h_.size(); ++i) {
        stream << " - " << i << ":" << *_v_[i];
      }
    }

    stream << "]";

    std::string res = stream.str();
    return res;
  }

  // returns true if the content of k equals that of *this
  template < typename Key, bool Gen >
  bool SequenceImplementation< Key, Gen >::operator==(
     const SequenceImplementation< Key, Gen >& k) const {
    if (size() != k.size()) return false;
    else {
      for (Idx i = 0; i < size(); ++i)
        if (*_v_[i] != *(k._v_[i])) return false;
    }

    return true;
  }

  // returns true if the content of k is different from that of *this
  template < typename Key, bool Gen >
  INLINE bool SequenceImplementation< Key, Gen >::operator!=(
     const SequenceImplementation< Key, Gen >& k) const {
    return !operator==(k);
  }

  // a << operator for displaying the content of the Sequence
  template < typename Key, bool Gen >
  INLINE std::ostream& operator<<(std::ostream&                             stream,
                                  const SequenceImplementation< Key, Gen >& seq) {
    stream << seq.toString();
    return stream;
  }

  // returns the safe begin iterator
  template < typename Key, bool Gen >
  INLINE SequenceIteratorSafe< Key > SequenceImplementation< Key, Gen >::beginSafe() const {
    return SequenceIteratorSafe< Key >{*this};
  }

  // returns the safe end iterator
  template < typename Key, bool Gen >
  INLINE const SequenceIteratorSafe< Key >&
               SequenceImplementation< Key, Gen >::endSafe() const noexcept {
    return _end_safe_;
  }

  // return an iterator pointing to the last element
  template < typename Key, bool Gen >
  INLINE SequenceIteratorSafe< Key > SequenceImplementation< Key, Gen >::rbeginSafe() const {
    SequenceIteratorSafe< Key > it{*this};
    it._setPos_(size() - 1);
    return it;
  }

  // returns an iterator pointing just before the first element
  template < typename Key, bool Gen >
  INLINE const SequenceIteratorSafe< Key >&
               SequenceImplementation< Key, Gen >::rendSafe() const noexcept {
    return _rend_safe_;
  }

  // returns the unsafe begin iterator
  template < typename Key, bool Gen >
  INLINE SequenceIterator< Key > SequenceImplementation< Key, Gen >::begin() const {
    return SequenceIterator< Key >{*this};
  }

  // returns the unsafe end iterator
  template < typename Key, bool Gen >
  INLINE const SequenceIterator< Key >& SequenceImplementation< Key, Gen >::end() const noexcept {
    return _end_safe_;
  }

  // return an iterator pointing to the last element
  template < typename Key, bool Gen >
  INLINE SequenceIterator< Key > SequenceImplementation< Key, Gen >::rbegin() const {
    SequenceIterator< Key > it{*this};
    it._setPos_(size() - 1);
    return it;
  }

  // returns an iterator pointing just before the first element
  template < typename Key, bool Gen >
  INLINE const SequenceIterator< Key >& SequenceImplementation< Key, Gen >::rend() const noexcept {
    return _rend_safe_;
  }

  // modifies the size of the internal structures of the sequence
  template < typename Key, bool Gen >
  INLINE void SequenceImplementation< Key, Gen >::resize(Size new_size) {
    if (new_size < _h_.size()) return;

    _h_.resize(new_size);
    _v_.reserve(new_size);
  }

  // ===========================================================================
  // ===                   SCALAR GUM SEQUENCE IMPLEMENTATION                ===
  // ===========================================================================

  // updates the end iterators
  template < typename Key >
  INLINE void SequenceImplementation< Key, true >::_update_end_() noexcept {
    _end_safe_._setAtEnd_();
  }

  // clear the sequence
  template < typename Key >
  INLINE void SequenceImplementation< Key, true >::clear() {
    _h_.clear();
    _v_.clear();
    _update_end_();
  }

  // clears the current sequence and fill it with copies the element of aSeq
  template < typename Key >
  INLINE void
     SequenceImplementation< Key, true >::_copy_(const SequenceImplementation< Key, true >& aSeq) {
    clear();

    for (Size i = 0; i < aSeq.size(); ++i) {
      _h_.insert(aSeq._v_[i], i);
      _v_.push_back(aSeq._v_[i]);
    }

    _update_end_();
  }

  // Default constructor
  template < typename Key >
  INLINE SequenceImplementation< Key, true >::SequenceImplementation(Size size_param) :
      _h_(size_param), _end_safe_{*this}, _rend_safe_{*this} {
    GUM_CONSTRUCTOR(SequenceImplementation);
    _rend_safe_._setAtRend_();
    _end_safe_._setAtEnd_();
  }

  // initializer list constructor
  template < typename Key >
  INLINE SequenceImplementation< Key, true >::SequenceImplementation(
     std::initializer_list< Key > list) :
      _end_safe_{*this},
      _rend_safe_{*this} {
    GUM_CONSTRUCTOR(SequenceImplementation);
    _rend_safe_._setAtRend_();
    for (const auto& elt: list) {
      insert(elt);
    }
  }

  // copy constructor
  template < typename Key >
  SequenceImplementation< Key, true >::SequenceImplementation(
     const SequenceImplementation< Key, true >& aSeq) :
      _h_(aSeq._h_),
      _v_(aSeq._v_), _end_safe_{*this}, _rend_safe_{*this} {
    GUM_CONS_CPY(SequenceImplementation);
    _rend_safe_._setAtRend_();
    _end_safe_._setAtEnd_();
  }

  // move constructor
  template < typename Key >
  INLINE SequenceImplementation< Key, true >::SequenceImplementation(
     SequenceImplementation< Key, true >&& aSeq) :
      _h_(std::move(aSeq._h_)),
      _v_(std::move(aSeq._v_)), _end_safe_{*this}, _rend_safe_{*this} {
    GUM_CONS_MOV(SequenceImplementation);
    _rend_safe_._setAtRend_();
    _end_safe_._setAtEnd_();
  }

  // destructor
  template < typename Key >
  SequenceImplementation< Key, true >::~SequenceImplementation() noexcept {
    GUM_DESTRUCTOR(SequenceImplementation);
  }

  // copy operator
  template < typename Key >
  INLINE SequenceImplementation< Key, true >& SequenceImplementation< Key, true >::operator=(
     const SequenceImplementation< Key, true >& aSeq) {
    // avoid self assignment
    if (&aSeq != this) { _copy_(aSeq); }

    return *this;
  }

  // move operator
  template < typename Key >
  INLINE SequenceImplementation< Key, true >&
     SequenceImplementation< Key, true >::operator=(SequenceImplementation< Key, true >&& aSeq) {
    // avoid self assignment
    if (&aSeq != this) {
      _h_ = std::move(aSeq._h_);
      _v_ = std::move(aSeq._v_);
      _update_end_();
    }

    return *this;
  }

  // check the existence of k in the sequence
  template < typename Key >
  INLINE bool SequenceImplementation< Key, true >::exists(Key k) const {
    return _h_.exists(k);
  }

  // insert an element at the end of the sequence
  template < typename Key >
  INLINE void SequenceImplementation< Key, true >::insert(Key k) {
    // k will be added at the end. Insert the new key into the hashtable
    _h_.insert(k, _h_.size());
    _v_.push_back(k);
    _update_end_();
  }

  // emplace a new element in the sequence
  template < typename Key >
  template < typename... Args >
  INLINE void SequenceImplementation< Key, true >::emplace(Args&&... args) {
    Key new_key(std::forward< Args >(args)...);
    _h_.insert(new_key, _h_.size());
    _v_.push_back(new_key);
    _update_end_();
  }

  // insert k in the sequence (synonym for insert)
  template < typename Key >
  INLINE SequenceImplementation< Key, true >&
     SequenceImplementation< Key, true >::operator<<(Key k) {
    insert(k);
    return *this;
  }

  // remove an element from the sequence
  template < typename Key >
  INLINE void SequenceImplementation< Key, true >::erase(Key k) {
    // get the position of the element to remove
    Idx pos;

    try {
      pos = _h_[k];
    } catch (NotFound const&) { return; }

    // erase the element
    _v_.erase(_v_.begin() + pos);
    for (Idx i = pos, nb_elts = _h_.size() - 1; i < nb_elts; ++i) {
      --_h_[_v_[i]];
    }
    _h_.erase(k);

    _update_end_();
  }

  // remove from the sequence the element pointed to by the iterator
  template < typename Key >
  INLINE void SequenceImplementation< Key, true >::erase(const iterator_safe& iter) {
    if (iter.pos() >= size()) return;

    // erase the element
    Idx pos = iter.pos();
    Key key = _v_[pos];
    _v_.erase(_v_.begin() + pos);

    for (Idx i = pos, nb_elts = _h_.size() - 1; i < nb_elts; ++i) {
      --_h_[_v_[i]];
    }
    _h_.erase(key);

    _update_end_();
  }

  // remove k in the sequence (synonym for erase)
  template < typename Key >
  INLINE SequenceImplementation< Key, true >&
     SequenceImplementation< Key, true >::operator>>(Key k) {
    erase(k);
    return *this;
  }

  // returns the object at position i
  template < typename Key >
  INLINE const Key& SequenceImplementation< Key, true >::atPos(Idx i) const {
    if (i >= _h_.size()) { GUM_ERROR(NotFound, "not enough elements in the sequence") }

    return _v_[i];
  }

  // returns the element at position i (synonym for atPos)
  template < typename Key >
  INLINE const Key& SequenceImplementation< Key, true >::operator[](Idx i) const {
    return atPos(i);
  }

  // returns the position of the object passed in argument (if it exists)
  template < typename Key >
  INLINE Idx SequenceImplementation< Key, true >::pos(Key key) const {
    return _h_[key];
  }

  // sets the object at position i
  template < typename Key >
  INLINE void SequenceImplementation< Key, true >::setAtPos(Idx i, Key newKey) {
    if (i >= _h_.size()) { GUM_ERROR(NotFound, "index too large") }

    _h_.insert(newKey, i);
    _h_.erase(_v_[i]);
    _v_[i] = newKey;
  }

  // replace two elements in the sequence
  template < typename Key >
  INLINE void SequenceImplementation< Key, true >::swap(Idx i, Idx j) {
    if (i == j) return;

    Key ki = atPos(i);
    Key kj = atPos(j);

    _h_[ki] = j;
    _h_[kj] = i;

    _v_[i] = kj;
    _v_[j] = ki;
  }

  // returns the first element
  template < typename Key >
  INLINE const Key& SequenceImplementation< Key, true >::front() const {
    return atPos(0);
  }

  // returns the last element
  template < typename Key >
  INLINE const Key& SequenceImplementation< Key, true >::back() const {
    return atPos(size() - 1);
  }

  // Print a sequence
  template < typename Key >
  std::string SequenceImplementation< Key, true >::toString() const {
    std::stringstream stream;
    stream << "[";

    if (!_h_.empty()) {
      stream << 0 << ":" << _v_[0];

      for (Idx i = 1; i < _h_.size(); ++i) {
        stream << " - " << i << ":" << _v_[i];
      }
    }

    stream << "]";

    std::string res = stream.str();
    return res;
  }

  // returns true if the content of k equals that of *this
  template < typename Key >
  bool SequenceImplementation< Key, true >::operator==(
     const SequenceImplementation< Key, true >& k) const {
    if (size() != k.size()) return false;
    else {
      for (Idx i = 0; i < size(); ++i)
        if (_v_[i] != k._v_[i]) return false;
    }

    return true;
  }

  // returns true if the content of k is different from that of *this
  template < typename Key >
  INLINE bool SequenceImplementation< Key, true >::operator!=(
     const SequenceImplementation< Key, true >& k) const {
    return !operator==(k);
  }

  // a << operator for displaying the content of the Sequence
  template < typename Key >
  INLINE std::ostream& operator<<(std::ostream&                              stream,
                                  const SequenceImplementation< Key, true >& seq) {
    stream << seq.toString();
    return stream;
  }

  // returns the safe begin iterator
  template < typename Key >
  INLINE SequenceIteratorSafe< Key > SequenceImplementation< Key, true >::beginSafe() const {
    return SequenceIteratorSafe< Key >{*this};
  }

  // return the safe end iterator
  template < typename Key >
  INLINE const SequenceIteratorSafe< Key >&
               SequenceImplementation< Key, true >::endSafe() const noexcept {
    return _end_safe_;
  }

  // return an iterator pointing to the last element
  template < typename Key >
  INLINE SequenceIteratorSafe< Key > SequenceImplementation< Key, true >::rbeginSafe() const {
    SequenceIteratorSafe< Key > it{*this};
    it._setPos_(size() - 1);
    return it;
  }

  // returns an iterator pointing just before the first element
  template < typename Key >
  INLINE const SequenceIteratorSafe< Key >&
               SequenceImplementation< Key, true >::rendSafe() const noexcept {
    return _rend_safe_;
  }

  // returns the unsafe begin iterator
  template < typename Key >
  INLINE SequenceIterator< Key > SequenceImplementation< Key, true >::begin() const {
    return SequenceIterator< Key >{*this};
  }

  // return the unsafe end iterator
  template < typename Key >
  INLINE const SequenceIterator< Key >& SequenceImplementation< Key, true >::end() const noexcept {
    return _end_safe_;
  }

  // return an unsafe iterator pointing to the last element
  template < typename Key >
  INLINE SequenceIterator< Key > SequenceImplementation< Key, true >::rbegin() const {
    SequenceIterator< Key > it{*this};
    it._setPos_(size() - 1);
    return it;
  }

  // returns an unsafe iterator pointing just before the first element
  template < typename Key >
  INLINE const SequenceIterator< Key >& SequenceImplementation< Key, true >::rend() const noexcept {
    return _rend_safe_;
  }

  // modifies the size of the internal structures of the sequence
  template < typename Key >
  INLINE void SequenceImplementation< Key, true >::resize(Size new_size) {
    if (new_size < _h_.size()) return;

    _h_.resize(new_size);
    _v_.reserve(new_size);
  }

  // ===========================================================================
  // Sequence
  // ===========================================================================

  // Default constructor
  template < typename Key >
  INLINE Sequence< Key >::Sequence(Size size_param) :
      SequenceImplementation< Key, std::is_scalar< Key >::value >(size_param) {
    GUM_CONSTRUCTOR(Sequence);
  }

  // initializer list constructor
  template < typename Key >
  INLINE Sequence< Key >::Sequence(std::initializer_list< Key > list) :
      SequenceImplementation< Key, std::is_scalar< Key >::value >(list) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Sequence);
  }

  // copy constructor
  template < typename Key >
  INLINE Sequence< Key >::Sequence(const Sequence< Key >& aSeq) :
      SequenceImplementation< Key, std::is_scalar< Key >::value >(aSeq) {
    // for debugging purposes
    GUM_CONS_CPY(Sequence);
  }

  // move constructor
  template < typename Key >
  INLINE Sequence< Key >::Sequence(Sequence< Key >&& aSeq) :
      SequenceImplementation< Key, std::is_scalar< Key >::value >(std::move(aSeq)) {
    // for debugging purposes
    GUM_CONS_MOV(Sequence);
  }

  // destructor
  template < typename Key >
  INLINE Sequence< Key >::~Sequence() noexcept {
    // for debugging purposes
    GUM_DESTRUCTOR(Sequence);
  }

  // copy operator
  template < typename Key >
  INLINE Sequence< Key >& Sequence< Key >::operator=(const Sequence< Key >& aSeq) {
    Implementation::operator=(aSeq);
    return *this;
  }

  // move operator
  template < typename Key >
  INLINE Sequence< Key >& Sequence< Key >::operator=(Sequence< Key >&& aSeq) {
    Implementation::operator=(std::move(aSeq));
    return *this;
  }

  // returns the set difference : this \ seq
  template < typename Key >
  Set< Key > Sequence< Key >::diffSet(const Sequence< Key >& seq) const {
    Set< Key > res;

    for (iterator iter = this->begin(); iter != this->end(); ++iter) {
      if (!seq.exists(*iter)) res << *iter;
    }

    return res;
  }

  // a << operator for displaying the content of the Sequence
  template < typename Key >
  INLINE std::ostream& operator<<(std::ostream& stream, const Sequence< Key >& seq) {
    stream << seq.toString();
    return stream;
  }

} /* namespace gum */
