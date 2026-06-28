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
 * learnDAG() and learnPDAG() work via PAG→MixedGraph approximation.
 * Users wanting the full PAG call learnPAG() directly.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_FCI_H
#define GUM_LEARNING_FCI_H

#include <agrum/base/graphs/PAG.h>
#include <agrum/BN/learning/CIBasedLearning.h>

namespace gum {

  namespace learning {

    /**
     * @class FCI
     * @brief Fast Causal Inference — PAG learning via constraint-based methods.
     *
     * Phases:
     *  1. Skeleton discovery (inherited from CIBasedLearning)
     *  2. Initial PAG + unshielded colliders oriented as arrowheads
     *  3. possibleDSep pruning (Phase 3 — to be added in later step)
     *  4. Reset + re-orient colliders after possibleDSep (Phase 4 — later)
     *  5. Orientation rules R1–R10 (Phase 5 — later)
     *  6. Collect _latentCouples_ from final PAG (Phase 6 — later)
     *
     * learnMixedStructure() bridges to learnDAG/learnPDAG via toMixedGraph().
     *
     * @ingroup learning_group
     */
    class FCI: public CIBasedLearning {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      FCI();
      FCI(const FCI&);
      FCI(FCI&&) noexcept;
      ~FCI() override;

      FCI& operator=(const FCI&);
      FCI& operator=(FCI&&) noexcept;

      /// @}

      // ##########################################################################
      /// @name FCI-specific parameterisation
      // ##########################################################################
      /// @{

      /// maximum path length for R4 discriminating-path search; Size(-1) = unlimited
      void setMaxPathLength(Size maxLen);
      Size maxPathLength() const;

      /// @}

      // ##########################################################################
      /// @name Learning
      // ##########################################################################
      /// @{

      /// primary FCI output: learn a PAG from the given node-only or partial graph
      PAG learnPAG(MixedGraph graph);

      /// bridge for learnDAG/learnPDAG: runs learnPAG then converts via toMixedGraph()
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

      Size maxPathLength_ = Size(-1);
    };

  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_FCI_H */
