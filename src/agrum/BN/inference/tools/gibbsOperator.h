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
 * @brief This file contains Gibbs sampling (for BNs) class definitions
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 */


#ifndef GUM_GIBBS_OPERATOR_H
#define GUM_GIBBS_OPERATOR_H

#include <array>
#include <vector>

#include <agrum/BN/IBayesNet.h>

namespace gum {

  /**
   * @class GibbsOperator
   * @headerfile gibbsOperator.h <agrum/BN/inference/gibbsOperator.h>
   * @brief class containing all variables and methods required for Gibbssampling
   *
   * A Gibbs move draws one variable given the others. A node whose CPT is deterministic (each row
   * gives the probability 1 to a value: the node is a function of its parents) would then never
   * change: a move of the node gives back the value of its parents, a move of a parent only accepts
   * the values that keep it. Hence such a node (if not observed) is never drawn: its value is
   * computed from its parents, and the move of a variable X draws X jointly with the deterministic
   * nodes that depend on it (through deterministic nodes only), whose values follow X.
   *
   * Limit: a CPT that is only partly deterministic (some null parameters) may still prevent the
   * chain from reaching some configurations.
   *
   * @ingroup bn_approximation
   *
   */

  template < GUM_Numeric GUM_SCALAR >
  class GibbsOperator {
    public:
    /**
     *  constructor
     */
    GibbsOperator(const IBayesNet< GUM_SCALAR >& BN,
                  const NodeProperty< Idx >*     hardEv,
                  Size                           nbr      = 1,
                  bool                           atRandom = false);

    /**
     * @brief Constructor whose Gibbs parameters are read from gum::Configuration.
     *
     * The key `percentDrawnSample` of the section gives the percentage (0 to 100) of the variables
     * drawn between two samples and the key `drawnAtRandom` tells whether they are chosen at random
     * or in turn.
     *
     * @param BN The Bayesian network to sample.
     * @param hardEv The hard evidence (may be nullptr).
     * @param section The name of the section of the configuration.
     */
    GibbsOperator(const IBayesNet< GUM_SCALAR >& BN,
                  const NodeProperty< Idx >*     hardEv,
                  const std::string&             section);

    /**
     * Destructor.
     */
    virtual ~GibbsOperator();

    /** Getters and setters*/
    Size nbrDrawnVar() const;

    void setNbrDrawnVar(Size nbr);

    bool isDrawnAtRandom() const;

    void setDrawnAtRandom(bool atRandom);

    /// draws a first sample with a positive probability (starting point of a Gibbs chain)
    /**
     * From a sample with a null probability, every Gibbs move may have a null mass: the chain
     * would never leave it. Hence the search for a sample with a positive probability, which is a
     * constraint satisfaction problem (NP-hard in general). The search is heuristic:
     *
     * 0. the domains are pruned by generalized arc consistency on the null probabilities of the
     *    CPTs, starting from the hard evidence. An empty domain proves that the evidence is
     *    incompatible with the model;
     * 1. up to _nbrRejectionTries_ forward samples are drawn in the pruned domains;
     * 2. then, Gibbs moves are performed in a smoothed model (a null probability is replaced by
     *    epsilon times the smallest positive probability of its CPT), with growing smoothing levels
     *    (_smoothingLevels_, _nbrSweepsBySmoothingLevel_ sweeps each) and _nbrSmoothingRestarts_
     *    restarts from new forward samples.
     *
     * Limits: arc consistency is not complete (it may not detect an incompatibility involving
     * several CPTs together, e.g. a parity constraint over a chain of XOR), and the search is
     * stochastic and bounded. Hence, when the compatible samples are very rare (long chains of
     * deterministic constraints, several deterministic plateaus), the search may fail even though
     * such a sample exists, and the result may depend on the random seed. OperationNotAllowed
     * then means "not found", not "does not exist". An exact inference (e.g. LazyPropagation) can
     * decide whether the evidence is compatible (evidence probability > 0).
     *
     * @param prunedDomains if not null, the result of the pruning of step 0 (the output allowed of
     * pruneDomainsByEvidence for samplingBn_ and hardEv_, already proved compatible): the pruning
     * is not done again.
     * @throw IncompatibleEvidence if the pruning proves that the evidence has a null probability
     * @throw OperationNotAllowed if no sample with a positive probability is found
     */
    Instantiation monteCarloSample(const NodeProperty< std::vector< bool > >* prunedDomains
                                   = nullptr);

    /// draws next sample of Gibbs sampling
    Instantiation nextSample(Instantiation prev);

    protected:
    Size                           counting_;
    const IBayesNet< GUM_SCALAR >& samplingBn_;
    const NodeProperty< Idx >*     hardEv_;
    Sequence< NodeId >             samplingNodes_;

    Size nbr_;
    bool atRandom_;

    void updateSamplingNodes_();

    private:
    /// number of forward samples drawn before searching a compatible sample by smoothed Gibbs moves
    static constexpr Size _nbrRejectionTries_ = 100;

    /// number of restarts (from a new forward sample) of the smoothed Gibbs moves
    static constexpr Size _nbrSmoothingRestarts_ = 10;

    /// growing smoothing levels: a null probability is replaced by epsilon times the smallest
    /// positive probability of its CPT
    static constexpr std::array< double, 4 > _smoothingLevels_ = {1e-3, 1e-2, 1e-1, 0.5};

    /// maximal number of sweeps (over the non-evidence nodes) for each smoothing level
    static constexpr Size _nbrSweepsBySmoothingLevel_ = 50;

    /// the values of each node that may belong to a sample with a positive probability
    NodeProperty< std::vector< bool > > _allowed_;

    /// the smallest positive probability of the CPT of each node (for the smoothed Gibbs moves)
    NodeProperty< GUM_SCALAR > _minNonZero_;

    // sets to 0 the probabilities of the values of node that are not allowed
    void _mask_(NodeId node, Tensor< GUM_SCALAR >& p) const;

    // draws a forward sample (hard evidence clamped, values restricted to the allowed ones)
    Instantiation _forwardSample_();

    // adds a node to current instantiation
    void _drawVarMonteCarlo_(NodeId nod, Instantiation* I);

    // checks that the sample has a positive probability
    bool _isCompatible_(const Instantiation& I) const;

    /// the non-observed nodes whose CPT is deterministic (never drawn: they follow their parents)
    NodeSet _deterministic_;

    /// for a drawn node, the deterministic nodes that depend on it through deterministic nodes only
    /// (in a topological order)
    NodeProperty< std::vector< NodeId > > _detDescendants_;

    /// for a drawn node with deterministic descendants, the nodes whose CPT depends on its value
    /// (itself, its children and the children of its deterministic descendants, these excluded)
    NodeProperty< std::vector< NodeId > > _factors_;

    // true if each row of the CPT of node gives the probability 1 to a value
    bool _isDeterministic_(NodeId node) const;

    // sets in I the value of the deterministic node given its parents
    void _setDeterministicValue_(NodeId node, Instantiation* I) const;

    // Gibbs move of node id in the model where the null probabilities are replaced by epsilon times
    // _minNonZero_ (which must be computed beforehand)
    void _smoothedGibbsSample_(NodeId id, Instantiation* I, GUM_SCALAR epsilon);

    void _GibbsSample_(NodeId id, Instantiation* I);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS

  GUM_IGNORE_ATTRIBUTES_BEGIN
  extern template class GUM_PUBLIC_BN GibbsOperator< double >;
  GUM_IGNORE_ATTRIBUTES_END

#endif

}   // namespace gum

#include <agrum/BN/inference/tools/gibbsOperator_tpl.h>

#endif
