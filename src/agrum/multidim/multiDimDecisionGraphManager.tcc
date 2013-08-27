
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
    // You have to call MultiDimDecisionGraph::manager() to get the instance
    // of MultiDimDecisionGraphManager bound to your decision graph.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::MultiDimDecisionGraphManager( MultiDimDecisionGraph<GUM_SCALAR>* mddg ) :
        __model( 500, true ){
      GUM_CONSTRUCTOR ( MultiDimDecisionGraphManager ) ;
      __decisionGraph = mddg;
      // Pop up a first node so that id 0 is unavailable
      __model.insertNode();
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
    const NodeId& MultiDimDecisionGraphManager< GUM_SCALAR>::addNonTerminalNode ( const DiscreteVariable* var ){

      // *******************************************************************************************
      // Verification
      // First, we check if variable order has been specified
      if ( __decisionGraph->variablesSequence().size() == 0 ){
        GUM_ERROR ( OperationNotAllowed, "MultiDim has no variable" );
      }
      else
      // if so we check if var is in the order or not
        if ( !__decisionGraph->variablesSequence().exists( var ) ){
          GUM_ERROR ( OperationNotAllowed, "Variable " << var->name() << " is not in the MultiDim" );
        }

      // *********************************************************************************************      
      // Insertion du noeud
      NodeId newNode = unsafeAddNonTerminalNode( var );

      // *********************************************************************************************
      // Initialisation du vecteur fils.
      NodeId* sonVector = static_cast<NodeId*>(MultiDimDecisionGraph::soa.allocate(var->domainSize()*sizeof(NodeId)));
      for( gum::Idx i = 0; i < var->domainSize(); ++i)
        sonVector[i] = 0;
      __decisionGraph->__internalNodeMap[newNode]->nodeSons = sonVector;

      return newNode;
    }


    // ============================================================================
    //
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    const NodeId& MultiDimDecisionGraphManager< GUM_SCALAR>::addNonTerminalNode ( const DiscreteVariable* var,
                                                                                        NodeId* sons, NodeId& nid){

      // Getting a new id for this node
      if( nid = 0 )
          nid = __model.insertNode();

      // Getting the structure for this internal node
      MultiDimDecisionGraph< GUM_SCALAR>::InternalNode* newNodeStruct = __decisionGraph->_createInternalNode(var);

      // Initializing it
      newNodeStruct->nodeVar = var;
      newNodeStruct->nodeSons = sons;
//      newNodeStruct->nodeDefaultSon = defaultSon;

      // Adding this node to the list of node bound to variable var
      __decisionGraph->__var2NodeIdMap[var]->insert ( newNode );

      // Returning the result.
      return newNode;
    }


    // ============================================================================
    //
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    const NodeId&
    MultiDimDecisionGraphManager< GUM_SCALAR>::checkRedundancy ( const DiscreteVariable* var,
                                                                 NodeId* sons,
                                                                 const NodeId& defaultSon){

      Idx i = 0;
      const MultiDimDecisionGraph::InternalNode* nody = nullptr;

      // ************************************************************************
      // Check abscence of identical node
      for(__var2NodeIdIter = __decisionGraph->__var2NodeIdMap[ var ]->begin();
          __var2NodeIdIter != __decisionGraph->__var2NodeIdMap[ var ]->end();
          ++__var2NodeIdIter){

        nody = __decisionGraph->__internalNodeMap[*__var2NodeIdIter];
        // Check on default son node
        if(defaultSon != nody->nodeDefaultSon)
          continue;

        // Check on the other sons
        for(i = 0; i < var->domainSize(); ++i ) {
          if( sons[i] != nody->nodeSons[i] &&
              ( sons[i] != 0 || defaultSon != nody->nodeSons[i] ) &&
              ( nody->nodeSons[i] != 0 || sons[i] != nody->nodeDefaultSon ))
            break;
          if(i == var->domainSize() - 1){
            delete sons;
            return *__var2NodeIdIter;
          }
        }
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
    const NodeId&
    MultiDimDecisionGraphManager< GUM_SCALAR>::addTerminalNode ( const GUM_SCALAR& value ){

      if( __decisionGraph->__valueMap.existsSecond( value ) )
        return __decisionGraph->__valueMap.first( value );

      NodeId node = __model.insertNode();
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

      if( !__model.exists ( n )){
        GUM_ERROR ( NotFound, "Node " <<  n << " does not exist in diagram." );
      }

      for(HashTable<const DiscreteVariable*, List<NodeId>*>::const_iterator iterVar = __decisionGraph->__var2NodeIdMap.begin();
          iterVar != __decisionGraph->__var2NodeIdMap.end(); ++iterVar){

        if( !__decisionGraph->isTerminalNode(n) &&
            __decisionGraph->variablesSequence().pos(iterVar.key()) > __decisionGraph->variablesSequence().pos(__decisionGraph->__variableMap[n]))
          break;

        for(List<NodeId>::const_iterator iterNode = (*iterVar)->begin(); iterNode != (iterVar)->end(); ++ iterNode){
          for( Idx iterSon = 0; iterSon < iterVar.key()->domainSize(); ++iterSon)
            if( __decisionGraph->__sonsMap[*iterNode][iterSon] == n )
              __decisionGraph->__sonsMap[*iterNode][iterSon] = 0;

          if( __decisionGraph->__defaultSonMap.exists( *iterNode ) && _defaultArcMap[*iterNode] == n )
            __decisionGraph->__defaultSonMap.erase( *iterNode );
        }
      }

      if ( __decisionGraph->isTerminalNode(n) )
        __decisionGraph->__valueMap.eraseFirst(n);
      else {
        __decisionGraph->__variableMap.erase(n);
        delete __decisionGraph->__sonsMap[n];
        __decisionGraph->__sonsMap.erase (n);
        if(__decisionGraph->__defaultSonMap.exists(n))
          __decisionGraph->__defaultSonMap.erase(n);
      }

      __model.eraseNode(n);

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

        if(!__model.exists(from)){
          GUM_ERROR ( NotFound, " Origin node " <<  from << " does not exist." )
        }

        if(!_model.exists(to)){
          GUM_ERROR ( NotFound, " Destination node " <<  to << " does not exist." )
        }

        if ( _valueMap.existsFirst ( from ) ) {
          GUM_ERROR ( InvalidNode, " Origin node " <<  from << " is a terminal Node. No arcs can start from a terminal node" );
        } else if ( !_noVariableCheckMode && !_valueMap.existsFirst ( to ) ) {
          // GUM_TRACE( "From : " << _varMap[from]->toString() << " - To : " << _varMap[ to ]->toString() << std::endl );
          if ( _varsSeq.pos ( _varMap[ from ] ) >= _varsSeq.pos ( _varMap[ to ] ) ) {
            GUM_ERROR ( OperationNotAllowed, " This arc does not respect the variable order property. Variable " <<  _varMap[from]->name() << " tied to node " << from <<
                        " is after Variable " << _varMap[to]->name() << " tied to node " << to << " in variable order." );
          }
        }

        for ( std::vector<NodeId>::iterator iter = _arcMap[from]->begin(); iter != _arcMap[from]->end(); ++iter )
          if ( *iter == to && ( gum::Idx ) std::distance ( _arcMap[from]->begin(), iter ) == value ) {
            GUM_ERROR ( DuplicateElement, " A same (meaning with same value " <<  value << " ) arc linking those two nodes " << from << " -> " << to << " already exist." );
            break;
          }

        if ( _defaultArcMap.exists ( from ) && _defaultArcMap[from] == to )
          return;

        _model.insertArc ( from, to );

        ( *_arcMap[from] ) [value] =  to;
        ( *_varUsedModalitiesMap[ _varMap[from] ] ) [value]++;
    }

    // ============================================================================
    /**
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
    template<typename GUM_SCALAR>
    INLINE
    void
    MultiDimDecisionGraphManager< GUM_SCALAR>::insertDefaultArc ( NodeId from, NodeId to ){
        return 0;
      }
    template<typename GUM_SCALAR>
    INLINE
    void
    MultiDimDecisionGraphManager< GUM_SCALAR>::unsafeInsertDefaultArc ( NodeId from, NodeId to ){
        return 0;
      }
    ///@}

    // ============================================================================
    /**
     * Erases arcs in the DD
     *
     * @param from and
     * @param to as NodeId
     * @throw InvalidArc If arc does not exist
     * @warning due to the possibility that several arc with different value have the same from and to,
     * if several arcs have different value but same parent and child, this method will erase all of them .
     * If you want to erase a specific one, use eraseSpecificArc
     */
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void
    MultiDimDecisionGraphManager< GUM_SCALAR>::eraseArc ( NodeId from, NodeId to ){
        return 0;
      }

    // ============================================================================
    /**
     * Erases an arc in the DD
     *
     * @param from and
     * @param to as NodeId
     * @param modality the modality corresponding to the to delete arc
     * @throw InvalidArc If arc does not exist
     */
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void
    MultiDimDecisionGraphManager< GUM_SCALAR>::eraseSpecificArc ( NodeId from, NodeId to, Idx modality ){
        return 0;
      }

  /// @}

  // ############################################################################
  /// @name Arcs manipulation methods.
  // ############################################################################
  /// @{

    // ============================================================================
    /**
     * For each non terminal node, this function first checks if node has a son
     * for each value of associated variable.
     * If not, function checks if defaultSon is already specified.
     * If not, function affect defaultSon to the terminal node associated with given value.
     */
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void
    MultiDimDecisionGraphManager< GUM_SCALAR>::fillWithDefaultArc( GUM_SCALAR defaultValue ){
        return 0;
      }

    // ============================================================================
    /// Ensures that every isomorphic subgraphs are merged together.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void
    MultiDimDecisionGraphManager< GUM_SCALAR>::reduce(){
        return 0;
      }

    // ============================================================================
    /// Resets the MultiDimDecisionGraph, clearing all nodes, values, variables.
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void
    MultiDimDecisionGraphManager< GUM_SCALAR>::clear(){
        return 0;
      }

} // namespace gum
