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
#include <agrum/FMDP/learning/datastructure/incrementalGraphLearner.h>
#include <agrum/FMDP/learning/core/chiSquare.h>
// =======================================================
#include <agrum/variables/discreteVariable.h>
// =======================================================

#define ALLOCATE(x) SmallObjectAllocator::instance().allocate(x)
#define DEALLOCATE(x,y) SmallObjectAllocator::instance().deallocate(x,y)

namespace gum {

  // ############################################################################
  // Constructors / Destructors
  // ############################################################################

    // ============================================================================
    // Default constructor.
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    IncrementalGraphLearner<AttributeSelection, isScalar>::IncrementalGraphLearner( MultiDimDecisionGraph<double>* target,
                                                                                    Set<const DiscreteVariable*> varList,
                                                                                    const DiscreteVariable* value ) : _target(target),
                                                                                                                      _setOfVars(varList),
                                                                                                                      _value(value)
    {
      GUM_CONSTRUCTOR(IncrementalGraphLearner);

      _model.addNode();
      _root = _model.addNode();
      _nodeId2Database.insert( _root, new NodeDatabase<AttributeSelection, isScalar>(&_setOfVars, _value));
      _nodeVarMap.insert( _root, _value );

      for( auto varIter = _setOfVars.cbeginSafe(); varIter != _setOfVars.cendSafe(); ++varIter )
          _var2Node.insert( *varIter, new LinkedList<NodeId>());

      _var2Node.insert( _value, new LinkedList<NodeId>());
      _var2Node[_value]->addLink(_root);

    }


    // ============================================================================
    // Default destructor.
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    IncrementalGraphLearner<AttributeSelection, isScalar>::~IncrementalGraphLearner(){

      for( auto nodeIter = _nodeId2Database.beginSafe(); nodeIter != _nodeId2Database.endSafe(); ++nodeIter )
        delete nodeIter.val();

      for( auto nodeIter = _nodeSonsMap.beginSafe(); nodeIter != _nodeSonsMap.endSafe(); ++nodeIter )
        DEALLOCATE( nodeIter.val(), sizeof(NodeId)*_nodeVarMap[nodeIter.key()]->domainSize());

      for( auto varIter = _var2Node.beginSafe(); varIter != _var2Node.endSafe(); ++varIter )
          delete varIter.val();

      clearValue();

      GUM_DESTRUCTOR(IncrementalGraphLearner);
    }



  // ############################################################################
  // Incrementals methods
  // ############################################################################

    // ============================================================================
    // Adds a new observation to the structure
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    void IncrementalGraphLearner<AttributeSelection, isScalar>::addObservation( const Observation *newObs ){

      // The we go across the tree
      NodeId currentNodeId = _root;

      while( _nodeSonsMap.exists(currentNodeId) ){

        // On each encountered node, we update the database
        _nodeId2Database[currentNodeId]->addObservation( newObs );

        // The we select the next to go throught
        currentNodeId = _nodeSonsMap[currentNodeId][newObs->modality( _nodeVarMap[currentNodeId] )];
      }

      // On final insertion into the leave we reach
      _nodeId2Database[currentNodeId]->addObservation( newObs );

    }




    template < TESTNAME AttributeSelection, bool isScalar >
    void IncrementalGraphLearner<AttributeSelection, isScalar>::updateVar( const DiscreteVariable* var ){
      Link<NodeId>* nodIter = _var2Node[var]->list();
      while(nodIter){
        _convertNode2Leaf(nodIter);
        nodIter = nodIter->nextLink();
      }
    }


    // ============================================================================
    // Bring the desiredVar to the given node
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    void IncrementalGraphLearner<AttributeSelection, isScalar>::_transpose( NodeId currentNodeId,
                                                                            const DiscreteVariable* desiredVar ){

      // **************************************************************************************
      // Si le noeud courant contient déjà la variable qu'on souhaite lui amener
      // Il n'y a rien à faire
      if( _nodeVarMap[currentNodeId] == desiredVar )
        return;

      // **************************************************************************************
      // Si le noeud courant est terminal,
      // Il faut artificiellement insérer un noeud liant à la variable
      if( _nodeVarMap[currentNodeId] == _value ){

        Sequence<NodeDatabase<AttributeSelection, isScalar>*> sonsNodeDatabase = _nodeId2Database[currentNodeId]->splitOnVar(desiredVar);
        NodeId* sonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*desiredVar->domainSize()) );

        for( Idx modality = 0; modality < desiredVar->domainSize(); ++modality ){
          NodeId newNodeId = _model.addNode();
          _nodeVarMap.insert(newNodeId, _value);
          _nodeId2Database.insert(newNodeId, sonsNodeDatabase.atPos(modality));
          _var2Node[_value]->addLink(newNodeId);
          sonsMap[modality] = newNodeId;
        }

        _var2Node[desiredVar]->addLink(currentNodeId);
        _var2Node[_value]->searchAndRemoveLink(currentNodeId);

        _nodeVarMap[currentNodeId] = desiredVar;
        _nodeSonsMap.insert( currentNodeId, sonsMap);

        return;
      }

      // *************************************************************************************
      // Remains the general case where currentNodeId is an internal node.

          // First we ensure that children node use desiredVar as variable
          for(Idx modality = 0; modality < _nodeVarMap[currentNodeId]->domainSize(); ++modality )
            _transpose( _nodeSonsMap[currentNodeId][modality], desiredVar );

          Sequence<NodeDatabase<AttributeSelection, isScalar>*> sonsNodeDatabase = _nodeId2Database[currentNodeId]->splitOnVar(desiredVar);
          NodeId* sonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*desiredVar->domainSize()) );

          // Then we create the new mapping
          for(Idx desiredVarModality = 0; desiredVarModality < desiredVar->domainSize(); ++desiredVarModality){
            NodeId* grandSonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*_nodeVarMap[currentNodeId]->domainSize()) );
            for(Idx currentVarModality = 0; currentVarModality < _nodeVarMap[currentNodeId]->domainSize(); ++currentVarModality )
              grandSonsMap[currentVarModality] = _nodeSonsMap[_nodeSonsMap[currentNodeId][currentVarModality]][desiredVarModality];

            NodeId newNodeId = _model.addNode();
            _nodeVarMap.insert(newNodeId, _nodeVarMap[currentNodeId]);
            _nodeSonsMap.insert(newNodeId, grandSonsMap);
            _var2Node[_nodeVarMap[currentNodeId]]->addLink(newNodeId);
            _nodeId2Database.insert(newNodeId, sonsNodeDatabase.atPos(desiredVarModality));
            sonsMap[desiredVarModality] = newNodeId;
          }

          // Finally we clean the old remaining nodes
          for(Idx currentVarModality = 0; currentVarModality < _nodeVarMap[currentNodeId]->domainSize(); ++currentVarModality ){
            NodeId sonId = _nodeSonsMap[currentNodeId][currentVarModality];

            // Retriat de l'id
            _model.eraseNode( sonId );

            // Retrait de la variable
            _var2Node[_nodeVarMap[sonId]]->searchAndRemoveLink(sonId);
            _nodeVarMap.erase( sonId );

            // Retrait du NodeDatabase
            delete _nodeId2Database[sonId];
            _nodeId2Database.erase(sonId);

            // Turn given node into a leaf, recurssively destroying its children
            // ============================================================================
            // Retrait du vecteur fils
            DEALLOCATE( _nodeSonsMap[sonId], sizeof(NodeId)*desiredVar->domainSize() );
            _nodeSonsMap.erase(sonId);
          }

          // We suppress the old sons map and remap to the new one
          DEALLOCATE( _nodeSonsMap[currentNodeId], sizeof(NodeId)*_nodeVarMap[currentNodeId]->domainSize() );
          _nodeSonsMap[currentNodeId] = sonsMap;

          _var2Node[_nodeVarMap[currentNodeId]]->searchAndRemoveLink(currentNodeId);
          _var2Node[desiredVar]->addLink(currentNodeId);

          _nodeVarMap[currentNodeId] = desiredVar;

    }



    // ============================================================================
    // Turn given node into a leaf, recurssively destroying its children
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    void IncrementalGraphLearner<AttributeSelection, isScalar>::_convertNode2Leaf( NodeId currentNodeId ){

      if(_nodeVarMap[currentNodeId] != _value ){

        //
        for(Idx modality = 0; modality < _nodeVarMap[currentNodeId]->domainSize(); ++modality) {
          NodeId sonId = _nodeSonsMap[currentNodeId][modality];

          _convertNode2Leaf(sonId);

          // Retriat de l'id
          _model.eraseNode( sonId );

          // Retrait du NodeDatabase
          delete _nodeId2Database[sonId];
          _nodeId2Database.erase(sonId);

          // Retrait de la variable
          _nodeVarMap.erase( sonId );

          // Retrait du fils de la liste des noeuds terminaux
          _var2Node[_value]->searchAndRemoveLink(sonId);
        }


        DEALLOCATE( _nodeSonsMap[currentNodeId], sizeof(NodeId)*_nodeVarMap[currentNodeId]->domainSize() );
        _nodeSonsMap.erase(currentNodeId);

        _var2Node[_nodeVarMap[currentNodeId]]->searchAndRemoveLink(currentNodeId);
        _var2Node[_value]->addLink(currentNodeId);

        _nodeVarMap[currentNodeId] = _value;
      }
    }


    // ============================================================================
    // Computes the Reduced and Ordered Decision Graph  associated to this ordered tree
    // ============================================================================
//    template < TESTNAME AttributeSelection, bool isScalar >
//    void IncrementalGraphLearner<AttributeSelection, isScalar>::toDG(){

//       _target->clear();
//       for( auto varIter = _varOrder.beginSafe(); varIter != _varOrder.endSafe(); ++varIter )
//         _target->add(**varIter);
//       _target->add(*_value);

//       HashTable<NodeId, NodeId> toTarget;
//       _mergeLeaves(toTarget, false);


////       for( auto nodeIter = _var2Node[_value]->cbeginSafe(); nodeIter != _var2Node[_value]->cendSafe(); ++nodeIter ){
////         GUM_SCALAR* probDist = _nodeId2Database[*nodeIter]->probDist();
////         NodeId* sonsMap = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(sizeof(NodeId)*_value->domainSize()) );
////         for(Idx modality = 0; modality < _value->domainSize(); ++modality )
////           sonsMap[modality] = _target->manager()->addTerminalNode( probDist[modality] );
////         toTarget.insert(*nodeIter, _target->manager()->nodeRedundancyCheck( _value, sonsMap ) );
////         MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( probDist, sizeof(GUM_SCALAR)*_value->domainSize());
////       }

//       for( auto varIter = _varOrder.rbeginSafe(); varIter != _varOrder.rendSafe(); --varIter ) {

//         for( auto nodeIter = _var2Node[*varIter]->cbeginSafe(); nodeIter != _var2Node[*varIter]->cendSafe(); ++nodeIter ){
//           NodeId* sonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*(*varIter)->domainSize()) );
//           for(Idx modality = 0; modality < (*varIter)->domainSize(); ++modality ){

//             sonsMap[modality] = toTarget[_nodeSonsMap[*nodeIter][modality]];
//           }
//           toTarget.insert(*nodeIter, _target->manager()->nodeRedundancyCheck( *varIter, sonsMap ) );
//         }

//       }
//       _target->manager()->setRootNode( toTarget[_root] );
//       _target->manager()->clean();
//    }


    template < TESTNAME AttributeSelection, bool isScalar >
    void IncrementalGraphLearner<AttributeSelection, isScalar>::_debugTree( ){

        for( auto varIter = _nodeVarMap.beginSafe(); varIter != _nodeVarMap.endSafe(); ++varIter )
            std::cout << varIter.key() << " - " << varIter.val()->name() << " | ";
        std::cout << std::endl;

        for( auto nodeIter = _nodeSonsMap.beginSafe(); nodeIter != _nodeSonsMap.endSafe(); ++nodeIter ) {
            std::cout << nodeIter.key() << " : ";
            for( Idx i = 0; i < _nodeVarMap[nodeIter.key()]->domainSize(); ++i )
              std::cout << i << " -> " << _nodeSonsMap[nodeIter.key()][i] << " . ";
            std::cout << " | " << std::endl;
        }


        for(auto varLiter = _var2Node.beginSafe(); varLiter != _var2Node.endSafe(); ++varLiter ){
         std::cout << "Var " << varLiter.key()->name();
         LinkedList<NodeId>* mojo = varLiter.val();
         std::cout << " - Liste : ";
         Link<NodeId> moter = mojo->list();
         while(moter){
             std::cout << moter.element() << " | ";
             moter = moter.nextLink();
         }
         std::cout <<  std::endl;
        }

        std::cout << _target->toDot() << std::endl << "     " << std::endl;
    }
} // end gum namespace
