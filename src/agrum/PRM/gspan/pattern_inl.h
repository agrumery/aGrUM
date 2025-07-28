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
 * @brief Inline implementation of the Pattern class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

namespace gum {
  namespace prm {
    namespace gspan {

      INLINE
      Pattern::Pattern() : DiGraph(), _last_(0) {
        GUM_CONSTRUCTOR(Pattern);
        ;
      }

      INLINE
      Pattern::~Pattern() {
        GUM_DESTRUCTOR(Pattern);
        ;
      }

      INLINE
      NodeId Pattern::addNodeWithLabel(LabelData& l) {
        NodeId n = NodeId(size() + 1);
        DiGraph::addNodeWithId(n);
        _node_map_.insert(n, &l);
        _last_ = &l;
        return n;
      }

      INLINE
      LabelData& Pattern::label(NodeId node) {
        try {
          return *(_node_map_[node]);
        } catch (NotFound const&) { GUM_ERROR(NotFound, "node not found in this Pattern") }
      }

      INLINE
      const LabelData& Pattern::label(NodeId node) const {
        try {
          return *(_node_map_[node]);
        } catch (NotFound const&) { GUM_ERROR(NotFound, "node not found in this Pattern") }
      }

      INLINE
      LabelData& Pattern::lastAdded() {
        if (_last_) return *_last_;

        GUM_ERROR(OperationNotAllowed, "there are no LabelData yet")
      }

      INLINE
      const LabelData& Pattern::lastAdded() const {
        if (_last_) return *_last_;

        GUM_ERROR(OperationNotAllowed, "there are no LabelData yet")
      }

      INLINE
      LabelData& Pattern::label(NodeId i, NodeId j) {
        try {
          return *(_arc_map_[Arc(i, j)].first);
        } catch (NotFound const&) { GUM_ERROR(NotFound, "arc not found in this Pattern") }
      }

      INLINE
      const LabelData& Pattern::label(NodeId i, NodeId j) const {
        try {
          return *(_arc_map_[Arc(i, j)].first);
        } catch (NotFound const&) { GUM_ERROR(NotFound, "arc not found in this Pattern") }
      }

      INLINE
      LabelData& Pattern::label(const Arc& arc) {
        try {
          return *(_arc_map_[arc].first);
        } catch (NotFound const&) { GUM_ERROR(NotFound, "arc not found in this Pattern") }
      }

      INLINE
      const LabelData& Pattern::label(const Arc& arc) const {
        try {
          return *(_arc_map_[arc].first);
        } catch (NotFound const&) { GUM_ERROR(NotFound, "arc not found in this Pattern") }
      }

      INLINE
      void Pattern::addArc(NodeId i, NodeId j, LabelData& l) {
        if (!(DiGraph::exists(i) && DiGraph::exists(j))) {
          GUM_ERROR(NotFound, "node not found in this pattern")
        }

        EdgeCode* edge = new EdgeCode(i, j, _node_map_[i]->id, l.id, _node_map_[j]->id);

        if ((code().codes.size() == 0) || (DFSCode::validNeighbors(code().codes.back(), edge))) {
          DiGraph::addArc(i, j);
          _arc_map_.insert(Arc(i, j), std::make_pair(&l, edge));
          code().codes.push_back(edge);
        } else {
          delete edge;
          GUM_ERROR(OperationNotAllowed, "illegal arc considering neighborhood restriction")
        }
      }

      INLINE
      bool Pattern::exists(NodeId id) const { return DiGraph::exists(id); }

      INLINE
      bool Pattern::exists(NodeId tail, NodeId head) const {
        return DiGraph::existsArc(tail, head);
      }

      INLINE
      Size Pattern::size() const { return DiGraph::size(); }

      INLINE
      Size Pattern::sizeArcs() const { return DiGraph::sizeArcs(); }

      INLINE
      void Pattern::rightmostPath(std::list< NodeId >& r_path) const {
        r_path.push_back(NodeId(size()));

        while (r_path.front() != 1) {
          for (const auto par: parents(r_path.front())) {
            if (par < r_path.front()) {
              r_path.push_front(par);
              break;
            }
          }
        }
      }

      INLINE const NodeGraphPart& Pattern::nodes() const { return DiGraph::nodes(); }

      INLINE const ArcSet& Pattern::arcs() const { return DiGraph::arcs(); }

      INLINE
      DFSCode& Pattern::code() { return _code_; }

      INLINE
      const DFSCode& Pattern::code() const { return _code_; }

      INLINE
      EdgeCode& Pattern::edgeCode(NodeId tail, NodeId head) {
        try {
          return *(_arc_map_[Arc(tail, head)].second);
        } catch (NotFound const&) { GUM_ERROR(NotFound, "arc not found in Pattern") }
      }

      INLINE
      EdgeCode& Pattern::edgeCode(const Arc& arc) {
        try {
          return *(_arc_map_[arc].second);
        } catch (NotFound const&) { GUM_ERROR(NotFound, "arc not found in Pattern") }
      }

      INLINE
      const EdgeCode& Pattern::edgeCode(NodeId tail, NodeId head) const {
        try {
          return *(_arc_map_[Arc(tail, head)].second);
        } catch (NotFound const&) { GUM_ERROR(NotFound, "arc not found in Pattern") }
      }

      INLINE
      const EdgeCode& Pattern::edgeCode(const Arc& arc) const {
        try {
          return *(_arc_map_[arc].second);
        } catch (NotFound const&) { GUM_ERROR(NotFound, "arc not found in Pattern") }
      }

      INLINE
      void Pattern::pop_back() {
        EdgeCode* edge = _code_.codes.back();
        _code_.codes.pop_back();

        if (edge->isForward()) {
          _node_map_.erase(edge->j);
          _arc_map_.erase(Arc(edge->i, edge->j));
          DiGraph::eraseArc(Arc(edge->i, edge->j));
          DiGraph::eraseNode(edge->j);
        } else {
          _arc_map_.erase(Arc(edge->i, edge->j));
          DiGraph::eraseArc(Arc(edge->i, edge->j));
        }

        delete edge;
      }

      INLINE
      void Pattern::remove(NodeId node) {
        if (DiGraph::parents(node).empty() && DiGraph::children(node).empty()) {
          DiGraph::eraseNode(node);
          _node_map_.erase(node);
        } else {
          GUM_ERROR(OperationNotAllowed, "the given node has neighbors")
        }
      }
    } /* namespace gspan */
  } /* namespace prm */
} /* namespace gum */
