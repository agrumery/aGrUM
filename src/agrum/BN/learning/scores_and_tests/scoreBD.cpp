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
 * @brief Implementation of gum::learning::ScoreBD.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/scores_and_tests/scoreBD.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/BN/learning/scores_and_tests/scoreBD_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// copy operator
    ScoreBD& ScoreBD::operator=(const ScoreBD& from) {
      if (this != &from) {
        Score::operator    =(from);
        _internal_apriori_ = from._internal_apriori_;
      }
      return *this;
    }


    /// move operator
    ScoreBD& ScoreBD::operator=(ScoreBD&& from) {
      if (this != &from) {
        Score::operator    =(std::move(from));
        _internal_apriori_ = std::move(from._internal_apriori_);
      }
      return *this;
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    std::string ScoreBD::isPriorCompatible(PriorType apriori_type, double weight) {
      if (apriori_type == PriorType::NoPriorType) { return "The BD score requires an apriori"; }

      if (weight != 0.0) {
        return "The apriori is currently compatible with the BD score but if "
               "you change the weight, it may become biased";
      }

      // apriori types unsupported by the type checker
      std::stringstream msg;
      msg << "The apriori '" << priorTypeToString(apriori_type) << "' is not yet compatible with the score 'BD'.";
      return msg.str();
    }


    /// returns the score corresponding to a given nodeset
    double ScoreBD::score_(const IdCondSet& idset) {
      // if the weight of the apriori is 0, then gammaLog2 will fail
      if (!this->apriori_->isInformative()) {
        GUM_ERROR(OutOfBounds,
                  "The BD score requires its external apriori to "
                     << "be strictly positive");
      }

      // get the counts for all the nodes in the idset and add the apriori
      std::vector< double > N_ijk(this->counter_.counts(idset, true));
      const std::size_t     all_size = N_ijk.size();
      std::vector< double > N_prime_ijk(all_size, 0.0);
      this->apriori_->addAllApriori(idset, N_prime_ijk);

      double score = 0.0;

      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        // get the counts for the conditioning nodes
        std::vector< double > N_ij(this->marginalize_(idset[0], N_ijk));
        const std::size_t     conditioning_size = N_ij.size();

        std::vector< double > N_prime_ij(N_ij.size(), 0.0);
        this->apriori_->addConditioningApriori(idset, N_prime_ij);

        // the BD score can be computed as follows:
        // sum_j=1^qi [ gammalog2 ( N'_ij ) - gammalog2 ( N_ij + N'_ij )
        //              + sum_k=1^ri { gammlog2 ( N_ijk + N'_ijk ) -
        //                             gammalog2 ( N'_ijk ) } ]
        for (std::size_t j = std::size_t(0); j < conditioning_size; ++j) {
          score += _gammalog2_(N_prime_ij[j]) - _gammalog2_(N_ij[j] + N_prime_ij[j]);
        }
        for (std::size_t k = std::size_t(0); k < all_size; ++k) {
          score += _gammalog2_(N_ijk[k] + N_prime_ijk[k]) - _gammalog2_(N_prime_ijk[k]);
        }
      } else {
        // the BD score can be computed as follows:
        // gammalog2 ( N' ) - gammalog2 ( N + N' )
        // + sum_k=1^ri { gammlog2 ( N_i + N'_i ) - gammalog2 ( N'_i ) }
        double N       = 0.0;
        double N_prime = 0.0;
        for (std::size_t k = std::size_t(0); k < all_size; ++k) {
          score += _gammalog2_(N_ijk[k] + N_prime_ijk[k]) - _gammalog2_(N_prime_ijk[k]);
          N += N_ijk[k];
          N_prime += N_prime_ijk[k];
        }
        score += _gammalog2_(N_prime) - _gammalog2_(N + N_prime);
      }

      return score;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
