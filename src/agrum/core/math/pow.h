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

#ifndef __GUM_POW__H__
#define __GUM_POW__H__

/**
 * @file
 * @brief Overload of pow functions with integers, faster than stl implementation
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
 */

namespace gum {

  /// @name Integers Pow utility methods
  /// @{

  /**
   * Specialized pow function with integers (faster implementation).
   * @param base The constant unsigned long integer base used to compute \f$
   * base^{exponent} \f$.
   * @param exponent The unsigned long integer exponent used which will hold the
   * result afterward.
   */
  inline void intPow(const unsigned long &base, unsigned long &exponent) {
    if (exponent == 0) {
      exponent = 1;
      return;
    }

    unsigned long out = base;

    for (unsigned long i = 1; i < exponent; i++)
      out *= base;

    exponent = out;
  }

  /**
   * Specialized base 2 pow function with integer.
   *
   * @param exponent The unsigned long integer exponent used to compute \f$
   *2^{exponent} \f$ which will hold the result of afterward.
   */
  inline void int2Pow(unsigned long &exponent) {
    unsigned long base = 1;
    base <<= exponent;
    exponent = base;
  }

  /**
   * Given an integer, compute it's - superior - and closest power of two, i.e. the
   *number of bits necessary to represent this integer as well as the maximum integer
   *that can be represented by those bits.
   *
   * @param card The constant unsigned long integer we wish to represent by bits.
   * @param num_bits The unsigned long integer used as a "return" value to get the
   *minimum number of bits used to represend card.
   * @param new_card The unsigned long integer used as a "return" value to get the
   *maximum number those bits can represent, i.e. \f$ 2^{num\_bits} \f$.
   */
  inline void superiorPow(const unsigned long &card, unsigned long &num_bits,
                          unsigned long &new_card) {
    if (card <= 0) {
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

  /// @}

} // end of gum namespace

#endif
