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
 * @brief Template implementation of the basic hash functions
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <agrum/core/debug.h>
#include <agrum/core/exceptions.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                   GUM_HASH_FUNC_BASE IMPLEMENTATION                   ===*/
  /* ============================================================================ */
  /* ============================================================================ */

  // ==============================================================================
  /// hash function for default size hash tables
  // ==============================================================================
  template <typename Key>
  HashFuncBase<Key>::HashFuncBase()  :
    hash_size( 0 ), hash_log2_size( 0 ), hash_mask( 0 ) {
  }

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  template <typename Key>
  HashFuncBase<Key>::HashFuncBase( const HashFuncBase& from )  :
    hash_size( from.hash_size ), hash_log2_size( from.hash_log2_size ),
    hash_mask( from.hash_mask ) {
  }

  // ==============================================================================
  /// assignment operator
  // ==============================================================================
  template <typename Key> HashFuncBase<Key>&
  HashFuncBase<Key>::operator= ( const HashFuncBase<Key>& from )  {
    hash_size = from.hash_size;
    hash_log2_size = from.hash_log2_size;
    hash_mask = from.hash_mask;
    return *this;
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template <typename Key>
  HashFuncBase<Key>::~HashFuncBase<Key> ()  {
  }

  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  template <typename Key>
  void HashFuncBase<Key>::resize( Size new_size )  {
    // things work properly only for hashtables with at least 2 elements
    if ( new_size < 2 )
      GUM_ERROR( HashSize, "the size of the hashtable is too small" );

    hash_log2_size = __hashTableLog2( new_size );
    hash_size = 1UL << hash_log2_size;
    hash_mask = hash_size - 1;
  }

  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  template <typename Key> INLINE
  Size HashFuncBase<Key>::size() const  {
    return hash_size;
  }



  
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                 GUM_HASH_FUNC_SMALL_KEY IMPLEMENTATION                ===*/
  /* ============================================================================ */
  /* ============================================================================ */

  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  template <typename Key>
  void HashFuncSmallKey<Key>::resize( Size new_size ) {
    HashFuncBase<Key>::resize( new_size );
    right_shift = GUM_HASHTABLE_OFFSET - HashFuncBase<Key>::hash_log2_size;
  }

  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by "small" integers
  // ==============================================================================
  template <typename Key> INLINE Size
  HashFuncSmallKey<Key>::operator()( const Key& key ) const  {
    return (( key * GUM_HASHTABLE_INT_GOLD ) >> right_shift );
  }



  
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===              GUM_HASH_FUNC_SMALL_KEY_PAIR IMPLEMENTATION             === */
  /* ============================================================================ */
  /* ============================================================================ */

  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  template <typename Key1, typename Key2>
  void HashFuncSmallKeyPair<Key1,Key2>::resize( Size new_size ) {
    HashFuncBase< std::pair<Key1,Key2> >::resize( new_size );
    right_shift = GUM_HASHTABLE_OFFSET -
      HashFuncBase< std::pair<Key1,Key2> >::hash_log2_size;
  }

  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by pairs of "small" integers
  // ==============================================================================
  template <typename Key1, typename Key2> INLINE Size
  HashFuncSmallKeyPair<Key1,Key2>::operator()( const std::pair<Key1,Key2>& key )
    const  {
    return (( key.first * GUM_HASHTABLE_INT_GOLD +
              key.second * GUM_HASHTABLE_INT_PI ) >> right_shift );
  }

  

  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                  GUM_HASH_FUNC_BIG_KEY IMPLEMENTATION                 ===*/
  /* ============================================================================ */
  /* ============================================================================ */

  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  //for pedantic  template <typename Key>
  //for pedantic  void HashFuncBigKey<Key>::resize( Size new_size ) {
    //for pedantic  HashFuncBase<Key>::resize( new_size );
    //for pedantic  right_shift = GUM_HASHTABLE_OFFSET - HashFuncBase<Key>::hash_log2_size;
  //for pedantic  }

  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by "large" integers
  // ==============================================================================
//for pedantic    template <typename Key> INLINE Size
  //for pedantic  HashFuncBigKey<Key>::operator()( const Key& key ) const  {
  //for pedantic  #if ULLONG_MAX == ULONG_MAX
    //for pedantic  return (( key * GUM_HASHTABLE_INT_GOLD ) >> right_shift );
  //for pedantic  #else
    //for pedantic  return ((( key * GUM_HASHTABLE_LONG_GOLD +
               //for pedantic  ( key * GUM_HASHTABLE_LONG_GOLD ) >> GUM_HASHTABLE_OFFSET ) >>
             //for pedantic  right_shift ) & GUM_HASHTABLE_MASK );
  //for pedantic  #endif
  //for pedantic  }


  
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===               GUM_HASH_FUNC_BIG_KEY_PAIR IMPLEMENTATION               ===*/
  /* ============================================================================ */
  /* ============================================================================ */
  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  //for pedantic  template <typename Key1, typename Key2>
  //for pedantic  void HashFuncBigKeyPair<Key1,Key2>::resize( Size new_size ) {
    //for pedantic  HashFuncBase< std::pair<Key1,Key2> >::resize( new_size );
    //for pedantic  right_shift = GUM_HASHTABLE_OFFSET -
      //for pedantic  HashFuncBase< std::pair<Key1,Key2> >::hash_log2_size;
  //for pedantic  }

  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by pairs of "large" integers
  // ==============================================================================
  //for pedantic
 /*template <typename Key1, typename Key2> INLINE Size
  HashFuncBigKeyPair<Key1,Key2>::operator()( const std::pair<Key1,Key2>& key )
    const  {
  #if ULLONG_MAX == ULONG_MAX
    return (( key.first * GUM_HASHTABLE_INT_GOLD +
              key.second * GUM_HASHTABLE_INT_PI ) >> right_shift );
  #else
    return ((( key.first * GUM_HASHTABLE_LONG_GOLD +
               key.second * GUM_HASHTABLE_LONG_PI +
               ( key.first * GUM_HASHTABLE_LONG_GOLD +
                 key.second * GUM_HASHTABLE_LONG_PI ) >> GUM_HASHTABLE_OFFSET ) >>
             right_shift ) & GUM_HASHTABLE_MASK );
  #endif
  }*/

  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by a pointer
  // ==============================================================================
  template <typename Type> INLINE Size
  HashFunc<Type*>::operator()( Type* const & key ) const  {
    return HashFunc<unsigned long>::operator()(( unsigned long )key );
  }

  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by RefPtr<Type>
  // ==============================================================================
  template <typename Type> INLINE Size
  HashFunc< RefPtr<Type> >::operator()( const RefPtr<Type> & key ) const {
    return HashFunc<Type*>::operator()(( Type * )(( void* ) &( *( key ) ) ) );
  }

  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
