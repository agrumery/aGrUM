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
#include <agrum/config.h>

//to help IDE parser
#include <agrum/core/hashFunc.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                   GUM_HASH_FUNC_BASE IMPLEMENTATION                  ===*/
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// hash function for default size hash tables
  // ==============================================================================
  template <typename Key>
  HashFuncBase<Key>::HashFuncBase()  :
    _hash_size( 0 ), _hash_log2_size( 0 ), _hash_mask( 0 ) {
  }

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  template <typename Key>
  HashFuncBase<Key>::HashFuncBase( const HashFuncBase& from )  :
    _hash_size( from._hash_size ), _hash_log2_size( from._hash_log2_size ),
    _hash_mask( from._hash_mask ) {
  }

  // ==============================================================================
  /// assignment operator
  // ==============================================================================
  template <typename Key> HashFuncBase<Key>&
  HashFuncBase<Key>::operator= ( const HashFuncBase<Key>& from )  {
    _hash_size = from._hash_size;
    _hash_log2_size = from._hash_log2_size;
    _hash_mask = from._hash_mask;
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
    if ( new_size < 2 ) {
      GUM_ERROR( HashSize, "the size of the hashtable is too small" );
    }

    _hash_log2_size = __hashTableLog2( new_size );

    _hash_size = 1UL << _hash_log2_size;
    _hash_mask = _hash_size - 1;
  }

  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  template <typename Key> INLINE
  Size HashFuncBase<Key>::size() const  {
    return _hash_size;
  }




  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                 GUM_HASH_FUNC_SMALL_KEY IMPLEMENTATION               ===*/
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  template <typename Key>
  void HashFuncSmallKey<Key>::resize( Size new_size ) {
    HashFuncBase<Key>::resize( new_size );
    _right_shift = GUM_HASHTABLE_OFFSET - HashFuncBase<Key>::_hash_log2_size;
  }

  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by "small" integers
  // ==============================================================================
  template <typename Key> INLINE Size
  HashFuncSmallKey<Key>::operator()( const Key& key ) const  {
    return (( key * GUM_HASHTABLE_INT_GOLD ) >> _right_shift );
  }



  /* =========================================================================== */
  /* =========================================================================== */
  /* ===              GUM_HASH_FUNC_SMALL_CAST_KEY IMPLEMENTATION             ===*/
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// basic constructor
  // ==============================================================================
  template <typename Key> INLINE
  HashFuncSmallCastKey<Key>::HashFuncSmallCastKey() :
    HashFuncBase<Key>(),
    _small_key_mask ( ( 1UL << ( 8 * sizeof( Key ) ) ) - 1UL ) {
  }


  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  template <typename Key>
  void HashFuncSmallCastKey<Key>::resize( Size new_size ) {
    HashFuncBase<Key>::resize( new_size );
    _right_shift = GUM_HASHTABLE_OFFSET - HashFuncBase<Key>::_hash_log2_size;
  }

  
  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by "small" integers
  // ==============================================================================
  template <typename Key> INLINE Size
  HashFuncSmallCastKey<Key>::operator()( const Key& key ) const  {
    return ((( *(( unsigned long* )( &key ) ) & _small_key_mask ) *
             GUM_HASHTABLE_INT_GOLD ) >> _right_shift );
  }

  

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===             GUM_HASH_FUNC_MEDIUM_CAST_KEY IMPLEMENTATION             ===*/
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  template <typename Key>
  void HashFuncMediumCastKey<Key>::resize( Size new_size ) {
    HashFuncBase<Key>::resize( new_size );
    _right_shift = GUM_HASHTABLE_OFFSET - HashFuncBase<Key>::_hash_log2_size;
  }

  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by "small" integers
  // ==============================================================================
  template <typename Key> INLINE Size
  HashFuncMediumCastKey<Key>::operator()( const Key& key ) const  {
    return ( ( *(( unsigned long* )( &key ) ) *
               GUM_HASHTABLE_INT_GOLD ) >> _right_shift );
  }



  /* =========================================================================== */
  /* =========================================================================== */
  /* ===              GUM_HASH_FUNC_SMALL_KEY_PAIR IMPLEMENTATION            === */
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  template <typename Key1, typename Key2>
  void HashFuncSmallKeyPair<Key1,Key2>::resize( Size new_size ) {
    HashFuncBase< std::pair<Key1,Key2> >::resize( new_size );
    _right_shift = GUM_HASHTABLE_OFFSET -
      HashFuncBase< std::pair<Key1,Key2> >::_hash_log2_size;
  }

  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by pairs of "small" integers
  // ==============================================================================
  template <typename Key1, typename Key2> INLINE Size
  HashFuncSmallKeyPair<Key1,Key2>::operator()( const std::pair<Key1,Key2>& key )
    const  {
    return (( key.first * GUM_HASHTABLE_INT_GOLD +
              key.second * GUM_HASHTABLE_INT_PI ) >> _right_shift );
  }



  /* =========================================================================== */
  /* =========================================================================== */
  /* ===           GUM_HASH_FUNC_SMALL_CAST_KEY_PAIR IMPLEMENTATION          === */
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// basic constructor
  // ==============================================================================
  template <typename Key1, typename Key2> INLINE
  HashFuncSmallCastKeyPair<Key1,Key2>::HashFuncSmallCastKeyPair() :
    HashFuncBase< std::pair<Key1,Key2> >(),
    _small_key_mask1( ( 1UL << ( 8 * sizeof( Key1 ) ) ) - 1UL ),
    _small_key_mask2( ( 1UL << ( 8 * sizeof( Key2 ) ) ) - 1UL ) {
  }

  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  template <typename Key1, typename Key2>
  void HashFuncSmallCastKeyPair<Key1,Key2>::resize( Size new_size ) {
    HashFuncBase< std::pair<Key1,Key2> >::resize( new_size );
    _right_shift = GUM_HASHTABLE_OFFSET -
      HashFuncBase< std::pair<Key1,Key2> >::_hash_log2_size;
  }

  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by pairs of "small" integers
  // ==============================================================================
  template <typename Key1, typename Key2> INLINE Size
  HashFuncSmallCastKeyPair<Key1,Key2>::operator()
    ( const std::pair<Key1,Key2>& key ) const  {
    const Key1* const key1 = &( key.first );
    const Key2* const key2 = &( key.second );
    return ((( *(( unsigned long* ) key1 ) & _small_key_mask1 ) *
             GUM_HASHTABLE_INT_GOLD +
             ( *(( unsigned long* ) key2 ) & _small_key_mask2 ) *
             GUM_HASHTABLE_INT_PI ) >> _right_shift );
  }

  

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===           GUM_HASH_FUNC_MEDIUM_CAST_KEY_PAIR IMPLEMENTATION         === */
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  template <typename Key1, typename Key2>
  void HashFuncMediumCastKeyPair<Key1,Key2>::resize( Size new_size ) {
    HashFuncBase< std::pair<Key1,Key2> >::resize( new_size );
    _right_shift = GUM_HASHTABLE_OFFSET -
      HashFuncBase< std::pair<Key1,Key2> >::_hash_log2_size;
  }

  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by pairs of "small" integers
  // ==============================================================================
  template <typename Key1, typename Key2> INLINE Size
  HashFuncMediumCastKeyPair<Key1,Key2>::operator()
    ( const std::pair<Key1,Key2>& key ) const  {
    const Key1* const key1 = &( key.first );
    const Key2* const key2 = &( key.second );
    return (( *(( unsigned long* )( key1 ) ) * GUM_HASHTABLE_INT_GOLD +
              *(( unsigned long* )( key2 ) ) * GUM_HASHTABLE_INT_PI )
            >> _right_shift );
  }



  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                  GUM_HASH_FUNC_BIG_KEY IMPLEMENTATION                ===*/
  /* =========================================================================== */
  /* =========================================================================== */

  /*
  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  template <typename Key>
  void HashFuncBigKey<Key>::resize( Size new_size ) {
  HashFuncBase<Key>::resize( new_size );
  _right_shift = GUM_HASHTABLE_OFFSET - HashFuncBase<Key>::_hash_log2_size;
  }

  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by "large" integers
  // ==============================================================================
  template <typename Key> INLINE Size
  HashFuncBigKey<Key>::operator()( const Key& key ) const  {
  #if ULLONG_MAX == ULONG_MAX
  return ( reinterpret_cast<unsigned long>(key * GUM_HASHTABLE_INT_GOLD )
  >> _right_shift );
  #else
  return ( ( reinterpret_cast<Size>
  ( key * GUM_HASHTABLE_LONG_GOLD +
  ( reinterpret_cast<unsigned long>
  ( key * GUM_HASHTABLE_LONG_GOLD ) >> GUM_HASHTABLE_OFFSET ) ) >>
  _right_shift ) & GUM_HASHTABLE_MASK );
  #endif
  }
  */


  /* =========================================================================== */
  /* =========================================================================== */
  /* ===              GUM_HASH_FUNC_BIG_KEY_PAIR IMPLEMENTATION               ===*/
  /* =========================================================================== */
  /* =========================================================================== */

  /* for pedantic reasons, the following lines are removed (not iso c++)

  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  template <typename Key1, typename Key2>
  void HashFuncBigKeyPair<Key1,Key2>::resize( Size new_size ) {
  HashFuncBase< std::pair<Key1,Key2> >::resize( new_size );
  _right_shift = GUM_HASHTABLE_OFFSET -
  HashFuncBase< std::pair<Key1,Key2> >::_hash_log2_size;
  }

  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by pairs of "large" integers
  // ==============================================================================
  template <typename Key1, typename Key2> INLINE Size
  HashFuncBigKeyPair<Key1,Key2>::operator()( const std::pair<Key1,Key2>& key )
  const  {
  #if ULLONG_MAX == ULONG_MAX
  return (( key.first * GUM_HASHTABLE_INT_GOLD +
  key.second * GUM_HASHTABLE_INT_PI ) >> _right_shift );
  #else
  return ((( key.first * GUM_HASHTABLE_LONG_GOLD +
  key.second * GUM_HASHTABLE_LONG_PI +
  ( key.first * GUM_HASHTABLE_LONG_GOLD +
  key.second * GUM_HASHTABLE_LONG_PI ) >> GUM_HASHTABLE_OFFSET ) >>
  _right_shift ) & GUM_HASHTABLE_MASK );
  #endif
  } */

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
    return HashFunc<unsigned int*>::operator()( key.__refCountPtr() );
  }

  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */


