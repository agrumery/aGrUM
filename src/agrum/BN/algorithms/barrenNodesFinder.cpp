/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Detect barren nodes for inference in Bayesian networks
 */
#include <limits>

#include <agrum/BN/algorithms/barrenNodesFinder.h>

#ifdef GUM_NO_INLINE
#  include <agrum/BN/algorithms/barrenNodesFinder_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  /// returns the set of barren nodes in the messages sent in a junction tree
  ArcProperty< NodeSet > BarrenNodesFinder::barrenNodes(const CliqueGraph& junction_tree) {
    // assign a mark to all the nodes
    // and mark all the observed nodes and their ancestors as non-barren
    NodeProperty< Size > mark(_dag_->size());
    {
      for (const auto node: *_dag_)
        mark.insert(node, 0);   // for the moment, 0 = possibly barren

      // mark all the observed nodes and their ancestors as non barren
      // std::numeric_limits<unsigned int>::max () will be necessarily non
      // barren
      // later on
      Sequence< NodeId > observed_anc(_dag_->size());
      const Size         non_barren = std::numeric_limits< Size >::max();
      for (const auto node: *_observed_nodes_)
        observed_anc.insert(node);
      for (Idx i = 0; i < observed_anc.size(); ++i) {
        const NodeId node = observed_anc[i];
        if (!mark[node]) {
          mark[node] = non_barren;
          for (const auto par: _dag_->parents(node)) {
            if (!mark[par] && !observed_anc.exists(par)) { observed_anc.insert(par); }
          }
        }
      }
    }

    // create the data structure that will contain the result of the
    // method. By default, we assume that, for each pair of adjacent cliques,
    // all
    // the nodes that do not belong to their separator are possibly barren and,
    // by sweeping the dag, we will remove the nodes that were determined
    // above as non-barren. Structure result will assign to each (ordered) pair
    // of adjacent cliques its set of barren nodes.
    ArcProperty< NodeSet > result;
    for (const auto& edge: junction_tree.edges()) {
      const NodeSet& separator = junction_tree.separator(edge);

      NodeSet non_barren1 = junction_tree.clique(edge.first());
      for (auto iter = non_barren1.beginSafe(); iter != non_barren1.endSafe(); ++iter) {
        if (mark[*iter] || separator.exists(*iter)) { non_barren1.erase(iter); }
      }
      result.insert(Arc(edge.first(), edge.second()), std::move(non_barren1));

      NodeSet non_barren2 = junction_tree.clique(edge.second());
      for (auto iter = non_barren2.beginSafe(); iter != non_barren2.endSafe(); ++iter) {
        if (mark[*iter] || separator.exists(*iter)) { non_barren2.erase(iter); }
      }
      result.insert(Arc(edge.second(), edge.first()), std::move(non_barren2));
    }

    // for each node in the DAG, indicate which are the arcs in the result
    // structure whose separator contain it: the separators are actually the
    // targets of the queries.
    NodeProperty< ArcSet > node2arc;
    for (const auto node: *_dag_)
      node2arc.insert(node, ArcSet());
    for (const auto& elt: result) {
      const Arc& arc = elt.first;
      if (!result[arc].empty()) {    // no need to further process cliques
        const NodeSet& separator =   // with no barren nodes
            junction_tree.separator(Edge(arc.tail(), arc.head()));

        for (const auto node: separator) {
          node2arc[node].insert(arc);
        }
      }
    }

    // To determine the set of non-barren nodes w.r.t. a given single node
    // query, we rely on the fact that those are precisely all the ancestors of
    // this single node. To mutualize the computations, we will thus sweep the
    // DAG from top to bottom and exploit the fact that the set of ancestors of
    // the child of a given node A contain the ancestors of A. Therefore, we
    // will
    // determine sets of paths in the DAG and, for each path, compute the set of
    // its barren nodes from the source to the destination of the path. The
    // optimal set of paths, i.e., that which will minimize computations, is
    // obtained by solving a "minimum path cover in directed acyclic graphs".
    // But
    // such an algorithm is too costly for the gain we can get from it, so we
    // will
    // rely on a simple heuristics.

    // To compute the heuristics, we proceed as follows:
    // 1/ we mark to 1 all the nodes that are ancestors of at least one (key)
    // node
    //     with a non-empty arcset in node2arc and we extract from those the
    //     roots, i.e., those nodes whose set of parents, if any, have all been
    //     identified as non-barren by being marked as
    //     std::numeric_limits<unsigned int>::max (). Such nodes are
    //     thus the top of the graph to sweep.
    // 2/ create a copy of the subgraph of the DAG w.r.t. the 1-marked nodes
    //     and, for each node, if the node has several parents and children,
    //     keep only one arc from one of the parents to the child with the
    //     smallest
    //     number of parents, and try to create a matching between parents and
    //     children and add one arc for each edge of this matching. This will
    //     allow
    //     us to create distinct paths in the DAG. Whenever a child has no more
    //     parents, it becomes the root of a new path.
    // 3/ the sweeping will be performed from the roots of all these paths.

    // perform step 1/
    NodeSet path_roots;
    {
      List< NodeId > nodes_to_mark;
      for (const auto& elt: node2arc) {
        if (!elt.second.empty()) {   // only process nodes with assigned arcs
          nodes_to_mark.insert(elt.first);
        }
      }
      while (!nodes_to_mark.empty()) {
        NodeId node = nodes_to_mark.front();
        nodes_to_mark.popFront();

        if (!mark[node]) {   // mark the node and all its ancestors
          mark[node]  = 1;
          Size nb_par = 0;
          for (auto par: _dag_->parents(node)) {
            Size parent_mark = mark[par];
            if (parent_mark != std::numeric_limits< Size >::max()) {
              ++nb_par;
              if (parent_mark == 0) { nodes_to_mark.insert(par); }
            }
          }

          if (nb_par == 0) { path_roots.insert(node); }
        }
      }
    }

    // perform step 2/
    DAG sweep_dag = *_dag_;
    for (const auto node: *_dag_) {   // keep only nodes marked with 1
      if (mark[node] != 1) { sweep_dag.eraseNode(node); }
    }
    for (const auto node: sweep_dag) {
      const Size nb_parents  = sweep_dag.parents(node).size();
      const Size nb_children = sweep_dag.children(node).size();
      if ((nb_parents > 1) || (nb_children > 1)) {
        // perform the matching
        const auto& parents = sweep_dag.parents(node);

        // if there is no child, remove all the parents except the first one
        if (nb_children == 0) {
          auto iter_par = parents.beginSafe();
          for (++iter_par; iter_par != parents.endSafe(); ++iter_par) {
            sweep_dag.eraseArc(Arc(*iter_par, node));
          }
        } else {
          // find the child with the smallest number of parents
          const auto& children        = sweep_dag.children(node);
          NodeId      smallest_child  = 0;
          Size        smallest_nb_par = std::numeric_limits< Size >::max();
          for (const auto child: children) {
            const auto new_nb = sweep_dag.parents(child).size();
            if (new_nb < smallest_nb_par) {
              smallest_nb_par = new_nb;
              smallest_child  = child;
            }
          }

          // if there is no parent, just keep the link with the smallest child
          // and remove all the other arcs
          if (nb_parents == 0) {
            for (auto iter = children.beginSafe(); iter != children.endSafe(); ++iter) {
              if (*iter != smallest_child) {
                if (sweep_dag.parents(*iter).size() == 1) { path_roots.insert(*iter); }
                sweep_dag.eraseArc(Arc(node, *iter));
              }
            }
          } else {
            auto nb_match = Size(std::min(nb_parents, nb_children) - 1);
            auto iter_par = parents.beginSafe();
            ++iter_par;   // skip the first parent, whose arc with node will
                          // remain
            auto iter_child = children.beginSafe();
            for (Idx i = 0; i < nb_match; ++i, ++iter_par, ++iter_child) {
              if (*iter_child == smallest_child) { ++iter_child; }
              sweep_dag.addArc(*iter_par, *iter_child);
              sweep_dag.eraseArc(Arc(*iter_par, node));
              sweep_dag.eraseArc(Arc(node, *iter_child));
            }
            for (; iter_par != parents.endSafe(); ++iter_par) {
              sweep_dag.eraseArc(Arc(*iter_par, node));
            }
            for (; iter_child != children.endSafe(); ++iter_child) {
              if (*iter_child != smallest_child) {
                if (sweep_dag.parents(*iter_child).size() == 1) { path_roots.insert(*iter_child); }
                sweep_dag.eraseArc(Arc(node, *iter_child));
              }
            }
          }
        }
      }
    }

    // step 3: sweep the paths from the roots of sweep_dag
    // here, the idea is that, for each path of sweep_dag, the mark we put
    // to the ancestors is a given number, say N, that increases from path
    // to path. Hence, for a given path, all the nodes that are marked with a
    // number at least as high as N are non-barren, the others being barren.
    Idx mark_id = 2;
    for (NodeId path: path_roots) {
      // perform the sweeping from the path
      while (true) {
        // mark all the ancestors of the node
        List< NodeId > to_mark{path};
        while (!to_mark.empty()) {
          NodeId node = to_mark.front();
          to_mark.popFront();
          if (mark[node] < mark_id) {
            mark[node] = mark_id;
            for (const auto par: _dag_->parents(node)) {
              if (mark[par] < mark_id) { to_mark.insert(par); }
            }
          }
        }

        // now, get all the arcs that contained node "path" in their separator.
        // this node acts as a query target and, therefore, its ancestors
        // shall be non-barren.
        const ArcSet& arcs = node2arc[path];
        for (const auto& arc: arcs) {
          NodeSet& barren = result[arc];
          for (auto iter = barren.beginSafe(); iter != barren.endSafe(); ++iter) {
            if (mark[*iter] >= mark_id) {
              // this indicates a non-barren node
              barren.erase(iter);
            }
          }
        }

        // go to the next sweeping node
        const NodeSet& sweep_children = sweep_dag.children(path);
        if (sweep_children.size()) {
          path = *(sweep_children.begin());
        } else {
          // here, the path has ended, so we shall go to the next path
          ++mark_id;
          break;
        }
      }
    }

    return result;
  }

  /// returns the set of barren nodes
  NodeSet BarrenNodesFinder::barrenNodes() {
    // mark all the nodes in the dag as barren (true)
    NodeProperty< bool > barren_mark = _dag_->nodesPropertyFromVal(true);

    // mark all the ancestors of the evidence and targets as non-barren
    List< NodeId > nodes_to_examine;
    int            nb_non_barren = 0;
    for (const auto node: *_observed_nodes_)
      nodes_to_examine.insert(node);
    for (const auto node: *_target_nodes_)
      nodes_to_examine.insert(node);

    while (!nodes_to_examine.empty()) {
      const NodeId node = nodes_to_examine.front();
      nodes_to_examine.popFront();
      if (barren_mark[node]) {
        barren_mark[node] = false;
        ++nb_non_barren;
        for (const auto par: _dag_->parents(node))
          nodes_to_examine.insert(par);
      }
    }

    // here, all the nodes marked true are barren
    NodeSet barren_nodes(_dag_->sizeNodes() - nb_non_barren);
    for (const auto& marked_pair: barren_mark)
      if (marked_pair.second) barren_nodes.insert(marked_pair.first);

    return barren_nodes;
  }

} /* namespace gum */
