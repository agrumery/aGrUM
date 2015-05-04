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
 * @brief the internal apriori for the K2 score: Laplace Apriori
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreInternalK2Apriori<IdSetAlloc, CountAlloc>::ScoreInternalK2Apriori() {
      GUM_CONSTRUCTOR(ScoreInternalK2Apriori);
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreInternalK2Apriori<IdSetAlloc, CountAlloc>::ScoreInternalK2Apriori(
        const ScoreInternalK2Apriori<IdSetAlloc, CountAlloc> &from)
        : ScoreInternalApriori<IdSetAlloc, CountAlloc>(from) {
      GUM_CONS_CPY(ScoreInternalK2Apriori);
    }

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreInternalK2Apriori<IdSetAlloc, CountAlloc>::ScoreInternalK2Apriori(
        ScoreInternalK2Apriori<IdSetAlloc, CountAlloc> &&from)
        : ScoreInternalApriori<IdSetAlloc, CountAlloc>(std::move(from)) {
      GUM_CONS_MOV(ScoreInternalK2Apriori);
    }

    /// virtual copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreInternalK2Apriori<IdSetAlloc, CountAlloc> *
    ScoreInternalK2Apriori<IdSetAlloc, CountAlloc>::copyFactory() const {
      return new ScoreInternalK2Apriori<IdSetAlloc, CountAlloc>(*this);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE
        ScoreInternalK2Apriori<IdSetAlloc, CountAlloc>::~ScoreInternalK2Apriori() {
      GUM_DESTRUCTOR(ScoreInternalK2Apriori);
    }

    /// insert the internal score apriori into a set of countings
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void ScoreInternalK2Apriori<IdSetAlloc, CountAlloc>::insertScoreApriori(
        const std::vector<unsigned int> &modalities,
        std::vector<std::vector<float, CountAlloc>> &counts,
        const std::vector<std::pair<std::vector<unsigned int, IdSetAlloc>,
                                    unsigned int> *> &target_nodesets,
        const std::vector<std::pair<std::vector<unsigned int, IdSetAlloc>,
                                    unsigned int> *> &conditioning_nodesets) {
      // put 1's into the countings for the targets
      // and the sum of the weight times the target for the conditioning nodes
      const unsigned int size = target_nodesets.size();
      for (unsigned int i = 0; i < size; ++i) {
        if (target_nodesets[i] != nullptr) {
          std::vector<float, CountAlloc> &countings =
              counts[target_nodesets[i]->second];
          for (auto &count : countings) {
            ++count;
          }
        }

        if (conditioning_nodesets[i] != nullptr) {
          const float weight = modalities[target_nodesets[i]->first.back()];
          std::vector<float, CountAlloc> &countings =
              counts[conditioning_nodesets[i]->second];
          for (auto &count : countings) {
            count += weight;
          }
        }
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
