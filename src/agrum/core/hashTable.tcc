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


namespace gum {


  
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===      IMPLEMENTATION OF THE CHAINED LISTS USED IN THE HASH TABLES     === */
  /* ============================================================================ */
  /* ============================================================================ */

  // ==============================================================================
  /// a function used to perform copies of HashTableLists
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTableList<Key,Val>::_copy( const HashTableList<Key, Val> &from ) {
    HashTableBucket<Key, Val> *ptr, *old_ptr, *new_elt = 0;
    // set the defaults
    deb_list = 0;
    
    // copy from's list
    try {
      for ( ptr = from.deb_list, old_ptr = 0; ptr; ptr = ptr->next ) {
        // copy the current from's bucket (may throw an exception either because new
        // cannot allocate the bucket or because the copy constructor of Val throws
        // an exception)
        new_elt = new HashTableBucket<Key,Val>( *ptr );
        // rechain properly the new list
        new_elt->prev = old_ptr;

        if ( old_ptr ) old_ptr->next = new_elt;
        else deb_list = new_elt;

        old_ptr = new_elt;
      }

      if ( old_ptr ) old_ptr->next = 0;

      // update the number of elements stored into the list and the end of the list
      nb_elements = from.nb_elements;

      end_list = new_elt;
    }
    catch ( ... ) {
      // problem: we could not allocate an element in the list => we delete
      // the elements created so far and we throw an exception
      for ( ; deb_list != old_ptr; deb_list = ptr ) {
        ptr = deb_list->next;
        delete deb_list;
      }

      if ( old_ptr ) delete old_ptr;

      nb_elements = 0;

      end_list = 0;

      throw;
    }
  }

  // ==============================================================================
  /// returns the bucket corresponding to a given key
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableBucket<Key,Val>* HashTableList<Key,Val>::getBucket( const Key& key )
    const  {
    for ( HashTableBucket<Key, Val> *ptr = deb_list; ptr; ptr = ptr->next )
      if ( ptr->key == key ) return ptr;

    return 0;
  }

  // ==============================================================================
  // suppresses an element from a chained list
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTableList<Key,Val>::erase( const HashTableBucket<Key, Val>* ptr ) {
    // check that the pointer is not NULL
    if ( ptr == 0 )
      GUM_ERROR( NullElement, "trying to erase a NULL bucket" );

    // relink properly the doubly chained list
    if ( ptr->prev )
      ptr->prev->next = ptr->next;
    else
      deb_list = ptr->next;

    if ( ptr->next )
      ptr->next->prev = ptr->prev;
    else
      end_list = ptr->prev;

    // remove the current element from the list
    delete ptr;

    --nb_elements;
  }

  // ==============================================================================
  // basic constructor that creates an empty list
  // ==============================================================================
  template <typename Key, typename Val>
  HashTableList<Key,Val>::HashTableList()  :
    deb_list( 0 ), end_list( 0 ), nb_elements( 0 ) {}

  // ==============================================================================
  // copy constructor
  // ==============================================================================
  template <typename Key, typename Val>
  HashTableList<Key,Val>::HashTableList( const HashTableList<Key,Val>&from ) {
    _copy( from );
  }

  // ==============================================================================
  // assignment operator
  // ==============================================================================
  template <typename Key, typename Val>
  HashTableList<Key,Val>& HashTableList<Key,Val>::operator=
  ( const HashTableList<Key, Val>& from ) {
    // avoid self assignment
    if ( this != &from ) {
      // remove the old content of the list
      for ( HashTableBucket<Key, Val> *next_ptr, *ptr = deb_list;
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
  template <typename Key, typename Val>
  HashTableList<Key,Val>::~HashTableList() {
    HashTableBucket<Key, Val> *ptr, *next_ptr;

    for ( ptr = deb_list; ptr; ptr = next_ptr ) {
      next_ptr = ptr->next;
      delete ptr;
    }
  }

  // ==============================================================================
  // function at returns the ith element in the current chained list
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Val& HashTableList<Key,Val>::at( unsigned int i ) const {
    if ( i >= nb_elements )
      GUM_ERROR( NotFound, "not enough elements in the chained list" );

    HashTableBucket<Key, Val> *ptr;

    for ( ptr = deb_list; i; --i, ptr = ptr->next ) {};

    return ptr->val;
  }

  // ==============================================================================
  // function at returns the ith element in the current chained list
  // ==============================================================================
  /*template <typename Key, typename Val> INLINE
    const Val& HashTableList<Key,Val>::at( unsigned int i ) const {
    if ( i >= nb_elements )
    GUM_ERROR( NotFound, "not enough elements in the chained list" );
    HashTableBucket<Key, Val> *ptr;
    for ( ptr = deb_list; i; --i, ptr = ptr->next );
    return ptr->val;
    }*/
  
  // ==============================================================================
  // operator [] returns the value corresponding to a given key
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Val& HashTableList<Key,Val>::operator[]( const Key& key ) const {
    for ( HashTableBucket<Key,Val> *ptr = deb_list; ptr; ptr = ptr->next )
      if ( ptr->key == key ) return ptr->val;

    GUM_ERROR( NotFound,
               "hashtable's chained list contains no element with this key" );
  }

  // ==============================================================================
  // operator [] returns the value corresponding to a given key
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val& HashTableList<Key,Val>::operator[]( const Key& key ) {
    for ( HashTableBucket<Key,Val> *ptr = deb_list; ptr; ptr = ptr->next )
      if ( ptr->key == key ) return ptr->val;

    GUM_ERROR( NotFound,
               "hashtable's chained list contains no element with this key" );
  }

  // ==============================================================================
  // check whether there exists an element with a given key in the list
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTableList<Key,Val>::exists( const Key& key ) const  {
    for ( HashTableBucket<Key, Val> *ptr = deb_list; ptr; ptr = ptr->next )
      if ( ptr->key == key ) return true;

    return false;
  }

  // ==============================================================================
  // insertion of a new element in the chained list
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val& HashTableList<Key,Val>::insert( const Key& key, const Val& val ) {
    // create a new bucket
    HashTableBucket<Key,Val> *new_elt = new HashTableBucket<Key,Val>( key,val );
    // place the bucket at the beginning of the list
    new_elt->prev = 0;
    new_elt->next = deb_list;

    if ( deb_list )
      deb_list->prev = new_elt;

    deb_list = new_elt;

    if ( end_list == 0 )
      end_list = new_elt;

    ++nb_elements;

    return new_elt->val;
  }

  // ==============================================================================
  // function for deleting all the elements of a chained list
  // ==============================================================================
  template <typename Key, typename Val>
  void HashTableList<Key,Val>::clear() {
    HashTableBucket<Key, Val> *ptr, *next_ptr;

    for ( ptr = deb_list; ptr; ptr = next_ptr ) {
      next_ptr = ptr->next;
      delete ptr;
    }

    nb_elements = 0;

    deb_list = 0;
    end_list = 0;
  }

  // ==============================================================================
  // returns a boolean indicating whether the chained list is empty
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTableList<Key,Val>::empty() const  {
    return ( nb_elements == 0 );
  }



  
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                   GENERIC HASH TABLE IMPLEMENTATION                  === */
  /* ============================================================================ */
  /* ============================================================================ */

  // ==============================================================================
  // a function used to perform copies of HashTables
  // ==============================================================================
  template <typename Key, typename Val>
  void HashTable<Key,Val>::_copy( const HashTable<Key,Val>& table ) {
    // in debug mode, check that this and table have 'nodes' arrays of the same size
    GUM_ASSERT( table.__size == __size );
    // try to fill the array of chained lists

    for ( Size i = 0; i < table.__size; ++i ) {
      try {
        nodes[i] = table.nodes[i];
      } catch ( ... ) {
        // here we could allocate the nodes[j], j=0..i-1, so we should
        // deallocate them
        for ( Size j=0; j < __size; ++j )
          nodes[j].clear();

        nb_elements = 0;

        // create the end() and rend() iterators properly
        iter_end.initialize( this, true );

        iter_rend.initialize( this, false );

        // propagate the exception
        throw;
      }
    }

    nb_elements = table.nb_elements;

    // create the end() and rend() iterators properly
    iter_end.initialize( this, true );
    iter_rend.initialize( this, false );
  }

  // ==============================================================================
  // Default constructor
  // ==============================================================================
  template <typename Key, typename Val>
  void HashTable<Key,Val>::_create( Size size )  {
    // setup the nodes vector (contains only empty lists)
    nodes = new HashTableList<Key, Val> [size];
    // set up properly the hash function
    hash_func.resize( size );
    // create the end() and rend() iterators properly
    iter_end.initialize( this, true );
    iter_rend.initialize( this, false );
  }

  // ==============================================================================
  // Default constructor
  // ==============================================================================
  template <typename Key, typename Val>
  HashTable<Key,Val>::HashTable( Size size_param,
                                 bool resize_pol, bool key_uniqueness_pol ) :
    nodes( 0 ),
    // size must be >= 2 else we lose all the bits of the hash function
    __size( 1UL << __hashTableLog2( std::max( 2UL,size_param ) ) ), nb_elements( 0 ),
    resize_policy( resize_pol ), key_uniqueness_policy( key_uniqueness_pol ),
    iterator_list( 0 ) {
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
  HashTable<Key,Val>::HashTable( std::string dummy,Size size_param,
                                 bool resize_pol, bool key_uniqueness_pol )
    :
    nodes( 0 ),
    // size must be >= 2 else we lose all the bits of the hash function
    __size( 1UL << __hashTableLog2( std::max( 2UL,size_param ) ) ), nb_elements( 0 ),
    resize_policy( resize_pol ), key_uniqueness_policy( key_uniqueness_pol ),
    iterator_list( 0 ) {
    // finalize the creation
    _create( __size );
  }

#endif


  // ==============================================================================
  // copy constructor
  // ==============================================================================
  template <typename Key, typename Val>
  HashTable<Key,Val>::HashTable( const HashTable<Key, Val>& table ) :
    nodes( new HashTableList<Key, Val> [table.__size] ), __size( table.__size ),
    nb_elements( 0 ), resize_policy( table.resize_policy ),
    key_uniqueness_policy( table.key_uniqueness_policy ), iterator_list( 0 ) {
    // for debugging purposes
    GUM_CONS_CPY( HashTable );
    // set up properly the hash function
    hash_func.resize( __size );
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
      // if sizes of from's and this' nodes vectors are not the same,
      // we need to remove the current nodes' array and to create a
      // new array with the correct size

      if ( __size != from.__size ) {
        // keep track of the old set of nodes and remove it after the new so that
        // if the new throws a bad_alloc, the hashtable is empty but in a coherent
        // state (its size corresponds to the size of its nodes vector, etc)
        HashTableList<Key, Val> *old_nodes = nodes;
        nodes = new HashTableList<Key, Val> [from.__size];
        delete[] old_nodes;
        __size = from.__size;
        /* update the hash function : this is important as the computation of
           the hash values heavily depends on the size of the hash table */
        hash_func.resize( __size );
      }

      resize_policy = from.resize_policy;

      key_uniqueness_policy = from.key_uniqueness_policy;
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
    while ( iterator_list )
      iterator_list->clear();

    // we delete properly all the buckets
    delete[] nodes;
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
    return iter_end;
  }

  // ==============================================================================
  /// the iterator at the rend (just before the beginning) of the hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const HashTableIterator<Key,Val>& HashTable<Key,Val>::rend() const  {
    return iter_rend;
  }

  // ==============================================================================
  /// the iterator at the beginning of the hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val> HashTable<Key,Val>::begin() const  {
    // if the table is empty, make the begin and end point to the same element
    if ( nb_elements == 0 ) return iterator( iter_end );
    else return iterator( *this, 0 );
  }

  // ==============================================================================
  /// the iterator at the rbegin (the last element) of the hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val> HashTable<Key,Val>::rbegin() const  {
    // if the table is empty, make the rbegin and rend point to the same element
    if ( nb_elements == 0 ) return iterator( iter_rend );
    else return iterator( *this, nb_elements - 1 );
  }

  // ==============================================================================
  /// returns a reference on the value the key of which is passed in argument
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val& HashTable<Key,Val>::operator[]( const Key& key )  {
    return nodes[hash_func( key )][key];
  }

  // ==============================================================================
  /// returns a reference on the value the key of which is passed in argument
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Val& HashTable<Key,Val>::operator[]( const Key& key ) const {
    return nodes[hash_func( key )][key];
  }

  // ==============================================================================
  /// returns a reference on the value the key of which is passed in argument
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Val& HashTable<Key,Val>::getWithDefault( const Key& key,
                                                 const Val& default_value ) const {
    if ( ! exists( key ) ) insert( key,default_value );

    return operator[]( key );
  }

  // ==============================================================================
  /// returns a reference on the value the key of which is passed in argument
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val& HashTable<Key,Val>::getWithDefault(const Key& key,const Val& default_value) {
    if ( ! exists( key ) ) insert( key,default_value );

    return operator[]( key );
  }

  // ==============================================================================
  /// add or update a couple (key,value)
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::set( const Key& key,const Val& value ) {
    if ( ! exists( key ) ) {
      insert( key,value );
    } else {
      operator[]( key )=value;
    }
  }

  // ==============================================================================
  /// remove a (key,...) if exists
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::reset( const Key& key ) {
    if ( exists( key ) ) erase( key );
  }

  // ==============================================================================
  /// returns the number of elements in the hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Size HashTable<Key,Val>::size() const  {
    return nb_elements;
  }

  // ==============================================================================
  /// returns the size of the nodes vector of the hashtable
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
    return nodes[hash_func( key )].exists( key );
  }

  // ==============================================================================
  /// enables the user to change dynamically the resizing policy.
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::setResizePolicy( const bool new_policy )  {
    resize_policy = new_policy;
  }

  // ==============================================================================
  /// returns the current resizing policy.
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTable<Key,Val>::getResizePolicy() const  {
    return resize_policy;
  }

  // ==============================================================================
  /** @brief enables the user to change dynamically the policy for checking
   * whether there can exist several elements in the table having identical keys. */
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::setKeyUniquenessPolicy( const bool new_policy )  {
    key_uniqueness_policy = new_policy;
  }

  // ==============================================================================
  /// returns the current checking policy
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTable<Key,Val>::keyUniquenessPolicy() const  {
    return key_uniqueness_policy;
  }

  // ==============================================================================
  /// changes the size of the nodes vector of the hash table
  // ==============================================================================
  template <typename Key, typename Val>
  void HashTable<Key,Val>::resize( Size new_size ) {
    // new_size must be >= 2 else all the bits of the hash function are lost
    new_size = std::max( 2UL,new_size );
    // find the real size for allocation
    int log_size = __hashTableLog2( new_size );
    new_size = 1UL << log_size;
    // check if the new size is different from the actual size
    // if not, nothing else need be done

    if ( new_size != __size ) {
      // under automatic resize policy, check if the new size leaves
      // enough space for storing all the current elements
      if ( !resize_policy ||
           ( nb_elements <= new_size * GUM_HASHTABLE_DEFAULT_MEAN_VAL_BY_SLOT ) ) {
        // create a new array of nodes to store the elements
        HashTableList<Key, Val> *new_nodes =
          new HashTableList<Key, Val> [new_size];
        // set the new hash function
        hash_func.resize( new_size );
        // put all the elements of the current nodes array into the new one
        HashTableBucket<Key, Val> *bucket;
        Size new_hashed_key;

        for ( Size i = 0; i < __size; ++i ) {
          while (( bucket = nodes[i].deb_list ) != 0 ) {
            // compute the new hashed key
            new_hashed_key = hash_func( bucket->key );
            // remove the bucket from the list of buckets of the current node vector
            nodes[i].deb_list = bucket->next;
            // put the bucket into the new nodes vector
            bucket->prev = new_nodes[new_hashed_key].end_list;
            bucket->next = 0;

            if ( new_nodes[new_hashed_key].end_list )
              new_nodes[new_hashed_key].end_list->next = bucket;

            new_nodes[new_hashed_key].end_list = bucket;

            if ( new_nodes[new_hashed_key].deb_list == 0 )
              new_nodes[new_hashed_key].deb_list = bucket;

            ++new_nodes[new_hashed_key].nb_elements;
          }
        }

        // update the size of the hash table
        __size = new_size;

        // substitute the current nodes array by the new one
        delete[] nodes;

        nodes = new_nodes;

        // update end and rend
        iter_end.initialize( this, true );

        iter_rend.initialize( this, false );

        // update the iterators
        for ( iterator* iter = iterator_list; iter; iter = iter->next ) {
          if ( iter->bucket )
            iter->index = hash_func( iter->bucket->key );
          else if ( iter->index == 0 )
            // make iter equal to rend.
            *iter = iter_rend;
          else
            *iter = iter_end;
        }
      }
    }
  }

  // ==============================================================================
  /// add a new element (actually a copy of this element) in the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val& HashTable<Key,Val>::insert( const Key& key, const Val& val ) {
    Size hash_key = hash_func( key );

    if ( key_uniqueness_policy )
      // check that there does not already exist an element with the same key
      for ( HashTableBucket<Key, Val> *ptr = nodes[hash_key].deb_list;
            ptr; ptr = ptr->next )
        if ( ptr->key == key )
          GUM_ERROR( DuplicateElement,
                     "the hashtable contains an element with the same key" );

    // check whether there is sufficient space to insert the new pair
    // if not, resize the current hashtable
    if ( resize_policy &&
         ( nb_elements >= __size * GUM_HASHTABLE_DEFAULT_MEAN_VAL_BY_SLOT ) ) {
      resize( __size << 1 );
      hash_key = hash_func( key );
    }

    // add the new pair
    Val& new_val = nodes[hash_key].insert( key, val );

    ++nb_elements;

    return new_val;
  }

  // ==============================================================================
  /// removes a given element from the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::erase( const Key& key ) {
    // get the hashed key
    Size hash = hash_func( key );
    // get the bucket containing the element to erase
    HashTableBucket<Key, Val>* bucket = nodes[hash].getBucket( key );

    if ( bucket == 0 ) return;

    // update the registered iterators pointing to this bucket
    for ( iterator* iter = iterator_list; iter; iter = iter->next ) {
      if ( iter->next_current_bucket == bucket )
        iter->next_current_bucket = bucket->prev;

      if ( iter->prev_current_bucket == bucket )
        iter->prev_current_bucket = bucket->next;

      if ( iter->bucket == bucket )
        iter->bucket = 0;
    }

    // remove the element from the nodes vector
    nodes[hash].erase( bucket );

    --nb_elements;
  }

  // ==============================================================================
  /// removes a given element from the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::eraseByVal( const Val& val ) {
    for ( HashTableIterator<Key,Val> iter = begin(); iter != end(); ++iter )
      if ( *iter == val ) {
        // get the bucket containing the element to erase
        HashTableBucket<Key, Val>* bucket = iter.getBucket();
        Size index = iter.getIndex();
        // update the registered iterators pointing to this bucket

        for ( iterator* iter = iterator_list; iter; iter = iter->next ) {
          if ( iter->next_current_bucket == bucket )
            iter->next_current_bucket = bucket->prev;

          if ( iter->prev_current_bucket == bucket )
            iter->prev_current_bucket = bucket->next;

          if ( iter->bucket == bucket )
            iter->bucket = 0;
        }

        // remove the element from the nodes vector
        nodes[index].erase( bucket );

        --nb_elements;

        return;
      }
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

  // ============================================================================
  /// returns a reference on a given key
  // ============================================================================
  template <typename Key, typename Val> INLINE
  const Key& HashTable<Key,Val>::key( const Key& key ) const {
    // get the bucket corresponding to the key
    HashTableBucket<Key, Val>* bucket = nodes[hash_func( key )].getBucket( key );

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
        HashTableBucket<Key, Val>* bucket = iter.getBucket();
        Size index = iter.getIndex();
        // update the registered iterators pointing to this bucket

        for ( iterator* iter = iterator_list; iter; iter = iter->next ) {
          if ( iter->next_current_bucket == bucket )
            iter->next_current_bucket = bucket->prev;

          if ( iter->prev_current_bucket == bucket )
            iter->prev_current_bucket = bucket->next;

          if ( iter->bucket == bucket )
            iter->bucket = 0;
        }

        // remove the element from the nodes vector
        nodes[index].erase( bucket );

        --nb_elements;
      }
  }

  // ==============================================================================
  /// removes all the elements in the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  void HashTable<Key,Val>::clear() {
    /* update all the registered iterators: they should now point to NULL
       and they are positioned to the rend of the hashtable. */
    for ( iterator* iter = iterator_list; iter; iter = iter->next )
      *iter = iter_rend;

    // remove the buckets
    for ( Size i = 0; i < __size; ++i )
      nodes[i].clear();

    nb_elements = 0;
  }

  // ==============================================================================
  /// indicates whether the hash table is empty
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTable<Key,Val>::empty() const  {
    return ( nb_elements == 0 );
  }

  // ==============================================================================
  /// transforms a hashtable of val into a hashtable of mountains
  // ==============================================================================
  template <typename Key, typename Val>
  template <typename Mount> HashTable<Key,Mount> INLINE
  HashTable<Key,Val>::map( Mount( *f )( Val ), Size __size,
                           bool resize_pol, bool key_uniqueness_pol ) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without needing to
    // resize in autmatic resizing mode
    if ( __size == 0 )
      __size = std::max( 2UL, nb_elements / 2 );

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
      size = std::max( 2UL, nb_elements / 2 );

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
      size = std::max( 2UL, nb_elements / 2 );

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
      size = std::max( 2UL, nb_elements / 2 );

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
    if ( from.nb_elements != nb_elements ) return false;

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
    if ( from.nb_elements != nb_elements ) return true;

    // parse this and check that each element also belongs to from
    for ( typename HashTable<Key, Val>::iterator iter = begin();
          iter != end(); ++iter ) {

      try { if ( *iter != from[iter.key()] ) return true; }
      catch ( NotFound& ) { return true; }
    }

    return false;
  }



  
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===              GENERIC HASH TABLE ITERATORS IMPLEMENTATION             === */
  /* ============================================================================ */
  /* ============================================================================ */

  // ==============================================================================
  /// basic constructor: creates an iterator pointing to nothing
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val>::HashTableIterator()  :
    table( 0 ), index( 0 ), bucket( 0 ),
    next_current_bucket( 0 ), prev_current_bucket( 0 ), next( 0 ), prev( 0 ) {}

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val>::HashTableIterator
  ( const HashTableIterator<Key, Val>& from )  :
    table( from.table ), index( from.index ), bucket( from.bucket ),
    next_current_bucket( from.next_current_bucket ),
    prev_current_bucket( from.prev_current_bucket ), prev( 0 ) {
    // rechain properly the list of iterators of the hash table
    if ( table ) {
      next = table->iterator_list;

      if ( table->iterator_list )
        table->iterator_list->prev = this;

      table->iterator_list = this;
    } else
      next = 0;
  }

  // ==============================================================================
  /// constructor for an iterator pointing to the nth element of a hashtable
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val>::HashTableIterator
  ( const HashTable<Key,Val>& tab, Size ind_elt ) :
    table( &tab ),bucket( 0 ),next_current_bucket( 0 ),
    prev_current_bucket( 0 ),prev( 0 ) {
    register Size i;
    // check if it is faster to find the ind_eltth element from the start or
    // from the end of the hashtable

    if ( ind_elt < ( table->nb_elements >> 1 ) ) {
      // find the element we shall point to from the start of the hashtable
      for ( i = 0;; ++i )
        if ( table->nodes[i].nb_elements ) {
          if ( ind_elt >= table->nodes[i].nb_elements )
            ind_elt -= table->nodes[i].nb_elements;
          else {
            for ( bucket = table->nodes[i].deb_list; ind_elt;
                  --ind_elt, bucket = bucket->next ) {};

            index = i;

            prev_current_bucket = bucket;

            next_current_bucket = bucket;

            break;
          }
        }
    }
    else {
      // ind_elt = the index of the element we should point to
      // check if the index passed as parameter is valid
      if ( ind_elt >= table->nb_elements )
        GUM_ERROR( UndefinedIteratorValue,"Not enough elements in the hashtable" );

      // find the element we shall point to from the end of the hashtable
      for ( i = table->__size - 1, ind_elt = table->nb_elements - ind_elt - 1;; --i )
        if ( table->nodes[i].nb_elements ) {
          if ( ind_elt >= table->nodes[i].nb_elements )
            ind_elt -= table->nodes[i].nb_elements;
          else {
            for ( bucket = table->nodes[i].end_list; ind_elt;
                  --ind_elt, bucket = bucket->prev ) {};
            index = i;
            prev_current_bucket = bucket;
            next_current_bucket = bucket;

            break;
          }
        }
    }

    // rechain properly the iterator's list of the hashtable
    if ( table->iterator_list ) {
      next = table->iterator_list;
      table->iterator_list->prev = this;
    } else
      next = 0;

    table->iterator_list = this;
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
    // check if the current hashtable is different from that of from. In such
    // a case, we shall remove the iterator from its current hashtable iterator's
    // list and add it to the new hashtable iterator's list
    if ( table != from.table ) {
      // remove the iterator from its hashtable iterator's list'
      if ( prev )
        prev->next = next;
      else if ( table )
        table->iterator_list = next;

      if ( next )
        next->prev = prev;

      // add to the new one
      prev = 0;

      if ( from.table ) {
        next = from.table->iterator_list;

        if ( from.table->iterator_list )
          from.table->iterator_list->prev = this;

        from.table->iterator_list = this;
      } else
        next = 0;
    }

    table = from.table;

    index = from.index;
    bucket = from.bucket;
    prev_current_bucket = from.prev_current_bucket;
    next_current_bucket = from.next_current_bucket;
    return *this;
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template <typename Key, typename Val>
  HashTableIterator<Key,Val>::~HashTableIterator()  {
    // remove the iterator from the table's iterator list
    if ( prev )
      prev->next = next;
    else if ( table )
      table->iterator_list = next;

    if ( next )
      next->prev = prev;
  }

  // ==============================================================================
  /// returns the key corresponding to the current iterator's bucket
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Key& HashTableIterator<Key,Val>::key() const {
    if ( bucket )
      return bucket->key;
    else
      GUM_ERROR( UndefinedIteratorValue, "Accessing a NULL object" );
  }

  // ==============================================================================
  /** @brief makes the iterator point toward nothing (in particular, it is not
   * related anymore to its current hash table) */
  // ==============================================================================
  template <typename Key, typename Val>
  void HashTableIterator<Key,Val>::clear()  {
    // remove the iterator from the table's iterator list
    if ( prev )
      prev->next = next;
    else if ( table )
      table->iterator_list = next;

    if ( next )
      next->prev = prev;

    // set its table as well as the element it points to to 0
    table = 0;
    index = 0;
    bucket = 0;
    next_current_bucket = 0;
    prev_current_bucket = 0;
    next = 0;
    prev = 0;
  }

  // ==============================================================================
  /// makes the iterator point to the next element in the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val>& HashTableIterator<Key,Val>::operator++()  {
    // if we are not pointing on the last element of the chained list, just
    // point on the next bucket in this list
    if ( next_current_bucket && next_current_bucket->next ) {
      bucket = next_current_bucket->next;
      prev_current_bucket = bucket;
      next_current_bucket = bucket;
      return *this;
    }

    // if we are pointing on 0, i.e., we are at the beginning of the chained
    // list, and the list contains some elements, point on the first one
    if ( !next_current_bucket && table->nodes[index].nb_elements ) {
      bucket = table->nodes[index].deb_list;
      prev_current_bucket = bucket;
      next_current_bucket = bucket;
      return *this;
    }

    // arrived here, we need to parse the hash table until we find a new bucket
    // because we are pointing on a chained list with no more element to the right
    // of the current element
    for ( Size i = index + 1UL; i < table->__size; ++i )
      if ( table->nodes[i].nb_elements ) {
        index = i;
        bucket = table->nodes[i].deb_list;
        prev_current_bucket = bucket;
        next_current_bucket = bucket;
        return *this;
      }

    // we are at the end of the hash table, do nothing
    index = table->__size;

    bucket = 0;

    prev_current_bucket = 0;

    next_current_bucket = 0;

    return *this;
  }

  // ==============================================================================
  /// makes the iterator point to the preceding element in the hash table
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableIterator<Key,Val>& HashTableIterator<Key,Val>::operator--()  {
    // if we are not pointing on the first element of the chained list, just
    // point on the preceding bucket in this list
    if ( prev_current_bucket && prev_current_bucket->prev ) {
      bucket = prev_current_bucket->prev;
      prev_current_bucket = bucket;
      next_current_bucket = bucket;
      return *this;
    }

    // if we are pointing on 0, i.e., we are at the end of the chained
    // list, and the list contains some elements, point on the last one
    if ( !prev_current_bucket && table->nodes[index].nb_elements ) {
      bucket = table->nodes[index].end_list;
      prev_current_bucket = bucket;
      next_current_bucket = bucket;
      return *this;
    }

    // check if we are not at the beginning of the table
    // it is compulsory to perform this test before entering the for loop below
    // because if index is equal to 0, the for loop would parse unallocated
    // elements of vector node (because index is an unsigned int)
    if ( index == 0 ) {
      bucket = 0;
      prev_current_bucket = bucket;
      next_current_bucket = bucket;
      return *this;
    }

    // arrived here, we need to parse the hash table until we find a new bucket
    // because we are pointing on a chained list with no more element to the left
    // of the current element
    for ( Size i = index - 1UL; i; --i )
      if ( table->nodes[i].nb_elements ) {
        index = i;
        bucket = table->nodes[i].end_list;
        prev_current_bucket = bucket;
        next_current_bucket = bucket;
        return *this;
      }

    // we are at the beginning of the hash table, do nothing
    index = 0UL;
    bucket = table->nodes[0].end_list;
    prev_current_bucket = bucket;
    next_current_bucket = bucket;

    return *this;
  }

  // ==============================================================================
  /// checks whether two iterators are pointing toward different elements
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTableIterator<Key,Val>::operator!=
  ( const HashTableIterator<Key, Val> &from ) const  {
    return (( bucket != from.bucket ) ||
            ( index != from.index ) ||
            ( table != from.table ) );
  }

  // ==============================================================================
  /// checks whether two iterators are pointing toward the same element
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  bool HashTableIterator<Key,Val>::operator==
  ( const HashTableIterator<Key, Val> &from ) const  {
    return (( bucket == from.bucket ) &&
            ( index == from.index ) &&
            ( table == from.table ) );
  }

  // ==============================================================================
  /// returns the value pointed to by the iterator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val& HashTableIterator<Key,Val>::operator*()  {
    if ( bucket )
      return bucket->val;
    else
      GUM_ERROR( UndefinedIteratorValue, "Accessing a NULL object" );
  }

  // ==============================================================================
  /// dereferences the value pointed to by the iterator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Val* HashTableIterator<Key,Val>::operator->() const {
    if ( bucket )
      return &( bucket->val );
    else
      GUM_ERROR( UndefinedIteratorValue, "Accessing a NULL object" );
  }

  // ==============================================================================
  /// returns the value pointed to by the iterator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  const Val& HashTableIterator<Key,Val>::operator*()
    const  {
    if ( bucket )
      return bucket->val;
    else
      GUM_ERROR( UndefinedIteratorValue, "Accessing a NULL object" );
  }

  // ==============================================================================
  /// a method used by the hashtables to construct properly end and rend iters
  // ==============================================================================
  template <typename Key, typename Val>
  void HashTableIterator<Key,Val>::initialize
  ( HashTable<Key, Val> *tab, bool forward )  {
    // attach the hash table to the iterator
    table = tab;
    // fill the iterator with the corresponding bucket
    bucket = 0;
    next_current_bucket = 0;
    prev_current_bucket = 0;

    if ( forward )
      // initialize end()
      index = table->__size;
    else
      // initialize rend()
      index = 0;
  }

  // ==============================================================================
  /// returns the current iterator's bucket
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  HashTableBucket<Key,Val>* HashTableIterator<Key,Val>::getBucket()
    const {
    return bucket;
  }

  // ==============================================================================
  /// returns the index in the hashtable's node vector pointed to by the iterator
  // ==============================================================================
  template <typename Key, typename Val> INLINE
  Size HashTableIterator<Key,Val>::getIndex() const  {
    return index;
  }  

  // ==============================================================================
  // a << operator for HashTableList
  // ==============================================================================
  template <typename Key, typename Val>
  std::ostream& operator<< ( std::ostream& stream,
                             const HashTableList<Key,Val>& list ) {
    bool deja=false;
    stream << "[";
  
    for ( HashTableBucket<Key, Val>* ptr = list.deb_list; ptr;
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
  
    for ( HashTableBucket<Key, Val>* ptr = list.deb_list; ptr;
          ptr = ptr->list.next, deja = true ) {
      if ( deja ) stream << " , ";
    
      stream << (( unsigned long ) ptr->key ) << "=>" << ptr->val;
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
      for ( HashTableBucket<Key,Val>* ptr = table.nodes[i].deb_list; ptr;
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
      for ( HashTableBucket<Key,Val>* ptr = table.nodes[i].deb_list; ptr;
            ptr=ptr->next ) {
        if ( deja ) stream << " , ";
      
        stream << (( unsigned long ) ptr->key ) << "=>" << ptr->val;
      
        deja = true;
      }
  
    stream << "}";
  
    return stream;
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

