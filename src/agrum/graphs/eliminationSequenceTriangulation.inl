INLINE
const EdgeSet& EliminationSequenceTriangulation::fillIns() {
  if(__fillIns == 0) {
    __computeEliminationTree();
  }
  return *__fillIns;
}
    
INLINE
const std::vector<NodeId>& EliminationSequenceTriangulation::eliminationOrder() {
  return __eliminationSequence;
}

INLINE
unsigned int EliminationSequenceTriangulation::eliminationOrder( const NodeId id )  {
  return __invertedElimination[id];
}

INLINE
const UndiGraph& EliminationSequenceTriangulation::triangulatedGraph() {
  if(__triangulatedGraph == 0) {
    __computeEliminationTree();
  }
  return *__triangulatedGraph;
}

INLINE
const CliqueGraph& EliminationSequenceTriangulation::eliminationTree() {
  if(__eliminationTree == 0) {
    __computeEliminationTree();
  }
  return *__eliminationTree;
}

INLINE
const CliqueGraph& EliminationSequenceTriangulation::junctionTree() {
  if(__junctionTree == 0) {
    __computeJunctionTree();
  }
  return *__junctionTree;
}

INLINE
NodeId EliminationSequenceTriangulation::createdClique( const NodeId id ) {
  if(__junctionTree == 0) {
    __computeJunctionTree();
  }
  return (*__createdCliqueTab)[id];
}

INLINE
void EliminationSequenceTriangulation::clear() {
  if(__fillIns != 0) {
    delete __fillIns;
  }
  if(__triangulatedGraph != 0) {
    delete __triangulatedGraph;
  }
  if(__eliminationTree != 0) {
    delete __eliminationTree;
  }
  if(__junctionTree != 0) {
    delete __junctionTree;
  }
  if(__createdCliqueTab != 0) {
    delete __createdCliqueTab;
  }
  __cliqueOrder.clear();
}

INLINE
void EliminationSequenceTriangulation::setGraph ( const UndiGraph& theGraph,
						  const Property<unsigned int>::onNodes& modal ) {
  __graph = theGraph;
  clear();
}

INLINE
Triangulation* EliminationSequenceTriangulation::copyFactory () const {
  return new EliminationSequenceTriangulation(*this);
}
