
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
 * @brief Headers of gum::MultiDimDecisionGraphManager.
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_DECISION_GRAPH_MANAGER_H
#define GUM_MULTI_DIM_DECISION_GRAPH_MANAGER_H

// ============================================================================
#include <agrum/config.h>
// ============================================================================
#include <agrum/graphs/nodeGraphPart.h>
// ============================================================================
#include <agrum/multidim/multiDimDecisionGraph.h>
// ============================================================================

namespace gum{

  template<typename GUM_SCALAR>
  class MultiDimDecisionGraph;

  /**
   * @class MultiDimDecisionGraphManager multiDimDecisionGraphManager.h <agrum/multidim/multiDimDecisionGraphManager.h>
   *
   * @brief Class implementingting a decision graph manager
   * This class provides the methods to edit a Decision Graph.
   * Any modification on a MultiDimDecisionGraph graph is done via this class.
   *
   * At any time during the runtime, it exists only one instance of this class for a given MultiDimDecisionGraph.
   * To get such instance, use MultiDimDecisionGraph::getManager();
   * To do so :@code
       MultiDimDecisionGraph<GUM_SCALAR>* dg = new MultiDimDecisionGraph<GUM_SCALAR>();
       MultiDimDecisionGraphManager<GUM_SCALAR>* dgm = dg->manager();
       @endcode
   * This is the only way to get an instance of a MultiDimDecisionGraphManager since the constructor is private.
   *
   * @ingroup multidim_group
   */
  template<typename GUM_SCALAR>
  class MultiDimDecisionGraphManager
  {
      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{

        // ============================================================================
        /// This friend methods from is the only way to get an instance of a manager.
        /// See class description for more info.
        // ============================================================================
        friend MultiDimDecisionGraphManager< GUM_SCALAR >* MultiDimDecisionGraph<GUM_SCALAR>::manager();

        // ============================================================================
        /**
         * Default constructor.
         * Private.
         * You have to call MultiDimDecisionGraph::getManager() to get the instance
         * of MultiDimDecisionGraphManager bound to your decision graph.
         */
        // ============================================================================
        MultiDimDecisionGraphManager( MultiDimDecisionGraph<GUM_SCALAR>* master);

    public :
        // ============================================================================
        /// Destructor. Don't worry, it will be call on the destruction of your
        /// MultiDimDecisionGraph.
        // ============================================================================
        ~MultiDimDecisionGraphManager();

      /// @}

      // ############################################################################
      /// @name Nodes manipulation methods.
      // ############################################################################
      /// @{
        // ============================================================================
        /// Sets root node of decision diagram
        // ============================================================================
        void setRootNode ( const NodeId& root );


        // ============================================================================
        /**
         * Inserts a new non terminal node in graph.
         * NodeId of this node is generated automatically.
         *
         * @param var Associated variable
         * @return the id of the added non terminal node.
         */
        // ============================================================================
        NodeId addNonTerminalNode ( const DiscreteVariable* var );

        // ============================================================================
        /**
         * Inserts a new non terminal node in graph.
         * NodeId of this node is generated automatically.
         *
         * @param var Associated variable
         * @throw OperationNotAllowed if MultiDimDecisionGraph has no variable yet.
         * @return the id of the added non terminal node.
         */
        // ============================================================================
        NodeId addNonTerminalNode ( const DiscreteVariable* var, NodeId* sons  );

        // ============================================================================
        /**
         * Inserts a new non terminal node in graph.
         * NodeId of this node is generated automatically.
         *
         * @param var Associated variable
         * @throw OperationNotAllowed if MultiDimDecisionGraph has no variable yet.
         * @return the id of the added non terminal node.
         */
        // ============================================================================
        NodeId addNonTerminalNode ( const DiscreteVariable* var, NodeId nid  );

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
        NodeId addTerminalNode ( const GUM_SCALAR& value );

//        typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* node(NodeId nid){ return __decisionGraph->__internalNodeMap[nid]; }

        // ============================================================================
        /**
         * Erases a node from the diagram.
         *
         * @param id The id of the variable to erase.
         * @param replacingId offers the possibility to reroute any parent to the given node
         * @throw NotFound if node isn't in diagram
         */
        // ============================================================================
        void eraseNode ( NodeId id, NodeId replacingId = 0 );

      /// @}

      // ############################################################################
      /// @name Manipulation methods.
      // ############################################################################
      /// @{

        // ============================================================================
        /// Sets nodes son for given modality to designated son node
        // ============================================================================
        void setSon( const NodeId& node, const Idx& modality, const NodeId& sonNode );

        // ============================================================================
        /// Sets nodes son for given modality to designated son node
        // ============================================================================
        NodeId checkIsomorphism(const DiscreteVariable* var, NodeId* sons);

        // ============================================================================
        /// Sets nodes son for given modality to designated son node
        // ============================================================================
        bool isRedundant(const DiscreteVariable* var, NodeId* sons);

        // ============================================================================
        /// Ensures that every isomorphic subgraphs are merged together.
        // ============================================================================
        void reduce();

        // ============================================================================
        /// Changes var position in variable sequence
        // ============================================================================
        void moveTo( const DiscreteVariable* x, Idx desiredPos );

  protected :
        void _migrateNode(const NodeId&, const NodeId&);

  private :

        // ============================================================================
        /// Swap two adjacent variable.
        /// Order is important here.
        /// X must precede Y before the swap (at the end Y will then precede X).
        /// Not respecting this constraint leads to unattended behaviour.
        // ============================================================================
        void __adjacentSwap( const DiscreteVariable* x, const DiscreteVariable* y );

        // ============================================================================
        /// The multidimdecisiongraph supposed to be edited.
        // ============================================================================
        MultiDimDecisionGraph<GUM_SCALAR>* __decisionGraph;

  };
} // namespace gum

// ============================================================================
#include <agrum/multidim/multiDimDecisionGraphManager.tcc>
// ============================================================================
#endif // GUM_MULTI_DIM_DECISION_GRAPH_MANAGER_H
// ============================================================================



#ifdef WASTELAND
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
//        void insertArc ( NodeId from, NodeId to, Idx modality );

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
//        void eraseArc ( NodeId from, NodeId to );

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
//        void eraseSpecificArc ( NodeId from, NodeId to, Idx modality );

/// @}

#endif
