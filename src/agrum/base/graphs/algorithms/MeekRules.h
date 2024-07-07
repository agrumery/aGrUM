/**
 *
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
