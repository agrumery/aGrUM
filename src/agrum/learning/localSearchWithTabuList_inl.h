
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
      GUM_CONSTRUCTOR(LocalSearchWithTabuList);
    }

    /// copy constructor
    INLINE LocalSearchWithTabuList::LocalSearchWithTabuList(
       const LocalSearchWithTabuList& from) :
        ApproximationScheme(from),
        __MaxNbDecreasing(from.__MaxNbDecreasing) {
      GUM_CONS_CPY(LocalSearchWithTabuList);
    }

    /// move constructor
    INLINE
    LocalSearchWithTabuList::LocalSearchWithTabuList(
       LocalSearchWithTabuList&& from) :
        ApproximationScheme(std::move(from)),
        __MaxNbDecreasing(std::move(from.__MaxNbDecreasing)) {
      GUM_CONS_MOV(LocalSearchWithTabuList);
    }

    /// destructor
    INLINE LocalSearchWithTabuList::~LocalSearchWithTabuList() {
      GUM_DESTRUCTOR(LocalSearchWithTabuList);
    }

    /// copy operator
    INLINE LocalSearchWithTabuList& LocalSearchWithTabuList::
                                    operator=(const LocalSearchWithTabuList& from) {
      ApproximationScheme::operator=(from);
      __MaxNbDecreasing = from.__MaxNbDecreasing;
      return *this;
    }

    /// move operator
    INLINE LocalSearchWithTabuList& LocalSearchWithTabuList::
                                    operator=(LocalSearchWithTabuList&& from) {
      ApproximationScheme::operator=(std::move(from));
      __MaxNbDecreasing = std::move(from.__MaxNbDecreasing);
      return *this;
    }

    /// set the max number of changes decreasing the score that we allow to
    /// apply
    INLINE void LocalSearchWithTabuList::setMaxNbDecreasingChanges(Size nb) {
      __MaxNbDecreasing = nb;
    }

    /// returns the approximation policy of the learning algorithm
    INLINE ApproximationScheme& LocalSearchWithTabuList::approximationScheme() {
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
