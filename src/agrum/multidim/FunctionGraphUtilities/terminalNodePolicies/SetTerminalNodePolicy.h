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
 * @brief Headers of the ITerminalNodePolicy
 *
 * @author Jean-Christophe Magnan
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_SET_TERMINAL_NODE_POLICY_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_SET_TERMINAL_NODE_POLICY_H

#include <agrum/graphs/nodeGraphPart.h>

namespace gum {

  // clang-format off
  /**
   * @class SetTerminalNodePolicy
   * @headerfile SetTerminalNodePolicy.h <agrum/multidim/FunctionGraphUtilities/terminalNodePolicies/SetTerminalNodePolicy.h>
   * @ingroup multidim_group
   *
   * @brief Implementation of a Terminal Node Policy that maps nodeid to a set
   * of value
   */
  // clang-format on
  template <typename GUM_SCALAR>
  class SetTerminalNodePolicy {

    public:
    ~SetTerminalNodePolicy() { clearAllTerminalNodes(); }

    // ============================================================================
    /// @name Terminal Node Creation and Destruction
    // ============================================================================
    /// @{

    /// Insert a new terminal node with given value
    void addTerminalNode( const NodeId& n, const GUM_SCALAR& v ) {
      if ( __map.exists( n ) ) *( __map[n] ) += v;
      __map.insert( n, new GUM_SCALAR( v ) );
    }

    /// Remove node matching given id
    void eraseTerminalNode( const NodeId& n ) {
      if ( __map.exists( n ) ) __map.erase( n );
    }

    /// Erase all terminal nodes
    void clearAllTerminalNodes() {
      for ( auto nodeIter = __map.beginSafe(); nodeIter != __map.endSafe();
            ++nodeIter )
        delete nodeIter.val();
      __map.clear();
    }

    /// @}
    // ============================================================================
    /// @name Terminal Nodes Existence
    // ============================================================================
    /// @{

    /// Returns true if a terminal node matching this id exists
    bool existsTerminalNodeWithId( const NodeId& n ) const {
      return __map.exists( n );
    }

    /// Returns true if a terminal node matching this value exists
    bool existsTerminalNodeWithValue( const GUM_SCALAR& v ) const {
      return terminalNodeId( v ) != 0;
    }

    /// @}
    // ============================================================================
    /// @name Terminal Nodes value and id access
    // ============================================================================
    /// @{

    /// Returns the value of the terminal node that has the given id
    const GUM_SCALAR& terminalNodeValue( const NodeId& n ) const {
      return *( __map[n] );
    }

    /// Returns the id of the terminal node that has the given value
    const NodeId& terminalNodeId( const GUM_SCALAR& v ) const {
      for ( auto nodeIter = __map.beginSafe(); nodeIter != __map.endSafe();
            ++nodeIter )
        if ( *( nodeIter.val() ) == v ) return nodeIter.key();
      return jocker;
    }

    /// @}
    // ============================================================================
    /// @name Iterator on Terminal Nodes
    // ============================================================================
    /// @{

    /// Initializes the constant safe iterator on terminal nodes
    void beginValues() const { __mappy = __map.cbeginSafe(); }

    /// Indicates if constant safe iterator has reach end of terminal nodes list
    bool hasValue() const { return __mappy != __map.cendSafe(); }

    /// Increments the constant safe iterator
    void nextValue() const { ++__mappy; }

    /// Returns the value of the current terminal nodes pointed by the constant
    /// safe iterator
    const GUM_SCALAR& value() const { return *( __mappy.val() ); }

    /// Returns the id of the current terminal nodes pointed by the constant
    /// safe iterator
    const NodeId& id() const { return __mappy.key(); }

    private:
    /// The mapping between NodeIds and Value Sets
    HashTable<NodeId, GUM_SCALAR*>                          __map;
    mutable HashTableConstIteratorSafe<NodeId, GUM_SCALAR*> __mappy;

    const NodeId jocker = 0;
  };

}  // End of namespace gum

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_SET_TERMINAL_NODE_POLICY_H */
