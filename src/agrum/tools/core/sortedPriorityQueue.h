/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Priority queues which can be parsed using iterators
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */

#ifndef GUM_SORTED_PRIORITY_QUEUE_H
#define GUM_SORTED_PRIORITY_QUEUE_H

#include <functional>
#include <initializer_list>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <cstddef>

#include <agrum/agrum.h>
#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/core/sharedAVLTree.h>
#include <agrum/tools/core/priorityQueue.h>

namespace gum {

  /**
   * @class SortedPriorityQueue
   * @headerfile sortedPriorityQueue.h <agrum/tools/core/sortedPriorityQueue.h>
   * @ingroup priorityqueue_group
   * @brief A priority queue in which we can iterate over the elements from the
   * top to bottom or conversely
   *
   * A sorted priority queue is quite similar to a binary search tree except that
   * a priority (a score) is assigned to each element in the structure. The
   * elements are sorted according to a weak order on theses scores. The priority
   * of any element can be modified at any moment by the user. The sorted priority
   * queue then restores a binary search tree property accordingly. Duplicate
   * elements are not allowed in sorted priority queues.
   *
   * @par Usage example:
   * @code
   * // create a priority queue of strings, the priorities of which are integers
   * // the element at the top of the queue has the smallest priority
   * SortedPriorityQueue<std::string> queue1;
   *
   * // insert elements into the queue
   * queue1.insert ("AAA", 8);
   * queue1.insert ("BBB", 10);
   * queue1.insert ("CCC", 2);
   * queue1.insert ("DDD", 23);
   * queue1.insert ("EEE", 24);
   *
   * // copy the queue
   * SortedPriorityQueue<std::string> queue2 = queue1;
   *
   * // initializer list constructor
   * SortedPriorityQueue<std::string,int> queue3 {
   *   std::pair<std::string,int>("aa",3),
   *   std::pair<std::string,int>("bb",2)
   * };
   *
   * // create a sorted priority queue of strings, the priorities of which are
   * // pairs of ints
   * SortedPriorityQueue< std::string, std::pair<int,int> > queue3;
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
   * // change the priority of the instance of element "AAA"
   * queue1.setPriority ("AAA",100);
   * @endcode
   *
   * @tparam Val The values type.
   * @tparam Priority The priorities type.
   * @tparam Cmp The priorities comparator.
   */
  template < typename Val, typename Priority, typename Cmp >
  class SortedPriorityQueue {
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
     * @brief Basic constructor. Creates an empty sorted priority queue.
     *
     * @param compare A function taking two elements in argument, say e1 and
     * e2, and returning a Boolean indicating whether e1 < e2, i.e., whether e1
     * should be nearer than e2 to the top of the heap.
     * @param capacity The size of the internal data structures containing the
     * elements (could be for instance vectors or hashtables)
     */
    explicit SortedPriorityQueue(Cmp compare,
                                 Size capacity = GUM_PRIORITY_QUEUE_DEFAULT_CAPACITY);

    /**
     * @brief Initializer list constructor.
     *
     * The elements of the initializer list are pairs <Val,Priority>.
     *
     * @param list The initializer list.
     */
    explicit SortedPriorityQueue(std::initializer_list< std::pair< Val, Priority > > list);

    /**
     * @brief Copy constructor.
     * @param from The gum::SortedPriorityQueue to copy.
     */
    SortedPriorityQueue(const SortedPriorityQueue< Val, Priority, Cmp >& from);

    /**
     * @brief Move constructor.
     * @param from The gum::SortedPriorityQueue to move.
     */
    SortedPriorityQueue(SortedPriorityQueue< Val, Priority, Cmp >&& from) noexcept;

    /**
     * @brief Class destructor.
     */
    ~SortedPriorityQueue();

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
     * memory is available), the operator guarantees that the queue stays in a
     * coherent state. Actually, the priority queue becomes empty. An exception
     * is then thrown.
     *
     * @param from The gum::SortedPriorityQueue to copy.
     * @return Returns this gum::SortedPriorityQueue.
     */
    SortedPriorityQueue< Val, Priority, Cmp >&
       operator=(const SortedPriorityQueue< Val, Priority, Cmp >& from);

    /**
     * @brief Move operator.
     *
     * @param from The gum::SortedPriorityQueue to move.
     * @return Returns this gum::SortedPriorityQueue.
     */
    SortedPriorityQueue< Val, Priority, Cmp >&
       operator=(SortedPriorityQueue< Val, Priority, Cmp >&& from) noexcept;

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
    bool contains(const Val& val) const noexcept;

    /// returns the element at the top of the sorted priority queue
    /** @throw NotFound Raised if the queue is empty */
    const Val& top() const;

    /// returns the element at the bottom of the sorted priority queue
    /** @throw NotFound Raised if the queue is empty */
    const Val& bottom() const;

    /**
     * @brief Returns the priority of the top element.
     * @return Returns the priority of the top element.
     * @throw NotFound Raised if the queue is empty.
     */
    const Priority& topPriority() const;

    /**
     * @brief Returns the priority of the bottom element.
     * @return Returns the priority of the bottom element.
     * @throw NotFound Raised if the queue is empty.
     */
    const Priority& bottomPriority() const;

    /**
     * @brief Removes the top element from the priority queue and return it.
     * @return Returns the top element from the priority queue.
     * @throw NotFound Raised if the queue is empty.
     */
    value_type pop();

    /**
     * @brief Alias of pop
     * @return Returns the top element from the priority queue.
     * @throw NotFound Raised if the queue is empty.
     */
    value_type popTop();

    /**
     * @brief Removes the bottom element from the priority queue and return it.
     * @return Returns the bottom element from the priority queue.
     * @throw NotFound Raised if the queue is empty.
     */
    value_type popBottom();

    /**
     * @brief Inserts a new (a copy) element in the priority queue.
     *
     * @param val The value to insert.
     * @param priority The value's priority in the queue.
     * @return Returns a reference on the value stored into the queue.
     * @throw DuplicateElement Raised if the element already exists.
     */
    const_reference insert(const Val& val, const Priority& priority);

    /**
     * @brief Inserts (by move) a new element in the priority queue.
     *
     * @param val The value to insert.
     * @param priority The value's priority in the queue.
     * @return Returns a reference on the value stored into the queue.
     * @throw DuplicateElement Raised if the element already exists.
     */
    const_reference insert(Val&& val, Priority&& priority);

    /**
     * @brief Emplace a new element into the priority queue.
     *
     * @tparam Args The emplace arguments types.
     * @param args The emplace arguments.
     * @return Returns a reference on the value stored into the queue.
     * @throw DuplicateElement Raised if the element already exists.
     */
    template < typename... Args >
    const_reference emplace(Args&&... args);

    /**
     * @brief Removes the top of the priority queue (but does not return it).
     *
     * If the priority queue is empty, it does nothing (in particular, it does not
     * throw any exception).
     */
    void eraseTop();

    /**
     * @brief Removes the bottom of the priority queue (but does not return it).
     *
     * If the priority queue is empty, it does nothing (in particular, it does not
     * throw any exception).
     */
    void eraseBottom();

    /**
     * @brief Removes a given element from the priority queue (but does not
     * return it).
     *
     * If the element cannot be found, the function returns without throwing
     * any exception.
     *
     * @param val the element we wish to remove.
     */
    void erase(const Val& val);

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
     * @brief Removes all the elements from the queue.
     */
    void clear();

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

    /// the types of the nodes in the AVL tree
    using AVLNode = AVLTreeNode< Val >;

    /// the types of the elements stored into the hash table
    using HashElt = typename std::pair< const AVLTreeNode< Val>, Priority >;

    /// the comparison function used to sort the elements in the tree
    struct TreeCmp {
      TreeCmp(const Cmp& cmp) : _cmp_(cmp) {}
      TreeCmp(Cmp&& cmp) : _cmp_(std::move(cmp)) {}

      // get the priority associated with a given AVLTreeNode value. It turns out
      // that the elements stored into the HashTable are of type HashElt defined
      // above. Now, the constexpr offset defined below contains precisely the
      // offset in bytes between Value v and its priority. Function getPriority
      // therefore just computes the location of the priority and returns it.
      inline const Priority& getPriority(const Val& v) {
        return *((Priority*)((char*)&v + offset_from_value_to_priority));
      }

      // from a Val instance stored into memory, get the address of the start of
      // the AVLNode structure that would have contained it. This is useful for
      // comparing elements in the hashTable without having to create dummy AVLNodes
      inline const AVLNode& getNode(const Val& v) {
        return *((AVLNode*)((char*)&v - offset_to_value));
      }

      // the comparison function betwwen two contents of two nodes of the AVL tree:
      // the nodes should be sorted according to their priority
      constexpr bool operator()(const Val& x, const Val& y) const {
        return _cmp_(getPriority(x), getPriority(y));
      }

      Cmp _cmp_;

      // compute the offset in bytes between the location of Val and Priority in
      // the elements stored into the hash table
      static constexpr std::size_t offset_to_priority = offsetof(HashElt , second);
      static constexpr std::size_t offset_to_value    = offsetof(AVLTreeNode< Val >, value);
      static constexpr std::size_t offset_from_value_to_priority =
         offset_to_priority - offset_to_value;
    };


    /**
     * @brief A binary search tree storing all the values of the queue.
     *
     * Basically, the tree does not store the priorities which are used to sort its
     * nodes. However, these nodes are guaranteed to be the first element of the
     * HashElt elements stored into the hash table. As such, using basic pointer
     * arithmetic, we can determine where the priority associated with a value is
     * located in memory. In addition, it is just after the value, which means that
     * it has a high probability to be already in the CPU cache, so jumping from
     * a value to a priority should be fast.
     */
    SharedAVLTree< Val, TreeCmp > _tree_;

    /// A hashtable for quickly finding the elements by their value.
    HashTable< AVLTreeNode< Val >, Priority > _nodes_{HashTableConst::default_size, true, true};

    /// Comparison function.
    TreeCmp _tree_cmp_;
  };

} /* namespace gum */


// always include the implementation of the templates
#include <agrum/tools/core/sortedPriorityQueue_tpl.h>

#endif /* GUM_SORTED_PRIORITY_QUEUE_H */
