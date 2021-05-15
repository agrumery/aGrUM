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
 * @brief template implementations of priority queues
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/tools/core/priorityQueue.h>

namespace gum {

  // ===========================================================================
  // ===             GENERAL IMPLEMENTATIION OF PRIORITY QUEUES              ===
  // ===========================================================================

  // basic constructor
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::
     PriorityQueueImplementation(Cmp compare, Size capacity) :
      indices__(capacity >> 1, true, true),
      cmp__(compare) {
    heap__.reserve(capacity);

    GUM_CONSTRUCTOR(PriorityQueueImplementation);
  }

  // initializer list constructor
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::
     PriorityQueueImplementation(
        std::initializer_list< std::pair< Val, Priority > > list) :
      indices__(Size(list.size()) / 2, true, true) {
    // fill the queue
    heap__.reserve(list.size());
    for (const auto& elt: list) {
      insert(elt.first, elt.second);
    }

    GUM_CONSTRUCTOR(PriorityQueueImplementation);
  }

  // copy constructor
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::
     PriorityQueueImplementation(
        const PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >&
           from) :
      heap__(from.heap__),
      indices__(from.indices__), nb_elements__(from.nb_elements__),
      cmp__(from.cmp__) {
    // fill the heap structure
    for (const auto& elt: indices__) {
      heap__[elt.second].second = &(elt.first);
    }

    GUM_CONS_CPY(PriorityQueueImplementation);
  }

  // generalized copy constructor
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  template < typename OtherAlloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::
     PriorityQueueImplementation(
        const PriorityQueueImplementation< Val, Priority, Cmp, OtherAlloc, Gen >&
           from) :
      indices__(from.indices__),
      nb_elements__(from.nb_elements__), cmp__(from.cmp__) {
    // fill the heap structure
    if (nb_elements__) {
      heap__.reserve(from.heap__.size());
      for (const auto& elt: from.heap__) {
        heap__.push_back(elt);
      }
      for (const auto& elt: indices__) {
        heap__[elt.second].second = &(elt.first);
      }
    }

    GUM_CONS_CPY(PriorityQueueImplementation);
  }

  // move constructor
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::
     PriorityQueueImplementation(
        PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >&& from) :
      heap__(std::move(from.heap__)),
      indices__(std::move(from.indices__)),
      nb_elements__(std::move(from.nb_elements__)), cmp__(std::move(from.cmp__)) {
    GUM_CONS_MOV(PriorityQueueImplementation)
  }

  // destructor
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::
     ~PriorityQueueImplementation() {
    GUM_DESTRUCTOR(PriorityQueueImplementation);
  }

  // copy operator
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::operator=(
        const PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >&
           from) {
    // avoid self assignment
    if (this != &from) {
      GUM_OP_CPY(PriorityQueueImplementation)

      try {
        // set the comparison function
        cmp__ = from.cmp__;

        // copy the indices and the heap
        indices__     = from.indices__;
        heap__        = from.heap__;
        nb_elements__ = from.nb_elements__;

        // restore the link between indices__ and heap__
        for (const auto& elt: indices__) {
          heap__[elt.second].second = &(elt.first);
        }
      } catch (...) {
        heap__.clear();
        indices__.clear();
        nb_elements__ = 0;

        throw;
      }
    }

    return *this;
  }

  // generalized copy operator
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  template < typename OtherAlloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::operator=(
        const PriorityQueueImplementation< Val, Priority, Cmp, OtherAlloc, Gen >&
           from) {
    GUM_OP_CPY(PriorityQueueImplementation);

    try {
      // set the comprison function
      cmp__ = from.cmp__;

      // copy the indices and the heap
      indices__     = from.indices__;
      nb_elements__ = from.nb_elements__;

      heap__.clear();
      if (nb_elements__) {
        heap__.reserve(from.heap__.size());
        for (const auto& elt: from.heap__) {
          heap__.push_back(elt);
        }
        for (const auto& elt: indices__) {
          heap__[elt.second].second = &(elt.first);
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
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::operator=(
        PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >&& from) {
    // avoid self assignment
    if (this != &from) {
      GUM_OP_MOV(PriorityQueueImplementation)

      indices__     = std::move(from.indices__);
      heap__        = std::move(from.heap__);
      cmp__         = std::move(from.cmp__);
      nb_elements__ = std::move(from.nb_elements__);
    }

    return *this;
  }

  // returns the element at the top of the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE const Val&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::top() const {
    if (!nb_elements__) { GUM_ERROR(NotFound, "empty priority queue") }

    return *(heap__[0].second);
  }

  // returns the priority of the top element
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE const Priority&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::topPriority()
        const {
    if (!nb_elements__) { GUM_ERROR(NotFound, "empty priority queue") }

    return heap__[0].first;
  }

  // returns the number of elements in the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE Size PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::size()
     const noexcept {
    return nb_elements__;
  }

  // return the size of the array storing the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::capacity()
        const noexcept {
    return Size(heap__.capacity());
  }

  // changes the size of the array storing the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE void
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::resize(
        Size new_size) {
    if (new_size < nb_elements__) return;

    heap__.reserve(new_size);
    indices__.resize(new_size / 2);
  }

  // removes all the elements from the queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE void
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::clear() {
    nb_elements__ = 0;
    heap__.clear();
    indices__.clear();
  }

  // removes the element at index elt from the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  void PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::eraseByPos(
     Size index) {
    if (index >= nb_elements__) return;

    // remove the element from the hashtable
    indices__.erase(*(heap__[index].second));

    // put the last element at the "index" location
    std::pair< Priority, const Val* > last = std::move(heap__[nb_elements__ - 1]);
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
      heap__[i]                      = std::move(heap__[j]);
      indices__[*(heap__[i].second)] = i;
    }

    // put "last" back into the heap
    heap__[i]                      = std::move(last);
    indices__[*(heap__[i].second)] = i;
  }

  // removes a given element from the priority queue (but does not return it)
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE void PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::erase(
     const Val& val) {
    try {
      eraseByPos(indices__[val]);
    } catch (NotFound&) {}
  }

  // removes the top of the priority queue (but does not return it)
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE void
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::eraseTop() {
    eraseByPos(0);
  }

  // removes the top element from the priority queue and return it
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE Val PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::pop() {
    if (!nb_elements__) { GUM_ERROR(NotFound, "empty priority queue") }

    Val v = *(heap__[0].second);
    eraseByPos(0);

    return v;
  }

  // returns a hashtable the keys of which are the values stored in the queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE const HashTable< Val, Size >&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::allValues()
        const noexcept {
    return reinterpret_cast< const HashTable< Val, Size >& >(indices__);
  }

  // inserts a new (a copy) element in the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::insert(
        const Val&      val,
        const Priority& priority) {
    // create the entry in the indices hashtable (if the element already exists,
    // indices__.insert will raise a Duplicateelement exception)
    typename HashTable< Val, Size, IndexAllocator >::value_type& new_elt
       = indices__.insert(val, 0);

    try {
      heap__.push_back(
         std::pair< Priority, const Val* >(priority, &new_elt.first));
    } catch (...) {
      indices__.erase(val);
      throw;
    }

    std::pair< Priority, const Val* > new_heap_val
       = std::move(heap__[nb_elements__]);
    ++nb_elements__;

    // restore the heap property
    Size i = nb_elements__ - 1;

    for (Size j = (i - 1) >> 1; i && cmp__(new_heap_val.first, heap__[j].first);
         i = j, j = (j - 1) >> 1) {
      heap__[i]                      = std::move(heap__[j]);
      indices__[*(heap__[i].second)] = i;
    }

    // put the new bucket into the heap
    heap__[i].first  = std::move(new_heap_val.first);
    heap__[i].second = &(new_elt.first);
    new_elt.second   = i;

    return i;
  }

  // inserts by move a new element in the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::insert(
        Val&&      val,
        Priority&& priority) {
    // create the entry in the indices hashtable (if the element already exists,
    // indices__.insert will raise a Duplicateelement exception)
    typename HashTable< Val, Size, IndexAllocator >::value_type& new_elt
       = indices__.insert(std::move(val), 0);

    try {
      heap__.push_back(
         std::pair< Priority, const Val* >(std::move(priority), &(new_elt.first)));
    } catch (...) {
      indices__.erase(new_elt.first);
      throw;
    }

    std::pair< Priority, const Val* > new_heap_val
       = std::move(heap__[nb_elements__]);
    ++nb_elements__;

    // restore the heap property
    Size i = nb_elements__ - 1;

    for (Size j = (i - 1) >> 1; i && cmp__(new_heap_val.first, heap__[j].first);
         i = j, j = (j - 1) >> 1) {
      heap__[i]                      = std::move(heap__[j]);
      indices__[*(heap__[i].second)] = i;
    }

    // put the new bucket into the heap
    heap__[i].first  = std::move(new_heap_val.first);
    heap__[i].second = &(new_elt.first);
    new_elt.second   = i;

    return i;
  }

  // emplace a new element into the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  template < typename... Args >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::emplace(
        Args&&... args) {
    std::pair< Val, Priority > new_elt
       = std::make_pair< Val, Priority >(std::forward< Args >(args)...);
    return insert(std::move(new_elt.first), std::move(new_elt.second));
  }

  // indicates whether the priority queue is empty
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE bool
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::empty()
        const noexcept {
    return (nb_elements__ == 0);
  }

  // indicates whether the priority queue contains a given value
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE bool
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::contains(
        const Val& val) const {
    return indices__.exists(val);
  }

  // returns the element at position "index" in the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE const Val&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::operator[](
        Size index) const {
    if (index >= nb_elements__) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation")
    }

    return *(heap__[index].second);
  }

  // displays the content of the queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  std::string
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::toString()
        const {
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
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  Size PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::
     setPriorityByPos(Size index, const Priority& new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >= nb_elements__) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation")
    }

    // get the element itself
    const Val* val = heap__[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i && cmp__(new_priority, heap__[j].first);
         i = j, j = (j - 1) >> 1) {
      heap__[i]                      = std::move(heap__[j]);
      indices__[*(heap__[i].second)] = i;
    }

    // move val downward if needed
    for (Size j = (i << 1) + 1; j < nb_elements__; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < nb_elements__) && cmp__(heap__[j + 1].first, heap__[j].first))
        ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if (cmp__(new_priority, heap__[j].first)) break;

      // else pull up the jth node
      heap__[i]                      = std::move(heap__[j]);
      indices__[*(heap__[i].second)] = i;
    }

    // update the index of val
    heap__[i].first  = new_priority;
    heap__[i].second = val;
    indices__[*val]  = i;

    return i;
  }

  // changes the size of the internal structure storing the priority queue
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  Size PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::
     setPriorityByPos(Size index, Priority&& new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >= nb_elements__) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation")
    }

    // get the element itself
    const Val* val = heap__[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i && cmp__(new_priority, heap__[j].first);
         i = j, j = (j - 1) >> 1) {
      heap__[i]                      = std::move(heap__[j]);
      indices__[*(heap__[i].second)] = i;
    }

    // move val downward if needed
    for (Size j = (i << 1) + 1; j < nb_elements__; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < nb_elements__) && cmp__(heap__[j + 1].first, heap__[j].first))
        ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if (cmp__(new_priority, heap__[j].first)) break;

      // else pull up the jth node
      heap__[i]                      = std::move(heap__[j]);
      indices__[*(heap__[i].second)] = i;
    }

    // update the index of val
    heap__[i].first  = std::move(new_priority);
    heap__[i].second = val;
    indices__[*val]  = i;

    return i;
  }

  // modifies the priority of a given element
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  void PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::setPriority(
     const Val&      elt,
     const Priority& new_priority) {
    setPriorityByPos(indices__[elt], new_priority);
  }

  // modifies the priority of a given element
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  void PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::setPriority(
     const Val& elt,
     Priority&& new_priority) {
    setPriorityByPos(indices__[elt], std::move(new_priority));
  }

  // returns the priority of a given element
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE const Priority&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::priority(
        const Val& elt) const {
    return heap__[indices__[elt]].first;
  }

  // returns the priority of a given element
  template < typename Val,
             typename Priority,
             typename Cmp,
             typename Alloc,
             bool Gen >
  INLINE const Priority&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, Gen >::priorityByPos(
        Size index) const {
    if (index > nb_elements__) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation")
    }
    return heap__[index].first;
  }

  // ===========================================================================
  // ===          SCALAR OPTIMIZED IMPLEMENTATION OF PRIORITY QUEUES         ===
  // ===========================================================================

  // basic constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::
     PriorityQueueImplementation(Cmp compare, Size capacity) :
      indices__(capacity >> 1, true, true),
      cmp__(compare) {
    heap__.reserve(capacity);

    // for debugging purposes
    GUM_CONSTRUCTOR(PriorityQueueImplementation);
  }

  // initializer list constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::
     PriorityQueueImplementation(
        std::initializer_list< std::pair< Val, Priority > > list) :
      indices__(Size(list.size()) / 2, true, true) {
    // fill the queue
    heap__.reserve(list.size());
    for (const auto& elt: list) {
      insert(elt.first, elt.second);
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(PriorityQueueImplementation);
  }

  // copy constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::
     PriorityQueueImplementation(
        const PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >&
           from) :
      heap__(from.heap__),
      indices__(from.indices__), nb_elements__(from.nb_elements__),
      cmp__(from.cmp__) {
    // for debugging purposes
    GUM_CONS_CPY(PriorityQueueImplementation);
  }

  // generalized copy constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  template < typename OtherAlloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::
     PriorityQueueImplementation(
        const PriorityQueueImplementation< Val, Priority, Cmp, OtherAlloc, true >&
           from) :
      indices__(from.indices__),
      nb_elements__(from.nb_elements__), cmp__(from.cmp__) {
    // fill the heap structure
    if (nb_elements__) {
      heap__.reserve(from.heap__.size());
      for (const auto& elt: from.heap__) {
        heap__.push_back(elt);
      }
    }

    // for debugging purposes
    GUM_CONS_CPY(PriorityQueueImplementation);
  }

  // move constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::
     PriorityQueueImplementation(
        PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >&& from) :
      heap__(std::move(from.heap__)),
      indices__(std::move(from.indices__)),
      nb_elements__(std::move(from.nb_elements__)), cmp__(std::move(from.cmp__)) {
    // for debugging purposes
    GUM_CONS_MOV(PriorityQueueImplementation);
  }

  // destructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::
     ~PriorityQueueImplementation() {
    // for debugging purposes
    GUM_DESTRUCTOR(PriorityQueueImplementation);
  }

  // copy operator
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::operator=(
        const PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >&
           from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_CPY(PriorityQueueImplementation);

      try {
        // set the comparison function
        cmp__ = from.cmp__;

        // copy the indices and the heap
        indices__     = from.indices__;
        heap__        = from.heap__;
        nb_elements__ = from.nb_elements__;
      } catch (...) {
        heap__.clear();
        indices__.clear();
        nb_elements__ = 0;

        throw;
      }
    }

    return *this;
  }

  // generalized copy operator
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  template < typename OtherAlloc >
  PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::operator=(
        const PriorityQueueImplementation< Val, Priority, Cmp, OtherAlloc, true >&
           from) {
    // for debugging purposes
    GUM_OP_CPY(PriorityQueueImplementation);

    try {
      // set the comprison function
      cmp__ = from.cmp__;

      // copy the indices and the heap
      indices__     = from.indices__;
      nb_elements__ = from.nb_elements__;

      heap__.clear();
      if (nb_elements__) {
        heap__.reserve(from.heap__.size());
        for (const auto& elt: from.heap__) {
          heap__.push_back(elt);
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
  INLINE PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::operator=(
        PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >&& from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_MOV(PriorityQueueImplementation);

      indices__     = std::move(from.indices__);
      heap__        = std::move(from.heap__);
      cmp__         = std::move(from.cmp__);
      nb_elements__ = std::move(from.nb_elements__);
    }

    return *this;
  }

  // returns the element at the top of the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const Val&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::top() const {
    if (!nb_elements__) { GUM_ERROR(NotFound, "empty priority queue") }

    return heap__[0].second;
  }

  // returns the priority of the top element
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const Priority&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::topPriority()
        const {
    if (!nb_elements__) { GUM_ERROR(NotFound, "empty priority queue") }

    return heap__[0].first;
  }

  // returns the number of elements in the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::size()
        const noexcept {
    return nb_elements__;
  }

  // return the size of the array storing the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::capacity()
        const noexcept {
    return Size(heap__.capacity());
  }

  // changes the size of the array storing the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE void
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::resize(
        Size new_size) {
    if (new_size < nb_elements__) return;

    heap__.reserve(new_size);
    indices__.resize(new_size / 2);
  }

  // removes all the elements from the queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE void
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::clear() {
    nb_elements__ = 0;
    heap__.clear();
    indices__.clear();
  }

  // removes the element at index elt from the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  void PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::eraseByPos(
     Size index) {
    if (index >= nb_elements__) return;

    // remove the element from the hashtable
    indices__.erase(heap__[index].second);

    // put the last element at the "index" location
    std::pair< Priority, Val > last = std::move(heap__[nb_elements__ - 1]);
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
      heap__[i]                   = std::move(heap__[j]);
      indices__[heap__[i].second] = i;
    }

    // put "last" back into the heap
    heap__[i]                   = std::move(last);
    indices__[heap__[i].second] = i;
  }

  // removes a given element from the priority queue (but does not return it)
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE void
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::erase(
        Val val) {
    try {
      eraseByPos(indices__[val]);
    } catch (NotFound&) {}
  }

  // removes the top of the priority queue (but does not return it)
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE void
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::eraseTop() {
    eraseByPos(0);
  }

  // removes the top element from the priority queue and return it
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE Val
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::pop() {
    if (!nb_elements__) { GUM_ERROR(NotFound, "empty priority queue") }

    Val v = heap__[0].second;
    eraseByPos(0);

    return v;
  }

  // returns a hashtable the keys of which are the values stored in the queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const HashTable< Val, Size >&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::allValues()
        const noexcept {
    return reinterpret_cast< const HashTable< Val, Size >& >(indices__);
  }

  // inserts a new (a copy) element in the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::insert(
        Val             val,
        const Priority& priority) {
    // create the entry in the indices hashtable (if the element already exists,
    // indices__.insert will raise a Duplicateelement exception)
    typename HashTable< Val, Size, IndexAllocator >::value_type& new_elt
       = indices__.insert(val, 0);

    try {
      heap__.push_back(std::pair< Priority, Val >(priority, val));
    } catch (...) {
      indices__.erase(val);
      throw;
    }

    std::pair< Priority, Val > new_heap_val = std::move(heap__[nb_elements__]);
    ++nb_elements__;

    // restore the heap property
    Size i = nb_elements__ - 1;

    for (Size j = (i - 1) >> 1; i && cmp__(new_heap_val.first, heap__[j].first);
         i = j, j = (j - 1) >> 1) {
      heap__[i]                   = std::move(heap__[j]);
      indices__[heap__[i].second] = i;
    }

    // put the new bucket into the heap
    heap__[i].first  = std::move(new_heap_val.first);
    heap__[i].second = val;
    new_elt.second   = i;

    return i;
  }

  // inserts by move a new element in the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::insert(
        Val        val,
        Priority&& priority) {
    // create the entry in the indices hashtable (if the element already exists,
    // indices__.insert will raise a Duplicateelement exception)
    typename HashTable< Val, Size, IndexAllocator >::value_type& new_elt
       = indices__.insert(val, 0);

    try {
      heap__.push_back(std::pair< Priority, Val >(std::move(priority), val));
    } catch (...) {
      indices__.erase(val);
      throw;
    }

    std::pair< Priority, Val > new_heap_val = std::move(heap__[nb_elements__]);
    ++nb_elements__;

    // restore the heap property
    Size i = nb_elements__ - 1;

    for (Size j = (i - 1) >> 1; i && cmp__(new_heap_val.first, heap__[j].first);
         i = j, j = (j - 1) >> 1) {
      heap__[i]                   = std::move(heap__[j]);
      indices__[heap__[i].second] = i;
    }

    // put the new bucket into the heap
    heap__[i].first  = std::move(new_heap_val.first);
    heap__[i].second = val;
    new_elt.second   = i;

    return i;
  }

  // emplace a new element into the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  template < typename... Args >
  INLINE Size
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::emplace(
        Args&&... args) {
    std::pair< Val, Priority > new_elt
       = std::make_pair< Val, Priority >(std::forward< Args >(args)...);
    return insert(new_elt.first, std::move(new_elt.second));
  }

  // indicates whether the priority queue is empty
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE bool
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::empty()
        const noexcept {
    return (nb_elements__ == 0);
  }

  // indicates whether the priority queue contains a given value
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE bool
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::contains(
        Val val) const {
    return indices__.exists(val);
  }

  // returns the element at position "index" in the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const Val&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::operator[](
        Size index) const {
    if (index >= nb_elements__) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation")
    }

    return heap__[index].second;
  }

  // displays the content of the queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  std::string
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::toString()
        const {
    bool              deja = false;
    std::stringstream stream;
    stream << "[";

    for (Size i = 0; i != nb_elements__; ++i, deja = true) {
      if (deja) stream << " , ";

      stream << "(" << heap__[i].first << " , " << heap__[i].second << ")";
    }

    stream << "]";

    return stream.str();
  }

  // changes the size of the internal structure storing the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  Size PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::
     setPriorityByPos(Size index, const Priority& new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >= nb_elements__) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation")
    }

    // get the element itself
    Val val = heap__[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i && cmp__(new_priority, heap__[j].first);
         i = j, j = (j - 1) >> 1) {
      heap__[i]                   = std::move(heap__[j]);
      indices__[heap__[i].second] = i;
    }

    // move val downward if needed
    for (Size j = (i << 1) + 1; j < nb_elements__; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < nb_elements__) && cmp__(heap__[j + 1].first, heap__[j].first))
        ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if (cmp__(new_priority, heap__[j].first)) break;

      // else pull up the jth node
      heap__[i]                   = std::move(heap__[j]);
      indices__[heap__[i].second] = i;
    }

    // update the index of val
    heap__[i].first  = new_priority;
    heap__[i].second = val;
    indices__[val]   = i;

    return i;
  }

  // changes the size of the internal structure storing the priority queue
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  Size PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::
     setPriorityByPos(Size index, Priority&& new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >= nb_elements__) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation")
    }

    // get the element itself
    Val val = heap__[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i && cmp__(new_priority, heap__[j].first);
         i = j, j = (j - 1) >> 1) {
      heap__[i]                   = std::move(heap__[j]);
      indices__[heap__[i].second] = i;
    }

    // move val downward if needed
    for (Size j = (i << 1) + 1; j < nb_elements__; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < nb_elements__) && cmp__(heap__[j + 1].first, heap__[j].first))
        ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if (cmp__(new_priority, heap__[j].first)) break;

      // else pull up the jth node
      heap__[i]                   = std::move(heap__[j]);
      indices__[heap__[i].second] = i;
    }

    // update the index of val
    heap__[i].first  = std::move(new_priority);
    heap__[i].second = val;
    indices__[val]   = i;

    return i;
  }

  // modifies the priority of a given element
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  void PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::setPriority(
     Val             elt,
     const Priority& new_priority) {
    setPriorityByPos(indices__[elt], new_priority);
  }

  // modifies the priority of a given element
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  void PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::setPriority(
     Val        elt,
     Priority&& new_priority) {
    setPriorityByPos(indices__[elt], std::move(new_priority));
  }

  // returns the priority of a given element
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const Priority&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::priority(
        Val elt) const {
    return heap__[indices__[elt]].first;
  }

  // returns the priority of a given element
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE const Priority&
     PriorityQueueImplementation< Val, Priority, Cmp, Alloc, true >::priorityByPos(
        Size index) const {
    if (index > nb_elements__) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation")
    }
    return heap__[index].first;
  }

  // ===========================================================================
  // ===                           PRIORITY QUEUES                           ===
  // ===========================================================================

  // basic constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >::PriorityQueue(Cmp  cmp,
                                                                   Size capacity) :
      PriorityQueue< Val, Priority, Cmp, Alloc >::Implementation(cmp, capacity) {
    // for debugging purposes
    GUM_CONSTRUCTOR(PriorityQueue);
  }

  // initializer list constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >::PriorityQueue(
     std::initializer_list< std::pair< Val, Priority > > list) :
      PriorityQueue< Val, Priority, Cmp, Alloc >::Implementation{list} {
    // for debugging purposes
    GUM_CONSTRUCTOR(PriorityQueue);
  }

  // copy constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >::PriorityQueue(
     const PriorityQueue< Val, Priority, Cmp, Alloc >& from) :
      PriorityQueue< Val, Priority, Cmp, Alloc >::Implementation(from) {
    // for debugging purposes
    GUM_CONS_CPY(PriorityQueue);
  }

  // generalized copy constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  template < typename OtherAlloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >::PriorityQueue(
     const PriorityQueue< Val, Priority, Cmp, OtherAlloc >& from) :
      PriorityQueue< Val, Priority, Cmp, Alloc >::Implementation(from) {
    // for debugging purposes
    GUM_CONS_CPY(PriorityQueue);
  }

  // move constructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >::PriorityQueue(
     PriorityQueue< Val, Priority, Cmp, Alloc >&& from) :
      PriorityQueue< Val, Priority, Cmp, Alloc >::Implementation(std::move(from)) {
    // for debugging purposes
    GUM_CONS_MOV(PriorityQueue);
  }

  // destructor
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >::~PriorityQueue() {
    // for debugging purposes
    GUM_DESTRUCTOR(PriorityQueue);
  }

  // copy operator
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >&
     PriorityQueue< Val, Priority, Cmp, Alloc >::operator=(
        const PriorityQueue< Val, Priority, Cmp, Alloc >& from) {
    Implementation::operator=(from);
    return *this;
  }

  // generalized copy operator
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  template < typename OtherAlloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >&
     PriorityQueue< Val, Priority, Cmp, Alloc >::operator=(
        const PriorityQueue< Val, Priority, Cmp, OtherAlloc >& from) {
    Implementation::operator=(from);
    return *this;
  }

  // move operator
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE PriorityQueue< Val, Priority, Cmp, Alloc >&
     PriorityQueue< Val, Priority, Cmp, Alloc >::operator=(
        PriorityQueue< Val, Priority, Cmp, Alloc >&& from) {
    Implementation::operator=(std::move(from));
    return *this;
  }

  // A \c << operator for priority queues
  template < typename Val, typename Priority, typename Cmp, typename Alloc >
  INLINE std::ostream&
         operator<<(std::ostream&                                     stream,
                const PriorityQueue< Val, Priority, Cmp, Alloc >& queue) {
    stream << queue.toString();
    return stream;
  }

} /* namespace gum */
