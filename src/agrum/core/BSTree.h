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
/** @file
 * @brief classes for binary and AVL search trees
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_BSTREE_H
#define GUM_BSTREE_H

#include <agrum/core/utils.h>

namespace gum {

  /// the direction of a given edge in a binary tree
  enum BSTreeDir {
    BSTREE_LEFT,
    BSTREE_RIGHT
  };
  
  /// how the subtrees of a given AVL tree node are balanced
  enum BSTreeBalance {
    BSTREE_LEFT_BAL,
    BSTREE_BALANCED,
    BSTREE_RIGHT_BAL
  };
  
  /// the classes provided by this file
  template <typename Key, typename Val> class BSTree;
  template <typename Key, typename Val> class BSTreeNode;
  template <typename Key, typename Val> class BSTreeIterator;

  
  /* ============================================================================ */
  /* ===                 GENERIC NODE OF A BINARY SEARCH TREE                 === */
  /* ============================================================================ */
  /** @class BSTreeNode
   * @brief Nodes of a binary search tree 
   * @ingroup basicstruct_group
   */
  /* ============================================================================ */
  template <typename Key, typename Val> class BSTreeNode {
    /// the binary tree should have unrestricted access to its nodes
    friend class BSTree<Key,Val>;
    friend class BSTreeIterator<Key,Val>;


    
  protected:
    /// the key according to which the nodes are ordered in the binary tree
    Key& key;
    
    /// the value stored in a node of the tree
    Val& val;
    
    /// the height of the tree rooted at the current node
    long height;
    
    /// the parent of the node
    BSTreeNode<Key,Val> *parent;
    
    /// the direction to follow from the parent to reach the current node
    BSTreeDir parent_dir;
    
    /// the children of the current node
    BSTreeNode<Key,Val> *children[2];

    
    // ============================================================================
    /// a function to update the height values in all the ancestors of a node
    // ============================================================================
    void updateAncestorsHeight() {
      long h = height+1;
      BSTreeNode<Key,Val>* node = parent;
      
      for ( ; node; ++h, node = node->parent )
        node->height = std::max( h, node->height );
    }

  private:
    // ============================================================================
    /// copy constructor: used to prevent using a default constructor provided by C++
    /** copying a BSTree node would be meaningless as its parent and children would
     * still be linked to the original node, not to the current one. Hence the
     * system would be in an instable state. */
    // ============================================================================
    BSTreeNode( const BSTreeNode<Key,Val>& node ) {
      GUM_ERROR(OperationNotAllowed,"use of BSTreeNode copy constructor is forbidden");
    }

    // ==============================================================================
    /// copy operator: used to prevent using a default operator provided by C++
    /** copying a BSTree node would be meaningless as its parent and children would
     * still be linked to the original node, not to the current one. Hence the
     * system would be in an instable state. */
    // ==============================================================================
    BSTreeNode<Key,Val>& operator= ( const BSTreeNode<Key,Val>& node ) {
      GUM_ERROR(OperationNotAllowed,"use of BSTreeNode copy constructor is forbidden");
    }

  public:
    // ############################################################################
    // constructors / destructors
    // ############################################################################

    // ============================================================================
    /// basic constructor
    // ============================================================================
    BSTreeNode( const Key& k, const Val& v,
                   BSTreeNode<Key,Val> *par = 0,
                   BSTreeDir dir = BSTREE_LEFT ) :
      key( k ), val( v ), height( 1 ), parent( par ) {
      // for debugging purposes
      GUM_CONSTRUCTOR( BSTreeNode );
      // set the children
      children[0] = 0;
      children[1] = 0;
      // if there is a parent, link things properly and update
      // the supertrees' heights

      if ( parent ) {
        parent->children[dir] = this;
        parent_dir = dir;
        updateAncestorsHeight();
      }
    }

    // ============================================================================
    /// basic constructor
    // ============================================================================
    BSTreeNode( const BSTreeNode<Key,Val>& node,
                   BSTreeNode<Key,Val> *par=0,
                   BSTreeDir dir = BSTREE_LEFT ) :
      key( node.key ), val( node.val ), height( 1 ), parent( par ) {
      // for debugging purposes
      GUM_CONSTRUCTOR( BSTreeNode );
      // set the children
      children[0] = 0;
      children[1] = 0;
      // if there is a parent, link things properly
      
      if ( parent ) {
        par->children[dir] = this;
        parent_dir = dir;
        updateAncestorsHeight();
      }
    }

    // ============================================================================
    /// destructor
    // ============================================================================
    ~BSTreeNode() {
      // for debugging purposes
      DESTRUCTOR( BSTreeNode );
    }

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################

    /// @{
    // ============================================================================
    /// returns the value stored in a node of the binary search tree
    // ============================================================================
    Val& val() const {
      return val;
    }

    // ============================================================================
    /// returns the key stored in a node (key according to which the BST is ordered)
    // ============================================================================
    const Key& key() const {
      return key;
    }
    
    // ============================================================================
    /// returns the height of the subtree rooted at the current node
    // ============================================================================
    long getHeight() const {
      return height;
    }

    // ============================================================================
    /// returns the given child of a node
    /** @warning if the child does not exists, the method returns a 0 pointer. */
    // ============================================================================
    BSTreeNode<Key,Val>* getChildNode( BSTreeDir dir ) const {
      return children[dir];
    }

    // ============================================================================
    /// returns the parent of a node
    /** @warning if the parent does not exists, the method returns a 0 pointer. */
    // ============================================================================
    BSTreeNode<Key,Val>* getParentNode() const {
      return parent;
    }
    
    // ============================================================================
    /// returns the node with the minimal key in the subtree rooted at @e this
    /** @warning the method may return @e this if the latter has the smallest key
     * in the whole subtree */
    // ============================================================================
    BSTreeNode<Key,Val>* getMinNode() const {
      // return the leftmost element
      BSTreeNode<Key,Val>* node;
      
      for ( node = this; node->children[BSTREE_LEFT];
            node = node->children[BSTREE_LEFT] );

      return node;
    }

    // ============================================================================
    /// returns the node with the maximal key in the subtree rooted at @e this
    /** @warning the method may return @e this if the latter has the highest key
     * in the whole subtree */
    // ============================================================================
    BSTreeNode<Key,Val>* getMaxNode() const {
      // return the rightmost element
      BSTreeNode<Key,Val>*node;
      
      for ( node = this; node->children[BSTREE_RIGHT];
            node=node->children[BSTREE_RIGHT] );
      
      return node;
    }
    
    // ============================================================================
    /// returns the node with the highest key less than or equal to the current node
    /** if the current node is the leftmost node with the smallest key in the tree,
     * then it has no predecessor and the method returns 0 */
    // ============================================================================
    BSTreeNode<Key,Val>* getPrecNode() const {
      // if node has a left child, find the max in this subtree
      if ( children[BSTREE_LEFT] )
        return children[BSTREE_LEFT].getMaxNode();

      // else if it has no parent and no left child, there is no predecessor
      if ( parent == 0 )
        return 0;
      
      // here, the node has a parent. if it is on its left, then this is
      // the predecessor
      if ( parent_dir == BSTREE_RIGHT )
        return parent;

      // here the parent is on the right => go up in the tree until you find
      // a left parent. Then this is the predecessor
      BSTreeNode<Key,Val>* node = this;

      while ( node->parent && ( node->parent_dir == BSTREE_LEFT ) )
        node = node->parent;
      
      // if there is no more parent, then there is no predecessor
      // else the parent is the predecessor we look for
      return node->parent;
    }

    // ============================================================================
    /** @brief returns the node with the lowest key greater than or equal to the
     * current node
     *
     * if the current node is the rightmost node with the highest key in the tree,
     * then it has no successor and the method returns 0 */
    // ============================================================================
    BSTreeNode<Key,Val>* getSuccNode() const {
      // if node has a right child, find the min in this subtree
      if ( children[BSTREE_RIGHT] )
        return children[BSTREE_RIGHT].getMinNode();
      
      // else if it has no parent and no right child, there is no successor
      if ( parent == 0 )
        return 0;
      
      // here, the node has a parent. if it is on its right, then this is
      // the successor
      if ( parent_dir == BSTREE_LEFT )
        return parent;
      
      // here the parent is on the left => go up in the tree until you find
      // a right parent. Then this is the successor
      BSTreeNode<Key,Val>* node = this;
      
      while ( node->parent && ( node->parent_dir == BSTREE_RIGHT ) )
        node = node->parent;
      
      // if there is no more parent, then there is no successor
      // else the parent is the successor we look for
      return node->parent;
    }

    // ============================================================================
    /// returns the leftmost node with the given key in the subtree rooted by @e this
    /** if the node cannot be found, the function returns 0
     * @param k the key of the lement we look for
     * @param uniqueness_policy the uniqueness policy of the binary search tree the
     * node belongs to */
    // ============================================================================
    BSTreeNode<Key,Val>* getLeftmostNode(const Key& k,
                                            bool uniqueness_policy = false ) const {
      BSTreeNode<Key,Val>* node = this;
      // for efficiency reasons, we differentiate the case where the uniqueness
      // policy is on and that where it is off

      if ( uniqueness_policy ) {
        // search for the node in the binary search tree
        while ( node ) {
          if ( k < node->key )
            node = node->children[BSTREE_LEFT];
          else if ( k > node->key )
            node = node->children[BSTREE_RIGHT];
          else
            return node;
        }
        
        // here, we could not find the element
        GUM_ERROR( NotFound, "no such key in binary search tree" );
      }
      else {
        BSTreeNode<Key,Val>* return_node = 0;
        // search for the node in the binary search tree
        
        while ( node ) {
          if ( k < node->key )
            node = node->children[BSTREE_LEFT];
          else if ( k > node->key )
            node = node->children[BSTREE_RIGHT];
          else {
            return_node = node;
            node = node->children[BSTREE_LEFT];
          }
        }
        
        // here, either return_node is different from 0 and it is equal to
        // the leftmost element in
        if ( return_node )
          return return_node;
        else
          // here, we could not find the element
          GUM_ERROR( NotFound, "no such key in binary search tree" );
      }
    }
    
    // ============================================================================
    /** @brief returns the rightmost node with the given key in the subtree
     * rooted by @e this
     *
     * if the node cannot be found, the function returns 0
     * @param k the key of the lement we look for
     * @param uniqueness_policy the uniqueness policy of the binary search tree the
     * node belongs to */
    // ============================================================================
    BSTreeNode<Key,Val>*
    getRightmostNode( const Key& k,
                      bool uniqueness_policy = false ) const {
      BSTreeNode<Key,Val>* node = this;
      // for efficiency reasons, we differentiate the case where the uniqueness
      // policy is on and that where it is off
      
      if ( uniqueness_policy ) {
        // search for the node in the binary search tree
        while ( node ) {
          if ( k < node->key )
            node = node->children[BSTREE_LEFT];
          else if ( k > node->key )
            node = node->children[BSTREE_RIGHT];
          else
            return node;
        }
        
        // here, we could not find the element
        GUM_ERROR( NotFound, "no such key in binary search tree" );
      }
      else {
        BSTreeNode<Key,Val>* return_node = 0;
        // search for the node in the binary search tree
        
        while ( node ) {
          if ( k < node->key )
            node = node->children[BSTREE_LEFT];
          else if ( k > node->key )
            node = node->children[BSTREE_RIGHT];
          else {
            return_node = node;
            node = node->children[BSTREE_RIGHT];
          }
        }
        
        // here, either return_node is different from 0 and it is equal to
        // the rightmost element in
        if ( return_node )
          return return_node;
        else
          // here, we could not find the element
          GUM_ERROR( NotFound, "no such key in binary search tree" );
      }
    }
    
    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################

    /// @{
    // ============================================================================
    /// alias for method val
    // ============================================================================
    Val& operator*() {
      return val;
    }
    
    // ============================================================================
    /// alias for method getLeftmostNode
    /** returns the leftmost node with the given key in the subtree rooted by the
     * current node. By default, as C++ does not permit more than one parameter for
     * operator[], we consider that the binary tree key uniqueness policy is off.
     * If this is not the case, then calling method getLeftmostNode may be faster
     * than calling operator[]. Note that if the node cannot be found, the
     * function returns 0 */
    // ============================================================================
    BSTreeNode<Key,Val>* operator[]( const Key& k ) const {
      return getLeftmostNode( k );
    }
    
    /// @}
  };

  
  /* ============================================================================ */
  /* ===                      GENERIC BINARY SEARCH TREE                      === */
  /* ============================================================================ */
  /** @class BSTree
   * @brief binary search tree */
  /* ============================================================================ */
  template <typename Key, typename Val> class BSTree {
    /// developpers should always use the BSTree<X,Y>::iterator terminology
    typedef BSTreeIterator<Key,Val> iterator;
    /// BSTreeIterator should be a friend to optimize access to elements
    
    friend class BSTreeIterator<Key,Val>;
  protected:
    /// the root node of the tree
    BSTreeNode<Key,Val>* root;
    /// height of the tree
    long height;
    /// shall we check for key uniqueness in the binary search tree?
    bool key_uniqueness_policy;
    /// the list of iterators pointing to the binary search tree
    iterator *iterator_list;
    /** @name pseudo static iterator
     * the end and rend iterators are constructed only once per binary search tree
     * so as to optimize for(iter = begin();iter != end(); iter++) loops: this
     * will avoid creating objects end and rend each time we pass in the loop. */
    //\{
    iterator iter_end;
    //\}

    // ============================================================================
    /// a method for recursively copying the contents of a BSTree
    // ============================================================================
    BSTreeNode<Key,Val>* _copy( BSTreeNode<Key,Val> *node,
                                   BSTreeNode<Key,Val> *parent = 0,
                                   BSTreeDir dir = BSTREE_LEFT ) {
      // create the copy of node
      BSTreeNode<Key,Val>* new_node = new BSTreeNode<Key,Val>( *node,parent,dir );
      BSTreeNode<Key,Val>* child;
      // if necessary, create the left subgraph
      
      if ( child = node->GetChild( BSTREE_LEFT ) )
        _copy( child, this, BSTREE_LEFT );
      
      // if necessary, create the right subgraph
      if ( child = node->GetChild( BSTREE_RIGHT ) )
        _copy( child, this, BSTREE_RIGHT );
      
      return new_node;
    }
    
    // ==============================================================================
    /// a method for recursively destroying the content of a BSTree
    // ==============================================================================
    void _delete( BSTreeNode<Key,Val> *node ) {
      BSTreeNode<Key,Val>* child;
      // delete the left subgraph
      
      if ( child = node->getChild( BSTREE_LEFT ) )
        _delete( child );
      
      // delete the right subgraph
      if ( child = node->getChild( BSTREE_RIGHT ) )
        _delete( child );
      
      // delete the node itself
      delete node;
    }
    
    // ==============================================================================
    /// erase the node passed in argument
    // ==============================================================================
    void _erase( BSTreeNode<Key,Val> *node ) {
      // if the node is a leaf, we remove it directly
      if ( !node->getChild( BSTREE_LEFT ) && !node->getChild( BSTREE_RIGHT ) ) {
        if ( node->getParent() == 0 ) {
          root = 0;
          height = 0;
          delete node;
        } else {
          delete node;
          height = root->getHeight();
        }
      } else if ( !node->getChild( BSTREE_LEFT ) || !node->getChild( BSTREE_RIGHT ) ) {
        // if the node has only one child, we substitute the former by the latter
        // get the child
        BSTreeNode<Key,Val> *child;
        
        if ( node->getChild( BSTREE_LEFT ) )
          child = node->getChild( BSTREE_LEFT );
        else
          child = node->getChild( BSTREE_RIGHT );
        
        // set the child's parent appropriately
        child->parent = node->parent;
        
        child->parent_dir = node->parent_dir;
        
        // if node has a parent, set its child properly
        if ( node->getParent() ) {
          node->parent->children[child->parent_dir] = child;
        }
        
        // otherwise, change the tree's root
        else {
          root = child;
          height = child->getHeight();
        }
        
        node->parent = 0;
        
        // indicate to node that child is no more one of its children
        
        if ( child == node->getChild( BSTREE_LEFT ) )
          node->children[BSTREE_LEFT] = 0;
        else
          node->children[BSTREE_RIGHT] = 0;
        
        delete node;
      } else {
        // here the node has two children, so we swap the node itself with its
        // predecessor. This one has no child so we will be able to apply the
        // same process as above
        BSTreeNode<Key,Val> *pred = node.getPredNode();
        // remove pred as a child of its parent
        pred->parent->children[pred->parent_dir] = 0;
        
        if ( pred->parent->children[1 - pred->parent_dir] )
          pred->parent->height = 1+pred->parent->children[1-pred->parent_dir]->height;
        else
          pred->parent->height = 1;
        
        pred->parent->updateAncestorsHeight();
        
        // put pred where node was previously
        pred->parent = node->parent;
        
        pred->parent_dir = node->parent_dir;
        
        pred->height = node->height;
        
        pred->children[0] = node->children[0];
        
        pred->children[1] = node->children[1];
        
        // rechain properly parent and children
        if ( pred->children[0] ) pred->children[0]->parent = pred;
        
        if ( pred->children[1] ) pred->children[1]->parent = pred;
        
        if ( pred->parent ) {
          pred->parent->children[pred->parent_dir] = pred;
        } else {
          root = pred;
          height = pred->getHeight();
        }
      }
    }
    
  public:
    // ##############################################################################
    // constructors / destructors
    // ##############################################################################
    // ==============================================================================
    /// basic constructor: returns an empty binary search tree
    // ==============================================================================
    BSTree( bool uniqueness_policy = true ) :
      root( 0 ), height( 0 ), key_uniqueness_policy( uniqueness_policy ) {
      GUM_CONSTRUCTOR( BSTree );
    }
    
    // ==============================================================================
    /// copy constructor
    // ==============================================================================
    BSTree( const BSTree<Key,Val>& from ) :
      height( from.height ), key_uniqueness_policy( from.key_uniqueness_policy ) {
      // for debugging purposes
      GUM_CONS_CPY( BSTree );
      // copy the content of BSTree from
      
      if ( from.root == 0 )
        root = 0;
      else
        root = _copy( from.root );
    }
    
    // ==============================================================================
    /// copy operator
    // ==============================================================================
    const BSTree<Key,Val>& operator= ( const BSTree<Key,Val>& from ) {
      // avoid self assignment
      if ( this != &from ) {
        // for debugging purposes
        GUM_OP_CPY( BSTree );
        // if the tree is not currently empty, remove it

        if ( root )
          _delete( root );

        // set the height and key uniqueness properties
        height = from.height;

        key_uniqueness_policy = from.key_uniqueness_policy;

        // copy the content of BSTree from
        if ( from.root == 0 )
          root = 0;
        else
          root = _copy( from.root );
      }

      return *this;
    }

    // ==============================================================================
    /// destructor
    // ==============================================================================
    virtual ~BSTree() {
      // for debugging purposes
      DESTRUCTOR( BSTree );
      // remove all the nodes of the tree

      if ( root )
        _delete( root );
    }

    // ##############################################################################
    /// @name Accessors / Modifiers
    // ##############################################################################
    /// @{
    // ==============================================================================
    /// returns the value of the leftmost node with the given key
    /** @throw if the key cannot be found, the method throws a
     * NotFound exception */
    // ==============================================================================
    Val& operator[]( const Key& key ) const {
      BSTreeNode<Key,Val>* node = root->getLeftmostNode( key,key_uniqueness_policy );

      if ( node ) return node;

      GUM_ERROR( NotFound, "the binary search tree contains no such key" );
    }

    // ==============================================================================
    /// returns the value of the leftmost node with the minimal key in the tree
    /** @throw if the tree is empty, the method throws a EmptyBSTree exception */
    // ==============================================================================
    Val& getMinValue() const {
      if ( root == 0 )
        GUM_ERROR( EmptyBSTree, "no minimal value in an empty BS tree" );

      return root->getMinNode()->val;
    }

    // ==============================================================================
    /// returns the minimal key in the tree
    /** @throw if the tree is empty, the method throws a EmptyBSTree exception */
    // ==============================================================================
    const Key& getMinKey() const {
      if ( root == 0 )
        GUM_ERROR( EmptyBSTree, "no minimal key in an empty BS tree" );

      return root->getMinNode()->key;
    }

    // ==============================================================================
    /// returns the value of the rightmost node with the maximal key in the tree
    /** @throw if the tree is empty, the method throws a EmptyBSTree exception */
    // ==============================================================================
    Val& getMaxValue() const {
      if ( root == 0 )
        GUM_ERROR( EmptyBSTree, "no maximal value in an empty BS tree" );

      return root->getMaxNode()->val;
    }

    // ==============================================================================
    /// returns the maximal key in the tree
    /** @throw if the tree is empty, the method throws a EmptyBSTree exception */
    // ==============================================================================
    const Key& getMaxKey() const {
      if ( root == 0 )
        GUM_ERROR( EmptyBSTree, "no maximal key in an empty BS tree" );

      return root->getMaxNode()->key;
    }

    // ==============================================================================
    /** @brief creates a copy of the key and value and insert their pair in the tree
     * and returns the copy value
     *
     * When elements are inserted into binary search trees, this is actually copies
     * that are inserted (both key and value). Thus, the method returns the newly
     * created copy, so that the user may modify or reference it. */
    // ==============================================================================
    Val& insert( const Key& key, const Val& val ) {
      // if the tree is not empty, search the binary search tree to know
      // where the node should be inserted
      if ( root ) {
        BSTreeNode<Key,Val>* node = root;

        while ( true ) {
          if ( key < node->key() )
            if ( node->getChild( BSTREE_LEFT ) == 0 ) {
              // here we are on a leaf => insert the new node
              BSTreeNode<Key,Val>* new_node =
                new BSTreeNode<Key,Val> ( key,val,node,BSTREE_LEFT );
              height = root->getHeight();
              return new_node->val();
            } else {
              node = node->getChild( BSTREE_LEFT );
            }
          else if (( key > node->key() ) ||
                   (( key == node->key() ) && !key_uniqueness_policy ) )
            if ( node->getChild( BSTREE_RIGHT ) == 0 ) {
              // here we are on a leaf => insert the new node
              BSTreeNode<Key,Val>* new_node =
                new BSTreeNode<Key,Val> ( key,val,node,BSTREE_RIGHT );
              height = root->getHeight();
              return new_node->val();
            } else {
              node = node->getChild( BSTREE_RIGHT );
            }
          else
            // here we found a node with the same key and the uniqueness policy
            // is set. So we should raise an exception
            GUM_ERROR( DuplicateElement,
                       "key already defined in binary search tree" );
        }
      }

      // here the tree is empty, just create a new node
      root = new BSTreeNode<Key,Val> ( key,val );

      height = 1;

      return root->val();
    }

    // ==============================================================================
    /// erase the leftmost node with the given (key,val) pair
    /** if we could not find the node, then nothing happens. In particular, no
     * exception is raised. */
    // ==============================================================================
    void erase( const Key& key, const Val& val ) {
      BSTreeNode<Key,Val> *node;
      // first, we should find the node corresponding to the pair (key,val)
      // for efficiency reasons, we differentiate the case where the key uniqueness
      // policy is set to on and that where it is set to off

      if ( key_uniqueness_policy ) {
        node = root.getLeftmostNode( key, true );
      } else {
        node = root.getLeftmostNode( key, false );

        while ( node && ( val != node->val ) && ( node->key == key ) )
          node = node->getSuccNode();
      }

      // check if we could find the node we looked for
      if ( node == 0 ) return;

      // OK, here, we found the node to erase, so erase it
      _erase( node );
    }

    // ==============================================================================
    /// erase the leftmost node with the given key
    /** if we could not find the node, then nothing happens. In particular, no
     * exception is raised. */
    // ==============================================================================
    void erase( const Key& key ) {
      BSTreeNode<Key,Val> *node = root.getLeftmostNode( key,key_uniqueness_policy );
      // check if we could find the node we looked for

      if ( node == 0 ) return;

      // OK, here, we found the node to erase, so erase it
      _erase( node );
    }

    /// @}
  };

  /* ============================================================================== */
  /* ===                      BINARY SEARCH TREE ITERATORS                      === */
  /* ============================================================================== */
  /** @class BSTreeIterator
   * @brief Binary search tree iterators */
  /* ============================================================================== */

  template <typename Key, typename Val> class BSTreeIterator {
    /** class BSTree must be a friend because it stores iterators end and
     * rend, and those can be properly initialized only when the list has been
     * fully allocated. Thus, proper initialization can only take place within
     * the constructor's code of the list. */

    friend class BSTree<Key, Val>;
  protected:
    /// the BST the iterator is pointing to
    BSTree<Key, Val> *tree;
    /// the node pointed to by the iterator
    BSTreeNode<Key, Val> *node;
    /** @brief the BSTree node we should start from when we decide to do a ++. Usually
     * it should be equal to node. However, if the user has deleted the object
     * pointed to by node, this will point to another BSTreeNode. When it is equal
     * to 0, it means that the node reached after a ++ is end(). */
    BSTreeNode<Key, Val> *next_current_node;
    /** @brief the node we should start from when we decide to do a --. When it
     * is equal to 0, it means that the node reached after a -- is rend(). */
    BSTreeNode<Key, Val> *prev_current_node;
    /** @brief the node we should start from when we decide to do a up. When it
     * is equal to 0, it means that the node reached after an up is upend() */
    BSTreeNode<Key, Val> *up_current_node;
    /** @brief the node we should start from when we decide to do a downLeft. When it
     * is equal to 0, it means that the node reached after an up is downleftend() */
    BSTreeNode<Key, Val> *downleft_current_node;
    /** @brief the node we should start from when we decide to do a downRight. When it
     * is equal to 0, it means that the node reached after an up is downrightend() */
    BSTreeNode<Key, Val> *downright_current_node;
    /// next iterator attached to the tree
    BSTreeIterator<Key, Val> *next;
    /// preceding iterator of the tree registered list of iterators
    BSTreeIterator<Key, Val> *prev;
  public:
    // ##############################################################################
    // constructors / destructors
    // ##############################################################################
    // ==============================================================================
    /// basic constructor
    // ==============================================================================
    BSTreeIterator() :
      tree( 0 ), node( 0 ),
      next_current_node( 0 ), prev_current_node( 0 ), up_current_node( 0 ),
      downleft_current_node( 0 ), downright_current_node( 0 ), next( 0 ), prev( 0 ) {
      GUM_CONSTRUCTOR( BSTreeIterator );
      GUM_DEBUG_TRACE( "<-- BSTreeIterator EMPTY constructor" );
    }

    // ==============================================================================
    /// copy constructor
    // ==============================================================================
    BSTreeIterator( const BSTreeIterator<Key,Val>& from ) :
      tree( from.tree ), node( from.node ),
      next_current_node( from.next_current_node ),
      prev_current_node( from.prev_current_node ),
      up_current_node( from.up_current_node ),
      downleft_current_node( from.downleft_current_node ),
      downright_current_node( downright_current_node ), prev( 0 ) {
      // for debugging purposes
      GUM_CONS_CPY( BSTreeIterator );
      GUM_DEBUG_TRACE( "<-- BSTreeIterator COPY constructor" );
      // rechain properly the list of iterators of the BSTree

      if ( tree ) {
        next = tree->iterator_list;

        if ( tree->iterator_list )
          tree->iterator_list->prev = this;

        tree->iterator_list = this;
      } else
        next = 0;
    }

    // ==============================================================================
    /// copy operator
    // ==============================================================================
    BSTreeIterator<Key,Val>& operator= ( const BSTreeIterator<Key,Val>& from ) {
      // avoid self assignment
      if ( this != &from ) {
        // for debugging purposes
        GUM_OP_CPY( BSTreeIterator );
        // check if the current BSTree is different from that of from. In such
        // a case, we shall remove the iterator from its current BSTree iterator's
        // list and add it to the new BSTree iterator's list

        if ( tree != from.tree ) {
          // remove the iterator from its BSTree iterator's list'
          if ( prev )
            prev->next = next;
          else if ( tree )
            tree->iterator_list = next;

          if ( next )
            next->prev = prev;

          // add to the new one
          prev = 0;

          if ( from.tree ) {
            next = from.tree->iterator_list;

            if ( from.tree->iterator_list )
              from.tree->iterator_list->prev = this;

            from.tree->iterator_list = this;
          } else
            next = 0;
        }

        // update the content of the iterator
        tree = from.tree;

        node = from.node;

        next_current_node      = from.next_current_node;

        prev_current_node      = from.prev_current_node;

        up_current_node        = from.up_current_node;

        downleft_current_node  = from.downleft_current_node;

        downright_current_node = downright_current_node;
      }

      return *this;
    }

    // ==============================================================================
    /// Constructor for an iterator pointing to the leftmost element of
    /** The first element of a list is in position 0.
     * If the list does not contain at leat \e ind_elt elements, then the method
     * throws a UndefinedIteratorValue exception. The method runs in linear time
     * in the size of the list.
     * @param theList a reference on the list the iterator should point to.
     * @param ind_elt the position of the element in the list we should point to.
     * The frist element of a list is in position 0.
     * @throw UndefinedIteratorValue the list does not contain at least
     * \e ind_elt elements */
    // ==============================================================================
    BSTreeIterator( const BSTree<Key,Val>& theTree, BSTreeDir dir ) :
      tree( const_cast<BSTree<Key,Val>*>( &theTree ) ), node( 0 ),
      next_current_node( 0 ), prev_current_node( 0 ), up_current_node( 0 ),
      downleft_current_node( 0 ), downright_current_node( 0 ), prev( 0 ) {
      // for debugging purposes
      GUM_CONSTRUCTOR( BSTreeIterator );
      GUM_DEBUG_TRACE( "<-- BSTreeIterator VALUED constructor" );
      // checks if tree is a NULL pointer

      if ( tree == 0 )
        next = 0;
      else {
        // depending on dir, find the leftmost or the rightmost element
        if ( tree->root != 0 ) {
          if ( dir == BSTREE_LEFT )
            node = tree->root->getMinNode();
          else
            node = tree->root->getMaxNode();
        }

        // assign the *_current_node accordingly
        next_current_node      = node;

        prev_current_node      = node;

        up_current_node        = node;

        downleft_current_node  = node;

        downright_current_node = node;

        // rechain properly the iterator's list of the List
        next = tree->iterator_list;

        if ( tree->iterator_list )
          tree->iterator_list->prev = this;

        tree->iterator_list = this;
      }
    }

    // ##############################################################################
    /// @name Accessors / Modifiers
    // ##############################################################################
    /// @{
    // ==============================================================================
    /// returns the value of a given child of the current node
    /** @throw if the child does not exist, the method throws a NoChild
     * exception */
    // ==============================================================================
    Val& getChildValue( BSTreeDir dir ) const {
      if ( node && node->children[dir] )
        return node->children[dir]->val;
      else
        GUM_ERROR( NoChild, "the child cannot be found" );
    }

    // ==============================================================================
    /// returns the key of a given child of the current node
    /** @throw if the child does not exist, the method throws a NoChild
     * exception */
    // ==============================================================================
    const Key& getChildKey( BSTreeDir dir ) const {
      if ( node && node->children[dir] )
        return node->children[dir]->key;
      else
        GUM_ERROR( NoChild, "the child cannot be found" );
    }

    // ==============================================================================
    /// returns the value of the parent of the current node
    /** @throw if the parent does not exist, the method throws a NoParent
     * exception */
    // ==============================================================================
    Val& getParentValue() const {
      if ( node && node->parent )
        return node->parent->val;
      else
        GUM_ERROR( NoParent, "the curent node has no parent" );
    }

    // ==============================================================================
    /// returns the key of the parent of the current node
    /** @throw if the parent does not exist, the method throws a NoParent
     * exception */
    // ==============================================================================
    const Key& getParentKey() const {
      if ( node && node->parent )
        return node->parent->key;
      else
        GUM_ERROR( NoParent, "the curent node has no parent" );
    }

    // ==============================================================================
    /** @brief returns the value of the leftmost node with the minimal key in the
     * subtree rooted by the current node
     *
     * @warning the method may return the value of the current node if the latter
     * has the smallest key in the whole subtree */
    // ==============================================================================
    Val& getMinValue() const {
      // check if we point somewhere in a tree
      if ( node == 0 )
        GUM_ERROR( UndefinedIteratorValue, "the iterator points to no node" );

      // return the value of the leftmost element
      return node->getMinNode()->val;
    }

    // ==============================================================================
    /// returns the minimal key in the subtree rooted by the current node
    /** @warning the method may return the key of the current node if the latter
     * is the smallest in the whole subtree */
    // ==============================================================================
    Key& getMinKey() const {
      // check if we point somewhere in a tree
      if ( node == 0 )
        GUM_ERROR( UndefinedIteratorKey, "the iterator points to no node" );

      // return the key of the leftmost element
      return node->getMinNode()->key;
    }

    // ==============================================================================
    /** @brief returns the value of the rightmost node with the maximal key in the
     * subtree rooted by the current node
     *
     * @warning the method may return the value of the current node if the latter
     * has the highest key in the whole subtree */
    // ==============================================================================
    Val& getMaxValue() const {
      // check if we point somewhere in a tree
      if ( node == 0 )
        GUM_ERROR( UndefinedIteratorValue, "the iterator points to no node" );

      // return the value of the rightmost element
      return node->getMaxNode()->val;
    }

    // ==============================================================================
    /// returns the maximal key in the subtree rooted by the current node
    /** @warning the method may return the key of the current node if the latter
     * is the highest in the whole subtree */
    // ==============================================================================
    Key& getMaxKey() const {
      // check if we point somewhere in a tree
      if ( node == 0 )
        GUM_ERROR( UndefinedIteratorKey, "the iterator points to no node" );

      // return the key of the rightmost element
      return node->getMaxNode()->key;
    }

    // ==============================================================================
    /// detaches the iterator from the BSTree it is attached to
    /** It is mainly used by the BSTree when the latter is deleted while the
     * iterator is still alive. */
    // ==============================================================================
    void detach() {
      // remove the iterator from the tree's iterator list
      if ( prev )
        prev->next = next;
      else if ( tree )
        tree->iterator_list = next;

      if ( next )
        next->prev = prev;

      // set its table as well as the element it points to to 0
      tree = 0;

      node = 0;

      next_current_node      = 0;

      prev_current_node      = 0;

      up_current_node        = 0;

      downleft_current_node  = 0;

      downright_current_node = 0;

      next = 0;

      prev = 0;
    }

    /// @}
    // ##############################################################################
    /// @name Operators
    // ##############################################################################
    /// @{
    // ==============================================================================
    /// makes the iterator point to the successor of the current element
    /** for (iter=begin(); iter!=end(); ++iter) loops are guaranteed to parse the
     * whole BSTree as long as no element is added to or deleted from the
     * BSTree while being in the loop. Deleting elements during the
     * loop is guaranteed to never produce a segmentation fault. */
    // ==============================================================================
    BSTreeIterator<Key, Val>& operator++() {
      // if we are pointing to an element of the BSTree, just
      // point on the next node in the tree
      if ( next_current_node ) {
        node = next_current_node->getSuccNode();
        prev_current_node = node;
        next_current_node = node;
        up_current_node   = node;
        downleft_current_node  = node;
        downright_current_node = node;
        return *this;
      }

      // we are pointing just before the beginning of the BSTree, point on
      // the first one, if any, or on 0 otherwise
      if ( prev_current_node ) {
        node = tree->root->getMinNode();
        prev_current_node = node;
        next_current_node = node;
        up_current_node   = node;
        downleft_current_node  = node;
        downright_current_node = node;
        return *this;
      }

      // here we are pointing on the end of the BSTree
      node = 0;

      prev_current_node = node;

      next_current_node = node;

      up_current_node   = node;

      downleft_current_node  = node;

      downright_current_node = node;

      return *this;
    }

    // ==============================================================================
    /// makes the iterator point to the preceding element in the BSTree
    /** for (iter=rbegin(); iter!=rend(); --iter) loops are guaranteed to
     * parse the whole BSTree as long as no element is added to or deleted from
     * the BSTree while being in the loop. Deleting elements during the
     * loop is guaranteed to never produce a segmentation fault. */
    // ==============================================================================
    BSTreeIterator<Key, Val>& operator--() {
      // if we are not pointing on the first element of the BSTree, just
      // point on the preceding node in this tree
      if ( prev_current_node ) {
        node = prev_current_node->getPrecNode();
        prev_current_node = node;
        next_current_node = node;
        up_current_node   = node;
        downleft_current_node  = node;
        downright_current_node = node;
        return *this;
      }

      // we are pointing on the element just after the end of the BSTree,
      // point on the last one
      if ( next_current_node ) {
        node = tree->root->getMaxNode();
        prev_current_node = node;
        next_current_node = node;
        up_current_node   = node;
        downleft_current_node  = node;
        downright_current_node = node;
        return *this;
      }

      // here we are pointing on the beginning of the BSTree
      node = 0;

      prev_current_node = node;

      next_current_node = node;

      up_current_node   = node;

      downleft_current_node  = node;

      downright_current_node = node;

      return *this;
    }

    // ==============================================================================
    /// checks whether two iterators are pointing toward different elements
    // ==============================================================================
    const bool operator!= ( const BSTreeIterator<Key, Val> &from ) {
      return (( node != from.node ) ||
              ( next_current_node != from.next_current_node ) ||
              ( prev_current_node != from.prev_current_node ) );
    }

    // ==============================================================================
    /// checks whether two iterators are pointing toward the same element
    // ==============================================================================
    const bool operator== ( const BSTreeIterator<Key, Val> &from ) {
      return (( node == from.node ) &&
              ( next_current_node == from.next_current_node ) &&
              ( prev_current_node == from.prev_current_node ) );
    }

    // ==============================================================================
    /// returns the value pointed to by the iterator
    /** @throws UndefinedIteratorValue when the iterator does not point to
     * a valid BSTree node */
    // ==============================================================================
    Val& operator*() {
      if ( node )
        return node->val;
      else
        GUM_ERROR( UndefinedIteratorValue, "Accessing a NULL object" );
    }

    /// @}
  };

} /* namespace gum */
  
#endif /* BSTREE_H */
