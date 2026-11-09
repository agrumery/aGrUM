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


/**
 * @file
 * @brief Class building the essential Graph from a DAGmodel
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#ifndef GUM_ESSENTIALGRAPH_H
#define GUM_ESSENTIALGRAPH_H

#include <optional>
#include <string>

#include <agrum/agrum.h>

#include <agrum/base/graphicalModels/DAGmodel.h>
#include <agrum/base/graphs/io/graphGumIO.h>

#include <string_view>

namespace gum {

  namespace dot {
    class DotOptions;
  }   // namespace dot

  /**
   * @class EssentialGraph
   * @headerfile essentialGraph.h <agrum/BN/algorithms/essentialGraph.h>
   * @brief The essential graph of a Bayesian network, or a PDAG learned as such.
   * @ingroup bn_group
   *
   * The essential graph (or completed PDAG) of a DAG represents its Markov equivalence class: its
   * arcs are the compelled arcs, which have the same orientation in all the DAGs of the class, and
   * its undirected edges are the reversible ones (Andersson, Madigan and Perlman 1997). It is a
   * chain graph whose chain components are chordal.
   *
   * Built from a model, it is computed by graph::cpdag (algorithm Find-Compelled of Chickering
   * 1995, linear time). A constraint-based learner may also give a PDAG to encapsulate: it is then
   * kept as is, and may not be a CPDAG if the learned PDAG is not extendable (see cpdag()).
   *
   * The names of the nodes are stored in the graph: an EssentialGraph does not depend on the model
   * it was built from.
   *
   * @par References
   * - D. M. Chickering. A Transformational Characterization of Equivalent Bayesian Network
   *   Structures. Proceedings of the 11th Conference on Uncertainty in Artificial Intelligence
   *   (UAI 1995), pp. 87-98. arXiv:1302.4938.
   * - S. A. Andersson, D. Madigan, M. D. Perlman. A characterization of Markov equivalence classes
   *   for acyclic digraphs. The Annals of Statistics 25(2):505-541, 1997.
   *   doi:10.1214/aos/1031833662.
   */
  class GUM_PUBLIC_BN EssentialGraph {
    public:
    EssentialGraph() = default;

    /// the essential graph of the DAG of m, the nodes being named after the variables of m
    explicit EssentialGraph(const DAGmodel& m);

    /// pdag as the essential graph of m, the nodes being named after the variables of m
    EssentialGraph(const DAGmodel& m, const PDAG& pdag);

    /// pdag as an essential graph (e.g. a PDAG learned by a constraint-based algorithm), with its
    /// names
    explicit EssentialGraph(const PDAG& pdag);

    EssentialGraph(const EssentialGraph& g)                = default;
    EssentialGraph(EssentialGraph&& g) noexcept            = default;
    EssentialGraph& operator=(const EssentialGraph& g)     = default;
    EssentialGraph& operator=(EssentialGraph&& g) noexcept = default;

    ~EssentialGraph();

    /// @return a copy of the mixed graph
    PDAG pdag() const;

    /// @return a dot representation of this essentialGraph
    std::string toDot() const;

    /// @return a dot representation of this essentialGraph, with the options given by the caller
    std::string toDot(const dot::DotOptions& options) const;

    /// wrapping @ref MixedGraph::parents(id)
    const NodeSet& parents(NodeId id) const;

    /// wrapping @ref MixedGraph::parents(id)
    const NodeSet& children(NodeId id) const;

    /// wrapping @ref MixedGraph::parents(ids)
    NodeSet parents(const NodeSet& ids) const;

    /// wrapping @ref MixedGraph::parents(ids)
    NodeSet children(const NodeSet& ids) const;

    /// wrapping @ref PDAG::descendants(id)
    NodeSet descendants(NodeId id) const;

    /// wrapping @ref PDAG::ancestors(id)
    NodeSet ancestors(NodeId id) const;

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

    /// Returns the connected components of the essential graph.
    /// Each node maps to the id of its component root.
    NodeProperty< NodeId > connectedComponents() const;

    /// the id of the node with the given name, or std::nullopt
    std::optional< NodeId > idFromName(std::string_view name) const;

    /// the name of a node
    std::string nameFromId(NodeId node) const;

    /// the essential graph as a CPDAG
    /** @throw InvalidArgument if the PDAG is not a CPDAG (e.g. a learned PDAG which is not
     * extendable) */
    CPDAG cpdag() const;

    private:
    PDAG _pdag_;
  };

  /// @name EssentialGraph in the GUM format (see graphGumIO.h)
  /// An essential graph is read as a PDAG, checked as such, not as a CPDAG: it may be a PDAG
  /// learned by a constraint-based algorithm (see IBNLearner::learnEssentialGraph).
  /// @{
  GUM_PUBLIC_BN std::string graphToGUMString(const EssentialGraph& g,
                                             int                   indent = defaultJgumIndent());
  GUM_PUBLIC_BN void        saveGraphGUM(const EssentialGraph& g,
                                         std::string_view      path,
                                         std::optional< bool > binary = std::nullopt,
                                         int                   indent = defaultJgumIndent());
  template <>
  GUM_PUBLIC_BN EssentialGraph graphFromGUMString< EssentialGraph >(std::string_view content);
  template <>
  GUM_PUBLIC_BN EssentialGraph loadGraphGUM< EssentialGraph >(std::string_view      path,
                                                              std::optional< bool > binary);
  /// @}

}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/BN/algorithms/essentialGraph_inl.h>
#endif   // GUM_NO_INLINE

#endif   // GUM_ESSENTIALGRAPH_H
