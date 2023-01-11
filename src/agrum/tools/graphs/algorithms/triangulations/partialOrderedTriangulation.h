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
 * @brief class for graph triangulations for which we enforce a given partial
 * ordering on the nodes eliminations, that is, the set of all the nodes is
 * divided into several subsets. Within each subset, any ordering can be chosen.
 * But all the nodes of the first subset must be eliminated before the nodes
 * of the second, which must be eliminated before those of the third subset,
 * and so on.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_PARTIAL_ORDERED_TRIANGULATION_H
#define GUM_PARTIAL_ORDERED_TRIANGULATION_H

#include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/defaultPartialOrderedEliminationSequenceStrategy.h>
#include <agrum/tools/graphs/algorithms/triangulations/junctionTreeStrategies/defaultJunctionTreeStrategy.h>
#include <agrum/tools/graphs/algorithms/triangulations/staticTriangulation.h>


namespace gum {


  /** @class PartialOrderedTriangulation
   * @brief class for graph triangulations for which we enforce a given partial
   * ordering on the nodes eliminations, that is, the set of all the nodes is
   * divided into several subsets. Within each subset, any ordering can be
   * chosen. But all the nodes of the first subset must be eliminated before
   * the nodes of the second, which must be eliminated before those of the third
   * subset, and so on.
   *
   * \ingroup graph_group
   */
  class PartialOrderedTriangulation: public StaticTriangulation {
    public:
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
    PartialOrderedTriangulation(const PartialOrderedEliminationSequenceStrategy& elimSeq
                                = DefaultPartialOrderedEliminationSequenceStrategy(),
                                const JunctionTreeStrategy& JTStrategy
                                = DefaultJunctionTreeStrategy(),
                                bool minimality = false);

    /// constructor with a given graph
    /** @param graph the graph to be triangulated, i.e., the nodes of which will
     * be eliminated
     * @param domsizes the domain sizes of the nodes to be eliminated
     * @param partial_order the list of the subsets constituting the partial
     * ordering
     * @param elimSeq the elimination sequence used to triangulate the graph
     * @param JTStrategy the junction tree strategy used to create junction
     * trees
     * @param minimality a Boolean indicating whether we should enforce that
     * the triangulation is minimal w.r.t. inclusion
     * @warning Note that we allow dom_sizes to be defined over nodes/variables
     * that do not belong to graph. These sizes will simply be ignored. However,
     * it is compulsory that all the nodes of graph belong to dom_sizes
     * @warning note that, by aGrUM's rule, the graph, the domain sizes and the
     * partial ordering are not copied but only referenced by the
     * triangulation algorithm. */
    PartialOrderedTriangulation(const UndiGraph*                                 graph,
                                const NodeProperty< Size >*                      domsizes,
                                const List< NodeSet >*                           partial_order,
                                const PartialOrderedEliminationSequenceStrategy& elimSeq
                                = DefaultPartialOrderedEliminationSequenceStrategy(),
                                const JunctionTreeStrategy& JTStrategy
                                = DefaultJunctionTreeStrategy(),
                                bool minimality = false);

    /// copy constructor
    PartialOrderedTriangulation(const PartialOrderedTriangulation& from);

    /// move constructor
    PartialOrderedTriangulation(PartialOrderedTriangulation&& from);

    /** @brief returns a fresh triangulation (over an empty graph) of the same
     * type as the current object
     *
     * note that we return a pointer as it enables subclasses to return
     * pointers to their types, not Triangulation pointers. See item 25 of the
     * more effective C++.*/
    virtual PartialOrderedTriangulation* newFactory() const;

    /// virtual copy constructor
    virtual PartialOrderedTriangulation* copyFactory() const final;

    /// destructor
    virtual ~PartialOrderedTriangulation();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// initialize the triangulation data structures for a new graph
    /** @param graph the graph to be triangulated, i.e., the nodes of which will
     * be eliminated
     * @param domsizes the domain sizes of the nodes to be eliminated
     * @param sequence  the order in which the nodes should be eliminated
     * @warning note that, by aGrUM's rule, the graph and the domain sizes are
     * not
     *  notcopied but only referenced by the triangulation algorithm. */
    virtual void setGraph(const UndiGraph* graph, const NodeProperty< Size >* domsizes) final;

    /// sets the elimination sequence's partial order (only a reference is
    /// stored)
    /** The elimination sequence is kept outside this class for efficiency
     * reasons. */
    virtual void setPartialOrder(const List< NodeSet >* partial_order) final;

    /// @}


    protected:
    /// the function called to initialize the triangulation process
    /** This function is called when the triangulation process starts and is
     * used to initialize the elimination sequence strategy. Actually, the
     * graph that is modified by the triangulation algorithm is a copy of
     * the original graph, and this copy need be known by the elimination
     * sequence strategy. initTriangulation_ is used to transmit this
     * knowledge to the elimination sequence (through method setGraph of the
     * elimination sequence class).
     * @param graph the very graph that is triangulated (this is a copy of
     * original_graph_) */
    virtual void initTriangulation_(UndiGraph& graph) final;

    /// the partial ordering to apply to eliminate nodes
    const List< NodeSet >* _partial_order_{nullptr};

    /// @}

    private:
    /// forbid copy operator
    PartialOrderedTriangulation& operator=(const PartialOrderedTriangulation&);
  };

} /* namespace gum */

#endif /* GUM_PARTIAL_ORDERED_TRIANGULATION_H */
