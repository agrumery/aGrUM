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
 * @brief Implementation of gum::learning::ScoreLog2Likelihood.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/scores_and_tests/scoreLog2Likelihood.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/BN/learning/scores_and_tests/scoreLog2Likelihood_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// copy operator
    ScoreLog2Likelihood& ScoreLog2Likelihood::operator=(const ScoreLog2Likelihood& from) {
      if (this != &from) {
        Score::operator    =(from);
        _internal_apriori_ = from._internal_apriori_;
      }
      return *this;
    }


    /// move operator
    ScoreLog2Likelihood& ScoreLog2Likelihood::operator=(ScoreLog2Likelihood&& from) {
      if (this != &from) {
        Score::operator    =(std::move(from));
        _internal_apriori_ = std::move(from._internal_apriori_);
      }
      return *this;
    }


    /// indicates whether the apriori is compatible (meaningful) with the score
    std::string ScoreLog2Likelihood::isAprioriCompatible(const std::string& apriori_type,
                                                         double             weight) {
      // check that the apriori is compatible with the score
      if ((apriori_type == AprioriDirichletType::type)
          || (apriori_type == AprioriSmoothingType::type)
          || (apriori_type == AprioriNoAprioriType::type)) {
        return "";
      }

      // apriori types unsupported by the type checker
      std::stringstream msg;
      msg << "The apriori '" << apriori_type
          << "' is not yet compatible with the score 'Log2Likelihood'.";
      return msg.str();
    }


    /// returns the score corresponding to a given nodeset
    double ScoreLog2Likelihood::score_(const IdCondSet& idset) {
      // get the counts for all the nodes in the idset and add the apriori
      std::vector< double > N_ijk(this->counter_.counts(idset, true));
      const bool            informative_external_apriori = this->apriori_->isInformative();
      if (informative_external_apriori) this->apriori_->addAllApriori(idset, N_ijk);

      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        // get the counts for the conditioning nodes
        std::vector< double > N_ij(this->marginalize_(idset[0], N_ijk));

        // compute the score: it remains to compute the log likelihood, i.e.,
        // sum_k=1^r_i sum_j=1^q_i N_ijk log (N_ijk / N_ij), which is also
        // equivalent to:
        // sum_j=1^q_i sum_k=1^r_i N_ijk log N_ijk - sum_j=1^q_i N_ij log N_ij
        double score = 0.0;
        for (const auto n_ijk: N_ijk) {
          if (n_ijk) { score += n_ijk * std::log(n_ijk); }
        }
        for (const auto n_ij: N_ij) {
          if (n_ij) { score -= n_ij * std::log(n_ij); }
        }

        // divide by log(2), since the log likelihood uses log_2
        score *= this->one_log2_;

        return score;
      } else {
        // here, there are no conditioning nodes

        // compute the score: it remains to compute the log likelihood, i.e.,
        // sum_k=1^r_i N_ijk log (N_ijk / N), which is also
        // equivalent to:
        // sum_j=1^q_i sum_k=1^r_i N_ijk log N_ijk - N log N
        double N     = 0.0;
        double score = 0.0;
        for (const auto n_ijk: N_ijk) {
          if (n_ijk) {
            score += n_ijk * std::log(n_ijk);
            N += n_ijk;
          }
        }
        score -= N * std::log(N);

        // divide by log(2), since the log likelihood uses log_2
        score *= this->one_log2_;

        return score;
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
