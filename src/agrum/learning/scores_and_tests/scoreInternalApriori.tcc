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
 * @brief the base class for all the score's internal aprioris
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreInternalApriori<IdSetAlloc, CountAlloc>::ScoreInternalApriori() {
      GUM_CONSTRUCTOR(ScoreInternalApriori);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreInternalApriori<IdSetAlloc, CountAlloc>::~ScoreInternalApriori() {
      GUM_DESTRUCTOR(ScoreInternalApriori);
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreInternalApriori<IdSetAlloc, CountAlloc>::ScoreInternalApriori(
        const ScoreInternalApriori &from) {
      GUM_CONS_CPY(ScoreInternalApriori);
    }

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreInternalApriori<IdSetAlloc, CountAlloc>::ScoreInternalApriori(
        ScoreInternalApriori &&from) {
      GUM_CONS_MOV(ScoreInternalApriori);
    }

    /// indicates whether the apriori is potentially informative
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE bool ScoreInternalApriori<IdSetAlloc, CountAlloc>::isInformative() const {
      return true;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
