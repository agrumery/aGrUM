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
 * @brief Base Class for all the algorithms producing a junction given a set
 * of cliques/subcliques resulting from a triangulation
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_JUNCTION_TREE_STRATEGY_H
#define GUM_JUNCTION_TREE_STRATEGY_H

#include <agrum/agrum.h>

#include <agrum/tools/graphs/cliqueGraph.h>

namespace gum {

  class StaticTriangulation;

  /** @class JunctionTreeStrategy
   * @brief Base Class for all the algorithms producing a junction given a set
   * of cliques/subcliques resulting from a triangulation.
   *
   * \ingroup graph_group
   */
  class JunctionTreeStrategy {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// destructor
    virtual ~JunctionTreeStrategy();

    /// create a clone not assigned to any triangulation algorithm
    virtual JunctionTreeStrategy* newFactory() const = 0;

    /// virtual copy constructor
    /** @param triangulation if triangulation is different from nullptr, this
     * becomes the new triangulation algorithm associated with the junction tree
     * strategy */
    virtual JunctionTreeStrategy* copyFactory(StaticTriangulation* triangulation = nullptr) const
       = 0;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /** @brief indicates whether the junction tree strategy needs fill-ins
     * to work properly
     *
     * If the junctionTreeStrategy needs fill-ins to work properly, its
     * assigned triangulation instance (see method setTriangulation) will be
     * commited to compute them. */
    virtual bool requiresFillIns() const = 0;

    /// returns the junction tree computed
    /** The idea behind this method is that the JunctionTreeStrategy asks its
     * assigned triangulation (see method setTriangulation) all the data it
     * needs to compute correctly the junction tree. For instance, it may asks
     * for a triangulated graph or an elimination tree, or even the order of
     * elimination of the nodes, etc. All these data are available from the
     * triangulation class. Knowing these data, the junctionTreeStrategy
     * computes and returns a junction tree corresponding to the
     * triangulated graph. */
    virtual const CliqueGraph& junctionTree() = 0;

    /// assigns the triangulation to the junction tree strategy
    virtual void setTriangulation(StaticTriangulation* triangulation) = 0;

    /** @brief assigns a new triangulation to the junction tree strategy
     * during a move construction */
    virtual void moveTriangulation(StaticTriangulation* triangulation);

    /** @brief returns, for each node, the clique of the junction tree which was
     * created by its deletion */
    virtual const NodeProperty< NodeId >& createdCliques() = 0;

    /** @brief returns the Id of the clique of the junction tree created by the
     * elimination of a given node during the triangulation process */
    virtual NodeId createdClique(const NodeId id) = 0;

    /// resets the current junction tree strategy data structures
    virtual void clear() = 0;

    /// @}


    protected:
    /// the triangulation to which the junction tree is associated
    StaticTriangulation* triangulation_{nullptr};


    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    JunctionTreeStrategy();

    /// copy constructor
    JunctionTreeStrategy(const JunctionTreeStrategy& from);

    /// move constructor
    JunctionTreeStrategy(JunctionTreeStrategy&& from);

    /// @}
  };

} /* namespace gum */

#endif /* GUM_JUNCTION_TREE_STRATEGY_H */
