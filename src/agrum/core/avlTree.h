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
 * @brief Class providing generic AVL trees
 *
 * @author Jean-Philippe DUBUS
 *
 * This file provides class AVL tree : binary search tree providing
 * a safe complexity (O(log(n))) for many operations
 */

#ifndef GUM_AVL_TREE_H
#define GUM_AVL_TREE_H

#include <agrum/core/utils.h>
#include <utility>

namespace gum {

GUM_MAKE_ERROR(NoFather, Exception, "No father")
GUM_MAKE_ERROR(NoLeftSon, Exception, "No left son")
GUM_MAKE_ERROR(NoRightSon, Exception, "No right son")

/* ============================================================================= */
  /* =========================================================================== */
  /* ===                          GENERIC TREE NODES                         === */
  /* =========================================================================== */
  /* =========================================================================== */
  /** @class TreeNode
   * @brief The class for generic Tree Nodes
   * @ingroup basicstruct_group
   *
   * This class is a generic representation for node in binary tree, each node
   * contains an element and almost two sons.
   */
template <typename Element> class TreeNode {
public:

  /**
   * Constructor
   * Construct a node containing an element, a father, a left son and a right son.
   * 
   * @param e the element
   * @param father the father (if 0, we assume that the node doesn't have father)
   * @param leftSon the left son (if 0, we assume that the node doesn't have
   * left son)
   * @param father the father (if 0, we assume that the node doesn't have
   * right son)
   */
  TreeNode(const Element& e,
	      TreeNode<Element>* father = 0,
	      TreeNode<Element>* leftSon = 0,
	      TreeNode<Element>* rightSon = 0);

  /**
   * Copy constructor
   * Construct a node copying another node
   * 
   * @param node node to copy
   * @warning it's not a safe method : father, leftSon and rightSon are at the
   * same address than the original node and aren't modifyed you must know what
   * you do !
   */
  TreeNode(const TreeNode<Element>& node);

  /**
   * Destructor
   */
  virtual ~TreeNode();

  /**
   * @return true if this node has a father
   */
  bool hasFather() const;

  /**
   * @return true if this node has a right son
   */
  bool hasRightSon() const;

  /**
   * @return true if this node has a left son
   */
  bool hasLeftSon() const;

  /**
   * @return the father of this node
   * @warning throw gumNoFather exception if this node has no father
   */  
  TreeNode<Element>& getFather();

  /**
   * @return the right son of this node
   * @warning throw gumNoRightSon exception if this node has no right son
   */  
  TreeNode<Element>& getRightSon();

  /**
   * @return the left son of this node
   * @warning throw gumNoLeftSon exception if this node has no left son
   */  
  TreeNode<Element>& getLeftSon();

  /**
   * @return the father of this node
   * @warning throw gumNoFather exception if this node has no father
   */  
  const TreeNode<Element>& getFather() const;

  /**
   * @return the right son of this node
   * @warning throw gumNoRightSon exception if this node has no right son
   */  
  const TreeNode<Element>& getRightSon() const;
 
  /**
   * @return the left son of this node
   * @warning throw gumNoLeftSon exception if this node has no left son
   */  
  const TreeNode<Element>& getLeftSon() const;
  
  /**
   * @return the element stored in this node
   */
  Element& getElement();

  /**
   * @return the element stored in this node
   */
  const Element& getElement() const;
  
  /**
   * Change the element stored in this node
   * @param e the new element
   */
  void setElement(const Element& e);

  /**
   * Change the father of this node
   * @param node the new father (0 if you want that this node forgets its father)
   */  
  void setFather(TreeNode<Element>* node);

  /**
   * Change the left son of this node
   * @param node the new left son (0 if you want that this node forgets its
   * left son)
   */  
  void setLeftSon(TreeNode<Element>* node);

  /**
   * Change the right son of this node
   * @param node the new right son (0 if you want that this node forgets its
   * right son)
   */  
  void setRightSon(TreeNode<Element>* node);

  /**
   * @returns the height stored in the node
   * @warning you must be sure that the height is the good value
   */
  unsigned int getHeight() const;

  /**
   * Define a new height for this node
   * @param newHeight the new height
   */
  void setHeight(unsigned int newHeight);


  /**
   * If the node has a father, this method returns the position of actual node
   * @return true if the current node is a left son of his father, false for
   * right son
   * @warning throw gumNoFather if the current node doesn't have a father
   */
  bool getPosition() const;
  

  /**
   * A little more safe method for defining a new father :
   * The old father is forgotten (and it forgets his son)
   * A new father is defined, this new father knows its new son (and forgets
   * its old son)
   * 
   * @param newFather the new father
   * @param direction true if we want to have a left son, false if right
   */
  void setCoherentFather(TreeNode<Element>& newFather, bool direction);


  /**
   * A little more safe method for defining a new left son :
   * @param newLeftSon same as newLeftSon.setCoherentFather(*this, true)
   */
  void setCoherentLeftSon(TreeNode<Element>& newLeftSon);

  /**
   * A little more safe method for defining a new right son :
   * @param newRightSon same as newRightSon.setCoherentFather(*this, false)
   */
  void setCoherentRightSon(TreeNode<Element>& newRightSon);

  /**
   * Recompute height such as 1 + max { left height, right height }
   * That's not a recursive method
   */
  void recomputeHeight();

protected:
  

  Element content;
  TreeNode<Element>* father;
  TreeNode<Element>* leftSon;
  TreeNode<Element>* rightSon;

  unsigned int height;

};

GUM_MAKE_ERROR(NoMax, Exception, "")

template <typename Element,
          typename Priority = int,
          typename Cmp = std::less<Priority> >
class AVLTree {
public:
  
  /**
   * Constructor
   *
   * @param compare a comparaison function
   */
  AVLTree(Cmp compare = Cmp());

  /**
   * Copy constructor
   *
   * @param tree tree to copy
   */
  AVLTree(const AVLTree<Element, Priority, Cmp>& tree);

  /**
   * Destructor
   */
  virtual ~AVLTree();


  /**
   * Insert a (key, value) association in the tree structure
   *
   * @param priority key 
   * @param element value
   */
  virtual void insert(const Priority& priority, const Element& element);

  /**
   * Insert every (key, value) association into th egiven tree
   *
   * @param tree tree where the associations will be inserted
   */
  void insertInto(AVLTree<Element, Priority, Cmp>& tree); 

  /**
   * @return returns the element of maximal key
   */
  Element& getMaximalElement() const;

  /**
   * @return returns the maximal priority in the str
   */
  Priority& getMaximalPriority() const;


  /**
   * Remove the element of maximal key
   */
  void deleteMaximalElement();


  /**
   * Remove one element for a given key
   *
   * @param priority key
   */
  void deleteByPriority(const Priority& priority);

  /**
   * Search an element associated to a given key
   *
   * @param priority key associated to the element
   * @return the element associated to the key
   */
  const Element& search(const Priority& priority) const;

  /**
   * Search an element associated to a given key
   *
   * @param priority key associated to the element
   * @return the element associated to the key
   */  
  Element& search(const Priority& priority);


  /**
   * @return the number of elements stored in the tree structure
   */
  unsigned long int getNbrElts() const;

  /**
   * For testing purpose
   *
   * @param node don't use this param
   * @return true if the tree structure respects the AVL property
   */
  bool hasAVLProperty(TreeNode<std::pair<Priority, Element> >* node = 0) const;


  // ============================================================================
  /// operator [] returns the value corresponding to a given key
  /**
   * @throw NotFound
   */
  // ============================================================================
  Element& operator[]( const Priority& key );
  const Element& operator[]( const Priority& key ) const ;
 

protected:

  void recomputeMax();

  void balance(TreeNode<std::pair<Priority, Element> >& node);

  void leftRotation(TreeNode<std::pair<Priority, Element> >& node);

  void rightRotation(TreeNode<std::pair<Priority, Element> >& node);
  
  void deleteSubtree(TreeNode<std::pair<Priority, Element> >& node);

  void deleteJustANode(TreeNode<std::pair<Priority, Element> >& node);
  
  TreeNode<std::pair<Priority, Element> >&
  getMaximalNodeInSubtree(TreeNode<std::pair<Priority, Element> >& subtree);


  const TreeNode<std::pair<Priority, Element> >&
  searchByPriorityInSubtree(const Priority& priority,
                            const TreeNode<std::pair<Priority, Element> >& subTree) const;

  TreeNode<std::pair<Priority, Element> >& searchByPriorityInSubtree(const Priority& priority,
									TreeNode<std::pair<Priority, Element> >& subTree);

  virtual void recursiveInsertion(TreeNode<std::pair<Priority, Element> >& toInsert,
			  TreeNode<std::pair<Priority, Element> >* toStudy);
  
  virtual void recursiveInsertInto(AVLTree<Element, Priority, Cmp>& tree,
			   TreeNode<std::pair<Priority, Element> >& toInsert);

  Cmp compare;
  TreeNode<std::pair<Priority, Element> >* root;
  TreeNode<std::pair<Priority, Element> >* maximalNode;
  unsigned long int nbrElements;
  
private:

  TreeNode<std::pair<Priority, Element> >* copyTreeContent(const TreeNode<std::pair<Priority, Element> >& node);
  
};

#include "avlTree.tcc"

}

#endif
