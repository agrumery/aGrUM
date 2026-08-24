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
 * @brief Random k-order dynamic Bayesian network generator.
 *
 * gum::KTBNGenerator draws a random gum::KTBN template: variables, a legal
 * arc set and (optionally) random CPTs. It is the k-DBN counterpart of
 * gum::SimpleBayesNetGenerator, and exists mainly to build ground-truth models
 * for learning experiments: sample trajectories from the generated model, learn
 * it back, and compare.
 *
 * @author Seth AGUILA & Anis KHACEF
 */
#ifndef GUM_KTBN_GENERATOR_H
#define GUM_KTBN_GENERATOR_H

#include <string>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/core/utils_random.h>
#include <agrum/KTBN/KTBN.h>

namespace gum {

  /**
   * @class KTBNGenerator
   * @headerfile KTBNGenerator.h <agrum/KTBN/generator/KTBNGenerator.h>
   * @brief Draws a random k-DBN template (structure and, optionally, CPTs).
   * @ingroup bn_group
   *
   * @par Usage
   * @code
   *   initRandom(42);                              // reproducible
   *   KTBNGenerator<double> gen(3, 4, 1);          // k=3, 4 temporal, 1 atemporal
   *   gen.setDensity(0.15).setMaxParents(4);
   *   KTBN<double> model = gen.generate();
   * @endcode
   *
   * @par Which arcs are legal
   * Exactly those gum::KTBN accepts: atemporal @f$\to@f$ atemporal, atemporal
   * @f$\to@f$ temporal, and temporal @f$(b_1,s_1) \to (b_2,s_2)@f$ with
   * @f$s_1 \leq s_2@f$ (so the lag lies in @f$[0, k-1]@f$).  Temporal
   * @f$\to@f$ atemporal is forbidden.  nbLegalArcs() counts them.
   *
   * @par Acyclicity comes for free
   * A cross-slice arc can never close a cycle, since the slice index strictly
   * increases along any path made of them.  Only @b lag-0 arcs and
   * atemporal@f$\to@f$atemporal arcs can.  The generator therefore draws one
   * random permutation of the temporal bases and one of the atemporal bases,
   * and admits those two families only from the lower-ranked endpoint to the
   * higher-ranked one.  No cycle can be built, so no rejection sampling and no
   * cycle test are needed.
   *
   * @par Effective order vs. nominal @f$k@f$
   * Drawn uniformly, a "k-TBN" may end up with @b no arc of lag @f$k-1@f$ into
   * the kernel slice @f$k-1@f$ — its true Markov order is then below @f$k@f$,
   * and no learner can recover @f$k@f$ from data sampled off it, because the
   * data simply does not depend on that far a past.  Order-recovery experiments
   * built on such models silently score correct answers as errors, and the
   * effect grows with @f$k@f$ (more lags to miss at a fixed density).
   * setGuaranteeOrder() (@b on by default) rules this out by forcing one arc of
   * lag exactly @f$k-1@f$ into slice @f$k-1@f$, so the generated model's
   * effective order really is @f$k@f$.  Switch it off to study the phenomenon.
   *
   * @warning The maximum lag a k-slice template can express is @f$k-1@f$ (slice
   * 0 to slice @f$k-1@f$), not @f$k@f$: a lag of @f$k@f$ would need
   * @f$k+1@f$ slices.  For @f$k=1@f$ there is no lag at all, and
   * setGuaranteeOrder() is a silent no-op.
   */
  template < GUM_Numeric GUM_SCALAR >
  class KTBNGenerator {
    public:
    // ===========================================================================
    /// @name Constructors / Destructor
    // ===========================================================================
    /// @{

    /**
     * @brief Constructor.
     * @param k             Order of the generated k-DBN (number of template
     *        slices). Must be @f$\geq 1@f$.
     * @param nbTemporal    Number of temporal processes.
     * @param nbAtemporal   Number of atemporal variables.
     * @param maxArcs       Hard cap on the number of arcs. 0 (default) means
     *        "derive it from the density" — see setDensity().
     * @param maxModality   Largest domain size; domains are drawn uniformly in
     *        @f$[2, maxModality]@f$. Must be @f$\geq 2@f$.
     * @throw InvalidArgument if @p k is 0 or @p maxModality is below 2.
     */
    explicit KTBNGenerator(Size k,
                           Size nbTemporal,
                           Size nbAtemporal = 0,
                           Size maxArcs     = 0,
                           Size maxModality = 2);

    /// @brief Destructor.
    ~KTBNGenerator();

    /// @}
    // ===========================================================================
    /// @name Generation
    // ===========================================================================
    /// @{

    /// @brief Fills @p out with a freshly drawn model (its previous content is
    /// discarded). Seed it with gum::initRandom() for reproducibility.
    void generateKTBN(KTBN< GUM_SCALAR >& out);

    /// @brief Same, returning the model by value.
    KTBN< GUM_SCALAR > generate();

    /// @}
    // ===========================================================================
    /// @name Configuration (fluent)
    // ===========================================================================
    /// @{

    /// @brief Fraction of the legal arc set to draw, in @f$[0,1]@f$. Ignored
    /// when a non-zero @c maxArcs was given to the constructor. Default 0.1.
    /// @throw OutOfBounds if @p density is outside @f$[0,1]@f$.
    KTBNGenerator< GUM_SCALAR >& setDensity(double density);

    /// @brief Domain sizes are drawn uniformly in @f$[min, max]@f$.
    /// @throw InvalidArgument if @p minModality < 2 or @p maxModality < @p minModality.
    KTBNGenerator< GUM_SCALAR >& setDomainRange(Size minModality, Size maxModality);

    /// @brief Caps the number of parents of any node, which bounds CPT size.
    /// 0 (default) means unlimited — a dense draw can then produce very large
    /// CPTs, so set it when generating dense or high-k models.
    KTBNGenerator< GUM_SCALAR >& setMaxParents(Size maxParents);

    /// @brief Force one arc of lag @f$k-1@f$ into the kernel slice, so the
    /// model's effective order equals @f$k@f$ (see the class doc). Default
    /// @c true. No-op when @f$k = 1@f$ or there is no temporal process.
    KTBNGenerator< GUM_SCALAR >& setGuaranteeOrder(bool on);

    /// @brief Whether to fill the CPTs with random values (default @c true).
    /// When @c false only the structure is drawn and the CPTs stay at their
    /// default content.
    KTBNGenerator< GUM_SCALAR >& setGenerateCPTs(bool on);

    /// @brief Name prefixes; variables are @c prefix0, @c prefix1, …
    /// Defaults are @c "X" (temporal) and @c "A" (atemporal).
    /// @throw InvalidArgument if a prefix is empty or the two are equal.
    KTBNGenerator< GUM_SCALAR >& setNamePrefixes(const std::string& temporal,
                                                 const std::string& atemporal);

    /// @}
    // ===========================================================================
    /// @name Accessors
    // ===========================================================================
    /// @{

    /// @return The order of the generated models.
    Size k() const;

    /// @return How many arcs the k-DBN's own rules allow, given the current
    /// shape. The density is a fraction of this, and it is the ceiling any
    /// @c maxArcs is silently clamped to.
    Size nbLegalArcs() const;

    /// @}

    private:
    /// A (tail, head) endpoint pair, each as (base, slice).
    struct _Arc_ {
      std::string tailBase;
      int         tailSlice;
      std::string headBase;
      int         headSlice;
    };

    Size   _k_;
    Size   _nbTemporal_;
    Size   _nbAtemporal_;
    Size   _maxArcs_;
    Size   _minModality_{2};
    Size   _maxModality_;
    Size   _maxParents_{0};
    double _density_{0.1};
    bool   _guaranteeOrder_{true};
    bool   _generateCPTs_{true};

    std::string _temporalPrefix_{"X"};
    std::string _atemporalPrefix_{"A"};

    /// @brief Every arc the k-DBN's rules allow, with the two cycle-prone
    /// families (lag 0, atemporal→atemporal) already restricted to the ranks
    /// given by @p tRank / @p aRank, so the result is acyclic by construction.
    std::vector< _Arc_ > _legalArcs_(const std::vector< std::string >& temporal,
                                     const std::vector< std::string >& atemporal,
                                     const std::vector< Size >&        tRank,
                                     const std::vector< Size >&        aRank) const;

    /// @brief Fisher-Yates through gum::randomValue, so gum::initRandom() alone
    /// makes a whole generation reproducible.
    template < typename T >
    static void _shuffle_(std::vector< T >& v);
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class KTBNGenerator< double >;
#endif

}   // namespace gum

#include <agrum/KTBN/generator/KTBNGenerator_tpl.h>

#endif /* GUM_KTBN_GENERATOR_H */
