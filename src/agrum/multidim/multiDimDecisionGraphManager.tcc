
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
#include <agrum/multidim/multiDimDecisionGraph.h>

namespace gum {
  // ############################################################################
  /// @name Constructors / Destructors
  // ############################################################################
  /// @{
    // ============================================================================
    /**
     * Default constructor.
     * Private.
     * You have to call MultiDimDecisionGraph::getManager() to get the instance
     * of MultiDimDecisionGraphManager bound to your decision graph.
     */
    // ============================================================================
    template< GUM_SCALAR >
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::MultiDimDecisionGraphManager( MultiDimDecisionGraph<GUM_SCALAR>* mddg ) :
        __model( 500, true ){
      GUM_CONSTRUCTOR ( MultiDimDecisionGraphManager ) ;
      __decisionGraph = mddg;
    }

    // ============================================================================
    /// Destructor. Don't worry, it will be call on the destruction of your
    /// MultiDimDecisionGraph.
    // ============================================================================
    template< GUM_SCALAR >
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::~MultiDimDecisionGraphManager(){
      GUM_DESTRUCTOR ( MultiDimDecisionDiagramFactoryBase );
    }

  /// @}

  // ############################################################################
  /// @name Nodes manipulation methods.
  // ############################################################################
  /// @{
    // ============================================================================
    /// Sets root node of decision diagram
    // ============================================================================
    template< GUM_SCALAR >
    INLINE
    void MultiDimDecisionGraphManager< GUM_SCALAR>::setRootNode ( const NodeId& root ){
      __decisionGraph->__root = root;
    }

    // ============================================================================
    /**
     * Inserts a new non terminal node in graph.
     * NodeId of this node is generated automatically.
     *
     * @param var Associated variable
     * @throw OperationNotAllowed if MultiDimDecisionGraph has no variables yet.
     * @throw OperationNotAllowed if MultiDimDecisionGraph has no this variable yet.
     * @return the id of the added non terminal node.
     */
    // ============================================================================
    ///@{
    template< GUM_SCALAR >
    INLINE
    const NodeId& MultiDimDecisionGraphManager< GUM_SCALAR>::addNonTerminalNode ( const DiscreteVariable* var ){

      // *******************************************************************************************
      // Verification
      // First, we check if variable order has been specified
      if ( _varsSeq.size() == 0 )
        GUM_ERROR ( OperationNotAllowed, "You must first specify the order of variable" );
      else
      // if so we check if var is in the order or not
        if ( !_varsSeq.exists ( var ) )
          GUM_ERROR ( OperationNotAllowed, "Variable " << var->name() << " is not in the specify order" );
      // *********************************************************************************************

      return unsafeAddNonTerminalNode ( var );
    }

    template< GUM_SCALAR >
    INLINE
    const NodeId& MultiDimDecisionGraphManager< GUM_SCALAR>::unsafeAddNonTerminalNode ( const DiscreteVariable* var ){


        // *******************************************************************************************
        // Part were we had the variable's new node
        NodeId node = _model.insertNode();

        // We mention that new node to the list of node bound to that variable
        _varMap.insert ( node, var );

        _arcMap.insert ( node, new std::vector< NodeId > ( var->domainSize(), 0 ) );

        // **********************************************************************************************
        // Addition of the node to the list of tied to given variable

        // If list hasn't be created yet, we create it
        if ( !_var2NodeIdMap.exists ( var ) ) {
          _var2NodeIdMap.insert ( var, new List<NodeId>() );
          _varUsedModalitiesMap.insert ( var, new std::vector<Idx> ( var->domainSize(), 0 ) );
        }

        // And finally we add the node to that list
        _var2NodeIdMap[ var ]->insert ( node );

        //*************************************************************************************************

        return node;
      }
    ///@}

    // ============================================================================
    /**
     * Adds a non-terminal node in the diagram.
     * NodeId of this node is generated automatically.
     *
     * @param var the associated variable
     * @param sons table containing the nodeids of its sons (There are as many sons
     * as the number of value assumed by var)
     * @param defaultSon the default son (optional)
     * @throw OperationNotAllowed if MultiDimDecisionGraph has no variable yet.
     * @return the id of that node
     */
    // ============================================================================
    ///@{
    template< GUM_SCALAR >
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::
    const NodeId& addNonTerminalNodeWithArcs ( const DiscreteVariable* var,
                                             const NodeId* sons,
                                             const NodeId& defaultSon = 0 );
    template< GUM_SCALAR >
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::
    const NodeId& unsafeAddNonTerminalNodeWithArcs ( const DiscreteVariable* var,
                                                   const NodeId* sons,
                                                   const NodeId& defaultSon = 0 );
    ///@}

    // ============================================================================
    /**
     * Checks if a node with same variable, same sons and same default son does not
     * already exists in diagram
     *
     * @param var the associated variable
     * @param sons table containing the nodeids of its sons (There are as many sons
     * as the number of value assumed by var)
     * @param defaultSon the default son (optional)
     * @return a pair. First element is boolean indicating if such duplicate exists
     * already. Second element is the such node.
     */
    // ============================================================================
    template< GUM_SCALAR >
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::
    const std::pair<bool, NodeId>& checkRedundancy ( const DiscreteVariable* var,
                                                   const NodeId* sons,
                                                   const NodeId& defaultSon = 0 );

    // ============================================================================
    /**
     * Adds a value to the MultiDimDecisionGraph.
     * This will create a terminal node, which of id is returned.
     * If a terminal node with such value already exists,
     * its id will be return instead.
     *
     * @param value The value added by copy.
     * @return the id of the terminal node hence created.
     *
     */
    // ============================================================================
    template< GUM_SCALAR >
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::
    const NodeId& addTerminalNode ( const GUM_SCALAR& value );

    // ============================================================================
    /**
     * Erases a node from the diagram.
     *
     * @param id The id of the variable to erase.
     * @throw NotFound if node isn't in diagram
     */
    // ============================================================================
    template< GUM_SCALAR >
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::
    void eraseNode ( NodeId id );

  /// @}

  // ############################################################################
  /// @name Arcs manipulation methods.
  // ############################################################################
  /// @{

    // ============================================================================
    /**
     * Adds an arc in the DD
     *
     * @param from and
     * @param to as NodeId
     * @param modality the modality on which arc is bind
     * @throw NotFound If from and/or tail are not in the DD.
     * @throw InvalidNode if head is a terminal node
     * @throw OperationNotAllowed if arc doesn't respect variable order property
     * @throw DuplicateElement if another arc linking those nodes already exists
     */
    // ============================================================================
    ///@{
    template< GUM_SCALAR >
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::
    void insertArc ( NodeId from, NodeId to, Idx modality );
    template< GUM_SCALAR >
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::
    void unsafeInsertArc ( NodeId from, NodeId to, Idx modality );
    ///@}

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
    template< GUM_SCALAR >
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::
    void insertDefaultArc ( NodeId from, NodeId to );
    template< GUM_SCALAR >
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::
    void unsafeInsertDefaultArc ( NodeId from, NodeId to );
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
    template< GUM_SCALAR >
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::
    void eraseArc ( NodeId from, NodeId to );

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
    template< GUM_SCALAR >
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::
    void eraseSpecificArc ( NodeId from, NodeId to, Idx modality );

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
    template< GUM_SCALAR >
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::
    void fillWithDefaultArc( GUM_SCALAR defaultValue = 0 );

    // ============================================================================
    /// Ensures that every isomorphic subgraphs are merged together.
    // ============================================================================
    template< GUM_SCALAR >
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::
    void reduce();

    // ============================================================================
    /// Resets the MultiDimDecisionGraph, clearing all nodes, values, variables.
    // ============================================================================
    template< GUM_SCALAR >
    INLINE
    MultiDimDecisionGraphManager< GUM_SCALAR>::
    void clear();

} // namespace gum
