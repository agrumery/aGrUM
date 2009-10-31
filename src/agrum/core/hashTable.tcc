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
 * holds currently) that hashTable's iterators end are never modified by
 * insertions or deletions of elements in hash tables. If this property were to be
 * changed, set.tcc should be updated accordingly
 * agrum/core/bijection.tcc: same as set.tcc
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
  template <typename Key, typename Val>
  void HashTableList<Key,Val>::__copy( const HashTableList<Key, Val> &from ) {
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
    } catch ( ... ) {
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
  HashTableList<Key,Val>::__getBucket( const Key& key ) const  {
    for ( HashTableBucket<Key, Val> *ptr = __deb_list; ptr; ptr = ptr->next )
      if ( ptr->key == key ) return ptr;

    return 0;
  }

  // ==============================================================================
  // suppresses an element from a chained list
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTableList<Key,Val>::__erase( const HashTableBucket<Key, Val>* ptr ) {
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
    __copy( from );
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
      __copy( from );
    }

    return *this;
  }

  // ==============================================================================
  // destructor
  // ==============================================================================
  template <typename Key, typename Val>
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
  HashTableList<Key,Val>::__insertAndGetBucket( const Key& key, const Val& val ) {
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
    return __insertAndGetBucket( key, val )->val;
  }

  // ==============================================================================
  // insertion of a new element in the chained list
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Key&
  HashTableList<Key,Val>::insertAndGetKey( const Key& key, const Val& val ) {
    return __insertAndGetBucket( key, val )->key;
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
  HashTableConstIterator<Key,Val>::HashTableConstIterator()  :
      __table( 0 ), __index( 0 ), __bucket( 0 ),
      __next_bucket( 0 ), __next( 0 ), __prev( 0 ) {}

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableConstIterator<Key,Val>::HashTableConstIterator
  ( const HashTableConstIterator<Key, Val>& from )  :
      __table( from.__table ), __index( from.__index ), __bucket( from.__bucket ),
      __next_bucket( from.__next_bucket ), __prev( 0 ) {
    // rechain properly the list of iterators of the hash table
    if ( __table ) {
      __next = __table->__iterator_list;

      if ( __table->__iterator_list )
        __table->__iterator_list->__prev = this;

      __table->__iterator_list = this;
    }
    else
      __next = 0;
  }

  // ==============================================================================
  /// constructor for an iterator pointing to the nth element of a hashtable
  // ==============================================================================
  template <typename Key, typename Val>
  HashTableConstIterator<Key,Val>::HashTableConstIterator
  ( const HashTable<Key,Val>& tab, Size ind_elt ) :
      __table( &tab ), __bucket( 0 ), __next_bucket( 0 ), __prev( 0 ) {
    register Size i;

    // check if we are looking for a begin() and wr know for surte its index
    if ( ( ind_elt == 0 ) && __table->__know_begin_index ) {
      __index  = __table->__begin_index;
      __bucket = __table->__nodes[__index].__end_list;
    }
    else {
      // check if it is faster to find the ind_eltth element from the start or
      // from the end of the hashtable

      if ( ind_elt < ( __table->__nb_elements >> 1 ) ) {
        // find the element we shall point to from the start of the hashtable
        for ( i = __table->__size - 1;; --i ) { // no test on i since
                                                // ind_elt < _table->__nb_elements
          if ( __table->__nodes[i].__nb_elements ) {
            if ( ind_elt >= __table->__nodes[i].__nb_elements )
              ind_elt -= __table->__nodes[i].__nb_elements;
            else {
              for ( __bucket = __table->__nodes[i].__end_list; ind_elt;
                    --ind_elt, __bucket = __bucket->prev ) {}
              
              __index = i;
              break;
            }
          }
        }
      }
      else {
        // ind_elt = the index of the element we should point to
        // check if the index passed as parameter is valid
        if ( ind_elt >= __table->__nb_elements )
          GUM_ERROR( UndefinedIteratorValue,
                     "Not enough elements in the hashtable" );
        
        // find the element we shall point to from the end of the hashtable
        for ( i = 0, ind_elt = __table->__nb_elements - ind_elt - 1;; ++i )
          if ( __table->__nodes[i].__nb_elements ) {
            if ( ind_elt >= __table->__nodes[i].__nb_elements )
              ind_elt -= __table->__nodes[i].__nb_elements;
            else {
              for ( __bucket = __table->__nodes[i].__deb_list; ind_elt;
                    --ind_elt, __bucket = __bucket->next ) {}
              
              __index = i;
              break;
            }
          }
      }

      __table->__know_begin_index = true;
      __table->__begin_index = __index;
    }

    // rechain properly the iterator's list of the hashtable
    if ( __table->__iterator_list ) {
      __next = __table->__iterator_list;
      __table->__iterator_list->__prev = this;
    }
    else
      __next = 0;

    __table->__iterator_list = this;
  }

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  template <typename Key, typename Val>
  HashTableConstIterator<Key,Val>& HashTableConstIterator<Key,Val>::operator=
  ( const HashTableConstIterator<Key,Val>& from )  {
    // here, no need to avoid self assignment: this would slow down normal
    // assignments and, in any case, this would not result in an iterator in
    // an incoherent state
    // check if the current hashtable is different from that of "from". In such
    // a case, we shall remove the iterator from its current hashtable iterator's
    // list and add it to the new hashtable iterator's list
    if ( __table != from.__table ) {
      // remove the iterator from its hashtable iterator's list'
      if ( __prev )
        __prev->__next = __next;
      else if ( __table )
        __table->__iterator_list = __next;

      if ( __next )
        __next->__prev = __prev;

      // add to the new table
      if ( from.__table ) {
        __next = from.__table->__iterator_list;

        if ( from.__table->__iterator_list )
          from.__table->__iterator_list->__prev = this;

        from.__table->__iterator_list = this;
      }
      else
        __next = 0;

      __prev = 0;
      __table = from.__table;
    }

    __index = from.__index;
    __bucket = from.__bucket;
    __next_bucket = from.__next_bucket;

    return *this;
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template <typename Key, typename Val>
  HashTableConstIterator<Key,Val>::~HashTableConstIterator()  {
    // remove the iterator from the table's iterator list
    if ( __prev )
      __prev->__next = __next;
    else if ( __table )
      __table->__iterator_list = __next;

    if ( __next )
      __next->__prev = __prev;
  }

  // ==============================================================================
  /// returns the key corresponding to the current iterator's bucket
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Key& HashTableConstIterator<Key,Val>::key() const {
    if ( __bucket )
      return __bucket->key;
    else
      GUM_ERROR( UndefinedIteratorValue, "Accessing a NULL object" );
  }

  // ==============================================================================
  /** @brief makes the iterator point toward nothing (in particular, it is not
   * related anymore to its current hash table) */
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTableConstIterator<Key,Val>::clear()  {
    // remove the iterator from the table's iterator list
    if ( __prev )
      __prev->__next = __next;
    else if ( __table )
      __table->__iterator_list = __next;

    if ( __next )
      __next->__prev = __prev;

    // set its table as well as the element it points to to 0
    __table = 0;
    __index = 0;
    __bucket = 0;
    __next_bucket = 0;
    __next = 0;
    __prev = 0;
  }


  // ==============================================================================
  /// makes the iterator point to the next element in the hash table
  // ==============================================================================
  // WARNING: never inline this function: this result in g++4.3.3 producing a code
  // that segfaults.
  template <typename Key, typename Val>
  HashTableConstIterator<Key,Val>& HashTableConstIterator<Key,Val>::operator++()  {
    // if __bucket != 0 then use it, else use next_bucket
    if ( ! __bucket ) {
      // note that this case only happens when the iterator pointed to an element
      // that has just been erased. Fortunately, in this case, the Hashtable's
      // erase functions update appropriately the __next_bucket and __index fields.
      __bucket = __next_bucket;
      __next_bucket = 0;
    }
    else {
      // ok, here we can use __bucket as a starting point

      // if we are not pointing on the first element of the chained list, just
      // point on the preceding bucket in this list
      if ( __bucket->prev ) {
        __bucket = __bucket->prev;
        // here, no need to update __next_bucket, which is compulsorily
        // equal to 0, nor __index which has not changed.
      }
      else {
        // ok, here we are on the beginning of a chained list,
        // so 2 cases can obtain:
        // 1/ index = 0 : then we have reached the end of the hashtable
        // 2/ index != 0 => we must search for a new slot containing elements

        // case 1:
        if ( ! __index ) {
          __bucket = 0;
          // we are thus at the end() of the hashTable
        }

        // case 2:
        else {
          // arrived here, we need to parse the hash table until we find a new
          // bucket because we are pointing on a chained list with no more element
          // to the left of the current element
          for ( Size i = __index - 1UL; i; --i ) {
            if ( __table->__nodes[i].__nb_elements ) {
              __index = i;
              __bucket = __table->__nodes[i].__end_list;
              return *this;
            }
          }

          if ( __table->__nodes[0].__nb_elements )
            __bucket = __table->__nodes[0].__end_list;
          else
            __bucket = 0;

          __index = 0;
        }
      }
    }

    return *this;
  }

  // ==============================================================================
  /// checks whether two iterators are pointing toward different elements
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTableConstIterator<Key,Val>::operator!=
  ( const HashTableConstIterator<Key, Val> &from ) const  {
    return (( __bucket != from.__bucket ) || ( __index != from.__index ) );
  }

  // ==============================================================================
  /// checks whether two iterators are pointing toward the same element
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTableConstIterator<Key,Val>::operator==
  ( const HashTableConstIterator<Key, Val> &from ) const  {
    return (( __bucket == from.__bucket ) && ( __index == from.__index ) );
  }

  // ==============================================================================
  /// dereferences the value pointed to by the iterator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Val* HashTableConstIterator<Key,Val>::operator->() const {
    if ( __bucket )
      return &( __bucket->val );
    else
      GUM_ERROR( UndefinedIteratorValue, "Accessing a NULL object" );
  }

  // ==============================================================================
  /// returns the value pointed to by the iterator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Val& HashTableConstIterator<Key,Val>::operator*() const  {
    if ( __bucket )
      return __bucket->val;
    else
      GUM_ERROR( UndefinedIteratorValue, "Accessing a NULL object" );
  }


  // ==============================================================================
  /// returns the current iterator's bucket
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableBucket<Key,Val>* HashTableConstIterator<Key,Val>::__getBucket() const {
    return __bucket;
  }

  // ==============================================================================
  /// returns the index in the hashtable's node vector pointed to by the iterator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Size HashTableConstIterator<Key,Val>::__getIndex() const  {
    return __index;
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
  HashTableIterator<Key,Val>::HashTableIterator() :
    HashTableConstIterator<Key,Val>() {
    GUM_CONSTRUCTOR( HashTableIterator );
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val>::HashTableIterator
  ( const HashTableIterator<Key, Val>& from ) :
    HashTableConstIterator<Key,Val> ( from ) {
    GUM_CONS_CPY( HashTableIterator );
  }

  // ==============================================================================
  /// constructor for an iterator pointing to the nth element of a hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val>::HashTableIterator
  ( const HashTable<Key,Val>& tab, Size ind_elt ) :
    HashTableConstIterator<Key,Val> ( tab, ind_elt) {
    GUM_CONSTRUCTOR( HashTableIterator );
  }

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val>& HashTableIterator<Key,Val>::operator=
  ( const HashTableIterator<Key,Val>& from )  {
    GUM_OP_CPY( HashTableIterator );
    HashTableConstIterator<Key,Val>::operator= ( from );
    return *this;
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template <typename Key, typename Val>
  HashTableIterator<Key,Val>::~HashTableIterator()  {
    GUM_DESTRUCTOR( HashTableIterator );
  }

  // ==============================================================================
  /// makes the iterator point to the next element in the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val>& HashTableIterator<Key,Val>::operator++()  {
    HashTableConstIterator<Key,Val>::operator++();
    return *this;
  }

  // ==============================================================================
  /// checks whether two iterators are pointing toward different elements
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTableIterator<Key,Val>::operator!=
  ( const HashTableIterator<Key, Val> &from ) const  {
    return HashTableConstIterator<Key,Val>::operator!= ( from );
  }

  // ==============================================================================
  /// checks whether two iterators are pointing toward the same element
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTableIterator<Key,Val>::operator==
  ( const HashTableIterator<Key, Val> &from ) const {
    return HashTableConstIterator<Key,Val>::operator== ( from );
  }

  // ==============================================================================
  /// returns the value pointed to by the iterator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val& HashTableIterator<Key,Val>::operator*() {
    return const_cast<Val&> (HashTableConstIterator<Key,Val>::operator*());
  }

  // ==============================================================================
  /// dereferences the value pointed to by the iterator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val* HashTableIterator<Key,Val>::operator->() {
    return const_cast<Val*> (HashTableConstIterator<Key,Val>::operator->());
  }

  // ==============================================================================
  /// returns the value pointed to by the iterator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Val& HashTableIterator<Key,Val>::operator*() const {
    return HashTableConstIterator<Key,Val>::operator*();
  }

  // ==============================================================================
  /// dereferences the value pointed to by the iterator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Val* HashTableIterator<Key,Val>::operator->() const {
    return HashTableConstIterator<Key,Val>::operator->();
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
  void HashTable<Key,Val>::__copy( const HashTable<Key,Val>& table ) {
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

        // propagate the exception
        throw;
      }
    }

    __nb_elements = table.__nb_elements;
  }


  // ==============================================================================
  // returns the end iterator for other classes' statics
  // ==============================================================================
  template <typename Key, typename Val>
  const HashTableIterator<Key,Val>& HashTable<Key,Val>::end4Statics() {
    return *( reinterpret_cast<const HashTableIterator<Key,Val>*>
              ( HashTableIteratorStaticEnd::end4Statics() ) );
  }


  // ==============================================================================
  // returns the end iterator for other classes' statics
  // ==============================================================================
  template <typename Key, typename Val>
  const HashTableConstIterator<Key,Val>& HashTable<Key,Val>::constEnd4Statics() {
    return *( reinterpret_cast<const HashTableConstIterator<Key,Val>*>
              ( HashTableIteratorStaticEnd::constEnd4Statics() ) );
  }


  // ==============================================================================
  // Default constructor
  // ==============================================================================
  template <typename Key, typename Val>
  void HashTable<Key,Val>::__create( Size size )  {
    // setup the __nodes vector (contains only empty lists)
    __nodes = new HashTableList<Key, Val> [size];

    // set up properly the hash function
    __hash_func.resize( size );

    // make sure the end() iterator is constructed properly
    end4Statics();
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
      __know_begin_index ( false ),
      __begin_index ( 0 ),
      __iterator_list( 0 ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( HashTable );

    // finalize the creation
    __create( __size );
  }

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
      __know_begin_index ( table.__know_begin_index ),
      __begin_index ( table.__begin_index ),
      __iterator_list( 0 ) {
    // for debugging purposes
    GUM_CONS_CPY( HashTable );

    // set up properly the hash function
    __hash_func.resize( __size );
    __copy( table );

    // make sure the end() iterator is created properly
    end4Statics();
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
      // the iterators point to end
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
        __size = from.__size;

        /* update the hash function : this is important as the computation of
           the hash values heavily depends on the size of the hash table */
        __hash_func.resize( __size );

        delete[] old_nodes;
      }

      __resize_policy = from.__resize_policy;
      __key_uniqueness_policy = from.__key_uniqueness_policy;
      __know_begin_index = from.__know_begin_index;
      __begin_index = from.__begin_index;
      

      // perform the copy
      __copy( from );
    }

    return *this;
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template <typename Key, typename Val>
  HashTable<Key,Val>::~HashTable() {
    // for debugging purposes
    GUM_DESTRUCTOR( HashTable );

    /* update all the registered iterators: they should now point to NULL
       and their hashtable should be set to NULL */
    while ( __iterator_list )
      __iterator_list->clear();

    // we delete properly all the buckets
    delete[] __nodes;
  }

  // ==============================================================================
  // the iterator to the end of the hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const HashTableConstIterator<Key,Val>& HashTable<Key,Val>::end() const  {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *( reinterpret_cast<const HashTableConstIterator<Key,Val>*>
              ( HashTableIteratorStaticEnd::__HashTableIterEnd ) );
  }

  // ==============================================================================
  // the iterator to the end of the hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const HashTableIterator<Key,Val>& HashTable<Key,Val>::end() {
    // note that, here, we know for sure that HashTableIterEnd has been properly
    // initialized as it is initialized by end4Statics, which is called by
    // all hashtables' constructors
    return *( reinterpret_cast<const HashTableIterator<Key,Val>*>
              ( HashTableIteratorStaticEnd::__HashTableIterEnd ) );
  }

  // ==============================================================================
  /// the iterator at the beginning of the hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableConstIterator<Key,Val> HashTable<Key,Val>::begin() const  {
    // if the table is empty, make the begin and end point to the same element
    if ( __nb_elements == 0 ) return const_iterator( end() );
    else return const_iterator( *this, 0 );
  }

  // ==============================================================================
  /// the iterator at the beginning of the hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val> HashTable<Key,Val>::begin()  {
    // if the table is empty, make the begin and end point to the same element
    if ( __nb_elements == 0 ) return iterator( end() );
    else return iterator( *this, 0 );
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
      if ( ! __resize_policy ||
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
        __know_begin_index = false;


        // substitute the current __nodes array by the new one
        delete[] __nodes;

        __nodes = new_nodes;

        // update the iterators
        for ( const_iterator* iter = __iterator_list; iter; iter = iter->__next ) {
          if ( iter->__bucket )
            iter->__index = __hash_func( iter->__bucket->key );
          else {
            iter->__next_bucket = 0;
            iter->__index = 0;
          }
        }
      }
    }
  }

  // ==============================================================================
  /// add a new element (actually a copy of this element) in the hash table
  // ==============================================================================
  template <typename Key, typename Val>
  HashTableBucket<Key,Val>*
  HashTable<Key,Val>::__insertAndGetBucket( const Key& key, const Val& val ) {
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
      __nodes[hash_key].__insertAndGetBucket( key, val );

    ++__nb_elements;
    __know_begin_index = false;

    return new_bucket;
  }

  // ==============================================================================
  /// add a new element (actually a copy of this element) in the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val& HashTable<Key,Val>::insert( const Key& thekey, const Val& theval ) {
    return __insertAndGetBucket( thekey,theval )->val;
  }

  // ==============================================================================
  /// add a new element (actually a copy of this element) in the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Key&
  HashTable<Key,Val>::insertAndGetKey( const Key& thekey, const Val& theval ) {
    return __insertAndGetBucket( thekey,theval )->key;
  }

  // ==============================================================================
  /// returns a reference on the value the key of which is passed in argument
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Val&
  HashTable<Key,Val>::getWithDefault( const Key& key,
                                      const Val& default_value ) const {
    HashTableBucket<Key, Val> *bucket =
      __nodes[ __hash_func( key )].__getBucket( key );

    if ( ! bucket ) return insert( key, default_value );
    else return bucket->val;
  }

  // ==============================================================================
  /// returns a reference on the value the key of which is passed in argument
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val& HashTable<Key,Val>::getWithDefault( const Key& key,
                                           const Val& default_value ) {
    HashTableBucket<Key, Val> *bucket =
      __nodes[ __hash_func( key )].__getBucket( key );

    if ( ! bucket ) return insert( key, default_value );
    else return bucket->val;
  }

  // ==============================================================================
  /// add or update a couple (key,value)
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::set( const Key& key,const Val& value ) {
    HashTableBucket<Key, Val> *bucket =
      __nodes[ __hash_func( key )].__getBucket( key );

    if ( ! bucket ) insert( key,value );
    else bucket->val = value;
  }

  // ==============================================================================
  /// removes a given element from the hash table
  // ==============================================================================
  template <typename Key, typename Val>
  void HashTable<Key,Val>::__erase( HashTableBucket<Key, Val>* bucket,
                                    Size index ) {
    if ( bucket == 0 ) return;

    // update the registered iterators pointing to this bucket
    for ( const_iterator* iter = __iterator_list; iter; iter = iter->__next ) {
      if ( iter->__bucket == bucket ) {
        iter->operator++();
        iter->__next_bucket = iter->__bucket;
        iter->__bucket = 0;
      }
      else if ( iter->__next_bucket == bucket ) {
        iter->__bucket = bucket;
        iter->operator++();
        iter->__next_bucket = iter->__bucket;
        iter->__bucket = 0;
      }
    }

    // remove the element from the __nodes vector
    __nodes[index].__erase( bucket );

    --__nb_elements;
    __know_begin_index = false;
  }

  // ==============================================================================
  /// removes a given element from the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::erase( const Key& key ) {
    // get the hashed key
    Size hash = __hash_func( key );

    // get the bucket containing the element to erase
    HashTableBucket<Key, Val>* bucket = __nodes[hash].__getBucket( key );

    __erase( bucket, hash );
  }

  // ==============================================================================
  /// removes a given element from the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::erase( const iterator& iter ) {
    __erase( iter.__getBucket(), iter.__getIndex() );
  }

  // ==============================================================================
  /// removes a given element from the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::erase( const const_iterator& iter ) {
    __erase( iter.__getBucket(), iter.__getIndex() );
  }

  // ==============================================================================
  /// removes a given element from the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::eraseByVal( const Val& val ) {
    for ( HashTableIterator<Key,Val> iter = begin(); iter != end(); ++iter )
      if ( iter.__bucket->val == val ) {
        __erase( iter.__getBucket(), iter.__getIndex() );
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
    for ( HashTableConstIterator<Key,Val> iter = begin(); iter != end(); ++iter )
      if ( iter.__bucket->val == val )
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
      __nodes[__hash_func( key )].__getBucket( key );

    if ( ! bucket )
      GUM_ERROR( NotFound, "key does not belong to the hashtable" );

    return bucket->key;
  }

  // ==============================================================================
  /// removes all the elements having a certain value from the hash table
  // ==============================================================================
  template <typename Key, typename Val>
  void HashTable<Key,Val>::eraseAllVal( const Val& val ) {
    for ( HashTableIterator<Key,Val> iterAll = begin();
          iterAll != end(); ++iterAll ) {
      if ( iterAll.__bucket->val == val ) {
        __erase( iterAll.__bucket, iterAll.__index );
      }
    }
  }

  // ==============================================================================
  /// removes all the elements in the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::clear() {
    /* update all the registered iterators: they should now point to NULL
       and they are positioned to the end of the hashtable. */
    while ( __iterator_list )
      __iterator_list->clear();

    // remove the buckets
    for ( Size i = 0; i < __size; ++i )
      __nodes[i].clear();

    __nb_elements = 0;
    __know_begin_index = false;
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
    for ( HashTableConstIterator<Key,Val> iter = begin(); iter != end(); ++iter ) {
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
    for ( HashTableConstIterator<Key,Val> iter = begin(); iter != end(); ++iter ) {
      table.insert( iter.key(), f( const_cast<Val&>(*iter) ) );
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
    for ( HashTableConstIterator<Key,Val> iter = begin(); iter != end(); ++iter ) {
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
    for ( HashTableConstIterator<Key,Val> iter = begin(); iter != end(); ++iter ) {
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
    for ( typename HashTable<Key, Val>::const_iterator iter = begin();
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
    for ( typename HashTable<Key, Val>::const_iterator iter = begin();
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

