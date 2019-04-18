
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
 * @brief Base Class for all the algorithms producing a junction given a set
 * of cliques/subcliques resulting from a triangulation
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/graphs/algorithms/triangulations/junctionTreeStrategies/junctionTreeStrategy.h>

namespace gum {

  // default constructor
  JunctionTreeStrategy::JunctionTreeStrategy() {
    // for debugging purposes
    GUM_CONSTRUCTOR(JunctionTreeStrategy);
  }

  // copy constructor
  JunctionTreeStrategy::JunctionTreeStrategy(const JunctionTreeStrategy& from) :
      _triangulation(from._triangulation) {
    // for debugging purposes
    GUM_CONS_CPY(JunctionTreeStrategy);
  }

  // move constructor
  JunctionTreeStrategy::JunctionTreeStrategy(JunctionTreeStrategy&& from) :
      _triangulation(from._triangulation) {
    // for debugging purposes
    GUM_CONS_MOV(JunctionTreeStrategy);
  }

  // destructor
  JunctionTreeStrategy::~JunctionTreeStrategy() {
    // for debugging purposes
    GUM_DESTRUCTOR(JunctionTreeStrategy);
  }

  /** @brief assigns a new triangulation to the junction tree strategy
   * during a move construction */
  void
     JunctionTreeStrategy::moveTriangulation(StaticTriangulation* triangulation) {
    _triangulation = triangulation;
  }

} /* namespace gum */
