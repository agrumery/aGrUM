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
 * @brief the class for computing K2 scores (actually their log2 value)
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
    INLINE ScoreK2<IdSetAlloc, CountAlloc>::ScoreK2(
        const RowFilter &filter, const std::vector<unsigned int> &var_modalities,
        Apriori<IdSetAlloc, CountAlloc> &apriori)
        : Score<IdSetAlloc, CountAlloc>(filter, var_modalities, apriori) {
      // for debugging purposes
      GUM_CONSTRUCTOR(ScoreK2);
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    ScoreK2<IdSetAlloc, CountAlloc>::ScoreK2(
        const ScoreK2<IdSetAlloc, CountAlloc> &from)
        : Score<IdSetAlloc, CountAlloc>(from), __gammalog2(from.__gammalog2),
          __internal_apriori(from.__internal_apriori) {
      GUM_CONS_CPY(ScoreK2);
    }

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    ScoreK2<IdSetAlloc, CountAlloc>::ScoreK2(ScoreK2<IdSetAlloc, CountAlloc> &&from)
        : Score<IdSetAlloc, CountAlloc>(std::move(from)),
          __gammalog2(std::move(from.__gammalog2)),
          __internal_apriori(std::move(from.__internal_apriori)) {
      GUM_CONS_MOV(ScoreK2);
    }

    /// virtual copy factory
    template <typename IdSetAlloc, typename CountAlloc>
    ScoreK2<IdSetAlloc, CountAlloc> *
    ScoreK2<IdSetAlloc, CountAlloc>::copyFactory() const {
      return new ScoreK2<IdSetAlloc, CountAlloc>(*this);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreK2<IdSetAlloc, CountAlloc>::~ScoreK2() {
      // for debugging purposes
      GUM_DESTRUCTOR(ScoreK2);
    }

    /// indicates whether the apriori is compatible (meaningful) with the score
    template <typename IdSetAlloc, typename CountAlloc>
    bool ScoreK2<IdSetAlloc, CountAlloc>::isAprioriCompatible(
        const std::string &apriori_type, float weight) {
      // check that the apriori is compatible with the score
      if (apriori_type == AprioriNoAprioriType::type) {
        return true;
      }

      if (weight == 0) {
        GUM_ERROR(PossiblyIncompatibleScoreApriori,
                  "The apriori is currently compatible with the K2 score but if "
                  "you change the weight, it will become incompatible");
      }

      // known incompatible aprioris
      if ((apriori_type == AprioriDirichletType::type) ||
          (apriori_type == AprioriSmoothingType::type)) {
        GUM_ERROR(IncompatibleScoreApriori,
                  "The K2 score already contains a different 'implicit' apriori."
                  " Therefore, the learning will probably be meaningless.");
      }

      // apriori types unsupported by the type checker
      std::stringstream msg;
      msg << "The apriori '" << apriori_type
          << "' is not yet supported by method isAprioriCompatible";
      GUM_ERROR(InvalidArgument, msg.str());
    }

    /// indicates whether the apriori is compatible (meaningful) with the score
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE bool ScoreK2<IdSetAlloc, CountAlloc>::isAprioriCompatible(
        const Apriori<IdSetAlloc, CountAlloc> &apriori) {
      return isAprioriCompatible(apriori.getType(), apriori.weight());
    }

    /// indicates whether the apriori is compatible (meaningful) with the score
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE bool ScoreK2<IdSetAlloc, CountAlloc>::isAprioriCompatible() const {
      return isAprioriCompatible(*this->_apriori);
    }

    /// returns the internal apriori of the score
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const ScoreInternalApriori<IdSetAlloc, CountAlloc> &
    ScoreK2<IdSetAlloc, CountAlloc>::internalApriori() const noexcept {
      return __internal_apriori;
    }

    /// returns the score corresponding to a given nodeset
    template <typename IdSetAlloc, typename CountAlloc>
    float ScoreK2<IdSetAlloc, CountAlloc>::score(unsigned int nodeset_index) {
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
        const std::vector<float, CountAlloc> &N_ij =
            this->_getConditioningCounts(nodeset_index);
        const unsigned int conditioning_modal = N_ij.size();

        if (this->_apriori->weight()) {
          // the score to compute is that of BD with aprioris N'_ijk + 1
          // (the + 1 is here to take into account the internal apriori of K2)
          const std::vector<float, CountAlloc> &N_prime_ijk =
              this->_getAllApriori(nodeset_index);
          const std::vector<float, CountAlloc> &N_prime_ij =
              this->_getConditioningApriori(nodeset_index);

          // the K2 score can be computed as follows:
          // sum_j=1^qi [ gammalog2 ( N'_ij + r_i ) -
          //              gammalog2 ( N_ij + N'_ij + r_i )
          //              + sum_k=1^ri { gammlog2 ( N_ijk + N'_ijk + 1 ) -
          //                             gammalog2 ( N'_ijk + 1 ) } ]

          const float r_i = modalities[all_nodes.back()];
          for (unsigned int j = 0; j < conditioning_modal; ++j) {
            score += __gammalog2(N_prime_ij[j] + r_i) -
                     __gammalog2(N_ij[j] + N_prime_ij[j] + r_i);
          }
          for (unsigned int k = 0; k < targets_modal; ++k) {
            score += __gammalog2(N_ijk[k] + N_prime_ijk[k] + 1) -
                     __gammalog2(N_prime_ijk[k] + 1);
          }
        } else {
          // the K2 score can be computed as follows:
          // qi log {(ri - 1)!} + sum_j=1^qi [ - log {(N_ij+ri-1)!} +
          //                                   sum_k=1^ri log { N_ijk! } ]

          // put the first term: qi log {(ri - 1)!} into the score
          const float r_i = modalities[all_nodes.back()];
          score = conditioning_modal * __gammalog2(r_i);

          for (unsigned int k = 0; k < targets_modal; ++k) {
            score += __gammalog2(N_ijk[k] + 1);
          }

          for (unsigned int j = 0; j < conditioning_modal; ++j) {
            score -= __gammalog2(N_ij[j] + r_i);
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

          // the score to compute is that of BD with aprioris N'_ijk + 1
          // (the + 1 is here to take into account the internal apriori of K2)

          // the BD score can be computed as follows:
          // gammalog2 ( N' + r_i ) - gammalog2 ( N + N' + r_i )
          // + sum_k=1^ri { gammlog2 ( N_i + N'_i + 1 ) - gammalog2 ( N'_i + 1 ) }
          const float r_i = modalities[all_nodes.back()];
          float N = 0;
          float N_prime = 0;
          for (unsigned int k = 0; k < targets_modal; ++k) {
            score += __gammalog2(N_ijk[k] + N_prime_ijk[k] + 1) -
                     __gammalog2(N_prime_ijk[k] + 1);
            N += N_ijk[k];
            N_prime += N_prime_ijk[k];
          }
          score += __gammalog2(N_prime + r_i) - __gammalog2(N + N_prime + r_i);
        } else {
          // the K2 score can be computed as follows:
          // log {(ri - 1)!} - log {(N + ri-1)!} + sum_k=1^ri log { N_ijk! } ]

          // put the first term: log {(ri - 1)!} into the score
          const float r_i = modalities[all_nodes.back()];
          score = __gammalog2(r_i);

          float N = 0;
          for (unsigned int k = 0; k < targets_modal; ++k) {
            score += __gammalog2(N_ijk[k] + 1);
            N += N_ijk[k];
          }

          score -= __gammalog2(N + r_i);
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
