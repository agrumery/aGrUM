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
 * @brief the internal apriori for the scores without apriori (e.g., BD)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreInternalNoApriori<IdSetAlloc, CountAlloc>::ScoreInternalNoApriori() {
      GUM_CONSTRUCTOR(ScoreInternalNoApriori);
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreInternalNoApriori<IdSetAlloc, CountAlloc>::ScoreInternalNoApriori(
        const ScoreInternalNoApriori<IdSetAlloc, CountAlloc> &from)
        : ScoreInternalApriori<IdSetAlloc, CountAlloc>(from) {
      GUM_CONS_CPY(ScoreInternalNoApriori);
    }

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreInternalNoApriori<IdSetAlloc, CountAlloc>::ScoreInternalNoApriori(
        ScoreInternalNoApriori<IdSetAlloc, CountAlloc> &&from)
        : ScoreInternalApriori<IdSetAlloc, CountAlloc>(std::move(from)) {
      GUM_CONS_MOV(ScoreInternalNoApriori);
    }

    /// virtual copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreInternalNoApriori<IdSetAlloc, CountAlloc> *
    ScoreInternalNoApriori<IdSetAlloc, CountAlloc>::copyFactory() const {
      return new ScoreInternalNoApriori<IdSetAlloc, CountAlloc>(*this);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE
        ScoreInternalNoApriori<IdSetAlloc, CountAlloc>::~ScoreInternalNoApriori() {
      GUM_DESTRUCTOR(ScoreInternalNoApriori);
    }

    /// insert the internal score apriori into a set of countings
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void ScoreInternalNoApriori<IdSetAlloc, CountAlloc>::insertScoreApriori(
        const std::vector<unsigned int> &modalities,
        std::vector<std::vector<float, CountAlloc>> &counts,
        const std::vector<std::pair<std::vector<unsigned int, IdSetAlloc>,
                                    unsigned int> *> &target_nodesets,
        const std::vector<std::pair<std::vector<unsigned int, IdSetAlloc>,
                                    unsigned int> *> &conditioning_nodesets) {}

    /// indicates whether the apriori is potentially informative
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE bool
    ScoreInternalNoApriori<IdSetAlloc, CountAlloc>::isInformative() const {
      return false;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
