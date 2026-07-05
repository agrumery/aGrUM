/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Headers of the ExactTerminalNodePolicy
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_EXACT_TERMINAL_NODE_POLICY_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_EXACT_TERMINAL_NODE_POLICY_H

#include <agrum/base/multidim/utils/FunctionGraphUtilities/terminalNodePolicies/ITerminalNodePolicy.h>

namespace gum {

  // clang-format off
  /**
   * @class ExactTerminalNodePolicy
   * @headerfile ExactTerminalNodePolicy.h <agrum/base/multidim/FunctionGraphUtilities/terminalNodePolicies/ExactTerminalNodePolicy.h>
   * @ingroup multidim_group
   *
   * @brief Implementation of a Terminal Node Policy that maps nodeid directly
   * to value.
   */
  // clang-format on

  template < typename GUM_ELEMENT >
  class ExactTerminalNodePolicy: public ITerminalNodePolicy< GUM_ELEMENT > {
    public:
    // ============================================================================
    /// @name Terminal Node Creation and Destruction
    // ============================================================================
    /// @{

    /// Insert a new terminal node with given value
    void addTerminalNode(const NodeId& n, const GUM_ELEMENT& v);

    /// Remove node matching given id
    void eraseTerminalNode(const NodeId& n);

    /// Erase all terminal nodes
    void clearAllTerminalNodes();

    /// @}
    // ============================================================================
    /// @name Terminal Nodes Existence
    // ============================================================================
    /// @{

    /// Returns true if a terminal node matching this id exists
    bool existsTerminalNodeWithId(const NodeId& n) const;

    /// Returns true if a terminal node matching this value exists
    bool existsTerminalNodeWithValue(const GUM_ELEMENT& v) const;

    /// @}
    // ============================================================================
    /// @name Terminal Nodes value and id access
    // ============================================================================
    /// @{

    /// Returns the value of the terminal node that has the given id
    const GUM_ELEMENT& terminalNodeValue(const NodeId& n) const;

    /// Returns the id of the terminal node that has the given value
    const NodeId& terminalNodeId(const GUM_ELEMENT& v) const;

    /// @}
    // ============================================================================
    /// @name Iterator on Terminal Nodes
    // ============================================================================
    /// @{

    /// Initializes the constant safe iterator on terminal nodes
    void beginValues() const;

    /// Indicates if constant safe iterator has reach end of terminal nodes list
    bool hasValue() const;

    /// Increments the constant safe iterator
    void nextValue() const;

    /// Returns the value of the current terminal nodes pointed by the constant
    /// safe iterator
    const GUM_ELEMENT& value() const;

    /// Returns the id of the current terminal nodes pointed by the constant
    /// safe iterator
    const NodeId& id() const;

    /// @}

    private:
    Bijection< NodeId, GUM_ELEMENT >                     _map_;
    mutable BijectionIteratorSafe< NodeId, GUM_ELEMENT > _mappy_;
  };

}   // End of namespace gum

#include <agrum/base/multidim/utils/FunctionGraphUtilities/terminalNodePolicies/ExactTerminalNodePolicy_tpl.h>

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_EXACT_TERMINAL_NODE_POLICY_H */
