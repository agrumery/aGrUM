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

      _leafDatabase.insert( _root, new Set<const Observation*>() );

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

      for( auto nodeIter = _leafDatabase.beginSafe(); nodeIter != _leafDatabase.endSafe(); ++nodeIter )
        delete nodeIter.val();

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
        _updateNodeWithObservation(newObs, currentNodeId);

        // The we select the next to go throught
        currentNodeId = _nodeSonsMap[currentNodeId][newObs->modality( _nodeVarMap[currentNodeId] )];
      }

      // On final insertion into the leave we reach
      _updateNodeWithObservation(newObs, currentNodeId);
      _leafDatabase[currentNodeId]->insert(newObs);

    }



    // ============================================================================
    // If a new modality appears to exists for given variable,
    // call this method to turn every associated node to this variable into leaf.
    // Graph has then indeed to be revised
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    void IncrementalGraphLearner<AttributeSelection, isScalar>::updateVar( const DiscreteVariable* var ){
      Link<NodeId>* nodIter = _var2Node[var]->list();
      Link<NodeId>* nni = nullptr;
      while(nodIter){
        nni = nodIter->nextLink();
        _convertNode2Leaf(nodIter->element());
        nodIter = nni;
      }
    }



    // ============================================================================
    // Install one of the best given
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    void IncrementalGraphLearner<AttributeSelection, isScalar>::_updateNode( NodeId updatedNode,
                                                                             Set<const DiscreteVariable*>& varsOfInterest ){

      // If this node has no interesting variable, we turn it into a leaf
      if( varsOfInterest.empty() ) {
        _convertNode2Leaf( updatedNode );
        return;
      }

      // If this node has already one of the best variable intalled as test, we move on
      if( _nodeVarMap.exists(updatedNode) && varsOfInterest.exists( _nodeVarMap[updatedNode] ) )
        return;

      // In any other case we have to install variable as best test
      Idx randy = (Idx)( std::rand() / RAND_MAX )*varsOfInterest.size(), basc = 0;
      SetConstIteratorSafe<const DiscreteVariable*> varIter;
      for(varIter = varsOfInterest.cbeginSafe(), basc = 0; varIter != varsOfInterest.cendSafe() && basc < randy; ++varIter, basc++ );

      _transpose( updatedNode, *varIter);

    }

    // ============================================================================
    // Insert a new node with given associated database, var and maybe sons
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    NodeId IncrementalGraphLearner<AttributeSelection, isScalar>::_insertNode( NodeDatabase<AttributeSelection, isScalar>* nDB,
                                                                               const DiscreteVariable* boundVar,
                                                                               NodeId* sonsMap ){
      NodeId newNodeId = _model.addNode();
      _nodeVarMap.insert(newNodeId, boundVar);
      _nodeId2Database.insert(newNodeId, nDB);
      _var2Node[boundVar]->addLink(newNodeId);
      if( sonsMap )
        _nodeSonsMap.insert(newNodeId, sonsMap);

      if( boundVar == _value )
        _leafDatabase.insert( newNodeId, new Set< const Observation*>() );

      return newNodeId;
    }



    // ============================================================================
    // Changes var associated to a node
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    void IncrementalGraphLearner<AttributeSelection, isScalar>::_chgNodeBoundVar( NodeId currentNodeId,
                                                                                  const DiscreteVariable* desiredVar ){
      _var2Node[_nodeVarMap[currentNodeId]]->searchAndRemoveLink(currentNodeId);
      _var2Node[desiredVar]->addLink(currentNodeId);
      _nodeVarMap[currentNodeId] = desiredVar;

//      if( _leafDatabase.exists(currentNodeId) ){
//        delete _leafDatabase[currentNodeId];
//        _leafDatabase.erase( currentNodeId );
//      }
    }



    // ============================================================================
    // Remove node from graph
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    void IncrementalGraphLearner<AttributeSelection, isScalar>::_removeNode( NodeId currentNodeId ){
      // Retriat de l'id
      _model.eraseNode( currentNodeId );

      // Retrait du vecteur fils
      if( _nodeSonsMap.exists(currentNodeId)){
        DEALLOCATE( _nodeSonsMap[currentNodeId], sizeof(NodeId)*_nodeVarMap[currentNodeId]->domainSize() );
        _nodeSonsMap.erase(currentNodeId);
      }

      if( _leafDatabase.exists(currentNodeId) ){
        delete _leafDatabase[currentNodeId];
        _leafDatabase.erase( currentNodeId );
      }

      // Retrait de la variable
      _var2Node[_nodeVarMap[currentNodeId]]->searchAndRemoveLink(currentNodeId);
      _nodeVarMap.erase( currentNodeId );

      // Retrait du NodeDatabase
      delete _nodeId2Database[currentNodeId];
      _nodeId2Database.erase(currentNodeId);

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

//        Sequence<NodeDatabase<AttributeSelection, isScalar>*> sonsNodeDatabase = _nodeId2Database[currentNodeId]->splitOnVar(desiredVar);

        NodeId* sonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*desiredVar->domainSize()) );
        for( Idx modality = 0; modality < desiredVar->domainSize(); ++modality )
          sonsMap[modality] = _insertNode( new NodeDatabase<AttributeSelection, isScalar>(&_setOfVars, _value), _value );

        _nodeSonsMap.insert( currentNodeId, sonsMap);

        for( SetIteratorSafe<const Observation*> obsIter = _leafDatabase[currentNodeId]->beginSafe(); _leafDatabase[currentNodeId]->endSafe() != obsIter; ++obsIter )
          _nodeId2Database[sonsMap[(*obsIter)->modality(desiredVar)]]->addObservation(*obsIter);

        _chgNodeBoundVar(currentNodeId, desiredVar);
        return;
      }

      // *************************************************************************************
      // Remains the general case where currentNodeId is an internal node.

        // First we ensure that children node use desiredVar as variable
        for(Idx modality = 0; modality < _nodeVarMap[currentNodeId]->domainSize(); ++modality )
          _transpose( _nodeSonsMap[currentNodeId][modality], desiredVar );

//        Sequence<NodeDatabase<AttributeSelection, isScalar>*> sonsNodeDatabase = _nodeId2Database[currentNodeId]->splitOnVar(desiredVar);
        NodeId* sonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*desiredVar->domainSize()) );

        // Then we create the new mapping
        for(Idx desiredVarModality = 0; desiredVarModality < desiredVar->domainSize(); ++desiredVarModality){

          NodeId* grandSonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*_nodeVarMap[currentNodeId]->domainSize()) );
          for(Idx currentVarModality = 0; currentVarModality < _nodeVarMap[currentNodeId]->domainSize(); ++currentVarModality )
            grandSonsMap[currentVarModality] = _nodeSonsMap[_nodeSonsMap[currentNodeId][currentVarModality]][desiredVarModality];

          sonsMap[desiredVarModality] = _insertNode( new NodeDatabase<AttributeSelection, isScalar>(&_setOfVars, _value), _nodeVarMap[currentNodeId], grandSonsMap );
        }

        // Finally we clean the old remaining nodes
        for(Idx currentVarModality = 0; currentVarModality < _nodeVarMap[currentNodeId]->domainSize(); ++currentVarModality ){
          _removeNode( _nodeSonsMap[currentNodeId][currentVarModality] );
        }

        // We suppress the old sons map and remap to the new one
        DEALLOCATE( _nodeSonsMap[currentNodeId], sizeof(NodeId)*_nodeVarMap[currentNodeId]->domainSize() );
        _nodeSonsMap[currentNodeId] = sonsMap;

        _chgNodeBoundVar(currentNodeId, desiredVar);
    }



    // ============================================================================
    // Turn given node into a leaf, recurssively destroying its children
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    void IncrementalGraphLearner<AttributeSelection, isScalar>::_convertNode2Leaf( NodeId currentNodeId ){

      if(_nodeVarMap[currentNodeId] != _value ){

        _leafDatabase.insert(currentNodeId, new Set<const Observation*>() );

        // Resolving potential sons issue
        for(Idx modality = 0; modality < _nodeVarMap[currentNodeId]->domainSize(); ++modality) {
          NodeId sonId = _nodeSonsMap[currentNodeId][modality];
          _convertNode2Leaf(sonId);
          _leafDatabase[currentNodeId]->operator +(*(_leafDatabase[sonId]));
          _removeNode(sonId);
        }

        DEALLOCATE( _nodeSonsMap[currentNodeId], sizeof(NodeId)*_nodeVarMap[currentNodeId]->domainSize() );
        _nodeSonsMap.erase(currentNodeId);

        _chgNodeBoundVar(currentNodeId, _value);
      }
    }



    // ============================================================================
    // Display tree current structure in console
    // For debugging purposes only.
    // ============================================================================
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
          Link<NodeId>* moter = mojo->list();
          while(moter){
            std::cout << moter->element() << " | ";
            moter = moter->nextLink();
          }
          std::cout <<  std::endl;
        }

        std::cout << _target->toDot() << std::endl << "     " << std::endl;
    }
} // end gum namespace
