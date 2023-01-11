/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Headers of MultiDimFunctionGraphManager.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 *
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_MANAGER_H
#  define GUM_MULTI_DIM_FUNCTION_GRAPH_MANAGER_H

#  include <agrum/agrum.h>
#  include <agrum/tools/graphs/parts/nodeGraphPart.h>
#  include <agrum/tools/multidim/utils/FunctionGraphUtilities/internalNode.h>

namespace gum {

  template < typename GUM_SCALAR, template < typename > class TerminalNodePolicy >
  class MultiDimFunctionGraph;

  /**
   * @class MultiDimFunctionGraphManager
   * @ingroup multidim_group
   *
   * @brief Class implementingting a function graph manager.
   *
   * @warning Doxygen does not like spanning command on multiple line, so we
   * could not configure it with the correct include directive. Use the
   * following code snippet to include this file.
   * @code
   * #include <agrum/tools/multidim/implementations/multiDimFunctionGraphManager.h>
   * @endcode
   *
   * This class provides methods to edit a Function Graph. Any modification on
   * a MultiDimFunctionGraph graph must be done via this class.
   *
   * This class is a singleton and its unique instance ca be accessed using the
   * MultiDimFunctionGraph::getManager() method.
   *
   * To do so :
   *
   * @code
   * // You can also use getTreeInstance()
   * auto * func_graph =
   *    MultiDimFunctionGraph<double>::getReducedAndOrderedInstance();
   * auto manager = dg->manager();
   * @endcode
   *
   * @tparam GUM_SCALAR The type of scalars stored in the multidimensional
   * table.
   * @tparam TerminalNodePolicy The terminal node policy to use.
   */
  template < typename GUM_SCALAR, template < typename > class TerminalNodePolicy >
  class MultiDimFunctionGraphManager {
    // =========================================================================
    /// @name Constructors / Destructors
    // =========================================================================
    /// @{

    /**
     * @brief This friend methods from is the only way to get an instance of a
     * manager.
     *
     * See class description for more info.
     */
    friend MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >*
       MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >::manager();

    /**
     * @brief Default constructor.
     *
     * You have to call MultiDimFunctionGraph::getManager() to get the instance
     * of MultiDimFunctionGraphManager bound to your function graph.
     */
    protected:
    explicit MultiDimFunctionGraphManager(
       MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* master);

    public:
    /**
     * @brief Class destructor.
     */
    virtual ~MultiDimFunctionGraphManager();

    /// @}
    // =========================================================================
    /// @name Nodes manipulation methods.
    // =========================================================================
    /// @{

    /**
     * @brief Sets root node of decision diagram.
     * @param root The node to set as root.
     */
    void setRootNode(const NodeId& root);

    /**
     * @brief Inserts a new non terminal node in graph.
     *
     * NodeId of this node is generated automatically.
     *
     * @param var Associated variable
     * @return The id of the added non terminal node.
     */
    NodeId addInternalNode(const DiscreteVariable* var);

    /**
     * @brief Inserts a new non terminal node in graph.
     *
     * NodeId of this node is generated automatically.
     *
     * @param var The associated variable.
     * @param sons A table of size var->domainSize() containing nodeid of sons
     * nodes.
     * @return Returns the id of the added non terminal node.
     *
     * @throw OperationNotAllowed Raised if MultiDimFunctionGraph has no
     * variable yet.
     */
    virtual NodeId addInternalNode(const DiscreteVariable* var, NodeId* sons) = 0;

    protected:
    /**
     * @brief Adds an internal node.
     * @param var The node to add.
     * @param sons The node sons.
     * @return Returns the added node id.
     */
    NodeId addInternalNode_(const DiscreteVariable* var, NodeId* sons);

    public:
    /**
     * @brief Inserts a new non terminal node in graph.
     *
     * NodeId of this node is generated automatically.
     *
     * @param var The ssociated variable.
     * @param nid The desired id for that node.
     * @return Returns the id of the added non terminal node.
     *
     * @throw OperationNotAllowed Raised if MultiDimFunctionGraph has no
     * variable yet.
     */
    NodeId addInternalNode(const DiscreteVariable* var, NodeId nid);

    /**
     * @brief Adds a value to the MultiDimFunctionGraph.
     *
     * This will create a terminal node, which of id is returned. If a
     * terminal node with such value already exists, its id will be return
     * instead.
     *
     * @param value The value added by copy.
     * @return Returns he id of the terminal node hence created.
     */
    NodeId addTerminalNode(const GUM_SCALAR& value);

    /**
     * @brief Erases a node from the diagram.
     *
     * @param id The id of the variable to erase.
     * @param replacingId Offers the possibility to reroute any parent to the
     * given node.
     * @param updateParents Indicates if such remapping has to be done.
     *
     * @throw NotFound Raised if node isn't in diagram.
     */
    void eraseNode(NodeId id, NodeId replacingId = 0, bool updateParents = true);

    /// @}
    // =========================================================================
    /// @name Manipulation methods.
    // =========================================================================
    /// @{

    /**
     * @brief Sets nodes son for given modality to designated son node.
     * @param node The node to which a node is added.
     * @param modality The modality for which sonNode is added to node.
     * @param sonNode The node to add as a son to node.
     */
    void setSon(const NodeId& node, const Idx& modality, const NodeId& sonNode);


    /**
     * @brief Performs a sifting in search of a(local) minimal size.
     */
    void minimizeSize();

    /**
     * @brief Changes var position in variable sequence.
     * @param x The varaible to change.
     * @param desiredPos The new posiition.
     */
    void moveTo(const DiscreteVariable* x, Idx desiredPos);

    private:
    /**
     * @brief Swap two adjacent variable.
     *
     * Order is important here.  X must precede Y before the swap (at the end Y
     * will then precede X).  Not respecting this constraint leads to
     * unattended behaviour.
     *
     * @param x The first variable to swap.
     * @param y The second variable to swap.
     */
    void _adjacentSwap_(const DiscreteVariable* x, const DiscreteVariable* y);

    protected:
    /**
     * @brief Remaps all arcs going to ou going from the first given node to
     * the second node, then delete first node.
     *
     * @param x The variable from which all arcs are removed.
     * @param y The variable for which all of x arcs are added.
     */
    void migrateNode_(const NodeId& x, const NodeId& y);

    /// @}
    // =========================================================================
    /// @name Redundancy methods.
    // =========================================================================
    /// @{

    protected:
    /**
     * @brief Check for redundancy.
     *
     * Checks if a similar node does not already exists in the graph or if it
     * has the same child for every variable value. If no node is a match, this
     * node is added to the graph.
     *
     * @warning : will free by itslef sonsMap if a match exists.
     *
     * @param var The node to add in the graph.
     * @param sonsMap The node sons.
     * @return Returns the nodes id in the graph.
     */
    NodeId nodeRedundancyCheck_(const DiscreteVariable* var, NodeId* sonsMap);

    private:
    /**
     * @brief Checks if a similar node does not already exists in the graph.
     *
     * Tow nodes are similar if for every value assumed by the associated
     * variable, these two nodes have the same children.
     *
     * @warning This will not free sons.
     *
     * @param var The node to check for.
     * @param sons The node sons.
     * @return Returns the node id if found, 0 otherwhise.
     */
    NodeId _checkIsomorphism_(const DiscreteVariable* var, NodeId* sons);

    /**
     * @brief Checks if node has the same child for every variable value.
     *
     * @warning WON'T deallocate sons
     *
     * @param var The node to check for.
     * @param sons The node sons.
     * @return Returns true if the node is redundant.
     */
    bool _isRedundant_(const DiscreteVariable* var, NodeId* sons);

    public:
    /**
     * @brief Ensures that every isomorphic subgraphs are merged together.
     */
    virtual void reduce() = 0;

    protected:
    /**
     * @brief Ensures that every isomorphic subgraphs are merged together.
     */
    void reduce_();

    /// @}

    public:
    /**
     * @brief Removes var without nodes in the diagram
     */
    void clean();

    private:
    /// The multidimdecisiongraph supposed to be edited.
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* _functionGraph_;
  };

  // ===========================================================================
  // MultiDimFunctionGraphTreeManager
  // ===========================================================================

  /**
   * @class MultiDimFunctionGraphTreeManager
   * @ingroup multidim_group
   *
   * @warning Doxygen does not like spanning command on multiple line, so we
   * could not configure it with the correct include directive. Use the
   * following code snippet to include this file.
   * @code
   * #include <agrum/tools/multidim/implementations/multiDimFunctionGraphManager.h>
   * @endcode
   *
   * @tparam GUM_SCALAR The type of scalars stored in the multidimensional
   * table.
   * @tparam TerminalNodePolicy The terminal node policy to use.
   */
  template < typename GUM_SCALAR, template < typename > class TerminalNodePolicy >
  class MultiDimFunctionGraphTreeManager:
      public MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy > {
    /// This friend methods from is the only way to get an instance of a
    /// manager.
    friend MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >*
       MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >::manager();

    // ========================================================================
    /// @name Constructor and destructor
    // ========================================================================
    /// @{
    /**
     * @brief Class constructor.
     */
    MultiDimFunctionGraphTreeManager(
       MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* master);

    public:
    /**
     * @brief Class destructor.
     */
    ~MultiDimFunctionGraphTreeManager();

    /// @}
    // ========================================================================
    /// @name Inherited methods
    // ========================================================================
    /// @{
    virtual NodeId addInternalNode(const DiscreteVariable* var, NodeId* sons);

    virtual void reduce();
    /// @}
  };

  // ===========================================================================
  // MultiDimFunctionGraphROManager
  // ===========================================================================

  /**
   * @class MultiDimFunctionGraphTreeManager
   * @ingroup multidim_group
   *
   * @warning Doxygen does not like spanning command on multiple line, so we
   * could not configure it with the correct include directive. Use the
   * following code snippet to include this file.
   * @code
   * #include <agrum/tools/multidim/implementations/multiDimFunctionGraphManager.h>
   * @endcode
   *
   * @tparam GUM_SCALAR The type of scalars stored in the multidimensional
   * table.
   * @tparam TerminalNodePolicy The terminal node policy to use.
   */
  template < typename GUM_SCALAR, template < typename > class TerminalNodePolicy >
  class MultiDimFunctionGraphROManager:
      public MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy > {
    /// This friend methods from is the only way to get an instance of a
    /// manager.
    friend MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >*
       MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >::manager();

    // ========================================================================
    /// @name Constructor and destructor
    // ========================================================================
    /// @{
    MultiDimFunctionGraphROManager(MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* master);

    public:
    ~MultiDimFunctionGraphROManager();

    /// @}
    // ========================================================================
    /// @name Inherited methods
    // ========================================================================
    /// @{

    virtual NodeId addInternalNode(const DiscreteVariable* var, NodeId* sons);

    virtual void reduce();

    /// @}
  };

}   // namespace gum

// ============================================================================
#  include <agrum/tools/multidim/implementations/multiDimFunctionGraphManager_tpl.h>
// ============================================================================
#endif   // GUM_MULTI_DIM_FUNCTION_GRAPH_MANAGER_H
// ============================================================================
