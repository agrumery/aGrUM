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

#pragma once

/**
 * @file
 * @brief Contains useful methods for random stuff.
 *
 * @author Vincent RENAUDINEAU and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/core/utils_random.h>   // to ease IDE parser

namespace gum {

  // return a random discrete distribution
  // @param n is the number of modalities for the distribution
  template < GUM_Numeric GUM_SCALAR >
  std::vector< GUM_SCALAR > randomDistribution(Size n) {
    if (n < 2) n = 2;

    if (n == std::numeric_limits< Size >::max()) {
      GUM_ERROR(OutOfBounds, "randomDistribution: n is too large (SIZE_MAX would overflow n+1)")
    }

    // using sort method for uniformly distributed distributions
    // https://doi.org/10.1016/0377-2217(82)90161-8
    std::vector< GUM_SCALAR > v(n + 1);
    std::vector< GUM_SCALAR > res(n);
    v[0] = (GUM_SCALAR)0.0;
    v[n] = (GUM_SCALAR)1.0;
    for (Idx i = 1; i < n; i++) {
      v[i] = (GUM_SCALAR)randomProba();
    }
    std::sort(v.begin(), v.end());

    for (Idx i = 0; i < n; i++) {
      res[i] = v[i + 1] - v[i];
    }

    return res;
  }

  namespace _rand_namespace_ {
    template < typename GENERATOR >
    constexpr bool is32Bits() {
      return GENERATOR::min() == 0 && GENERATOR::max() == 0xFFFFFFFFu;
    }

    // a value of the standard normal distribution (Marsaglia's polar method)
    template < typename GENERATOR >
    double standardNormal(GENERATOR& generator) {
      double u, v, s;
      do {
        u = 2.0 * randomProba(generator) - 1.0;
        v = 2.0 * randomProba(generator) - 1.0;
        s = u * u + v * v;
      } while (s >= 1.0 || s == 0.0);
      return u * std::sqrt(-2.0 * std::log(s) / s);
    }
  }   // namespace _rand_namespace_

  template < typename GENERATOR >
  Idx randomValue(GENERATOR& generator, const Size max) {
    static_assert(_rand_namespace_::is32Bits< GENERATOR >(),
                  "a generator of 32-bit values is needed");
    if (max == 0) { GUM_ERROR(OutOfBounds, "randomValue: max must be > 0") }

    const auto bound = static_cast< std::uint64_t >(max);
    if (bound <= 0x100000000ull) {
      // the outputs above the largest multiple of bound are rejected (no modulo bias)
      const std::uint64_t limit = 0x100000000ull - 0x100000000ull % bound;
      std::uint64_t       draw;
      do {
        draw = static_cast< std::uint64_t >(generator());
      } while (draw >= limit);
      return static_cast< Idx >(draw % bound);
    }

    // 64 bits from two outputs; rest = 2^64 mod bound
    const std::uint64_t all  = std::numeric_limits< std::uint64_t >::max();
    const std::uint64_t rest = (all % bound + 1) % bound;
    std::uint64_t       draw;
    do {
      const auto high = static_cast< std::uint64_t >(generator());
      const auto low  = static_cast< std::uint64_t >(generator());
      draw            = (high << 32) | low;
    } while (rest != 0 && draw > all - rest);
    return static_cast< Idx >(draw % bound);
  }

  template < typename GENERATOR >
  double randomProba(GENERATOR& generator) {
    static_assert(_rand_namespace_::is32Bits< GENERATOR >(),
                  "a generator of 32-bit values is needed");
    // 53 bits (the precision of a double) from two outputs, drawn in this order
    const auto high = static_cast< std::uint64_t >(generator()) >> 5;
    const auto low  = static_cast< std::uint64_t >(generator()) >> 6;
    return (static_cast< double >(high) * 67108864.0 + static_cast< double >(low))
         * (1.0 / 9007199254740992.0);
  }

  template < typename RANDOM_IT >
  void randomShuffle(RANDOM_IT first, RANDOM_IT last) {
    randomShuffle(first, last, _rand_namespace_::generator());
  }

  template < typename RANDOM_IT, typename GENERATOR >
  void randomShuffle(RANDOM_IT first, RANDOM_IT last, GENERATOR& generator) {
    for (auto i = static_cast< Size >(last - first); i > 1; --i) {
      std::iter_swap(first + (i - 1), first + randomValue(generator, i));
    }
  }

  template < typename GENERATOR >
  double randomGamma(double shape, GENERATOR& generator) {
    if (!(shape > 0.0)) { GUM_ERROR(OutOfBounds, "randomGamma: shape must be > 0") }

    if (shape < 1.0) {
      // Gamma(shape) = Gamma(shape + 1) * U^(1/shape)
      const double gamma = randomGamma(shape + 1.0, generator);
      double       u;
      do {
        u = randomProba(generator);
      } while (u == 0.0);
      return gamma * std::pow(u, 1.0 / shape);
    }

    const double d = shape - 1.0 / 3.0;
    const double c = 1.0 / std::sqrt(9.0 * d);
    while (true) {
      double x, v;
      do {
        x = _rand_namespace_::standardNormal(generator);
        v = 1.0 + c * x;
      } while (v <= 0.0);
      v              = v * v * v;
      const double u = randomProba(generator);
      if (u < 1.0 - 0.0331 * (x * x) * (x * x)) return d * v;
      if (u > 0.0 && std::log(u) < 0.5 * x * x + d * (1.0 - v + std::log(v))) return d * v;
    }
  }


}   // namespace gum
