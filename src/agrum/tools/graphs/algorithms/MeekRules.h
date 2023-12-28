/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

#include <agrum/tools/graphs/PDAG.h>

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
     * @param _latentCouples__ the set of arc with a latent variable.
     */
    PDAG orientToPDAG(MixedGraph graph, std::vector< Arc >& _latentCouples_);

    /// Propagates the orientation of a MixedGraph and return a DAG.
    /** @param graph the graph in which to which to propagate arcs
     * @param _latentCouples__ the set of arc with a latent variable.
     */
    DAG orientToDAG(MixedGraph graph, std::vector< Arc >& _latentCouples_);

    /// Takes a partially oriented graph and orient all possible edges
    /**
     * @param graph graph in which to which to propagate arcs
     * @param _latentCouples_ the set of arc with a latent variable
     */
    void orientAllEdges(MixedGraph& graph, std::vector< Arc >& _latentCouples_);

    /// Takes a partially oriented graph and propagates the existing orientation
    /**
     * @param graph graph in which to which to propagate arcs
     * @param _latentCouples_ the set of arc with a latent variable
     */
    void propagatesOrientations(MixedGraph& graph, std::vector< Arc >& _latentCouples_);

    private:
    /// Propagates the orientation from a node to its neighbours
    /** @param graph graph in which to which to propagate arcs
     * @param node node on which neighbours to propagate th orientation
     * @param force : true if an orientation has always to be found.
     */
    bool _propagatesRemainingOrientableEdges_(MixedGraph&         graph,
                                              NodeId              xj,
                                              std::vector< Arc >& _latentCouples_);

    /// heuristic for remaining edges when everything else has been tried
    /** @param graph graph in which to which to propagate arcs
     * @param _latentCouples_
     */
    void _propagatesOrientationInChainOfRemainingEdges_(gum::MixedGraph&    graph,
                                                        std::vector< Arc >& _latentCouples_);

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

  };   /// class MeekRules

}   // namespace gum


#endif   // AGRUM_MEEKRULES_H
