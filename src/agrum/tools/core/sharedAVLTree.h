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
 * @brief AVL binary search trees that do not possess their own nodes
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */

#ifndef GUM_SHARED_AVL_TREE_H
#define GUM_SHARED_AVL_TREE_H

#include <agrum/agrum.h>
#include <agrum/tools/core/AVLTree.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  template < typename Val, typename Cmp >
  class SharedAVLTreeIterator;
  template < typename Val, typename Cmp >
  class SharedAVLTreeIteratorSafe;

  template < typename Val, typename Cmp >
  class SharedAVLTreeReverseIterator;
  template < typename Val, typename Cmp >
  class SharedAVLTreeReverseIteratorSafe;

  /**
   * @class SharedAVLTree
   * @headerfile sharedAVLTree.h <agrum/tools/core/sharedAVLTree.h>
   * @ingroup basicstruct_group
   * @brief AVL binary search tree that does not possess its own nodes
   *
   * @tparam Val The type of the elements stored into the tree.
   * @tparam Cmp The function used for sorting the elements.
   * queues.
   */
  template < typename Val, typename Cmp = std::less< Val > >
  class SharedAVLTree: private AVLTree< Val, Cmp > {
    public:
    /// Types for STL compliance.
    /// @{
    using value_type            = Val;
    using reference             = Val&;
    using const_reference       = const Val&;
    using pointer               = Val*;
    using const_pointer         = const Val*;
    using iterator              = SharedAVLTreeIterator< Val, Cmp >;
    using iterator_safe         = SharedAVLTreeIteratorSafe< Val, Cmp >;
    using reverse_iterator      = SharedAVLTreeReverseIterator< Val, Cmp >;
    using reverse_iterator_safe = SharedAVLTreeReverseIteratorSafe< Val, Cmp >;
    using AVLNode               = AVLTreeNode< Val >;
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
    explicit SharedAVLTree(const Cmp& compare = Cmp());

    /**
     * @brief Move constructor.
     * @param from The gum::sharedAVLTree to move.
     */
    SharedAVLTree(SharedAVLTree< Val, Cmp >&& from) noexcept;

    /**
     * @brief Class destructor.
     */
    ~SharedAVLTree();

    /// @}

    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Move operator.
     *
     * @param from The gum::SharedAVLTree to move.
     * @return Returns this gum::SharedAVLTree.
     */
    SharedAVLTree< Val, Cmp >& operator=(SharedAVLTree< Val, Cmp >&& from);

    /// @}

    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Returns the number of elements in the tree.
     * @return Returns the number of elements in the tree.
     */
    using AVLTree< Val, Cmp >::size;

    /**
     * @brief Indicates whether the tree is empty.
     * @return Returns true if the tree is empty.
     */
    using AVLTree< Val, Cmp >::empty;

    /**
     * @brief Indicates whether the tree contains a given value.
     * @param val The value to look for.
     * @return Returns true if val is in the tree.
     */
    using AVLTree< Val, Cmp >::contains;

    /**
     * @brief Alias of contains: indicates whether the tree contains a given value.
     * @param val The value to look for.
     * @return Returns true if val is in the tree.
     */
    using AVLTree< Val, Cmp >::exists;

    /// returns the node containing the max element (w.r.t. Cmp) in the tree
    /** @warning If the tree is empty, nullptr is returned */
    AVLNode* highestNode() const noexcept;

    /// returns the max element (w.r.t. Cmp) in the tree
    /** @throw NotFound Raised if the tree is empty */
    using AVLTree< Val, Cmp >::highestValue;

    /// returns the node containing the min element (w.r.t. Cmp) in the tree
    /** @warning If the tree is empty, nullptr is returned */
    AVLNode* lowestNode() const noexcept;

    /// returns the min element (w.r.t. Cmp) in the tree
    /** @throw NotFound Raised if the tree is empty */
    using AVLTree< Val, Cmp >::lowestValue;

    /// adds a new node into the tree
    void insert(AVLNode* node);

    /// remove an element from the tree
    /** Here, we assume (but not test) that the node actually belongs to the tree */
    void erase(AVLNode* node);

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
    using AVLTree< Val, Cmp >::clear;

    /** @brief returns a string with the content of the tree, order from the lowest
     * to the highest element
     */
    using AVLTree< Val, Cmp >::toString;

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


    private:
    /// prevent copy/moves
    SharedAVLTree(const SharedAVLTree< Val, Cmp >& from)                        = delete;
    SharedAVLTree< Val, Cmp >& operator=(const SharedAVLTree< Val, Cmp >& from) = delete;


    /// make the iterators access to the AVLNodes
    friend iterator;
    friend reverse_iterator;
    friend iterator_safe;
    friend reverse_iterator_safe;
  };


  /**
   * @class SharedAVLTreeIterator
   * @headerfile sharedAVLTree.h <agrum/tools/core/sharedAVLTree.h>
   * @ingroup basicstruct_group
   * @brief AVL binary search tree iterator
   *
   * @tparam Val The type of the elements stored into the tree.
   * @tparam Cmp The function used for sorting the elements.
   * queues.
   */
  template < typename Val, typename Cmp = std::less< Val > >
  class SharedAVLTreeIterator: protected AVLTreeIterator< Val, Cmp > {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using AVLNode           = AVLTreeNode< Val >;
    using value_type        = AVLNode;
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
    explicit SharedAVLTreeIterator(const SharedAVLTree< Val, Cmp >& tree,
                                   const bool                       begin = true) noexcept;

#  ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static end iterator
    // only AVLTree.cpp should use this constructor
    explicit consteval SharedAVLTreeIterator(StaticInitializer init) noexcept :
        AVLTreeIterator< Val, Cmp >(init) {}
#  endif   // DOXYGEN_SHOULD_SKIP_THIS

    /// copy constructor
    SharedAVLTreeIterator(const SharedAVLTreeIterator< Val, Cmp >& from) noexcept;

    /// move constructor
    SharedAVLTreeIterator(SharedAVLTreeIterator< Val, Cmp >&& from) noexcept;

    /// destructor
    ~SharedAVLTreeIterator() noexcept;

    /// @}


    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /// copy operator
    SharedAVLTreeIterator< Val, Cmp >&
       operator=(const SharedAVLTreeIterator< Val, Cmp >& from) noexcept;

    /// move operator
    SharedAVLTreeIterator< Val, Cmp >& operator=(SharedAVLTreeIterator< Val, Cmp >&& from) noexcept;

    /// indicates whether two iterator point to the same element
    bool operator==(const SharedAVLTreeIterator< Val, Cmp >& from) const;

    /// indicates whether two iterator point to different element
    bool operator!=(const SharedAVLTreeIterator< Val, Cmp >& from) const;

    /// move to the next element in the tree
    /** @warning Applying this operator to an iterator equal to the end iterator
     * has no effect. In particular, it does not raise any exception. */
    SharedAVLTreeIterator< Val, Cmp >& operator++() noexcept;

    /// move to the next k element
    /** @warning Applying this operator to an iterator equal to the end iterator
     * has no effect. In particular, it does not raise any exception. */
    SharedAVLTreeIterator< Val, Cmp >& operator+=(const Size k) noexcept;

    /// move to the preceding element in the tree
    /** @warning Applying this operator to an iterator equal to the begin iterator
     * has no effect. In particular, it does not raise any exception. */
    SharedAVLTreeIterator< Val, Cmp >& operator--() noexcept;

    /// move to the preceding k element
    /** @warning Applying this operator to an iterator equal to the begin iterator
     * has no effect. In particular, it does not raise any exception. */
    SharedAVLTreeIterator< Val, Cmp >& operator-=(const Size k) noexcept;

    /** @brief returns the node pointed to by the iterator
     *
     * @return the node pointed to by the iterator, if the iterator actually points
     * to an element
     * @throws NotFound is raised if the iterator points to a nullptr node
     */
    const_reference operator*() const;

    /// returns a pointer on the node pointed to by the iterator
    const_pointer operator->() const;

    /// @}

    /// allow AVL trees to access the content of the iterators
    friend AVLTree< Val, Cmp >;
    friend SharedAVLTree< Val, Cmp >;
  };


  /**
   * @class SharedAVLTreeIteratorSafe
   * @headerfile sharedAVLTree.h <agrum/tools/core/sharedAVLTree.h>
   * @ingroup basicstruct_group
   * @brief AVL binary search tree safe (w.r.t. erasures) iterator
   *
   * @tparam Val The type of the elements stored into the tree.
   * @tparam Cmp The function used for sorting the elements.
   * queues.
   */
  template < typename Val, typename Cmp = std::less< Val > >
  class SharedAVLTreeIteratorSafe: protected AVLTreeIteratorSafe< Val, Cmp > {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using AVLNode           = AVLTreeNode< Val >;
    using value_type        = AVLNode;
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
    explicit SharedAVLTreeIteratorSafe(SharedAVLTree< Val, Cmp >& tree, const bool rbegin = true);

#  ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static endSafe iterator
    // only AVLTree.cpp should use this constructor
    explicit consteval SharedAVLTreeIteratorSafe(StaticInitializer init) noexcept :
        AVLTreeIteratorSafe< Val, Cmp >(init) {}
#  endif   // DOXYGEN_SHOULD_SKIP_THIS

    /// copy constructor
    SharedAVLTreeIteratorSafe(const SharedAVLTreeIteratorSafe< Val, Cmp >& from);

    /// move constructor
    SharedAVLTreeIteratorSafe(SharedAVLTreeIteratorSafe< Val, Cmp >&& from);

    /// destructor
    ~SharedAVLTreeIteratorSafe() noexcept;

    /// @}

    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /// copy operator
    SharedAVLTreeIteratorSafe< Val, Cmp >&
       operator=(const SharedAVLTreeIteratorSafe< Val, Cmp >& from);

    /// move operator
    SharedAVLTreeIteratorSafe< Val, Cmp >& operator=(SharedAVLTreeIteratorSafe< Val, Cmp >&& from);

    /// indicates whether two iterator point to the same element
    bool operator==(const SharedAVLTreeIteratorSafe< Val, Cmp >& from) const;

    /// indicates whether two iterator point to different element
    bool operator!=(const SharedAVLTreeIteratorSafe< Val, Cmp >& from) const;

    /// move to the next element in the tree
    /** @warning Applying this operator to an iterator equal to the end iterator
     * has no effect. In particular, it does not raise any exception. */
    SharedAVLTreeIteratorSafe< Val, Cmp >& operator++() noexcept;

    /// move to the next k elements
    /** @warning Applying this operator to an iterator equal to the end iterator
     * has no effect. In particular, it does not raise any exception. */
    SharedAVLTreeIteratorSafe< Val, Cmp >& operator+=(const Size k) noexcept;

    /// move to the preceding element in the tree
    /** @warning Applying this operator to an iterator equal to the begin iterator
     * has no effect. In particular, it does not raise any exception. */
    SharedAVLTreeIteratorSafe< Val, Cmp >& operator--() noexcept;

    /// move to the preceding k element
    /** @warning Applying this operator to an iterator equal to the begin iterator
     * has no effect. In particular, it does not raise any exception. */
    SharedAVLTreeIteratorSafe< Val, Cmp >& operator-=(const Size k) noexcept;

    /** @brief returns the node pointed to by the iterator
     *
     * @return the node pointed to by the iterator, if the iterator actually points
     * to an element
     * @throws NotFound is raised if the iterator points to a nullptr node
     */
    const_reference operator*() const;

    /// returns a pointer on the node pointed to by the iterator
    const_pointer operator->() const;

    /// @}


    protected:
    /// allow AVL trees to access the content of the iterators
    friend AVLTree< Val, Cmp >;
    friend SharedAVLTree< Val, Cmp >;
  };


  /**
   * @class SharedAVLTreeReverseIterator
   * @headerfile sharedAVLTree.h <agrum/tools/core/sharedAVLTree.h>
   * @ingroup basicstruct_group
   * @brief AVL binary search tree reverse iterator
   *
   * @tparam Val The type of the elements stored into the tree.
   * @tparam Cmp The function used for sorting the elements.
   * queues.
   */
  template < typename Val, typename Cmp = std::less< Val > >
  class SharedAVLTreeReverseIterator: protected SharedAVLTreeIterator< Val, Cmp > {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using AVLNode           = AVLTreeNode< Val >;
    using value_type        = AVLNode;
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
    explicit SharedAVLTreeReverseIterator(const SharedAVLTree< Val, Cmp >& tree,
                                          const bool                       rbegin = true) noexcept;

#  ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static rend iterator
    // only AVLTree.cpp should use this constructor
    explicit consteval SharedAVLTreeReverseIterator(StaticInitializer init) noexcept :
        SharedAVLTreeIterator< Val, Cmp >(init) {}
#  endif   // DOXYGEN_SHOULD_SKIP_THIS

    /// copy constructor
    SharedAVLTreeReverseIterator(const SharedAVLTreeReverseIterator< Val, Cmp >& from) noexcept;

    /// move constructor
    SharedAVLTreeReverseIterator(SharedAVLTreeReverseIterator< Val, Cmp >&& from) noexcept;

    /// destructor
    ~SharedAVLTreeReverseIterator() noexcept;

    /// @}

    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /// copy operator
    SharedAVLTreeReverseIterator< Val, Cmp >&
       operator=(const SharedAVLTreeReverseIterator< Val, Cmp >& from) noexcept;

    /// move operator
    SharedAVLTreeReverseIterator< Val, Cmp >&
       operator=(SharedAVLTreeReverseIterator< Val, Cmp >&& from) noexcept;

    /// indicates whether two iterator point to the same element
    bool operator==(const SharedAVLTreeReverseIterator< Val, Cmp >& from) const;

    /// indicates whether two iterator point to different element
    bool operator!=(const SharedAVLTreeReverseIterator< Val, Cmp >& from) const;

    /// move to the next element in the tree
    /** @warning Applying this operator to an iterator equal to the rend iterator
     * has no effect. In particular, it does not raise any exception. */
    SharedAVLTreeReverseIterator< Val, Cmp >& operator++() noexcept;

    /// move to the next k element
    /** @warning Applying this operator to an iterator equal to the rend iterator
     * has no effect. In particular, it does not raise any exception. */
    SharedAVLTreeReverseIterator< Val, Cmp >& operator+=(const Size k) noexcept;

    /// move to the preceding element in the tree
    /** @warning Applying this operator to an iterator equal to the rbegin iterator
     * has no effect. In particular, it does not raise any exception. */
    SharedAVLTreeReverseIterator< Val, Cmp >& operator--() noexcept;

    /// move to the preceding k element
    /** @warning Applying this operator to an iterator equal to the rbegin iterator
     * has no effect. In particular, it does not raise any exception. */
    SharedAVLTreeReverseIterator< Val, Cmp >& operator-=(const Size k) noexcept;

    /** @brief returns the node pointed to by the iterator
     *
     * @return the node pointed to by the iterator, if the iterator actually points
     * to an element
     * @throws NotFound is raised if the iterator points to a nullptr node
     */
    using SharedAVLTreeIterator< Val, Cmp >::operator*;

    /// returns a pointer on the node pointed to by the iterator
    using SharedAVLTreeIterator< Val, Cmp >::operator->;

    /// @}


    protected:
    /// allow AVL trees to access the content of the iterators
    friend AVLTree< Val, Cmp >;
    friend SharedAVLTree< Val, Cmp >;
  };


  /**
   * @class SharedAVLTreeReverseIteratorSafe
   * @headerfile AVLTree.h <agrum/tools/core/AVLTree.h>
   * @ingroup basicstruct_group
   * @brief AVL binary search tree safe (w.r.t. erasures) reverse iterator
   *
   * @tparam Val The type of the elements stored into the tree.
   * @tparam Cmp The function used for sorting the elements.
   * queues.
   */
  template < typename Val, typename Cmp = std::less< Val > >
  class SharedAVLTreeReverseIteratorSafe: protected SharedAVLTreeIteratorSafe< Val, Cmp > {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using AVLNode           = AVLTreeNode< Val >;
    using value_type        = AVLNode;
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
    explicit SharedAVLTreeReverseIteratorSafe(SharedAVLTree< Val, Cmp >& tree,
                                              const bool                 rbegin = true);

#  ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static rendSafe iterator
    // only AVLTree.cpp should use this constructor
    explicit consteval SharedAVLTreeReverseIteratorSafe(StaticInitializer init) noexcept :
        SharedAVLTreeIteratorSafe< Val, Cmp >(init) {}
#  endif   // DOXYGEN_SHOULD_SKIP_THIS

    /// copy constructor
    SharedAVLTreeReverseIteratorSafe(const SharedAVLTreeReverseIteratorSafe< Val, Cmp >& from);

    /// move constructor
    SharedAVLTreeReverseIteratorSafe(SharedAVLTreeReverseIteratorSafe< Val, Cmp >&& from);

    /// destructor
    ~SharedAVLTreeReverseIteratorSafe() noexcept;

    /// @}

    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /// copy operator
    SharedAVLTreeReverseIteratorSafe< Val, Cmp >&
       operator=(const SharedAVLTreeReverseIteratorSafe< Val, Cmp >& from);

    /// move operator
    SharedAVLTreeReverseIteratorSafe< Val, Cmp >&
       operator=(SharedAVLTreeReverseIteratorSafe< Val, Cmp >&& from);

    /// indicates whether two iterator point to the same element
    bool operator==(const SharedAVLTreeReverseIteratorSafe< Val, Cmp >& from) const;

    /// indicates whether two iterator point to different element
    bool operator!=(const SharedAVLTreeReverseIteratorSafe< Val, Cmp >& from) const;

    /// move to the next element in the tree
    /** @warning Applying this operator to an iterator equal to the rend iterator
     * has no effect. In particular, it does not raise any exception. */
    SharedAVLTreeReverseIteratorSafe< Val, Cmp >& operator++() noexcept;

    /// move to the next k element
    /** @warning Applying this operator to an iterator equal to the rend iterator
     * has no effect. In particular, it does not raise any exception. */
    SharedAVLTreeReverseIteratorSafe< Val, Cmp >& operator+=(const Size k) noexcept;

    /// move to the preceding element in the tree
    /** @warning Applying this operator to an iterator equal to the rbegin iterator
     * has no effect. In particular, it does not raise any exception. */
    SharedAVLTreeReverseIteratorSafe< Val, Cmp >& operator--() noexcept;

    /// move to the preceding k element
    /** @warning Applying this operator to an iterator equal to the rbegin iterator
     * has no effect. In particular, it does not raise any exception. */
    SharedAVLTreeReverseIteratorSafe< Val, Cmp >& operator-=(const Size k) noexcept;

    /** @brief returns the node pointed to by the iterator
     *
     * @return the node pointed to by the iterator, if the iterator actually points
     * to an element
     * @throws NotFound is raised if the iterator points to a nullptr node
     */
    using SharedAVLTreeIteratorSafe< Val, Cmp >::operator*;

    /// returns a pointer on the node pointed to by the iterator
    using SharedAVLTreeIteratorSafe< Val, Cmp >::operator->;

    /// @}

    protected:
    /// allow AVL trees to access the content of the iterators
    friend AVLTree< Val, Cmp >;
    friend SharedAVLTree< Val, Cmp >;
  };


  /// display the content of a tree
  template < typename Val, typename Cmp >
  std::ostream& operator<<(std::ostream& stream, const SharedAVLTree< Val, Cmp >& tree) {
    return stream << tree.toString();
  }


#  ifndef DOXYGEN_SHOULD_SKIP_THIS
  // _static_SharedAVLTree_end_ is a 'constant' iterator initialized at compile time
  // that represents the end iterators for all shared AVL trees (whatever their
  // type). This global variable avoids creating the same iterators within every
  // shared AVL tree instance (this would be quite inefficient as end is precisely
  // identical for all AVL trees). The same hold for reverse and safe end iterators.
  // The type of _SharedAVLTree_end_ is a pointer to void because C++ allows
  // pointers to void to be cast into pointers to other types (and conversely).
  // This avoids the painful strict-aliasing rule warning
  extern const SharedAVLTreeIterator< int, std::less< int > >        _static_SharedAVLTree_end_;
  extern const SharedAVLTreeReverseIterator< int, std::less< int > > _static_SharedAVLTree_rend_;
  extern const SharedAVLTreeIteratorSafe< int, std::less< int > > _static_SharedAVLTree_end_safe_;
  extern const SharedAVLTreeReverseIteratorSafe< int, std::less< int > >
     _static_SharedAVLTree_rend_safe_;

  inline constexpr void* const _SharedAVLTree_end_  = (void* const)&_static_SharedAVLTree_end_;
  inline constexpr void* const _SharedAVLTree_rend_ = (void* const)&_static_SharedAVLTree_rend_;
  inline constexpr void* const _SharedAVLTree_end_safe_
     = (void* const)&_static_SharedAVLTree_end_safe_;
  inline constexpr void* const _SharedAVLTree_rend_safe_
     = (void* const)&_static_SharedAVLTree_rend_safe_;
#  endif   // DOXYGEN_SHOULD_SKIP_THIS


}   // namespace gum

// always include the implementation of the templates
#  include <agrum/tools/core/sharedAVLTree_tpl.h>

#endif   // DOXYGEN_SHOULD_SKIP_THIS

#endif   // GUM_SHARED_AVL_TREE_H
