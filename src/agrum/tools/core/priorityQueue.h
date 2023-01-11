/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief priority queues (in which an element cannot appear more than once)
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */

#ifndef GUM_PRIORITY_QUEUE_H
#define GUM_PRIORITY_QUEUE_H

#include <functional>
#include <initializer_list>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <agrum/agrum.h>
#include <agrum/tools/core/hashTable.h>

namespace gum {

#define GUM_PRIORITY_QUEUE_DEFAULT_CAPACITY 10

  // templates provided by this file
  template < typename Val, typename Priority, typename Cmp >
  class PriorityQueue;
  template < typename Val, typename Priority, typename Cmp >
  std::ostream& operator<<(std::ostream&, const PriorityQueue< Val, Priority, Cmp >&);

  // ===========================================================================
  // ===              GENERAL IMPLEMENTATION OF PRIORITY QUEUES              ===
  // ===========================================================================

  /**
   * @class PriorityQueueImplementation
   * @headerfile priorityQueue.h <agrum/tools/core/priorityQueue.h>
   * @ingroup priorityqueue_group
   * @brief The internal class for representing priority queues.
   *
   * Priority Queues have different implementations depending on the nature of
   * the values they store. Basically, scalar values can lead to optimization
   * of the code whereas general types like classes cannot. The current class
   * is used for general types (and therefore for classes). The user shall not
   * use directly the implementation but rather use the PriorityQueue class.
   * The latter will be assigned the best implementation at compile time.
   *
   * @tparam Val The values type.
   * @tparam Priority The priorities type.
   * @tparam Cmp The priorities comparator.
   * @tparam Gen Used for metaprogramation, for scalar and non-scalar priority
   * queues.
   */
  template < typename Val, typename Priority, typename Cmp, bool Gen >
  class PriorityQueueImplementation {
    /// All gum::PriorityQueue are friends with themselves.
    friend class PriorityQueue< Val, Priority, Cmp >;

    public:
    /// Types for STL compliance.
    /// @{
    using value_type      = Val;
    using reference       = Val&;
    using const_reference = const Val&;
    using pointer         = Val*;
    using const_pointer   = const Val*;
    using difference_type = std::ptrdiff_t;
    /// @}

    private:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Basic constructor. Creates an empty priority queue.
     *
     * @param compare A function taking two elements in argument, say e1 and
     * e2, and returning a Boolean indicating wether e1 < e2, i.e., whether e1
     * should be nearer than e2 to the top of the heap.
     * @param capacity The size of the internal data structures containing the
     * elements (could be for instance vectors or hashtables)
     */
    explicit PriorityQueueImplementation(Cmp compare, Size capacity);

    /**
     * @brief Initializer list constructor.
     *
     * The elements of the initializer list are pairs <Val,Priority>. The
     * comparison function is the default one, i.e., std::less<Priority>.
     *
     * @param list The initializer list.
     */
    explicit PriorityQueueImplementation(std::initializer_list< std::pair< Val, Priority > > list);

    /**
     * @brief Copy constructor.
     * @param from The gum::PriorityQueueImplementation to copy.
     */
    PriorityQueueImplementation(const PriorityQueueImplementation< Val, Priority, Cmp, Gen >& from);

    /**
     * @brief Move constructor.
     * @param from The gum::PriorityQueueImplementation to move.
     */
    PriorityQueueImplementation(PriorityQueueImplementation< Val, Priority, Cmp, Gen >&& from);

    /**
     * @brief Class destructor.
     */
    ~PriorityQueueImplementation();

    /// @}

    public:
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     *
     * When a problem occurs during the copy (for instance when not enough
     * memory is available), the operator guarantees that the heap stays in a
     * coherent state. Actually, the priority queue becomes empty. An exception
     * is then thrown.
     *
     * @param from The gum::PriorityQueueImplementation to copy.
     * @return Returns this gum::PriorityQueueImplementation.
     */
    PriorityQueueImplementation< Val, Priority, Cmp, Gen >&
       operator=(const PriorityQueueImplementation< Val, Priority, Cmp, Gen >& from);

    /**
     * @brief Move operator.
     *
     * @param from The gum::PriorityQueueImplementation to move.
     * @return Returns this gum::PriorityQueueImplementation.
     */
    PriorityQueueImplementation< Val, Priority, Cmp, Gen >&
       operator=(PriorityQueueImplementation< Val, Priority, Cmp, Gen >&& from);

    /**
     * @brief Returns the element at index "index_elt" from the priority queue.
     * @param index_elt The index of the element to return.
     * @return Returns the element at index "index_elt" from the priority
     * queue.
     * @throw NotFound Raised if the element does not exist.
     */
    const Val& operator[](Size index_elt) const;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Returns the number of elements in the priority queue.
     * @return Returns the number of elements in the priority queue.
     */
    Size size() const noexcept;

    /**
     * @brief Indicates whether the priority queue is empty.
     * @return Returns true if the priority queue is empty.
     */
    bool empty() const noexcept;

    /**
     * @brief Indicates whether the priority queue contains a given value.
     * @param val The value to look for.
     * @return Returns true if val is in the priority queue.
     */
    bool contains(const Val& val) const;

    /// returns the element at the top of the priority queue
    /** @throw NotFound Raised if the queue is empty */
    const Val& top() const;

    /**
     * @brief Returns the priority of the top element.
     * @return Returns the priority of the top element.
     * @throw NotFound Raised if the queue is empty.
     */
    const Priority& topPriority() const;

    /**
     * @brief Removes the top element from the priority queue and return it.
     * @return Returns the top element from the priority queue.
     * @throw NotFound Raised if the queue is empty.
     */
    Val pop();

    /**
     * @brief Inserts a new (a copy) element in the priority queue.
     *
     * See gum::PriorityQueueImplementation::eraseByPos(Size) for more details
     * about the index.
     *
     * @param val The value to insert.
     * @param priority The value's priority in the queue.
     * @return Returns the index of the element inserted into the priority
     * queue.
     * @throw DuplicateElement Raised if the element already exists.
     */
    Size insert(const Val& val, const Priority& priority);

    /**
     * @brief Inserts (by move) a new element in the priority queue.
     *
     * See gum::PriorityQueueImplementation::eraseByPos(Size) for more details
     * about the index.
     *
     * @param val The value to insert.
     * @param priority The value's priority in the queue.
     * @return Returns the index of the element inserted into the priority
     * queue.
     * @throw DuplicateElement Raised if the element already exists.
     */
    Size insert(Val&& val, Priority&& priority);

    /**
     * @brief Emplace a new element into the priority queue.
     *
     * See gum::PriorityQueueImplementation::eraseByPos(Size) for more details
     * about the index.
     *
     * @tparam Args The emplace arguments types.
     * @param args The emplace arguments.
     * @return Returns the index of the element inserted into the priority
     * queue.
     * @throw DuplicateElement Raised if the element already exists.
     */
    template < typename... Args >
    Size emplace(Args&&... args);

    /**
     * @brief Removes the top of the priority queue (but does not return it).
     *
     * If the heap is empty, it does nothing (in particular, it does not throw
     * any exception).
     */
    void eraseTop();

    /**
     * @brief Removes the element at position "index" from the priority queue.
     *
     * If the element cannot be found, the function returns without throwing
     * any exception.
     *
     * The priority is computed as follows: suppose that the queue is a
     * complete binary tree where all levels are completely filled except,
     * eventually, the last one. In this case, the elements of the last
     * level are all on the left of the tree.
     *
     * We assign 0 to the root, then parsing the tree from top to bottom then
     * from left to right we increment the index and assigned it to the current
     * node. Doing so, we get a unique index for each element. This is
     * precisely what the index passed in argument of the function represents.
     *
     * @param index represents the position of the element to be removed.
     */
    void eraseByPos(Size index);

    /**
     * @brief Removes a given element from the priority queue (but does not
     * return it).
     *
     * If the element cannot be found, the function returns without throwing
     * any exception.
     *
     * If the queue contains several times this element, then the one with the
     * smallest index is removed.
     *
     * @param val the element we wish to remove.
     */
    void erase(const Val& val);

    /**
     * @brief Modifies the priority of the element at position "index" of the
     * queue.
     *
     * @param index The index of the element to update.
     * @param new_priority The element's new priority.
     * @return Returns the elements new priority.
     * @throw NotFound Raised if the element cannot be found.
     */
    Size setPriorityByPos(Size index, const Priority& new_priority);

    /**
     * @brief Modifies the priority of the element at position "index" of the
     * queue.
     *
     * @param index The index of the element to update.
     * @param new_priority The element's new priority.
     * @return Returns the elements new priority.
     * @throw NotFound Raised if the element cannot be found.
     */
    Size setPriorityByPos(Size index, Priority&& new_priority);

    /**
     * @brief Modifies the priority of each instance of a given element.
     * @param elt The value to update.
     * @param new_priority The values new priority.
     * @throw NotFound Raised if the element cannot be found.
     */
    void setPriority(const Val& elt, const Priority& new_priority);

    /**
     * @brief Modifies the priority of each instance of a given element.
     * @param elt The value to update.
     * @param new_priority The values new priority.
     * @throw NotFound Raised if the element cannot be found.
     */
    void setPriority(const Val& elt, Priority&& new_priority);

    /**
     * @brief Returns the priority of an instance of the value passed in
     * argument.
     *
     * @param elt The element for which the priority is returned.
     * @return Returns the priority of an instance of the value passed in
     * argument.
     * @throw NotFound Raised if the element cannot be found.
     */
    const Priority& priority(const Val& elt) const;

    /**
     * @brief Returns the priority of the value passed in argument.
     * @param index The index of the value of which the priority is returned.
     * @throw NotFound Raised if the element cannot be found.
     */
    const Priority& priorityByPos(Size index) const;

    /**
     * @brief Removes all the elements from the queue.
     */
    void clear();

    /**
     * @brief Returns a hashtable the keys of which are the values stored in
     * the queue.
     *
     * The keys of the hashtable correspond to the values stored in the
     * priority queue and, for each key, the corresponding value is the index
     * in the queue where we can find the key.
     *
     * @return Returns a hashtable the keys of which are the values stored in
     * the queue.
     */
    const HashTable< Val, Size >& allValues() const noexcept;

    /**
     * @brief Displays the content of the queue.
     * @return Returns the content of the queue.
     */
    std::string toString() const;

    /// @}
    // ============================================================================
    /// @name Fine tuning
    // ============================================================================
    /// @{

    /**
     * @brief Returns the size of the internal structure storing the priority
     * queue.
     * @return Returns the size of the internal structure storing the priority
     * queue.
     */
    Size capacity() const noexcept;

    /**
     * @brief Changes the size of the internal structure storing the priority
     * queue.
     * @param new_size The internal structure new size.
     */
    void resize(Size new_size);

    /// @}

    private:
    /// An array storing all the elements of the heap as well as their score.
    std::vector< std::pair< Priority, const Val* > > _heap_;

    /// A hashtable for quickly finding the elements by their value.
    HashTable< Val, Size > _indices_{HashTableConst::default_size, true, true};

    /// The number of elements in the heap.
    Size _nb_elements_{0};

    /// Comparison function.
    Cmp _cmp_;
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  // ===========================================================================
  // ===              SCALAR IMPLEMENTATION OF PRIORITY QUEUES               ===
  // ===========================================================================

  /**
   * @class PriorityQueueImplementation
   * @headerfile priorityQueue.h <agrum/tools/core/priorityQueue.h>
   * @ingroup priorityqueue_group
   * @brief The internal class for representing priority queues for scalar
   * Vals.
   *
   * Priority Queues have different implementations depending on the nature of
   * the values they store. Basically, scalar values can lead to optimization
   * of the code whereas general types like classes cannot. The current class
   * is used for general types (and therefore for classes). The user shall not
   * use directly the implementation but rather use the PriorityQueue class.
   * The latter will be assigned the best implementation at compile time.
   *
   * @tparam Val The values type.
   * @tparam Priority The priorities type.
   * @tparam Cmp The priorities comparator.
   * @tparam Gen Used for metaprogramation.
   */
  template < typename Val, typename Priority, typename Cmp >
  class PriorityQueueImplementation< Val, Priority, Cmp, true > {
    /// All gum::PriorityQueue are friends with themselves.
    friend class PriorityQueue< Val, Priority, Cmp >;

    public:
    /// Types for STL compliance.
    /// @{
    using value_type      = Val;
    using reference       = Val&;
    using const_reference = const Val&;
    using pointer         = Val*;
    using const_pointer   = const Val*;
    using difference_type = std::ptrdiff_t;
    /// @}

    private:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Basic constructor. Creates an empty priority queue.
     *
     * @param compare A function taking two elements in argument, say e1 and
     * e2, and returning a Boolean indicating wether e1 < e2, i.e., whether e1
     * should be nearer than e2 to the top of the heap.
     * @param capacity The size of the internal data structures containing the
     * elements (could be for instance vectors or hashtables)
     */
    explicit PriorityQueueImplementation(Cmp compare, Size capacity);

    /**
     * @brief Initializer list constructor.
     *
     * The elements of the initializer list are pairs <Val,Priority>. The
     * comparison function is the default one, i.e., std::less<Priority>.
     *
     * @param list The initializer list.
     */
    explicit PriorityQueueImplementation(std::initializer_list< std::pair< Val, Priority > > list);

    /**
     * @brief Copy constructor.
     * @param from The gum::PriorityQueueImplementation to copy.
     */
    PriorityQueueImplementation(
       const PriorityQueueImplementation< Val, Priority, Cmp, true >& from);

    /**
     * @brief Move constructor.
     * @param from The gum::PriorityQueueImplementation to move.
     */
    PriorityQueueImplementation(PriorityQueueImplementation< Val, Priority, Cmp, true >&& from);

    /**
     * @brief Class destructor.
     */
    ~PriorityQueueImplementation();

    /// @}

    public:
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     *
     * When a problem occurs during the copy (for instance when not enough
     * memory is available), the operator guarantees that the heap stays in a
     * coherent state. Actually, the priority queue becomes empty. An exception
     * is then thrown.
     *
     * @param from The gum::PriorityQueueImplementation to copy.
     * @return Returns this gum::PriorityQueueImplementation.
     */
    PriorityQueueImplementation< Val, Priority, Cmp, true >&
       operator=(const PriorityQueueImplementation< Val, Priority, Cmp, true >& from);

    /**
     * @brief Move operator.
     *
     * @param from The gum::PriorityQueueImplementation to move.
     * @return Returns this gum::PriorityQueueImplementation.
     */
    PriorityQueueImplementation< Val, Priority, Cmp, true >&
       operator=(PriorityQueueImplementation< Val, Priority, Cmp, true >&& from);

    /**
     * @brief Returns the element at index "index_elt" from the priority queue.
     * @param index_elt The index of the element to return.
     * @return Returns the element at index "index_elt" from the priority
     * queue.
     * @throw NotFound Raised if the element does not exist.
     */
    const Val& operator[](Size index_elt) const;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Returns the number of elements in the priority queue.
     * @return Returns the number of elements in the priority queue.
     */
    Size size() const noexcept;

    /**
     * @brief Indicates whether the priority queue is empty.
     * @return Returns true if the priority queue is empty.
     */
    bool empty() const noexcept;

    /**
     * @brief Indicates whether the priority queue contains a given value.
     * @param val The value to look for.
     * @return Returns true if val is in the priority queue.
     */
    bool contains(Val val) const;

    /// returns the element at the top of the priority queue
    /** @throw NotFound Raised if the queue is empty */
    const Val& top() const;

    /**
     * @brief Returns the priority of the top element.
     * @return Returns the priority of the top element.
     * @throw NotFound Raised if the queue is empty.
     */
    const Priority& topPriority() const;

    /**
     * @brief Removes the top element from the priority queue and return it.
     * @return Returns the top element from the priority queue.
     * @throw NotFound Raised if the queue is empty.
     */
    Val pop();

    /**
     * @brief Inserts a new (a copy) element in the priority queue.
     *
     * See gum::PriorityQueueImplementation::eraseByPos(Size) for more details
     * about the index.
     *
     * @param val The value to insert.
     * @param priority The value's priority in the queue.
     * @return Returns the index of the element inserted into the priority
     * queue.
     * @throw DuplicateElement Raised if the element already exists.
     */
    Size insert(Val val, const Priority& priority);

    /**
     * @brief Inserts (by move) a new element in the priority queue.
     *
     * See gum::PriorityQueueImplementation::eraseByPos(Size) for more details
     * about the index.
     *
     * @param val The value to insert.
     * @param priority The value's priority in the queue.
     * @return Returns the index of the element inserted into the priority
     * queue.
     * @throw DuplicateElement Raised if the element already exists.
     */
    Size insert(Val val, Priority&& priority);

    /**
     * @brief Emplace a new element into the priority queue.
     *
     * See gum::PriorityQueueImplementation::eraseByPos(Size) for more details
     * about the index.
     *
     * @tparam Args The emplace arguments types.
     * @param args The emplace arguments.
     * @return Returns the index of the element inserted into the priority
     * queue.
     * @throw DuplicateElement Raised if the element already exists.
     */
    template < typename... Args >
    Size emplace(Args&&... args);

    /**
     * @brief Removes the top of the priority queue (but does not return it).
     *
     * If the heap is empty, it does nothing (in particular, it does not throw
     * any exception).
     */
    void eraseTop();

    /**
     * @brief Removes the element at position "index" from the priority queue.
     *
     * If the element cannot be found, the function returns without throwing
     * any exception.
     *
     * The priority is computed as follows: suppose that the queue is a
     * complete binary tree where all levels are completely filled except,
     * eventually, the last one. In this case, the elements of the last
     * level are all on the left of the tree.
     *
     * We assign 0 to the root, then parsing the tree from top to bottom then
     * from left to right we increment the index and assigned it to the current
     * node. Doing so, we get a unique index for each element. This is
     * precisely what the index passed in argument of the function represents.
     *
     * @param index represents the position of the element to be removed.
     */
    void eraseByPos(Size index);

    /**
     * @brief Removes a given element from the priority queue (but does not
     * return it).
     *
     * If the element cannot be found, the function returns without throwing
     * any exception.
     *
     * If the queue contains several times this element, then the one with the
     * smallest index is removed.
     *
     * @param val the element we wish to remove.
     */
    void erase(Val val);

    /**
     * @brief Modifies the priority of the element at position "index" of the
     * queue.
     *
     * @param index The index of the element to update.
     * @param new_priority The element's new priority.
     * @return Returns the elements new priority.
     * @throw NotFound Raised if the element cannot be found.
     */
    Size setPriorityByPos(Size index, const Priority& new_priority);

    /**
     * @brief Modifies the priority of the element at position "index" of the
     * queue.
     *
     * @param index The index of the element to update.
     * @param new_priority The element's new priority.
     * @return Returns the elements new priority.
     * @throw NotFound Raised if the element cannot be found.
     */
    Size setPriorityByPos(Size index, Priority&& new_priority);

    /**
     * @brief Modifies the priority of each instance of a given element.
     * @param elt The value to update.
     * @param new_priority The values new priority.
     * @throw NotFound Raised if the element cannot be found.
     */
    void setPriority(Val elt, const Priority& new_priority);

    /**
     * @brief Modifies the priority of each instance of a given element.
     * @param elt The value to update.
     * @param new_priority The values new priority.
     * @throw NotFound Raised if the element cannot be found.
     */
    void setPriority(Val elt, Priority&& new_priority);

    /**
     * @brief Returns the priority of an instance of the value passed in
     * argument.
     *
     * @param elt The element for which the priority is returned.
     * @return Returns the priority of an instance of the value passed in
     * argument.
     * @throw NotFound Raised if the element cannot be found.
     */
    const Priority& priority(Val elt) const;

    /**
     * @brief Returns the priority of the value passed in argument.
     * @param index The index of the value of which the priority is returned.
     * @throw NotFound Raised if the element cannot be found.
     */
    const Priority& priorityByPos(Size index) const;

    /**
     * @brief Removes all the elements from the queue.
     */
    void clear();

    /**
     * @brief Returns a hashtable the keys of which are the values stored in
     * the queue.
     *
     * The keys of the hashtable correspond to the values stored in the
     * priority queue and, for each key, the corresponding value is the index
     * in the queue where we can find the key.
     *
     * @return Returns a hashtable the keys of which are the values stored in
     * the queue.
     */
    const HashTable< Val, Size >& allValues() const noexcept;

    /**
     * @brief Displays the content of the queue.
     * @return Returns the content of the queue.
     */
    std::string toString() const;

    /// @}
    // ============================================================================
    /// @name Fine tuning
    // ============================================================================
    /// @{

    /**
     * @brief Returns the size of the internal structure storing the priority
     * queue.
     * @return Returns the size of the internal structure storing the priority
     * queue.
     */
    Size capacity() const noexcept;

    /**
     * @brief Changes the size of the internal structure storing the priority
     * queue.
     * @param new_size The internal structure new size.
     */
    void resize(Size new_size);

    /// @}

    private:
    /// An array storing all the elements of the heap as well as their score.
    std::vector< std::pair< Priority, Val > > _heap_;

    /// A hashtable for quickly finding the elements by their value.
    HashTable< Val, Size > _indices_{HashTableConst::default_size, true, true};

    /// The number of elements in the heap.
    Size _nb_elements_{0};

    /// Comparison function.
    Cmp _cmp_;
  };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  // ===========================================================================
  // ===                           PRIORITY QUEUES                           ===
  // ===========================================================================
  /**
   * @class PriorityQueue
   * @headerfile priorityQueue.h <agrum/tools/core/priorityQueue.h>
   * @brief A priorityQueue is a heap in which each element has a mutable
   * priority
   * @ingroup basicstruct_group
   * @ingroup priorityqueue_group
   *
   * A priority queue is quite similar to a heap except that a priority (a
   * score) is assigned to each element in the structure. The elements are
   * sorted according to a weak order on the scores. The priority of any
   * element can be changed at any moment by the user. The priority queue then
   * restores a heap property accordingly. Duplicate elements are not allowed
   * in priority queues; if you wish an element to appear several times with
   * different priorities, prefer using class MultiplePriorityQueue.
   *
   * @par Usage example:
   * @code
   * // create a priority queue of strings, the priorities of which are integers
   * // the element at the top of the queue has the smallest priority
   * PriorityQueue<std::string> queue1;
   *
   * // insert elements into the queue
   * queue1.insert ("AAA", 8);
   * queue1.insert ("BBB", 10);
   * queue1.insert ("CCC", 2);
   * queue1.insert ("DDD", 23);
   * queue1.insert ("EEE", 24);
   *
   * // copy the queue
   * PriorityQueue<std::string> queue2 = queue1;
   *
   * // initializer list constructor
   * PriorityQueue<std::string,int> queue3 { std::pair<std::string,int>("aa",3),
   *                                         std::pair<std::string,int>("bb",2)
   * };
   *
   * // create a priority queue of strings, the priorities of which are
   * // pairs of ints
   * PriorityQueue< std::string, std::pair<int,int> > queue3;
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
   *
   * @tparam Val The values type.
   * @tparam Priority The priorities type.
   * @tparam Cmp The priorities comparator.
   */
  template < typename Val, typename Priority = int, typename Cmp = std::less< Priority > >
  class PriorityQueue:
      public PriorityQueueImplementation< Val, Priority, Cmp, std::is_scalar< Val >::value > {
    public:
    /// Types for STL compliance.
    /// @{
    using value_type      = Val;
    using reference       = Val&;
    using const_reference = const Val&;
    using pointer         = Val*;
    using const_pointer   = const Val*;
    using difference_type = std::ptrdiff_t;
    /// @}

    using Implementation
       = PriorityQueueImplementation< Val, Priority, Cmp, std::is_scalar< Val >::value >;

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Basic constructor. Creates an empty priority queue.
     *
     * @param compare a function taking two elements in argument, say e1 and
     * e2, and returning a Boolean indicating whether e1 < e2, i.e., whether e1
     * should be nearer than e2 to the top of the heap.
     * @param capacity the size of the internal data structures containing the
     * elements (could be for instance vectors or hashtables).
     */
    explicit PriorityQueue(Cmp  compare  = Cmp(),
                           Size capacity = GUM_PRIORITY_QUEUE_DEFAULT_CAPACITY);

    /**
     * @brief Initializer list constructor.
     *
     * The elements of the initializer list are pairs <Val,Priority>.
     * The comparison function is the default one, i.e., std::less<Priority>.
     *
     * @param list The initializer list.
     */
    explicit PriorityQueue(std::initializer_list< std::pair< Val, Priority > > list);

    /**
     * @brief Copy constructor.
     * @param from The gum::PriorityQueue to copy.
     */
    PriorityQueue(const PriorityQueue< Val, Priority, Cmp >& from);

    /**
     * @brief Move constructor.
     * @param from The gum::PriorityQueue to move.
     */
    PriorityQueue(PriorityQueue< Val, Priority, Cmp >&& from);

    /**
     * Class destructor.
     */
    ~PriorityQueue();

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
     * coherent state. Actually, the priority queue becomes empty. An exception
     * is then thrown.
     *
     * @param from The gum::PriorityQueue to copy.
     * @return Returns this gum::PriorityQueue.
     */
    PriorityQueue< Val, Priority, Cmp >& operator=(const PriorityQueue< Val, Priority, Cmp >& from);

    /**
     * @brief Move operator.
     * @param from The gum::PriorityQueue to move.
     * @return Returns this gum::PriorityQueue.
     */
    PriorityQueue< Val, Priority, Cmp >& operator=(PriorityQueue< Val, Priority, Cmp >&& from);

    /// @}
  };

} /* namespace gum */

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::PriorityQueue< std::string >;
extern template class gum::PriorityQueue< int, int >;
#endif

// always include the implementation of the templates
#include <agrum/tools/core/priorityQueue_tpl.h>

#endif /* GUM_PRIORITY_QUEUE_H */
