/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 * @brief Inline implementation of pow functions with integers, faster than stl
 * implementation.
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// To help IDE Parsers
#include <agrum/base/core/math/pow.h>

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
