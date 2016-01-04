#include <agrum/multidim/FunctionGraphUtilities/globalFunctionGraphMinimizer.h>


namespace gum {


  // *****************************************************************************************************
  /// Loading the two function graphs to minimize together
  // *****************************************************************************************************
  template <typename GUM_SCALAR, template <typename> class TerminalNodePolicy>
  void GlobalFunctionGraphMinimizer<GUM_SCALAR, TerminalNodePolicy>::
      loadFunctionGraphs(
          const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* fg1,
          const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* fg2 ) {
    __varList.clear();
    __dependancyMap.clear();
    __dependancyGraph.clear();
    __fg1 = const_cast<MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>*>(
        fg1 );
    __fg2 = const_cast<MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>*>(
        fg2 );
  }


  // *****************************************************************************************************
  /// Minimizes the two function graphs it holds
  // *****************************************************************************************************
  template <typename GUM_SCALAR, template <typename> class TerminalNodePolicy>
  void
  GlobalFunctionGraphMinimizer<GUM_SCALAR, TerminalNodePolicy>::minimize() {
    __extractVarList();
    __orderSimillary();
    __minimizeGlobally();
  }


  // *****************************************************************************************************
  /// Extracting gloabl list of variables
  // *****************************************************************************************************
  template <typename GUM_SCALAR, template <typename> class TerminalNodePolicy>
  void GlobalFunctionGraphMinimizer<GUM_SCALAR,
                                    TerminalNodePolicy>::__extractVarList() {

    const DiscreteVariable* parent = nullptr;
    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              __fg1->variablesSequence().beginSafe();
          varIter != __fg1->variablesSequence().endSafe();
          ++varIter ) {
      __varList.insert( *varIter );
      __dependancyMap.insert( __dependancyGraph.addNode(), *varIter );
      if ( parent != nullptr )
        __dependancyGraph.addArc( __dependancyMap.first( parent ),
                                  __dependancyMap.first( *varIter ) );
      parent = *varIter;
    }

    parent = nullptr;
    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              __fg2->variablesSequence().beginSafe();
          varIter != __fg2->variablesSequence().endSafe();
          ++varIter ) {
      if ( !__varList.exists( *varIter ) ) {
        __varList.insert( *varIter );
        __dependancyMap.insert( __dependancyGraph.addNode(), *varIter );
      }
      if ( parent != nullptr )
        __dependancyGraph.addArc( __dependancyMap.first( parent ),
                                  __dependancyMap.first( *varIter ) );
      parent = *varIter;
    }
  }

  // *****************************************************************************************************
  /// Before minimization, the two graphs have to be ordered simillary
  // *****************************************************************************************************
  template <typename GUM_SCALAR, template <typename> class TerminalNodePolicy>
  void GlobalFunctionGraphMinimizer<GUM_SCALAR,
                                    TerminalNodePolicy>::__orderSimillary() {

    // Finding an order on variables that will minimizes the number
    // of variables swaps
    // Inspired from an algorithm to find an efficient order in dependancy graphs
    Sequence<const DiscreteVariable *> source, puits;
    while ( !__varList.empty() ) {

      // Removing  source and well nodes
      bool modified = true;
      while ( modified ) {
        modified = false;
        for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
                  __varList.beginSafe();
              varIter != __varList.endSafe();
              ++varIter ) {
          if ( __dependancyGraph.parents( __dependancyMap.first( *varIter ) )
                   .empty() ) {
            source.insert( *varIter );
            __dependancyGraph.eraseNode( __dependancyMap.first( *varIter ) );
            __varList.erase( *varIter );
            modified = true;
            continue;
          } else if ( __dependancyGraph.children(
                                           __dependancyMap.first( *varIter ) )
                          .empty() ) {
            puits.insert( *varIter );
            __dependancyGraph.eraseNode( __dependancyMap.first( *varIter ) );
            __varList.erase( *varIter );
            modified = true;
            continue;
          }
        }
      }

      // Delect best var to move out
      const DiscreteVariable* bestVar = nullptr;
      Idx bestVarDegree = 0;
      for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
                __varList.beginSafe();
            varIter != __varList.endSafe();
            ++varIter ) {
        Idx varDegree =
            __dependancyGraph.parents( __dependancyMap.first( *varIter ) )
                .size() +
            __dependancyGraph.children( __dependancyMap.first( *varIter ) )
                .size();
        if ( varDegree > bestVarDegree ) {
          bestVarDegree = varDegree;
          bestVar = *varIter;
        }
      }

      // Moving out this var
      if ( bestVar != nullptr ) {
        source.insert( bestVar );
        __dependancyGraph.eraseNode( __dependancyMap.first( bestVar ) );
        __varList.erase( bestVar );
      }
    }

    // Performing the reordering
    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              source.beginSafe();
          varIter != source.endSafe();
          ++varIter )
      __varList.insert( *varIter );
    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              puits.rbeginSafe();
          varIter != puits.rendSafe();
          --varIter )
      __varList.insert( *varIter );

    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              __varList.beginSafe();
          varIter != __varList.endSafe();
          ++varIter ) {

      if ( !__fg1->variablesSequence().exists( *varIter ) )
        __fg1->add( **varIter );

      if ( !__fg2->variablesSequence().exists( *varIter ) )
        __fg2->add( **varIter );

      __fg1->manager()->moveTo( *varIter, __varList.pos( *varIter ) );
      __fg2->manager()->moveTo( *varIter, __varList.pos( *varIter ) );
    }
  }


  // *****************************************************************************************************
  /// Minimizes the two graphs together
  // *****************************************************************************************************
  template <typename GUM_SCALAR, template <typename> class TerminalNodePolicy>
  void GlobalFunctionGraphMinimizer<GUM_SCALAR,
                                    TerminalNodePolicy>::__minimizeGlobally() {

    // Ordering variables by number of nodes tied to
    Sequence<const DiscreteVariable*> siftingSeq;
    HashTable<const DiscreteVariable*, Idx> varLvlSize;
    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              __varList.beginSafe();
          varIter != __varList.endSafe();
          ++varIter ) {

      Idx nbElem = 0;

      const Link<NodeId>* curElem = __fg1->varNodeListe( *varIter )->list();
      for ( ; curElem != nullptr; nbElem++, curElem = curElem->nextLink() )
        ;
      curElem = __fg2->varNodeListe( *varIter )->list();
      for ( ; curElem != nullptr; nbElem++, curElem = curElem->nextLink() )
        ;

      varLvlSize.insert( *varIter, nbElem );
      siftingSeq.insert( *varIter );
      Idx pos = siftingSeq.pos( *varIter );
      while ( pos > 0 && varLvlSize[siftingSeq.atPos( pos - 1 )] > nbElem ) {
        siftingSeq.swap( pos - 1, pos );
        pos--;
      }
    }

    // Sifting var by var
    for ( SequenceIteratorSafe<const DiscreteVariable*> sifIter =
              siftingSeq.beginSafe();
          sifIter != siftingSeq.endSafe();
          ++sifIter ) {

      bool goOn = true;

      // Initialization
      Idx currentPos = __varList.pos( *sifIter );
      Idx bestSize = __fg1->realSize() + __fg2->realSize();
      Idx bestPos = currentPos;
      Idx initPos = currentPos;

      // Sifting twoard upper level
      while ( currentPos > 0 && goOn ) {

        // First ensure that swap won't potentially overgrow MDD size
        Idx nbElemPar1 = 0;
        const Link<NodeId>* curElem =
            __fg1->varNodeListe(
                     __fg1->variablesSequence().atPos( currentPos - 1 ) )
                ->list();
        for ( ; curElem != nullptr;
              nbElemPar1++, curElem = curElem->nextLink() )
          ;
        Idx nbElemCur1 = 0;
        curElem = __fg1->varNodeListe( *sifIter )->list();
        for ( ; curElem != nullptr;
              nbElemCur1++, curElem = curElem->nextLink() )
          ;

        Idx nbElemPar2 = 0;
        curElem = __fg2->varNodeListe(
                           __fg2->variablesSequence().atPos( currentPos - 1 ) )
                      ->list();
        for ( ; curElem != nullptr;
              nbElemPar2++, curElem = curElem->nextLink() )
          ;
        Idx nbElemCur2 = 0;
        curElem = __fg2->varNodeListe( *sifIter )->list();
        for ( ; curElem != nullptr;
              nbElemCur2++, curElem = curElem->nextLink() )
          ;

        if ( __fg1->realSize() +
                     ( nbElemCur1 + nbElemPar1 ) * ( *sifIter )->domainSize() >
                 100000 ||
             __fg2->realSize() +
                     ( nbElemCur2 + nbElemPar2 ) * ( *sifIter )->domainSize() >
                 100000 )
          break;

        // Do the SWAP if it's ok
        __fg1->manager()->moveTo( *sifIter, currentPos - 1 );
        __fg2->manager()->moveTo( *sifIter, currentPos - 1 );
        currentPos--;
        if ( __fg1->realSize() + __fg2->realSize() < bestSize ) {
          bestPos = currentPos;
          bestSize = __fg1->realSize() + __fg2->realSize();
        }
      }

      __fg1->manager()->moveTo( *sifIter, initPos );
      __fg2->manager()->moveTo( *sifIter, initPos );
      goOn = true;

      // Sifting towards lower level
      while ( currentPos < siftingSeq.size() - 1 && goOn ) {

        // First ensure that swap won't potentially overgrow MDD size
        Idx nbElemPar1 = 0;
        const Link<NodeId>* curElem =
            __fg1->varNodeListe(
                     __fg1->variablesSequence().atPos( currentPos + 1 ) )
                ->list();
        for ( ; curElem != nullptr;
              nbElemPar1++, curElem = curElem->nextLink() )
          ;
        Idx nbElemCur1 = 0;
        curElem = __fg1->varNodeListe( *sifIter )->list();
        for ( ; curElem != nullptr;
              nbElemCur1++, curElem = curElem->nextLink() )
          ;

        Idx nbElemPar2 = 0;
        curElem = __fg2->varNodeListe(
                           __fg2->variablesSequence().atPos( currentPos + 1 ) )
                      ->list();
        for ( ; curElem != nullptr;
              nbElemPar2++, curElem = curElem->nextLink() )
          ;
        Idx nbElemCur2 = 0;
        curElem = __fg2->varNodeListe( *sifIter )->list();
        for ( ; curElem != nullptr;
              nbElemCur2++, curElem = curElem->nextLink() )
          ;

        if ( __fg1->realSize() +
                     ( nbElemCur1 + nbElemPar1 ) * ( *sifIter )->domainSize() >
                 100000 ||
             __fg2->realSize() +
                     ( nbElemCur2 + nbElemPar2 ) * ( *sifIter )->domainSize() >
                 100000 )
          break;


        // Do the SWAP if it's ok
        __fg1->manager()->moveTo( *sifIter, currentPos + 1 );
        __fg2->manager()->moveTo( *sifIter, currentPos + 1 );
        currentPos++;
        if ( __fg1->realSize() + __fg2->realSize() < bestSize ) {
          bestPos = currentPos;
          bestSize = __fg1->realSize() + __fg2->realSize();
        }
      }

      __fg1->manager()->moveTo( *sifIter, bestPos );
      __fg2->manager()->moveTo( *sifIter, bestPos );
    }
  }
}
