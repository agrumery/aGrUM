
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
 * @brief Class allowing creation of Algebraic Decision Diagram
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_DECISION_DIAGRAM_FACTORY_BASE_H
#define GUM_MULTI_DIM_DECISION_DIAGRAM_FACTORY_BASE_H
// ============================================================================
#include <agrum/core/hashTable.h>
#include <agrum/core/bijection.h>
#include <agrum/core/approximationPolicy/approximationPolicy.h>
// ============================================================================
#include <agrum/multidim/multiDimDecisionDiagramBase.h>
// ============================================================================
#include <agrum/graphs/DAG.h>
// ============================================================================

namespace gum {

  template<typename T_DATA >
  class MultiDimDecisionDiagramBase;

  /**
   * @class MultiDimDecisionDiagramFactoryBase multiDimDecisionDiagramFactoryBase.h <agrum/multidim/multiDimDecisionDiagramFactoryBase.h>
   * @brief Class implementingting an algebraic decision diagram factory
   * Can work either with checks on variables to ensure order property is respected in diagram or without (NoVariableCheckMode)
   * @ingroup multidim_group
   *
   */
  template<typename T_DATA>

  class MultiDimDecisionDiagramFactoryBase : public virtual ApproximationPolicy<T_DATA> {

    public :

      // ===========================================================================
      /// @name Constructors, Destructors.
      // ===========================================================================
      /// @{

		  /**
		   * Default constructor.
		   */
		  MultiDimDecisionDiagramFactoryBase();

		  /**
		   * Destructor.
		   * @warnings : Since getMultiDimDecisionDiagram does a copy of DecisionDiagram
		   * deleting this factory will not destroy properties on node of any instantiated DecisionDiagram.
		   */
		  virtual ~MultiDimDecisionDiagramFactoryBase();

      /// @}

      // ===========================================================================
      /// @name Graph manipulation methods.
      // ===========================================================================
      /// @{

		  /**
		   * Returns the sequence of variables on which is based the diagram construction
		   */
		  const Sequence< const DiscreteVariable* >& variablesSequence(  );

		  /**
		   * To be done before any insertion of node linked to variable.
		   * Specifies the order between variable in the diagram
		   *
		   * @param a sequence containing the variable (wich will be the referent )
		   */
		  void setVariablesSequence( Sequence< const DiscreteVariable* > s );

		  // ===========================================================================
		  /// @name Nodes manipulation methods.
		  // ===========================================================================
		  /// @{

			  /**
			   * Adds a variable and its associate non terminal node. The id of the new
			   * variable is automatically generated.
			   *
			   * @param variable The variable added by copy.
			   * @throw OperationNotAllowed if no sequence of variable have yet been specified
			   * @return the id of the added variable.
			   */
			  NodeId addNonTerminalNode( const DiscreteVariable* var );
			  NodeId unsafeAddNonTerminalNode( const DiscreteVariable* var );


			  /**
			   * Adds a non-terminal node in the diagram linked to given variable.
			   * Inserts also arc between that node and mentionned node in the given hashtable
			   * @throw OperationNotAllowed if no sequence of variable have yet been specified
			   * Returns the id of that node
			   */
			  NodeId addNonTerminalNodeWithArcs( const DiscreteVariable* var, const HashTable< Idx, NodeId >& nodeArcMap, NodeId* defaultArcTo = NULL );
			  NodeId unsafeAddNonTerminalNodeWithArcs( const DiscreteVariable* var, const HashTable< Idx, NodeId >& nodeArcMap, NodeId* defaultArcTo = NULL );
			  std::pair<bool, NodeId> checkredundancy( const DiscreteVariable* var, const HashTable< Idx, NodeId >& nodeArcMap, NodeId* defaultArcTo = NULL );


			  /**
			   * Adds a value and it's associate terminal node. The id of the new
			   * variable is automatically generated.
			   *
			   * @param value The value added by copy.
			   * @return the id of the added variable.
			   *
			   * If a terminal node with such value already exists, its value will be return instead.
			   */
			  NodeId addTerminalNode( const T_DATA& value );


			  /**
			   * Erases a node from the diagram
			   * If no variable matches the id, then nothing is done.
			   *
			   * @param id The id of the variable to erase.
			   * @throw NotFound if node isn't in diagram
			   */
			  void eraseNode( NodeId id );

		  /// @}

		  // ===========================================================================
		  /// @name Arcs manipulation methods.
		  // ===========================================================================
		  /// @{


			  /**
			   * Adds an arc in the DD
			   *
			   * @param from and
			   * @param to as NodeId
			   * @param value the value of the arc
			   * @throw NotFound If from and/or tail are not in the DD.
			   * @throw InvalidNode if head is a terminal node
			   * @throw OperationNotAllowed if arc doesn't respect variable order property
			   * @throw DuplicateElement if another arc linking those nodes already exists
			   */
			  void insertArc( NodeId from, NodeId to, Idx value );
			  void unsafeInsertArc( NodeId from, NodeId to, Idx value );


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
			  void insertDefaultArc( NodeId from, NodeId to );
			  void unsafeInsertDefaultArc( NodeId from, NodeId to );


			  /**
			   * Erases an arc in the DD
			   *
			   * @param from and
			   * @param to as NodeId
			   * @throw InvalidArc If arc does not exist
			   * @warning due to the possibility that several arc with different value have the same from and to,
			   * if several arcs have different value but same parent and child, this method will erase all of them .
			   * If you want to erase a specific one, use eraseArcWithValue
			   */
			  void eraseArc( NodeId from, NodeId to );


			  /**
			   * Erases an arc in the DD
			   *
			   * @param from and
			   * @param to as NodeId
			   * @throw InvalidArc If arc does not exist
			   */
			  void eraseArcWithValue( NodeId from, NodeId to, Idx value );

		/// @}

      /// @}
							
	  /**
	   * Inserts sub decision diagram at specified node on specified modality
       * It does a copy of given decision diagram.
       * @return the id of the sub diagram root node in the bigger one
       * @throw NotFound if node does not yet exist
	   * @throw OperationNotAllowed if node var has already this modality used
	   */
	   NodeId insertSubDecisionDiagram( const MultiDimDecisionDiagramBase<T_DATA>* subdd, NodeId parent, Idx parentModality );
							
	  /**
	   * Inserts sub decision diagram at specified node on default modality
       * It does a copy of given decision diagram.
       * @return the id of the sub diagram root node in the bigger one
       * @throw NotFound if node does not yet exist
	   * @throw OperationNotAllowed if node var has already this modality used
	   */
	   NodeId insertSubDecisionDiagram( const MultiDimDecisionDiagramBase<T_DATA>* subdd, NodeId parent );
							
	  /**
	   * Inserts sub decision diagram
       * It does a copy of given decision diagram.
       * @return the id of the sub diagram root node in the bigger one
	   */
	   NodeId insertSubDecisionDiagram( const MultiDimDecisionDiagramBase<T_DATA>* subdd );

      /**
       * Returns the multidimDecisionDiagram made
       */
      virtual MultiDimDecisionDiagramBase<T_DATA>* getMultiDimDecisionDiagram( bool fillWithDefaultArc = true, T_DATA defaultValue = 0 ) = 0;

      /**
       * Displays the current DecisionDiagram structures
       */
      void showProperties();

      /**
       * Resets the factory
       */
      void clear();
      
      /**
       * Puts the factory in a mode where no verification are made on variable
       * ( including arc respecting a variable order )
       * Helpful when diagram is build on stream
       */
      void putOnNoVariableCheckMode();
      
      /**
       * Puts the factory out of no check on variable mode
       */
      void putOffNoVariableCheckMode();

    protected :
      
      /**
       * Finds an order of variable compatible to the diagram
       */
      Sequence< const DiscreteVariable* > _findVariableOrder();
							
	  /**
	   * does the main stuff about inserting a decision diagram in this one
	   */
	   NodeId _insertSubDecisionDiagram( const MultiDimDecisionDiagramBase<T_DATA>* subdd );
    
	  /// Boolean precising if we are in check on variable mode or not
	  bool _noVariableCheckMode;

      /// The order of variables in the diagram
      Sequence< const DiscreteVariable* > _varsSeq;

      /// The algebraic decision diagram model
      DAG _model;

      /// Mapping between id and variable
      typename Property< const DiscreteVariable* >::onNodes _varMap;

      /// Mapping between terminal nodes and their values
      Bijection< NodeId, T_DATA > _valueMap;

      /// Mapping between variable's values and associated node
      typename Property< HashTable< Idx, NodeId >* >::onNodes _arcMap;

      /// Mapping between variable's values and associated node
      typename Property< NodeId >::onNodes _defaultArcMap;

      /// Mapping between variable and nodes tied to this var
      HashTable< const DiscreteVariable*, List<NodeId>* > _var2NodeIdMap;

  };

} /* namespace gum */

// ============================================================================
#include <agrum/multidim/multiDimDecisionDiagramFactoryBase.tcc>
// ============================================================================
#endif /* GUM_MULTI_DIM_DECISION_DIAGRAM_FACTORY_BASE_H */
// ============================================================================

// kate: indent-mode cstyle; space-indent on; indent-width 0;  replace-tabs on;
