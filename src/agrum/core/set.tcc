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
 * @brief Implementation of the Set
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <sstream>


namespace gum {


  template<typename KEY>
  Set<KEY>::Set( Size size ,  bool resize_policy ):
    __inside( size,resize_policy ),
    __it_end( SetIterator<KEY>( *this ) ) {
    GUM_CONSTRUCTOR( Set );
    __it_end.__initialize( __inside.end() );
  }

  template<typename KEY>
  Set<KEY>::Set( const HashTable<KEY,bool>& h ): __inside( h ) {
    GUM_CONSTRUCTOR( Set );
    __it_end.__initialize( __inside.end() );
  }

  template<typename KEY>
  Set<KEY>::Set( const Set<KEY>& s ) :
    __inside( s.__inside ),
    __it_end( *this ) {
    GUM_CONS_CPY( Set );
    __it_end.__initialize( __inside.end() );
  }

  template<typename KEY>
  Set<KEY>& Set<KEY>::operator=( const Set<KEY>& s ) {
    clear();
    resize( s.capacity() );
    setResizePolicy( s.getResizePolicy() );

    for ( SetIterator<KEY> it=s.begin();it!=s.end();++it ) {
      insert( *it );
    }

    __it_end.__initialize( __inside.end() );

    return *this;
  }

  template<typename KEY>
  Set<KEY>::Set::~Set() {
    GUM_DESTRUCTOR( Set );
  }

  template<typename KEY> INLINE
  bool Set<KEY>::contains( const KEY& k ) const {
    return __inside.exists( k );
  }

  template<typename KEY> INLINE
  void Set<KEY>::insert( const KEY& k ) {
    if ( ! contains( k ) ) {
      __inside.insert( k,true );
      __it_end.__initialize( __inside.end() );
    }
  }

  template<typename KEY> INLINE
  void Set<KEY>::erase( const KEY& k ) {
    if ( contains( k ) ) {
      __inside.erase( k );
      __it_end.__initialize( __inside.end() );
    }
  }

  template<typename KEY> INLINE
  void Set<KEY>::clear( ) {
    __inside.clear();
    __it_end.__initialize( __inside.end() );
  }

  template<typename KEY> INLINE
  Set<KEY>& Set<KEY>::operator<<( const KEY& k ) {
    insert( k );
    return ( *this );
  }

  template<typename KEY> INLINE
  Set<KEY>& Set<KEY>::operator>>( const KEY& k ) {
    erase( k );
    return ( *this );
  }

  template<typename KEY> INLINE
  Size Set<KEY>::size() const {
    return __inside.size();
  }

  template<typename KEY> INLINE
  bool Set<KEY>::empty( ) const {
    return __inside.empty();
  }

  template<typename KEY>
  bool Set<KEY>::operator==( const Set<KEY>& s2 ) const {
    const HashTable<KEY, bool>& h2=s2.__inside;

    if ( size() != h2.size() ) return false;

    for ( HashTableIterator<KEY, bool> iter = __inside.begin();
          iter != __inside.end(); ++iter ) {
      if ( ! h2.exists( iter.key() ) )
        return false;
    }

    return true;
  }

  template<typename KEY> INLINE
  bool Set<KEY>::operator!=( const Set<KEY>& s2 ) const {
    return !( operator==( s2 ) );
  }

  // ==============================================================================
  /// Intersection operator
  // ==============================================================================
  template<typename KEY>
  Set<KEY> Set<KEY>::operator*( const Set<KEY>& s2 ) const {
    const HashTable<KEY, bool>& h2=s2.__inside;
    HashTable<KEY, bool> h_r;

    if ( size() < h2.size() ) {
      for ( HashTableIterator<KEY, bool> iter = __inside.begin();
            iter != __inside.end(); ++iter ) {
        if ( h2.exists( iter.key() ) )
          h_r.insert( iter.key(), true );
      }
    }
    else {
      for ( HashTableIterator<KEY, bool> iter=h2.begin();
            iter!=h2.end(); ++iter ) {        
        if ( __inside.exists( iter.key() ) )
          h_r.insert( iter.key(), true );
      }
    }

    // for return value optimization
    return Set<KEY>(h_r);
  }

  // ==============================================================================
  /// Union operator
  // ==============================================================================
  template<typename KEY>
  Set<KEY> Set<KEY>::operator+( const Set<KEY>& s2 ) const {
    const HashTable<KEY, bool>& h2=s2.__inside;
    HashTable<KEY, bool> h_r;

    for ( HashTableIterator<KEY,bool> iter = __inside.begin();
          iter != __inside.end(); ++iter )
      h_r.insert( iter.key(), true );

    for ( HashTableIterator<KEY,bool> iter=h2.begin(); iter!=h2.end(); ++iter ) {
      if ( ! h_r.exists( iter.key() ) )
        h_r.insert( iter.key(), true );
    }
    
    // for return value optimization
    return Set<KEY>(h_r);
  }

  // ==============================================================================
  /// Disjunction operator
  // ==============================================================================
  template<typename KEY>
  Set<KEY> Set<KEY>::operator-( const Set<KEY>& s2 ) const {
    const HashTable<KEY, bool>& h2=s2.__inside;
    HashTable<KEY, bool> h_r;

    for ( HashTableIterator<KEY,bool> iter = __inside.begin();
          iter != __inside.end();++iter )
      if ( ! h2.exists( iter.key() ) )
        h_r.insert( iter.key(), true );

    // for return value optimization
    return Set<KEY>(h_r);
  }

  template<typename KEY> INLINE
  SetIterator<KEY> Set<KEY>::begin() const {
    SetIterator<KEY> k( *this );
    k.__initialize( __inside.begin() );
    return k;
  }

  template<typename KEY> INLINE
  const SetIterator<KEY> & Set<KEY>::end() const { return __it_end;}

  template<typename KEY> INLINE
  std::string Set<KEY>::toString() const {
    std::stringstream out;
    bool first=true;
    out<<"{";

    for ( iterator iter=begin();iter!=end();++iter ) {
      if ( first ) {
        out<<*iter;
        first=false;
      } else {
        out<<","<<*iter;
      }
    }

    out<<"}";

    std::string res;
    out>>res;
    return res;
  }

  template <typename Key> std::ostream& operator<<
  ( std::ostream& stream, const Set<Key>& set ) {
    stream<<set.toString();
    return stream;
  }

  //////////////////////////////////////////////////////////////////////////
  template<typename KEY> INLINE
  SetIterator<KEY>::SetIterator( ) :
    __set( NULL ) {
    GUM_CONSTRUCTOR( SetIterator );
  }

  template<typename KEY> INLINE
  SetIterator<KEY>::SetIterator( const Set<KEY>& from ) : __set( &from ) {
    GUM_CONSTRUCTOR( SetIterator );
  }

  template<typename KEY> INLINE
  SetIterator<KEY>::SetIterator( const SetIterator<KEY>& iter ) :
    __set( iter.__set ),__ht_iter( iter.__ht_iter ) {
    GUM_CONSTRUCTOR( SetIterator );
  }

  template<typename KEY> INLINE SetIterator<KEY>::~SetIterator() {
    GUM_DESTRUCTOR( SetIterator );
  }

  template<typename KEY> INLINE
  bool SetIterator<KEY>::operator!= ( const SetIterator<KEY> &from ) const {
    return __ht_iter!=from.__ht_iter;
  }

  template<typename KEY> INLINE
  bool SetIterator<KEY>::operator== ( const SetIterator<KEY> &from ) const {
    return __ht_iter==from.__ht_iter;
  }

  template<typename KEY> INLINE
  SetIterator<KEY>& SetIterator<KEY>::operator= ( const SetIterator<KEY> &from ) {
    __set=from.__set;
    __ht_iter=from.__ht_iter;
    return *this;
  }

  template<typename KEY> INLINE
  const KEY& SetIterator<KEY>::operator*() const {
    return __ht_iter.key();
  }

  template<typename KEY> INLINE
  const KEY* SetIterator<KEY>::operator->() const {
    return &( __ht_iter.key() );
  }

  template<typename KEY> INLINE
  SetIterator<KEY>& SetIterator<KEY>::operator++() {
    ++__ht_iter;
    return *this;
  }

  template<typename KEY> INLINE
  SetIterator<KEY>& SetIterator<KEY>::operator--() {
    --__ht_iter;
    return *this;
  }

  template<typename KEY> INLINE
  Size Set<KEY>::capacity() const {
    return __inside.capacity();
  }

  /// changes the size of the underlying hashtable
  template<typename KEY> INLINE
  void Set<KEY>::resize( Size new_size ) {
    __inside.resize( new_size );
  }

  /// enables the user to change dynamically the resizing policy of the
  /// underlying hashtable
  template<typename KEY> INLINE
  void Set<KEY>::setResizePolicy( const bool new_policy ) {
    __inside.setResizePolicy( new_policy );
  }

  /// returns the current resizing policy of the underlying hashtable
  template<typename KEY> INLINE
  bool Set<KEY>::getResizePolicy() const {
    return __inside.getResizePolicy();
  }

  template<typename KEY>
  template <typename ASSENT>
  HashTable<KEY,ASSENT>
  Set<KEY>::hashMap( ASSENT( *f )( const KEY& ), Size size ) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without resizing
    if ( size == 0 )
      size = std::max( 2UL, __inside.size() / 2 );

    // create a new table
    HashTable<KEY,ASSENT> table( size );

    // fill the new hash table
    for ( HashTableIterator<KEY,bool > iter = __inside.rbegin();
          iter != __inside.rend(); --iter ) {
      table.insert( iter.key(), f( iter.key() ) );
    }

    return table;
  }

  template<typename KEY>
  template <typename ASSENT>
  HashTable<KEY,ASSENT> Set<KEY>::hashMap( const ASSENT& val, Size size ) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without resizing
    if ( size == 0 )
      size = std::max( 2UL, __inside.size() / 2 );

    // create a new table
    HashTable<KEY,ASSENT> table( size );

    // fill the new hash table
    for ( HashTableIterator<KEY,bool > iter = __inside.rbegin();
          iter != __inside.rend(); --iter ) {
      table.insert( iter.key(), val );
    }

    return table;
  }

  /// a method to create a list of Assent from a node list
  // ==============================================================================
  template<typename KEY>
  template <typename ASSENT>
  List<ASSENT> Set<KEY>::listMap( ASSENT( *f )( const KEY& ) ) const {
    // create a new list
    List<ASSENT> list;
    // fill the new list

    for ( HashTableIterator<KEY,bool > iter = __inside.begin();
          iter != __inside.end(); ++iter ) {
      list.pushBack( f( iter.key() ) );
    }

    return list;
  }

  template<typename KEY> INLINE
  void SetIterator<KEY>::__initialize( const HashTableIterator<KEY,bool>& iter ) {
    __ht_iter=iter;
  }


} /* namespace gum */

  
#endif    // DOXYGEN_SHOULD_SKIP_THIS
