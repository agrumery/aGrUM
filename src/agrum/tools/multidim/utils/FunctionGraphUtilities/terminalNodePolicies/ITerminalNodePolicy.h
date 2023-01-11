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
 * @brief Headers of the ITerminalNodePolicy
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_INTERFACE_TERMINAL_NODE_POLICY_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_INTERFACE_TERMINAL_NODE_POLICY_H

#include <agrum/tools/graphs/parts/nodeGraphPart.h>

namespace gum {

  // clang-format off
  /**
   * @class ITerminalNodePolicy
   * @headerfile ITerminalNodePolicy.h <agrum/tools/multidim/FunctionGraphUtilities/terminalNodePolicies/ITerminalNodePolicy.h>
   * @ingroup multidim_group
   *
   * @brief Interface specifying the methods to be implemented by any
   * TerminalNodePolicy
   */
  template <typename GUM_SCALAR>
  class ITerminalNodePolicy {

    public:
    // ============================================================================
    /// @name Terminal Node Creation and Destruction
    // ============================================================================
    /// @{

    /// Insert a new terminal node with given value
    virtual void addTerminalNode( const NodeId& n, const GUM_SCALAR& v ) = 0;

    /// Remove node matching given id
    virtual void eraseTerminalNode( const NodeId& n ) = 0;

    /// Erase all terminal nodes
    virtual void clearAllTerminalNodes() = 0;

    /// @}
    // ============================================================================
    /// @name Terminal Nodes Existence
    // ============================================================================
    /// @{

    /// Returns true if a terminal node matching this id exists
    virtual bool existsTerminalNodeWithId( const NodeId& n ) const = 0;

    /// Returns true if a terminal node matching this value exists
    virtual bool existsTerminalNodeWithValue( const GUM_SCALAR& v ) const = 0;

    /// @}
    // ============================================================================
    /// @name Terminal Nodes value and id access
    // ============================================================================
    /// @{

    /// Returns the value of the terminal node that has the given id
    virtual const GUM_SCALAR& terminalNodeValue( const NodeId& n ) const = 0;

    /// Returns the id of the terminal node that has the given value
    virtual const NodeId& terminalNodeId( const GUM_SCALAR& v ) const = 0;

    /// @}
    // ============================================================================
    /// @name Iterator on Terminal Nodes
    // ============================================================================
    /// @{

    /// Initializes the constant safe iterator on terminal nodes
    virtual void beginValues() const = 0;

    /// Indicates if constant safe iterator has reach end of terminal nodes list
    virtual bool hasValue() const = 0;

    /// Increments the constant safe iterator
    virtual void nextValue() const = 0;

    /// Returns the value of the current terminal nodes pointed by the constant
    /// safe iterator
    virtual const GUM_SCALAR& value() const = 0;

    /// Returns the id of the current terminal nodes pointed by the constant
    /// safe iterator
    virtual const NodeId& id() const = 0;

    /// @}
  };

}  // End of namespace gum

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_INTERFACE_TERMINAL_NODE_POLICY_H */
