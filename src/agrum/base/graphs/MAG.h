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
 * @brief Maximal ancestral graphs (MAG)
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_MAG_H
#define GUM_MAG_H

#include <string>

#include <agrum/agrum.h>

#include <agrum/base/graphs/immutableGraph.h>
#include <agrum/base/graphs/PAG.h>

namespace gum {

  /** @class MAG
   * @brief Maximal ancestral graph: the independence model of a DAG over observed variables, some
   * variables being latent and some being selected.
   * @ingroup graph_group
   *
   * A MAG is a marked graph without circle (edges ->, <-> and --) which is ancestral (no node is
   * anterior to a parent or a spouse of itself, no arrowhead at a node with an undirected edge) and
   * maximal (two non adjacent nodes are m-separated by some set). The marginal and conditional
   * independence model of a DAG over its observed variables (latent variables marginalized, the
   * selected ones conditioned on) is the m-separation of a MAG (Richardson and Spirtes 2002).
   *
   * The invariants of a MAG are global: this class is an immutable value, built by an algorithm
   * (fromDAG, fromPAG) or by a checked conversion (fromMarkedGraph), which gives a read access to
   * its marked graph.
   *
   * @par References
   * - T. Richardson, P. Spirtes. Ancestral graph Markov models. The Annals of Statistics
   *   30(4):962-1030, 2002. doi:10.1214/aos/1031689015.
   * - J. Zhang. Causal reasoning with ancestral graphs. Journal of Machine Learning Research
   *   9:1437-1474, 2008.
   * - J. Zhang. On the completeness of orientation rules for causal discovery in the presence of
   *   latent confounders and selection bias. Artificial Intelligence 172(16-17):1873-1896, 2008.
   *   doi:10.1016/j.artint.2008.08.001.
   */
  class GUM_SHARED_PUBLIC MAG: public ImmutableGraph< MarkedGraph > {
    public:
    // ############################################################################
    /// @name Constructions
    // ############################################################################
    /// @{

    /// the MAG of dag over its observed nodes, the others being latent or selected
    /** Construction of Richardson and Spirtes (2002, Sec. 4.2): two observed nodes a and b are
     * adjacent iff they are not d-separated in dag by an({a, b} ∪ selection) \ (latents ∪ {a, b})
     * (Theorem 4.2); the edge is a--b if each one is an ancestor of the other or of a selected
     * node, a->b if only a is, a<->b if none is. The nodes keep their ids and names. O(n^2 (n +
     * m)).
     * @param dag the DAG
     * @param latents its latent nodes (marginalized)
     * @param selection its selected nodes (conditioned on)
     * @throw InvalidArgument if latents and selection intersect, or contain a node not in dag */
    static MAG fromDAG(const DAG&     dag,
                       const NodeSet& latents   = NodeSet(),
                       const NodeSet& selection = NodeSet());

    /// a representative MAG of the Markov equivalence class represented by a PAG
    /** Theorem 2 of Zhang (2008, Artificial Intelligence): the circles of the edges o-> become
     * tails and the circles of the edges --o become arrowheads (both become ->), and the edges o-o
     * are oriented into a DAG without unshielded collider (graph::consistentExtension).
     * @throw InvalidArgument if pag is not a valid PAG (its edges o-o can not be so oriented, or
     * the result is not a MAG) */
    static MAG fromPAG(const PAG& pag);

    /// checked conversion of a marked graph
    /** O(n^2 (n + m)).
     * @throw InvalidArgument if g has a circle, or is not ancestral, or is not maximal */
    static MAG fromMarkedGraph(const MarkedGraph& g);

    MAG(const MAG& g);
    MAG(MAG&& g);
    ~MAG();
    MAG& operator=(const MAG& g);
    MAG& operator=(MAG&& g);

    /// @}

    // ############################################################################
    /// @name Read access
    // ############################################################################
    /// @{

    /// the nodes linked to node by an undirected edge node--y
    NodeSet neighbours(NodeId node) const;

    /// the nodes linked to node by an edge, whatever its marks
    const NodeSet& adjacents(NodeId node) const;

    /// the nodes y such that y->node
    NodeSet parents(NodeId node) const;

    /// the nodes y such that node->y
    NodeSet children(NodeId node) const;

    /// the nodes y such that node<->y
    NodeSet spouses(NodeId node) const;

    /// the mark of the edge src-dst at dst
    /** @throw NotFound if there is no edge src-dst */
    EdgeMark markAt(NodeId src, NodeId dst) const;

    /// @}

    // ############################################################################
    /// @name Separation
    // ############################################################################
    /// @{

    /// m-separation of X and Y by Z (graph::mSeparation)
    bool mSeparation(NodeId X, NodeId Y, const NodeSet& Z) const;

    /// m-separation of two sets of nodes by a third one (graph::mSeparation)
    /** @throw InvalidArgument if X and Y are not disjoint */
    bool mSeparation(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const;

    /// @}

    /// a dot representation (see MarkedGraph::toDot)
    std::string toDot() const;

    /// a dot representation, with the options given by the caller
    std::string toDot(const dot::DotOptions& options) const;

    private:
    /// trusted constructor: g is a MAG
    explicit MAG(MarkedGraph&& g);
  };

}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/base/graphs/MAG_inl.h>
#endif   // GUM_NO_INLINE

#endif   // GUM_MAG_H
