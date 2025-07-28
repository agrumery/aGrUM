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
 * @brief Class building the essential Graph from a DAGmodel
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#ifndef GUM_ESSENTIALGRAPH_H
#define GUM_ESSENTIALGRAPH_H

#include <agrum/agrum.h>

#include <agrum/base/graphicalModels/DAGmodel.h>
#include <agrum/base/graphs/PDAG.h>

namespace gum {

  /**
   * @class EssentialGraph
   * @headerfile essentialGraph.h <agrum/BN/algorithms/essentialGraph.h>
   * @brief Class building the essential graph from a BN.
   * @ingroup bn_group
   *
   * Essential graph is a mixed graph (Chain Graph) that represents the class of
   * markov equivalent Bayesian networks (with the same independence model).
   *
   * The main goal of this class is to nest the algorithm to build the essential
   * graph from a BN and to encapsulate the representation (as a MixedGraph) of the
   * essential graph.
   *
   * gum::operator<<(std::ostream&, const BayesNet<GUM_SCALAR>&).
   */
  class EssentialGraph {
    public:
    EssentialGraph() = default;
    // this constructor will build the essential graph
    explicit EssentialGraph(const DAGmodel& m);
    // this constructor will use mh as essential graph for m
    EssentialGraph(const DAGmodel& m, const PDAG& mg);
    EssentialGraph(const EssentialGraph& g);
    EssentialGraph& operator=(const EssentialGraph& g);

    ~EssentialGraph();

    /// @return a copy of the mixed graph
    PDAG pdag() const;

    /// @return a dot representation of this essentialGraph
    std::string toDot() const;

    /// wrapping @ref MixedGraph::parents(id)
    const NodeSet& parents(NodeId id) const;

    /// wrapping @ref MixedGraph::parents(id)
    const NodeSet& children(NodeId id) const;

    /// wrapping @ref MixedGraph::parents(ids)
    NodeSet parents(const NodeSet& ids) const;

    /// wrapping @ref MixedGraph::parents(ids)
    NodeSet children(const NodeSet& ids) const;

    /// wrapping @ref MixedGraph::parents(id)
    const NodeSet& neighbours(NodeId id) const;

    /// wrapping @ref MixedGraph::sizeArcs()
    Size sizeArcs() const;

    /// wrapping @ref MixedGraph::arcs()
    const ArcSet& arcs() const;
    /// wrapping @ref MixedGraph::sizeEdges()
    Size sizeEdges() const;

    /// wrapping @ref MixedGraph::edges()
    const EdgeSet& edges() const;

    /// wrapping @ref MixedGraph::sizeNodes()
    Size sizeNodes() const;
    /// wrapping @ref MixedGraph::size()
    Size size() const;

    UndiGraph skeleton() const;

    /// wrapping @ref MixedGraph::nodes()
    const NodeGraphPart& nodes() const;

    /// wrappping @ref DAGModel::idFromName()
    NodeId idFromName(const std::string& name) const;

    /// wrappping @ref DAGModel::variable(node).name()
    const std::string& nameFromId(NodeId node) const;

    private:
    void _buildEssentialGraph_();
    bool _strongly_protected_(NodeId a, NodeId b) const;

    static bool _strongly_protected_(MixedGraph mg, NodeId a, NodeId b);

    const DAGmodel* _dagmodel_;
    PDAG            _pdag_;
  };
}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/BN/algorithms/essentialGraph_inl.h>
#endif   // GUM_NOINLINE

#endif   // GUM_ESSENTIALGRAPH_H
