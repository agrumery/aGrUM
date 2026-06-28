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
 * @brief Partial Ancestral Graph (PAG) — implementation.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <sstream>

#include <agrum/base/graphs/PAG.h>

namespace gum {

  // ##########################################################################
  // Constructors / Destructors
  // ##########################################################################

  PAG::PAG() { GUM_CONSTRUCTOR(PAG); }

  PAG::PAG(const PAG& other) : NodeGraphPart(other), UndiGraph(other), marks_(other.marks_) {
    GUM_CONS_CPY(PAG);
  }

  PAG::PAG(PAG&& other) noexcept :
      NodeGraphPart(std::move(other)), UndiGraph(std::move(other)),
      marks_(std::move(other.marks_)) {
    GUM_CONS_MOV(PAG);
  }

  PAG::~PAG() { GUM_DESTRUCTOR(PAG); }

  PAG& PAG::operator=(const PAG& other) {
    if (this != &other) {
      UndiGraph::operator=(other);
      marks_ = other.marks_;
    }
    return *this;
  }

  PAG& PAG::operator=(PAG&& other) noexcept {
    if (this != &other) {
      UndiGraph::operator=(std::move(other));
      marks_ = std::move(other.marks_);
    }
    return *this;
  }

  // ##########################################################################
  // Edge operations
  // ##########################################################################

  void PAG::addEdge(NodeId x, NodeId y) { addEdge(x, y, EdgeMark::Circle, EdgeMark::Circle); }

  void PAG::addEdge(NodeId x, NodeId y, EdgeMark markAtX, EdgeMark markAtY) {
    if (existsEdge(x, y)) { return; }
    UndiGraph::addEdge(x, y);
    // marks_[Arc(x,y)] = mark at y; marks_[Arc(y,x)] = mark at x
    marks_.insert(Arc(x, y), markAtY);
    marks_.insert(Arc(y, x), markAtX);
  }

  void PAG::eraseEdge(const Edge& e) {
    if (!existsEdge(e)) { return; }
    if (marks_.exists(Arc(e.first(), e.second()))) { marks_.erase(Arc(e.first(), e.second())); }
    if (marks_.exists(Arc(e.second(), e.first()))) { marks_.erase(Arc(e.second(), e.first())); }
    EdgeGraphPart::eraseEdge(e);
  }

  void PAG::clearEdges() {
    marks_.clear();
    EdgeGraphPart::clearEdges();
  }

  void PAG::eraseNode(NodeId id) {
    if (existsNode(id)) {
      // unvirtualizedEraseNeighbours (called by UndiGraph::eraseNode) bypasses
      // PAG::eraseEdge, so we clean marks manually before delegating
      for (const NodeId nb: neighbours(id)) {
        if (marks_.exists(Arc(id, nb))) { marks_.erase(Arc(id, nb)); }
        if (marks_.exists(Arc(nb, id))) { marks_.erase(Arc(nb, id)); }
      }
    }
    UndiGraph::eraseNode(id);
  }

  void PAG::clear() {
    marks_.clear();
    UndiGraph::clear();
  }

  // ##########################################################################
  // Mark accessors
  // ##########################################################################

  EdgeMark PAG::markAt(NodeId src, NodeId dst) const { return marks_[Arc(src, dst)]; }

  void PAG::setMarkAt(NodeId src, NodeId dst, EdgeMark m) { marks_.set(Arc(src, dst), m); }

  // ##########################################################################
  // Predicates
  // ##########################################################################

  bool PAG::isArrowhead(NodeId src, NodeId dst) const {
    return markAt(src, dst) == EdgeMark::Arrowhead;
  }

  bool PAG::isTail(NodeId src, NodeId dst) const { return markAt(src, dst) == EdgeMark::Tail; }

  bool PAG::isCircle(NodeId src, NodeId dst) const {
    return markAt(src, dst) == EdgeMark::Circle;
  }

  bool PAG::isDefinitelyDirected(NodeId x, NodeId y) const {
    // Tail at x (from y's perspective) and Arrowhead at y (from x's perspective)
    return isTail(y, x) && isArrowhead(x, y);
  }

  bool PAG::isBidirected(NodeId x, NodeId y) const {
    return isArrowhead(x, y) && isArrowhead(y, x);
  }

  bool PAG::isDefCollider(NodeId x, NodeId z, NodeId y) const {
    // z is a definite collider on x-z-y iff both endpoints at z are Arrowhead
    return isArrowhead(x, z) && isArrowhead(y, z);
  }

  // ##########################################################################
  // Bulk operations
  // ##########################################################################

  void PAG::reorientAllWith(EdgeMark m) {
    for (const Edge& e: edges()) {
      setMarkAt(e.first(), e.second(), m);
      setMarkAt(e.second(), e.first(), m);
    }
  }

  // ##########################################################################
  // Conversion / display
  // ##########################################################################

  MixedGraph PAG::toMixedGraph() const {
    MixedGraph g;
    for (const NodeId n: nodes()) {
      g.addNodeWithId(n);
    }

    for (const Edge& e: edges()) {
      const NodeId x      = e.first();
      const NodeId y      = e.second();
      const bool   xArrow = isArrowhead(y, x);   // arrowhead at x-endpoint
      const bool   yArrow = isArrowhead(x, y);   // arrowhead at y-endpoint
      if (xArrow && yArrow) {
        g.addArc(x, y);
        g.addArc(y, x);
      } else if (yArrow) {
        g.addArc(x, y);
      } else if (xArrow) {
        g.addArc(y, x);
      } else {
        g.addEdge(x, y);
      }
    }
    return g;
  }

  namespace {
    char markChar(EdgeMark m) {
      switch (m) {
        case EdgeMark::Circle:    return 'o';
        case EdgeMark::Tail:      return '-';
        case EdgeMark::Arrowhead: return '>';
      }
      return '?';
    }
  }   // namespace

  std::string PAG::toString() const {
    std::ostringstream s;
    s << "PAG{nodes: " << NodeGraphPart::toString() << ", edges: [";
    bool first = true;
    for (const Edge& e: edges()) {
      if (!first) { s << ", "; }
      first         = false;
      const NodeId x = e.first();
      const NodeId y = e.second();
      s << x << " " << markChar(markAt(y, x)) << "--" << markChar(markAt(x, y)) << " " << y;
    }
    s << "]}";
    return s.str();
  }

  std::string PAG::toDot() const {
    std::ostringstream out;
    out << "digraph PAG {\n";
    out << "  node [shape=ellipse];\n";

    for (const NodeId n: nodes()) {
      out << "  " << n << dotNodeLabel(n) << ";\n";
    }

    for (const Edge& e: edges()) {
      const NodeId x      = e.first();
      const NodeId y      = e.second();
      const bool   xArrow = isArrowhead(y, x);
      const bool   yArrow = isArrowhead(x, y);
      const bool   xCircle = isCircle(y, x);
      const bool   yCircle = isCircle(x, y);

      const std::string arrowhead = yArrow ? "normal" : (yCircle ? "odot" : "none");
      const std::string arrowtail = xArrow ? "normal" : (xCircle ? "odot" : "none");

      out << "  " << x << " -> " << y
          << " [dir=both"
          << ", arrowhead=" << arrowhead
          << ", arrowtail=" << arrowtail
          << "];\n";
    }
    out << "}\n";
    return out.str();
  }

  std::ostream& operator<<(std::ostream& stream, const PAG& g) {
    stream << g.toString();
    return stream;
  }

} /* namespace gum */
