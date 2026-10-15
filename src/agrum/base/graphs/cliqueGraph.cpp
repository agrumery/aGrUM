/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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
#include <cmath>
#include <sstream>

#include <agrum/base/core/dot/dotElements.h>
#include <agrum/base/graphs/cliqueGraph.h>


#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/cliqueGraph_inl.h>
#endif   // GU%_NO_INLINE

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

  CliqueGraph::CliqueGraph(CliqueGraph&& from) :
      NodeGraphPart(std::move(from)),                // virtual base — must be listed explicitly
      UndiGraph(std::move(from)), _cliques_(std::move(from._cliques_)),
      _separators_(std::move(from._separators_)) {   // for debugging purposes
    GUM_CONS_MOV(CliqueGraph)
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
    auto opt = undirectedPath(container(node1), container(node2));
    if (!opt) GUM_ERROR(NotFound, "no path between cliques containing the given nodes")
    std::vector< NodeId > path = std::move(*opt);

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
    if (!UndiGraph::operator==(from)) return false;

    // check if the  _cliques_ are identical
    for (const auto& [node, nodes]: _cliques_)
      if (nodes != from._cliques_[node]) return false;

    return true;
  }

  std::string CliqueGraph::toString() const {
    std::stringstream stream;
    stream << "list of nodes:\n";

    for (const auto node: nodes()) {
      stream << std::format(" -- node: {}\n    clique:", node);

      for (const auto cliq: clique(node))
        stream << "  " << cliq;

      stream << '\n';
    }

    stream << "\n\nlist of edges:\n";

    for (const auto& edge: edges())
      stream << edge << "  ";

    return stream.str();
  }

  namespace {
    // "0-1-3", the identifiers of a set of nodes
    std::string expandCliqueContent(const NodeSet& clique, std::string_view delim = "-") {
      std::string result;
      bool        first = true;

      std::vector< NodeId > sorted(clique.begin(), clique.end());
      std::sort(sorted.begin(), sorted.end());
      for (auto node: sorted) {
        if (!first) { result += delim; }
        result += std::to_string(node);
        first = false;
      }

      return result;
    }

    // "size : 2", then one identifier by line
    std::string expandCliqueTooltip(const NodeSet& clique) {
      return std::format("size : {}\n{}", clique.size(), expandCliqueContent(clique, "\n"));
    }

    // the node that stands for the separator of an edge
    std::string separatorId(const Edge& edge) {
      return std::format("{}~{}", edge.first(), edge.second());
    }

    // a size, without the rounding errors of the products (0.3 * 4 is 1.2000000000000002)
    std::string dotSize(double value) {
      return dot::DotWriter::number(std::round(value * 1e6) / 1e6);
    }
  }   // namespace

  std::string CliqueGraph::toDot() const { return toDot(dot::DotOptions()); }

  std::string CliqueGraph::toDot(const dot::DotOptions& options) const {
    const auto     style = dot::JunctionTreeDotStyle::read();
    dot::DotWriter writer(false);
    dot::writePreamble(writer, style, options);

    // the font color of a node is adaptive against its own background
    auto cliqueStyle           = static_cast< dot::DotStyle >(style);
    cliqueStyle.nodeBgColor    = style.cliqueBgColor;
    auto separatorStyle        = static_cast< dot::DotStyle >(style);
    separatorStyle.nodeBgColor = style.separatorBgColor;

    // cliques as nodes
    for (const auto node: nodes()) {
      auto attributes = dot::nodeAttributes(cliqueStyle,
                                            options.node(node),
                                            expandCliqueContent(clique(node)),
                                            expandCliqueTooltip(clique(node)));
      dot::addDefaultAttribute(attributes, "fontsize", std::to_string(style.cliqueFontSize));
      dot::addDefaultAttribute(attributes, "fontcolor", style.cliqueFgColor);
      dot::addDefaultAttribute(attributes, "fillcolor", style.cliqueBgColor);
      writer.node(std::to_string(node), attributes);
    }

    // separators as nodes : the options of an edge are the ones of its separator
    for (const auto& edge: edges()) {
      const auto  sep = separator(edge);
      const auto* arc = options.edge(edge.first(), edge.second());

      dot::DotNodeOptions separatorOptions;
      if (arc != nullptr) {
        separatorOptions.bgColor    = arc->color;
        separatorOptions.label      = arc->label;
        separatorOptions.tooltip    = arc->tooltip;
        separatorOptions.attributes = arc->attributes;
      }

      auto attributes = dot::nodeAttributes(separatorStyle,
                                            arc != nullptr ? &separatorOptions : nullptr,
                                            expandCliqueContent(sep),
                                            expandCliqueTooltip(sep));
      dot::addDefaultAttribute(attributes, "margin", "0.02");
      dot::addDefaultAttribute(attributes, "height", "0");
      dot::addDefaultAttribute(attributes, "width", "0");
      dot::addDefaultAttribute(attributes, "shape", "box");
      dot::addDefaultAttribute(attributes, "fontsize", std::to_string(style.separatorFontSize));
      dot::addDefaultAttribute(attributes, "fontcolor", style.separatorFgColor);
      dot::addDefaultAttribute(attributes, "fillcolor", style.separatorBgColor);
      writer.node(separatorId(edge), attributes);
    }

    // edges now as c1--sep--c2
    for (const auto& edge: edges()) {
      dot::DotAttributes link;
      const auto*        arc = options.edge(edge.first(), edge.second());
      if (arc != nullptr && arc->width.has_value())
        link.push_back({"penwidth", dot::DotWriter::number(*arc->width)});

      writer.edge(std::to_string(edge.first()), separatorId(edge), link);
      writer.edge(separatorId(edge), std::to_string(edge.second()), link);
    }

    return writer.str();
  }

  std::string CliqueGraph::mapToDot() const {
    const auto style = dot::JunctionTreeDotStyle::read();
    return mapToDot(style.mapCliqueScale,
                    style.mapSepScale,
                    style.mapEdgeLen,
                    style.cliqueBgColor,
                    style.separatorBgColor);
  }

  std::string CliqueGraph::mapToDot(double           scaleClique,
                                    double           scaleSep,
                                    double           lenEdge,
                                    std::string_view colorClique,
                                    std::string_view colorSep) const {
    const auto style = dot::JunctionTreeDotStyle::read();

    dot::DotWriter writer(false);
    writer.graphAttributes(
        {{"bgcolor", "transparent"}, {"layout", "neato"}, {"size", style.mapSize}});
    writer.nodeDefaults(
        {{"shape", "point"},
         {"style", "filled"},
         {"fillcolor", std::string(colorClique.empty() ? style.cliqueBgColor : colorClique)}});
    writer.edgeDefaults({{"len", dot::DotWriter::number(lenEdge)}, {"color", style.arcColor}});

    // cliques as nodes
    for (const auto node: nodes()) {
      const auto& clik = clique(node);
      writer.node(std::to_string(node),
                  {{"tooltip", expandCliqueTooltip(clik)},
                   {"width", dotSize(scaleClique * double(clik.size()))}});
    }

    writer.nodeDefaults(
        {{"shape", "square"},
         {"style", "filled"},
         {"fillcolor", std::string(colorSep.empty() ? style.separatorBgColor : colorSep)},
         {"label", ""}});

    // separators as nodes and edges
    for (const auto& edge: edges()) {
      const auto sep = clique(edge.first()) * clique(edge.second());
      writer.node(separatorId(edge),
                  {{"tooltip", expandCliqueTooltip(sep)},
                   {"width", dotSize(scaleSep * double(sep.size()))}});
      writer.edge(std::to_string(edge.first()), separatorId(edge));
      writer.edge(separatorId(edge), std::to_string(edge.second()));
    }

    return writer.str();
  }

  /// a << operator for CliqueGraph

  std::ostream& operator<<(std::ostream& stream, const CliqueGraph& graph) {
    stream << graph.toString();
    return stream;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
