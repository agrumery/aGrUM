#include <run/globalFunctionGraphMinimizer.h>


namespace gum {



  // *****************************************************************************************************
  /// Binarisation d'un graphe de fonction
  // *****************************************************************************************************
  void GlobalFunctionGraphMinimizer::loadFunctionGraphs(MultiDimFunctionGraph<double>* fg1, MultiDimFunctionGraph<double>* fg2){
    __varList.clear();
    __fg1 = fg1;
    __fg2 = fg2;
  }



  // *****************************************************************************************************
  /// Binarisation d'un graphe de fonction
  // *****************************************************************************************************
  void GlobalFunctionGraphMinimizer::minimize(){
    __extractVarList();
    __orderSimillary();
    __minimizeGlobally();
  }



  // *****************************************************************************************************
  /// Binarisation d'un graphe de fonction
  // *****************************************************************************************************
  void GlobalFunctionGraphMinimizer::__extractVarList(){

    for( SequenceIteratorSafe<const DiscreteVariable*> varIter = __fg1->variablesSequence().beginSafe(); varIter != __fg1->variablesSequence().endSafe(); ++varIter)
      __varList.insert(*varIter);

    for( SequenceIteratorSafe<const DiscreteVariable*> varIter = __fg2->variablesSequence().beginSafe(); varIter != __fg2->variablesSequence().endSafe(); ++varIter)
      if(!__varList.exists(*varIter))
        __varList.insert(*varIter);
  }

  // *****************************************************************************************************
  /// Binarisation de plusieurs graphes de fonction
  // *****************************************************************************************************
  void GlobalFunctionGraphMinimizer::__orderSimillary(){
    for( SequenceIteratorSafe<const DiscreteVariable*> varIter = __varList.beginSafe(); varIter != __varList.endSafe(); ++varIter)  {

      if( !__fg1->variablesSequence().exists(*varIter))
        __fg1->add(**varIter);
      __fg1->manager()->moveTo(*varIter, __varList.pos(*varIter));

      if( !__fg2->variablesSequence().exists(*varIter))
        __fg2->add(**varIter);
      __fg2->manager()->moveTo(*varIter, __varList.pos(*varIter));
    }
  }



  // *****************************************************************************************************
  /// Binarisation d'un graphe de fonction
  // *****************************************************************************************************
  void GlobalFunctionGraphMinimizer::__minimizeGlobally( ){

    // Classement des variables par taille de niveau
    Sequence<const DiscreteVariable*> siftingSeq;
    HashTable<const DiscreteVariable*, Idx> varLvlSize;
    for( SequenceIteratorSafe<const DiscreteVariable*> varIter = __varList.beginSafe(); varIter != __varList.endSafe(); ++varIter ){

      Idx nbElem = 0;

      const Link<NodeId>* curElem = __fg1->varNodeListe(*varIter)->list();
      for(;curElem != nullptr; nbElem++, curElem = curElem->nextLink());
      curElem = __fg2->varNodeListe(*varIter)->list();
      for(;curElem != nullptr; nbElem++, curElem = curElem->nextLink());

      varLvlSize.insert(*varIter, nbElem);
      siftingSeq.insert(*varIter);
      Idx pos = siftingSeq.pos(*varIter);
      while( pos > 0 && varLvlSize[siftingSeq.atPos(pos - 1)] > nbElem ){
        siftingSeq.swap(pos - 1, pos);
        pos--;
      }

    }

    // Sifting var par var
    for( SequenceIteratorSafe<const DiscreteVariable*> sifIter = siftingSeq.beginSafe(); sifIter != siftingSeq.endSafe(); ++sifIter ){

      // Initialisation nouveau Sifting
      Idx currentPos = __varList.pos(*sifIter);
      Idx bestSize = __fg1->realSize() +__fg2->realSize();
      Idx bestPos = 0;

      // Sifting vers level supérieur
      while( currentPos > 0 ){
        __fg1->manager()->moveTo(*sifIter, currentPos - 1);
        __fg2->manager()->moveTo(*sifIter, currentPos - 1);
        currentPos--;
        if( __fg1->realSize() + __fg2->realSize() < bestSize ){
          bestPos = currentPos;
          bestSize = __fg1->realSize() + __fg2->realSize();
        }
      }

      // Sifting vers level inférieur
      while( currentPos < siftingSeq.size() - 1 ){
        __fg1->manager()->moveTo(*sifIter, currentPos + 1);
        __fg2->manager()->moveTo(*sifIter, currentPos + 1);
        currentPos++;
        if( __fg1->realSize() + __fg2->realSize() < bestSize ){
          bestPos = currentPos;
          bestSize = __fg1->realSize() + __fg2->realSize();
        }
      }

      __fg1->manager()->moveTo(*sifIter, bestPos);
      __fg2->manager()->moveTo(*sifIter, bestPos);
  //    currentPos = __varList.pos(*sifIter);
  //    while( currentPos != bestPos ){
  //      std::cout << *sifIter << " and again" << std::endl;
  //      if(currentPos > bestPos)
  //        siftingSeq.swap(currentPos - 1, currentPos);
  //      else
  //        siftingSeq.swap(currentPos + 1, currentPos);
  //      currentPos = __varList.pos(*sifIter);
  //    }
    }

  }

}
