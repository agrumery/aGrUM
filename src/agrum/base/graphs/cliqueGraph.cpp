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


/** @file
 * @brief source of basic clique graphs (join trees, etc)
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <algorithm>
#include <sstream>

#include <agrum/base/graphs/cliqueGraph.h>


#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/cliqueGraph_inl.h>
#endif   // GUM_NOINLINE

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* =================================================================== */
  /* =================================================================== */
  /* ===                  IMPLEMENTATION OF GUM_CLIQUE_GRAPH         === */
  /* =================================================================== */
  /* =================================================================== */

  /// basic constructor: creates an empty clique graph

  CliqueGraph::CliqueGraph(Size nodes_size,
                           bool nodes_resize_policy,
                           Size edges_size,
                           bool edges_resize_policy) :
      NodeGraphPart(nodes_size, nodes_resize_policy),
      UndiGraph(nodes_size, nodes_resize_policy, edges_size, edges_resize_policy) {
    // for debugging purposes
    GUM_CONSTRUCTOR(CliqueGraph)
  }

  /// copy constructor

  CliqueGraph::CliqueGraph(const CliqueGraph& from) :
      NodeGraphPart(from),   // needed because NodeGraphPart is a virtual inherited
      UndiGraph(from),       // class (see C++ FAQ Lite #25.12 for details)
      _cliques_(from._cliques_), _separators_(from._separators_) {   // for debugging purposes
    GUM_CONS_CPY(CliqueGraph)
  }

  /// destructor

  CliqueGraph::~CliqueGraph() {   // for debugging purposes
    GUM_DESTRUCTOR(CliqueGraph)
  }

  /// returns a path from a clique containing node1 to a clique containing
  /// node2

  std::vector< NodeId > CliqueGraph::containerPath(const NodeId node1, const NodeId node2) const {
    // get a path from a  _clique_ containing node1 to a  _clique_ containing
    // node2
    std::vector< NodeId > path = undirectedPath(container(node1), container(node2));

    // it may happen that the path contains several nodes containing node1 and
    // node2. Hence we shall remove the superfluous nodes
    while ((path.size() >= 2) && (clique(path[path.size() - 2]).contains(node2)))
      path.pop_back();

    while ((path.size() >= 2) && (clique(path[1]).contains(node1)))
      path.erase(path.begin());

    return path;
  }

  /// changes the set of nodes included into a given clique and returns the new
  /// set
  /// @throw DuplicateElement if node_id already is in clique_id

  void CliqueGraph::addToClique(const NodeId clique_id, const NodeId node_id) {
    // get the current clique set
    NodeSet& clique = _cliques_[clique_id];

    // check if the node already exists, in which case throw an exception
    if (clique.contains(node_id)) {
      GUM_ERROR(DuplicateElement, "the clique set already contains the node " << node_id)
    }

    clique.insert(node_id);

    // update the  _separators_ adjacent to clique 'id'
    for (const auto nei: neighbours(clique_id))
      if (_cliques_[nei].contains(node_id)) _separators_[Edge(nei, clique_id)].insert(node_id);
  }

  /// remove a node from a  _clique_

  void CliqueGraph::eraseFromClique(const NodeId clique_id, const NodeId node_id) {
    // get the current  _clique_ set
    NodeSet& clique = _cliques_[clique_id];

    // check if the node does not exist, in which case throw an exception
    if (clique.contains(node_id)) {
      clique.erase(node_id);

      // update the  _separators_ adjacent to  _clique_ 'id'
      for (const auto nei: neighbours(clique_id)) {
        Edge edge(nei, clique_id);

        if (_separators_[edge].contains(node_id)) _separators_[edge].erase(node_id);
      }
    }
  }

  /// DFS function for computing whether the running intersection property holds

  bool CliqueGraph::_runningIntersectionDFS_(const NodeId                     clique,
                                             const NodeId                     from,
                                             CliqueGraph::_RunningIntersect_& infos_DFS) const {
    // check that no node in the clique belongs to the set of nodes belonging to
    // other connected components of the cliqueGraph
    const NodeSet& nodes_clique = _cliques_[clique];

    for (const auto node: nodes_clique)
      if (infos_DFS.nodes_other_components.contains(node)) return false;

    // update the structure that keeps track of the cliques that still require
    // chains to access some of their nodes
    for (const auto node: nodes_clique)
      if (!infos_DFS.nodes_DFS_forbidden.contains(node))
        infos_DFS.cliques_DFS_chain[clique].erase(node);

    // if the clique had already been visited, no need to see its neighbours
    // or to update the list of visited nodes
    if (infos_DFS.visited_cliques.contains(clique)) return true;

    // update the list of nodes visited during the DFS
    for (const auto node: nodes_clique)
      if (!infos_DFS.nodes_DFS_seen.contains(node)) infos_DFS.nodes_DFS_seen.insert(node);

    // update the fact that the clique has been visited
    infos_DFS.visited_cliques.insert(clique);

    // check the neighbours that are different from "from" and that have not
    // been visited yet

    for (const auto otherID: neighbours(clique))
      if (otherID != from) {
        // update the list of forbidden nodes in the DFS, i.e., the nodes that
        // belong to the clique but not to the separator
        const Edge     edge(otherID, clique);
        const NodeSet& from_separ = _separators_[edge];

        for (const auto node: nodes_clique) {
          if (!from_separ.contains(node)) infos_DFS.nodes_DFS_forbidden.insert(node);
        }

        // check the neighbour
        if (!_runningIntersectionDFS_(otherID, clique, infos_DFS)) return false;

        // remove from the forbidden list the nodes that belong to clique
        for (const auto node: nodes_clique)
          infos_DFS.nodes_DFS_forbidden.erase(node);

        // check again the structure that keeps track of the cliques that still
        // require chains to access some of their nodes: the chain may be
        // the neighbour we just encountered
        for (const auto node: nodes_clique) {
          if (!infos_DFS.nodes_DFS_forbidden.contains(node))
            infos_DFS.cliques_DFS_chain[clique].erase(node);
        }
      }

    // when a node is terminal, i.e., it has at most one neighbour, add its
    // nodes
    // to the nodes forbidden by the DFS. It will prevent non adjacent extremal
    // cliques to contain the same node while this one does not belong to any
    // separator
    if (neighbours(clique).size() <= 1)
      for (const auto node: nodes_clique)
        if (!infos_DFS.nodes_DFS_forbidden.contains(node))
          infos_DFS.nodes_DFS_forbidden.insert(node);

    // here everything was OK. A priori, the running intersection property holds
    return true;
  }

  /// indicates whether the running intersection property holds

  bool CliqueGraph::hasRunningIntersection() const {
    // create a RunningIntersect structure and initialize it
    _RunningIntersect_ infos_DFS;
    infos_DFS.cliques_DFS_chain = _cliques_;

    // while there exist unvisited cliques, perform a DFS on them
    for (const auto DFSnode: nodes())
      if (!infos_DFS.visited_cliques.contains(DFSnode)) {
        // no nodes are forbidden priori in the DFS
        infos_DFS.nodes_DFS_forbidden.clear();

        // no node has already been seen in the DFS
        infos_DFS.nodes_DFS_seen.clear();

        // here iter_DFS points on a clique that has not been visited yet
        // visit the clique graph from this clique
        if (!_runningIntersectionDFS_(DFSnode, DFSnode, infos_DFS)) return false;

        // the nodes that were seen during the DFS belong to a connected
        // component
        // that is different from the connected components of the subsequent DFS
        for (const auto node: infos_DFS.nodes_DFS_seen)
          if (!infos_DFS.nodes_other_components.contains(node))
            infos_DFS.nodes_other_components.insert(node);
      }

    // check that no clique requires an additional chain to guarantee the
    // running intersection property
    for (const auto& [node, nodes]: infos_DFS.cliques_DFS_chain)
      if (!nodes.empty()) return false;

    return true;
  }

  /// checks whether two  _clique_ graphs are equal

  bool CliqueGraph::operator==(const CliqueGraph& from) const {
    // check if both graphical structures are identical
    if (UndiGraph::operator!=(from)) return false;

    // check if the  _cliques_ are identical
    for (const auto& [node, nodes]: _cliques_)
      if (nodes != from._cliques_[node]) return false;

    return true;
  }

  std::string CliqueGraph::toString() const {
    std::stringstream stream;
    stream << "list of nodes:" << std::endl;

    for (const auto node: nodes()) {
      stream << " -- node: " << node << std::endl << "    clique:";

      for (const auto cliq: clique(node))
        stream << "  " << cliq;

      stream << std::endl;
    }

    stream << "\n\nlist of edges:\n";

    for (const auto& edge: edges())
      stream << edge << "  ";

    return stream.str();
  }

  std::string expandCliqueContent(const NodeSet& clique, const std::string& delim = "-") {
    std::stringstream stream;
    bool              first = true;

    std::vector< NodeId > sorted(clique.begin(), clique.end());
    std::sort(sorted.begin(), sorted.end());
    for (auto node: sorted) {
      if (!first) { stream << delim; }

      stream << node;
      first = false;
    }

    return stream.str();
  }

  std::string expandCliqueTooltip(const NodeSet& clique) {
    std::stringstream stream;
    stream << "size : " << clique.size() << "\\n" << expandCliqueContent(clique, "\\n");
    return stream.str();
  }

  std::string expandClique(const NodeId n, const NodeSet& clique) {
    std::stringstream stream;
    stream << '(' << n << ") " << expandCliqueContent(clique);
    return stream.str();
  }

  std::string expandSeparator(const NodeId   n1,
                              const NodeSet& clique1,
                              const NodeId   n2,
                              const NodeSet& clique2) {
    std::stringstream stream;
    stream << expandClique(n1, clique1) << "^" << expandClique(n2, clique2);
    return stream.str();
  }

  std::string CliqueGraph::toDot() const {
    std::stringstream stream;
    stream << "graph {" << std::endl;
    stream << R"(  node [style="filled", fontcolor="black"];)" << std::endl;

    // cliques as nodes
    for (auto node: nodes()) {
      std::string nom = '"' + expandClique(node, clique(node)) + '"';
      stream << "  " << nom << " [label=\"" << expandCliqueContent(clique(node)) << "\",tooltip=\""
             << expandCliqueTooltip(clique(node)) << R"(",fillcolor ="burlywood"];)" << std::endl;
    }

    stream << std::endl;

    // separator as nodes
    for (const auto& edge: edges()) {
      stream << "  \""
             << expandSeparator(edge.first(),
                                clique(edge.first()),
                                edge.second(),
                                clique(edge.second()))
             << "\" [label=\"" << expandCliqueContent(separator(edge)) << "\",tooltip=\""
             << expandCliqueTooltip(separator(edge))
             << R"(",shape=box,fillcolor="palegreen",fontsize=8,width=0,height=0];)" << std::endl;
    }

    stream << std::endl;

    // edges now as c1--sep--c2
    for (const auto& edge: edges())
      stream << "  \"" << expandClique(edge.first(), clique(edge.first())) << "\"--\""
             << expandSeparator(edge.first(),
                                clique(edge.first()),
                                edge.second(),
                                clique(edge.second()))
             << "\"--\"" << expandClique(edge.second(), clique(edge.second())) << "\";"
             << std::endl;

    stream << "}" << std::endl;

    return stream.str();
  }

  std::string CliqueGraph::mapToDot(double             scaleClique,
                                    double             scaleSep,
                                    double             lenEdge,
                                    const std::string& colorClique,
                                    const std::string& colorSep) const {
    std::stringstream stream;
    stream << "graph {" << std::endl;
    stream << "  bgcolor=transparent;" << std::endl;
    stream << "  layout=neato;" << std::endl << std::endl;
    stream << "  node [shape=point,style=filled, fillcolor =" << colorClique << "];" << std::endl;
    stream << "  edge [len=" << lenEdge << "];" << std::endl << std::endl;

    // cliques as nodes
    for (auto node: nodes()) {
      const auto& clik = clique(node);
      stream << "  " << node << " [tooltip=\"" << expandCliqueTooltip(clik)
             << "\", width=" << scaleClique * double(clik.size()) << "];" << std::endl;
    }
    stream << std::endl;
    stream << "  node [shape=square,style=filled, fillcolor =" << colorSep << ",label=\"\"];"
           << std::endl
           << std::endl;

    // separator as nodes and edges
    for (const auto& edge: edges()) {
      // the separator as node
      const auto sep = clique(edge.first()) * clique(edge.second());
      stream << "  \"" << edge.first() << "~" << edge.second() << "\" [tooltip=\""
             << expandCliqueTooltip(sep) << "\""
             << ", width=" << scaleSep * double(sep.size()) << "];" << std::endl;
      // the edges
      stream << "  \"" << edge.first() << "\"--\"" << edge.first() << "~" << edge.second()
             << "\"--\"" << edge.second() << "\";" << std::endl
             << std::endl;
    }

    stream << "}" << std::endl;

    return stream.str();
  }

  /// a << operator for CliqueGraph

  std::ostream& operator<<(std::ostream& stream, const CliqueGraph& graph) {
    stream << graph.toString();
    return stream;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
