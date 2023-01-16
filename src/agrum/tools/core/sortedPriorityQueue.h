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
#include <agrum/tools/core/sharedAVLTree.h>
#include <agrum/tools/core/priorityQueue.h>

namespace gum {

  template < typename Val, typename Priority, typename Cmp >
  class SortedPriorityQueueIterator;
  template < typename Val, typename Priority, typename Cmp >
  class SortedPriorityQueueIteratorSafe;

  template < typename Val, typename Priority, typename Cmp >
  class SortedPriorityQueueReverseIterator;
  template < typename Val, typename Priority, typename Cmp >
  class SortedPriorityQueueReverseIteratorSafe;


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
  template < typename Val, typename Priority = int, typename Cmp = std::less< Priority > >
  class SortedPriorityQueue {
    public:
    /// Types for STL compliance.
    /// @{
    using value_type            = Val;
    using reference             = Val&;
    using const_reference       = const Val&;
    using pointer               = Val*;
    using const_pointer         = const Val*;
    using difference_type       = std::ptrdiff_t;
    using iterator              = SortedPriorityQueueIterator< Val, Priority, Cmp >;
    using iterator_safe         = SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >;
    using reverse_iterator      = SortedPriorityQueueReverseIterator< Val, Priority, Cmp >;
    using reverse_iterator_safe = SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >;
    /// @}


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
    explicit SortedPriorityQueue(Cmp  compare  = Cmp(),
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
    /// @name Iterators
    // ============================================================================
    /// @{

    /// returns a new iterator pointing to the minimal element of the tree
    iterator begin() const;

    /// returns an iterator pointing just after the maximal element
    constexpr const iterator& end() const;

    /// returns a new iterator pointing to the maximal element of the tree
    reverse_iterator rbegin() const;

    /// returns an iterator pointing just before the minimal element
    constexpr const reverse_iterator& rend() const;

    /// returns a new safe iterator pointing to the minimal element of the tree
    iterator_safe beginSafe();

    /// returns a safe iterator pointing just after the maximal element
    constexpr const iterator_safe& endSafe() const;

    /// returns a safe iterator pointing to the maximal element of the tree
    reverse_iterator_safe rbeginSafe();

    /// returns a safe iterator pointing just before the minimal element
    constexpr const reverse_iterator_safe& rendSafe() const;

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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    /// the types of the nodes in the AVL tree
    using AVLNode = AVLTreeNode< Val >;

    /// the types of the elements stored into the hash table
    using HashElt = typename std::pair< const AVLTreeNode< Val >, Priority >;

    /// the comparison function used to sort the elements in the tree
    struct TreeCmp {
      TreeCmp() = default;
      TreeCmp(const Cmp& cmp) : _cmp_(cmp) {}
      TreeCmp(Cmp&& cmp) : _cmp_(std::move(cmp)) {}

      // get the priority associated with a given AVLTreeNode value. It turns out
      // that the elements stored into the HashTable are of type HashElt defined
      // above. Now, the constexpr offset defined below contains precisely the
      // offset in bytes between Value v and its priority. Function getPriority
      // therefore just computes the location of the priority and returns it.
      inline const Priority& getPriority(const Val& v) const {
        return *((Priority*)((char*)&v + offset_from_value_to_priority));
      }

      // from a Val instance stored into memory, get the address of the start of
      // the AVLNode structure that would have contained it. This is useful for
      // comparing elements in the hashTable without having to create dummy AVLNodes
      inline AVLTreeNode< Val >* getNode(const Val& v) const {
        return (AVLTreeNode< Val >*)((char*)&v - offset_to_value);
      }

      // the comparison function betwwen two contents of two nodes of the AVL tree:
      // the nodes should be sorted according to their priority
      constexpr bool operator()(const Val& x, const Val& y) const {
        return _cmp_(getPriority(x), getPriority(y));
      }

      Cmp _cmp_;

      // compute the offset in bytes between the location of Val and Priority in
      // the elements stored into the hash table
      static constexpr std::size_t offset_to_priority = offsetof(HashElt, second);
      static constexpr std::size_t offset_to_value    = offsetof(AVLTreeNode< Val >, value);
      static constexpr std::size_t offset_from_value_to_priority
         = offset_to_priority - offset_to_value;
    };
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    private:
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

    /** @brief returns the node in the hash table corresponding to a given value
     * @throws NotFound is raised if the node cannot be found
     */
    AVLNode& getNode_(const Val& val) const;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // the TreeCmp getnode function induces some warning on some compilers when Val
    // is a string because some optimizations are made by the compiler in this case.
    // to avoid this, the getNode_ function above executes different codes depending
    // on whether Val is different from a string or not. The structs below allow
    // to make this discrimination at compile time.
    template < typename T >
    struct is_basic_string: std::false_type {};
    template < typename T1, typename T2, typename T3 >
    struct is_basic_string< std::basic_string< T1, T2, T3 > >: std::true_type {};
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /// make the iterators access to the AVLNodes
    friend iterator;
    friend reverse_iterator;
    friend iterator_safe;
    friend reverse_iterator_safe;
  };


  /**
   * @class SortedPriorityQueueIterator
   * @headerfile sortedPriorityQueue.h <agrum/tools/core/sortedPriorityQueue.h>
   * @ingroup basicstruct_group
   * @brief Sorted priority queue iterator
   *
   * @tparam Val The type of the elements stored into the queue.
   * @tparam Priority The type of the priorities assigned to the elements.
   * @tparam Cmp The function used for sorting the priorities.
   */
  template < typename Val, typename Priority = int, typename Cmp = std::less< Priority > >
  class SortedPriorityQueueIterator:
      protected SharedAVLTreeReverseIterator<
         Val,
         typename SortedPriorityQueue< Val, Priority, Cmp >::TreeCmp > {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = Val;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using TreeCmp           = typename SortedPriorityQueue< Val, Priority, Cmp >::TreeCmp;
    /// @}


    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /** @brief constructor for begin iterators
     *
     * @param queue the sorted priority queue which parses the iterator
     * @param begin if true, the iterator points to the highest value in the queue,
     * else it points to the lowest value
     */
    explicit SortedPriorityQueueIterator(const SortedPriorityQueue< Val, Priority, Cmp >& queue,
                                         const bool begin = true) noexcept;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static end iterator
    // only sortedPriorityQueue.cpp should use this constructor
    explicit consteval SortedPriorityQueueIterator(StaticInitializer init) noexcept :
        SharedAVLTreeReverseIterator< Val, TreeCmp >(init) {}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /// copy constructor
    SortedPriorityQueueIterator(
       const SortedPriorityQueueIterator< Val, Priority, Cmp >& from) noexcept;

    /// move constructor
    SortedPriorityQueueIterator(SortedPriorityQueueIterator< Val, Priority, Cmp >&& from) noexcept;

    /// destructor
    ~SortedPriorityQueueIterator() noexcept;

    /// @}


    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /// copy operator
    SortedPriorityQueueIterator< Val, Priority, Cmp >&
       operator=(const SortedPriorityQueueIterator< Val, Priority, Cmp >& from) noexcept;

    /// move operator
    SortedPriorityQueueIterator< Val, Priority, Cmp >&
       operator=(SortedPriorityQueueIterator< Val, Priority, Cmp >&& from) noexcept;

    /// indicates whether two iterator point to the same element
    bool operator==(const SortedPriorityQueueIterator< Val, Priority, Cmp >& from) const;

    /// indicates whether two iterator point to different element
    bool operator!=(const SortedPriorityQueueIterator< Val, Priority, Cmp >& from) const;

    /// move to the next element in the queue
    /** @warning Applying this operator to an iterator equal to the end iterator
     * has no effect. In particular, it does not raise any exception. */
    SortedPriorityQueueIterator< Val, Priority, Cmp >& operator++() noexcept;

    /// move to the next k element
    /** @warning Applying this operator to an iterator equal to the end iterator
     * has no effect. In particular, it does not raise any exception. */
    SortedPriorityQueueIterator< Val, Priority, Cmp >& operator+=(const Size k) noexcept;

    /// move to the preceding element in the queue
    /** @warning Applying this operator to an iterator equal to the begin iterator
     * has no effect. In particular, it does not raise any exception. */
    SortedPriorityQueueIterator< Val, Priority, Cmp >& operator--() noexcept;

    /// move to the preceding k element
    /** @warning Applying this operator to an iterator equal to the begin iterator
     * has no effect. In particular, it does not raise any exception. */
    SortedPriorityQueueIterator< Val, Priority, Cmp >& operator-=(const Size k) noexcept;

    /** @brief returns the value pointed to by the iterator
     *
     * @return the value pointed to by the iterator, if the iterator actually points
     * to an element
     * @throws NotFound is raised if the iterator points to nothing
     */
    const_reference operator*() const;

    /// returns a pointer on the value pointed to by the iterator
    const_pointer operator->() const;

    /// @}

    /// allow sorted priority queues to access the content of the iterators
    friend SortedPriorityQueue< Val, Priority, Cmp >;
  };


  /**
   * @class SortedPriorityQueueIteratorSafe
   * @headerfile sortedPriorityQueue.h <agrum/tools/core/sortedPriorityQueue.h>
   * @ingroup basicstruct_group
   * @brief Sorted priority queues safe (w.r.t. erasures) iterator
   *
   * @tparam Val The type of the elements stored into the queue.
   * @tparam Priority The type of the priorities assigned to the elements.
   * @tparam Cmp The function used for sorting the priorities.
   */
  template < typename Val, typename Priority = int, typename Cmp = std::less< Val > >
  class SortedPriorityQueueIteratorSafe:
      protected SharedAVLTreeReverseIteratorSafe<
         Val,
         typename SortedPriorityQueue< Val, Priority, Cmp >::TreeCmp > {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = Val;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using TreeCmp           = typename SortedPriorityQueue< Val, Priority, Cmp >::TreeCmp;
    /// @}


    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /** @brief constructor for begin safe iterators
     *
     * @param queue the sorted priority queue which parses the iterator
     * @param begin if true, the iterator points to the highest value in the queue,
     * else it points to the lowest value
     */
    explicit SortedPriorityQueueIteratorSafe(SortedPriorityQueue< Val, Priority, Cmp >& queue,
                                             const bool rbegin = true);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static endSafe iterator
    // only sortedPriorityQueue.cpp should use this constructor
    explicit consteval SortedPriorityQueueIteratorSafe(StaticInitializer init) noexcept :
        SharedAVLTreeReverseIteratorSafe< Val, TreeCmp >(init) {}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /// copy constructor
    SortedPriorityQueueIteratorSafe(
       const SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >& from);

    /// move constructor
    SortedPriorityQueueIteratorSafe(SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >&& from);

    /// destructor
    ~SortedPriorityQueueIteratorSafe() noexcept;

    /// @}

    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /// copy operator
    SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >&
       operator=(const SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >& from);

    /// move operator
    SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >&
       operator=(SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >&& from);

    /// indicates whether two iterator point to the same element
    bool operator==(const SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >& from) const;

    /// indicates whether two iterator point to different element
    bool operator!=(const SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >& from) const;

    /// move to the next element in the queue
    /** @warning Applying this operator to an iterator equal to the end iterator
     * has no effect. In particular, it does not raise any exception. */
    SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >& operator++() noexcept;

    /// move to the next k elements
    /** @warning Applying this operator to an iterator equal to the end iterator
     * has no effect. In particular, it does not raise any exception. */
    SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >& operator+=(const Size k) noexcept;

    /// move to the preceding element in the queue
    /** @warning Applying this operator to an iterator equal to the begin iterator
     * has no effect. In particular, it does not raise any exception. */
    SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >& operator--() noexcept;

    /// move to the preceding k element
    /** @warning Applying this operator to an iterator equal to the begin iterator
     * has no effect. In particular, it does not raise any exception. */
    SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >& operator-=(const Size k) noexcept;

    /** @brief returns the value pointed to by the iterator
     *
     * @return the value pointed to by the iterator, if the iterator actually points
     * to an element
     * @throws NotFound is raised if the iterator points to nothing
     */
    const_reference operator*() const;

    /// returns a pointer on the value pointed to by the iterator
    const_pointer operator->() const;

    /// @}

    protected:
    /// allow sorted priority queues to access the content of the iterators
    friend SortedPriorityQueue< Val, Cmp >;
  };


  /**
   * @class SortedPriorityQueueReverseIterator
   * @headerfile sortedPriorityQueue.h <agrum/tools/core/sortedPriorityQueue.h>
   * @ingroup basicstruct_group
   * @brief Sorted priority queue reverse iterator
   *
   * @tparam Val The type of the elements stored into the queue.
   * @tparam Priority The type of the priorities assigned to the elements.
   * @tparam Cmp The function used for sorting the priorities.
   */
  template < typename Val, typename Priority = int, typename Cmp = std::less< Val > >
  class SortedPriorityQueueReverseIterator:
      protected SharedAVLTreeIterator<
         Val,
         typename SortedPriorityQueue< Val, Priority, Cmp >::TreeCmp > {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = Val;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using TreeCmp           = typename SortedPriorityQueue< Val, Priority, Cmp >::TreeCmp;
    /// @}


    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /** @brief constructor for rbegin iterators
     *
     * @param queue the sorted priority queue which parses the iterator
     * @param begin if true, the iterator points to the lowest value in the queue,
     * else it points to the highest value
     */
    explicit SortedPriorityQueueReverseIterator(
       const SortedPriorityQueue< Val, Priority, Cmp >& queue,
       const bool                                       rbegin = true) noexcept;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static rend iterator
    // only sortedPriorityQueue.cpp should use this constructor
    explicit consteval SortedPriorityQueueReverseIterator(StaticInitializer init) noexcept :
        SharedAVLTreeIterator< Val, TreeCmp >(init) {}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /// copy constructor
    SortedPriorityQueueReverseIterator(
       const SortedPriorityQueueReverseIterator< Val, Priority, Cmp >& from) noexcept;

    /// move constructor
    SortedPriorityQueueReverseIterator(
       SortedPriorityQueueReverseIterator< Val, Priority, Cmp >&& from) noexcept;

    /// destructor
    ~SortedPriorityQueueReverseIterator() noexcept;

    /// @}

    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /// copy operator
    SortedPriorityQueueReverseIterator< Val, Priority, Cmp >&
       operator=(const SortedPriorityQueueReverseIterator< Val, Priority, Cmp >& from) noexcept;

    /// move operator
    SortedPriorityQueueReverseIterator< Val, Priority, Cmp >&
       operator=(SortedPriorityQueueReverseIterator< Val, Priority, Cmp >&& from) noexcept;

    /// indicates whether two iterator point to the same element
    bool operator==(const SortedPriorityQueueReverseIterator< Val, Priority, Cmp >& from) const;

    /// indicates whether two iterator point to different element
    bool operator!=(const SortedPriorityQueueReverseIterator< Val, Priority, Cmp >& from) const;

    /// move to the next element in the queue
    /** @warning Applying this operator to an iterator equal to the rend iterator
     * has no effect. In particular, it does not raise any exception. */
    SortedPriorityQueueReverseIterator< Val, Priority, Cmp >& operator++() noexcept;

    /// move to the next k element
    /** @warning Applying this operator to an iterator equal to the rend iterator
     * has no effect. In particular, it does not raise any exception. */
    SortedPriorityQueueReverseIterator< Val, Priority, Cmp >& operator+=(const Size k) noexcept;

    /// move to the preceding element in the queue
    /** @warning Applying this operator to an iterator equal to the rbegin iterator
     * has no effect. In particular, it does not raise any exception. */
    SortedPriorityQueueReverseIterator< Val, Priority, Cmp >& operator--() noexcept;

    /// move to the preceding k element
    /** @warning Applying this operator to an iterator equal to the rbegin iterator
     * has no effect. In particular, it does not raise any exception. */
    SortedPriorityQueueReverseIterator< Val, Priority, Cmp >& operator-=(const Size k) noexcept;

    /** @brief returns the value pointed to by the iterator
     *
     * @return the value pointed to by the iterator, if the iterator actually points
     * to an element
     * @throws NotFound is raised if the iterator points to nothing
     */
    const_reference operator*() const;

    /// returns a pointer on the value pointed to by the iterator
    const_pointer operator->() const;

    /// @}


    protected:
    /// allow sorted priority queues to access the content of the iterators
    friend SortedPriorityQueue< Val, Priority, Cmp >;
  };


  /**
   * @class SortedPriorityQueueReverseIteratorSafe
   * @headerfile sortedPriorityQueue.h <agrum/tools/core/sortedPriorityQueue.h>
   * @ingroup basicstruct_group
   * @brief Sorted priority queue safe (w.r.t. erasures) reverse iterator
   *
   * @tparam Val The type of the elements stored into the queue.
   * @tparam Priority The type of the priorities assigned to the elements.
   * @tparam Cmp The function used for sorting the priorities.
   */
  template < typename Val, typename Priority = int, typename Cmp = std::less< Val > >
  class SortedPriorityQueueReverseIteratorSafe:
      protected SharedAVLTreeIteratorSafe<
         Val,
         typename SortedPriorityQueue< Val, Priority, Cmp >::TreeCmp > {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = Val;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using TreeCmp           = typename SortedPriorityQueue< Val, Priority, Cmp >::TreeCmp;
    /// @}


    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /** @brief constructor for rbegin safe iterators
     *
     * @param queue the sorted priority queue which parses the iterator
     * @param begin if true, the iterator points to the lowest value in the queue,
     * else it points to the highest value
     */
    explicit SortedPriorityQueueReverseIteratorSafe(
       SortedPriorityQueue< Val, Priority, Cmp >& queue,
       const bool                                 rbegin = true);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static rendSafe iterator
    // only sortedPriorityQueue.cpp should use this constructor
    explicit consteval SortedPriorityQueueReverseIteratorSafe(StaticInitializer init) noexcept :
        SharedAVLTreeIteratorSafe< Val, TreeCmp >(init) {}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /// copy constructor
    SortedPriorityQueueReverseIteratorSafe(
       const SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >& from);

    /// move constructor
    SortedPriorityQueueReverseIteratorSafe(
       SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >&& from);

    /// destructor
    ~SortedPriorityQueueReverseIteratorSafe() noexcept;

    /// @}

    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /// copy operator
    SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >&
       operator=(const SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >& from);

    /// move operator
    SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >&
       operator=(SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >&& from);

    /// indicates whether two iterator point to the same element
    bool operator==(const SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >& from) const;

    /// indicates whether two iterator point to different element
    bool operator!=(const SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >& from) const;

    /// move to the next element in the queue
    /** @warning Applying this operator to an iterator equal to the rend iterator
     * has no effect. In particular, it does not raise any exception. */
    SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >& operator++() noexcept;

    /// move to the next k element
    /** @warning Applying this operator to an iterator equal to the rend iterator
     * has no effect. In particular, it does not raise any exception. */
    SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >& operator+=(const Size k) noexcept;

    /// move to the preceding element in the queue
    /** @warning Applying this operator to an iterator equal to the rbegin iterator
     * has no effect. In particular, it does not raise any exception. */
    SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >& operator--() noexcept;

    /// move to the preceding k element
    /** @warning Applying this operator to an iterator equal to the rbegin iterator
     * has no effect. In particular, it does not raise any exception. */
    SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >& operator-=(const Size k) noexcept;

    /** @brief returns the value pointed to by the iterator
     *
     * @return the value pointed to by the iterator, if the iterator actually points
     * to an element
     * @throws NotFound is raised if the iterator points to nothing
     */
    const_reference operator*() const;

    /// returns a pointer on the value pointed to by the iterator
    const_pointer operator->() const;

    /// @}

    protected:
    /// allow AVL trees to access the content of the iterators
    friend AVLTree< Val, Cmp >;
    friend SortedPriorityQueue< Val, Cmp >;
  };


  /// display the content of a sorted priority queue
  template < typename Val, typename Priority, typename Cmp >
  std::ostream& operator<<(std::ostream&                                    stream,
                           const SortedPriorityQueue< Val, Priority, Cmp >& queue) {
    return stream << queue.toString();
  }


#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // _static_SortedPriorityQueue_end_ is a 'constant' iterator initialized at
  // compile time that represents the end iterators for all the sorted priority
  // queues (whatever their type). This global variable avoids creating the same
  // iterators within every sorted priority queue instance (this would be quite
  // inefficient as end is precisely identical for all these queues). The same hold
  // for reverse and safe end iterators.
  // The type of _SortedPriorityQueue_end_ is a pointer to void because C++ allows
  // pointers to void to be cast into pointers to other types (and conversely).
  // This avoids the painful strict-aliasing rule warning
  extern const SortedPriorityQueueIterator< int, std::less< int > >
     _static_SortedPriorityQueue_end_;
  extern const SortedPriorityQueueReverseIterator< int, std::less< int > >
     _static_SortedPriorityQueue_rend_;
  extern const SortedPriorityQueueIteratorSafe< int, std::less< int > >
     _static_SortedPriorityQueue_end_safe_;
  extern const SortedPriorityQueueReverseIteratorSafe< int, std::less< int > >
     _static_SortedPriorityQueue_rend_safe_;

  inline constexpr void* const _SortedPriorityQueue_end_
     = (void* const)&_static_SortedPriorityQueue_end_;
  inline constexpr void* const _SortedPriorityQueue_rend_
     = (void* const)&_static_SortedPriorityQueue_rend_;
  inline constexpr void* const _SortedPriorityQueue_end_safe_
     = (void* const)&_static_SortedPriorityQueue_end_safe_;
  inline constexpr void* const _SortedPriorityQueue_rend_safe_
     = (void* const)&_static_SortedPriorityQueue_rend_safe_;
#endif   // DOXYGEN_SHOULD_SKIP_THIS

} /* namespace gum */


// always include the implementation of the templates
#include <agrum/tools/core/sortedPriorityQueue_tpl.h>

#endif /* GUM_SORTED_PRIORITY_QUEUE_H */
