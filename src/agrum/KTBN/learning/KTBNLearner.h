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
 * @brief A structure/parameter learner for k-order dynamic Bayesian networks.
 *
 * gum::learning::KTBNLearner is the k-TBN counterpart of
 * gum::learning::BNLearner. It learns a gum::KTBN from a set of @e trajectory
 * CSV files (one file per trajectory, as produced by
 * gum::learning::KTBNDatabaseGenerator: one column per @e base variable,
 * one row per time step).
 *
 * It is @b not a subclass of BNLearner. Instead it wraps @b three BNLearner
 * instances and reduces k-TBN learning to three ordinary BN-learning problems on
 * three flat tables built from the trajectories:
 *   - the @b transition table (sliding windows of width @f$k@f$): learns the
 *     repeating kernel, i.e. the arcs arriving at slice @f$k-1@f$;
 *   - the @b initial table (the first @f$k-1@f$ time steps of each trajectory):
 *     learns the distribution of the initial slices;
 *   - the @b atemporal table (the atemporal columns, one row per trajectory):
 *     learns the arcs between atemporal variables (built only when there are
 *     at least two atemporal variables).
 * The three learned BayesNets are then glued back into a single gum::KTBN.
 * The temporal ordering and no-backward-arc constraints are applied
 * automatically and invisibly.
 *
 * @code
 *  // atemporal variables inferred from the data (see the tagless constructor)
 *  gum::learning::KTBNLearner< double > learner("trajs/", "traj", 500, 2);
 *  learner.useScoreBIC().useGreedyHillClimbing();
 *  gum::KTBN< double >                  kdbn = learner.learnKTBN();
 *
 *  // or state the classification explicitly
 *  const std::unordered_set< std::string > atemporal{"C", "D"};
 *  gum::learning::KTBNLearner< double > learner2("trajs/", "traj", 500, 2, atemporal);
 * @endcode
 *
 * @author Seth AGUILA & Anis KHACEF
 */

#ifndef GUM_LEARNING_KTBN_LEARNER_H
#define GUM_LEARNING_KTBN_LEARNER_H

#include <memory>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/BN/learning/BNLearner.h>
#include <agrum/KTBN/KTBN.h>
#include <agrum/KTBN/learning/IKTBNLearner.h>

namespace gum {

  namespace learning {

    /**
     * @class KTBNLearner
     * @headerfile KTBNLearner.h <agrum/KTBN/learning/KTBNLearner.h>
     * @brief Learns a k-TBN (structure and/or parameters) from trajectory CSVs.
     * @ingroup learning_group
     *
     * @see gum::learning::BNLearner, the static Bayesian-network counterpart whose
     * API this class mirrors. KTBNLearner delegates score / algorithm / prior /
     * constraint settings to three internal BNLearner instances (one per table).
     *
     * Implements gum::learning::IKTBNLearner, the configuration interface shared
     * with gum::learning::KTBNAdaptiveLearner (which learns k as well): here the
     * setters apply each setting to the internal learners immediately, since k is
     * fixed at construction.
     */
    template < GUM_Numeric GUM_SCALAR >
    class KTBNLearner: public IKTBNLearner< GUM_SCALAR > {
      public:
      // #######################################################################
      /// @name Constructors / Destructors
      // #######################################################################
      /// @{

      /**
       * @brief Structure-learning constructor — variable roles supplied explicitly.
       *
       * Use this constructor when you want to learn the k-TBN structure from the
       * data. The order @f$k@f$ and the atemporal variable list must be given
       * because they cannot be reliably inferred from the CSVs alone. Variable
       * domains (modality counts) are read from the CSV data.
       *
       * Internally calls @c _buildPriorFromCSV_() then @c _build_().
       *
       * @param dirPath        Directory holding the trajectory CSV files.
       * @param csvBaseName    Stem of each file name (1-based index and @c .csv
       *        appended, e.g. @c "traj" -> @c "traj1.csv", @c "traj2.csv", ...).
       * @param nbSamples      Number of CSV files to read.
       * @param k              Order of the k-TBN. Must be @f$\geq 2@f$.
       * @param atemporalVars  Base names of the atemporal (static) variables.
       *        All other variables found in the CSV header are treated as temporal.
       *        Pass an empty set for "every variable is temporal"; it has no
       *        default, since omitting it selects the inferring overload below.
       * @param missingSymbols Symbols in the CSVs to interpret as missing values.
       * @param induceTypes    When true (default), columns whose values are all
       *        numeric are retyped (integer/range/continuous) instead of being
       *        treated as plain labels — same semantics as gum::learning::BNLearner.
       * @param ignoreMissingSymbols When true, a row carrying a missing symbol is
       *        dropped rather than handed to the internal learners, which cannot
       *        cope with one (see nbDroppedRows() and its @b warning on the
       *        distortion this introduces). When false (the default), such data
       *        is refused outright.
       *
       * @warning A @b braced literal is ambiguous between this overload and the
       * inferring one, since it could equally initialize either's fifth
       * parameter. Name the type — @c std::unordered_set<std::string>{"C","D"}
       * — or pass a named variable.
       */
      KTBNLearner(std::string_view                  dirPath,
                  std::string_view                  csvBaseName,
                  Size                              nbSamples,
                  Size                              k,
                  const std::unordered_set< std::string >& atemporalVars,
                  const std::vector< std::string >& missingSymbols = {"?"},
                  bool                              induceTypes    = true,
                  bool                              ignoreMissingSymbols = false);

      /**
       * @brief Structure-learning constructor — atemporal variables inferred from the CSVs.
       *
       * Same as the explicit-@p atemporalVars constructor, except the
       * temporal/atemporal classification is inferred instead of supplied: a base
       * variable is classified atemporal iff its value never changes across the
       * rows of any single trajectory. It may still differ \e between
       * trajectories — this matches the model's actual semantics (constant
       * through time, not constant across samples) rather than requiring one
       * global value, e.g. a per-individual static covariate such as an age or
       * a site identifier. A column with too few non-missing values to ever
       * witness a change is optimistically classified atemporal.
       *
       * This is a heuristic, not a guarantee: a genuinely temporal variable that
       * happens to hold one value throughout every sampled trajectory (a very
       * short horizon, or a near-deterministic process) will be misclassified as
       * atemporal. Prefer the explicit-@p atemporalVars constructor whenever the
       * classification is already known.
       *
       * @warning Variable domains are still discovered from trajectory 1 alone
       * (see the explicit-@p atemporalVars constructor). An inferred atemporal
       * variable is, by construction, one whose value may vary between
       * trajectories — exactly the case most likely to reveal only one of
       * several modalities in trajectory 1 and later throw
       * UnknownLabelInDatabase. Use the BN-schema constructor if the full domain
       * cannot be guaranteed present in the first trajectory.
       *
       * @param dirPath        Directory holding the trajectory CSV files.
       * @param csvBaseName    Stem of each file name (1-based index and @c .csv
       *        appended, e.g. @c "traj" -> @c "traj1.csv", @c "traj2.csv", ...).
       * @param nbSamples      Number of CSV files to read.
       * @param k              Order of the k-TBN. Must be @f$\geq 2@f$.
       * @param missingSymbols Symbols in the CSVs to interpret as missing values.
       * @param induceTypes    When true (default), columns whose values are all
       *        numeric are retyped (integer/range/continuous) instead of being
       *        treated as plain labels.
       * @param ignoreMissingSymbols When true, a row carrying a missing symbol is
       *        dropped rather than handed to the internal learners, which cannot
       *        cope with one (see nbDroppedRows() and its @b warning on the
       *        distortion this introduces). When false (the default), such data
       *        is refused outright.
       */
      KTBNLearner(std::string_view                  dirPath,
                  std::string_view                  csvBaseName,
                  Size                              nbSamples,
                  Size                              k,
                  const std::vector< std::string >& missingSymbols = {"?"},
                  bool                              induceTypes    = true,
                  bool                              ignoreMissingSymbols = false);

      /**
       * @brief Variable-schema constructor — types and domains supplied via a BN.
       *
       * Use this constructor when the variable types and domains are already known
       * (e.g. from a reference BN). The BN must contain exactly one node per base
       * variable (temporal or atemporal), with the correct @c DiscreteVariable
       * type and domain. Arcs in @p bn are ignored.
       *
       * Internally calls @c _buildPriorFromBN_() then @c _build_().
       *
       * @param dirPath        Directory holding the trajectory CSV files.
       * @param csvBaseName    Stem of each file name.
       * @param nbSamples      Number of CSV files to read.
       * @param k              Order of the k-TBN. Must be @f$\geq 2@f$.
       * @param bn             A BayesNet with one node per base variable
       *        providing variable types and domains. Atemporal variables are
       *        identified by @p atemporalVars.
       * @param atemporalVars  Base names of the atemporal (static) variables
       *        present in @p bn.
       * @param missingSymbols Symbols in the CSVs to interpret as missing values.
       * @param ignoreMissingSymbols When true, a row carrying a missing symbol is
       *        dropped rather than handed to the internal learners, which cannot
       *        cope with one (see nbDroppedRows() and its @b warning on the
       *        distortion this introduces). When false (the default), such data
       *        is refused outright.
       */
      KTBNLearner(std::string_view                         dirPath,
                  std::string_view                         csvBaseName,
                  Size                                     nbSamples,
                  Size                                     k,
                  const BayesNet< GUM_SCALAR >&            bn,
                  const std::unordered_set< std::string >& atemporalVars  = {},
                  const std::vector< std::string >&        missingSymbols = {"?"},
                  bool                                     ignoreMissingSymbols = false);

      /// destructor
      ~KTBNLearner();

      /// @}
      // #######################################################################
      /// @name Main learning methods
      // #######################################################################
      /// @{

      /// @brief Full learning (structure + CPTs). Mirrors BNLearner::learnBN().
      KTBN< GUM_SCALAR > learnKTBN() override;

      /// @brief CPTs only, using the arc structure of @p structure.
      /// @p structure must have the same base variables (names and domains) as
      /// those used to construct this learner; mismatches throw at learn time.
      KTBN< GUM_SCALAR > learnParameters(const KTBN< GUM_SCALAR >& structure,
                                         bool takeIntoAccountScore = true);

      /// @}
      // #######################################################################
      /// @name Score selection (applied to all internal learners)
      // #######################################################################
      /// @{

      KTBNLearner< GUM_SCALAR >& useScoreAIC() override;
      KTBNLearner< GUM_SCALAR >& useScoreBD() override;
      KTBNLearner< GUM_SCALAR >& useScoreBDeu() override;
      KTBNLearner< GUM_SCALAR >& useScoreBIC() override;
      KTBNLearner< GUM_SCALAR >& useScoreLog2Likelihood() override;
      KTBNLearner< GUM_SCALAR >& useScoreMDL() override;
      void                       useScorefNML() override;

      /// @brief Returns a warning string if the current score and prior are incompatible,
      /// empty string otherwise.
      std::string checkScorePriorCompatibility() const;

      /// @}
      // #######################################################################
      /// @name Algorithm selection (applied to all internal learners)
      // #######################################################################
      /// @{

      KTBNLearner< GUM_SCALAR >& useGreedyHillClimbing() override;
      KTBNLearner< GUM_SCALAR >& useExtendedGreedyHillClimbing() override;
      KTBNLearner< GUM_SCALAR >& useLocalSearchWithTabuList(Size tabu_size   = 100,
                                                            Size nb_decrease = 2) override;
      KTBNLearner< GUM_SCALAR >& useMIIC() override;

      /// @}
      // #######################################################################
      /// @name MIIC correction (applied to all internal learners)
      // #######################################################################
      /// @{

      KTBNLearner< GUM_SCALAR >& useNMLCorrection() override;
      KTBNLearner< GUM_SCALAR >& useMDLCorrection() override;
      KTBNLearner< GUM_SCALAR >& useNoCorrection() override;

      /// @brief Engine-name pairs (tail, head) of arcs flagged as hiding a latent
      /// variable by MIIC (merged from all internal learners). Returned as names rather
      /// than Arc/NodeId pairs because the internal learners live in independent
      /// NodeId spaces, so a bare NodeId would be ambiguous.
      std::vector< std::pair< std::string, std::string > > latentVariables() const;

      /// @}
      // #######################################################################
      /// @name Prior selection (applied to all internal learners)
      // #######################################################################
      /// @{

      KTBNLearner< GUM_SCALAR >& useSmoothingPrior(double weight = 1.0) override;

      /// @}
      // #######################################################################
      /// @name Structural constraints (base names, translated to the relevant internal learner(s))
      // #######################################################################
      /// @{

      /// @brief Forbid @p tailNode from ever parenting @p headNode (engine names,
      /// e.g. "X[1]", "C").
      KTBNLearner< GUM_SCALAR >& addForbiddenArc(std::string_view tailNode,
                                                 std::string_view headNode) override;

      /// @brief Forbid one (base, slice) -> (base, slice) arc (KTBN::ATEMPORAL for static).
      /// A backward arc (@p tailSlice > @p headSlice) is accepted but has no effect:
      /// such an arc is already impossible, so forbidding it is a harmless no-op.
      KTBNLearner< GUM_SCALAR >& addForbiddenArc(std::string_view tailBase,
                                                 int              tailSlice,
                                                 std::string_view headBase,
                                                 int              headSlice) override;

      /// @brief Undo a previous addForbiddenArc (engine names, e.g. "X[1]", "C").
      KTBNLearner< GUM_SCALAR >& eraseForbiddenArc(std::string_view tailNode,
                                                   std::string_view headNode) override;

      /// @brief Undo a previous addForbiddenArc for a specific (base, slice) pair.
      KTBNLearner< GUM_SCALAR >& eraseForbiddenArc(std::string_view tailBase,
                                                   int              tailSlice,
                                                   std::string_view headBase,
                                                   int              headSlice) override;

      /// @brief Force @p tailNode to be a parent of @p headNode (engine names, e.g. "X[1]", "C").
      KTBNLearner< GUM_SCALAR >& addMandatoryArc(std::string_view tailNode,
                                                 std::string_view headNode) override;

      /// @brief Force one arc, lag stated explicitly via slices (KTBN::ATEMPORAL for static).
      KTBNLearner< GUM_SCALAR >& addMandatoryArc(std::string_view tailBase,
                                                 int              tailSlice,
                                                 std::string_view headBase,
                                                 int              headSlice) override;

      /// @brief Undo a previous addMandatoryArc (engine names, e.g. "X[1]", "C").
      KTBNLearner< GUM_SCALAR >& eraseMandatoryArc(std::string_view tailNode,
                                                   std::string_view headNode) override;

      /// @brief Undo a previous addMandatoryArc.
      KTBNLearner< GUM_SCALAR >& eraseMandatoryArc(std::string_view tailBase,
                                                   int              tailSlice,
                                                   std::string_view headBase,
                                                   int              headSlice) override;

      /// @brief Forbid @p tailBase -> @p headBase at every intra-slice position
      /// (i.e. tailBase[t] -> headBase[t] for all t in [0, k-1]).
      /// @throw InvalidArgument if either endpoint is unknown or atemporal (an
      /// atemporal variable has no intra-slice position).
      KTBNLearner< GUM_SCALAR >& addForbiddenIntraSliceArc(std::string_view tailBase,
                                                           std::string_view headBase) override;

      /// @brief Undo a previous addForbiddenIntraSliceArc.
      /// @throw InvalidArgument under the same conditions as addForbiddenIntraSliceArc.
      KTBNLearner< GUM_SCALAR >& eraseForbiddenIntraSliceArc(std::string_view tailBase,
                                                             std::string_view headBase) override;

      /// @brief Forbid @p tailBase -> @p headBase at every causally-possible slice pair
      /// (every lag, not just matching slices): tailBase can never be an ancestor of
      /// headBase in the learned k-TBN. A temporal->atemporal pair is a no-op (already
      /// structurally impossible).
      KTBNLearner< GUM_SCALAR >& addForbiddenArcAllSlices(std::string_view tailBase,
                                                          std::string_view headBase) override;

      /// @brief Undo a previous addForbiddenArcAllSlices.
      KTBNLearner< GUM_SCALAR >& eraseForbiddenArcAllSlices(std::string_view tailBase,
                                                            std::string_view headBase) override;

      /// @brief Declare a single (base, slice) node as a root (no parents).
      KTBNLearner< GUM_SCALAR >& addNoParentNode(std::string_view base, int slice) override;

      /// @brief Declare a single node (bracket notation, e.g. "X[2]" or "C") as a root.
      KTBNLearner< GUM_SCALAR >& addNoParentNode(std::string_view name) override;

      /// @brief Undo a previous addNoParentNode for a single (base, slice) node.
      KTBNLearner< GUM_SCALAR >& eraseNoParentNode(std::string_view base, int slice) override;

      /// @brief Undo addNoParentNode for a node given by bracket notation.
      KTBNLearner< GUM_SCALAR >& eraseNoParentNode(std::string_view name) override;

      /// @brief Declare a single (base, slice) node as a leaf (no children).
      KTBNLearner< GUM_SCALAR >& addNoChildrenNode(std::string_view base, int slice) override;

      /// @brief Declare a single node (bracket notation, e.g. "X[2]" or "C") as a leaf.
      KTBNLearner< GUM_SCALAR >& addNoChildrenNode(std::string_view name) override;

      /// @brief Undo a previous addNoChildrenNode for a single (base, slice) node.
      KTBNLearner< GUM_SCALAR >& eraseNoChildrenNode(std::string_view base, int slice) override;

      /// @brief Undo addNoChildrenNode for a node given by bracket notation.
      KTBNLearner< GUM_SCALAR >& eraseNoChildrenNode(std::string_view name) override;

      /// @brief Add a candidate edge for MIIC (only edges explicitly listed are explored).
      KTBNLearner< GUM_SCALAR >& addPossibleEdge(std::string_view tailBase,
                                                 int              tailSlice,
                                                 std::string_view headBase,
                                                 int              headSlice) override;

      /// @brief Add a candidate edge for MIIC using engine names (e.g. "X[1]", "C").
      KTBNLearner< GUM_SCALAR >& addPossibleEdge(std::string_view tail,
                                                 std::string_view head) override;

      /// @brief Undo a previous addPossibleEdge.
      KTBNLearner< GUM_SCALAR >& erasePossibleEdge(std::string_view tailBase,
                                                   int              tailSlice,
                                                   std::string_view headBase,
                                                   int              headSlice) override;

      /// @brief Undo a previous addPossibleEdge using engine names (e.g. "X[1]", "C").
      KTBNLearner< GUM_SCALAR >& erasePossibleEdge(std::string_view tail,
                                                   std::string_view head) override;

      /// @brief Allow or forbid arc additions during structure search.
      KTBNLearner< GUM_SCALAR >& allowArcAdditions(bool allow = true) override;

      /// @brief Allow or forbid arc deletions during structure search.
      KTBNLearner< GUM_SCALAR >& allowArcDeletions(bool allow = true) override;

      /// @brief Allow or forbid arc reversals during structure search.
      KTBNLearner< GUM_SCALAR >& allowArcReversals(bool allow = true) override;

      /// @brief Cap the number of parents of any single node.
      KTBNLearner< GUM_SCALAR >& setMaxIndegree(Size max_indegree) override;

      /// @}
      // #######################################################################
      /// @name Diagnostics
      // #######################################################################
      /// @{

      /// @brief Order @f$k@f$ of the k-TBN being learned.
      Size k() const;

      /// @brief Number of columns in each CSV, i.e. of base variables
      /// (temporal + atemporal).
      Size nbCols() const;

      /// @brief Number of time steps in each trajectory CSV (one entry per sample,
      /// in load order). This is the raw trajectory length; the transition table's
      /// sliding-window row count for trajectory @c i is @c nbRows()[i] - k + 1.
      std::vector< Size > nbRows() const;

      /// @brief True if the current structure-learning algorithm is constraint-based (e.g. MIIC).
      bool isConstraintBased() const;

      /// @brief True if the current structure-learning algorithm is score-based (e.g. BIC, AIC).
      bool isScoreBased() const;

      /// @brief Human-readable summary of the learner's current configuration.
      std::string toString() const;

      /// @brief Settings as a vector of (key, value, comment) tuples (mirrors BNLearner::state()).
      std::vector< std::tuple< std::string, std::string, std::string > > state() const;

      /// @brief Copy all score/algorithm/prior/constraint settings from another KTBNLearner
      /// (does not copy the database).
      void copyState(const KTBNLearner< GUM_SCALAR >& learner);

      /// @}
      // #######################################################################
      /// @name Database accessors
      // #######################################################################
      /// @{

      /// @brief Number of trajectory CSV files loaded (the constructor's
      /// @p nbSamples).
      Size nbSamples() const;
      
      /// @brief True if any internal database contains missing values.
      ///
      /// @warning Always @c false since _build_() performs complete-case
      /// selection: a row carrying a missing symbol is never inserted, so the
      /// databases hold no gap by construction. To learn whether the CSVs
      /// actually had any, read nbDroppedRows() instead.
      bool hasMissingValues() const;

      /**
       * @brief Number of rows dropped from the internal databases because they
       * carried a missing symbol.
       *
       * aGrUM's structure learning refuses a database holding any missing value
       * (gum::learning::IBNLearner::learnDag_), so _build_() performs
       * complete-case selection: a row is inserted only if every cell it needs is
       * observed. This mirrors the cross-k score, which likewise skips any
       * instance whose family is incomplete, so both layers agree on which data
       * counts. hasMissingValues() is consequently always false.
       *
       * The unit differs per table: a transition row is a whole width-k window,
       * so one gap costs up to @f$k@f$ of them; an initial or atemporal row is a
       * whole trajectory's contribution.
       *
       * @warning <b>Complete-case selection distorts any likelihood computed on
       * the result, and it does so unevenly across @f$k@f$.</b>
       *
       * A row is dropped as soon as @b one cell of the window it feeds is
       * missing, so a single gap costs up to @f$k@f$ transition rows. A larger
       * @f$k@f$ spans more rows per window and therefore loses proportionally
       * more of them.
       *
       * That matters because @f$\log_2 L@f$ is a @e sum of per-instance terms,
       * every one of which is @f$\leq 0@f$. Scoring fewer instances removes
       * negative terms, so it @b raises the total — a model does not fit better,
       * it is simply charged for less. Two consequences:
       *  - a @f$\log_2 L@f$ obtained with dropping is @b not comparable to one
       *    obtained on complete data, nor to one obtained at a different
       *    missing-value rate;
       *  - comparing @e orders this way (gum::learning::KTBNAdaptiveLearner)
       *    skews towards larger @f$k@f$, since the larger candidate both drops
       *    more instances and gains more from having dropped them. The BIC
       *    penalty does not offset it: @f$\log_2 N@f$ is taken once from the raw
       *    trajectory length and held fixed across candidates, so the penalty
       *    describes a sample size the likelihood no longer uses.
       *
       * The effect grows with the missing-value rate. Read bestK() and
       * scorePerCandidateK() with that in mind, and prefer complete trajectories
       * whenever the order itself is the question being asked.
       *
       * @note Independently of the above, complete-case selection is only
       * unbiased when values are missing completely at random (MCAR). If
       * missingness depends on the data, the retained rows are a biased sample
       * and the learned CPTs inherit that bias.
       */
      Size nbDroppedRows() const;

      /// @brief Whether _build_() drops the rows carrying a missing symbol.
      /// @see nbDroppedRows() for how much was dropped, and for the @b warning
      /// on the likelihood distortion this causes.
      bool isIgnoringMissingSymbols() const;

      /// @brief Base names (no slice suffix), one entry per base variable
      /// (temporal or atemporal), in the original CSV header order.
      std::vector< std::string > names() const;

      /// @brief Domain sizes of the base variables, in the same column order as names().
      std::vector< std::size_t > domainSizes() const;

      /// @brief Domain size of the base variable @p base (e.g. "X", "C"). Engine
      /// names (e.g. "X[1]") are also accepted.
      /// @note KTBNLearner is deliberately name-only: it exposes no NodeId-based
      /// accessor (no domainSize(NodeId), idFromName or nameFromId). The three
      /// internal learners live in independent NodeId spaces, so a bare NodeId
      /// would be ambiguous — always address variables by name here.
      Size domainSize(std::string_view base) const;

      /// @}

      private:
      /// learns the transition kernel (arcs arriving at slice k-1)
      std::unique_ptr< BNLearner< GUM_SCALAR > > _transitionLearner_;

      /// learns the initial slices 0..k-2
      std::unique_ptr< BNLearner< GUM_SCALAR > > _initialLearner_;

      /// learns the atemporal variables (arcs atemporal -> atemporal)
      std::unique_ptr< BNLearner< GUM_SCALAR> > _atemporalLearner_;

      /// prior k-TBN: the single source of truth for k, variable domains,
      /// temporal/atemporal classification and engine names. Everything else
      /// (base lists, column names) is derived from it on demand. Analogous to
      /// the prior BayesNet stored in BNLearner when a reference BN is given.
      /// whether _build_() drops incomplete rows rather than refusing the data
      bool _ignoreMissingSymbols_{false};

      KTBN< GUM_SCALAR > _prior_ktbn_;

      /// number of time steps (rows) in each trajectory CSV, in load order.
      /// rows _build_() dropped because they carried a missing symbol
      Size _nbDroppedRows_{0};

      /// Captured once by _build_() and exposed by nbRows(). This is the raw
      /// trajectory length, not the sliding-window row count of the transition
      /// table (which is _nbTimeSlices_[i] - k + 1 summed over trajectories).
      std::vector< Size > _nbTimeSlices_;

      /// counts of currently-active possible edges, split by kind: edges with at
      /// least one temporal endpoint, and edges between two atemporal variables.
      /// Maintained by add/erasePossibleEdge(). When a whitelist is active
      /// (temporal count > 0) but no atemporal->atemporal edge is whitelisted, the
      /// atemporal learner would otherwise stay unrestricted; learnKTBN() then
      /// suppresses it so the whitelist is honoured (no atemporal arc is produced).
      Size _nbTemporalPossibleEdges_  = 0;
      Size _nbAtemporalPossibleEdges_ = 0;

      /// @brief Apply @p f to each present internal learner (the atemporal one
      /// only when it exists). Factors out the fan-out shared by every score /
      /// algorithm / prior / correction / graph-change setter.
      template < class F >
      void _forEachLearner_(F&& f);

      /// @brief Apply @p f to the ONE internal learner that can learn the arc
      /// @p tail -> @p head, chosen by its @b head: a slice-(k-1) head belongs
      /// to the transition learner, an atemporal->atemporal arc to the
      /// atemporal learner, any other head (a past temporal slice) to the
      /// initial learner. The three cases are exclusive because _build_() forces
      /// every other node root in the learners that do not own it, so no arc is
      /// ever learnable in two of them.
      ///
      /// The bothAtemp guard matters: without it an "X[0] -> C" arc would be
      /// routed to the atemporal learner, whose table has no X[0] column, and
      /// surface as MissingVariableInDatabase. It falls to the initial learner
      /// instead, where both endpoints exist and the arc is a harmless no-op.
      ///
      /// Callers keep their own _checkArcTemporallyFeasible_ call, which is
      /// deliberately asymmetric -- forbidding an impossible arc is harmless
      /// while UN-forbidding it lifts an invariant, and forcing one must be
      /// refused while erasing a never-forced one is harmless -- so it cannot be
      /// folded in here without flattening that asymmetry.
      template < class F >
      void _forOwningLearner_(std::string_view tail, std::string_view head, F&& f);

      /// @brief Apply @p f(tailSlice, headSlice) to every causally-possible slice
      /// pair of an all-slices constraint between @p tailBase and @p headBase.
      /// Four shapes: atemporal->atemporal is one pair; an atemporal tail
      /// reaches every slice of the head; a temporal tail into an atemporal head
      /// is structurally impossible and yields none; two temporal bases give
      /// every pair with tailSlice <= headSlice, i.e. every lag.
      ///
      /// Validates that both bases exist -- which the two callers used not to do,
      /// letting an unknown name reach a BNLearner as an opaque
      /// MissingVariableInDatabase -- but deliberately NOT that they are
      /// temporal: an atemporal tail is one of the four shapes above.
      template < class F >
      void _forEachAllSlicesPair_(std::string_view tailBase,
                                  std::string_view headBase,
                                  F&&              f) const;

      // ----- construction (runs once, in the constructor) -----

      /// reads every trajectory, builds the three DatabaseTables (sliding window,
      /// initial-slice flattening, atemporal columns), constructs the BNLearners from them and
      /// applies the automatic temporal constraints (slice order, root past
      /// slices, temporal->atemporal forbids). Relies solely on _prior_ktbn_
      /// (already fully populated by the time it runs): the column order is read
      /// from the first trajectory's header and the atemporal columns are derived
      /// from _prior_ktbn_.atemporalVarNames(). Shared by both constructors.
      void _build_(std::string_view                  dirPath,
                   std::string_view                  csvBaseName,
                   Size                              nbSamples,
                   const std::vector< std::string >& missingSymbols);

      /// checks @c k \>= 2, then delegates the actual scan to the shared
      /// IKTBNLearner::_scanConstantColumns_() (also used by
      /// KTBNAdaptiveLearner, hence not duplicated here). The check happens
      /// first and here, not inside the shared scan: called once, in the
      /// member-initialiser list of the atemporal-inferring constructor,
      /// before _prior_ktbn_ exists — before that constructor's body runs the
      /// equivalent check in _buildPriorFromCSV_ — so without it here too, a
      /// bad @p k would only be caught after a wasted scan of every trajectory.
      static std::unordered_set< std::string >
          _inferAtemporalVars_(std::string_view                  dirPath,
                               std::string_view                  csvBaseName,
                               Size                              nbSamples,
                               Size                              k,
                               const std::vector< std::string >& missingSymbols);

      /// called in the member-initialiser list of the k-CSV constructor: opens
      /// the first trajectory CSV, uses a temporary BNLearner to type the base
      /// variables (induceTypes promotes numeric columns to range/integer), builds
      /// and returns a KTBN with the right temporal/atemporal classification.
      /// Must be static because it is called before the object exists.
      static KTBN< GUM_SCALAR >
          _buildPriorFromCSV_(std::string_view                         dirPath,
                              std::string_view                         csvBaseName,
                              Size                                     k,
                              const std::unordered_set< std::string >& atemporalVars,
                              const std::vector< std::string >&        missingSymbols,
                              bool                                     induceTypes);

      /// called in the member-initialiser list of the BN constructor: builds
      /// and returns a KTBN whose variables are copied from @p bn (one node per
      /// base variable, right types/domains) and classified as temporal/atemporal
      /// according to @p atemporalVars.
      /// Must be static because it is called before the object exists.
      static KTBN< GUM_SCALAR >
          _buildPriorFromBN_(Size                                     k,
                             const BayesNet< GUM_SCALAR >&            bn,
                             const std::unordered_set< std::string >& atemporalVars);

      // ----- name encoding -----

      /// atemporal base names for IKTBNLearner's shared _encode_/_determineNode_;
      /// read straight from the prior k-TBN (the single source of truth).
      const std::unordered_set< std::string >& _atemporalVarNames_() const override;

      /// whether @p base is one of this learner's variables; read straight from
      /// the prior k-TBN, like _atemporalVarNames_() above.
      bool _isKnownBase_(std::string_view base) const override;

      /// bring the shared helpers of the dependent base into unqualified scope,
      /// so member functions can call _encode_(...) / _determineNode_(...) plainly.
      using IKTBNLearner< GUM_SCALAR >::_encode_;
      using IKTBNLearner< GUM_SCALAR >::_determineNode_;
      using IKTBNLearner< GUM_SCALAR >::_checkArcTemporallyFeasible_;
      using IKTBNLearner< GUM_SCALAR >::_checkBaseIsTemporal_;

      /// glues the three parameter-learned BNs into a single k-TBN
      KTBN< GUM_SCALAR > _assemble_(const BayesNet< GUM_SCALAR >& transitionBN,
                                    const BayesNet< GUM_SCALAR >& initialBN,
                                    const BayesNet< GUM_SCALAR >& atemporalBN) const;

      // forbidden copies / moves
      KTBNLearner(const KTBNLearner< GUM_SCALAR >&)                       = delete;
      KTBNLearner(KTBNLearner< GUM_SCALAR >&&)                            = delete;
      KTBNLearner< GUM_SCALAR >& operator=(const KTBNLearner< GUM_SCALAR >&) = delete;
      KTBNLearner< GUM_SCALAR >& operator=(KTBNLearner< GUM_SCALAR >&&)      = delete;
    };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class KTBNLearner< double >;
#endif

  } /* namespace learning */
} /* namespace gum */

#include <agrum/KTBN/learning/KTBNLearner_tpl.h>

#endif /* GUM_LEARNING_KTBN_LEARNER_H */
