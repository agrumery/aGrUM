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
 * @brief Exact inference for k-order dynamic Bayesian networks: Murphy's
 * interface algorithm, extended to interventions.
 *
 * KTBNInference answers queries on a gum::KTBN under any mix of
 * @b observations @f$V[t]=v@f$ (soft or hard) and hard @b interventions
 * @f$do(V[t]=v)@f$, returning @f$P(\text{target}[t] \mid \text{obs},
 * do(\cdot))@f$ for every declared target at every slice.
 *
 * @par The algorithm
 * Murphy's interface algorithm (Dynamic Bayesian Networks: Representation,
 * Inference and Learning, 2002, §3.4), generalised from order-1 DBNs to order
 * k. The @e forward @e interface @f$I_t@f$ is the set of node occurrences at
 * slices @f$\leq t@f$ still coupled to the future; it d-separates past from
 * future, so a distribution over @f$I_t@f$ is all that crosses one slice
 * boundary. At order k, @f$I_t@f$ spans the last @f$k-1@f$ slices -- exactly
 * what the KTBN's ring of k variable objects holds.
 *
 * Inference runs on a chain of @e windows: window t covers
 * @f$H_t = I_{t-1} \cup V_t@f$, compiled once (at construction) into a
 * junction tree by moralising its families, forcing @f$I_{t-1}@f$ and
 * @f$I_t@f$ each into a clique, and triangulating. Time-homogeneity means
 * every window from @f$t=k-1@f$ on has the same shape, so @b one junction
 * tree serves the whole repeating part, re-entered each step with fresh
 * potentials; only the @f$k-1@f$ initial windows get their own trees. A
 * slice's variables are ring slots @f$(t-\delta) \bmod k@f$, so advancing is
 * a relabelling, never an allocation -- the model is never unrolled.
 *
 * Within a window, Shafer-Shenoy message passing (collect then distribute, no
 * division) combines potentials; neighbouring windows exchange @f$m_t@f$
 * forward and @f$r_t@f$ backward over their shared interface. Cost per slice
 * is @f$O(K^{w})@f$, w the fixed window's triangulation width -- independent
 * of the horizon.
 *
 * @par Observations vs. interventions
 * An observation is conditioning: it revises the whole network, ancestors
 * included, so information flows @b both ways. An intervention
 * @f$do(V[t]=v)@f$ is surgery: V[t] is cut from its causes and its CPT
 * replaced by a point mass, so the effect only reaches descendants. Both may
 * be combined, and a node may carry both.
 *
 * @par Cost
 * With no observations the backward messages are provably uniform, so
 * makeInference() runs a single forward sweep holding one window at a time:
 * memory is @b independent of the horizon. Once any observation exists, exact
 * smoothing needs the future, so the engine also runs backward, retaining one
 * interface message per slice -- @f$O(T \cdot |I|)@f$, never whole windows.
 *
 * @author Seth AGUILA & Anis KHACEF
 */
#ifndef GUM_KTBN_INFERENCE_H
#define GUM_KTBN_INFERENCE_H

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/graphs/algorithms/triangulations/defaultTriangulation.h>
#include <agrum/base/graphs/cliqueGraph.h>
#include <agrum/base/graphs/undiGraph.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/discreteVariable.h>
#include <agrum/KTBN/KTBN.h>

namespace gum {

  /**
   * @class KTBNInference
   * @headerfile KTBNInference.h <agrum/KTBN/inference/KTBNInference.h>
   * @brief Exact inference on a gum::KTBN with observations and interventions,
   * by the interface algorithm.
   * @ingroup bn_group
   *
   * @par Usage
   * @code
   *   KTBNInference<double> ie(&ktbn);
   *   ie.addObservation("Y", 3, 1);            // observe Y[3]=1
   *   ie.addIntervention("X", 2, 1);        // do(X[2]=1)
   *   ie.addTarget("X");                    // want the whole P(X[t] | ...) series
   *   ie.makeInference(10);                 // compute slices 0..9
   *   const Tensor<double>&              p3 = ie.posterior("X", 3);
   *   const std::vector<Tensor<double>>& px = ie.posteriors("X");
   *   double logpe = ie.logObservationProbability();
   * @endcode
   *
   * @par Targets (the query)
   * A @e target is a @b base variable (temporal or atemporal) whose marginals
   * you want. makeInference(T) computes, for every targeted temporal base, its
   * marginal at @b every slice @f$0..T-1@f$ (a single entry at ATEMPORAL for
   * an atemporal base). With none declared, @b every base is a target
   * (mirrors gum::MarginalTargetedInference's default-all-targets mode).
   * Declaring targets restricts the roll to the requisite subnetwork -- the
   * targets, the observed nodes, and their ancestors; everything else is
   * barren. posterior()/posteriors() throw for anything not a target.
   *
   * @warning The KTBN is referenced, not copied (aGrUM's rule); it must outlive
   * the inference engine.
   */
  template < GUM_Numeric GUM_SCALAR >
  class KTBNInference {
    public:
    /// Convenience alias for the atemporal-slice sentinel.
    static constexpr int ATEMPORAL = KTBN< GUM_SCALAR >::ATEMPORAL;

    // ===========================================================================
    /// @name Constructors / Destructor
    // ===========================================================================
    /// @{

    /**
     * @brief Constructor.
     * @param ktbn The k-DBN to reason about (referenced, not copied).
     * @throw InvalidArgument if @p ktbn is null.
     */
    explicit KTBNInference(const KTBN< GUM_SCALAR >* ktbn);

    /// @brief Destructor.
    ~KTBNInference() = default;

    /// Copy is disabled (owns per-run variable descriptors and cached tensors).
    KTBNInference(const KTBNInference< GUM_SCALAR >&)                          = delete;
    KTBNInference< GUM_SCALAR >& operator=(const KTBNInference< GUM_SCALAR >&) = delete;

    /// @}

    /// @brief A node designated either by its engine name (@c "X[2]", @c "C")
    /// or by its (base, slice) identity.
    using NodeKey = std::variant< std::string, std::pair< std::string, int > >;

    // ===========================================================================
    /// @name Interventions
    // ===========================================================================
    /// @{

    /**
     * @brief Records a hard intervention @f$do(base[slice]=value)@f$.
     *
     * Recorded only; applied at makeInference(). Re-intervening overwrites.
     * @p value is a modality index or label (see gum::KTBNModality).
     *
     * @code
     *   ie.addIntervention("X", 5, 0);        // by index
     *   ie.addIntervention("X", 5, "high");   // by label
     * @endcode
     *
     * @throw NotFound if @p base is unknown.
     * @throw InvalidArgument if the slice is invalid for the variable kind.
     * @throw OutOfBounds / NotFound if @p value is out of the variable's domain
     * or is not one of its modality labels.
     */
    void addIntervention(std::string_view base, int slice, const KTBNModality& value);
    /// @brief Same, using an engine name (@c "X[2]", @c "C", …).
    void addIntervention(std::string_view node_name, const KTBNModality& value);

    /**
     * @brief Records several interventions in one call.
     *
     * Each entry is a (node, value) pair: node keyed by engine name @b or by
     * @c (base, slice), value an index @b or a label -- freely mixed.
     *
     * **All-or-nothing**: every entry is validated before any is recorded. A
     * node listed twice keeps its last value.
     *
     * @code
     *   ie.addIntervention({{"X[5]", 0}, {std::pair{"Z", 2}, "high"}});
     * @endcode
     *
     * @warning A @c (base, slice) key needs an explicit @c std::pair{...}: a
     * bare @c {"Z", 2} cannot implicitly construct the variant (same rule as
     * gum::KTBN::fillCPT()).
     *
     * @throw NotFound / InvalidArgument / OutOfBounds -- same as the
     * single-node form, raised before anything is recorded.
     */
    void addIntervention(const std::vector< std::pair< NodeKey, KTBNModality > >& interventions);

    /// @brief Removes a recorded intervention (silent no-op if absent).
    void eraseIntervention(std::string_view base, int slice);
    /// @brief Same, using an engine name (@c "X[2]", @c "C", …).
    void eraseIntervention(std::string_view node_name);
    /// @brief Removes all recorded interventions.
    void clearInterventions();

    /// @return @c true if @p base at @p slice carries an intervention.
    bool hasIntervention(std::string_view base, int slice) const;
    /// @brief Same, using an engine name (@c "X[2]", @c "C", …).
    bool hasIntervention(std::string_view node_name) const;

    /// @}
    // ===========================================================================
    /// @name Observations
    // ===========================================================================
    /// @{

    /**
     * @brief Records a @b hard observation @f$base[slice]=value@f$.
     *
     * Unlike an intervention, an observation is conditioning: it revises the
     * whole network, ancestors included. Recorded only; applied at
     * makeInference(). Re-observing overwrites. @p value is a modality index
     * or label (see gum::KTBNModality).
     *
     * @code
     *   ie.addObservation("X", 5, 0);        // by index
     *   ie.addObservation("X", 5, "high");   // by label
     * @endcode
     *
     * @throw NotFound if @p base is unknown.
     * @throw InvalidArgument if the slice is invalid for the variable kind.
     * @throw OutOfBounds / NotFound if @p value is not a legal modality.
     */
    void addObservation(std::string_view base, int slice, const KTBNModality& value);
    /// @brief Same, using an engine name (@c "X[2]", @c "C", …).
    void addObservation(std::string_view node_name, const KTBNModality& value);

    /**
     * @brief Records a @b soft (likelihood) observation on @f$base[slice]@f$.
     *
     * @p likelihood[i] is the likelihood of the observation given that the node
     * takes its i-th modality; it need not sum to 1, but must be non-negative
     * and not all-zero. A one-hot vector is equivalent to a hard observation.
     *
     * @throw NotFound / InvalidArgument as above.
     * @throw InvalidArgument if @p likelihood has the wrong length, holds a
     * negative entry, or is all zeros.
     */
    void addObservation(std::string_view                  base,
                     int                               slice,
                     const std::vector< GUM_SCALAR >& likelihood);
    /// @brief Same, using an engine name (@c "X[2]", @c "C", …).
    void addObservation(std::string_view node_name, const std::vector< GUM_SCALAR >& likelihood);

    /**
     * @brief Records several observations in one call, all-or-nothing.
     * Same keying rules (and the same @c std::pair{...} warning) as the batch
     * addIntervention().
     */
    void addObservations(const std::vector< std::pair< NodeKey, KTBNModality > >& observations);

    /// @brief Removes a recorded observation (silent no-op if absent).
    void eraseObservation(std::string_view base, int slice);
    /// @brief Same, using an engine name (@c "X[2]", @c "C", …).
    void eraseObservation(std::string_view node_name);
    /// @brief Removes all recorded observations.
    void clearObservation();

    /// @return @c true if @p base at @p slice carries an observation.
    bool hasObservation(std::string_view base, int slice) const;
    /// @brief Same, using an engine name (@c "X[2]", @c "C", …).
    bool hasObservation(std::string_view node_name) const;
    /// @return @c true if @b any observation is recorded. When false,
    /// makeInference() needs no backward pass and runs in horizon-independent
    /// memory.
    bool hasObservation() const;

    /// @}
    // ===========================================================================
    /// @name Targets
    // ===========================================================================
    /// @{

    /**
     * @brief Declares a target: a @b base variable whose marginals we want.
     *
     * A temporal base yields one marginal per slice @f$0..T-1@f$ at
     * makeInference(T); an atemporal base yields its single marginal. The first
     * declared target switches the engine out of default-all-targets mode.
     * Recorded only; applied at makeInference().
     * @throw NotFound if @p base is neither a temporal process nor an atemporal
     * variable of the k-DBN.
     */
    void addTarget(std::string_view base);

    /// @brief Removes a target; when the last one is removed, default-all-targets
    /// mode is restored.
    void eraseTarget(std::string_view base);
    /// @brief Removes all targets (restores default-all-targets mode).
    void clearTargets();

    /// @return @c true if @p base is a target of the next inference.
    bool isTarget(std::string_view base) const;

    /// @return @c true iff at least one explicit target has been declared; when
    /// @c false, every base is a target. Mirrors
    /// gum::MarginalTargetedInference::isInTargetMode().
    bool isInTargetMode() const;

    /// @}
    // ===========================================================================
    /// @name Inference
    // ===========================================================================
    /// @{

    /**
     * @brief Runs the interface algorithm over @p nbTimeSlices slices
     * (@f$0..nbTimeSlices-1@f$) and caches, for every targeted base, its
     * marginal at @b every slice.
     *
     * A forward sweep passes @f$m_t@f$ window to window; if any observation is
     * recorded, a backward sweep passes @f$r_t@f$ the other way and the two
     * combine into exact smoothed posteriors. With no observation the backward
     * pass is skipped and only one window is ever live.
     *
     * Window junction trees are compiled once, at construction, and reused by
     * every call regardless of horizon or target set; only the requisite
     * subnetwork is recomputed per run. Observations/interventions at slices
     * @f$\geq@f$ @p nbTimeSlices fall outside the roll and are ignored.
     *
     * Idempotent. Called lazily by posterior()/posteriors() (last horizon) when
     * out of date. @p nbTimeSlices becomes the stored horizon.
     * @throw InvalidArgument if @p nbTimeSlices is 0.
     * @throw FatalError if the observations have probability 0 under the model.
     */
    void makeInference(Size nbTimeSlices);

    /**
     * @brief Returns @f$P(base[slice] \mid \text{obs}, do(\cdot))@f$.
     *
     * Use @c ATEMPORAL as @p slice for an atemporal base. Lazily (re)runs
     * makeInference() with the last horizon if out of date. The returned
     * reference is owned by the engine and is invalidated by the next
     * makeInference(); copy it to keep it.
     * @throw OperationNotAllowed if makeInference() has never been run.
     * @throw UndefinedElement if @p base is not a target.
     * @throw OutOfBounds if @p slice was not computed (@f$\geq@f$ the horizon).
     */
    const Tensor< GUM_SCALAR >& posterior(std::string_view base, int slice);
    /// @brief Same, using an engine name (@c "X[2]", @c "C", …).
    const Tensor< GUM_SCALAR >& posterior(std::string_view node_name);

    /**
     * @brief The whole marginal time-series of a targeted base: @c tensors[t] is
     * @f$P(base[t] \mid \cdot)@f$ for @f$t = 0..T-1@f$ (a single-element vector,
     * holding the atemporal marginal, for an atemporal base).
     *
     * Zero-copy: returns a reference to the engine-owned vector, invalidated by
     * the next makeInference(). Lazily (re)runs makeInference() if out of date.
     * @throw OperationNotAllowed if makeInference() has never been run.
     * @throw UndefinedElement if @p base is not a target.
     */
    const std::vector< Tensor< GUM_SCALAR > >& posteriors(std::string_view base);

    /// @brief @f$\log P(\text{obs} \mid do(\cdot))@f$ for the last run: the
    /// likelihood of the observations under the (possibly mutilated) model.
    /// 0 when nothing is observed. Lazily (re)runs makeInference() if out of date.
    /// @throw OperationNotAllowed if makeInference() has never been run.
    GUM_SCALAR logObservationProbability();

    /// @brief @f$P(\text{obs} \mid do(\cdot))@f$, i.e. exp of
    /// logObservationProbability(). Underflows to 0 on long horizons; prefer the
    /// log form.
    GUM_SCALAR observationProbability();

    /// @}
    // ===========================================================================
    /// @name Various
    // ===========================================================================
    /// @{

    /// @return The k-DBN reasoned about.
    const KTBN< GUM_SCALAR >& ktbn() const;

    /// @return A human-readable summary of the engine's state.
    std::string toString() const;

    /// @brief The junction tree of the repeating window -- the one compiled from
    /// the k-slice template and re-entered at every step from slice @f$k-1@f$ on.
    /// Introspection only.
    const JunctionTree& windowJunctionTree() const;

    /// @brief Size of the forward interface of the repeating window: how many
    /// node occurrences have to cross each slice boundary. Introspection only.
    Size interfaceSize() const;

    /// @}

    private:
    /// A cached marginal time-series for one base: owned variable descriptors
    /// paired with their marginals, indexed by slice (single entry for an
    /// atemporal base). Descriptors are owned so tensors get a stable
    /// per-slice name rather than the reused ring-slot name they came from.
    struct _Series_ {
      std::vector< std::unique_ptr< DiscreteVariable > > vars;
      std::vector< Tensor< GUM_SCALAR > >                tensors;
    };

    /// One node of a window template: a base (index into _baseNames_) at a lag
    /// behind the window's current slice. @c lag == ATEMPORAL marks an
    /// atemporal base, which sits in every interface and never ages.
    struct _Slot_ {
      int base;
      int lag;
      bool operator==(const _Slot_& o) const;
    };

    /// A compiled window: the junction tree of @f$H_t = I_{t-1} \cup V_t@f$,
    /// rooted at the clique holding @f$I_t@f$, plus everything needed to fill
    /// and message-pass it. Built once; windows 0..k-2 are the initial ones,
    /// window k-1 is the repeating one, re-entered from slice k-1 on.
    struct _Window_ {
      /// template graph NodeId -> slot it stands for
      std::vector< _Slot_ > slotOfNode;
      /// the junction tree over those nodes
      JunctionTree jt;
      /// clique containing the whole outgoing interface I_t
      NodeId rootC{0};
      /// clique containing the whole incoming interface I_{t-1}
      NodeId rootD{0};
      /// cliques in BFS order from rootC, and each one's parent in that rooting
      std::vector< NodeId >                bfs;
      std::unordered_map< NodeId, NodeId > parentOf;
      /// clique -> base indices whose family factor is multiplied in there
      std::unordered_map< NodeId, std::vector< int > > factorsOf;
      /// base index -> clique holding that base's own slot (lag 0 /
      /// atemporal), for reading its posterior and placing its observation
      std::unordered_map< int, NodeId > selfClique;
      /// the two interfaces, as slot lists
      std::vector< _Slot_ > Iprev, Icur;
    };

    /// The k-DBN (referenced, not owned).
    const KTBN< GUM_SCALAR >* _ktbn_;

    /// The order k, cached as int for slice arithmetic.
    int _k_;

    /// Recorded interventions, keyed by engine name -> forced value.
    std::map< std::string, Idx > _interventions_;

    /// Recorded observations, keyed by engine name -> likelihood vector (one-hot
    /// for a hard observation).
    std::map< std::string, std::vector< GUM_SCALAR > > _observations_;

    /// Recorded targets (base names). Empty <=> default-all-targets mode.
    std::set< std::string > _targets_;

    /// Whether at least one explicit target has been declared.
    bool _targeted_mode_{false};

    /// Horizon (nbTimeSlices) of the last/next run; 0 <=> makeInference never run.
    Size _horizon_{0};

    /// Whether the cached posteriors are up to date.
    bool _done_{false};

    /// log P(observation | do) of the last run.
    GUM_SCALAR _logObservation_{0};

    /// Cached marginal series of the last run, keyed by base name.
    std::unordered_map< std::string, _Series_ > _posteriors_;

    /// Temporal / atemporal base names in a deterministic (sorted) order, cached
    /// once at construction (the KTBN's own sets are unordered).
    std::vector< std::string > _temporalSorted_;
    std::vector< std::string > _atemporalSorted_;

    /// All bases: temporal first (indices 0.._nbTemporal_-1), then atemporal.
    /// Window slots index into this.
    std::vector< std::string > _baseNames_;
    std::size_t                _nbTemporal_{0};
    /// name -> index into _baseNames_
    std::unordered_map< std::string, int > _baseIdx_;

    /// _maxLag_[i]: largest lag at which the transition kernel still consumes
    /// temporal base i -- how long an occurrence must stay in the interface,
    /// which is what makes |I| finite and the window template time-invariant.
    std::vector< int > _maxLag_;

    /// The compiled windows: index t for t <= k-2 (initial), index k-1 for the
    /// repeating window, reused by every slice from k-1 on. Built once.
    std::vector< _Window_ > _windows_;

    /// Bases actually folded by the current run: the targets, the observed nodes
    /// and all their ancestors. Rebuilt per makeInference() from the current
    /// target/observation sets; anything outside is barren and cannot move an answer.
    std::vector< bool > _requisite_;

    /// @brief Memoized clique potentials for the slices that carry no temporal
    /// evidence, indexed by _psiKey_(t). Sized 2k: the k initial slices have
    /// their own windows and their own CPTs, the repeating one contributes k
    /// phases. O(k) whatever the horizon -- unlike keeping one per slice, which
    /// would make smoothing grow linearly in T.
    mutable std::vector< std::unordered_map< NodeId, Tensor< GUM_SCALAR > > > _psiCache_;
    mutable std::vector< bool >                                               _psiCached_;

    /// @brief Slices carrying a temporal @b observation. Their potentials are the
    /// periodic ones times that slice's likelihood, so they are served by copying
    /// the cached base and multiplying the evidence in -- cheaper than a rebuild,
    /// which would redo the unit fill and every CPT product.
    mutable std::unordered_set< int > _observationSlices_;

    /// @brief Slices carrying a temporal @b intervention. These need a full
    /// rebuild: do(X=x) @e replaces the node's CPT, which the cached base has
    /// already multiplied in, so the base is unusable rather than merely
    /// incomplete. Atemporal evidence appears in neither set -- it applies at
    /// every slice alike and so belongs to the periodic structure.
    mutable std::unordered_set< int > _interventionSlices_;

    /// @brief Potentials of an evidence-carrying slice, rebuilt on each visit.
    mutable std::unordered_map< NodeId, Tensor< GUM_SCALAR > > _psiScratch_;

    /// @brief Cache slot for slice @p t: the initial slices keep their own,
    /// the repeating window contributes one per phase.
    Size _psiKey_(int t) const;

    /// @brief Memoized transition kernels, keyed by (process, @c t % k).
    ///
    /// _buildKernel_ reads @p t only through <tt>t % k</tt> (directly for the
    /// child, as <tt>(t - lag) % k</tt> for each parent -- and @c t >= k with
    /// @c lag <= k-1 there, so the subtraction never goes negative). The kernel
    /// is therefore periodic in @c t with period @c k: at most @c k distinct
    /// tensors per process, whatever the horizon. Without this, a run rebuilds
    /// one per slice per process -- and twice per slice once smoothing is on.
    ///
    /// Cleared at the top of every makeInference(): cpt() hands out a const
    /// reference whose *content* is mutable, so a caller may edit CPT values
    /// between runs. Nothing can change during a run.
    mutable std::map< std::pair< std::string, int >, Tensor< GUM_SCALAR > > _kernelCache_;

    /// @name Structural helpers
    /// @{

    /// @return whether @p base is a temporal process of the k-DBN.
    bool _isTemporal_(const std::string& base) const;
    /// @return whether @p base is an atemporal variable of the k-DBN.
    bool _isAtemporal_(const std::string& base) const;

    /// @brief Encodes (base, slice) -> engine name (@c base[slice] or bare base).
    static std::string _encode_(const std::string& base, int slice);

    /// @brief Cache-aware classification of an engine name -> (base, slice): a
    /// name registered as atemporal (incl. bracket-shaped) maps to ATEMPORAL,
    /// every other name is parsed syntactically. Mirrors KTBN::_determineNode_.
    std::pair< std::string, int > _determineNode_(const std::string& name) const;

    /// @brief Validates that (base, slice) denotes a legal node (future slices ok).
    void _validateNode_(const std::string& base, int slice) const;

    /// @brief A representative template variable of @p base for domain/cloning.
    const DiscreteVariable& _templateVar_(const std::string& base, int slice) const;

    /// @brief The variable a window slot stands for at absolute slice @p t:
    /// ring slot @f$(t-\text{lag}) \bmod k@f$ for a temporal base, the
    /// atemporal object otherwise. This is where "advance one slice" happens
    /// -- a relabelling, not an allocation.
    const DiscreteVariable* _varOfSlot_(const _Slot_& s, int t) const;

    /// @brief Transition-kernel tensor of process @p p at slice @p t
    /// (@f$t \geq k@f$): the template kernel remapped onto the k-DBN's own
    /// per-slice objects, reused via @c slice % k (no allocation).
    ///
    /// Memoized in _kernelCache_: the result depends on @p t only through
    /// <tt>t % k</tt>, so at most @f$k@f$ tensors exist per process however long
    /// the horizon. Returns a reference into the cache -- valid until the next
    /// makeInference(), which clears it.
    const Tensor< GUM_SCALAR >& _buildKernel_(const std::string& p, int t) const;

    /// @brief Parents of @p base at a window whose current slice is @p t, as
    /// slots (lag = t - parentSlice). Uses the initial CPT structure for
    /// @f$t \leq k-2@f$, the transition kernel from @f$k-1@f$ on -- which is
    /// why the repeating window is compiled from slice k-1's families.
    std::vector< _Slot_ > _familySlots_(int baseIdx, int t) const;

    /// @brief The forward interface after slice @p t, as slots relative to
    /// t: every requisite occurrence at a slice @f$\leq t@f$ still coupled to
    /// the future, plus every requisite atemporal base. From @f$t=k-2@f$ on
    /// this is the steady @f$\{(p,\delta): \delta < maxLag(p)\}@f$ -- why one
    /// repeating window suffices.
    std::vector< _Slot_ > _interfaceAfter_(int t) const;

    /// @brief The last slice at which occurrence @p base[@p s] is still consumed
    /// (-1 if never), over both the initial families and the transition kernel.
    int _lastConsumerSlice_(int baseIdx, int s) const;

    /// @brief Compiles the k window junction trees, once, from the
    /// constructor: moralise each window's families, force @f$I_{t-1}@f$ and
    /// @f$I_t@f$ into cliques, triangulate, root at @f$C_t@f$, and assign
    /// every family factor to a clique that covers it.
    void _buildWindows_();

    /// @brief Compiles one window over the given slot set / interfaces.
    _Window_ _compileWindow_(const std::vector< _Slot_ >& Iprev,
                             const std::vector< _Slot_ >& Icur,
                             int                          t,
                             bool                         withAtemporalFamilies) const;

    /// @brief Marks the requisite bases of the current run (targets, observed
    /// bases and all their ancestors) into _requisite_.
    void _markRequisite_();

    /// @}
    /// @name Runtime
    /// @{

    /// @brief The window for absolute slice @p t: its own while t is inside
    /// the initial block, the repeating one (index k) from then on.
    const _Window_& _windowAt_(int t) const;

    /// @brief Clique potentials of the window at slice @p t: every requisite
    /// family's CPT (or, under an intervention, a point mass severing it from
    /// its causes), times every observation's likelihood.
    /// @param psi out: clique -> its potential (absent clique == unit potential).
    /// @brief Clique potentials for slice @p t, memoized where they repeat.
    ///
    /// psi(t) reads @p t in exactly four places: _windowAt_ (fixed at
    /// _windows_[k] once t >= k), _varOfSlot_ ((t - lag) % k), _buildKernel_
    /// (t % k) and the _encode_(base, t) lookups into _observations_ /
    /// _interventions_. The first three are periodic in <tt>t % k</tt>, so on a
    /// slice carrying no @b temporal evidence the whole map is too: one entry
    /// serves every slice of that phase.
    ///
    /// Atemporal evidence needs no special case -- it applies identically at
    /// every slice, so it is part of the periodic structure.
    ///
    /// Returns a reference into the cache (or into a scratch map for a slice
    /// that does carry evidence). Valid until the next call, and until the next
    /// makeInference(), which clears the cache. Callers only read it:
    /// _propagate_ and _belief_ both take psi by const reference.
    const std::unordered_map< NodeId, Tensor< GUM_SCALAR > >&
        _windowPotentials_(const _Window_& w, int t) const;

    /// @brief Multiplies slice @p t's temporal observation likelihoods into an
    /// already-built base. Atemporal ones are skipped: the base holds them.
    void _applyTemporalObservations_(const _Window_&                                    w,
                                     int                                                t,
                                     std::unordered_map< NodeId, Tensor< GUM_SCALAR > >& psi) const;

    /// @param withTemporalEvidence false builds the @b base: the periodic part
    /// only, i.e. CPTs/kernels plus atemporal evidence, with slice @p t's own
    /// observations and interventions left out. That is what _psiCache_ stores.
    void _fillWindow_(const _Window_&                                    w,
                      int                                                t,
                      std::unordered_map< NodeId, Tensor< GUM_SCALAR > >& psi,
                      bool withTemporalEvidence = true) const;

    /// @brief Shafer-Shenoy pass over a filled window. @p inPrev / @p inNext
    /// are the interface messages arriving at rootD / rootC (null when
    /// absent); division-free, so deterministic potentials need no special
    /// casing.
    /// @param msgs out: (from,to) clique message. Collect-only when
    /// @p distribute is false -- enough for the forward message alone.
    void _propagate_(const _Window_&                                          w,
                     int                                                      t,
                     const std::unordered_map< NodeId, Tensor< GUM_SCALAR > >& psi,
                     const Tensor< GUM_SCALAR >*                              inPrev,
                     const Tensor< GUM_SCALAR >*                              inNext,
                     bool                                                     distribute,
                     std::map< std::pair< NodeId, NodeId >, Tensor< GUM_SCALAR > >& msgs) const;

    /// @brief The belief of clique @p c: its potential times every message
    /// reaching it, interface messages included.
    Tensor< GUM_SCALAR > _belief_(
        const _Window_&                                                     w,
        const std::unordered_map< NodeId, Tensor< GUM_SCALAR > >&           psi,
        const std::map< std::pair< NodeId, NodeId >, Tensor< GUM_SCALAR > >& msgs,
        const Tensor< GUM_SCALAR >*                                         inPrev,
        const Tensor< GUM_SCALAR >*                                         inNext,
        NodeId                                                              c,
        NodeId                                                              skipNeighbour) const;

    /// @brief Snapshots @p marginal onto an owned, stably-named descriptor and
    /// appends it to that base's series (index == slice for a temporal base).
    /// Positional fillWith, not name-matched: marginal's axis is a shared
    /// ring object, reused across residue-k slices, whose name generally
    /// isn't @c base[slice].
    void _snapshot_(const std::string& base, int slice, const Tensor< GUM_SCALAR >& marginal);

    /// @brief The cached series of a targeted @p base, running makeInference()
    /// lazily (with the last horizon) if out of date. Shared by both accessors.
    const _Series_& _series_(const std::string& base);

    /// @}
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class KTBNInference< double >;
#endif

}   // namespace gum

#include <agrum/KTBN/inference/KTBNInference_tpl.h>

#endif /* GUM_KTBN_INFERENCE_H */
