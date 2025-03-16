/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





/**
 * @file
 * @brief Contains useful methods for random stuff.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES (_at_AMU)
 */

#ifndef GUM_UTILS_RANDOM_H
#define GUM_UTILS_RANDOM_H

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

#include <agrum/agrum.h>

namespace gum {
  namespace _rand_namespace_ {
    extern std::mt19937 Generator_;
  }

  /// @ingroup utilities_group
  /// @{

  /**
   * @brief Returns a random Idx between 0 and max-1 included.
   * @return Returns a random idx between 0 and max-1 included .
   * By default, max=2
   */
  Idx randomValue(const Size max = 2);

  /**
   * @brief Returns a random double between 0 and 1 included (i.e. a proba).
   * @return Returns a random double between 0 and 1 included (i.e. a proba).
   */
  double randomProba();

  /**
   * @brief Return a random discrete distribution.
   * @param n The number of modalities for the ditribution.
   * @return Return a random discrete distribution.
   * @tparam GUM_SCALAR The type used for representing probabilities.
   */
  template < typename GUM_SCALAR >
  std::vector< GUM_SCALAR > randomDistribution(Size n);

  /**
   * @brief Returns the aGrUM's seed used by the std::generators.
   * @return Returns the aGrUM's seed used by the std::generators.
   */
  unsigned int randomGeneratorSeed();

  /// returns the current generator's value
  unsigned int currentRandomGeneratorValue();

  /**
   * @brief Initialize random generator seed.
   */
  void initRandom(unsigned int seed = 0);

  /**
   * define a random_engine with correct seed
   */
  std::mt19937& randomGenerator();

  /// @}
} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/base/core/utils_random_inl.h>
#endif   // GUM_NO_INLINE

#include <agrum/base/core/utils_random_tpl.h>
#endif   /* GUM_UTILS_RANDOM_H */
