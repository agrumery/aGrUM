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
 * @brief Outlined implementation of the marked graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#include <sstream>

#include <agrum/base/core/dot/dotElements.h>
#include <agrum/base/graphs/algorithms/generic/reachability.h>
#include <agrum/base/graphs/markedGraph.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/markedGraph_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  MarkedGraph::MarkedGraph(Size nodes_size,
                           bool nodes_resize_policy,
                           Size edges_size,
                           bool edges_resize_policy) :
      NodeGraphPart(nodes_size, nodes_resize_policy),
      EdgeGraphPart(edges_size, edges_resize_policy) {
    GUM_CONSTRUCTOR(MarkedGraph);
  }

  MarkedGraph::MarkedGraph(const MarkedGraph& g) :
      NodeGraphPart(g), EdgeGraphPart(g), _marks_(g._marks_) {
    GUM_CONS_CPY(MarkedGraph);
  }

  MarkedGraph::MarkedGraph(MarkedGraph&& g) :
      NodeGraphPart(std::move(g)), EdgeGraphPart(std::move(g)), _marks_(std::move(g._marks_)) {
    GUM_CONS_MOV(MarkedGraph);
  }

  MarkedGraph::~MarkedGraph() { GUM_DESTRUCTOR(MarkedGraph); }

  MarkedGraph& MarkedGraph::operator=(const MarkedGraph& g) {
    if (this != &g) {
      MarkedGraph::clear();
      NodeGraphPart::operator=(g);
      EdgeGraphPart::operator=(g);
      _marks_ = g._marks_;
      GUM_OP_CPY(MarkedGraph);
    }
    return *this;
  }

  MarkedGraph& MarkedGraph::operator=(MarkedGraph&& g) {
    if (this != &g) {
      NodeGraphPart::operator=(std::move(g));
      EdgeGraphPart::operator=(std::move(g));
      _marks_ = std::move(g._marks_);
      GUM_OP_MOV(MarkedGraph);
    }
    return *this;
  }

  bool MarkedGraph::operator==(const MarkedGraph& g) const {
    if (!(NodeGraphPart::operator==(g)) || !(EdgeGraphPart::operator==(g))) return false;
    for (const auto& edge: edges())
      if (markAt(edge.first(), edge.second()) != g.markAt(edge.first(), edge.second())
          || markAt(edge.second(), edge.first()) != g.markAt(edge.second(), edge.first()))
        return false;
    return true;
  }

  void MarkedGraph::addEdge(NodeId x, NodeId y) {
    if (!existsEdge(x, y)) addEdge(x, y, EdgeMark::Circle, EdgeMark::Circle);
  }

  void MarkedGraph::addEdge(NodeId x, NodeId y, EdgeMark markAtX, EdgeMark markAtY) {
    if (!existsNode(x)) GUM_ERROR(InvalidNode, "Node (" << x << ") does not exist.")
    if (!existsNode(y)) GUM_ERROR(InvalidNode, "Node (" << y << ") does not exist.")
    if (x == y) GUM_ERROR(InvalidEdge, "No edge from the node " << x << " to itself.")
    if (!existsEdge(x, y)) EdgeGraphPart::addEdge(x, y);
    _marks_.set(Arc(x, y), markAtY);
    _marks_.set(Arc(y, x), markAtX);
  }

  void MarkedGraph::_eraseMarks_(NodeId x, NodeId y) {
    _marks_.erase(Arc(x, y));
    _marks_.erase(Arc(y, x));
  }

  void MarkedGraph::eraseEdge(const Edge& edge) {
    if (!existsEdge(edge)) return;
    _eraseMarks_(edge.first(), edge.second());
    EdgeGraphPart::eraseEdge(edge);
  }

  void MarkedGraph::clearEdges() {
    _marks_.clear();
    EdgeGraphPart::clearEdges();
  }

  void MarkedGraph::eraseNode(NodeId id) {
    if (!existsNode(id)) return;
    // unvirtualizedEraseNeighbours bypasses eraseEdge: the marks are erased here
    for (const auto neighbour: neighbours(id))
      _eraseMarks_(id, neighbour);
    EdgeGraphPart::unvirtualizedEraseNeighbours(id);
    NodeGraphPart::eraseNode(id);
  }

  void MarkedGraph::clear() {
    MarkedGraph::clearEdges();
    NodeGraphPart::clearNodes();
  }

  void MarkedGraph::clearNodes() { MarkedGraph::clear(); }

  EdgeMark MarkedGraph::markAt(NodeId src, NodeId dst) const {
    // the marks of the edges, and only them, are stored: one lookup checks the edge
    try {
      return _marks_[Arc(src, dst)];
    } catch (const NotFound&) { GUM_ERROR(NotFound, "No edge " << src << "-" << dst) }
  }

  void MarkedGraph::setMarkAt(NodeId src, NodeId dst, EdgeMark mark) {
    if (!existsEdge(src, dst)) GUM_ERROR(NotFound, "No edge " << src << "-" << dst)
    _marks_.set(Arc(src, dst), mark);
  }

  void MarkedGraph::reorientAllWith(EdgeMark mark) {
    for (const auto& edge: edges()) {
      _marks_.set(Arc(edge.first(), edge.second()), mark);
      _marks_.set(Arc(edge.second(), edge.first()), mark);
    }
  }

  NodeSet MarkedGraph::parents(NodeId x) const {
    NodeSet result;
    for (const auto y: neighbours(x))
      if (isDefinitelyDirected(y, x)) result.insert(y);
    return result;
  }

  NodeSet MarkedGraph::children(NodeId x) const {
    NodeSet result;
    for (const auto y: neighbours(x))
      if (isDefinitelyDirected(x, y)) result.insert(y);
    return result;
  }

  NodeSet MarkedGraph::spouses(NodeId x) const {
    NodeSet result;
    for (const auto y: neighbours(x))
      if (isBidirected(x, y)) result.insert(y);
    return result;
  }

  NodeSet MarkedGraph::undirectedNeighbours(NodeId x) const {
    NodeSet result;
    for (const auto y: neighbours(x))
      if (isUndirected(x, y)) result.insert(y);
    return result;
  }

  UndiGraph MarkedGraph::skeleton() const {
    UndiGraph skeleton;
    for (const auto node: nodes()) {
      skeleton.addNodeWithId(node);
      if (hasName(node)) skeleton.setName(node, nameFromId(node));
    }
    for (const auto& edge: edges())
      skeleton.addEdge(edge.first(), edge.second());
    return skeleton;
  }

  NodeProperty< NodeId > MarkedGraph::connectedComponents() const {
    return graph::connectedComponents(*this);   // on the edges, without copying the skeleton
  }

  namespace {
    /// the text of the mark at the left (resp. right) end of an edge
    char leftMark(EdgeMark mark) {
      switch (mark) {
        case EdgeMark::Circle : return 'o';
        case EdgeMark::Arrowhead : return '<';
        case EdgeMark::Tail : return '-';
      }
      return '?';
    }

    char rightMark(EdgeMark mark) {
      switch (mark) {
        case EdgeMark::Circle : return 'o';
        case EdgeMark::Arrowhead : return '>';
        case EdgeMark::Tail : return '-';
      }
      return '?';
    }

    /// the graphviz arrow of a mark
    std::string dotArrow(EdgeMark mark) {
      switch (mark) {
        case EdgeMark::Circle : return "odot";
        case EdgeMark::Arrowhead : return "normal";
        case EdgeMark::Tail : return "none";
      }
      return "none";
    }
  }   // namespace

  std::string MarkedGraph::toString() const {
    std::ostringstream s;
    s << NodeGraphPart::toString() << " , {";
    bool first = true;
    for (const auto& edge: edges()) {
      if (!first) s << ", ";
      first        = false;
      const auto x = edge.first();
      const auto y = edge.second();
      s << x << ' ' << leftMark(markAt(y, x)) << '-' << rightMark(markAt(x, y)) << ' ' << y;
    }
    s << '}';
    return s.str();
  }

  std::string MarkedGraph::toDot() const { return toDot(dot::DotOptions()); }

  std::string MarkedGraph::toDot(const dot::DotOptions& options) const {
    const auto     style = dot::DotStyle::read();
    dot::DotWriter writer(true);
    dot::writePreamble(writer, style, options);
    writeDotNodes(writer, style, options);

    for (const auto& edge: edges()) {
      const auto x          = edge.first();
      const auto y          = edge.second();
      auto       attributes = dot::arcAttributes(options.edge(x, y));
      if (isBidirected(x, y) && !style.bidirectedEdgeColor.empty())
        dot::addDefaultAttribute(attributes, "color", style.bidirectedEdgeColor);
      if ((isCircle(x, y) || isCircle(y, x)) && !style.circleMarkEdgeColor.empty())
        dot::addDefaultAttribute(attributes, "color", style.circleMarkEdgeColor);
      attributes.insert(attributes.begin(),
                        {{"dir", "both"},
                         {"arrowhead", dotArrow(markAt(x, y))},
                         {"arrowtail", dotArrow(markAt(y, x))}});
      writer.edge(x, y, attributes);
    }
    return writer.str();
  }

  std::ostream& operator<<(std::ostream& stream, const MarkedGraph& g) {
    stream << g.toString();
    return stream;
  }

}   // namespace gum
