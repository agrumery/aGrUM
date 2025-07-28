/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
 * @brief Implementation of gum::learning::ScoreK2.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/scores_and_tests/scoreK2.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/BN/learning/scores_and_tests/scoreK2_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// copy operator
    ScoreK2& ScoreK2::operator=(const ScoreK2& from) {
      if (this != &from) {
        Score::operator=(from);
        _internal_prior_ = from._internal_prior_;
      }
      return *this;
    }

    /// move operator
    ScoreK2& ScoreK2::operator=(ScoreK2&& from) {
      if (this != &from) {
        Score::operator=(std::move(from));
        _internal_prior_ = std::move(from._internal_prior_);
      }
      return *this;
    }

    /// indicates whether the prior is compatible (meaningful) with the score
    std::string ScoreK2::isPriorCompatible(PriorType prior_type, double weight) {
      // check that the prior is compatible with the score
      if (prior_type == PriorType::NoPriorType) { return ""; }

      if (weight == 0.0) {
        return "The prior is currently compatible with the K2 score but "
               "if you change the weight, it will become incompatible.";
      }

      // known incompatible priors
      if ((prior_type == PriorType::DirichletPriorType)
          || (prior_type == PriorType::SmoothingPriorType)) {
        return "The K2 score already contains a different 'implicit' prior. "
               "Therefore, the learning will probably be biased.";
      }

      // prior types unsupported by the type checker
      std::stringstream msg;
      msg << "The prior '" << priorTypeToString(prior_type)
          << "' is not yet compatible with the score 'K2'.";
      return msg.str();
    }

    /// returns the score corresponding to a given nodeset
    double ScoreK2::score_(const IdCondSet& idset) {
      // get the counts for all the nodes in the idset and add the prior
      std::vector< double > N_ijk(this->counter_.counts(idset, true));
      const std::size_t     all_size                   = N_ijk.size();
      const bool            informative_external_prior = this->prior_->isInformative();
      double                score                      = 0.0;

      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        // get the counts for the conditioning nodes
        std::vector< double > N_ij(this->marginalize_(idset[0], N_ijk));
        const std::size_t     conditioning_size = N_ij.size();
        const double          ri                = double(all_size / conditioning_size);

        if (informative_external_prior) {
          // the score to compute is that of BD with priors N'_ijk + 1
          // (the + 1 is here to take into account the internal prior of K2)
          std::vector< double > N_prime_ijk(all_size, 0.0);
          this->prior_->addJointPseudoCount(idset, N_prime_ijk);
          std::vector< double > N_prime_ij(N_ij.size(), 0.0);
          this->prior_->addConditioningPseudoCount(idset, N_prime_ij);

          // the K2 score can be computed as follows:
          // sum_j=1^qi [ gammalog2 ( N'_ij + r_i ) -
          //              gammalog2 ( N_ij + N'_ij + r_i )
          //              + sum_k=1^ri { gammlog2 ( N_ijk + N'_ijk + 1 ) -
          //                             gammalog2 ( N'_ijk + 1 ) } ]
          for (std::size_t j = std::size_t(0); j < conditioning_size; ++j) {
            score += _gammalog2_(N_prime_ij[j] + ri) - _gammalog2_(N_ij[j] + N_prime_ij[j] + ri);
          }
          for (std::size_t k = std::size_t(0); k < all_size; ++k) {
            score
                += _gammalog2_(N_ijk[k] + N_prime_ijk[k] + 1.0) - _gammalog2_(N_prime_ijk[k] + 1.0);
          }
        } else {
          // the K2 score can be computed as follows:
          // qi log {(ri - 1)!} + sum_j=1^qi [ - log {(N_ij+ri-1)!} +
          //                                   sum_k=1^ri log { N_ijk! } ]
          score = conditioning_size * _gammalog2_(ri);

          for (const auto n_ij: N_ij) {
            score -= _gammalog2_(n_ij + ri);
          }
          for (const auto n_ijk: N_ijk) {
            score += _gammalog2_(n_ijk + 1);
          }
        }
      } else {
        // here, there are no conditioning nodes
        const double ri = double(all_size);

        if (informative_external_prior) {
          // the score to compute is that of BD with priors N'_ijk + 1
          // (the + 1 is here to take into account the internal prior of K2)

          // the K2 score can be computed as follows:
          // gammalog2 ( N' + r_i ) - gammalog2 ( N + N' + r_i )
          // + sum_k=1^ri { gammlog2 ( N_i + N'_i + 1 ) - gammalog2 ( N'_i + 1 )
          // }
          std::vector< double > N_prime_ijk(all_size, 0.0);
          this->prior_->addJointPseudoCount(idset, N_prime_ijk);

          // the K2 score can be computed as follows:
          double N       = 0.0;
          double N_prime = 0.0;
          for (std::size_t k = std::size_t(0); k < all_size; ++k) {
            score += _gammalog2_(N_ijk[k] + N_prime_ijk[k] + 1) - _gammalog2_(N_prime_ijk[k] + 1);
            N += N_ijk[k];
            N_prime += N_prime_ijk[k];
          }
          score += _gammalog2_(N_prime + ri) - _gammalog2_(N + N_prime + ri);
        } else {
          // the K2 score can be computed as follows:
          // log {(ri - 1)!} - log {(N + ri-1)!} + sum_k=1^ri log { N_ijk! } ]
          score    = _gammalog2_(ri);
          double N = 0;
          for (const auto n_ijk: N_ijk) {
            score += _gammalog2_(n_ijk + 1);
            N += n_ijk;
          }
          score -= _gammalog2_(N + ri);
        }
      }

      return score;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
