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
 * @brief Class for fast retrieval of simplicial and quasi/almost simplicial
 * nodes
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_SIMPLICIAL_SET_H
#define GUM_SIMPLICIAL_SET_H

#include <iostream>
#include <utility>

#include <agrum/agrum.h>

#include <agrum/tools/core/priorityQueue.h>
#include <agrum/tools/graphs/cliqueGraph.h>

#define GUM_QUASI_RATIO      0.99
#define GUM_WEIGHT_THRESHOLD 0.0

namespace gum {

  /* ===========================================================================
   */
  /* ===  CLASS FOR RETRIEVING SIMPLICIAL, ALMOST AND QUASI SIMPLICIAL NODES ===
   */
  /* ===========================================================================
   */
  /** @class SimplicialSet
   * @brief Class enabling fast retrieval of simplicial, quasi and almost
   * simplicial nodes
   *
   * \ingroup graph_group
   *
   */
  /* ===========================================================================
   */
  class SimplicialSet {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// constructor. initializes the simplicial set w.r.t. a given graph
    /** creates a class for managing the simplicial sets of a given undirected
     * graph. When we add or remove nodes or edges within this undirected graph,
     * the simplicial set updates its list of simplicial, almost simplicial
     * and quasi simplicial sets. Recall that a node is simplicial if, along
     * with
     * its neighbors, it forms a clique. A node X is almost simplicial if it
     * has a neighbor, say Y, such that, after removing Y, X and its neighbors
     * form a clique.
     *
     * @param graph The undirected graph the simplicial sets of which we are
     * interested in.
     * @param log_domain_sizes The logarithm of the domain sizes of the
     * nodes/variables. This is used for two different reasons: i) it enables
     * to retrieve the simplicial nodes that have the smallest domain size
     * (useful for triangulations); and ii) it enables to compute and update
     * the log-weight of the cliques containing the nodes (the log-weight of a
     * clique is the sum of the log_domain_sizes of its nodes).
     * @param log_weights The logarithm of the weights of cliques.
     * @param theRatio Let L be the number of edges between neighbors of a
     * given node and let L' be the number of all the possible edges between
     * these neighbors (n * (n+1) / 2). If L/L' >= theRatio, then we consider
     * that the node and its neighbors quasi form a clique and, hence is a
     * quasi-simplicial node.
     * @param theThreshold for a safe triangulation (see Bodlaender), almost and
     * quasi-simplicial nodes should not be eliminated, unless their weight is
     * lower than the highest weight of the cliques created so far. Here, we
     * consider it safe if the weight of a new clique is lower than
     * (1+theThreshold) * this highest weight. This enables flexibility.
     * @warning  Note that, by the aGrUM's constructor parameter's rule, the
     * fact that an argument is passed as a pointer means that it is not copied
     * within the SimplicialSet, but rather it is only referenced within it.
     * @throws OperationNotAllowed exception is thrown if the graph, the
     * log_modalities or the log_weights are null pointers.
     * @warning Note that we allow log_domain_sizes to be defined over
     * nodes/variables that do not belong to graph. These sizes will simply be
     * ignored. However, it is compulsory that all the nodes of graph belong to
     * log_domain_sizes. */
    explicit SimplicialSet(UndiGraph*                    graph,
                           const NodeProperty< double >* log_domain_sizes,
                           NodeProperty< double >*       log_weights,
                           double                        theRatio     = GUM_QUASI_RATIO,
                           double                        theThreshold = GUM_WEIGHT_THRESHOLD);

    /// copy constructor
    /** The constructor tries to make a copy of simplicial_from. In addition, it
     * requires a graph that is a perfect copy of that of simplicial_from, as
     * well as perfect copies of the log domain sizes and weights of
     * simplicial_from. This requirement is necessary to avoid a mess: as the
     * graph, the log domain sizes and the log weights are the only data that
     * are not copied into the SimplicialSet, creating a copy of simplicial_from
     * without using, say, a new graph would result in the new SimplicialSet
     * asserting that some nodes are simplicial while they are not because the
     * graph has been changed by simplicial_from. With these new copies, this
     * kind of case cannot occur.
     * @param simplicial_from the simplicial set we wish to copy
     * @param graph The undirected graph the simplicial sets of which we are
     * interested in. It should be identical to that used by simplicial_from.
     * @param log_domain_sizes The logarithm of the domain sizes of the
     * nodes/variabless. This is used for two different reasons: i) it enables
     * to retrieve the simplicial nodes that have the smallest domain sizes
     * (useful for triangulations); and ii) it enables to compute and update
     * the log-weight of the cliques containing the nodes (the log-weight of a
     * clique is the sum of the log_domain_sizes of its nodes).
     * log_domain_sizes should be identical to that used by simplicial_from.
     * @param log_weights The logarithm of the weights of the cliques.
     * @param avoid_check if this Boolean is set to true, the SimplicialSet will
     * not check whether the graph, the log_domain_sizes and the log_weights
     * are OK. It will simply assume that everything is OK. Never use this
     * unless
     * you know what you do: setting avoid_check to true results in a faster
     * constructor but can also lead to a mess that is quite complicated to fix.
     * @warning Note that, by the aGrUM's constructor parameter's rule, the
     * fact that an argument is passed as a pointer means that it is not copied
     * within the SimplicialSet, but rather it is only referenced within it.
     * @warning Note that we allow log_domain_sizes to be defined over
     * nodes/variables that do not belong to graph. These sizes will simply be
     * ignored. However, it is compulsory that all the nodes of graph belong to
     * log_domain_sizes.
     * @throws OperationNotAllowed exception is thrown if the graph, the
     * log_domain_sizes or the log_weights are null pointers, or if these data
     * are
     * different from those stored into simplicial_from */
    SimplicialSet(const SimplicialSet&          simplicial_from,
                  UndiGraph*                    graph,
                  const NodeProperty< double >* log_domain_sizes,
                  NodeProperty< double >*       log_weights,
                  bool                          avoid_check = false);

    /// move constructor
    SimplicialSet(SimplicialSet&& from);

    /// destructor
    ~SimplicialSet();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// adds the necessary edges so that node 'id' and its neighbors form a
    /// clique
    /** @param id the node which will form, with its neighbors, a clique */
    void makeClique(const NodeId id);

    /// removes a node and its adjacent edges from the underlying graph
    /** The node should form a clique with its neighbors.
     * @param id the id of the node which, along with its neighbors, forms the
     * clique that will be removed
     * @throw NotFound exception is thrown if the node cannot be found
     * in the graph or if it is not a clique. */
    void eraseClique(const NodeId id);

    /// removes a node and its adjacent edges from the underlying graph
    /** @param id the id of the node which, along with its adjacent edges, will
     * be removed
     * @throw NotFound exception is thrown if the node cannot be found
     * in the graph. */
    void eraseNode(const NodeId id);

    /// removes an edge from the graph and recomputes the simplicial set
    /** @param edge the edge to be removed
     * @warning if the edge does not exist, nothing is done. In particular, no
     * exception is thrown. */
    void eraseEdge(const Edge& edge);

    /// adds a new edge to the graph and recomputes the simplicial set
    /** @param first the id of one extremal node of the new inserted edge
     * @param second the id of the other extremal node of the new inserted edge
     * @warning if the edge already exists, nothing is done. In particular, no
     * exception is raised.
     * @throw InvalidNode if first and/or second do not belong to the
     * graph nodes */
    void addEdge(NodeId first, NodeId second);

    /// indicates whether a given node is a simplicial node
    /** A simplicial node is a node such that the latter and its neighbors form
     * a clique.
     * @param id the ID of the node the simpliciality of which we test */
    bool isSimplicial(const NodeId id);

    /// indicates whether there exists a simplicial node
    /** A simplicial node is a node such that the latter and its neighbors form
     * a clique. */
    bool hasSimplicialNode();

    /// indicates whether there exists an almost simplicial node
    bool hasAlmostSimplicialNode();

    /// indicates whether there exists a quasi simplicial node
    bool hasQuasiSimplicialNode();

    /// returns the simplicial node with the lowest clique weight
    /** A simplicial node is a node such that the latter and its neighbors form
     * a clique. */
    NodeId bestSimplicialNode();

    /// returns all the simplicial nodes
    /** In the priority queue returned, the doubles correspond to the
     * log-weight of the cliques the nodes belong to. */
    const PriorityQueue< NodeId, double >& allSimplicialNodes();

    /// gets the almost simplicial node with the lowest clique weight
    NodeId bestAlmostSimplicialNode();

    /// returns all the almost simplicial nodes
    /** In the priority queue returned, the doubles correspond to the
     * log-weight of the cliques formed by the nodes and their neighbors. */
    const PriorityQueue< NodeId, double >& allAlmostSimplicialNodes();

    /// gets a quasi simplicial node with the lowest clique weight
    NodeId bestQuasiSimplicialNode();

    /// returns all the quasi simplicial nodes
    /** In the priority queue returned, the doubles correspond to the weight of
     * cliques formed by the nodes and their neighbors. */
    const PriorityQueue< NodeId, double >& allQuasiSimplicialNodes();

    /// sets/unset the fill-ins storage in the standard triangulation procedure
    /** @param on_off when true means that the SimplicialSet will compute the
     * fill-ins added to the graph. When on_off is false, the fill-ins are not
     * computed. Note that, to produce a correct result, you should call
     * setFillIns before any modification to the graph. */
    void setFillIns(bool on_off);

    /// returns the set of all the fill-ins added to the graph so far
    const EdgeSet& fillIns() const;

    /// initialize the simplicial set w.r.t. a new graph
    /** @param graph The undirected graph the simplicial sets of which we are
     * interested in.
     * @param log_domain_sizes The logarithm of the domain sizes of the
     * nodes/variables. This is used for two different reasons: i) it enables
     * to retrieve the simplicial nodes that have the smallest domain sizes
     * (useful for triangulations); and ii) it enables to compute and update
     * the log-weight of the cliques containing the nodes (the log-weight of a
     * clique is the sum of the log_domain_sizes of its nodes).
     * @param log_weights The logarithm of the weights of the cliques.
     * @param theRatio Let L be the number of edges between neighbors of a
     * given node and let L' be the number of all the possible edges between
     * these neighbors (n * (n+1) / 2). If L/L' >= theRatio, then we consider
     * that the node and its neighbors quasi form a clique and, hence is a
     * quasi-simplicial node.
     * @param theThreshold for a safe triangulation (see Bodlaender), almost and
     * quasi-simplicial nodes should not be eliminated, unless their weight is
     * lower than the highest weight of the cliques created so far. Here, we
     * consider it safe if the weight of a new clique is lower than
     * (1+theThreshold) * this highest weight. This enables flexibility.
     * @warning Note that we allow log_domain_sizes to be defined over
     * nodes/variables that do not belong to graph. These sizes will simply be
     * ignored. However, it is compulsory that all the nodes of graph belong to
     * log_domain_sizes.
     * @warning  Note that, by the aGrUM's constructor parameter's rule, the
     * fact that an argument is passed as a pointer means that it is not copied
     * within the SimplicialSet, but rather it is only referenced within it. */
    void setGraph(UndiGraph*                    graph,
                  const NodeProperty< double >* log_domain_sizes,
                  NodeProperty< double >*       log_weights,
                  double                        theRatio     = GUM_QUASI_RATIO,
                  double                        theThreshold = GUM_WEIGHT_THRESHOLD);

    /// reassigns a new set of cliques' log weights (with the same content)
    /** This method is useful for move constructors in elimination sequences.
     * @throws InvalidArgument is raised if the old_weights argument is
     * different from the current  _log_weights_ pointer. */
    void replaceLogWeights(NodeProperty< double >* old_weigths,
                           NodeProperty< double >* new_weights);

    /// @}


    private:
    /// the graph on which we perform the simplicial computations
    UndiGraph* _graph_;

    /// the weights of the nodes (i.e., weight of their clique)
    NodeProperty< double >* _log_weights_;

    /// the log of the modalities of the nodes
    const NodeProperty< double >* _log_domain_sizes_;

    /// a queue of the simplicial nodes ordered by increasing node weight
    PriorityQueue< NodeId, double > _simplicial_nodes_;

    /// a queue of the almost simplicial nodes ordered by increasing node weight
    PriorityQueue< NodeId, double > _almost_simplicial_nodes_;

    /// a queue of the quasi simplicial nodes ordered by increasing node weight
    PriorityQueue< NodeId, double > _quasi_simplicial_nodes_;

    /** @brief indicates for each node to which list (simplicial, almost
     * simplicial, quasi simplicial) it belongs */
    enum class _Belong_ : char {
      SIMPLICIAL,
      ALMOST_SIMPLICIAL,
      QUASI_SIMPLICIAL,
      NO_LIST
    };
    NodeProperty< _Belong_ > _containing_list_;

    /** @brief for each edge, keep track of the number of triangles passing
     * through this egde */
    EdgeProperty< Size > _nb_triangles_;

    /// for each node, the number of pairs of adjacent neighbours
    NodeProperty< Size > _nb_adjacent_neighbours_;

    /// the current (induced) tree width
    /** @warning Note that what we call tree width here is not the classical
     * definition, i.e., the number of nodes in the largest clique, as this is
     * not, to our mind, what is important for computations: what is important
     * is the size of the tables that would be stored into the cliques, i.e.,
     * the product of the modalities of the nodes/variables contained in
     * the cliques
     */
    double _log_tree_width_;

    /** @brief for a given node, if the number of pairs of neighbors that are
     * adjacent / the number of adjacent neighbors in a clique is greater than
     * the quasi ratio, then the node should belong the quasi simplicial list */
    double _quasi_ratio_;

    /** @brief quasi and almost simplicial nodes may not be eliminated unless
     * their weight is lower than (1 + threshold) * tree_width */
    double _log_threshold_;

    /// the set of nodes that have potentially changed of status
    NodeSet _changed_status_;

    /** @brief a boolean indicating if we want fill-ins list with the standard
     * triangulation method */
    bool _we_want_fill_ins_{false};

    /// fill-ins list
    EdgeSet _fill_ins_list_;


    /** @brief put node id in the correct simplicial/almost simplicial/quasi
     * simplicial list */
    void _updateList_(const NodeId id);

    /// put all the nodes in their appropriate list
    void _updateAllNodes_();

    /** @brief initialize: compute  _nb_triangles_,  _nb_adjacent_neighbors_, etc
     * when a new graph is set
     *
     * This method initializes the log_weights, the number of triangles and the
     * number of adjacent neighbors given the current graph. This is to be used
     * in constructors and method setGraph */
    void _initialize_();

    /// prevent a copy operator to be used
    /** If we did not prevent this operator to be used, we would be in a mess
     * since the graph, the modalities and the weights would be shared and
     * updated
     * by several Simplicial sets whereas the number of triangles and the number
     * of joined neighbors would not be shared. */
    SimplicialSet& operator=(const SimplicialSet&);

    /// prevent the default copy constructor
    /** If we did not prevent this operator to be used, we would be in a mess
     * since the graph, the domain sizes and the weights would be shared and
     * updated by several Simplicial sets whereas the number of triangles and
     * the number of joined neighbors would not be shared. */
    SimplicialSet(const SimplicialSet&);
  };

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphs/algorithms/simplicialSet_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_SIMPLICIAL_SET_H */
