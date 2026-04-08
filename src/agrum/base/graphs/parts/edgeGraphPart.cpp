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
 * @brief Source implementation of classes for undirected edge sets
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#include <agrum/base/graphs/parts/edgeGraphPart.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/parts/edgeGraphPart_inl.h>
#endif   // GUM_NO_INLINE
#include "agrum/base/graphs/graphElements.h"

namespace gum {

  ///////////////////// EdgeGraphPart
  EdgeGraphPart::EdgeGraphPart(Size edges_size, bool edges_resize_policy) :
      _edges_(edges_size, edges_resize_policy) {
    GUM_CONSTRUCTOR(EdgeGraphPart);
  }

  EdgeGraphPart::EdgeGraphPart(EdgeGraphPart&& s) :
      _edges_(std::move(s._edges_)), _neighbours_(std::move(s._neighbours_)) {
    GUM_CONS_MOV(EdgeGraphPart);
  }

  EdgeGraphPart::EdgeGraphPart(const EdgeGraphPart& s) : _edges_(s._edges_) {
    GUM_CONS_CPY(EdgeGraphPart)

    // copy the set of neighbours
    _neighbours_.resize(s._neighbours_.capacity());

    for (const auto& [key, nodeset]: s._neighbours_) {
      NodeSet* newneigh = new NodeSet(*nodeset);
      _neighbours_.insert(key, newneigh);
    }

    // send signals to indicate that there are new edges
    if (onEdgeAdded.hasListener())
      for (const auto& edge: _edges_)
        GUM_EMIT2(onEdgeAdded, edge.first(), edge.second());
  }

  EdgeGraphPart::~EdgeGraphPart() {
    GUM_DESTRUCTOR(EdgeGraphPart)
    // be sure to deallocate all the neighbours sets
    _clearEdges_();
  }

  void EdgeGraphPart::clearEdges() { _clearEdges_(); }

  void EdgeGraphPart::_clearEdges_() {
    for (const auto& elt: _neighbours_)
      delete elt.second;

    _neighbours_.clear();

    if (onEdgeDeleted.hasListener()) {
      EdgeSet tmp = _edges_;
      _edges_.clear();

      for (const auto& edge: tmp)
        GUM_EMIT2(onEdgeDeleted, edge.first(), edge.second());
    } else {
      _edges_.clear();
    }
  }

  EdgeGraphPart& EdgeGraphPart::operator=(const EdgeGraphPart& s) {
    // avoid self assignment
    if (this != &s) {
      clearEdges();

      _edges_ = s._edges_;

      // copy the set of neighbours
      _neighbours_.resize(s._neighbours_.capacity());

      for (const auto& [key, nodeset]: s._neighbours_) {
        NodeSet* newneigh = new NodeSet(*nodeset);
        _neighbours_.insert(key, newneigh);
      }

      if (onEdgeAdded.hasListener())
        for (const auto& edge: _edges_)
          GUM_EMIT2(onEdgeAdded, edge.first(), edge.second());

      GUM_OP_CPY(EdgeGraphPart);
    }

    return *this;
  }

  EdgeGraphPart& EdgeGraphPart::operator=(EdgeGraphPart&& s) {
    if (this != &s) {
      clearEdges();
      _edges_      = std::move(s._edges_);
      _neighbours_ = std::move(s._neighbours_);
      if (onEdgeAdded.hasListener()) {
        for (const auto& edge: _edges_) {
          GUM_EMIT2(onEdgeAdded, edge.first(), edge.second());
        }
      }
      GUM_OP_MOV(EdgeGraphPart);
    }
    return *this;
  }

  std::string EdgeGraphPart::toString() const {
    std::stringstream s;
    bool              first = true;
    s << "{";

    for (const auto& edge: _edges_) {
      if (first) first = false;
      else s << ",";

      s << edge;
    }

    s << "}";

    return s.str();
  }

  std::ostream& operator<<(std::ostream& stream, const EdgeGraphPart& set) {
    stream << set.toString();
    return stream;
  }

} /* namespace gum */
