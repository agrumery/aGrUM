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
 * @brief the internal apriori for the BDeu score (N' / (r_i * q_i)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE
    ScoreInternalBDeuApriori<IdSetAlloc, CountAlloc>::ScoreInternalBDeuApriori() {
      GUM_CONSTRUCTOR(ScoreInternalBDeuApriori);
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE
    ScoreInternalBDeuApriori<IdSetAlloc, CountAlloc>::ScoreInternalBDeuApriori(
        const ScoreInternalBDeuApriori<IdSetAlloc, CountAlloc> &from)
        : ScoreInternalApriori<IdSetAlloc, CountAlloc>(from), __ess(from.__ess) {
      GUM_CONS_CPY(ScoreInternalBDeuApriori);
    }

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE
    ScoreInternalBDeuApriori<IdSetAlloc, CountAlloc>::ScoreInternalBDeuApriori(
        ScoreInternalBDeuApriori<IdSetAlloc, CountAlloc> &&from)
        : ScoreInternalApriori<IdSetAlloc, CountAlloc>(std::move(from)),
          __ess(std::move(from.__ess)) {
      GUM_CONS_MOV(ScoreInternalBDeuApriori);
    }

    /// virtual copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreInternalBDeuApriori<IdSetAlloc, CountAlloc> *
    ScoreInternalBDeuApriori<IdSetAlloc, CountAlloc>::copyFactory() const {
      return new ScoreInternalBDeuApriori<IdSetAlloc, CountAlloc>(*this);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreInternalBDeuApriori<IdSetAlloc,
                                    CountAlloc>::~ScoreInternalBDeuApriori() {
      GUM_DESTRUCTOR(ScoreInternalBDeuApriori);
    }

    /// sets the effective sample size of the internal apriori
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void
    ScoreInternalBDeuApriori<IdSetAlloc, CountAlloc>::setEffectiveSampleSize(
        float ess) {
      if (ess < 0) {
        GUM_ERROR(OutOfBounds, "The effective sample size of the BDeu's "
                               "internal apriori shall be positive");
      } else {
        __ess = ess;
      }
    }

    /// insert the internal score apriori into a set of countings
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void ScoreInternalBDeuApriori<IdSetAlloc, CountAlloc>::insertScoreApriori(
        const std::vector<unsigned int> &modalities,
        std::vector<std::vector<float, CountAlloc>> &counts,
        const std::vector<std::pair<std::vector<unsigned int, IdSetAlloc>,
                                    unsigned int> *> &target_nodesets,
        const std::vector<std::pair<std::vector<unsigned int, IdSetAlloc>,
                                    unsigned int> *> &conditioning_nodesets) {
      if (__ess == 0)
        return;

      const unsigned int size = target_nodesets.size();
      for (unsigned int i = 0; i < size; ++i) {
        if (target_nodesets[i] != nullptr) {
          // compute the r_i's and q_i's
          const float r_i = modalities[target_nodesets[i]->first.back()];
          float q_i = 1;
          if (conditioning_nodesets[i] != nullptr) {
            const std::vector<unsigned int, IdSetAlloc> &cond =
                conditioning_nodesets[i]->first;
            for (auto parent : cond) {
              q_i *= modalities[parent];
            }
          }

          // put ess / (r_i * q_i)  into the countings for the targets
          const float target_weight = __ess / (r_i * q_i);
          std::vector<float, CountAlloc> &countings =
              counts[target_nodesets[i]->second];
          for (auto &count : countings) {
            count += target_weight;
          }

          // put ess / q_i into the countings for the conditioning nodes
          if (conditioning_nodesets[i] != nullptr) {
            const float cond_weight = __ess / q_i;
            std::vector<float, CountAlloc> &countings =
                counts[conditioning_nodesets[i]->second];
            for (auto &count : countings) {
              count += cond_weight;
            }
          }
        }
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
