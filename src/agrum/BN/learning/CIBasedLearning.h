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
 * @brief Abstract base class for CI-test-based structure learning algorithms.
 *
 * CIBasedLearning sits between ConstraintBasedLearning and concrete algorithms
 * (PC, FCI). It holds all state shared by CI-test algorithms: the independence
 * test, alpha threshold, stable-PC mode, conditioning-set size, separation sets,
 * and unshielded-collider orientation logic.
 *
 * Concrete subclasses implement learnMixedStructure().
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_CI_BASED_LEARNING_H
#define GUM_CI_BASED_LEARNING_H

#include <vector>

#include <agrum/base/stattests/independenceTest.h>
#include <agrum/BN/learning/ConstraintBasedLearning.h>

namespace gum {

  namespace learning {

    /**
     * @class CIBasedLearning
     * @brief Abstract base for CI-test-based causal structure learning (PC, FCI).
     *
     * Provides:
     *  - Independence test injection and parameterisation (alpha, stable, maxCondSetSize).
     *  - learnSkeleton() — PC-stable skeleton discovery with separation-set tracking.
     *  - orientUnshieldedColliders_() — collider orientation on MixedGraph.
     *  - resolveOrientConflict_() virtual hook — called when orientation conflicts;
     *    default records the conflicting pair in _latentCouples_ (PC behaviour).
     *    FCI override is a no-op (circle marks are kept).
     *
     * @ingroup learning_group
     */
    class CIBasedLearning: public ConstraintBasedLearning {
      public:
      /// Controls how collider candidates are ordered before orientation.
      enum class UCPriority : uint8_t {
        Standard,   ///< process triples in natural traversal order
        Sorted,     ///< descending p-value order (strongest evidence first)
      };

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      CIBasedLearning();
      CIBasedLearning(const CIBasedLearning&);
      CIBasedLearning(CIBasedLearning&&) noexcept;
      ~CIBasedLearning() override;

      CIBasedLearning& operator=(const CIBasedLearning&);
      CIBasedLearning& operator=(CIBasedLearning&&) noexcept;

      /// @}

      // ##########################################################################
      /// @name CI test injection
      // ##########################################################################
      /// @{

      /// inject independence test (non-owning: caller manages lifetime)
      void setIndependenceTest(IndependenceTest& test);

      /// @}

      // ##########################################################################
      /// @name Parameterisation
      // ##########################################################################
      /// @{

      void   setAlpha(double alpha);
      double alpha() const;

      /// maximum conditioning set size; Size(-1) = unlimited (default)
      void setMaxCondSetSize(Size max_k);

      /// stable mode: defer edge removals until end of each depth (default: true)
      void setStable(bool stable);

      /// exhaustive sepset mode: accumulate union of all separating sets found at
      /// each depth, rather than stopping at the first (default: false)
      void setExhaustiveSepSet(bool exhaustive);
      bool exhaustiveSepSet() const;

      /// collider candidate ordering (default: Standard)
      void       setUCPriority(UCPriority p);
      UCPriority ucPriority() const;

      /// @}

      // ##########################################################################
      /// @name Learning
      // ##########################################################################
      /// @{

      /// Phase 1: skeleton discovery via conditional independence tests
      MixedGraph learnSkeleton(MixedGraph graph) override;

      // learnMixedStructure() remains pure virtual (inherited from ConstraintBasedLearning)

      /// @}

      protected:
      // separation set entry: conditioning set + p-value that triggered removal
      struct SepSetEntry_ {
        std::vector< NodeId > cond;
        double                pval{0.0};
      };

      // ##########################################################################
      /// @name Collider orientation (shared between PC and FCI Phase 2/4)
      // ##########################################################################
      /// @{

      /// orient unshielded colliders in graph and return the updated graph
      MixedGraph orientUnshieldedColliders_(MixedGraph graph);

      /// orient one arm (src → dst) of a collider; calls resolveOrientConflict_ on conflict
      void orientColliderArm_(MixedGraph& graph, NodeId src, NodeId dst);

      /// conflict hook: called when orientation cannot proceed (arc blocked or reversed).
      /// Default: push Arc(src,dst) to _latentCouples_ (PC behaviour).
      /// FCI override: no-op (leave circle mark).
      virtual void resolveOrientConflict_(NodeId src, NodeId dst);

      /// @}

      // ##########################################################################
      /// @name Shared state
      // ##########################################################################

      IndependenceTest* test_{nullptr};
      double            alpha_            = 0.05;
      Size              maxCondSetSize_   = Size(-1);
      bool              stable_           = true;
      bool              exhaustiveSepSet_ = false;
      UCPriority        ucPriority_       = UCPriority::Standard;

      HashTable< std::pair< NodeId, NodeId >, SepSetEntry_ > sepSet_;
    };

  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_CI_BASED_LEARNING_H */
