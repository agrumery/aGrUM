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
 * @brief Inline implementation of pow functions with integers, faster than stl
 * implementation.
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// To help IDE Parsers
#include <agrum/tools/core/math/pow.h>

namespace gum {
  //@beforeMerging is this really faster ?
  // Specialized pow function with integers (faster implementation).
  INLINE unsigned long intPow(unsigned long base, unsigned long exponent) {
    if (exponent == 0) { return 1UL; }

    unsigned long out = base;

    for (unsigned long i = 1; i < exponent; i++)
      out *= base;

    return out;
  }

  //@beforeMerging is this really faster ?
  // Specialized base 2 pow function with integer.
  INLINE unsigned long int2Pow(unsigned long exponent) { return 1UL << exponent; }

  //@beforeMerging is this really faster ?
  // Given an integer, compute it's - superior - and closest power of two, i.e.
  // the number of bits necessary to represent this integer as well as the
  // maximum integer that can be represented by those bits.
  INLINE void superiorPow(unsigned long card, unsigned long& num_bits, unsigned long& new_card) {
    if (card == 0) {
      num_bits = 0;
      new_card = 1;
      return;
    }

    num_bits = 1;
    new_card = 2;

    while (new_card < card) {
      new_card *= 2;
      num_bits++;
    }
  }

}   // namespace gum
