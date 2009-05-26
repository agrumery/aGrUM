#include <agrum/graphs/eliminationSequenceTriangulation.h>

namespace gum {

#ifdef GUM_NO_INLINE
#include <agrum/graphs/eliminationSequenceTriangulation.inl>
#endif

  EliminationSequenceTriangulation::EliminationSequenceTriangulation(const UndiGraph& graph,
								     const std::vector<NodeId>& eliminationSequence) :
    __graph(graph),
    __eliminationSequence(eliminationSequence),
    __invertedElimination(__eliminationSequence.size()),
    __fillIns(0),
    __triangulatedGraph(0),
    __eliminationTree(0),
    __junctionTree(0),
    __createdCliqueTab(0)
  {
    GUM_CONSTRUCTOR(EliminationSequenceTriangulation);
    for(unsigned int iter = 0;
	iter < __eliminationSequence.size();
	++iter) {
      __invertedElimination.insert(__eliminationSequence[iter], iter);
    }
  }

  EliminationSequenceTriangulation::~EliminationSequenceTriangulation() {
    clear();
    GUM_DESTRUCTOR(EliminationSequenceTriangulation);
  }

  EliminationSequenceTriangulation::EliminationSequenceTriangulation(const EliminationSequenceTriangulation& toCopy) :
    __graph(toCopy.__graph),
    __eliminationSequence(toCopy.__eliminationSequence),
    __invertedElimination(toCopy.__invertedElimination),
    __fillIns(0),
    __triangulatedGraph(0),
    __eliminationTree(0),
    __junctionTree(0),
    __createdCliqueTab(0)
  {
    GUM_CONS_CPY(EliminationSequenceTriangulation);
  }
  
  
  EliminationSequenceTriangulation& EliminationSequenceTriangulation::operator=(const EliminationSequenceTriangulation& toCopy) {
    __graph = toCopy.__graph;
    __eliminationSequence = toCopy.__eliminationSequence;
    __invertedElimination = toCopy.__invertedElimination;
    __fillIns = 0;
    __triangulatedGraph = 0;
    __eliminationTree = 0;
    __junctionTree = 0;
    __createdCliqueTab = 0;
    return *this;
  }


  const CliqueGraph& EliminationSequenceTriangulation::maxPrimeSubgraphTree() {
    GUM_ASSERT(false);
    return __maxPrimeSubgraph;
  }

  NodeId EliminationSequenceTriangulation::createdMaxPrimeSubgraph( const NodeId id ) {
    GUM_ASSERT(false);
    return 666;
  }

 
  void EliminationSequenceTriangulation::__computeEliminationTree() {
    __fillIns = new EdgeSet;
    __triangulatedGraph = new UndiGraph(__graph);
    __eliminationTree = new CliqueGraph;
    __createdCliqueTab = new HashTable<NodeId, NodeId>(__graph.size());
    HashTable<NodeId, NodeId> cliqueToLink(__graph.size());


    UndiGraph copy(__graph);
    for(unsigned int i = 0; i < __eliminationSequence.size(); ++i) {

      NodeId id = __eliminationSequence[i];
      const EdgeSet& neighbours = copy.neighbours(id);
      NodeSet clique(neighbours.size());
      clique.insert(id);
      unsigned int* minOrder = 0;
      NodeId argMinOrder;
      for(EdgeSet::iterator iter = neighbours.begin();
	  iter != neighbours.end(); ++iter) {
	NodeId id1 = iter->other(id);
	clique.insert(id1);
	unsigned int elim = eliminationOrder(id1);
	// We compute next clique to link in elimination tree
	if(minOrder == 0) {
	  minOrder = new unsigned int(elim);
	  argMinOrder = id1;
	} else if(*minOrder > elim) {
	  *minOrder = elim;
	  argMinOrder = id1;
	}
	for(EdgeSet::iterator iter2 = iter;
	    iter2 != neighbours.end(); ++iter2) {
	  NodeId id2 = iter2->other(id);
	  if( (id1 != id2) && (! copy.existsEdge(id1, id2)) ) {
	    // We create fill-in edge

	    copy.insertEdge(id1, id2);
	    __triangulatedGraph->insertEdge(id1, id2);
	    __fillIns->insert(Edge(id1, id2));

	  }
	}
      }
      copy.eraseNode(id);
      NodeId cliqueId = __eliminationTree->insertNode(clique);

      __createdCliqueTab->insert(id, cliqueId);
      __cliqueOrder.insert(cliqueId, i);
      if(minOrder != 0) {
	cliqueToLink.insert(cliqueId, argMinOrder);
	delete minOrder;
      }
    }
    // Now we have cliques, we need to add separator
    for(HashTable<NodeId, NodeId>::iterator iter = cliqueToLink.begin();
	iter != cliqueToLink.end(); ++iter) {
      __eliminationTree->insertEdge(iter.key(),
				    (*__createdCliqueTab)[*iter]);
    }

  }



  void EliminationSequenceTriangulation::__computeJunctionTree() {
    __junctionTree = new CliqueGraph(eliminationTree());
    EdgeSet flag;
    for(unsigned int i = __eliminationSequence.size() - 1;
	i < __eliminationSequence.size(); --i) /* :p  */ {
      NodeId cliqueId = (*__createdCliqueTab)[__eliminationSequence[i]];
      const EdgeSet& neighbours = __junctionTree->neighbours(cliqueId);
      int cliqueSize = (int) __junctionTree->clique(cliqueId).size();
      NodeId idToAbsorb = cliqueId;
      unsigned int order = __cliqueOrder[cliqueId];
      for(EdgeSet::iterator iter = neighbours.begin();
	  iter != neighbours.end(); ++iter) {
	Edge edge = *iter;
	NodeId otherClique = edge.other(cliqueId);
	if((! flag.contains(edge)) && 
	   (((int) __junctionTree->clique(otherClique).size()) - cliqueSize == 1)
	   && (__cliqueOrder[otherClique] < order)) {
	  idToAbsorb = edge.other(cliqueId);
	  break;
	}
      }
      if(idToAbsorb != cliqueId) {

	// We reaffect created clique tab
	const NodeSet& nodeSet = __junctionTree->clique(cliqueId);
	for(NodeSet::iterator iter = nodeSet.begin();
	    iter != nodeSet.end(); ++iter) {
	  NodeId node = *iter;
	  if((*__createdCliqueTab)[node] == cliqueId) {

	    __createdCliqueTab->set(node, idToAbsorb);
	  }
	}
	// We delete the clique and adding new separators between cliques
	for(EdgeSet::iterator iter = neighbours.begin();
	    iter != neighbours.end(); ++iter) {
	  Edge edge = *iter;
	  NodeId otherClique = edge.other(cliqueId);
	  if(otherClique != idToAbsorb) {
	    __junctionTree->insertEdge(Edge(otherClique, idToAbsorb));
	    if(__cliqueOrder[otherClique] < order) {
	      flag.insert(Edge(otherClique, idToAbsorb)); 
	    }
	  }
	}
	__junctionTree->eraseNode(cliqueId);
      }
    }

  }

}
