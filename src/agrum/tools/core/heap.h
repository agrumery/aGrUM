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
 * @brief Heaps definition.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */

#ifndef GUM_HEAP_H
#define GUM_HEAP_H

#include <functional>
#include <initializer_list>
#include <utility>
#include <vector>

#include <agrum/agrum.h>

namespace gum {

#define GUM_HEAP_DEFAULT_CAPACITY 10

  // templates provided by this file

  template < typename Val, typename Cmp >
  class Heap;
  template < typename Val, typename Cmp >
  std::ostream& operator<<(std::ostream&, const Heap< Val, Cmp >&);

  // ===========================================================================
  // ===                      SIMPLE HEAP DATA STRUCTURE                     ===
  // ===========================================================================

  /**
   * @class Heap
   * @headerfile heap.h <agrum/tools/core/heap.h>
   * @brief Heap data structure
   * @ingroup basicstruct_group
   * @ingroup heap_group
   *
   * This structure is a basic heap data structure, i.e., it is a container in
   * which elements are sorted according to a weak ordering. Given this
   * ordering, we do have access in O(1) to the "smallest" element contained by
   * the structure. Insertions and deletions of elements are processed in O(ln
   * n), where n is the number of elements in the heap. In addition to the
   * classical pop, top and push functions, Heap provide direct accessors to
   * the elements in the heap (see operator[]). However, these can only be
   * accessed in read-only mode.
   * @par Usage example:
   * @code
   * // create a heap of integers, the top element is the smallest element
   * Heap<int> heap1;
   *
   * // create a heap of floats, the top element is the greatest
   * Heap<float> heap2 (std::greater<float>());
   *
   * // insert elements into the heap
   * heap1.insert (8);
   * heap1.insert (10);
   * heap1.insert (2);
   * heap1.insert (23);
   * heap1.insert (24);
   * heap1.insert (10);
   * heap1.insert (10);
   *
   * // copy a heap into another
   * Heap<int> heap2 = heap1;
   *
   * // get the number of elements of the heap
   * std::cerr << heap2.size() << std::endl;
   *
   * // get the top element but do not remove it
   * std::cerr << heap2.top() << std::endl;
   *
   * // get and remove the top element of the heap
   * std::cerr << heap2.pop() << std::endl;
   *
   * // remove the top element but do not return it (faster than above)
   * heap2.eraseTop();
   *
   * // erase in heap1 value 8
   * heap1.eraseByVal (8);
   *
   * // erase element at position 3 (see function erase for the
   * // meaning of position 3)
   * heap1.erase (3);
   *
   * // get the element at position 3
   * heap1[3];
   *
   * // check whether element 24 belongs to the heap
   * heap1.contains (24);
   * @endcode
   *
   * @tparam Val The gum::Heap values type.
   * @tparam Cmp The comperator used to sort elements in the gum::Heap.
   */
  template < typename Val, typename Cmp = std::less< Val > >
  class Heap {
    public:
    /// Types for STL compliance
    /// @{
    using value_type      = Val;
    using reference       = Val&;
    using const_reference = const Val&;
    using pointer         = Val*;
    using const_pointer   = const Val*;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    /// @}

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Basic constructor: creates an empty heap.
     * @param compare a function taking two elements in argument, say e1 and
     * e2, and returning a Boolean indicating wether e1 < e2, i.e., whether e1
     * should be nearer than e2 to the top of the heap.
     * @param capacity the size of the internal data structures containing the
     * elements (could be for instance vectors or hashtables).
     */
    explicit Heap(Cmp compare = Cmp(), Size capacity = GUM_HEAP_DEFAULT_CAPACITY);

    /**
     * @brief Initializer list constructor.
     * @param list The initializer list.
     */
    explicit Heap(std::initializer_list< Val > list);

    /**
     * @brief Copy constructor.
     * @param from The gum::Heap to copy.
     */
    Heap(const Heap< Val, Cmp >& from);

    /**
     * @brief Move constructor.
     * @param from The gum::Heap to move.
     */
    Heap(Heap< Val, Cmp >&& from) noexcept;

    /**
     * @brief Class destructor.
     */
    ~Heap();

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     *
     * When a problem occurs during the copy (for instance when not enough
     * memory is available), the operator guarantees that the heap stays in a
     * coherent state. Actually, the heap becomes empty. An exception is then
     * thrown.
     *
     * @param from The gum::Heap to copy.
     */
    Heap< Val, Cmp >& operator=(const Heap< Val, Cmp >& from);

    /**
     * @brief Move operator.
     *
     * When a problem occurs during the copy (for instance when not enough
     * memory is available), the operator guarantees that the heap stays in a
     * coherent state. Actually, the heap becomes empty. An exception is then
     * thrown.
     *
     * @param from The gum::Heap to move.
     */
    Heap< Val, Cmp >& operator=(Heap< Val, Cmp >&& from) noexcept;

    /**
     * @brief Returns the element at index index_elt from the heap.
     *
     * @param index_elt The index of the element to return.
     * @return Returns the element at index index_elt from the heap.
     * @throw NotFound exception is thrown if there is no element
     * at position "index_elt".
     */
    const Val& operator[](Size index_elt) const;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Returns the element at the top of the heap.
     * @return Returns the element at the top of the heap.
     * @throw NotFound exception is thrown if the heap is empty.
     */
    const Val& top() const;

    /**
     * @brief Removes the top element from the heap and return it.
     * @return Returns the top element from the heap.
     * @throw NotFound exception is thrown if the heap is empty.
     */
    Val pop();

    /**
     * @brief Removes the top of the heap (but does not return it).
     *
     * If the heap is empty, it does nothing (in particular, it does not throw
     * any exception).
     */
    void eraseTop();

    /**
     * @brief Removes the element positioned at "index" from the heap.
     *
     * If the element cannot be found, the function returns without throwing
     * any exception.  @param index represents the position of the element to
     * be removed. This is computed as follows: suppose that the heap is a
     * complete binary tree, that is, a binary tree where all levels are
     * completely filled except, maybe, the last one and, in this case, the
     * elements of this level are all to the left of the tree. Then parsing the
     * tree from top to bottom and, for each level, from left to right, and
     * assigning index 0 to the root of the tree and, incrementing the index by
     * 1 each time we jump to another node, we get a unique index for each
     * element. This is precisely what the index passed in argument of the
     * function represents.
     *
     * @param index The index of the element to remove
     */
    void eraseByPos(Size index);

    /**
     * @brief Removes a given element from the heap (but does not return it).
     *
     * If the element cannot be found, the function returns without throwing
     * any exception.
     *
     * @param val The element we wish to remove. If the heap contains several
     * times this element, then the one with the smallest index is removed.
     */
    void erase(const Val& val);

    /**
     * @brief inserts a new element (actually a copy) in the heap and returns
     * its index
     *
     * @param val The element to insert.
     * @return The inserted element position in the gum::Heap.
     */
    Size insert(const Val& val);

    /**
     * @brief Inserts a new element (by moving it) in the heap and returns its
     * index.
     *
     * @param val The element to insert.
     * @return The inserted element position in the gum::Heap.
     */
    Size insert(Val&& val);

    /**
     * @brief Emplace a new element in the heap and returns its index.
     *
     * @tparam Args the type of the new element to emplace.
     * @param args The new element to emplace.
     * @return The emplaced element position in the gum::Heap.
     */
    template < typename... Args >
    Size emplace(Args&&... args);

    /**
     * @brief Returns the number of elements in the heap.
     * @return Returns the number of elements in the heap.
     */
    Size size() const noexcept;

    /**
     * @brief Indicates whether the heap is empty.
     * @return Indicates whether the heap is empty.
     */
    bool empty() const noexcept;

    /**
     * @brief Indicates whether the heap contains a given value.
     * @return Indicates whether the heap contains a given value.
     */
    bool contains(const Val&) const;

    /**
     * @return Displays the content of the heap.
     * @return Returns a std::string representing the content of the heap.
     */
    std::string toString() const;

    /// @}
    // ============================================================================
    /// @name Fine tuning
    // ============================================================================
    /// @{

    /**
     * @brief Returns the size of the internal structure storing the heap.
     * @return Returns the size of the internal structure storing the heap.
     */
    Size capacity() const noexcept;

    /**
     * @brief Changes the size of the the internal structure storing the heap.
     *
     * If the new size does not enable the heap to contain the elements it
     * currently contains, then the resizing does not occur.
     *
     * @param new_size The gum::Heap new size.
     */
    void resize(Size new_size);

    /// @}

    private:
    /// An array storing all the elements of the heap.
    std::vector< Val > _heap_;

    /// The number of elements in the heap.
    Size _nb_elements_{0};

    /// Comparison function.
    Cmp _cmp_;

    /// After inserting an element at the end of the heap, restore heap property
    Size _restoreHeap_();
  };

} /* namespace gum */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::Heap< int >;
extern template class gum::Heap< long >;
extern template class gum::Heap< double >;
#endif


// always include the implementation of the templates
#include <agrum/tools/core/heap_tpl.h>

#endif /* GUM_HEAP_H */
