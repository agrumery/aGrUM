/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief base class for graph non-incremental triangulations.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/tools/core/priorityQueue.h>
#include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/defaultEliminationSequenceStrategy.h>
#include <agrum/tools/graphs/algorithms/triangulations/junctionTreeStrategies/defaultJunctionTreeStrategy.h>
#include <agrum/tools/graphs/algorithms/triangulations/staticTriangulation.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/algorithms/triangulations/staticTriangulation_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  // default constructor
  StaticTriangulation::StaticTriangulation(const UndiGraph*                   theGraph,
                                           const NodeProperty< Size >*        domsizes,
                                           const EliminationSequenceStrategy& elimSeq,
                                           const JunctionTreeStrategy&        JTStrategy,
                                           bool                               minimality) :
      Triangulation(domsizes),
      elimination_sequence_strategy_(elimSeq.newFactory()),
      junction_tree_strategy_(JTStrategy.newFactory()), _original_graph_(theGraph),
      _minimality_required_(minimality) {
    // for debugging purposes
    GUM_CONSTRUCTOR(StaticTriangulation);

    // if the graph is not empty, resize several structures in order to speed-up
    // their fillings.
    if (theGraph != nullptr) {
      _elim_order_.resize(theGraph->size());
      _reverse_elim_order_.resize(theGraph->size());
      _elim_cliques_.resize(theGraph->size());
      _node_2_max_prime_clique_.resize(theGraph->size());
      _added_fill_ins_.resize(theGraph->size());
    }

    // register the triangulation to its junction tree strategy
    junction_tree_strategy_->setTriangulation(this);
  }

  // default constructor
  StaticTriangulation::StaticTriangulation(const EliminationSequenceStrategy& elimSeq,
                                           const JunctionTreeStrategy&        JTStrategy,
                                           bool                               minimality) :
      elimination_sequence_strategy_(elimSeq.newFactory()),
      junction_tree_strategy_(JTStrategy.newFactory()), _minimality_required_(minimality) {
    // for debugging purposes
    GUM_CONSTRUCTOR(StaticTriangulation);

    // register the triangulation to its junction tree strategy
    junction_tree_strategy_->setTriangulation(this);
  }

  // copy constructor
  StaticTriangulation::StaticTriangulation(const StaticTriangulation& from) :
      Triangulation(from), _original_graph_(from._original_graph_),
      _triangulated_graph_(from._triangulated_graph_), _fill_ins_(from._fill_ins_),
      _elim_order_(from._elim_order_), _reverse_elim_order_(from._reverse_elim_order_),
      _elim_cliques_(from._elim_cliques_), _elim_tree_(from._elim_tree_),
      _max_prime_junction_tree_(from._max_prime_junction_tree_),
      _node_2_max_prime_clique_(from._node_2_max_prime_clique_),
      _has_triangulation_(from._has_triangulation_),
      _has_triangulated_graph_(from._has_triangulated_graph_),
      _has_elimination_tree_(from._has_elimination_tree_),
      _has_junction_tree_(from._has_junction_tree_),
      _has_max_prime_junction_tree_(from._has_max_prime_junction_tree_),
      _has_fill_ins_(from._has_fill_ins_), _minimality_required_(from._minimality_required_),
      _added_fill_ins_(from._added_fill_ins_), _we_want_fill_ins_(from._we_want_fill_ins_) {
    // copy the strategies
    elimination_sequence_strategy_ = from.elimination_sequence_strategy_->copyFactory();
    junction_tree_strategy_        = from.junction_tree_strategy_->copyFactory(this);

    // if from has a junction tree, copy it
    if (from._junction_tree_ != nullptr) {
      _junction_tree_ = &(junction_tree_strategy_->junctionTree());
    }

    // for debugging purposes
    GUM_CONS_CPY(StaticTriangulation);
  }

  // move constructor
  StaticTriangulation::StaticTriangulation(StaticTriangulation&& from) :
      Triangulation(std::move(from)),
      elimination_sequence_strategy_(from.elimination_sequence_strategy_),
      junction_tree_strategy_(from.junction_tree_strategy_),
      _original_graph_(from._original_graph_),
      _triangulated_graph_(std::move(from._triangulated_graph_)),
      _fill_ins_(std::move(from._fill_ins_)), _elim_order_(std::move(from._elim_order_)),
      _reverse_elim_order_(std::move(from._reverse_elim_order_)),
      _elim_cliques_(std::move(from._elim_cliques_)), _elim_tree_(std::move(from._elim_tree_)),
      _max_prime_junction_tree_(std::move(from._max_prime_junction_tree_)),
      _node_2_max_prime_clique_(std::move(from._node_2_max_prime_clique_)),
      _has_triangulation_(from._has_triangulation_),
      _has_triangulated_graph_(from._has_triangulated_graph_),
      _has_elimination_tree_(from._has_elimination_tree_),
      _has_junction_tree_(from._has_junction_tree_),
      _has_max_prime_junction_tree_(from._has_max_prime_junction_tree_),
      _has_fill_ins_(from._has_fill_ins_), _minimality_required_(from._minimality_required_),
      _added_fill_ins_(std::move(from._added_fill_ins_)),
      _we_want_fill_ins_(from._we_want_fill_ins_) {
    // move the factories contained in from
    from.elimination_sequence_strategy_ = new DefaultEliminationSequenceStrategy;
    from.junction_tree_strategy_        = new DefaultJunctionTreeStrategy;
    junction_tree_strategy_->moveTriangulation(this);

    // if from has a junction tree, copy it
    if (from._junction_tree_ != nullptr) {
      _junction_tree_ = &(junction_tree_strategy_->junctionTree());
    }

    // for debugging purposes
    GUM_CONS_MOV(StaticTriangulation);
  }

  /// destructor
  StaticTriangulation::~StaticTriangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR(StaticTriangulation);

    delete elimination_sequence_strategy_;
    delete junction_tree_strategy_;

    // no need to deallocate  _original_graph_ nor  _junction_tree_ because
    // they are not owned by the static triangulation class
  }

  /// reinitialize the graph to be triangulated to an empty graph
  void StaticTriangulation::clear() {
    // clear the factories
    elimination_sequence_strategy_->clear();
    junction_tree_strategy_->clear();

    // remove the current graphs
    _original_graph_ = nullptr;
    _junction_tree_  = nullptr;
    _triangulated_graph_.clear();
    _elim_tree_.clear();
    _max_prime_junction_tree_.clear();
    _elim_cliques_.clear();
    _node_2_max_prime_clique_.clear();

    // remove all fill-ins and orderings
    _fill_ins_.clear();
    _added_fill_ins_.clear();
    _elim_order_.clear();
    _reverse_elim_order_.clear();

    // indicates that the junction tree, the max prime tree, etc, are updated
    _has_triangulation_           = true;
    _has_triangulated_graph_      = true;
    _has_elimination_tree_        = true;
    _has_junction_tree_           = true;
    _has_max_prime_junction_tree_ = true;
    _has_fill_ins_                = true;
  }

  // removes redondant fill-ins and compute proper elimination cliques and order
  void StaticTriangulation::_computeRecursiveThinning_() {
    // apply recursive thinning (fmint, see Kjaerulff (90), "triangulation of
    // graphs - algorithms giving small total state space")
    NodeId                       node1, node2;
    bool                         incomplete;
    std::vector< NodeId >        adj;
    EdgeSet                      T_prime;
    NodeProperty< unsigned int > R(_triangulated_graph_.size());

    for (const auto node: _triangulated_graph_)
      R.insert(node, 0);

    // the FMINT loop
    if (_added_fill_ins_.size() > 0) {
      for (auto iter = _added_fill_ins_.rbegin(); iter != _added_fill_ins_.rend(); ++iter) {
        if (iter->size()) {
          // here apply MINT to T_i =  _added_fill_ins_[i]
          EdgeSet& T = *iter;

          // compute R: by default, R is equal to all the nodes in the graph
          // when R[...] >= j (see the j in the loop below), this means that the
          // node belongs to an extremal edge in R
          for (std::size_t k = 0; k < _elim_order_.size(); ++k)
            R[_elim_order_[k]] = 0;   // WARNING: do not replace R[ _elim_order_[k]] by

          // R[k] because the node ids may not be
          // consecutive numbers

          // apply MINT while some edges can possibly be deleted
          bool require_mint = true;

          for (unsigned int j = 0; require_mint; ++j) {
            // find T' (it is equal to the edges (v,w) of T such that
            // the intersection of adj(v,G) and adj(w,G) is complete and such
            // that
            // v and/or w belongs to an extremal node in R
            for (const auto& edge: T) {
              node1 = edge.first();
              node2 = edge.second();

              // check if at least one extremal node belongs to R
              if ((R[node1] < j) && (R[node2] < j)) continue;

              // check if the intersection of adj(v,G) and adj(w,G) is a
              // complete subgraph
              if (_triangulated_graph_.neighbours(node2).size()
                  < _triangulated_graph_.neighbours(node1).size()) {
                NodeId tmp = node1;
                node1      = node2;
                node2      = tmp;
              }

              incomplete = false;

              // find the nodes belonging to the intersection of adj(v,G)
              // and adj(w,G)
              for (const auto adjnode: _triangulated_graph_.neighbours(node1))
                if (_triangulated_graph_.existsEdge(node2, adjnode)) adj.push_back(adjnode);

              // check if the intersection is complete
              for (unsigned int k = 0; k < adj.size() && !incomplete; ++k) {
                for (unsigned int m = k + 1; m < adj.size(); ++m) {
                  if (!_triangulated_graph_.existsEdge(adj[k], adj[m])) {
                    incomplete = true;
                    break;
                  }
                }
              }

              adj.clear();

              if (!incomplete) {
                T_prime.insert(edge);
                R[node1] = j + 1;
                R[node2] = j + 1;
              }
            }

            // compute the new value of T (i.e. T\T') and update the
            // triangulated graph
            for (const auto& edge: T_prime) {
              T.erase(edge);
              _triangulated_graph_.eraseEdge(edge);

              if (_has_fill_ins_) _fill_ins_.erase(edge);
            }

            if (T_prime.size() == 0) require_mint = false;

            T_prime.clear();
          }
        }
      }
    }

    // here, the recursive thinning has removed all the superfluous edges.
    // Hence some cliques have been split and, as a result, the elimination
    // order has changed. We should thus compute a new perfect ordering. To
    // do so, we use a Maximal Cardinality Search: it has indeed the nice
    // property that, when the graph is already triangulated, it finds a
    // compatible order of elimination that does not require any edge addition

    // a structure storing the number of neighbours previously processed
    PriorityQueue< NodeId, unsigned int, std::greater< unsigned int > > numbered_neighbours(
       std::greater< unsigned int >(),
       _triangulated_graph_.size());

    for (Size i = 0; i < _elim_order_.size(); ++i)
      numbered_neighbours.insert(_elim_order_[i], 0);

    // perform the maximum cardinality search
    if (_elim_order_.size() > 0) {
      for (Size i = Size(_elim_order_.size()); i >= 1; --i) {
        NodeId ni                  = NodeId(i - 1);
        NodeId node                = numbered_neighbours.pop();
        _elim_order_[ni]           = node;
        _reverse_elim_order_[node] = ni;

        for (const auto neighbour: _triangulated_graph_.neighbours(node)) {
          try {
            numbered_neighbours.setPriority(neighbour, 1 + numbered_neighbours.priority(neighbour));
          } catch (NotFound&) {}
        }
      }
    }

    // here the elimination order is ok. We now need to update the
    //  _elim_cliques_
    for (Size i = 0; i < _elim_order_.size(); ++i) {
      NodeSet& cliques = _elim_cliques_.insert(_elim_order_[i], NodeSet()).second;
      cliques << _elim_order_[i];

      for (const auto neighbour: _triangulated_graph_.neighbours(_elim_order_[i]))
        if (_reverse_elim_order_[neighbour] > i) cliques << neighbour;
    }
  }

  /// returns an elimination tree from a triangulated graph
  void StaticTriangulation::_computeEliminationTree_() {
    // if there already exists an elimination tree, no need to compute it again
    if (_has_elimination_tree_) return;

    // if the graph is not triangulated, triangulate it
    if (!_has_triangulation_) _triangulate_();

    // create the nodes of the elimination tree
    _elim_tree_.clear();

    Size size = Size(_elim_order_.size());
    for (NodeId i = NodeId(0); i < size; ++i)
      _elim_tree_.addNode(i, _elim_cliques_[_elim_order_[i]]);

    // create the edges of the elimination tree: join a node to the one in
    // its clique that is eliminated first
    for (NodeId i = NodeId(0); i < size; ++i) {
      NodeId         clique_i_creator = _elim_order_[i];
      const NodeSet& list_of_nodes    = _elim_cliques_[clique_i_creator];
      Idx            child            = _original_graph_->bound() + 1;

      for (const auto node: list_of_nodes) {
        Idx it_elim_step = _reverse_elim_order_[node];

        if ((node != clique_i_creator) && (child > it_elim_step)) child = it_elim_step;
      }

      if (child <= _original_graph_->bound()) {
        // WARNING: here, we assume that the nodes of the elimination tree are
        // indexed from 0 to n-1
        _elim_tree_.addEdge(i, child);
      }
    }

    _has_elimination_tree_ = true;
  }

  /// used for computing the junction tree of the maximal prime subgraphs
  void StaticTriangulation::_computeMaxPrimeMergings_(const NodeId        node,
                                                      const NodeId        from,
                                                      std::vector< Arc >& merged_cliques,
                                                      NodeSet&            mark) const {
    mark << node;

    for (const auto other_node: _junction_tree_->neighbours(node))
      if (other_node != from) {
        const NodeSet& separator = _junction_tree_->separator(node, other_node);
        // check that the separator between node and other_node is complete
        bool complete = true;

        for (auto iter_sep1 = separator.begin(); iter_sep1 != separator.end() && complete;
             ++iter_sep1) {
          auto iter_sep2 = iter_sep1;

          for (++iter_sep2; iter_sep2 != separator.end(); ++iter_sep2) {
            if (!_original_graph_->existsEdge(*iter_sep1, *iter_sep2)) {
              complete = false;
              break;
            }
          }
        }

        // here complete indicates whether the separator is complete or not
        if (!complete) merged_cliques.push_back(Arc(other_node, node));

        _computeMaxPrimeMergings_(other_node, node, merged_cliques, mark);
      }
  }

  /// computes the junction tree of the maximal prime subgraphs
  void StaticTriangulation::_computeMaxPrimeJunctionTree_() {
    // if there already exists a max prime junction tree, no need
    // to recompute it
    if (_has_max_prime_junction_tree_) return;

    // if there is no junction tree, create it
    if (!_has_junction_tree_) junctionTree();

    // the maximal prime subgraph join tree is created by aggregation of some
    // cliques. More precisely, when the separator between 2 cliques is not
    // complete in the original graph, then the two cliques must be merged.
    // Create a hashtable indicating which clique has been absorbed by some
    // other
    // clique.
    NodeProperty< NodeId > T_mpd_cliques(_junction_tree_->size());

    for (const auto clik: _junction_tree_->nodes())
      T_mpd_cliques.insert(clik, clik);

    // parse all the separators of the junction tree and test those that are not
    // complete in the orginal graph
    std::vector< Arc > merged_cliques;

    NodeSet mark;

    for (const auto clik: _junction_tree_->nodes())
      if (!mark.contains(clik)) _computeMaxPrimeMergings_(clik, clik, merged_cliques, mark);

    // compute the transitive closure of merged_cliques. This one will contain
    // pairs (X,Y) indicating that clique X must be merged with clique Y.
    // Actually clique X will be inserted into clique Y.
    for (unsigned int i = 0; i < merged_cliques.size(); ++i) {
      T_mpd_cliques[merged_cliques[i].tail()] = T_mpd_cliques[merged_cliques[i].head()];
    }

    // now we can create the max prime junction tree. First, create the cliques
    for (const auto& elt: T_mpd_cliques)
      if (elt.first == elt.second)
        _max_prime_junction_tree_.addNode(elt.second, _junction_tree_->clique(elt.second));

    // add to the cliques previously created the nodes of the cliques that were
    // merged into them
    for (const auto& elt: T_mpd_cliques)
      if (elt.first != elt.second)
        for (const auto node: _junction_tree_->clique(elt.first)) {
          try {
            _max_prime_junction_tree_.addToClique(elt.second, node);
          } catch (DuplicateElement&) {}
        }

    // add the edges to the graph
    for (const auto& edge: _junction_tree_->edges()) {
      NodeId node1 = T_mpd_cliques[edge.first()];
      NodeId node2 = T_mpd_cliques[edge.second()];

      if (node1 != node2) {
        try {
          _max_prime_junction_tree_.addEdge(node1, node2);
        } catch (DuplicateElement&) {}
      }
    }

    // compute for each node which clique of the max prime junction tree was
    // created by the elimination of the node
    const NodeProperty< NodeId >& node_2_junction_clique
       = junction_tree_strategy_->createdCliques();

    for (const auto& elt: node_2_junction_clique)
      _node_2_max_prime_clique_.insert(elt.first, T_mpd_cliques[elt.second]);

    _has_max_prime_junction_tree_ = true;
  }

  /// returns the triangulated graph
  const UndiGraph& StaticTriangulation::triangulatedGraph() {
    if (!_has_triangulation_) _triangulate_();

    // if we did not construct the triangulated graph during the triangulation,
    // that is, we just constructed the junction tree, we shall construct it now
    if (!_has_triangulated_graph_) {
      // just in case, be sure that the junction tree has been constructed
      if (!_has_junction_tree_) junctionTree();

      // copy the original graph
      _triangulated_graph_ = *_original_graph_;

      for (const auto clik_id: *_junction_tree_) {
        // for each clique, add the edges necessary to make it complete
        const NodeSet&        clique = _junction_tree_->clique(clik_id);
        std::vector< NodeId > clique_nodes(clique.size());
        unsigned int          i = 0;

        for (const auto node: clique) {
          clique_nodes[i] = node;
          i += 1;
        }

        for (i = 0; i < clique_nodes.size(); ++i) {
          for (unsigned int j = i + 1; j < clique_nodes.size(); ++j) {
            try {
              _triangulated_graph_.addEdge(clique_nodes[i], clique_nodes[j]);
            } catch (DuplicateElement&) {}
          }
        }
      }

      _has_triangulated_graph_ = true;
    }

    return _triangulated_graph_;
  }

  // initialize the triangulation algorithm for a new graph
  void StaticTriangulation::setGraph(const UndiGraph* graph, const NodeProperty< Size >* domsizes) {
    // remove the old graph
    clear();

    if (graph != nullptr) {
      // prepare the size of the data for the new graph
      _elim_order_.resize(graph->size());
      _reverse_elim_order_.resize(graph->size());
      _elim_cliques_.resize(graph->size());
      _added_fill_ins_.resize(graph->size());
      _node_2_max_prime_clique_.resize(graph->size());
    }

    // keep track of the variables passed in argument
    _original_graph_ = graph;
    domain_sizes_    = domsizes;

    // indicate that no triangulation was performed for this graph
    _has_triangulation_           = false;
    _has_triangulated_graph_      = false;
    _has_elimination_tree_        = false;
    _has_junction_tree_           = false;
    _has_max_prime_junction_tree_ = false;
    _has_fill_ins_                = false;
  }

  /// the function that performs the triangulation
  void StaticTriangulation::_triangulate_() {
    // if the graph is already triangulated, no need to triangulate it again
    if (_has_triangulation_) return;

    // copy the graph to be triangulated, so that we can modify it
    UndiGraph tmp_graph = *_original_graph_;

    initTriangulation_(tmp_graph);
    elimination_sequence_strategy_->askFillIns(_we_want_fill_ins_);

    // if we are to do recursive thinning, we will have to add fill-ins to the
    // triangulated graph each time we eliminate a node. Hence, we shall
    // initialize the triangulated graph by a copy of the original graph
    if (_minimality_required_) {
      _triangulated_graph_     = *_original_graph_;
      _has_triangulated_graph_ = true;
    }

    // perform the triangulation
    NodeId removable_node = 0;

    for (unsigned int nb_elim = 0; tmp_graph.size() != 0; ++nb_elim) {
      removable_node = elimination_sequence_strategy_->nextNodeToEliminate();

      // when minimality is not required, i.e., we won't apply recursive
      // thinning, the cliques sets can be computed
      if (!_minimality_required_) {
        NodeSet& cliques = _elim_cliques_.insert(removable_node, NodeSet()).second;
        cliques.resize(tmp_graph.neighbours(removable_node).size() / 2);
        cliques << removable_node;
        for (const auto clik: tmp_graph.neighbours(removable_node))
          cliques << clik;
      } else {
        // here recursive thinning will be applied, hence we need store the
        // fill-ins added by the current node removal
        EdgeSet& current_fill = _added_fill_ins_[nb_elim];
        NodeId   node1, node2;

        const NodeSet& nei = tmp_graph.neighbours(removable_node);

        for (auto iter_edges1 = nei.begin(); iter_edges1 != nei.end(); ++iter_edges1) {
          node1            = *iter_edges1;
          auto iter_edges2 = iter_edges1;

          for (++iter_edges2; iter_edges2 != nei.end(); ++iter_edges2) {
            node2 = *iter_edges2;
            Edge edge(node1, node2);

            if (!tmp_graph.existsEdge(edge)) {
              current_fill.insert(edge);
              _triangulated_graph_.addEdge(node1, node2);
            }
          }
        }
      }

      // if we want fill-ins but the elimination sequence strategy does not
      // compute them, we shall compute them here
      if (_we_want_fill_ins_ && !elimination_sequence_strategy_->providesFillIns()) {
        NodeId node1, node2;

        // add edges between removable_node's neighbours in order to create
        // a clique
        const NodeSet& nei = tmp_graph.neighbours(removable_node);

        for (auto iter_edges1 = nei.begin(); iter_edges1 != nei.end(); ++iter_edges1) {
          node1            = *iter_edges1;
          auto iter_edges2 = iter_edges1;

          for (++iter_edges2; iter_edges2 != nei.end(); ++iter_edges2) {
            node2 = *iter_edges2;
            Edge edge(node1, node2);

            if (!tmp_graph.existsEdge(edge)) { _fill_ins_.insert(edge); }
          }
        }

        // delete removable_node and its adjacent edges
        tmp_graph.eraseNode(removable_node);
      }

      // inform the elimination sequence that we are actually removing
      // removable_node (this enables, for instance, to update the weights of
      // the nodes in the graph)
      elimination_sequence_strategy_->eliminationUpdate(removable_node);

      if (!elimination_sequence_strategy_->providesGraphUpdate()) {
        NodeId node1, node2;

        const NodeSet& nei = tmp_graph.neighbours(removable_node);

        for (auto iter_edges1 = nei.begin(); iter_edges1 != nei.end(); ++iter_edges1) {
          node1            = *iter_edges1;
          auto iter_edges2 = iter_edges1;

          for (++iter_edges2; iter_edges2 != nei.end(); ++iter_edges2) {
            node2 = *iter_edges2;
            Edge edge(node1, node2);

            if (!tmp_graph.existsEdge(edge)) { tmp_graph.addEdge(node1, node2); }
          }
        }

        tmp_graph.eraseNode(removable_node);
      }

      // update the elimination order
      _elim_order_[nb_elim] = removable_node;
      _reverse_elim_order_.insert(removable_node, nb_elim);
    }

    // indicate whether we actually computed fill-ins
    _has_fill_ins_ = _we_want_fill_ins_;

    // if minimality is required, remove the redondant edges
    if (_minimality_required_) _computeRecursiveThinning_();

    _has_triangulation_ = true;
  }

  /// returns the fill-ins added by the triangulation algorithm
  const EdgeSet& StaticTriangulation::fillIns() {
    // if we did not compute the triangulation yet, do it and commpute
    // the fill-ins at the same time
    if (!_has_triangulation_) {
      bool want_fill_ins = _we_want_fill_ins_;
      _we_want_fill_ins_ = true;
      _triangulate_();
      _we_want_fill_ins_ = want_fill_ins;
      _has_fill_ins_     = true;
    }

    // here, we already computed a triangulation and we already computed
    // the fill-ins, so return them
    if (_has_fill_ins_) {
      if (elimination_sequence_strategy_->providesFillIns())
        return elimination_sequence_strategy_->fillIns();
      else
        return _fill_ins_;
    } else {
      // ok, here, we shall compute the fill-ins as they were not precomputed
      if (!_original_graph_) return _fill_ins_;

      // just in case, be sure that the junction tree has been constructed
      if (!_has_junction_tree_) junctionTree();

      for (const auto clik_id: _junction_tree_->nodes()) {
        // for each clique, add the edges necessary to make it complete
        const NodeSet&        clique = _junction_tree_->clique(clik_id);
        std::vector< NodeId > clique_nodes(clique.size());
        unsigned int          i = 0;

        for (const auto node: clique) {
          clique_nodes[i] = node;
          i += 1;
        }

        for (i = 0; i < clique_nodes.size(); ++i) {
          for (unsigned int j = i + 1; j < clique_nodes.size(); ++j) {
            Edge edge(clique_nodes[i], clique_nodes[j]);

            if (!_original_graph_->existsEdge(edge)) {
              try {
                _fill_ins_.insert(edge);
              } catch (DuplicateElement&) {}
            }
          }
        }
      }

      return _fill_ins_;
    }
  }

  /// the function called to initialize the triangulation process
  void StaticTriangulation::initTriangulation_(UndiGraph& graph) {
    elimination_sequence_strategy_->setGraph(&graph, domain_sizes_);
  }

} /* namespace gum */
