/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr   *
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


namespace gum {
  

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  // ==============================================================================
  /// clear the sequence
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::clear() {
    __h.clear();
    __v.clear();
    __update_iterators();
  }

  // ==============================================================================
  /// clears the current sequence and fill it with copies the element of aSeq
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::__copy( const Sequence<KEY>& aSeq ) {
    clear();

    for ( iterator iter = aSeq.begin(); iter != aSeq.end(); ++iter )
      __insert( *iter );

    __update_iterators();
  }

  // ==============================================================================
  /// Default constructor
  // ==============================================================================
  template <typename KEY>
  Sequence<KEY>::Sequence():__h(),__v(),__end( this ),__rend( this ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Sequence );
    __rend.__setAtBegin();
    __end.__setAtEnd();
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  template <typename KEY>
  Sequence<KEY>::Sequence( const Sequence<KEY>& aSeq ) :
    __h( aSeq.__h ),__v( aSeq.__v ),__end( this ),__rend( this ) {
    // for debugging purposes
    GUM_CONS_CPY( Sequence );
    __rend.__setAtBegin();
    __end.__setAtEnd();
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template <typename KEY>
  Sequence<KEY>::~Sequence() {
    GUM_DESTRUCTOR( Sequence );
  }

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  template <typename KEY> INLINE
  Sequence<KEY>& Sequence<KEY>::operator= ( const Sequence<KEY>& aSeq ) {
    // avoid self assignment
    if ( &aSeq != this ) {
      __copy( aSeq );
    }

    return *this;
  }

  // ==============================================================================
  /// returns the size of the sequence
  // ==============================================================================
  template <typename KEY> INLINE
  Size Sequence<KEY>::size() const  {
    return __h.size();
  }

  // ==============================================================================
  /// return true if empty
  // ==============================================================================
  template <typename KEY> INLINE
  bool Sequence<KEY>::empty() const  {
    return __h.empty();
  }

  // ==============================================================================
  /// check the existence of k in the sequence
  // ==============================================================================
  template <typename KEY> INLINE
  bool Sequence<KEY>::exists( const KEY& k ) const  {
    return __h.exists( k );
  }

  // ==============================================================================
  /// insert an element at the end of the sequence
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::insert( const KEY& k ) {
    __insert( k );
    __update_iterators();
  }

  // ==============================================================================
  /// insert an element at the end of the sequence. private version for internal use
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::__insert( const KEY k ) {
    if ( __h.exists( k ) ) GUM_ERROR( DuplicateElement,"Key already in sequence" );

    // k will be added at the end.
    Idx pos=__h.size();

    __h.insert( k,pos );

    __v.push_back( k );
  }

  // ==============================================================================
  /// insert k in the sequence (synonym for insert)
  // ==============================================================================
  template <typename KEY> INLINE
  Sequence<KEY>& Sequence<KEY>::operator<<( const KEY& k ) {
    insert( k );
    return *this;
  }

  // ==============================================================================
  /// remove an element from the sequence
  // ==============================================================================
  template <typename KEY>
  void Sequence<KEY>::erase( const KEY& k ) {
    // get the position of the element to remove
    Idx pos;

    try { pos=__h[k]; }
    catch ( NotFound& ) { return; }

    // erase the element
    try {
      __v.erase( __v.begin()+pos );
      __h.erase( k );

      for ( Idx i=pos; i<__v.size(); ++i ) {
        --__h[ __v[i] ];
      }
    } catch ( Exception& e ) {
      GUM_SHOWERROR( e );
    }

    __update_iterators();
  }

  // ==============================================================================
  /// remove k in the sequence (synonym for remove)
  // ==============================================================================
  template <typename KEY> INLINE
  Sequence<KEY>& Sequence<KEY>::operator>>( const KEY& k )  {
    erase( k );
    return *this;
  }

  // ==============================================================================
  // returns the object at the pos i
  // ==============================================================================
  template <typename KEY> INLINE
  const KEY& Sequence<KEY>::atPos( const Idx i ) const {
    if ( i >= __h.size() )
      GUM_ERROR( NotFound, "" );

    return __v[i];
  }

  // ==============================================================================
  // returns the object at the pos i
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::setAtPos( Idx i,const KEY newKey ) {
    if ( i >= __h.size() )  GUM_ERROR( NotFound, "index too large" );

    if ( __h.exists( newKey ) )
      GUM_ERROR( DuplicateElement,"Key already in sequence" );

    const KEY& oldKey=atPos( i );

    __h.erase( oldKey );

    __h.insert( newKey,i );

    __v[i]=newKey;
  }

  // ==============================================================================
  /// returns the object at the pos i
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::swap( Idx i,Idx j ) {
    if ( i==j ) return;

    const KEY ki=atPos( i ); // we copy only one KEY

    const KEY& kj=atPos( j );

    __h[ki]=j;__h[kj]=i;

    __v[i]=kj; __v[j]=ki;
  }

  // ============================================================================
  /// returns the first element
  // ============================================================================
  template <typename KEY> INLINE
  const KEY& Sequence<KEY>::front() const {
    return this->atPos(0);
  }
  
  // ============================================================================
  /// returns the last element
  // ============================================================================
  template <typename KEY> INLINE
  const KEY& Sequence<KEY>::back() const {
    return this->atPos(this->size() - 1);
  }

  // ==============================================================================
  /// returns the index of object k if it exists
  // ==============================================================================
  template <typename KEY> INLINE
  Idx Sequence<KEY>::operator[]( const KEY& k ) const {
    return __h[k];
  }

  // ==============================================================================
  /// returns the element at position i (synonym for atPos)
  // ==============================================================================
  template <typename KEY> INLINE
  const KEY& Sequence<KEY>::operator[]( const Idx i ) const {
    return atPos( i );
  }

  // ==============================================================================
  /// Print a sequence
  // ==============================================================================
  template <typename KEY>
  std::string Sequence<KEY>::toString() const {
    std::stringstream stream;
    stream<<"[";

    if ( ! __h.empty() ) {
      stream<<0<<":"<<  __v[0] ;

      for ( Idx i=1;i<__h.size();++i ) {
        stream<<" - "<<i<<":"<<  __v[i] ;
      }
    }

    stream<<"]";

    std::string res=stream.str();
    return res;
  }

  // ==============================================================================
  /// returns true if the content of k equals that of *this
  // ==============================================================================
  template <typename KEY> INLINE
  bool Sequence<KEY>::operator==( const Sequence<KEY>& k ) const {
    if ( size()!=k.size() ) return false;
    else {
      for ( Idx i=0;i<size();++i )
        if ( __v[i] != k.__v[i] ) return false;
    }

    return true;
  }

  // ==============================================================================
  /// returns true if the content of k is different from that of *this
  // ==============================================================================
  template <typename KEY> INLINE
  bool Sequence<KEY>::operator!=( const Sequence<KEY>& k ) const {
    return !operator==( k );
  }

  // ==============================================================================
  /// a << operator for displaying the content of the Sequence
  // ==============================================================================
  template <typename KEY> INLINE
  std::ostream& operator<< ( std::ostream& stream, const Sequence<KEY>& seq ) {
    stream<<seq.toString();
    return stream;
  }

  // ==============================================================================
  /// returns a value to 0
  // ==============================================================================
  template <typename KEY> INLINE
  SequenceIterator<KEY> Sequence<KEY>::begin() const {
    SequenceIterator<KEY> it( this,0 );

    if ( empty() ) it.__setAtBegin();

    return it;
  }

  // ==============================================================================
  /// return a value to end=size()
  // ==============================================================================
  template <typename KEY> INLINE
  const SequenceIterator<KEY>& Sequence<KEY>::end() const  {
    return __end;
  }

  // ==============================================================================
  /// return a value to terminal =size()-1
  // ==============================================================================
  template <typename KEY> INLINE
  SequenceIterator<KEY> Sequence<KEY>::rbegin() const {
    SequenceIterator<KEY> it( this,0 );

    if ( empty() ) it.__setAtEnd();
    else it.__setPos( size()-1 );

    return it;
  }

  // ==============================================================================
  /// returns a value to 0
  // ==============================================================================
  template <typename KEY> INLINE
  const SequenceIterator<KEY>& Sequence<KEY>::rend() const  {
    return __rend;
  }

  // ==============================================================================
  /// modifies the size of the internal structures of the sequence
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::resize( unsigned int new_size ) {
    if ( new_size < __h.size() ) return;

    __h.resize( new_size );

    __v.reserve( new_size );
  }

  // ==============================================================================
  /// updates const iterators
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::__update_iterators( ) {
    __end.__setAtEnd();
  }





  
  // ==============================================================================
  // ==============================================================================
  // SequenceIterator
  // ==============================================================================
  // ==============================================================================
  template <typename KEY> INLINE
  SequenceIterator<KEY>::SequenceIterator( const Sequence<KEY> * const seq,
                                           Idx pos ):
    __iterator( pos ),__seq( seq ) {
    GUM_CONSTRUCTOR( SequenceIterator );

    if ( pos>__seq->size()-1 ) __iterator=__seq->size()-1;

    __iterator++; // internally : __iterator=0 at begin, in [1,..,size()]
                  // if valid, =size()+1 at end
  }

  template <typename KEY> INLINE
  SequenceIterator<KEY>::SequenceIterator( const SequenceIterator<KEY>& source ):
    __iterator( source.__iterator ),__seq( source.__seq ) {
    GUM_CONS_CPY( SequenceIterator );
  }

  template <typename KEY> INLINE
  SequenceIterator<KEY>::~SequenceIterator() {
    GUM_DESTRUCTOR( SequenceIterator );
  }

  template <typename KEY> INLINE
  SequenceIterator<KEY>& SequenceIterator<KEY>::operator=
  ( const SequenceIterator<KEY>& source ) {
    __iterator=source.__iterator;
    __seq=source.__seq;
    return *this;
  }

  template <typename KEY> INLINE
  SequenceIterator<KEY>& SequenceIterator<KEY>::operator++( ) {
    if ( __iterator<__seq->size()+1 )
      __iterator++;

    return *this;
  }

  template <typename KEY> INLINE
  SequenceIterator<KEY>& SequenceIterator<KEY>::operator--( ) {
    if ( __iterator>0 )
      __iterator--;

    return *this;
  }

  template <typename KEY> INLINE
  bool SequenceIterator<KEY>::operator==
  ( const SequenceIterator<KEY>& source ) const {
    if ( __seq!=source.__seq ) return false;

    if ( __seq->empty() ) return true; // all iterators are the same if seq is empty

    return ( __iterator==source.__iterator );
  }

  template <typename KEY> INLINE
  bool SequenceIterator<KEY>::operator!=
  ( const SequenceIterator<KEY>& source ) const {
    return ! operator==( source );
  }

  template <typename KEY> INLINE
  const KEY& SequenceIterator<KEY>::operator*() const {
    return __seq->atPos( pos() );
  }

  template <typename KEY> INLINE
  const KEY* SequenceIterator<KEY>::operator->() const {
    return &__seq->atPos( pos() );
  }

  template <typename KEY> INLINE
  Idx SequenceIterator<KEY>::pos() const {
    if ( __iterator==0 ) GUM_ERROR( OperationNotAllowed,"iterator is begin()" );

    if ( __iterator==__seq->size()+1 )
      GUM_ERROR( OperationNotAllowed,"iterator is end()" );

    return  __iterator-1 ;// internally : __iterator=0 at begin, in [1,..,size()]
                          // if valid, =size()+1 at end. externally :
                          // from 0 to size()-1
  }

  template <typename KEY> INLINE
  void SequenceIterator<KEY>::__setPos( Idx pos ) {
    if ( pos>__seq->size()-1 ) pos=__seq->size()-1;

    __iterator=pos+1;
  }

  template <typename KEY> INLINE
  void SequenceIterator<KEY>::__setAtBegin( ) {
    __iterator=0;
  }

  template <typename KEY> INLINE
  void SequenceIterator<KEY>::__setAtEnd( ) {
    __iterator=__seq->size()+1;
  }
  
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


} /* namespace gum */

