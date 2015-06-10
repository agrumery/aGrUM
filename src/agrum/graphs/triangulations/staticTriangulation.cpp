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
 * @brief base class for graph non-incremental triangulations.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/graphs/triangulations/staticTriangulation.h>
#include <agrum/core/priorityQueue.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/triangulations/staticTriangulation.inl>
#endif // GUM_NO_INLINE

namespace gum {

  /// default constructor

  StaticTriangulation::StaticTriangulation(
      const UndiGraph *theGraph, const NodeProperty<Size> *modal,
      const EliminationSequenceStrategy &elimSeq,
      const JunctionTreeStrategy &JTStrategy, bool minimality)
      : _elimination_sequence_strategy(elimSeq.newFactory()),
        _junction_tree_strategy(JTStrategy.newFactory()), __original_graph(theGraph),
        __junction_tree(0), __has_triangulation(false),
        __has_triangulated_graph(false), __has_elimination_tree(false),
        __has_junction_tree(false), __has_max_prime_junction_tree(false),
        __has_fill_ins(false), __minimality_required(minimality),
        __we_want_fill_ins(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(StaticTriangulation);

    _modalities = *modal;

    // if the graph is not empty, resize several structures in order to speed-up
    // their fillings.
    if (theGraph) {
      __elim_order.resize(theGraph->size());
      __reverse_elim_order.resize(theGraph->size());
      __elim_cliques.resize(theGraph->size());
      __node_2_max_prime_clique.resize(theGraph->size());
      __added_fill_ins.resize(theGraph->size());
    }

    // register the triangulation to its junction tree strategy
    _junction_tree_strategy->setTriangulation(this);
  }

  /// default constructor

  StaticTriangulation::StaticTriangulation(
      const EliminationSequenceStrategy &elimSeq,
      const JunctionTreeStrategy &JTStrategy, bool minimality)
      : _elimination_sequence_strategy(elimSeq.newFactory()),
        _junction_tree_strategy(JTStrategy.newFactory()), __original_graph(0),
        __junction_tree(0), __has_triangulation(false),
        __has_triangulated_graph(false), __has_elimination_tree(false),
        __has_junction_tree(false), __has_max_prime_junction_tree(false),
        __has_fill_ins(false), __minimality_required(minimality),
        __we_want_fill_ins(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(StaticTriangulation);

    // register the triangulation to its junction tree strategy
    _junction_tree_strategy->setTriangulation(this);
  }

  /// destructor

  StaticTriangulation::~StaticTriangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR(StaticTriangulation);
    delete _elimination_sequence_strategy;
    delete _junction_tree_strategy;
  }

  /// reinitialize the graph to be triangulated to an empty graph

  void StaticTriangulation::clear() {
    // clear the factories
    _elimination_sequence_strategy->clear();
    _junction_tree_strategy->clear();

    // remove the current graphs
    __original_graph = 0;
    _modalities.clear();
    __junction_tree = 0;
    __triangulated_graph.clear();
    __elim_tree.clear();
    __max_prime_junction_tree.clear();
    __elim_cliques.clear();
    __node_2_max_prime_clique.clear();

    // remove all fill-ins and orderings
    __fill_ins.clear();
    __added_fill_ins.clear();
    __elim_order.clear();
    __reverse_elim_order.clear();

    // indicates that the junction tree, the max prime tree, etc, are updated
    __has_triangulation = true;
    __has_triangulated_graph = true;
    __has_elimination_tree = true;
    __has_junction_tree = true;
    __has_max_prime_junction_tree = true;
    __has_fill_ins = true;
  }

  // removes redondant fill-ins and compute proper elimination cliques and order
  void StaticTriangulation::__computeRecursiveThinning() {
    // apply recursive thinning (fmint, see Kjaerulff (90), "triangulation of
    // graphs - algorithms giving small total state space")
    NodeId node1, node2;
    bool incomplete;
    std::vector<NodeId> adj;
    EdgeSet T_prime;
    NodeProperty<unsigned int> R(__triangulated_graph.size());

    for (const auto node : __triangulated_graph.nodes())
      R.insert(node, 0);

    // the FMINT loop
    for (unsigned int i = __added_fill_ins.size() - 1; i < __added_fill_ins.size();
         --i) {
      if (__added_fill_ins[i].size()) {
        // here apply MINT to T_i = __added_fill_ins[i]
        EdgeSet &T = __added_fill_ins[i];

        // compute R: by default, R is equal to all the nodes in the graph
        // when R[...] >= j (see the j in the loop below), this means that the node
        // belongs to an extremal edge in R
        for (unsigned int k = 0; k < __elim_order.size(); ++k)
          R[__elim_order[k]] = 0; // WARNING: do not replace R[__elim_order[k]] by

        // R[k] because the node ids may not be
        // consecutive numbers

        // apply MINT while some edges can possibly be deleted
        bool require_mint = true;

        for (unsigned int j = 0; require_mint; ++j) {
          // find T' (it is equal to the edges (v,w) of T such that
          // the intersection of adj(v,G) and adj(w,G) is complete and such that
          // v and/or w belongs to an extremal node in R
          for (const auto edge : T) {
            node1 = edge.first();
            node2 = edge.second();

            // check if at least one extremal node belongs to R
            if ((R[node1] < j) && (R[node2] < j))
              continue;

            // check if the intersection of adj(v,G) and adj(w,G) is a
            // complete subgraph
            if (__triangulated_graph.neighbours(node2).size() <
                __triangulated_graph.neighbours(node1).size()) {
              NodeId tmp = node1;
              node1 = node2;
              node2 = tmp;
            }

            incomplete = false;

            // find the nodes belonging to the intersection of adj(v,G)
            // and adj(w,G)
            for (const auto adjnode : __triangulated_graph.neighbours(node1))
              if (__triangulated_graph.existsEdge(node2, adjnode))
                adj.push_back(adjnode);

            // check if the intersection is complete
            for (unsigned int k = 0; k < adj.size() && !incomplete; ++k) {
              for (unsigned int m = k + 1; m < adj.size(); ++m) {
                if (!__triangulated_graph.existsEdge(adj[k], adj[m])) {
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
          for (const auto edge : T_prime) {
            T.erase(edge);
            __triangulated_graph.eraseEdge(edge);

            if (__has_fill_ins)
              __fill_ins.erase(edge);
          }

          if (T_prime.size() == 0)
            require_mint = false;

          T_prime.clear();
        }
      }
    }

    // here, the recursive thinning has removed all the superfluous edges.
    // Hence some cliques have been split and, as a result, the elimination order
    // has changed. We should thus compute a new perfect ordering. To do so,
    // we use a Maximal Cardinality Search: it has indeed the nice property that,
    // when the graph is already triangulated, it finds a compatible order of
    // elimination that does not require any edge addition

    // a structure storing the number of neighbours previously processed
    PriorityQueue<NodeId, unsigned int, std::greater<unsigned int>>
    numbered_neighbours(std::greater<unsigned int>(), __triangulated_graph.size());

    for (unsigned int i = 0; i < __elim_order.size(); ++i)
      numbered_neighbours.insert(__elim_order[i], 0);

    // perform the maximum cardinality search
    for (unsigned int i = __elim_order.size() - 1; i < __elim_order.size(); --i) {
      NodeId node = numbered_neighbours.pop();
      __elim_order[i] = node;
      __reverse_elim_order[node] = i;

      for (const auto neighbour : __triangulated_graph.neighbours(node)) {
        try {
          numbered_neighbours.setPriority(
              neighbour, 1 + numbered_neighbours.priority(neighbour));
        } catch (NotFound &) {
        }
      }
    }

    // here the elimination order is ok. We now need to update the __elim_cliques
    for (unsigned int i = 0; i < __elim_order.size(); ++i) {
      NodeSet &cliques = __elim_cliques.insert(__elim_order[i], NodeSet()).second;
      cliques << __elim_order[i];

      for (const auto neighbour : __triangulated_graph.neighbours(__elim_order[i]))
        if (__reverse_elim_order[neighbour] > i)
          cliques << neighbour;
    }
  }

  /// returns an elimination tree from a triangulated graph

  void StaticTriangulation::__computeEliminationTree() {
    // if there already exists an elimination tree, no need to compute it again
    if (__has_elimination_tree)
      return;

    // if the graph is not triangulated, triangulate it
    if (!__has_triangulation)
      __triangulate();

    // create the nodes of the elimination tree
    __elim_tree.clear();

    for (unsigned int i = 0; i < __elim_order.size(); ++i)
      __elim_tree.addNode(i, __elim_cliques[__elim_order[i]]);

    // create the edges of the elimination tree: join a node to the one in
    // its clique that is eliminated first
    for (unsigned int i = 0; i < __elim_order.size(); ++i) {
      NodeId clique_i_creator = __elim_order[i];
      const NodeSet &list_of_nodes = __elim_cliques[clique_i_creator];
      unsigned int child = __original_graph->bound() + 1;

      for (const auto node : list_of_nodes) {
        unsigned int it_elim_step = __reverse_elim_order[node];

        if ((node != clique_i_creator) && (child > it_elim_step))
          child = it_elim_step;
      }

      if (child <= __original_graph->bound()) {
        // WARNING: here, we assume that the nodes of the elimination tree are
        // indexed from 0 to n-1
        __elim_tree.addEdge(i, child);
      }
    }

    __has_elimination_tree = true;
  }

  /// used for computing the junction tree of the maximal prime subgraphs

  void StaticTriangulation::__computeMaxPrimeMergings(
      const NodeId node, const NodeId from, std::vector<Arc> &merged_cliques,
      NodeSet &mark) const {
    mark << node;

    for (const auto other_node : __junction_tree->neighbours(node))
      if (other_node != from) {
        const NodeSet &separator = __junction_tree->separator(node, other_node);
        // check that the separator between node and other_node is complete
        bool complete = true;

        for (auto iter_sep1 = separator.begin();
             iter_sep1 != separator.end() && complete; ++iter_sep1) {
          auto iter_sep2 = iter_sep1;

          for (++iter_sep2; iter_sep2 != separator.end(); ++iter_sep2) {
            if (!__original_graph->existsEdge(*iter_sep1, *iter_sep2)) {
              complete = false;
              break;
            }
          }
        }

        // here complete indicates whether the separator is complete or not
        if (!complete)
          merged_cliques.push_back(Arc(other_node, node));

        __computeMaxPrimeMergings(other_node, node, merged_cliques, mark);
      }
  }

  /// computes the junction tree of the maximal prime subgraphs

  void StaticTriangulation::__computeMaxPrimeJunctionTree() {
    // if there already exists a max prime junction tree, no need to recompute it
    if (__has_max_prime_junction_tree)
      return;

    // if there is no junction tree, create it
    if (!__has_junction_tree)
      junctionTree();

    // the maximal prime subgraph join tree is created by aggregation of some
    // cliques. More precisely, when the separator between 2 cliques is not
    // complete in the original graph, then the two cliques must be merged.
    // Create a hashtable indicating which clique has been absorbed by some other
    // clique.
    NodeProperty<NodeId> T_mpd_cliques(__junction_tree->size());

    for (const auto clik : __junction_tree->nodes())
      T_mpd_cliques.insert(clik, clik);

    // parse all the separators of the junction tree and test those that are not
    // complete in the orginal graph
    std::vector<Arc> merged_cliques;

    NodeSet mark;

    for (const auto clik : __junction_tree->nodes())
      if (!mark.contains(clik))
        __computeMaxPrimeMergings(clik, clik, merged_cliques, mark);

    // compute the transitive closure of merged_cliques. This one will contain
    // pairs (X,Y) indicating that clique X must be merged with clique Y.
    // Actually clique X will be inserted into clique Y.
    for (unsigned int i = 0; i < merged_cliques.size(); ++i) {
      T_mpd_cliques[merged_cliques[i].tail()] =
          T_mpd_cliques[merged_cliques[i].head()];
    }

    // now we can create the max prime junction tree. First, create the cliques
    for (const auto &elt : T_mpd_cliques)
      if (elt.first == elt.second)
        __max_prime_junction_tree.addNode(elt.second,
                                          __junction_tree->clique(elt.second));

    // add to the cliques previously created the nodes of the cliques that were
    // merged into them
    for (const auto &elt : T_mpd_cliques)
      if (elt.first != elt.second)
        for (const auto node : __junction_tree->clique(elt.first)) {
          try {
            __max_prime_junction_tree.addToClique(elt.second, node);
          } catch (DuplicateElement &) {
          }
        }

    // add the edges to the graph
    for (const auto edge : __junction_tree->edges()) {
      NodeId node1 = T_mpd_cliques[edge.first()];
      NodeId node2 = T_mpd_cliques[edge.second()];

      if (node1 != node2) {
        try {
          __max_prime_junction_tree.addEdge(node1, node2);
        } catch (DuplicateElement &) {
        }
      }
    }

    // compute for each node which clique of the max prime junction tree was
    // created by the elimination of the node
    const NodeProperty<NodeId> &node_2_junction_clique =
        _junction_tree_strategy->createdCliques();

    for (const auto &elt : node_2_junction_clique)
      __node_2_max_prime_clique.insert(elt.first, T_mpd_cliques[elt.second]);

    __has_max_prime_junction_tree = true;
  }

  /// returns the triangulated graph

  const UndiGraph &StaticTriangulation::triangulatedGraph() {
    if (!__has_triangulation)
      __triangulate();

    // if we did not construct the triangulated graph during the triangulation,
    // that is, we just constructed the junction tree, we shall construct it now
    if (!__has_triangulated_graph) {
      // just in case, be sure that the junction tree has been constructed
      if (!__has_junction_tree)
        junctionTree();

      // copy the original graph
      __triangulated_graph = *__original_graph;

      for (const auto clik_id : __junction_tree->nodes()) {
        // for each clique, add the edges necessary to make it complete
        const NodeSet &clique = __junction_tree->clique(clik_id);
        std::vector<NodeId> clique_nodes(clique.size());
        unsigned int i = 0;

        for (const auto node : clique) {
          clique_nodes[i] = node;
          i += 1;
        }

        for (i = 0; i < clique_nodes.size(); ++i) {
          for (unsigned int j = i + 1; j < clique_nodes.size(); ++j) {
            try {
              __triangulated_graph.addEdge(clique_nodes[i], clique_nodes[j]);
            } catch (DuplicateElement &) {
            }
          }
        }
      }

      __has_triangulated_graph = true;
    }

    return __triangulated_graph;
  }

  /* initialize the triangulation algorithm for a new graph
   * @param gr the new @ref UndiGraph
   * @param modal the @ref Property giving the number of modalities for each node in
   * the graph
   */
  void StaticTriangulation::_setGraph(const UndiGraph *gr,
                                      const NodeProperty<Size> *modal) {
    // remove the old graph
    clear();

    // prepare the size of the data for the new graph
    if (gr) {
      __elim_order.resize(gr->size());
      __reverse_elim_order.resize(gr->size());
      __elim_cliques.resize(gr->size());
      __added_fill_ins.resize(gr->size());
      __node_2_max_prime_clique.resize(gr->size());
    }

    // copy the graph passed in argument
    __original_graph = gr;
    _modalities = *modal;

    // indicate that no triangulation was performed for this graph
    __has_triangulation = false;
    __has_triangulated_graph = false;
    __has_elimination_tree = false;
    __has_junction_tree = false;
    __has_max_prime_junction_tree = false;
    __has_fill_ins = false;
  }

  /// the function that performs the triangulation

  void StaticTriangulation::__triangulate() {
    // if the graph is already triangulated, no need to triangulate it again
    if (__has_triangulation)
      return;

    // copy the graph to be triangulated, so that we can modify it
    UndiGraph tmp_graph = *__original_graph;

    _initTriangulation(tmp_graph);
    _elimination_sequence_strategy->askFillIns(__we_want_fill_ins);

    // if we are to do recursive thinning, we will have to add fill-ins to the
    // triangulated graph each time we eliminate a node. Hence, we shall initialize
    // the triangulated graph by a copy of the original graph
    if (__minimality_required) {
      __triangulated_graph = *__original_graph;
      __has_triangulated_graph = true;
    }

    // perform the triangulation
    NodeId removable_node = 0;

    for (unsigned int nb_elim = 0; tmp_graph.size() != 0; ++nb_elim) {
      removable_node = _elimination_sequence_strategy->nextNodeToEliminate();

      // when minimality is not required, i.e., we won't apply recursive thinning,
      // the cliques sets can be computed
      if (!__minimality_required) {
        NodeSet &cliques = __elim_cliques.insert(removable_node, NodeSet()).second;
        cliques.resize(tmp_graph.neighbours(removable_node).size() / 2);
        cliques << removable_node;

        for (const auto clik : tmp_graph.neighbours(removable_node))
          cliques << clik;
      } else {
        // here recursive thinning will be applied, hence we need store the
        // fill-ins added by the current node removal
        EdgeSet &current_fill = __added_fill_ins[nb_elim];
        NodeId node1, node2;

        const NodeSet &nei = tmp_graph.neighbours(removable_node);

        for (auto iter_edges1 = nei.begin(); iter_edges1 != nei.end();
             ++iter_edges1) {
          node1 = *iter_edges1;
          auto iter_edges2 = iter_edges1;

          for (++iter_edges2; iter_edges2 != nei.end(); ++iter_edges2) {
            node2 = *iter_edges2;
            Edge edge(node1, node2);

            if (!tmp_graph.existsEdge(edge)) {
              current_fill.insert(edge);
              __triangulated_graph.addEdge(node1, node2);
            }
          }
        }
      }

      // if we want fill-ins but the elimination sequence strategy does not
      // compute them, we shall compute them here
      if (__we_want_fill_ins && !_elimination_sequence_strategy->providesFillIns()) {
        NodeId node1, node2;

        // add edges between removable_node's neighbours in order to create
        // a clique
        const NodeSet &nei = tmp_graph.neighbours(removable_node);

        for (auto iter_edges1 = nei.begin(); iter_edges1 != nei.end();
             ++iter_edges1) {
          node1 = *iter_edges1;
          auto iter_edges2 = iter_edges1;

          for (++iter_edges2; iter_edges2 != nei.end(); ++iter_edges2) {
            node2 = *iter_edges2;
            Edge edge(node1, node2);

            if (!tmp_graph.existsEdge(edge)) {
              __fill_ins.insert(edge);
            }
          }
        }

        // delete removable_node and its adjacent edges
        tmp_graph.eraseNode(removable_node);
      }

      // inform the elimination sequence that we are actually removing
      // removable_node (this enables, for instance, to update the weights of
      // the nodes in the graph)
      _elimination_sequence_strategy->eliminationUpdate(removable_node);

      if (!_elimination_sequence_strategy->providesGraphUpdate()) {
        NodeId node1, node2;

        const NodeSet &nei = tmp_graph.neighbours(removable_node);

        for (auto iter_edges1 = nei.begin(); iter_edges1 != nei.end();
             ++iter_edges1) {
          node1 = *iter_edges1;
          auto iter_edges2 = iter_edges1;

          for (++iter_edges2; iter_edges2 != nei.end(); ++iter_edges2) {
            node2 = *iter_edges2;
            Edge edge(node1, node2);

            if (!tmp_graph.existsEdge(edge)) {
              tmp_graph.addEdge(node1, node2);
            }
          }
        }

        tmp_graph.eraseNode(removable_node);
      }

      // update the elimination order
      __elim_order[nb_elim] = removable_node;
      __reverse_elim_order.insert(removable_node, nb_elim);
    }

    // indicate whether we actually computed fill-ins
    __has_fill_ins = __we_want_fill_ins;

    // if minimality is required, remove the redondant edges
    if (__minimality_required)
      __computeRecursiveThinning();

    __has_triangulation = true;
  }

  /// returns the fill-ins added by the triangulation algorithm

  const EdgeSet &StaticTriangulation::fillIns() {
    // if we did not compute the triangulation yet, do it and commpute
    // the fill-ins at the same time
    if (!__has_triangulation) {
      bool want_fill_ins = __we_want_fill_ins;
      __we_want_fill_ins = true;
      __triangulate();
      __we_want_fill_ins = want_fill_ins;
      __has_fill_ins = true;
    }

    // here, we already computed a triangulation and we already computed
    // the fill-ins, so return them
    if (__has_fill_ins) {
      if (_elimination_sequence_strategy->providesFillIns())
        return _elimination_sequence_strategy->fillIns();
      else
        return __fill_ins;
    } else {
      // ok, here, we shall compute the fill-ins as they were not precomputed
      if (!__original_graph)
        return __fill_ins;

      // just in case, be sure that the junction tree has been constructed
      if (!__has_junction_tree)
        junctionTree();

      for (const auto clik_id : __junction_tree->nodes()) {
        // for each clique, add the edges necessary to make it complete
        const NodeSet &clique = __junction_tree->clique(clik_id);
        std::vector<NodeId> clique_nodes(clique.size());
        unsigned int i = 0;

        for (const auto node : clique) {
          clique_nodes[i] = node;
          i += 1;
        }

        for (i = 0; i < clique_nodes.size(); ++i) {
          for (unsigned int j = i + 1; j < clique_nodes.size(); ++j) {
            Edge edge(clique_nodes[i], clique_nodes[j]);

            if (!__original_graph->existsEdge(edge)) {
              try {
                __fill_ins.insert(edge);
              } catch (DuplicateElement &) {
              }
            }
          }
        }
      }

      return __fill_ins;
    }
  }

} /* namespace gum */
