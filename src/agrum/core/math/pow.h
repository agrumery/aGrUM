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
 * @brief Implementation of pow functions with integers, faster than stl
 * implementation.
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_MATH_POW_H
#define GUM_MATH_POW_H

#include <agrum/config.h>

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
  unsigned long intPow( unsigned long base, unsigned long exponent );

  /**
   * @brief Specialized base 2 pow function with integer.
   * @ingroup math_group
   *
   * @param exponent The unsigned long integer exponent used to compute \f$
   * 2^{exponent} \f$ which will hold the result of afterward.
   */
  unsigned long int2Pow( unsigned long exponent );

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
  void superiorPow( unsigned long  card,
                    unsigned long& num_bits,
                    unsigned long& new_card );

  /// @}

}  // end of gum namespace

#ifndef GUM_NO_INLINE
#include <agrum/core/math/pow_inl.h>
#endif  // GUM_NO_INLINE

#endif  // GUM_MATH_POW_H
