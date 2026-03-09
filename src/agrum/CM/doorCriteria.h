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
 * @brief Door criteria (backdoor/frontdoor) utilities for a DAG.
 */
#ifndef GUM_DOOR_CRITERIA_H
#define GUM_DOOR_CRITERIA_H

#include <vector>

#include <agrum/base/graphs/DAG.h>

namespace gum {

  /**
   * @class DoorCriteria
   * @brief Implements Backdoor and Frontdoor criteria utilities for a DAG.
   *
   * This class provides helpers to:
   *  - Check whether a conditioning set satisfies the backdoor/frontdoor criterion.
   *  - Enumerate all valid backdoor/frontdoor adjustment sets.
   *
   * The DAG is passed as the first argument to each method rather than stored
   * in the object, making DoorCriteria a stateless utility class.
   *
   * The implementation mirrors pyAgrum functionality.
   *
   * **Naming correspondences with pyAgrum:**
   *  - enumerateBackdoorSets        => pyagrum: backdoor_generator
   *  - enumerateFrontdoorSets       => pyagrum: frontdoor_generator
   *  - existsUnblockedDirectedPath  => pyagrum: exists_unblocked_directed_path
   *  - nodesOnDirectedPaths         => pyagrum: nodes_on_dipath
   *  - backdoorReach                => pyagrum: backdoor_reach
   *  - hasBackdoorPath              => pyagrum: backdoor_path
   *  - satisfiesBackdoorCriterion   => pyagrum: is_backdoor
   *  - satisfiesFrontdoorCriterion  => pyagrum: is_frontdoor
   */
  class DoorCriteria {
    public:
    /// Convenience type: list of candidate adjustment sets.
    using NodeSetVec = std::vector< NodeSet >;

    /* ------------------------- Backdoor --------------------------- */

    /**
     * @brief Check if Z satisfies the backdoor criterion relative to (X, Y).
     *
     * @param dag Directed acyclic graph representing the causal structure.
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param Z Conditioning set.
     * @return True if Z blocks all backdoor paths from X to Y.
     */
    static bool satisfiesBackdoorCriterion(const DAG& dag, NodeId X, NodeId Y, const NodeSet& Z);

    /**
     * @brief Enumerate valid backdoor adjustment sets.
     *
     * @param dag Directed acyclic graph representing the causal structure.
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param excluded_nodes Nodes to exclude from candidate sets (e.g., latent variables).
     * @param max_cardinality Maximum set size (0 = no limit).
     * @param only_minimal If true, return only minimal adjustment sets.
     * @param stopAtFirst If true, return only the first found set and stop early.
     * @return Vector of valid backdoor adjustment sets (at most one if stopAtFirst is true).
     */
    static NodeSetVec enumerateBackdoorSets(const DAG&     dag,
                                            NodeId         X,
                                            NodeId         Y,
                                            const NodeSet& excluded_nodes  = NodeSet(),
                                            std::size_t    max_cardinality = 0,
                                            bool           only_minimal    = true,
                                            bool           stopAtFirst     = false);

    /**
     * @brief Enumerate valid backdoor adjustment sets — shorthand to set stopAtFirst only.
     *
     * @param dag Directed acyclic graph representing the causal structure.
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param stopAtFirst If true, return only the first found set and stop early.
     * @return Vector of valid backdoor adjustment sets (at most one if stopAtFirst is true).
     */
    static NodeSetVec enumerateBackdoorSets(const DAG& dag, NodeId X, NodeId Y, bool stopAtFirst);

    /* ------------------------- Frontdoor -------------------------- */

    /**
     * @brief Check if Z satisfies the frontdoor criterion relative to (X, Y).
     *
     * @param dag Directed acyclic graph representing the causal structure.
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param Z Conditioning set.
     * @return True if Z satisfies all three frontdoor conditions (FD-1, FD-2, FD-3).
     */
    static bool satisfiesFrontdoorCriterion(const DAG& dag, NodeId X, NodeId Y, const NodeSet& Z);

    /**
     * @brief Enumerate valid frontdoor adjustment sets.
     *
     * @param dag Directed acyclic graph representing the causal structure.
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param excluded_nodes Nodes to exclude from candidate sets (e.g., latent variables).
     * @param max_cardinality Maximum set size (0 = no limit).
     * @param only_minimal If true, return only minimal adjustment sets.
     * @param stopAtFirst If true, return only the first found set and stop early.
     * @return Vector of valid frontdoor adjustment sets (at most one if stopAtFirst is true).
     */
    static NodeSetVec enumerateFrontdoorSets(const DAG&     dag,
                                             NodeId         X,
                                             NodeId         Y,
                                             const NodeSet& excluded_nodes  = NodeSet(),
                                             std::size_t    max_cardinality = 0,
                                             bool           only_minimal    = true,
                                             bool           stopAtFirst     = false);

    /**
     * @brief Enumerate valid frontdoor adjustment sets — shorthand to set stopAtFirst only.
     *
     * @param dag Directed acyclic graph representing the causal structure.
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param stopAtFirst If true, return only the first found set and stop early.
     * @return Vector of valid frontdoor adjustment sets (at most one if stopAtFirst is true).
     */
    static NodeSetVec enumerateFrontdoorSets(const DAG& dag, NodeId X, NodeId Y, bool stopAtFirst);

    /* -------------------------- Utilities ------------------------- */

    /**
     * @brief Check whether there exists a directed path X->..->Y unblocked by Z.
     *
     * @param dag Directed acyclic graph representing the causal structure.
     * @param X Start node.
     * @param Y End node.
     * @param Z Conditioning set (nodes that block traversal if encountered).
     * @return True if an unblocked directed path exists from X to Y.
     */
    static bool existsUnblockedDirectedPath(const DAG& dag, NodeId X, NodeId Y, const NodeSet& Z);

    /**
     * @brief Compute nodes lying on some directed path from X to Y.
     *
     * @param dag Directed acyclic graph representing the causal structure.
     * @param X Start node.
     * @param Y End node.
     * @return Set of nodes on at least one X->..->Y directed path.
     */
    static NodeSet nodesOnDirectedPaths(const DAG& dag, NodeId X, NodeId Y);

    /**
     * @brief Compute the "backdoor reach" of a node.
     *
     * Backdoor reach = set of nodes reachable from X by traversing one
     * incoming edge (parent step) followed by any sequence of child steps.
     *
     * @param dag Directed acyclic graph representing the causal structure.
     * @param X Node of interest (typically the treatment).
     * @return Set of nodes in the backdoor reach of X.
     */
    static NodeSet backdoorReach(const DAG& dag, NodeId X);

    /**
     * @brief Test whether there exists an open backdoor path from X to Y given Z.
     *
     * @param dag Directed acyclic graph representing the causal structure.
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param Z Conditioning set.
     * @return True if there exists at least one unblocked backdoor path.
     */
    static bool hasBackdoorPath(const DAG& dag, NodeId X, NodeId Y, const NodeSet& Z);

    private:
    /**
     * @brief Check whether Z is a minimal backdoor adjustment set.
     *
     * @param dag Directed acyclic graph representing the causal structure.
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param Z Candidate conditioning set.
     * @return True if Z is a valid backdoor set and no strict subset is valid.
     */
    static bool _isMinimalBackdoorAdjustment(const DAG& dag, NodeId X, NodeId Y, const NodeSet& Z);

    /**
     * @brief Check whether Z is a minimal frontdoor adjustment set.
     *
     * @param dag Directed acyclic graph representing the causal structure.
     * @param X Cause variable.
     * @param Y Effect variable.
     * @param Z Candidate conditioning set.
     * @return True if Z is a valid frontdoor set and no strict subset is valid.
     */
    static bool _isMinimalFrontdoorAdjustment(const DAG& dag, NodeId X, NodeId Y, const NodeSet& Z);
  };

}   // namespace gum

#endif   // GUM_DOOR_CRITERIA_H
