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
 * @brief Implementation of the HashTable
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <iostream>
#include <sstream>
#include <string>


#ifndef DOXYGEN_SHOULD_SKIP_THIS



/* ############################################################################# */
/* #                           WARNING:  DEPENDENCIES                          # */
/* ############################################################################# */
/*
 * agrum/core/set.tcc: to speed-up accessors in sets, we rely on the fact (which
 * holds currently) that hashTable's iterators rend are never modified by
 * insertions or deletions of elements in hash tables. If this property were to be
 * changed, set.tcc should be updated accordingly
 */
/* ############################################################################# */




namespace gum {


  
  /* =========================================================================== */
  /* =========================================================================== */
  /* ===     IMPLEMENTATION OF THE CHAINED LISTS USED IN THE HASH TABLES     === */
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// a function used to perform copies of HashTableLists
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTableList<Key,Val>::_copy( const HashTableList<Key, Val> &from ) {
    HashTableBucket<Key, Val> *ptr, *old_ptr, *new_elt = 0;
    // set the defaults
    __deb_list = 0;
    
    // copy from's list
    try {
      for ( ptr = from.__deb_list, old_ptr = 0; ptr; ptr = ptr->next ) {
        // copy the current from's bucket (may throw an exception either because
        // new cannot allocate the bucket or because the copy constructor of Val
        // throws an exception)
        new_elt = new HashTableBucket<Key,Val>( *ptr );

        // rechain properly the new list
        new_elt->prev = old_ptr;

        if ( old_ptr ) old_ptr->next = new_elt;
        else __deb_list = new_elt;

        old_ptr = new_elt;
      }

      if ( old_ptr ) old_ptr->next = 0;

      // update the number of elements stored into the list and the end of the list
      __nb_elements = from.__nb_elements;
      __end_list = new_elt;
    }
    catch ( ... ) {
      // problem: we could not allocate an element in the list => we delete
      // the elements created so far and we throw an exception
      for ( ; __deb_list != old_ptr; __deb_list = ptr ) {
        ptr = __deb_list->next;
        delete __deb_list;
      }

      if ( old_ptr ) delete old_ptr;
      __nb_elements = 0;
      __end_list = 0;

      throw;
    }
  }

  // ==============================================================================
  /// returns the bucket corresponding to a given key
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableBucket<Key,Val>*
  HashTableList<Key,Val>::_getBucket( const Key& key ) const  {
    for ( HashTableBucket<Key, Val> *ptr = __deb_list; ptr; ptr = ptr->next )
      if ( ptr->key == key ) return ptr;

    return 0;
  }

  // ==============================================================================
  // suppresses an element from a chained list
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTableList<Key,Val>::_erase( const HashTableBucket<Key, Val>* ptr ) {
    // check that the pointer is not NULL
    if ( ptr == 0 )
      GUM_ERROR( NullElement, "trying to erase a NULL bucket" );

    // relink properly the doubly chained list
    if ( ptr->prev )
      ptr->prev->next = ptr->next;
    else
      __deb_list = ptr->next;

    if ( ptr->next )
      ptr->next->prev = ptr->prev;
    else
      __end_list = ptr->prev;

    // remove the current element from the list
    delete ptr;

    --__nb_elements;
  }

  // ==============================================================================
  // basic constructor that creates an empty list
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableList<Key,Val>::HashTableList()  :
    __deb_list( 0 ), __end_list( 0 ), __nb_elements( 0 ) {
  }

  // ==============================================================================
  // copy constructor
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableList<Key,Val>::HashTableList( const HashTableList<Key,Val>&from ) {
    _copy( from );
  }

  // ==============================================================================
  // assignment operator
  // ==============================================================================
  template <typename Key, typename Val>
  HashTableList<Key,Val>&
  HashTableList<Key,Val>::operator= ( const HashTableList<Key, Val>& from ) {
    // avoid self assignment
    if ( this != &from ) {
      // remove the old content of the list
      for ( HashTableBucket<Key, Val> *next_ptr, *ptr = __deb_list;
            ptr; ptr = next_ptr ) {
        next_ptr = ptr->next;
        delete ptr;
      }

      // perform the copy
      _copy( from );
    }

    return *this;
  }

  // ==============================================================================
  // destructor
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableList<Key,Val>::~HashTableList() {
    HashTableBucket<Key, Val> *ptr, *next_ptr;

    for ( ptr = __deb_list; ptr; ptr = next_ptr ) {
      next_ptr = ptr->next;
      delete ptr;
    }
  }

  // ==============================================================================
  // function at returns the ith element in the current chained list
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val& HashTableList<Key,Val>::at( unsigned int i ) {
    if ( i >= __nb_elements )
      GUM_ERROR( NotFound, "not enough elements in the chained list" );

    HashTableBucket<Key, Val> *ptr;
    for ( ptr = __deb_list; i; --i, ptr = ptr->next ) {}

    return ptr->val;
  }

  // ==============================================================================
  // function at returns the ith element in the current chained list
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Val& HashTableList<Key,Val>::at( unsigned int i ) const {
    if ( i >= __nb_elements )
      GUM_ERROR( NotFound, "not enough elements in the chained list" );

    HashTableBucket<Key, Val> *ptr;
    for ( ptr = __deb_list; i; --i, ptr = ptr->next ) {}

    return ptr->val;
  }
  
  // ==============================================================================
  // operator [] returns the value corresponding to a given key
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Val& HashTableList<Key,Val>::operator[]( const Key& key ) const {
    for ( HashTableBucket<Key,Val> *ptr = __deb_list; ptr; ptr = ptr->next )
      if ( ptr->key == key ) return ptr->val;

    GUM_ERROR( NotFound,
               "hashtable's chained list contains no element with this key" );
  }

  // ==============================================================================
  // operator [] returns the value corresponding to a given key
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val& HashTableList<Key,Val>::operator[]( const Key& key ) {
    for ( HashTableBucket<Key,Val> *ptr = __deb_list; ptr; ptr = ptr->next )
      if ( ptr->key == key ) return ptr->val;

    GUM_ERROR( NotFound,
               "hashtable's chained list contains no element with this key" );
  }

  // ==============================================================================
  // check whether there exists an element with a given key in the list
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTableList<Key,Val>::exists( const Key& key ) const  {
    for ( HashTableBucket<Key, Val> *ptr = __deb_list; ptr; ptr = ptr->next )
      if ( ptr->key == key ) return true;

    return false;
  }

  // ==============================================================================
  // insertion of a new element in the chained list
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableBucket<Key,Val>*
  HashTableList<Key,Val>::_insertAndGetBucket ( const Key& key, const Val& val ) {
    // create a new bucket
    HashTableBucket<Key,Val> *new_elt = new HashTableBucket<Key,Val>( key,val );

    // place the bucket at the beginning of the list
    new_elt->prev = 0;
    new_elt->next = __deb_list;

    if ( __deb_list )
      __deb_list->prev = new_elt;

    __deb_list = new_elt;

    if ( __end_list == 0 )
      __end_list = new_elt;

    ++__nb_elements;

    return new_elt;
  }

  // ==============================================================================
  // insertion of a new element in the chained list
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val& HashTableList<Key,Val>::insert( const Key& key, const Val& val ) {
    return _insertAndGetBucket ( key, val )->val;
  }

  // ==============================================================================
  // insertion of a new element in the chained list
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Key&
  HashTableList<Key,Val>::insertAndGetKey ( const Key& key, const Val& val ) {
    return _insertAndGetBucket ( key, val )->key;
  }

  // ==============================================================================
  // function for deleting all the elements of a chained list
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTableList<Key,Val>::clear() {
    HashTableBucket<Key, Val> *ptr, *next_ptr;

    for ( ptr = __deb_list; ptr; ptr = next_ptr ) {
      next_ptr = ptr->next;
      delete ptr;
    }

    __nb_elements = 0;
    __deb_list = 0;
    __end_list = 0;
  }

  // ==============================================================================
  // returns a boolean indicating whether the chained list is empty
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTableList<Key,Val>::empty() const  {
    return ( __nb_elements == 0 );
  }






  /* =========================================================================== */
  /* =========================================================================== */
  /* ===              GENERIC HASH TABLE ITERATORS IMPLEMENTATION            === */
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// basic constructor: creates an iterator pointing to nothing
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val>::HashTableIterator()  :
    _table( 0 ), _index( 0 ), _bucket( 0 ),
    _next_current_bucket( 0 ), _prev_current_bucket( 0 ),
    _next( 0 ), _prev( 0 ) {}

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val>::HashTableIterator
  ( const HashTableIterator<Key, Val>& from )  :
    _table( from._table ), _index( from._index ), _bucket( from._bucket ),
    _next_current_bucket( from._next_current_bucket ),
    _prev_current_bucket( from._prev_current_bucket ), _prev( 0 ) {
    // rechain properly the list of iterators of the hash table
    if ( _table ) {
      _next = _table->__iterator_list;

      if ( _table->__iterator_list )
        _table->__iterator_list->_prev = this;

      _table->__iterator_list = this;
    }
    else
      _next = 0;
  }

  // ==============================================================================
  /// constructor for an iterator pointing to the nth element of a hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val>::HashTableIterator
  ( const HashTable<Key,Val>& tab, Size ind_elt ) :
    _table( &tab ),_bucket( 0 ),_next_current_bucket( 0 ),
    _prev_current_bucket( 0 ),_prev( 0 ) {
    register Size i;
    // check if it is faster to find the ind_eltth element from the start or
    // from the end of the hashtable

    if ( ind_elt < ( _table->__nb_elements >> 1 ) ) {
      // find the element we shall point to from the start of the hashtable
      for ( i = 0;; ++i ) // no test on i since ind_elt < _table->__nb_elements
        if ( _table->__nodes[i].__nb_elements ) {
          if ( ind_elt >= _table->__nodes[i].__nb_elements )
            ind_elt -= _table->__nodes[i].__nb_elements;
          else {
            for ( _bucket = _table->__nodes[i].__deb_list; ind_elt;
                  --ind_elt, _bucket = _bucket->next ) {}

            _index = i;
            _prev_current_bucket = _bucket;
            _next_current_bucket = _bucket;

            break;
          }
        }
    }
    else {
      // ind_elt = the index of the element we should point to
      // check if the index passed as parameter is valid
      if ( ind_elt >= _table->__nb_elements )
        GUM_ERROR( UndefinedIteratorValue,"Not enough elements in the hashtable" );

      // find the element we shall point to from the end of the hashtable
      for ( i = _table->__size - 1,
              ind_elt = _table->__nb_elements - ind_elt - 1;; --i )
        if ( _table->__nodes[i].__nb_elements ) {
          if ( ind_elt >= _table->__nodes[i].__nb_elements )
            ind_elt -= _table->__nodes[i].__nb_elements;
          else {
            for ( _bucket = _table->__nodes[i].__end_list; ind_elt;
                  --ind_elt, _bucket = _bucket->prev ) {}
            _index = i;
            _prev_current_bucket = _bucket;
            _next_current_bucket = _bucket;

            break;
          }
        }
    }

    // rechain properly the iterator's list of the hashtable
    if ( _table->__iterator_list ) {
      _next = _table->__iterator_list;
      _table->__iterator_list->_prev = this;
    }
    else
      _next = 0;

    _table->__iterator_list = this;
  }

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val>& HashTableIterator<Key,Val>::operator=
  ( const HashTableIterator<Key,Val>& from )  {
    // here, no need to avoid self assignment: this would slow down normal
    // assignments and, in any case, this would not result in an iterator in
    // an incoherent state
    // check if the current hashtable is different from that of "from". In such
    // a case, we shall remove the iterator from its current hashtable iterator's
    // list and add it to the new hashtable iterator's list
    if ( _table != from._table ) {
      // remove the iterator from its hashtable iterator's list'
      if ( _prev )
        _prev->_next = _next;
      else if ( _table )
        _table->__iterator_list = _next;

      if ( _next )
        _next->_prev = _prev;

      // add to the new one
      _prev = 0;
      if ( from._table ) {
        _next = from._table->__iterator_list;

        if ( from._table->__iterator_list )
          from._table->__iterator_list->_prev = this;

        from._table->__iterator_list = this;
      }
      else
        _next = 0;
    }

    _table = from._table;
    _index = from._index;
    _bucket = from._bucket;
    _prev_current_bucket = from._prev_current_bucket;
    _next_current_bucket = from._next_current_bucket;

    return *this;
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val>::~HashTableIterator()  {
    // remove the iterator from the table's iterator list
    if ( _prev )
      _prev->_next = _next;
    else if ( _table )
      _table->__iterator_list = _next;

    if ( _next )
      _next->_prev = _prev;
  }

  // ==============================================================================
  /// returns the key corresponding to the current iterator's bucket
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Key& HashTableIterator<Key,Val>::key() const {
    if ( _bucket )
      return _bucket->key;
    else
      GUM_ERROR( UndefinedIteratorValue, "Accessing a NULL object" );
  }

  // ==============================================================================
  /** @brief makes the iterator point toward nothing (in particular, it is not
   * related anymore to its current hash table) */
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTableIterator<Key,Val>::clear()  {
    // remove the iterator from the table's iterator list
    if ( _prev )
      _prev->_next = _next;
    else if ( _table )
      _table->__iterator_list = _next;

    if ( _next )
      _next->_prev = _prev;

    // set its table as well as the element it points to to 0
    _table = 0;
    _index = 0;
    _bucket = 0;
    _next_current_bucket = 0;
    _prev_current_bucket = 0;
    _next = 0;
    _prev = 0;
  }

  // ==============================================================================
  /// makes the iterator point to the next element in the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val>& HashTableIterator<Key,Val>::operator++()  {
    // if we are not pointing on the last element of the chained list, just
    // point on the next bucket in this list
    if ( _next_current_bucket && _next_current_bucket->next ) {
      _bucket = _next_current_bucket->next;
      _prev_current_bucket = _bucket;
      _next_current_bucket = _bucket;
      return *this;
    }

    // if we are pointing on 0, i.e., we are at the beginning of the chained
    // list, and the list contains some elements, point on the first one
    if ( !_next_current_bucket && _table->__nodes[_index].__nb_elements ) {
      _bucket = _table->__nodes[_index].__deb_list;
      _prev_current_bucket = _bucket;
      _next_current_bucket = _bucket;
      return *this;
    }

    // arrived here, we need to parse the hash table until we find a new bucket
    // because we are pointing on a chained list with no more element to the right
    // of the current element
    for ( Size i = _index + 1UL; i < _table->__size; ++i )
      if ( _table->__nodes[i].__nb_elements ) {
        _index = i;
        _bucket = _table->__nodes[i].__deb_list;
        _prev_current_bucket = _bucket;
        _next_current_bucket = _bucket;
        return *this;
      }

    // we are at the end of the hash table, do nothing
    _index = _table->__size;
    _bucket = 0;
    _prev_current_bucket = 0;
    _next_current_bucket = 0;

    return *this;
  }

  // ==============================================================================
  /// makes the iterator point to the preceding element in the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val>& HashTableIterator<Key,Val>::operator--()  {
    // if we are not pointing on the first element of the chained list, just
    // point on the preceding bucket in this list
    if ( _prev_current_bucket && _prev_current_bucket->prev ) {
      _bucket = _prev_current_bucket->prev;
      _prev_current_bucket = _bucket;
      _next_current_bucket = _bucket;
      return *this;
    }

    // if we are pointing on 0, i.e., we are at the end of the chained
    // list, and the list contains some elements, point on the last one
    if ( !_prev_current_bucket && _table->__nodes[_index].__nb_elements ) {
      _bucket = _table->__nodes[_index].__end_list;
      _prev_current_bucket = _bucket;
      _next_current_bucket = _bucket;
      return *this;
    }

    // check if we are not at the beginning of the table
    // it is compulsory to perform this test before entering the for loop below
    // because if index is equal to 0, the for loop would parse unallocated
    // elements of vector node (because index is an unsigned int)
    if ( _index == 0 ) {
      _bucket = 0;
      _prev_current_bucket = _bucket;
      _next_current_bucket = _bucket;
      return *this;
    }

    // arrived here, we need to parse the hash table until we find a new bucket
    // because we are pointing on a chained list with no more element to the left
    // of the current element
    for ( Size i = _index - 1UL; i; --i )
      if ( _table->__nodes[i].__nb_elements ) {
        _index = i;
        _bucket = _table->__nodes[i].__end_list;
        _prev_current_bucket = _bucket;
        _next_current_bucket = _bucket;
        return *this;
      }

    // we are at the beginning of the hash table, do nothing
    _index = 0UL;
    _bucket = _table->__nodes[0].__end_list;
    _prev_current_bucket = _bucket;
    _next_current_bucket = _bucket;

    return *this;
  }

  // ==============================================================================
  /// checks whether two iterators are pointing toward different elements
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTableIterator<Key,Val>::operator!=
  ( const HashTableIterator<Key, Val> &from ) const  {
    return (( _bucket != from._bucket ) ||
            ( _index != from._index ) ||
            ( _table != from._table ) );
  }

  // ==============================================================================
  /// checks whether two iterators are pointing toward the same element
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTableIterator<Key,Val>::operator==
  ( const HashTableIterator<Key, Val> &from ) const  {
    return (( _bucket == from._bucket ) &&
            ( _index == from._index ) &&
            ( _table == from._table ) );
  }

  // ==============================================================================
  /// returns the value pointed to by the iterator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val& HashTableIterator<Key,Val>::operator*()  {
    if ( _bucket )
      return _bucket->val;
    else
      GUM_ERROR( UndefinedIteratorValue, "Accessing a NULL object" );
  }

  // ==============================================================================
  /// dereferences the value pointed to by the iterator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val* HashTableIterator<Key,Val>::operator->() const {
    if ( _bucket )
      return &( _bucket->val );
    else
      GUM_ERROR( UndefinedIteratorValue, "Accessing a NULL object" );
  }

  // ==============================================================================
  /// returns the value pointed to by the iterator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Val& HashTableIterator<Key,Val>::operator*()
    const  {
    if ( _bucket )
      return _bucket->val;
    else
      GUM_ERROR( UndefinedIteratorValue, "Accessing a NULL object" );
  }

  // ==============================================================================
  /// a method used by the hashtables to construct properly end and rend iters
  // ==============================================================================
  template <typename Key, typename Val>  INLINE
  void HashTableIterator<Key,Val>::_initialize
  ( HashTable<Key, Val> *tab, bool forward )  {
    // attach the hash table to the iterator
    _table = tab;
    // fill the iterator with the corresponding bucket
    _bucket = 0;
    _next_current_bucket = 0;
    _prev_current_bucket = 0;

    if ( forward )
      // initialize end()
      _index = _table->__size;
    else
      // initialize rend()
      _index = 0;
  }

  // ==============================================================================
  /// returns the current iterator's bucket
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableBucket<Key,Val>* HashTableIterator<Key,Val>::_getBucket()
    const {
    return _bucket;
  }

  // ==============================================================================
  /// returns the index in the hashtable's node vector pointed to by the iterator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Size HashTableIterator<Key,Val>::_getIndex() const  {
    return _index;
  }  





  
  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                   GENERIC HASH TABLE IMPLEMENTATION                 === */
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  // a function used to perform copies of HashTables
  // ==============================================================================
  template <typename Key, typename Val>
  void HashTable<Key,Val>::_copy( const HashTable<Key,Val>& table ) {
    // in debug mode, check that this and table have '__nodes' arrays of the
    // same size
    GUM_ASSERT( table.__size == __size );

    // try to fill the array of chained lists
    for ( Size i = 0; i < table.__size; ++i ) {
      try {
        __nodes[i] = table.__nodes[i];
      }
      catch ( ... ) {
        // here we could allocate the __nodes[j], j=0..i-1, so we should
        // deallocate them
        for ( Size j=0; j < __size; ++j )
          __nodes[j].clear();
        __nb_elements = 0;

        // create the end() and rend() iterators properly
        __iter_end._initialize( this, true );
        __iter_rend._initialize( this, false );

        // propagate the exception
        throw;
      }
    }

    __nb_elements = table.__nb_elements;

    // create the end() and rend() iterators properly
    __iter_end._initialize( this, true );
    __iter_rend._initialize( this, false );
  }

  // ==============================================================================
  // Default constructor
  // ==============================================================================
  template <typename Key, typename Val>
  void HashTable<Key,Val>::_create( Size size )  {
    // setup the __nodes vector (contains only empty lists)
    __nodes = new HashTableList<Key, Val> [size];
    
    // set up properly the hash function
    __hash_func.resize( size );

    // create the end() and rend() iterators properly
    __iter_end._initialize( this, true );
    __iter_rend._initialize( this, false );
  }

  // ==============================================================================
  // Default constructor
  // ==============================================================================
  template <typename Key, typename Val>
  HashTable<Key,Val>::HashTable( Size size_param,
                                 bool resize_pol, bool key_uniqueness_pol ) :
    __nodes( 0 ),
    // size must be >= 2 else we lose all the bits of the hash function
    __size( 1UL << __hashTableLog2( std::max( 2UL,size_param ) ) ),
    __nb_elements( 0 ),
    __resize_policy( resize_pol ),
    __key_uniqueness_policy( key_uniqueness_pol ),
    __iterator_list( 0 ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( HashTable );

    // finalize the creation
    _create( __size );
  }


#ifndef NDEBUG

  // ==============================================================================
  // DUMMY constructor (just for debug purpose)
  // ==============================================================================
  template <typename Key, typename Val>
  HashTable<Key,Val>::HashTable( std::string dummy, Size size_param,
                                 bool resize_pol, bool key_uniqueness_pol ) :
    __nodes( 0 ),
    // size must be >= 2 else we lose all the bits of the hash function
    __size( 1UL << __hashTableLog2( std::max( 2UL,size_param ) ) ),
    __nb_elements( 0 ),
    __resize_policy( resize_pol ),
    __key_uniqueness_policy( key_uniqueness_pol ),
    __iterator_list( 0 ) {
    // finalize the creation
    _create( __size );
  }

#endif


  // ==============================================================================
  // copy constructor
  // ==============================================================================
  template <typename Key, typename Val>
  HashTable<Key,Val>::HashTable( const HashTable<Key, Val>& table ) :
    __nodes( new HashTableList<Key, Val> [table.__size] ),
    __size( table.__size ),
    __nb_elements( 0 ),
    __resize_policy( table.__resize_policy ),
    __key_uniqueness_policy( table.__key_uniqueness_policy ),
    __iterator_list( 0 ) {
    // for debugging purposes
    GUM_CONS_CPY( HashTable );

    // set up properly the hash function
    __hash_func.resize( __size );
    _copy( table );
  }

  // ==============================================================================
  // copy operator
  // ==============================================================================
  template <typename Key, typename Val>
  HashTable<Key,Val>& HashTable<Key,Val>::operator=
  ( const HashTable<Key, Val> &from ) {
    // avoid self assignment
    if ( this != &from ) {
      // for debugging purposes
      GUM_OP_CPY( HashTable );

      // first remove the current content of the hashtable and make
      // the iterators point to rend
      clear();

      // if sizes of from's and this' __nodes vectors are not the same,
      // we need to remove the current __nodes' array and to create a
      // new array with the correct size
      if ( __size != from.__size ) {
        // keep track of the old set of __nodes and remove it after the new so that
        // if the new throws a bad_alloc, the hashtable is empty but in a coherent
        // state (its size corresponds to the size of its __nodes vector, etc)
        HashTableList<Key, Val> *old_nodes = __nodes;
        __nodes = new HashTableList<Key, Val> [from.__size];
        delete[] old_nodes;
        __size = from.__size;
        
        /* update the hash function : this is important as the computation of
           the hash values heavily depends on the size of the hash table */
        __hash_func.resize( __size );
      }

      __resize_policy = from.__resize_policy;
      __key_uniqueness_policy = from.__key_uniqueness_policy;
      
      // perform the copy
      _copy( from );
    }

    return *this;
  }

  // ==============================================================================
  /// utility used by all destructors
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::_destroy() {
    /* update all the registered iterators: they should now point to NULL
       and their hashtable should be set to NULL */
    while ( __iterator_list )
      __iterator_list->clear();

    // we delete properly all the buckets
    delete[] __nodes;
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template <typename Key, typename Val>
  HashTable<Key,Val>::~HashTable() {
    // for debugging purposes
    GUM_DESTRUCTOR( HashTable );

    // acutally destroy the instance
    _destroy();
  }

  // ==============================================================================
  // the iterator to the end of the hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const HashTableIterator<Key,Val>& HashTable<Key,Val>::end() const  {
    return __iter_end;
  }

  // ==============================================================================
  /// the iterator at the rend (just before the beginning) of the hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const HashTableIterator<Key,Val>& HashTable<Key,Val>::rend() const  {
    return __iter_rend;
  }

  // ==============================================================================
  /// the iterator at the beginning of the hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val> HashTable<Key,Val>::begin() const  {
    // if the table is empty, make the begin and end point to the same element
    if ( __nb_elements == 0 ) return iterator( __iter_end );
    else return iterator( *this, 0 );
  }

  // ==============================================================================
  /// the iterator at the rbegin (the last element) of the hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val> HashTable<Key,Val>::rbegin() const  {
    // if the table is empty, make the rbegin and rend point to the same element
    if ( __nb_elements == 0 ) return iterator( __iter_rend );
    else return iterator( *this, __nb_elements - 1 );
  }

  // ==============================================================================
  /// returns a reference on the value the key of which is passed in argument
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val& HashTable<Key,Val>::operator[]( const Key& key )  {
    return __nodes[__hash_func( key )][key];
  }

  // ==============================================================================
  /// returns a reference on the value the key of which is passed in argument
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Val& HashTable<Key,Val>::operator[]( const Key& key ) const {
    return __nodes[__hash_func( key )][key];
  }

  // ==============================================================================
  /// returns the number of elements in the hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Size HashTable<Key,Val>::size() const  {
    return __nb_elements;
  }

  // ==============================================================================
  /// returns the size of the __nodes vector of the hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Size HashTable<Key,Val>::capacity() const  {
    return __size;
  }

  // ==============================================================================
  /// checks whether there exists an element with a given key in the hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTable<Key,Val>::exists( const Key& key ) const  {
    return __nodes[__hash_func( key )].exists( key );
  }

  // ==============================================================================
  /// enables the user to change dynamically the resizing policy.
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::setResizePolicy( const bool new_policy )  {
    __resize_policy = new_policy;
  }

  // ==============================================================================
  /// returns the current resizing policy.
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTable<Key,Val>::resizePolicy() const  {
    return __resize_policy;
  }

  // ==============================================================================
  /** @brief enables the user to change dynamically the policy for checking
   * whether there can exist several elements in the table having identical keys */
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::setKeyUniquenessPolicy( const bool new_policy )  {
    __key_uniqueness_policy = new_policy;
  }

  // ==============================================================================
  /// returns the current checking policy
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTable<Key,Val>::keyUniquenessPolicy() const  {
    return __key_uniqueness_policy;
  }

  // ==============================================================================
  /// changes the size of the __nodes vector of the hash table
  // ==============================================================================
  template <typename Key, typename Val>
  void HashTable<Key,Val>::resize( Size new_size ) {
    // new_size must be >= 2 else all the bits of the hash function are lost
    new_size = std::max( 2UL,new_size );
    
    // find the real size for allocation (the smallest power of 2 greater
    // than or equal to new_size) and get its base-2 logarithm 
    int log_size = __hashTableLog2( new_size );
    new_size = 1UL << log_size;

    // check if the new size is different from the actual size
    // if not, nothing else need be done
    if ( new_size != __size ) {
      // under automatic resize policy, check if the new size leaves
      // enough space for storing all the current elements
      if ( !__resize_policy ||
           ( __nb_elements <= new_size*GUM_HASHTABLE_DEFAULT_MEAN_VAL_BY_SLOT ) ) {
        // create a new array of __nodes to store the elements
        HashTableList<Key, Val> *new_nodes =
          new HashTableList<Key, Val> [new_size];
        
        // set the new hash function
        __hash_func.resize( new_size );

        // put all the elements of the current __nodes array into the new one
        HashTableBucket<Key, Val> *bucket;
        Size new_hashed_key;
        for ( Size i = 0; i < __size; ++i ) {
          while (( bucket = __nodes[i].__deb_list ) != 0 ) {
            // compute the new hashed key
            new_hashed_key = __hash_func( bucket->key );
            
            // remove the bucket from the list of buckets of the current
            // node vector
            __nodes[i].__deb_list = bucket->next;

            // put the bucket into the new __nodes vector
            bucket->prev = new_nodes[new_hashed_key].__end_list;
            bucket->next = 0;
            if ( new_nodes[new_hashed_key].__end_list )
              new_nodes[new_hashed_key].__end_list->next = bucket;
            new_nodes[new_hashed_key].__end_list = bucket;
            if ( new_nodes[new_hashed_key].__deb_list == 0 )
              new_nodes[new_hashed_key].__deb_list = bucket;
            ++new_nodes[new_hashed_key].__nb_elements;
          }
        }

        // update the size of the hash table
        __size = new_size;

        // substitute the current __nodes array by the new one
        delete[] __nodes;
        __nodes = new_nodes;

        // update end and rend
        __iter_end._initialize( this, true );
        __iter_rend._initialize( this, false );

        // update the iterators
        for ( iterator* iter = __iterator_list; iter; iter = iter->_next ) {
          if ( iter->_bucket )
            iter->_index = __hash_func( iter->_bucket->key );
          else if ( iter->_index == 0 )
            // make iter equal to rend.
            *iter = __iter_rend;
          else
            *iter = __iter_end;
        }
      }
    }
  }

  // ==============================================================================
  /// add a new element (actually a copy of this element) in the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableBucket<Key,Val>*
  HashTable<Key,Val>::_insertAndGetBucket ( const Key& key, const Val& val ) {
    Size hash_key = __hash_func( key );

    if ( __key_uniqueness_policy )
      // check that there does not already exist an element with the same key
      for ( HashTableBucket<Key, Val> *ptr = __nodes[hash_key].__deb_list;
            ptr; ptr = ptr->next )
        if ( ptr->key == key )
          GUM_ERROR( DuplicateElement,
                     "the hashtable contains an element with the same key" );

    // check whether there is sufficient space to insert the new pair
    // if not, resize the current hashtable
    if ( __resize_policy &&
         ( __nb_elements >= __size * GUM_HASHTABLE_DEFAULT_MEAN_VAL_BY_SLOT ) ) {
      resize( __size << 1 );
      hash_key = __hash_func( key );
    }

    // add the new pair
    HashTableBucket<Key,Val>* new_bucket =
      __nodes[hash_key]._insertAndGetBucket ( key, val );
    ++__nb_elements;

    return new_bucket;
  }

  // ==============================================================================
  /// add a new element (actually a copy of this element) in the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val& HashTable<Key,Val>::insert( const Key& thekey, const Val& theval ) {
    return _insertAndGetBucket (thekey,theval)->val;
  }

  // ==============================================================================
  /// add a new element (actually a copy of this element) in the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Key&
  HashTable<Key,Val>::insertAndGetKey ( const Key& thekey, const Val& theval ) {
    return _insertAndGetBucket (thekey,theval)->key;
  }

  // ==============================================================================
  /// returns a reference on the value the key of which is passed in argument
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Val&
  HashTable<Key,Val>::getWithDefault( const Key& key,
                                      const Val& default_value ) const {
    if ( ! exists( key ) ) return insert( key,default_value );
    return operator[]( key );
  }

  // ==============================================================================
  /// returns a reference on the value the key of which is passed in argument
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val&
  HashTable<Key,Val>::getWithDefault( const Key& key,
                                      const Val& default_value) {
    if ( ! exists( key ) ) return insert( key,default_value );
    return operator[]( key );
  }

  // ==============================================================================
  /// add or update a couple (key,value)
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::set( const Key& key,const Val& value ) {
    if ( ! exists( key ) ) {
      insert( key,value );
    }
    else {
      operator[]( key )=value;
    }
  }

  // ==============================================================================
  /// removes a given element from the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::erase( const Key& key ) {
    // get the hashed key
    Size hash = __hash_func( key );
    
    // get the bucket containing the element to erase
    HashTableBucket<Key, Val>* bucket = __nodes[hash]._getBucket( key );

    if ( bucket == 0 ) return;

    // update the registered iterators pointing to this bucket
    for ( iterator* iter = __iterator_list; iter; iter = iter->_next ) {
      if ( iter->_next_current_bucket == bucket )
        iter->_next_current_bucket = bucket->prev;
      if ( iter->_prev_current_bucket == bucket )
        iter->_prev_current_bucket = bucket->next;
      if ( iter->_bucket == bucket )
        iter->_bucket = 0;
    }

    // remove the element from the __nodes vector
    __nodes[hash]._erase( bucket );
    --__nb_elements;
  }
  
  // ==============================================================================
  /// removes a given element from the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::erase( const iterator& elt_iter ) {
    // get the bucket containing the element to erase
    HashTableBucket<Key, Val>* bucket = elt_iter._getBucket();
    if ( bucket == 0 ) return;

    // update the registered iterators pointing to this bucket
    for ( iterator* iter = __iterator_list; iter; iter = iter->_next ) {
      if ( iter->_next_current_bucket == bucket )
        iter->_next_current_bucket = bucket->prev;
      if ( iter->_prev_current_bucket == bucket )
        iter->_prev_current_bucket = bucket->next;
      if ( iter->_bucket == bucket )
        iter->_bucket = 0;
    }

    // remove the element from the __nodes vector
    __nodes[elt_iter._getIndex()]._erase( bucket );
    --__nb_elements;
  }

  // ==============================================================================
  /// removes a given element from the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::eraseByVal( const Val& val ) {
    for ( HashTableIterator<Key,Val> iter = begin(); iter != end(); ++iter )
      if ( *iter == val ) {
        // get the bucket containing the element to erase
        HashTableBucket<Key, Val>* bucket = iter._getBucket();
        Size index = iter._getIndex();
        
        // update the registered iterators pointing to this bucket
        for ( iterator* iter = __iterator_list; iter; iter = iter->_next ) {
          if ( iter->_next_current_bucket == bucket )
            iter->_next_current_bucket = bucket->prev;
          if ( iter->_prev_current_bucket == bucket )
            iter->_prev_current_bucket = bucket->next;
          if ( iter->_bucket == bucket )
            iter->_bucket = 0;
        }

        // remove the element from the __nodes vector
        __nodes[index]._erase( bucket );
        --__nb_elements;

        return;
      }
  }

  // ==============================================================================
  /// remove a (key,...) if exists
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::reset( const Key& key ) {
    erase( key );
  }

  // ==============================================================================
  /// return the (first) key of a given value
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Key& HashTable<Key,Val>::keyByVal( const Val& val ) const {
    for ( HashTableIterator<Key,Val> iter = begin(); iter != end(); ++iter )
      if ( *iter == val )
        return iter.key();

    GUM_ERROR( NotFound, "not enough elements in the chained list" );
  }

  // ==============================================================================
  /// returns a reference on a given key
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Key& HashTable<Key,Val>::key( const Key& key ) const {
    // get the bucket corresponding to the key
    HashTableBucket<Key, Val>* bucket =
      __nodes[__hash_func( key )]._getBucket( key );

    if ( !bucket )
      GUM_ERROR( NotFound, "key does not belong to the hashtable" );

    return bucket->key;
  }

  // ==============================================================================
  /// removes all the elements having a certain value from the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::eraseAllVal( const Val& val ) {
    for ( HashTableIterator<Key,Val> iter = begin(); iter != end(); ++iter )
      if ( *iter == val ) {
        // get the bucket containing the element to erase
        HashTableBucket<Key, Val>* bucket = iter._getBucket();
        Size index = iter._getIndex();

        // update the registered iterators pointing to this bucket
        for ( iterator* iter = __iterator_list; iter; iter = iter->_next ) {
          if ( iter->_next_current_bucket == bucket )
            iter->_next_current_bucket = bucket->prev;
          if ( iter->_prev_current_bucket == bucket )
            iter->_prev_current_bucket = bucket->next;
          if ( iter->_bucket == bucket )
            iter->_bucket = 0;
        }

        // remove the element from the __nodes vector
        __nodes[index]._erase( bucket );
        --__nb_elements;
      }
  }

  // ==============================================================================
  /// removes all the elements in the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::clear() {
    /* update all the registered iterators: they should now point to NULL
       and they are positioned to the rend of the hashtable. */
    for ( iterator* iter = __iterator_list; iter; iter = iter->_next )
      *iter = __iter_rend;

    // remove the buckets
    for ( Size i = 0; i < __size; ++i )
      __nodes[i].clear();
    __nb_elements = 0;
  }

  // ==============================================================================
  /// indicates whether the hash table is empty
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTable<Key,Val>::empty() const  {
    return ( __nb_elements == 0 );
  }

  // ==============================================================================
  /// transforms a hashtable of val into a hashtable of mountains
  // ==============================================================================
  template <typename Key, typename Val>
  template <typename Mount> HashTable<Key,Mount> INLINE
  HashTable<Key,Val>::map( Mount( *f )( Val ), Size size,
                           bool resize_pol, bool key_uniqueness_pol ) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without needing to
    // resize in autmatic resizing mode
    if ( size == 0 )
      size = std::max( 2UL, __nb_elements / 2 );

    // create a new table
    HashTable<Key,Mount> table( __size, resize_pol, key_uniqueness_pol );

    // fill the new hash table
    for ( HashTableIterator<Key,Val> iter = rbegin(); iter != rend(); --iter ) {
      table.insert( iter.key(), f( *iter ) );
    }

    return table;
  }

  // ==============================================================================
  /// transforms a hashtable of val into a hashtable of mountains
  // ==============================================================================
  template <typename Key, typename Val>
  template <typename Mount> HashTable<Key,Mount> INLINE
  HashTable<Key,Val>::map( Mount( *f )( Val& ), Size size,
                           bool resize_pol, bool key_uniqueness_pol ) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without needing to
    // resize in autmatic resizing mode
    if ( size == 0 )
      size = std::max( 2UL, __nb_elements / 2 );

    // create a new table
    HashTable<Key,Mount> table( size, resize_pol, key_uniqueness_pol );

    // fill the new hash table
    for ( HashTableIterator<Key,Val> iter = rbegin(); iter != rend(); --iter ) {
      table.insert( iter.key(), f( *iter ) );
    }

    return table;
  }

  // ==============================================================================
  /// transforms a hashtable of val into a hashtable of mountains
  // ==============================================================================
  template <typename Key, typename Val>
  template <typename Mount> HashTable<Key,Mount> INLINE
  HashTable<Key,Val>::map( Mount( *f )( const Val& ), Size size,
                           bool resize_pol, bool key_uniqueness_pol ) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without needing to
    // resize in autmatic resizing mode
    if ( size == 0 )
      size = std::max( 2UL, __nb_elements / 2 );

    // create a new table
    HashTable<Key,Mount> table( size, resize_pol, key_uniqueness_pol );

    // fill the new hash table
    for ( HashTableIterator<Key,Val> iter = rbegin(); iter != rend(); --iter ) {
      table.insert( iter.key(), f( *iter ) );
    }

    return table;
  }

  // ==============================================================================
  /// creates a hashtable of Val with a given value from a hashtable of Mount
  // ==============================================================================
  template <typename Key, typename Val>
  template <typename Mount> HashTable<Key,Mount> INLINE
  HashTable<Key,Val>::map( const Mount& val, Size size,
                           bool resize_pol, bool key_uniqueness_pol ) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without needing to
    // resize in autmatic resizing mode
    if ( size == 0 )
      size = std::max( 2UL, __nb_elements / 2 );

    // create a new table
    HashTable<Key,Mount> table( size, resize_pol, key_uniqueness_pol );

    // fill the new hash table
    for ( HashTableIterator<Key,Val> iter = rbegin(); iter != rend(); --iter ) {
      table.insert( iter.key(), val );
    }

    return table;
  }

  // ==============================================================================
  /// checks whether two hashtables contain the same elements
  // ==============================================================================
  template <typename Key, typename Val>
  bool HashTable<Key,Val>::operator==( const HashTable<Key, Val> &from ) const  {
    // checks whether the two hashtable contain the same number of elements
    if ( from.__nb_elements != __nb_elements ) return false;

    // parse this and check that each element also belongs to from
    for ( typename HashTable<Key, Val>::iterator iter = begin();
          iter != end(); ++iter ) {
      try { if ( *iter != from[iter.key()] ) return false; }
      catch ( NotFound& ) { return false; }
    }

    return true;
  }

  // ==============================================================================
  /// checks whether two hashtables contain different sets of elements
  // ==============================================================================
  template <typename Key, typename Val>
  bool HashTable<Key,Val>::operator!=( const HashTable<Key, Val> &from ) const  {
    // checks whether the two hashtable contain the same number of elements
    if ( from.__nb_elements != __nb_elements ) return true;

    // parse this and check that each element also belongs to from
    for ( typename HashTable<Key, Val>::iterator iter = begin();
          iter != end(); ++iter ) {

      try { if ( *iter != from[iter.key()] ) return true; }
      catch ( NotFound& ) { return true; }
    }

    return false;
  }



  

  // ==============================================================================
  // a << operator for HashTableList
  // ==============================================================================
  template <typename Key, typename Val>
  std::ostream& operator<< ( std::ostream& stream,
                             const HashTableList<Key,Val>& list ) {
    bool deja=false;
    stream << "[";
  
    for ( HashTableBucket<Key, Val>* ptr = list.__deb_list; ptr;
          ptr = ptr->list.next, deja = true ) {
      if ( deja ) stream << " , ";
      stream << ptr->key << "=>" << ptr->val;
    }
  
    stream << "]";
  
    return stream;
  }

  // ==============================================================================
  // a << operator for HashTableList
  // ==============================================================================
  template <typename Key, typename Val>
  std::ostream& operator<< ( std::ostream& stream,
                             const HashTableList<Key*,Val>& list ) {
    bool deja=false;
    stream << "[";
  
    for ( HashTableBucket<Key, Val>* ptr = list.__deb_list; ptr;
          ptr = ptr->list.next, deja = true ) {
      if ( deja ) stream << " , ";
    
      stream << ptr->key << "=>" << ptr->val;
    }
  
    stream << "]";
  
    return stream;
  }

  // ==============================================================================
  /// a \c << operator for HashTable
  // ==============================================================================
  template <typename Key, typename Val>
  std::ostream& operator<< ( std::ostream& stream,
                             const HashTable<Key,Val>& table ) {
    bool deja = false;
    stream << "{";
  
    for ( Size i = 0; i < table.__size; ++i )
      for ( HashTableBucket<Key,Val>* ptr = table.__nodes[i].__deb_list; ptr;
            ptr = ptr->next ) {
        if ( deja ) stream << " , ";
      
        stream << ptr->key << "=>" << ptr->val;
      
        deja = true;
      }
  
    stream << "}";
  
    return stream;
  }

  // ==============================================================================
  /// a \c << operator for HashTable
  // ==============================================================================
  template <typename Key, typename Val>
  std::ostream& operator<<( std::ostream& stream,
                            const HashTable<Key*,Val>& table ) {
    bool deja = false;
    stream << "{";
  
    for ( Size i = 0; i < table.__size; ++i )
      for ( HashTableBucket<Key,Val>* ptr = table.__nodes[i].__deb_list; ptr;
            ptr=ptr->next ) {
        if ( deja ) stream << " , ";
      
        stream << ptr->key << "=>" << ptr->val;
      
        deja = true;
      }
  
    stream << "}";
  
    return stream;
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

