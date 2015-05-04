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
 * @brief template implementations of heaps
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <string>
#include <sstream>

// to ease IDE parser
#include <agrum/core/heap.h>

namespace gum {

  /// basic constructor. Creates an empty heap
  template <typename Val, typename Cmp, typename Alloc>
  Heap<Val, Cmp, Alloc>::Heap(Cmp compare, Size capacity)
      : __cmp(compare) {
    __heap.reserve(capacity);

    // for debugging purposes
    GUM_CONSTRUCTOR(Heap);
  }

  /// initializer list constructor
  template <typename Val, typename Cmp, typename Alloc>
  Heap<Val, Cmp, Alloc>::Heap(std::initializer_list<Val> list) {
    __heap.reserve(list.size());
    for (const auto &elt : list) {
      insert(elt);
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(Heap);
  }

  /// copy constructor
  template <typename Val, typename Cmp, typename Alloc>
  Heap<Val, Cmp, Alloc>::Heap(const Heap<Val, Cmp, Alloc> &from)
      : __heap(from.__heap), __nb_elements(from.__nb_elements), __cmp(from.__cmp) {
    // for debugging purposes
    GUM_CONS_CPY(Heap);
  }

  /// generalized copy constructor
  template <typename Val, typename Cmp, typename Alloc>
  template <typename OtherAlloc>
  Heap<Val, Cmp, Alloc>::Heap(const Heap<Val, Cmp, OtherAlloc> &from)
      : __nb_elements(from.__nb_elements), __cmp(from.__cmp) {
    __heap.reserve(__nb_elements);

    // copy the elements of from.__heap
    for (unsigned i = 0; i < __nb_elements; ++i)
      __heap.push_back(from.__heap[i]);

    // for debugging purposes
    GUM_CONS_CPY(Heap);
  }

  /// move constructor
  template <typename Val, typename Cmp, typename Alloc>
  Heap<Val, Cmp, Alloc>::Heap(Heap<Val, Cmp, Alloc> &&from) noexcept
      : __heap(std::move(from.__heap)),
        __nb_elements(std::move(from.__nb_elements)),
        __cmp(std::move(from.__cmp)) {
    // for debugging purposes
    GUM_CONS_MOV(Heap);
  }

  /// destructor
  template <typename Val, typename Cmp, typename Alloc>
  Heap<Val, Cmp, Alloc>::~Heap() {
    // for debugging purposes
    GUM_DESTRUCTOR(Heap);
  }

  /// copy operator
  template <typename Val, typename Cmp, typename Alloc>
  Heap<Val, Cmp, Alloc> &Heap<Val, Cmp, Alloc>::
  operator=(const Heap<Val, Cmp, Alloc> &from) {
    // avoid self assignment
    if (this != &from) {
      try {
        __heap = from.__heap;
      } catch (...) {
        __heap.clear();
        __nb_elements = 0;

        throw;
      }

      // set the comparison function
      __cmp = from.__cmp;
      __nb_elements = from.__nb_elements;

      // for debugging purposes
      GUM_OP_CPY(Heap);
    }

    return *this;
  }

  /// generalized copy operator
  template <typename Val, typename Cmp, typename Alloc>
  template <typename OtherAlloc>
  Heap<Val, Cmp, Alloc> &Heap<Val, Cmp, Alloc>::
  operator=(const Heap<Val, Cmp, OtherAlloc> &from) {
    // avoid self assignment
    if (this != &from) {
      try {
        __heap.clear();

        if (__heap.capacity() < from.__nb_elements)
          __heap.reserve(from.__nb_elements);

        for (unsigned int i = 0; i < from.__nb_elements; ++i) {
          __heap.push_back(from.__heap[i]);
        }
      } catch (...) {
        __heap.clear();
        __nb_elements = 0;
        throw;
      }

      __cmp = from.__cmp;
      __nb_elements = from.__nb_elements;

      // for debugging purposes
      GUM_OP_CPY(Heap);
    }

    return *this;
  }

  /// move operator
  template <typename Val, typename Cmp, typename Alloc>
  Heap<Val, Cmp, Alloc> &Heap<Val, Cmp, Alloc>::
  operator=(Heap<Val, Cmp, Alloc> &&from) noexcept {
    // avoid self assignment
    if (this != &from) {
      __heap = std::move(from.__heap);
      __nb_elements = std::move(from.__nb_elements);
      __cmp = std::move(from.__cmp);
    }

    return *this;
  }

  /// returns the element at the top of the heap
  template <typename Val, typename Cmp, typename Alloc>
  INLINE const Val &Heap<Val, Cmp, Alloc>::top() const {
    if (!__nb_elements) {
      GUM_ERROR(NotFound, "empty heap");
    }

    return __heap[0];
  }

  /// returns the number of elements in the heap
  template <typename Val, typename Cmp, typename Alloc>
  INLINE Size Heap<Val, Cmp, Alloc>::size() const noexcept {
    return __nb_elements;
  }

  /// return the size of the array storing the heap
  template <typename Val, typename Cmp, typename Alloc>
  INLINE Size Heap<Val, Cmp, Alloc>::capacity() const noexcept {
    return __heap.size();
  }

  /// changes the size of the array storing the heap
  template <typename Val, typename Cmp, typename Alloc>
  INLINE void Heap<Val, Cmp, Alloc>::resize(Size new_size) {
    if (new_size > __nb_elements)
      __heap.reserve(new_size);
  }

  /// removes the element at position 'index' from the heap
  template <typename Val, typename Cmp, typename Alloc>
  void Heap<Val, Cmp, Alloc>::eraseByPos(Size index) {
    if (index >= __nb_elements)
      return;

    // remove the element and put the last element in its place
    Val last = std::move(__heap[__nb_elements - 1]);
    __heap.pop_back();
    --__nb_elements;

    if (!__nb_elements || (index == __nb_elements))
      return;

    // restore the heap property
    Size i = index;

    for (Size j = (index << 1) + 1; j < __nb_elements; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < __nb_elements) && __cmp(__heap[j + 1], __heap[j]))
        ++j;

      // if "last" is smaller than __heap[j], "last" must be stored at index i
      if (__cmp(last, __heap[j]))
        break;

      __heap[i] = std::move(__heap[j]);
    }

    __heap[i] = std::move(last);
  }

  /// removes a given element from the heap (but does not return it)
  template <typename Val, typename Cmp, typename Alloc>
  INLINE void Heap<Val, Cmp, Alloc>::erase(const Val &val) {
    // find val in the heap
    for (Size i = 0; i < __nb_elements; ++i)
      if (__heap[i] == val) {
        eraseByPos(i);
        break;
      }
  }

  /// removes the top of the heap (but does not return it)
  template <typename Val, typename Cmp, typename Alloc>
  INLINE void Heap<Val, Cmp, Alloc>::eraseTop() {
    // if the heap is empty, do nothing
    if (!__nb_elements)
      return;
    eraseByPos(0);
  }

  /// removes the top element from the heap and return it
  template <typename Val, typename Cmp, typename Alloc>
  INLINE Val Heap<Val, Cmp, Alloc>::pop() {
    if (!__nb_elements) {
      GUM_ERROR(NotFound, "empty heap");
    }

    Val v = __heap[0];
    eraseByPos(0);
    return v;
  }

  /// after inserting an element at the end of the heap, restore heap property
  template <typename Val, typename Cmp, typename Alloc>
  Size Heap<Val, Cmp, Alloc>::__restoreHeap() {
    // get the element at the end of the heap
    Size i = __nb_elements - 1;
    Val v = std::move(__heap[i]);

    // restore the heap property
    for (Size j = (i - 1) >> 1; i && __cmp(v, __heap[j]); i = j, j = (j - 1) >> 1)
      __heap[i] = std::move(__heap[j]);

    __heap[i] = std::move(v);

    return i;
  }

  /// inserts a new (a copy) element in the heap
  template <typename Val, typename Cmp, typename Alloc>
  INLINE Size Heap<Val, Cmp, Alloc>::insert(const Val &val) {
    // create a new element at the end of the heap
    __heap.push_back(val);
    ++__nb_elements;
    return __restoreHeap();
  }

  /// inserts a new (a copy) element in the heap
  template <typename Val, typename Cmp, typename Alloc>
  Size Heap<Val, Cmp, Alloc>::insert(Val &&val) {
    // create a new element at the end of the heap
    __heap.push_back(std::move(val));
    ++__nb_elements;
    return __restoreHeap();
  }

  /// emplace a new element in the heap
  template <typename Val, typename Cmp, typename Alloc>
  template <typename... Args>
  Size Heap<Val, Cmp, Alloc>::emplace(Args &&... args) {
    // create a new element at the end of the heap
    __heap.emplace_back(std::forward<Args>(args)...);
    ++__nb_elements;
    return __restoreHeap();
  }

  /// indicates whether the heap is empty
  template <typename Val, typename Cmp, typename Alloc>
  INLINE bool Heap<Val, Cmp, Alloc>::empty() const noexcept {
    return (__nb_elements == 0);
  }

  /// indicates whether the heap contains a given value
  template <typename Val, typename Cmp, typename Alloc>
  INLINE bool Heap<Val, Cmp, Alloc>::contains(const Val &val) const {
    for (Size i = 0; i < __nb_elements; ++i)
      if (__heap[i] == val)
        return true;

    return false;
  }

  /// returns the element at index elt from the heap
  template <typename Val, typename Cmp, typename Alloc>
  INLINE const Val &Heap<Val, Cmp, Alloc>::operator[](Size index) const {
    // check if the element exists
    if (index >= __nb_elements) {
      GUM_ERROR(NotFound, "not enough elements in the heap");
    }

    return __heap[index];
  }

  /// displays the content of the heap
  template <typename Val, typename Cmp, typename Alloc>
  std::string Heap<Val, Cmp, Alloc>::toString() const {
    bool deja = false;
    std::stringstream stream;
    stream << "[";

    for (Size i = 0; i != __nb_elements; ++i, deja = true) {
      if (deja)
        stream << " , ";

      stream << __heap[i];
    }

    stream << "]";

    return stream.str();
  }

  /// A \c << operator for Heap
  template <typename Val, typename Cmp, typename Alloc>
  INLINE std::ostream &operator<<(std::ostream &stream,
                                  const Heap<Val, Cmp, Alloc> &heap) {
    stream << heap.toString();
    return stream;
  }

} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
