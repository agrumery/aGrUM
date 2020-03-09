
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
 * @brief The greedy hill learning algorithm (for directed graphs)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/BN/learning/greedyHillClimbing.h>

namespace gum {

  namespace learning {

    /// default constructor
    GreedyHillClimbing::GreedyHillClimbing() {
      setEpsilon(0);
      disableMinEpsilonRate();
      disableMaxIter();
      disableMaxTime();
      GUM_CONSTRUCTOR(GreedyHillClimbing);
    }

    /// copy constructor
    GreedyHillClimbing::GreedyHillClimbing(const GreedyHillClimbing& from) :
        ApproximationScheme(from) {
      GUM_CONS_CPY(GreedyHillClimbing);
    }

    /// move constructor
    GreedyHillClimbing::GreedyHillClimbing(GreedyHillClimbing&& from) :
        ApproximationScheme(std::move(from)) {
      GUM_CONS_MOV(GreedyHillClimbing);
    }

    /// destructor
    GreedyHillClimbing::~GreedyHillClimbing() {
      GUM_DESTRUCTOR(GreedyHillClimbing);
    }

    /// copy operator
    GreedyHillClimbing&
       GreedyHillClimbing::operator=(const GreedyHillClimbing& from) {
      ApproximationScheme::operator=(from);
      return *this;
    }

    /// move operator
    GreedyHillClimbing& GreedyHillClimbing::operator=(GreedyHillClimbing&& from) {
      ApproximationScheme::operator=(std::move(from));
      return *this;
    }

    /// returns the approximation policy of the learning algorithm
    ApproximationScheme& GreedyHillClimbing::approximationScheme() {
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
