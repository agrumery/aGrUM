/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Implementation of gum::learning::ScoreBDeu.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/scores_and_tests/scoreBDeu.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/BN/learning/scores_and_tests/scoreBDeu_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// copy operator
    ScoreBDeu& ScoreBDeu::operator=(const ScoreBDeu& from) {
      if (this != &from) {
        Score::operator    =(from);
        _internal_apriori_ = from._internal_apriori_;
      }
      return *this;
    }


    /// move operator
    ScoreBDeu& ScoreBDeu::operator=(ScoreBDeu&& from) {
      if (this != &from) {
        Score::operator    =(std::move(from));
        _internal_apriori_ = std::move(from._internal_apriori_);
      }
      return *this;
    }


    /// indicates whether the prior is compatible (meaningful) with the score
    std::string ScoreBDeu::isPriorCompatible(PriorType apriori_type, double weight) {
      // check that the prior is compatible with the score
      if (apriori_type == PriorType::NoPriorType) { return ""; }

      if (weight == 0.0) {
        return "The prior is currently compatible with the BDeu score but "
               "if you change the weight, it will become incompatible.";
      }

      // known incompatible priors
      if ((apriori_type == PriorType::DirichletPriorType)
          || (apriori_type == PriorType::SmoothingPriorType)) {
        return "The BDeu score already contains a different 'implicit' prior. "
               "Therefore, the learning will probably be biased.";
      }

      // prior types unsupported by the type checker
      std::stringstream msg;
      msg << "The prior '" << priorTypeToString(apriori_type)
          << "' is not yet compatible with the score 'BDeu'.";
      return msg.str();
    }


    /// returns the score corresponding to a given nodeset
    double ScoreBDeu::score_(const IdCondSet& idset) {
      // get the counts for all the nodes in the idset and add the prior
      std::vector< double > N_ijk(this->counter_.counts(idset, true));
      const std::size_t     all_size = N_ijk.size();

      double       score                        = 0.0;
      const double ess                          = _internal_apriori_.weight();
      const bool   informative_external_apriori = this->apriori_->isInformative();


      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        // get the counts for the conditioning nodes
        std::vector< double > N_ij(this->marginalize_(idset[0], N_ijk));
        const std::size_t     conditioning_size = N_ij.size();
        const double          ess_qi            = ess / conditioning_size;
        const double          ess_riqi          = ess / all_size;

        if (informative_external_apriori) {
          // the score to compute is that of BD with priors
          // N'_ijk + ESS / (r_i * q_i )
          // (the + ESS / (r_i * q_i ) is here to take into account the
          // internal prior of BDeu)
          std::vector< double > N_prime_ijk(all_size, 0.0);
          this->apriori_->addAllApriori(idset, N_prime_ijk);
          std::vector< double > N_prime_ij(N_ij.size(), 0.0);
          this->apriori_->addConditioningApriori(idset, N_prime_ij);

          // the BDeu score can be computed as follows:
          // sum_j=1^qi [ gammalog2 ( N'_ij + ESS / q_i ) -
          //     gammalog2 ( N_ij + N'_ij + ESS / q_i )
          //     + sum_k=1^ri { gammlog2 ( N_ijk + N'_ijk + ESS / (r_i * q_i ) )
          //     - gammalog2 ( N'_ijk + ESS / (r_i * q_i ) ) } ]
          for (std::size_t j = std::size_t(0); j < conditioning_size; ++j) {
            score += _gammalog2_(N_prime_ij[j] + ess_qi)
                   - _gammalog2_(N_ij[j] + N_prime_ij[j] + ess_qi);
          }
          for (std::size_t k = std::size_t(0); k < all_size; ++k) {
            score += _gammalog2_(N_ijk[k] + N_prime_ijk[k] + ess_riqi)
                   - _gammalog2_(N_prime_ijk[k] + ess_riqi);
          }
        } else {
          // the BDeu score can be computed as follows:
          // qi * gammalog2 (ess / qi) - ri * qi * gammalog2 (ess / (ri * qi) )
          // - sum_j=1^qi [ gammalog2 ( N_ij + ess / qi ) ]
          // + sum_j=1^qi sum_k=1^ri log [ gammalog2 ( N_ijk + ess / (ri * qi) )
          // ]
          score = conditioning_size * _gammalog2_(ess_qi) - all_size * _gammalog2_(ess_riqi);

          for (const auto n_ij: N_ij) {
            score -= _gammalog2_(n_ij + ess_qi);
          }
          for (const auto n_ijk: N_ijk) {
            score += _gammalog2_(n_ijk + ess_riqi);
          }
        }
      } else {
        // here, there are no conditioning nodes
        const double ess_ri = ess / all_size;

        if (informative_external_apriori) {
          // the score to compute is that of BD with priors
          // N'_ijk + ESS / ( ri * qi )
          // (the + ESS / ( ri * qi ) is here to take into account the
          // internal prior of K2)
          std::vector< double > N_prime_ijk(all_size, 0.0);
          this->apriori_->addAllApriori(idset, N_prime_ijk);

          // the BDeu score can be computed as follows:
          // gammalog2 ( N' + ess ) - gammalog2 ( N + N' + ess )
          // + sum_k=1^ri { gammlog2 ( N_i + N'_i + ESS / ri)
          //                - gammalog2 ( N'_i + ESS / ri ) }
          double N       = 0.0;
          double N_prime = 0.0;
          for (std::size_t k = std::size_t(0); k < all_size; ++k) {
            score += _gammalog2_(N_ijk[k] + N_prime_ijk[k] + ess_ri)
                   - _gammalog2_(N_prime_ijk[k] + ess_ri);
            N += N_ijk[k];
            N_prime += N_prime_ijk[k];
          }
          score += _gammalog2_(N_prime + ess) - _gammalog2_(N + N_prime + ess);
        } else {
          // the BDeu score can be computed as follows:
          // gammalog2 ( ess ) - ri * gammalog2 ( ess / ri )
          // - gammalog2 ( N + ess )
          // + sum_k=1^ri log [ gammalog2 ( N_ijk + ess / ri ) ]

          score    = _gammalog2_(ess) - all_size * _gammalog2_(ess_ri);
          double N = 0;
          for (const auto n_ijk: N_ijk) {
            score += _gammalog2_(n_ijk + ess_ri);
            N += n_ijk;
          }
          score -= _gammalog2_(N + ess);
        }
      }

      return score;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
