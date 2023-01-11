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
 * @brief template implementations of heaps
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <sstream>
#include <string>

// to ease IDE parser
#include <agrum/tools/core/heap.h>

namespace gum {

  // basic constructor. Creates an empty heap
  template < typename Val, typename Cmp >
  Heap< Val, Cmp >::Heap(Cmp compare, Size capacity) : _cmp_(compare) {
    _heap_.reserve(capacity);

    GUM_CONSTRUCTOR(Heap);
  }

  // initializer list constructor
  template < typename Val, typename Cmp >
  Heap< Val, Cmp >::Heap(std::initializer_list< Val > list) {
    _heap_.reserve(list.size());
    for (const auto& elt: list) {
      insert(elt);
    }

    GUM_CONSTRUCTOR(Heap);
  }

  // copy constructor
  template < typename Val, typename Cmp >
  Heap< Val, Cmp >::Heap(const Heap< Val, Cmp >& from) :
      _heap_(from._heap_), _nb_elements_(from._nb_elements_), _cmp_(from._cmp_) {
    // for debugging purposes
    GUM_CONS_CPY(Heap);
  }

  // move constructor
  template < typename Val, typename Cmp >
  Heap< Val, Cmp >::Heap(Heap< Val, Cmp >&& from) noexcept :
      _heap_(std::move(from._heap_)), _nb_elements_(std::move(from._nb_elements_)),
      _cmp_(std::move(from._cmp_)) {
    // for debugging purposes
    GUM_CONS_MOV(Heap);
  }

  // destructor
  template < typename Val, typename Cmp >
  Heap< Val, Cmp >::~Heap() {
    // for debugging purposes
    GUM_DESTRUCTOR(Heap);
  }

  // copy operator
  template < typename Val, typename Cmp >
  Heap< Val, Cmp >& Heap< Val, Cmp >::operator=(const Heap< Val, Cmp >& from) {
    // avoid self assignment
    if (this != &from) {
      try {
        _heap_ = from._heap_;
      } catch (...) {
        _heap_.clear();
        _nb_elements_ = 0;

        throw;
      }

      // set the comparison function
      _cmp_         = from._cmp_;
      _nb_elements_ = from._nb_elements_;

      // for debugging purposes
      GUM_OP_CPY(Heap);
    }

    return *this;
  }

  // move operator
  template < typename Val, typename Cmp >
  Heap< Val, Cmp >& Heap< Val, Cmp >::operator=(Heap< Val, Cmp >&& from) noexcept {
    // avoid self assignment
    if (this != &from) {
      _heap_        = std::move(from._heap_);
      _nb_elements_ = std::move(from._nb_elements_);
      _cmp_         = std::move(from._cmp_);
    }

    return *this;
  }

  // returns the element at the top of the heap
  template < typename Val, typename Cmp >
  INLINE const Val& Heap< Val, Cmp >::top() const {
    if (!_nb_elements_) { GUM_ERROR(NotFound, "empty heap") }

    return _heap_[0];
  }

  // returns the number of elements in the heap
  template < typename Val, typename Cmp >
  INLINE Size Heap< Val, Cmp >::size() const noexcept {
    return _nb_elements_;
  }

  // return the size of the array storing the heap
  template < typename Val, typename Cmp >
  INLINE Size Heap< Val, Cmp >::capacity() const noexcept {
    return Size(_heap_.size());
  }

  // changes the size of the array storing the heap
  template < typename Val, typename Cmp >
  INLINE void Heap< Val, Cmp >::resize(Size new_size) {
    if (new_size > _nb_elements_) _heap_.reserve(new_size);
  }

  // removes the element at position 'index' from the heap
  template < typename Val, typename Cmp >
  void Heap< Val, Cmp >::eraseByPos(Size index) {
    if (index >= _nb_elements_) return;

    // remove the element and put the last element in its place
    Val last = std::move(_heap_[_nb_elements_ - 1]);
    _heap_.pop_back();
    --_nb_elements_;

    if (!_nb_elements_ || (index == _nb_elements_)) return;

    // restore the heap property
    Size i = index;

    for (Size j = (index << 1) + 1; j < _nb_elements_; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < _nb_elements_) && _cmp_(_heap_[j + 1], _heap_[j])) ++j;

      // if "last" is smaller than  _heap_[j], "last" must be stored at index i
      if (_cmp_(last, _heap_[j])) break;

      _heap_[i] = std::move(_heap_[j]);
    }

    _heap_[i] = std::move(last);
  }

  // removes a given element from the heap (but does not return it)
  template < typename Val, typename Cmp >
  INLINE void Heap< Val, Cmp >::erase(const Val& val) {
    // find val in the heap
    for (Size i = 0; i < _nb_elements_; ++i)
      if (_heap_[i] == val) {
        eraseByPos(i);
        break;
      }
  }

  // removes the top of the heap (but does not return it)
  template < typename Val, typename Cmp >
  INLINE void Heap< Val, Cmp >::eraseTop() {
    // if the heap is empty, do nothing
    if (!_nb_elements_) return;
    eraseByPos(0);
  }

  // removes the top element from the heap and return it
  template < typename Val, typename Cmp >
  INLINE Val Heap< Val, Cmp >::pop() {
    if (!_nb_elements_) { GUM_ERROR(NotFound, "empty heap") }

    Val v = _heap_[0];
    eraseByPos(0);
    return v;
  }

  // after inserting an element at the end of the heap, restore heap property
  template < typename Val, typename Cmp >
  Size Heap< Val, Cmp >::_restoreHeap_() {
    // get the element at the end of the heap
    Size i = _nb_elements_ - 1;
    Val  v = std::move(_heap_[i]);

    // restore the heap property
    for (Size j = (i - 1) >> 1; i && _cmp_(v, _heap_[j]); i = j, j = (j - 1) >> 1)
      _heap_[i] = std::move(_heap_[j]);

    _heap_[i] = std::move(v);

    return i;
  }

  // inserts a new (a copy) element in the heap
  template < typename Val, typename Cmp >
  INLINE Size Heap< Val, Cmp >::insert(const Val& val) {
    // create a new element at the end of the heap
    _heap_.push_back(val);
    ++_nb_elements_;
    return _restoreHeap_();
  }

  // inserts a new (a copy) element in the heap
  template < typename Val, typename Cmp >
  Size Heap< Val, Cmp >::insert(Val&& val) {
    // create a new element at the end of the heap
    _heap_.push_back(std::move(val));
    ++_nb_elements_;
    return _restoreHeap_();
  }

  // emplace a new element in the heap
  template < typename Val, typename Cmp >
  template < typename... Args >
  Size Heap< Val, Cmp >::emplace(Args&&... args) {
    // create a new element at the end of the heap
    _heap_.emplace_back(std::forward< Args >(args)...);
    ++_nb_elements_;
    return _restoreHeap_();
  }

  // indicates whether the heap is empty
  template < typename Val, typename Cmp >
  INLINE bool Heap< Val, Cmp >::empty() const noexcept {
    return (_nb_elements_ == 0);
  }

  // indicates whether the heap contains a given value
  template < typename Val, typename Cmp >
  INLINE bool Heap< Val, Cmp >::contains(const Val& val) const {
    for (Size i = 0; i < _nb_elements_; ++i)
      if (_heap_[i] == val) return true;

    return false;
  }

  // returns the element at index elt from the heap
  template < typename Val, typename Cmp >
  INLINE const Val& Heap< Val, Cmp >::operator[](Size index) const {
    // check if the element exists
    if (index >= _nb_elements_) { GUM_ERROR(NotFound, "not enough elements in the heap") }

    return _heap_[index];
  }

  // displays the content of the heap
  template < typename Val, typename Cmp >
  std::string Heap< Val, Cmp >::toString() const {
    bool              deja = false;
    std::stringstream stream;
    stream << "[";

    for (Size i = 0; i != _nb_elements_; ++i, deja = true) {
      if (deja) stream << " , ";

      stream << _heap_[i];
    }

    stream << "]";

    return stream.str();
  }

  // A \c << operator for Heap
  template < typename Val, typename Cmp >
  INLINE std::ostream& operator<<(std::ostream& stream, const Heap< Val, Cmp >& heap) {
    stream << heap.toString();
    return stream;
  }

} /* namespace gum */
