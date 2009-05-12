template <typename Element>
TreeNode<Element>::TreeNode(const Element& e,
				TreeNode<Element>* father_,
				TreeNode<Element>* leftSon_,
				TreeNode<Element>* rightSon_) :
  content(e), father(father_), leftSon(leftSon_), rightSon(rightSon_),
  height(1)
{
  GUM_CONSTRUCTOR(TreeNode);
  recomputeHeight();
}

template <typename Element>
TreeNode<Element>::TreeNode(const TreeNode<Element>& node) :
  content(node.content), father(node.father), leftSon(node.leftSon),
  rightSon(node.rightSon), height(node.height)
{
  GUM_CONS_CPY(TreeNode);
}

template <typename Element>
TreeNode<Element>::~TreeNode() {
  GUM_DESTRUCTOR(TreeNode);
}

template <typename Element> INLINE
bool TreeNode<Element>::hasFather() const {
  return father != 0;
}

template <typename Element>  INLINE
bool TreeNode<Element>::hasRightSon() const {
  return rightSon != 0;
}

template <typename Element> INLINE
bool TreeNode<Element>::hasLeftSon() const {
  return leftSon != 0;
}

template <typename Element> INLINE
TreeNode<Element>& TreeNode<Element>::getFather() {
  if(hasFather()) {
    return *father;
  } else {
    GUM_ERROR(NoFather, "");
  }
}

template <typename Element> INLINE
TreeNode<Element>& TreeNode<Element>::getRightSon() {
  if(hasRightSon()) {
    return *rightSon;
  } else {
    GUM_ERROR(NoRightSon, "");
  }
}

template <typename Element> INLINE
TreeNode<Element>& TreeNode<Element>::getLeftSon() {
  if(hasLeftSon()) {
    return *leftSon;
  } else {
    GUM_ERROR(NoLeftSon, "");
  }
}

template <typename Element> INLINE
const TreeNode<Element>& TreeNode<Element>::getFather() const {
  if(hasFather()) {
    return *father;
  } else {
    GUM_ERROR(NoFather, "");
  }
}

template <typename Element> INLINE
const TreeNode<Element>& TreeNode<Element>::getRightSon() const {
  if(hasRightSon()) {
    return *rightSon;
  } else {
    GUM_ERROR(NoRightSon, "");
  }
}

template <typename Element> INLINE
const TreeNode<Element>& TreeNode<Element>::getLeftSon() const {
  if(hasLeftSon()) {
    return *leftSon;
  } else {
    GUM_ERROR(NoLeftSon, "");
  }
}

template <typename Element> INLINE
Element& TreeNode<Element>::getElement() {
  return content;
}

template <typename Element> INLINE
const Element& TreeNode<Element>::getElement() const {
  return content;
}

template <typename Element> INLINE
void TreeNode<Element>::setElement(const Element& e) {
  content = e;
}

template <typename Element> INLINE
void TreeNode<Element>::setFather(TreeNode<Element>* node) {
  father = node;
}

template <typename Element> INLINE
void TreeNode<Element>::setLeftSon(TreeNode<Element>* node) {
  leftSon = node;
}

template <typename Element> INLINE
void TreeNode<Element>::setRightSon(TreeNode<Element>* node) {
  rightSon = node;
}

template <typename Element> INLINE
unsigned int TreeNode<Element>::getHeight() const {
  return height;
}

template <typename Element> INLINE
void TreeNode<Element>::setHeight(unsigned int newHeight) {
  height = newHeight;
}

template <typename Element> INLINE
bool TreeNode<Element>::getPosition() const {
  const TreeNode<Element>& father = getFather();
  return (father.hasLeftSon() && (this == &(father.getLeftSon())));
}

template <typename Element> INLINE
void TreeNode<Element>::setCoherentFather(TreeNode<Element>& newFather, bool direction) {
  try {
    TreeNode<Element>& actualFather =
      getFather();
    if(getPosition()) {
      actualFather.setLeftSon(0);
    } else {
      actualFather.setRightSon(0);
    }
    actualFather.recomputeHeight();
  } catch(NoFather&) { }
  setFather(&newFather);
  if(direction) {
    newFather.setLeftSon(this);
  } else {
    newFather.setRightSon(this);
  }
  newFather.recomputeHeight();
}

template <typename Element> INLINE
void TreeNode<Element>::setCoherentLeftSon(TreeNode<Element>& newLeftSon) {
  newLeftSon.setCoherentFather(*this, true);
}

template <typename Element> INLINE
void TreeNode<Element>::setCoherentRightSon(TreeNode<Element>& newRightSon) {
  newRightSon.setCoherentFather(*this, false);
}

template <typename Element>
void TreeNode<Element>::recomputeHeight() {
  if(hasLeftSon() && hasRightSon()) {
    height = std::max<unsigned int>(getLeftSon().getHeight(),
				    getRightSon().getHeight()) + 1;
  } else if(hasLeftSon()) {
    height = getLeftSon().getHeight() + 1;
  } else if(hasRightSon()) {
    height = getRightSon().getHeight() + 1;
  } else {
    height = 1;
  }
}











template <typename Element, typename Priority, typename Cmp>
AVLTree<Element, Priority, Cmp>::AVLTree(Cmp compare_) :
  compare(compare_), root(0), maximalNode(0), nbrElements(0)
{
  GUM_CONSTRUCTOR(AVLTree);
}

template <typename Element, typename Priority, typename Cmp>
AVLTree<Element, Priority, Cmp>::AVLTree(const AVLTree<Element, Priority, Cmp>& tree) :
  compare(tree.compare), root(0), maximalNode(0), nbrElements(tree.nbrElements)
{
  GUM_CONS_CPY(AVLTree);
  root = copyTreeContent(tree.root);
  recomputeMax();
}

template <typename Element, typename Priority, typename Cmp>
AVLTree<Element, Priority, Cmp>::~AVLTree() {
  if(root != 0) {
    deleteSubtree(*root);
  }
  GUM_DESTRUCTOR(AVLTree);
}

template <typename Element, typename Priority, typename Cmp> INLINE
Element& AVLTree<Element, Priority, Cmp>::getMaximalElement() const {
  if(maximalNode != 0) {
    return maximalNode->getElement().second;
  } else {
    GUM_ERROR(NotFound, "");
  }
}

template <typename Element, typename Priority, typename Cmp> INLINE
void AVLTree<Element, Priority, Cmp>::deleteByPriority(const Priority& priority) {
  if(root != 0) {
    TreeNode<std::pair<Priority, Element> >& node =
      searchByPriorityInSubtree(priority, *root);
    deleteJustANode(node);
  } else {
    GUM_ERROR(NotFound, "");
  }
}

template <typename Element, typename Priority, typename Cmp> INLINE
Element& AVLTree<Element, Priority, Cmp>::search(const Priority& priority) {
  if(root != 0) {
    return searchByPriorityInSubtree(priority, *root).getElement().second;
  } else {
    GUM_ERROR(NotFound, "");
  }
}

template <typename Element, typename Priority, typename Cmp> INLINE
const Element& AVLTree<Element, Priority, Cmp>::search(const Priority& priority) const {
  if(root != 0) {
    return searchByPriorityInSubtree(priority, *root).getElement().second;
  } else {
    GUM_ERROR(NotFound, "");
  }
}

template <typename Element, typename Priority, typename Cmp> INLINE
Priority& AVLTree<Element, Priority, Cmp>::getMaximalPriority() const {
  if(maximalNode != 0) {
    return maximalNode->getElement().first;
  } else {
    GUM_ERROR(NotFound, "");
  } 
}

template <typename Element, typename Priority, typename Cmp> INLINE
unsigned long int AVLTree<Element, Priority, Cmp>::getNbrElts() const {
  return nbrElements;
}

template <typename Element, typename Priority, typename Cmp> INLINE
Element& AVLTree<Element, Priority, Cmp>::operator[]( const Priority& key ) {
  return search(key);
}

template <typename Element, typename Priority, typename Cmp> INLINE
const Element& AVLTree<Element, Priority, Cmp>::operator[]( const Priority& key ) const {
  return search(key);
}


template <typename Element, typename Priority, typename Cmp>
TreeNode<std::pair<Priority, Element> >* AVLTree<Element, Priority, Cmp>::copyTreeContent(const TreeNode<std::pair<Priority, Element> >& node) {
  TreeNode<std::pair<Priority, Element> >* nodeCopy = new TreeNode<std::pair<Priority, Element> >(node);
  if(node.hasLeftSon()) {
    TreeNode<std::pair<Priority, Element> >* left = copyTreeContent(node.getLeftSon());
    nodeCopy->setLeftSon(left);
    left->setFather(nodeCopy);
  }
  if(node.hasRightSon()) {
    TreeNode<std::pair<Priority, Element> >* right = copyTreeContent(node.getRightSon());
    nodeCopy->setRightSon(right);
    right->setFather(nodeCopy);
  }
  nodeCopy->recomputeHeight();
  return nodeCopy;
}

template <typename Element, typename Priority, typename Cmp>
void AVLTree<Element, Priority, Cmp>::deleteSubtree(TreeNode<std::pair<Priority, Element> >& node) {
  if(node.hasLeftSon()) {
    deleteSubtree(node.getLeftSon());
  }
  if(node.hasRightSon()) {
    deleteSubtree(node.getRightSon());
  }
  if(node.hasFather()) {
    TreeNode<std::pair<Priority, Element> >& father = node.getFather();
    if(node.getPosition()) {
      father.setLeftSon(0);
    } else {
      father.setRightSon(0);
    }
    father.recomputeHeight();
  }
  --nbrElements;
  delete &node;
}

template <typename Element, typename Priority, typename Cmp>
void AVLTree<Element, Priority, Cmp>::insert(const Priority& priority, const Element& element) {
  ++nbrElements;
  TreeNode<std::pair<Priority, Element> >* node = new TreeNode<std::pair<Priority, Element> >(std::make_pair(priority, element));
  recursiveInsertion(*node, root);
  if( ((maximalNode != 0) && 
       (! compare(priority, maximalNode->getElement().first)))
      || (maximalNode == 0) ) {
    maximalNode = node;
  }
}

template <typename Element, typename Priority, typename Cmp>
void AVLTree<Element, Priority, Cmp>::recursiveInsertion(TreeNode<std::pair<Priority, Element> >& toInsert,
							    TreeNode<std::pair<Priority, Element> >* toStudy) {
  Priority priority = toInsert.getElement().first;
  if(toStudy != 0) {
    toInsert.setFather(toStudy);
    TreeNode<std::pair<Priority, Element> >* nextNode; 
    if(compare(priority, toStudy->getElement().first)) {
      nextNode = (toStudy->hasLeftSon() ? &(toStudy->getLeftSon()) : 0);
    } else {
      nextNode = (toStudy->hasRightSon() ? &(toStudy->getRightSon()) : 0);
    }
    recursiveInsertion(toInsert, nextNode);
    toStudy->recomputeHeight();
    //balance(*toStudy);
  } else {
    if(toInsert.hasFather()) {
      TreeNode<std::pair<Priority, Element> >& father =
	toInsert.getFather();
      if(compare(priority, father.getElement().first)) {
	father.setLeftSon(&toInsert);
      } else {
	father.setRightSon(&toInsert);
      }
      toInsert.recomputeHeight();
      balance(toInsert);
    } else {
      root = &toInsert;
    }
  }
}

template <typename Element, typename Priority, typename Cmp>
void AVLTree<Element, Priority, Cmp>::balance(TreeNode<std::pair<Priority, Element> >& node) {
  unsigned int lHeight = (node.hasLeftSon() ? node.getLeftSon().getHeight() : 0);
  unsigned int rHeight = (node.hasRightSon() ? node.getRightSon().getHeight() : 0);

  if((lHeight > rHeight) && ((lHeight - rHeight) > 1)) {
    unsigned int hll = 
      (node.getLeftSon().hasLeftSon() ? node.getLeftSon().getLeftSon().getHeight() : 0);
    unsigned int hlr = 
      (node.getLeftSon().hasRightSon() ? node.getLeftSon().getRightSon().getHeight() : 0);
    if(hll < hlr) {
      leftRotation(node.getLeftSon());
    }
    rightRotation(node);
    if(root == &node) {
      root = &(node.getFather());
    }
  } else if((rHeight > lHeight) && ((rHeight - lHeight) > 1)) {
    unsigned int hrl = 
      (node.getRightSon().hasLeftSon() ? node.getRightSon().getLeftSon().getHeight() : 0);
    unsigned int hrr = 
      (node.getRightSon().hasRightSon() ? node.getRightSon().getRightSon().getHeight() : 0);
    if(hrr < hrl) {
      rightRotation(node.getRightSon());
    }
    leftRotation(node);
    if(root == &node) {
      root = &(node.getFather());
    }
  }
  node.recomputeHeight();
  if(node.hasFather()) {
    node.getFather().recomputeHeight();
    balance(node.getFather());
    
  }
}

/*
//          node                       A
//         /    \                     / \
//        A      B                   C  node
//       / \                           /   \
//      C   D          ===>           D     B
// max is unchanged
*/   
template <typename Element, typename Priority, typename Cmp>
void AVLTree<Element, Priority, Cmp>::rightRotation(TreeNode<std::pair<Priority, Element> >& node) {
  TreeNode<std::pair<Priority, Element> >& A = node.getLeftSon();
  TreeNode<std::pair<Priority, Element> >* D = 
    (A.hasRightSon() ? &(A.getRightSon()) : 0);
  TreeNode<std::pair<Priority, Element> >* nodeFather = 
    (node.hasFather() ? &(node.getFather()) : 0);

  A.setFather(nodeFather);
  if(nodeFather != 0) {
    if(&(nodeFather->getRightSon()) == &node) {
      nodeFather->setRightSon(&A);
    } else {
      nodeFather->setLeftSon(&A);
    }
  }
  node.setFather(&A);
  A.setRightSon(&node);
  node.setLeftSon(D);
  if(D != 0) {
    D->setFather(&node);
  }
  node.recomputeHeight();
  A.recomputeHeight();
  if(nodeFather != 0) {
    nodeFather->recomputeHeight();
  }
}

/*          node                     B
 *         /    \                   / \
 *        A      B                node D
 *              / \               / \
 *             C   D    ===>     A   C
 * max is unchanged 
*/   
template <typename Element, typename Priority, typename Cmp>
void AVLTree<Element, Priority, Cmp>::leftRotation(TreeNode<std::pair<Priority, Element> >& node) {
  TreeNode<std::pair<Priority, Element> >& B = node.getRightSon();
  TreeNode<std::pair<Priority, Element> >* C = 
    (B.hasLeftSon() ? &(B.getLeftSon()) : 0);
  TreeNode<std::pair<Priority, Element> >* nodeFather = 
    (node.hasFather() ? &(node.getFather()) : 0);

  B.setFather(nodeFather);
  if(nodeFather != 0) {
    if(node.getPosition()) {
      nodeFather->setLeftSon(&B);
    } else {
      nodeFather->setRightSon(&B);
    }
  }
  node.setFather(&B);
  B.setLeftSon(&node);
  node.setRightSon(C);
  if(C != 0) {
    C->setFather(&node);
  }
  node.recomputeHeight();
  B.recomputeHeight();
  if(nodeFather != 0) {
    nodeFather->recomputeHeight();
  }
}

template <typename Element, typename Priority, typename Cmp>
void AVLTree<Element, Priority, Cmp>::insertInto(AVLTree<Element, Priority, Cmp>& tree) {
  if(root != 0) {
    recursiveInsertInto(tree, *root);
  }
}

template <typename Element, typename Priority, typename Cmp>
void AVLTree<Element, Priority, Cmp>::recursiveInsertInto(AVLTree<Element, Priority, Cmp>& tree,
							     TreeNode<std::pair<Priority, Element> >& toInsert) {
  if(toInsert.hasLeftSon()) {
    recursiveInsertInto(tree, toInsert.getLeftSon());
  }
  if(toInsert.hasRightSon()) {
    recursiveInsertInto(tree, toInsert.getRightSon());
  }
  tree.insert(toInsert.getElement().first, toInsert.getElement().second);
}




template <typename Element, typename Priority, typename Cmp>
void AVLTree<Element, Priority, Cmp>::recomputeMax() {
  if(root == 0) {
    maximalNode = 0;
  } else {
    maximalNode = root;
    while(maximalNode->hasRightSon()) {
      maximalNode = &(maximalNode->getRightSon());
    }
  }
}

template <typename Element, typename Priority, typename Cmp>
void AVLTree<Element, Priority, Cmp>::deleteMaximalElement() {
  nbrElements--;
  TreeNode<std::pair<Priority, Element> >* toDelete = maximalNode;
  if(toDelete == 0) {
    GUM_ERROR(NoMax, "");
  }
  // A maximal element couldn't have right son
  TreeNode<std::pair<Priority, Element> >* father = 
    (toDelete->hasFather() ? &(toDelete->getFather()) : 0);
  TreeNode<std::pair<Priority, Element> >* left = 
    (toDelete->hasLeftSon() ? &(toDelete->getLeftSon()) : 0);
  if(left == 0) {
    if(father == 0) {
      // ROOT
      root = 0;
    } else {
      father->setRightSon(0);
      father->recomputeHeight();
      balance(*father);
    }
  } else {
    if(father == 0) {
      left->setFather(0);
      root = left;
    } else {
      left->setFather(father);
      father->setRightSon(left);
      left->recomputeHeight();
      father->recomputeHeight();
      balance(*father);
    }
  }
  recomputeMax();
  delete toDelete;
}


template <typename Element, typename Priority, typename Cmp>
void AVLTree<Element, Priority, Cmp>::deleteJustANode(TreeNode<std::pair<Priority, Element> >& node) {
  TreeNode<std::pair<Priority, Element> >* nodeToBalance = 0;
  if(node.hasLeftSon() && node.hasRightSon()) {
    // 2 sons
    TreeNode<std::pair<Priority, Element> >& leftSon =
      node.getLeftSon();
    TreeNode<std::pair<Priority, Element> >& maxNode = 
      getMaximalNodeInSubtree(leftSon);
    if(maxNode.hasLeftSon()) {
      nodeToBalance = &(maxNode.getLeftSon());
      maxNode.getLeftSon().setCoherentFather(maxNode.getFather(), true);
    }
    if(&maxNode != &(node.getLeftSon())) {
      maxNode.setCoherentLeftSon(node.getLeftSon());
    }
    maxNode.setCoherentRightSon(node.getRightSon());
    if(node.hasFather()) {
      maxNode.setCoherentFather(node.getFather(), node.getPosition());
    } else {
      if(maxNode.getPosition()) {
	maxNode.getFather().setLeftSon(0);
      } else {
	maxNode.getFather().setRightSon(0);
      }
      maxNode.setFather(0);
    }
    if(root == &node) {
      root = &maxNode;
    }
    if(nodeToBalance == 0) {
      nodeToBalance = &maxNode;
    }
  } else {
    if(node.hasRightSon()) {
      // 1 son
      TreeNode<std::pair<Priority, Element> >& rightSon =
	node.getRightSon();
      if(root == &node) {
	rightSon.setFather(0);
	root = &rightSon;
      } else {
	rightSon.setCoherentFather(node.getFather(), node.getPosition());
      }
      nodeToBalance = &rightSon;
    } else if(node.hasLeftSon()) {
      // 1 son
      TreeNode<std::pair<Priority, Element> >& leftSon =
	node.getLeftSon();
      if(root == &node) {
	leftSon.setFather(0);
	root = &leftSon;
      } else {
	leftSon.setCoherentFather(node.getFather(), node.getPosition());
      }
      nodeToBalance = &leftSon;
    } else {
      // No son
      if(node.hasFather()) {
	if(node.getPosition()) {
	  node.getFather().setLeftSon(0);
	} else {
	  node.getFather().setRightSon(0);
	}
	nodeToBalance = &(node.getFather());
      }
      if(root == &node) {
	root = 0;
      }
    }
  }
  delete &node;
  --nbrElements;
  if(nodeToBalance != 0) {
    balance(*nodeToBalance);
  }
}

template <typename Element, typename Priority, typename Cmp>
const TreeNode<std::pair<Priority, Element> >& AVLTree<Element, Priority, Cmp>::searchByPriorityInSubtree(const Priority& priority, 
														const TreeNode<std::pair<Priority, Element> >& subTree) const {
  const Priority& priorityToStudy = subTree.getElement().first;
  if(subTree.getElement().first == priority) {
    // We've got it !
    return subTree;
  }
  if(compare(priority, priorityToStudy)) {
    if(subTree.hasLeftSon()) {
      return searchByPriorityInSubtree(priority, subTree.getLeftSon());
    } else {
      GUM_ERROR(NotFound, "");
    }
  } else {
    if(subTree.hasRightSon()) {
      return searchByPriorityInSubtree(priority, subTree.getRightSon());
    } else {
      GUM_ERROR(NotFound, "");
    }
  }
}

template <typename Element, typename Priority, typename Cmp>
TreeNode<std::pair<Priority, Element> >& AVLTree<Element, Priority, Cmp>::searchByPriorityInSubtree(const Priority& priority, 
													  TreeNode<std::pair<Priority, Element> >& subTree) {
  const Priority& priorityToStudy = subTree.getElement().first;
  if(subTree.getElement().first == priority) {
    // We've got it !
    return subTree;
  }
  if(compare(priority, priorityToStudy)) {
    if(subTree.hasLeftSon()) {
      return searchByPriorityInSubtree(priority, subTree.getLeftSon());
    } else {
      GUM_ERROR(NotFound, "");
    }
  } else {
    if(subTree.hasRightSon()) {
      return searchByPriorityInSubtree(priority, subTree.getRightSon());
    } else {
      GUM_ERROR(NotFound, "");
    }
  }
}

template <typename Element, typename Priority, typename Cmp>
TreeNode<std::pair<Priority, Element> >& AVLTree<Element, Priority, Cmp>::getMaximalNodeInSubtree(TreeNode<std::pair<Priority, Element> >& subtree) {
  try {
    return getMaximalNodeInSubtree(subtree.getRightSon());
  } catch(NoRightSon&) {
    return subtree;
  }
}

template <typename Element, typename Priority, typename Cmp>
bool AVLTree<Element, Priority, Cmp>::hasAVLProperty(TreeNode<std::pair<Priority, Element> >* node) const {
  if(node == 0) {
    if(root == 0) {
      return true;
    } else {
      return hasAVLProperty(root);
    }
  } else {
    unsigned int lHeight, rHeight;
    if(node->hasLeftSon()) {
      if(! hasAVLProperty(&(node->getLeftSon()))) {
	return false;
      } 
      lHeight = node->getLeftSon().getHeight();
    } else {
      lHeight = 0;
    }
    if(node->hasRightSon()) {
      if(! hasAVLProperty(&(node->getRightSon()))) {
	return false;
      } 
      rHeight = node->getRightSon().getHeight();
    } else {
      rHeight = 0;
    }
    unsigned int diff=(lHeight>rHeight)?(lHeight-rHeight):(rHeight-lHeight);
    return (diff <= 1);
  }
}

// template <typename Element, typename Priority, typename Cmp>
// void AVLTree<Element, Priority, Cmp>::print(TreeNode<std::pair<Priority, Element> >* node, int plop) {
//   if(node == 0) {
//     if(root != 0) { print(root); }
//   } else {
//     if(node->hasLeftSon()) {
//       print(&(node->getLeftSon()), plop + 1);
//     }
//     for(int i = 0; i < plop; ++i) {
//       std::cerr << "-";
//     }
//     std::cerr << " " << node->getElement().first << std::endl;
//     if(node->hasRightSon()) {
//       print(&(node->getRightSon()), plop + 1);
//     }
//   }
// }
