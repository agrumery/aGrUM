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
 * @brief priority queues in which the same element can appear several times
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 * This file provides class MultiPriorityQueue that is essentially a heap in
 * which elements are sorted according to a dynamically modifiable priority. As in
 * heaps, elements are sorted according to a weak order which is < by default,
 * i.e., the top element of the queue is the smallest element and the more to the
 * bottom the greater the element.
 * @par Usage example:
 * @code
 * // create a priority queue of strings, the priorities of which are integers
 * // the element at the top of the queue has the smallest priority
 * MultiPriorityQueue<std::string> queue1;
 *
 * // insert elements into the queue
 * queue1.insert (8,  "AAA");
 * queue1.insert (10, "BBB");
 * queue1.insert (2,  "CCC");
 * queue1.insert (23, "DDD");
 * queue1.insert (24, "EEE");
 * queue1.insert (10, "AAA");
 *
 * // copy the queue
 * MultiPriorityQueue<std::string> queue2 = queue1;
 *
 * // initializer list constructor
 * MultiPriorityQueue<std::string,int>
 * queue3 { std::pair<std::string,int> ( "aa", 3 ),
 *          std::pair<std::string,int> ( "bb", 2 ) };
 *
 * // create a priority queue of strings, the priorities of which are pairs of int
 * MultiPriorityQueue< std::string, std::pair<int,int> > queue3;
 *
 * // get the top element, then remove it
 * std::cerr << queue2.top() << std::endl;
 * queue2.eraseTop();
 *
 * // get the top element, then remove it
 * std::cerr << queue2.pop() << std::endl;
 *
 * // output the content of the queue
 * std::cerr << queue1 << std::endl;
 *
 * // change the priority of the element at position 3
 * Size new_pos=queue1.setPriorityByPos (3,100);
 *
 * // change the priority of all instances of element "AAA"
 * queue1.setPriority ("AAA",100);
 * @endcode
 */

#ifndef GUM_MULTI_PRIORITY_QUEUE_H
#define GUM_MULTI_PRIORITY_QUEUE_H

#include <functional>
#include <vector>
#include <utility>
#include <initializer_list>

#include <agrum/config.h>

#include <agrum/core/hashTable.h>

namespace gum {

#define GUM_MULTIPLE_PRIORITY_QUEUE_DEFAULT_CAPACITY 10

  // templates provided by this file
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  class MultiPriorityQueue;
  template <typename Val, typename Priority, typename Cmp, typename Alloc>
  std::ostream &operator<<(std::ostream &,
                           const MultiPriorityQueue<Val, Priority, Cmp, Alloc> &);

  /* =========================================================================== */
  /* ===                           PRIORITY QUEUES                           === */
  /* =========================================================================== */
  /** @class MultiPriorityQueue
   * @brief A MultiPriorityQueue is a heap in which each element has a mutable
   * priority and duplicates are allowed
   * @ingroup basicstruct_group
   *
   * A priority queue is quite similar to a heap except that a priority (a score)
   * is assigned to each element in the structure. The elements are sorted
   * according to a weak order on the scores. The priority of any element can be
   * changed at any moment by the user. The priority queue then restores a heap
   * property accordingly.
   * @par Usage example:
   * @code
   * // create a priority queue of strings, the priorities of which are integers
   * // the element at the top of the queue has the smallest priority
   * MultiPriorityQueue<std::string> queue1;
   *
   * // insert elements into the queue
   * queue1.insert (8,  "AAA");
   * queue1.insert (10, "BBB");
   * queue1.insert (2,  "CCC");
   * queue1.insert (23, "DDD");
   * queue1.insert (24, "EEE");
   * queue1.insert (10, "AAA");
   *
   * // copy the queue
   * MultiPriorityQueue<std::string> queue2 = queue1;
   *
   * // initializer list constructor
   * MultiPriorityQueue<std::string,int>
   * queue3 { std::pair<std::string,int> ( "aa", 3 ),
   *          std::pair<std::string,int> ( "bb", 2 ) };
   *
   * // create a priority queue of strings, the priorities of which are
   * // pairs of ints
   * MultiPriorityQueue< std::string, std::pair<int,int> > queue3;
   *
   * // get the top element, then remove it
   * std::cerr << queue2.top() << std::endl;
   * queue2.eraseTop();
   *
   * // get the top element, then remove it
   * std::cerr << queue2.pop() << std::endl;
   *
   * // output the content of the queue
   * std::cerr << queue1 << std::endl;
   *
   * // change the priority of the element at position 3
   * Size new_pos=queue1.setPriorityByPos (3,100);
   *
   * // change the priority of all instances of element "AAA"
   * queue1.setPriority ("AAA",100);
   * @endcode
   */
  /* =========================================================================== */
  template <typename Val, typename Priority = int,
            typename Cmp = std::less<Priority>, typename Alloc = std::allocator<Val>>
  class MultiPriorityQueue {

    template <typename V, typename P, typename C, typename A>
    friend class MultiPriorityQueue;

    public:
    /// types for STL compliance
    /// @{
    using value_type = Val;
    using reference = Val &;
    using const_reference = const Val &;
    using pointer = Val *;
    using const_pointer = const Val *;
    using difference_type = std::ptrdiff_t;
    using allocator_type = Alloc;
    /// @}

    // the allocator for the indices
    using IndexAlloc =
        typename Alloc::template rebind<std::pair<Val, std::vector<Size>>>::other;

    // the allocator for the heap
    using HeapAlloc =
        typename Alloc::template rebind<std::pair<Priority, const Val *>>::other;

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// basic constructor. Creates an empty priority queue
    /** @param compare a function taking two elements in argument, say e1 and e2,
     * and returning a Boolean indicating wether e1 < e2, i.e., whether e1 should
     * be nearer than e2 to the top of the heap.
     * @param capacity the size of the internal data structures containing the
     * elements (could be for instance vectors or hashtables) */
    explicit MultiPriorityQueue(
        Cmp compare = Cmp(),
        Size capacity = GUM_MULTIPLE_PRIORITY_QUEUE_DEFAULT_CAPACITY);

    /// initializer list constructor
    /** The elements of the initializer list are pairs <Val,Priority>.
     * The comparison function is the default one, i.e., std::less<Priority>. */
    explicit MultiPriorityQueue(
        std::initializer_list<std::pair<Val, Priority>> list);

    /// copy constructor
    MultiPriorityQueue(const MultiPriorityQueue<Val, Priority, Cmp, Alloc> &);

    /// generalized copy constructor
    template <typename OtherAlloc>
    MultiPriorityQueue(const MultiPriorityQueue<Val, Priority, Cmp, OtherAlloc> &);

    /// move constructor
    MultiPriorityQueue(MultiPriorityQueue<Val, Priority, Cmp, Alloc> &&);

    /// destructor
    ~MultiPriorityQueue();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** When a problem occurs during the copy (for instance when not enough memory
     * is available), the operator guarantees that the heap stays in a coherent
     * state. Actually, the priority queue becomes empty. An exception is then
     * thrown. */
    MultiPriorityQueue<Val, Priority, Cmp, Alloc> &
    operator=(const MultiPriorityQueue<Val, Priority, Cmp, Alloc> &);

    /// generalized copy operator
    /** When a problem occurs during the copy (for instance when not enough memory
     * is available), the operator guarantees that the heap stays in a coherent
     * state. Actually, the priority queue becomes empty. An exception is then
     * thrown. */
    template <typename OtherAlloc>
    MultiPriorityQueue<Val, Priority, Cmp, Alloc> &
    operator=(const MultiPriorityQueue<Val, Priority, Cmp, OtherAlloc> &);

    /// move operator
    MultiPriorityQueue<Val, Priority, Cmp, Alloc> &
    operator=(MultiPriorityQueue<Val, Priority, Cmp, Alloc> &&);

    /// returns the element at index "index_elt" from the priority queue
    /** @throw NotFound exception is thrown if the element does not exist */
    const Val &operator[](Size index_elt) const;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// returns the number of elements in the priority queue
    Size size() const noexcept;

    /// indicates whether the priority queue is empty
    bool empty() const noexcept;

    /// indicates whether the priority queue contains a given value
    bool contains(const Val &) const;

    /// returns the element at the top of the priority queue
    /** @throw NotFound exception is thrown if the queue is empty */
    const Val &top() const;

    /// returns the priority of the top element
    /** @throw NotFound exception is thrown if the queue is empty */
    const Priority &topPriority() const;

    /// removes the top element from the priority queue and return it
    /** @throw NotFound exception is thrown if the queue is empty */
    Val pop();

    /// inserts a new (a copy) element in the priority queue
    /** @return the index of the element inserted into the priority queue (see
     * method eraseByPos for more details about the index) */
    Size insert(const Val &val, const Priority &priority);

    /// inserts (by move) a new element in the priority queue
    /** @return the index of the element inserted into the priority queue (see
     * method eraseByPos for more details about the index) */
    Size insert(Val &&val, Priority &&priority);

    /// emplace a new element into the priority queue
    /** @return the index of the element inserted into the priority queue (see
     * method eraseByPos for more details about the index)
     * @throw DuplicateElement exception is thrown if the element already exists */
    template <typename... Args> Size emplace(Args &&... args);

    /// removes the top of the priority queue (but does not return it)
    /** If the heap is empty, it does nothing (in particular, it does not throw
     * any exception). */
    void eraseTop();

    /// removes the element at position "index" from the priority queue
    /** If the element cannot be found, the function returns without throwing any
     * exception.
     * @param index represents the position of the element to be removed. This is
     * computed as follows: suppose that the queue is a complete binary tree, that
     * is, a binary tree where all levels are completely filled except, maybe, the
     * last one and, in this case, the elements of this level are all to the left
     * of the tree. Then parsing the tree from top to bottom and, for each level,
     * from left to right, and assigning index 0 to the root of the tree and,
     * incrementing the index by 1 each time we jump to another node, we get a
     * unique index for each element. This is precisely what the index passed in
     * argument of the function represents. */
    void eraseByPos(Size index);

    /// removes a given element from the priority queue (but does not return it)
    /** If the element cannot be found, the function returns without throwing any
     * exception.
     * @param val the element we wish to remove. If the queue contains several
     * times this element, then the one with the smallest index is removed. */
    void erase(const Val &val);

    /// modifies the priority of the element at position "index" of the queue
    /** @throw NotFound If the element cannot be found */
    Size setPriorityByPos(Size index, const Priority &new_priority);

    /// modifies the priority of the element at position "index" of the queue
    /** @throw NotFound If the element cannot be found */
    Size setPriorityByPos(Size index, Priority &&new_priority);

    /// modifies the priority of each instance of a given element
    /** @throw NotFound If the element cannot be found */
    void setPriority(const Val &elt, const Priority &new_priority);

    /// returns the priority of an instance of the value passed in argument
    /** Of course, this method is really meaningful only when there is only one
     * instance of the given element within the PriorityQueue.
     * @throw NotFound If the element cannot be found */
    const Priority &priority(const Val &elt) const;

    /// removes all the elements from the queue
    void clear();

    /// returns a hashtable the keys of which are the values stored in the queue
    /** The keys of the hashtable correspond to the values stored in the
     * priority queue and, for each key, the corresponding value is the list of
     * indices in the queue where we can find the key. */
    const HashTable<Val, std::vector<Size>> &allValues() const;

    /// displays the content of the queue
    std::string toString() const;

    /// @}

    // ############################################################################
    /// @name Fine tuning
    // ############################################################################
    /// @{

    /// return the size of the internal structure storing the priority queue
    Size capacity() const noexcept;

    /// changes the size of the internal structure storing the priority queue
    void resize(Size new_size);

    /// @}

    private:
    /// an array storing all the elements of the heap as well as their score
    std::vector<std::pair<Priority, const Val *>, HeapAlloc> __heap;

    /// a hashtable for quickly finding the elements by their value
    HashTable<Val, std::vector<Size>, IndexAlloc> __indices;

    /// the number of elements in the heap
    Size __nb_elements{0};

    /// comparison function
    Cmp __cmp;
  };

} /* namespace gum */

/// always include the implementation of the templates
#include <agrum/core/multiPriorityQueue.tcc>

#endif /* GUM_MULTI_PRIORITY_QUEUE_H */
