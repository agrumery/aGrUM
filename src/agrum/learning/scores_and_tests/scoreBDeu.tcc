/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief the class for computing BDeu scores (actually their log2 value)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <cmath>
#include <sstream>

namespace gum {

  namespace learning {

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    template <typename RowFilter>
    INLINE ScoreBDeu<IdSetAlloc, CountAlloc>::ScoreBDeu(
        const RowFilter &filter, const std::vector<unsigned int> &var_modalities,
        Apriori<IdSetAlloc, CountAlloc> &apriori)
        : Score<IdSetAlloc, CountAlloc>(filter, var_modalities, apriori) {
      __internal_apriori.setEffectiveSampleSize(__ess);
      // for debugging purposes
      GUM_CONSTRUCTOR(ScoreBDeu);
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    ScoreBDeu<IdSetAlloc, CountAlloc>::ScoreBDeu(
        const ScoreBDeu<IdSetAlloc, CountAlloc> &from)
        : Score<IdSetAlloc, CountAlloc>(from), __gammalog2(from.__gammalog2),
          __internal_apriori(from.__internal_apriori) {
      // for debugging purposes
      GUM_CONS_CPY(ScoreBDeu);
    }

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    ScoreBDeu<IdSetAlloc, CountAlloc>::ScoreBDeu(
        ScoreBDeu<IdSetAlloc, CountAlloc> &&from)
        : Score<IdSetAlloc, CountAlloc>(std::move(from)),
          __gammalog2(std::move(from.__gammalog2)),
          __internal_apriori(std::move(from.__internal_apriori)) {
      // for debugging purposes
      GUM_CONS_MOV(ScoreBDeu);
    }

    /// virtual copy factory
    template <typename IdSetAlloc, typename CountAlloc>
    ScoreBDeu<IdSetAlloc, CountAlloc> *
    ScoreBDeu<IdSetAlloc, CountAlloc>::copyFactory() const {
      return new ScoreBDeu<IdSetAlloc, CountAlloc>(*this);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreBDeu<IdSetAlloc, CountAlloc>::~ScoreBDeu() {
      // for debugging purposes
      GUM_DESTRUCTOR(ScoreBDeu);
    }

    /// indicates whether the apriori is compatible (meaningful) with the score
    template <typename IdSetAlloc, typename CountAlloc>
    bool ScoreBDeu<IdSetAlloc, CountAlloc>::isAprioriCompatible(
        const std::string &apriori_type, float weight) {
      // check that the apriori is compatible with the score
      if (apriori_type == AprioriNoAprioriType::type) {
        return true;
      }

      if (weight == 0) {
        GUM_ERROR(PossiblyIncompatibleScoreApriori,
                  "The apriori is currently compatible with the BDeu score but "
                  "if you change the weight, it will become incompatible");
      }

      // known incompatible aprioris
      if ((apriori_type == AprioriDirichletType::type) ||
          (apriori_type == AprioriSmoothingType::type)) {
        GUM_ERROR(IncompatibleScoreApriori,
                  "The BDeu score already contains a different 'implicit' aprio"
                  "ri. Therefore, the learning will probably be meaningless.");
      }

      // apriori types unsupported by the type checker
      std::stringstream msg;
      msg << "The apriori '" << apriori_type
          << "' is not yet supported by method isAprioriCompatible";
      GUM_ERROR(InvalidArgument, msg.str());
    }

    /// indicates whether the apriori is compatible (meaningful) with the score
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE bool ScoreBDeu<IdSetAlloc, CountAlloc>::isAprioriCompatible(
        const Apriori<IdSetAlloc, CountAlloc> &apriori) {
      return isAprioriCompatible(apriori.getType(), apriori.weight());
    }

    /// indicates whether the apriori is compatible (meaningful) with the score
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE bool ScoreBDeu<IdSetAlloc, CountAlloc>::isAprioriCompatible() const {
      return isAprioriCompatible(*this->_apriori);
    }

    /// returns the internal apriori of the score
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const ScoreInternalApriori<IdSetAlloc, CountAlloc> &
    ScoreBDeu<IdSetAlloc, CountAlloc>::internalApriori() const noexcept {
      return __internal_apriori;
    }

    /// sets the effective sample size of the internal apriori
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void
    ScoreBDeu<IdSetAlloc, CountAlloc>::setEffectiveSampleSize(float ess) {
      if (ess < 0) {
        GUM_ERROR(OutOfBounds, "The effective sample size of the BDeu's "
                               "internal apriori shall be positive");
      } else {
        __ess = ess;
        __internal_apriori.setEffectiveSampleSize(ess);
      }
    }

    /// returns the score corresponding to a given nodeset
    template <typename IdSetAlloc, typename CountAlloc>
    float ScoreBDeu<IdSetAlloc, CountAlloc>::score(unsigned int nodeset_index) {
      // if the score has already been computed, get its value
      if (this->_isInCache(nodeset_index)) {
        return this->_cachedScore(nodeset_index);
      }

      // get the counts for all the targets and for the conditioning nodes
      const std::vector<float, CountAlloc> &N_ijk =
          this->_getAllCounts(nodeset_index);
      const unsigned int targets_modal = N_ijk.size();
      float score = 0;

      // get the nodes involved in the score as well as their modalities
      const std::vector<unsigned int, IdSetAlloc> &all_nodes =
          this->_getAllNodes(nodeset_index);
      const std::vector<unsigned int, IdSetAlloc> *conditioning_nodes =
          this->_getConditioningNodes(nodeset_index);
      const std::vector<unsigned int> &modalities = this->modalities();

      // here, we distinguish nodesets with conditioning nodes from those
      // without conditioning nodes
      if (conditioning_nodes) {
        // get the count of the conditioning nodes
        const std::vector<float, CountAlloc> &N_ij =
            this->_getConditioningCounts(nodeset_index);
        const unsigned int conditioning_modal = N_ij.size();

        if (this->_apriori->weight()) {
          // the score to compute is that of BD with aprioris
          // N'_ijk + ESS / (r_i * q_i )
          // (the + ESS / (r_i * q_i ) is here to take into account the
          // internal apriori of K2)
          const std::vector<float, CountAlloc> &N_prime_ijk =
              this->_getAllApriori(nodeset_index);
          const std::vector<float, CountAlloc> &N_prime_ij =
              this->_getConditioningApriori(nodeset_index);

          // the BDeu score can be computed as follows:
          // sum_j=1^qi [ gammalog2 ( N'_ij + ESS / q_i ) -
          //     gammalog2 ( N_ij + N'_ij + ESS / q_i )
          //     + sum_k=1^ri { gammlog2 ( N_ijk + N'_ijk + ESS / (r_i * q_i ) ) -
          //     gammalog2 ( N'_ijk + ESS / (r_i * q_i ) ) } ]
          const float ess_qi = __ess / conditioning_modal;
          const float ess_riqi = ess_qi / modalities[all_nodes.back()];

          for (unsigned int j = 0; j < conditioning_modal; ++j) {
            score += __gammalog2(N_prime_ij[j] + ess_qi) -
                     __gammalog2(N_ij[j] + N_prime_ij[j] + ess_qi);
          }
          for (unsigned int k = 0; k < targets_modal; ++k) {
            score += __gammalog2(N_ijk[k] + N_prime_ijk[k] + ess_riqi) -
                     __gammalog2(N_prime_ijk[k] + ess_riqi);
          }
        } else {
          // the BDeu score can be computed as follows:
          // qi * gammalog2 (ess / qi) - ri * qi * gammalog2 (ess / (ri * qi) )
          // - sum_j=1^qi [ gammalog2 ( N_ij + ess / qi ) ]
          // + sum_j=1^qi sum_k=1^ri log [ gammalog2 ( N_ijk + ess / (ri * qi) ) ]

          // precompute ess / qi and ess / ( ri * qi )
          const float ri = modalities[all_nodes.back()];
          const float ess_qi = __ess / conditioning_modal;
          const float ess_qiri = ess_qi / ri;

          score = conditioning_modal * __gammalog2(ess_qi) -
                  ri * conditioning_modal * __gammalog2(ess_qiri);

          for (unsigned int j = 0; j < conditioning_modal; ++j) {
            score -= __gammalog2(N_ij[j] + ess_qi);
          }
          for (unsigned int k = 0; k < targets_modal; ++k) {
            score += __gammalog2(N_ijk[k] + ess_qiri);
          }
        }

        // shall we put the score into the cache?
        if (this->_isUsingCache()) {
          this->_insertIntoCache(nodeset_index, score);
        }

        return score;
      } else {
        // here, there are no conditioning nodes

        if (this->_apriori->weight()) {
          const std::vector<float, CountAlloc> &N_prime_ijk =
              this->_getAllApriori(nodeset_index);

          // the score to compute is that of BD with aprioris
          // N'_ijk + ESS / ( ri * qi )
          // (the + ESS / ( ri * qi ) is here to take into account the
          // internal apriori of K2)

          // the BDeu score can be computed as follows:
          // gammalog2 ( N' + ess ) - gammalog2 ( N + N' + ess )
          // + sum_k=1^ri { gammlog2 ( N_i + N'_i + ESS / ri)
          //                - gammalog2 ( N'_i + ESS / ri ) }

          // precompute ess / ri
          const float ri = modalities[all_nodes.back()];
          const float ess_ri = __ess / ri;

          float N = 0;
          float N_prime = 0;
          for (unsigned int k = 0; k < targets_modal; ++k) {
            score += __gammalog2(N_ijk[k] + N_prime_ijk[k] + ess_ri) -
                     __gammalog2(N_prime_ijk[k] + ess_ri);
            N += N_ijk[k];
            N_prime += N_prime_ijk[k];
          }
          score += __gammalog2(N_prime + __ess) - __gammalog2(N + N_prime + __ess);
        } else {
          // the BDeu score can be computed as follows:
          // gammalog2 ( ess ) - ri * gammalog2 ( ess / ri )
          // - gammalog2 ( N + ess )
          // + sum_k=1^ri log [ gammalog2 ( N_ijk + ess / ri ) ]

          // precompute ess / ri
          const float ri = modalities[all_nodes.back()];
          const float ess_ri = __ess / ri;

          score = __gammalog2(__ess) - ri * __gammalog2(ess_ri);
          float N = 0;
          for (unsigned int k = 0; k < targets_modal; ++k) {
            score += __gammalog2(N_ijk[k] + ess_ri);
            N += N_ijk[k];
          }
          score -= __gammalog2(N + __ess);
        }

        // shall we put the score into the cache?
        if (this->_isUsingCache()) {
          this->_insertIntoCache(nodeset_index, score);
        }

        return score;
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
