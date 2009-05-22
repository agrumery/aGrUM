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

  /* =========================================================================== */
  /* ===                             SET ITERATORS                           === */
  /* =========================================================================== */

  /// default constructor: the iterator points toward nothing
  template<typename KEY> INLINE
  SetIterator<KEY>::SetIterator( ) {
    GUM_CONSTRUCTOR( SetIterator );
  }

  
  /// creates an iterator for a given set
  template<typename KEY> INLINE
  SetIterator<KEY>::SetIterator( const Set<KEY>& set, Position pos ) :
    __ht_iter ( pos == GUM_SET_ITERATOR_END ? set.__inside.end() :
                pos == GUM_SET_ITERATOR_BEGIN ? set.__inside.begin() :
                pos == GUM_SET_ITERATOR_RBEGIN ? set.__inside.rbegin() :
                set.__inside.rend() ) {
    GUM_CONSTRUCTOR( SetIterator );
  }

  
  /// copy constructor
  template<typename KEY> INLINE
  SetIterator<KEY>::SetIterator( const SetIterator<KEY>& iter ) :
    __ht_iter( iter.__ht_iter ) {
    GUM_CONSTRUCTOR( SetIterator );
  }

  
  /// destructor
  template<typename KEY> INLINE SetIterator<KEY>::~SetIterator() {
    GUM_DESTRUCTOR( SetIterator );
  }


  /// indicates whether two iterators point to different elements or sets
  template<typename KEY> INLINE
  bool SetIterator<KEY>::operator!= ( const SetIterator<KEY> &from ) const {
    return __ht_iter!=from.__ht_iter;
  }
  

  /// indicates whether two iterators point toward the same element of a same set
  template<typename KEY> INLINE
  bool SetIterator<KEY>::operator== ( const SetIterator<KEY> &from ) const {
    return __ht_iter==from.__ht_iter;
  }

  
  /// assignment operator
  template<typename KEY> INLINE
  SetIterator<KEY>& SetIterator<KEY>::operator= ( const SetIterator<KEY> &from ) {
    __ht_iter=from.__ht_iter;
    return *this;
  }

  
  /// returns the element pointed to by the iterator
  template<typename KEY> INLINE
  const KEY& SetIterator<KEY>::operator*() const {
    // note that, if the hashtable's iterator points toward nothing, it will
    // raise an UndefinedIteratorValue exception
    return __ht_iter.key();
  }

  
  /// returns aointer to the element pointed to by the iterator
  template<typename KEY> INLINE
  const KEY* SetIterator<KEY>::operator->() const {
    // note that, if the hashtable's iterator points toward nothing, it will
    // raise an UndefinedIteratorValue exception
    return &( __ht_iter.key() );
  }

  
  /// increments the iterator
  template<typename KEY> INLINE
  SetIterator<KEY>& SetIterator<KEY>::operator++() {
    // note that, if the hashtable's iterator points toward nothing, the
    // hashtable's iterator incrementation will do nothing. In particular, it
    // will not segfault.
    ++__ht_iter;
    return *this;
  }

  
  /// decrements the iterator
  template<typename KEY> INLINE
  SetIterator<KEY>& SetIterator<KEY>::operator--() {
    // note that, if the hashtable's iterator points toward nothing, the
    // hashtable's iterator decrementation will do nothing. In particular, it
    // will not segfault.
    --__ht_iter;
    return *this;
  }


  /// a function to update end/rend iterators
  template<typename KEY> INLINE
  void
  SetIterator<KEY>::__updatePosition( const HashTableIterator<KEY,bool>& iter ) {
    __ht_iter=iter;
  }








  /* =========================================================================== */
  /* ===                                 SETS                                === */
  /* =========================================================================== */
  
  /// default constructor
  template<typename KEY>
  Set<KEY>::Set( Size capacity ,  bool resize_policy ):
    __inside( capacity, resize_policy ),
    __it_end( *this, SetIterator<KEY>::GUM_SET_ITERATOR_END ),
    __it_rend ( *this, SetIterator<KEY>::GUM_SET_ITERATOR_REND ) {
    GUM_CONSTRUCTOR( Set );
  }
  

  /// convert a hashtable into a set of keys
  template<typename KEY>
  Set<KEY>::Set( const HashTable<KEY,bool>& h ):
    __inside( h ),
    __it_end( *this, SetIterator<KEY>::GUM_SET_ITERATOR_END ),
    __it_rend ( *this, SetIterator<KEY>::GUM_SET_ITERATOR_REND ) {
    GUM_CONSTRUCTOR( Set );
  }

  
  /// copy constructor
  template<typename KEY>
  Set<KEY>::Set( const Set<KEY>& s ) :
    __inside( s.__inside ),
    __it_end( *this, SetIterator<KEY>::GUM_SET_ITERATOR_END ),
    __it_rend ( *this, SetIterator<KEY>::GUM_SET_ITERATOR_REND ) {
    GUM_CONS_CPY( Set );
  }


  /// destructor
  template<typename KEY>
  Set<KEY>::Set::~Set() {
    GUM_DESTRUCTOR( Set );
  }


  /// the usual begin iterator to parse the set
  template<typename KEY> INLINE
  SetIterator<KEY> Set<KEY>::begin() const {
    return SetIterator<KEY>( *this );
  }
  
  
  /// the usual end iterator to parse the set
  template<typename KEY> INLINE
  const SetIterator<KEY>& Set<KEY>::end() const {
    return __it_end;
  }

  
  /// the usual rbegin iterator to parse the set
  template<typename KEY> INLINE
  SetIterator<KEY> Set<KEY>::rbegin() const {
    return SetIterator<KEY>( *this, SetIterator<KEY>::GUM_SET_ITERATOR_RBEGIN );
  }


  /// the usual rend iterator to parse the set
  template<typename KEY> INLINE
  const SetIterator<KEY> & Set<KEY>::rend() const {
    return __it_rend;
  }

  
  /// removes all the elements, if any, from the set
  template<typename KEY> INLINE
  void Set<KEY>::clear( ) {
    // first we remove all the elements from the hashtable actually containing
    // the elements of the set. Note that, doing so, all the hashtable iterators
    // will be updated as well. In turn, this will imply that, whenever an
    // operation will be performed on a SetIterator, this will raise an exception.
    __inside.clear();

    // now we shall update the end iterator. Note that actually there is no need
    // to update the rend iterator as this one is not affected by changes within
    // hashtables (adding/deleting elements). Hence, for speedup, we do not
    // update the rend iterator
    __it_end.__updatePosition( __inside.end() );
  }

  
  /// copy operator
  template<typename KEY>
  Set<KEY>& Set<KEY>::operator=( const Set<KEY>& s ) {
    // avoid self assignment
    if (&s != this) {
      // remove the old content of the set. Actually, we remove all the elements
      // from the underlying hashtable. Note that, doing so, all the hashtable
      // iterators will be updated as well. In turn, this will imply that, whenever
      // an operation will be performed on a SetIterator, this will raise an
      // exception.
      clear();

      // prepare the set for its new data
      resize( s.capacity() );
      setResizePolicy( s.getResizePolicy() );

      // copy the set
      __inside = s.__inside;
    
      // now we shall update the end/rend iterators. Note that actually there is no
      // need to update the rend iterator as this one is not affected by changes
      // within hashtables (adding/deleting elements).  Hence, for speedup, we do
      // not update the rend iterator
      __it_end.__updatePosition( __inside.end() );
    }
    
    return *this;
  }

  
  /// indicates whether a given elements belong to the set
  template<typename KEY> INLINE
  bool Set<KEY>::contains( const KEY& k ) const {
    return __inside.exists( k );
  }


  /// inserts a new element in the set
  template<typename KEY> INLINE
  void Set<KEY>::insert( const KEY& k ) {
    if ( ! contains( k ) ) {
      // insert the element
      __inside.insert( k, true );

      // update end iterator. Note that actually there is no need to update the
      // rend iterator as this one is not affected by changes within hashtables
      // (adding/deleting elements).  Hence, for speedup, we do not update the
      // rend iterator
      __it_end.__updatePosition( __inside.end() );
    }
  }

  
  /// erases an element from the set
  template<typename KEY> INLINE
  void Set<KEY>::erase( const KEY& k ) {
    if ( contains( k ) ) {
      // erase the element
      __inside.erase( k );
      
      // update end and rend iterators. Note that actually there is no
      // need to update the rend iterator as this one is not affected by changes
      // within hashtables (adding/deleting elements). Hence, for speedup, we do
      // not update the rend iterator
      __it_end.__updatePosition( __inside.end() );
    }
  }


  /// adds a new element to the set
  template<typename KEY> INLINE
  Set<KEY>& Set<KEY>::operator<<( const KEY& k ) {
    insert( k );
    return ( *this );
  }

  
  /// removes an element from the set
  template<typename KEY> INLINE
  Set<KEY>& Set<KEY>::operator>>( const KEY& k ) {
    erase( k );
    return ( *this );
  }


  /// returns the number of elements in the set
  template<typename KEY> INLINE
  Size Set<KEY>::size() const {
    return __inside.size();
  }


  /// indicates whether the set is the empty set
  template<typename KEY> INLINE
  bool Set<KEY>::empty( ) const {
    return __inside.empty();
  }

  
  /// mathematical equality between two sets
  template<typename KEY>
  bool Set<KEY>::operator==( const Set<KEY>& s2 ) const {
    const HashTable<KEY, bool>& h2=s2.__inside;

    // check whether both sets have the same number of elements
    if ( size() != h2.size() ) return false;

    // check the content of the sets
    for ( HashTableIterator<KEY, bool> iter = __inside.begin();
          iter != __inside.end(); ++iter ) {
      if ( ! h2.exists( iter.key() ) )
        return false;
    }

    return true;
  }

  
  /// mathematical inequality between two sets
  template<typename KEY> INLINE
  bool Set<KEY>::operator!=( const Set<KEY>& s2 ) const {
    return !( operator==( s2 ) );
  }


  /// Intersection operator
  template<typename KEY>
  Set<KEY> Set<KEY>::operator*( const Set<KEY>& s2 ) const {
    Set<KEY> res;
    const HashTable<KEY, bool>& h2=s2.__inside;
    HashTable<KEY, bool>& h_r=res.__inside;

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

    return res;
  }

  
  /// Union operator
  template<typename KEY>
  Set<KEY> Set<KEY>::operator+( const Set<KEY>& s2 ) const {
    Set<KEY> res = *this;
    const HashTable<KEY, bool>& h2=s2.__inside;
    HashTable<KEY, bool>& h_r=res.__inside;

    for ( HashTableIterator<KEY,bool> iter=h2.begin(); iter!=h2.end(); ++iter ) {
      if ( ! h_r.exists( iter.key() ) )
        h_r.insert( iter.key(), true );
    }
    
    return res;
  }


  /// Disjunction operator
  template<typename KEY>
  Set<KEY> Set<KEY>::operator-( const Set<KEY>& s2 ) const {
    Set<KEY> res;
    const HashTable<KEY, bool>& h2=s2.__inside;
    HashTable<KEY, bool>& h_r=res.__inside;

    for ( HashTableIterator<KEY,bool> iter = __inside.begin();
          iter != __inside.end();++iter )
      if ( ! h2.exists( iter.key() ) )
        h_r.insert( iter.key(), true );

    return res;
  }
  

  /// to display the content of the set
  template<typename KEY> INLINE
  std::string Set<KEY>::toString() const {
    std::stringstream out;
    bool first=true;
    out<<"{";

    for ( iterator iter=begin();iter!=end();++iter ) {
      if ( first ) {
        out<<*iter;
        first=false;
      }
      else {
        out<<","<<*iter;
      }
    }

    out<<"}";

    std::string res;
    out>>res;
    return res;
  }


  /// to friendly display the content of the set
  template <typename Key> std::ostream& operator<<
  ( std::ostream& stream, const Set<Key>& set ) {
    stream<<set.toString();
    return stream;
  }


  /// returns the size of the underlying hashtable containing the set
  template<typename KEY> INLINE
  Size Set<KEY>::capacity() const {
    return __inside.capacity();
  }

  
  /// changes the size of the underlying hashtable
  template<typename KEY> INLINE
  void Set<KEY>::resize( Size new_size ) {
    __inside.resize( new_size );
    // update the position of the end iterator as the size of the table has changed
    __it_end.__updatePosition( __inside.end() );
  }

  
  /// enables the user to change dynamically the resizing policy of the
  /// underlying hashtable
  template<typename KEY> INLINE
  void Set<KEY>::setResizePolicy( const bool new_policy ) {
    __inside.setResizePolicy( new_policy );
    // update the position of the end iterator as the size of the table may change
    __it_end.__updatePosition( __inside.end() );
  }

  
  /// returns the current resizing policy of the underlying hashtable
  template<typename KEY> INLINE
  bool Set<KEY>::getResizePolicy() const {
    return __inside.getResizePolicy();
  }

  
  /// creates a hashtable of NEWKEY from the set
  template<typename KEY>
  template <typename NEWKEY>
  HashTable<KEY,NEWKEY>
  Set<KEY>::hashMap( NEWKEY( *f )( const KEY& ), Size size ) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without resizing
    if ( size == 0 )
      size = std::max( 2UL, __inside.size() / 2 );

    // create a new table
    HashTable<KEY,NEWKEY> table( size );

    // fill the new hash table
    for ( HashTableIterator<KEY,bool > iter = __inside.rbegin();
          iter != __inside.rend(); --iter ) {
      table.insert( iter.key(), f( iter.key() ) );
    }

    return table;
  }

  
  /// creates a hashtable of NEWKEY from the set
  template<typename KEY>
  template <typename NEWKEY>
  HashTable<KEY,NEWKEY> Set<KEY>::hashMap( const NEWKEY& val, Size size ) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without resizing
    if ( size == 0 )
      size = std::max( 2UL, __inside.size() / 2 );

    // create a new table
    HashTable<KEY,NEWKEY> table( size );

    // fill the new hash table
    for ( HashTableIterator<KEY,bool > iter = __inside.rbegin();
          iter != __inside.rend(); --iter ) {
      table.insert( iter.key(), val );
    }

    return table;
  }


  /// a method to create a list of Assent from a node list
  template<typename KEY>
  template <typename NEWKEY>
  List<NEWKEY> Set<KEY>::listMap( NEWKEY( *f )( const KEY& ) ) const {
    // create a new list
    List<NEWKEY> list;
    // fill the new list

    for ( HashTableIterator<KEY,bool > iter = __inside.begin();
          iter != __inside.end(); ++iter ) {
      list.pushBack( f( iter.key() ) );
    }

    return list;
  }



} /* namespace gum */

  
#endif    // DOXYGEN_SHOULD_SKIP_THIS
