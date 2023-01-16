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
 * @brief Headers of the InternalNode class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_INTERNAL_NODE_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_INTERNAL_NODE_H

#include <agrum/agrum.h>
#include <agrum/tools/graphs/graphElements.h>
#include <agrum/tools/multidim/utils/FunctionGraphUtilities/link.h>
#include <agrum/tools/variables/discreteVariable.h>

namespace gum {

  // clang-format off
  /**
   * @class Parent
   * @headerfile internalNode.h <agrum/tools/multidim/FunctionGraphUtilities/internalNode.h>
   * @ingroup multidim_group
   *
   * @brief Represent a node's parent.
   */
  // clang-format on
  struct Parent {
    NodeId parentId;
    Idx    modality;

    /**
     * @brief Class constructor.
     */
    Parent(NodeId pi, Idx m);

    /**
     * @brief Copy constructor.
     */
    Parent(const Parent& src);

    // ============================================================================
    /// @name Allocators and Deallocators redefinition
    // ============================================================================
    /// @{

    /**
     * @brief New operator.
     */
    void* operator new(size_t s);

    /**
     * @brief Delete operator.
     */
    void operator delete(void* p);

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    bool operator==(const Parent& comp) const;

    bool operator!=(const Parent& comp) const;

    Parent& operator=(const Parent& src);
    /// @}
  };

  // clang-format off
  /**
   * @class InternalNode
   * @headerfile internalNode.h <agrum/tools/multidim/FunctionGraphUtilities/internalNode.h>
   * @ingroup multidim_group
   *
   * @brief Structure used to represent a node internal structure
   */
  // clang-format on
  class InternalNode {
    private:
    /// Variable associated to such node
    const DiscreteVariable* _nodeVar_;

    /**
     * @brief Table of sons of the node.
     *
     * Each son is bound to a modality of the variable.
     * So those modalities are used has indexes for that table.
     *
     * @code
     *  _____________________________
     * |      |      |      |      |
     * | son1 | son2 | son3 | son4 |
     * | ______| ______| ______| ______|
     *    x1     x2     x3     x4
     *
     * @endcode
     */
    NodeId* _nodeSons_;

    /// The list of the node's parent
    LinkedList< Parent > _nodeParents_;

    /// @brief Set the node variable.
    void _setNodeVar_(const DiscreteVariable* v);

    public:
    // ============================================================================
    /// @name Constructors and Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default Constructor
     * Creates an empty node with no variable attached.
     */
    InternalNode();

    /**
     * @brief Class constructor
     *
     * Creates a node and attached the specified variable.
     * Initializes the sons.
     */
    explicit InternalNode(const DiscreteVariable* v);

    /**
     * @brief Constructor
     *
     * Creates a node and attached the specified variable.
     * Also attached the given on map (and will handle it by itself especially
     * deallocate it)
     *
     * @warning You'd better known what you're doing if you're using this one.
     * sons must have the size of domainSize of v or the program will fail!
     */
    InternalNode(const DiscreteVariable* v, NodeId* sons);

    /**
     * @brief Class destructors.
     */
    ~InternalNode();
    /// @}
    // ============================================================================
    /// @name Allocators and Deallocators redefinition
    // ============================================================================
    /// @{

    /**
     * @brief New operator.
     */
    void* operator new(size_t s);

    /**
     * @brief Delete operator.
     */
    void operator delete(void* p);

    /// @}
    // ============================================================================
    /// @name Node handlers
    // ============================================================================
    /// @{

    /**
     * @brief Allows you to respecify the node, changing its attached variable
     * as well as its son map
     *
     * @warning You'd better known what you're doing if you're using this one.
     * sons must have the size of domainSize of v or the program will fail!
     */
    void setNode(const DiscreteVariable* v, NodeId* sons);

    /// @}
    // ============================================================================
    /// @name Var handlers
    // ============================================================================
    /// @{

    /**
     * @brief Set the node variable.
     */
    void setNodeVar(const DiscreteVariable* v);

    /**
     * @brief Returns the node variable.
     */
    const DiscreteVariable* nodeVar() const;

    /// @}
    // ============================================================================
    /// @name Sons handlers
    // ============================================================================
    /// @{

    /**
     * @brief Sets the node's son.
     */
    void setSon(Idx modality, NodeId son);

    /**
     * @brief Returns the son at a given index.
     */
    NodeId son(Idx modality) const;

    /**
     * @brief Returns the number of sons.
     */
    Idx nbSons() const;

    /// @}
    // ============================================================================
    /// @name Parent handlers
    // ============================================================================
    /// @{

    /**
     * @brief Adds a parent.
     */
    void addParent(NodeId parent, Idx modality);

    /**
     * @brief Removes a parent.
     */
    void removeParent(NodeId parent, Idx modality);

    /**
     * @brief Returns the list of parents.
     */
    Link< Parent >* parents();

    /**
     * @brief Returns the list of parents.
     */
    const Link< Parent >* parents() const;

    /// @}
    // ============================================================================
    /// @name Allocator/deallocator
    // ============================================================================
    /// @{

    /**
     * @brief Allocates a table of nodeid of the size given in parameter.
     */
    static NodeId* allocateNodeSons(const DiscreteVariable* v);

    /**
     * @brief Deallocates a NodeSons table.
     */
    static void deallocateNodeSons(const DiscreteVariable* v, NodeId* s);

    /// @}
  };

}   // End of namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/tools/multidim/utils/FunctionGraphUtilities/internalNode_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_INTERNAL_NODE_H */
