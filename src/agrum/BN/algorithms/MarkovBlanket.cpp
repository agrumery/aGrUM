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
 * @brief Source implementation of the class building the Markov Blanket from a
 * DAGmodel and a node (id or name)
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#include <agrum/BN/algorithms/MarkovBlanket.h>

#ifdef GUM_NO_INLINE
#  include <agrum/BN/algorithms/MarkovBlanket_inl.h>
#endif   // GUM_NOINLINE

namespace gum {
  MarkovBlanket::MarkovBlanket(const DAGmodel& m, NodeId id, int level) : _model_(m), _node_(id) {
    if (level < 1) GUM_ERROR(InvalidArgument, "Argument level(=" << level << ") must be >0.")

    NodeSet done;
    _buildMarkovBlanket_(_node_);
    done.insert(_node_);

    while (level > 1) {
      level--;
      auto todo        = _mb_.nodes().asNodeSet() - done;
      bool anythingnew = false;
      for (NodeId n: todo) {
        done.insert(n);
        if (_buildMarkovBlanket_(n)) anythingnew = true;
      }
      if (!anythingnew) break;
    }

    // we add now some arcs that are between the nodes in  _mb_ but are not part of
    // the last ones.
    // For instance, an arc between a parent and a parent of children
    for (const auto node: _mb_.nodes()) {
      for (const auto child: _model_.children(node)) {
        if (_mb_.existsNode(child) && !_mb_.existsArc(Arc(node, child))) {
          _mb_.addArc(node, child);
          _specialArcs_.insert(Arc(node, child));
        }
      }
    }
  }

  MarkovBlanket::MarkovBlanket(const DAGmodel& m, const std::string& name, int level) :
      MarkovBlanket(m, m.idFromName(name), level) {}

  MarkovBlanket::~MarkovBlanket() {}

  bool MarkovBlanket::_buildMarkovBlanket_(const NodeId node) {
    bool change = false;
    if (!_model_.nodes().exists(node))
      GUM_ERROR(InvalidArgument, "Node " << node << " does not exist.")

    if (!_mb_.nodes().exists(node)) {
      _mb_.addNodeWithId(node);
      change = true;
    }

    for (const auto& parent: _model_.parents(node)) {
      if (!_mb_.nodes().exists(parent)) {
        _mb_.addNodeWithId(parent);
        change = true;
      }
      _mb_.addArc(parent, node);
    }

    for (const auto& child: _model_.children(node)) {
      if (!_mb_.nodes().exists(child)) {
        _mb_.addNodeWithId(child);
        change = true;
      }
      _mb_.addArc(node, child);
      for (const auto& opar: _model_.parents(child)) {
        if (opar != node) {
          if (!_mb_.nodes().exists(opar)) {
            _mb_.addNodeWithId(opar);
            change = true;
          }
          _mb_.addArc(opar, child);
        }
      }
    }

    return change;
  }

  bool MarkovBlanket::hasSameStructure(const DAGmodel& other) const {
    if (size() != other.size()) return false;

    if (sizeArcs() != other.sizeArcs()) return false;

    for (const auto& nid: nodes()) {
      try {
        other.idFromName(_model_.variable(nid).name());
      } catch (NotFound const&) { return false; }
    }

    for (const auto& arc: arcs()) {
      if (!other.arcs().exists(Arc(other.idFromName(_model_.variable(arc.tail()).name()),
                                   other.idFromName(_model_.variable(arc.head()).name()))))
        return false;
    }

    return true;
  }

  std::string MarkovBlanket::toDot() const {
    std::stringstream output;
    std::stringstream nodeStream;
    std::stringstream arcStream;
    List< NodeId >    treatedNodes;
    output << "digraph \""
           << "no_name\" {" << std::endl;
    nodeStream << "node [shape = ellipse];" << std::endl;
    std::string tab = "  ";

    for (const auto node: _mb_.nodes()) {
      nodeStream << tab << node << "[label=\"" << _model_.variable(node).name() << "\"";
      if (node == _node_) { nodeStream << ", color=red"; }
      nodeStream << "];" << std::endl;

      for (const auto chi: _mb_.children(node)) {
        arcStream << tab << node << " -> " << chi;
        if (_specialArcs_.exists(Arc(node, chi))) { arcStream << " [color=grey]"; }
        arcStream << ";" << std::endl;
      }
    }

    output << nodeStream.str() << std::endl << arcStream.str() << std::endl << "}" << std::endl;

    return output.str();
  }
}   // namespace gum
