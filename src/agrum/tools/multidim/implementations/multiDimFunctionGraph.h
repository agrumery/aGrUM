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
 * @brief Headers of MultiDimFunctionGraph.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_H

#include <agrum/tools/core/types.h>

#include <agrum/FMDP/learning/core/templateStrategy.h>


#include <agrum/tools/multidim/implementations/multiDimFunctionGraphManager.h>
#include <agrum/tools/multidim/implementations/multiDimImplementation.h>
#include <agrum/tools/multidim/utils/FunctionGraphUtilities/terminalNodePolicies/ExactTerminalNodePolicy.h>

namespace gum {

  template < typename GUM_SCALAR, template < typename > class TerminalNodePolicy >
  class MultiDimFunctionGraphManager;

  template < typename GUM_SCALAR, template < typename > class TerminalNodePolicy >
  class MultiDimFunctionGraphROManager;

  template < typename GUM_SCALAR, template < typename > class TerminalNodePolicy >
  class MultiDimFunctionGraphTreeManager;

  /**
   * @class MultiDimFunctionGraph
   * @ingroup multidim_group
   *
   * @warning Doxygen does not like spanning command on multiple line, so we
   * could not configure it with the correct include directive. Use the
   * following code snippet to include this file.
   * @code
   * #include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>
   * @endcode
   *
   * @brief Class implementingting a function graph.
   *
   * @tparam GUM_SCALAR The type of scalars stored in this multidimensional
   * table.
   * @tparam TerminalNodePolicy The terminal node policy to use in this
   * MultiDimFunctionGraph.
   */
  template < typename GUM_SCALAR,
             template < typename > class TerminalNodePolicy = ExactTerminalNodePolicy >
  class MultiDimFunctionGraph:
      public MultiDimImplementation< GUM_SCALAR >,
      public TerminalNodePolicy< GUM_SCALAR > {
    public:
    /// Only for proper initialization of a certain returned value
    const static GUM_SCALAR defaultValue;

    public:
    // =========================================================================
    /// @name Constructors, destructor and copy
    // =========================================================================
    /// @{

    private:
    /**
     * @brief Default constructor.
     * @param isReduced If true, then this MultiDimFunctionGraph is reduced.
     */
    MultiDimFunctionGraph(bool isReduced = true);

    public:
    /**
     * @brief Copy constructor.
     * @param from The MultiDimFunctionGraph to copy.
     */
    MultiDimFunctionGraph(const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >& from);

    /**
     * @brief Copy Operator.
     * @param from The MultiDimFunctionGraph to copy.
     * @return Returns this MultiDimFunctionGraph.
     */
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >&
       operator=(const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >& from);

    /**
     * @brief Class destructor.
     */
    ~MultiDimFunctionGraph();

    /// @}
    // =========================================================================
    /// @name Unallowed inherited methods
    // =========================================================================
    /// @{

    /**
     * @warning This will raise an OperationNotAllowed exceptions. Use the
     * MultiDimFunctionGraphManager class to edit a MultiDimFunctionGraph.
     */
    virtual void set(const Instantiation& i, const GUM_SCALAR& value) const;

    /**
     * @warning This will raise an OperationNotAllowed exceptions. Use the
     * MultiDimFunctionGraphManager class to edit a MultiDimFunctionGraph.
     */
    virtual void fill(const GUM_SCALAR& d) const;

    /**
     * @warning This will raise an OperationNotAllowed exceptions. Use the
     * MultiDimFunctionGraphManager class to edit a MultiDimFunctionGraph.
     */
    virtual void populate(const std::vector< GUM_SCALAR >& v) const;
    /**
     * @warning This will raise an OperationNotAllowed exceptions. Use the
     * MultiDimFunctionGraphManager class to edit a MultiDimFunctionGraph.
     */
    virtual void populate(std::initializer_list< GUM_SCALAR > v) const;

    /**
     * @warning This will raise an OperationNotAllowed as MultiDimFunctionGraph
     * can't copy other multiDim.
     */
    virtual void copyFrom(const MultiDimContainer< GUM_SCALAR >& src,
                          Instantiation*                         p_i = (Instantiation*)0) const;

    /**
     * @warning This will raise an OperationNotAllowed as MultiDimFunctionGraph
     * can't copy other multiDim.
     */
    virtual void copy(const MultiDimContainer< GUM_SCALAR >& src);

    /// @}
    // =========================================================================
    /// @name Inherited methods
    // =========================================================================
    /// @{

    virtual MultiDimContainer< GUM_SCALAR >* newFactory() const;

    virtual GUM_SCALAR get(const Instantiation& i) const;

    virtual const std::string& name() const;

    virtual void add(const DiscreteVariable& v);

    virtual void erase(const DiscreteVariable& v);

    virtual Size realSize() const;

    virtual void changeNotification(const Instantiation&          i,
                                    const DiscreteVariable* const var,
                                    Idx                           oldval,
                                    Idx                           newval);

    virtual void setFirstNotification(const Instantiation& i);

    virtual void setLastNotification(const Instantiation& i);

    virtual void setIncNotification(const Instantiation& i);

    virtual void setDecNotification(const Instantiation& i);

    virtual void setChangeNotification(const Instantiation& i);

    virtual std::string toString(const Instantiation* i) const;

    /// @}
    // =========================================================================
    /// @name Copy methods.
    // =========================================================================
    /// @{

    /**
     * @brief Removes all variables in this MultiDimFunctionGraph and copy the
     * content of src, variables included.
     *
     * @param src The MultiDimFunctionGraph to copy.
     */
    void copy(const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >& src);

    /**
     * @brief Copies src diagrams structure into this diagrams.
     *
     * However it also changes the variables.
     *
     * @warning This has two implications:
     *  - First, this is not just a renaming. Pointers are trully changed.
     *  - Second, for each pair of variable, the new variable MUST macth the
     *  number of modalities of the old variable.
     *
     * @param src The MultiDimFunctionGraph to copy.
     * @param reassign A Bijection form variables in src to variables in this.
     */
    void copyAndReassign(
       const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >&       src,
       const Bijection< const DiscreteVariable*, const DiscreteVariable* >& reassign);

    /**
     * @brief Copies src diagrams and multiply every value by the given scalar.
     *
     * @param src The MultiDimFunctionGraph to copy.
     * @param gamma The scalar used to multiply every value with.
     */
    void copyAndMultiplyByScalar(const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >& src,
                                 GUM_SCALAR gamma);

    /**
     * @brief Clears the function graph
     */
    void clear();

    /// @}
    // =========================================================================
    /// @name Accessors and modifiers
    // =========================================================================
    /// @{

    /**
     * @brief Returns a const reference to the manager of this diagram
     *
     * @param withBackArcs If true, back arcs will be added.
     * @return Returns a const reference to the manager of this diagram
     */
    std::string toDot(bool withBackArcs = false) const;

    /**
     * @brief Returns a const reference to the manager of this diagram
     * @return Returns a const reference to the manager of this diagram
     */
    const NodeGraphPart& model() const;

    /**
     * @brief Returns a const reference to the manager of this diagram
     * @return Returns a const reference to the manager of this diagram
     */
    MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >* manager();

    /**
     * @brief Returns the id of the root node from the diagram
     * @return Returns the id of the root node from the diagram
     */
    const NodeId& root() const;

    /**
     * @brief Indicates if given node is terminal or not
     * @param node The node to test for terminality.
     * @return Returns true if node is terminal.
     */
    bool isTerminalNode(const NodeId& node) const;

    /**
     * @brief Indicates if given node is terminal or not
     * @param node The node to test for being itnernal.
     * @return Returns true if node is internal.
     */
    bool isInternalNode(const NodeId& node) const;

    /**
     * @brief Returns value associated to given node.
     * @param n The node for which the value is returned.
     * @return Returns value associated to given node.
     * @throw InvalidNode Raised if node isn't terminal.
     */
    const GUM_SCALAR& nodeValue(NodeId n) const;

    /**
     * @brief Returns internalNode structure associated to that nodeId
     * @param n The node for which the internal structure is returned.
     * @return Returns internalNode structure associated to that nodeId
     * @throw InvalidNode Raised if node is terminal
     */
    const InternalNode* node(NodeId n) const;

    /**
     * @brief Returns the list of node associated to given variable
     * @param var The variable for which the list of associated nodes is
     * returned.
     * @return Returns the list of node associated to given variable
     */
    const LinkedList< NodeId >* varNodeListe(const DiscreteVariable* var) const;

    /**
     * @brief Returns the name of the table represented by this structure.
     * @return Returns the name of the table represented by this structure.
     */
    const std::string& tableName() const;

    /**
     * @brief Sets the name of the table represented by this structure.
     * @param name the new name of this structure.
     */
    void setTableName(const std::string& name);

    /**
     * @brief Returns true if this MultiDimFunctionGraph is reduced and
     * Ordered.
     * @return Returns true if this MultiDimFunctionGraph is reduced and
     * Ordered.
     */
    bool isReducedAndOrdered() const;

    /**
     * @brief Returns a reduced and ordered instance.
     *
     * Reduced and ordered instance will reduce the size of the graph whenever
     * it's possible. An inherent order on the variable helps doing so.  The
     * order in which variables will be inserted with function add(const
     * DiscreteVariable&) specify that order.
     *
     * @return Returns a reduced and ordered instance.
     */
    static MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* getReducedAndOrderedInstance();

    /**
     * @brief Returns an arborescent instance
     * @return Returns an arborescent instance
     */
    static MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* getTreeInstance();

    /// @}

    protected:
    /**
     * @warning This will raise an OperationNotAllowed as this method has not
     * yet been implemented.
     */
    virtual void replace_(const DiscreteVariable* x, const DiscreteVariable* y);

    /**
     * @warning This will raise an OperationNotAllowed as you should not use
     * this method as MultiDimFunctionGraph use its own internal structure for
     * storing data.
     */
    GUM_SCALAR& get_(const Instantiation& inst) const;

    private:
    /// The name of the data structure
    std::string _name_;

    /// The name of the data structure
    std::string _tableName_;

    /// Indicates available nodeIds
    NodeGraphPart _model_;

    /// A reference to the manager that edits this function graph
    MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >* _manager_;

    /// The root node of the function graph
    NodeId _root_;

    /// Associates each non-terminal node to a variable
    HashTable< NodeId, InternalNode* > _internalNodeMap_;

    /// Mapping between var and node
    HashTable< const DiscreteVariable*, LinkedList< NodeId >* > _var2NodeIdMap_;

    /// Wheter the MultiDimFunctionGraphManager is reduced or not
    bool _isReduced_;

    friend class MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  // extern template class MultiDimFunctionGraph<float>;
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  // extern template class MultiDimFunctionGraph<double>;
#  endif
#endif
}   // namespace gum

#include <agrum/tools/multidim/implementations/multiDimFunctionGraph_tpl.h>

#endif   // GUM_MULTI_DIM_FUNCTION_GRAPH_H
