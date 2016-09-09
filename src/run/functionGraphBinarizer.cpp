#include <run/functionGraphBinarizer.h>

namespace gum {

  FunctionGraphBinarizer::~FunctionGraphBinarizer(){
    for( HashTableIteratorSafe<const DiscreteVariable*, Sequence<const DiscreteVariable*>*> seqIter = __varMap.beginSafe();
         seqIter != __varMap.endSafe(); ++seqIter ){
      for( SequenceIteratorSafe<const DiscreteVariable*> varIter = seqIter.val()->beginSafe(); varIter !=  seqIter.val()->endSafe(); ++varIter)
        delete *varIter;
      delete seqIter.val();
    }

    GUM_DESTRUCTOR(FunctionGraphBinarizer)
  }


  // *****************************************************************************************************
  ///
  // *****************************************************************************************************
  void FunctionGraphBinarizer::loadFunctionGraph(const MultiDimFunctionGraph<double>* src){
    __binarizedFG = src;
    __src2dest.clear();
  }


  // *****************************************************************************************************
  ///
  // *****************************************************************************************************
  MultiDimFunctionGraph<double>* FunctionGraphBinarizer::binarize(){

    __binarizeVariables();

    __dest = MultiDimFunctionGraph<double>::getReducedAndOrderedInstance();
    __binarizeFunctionGraph();
    __dest->manager()->setRootNode( __src2dest[__binarizedFG->root()] );

    return __dest;
  }


  // *****************************************************************************************************
  /// Generates binary variables associated to each variables
  // *****************************************************************************************************
  void FunctionGraphBinarizer::__binarizeVariables(  ){

    for( SequenceIteratorSafe<const DiscreteVariable*> varIter = __binarizedFG->variablesSequence().beginSafe();
         varIter != __binarizedFG->variablesSequence().endSafe(); ++varIter)  {
      if( !__varMap.exists(*varIter)){
        __varMap.insert( *varIter, new  Sequence<const DiscreteVariable*>() );
        Idx nbVar = Idx(std::ceil(std::log2((*varIter)->domainSize())));
        for( Idx i = 0; i < nbVar; ++i )
          __varMap[*varIter]->insert(new LabelizedVariable((*varIter)->name() + std::to_string(i)));
      }
    }
  }


  // *****************************************************************************************************
  /// Binarisation d'un graphe de fonction
  // *****************************************************************************************************
  NodeId FunctionGraphBinarizer::__recurBinarize(NodeId binarizedNode,
                                                 Idx valueModa,
                                                 Idx vPos ){

    const DiscreteVariable* binarizedVar = __binarizedFG->node(binarizedNode)->nodeVar();
    if( valueModa >= binarizedVar->domainSize() )
      return __defaultNode;

    NodeId* sonsMap = static_cast<NodeId*>(SmallObjectAllocator::instance().allocate(2*sizeof(NodeId)));

    if( vPos > 0 ){
      sonsMap[0] = Size(__recurBinarize( binarizedNode, valueModa, vPos - 1 ));
      sonsMap[1] = Size(__recurBinarize( binarizedNode, valueModa + Idx(std::pow(2,vPos)), vPos - 1 ));
    } else {

      NodeId srcSon = __binarizedFG->node( binarizedNode )->son( valueModa );
      NodeId destSon = __src2dest[srcSon];
      sonsMap[0] = destSon;

      if( valueModa + 1 < binarizedVar->domainSize() ){
        srcSon = __binarizedFG->node( binarizedNode )->son( valueModa + 1 );
        destSon = __src2dest[srcSon];
      } else
        destSon = __defaultNode;
      sonsMap[1] = destSon;
    }
    return __dest->manager()->addInternalNode(__varMap[binarizedVar]->atPos(vPos), sonsMap);
  }


  // *****************************************************************************************************
  ///
  // *****************************************************************************************************
  void FunctionGraphBinarizer::__binarizeFunctionGraph (  ) {

    // Insertion des variables dans le FunctionGraph
    for( SequenceIteratorSafe<const DiscreteVariable*> varIter = __binarizedFG->variablesSequence().beginSafe();
         varIter != __binarizedFG->variablesSequence().endSafe(); ++varIter )
      for(  SequenceIteratorSafe<const DiscreteVariable*> binVarIter = __varMap[*varIter]->rbeginSafe();
            binVarIter != __varMap[*varIter]->rendSafe(); --binVarIter )
        __dest->add(**binVarIter);

    // Insertion des noeuds terminaux
    for( __binarizedFG->beginValues(); __binarizedFG->hasValue(); __binarizedFG->nextValue() )
      __src2dest.insert( __binarizedFG->id(), __dest->manager()->addTerminalNode( __binarizedFG->value() ) );

    __defaultNode = __dest->manager()->addTerminalNode(0.0);

    // Insertion des noeuds internes
    for( SequenceIteratorSafe<const DiscreteVariable*> varIter = __binarizedFG->variablesSequence().rbeginSafe();
         varIter != __binarizedFG->variablesSequence().rendSafe(); --varIter ){
      const Link<NodeId>* curNodeLink = __binarizedFG->varNodeListe(*varIter)->list();
      for( ;curNodeLink != nullptr; curNodeLink = curNodeLink->nextLink() ){
        NodeId srcn = curNodeLink->element();
        NodeId destn =  __recurBinarize(curNodeLink->element(), 0, __varMap[*varIter]->size() - 1 );

        __src2dest.insert( srcn, destn );
      }
    }
  }
}
