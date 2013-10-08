/****************************************************************************
 *    Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                                *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program; if not, write to the                          *
 *   Free Software Foundation, Inc.,                                        *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ****************************************************************************/
/**
* @file
* @brief Files containing structures and functions used to manipulate data structures in Decision Diagram Operations
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/
// ================================================================
#ifndef GUM_DECISION_GRAPH_OPERATOR_DATA
#define GUM_DECISION_GRAPH_OPERATOR_DATA
// ================================================================
#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>
// ================================================================
#include <agrum/variables/discreteVariable.h>
// ================================================================
#include <agrum/multidim/multiDimDecisionGraph.h>
#include <agrum/multidim/patterns/DGUtility/o4DGContext.h>
// ================================================================


namespace gum {

//========================================================================================================================================
/// DGOperatorData contains all data structure needed to compute the outcoming diagram.
//========================================================================================================================================
  template< typename GUM_SCALAR >
  class DGOperatorData {

    public :
      /// The two operated diagrams
      const MultiDimDecisionGraph< GUM_SCALAR >* DG1;
      const MultiDimDecisionGraph< GUM_SCALAR >* DG2;

      /// The resulting diagram
      MultiDimDecisionGraph< GUM_SCALAR >* DG3;

      /// Context handler
      O4DGContext conti;

      /// Table to remind us wich part of both diagram have already been explored, and the resulting node
      HashTable< double, NodeId >* explorationTable;

      /** ********************************************************************************************** **
       ** Default constructor
       ** ********************************************************************************************** **/
      DGOperatorData( ) {    }

      /** ********************************************************************************************** **
       ** Initializes the data structure with operated DG1 et DG2, and compute order for returned diagram
       ** ********************************************************************************************** **/
      virtual void initialize( const MultiDimDecisionGraph< GUM_SCALAR >* DG1, const MultiDimDecisionGraph< GUM_SCALAR >* DG2 ) {

        // First, we determine leading order and the sens in which we perform operation on diagrams
        Sequence< const DiscreteVariable* > leadingOrder;
        _getLeadingSequence( DG1, DG2, leadingOrder );


        // Next, we initialize node for exploration departure
        this->conti.setDG1Node( this->DG1->root() );
        this->conti.setDG2Node( this->DG2->root() );

//#ifdef O4DGDEBUG
//        GUM_TRACE( " Mixed Sequence variable : " );

//        for ( SequenceIterator< const DiscreteVariable* > iter = leadingOrder.begin(); iter != leadingOrder.end(); ++iter )
//          GUM_TRACE( ( *iter )->toString() << " - " );

//        GUM_TRACE( std::endl );
//#endif


        // Then we instantiate the DG3 that will create the new multidim
        // give it its new bounds, and set its variable sequence
        this->DG3 = static_cast<MultiDimDecisionGraph< GUM_SCALAR >*>( this->DG1->newFactory() );
//        DG3->GUM_APPROXIMATION_COMBINE_FUNCTION( DG2 );
        for ( SequenceIterator< const DiscreteVariable* > iter = leadingOrder.begin(); iter != leadingOrder.end(); ++iter ){
//          GUM_TRACE( ( *iter )->toString() << " - " );
          this->DG3->add(**iter);
        }

        // And we finally instantiates the other data structures
        explorationTable = new HashTable< double, NodeId >( this->DG1->realSize()*this->DG2->realSize(), true, false );
      }


      /** **************************************************************************************************
        ** Default destructor
        ** ********************************************************************************************** **/
      ~DGOperatorData() {
        delete explorationTable;
      }

    protected :
      virtual void _getLeadingSequence( const MultiDimDecisionGraph< GUM_SCALAR >* DG1, const MultiDimDecisionGraph< GUM_SCALAR >* DG2, gum::Sequence< const DiscreteVariable* >& leadingOrder ) = 0;
  };



//========================================================================================================================================
/// DGOperatorData contains all is needed to perform an operation beteween seamingly ordered decision diagram.
//========================================================================================================================================
  template< typename GUM_SCALAR >
  class DGOrderedOperatorData : public DGOperatorData< GUM_SCALAR > {

    public :

      /** **************************************************************************************************
       ** Default constructor
       ** ********************************************************************************************** **/
      DGOrderedOperatorData( ) : DGOperatorData< GUM_SCALAR >( ) {}

    protected :

      /** **************************************************************************************************
       ** The very implementation of the function that gave order for final diagram
       ** ********************************************************************************************** **/
      void  _getLeadingSequence( const MultiDimDecisionGraph< GUM_SCALAR >* DG1, const MultiDimDecisionGraph< GUM_SCALAR >* DG2, Sequence< const DiscreteVariable* >& leadingOrder ) {
        leadingOrder = DG1->variablesSequence();
      }
  };



//========================================================================================================================================
/// DGOperatorData contains all is needed to perform an operation beteween non seamingly ordered decision diagram.
//========================================================================================================================================
  template< typename GUM_SCALAR >
  class DGNonOrderedOperatorData : public DGOperatorData< GUM_SCALAR > {

    public:

      /** **************************************************************************************************
       ** Default constructor
       ** ********************************************************************************************** **/
      DGNonOrderedOperatorData( ) : DGOperatorData< GUM_SCALAR >( ) {    }

      virtual void initialize( const MultiDimDecisionGraph< GUM_SCALAR >* DG1, const MultiDimDecisionGraph< GUM_SCALAR >* DG2 ) {

        DGOperatorData< GUM_SCALAR >::initialize( DG1, DG2 );

        this->conti.retrogradeVarTable = new HashTable< NodeId, Set< const DiscreteVariable* >* >( this->DG2->realSize(), true, false );
        HashTable< NodeId, Set< const DiscreteVariable* >* > reachableVars(this->DG2->model().size(), true, false);
        __findRetrogradeVariables( this->DG2->root(), reachableVars );
        for ( HashTableIterator< NodeId, Set< const DiscreteVariable* >* > reachVarIter = reachableVars.begin(); reachVarIter != reachableVars.end(); ++reachVarIter )
            delete *reachVarIter;

//#ifdef O4DGDEBUG
//        GUM_TRACE( "RETRO VAR TABLE : " << *(this->conti.retrogradeVarTable ));
//#endif

        for ( HashTableIterator< NodeId, Set< const DiscreteVariable* >* > retroVarIter = this->conti.retrogradeVarTable->begin();
              retroVarIter != this->conti.retrogradeVarTable->end(); ++retroVarIter )
          if ( *retroVarIter != nullptr && !( *retroVarIter )->empty() ) {

//#ifdef O4DGDEBUG
//            GUM_TRACE( "\tNode : " << retroVarIter.key() );
//#endif

            for ( SetIterator< const DiscreteVariable* > iter = ( *retroVarIter )->begin(); iter != ( *retroVarIter )->end(); ++iter ) {

//#ifdef O4DGDEBUG
//              GUM_TRACE( "\t\tVariable : " << ( *iter )->name() );
//#endif

              if ( ! this->conti.isRetrogradeVar( *iter ) )
                this->conti.addRetrogradeVar( *iter );
            }
          }
      }

      ~DGNonOrderedOperatorData() {
        for ( HashTableIterator< NodeId, Set< const DiscreteVariable* >* > iterH = this->conti.retrogradeVarTable->begin();
              iterH != this->conti.retrogradeVarTable->end(); ++iterH )
          delete *iterH;

        delete this->conti.retrogradeVarTable;
      }


    protected :
      /** **************************************************************************************************
       ** The very implementation of the function that gave order for final diagram
       ** ********************************************************************************************** **/
      virtual void _getLeadingSequence( const MultiDimDecisionGraph< GUM_SCALAR >* DG1, const MultiDimDecisionGraph< GUM_SCALAR >* DG2,
                                        gum::Sequence< const DiscreteVariable* >& leadingOrder ) {

//#ifdef GUM_MULTI_DIM_DECISION_GRAPH_RECUR_FUNCTION == DecisionGraphRecur4Subtraction || GUM_MULTI_DIM_DECISION_GRAPH_RECUR_FUNCTION == DecisionGraphRecur4Division
//         __makeMergedVariableSequence( DG1->variablesSequence(), DG2->variablesSequence(), leadingOrder );
//         this->DG1 = DG1;
//         this->DG2 = DG2;
//#else
        Sequence< const DiscreteVariable* > mergedVarSeq1;
        __makeMergedVariableSequence( DG1->variablesSequence(), DG2->variablesSequence(), mergedVarSeq1 );
        Sequence< const DiscreteVariable* > mergedVarSeq2;
        __makeMergedVariableSequence( DG2->variablesSequence(), DG1->variablesSequence(), mergedVarSeq2 );

        if ( __evalRetrogradeVarSpaceSize( mergedVarSeq1, DG2->variablesSequence() ) <= __evalRetrogradeVarSpaceSize( mergedVarSeq2, DG1->variablesSequence() ) ) {
          leadingOrder = mergedVarSeq1;
          this->DG1 = DG1;
          this->DG2 = DG2;
        } else {
          leadingOrder = mergedVarSeq2;
          this->DG1 = DG2;
          this->DG2 = DG1;
        }

//#endif
      }

    private :

      /** **************************************************************************************************
       ** Creates the sequence of variable for returned diagram considering the first variables sequence
       ** given in parameter is the leading one.
       ** ********************************************************************************************** **/
      void  __makeMergedVariableSequence( const Sequence< const DiscreteVariable* >& DG1VarSeq, const Sequence< const DiscreteVariable* >& DG2VarSeq, Sequence< const DiscreteVariable* >& mergedVarSeq ) {

        SequenceIterator< const DiscreteVariable* > iterS1 = DG1VarSeq.begin();
        SequenceIterator< const DiscreteVariable* > iterS2 = DG2VarSeq.begin();

        while ( iterS1 != DG1VarSeq.end() || iterS2 != DG2VarSeq.end() ) {
          if ( iterS1 == DG1VarSeq.end() ) {
            for ( ; iterS2 != DG2VarSeq.end(); ++iterS2 )
              if ( !mergedVarSeq.exists( *iterS2 ) )
                mergedVarSeq.insert( *iterS2 );
          } else if ( iterS2 == DG2VarSeq.end() ) {
            for ( ; iterS1 != DG1VarSeq.end(); ++iterS1 )
              if ( !mergedVarSeq.exists( *iterS1 ) )
                mergedVarSeq.insert( *iterS1 );
          } else {
            if ( *iterS1 == *iterS2 ) {
              if ( !mergedVarSeq.exists( *iterS1 ) )
                mergedVarSeq.insert( *iterS1 );

              ++iterS1;
              ++iterS2;
              continue;
            }

            if ( DG1VarSeq.pos( *iterS1 ) <= DG2VarSeq.pos( *iterS2 ) || DG1VarSeq.exists( *iterS2 ) ) {
              if ( !mergedVarSeq.exists( *iterS1 ) )
                mergedVarSeq.insert( *iterS1 );

              ++iterS1;
              continue;
            } else {
              if ( !mergedVarSeq.exists( *iterS2 ) )
                mergedVarSeq.insert( *iterS2 );

              ++iterS2;
              continue;
            }
          }
        }
      }



      /** **************************************************************************************************
        ** Computes size of retrograde var state space generated by the final sequence
        ** ********************************************************************************************** **/
      Idx __evalRetrogradeVarSpaceSize( const Sequence< const DiscreteVariable* >& leadingVarSeq, const Sequence< const DiscreteVariable* >& followingVarSeq ) {

        SequenceIterator< const DiscreteVariable* > iterSfin = followingVarSeq.begin(), iterSfol = followingVarSeq.begin();

        // ******************************************************************************
        // Then we search in second diagram for possible preneeded variable
        Idx sizeRetro = 1;

        for ( iterSfol = followingVarSeq.begin(); iterSfol != followingVarSeq.end(); ++iterSfol )
          for ( iterSfin = iterSfol; iterSfin != followingVarSeq.rend(); --iterSfin )
            if ( leadingVarSeq.pos( *iterSfin ) > leadingVarSeq.pos( *iterSfol ) )
              sizeRetro *= ( *iterSfol )->domainSize();

        return sizeRetro;
      }






      // Returns a hashtable containing for each node a list of variable
      // This method looks, for each path in the diagram, if a var does not precede others in
      // the given in parameter order.
      void __findRetrogradeVariables ( NodeId currentNode, HashTable<NodeId, Set<const DiscreteVariable*>*>& reachableVars ) {

          if( this->DG2->isTerminalNode(currentNode)){
              reachableVars.insert( currentNode, new Set<const DiscreteVariable*>() );
              return;
          }

          Set< const DiscreteVariable* >* myReachableVars = new Set<const DiscreteVariable*>();
          Set< const DiscreteVariable* >* myRetrogradeVars  = new Set<const DiscreteVariable*>();
          for( Idx sonIndex = 0; sonIndex < this->DG2->node(currentNode)->nodeVar->domainSize(); sonIndex++ ){
              NodeId currentSonId = this->DG2->node(currentNode)->nodeSons[sonIndex];
              if( ! reachableVars.exists(currentSonId))
                __findRetrogradeVariables( currentSonId, reachableVars );
              for( SetIterator<const DiscreteVariable*> varIter = reachableVars[currentSonId]->begin(); varIter != reachableVars[currentSonId]->end(); ++varIter){
                  if( ! myReachableVars->exists( *varIter )){
                      myReachableVars->insert( *varIter );
                      if( this->DG3->variablesSequence().pos( *varIter ) < this->DG3->variablesSequence().pos( this->DG2->node(currentNode)->nodeVar ) )
                          myRetrogradeVars->insert( * varIter );
                  }
              }
          }
          this->conti.retrogradeVarTable->insert(currentNode, myRetrogradeVars);
          reachableVars.insert( currentNode, myReachableVars);
      }

  };



/// Insertion of a node when we know all its sons and its possible default son

//  template< typename GUM_SCALAR>
//  NodeId
//  insertNonTerminalNode( DGOperatorData< GUM_SCALAR >& opData, const DiscreteVariable* associatedVariable, NodeId* sonsMap, NodeId defaultSon , const HashTable< NodeId, Idx >& countTable ) {

//    HashTableConstIterator< NodeId, Idx > ctIter = countTable.begin();

//    if ( countTable.size() == 1 && ( defaultSon == 0 || ctIter.key() == defaultSon ) )
//      return  ctIter.key();

//    if ( associatedVariable->domainSize() < 5 ) {

//      //********************************************************************************************************************************************
//      if ( defaultSon != 0 ) {
//        Idx nbDefault = 0;

//        for ( std::vector<NodeId >::iterator iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end(); ++iterArcMap ) {
//          if ( *iterArcMap == 0 )
//            ++nbDefault;

//          if ( *iterArcMap == defaultSon ) {
//            ++nbDefault;
//            sonsMap[ std::distance( sonsMap.begin(), iterArcMap ) ] = 0;
//          }
//        }

//        if ( nbDefault == 1 )
//          for ( std::vector<NodeId >::iterator iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end(); ++iterArcMap )
//            if ( *iterArcMap == 0 ) {
//              sonsMap[ std::distance( sonsMap.begin(), iterArcMap ) ] = defaultSon;
//              defaultSon = 0;
//              break;
//            }
//      }

//      //********************************************************************************************************************************************
//    } else {

//      Idx max = 0;
//      NodeId maxNode = 0;

//      while ( ctIter != countTable.end() ) {
//        if ( *ctIter > max ) {
//          max = *ctIter;
//          maxNode = ctIter.key();
//        }

//        ++ctIter;
//      }

//      if ( max == 1 ) {
//        if ( defaultSon != 0 ) {
//          for ( std::vector<NodeId >::iterator iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end(); ++iterArcMap )
//            if ( *iterArcMap == 0 ) {
//              sonsMap[ std::distance( sonsMap.begin(), iterArcMap ) ] = defaultSon;
//              defaultSon = 0;
//              break;
//            }
//        }
//      } else {
//        if ( maxNode != defaultSon ) {
//          for ( std::vector<NodeId >::iterator iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end(); ++iterArcMap ) {
//            if ( *iterArcMap == 0 )
//              sonsMap[ std::distance( sonsMap.begin(), iterArcMap ) ] = defaultSon;

//            if ( *iterArcMap == maxNode )
//              sonsMap[ std::distance( sonsMap.begin(), iterArcMap ) ] = 0;
//          }

//          defaultSon = maxNode;
//        } else {
//          for ( std::vector<NodeId >::iterator iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end(); ++iterArcMap )
//            if ( *iterArcMap == maxNode )
//              sonsMap[ std::distance( sonsMap.begin(), iterArcMap ) ] = 0;
//        }
//      }
//    }

//#ifdef O4DGDEBUG
//    return opData.DG3->aDGNonTerminalNodeWithArcs( associatedVariable, sonsMap, defaultSon );
//#else
   // return opData.DG3->manager()->addNonTerminalNodeWithArcs( associatedVariable, sonsMap );//, defaultSon );
//#endif
//  }

}/* end of namespace GUM */

#endif /*GUM_DECISION_GRAPH_OPERATOR_DATA*/

