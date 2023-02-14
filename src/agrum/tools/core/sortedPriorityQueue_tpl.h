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

#include <agrum/tools/core/sortedPriorityQueue.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  // basic constructor
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueue< Val, Priority, Cmp >::SortedPriorityQueue(Cmp  compare,
                                                                        Size capacity) :
      _nodes_(capacity, true, true),
      _tree_cmp_(compare) {
    GUM_CONSTRUCTOR(SortedPriorityQueue);
  }

  // initializer list constructor
  template < typename Val, typename Priority, typename Cmp >
  SortedPriorityQueue< Val, Priority, Cmp >::SortedPriorityQueue(
     std::initializer_list< std::pair< Val, Priority > > list) :
      _nodes_(Size(list.size()) / 2, true, true) {
    // fill the queue
    for (const auto& elt: list) {
      insert(elt.first, elt.second);
    }

    GUM_CONSTRUCTOR(SortedPriorityQueue);
  }

  // copy constructor
  template < typename Val, typename Priority, typename Cmp >
  SortedPriorityQueue< Val, Priority, Cmp >::SortedPriorityQueue(
     const SortedPriorityQueue< Val, Priority, Cmp >& from) :
      _nodes_(from._nodes_),
      _tree_cmp_(from._tree_cmp_) {
    // fill the heap structure
    for (const auto& node_prio: _nodes_) {
      _tree_.insert(&node_prio.first);
    }

    GUM_CONS_CPY(SortedPriorityQueue);
  }

  // move constructor
  template < typename Val, typename Priority, typename Cmp >
  SortedPriorityQueue< Val, Priority, Cmp >::SortedPriorityQueue(
     SortedPriorityQueue< Val, Priority, Cmp >&& from) noexcept :
      _tree_(std::move(from._tree_)),
      _nodes_(std::move(from._nodes_)), _tree_cmp_(std::move(from._tree_cmp_)) {
    GUM_CONS_MOV(SortedPriorityQueue)
  }

  // destructor
  template < typename Val, typename Priority, typename Cmp >
  SortedPriorityQueue< Val, Priority, Cmp >::~SortedPriorityQueue() {
    GUM_DESTRUCTOR(SortedPriorityQueue);
  }

  // copy operator
  template < typename Val, typename Priority, typename Cmp >
  SortedPriorityQueue< Val, Priority, Cmp >& SortedPriorityQueue< Val, Priority, Cmp >::operator=(
     const SortedPriorityQueue< Val, Priority, Cmp >& from) {
    // avoid self assignment
    if (this != &from) {
      GUM_OP_CPY(SortedPriorityQueue)

      try {
        // set the comparison function
        _tree_cmp_ = from._tree_cmp_;

        // copy the nodes within the hash table
        _nodes_ = from._nodes_;

        // fill the AVL tree
        for (const auto& node_prio: _nodes_)
          _tree_.insert(&node_prio.first);
      } catch (...) {
        _tree_.clear();
        _nodes_.clear();
        throw;
      }
    }

    return *this;
  }

  // move operator
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueue< Val, Priority, Cmp >&
     SortedPriorityQueue< Val, Priority, Cmp >::operator=(
        SortedPriorityQueue< Val, Priority, Cmp >&& from) noexcept {
    // avoid self assignment
    if (this != &from) {
      GUM_OP_MOV(SortedPriorityQueue)

      _nodes_    = std::move(from._nodes_);
      _tree_     = std::move(from._tree_);
      _tree_cmp_ = std::move(from._tree_cmp_);
    }

    return *this;
  }

  // returns the number of elements in the priority queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE Size SortedPriorityQueue< Val, Priority, Cmp >::size() const noexcept {
    return _tree_.size();
  }

  // indicates whether the priority queue is empty
  template < typename Val, typename Priority, typename Cmp >
  INLINE bool SortedPriorityQueue< Val, Priority, Cmp >::empty() const noexcept {
    return (_tree_.empty());
  }

  // indicates whether the priority queue contains a given value
  template < typename Val, typename Priority, typename Cmp >
  INLINE bool SortedPriorityQueue< Val, Priority, Cmp >::contains(const Val& val) const noexcept {
    if constexpr (std::is_scalar_v< Val >) {
      return _nodes_.exists(AVLTreeNode< Val >(val));
    } else {
      AVLTreeNode< Val > xval(std::move(const_cast< Val& >(val)));
      bool               res  = _nodes_.exists(xval);
      const_cast< Val& >(val) = std::move(xval.value);
      return res;
    }
  }

  // returns the element at the top of the priority queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE const Val& SortedPriorityQueue< Val, Priority, Cmp >::top() const {
    if (_tree_.empty()) { GUM_ERROR(NotFound, "An empty sorted priority queue has no top element") }

    return _tree_.highestNode()->value;
  }

  // returns the element at the bottom of the priority queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE const Val& SortedPriorityQueue< Val, Priority, Cmp >::bottom() const {
    if (_tree_.empty()) {
      GUM_ERROR(NotFound, "An empty sorted priority queue has no bottom element")
    }

    return _tree_.lowestNode()->value;
  }

  // returns the priority of the top element
  template < typename Val, typename Priority, typename Cmp >
  INLINE const Priority& SortedPriorityQueue< Val, Priority, Cmp >::topPriority() const {
    if (_tree_.empty()) { GUM_ERROR(NotFound, "An empty priority queue has no top priority") }

    return _tree_cmp_.getPriority(_tree_.highestNode()->value);
  }

  // returns the priority of the top element
  template < typename Val, typename Priority, typename Cmp >
  INLINE const Priority& SortedPriorityQueue< Val, Priority, Cmp >::bottomPriority() const {
    if (_tree_.empty()) { GUM_ERROR(NotFound, "An empty priority queue has no bottom priority") }

    return _tree_cmp_.getPriority(_tree_.lowestNode()->value);
  }

  // removes the top element from the priority queue and return it
  template < typename Val, typename Priority, typename Cmp >
  INLINE Val SortedPriorityQueue< Val, Priority, Cmp >::popTop() {
    if (_tree_.empty()) { GUM_ERROR(NotFound, "An empty sorted priority queue has no top element") }

    // erase the node from the tree
    AVLNode* node = _tree_.highestNode();
    _tree_.erase(node);

    // erase the node from the hash table
    Val v = std::move(node->value);
    _nodes_.erase(*node);

    return v;
  }

  // removes the top element from the priority queue and return it
  template < typename Val, typename Priority, typename Cmp >
  INLINE Val SortedPriorityQueue< Val, Priority, Cmp >::pop() {
    return popTop();
  }

  // removes the bottom element from the priority queue and return it
  template < typename Val, typename Priority, typename Cmp >
  INLINE Val SortedPriorityQueue< Val, Priority, Cmp >::popBottom() {
    if (_tree_.empty()) {
      GUM_ERROR(NotFound, "An empty sorted priority queue has no bottom element")
    }

    // erase the node from the tree
    AVLNode* node = _tree_.lowestNode();
    _tree_.erase(node);

    // erase the node from the hash table
    Val v = std::move(node->value);
    _nodes_.erase(*node);

    return v;
  }

  // inserts a new (a copy) element in the priority queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE typename SortedPriorityQueue< Val, Priority, Cmp >::const_reference
     SortedPriorityQueue< Val, Priority, Cmp >::insert(const Val& val, const Priority& priority) {
    // create the entry in the _nodes_ hashtable (if the element already exists,
    // _nodes_.insert will raise a DuplicateElement exception)
    Priority    new_priority(priority);
    const auto& new_elt = _nodes_.insert(AVLNode(val), std::move(new_priority));

    // update the tree
    _tree_.insert(const_cast< AVLTreeNode< Val >* >(&new_elt.first));

    return new_elt.first.value;
  }


  // inserts by move a new element in the priority queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE typename SortedPriorityQueue< Val, Priority, Cmp >::const_reference
     SortedPriorityQueue< Val, Priority, Cmp >::insert(Val&& val, Priority&& priority) {
    // create the entry in the indices hashtable (if the element already exists,
    // _nodes_.insert will raise a DuplicateElement exception)
    const auto& new_elt = _nodes_.insert(AVLNode(std::move(val)), std::move(priority));

    // update the tree
    _tree_.insert(const_cast< AVLTreeNode< Val >* >(&new_elt.first));

    return new_elt.first.value;
  }


  // emplace a new element into the priority queue
  template < typename Val, typename Priority, typename Cmp >
  template < typename... Args >
  INLINE typename SortedPriorityQueue< Val, Priority, Cmp >::const_reference
     SortedPriorityQueue< Val, Priority, Cmp >::emplace(Args&&... args) {
    auto new_elt = std::make_pair< Val, Priority >(std::forward< Args >(args)...);
    return insert(std::move(new_elt.first), std::move(new_elt.second));
  }


  // removes the top of the priority queue (but does not return it)
  template < typename Val, typename Priority, typename Cmp >
  INLINE void SortedPriorityQueue< Val, Priority, Cmp >::eraseTop() {
    if (_tree_.empty()) return;
    AVLNode* node = _tree_.highestNode();
    _tree_.erase(node);
    _nodes_.erase(*node);
  }


  // removes the bottom of the priority queue (but does not return it)
  template < typename Val, typename Priority, typename Cmp >
  INLINE void SortedPriorityQueue< Val, Priority, Cmp >::eraseBottom() {
    if (_tree_.empty()) return;
    AVLNode* node = _tree_.lowestNode();
    _tree_.erase(node);
    _nodes_.erase(*node);
  }

  // returns the node in the hash table corresponding to a given value
  template < typename Val, typename Priority, typename Cmp >
  INLINE AVLTreeNode< Val >&
         SortedPriorityQueue< Val, Priority, Cmp >::getNode_(const Val& val) const {
    // Some compilers are optimizing _tree_cmp_.getNode(val) for Val=string. This
    // induces a lot of warnings. To prevent this, we have an optimized code for
    // non-strings and a less optimal but warning-free
    if constexpr (!is_basic_string< Val >::value)
      return const_cast< AVLTreeNode< Val >& >(_nodes_.key(*(_tree_cmp_.getNode(val))));
    else {
      AVLTreeNode< Val > xval(std::move(const_cast< Val& >(val)));
      try {
        auto& node              = const_cast< AVLTreeNode< Val >& >(_nodes_.key(xval));
        const_cast< Val& >(val) = std::move(xval.value);
        return node;
      } catch (NotFound const&) {
        // restore into val the value that was moved
        const_cast< Val& >(val) = std::move(xval.value);
        throw;
      }
    }
  }


  // removes a given element from the priority queue (but does not return it)
  template < typename Val, typename Priority, typename Cmp >
  INLINE void SortedPriorityQueue< Val, Priority, Cmp >::erase(const Val& val) {
    try {
      AVLNode& node = getNode_(val);
      _tree_.erase(&node);
      _nodes_.erase(node);
    } catch (NotFound const&) {}
  }


  // modifies the priority of a given element
  template < typename Val, typename Priority, typename Cmp >
  INLINE void SortedPriorityQueue< Val, Priority, Cmp >::setPriority(const Val&      elt,
                                                                     const Priority& new_priority) {
    try {
      AVLNode& node = getNode_(elt);
      _tree_.erase(&node);
      _nodes_[node] = new_priority;
      _tree_.insert(&node);
    } catch (NotFound const&) {
      GUM_ERROR(NotFound,
                "The sorted priority queue does not contain"
                   << elt << ". Hence it is not possible to change its priority")
    }
  }


  // modifies the priority of a given element
  template < typename Val, typename Priority, typename Cmp >
  INLINE void SortedPriorityQueue< Val, Priority, Cmp >::setPriority(const Val& elt,
                                                                     Priority&& new_priority) {
    try {
      AVLNode& node = getNode_(elt);
      _tree_.erase(&node);
      _nodes_[node] = std::move(new_priority);
      _tree_.insert(&node);
    } catch (NotFound const&) {
      GUM_ERROR(NotFound,
                "The sorted priority queue does not contain"
                   << elt << ". Hence it is not possible to change its priority")
    }
  }


  // returns the priority of a given element
  template < typename Val, typename Priority, typename Cmp >
  INLINE const Priority& SortedPriorityQueue< Val, Priority, Cmp >::priority(const Val& elt) const {
    return _nodes_[getNode_(elt)];
  }


  // removes all the elements from the queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE void SortedPriorityQueue< Val, Priority, Cmp >::clear() {
    _tree_.clear();
    _nodes_.clear();
  }


  // displays the content of the queue
  template < typename Val, typename Priority, typename Cmp >
  std::string SortedPriorityQueue< Val, Priority, Cmp >::toString() const {
    bool              deja = false;
    std::stringstream stream;
    stream << "[";

    // parse the tree from the highest element to the lowest
    for (auto iter = _tree_.rbegin(); iter != _tree_.rend(); ++iter) {
      if (deja) stream << " ; ";
      else deja = true;

      stream << "(" << iter->value << ", " << _tree_cmp_.getPriority(iter->value) << ")";
    }

    stream << "]";

    return stream.str();
  }


  /// returns a new iterator pointing to the minimal element of the tree
  template < typename Val, typename Priority, typename Cmp >
  INLINE typename SortedPriorityQueue< Val, Priority, Cmp >::iterator
     SortedPriorityQueue< Val, Priority, Cmp >::begin() const {
    return iterator(*this);
  }

  /// returns an iterator pointing just after the maximal element
  template < typename Val, typename Priority, typename Cmp >
  INLINE constexpr const typename SortedPriorityQueue< Val, Priority, Cmp >::iterator&
     SortedPriorityQueue< Val, Priority, Cmp >::end() const {
    return *(reinterpret_cast< const iterator* >(_SortedPriorityQueue_end_));
  }

  /// returns a new iterator pointing to the maximal element of the tree
  template < typename Val, typename Priority, typename Cmp >
  INLINE typename SortedPriorityQueue< Val, Priority, Cmp >::reverse_iterator
     SortedPriorityQueue< Val, Priority, Cmp >::rbegin() const {
    return reverse_iterator(*this, true);
  }

  /// returns an iterator pointing just before the minimal element
  template < typename Val, typename Priority, typename Cmp >
  INLINE constexpr const typename SortedPriorityQueue< Val, Priority, Cmp >::reverse_iterator&
     SortedPriorityQueue< Val, Priority, Cmp >::rend() const {
    return *(reinterpret_cast< const reverse_iterator* >(_SortedPriorityQueue_rend_));
  }

  /// returns a new safe iterator pointing to the minimal element of the tree
  template < typename Val, typename Priority, typename Cmp >
  INLINE typename SortedPriorityQueue< Val, Priority, Cmp >::iterator_safe
     SortedPriorityQueue< Val, Priority, Cmp >::beginSafe() {
    return iterator_safe(*this);
  }

  /// returns a safe iterator pointing just after the maximal element
  template < typename Val, typename Priority, typename Cmp >
  INLINE constexpr const typename SortedPriorityQueue< Val, Priority, Cmp >::iterator_safe&
     SortedPriorityQueue< Val, Priority, Cmp >::endSafe() const {
    return *(reinterpret_cast< const iterator_safe* >(_SortedPriorityQueue_end_safe_));
  }

  /// returns a safe iterator pointing to the maximal element of the tree
  template < typename Val, typename Priority, typename Cmp >
  INLINE typename SortedPriorityQueue< Val, Priority, Cmp >::reverse_iterator_safe
     SortedPriorityQueue< Val, Priority, Cmp >::rbeginSafe() {
    return reverse_iterator_safe(*this, true);
  }

  /// returns a safe iterator pointing just before the minimal element
  template < typename Val, typename Priority, typename Cmp >
  INLINE constexpr const typename SortedPriorityQueue< Val, Priority, Cmp >::reverse_iterator_safe&
     SortedPriorityQueue< Val, Priority, Cmp >::rendSafe() const {
    return *(reinterpret_cast< const reverse_iterator_safe* >(_SortedPriorityQueue_rend_safe_));
  }


  // return the size of the array storing the priority queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE Size SortedPriorityQueue< Val, Priority, Cmp >::capacity() const noexcept {
    return Size(_nodes_.capacity());
  }


  // changes the size of the array storing the priority queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE void SortedPriorityQueue< Val, Priority, Cmp >::resize(Size new_size) {
    if (new_size < _tree_.size() / 2) return;
    _nodes_.resize(new_size);
  }

  /// ======================================================================================

  /// constructor for begin iterators
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIterator< Val, Priority, Cmp >::SortedPriorityQueueIterator(
     const SortedPriorityQueue< Val, Priority, Cmp >& queue,
     const bool                                       begin) noexcept :
      SharedAVLTreeReverseIterator< Val, TreeCmp >(queue._tree_, begin) {
    GUM_CONSTRUCTOR(SortedPriorityQueueIterator)
  }


  /// copy constructor
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIterator< Val, Priority, Cmp >::SortedPriorityQueueIterator(
     const SortedPriorityQueueIterator< Val, Priority, Cmp >& from) noexcept :
      SharedAVLTreeReverseIterator< Val, TreeCmp >(from) {
    GUM_CONS_CPY(SortedPriorityQueueIterator)
  }

  /// move constructor
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIterator< Val, Priority, Cmp >::SortedPriorityQueueIterator(
     SortedPriorityQueueIterator< Val, Priority, Cmp >&& from) noexcept :
      SharedAVLTreeReverseIterator< Val, TreeCmp >(std::move(from)) {
    GUM_CONS_MOV(SortedPriorityQueueIterator)
  }

  /// destructor
  template < typename Val, typename Priority, typename Cmp >
  INLINE
     SortedPriorityQueueIterator< Val, Priority, Cmp >::~SortedPriorityQueueIterator() noexcept {
    GUM_DESTRUCTOR(SortedPriorityQueueIterator)
  }

  /// copy operator
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIterator< Val, Priority, Cmp >&
     SortedPriorityQueueIterator< Val, Priority, Cmp >::operator=(
        const SortedPriorityQueueIterator< Val, Priority, Cmp >& from) noexcept {
    SharedAVLTreeReverseIterator< Val, TreeCmp >::operator=(from);
    return *this;
  }

  /// move operator
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIterator< Val, Priority, Cmp >&
     SortedPriorityQueueIterator< Val, Priority, Cmp >::operator=(
        SortedPriorityQueueIterator< Val, Priority, Cmp >&& from) noexcept {
    SharedAVLTreeReverseIterator< Val, TreeCmp >::operator=(std::move(from));
    return *this;
  }

  /// indicates whether two iterator point to the same element
  template < typename Val, typename Priority, typename Cmp >
  INLINE bool SortedPriorityQueueIterator< Val, Priority, Cmp >::operator==(
     const SortedPriorityQueueIterator< Val, Priority, Cmp >& from) const {
    return SharedAVLTreeReverseIterator< Val, TreeCmp >::operator==(from);
  }

  /// indicates whether two iterator point to different element
  template < typename Val, typename Priority, typename Cmp >
  INLINE bool SortedPriorityQueueIterator< Val, Priority, Cmp >::operator!=(
     const SortedPriorityQueueIterator< Val, Priority, Cmp >& from) const {
    return !operator==(from);
  }

  /// move to the next element in the queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIterator< Val, Priority, Cmp >&
     SortedPriorityQueueIterator< Val, Priority, Cmp >::operator++() noexcept {
    SharedAVLTreeReverseIterator< Val, TreeCmp >::operator++();
    return *this;
  }

  /// move to the next k element
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIterator< Val, Priority, Cmp >&
     SortedPriorityQueueIterator< Val, Priority, Cmp >::operator+=(const Size k) noexcept {
    SharedAVLTreeReverseIterator< Val, TreeCmp >::operator+=(k);
    return *this;
  }

  /// move to the preceding element in the queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIterator< Val, Priority, Cmp >&
     SortedPriorityQueueIterator< Val, Priority, Cmp >::operator--() noexcept {
    SharedAVLTreeReverseIterator< Val, TreeCmp >::operator--();
    return *this;
  }

  /// move to the preceding k element
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIterator< Val, Priority, Cmp >&
     SortedPriorityQueueIterator< Val, Priority, Cmp >::operator-=(const Size k) noexcept {
    SharedAVLTreeReverseIterator< Val, TreeCmp >::operator-=(k);
    return *this;
  }

  /// returns the value pointed to by the iterator
  template < typename Val, typename Priority, typename Cmp >
  INLINE typename SortedPriorityQueueIterator< Val, Priority, Cmp >::const_reference
     SortedPriorityQueueIterator< Val, Priority, Cmp >::operator*() const {
    return SharedAVLTreeReverseIterator< Val, TreeCmp >::operator*().value;
  }

  /// returns a pointer on the value pointed to by the iterator
  template < typename Val, typename Priority, typename Cmp >
  INLINE typename SortedPriorityQueueIterator< Val, Priority, Cmp >::const_pointer
     SortedPriorityQueueIterator< Val, Priority, Cmp >::operator->() const {
    auto node = SharedAVLTreeReverseIterator< Val, TreeCmp >::operator->();
    if (node != nullptr) return &(node->value);
    else { GUM_ERROR(NotFound, "The sorted priority queue iterator does not point on any value") }
  }

  /// ======================================================================================

  /// constructor for begin safe iterators
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >::SortedPriorityQueueIteratorSafe(
     SortedPriorityQueue< Val, Priority, Cmp >& queue,
     const bool                                 rbegin) :
      SharedAVLTreeReverseIteratorSafe< Val, TreeCmp >(queue._tree_, rbegin) {
    GUM_CONSTRUCTOR(SortedPriorityQueueIteratorSafe)
  }

  /// copy constructor
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >::SortedPriorityQueueIteratorSafe(
     const SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >& from) :
      SharedAVLTreeReverseIteratorSafe< Val, TreeCmp >(from) {
    GUM_CONS_CPY(SortedPriorityQueueIteratorSafe)
  }

  /// move constructor
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >::SortedPriorityQueueIteratorSafe(
     SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >&& from) :
      SharedAVLTreeReverseIteratorSafe< Val, TreeCmp >(std::move(from)) {
    GUM_CONS_CPY(SortedPriorityQueueIteratorSafe)
  }

  /// destructor
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >::
     ~SortedPriorityQueueIteratorSafe() noexcept {
    GUM_DESTRUCTOR(SortedPriorityQueueIteratorSafe)
  }

  /// copy operator
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >&
     SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >::operator=(
        const SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >& from) {
    SharedAVLTreeReverseIteratorSafe< Val, TreeCmp >::operator=(from);
    return *this;
  }

  /// move operator
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >&
     SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >::operator=(
        SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >&& from) {
    SharedAVLTreeReverseIteratorSafe< Val, TreeCmp >::operator=(std::move(from));
    return *this;
  }

  /// indicates whether two iterator point to the same element
  template < typename Val, typename Priority, typename Cmp >
  INLINE bool SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >::operator==(
     const SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >& from) const {
    return SharedAVLTreeReverseIteratorSafe< Val, TreeCmp >::operator==(from);
  }

  /// indicates whether two iterator point to different element
  template < typename Val, typename Priority, typename Cmp >
  INLINE bool SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >::operator!=(
     const SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >& from) const {
    return !operator==(from);
  }

  /// move to the next element in the queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >&
     SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >::operator++() noexcept {
    SharedAVLTreeReverseIteratorSafe< Val, TreeCmp >::operator++();
    return *this;
  }

  /// move to the next k elements
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >&
     SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >::operator+=(const Size k) noexcept {
    SharedAVLTreeReverseIteratorSafe< Val, TreeCmp >::operator+=(k);
    return *this;
  }

  /// move to the preceding element in the queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >&
     SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >::operator--() noexcept {
    SharedAVLTreeReverseIteratorSafe< Val, TreeCmp >::operator--();
    return *this;
  }

  /// move to the preceding k element
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >&
     SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >::operator-=(const Size k) noexcept {
    SharedAVLTreeReverseIteratorSafe< Val, TreeCmp >::operator-=(k);
    return *this;
  }

  /// returns the value pointed to by the iterator
  template < typename Val, typename Priority, typename Cmp >
  INLINE typename SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >::const_reference
     SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >::operator*() const {
    return SharedAVLTreeReverseIteratorSafe< Val, TreeCmp >::operator*().value;
  }

  /// returns a pointer on the value pointed to by the iterator
  template < typename Val, typename Priority, typename Cmp >
  INLINE typename SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >::const_pointer
     SortedPriorityQueueIteratorSafe< Val, Priority, Cmp >::operator->() const {
    auto node = SharedAVLTreeReverseIteratorSafe< Val, TreeCmp >::operator->();
    if (node != nullptr) return &(node->value);
    else { GUM_ERROR(NotFound, "The sorted priority queue iterator does not point on any value") }
  }

  /// ======================================================================================

  /// constructor for rbegin iterators
  template < typename Val, typename Priority, typename Cmp >
  INLINE
     SortedPriorityQueueReverseIterator< Val, Priority, Cmp >::SortedPriorityQueueReverseIterator(
        const SortedPriorityQueue< Val, Priority, Cmp >& queue,
        const bool                                       rbegin) noexcept :
      SharedAVLTreeIterator< Val, TreeCmp >(queue._tree_, rbegin) {
    GUM_CONSTRUCTOR(SortedPriorityQueueReverseIterator)
  }

  /// copy constructor
  template < typename Val, typename Priority, typename Cmp >
  INLINE
     SortedPriorityQueueReverseIterator< Val, Priority, Cmp >::SortedPriorityQueueReverseIterator(
        const SortedPriorityQueueReverseIterator< Val, Priority, Cmp >& from) noexcept :
      SharedAVLTreeIterator< Val, TreeCmp >(from) {
    GUM_CONS_CPY(SortedPriorityQueueReverseIterator)
  }

  /// move constructor
  template < typename Val, typename Priority, typename Cmp >
  INLINE
     SortedPriorityQueueReverseIterator< Val, Priority, Cmp >::SortedPriorityQueueReverseIterator(
        SortedPriorityQueueReverseIterator< Val, Priority, Cmp >&& from) noexcept :
      SharedAVLTreeIterator< Val, TreeCmp >(std::move(from)) {
    GUM_CONS_MOV(SortedPriorityQueueReverseIterator)
  }


  /// destructor
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIterator< Val, Priority, Cmp >::
     ~SortedPriorityQueueReverseIterator() noexcept {
    GUM_DESTRUCTOR(SortedPriorityQueueReverseIterator)
  }

  /// copy operator
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIterator< Val, Priority, Cmp >&
     SortedPriorityQueueReverseIterator< Val, Priority, Cmp >::operator=(
        const SortedPriorityQueueReverseIterator< Val, Priority, Cmp >& from) noexcept {
    SharedAVLTreeIterator< Val, TreeCmp >::operator=(from);
    return *this;
  }

  /// move operator
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIterator< Val, Priority, Cmp >&
     SortedPriorityQueueReverseIterator< Val, Priority, Cmp >::operator=(
        SortedPriorityQueueReverseIterator< Val, Priority, Cmp >&& from) noexcept {
    SharedAVLTreeIterator< Val, TreeCmp >::operator=(std::move(from));
    return *this;
  }

  /// indicates whether two iterator point to the same element
  template < typename Val, typename Priority, typename Cmp >
  INLINE bool SortedPriorityQueueReverseIterator< Val, Priority, Cmp >::operator==(
     const SortedPriorityQueueReverseIterator< Val, Priority, Cmp >& from) const {
    return SharedAVLTreeIterator< Val, TreeCmp >::operator==(from);
  }

  /// indicates whether two iterator point to different element
  template < typename Val, typename Priority, typename Cmp >
  INLINE bool SortedPriorityQueueReverseIterator< Val, Priority, Cmp >::operator!=(
     const SortedPriorityQueueReverseIterator< Val, Priority, Cmp >& from) const {
    return !operator==(from);
  }

  /// move to the next element in the queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIterator< Val, Priority, Cmp >&
     SortedPriorityQueueReverseIterator< Val, Priority, Cmp >::operator++() noexcept {
    SharedAVLTreeIterator< Val, TreeCmp >::operator++();
    return *this;
  }

  /// move to the next k element
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIterator< Val, Priority, Cmp >&
     SortedPriorityQueueReverseIterator< Val, Priority, Cmp >::operator+=(const Size k) noexcept {
    SharedAVLTreeIterator< Val, TreeCmp >::operator+=(k);
    return *this;
  }

  /// move to the preceding element in the queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIterator< Val, Priority, Cmp >&
     SortedPriorityQueueReverseIterator< Val, Priority, Cmp >::operator--() noexcept {
    SharedAVLTreeIterator< Val, TreeCmp >::operator--();
    return *this;
  }

  /// move to the preceding k element
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIterator< Val, Priority, Cmp >&
     SortedPriorityQueueReverseIterator< Val, Priority, Cmp >::operator-=(const Size k) noexcept {
    SharedAVLTreeIterator< Val, TreeCmp >::operator-=(k);
    return *this;
  }

  /// returns the value pointed to by the iterator
  template < typename Val, typename Priority, typename Cmp >
  INLINE typename SortedPriorityQueueReverseIterator< Val, Priority, Cmp >::const_reference
     SortedPriorityQueueReverseIterator< Val, Priority, Cmp >::operator*() const {
    return SharedAVLTreeIterator< Val, TreeCmp >::operator*().value;
  }

  /// returns a pointer on the value pointed to by the iterator
  template < typename Val, typename Priority, typename Cmp >
  INLINE typename SortedPriorityQueueReverseIterator< Val, Priority, Cmp >::const_pointer
     SortedPriorityQueueReverseIterator< Val, Priority, Cmp >::operator->() const {
    auto node = SharedAVLTreeIterator< Val, TreeCmp >::operator->();
    if (node != nullptr) return &(node->value);
    else { GUM_ERROR(NotFound, "The sorted priority queue iterator does not point on any value") }
  }

  /// ======================================================================================

  /// constructor for rbegin safe iterators
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >::
     SortedPriorityQueueReverseIteratorSafe(SortedPriorityQueue< Val, Priority, Cmp >& queue,
                                            const bool                                 rbegin) :
      SharedAVLTreeIteratorSafe< Val, TreeCmp >(queue._tree_, rbegin) {
    GUM_CONSTRUCTOR(SortedPriorityQueueReverseIteratorSafe)
  }

  /// copy constructor
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >::
     SortedPriorityQueueReverseIteratorSafe(
        const SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >& from) :
      SharedAVLTreeIteratorSafe< Val, TreeCmp >(from) {
    GUM_CONS_CPY(SortedPriorityQueueReverseIteratorSafe)
  }

  /// move constructor
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >::
     SortedPriorityQueueReverseIteratorSafe(
        SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >&& from) :
      SharedAVLTreeIteratorSafe< Val, TreeCmp >(std::move(from)) {
    GUM_CONS_MOV(SortedPriorityQueueReverseIteratorSafe)
  }

  /// destructor
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >::
     ~SortedPriorityQueueReverseIteratorSafe() noexcept {
    GUM_DESTRUCTOR(SortedPriorityQueueReverseIteratorSafe)
  }

  /// copy operator
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >&
     SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >::operator=(
        const SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >& from) {
    SharedAVLTreeIteratorSafe< Val, TreeCmp >::operator=(from);
    return *this;
  }

  /// move operator
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >&
     SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >::operator=(
        SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >&& from) {
    SharedAVLTreeIteratorSafe< Val, TreeCmp >::operator=(std::move(from));
    return *this;
  }

  /// indicates whether two iterator point to the same element
  template < typename Val, typename Priority, typename Cmp >
  INLINE bool SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >::operator==(
     const SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >& from) const {
    return SharedAVLTreeIteratorSafe< Val, TreeCmp >::operator==(from);
  }

  /// indicates whether two iterator point to different element
  template < typename Val, typename Priority, typename Cmp >
  INLINE bool SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >::operator!=(
     const SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >& from) const {
    return !operator==(from);
  }

  /// move to the next element in the queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >&
     SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >::operator++() noexcept {
    SharedAVLTreeIteratorSafe< Val, TreeCmp >::operator++();
    return *this;
  }

  /// move to the next k element
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >&
     SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >::operator+=(
        const Size k) noexcept {
    SharedAVLTreeIteratorSafe< Val, TreeCmp >::operator+=(k);
    return *this;
  }

  /// move to the preceding element in the queue
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >&
     SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >::operator--() noexcept {
    SharedAVLTreeIteratorSafe< Val, TreeCmp >::operator--();
    return *this;
  }

  /// move to the preceding k element
  template < typename Val, typename Priority, typename Cmp >
  INLINE SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >&
     SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >::operator-=(
        const Size k) noexcept {
    SharedAVLTreeIteratorSafe< Val, TreeCmp >::operator-=(k);
    return *this;
  }

  /// returns the value pointed to by the iterator
  template < typename Val, typename Priority, typename Cmp >
  INLINE typename SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >::const_reference
     SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >::operator*() const {
    return SharedAVLTreeIteratorSafe< Val, TreeCmp >::operator*().value;
  }

  /// returns a pointer on the value pointed to by the iterator
  template < typename Val, typename Priority, typename Cmp >
  INLINE typename SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >::const_pointer
     SortedPriorityQueueReverseIteratorSafe< Val, Priority, Cmp >::operator->() const {
    auto node = SharedAVLTreeIteratorSafe< Val, TreeCmp >::operator->();
    if (node != nullptr) return &(node->value);
    else { GUM_ERROR(NotFound, "The sorted priority queue iterator does not point on any value") }
  }


}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
