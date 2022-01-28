/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
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
 * @author Vincent RENAUDINEAU and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// to ease IDE parser
#include <agrum/tools/core/utils_random.h>
#include <random>

namespace gum {

  namespace {
    static std::mt19937 Generator_;
  }

  INLINE
  Idx randomValue(const Size max) {
    std::uniform_int_distribution<Idx> uni_int(0, max - 1);
    return uni_int(Generator_);
  }

  INLINE
  double randomProba() {
    std::uniform_real_distribution<> uni_real(0.0, 1.0);
    return uni_real(Generator_);
  }


  INLINE
  void initRandom(unsigned int seed) {
    if (seed) {
      Generator_ = std::mt19937(seed);
    } else {
      Generator_ = std::mt19937(randomGeneratorSeed());
    }
  }

} /* namespace gum */
