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
 * @brief template implementations of multi priority queues
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <string>
#include <sstream>
#include "priorityQueue.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /// basic constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  MultiPriorityQueue<Val, Priority, Cmp, Alloc>::MultiPriorityQueue(Cmp compare,
                                                                    Size capacity)
      : __indices(capacity >> 1, true, false), __cmp(compare) {
    __heap.reserve(capacity);

    // for debugging purposes
    GUM_CONSTRUCTOR(MultiPriorityQueue);
  }

  /// initializer list constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  MultiPriorityQueue<Val, Priority, Cmp, Alloc>::MultiPriorityQueue(
      std::initializer_list<std::pair<Val, Priority>> list)
      : __indices(list.size() / 2, true, false) {
    // fill the queue
    __heap.reserve(list.size());
    for (const auto &elt : list) {
      insert(elt.first, elt.second);
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(MultiPriorityQueue);
  }

  /// copy constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  MultiPriorityQueue<Val, Priority, Cmp, Alloc>::MultiPriorityQueue(
      const MultiPriorityQueue<Val, Priority, Cmp, Alloc> &from)
      : __heap(from.__heap), __indices(from.__indices),
        __nb_elements(from.__nb_elements), __cmp(from.__cmp) {
    // for debugging purposes
    GUM_CONS_CPY(MultiPriorityQueue);

    // fill the heap structure
    for (const auto &val_and_index : __indices) {
      const Val *val = &(val_and_index.first);
      const std::vector<Size> &vect = val_and_index.second;
      for (auto index : vect) {
        __heap[index].second = val;
      }
    }
  }

  /// generalized copy constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  template <typename OtherAlloc>
  MultiPriorityQueue<Val, Priority, Cmp, Alloc>::MultiPriorityQueue(
      const MultiPriorityQueue<Val, Priority, Cmp, OtherAlloc> &from)
      : __indices(from.__indices), __nb_elements(from.__nb_elements),
        __cmp(from.__cmp) {
    // for debugging purposes
    GUM_CONS_CPY(MultiPriorityQueue);

    // fill the heap structure
    if (__nb_elements) {
      __heap.reserve(from.__heap.size());
      for (const auto &elt : from.__heap) {
        __heap.push_back(elt);
      }
      for (const auto &val_and_index : __indices) {
        const Val *val = &(val_and_index.first);
        const std::vector<Size> &vect = val_and_index.second;
        for (auto index : vect) {
          __heap[index].second = val;
        }
      }
    }
  }

  /// move constructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  MultiPriorityQueue<Val, Priority, Cmp, Alloc>::MultiPriorityQueue(
      MultiPriorityQueue<Val, Priority, Cmp, Alloc> &&from)
      : __heap(std::move(from.__heap)), __indices(std::move(from.__indices)),
        __nb_elements(std::move(from.__nb_elements)), __cmp(std::move(from.__cmp)) {
    // for debugging purposes
    GUM_CONS_MOV(MultiPriorityQueue);
  }

  /// destructor
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  MultiPriorityQueue<Val, Priority, Cmp, Alloc>::~MultiPriorityQueue() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiPriorityQueue);
  }

  /// copy operator
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  MultiPriorityQueue<Val, Priority, Cmp, Alloc> &
      MultiPriorityQueue<Val, Priority, Cmp, Alloc>::
      operator=(const MultiPriorityQueue<Val, Priority, Cmp, Alloc> &from) {
    // for debugging purposes
    GUM_OP_CPY(MultiPriorityQueue);

    try {
      // set the comprison function
      __cmp = from.__cmp;

      // copy the indices and the heap
      __indices = from.__indices;
      __heap = from.__heap;
      __nb_elements = from.__nb_elements;

      // restore the link between __indices and __heap
      for (const auto &val_and_index : __indices) {
        const Val *val = &(val_and_index.first);
        const std::vector<Size> &vect = val_and_index.second;
        for (auto index : vect) {
          __heap[index].second = val;
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

  /// generalized copy operator
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  template <typename OtherAlloc>
  MultiPriorityQueue<Val, Priority, Cmp, Alloc> &
      MultiPriorityQueue<Val, Priority, Cmp, Alloc>::
      operator=(const MultiPriorityQueue<Val, Priority, Cmp, OtherAlloc> &from) {
    // for debugging purposes
    GUM_OP_CPY(MultiPriorityQueue);

    try {
      // set the comprison function
      __cmp = from.__cmp;

      // copy the indices and the heap
      __indices = from.__indices;
      __nb_elements = from.__nb_elements;

      // restore the link between __indices and __heap
      __heap.clear();
      __heap.reserve(from.__heap.size());
      for (const auto &elt : from.__heap) {
        __heap.push_back(elt);
      }
      for (const auto &val_and_index : __indices) {
        const Val *val = &(val_and_index.first);
        const std::vector<Size> &vect = val_and_index.second;
        for (auto index : vect) {
          __heap[index].second = val;
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
  MultiPriorityQueue<Val, Priority, Cmp, Alloc> &
      MultiPriorityQueue<Val, Priority, Cmp, Alloc>::
      operator=(MultiPriorityQueue<Val, Priority, Cmp, Alloc> &&from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_MOV(MultiPriorityQueue);

      __cmp = std::move(from.__cmp);
      __indices = std::move(from.__indices);
      __heap = std::move(from.__heap);
      __nb_elements = std::move(from.__nb_elements);
    }

    return *this;
  }

  /// returns the element at the top of the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE const Val &MultiPriorityQueue<Val, Priority, Cmp, Alloc>::top() const {
    if (!__nb_elements) {
      GUM_ERROR(NotFound, "empty priority queue");
    }

    return *(__heap[0].second);
  }

  /// returns the priority of the top element
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE const Priority &
  MultiPriorityQueue<Val, Priority, Cmp, Alloc>::topPriority() const {
    if (!__nb_elements) {
      GUM_ERROR(NotFound, "empty priority queue");
    }

    return __heap[0].first;
  }

  /// returns the number of elements in the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE Size MultiPriorityQueue<Val, Priority, Cmp, Alloc>::size() const noexcept {
    return __nb_elements;
  }

  /// return the size of the array storing the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE Size MultiPriorityQueue<Val, Priority, Cmp, Alloc>::capacity() const
      noexcept {
    return __heap.capacity();
  }

  /// changes the size of the array storing the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE void MultiPriorityQueue<Val, Priority, Cmp, Alloc>::resize(Size new_size) {
    if (new_size < __nb_elements)
      return;

    __heap.reserve(new_size);
    __indices.resize(new_size / 2);
  }

  /// removes all the elements from the queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  void MultiPriorityQueue<Val, Priority, Cmp, Alloc>::clear() {
    __nb_elements = 0;
    __heap.clear();
    __indices.clear();
  }

  /// removes the element at index elt from the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  void MultiPriorityQueue<Val, Priority, Cmp, Alloc>::eraseByPos(Size index) {
    if (index >= __nb_elements)
      return;

    // remove the element from the hashtable
    const Val &del_val = *(__heap[index].second);
    std::vector<Size> &vect_index = __indices[del_val];
    if (vect_index.size() == 1)
      __indices.erase(del_val);
    else {
      for (auto &v_index : vect_index) {
        if (v_index == index) {
          v_index = vect_index.back();
          vect_index.pop_back();
          break;
        }
      }
    }

    // put the last element at the "index" location
    std::pair<Priority, const Val *> last = std::move(__heap.back());
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
      std::vector<Size> &vect_index = __indices[*(__heap[i].second)];
      for (auto &v_index : vect_index) {
        if (v_index == j) {
          v_index = i;
          break;
        }
      }
    }

    // put "last" back into the heap
    __heap[i] = std::move(last);
    std::vector<Size> &last_indices = __indices[*(__heap[i].second)];
    for (auto &v_index : last_indices) {
      if (v_index == __nb_elements) {
        v_index = i;
        break;
      }
    }
  }

  /// removes a given element from the priority queue (but does not return it)
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE void MultiPriorityQueue<Val, Priority, Cmp, Alloc>::erase(const Val &val) {
    try {
      eraseByPos(__indices[val][0]);
    } catch (NotFound &) {
    }
  }

  /// removes the top of the priority queue (but does not return it)
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE void MultiPriorityQueue<Val, Priority, Cmp, Alloc>::eraseTop() {
    eraseByPos(0);
  }

  /// removes the top element from the priority queue and return it
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE Val MultiPriorityQueue<Val, Priority, Cmp, Alloc>::pop() {
    if (!__nb_elements) {
      GUM_ERROR(NotFound, "empty priority queue");
    }

    Val v = *(__heap[0].second);
    eraseByPos(0);

    return v;
  }

  /// returns a hashtable the keys of which are the values stored in the queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE const HashTable<Val, std::vector<Size>> &
  MultiPriorityQueue<Val, Priority, Cmp, Alloc>::allValues() const {
    return reinterpret_cast<const HashTable<Val, std::vector<Size>> &>(__indices);
  }

  /// inserts a new (a copy) element in the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  Size
  MultiPriorityQueue<Val, Priority, Cmp, Alloc>::insert(const Val &val,
                                                        const Priority &priority) {
    // create the entry in the indices hashtable
    const Val *new_val;
    std::vector<Size> *new_vect;
    if (!__indices.exists(val)) {
      auto &new_elt = __indices.insert(val, std::vector<Size>());
      new_val = &(new_elt.first);
      new_vect = &(new_elt.second);
    } else {
      new_val = &(__indices.key(val));
      new_vect = &(__indices[val]);
    }

    try {
      new_vect->push_back(0);
    } catch (...) {
      if (new_vect->empty()) {
        __indices.erase(val);
      }
      throw;
    }

    try {
      __heap.push_back(std::pair<Priority, const Val *>(priority, new_val));
    } catch (...) {
      if (new_vect->size() == 1) {
        __indices.erase(val);
      }
      throw;
    }

    std::pair<Priority, const Val *> new_heap_val = std::move(__heap[__nb_elements]);
    ++__nb_elements;

    // restore the heap property
    Size i = __nb_elements - 1;

    for (Size j = (i - 1) >> 1; i && __cmp(new_heap_val.first, __heap[j].first);
         i = j, j = (j - 1) >> 1) {
      __heap[i] = std::move(__heap[j]);
      std::vector<Size> &vect_index = __indices[*(__heap[i].second)];
      for (auto &index : vect_index) {
        if (index == j) {
          index = i;
          break;
        }
      }
    }

    // put the new bucket into the heap
    __heap[i].first = std::move(new_heap_val.first);
    __heap[i].second = new_val;
    new_vect->back() = i;

    return i;
  }

  /// inserts a new (a copy) element in the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  Size MultiPriorityQueue<Val, Priority, Cmp, Alloc>::insert(Val &&val,
                                                             Priority &&priority) {
    // create the entry in the indices hashtable
    const Val *new_val;
    std::vector<Size> *new_vect;
    if (!__indices.exists(val)) {
      auto &new_elt = __indices.insert(std::move(val), std::vector<Size>());
      new_val = &(new_elt.first);
      new_vect = &(new_elt.second);
    } else {
      new_val = &(__indices.key(val));
      new_vect = &(__indices[val]);
    }

    try {
      new_vect->push_back(0);
    } catch (...) {
      if (new_vect->empty()) {
        __indices.erase(*new_val);
      }
      throw;
    }

    try {
      __heap.push_back(
          std::pair<Priority, const Val *>(std::move(priority), new_val));
    } catch (...) {
      if (new_vect->size() == 1) {
        __indices.erase(*new_val);
      }
      throw;
    }

    std::pair<Priority, const Val *> new_heap_val = std::move(__heap[__nb_elements]);
    ++__nb_elements;

    // restore the heap property
    Size i = __nb_elements - 1;

    for (Size j = (i - 1) >> 1; i && __cmp(new_heap_val.first, __heap[j].first);
         i = j, j = (j - 1) >> 1) {
      __heap[i] = std::move(__heap[j]);
      std::vector<Size> &vect_index = __indices[*(__heap[i].second)];
      for (auto &index : vect_index) {
        if (index == j) {
          index = i;
          break;
        }
      }
    }

    // put the new bucket into the heap
    __heap[i].first = std::move(new_heap_val.first);
    __heap[i].second = new_val;
    new_vect->back() = i;

    return i;
  }

  /// emplace a new element into the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  template <typename... Args>
  INLINE Size
      MultiPriorityQueue<Val, Priority, Cmp, Alloc>::emplace(Args &&... args) {
    std::pair<Val, Priority> new_elt =
        std::make_pair<Val, Priority>(std::forward<Args>(args)...);
    return insert(std::move(new_elt.first), std::move(new_elt.second));
  }

  /// indicates whether the priority queue is empty
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE bool MultiPriorityQueue<Val, Priority, Cmp, Alloc>::empty() const noexcept {
    return (__nb_elements == 0);
  }

  /// indicates whether the priority queue contains a given value
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE bool
  MultiPriorityQueue<Val, Priority, Cmp, Alloc>::contains(const Val &val) const {
    return __indices.exists(val);
  }

  /// returns the element at position "index" in the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE const Val &MultiPriorityQueue<Val, Priority, Cmp, Alloc>::
  operator[](Size index) const {
    if (index >= __nb_elements) {
      GUM_ERROR(NotFound, "not enough elements in the MultiPriorityQueue");
    }

    return *(__heap[index].second);
  }

  /// displays the content of the queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  std::string MultiPriorityQueue<Val, Priority, Cmp, Alloc>::toString() const {
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
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  Size MultiPriorityQueue<Val, Priority, Cmp, Alloc>::setPriorityByPos(
      Size index, const Priority &new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >= __nb_elements) {
      GUM_ERROR(NotFound, "not enough elements in the MultiPriorityQueue");
    }

    // get the element itself
    const Val *val = __heap[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i && __cmp(new_priority, __heap[j].first);
         i = j, j = (j - 1) >> 1) {
      __heap[i] = std::move(__heap[j]);
      std::vector<Size> &vect_index = __indices[*(__heap[i].second)];
      for (auto &idx : vect_index) {
        if (idx == j) {
          idx = i;
          break;
        }
      }
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
      std::vector<Size> &vect_index = __indices[*(__heap[i].second)];
      for (auto &idx : vect_index) {
        if (idx == j) {
          idx = i;
          break;
        }
      }
    }

    // update the index of val
    __heap[i].first = new_priority;
    __heap[i].second = val;
    std::vector<Size> &vect_index = __indices[*(__heap[i].second)];
    for (auto &idx : vect_index) {
      if (idx == index) {
        idx = i;
        break;
      }
    }

    return i;
  }

  /// changes the size of the internal structure storing the priority queue
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  Size MultiPriorityQueue<Val, Priority, Cmp, Alloc>::setPriorityByPos(
      Size index, Priority &&new_priority) {
    // check whether the element the priority of which should be changed exists
    if (index >= __nb_elements) {
      GUM_ERROR(NotFound, "not enough elements in the MultiPriorityQueue");
    }

    // get the element itself
    const Val *val = __heap[index].second;

    // restore the heap property
    Size i = index;

    // move val upward if needed
    for (Size j = (i - 1) >> 1; i && __cmp(new_priority, __heap[j].first);
         i = j, j = (j - 1) >> 1) {
      __heap[i] = std::move(__heap[j]);
      std::vector<Size> &vect_index = __indices[*(__heap[i].second)];
      for (auto &idx : vect_index) {
        if (idx == j) {
          idx = i;
          break;
        }
      }
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
      std::vector<Size> &vect_index = __indices[*(__heap[i].second)];
      for (auto &idx : vect_index) {
        if (idx == j) {
          idx = i;
          break;
        }
      }
    }

    // update the index of val
    __heap[i].first = std::move(new_priority);
    __heap[i].second = val;
    std::vector<Size> &vect_index = __indices[*(__heap[i].second)];
    for (auto &idx : vect_index) {
      if (idx == index) {
        idx = i;
        break;
      }
    }

    return i;
  }

  /// modifies the priority of each instance of a given element
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  void MultiPriorityQueue<Val, Priority, Cmp, Alloc>::setPriority(
      const Val &elt, const Priority &new_priority) {
    std::vector<Size> &vect_index = __indices[elt];

    for (auto index : vect_index) {
      setPriorityByPos(index, new_priority);
    }
  }

  /// modifies the priority of each instance of a given element
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE const Priority &
  MultiPriorityQueue<Val, Priority, Cmp, Alloc>::priority(const Val &elt) const {
    return __heap[__indices[elt][0]].first;
  }

  /// A \c << operator for priority queues
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  INLINE std::ostream &
  operator<<(std::ostream &stream,
             const MultiPriorityQueue<Val, Priority, Cmp, Alloc> &queue) {
    stream << queue.toString();
    return stream;
  }

} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
