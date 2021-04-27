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
 * @brief template implementations of heaps
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <sstream>
#include <string>

// to ease IDE parser
#include <agrum/tools/core/heap.h>

namespace gum {

  // basic constructor. Creates an empty heap
  template < typename Val, typename Cmp, typename Alloc >
  Heap< Val, Cmp, Alloc >::Heap(Cmp compare, Size capacity) : cmp__(compare) {
    heap__.reserve(capacity);

    // for debugging purposes
    GUM_CONSTRUCTOR(Heap);
  }

  // initializer list constructor
  template < typename Val, typename Cmp, typename Alloc >
  Heap< Val, Cmp, Alloc >::Heap(std::initializer_list< Val > list) {
    heap__.reserve(list.size());
    for (const auto& elt: list) {
      insert(elt);
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(Heap);
  }

  // copy constructor
  template < typename Val, typename Cmp, typename Alloc >
  Heap< Val, Cmp, Alloc >::Heap(const Heap< Val, Cmp, Alloc >& from) :
      heap__(from.heap__), nb_elements__(from.nb_elements__), cmp__(from.cmp__) {
    // for debugging purposes
    GUM_CONS_CPY(Heap);
  }

  // generalized copy constructor
  template < typename Val, typename Cmp, typename Alloc >
  template < typename OtherAlloc >
  Heap< Val, Cmp, Alloc >::Heap(const Heap< Val, Cmp, OtherAlloc >& from) :
      nb_elements__(from.nb_elements__), cmp__(from.cmp__) {
    heap__.reserve(nb_elements__);

    // copy the elements of from.heap__
    for (unsigned i = 0; i < nb_elements__; ++i)
      heap__.push_back(from.heap__[i]);

    // for debugging purposes
    GUM_CONS_CPY(Heap);
  }

  // move constructor
  template < typename Val, typename Cmp, typename Alloc >
  Heap< Val, Cmp, Alloc >::Heap(Heap< Val, Cmp, Alloc >&& from) noexcept :
      heap__(std::move(from.heap__)), nb_elements__(std::move(from.nb_elements__)),
      cmp__(std::move(from.cmp__)) {
    // for debugging purposes
    GUM_CONS_MOV(Heap);
  }

  // destructor
  template < typename Val, typename Cmp, typename Alloc >
  Heap< Val, Cmp, Alloc >::~Heap() {
    // for debugging purposes
    GUM_DESTRUCTOR(Heap);
  }

  // copy operator
  template < typename Val, typename Cmp, typename Alloc >
  Heap< Val, Cmp, Alloc >&
     Heap< Val, Cmp, Alloc >::operator=(const Heap< Val, Cmp, Alloc >& from) {
    // avoid self assignment
    if (this != &from) {
      try {
        heap__ = from.heap__;
      } catch (...) {
        heap__.clear();
        nb_elements__ = 0;

        throw;
      }

      // set the comparison function
      cmp__         = from.cmp__;
      nb_elements__ = from.nb_elements__;

      // for debugging purposes
      GUM_OP_CPY(Heap);
    }

    return *this;
  }

  // generalized copy operator
  template < typename Val, typename Cmp, typename Alloc >
  template < typename OtherAlloc >
  Heap< Val, Cmp, Alloc >&
     Heap< Val, Cmp, Alloc >::operator=(const Heap< Val, Cmp, OtherAlloc >& from) {
    // avoid self assignment
    if (this != &from) {
      try {
        heap__.clear();

        if (heap__.capacity() < from.nb_elements__)
          heap__.reserve(from.nb_elements__);

        for (unsigned int i = 0; i < from.nb_elements__; ++i) {
          heap__.push_back(from.heap__[i]);
        }
      } catch (...) {
        heap__.clear();
        nb_elements__ = 0;
        throw;
      }

      cmp__         = from.cmp__;
      nb_elements__ = from.nb_elements__;

      // for debugging purposes
      GUM_OP_CPY(Heap);
    }

    return *this;
  }

  // move operator
  template < typename Val, typename Cmp, typename Alloc >
  Heap< Val, Cmp, Alloc >&
     Heap< Val, Cmp, Alloc >::operator=(Heap< Val, Cmp, Alloc >&& from) noexcept {
    // avoid self assignment
    if (this != &from) {
      heap__        = std::move(from.heap__);
      nb_elements__ = std::move(from.nb_elements__);
      cmp__         = std::move(from.cmp__);
    }

    return *this;
  }

  // returns the element at the top of the heap
  template < typename Val, typename Cmp, typename Alloc >
  INLINE const Val& Heap< Val, Cmp, Alloc >::top() const {
    if (!nb_elements__) { GUM_ERROR(NotFound, "empty heap"); }

    return heap__[0];
  }

  // returns the number of elements in the heap
  template < typename Val, typename Cmp, typename Alloc >
  INLINE Size Heap< Val, Cmp, Alloc >::size() const noexcept {
    return nb_elements__;
  }

  // return the size of the array storing the heap
  template < typename Val, typename Cmp, typename Alloc >
  INLINE Size Heap< Val, Cmp, Alloc >::capacity() const noexcept {
    return Size(heap__.size());
  }

  // changes the size of the array storing the heap
  template < typename Val, typename Cmp, typename Alloc >
  INLINE void Heap< Val, Cmp, Alloc >::resize(Size new_size) {
    if (new_size > nb_elements__) heap__.reserve(new_size);
  }

  // removes the element at position 'index' from the heap
  template < typename Val, typename Cmp, typename Alloc >
  void Heap< Val, Cmp, Alloc >::eraseByPos(Size index) {
    if (index >= nb_elements__) return;

    // remove the element and put the last element in its place
    Val last = std::move(heap__[nb_elements__ - 1]);
    heap__.pop_back();
    --nb_elements__;

    if (!nb_elements__ || (index == nb_elements__)) return;

    // restore the heap property
    Size i = index;

    for (Size j = (index << 1) + 1; j < nb_elements__; i = j, j = (j << 1) + 1) {
      // let j be the max child
      if ((j + 1 < nb_elements__) && cmp__(heap__[j + 1], heap__[j])) ++j;

      // if "last" is smaller than heap__[j], "last" must be stored at index i
      if (cmp__(last, heap__[j])) break;

      heap__[i] = std::move(heap__[j]);
    }

    heap__[i] = std::move(last);
  }

  // removes a given element from the heap (but does not return it)
  template < typename Val, typename Cmp, typename Alloc >
  INLINE void Heap< Val, Cmp, Alloc >::erase(const Val& val) {
    // find val in the heap
    for (Size i = 0; i < nb_elements__; ++i)
      if (heap__[i] == val) {
        eraseByPos(i);
        break;
      }
  }

  // removes the top of the heap (but does not return it)
  template < typename Val, typename Cmp, typename Alloc >
  INLINE void Heap< Val, Cmp, Alloc >::eraseTop() {
    // if the heap is empty, do nothing
    if (!nb_elements__) return;
    eraseByPos(0);
  }

  // removes the top element from the heap and return it
  template < typename Val, typename Cmp, typename Alloc >
  INLINE Val Heap< Val, Cmp, Alloc >::pop() {
    if (!nb_elements__) { GUM_ERROR(NotFound, "empty heap"); }

    Val v = heap__[0];
    eraseByPos(0);
    return v;
  }

  // after inserting an element at the end of the heap, restore heap property
  template < typename Val, typename Cmp, typename Alloc >
  Size Heap< Val, Cmp, Alloc >::restoreHeap__() {
    // get the element at the end of the heap
    Size i = nb_elements__ - 1;
    Val  v = std::move(heap__[i]);

    // restore the heap property
    for (Size j = (i - 1) >> 1; i && cmp__(v, heap__[j]); i = j, j = (j - 1) >> 1)
      heap__[i] = std::move(heap__[j]);

    heap__[i] = std::move(v);

    return i;
  }

  // inserts a new (a copy) element in the heap
  template < typename Val, typename Cmp, typename Alloc >
  INLINE Size Heap< Val, Cmp, Alloc >::insert(const Val& val) {
    // create a new element at the end of the heap
    heap__.push_back(val);
    ++nb_elements__;
    return restoreHeap__();
  }

  // inserts a new (a copy) element in the heap
  template < typename Val, typename Cmp, typename Alloc >
  Size Heap< Val, Cmp, Alloc >::insert(Val&& val) {
    // create a new element at the end of the heap
    heap__.push_back(std::move(val));
    ++nb_elements__;
    return restoreHeap__();
  }

  // emplace a new element in the heap
  template < typename Val, typename Cmp, typename Alloc >
  template < typename... Args >
  Size Heap< Val, Cmp, Alloc >::emplace(Args&&... args) {
    // create a new element at the end of the heap
    heap__.emplace_back(std::forward< Args >(args)...);
    ++nb_elements__;
    return restoreHeap__();
  }

  // indicates whether the heap is empty
  template < typename Val, typename Cmp, typename Alloc >
  INLINE bool Heap< Val, Cmp, Alloc >::empty() const noexcept {
    return (nb_elements__ == 0);
  }

  // indicates whether the heap contains a given value
  template < typename Val, typename Cmp, typename Alloc >
  INLINE bool Heap< Val, Cmp, Alloc >::contains(const Val& val) const {
    for (Size i = 0; i < nb_elements__; ++i)
      if (heap__[i] == val) return true;

    return false;
  }

  // returns the element at index elt from the heap
  template < typename Val, typename Cmp, typename Alloc >
  INLINE const Val& Heap< Val, Cmp, Alloc >::operator[](Size index) const {
    // check if the element exists
    if (index >= nb_elements__) {
      GUM_ERROR(NotFound, "not enough elements in the heap");
    }

    return heap__[index];
  }

  // displays the content of the heap
  template < typename Val, typename Cmp, typename Alloc >
  std::string Heap< Val, Cmp, Alloc >::toString() const {
    bool              deja = false;
    std::stringstream stream;
    stream << "[";

    for (Size i = 0; i != nb_elements__; ++i, deja = true) {
      if (deja) stream << " , ";

      stream << heap__[i];
    }

    stream << "]";

    return stream.str();
  }

  // A \c << operator for Heap
  template < typename Val, typename Cmp, typename Alloc >
  INLINE std::ostream& operator<<(std::ostream&                  stream,
                                  const Heap< Val, Cmp, Alloc >& heap) {
    stream << heap.toString();
    return stream;
  }

} /* namespace gum */
