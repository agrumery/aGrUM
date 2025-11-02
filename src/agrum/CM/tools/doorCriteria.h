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
 * @brief Door criteria (backdoor/frontdoor) utilities bound to a single DAG.
 */
#ifndef GUM_DOOR_CRITERIA_H
#define GUM_DOOR_CRITERIA_H

#include <vector>

#include <agrum/base/graphs/DAG.h>

namespace gum {

  /**
   * @class DoorCriteria
   * @brief Implements Backdoor and Frontdoor criteria utilities bound to a single DAG.
   *
   * This class provides helpers to:
   *  - Check whether a conditioning set satisfies the backdoor/frontdoor criterion.
   *  - Enumerate all valid backdoor/frontdoor adjustment sets.
   *
   * The implementation mirrors pyAgrum functionality,.
   *
   * **Naming correspondences with pyAgrum:**
   *  - enumerateBackdoorSets        ⇨ pyagrum: backdoor_generator
   *  - enumerateFrontdoorSets       ⇨ pyagrum: frontdoor_generator
   *  - existsUnblockedDirectedPath  ⇨ pyagrum: exists_unblocked_directed_path
   *  - nodesOnDirectedPaths         ⇨ pyagrum: nodes_on_dipath
   *  - backdoorReach                ⇨ pyagrum: backdoor_reach
   *  - hasBackdoorPath              ⇨ pyagrum: backdoor_path
   *  - satisfiesBackdoorCriterion   ⇨ pyagrum: is_backdoor
   *  - satisfiesFrontdoorCriterion  ⇨ pyagrum: is_frontdoor
   */
  class DoorCriteria {
    public:
    /// Convenience type: list of candidate adjustment sets.
    using NodeSetVec = std::vector< NodeSet >;

    /**
     * @struct EnumerationOptions
     * @brief Options controlling enumeration of adjustment sets.
     *
     * Mirrors pyAgrum's pre-yield filters in *_generator functions.
     */
    struct EnumerationOptions {
      NodeSet     excluded_nodes{};      ///< Nodes to exclude from candidate sets (e.g., {X, Y}).
      std::size_t max_cardinality = 0;   ///< Maximum set size (0 = no limit).
      bool        only_minimal    = true;   ///< If true, return only minimal adjustment sets.
    };

    /* ------------------------ Construction ------------------------ */

    /**
     * @brief Construct a DoorCriteria object bound to a given DAG.
     * @param dag Directed acyclic graph representing the causal structure.
     */
    explicit DoorCriteria(const DAG& dag) noexcept : _dag{dag} {}

    /**
     * @brief Accessor for the bound DAG.
     * @return Const reference to the DAG used by this instance.
     */
    inline const DAG& dag() const noexcept { return _dag; }

    /* ------------------------- Backdoor --------------------------- */

    /**
     * @brief Check if Z satisfies the backdoor criterion relative to (X, Y).
     *
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param Z Conditioning set.
     * @return True if Z blocks all backdoor paths from X to Y.
     */
    bool satisfiesBackdoorCriterion(NodeId X, NodeId Y, const NodeSet& Z) const;

    /**
     * @brief Enumerate valid backdoor adjustment sets.
     *
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param opts Enumeration options (excluded nodes, maximal size, minimality).
     * @return Vector of valid backdoor adjustment sets.
     */
    /**
     * @brief Enumerate valid backdoor adjustment sets, with option to stop at first found.
     *
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param opts Enumeration options (excluded nodes, maximal size, minimality).
     * @param stopAtFirst If true, return only the first found set and stop computation early.
     * @return Vector of valid backdoor adjustment sets (at most one if stopAtFirst is true).
     */
    NodeSetVec enumerateBackdoorSets(NodeId                    X,
                                     NodeId                    Y,
                                     const EnumerationOptions& opts,
                                     bool                      stopAtFirst = false) const;

    /**
     * @brief Enumerate valid backdoor adjustment sets with default options, with option to stop at
     * first found.
     *
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param stopAtFirst If true, return only the first found set and stop computation early.
     * @return Vector of valid backdoor adjustment sets (at most one if stopAtFirst is true).
     */
    NodeSetVec enumerateBackdoorSets(NodeId X, NodeId Y, bool stopAtFirst) const;
    NodeSetVec enumerateBackdoorSets(NodeId X, NodeId Y) const;

    /* ------------------------- Frontdoor -------------------------- */

    /**
     * @brief Check if Z satisfies the frontdoor criterion relative to (X, Y).
     *
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param Z Conditioning set.
     * @return True if Z satisfies all three frontdoor conditions (FD-1, FD-2, FD-3).
     */
    bool satisfiesFrontdoorCriterion(NodeId X, NodeId Y, const NodeSet& Z) const;

    /**
     * @brief Enumerate valid frontdoor adjustment sets, with option to stop at first found.
     *
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param opts Enumeration options (excluded nodes, maximal size, minimality).
     * @param stopAtFirst If true, return only the first found set and stop computation early.
     * @return Vector of valid frontdoor adjustment sets (at most one if stopAtFirst is true).
     */
    NodeSetVec enumerateFrontdoorSets(NodeId                    X,
                                      NodeId                    Y,
                                      const EnumerationOptions& opts,
                                      bool                      stopAtFirst = false) const;

    /**
     * @brief Enumerate valid frontdoor adjustment sets with default options, with option to stop at
     * first found.
     *
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param stopAtFirst If true, return only the first found set and stop computation early.
     * @return Vector of valid frontdoor adjustment sets (at most one if stopAtFirst is true).
     */
    NodeSetVec enumerateFrontdoorSets(NodeId X, NodeId Y, bool stopAtFirst) const;
    NodeSetVec enumerateFrontdoorSets(NodeId X, NodeId Y) const;

    /* -------------------------- Utilities ------------------------- */

    /**
     * @brief Check whether there exists a directed path X→…→Y unblocked by Z.
     *
     * @param X Start node.
     * @param Y End node.
     * @param Z Conditioning set (nodes that block traversal if encountered).
     * @return True if an unblocked directed path exists from X to Y.
     */
    bool existsUnblockedDirectedPath(NodeId X, NodeId Y, const NodeSet& Z) const;

    /**
     * @brief Compute nodes lying on some directed path from X to Y.
     *
     * @param X Start node.
     * @param Y End node.
     * @return Set of nodes on at least one X→…→Y directed path.
     */
    NodeSet nodesOnDirectedPaths(NodeId X, NodeId Y) const;

    /**
     * @brief Compute the "backdoor reach" of a node.
     *
     * Backdoor reach = set of nodes reachable from X by traversing one
     * incoming edge (parent step) followed by any sequence of child steps.
     *
     * @param X Node of interest (typically the treatment).
     * @return Set of nodes in the backdoor reach of X.
     */
    NodeSet backdoorReach(NodeId X) const;

    /**
     * @brief Test whether there exists an open backdoor path from X to Y given Z.
     *
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param Z Conditioning set.
     * @return True if there exists at least one unblocked backdoor path.
     */
    bool hasBackdoorPath(NodeId X, NodeId Y, const NodeSet& Z) const;

    private:
    /**
     * @brief Check whether Z is a minimal backdoor adjustment set.
     *
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param Z Candidate conditioning set.
     * @return True if Z is a valid backdoor set and no strict subset is valid.
     */
    bool _isMinimalBackdoorAdjustment(NodeId X, NodeId Y, const NodeSet& Z) const;

    /**
     * @brief Check whether Z is a minimal frontdoor adjustment set.
     *
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param Z Candidate conditioning set.
     * @return True if Z is a valid frontdoor set and no strict subset is valid.
     */
    bool _isMinimalFrontdoorAdjustment(NodeId X, NodeId Y, const NodeSet& Z) const;

    /// Underlying causal DAG (bound reference).
    const DAG& _dag;
  };

}   // namespace gum

#endif   // GUM_DOOR_CRITERIA_H
