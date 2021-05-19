/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
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
  StaticTriangulation::StaticTriangulation(
     const UndiGraph*                   theGraph,
     const NodeProperty< Size >*        domsizes,
     const EliminationSequenceStrategy& elimSeq,
     const JunctionTreeStrategy&        JTStrategy,
     bool                               minimality) :
      Triangulation(domsizes),
      elimination_sequence_strategy_(elimSeq.newFactory()),
      junction_tree_strategy_(JTStrategy.newFactory()), original_graph__(theGraph),
      minimality_required__(minimality) {
    // for debugging purposes
    GUM_CONSTRUCTOR(StaticTriangulation);

    // if the graph is not empty, resize several structures in order to speed-up
    // their fillings.
    if (theGraph != nullptr) {
      elim_order__.resize(theGraph->size());
      reverse_elim_order__.resize(theGraph->size());
      elim_cliques__.resize(theGraph->size());
      node_2_max_prime_clique__.resize(theGraph->size());
      added_fill_ins__.resize(theGraph->size());
    }

    // register the triangulation to its junction tree strategy
    junction_tree_strategy_->setTriangulation(this);
  }

  // default constructor
  StaticTriangulation::StaticTriangulation(
     const EliminationSequenceStrategy& elimSeq,
     const JunctionTreeStrategy&        JTStrategy,
     bool                               minimality) :
      elimination_sequence_strategy_(elimSeq.newFactory()),
      junction_tree_strategy_(JTStrategy.newFactory()),
      minimality_required__(minimality) {
    // for debugging purposes
    GUM_CONSTRUCTOR(StaticTriangulation);

    // register the triangulation to its junction tree strategy
    junction_tree_strategy_->setTriangulation(this);
  }

  // copy constructor
  StaticTriangulation::StaticTriangulation(const StaticTriangulation& from) :
      Triangulation(from), original_graph__(from.original_graph__),
      triangulated_graph__(from.triangulated_graph__), fill_ins__(from.fill_ins__),
      elim_order__(from.elim_order__),
      reverse_elim_order__(from.reverse_elim_order__),
      elim_cliques__(from.elim_cliques__), elim_tree__(from.elim_tree__),
      max_prime_junction_tree__(from.max_prime_junction_tree__),
      node_2_max_prime_clique__(from.node_2_max_prime_clique__),
      has_triangulation__(from.has_triangulation__),
      has_triangulated_graph__(from.has_triangulated_graph__),
      has_elimination_tree__(from.has_elimination_tree__),
      has_junction_tree__(from.has_junction_tree__),
      has_max_prime_junction_tree__(from.has_max_prime_junction_tree__),
      has_fill_ins__(from.has_fill_ins__),
      minimality_required__(from.minimality_required__),
      added_fill_ins__(from.added_fill_ins__),
      we_want_fill_ins__(from.we_want_fill_ins__) {
    // copy the strategies
    elimination_sequence_strategy_
       = from.elimination_sequence_strategy_->copyFactory();
    junction_tree_strategy_ = from.junction_tree_strategy_->copyFactory(this);

    // if from has a junction tree, copy it
    if (from.junction_tree__ != nullptr) {
      junction_tree__ = &(junction_tree_strategy_->junctionTree());
    }

    // for debugging purposes
    GUM_CONS_CPY(StaticTriangulation);
  }

  // move constructor
  StaticTriangulation::StaticTriangulation(StaticTriangulation&& from) :
      Triangulation(std::move(from)),
      elimination_sequence_strategy_(from.elimination_sequence_strategy_),
      junction_tree_strategy_(from.junction_tree_strategy_),
      original_graph__(from.original_graph__),
      triangulated_graph__(std::move(from.triangulated_graph__)),
      fill_ins__(std::move(from.fill_ins__)),
      elim_order__(std::move(from.elim_order__)),
      reverse_elim_order__(std::move(from.reverse_elim_order__)),
      elim_cliques__(std::move(from.elim_cliques__)),
      elim_tree__(std::move(from.elim_tree__)),
      max_prime_junction_tree__(std::move(from.max_prime_junction_tree__)),
      node_2_max_prime_clique__(std::move(from.node_2_max_prime_clique__)),
      has_triangulation__(from.has_triangulation__),
      has_triangulated_graph__(from.has_triangulated_graph__),
      has_elimination_tree__(from.has_elimination_tree__),
      has_junction_tree__(from.has_junction_tree__),
      has_max_prime_junction_tree__(from.has_max_prime_junction_tree__),
      has_fill_ins__(from.has_fill_ins__),
      minimality_required__(from.minimality_required__),
      added_fill_ins__(std::move(from.added_fill_ins__)),
      we_want_fill_ins__(from.we_want_fill_ins__) {
    // move the factories contained in from
    from.elimination_sequence_strategy_ = new DefaultEliminationSequenceStrategy;
    from.junction_tree_strategy_        = new DefaultJunctionTreeStrategy;
    junction_tree_strategy_->moveTriangulation(this);

    // if from has a junction tree, copy it
    if (from.junction_tree__ != nullptr) {
      junction_tree__ = &(junction_tree_strategy_->junctionTree());
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

    // no need to deallocate original_graph__ nor junction_tree__ because
    // they are not owned by the static triangulation class
  }

  /// reinitialize the graph to be triangulated to an empty graph
  void StaticTriangulation::clear() {
    // clear the factories
    elimination_sequence_strategy_->clear();
    junction_tree_strategy_->clear();

    // remove the current graphs
    original_graph__ = nullptr;
    junction_tree__  = nullptr;
    triangulated_graph__.clear();
    elim_tree__.clear();
    max_prime_junction_tree__.clear();
    elim_cliques__.clear();
    node_2_max_prime_clique__.clear();

    // remove all fill-ins and orderings
    fill_ins__.clear();
    added_fill_ins__.clear();
    elim_order__.clear();
    reverse_elim_order__.clear();

    // indicates that the junction tree, the max prime tree, etc, are updated
    has_triangulation__           = true;
    has_triangulated_graph__      = true;
    has_elimination_tree__        = true;
    has_junction_tree__           = true;
    has_max_prime_junction_tree__ = true;
    has_fill_ins__                = true;
  }

  // removes redondant fill-ins and compute proper elimination cliques and order
  void StaticTriangulation::computeRecursiveThinning__() {
    // apply recursive thinning (fmint, see Kjaerulff (90), "triangulation of
    // graphs - algorithms giving small total state space")
    NodeId                       node1, node2;
    bool                         incomplete;
    std::vector< NodeId >        adj;
    EdgeSet                      T_prime;
    NodeProperty< unsigned int > R(triangulated_graph__.size());

    for (const auto node: triangulated_graph__)
      R.insert(node, 0);

    // the FMINT loop
    if (added_fill_ins__.size() > 0) {
      for (auto iter = added_fill_ins__.rbegin(); iter != added_fill_ins__.rend();
           ++iter) {
        if (iter->size()) {
          // here apply MINT to T_i = added_fill_ins__[i]
          EdgeSet& T = *iter;

          // compute R: by default, R is equal to all the nodes in the graph
          // when R[...] >= j (see the j in the loop below), this means that the
          // node belongs to an extremal edge in R
          for (std::size_t k = 0; k < elim_order__.size(); ++k)
            R[elim_order__[k]]
               = 0;   // WARNING: do not replace R[elim_order__[k]] by

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
              if (triangulated_graph__.neighbours(node2).size()
                  < triangulated_graph__.neighbours(node1).size()) {
                NodeId tmp = node1;
                node1      = node2;
                node2      = tmp;
              }

              incomplete = false;

              // find the nodes belonging to the intersection of adj(v,G)
              // and adj(w,G)
              for (const auto adjnode: triangulated_graph__.neighbours(node1))
                if (triangulated_graph__.existsEdge(node2, adjnode))
                  adj.push_back(adjnode);

              // check if the intersection is complete
              for (unsigned int k = 0; k < adj.size() && !incomplete; ++k) {
                for (unsigned int m = k + 1; m < adj.size(); ++m) {
                  if (!triangulated_graph__.existsEdge(adj[k], adj[m])) {
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
              triangulated_graph__.eraseEdge(edge);

              if (has_fill_ins__) fill_ins__.erase(edge);
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
    PriorityQueue< NodeId, unsigned int, std::greater< unsigned int > >
       numbered_neighbours(std::greater< unsigned int >(),
                           triangulated_graph__.size());

    for (Size i = 0; i < elim_order__.size(); ++i)
      numbered_neighbours.insert(elim_order__[i], 0);

    // perform the maximum cardinality search
    if (elim_order__.size() > 0) {
      for (Size i = Size(elim_order__.size()); i >= 1; --i) {
        NodeId ni                  = NodeId(i - 1);
        NodeId node                = numbered_neighbours.pop();
        elim_order__[ni]           = node;
        reverse_elim_order__[node] = ni;

        for (const auto neighbour: triangulated_graph__.neighbours(node)) {
          try {
            numbered_neighbours.setPriority(
               neighbour,
               1 + numbered_neighbours.priority(neighbour));
          } catch (NotFound&) {}
        }
      }
    }

    // here the elimination order is ok. We now need to update the
    // elim_cliques__
    for (Size i = 0; i < elim_order__.size(); ++i) {
      NodeSet& cliques = elim_cliques__.insert(elim_order__[i], NodeSet()).second;
      cliques << elim_order__[i];

      for (const auto neighbour: triangulated_graph__.neighbours(elim_order__[i]))
        if (reverse_elim_order__[neighbour] > i) cliques << neighbour;
    }
  }

  /// returns an elimination tree from a triangulated graph
  void StaticTriangulation::computeEliminationTree__() {
    // if there already exists an elimination tree, no need to compute it again
    if (has_elimination_tree__) return;

    // if the graph is not triangulated, triangulate it
    if (!has_triangulation__) triangulate__();

    // create the nodes of the elimination tree
    elim_tree__.clear();

    Size size = Size(elim_order__.size());
    for (NodeId i = NodeId(0); i < size; ++i)
      elim_tree__.addNode(i, elim_cliques__[elim_order__[i]]);

    // create the edges of the elimination tree: join a node to the one in
    // its clique that is eliminated first
    for (NodeId i = NodeId(0); i < size; ++i) {
      NodeId         clique_i_creator = elim_order__[i];
      const NodeSet& list_of_nodes    = elim_cliques__[clique_i_creator];
      Idx            child            = original_graph__->bound() + 1;

      for (const auto node: list_of_nodes) {
        Idx it_elim_step = reverse_elim_order__[node];

        if ((node != clique_i_creator) && (child > it_elim_step))
          child = it_elim_step;
      }

      if (child <= original_graph__->bound()) {
        // WARNING: here, we assume that the nodes of the elimination tree are
        // indexed from 0 to n-1
        elim_tree__.addEdge(i, child);
      }
    }

    has_elimination_tree__ = true;
  }

  /// used for computing the junction tree of the maximal prime subgraphs
  void StaticTriangulation::computeMaxPrimeMergings__(
     const NodeId        node,
     const NodeId        from,
     std::vector< Arc >& merged_cliques,
     NodeSet&            mark) const {
    mark << node;

    for (const auto other_node: junction_tree__->neighbours(node))
      if (other_node != from) {
        const NodeSet& separator = junction_tree__->separator(node, other_node);
        // check that the separator between node and other_node is complete
        bool complete = true;

        for (auto iter_sep1 = separator.begin();
             iter_sep1 != separator.end() && complete;
             ++iter_sep1) {
          auto iter_sep2 = iter_sep1;

          for (++iter_sep2; iter_sep2 != separator.end(); ++iter_sep2) {
            if (!original_graph__->existsEdge(*iter_sep1, *iter_sep2)) {
              complete = false;
              break;
            }
          }
        }

        // here complete indicates whether the separator is complete or not
        if (!complete) merged_cliques.push_back(Arc(other_node, node));

        computeMaxPrimeMergings__(other_node, node, merged_cliques, mark);
      }
  }

  /// computes the junction tree of the maximal prime subgraphs
  void StaticTriangulation::computeMaxPrimeJunctionTree__() {
    // if there already exists a max prime junction tree, no need
    // to recompute it
    if (has_max_prime_junction_tree__) return;

    // if there is no junction tree, create it
    if (!has_junction_tree__) junctionTree();

    // the maximal prime subgraph join tree is created by aggregation of some
    // cliques. More precisely, when the separator between 2 cliques is not
    // complete in the original graph, then the two cliques must be merged.
    // Create a hashtable indicating which clique has been absorbed by some
    // other
    // clique.
    NodeProperty< NodeId > T_mpd_cliques(junction_tree__->size());

    for (const auto clik: junction_tree__->nodes())
      T_mpd_cliques.insert(clik, clik);

    // parse all the separators of the junction tree and test those that are not
    // complete in the orginal graph
    std::vector< Arc > merged_cliques;

    NodeSet mark;

    for (const auto clik: junction_tree__->nodes())
      if (!mark.contains(clik))
        computeMaxPrimeMergings__(clik, clik, merged_cliques, mark);

    // compute the transitive closure of merged_cliques. This one will contain
    // pairs (X,Y) indicating that clique X must be merged with clique Y.
    // Actually clique X will be inserted into clique Y.
    for (unsigned int i = 0; i < merged_cliques.size(); ++i) {
      T_mpd_cliques[merged_cliques[i].tail()]
         = T_mpd_cliques[merged_cliques[i].head()];
    }

    // now we can create the max prime junction tree. First, create the cliques
    for (const auto& elt: T_mpd_cliques)
      if (elt.first == elt.second)
        max_prime_junction_tree__.addNode(elt.second,
                                          junction_tree__->clique(elt.second));

    // add to the cliques previously created the nodes of the cliques that were
    // merged into them
    for (const auto& elt: T_mpd_cliques)
      if (elt.first != elt.second)
        for (const auto node: junction_tree__->clique(elt.first)) {
          try {
            max_prime_junction_tree__.addToClique(elt.second, node);
          } catch (DuplicateElement&) {}
        }

    // add the edges to the graph
    for (const auto& edge: junction_tree__->edges()) {
      NodeId node1 = T_mpd_cliques[edge.first()];
      NodeId node2 = T_mpd_cliques[edge.second()];

      if (node1 != node2) {
        try {
          max_prime_junction_tree__.addEdge(node1, node2);
        } catch (DuplicateElement&) {}
      }
    }

    // compute for each node which clique of the max prime junction tree was
    // created by the elimination of the node
    const NodeProperty< NodeId >& node_2_junction_clique
       = junction_tree_strategy_->createdCliques();

    for (const auto& elt: node_2_junction_clique)
      node_2_max_prime_clique__.insert(elt.first, T_mpd_cliques[elt.second]);

    has_max_prime_junction_tree__ = true;
  }

  /// returns the triangulated graph
  const UndiGraph& StaticTriangulation::triangulatedGraph() {
    if (!has_triangulation__) triangulate__();

    // if we did not construct the triangulated graph during the triangulation,
    // that is, we just constructed the junction tree, we shall construct it now
    if (!has_triangulated_graph__) {
      // just in case, be sure that the junction tree has been constructed
      if (!has_junction_tree__) junctionTree();

      // copy the original graph
      triangulated_graph__ = *original_graph__;

      for (const auto clik_id: *junction_tree__) {
        // for each clique, add the edges necessary to make it complete
        const NodeSet&        clique = junction_tree__->clique(clik_id);
        std::vector< NodeId > clique_nodes(clique.size());
        unsigned int          i = 0;

        for (const auto node: clique) {
          clique_nodes[i] = node;
          i += 1;
        }

        for (i = 0; i < clique_nodes.size(); ++i) {
          for (unsigned int j = i + 1; j < clique_nodes.size(); ++j) {
            try {
              triangulated_graph__.addEdge(clique_nodes[i], clique_nodes[j]);
            } catch (DuplicateElement&) {}
          }
        }
      }

      has_triangulated_graph__ = true;
    }

    return triangulated_graph__;
  }

  // initialize the triangulation algorithm for a new graph
  void StaticTriangulation::setGraph(const UndiGraph*            graph,
                                     const NodeProperty< Size >* domsizes) {
    // remove the old graph
    clear();

    if (graph != nullptr) {
      // prepare the size of the data for the new graph
      elim_order__.resize(graph->size());
      reverse_elim_order__.resize(graph->size());
      elim_cliques__.resize(graph->size());
      added_fill_ins__.resize(graph->size());
      node_2_max_prime_clique__.resize(graph->size());
    }

    // keep track of the variables passed in argument
    original_graph__ = graph;
    domain_sizes_    = domsizes;

    // indicate that no triangulation was performed for this graph
    has_triangulation__           = false;
    has_triangulated_graph__      = false;
    has_elimination_tree__        = false;
    has_junction_tree__           = false;
    has_max_prime_junction_tree__ = false;
    has_fill_ins__                = false;
  }

  /// the function that performs the triangulation
  void StaticTriangulation::triangulate__() {
    // if the graph is already triangulated, no need to triangulate it again
    if (has_triangulation__) return;

    // copy the graph to be triangulated, so that we can modify it
    UndiGraph tmp_graph = *original_graph__;

    initTriangulation_(tmp_graph);
    elimination_sequence_strategy_->askFillIns(we_want_fill_ins__);

    // if we are to do recursive thinning, we will have to add fill-ins to the
    // triangulated graph each time we eliminate a node. Hence, we shall
    // initialize the triangulated graph by a copy of the original graph
    if (minimality_required__) {
      triangulated_graph__     = *original_graph__;
      has_triangulated_graph__ = true;
    }

    // perform the triangulation
    NodeId removable_node = 0;

    for (unsigned int nb_elim = 0; tmp_graph.size() != 0; ++nb_elim) {
      removable_node = elimination_sequence_strategy_->nextNodeToEliminate();

      // when minimality is not required, i.e., we won't apply recursive
      // thinning, the cliques sets can be computed
      if (!minimality_required__) {
        NodeSet& cliques = elim_cliques__.insert(removable_node, NodeSet()).second;
        cliques.resize(tmp_graph.neighbours(removable_node).size() / 2);
        cliques << removable_node;
        for (const auto clik: tmp_graph.neighbours(removable_node))
          cliques << clik;
      } else {
        // here recursive thinning will be applied, hence we need store the
        // fill-ins added by the current node removal
        EdgeSet& current_fill = added_fill_ins__[nb_elim];
        NodeId   node1, node2;

        const NodeSet& nei = tmp_graph.neighbours(removable_node);

        for (auto iter_edges1 = nei.begin(); iter_edges1 != nei.end();
             ++iter_edges1) {
          node1            = *iter_edges1;
          auto iter_edges2 = iter_edges1;

          for (++iter_edges2; iter_edges2 != nei.end(); ++iter_edges2) {
            node2 = *iter_edges2;
            Edge edge(node1, node2);

            if (!tmp_graph.existsEdge(edge)) {
              current_fill.insert(edge);
              triangulated_graph__.addEdge(node1, node2);
            }
          }
        }
      }

      // if we want fill-ins but the elimination sequence strategy does not
      // compute them, we shall compute them here
      if (we_want_fill_ins__
          && !elimination_sequence_strategy_->providesFillIns()) {
        NodeId node1, node2;

        // add edges between removable_node's neighbours in order to create
        // a clique
        const NodeSet& nei = tmp_graph.neighbours(removable_node);

        for (auto iter_edges1 = nei.begin(); iter_edges1 != nei.end();
             ++iter_edges1) {
          node1            = *iter_edges1;
          auto iter_edges2 = iter_edges1;

          for (++iter_edges2; iter_edges2 != nei.end(); ++iter_edges2) {
            node2 = *iter_edges2;
            Edge edge(node1, node2);

            if (!tmp_graph.existsEdge(edge)) { fill_ins__.insert(edge); }
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

        for (auto iter_edges1 = nei.begin(); iter_edges1 != nei.end();
             ++iter_edges1) {
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
      elim_order__[nb_elim] = removable_node;
      reverse_elim_order__.insert(removable_node, nb_elim);
    }

    // indicate whether we actually computed fill-ins
    has_fill_ins__ = we_want_fill_ins__;

    // if minimality is required, remove the redondant edges
    if (minimality_required__) computeRecursiveThinning__();

    has_triangulation__ = true;
  }

  /// returns the fill-ins added by the triangulation algorithm
  const EdgeSet& StaticTriangulation::fillIns() {
    // if we did not compute the triangulation yet, do it and commpute
    // the fill-ins at the same time
    if (!has_triangulation__) {
      bool want_fill_ins = we_want_fill_ins__;
      we_want_fill_ins__ = true;
      triangulate__();
      we_want_fill_ins__ = want_fill_ins;
      has_fill_ins__     = true;
    }

    // here, we already computed a triangulation and we already computed
    // the fill-ins, so return them
    if (has_fill_ins__) {
      if (elimination_sequence_strategy_->providesFillIns())
        return elimination_sequence_strategy_->fillIns();
      else
        return fill_ins__;
    } else {
      // ok, here, we shall compute the fill-ins as they were not precomputed
      if (!original_graph__) return fill_ins__;

      // just in case, be sure that the junction tree has been constructed
      if (!has_junction_tree__) junctionTree();

      for (const auto clik_id: junction_tree__->nodes()) {
        // for each clique, add the edges necessary to make it complete
        const NodeSet&        clique = junction_tree__->clique(clik_id);
        std::vector< NodeId > clique_nodes(clique.size());
        unsigned int          i = 0;

        for (const auto node: clique) {
          clique_nodes[i] = node;
          i += 1;
        }

        for (i = 0; i < clique_nodes.size(); ++i) {
          for (unsigned int j = i + 1; j < clique_nodes.size(); ++j) {
            Edge edge(clique_nodes[i], clique_nodes[j]);

            if (!original_graph__->existsEdge(edge)) {
              try {
                fill_ins__.insert(edge);
              } catch (DuplicateElement&) {}
            }
          }
        }
      }

      return fill_ins__;
    }
  }

  /// the function called to initialize the triangulation process
  void StaticTriangulation::initTriangulation_(UndiGraph& graph) {
    elimination_sequence_strategy_->setGraph(&graph, domain_sizes_);
  }

} /* namespace gum */
