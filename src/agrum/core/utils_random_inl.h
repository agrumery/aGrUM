
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


/**
 * @file
 * @brief Contains useful methods for random stuff.
 *
 * @author Vincent RENAUDINEAU and Pierre-Henri WUILLEMIN
 */

// to ease IDE parser
#include <agrum/core/utils_random.h>
namespace gum {

  INLINE
  Idx randomValue(const Size max) { return (Idx)(rand() % max); }

  INLINE
  double randomProba() { return ((double)rand()) / ((double)RAND_MAX); }

  INLINE
  void initRandom(unsigned int seed) {
    if (seed) {
      srand(seed);
    } else {
      srand(randomGeneratorSeed());
    }
  }

} /* namespace gum */
