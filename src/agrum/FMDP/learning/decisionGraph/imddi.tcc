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
* @file
* @brief
*
* @author Jean-Christophe Magnan
*/
// =======================================================
#include <cmath>
// =======================================================
#include <agrum/core/types.h>
#include <agrum/core/multiPriorityQueue.h>
// =======================================================
#include <agrum/FMDP/learning/decisionGraph/imddi.h>
#include <agrum/FMDP/learning/decisionGraph/chiSquare.h>
// =======================================================
#include <agrum/variables/discreteVariable.h>
// =======================================================

namespace gum {

  // ############################################################################
  // Constructors / Destructors
  // ############################################################################

    // ============================================================================
    // Default constructor.
    // ============================================================================
    template< typename GUM_SCALAR>
    IMDDI<GUM_SCALAR>::IMDDI( MultiDimDecisionGraph<GUM_SCALAR>* target,
                              double dependenceThreshold,
                              Set<const DiscreteVariable*> varList,
                              const DiscreteVariable* value,
                              bool rewardMode ) : __target(target),
                                                  __setOfVars(varList),
                                                  __value(value),
                                                  __nbTotalObservation(0),
                                                  __dependenceThreshold(dependenceThreshold),
                                                  __isReward(rewardMode)
    {
      GUM_CONSTRUCTOR(IMDDI);

      __model.insertNode();
      __root = __model.insertNode();
      __nodeId2Database.insert( __root, new NodeDatabase<GUM_SCALAR>(&__setOfVars, __value));
      __nodeVarMap.insert( __root, __value );

      for( auto varIter = __setOfVars.cbeginSafe(); varIter != __setOfVars.cendSafe(); ++varIter )
          __var2Node.insert( *varIter, new List<NodeId>());

      __var2Node.insert( __value, new List<NodeId>());      
      __var2Node[__value]->insert(__root);

    }


    // ============================================================================
    // Default destructor.
    // ============================================================================
    template <typename GUM_SCALAR>
    IMDDI<GUM_SCALAR>::~IMDDI(){

      for( auto nodeIter = __nodeId2Database.beginSafe(); nodeIter != __nodeId2Database.endSafe(); ++nodeIter )
        delete nodeIter.val();

      for( auto nodeIter = __nodeSonsMap.beginSafe(); nodeIter != __nodeSonsMap.endSafe(); ++nodeIter )
        MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( nodeIter.val(), sizeof(NodeId)*__nodeVarMap[nodeIter.key()]->domainSize());

      for( auto varIter = __var2Node.beginSafe(); varIter != __var2Node.endSafe(); ++varIter )
          delete varIter.val();

      GUM_DESTRUCTOR(IMDDI);
    }



  // ############################################################################
  // Incrementals methods
  // ############################################################################

    // ============================================================================
    // Adds a new observation to the structure
    // ============================================================================
    template <typename GUM_SCALAR>
    void IMDDI<GUM_SCALAR>::addObservation(const Observation *newObs ){

      // First we increase the total number of observation added
      __nbTotalObservation++;

      // The we go across the tree
      NodeId currentNodeId = __root;

      while( __nodeSonsMap.exists(currentNodeId) ){

        // On each encountered node, we update the database
        __nodeId2Database[currentNodeId]->addObservation( newObs );

        // The we select the next to go throught
        currentNodeId = __nodeSonsMap[currentNodeId][newObs->modality( __nodeVarMap[currentNodeId] )];
      }

      // On final insertion into the leave we reach
      __nodeId2Database[currentNodeId]->addObservation( newObs );

    }


    // ============================================================================
    // Updates the tree after a new observation has been added
    // ============================================================================
    template <typename GUM_SCALAR>
    void IMDDI<GUM_SCALAR>::updateOrderedTree(){

      __varOrder.clear();

      // First xe initialize the node set which will give us the scores
      Set<NodeId> currentNodeSet;
      currentNodeSet.insert( __root );

      // Then we initialize the pool of variables to consider
      Set<const DiscreteVariable*> remainingVars(__setOfVars);

      MultiPriorityQueue<const DiscreteVariable*, double, std::greater<double>> remainingVarsScore;
      for( auto varIter = remainingVars.beginSafe(); varIter != remainingVars.endSafe(); ++varIter ) {
        if( __nodeId2Database[__root]->isPValueRelevant(*varIter) )
            remainingVarsScore.insert(*varIter, __score(*varIter, __root));
        else
            remainingVarsScore.insert(*varIter, 0.0);

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
        __turnIntoLeaf(*nodeIter);

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
    template <typename GUM_SCALAR>
    INLINE
    double IMDDI<GUM_SCALAR>::__score( const DiscreteVariable* var, NodeId nody){

      double weight = (double)__nodeId2Database[nody]->nbObservation() / (double) this->__nbTotalObservation;
      return weight*__nodeId2Database[nody]->pValue( var );
    }


    // ============================================================================
    // For each node in the given set, this methods checks whether or not
    // we should installed the given variable as a test.
    // If so, the node is updated
    // ============================================================================
    template <typename GUM_SCALAR>
    void IMDDI<GUM_SCALAR>::__updateNodeSet( Set<NodeId>& nodeSet,
                         const DiscreteVariable* selectedVar,
                         MultiPriorityQueue<const DiscreteVariable*, double, std::greater<double>>& remainingVarsScore ){

      Set<NodeId> oldNodeSet(nodeSet);
      nodeSet.clear();
      for( SetIteratorSafe<NodeId> nodeIter = oldNodeSet.beginSafe(); nodeIter != oldNodeSet.endSafe(); ++nodeIter ){

        if( __nodeId2Database[*nodeIter]->isPValueRelevant( selectedVar)
                && __nodeId2Database[*nodeIter]->pValue( selectedVar) > __dependenceThreshold ){

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
    // Bring the desiredVar to the given node
    // ============================================================================
    template <typename GUM_SCALAR>
    void IMDDI<GUM_SCALAR>::__transpose( NodeId currentNodeId,
                                         const DiscreteVariable* desiredVar ){

      // **************************************************************************************
      // Si le noeud courant contient déjà la variable qu'on souhaite lui amener
      // Il n'y a rien à faire
      if( __nodeVarMap[currentNodeId] == desiredVar )
        return;

      // **************************************************************************************
      // Si le noeud courant est terminal,
      // Il faut artificiellement insérer un noeud liant à la variable
      if( __nodeVarMap[currentNodeId] == __value ){

        Sequence<NodeDatabase<GUM_SCALAR>*> sonsNodeDatabase = __nodeId2Database[currentNodeId]->splitOnVar(desiredVar);
        NodeId* sonsMap = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(sizeof(NodeId)*desiredVar->domainSize()) );

        for( Idx modality = 0; modality < desiredVar->domainSize(); ++modality ){
          NodeId newNodeId = __model.insertNode();
          __nodeVarMap.insert(newNodeId, __value);
          __nodeId2Database.insert(newNodeId, sonsNodeDatabase.atPos(modality));
          __var2Node[__value]->insert(newNodeId);
          sonsMap[modality] = newNodeId;
        }

        __var2Node[desiredVar]->insert(currentNodeId);
        __var2Node[__value]->eraseByVal(currentNodeId);

        __nodeVarMap[currentNodeId] = desiredVar;
        __nodeSonsMap.insert( currentNodeId, sonsMap);

        return;
      }

      // *************************************************************************************
      // Remains the general case where currentNodeId is an internal node.

          // First we ensure that children node use desiredVar as variable
          for(Idx modality = 0; modality < __nodeVarMap[currentNodeId]->domainSize(); ++modality )
            __transpose( __nodeSonsMap[currentNodeId][modality], desiredVar );

          Sequence<NodeDatabase<GUM_SCALAR>*> sonsNodeDatabase = __nodeId2Database[currentNodeId]->splitOnVar(desiredVar);
          NodeId* sonsMap = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(sizeof(NodeId)*desiredVar->domainSize()) );

          // Then we create the new mapping
          for(Idx desiredVarModality = 0; desiredVarModality < desiredVar->domainSize(); ++desiredVarModality){
            NodeId* grandSonsMap = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(sizeof(NodeId)*__nodeVarMap[currentNodeId]->domainSize()) );
            for(Idx currentVarModality = 0; currentVarModality < __nodeVarMap[currentNodeId]->domainSize(); ++currentVarModality )
              grandSonsMap[currentVarModality] = __nodeSonsMap[__nodeSonsMap[currentNodeId][currentVarModality]][desiredVarModality];

            NodeId newNodeId = __model.insertNode();
            __nodeVarMap.insert(newNodeId, __nodeVarMap[currentNodeId]);
            __nodeSonsMap.insert(newNodeId, grandSonsMap);
            __var2Node[__nodeVarMap[currentNodeId]]->insert(newNodeId);
            __nodeId2Database.insert(newNodeId, sonsNodeDatabase.atPos(desiredVarModality));
            sonsMap[desiredVarModality] = newNodeId;
          }

          // Finally we clean the old remaining nodes
          for(Idx currentVarModality = 0; currentVarModality < __nodeVarMap[currentNodeId]->domainSize(); ++currentVarModality ){
            NodeId sonId = __nodeSonsMap[currentNodeId][currentVarModality];

            // Retriat de l'id
            __model.eraseNode( sonId );

            // Retrait de la variable
            __var2Node[__nodeVarMap[sonId]]->eraseByVal(sonId);
            __nodeVarMap.erase( sonId );

            // Retrait du NodeDatabase
            delete __nodeId2Database[sonId];
            __nodeId2Database.erase(sonId);

            // Retrait du vecteur fils
            MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( __nodeSonsMap[sonId], sizeof(NodeId)*desiredVar->domainSize() );
            __nodeSonsMap.erase(sonId);
          }

          // We suppress the old sons map and remap to the new one
          MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( __nodeSonsMap[currentNodeId], sizeof(NodeId)*__nodeVarMap[currentNodeId]->domainSize() );
          __nodeSonsMap[currentNodeId] = sonsMap;

          __var2Node[__nodeVarMap[currentNodeId]]->eraseByVal(currentNodeId);
          __var2Node[desiredVar]->insert(currentNodeId);

          __nodeVarMap[currentNodeId] = desiredVar;

    }



    // ============================================================================
    // Turn given node into a leaf, recurssively destroying its children
    // ============================================================================
    template <typename GUM_SCALAR>
    void IMDDI<GUM_SCALAR>::__turnIntoLeaf(NodeId currentNodeId){

      if(__nodeVarMap[currentNodeId] != __value ){

        //
        for(Idx modality = 0; modality < __nodeVarMap[currentNodeId]->domainSize(); ++modality) {
          NodeId sonId = __nodeSonsMap[currentNodeId][modality];

          __turnIntoLeaf(sonId);

          // Retriat de l'id
          __model.eraseNode( sonId );

          // Retrait du NodeDatabase
          delete __nodeId2Database[sonId];
          __nodeId2Database.erase(sonId);

          // Retrait de la variable
          __nodeVarMap.erase( sonId );

          // Retrait du fils de la liste des noeuds terminaux
          __var2Node[__value]->eraseByVal(sonId);
        }


        MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( __nodeSonsMap[currentNodeId], sizeof(NodeId)*__nodeVarMap[currentNodeId]->domainSize() );
        __nodeSonsMap.erase(currentNodeId);

        __var2Node[__nodeVarMap[currentNodeId]]->eraseByVal(currentNodeId);
        __var2Node[__value]->insert(currentNodeId);

        __nodeVarMap[currentNodeId] = __value;
      }
    }


    // ============================================================================
    // Computes the Reduced and Ordered Decision Graph  associated to this ordered tree
    // ============================================================================
    template <typename GUM_SCALAR>
    void IMDDI<GUM_SCALAR>::toDG(){

       __target->clear();
       for( auto varIter = __varOrder.beginSafe(); varIter != __varOrder.endSafe(); ++varIter )
         __target->add(**varIter);
       __target->add(*__value);

       HashTable<NodeId, NodeId> toTarget;


       for( auto nodeIter = __var2Node[__value]->cbeginSafe(); nodeIter != __var2Node[__value]->cendSafe(); ++nodeIter ){
         GUM_SCALAR* probDist = __nodeId2Database[*nodeIter]->probDist();
         NodeId* sonsMap = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(sizeof(NodeId)*__value->domainSize()) );
         for(Idx modality = 0; modality < __value->domainSize(); ++modality )
           sonsMap[modality] = __target->manager()->addTerminalNode( probDist[modality] );
         toTarget.insert(*nodeIter, __target->manager()->nodeRedundancyCheck( __value, sonsMap ) );
         MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( probDist, sizeof(GUM_SCALAR)*__value->domainSize());
       }

       for( auto varIter = __varOrder.rbeginSafe(); varIter != __varOrder.rendSafe(); --varIter ) {

         for( auto nodeIter = __var2Node[*varIter]->cbeginSafe(); nodeIter != __var2Node[*varIter]->cendSafe(); ++nodeIter ){
           NodeId* sonsMap = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(sizeof(NodeId)*(*varIter)->domainSize()) );
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
    template <typename GUM_SCALAR>
    void IMDDI<GUM_SCALAR>::toDG(const Bijection<const DiscreteVariable*, const DiscreteVariable*>& main2prime){
       __target->clear();

       for( auto varIter = __varOrder.beginSafe(); varIter != __varOrder.endSafe(); ++varIter )
         __target->add(*( main2prime.first(*varIter) ) );

       HashTable<NodeId, NodeId> toTarget;


       for( auto nodeIter = __var2Node[__value]->cbeginSafe(); nodeIter != __var2Node[__value]->cendSafe(); ++nodeIter )
         toTarget.insert(*nodeIter, __target->manager()->addTerminalNode(__nodeId2Database[*nodeIter]->rewardValue()));

       for( auto varIter = __varOrder.rbeginSafe(); varIter != __varOrder.rendSafe(); --varIter ) {
         const DiscreteVariable* mainy = main2prime.first(*varIter);
         for( auto nodeIter = __var2Node[*varIter]->cbeginSafe(); nodeIter != __var2Node[*varIter]->cendSafe(); ++nodeIter ){
           NodeId* sonsMap = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(sizeof(NodeId)*mainy->domainSize()) );
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
    template <typename GUM_SCALAR>
    void IMDDI<GUM_SCALAR>::__mergeLeaves( HashTable<NodeId, NodeId>& toTarget ){

        MultiPriorityQueue<std::pair<NodeId,NodeId>, double, std::less<double>>* remainingPairs = new MultiPriorityQueue<std::pair<NodeId,NodeId>, double, std::less<double>>();
        HashTable<NodeId, double*> effectifTable;
        HashTable<NodeId, double> totalTable;

        HashTable<NodeId, NodeId> leafMap;

        for( auto lNodeIter = __var2Node[__value]->beginSafe(); lNodeIter != __var2Node[__value]->endSafe(); ++lNodeIter){
            effectifTable.insert( *lNodeIter, __nodeId2Database[*lNodeIter]->effectif() );
            totalTable.insert( *lNodeIter, __nodeId2Database[*lNodeIter]->nbObservation() );
            for( auto rNodeIter = lNodeIter; ++rNodeIter != __var2Node[__value]->endSafe();){
                effectifTable.insert( *rNodeIter, __nodeId2Database[*rNodeIter]->effectif() );
                totalTable.insert( *rNodeIter, __nodeId2Database[*rNodeIter]->nbObservation() );
                double d = __evalPair( effectifTable[*lNodeIter], totalTable[*lNodeIter], effectifTable[*rNodeIter], totalTable[*rNodeIter]);
                if( d < __similarityTreshold )
                    remainingPairs->insert( std::pair<NodeId, NodeId>(*lNodeIter, *rNodeIter), d );
            }
        }

        while(!remainingPairs->empty()){

            std::pair<NodeId,NodeId> p = remainingPairs->top();
            remainingPairs->eraseTop();

            NodeId nlid = __model.insertNode();
            totalTable.insert( nlid, totalTable[p.first] + totalTable[p.second] );
            double* nls = MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate( sizeof(GUM_SCALAR)*__value->domainSize() );
            for( Idx moda = 0; moda < __value->domainSize(); moda++ )
                nls[moda] = effectifTable[p.first][moda] + effectifTable[p.second][moda];
            effectifTable.insert(nlid, nls);

            MultiPriorityQueue<std::pair<NodeId,NodeId>, double, std::less<double>>* nextRemainingPairs = new MultiPriorityQueue<std::pair<NodeId,NodeId>, double, std::less<double>>();
            while( !remainingPairs->empty() ){

                std::pair<NodeId,NodeId> op = remainingPairs->top();
                double od = remainingPairs->topPriority();
                remainingPairs->eraseTop();

                if( op.first == p.first || op.first == p.second ){
                    double d = __evalPair( effectifTable[nlid], totalTable[nlid], effectifTable[op.second], totalTable[op.second] );
                    if( d < __similarityTreshold )
                        nextRemainingPairs->insert( std::pair<NodeId, NodeId>(nlid, op.second),  );
                } else {
                    if( op.second == p.first || op.second == p.second ){
                        double d = __evalPair( effectifTable[nlid], totalTable[nlid], effectifTable[op.first], totalTable[op.first] );
                        if( d < __similarityTreshold )
                            nextRemainingPairs->insert( std::pair<NodeId, NodeId>(nlid, op.first),  );
                    } else {
                        nextRemainingPairs->insert( op, od );
                    }
                }
            }
            delete remainingPairs;
            remainingPairs = nextRemainingPairs;

            leafMap.insert(p.first, nlid);
            leafMap.insert(p.first, nlid);
        }


        for( auto nodeIter = __var2Node[__value]->beginSafe(); nodeIter != __var2Node[__value]->endSafe(); ++nodeIter){

            NodeId ti = *nodeIter;
            while( leafMap.exists(ti) )
                ti = leafMap[ti];

            if(!__target->)
        }
    }


    // ============================================================================
    // Performs the leaves merging
    // ============================================================================
    template <typename GUM_SCALAR>
    double IMDDI<GUM_SCALAR>::__evalPair( double* feffectif, double ftotal, double* seffectif, double stotal ){

        double fg = 0.0;
        double sg = 0.0;
        double fScalingFactor = ftotal / (ftotal + stotal);
        double sScalingFactor = stotal / (ftotal + stotal);

        for( Idx moda = 0; moda < __value->domainSize(); moda++ ){
            if( feffectif[moda] != 0 || seffectif[moda] != 0 ){
                if( feffectif[moda] != 0 )
                    fg += feffectif[moda] * log( feffectif[moda] / ( ( feffectif[moda] + seffectif[moda] ) * fScalingFactor ) );
                if( seffectif[moda] != 0 )
                    sg += seffectif[moda] * log( seffectif[moda] / ( ( feffectif[moda] + seffectif[moda] ) * sScalingFactor ) );
            }
        }

        double fp = ChiSquare::probaChi2( fg, __value->domainSize() - 1 );
        double sp = ChiSquare::probaChi2( sg, __value->domainSize() - 1 );
        return fp > sp ? sp : fp;
    }


    template <typename GUM_SCALAR >
    void
    IMDDI<GUM_SCALAR>::__showMap( ){

        for( auto varIter = __nodeVarMap.beginSafe(); varIter != __nodeVarMap.endSafe(); ++varIter )
            std::cout << varIter.key() << " - " << varIter.val()->name() << " | ";
        std::cout << std::endl;

        for( auto nodeIter = __nodeSonsMap.beginSafe(); nodeIter != __nodeSonsMap.endSafe(); ++nodeIter ) {
            std::cout << nodeIter.key() << " : ";
            for( Idx i = 0; i < __nodeVarMap[nodeIter.key()]->domainSize(); ++i )
              std::cout << i << " -> " << __nodeSonsMap[nodeIter.key()][i] << " . ";
            std::cout << " | " << std::endl;
        }


        for(auto varLiter = __var2Node.beginSafe(); varLiter != __var2Node.endSafe(); ++varLiter ){
         std::cout << "Var " << varLiter.key()->name();
         List<NodeId>* mojo = varLiter.val();
         std::cout << " - Taille : " << mojo->size() << " - Liste : ";
         for( ListIteratorSafe<NodeId> nodeIter = mojo->beginSafe(); nodeIter != mojo->endSafe(); ++nodeIter)
             std::cout << *nodeIter << " | ";
         std::cout <<  std::endl;
        }

        std::cout << __target->toDot() << std::endl << "     " << std::endl;
    }
} // end gum namespace
