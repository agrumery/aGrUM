/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Implementation of gum::learning::ScoreBIC.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/scores_and_tests/scoreBIC.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/BN/learning/scores_and_tests/scoreBIC_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// copy operator
    ScoreBIC& ScoreBIC::operator=(const ScoreBIC& from) {
      if (this != &from) {
        Score::operator=(from);
        _internal_prior_ = from._internal_prior_;
      }
      return *this;
    }


    /// move operator
    ScoreBIC& ScoreBIC::operator=(ScoreBIC&& from) {
      if (this != &from) {
        Score::operator=(std::move(from));
        _internal_prior_ = std::move(from._internal_prior_);
      }
      return *this;
    }


    /// indicates whether the prior is compatible (meaningful) with the score
    std::string ScoreBIC::isPriorCompatible(PriorType prior_type, double weight) {
      // check that the prior is compatible with the score
      if ((prior_type == PriorType::DirichletPriorType)
          || (prior_type == PriorType::SmoothingPriorType)
          || (prior_type == PriorType::NoPriorType)) {
        return "";
      }

      // prior types unsupported by the type checker
      std::stringstream msg;
      msg << "The prior '" << priorTypeToString(prior_type)
          << "' is not yet compatible with the score 'BIC'.";
      return msg.str();
    }


    /// returns the score corresponding to a given nodeset
    double ScoreBIC::score_(const IdCondSet& idset) {
      // get the counts for all the nodes in the idset and add the prior
      std::vector< double > N_ijk(this->counter_.counts(idset, true));
      const bool            informative_external_prior = this->prior_->isInformative();
      if (informative_external_prior) this->prior_->addJointPseudoCount(idset, N_ijk);
      const std::size_t all_size = N_ijk.size();

      // here, we distinguish idsets with conditioning nodes from those
      // without conditioning nodes
      if (idset.hasConditioningSet()) {
        // get the counts for the conditioning nodes
        std::vector< double > N_ij(this->marginalize_(idset[0], N_ijk));
        const std::size_t     conditioning_size = N_ij.size();

        // initialize the score: this should be the penalty of the BIC score,
        // i.e., -(ri-1 ) * qi * .5 * log ( N + N' )
        const std::size_t target_domsize = all_size / conditioning_size;
        const double      penalty = conditioning_size * double(target_domsize - std::size_t(1));

        // compute the score: it remains to compute the log likelihood, i.e.,
        // sum_k=1^r_i sum_j=1^q_i N_ijk log (N_ijk / N_ij), which is also
        // equivalent to:
        // sum_j=1^q_i sum_k=1^r_i N_ijk log N_ijk - sum_j=1^q_i N_ij log N_ij
        double score = 0.0;
        for (const auto n_ijk: N_ijk) {
          if (n_ijk) { score += n_ijk * std::log(n_ijk); }
        }
        double N = 0;
        for (const auto n_ij: N_ij) {
          if (n_ij) {
            score -= n_ij * std::log(n_ij);
            N += n_ij;
          }
        }

        // finally, remove the penalty
        score -= penalty * std::log(N) * 0.5;

        // divide by log(2), since the log likelihood uses log_2
        score *= this->one_log2_;

        return score;
      } else {
        // here, there are no conditioning nodes

        // initialize the score: this should be the penalty of the BIC score,
        // i.e., -(ri-1 )
        const double penalty = double(all_size - std::size_t(1));

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

        // finally, remove the penalty
        score -= penalty * std::log(N) * 0.5;

        // divide by log(2), since the log likelihood uses log_2
        score *= this->one_log2_;

        return score;
      }
    }


    /// returns the size of the database w.r.t. a given idset
    double ScoreBIC::N(const IdCondSet& idset) {
      // get the counts for all the nodes in the idset and add the prior
      std::vector< double > N_ijk(this->counter_.counts(idset, true));
      if (this->prior_->isInformative()) this->prior_->addJointPseudoCount(idset, N_ijk);

      double N = 0;
      for (const auto n_ijk: N_ijk) {
        N += n_ijk;
      }

      return N;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
