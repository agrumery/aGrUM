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
 * @brief Template implementations of multi priority queues.
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

// to help IDE parser
#include <agrum/tools/core/multiPriorityQueue.h>

namespace gum {

  // basic constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  MultiPriorityQueue< Val, Priority, Cmp, Alloc >::MultiPriorityQueue(
     Cmp  compare,
     Size capacity) :
      indices__(capacity >> 1, true, false),
      cmp__(compare) {
    heap__.reserve(capacity);

    // for debugging purposes
    GUM_CONSTRUCTOR(MultiPriorityQueue);
  }

  // initializer list constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  MultiPriorityQueue< Val, Priority, Cmp, Alloc >::MultiPriorityQueue(
     std::initializer_list< std::pair< Val, Priority > > list) :
      indices__(Size(list.size()) / 2, true, false) {
    // fill the queue
    heap__.reserve(list.size());
    for (const auto& elt: list) {
      insert(elt.first, elt.second);
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(MultiPriorityQueue);
  }

  // copy constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  MultiPriorityQueue< Val, Priority, Cmp, Alloc >::MultiPriorityQueue(
     const MultiPriorityQueue< Val, Priority, Cmp, Alloc >& from) :
      heap__(from.heap__),
      indices__(from.indices__), nb_elements__(from.nb_elements__),
      cmp__(from.cmp__) {
    // for debugging purposes
    GUM_CONS_CPY(MultiPriorityQueue);

    // fill the heap structure
    for (const auto& val_and_index: indices__) {
      const Val*                 val  = &(val_and_index.first);
      const std::vector< Size >& vect = val_and_index.second;
      for (auto index: vect) {
        heap__[index].second = val;
      }
    }
  }

  // generalized copy constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  template < typename OtherAlloc >
  MultiPriorityQueue< Val, Priority, Cmp, Alloc >::MultiPriorityQueue(
     const MultiPriorityQueue< Val, Priority, Cmp, OtherAlloc >& from) :
      indices__(from.indices__),
      nb_elements__(from.nb_elements__), cmp__(from.cmp__) {
    // for debugging purposes
    GUM_CONS_CPY(MultiPriorityQueue);

    // fill the heap structure
    if (nb_elements__) {
      heap__.reserve(from.heap__.size());
      for (const auto& elt: from.heap__) {
        heap__.push_back(elt);
      }
      for (const auto& val_and_index: indices__) {
        const Val*                 val  = &(val_and_index.first);
        const std::vector< Size >& vect = val_and_index.second;
        for (auto index: vect) {
          heap__[index].second = val;
        }
      }
    }
  }

  // move constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  MultiPriorityQueue< Val, Priority, Cmp, Alloc >::MultiPriorityQueue(
     MultiPriorityQueue< Val, Priority, Cmp, Alloc >&& from) :
      heap__(std::move(from.heap__)),
      indices__(std::move(from.indices__)),
      nb_elements__(std::move(from.nb_elements__)), cmp__(std::move(from.cmp__)) {
    // for debugging purposes
    GUM_CONS_MOV(MultiPriorityQueue);
  }

  // destructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  MultiPriorityQueue< Val, Priority, Cmp, Alloc >::~MultiPriorityQueue() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiPriorityQueue);
  }

  // copy operator
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  MultiPriorityQueue< Val, Priority, Cmp, Alloc >&
     MultiPriorityQueue< Val, Priority, Cmp, Alloc >::operator=(
        const MultiPriorityQueue< Val, Priority, Cmp, Alloc >& from) {
    // for debugging purposes
    GUM_OP_CPY(MultiPriorityQueue);

    try {
      // set the comprison function
      cmp__ = from.cmp__;

      // copy the indices and the heap
      indices__     = from.indices__;
      heap__        = from.heap__;
      nb_elements__ = from.nb_elements__;

      // restore the link between indices__ and heap__
      for (const auto& val_and_index: indices__) {
        const Val*                 val  = &(val_and_index.first);
        const std::vector< Size >& vect = val_and_index.second;
        for (auto index: vect) {
          heap__[index].second = val;
        }
      }
    } catch (...) {
      heap__.clear();
      indices__.clear();
      nb_elements__ = 0;

      throw;
    }

    return *this;
  }

  // generalized copy operator
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  template < typename OtherAlloc >
  MultiPriorityQueue< Val, Priority, Cmp, Alloc >&
     MultiPriorityQueue< Val, Priority, Cmp, Alloc >::operator=(
        const MultiPriorityQueue< Val, Priority, Cmp, OtherAlloc >& from) {
    // for debugging purposes
    GUM_OP_CPY(MultiPriorityQueue);

    try {
      // set the comprison function
      cmp__ = from.cmp__;

      // copy the indices and the heap
      indices__     = from.indices__;
      nb_elements__ = from.nb_elements__;

      // restore the link between indices__ and heap__
      heap__.clear();
      heap__.reserve(from.heap__.size());
      for (const auto& elt: from.heap__) {
        heap__.push_back(elt);
      }
      for (const auto& val_and_index: indices__) {
        const Val*                 val  = &(val_and_index.first);
        const std::vector< Size >& vect = val_and_index.second;
        for (auto index: vect) {
          heap__[index].second = val;
        }
      }
    } catch (...) {
      heap__.clear();
      indices__.clear();
      nb_elements__ = 0;

      throw;
    }

    return *this;
  }

  // move operator
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  MultiPriorityQueue< Val, Priority, Cmp, Alloc >&
     MultiPriorityQueue< Val, Priority, Cmp, Alloc >::operator=(
        MultiPriorityQueue< Val, Priority, Cmp, Alloc >&& from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_MOV(MultiPriorityQueue);

      cmp__         = std::move(from.cmp__);
      indices__     = std::move(from.indices__);
      heap__        = std::move(from.heap__);
      nb_elements__ = std::move(from.nb_elements__);
    }

    return *this;
  }

  // returns the element at the top of the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const Val& MultiPriorityQueue< Val, Priority, Cmp, Alloc >::top() const {
    if (!nb_elements__) { GUM_ERROR(NotFound, "empty priority queue"); }

    return *(heap__[0].second);
  }

  // returns the priority of the top element
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const Priority&
     MultiPriorityQueue< Val, Priority, Cmp, Alloc >::topPriority() const {
    if (!nb_elements__) { GUM_ERROR(NotFound, "empty priority queue"); }

    return heap__[0].first;
  }

  // returns the number of elements in the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE Size
     MultiPriorityQueue< Val, Priority, Cmp, Alloc >::size() const noexcept {
    return nb_elements__;
  }

  // return the size of the array storing the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE Size
     MultiPriorityQueue< Val, Priority, Cmp, Alloc >::capacity() const noexcept {
    return Size(heap__.capacity());
  }

  // changes the size of the array storing the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE void
     MultiPriorityQueue< Val, Priority, Cmp, Alloc >::resize(Size new_size) {
    if (new_size < nb_elements__) return;

    heap__.reserve(new_size);
    indices__.resize(new_size / 2);
  }

  // removes all the elements from the queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  void MultiPriorityQueue< Val, Priority, Cmp, Alloc >::clear() {
    nb_elements__ = 0;
    heap__.clear();
    indices__.clear();
  }

  // removes the element at index elt from the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  void MultiPriorityQueue< Val, Priority, Cmp, Alloc >::eraseByPos(Size index) {
    if (index >= nb_elements__) return;

    // remove the element from the hashtable
    const Val&           del_val    = *(heap__[index].second);
    std::vector< Size >& vect_index = indices__[del_val];
    if (vect_index.size() == 1)
      indices__.erase(del_val);
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
    std::pair< Priority, const Val* > last = std::move(heap__.back());
    heap__.pop_back();
    --nb_elements__;

    if (!nb_elements__ || (index == nb_elements__)) return;

    // restore the heap property
    Size i = index;

    for (Size j = (index << 1) + 1; j < nb_elements__; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < nb_elements__) && cmp__(heap__[j + 1].first, heap__[j].first))
        ++j;

      // if "last" is lower than heap[j], "last" must be stored at index i
      if (cmp__(last.first, heap__[j].first)) break;

      // else pull up the jth node
      heap__[i]                       = std::move(heap__[j]);
      std::vector< Size >& vect_index = indices__[*(heap__[i].second)];
      for (auto& v_index: vect_index) {
        if (v_index == j) {
          v_index = i;
          break;
        }
      }
    }

    // put "last" back into the heap
    heap__[i]                         = std::move(last);
    std::vector< Size >& last_indices = indices__[*(heap__[i].second)];
    for (auto& v_index: last_indices) {
      if (v_index == nb_elements__) {
        v_index = i;
        break;
      }
    }
  }

  // removes a given element from the priority queue (but does not return it)
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE void
     MultiPriorityQueue< Val, Priority, Cmp, Alloc >::erase(const Val& val) {
    try {
      eraseByPos(indices__[val][0]);
    } catch (NotFound&) {}
  }

  // removes the top of the priority queue (but does not return it)
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE void MultiPriorityQueue< Val, Priority, Cmp, Alloc >::eraseTop() {
    eraseByPos(0);
  }

  // removes the top element from the priority queue and return it
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE Val MultiPriorityQueue< Val, Priority, Cmp, Alloc >::pop() {
    if (!nb_elements__) { GUM_ERROR(NotFound, "empty priority queue"); }

    Val v = *(heap__[0].second);
    eraseByPos(0);

    return v;
  }

  // returns a hashtable the keys of which are the values stored in the queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const HashTable< Val, std::vector< Size > >&
               MultiPriorityQueue< Val, Priority, Cmp, Alloc >::allValues() const {
    return reinterpret_cast< const HashTable< Val, std::vector< Size > >& >(
       indices__);
  }

  // inserts a new (a copy) element in the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  Size MultiPriorityQueue< Val, Priority, Cmp, Alloc >::insert(
     const Val&      val,
     const Priority& priority) {
    // create the entry in the indices hashtable
    const Val*           new_val;
    std::vector< Size >* new_vect;
    if (!indices__.exists(val)) {
      auto& new_elt = indices__.insert(val, std::vector< Size >());
      new_val       = &(new_elt.first);
      new_vect      = &(new_elt.second);
    } else {
      new_val  = &(indices__.key(val));
      new_vect = &(indices__[val]);
    }

    try {
      new_vect->push_back(0);
    } catch (...) {
      if (new_vect->empty()) { indices__.erase(val); }
      throw;
    }

    try {
      heap__.push_back(std::pair< Priority, const Val* >(priority, new_val));
    } catch (...) {
      if (new_vect->size() == 1) { indices__.erase(val); }
      throw;
    }

    std::pair< Priority, const Val* > new_heap_val
       = std::move(heap__[nb_elements__]);
    ++nb_elements__;

    // restore the heap property
    Size i = nb_elements__ - 1;

    for (Size j = (i - 1) >> 1; i && cmp__(new_heap_val.first, heap__[j].first);
         i = j, j = (j - 1) >> 1) {
      heap__[i]                       = std::move(heap__[j]);
      std::vector< Size >& vect_index = indices__[*(heap__[i].second)];
      for (auto& index: vect_index) {
        if (index == j) {
          index = i;
          break;
        }
      }
    }

    // put the new bucket into the heap
    heap__[i].first  = std::move(new_heap_val.first);
    heap__[i].second = new_val;
    new_vect->back() = i;

    return i;
  }

  // inserts a new (a copy) element in the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  Size
     MultiPriorityQueue< Val, Priority, Cmp, Alloc >::insert(Val&&      val,
                                                             Priority&& priority) {
    // create the entry in the indices hashtable
    const Val*           new_val;
    std::vector< Size >* new_vect;
    if (!indices__.exists(val)) {
      auto& new_elt = indices__.insert(std::move(val), std::vector< Size >());
      new_val       = &(new_elt.first);
      new_vect      = &(new_elt.second);
    } else {
      new_val  = &(indices__.key(val));
      new_vect = &(indices__[val]);
    }

    try {
      new_vect->push_back(0);
    } catch (...) {
      if (new_vect->empty()) { indices__.erase(*new_val); }
      throw;
    }

    try {
      heap__.push_back(
         std::pair< Priority, const Val* >(std::move(priority), new_val));
    } catch (...) {
      if (new_vect->size() == 1) { indices__.erase(*new_val); }
      throw;
    }

    std::pair< Priority, const Val* > new_heap_val
       = std::move(heap__[nb_elements__]);
    ++nb_elements__;

    // restore the heap property
    Size i = nb_elements__ - 1;

    for (Size j = (i - 1) >> 1; i && cmp__(new_heap_val.first, heap__[j].first);
         i = j, j = (j - 1) >> 1) {
      heap__[i]                       = std::move(heap__[j]);
      std::vector< Size >& vect_index = indices__[*(heap__[i].second)];
      for (auto& index: vect_index) {
        if (index == j) {
          index = i;
          break;
        }
      }
    }

    // put the new bucket into the heap
    heap__[i].first  = std::move(new_heap_val.first);
    heap__[i].second = new_val;
    new_vect->back() = i;

    return i;
  }

  // emplace a new element into the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  template < typename... Args >
  INLINE Size
     MultiPriorityQueue< Val, Priority, Cmp, Alloc >::emplace(Args&&... args) {
    std::pair< Val, Priority > new_elt
       = std::make_pair< Val, Priority >(std::forward< Args >(args)...);
    return insert(std::move(new_elt.first), std::move(new_elt.second));
  }

  // indicates whether the priority queue is empty
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE bool
     MultiPriorityQueue< Val, Priority, Cmp, Alloc >::empty() const noexcept {
    return (nb_elements__ == 0);
  }

  // indicates whether the priority queue contains a given value
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE bool MultiPriorityQueue< Val, Priority, Cmp, Alloc >::contains(
     const Val& val) const {
    return indices__.exists(val);
  }

  // returns the element at position "index" in the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const Val& MultiPriorityQueue< Val, Priority, Cmp, Alloc >::operator[](
     Size index) const {
    if (index >= nb_elements__) {
      GUM_ERROR(NotFound, "not enough elements in the MultiPriorityQueue");
    }

    return *(heap__[index].second);
  }

  // displays the content of the queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  std::string MultiPriorityQueue< Val, Priority, Cmp, Alloc >::toString() const {
    bool              deja = false;
    std::stringstream stream;
    stream << "[";

    for (Size i = 0; i != nb_elements__; ++i, deja = true) {
      if (deja) stream << " , ";

      stream << "(" << heap__[i].first << " , " << *(heap__[i].second) << ")";
    }

    stream << "]";

    return stream.str();
  }

  // changes the size of the internal structure storing the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  Size MultiPriorityQueue< Val, Priority, Cmp, Alloc >::setPriorityByPos(
     Size            index,
     const Priority& new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >= nb_elements__) {
      GUM_ERROR(NotFound, "not enough elements in the MultiPriorityQueue");
    }

    // get the element itself
    const Val* val = heap__[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i && cmp__(new_priority, heap__[j].first);
         i = j, j = (j - 1) >> 1) {
      heap__[i]                       = std::move(heap__[j]);
      std::vector< Size >& vect_index = indices__[*(heap__[i].second)];
      for (auto& idx: vect_index) {
        if (idx == j) {
          idx = i;
          break;
        }
      }
    }

    // move val downward if needed
    for (Size j = (i << 1) + 1; j < nb_elements__; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < nb_elements__) && cmp__(heap__[j + 1].first, heap__[j].first))
        ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if (cmp__(new_priority, heap__[j].first)) break;

      // else pull up the jth node
      heap__[i]                       = std::move(heap__[j]);
      std::vector< Size >& vect_index = indices__[*(heap__[i].second)];
      for (auto& idx: vect_index) {
        if (idx == j) {
          idx = i;
          break;
        }
      }
    }

    // update the index of val
    heap__[i].first                 = new_priority;
    heap__[i].second                = val;
    std::vector< Size >& vect_index = indices__[*(heap__[i].second)];
    for (auto& idx: vect_index) {
      if (idx == index) {
        idx = i;
        break;
      }
    }

    return i;
  }

  // changes the size of the internal structure storing the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  Size MultiPriorityQueue< Val, Priority, Cmp, Alloc >::setPriorityByPos(
     Size       index,
     Priority&& new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >= nb_elements__) {
      GUM_ERROR(NotFound, "not enough elements in the MultiPriorityQueue");
    }

    // get the element itself
    const Val* val = heap__[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i && cmp__(new_priority, heap__[j].first);
         i = j, j = (j - 1) >> 1) {
      heap__[i]                       = std::move(heap__[j]);
      std::vector< Size >& vect_index = indices__[*(heap__[i].second)];
      for (auto& idx: vect_index) {
        if (idx == j) {
          idx = i;
          break;
        }
      }
    }

    // move val downward if needed
    for (Size j = (i << 1) + 1; j < nb_elements__; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < nb_elements__) && cmp__(heap__[j + 1].first, heap__[j].first))
        ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if (cmp__(new_priority, heap__[j].first)) break;

      // else pull up the jth node
      heap__[i]                       = std::move(heap__[j]);
      std::vector< Size >& vect_index = indices__[*(heap__[i].second)];
      for (auto& idx: vect_index) {
        if (idx == j) {
          idx = i;
          break;
        }
      }
    }

    // update the index of val
    heap__[i].first                 = std::move(new_priority);
    heap__[i].second                = val;
    std::vector< Size >& vect_index = indices__[*(heap__[i].second)];
    for (auto& idx: vect_index) {
      if (idx == index) {
        idx = i;
        break;
      }
    }

    return i;
  }

  // modifies the priority of each instance of a given element
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  void MultiPriorityQueue< Val, Priority, Cmp, Alloc >::setPriority(
     const Val&      elt,
     const Priority& new_priority) {
    std::vector< Size >& vect_index = indices__[elt];

    for (auto index: vect_index) {
      setPriorityByPos(index, new_priority);
    }
  }

  // modifies the priority of each instance of a given element
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const Priority& MultiPriorityQueue< Val, Priority, Cmp, Alloc >::priority(
     const Val& elt) const {
    return heap__[indices__[elt][0]].first;
  }

  // A \c << operator for priority queues
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE std::ostream&
         operator<<(std::ostream&                                          stream,
                const MultiPriorityQueue< Val, Priority, Cmp, Alloc >& queue) {
    stream << queue.toString();
    return stream;
  }

} /* namespace gum */
