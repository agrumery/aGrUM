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
 * @brief Sources for the ChiSquare class.
 *
 * @author Jean-Christophe MAGNAN
 */
// =========================================================================
#include <agrum/FMDP/learning/core/chiSquare.h>

#include <agrum/base/core/math/math_utils.h>

// =========================================================================


// constants used by Gary Perlman for his code for computing chi2 critical
// values

namespace gum {

  // ==========================================================================
  /// computes the probability of normal z value (used by the cache)
  // ==========================================================================
  double ChiSquare::_probaZValue_(double z) {
    //      ++nbZt;

    //      z = std::round(z * std::pow(10, 3)) / std::pow(10, 3);
    //      if( ! _ZCache_.exists(z) ){

    double y, x, w;

    if (z == 0.0) x = 0.0;
    else {
      y = 0.5 * fabs(z);

      if (y >= (_Z_MAX_ * 0.5)) x = 1.0;
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

    //         _ZCache_.insert(z, ( z > 0.0 ? (( x + 1.0 ) * 0.5 ) : (( 1.0 - x )
    //        * 0.5 ) ) );
    //      } else {
    //          ++nbZ;
    //        }

    //      return  _ZCache_[z];
    return (z > 0.0 ? ((x + 1.0) * 0.5) : ((1.0 - x) * 0.5));
  }

  // ==========================================================================
  /// computes the probability of chi2 value (used by the cache)
  // ==========================================================================
  double ChiSquare::probaChi2(double x, Size df) {
    double retVal = 0.0;
    //      ++nbChit;

    //      std::pair<double, unsigned long> conty(x, df);
    //      if( ! _chi2Cache_.exists(conty) ){

    double a, y = 0, s;
    double e, c, z;
    int    even; /* true if df is an even number */

    if ((x <= 0.0) || (df < 1)) {
      //           _chi2Cache_.insert(conty,1.0);
      retVal = 1.0;
    } else {
      a = 0.5 * x;

      even = (2 * (df / 2)) == df;

      if (df > 1) y = _exp_(-a);

      s = (even ? y : (2.0 * _probaZValue_(-sqrt(x))));

      if (df > 2) {
        x = 0.5 * (df - 1.0);
        z = (even ? 1.0 : 0.5);

        if (a > _BIGX_) {
          e = (even ? 0.0 : _LOG_SQRT_PI_);
          c = log(a);

          while (z <= x) {
            e = log(z) + e;
            s += _exp_(c * z - a - e);
            z += 1.0;
          }

          //               _chi2Cache_.insert(conty,s);
          retVal = s;

        } else {
          e = (even ? 1.0 : (_I_SQRT_PI_ / sqrt(a)));
          c = 0.0;

          while (z <= x) {
            e = e * (a / z);
            c = c + e;
            z += 1.0;
          }

          //               _chi2Cache_.insert(conty,( c * y + s ));
          retVal = (c * y + s);
        }
      } else {
        //             _chi2Cache_.insert(conty,s);
        retVal = s;
      }
    }
    //      } else {
    //          ++nbChi;
    //        }
    //      std::cout << "Z avoid : " << nbZ << " / " << nbZt << ". Chi avoid :
    //      " << nbChi << " / " << nbChit << "." << std::endl;
    //      return  _chi2Cache_[conty];
    return retVal;
  }

}   // End of namespace gum

// HashTable<std::pair<double, unsigned long>, double> ChiSquare:: _chi2Cache_;
// HashTable<double, double> ChiSquare:: _ZCache_;
// Idx ChiSquare::nbZ = 0;
// Idx ChiSquare::nbChi = 0;
// Idx ChiSquare::nbZt = 0;
// Idx ChiSquare::nbChit = 0;
