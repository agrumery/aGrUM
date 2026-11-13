/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
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

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

#include <agrum/agrum.h>

namespace gum {
  namespace _rand_namespace_ {
    /// The only generator of aGrUM (exported, so that all the shared modules use the same one).
    GUM_SHARED_PUBLIC std::mt19937& generator();
  }   // namespace _rand_namespace_

  /// @ingroup utilities_group
  /// @{

  /*
   * All the draws of aGrUM are computed from the raw outputs of the generator (std::mt19937, whose
   * sequence is fixed by the standard), never through the distributions of <random> (whose
   * algorithms depend on the standard library): a given seed gives the same draws on every
   * platform.
   *
   * The generator is shared by the whole library and is not thread safe: the functions which do not
   * take a generator must not be called from several threads at the same time. A multithreaded
   * algorithm gives each thread its own std::mt19937, seeded from this generator before the threads
   * start, and draws with the overloads which take a generator.
   */

  /**
   * @brief Returns a random Idx between 0 and max-1 included.
   * @return Returns a random idx between 0 and max-1 included .
   * By default, max=2
   * @throw OutOfBounds if max is 0
   */
  GUM_SHARED_PUBLIC Idx randomValue(const Size max = 2);

  /**
   * @brief Returns a random Idx between 0 and max-1 included, drawn with generator.
   * @tparam GENERATOR a generator of 32-bit values (such as std::mt19937)
   * @throw OutOfBounds if max is 0
   *
   * Unbiased (rejection sampling); one output of the generator if max <= 2^32, two otherwise.
   */
  template < typename GENERATOR >
  Idx randomValue(GENERATOR& generator, const Size max);

  /**
   * @brief Returns a random double in [0, 1) (i.e. a proba).
   * @return Returns a random double in [0, 1) (i.e. a proba), with a precision of 2^-53.
   */
  GUM_SHARED_PUBLIC double randomProba();

  /**
   * @brief Returns a random double in [0, 1), drawn with generator (two outputs of the generator).
   * @tparam GENERATOR a generator of 32-bit values (such as std::mt19937)
   */
  template < typename GENERATOR >
  double randomProba(GENERATOR& generator);

  /**
   * @brief Shuffles the range [first, last) (Fisher-Yates with randomValue).
   */
  template < typename RANDOM_IT >
  void randomShuffle(RANDOM_IT first, RANDOM_IT last);

  /**
   * @brief Shuffles the range [first, last) with generator (Fisher-Yates with randomValue).
   * @tparam GENERATOR a generator of 32-bit values (such as std::mt19937)
   */
  template < typename RANDOM_IT, typename GENERATOR >
  void randomShuffle(RANDOM_IT first, RANDOM_IT last, GENERATOR& generator);

  /**
   * @brief Returns a random value of the Gamma distribution of parameters (shape, 1).
   * @throw OutOfBounds if shape is not > 0
   *
   * Marsaglia and Tsang's method (ACM TOMS 26(3), 2000), with the boost U^(1/shape) for shape < 1.
   */
  GUM_SHARED_PUBLIC double randomGamma(double shape);

  /**
   * @brief Returns a random value of the Gamma distribution of parameters (shape, 1), drawn with
   * generator.
   * @tparam GENERATOR a generator of 32-bit values (such as std::mt19937)
   * @throw OutOfBounds if shape is not > 0
   */
  template < typename GENERATOR >
  double randomGamma(double shape, GENERATOR& generator);

  /**
   * @brief Return a random discrete distribution.
   * @param n The number of modalities for the ditribution.
   * @return Return a random discrete distribution.
   * @tparam GUM_SCALAR The type used for representing probabilities.
   */
  template < GUM_Numeric GUM_SCALAR >
  std::vector< GUM_SCALAR > randomDistribution(Size n);

  /**
   * @brief Returns the seed of the generator of aGrUM.
   * @return The seed given to the last call to initRandom() (the value taken from the clock if it
   * was 0): the key core.randomSeed at start, then what initRandom() received. 0 if initRandom()
   * was never called, i.e. in a program linked statically which does not use gum::Configuration.
   */
  GUM_SHARED_PUBLIC unsigned int randomGeneratorSeed();

  /// returns the current generator's value
  GUM_SHARED_PUBLIC unsigned int currentRandomGeneratorValue();

  /**
   * @brief Initialize random generator seed.
   *
   * This is the way to act directly on the generator at any time. At start, the library calls it
   * once with the key core.randomSeed of gum::Configuration; changing this key later has no effect.
   *
   * @param seed the seed; 0 for a seed taken from the clock.
   */
  GUM_SHARED_PUBLIC void initRandom(unsigned int seed = 0);

  /**
   * @brief The generator of aGrUM, shared by the whole library (not thread safe).
   *
   * To be used with the overloads of gum:: which take a generator (randomValue(), randomProba(),
   * randomShuffle(), randomGamma()...), not with the distributions of <random>, whose results
   * depend on the standard library: a given seed would no longer give the same draws on every
   * platform.
   */
  GUM_SHARED_PUBLIC std::mt19937& randomGenerator();

  /**
   * @class RandomGeneratorGuard
   * @brief Saves the state of the generator of aGrUM and restores it later.
   *
   * At construction, the guard saves the seed (randomGeneratorSeed()) and the state of the
   * generator. restore() (called by the destructor if it was not called before) gives them back:
   * the draws after restore() are the ones that would have followed the construction without the
   * draws made in between, and randomGeneratorSeed() returns the saved seed. A block of code can
   * thus call initRandom() and draw without changing the stream of the rest of the program.
   *
   * The generator is shared by the whole library and is not thread safe: the guard is meant for a
   * sequential block of code.
   */
  class GUM_SHARED_PUBLIC RandomGeneratorGuard {
    public:
    /// saves the seed and the state of the generator
    RandomGeneratorGuard();

    /// restores the seed and the state of the generator, if restore() was not called before
    ~RandomGeneratorGuard();

    RandomGeneratorGuard(const RandomGeneratorGuard&)            = delete;
    RandomGeneratorGuard(RandomGeneratorGuard&&)                 = delete;
    RandomGeneratorGuard& operator=(const RandomGeneratorGuard&) = delete;
    RandomGeneratorGuard& operator=(RandomGeneratorGuard&&)      = delete;

    /**
     * @brief restores the seed and the state saved at construction.
     *
     * Only the first call restores: the next calls (and the destructor) do nothing.
     */
    void restore();

    /// whether restore() was already called
    bool isRestored() const noexcept;

    private:
    unsigned int _seed_;
    std::mt19937 _state_;
    bool         _restored_;
  };

  /// @}
} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/base/core/utils_random_inl.h>
#endif   // GUM_NO_INLINE

#include <agrum/base/core/utils_random_tpl.h>
#endif   /* GUM_UTILS_RANDOM_H */
