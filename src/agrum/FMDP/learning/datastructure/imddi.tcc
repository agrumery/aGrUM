/*********************************************************************************
 * Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES           *
 * {prenom.nom}_at_lip6.fr                                                       *
 *                                                                               *
 * This program is free software; you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by          *
 * the Free Software Foundation; either version 2 of the License, or             *
 * (at your option) any later version.                                           *
 *                                                                               *
 * This program is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  *
 * GNU General Public License for more details.                                  *
 *                                                                               *
 * You should have received a copy of the GNU General Public License             *
 * along with this program; if not, write to the                                 *
 * Free Software Foundation, Inc.,                                               *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                      *
 *********************************************************************************/
/**
* @file Template Implementations of the IMDDI datastructure learner
* @brief
*
* @author Jean-Christophe Magnan
*/
// =======================================================
#include <cmath>
// =======================================================
#include <agrum/core/types.h>
#include <agrum/core/priorityQueue.h>
// =======================================================
#include <agrum/FMDP/learning/datastructure/imddi.h>
#include <agrum/FMDP/learning/core/chiSquare.h>
// =======================================================
#include <agrum/variables/discreteVariable.h>
// =======================================================

#define ALLOCATE(x) SmallObjectAllocator::instance().allocate(x)
#define DEALLOCATE(x,y) SmallObjectAllocator::instance().deallocate(x,y)

namespace gum {

  // ############################################################################
  // Constructor & destructor.
  // ############################################################################

    // ============================================================================
    // Variable Learner constructor
    // ============================================================================
    template <TESTNAME AttributeSelection, bool isScalar >
    IMDDI<AttributeSelection, isScalar>::IMDDI ( MultiDimDecisionGraph<double>* target,
            double attributeSelectionThreshold,
            double pairSelectionThreshold,
            Set<const DiscreteVariable*> attributeListe,
            const DiscreteVariable* learnedValue ) : IncrementalGraphLearner( target, attributeListe, learnedValue),
                                                     __nbTotalObservation(0),
                                                     __attributeSelectionThreshold(attributeSelectionThreshold),
                                                     __pairSelectionThreshold(pairSelectionThreshold)
    {GUM_CONSTRUCTOR(IMDDI);}

    // ============================================================================
    // Reward Learner constructor
    // ============================================================================
    template <TESTNAME AttributeSelection, bool isScalar >
    IMDDI<AttributeSelection, isScalar>::IMDDI ( MultiDimDecisionGraph<double>* target,
            double attributeSelectionThreshold,
            double pairSelectionThreshold,
            Set<const DiscreteVariable*> attributeListe ) : IncrementalGraphLearner( target, attributeListe ),
                                                            __nbTotalObservation(0),
                                                            __attributeSelectionThreshold(attributeSelectionThreshold),
                                                            __pairSelectionThreshold(pairSelectionThreshold)
    {GUM_CONSTRUCTOR(IMDDI);}



  // ############################################################################
  // Incrementals methods
  // ############################################################################

    template <TESTNAME AttributeSelection, bool isScalar >
    void IMDDI<AttributeSelection, isScalar>::addObservation ( const Observation* obs ){
      __nbTotalObservation++;
      IncrementalGraphLearner<AttributeSelection, isScalar>::addObservation(obs);
    }


    // ============================================================================
    // Updates the tree after a new observation has been added
    // ============================================================================
    template <TESTNAME AttributeSelection, bool isScalar >
    void IMDDI<AttributeSelection, isScalar>::updateGraph(){

      __varOrder.clear();

      // First xe initialize the node set which will give us the scores
      Set<NodeId> currentNodeSet;
      currentNodeSet.insert( _root );

      // Then we initialize the pool of variables to consider
      Set<const DiscreteVariable*> remainingVars(_setOfVars);

      PriorityQueue<Set<const DiscreteVariable*>*, double, std::greater<double>> remainingVarsScore;
      HashTable<double,Set<const DiscreteVariable*>*> remainingVarsScoreTable;
      double score = 0.0;

      remainingVarsScoreTable.insert(score, new Set<const DiscreteVariable*>());
      remainingVarsScore.insert( remainingVarsScoreTable[score], score );
      
      for( auto varIter = remainingVars.beginSafe(); varIter != remainingVars.endSafe(); ++varIter ) {
        if( _nodeId2Database[_root]->isTestRelevant(*varIter) )
          score = _score(*varIter, _root);
        else
          score = 0.0;

        if( ! remainingVarsScoreTable.exists(score)){
          remainingVarsScoreTable.insert(score, new Set<const DiscreteVariable*>());
          remainingVarsScore.insert( remainingVarsScoreTable[score], score );
        }
        remainingVarsScoreTable[score]->insert(*varIter);

      }

      // Then, until there's no node remaining
      while( !remainingVars.empty() ){

        // We select the best var
        const DiscreteVariable* selectedVar = remainingVarsScore.pop();
        remainingVars >> selectedVar;
        __varOrder.insert(selectedVar);

        // Then we decide if we update each node according to this var
        __updateNodeSet( currentNodeSet, selectedVar, remainingVarsScore );

      }

      // If there are remaining node that are not leaves after we establish the var order
      // these nodes are turned into leaf.

      for( SetIteratorSafe<NodeId> nodeIter = currentNodeSet.beginSafe(); nodeIter != currentNodeSet.endSafe(); ++nodeIter )
        _convertNode2Leaf(*nodeIter);

    }



  // ############################################################################
  // Updating methods
  // ############################################################################

    // ============================================================================
    // Select the most relevant variable
    //
    // First parameter is the set of variables among which the most
    // relevant one is choosed
    // Second parameter is the set of node the will attribute a score
    // to each variable so that we choose the best.
    // ============================================================================
    template <TESTNAME AttributeSelection, bool isScalar >
    INLINE
    double IMDDI<AttributeSelection, isScalar>::__score( const DiscreteVariable* var, NodeId nody){

      double weight = (double)_nodeId2Database[nody]->nbObservation() / (double) this->__nbTotalObservation;
      return weight*_nodeId2Database[nody]->testValue( var );
    }


    // ============================================================================
    // For each node in the given set, this methods checks whether or not
    // we should installed the given variable as a test.
    // If so, the node is updated
    // ============================================================================
    template <TESTNAME AttributeSelection, bool isScalar >
    void IMDDI<AttributeSelection, isScalar>::__updateNodeSet( Set<NodeId>& nodeSet,
                         const DiscreteVariable* selectedVar,
                         MultiPriorityQueue<const DiscreteVariable*, double, std::greater<double>>& remainingVarsScore ){

      Set<NodeId> oldNodeSet(nodeSet);
      nodeSet.clear();
      for( SetIteratorSafe<NodeId> nodeIter = oldNodeSet.beginSafe(); nodeIter != oldNodeSet.endSafe(); ++nodeIter ){

        if( __nodeId2Database[*nodeIter]->isPValueRelevant( selectedVar)
                && __nodeId2Database[*nodeIter]->pValue( selectedVar) > __attributeSelectionThreshold ){

          __transpose(*nodeIter, selectedVar);

          // Then we subtract the from the score given to each variables the quantity given by this node
          for( auto varIter = remainingVarsScore.allValues().cbeginSafe(); varIter != remainingVarsScore.allValues().cendSafe(); ++varIter ) {
            if( __nodeId2Database[*nodeIter]->isPValueRelevant( varIter.key() ) ){
              double newPriority = remainingVarsScore.priority( varIter.key() ) - __score( varIter.key(), *nodeIter );
              remainingVarsScore.setPriority(varIter.key(), newPriority);
            }
          }

          // And finally we add all its child to the new set of nodes
          // and updates the remaining var's score
          for( Idx modality = 0; modality < __nodeVarMap[*nodeIter]->domainSize(); ++modality){
            nodeSet << __nodeSonsMap[*nodeIter][modality];

            for( auto varIter = remainingVarsScore.allValues().cbeginSafe(); varIter != remainingVarsScore.allValues().cendSafe(); ++varIter ) {
              if( __nodeId2Database[*nodeIter]->isPValueRelevant( varIter.key() ) ){
                double newPriority = remainingVarsScore.priority( varIter.key() )
                    + __score( varIter.key(), __nodeSonsMap[*nodeIter][modality] );
                remainingVarsScore.setPriority(varIter.key(), newPriority);
              }
            }
          }
        } else {
          nodeSet << *nodeIter;
        }
      }
    }


    // ============================================================================
    // Computes the Reduced and Ordered Decision Graph  associated to this ordered tree
    // ============================================================================
    template <TESTNAME AttributeSelection, bool isScalar >
    void IMDDI<AttributeSelection, isScalar>::toDG(){

       __target->clear();
       for( auto varIter = __varOrder.beginSafe(); varIter != __varOrder.endSafe(); ++varIter )
         __target->add(**varIter);
       __target->add(*__value);

       HashTable<NodeId, NodeId> toTarget;
       __mergeLeaves(toTarget, false);


//       for( auto nodeIter = __var2Node[__value]->cbeginSafe(); nodeIter != __var2Node[__value]->cendSafe(); ++nodeIter ){
//         GUM_SCALAR* probDist = __nodeId2Database[*nodeIter]->probDist();
//         NodeId* sonsMap = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(sizeof(NodeId)*__value->domainSize()) );
//         for(Idx modality = 0; modality < __value->domainSize(); ++modality )
//           sonsMap[modality] = __target->manager()->addTerminalNode( probDist[modality] );
//         toTarget.insert(*nodeIter, __target->manager()->nodeRedundancyCheck( __value, sonsMap ) );
//         MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( probDist, sizeof(GUM_SCALAR)*__value->domainSize());
//       }

       for( auto varIter = __varOrder.rbeginSafe(); varIter != __varOrder.rendSafe(); --varIter ) {

         for( auto nodeIter = __var2Node[*varIter]->cbeginSafe(); nodeIter != __var2Node[*varIter]->cendSafe(); ++nodeIter ){
           NodeId* sonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*(*varIter)->domainSize()) );
           for(Idx modality = 0; modality < (*varIter)->domainSize(); ++modality ){

             sonsMap[modality] = toTarget[__nodeSonsMap[*nodeIter][modality]];
           }
           toTarget.insert(*nodeIter, __target->manager()->nodeRedundancyCheck( *varIter, sonsMap ) );
         }

       }
       __target->manager()->setRootNode( toTarget[__root] );
       __target->manager()->clean();
    }


    // ============================================================================
    // Computes the Reduced and Ordered Decision Graph  associated to this ordered tree
    // ============================================================================
    template <TESTNAME AttributeSelection, bool isScalar >
    void IMDDI<AttributeSelection, isScalar>::toDG(const Bijection<const DiscreteVariable*, const DiscreteVariable*>& main2prime){
       __target->clear();

       for( auto varIter = __varOrder.beginSafe(); varIter != __varOrder.endSafe(); ++varIter )
         __target->add(*( main2prime.first(*varIter) ) );

       HashTable<NodeId, NodeId> toTarget;
       __mergeLeaves(toTarget, true);


//       for( auto nodeIter = __var2Node[__value]->cbeginSafe(); nodeIter != __var2Node[__value]->cendSafe(); ++nodeIter )
//         toTarget.insert(*nodeIter, __target->manager()->addTerminalNode(__nodeId2Database[*nodeIter]->rewardValue()));

       for( auto varIter = __varOrder.rbeginSafe(); varIter != __varOrder.rendSafe(); --varIter ) {
         const DiscreteVariable* mainy = main2prime.first(*varIter);
         for( auto nodeIter = __var2Node[*varIter]->cbeginSafe(); nodeIter != __var2Node[*varIter]->cendSafe(); ++nodeIter ){
           NodeId* sonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*mainy->domainSize()) );
           for(Idx modality = 0; modality < mainy->domainSize(); ++modality ){

             sonsMap[modality] = toTarget[__nodeSonsMap[*nodeIter][modality]];
           }
           toTarget.insert(*nodeIter, __target->manager()->nodeRedundancyCheck( mainy, sonsMap ) );
         }

       }
       __target->manager()->setRootNode( toTarget[__root] );

       __target->manager()->clean();
    }


    // ============================================================================
    // Performs the leaves merging
    // ============================================================================
    template <TESTNAME AttributeSelection, bool isScalar >
    void IMDDI<AttributeSelection, isScalar>::__mergeLeaves( HashTable<NodeId, NodeId>& toTarget, bool rewardLeaf ){

//      std::cout << "******************============================================*******************" << std::endl;
//      __showMap();

        MultiPriorityQueue<std::pair<NodeId,NodeId>, double, std::less<double>>* remainingPairs = new MultiPriorityQueue<std::pair<NodeId,NodeId>, double, std::less<double>>();
        HashTable<NodeId, double*> effectifTable;
        HashTable<NodeId, double> totalTable;

        HashTable<NodeId, NodeId> leafMap;

//        std::cout << "============================================" << std::endl << "Initialising heap : " << std::endl;

        for( auto lNodeIter = __var2Node[__value]->beginSafe(); lNodeIter != __var2Node[__value]->endSafe(); ++lNodeIter){
//            std::cout << "lNodeIter : " << *lNodeIter << std::endl;
            if( !effectifTable.exists(*lNodeIter) ){
                effectifTable.insert( *lNodeIter, __nodeId2Database[*lNodeIter]->effectif() );
                totalTable.insert( *lNodeIter, __nodeId2Database[*lNodeIter]->nbObservation() );
            }
            for( auto rNodeIter = lNodeIter; ++rNodeIter != __var2Node[__value]->endSafe();){
//                std::cout << "rNodeIter : " << *rNodeIter << std::endl;
                if( !effectifTable.exists(*rNodeIter) ){
                    effectifTable.insert( *rNodeIter, __nodeId2Database[*rNodeIter]->effectif() );
                    totalTable.insert( *rNodeIter, __nodeId2Database[*rNodeIter]->nbObservation() );
                }
                double d = __evalPair( effectifTable[*lNodeIter], totalTable[*lNodeIter], effectifTable[*rNodeIter], totalTable[*rNodeIter]);
                if( d > __pairSelectionThreshold )
                    remainingPairs->insert( std::pair<NodeId, NodeId>(*lNodeIter, *rNodeIter), d );
            }
//            std::cout << "RemainingPairs : " << remainingPairs->toString() << std::endl;
        }


//        std::cout << "============================================" << std::endl << "Seeking merging : " << std::endl;

        while(!remainingPairs->empty()){

            std::pair<NodeId,NodeId> p = remainingPairs->top();
            remainingPairs->eraseTop();

//            std::cout << "Pair recupered" << std::endl;

            NodeId nlid = __model.addNode();
            totalTable.insert( nlid, totalTable[p.first] + totalTable[p.second] );
            double* nls = static_cast<double*>( ALLOCATE( sizeof(double)*__value->domainSize() ) );
            for( Idx moda = 0; moda < __value->domainSize(); moda++ )
                nls[moda] = effectifTable[p.first][moda] + effectifTable[p.second][moda];
            effectifTable.insert(nlid, nls);

//            std::cout << "Recreating heap" << std::endl;

            MultiPriorityQueue<std::pair<NodeId,NodeId>, double, std::less<double>>* nextRemainingPairs = new MultiPriorityQueue<std::pair<NodeId,NodeId>, double, std::less<double>>();
            while( !remainingPairs->empty() ){

                std::pair<NodeId,NodeId> op = remainingPairs->top();
                double od = remainingPairs->topPriority();
                remainingPairs->eraseTop();

                if( op.first == p.first || op.first == p.second ){
                    double d = __evalPair( effectifTable[nlid], totalTable[nlid], effectifTable[op.second], totalTable[op.second] );
                    if( d > __pairSelectionThreshold )
                        nextRemainingPairs->insert( std::pair<NodeId, NodeId>(nlid, op.second), d );
                } else {
                    if( op.second == p.first || op.second == p.second ){
                        double d = __evalPair( effectifTable[nlid], totalTable[nlid], effectifTable[op.first], totalTable[op.first] );
                        if( d > __pairSelectionThreshold )
                            nextRemainingPairs->insert( std::pair<NodeId, NodeId>(nlid, op.first), d );
                    } else {
                        nextRemainingPairs->insert( op, od );
                    }
                }
            }

//            std::cout << "Leafmap Maintenance" << std::endl;

            delete remainingPairs;
            remainingPairs = nextRemainingPairs;

            if(!leafMap.exists(p.first))
              leafMap.insert(p.first, nlid);
            else
              leafMap[p.first] = nlid;

            if(!leafMap.exists(p.second))
              leafMap.insert(p.second, nlid);
            else
              leafMap[p.second] = nlid;
        }

//        std::cout << "============================================" << std::endl << "Now merging leaves " << leafMap << std::endl;


        for( auto nodeIter = __var2Node[__value]->beginSafe(); nodeIter != __var2Node[__value]->endSafe(); ++nodeIter){

            NodeId ti = *nodeIter;
            while( leafMap.exists(ti) )
                ti = leafMap[ti];

            if(! toTarget.exists(ti) ){

                if(rewardLeaf){
                    double ret = 0.0;
                    for(Idx modality = 0; modality < __value->domainSize(); ++modality)
                      ret += effectifTable[ti][modality] / totalTable[ti] * std::stod(__value->label(modality));
                    toTarget.insert( ti, __target->manager()->addTerminalNode(  ret ) );
                }else{
//                    std::cout << "Leaf : " << *nodeIter << " - Target : " << ti << " - HashTab : " << toTarget << std::endl;
                    NodeId* tts = static_cast<NodeId*>( ALLOCATE( sizeof(NodeId)*__value->domainSize()) );
                    for( Idx moda = 0; moda < __value->domainSize(); moda++){
                        tts[moda] = __target->manager()->addTerminalNode( effectifTable[ti][moda] / totalTable[ti] );
//                        std::cout << "\tModa : " << moda << " - DD Leaf : " << tts[moda] << std::endl;
                      }
//                    std::cout << "Installing Node" << toTarget << std::endl;
                    toTarget.insert( ti, __target->manager()->nodeRedundancyCheck(__value, tts ) );
//                    std::cout << "Done" << toTarget << std::endl;
                }

            }

            if( *nodeIter != ti )
              toTarget.insert( *nodeIter, toTarget[ti] );
        }
//        std::cout << "Yoh" << std::endl;

        for(auto it = effectifTable.beginSafe(); it != effectifTable.endSafe(); ++it)
          DEALLOCATE( it.val(), sizeof(double)*__value->domainSize());
        delete remainingPairs;
    }


    // ============================================================================
    // Performs the leaves merging
    // ============================================================================
    template <TESTNAME AttributeSelection, bool isScalar >
    double IMDDI<AttributeSelection, isScalar>::__evalPair( double* feffectif, double ftotal, double* seffectif, double stotal ){

        double fg = 0.0;
        double sg = 0.0;
        double fScalingFactor = ftotal / (ftotal + stotal);
        double sScalingFactor = stotal / (ftotal + stotal);

        for( Idx moda = 0; moda < __value->domainSize(); moda++ ){
//            std::cout << "Moda : " << moda << "\t- F : " << feffectif[moda] << "\t- Fe : " << ( ( feffectif[moda] + seffectif[moda] ) * fScalingFactor ) << "\t- S : " << seffectif[moda] << "\t- Se : " << ( ( feffectif[moda] + seffectif[moda] ) * sScalingFactor ) << std::endl;
            if( feffectif[moda] != 0 || seffectif[moda] != 0 ){
                if( feffectif[moda] != 0 )
                    fg += feffectif[moda] * log( feffectif[moda] / ( ( feffectif[moda] + seffectif[moda] ) * fScalingFactor ) );
                if( seffectif[moda] != 0 )
                    sg += seffectif[moda] * log( seffectif[moda] / ( ( feffectif[moda] + seffectif[moda] ) * sScalingFactor ) );
            }
        }

//        std::cout << " Fg : " << fg << "\t- Sg " << sg << std::endl;
        double fp = ChiSquare::probaChi2( fg, __value->domainSize() - 1 );
        double sp = ChiSquare::probaChi2( sg, __value->domainSize() - 1 );
//        std::cout << " Fp : " << fp << "\t- Sp " << sp <<  "\t- ST " << __pairSelectionThreshold << std::endl;
        return fp > sp ? sp : fp;
    }
} // end gum namespace
