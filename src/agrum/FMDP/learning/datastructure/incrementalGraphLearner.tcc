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
#include <queue>
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

      for( auto varIter = _setOfVars.cbeginSafe(); varIter != _setOfVars.cendSafe(); ++varIter )
          _var2Node.insert( *varIter, new LinkedList<NodeId>());
      _var2Node.insert( _value, new LinkedList<NodeId>());

      _model.addNode();
      this->_root = _insertLeafNode(new NodeDatabase<AttributeSelection, isScalar>(&_setOfVars, _value),_value,new Set<const Observation*>());

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

      __assumeValue(newObs);

      // The we go across the tree
      NodeId currentNodeId = _root;

      while( _nodeSonsMap.exists(currentNodeId) ){

        // On each encountered node, we update the database
        _updateNodeWithObservation(newObs, currentNodeId);

        // The we select the next to go throught
        currentNodeId = _nodeSonsMap[currentNodeId][__branchObs(newObs, _nodeVarMap[currentNodeId] )];
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
                                                                               const DiscreteVariable* boundVar ){
      NodeId newNodeId = _model.addNode();
      _nodeVarMap.insert(newNodeId, boundVar);
      _nodeId2Database.insert(newNodeId, nDB);
      _var2Node[boundVar]->addLink(newNodeId);

      _needUpdate = true;

      return newNodeId;
    }

    template < TESTNAME AttributeSelection, bool isScalar >
    NodeId IncrementalGraphLearner<AttributeSelection, isScalar>::_insertInternalNode( NodeDatabase<AttributeSelection, isScalar>* nDB,
                                                                                       const DiscreteVariable* boundVar,
                                                                                       NodeId* sonsMap ){
      NodeId newNodeId = _insertNode( nDB, boundVar );
      _nodeSonsMap.insert(newNodeId, sonsMap);
      return newNodeId;
    }

    template < TESTNAME AttributeSelection, bool isScalar >
    NodeId IncrementalGraphLearner<AttributeSelection, isScalar>::_insertLeafNode( NodeDatabase<AttributeSelection, isScalar>* nDB,
                                    const DiscreteVariable* boundVar,
                                    Set<const Observation*>* obsSet ){
      NodeId newNodeId = _insertNode( nDB, boundVar );
      _leafDatabase.insert(newNodeId, obsSet);
      return newNodeId;
    }



    // ============================================================================
    // Changes var associated to a node
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    void IncrementalGraphLearner<AttributeSelection, isScalar>::_chgNodeBoundVar( NodeId currentNodeId,
                                                                                  const DiscreteVariable* desiredVar ){

      if( _nodeVarMap[currentNodeId] == desiredVar )
        return;

      _var2Node[_nodeVarMap[currentNodeId]]->searchAndRemoveLink(currentNodeId);
      _var2Node[desiredVar]->addLink(currentNodeId);
      _nodeVarMap[currentNodeId] = desiredVar;

      if( _nodeVarMap[currentNodeId] != _value && _leafDatabase.exists(currentNodeId) ){
        delete _leafDatabase[currentNodeId];
        _leafDatabase.erase( currentNodeId );
      }

      if( _nodeVarMap[currentNodeId] == _value && !_leafDatabase.exists(currentNodeId) ){
        _leafDatabase.insert( currentNodeId, new Set<const Observation*>() );
      }

      _needUpdate = true;
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

      _needUpdate = true;

    }



    // ============================================================================
    // Bring the desiredVar to the given node
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    void IncrementalGraphLearner<AttributeSelection, isScalar>::_transpose( NodeId currentNodeId,
                                                                            const DiscreteVariable* desiredVar ){
//      std::cout << "IncrementalGraphLearner::_transpose begin " << currentNodeId << " - " << desiredVar->name() << std::endl;

      // **************************************************************************************
      // Si le noeud courant contient déjà la variable qu'on souhaite lui amener
      // Il n'y a rien à faire
      if( _nodeVarMap[currentNodeId] == desiredVar ){
//          std::cout << " IncrementalGraphLearner::_convertNode2Leaf targeted var end " << std::endl;
        return;
        }

      // **************************************************************************************
      // Si le noeud courant est terminal,
      // Il faut artificiellement insérer un noeud liant à la variable
      if( _nodeVarMap[currentNodeId] == _value ){

        // We turned this leaf into an internal node.
        // This mean that we'll need to install children leaves for each value of desiredVar

        // First We must prepare these new leaves NodeDatabases and Sets<const Observation*>
        NodeDatabase<AttributeSelection, isScalar>** dbMap =
            static_cast<NodeDatabase<AttributeSelection, isScalar>**>(
              ALLOCATE(sizeof(NodeDatabase<AttributeSelection, isScalar>*)*desiredVar->domainSize()) );
        Set<const Observation*>** obsetMap =
            static_cast<Set<const Observation*>**>(
              ALLOCATE(sizeof(Set<const Observation*>*)*desiredVar->domainSize()) );
        for( Idx modality = 0; modality < desiredVar->domainSize(); ++modality ){
          dbMap[modality] = new NodeDatabase<AttributeSelection, isScalar>(&_setOfVars, _value);
          obsetMap[modality] = new Set<const Observation*>();
        }
        for( SetIteratorSafe<const Observation*> obsIter = _leafDatabase[currentNodeId]->beginSafe(); _leafDatabase[currentNodeId]->endSafe() != obsIter; ++obsIter ){
          dbMap[__branchObs(*obsIter, desiredVar)]->addObservation(*obsIter);
          obsetMap[__branchObs(*obsIter, desiredVar)]->insert(*obsIter);
        }

        // Then we can install each new leaves (and put in place the sonsMap)
        NodeId* sonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*desiredVar->domainSize()) );
        for( Idx modality = 0; modality < desiredVar->domainSize(); ++modality )
          sonsMap[modality] = _insertLeafNode( dbMap[modality], _value, obsetMap[modality] );

        // Some necessary clean up
        DEALLOCATE(dbMap, sizeof(NodeDatabase<AttributeSelection, isScalar>*)*desiredVar->domainSize());
        DEALLOCATE(obsetMap, sizeof(Set<const Observation*>*)*desiredVar->domainSize());

        // And finally we can turn the node into an internal node associated to desiredVar
        _chgNodeBoundVar(currentNodeId, desiredVar);
        _nodeSonsMap.insert( currentNodeId, sonsMap);

//        std::cout << " IncrementalGraphLearner::_convertNode2Leaf leaf end" << std::endl;

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
          NodeDatabase<AttributeSelection, isScalar>* sonDB = new NodeDatabase<AttributeSelection, isScalar>(&_setOfVars, _value);
          for(Idx currentVarModality = 0; currentVarModality < _nodeVarMap[currentNodeId]->domainSize(); ++currentVarModality ){
            grandSonsMap[currentVarModality] = _nodeSonsMap[_nodeSonsMap[currentNodeId][currentVarModality]][desiredVarModality];
            sonDB->operator +=( (*_nodeId2Database[grandSonsMap[currentVarModality]]));
          }

          sonsMap[desiredVarModality] = _insertInternalNode( sonDB, _nodeVarMap[currentNodeId], grandSonsMap );
        }

        // Finally we clean the old remaining nodes
        for(Idx currentVarModality = 0; currentVarModality < _nodeVarMap[currentNodeId]->domainSize(); ++currentVarModality ){
          _removeNode( _nodeSonsMap[currentNodeId][currentVarModality] );
        }

        // We suppress the old sons map and remap to the new one
        DEALLOCATE( _nodeSonsMap[currentNodeId], sizeof(NodeId)*_nodeVarMap[currentNodeId]->domainSize() );
        _nodeSonsMap[currentNodeId] = sonsMap;

        _chgNodeBoundVar(currentNodeId, desiredVar);
//        std::cout << " IncrementalGraphLearner::_convertNode2Leaf end " << std::endl;
    }



    // ============================================================================
    // Turn given node into a leaf, recurssively destroying its children
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    void IncrementalGraphLearner<AttributeSelection, isScalar>::_convertNode2Leaf( NodeId currentNodeId ){

//      std::cout << " IncrementalGraphLearner::_convertNode2Leaf begin " << currentNodeId << std::endl;

      if(_nodeVarMap[currentNodeId] != _value ){

        _leafDatabase.insert(currentNodeId, new Set<const Observation*>() );

        // Resolving potential sons issue
        for(Idx modality = 0; modality < _nodeVarMap[currentNodeId]->domainSize(); ++modality) {
          NodeId sonId = _nodeSonsMap[currentNodeId][modality];
          _convertNode2Leaf(sonId);
          (*_leafDatabase[currentNodeId]) = (*_leafDatabase[currentNodeId]) + *(_leafDatabase[sonId]);
          _removeNode(sonId);
        }

        DEALLOCATE( _nodeSonsMap[currentNodeId], sizeof(NodeId)*_nodeVarMap[currentNodeId]->domainSize() );
        _nodeSonsMap.erase(currentNodeId);

        _chgNodeBoundVar(currentNodeId, _value);
      }
//      std::cout << " IncrementalGraphLearner::_convertNode2Leaf end" << std::endl;
    }


    template < TESTNAME AttributeSelection, bool isScalar >
    Set<const Observation*> IncrementalGraphLearner<AttributeSelection, isScalar>::_getNodeDataBase( NodeId nody ){

      Set<const Observation*> ret;
      std::vector<NodeId> filo;
      filo.push_back( nody );

      while( !filo.empty() ){
        NodeId curNody = filo.back();
        filo.pop_back();
        if( this->_nodeVarMap[curNody] == _value ){
          ret = ret + *(this->_leafDatabase[curNody]);
        } else {
          for( Idx moda = 0; moda < this->_nodeVarMap[curNody]->domainSize(); ++moda )
            filo.push_back( this->_nodeSonsMap[curNody][moda] );
        }
      }
      return ret;
    }

    template < TESTNAME AttributeSelection, bool isScalar >
    MultiDimDecisionGraph<double> *IncrementalGraphLearner<AttributeSelection, isScalar>::extractCount(){
      MultiDimDecisionGraph<double>* ret = new MultiDimDecisionGraph<double>();
      _insertSetOfVars(ret);

      ret->manager()->setRootNode(__extractCount(this->_root, ret));

      return ret;
    }

    template < TESTNAME AttributeSelection, bool isScalar >
    NodeId  IncrementalGraphLearner<AttributeSelection, isScalar>::__extractCount(NodeId currentNodeId,
                                                                                  MultiDimDecisionGraph<double>* ret){


      if( this->_nodeVarMap[currentNodeId] == this->_value ){
         NodeId retn = ret->manager()->addTerminalNode( (double) this->_nodeId2Database[currentNodeId]->nbObservation());
        return retn;
      }

      NodeId* sonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*this->_nodeVarMap[currentNodeId]->domainSize()) );
      for( Idx moda = 0; moda < this->_nodeVarMap[currentNodeId]->domainSize(); ++moda )
        sonsMap[moda] = __extractCount(this->_nodeSonsMap[currentNodeId][moda], ret);

      NodeId retn =  ret->manager()->checkIsomorphism(this->_nodeVarMap[currentNodeId], sonsMap);
      return retn;

    }

    // ============================================================================
    // Display tree current structure in console
    // For debugging purposes only.
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    void IncrementalGraphLearner<AttributeSelection, isScalar>::_debugTree( ){

      //std::stringstream output;
      int cpt = 0;

      std::queue<NodeId> fifo;
      fifo.push(this->_root);
//      std::cout << this->_nodeId2Database.size() << " elements to visit." << std::endl;

      while(!fifo.empty()){

        NodeId currentNodeId = fifo.front();
        fifo.pop();

        if(!this->_nodeId2Database.exists(currentNodeId))
          continue;

//         std::cout << "##########" << std::endl;
//         std::cout << "Noeud : " << currentNodeId;
//         std::cout << " - Associated Var : " << this->_nodeVarMap[currentNodeId]->name() << " -> Dom:" << this->_nodeVarMap[currentNodeId]->domainSize() << std::endl;
         //std::cout << "\tDATA : " << this->_nodeId2Database[currentNodeId]->toString();


       if( this->_nodeSonsMap.exists(currentNodeId) ){
          for( Idx i = 0; i < this->_nodeVarMap[currentNodeId]->domainSize(); ++i ){
//            std::cout << "Pushing "  << this->_nodeSonsMap[currentNodeId][i] << std::endl;
            fifo.push(this->_nodeSonsMap[currentNodeId][i]);
          }
        }
        cpt++;
        if( cpt == 100 ){
//          std::cout << "CPT > 100 BREAKING"<<std::endl;
          exit(1);
          }

//        Set<const Observation*> ret = this->_getNodeDataBase(currentNodeId);
//        for(SetIteratorSafe<const Observation*> obsIter = ret.beginSafe(); obsIter != ret.endSafe(); ++obsIter)
//          output << "\t\t\t" << (*obsIter)->toString() << std::endl;
      }

//      std::cout << output.str() << std::endl;
    }
} // end gum namespace
