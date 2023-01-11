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

#include <agrum/tools/core/AVLTree.h>
#include <agrum/tools/core/exceptions.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /// copies recursively the nodes of the tree
  template < typename Val, typename Cmp >
  typename AVLTree< Val, Cmp >::AVLNode* AVLTree< Val, Cmp >::copySubtree_(const AVLNode* from_node,
                                                                           AVLNode* new_parent) {
    if (from_node == nullptr) return nullptr;

    AVLNode* new_node       = nullptr;
    AVLNode* new_left_child = nullptr;
    AVLNode* new_right_child;
    try {
      new_node = new AVLNode(from_node->value);

      new_left_child  = copySubtree_(from_node->left_child, new_node);
      new_right_child = copySubtree_(from_node->right_child, new_node);

      new_node->parent      = new_parent;
      new_node->left_child  = new_left_child;
      new_node->right_child = new_right_child;
      new_node->height      = from_node->height;

      return new_node;
    } catch (...) {
      if (new_node != nullptr) delete new_node;
      if (new_left_child != nullptr) deleteSubtree_(new_left_child);
      // no need to delete new_right_child: if an exception was raised, it could not
      // have been copied.
      throw;
    }
  }


  /// deletes recursively a subtree of the AVL tree
  template < typename Val, typename Cmp >
  void AVLTree< Val, Cmp >::deleteSubtree_(AVLNode* subtree_root_node) {
    if (subtree_root_node == nullptr) return;

    deleteSubtree_(subtree_root_node->left_child);
    deleteSubtree_(subtree_root_node->right_child);
    delete subtree_root_node;
  }


  /// returns the node containing the lowest element of the tree
  template < typename Val, typename Cmp >
  INLINE typename AVLTree< Val, Cmp >::AVLNode* AVLTree< Val, Cmp >::lowestNode_() const noexcept {
    if (root_node_ == nullptr) return nullptr;

    AVLNode* node = root_node_;
    while (node->left_child != nullptr)
      node = node->left_child;

    return node;
  }


  /// returns the node containing the highest element of the tree
  template < typename Val, typename Cmp >
  INLINE typename AVLTree< Val, Cmp >::AVLNode* AVLTree< Val, Cmp >::highestNode_() const noexcept {
    if (root_node_ == nullptr) return nullptr;

    AVLNode* node = root_node_;
    while (node->right_child != nullptr)
      node = node->right_child;

    return node;
  }


  /// Basic constructor. Creates an empty tree.
  template < typename Val, typename Cmp >
  INLINE AVLTree< Val, Cmp >::AVLTree(const Cmp& compare) : cmp_(compare) {
    // for debugging purposes
    GUM_CONSTRUCTOR(AVLTree);
  }


  /// Initializer list constructor.
  template < typename Val, typename Cmp >
  INLINE AVLTree< Val, Cmp >::AVLTree(std::initializer_list< Val > list) {
    try {
      for (const auto& val: list)
        insert(val);
    } catch (...) {
      // if something went wrong, free all the memory allocated
      deleteSubtree_(root_node_);

      root_node_    = nullptr;
      lowest_node_  = nullptr;
      highest_node_ = nullptr;
      nb_elements_  = Size(0);

      throw;
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(AVLTree);
  }


  /// Copy constructor.
  template < typename Val, typename Cmp >
  INLINE AVLTree< Val, Cmp >::AVLTree(const AVLTree< Val, Cmp >& from) :
      nb_elements_(from.nb_elements_), cmp_(from.cmp_) {
    root_node_    = copySubtree_(from.root_node_, nullptr);
    lowest_node_  = lowestNode_();
    highest_node_ = highestNode_();

    // for debugging purposes
    GUM_CONS_CPY(AVLTree);
  }


  /// Move constructor.
  template < typename Val, typename Cmp >
  INLINE AVLTree< Val, Cmp >::AVLTree(AVLTree< Val, Cmp >&& from) noexcept :
      root_node_(from.root_node_), lowest_node_(from.lowest_node_),
      highest_node_(from.highest_node_), nb_elements_(from.nb_elements_),
      owns_nodes_(from.owns_nodes_), cmp_(std::move(from.cmp_)),
      safe_iterators_(std::move(from.safe_iterators_)) {
    from.root_node_    = nullptr;
    from.lowest_node_  = nullptr;
    from.highest_node_ = nullptr;

    // update the tree_ field in the safe iterators
    for (auto iter: safe_iterators_) {
      iter->tree_ = this;
    }

    // for debugging purposes
    GUM_CONS_CPY(AVLTree);
  }


  /// destructor
  template < typename Val, typename Cmp >
  INLINE AVLTree< Val, Cmp >::~AVLTree() {
    if (owns_nodes_) deleteSubtree_(root_node_);

    // make the safe iterators point to nothing
    for (auto iter: safe_iterators_) {
      iter->unregisterTree_();
    }

    // for debugging purposes
    GUM_DESTRUCTOR(AVLTree);
  }


  /// Copy operator
  template < typename Val, typename Cmp >
  AVLTree< Val, Cmp >& AVLTree< Val, Cmp >::operator=(const AVLTree< Val, Cmp >& from) {
    if (this != &from) {
      if (!owns_nodes_) {
        GUM_ERROR(OperationNotAllowed,
                  "It is forbidden to copy an AVLTree into a tree that does not own its nodes")
      }

      if (owns_nodes_) deleteSubtree_(root_node_);

      // make the safe iterators point to end/rend
      for (auto iter: safe_iterators_) {
        iter->pointToEndRend_();
      }

      try {
        root_node_    = copySubtree_(from.root_node_, nullptr);
        lowest_node_  = lowestNode_();
        highest_node_ = highestNode_();
        nb_elements_  = from.nb_elements_;
        cmp_          = from.cmp_;
      } catch (...) {
        root_node_    = nullptr;
        lowest_node_  = nullptr;
        highest_node_ = nullptr;
        nb_elements_  = Size(0);

        throw;
      }
    }

    return *this;
  }


  /// move operator
  template < typename Val, typename Cmp >
  AVLTree< Val, Cmp >& AVLTree< Val, Cmp >::operator=(AVLTree< Val, Cmp >&& from) noexcept {
    if (this != &from) {
      if (!owns_nodes_) {
        GUM_ERROR(OperationNotAllowed,
                  "It is forbidden to move an AVLTree into a tree that does not own its nodes")
      }

      if (owns_nodes_) deleteSubtree_(root_node_);

      // make the safe iterators point to end/rend
      for (auto iter: safe_iterators_) {
        iter->pointToEndRend_();
      }

      root_node_    = from.root_node_;
      lowest_node_  = from.lowest_node_;
      highest_node_ = from.highest_node_;
      nb_elements_  = from.nb_elements_;
      cmp_          = std::move(from.cmp_);

      // add the iterators of from to safe_iterators_
      if (safe_iterators_.empty()) {
        safe_iterators_ = std::move(from.safe_iterators_);
        for (auto iter: safe_iterators_) {
          iter->tree_ = this;
        }
      } else {
        for (auto from_iter: from.safe_iterators_) {
          safe_iterators_.push_back(from_iter);
          safe_iterators_.back()->tree_ = this;
          from.safe_iterators_.clear();
        }
      }

      from.root_node_    = nullptr;
      from.lowest_node_  = nullptr;
      from.highest_node_ = nullptr;
    }
  }


  /// Returns the number of elements in the tree.
  template < typename Val, typename Cmp >
  INLINE Size AVLTree< Val, Cmp >::size() const noexcept {
    return nb_elements_;
  }


  /// Indicates whether the tree is empty.
  template < typename Val, typename Cmp >
  INLINE bool AVLTree< Val, Cmp >::empty() const noexcept {
    return nb_elements_ == Size(0);
  }


  /// Indicates whether the tree contains a given value.
  template < typename Val, typename Cmp >
  bool AVLTree< Val, Cmp >::contains(const value_type& val) const {
    AVLNode* node = root_node_;
    while (node != nullptr) {
      if (node->value == val) return true;
      node = cmp_(val, node->value) ? node->left_child : node->right_child;
    }
    return false;
  }


  /// Alias of contains
  template < typename Val, typename Cmp >
  INLINE bool AVLTree< Val, Cmp >::exists(const value_type& val) const {
    return contains(val);
  }


  /// returns the max element (w.r.t. Cmp) in the tree
  template < typename Val, typename Cmp >
  INLINE const typename AVLTree< Val, Cmp >::value_type& AVLTree< Val, Cmp >::highestValue() const {
    if (highest_node_ == nullptr) {
      GUM_ERROR(NotFound, "an empty AVL tree has no highest element");
    }
    return highest_node_->value;
  }


  /// returns the min element (w.r.t. Cmp) in the tree
  template < typename Val, typename Cmp >
  INLINE const typename AVLTree< Val, Cmp >::value_type& AVLTree< Val, Cmp >::lowestValue() const {
    if (lowest_node_ == nullptr) { GUM_ERROR(NotFound, "an empty AVL tree has no lowest element"); }
    return lowest_node_->value;
  }


  /// rotate the subtree rooted at q to the right
  /*
  //          q                             p
  //         / \                           / \
  //        /   \     right rotation      /   \
  //       /   / \    -------------->    / \   \
  //      p   / W \                     / U \   q
  //    /   \ +---+                     +---+ /   \
  //   /     \       <--------------         /     \
  //  / \   / \       left rotation         / \   / \
  // / U \ / V \                           / V \ / W \
  // +---+ +---+                           +---+ +---+
  */
  template < typename Val, typename Cmp >
  typename AVLTree< Val, Cmp >::AVLNode* AVLTree< Val, Cmp >::rightRotation_(AVLNode* node_q) {
    AVLNode* node_p    = node_q->left_child;
    AVLNode* parent_q  = node_q->parent;
    AVLNode* subtree_u = node_p->left_child;
    AVLNode* subtree_v = node_p->right_child;
    AVLNode* subtree_w = node_q->right_child;

    // rotate p and q
    node_p->right_child = node_q;
    node_q->parent      = node_p;

    node_p->parent = parent_q;
    if (parent_q != nullptr) {
      if (parent_q->left_child == node_q) parent_q->left_child = node_p;
      else parent_q->right_child = node_p;
    }
    node_q->left_child = subtree_v;
    if (subtree_v != nullptr) subtree_v->parent = node_q;

    // update the heights
    const int height_u = subtree_u != nullptr ? subtree_u->height : 0;
    const int height_v = subtree_v != nullptr ? subtree_v->height : 0;
    const int height_w = subtree_w != nullptr ? subtree_w->height : 0;
    node_q->height     = std::max(height_v, height_w) + 1;
    node_p->height     = std::max(node_q->height, height_u) + 1;

    // return the new root
    return node_p;
  }


  /// rotate the subtree rooted at p to the left
  /*
  //          q                             p
  //         / \                           / \
  //        /   \     right rotation      /   \
  //       /   / \    -------------->    / \   \
  //      p   / W \                     / U \   q
  //    /   \ +---+                     +---+ /   \
  //   /     \       <--------------         /     \
  //  / \   / \       left rotation         / \   / \
  // / U \ / V \                           / V \ / W \
  // +---+ +---+                           +---+ +---+
  */
  template < typename Val, typename Cmp >
  typename AVLTree< Val, Cmp >::AVLNode* AVLTree< Val, Cmp >::leftRotation_(AVLNode* node_p) {
    AVLNode* node_q    = node_p->right_child;
    AVLNode* parent_p  = node_p->parent;
    AVLNode* subtree_u = node_p->left_child;
    AVLNode* subtree_v = node_q->left_child;
    AVLNode* subtree_w = node_q->right_child;

    // rotate p and q
    node_q->left_child = node_p;
    node_p->parent     = node_q;

    node_q->parent = parent_p;
    if (parent_p != nullptr) {
      if (parent_p->left_child == node_p) parent_p->left_child = node_q;
      else parent_p->right_child = node_q;
    }

    node_p->right_child = subtree_v;
    if (subtree_v != nullptr) subtree_v->parent = node_p;

    // update the heights
    const int height_u = subtree_u != nullptr ? subtree_u->height : 0;
    const int height_v = subtree_v != nullptr ? subtree_v->height : 0;
    const int height_w = subtree_w != nullptr ? subtree_w->height : 0;
    node_p->height     = std::max(height_u, height_v) + 1;
    node_q->height     = std::max(node_p->height, height_w) + 1;

    // return the new root
    return node_q;
  }


  /// rebalance the tree moving up recursively from a given node
  template < typename Val, typename Cmp >
  void AVLTree< Val, Cmp >::rebalanceTree_(AVLNode* node) {
    AVLNode* top_node = nullptr;
    while (node != nullptr) {
      const int left_height  = node->left_child != nullptr ? node->left_child->height : 0;
      const int right_height = node->right_child != nullptr ? node->right_child->height : 0;
      node->height           = 1 + std::max(left_height, right_height);

      // if the node becomes unbalanced, rebalance it
      if (left_height > right_height + 1) {
        // here, the left subtree of node is higher than the right subtree
        AVLNode*  left_child = node->left_child;
        const int left_left_height
           = left_child->left_child != nullptr ? left_child->left_child->height : 0;
        const int left_right_height
           = left_child->right_child != nullptr ? left_child->right_child->height : 0;
        if (left_left_height < left_right_height) {
          // here, the left subtree of node is higher than the right subtree and
          // the right subtree of node's left child is also higher than the left subtree.
          // So we need a double rotation
          leftRotation_(left_child);
        }
        top_node = rightRotation_(node);
      } else if (right_height > left_height + 1) {
        // here, the right subtree of node is higher than the left subtree
        AVLNode*  right_child = node->right_child;
        const int right_left_height
           = right_child->left_child != nullptr ? right_child->left_child->height : 0;
        const int right_right_height
           = right_child->right_child != nullptr ? right_child->right_child->height : 0;
        if (right_left_height > right_right_height) {
          // here, the right subtree of node is higher than the left subtree and
          // the left subtree of node's right child is also higher than the right subtree.
          // So we need a double rotation
          rightRotation_(right_child);
        }
        top_node = leftRotation_(node);
      } else {
        top_node = node;
      }

      // move up to rebalance the nodes closer to the top of the tree
      node = top_node->parent;
    }

    // here, top_node is the root node. Since it my differ from the root node before
    // the insertion, we update root_node_, just in case
    root_node_ = top_node;
  }


  /// insert a node into the tree
  template < typename Val, typename Cmp >
  const typename AVLTree< Val, Cmp >::value_type& AVLTree< Val, Cmp >::insert_(AVLNode* new_node) {
    // if the tree is empty, just create a new node
    if (root_node_ == nullptr) {
      new_node->parent = nullptr;
      root_node_       = new_node;
      lowest_node_     = root_node_;
      highest_node_    = root_node_;
      ++nb_elements_;
      return new_node->value;
    }

    // here, the tree is not empty, so add the new node as a leaf without
    // balancing the tree for the moment
    const Val& value       = new_node->value;
    AVLNode*   node        = root_node_;
    AVLNode*   parent_node = nullptr;
    while (node != nullptr) {
      parent_node = node;
      node        = cmp_(value, node->value) ? node->left_child : node->right_child;
    }

    // here, parent_node should be the parent of our new leaf
    new_node->parent = parent_node;
    if (cmp_(new_node->value, parent_node->value)) {
      parent_node->left_child = new_node;
      if (lowest_node_ == parent_node) lowest_node_ = new_node;
    } else {
      parent_node->right_child = new_node;
      if (highest_node_ == parent_node) highest_node_ = new_node;
    }
    ++nb_elements_;

    // update the parent node and rebalance the tree
    rebalanceTree_(parent_node);
    return new_node->value;
  }


  /// adds (by move) a new element into the tree
  template < typename Val, typename Cmp >
  INLINE const typename AVLTree< Val, Cmp >::value_type&
     AVLTree< Val, Cmp >::insert(typename AVLTree< Val, Cmp >::value_type&& value) {
    return insert_(new AVLNode(std::move(value)));
  }


  /// adds (by copy) a new element into the tree
  template < typename Val, typename Cmp >
  INLINE const typename AVLTree< Val, Cmp >::value_type&
     AVLTree< Val, Cmp >::insert(const typename AVLTree< Val, Cmp >::value_type& val) {
    return insert_(new AVLNode(val));
  }


  /// emplace a new element into the tree
  template < typename Val, typename Cmp >
  template < typename... Args >
  INLINE const typename AVLTree< Val, Cmp >::value_type&
     AVLTree< Val, Cmp >::emplace(Args&&... args) {
    return insert_(new AVLNode(AVLNode::Emplace::EMPLACE, std::forward< Args >(args)...));
  }


  /// remove a node from the tree
  template < typename Val, typename Cmp >
  typename AVLTree< Val, Cmp >::AVLNode* AVLTree< Val, Cmp >::removeNodeFromTree_(AVLNode* node) {
    // if val cannot be found, do nothing
    if (node == nullptr) return nullptr;

    // here, node contains the value to be removed
    AVLNode* parent_node = node->parent;

    // if node has exactly two children, swap node with its successor in the right
    // subtree, i.e., the leftmost leaf in the right subtree. This one is guaranteed
    // to have at most only one child (the right one)
    if ((node->left_child != nullptr) && (node->right_child != nullptr)) {
      // find the successor
      AVLNode* successor = node->right_child;
      while (successor->left_child != nullptr)
        successor = successor->left_child;

      // remove the successor from the tree: its right child, if any, should now
      // be the child of the parent of successor
      AVLNode* successor_parent = successor->parent;
      if (successor_parent->left_child == successor) {
        // here, successor is not the right child of node
        successor_parent->left_child = successor->right_child;
      } else {
        // here, successor is the right child of node
        successor_parent->right_child = successor->right_child;
      }
      if (successor->right_child != nullptr) successor->right_child->parent = successor_parent;

      // now, remove node from the tree: to do so, just swap node and its successor
      AVLNode *removed_node, *kept_node;
      if (owns_nodes_) {
        // here, we perform the swap just by swapping the contents of the nodes
        std::swap(node->value, successor->value);

        // keep track of the node that will be freed from memory and the one remaining
        removed_node = successor;
        kept_node    = node;
      } else {
        // here, we perform the swap by updating the parents/children of the nodes
        successor->parent = node->parent;
        if (node->parent != nullptr) {
          if (node->parent->right_child == node) node->parent->right_child = successor;
          else node->parent->left_child = successor;
        }
        successor->right_child = node->right_child;
        if (node->right_child != nullptr) node->right_child->parent = successor;
        successor->left_child = node->left_child;
        if (node->left_child != nullptr) node->left_child->parent = successor;

        // keep track of the node that will be freed from memory and the one remaining
        removed_node = node;
        kept_node    = successor;
      }
      --nb_elements_;

      // rebalance the tree (it also recomputes the root node)
      // here, we should rebalance from the parent that successor had before we removed
      // node, i.e., from successor_parent. However, if this parent was equal to node,
      // we cannot do this since node has been removed from the tree. In such a case,
      // this node has been substituted by kept_node. Hence we should rebalance the
      // tree from kept_node
      rebalanceTree_(successor_parent != node ? successor_parent : kept_node);

      // if the successor was the highest node, we must update the highest_node_ field
      // so that it still points to a node containing the value that successor had
      // before we removed node. This corresponds precisely to kept_node. Note that
      // there is no need to update the lowest node because we did not change the
      // left subtree of node
      if (highest_node_ == successor) { highest_node_ = kept_node; }

      // if there are safe iterators, update their content:
      //   a1/ if their node_ field points to node, then make them point on nullptr
      //   a2/ if their node_ field points to successor, make them point to kept_node
      //   b1/ if their next_node_ points to node, make them point to kept_node
      //   b2/ if their next_node_ points to successor, make them point to the new
      //       successor of the kept_node
      //   c1/ if their preceding_node_ points to node, make them point to the new
      //       predecessor of kept_node
      //   c2/ if their preceding_node_ points to successor, make them point to kept_node
      if (!safe_iterators_.empty()) {
        AVLNode *new_predecessor = nullptr, *new_successor = nullptr;
        for (auto iter: safe_iterators_) {
          if (iter->node_ == node) {   // cases a1/ and b2/
            // here, compute once and for all, the new successor for b2/
            if (new_successor == nullptr) { new_successor = iter->nextNode_(kept_node); }
            iter->node_      = nullptr;
            iter->next_node_ = new_successor;
          } else if (iter->node_ == successor) {   // cases a2/ and c1/
            // here, compute once and for all, the new predecessor for c1/
            if (new_predecessor == nullptr) { new_predecessor = iter->precedingNode_(kept_node); }
            iter->node_           = kept_node;
            iter->preceding_node_ = new_predecessor;
          } else if (iter->next_node_ == node) {   // case b1/
            iter->next_node_ = kept_node;
          } else if (iter->preceding_node_ == successor) {   // case c2/
            iter->preceding_node_ = kept_node;
          }
        }
      }

      return removed_node;
    }

    // here, node contains at most one child
    AVLNode* child = node->left_child == nullptr ? node->right_child : node->left_child;

    // if there are safe iterators, update their content:
    // * here, if the node_ field points to node, make it point to nullptr
    // * if preceding_node_ points to node, make it point to the predecessor
    //   of node
    // * if next_node_ points to node, make it point to the new successor
    //   of node
    if (!safe_iterators_.empty()) {
      AVLNode *new_predecessor = nullptr, *new_successor = nullptr;
      for (auto iter: safe_iterators_) {
        if (iter->node_ == node) {
          iter->node_ = nullptr;
        } else if (iter->preceding_node_ == node) {
          if (new_predecessor == nullptr) { new_predecessor = iter->precedingNode_(node); }
          iter->preceding_node_ = new_predecessor;
        } else if (iter->next_node_ == node) {
          if (new_successor == nullptr) { new_successor = iter->nextNode_(node); }
          iter->next_node_ = new_successor;
        }
      }
    }

    if (child == nullptr) {   // here, node has no children
      // simply remove node and indicate to its parent that node disappeared
      if (parent_node != nullptr) {
        if (parent_node->left_child == node) {
          parent_node->left_child = nullptr;
          if (node == lowest_node_) lowest_node_ = parent_node;
        } else {
          parent_node->right_child = nullptr;
          if (node == highest_node_) highest_node_ = parent_node;
        }

        --nb_elements_;
      } else {
        // here, the parent dos not exist. So, the tree becomes empty
        root_node_    = nullptr;
        lowest_node_  = nullptr;
        highest_node_ = nullptr;

        --nb_elements_;
        return node;
      }
    } else {   // here, node has precisely one child
      // so substitute node by its child in the tree
      --nb_elements_;

      if (parent_node != nullptr) {
        if (parent_node->left_child == node) {
          parent_node->left_child = child;
          child->parent           = parent_node;
          if (node == lowest_node_) {
            // if node is the lowest node of the tree, it has no left child. But
            // since, here, it has a child, this one must be a right child. This
            // child cannot have a left child, else the tree would have been
            // imbalanced (because node would have a height of 0 on the left and
            // of at least 2 on the right). Therefore, child is now the lowest node
            lowest_node_ = child;
          }
        } else {
          parent_node->right_child = child;
          child->parent            = parent_node;
          if (node == highest_node_) highest_node_ = child;
        }
      } else {
        // here, the parent does not exist. Hence, the new root node should
        // be child. But we may need to recompute the lowest or the highest node
        root_node_    = child;
        child->parent = nullptr;

        if (node == lowest_node_) {
          // here, necessarily, child was the right child of node. We should
          // therefore find the leftmost leaf of child's subtree
          lowest_node_ = child;
          while (lowest_node_->left_child != nullptr)
            lowest_node_ = lowest_node_->left_child;
        }
        if (node == highest_node_) {
          highest_node_ = child;
          while (highest_node_->right_child != nullptr)
            highest_node_ = highest_node_->right_child;
        }

        // no need to rebalance the tree
        return node;
      }
    }

    rebalanceTree_(parent_node);
    return node;
  }


  /// remove a node from the tree and free memory
  template < typename Val, typename Cmp >
  INLINE void AVLTree< Val, Cmp >::erase_(AVLNode* node) {
    AVLNode* removed_node = removeNodeFromTree_(node);
    if (removed_node != nullptr) delete removed_node;
  }


  /// remove an element from the tree
  template < typename Val, typename Cmp >
  void AVLTree< Val, Cmp >::erase(const value_type& val) {
    // find a node in which val is located
    AVLNode* node = root_node_;
    while (node != nullptr) {
      if (node->value == val) break;
      node = cmp_(val, node->value) ? node->left_child : node->right_child;
    }
    erase_(node);
  }


  /// remove from the tree an element pointed to by an iterator
  template < typename Val, typename Cmp >
  INLINE void AVLTree< Val, Cmp >::erase(typename AVLTree< Val, Cmp >::iterator_safe& iter) {
    erase_(iter.node_);
  }


  /// remove from the tree an element pointed to by an iterator
  template < typename Val, typename Cmp >
  INLINE void
     AVLTree< Val, Cmp >::erase(typename AVLTree< Val, Cmp >::reverse_iterator_safe& iter) {
    erase_(iter.node_);
  }


  /// remove all the elements in the tree
  template < typename Val, typename Cmp >
  void AVLTree< Val, Cmp >::clear() {
    // remove the elements from memory
    if (owns_nodes_) deleteSubtree_(root_node_);
    root_node_    = nullptr;
    lowest_node_  = nullptr;
    highest_node_ = nullptr;
    nb_elements_  = Size(0);

    // make all the safe iterators point to end/rend
    for (auto iter: safe_iterators_) {
      iter->pointToEndRend_();
    }
  }


  /// returns a new iterator pointing to the minimal element of the tree
  template < typename Val, typename Cmp >
  INLINE typename AVLTree< Val, Cmp >::iterator AVLTree< Val, Cmp >::begin() const {
    return AVLTreeIterator(*this);
  }

  /// returns an iterator pointing just after the maximal element
  template < typename Val, typename Cmp >
  constexpr const typename AVLTree< Val, Cmp >::iterator& AVLTree< Val, Cmp >::end() const {
    return *(reinterpret_cast< const iterator* >(_AVLTree_end_));
  }

  /// returns a new iterator pointing to the maximal element of the tree
  template < typename Val, typename Cmp >
  INLINE typename AVLTree< Val, Cmp >::reverse_iterator AVLTree< Val, Cmp >::rbegin() const {
    return AVLTreeReverseIterator(*this, true);
  }

  /// returns an iterator pointing just before the minimal element
  template < typename Val, typename Cmp >
  constexpr const typename AVLTree< Val, Cmp >::reverse_iterator&
     AVLTree< Val, Cmp >::rend() const {
    return *(reinterpret_cast< const reverse_iterator* >(_AVLTree_rend_));
  }

  /// returns a new safe iterator pointing to the minimal element of the tree
  template < typename Val, typename Cmp >
  INLINE typename AVLTree< Val, Cmp >::iterator_safe AVLTree< Val, Cmp >::beginSafe() {
    return AVLTreeIteratorSafe(*this);
  }

  /// returns a safe iterator pointing just after the maximal element
  template < typename Val, typename Cmp >
  constexpr const typename AVLTree< Val, Cmp >::iterator_safe&
     AVLTree< Val, Cmp >::endSafe() const {
    return *(reinterpret_cast< const iterator_safe* >(_AVLTree_end_safe_));
  }

  /// returns a new safe iterator pointing to the maximal element of the tree
  template < typename Val, typename Cmp >
  INLINE typename AVLTree< Val, Cmp >::reverse_iterator_safe AVLTree< Val, Cmp >::rbeginSafe() {
    return AVLTreeReverseIteratorSafe(*this, true);
  }

  /// returns a safe iterator pointing just before the minimal element
  template < typename Val, typename Cmp >
  constexpr const typename AVLTree< Val, Cmp >::reverse_iterator_safe&
     AVLTree< Val, Cmp >::rendSafe() const {
    return *(reinterpret_cast< const reverse_iterator_safe* >(_AVLTree_rend_safe_));
  }


  /// register a new safe iterator
  template < typename Val, typename Cmp >
  INLINE void
     AVLTree< Val, Cmp >::insertIntoSafeList_(typename AVLTree< Val, Cmp >::iterator_safe* iter) {
    safe_iterators_.push_back(iter);
  }


  /// unregister a safe iterator
  template < typename Val, typename Cmp >
  INLINE void
     AVLTree< Val, Cmp >::removeFromSafeList_(typename AVLTree< Val, Cmp >::iterator_safe* iter) {
    const Size len = safe_iterators_.size();
    for (Size i = Size(0); i < len; ++i) {
      if (safe_iterators_[i] == iter) {
        safe_iterators_[i] = safe_iterators_[len - 1];
        safe_iterators_.pop_back();
        return;
      }
    }
  }


  /// returns a string with the content of the tree, order from lowest to highest
  template < typename Val, typename Cmp >
  std::string AVLTree< Val, Cmp >::toString() const {
    std::stringstream str;
    str << '{';
    bool first = true;
    for (const auto& val: *this) {
      if (!first) str << " , ";
      else first = false;
      str << val;
    }
    str << '}';
    return str.str();
  }

  /// ======================================================================================

  /// move to the next element in the tree
  template < typename Val, typename Cmp >
  typename AVLTreeIterator< Val, Cmp >::AVLNode*
     AVLTreeIterator< Val, Cmp >::nextNode_(AVLNode* node) const noexcept {
    if (node != nullptr) {
      // here, the iterator points toward an element of the tree

      if (node->right_child != nullptr) {
        // here, node has a right child, hence the next element is the leftmost
        // leaf of this child
        AVLNode* next_node = node->right_child;
        while (next_node->left_child != nullptr)
          next_node = next_node->left_child;
        return next_node;
      } else {
        // if node is the highest node of the tree, we go to the end iterator, else
        // we are guaranteed that node has a successor
        if (node == tree_->highest_node_) { return nullptr; }

        // if node has no right child, we need to move up int the tree until we get a
        // node that is on a right upper edge. This is our next node
        AVLNode* current_node = node;
        AVLNode* next_node    = node->parent;

        while (next_node->right_child == current_node) {
          current_node = next_node;
          next_node    = next_node->parent;
        }

        return next_node;
      }
    } else {
      return nullptr;
    }
  }


  /// move to the preceding element in the tree
  template < typename Val, typename Cmp >
  typename AVLTreeIterator< Val, Cmp >::AVLNode*
     AVLTreeIterator< Val, Cmp >::precedingNode_(AVLNode* node) const noexcept {
    if (node != nullptr) {
      // here, the iterator points toward an element of the tree

      if (node->left_child != nullptr) {
        // here, node has a left child, hence the next element is the rightmost
        // leaf of this child
        AVLNode* next_node = node->left_child;
        while (next_node->right_child != nullptr)
          next_node = next_node->right_child;
        return next_node;
      } else {
        // if node is the lowest node of the tree, we go to the rend iterator, else
        // we are guaranteed that node has a predecessor
        if (node == tree_->lowest_node_) { return nullptr; }

        // if node has no left child, we need to move up int the tree until we get a
        // node that is on a left upper edge. This is our next node
        AVLNode* current_node = node;
        AVLNode* next_node    = node->parent;

        while (next_node->left_child == current_node) {
          current_node = next_node;
          next_node    = next_node->parent;
        }

        return next_node;
      }
    } else {
      return nullptr;
    }
  }


  /// constructor for begin iterators
  template < typename Val, typename Cmp >
  INLINE AVLTreeIterator< Val, Cmp >::AVLTreeIterator(const AVLTree< Val, Cmp >& tree,
                                                      const bool                 begin) noexcept :
      tree_(const_cast< AVLTree< Val, Cmp >* >(&tree)),
      node_(begin ? tree.lowest_node_ : tree.highest_node_) {
    next_node_      = nextNode_(node_);
    preceding_node_ = precedingNode_(node_);

    GUM_CONSTRUCTOR(AVLTreeIterator)
  }


  /// copy constructor
  template < typename Val, typename Cmp >
  INLINE
     AVLTreeIterator< Val, Cmp >::AVLTreeIterator(const AVLTreeIterator< Val, Cmp >& from) noexcept
      :
      tree_(from.tree_),
      node_(from.node_), next_node_(from.next_node_), preceding_node_(from.preceding_node_) {
    GUM_CONS_CPY(AVLTreeIterator)
  }


  /// move constructor
  template < typename Val, typename Cmp >
  INLINE AVLTreeIterator< Val, Cmp >::AVLTreeIterator(AVLTreeIterator< Val, Cmp >&& from) noexcept :
      tree_(from.tree_), node_(from.node_), next_node_(from.next_node_),
      preceding_node_(from.preceding_node_) {
    GUM_CONS_MOV(AVLTreeIterator)
  }


  /// destructor
  template < typename Val, typename Cmp >
  INLINE AVLTreeIterator< Val, Cmp >::~AVLTreeIterator() noexcept {
    GUM_DESTRUCTOR(AVLTreeIterator);
  }


  /// copy operator
  template < typename Val, typename Cmp >
  INLINE AVLTreeIterator< Val, Cmp >&
     AVLTreeIterator< Val, Cmp >::operator=(const AVLTreeIterator< Val, Cmp >& from) noexcept {
    tree_           = from.tree_;
    node_           = from.node_;
    next_node_      = from.next_node_;
    preceding_node_ = from.preceding_node_;
    return *this;
  }


  /// move operator
  template < typename Val, typename Cmp >
  INLINE AVLTreeIterator< Val, Cmp >&
     AVLTreeIterator< Val, Cmp >::operator=(AVLTreeIterator< Val, Cmp >&& from) noexcept {
    tree_           = from.tree_;
    node_           = from.node_;
    next_node_      = from.next_node_;
    preceding_node_ = from.preceding_node_;
    return *this;
  }


  /// indicates whether two iterator point to the same element
  template < typename Val, typename Cmp >
  INLINE bool
     AVLTreeIterator< Val, Cmp >::operator==(const AVLTreeIterator< Val, Cmp >& from) const {
    // when node_ is different from nullptr, testing whether "this" is equal to from
    // simply amounts to comparing their node_ fields. However, due to erasures in
    // the tree, it may happen that two iterators pointing to different nodes have
    // a nullptr node_ field. In this case, they will be equal if and only if their
    // next_node_ fields are equal. Here, it is important to use the next_node_
    // rather than their preceding_node_ field because the end iterator has nullptr
    // as the value of its preceding_node_ while an iterator moving by ++ operators
    // up to the end will not have this value for its preceding_node_. However, it
    // will have a next_node_ equal to nullptr, exactly as the end iterator.
    return (node_ == from.node_) && (next_node_ == from.next_node_);
  }


  /// indicates whether two iterator point to different element
  template < typename Val, typename Cmp >
  INLINE bool
     AVLTreeIterator< Val, Cmp >::operator!=(const AVLTreeIterator< Val, Cmp >& from) const {
    // for the reason of this or test, see operator== above.
    return (node_ != from.node_) || (next_node_ != from.next_node_);
  }


  /// move to the next element in the tree
  template < typename Val, typename Cmp >
  INLINE AVLTreeIterator< Val, Cmp >& AVLTreeIterator< Val, Cmp >::operator++() noexcept {
    preceding_node_ = node_;
    node_           = next_node_;
    next_node_      = nextNode_(node_);
    return *this;
  }


  /// move to the next k element
  template < typename Val, typename Cmp >
  INLINE AVLTreeIterator< Val, Cmp >&
     AVLTreeIterator< Val, Cmp >::operator+=(const Size k) noexcept {
    for (Size i = 0; i < k; ++i) {
      AVLTreeIterator< Val, Cmp >::operator++();
    }
    return *this;
  }


  /// move to the preceding element in the tree
  template < typename Val, typename Cmp >
  INLINE AVLTreeIterator< Val, Cmp >& AVLTreeIterator< Val, Cmp >::operator--() noexcept {
    next_node_      = node_;
    node_           = preceding_node_;
    preceding_node_ = precedingNode_(node_);
    return *this;
  }


  /// move to the preceding k element
  template < typename Val, typename Cmp >
  INLINE AVLTreeIterator< Val, Cmp >&
     AVLTreeIterator< Val, Cmp >::operator-=(const Size k) noexcept {
    for (Size i = 0; i < k; ++i) {
      AVLTreeIterator< Val, Cmp >::operator--();
    }
    return *this;
  }


  /// make the iterator point to nothing
  template < typename Val, typename Cmp >
  INLINE void AVLTreeIterator< Val, Cmp >::unregisterTree_() noexcept {
    tree_           = nullptr;
    node_           = nullptr;
    preceding_node_ = nullptr;
    next_node_      = nullptr;
  }


  /// force the iterator to point to end/rend
  template < typename Val, typename Cmp >
  INLINE void AVLTreeIterator< Val, Cmp >::pointToEndRend_() noexcept {
    node_           = nullptr;
    preceding_node_ = nullptr;
    next_node_      = nullptr;
  }


  /// returns the element pointed to by the iterator
  template < typename Val, typename Cmp >
  INLINE typename AVLTreeIterator< Val, Cmp >::const_reference
     AVLTreeIterator< Val, Cmp >::operator*() const {
    if (node_ != nullptr) return node_->value;
    else {
      if ((next_node_ == nullptr) || (preceding_node_ == nullptr)) {
        GUM_ERROR(NotFound, "an end/rend AVLTree iterator does not contain any value")
      } else {
        GUM_ERROR(NotFound, "the AVLTree iterator points to an erased value")
      }
    }
  }

  /// ======================================================================================

  /// constructor for begin iterators
  template < typename Val, typename Cmp >
  INLINE AVLTreeIteratorSafe< Val, Cmp >::AVLTreeIteratorSafe(AVLTree< Val, Cmp >& tree,
                                                              const bool           rbegin) :
      AVLTreeIterator< Val, Cmp >(tree, rbegin) {
    tree.insertIntoSafeList_(this);
    GUM_CONSTRUCTOR(AVLTreeIteratorSafe)
  }


  /// copy constructor
  template < typename Val, typename Cmp >
  INLINE AVLTreeIteratorSafe< Val, Cmp >::AVLTreeIteratorSafe(
     const AVLTreeIteratorSafe< Val, Cmp >& from) :
      AVLTreeIterator< Val, Cmp >(from) {
    if (this->tree_ != nullptr) this->tree_->insertIntoSafeList_(this);
    GUM_CONS_CPY(AVLTreeIteratorSafe)
  }


  /// move constructor
  template < typename Val, typename Cmp >
  INLINE
     AVLTreeIteratorSafe< Val, Cmp >::AVLTreeIteratorSafe(AVLTreeIteratorSafe< Val, Cmp >&& from) :
      AVLTreeIterator< Val, Cmp >(std::move(from)) {
    if (this->tree_ != nullptr) {
      this->tree_->insertIntoSafeList_(this);
      this->tree_->removeFromSafeList_(&from);
    }
    GUM_CONS_MOV(AVLTreeIteratorSafe)
  }


  /// destructor
  template < typename Val, typename Cmp >
  INLINE AVLTreeIteratorSafe< Val, Cmp >::~AVLTreeIteratorSafe() noexcept {
    if (this->tree_ != nullptr) { this->tree_->removeFromSafeList_(this); }
    GUM_DESTRUCTOR(AVLTreeIteratorSafe)
  }


  /// copy operator
  template < typename Val, typename Cmp >
  INLINE AVLTreeIteratorSafe< Val, Cmp >&
     AVLTreeIteratorSafe< Val, Cmp >::operator=(const AVLTreeIteratorSafe< Val, Cmp >& from) {
    if (this != &from) {
      if (from.tree_ != this->tree_) {
        if (this->tree_ != nullptr) { this->tree_->removeFromSafeList_(this); }
        if (from.tree_ != nullptr) { from.tree_->insertIntoSafeList_(this); }
      }
      AVLTreeIterator< Val, Cmp >::operator=(from);
    }
    return *this;
  }


  /// move operator
  template < typename Val, typename Cmp >
  INLINE AVLTreeIteratorSafe< Val, Cmp >&
     AVLTreeIteratorSafe< Val, Cmp >::operator=(AVLTreeIteratorSafe< Val, Cmp >&& from) {
    if (this != &from) {
      if (from.tree_ != this->tree_) {
        if (this->tree_ != nullptr) { this->tree_->removeFromSafeList_(this); }
        if (from.tree_ != nullptr) { from.tree_->insertIntoSafeList_(this); }
      }
      AVLTreeIterator< Val, Cmp >::operator=(std::move(from));
    }
    return *this;
  }


  /// indicates whether two iterator point to the same element
  template < typename Val, typename Cmp >
  INLINE bool AVLTreeIteratorSafe< Val, Cmp >::operator==(
     const AVLTreeIteratorSafe< Val, Cmp >& from) const {
    return AVLTreeIterator< Val, Cmp >::operator==(from);
  }


  /// indicates whether two iterator point to different element
  template < typename Val, typename Cmp >
  INLINE bool AVLTreeIteratorSafe< Val, Cmp >::operator!=(
     const AVLTreeIteratorSafe< Val, Cmp >& from) const {
    return AVLTreeIterator< Val, Cmp >::operator!=(from);
  }


  /// move to the next element in the tree
  template < typename Val, typename Cmp >
  INLINE AVLTreeIteratorSafe< Val, Cmp >& AVLTreeIteratorSafe< Val, Cmp >::operator++() noexcept {
    AVLTreeIterator< Val, Cmp >::operator++();
    return *this;
  }


  /// move to the next k elements
  template < typename Val, typename Cmp >
  INLINE AVLTreeIteratorSafe< Val, Cmp >&
     AVLTreeIteratorSafe< Val, Cmp >::operator+=(const Size k) noexcept {
    AVLTreeIterator< Val, Cmp >::operator+=(k);
    return *this;
  }

  /// move to the preceding element in the tree
  template < typename Val, typename Cmp >
  INLINE AVLTreeIteratorSafe< Val, Cmp >& AVLTreeIteratorSafe< Val, Cmp >::operator--() noexcept {
    AVLTreeIterator< Val, Cmp >::operator--();
    return *this;
  }


  /// move to the preceding k element
  template < typename Val, typename Cmp >
  INLINE AVLTreeIteratorSafe< Val, Cmp >&
     AVLTreeIteratorSafe< Val, Cmp >::operator-=(const Size k) noexcept {
    AVLTreeIterator< Val, Cmp >::operator-=(k);
    return *this;
  }

  /// ======================================================================================

  /// constructor for rbegin iterators
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIterator< Val, Cmp >::AVLTreeReverseIterator(const AVLTree< Val, Cmp >& tree,
                                                                    const bool rbegin) noexcept :
      AVLTreeIterator< Val, Cmp >(tree, !rbegin) {
    GUM_CONSTRUCTOR(AVLTreeReverseIterator)
  }


  /// copy constructor
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIterator< Val, Cmp >::AVLTreeReverseIterator(
     const AVLTreeReverseIterator< Val, Cmp >& from) noexcept :
      AVLTreeIterator< Val, Cmp >(from) {
    GUM_CONS_CPY(AVLTreeReverseIterator)
  }


  /// move constructor
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIterator< Val, Cmp >::AVLTreeReverseIterator(
     AVLTreeReverseIterator< Val, Cmp >&& from) noexcept :
      AVLTreeIterator< Val, Cmp >(std::move(from)) {
    GUM_CONS_MOV(AVLTreeReverseIterator)
  }


  /// destructor
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIterator< Val, Cmp >::~AVLTreeReverseIterator() noexcept {
    GUM_DESTRUCTOR(AVLTreeReverseIterator)
  }


  /// copy operator
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIterator< Val, Cmp >& AVLTreeReverseIterator< Val, Cmp >::operator=(
     const AVLTreeReverseIterator< Val, Cmp >& from) noexcept {
    AVLTreeIterator< Val, Cmp >::operator=(from);
    return *this;
  }


  /// move operator
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIterator< Val, Cmp >& AVLTreeReverseIterator< Val, Cmp >::operator=(
     AVLTreeReverseIterator< Val, Cmp >&& from) noexcept {
    AVLTreeIterator< Val, Cmp >::operator=(std::move(from));
    return *this;
  }


  /// indicates whether two iterator point to the same element
  template < typename Val, typename Cmp >
  INLINE bool AVLTreeReverseIterator< Val, Cmp >::operator==(
     const AVLTreeReverseIterator< Val, Cmp >& from) const {
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
  INLINE bool AVLTreeReverseIterator< Val, Cmp >::operator!=(
     const AVLTreeReverseIterator< Val, Cmp >& from) const {
    // for the reason of this or test, see operator== above.
    return (this->node_ != from.node_) || (this->preceding_node_ != from.preceding_node_);
  }


  /// move to the next element in the tree
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIterator< Val, Cmp >&
     AVLTreeReverseIterator< Val, Cmp >::operator++() noexcept {
    AVLTreeIterator< Val, Cmp >::operator--();
    return *this;
  }


  /// move to the next k element
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIterator< Val, Cmp >&
     AVLTreeReverseIterator< Val, Cmp >::operator+=(const Size k) noexcept {
    for (Size i = 0; i < k; ++i) {
      AVLTreeReverseIterator< Val, Cmp >::operator++();
    }
    return *this;
  }


  /// move to the preceding element in the tree
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIterator< Val, Cmp >&
     AVLTreeReverseIterator< Val, Cmp >::operator--() noexcept {
    AVLTreeIterator< Val, Cmp >::operator++();
    return *this;
  }


  /// move to the preceding k element
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIterator< Val, Cmp >&
     AVLTreeReverseIterator< Val, Cmp >::operator-=(const Size k) noexcept {
    for (Size i = 0; i < k; ++i) {
      AVLTreeReverseIterator< Val, Cmp >::operator--();
    }
    return *this;
  }

  /// ======================================================================================

  /// constructor for rbegin iterators
  template < typename Val, typename Cmp >
  INLINE
     AVLTreeReverseIteratorSafe< Val, Cmp >::AVLTreeReverseIteratorSafe(AVLTree< Val, Cmp >& tree,
                                                                        const bool rbegin) :
      AVLTreeIteratorSafe< Val, Cmp >(tree, !rbegin) {
    GUM_CONSTRUCTOR(AVLTreeReverseIteratorSafe)
  }


  /// copy constructor
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIteratorSafe< Val, Cmp >::AVLTreeReverseIteratorSafe(
     const AVLTreeReverseIteratorSafe< Val, Cmp >& from) :
      AVLTreeIteratorSafe< Val, Cmp >(from) {
    GUM_CONS_CPY(AVLTreeReverseIteratorSafe)
  }


  /// move constructor
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIteratorSafe< Val, Cmp >::AVLTreeReverseIteratorSafe(
     AVLTreeReverseIteratorSafe< Val, Cmp >&& from) :
      AVLTreeIteratorSafe< Val, Cmp >(std::move(from)) {
    GUM_CONS_MOV(AVLTreeReverseIteratorSafe)
  }


  /// destructor
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIteratorSafe< Val, Cmp >::~AVLTreeReverseIteratorSafe() noexcept {
    GUM_DESTRUCTOR(AVLTreeReverseIteratorSafe)
  }


  /// copy operator
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIteratorSafe< Val, Cmp >& AVLTreeReverseIteratorSafe< Val, Cmp >::operator=(
     const AVLTreeReverseIteratorSafe< Val, Cmp >& from) {
    AVLTreeIteratorSafe< Val, Cmp >::operator=(from);
    return *this;
  }


  /// move operator
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIteratorSafe< Val, Cmp >& AVLTreeReverseIteratorSafe< Val, Cmp >::operator=(
     AVLTreeReverseIteratorSafe< Val, Cmp >&& from) {
    AVLTreeIteratorSafe< Val, Cmp >::operator=(std::move(from));
    return *this;
  }


  /// indicates whether two iterator point to the same element
  template < typename Val, typename Cmp >
  INLINE bool AVLTreeReverseIteratorSafe< Val, Cmp >::operator==(
     const AVLTreeReverseIteratorSafe< Val, Cmp >& from) const {
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
  INLINE bool AVLTreeReverseIteratorSafe< Val, Cmp >::operator!=(
     const AVLTreeReverseIteratorSafe< Val, Cmp >& from) const {
    // for the reason of this or test, see operator== above.
    return (this->node_ != from.node_) || (this->preceding_node_ != from.preceding_node_);
  }

  /// move to the next element in the tree
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIteratorSafe< Val, Cmp >&
     AVLTreeReverseIteratorSafe< Val, Cmp >::operator++() noexcept {
    AVLTreeIteratorSafe< Val, Cmp >::operator--();
    return *this;
  }


  /// move to the next k element
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIteratorSafe< Val, Cmp >&
     AVLTreeReverseIteratorSafe< Val, Cmp >::operator+=(const Size k) noexcept {
    for (Size i = 0; i < k; ++i) {
      AVLTreeReverseIteratorSafe< Val, Cmp >::operator++();
    }
    return *this;
  }


  /// move to the preceding element in the tree
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIteratorSafe< Val, Cmp >&
     AVLTreeReverseIteratorSafe< Val, Cmp >::operator--() noexcept {
    AVLTreeIteratorSafe< Val, Cmp >::operator++();
    return *this;
  }


  /// move to the preceding k element
  template < typename Val, typename Cmp >
  INLINE AVLTreeReverseIteratorSafe< Val, Cmp >&
     AVLTreeReverseIteratorSafe< Val, Cmp >::operator-=(const Size k) noexcept {
    for (Size i = 0; i < k; ++i) {
      AVLTreeReverseIteratorSafe< Val, Cmp >::operator--();
    }
    return *this;
  }


}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
