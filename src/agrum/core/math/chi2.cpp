/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief The class that represents the chi2 distribution
 *
 * The Chi2 class allows to easily compute critical values for the Chi2
 * distribution
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/config.h>
#include <agrum/core/math/chi2.h>

// constants used by Gary Perlman for his code for computing chi2 critical values
#define GUM_Z_MAX 6.0            // maximum meaningful z value
#define GUM_CHI_EPSILON 0.000001 // accuracy of critchi approximation
#define GUM_CHI_MAX 99999.0      // maximum chi square value
#define GUM_LOG_SQRT_PI 0.5723649429247000870717135 // std::log (std::sqrt (pi))
#define GUM_I_SQRT_PI 0.5641895835477562869480795   // 1 / std::sqrt (pi)
#define GUM_BIGX 20.0                               // max value to represent exp (x)
#define gum__ex(x) (((x) < -GUM_BIGX) ? 0.0 : std::exp(x))

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/core/math/chi2.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  /// default constructor
  Chi2::Chi2(const std::vector<unsigned int> &var_modalities, float confidence_proba)
      : __modalities(var_modalities), __confidence_proba(confidence_proba) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Chi2);
  }

  /// destructor
  Chi2::~Chi2() {
    // for debugging purposes
    GUM_DESTRUCTOR(Chi2);
  }

  /// computes the probability of normal z value (used by the cache)
  double Chi2::__probaZValue(double z) {
    double y, x, w;

    if (z == 0.0)
      x = 0.0;
    else {
      y = 0.5 * std::fabs(z);

      if (y >= (GUM_Z_MAX * 0.5))
        x = 1.0;
      else if (y < 1.0) {
        w = y * y;
        x = ((((((((0.000124818987 * w - 0.001075204047) * w + 0.005198775019) * w -
                  0.019198292004) *
                     w +
                 0.059054035642) *
                    w -
                0.151968751364) *
                   w +
               0.319152932694) *
                  w -
              0.531923007300) *
                 w +
             0.797884560593) *
            y * 2.0;
      } else {
        y -= 2.0;
        x = (((((((((((((-0.000045255659 * y + 0.000152529290) * y -
                        0.000019538132) *
                           y -
                       0.000676904986) *
                          y +
                      0.001390604284) *
                         y -
                     0.000794620820) *
                        y -
                    0.002034254874) *
                       y +
                   0.006549791214) *
                      y -
                  0.010557625006) *
                     y +
                 0.011630447319) *
                    y -
                0.009279453341) *
                   y +
               0.005353579108) *
                  y -
              0.002141268741) *
                 y +
             0.000535310849) *
                y +
            0.999936657524;
      }
    }

    return (z > 0.0 ? ((x + 1.0) * 0.5) : ((1.0 - x) * 0.5));
  }

  /// computes the probability of chi2 value (used by the cache)
  double Chi2::__probaChi2(double x, unsigned long df) {
    double a, y = 0, s;
    double e, c, z;
    int even; /* true if df is an even number */

    if ((x <= 0.0) || (df < 1))
      return (1.0);

    a = 0.5 * x;

    even = (2 * (df / 2)) == df;

    if (df > 1)
      y = gum__ex(-a);

    s = (even ? y : (2.0 * __probaZValue(-std::sqrt(x))));

    if (df > 2) {
      x = 0.5 * (df - 1.0);
      z = (even ? 1.0 : 0.5);

      if (a > GUM_BIGX) {
        e = (even ? 0.0 : GUM_LOG_SQRT_PI);
        c = std::log(a);

        while (z <= x) {
          e = std::log(z) + e;
          s += gum__ex(c * z - a - e);
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
    } else
      return (s);
  }

  /// computes the critical value of a given chi2 test (used by the cache)
  double Chi2::__criticalValue(double proba, unsigned long df) {
    double minchisq = 0.0;
    double maxchisq = GUM_CHI_MAX;
    double chisqval;

    if (proba <= 0.0)
      return (maxchisq);
    else if (proba >= 1.0)
      return (0.0);

    chisqval = df / std::sqrt(proba); /* fair first value */

    while (maxchisq - minchisq > GUM_CHI_EPSILON) {
      if (__probaChi2(chisqval, df) < proba)
        maxchisq = chisqval;
      else
        minchisq = chisqval;

      chisqval = (maxchisq + minchisq) * 0.5;
    }

    return (chisqval);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
