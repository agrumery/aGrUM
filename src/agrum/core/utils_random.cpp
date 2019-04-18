
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
 * @brief Contains usefull methods for random stuff.
 *
 * @author Vincent RENAUDINEAU and Pierre-Henri WUILLEMIN
 */
#include <chrono>

#include <agrum/agrum.h>
#include <agrum/core/utils_misc.h>

#ifdef GUM_NO_INLINE
#  include <agrum/core/utils_random_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  // returns the aGrUM's seed used by the std::generators
  unsigned int randomGeneratorSeed() {
    return (unsigned int)((GUM_RANDOMSEED == 0) ? std::chrono::system_clock::now()
                                                     .time_since_epoch()
                                                     .count()
                                                : GUM_RANDOMSEED);
  }


  std::default_random_engine getRandomGenerator(unsigned int seed) {
    std::default_random_engine generator;
    generator.seed(seed ? seed : randomGeneratorSeed());
    return generator;
  }

} /* namespace gum */
