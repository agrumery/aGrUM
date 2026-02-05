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


/** @file
 * @brief Class for Meek Rules
 *
 * @author Mahdi Hadj Ali and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef AGRUM_MEEKRULES_H
#define AGRUM_MEEKRULES_H

#include <agrum/agrum.h>

#include <agrum/base/graphs/PDAG.h>

namespace gum {
  class MeekRules {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    MeekRules();

    /// destructor
    virtual ~MeekRules();

    /// @}

    /// Propagates the orientation of a MixedGraph (no double-headed arcs) and return a PDAG.
    /** @param graph the graph in which to which to propagate arcs
     * @warning propagates may create double arcs
     */
    MixedGraph propagate(const MixedGraph& mg);

    /// Propagates the orientation of a MixedGraph (no double-headed arcs) and return a PDAG.
    /** @param graph the graph in which to which to propagate arcs
     * @warning propagateToCPDAG may have to select between double arcs created by propagates
     */
    PDAG propagateToCPDAG(const MixedGraph& mg);

    /// Propagates the orientation of a MixedGraph and completes it as a DAG.
    /** @param graph the graph in which to which to propagate arcs
     * @warning propagateToDAG may have to select between double arcs created by propagates
     */
    DAG propagateToDAG(const MixedGraph& mg);

    /**
     * @brief Get the Choices object : the list of arcs that the algorithm has to choose from a
     * double arc or a double non-arc.
     *
     * @return  std::vector< Arc >
     */
    std::vector< Arc > choices() const { return _choices_; };

    private:
    MixedGraph _propagates_(const MixedGraph& graph);

    std::vector< Arc > _choices_;

    /// Propagates the orientation from a node to its neighbours
    /** @param graph graph in which to which to propagate arcs
     * @param node node on which neighbours to propagate th orientation
     * @param force : true if an orientation has always to be found.
     */
    bool _applyMeekRules_(MixedGraph& graph, NodeId xj);

    /// heuristic for remaining edges when everything else has been tried
    /** @param graph graph in which to which to propagate arcs
     * @param _latentCouples_
     */
    void _propagatesOrientationInChainOfRemainingEdges_(gum::MixedGraph& graph);

    /** Tells us if we can orient the edge xi - xj to xi -> xj
     * @param graph the graph
     * @param xi the tail of the arc
     * @param xj the head of the arc
     * @return true if we can orient xi to xj
     */

    /// Gets the orientation probabilities like MIIC for the orientation phase
    /**@param mg the MixedGraph the graph from which the double headed arcs will be oriented.
     */
    void _orientDoubleHeadedArcs_(MixedGraph& mg);

    bool _isOrientable_(const gum::MixedGraph& graph, gum::NodeId xi, gum::NodeId xj) const;

    static bool _existsDirectedPath_(const MixedGraph& graph, NodeId n1, NodeId n2);

    void _complete_(MixedGraph& graph);

    /**
     * @brief
     * When resolving double-headed arcs, prioritize selecting the option that minimizes the number
     * of parent nodes in the graph.
     *
     * @return  gum::Arc the arc that should be earased.
     */
    gum::Arc _critereMinParents_(const gum::MixedGraph& graph, gum::NodeId x, gum::NodeId y);
  };   /// class MeekRules

}   // namespace gum


#endif   // AGRUM_MEEKRULES_H
