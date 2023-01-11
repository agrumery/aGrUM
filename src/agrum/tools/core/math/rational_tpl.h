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
 * @brief Class template used to approximate decimal numbers by rationals.
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// To help IDE Parsers
#include <agrum/agrum.h>
#include <agrum/tools/core/math/rational.h>

namespace gum {

  template < typename GUM_SCALAR >
  void Rational< GUM_SCALAR >::farey(int64_t&          numerator,
                                     int64_t&          denominator,
                                     const GUM_SCALAR& number,
                                     const int64_t&    den_max,
                                     const GUM_SCALAR& zero) {
    bool       isNegative = (number < 0) ? true : false;
    GUM_SCALAR pnumber    = (isNegative) ? -number : number;

    if (std::abs(pnumber - GUM_SCALAR(1.)) < zero) {
      numerator   = (isNegative) ? -1 : 1;
      denominator = 1;
      return;
    } else if (pnumber < zero) {
      numerator   = 0;
      denominator = 1;
      return;
    }

    int64_t a(0), b(1), c(1), d(1);
    double  mediant(0.0F);

    while (b <= den_max && d <= den_max) {
      mediant = (GUM_SCALAR)(a + c) / (GUM_SCALAR)(b + d);

      if (std::fabs(pnumber - mediant) < zero) {
        if (b + d <= den_max) {
          numerator   = (isNegative) ? -(a + c) : (a + c);
          denominator = b + d;
          return;
        } else if (d > b) {
          numerator   = (isNegative) ? -c : c;
          denominator = d;
          return;
        } else {
          numerator   = (isNegative) ? -a : a;
          denominator = b;
          return;
        }
      } else if (pnumber > mediant) {
        a = a + c;
        b = b + d;
      } else {
        c = a + c;
        d = b + d;
      }
    }

    if (b > den_max) {
      numerator   = (isNegative) ? -c : c;
      denominator = d;
      return;
    } else {
      numerator   = (isNegative) ? -a : a;
      denominator = b;
      return;
    }
  }   /// end of farey func

  template < typename GUM_SCALAR >
  void Rational< GUM_SCALAR >::continuedFracFirst(int64_t&          numerator,
                                                  int64_t&          denominator,
                                                  const GUM_SCALAR& number,
                                                  const double&     zero) {
    const GUM_SCALAR pnumber = (number > 0) ? number : -number;

    /// reciprocal over iterations
    GUM_SCALAR rnumber = pnumber;

    /// convergents
    std::vector< uint64_t > p({0, 1});
    std::vector< uint64_t > q({1, 0});

    /// quotients
    std::vector< uint64_t > a;

    uint64_t p_tmp, q_tmp;

    uint64_t n;
    double   delta, delta_tmp;

    /// we find all convergents until we found a best one
    /// since we look for a delta < zero, we can start looking for
    /// semi-convergents
    /// when we found a convergent with delta < zero, and look for the
    /// semi-convergents before
    while (true) {
      a.push_back(std::lrint(std::floor(rnumber)));
      p.push_back(a.back() * p.back() + p[p.size() - 2]);
      q.push_back(a.back() * q.back() + q[q.size() - 2]);

      delta = std::fabs(pnumber - (GUM_SCALAR)p.back() / q.back());

      if (delta < zero) {
        numerator = (int64_t)p.back();
        if (number < 0) numerator = -numerator;
        denominator = q.back();
        break;
      }

      if (std::abs(rnumber - a.back()) < 1e-6) break;

      rnumber = GUM_SCALAR(1.) / (rnumber - a.back());
    }   /// end of while

    if (a.size() < 2) return;

    /// we can start looking at the semi-convergents made of the last two
    /// convergents
    /// before the one within precision zero of number found previously
    Idx i = Idx(p.size() - 2);
    /// the last convergent has already been computed previously : end of for is
    /// p.size() - 2
    /// for ( ; i < p.size() - 1; ++i ) {
    // Test n = a[i-1]/2 ( when a[i-1] is even )
    n     = a[i - 1] / 2;
    p_tmp = n * p[i] + p[i - 1];
    q_tmp = n * q[i] + q[i - 1];

    delta     = std::fabs(pnumber - ((double)p[i]) / q[i]);
    delta_tmp = std::fabs(pnumber - ((double)p_tmp) / q_tmp);

    if (delta < zero) {
      numerator = (int64_t)p[i];
      if (number < 0) numerator = -numerator;
      denominator = q[i];
      return;
    }

    if (delta_tmp < zero) {
      numerator = (int64_t)p_tmp;
      if (number < 0) numerator = -numerator;
      denominator = q_tmp;
      return;
    }

    // next semi-convergents until next convergent from smaller denominator to
    // bigger
    // denominator
    for (n = (a[i - 1] + 2) / 2; n < a[i - 1]; ++n) {
      p_tmp = n * p[i] + p[i - 1];
      q_tmp = n * q[i] + q[i - 1];

      delta_tmp = std::fabs(pnumber - ((double)p_tmp) / q_tmp);

      if (delta_tmp < zero) {
        numerator = (int64_t)p_tmp;
        if (number < 0) numerator = -numerator;
        denominator = q_tmp;
        return;
      }
    }   /// end of for

    ///} // end of for
  }

  template < typename GUM_SCALAR >
  void Rational< GUM_SCALAR >::continuedFracBest(int64_t&          numerator,
                                                 int64_t&          denominator,
                                                 const GUM_SCALAR& number,
                                                 const int64_t&    den_max) {
    const GUM_SCALAR pnumber = (number > 0) ? number : -number;

    const uint64_t denMax = (uint64_t)den_max;   /// signed and unsigned comparison resolution ...

    /// reciprocal over iterations
    GUM_SCALAR rnumber = pnumber;

    /// convergents
    std::vector< uint64_t > p({0, 1});
    std::vector< uint64_t > q({1, 0});

    /// quotients
    std::vector< uint64_t > a;

    uint64_t p_tmp, q_tmp;

    uint64_t n;
    double   delta, delta_tmp;

    /// we find all convergents until we met den_max
    while (true) {
      a.push_back(std::lrint(std::floor(rnumber)));

      p_tmp = a.back() * p.back() + p[p.size() - 2];
      q_tmp = a.back() * q.back() + q[q.size() - 2];

      if (q_tmp > denMax || p_tmp > denMax) break;

      p.push_back(p_tmp);
      q.push_back(q_tmp);

      if (std::fabs(rnumber - a.back()) < 1e-6) break;

      rnumber = GUM_SCALAR(1.) / (rnumber - a.back());
    }   /// end of while

    if (a.size() < 2 || q.back() == denMax || p.back() == denMax) {
      numerator = p.back();
      if (number < 0) numerator = -numerator;
      denominator = q.back();
      return;
    }

    /// we can start looking at the semi-convergents made of the last two
    /// convergents
    /// before the one within precision zero of number found previously
    Idx i = Idx(p.size() - 1);

    /// the last convergent has already been computed previously : end of for is
    /// p.size() - 2
    /// for ( ; i < p.size() - 1; ++i ) {
    for (n = a[i - 1] - 1; n >= (a[i - 1] + 2) / 2; --n) {
      p_tmp = n * p[i] + p[i - 1];
      q_tmp = n * q[i] + q[i - 1];

      if (q_tmp > denMax || p_tmp > denMax) continue;

      numerator = (int64_t)p_tmp;
      if (number < 0) numerator = -numerator;
      denominator = q_tmp;
      return;
    }   // end of for

    // Test n = a[i-1]/2
    n     = a[i - 1] / 2;
    p_tmp = n * p[i] + p[i - 1];
    q_tmp = n * q[i] + q[i - 1];

    delta_tmp = std::fabs(pnumber - ((double)p_tmp) / q_tmp);
    delta     = std::fabs(pnumber - ((double)p[i]) / q[i]);

    if (delta_tmp < delta && q_tmp <= denMax && p_tmp <= denMax) {
      numerator = (int64_t)p_tmp;
      if (number < 0) numerator = -numerator;
      denominator = q_tmp;
    } else {
      numerator = (int64_t)p[i];
      if (number < 0) numerator = -numerator;

      denominator = q[i];
    }

    ///}
  }

}   // namespace gum
