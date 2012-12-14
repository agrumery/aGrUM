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


#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <limits>

//to ease IDE parser
#include <agrum/core/sequence.h>

namespace gum {



  // ==============================================================================
  // ==============================================================================
  // SequenceIterator
  // ==============================================================================
  // ==============================================================================

  // ==============================================================================
  /// returns the size of the sequence
  // ==============================================================================
  template <typename KEY> INLINE
  Size Sequence<KEY>::size() const  {
    // WARNING: we MUST use __v.size() and not __h.size() because, in the
    // Sequence::erase(x) functions, iterators are updated according to size()
    // after the x has been removed from __v but before it is removed from __h
    // (this is done so as to avoid problems due to the potential exceptions
    // raised by the deletion of x within __h)
    return __v.size();
  }


  template <typename KEY> INLINE
  SequenceIterator<KEY>::SequenceIterator ( const Sequence<KEY>& seq,
      Idx pos ) : __seq ( &seq ) {
    GUM_CONSTRUCTOR ( SequenceIterator );

    if ( pos > __seq->size() )
      __iterator = __seq->size(); // make the iterator point to end
    else
      __iterator = pos;
  }


  template <typename KEY> INLINE
  SequenceIterator<KEY>::SequenceIterator ( const SequenceIterator<KEY>& source ) :
    __iterator ( source.__iterator ),__seq ( source.__seq ) {
    GUM_CONS_CPY ( SequenceIterator );
  }


  template <typename KEY> INLINE
  SequenceIterator<KEY>::~SequenceIterator() {
    GUM_DESTRUCTOR ( SequenceIterator );
  }


  template <typename KEY> INLINE
  SequenceIterator<KEY>& SequenceIterator<KEY>::operator=
  ( const SequenceIterator<KEY>& source ) {
    __iterator = source.__iterator;
    __seq = source.__seq;
    return *this;
  }


  template <typename KEY> INLINE
  SequenceIterator<KEY>& SequenceIterator<KEY>::operator++( ) {
    if ( __iterator < __seq->size() )
      ++__iterator;
    else
      __iterator = __seq->size();

    return *this;
  }


  template <typename KEY> INLINE
  SequenceIterator<KEY>& SequenceIterator<KEY>::operator--( ) {
    if ( __iterator != std::numeric_limits<Idx>::max() )
      --__iterator;
    else
      __iterator = std::numeric_limits<Idx>::max();

    return *this;
  }


  template <typename KEY> INLINE
  bool SequenceIterator<KEY>::operator==
  ( const SequenceIterator<KEY>& source ) const {
    if ( __seq->empty() )
      return true; // all iterators are the same if seq is empty

    if ( ( __iterator != source.__iterator ) ||
         ( __seq != source.__seq ) )
      return false;

    return true;
  }


  template <typename KEY> INLINE
  bool SequenceIterator<KEY>::operator!=
  ( const SequenceIterator<KEY>& source ) const {
    return ! operator== ( source );
  }


  template <typename KEY> INLINE
  Idx SequenceIterator<KEY>::pos() const {
    if ( __iterator == std::numeric_limits<Idx>::max() ) {
      GUM_ERROR ( UndefinedIteratorValue, "iterator is rend()" );
    }

    if ( __iterator >= __seq->size() ) {
      GUM_ERROR ( UndefinedIteratorValue, "iterator is end()" );
    }

    return  __iterator;
  }


  template <typename KEY> INLINE
  const KEY& SequenceIterator<KEY>::operator*() const {
    return * ( __seq->__v[ pos()] );
  }

  template <typename KEY> INLINE
  const KEY* SequenceIterator<KEY>::operator->() const {
    return  __seq->__v[ pos()];
  }


  template <typename KEY> INLINE
  void SequenceIterator<KEY>::__setPos ( Idx pos ) {
    if ( pos > __seq->size() ) __iterator = __seq->size();
    else __iterator = pos;
  }


  template <typename KEY> INLINE
  void SequenceIterator<KEY>::__setAtRend( ) {
    __iterator = std::numeric_limits<Idx>::max();
  }


  template <typename KEY> INLINE
  void SequenceIterator<KEY>::__setAtEnd( ) {
    __iterator = __seq->size();
  }








  // ==============================================================================
  // ==============================================================================
  // SequenceIterator
  // ==============================================================================
  // ==============================================================================


  // ==============================================================================
  /// updates const iterators
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::__update_end( ) {
    __end.__setAtEnd();
  }

  // ==============================================================================
  /// clear the sequence
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::clear() {
    __h.clear();
    __v.clear();
    __update_end();
  }

  // ==============================================================================
  /** insert an element at the end of the sequence. private version for internal
   * use */
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::__insert ( const KEY& k ) {
    if ( __h.exists ( k ) ) {
      GUM_ERROR ( DuplicateElement,"Key already in sequence" );
    }

    // k will be added at the end. Insert the new key into the hashtable
    KEY& new_key = const_cast<KEY&> ( __h.insertAndGetKey ( k, __h.size() ) );

    __v.push_back ( &new_key );
  }

  // ==============================================================================
  /// clears the current sequence and fill it with copies the element of aSeq
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::__copy ( const Sequence<KEY>& aSeq ) {
    clear();

    for ( unsigned int i = 0; i < aSeq.size(); ++i )
      __insert ( * ( aSeq.__v[i] ) );

    __update_end();
  }

  // ==============================================================================
  /// Default constructor
  // ==============================================================================
  template <typename KEY>
  Sequence<KEY>::Sequence() : __h(), __v(), __end ( *this ), __rend ( *this ) {
    // for debugging purposes
    GUM_CONSTRUCTOR ( Sequence );
    __rend.__setAtRend();
    __end.__setAtEnd();
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  template <typename KEY>
  Sequence<KEY>::Sequence ( const Sequence<KEY>& aSeq ) :
    __h(), __v(), __end ( *this ), __rend ( *this ) {
    // for debugging purposes
    GUM_CONS_CPY ( Sequence );
    __copy ( aSeq );
    __rend.__setAtRend();
    __end.__setAtEnd();
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template <typename KEY>
  Sequence<KEY>::~Sequence() {
    GUM_DESTRUCTOR ( Sequence );
  }

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  template <typename KEY> INLINE
  Sequence<KEY>& Sequence<KEY>::operator= ( const Sequence<KEY>& aSeq ) {
    // avoid self assignment
    if ( &aSeq != this ) {
      __copy ( aSeq );
    }

    return *this;
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
  bool Sequence<KEY>::exists ( const KEY& k ) const  {
    return __h.exists ( k );
  }

  // ==============================================================================
  /// insert an element at the end of the sequence
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::insert ( const KEY& k ) {
    __insert ( k );
    __update_end();
  }

  // ==============================================================================
  // returns the object at the pos i
  // ==============================================================================
  template <typename KEY> INLINE
  const KEY& Sequence<KEY>::atPos ( Idx i ) const {
    if ( i >= __h.size() ) {
      GUM_ERROR ( NotFound, "" );
    }

    return * ( __v[i] );
  }

  // ==============================================================================
  /// returns the position of the object passed in argument (if it exists)
  // ==============================================================================
  template <typename KEY> INLINE
  Idx Sequence<KEY>::pos ( const KEY& key ) const {
    return __h[key];
  }


  // ==============================================================================
  /// insert k in the sequence (synonym for insert)
  // ==============================================================================
  template <typename KEY> INLINE
  Sequence<KEY>& Sequence<KEY>::operator<< ( const KEY& k ) {
    insert ( k );
    return *this;
  }

  // ==============================================================================
  /// remove an element from the sequence
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::erase ( const KEY& k ) {
    // get the position of the element to remove
    Idx pos;

    try { pos = __h[k]; }
    catch ( NotFound& ) { return; }

    // erase the element
    __v.erase ( __v.begin() +pos );

    for ( Idx i = pos; i < __v.size(); ++i ) {
      --__h[ * ( __v[i] )];
    }

    __update_end();
    __h.erase ( k );
  }

  // ==============================================================================
  /// remove from the sequence the element pointed to by the iterator
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::erase ( const iterator& iter ) {
    if ( iter.pos() >= size() ) return;

    // erase the element
    Idx  pos = iter.pos();
    KEY* key = __v[pos];
    __v.erase ( __v.begin() +pos );

    for ( Idx i = pos; i < __v.size(); ++i ) {
      --__h[ * ( __v[i] )];
    }

    __update_end();
    __h.erase ( *key );
  }

  // ==============================================================================
  /// remove k in the sequence (synonym for remove)
  // ==============================================================================
  template <typename KEY> INLINE
  Sequence<KEY>& Sequence<KEY>::operator>> ( const KEY& k )  {
    erase ( k );
    return *this;
  }

  // ==============================================================================
  // returns the object at the pos i
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::setAtPos ( Idx i,const KEY& newKey ) {
    if ( i >= __h.size() ) {
      GUM_ERROR ( NotFound, "index too large" );
    }

    if ( __h.exists ( newKey ) ) {
      GUM_ERROR ( DuplicateElement,"Key already in sequence" );
    }

    __h.erase ( * ( __v[i] ) );
    KEY& new_key = const_cast<KEY&> ( __h.insertAndGetKey ( newKey,i ) );
    __v[i] = &new_key;
  }

  // ==============================================================================
  /// returns the object at the pos i
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY>::swap ( Idx i,Idx j ) {
    if ( i == j ) return;

    KEY& ki = const_cast<KEY&> ( atPos ( i ) );
    KEY& kj = const_cast<KEY&> ( atPos ( j ) );

    __h[ki]=j;
    __h[kj]=i;

    __v[i] = &kj;
    __v[j] = &ki;
  }

  // ==============================================================================
  /// returns the first element
  // ==============================================================================
  template <typename KEY> INLINE
  const KEY& Sequence<KEY>::front() const {
    return atPos ( 0 );
  }

  // ==============================================================================
  /// returns the last element
  // ==============================================================================
  template <typename KEY> INLINE
  const KEY& Sequence<KEY>::back() const {
    return atPos ( size() - 1 );
  }

  // ==============================================================================
  /// returns the index of object k if it exists
  // ==============================================================================
  template <typename KEY> INLINE
  Idx Sequence<KEY>::operator[] ( const KEY& k ) const {
    return __h[k];
  }

  // ==============================================================================
  /// returns the element at position i (synonym for atPos)
  // ==============================================================================
  template <typename KEY> INLINE
  const KEY& Sequence<KEY>::operator[] ( Idx i ) const {
    return atPos ( i );
  }

  // ==============================================================================
  /// Print a sequence
  // ==============================================================================
  template <typename KEY>
  std::string Sequence<KEY>::toString() const {
    std::stringstream stream;
    stream<<"[";

    if ( ! __h.empty() ) {
      stream<<0<<":"<<  *__v[0] ;

      for ( Idx i=1; i<__h.size(); ++i ) {
        stream<<" - "<<i<<":"<<  *__v[i] ;
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
  bool Sequence<KEY>::operator== ( const Sequence<KEY>& k ) const {
    if ( size() != k.size() ) return false;
    else {
      for ( Idx i=0; i < size(); ++i )
        if ( *__v[i] != * ( k.__v[i] ) ) return false;
    }

    return true;
  }

  // ==============================================================================
  /// returns true if the content of k is different from that of *this
  // ==============================================================================
  template <typename KEY> INLINE
  bool Sequence<KEY>::operator!= ( const Sequence<KEY>& k ) const {
    return !operator== ( k );
  }

  // ==============================================================================
  /// a << operator for displaying the content of the Sequence
  // ==============================================================================
  template <typename KEY> INLINE
  std::ostream& operator<< ( std::ostream& stream, const Sequence<KEY>& seq ) {
    stream << seq.toString();
    return stream;
  }

  // ==============================================================================
  /// returns a value to 0
  // ==============================================================================
  template <typename KEY> INLINE
  SequenceIterator<KEY> Sequence<KEY>::begin() const {
    return SequenceIterator<KEY> ( *this );
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
    SequenceIterator<KEY> it ( *this );
    it.__setPos ( size()-1 );
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
  void Sequence<KEY>::resize ( unsigned int new_size ) {
    if ( new_size < __h.size() ) return;

    __h.resize ( new_size );
    __v.reserve ( new_size );
  }


  // ==============================================================================
  /// returns the set difference : this \ seq
  // ==============================================================================
  template <typename KEY> INLINE
  Set<KEY> Sequence<KEY>::diffSet ( const Sequence<KEY>& seq ) const {
    Set<KEY> res;

    for ( typename Sequence<KEY>::iterator iter = this->begin(); iter != this->end(); ++iter ) {
      if ( ! seq.exists ( *iter ) ) res<<*iter;
    }

    return res;
  }






  // ##############################################################################
  // ##############################################################################
  //
  // BELOW, CLASSES ARE SPECIALIZATIONS DESIGNED TO OPTIMIZE SEQUENCES OF POINTERS
  //
  // ##############################################################################
  // ##############################################################################


  // ==============================================================================
  // ==============================================================================
  // SequenceIterator optimized for pointers
  // ==============================================================================
  // ==============================================================================

  // ==============================================================================
  /// returns the size of the sequence
  // ==============================================================================
  template <typename KEY> INLINE
  Size Sequence<KEY*>::size() const  {
    // note that, unlike function Sequence<KEY>::size(), here no exception can
    // be raised within functions Sequence<KEY*>::erase. Hence, in this function,
    // __update_end() can be called after both __h and __v, which enables us to
    // define size() as either __v.size() or __h.size()
    return __h.size();
  }

  template <typename KEY> INLINE
  SequenceIterator<KEY*>::SequenceIterator ( const Sequence<KEY*>& seq,
      Idx pos ) : __seq ( &seq ) {
    GUM_CONSTRUCTOR ( SequenceIterator );

    if ( pos > __seq->size() )
      __iterator = __seq->size(); // make the iterator point to end
    else
      __iterator = pos;
  }


  template <typename KEY> INLINE
  SequenceIterator<KEY*>::SequenceIterator ( const SequenceIterator<KEY*>& source ) :
    __iterator ( source.__iterator ),__seq ( source.__seq ) {
    GUM_CONS_CPY ( SequenceIterator );
  }


  template <typename KEY> INLINE
  SequenceIterator<KEY*>::~SequenceIterator() {
    GUM_DESTRUCTOR ( SequenceIterator );
  }


  template <typename KEY> INLINE
  SequenceIterator<KEY*>& SequenceIterator<KEY*>::operator=
  ( const SequenceIterator<KEY*>& source ) {
    __iterator = source.__iterator;
    __seq = source.__seq;
    return *this;
  }


  template <typename KEY> INLINE
  SequenceIterator<KEY*>& SequenceIterator<KEY*>::operator++( ) {
    if ( __iterator < __seq->size() )
      ++__iterator;
    else
      __iterator = __seq->size();

    return *this;
  }


  template <typename KEY> INLINE
  SequenceIterator<KEY*>& SequenceIterator<KEY*>::operator--( ) {
    if ( __iterator != std::numeric_limits<Idx>::max() )
      --__iterator;
    else
      __iterator = std::numeric_limits<Idx>::max();

    return *this;
  }


  template <typename KEY> INLINE
  bool SequenceIterator<KEY*>::operator==
  ( const SequenceIterator<KEY*>& source ) const {
    if ( __seq->empty() )
      return true; // all iterators are the same if seq is empty

    if ( ( __iterator != source.__iterator ) ||
         ( __seq != source.__seq ) )
      return false;

    return true;
  }


  template <typename KEY> INLINE
  bool SequenceIterator<KEY*>::operator!=
  ( const SequenceIterator<KEY*>& source ) const {
    return ! operator== ( source );
  }


  template <typename KEY> INLINE
  Idx SequenceIterator<KEY*>::pos() const {
    if ( __iterator == std::numeric_limits<Idx>::max() ) {
      GUM_ERROR ( UndefinedIteratorValue, "iterator is rend()" );
    }

    if ( __iterator >= __seq->size() ) {
      GUM_ERROR ( UndefinedIteratorValue, "iterator is end()" );
    }

    return  __iterator;
  }


  template <typename KEY> INLINE
  KEY* const SequenceIterator<KEY*>::operator*() const {
    return __seq->__v[ pos()];
  }

  template <typename KEY> INLINE
  void SequenceIterator<KEY*>::__setPos ( Idx pos ) {
    if ( pos > __seq->size() ) __iterator = __seq->size();
    else __iterator = pos;
  }


  template <typename KEY> INLINE
  void SequenceIterator<KEY*>::__setAtRend( ) {
    __iterator = std::numeric_limits<Idx>::max();
  }


  template <typename KEY> INLINE
  void SequenceIterator<KEY*>::__setAtEnd( ) {
    __iterator = __seq->size();
  }





  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                 GUM_SEQUENCE OPTIMIZED FOR POINTERS                 === */
  /* =========================================================================== */
  /* =========================================================================== */


  // ==============================================================================
  /// updates const iterators
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY*>::__update_end( ) {
    __end.__setAtEnd();
  }

  // ==============================================================================
  /// clear the sequence
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY*>::clear() {
    __h.clear();
    __v.clear();
    __update_end();
  }

  // ==============================================================================
  /** insert an element at the end of the sequence. private version for internal
   * use */
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY*>::__insert ( KEY* const k ) {
    if ( __h.exists ( k ) ) {
      GUM_ERROR ( DuplicateElement,"Key already in sequence" );
    }

    // k will be added at the end. Insert the new key into the hashtable
    __h.insert ( k, __h.size() );
    __v.push_back ( k );
  }

  // ==============================================================================
  /// clears the current sequence and fill it with copies the element of aSeq
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY*>::__copy ( const Sequence<KEY*>& aSeq ) {
    clear();

    for ( unsigned int i = 0; i < aSeq.size(); ++i )
      __insert ( aSeq.__v[i] );

    __update_end();
  }

  // ==============================================================================
  /// Default constructor
  // ==============================================================================
  template <typename KEY>
  Sequence<KEY*>::Sequence() : __h(), __v(), __end ( *this ), __rend ( *this ) {
    // for debugging purposes
    GUM_CONSTRUCTOR ( Sequence );
    __rend.__setAtRend();
    __end.__setAtEnd();
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  template <typename KEY>
  Sequence<KEY*>::Sequence ( const Sequence<KEY*>& aSeq ) :
    __h ( aSeq.__h ), __v ( aSeq.__v ), __end ( *this ), __rend ( *this ) {
    // for debugging purposes
    GUM_CONS_CPY ( Sequence );
    __rend.__setAtRend();
    __end.__setAtEnd();
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template <typename KEY>
  Sequence<KEY*>::~Sequence() {
    GUM_DESTRUCTOR ( Sequence );
  }

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  template <typename KEY> INLINE
  Sequence<KEY*>& Sequence<KEY*>::operator= ( const Sequence<KEY*>& aSeq ) {
    // avoid self assignment
    if ( &aSeq != this ) {
      __copy ( aSeq );
    }

    return *this;
  }

  // ==============================================================================
  /// return true if empty
  // ==============================================================================
  template <typename KEY> INLINE
  bool Sequence<KEY*>::empty() const  {
    return __h.empty();
  }

  // ==============================================================================
  /// check the existence of k in the sequence
  // ==============================================================================
  template <typename KEY> INLINE
  bool Sequence<KEY*>::exists ( KEY* const k ) const  {
    return __h.exists ( k );
  }

  // ==============================================================================
  /// insert an element at the end of the sequence
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY*>::insert ( KEY* const k ) {
    __insert ( k );
    __update_end();
  }

  // ==============================================================================
  // returns the object at the pos i
  // ==============================================================================
  template <typename KEY> INLINE
  KEY* const Sequence<KEY*>::atPos ( Idx i ) const {
    if ( i >= __h.size() ) {
      GUM_ERROR ( NotFound, "" );
    }

    return __v[i];
  }

  // ==============================================================================
  /// returns the position of the object passed in argument (if it exists)
  // ==============================================================================
  template <typename KEY> INLINE
  Idx Sequence<KEY*>::pos ( KEY* const key ) const {
    return __h[key];
  }

  // ==============================================================================
  /// insert k in the sequence (synonym for insert)
  // ==============================================================================
  template <typename KEY> INLINE
  Sequence<KEY*>& Sequence<KEY*>::operator<< ( KEY* const k ) {
    insert ( k );
    return *this;
  }

  // ==============================================================================
  /// remove an element from the sequence
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY*>::erase ( KEY* const k ) {
    // get the position of the element to remove
    Idx pos;

    try { pos = __h[k]; }
    catch ( NotFound& ) { return; }

    // erase the element
    __v.erase ( __v.begin() +pos );

    for ( Idx i = pos; i < __v.size(); ++i ) {
      --__h[ __v[i] ];
    }

    __h.erase ( k );
    __update_end();
  }

  // ==============================================================================
  /// remove from the sequence the element pointed to by the iterator
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY*>::erase ( const iterator& iter ) {
    if ( iter.pos() >= size() ) return;

    // erase the element
    Idx  pos = iter.pos();
    KEY* key = __v[pos];
    __v.erase ( __v.begin() +pos );

    for ( Idx i = pos; i < __v.size(); ++i ) {
      --__h[ __v[i] ];
    }

    __h.erase ( key );
    __update_end();
  }

  // ==============================================================================
  /// remove k in the sequence (synonym for remove)
  // ==============================================================================
  template <typename KEY> INLINE
  Sequence<KEY*>& Sequence<KEY*>::operator>> ( KEY* const k )  {
    erase ( k );
    return *this;
  }

  // ==============================================================================
  // returns the object at the pos i
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY*>::setAtPos ( Idx i, KEY* const newKey ) {
    if ( i >= __h.size() ) {
      GUM_ERROR ( NotFound, "index too large" );
    }

    if ( __h.exists ( newKey ) ) {
      GUM_ERROR ( DuplicateElement,"Key already in sequence" );
    }

    __h.erase ( __v[i] );
    __h.insert ( newKey,i );
    __v[i] = newKey;
  }

  // ==============================================================================
  /// returns the object at the pos i
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY*>::swap ( Idx i,Idx j ) {
    if ( i == j ) return;

    KEY* const ki = atPos ( i );
    KEY* const kj = atPos ( j );

    __h[ki]=j;
    __h[kj]=i;

    __v[i] = kj;
    __v[j] = ki;
  }

  // ==============================================================================
  /// returns the first element
  // ==============================================================================
  template <typename KEY> INLINE
  KEY* const Sequence<KEY*>::front() const {
    return atPos ( 0 );
  }

  // ==============================================================================
  /// returns the last element
  // ==============================================================================
  template <typename KEY> INLINE
  KEY* const Sequence<KEY*>::back() const {
    return atPos ( size() - 1 );
  }

  // ==============================================================================
  /// returns the index of object k if it exists
  // ==============================================================================
  template <typename KEY> INLINE
  Idx Sequence<KEY*>::operator[] ( KEY* const k ) const {
    return __h[k];
  }

  // ==============================================================================
  /// returns the element at position i (synonym for atPos)
  // ==============================================================================
  template <typename KEY> INLINE
  KEY* const Sequence<KEY*>::operator[] ( Idx i ) const {
    return atPos ( i );
  }

  // ==============================================================================
  /// Print a sequence
  // ==============================================================================
  template <typename KEY>
  std::string Sequence<KEY*>::toString() const {
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

  // ==============================================================================
  /// returns true if the content of k equals that of *this
  // ==============================================================================
  template <typename KEY> INLINE
  bool Sequence<KEY*>::operator== ( const Sequence<KEY*>& k ) const {
    if ( size() != k.size() ) return false;
    else {
      for ( Idx i=0; i < size(); ++i )
        if ( __v[i] != k.__v[i] ) return false;
    }

    return true;
  }

  // ==============================================================================
  /// returns true if the content of k is different from that of *this
  // ==============================================================================
  template <typename KEY> INLINE
  bool Sequence<KEY*>::operator!= ( const Sequence<KEY*>& k ) const {
    return !operator== ( k );
  }


  // ==============================================================================
  /// returns a value to 0
  // ==============================================================================
  template <typename KEY> INLINE
  SequenceIterator<KEY*> Sequence<KEY*>::begin() const {
    return SequenceIterator<KEY*> ( *this );
  }

  // ==============================================================================
  /// return a value to end=size()
  // ==============================================================================
  template <typename KEY> INLINE
  const SequenceIterator<KEY*>& Sequence<KEY*>::end() const  {
    return __end;
  }

  // ==============================================================================
  /// return a value to terminal =size()-1
  // ==============================================================================
  template <typename KEY> INLINE
  SequenceIterator<KEY*> Sequence<KEY*>::rbegin() const {
    SequenceIterator<KEY*> it ( *this );
    it.__setPos ( size()-1 );
    return it;
  }

  // ==============================================================================
  /// returns a value to 0
  // ==============================================================================
  template <typename KEY> INLINE
  const SequenceIterator<KEY*>& Sequence<KEY*>::rend() const  {
    return __rend;
  }

  // ==============================================================================
  /// modifies the size of the internal structures of the sequence
  // ==============================================================================
  template <typename KEY> INLINE
  void Sequence<KEY*>::resize ( unsigned int new_size ) {
    if ( new_size < __h.size() ) return;

    __h.resize ( new_size );
    __v.reserve ( new_size );
  }
  // ==============================================================================
  /// returns the set difference : this \ seq
  // ==============================================================================
  template <typename KEY> INLINE
  Set<KEY*> Sequence<KEY*>::diffSet ( const Sequence<KEY*>& seq ) const {
    Set<KEY*> res;

    for ( typename Sequence<KEY*>::iterator iter = seq.begin(); iter != seq.end(); ++iter ) {
      if ( ! this->exists ( *iter ) ) res<<*iter;
    }

    return res;
  }
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

