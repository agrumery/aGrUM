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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
 * @brief Implementation of pow functions with integers, faster than stl
 * implementation.
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_MATH_POW_H
#define GUM_MATH_POW_H

#include <agrum/agrum.h>

namespace gum {

  /// @name Integers Pow utility methods
  /// @{

  /**
   * @brief Specialized pow function with integers (faster implementation).
   * @ingroup math_group
   *
   * @param base The constant unsigned long integer base used to compute \f$
   * base^{exponent} \f$.
   * @param exponent The unsigned long integer exponent used which will hold the
   * result afterward.
   */
  unsigned long intPow(unsigned long base, unsigned long exponent);

  /**
   * @brief Specialized base 2 pow function with integer.
   * @ingroup math_group
   *
   * @param exponent The unsigned long integer exponent used to compute \f$
   * 2^{exponent} \f$ which will hold the result of afterward.
   */
  unsigned long int2Pow(unsigned long exponent);

  /**
   * @brief Compute the superior and closest power of two of an integer.
   * @ingroup math_group
   *
   * Given an integer, compute it's - superior - and closest power of two, i.e.
   * the number of bits necessary to represent this integer as well as the
   * maximum integer that can be represented by those bits.
   *
   * @param card The constant unsigned long integer we wish to represent by
   * bits.
   * @param num_bits The unsigned long integer used as a "return" value to get
   * the minimum number of bits used to represend card.
   * @param new_card The unsigned long integer used as a "return" value to get
   * the maximum number those bits can represent, i.e. \f$ 2^{num\_bits} \f$.
   */
  void superiorPow(unsigned long card, unsigned long& num_bits, unsigned long& new_card);

  /// @}

}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/base/core/math/pow_inl.h>
#endif   // GUM_NO_INLINE

#endif   // GUM_MATH_POW_H
