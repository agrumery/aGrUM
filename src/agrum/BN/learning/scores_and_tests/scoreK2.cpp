/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
        Score::operator    =(from);
        _internal_apriori_ = from._internal_apriori_;
      }
      return *this;
    }


    /// move operator
    ScoreK2& ScoreK2::operator=(ScoreK2&& from) {
      if (this != &from) {
        Score::operator    =(std::move(from));
        _internal_apriori_ = std::move(from._internal_apriori_);
      }
      return *this;
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    std::string ScoreK2::isAprioriCompatible(const std::string& apriori_type, double weight) {
      // check that the apriori is compatible with the score
      if (apriori_type == AprioriNoAprioriType::type) { return ""; }

      if (weight == 0.0) {
        return "The apriori is currently compatible with the K2 score but "
               "if you change the weight, it will become incompatible.";
      }

      // known incompatible aprioris
      if ((apriori_type == AprioriDirichletType::type)
          || (apriori_type == AprioriSmoothingType::type)) {
        return "The K2 score already contains a different 'implicit' apriori. "
               "Therefore, the learning will probably be biased.";
      }

      // apriori types unsupported by the type checker
      std::stringstream msg;
      msg << "The apriori '" << apriori_type << "' is not yet compatible with the score 'K2'.";
      return msg.str();
    }


    /// returns the score corresponding to a given nodeset
    double ScoreK2::score_(const IdCondSet& idset) {
      // get the counts for all the nodes in the idset and add the apriori
      std::vector< double > N_ijk(this->counter_.counts(idset, true));
      const std::size_t     all_size                     = N_ijk.size();
      const bool            informative_external_apriori = this->apriori_->isInformative();
      double                score                        = 0.0;

      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        // get the counts for the conditioning nodes
        std::vector< double > N_ij(this->marginalize_(idset[0], N_ijk));
        const std::size_t     conditioning_size = N_ij.size();
        const double          ri                = double(all_size / conditioning_size);

        if (informative_external_apriori) {
          // the score to compute is that of BD with aprioris N'_ijk + 1
          // (the + 1 is here to take into account the internal apriori of K2)
          std::vector< double > N_prime_ijk(all_size, 0.0);
          this->apriori_->addAllApriori(idset, N_prime_ijk);
          std::vector< double > N_prime_ij(N_ij.size(), 0.0);
          this->apriori_->addConditioningApriori(idset, N_prime_ij);

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

        if (informative_external_apriori) {
          // the score to compute is that of BD with aprioris N'_ijk + 1
          // (the + 1 is here to take into account the internal apriori of K2)

          // the K2 score can be computed as follows:
          // gammalog2 ( N' + r_i ) - gammalog2 ( N + N' + r_i )
          // + sum_k=1^ri { gammlog2 ( N_i + N'_i + 1 ) - gammalog2 ( N'_i + 1 )
          // }
          std::vector< double > N_prime_ijk(all_size, 0.0);
          this->apriori_->addAllApriori(idset, N_prime_ijk);

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
