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
/** @file
 * @brief base class for graph triangulations without constraints on nodes
 * elimination ordering.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_UNCONSTRAINED_TRIANGULATION_H
#define GUM_UNCONSTRAINED_TRIANGULATION_H

#include <agrum/graphs/triangulations/staticTriangulation.h>
#include <agrum/graphs/eliminations/unconstrainedEliminationSequenceStrategy.h>

namespace gum {

  /* =========================================================================== */
  /** @class UnconstrainedTriangulation
   * @brief Interface for all triangulation methods without constraints on
   * node elimination orderings
   *
   * \ingroup graph_group
   *
   */
  /* =========================================================================== */
  class UnconstrainedTriangulation : public StaticTriangulation {
    public:
    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// initialize the triangulation data structures for a new graph
    /** @param graph the graph to be triangulated, i.e., the nodes of which will
     * be eliminated
     * @param dom the domain sizes of the nodes to be eliminated
     * @warning note that, by aGrUM's rule, the graph and the modalities are not
     * copied but only referenced by the elimination sequence algorithm. */
    virtual void setGraph(const UndiGraph *theGraph,
                          const NodeProperty<Size> *modal);

    /** @brief returns a fresh triangulation (over an empty graph) of the same
     * type as the current object
     *
     * note that we return a pointer as it enables subclasses to return
     * pointers to their types, not Triangulation pointers. See item 25 of the
     * more effective C++.*/
    virtual UnconstrainedTriangulation *newFactory() const = 0;

    /// @}

    /// destructor
    virtual ~UnconstrainedTriangulation();

    protected:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param elimSeq the elimination sequence used to triangulate the graph
     * @param JTStrategy the junction tree strategy used to create junction trees
     * @param minimality a Boolean indicating whether we should enforce that
     * the triangulation is minimal w.r.t. inclusion */
    UnconstrainedTriangulation(
        const UnconstrainedEliminationSequenceStrategy &elimSeq,
        const JunctionTreeStrategy &JTStrategy, bool minimality = false);

    /// constructor with a given graph
    /** @param graph the graph to be triangulated, i.e., the nodes of which will
     * be eliminated
     * @param dom the domain sizes of the nodes to be eliminated
     * @param elimSeq the elimination sequence used to triangulate the graph
     * @param JTStrategy the junction tree strategy used to create junction trees
     * @param minimality a Boolean indicating whether we should enforce that
     * the triangulation is minimal w.r.t. inclusion
     * @warning note that, by aGrUM's rule, the graph and the modalities are not
     * copied but only referenced by the elimination sequence algorithm. */
    UnconstrainedTriangulation(
        const UndiGraph *graph, const NodeProperty<Size> *dom,
        const UnconstrainedEliminationSequenceStrategy &elimSeq,
        const JunctionTreeStrategy &JTStrategy, bool minimality = false);

    /// the function called to initialize the triangulation process
    /** This function is called when the triangulation process starts and is
     * used to initialize the elimination sequence strategy. Actually, the
     * graph that is modified by the triangulation algorithm is a copy of
     * the original graph, and this copy need be known by the elimination
     * sequence strategy. _initTriangulation is used to transmit this
     * knowledge to the elimination sequence (through method setGraph of the
     * elimination sequence class).
     * @param graph the very graph that is triangulated (this is a copy of
     * __original_graph) */
    void _initTriangulation(UndiGraph &graph);

    /// @}

    private:
    /// forbid copy constructor
    UnconstrainedTriangulation(const UnconstrainedTriangulation &);

    /// forbid copy operator
    UnconstrainedTriangulation &operator=(const UnconstrainedTriangulation &);
  };

} /* namespace gum */

#endif /* GUM_UNCONSTRAINED_TRIANGULATION_H */
