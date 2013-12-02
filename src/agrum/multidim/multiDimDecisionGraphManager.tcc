
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
    NodeId MultiDimDecisionGraphManager< GUM_SCALAR>::addNonTerminalNode ( const DiscreteVariable* var ){

      // *******************************************************************************************
      // Verification
      // First, we check if variable order has been specified
      if ( __decisionGraph->variablesSequence().size() == 0 ){
        GUM_ERROR ( OperationNotAllowed, "MultiDim has no variable" );
      }else{
      // if so we check if var is in the order or not
        if ( !__decisionGraph->variablesSequence().exists( var ) ){
          GUM_ERROR ( OperationNotAllowed, "Variable " << var->name() << " is not in the MultiDim" );
        }
      }

      // *********************************************************************************************      
      // Instanciation du noeud
      NodeId newNodeId = __decisionGraph->__model.insertNode();

      // Getting the structure for this internal node
      typename MultiDimDecisionGraph< GUM_SCALAR>::InternalNode* newNodeStruct = MultiDimDecisionGraph< GUM_SCALAR>::_allocateInternalNode(var);
      newNodeStruct->nodeVar = var;

      // *********************************************************************************************
      // Initialisation du vecteur fils.
      for( gum::Idx i = 0; i < var->domainSize(); ++i){
        newNodeStruct->nodeSons[i] = 0;
      }

      // *********************************************************************************************
      // Insertion du noeud
      __decisionGraph->__internalNodeMap.insert(newNodeId, newNodeStruct);
      MultiDimDecisionGraph< GUM_SCALAR>::_addElemToNICL( &(__decisionGraph->__var2NodeIdMap[var]), newNodeId );

      return newNodeId;
    }


    // ============================================================================
    //
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    NodeId MultiDimDecisionGraphManager< GUM_SCALAR>::addNonTerminalNode ( const DiscreteVariable* var,
                                                                                        NodeId* sons, NodeId nid){


      // Getting the structure for this internal node
      typename MultiDimDecisionGraph< GUM_SCALAR>::InternalNode* newNodeStruct = MultiDimDecisionGraph< GUM_SCALAR>::_allocateInternalNode(var, sons);

      // Initializing it
      newNodeStruct->nodeVar = var;
//      newNodeStruct->nodeSons = sons;
//      newNodeStruct->nodeDefaultSon = defaultSon;

      // Adding this node to the list of node bound to variable var
      // Getting a new id for this node
      if( nid == 0 ){
        nid = __decisionGraph->__model.insertNode();
        __decisionGraph->__internalNodeMap.insert(nid, newNodeStruct);
      }else
          __decisionGraph->__internalNodeMap[nid] = newNodeStruct;
      MultiDimDecisionGraph< GUM_SCALAR>::_addElemToNICL( &(__decisionGraph->__var2NodeIdMap[var]), nid );

      // Returning the result.
      return nid;
    }


    // ============================================================================
    //
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    NodeId
    MultiDimDecisionGraphManager< GUM_SCALAR>::checkIsomorphism ( const DiscreteVariable* var,
                                                                 NodeId* sons){/*,
                                                                 const NodeId& defaultSon){*/

      const typename MultiDimDecisionGraph< GUM_SCALAR>::InternalNode* nody = nullptr;
      Idx i = 0;

      // ************************************************************************
      // Check abscence of identical node
      typename MultiDimDecisionGraph< GUM_SCALAR>::NICLElem* currentElem = __decisionGraph->__var2NodeIdMap[var];
      while( currentElem != nullptr ){

        nody = __decisionGraph->__internalNodeMap[currentElem->elemId];
//        // Check on default son node
//        if(defaultSon != nody->nodeDefaultSon)
//          continue;

        // Check on the other sons
        for(i = 0; i < var->domainSize(); ++i ) {
          if( sons[i] != nody->nodeSons[i] )// &&
//              ( sons[i] != 0 || defaultSon != nody->nodeSons[i] ) &&
//              ( nody->nodeSons[i] != 0 || sons[i] != nody->nodeDefaultSon ))
            break;
          if(i == var->domainSize() - 1){
            MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( sons, var->domainSize()*sizeof(NodeId) );
            return currentElem->elemId;
          }
        }

        currentElem = currentElem->nextElem;
      }
      return 0;
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
    // @throw NotFound if node isn't in diagram
    //
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void
    MultiDimDecisionGraphManager< GUM_SCALAR>::eraseNode ( NodeId n ){

      if( !__decisionGraph->__model.exists ( n )){
        GUM_ERROR ( NotFound, "Node " <<  n << " does not exist in diagram." );
      }

      for(typename HashTable<const DiscreteVariable*, typename MultiDimDecisionGraph< GUM_SCALAR>::NICLElem*>::const_iterator iterVar = __decisionGraph->__var2NodeIdMap.begin();
          iterVar != __decisionGraph->__var2NodeIdMap.end(); ++iterVar){

        if( !__decisionGraph->isTerminalNode(n) &&
            __decisionGraph->variablesSequence().pos(iterVar.key()) >= __decisionGraph->variablesSequence().pos(__decisionGraph->__internalNodeMap[n]->nodeVar))
          continue;

        typename MultiDimDecisionGraph< GUM_SCALAR>::NICLElem* currentElem = *iterVar;
        while( currentElem != nullptr ){
//        for(List<NodeId>::const_iterator iterNode = (*iterVar)->begin(); iterNode != (iterVar)->end(); ++ iterNode){
          for( Idx iterSon = 0; iterSon < iterVar.key()->domainSize(); ++iterSon)
            if( __decisionGraph->__internalNodeMap[currentElem->elemId]->nodeSons[iterSon] == n )
              __decisionGraph->__internalNodeMap[currentElem->elemId]->nodeSons[iterSon] = 0;

//          if( __decisionGraph->__defaultSonMap.exists( *iterNode ) && _defaultArcMap[*iterNode] == n )
//            __decisionGraph->__defaultSonMap.erase( *iterNode );

          currentElem = currentElem->nextElem;
        }
      }

      if ( __decisionGraph->isTerminalNode(n) )
        __decisionGraph->__valueMap.eraseFirst(n);
      else {
////        __decisionGraph->__variableMap.erase(n);
////        delete __decisionGraph->__sonsMap[n];
////        __decisionGraph->__sonsMap.erase (n);
////        if(__decisionGraph->__defaultSonMap.exists(n))
////          __decisionGraph->__defaultSonMap.erase(n);
          MultiDimDecisionGraph<GUM_SCALAR>::_removeElemFromNICL( &(__decisionGraph->__var2NodeIdMap[__decisionGraph->__internalNodeMap[n]->nodeVar]), n);
          MultiDimDecisionGraph<GUM_SCALAR>::_deallocateInternalNode( __decisionGraph->__internalNodeMap[n]);
          __decisionGraph->__internalNodeMap.erase(n);
      }

      __decisionGraph->__model.eraseNode(n);

      if ( __decisionGraph->__root == n )
        __decisionGraph->__root = 0;
    }


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
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void
    MultiDimDecisionGraphManager< GUM_SCALAR>::insertArc ( NodeId from, NodeId to, Idx modality ){

        if(!__decisionGraph->__model.exists(from)){
          GUM_ERROR ( NotFound, " Origin node " <<  from << " does not exist." )
        }

        if ( __decisionGraph->__valueMap.existsFirst ( from ) ) {
          GUM_ERROR ( InvalidNode, " Origin node " <<  from << " is a terminal Node. No arcs can start from a terminal node" );
        }

        if(__decisionGraph->__internalNodeMap[ from ]->nodeVar->domainSize() <= modality){
          GUM_ERROR ( InvalidArgument, " modality " << modality << " is too high for variable " << __decisionGraph->__internalNodeMap[ from ]->nodeVar->name() << "." )
        }

        if(!__decisionGraph->__model.exists(to)){
          GUM_ERROR ( NotFound, " Destination node " <<  to << " does not exist." )
        }

        if ( !__decisionGraph->__valueMap.existsFirst ( to ) ) {
          if ( __decisionGraph->variablesSequence().pos ( __decisionGraph->__internalNodeMap[ from ]->nodeVar )
               >= __decisionGraph->variablesSequence().pos ( __decisionGraph->__internalNodeMap[ to ]->nodeVar ) ) {
            GUM_ERROR ( OperationNotAllowed, " This arc does not respect the variable order property. Variable " <<  __decisionGraph->__internalNodeMap[ from ]->nodeVar->name()
                << " tied to node " << from << " is after Variable " << __decisionGraph->__internalNodeMap[ to ]->nodeVar->name() << " tied to node "
                << to << " in variable order." );
          }
        }

        for ( Idx i = 0; i < __decisionGraph->__internalNodeMap[ from ]->nodeVar->domainSize(); i++ )
          if ( __decisionGraph->__internalNodeMap[ from ]->nodeSons[i] == to && i == modality ) {
            GUM_ERROR ( DuplicateElement, " A same (meaning with same value " <<  modality << " ) arc linking those two nodes " << from << " -> " << to << " already exist." );
            break;
          }

//        if ( _defaultArcMap.exists ( from ) && _defaultArcMap[from] == to )
//          return;

//        _model.insertArc ( from, to );

        __decisionGraph->__internalNodeMap[ from ]->nodeSons[modality] =  to;
//        ( *_varUsedModalitiesMap[ _varMap[from] ] ) [value]++;
    }

    // ============================================================================
    /*
     * Adds a default arc in the DD
     *
     * @param from and
     * @param to as NodeId
     * @throw NotFound If from and/or tail are not in the DD.
     * @throw InvalidNode if head is a terminal node
     * @throw OperationNotAllowed if arc doesn't respect variable order property
     * @throw DuplicateElement if another arc linking those nodes already exists
     */
    // ============================================================================
    ///@{
//    template<typename GUM_SCALAR>
//    INLINE
//    void
//    MultiDimDecisionGraphManager< GUM_SCALAR>::insertDefaultArc ( NodeId from, NodeId to ){
//        return 0;
//      }
//    template<typename GUM_SCALAR>
//    INLINE
//    void
//    MultiDimDecisionGraphManager< GUM_SCALAR>::unsafeInsertDefaultArc ( NodeId from, NodeId to ){
//        return 0;
//      }
    ///@}

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
    template<typename GUM_SCALAR>
    INLINE
    void
    MultiDimDecisionGraphManager< GUM_SCALAR>::eraseArc ( NodeId from, NodeId to ){

      if(!__decisionGraph->__model.exists(from)){
        GUM_ERROR ( NotFound, " Origin node " <<  from << " does not exist." )
      }

      if(!__decisionGraph->__model.exists(to)){
        GUM_ERROR ( NotFound, " Destination node " <<  to << " does not exist." )
      }

      if ( __decisionGraph->__valueMap.existsFirst ( from ) ) {
        GUM_ERROR ( InvalidNode, " Origin node " <<  from << " is a terminal Node. No arcs can start from a terminal node" );
      } else if ( !__decisionGraph->__valueMap.existsFirst ( to ) ) {
        if ( __decisionGraph->variablesSequence().pos ( __decisionGraph->__internalNodeMap[ from ]->nodeVar )
             >= __decisionGraph->variablesSequence().pos ( __decisionGraph->__internalNodeMap[ to ]->nodeVar ) ) {
            GUM_ERROR ( InvalidArc, " This(those) arc(s) " << from << " - " << to <<" cannot exist since it(they) violate(s) variable order." );
        }
      }

      for(Idx i = 0; i < __decisionGraph->__internalNodeMap[from]->nodeVar->domainSize(); i++)
        if( __decisionGraph->__internalNodeMap[from]->nodeSons[i] == to )
          __decisionGraph->__internalNodeMap[from]->nodeSons[i] = 0;
    }

    // ============================================================================
    // Erases an arc in the DD
    //
    // @param from and
    // @param to as NodeId
    // @param modality the modality corresponding to the to delete arc
    // @throw InvalidArc If arc does not exist
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void
    MultiDimDecisionGraphManager< GUM_SCALAR>::eraseSpecificArc ( NodeId from, NodeId to, Idx modality ){

      if(!__decisionGraph->__model.exists(from)){
        GUM_ERROR ( NotFound, " Origin node " <<  from << " does not exist." )
      }

      if(!__decisionGraph->__model.exists(to)){
        GUM_ERROR ( NotFound, " Destination node " <<  to << " does not exist." )
      }

      if(__decisionGraph->__internalNodeMap[ from ]->nodeVar->domainSize() <= modality){
        GUM_ERROR ( gum::InvalidArgument, " modality " << modality << " is too high for variable " << __decisionGraph->__internalNodeMap[ from ]->nodeVar->name() << "." )
      }

      if ( __decisionGraph->__valueMap.existsFirst ( from ) ) {
        GUM_ERROR ( InvalidNode, " Origin node " <<  from << " is a terminal Node. No arcs can start from a terminal node" );
      } else if ( !__decisionGraph->__valueMap.existsFirst ( to ) ) {
        // GUM_TRACE( "From : " << _varMap[from]->toString() << " - To : " << _varMap[ to ]->toString() << std::endl );
        if ( __decisionGraph->variablesSequence().pos ( __decisionGraph->__internalNodeMap[ from ]->nodeVar )
             >= __decisionGraph->variablesSequence().pos ( __decisionGraph->__internalNodeMap[ to ]->nodeVar ) ) {
          GUM_ERROR ( OperationNotAllowed, " This arc does not respect the variable order property. Variable " <<  __decisionGraph->__internalNodeMap[ from ]->nodeVar->name()
              << " tied to node " << from << " is after Variable " << __decisionGraph->__internalNodeMap[ to ]->nodeVar->name() << " tied to node "
              << to << " in variable order." );
        }
      }

      __decisionGraph->__internalNodeMap[from]->nodeSons[modality] = 0;
    }

  // ############################################################################
  // Miscelleanous methods.
  // ############################################################################

    // ============================================================================
    // For each non terminal node, this function first checks if node has a son
    // for each value of associated variable.
    // If not, function checks if defaultSon is already specified.
    // If not, function affect defaultSon to the terminal node associated with given value.
    // ============================================================================
//    template<typename GUM_SCALAR>
//    INLINE
//    void
//    MultiDimDecisionGraphManager< GUM_SCALAR>::fillWithDefaultArc( GUM_SCALAR defaultValue ){
//        return 0;
//      }

    // ============================================================================
    // Changes var position in variable sequence
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraphManager<GUM_SCALAR>::moveTo( const DiscreteVariable* x, Idx desiredPos ){

      // First we determine the position of both variable
      // We also determine which one precede the other
      if( __decisionGraph->variablesSequence().pos(x) > desiredPos )
        for(Idx currentPos = __decisionGraph->variablesSequence().pos(x); currentPos != desiredPos; currentPos-- ){
          __adjacentSwap( __decisionGraph->variablesSequence().atPos(currentPos - 1), __decisionGraph->variablesSequence().atPos(currentPos));
          __decisionGraph->_invert( currentPos - 1, currentPos );
        }
      else
        for(Idx currentPos = __decisionGraph->variablesSequence().pos(x); currentPos != desiredPos; currentPos++ ) {
          __adjacentSwap( __decisionGraph->variablesSequence().atPos(currentPos),__decisionGraph->variablesSequence().atPos(currentPos + 1));
          __decisionGraph->_invert( currentPos, currentPos + 1 );
        }

      removeRedundancy();
    }

    // ============================================================================
    // Swap two adjacent variable.
    // Order is important here.
    // X must precede Y before the swap (at the end Y will then precede X).
    // Not respecting this constraint leads to hasardous behaviour.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraphManager<GUM_SCALAR>::__adjacentSwap( const DiscreteVariable* x, const DiscreteVariable* y ){

      typename MultiDimDecisionGraph<GUM_SCALAR>::NICLElem* oldxNodes = __decisionGraph->__var2NodeIdMap[ x ];
      __decisionGraph->__var2NodeIdMap[ x ] = nullptr;
      typename MultiDimDecisionGraph<GUM_SCALAR>::NICLElem* oldyNodes = __decisionGraph->__var2NodeIdMap[ y ];
      __decisionGraph->__var2NodeIdMap[ y ] = nullptr;


      typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* currentOldXNode = nullptr;
      typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* currentSonNode = nullptr;
      NodeId* newYSons = nullptr;
      NodeId* newXSons = nullptr;
      typename MultiDimDecisionGraph<GUM_SCALAR>::NICLElem* nextn = nullptr;
      Idx indy = 0;
      Idx indx = 0;

      while( oldxNodes ){

        // Recuperation de la structure associée au noeud courant
        currentOldXNode = __decisionGraph->__internalNodeMap[oldxNodes->elemId];

        // Creation de la structure amenee à remplacer la structure courante
        // et associée par conséquence à y
        newYSons = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(y->domainSize()*sizeof(NodeId)) );

        // Maintenant il faut remapper le graphe en insérant un noeud associé à x
        // Pour chaque valeur de y
        for( indy = 0; indy < y->domainSize(); ++indy ){

          // Creation du vecteur fils du noeud associe à x qui sera sont descendant pour cette valeur-ci
          newXSons = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(x->domainSize()*sizeof(NodeId)) );

          // Iteration sur les différente valeurs de x pour faire le mapping
          for( indx = 0; indx < x->domainSize(); ++indx ){
            if( __decisionGraph->isTerminalNode(currentOldXNode->nodeSons[indx]))
              newXSons[indx] = currentOldXNode->nodeSons[indx];
            else {
              currentSonNode = __decisionGraph->__internalNodeMap[ currentOldXNode->nodeSons[indx] ];
              if( currentSonNode->nodeVar == y )
                newXSons[indx] = currentSonNode->nodeSons[indy];
              else
                newXSons[indx] = currentOldXNode->nodeSons[indx];
            }
          }

          // Insertion du nouveau noeud x
          newYSons[indy] = checkIsomorphism( x, newXSons );
          // Si non, alors on l'insère
          if( newYSons[indy] == 0 )
            newYSons[indy] = addNonTerminalNode( x, newXSons );
        }

        // Même opération pour y une fois la creation de ses fils achevée
        addNonTerminalNode( y, newYSons, oldxNodes->elemId );

        nextn = oldxNodes->nextElem;
        MultiDimDecisionGraph<GUM_SCALAR>::_deallocateInternalNode(currentOldXNode);
        MultiDimDecisionGraph<GUM_SCALAR>::_removeElemFromNICL(&oldxNodes, oldxNodes->elemId);
        oldxNodes = nextn;
      }

      while(oldyNodes){
        nextn = oldyNodes->nextElem;
        MultiDimDecisionGraph<GUM_SCALAR>::_deallocateInternalNode(__decisionGraph->__internalNodeMap[oldyNodes->elemId]);
        __decisionGraph->__internalNodeMap.erase(oldyNodes->elemId);
        __decisionGraph->__model.eraseNode(oldyNodes->elemId);
        MultiDimDecisionGraph<GUM_SCALAR>::_removeElemFromNICL(&oldyNodes, oldyNodes->elemId);
        oldyNodes = nextn;
      }
    }

    // ============================================================================
    // Ensures that every isomorphic subgraphs are merged together.
    // ============================================================================
//    template<typename GUM_SCALAR>
//    INLINE
//    void
//    MultiDimDecisionGraphManager< GUM_SCALAR>::reduce( bool checkRedundancy, bool checkIsomorphism ){

//      HashTable<NodeId,NodeId> old2NewId(500,true,false);
//      typename MultiDimDecisionGraph<GUM_SCALAR>::NICLElem* currentNodeId = nullptr;
//      typename MultiDimDecisionGraph<GUM_SCALAR>::NICLElem* nextNodeId = nullptr;
//      typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* currentNode = nullptr;
//      bool theSame = true;
//      Idx currentInd;

//      for( SequenceIterator<const DiscreteVariable*> varIter = __decisionGraph->variablesSequence().rbegin();
//             varIter != __decisionGraph->variablesSequence().rend(); --varIter ){

//        currentNodeId = __decisionGraph->__var2NodeIdMap[*varIter];

//        while( currentNodeId != nullptr ){
//          currentNode = __decisionGraph->__internalNodeMap[currentNodeId->elemId];

//          theSame = true;
//          for( currentInd = 0; currentInd < (*varIter)->domainSize(); currentInd++){
//            if( old2NewId.exists(currentNode->nodeSons[currentInd]) )
//              currentNode->nodeSons[currentInd] = old2NewId[currentNode->nodeSons[currentInd]];
//            if( currentNode->nodeSons[currentInd] != currentNode->nodeSons[0])
//              theSame = false;
//          }

//          if( checkRedundancy ){
//              nextNodeId = currentNodeId->nextElem;
//              if( theSame == true ){
//                old2NewId.insert(currentNodeId->elemId, currentNode->nodeSons[0] );
//                MultiDimDecisionGraph<GUM_SCALAR>::_deallocateInternalNode(currentNode);
//                __decisionGraph->__internalNodeMap.erase(currentNodeId->elemId);
//                __decisionGraph->__model.eraseNode(currentNodeId->elemId);
//                MultiDimDecisionGraph<GUM_SCALAR>::_removeElemFromNICL( &(__decisionGraph->__var2NodeIdMap[*varIter]), currentNodeId->elemId);
//              }
//              currentNodeId = nextNodeId;

//          }
//        }
//      }
//    }

} // namespace gum
