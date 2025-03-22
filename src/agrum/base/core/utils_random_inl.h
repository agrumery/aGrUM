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
 * @author  Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES (_at_AMU)
 */

// to ease IDE parser
#include <random>

#include <agrum/base/core/utils_random.h>

namespace gum {

  INLINE
  Idx randomValue(const Size max) {
    std::uniform_int_distribution< Idx > uni_int(0, int(max) - 1);
    return uni_int(_rand_namespace_::Generator_);
  }

  INLINE
  double randomProba() {
    std::uniform_real_distribution uni_real(0.0, 1.0);
    return uni_real(_rand_namespace_::Generator_);
  }

  INLINE
  void initRandom(unsigned int seed) {
    if (seed == 0) seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
    std::seed_seq seq{seed + 1, seed + 2, seed + 3, seed + 4, seed + 5};
    _rand_namespace_::Generator_.seed(seq);
  }

  /// returns the current generator's value
  INLINE unsigned int currentRandomGeneratorValue() { return _rand_namespace_::Generator_(); }

  // returns the aGrUM's seed used by the std::generators
  INLINE unsigned int randomGeneratorSeed() {
    return (unsigned int)((GUM_RANDOMSEED == 0)
                              ? std::chrono::system_clock::now().time_since_epoch().count()
                              : GUM_RANDOMSEED);
  }

  INLINE std::mt19937& randomGenerator() { return _rand_namespace_::Generator_; }

} /* namespace gum */
