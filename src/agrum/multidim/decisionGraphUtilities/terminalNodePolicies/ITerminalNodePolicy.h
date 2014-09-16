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
#ifndef GUM_MULTI_DIM_DECISION_GRAPH_INTERFACE_TERMINAL_NODE_POLICY_H
#define GUM_MULTI_DIM_DECISION_GRAPH_INTERFACE_TERMINAL_NODE_POLICY_H

// ============================================================================
#include <agrum/graphs/nodeGraphPart.h>
// ============================================================================

namespace gum {

  /**
   * @class ITerminalNodePolicy ITerminalNodePolicy.h <agrum/multidim/decisionGraphUtilities/terminalNodePolicies/ITerminalNodePolicy.h>
   *
   * @brief Interface specifying the methods to be implemented by any TerminalNodePolicy
   *
   * @ingroup multidim_group
   */
  template <typename GUM_SCALAR>
  class ITerminalNodePolicy {

    public:
      virtual void addValue( const NodeId&, const GUM_SCALAR& ) = 0;
      virtual void eraseValue( const NodeId& ) = 0;
      virtual void clearValueMap() = 0;

      virtual bool existsNode( const NodeId& ) const = 0;
      virtual bool existsValue( const GUM_SCALAR& ) const = 0;

      virtual const GUM_SCALAR& nodeId2Value( const NodeId& ) const = 0;
      virtual const NodeId& value2NodeId( const GUM_SCALAR& ) const = 0;
  };

} // End of namespace gum

#endif /* GUM_MULTI_DIM_DECISION_GRAPH_INTERFACE_TERMINAL_NODE_POLICY_H */
