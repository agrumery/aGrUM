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
 * @brief An Elimination sequence algorithm that imposes a given partial ordering
 * on the nodes elimination sequence, that is, the set of all the nodes is divided
 * into several subsets. All the nodes of the first subset must be eliminated
 * before the nodes of the second, which must be eliminated before those of the
 * third subset, and so on. Within a subset, the ordering is determined as follows:
 * # the nodes that are simplicial (i.e., those that already form a clique with
 * their neighbours) are eliminated first
 * # then the nodes that are almost simplicial (i.e. if we remove one of their
 * neighbours, they become simplicial) and that create small cliques, are
 * eliminated
 * # the quasi simplicial nodes (i.e., nodes that do not require many fill-ins to
 * create cliques) that would create small cliques, are eliminated
 * # finally, the heuristic proposed by Kjaerulff(90) is used to compute the last
 * nodes to be eliminated.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_DEFAULT_PARTIAL_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H
#define GUM_DEFAULT_PARTIAL_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H

#include <agrum/core/priorityQueue.h>
#include <agrum/graphs/graphElements.h>
#include <agrum/graphs/simplicialSet.h>
#include <agrum/graphs/eliminations/partialOrderedEliminationSequenceStrategy.h>

namespace gum {

  /** @class DefaultPartialOrderedEliminationSequenceStrategy
   * @brief An Elimination sequence algorithm that imposes a given partial ordering
   * on the nodes elimination sequence, that is, the set of all the nodes is
   * divided into several subsets. All the nodes of the first subset must be
   * eliminated before the nodes of the second, which must be eliminated before
   * those of the third subset, and so on. Within a subset, the ordering is
   * determined as follows:
   * # the nodes that are simplicial (i.e., those that already form a clique with
   * their neighbours) are eliminated first
   * # then the nodes that are almost simplicial (i.e. if we remove one of their
   * neighbours, they become simplicial) and that create small cliques, are
   * eliminated
   * # the quasi simplicial nodes (i.e., nodes that do not require many fill-ins to
   * create cliques) that would create small cliques, are eliminated
   * # finally, the heuristic proposed by Kjaerulff(90) is used to compute the last
   * nodes to be eliminated.
   *
   * \ingroup graph_group
   *
   */
  class DefaultPartialOrderedEliminationSequenceStrategy
      : public PartialOrderedEliminationSequenceStrategy {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor (uses an empty graph)
    /** @param theRatio the ratio used by the SimplicialSet included in the
     * DefaultEliminationSequenceStrategy
     * @param theThreshold the weight threshhold of the SimplicialSet included in
     * the DefaultEliminationSequenceStrategy */
    DefaultPartialOrderedEliminationSequenceStrategy(
        float theRatio = GUM_QUASI_RATIO, float theThreshold = GUM_WEIGHT_THRESHOLD);

    /// constructor for an a priori non empty graph
    /** @param graph the graph to be triangulated, i.e., the nodes of which will
     * be eliminated
     * @param dom the modalities of the nodes (i.e., their domain sizes)
     * @param subsets the list of the subsets constituting the partial ordering
     * @param ratio the ratio used by the SimplicialSet included in the
     * DefaultEliminationSequenceStrategy
     * @param threshold the weight threshhold of the SimplicialSet included in
     * the DefaultEliminationSequenceStrategy
     * @warning note that, by aGrUM's rule, the graph and the sequence are not
     * copied but only referenced by the elimination sequence algorithm. */
    DefaultPartialOrderedEliminationSequenceStrategy(
        UndiGraph *graph, const NodeProperty<Size> *dom,
        const List<NodeSet> *subsets, float ratio = GUM_QUASI_RATIO,
        float threshold = GUM_WEIGHT_THRESHOLD);

    /// copy constructor
    DefaultPartialOrderedEliminationSequenceStrategy(
        const DefaultPartialOrderedEliminationSequenceStrategy &);

    /// destructor
    virtual ~DefaultPartialOrderedEliminationSequenceStrategy();

    /** @brief creates a new elimination sequence of the same type as the current
     * object, but this sequence contains only an empty graph
     * @warning you must deallocate by yourself the object returned
     * @return an empty clone of the current object with the same type */
    virtual DefaultPartialOrderedEliminationSequenceStrategy *newFactory() const;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// sets a new graph to be triangulated
    /** The elimination sequence algorithms reinitializes its data to start a new
     * triangulation with graph Graph
     * @param graph the new graph to be triangulated
     * @param dom the modalities of the nodes (i.e., their domain sizes)
     * @param subsets the list of the subsets constituting the partial ordering
     * @warning note that, by aGrUM's rule, the graph and the sequence are not
     * copied but only referenced by the elimination sequence algorithm. */
    void setGraph(UndiGraph *graph, const NodeProperty<Size> *dom,
                  const List<NodeSet> *subsets);

    /// clears the sequence (to prepare, for instance, a new elimination sequence)
    void clear();

    /// returns the new node to be eliminated within the triangulation algorithm
    /** @throws NotFound exception is thrown if there is no more node to
     * eliminate in the graph */
    NodeId nextNodeToEliminate();

    /** @brief if the elimination sequence is able to compute fill-ins, we indicate
     * whether we want this feature to be activated
     *
     * @param do_it when true and the elimination sequence has the ability to
     * compute fill-ins, the elimination sequence will actually compute them (for
     * the triangulation to use them), else they will not be available. */
    void askFillIns(bool do_it);

    /** @brief indicates whether the fill-ins generated by the eliminated
     * nodes, if needed, will be computed by the elimination sequence, or need be
     * computed by the triangulation itself.
     *
     * An elimination sequence provides fill-ins to its triangulation if and
     * only if it has the ability to compute them and it has been asked to do so
     * (by method askFillIns) */
    bool providesFillIns() const;

    /** @brief indicates whether the elimination sequence updates by itself the
     * graph after a node has been eliminated */
    bool providesGraphUpdate() const;

    /// performs all the graph/fill-ins updates provided (if any)
    /** @param node the node the elimination of which requires the graph update */
    void eliminationUpdate(const NodeId node);

    /** @brief in case fill-ins are provided, this function returns the fill-ins
     * due to all the nodes eliminated so far */
    const EdgeSet &fillIns();

    /// @}

    private:
    /// the graph to triangulate
    UndiGraph *__graph;

    /// the modalities of the nodes (domain sizes)
    const NodeProperty<Size> *__modalities;

    /// the log of the modalities of the nodes (domain sizes)
    NodeProperty<float> __log_modalities;

    /// for each node, the weight of the clique created by the node's elimination
    NodeProperty<float> __log_weights;

    /// the subsets constituting the partial ordering
    const List<NodeSet> *__subsets;

    /// the iterator indicating which is the current subset on which we work
    List<NodeSet>::const_iterator __subset_iter;

    /// the nodes which can be currently eliminated
    NodeSet __nodeset;

    /// the simplicial set used for determining the best nodes to eliminate
    SimplicialSet *__simplicial_set;

    /// the ratio used by __simplicial_set for its quasi-simplicial nodes
    float __simplicial_ratio;

    /// the threshold used by  __simplicial_set to determine small cliques
    float __simplicial_threshold;

    /// indicates whether we compute new fill-ins
    bool __provide_fill_ins;

    /// returns the best possible node to be eliminated
    /** this function is used by method nextNodeToEliminate */
    NodeId __nodeToEliminate(const PriorityQueue<NodeId, float> &possibleNodes);
  };

} /* namespace gum */

#endif /* GUM_DEFAULT_PARTIAL_ORDERED_ELIMINATION_SEQUENCE_STRATEGY_H */
