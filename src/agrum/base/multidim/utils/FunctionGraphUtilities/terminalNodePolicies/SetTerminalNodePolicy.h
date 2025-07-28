/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
 * @brief Headers of the ITerminalNodePolicy
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_SET_TERMINAL_NODE_POLICY_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_SET_TERMINAL_NODE_POLICY_H

#include <agrum/base/graphs/parts/nodeGraphPart.h>

namespace gum {

  // clang-format off
  /**
   * @class SetTerminalNodePolicy
   * @headerfile SetTerminalNodePolicy.h <agrum/base/multidim/FunctionGraphUtilities/terminalNodePolicies/SetTerminalNodePolicy.h>
   * @ingroup multidim_group
   *
   * @brief Implementation of a Terminal Node Policy that maps nodeid to a set
   * of value
   */
  // clang-format on

  template < typename GUM_SCALAR >
  class SetTerminalNodePolicy {
    public:
    ~SetTerminalNodePolicy() { clearAllTerminalNodes(); }

    // ============================================================================
    /// @name Terminal Node Creation and Destruction
    // ============================================================================
    /// @{

    /// Insert a new terminal node with given value
    void addTerminalNode(const NodeId& n, const GUM_SCALAR& v) {
      if (_map_.exists(n)) *(_map_[n]) += v;
      _map_.insert(n, new GUM_SCALAR(v));
    }

    /// Remove node matching given id
    void eraseTerminalNode(const NodeId& n) {
      if (_map_.exists(n)) _map_.erase(n);
    }

    /// Erase all terminal nodes
    void clearAllTerminalNodes() {
      for (auto nodeIter = _map_.beginSafe(); nodeIter != _map_.endSafe(); ++nodeIter)
        delete nodeIter.val();
      _map_.clear();
    }

    /// @}
    // ============================================================================
    /// @name Terminal Nodes Existence
    // ============================================================================
    /// @{

    /// Returns true if a terminal node matching this id exists
    bool existsTerminalNodeWithId(const NodeId& n) const { return _map_.exists(n); }

    /// Returns true if a terminal node matching this value exists
    bool existsTerminalNodeWithValue(const GUM_SCALAR& v) const { return terminalNodeId(v) != 0; }

    /// @}
    // ============================================================================
    /// @name Terminal Nodes value and id access
    // ============================================================================
    /// @{

    /// Returns the value of the terminal node that has the given id
    const GUM_SCALAR& terminalNodeValue(const NodeId& n) const { return *(_map_[n]); }

    /// Returns the id of the terminal node that has the given value
    const NodeId& terminalNodeId(const GUM_SCALAR& v) const {
      for (auto nodeIter = _map_.beginSafe(); nodeIter != _map_.endSafe(); ++nodeIter)
        if (*(nodeIter.val()) == v) return nodeIter.key();
      return jocker;
    }

    /// @}
    // ============================================================================
    /// @name Iterator on Terminal Nodes
    // ============================================================================
    /// @{

    /// Initializes the constant safe iterator on terminal nodes
    void beginValues() const { _mappy_ = _map_.cbeginSafe(); }

    /// Indicates if constant safe iterator has reach end of terminal nodes list
    bool hasValue() const { return _mappy_ != _map_.cendSafe(); }

    /// Increments the constant safe iterator
    void nextValue() const { ++_mappy_; }

    /// Returns the value of the current terminal nodes pointed by the constant
    /// safe iterator
    const GUM_SCALAR& value() const { return *(_mappy_.val()); }

    /// Returns the id of the current terminal nodes pointed by the constant
    /// safe iterator
    const NodeId& id() const { return _mappy_.key(); }

    private:
    /// The mapping between NodeIds and Value Sets
    HashTable< NodeId, GUM_SCALAR* >                          _map_;
    mutable HashTableConstIteratorSafe< NodeId, GUM_SCALAR* > _mappy_;

    const NodeId jocker = 0;
  };

}   // End of namespace gum

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_SET_TERMINAL_NODE_POLICY_H */
