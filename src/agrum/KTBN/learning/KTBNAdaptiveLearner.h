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
 * @brief A k-TBN learner that also selects the order k from the data.
 *
 * gum::learning::KTBNAdaptiveLearner is the k-selecting counterpart of
 * gum::learning::KTBNLearner: instead of being given the order @f$k@f$, it
 * explores every candidate @f$k \in [k_{min}, k_{max}]@f$, learns one k-TBN
 * per candidate and keeps the best one according to a model-selection
 * criterion. @f$k_{min}@f$ starts at 2 and is raised automatically by the
 * structural-constraint setters (see below); it is never given directly.
 *
 * It implements the same configuration interface as KTBNLearner
 * (gum::learning::IKTBNLearner) but its setters do not apply anything
 * immediately: since no @f$k@f$ is fixed yet, they only @e record the
 * requested configuration into plain string-based sets / attributes. At learn
 * time, that recorded configuration is replayed onto a freshly-constructed
 * KTBNLearner for each candidate @f$k@f$:
 *   - constraints whose slice indices do not fit the current candidate
 *     (slice @f$\geq k@f$) are skipped for that candidate only;
 *   - constraints naming an unknown base variable are rejected eagerly, at
 *     setter-call time (same behaviour as BNLearner / KTBNLearner);
 *   - a forbidden/mandatory arc, possible edge, or no-parent/no-children node
 *     naming a concrete slice @f$t@f$ also raises @f$k_{min}@f$ to
 *     @f$t+1@f$, so learnKTBN() skips candidates that would only end up
 *     silently dropping it instead of learning them for nothing.
 *
 * @author Seth AGUILA & Anis KHACEF
 */

#ifndef GUM_LEARNING_KTBN_ADAPTIVE_LEARNER_H
#define GUM_LEARNING_KTBN_ADAPTIVE_LEARNER_H

#include <limits>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/core/math/variableLog2ParamComplexity.h>
#include <agrum/BN/learning/BNLearnUtils/IBNLearner.h>
#include <agrum/BN/learning/correctedMutualInformation.h>
#include <agrum/KTBN/KTBN.h>
#include <agrum/KTBN/learning/IKTBNLearner.h>
#include <agrum/KTBN/learning/KTBNLearner.h>


namespace gum {

  namespace learning {

    /**
     * @class KTBNAdaptiveLearner
     * @headerfile KTBNAdaptiveLearner.h <agrum/KTBN/learning/KTBNAdaptiveLearner.h>
     * @brief Learns a k-TBN (order k + structure + parameters) from trajectory CSVs.
     * @ingroup learning_group
     *
     * @see gum::learning::IKTBNLearner for the configuration interface,
     *      gum::learning::KTBNLearner for the fixed-k learner this class
     *      builds and drives once per candidate k.
     */
    template < GUM_Numeric GUM_SCALAR >
    class KTBNAdaptiveLearner: public IKTBNLearner< GUM_SCALAR > {
      public:
      // #######################################################################
      /// @name Constructors / Destructors
      // #######################################################################
      /// @{

      /**
       * @brief Constructor — the candidate orders are kMin..kMax.
       *
       * Mirrors the CSV constructor of gum::learning::KTBNLearner, except that
       * no single order k is given: the learner explores every
       * @f$k \in [k_{min}, kMax]@f$ at learn time. @f$k_{min}@f$ starts at 2
       * (k = 1 is a static BN; use BNLearner) and is raised by the
       * structural-constraint setters — see the class doc.
       *
       * @param dirPath        Directory holding the trajectory CSV files.
       * @param csvBaseName    Stem of each file name (1-based index and @c .csv
       *        appended, e.g. @c "traj" -> @c "traj1.csv", @c "traj2.csv", ...).
       * @param nbSamples      Number of CSV files to read.
       * @param kMax           Largest order to explore. Must be @f$\geq 2@f$.
       * @param atemporalVars  Base names of the atemporal (static) variables.
       *        Pass an empty set for "every variable is temporal"; it has no
       *        default, since omitting it selects the inferring overload below.
       * @param missingSymbols Symbols in the CSVs to interpret as missing values.
       * @param induceTypes    When true (default), columns whose values are all
       *        numeric are retyped (integer/range/continuous) instead of being
       *        treated as plain labels — same semantics as KTBNLearner.
       *
       * @warning A @b braced literal is ambiguous between this overload and the
       * inferring one, since it could equally initialize either's fifth
       * parameter. Name the type — @c std::unordered_set<std::string>{"C","D"}
       * — or pass a named variable.
       */
      KTBNAdaptiveLearner(std::string_view                         dirPath,
                           std::string_view                         csvBaseName,
                           Size                                     nbSamples,
                           Size                                     kMax,
                           const std::unordered_set< std::string >& atemporalVars,
                           const std::vector< std::string >&        missingSymbols = {"?"},
                           bool                                     induceTypes    = true);

      /**
       * @brief Structure-learning constructor — atemporal variables inferred from the CSVs.
       *
       * Same as the explicit-@p atemporalVars constructor, except a base
       * variable is classified atemporal iff its value never changes within a
       * single trajectory (it may still differ \e between trajectories, e.g.
       * a per-individual covariate such as age). A column with too few
       * non-missing values to ever witness a change is classified atemporal
       * optimistically.
       *
       * This is a heuristic: a genuinely temporal variable that happens to
       * hold one value throughout every sampled trajectory (short horizon,
       * near-deterministic process) will be misclassified. Prefer the
       * explicit-@p atemporalVars constructor when the classification is
       * already known.
       *
       * @warning Domains are still discovered from trajectory 1 alone. An
       * inferred atemporal variable, by construction, may vary between
       * trajectories — exactly the case most likely to reveal only one of
       * several modalities in trajectory 1 and later throw
       * UnknownLabelInDatabase at learnKTBN() time. Use the BN-schema
       * constructor if the full domain isn't guaranteed present in trajectory 1.
       *
       * @warning Unlike every other constructor, this one is not fully lazy:
       * it keeps opening trajectories until every column has been falsified
       * as a constancy candidate, so a missing/malformed file is usually (not
       * always — an early-falsifying column set can stop the scan first)
       * caught here rather than at learnKTBN().
       *
       * @param dirPath        Directory holding the trajectory CSV files.
       * @param csvBaseName    Stem of each file name (1-based index and @c .csv
       *        appended, e.g. @c "traj" -> @c "traj1.csv", @c "traj2.csv", ...).
       * @param nbSamples      Number of CSV files to read.
       * @param kMax           Largest order to explore. Must be @f$\geq 2@f$.
       * @param missingSymbols Symbols in the CSVs to interpret as missing values.
       * @param induceTypes    When true (default), columns whose values are all
       *        numeric are retyped (integer/range/continuous) instead of being
       *        treated as plain labels.
       */
      KTBNAdaptiveLearner(std::string_view                  dirPath,
                          std::string_view                  csvBaseName,
                          Size                              nbSamples,
                          Size                              kMax,
                          const std::vector< std::string >& missingSymbols = {"?"},
                          bool                               induceTypes    = true);

      /**
       * @brief Variable-schema constructor — types and domains supplied via a BN.
       *
       * The adaptive counterpart of gum::learning::KTBNLearner's BN constructor.
       * Use it when a variable's full domain is not guaranteed to appear in the
       * first trajectory — most notably @e atemporal variables, which are constant
       * within a trajectory and so only ever reveal one value per file, and the
       * CSV constructor would then infer an incomplete domain and later throw
       * gum::UnknownLabelInDatabase. Each per-k KTBNLearner is built from @p bn so
       * every variable's type and domain is fixed up front; @c induceTypes and the
       * trajectory-1 domain inference are bypassed entirely.
       *
       * @param dirPath        Directory holding the trajectory CSV files.
       * @param csvBaseName    Stem of each file name (index and @c .csv appended).
       * @param nbSamples      Number of CSV files to read.
       * @param kMax           Largest order to explore. Must be @f$\geq 2@f$.
       * @param bn             A BayesNet with one node per base variable providing
       *        the variable types and domains. Arcs in @p bn are ignored.
       * @param atemporalVars  Base names of the atemporal (static) variables
       *        present in @p bn.
       * @param missingSymbols Symbols in the CSVs to interpret as missing values.
       */
      KTBNAdaptiveLearner(std::string_view                         dirPath,
                           std::string_view                         csvBaseName,
                           Size                                     nbSamples,
                           Size                                     kMax,
                           const BayesNet< GUM_SCALAR >&            bn,
                           const std::unordered_set< std::string >& atemporalVars  = {},
                           const std::vector< std::string >&        missingSymbols = {"?"});

      ~KTBNAdaptiveLearner();

      /// @}
      // #######################################################################
      /// @name Main learning methods
      // #######################################################################
      /// @{

      /// @brief Learns the best k in [kMin, kMax] together with the structure
      /// and the CPTs: one KTBNLearner is built and run per candidate k, the
      /// recorded configuration is replayed on each, and the k-TBN with the
      /// best model-selection score is returned. kMin starts at 2 and is
      /// raised by the structural-constraint setters (see class doc).
      KTBN< GUM_SCALAR > learnKTBN() override;

      /// @brief Order k selected by the last learnKTBN() call.
      /// @throw OperationNotAllowed if learnKTBN() has not run yet.
      Size bestK() const;

      /// @brief Per-candidate cross-k scores from the last learnKTBN() call, as
      /// (k, score) pairs for k = kMin..kMax in ascending k order — the values the
      /// order selection compared to choose bestK() (higher is better; the argmax
      /// is bestK()). Under the default useOrderScoreBIC() each score is
      /// @f$\log_2 L - \tfrac{1}{2}\,d\,\log_2 N@f$ of the k-TBN learned for that k.
      /// @throw OperationNotAllowed if learnKTBN() has not run yet.
      const std::vector< std::pair< Size, double > >& scorePerCandidateK() const;

      /// @brief Engine-name (tail, head) pairs of arcs the selected model's MIIC
      /// run flagged as hiding a latent variable. Empty when the recorded
      /// algorithm is not MIIC (only MIIC produces latent-variable annotations)
      /// or when none were found.
      /// @throw OperationNotAllowed if learnKTBN() has not run yet.
      const std::vector< std::pair< std::string, std::string > >& latentVariables() const;

      /// @}
      // #######################################################################
      /// @name Order selection (cross-k model selection)
      // #######################################################################
      /// @{

      /// @brief The criterion learnKTBN() uses to pick the best @f$k@f$ among the
      /// candidates — the @e outer score, distinct from the per-k structure score
      /// set by useScoreX(). Only criteria that are comparable across @f$k@f$
      /// belong here (raw score types are not). Extended as more are added.
      enum class OrderScoreType { BIC, AIC, fNML };

      /// @brief Select k by BIC (the default): keep the k whose learned model
      /// maximises @f$\log_2 L - \tfrac{1}{2}\,d\,\log_2 N@f$.
      KTBNAdaptiveLearner< GUM_SCALAR >& useOrderScoreBIC();

      /// @brief Select k by AIC: keep the k whose learned model maximises
      /// @f$\log_2 L - d@f$ (a lighter, sample-size-independent complexity
      /// penalty than BIC).
      KTBNAdaptiveLearner< GUM_SCALAR >& useOrderScoreAIC();

      /// @brief Select k by fNML (factorized Normalized Maximum Likelihood): keep
      /// the k whose learned model maximises @f$\log_2 L - \sum_i \sum_j
      /// \log_2 C^{r_i}_{N_{ij}}@f$, where the penalty replaces BIC's
      /// @f$\tfrac{1}{2}\,d\,\log_2 N@f$ by a sum of per-node, per-parent-configuration
      /// multinomial parametric complexities (regret) -- data-dependent, unlike
      /// BIC/AIC, matching aGrUM's ScorefNML.
      KTBNAdaptiveLearner< GUM_SCALAR >& useOrderScorefNML();

      /// @}
      // #######################################################################
      /// @name Missing values
      // #######################################################################
      /// @{

      /**
       * @brief Learn and score on the fully observed data only, dropping every
       * row and every scoring instance that carries a missing symbol.
       *
       * Without this, learnKTBN() refuses trajectories containing missing values:
       * aGrUM's structure learning cannot cope with them
       * (gum::learning::IBNLearner::learnDag_ raises MissingValueInDatabase), so
       * there is nothing sensible to hand it. With it, each candidate's databases
       * are built by complete-case selection and the cross-k score skips any
       * instance whose family is incomplete — the two agree on which data counts.
       *
       * @warning <b>This inflates @f$\log_2 L@f$, and inflates it more for larger
       * @f$k@f$ — so it biases the order selection this class exists to perform.</b>
       *
       * A row or instance is dropped as soon as @b one cell of its window is
       * missing, so a single gap costs up to @f$k@f$ of them. A larger @f$k@f$
       * spans more rows per window and loses proportionally more.
       *
       * @f$\log_2 L@f$ is a @e sum of per-instance terms, each @f$\leq 0@f$.
       * Dropping instances removes negative terms and therefore @b raises the
       * total: the candidate is not fitting better, it is being charged for less
       * data. Since the larger @f$k@f$ drops more, it gains more — precisely the
       * comparison bestK() rests on.
       *
       * The BIC penalty does not compensate. @f$\log_2 N@f$ is computed once
       * from the raw trajectory lengths (deliberately, so it is identical across
       * candidates) and so describes a sample size the likelihood no longer uses;
       * the penalty is unchanged while the likelihood is inflated.
       *
       * The distortion grows with the missing-value rate. At a few percent it is
       * usually harmless; on heavily incomplete trajectories bestK() can be
       * pulled a full order upwards. Inspect scorePerCandidateK() rather than
       * trusting bestK() alone, and prefer complete trajectories whenever the
       * order itself is the question. gum::learning::KTBNLearner::nbDroppedRows()
       * reports how much each candidate actually lost.
       */
      KTBNAdaptiveLearner< GUM_SCALAR >& ignoreMissingSymbols(bool ignore = true);

      /// @brief Whether incomplete rows and instances are dropped. False by default.
      bool isIgnoringMissingSymbols() const;

      /// @}
      // #######################################################################
      /// @name Diagnostics
      // #######################################################################
      /// @{

      /// @brief Largest order explored (the kMax argument of the constructor).
      Size kMax() const;

      /// @brief Warning string if the recorded score and prior are incompatible,
      /// empty otherwise. Data-free: it evaluates the recorded (score, prior)
      /// pair exactly as KTBNLearner/BNLearner would, so an incompatible
      /// combination can be caught before learnKTBN() reads any trajectory.
      std::string checkScorePriorCompatibility() const;

      /// @brief Human-readable summary of the recorded configuration (candidate
      /// order range, algorithm / score / correction / prior, structural
      /// constraints) plus the selected k once learnKTBN() has run.
      std::string toString() const;

      /// @brief The recorded configuration as (key, value, comment) tuples
      /// (mirrors KTBNLearner::state()); toString() is a formatted view of it.
      std::vector< std::tuple< std::string, std::string, std::string > > state() const;

      /// @}
      // #######################################################################
      /// @name Score selection (recorded, replayed on each candidate k)
      // #######################################################################
      /// @{

      KTBNAdaptiveLearner< GUM_SCALAR >& useScoreAIC() override;
      KTBNAdaptiveLearner< GUM_SCALAR >& useScoreBD() override;
      KTBNAdaptiveLearner< GUM_SCALAR >& useScoreBDeu() override;
      KTBNAdaptiveLearner< GUM_SCALAR >& useScoreBIC() override;
      KTBNAdaptiveLearner< GUM_SCALAR >& useScoreLog2Likelihood() override;
      KTBNAdaptiveLearner< GUM_SCALAR >& useScoreMDL() override;
      void                                useScorefNML() override;

      /// @}
      // #######################################################################
      /// @name Algorithm selection (recorded, replayed on each candidate k)
      // #######################################################################
      /// @{

      KTBNAdaptiveLearner< GUM_SCALAR >& useGreedyHillClimbing() override;
      KTBNAdaptiveLearner< GUM_SCALAR >& useExtendedGreedyHillClimbing() override;
      KTBNAdaptiveLearner< GUM_SCALAR >& useLocalSearchWithTabuList(Size tabu_size   = 100,
                                                                     Size nb_decrease = 2) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& useMIIC() override;

      /// @}
      // #######################################################################
      /// @name MIIC correction (recorded, replayed on each candidate k)
      // #######################################################################
      /// @{

      KTBNAdaptiveLearner< GUM_SCALAR >& useNMLCorrection() override;
      KTBNAdaptiveLearner< GUM_SCALAR >& useMDLCorrection() override;
      KTBNAdaptiveLearner< GUM_SCALAR >& useNoCorrection() override;

      /// @}
      // #######################################################################
      /// @name Prior selection (recorded, replayed on each candidate k)
      // #######################################################################
      /// @{

      KTBNAdaptiveLearner< GUM_SCALAR >& useSmoothingPrior(double weight = 1.0) override;

      /// @}
      // #######################################################################
      /// @name Structural constraints (recorded, replayed on each candidate k)
      // #######################################################################
      /// @{

      KTBNAdaptiveLearner< GUM_SCALAR >& addForbiddenArc(std::string_view tailNode,
                                                          std::string_view headNode) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& addForbiddenArc(std::string_view tailBase,
                                                          int              tailSlice,
                                                          std::string_view headBase,
                                                          int              headSlice) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& eraseForbiddenArc(std::string_view tailNode,
                                                            std::string_view headNode) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& eraseForbiddenArc(std::string_view tailBase,
                                                            int              tailSlice,
                                                            std::string_view headBase,
                                                            int              headSlice) override;

      KTBNAdaptiveLearner< GUM_SCALAR >& addMandatoryArc(std::string_view tailNode,
                                                          std::string_view headNode) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& addMandatoryArc(std::string_view tailBase,
                                                          int              tailSlice,
                                                          std::string_view headBase,
                                                          int              headSlice) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& eraseMandatoryArc(std::string_view tailNode,
                                                            std::string_view headNode) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& eraseMandatoryArc(std::string_view tailBase,
                                                            int              tailSlice,
                                                            std::string_view headBase,
                                                            int              headSlice) override;

      /// @brief Forbid an arc from @p tailBase, @p lag slices before the kernel,
      /// to @p headBase in the kernel: tailBase at slice k-1-lag -> headBase at
      /// slice k-1, for whichever k is selected. The slice is not given directly
      /// but derived from @p lag relative to the kernel slice k-1, which moves
      /// with the candidate — so no absolute slice could express it at record
      /// time. Adaptive-only: the fixed-k learners have no moving kernel slice to
      /// anchor it to. @p lag must be in [0, kMax) and both bases temporal (an
      /// atemporal variable has no kernel-slice instance). Raises _kMin_ to at
      /// least lag+1, like a constraint naming slice lag.
      KTBNAdaptiveLearner< GUM_SCALAR >&
          addForbiddenKernelArc(std::string_view tailBase, int lag, std::string_view headBase);

      /// @brief Undo a previous addForbiddenKernelArc (same (tailBase, lag,
      /// headBase) triple).
      KTBNAdaptiveLearner< GUM_SCALAR >&
          eraseForbiddenKernelArc(std::string_view tailBase, int lag, std::string_view headBase);

      /// @brief Force an arc from @p tailBase, @p lag slices before the kernel,
      /// to @p headBase in the kernel. See addForbiddenKernelArc for the
      /// (base, slice) vs. kernel-relative distinction; the validity
      /// constraints (@p lag range, temporal bases, _kMin_ effect) are the same.
      /// Always temporally feasible by construction (lag >= 0 guarantees
      /// headSlice = k-1 >= k-1-lag = tailSlice), so unlike the plain
      /// addMandatoryArc there is no separate feasibility check to run.
      KTBNAdaptiveLearner< GUM_SCALAR >&
          addMandatoryKernelArc(std::string_view tailBase, int lag, std::string_view headBase);

      /// @brief Undo a previous addMandatoryKernelArc (same (tailBase, lag,
      /// headBase) triple).
      KTBNAdaptiveLearner< GUM_SCALAR >&
          eraseMandatoryKernelArc(std::string_view tailBase, int lag, std::string_view headBase);

      KTBNAdaptiveLearner< GUM_SCALAR >&
          addForbiddenIntraSliceArc(std::string_view tailBase, std::string_view headBase) override;
      KTBNAdaptiveLearner< GUM_SCALAR >&
          eraseForbiddenIntraSliceArc(std::string_view tailBase,
                                      std::string_view headBase) override;

      KTBNAdaptiveLearner< GUM_SCALAR >&
          addForbiddenArcAllSlices(std::string_view tailBase, std::string_view headBase) override;
      KTBNAdaptiveLearner< GUM_SCALAR >&
          eraseForbiddenArcAllSlices(std::string_view tailBase, std::string_view headBase) override;

      KTBNAdaptiveLearner< GUM_SCALAR >& addNoParentNode(std::string_view base,
                                                          int              slice) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& addNoParentNode(std::string_view name) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& eraseNoParentNode(std::string_view base,
                                                            int              slice) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& eraseNoParentNode(std::string_view name) override;

      KTBNAdaptiveLearner< GUM_SCALAR >& addNoChildrenNode(std::string_view base,
                                                            int              slice) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& addNoChildrenNode(std::string_view name) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& eraseNoChildrenNode(std::string_view base,
                                                              int              slice) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& eraseNoChildrenNode(std::string_view name) override;

      KTBNAdaptiveLearner< GUM_SCALAR >& addPossibleEdge(std::string_view tailBase,
                                                          int              tailSlice,
                                                          std::string_view headBase,
                                                          int              headSlice) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& addPossibleEdge(std::string_view tail,
                                                          std::string_view head) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& erasePossibleEdge(std::string_view tailBase,
                                                            int              tailSlice,
                                                            std::string_view headBase,
                                                            int              headSlice) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& erasePossibleEdge(std::string_view tail,
                                                            std::string_view head) override;

      KTBNAdaptiveLearner< GUM_SCALAR >& allowArcAdditions(bool allow = true) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& allowArcDeletions(bool allow = true) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& allowArcReversals(bool allow = true) override;
      KTBNAdaptiveLearner< GUM_SCALAR >& setMaxIndegree(Size max_indegree) override;

      /// @}

      private:
      // ----- data source (kept so a KTBNLearner can be built per candidate k) -----

      /// directory holding the trajectory CSV files
      std::string _dirPath_;

      /// stem of each trajectory file name
      std::string _csvBaseName_;

      /// number of CSV files to read
      Size _nbSamples_;

      /// largest order to explore (candidates are kMin..kMax)
      Size _kMax_;

      /// smallest order worth exploring (candidates are kMin..kMax). Starts at
      /// 2 and is recomputed by _recomputeKMin_() from the recorded
      /// slice-bearing constraints every time one is added or erased; never set
      /// directly. See _recomputeKMin_() for why it must be a recompute rather
      /// than an incremental raise/lower.
      Size _kMin_{2};

      /// base names of the atemporal (static) variables
      std::unordered_set< std::string > _atemporalVars_;

      /// all base variable names (temporal + atemporal), read from the first
      /// trajectory CSV header at construction. Authoritative universe used by
      /// the constraint setters to reject names that do not exist.
      std::unordered_set< std::string > _baseNames_;

      /// symbols in the CSVs to interpret as missing values
      std::vector< std::string > _missingSymbols_;

      /// whether incomplete rows/instances are dropped (see ignoreMissingSymbols())
      bool _ignoreMissingSymbols_{false};

      /// whether numeric columns are retyped (see KTBNLearner); unused (and
      /// forced false) when a schema BN is supplied
      bool _induceTypes_;

      /// optional variable-schema BN (set by the BN constructor): when present,
      /// each per-k KTBNLearner is built from it so the variable domains are fixed
      /// explicitly instead of inferred from trajectory 1. Null in CSV mode.
      std::unique_ptr< BayesNet< GUM_SCALAR > > _prior_bn_;

      // ----- recorded state parameters (replayed on each candidate k) -----
      // Defaults mirror IBNLearner's own defaults, so an unconfigured
      // KTBNAdaptiveLearner behaves like an unconfigured KTBNLearner.

      /// recorded per-k structure score (the inner score, replayed on each candidate)
      IBNLearner::ScoreType _score_{IBNLearner::ScoreType::BDeu};

      /// recorded cross-k order-selection criterion (the outer score used by
      /// learnKTBN() to pick the best k; independent of _score_)
      OrderScoreType _orderScore_{OrderScoreType::BIC};

      /// recorded structure-learning algorithm
      IBNLearner::AlgoType _algo_{IBNLearner::AlgoType::MIIC};

      /// tabu-list parameters (meaningful when _algo_ is LOCAL_SEARCH_WITH_TABU_LIST)
      Size _tabuSize_{100};
      Size _nbDecrease_{2};

      /// recorded MIIC correction
      CorrectedMutualInformation::KModeTypes _correction_{
          CorrectedMutualInformation::KModeTypes::MDL};

      /// recorded prior and its weight
      IBNLearner::BNLearnerPriorType _prior_{IBNLearner::BNLearnerPriorType::NO_prior};
      double                         _priorWeight_{1.0};

      /// recorded graph-change permissions / indegree cap
      bool _allowAdditions_{true};
      bool _allowDeletions_{true};
      bool _allowReversals_{true};
      Size _maxIndegree_{std::numeric_limits< Size >::max()};

      // ----- recorded constraints (replayed on each candidate k) -----
      // Everything is stored by name: NodeIds are meaningless here because the
      // internal NodeId spaces differ from one candidate k to another. Arcs and
      // edges are stored as engine-name pairs ("X[1]", "C", ...); the per-base
      // constraints (intra-slice / all-slices) are stored as base-name pairs.

      /// forbidden arcs, as (tail, head) engine-name pairs
      std::set< std::pair< std::string, std::string > > _forbiddenArcs_;

      /// mandatory arcs, as (tail, head) engine-name pairs
      std::set< std::pair< std::string, std::string > > _mandatoryArcs_;

      /// MIIC candidate edges, as (tail, head) engine-name pairs
      std::set< std::pair< std::string, std::string > > _possibleEdges_;

      /// forbidden intra-slice arcs, as (tailBase, headBase) pairs
      std::set< std::pair< std::string, std::string > > _forbiddenIntraSliceArcs_;

      /// forbidden all-slices arcs, as (tailBase, headBase) pairs
      std::set< std::pair< std::string, std::string > > _forbiddenArcsAllSlices_;

      /// forbidden kernel-relative arcs, as (tailBase, headBase, lag) triples:
      /// tailBase at slice k-1-lag -> headBase at the kernel slice k-1. Kept
      /// separate from _forbiddenArcs_ (which stores resolved engine names)
      /// since the slice here is only known once a candidate k is fixed.
      std::set< std::tuple< std::string, std::string, int > > _forbiddenKernelArcs_;

      /// mandatory kernel-relative arcs, same shape as _forbiddenKernelArcs_
      std::set< std::tuple< std::string, std::string, int > > _mandatoryKernelArcs_;

      /// root nodes (no parents), as engine names
      std::set< std::string > _noParentNodes_;

      /// leaf nodes (no children), as engine names
      std::set< std::string > _noChildrenNodes_;

      /// k selected by the last learnKTBN() call, or 0 as a sentinel while no
      /// learning has happened yet. 0 can never be a valid order (candidates are
      /// 2..kMax), so bestK() reads it to decide whether learnKTBN() has run.
      Size _bestK_{0};

      /// latent-variable arcs (engine-name pairs) reported by the winning
      /// candidate's MIIC run, captured by learnKTBN(); empty when the selected
      /// algorithm is not MIIC. Exposed by latentVariables().
      std::vector< std::pair< std::string, std::string > > _bestLatentVariables_;

      /// per-candidate (k, order-score) pairs from the last learnKTBN() run, in
      /// ascending k. One entry appended per candidate; read by scorePerCandidateK().
      /// Cleared at the start of each learnKTBN() (empty while _bestK_ == 0).
      std::vector< std::pair< Size, double > > _scorePerCandidateK_;

      // ----- construction (runs once, in the constructor) -----

      /// checks @c kMax \>= 2 and @c nbSamples \>= 1, then delegates to the
      /// shared IKTBNLearner::_scanConstantColumns_(). Called from the
      /// atemporal-inferring constructor's member-initialiser list, before
      /// this object exists, so the checks live here rather than in the body
      /// — otherwise a bad argument would only surface after a wasted scan.
      static std::unordered_set< std::string >
          _inferAtemporalVars_(std::string_view                  dirPath,
                               std::string_view                  csvBaseName,
                               Size                              nbSamples,
                               Size                              kMax,
                               const std::vector< std::string >& missingSymbols);

      // ----- name encoding -----

      /// atemporal base names for IKTBNLearner's shared _encode_/_determineNode_;
      /// the set recorded at construction.
      const std::unordered_set< std::string >& _atemporalVarNames_() const override;

      /// whether @p base is one of this learner's variables; the base names read
      /// at construction, from the CSV header or the schema BN.
      bool _isKnownBase_(std::string_view base) const override;

      /// bring the shared helpers of the dependent base into unqualified scope,
      /// so member functions can call _encode_(...) / _determineNode_(...) plainly.
      using IKTBNLearner< GUM_SCALAR >::_encode_;
      using IKTBNLearner< GUM_SCALAR >::_determineNode_;
      using IKTBNLearner< GUM_SCALAR >::_checkBaseIsTemporal_;

      // ----- validation -----

      /// @brief Throw InvalidArgument unless @p base is a known base variable and
      /// @p slice is valid for it: @p base must be in _baseNames_; an atemporal
      /// @p base requires @p slice == KTBN::ATEMPORAL, a temporal one requires
      /// @p slice in [0, kMax). Used by the constraint setters to reject unknown
      /// names / out-of-range slices eagerly.
      void _verifyBase_(std::string_view base, int slice) const;

      /// @brief Throw InvalidArgument unless @p tailBase and @p headBase are
      /// known, temporal base variables (a kernel-relative arc has no meaning
      /// for an atemporal one: it has no kernel-slice instance) and @p lag is
      /// in [0, kMax) — so some candidate k in [2,kMax] can place tailBase at
      /// slice k-1-lag >= 0 and headBase at the kernel slice k-1. Used by the
      /// four kernel-arc setters (add/eraseForbiddenKernelArc,
      /// add/eraseMandatoryKernelArc) to reject bad calls eagerly.
      void _verifyKernelArc_(std::string_view tailBase, std::string_view headBase, int lag) const;

      // ----- candidate-range bookkeeping -----

      /// @brief Raise _kMin_, if needed, so that _kMin_ > @p slice: a smaller
      /// candidate would silently drop a constraint naming that slice (the
      /// "slice < k" rule _applyConstraints_'s fits() filter uses). O(1),
      /// since adding a constraint can only push _kMin_ up. No-op for
      /// KTBN::ATEMPORAL.
      void _raiseKMinForSlice_(int slice);

      /// @brief Recompute _kMin_ from scratch: max(2, 1 + the largest concrete
      /// slice named by any recorded forbidden/mandatory arc, possible edge, or
      /// no-parent/no-children node (the kinds fits() can drop). Intra-slice /
      /// all-slices constraints carry no slice and are excluded. Unlike
      /// _raiseKMinForSlice_, an erase can *lower* _kMin_ with no O(1) way to
      /// tell, so every erase* setter runs this full rescan instead.
      void _recomputeKMin_();

      // ----- configuration replay (used by learnKTBN() for each candidate k) -----

      /// @brief Apply the recorded score / algorithm / correction / prior onto a
      /// freshly-built @p learner. Only knobs that differ from their default are
      /// touched, and only those the chosen algorithm actually consumes (a
      /// score-based algo takes a score; MIIC takes a correction) — so an
      /// unconfigured learner is left at its defaults and no irrelevant setting
      /// can trip checkScorePriorCompatibility().
      void _applyConfig_(KTBNLearner< GUM_SCALAR >& learner) const;

      /// @brief Replay the recorded structural constraints onto @p learner (built
      /// for order @p k). Engine-name constraints whose slice does not fit @p k
      /// (slice >= k) are skipped for this candidate; the base-only intra-slice /
      /// all-slices constraints are handed to @p learner, which expands them for
      /// its own k. The allow-* flags and max-indegree are applied when non-default.
      void _applyConstraints_(KTBNLearner< GUM_SCALAR >& learner, Size k) const;

      // ----- model selection (learnKTBN() helper) -----

      /// @brief Stream every scored template-node instance of @p net over the
      /// recorded trajectories, driving both the likelihood and the fNML penalty
      /// off one data walk, so the width-k window logic lives in one place.
      ///
      /// Each trajectory is streamed row by row through a width-k circular
      /// buffer (absolute time @c t at slot <tt>t % k</tt>): every temporal
      /// node @c X[t] is visited against @p net's template CPT for slice
      /// <tt>min(t, k-1)</tt>, reading each parent from row <tt>t - lag</tt>;
      /// atemporal nodes are visited once per trajectory. @p perInstance runs
      /// once per instance with the observed labels loaded into @c e.inst;
      /// @p perNodeFinal runs once per template node after the stream, for
      /// criteria reducing accumulated per-node state (e.g. fNML's regret).
      template < typename PerInstance, typename PerNodeFinal >
      void _forEachScoredNode_(const KTBN< GUM_SCALAR >& net,
                               PerInstance               perInstance,
                               PerNodeFinal              perNodeFinal) const;

      /// @brief Factorized log2-likelihood of @p net over the recorded
      /// trajectories, comparable across candidate k (the raw score types are
      /// not: their flattened tables differ from one k to another). The data is
      /// complete, so this is a closed-form sum of CPT lookups — no inference.
      /// A thin reduction over _forEachScoredNode_ that sums @c log2 of the CPT
      /// entry over every visited node instance.
      double _log2Likelihood_(const KTBN< GUM_SCALAR >& net) const;

      /// @brief Number of free parameters of @p net's template: summed over every
      /// template node (all initial slices, the kernel slice and the atemporal
      /// nodes), @f$(|X|-1)\prod_{P\in pa(X)}|P|@f$. This is the model-complexity
      /// term of the cross-k BIC and AIC scores used by learnKTBN(): it grows
      /// with @f$k@f$ (a higher order adds initial-slice CPTs and can widen the
      /// kernel), which is what offsets the monotonic likelihood gain and
      /// prevents always picking the largest @f$k@f$.
      double _countParameters_(const KTBN< GUM_SCALAR >& net) const;

      /// @brief fNML order score of @p net over the recorded trajectories:
      /// @f$\log_2 L - \sum_i \sum_j \log_2 C^{r_i}_{N_{ij}}@f$, with @f$r_i@f$
      /// the node's domain size and @f$N_{ij}@f$ the count for parent
      /// configuration @f$j@f$. Unlike BIC/AIC the penalty is data-dependent, so
      /// the regret needs the same per-instance walk as the likelihood — both are
      /// accumulated in one _forEachScoredNode_ pass. Mirrors aGrUM's ScorefNML.
      double _fNMLScore_(const KTBN< GUM_SCALAR >& net) const;

      /// @brief The cross-k order-selection score of @p net under the recorded
      /// _orderScore_ criterion, given @p logN = log2 of the sample size. The
      /// single dispatch point learnKTBN() uses to compare candidates; add a
      /// branch here for each new OrderScoreType.
      double _orderSelectionScore_(const KTBN< GUM_SCALAR >& net, double logN) const;

      /// @brief cache of log2 of the multinomial parametric complexity C^r_n, used
      /// by the fNML order penalty. Mutable because log2Cnr() memoizes; the const
      /// order-score helpers may therefore call it. Only touched under fNML.
      mutable VariableLog2ParamComplexity _ctable_;

      // forbidden copies / moves (same policy as KTBNLearner)
      KTBNAdaptiveLearner(const KTBNAdaptiveLearner< GUM_SCALAR >&)            = delete;
      KTBNAdaptiveLearner(KTBNAdaptiveLearner< GUM_SCALAR >&&)                 = delete;
      KTBNAdaptiveLearner< GUM_SCALAR >& operator=(const KTBNAdaptiveLearner< GUM_SCALAR >&)
          = delete;
      KTBNAdaptiveLearner< GUM_SCALAR >& operator=(KTBNAdaptiveLearner< GUM_SCALAR >&&) = delete;

    };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class KTBNAdaptiveLearner< double >;
#endif

  } /* namespace learning */
} /* namespace gum */

#include <agrum/KTBN/learning/KTBNAdaptiveLearner_tpl.h>

#endif /* GUM_LEARNING_KTBN_ADAPTIVE_LEARNER_H */
