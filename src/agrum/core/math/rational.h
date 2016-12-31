/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN and Christophe GONZALES  *
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

/**
 * @file
 * @brief Class template used to approximate decimal numbers by rationals
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_RATIONAL_H
#define GUM_RATIONAL_H

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <vector>

// 64 bits for windows (long is 32 bits)
#ifdef _MSC_VER
typedef __int64          int64_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

namespace gum {

  /**
   * @class Rational rational.h <agrum/core/math/rational.h>
   * @brief Class template used to approximate decimal numbers by rationals.
   * @ingroup math_group
   *
   * @tparam GUM_SCALAR The floating type ( float, double, long double ... ) of
   * the number.
   */
  template <typename GUM_SCALAR>
  class Rational {
    public:
    // ========================================================================
    /// @name Real approximation by rational
    // ========================================================================
    /// @{

    /**
     * @brief Find the rational close enough to a given ( decimal ) number in
     * [-1,1] and whose denominator is not higher than a given integer number.
     *
     * Because of the double constraint on precision and size of the
     * denominator, there is no guarantee on the precision of the approximation
     * if \c den_max is low and \c zero is high. Prefer the use of continued
     * fractions.
     *
     * @param numerator The numerator of the rational.
     * @param denominator The denominator of the rational.
     * @param number The constant number we want to approximate using rationals.
     * @param den_max The constant highest authorized denominator. 1000000 by
     * default.
     * @param zero The positive value below which a number is considered zero.
     * 1e-6 by default.
     */
    static void farey( int64_t&          numerator,
                       int64_t&          denominator,
                       const GUM_SCALAR& number,
                       const int64_t&    den_max = 1000000L,
                       const GUM_SCALAR& zero = 1e-6 );

    /**
     * @brief Find the first best rational approximation.
     *
     * The one with the smallest denominator such that no other rational with
     * smaller denominator is a better approx,  within precision zero to a
     * given ( decimal ) number ( ANY number).
     *
     * It gives the same answer than farey assuming \c zero is the same and
     * den_max is infinite. Use this functions because you are sure to get an
     * approx within \c zero of \c number.
     *
     * We look at the semi-convergents left of the last admissible convergent,
     * if any. They may be within the same precision and have a smaller
     * denominator.
     *
     * @param numerator The numerator of the rational.
     * @param denominator The denominator of the rational.
     * @param number The constant number we want to approximate using rationals.
     * @param zero The positive value below which a number is considered zero.
     * 1e-6 by default.
     */
    static void continuedFracFirst( int64_t&          numerator,
                                    int64_t&          denominator,
                                    const GUM_SCALAR& number,
                                    const double&     zero = 1e-6 );

    /**
     * @brief Find the best rational approximation.
     *
     * Not the first, to a given ( decimal) number ( ANY number ) and whose
     * denominator is not higher than a given integer number.
     *
     * In this case, we look for semi-convergents at the right of the last
     * admissible convergent, if any. They are better approximations, but have
     * higher denominators.
     *
     * @param numerator The numerator of the rational.
     * @param denominator The denominator of the rational.
     * @param number The constant number we want to approximate using rationals.
     * @param den_max The constant highest authorized denominator. 1000000 by
     * default.
     */
    static void continuedFracBest( int64_t&          numerator,
                                   int64_t&          denominator,
                                   const GUM_SCALAR& number,
                                   const int64_t&    den_max = 1000000 );

    /// @}
  };

}  // end of gum namespace


extern template class gum::Rational<float>;
extern template class gum::Rational<double>;
extern template class gum::Rational<long double>;


// Always include template implementation in header file
#include <agrum/core/math/rational_tpl.h>

#endif  // GUM_RATIONAL_H
