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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





/** @file
 * @brief Inline implementation of the class building the essential Graph from a
 * DAGmodel
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

// to ease parsing by IDE
#include <agrum/BN/algorithms/essentialGraph.h>

namespace gum {

  INLINE PDAG EssentialGraph::pdag() const { return _pdag_; }

  INLINE const NodeSet& EssentialGraph::parents(NodeId id) const { return _pdag_.parents(id); }

  INLINE const NodeSet& EssentialGraph::children(NodeId id) const { return _pdag_.children(id); }

  INLINE NodeSet EssentialGraph::parents(const NodeSet& ids) const { return _pdag_.parents(ids); }

  INLINE NodeSet EssentialGraph::children(const NodeSet& ids) const { return _pdag_.children(ids); }

  INLINE const NodeSet& EssentialGraph::neighbours(NodeId id) const {
    return _pdag_.neighbours(id);
  }

  INLINE Size EssentialGraph::sizeArcs() const { return _pdag_.sizeArcs(); }

  INLINE const ArcSet& EssentialGraph::arcs() const { return _pdag_.arcs(); }

  INLINE Size EssentialGraph::sizeEdges() const { return _pdag_.sizeEdges(); }

  INLINE const EdgeSet& EssentialGraph::edges() const { return _pdag_.edges(); }

  INLINE Size EssentialGraph::sizeNodes() const { return _pdag_.sizeNodes(); }

  INLINE Size EssentialGraph::size() const { return _pdag_.size(); }

  INLINE const NodeGraphPart& EssentialGraph::nodes() const { return _pdag_.nodes(); }

  INLINE NodeId EssentialGraph::idFromName(const std::string& name) const {
    return _dagmodel_->idFromName(name);
  };

  INLINE const std::string& EssentialGraph::nameFromId(NodeId node) const {
    return _dagmodel_->variable(node).name();
  };
}   // namespace gum
