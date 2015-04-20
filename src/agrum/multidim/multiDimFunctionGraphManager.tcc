
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
 * @brief Template methods of gum::MultiDimFunctionGraphManager.
 *
 * @author Jean-Christophe Magnan
 *
 */
// ============================================================================
#include <agrum/multidim/multiDimFunctionGraphManager.h>
// ============================================================================
#include <agrum/multidim/FunctionGraphUtilities/link.h>
// ============================================================================

#define ALLOCATE(x) SmallObjectAllocator::instance().allocate(x)
#define DEALLOCATE(x,y) SmallObjectAllocator::instance().deallocate(x,y)

namespace gum {

  // ############################################################################
  // @name Constructors / Destructors
  // ############################################################################

    // ============================================================================
    // Default constructor.
    // Private.
    // You have to call MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >::manager() to get the instance
    // of MultiDimFunctionGraphManager bound to your function graph.
    // ============================================================================
    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    INLINE
    MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::MultiDimFunctionGraphManager(MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy > *mddg ){
      GUM_CONSTRUCTOR ( MultiDimFunctionGraphManager ) ;
      __FunctionGraph = mddg;
    }



    // ============================================================================
    // Destructor. Don't worry, it will be call on the destruction of your
    // MultiDimFunctionGraph.
    // ============================================================================
    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    INLINE
    MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::~MultiDimFunctionGraphManager(){
      GUM_DESTRUCTOR ( MultiDimFunctionGraphManager );
    }



  // ############################################################################
  // @name Nodes manipulation methods.
  // ############################################################################

    // ============================================================================
    /// Sets root node of decision diagram
    // ============================================================================
    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    INLINE
    void MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::setRootNode ( const NodeId& root ){
      __FunctionGraph->__root = root;
    }



    // ============================================================================
    // Inserts a new non terminal node in graph.
    // NodeId of this node is generated automatically.
    //
    // @param var Associated variable
    // @throw OperationNotAllowed if MultiDimFunctionGraph has no variables yet.
    // @throw OperationNotAllowed if MultiDimFunctionGraph has no this variable yet.
    // @return the id of the added non terminal node.
    // ============================================================================
    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    INLINE
    NodeId MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::addInternalNode ( const DiscreteVariable* var,
                                                                                              NodeId nid ){

        InternalNode* newNodeStruct = new InternalNode( var );

        __FunctionGraph->__internalNodeMap.insert(nid, newNodeStruct);

        __FunctionGraph->__var2NodeIdMap[var]->addLink(nid);

      return nid;
    }

    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    INLINE
    NodeId MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::addInternalNode( const DiscreteVariable* var ){
        InternalNode* newNodeStruct = new InternalNode( var );
        NodeId nid = __FunctionGraph->__model.addNode();
        __FunctionGraph->__internalNodeMap.insert( nid, newNodeStruct );
        __FunctionGraph->__var2NodeIdMap[var]->addLink(nid);

        return nid;
    }

    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    INLINE
    NodeId MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::_addInternalNode( const DiscreteVariable* var,
                                                                                             NodeId* sons ){
        InternalNode* newNodeStruct = new InternalNode( var, sons );
        NodeId nid = __FunctionGraph->__model.addNode();
        __FunctionGraph->__internalNodeMap.insert( nid, newNodeStruct );
        __FunctionGraph->__var2NodeIdMap[var]->addLink(nid);
        for( Idx i = 0; i < newNodeStruct->nbSons(); i++ )
            if( !__FunctionGraph->isTerminalNode(sons[i]))
                __FunctionGraph->__internalNodeMap[sons[i]]->addParent( nid, i );

        return nid;
    }

    // ============================================================================
    // Adds a value to the MultiDimFunctionGraph.
    // This will create a terminal node, which of id is returned.
    // If a terminal node with such value already exists,
    // its id will be return instead.
    //
    // @param value The value added by copy.
    // @return the id of the terminal node hence created.
    // ============================================================================
    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    INLINE
    NodeId
    MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::addTerminalNode ( const GUM_SCALAR& value ){

      if( __FunctionGraph->existsTerminalNode( value ) )
        return __FunctionGraph->terminalNodeId( value );

      NodeId node = __FunctionGraph->__model.addNode();
      __FunctionGraph->addTerminalNode(node, value);
      return node;
    }



    // ============================================================================
    // Erases a node from the diagram.
    //
    // @param id The id of the variable to erase.
    //
    // ============================================================================
    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    void
    MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::eraseNode ( NodeId eraseId,
                                                                              NodeId replacingId,
                                                                              bool updateParents ){

        if ( __FunctionGraph->isTerminalNode( eraseId ) ) {

            for(SequenceIterator< const DiscreteVariable*> iterVar = __FunctionGraph->variablesSequence().begin();
                    iterVar != __FunctionGraph->variablesSequence().end(); ++iterVar){

                Link<NodeId>* nodeIter = __FunctionGraph->__var2NodeIdMap[*iterVar]->list();
                while( nodeIter != nullptr ){

                    for( Idx modality = 0; modality < (*iterVar)->domainSize(); ++modality)
                        if( __FunctionGraph->node( nodeIter->element() )->son(modality) == eraseId )
                            setSon( nodeIter->element(), modality, replacingId );

                    nodeIter = nodeIter->nextLink();

                }
            }
            __FunctionGraph->eraseTerminalNode( eraseId );

        } else {

            InternalNode* eraseNode = __FunctionGraph->__internalNodeMap[eraseId];

            if(updateParents){
              Link<Parent>* picle = eraseNode->parents();
              while( picle != nullptr ) {
                setSon( picle->element().parentId, picle->element().modality, replacingId );
                picle = picle->nextLink();
              }
            }

            __FunctionGraph->__var2NodeIdMap[ __FunctionGraph->__internalNodeMap[eraseId]->nodeVar() ]->searchAndRemoveLink(eraseId);

            delete __FunctionGraph->__internalNodeMap[eraseId];
            __FunctionGraph->__internalNodeMap.erase(eraseId);
        }

        __FunctionGraph->__model.eraseNode(eraseId);

        if ( __FunctionGraph->__root == eraseId )
          __FunctionGraph->__root = replacingId;
    }



    // ============================================================================
    /// Sets nodes son for given modality to designated son node
    // ============================================================================
    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    INLINE
    void MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::setSon( const NodeId& node,
                                                                               const Idx& modality,
                                                                               const NodeId& sonNode ){
        __FunctionGraph->__internalNodeMap[node]->setSon( modality, sonNode );
        if( sonNode && !__FunctionGraph->isTerminalNode( sonNode ))
            __FunctionGraph->__internalNodeMap[sonNode]->addParent( node, modality );
    }



    // ============================================================================
    // Changes var position in variable sequence
    // ============================================================================
    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    void MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::moveTo( const DiscreteVariable* x, Idx desiredPos ){

      // First we determine the position of both variable
      // We also determine which one precede the other
      if( __FunctionGraph->variablesSequence().pos(x) > desiredPos )
        for(Idx currentPos = __FunctionGraph->variablesSequence().pos(x); currentPos != desiredPos; currentPos-- ){
          __adjacentSwap( __FunctionGraph->variablesSequence().atPos(currentPos - 1), __FunctionGraph->variablesSequence().atPos(currentPos));
          __FunctionGraph->_invert( currentPos - 1, currentPos );

        }
      else
        for(Idx currentPos = __FunctionGraph->variablesSequence().pos(x); currentPos != desiredPos; currentPos++ ) {
          __adjacentSwap( __FunctionGraph->variablesSequence().atPos(currentPos),__FunctionGraph->variablesSequence().atPos(currentPos + 1));
          __FunctionGraph->_invert( currentPos, currentPos + 1 );

        }

//      reduce();
    }



    // ============================================================================
    // Swap two adjacent variable.
    // Order is important here.
    // X must precede Y before the swap (at the end Y will then precede X).
    // Not respecting this constraint leads to hasardous behaviour.
    // ============================================================================
    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    void MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::__adjacentSwap( const DiscreteVariable* x,
                                                                                       const DiscreteVariable* y ){

      LinkedList<NodeId>* oldxNodes = __FunctionGraph->__var2NodeIdMap[ x ];
      __FunctionGraph->__var2NodeIdMap[ x ] = new LinkedList<NodeId>();
      LinkedList<NodeId>* oldyNodes = __FunctionGraph->__var2NodeIdMap[ y ];
      __FunctionGraph->__var2NodeIdMap[ y ] = new LinkedList<NodeId>();


      InternalNode* currentOldXNode = nullptr;
      NodeId* currentNewXNodeSons = nullptr;
      Idx indx = 0;

      NodeId* currentNewYNodeSons = nullptr;
      NodeId currentNewYNodeId = 0;
      Idx indy = 0;

      while( oldxNodes->list() ){

        // Recuperation de la structure associée au noeud courant
        currentOldXNode = __FunctionGraph->__internalNodeMap[oldxNodes->list()->element()];

        // Creation de la structure amenee à remplacer la structure courante
        // et associée par conséquence à y
        currentNewYNodeSons = InternalNode::allocateNodeSons(y);

        // Maintenant il faut remapper le graphe en insérant un noeud associé à x
        // Pour chaque valeur de y
        for( indy = 0; indy < y->domainSize(); ++indy ){

          // Creation du vecteur fils du noeud associe à x qui sera sont descendant pour cette valeur-ci
            currentNewXNodeSons = InternalNode::allocateNodeSons(x);

          // Iteration sur les différente valeurs de x pour faire le mapping
          for(indx = 0; indx < x->domainSize(); ++indx){
            currentNewXNodeSons[ indx ] = currentOldXNode->son(indx);
            if(!__FunctionGraph->isTerminalNode(currentOldXNode->son(indx)) && __FunctionGraph->node(currentOldXNode->son(indx))->nodeVar() == y)
              currentNewXNodeSons[ indx ] = __FunctionGraph->node(currentOldXNode->son(indx))->son(indy);
          }

          // Insertion du nouveau noeud x
          currentNewYNodeSons[ indy ] = _nodeRedundancyCheck( x, currentNewXNodeSons);
        }
        
        // Transformation de l'ancien noeud x en nouveau noeud y
        currentNewYNodeId = currentNewYNodeSons[0];
        if( __isRedundant( y, currentNewYNodeSons ) ) {
            _migrateNode(oldxNodes->list()->element(), currentNewYNodeId);
            DEALLOCATE( currentNewYNodeSons, y->domainSize()*sizeof(NodeId) );
        } else {
            currentNewYNodeId = __checkIsomorphism( y, currentNewYNodeSons );
            if( currentNewYNodeId != 0 ) {
                _migrateNode(oldxNodes->list()->element(), currentNewYNodeId);
                DEALLOCATE( currentNewYNodeSons, y->domainSize()*sizeof(NodeId) );
            } else {
                // Mise à jour des fils (il ne doivent plus considérer currentOldXNode comme un parent)
                for( Idx i = 0; i < currentOldXNode->nodeVar()->domainSize(); ++i){
                    if( __FunctionGraph->__internalNodeMap.exists( currentOldXNode->son(i))){
                        __FunctionGraph->__internalNodeMap[currentOldXNode->son(i)]->removeParent(oldxNodes->list()->element(), i);
                    }
                }
                // Transmutation du noeud d'un noeud associé à X à un noeud associé à Y
                currentOldXNode->setNode( y, currentNewYNodeSons );
                // Mise à jour des nouveaux fils (ils doivent considerer currentOldXNode comme un parent)
                for( Idx i = 0; i < currentOldXNode->nodeVar()->domainSize(); ++i){
                  if( __FunctionGraph->__internalNodeMap.exists( currentNewYNodeSons[i])){
                    __FunctionGraph->__internalNodeMap[currentNewYNodeSons[i]]->addParent(oldxNodes->list()->element(), i);
                  }
                }

                __FunctionGraph->__var2NodeIdMap[y]->addLink( oldxNodes->list()->element() );
            }
        }

        oldxNodes->searchAndRemoveLink( oldxNodes->list()->element() );
      }
      delete oldxNodes;

      while(oldyNodes->list()){
        NodeId curId = oldyNodes->list()->element();
        if( __FunctionGraph->__internalNodeMap[curId]->parents() == nullptr ){

            for( Idx i = 0; i < __FunctionGraph->__internalNodeMap[curId]->nodeVar()->domainSize(); ++i)
                if( __FunctionGraph->__internalNodeMap.exists( __FunctionGraph->__internalNodeMap[curId]->son(i)))
                    __FunctionGraph->__internalNodeMap[__FunctionGraph->__internalNodeMap[curId]->son(i)]->removeParent(curId, i);
            delete __FunctionGraph->__internalNodeMap[curId];
            __FunctionGraph->__internalNodeMap.erase(curId);
            __FunctionGraph->__model.eraseNode(curId);
        } else {
            __FunctionGraph->__var2NodeIdMap[y]->addLink( curId );
        }
        oldyNodes->searchAndRemoveLink(curId);
      }
      delete oldyNodes;
    }



    // ============================================================================
    //
    // ============================================================================
    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    INLINE
    void MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::_migrateNode( const NodeId& origin,
                                                                 const NodeId& destination ){

        InternalNode* org = __FunctionGraph->__internalNodeMap[origin];
        // Mis à jour des parents suite au déplacement
        Link<Parent>* picle = org->parents();
        while( picle != nullptr ) {
            setSon( picle->element().parentId, picle->element().modality, destination );
            picle = picle->nextLink();
        }

        // Mis à jour des enfants suite au déplacement
        for( Idx i = 0; i < org->nbSons(); ++i )
            if( __FunctionGraph->__internalNodeMap.exists(org->son(i)))
                __FunctionGraph->__internalNodeMap[ org->son(i) ]->removeParent(origin, i);

        delete org;
        __FunctionGraph->__internalNodeMap.erase(origin);
        __FunctionGraph->__model.eraseNode(origin);

        if(__FunctionGraph->root() == origin )
            this->setRootNode( destination );
    }



    // ============================================================================
    // Checks if a similar node does not already exists in the graph or
    // if it has the same child for every variable value
    // @warning : will deallocate by itslef sonsMap if a match exists
    // ============================================================================
    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    INLINE
    NodeId
    MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::_nodeRedundancyCheck( const DiscreteVariable* var, NodeId* sonsIds ){

      NodeId newNode = sonsIds[0];

      if( __isRedundant( var, sonsIds ) ){
        DEALLOCATE( sonsIds, sizeof(NodeId)*var->domainSize() );
      } else {
        newNode = __checkIsomorphism( var, sonsIds );
        if ( newNode == 0 ) {
          newNode = _addInternalNode( var, sonsIds);
        } else {
          DEALLOCATE( sonsIds, sizeof(NodeId)*var->domainSize() );
        }
      }

      return newNode;
    }



    // ============================================================================
    // Checks if a similar node does not already exists in the graph
    // (meaning for every value assume by the associated variable, these two nodes
    // have the same children)
    // ============================================================================
    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    INLINE
    NodeId MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::__checkIsomorphism ( const DiscreteVariable* var,
                                                                         NodeId* sons){

      const InternalNode* nody = nullptr;
      Idx i = 0;

      // ************************************************************************
      // Check abscence of identical node
      Link<NodeId>* currentElem = __FunctionGraph->__var2NodeIdMap[var]->list();
      while( currentElem != nullptr ){

        nody = __FunctionGraph->__internalNodeMap[currentElem->element()];

        // Check on the other sons
        i = 0;
        while( i < var->domainSize() && sons[i] == nody->son(i))
            ++i;
        if(i == var->domainSize())
          return currentElem->element();

        currentElem = currentElem->nextLink();
      }
      return 0;
    }



    // ============================================================================
    // Checks if node has the same child for every variable value
    // @warning WON'T deallocate sons
    // ============================================================================
    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    INLINE
    bool MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::__isRedundant ( const DiscreteVariable* var,
                                                                  NodeId* sons){
        for( Idx m = 1; m < var->domainSize(); m++ )
            if( sons[m] != sons[0] )
                return false;
        return true;
    }

    // ============================================================================
    // Ensures that every isomorphic subgraphs are merged together.
    // ============================================================================
    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    INLINE
    void
    MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::_reduce( ){
      Link<NodeId>* currentNodeId = nullptr;
      Link<NodeId>* nextNodeId = nullptr;
      InternalNode* currentNode = nullptr;
      bool theSame = true;
      Idx currentInd;

      for( SequenceIterator<const DiscreteVariable*> varIter = __FunctionGraph->variablesSequence().rbegin();
             varIter != __FunctionGraph->variablesSequence().rend(); --varIter ){

        currentNodeId = __FunctionGraph->__var2NodeIdMap[*varIter]->list();

        while( currentNodeId != nullptr ){

          nextNodeId = currentNodeId->nextLink();
          currentNode = __FunctionGraph->__internalNodeMap[currentNodeId->element()];

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
            _migrateNode(currentNodeId->element(), currentNode->son(0));
            __FunctionGraph->__var2NodeIdMap[*varIter]->searchAndRemoveLink( currentNodeId->element() );
            currentNodeId = nextNodeId;
            continue;
          }


          // ====================================================================================
          // Second isomorphism to handle is the one where two nodes have same variable and same children
          if( nextNodeId ){
            Link<NodeId>* anotherNodeId = currentNodeId->nextLink();
            InternalNode* anotherNode = nullptr;
            Idx modality = 0;
            while( anotherNodeId->nextLink() != nullptr ){

              nextNodeId = anotherNodeId->nextLink();
              anotherNode = __FunctionGraph->__internalNodeMap[ anotherNodeId->element() ];

              // Check on the other sons
              for(modality = 0; modality < (*varIter)->domainSize(); ++modality ) {
                if( anotherNode->son(modality) != currentNode->son(modality) )
                  break;
                if( modality == (*varIter)->domainSize() - 1){
                  _migrateNode( anotherNodeId->element(), currentNodeId->element() );
                  __FunctionGraph->__var2NodeIdMap[*varIter]->searchAndRemoveLink( anotherNodeId->element() );
                }
              }

              anotherNodeId = nextNodeId;
            }
          }
          currentNodeId = currentNodeId->nextLink();
        }
      }
    }


    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
    INLINE
    void
    MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::clean( ){
      Sequence< const DiscreteVariable* > oldSequence(__FunctionGraph->variablesSequence());
      for ( SequenceIterator< const DiscreteVariable* > varIter = oldSequence.begin(); varIter != oldSequence.end(); ++varIter )
        if( ! __FunctionGraph->varNodeListe( *varIter )->list() )
          __FunctionGraph->erase(**varIter);
    }

} // namespace gum




// ============================================================================
// Inserts a new non terminal node in graph.
// NodeId of this node is generated automatically.
//
// @return the id of the added non terminal node.
// ============================================================================
//    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
//    INLINE
//    NodeId MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::addInternalNode ( NodeId nid ){


//        // Getting the structure for this internal node
//        typename MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >::InternalNode* newNodeStruct =
//                new typename MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >::InternalNode();

//        // Getting a new id for this node
//        if( nid == 0 )
//          nid = __FunctionGraph->__model.addNode();

//        if( !__FunctionGraph->__internalNodeMap.exists(nid) ){
//           __FunctionGraph->__internalNodeMap.insert( nid, newNodeStruct );
//        }else{
//           __FunctionGraph->__internalNodeMap[nid] = newNodeStruct;
//        }

//        return nid;
//    }




// *******************************************************************************************
// Verification
// First, we check if variable order has been specified
//      if ( __FunctionGraph->variablesSequence().size() == 0 ){
//        GUM_ERROR ( OperationNotAllowed, "MultiDim has no variable" );
//      }else{
//      // if so we check if var is in the order or not
//        if ( !__FunctionGraph->variablesSequence().exists( var ) ){
//          GUM_ERROR ( OperationNotAllowed, "Variable " << var->name() << " is not in the MultiDim" );
//        }
//      }

//      NodeId rnid = addInternalNode(nid);
//      __FunctionGraph->__internalNodeMap[rnid]->setNodeVar(var);
//      MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >::_addElemToNICL( &(__FunctionGraph->__var2NodeIdMap[var]), rnid );


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
//    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
//    void
//    MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::insertArc ( NodeId from, NodeId to, Idx modality ){

//        if(!__FunctionGraph->__model.exists(from)){
//          GUM_ERROR ( NotFound, " Origin node " <<  from << " does not exist." )
//        }

//        if ( __FunctionGraph->__valueMap.existsFirst ( from ) ) {
//          GUM_ERROR ( InvalidNode, " Origin node " <<  from << " is a terminal Node. No arcs can start from a terminal node" );
//        }

//        if(__FunctionGraph->__internalNodeMap[ from ]->nbSons() <= modality){
//          GUM_ERROR ( InvalidArgument, " modality " << modality << " is too high for variable " << __FunctionGraph->__internalNodeMap[ from ]->nodeVar()->name() << "." )
//        }

//        if(!__FunctionGraph->__model.exists(to)){
//          GUM_ERROR ( NotFound, " Destination node " <<  to << " does not exist." )
//        }

//        if ( !__FunctionGraph->__valueMap.existsFirst ( to ) ) {
//          if ( __FunctionGraph->variablesSequence().pos ( __FunctionGraph->__internalNodeMap[ from ]->nodeVar() )
//               >= __FunctionGraph->variablesSequence().pos ( __FunctionGraph->__internalNodeMap[ to ]->nodeVar() ) ) {
//            GUM_ERROR ( OperationNotAllowed, " This arc does not respect the variable order property. Variable " <<  __FunctionGraph->__internalNodeMap[ from ]->nodeVar()->name()
//                << " tied to node " << from << " is after Variable " << __FunctionGraph->__internalNodeMap[ to ]->nodeVar()->name() << " tied to node "
//                << to << " in variable order." );
//          }
//        }

//        for ( Idx i = 0; i < __FunctionGraph->__internalNodeMap[ from ]->nbSons(); i++ )
//          if ( __FunctionGraph->__internalNodeMap[ from ]->son(i) == to && i == modality ) {
//            GUM_ERROR ( DuplicateElement, " A same (meaning with same value " <<  modality << " ) arc linking those two nodes " << from << " -> " << to << " already exist." );
//            break;
//          }

//        __FunctionGraph->__internalNodeMap[ from ]->setSon(modality, to);
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
//    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
//    void
//    MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::eraseArc ( NodeId from, NodeId to ){

//      if(!__FunctionGraph->__model.exists(from)){
//        GUM_ERROR ( NotFound, " Origin node " <<  from << " does not exist." )
//      }

//      if(!__FunctionGraph->__model.exists(to)){
//        GUM_ERROR ( NotFound, " Destination node " <<  to << " does not exist." )
//      }

//      if ( __FunctionGraph->__valueMap.existsFirst ( from ) ) {
//        GUM_ERROR ( InvalidNode, " Origin node " <<  from << " is a terminal Node. No arcs can start from a terminal node" );
//      } else if ( !__FunctionGraph->__valueMap.existsFirst ( to ) ) {
//        if ( __FunctionGraph->variablesSequence().pos ( __FunctionGraph->__internalNodeMap[ from ]->nodeVar() )
//             >= __FunctionGraph->variablesSequence().pos ( __FunctionGraph->__internalNodeMap[ to ]->nodeVar() ) ) {
//            GUM_ERROR ( InvalidArc, " This(those) arc(s) " << from << " - " << to <<" cannot exist since it(they) violate(s) variable order." );
//        }
//      }

//      for(Idx i = 0; i < __FunctionGraph->__internalNodeMap[from]->nbSons(); i++)
//        if( __FunctionGraph->__internalNodeMap[from]->son(i) == to )
//            __FunctionGraph->__internalNodeMap[from]->setSon(i, 0);
//    }

// ============================================================================
// Erases an arc in the DD
//
// @param from and
// @param to as NodeId
// @param modality the modality corresponding to the to delete arc
// @throw InvalidArc If arc does not exist
// ============================================================================
//    template<typename GUM_SCALAR, template <class> class TerminalNodePolicy>
//    void
//    MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::eraseSpecificArc ( NodeId from, NodeId to, Idx modality ){

//      if(!__FunctionGraph->__model.exists(from)){
//        GUM_ERROR ( NotFound, " Origin node " <<  from << " does not exist." )
//      }

//      if(!__FunctionGraph->__model.exists(to)){
//        GUM_ERROR ( NotFound, " Destination node " <<  to << " does not exist." )
//      }

//      if(__FunctionGraph->__internalNodeMap[ from ]->nbSons() <= modality){
//        GUM_ERROR ( gum::InvalidArgument, " modality " << modality << " is too high for variable " << __FunctionGraph->__internalNodeMap[ from ]->nodeVar()->name() << "." )
//      }

//      if ( __FunctionGraph->__valueMap.existsFirst ( from ) ) {
//        GUM_ERROR ( InvalidNode, " Origin node " <<  from << " is a terminal Node. No arcs can start from a terminal node" );
//      } else if ( !__FunctionGraph->__valueMap.existsFirst ( to ) ) {
//        // GUM_TRACE( "From : " << _varMap[from]->toString() << " - To : " << _varMap[ to ]->toString() << std::endl );
//        if ( __FunctionGraph->variablesSequence().pos ( __FunctionGraph->__internalNodeMap[ from ]->nodeVar() )
//             >= __FunctionGraph->variablesSequence().pos ( __FunctionGraph->__internalNodeMap[ to ]->nodeVar() ) ) {
//          GUM_ERROR ( OperationNotAllowed, " This arc does not respect the variable order property. Variable " <<  __FunctionGraph->__internalNodeMap[ from ]->nodeVar()->name()
//              << " tied to node " << from << " is after Variable " << __FunctionGraph->__internalNodeMap[ to ]->nodeVar()->name() << " tied to node "
//              << to << " in variable order." );
//        }
//      }

//      __FunctionGraph->__internalNodeMap[from]->setSon(modality, 0);
//    }
