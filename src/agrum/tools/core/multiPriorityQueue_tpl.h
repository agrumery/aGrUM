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
 * @brief Template implementations of multi priority queues.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// to help IDE parser
#include <agrum/tools/core/multiPriorityQueue.h>

namespace gum {

  // basic constructor
  template < typename Val, typename Priority, typename Cmp >
  MultiPriorityQueue< Val, Priority, Cmp >::MultiPriorityQueue(Cmp compare, Size capacity) :
      _indices_(capacity >> 1, true, false), _cmp_(compare) {
    _heap_.reserve(capacity);

    // for debugging purposes
    GUM_CONSTRUCTOR(MultiPriorityQueue);
  }

  // initializer list constructor
  template < typename Val, typename Priority, typename Cmp >
  MultiPriorityQueue< Val, Priority, Cmp >::MultiPriorityQueue(
     std::initializer_list< std::pair< Val, Priority > > list) :
      _indices_(Size(list.size()) / 2, true, false) {
    // fill the queue
    _heap_.reserve(list.size());
    for (const auto& elt: list) {
      insert(elt.first, elt.second);
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(MultiPriorityQueue);
  }

  // copy constructor
  template < typename Val, typename Priority, typename Cmp >
  MultiPriorityQueue< Val, Priority, Cmp >::MultiPriorityQueue(
     const MultiPriorityQueue< Val, Priority, Cmp >& from) :
      _heap_(from._heap_),
      _indices_(from._indices_), _nb_elements_(from._nb_elements_), _cmp_(from._cmp_) {
    // for debugging purposes
    GUM_CONS_CPY(MultiPriorityQueue);

    // fill the heap structure
    for (const auto& val_and_index: _indices_) {
      const Val*                 val  = &(val_and_index.first);
      const std::vector< Size >& vect = val_and_index.second;
      for (auto index: vect) {
        _heap_[index].second = val;
      }
    }
  }

  // move constructor
  template < typename Val, typename Priority, typename Cmp >
  MultiPriorityQueue< Val, Priority, Cmp >::MultiPriorityQueue(
     MultiPriorityQueue< Val, Priority, Cmp >&& from) :
      _heap_(std::move(from._heap_)),
      _indices_(std::move(from._indices_)), _nb_elements_(std::move(from._nb_elements_)),
      _cmp_(std::move(from._cmp_)) {
    // for debugging purposes
    GUM_CONS_MOV(MultiPriorityQueue);
  }

  // destructor
  template < typename Val, typename Priority, typename Cmp >
  MultiPriorityQueue< Val, Priority, Cmp >::~MultiPriorityQueue() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiPriorityQueue);
  }

  // copy operator
  template < typename Val, typename Priority, typename Cmp >
  MultiPriorityQueue< Val, Priority, Cmp >& MultiPriorityQueue< Val, Priority, Cmp >::operator=(
     const MultiPriorityQueue< Val, Priority, Cmp >& from) {
    // for debugging purposes
    GUM_OP_CPY(MultiPriorityQueue);

    try {
      // set the comprison function
      _cmp_ = from._cmp_;

      // copy the indices and the heap
      _indices_     = from._indices_;
      _heap_        = from._heap_;
      _nb_elements_ = from._nb_elements_;

      // restore the link between  _indices_ and  _heap_
      for (const auto& val_and_index: _indices_) {
        const Val*                 val  = &(val_and_index.first);
        const std::vector< Size >& vect = val_and_index.second;
        for (auto index: vect) {
          _heap_[index].second = val;
        }
      }
    } catch (...) {
      _heap_.clear();
      _indices_.clear();
      _nb_elements_ = 0;

      throw;
    }

    return *this;
  }

  // move operator
  template < typename Val, typename Priority, typename Cmp >
  MultiPriorityQueue< Val, Priority, Cmp >& MultiPriorityQueue< Val, Priority, Cmp >::operator=(
     MultiPriorityQueue< Val, Priority, Cmp >&& from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_MOV(MultiPriorityQueue);

      _cmp_         = std::move(from._cmp_);
      _indices_     = std::move(from._indices_);
      _heap_        = std::move(from._heap_);
      _nb_elements_ = std::move(from._nb_elements_);
    }

    return *this;
  }

  // returns the element at the top of the priority queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE const Val& MultiPriorityQueue< Val, Priority, Cmp >::top() const {
    if (!_nb_elements_) { GUM_ERROR(NotFound, "empty priority queue") }

    return *(_heap_[0].second);
  }

  // returns the priority of the top element
  template < typename Val, typename Priority, typename Cmp >
  INLINE const Priority& MultiPriorityQueue< Val, Priority, Cmp >::topPriority() const {
    if (!_nb_elements_) { GUM_ERROR(NotFound, "empty priority queue") }

    return _heap_[0].first;
  }

  // returns the number of elements in the priority queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE Size MultiPriorityQueue< Val, Priority, Cmp >::size() const noexcept {
    return _nb_elements_;
  }

  // return the size of the array storing the priority queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE Size MultiPriorityQueue< Val, Priority, Cmp >::capacity() const noexcept {
    return Size(_heap_.capacity());
  }

  // changes the size of the array storing the priority queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE void MultiPriorityQueue< Val, Priority, Cmp >::resize(Size new_size) {
    if (new_size < _nb_elements_) return;

    _heap_.reserve(new_size);
    _indices_.resize(new_size / 2);
  }

  // removes all the elements from the queue
  template < typename Val, typename Priority, typename Cmp >
  void MultiPriorityQueue< Val, Priority, Cmp >::clear() {
    _nb_elements_ = 0;
    _heap_.clear();
    _indices_.clear();
  }

  // removes the element at index elt from the priority queue
  template < typename Val, typename Priority, typename Cmp >
  void MultiPriorityQueue< Val, Priority, Cmp >::eraseByPos(Size index) {
    if (index >= _nb_elements_) return;

    // remove the element from the hashtable
    const Val&           del_val    = *(_heap_[index].second);
    std::vector< Size >& vect_index = _indices_[del_val];
    if (vect_index.size() == 1) _indices_.erase(del_val);
    else {
      for (auto& v_index: vect_index) {
        if (v_index == index) {
          v_index = vect_index.back();
          vect_index.pop_back();
          break;
        }
      }
    }

    // put the last element at the "index" location
    std::pair< Priority, const Val* > last = std::move(_heap_.back());
    _heap_.pop_back();
    --_nb_elements_;

    if (!_nb_elements_ || (index == _nb_elements_)) return;

    // restore the heap property
    Size i = index;

    for (Size j = (index << 1) + 1; j < _nb_elements_; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < _nb_elements_) && _cmp_(_heap_[j + 1].first, _heap_[j].first)) ++j;

      // if "last" is lower than heap[j], "last" must be stored at index i
      if (_cmp_(last.first, _heap_[j].first)) break;

      // else pull up the jth node
      _heap_[i]                       = std::move(_heap_[j]);
      std::vector< Size >& vect_index = _indices_[*(_heap_[i].second)];
      for (auto& v_index: vect_index) {
        if (v_index == j) {
          v_index = i;
          break;
        }
      }
    }

    // put "last" back into the heap
    _heap_[i]                         = std::move(last);
    std::vector< Size >& last_indices = _indices_[*(_heap_[i].second)];
    for (auto& v_index: last_indices) {
      if (v_index == _nb_elements_) {
        v_index = i;
        break;
      }
    }
  }

  // removes a given element from the priority queue (but does not return it)
  template < typename Val, typename Priority, typename Cmp >
  INLINE void MultiPriorityQueue< Val, Priority, Cmp >::erase(const Val& val) {
    try {
      eraseByPos(_indices_[val][0]);
    } catch (NotFound const&) {}
  }

  // removes the top of the priority queue (but does not return it)
  template < typename Val, typename Priority, typename Cmp >
  INLINE void MultiPriorityQueue< Val, Priority, Cmp >::eraseTop() {
    eraseByPos(0);
  }

  // removes the top element from the priority queue and return it
  template < typename Val, typename Priority, typename Cmp >
  INLINE Val MultiPriorityQueue< Val, Priority, Cmp >::pop() {
    if (!_nb_elements_) { GUM_ERROR(NotFound, "empty priority queue") }

    Val v = *(_heap_[0].second);
    eraseByPos(0);

    return v;
  }

  // returns a hashtable the keys of which are the values stored in the queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE const HashTable< Val, std::vector< Size > >&
               MultiPriorityQueue< Val, Priority, Cmp >::allValues() const {
    return reinterpret_cast< const HashTable< Val, std::vector< Size > >& >(_indices_);
  }

  // inserts a new (a copy) element in the priority queue
  template < typename Val, typename Priority, typename Cmp >
  Size MultiPriorityQueue< Val, Priority, Cmp >::insert(const Val& val, const Priority& priority) {
    // create the entry in the indices hashtable
    const Val*           new_val;
    std::vector< Size >* new_vect;
    if (!_indices_.exists(val)) {
      auto& new_elt = _indices_.insert(val, std::vector< Size >());
      new_val       = &(new_elt.first);
      new_vect      = &(new_elt.second);
    } else {
      new_val  = &(_indices_.key(val));
      new_vect = &(_indices_[val]);
    }

    try {
      new_vect->push_back(0);
    } catch (...) {
      if (new_vect->empty()) { _indices_.erase(val); }
      throw;
    }

    try {
      _heap_.push_back(std::pair< Priority, const Val* >(priority, new_val));
    } catch (...) {
      if (new_vect->size() == 1) { _indices_.erase(val); }
      throw;
    }

    std::pair< Priority, const Val* > new_heap_val = std::move(_heap_[_nb_elements_]);
    ++_nb_elements_;

    // restore the heap property
    Size i = _nb_elements_ - 1;

    for (Size j = (i - 1) >> 1; i && _cmp_(new_heap_val.first, _heap_[j].first);
         i = j, j = (j - 1) >> 1) {
      _heap_[i]                       = std::move(_heap_[j]);
      std::vector< Size >& vect_index = _indices_[*(_heap_[i].second)];
      for (auto& index: vect_index) {
        if (index == j) {
          index = i;
          break;
        }
      }
    }

    // put the new bucket into the heap
    _heap_[i].first  = std::move(new_heap_val.first);
    _heap_[i].second = new_val;
    new_vect->back() = i;

    return i;
  }

  // inserts a new (a copy) element in the priority queue
  template < typename Val, typename Priority, typename Cmp >
  Size MultiPriorityQueue< Val, Priority, Cmp >::insert(Val&& val, Priority&& priority) {
    // create the entry in the indices hashtable
    const Val*           new_val;
    std::vector< Size >* new_vect;
    if (!_indices_.exists(val)) {
      auto& new_elt = _indices_.insert(std::move(val), std::vector< Size >());
      new_val       = &(new_elt.first);
      new_vect      = &(new_elt.second);
    } else {
      new_val  = &(_indices_.key(val));
      new_vect = &(_indices_[val]);
    }

    try {
      new_vect->push_back(0);
    } catch (...) {
      if (new_vect->empty()) { _indices_.erase(*new_val); }
      throw;
    }

    try {
      _heap_.push_back(std::pair< Priority, const Val* >(std::move(priority), new_val));
    } catch (...) {
      if (new_vect->size() == 1) { _indices_.erase(*new_val); }
      throw;
    }

    std::pair< Priority, const Val* > new_heap_val = std::move(_heap_[_nb_elements_]);
    ++_nb_elements_;

    // restore the heap property
    Size i = _nb_elements_ - 1;

    for (Size j = (i - 1) >> 1; i && _cmp_(new_heap_val.first, _heap_[j].first);
         i = j, j = (j - 1) >> 1) {
      _heap_[i]                       = std::move(_heap_[j]);
      std::vector< Size >& vect_index = _indices_[*(_heap_[i].second)];
      for (auto& index: vect_index) {
        if (index == j) {
          index = i;
          break;
        }
      }
    }

    // put the new bucket into the heap
    _heap_[i].first  = std::move(new_heap_val.first);
    _heap_[i].second = new_val;
    new_vect->back() = i;

    return i;
  }

  // emplace a new element into the priority queue
  template < typename Val, typename Priority, typename Cmp >
  template < typename... Args >
  INLINE Size MultiPriorityQueue< Val, Priority, Cmp >::emplace(Args&&... args) {
    std::pair< Val, Priority > new_elt
       = std::make_pair< Val, Priority >(std::forward< Args >(args)...);
    return insert(std::move(new_elt.first), std::move(new_elt.second));
  }

  // indicates whether the priority queue is empty
  template < typename Val, typename Priority, typename Cmp >
  INLINE bool MultiPriorityQueue< Val, Priority, Cmp >::empty() const noexcept {
    return (_nb_elements_ == 0);
  }

  // indicates whether the priority queue contains a given value
  template < typename Val, typename Priority, typename Cmp >
  INLINE bool MultiPriorityQueue< Val, Priority, Cmp >::contains(const Val& val) const {
    return _indices_.exists(val);
  }

  // returns the element at position "index" in the priority queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE const Val& MultiPriorityQueue< Val, Priority, Cmp >::operator[](Size index) const {
    if (index >= _nb_elements_) {
      GUM_ERROR(NotFound, "not enough elements in the MultiPriorityQueue")
    }

    return *(_heap_[index].second);
  }

  // displays the content of the queue
  template < typename Val, typename Priority, typename Cmp >
  std::string MultiPriorityQueue< Val, Priority, Cmp >::toString() const {
    bool              deja = false;
    std::stringstream stream;
    stream << "[";

    for (Size i = 0; i != _nb_elements_; ++i, deja = true) {
      if (deja) stream << " , ";

      stream << "(" << _heap_[i].first << " , " << *(_heap_[i].second) << ")";
    }

    stream << "]";

    return stream.str();
  }

  // changes the size of the internal structure storing the priority queue
  template < typename Val, typename Priority, typename Cmp >
  Size MultiPriorityQueue< Val, Priority, Cmp >::setPriorityByPos(Size            index,
                                                                  const Priority& new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >= _nb_elements_) {
      GUM_ERROR(NotFound, "not enough elements in the MultiPriorityQueue")
    }

    // get the element itself
    const Val* val = _heap_[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i && _cmp_(new_priority, _heap_[j].first);
         i = j, j = (j - 1) >> 1) {
      _heap_[i]                       = std::move(_heap_[j]);
      std::vector< Size >& vect_index = _indices_[*(_heap_[i].second)];
      for (auto& idx: vect_index) {
        if (idx == j) {
          idx = i;
          break;
        }
      }
    }

    // move val downward if needed
    for (Size j = (i << 1) + 1; j < _nb_elements_; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < _nb_elements_) && _cmp_(_heap_[j + 1].first, _heap_[j].first)) ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if (_cmp_(new_priority, _heap_[j].first)) break;

      // else pull up the jth node
      _heap_[i]                       = std::move(_heap_[j]);
      std::vector< Size >& vect_index = _indices_[*(_heap_[i].second)];
      for (auto& idx: vect_index) {
        if (idx == j) {
          idx = i;
          break;
        }
      }
    }

    // update the index of val
    _heap_[i].first                 = new_priority;
    _heap_[i].second                = val;
    std::vector< Size >& vect_index = _indices_[*(_heap_[i].second)];
    for (auto& idx: vect_index) {
      if (idx == index) {
        idx = i;
        break;
      }
    }

    return i;
  }

  // changes the size of the internal structure storing the priority queue
  template < typename Val, typename Priority, typename Cmp >
  Size MultiPriorityQueue< Val, Priority, Cmp >::setPriorityByPos(Size       index,
                                                                  Priority&& new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >= _nb_elements_) {
      GUM_ERROR(NotFound, "not enough elements in the MultiPriorityQueue")
    }

    // get the element itself
    const Val* val = _heap_[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i && _cmp_(new_priority, _heap_[j].first);
         i = j, j = (j - 1) >> 1) {
      _heap_[i]                       = std::move(_heap_[j]);
      std::vector< Size >& vect_index = _indices_[*(_heap_[i].second)];
      for (auto& idx: vect_index) {
        if (idx == j) {
          idx = i;
          break;
        }
      }
    }

    // move val downward if needed
    for (Size j = (i << 1) + 1; j < _nb_elements_; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < _nb_elements_) && _cmp_(_heap_[j + 1].first, _heap_[j].first)) ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if (_cmp_(new_priority, _heap_[j].first)) break;

      // else pull up the jth node
      _heap_[i]                       = std::move(_heap_[j]);
      std::vector< Size >& vect_index = _indices_[*(_heap_[i].second)];
      for (auto& idx: vect_index) {
        if (idx == j) {
          idx = i;
          break;
        }
      }
    }

    // update the index of val
    _heap_[i].first                 = std::move(new_priority);
    _heap_[i].second                = val;
    std::vector< Size >& vect_index = _indices_[*(_heap_[i].second)];
    for (auto& idx: vect_index) {
      if (idx == index) {
        idx = i;
        break;
      }
    }

    return i;
  }

  // modifies the priority of each instance of a given element
  template < typename Val, typename Priority, typename Cmp >
  void MultiPriorityQueue< Val, Priority, Cmp >::setPriority(const Val&      elt,
                                                             const Priority& new_priority) {
    std::vector< Size >& vect_index = _indices_[elt];

    for (auto index: vect_index) {
      setPriorityByPos(index, new_priority);
    }
  }

  // modifies the priority of each instance of a given element
  template < typename Val, typename Priority, typename Cmp >
  INLINE const Priority& MultiPriorityQueue< Val, Priority, Cmp >::priority(const Val& elt) const {
    return _heap_[_indices_[elt][0]].first;
  }

  // A \c << operator for priority queues
  template < typename Val, typename Priority, typename Cmp >
  INLINE std::ostream& operator<<(std::ostream&                                   stream,
                                  const MultiPriorityQueue< Val, Priority, Cmp >& queue) {
    stream << queue.toString();
    return stream;
  }

} /* namespace gum */
