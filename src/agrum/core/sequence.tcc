/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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


#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <limits>

//to ease IDE parser
#include <agrum/core/sequence.h>


namespace gum {


  /* =========================================================================== */
  /* ===                 NON SCALAR GUM SEQUENCE IMPLEMENTATION              === */
  /* =========================================================================== */

  /// updates const iterators
  template <typename Key, typename Alloc, bool Gen> INLINE
  void SequenceImplementation<Key,Alloc,Gen>::__update_end () {
    __end_safe.__setAtEnd();
  }

  
  /// clear the sequence
  template <typename Key, typename Alloc, bool Gen> INLINE
  void SequenceImplementation<Key,Alloc,Gen>::clear () {
    __h.clear();
    __v.clear();
    __update_end();
  }

  
  /// clears the current sequence and fill it with copies the element of aSeq
  template <typename Key, typename Alloc, bool Gen>
  template <typename OtherAlloc> INLINE
  void SequenceImplementation<Key,Alloc,Gen>::__copy
  ( const SequenceImplementation<Key,OtherAlloc,Gen>& aSeq ) {
    clear();

    for ( unsigned int i = 0; i < aSeq.size(); ++i ) {
      Key& new_key = const_cast<Key&>( __h.insert( * ( aSeq.__v[i] ), i ).first );
      __v.push_back( &new_key );
    }

    __update_end();
  }

  
  /// Default constructor
  template <typename Key, typename Alloc, bool Gen> INLINE
  SequenceImplementation<Key,Alloc,Gen>::SequenceImplementation
  ( Size size_param ) :
    __h ( size_param ),
    __end_safe  { *this },
    __rend_safe { *this } {
    // for debugging purposes
    GUM_CONSTRUCTOR( SequenceImplementation );
    __rend_safe.__setAtRend ();
    __end_safe.__setAtEnd ();
  }

  
  /// initializer list constructor
  template <typename Key, typename Alloc, bool Gen> INLINE
  SequenceImplementation<Key,Alloc,Gen>::SequenceImplementation
  ( std::initializer_list<Key> list ) :
    __end_safe  { *this },
    __rend_safe { *this } {
    GUM_CONSTRUCTOR( SequenceImplementation );
    __rend_safe.__setAtRend ();
    for ( const auto& elt : list ) {
      insert ( elt );
    }
  }

  
  /// copy constructor
  template <typename Key, typename Alloc, bool Gen> INLINE
  SequenceImplementation<Key,Alloc,Gen>::Sequence
  ( const SequenceImplementation<Key,Alloc,Gen>& aSeq ) :
    __end_safe  { *this },
    __rend_safe { *this } {
    // for debugging purposes
    GUM_CONS_CPY( SequenceImplementation );
    __rend_safe.__setAtRend();
    __copy( aSeq );
  }

  
  /// generalized copy constructor
  template <typename Key, typename Alloc, bool Gen>
  template <typename OtherAlloc> INLINE
  SequenceImplementation<Key,Alloc,Gen>::Sequence
  ( const SequenceImplementation<Key,OtherAlloc,Gen>& aSeq ) :
    __end_safe  { *this },
    __rend_safe { *this } {
    // for debugging purposes
    GUM_CONS_CPY( SequenceImplementation );
    __rend_safe.__setAtRend();
    __copy( aSeq );
  }

  
  /// move constructor
  template <typename Key, typename Alloc, bool Gen> INLINE
  SequenceImplementation ( SequenceImplementation<Key,Alloc,Gen>&& aSeq ) :
    __h ( std::move ( aSeq.__h ) ),
    __v ( std::move ( aSeq.__v ) ),
    __end_safe  { std::move ( aSeq.__end_safe ) },
    __rend_safe { std::move ( aSeq.__rend_safe ) } {
    // for debugging purposes
    GUM_CONS_MOV( SequenceImplementation );
  }

  
  /// destructor
  template <typename Key, typename Alloc, bool Gen> INLINE
  SequenceImplementation<Key,Alloc,Gen>::~SequenceImplementation () {
    GUM_DESTRUCTOR( SequenceImplementation );
  }


  /// copy operator
  template <typename Key, typename Alloc, bool Gen> INLINE
  SequenceImplementation<Key,Alloc,Gen>&
  SequenceImplementation<Key,Alloc,Gen>::operator=
  ( const SequenceImplementation<Key,Alloc,Gen>& aSeq ) {
    // avoid self assignment
    if ( &aSeq != this ) {
      __copy( aSeq );
    }

    return *this;
  }

  
  /// generalized copy operator
  template <typename Key, typename Alloc, bool Gen> INLINE
  SequenceImplementation<Key,Alloc,Gen>&
  SequenceImplementation<Key,Alloc,Gen>::operator=
  ( const SequenceImplementation<Key,OtherAlloc,Gen>& aSeq ) {
    __copy( aSeq );
    return *this;
  }

  
  /// move operator
  template <typename Key, typename Alloc, bool Gen> INLINE
  SequenceImplementation<Key,Alloc,Gen>&
  SequenceImplementation<Key,Alloc,Gen>::operator=
  ( SequenceImplementation<Key,Alloc,Gen>&& aSeq ) {
    // avoid self assignment
    if ( &aSeq != this ) {
      __h = std::move ( aSeq.__h );
      __v = std::move ( aSeq.__v );
      __end_safe  = std::move ( aSeq.__end_safe );
      __rend_safe = std::move ( aSeq.__rend_safe );
    }

    return *this;
  }

  
  /// returns the size of the sequence
  template <typename Key, typename Alloc, bool Gen> INLINE
  Size SequenceImplementation<Key,Alloc,Gen>::size() const noexcept {
    return __h.size();
  }


  /// return true if empty
  template <typename Key, typename Alloc, bool Gen> INLINE
  bool SequenceImplementation<Key,Alloc,Gen>::empty() const noexcept {
    return __h.empty();
  }


  /// check the existence of k in the sequence
  template <typename Key, typename Alloc, bool Gen> INLINE
  bool SequenceImplementation<Key,Alloc,Gen>::exists( const Key& k ) const {
    return __h.exists( k );
  }


  /// insert an element at the end of the sequence
  template <typename Key, typename Alloc, bool Gen> INLINE
  void SequenceImplementation<Key,Alloc,Gen>::insert( const Key& k ) {
    try {
      // k will be added at the end. Insert the new key into the hashtable
      Key& new_key = const_cast<Key&>( __h.insert( k, __h.size() ).first );
      __v.push_back( &new_key );
      __update_end();
    }
    catch ( DuplicateElement& e ) {
      GUM_ERROR( DuplicateElement, "Key already in sequence" );
    }
  }

  
  /// insert an element at the end of the sequence
  template <typename Key, typename Alloc, bool Gen> INLINE
  void SequenceImplementation<Key,Alloc,Gen>::insert( Key&& k ) {
    try{
      // k will be added at the end. Insert the new key into the hashtable
      Key& new_key =
        const_cast<Key&>( __h.insert( std::move ( k ), __h.size() ).first );
      __v.push_back( &new_key );
      __update_end();
    }
    catch ( DuplicateElement& e ) {
      GUM_ERROR( DuplicateElement, "Key already in sequence" );
    }
  }


  /// insert k in the sequence (synonym for insert)
  template <typename Key, typename Alloc, bool Gen> INLINE
  SequenceImplementation<Key,Alloc,Gen>&
  SequenceImplementation<Key,Alloc,Gen>::operator<< ( const Key& k ) {
    insert( k );
    return *this;
  }

  
  /// insert k in the sequence (synonym for insert)
  template <typename Key, typename Alloc, bool Gen> INLINE
  SequenceImplementation<Key,Alloc,Gen>&
  SequenceImplementation<Key,Alloc,Gen>::operator<< ( Key&& k ) {
    insert( std::move ( k ) );
    return *this;
  }


  /// remove an element from the sequence
  template <typename Key, typename Alloc, bool Gen> INLINE
  void SequenceImplementation<Key,Alloc,Gen>::erase( const Key& k ) {
    // get the position of the element to remove
    Idx pos;

    try { pos = __h[k]; }
    catch ( NotFound& ) { return; }

    // erase the element
    __v.erase( __v.begin() + pos );
    for ( Idx i = pos, nb_elts = __h.size () - 1; i < nb_elts; ++i ) {
      --__h[ * ( __v[i] )];
    }
    __h.erase( k );
    
    __update_end();
  }


  /// remove from the sequence the element pointed to by the iterator
  template <typename Key, typename Alloc, bool Gen> INLINE
  void SequenceImplementation<Key,Alloc,Gen>::erase ( const iterator_safe& iter ) {
    if ( iter.pos() >= size() ) return;

    // erase the element
    Idx  pos = iter.pos();
    Key* key = __v[pos];
    __v.erase( __v.begin() + pos );

    for ( Idx i = pos, nb_elts = __h.size () - 1; i < nb_elts; ++i ) {
      --__h[ * ( __v[i] )];
    }
    __h.erase( *key );

    __update_end();
  }


  /// remove k in the sequence (synonym for remove)
  template <typename Key, typename Alloc, bool Gen> INLINE
  SequenceImplementation<Key,Alloc,Gen>&
  SequenceImplementation<Key,Alloc,Gen>::operator>> ( const Key& k )  {
    erase( k );
    return *this;
  }
  
  
  // returns the object at the pos i
  template <typename Key, typename Alloc, bool Gen> INLINE
  const Key& SequenceImplementation<Key,Alloc,Gen>::atPos( Idx i ) const {
    if ( i >= __h.size() ) {
      GUM_ERROR( NotFound, "not enough elements in the sequence" );
    }

    return * ( __v[i] );
  }

  
  /// returns the element at position i (synonym for atPos)
  template <typename Key, typename Alloc, bool Gen> INLINE
  const Key& SequenceImplementation<Key,Alloc,Gen>::operator[]( Idx i ) const {
    return atPos( i );
  }


  /// returns the position of the object passed in argument (if it exists)
  template <typename Key, typename Alloc, bool Gen> INLINE
  Idx SequenceImplementation<Key,Alloc,Gen>::pos( const Key& key ) const {
    return __h[key];
  }


  // inserts and returns the object at the pos i
  template <typename Key, typename Alloc, bool Gen> INLINE
  void SequenceImplementation<Key,Alloc,Gen>::setAtPos ( Idx i,
                                                         const Key& newKey ) {
    if ( i >= __h.size() ) {
      GUM_ERROR( NotFound, "index too large" );
    }

    Key& new_key = const_cast<Key&>( __h.insert( newKey, i ).first );
    __h.erase( * ( __v[i] ) );
    __v[i] = &new_key;
  }

  
  // inserts and returns the object at the pos i
  template <typename Key, typename Alloc, bool Gen> INLINE
  void SequenceImplementation<Key,Alloc,Gen>::setAtPos ( Idx i, Key&& newKey ) {
    if ( i >= __h.size() ) {
      GUM_ERROR( NotFound, "index too large" );
    }

    Key& new_key = const_cast<Key&>( __h.insert( std::move ( newKey ), i ).first );
    __h.erase( * ( __v[i] ) );
    __v[i] = &new_key;
  }


  /// returns the object at the pos i
  template <typename Key, typename Alloc, bool Gen> INLINE
  void SequenceImplementation<Key,Alloc,Gen>::swap( Idx i,Idx j ) {
    if ( i == j ) return;

    Key& ki = const_cast<Key&>( atPos( i ) );
    Key& kj = const_cast<Key&>( atPos( j ) );

    __h[ki]=j;
    __h[kj]=i;

    __v[i] = &kj;
    __v[j] = &ki;
  }


  /// returns the first element
  template <typename Key, typename Alloc, bool Gen> INLINE
  const Key& SequenceImplementation<Key,Alloc,Gen>::front() const {
    return atPos( 0 );
  }


  /// returns the last element
  template <typename Key, typename Alloc, bool Gen> INLINE
  const Key& SequenceImplementation<Key,Alloc,Gen>::back() const {
    return atPos( size() - 1 );
  }


  /// Print a sequence
  template <typename Key, typename Alloc, bool Gen>
  std::string SequenceImplementation<Key,Alloc,Gen>::toString() const {
    std::stringstream stream;
    stream<<"[";

    if ( ! __h.empty() ) {
      stream << 0 << ":" <<  *__v[0] ;

      for ( Idx i=1; i<__h.size(); ++i ) {
        stream << " - " << i << ":" <<  *__v[i] ;
      }
    }

    stream<<"]";

    std::string res=stream.str();
    return res;
  }


  /// returns true if the content of k equals that of *this
  template <typename Key, typename Alloc, bool Gen>
  template <typename OtherAlloc>
  bool SequenceImplementation<Key,Alloc,Gen>::operator==
  ( const SequenceImplementation<Key,OtherAlloc,Gen>& k ) const {
    if ( size() != k.size() ) return false;
    else {
      for ( Idx i=0; i < size(); ++i )
        if ( *__v[i] != * ( k.__v[i] ) ) return false;
    }

    return true;
  }


  /// returns true if the content of k is different from that of *this
  template <typename Key, typename Alloc, bool Gen>
  template <typename OtherAlloc> INLINE
  bool SequenceImplementation<Key,Alloc,Gen>::operator!=
  ( const SequenceImplementation<Key,OtherAlloc,Gen>& k ) const {
    return !operator== ( k );
  }


  /// a << operator for displaying the content of the Sequence
  template <typename Key, typename Alloc, bool Gen> INLINE
  std::ostream& operator<< ( std::ostream& stream,
                             const SequenceImplementation<Key,Alloc,Gen>& seq ) {
    stream << seq.toString();
    return stream;
  }


  /// returns the safe begin iterator
  template <typename Key, typename Alloc, bool Gen> INLINE
  SequenceIteratorSafe<Key,Alloc,Gen>
  SequenceImplementation<Key,Alloc,Gen>::beginSafe () const {
    return SequenceIteratorSafe<Key,Alloc,Gen> { *this };
  }


  /// return a value to end=size()
  template <typename Key, typename Alloc, bool Gen> INLINE
  const SequenceIteratorSafe<Key,Alloc,Gen>&
  SequenceImplementation<Key,Alloc,Gen>::endSafe () const noexcept {
    return __end_safe;
  }


  /// return a value to terminal =size()-1
  template <typename Key, typename Alloc, bool Gen> INLINE
  SequenceIteratorSafe<Key,Alloc,Gen>
  SequenceImplementation<Key,Alloc,Gen>::rbeginSafe () const {
    SequenceIteratorSafe<Key,Alloc,Gen> it { *this };
    it.__setPos( size()-1 );
    return it;
  }


  /// returns a value to 0
  template <typename Key, typename Alloc, bool Gen> INLINE
  const SequenceIteratorSafe<Key,Alloc,Gen>&
  SequenceImplementation<Key,Alloc,Gen>::rendSafe () const noexcept {
    return __rend_safe;
  }


  /// modifies the size of the internal structures of the sequence
  template <typename Key, typename Alloc, bool Gen> INLINE
  void SequenceImplementation<Key,Alloc,Gen>::resize( unsigned int new_size ) {
    if ( new_size < __h.size() ) return;

    __h.resize( new_size );
    __v.reserve( new_size );
  }


  /// returns the set difference : this \ seq
  template <typename Key, typename Alloc, bool Gen>
  template <typename OtherAlloc> INLINE
  Set<Key,Alloc>
  SequenceImplementation<Key,Alloc,Gen>::diffSet
  ( const SequenceImplementation<Key,OtherAlloc,Gen>& seq ) const {
    Set<Key,Alloc> res;

    for ( iterator_safe iter = begin(); iter != end(); ++iter ) {
      if ( ! seq.exists( *iter ) ) res << *iter;
    }

    return res;
  }













  






  

  /* =========================================================================== */
  // class SequenceIteratorSafe
  /* =========================================================================== */


  /// returns the size of the sequence


  template <typename Key> INLINE
  SequenceIteratorSafe<Key>::SequenceIteratorSafe( const Sequence<Key>& seq,
                                                   Idx pos ) : __seq( &seq ) {
    GUM_CONSTRUCTOR( SequenceIteratorSafe );

    if ( pos > __seq->size() )
      __iterator = __seq->size(); // make the iterator point to end
    else
      __iterator = pos;
  }


  template <typename Key> INLINE
  SequenceIteratorSafe<Key>::SequenceIteratorSafe( const SequenceIteratorSafe<Key>& source ) :
    __iterator( source.__iterator ),__seq( source.__seq ) {
    GUM_CONS_CPY( SequenceIteratorSafe );
  }


  template <typename Key> INLINE
  SequenceIteratorSafe<Key>::~SequenceIteratorSafe() {
    GUM_DESTRUCTOR( SequenceIteratorSafe );
  }


  template <typename Key> INLINE
  SequenceIteratorSafe<Key>& SequenceIteratorSafe<Key>::operator=
  ( const SequenceIteratorSafe<Key>& source ) {
    __iterator = source.__iterator;
    __seq = source.__seq;
    return *this;
  }


  template <typename Key> INLINE
  SequenceIteratorSafe<Key>& SequenceIteratorSafe<Key>::operator++( ) {
    if ( __iterator < __seq->size() )
      ++__iterator;
    else
      __iterator = __seq->size();

    return *this;
  }


  template <typename Key> INLINE
  SequenceIteratorSafe<Key>& SequenceIteratorSafe<Key>::operator--( ) {
    if ( __iterator != std::numeric_limits<Idx>::max() )
      --__iterator;
    else
      __iterator = std::numeric_limits<Idx>::max();

    return *this;
  }


  template <typename Key> INLINE
  bool SequenceIteratorSafe<Key>::operator==
  ( const SequenceIteratorSafe<Key>& source ) const {
    if ( __seq->empty() )
      return true; // all iterators are the same if seq is empty

    if ( ( __iterator != source.__iterator ) ||
         ( __seq != source.__seq ) )
      return false;

    return true;
  }


  template <typename Key> INLINE
  bool SequenceIteratorSafe<Key>::operator!=
  ( const SequenceIteratorSafe<Key>& source ) const {
    return ! operator== ( source );
  }


  template <typename Key> INLINE
  Idx SequenceIteratorSafe<Key>::pos() const {
    if ( __iterator == std::numeric_limits<Idx>::max() ) {
      GUM_ERROR( UndefinedIteratorValue, "iterator is rend()" );
    }

    if ( __iterator >= __seq->size() ) {
      GUM_ERROR( UndefinedIteratorValue, "iterator is end()" );
    }

    return  __iterator;
  }


  template <typename Key> INLINE
  const Key& SequenceIteratorSafe<Key>::operator*() const {
    return * ( __seq->__v[ pos()] );
  }

  template <typename Key> INLINE
  const Key* SequenceIteratorSafe<Key>::operator->() const {
    return  __seq->__v[ pos()];
  }


  template <typename Key> INLINE
  void SequenceIteratorSafe<Key>::__setPos( Idx pos ) {
    if ( pos > __seq->size() ) __iterator = __seq->size();
    else __iterator = pos;
  }


  template <typename Key> INLINE
  void SequenceIteratorSafe<Key>::__setAtRend( ) {
    __iterator = std::numeric_limits<Idx>::max();
  }


  template <typename Key> INLINE
  void SequenceIteratorSafe<Key>::__setAtEnd( ) {
    __iterator = __seq->size();
  }










  // SequenceIteratorSafe





  // ##############################################################################
  // ##############################################################################
  //
  // BELOW, CLASSES ARE SPECIALIZATIONS DESIGNED TO OPTIMIZE SEQUENCES OF POINTERS
  //
  // ##############################################################################
  // ##############################################################################




  // SequenceIteratorSafe optimized for pointers




  /// returns the size of the sequence

  template <typename Key> INLINE
  Size Sequence<Key*>::size() const  {
    // note that, unlike function Sequence<Key>::size(), here no exception can
    // be raised within functions Sequence<Key*>::erase. Hence, in this function,
    // __update_end() can be called after both __h and __v, which enables us to
    // define size() as either __v.size() or __h.size()
    return __h.size();
  }

  template <typename Key> INLINE
  SequenceIteratorSafe<Key*>::SequenceIteratorSafe( const Sequence<Key*>& seq,
                                                    Idx pos ) : __seq( &seq ) {
    GUM_CONSTRUCTOR( SequenceIteratorSafe );

    if ( pos > __seq->size() )
      __iterator = __seq->size(); // make the iterator point to end
    else
      __iterator = pos;
  }


  template <typename Key> INLINE
  SequenceIteratorSafe<Key*>::SequenceIteratorSafe( const SequenceIteratorSafe<Key*>& source ) :
    __iterator( source.__iterator ),__seq( source.__seq ) {
    GUM_CONS_CPY( SequenceIteratorSafe );
  }


  template <typename Key> INLINE
  SequenceIteratorSafe<Key*>::~SequenceIteratorSafe() {
    GUM_DESTRUCTOR( SequenceIteratorSafe );
  }


  template <typename Key> INLINE
  SequenceIteratorSafe<Key*>& SequenceIteratorSafe<Key*>::operator=
  ( const SequenceIteratorSafe<Key*>& source ) {
    __iterator = source.__iterator;
    __seq = source.__seq;
    return *this;
  }


  template <typename Key> INLINE
  SequenceIteratorSafe<Key*>& SequenceIteratorSafe<Key*>::operator++( ) {
    if ( __iterator < __seq->size() )
      ++__iterator;
    else
      __iterator = __seq->size();

    return *this;
  }


  template <typename Key> INLINE
  SequenceIteratorSafe<Key*>& SequenceIteratorSafe<Key*>::operator--( ) {
    if ( __iterator != std::numeric_limits<Idx>::max() )
      --__iterator;
    else
      __iterator = std::numeric_limits<Idx>::max();

    return *this;
  }


  template <typename Key> INLINE
  bool SequenceIteratorSafe<Key*>::operator==
  ( const SequenceIteratorSafe<Key*>& source ) const {
    if ( __seq->empty() )
      return true; // all iterators are the same if seq is empty

    if ( ( __iterator != source.__iterator ) ||
         ( __seq != source.__seq ) )
      return false;

    return true;
  }


  template <typename Key> INLINE
  bool SequenceIteratorSafe<Key*>::operator!=
  ( const SequenceIteratorSafe<Key*>& source ) const {
    return ! operator== ( source );
  }


  template <typename Key> INLINE
  Idx SequenceIteratorSafe<Key*>::pos() const {
    if ( __iterator == std::numeric_limits<Idx>::max() ) {
      GUM_ERROR( UndefinedIteratorValue, "iterator is rend()" );
    }

    if ( __iterator >= __seq->size() ) {
      GUM_ERROR( UndefinedIteratorValue, "iterator is end()" );
    }

    return  __iterator;
  }


  template <typename Key> INLINE
  Key* const SequenceIteratorSafe<Key*>::operator*() const {
    return __seq->__v[ pos()];
  }

  template <typename Key> INLINE
  void SequenceIteratorSafe<Key*>::__setPos( Idx pos ) {
    if ( pos > __seq->size() ) __iterator = __seq->size();
    else __iterator = pos;
  }


  template <typename Key> INLINE
  void SequenceIteratorSafe<Key*>::__setAtRend( ) {
    __iterator = std::numeric_limits<Idx>::max();
  }


  template <typename Key> INLINE
  void SequenceIteratorSafe<Key*>::__setAtEnd( ) {
    __iterator = __seq->size();
  }















  

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                 GUM_SEQUENCE OPTIMIZED FOR POINTERS                 === */
  /* =========================================================================== */
  /* =========================================================================== */



  /// updates const iterators

  template <typename Key> INLINE
  void Sequence<Key*>::__update_end( ) {
    __end.__setAtEnd();
  }


  /// clear the sequence

  template <typename Key> INLINE
  void Sequence<Key*>::clear() {
    __h.clear();
    __v.clear();
    __update_end();
  }


  /** insert an element at the end of the sequence. private version for internal
   * use */

  template <typename Key> INLINE
  void Sequence<Key*>::__insert( Key* const k ) {
    if ( __h.exists( k ) ) {
      GUM_ERROR( DuplicateElement,"Key already in sequence" );
    }

    // k will be added at the end. Insert the new key into the hashtable
    __h.insert( k, __h.size() );
    __v.push_back( k );
  }


  /// clears the current sequence and fill it with copies the element of aSeq

  template <typename Key> INLINE
  void Sequence<Key*>::__copy( const Sequence<Key*>& aSeq ) {
    clear();

    for ( unsigned int i = 0; i < aSeq.size(); ++i )
      __insert( aSeq.__v[i] );

    __update_end();
  }


  /// Default constructor

  template <typename Key>
  Sequence<Key*>::Sequence() : __h(), __v(), __end( *this ), __rend( *this ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Sequence );
    __rend.__setAtRend();
    __end.__setAtEnd();
  }


  /// copy constructor

  template <typename Key>
  Sequence<Key*>::Sequence( const Sequence<Key*>& aSeq ) :
    __h( aSeq.__h ), __v( aSeq.__v ), __end( *this ), __rend( *this ) {
    // for debugging purposes
    GUM_CONS_CPY( Sequence );
    __rend.__setAtRend();
    __end.__setAtEnd();
  }


  /// destructor

  template <typename Key>
  Sequence<Key*>::~Sequence() {
    GUM_DESTRUCTOR( Sequence );
  }


  /// copy operator

  template <typename Key> INLINE
  Sequence<Key*>& Sequence<Key*>::operator= ( const Sequence<Key*>& aSeq ) {
    // avoid self assignment
    if ( &aSeq != this ) {
      __copy( aSeq );
    }

    return *this;
  }


  /// return true if empty

  template <typename Key> INLINE
  bool Sequence<Key*>::empty() const  {
    return __h.empty();
  }


  /// check the existence of k in the sequence

  template <typename Key> INLINE
  bool Sequence<Key*>::exists( Key* const k ) const  {
    return __h.exists( k );
  }


  /// insert an element at the end of the sequence

  template <typename Key> INLINE
  void Sequence<Key*>::insert( Key* const k ) {
    __insert( k );
    __update_end();
  }


  // returns the object at the pos i

  template <typename Key> INLINE
  Key* const Sequence<Key*>::atPos( Idx i ) const {
    if ( i >= __h.size() ) {
      GUM_ERROR( NotFound, "" );
    }

    return __v[i];
  }


  /// returns the position of the object passed in argument (if it exists)

  template <typename Key> INLINE
  Idx Sequence<Key*>::pos( Key* const key ) const {
    return __h[key];
  }


  /// insert k in the sequence (synonym for insert)

  template <typename Key> INLINE
  Sequence<Key*>& Sequence<Key*>::operator<< ( Key* const k ) {
    insert( k );
    return *this;
  }


  /// remove an element from the sequence

  template <typename Key> INLINE
  void Sequence<Key*>::erase( Key* const k ) {
    // get the position of the element to remove
    Idx pos;

    try { pos = __h[k]; }
    catch ( NotFound& ) { return; }

    // erase the element
    __v.erase( __v.begin() +pos );

    for ( Idx i = pos; i < __v.size(); ++i ) {
      --__h[ __v[i] ];
    }

    __h.erase( k );
    __update_end();
  }


  /// remove from the sequence the element pointed to by the iterator

  template <typename Key> INLINE
  void Sequence<Key*>::erase( const iterator_safe& iter ) {
    if ( iter.pos() >= size() ) return;

    // erase the element
    Idx  pos = iter.pos();
    Key* key = __v[pos];
    __v.erase( __v.begin() +pos );

    for ( Idx i = pos; i < __v.size(); ++i ) {
      --__h[ __v[i] ];
    }

    __h.erase( key );
    __update_end();
  }


  /// remove k in the sequence (synonym for remove)

  template <typename Key> INLINE
  Sequence<Key*>& Sequence<Key*>::operator>> ( Key* const k )  {
    erase( k );
    return *this;
  }


  // returns the object at the pos i

  template <typename Key> INLINE
  void Sequence<Key*>::setAtPos( Idx i, Key* const newKey ) {
    if ( i >= __h.size() ) {
      GUM_ERROR( NotFound, "index too large" );
    }

    if ( __h.exists( newKey ) ) {
      GUM_ERROR( DuplicateElement,"Key already in sequence" );
    }

    __h.erase( __v[i] );
    __h.insert( newKey,i );
    __v[i] = newKey;
  }


  /// returns the object at the pos i

  template <typename Key> INLINE
  void Sequence<Key*>::swap( Idx i,Idx j ) {
    if ( i == j ) return;

    Key* const ki = atPos( i );
    Key* const kj = atPos( j );

    __h[ki]=j;
    __h[kj]=i;

    __v[i] = kj;
    __v[j] = ki;
  }


  /// returns the first element

  template <typename Key> INLINE
  Key* const Sequence<Key*>::front() const {
    return atPos( 0 );
  }


  /// returns the last element

  template <typename Key> INLINE
  Key* const Sequence<Key*>::back() const {
    return atPos( size() - 1 );
  }


  /// returns the index of object k if it exists

  template <typename Key> INLINE
  Idx Sequence<Key*>::operator[]( Key* const k ) const {
    return __h[k];
  }


  /// returns the element at position i (synonym for atPos)

  template <typename Key> INLINE
  Key* const Sequence<Key*>::operator[]( Idx i ) const {
    return atPos( i );
  }


  /// Print a sequence

  template <typename Key>
  std::string Sequence<Key*>::toString() const {
    std::stringstream stream;
    stream<<"[";

    if ( ! __h.empty() ) {
      stream << 0 << ":" << __v[0] ;

      for ( Idx i=1; i<__h.size(); ++i ) {
        stream<<" - "<<i<<":"<< __v[i] ;
      }
    }

    stream<<"]";

    std::string res=stream.str();
    return res;
  }


  /// returns true if the content of k equals that of *this

  template <typename Key> INLINE
  bool Sequence<Key*>::operator== ( const Sequence<Key*>& k ) const {
    if ( size() != k.size() ) return false;
    else {
      for ( Idx i=0; i < size(); ++i )
        if ( __v[i] != k.__v[i] ) return false;
    }

    return true;
  }


  /// returns true if the content of k is different from that of *this

  template <typename Key> INLINE
  bool Sequence<Key*>::operator!= ( const Sequence<Key*>& k ) const {
    return !operator== ( k );
  }



  /// returns a value to 0

  template <typename Key> INLINE
  SequenceIteratorSafe<Key*> Sequence<Key*>::beginSafe() const {
    return SequenceIteratorSafe<Key*> ( *this );
  }


  /// return a value to end=size()

  template <typename Key> INLINE
  const SequenceIteratorSafe<Key*>& Sequence<Key*>::endSafe() const  {
    return __end;
  }


  /// return a value to terminal =size()-1

  template <typename Key> INLINE
  SequenceIteratorSafe<Key*> Sequence<Key*>::rbeginSafe() const {
    SequenceIteratorSafe<Key*> it( *this );
    it.__setPos( size()-1 );
    return it;
  }


  /// returns a value to 0

  template <typename Key> INLINE
  const SequenceIteratorSafe<Key*>& Sequence<Key*>::rendSafe() const  {
    return __rend;
  }


  /// modifies the size of the internal structures of the sequence

  template <typename Key> INLINE
  void Sequence<Key*>::resize( unsigned int new_size ) {
    if ( new_size < __h.size() ) return;

    __h.resize( new_size );
    __v.reserve( new_size );
  }

  /// returns the set difference : this \ seq

  template <typename Key> INLINE
  Set<Key*> Sequence<Key*>::diffSet( const Sequence<Key*>& seq ) const {
    Set<Key*> res;

    for ( typename Sequence<Key*>::iterator_safe iter = seq.beginSafe(); iter != seq.endSafe(); ++iter ) {
      if ( ! this->exists( *iter ) ) res<<*iter;
    }

    return res;
  }
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

