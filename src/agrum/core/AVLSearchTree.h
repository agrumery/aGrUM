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

#include <agrum/core/binTreeNode.h>
#include <agrum/core/binSearchTree.h>
#include <agrum/core/binTreeNode4AVL.h>

namespace gum {

  // ===========================================================================
  // ===========================================================================
  // ===                        AVL BINARY SEARCH TREE                       ===
  // ===========================================================================
  // ===========================================================================
  /**
   * @class AVLSearchTree <AVLSearchTree.h> <agrum/core/AVLSearchTree.h>
   * @ingroup basicstruct_group
   * @brief generic binary search trees
   */
  template <typename Val, class Cmp = std::less<Val>>
  class AVLSearchTree : public BinSearchTree<Val, Cmp, BinTreeNode4AVL<Val>> {
    public:
    typedef BinTreeNode4AVL<Val> Node;
    typedef BinSearchTreeIterator<Val, Cmp, Node> iterator;

    // ############################################################################
    /// @name constructors / destructors
    // ############################################################################
    ///@{

    /// basic constructor: returns an empty binary search tree
    /** @param uniqueness_policy allows (false) or disables (true) the
     * possibility
     * for the binary tree to have multiple instances of the same value within
     * the tree. */

    explicit AVLSearchTree( bool uniqueness_policy = false );

    /// copy constructor

    AVLSearchTree( const AVLSearchTree<Val, Cmp>& from );

    /// destructor

    ~AVLSearchTree();

    ///@}

    // ############################################################################
    /// @name operators
    // ############################################################################
    ///@{

    /// copy operator

    AVLSearchTree<Val, Cmp>& operator=( const AVLSearchTree<Val, Cmp>& from );

    ///@}

    // ############################################################################
    /// @name iterators
    // ############################################################################
    ///@{
    using BinSearchTree<Val, Cmp, Node>::begin;
    using BinSearchTree<Val, Cmp, Node>::rbegin;
    using BinSearchTree<Val, Cmp, Node>::end;
    using BinSearchTree<Val, Cmp, Node>::rend;
    ///@}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
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

    /// check the tree to determine whether it is actually AVL balanced

    bool checkAVLStructure() const;

    /// @}

    // ############################################################################
    /// @name Fine tuning
    // ############################################################################
    /// @{
    using BinSearchTree<Val, Cmp, Node>::uniquenessPolicy;
    using BinSearchTree<Val, Cmp, Node>::setUniquenessPolicy;
    /// @}

    /// friendly displays the content of the CliqueGraph

    const std::string toString() const;

    protected:
    /// erase the node passed in argument

    void _erase( Node* node );

    /** @brief creates a copy of the value, insert it in the tree and returns
     * the copy value
     *
     * When elements are inserted into binary search trees, this is actually
     *copies
     * that are inserted. Thus, the method returns the node containing the newly
     * created copy, so that the user may reference the new copy.
     * @warning this method is actually the implementation of method insert. It
     * is used to speed-up insertions in terminal classes such as AVL trees.
     * @throw DuplicateElement exception is raised if the binary tree already
     * contains the value and the uniqueness property is set to true */

    Node* _insert( const Val& val );
  };

  /// for friendly displaying the content of AVLSearchTree
  template <typename Val, class Cmp>
  std::ostream& operator<<( std::ostream&, const AVLSearchTree<Val, Cmp>& );

} /* namespace gum */

// always include the template implementations
#include <agrum/core/AVLSearchTree.tcc>

#endif /* GUM_AVL_SEARCH_TREE_H */
