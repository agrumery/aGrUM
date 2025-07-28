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
 * @brief The class that represents the chi2 distribution
 *
 * The Chi2 class allows to easily compute critical values for the Chi2
 * distribution.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/core/math/chi2.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// constants used by Gary Perlman for his code for computing chi2 critical
// values
#  define GUM_Z_MAX       6.0                           // maximum meaningful z value
#  define GUM_CHI_EPSILON 0.000001                      // accuracy of critchi approximation
#  define GUM_CHI_MAX     99999.0                       // maximum chi square value
#  define GUM_LOG_SQRT_PI 0.5723649429247000870717135   // std::log (std::sqrt (pi))
#  define GUM_I_SQRT_PI   0.5641895835477562869480795   // 1 / std::sqrt (pi)
#  define GUM_BIGX        20.0                          // max value to represent exp (x)
#  define _gum_ex(x)      (((x) < -GUM_BIGX) ? 0.0 : std::exp(x))

#endif                                                  /* DOXYGEN_SHOULD_SKIP_THIS */

// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/base/core/math/chi2_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  // default constructor
  Chi2::Chi2(const std::vector< std::size_t >& var_modalities, double confidence_proba) :
      _modalities_(var_modalities),
      _confidence_proba_(confidence_proba) {   // for debugging purposes
    GUM_CONSTRUCTOR(Chi2);
  }

  // destructor
  Chi2::~Chi2() {
    // for debugging purposes
    GUM_DESTRUCTOR(Chi2);
  }

  // computes the probability of normal z value (used by the cache)
  double Chi2::_probaZValue_(double z) {
    double y, x, w;

    if (z == 0.0) x = 0.0;
    else {
      y = 0.5 * std::fabs(z);

      if (y >= (GUM_Z_MAX * 0.5)) x = 1.0;
      else if (y < 1.0) {
        w = y * y;
        x = ((((((((0.000124818987 * w - 0.001075204047) * w + 0.005198775019) * w - 0.019198292004)
                     * w
                 + 0.059054035642)
                    * w
                - 0.151968751364)
                   * w
               + 0.319152932694)
                  * w
              - 0.531923007300)
                 * w
             + 0.797884560593)
          * y * 2.0;
      } else {
        y -= 2.0;
        x = (((((((((((((-0.000045255659 * y + 0.000152529290) * y - 0.000019538132) * y
                       - 0.000676904986)
                          * y
                      + 0.001390604284)
                         * y
                     - 0.000794620820)
                        * y
                    - 0.002034254874)
                       * y
                   + 0.006549791214)
                      * y
                  - 0.010557625006)
                     * y
                 + 0.011630447319)
                    * y
                - 0.009279453341)
                   * y
               + 0.005353579108)
                  * y
              - 0.002141268741)
                 * y
             + 0.000535310849)
              * y
          + 0.999936657524;
      }
    }

    return (z > 0.0 ? ((x + 1.0) * 0.5) : ((1.0 - x) * 0.5));
  }

  // computes the probability of chi2 value (used by the cache)
  double Chi2::probaChi2(double x, Size df) {
    double a, y = 0, s;
    double e, c, z;
    int    even; /* true if df is an even number */

    if ((x <= 0.0) || (df < 1)) return (1.0);

    a = 0.5 * x;

    even = (2 * (df / 2)) == df;

    if (df > 1) y = _gum_ex(-a);

    s = (even ? y : (2.0 * _probaZValue_(-std::sqrt(x))));

    if (df > 2) {
      x = 0.5 * (df - 1.0);
      z = (even ? 1.0 : 0.5);

      if (a > GUM_BIGX) {
        e = (even ? 0.0 : GUM_LOG_SQRT_PI);
        c = std::log(a);

        while (z <= x) {
          e = std::log(z) + e;
          s += _gum_ex(c * z - a - e);
          z += 1.0;
        }

        return (s);
      } else {
        e = (even ? 1.0 : (GUM_I_SQRT_PI / std::sqrt(a)));
        c = 0.0;

        while (z <= x) {
          e = e * (a / z);
          c = c + e;
          z += 1.0;
        }

        return (c * y + s);
      }
    } else return (s);
  }

  // computes the critical value of a given chi2 test (used by the cache)
  double Chi2::_criticalValue_(double proba, Size df) {
    double minchisq = 0.0;
    double maxchisq = GUM_CHI_MAX;
    double chisqval;

    if (proba <= 0.0) return (maxchisq);
    else if (proba >= 1.0) return (0.0);

    chisqval = df / std::sqrt(proba); /* fair first value */

    while (maxchisq - minchisq > GUM_CHI_EPSILON) {
      if (probaChi2(chisqval, df) < proba) maxchisq = chisqval;
      else minchisq = chisqval;

      chisqval = (maxchisq + minchisq) * 0.5;
    }

    return (chisqval);
  }

} /* namespace gum */
