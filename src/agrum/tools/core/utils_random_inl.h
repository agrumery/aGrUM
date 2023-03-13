/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
    std::uniform_int_distribution< Idx > uni_int(0, int(max) - 1);
    return uni_int(Generator_);
  }

  INLINE
  Idx randomValue(std::mt19937& generator, const Size max) {
    std::uniform_int_distribution< Idx > uni_int(0, int(max) - 1);
    return uni_int(generator);
  }

  INLINE
  double randomProba() {
    std::uniform_real_distribution<> uni_real(0.0, 1.0);
    return uni_real(Generator_);
  }


  INLINE
  void initRandom(unsigned int seed) {
    if (seed != 0) {
      Generator_.seed(seed);
    } else {
      Generator_.seed((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
    }
  }

  /// returns the current generator's value
  INLINE unsigned int currentRandomGeneratorValue() { return Generator_(); }

  // returns the aGrUM's seed used by the std::generators
  INLINE unsigned int randomGeneratorSeed() {
    return (unsigned int)((GUM_RANDOMSEED == 0)
                             ? std::chrono::system_clock::now().time_since_epoch().count()
                             : GUM_RANDOMSEED);
  }

  INLINE std::mt19937& randomGenerator() { return Generator_; }
} /* namespace gum */
