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
#include <string_view>

#include <agrum/agrum.h>

#include <agrum/base/graphicalModels/DAGmodel.h>
#include <agrum/base/graphs/io/graphGumIO.h>

namespace gum {

  /**
   * @class EssentialGraph
   * @headerfile essentialGraph.h <agrum/BN/algorithms/essentialGraph.h>
   * @brief The essential graph of a Bayesian network: the CPDAG of its Markov equivalence class,
   * with the names of its variables.
   * @ingroup bn_group
   *
   * The essential graph (or completed PDAG) of a DAG represents its Markov equivalence class: its
   * arcs are the compelled arcs, which have the same orientation in all the DAGs of the class, and
   * its undirected edges are the reversible ones (Andersson, Madigan and Perlman 1997). An
   * EssentialGraph is a CPDAG (immutable): built from a model, it is computed by graph::cpdag
   * (algorithm Find-Compelled of Chickering 1995, linear time); built from a PDAG, the PDAG is
   * checked to be a CPDAG.
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
  class GUM_PUBLIC_BN EssentialGraph: public CPDAG {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// the empty essential graph
    EssentialGraph();

    /// the essential graph of the DAG of m, the nodes being named after the variables of m
    explicit EssentialGraph(const DAGmodel& m);

    /// pdag as the essential graph of m, the nodes being named after the variables of m
    /** @throw InvalidArgument if pdag is not a CPDAG (see CPDAG::fromPDAG) */
    EssentialGraph(const DAGmodel& m, const PDAG& pdag);

    /// pdag as an essential graph, with its names
    /** @throw InvalidArgument if pdag is not a CPDAG (see CPDAG::fromPDAG) */
    explicit EssentialGraph(const PDAG& pdag);

    /// cpdag as an essential graph, with its names
    explicit EssentialGraph(const CPDAG& cpdag);

    EssentialGraph(const EssentialGraph& g)            = default;
    EssentialGraph(EssentialGraph&& g)                 = default;
    EssentialGraph& operator=(const EssentialGraph& g) = default;
    EssentialGraph& operator=(EssentialGraph&& g)      = default;

    ~EssentialGraph();

    /// @}

    // ############################################################################
    /// @name Read access (beyond the one of CPDAG)
    // ############################################################################
    /// @{

    using CPDAG::children;
    using CPDAG::parents;

    /// the parents of a set of nodes (see MixedGraph::parents)
    NodeSet parents(const NodeSet& ids) const;

    /// the children of a set of nodes (see MixedGraph::children)
    NodeSet children(const NodeSet& ids) const;

    /// the descendants of a node (see PDAG::descendants)
    NodeSet descendants(NodeId id) const;

    /// the ancestors of a node (see PDAG::ancestors)
    NodeSet ancestors(NodeId id) const;

    /// the number of nodes
    Size sizeNodes() const;

    /// a copy of the graph of the essential graph, as a PDAG
    PDAG pdag() const;

    /// the essential graph as a CPDAG (without the class EssentialGraph)
    CPDAG cpdag() const;

    /// @}

    /// a dot representation: the one of a CPDAG, the names of the nodes as labels
    std::string toDot() const;

    /// a dot representation, with the options given by the caller (which win over the colors of
    /// the compelled arcs and the labels)
    std::string toDot(const dot::DotOptions& options) const;
  };

  /// @name EssentialGraph in the GUM format (see graphGumIO.h)
  /// An essential graph is read as a PDAG, then checked to be a CPDAG (IOError otherwise).
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
