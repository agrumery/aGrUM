
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @brief Headers of the ExactTerminalNodePolicy
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN (@LIP6)
 * @author Christophe GONZALES (@AMU) and Pierre-Henri WUILLEMIN (@LIP6)
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_EXACT_TERMINAL_NODE_POLICY_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_EXACT_TERMINAL_NODE_POLICY_H

#include <agrum/tools/core/bijection.h>
#include <agrum/tools/graphs/parts/nodeGraphPart.h>
#include <agrum/tools/multidim/utils/FunctionGraphUtilities/terminalNodePolicies/ITerminalNodePolicy.h>

namespace gum {

  // clang-format off
  /**
   * @class ExactTerminalNodePolicy
   * @headerfile ExactTerminalNodePolicy.h <agrum/tools/multidim/FunctionGraphUtilities/terminalNodePolicies/ExactTerminalNodePolicy.h>
   * @ingroup multidim_group
   *
   * @brief Implementation of a Terminal Node Policy that maps nodeid directly
   * to value.
   */
  // clang-format on
  template < typename GUM_SCALAR >
  class ExactTerminalNodePolicy: public ITerminalNodePolicy< GUM_SCALAR > {
    public:
    // ============================================================================
    /// @name Terminal Node Creation and Destruction
    // ============================================================================
    /// @{

    /// Insert a new terminal node with given value
    void addTerminalNode(const NodeId& n, const GUM_SCALAR& v) {
      __map.insert(n, v);
    }

    /// Remove node matching given id
    void eraseTerminalNode(const NodeId& n) { __map.eraseFirst(n); }

    /// Erase all terminal nodes
    void clearAllTerminalNodes() { __map.clear(); }

    /// @}
    // ============================================================================
    /// @name Terminal Nodes Existence
    // ============================================================================
    /// @{

    /// Returns true if a terminal node matching this id exists
    bool existsTerminalNodeWithId(const NodeId& n) const {
      return __map.existsFirst(n);
    }

    /// Returns true if a terminal node matching this value exists
    bool existsTerminalNodeWithValue(const GUM_SCALAR& v) const {
      return __map.existsSecond(v);
    }

    /// @}
    // ============================================================================
    /// @name Terminal Nodes value and id access
    // ============================================================================
    /// @{

    /// Returns the value of the terminal node that has the given id
    const GUM_SCALAR& terminalNodeValue(const NodeId& n) const {
      return __map.second(n);
    }

    /// Returns the id of the terminal node that has the given value
    const NodeId& terminalNodeId(const GUM_SCALAR& v) const {
      return __map.first(v);
    }

    /// @}
    // ============================================================================
    /// @name Iterator on Terminal Nodes
    // ============================================================================
    /// @{

    /// Initializes the constant safe iterator on terminal nodes
    void beginValues() const { __mappy = __map.beginSafe(); }

    /// Indicates if constant safe iterator has reach end of terminal nodes list
    bool hasValue() const { return __mappy != __map.endSafe(); }

    /// Increments the constant safe iterator
    void nextValue() const { ++__mappy; }

    /// Returns the value of the current terminal nodes pointed by the constant
    /// safe iterator
    const GUM_SCALAR& value() const { return __mappy.second(); }

    /// Returns the id of the current terminal nodes pointed by the constant
    /// safe iterator
    const NodeId& id() const { return __mappy.first(); }

    /// @}

    private:
    Bijection< NodeId, GUM_SCALAR >                     __map;
    mutable BijectionIteratorSafe< NodeId, GUM_SCALAR > __mappy;
  };

}   // End of namespace gum

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_EXACT_TERMINAL_NODE_POLICY_H */