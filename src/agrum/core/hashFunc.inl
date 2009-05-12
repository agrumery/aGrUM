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
 * @brief Inlined implementation of the basic hash functions
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <string>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  // ==============================================================================
  /* in aGrUM, the sizes of hash tables (number of slots) are powers of 2. This is
   * not actually compulsory for the hash function we use. However, as it
   * speeds up the computations of hashed values, we chose to impose
   * this restriction. Function __hashTableLog2 thus returns the size in
   * bits - 1 necessary to store the smallest power of 2 greater than or
   * equal nb. */
  // ==============================================================================
  INLINE unsigned int __hashTableLog2( const Size& nb ) {
    unsigned int i = 0;
    for ( Size nbb = nb; nbb > 1UL; ++i, nbb >>= 1 ) {}; 
    return (( 1UL << i ) < nb ? i + 1UL : i );
  }

  
  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by strings
  // ==============================================================================
  INLINE Size
  HashFunc<std::string>::operator()( const std::string& key ) const  {
    register Size h = 0;
    for ( size_t i = 0, j = key.size(); i < j; ++i ) h = 19 * h + key[i];
    return (( h * GUM_HASHTABLE_INT_GOLD ) & hash_mask );
  }

  
  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by Debugs
  // ==============================================================================
  INLINE Size
  HashFunc<Debug>::operator()( const Debug& key ) const  {
    register Size h = 0;
    for ( size_t i = 0, j = key.size(); i < j; ++i ) h = 19 * h + key[i];
    return (( h * GUM_HASHTABLE_INT_GOLD ) & hash_mask );
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
