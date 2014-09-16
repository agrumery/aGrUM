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
 *
 */
#ifndef GUM_MULTI_DIM_DECISION_GRAPH_SET_TERMINAL_NODE_POLICY_H
#define GUM_MULTI_DIM_DECISION_GRAPH_SET_TERMINAL_NODE_POLICY_H

// ============================================================================
#include <agrum/graphs/nodeGraphPart.h>
// ============================================================================

namespace gum {

  /**
   * @class SetTerminalNodePolicy SetTerminalNodePolicy.h <agrum/multidim/decisionGraphUtilities/terminalNodePolicies/SetTerminalNodePolicy.h>
   *
   * @brief Implementation of a Terminal Node Policy that maps nodeid to a set of value
   *
   * @ingroup multidim_group
   */
  template <typename GUM_SCALAR>
  class SetTerminalNodePolicy {

    public:
      void addValue( const NodeId& n, const GUM_SCALAR& v ) {
        if( __map.exists(n) )
          *(__map[n]) += v;
        __map.insert( n, new GUM_SCALAR(v) );
      }

      void eraseValue( const NodeId& n ) {
        if( __map.exists(n) )
          __map.erase(n);
      }

      void clearValueMap() {
        for( auto nodeIter = __map.beginSafe(); nodeIter != __map.beginSafe(); ++nodeIter )
          delete *nodeIter;
        __map.clear();
      }


      bool existsNode( const NodeId& n ) const { return __map.exists(n); }

      bool existsValue( const GUM_SCALAR& v ) const { return value2NodeId(v) != 0; }


      const GUM_SCALAR& nodeId2Value( const NodeId& n ) const { return *(__map[n]); }

      const NodeId& value2NodeId( const GUM_SCALAR& v ) const {
        for( auto nodeIter = __map.beginSafe(); nodeIter != __map.beginSafe(); ++nodeIter )
          if( **nodeIter == v )
            return nodeIter.key();
        return 0;
      }

    private:

      /// The mapping between NodeIds and Value Sets
      HashTable<NodeId, GUM_SCALAR*> __map;
  };

} // End of namespace gum

#endif /* GUM_MULTI_DIM_DECISION_GRAPH_INTERFACE_TERMINAL_NODE_POLICY_H */
