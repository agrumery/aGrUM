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
/**
 * @file
 * @brief AVL binary search trees.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_AVL_SEARCH_TREE_H
#define GUM_AVL_SEARCH_TREE_H

#include <agrum/config.h>

#include <agrum/core/binSearchTree.h>
#include <agrum/core/binTreeNode.h>
#include <agrum/core/binTreeNodeAVL.h>

namespace gum {

  // ===========================================================================
  // ===========================================================================
  // ===                        AVL BINARY SEARCH TREE                       ===
  // ===========================================================================
  // ===========================================================================

  /**
   * @class AVLSearchTree AVLSearchTree.h <agrum/core/AVLSearchTree.h>
   * @brief A generic binary search tree.
   * @ingroup basicstruct_group
   *
   * @tparam Val The type of the values stored in the AVLSearchTree.
   * @tparam Cmp The comparator used to balance the AVLSearchTree.
   */
  template <typename Val, class Cmp = std::less<Val>>
  class AVLSearchTree : public BinSearchTree<Val, Cmp, BinTreeNodeAVL<Val>> {

    public:
    typedef BinTreeNodeAVL<Val> Node;
    typedef BinSearchTreeIterator<Val, Cmp, Node> iterator;

    // ============================================================================
    /// @name Constructors and Destructor
    // ============================================================================
    /// @{


    /**
     * @brief Basic constructor: returns an empty binary search tree.
     * @param uniqueness_policy Enable (false) or disables (true) multiple
     * instances of the same value within the tree.
     */
    explicit AVLSearchTree( bool uniqueness_policy = false );

    /**
     * @brief Copy constructor.
     * @param from The AVLSearchTree from which the copy is made.
     */
    AVLSearchTree( const AVLSearchTree<Val, Cmp>& from );

    /**
     * @brief Destructor.
     */
    ~AVLSearchTree();

    ///@}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    ///@{

    /**
     * @brief Copy operator.
     * @param from The AVLSearchTree from which the copy is made.
     * @return Returns the AVLSearchTree in which from was copied.
     */
    AVLSearchTree<Val, Cmp>& operator=( const AVLSearchTree<Val, Cmp>& from );

    ///@}
    // ============================================================================
    /// @name iterators
    // ============================================================================
    ///@{

    using BinSearchTree<Val, Cmp, Node>::begin;
    using BinSearchTree<Val, Cmp, Node>::rbegin;
    using BinSearchTree<Val, Cmp, Node>::end;
    using BinSearchTree<Val, Cmp, Node>::rend;

    ///@}

    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    using BinSearchTree<Val, Cmp, Node>::rootValue;
    using BinSearchTree<Val, Cmp, Node>::minValue;
    using BinSearchTree<Val, Cmp, Node>::maxValue;
    using BinSearchTree<Val, Cmp, Node>::contains;
    using BinSearchTree<Val, Cmp, Node>::empty;
    using BinSearchTree<Val, Cmp, Node>::size;
    using BinSearchTree<Val, Cmp, Node>::clear;
    using BinSearchTree<Val, Cmp, Node>::toString;
    using BinSearchTree<Val, Cmp, Node>::insert;
    using BinSearchTree<Val, Cmp, Node>::erase;

    /**
     * @brief Check the tree to determine whether it is actually AVL balanced.
     * @return Returnes true if the tree is balanced.
     */
    bool checkAVLStructure() const;

    /// @}
    // ============================================================================
    /// @name Fine tuning
    // ============================================================================
    /// @{

    using BinSearchTree<Val, Cmp, Node>::uniquenessPolicy;
    using BinSearchTree<Val, Cmp, Node>::setUniquenessPolicy;

    /// @}

    /**
     * @brief Friendly display of the AVLSearchTree.
     * @return Returns a representation of the AVLSearchTree.
     */
    const std::string toString() const;

    protected:
    /**
     * @brief Erase the node passed in argument.
     * @param node The node to be removed.
     */
    void _erase( Node* node );

    /**
     * @brief Creates a copy of the value, insert it in the tree and returns
     * the node holding it in the tree.
     *
     * Elements are inserted by copy in the binary search tree.  Thus, the
     * method returns the node containing the newly created copy, so that the
     * user may reference it.
     *
     * @param val The value inserted by copy in the AVLSearchTree.
     * @return Returns a pointer over the node holding a copy of val in the
     * AVLSearchTree.
     * @warning This method is actually the implementation of the inster
     * method. It is used to speed-up insertions in terminal classes such as
     * AVL trees.
     * @throw DuplicateElement exception is raised if the binary tree already
     * contains the value and the uniqueness property is set to true
     */
    Node* _insert( const Val& val );
  };

  /**
   * @brief For friendly displaying of AVLSearchTree content.
   * @param stream The stream in which to print the AVLSearchTree content.
   * @param avl The AVLSearchTree to print.
   * @return Returns stream.
   */
  template <typename Val, class Cmp>
  std::ostream& operator<<( std::ostream& stream,
                            const AVLSearchTree<Val, Cmp>& avl );

} /* namespace gum */


extern template class gum::AVLSearchTree<int>;


// always include the template implementations
#include <agrum/core/AVLSearchTree_tpl.h>

#endif /* GUM_AVL_SEARCH_TREE_H */
