
/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Template methods of gum::MultiDimDecisionGraphManager.
 *
 * @author Jean-Christophe Magnan
 *
 */
// ============================================================================
#include <agrum/multidim/multiDimDecisionGraphManager.h>
// ============================================================================

namespace gum {

  // ############################################################################
  // @name Constructors / Destructors
  // ############################################################################

    // ============================================================================
    // Default constructor.
    // Private.
    // You have to call MultiDimDecisionGraph< GUM_SCALAR>::manager() to get the instance
    // of MultiDimDecisionGraphManager bound to your decision graph.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::MultiDimDecisionGraphManager( MultiDimDecisionGraph<GUM_SCALAR>* mddg ){
      GUM_CONSTRUCTOR ( MultiDimDecisionGraphManager ) ;
      __decisionGraph = mddg;
    }



    // ============================================================================
    // Destructor. Don't worry, it will be call on the destruction of your
    // MultiDimDecisionGraph.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::~MultiDimDecisionGraphManager(){
      GUM_DESTRUCTOR ( MultiDimDecisionGraphManager );
    }



  // ############################################################################
  // @name Nodes manipulation methods.
  // ############################################################################

    // ============================================================================
    /// Sets root node of decision diagram
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraphManager< GUM_SCALAR>::setRootNode ( const NodeId& root ){
      __decisionGraph->__root = root;
    }



    // ============================================================================
    // Inserts a new non terminal node in graph.
    // NodeId of this node is generated automatically.
    //
    // @param var Associated variable
    // @throw OperationNotAllowed if MultiDimDecisionGraph has no variables yet.
    // @throw OperationNotAllowed if MultiDimDecisionGraph has no this variable yet.
    // @return the id of the added non terminal node.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    NodeId MultiDimDecisionGraphManager< GUM_SCALAR>::addNonTerminalNode ( const DiscreteVariable* var, NodeId nid ){

        typename MultiDimDecisionGraph< GUM_SCALAR>::InternalNode* newNodeStruct =
                new typename MultiDimDecisionGraph< GUM_SCALAR>::InternalNode( var );

        __decisionGraph->__internalNodeMap.insert(nid, newNodeStruct);

        MultiDimDecisionGraph< GUM_SCALAR>::_addElemToNICL( &(__decisionGraph->__var2NodeIdMap[var]), nid );

      return nid;
    }

    template<typename GUM_SCALAR>
    INLINE
    NodeId MultiDimDecisionGraphManager<GUM_SCALAR>::addNonTerminalNode( const DiscreteVariable* var ){
        typename MultiDimDecisionGraph< GUM_SCALAR>::InternalNode* newNodeStruct =
                new typename MultiDimDecisionGraph< GUM_SCALAR>::InternalNode( var );
        NodeId nid = __decisionGraph->__model.insertNode();
        __decisionGraph->__internalNodeMap.insert( nid, newNodeStruct );
        MultiDimDecisionGraph< GUM_SCALAR>::_addElemToNICL( &(__decisionGraph->__var2NodeIdMap[var]), nid );

        return nid;
    }

    template<typename GUM_SCALAR>
    INLINE
    NodeId MultiDimDecisionGraphManager<GUM_SCALAR>::addNonTerminalNode( const DiscreteVariable* var, NodeId* sons ){
        typename MultiDimDecisionGraph< GUM_SCALAR>::InternalNode* newNodeStruct =
                new typename MultiDimDecisionGraph< GUM_SCALAR>::InternalNode( var, sons );
        NodeId nid = __decisionGraph->__model.insertNode();
        __decisionGraph->__internalNodeMap.insert( nid, newNodeStruct );
        MultiDimDecisionGraph< GUM_SCALAR>::_addElemToNICL( &(__decisionGraph->__var2NodeIdMap[var]), nid );
        for( Idx i = 0; i < newNodeStruct->nbSons(); i++ )
            if( !__decisionGraph->isTerminalNode(sons[i]))
                __decisionGraph->__internalNodeMap[sons[i]]->addParent( nid, i );

        return nid;
    }

    // ============================================================================
    // Adds a value to the MultiDimDecisionGraph.
    // This will create a terminal node, which of id is returned.
    // If a terminal node with such value already exists,
    // its id will be return instead.
    //
    // @param value The value added by copy.
    // @return the id of the terminal node hence created.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    NodeId
    MultiDimDecisionGraphManager< GUM_SCALAR>::addTerminalNode ( const GUM_SCALAR& value ){

      if( __decisionGraph->__valueMap.existsSecond( value ) )
        return __decisionGraph->__valueMap.first( value );

      NodeId node = __decisionGraph->__model.insertNode();
      __decisionGraph->__valueMap.insert(node, value);
      return node;
    }



    // ============================================================================
    // Erases a node from the diagram.
    //
    // @param id The id of the variable to erase.
    //
    // ============================================================================
    template<typename GUM_SCALAR>
    void
    MultiDimDecisionGraphManager< GUM_SCALAR>::eraseNode ( NodeId eraseId, NodeId replacingId, bool updateParents ){

        if ( __decisionGraph->isTerminalNode( eraseId ) ) {

            for(SequenceIterator< const DiscreteVariable*> iterVar = __decisionGraph->variablesSequence().begin();
                    iterVar != __decisionGraph->variablesSequence().end(); ++iterVar){

                typename MultiDimDecisionGraph< GUM_SCALAR>::NICLElem* nodeIter = __decisionGraph->__var2NodeIdMap[*iterVar];
                while( nodeIter != nullptr ){

                    for( Idx modality = 0; modality < (*iterVar)->domainSize(); ++modality)
                        if( __decisionGraph->node( nodeIter->elemId )->son(modality) == eraseId )
                            setSon( nodeIter->elemId, modality, replacingId );

                    nodeIter = nodeIter->nextElem;

                }
            }
            __decisionGraph->__valueMap.eraseFirst( eraseId );

        } else {

            typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* eraseNode = __decisionGraph->__internalNodeMap[eraseId];

            if(updateParents){
              typename MultiDimDecisionGraph<GUM_SCALAR>::PICLElem* picle = eraseNode->parents();
              while( picle != nullptr ) {
                setSon( picle->parentId, picle->modality, replacingId );
                picle = picle->nextElem;
              }
            }

            MultiDimDecisionGraph<GUM_SCALAR>::_removeElemFromNICL(
                        &(__decisionGraph->__var2NodeIdMap[__decisionGraph->__internalNodeMap[eraseId]->nodeVar()]), eraseId );
            delete __decisionGraph->__internalNodeMap[eraseId];
            __decisionGraph->__internalNodeMap.erase(eraseId);
        }

        __decisionGraph->__model.eraseNode(eraseId);

        if ( __decisionGraph->__root == eraseId )
          __decisionGraph->__root = replacingId;
    }



    // ============================================================================
    /// Sets nodes son for given modality to designated son node
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraphManager<GUM_SCALAR>::setSon( const NodeId& node, const Idx& modality, const NodeId& sonNode ){
        __decisionGraph->__internalNodeMap[node]->setSon( modality, sonNode );
        if( sonNode && !__decisionGraph->isTerminalNode( sonNode ))
            __decisionGraph->__internalNodeMap[sonNode]->addParent( node, modality );
    }



    // ============================================================================
    // Changes var position in variable sequence
    // ============================================================================
    template<typename GUM_SCALAR>
    void MultiDimDecisionGraphManager<GUM_SCALAR>::moveTo( const DiscreteVariable* x, Idx desiredPos ){

      // First we determine the position of both variable
      // We also determine which one precede the other
      if( __decisionGraph->variablesSequence().pos(x) > desiredPos )
        for(Idx currentPos = __decisionGraph->variablesSequence().pos(x); currentPos != desiredPos; currentPos-- ){
          __adjacentSwap( __decisionGraph->variablesSequence().atPos(currentPos - 1), __decisionGraph->variablesSequence().atPos(currentPos));
          __decisionGraph->_invert( currentPos - 1, currentPos );






//          std::cout << std::endl << "Diagram After intermediate swapping : " << std::endl << __decisionGraph->toDot(true) << std::endl;

//          for(SequenceIteratorSafe<const DiscreteVariable*> varIter = __decisionGraph->variablesSequence().beginSafe();
//              varIter != __decisionGraph->variablesSequence().endSafe(); ++varIter){
//            std::cout << "Variable : " << (*varIter)->name();
//            const typename MultiDimDecisionGraph<GUM_SCALAR>::NICLElem* nodeIter = __decisionGraph->varNodeListe( *varIter );
//            while( nodeIter ){
//              std::cout << " | " << nodeIter->elemId;
//              nodeIter = nodeIter->nextElem;
//            }
//            std::cout << std::endl;
//          }
//          std::cout << std::endl << std::endl;

//           exit(1);


        }
      else
        for(Idx currentPos = __decisionGraph->variablesSequence().pos(x); currentPos != desiredPos; currentPos++ ) {
          __adjacentSwap( __decisionGraph->variablesSequence().atPos(currentPos),__decisionGraph->variablesSequence().atPos(currentPos + 1));
          __decisionGraph->_invert( currentPos, currentPos + 1 );






//          std::cout << std::endl << "Diagram After intermediate swapping : " << std::endl << __decisionGraph->toDot() << std::endl;

//          for(SequenceIteratorSafe<const DiscreteVariable*> varIter = __decisionGraph->variablesSequence().beginSafe();
//              varIter != __decisionGraph->variablesSequence().endSafe(); ++varIter){
//            std::cout << "Variable : " << (*varIter)->name();
//            const typename MultiDimDecisionGraph<GUM_SCALAR>::NICLElem* nodeIter = __decisionGraph->varNodeListe( *varIter );
//            while( nodeIter ){
//              std::cout << " | " << nodeIter->elemId;
//              nodeIter = nodeIter->nextElem;
//            }
//            std::cout << std::endl;
//          }
//          std::cout << std::endl << std::endl;




        }

//      reduce();
    }



    // ============================================================================
    // Swap two adjacent variable.
    // Order is important here.
    // X must precede Y before the swap (at the end Y will then precede X).
    // Not respecting this constraint leads to hasardous behaviour.
    // ============================================================================
    template<typename GUM_SCALAR>
    void MultiDimDecisionGraphManager<GUM_SCALAR>::__adjacentSwap( const DiscreteVariable* x, const DiscreteVariable* y ){

      typename MultiDimDecisionGraph<GUM_SCALAR>::NICLElem* oldxNodes = __decisionGraph->__var2NodeIdMap[ x ];
      __decisionGraph->__var2NodeIdMap[ x ] = nullptr;
      typename MultiDimDecisionGraph<GUM_SCALAR>::NICLElem* oldyNodes = __decisionGraph->__var2NodeIdMap[ y ];
      __decisionGraph->__var2NodeIdMap[ y ] = nullptr;


      typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* currentOldXNode = nullptr;
//      typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* currentOldXSonNode = nullptr;
      NodeId* currentNewXNodeSons = nullptr;
      NodeId currentNewXNodeId = 0;
      Idx indx = 0;

      NodeId* currentNewYNodeSons = nullptr;
      NodeId currentNewYNodeId = 0;
      Idx indy = 0;

      typename MultiDimDecisionGraph<GUM_SCALAR>::NICLElem* nextn = nullptr;

      while( oldxNodes ){

        // Recuperation de la structure associée au noeud courant
        currentOldXNode = __decisionGraph->__internalNodeMap[oldxNodes->elemId];

        // Creation de la structure amenee à remplacer la structure courante
        // et associée par conséquence à y
        currentNewYNodeSons = MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::allocateNodeSons(y);

        // Maintenant il faut remapper le graphe en insérant un noeud associé à x
        // Pour chaque valeur de y
        for( indy = 0; indy < y->domainSize(); ++indy ){

          // Creation du vecteur fils du noeud associe à x qui sera sont descendant pour cette valeur-ci
            currentNewXNodeSons = MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::allocateNodeSons(x);

          // Iteration sur les différente valeurs de x pour faire le mapping
          for(indx = 0; indx < x->domainSize(); ++indx){
            currentNewXNodeSons[ indx ] = currentOldXNode->son(indx);
            if(!__decisionGraph->isTerminalNode(currentOldXNode->son(indx)) && __decisionGraph->node(currentOldXNode->son(indx))->nodeVar() == y)
              currentNewXNodeSons[ indx ] = __decisionGraph->node(currentOldXNode->son(indx))->son(indy);
          }

          // Insertion du nouveau noeud x
          currentNewXNodeId = currentNewXNodeSons[0];
          if( isRedundant( x, currentNewXNodeSons ) ) {
              MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( currentNewXNodeSons, x->domainSize()*sizeof(NodeId) );
          } else {
              currentNewXNodeId = checkIsomorphism( x, currentNewXNodeSons );
              if( currentNewXNodeId != 0 ) {
                  MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( currentNewXNodeSons, x->domainSize()*sizeof(NodeId) );
              } else {
                  currentNewXNodeId = addNonTerminalNode( x, currentNewXNodeSons );
              }
          }
          currentNewYNodeSons[ indy ] = currentNewXNodeId;
        }
        
        // Transformation de l'ancien noeud x en nouveau noeud y
        currentNewYNodeId = currentNewYNodeSons[0];
        if( isRedundant( y, currentNewYNodeSons ) ) {
            _migrateNode(oldxNodes->elemId, currentNewYNodeId);
            MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( currentNewYNodeSons, y->domainSize()*sizeof(NodeId) );
        } else {
            currentNewYNodeId = checkIsomorphism( y, currentNewYNodeSons );
            if( currentNewYNodeId != 0 ) {
                _migrateNode(oldxNodes->elemId, currentNewYNodeId);
                MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( currentNewYNodeSons, y->domainSize()*sizeof(NodeId) );
            } else {
                // Mise à jour des fils (il ne doivent plus considérer currentOldXNode comme un parent)
                for( Idx i = 0; i < currentOldXNode->nodeVar()->domainSize(); ++i){
                    if( __decisionGraph->__internalNodeMap.exists( currentOldXNode->son(i))){
                        __decisionGraph->__internalNodeMap[currentOldXNode->son(i)]->removeParent(oldxNodes->elemId, i);
                    }
                }
                // Transmutation du noeud d'un noeud associé à X à un noeud associé à Y
                currentOldXNode->setNode( y, currentNewYNodeSons );
                // Mise à jour des nouveaux fils (ils doivent considerer currentOldXNode comme un parent)
                for( Idx i = 0; i < currentOldXNode->nodeVar()->domainSize(); ++i){
                  if( __decisionGraph->__internalNodeMap.exists( currentNewYNodeSons[i])){
                    __decisionGraph->__internalNodeMap[currentNewYNodeSons[i]]->addParent(oldxNodes->elemId, i);
                  }
                }

                MultiDimDecisionGraph<GUM_SCALAR>::_addElemToNICL( &(__decisionGraph->__var2NodeIdMap[y]), oldxNodes->elemId );
            }
        }

        nextn = oldxNodes->nextElem;
        MultiDimDecisionGraph<GUM_SCALAR>::_removeElemFromNICL(&oldxNodes, oldxNodes->elemId);
        oldxNodes = nextn;
      }

      while(oldyNodes){
        nextn = oldyNodes->nextElem;
        if( __decisionGraph->__internalNodeMap[oldyNodes->elemId]->parents() == nullptr ){

            for( Idx i = 0; i < __decisionGraph->__internalNodeMap[oldyNodes->elemId]->nodeVar()->domainSize(); ++i)
                if( __decisionGraph->__internalNodeMap.exists( __decisionGraph->__internalNodeMap[oldyNodes->elemId]->son(i)))
                    __decisionGraph->__internalNodeMap[__decisionGraph->__internalNodeMap[oldyNodes->elemId]->son(i)]->removeParent(oldyNodes->elemId, i);
            delete __decisionGraph->__internalNodeMap[oldyNodes->elemId];
            __decisionGraph->__internalNodeMap.erase(oldyNodes->elemId);
            __decisionGraph->__model.eraseNode(oldyNodes->elemId);
        } else {
            MultiDimDecisionGraph<GUM_SCALAR>::_addElemToNICL( &(__decisionGraph->__var2NodeIdMap[y]), oldyNodes->elemId);
        }
        MultiDimDecisionGraph<GUM_SCALAR>::_removeElemFromNICL(&oldyNodes, oldyNodes->elemId);
        oldyNodes = nextn;
      }
    }



    // ============================================================================
    //
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraphManager<GUM_SCALAR>::_migrateNode( const NodeId& origin,
                                                                 const NodeId& destination ){

        typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* org = __decisionGraph->__internalNodeMap[origin];
        // Mis à jour des parents suite au déplacement
        typename MultiDimDecisionGraph<GUM_SCALAR>::PICLElem* picle = org->parents();
        while( picle != nullptr ) {
            setSon( picle->parentId, picle->modality, destination );
            picle = picle->nextElem;
        }

        // Mis à jour des enfants suite au déplacement
        for( Idx i = 0; i < org->nbSons(); ++i )
            if( __decisionGraph->__internalNodeMap.exists(org->son(i)))
                __decisionGraph->__internalNodeMap[ org->son(i) ]->removeParent(origin, i);

        delete org;
        __decisionGraph->__internalNodeMap.erase(origin);
        __decisionGraph->__model.eraseNode(origin);

        if(__decisionGraph->root() == origin )
            this->setRootNode( destination );
    }



    // ============================================================================
    //
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    NodeId MultiDimDecisionGraphManager< GUM_SCALAR>::checkIsomorphism ( const DiscreteVariable* var,
                                                                         NodeId* sons){

      const typename MultiDimDecisionGraph< GUM_SCALAR>::InternalNode* nody = nullptr;
      Idx i = 0;

      // ************************************************************************
      // Check abscence of identical node
      typename MultiDimDecisionGraph< GUM_SCALAR>::NICLElem* currentElem = __decisionGraph->__var2NodeIdMap[var];
      while( currentElem != nullptr ){

        nody = __decisionGraph->__internalNodeMap[currentElem->elemId];

        // Check on the other sons
        i = 0;
        while( i < var->domainSize() && sons[i] == nody->son(i))
            ++i;
        if(i == var->domainSize())
          return currentElem->elemId;

        currentElem = currentElem->nextElem;
      }
      return 0;
    }



    // ============================================================================
    //
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    bool MultiDimDecisionGraphManager< GUM_SCALAR>::isRedundant ( const DiscreteVariable* var,
                                                                  NodeId* sons){
        for( Idx m = 1; m < var->domainSize(); m++ )
            if( sons[m] != sons[0] )
                return false;
        return true;
    }

    // ============================================================================
    // Ensures that every isomorphic subgraphs are merged together.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void
    MultiDimDecisionGraphManager< GUM_SCALAR>::reduce( ){

      typename MultiDimDecisionGraph<GUM_SCALAR>::NICLElem* currentNodeId = nullptr;
      typename MultiDimDecisionGraph<GUM_SCALAR>::NICLElem* nextNodeId = nullptr;
      typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* currentNode = nullptr;
      bool theSame = true;
      Idx currentInd;

      for( SequenceIterator<const DiscreteVariable*> varIter = __decisionGraph->variablesSequence().rbegin();
             varIter != __decisionGraph->variablesSequence().rend(); --varIter ){

        currentNodeId = __decisionGraph->__var2NodeIdMap[*varIter];

        while( currentNodeId != nullptr ){

          nextNodeId = currentNodeId->nextElem;
          currentNode = __decisionGraph->__internalNodeMap[currentNodeId->elemId];

          // ==================================================================================
          // First isomorphism to handle is the one where all node children are the same
          theSame = true;
          for( currentInd = 1; currentInd < (*varIter)->domainSize(); currentInd++){
            if( currentNode->son(currentInd) != currentNode->son(0)){
              theSame = false;
              break;
            }
          }

          if( theSame == true ){
            _migrateNode(currentNodeId->elemId, currentNode->son(0));
            MultiDimDecisionGraph<GUM_SCALAR>::_removeElemFromNICL( &(__decisionGraph->__var2NodeIdMap[*varIter]), currentNodeId->elemId);
            currentNodeId = nextNodeId;
            continue;
          }


          // ====================================================================================
          // Second isomorphism to handle is the one where two node have same variable and same children
          if( nextNodeId ){
            typename MultiDimDecisionGraph< GUM_SCALAR>::NICLElem* anotherNodeId = currentNodeId->nextElem;
            typename MultiDimDecisionGraph< GUM_SCALAR>::InternalNode* anotherNode = nullptr;
            Idx modality = 0;
            while( anotherNodeId->nextElem != nullptr ){

              nextNodeId = anotherNodeId->nextElem;
              anotherNode = __decisionGraph->__internalNodeMap[ anotherNodeId->elemId ];

              // Check on the other sons
              for(modality = 0; modality < (*varIter)->domainSize(); ++modality ) {
                if( anotherNode->son(modality) != currentNode->son(modality) )
                  break;
                if( modality == (*varIter)->domainSize() - 1){
                  _migrateNode( anotherNodeId->elemId, currentNodeId->elemId);
                  MultiDimDecisionGraph<GUM_SCALAR>::_removeElemFromNICL( &(__decisionGraph->__var2NodeIdMap[*varIter]), anotherNodeId->elemId);
                }
              }

              anotherNodeId = nextNodeId;
            }
          }

          currentNodeId = nextNodeId;
        }
      }
    }


    template<typename GUM_SCALAR>
    INLINE
    void
    MultiDimDecisionGraphManager< GUM_SCALAR>::clean( ){
      Sequence< const DiscreteVariable* > oldSequence(__decisionGraph->variablesSequence());
      for ( SequenceIterator< const DiscreteVariable* > varIter = oldSequence.begin(); varIter != oldSequence.end(); ++varIter )
        if( ! __decisionGraph->varNodeListe( *varIter) )
          __decisionGraph->erase(**varIter);
    }

} // namespace gum




// ============================================================================
// Inserts a new non terminal node in graph.
// NodeId of this node is generated automatically.
//
// @return the id of the added non terminal node.
// ============================================================================
//    template<typename GUM_SCALAR>
//    INLINE
//    NodeId MultiDimDecisionGraphManager< GUM_SCALAR>::addNonTerminalNode ( NodeId nid ){


//        // Getting the structure for this internal node
//        typename MultiDimDecisionGraph< GUM_SCALAR>::InternalNode* newNodeStruct =
//                new typename MultiDimDecisionGraph< GUM_SCALAR>::InternalNode();

//        // Getting a new id for this node
//        if( nid == 0 )
//          nid = __decisionGraph->__model.insertNode();

//        if( !__decisionGraph->__internalNodeMap.exists(nid) ){
//           __decisionGraph->__internalNodeMap.insert( nid, newNodeStruct );
//        }else{
//           __decisionGraph->__internalNodeMap[nid] = newNodeStruct;
//        }

//        return nid;
//    }




// *******************************************************************************************
// Verification
// First, we check if variable order has been specified
//      if ( __decisionGraph->variablesSequence().size() == 0 ){
//        GUM_ERROR ( OperationNotAllowed, "MultiDim has no variable" );
//      }else{
//      // if so we check if var is in the order or not
//        if ( !__decisionGraph->variablesSequence().exists( var ) ){
//          GUM_ERROR ( OperationNotAllowed, "Variable " << var->name() << " is not in the MultiDim" );
//        }
//      }

//      NodeId rnid = addNonTerminalNode(nid);
//      __decisionGraph->__internalNodeMap[rnid]->setNodeVar(var);
//      MultiDimDecisionGraph< GUM_SCALAR>::_addElemToNICL( &(__decisionGraph->__var2NodeIdMap[var]), rnid );


// ############################################################################
// @name Arcs manipulation methods.
// ############################################################################

// ============================================================================
// Adds an arc in the DD
//
// @param from and
// @param to as NodeId
// @param modality the modality on which arc is bind
// @throw NotFound If from and/or tail are not in the DD.
// @throw InvalidNode if head is a terminal node
// @throw OperationNotAllowed if arc doesn't respect variable order property
// @throw DuplicateElement if another arc linking those nodes already exists
//    // ============================================================================
//    template<typename GUM_SCALAR>
//    void
//    MultiDimDecisionGraphManager< GUM_SCALAR>::insertArc ( NodeId from, NodeId to, Idx modality ){

//        if(!__decisionGraph->__model.exists(from)){
//          GUM_ERROR ( NotFound, " Origin node " <<  from << " does not exist." )
//        }

//        if ( __decisionGraph->__valueMap.existsFirst ( from ) ) {
//          GUM_ERROR ( InvalidNode, " Origin node " <<  from << " is a terminal Node. No arcs can start from a terminal node" );
//        }

//        if(__decisionGraph->__internalNodeMap[ from ]->nbSons() <= modality){
//          GUM_ERROR ( InvalidArgument, " modality " << modality << " is too high for variable " << __decisionGraph->__internalNodeMap[ from ]->nodeVar()->name() << "." )
//        }

//        if(!__decisionGraph->__model.exists(to)){
//          GUM_ERROR ( NotFound, " Destination node " <<  to << " does not exist." )
//        }

//        if ( !__decisionGraph->__valueMap.existsFirst ( to ) ) {
//          if ( __decisionGraph->variablesSequence().pos ( __decisionGraph->__internalNodeMap[ from ]->nodeVar() )
//               >= __decisionGraph->variablesSequence().pos ( __decisionGraph->__internalNodeMap[ to ]->nodeVar() ) ) {
//            GUM_ERROR ( OperationNotAllowed, " This arc does not respect the variable order property. Variable " <<  __decisionGraph->__internalNodeMap[ from ]->nodeVar()->name()
//                << " tied to node " << from << " is after Variable " << __decisionGraph->__internalNodeMap[ to ]->nodeVar()->name() << " tied to node "
//                << to << " in variable order." );
//          }
//        }

//        for ( Idx i = 0; i < __decisionGraph->__internalNodeMap[ from ]->nbSons(); i++ )
//          if ( __decisionGraph->__internalNodeMap[ from ]->son(i) == to && i == modality ) {
//            GUM_ERROR ( DuplicateElement, " A same (meaning with same value " <<  modality << " ) arc linking those two nodes " << from << " -> " << to << " already exist." );
//            break;
//          }

//        __decisionGraph->__internalNodeMap[ from ]->setSon(modality, to);
//    }

// ============================================================================
// Erases arcs in the DD
//
// @param from and
// @param to as NodeId
// @throw InvalidArc If arc does not exist
// @warning due to the possibility that several arc with different value have the same from and to,
// if several arcs have different value but same parent and child, this method will erase all of them .
// If you want to erase a specific one, use eraseSpecificArc
// ============================================================================
//    template<typename GUM_SCALAR>
//    void
//    MultiDimDecisionGraphManager< GUM_SCALAR>::eraseArc ( NodeId from, NodeId to ){

//      if(!__decisionGraph->__model.exists(from)){
//        GUM_ERROR ( NotFound, " Origin node " <<  from << " does not exist." )
//      }

//      if(!__decisionGraph->__model.exists(to)){
//        GUM_ERROR ( NotFound, " Destination node " <<  to << " does not exist." )
//      }

//      if ( __decisionGraph->__valueMap.existsFirst ( from ) ) {
//        GUM_ERROR ( InvalidNode, " Origin node " <<  from << " is a terminal Node. No arcs can start from a terminal node" );
//      } else if ( !__decisionGraph->__valueMap.existsFirst ( to ) ) {
//        if ( __decisionGraph->variablesSequence().pos ( __decisionGraph->__internalNodeMap[ from ]->nodeVar() )
//             >= __decisionGraph->variablesSequence().pos ( __decisionGraph->__internalNodeMap[ to ]->nodeVar() ) ) {
//            GUM_ERROR ( InvalidArc, " This(those) arc(s) " << from << " - " << to <<" cannot exist since it(they) violate(s) variable order." );
//        }
//      }

//      for(Idx i = 0; i < __decisionGraph->__internalNodeMap[from]->nbSons(); i++)
//        if( __decisionGraph->__internalNodeMap[from]->son(i) == to )
//            __decisionGraph->__internalNodeMap[from]->setSon(i, 0);
//    }

// ============================================================================
// Erases an arc in the DD
//
// @param from and
// @param to as NodeId
// @param modality the modality corresponding to the to delete arc
// @throw InvalidArc If arc does not exist
// ============================================================================
//    template<typename GUM_SCALAR>
//    void
//    MultiDimDecisionGraphManager< GUM_SCALAR>::eraseSpecificArc ( NodeId from, NodeId to, Idx modality ){

//      if(!__decisionGraph->__model.exists(from)){
//        GUM_ERROR ( NotFound, " Origin node " <<  from << " does not exist." )
//      }

//      if(!__decisionGraph->__model.exists(to)){
//        GUM_ERROR ( NotFound, " Destination node " <<  to << " does not exist." )
//      }

//      if(__decisionGraph->__internalNodeMap[ from ]->nbSons() <= modality){
//        GUM_ERROR ( gum::InvalidArgument, " modality " << modality << " is too high for variable " << __decisionGraph->__internalNodeMap[ from ]->nodeVar()->name() << "." )
//      }

//      if ( __decisionGraph->__valueMap.existsFirst ( from ) ) {
//        GUM_ERROR ( InvalidNode, " Origin node " <<  from << " is a terminal Node. No arcs can start from a terminal node" );
//      } else if ( !__decisionGraph->__valueMap.existsFirst ( to ) ) {
//        // GUM_TRACE( "From : " << _varMap[from]->toString() << " - To : " << _varMap[ to ]->toString() << std::endl );
//        if ( __decisionGraph->variablesSequence().pos ( __decisionGraph->__internalNodeMap[ from ]->nodeVar() )
//             >= __decisionGraph->variablesSequence().pos ( __decisionGraph->__internalNodeMap[ to ]->nodeVar() ) ) {
//          GUM_ERROR ( OperationNotAllowed, " This arc does not respect the variable order property. Variable " <<  __decisionGraph->__internalNodeMap[ from ]->nodeVar()->name()
//              << " tied to node " << from << " is after Variable " << __decisionGraph->__internalNodeMap[ to ]->nodeVar()->name() << " tied to node "
//              << to << " in variable order." );
//        }
//      }

//      __decisionGraph->__internalNodeMap[from]->setSon(modality, 0);
//    }
