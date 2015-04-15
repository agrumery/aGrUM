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
 * @brief The greedy hill learning algorithm (for directed graphs)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/learning/greedyHillClimbing.h>

namespace gum {

  namespace learning {

    /// default constructor
    GreedyHillClimbing::GreedyHillClimbing() {
      setEpsilon(0);
      disableMinEpsilonRate();
      disableMaxIter();
      disableMaxTime();
      GUM_CONSTRUCTOR(learning::GreedyHillClimbing);
    }

    /// copy constructor
    GreedyHillClimbing::GreedyHillClimbing(const GreedyHillClimbing &from)
        : ApproximationScheme(from) {
      GUM_CONS_CPY(learning::GreedyHillClimbing);
    }

    /// move constructor
    GreedyHillClimbing::GreedyHillClimbing(GreedyHillClimbing &&from)
        : ApproximationScheme(std::move(from)) {
      GUM_CONS_MOV(learning::GreedyHillClimbing);
    }

    /// destructor
    GreedyHillClimbing::~GreedyHillClimbing() {
      GUM_DESTRUCTOR(learning::GreedyHillClimbing);
    }

    /// copy operator
    GreedyHillClimbing &GreedyHillClimbing::
    operator=(const GreedyHillClimbing &from) {
      ApproximationScheme::operator=(from);
      return *this;
    }

    /// move operator
    GreedyHillClimbing &GreedyHillClimbing::operator=(GreedyHillClimbing &&from) {
      ApproximationScheme::operator=(std::move(from));
      return *this;
    }

    /// returns the approximation policy of the learning algorithm
    ApproximationScheme &GreedyHillClimbing::approximationScheme() { return *this; }

  } /* namespace learning */

} /* namespace gum */
