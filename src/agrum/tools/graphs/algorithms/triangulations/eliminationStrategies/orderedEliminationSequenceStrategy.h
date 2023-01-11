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
 * @brief An Elimination sequence algorithm that imposes a given complete
 * ordering on the nodes elimination sequence
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H
#define GUM_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H

#include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/eliminationSequenceStrategy.h>
#include <vector>

namespace gum {

  /** @class OrderedEliminationSequenceStrategy
   * @brief An Elimination sequence algorithm that imposes a given complete
   * ordering on the nodes elimination sequence.
   *
   * \ingroup graph_group
   *
   */
  class OrderedEliminationSequenceStrategy: public EliminationSequenceStrategy {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor (uses an empty graph)
    OrderedEliminationSequenceStrategy();

    /// constructor for a (potentially) non empty graph
    /** @param graph the graph to be triangulated, i.e., the nodes of which will
     * be eliminated
     * @param dom_sizes thedomain sizes of the nodes/variables
     * @param order the order in which the nodes should be eliminated
     * @warning Note that we allow dom_sizes and order to be defined over
     * nodes/variables that do not belong to graph. These sizes/nodes will
     * simply be ignored. However, it is compulsory that all the nodes of graph
     * belong to both dom_sizes and order
     * @warning the graph is altered during the triangulation.
     * @warning note that, by aGrUM's rule, the graph and the order are not
     * copied but only referenced by the elimination sequence algorithm. */
    OrderedEliminationSequenceStrategy(UndiGraph*                   graph,
                                       const NodeProperty< Size >*  dom_sizes,
                                       const std::vector< NodeId >* order);

    /// copy constructor
    OrderedEliminationSequenceStrategy(const OrderedEliminationSequenceStrategy& from);

    /// move constructor
    OrderedEliminationSequenceStrategy(OrderedEliminationSequenceStrategy&& from);

    /// destructor
    virtual ~OrderedEliminationSequenceStrategy();

    /** @brief creates a new elimination sequence of the same type as the
     * current object, but this sequence contains only an empty graph
     * @warning you must deallocate by yourself the object returned
     * @return an empty clone of the current object with the same type */
    virtual OrderedEliminationSequenceStrategy* newFactory() const final;

    /// virtual copy constructor
    virtual OrderedEliminationSequenceStrategy* copyFactory() const final;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// sets a new graph to be triangulated
    /** The elimination sequence algorithms reinitializes its data to start a
     * new triangulation with graph Graph
     * @param graph the new graph to be triangulated
     * @param dom_sizes the domain sizes of the nodes/variables
     * @warning Note that we allow dom_sizes to be defined over nodes/variables
     * that do not belong to graph. These sizes will simply be ignored. However,
     * it is compulsory that all the nodes of graph belong to dom_sizes
     * @warning the graph is altered during the triangulation.
     * @warning note that, by aGrUM's rule, the graph and the domain sizes
     * are not copied but only referenced by the elimination sequence algorithm.
     */
    virtual bool setGraph(UndiGraph* graph, const NodeProperty< Size >* dom_sizes) final;

    /// sets the sequence of elimination
    /** @param order the order in which the nodes should be eliminated
     * @return true if the new complete order has been successfully assigned
     * (i.e., if all the nodes of the graph belong to one of the subsets)
     * @warning note that we allow order to contain nodes that do not
     * belong to the current graph to be triangulated: those will simply be
     * ignored. However, all the nodes of the graph need be contained in
     * order.
     * @warning note that, by aGrUM's rule, the graph and the domain sizes
     * are not copied but only referenced by the elimination sequence
     * algorithm. */
    virtual bool setOrder(const std::vector< NodeId >* order) final;

    /// clears the order (to prepare, for instance, a new elimination sequence)
    virtual void clear() final;

    /// returns the new node to be eliminated within the triangulation algorithm
    /** @throws NotFound exception is thrown if there is no more node to
     * eliminate in the graph */
    virtual NodeId nextNodeToEliminate() final;

    /** @brief if the elimination sequence is able to compute fill-ins, we
     * indicate whether we want this feature to be activated
     *
     * @param do_it when true and the elimination sequence has the ability to
     * compute fill-ins, the elimination sequence will actually compute them
     * (for the triangulation to use them), else they will not be available. */
    virtual void askFillIns(bool do_it) final;

    /** @brief indicates whether the fill-ins generated by the eliminated
     * nodes, if needed, will be computed by the elimination sequence, or need
     * be computed by the triangulation itself.
     *
     * An elimination sequence provides fill-ins to its triangulation if and
     * only if it has the ability to compute them and it has been asked to do so
     * (by method askFillIns) */
    virtual bool providesFillIns() const final;

    /** @brief indicates whether the elimination sequence updates by itself the
     * graph after a node has been eliminated */
    virtual bool providesGraphUpdate() const final;

    /// performs all the graph/fill-ins updates provided (if any)
    /** @param node the node the elimination of which requires the graph update
     */
    virtual void eliminationUpdate(const NodeId node) final;

    /** @brief in case fill-ins are provided, this function returns the fill-ins
     * due to all the nodes eliminated so far */
    virtual const EdgeSet& fillIns() final;

    /// returns the current complete ordering
    const std::vector< NodeId >* order() const noexcept;

    /// indicates whether a new complete ordering is needed
    /** if the current complete ordering does not contain all the nodes of the
     * graph or if the graph itself is not defined (nullptr) a new complete
     * ordering will be needed for the next triangulation */
    bool isOrderNeeded() const noexcept;

    /// @}


    private:
    /// the vector indicating in which order we should eliminate the nodes
    const std::vector< NodeId >* _order_{nullptr};

    /// the index in the order indicating the new node to eliminate
    std::size_t _order_index_{std::size_t(0)};

    /// indicate whether a new complete ordering is necessary for the
    /// elimination
    bool _order_needed_{true};


    /// indicates whether an order is compatible with the current graph
    bool _isOrderNeeded_(const std::vector< NodeId >* order) const;
  };


} /* namespace gum */


#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/orderedEliminationSequenceStrategy_inl.h>
#endif   // GUM_NOINLINE


#endif /* GUM_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H */
