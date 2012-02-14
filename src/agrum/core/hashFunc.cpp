/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *   test $Id: $                                                           *
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
 * @brief Outlined implementation of basic hash functions
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <climits>
#include <agrum/config.h>

#include <agrum/core/hashFunc.h>

#ifdef GUM_NO_INLINE
#include <agrum/core/hashFunc.inl>
#endif /* GUM_NO_INLINE */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* Hash functions are of the form [M * ((k * A) mod 1)], where [] stands for the
   * integer part, M is equal to the number of slots in the hashtable, k is the key
   * to be hashed, and mod 1 retrieves the decimal part of (k * A). A is an
   * irrational number (currently either the gold number or pi/4). To speed up
   * computations, the hash function is implemented using only unsigned longs.
   * Therefore pi/4 and the gold number are encoded as X * 2^{-n} where n is the
   * number of bits in an unsigned long. Consequently, we should adapt X's
   * definition to 32 and 64 bits architectures. */
  #if ULONG_MAX == 4294967295UL // unsigned long = 32 bits
  const Size   GUM_HASHTABLE_INT_GOLD  = 2654435769UL;
  const Size   GUM_HASHTABLE_INT_PI    = 3373259426UL;
  const Size   GUM_HASHTABLE_MASK      = 4294967295UL;
  const size_t GUM_HASHTABLE_OFFSET    = 32;
  /* for pedantic reasons, we remove these two lines:
  const unsigned long long GUM_HASHTABLE_LONG_GOLD = 11400714819323198486ULL;
  const unsigned long long GUM_HASHTABLE_LONG_PI   = 14488038916154245684ULL;
  */
  #else // unsigned long = 64 bits
  const Size   GUM_HASHTABLE_INT_GOLD  = 11400714819323198486UL;
  const Size   GUM_HASHTABLE_INT_PI    = 14488038916154245684UL;
  const Size   GUM_HASHTABLE_MASK      = 18446744073709551615UL;
  const size_t GUM_HASHTABLE_OFFSET    = 64;
  /* for pedantic reasons, we remove these two lines:
  const unsigned long long GUM_HASHTABLE_LONG_GOLD = 11400714819323198486ULL;
  const unsigned long long GUM_HASHTABLE_LONG_PI   = 14488038916154245684ULL;
  */
  #endif /* unsigned long = 32 bits */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
