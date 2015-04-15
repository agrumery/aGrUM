/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
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
 * @brief The local search learning algorithm (for directed graphs)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE LocalSearchWithTabuList::LocalSearchWithTabuList() {
      disableEpsilon();
      disableMinEpsilonRate();
      disableMaxIter();
      disableMaxTime();
      GUM_CONSTRUCTOR(learning::LocalSearchWithTabuList);
    }

    /// copy constructor
    INLINE LocalSearchWithTabuList::LocalSearchWithTabuList(
        const LocalSearchWithTabuList &from)
        : ApproximationScheme(from), __MaxNbDecreasing(from.__MaxNbDecreasing) {
      GUM_CONS_CPY(learning::LocalSearchWithTabuList);
    }

    /// move constructor
    INLINE
    LocalSearchWithTabuList::LocalSearchWithTabuList(LocalSearchWithTabuList &&from)
        : ApproximationScheme(std::move(from)),
          __MaxNbDecreasing(std::move(from.__MaxNbDecreasing)) {
      GUM_CONS_MOV(learning::LocalSearchWithTabuList);
    }

    /// destructor
    INLINE LocalSearchWithTabuList::~LocalSearchWithTabuList() {
      GUM_DESTRUCTOR(learning::LocalSearchWithTabuList);
    }

    /// copy operator
    INLINE LocalSearchWithTabuList &LocalSearchWithTabuList::
    operator=(const LocalSearchWithTabuList &from) {
      ApproximationScheme::operator=(from);
      __MaxNbDecreasing = from.__MaxNbDecreasing;
      return *this;
    }

    /// move operator
    INLINE LocalSearchWithTabuList &LocalSearchWithTabuList::
    operator=(LocalSearchWithTabuList &&from) {
      ApproximationScheme::operator=(std::move(from));
      __MaxNbDecreasing = std::move(from.__MaxNbDecreasing);
      return *this;
    }

    /// set the max number of changes decreasing the score that we allow to apply
    INLINE void LocalSearchWithTabuList::setMaxNbDecreasingChanges(unsigned int nb) {
      __MaxNbDecreasing = nb;
    }

    /// returns the approximation policy of the learning algorithm
    INLINE ApproximationScheme &LocalSearchWithTabuList::approximationScheme() {
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
