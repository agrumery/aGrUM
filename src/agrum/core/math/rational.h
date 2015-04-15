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

#ifndef __GUM_RATIONAL__H__
#define __GUM_RATIONAL__H__

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <cmath>

namespace gum {

  /**
   * @class Rational Rational.h <agrum/core/Rational.h>
   * @brief Class template used to approximate decimal numbers by rationals
   *
   * @tparam GUM_SCALAR The floating type ( float, double, long double ... ) of the
   *number.
   * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
   */
  template <typename GUM_SCALAR> class Rational {
    public:
    /**
     * Test the algorithms iters times with random doubles.
     *
     * From this, only farey should be used with doubles in [0,1].
     *
     * @param iters Number of tests of each type ( numbers lower than 1 and greater
     *than 1 ).
     */
    static void testRationalAlgorithms(const unsigned int &iters = 10);

    /// @name Real approximation by rational
    /// @{

    /**
     * Find the rational close enough to a given ( decimal ) number in [-1,1] and
     *whose denominator is not higher than a given integer number.
     *
     * Because of the double constraint on precision and size of the denominator,
     *there is no guarantee on the precision of the approximation if \c den_max is
     *low and \c zero is high. Prefer the use of continued fractions.
     *
     * @param numerator The numerator of the rational.
     * @param denominator The denominator of the rational.
     * @param number The constant number we want to approximate using rationals.
     * @param den_max The constant highest authorized denominator. 1000000 by
     *default.
     * @param zero The positive value below which a number is considered zero. 1e-6
     *by default.
     */
    static void farey(long int &numerator, long int &denominator,
                      const GUM_SCALAR &number, const long int &den_max = 1000000,
                      const double &zero = 1e-6);

    /**
     * Find the first best rational approximation ( the one with the smallest
     *denminator such that no other rational with smaller denominator is a better
     *approx ) within precision zero to a given ( decimal ) number ( ANY number ).
     *
     * It gives the same answer than farey assuming \c zero is the same and den_max
     *is infinite. Use this functions because you are sure to get an approx within \c
     *zero of \c number.
     *
     * We look at the semi-convergents left of the last admissible convergent, if
     *any. They may be within the same precision and have a smaller denominator.
     *
     * @param numerator The numerator of the rational.
     * @param denominator The denominator of the rational.
     * @param number The constant number we want to approximate using rationals.
     * @param zero The positive value below which a number is considered zero. 1e-6
     *by default.
     */
    static void continuedFracFirst(long int &numerator, long int &denominator,
                                   const GUM_SCALAR &number,
                                   const double &zero = 1e-6);

    /**
     * Find the best rational approximation -- not the first -- to a given ( decimal
     *) number ( ANY number ) and whose denominator is not higher than a given
     *integer number.
     *
     * In this case, we look for semi-convergents at the right of the last admissible
     *convergent, if any. They are better approximations, but have higher
     *denominators.
     *
     * @param numerator The numerator of the rational.
     * @param denominator The denominator of the rational.
     * @param number The constant number we want to approximate using rationals.
     * @param den_max The constant highest authorized denominator. 1000000 by
     *default.
     */
    static void continuedFracBest(long int &numerator, long int &denominator,
                                  const GUM_SCALAR &number,
                                  const long int &den_max = 1000000);

    /// @}

    private:
  };

} // end of gum namespace

#include <agrum/core/math/rational.tcc>

#endif
