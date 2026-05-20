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
 * @brief Source implementation of the class building the Markov Blanket from a
 * DAGmodel and a node (id or name)
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#include <agrum/BN/algorithms/MarkovBlanket.h>

#ifdef GUM_NO_INLINE
#  include <agrum/BN/algorithms/MarkovBlanket_inl.h>
#endif   // GU%_NO_INLINE

namespace gum {
  MarkovBlanket::MarkovBlanket(const DAGmodel& m, NodeId id, int level) : _model_(m), _node_(id) {
    if (level < 1) GUM_ERROR(InvalidArgument, "Argument level(=" << level << ") must be >0.")

    _mb_ = graph::markovBlanket(m.internalDag(), id, level);

    // Add arcs that exist in the model between MB nodes but are not part of
    // the canonical MB definition (e.g. arc between a parent and a co-parent).
    for (const auto node: _mb_.nodes()) {
      for (const auto child: _model_.children(node)) {
        if (_mb_.existsNode(child) && !_mb_.existsArc(Arc(node, child))) {
          _mb_.addArc(node, child);
          _specialArcs_.insert(Arc(node, child));
        }
      }
    }

    for (auto node: _mb_)
      _mb_.setName(node, _model_.variable(node).name());
  }

  MarkovBlanket::MarkovBlanket(const DAGmodel& m, std::string_view name, int level) :
      MarkovBlanket(m, m.idFromName(name), level) {}

  MarkovBlanket::~MarkovBlanket() {}

  bool MarkovBlanket::hasSameStructure(const DAGmodel& other) const {
    if (size() != other.size()) return false;

    if (sizeArcs() != other.sizeArcs()) return false;

    for (const auto& nid: nodes()) {
      if (!other.exists(_model_.variable(nid).name())) { return false; }
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
