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
 * @brief FCI (Fast Causal Inference) causal discovery algorithm.
 *
 * FCI extends PC to handle latent confounders and selection bias.
 * Output: a PAG (Partial Ancestral Graph) over observed variables.
 *
 * aGrUM learning contract: every algorithm must be able to produce a BN.
 * learnDAG() and learnPDAG() go through the representative MAG of the learned PAG (Zhang 2008,
 * Artificial Intelligence, Theorem 2). Users wanting the full PAG call learnPAG() directly.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_FCI_H
#define GUM_LEARNING_FCI_H

#include <agrum/base/graphs/PAG.h>
#include <agrum/BN/learning/CIBasedLearning.h>

namespace gum::learning {

  /**
   * @class FCI
   * @brief Fast Causal Inference — PAG learning via constraint-based methods.
   *
   * Phases:
   *  1. Skeleton discovery (inherited from CIBasedLearning)
   *  2. Initial PAG + unshielded colliders oriented as arrowheads
   *  3. possibleDSep pruning
   *  4. Reset + re-orient colliders after possibleDSep
   *  5. Orientation rules R1–R10
   *  6. Collect _latentCouples_ from final PAG
   *
   * learnPDAG() and learnDAG() do not apply the Meek rules (which are not defined on a PAG): the
   * learned PAG is turned into its representative MAG (Zhang 2008, Artificial Intelligence,
   * Theorem 2: the edges o-> and --o become ->, the edges o-o are oriented into a DAG without
   * unshielded collider), whose edges <-> are the latent couples (latentVariables()).
   *
   * @par References
   * - P. Spirtes, C. Glymour, R. Scheines. Causation, Prediction, and Search (2nd ed.). MIT Press,
   *   2000.
   * - J. Zhang. On the completeness of orientation rules for causal discovery in the presence of
   *   latent confounders and selection bias. Artificial Intelligence 172(16-17):1873-1896, 2008.
   *   doi:10.1016/j.artint.2008.08.001.
   *
   * @ingroup learning_group
   */
  class GUM_PUBLIC_BN FCI: public CIBasedLearning {
    public:
    // ##########################################################################
    /// @name Constructors / Destructors
    // ##########################################################################
    /// @{

    FCI();
    FCI(const FCI&) = default;
    FCI(FCI&&)      = default;
    ~FCI() override;

    FCI& operator=(const FCI&) = default;
    FCI& operator=(FCI&&)      = default;

    /// @}

    // ##########################################################################
    /// @name FCI-specific parameterisation
    // ##########################################################################
    /// @{

    /// maximum path length for R4 discriminating-path search; Size(-1) = unlimited
    void setMaxPathLength(Size maxLen);
    Size maxPathLength() const;

    /// when the learned PAG is detected as not valid (see learnPDAG): an error, or the former
    /// approximation
    /** Initialized from the key `learning.fciErrorOnInvalidPAG` at the construction. */
    void setErrorOnInvalidPAG(bool error);
    bool errorOnInvalidPAG() const;

    /// @}

    // ##########################################################################
    /// @name Learning
    // ##########################################################################
    /// @{

    /// primary FCI output: learn a PAG from the given node-only or partial graph
    PAG learnPAG(MixedGraph graph);

    /// the PDAG of the representative MAG of the learned PAG
    /** The arcs -> of the MAG are kept, its edges -- and <-> become undirected edges. The edges
     * o-o of the PAG are oriented by graph::orientAsDAG. An invalid PAG is detected only by a
     * directed cycle in this representative graph: then the former approximation (PAG seen as a
     * mixed graph, then the Meek rules) is used, or InvalidArgument is raised if
     * errorOnInvalidPAG(). Other invalid PAGs are not detected and give their representative graph
     * as is: e.g. a graph of edges o-o which is not chordal (orientAsDAG then creates new
     * v-structures) or an almost directed cycle through an edge <->.
     * @throw InvalidArgument if the representative graph has a directed cycle and
     * errorOnInvalidPAG() */
    PDAG learnPDAG(MixedGraph graph) override;

    /// a DAG of the representative MAG of the learned PAG, all the domains being considered equal
    DAG learnDAG(MixedGraph graph) override;

    /// a DAG of the representative MAG of the learned PAG
    /** The arcs of the PDAG of learnPDAG() are kept, its edges are oriented by graph::orientAsDAG
     * (small CPTs given the domain sizes). The edges <-> of the MAG remain in latentVariables(). */
    DAG learnDAG(MixedGraph graph, const NodeProperty< Size >& domainSizes) override;

    /// the mixed graph of learnPDAG()
    MixedGraph learnMixedStructure(MixedGraph graph) override;

    /// learnSkeleton() inherited from CIBasedLearning

    /// return all nodes on possible-d-sep paths from x toward y (excl. x and y).
    /// Public accessor for testing and introspection.
    std::vector< NodeId > possibleDSep(const PAG& pag, NodeId x, NodeId y) const;

    /// @}

    protected:
    /// conflict hook override: FCI leaves circle marks — orientation conflicts
    /// are not recorded as latent couples here (only final ↔ edges in Phase 6 are)
    void resolveOrientConflict_(NodeId src, NodeId dst) override;

    private:
    /// Phase 2 / Phase 4: orient unshielded colliders directly on the PAG.
    /// topology is used for unshielded-triple detection (adjacency queries only).
    void orientCollidersOnPAG_(PAG& pag, const MixedGraph& topology);

    /// Phase 3: return all nodes on possible-d-sep paths from x toward y (excl. x, y).
    std::vector< NodeId > computePossibleDSep_(const PAG& pag, NodeId x, NodeId y) const;

    /// Phase 3: prune PAG edges using possibleDSep conditioning sets.
    void possibleDSepPhase_(PAG& pag);

    // Phase 5: orientation rules (all return true if any mark changed)
    bool ruleR1_(PAG& pag) const;    ///< away from collider
    bool ruleR2_(PAG& pag) const;    ///< away from cycle
    bool ruleR3_(PAG& pag) const;    ///< double triangle
    bool ruleR5_(PAG& pag) const;    ///< uncovered circle path → undirected
    bool ruleR6_(PAG& pag) const;    ///< tail propagation from undirected edge
    bool ruleR7_(PAG& pag) const;    ///< tail propagation from A-oB
    bool ruleR4_(PAG& pag) const;    ///< discriminating path (uses CI test)
    bool ruleR8_(PAG& pag) const;    ///< away from ancestor (graph only)
    bool ruleR9_(PAG& pag) const;    ///< away from ancestor via uncovered pd-path
    bool ruleR10_(PAG& pag) const;   ///< away from ancestor via two semi-directed paths

    /// Phase 5: fixed-point loop over all orientation rules R1–R10.
    void applyOrientationRules_(PAG& pag) const;

    /// R4 helper: orient B,C in a detected discriminating path D..A,B,C.
    bool doDdpOrientation_(PAG&                               pag,
                           NodeId                             d,
                           NodeId                             a,
                           NodeId                             b,
                           NodeId                             c,
                           const HashTable< NodeId, NodeId >& previous) const;

    /// the PDAG of the representative MAG of pag (see learnPDAG)
    PDAG representativePDAG_(const PAG& pag, const NodeProperty< Size >& domainSizes);

    Size maxPathLength_     = Size(-1);
    bool errorOnInvalidPAG_ = gum::config().typed< bool >("learning", "fciErrorOnInvalidPAG");
  };

}   // namespace gum::learning

#endif /* GUM_LEARNING_FCI_H */
