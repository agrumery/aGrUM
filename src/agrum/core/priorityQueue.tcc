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
 * @brief template implementations of priority queues
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <string>
#include <sstream>
#include <agrum/core/priorityQueue.h>

namespace gum {

  /* =========================================================================== */
  /* ===             GENERAL IMPLEMENTATIION OF PRIORITY QUEUES              === */
  /* =========================================================================== */

  /// basic constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE PriorityQueueImplementation<Val, Priority, Cmp, Alloc,
                                     Gen>::PriorityQueueImplementation(Cmp compare,
                                                                       Size capacity)
      : __indices(capacity >> 1, true, true), __cmp(compare) {
    __heap.reserve(capacity);

    // for debugging purposes
    GUM_CONSTRUCTOR(PriorityQueueImplementation);
  }

  /// initializer list constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::
      PriorityQueueImplementation(
          std::initializer_list<std::pair<Val, Priority>> list)
      : __indices(list.size() / 2, true, true) {
    // fill the queue
    __heap.reserve(list.size());
    for (const auto &elt : list) {
      insert(elt.first, elt.second);
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(PriorityQueueImplementation);
  }

  /// copy constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::
      PriorityQueueImplementation(
          const PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen> &from)
      : __heap(from.__heap), __indices(from.__indices),
        __nb_elements(from.__nb_elements), __cmp(from.__cmp) {
    // fill the heap structure
    for (const auto &elt : __indices) {
      __heap[elt.second].second = &(elt.first);
    }

    // for debugging purposes
    GUM_CONS_CPY(PriorityQueueImplementation);
  }

  /// generalized copy constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  template <typename OtherAlloc>
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::
      PriorityQueueImplementation(const PriorityQueueImplementation<
          Val, Priority, Cmp, OtherAlloc, Gen> &from)
      : __indices(from.__indices), __nb_elements(from.__nb_elements),
        __cmp(from.__cmp) {
    // fill the heap structure
    if (__nb_elements) {
      __heap.reserve(from.__heap.size());
      for (const auto &elt : from.__heap) {
        __heap.push_back(elt);
      }
      for (const auto &elt : __indices) {
        __heap[elt.second].second = &(elt.first);
      }
    }

    // for debugging purposes
    GUM_CONS_CPY(PriorityQueueImplementation);
  }

  /// move constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::
      PriorityQueueImplementation(
          PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen> &&from)
      : __heap(std::move(from.__heap)), __indices(std::move(from.__indices)),
        __nb_elements(std::move(from.__nb_elements)), __cmp(std::move(from.__cmp)) {
    // for debugging purposes
    GUM_CONS_MOV(PriorityQueueImplementation);
  }

  /// destructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc,
                              Gen>::~PriorityQueueImplementation() {
    // for debugging purposes
    GUM_DESTRUCTOR(PriorityQueueImplementation);
  }

  /// copy operator
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen> &
      PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::
      operator=(
          const PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen> &from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_CPY(PriorityQueueImplementation);

      try {
        // set the comparison function
        __cmp = from.__cmp;

        // copy the indices and the heap
        __indices = from.__indices;
        __heap = from.__heap;
        __nb_elements = from.__nb_elements;

        // restore the link between __indices and __heap
        for (const auto &elt : __indices) {
          __heap[elt.second].second = &(elt.first);
        }
      } catch (...) {
        __heap.clear();
        __indices.clear();
        __nb_elements = 0;

        throw;
      }
    }

    return *this;
  }

  /// generalized copy operator
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  template <typename OtherAlloc>
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen> &
      PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::
      operator=(const PriorityQueueImplementation<Val, Priority, Cmp, OtherAlloc,
                                                  Gen> &from) {
    // for debugging purposes
    GUM_OP_CPY(PriorityQueueImplementation);

    try {
      // set the comprison function
      __cmp = from.__cmp;

      // copy the indices and the heap
      __indices = from.__indices;
      __nb_elements = from.__nb_elements;

      __heap.clear();
      if (__nb_elements) {
        __heap.reserve(from.__heap.size());
        for (const auto &elt : from.__heap) {
          __heap.push_back(elt);
        }
        for (const auto &elt : __indices) {
          __heap[elt.second].second = &(elt.first);
        }
      }
    } catch (...) {
      __heap.clear();
      __indices.clear();
      __nb_elements = 0;

      throw;
    }

    return *this;
  }

  /// move operator
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen> &
      PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::
      operator=(PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen> &&from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_MOV(PriorityQueueImplementation);

      __indices = std::move(from.__indices);
      __heap = std::move(from.__heap);
      __cmp = std::move(from.__cmp);
      __nb_elements = std::move(from.__nb_elements);
    }

    return *this;
  }

  /// returns the element at the top of the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE const Val &
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::top() const {
    if (!__nb_elements) {
      GUM_ERROR(NotFound, "empty priority queue");
    }

    return *(__heap[0].second);
  }

  /// returns the priority of the top element
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE const Priority &
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::topPriority() const {
    if (!__nb_elements) {
      GUM_ERROR(NotFound, "empty priority queue");
    }

    return __heap[0].first;
  }

  /// returns the number of elements in the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE Size
      PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::size() const
      noexcept {
    return __nb_elements;
  }

  /// return the size of the array storing the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE Size
      PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::capacity() const
      noexcept {
    return __heap.capacity();
  }

  /// changes the size of the array storing the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE void PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::resize(
      Size new_size) {
    if (new_size < __nb_elements)
      return;

    __heap.reserve(new_size);
    __indices.resize(new_size / 2);
  }

  /// removes all the elements from the queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE void PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::clear() {
    __nb_elements = 0;
    __heap.clear();
    __indices.clear();
  }

  /// removes the element at index elt from the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  void PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::eraseByPos(
      Size index) {
    if (index >= __nb_elements)
      return;

    // remove the element from the hashtable
    __indices.erase(*(__heap[index].second));

    // put the last element at the "index" location
    std::pair<Priority, const Val *> last = std::move(__heap[__nb_elements - 1]);
    __heap.pop_back();
    --__nb_elements;

    if (!__nb_elements || (index == __nb_elements))
      return;

    // restore the heap property
    Size i = index;

    for (Size j = (index << 1) + 1; j < __nb_elements; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < __nb_elements) && __cmp(__heap[j + 1].first, __heap[j].first))
        ++j;

      // if "last" is lower than heap[j], "last" must be stored at index i
      if (__cmp(last.first, __heap[j].first))
        break;

      // else pull up the jth node
      __heap[i] = std::move(__heap[j]);
      __indices[*(__heap[i].second)] = i;
    }

    // put "last" back into the heap
    __heap[i] = std::move(last);
    __indices[*(__heap[i].second)] = i;
  }

  /// removes a given element from the priority queue (but does not return it)
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE void PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::erase(
      const Val &val) {
    try {
      eraseByPos(__indices[val]);
    } catch (NotFound &) {
    }
  }

  /// removes the top of the priority queue (but does not return it)
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE void
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::eraseTop() {
    eraseByPos(0);
  }

  /// removes the top element from the priority queue and return it
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE Val PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::pop() {
    if (!__nb_elements) {
      GUM_ERROR(NotFound, "empty priority queue");
    }

    Val v = *(__heap[0].second);
    eraseByPos(0);

    return v;
  }

  /// returns a hashtable the keys of which are the values stored in the queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE const HashTable<Val, Size> &
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::allValues() const
      noexcept {
    return reinterpret_cast<const HashTable<Val, Size> &>(__indices);
  }

  /// inserts a new (a copy) element in the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE Size PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::insert(
      const Val &val, const Priority &priority) {
    // create the entry in the indices hashtable (if the element already exists,
    // __indices.insert will raise a Duplicateelement exception)
    typename HashTable<Val, Size, IndexAllocator>::value_type &new_elt =
        __indices.insert(val, 0);

    try {
      __heap.push_back(std::pair<Priority, const Val *>(priority, &new_elt.first));
    } catch (...) {
      __indices.erase(val);
      throw;
    }

    std::pair<Priority, const Val *> new_heap_val = std::move(__heap[__nb_elements]);
    ++__nb_elements;

    // restore the heap property
    Size i = __nb_elements - 1;

    for (Size j = (i - 1) >> 1; i && __cmp(new_heap_val.first, __heap[j].first);
         i = j, j = (j - 1) >> 1) {
      __heap[i] = std::move(__heap[j]);
      __indices[*(__heap[i].second)] = i;
    }

    // put the new bucket into the heap
    __heap[i].first = std::move(new_heap_val.first);
    __heap[i].second = &(new_elt.first);
    new_elt.second = i;

    return i;
  }

  /// inserts by move a new element in the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE Size PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::insert(
      Val &&val, Priority &&priority) {
    // create the entry in the indices hashtable (if the element already exists,
    // __indices.insert will raise a Duplicateelement exception)
    typename HashTable<Val, Size, IndexAllocator>::value_type &new_elt =
        __indices.insert(std::move(val), 0);

    try {
      __heap.push_back(
          std::pair<Priority, const Val *>(std::move(priority), &(new_elt.first)));
    } catch (...) {
      __indices.erase(new_elt.first);
      throw;
    }

    std::pair<Priority, const Val *> new_heap_val = std::move(__heap[__nb_elements]);
    ++__nb_elements;

    // restore the heap property
    Size i = __nb_elements - 1;

    for (Size j = (i - 1) >> 1; i && __cmp(new_heap_val.first, __heap[j].first);
         i = j, j = (j - 1) >> 1) {
      __heap[i] = std::move(__heap[j]);
      __indices[*(__heap[i].second)] = i;
    }

    // put the new bucket into the heap
    __heap[i].first = std::move(new_heap_val.first);
    __heap[i].second = &(new_elt.first);
    new_elt.second = i;

    return i;
  }

  /// emplace a new element into the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  template <typename... Args>
  INLINE Size PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::emplace(
      Args &&... args) {
    std::pair<Val, Priority> new_elt =
        std::make_pair<Val, Priority>(std::forward<Args>(args)...);
    return insert(std::move(new_elt.first), std::move(new_elt.second));
  }

  /// indicates whether the priority queue is empty
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE bool
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::empty() const
      noexcept {
    return (__nb_elements == 0);
  }

  /// indicates whether the priority queue contains a given value
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE bool PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::contains(
      const Val &val) const {
    return __indices.exists(val);
  }

  /// returns the element at position "index" in the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE const Val &PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::
  operator[](Size index) const {
    if (index >= __nb_elements) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation");
    }

    return *(__heap[index].second);
  }

  /// displays the content of the queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  std::string
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::toString() const {
    bool deja = false;
    std::stringstream stream;
    stream << "[";

    for (Size i = 0; i != __nb_elements; ++i, deja = true) {
      if (deja)
        stream << " , ";

      stream << "(" << __heap[i].first << " , " << *(__heap[i].second) << ")";
    }

    stream << "]";

    return stream.str();
  }

  /// changes the size of the internal structure storing the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  Size PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::setPriorityByPos(
      Size index, const Priority &new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >= __nb_elements) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation");
    }

    // get the element itself
    const Val *val = __heap[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i && __cmp(new_priority, __heap[j].first);
         i = j, j = (j - 1) >> 1) {
      __heap[i] = std::move(__heap[j]);
      __indices[*(__heap[i].second)] = i;
    }

    // move val downward if needed
    for (Size j = (i << 1) + 1; j < __nb_elements; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < __nb_elements) && __cmp(__heap[j + 1].first, __heap[j].first))
        ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if (__cmp(new_priority, __heap[j].first))
        break;

      // else pull up the jth node
      __heap[i] = std::move(__heap[j]);
      __indices[*(__heap[i].second)] = i;
    }

    // update the index of val
    __heap[i].first = new_priority;
    __heap[i].second = val;
    __indices[*val] = i;

    return i;
  }

  /// changes the size of the internal structure storing the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  Size PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::setPriorityByPos(
      Size index, Priority &&new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >= __nb_elements) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation");
    }

    // get the element itself
    const Val *val = __heap[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i && __cmp(new_priority, __heap[j].first);
         i = j, j = (j - 1) >> 1) {
      __heap[i] = std::move(__heap[j]);
      __indices[*(__heap[i].second)] = i;
    }

    // move val downward if needed
    for (Size j = (i << 1) + 1; j < __nb_elements; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < __nb_elements) && __cmp(__heap[j + 1].first, __heap[j].first))
        ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if (__cmp(new_priority, __heap[j].first))
        break;

      // else pull up the jth node
      __heap[i] = std::move(__heap[j]);
      __indices[*(__heap[i].second)] = i;
    }

    // update the index of val
    __heap[i].first = std::move(new_priority);
    __heap[i].second = val;
    __indices[*val] = i;

    return i;
  }

  /// modifies the priority of a given element
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  void PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::setPriority(
      const Val &elt, const Priority &new_priority) {
    setPriorityByPos(__indices[elt], new_priority);
  }

  /// modifies the priority of a given element
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  void PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::setPriority(
      const Val &elt, Priority &&new_priority) {
    setPriorityByPos(__indices[elt], std::move(new_priority));
  }

  /// returns the priority of a given element
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE const Priority &
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::priority(
      const Val &elt) const {
    return __heap[__indices[elt]].first;
  }

  /// returns the priority of a given element
  template <typename Val, typename Priority, typename Cmp, typename Alloc, bool Gen>
  INLINE const Priority &
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, Gen>::priorityByPos(
      Size index) const {
    if (index > __nb_elements) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation");
    }
    return __heap[index].first;
  }

  /* =========================================================================== */
  /* ===          SCALAR OPTIMIZED IMPLEMENTATION OF PRIORITY QUEUES         === */
  /* =========================================================================== */

  /// basic constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE PriorityQueueImplementation<
      Val, Priority, Cmp, Alloc, true>::PriorityQueueImplementation(Cmp compare,
                                                                    Size capacity)
      : __indices(capacity >> 1, true, true), __cmp(compare) {
    __heap.reserve(capacity);

    // for debugging purposes
    GUM_CONSTRUCTOR(PriorityQueueImplementation);
  }

  /// initializer list constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::
      PriorityQueueImplementation(
          std::initializer_list<std::pair<Val, Priority>> list)
      : __indices(list.size() / 2, true, true) {
    // fill the queue
    __heap.reserve(list.size());
    for (const auto &elt : list) {
      insert(elt.first, elt.second);
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(PriorityQueueImplementation);
  }

  /// copy constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::
      PriorityQueueImplementation(
          const PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true> &from)
      : __heap(from.__heap), __indices(from.__indices),
        __nb_elements(from.__nb_elements), __cmp(from.__cmp) {
    // for debugging purposes
    GUM_CONS_CPY(PriorityQueueImplementation);
  }

  /// generalized copy constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  template <typename OtherAlloc>
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::
      PriorityQueueImplementation(const PriorityQueueImplementation<
          Val, Priority, Cmp, OtherAlloc, true> &from)
      : __indices(from.__indices), __nb_elements(from.__nb_elements),
        __cmp(from.__cmp) {
    // fill the heap structure
    if (__nb_elements) {
      __heap.reserve(from.__heap.size());
      for (const auto &elt : from.__heap) {
        __heap.push_back(elt);
      }
    }

    // for debugging purposes
    GUM_CONS_CPY(PriorityQueueImplementation);
  }

  /// move constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::
      PriorityQueueImplementation(
          PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true> &&from)
      : __heap(std::move(from.__heap)), __indices(std::move(from.__indices)),
        __nb_elements(std::move(from.__nb_elements)), __cmp(std::move(from.__cmp)) {
    // for debugging purposes
    GUM_CONS_MOV(PriorityQueueImplementation);
  }

  /// destructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc,
                              true>::~PriorityQueueImplementation() {
    // for debugging purposes
    GUM_DESTRUCTOR(PriorityQueueImplementation);
  }

  /// copy operator
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true> &
      PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::
      operator=(
          const PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true> &from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_CPY(PriorityQueueImplementation);

      try {
        // set the comparison function
        __cmp = from.__cmp;

        // copy the indices and the heap
        __indices = from.__indices;
        __heap = from.__heap;
        __nb_elements = from.__nb_elements;
      } catch (...) {
        __heap.clear();
        __indices.clear();
        __nb_elements = 0;

        throw;
      }
    }

    return *this;
  }

  /// generalized copy operator
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  template <typename OtherAlloc>
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true> &
      PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::
      operator=(const PriorityQueueImplementation<Val, Priority, Cmp, OtherAlloc,
                                                  true> &from) {
    // for debugging purposes
    GUM_OP_CPY(PriorityQueueImplementation);

    try {
      // set the comprison function
      __cmp = from.__cmp;

      // copy the indices and the heap
      __indices = from.__indices;
      __nb_elements = from.__nb_elements;

      __heap.clear();
      if (__nb_elements) {
        __heap.reserve(from.__heap.size());
        for (const auto &elt : from.__heap) {
          __heap.push_back(elt);
        }
      }
    } catch (...) {
      __heap.clear();
      __indices.clear();
      __nb_elements = 0;

      throw;
    }

    return *this;
  }

  /// move operator
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true> &
      PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::
      operator=(
          PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true> &&from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_MOV(PriorityQueueImplementation);

      __indices = std::move(from.__indices);
      __heap = std::move(from.__heap);
      __cmp = std::move(from.__cmp);
      __nb_elements = std::move(from.__nb_elements);
    }

    return *this;
  }

  /// returns the element at the top of the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE const Val &
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::top() const {
    if (!__nb_elements) {
      GUM_ERROR(NotFound, "empty priority queue");
    }

    return __heap[0].second;
  }

  /// returns the priority of the top element
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE const Priority &
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::topPriority() const {
    if (!__nb_elements) {
      GUM_ERROR(NotFound, "empty priority queue");
    }

    return __heap[0].first;
  }

  /// returns the number of elements in the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE Size
      PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::size() const
      noexcept {
    return __nb_elements;
  }

  /// return the size of the array storing the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE Size
      PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::capacity() const
      noexcept {
    return __heap.capacity();
  }

  /// changes the size of the array storing the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE void PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::resize(
      Size new_size) {
    if (new_size < __nb_elements)
      return;

    __heap.reserve(new_size);
    __indices.resize(new_size / 2);
  }

  /// removes all the elements from the queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE void PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::clear() {
    __nb_elements = 0;
    __heap.clear();
    __indices.clear();
  }

  /// removes the element at index elt from the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  void PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::eraseByPos(
      Size index) {
    if (index >= __nb_elements)
      return;

    // remove the element from the hashtable
    __indices.erase(__heap[index].second);

    // put the last element at the "index" location
    std::pair<Priority, Val> last = std::move(__heap[__nb_elements - 1]);
    __heap.pop_back();
    --__nb_elements;

    if (!__nb_elements || (index == __nb_elements))
      return;

    // restore the heap property
    Size i = index;

    for (Size j = (index << 1) + 1; j < __nb_elements; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < __nb_elements) && __cmp(__heap[j + 1].first, __heap[j].first))
        ++j;

      // if "last" is lower than heap[j], "last" must be stored at index i
      if (__cmp(last.first, __heap[j].first))
        break;

      // else pull up the jth node
      __heap[i] = std::move(__heap[j]);
      __indices[__heap[i].second] = i;
    }

    // put "last" back into the heap
    __heap[i] = std::move(last);
    __indices[__heap[i].second] = i;
  }

  /// removes a given element from the priority queue (but does not return it)
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE void
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::erase(Val val) {
    try {
      eraseByPos(__indices[val]);
    } catch (NotFound &) {
    }
  }

  /// removes the top of the priority queue (but does not return it)
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE void
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::eraseTop() {
    eraseByPos(0);
  }

  /// removes the top element from the priority queue and return it
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE Val PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::pop() {
    if (!__nb_elements) {
      GUM_ERROR(NotFound, "empty priority queue");
    }

    Val v = __heap[0].second;
    eraseByPos(0);

    return v;
  }

  /// returns a hashtable the keys of which are the values stored in the queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE const HashTable<Val, Size> &
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::allValues() const
      noexcept {
    return reinterpret_cast<const HashTable<Val, Size> &>(__indices);
  }

  /// inserts a new (a copy) element in the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE Size PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::insert(
      Val val, const Priority &priority) {
    // create the entry in the indices hashtable (if the element already exists,
    // __indices.insert will raise a Duplicateelement exception)
    typename HashTable<Val, Size, IndexAllocator>::value_type &new_elt =
        __indices.insert(val, 0);

    try {
      __heap.push_back(std::pair<Priority, Val>(priority, val));
    } catch (...) {
      __indices.erase(val);
      throw;
    }

    std::pair<Priority, Val> new_heap_val = std::move(__heap[__nb_elements]);
    ++__nb_elements;

    // restore the heap property
    Size i = __nb_elements - 1;

    for (Size j = (i - 1) >> 1; i && __cmp(new_heap_val.first, __heap[j].first);
         i = j, j = (j - 1) >> 1) {
      __heap[i] = std::move(__heap[j]);
      __indices[__heap[i].second] = i;
    }

    // put the new bucket into the heap
    __heap[i].first = std::move(new_heap_val.first);
    __heap[i].second = val;
    new_elt.second = i;

    return i;
  }

  /// inserts by move a new element in the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE Size PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::insert(
      Val val, Priority &&priority) {
    // create the entry in the indices hashtable (if the element already exists,
    // __indices.insert will raise a Duplicateelement exception)
    typename HashTable<Val, Size, IndexAllocator>::value_type &new_elt =
        __indices.insert(val, 0);

    try {
      __heap.push_back(std::pair<Priority, Val>(std::move(priority), val));
    } catch (...) {
      __indices.erase(val);
      throw;
    }

    std::pair<Priority, Val> new_heap_val = std::move(__heap[__nb_elements]);
    ++__nb_elements;

    // restore the heap property
    Size i = __nb_elements - 1;

    for (Size j = (i - 1) >> 1; i && __cmp(new_heap_val.first, __heap[j].first);
         i = j, j = (j - 1) >> 1) {
      __heap[i] = std::move(__heap[j]);
      __indices[__heap[i].second] = i;
    }

    // put the new bucket into the heap
    __heap[i].first = std::move(new_heap_val.first);
    __heap[i].second = val;
    new_elt.second = i;

    return i;
  }

  /// emplace a new element into the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  template <typename... Args>
  INLINE Size PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::emplace(
      Args &&... args) {
    std::pair<Val, Priority> new_elt =
        std::make_pair<Val, Priority>(std::forward<Args>(args)...);
    return insert(new_elt.first, std::move(new_elt.second));
  }

  /// indicates whether the priority queue is empty
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE bool
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::empty() const
      noexcept {
    return (__nb_elements == 0);
  }

  /// indicates whether the priority queue contains a given value
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE bool PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::contains(
      Val val) const {
    return __indices.exists(val);
  }

  /// returns the element at position "index" in the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE const Val &PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::
  operator[](Size index) const {
    if (index >= __nb_elements) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation");
    }

    return __heap[index].second;
  }

  /// displays the content of the queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  std::string
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::toString() const {
    bool deja = false;
    std::stringstream stream;
    stream << "[";

    for (Size i = 0; i != __nb_elements; ++i, deja = true) {
      if (deja)
        stream << " , ";

      stream << "(" << __heap[i].first << " , " << __heap[i].second << ")";
    }

    stream << "]";

    return stream.str();
  }

  /// changes the size of the internal structure storing the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  Size
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::setPriorityByPos(
      Size index, const Priority &new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >= __nb_elements) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation");
    }

    // get the element itself
    Val val = __heap[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i && __cmp(new_priority, __heap[j].first);
         i = j, j = (j - 1) >> 1) {
      __heap[i] = std::move(__heap[j]);
      __indices[__heap[i].second] = i;
    }

    // move val downward if needed
    for (Size j = (i << 1) + 1; j < __nb_elements; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < __nb_elements) && __cmp(__heap[j + 1].first, __heap[j].first))
        ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if (__cmp(new_priority, __heap[j].first))
        break;

      // else pull up the jth node
      __heap[i] = std::move(__heap[j]);
      __indices[__heap[i].second] = i;
    }

    // update the index of val
    __heap[i].first = new_priority;
    __heap[i].second = val;
    __indices[val] = i;

    return i;
  }

  /// changes the size of the internal structure storing the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  Size
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::setPriorityByPos(
      Size index, Priority &&new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >= __nb_elements) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation");
    }

    // get the element itself
    Val val = __heap[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i && __cmp(new_priority, __heap[j].first);
         i = j, j = (j - 1) >> 1) {
      __heap[i] = std::move(__heap[j]);
      __indices[__heap[i].second] = i;
    }

    // move val downward if needed
    for (Size j = (i << 1) + 1; j < __nb_elements; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < __nb_elements) && __cmp(__heap[j + 1].first, __heap[j].first))
        ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if (__cmp(new_priority, __heap[j].first))
        break;

      // else pull up the jth node
      __heap[i] = std::move(__heap[j]);
      __indices[__heap[i].second] = i;
    }

    // update the index of val
    __heap[i].first = std::move(new_priority);
    __heap[i].second = val;
    __indices[val] = i;

    return i;
  }

  /// modifies the priority of a given element
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  void PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::setPriority(
      Val elt, const Priority &new_priority) {
    setPriorityByPos(__indices[elt], new_priority);
  }

  /// modifies the priority of a given element
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  void PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::setPriority(
      Val elt, Priority &&new_priority) {
    setPriorityByPos(__indices[elt], std::move(new_priority));
  }

  /// returns the priority of a given element
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE const Priority &
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::priority(
      Val elt) const {
    return __heap[__indices[elt]].first;
  }

  /// returns the priority of a given element
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE const Priority &
  PriorityQueueImplementation<Val, Priority, Cmp, Alloc, true>::priorityByPos(
      Size index) const {
    if (index > __nb_elements) {
      GUM_ERROR(NotFound, "not enough elements in the PriorityQueueImplementation");
    }
    return __heap[index].first;
  }

  /* =========================================================================== */
  /* ===                           PRIORITY QUEUES                           === */
  /* =========================================================================== */

  /// basic constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE PriorityQueue<Val, Priority, Cmp, Alloc>::PriorityQueue(Cmp cmp,
                                                                 Size capacity)
      : PriorityQueue<Val, Priority, Cmp, Alloc>::Implementation(cmp, capacity) {
    // for debugging purposes
    GUM_CONSTRUCTOR(PriorityQueue);
  }

  /// initializer list constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE PriorityQueue<Val, Priority, Cmp, Alloc>::PriorityQueue(
      std::initializer_list<std::pair<Val, Priority>> list)
      : PriorityQueue<Val, Priority, Cmp, Alloc>::Implementation{list} {
    // for debugging purposes
    GUM_CONSTRUCTOR(PriorityQueue);
  }

  /// copy constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE PriorityQueue<Val, Priority, Cmp, Alloc>::PriorityQueue(
      const PriorityQueue<Val, Priority, Cmp, Alloc> &from)
      : PriorityQueue<Val, Priority, Cmp, Alloc>::Implementation(from) {
    // for debugging purposes
    GUM_CONS_CPY(PriorityQueue);
  }

  /// generalized copy constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  template <typename OtherAlloc>
  INLINE PriorityQueue<Val, Priority, Cmp, Alloc>::PriorityQueue(
      const PriorityQueue<Val, Priority, Cmp, OtherAlloc> &from)
      : PriorityQueue<Val, Priority, Cmp, Alloc>::Implementation(from) {
    // for debugging purposes
    GUM_CONS_CPY(PriorityQueue);
  }

  /// move constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE PriorityQueue<Val, Priority, Cmp, Alloc>::PriorityQueue(
      PriorityQueue<Val, Priority, Cmp, Alloc> &&from)
      : PriorityQueue<Val, Priority, Cmp, Alloc>::Implementation(std::move(from)) {
    // for debugging purposes
    GUM_CONS_MOV(PriorityQueue);
  }

  /// destructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE PriorityQueue<Val, Priority, Cmp, Alloc>::~PriorityQueue() {
    // for debugging purposes
    GUM_DESTRUCTOR(PriorityQueue);
  }

  /// copy operator
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE PriorityQueue<Val, Priority, Cmp, Alloc> &
      PriorityQueue<Val, Priority, Cmp, Alloc>::
      operator=(const PriorityQueue<Val, Priority, Cmp, Alloc> &from) {
    Implementation::operator=(from);
    return *this;
  }

  /// generalized copy operator
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  template <typename OtherAlloc>
  INLINE PriorityQueue<Val, Priority, Cmp, Alloc> &
      PriorityQueue<Val, Priority, Cmp, Alloc>::
      operator=(const PriorityQueue<Val, Priority, Cmp, OtherAlloc> &from) {
    Implementation::operator=(from);
    return *this;
  }

  /// move operator
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE PriorityQueue<Val, Priority, Cmp, Alloc> &
      PriorityQueue<Val, Priority, Cmp, Alloc>::
      operator=(PriorityQueue<Val, Priority, Cmp, Alloc> &&from) {
    Implementation::operator=(std::move(from));
    return *this;
  }

  /// A \c << operator for priority queues
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE std::ostream &
  operator<<(std::ostream &stream,
             const PriorityQueue<Val, Priority, Cmp, Alloc> &queue) {
    stream << queue.toString();
    return stream;
  }

} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
