/*********************************************************************************
 *  Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES          *
 *  {prenom.nom}_at_lip6.fr                                                      *
 *                                                                               *
 *  This program is free software; you can redistribute it and/or modify         *
 *  it under the terms of the GNU General Public LiceDG2NodeIde as published by  *
 *  the Free Software Foundation; either version 2 of the LiceDG2NodeIde, or     *
 *  (at your option) any later version.                                          *
 *                                                                               *
 *  This program is distributed in the hope that it will be useful,              *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 *
 *  GNU General Public LiceDG2NodeIde for more details.                          *
 *                                                                               *
 *  You should have received a copy of the GNU General Public LiceDG2NodeIde     *
 *  along with this program; if not, write to the                                *
 *  Free Software Foundation, Inc.,                                              *
 *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                     *
 *********************************************************************************/
/**
* @file
* @brief Class used to compute the Projection between two decision diagrams
*
* @author Jean-Christophe Magnan
*/

// =======================================================
#include <agrum/multidim/patterns/DGUtility/multiDimDecisionGraphProjector.h>
// =======================================================
#include <agrum/variables/labelizedVariable.h>

namespace gum {

  /* ***************************************************************************************************************************** */
  /* CONSTRUCTOR                                                          */
  /* ***************************************************************************************************************************** */
  template <typename GUM_SCALAR, template <typename> class FUNCTOR >
  MultiDimDecisionGraphProjector<GUM_SCALAR, FUNCTOR>::MultiDimDecisionGraphProjector( const MultiDimDecisionGraph<GUM_SCALAR>* src,
                                                                                       const Set<const DiscreteVariable*>& delVars,
                                                                                       const GUM_SCALAR neutral ):
                                                                                       __src( src ),
                                                                                       __delVars( delVars ),
                                                                                       __function(),
                                                                                       __neutral( neutral ){
    GUM_CONSTRUCTOR(MultiDimDecisionGraphProjector);
    __rd = new MultiDimDecisionGraph<GUM_SCALAR>();
  }



  /* ***************************************************************************************************************************** */
  /* DESTRUCTOR                                                          */
  /* ***************************************************************************************************************************** */
  template <typename GUM_SCALAR, template <typename> class FUNCTOR >
  MultiDimDecisionGraphProjector<GUM_SCALAR, FUNCTOR>::~MultiDimDecisionGraphProjector( ){
    GUM_DESTRUCTOR(MultiDimDecisionGraphProjector);
  }



  /* ***************************************************************************************************************************** */
  /* Compute                                                            */
  /*                                                                */
  /* This function is the main function. To be call every time an Projection between the two given Decision Graphs is required   */
  /* ***************************************************************************************************************************** */
  template <typename GUM_SCALAR, template <typename> class FUNCTOR >
  MultiDimDecisionGraph<GUM_SCALAR>*
  MultiDimDecisionGraphProjector<GUM_SCALAR, FUNCTOR>::project(){

//      Sequence<const DiscreteVariable*> seq;
//      __maker(__delVars, seq, "");
////        for( SetIteratorSafe<const DiscreteVariable*> varIter = __delVars.beginSafe(); varIter != __delVars.endSafe(); ++varIter )
////            if( !(*varIter)->name().compare("var4")){
////                seq.insert(*varIter);
////                std::cout << (*varIter)->name() << std::endl;
////                break;
////            }
////        for( SetIteratorSafe<const DiscreteVariable*> varIter = __delVars.beginSafe(); varIter != __delVars.endSafe(); ++varIter )
////            if( !(*varIter)->name().compare("var6")){
////                seq.insert(*varIter);
////                break;
////            }
////        for( SetIteratorSafe<const DiscreteVariable*> varIter = __delVars.beginSafe(); varIter != __delVars.endSafe(); ++varIter )
////            if( !(*varIter)->name().compare("var0")){
////                seq.insert(*varIter);
////                break;
////            }
////        for( SetIteratorSafe<const DiscreteVariable*> varIter = __delVars.beginSafe(); varIter != __delVars.endSafe(); ++varIter )
////            if( !(*varIter)->name().compare("var1")){
////                seq.insert(*varIter);
////                break;
////            }
////        for( SetIteratorSafe<const DiscreteVariable*> varIter = __delVars.beginSafe(); varIter != __delVars.endSafe(); ++varIter )
////            if( !(*varIter)->name().compare("var5")){
////                seq.insert(*varIter);
////                break;
////            }
////        for( SetIteratorSafe<const DiscreteVariable*> varIter = __delVars.beginSafe(); varIter != __delVars.endSafe(); ++varIter )
////            if( !(*varIter)->name().compare("var2")){
////                seq.insert(*varIter);
////                break;
////            }

//      return nullptr;
//  }

//  template <typename GUM_SCALAR, template <typename> class FUNCTOR >
//  void
//  MultiDimDecisionGraphProjector<GUM_SCALAR, FUNCTOR>::__maker(Set<const DiscreteVariable*> remainingVar, Sequence<const DiscreteVariable*> seq, std::string tab){

//      Set<const DiscreteVariable*> boulga(remainingVar);
//      for( SetIteratorSafe<const DiscreteVariable*> varIter = remainingVar.beginSafe(); varIter != remainingVar.endSafe(); ++varIter ){
//          std::cout << tab << (*varIter)->name() << std::endl;
//          boulga >> *varIter;
//          seq << *varIter;
//          if(boulga.empty()){
//              __project(seq);
//          } else {
//              __maker(boulga, seq, tab + "\t");
//          }
//          boulga << *varIter;
//          seq >> *varIter;
//      }
//  }

//  template <typename GUM_SCALAR, template <typename> class FUNCTOR >
//  MultiDimDecisionGraph<GUM_SCALAR>*
//  MultiDimDecisionGraphProjector<GUM_SCALAR, FUNCTOR>::__project(Sequence<const DiscreteVariable*> seq){
      __rd->copy( *__src );

    for( SetIteratorSafe<const DiscreteVariable*> varIter = __delVars.beginSafe(); varIter != __delVars.endSafe(); ++varIter ){
//    for( SequenceIteratorSafe<const DiscreteVariable*> varIter = seq.beginSafe(); varIter != seq.endSafe(); ++varIter ){

      const DiscreteVariable* curVar = *varIter;






//      std::cout << std::endl << " " << std::endl << " " << std::endl << " *************************************************************************************" << std::endl;


//      std::cout << "Removing " << curVar->name() << std::endl;


//      std::cout << "Diagram before swapping : " << std::endl << __rd->toDot() << std::endl;

//      for(SequenceIteratorSafe<const DiscreteVariable*> varIter = __rd->variablesSequence().beginSafe();
//          varIter != __rd->variablesSequence().endSafe(); ++varIter){

//        std::cout << "Variable : " << (*varIter)->name();

//        const typename MultiDimDecisionGraph<GUM_SCALAR>::NICLElem* nodeIter = __rd->varNodeListe( *varIter );
//        while( nodeIter ){
//          std::cout << " | " << nodeIter->elemId << " - ";
//          const typename MultiDimDecisionGraph<GUM_SCALAR>::PICLElem* parentIter = __rd->node(nodeIter->elemId)->parentsIter();
//          while(parentIter){
//              std::cout << " " << parentIter->parentId << " " << parentIter->modality << "   ";
//              parentIter = parentIter->nextElem;
//          }
//          nodeIter = nodeIter->nextElem;
//        }
//        std::cout << std::endl;
//      }








      if( __rd->variablesSequence().exists(curVar) )
        __rd->manager()->moveTo( curVar, __rd->variablesSequence().size() - 1 );












//      std::cout << "Diagram After swapping : " << std::endl << __rd->toDot(true) << std::endl;

//      for(SequenceIteratorSafe<const DiscreteVariable*> varIter = __rd->variablesSequence().beginSafe();
//          varIter != __rd->variablesSequence().endSafe(); ++varIter){
//        std::cout << "Variable : " << (*varIter)->name();
//        const typename MultiDimDecisionGraph<GUM_SCALAR>::NICLElem* nodeIter = __rd->varNodeListe( *varIter );
//        while( nodeIter ){
//          std::cout << " | " << nodeIter->elemId << " - ";
//          const typename MultiDimDecisionGraph<GUM_SCALAR>::PICLElem* parentIter = __rd->node(nodeIter->elemId)->parentsIter();
//          while(parentIter){
//              std::cout << " " << parentIter->parentId << " " << parentIter->modality << "   ";
//              parentIter = parentIter->nextElem;
//          }
//          nodeIter = nodeIter->nextElem;
//        }
//        std::cout << std::endl;
//      }
//      std::cout << " " << std::endl << " " << std::endl;















      HashTable<NodeId, NodeId> visitedNode(2*__rd->realSize(),true,false);
      std::vector<NodeId> filo;
      filo.push_back( __rd->root() );

      while( !filo.empty() ){
//        std::cout << "New Iteration | filo : " << filo << " | Visited Node : " << visitedNode << std::endl;
        NodeId curNodeId = filo.back();
        filo.pop_back();

//        std::cout << " Cur Node : " << curNodeId << std::endl;

        const typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* curNode = __rd->node( curNodeId );

        for(Idx modality = 0; modality < curNode->nodeVar()->domainSize(); ++modality ){

          NodeId oldSonId = curNode->son(modality);

//          std::cout << "\tModality : " << modality << " => Son : " << oldSonId << std::endl;

          if( !visitedNode.exists( oldSonId ) ) {

//            std::cout << "\tNode non visited" << std::endl;

            NodeId newSonId = oldSonId;

            if( !__rd->isTerminalNode( oldSonId ) ){

//                std::cout << "\t\tNode non terminal" << std::endl;


              if( __rd->node( oldSonId )->nodeVar() != curVar ){

//                std::cout << "\t\t\tNode var not projected" << std::endl;
                filo.push_back( oldSonId );

              } else {

//                std::cout << "\t\t\tNode var projected" << std::endl;


                const typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* curVarNode = __rd->node( oldSonId );
                GUM_SCALAR newVal = __neutral;
                for( Idx curVarModality = 0; curVarModality < curVar->domainSize(); ++curVarModality )
                  newVal = __function(newVal, __rd->nodeValue( curVarNode->son( curVarModality ) ) );

                newSonId = __rd->manager()->addTerminalNode( newVal );

                __rd->manager()->eraseNode( oldSonId, newSonId, false );
                __rd->manager()->setSon( curNodeId, modality, newSonId );

//                std::cout << "\t\t\t" << oldSonId << " => " << newSonId << " - " << __rd->nodeValue(newSonId) << std::endl;

              }

            } else {

//              std::cout << "\t\tNode terminal" << std::endl;


              GUM_SCALAR newVal = __neutral, oldVal = __rd->nodeValue( oldSonId );
              for( Idx curVarModality = 0; curVarModality < curVar->domainSize(); ++curVarModality )
                newVal = __function( newVal, oldVal );

              newSonId = __rd->manager()->addTerminalNode( newVal );
              __rd->manager()->setSon( curNodeId, modality, newSonId );

//              std::cout << "\t\t" << oldSonId << " => " << newSonId << " - " << __rd->nodeValue(newSonId) << std::endl;

            }

            visitedNode.insert( oldSonId, newSonId );

          } else {

//              std::cout << "\tNode visited" << std::endl;

            if( __rd->node(curNodeId)->son(modality) != visitedNode[oldSonId] )
              __rd->manager()->setSon( curNodeId, modality, visitedNode[oldSonId] );

//            std::cout << "\t" << oldSonId << " => " << visitedNode[oldSonId] << std::endl;

          }
        }
      }

//      std::cout << "Projection done." << std::endl;


//      std::cout << "Diagram After Projection : " << std::endl << __rd->toDot() << std::endl;

//      for(SequenceIteratorSafe<const DiscreteVariable*> varIter = __rd->variablesSequence().beginSafe();
//          varIter != __rd->variablesSequence().endSafe(); ++varIter){
//        std::cout << "Variable : " << (*varIter)->name();
//        const typename MultiDimDecisionGraph<GUM_SCALAR>::NICLElem* nodeIter = __rd->varNodeListe( *varIter );
//        while( nodeIter ){
//          std::cout << " | " << nodeIter->elemId << " - ";
//          const typename MultiDimDecisionGraph<GUM_SCALAR>::PICLElem* parentIter = __rd->node(nodeIter->elemId)->parentsIter();
//          while(parentIter){
//              std::cout << " " << parentIter->parentId << " " << parentIter->modality << "   ";
//              parentIter = parentIter->nextElem;
//          }
//          nodeIter = nodeIter->nextElem;
//        }
//        std::cout << std::endl;
//      }
//      std::cout << " " << std::endl << " " << std::endl;



      if( __rd->variablesSequence().exists(curVar) )
          __rd->erase( *curVar );

//      std::cout << "Variable removed from sequence." << std::endl;


//      std::cout << "Diagram After Var Removal : " << std::endl << __rd->toDot() << std::endl;

//      for(SequenceIteratorSafe<const DiscreteVariable*> varIter = __rd->variablesSequence().beginSafe();
//          varIter != __rd->variablesSequence().endSafe(); ++varIter){
//        std::cout << "Variable : " << (*varIter)->name();
//        const typename MultiDimDecisionGraph<GUM_SCALAR>::NICLElem* nodeIter = __rd->varNodeListe( *varIter );
//        while( nodeIter ){
//          std::cout << " | " << nodeIter->elemId << " - ";
//          const typename MultiDimDecisionGraph<GUM_SCALAR>::PICLElem* parentIter = __rd->node(nodeIter->elemId)->parentsIter();
//          while(parentIter){
//              std::cout << " " << parentIter->parentId << " " << parentIter->modality << "   ";
//              parentIter = parentIter->nextElem;
//          }
//          nodeIter = nodeIter->nextElem;
//        }
//        std::cout << std::endl;
//      }
//      std::cout << " " << std::endl << " " << std::endl;

    }

//    std::cout << "DONE" << std::endl;
    return __rd;
  }

} // namespace gum

