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
 * @brief AVL binary serach trees
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */

#ifndef GUM_AVL_TREE_H
#define GUM_AVL_TREE_H

#include <initializer_list>
#include <string>
#include <algorithm>

#include <agrum/agrum.h>
#include <agrum/tools/core/staticInitializer.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  template < typename Val, typename Cmp >
  class AVLTreeIterator;
  template < typename Val, typename Cmp >
  class AVLTreeIteratorSafe;

  template < typename Val, typename Cmp >
  class AVLTreeReverseIterator;
  template < typename Val, typename Cmp >
  class AVLTreeReverseIteratorSafe;
#endif   // DOXYGEN_SHOULD_SKIP_THIS

  /**
   * @class AVLTree
   * @headerfile AVLTree.h <agrum/tools/core/AVLTree.h>
   * @ingroup basicstruct_group
   * @brief AVL binary search tree
   *
   * @tparam Val The type of the elements stored into the tree.
   * @tparam Cmp The function used for sorting the elements.
   * queues.
   */
  template < typename Val, typename Cmp = std::less< Val > >
  class AVLTree {
    public:
    /// Types for STL compliance.
    /// @{
    using value_type            = Val;
    using reference             = Val&;
    using const_reference       = const Val&;
    using pointer               = Val*;
    using const_pointer         = const Val*;
    using iterator              = AVLTreeIterator< Val, Cmp >;
    using iterator_safe         = AVLTreeIteratorSafe< Val, Cmp >;
    using reverse_iterator      = AVLTreeReverseIterator< Val, Cmp >;
    using reverse_iterator_safe = AVLTreeReverseIteratorSafe< Val, Cmp >;
    /// @}

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Basic constructor. Creates an empty tree.
     *
     * @param compare A function taking two elements in argument, say e1 and
     * e2, and returning a Boolean indicating whether e1 < e2, i.e., whether e1
     * should be "on the left of" e2.
     */
    explicit AVLTree(const Cmp& compare = Cmp());

    /**
     * @brief Initializer list constructor.
     *
     * A default comparison function is provided.
     *
     * @param list The initializer list.
     */
    explicit AVLTree(std::initializer_list< Val > list);

    /**
     * @brief Copy constructor.
     * @param from The gum::AVLTree to copy.
     */
    AVLTree(const AVLTree< Val, Cmp >& from);

    /**
     * @brief Move constructor.
     * @param from The gum::AVLTree to move.
     */
    AVLTree(AVLTree< Val, Cmp >&& from) noexcept;

    /**
     * @brief Class destructor.
     */
    ~AVLTree();

    /// @}

    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     *
     *
     * @param from The gum::AVLTree to copy.
     * @return Returns this gum::AVLTree.
     */
    AVLTree< Val, Cmp >& operator=(const AVLTree< Val, Cmp >& from);

    /**
     * @brief Move operator.
     *
     * @param from The gum::AVLTree to move.
     * @return Returns this gum::AVLTree.
     */
    AVLTree< Val, Cmp >& operator=(AVLTree< Val, Cmp >&& from) noexcept;

    /// @}


    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Returns the number of elements in the tree.
     * @return Returns the number of elements in the tree.
     */
    Size size() const noexcept;

    /**
     * @brief Indicates whether the tree is empty.
     * @return Returns true if the tree is empty.
     */
    bool empty() const noexcept;

    /**
     * @brief Indicates whether the tree contains a given value.
     * @param val The value to look for.
     * @return Returns true if val is in the tree.
     */
    bool contains(const value_type& val) const;

    /**
     * @brief Alias of contains: indicates whether the tree contains a given value.
     * @param val The value to look for.
     * @return Returns true if val is in the tree.
     */
    bool exists(const value_type& val) const;

    /// returns the max element (w.r.t. Cmp) in the tree
    /** @throw NotFound Raised if the queue is empty */
    const value_type& highestValue() const;

    /// returns the min element (w.r.t. Cmp) in the tree
    /** @throw NotFound Raised if the queue is empty */
    const value_type& lowestValue() const;

    /// adds (by copy) a new element into the tree
    const value_type& insert(const value_type& val);

    /// adds (by move) a new element into the tree
    const value_type& insert(value_type&& val);

    /// emplace a new element into the tree
    template < typename... Args >
    const value_type& emplace(Args&&... args);

    /// remove an element from the tree
    /** If the element does not exist, the function does nothing. In particular, it
     * does not raise any exception.
     *
     * For speed reasons, if you already have an iterator pointing to val, prefer
     * using erase(iterator) rather than erase(val). */
    void erase(const value_type& val);

    /// remove the element pointed to by an iterator
    /** @warning after removing the element, you can still use safely the ++operator
     * (resp. --operator) to move to the next (resp. preceding) elements. This
     * results from the fact that the iterators compute their next and preceding
     * nodes as soon as they move to a new noed. As a result, do not use a ++
     * followed by a -- operator, or a -- followed by a ++ operator, just after
     * removing the element because it will create a segfault. */
    void erase(iterator_safe& iter);

    /// remove the element pointed to by an iterator
    /** @warning after removing the element, you can still use safely the ++operator
     * (resp. --operator) to move to the next (resp. preceding) elements. This
     * results from the fact that the iterators compute their next and preceding
     * nodes as soon as they move to a new noed. As a result, do not use a ++
     * followed by a -- operator, or a -- followed by a ++ operator, just after
     * removing the element because it will create a segfault. */
    void erase(reverse_iterator_safe& iter);

    /// remove all the elements in the tree
    void clear();

    /// @}

    // ============================================================================
    /// @name Iterators
    // ============================================================================
    /// @{

    /// returns a new iterator pointing to the minimal element of the tree
    iterator begin();

    /// returns an iterator pointing just after the maximal element
    constexpr const iterator& end() const;

    /// returns a new iterator pointing to the maximal element of the tree
    reverse_iterator rbegin();

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

    private:
    // the nodes of the AVL tree used to sort the elements of the queue
    struct AVLNode {
      // the neighbors in our AVL tree
      AVLNode* parent{nullptr};
      AVLNode* left_child{nullptr};
      AVLNode* right_child{nullptr};

      // the height of the node in the AVL tree
      int height{1};

      // the element to be stored into the node
      Val value;

      // a class to enabling emplacing values in AVLNodes
      enum class Emplace {
        EMPLACE
      };

      AVLNode(const Val& val) : value(val) { GUM_CONSTRUCTOR(AVLNode); };
      AVLNode(Val&& val) noexcept : value(std::move(val)) { GUM_CONS_CPY(AVLNode); };

      template < typename... Args >
      AVLNode(const Emplace& emplace, Args&&... args) : value(std::forward< Args >(args)...) {
        GUM_CONSTRUCTOR(AVLNode);
      }

      ~AVLNode() { GUM_DESTRUCTOR(AVLNode); }
    };

    /// the root of the AVL tree
    AVLNode* _root_node_{nullptr};

    /// the node containing the lowest element
    AVLNode* _lowest_node_{nullptr};

    /// the node containing the highest element
    AVLNode* _highest_node_{nullptr};

    /// the number of elements in the tree
    Size _nb_elements_{Size(0)};

    /// the comparison function
    Cmp _cmp_;

    /// The list of safe iterators and reverse iterators used by the AVL tree
    std::vector< iterator_safe* > _safe_iterators_;


    /// returns the node containing the lowest element of the tree
    AVLNode* _lowestNode_() const noexcept;

    /// returns the node containing the highest element of the tree
    AVLNode* _highestNode_() const noexcept;

    /// rotate the subtree rooted at q to the right
    AVLNode* _rightRotation_(AVLNode* node_q);

    /// rotate the subtree rooted at p to the left
    AVLNode* _leftRotation_(AVLNode* node_p);

    /// rebalance the tree moving up recursively from a given node
    void _rebalanceTree_(AVLNode* node);

    /// delete a node from the tree
    void _erase_(AVLNode* node);

    /// register a new safe iterator
    void _insertIntoSafeList_(iterator_safe* iter);

    /// unregister a safe iterator
    void _removeFromSafeList_(iterator_safe* iter);

    /// copies recursively a subtree of the AVL tree
    /**
     * @param from_node the root node of the subtree that we copy recursively
     * @param new_parent the parent of the root node of the copied subtree
     * @return the root node of the copied subtree
     */
    static AVLNode* _copySubtree_(const AVLNode* from_node, AVLNode* new_parent);

    /// deletes recursively a subtree of the AVL tree
    static void _deleteSubtree_(AVLNode* subtree_root_node);

    /// make the iterators access to the AVLNodes
    friend iterator;
    friend reverse_iterator;
    friend iterator_safe;
    friend reverse_iterator_safe;
  };


  /**
   * @class AVLTreeIterator
   * @headerfile AVLTree.h <agrum/tools/core/AVLTree.h>
   * @ingroup basicstruct_group
   * @brief AVL binary search tree iterator
   *
   * @tparam Val The type of the elements stored into the tree.
   * @tparam Cmp The function used for sorting the elements.
   * queues.
   */
  template < typename Val, typename Cmp = std::less< Val > >
  class AVLTreeIterator {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = Val;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    /// @}


    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /** @brief constructor for begin iterators
     *
     * @param tree the AVL tree which parses the iterator
     * @param begin if true, the iterator points to the lowest value in the tree,
     * else it points to the highest value
     */
    explicit AVLTreeIterator(const AVLTree< Val, Cmp >& tree, const bool begin = true) noexcept;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static end iterator
    // only AVLTree.cpp should use this constructor
    explicit consteval AVLTreeIterator(StaticInitializer init) noexcept {}
#endif  // DOXYGEN_SHOULD_SKIP_THIS

    /// copy constructor
    AVLTreeIterator(const AVLTreeIterator< Val, Cmp >& from) noexcept;

    /// move constructor
    AVLTreeIterator(AVLTreeIterator< Val, Cmp >&& from) noexcept;

    /// destructor
    ~AVLTreeIterator() noexcept;

    /// @}


    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /// copy operator
    AVLTreeIterator< Val, Cmp >& operator=(const AVLTreeIterator< Val, Cmp >& from) noexcept;

    /// move operator
    AVLTreeIterator< Val, Cmp >& operator=(AVLTreeIterator< Val, Cmp >&& from) noexcept;

    /// indicates whether two iterator point to the same element
    bool operator==(const AVLTreeIterator< Val, Cmp >& from) const;

    /// indicates whether two iterator point to different element
    bool operator!=(const AVLTreeIterator< Val, Cmp >& from) const;

    /// move to the next element in the tree
    /** @warning Applying this operator to an iterator equal to the end iterator
     * has no effect. In particular, it does not raise any exception. */
    AVLTreeIterator< Val, Cmp >& operator++() noexcept;

    /// move to the next k element
    /** @warning Applying this operator to an iterator equal to the end iterator
     * has no effect. In particular, it does not raise any exception. */
    AVLTreeIterator< Val, Cmp >& operator+=(const Size k) noexcept;

    /// move to the preceding element in the tree
    /** @warning Applying this operator to an iterator equal to the begin iterator
     * has no effect. In particular, it does not raise any exception. */
    AVLTreeIterator< Val, Cmp >& operator--() noexcept;

    /// move to the preceding k element
    /** @warning Applying this operator to an iterator equal to the begin iterator
     * has no effect. In particular, it does not raise any exception. */
    AVLTreeIterator< Val, Cmp >& operator-=(const Size k) noexcept;

    /** @brief returns the element pointed to by the iterator
     *
     * @return the element pointed to by the iterator, if the iterator actually points
     * to an element
     * @throws NotFound is raised if the iterator does not point to any element
     */
    const_reference operator*();

    /// @}


    protected:
    /// the type of the nodes of the tree
    using AVLNode = typename AVLTree< Val, Cmp >::AVLNode;

    /// the tree the iterator points to
    AVLTree< Val, Cmp >* tree_{nullptr};

    /// the AVLNode the iterator points to
    AVLNode* node_{nullptr};

    /// the next node to go to when applying operator++
    AVLNode* next_node_{nullptr};

    /// the preceding node to go to when applying operator--
    AVLNode* preceding_node_{nullptr};


    /// computes the node to go to when applying operator++
    AVLNode* nextNode_(AVLNode* node) const noexcept;

    /// computes the node to go to when applying operator--
    AVLNode* precedingNode_(AVLNode* node) const noexcept;

    /// make the iterator point to nothing
    void unregisterTree_() noexcept;

    /// force the iterator to point to end/rend
    void pointToEndRend_() noexcept;


    /// allow AVL trees to access the content of the iterators
    friend AVLTree< Val, Cmp >;
  };


  /**
   * @class AVLTreeIteratorSafe
   * @headerfile AVLTree.h <agrum/tools/core/AVLTree.h>
   * @ingroup basicstruct_group
   * @brief AVL binary search tree safe (w.r.t. erasures) iterator
   *
   * @tparam Val The type of the elements stored into the tree.
   * @tparam Cmp The function used for sorting the elements.
   * queues.
   */
  template < typename Val, typename Cmp = std::less< Val > >
  class AVLTreeIteratorSafe: protected AVLTreeIterator< Val, Cmp > {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = Val;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    /// @}


    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /** @brief constructor for begin safe iterators
     *
     * @param tree the AVL tree which parses the iterator
     * @param begin if true, the iterator points to the highest value in the tree,
     * else it points to the lowest value
     */
    explicit AVLTreeIteratorSafe(AVLTree< Val, Cmp >& tree, const bool rbegin = true);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static endSafe iterator
    // only AVLTree.cpp should use this constructor
    explicit consteval AVLTreeIteratorSafe(StaticInitializer init) noexcept :
        AVLTreeIterator< Val, Cmp >(init) {}
#endif  // DOXYGEN_SHOULD_SKIP_THIS

    /// copy constructor
    AVLTreeIteratorSafe(const AVLTreeIteratorSafe< Val, Cmp >& from);

    /// move constructor
    AVLTreeIteratorSafe(AVLTreeIteratorSafe< Val, Cmp >&& from);

    /// destructor
    ~AVLTreeIteratorSafe() noexcept;

    /// @}

    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /// copy operator
    AVLTreeIteratorSafe< Val, Cmp >& operator=(const AVLTreeIteratorSafe< Val, Cmp >& from);

    /// move operator
    AVLTreeIteratorSafe< Val, Cmp >& operator=(AVLTreeIteratorSafe< Val, Cmp >&& from);

    /// indicates whether two iterator point to the same element
    bool operator==(const AVLTreeIteratorSafe< Val, Cmp >& from) const;

    /// indicates whether two iterator point to different element
    bool operator!=(const AVLTreeIteratorSafe< Val, Cmp >& from) const;

    /// move to the next element in the tree
    /** @warning Applying this operator to an iterator equal to the end iterator
     * has no effect. In particular, it does not raise any exception. */
    AVLTreeIteratorSafe< Val, Cmp >& operator++() noexcept;

    /// move to the next k elements
    /** @warning Applying this operator to an iterator equal to the end iterator
     * has no effect. In particular, it does not raise any exception. */
    AVLTreeIteratorSafe< Val, Cmp >& operator+=(const Size k) noexcept;

    /// move to the preceding element in the tree
    /** @warning Applying this operator to an iterator equal to the begin iterator
     * has no effect. In particular, it does not raise any exception. */
    AVLTreeIteratorSafe< Val, Cmp >& operator--() noexcept;

    /// move to the preceding k element
    /** @warning Applying this operator to an iterator equal to the begin iterator
     * has no effect. In particular, it does not raise any exception. */
    AVLTreeIteratorSafe< Val, Cmp >& operator-=(const Size k) noexcept;

    /** @brief returns the element pointed to by the iterator
     *
     * @return the element pointed to by the iterator, if the iterator actually points
     * to an element
     * @throws NotFound is raised if the iterator does not point to any element
     */
    using AVLTreeIterator< Val, Cmp >::operator*;

    /// @}


    protected:
    /// allow AVL trees to access the content of the iterators
    friend AVLTree< Val, Cmp >;
  };


  /**
   * @class AVLTreeReverseIterator
   * @headerfile AVLTree.h <agrum/tools/core/AVLTree.h>
   * @ingroup basicstruct_group
   * @brief AVL binary search tree reverse iterator
   *
   * @tparam Val The type of the elements stored into the tree.
   * @tparam Cmp The function used for sorting the elements.
   * queues.
   */
  template < typename Val, typename Cmp = std::less< Val > >
  class AVLTreeReverseIterator: protected AVLTreeIterator< Val, Cmp > {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = Val;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    /// @}


    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /** @brief constructor for rbegin iterators
     *
     * @param tree the AVL tree which parses the iterator
     * @param begin if true, the iterator points to the highest value in the tree,
     * else it points to the lowest value
     */
    explicit AVLTreeReverseIterator(const AVLTree< Val, Cmp >& tree,
                                    const bool                 rbegin = true) noexcept;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static rend iterator
    // only AVLTree.cpp should use this constructor
    explicit consteval AVLTreeReverseIterator(StaticInitializer init) noexcept :
        AVLTreeIterator< Val, Cmp >(init) {}
#endif  // DOXYGEN_SHOULD_SKIP_THIS

    /// copy constructor
    AVLTreeReverseIterator(const AVLTreeReverseIterator< Val, Cmp >& from) noexcept;

    /// move constructor
    AVLTreeReverseIterator(AVLTreeReverseIterator< Val, Cmp >&& from) noexcept;

    /// destructor
    ~AVLTreeReverseIterator() noexcept;

    /// @}

    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /// copy operator
    AVLTreeReverseIterator< Val, Cmp >&
       operator=(const AVLTreeReverseIterator< Val, Cmp >& from) noexcept;

    /// move operator
    AVLTreeReverseIterator< Val, Cmp >&
       operator=(AVLTreeReverseIterator< Val, Cmp >&& from) noexcept;

    /// indicates whether two iterator point to the same element
    bool operator==(const AVLTreeReverseIterator< Val, Cmp >& from) const;

    /// indicates whether two iterator point to different element
    bool operator!=(const AVLTreeReverseIterator< Val, Cmp >& from) const;

    /// move to the next element in the tree
    /** @warning Applying this operator to an iterator equal to the rend iterator
     * has no effect. In particular, it does not raise any exception. */
    AVLTreeReverseIterator< Val, Cmp >& operator++() noexcept;

    /// move to the next k element
    /** @warning Applying this operator to an iterator equal to the rend iterator
     * has no effect. In particular, it does not raise any exception. */
    AVLTreeReverseIterator< Val, Cmp >& operator+=(const Size k) noexcept;

    /// move to the preceding element in the tree
    /** @warning Applying this operator to an iterator equal to the rbegin iterator
     * has no effect. In particular, it does not raise any exception. */
    AVLTreeReverseIterator< Val, Cmp >& operator--() noexcept;

    /// move to the preceding k element
    /** @warning Applying this operator to an iterator equal to the rbegin iterator
     * has no effect. In particular, it does not raise any exception. */
    AVLTreeReverseIterator< Val, Cmp >& operator-=(const Size k) noexcept;

    /** @brief returns the element pointed to by the iterator
     *
     * @return the element pointed to by the iterator, if the iterator actually points
     * to an element
     * @throws NotFound is raised if the iterator does not point to any element
     */
    using AVLTreeIterator< Val, Cmp >::operator*;

    /// @}


    protected:
    /// allow AVL trees to access the content of the iterators
    friend AVLTree< Val, Cmp >;
  };


  /**
   * @class AVLTreeReverseIteratorSafe
   * @headerfile AVLTree.h <agrum/tools/core/AVLTree.h>
   * @ingroup basicstruct_group
   * @brief AVL binary search tree safe (w.r.t. erasures) reverse iterator
   *
   * @tparam Val The type of the elements stored into the tree.
   * @tparam Cmp The function used for sorting the elements.
   * queues.
   */
  template < typename Val, typename Cmp = std::less< Val > >
  class AVLTreeReverseIteratorSafe: protected AVLTreeIteratorSafe< Val, Cmp > {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = Val;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    /// @}


    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /** @brief constructor for rbegin safe iterators
     *
     * @param tree the AVL tree which parses the iterator
     * @param begin if true, the iterator points to the highest value in the tree,
     * else it points to the lowest value
     */
    explicit AVLTreeReverseIteratorSafe(AVLTree< Val, Cmp >& tree, const bool rbegin = true);

#ifndef DOXYGEN_SHOULD_SKIP_THIS \
    // constructor for the static rendSafe iterator
    // only AVLTree.cpp should use this constructor
    explicit consteval AVLTreeReverseIteratorSafe(StaticInitializer init) noexcept :
        AVLTreeIteratorSafe< Val, Cmp >(init) {}
#endif  // DOXYGEN_SHOULD_SKIP_THIS

    /// copy constructor
    AVLTreeReverseIteratorSafe(const AVLTreeReverseIteratorSafe< Val, Cmp >& from);

    /// move constructor
    AVLTreeReverseIteratorSafe(AVLTreeReverseIteratorSafe< Val, Cmp >&& from);

    /// destructor
    ~AVLTreeReverseIteratorSafe() noexcept;

    /// @}

    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /// copy operator
    AVLTreeReverseIteratorSafe< Val, Cmp >&
       operator=(const AVLTreeReverseIteratorSafe< Val, Cmp >& from);

    /// move operator
    AVLTreeReverseIteratorSafe< Val, Cmp >&
       operator=(AVLTreeReverseIteratorSafe< Val, Cmp >&& from);

    /// indicates whether two iterator point to the same element
    bool operator==(const AVLTreeReverseIteratorSafe< Val, Cmp >& from) const;

    /// indicates whether two iterator point to different element
    bool operator!=(const AVLTreeReverseIteratorSafe< Val, Cmp >& from) const;

    /// move to the next element in the tree
    /** @warning Applying this operator to an iterator equal to the rend iterator
     * has no effect. In particular, it does not raise any exception. */
    AVLTreeReverseIteratorSafe< Val, Cmp >& operator++() noexcept;

    /// move to the next k element
    /** @warning Applying this operator to an iterator equal to the rend iterator
     * has no effect. In particular, it does not raise any exception. */
    AVLTreeReverseIteratorSafe< Val, Cmp >& operator+=(const Size k) noexcept;

    /// move to the preceding element in the tree
    /** @warning Applying this operator to an iterator equal to the rbegin iterator
     * has no effect. In particular, it does not raise any exception. */
    AVLTreeReverseIteratorSafe< Val, Cmp >& operator--() noexcept;

    /// move to the preceding k element
    /** @warning Applying this operator to an iterator equal to the rbegin iterator
     * has no effect. In particular, it does not raise any exception. */
    AVLTreeReverseIteratorSafe< Val, Cmp >& operator-=(const Size k) noexcept;

    /** @brief returns the element pointed to by the iterator
     *
     * @return the element pointed to by the iterator, if the iterator actually points
     * to an element
     * @throws NotFound is raised if the iterator does not point to any element
     */
    using AVLTreeIteratorSafe< Val, Cmp >::operator*;

    /// @}

    protected:
    /// allow AVL trees to access the content of the iterators
    friend AVLTree< Val, Cmp >;
  };


#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // _static_AVLTree_end_ is a 'constant' iterator initialized at compile time
  // that represents the end iterators for all AVL trees (whatever their
  // type). This global variable avoids creating the same iterators within every
  // AVL tree instance (this would be quite inefficient as end is precisely
  // identical for all AVL trees). The same hold for reverse and safe end iterators.
  // The type of _AVLTree_end_ is a pointer to void because C++ allows
  // pointers to void to be cast into pointers to other types (and conversely).
  // This avoids the painful strict-aliasing rule warning
  extern constinit const AVLTreeIterator< int, std::less< int > >        _static_AVLTree_end_;
  extern constinit const AVLTreeReverseIterator< int, std::less< int > > _static_AVLTree_rend_;
  extern constinit const AVLTreeIteratorSafe< int, std::less< int > >    _static_AVLTree_end_safe_;
  extern constinit const AVLTreeReverseIteratorSafe< int, std::less< int > >
     _static_AVLTree_rend_safe_;

  inline constexpr void* const _AVLTree_end_       = (void* const)&_static_AVLTree_end_;
  inline constexpr void* const _AVLTree_rend_      = (void* const)&_static_AVLTree_rend_;
  inline constexpr void* const _AVLTree_end_safe_  = (void* const)&_static_AVLTree_end_safe_;
  inline constexpr void* const _AVLTree_rend_safe_ = (void* const)&_static_AVLTree_rend_safe_;
#endif  // DOXYGEN_SHOULD_SKIP_THIS


}   // namespace gum


// always include the implementation of the templates
#include <agrum/tools/core/AVLTree_tpl.h>

#endif   // GUM_AVL_TREE_H
