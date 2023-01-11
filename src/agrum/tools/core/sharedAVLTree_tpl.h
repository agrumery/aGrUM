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

#include <functional>
#include <type_traits>
#include <sstream>
#include <utility>

#include <agrum/tools/core/sharedAVLTree.h>
#include <agrum/tools/core/exceptions.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /// Basic constructor. Creates an empty tree.
  template < typename Val, typename Cmp >
  INLINE SharedAVLTree< Val, Cmp >::SharedAVLTree(const Cmp& compare) :
      AVLTree< Val, Cmp >(compare) {
    this->owns_nodes_ = false;

    // for debugging purposes
    GUM_CONSTRUCTOR(SharedAVLTree);
  }

  /// Move constructor.
  template < typename Val, typename Cmp >
  INLINE SharedAVLTree< Val, Cmp >::SharedAVLTree(SharedAVLTree< Val, Cmp >&& from) noexcept :
      AVLTree< Val, Cmp >(std::move(from)) {
    // for debugging purposes
    GUM_CONS_MOV(SharedAVLTree);
  }

  /// destructor.
  template < typename Val, typename Cmp >
  INLINE SharedAVLTree< Val, Cmp >::~SharedAVLTree() {
    // for debugging purposes
    GUM_DESTRUCTOR(SharedAVLTree);
  }

  /// Move operator
  template < typename Val, typename Cmp >
  INLINE SharedAVLTree< Val, Cmp >&
     SharedAVLTree< Val, Cmp >::operator=(SharedAVLTree< Val, Cmp >&& from) {
    operator=(std::move(from));
    return *this;
  }

  /// returns the node containing the max element (w.r.t. Cmp) in the tree
  template < typename Val, typename Cmp >
  INLINE typename SharedAVLTree< Val, Cmp >::AVLNode*
     SharedAVLTree< Val, Cmp >::highestNode() const noexcept {
    return this->highest_node_;
  }

  /// returns the node containing the min element (w.r.t. Cmp) in the tree
  template < typename Val, typename Cmp >
  INLINE typename SharedAVLTree< Val, Cmp >::AVLNode*
     SharedAVLTree< Val, Cmp >::lowestNode() const noexcept {
    return this->lowest_node_;
  }

  /// adds a new node into the tree
  template < typename Val, typename Cmp >
  INLINE void SharedAVLTree< Val, Cmp >::insert(AVLNode* node) {
    // the insert_ method will update the parent of node, but we should also
    // guarantee that the children are null pointers before the insertion (as the
    // node will be added to the
    node->left_child  = nullptr;
    node->right_child = nullptr;
    this->insert_(node);
  }

  /// remove an element from the tree
  template < typename Val, typename Cmp >
  INLINE void SharedAVLTree< Val, Cmp >::erase(AVLNode* node) {
    this->removeNodeFromTree_(node);
  }

  /// remove the element pointed to by an iterator
  template < typename Val, typename Cmp >
  INLINE void SharedAVLTree< Val, Cmp >::erase(iterator_safe& iter) {
    this->removeNodeFromTree_(iter.node_);
  }

  /// remove the element pointed to by an iterator
  template < typename Val, typename Cmp >
  INLINE void SharedAVLTree< Val, Cmp >::erase(reverse_iterator_safe& iter) {
    this->removeNodeFromTree_(iter.node_);
  }

  /// returns a new iterator pointing to the minimal element of the tree
  template < typename Val, typename Cmp >
  INLINE typename SharedAVLTree< Val, Cmp >::iterator SharedAVLTree< Val, Cmp >::begin() const {
    return SharedAVLTreeIterator(*this);
  }

  /// returns an iterator pointing just after the maximal element
  template < typename Val, typename Cmp >
  constexpr const typename SharedAVLTree< Val, Cmp >::iterator&
     SharedAVLTree< Val, Cmp >::end() const {
    return *(reinterpret_cast< const iterator* >(_SharedAVLTree_end_));
  }

  /// returns a new iterator pointing to the maximal element of the tree
  template < typename Val, typename Cmp >
  INLINE typename SharedAVLTree< Val, Cmp >::reverse_iterator
     SharedAVLTree< Val, Cmp >::rbegin() const {
    return SharedAVLTreeReverseIterator(*this, true);
  }

  /// returns an iterator pointing just before the minimal element
  template < typename Val, typename Cmp >
  constexpr const typename SharedAVLTree< Val, Cmp >::reverse_iterator&
     SharedAVLTree< Val, Cmp >::rend() const {
    return *(reinterpret_cast< const reverse_iterator* >(_SharedAVLTree_rend_));
  }

  /// returns a new safe iterator pointing to the minimal element of the tree
  template < typename Val, typename Cmp >
  INLINE typename SharedAVLTree< Val, Cmp >::iterator_safe SharedAVLTree< Val, Cmp >::beginSafe() {
    return SharedAVLTreeIteratorSafe(*this);
  }

  /// returns a safe iterator pointing just after the maximal element
  template < typename Val, typename Cmp >
  constexpr const typename SharedAVLTree< Val, Cmp >::iterator_safe&
     SharedAVLTree< Val, Cmp >::endSafe() const {
    return *(reinterpret_cast< const iterator_safe* >(_SharedAVLTree_end_safe_));
  }

  /// returns a safe iterator pointing to the maximal element of the tree
  template < typename Val, typename Cmp >
  INLINE typename SharedAVLTree< Val, Cmp >::reverse_iterator_safe
     SharedAVLTree< Val, Cmp >::rbeginSafe() {
    return SharedAVLTreeReverseIteratorSafe(*this, true);
  }

  /// returns a safe iterator pointing just before the minimal element
  template < typename Val, typename Cmp >
  constexpr const typename SharedAVLTree< Val, Cmp >::reverse_iterator_safe&
     SharedAVLTree< Val, Cmp >::rendSafe() const {
    return *(reinterpret_cast< const reverse_iterator_safe* >(_SharedAVLTree_rend_safe_));
  }

  /// ======================================================================================

  /// constructor for begin iterators
  template < typename Val, typename Cmp >
  INLINE
     SharedAVLTreeIterator< Val, Cmp >::SharedAVLTreeIterator(const SharedAVLTree< Val, Cmp >& tree,
                                                              const bool begin) noexcept :
      AVLTreeIterator< Val, Cmp >(tree, begin) {
    GUM_CONSTRUCTOR(SharedAVLTreeIterator)
  }

  /// copy constructor
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIterator< Val, Cmp >::SharedAVLTreeIterator(
     const SharedAVLTreeIterator< Val, Cmp >& from) noexcept :
      AVLTreeIterator< Val, Cmp >(from) {
    GUM_CONS_CPY(SharedAVLTreeIterator)
  }

  /// move constructor
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIterator< Val, Cmp >::SharedAVLTreeIterator(
     SharedAVLTreeIterator< Val, Cmp >&& from) noexcept :
      AVLTreeIterator< Val, Cmp >(std::move(from)) {
    GUM_CONS_MOV(SharedAVLTreeIterator)
  }

  /// destructor
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIterator< Val, Cmp >::~SharedAVLTreeIterator() noexcept {
    GUM_DESTRUCTOR(SharedAVLTreeIterator)
  }

  /// copy operator
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIterator< Val, Cmp >& SharedAVLTreeIterator< Val, Cmp >::operator=(
     const SharedAVLTreeIterator< Val, Cmp >& from) noexcept {
    AVLTreeIterator< Val, Cmp >::operator=(from);
    return *this;
  }

  /// move operator
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIterator< Val, Cmp >& SharedAVLTreeIterator< Val, Cmp >::operator=(
     SharedAVLTreeIterator< Val, Cmp >&& from) noexcept {
    AVLTreeIterator< Val, Cmp >::operator=(std::move(from));
    return *this;
  }

  /// indicates whether two iterator point to the same element
  template < typename Val, typename Cmp >
  INLINE bool SharedAVLTreeIterator< Val, Cmp >::operator==(
     const SharedAVLTreeIterator< Val, Cmp >& from) const {
    return AVLTreeIterator< Val, Cmp >::operator==(from);
  }

  /// indicates whether two iterator point to different element
  template < typename Val, typename Cmp >
  INLINE bool SharedAVLTreeIterator< Val, Cmp >::operator!=(
     const SharedAVLTreeIterator< Val, Cmp >& from) const {
    return !SharedAVLTreeIterator< Val, Cmp >::operator==(from);
  }

  /// move to the next element in the tree
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIterator< Val, Cmp >&
     SharedAVLTreeIterator< Val, Cmp >::operator++() noexcept {
    AVLTreeIterator< Val, Cmp >::operator++();
    return *this;
  }

  /// move to the next k element
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIterator< Val, Cmp >&
     SharedAVLTreeIterator< Val, Cmp >::operator+=(const Size k) noexcept {
    AVLTreeIterator< Val, Cmp >::operator+=(k);
    return *this;
  }

  /// move to the preceding element in the tree
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIterator< Val, Cmp >&
     SharedAVLTreeIterator< Val, Cmp >::operator--() noexcept {
    AVLTreeIterator< Val, Cmp >::operator--();
    return *this;
  }

  /// move to the preceding k element
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIterator< Val, Cmp >&
     SharedAVLTreeIterator< Val, Cmp >::operator-=(const Size k) noexcept {
    AVLTreeIterator< Val, Cmp >::operator-=(k);
    return *this;
  }

  /// returns the element pointed to by the iterator
  template < typename Val, typename Cmp >
  INLINE typename SharedAVLTreeIterator< Val, Cmp >::const_reference
     SharedAVLTreeIterator< Val, Cmp >::operator*() const {
    if (this->node_ != nullptr) return *(this->node_);
    else {
      if ((this->next_node_ == nullptr) || (this->preceding_node_ == nullptr)) {
        GUM_ERROR(NotFound, "an end SharedAVLTree iterator does not contain any value")
      } else {
        GUM_ERROR(NotFound, "the SharedAVLTree iterator points to an erased value")
      }
    }
  }

  /// returns a pointer on the node pointed to by the iterator
  template < typename Val, typename Cmp >
  INLINE typename SharedAVLTreeIterator< Val, Cmp >::const_pointer
     SharedAVLTreeIterator< Val, Cmp >::operator->() const {
    return this->node_;
  }

  /// ======================================================================================

  /// constructor for begin safe iterators
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIteratorSafe< Val, Cmp >::SharedAVLTreeIteratorSafe(
     SharedAVLTree< Val, Cmp >& tree,
     const bool                 rbegin) :
      AVLTreeIteratorSafe< Val, Cmp >(tree, rbegin) {
    GUM_CONSTRUCTOR(SharedAVLTreeIteratorSafe)
  }

  /// copy constructor
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIteratorSafe< Val, Cmp >::SharedAVLTreeIteratorSafe(
     const SharedAVLTreeIteratorSafe< Val, Cmp >& from) :
      AVLTreeIteratorSafe< Val, Cmp >(from) {
    GUM_CONS_CPY(SharedAVLTreeIteratorSafe)
  }

  /// move constructor
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIteratorSafe< Val, Cmp >::SharedAVLTreeIteratorSafe(
     SharedAVLTreeIteratorSafe< Val, Cmp >&& from) :
      AVLTreeIteratorSafe< Val, Cmp >(std::move(from)) {
    GUM_CONS_CPY(SharedAVLTreeIteratorSafe)
  }

  /// destructor
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIteratorSafe< Val, Cmp >::~SharedAVLTreeIteratorSafe() noexcept {
    GUM_DESTRUCTOR(SharedAVLTreeIteratorSafe)
  }

  /// copy operator
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIteratorSafe< Val, Cmp >& SharedAVLTreeIteratorSafe< Val, Cmp >::operator=(
     const SharedAVLTreeIteratorSafe< Val, Cmp >& from) {
    AVLTreeIteratorSafe< Val, Cmp >::operator=(from);
    return *this;
  }

  /// move operator
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIteratorSafe< Val, Cmp >& SharedAVLTreeIteratorSafe< Val, Cmp >::operator=(
     SharedAVLTreeIteratorSafe< Val, Cmp >&& from) {
    AVLTreeIteratorSafe< Val, Cmp >::operator=(std::move(from));
    return *this;
  }

  /// indicates whether two iterator point to the same element
  template < typename Val, typename Cmp >
  INLINE bool SharedAVLTreeIteratorSafe< Val, Cmp >::operator==(
     const SharedAVLTreeIteratorSafe< Val, Cmp >& from) const {
    return AVLTreeIteratorSafe< Val, Cmp >::operator==(from);
  }

  /// indicates whether two iterator point to different element
  template < typename Val, typename Cmp >
  INLINE bool SharedAVLTreeIteratorSafe< Val, Cmp >::operator!=(
     const SharedAVLTreeIteratorSafe< Val, Cmp >& from) const {
    return !SharedAVLTreeIteratorSafe< Val, Cmp >::operator==(from);
  }

  /// move to the next element in the tree
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIteratorSafe< Val, Cmp >&
     SharedAVLTreeIteratorSafe< Val, Cmp >::operator++() noexcept {
    AVLTreeIteratorSafe< Val, Cmp >::operator++();
    return *this;
  }

  /// move to the next k elements
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIteratorSafe< Val, Cmp >&
     SharedAVLTreeIteratorSafe< Val, Cmp >::operator+=(const Size k) noexcept {
    AVLTreeIteratorSafe< Val, Cmp >::operator+=(k);
    return *this;
  }

  /// move to the preceding element in the tree
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIteratorSafe< Val, Cmp >&
     SharedAVLTreeIteratorSafe< Val, Cmp >::operator--() noexcept {
    AVLTreeIteratorSafe< Val, Cmp >::operator--();
    return *this;
  }

  /// move to the preceding k element
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeIteratorSafe< Val, Cmp >&
     SharedAVLTreeIteratorSafe< Val, Cmp >::operator-=(const Size k) noexcept {
    AVLTreeIteratorSafe< Val, Cmp >::operator-=(k);
    return *this;
  }

  /// returns the element pointed to by the iterator
  template < typename Val, typename Cmp >
  INLINE typename SharedAVLTreeIteratorSafe< Val, Cmp >::const_reference
     SharedAVLTreeIteratorSafe< Val, Cmp >::operator*() const {
    if (this->node_ != nullptr) return *(this->node_);
    else {
      if ((this->next_node_ == nullptr) || (this->preceding_node_ == nullptr)) {
        GUM_ERROR(NotFound, "an end SharedAVLTree iterator does not contain any value")
      } else {
        GUM_ERROR(NotFound, "the SharedAVLTree iterator points to an erased value")
      }
    }
  }

  /// returns a pointer on the node pointed to by the iterator
  template < typename Val, typename Cmp >
  INLINE typename SharedAVLTreeIteratorSafe< Val, Cmp >::const_pointer
     SharedAVLTreeIteratorSafe< Val, Cmp >::operator->() const {
    return this->node_;
  }

  /// ======================================================================================

  /// constructor for rbegin iterators
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIterator< Val, Cmp >::SharedAVLTreeReverseIterator(
     const SharedAVLTree< Val, Cmp >& tree,
     const bool                       rbegin) noexcept :
      SharedAVLTreeIterator< Val, Cmp >(tree, !rbegin) {
    GUM_CONSTRUCTOR(SharedAVLTreeReverseIterator)
  }

  /// copy constructor
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIterator< Val, Cmp >::SharedAVLTreeReverseIterator(
     const SharedAVLTreeReverseIterator< Val, Cmp >& from) noexcept :
      SharedAVLTreeIterator< Val, Cmp >(from) {
    GUM_CONS_CPY(SharedAVLTreeReverseIterator)
  }

  /// move constructor
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIterator< Val, Cmp >::SharedAVLTreeReverseIterator(
     SharedAVLTreeReverseIterator< Val, Cmp >&& from) noexcept :
      SharedAVLTreeIterator< Val, Cmp >(std::move(from)) {
    GUM_CONS_CPY(SharedAVLTreeReverseIterator)
  }

  /// destructor
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIterator< Val, Cmp >::~SharedAVLTreeReverseIterator() noexcept {
    GUM_DESTRUCTOR(SharedAVLTreeReverseIterator)
  }

  /// copy operator
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIterator< Val, Cmp >&
     SharedAVLTreeReverseIterator< Val, Cmp >::operator=(
        const SharedAVLTreeReverseIterator< Val, Cmp >& from) noexcept {
    SharedAVLTreeIterator< Val, Cmp >::operator=(from);
    return *this;
  }

  /// move operator
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIterator< Val, Cmp >&
     SharedAVLTreeReverseIterator< Val, Cmp >::operator=(
        SharedAVLTreeReverseIterator< Val, Cmp >&& from) noexcept {
    SharedAVLTreeIterator< Val, Cmp >::operator=(std::move(from));
    return *this;
  }

  /// indicates whether two iterator point to the same element
  template < typename Val, typename Cmp >
  INLINE bool SharedAVLTreeReverseIterator< Val, Cmp >::operator==(
     const SharedAVLTreeReverseIterator< Val, Cmp >& from) const {
    // when node_ is different from nullptr, testing whether "this" is equal to from
    // simply amounts to comparing their node_ fields. However, due to erasures in
    // the tree, it may happen that two iterators pointing to different nodes have
    // a nullptr node_ field. In this case, they will be equal if and only if their
    // preceding_node_ fields are equal. Here, it is important to use the preceding_node_
    // rather than their next_node_ field because the rend iterator has nullptr
    // as the value of its next_node_ while a reverse iterator moving by ++ operators
    // up to the end will not have this value for its next_node_. However, it
    // will have a preceding_node_ equal to nullptr, exactly as the end iterator.
    return (this->node_ == from.node_) && (this->preceding_node_ == from.preceding_node_);
  }

  /// indicates whether two iterator point to different element
  template < typename Val, typename Cmp >
  INLINE bool SharedAVLTreeReverseIterator< Val, Cmp >::operator!=(
     const SharedAVLTreeReverseIterator< Val, Cmp >& from) const {
    return !SharedAVLTreeReverseIterator< Val, Cmp >::operator==(from);
  }

  /// move to the next element in the tree
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIterator< Val, Cmp >&
     SharedAVLTreeReverseIterator< Val, Cmp >::operator++() noexcept {
    SharedAVLTreeIterator< Val, Cmp >::operator--();
    return *this;
  }

  /// move to the next k element
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIterator< Val, Cmp >&
     SharedAVLTreeReverseIterator< Val, Cmp >::operator+=(const Size k) noexcept {
    SharedAVLTreeIterator< Val, Cmp >::operator-=(k);
    return *this;
  }

  /// move to the preceding element in the tree
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIterator< Val, Cmp >&
     SharedAVLTreeReverseIterator< Val, Cmp >::operator--() noexcept {
    SharedAVLTreeIterator< Val, Cmp >::operator++();
    return *this;
  }

  /// move to the preceding k element
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIterator< Val, Cmp >&
     SharedAVLTreeReverseIterator< Val, Cmp >::operator-=(const Size k) noexcept {
    SharedAVLTreeIterator< Val, Cmp >::operator+=(k);
    return *this;
  }

  /// ======================================================================================

  /// constructor for rbegin safe iterators
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIteratorSafe< Val, Cmp >::SharedAVLTreeReverseIteratorSafe(
     SharedAVLTree< Val, Cmp >& tree,
     const bool                 rbegin) :
      SharedAVLTreeIteratorSafe< Val, Cmp >(tree, !rbegin) {
    GUM_CONSTRUCTOR(SharedAVLTreeReverseIteratorSafe)
  }

  /// copy constructor
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIteratorSafe< Val, Cmp >::SharedAVLTreeReverseIteratorSafe(
     const SharedAVLTreeReverseIteratorSafe< Val, Cmp >& from) :
      SharedAVLTreeIteratorSafe< Val, Cmp >(from) {
    GUM_CONS_CPY(SharedAVLTreeReverseIteratorSafe)
  }

  /// move constructor
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIteratorSafe< Val, Cmp >::SharedAVLTreeReverseIteratorSafe(
     SharedAVLTreeReverseIteratorSafe< Val, Cmp >&& from) :
      SharedAVLTreeIteratorSafe< Val, Cmp >(std::move(from)) {
    GUM_CONS_MOV(SharedAVLTreeReverseIteratorSafe)
  }

  /// destructor
  template < typename Val, typename Cmp >
  INLINE
     SharedAVLTreeReverseIteratorSafe< Val, Cmp >::~SharedAVLTreeReverseIteratorSafe() noexcept {
    GUM_DESTRUCTOR(SharedAVLTreeReverseIteratorSafe)
  }

  /// copy operator
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIteratorSafe< Val, Cmp >&
     SharedAVLTreeReverseIteratorSafe< Val, Cmp >::operator=(
        const SharedAVLTreeReverseIteratorSafe< Val, Cmp >& from) {
    SharedAVLTreeIteratorSafe< Val, Cmp >::operator=(from);
    return *this;
  }

  /// move operator
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIteratorSafe< Val, Cmp >&
     SharedAVLTreeReverseIteratorSafe< Val, Cmp >::operator=(
        SharedAVLTreeReverseIteratorSafe< Val, Cmp >&& from) {
    SharedAVLTreeIteratorSafe< Val, Cmp >::operator=(std::move(from));
    return *this;
  }

  /// indicates whether two iterator point to the same element
  template < typename Val, typename Cmp >
  INLINE bool SharedAVLTreeReverseIteratorSafe< Val, Cmp >::operator==(
     const SharedAVLTreeReverseIteratorSafe< Val, Cmp >& from) const {
    // when node_ is different from nullptr, testing whether "this" is equal to from
    // simply amounts to comparing their node_ fields. However, due to erasures in
    // the tree, it may happen that two iterators pointing to different nodes have
    // a nullptr node_ field. In this case, they will be equal if and only if their
    // preceding_node_ fields are equal. Here, it is important to use the preceding_node_
    // rather than their next_node_ field because the rend iterator has nullptr
    // as the value of its next_node_ while a reverse iterator moving by ++ operators
    // up to the end will not have this value for its next_node_. However, it
    // will have a preceding_node_ equal to nullptr, exactly as the end iterator.
    return (this->node_ == from.node_) && (this->preceding_node_ == from.preceding_node_);
  }

  /// indicates whether two iterator point to different element
  template < typename Val, typename Cmp >
  INLINE bool SharedAVLTreeReverseIteratorSafe< Val, Cmp >::operator!=(
     const SharedAVLTreeReverseIteratorSafe< Val, Cmp >& from) const {
    return !SharedAVLTreeReverseIteratorSafe< Val, Cmp >::operator==(from);
  }

  /// move to the next element in the tree
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIteratorSafe< Val, Cmp >&
     SharedAVLTreeReverseIteratorSafe< Val, Cmp >::operator++() noexcept {
    SharedAVLTreeIteratorSafe< Val, Cmp >::operator--();
    return *this;
  }

  /// move to the next k element
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIteratorSafe< Val, Cmp >&
     SharedAVLTreeReverseIteratorSafe< Val, Cmp >::operator+=(const Size k) noexcept {
    SharedAVLTreeIteratorSafe< Val, Cmp >::operator-=(k);
    return *this;
  }

  /// move to the preceding element in the tree
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIteratorSafe< Val, Cmp >&
     SharedAVLTreeReverseIteratorSafe< Val, Cmp >::operator--() noexcept {
    SharedAVLTreeIteratorSafe< Val, Cmp >::operator++();
    return *this;
  }

  /// move to the preceding k element
  template < typename Val, typename Cmp >
  INLINE SharedAVLTreeReverseIteratorSafe< Val, Cmp >&
     SharedAVLTreeReverseIteratorSafe< Val, Cmp >::operator-=(const Size k) noexcept {
    SharedAVLTreeIteratorSafe< Val, Cmp >::operator+=(k);
    return *this;
  }


}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
