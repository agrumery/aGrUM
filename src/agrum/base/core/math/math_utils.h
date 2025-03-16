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
 * @brief Useful macros for maths.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_MATH_H
#define GUM_MATH_H

#include <cmath>
#include <limits>

#ifndef M_LOG2E
#  define M_LOG2E 1.4426950408889634074 /* log_2 e */
#endif

#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

#ifndef M_LN2
#  define M_LN2 0.69314718055994530942 /* log_e 2 */
#endif

#ifndef GUM_LOG_SQRT_2PI               // std::log ( std::sqrt ( 2pi ) )
#  define GUM_LOG_SQRT_2PI .91893853320467274177
#endif

#define GUM_LOG2_OR_0(x) ((x == 0.0) ? 0.0 : std::log2((x)))

// redefined due to a strange bug in visual c++
namespace gum {
  template < typename T >
  bool isfinite(T arg) {
    if constexpr (std::numeric_limits< T >::has_infinity) {
      if constexpr (std::numeric_limits< T >::is_signed) {
        return arg == arg && arg != std::numeric_limits< T >::infinity()
            && arg != -std::numeric_limits< T >::infinity();   // neither Nan, infty nor -infty
      } else {
        return arg == arg
            && arg != std::numeric_limits< T >::infinity();    // neither Nan nor inftty
      }
    } else {
      return arg == arg;                                       // not Nan
    }
  }
}   // namespace gum

#endif /* GUM_MATH_H */
