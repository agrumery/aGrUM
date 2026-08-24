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
 * @brief Common configuration interface for k-TBN learners.
 *
 * gum::learning::IKTBNLearner is the pure-virtual interface shared by every
 * k-TBN learner. It declares only the @e configuration surface — the score,
 * algorithm, MIIC-correction, prior and structural-constraint setters — and
 * says nothing about @e how, or for which @f$k@f$, a model is learned.
 *
 * Two classes implement it:
 *   - gum::learning::KTBNLearner: a fixed-@f$k@f$ learner. Its setters apply
 *     each setting to the right internal BNLearner immediately.
 *   - gum::learning::KTBNAdaptiveLearner: a learner that also selects
 *     @f$k@f$. Its setters merely record the requested configuration, to be
 *     replayed on a freshly-built KTBNLearner for every candidate @f$k@f$
 *     (constraints whose slices do not fit a given @f$k@f$ are skipped for
 *     that @f$k@f$ only).
 *
 * The single shared learning entry point @c learnKTBN() is part of the
 * interface. Deliberately excluded, since they assume a single fixed @f$k@f$ or
 * have no meaningful answer on the adaptive learner: @c learnParameters(), the
 * diagnostics (@c k(), @c toString(), @c state(), ...) and the database
 * accessors.
 *
 * Constraints are addressed @b by @b name only (engine names such as "X[1]"
 * or "C", or an explicit (base, slice) pair): a bare NodeId is meaningless at
 * this level because the underlying NodeId spaces differ from one @f$k@f$ to
 * another.
 *
 * The fluent setters return @c IKTBNLearner& ; each concrete class overrides
 * them with a covariant reference to its own type, so call chaining keeps the
 * concrete type.
 *
 * @author Seth AGUILA & Anis KHACEF
 */

#ifndef GUM_LEARNING_IKTBN_LEARNER_H
#define GUM_LEARNING_IKTBN_LEARNER_H

#include <cctype>
#include <filesystem>
#include <fstream>
#include <numeric>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/database/CSVParser.h>

#include <agrum/KTBN/KTBN.h>

namespace gum {

  namespace learning {

    /**
     * @class IKTBNLearner
     * @headerfile IKTBNLearner.h <agrum/KTBN/learning/IKTBNLearner.h>
     * @brief Pure-virtual configuration interface shared by all k-TBN learners.
     * @ingroup learning_group
     *
     * @see gum::learning::KTBNLearner (fixed k),
     *      gum::learning::KTBNAdaptiveLearner (k is learned too).
     */
    template < GUM_Numeric GUM_SCALAR >
    class IKTBNLearner {
      public:
      /// virtual destructor (polymorphic base class)
      virtual ~IKTBNLearner() = default;

      // #######################################################################
      /// @name Main learning method
      // #######################################################################
      /// @{

      /// @brief Learn a k-TBN (structure + CPTs) with the recorded configuration.
      /// The fixed-k learner learns for its single k; the adaptive learner also
      /// selects the best k in [2, kMax].
      virtual KTBN< GUM_SCALAR > learnKTBN() = 0;

      /// @}
      // #######################################################################
      /// @name Score selection
      // #######################################################################
      /// @{

      virtual IKTBNLearner< GUM_SCALAR >& useScoreAIC()            = 0;
      virtual IKTBNLearner< GUM_SCALAR >& useScoreBD()             = 0;
      virtual IKTBNLearner< GUM_SCALAR >& useScoreBDeu()           = 0;
      virtual IKTBNLearner< GUM_SCALAR >& useScoreBIC()            = 0;
      virtual IKTBNLearner< GUM_SCALAR >& useScoreLog2Likelihood() = 0;
      virtual IKTBNLearner< GUM_SCALAR >& useScoreMDL()            = 0;
      virtual void                        useScorefNML()           = 0;

      /// @}
      // #######################################################################
      /// @name Algorithm selection
      // #######################################################################
      /// @{

      virtual IKTBNLearner< GUM_SCALAR >& useGreedyHillClimbing()         = 0;
      virtual IKTBNLearner< GUM_SCALAR >& useExtendedGreedyHillClimbing() = 0;
      virtual IKTBNLearner< GUM_SCALAR >& useLocalSearchWithTabuList(Size tabu_size   = 100,
                                                                     Size nb_decrease = 2)
          = 0;
      virtual IKTBNLearner< GUM_SCALAR >& useMIIC() = 0;

      /// @}
      // #######################################################################
      /// @name MIIC correction
      // #######################################################################
      /// @{

      virtual IKTBNLearner< GUM_SCALAR >& useNMLCorrection() = 0;
      virtual IKTBNLearner< GUM_SCALAR >& useMDLCorrection() = 0;
      virtual IKTBNLearner< GUM_SCALAR >& useNoCorrection()  = 0;

      /// @}
      // #######################################################################
      /// @name Prior selection
      // #######################################################################
      /// @{

      virtual IKTBNLearner< GUM_SCALAR >& useSmoothingPrior(double weight = 1.0) = 0;

      /// @}
      // #######################################################################
      /// @name Structural constraints (by name only)
      // #######################################################################
      /// @{

      /// @brief Forbid @p tailNode -> @p headNode (engine names, e.g. "X[1]", "C").
      virtual IKTBNLearner< GUM_SCALAR >& addForbiddenArc(std::string_view tailNode,
                                                          std::string_view headNode)
          = 0;

      /// @brief Forbid one (base, slice) -> (base, slice) arc (KTBN::ATEMPORAL for static).
      virtual IKTBNLearner< GUM_SCALAR >& addForbiddenArc(std::string_view tailBase,
                                                          int              tailSlice,
                                                          std::string_view headBase,
                                                          int              headSlice)
          = 0;

      /// @brief Undo a previous addForbiddenArc (engine names).
      virtual IKTBNLearner< GUM_SCALAR >& eraseForbiddenArc(std::string_view tailNode,
                                                            std::string_view headNode)
          = 0;

      /// @brief Undo a previous addForbiddenArc for a specific (base, slice) pair.
      virtual IKTBNLearner< GUM_SCALAR >& eraseForbiddenArc(std::string_view tailBase,
                                                            int              tailSlice,
                                                            std::string_view headBase,
                                                            int              headSlice)
          = 0;

      /// @brief Force @p tailNode to be a parent of @p headNode (engine names).
      virtual IKTBNLearner< GUM_SCALAR >& addMandatoryArc(std::string_view tailNode,
                                                          std::string_view headNode)
          = 0;

      /// @brief Force one arc, lag stated explicitly via slices (KTBN::ATEMPORAL for static).
      virtual IKTBNLearner< GUM_SCALAR >& addMandatoryArc(std::string_view tailBase,
                                                          int              tailSlice,
                                                          std::string_view headBase,
                                                          int              headSlice)
          = 0;

      /// @brief Undo a previous addMandatoryArc (engine names).
      virtual IKTBNLearner< GUM_SCALAR >& eraseMandatoryArc(std::string_view tailNode,
                                                            std::string_view headNode)
          = 0;

      /// @brief Undo a previous addMandatoryArc.
      virtual IKTBNLearner< GUM_SCALAR >& eraseMandatoryArc(std::string_view tailBase,
                                                            int              tailSlice,
                                                            std::string_view headBase,
                                                            int              headSlice)
          = 0;

      /// @brief Forbid @p tailBase -> @p headBase at every intra-slice position.
      virtual IKTBNLearner< GUM_SCALAR >& addForbiddenIntraSliceArc(std::string_view tailBase,
                                                                    std::string_view headBase)
          = 0;

      /// @brief Undo a previous addForbiddenIntraSliceArc.
      virtual IKTBNLearner< GUM_SCALAR >& eraseForbiddenIntraSliceArc(std::string_view tailBase,
                                                                      std::string_view headBase)
          = 0;

      /// @brief Forbid @p tailBase -> @p headBase at every causally-possible slice pair.
      virtual IKTBNLearner< GUM_SCALAR >& addForbiddenArcAllSlices(std::string_view tailBase,
                                                                   std::string_view headBase)
          = 0;

      /// @brief Undo a previous addForbiddenArcAllSlices.
      virtual IKTBNLearner< GUM_SCALAR >& eraseForbiddenArcAllSlices(std::string_view tailBase,
                                                                     std::string_view headBase)
          = 0;

      /// @brief Declare a single (base, slice) node as a root (no parents).
      virtual IKTBNLearner< GUM_SCALAR >& addNoParentNode(std::string_view base, int slice) = 0;

      /// @brief Declare a single node (bracket notation, e.g. "X[2]" or "C") as a root.
      virtual IKTBNLearner< GUM_SCALAR >& addNoParentNode(std::string_view name) = 0;

      /// @brief Undo a previous addNoParentNode for a single (base, slice) node.
      virtual IKTBNLearner< GUM_SCALAR >& eraseNoParentNode(std::string_view base, int slice) = 0;

      /// @brief Undo addNoParentNode for a node given by bracket notation.
      virtual IKTBNLearner< GUM_SCALAR >& eraseNoParentNode(std::string_view name) = 0;

      /// @brief Declare a single (base, slice) node as a leaf (no children).
      virtual IKTBNLearner< GUM_SCALAR >& addNoChildrenNode(std::string_view base, int slice) = 0;

      /// @brief Declare a single node (bracket notation, e.g. "X[2]" or "C") as a leaf.
      virtual IKTBNLearner< GUM_SCALAR >& addNoChildrenNode(std::string_view name) = 0;

      /// @brief Undo a previous addNoChildrenNode for a single (base, slice) node.
      virtual IKTBNLearner< GUM_SCALAR >& eraseNoChildrenNode(std::string_view base, int slice)
          = 0;

      /// @brief Undo addNoChildrenNode for a node given by bracket notation.
      virtual IKTBNLearner< GUM_SCALAR >& eraseNoChildrenNode(std::string_view name) = 0;

      /// @brief Add a candidate edge for MIIC (only edges explicitly listed are explored).
      virtual IKTBNLearner< GUM_SCALAR >& addPossibleEdge(std::string_view tailBase,
                                                          int              tailSlice,
                                                          std::string_view headBase,
                                                          int              headSlice)
          = 0;

      /// @brief Add a candidate edge for MIIC using engine names (e.g. "X[1]", "C").
      virtual IKTBNLearner< GUM_SCALAR >& addPossibleEdge(std::string_view tail,
                                                          std::string_view head)
          = 0;

      /// @brief Undo a previous addPossibleEdge.
      virtual IKTBNLearner< GUM_SCALAR >& erasePossibleEdge(std::string_view tailBase,
                                                            int              tailSlice,
                                                            std::string_view headBase,
                                                            int              headSlice)
          = 0;

      /// @brief Undo a previous addPossibleEdge using engine names (e.g. "X[1]", "C").
      virtual IKTBNLearner< GUM_SCALAR >& erasePossibleEdge(std::string_view tail,
                                                            std::string_view head)
          = 0;

      /// @brief Allow or forbid arc additions during structure search.
      virtual IKTBNLearner< GUM_SCALAR >& allowArcAdditions(bool allow = true) = 0;

      /// @brief Allow or forbid arc deletions during structure search.
      virtual IKTBNLearner< GUM_SCALAR >& allowArcDeletions(bool allow = true) = 0;

      /// @brief Allow or forbid arc reversals during structure search.
      virtual IKTBNLearner< GUM_SCALAR >& allowArcReversals(bool allow = true) = 0;

      /// @brief Cap the number of parents of any single node.
      virtual IKTBNLearner< GUM_SCALAR >& setMaxIndegree(Size max_indegree) = 0;

      /// @}

      protected:
      // #######################################################################
      /// @name Name encoding: (base, slice) <-> engine name
      // #######################################################################
      /// @{

      /// @brief The base names of the atemporal (static) variables.
      /// The only subclass-specific input to _determineNode_(): the fixed-k
      /// learner reads it from its prior k-TBN, the adaptive learner from the
      /// atemporal set recorded at construction.
      virtual const std::unordered_set< std::string >& _atemporalVarNames_() const = 0;

      /// @brief Whether @p base is one of this learner's variables, temporal or
      /// atemporal. The second subclass-specific input to the shared helpers.
      /// A predicate rather than a _temporalVarNames_() counterpart to
      /// _atemporalVarNames_(): the adaptive learner holds the base names and
      /// the atemporal ones but never a temporal-only set, and would have to
      /// materialise and cache a third one just to return a reference.
      virtual bool _isKnownBase_(std::string_view base) const = 0;

      /// @brief (base, slice) -> engine name ("A[1]" / atemporal engine name).
      /// Pure function, shared by every learner.
      std::string _encode_(std::string_view base, int slice) const;

      /// @brief engine name -> (base, slice); atemporal names map to KTBN::ATEMPORAL.
      /// Shared by every learner; only the atemporal test varies (via
      /// _atemporalVarNames_()).
      std::pair< std::string, int > _determineNode_(const std::string& name) const;

      /// @brief Reject an arc the k-TBN definition can never contain, so
      /// eraseForbiddenArc and addMandatoryArc both fail early (before any
      /// internal learner is touched) with a uniform message. @p action is the
      /// verb phrase completing "cannot <action> <tail> -> <head>: ..." (e.g.
      /// "force the mandatory arc", "un-forbid the arc"). Two arcs are refused:
      /// a temporal -> atemporal arc (a time-varying variable can never parent a
      /// static one) and a backward-in-time arc (head strictly before tail).
      void _checkArcTemporallyFeasible_(std::string_view tail,
                                        std::string_view head,
                                        std::string_view action) const;

      /// @brief Throw InvalidArgument unless @p base is a known @b temporal
      /// base. @p context completes "cannot appear in <context>" (e.g. "an
      /// intra-slice constraint", "a kernel-relative arc"): every caller
      /// rejects an atemporal base for the same underlying reason -- it has no
      /// per-slice instance -- so they share the sentence and vary the setting.
      void _checkBaseIsTemporal_(std::string_view base, std::string_view context) const;

      /// @brief Throw InvalidArgument unless @p order is at least 2, @p label
      /// naming the offending parameter ("k" for the fixed-k learner, "kMax"
      /// for the adaptive one).
      ///
      /// This is a @b learner constraint, not a model one -- gum::KTBN,
      /// gum::KTBNGenerator and gum::KTBNInference all accept k = 1 -- which is
      /// why it lives here rather than on the model. Static because four of its
      /// call sites are themselves static helpers invoked from member-initialiser
      /// lists, before any object exists.
      static void _checkMinimalOrder_(Size order, std::string_view label);

      /// @brief Scans every trajectory and returns the base names classified
      /// atemporal: those whose value never changes across the rows of a
      /// single trajectory (checked independently per trajectory, so the
      /// value may still differ between trajectories). Shared by every
      /// atemporal-inferring CSV constructor (KTBNLearner, KTBNAdaptiveLearner):
      /// identical scanning logic regardless of which subclass calls it, so
      /// duplicating it per subclass would only invite the two copies to
      /// drift. Callers are responsible for validating their own arguments
      /// (e.g. k/kMax >= 2) \e before calling this — it does no such check
      /// itself, only opens files, so a bad argument would otherwise only be
      /// caught after a wasted scan.
      ///
      /// Column indices still "live" (not yet proven non-constant) are
      /// tracked as a shrinking list rather than rescanned from scratch, and
      /// scanning stops opening further trajectories once that list is empty
      /// — falsification is one-way, so nothing left to test can ever become
      /// atemporal again.
      static std::unordered_set< std::string >
          _scanConstantColumns_(std::string_view                  dirPath,
                                std::string_view                  csvBaseName,
                                Size                              nbSamples,
                                const std::vector< std::string >& missingSymbols);

      /// @}
    };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class IKTBNLearner< double >;
#endif

  } /* namespace learning */
} /* namespace gum */

#include <agrum/KTBN/learning/IKTBNLearner_tpl.h>

#endif /* GUM_LEARNING_IKTBN_LEARNER_H */
