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
 * @brief An Elimination sequence algorithm that imposes a given partial
 * ordering on the nodes elimination sequence
 *
 * Class DefaultPartialOrderedEliminationSequenceStrategy implements
 * an elimination sequence algorithm that satisfies a partial ordering, that
 * is, the set of all the nodes is divided into several subsets. All the nodes
 * of the first subset must be eliminated before the nodes of the second,
 * which must be eliminated before those of the third subset, and so on. Within
 * a subset, the ordering is determined as follows:
 * # the nodes that are simplicial (i.e., those that already form a clique with
 *   their neighbors) are eliminated first
 * # then the nodes that are almost simplicial (i.e. if we remove one of their
 *   neighbors, they become simplicial) and that create small cliques, are
 *   eliminated (see Bodlaender's safe reductions)
 * # the quasi simplicial nodes (i.e., the nodes that do not require many
 *   fill-ins to create cliques) that would create small cliques, are eliminated
 * # finally, the heuristic proposed by Kjaerulff(90) is used to compute the
 *   last nodes to be eliminated.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_DEFAULT_PARTIAL_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H
#define GUM_DEFAULT_PARTIAL_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H

#include <agrum/tools/graphs/algorithms/simplicialSet.h>
#include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/partialOrderedEliminationSequenceStrategy.h>

namespace gum {

  /** @class DefaultPartialOrderedEliminationSequenceStrategy
   * @brief An Elimination sequence algorithm that imposes a given partial
   * ordering on the nodes elimination sequence
   *
   * Class DefaultPartialOrderedEliminationSequenceStrategy implements
   * an elimination sequence algorithm that satisfies a partial ordering, that
   * is, the set of all the nodes is divided into several subsets. All the nodes
   * of the first subset must be eliminated before the nodes of the second,
   * which must be eliminated before those of the third subset, and so on.
   * Within
   * a subset, the ordering is determined as follows:
   * # the nodes that are simplicial (i.e., those that already form a clique
   * with
   *   their neighbors) are eliminated first
   * # then the nodes that are almost simplicial (i.e. if we remove one of their
   *   neighbors, they become simplicial) and that create small cliques, are
   *   eliminated (see Bodlaender's safe reductions)
   * # the quasi simplicial nodes (i.e., the nodes that do not require many
   *   fill-ins to create cliques) that would create small cliques, are
   * eliminated
   * # finally, the heuristic proposed by Kjaerulff(90) is used to compute the
   *   last nodes to be eliminated.
   *
   * \ingroup graph_group
   *
   */
  class DefaultPartialOrderedEliminationSequenceStrategy:
      public PartialOrderedEliminationSequenceStrategy {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor (uses an empty graph)
    /** @param theRatio the ratio used by the SimplicialSet included in the
     * DefaultEliminationSequenceStrategy
     * @param theThreshold the weight threshhold of the SimplicialSet included
     * in the DefaultEliminationSequenceStrategy */
    DefaultPartialOrderedEliminationSequenceStrategy(double theRatio     = GUM_QUASI_RATIO,
                                                     double theThreshold = GUM_WEIGHT_THRESHOLD);

    /// constructor for a (potentially) non empty graph
    /** @param graph the graph to be triangulated, i.e., the nodes of which will
     * be eliminated
     * @param dom_sizes thedomain sizes of the nodes/variables
     * @param subsets the list of the subsets constituting the partial ordering
     * @param ratio the ratio used by the SimplicialSet included in the
     * DefaultEliminationSequenceStrategy
     * @param threshold the weight threshhold of the SimplicialSet included in
     * the DefaultEliminationSequenceStrategy
     * @warning Note that we allow dom_sizes to be defined over nodes/variables
     * that do not belong to graph. These sizes will simply be ignored. However,
     * it is compulsory that all the nodes of graph belong to dom_sizes
     * @warning the graph is altered during the triangulation.
     * @warning note that, by aGrUM's rule, the graph, the domain sizes and
     * the sequence are not copied but only referenced by the elimination
     * sequence algorithm. */
    DefaultPartialOrderedEliminationSequenceStrategy(UndiGraph*                  graph,
                                                     const NodeProperty< Size >* dom_sizes,
                                                     const List< NodeSet >*      subsets,
                                                     double ratio     = GUM_QUASI_RATIO,
                                                     double threshold = GUM_WEIGHT_THRESHOLD);

    /// copy constructor
    /** @warning The newly created elimination sequence strategy points toward
     * the same undirected graph as the one contained in from but each strategy
     * possesses its own simplicial set. As a result, if both elimination
     * strategies are used at the same time, they will probably result in a mess
     * because their simplicial sets won't be synchronized correctly with the
     * changing undirected graph. So, whenever using this copy constructor, be
     * sure that either from or the newly created strategy is used for a
     * triangulation but not both. This will necessarily be OK in
     * DefaultTriangulations. */
    DefaultPartialOrderedEliminationSequenceStrategy(
       const DefaultPartialOrderedEliminationSequenceStrategy& from);

    /// move constructor
    DefaultPartialOrderedEliminationSequenceStrategy(
       DefaultPartialOrderedEliminationSequenceStrategy&& from);

    /// destructor
    virtual ~DefaultPartialOrderedEliminationSequenceStrategy();

    /** @brief creates a new elimination sequence of the same type as the
     * current object, but this sequence contains only an empty graph
     * @warning you must deallocate by yourself the object returned
     * @return an empty clone of the current object with the same type */
    virtual DefaultPartialOrderedEliminationSequenceStrategy* newFactory() const final;

    /// virtual copy constructor
    /** @warning The newly created elimination sequence strategy points toward
     * the same undirected graph as the one contained in the current strategy
     * but each strategy possesses its own simplicial set. As a result, if both
     * elimination strategies are used at the same time, they will probably
     * result in a mess because their simplicial sets won't be synchronized
     * correctly with the changing undirected graph. So, whenever using this
     * virtual copy constructor, be sure that either the current or the newly
     * created strategy is used for a triangulation but not both. This will
     * necessarily be OK in DefaultTriangulations. */
    virtual DefaultPartialOrderedEliminationSequenceStrategy* copyFactory() const final;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// sets a new graph to be triangulated
    /** The elimination sequence algorithms reinitializes its data to start a
     * new triangulation with graph Graph and a new partial ordrering
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

    /// clears the sequence (to prepare, for instance, a new elimination
    /// sequence)
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

    /// @}

    private:
    /// for each node, the weight of the clique created by the node's
    /// elimination
    NodeProperty< double > _log_weights_;

    /// the simplicial set used for determining the best nodes to eliminate
    SimplicialSet* _simplicial_set_{nullptr};

    /// the ratio used by  _simplicial_set_ for its quasi-simplicial nodes
    double _simplicial_ratio_;

    /// the threshold used by   _simplicial_set_ to determine small cliques
    double _simplicial_threshold_;

    /// indicates whether we compute new fill-ins
    bool _provide_fill_ins_{false};


    /// returns the best possible node to be eliminated
    /** this function is used by method nextNodeToEliminate */
    NodeId _nodeToEliminate_(const PriorityQueue< NodeId, double >& possibleNodes);

    /// create a new simplicial set suited for the current graph
    void _createSimplicialSet_();
  };

} /* namespace gum */

#endif /* GUM_DEFAULT_PARTIAL_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H */
