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
 * @brief Headers of the ExactTerminalNodePolicy
 *
 * @author Jean-Christophe Magnan
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_EXACT_TERMINAL_NODE_POLICY_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_EXACT_TERMINAL_NODE_POLICY_H

#include <agrum/core/bijection.h>
#include <agrum/graphs/nodeGraphPart.h>
#include <agrum/multidim/FunctionGraphUtilities/terminalNodePolicies/ITerminalNodePolicy.h>

namespace gum {

  // clang-format off
  /**
   * @class ExactTerminalNodePolicy
   * @headerfile ExactTerminalNodePolicy.h <agrum/multidim/FunctionGraphUtilities/terminalNodePolicies/ExactTerminalNodePolicy.h>
   * @ingroup multidim_group
   *
   * @brief Implementation of a Terminal Node Policy that maps nodeid directly
   * to value.
   */
  // clang-format on
  template <typename GUM_SCALAR>
  class ExactTerminalNodePolicy : public ITerminalNodePolicy<GUM_SCALAR> {

    public:
    // ============================================================================
    /// @name Terminal Node Creation and Destruction
    // ============================================================================
    /// @{

    /// Insert a new terminal node with given value
    void addTerminalNode( const NodeId& n, const GUM_SCALAR& v ) {
      __map.insert( n, v );
    }

    /// Remove node matching given id
    void eraseTerminalNode( const NodeId& n ) { __map.eraseFirst( n ); }

    /// Erase all terminal nodes
    void clearAllTerminalNodes() { __map.clear(); }

    /// @}
    // ============================================================================
    /// @name Terminal Nodes Existence
    // ============================================================================
    /// @{

    /// Returns true if a terminal node matching this id exists
    bool existsTerminalNodeWithId( const NodeId& n ) const {
      return __map.existsFirst( n );
    }

    /// Returns true if a terminal node matching this value exists
    bool existsTerminalNodeWithValue( const GUM_SCALAR& v ) const {
      return __map.existsSecond( v );
    }

    /// @}
    // ============================================================================
    /// @name Terminal Nodes value and id access
    // ============================================================================
    /// @{

    /// Returns the value of the terminal node that has the given id
    const GUM_SCALAR& terminalNodeValue( const NodeId& n ) const {
      return __map.second( n );
    }

    /// Returns the id of the terminal node that has the given value
    const NodeId& terminalNodeId( const GUM_SCALAR& v ) const {
      return __map.first( v );
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
    Bijection<NodeId, GUM_SCALAR>                     __map;
    mutable BijectionIteratorSafe<NodeId, GUM_SCALAR> __mappy;
  };

}  // End of namespace gum

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_EXACT_TERMINAL_NODE_POLICY_H */
