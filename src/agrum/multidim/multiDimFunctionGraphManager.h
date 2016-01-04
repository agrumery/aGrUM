
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
 * @brief Headers of gum::MultiDimFunctionGraphManager.
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_MANAGER_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_MANAGER_H

// ============================================================================
#include <agrum/config.h>
// ============================================================================
#include <agrum/graphs/nodeGraphPart.h>
// ============================================================================
#include <agrum/multidim/multiDimFunctionGraph.h>
#include <agrum/multidim/FunctionGraphUtilities/internalNode.h>
// ============================================================================

namespace gum {

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  class MultiDimFunctionGraph;

  /**
   * @class MultiDimFunctionGraphManager multiDimFunctionGraphManager.h
   <agrum/multidim/multiDimFunctionGraphManager.h>
   *
   * @brief Class implementingting a function graph manager
   * This class provides the methods to edit a Function Graph.
   * Any modification on a MultiDimFunctionGraph graph is done via this class.
   *
   * At any time during the runtime, it exists only one instance of this class
   for a given MultiDimFunctionGraph.
   * To get such instance, use MultiDimFunctionGraph::getManager();
   * To do so :@code
       MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy>* dg =
   MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy>::get*ANY*Instance();
       MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy>* dgm =
   dg->manager();
       @endcode
   * This is the only way to get an instance of a MultiDimFunctionGraphManager
   since the constructor is private.
   *
   * @ingroup multidim_group
   */
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  class MultiDimFunctionGraphManager {
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    // ============================================================================
    /// This friend methods from is the only way to get an instance of a
    /// manager.
    /// See class description for more info.
    // ============================================================================
    friend MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>*
    MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::manager();

    // ============================================================================
    /**
     * Default constructor.
     * Private.
     * You have to call MultiDimFunctionGraph::getManager() to get the instance
     * of MultiDimFunctionGraphManager bound to your function graph.
     */
    // ============================================================================
    protected:
    MultiDimFunctionGraphManager(
        MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* master );

    public:
    // ============================================================================
    /// Destructor. Don't worry, it will be call on the destruction of your
    /// MultiDimFunctionGraph.
    // ============================================================================
    virtual ~MultiDimFunctionGraphManager();

    /// @}

    // ############################################################################
    /// @name Nodes manipulation methods.
    // ############################################################################
    /// @{
    // ============================================================================
    /// Sets root node of decision diagram
    // ============================================================================
    void setRootNode( const NodeId& root );


    // ============================================================================
    /**
     * Inserts a new non terminal node in graph.
     * NodeId of this node is generated automatically.
     *
     * @param var Associated variable
     * @return the id of the added non terminal node.
     */
    // ============================================================================
    NodeId addInternalNode( const DiscreteVariable* var );

    // ============================================================================
    /**
     * Inserts a new non terminal node in graph.
     * NodeId of this node is generated automatically.
     *
     * @param var Associated variable
     * @throw OperationNotAllowed if MultiDimFunctionGraph has no variable yet.
     * @return the id of the added non terminal node.
     */
    // ============================================================================
    virtual NodeId addInternalNode( const DiscreteVariable* var,
                                    NodeId* sons ) = 0;

    protected:
    NodeId _addInternalNode( const DiscreteVariable* var, NodeId* sons );

    public:
    // ============================================================================
    /**
     * Inserts a new non terminal node in graph.
     * NodeId of this node is generated automatically.
     *
     * @param var Associated variable
     * @throw OperationNotAllowed if MultiDimFunctionGraph has no variable yet.
     * @return the id of the added non terminal node.
     */
    // ============================================================================
    NodeId addInternalNode( const DiscreteVariable* var, NodeId nid );

    // ============================================================================
    /**
     * Adds a value to the MultiDimFunctionGraph.
     * This will create a terminal node, which of id is returned.
     * If a terminal node with such value already exists,
     * its id will be return instead.
     *
     * @param value The value added by copy.
     * @return the id of the terminal node hence created.
     *
     */
    // ============================================================================
    NodeId addTerminalNode( const GUM_SCALAR& value );

    // ============================================================================
    /**
     * Erases a node from the diagram.
     *
     * @param id The id of the variable to erase.
     * @param replacingId offers the possibility to reroute any parent to the
     * given node
     * @param updateParents indicates if such remapping has to be done
     * @throw NotFound if node isn't in diagram
     */
    // ============================================================================
    void
    eraseNode( NodeId id, NodeId replacingId = 0, bool updateParents = true );

    /// @}

    // ############################################################################
    /// @name Manipulation methods.
    // ############################################################################
    /// @{

    // ============================================================================
    /// Sets nodes son for given modality to designated son node
    // ============================================================================
    void
    setSon( const NodeId& node, const Idx& modality, const NodeId& sonNode );


    // ============================================================================
    /// Performs a sifting in search of a(local) minimal size
    // ============================================================================
    void minimizeSize();

    // ============================================================================
    /// Changes var position in variable sequence
    // ============================================================================
    void moveTo( const DiscreteVariable* x, Idx desiredPos );

    private:
    // ============================================================================
    /// Swap two adjacent variable.
    /// Order is important here.
    /// X must precede Y before the swap (at the end Y will then precede X).
    /// Not respecting this constraint leads to unattended behaviour.
    // ============================================================================
    void __adjacentSwap( const DiscreteVariable* x, const DiscreteVariable* y );

    protected:
    // ============================================================================
    /// Remaps all arcs going to ou going from the first given node to the
    /// second
    /// node, then delete first node.
    // ============================================================================
    void _migrateNode( const NodeId&, const NodeId& );

    /// @}

    // ############################################################################
    /// @name Redundancy methods.
    // ############################################################################
    /// @{

    protected:
    // ============================================================================
    /// Checks if a similar node does not already exists in the graph or
    /// if it has the same child for every variable value
    /// If no node is a match, this node is added to the graph
    /// @warning : will deallocate by itslef sonsMap if a match exists
    // ============================================================================
    NodeId _nodeRedundancyCheck( const DiscreteVariable* var, NodeId* sonsMap );

    private:
    // ============================================================================
    /// Checks if a similar node does not already exists in the graph
    /// (meaning for every value assume by the associated variable, these two
    /// nodes
    /// have the same children)
    /// @warning WON'T deallocate sons
    // ============================================================================
    NodeId __checkIsomorphism( const DiscreteVariable* var, NodeId* sons );

    // ============================================================================
    /// Checks if node has the same child for every variable value
    /// @warning WON'T deallocate sons
    // ============================================================================
    bool __isRedundant( const DiscreteVariable* var, NodeId* sons );

    // ============================================================================
    /// Ensures that every isomorphic subgraphs are merged together.
    // ============================================================================
    public:
    virtual void reduce() = 0;

    protected:
    void _reduce();


    /// @}

    public:
    // ============================================================================
    /// Removes var without nodes in the diagram
    // ============================================================================
    void clean();


    private:
    // ============================================================================
    /// The multidimdecisiongraph supposed to be edited.
    // ============================================================================
    MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* __functionGraph;
  };

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  class MultiDimFunctionGraphTreeManager
      : public MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy> {
    // ============================================================================
    /// This friend methods from is the only way to get an instance of a
    /// manager.
    /// See class description for more info.
    // ============================================================================
    friend MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>*
    MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::manager();

    MultiDimFunctionGraphTreeManager(
        MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* master )
        : MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>(
              master ) {}

    public:
    NodeId addInternalNode( const DiscreteVariable* var, NodeId* sons ) {
      return this->_addInternalNode( var, sons );
    }
    void reduce() {}
  };

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  class MultiDimFunctionGraphROManager
      : public MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy> {

    // ============================================================================
    /// This friend methods from is the only way to get an instance of a
    /// manager.
    /// See class description for more info.
    // ============================================================================
    friend MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>*
    MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::manager();

    MultiDimFunctionGraphROManager(
        MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* master )
        : MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>(
              master ) {}

    public:
    NodeId addInternalNode( const DiscreteVariable* var, NodeId* sons ) {
      return this->_nodeRedundancyCheck( var, sons );
    }
    void reduce() { this->_reduce(); }
  };
}  // namespace gum

// ============================================================================
#include <agrum/multidim/multiDimFunctionGraphManager.tcc>
// ============================================================================
#endif  // GUM_MULTI_DIM_FUNCTION_GRAPH_MANAGER_H
// ============================================================================

