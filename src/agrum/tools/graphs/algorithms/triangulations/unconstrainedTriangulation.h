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
 * @brief base class for graph triangulations without constraints on nodes
 * elimination ordering.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_UNCONSTRAINED_TRIANGULATION_H
#define GUM_UNCONSTRAINED_TRIANGULATION_H

#include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/unconstrainedEliminationSequenceStrategy.h>
#include <agrum/tools/graphs/algorithms/triangulations/staticTriangulation.h>

namespace gum {


  /** @class UnconstrainedTriangulation
   * @brief Interface for all triangulation methods without constraints on
   * node elimination orderings
   *
   * \ingroup graph_group
   *
   */
  class UnconstrainedTriangulation: public StaticTriangulation {
    public:
    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /** @brief returns a fresh triangulation (over an empty graph) of the same
     * type as the current object
     *
     * note that we return a pointer as it enables subclasses to return
     * pointers to their types, not Triangulation pointers. See item 25 of the
     * more effective C++.*/
    virtual UnconstrainedTriangulation* newFactory() const = 0;

    /// virtual copy constructor
    /** note that we return a pointer as it enables subclasses to return
     * pointers to their types, not Triangulation pointers. See item 25 of the
     * more effective C++. */
    virtual UnconstrainedTriangulation* copyFactory() const = 0;

    /// destructor
    virtual ~UnconstrainedTriangulation();

    /// @}


    protected:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param elimSeq the elimination sequence used to triangulate the graph
     * @param JTStrategy the junction tree strategy used to create junction
     * trees
     * @param minimality a Boolean indicating whether we should enforce that
     * the triangulation is minimal w.r.t. inclusion */
    UnconstrainedTriangulation(const UnconstrainedEliminationSequenceStrategy& elimSeq,
                               const JunctionTreeStrategy&                     JTStrategy,
                               bool                                            minimality = false);

    /// constructor with a given graph
    /** @param graph the graph to be triangulated, i.e., the nodes of which will
     * be eliminated
     * @param dom the domain sizes of the nodes to be eliminated
     * @param elimSeq the elimination sequence used to triangulate the graph
     * @param JTStrategy the junction tree strategy used to create junction
     * trees
     * @param minimality a Boolean indicating whether we should enforce that
     * the triangulation is minimal w.r.t. inclusion
     * @warning note that, by aGrUM's rule, the graph and the modalities are not
     * copied but only referenced by the elimination sequence algorithm. */
    UnconstrainedTriangulation(const UndiGraph*                                graph,
                               const NodeProperty< Size >*                     dom,
                               const UnconstrainedEliminationSequenceStrategy& elimSeq,
                               const JunctionTreeStrategy&                     JTStrategy,
                               bool                                            minimality = false);

    /// forbid copy constructor except in newfactory
    UnconstrainedTriangulation(const UnconstrainedTriangulation&);

    /// forbid move constructor except in children's constructors
    UnconstrainedTriangulation(UnconstrainedTriangulation&&);

    /// @}


    private:
    /// forbid copy operator
    UnconstrainedTriangulation& operator=(const UnconstrainedTriangulation&);
  };

} /* namespace gum */

#endif /* GUM_UNCONSTRAINED_TRIANGULATION_H */
