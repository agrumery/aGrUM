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
 * @brief the class for computing Bayesian Dirichlet (BD) log2 scores
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
    INLINE ScoreBD<IdSetAlloc, CountAlloc>::ScoreBD(
        const RowFilter &filter, const std::vector<unsigned int> &var_modalities,
        Apriori<IdSetAlloc, CountAlloc> &apriori)
        : Score<IdSetAlloc, CountAlloc>(filter, var_modalities, apriori) {
      // for debugging purposes
      GUM_CONSTRUCTOR(ScoreBD);
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    ScoreBD<IdSetAlloc, CountAlloc>::ScoreBD(
        const ScoreBD<IdSetAlloc, CountAlloc> &from)
        : Score<IdSetAlloc, CountAlloc>(from), __gammalog2(from.__gammalog2),
          __internal_apriori(from.__internal_apriori) {
      // for debugging purposes
      GUM_CONS_CPY(ScoreBD);
    }

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    ScoreBD<IdSetAlloc, CountAlloc>::ScoreBD(ScoreBD<IdSetAlloc, CountAlloc> &&from)
        : Score<IdSetAlloc, CountAlloc>(std::move(from)),
          __gammalog2(std::move(from.__gammalog2)),
          __internal_apriori(std::move(from.__internal_apriori)) {
      // for debugging purposes
      GUM_CONS_MOV(ScoreBD);
    }

    /// virtual copy factory
    template <typename IdSetAlloc, typename CountAlloc>
    ScoreBD<IdSetAlloc, CountAlloc> *
    ScoreBD<IdSetAlloc, CountAlloc>::copyFactory() const {
      return new ScoreBD<IdSetAlloc, CountAlloc>(*this);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreBD<IdSetAlloc, CountAlloc>::~ScoreBD() {
      // for debugging purposes
      GUM_DESTRUCTOR(ScoreBD);
    }

    /// indicates whether the apriori is compatible (meaningful) with the score
    template <typename IdSetAlloc, typename CountAlloc>
    bool ScoreBD<IdSetAlloc, CountAlloc>::isAprioriCompatible(
        const std::string &apriori_type, float weight) {
      if (apriori_type == AprioriNoAprioriType::type) {
        GUM_ERROR(IncompatibleScoreApriori, "The BD score requires an apriori");
      }

      if (weight != 0) {
        GUM_ERROR(PossiblyIncompatibleScoreApriori,
                  "The apriori is currently compatible with the BD score but if "
                  "you change the weight, it may become incompatible");
      }

      // apriori types unsupported by the type checker
      std::stringstream msg;
      msg << "The apriori '" << apriori_type
          << "' is not yet supported by method isAprioriCompatible";
      GUM_ERROR(InvalidArgument, msg.str());
    }

    /// indicates whether the apriori is compatible (meaningful) with the score
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE bool ScoreBD<IdSetAlloc, CountAlloc>::isAprioriCompatible(
        const Apriori<IdSetAlloc, CountAlloc> &apriori) {
      return isAprioriCompatible(apriori.getType(), apriori.weight());
    }

    /// indicates whether the apriori is compatible (meaningful) with the score
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE bool ScoreBD<IdSetAlloc, CountAlloc>::isAprioriCompatible() const {
      return isAprioriCompatible(*this->_apriori);
    }

    /// returns the internal apriori of the score
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const ScoreInternalApriori<IdSetAlloc, CountAlloc> &
    ScoreBD<IdSetAlloc, CountAlloc>::internalApriori() const noexcept {
      return __internal_apriori;
    }

    /// returns the score corresponding to a given nodeset
    template <typename IdSetAlloc, typename CountAlloc>
    float ScoreBD<IdSetAlloc, CountAlloc>::score(unsigned int nodeset_index) {
      // if the score has already been computed, get its value
      if (this->_isInCache(nodeset_index)) {
        return this->_cachedScore(nodeset_index);
      }

      // if the weight of the apriori is 0, then gammaLog2 will fail
      if (this->_apriori->weight() == 0) {
        GUM_ERROR(OutOfBounds,
                  "The BD score requires the apriori to be strictly positive");
      }

      // get the counts for all the targets and the conditioning nodes
      const std::vector<float, CountAlloc> &N_ijk =
          this->_getAllCounts(nodeset_index);
      const unsigned int targets_modal = N_ijk.size();
      float score = 0;

      // here, we distinguish nodesets with conditioning nodes from those
      // without conditioning nodes
      if (this->_getConditioningNodes(nodeset_index)) {
        // get the count of the conditioning nodes
        const std::vector<float, CountAlloc> &N_ij =
            this->_getConditioningCounts(nodeset_index);
        const unsigned int conditioning_modal = N_ij.size();

        const std::vector<float, CountAlloc> &N_prime_ijk =
            this->_getAllApriori(nodeset_index);
        const std::vector<float, CountAlloc> &N_prime_ij =
            this->_getConditioningApriori(nodeset_index);

        // the BD score can be computed as follows:
        // sum_j=1^qi [ gammalog2 ( N'_ij ) - gammalog2 ( N_ij + N'_ij )
        //              + sum_k=1^ri { gammlog2 ( N_ijk + N'_ijk ) -
        //                             gammalog2 ( N'_ijk ) } ]

        for (unsigned int j = 0; j < conditioning_modal; ++j) {
          score += __gammalog2(N_prime_ij[j]) - __gammalog2(N_ij[j] + N_prime_ij[j]);
        }
        for (unsigned int k = 0; k < targets_modal; ++k) {
          score +=
              __gammalog2(N_ijk[k] + N_prime_ijk[k]) - __gammalog2(N_prime_ijk[k]);
        }
      } else {
        const std::vector<float, CountAlloc> &N_prime_ijk =
            this->_getAllApriori(nodeset_index);

        // the BD score can be computed as follows:
        // gammalog2 ( N' ) - gammalog2 ( N + N' )
        // + sum_k=1^ri { gammlog2 ( N_i + N'_i ) - gammalog2 ( N'_i ) }

        float N = 0;
        float N_prime = 0;
        for (unsigned int k = 0; k < targets_modal; ++k) {
          score +=
              __gammalog2(N_ijk[k] + N_prime_ijk[k]) - __gammalog2(N_prime_ijk[k]);
          N += N_ijk[k];
          N_prime += N_prime_ijk[k];
        }
        score += __gammalog2(N_prime) - __gammalog2(N + N_prime);
      }

      // shall we put the score into the cache?
      if (this->_isUsingCache()) {
        this->_insertIntoCache(nodeset_index, score);
      }

      return score;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
