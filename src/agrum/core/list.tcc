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
 * @brief template implementation of chained lists
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <sstream>
#include <agrum/core/debug.h>


namespace gum {


  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                         BUCKET IMPLEMENTATION                       === */
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// default constructor
  // ==============================================================================
  template <typename Val> INLINE
  ListBucket<Val>::ListBucket( const Val& v ) :
    __prev( 0 ), __next( 0 ), __val( v ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( ListBucket );
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  template <typename Val> INLINE
  ListBucket<Val>::ListBucket( const ListBucket<Val>& from ) :
    __prev( 0 ), __next( 0 ), __val( from.__val ) {
    // for debugging purposes
    GUM_CONS_CPY( ListBucket );
  }

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  template <typename Val> INLINE
  ListBucket<Val>& ListBucket<Val>::operator= ( const ListBucket<Val>& from ) {
    // for debugging purposes
    GUM_OP_CPY( ListBucket );

    // no need to avoid self assignment
    __val = from.__val;
    return *this;
  }

  // ==============================================================================
  // WARNING: during its deletion, the bucket takes care of properly
  // rechaining the chained list. However, it has no knowledge about the variables
  // that keep track of the beginning/end of the chained list, hence it cannot
  // update them properly. This should be done by the List/ListBase themselves
  // ==============================================================================
  template <typename Val> INLINE
  ListBucket<Val>::~ListBucket() {
    // for debugging purposes
    GUM_DESTRUCTOR( ListBucket );
    
    // rechain properly the buckets
    if ( __prev ) __prev->__next = __next;
    if ( __next ) __next->__prev = __prev;
  }

  // ==============================================================================
  // equality check
  // ==============================================================================
  template <typename Val> INLINE
  bool ListBucket<Val>::operator== ( const ListBucket<Val>& from ) const {
    return ( from.__val == __val );
  }

  // ==============================================================================
  // inequality check
  // ==============================================================================
  template <typename Val> INLINE
  bool ListBucket<Val>::operator!= ( const ListBucket<Val>& from ) const {
    return ( from.__val != __val );
  }

  // ==============================================================================
  // dereferencing operator
  // ==============================================================================
  template <typename Val> INLINE
  const Val& ListBucket<Val>::operator*() const  {
    return __val;
  }

  // ==============================================================================
  // dereferencing operator
  // ==============================================================================
  template <typename Val> INLINE
  Val& ListBucket<Val>::operator*()  {
    return __val;
  }



  

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                     GUM_LIST_BASE IMPLEMENTATION                    === */
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// a function used to perform copies of elements of ListBases.
  // ==============================================================================
  template <typename Val>
  void ListBase<Val>::__copy_elements( const ListBase<Val> &from ) {
    ListBucket<Val> *ptr, *old_ptr;

    // set the defaults
    __deb_list = 0;
    __end_list = 0;
    __nb_elements = 0;

    // copy from's list
    try {
      for ( ptr = from.__deb_list, old_ptr = 0; ptr; ptr = ptr->__next ) {
        // create a copy bucket
        ListBucket<Val> *new_elt = new ListBucket<Val>( *ptr );

        // rechain properly the new list
        new_elt->__prev = old_ptr;
        new_elt->__next = 0;

        if ( old_ptr ) old_ptr->__next = new_elt;
        else __deb_list = new_elt;

        old_ptr = new_elt;
      }
    }
    catch ( ... ) {
      // problem: we could not allocate an element in the list => we delete
      // the elements created so far and we throw an exception
      for ( ; __deb_list; __deb_list = ptr ) {
        ptr = __deb_list->__next;
        delete __deb_list;
      }

      __deb_list = 0;
      throw;
    }

    // update properly the end of the chained list and the number of elements
    __end_list = old_ptr;
    __nb_elements = from.__nb_elements;
  }

  // ==============================================================================
  /// returns the bucket corresponding to a given value.
  // ==============================================================================
  template <typename Val> INLINE
  ListBucket<Val>* ListBase<Val>::__getBucket( const Val& val ) const {
    for ( ListBucket<Val> *ptr = __deb_list; ptr; ptr = ptr->__next )
      if ( ptr->__val == val ) return ptr;

    return 0;
  }

  // ==============================================================================
  /// suppresses an element from a chained list.
  // ==============================================================================
  template <typename Val> INLINE
  void ListBase<Val>::__erase( ListBucket<Val>* bucket ) {
    // perform deletion only if there is a bucket to remove
    if ( bucket ) {
      // set properly the begin and end of the chained list (the other chainings
      // will be performed by operator delete)
      if ( !bucket->__prev ) __deb_list = bucket->__next;
      if ( !bucket->__next ) __end_list = bucket->__prev;

      // remove the current element from the list
      delete bucket;
      --__nb_elements;
    }
  }

  // ==============================================================================
  /// displays the content of a chained list
  // ==============================================================================
  template <typename Val>
  std::string ListBase<Val>::toString() const {
    bool deja = false;
    std::stringstream stream;
    stream << "[";

    for ( ListBucket<Val>* ptr = __deb_list; ptr; ptr=ptr->__next, deja=true ) {
      if ( deja ) stream << " --> ";

      stream << ptr->__val;
    }

    stream << "]";

    return stream.str();
  }

  // ==============================================================================
  /// A basic constructor that creates an empty list.
  // ==============================================================================
  template <typename Val> INLINE
  ListBase<Val>::ListBase() :
    __deb_list( 0 ), __end_list( 0 ), __nb_elements( 0 ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( ListBase );
  }

  // ==============================================================================
  /// Copy constructor
  // ==============================================================================
  template <typename Val> INLINE
  ListBase<Val>::ListBase( const ListBase<Val> &from ) {
    // for debugging purposes
    GUM_CONS_CPY( ListBase );

    // actually copy the elements of list from
    __copy_elements( from );
  }

  // ==============================================================================
  /// Copy operator.
  // ==============================================================================
  template <typename Val> INLINE
  ListBase<Val>& ListBase<Val>::operator= ( const ListBase<Val>& from ) {
    // avoid self assignment
    if ( this != &from ) {
      // for debugging purposes
      GUM_OP_CPY( ListBase );
      
      // remove the old content of 'this'
      ListBucket<Val> *ptr, *next_ptr;
      for ( ptr = __deb_list; ptr; ptr = next_ptr ) {
        next_ptr = ptr->__next;
        delete ptr;
      }

      // perform the copy
      __copy_elements( from );
    }

    return *this;
  }

  // ==============================================================================
  /// Destructor.
  // ==============================================================================
  template <typename Val>
  ListBase<Val>::~ListBase() {
    // for debugging (although this program is bug-free)
    GUM_DESTRUCTOR( ListBase );
    
    // we remove all the elements from the list
    for ( ListBucket<Val> *ptr = __deb_list, *next_ptr = 0; ptr; ptr = next_ptr ) {
      next_ptr = ptr->__next;
      delete ptr;
    }
  }

  // ==============================================================================
  /// Insertion of a new element (a copy) at the beginning of the chained list.
  // ==============================================================================
  template <typename Val> INLINE
  Val& ListBase<Val>::pushFront( const Val& val ) {
    // create a new bucket
    ListBucket<Val> *new_elt = new ListBucket<Val>( val );
    
    // place the bucket at the beginning of the list
    new_elt->__next = __deb_list;
    if ( __deb_list ) __deb_list->__prev = new_elt;
    else __end_list = new_elt;
    __deb_list = new_elt;

    // update the number of elements
    ++__nb_elements;

    // returns the current value
    return new_elt->__val;
  }

  // ==============================================================================
  /// Insertion of a new element (a copy) at the end of the chained list.
  // ==============================================================================
  template <typename Val> INLINE
  Val& ListBase<Val>::pushBack( const Val& val ) {
    // create a new bucket
    ListBucket<Val> *new_elt = new ListBucket<Val>( val );
    
    // place the bucket at the end of the list
    new_elt->__prev = __end_list;
    if ( __end_list ) __end_list->__next = new_elt;
    else __deb_list = new_elt;
    __end_list = new_elt;

    // update the number of elements
    ++__nb_elements;

    // returns the current value
    return new_elt->__val;
  }

  // ==============================================================================
  /// Insertion of a new element at the end of the chained list (alias of pushBack)
  // ==============================================================================
  template <typename Val> INLINE
  Val& ListBase<Val>::insert( const Val& val ) {
    return pushBack( val );
  }

  // ==============================================================================
  /// returns a reference to first element of a list
  // ==============================================================================
  template <typename Val> INLINE
  Val& ListBase<Val>::front() const  {
    if ( __nb_elements == 0 )
      GUM_ERROR( NotFound, "not enough elements in the chained list" );

    return __deb_list->__val;
  }

  // ==============================================================================
  /// returns a reference to last element of a list
  // ==============================================================================
  template <typename Val> INLINE
  Val& ListBase<Val>::back() const  {
    if ( __nb_elements == 0 )
      GUM_ERROR( NotFound, "not enough elements in the chained list" );

    return __end_list->__val;
  }

  // ==============================================================================
  /// returns the number of elements in the list.
  // ==============================================================================
  template <typename Val> INLINE
  Size ListBase<Val>::size() const  {
    return __nb_elements;
  }

  // ==============================================================================
  /// checks whether there exists a given element in the list.
  // ==============================================================================
  template <typename Val> INLINE
  bool ListBase<Val>::exists( const Val& val ) const {
    for ( ListBucket<Val> *ptr = __deb_list; ptr; ptr = ptr->__next )
      if ( ptr->__val == val ) return true;

    return false;
  }

  // ==============================================================================
  /// erases the first element encountered with a given value
  // ==============================================================================
  template <typename Val> INLINE
  void ListBase<Val>::eraseByVal( const Val& val ) {
    __erase( __getBucket( val ) );
  }

  // ==============================================================================
  /// erases the ith element of the List (the first one is in position 0)
  // ==============================================================================
  template <typename Val> INLINE
  void ListBase<Val>::erase( unsigned int i ) {
    if ( i >= __nb_elements ) return;

    // parse the list
    ListBucket<Val> *ptr;
    if ( i < __nb_elements / 2 )
      for ( ptr=__deb_list; i; --i, ptr=ptr->__next ) {}
    else
      for ( ptr=__end_list, i=__nb_elements - i - 1; i; --i, ptr=ptr->__prev ) {}

    __erase( ptr );
  }

  // ==============================================================================
  /// erases all the elements encountered with a given value
  // ==============================================================================
  template <typename Val> INLINE
  void ListBase<Val>::eraseAllVal( const Val& val ) {
    for ( ListBucket<Val> *ptr = __deb_list, *next_ptr=0; ptr; ptr = next_ptr ) {
      next_ptr = ptr->__next;

      if ( val == **ptr )
        __erase( ptr );
    }
  }

  // ==============================================================================
  /// removes the last element of a ListBase
  // ==============================================================================
  template <typename Val> INLINE
  void ListBase<Val>::popBack() {
    if ( __nb_elements != 0 )
      __erase( __end_list );
  }

  // ==============================================================================
  /// removes the first element of a ListBase
  // ==============================================================================
  template <typename Val> INLINE
  void ListBase<Val>::popFront() {
    if ( __nb_elements != 0 )
      __erase( __deb_list );
  }

  // ==============================================================================
  /// deletes all the elements of a chained list.
  // ==============================================================================
  template <typename Val> INLINE
  void ListBase<Val>::clear() {
    for ( ListBucket<Val> *ptr = __deb_list, *next_ptr=0; ptr; ptr = next_ptr ) {
      next_ptr = ptr->__next;
      delete ptr;
    }

    __nb_elements = 0;
    __deb_list = 0;
    __end_list = 0;
  }

  // ==============================================================================
  /// returns a boolean indicating whether the chained list is empty
  // ==============================================================================
  template <typename Val> INLINE
  bool ListBase<Val>::empty() const  {
    return ( __nb_elements == 0 );
  }

  // ==============================================================================
  /// creates a list of mountains from a list of val
  // ==============================================================================
  template <typename Val>
  template <typename Mount> INLINE
  ListBase<Mount> ListBase<Val>::map( Mount( *f )( Val& ) ) const {
    // create a new empty list
    ListBase<Mount> list;
    
    // fill the new list
    for ( ListBucket<Val> *ptr = __deb_list; ptr; ptr = ptr->__next )
      list.pushBack( f( **ptr ) );

    return list;
  }

  // ==============================================================================
  /// creates a list of mountains from a list of val
  // ==============================================================================
  template <typename Val>
  template <typename Mount> INLINE
  ListBase<Mount> ListBase<Val>::map( Mount( *f )( const Val& ) ) const {
    // create a new empty list
    ListBase<Mount> list;
    
    // fill the new list
    for ( ListBucket<Val> *ptr = __deb_list; ptr; ptr = ptr->__next )
      list.pushBack( f( **ptr ) );

    return list;
  }

  // ==============================================================================
  /// creates a list of mountains from a list of val
  // ==============================================================================
  template <typename Val>
  template <typename Mount> INLINE
  ListBase<Mount> ListBase<Val>::map( Mount( *f )( Val ) ) const {
    // create a new empty list
    ListBase<Mount> list;
    
    // fill the new list
    for ( ListBucket<Val> *ptr = __deb_list; ptr; ptr = ptr->__next )
      list.pushBack( f( **ptr ) );

    return list;
  }

  // ==============================================================================
  /// creates a list of mountains with a given value from a list of val
  // ==============================================================================
  template <typename Val>
  template <typename Mount> INLINE
  ListBase<Mount> ListBase<Val>::map( const Mount& mount ) const {
    // create a new empty list
    ListBase<Mount> list;
    
    // fill the new list
    for ( unsigned int i = 0; i < __nb_elements; ++i )
      list.pushBack( mount );

    return list;
  }

  // ==============================================================================
  /// creates and insert a new element at the end of the list (alias of pushBack).
  // ==============================================================================
  template <typename Val> INLINE
  Val& ListBase<Val>::operator+= ( const Val& val ) {
    return pushBack( val );
  }

  // ==============================================================================
  /// checks whether two lists are identical (same elements in the same order)
  // ==============================================================================
  template <typename Val> INLINE
  bool ListBase<Val>::operator== ( const ListBase<Val>& from ) const {
    // check if the two lists have at least the same number of elements
    if ( __nb_elements != from.__nb_elements ) return false;

    // parse the two lists
    for ( ListBucket<Val> *iter1=__deb_list, *iter2=from.__deb_list;
          iter1; iter1=iter1->__next, iter2=iter2->__next )
      if ( *iter1 != *iter2 ) return false;

    return true;
  }

  // ==============================================================================
  /// checks whether two lists are different (different elements or orders)
  // ==============================================================================
  template <typename Val> INLINE
  bool ListBase<Val>::operator!= ( const ListBase<Val>& from ) const {
    // check whether the two lists have different numbers of elements
    if ( __nb_elements != from.__nb_elements ) return true;

    // parse the two lists
    for ( ListBucket<Val> *iter1=__deb_list, *iter2=from.__deb_list;
          iter1; iter1=iter1->__next, iter2=iter2->__next )
      if ( *iter1 != *iter2 ) return true;

    return false;
  }

  // ==============================================================================
  /// returns the ith element in the current chained list.
  // ==============================================================================
  template <typename Val> INLINE
  Val& ListBase<Val>::operator[]( unsigned int i )  {
    // check if we can return the element we ask for
    if ( i >= __nb_elements )
      GUM_ERROR( NotFound, "not enough elements in the chained list" );

    // parse the list and return the element
    ListBucket<Val> *ptr;
    if ( i <= __nb_elements / 2 )
      for ( ptr=__deb_list; i; --i, ptr=ptr->__next ) {}
    else
      for ( ptr=__end_list, i=__nb_elements - i - 1; i; --i, ptr=ptr->__prev ) {}

    return ptr->__val;
  }

  // ==============================================================================
  /// returns the ith element in the current chained list.
  // ==============================================================================
  template <typename Val> INLINE
  const Val& ListBase<Val>::operator[]( unsigned int i )
    const  {
    // check if we can return the element we ask for
    if ( i >= __nb_elements )
      GUM_ERROR( NotFound, "not enough elements in the chained list" );

    // parse the list and return the element
    ListBucket<Val> *ptr;
    if ( i <= __nb_elements / 2 )
      for ( ptr=__deb_list; i; --i, ptr=ptr->__next ) {}
    else
      for ( ptr=__end_list, i=__nb_elements - i - 1; i; --i, ptr=ptr->__prev ) {}

    return ptr->__val;
  }



  

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                        GUM_LIST IMPLEMENTATION                      === */
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /** @brief A basic constructor that creates an empty list as well as its own
   * iterator's list */
  // ==============================================================================
  template <typename Val>
  List<Val>::List():
    __iterator_list( new ListBase<ListIterator<Val>*> ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( List );
  }

  // ==============================================================================
  /** @brief A basic constructor that creates an empty list. The iterator's list
   * may be shared by several List when \e iter_list is different from a
   * RefPtr containing 0. */
  // ==============================================================================
  template <typename Val>
  List<Val>::List
  ( const RefPtr< ListBase<ListIterator<Val>*> >& iter_list ) :
    __iterator_list( 0 ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( List );

    try {
      if ( iter_list.operator->() != 0 )
        __iterator_list = iter_list;
      // note that, if iter_list contains a null pointer, then operator() will
      // raise a NullElement exception 
    }
    catch ( NullElement& ) {
      __iterator_list = RefPtr< ListBase<ListIterator<Val>*> >
        ( new ListBase<ListIterator<Val>*> );
    }
  }

  // ==============================================================================
  /// Copy constructor sharing its iterator's list with that of \c from
  // ==============================================================================
  template <typename Val>
  List<Val>::List( const List<Val> &from ) :
    ListBase<Val>( from ), __iterator_list( from.__iterator_list ) {
    // for debugging purposes
    GUM_CONS_CPY( List );
  }

  // ==============================================================================
  /** @brief Copy constructor. The iterator's list is shared with that passed in
   * argument */
  // ==============================================================================
  template <typename Val>
  List<Val>::List( const List<Val> &from,
                   const RefPtr< ListBase<ListIterator<Val>*> >& iter_list ) :
    ListBase<Val>( from ), __iterator_list( 0 ) {
    // for debugging purposes
    GUM_CONS_CPY( List );

    try {
      if ( iter_list.operator-> () != 0 )
        __iterator_list = iter_list;
      // note that, if iter_list contains a null pointer, then operator() will
      // raise a NullElement exception 
    }
    catch ( NullElement& ) {
      __iterator_list = RefPtr< ListBase<ListIterator<Val>*> >
        ( new ListBase<ListIterator<Val>*> );
    }
  }

  // ==============================================================================
  /// Copy operator. The List iterator's list is not shared with that of \e from.
  // ==============================================================================
  template <typename Val>
  List<Val>& List<Val>::operator= ( const List<Val>& from ) {
    // avoid self assignment
    if ( this != &from ) {
      // for debugging purposes
      GUM_OP_CPY( List );
      
      // remove the old content of 'this' and update accordingly the iterators
      clear();

      // perform the copy
      ListBase<Val>::operator= ( from );
    }

    return *this;
  }

  // ==============================================================================
  /// Destructor
  // ==============================================================================
  template <typename Val>
  List<Val>::~List() {
    // for debugging (although this program is bug-free)
    GUM_DESTRUCTOR( List );
    
    // we detach all the iterators attached to the current List
    clear();
  }

  // ==============================================================================
  /// deletes all the elements of a chained list.
  // ==============================================================================
  template <typename Val>
  void List<Val>::clear() {
    // first we update all the iterators of the list : they should now
    // point to rend
    ListIterator<Val> *ptr;

    for ( ListBucket<ListIterator<Val>*> *iter = __iterator_list->__deb_list,
            *iter_next=0; iter; iter=iter_next ) {
      ptr = **iter;
      iter_next = iter->__next;

      if ( ptr->__list == this ) {
        ptr->clear();
      }
    }

    // we can now safely remove the elements from the list
    ListBase<Val>::clear();
  }

  // ==============================================================================
  /** @brief changes the iterator's list (this implies that all iterators
   * previously attached to the List will point to end/rend) */
  // ==============================================================================
  template <typename Val>
  void List<Val>::setIteratorList
  ( const RefPtr< ListBase<ListIterator<Val>*> >& new_list ) {
    // first we update all the iterators of the list : they should now
    // point to rend
    ListIterator<Val> *ptr;
    for ( ListBucket<ListIterator<Val>*> *iter = __iterator_list->__deb_list,
            *iter_next=0; iter; iter=iter_next ) {
      ptr = **iter;
      iter_next = iter->__next;

      if ( ptr->__list == this ) {
        ptr->clear();
      }
    }

    // we can now safely remove the list and copy the new one
    __iterator_list = new_list;
  }

  // ==============================================================================
  /// returns the list containing the iterators pointing toward the List
  // ==============================================================================
  template <typename Val> INLINE
  const RefPtr< ListBase<ListIterator<Val>*> >&
  List<Val>::getIteratorList() const  {
    return __iterator_list;
  }

  // ==============================================================================
  /// suppresses an element from a chained list.
  // ==============================================================================
  template <typename Val> INLINE
  void List<Val>::_erase( ListBucket<Val>* bucket ) {
    if ( bucket ) {
      ListIterator<Val> *ptr;
      
      // update the iterators pointing on this element
      for ( ListBucket<ListIterator<Val>*> *iter = __iterator_list->__deb_list;
            iter; iter=iter->__next ) {
        ptr = iter->__val;

        if ( ptr->__bucket == bucket ) {
          ptr->__next_current_bucket = bucket->__prev;
          ptr->__prev_current_bucket = bucket->__next;
          ptr->__bucket = 0;
          ptr->__null_pointing = true;
        }

        if ( ptr->__next_current_bucket == bucket )
          ptr->__next_current_bucket = bucket->__prev;

        if ( ptr->__prev_current_bucket == bucket )
          ptr->__prev_current_bucket = bucket->__next;
      }

      ListBase<Val>::__erase( bucket );
    }
  }

  // ==============================================================================
  /// the iterator pointing to the end of the List
  // ==============================================================================
  template <typename Val> INLINE
  const ListIterator<Val>& List<Val>::end() const  {
    // ok, the code below is a little bit complicated, but it avoids the
    // "Derefencing type-punned pointer will break strict-aliasing rules" warning
    return *( static_cast<const ListIterator<Val>*>(( void* ) &__list_end ) );
  }

  // ==============================================================================
  /// the iterator pointing to the rend (just before the beginning) of the List
  // ==============================================================================
  template <typename Val> INLINE
  const ListIterator<Val>& List<Val>::rend() const  {
    // ok, the code below is a little bit complicated, but it avoids the
    // "Derefencing type-punned pointer will break strict-aliasing rules" warning
    return *( static_cast<const ListIterator<Val>*>(( void* ) &__list_end ) );
  }

  // ==============================================================================
  /// the iterator pointing to the beginning of the List
  // ==============================================================================
  template <typename Val> INLINE
  ListIterator<Val> List<Val>::begin() const {
    if ( ListBase<Val>::__nb_elements )
      return ListIterator<Val> ( *this, 0U );
    else
      return ListIterator<Val>();
  }

  // ==============================================================================
  /// the iterator pointing to the rbegin (the last element) of the List
  // ==============================================================================
  template <typename Val> INLINE
  ListIterator<Val> List<Val>::rbegin() const {
    if ( ListBase<Val>::__nb_elements )
      return ListIterator<Val> ( *this, ListBase<Val>::__nb_elements-1 );
    else
      return ListIterator<Val>();
  }

  // ==============================================================================
  /// erases the first element encountered with a given value
  // ==============================================================================
  template <typename Val> INLINE
  void List<Val>::eraseByVal( const Val& val ) {
    _erase( ListBase<Val>::__getBucket( val ) );
  }

  // ==============================================================================
  /// erases the ith element of the List (the first one is in position 0)
  // ==============================================================================
  template <typename Val> INLINE
  void List<Val>::erase( unsigned int i ) {
    if ( i >= ListBase<Val>::__nb_elements ) return;

    // parse the list
    ListBucket<Val> *ptr;
    if ( i < ListBase<Val>::__nb_elements / 2 )
      for ( ptr=ListBase<Val>::__deb_list; i; --i, ptr=ptr->__next ) {}
    else
      for ( ptr=ListBase<Val>::__end_list, i=ListBase<Val>::__nb_elements - i - 1;
            i; --i, ptr=ptr->__prev ) {}

    _erase( ptr );
  }

  // ==============================================================================
  /// erases the element of the List pointed to by the iterator
  // ==============================================================================
  template <typename Val> INLINE
  void List<Val>::erase( const iterator& iter ) {
    _erase ( iter.__getBucket() );
  }

  // ==============================================================================
  /// erases all the elements encountered with a given value
  // ==============================================================================
  template <typename Val> INLINE
  void List<Val>::eraseAllVal( const Val& val ) {
    for ( ListBucket<Val> *iter=ListBase<Val>::__deb_list, *next_bucket=0;
          iter; iter = next_bucket ) {
      next_bucket = iter->__next;

      if ( val == iter->__val )
        _erase( iter );
    }
  }

  // ==============================================================================
  /// removes the last element of a List
  // ==============================================================================
  template <typename Val> INLINE
  void List<Val>::popBack() {
    if ( ListBase<Val>::__nb_elements )
      _erase( ListBase<Val>::__end_list );
  }

  // ==============================================================================
  /// removes the first element of a List
  // ==============================================================================
  template <typename Val> INLINE
  void List<Val>::popFront() {
    if ( ListBase<Val>::__nb_elements )
      _erase( ListBase<Val>::__deb_list );
  }

  // ==============================================================================
  /// creates a list of mountains from a list of val
  // ==============================================================================
  template <typename Val>
  template <typename Mount> List<Mount> List<Val>::map( Mount( *f )( Val ) )
    const {
    // create a new empty list
    List<Mount> list;
    
    // fill the new list
    for ( ListIterator<Val> iter = begin(); iter != end(); ++iter ) {
      list.pushBack( f( *iter ) );
    }

    return list;
  }

  // ==============================================================================
  /// creates a list of mountains from a list of val
  // ==============================================================================
  template <typename Val>
  template <typename Mount> List<Mount> List<Val>::map( Mount( *f )( Val& ) )
    const {
    // create a new empty list
    List<Mount> list;
    
    // fill the new list
    for ( ListIterator<Val> iter = begin(); iter != end(); ++iter ) {
      list.pushBack( f( *iter ) );
    }

    return list;
  }

  // ==============================================================================
  /// creates a list of mountains from a list of val
  // ==============================================================================
  template <typename Val>
  template <typename Mount> List<Mount> List<Val>::map( Mount( *f )( const Val& ) )
    const {
    // create a new empty list
    List<Mount> list;
    
    // fill the new list
    for ( ListIterator<Val> iter = begin(); iter != end(); ++iter ) {
      list.pushBack( f( *iter ) );
    }

    return list;
  }

  // ==============================================================================
  /// creates a list of mountains with a given value from a list of val
  // ==============================================================================
  template <typename Val>
  template <typename Mount> List<Mount> List<Val>::map( const Mount& mount )
    const {
    // create a new empty list
    List<Mount> list;
    
    // fill the new list
    for ( unsigned int i=0; i<ListBase<Val>::__nb_elements; ++i )
      list.pushBack( mount );

    return list;
  }

  // ==============================================================================
  /// creates and insert a new element at the end of the list (alias of pushBack).
  // ==============================================================================
  template <typename Val> INLINE
  Val& List<Val>::operator+= ( const Val& val ) {
    return ListBase<Val>::operator+= ( val );
  }

  // ==============================================================================
  /// checks whether two lists are identical (same elements in the same order)
  // ==============================================================================
  template <typename Val> INLINE
  bool List<Val>::operator== ( const List<Val>& from ) const {
    return ListBase<Val>::operator== ( from );
  }

  // ==============================================================================
  /// checks whether two lists are different (different elements or orders)
  // ==============================================================================
  template <typename Val> INLINE
  bool List<Val>::operator!= ( const List<Val>& from ) const {
    return ListBase<Val>::operator!= ( from );
  }

  // ==============================================================================
  /// returns the ith element in the current chained list.
  // ==============================================================================
  template <typename Val> INLINE
  Val& List<Val>::operator[]( const unsigned int i )  {
    return ListBase<Val>::operator[]( i );
  }

  // ==============================================================================
  /// returns the ith element in the current chained list.
  // ==============================================================================
  template <typename Val> INLINE
  const Val& List<Val>::operator[]( const unsigned int i ) const {
    return ListBase<Val>::operator[]( i );
  }





  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                   GUM_LIST_ITERATOR IMPLEMENTATION                  === */
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// basic constructor
  // ==============================================================================
  template <typename Val> INLINE
  ListIterator<Val>::ListIterator()  :
    __list( 0 ), __bucket( 0 ),
    __next_current_bucket( 0 ), __prev_current_bucket( 0 ),
    __null_pointing( false ), __container( 0 ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( ListIterator );
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  template <typename Val> INLINE
  ListIterator<Val>::ListIterator( const ListIterator<Val>& from ) :
    __list( from.__list ), __bucket( from.__bucket ),
    __next_current_bucket( from.__next_current_bucket ),
    __prev_current_bucket( from.__prev_current_bucket ),
    __null_pointing( from.__null_pointing ), __container( 0 ) {
    // for debugging purposes
    GUM_CONS_CPY( ListIterator );
    
    // rechain properly the list of iterators of the List
    if ( __list ) {
      __list->__iterator_list->pushFront( this );
      __container = __list->__iterator_list->__getBucket( this );
    }
  }

  // ==============================================================================
  /// Constructor for an iterator pointing to the \e ind_eltth element of a List.
  // ==============================================================================
  template <typename Val> INLINE
  ListIterator<Val>::ListIterator( const List<Val>& theList,
                                   unsigned int ind_elt ) :
    __list( &theList ), __bucket( 0 ), __next_current_bucket( 0 ),
    __prev_current_bucket( 0 ), __null_pointing( false ), __container( 0 ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( ListIterator );
    
    // check if the index ind_elt passed as parameter is valid
    if ( ind_elt >= __list->__nb_elements )
      GUM_ERROR( UndefinedIteratorValue,"Not enough elements in the list" );

    // check if it is faster to find the indexth element from the start or
    // from the end of the list
    if ( ind_elt < ( __list->__nb_elements >> 1 ) ) {
      // find the element we shall point to from the start of the list
      for ( __bucket = __list->__deb_list; ind_elt
              ; --ind_elt, __bucket = __bucket->__next ) {}
    }
    else {
      // find the element we shall point to from the end of the list
      for ( __bucket = __list->__end_list,
              ind_elt = __list->__nb_elements - ind_elt - 1;
            ind_elt; --ind_elt, __bucket = __bucket->__prev ) {}
    }

    // rechain properly the iterator's list of the List
    __list->__iterator_list->pushFront( this );
    __container = __list->__iterator_list->__getBucket( this );
  }

  // ==============================================================================
  /// Copy operator
  // ==============================================================================
  template <typename Val> INLINE
  ListIterator<Val>& ListIterator<Val>::operator=
  ( const ListIterator<Val>& from ) {
    // avoid self assignment
    if ( this != &from ) {
      // for debugging purposes
      GUM_OP_CPY( ListIterator );
      
      // check if from and this belong to the same iterator's list. If this is not
      // the case, we shall remove the iterator from its current List iterator's
      // list and add it to the new List iterator's list
      if ( __list &&
           ( !from.__list ||
             (*(__list->__iterator_list) != *(from.__list->__iterator_list)))) {
        // remove the iterator from its List iterator's list'
        __list->__iterator_list->__erase( __container );
        __list = 0;
      }

      // if necessary, add the iterator to from's iterator's list
      if ( from.__list &&
           (!__list ||
            (*(__list->__iterator_list) != *(from.__list->__iterator_list)))) {
        try {
          from.__list->__iterator_list->pushFront( this );
          __list = from.__list;
          __container = __list->__iterator_list->__getBucket( this );
        }

        // if a problem occurred, make the iterator point on 0
        catch ( ... ) {
          __container = 0;
          __bucket = 0;
          __list = 0;
          __prev_current_bucket = 0;
          __next_current_bucket = 0;
          __null_pointing = false;
        }
      }

      __bucket = from.__bucket;
      __prev_current_bucket = from.__prev_current_bucket;
      __next_current_bucket = from.__next_current_bucket;
      __null_pointing = from.__null_pointing;
    }

    return *this;
  }

  // ==============================================================================
  /// Destructor
  // ==============================================================================
  template <typename Val> INLINE
  ListIterator<Val>::~ListIterator()  {
    // for debugging purposes
    GUM_DESTRUCTOR( ListIterator );
    
    // remove the iterator from the table's iterator list
    if ( __list )
      __list->__iterator_list->__erase( __container );
  }

  // ==============================================================================
  /// returns the bucket the iterator is pointing to
  // ==============================================================================
  template <typename Val> INLINE
  ListBucket<Val>* ListIterator<Val>::__getBucket() const  {
    return __bucket;
  }

  // ==============================================================================
  /// Makes the iterator point toward nothing
  // ==============================================================================
  template <typename Val> INLINE
  void ListIterator<Val>::clear()  {
    // remove the iterator from the list's iterator list
    if ( __list )
      __list->__iterator_list->__erase( __container );

    // set its list as well as the element it points to to 0
    __list = 0;
    __bucket = 0;
    __prev_current_bucket = 0;
    __next_current_bucket = 0;
    __null_pointing = false;
    __container = 0;
  }

  // ==============================================================================
  /// makes the iterator point to the next element in the List
  // ==============================================================================
  template <typename Val> INLINE
  ListIterator<Val>& ListIterator<Val>::operator++()  {
    // check if we are pointing to something that has been deleted
    if ( __null_pointing ) {
      // if we are pointing to an element of the chained list that has been deleted
      // but that has a next element, just point on the latter
      if ( __next_current_bucket ) {
        __bucket = __next_current_bucket->__next;
        __null_pointing = false;
        return *this;
      }

      // here we were pointing on an extremity of the list (either end or rend)
      // if prev_current_bucket is not null, then we are at rend and doing
      // a ++ shall now point to the beginning of the list
      if ( __prev_current_bucket ) {
        __bucket = __prev_current_bucket;
        __null_pointing = false;
        return *this;
      }

      // here, we are at the end of the chained list, hence we shall remain at end
      __bucket = 0;
      __null_pointing = false;
      return *this;
    }
    else {
      // if we are pointing to an element of the chained list, just
      // point on the next bucket in this list
      if ( __bucket ) {
        __bucket = __bucket->__next;
        return *this;
      }

      // we were pointing on an extremity of the list (either end or rend)
      // in any case, doing a ++ does not change anything: we are out of
      // the scope of the chained list
      return *this;
    }
  }

  // ==============================================================================
  /// makes the iterator point to the preceding element in the List
  // ==============================================================================
  template <typename Val> INLINE
  ListIterator<Val>& ListIterator<Val>::operator--()  {
    // check if we are pointing to something that has been deleted
    if ( __null_pointing ) {
      // if we are pointing to an element of the chained list that has been deleted
      // but that has a preceding element, just point on the latter
      if ( __prev_current_bucket ) {
        __bucket = __prev_current_bucket->__prev;
        __null_pointing = false;
        return *this;
      }

      // here we were pointing on an extremity of the list (either end or rend)
      // if next_current_bucket is not null, then we are at end and doing
      // a -- shall now point to the beginning of the list
      if ( __next_current_bucket ) {
        __bucket = __next_current_bucket;
        __null_pointing = false;
        return *this;
      }

      // here, we are at the rend of the chained list, hence we shall remain
      // at rend
      __bucket = 0;
      __null_pointing = false;
      return *this;
    }
    else {
      // if we are pointing to an element of the chained list, just
      // point on the preceding bucket in this list
      if ( __bucket ) {
        __bucket = __bucket->__prev;
        return *this;
      }

      // we were pointing on an extremity of the list (either end or rend)
      // in any case, doing a ++ does not change anything: we are out of
      // the scope of the chained list
      return *this;
    }
  }

  // ==============================================================================
  /// checks whether two iterators point toward different elements
  // ==============================================================================
  template <typename Val> INLINE
  bool ListIterator<Val>::operator!= ( const ListIterator<Val> &from ) const  {
    if ( __null_pointing )
      return (( __next_current_bucket != from.__next_current_bucket ) ||
              ( __prev_current_bucket != from.__prev_current_bucket ) );
    else
      return ( __bucket != from.__bucket );
  }

  // ==============================================================================
  /// checks whether two iterators point toward the same elements.
  // ==============================================================================
  template <typename Val> INLINE
  bool ListIterator<Val>::operator== ( const ListIterator<Val> &from ) const  {
    if ( __null_pointing )
      return (( __next_current_bucket == from.__next_current_bucket ) &&
              ( __prev_current_bucket == from.__prev_current_bucket ) );
    else
      return ( __bucket == from.__bucket );
  }

  // ==============================================================================
  /// dereferences the value pointed to by the iterator
  // ==============================================================================
  template <typename Val> INLINE
  Val* ListIterator<Val>::operator->() const  {
    if ( __bucket )
      return &( __bucket->__val );
    else
      GUM_ERROR( UndefinedIteratorValue, "Accessing a NULL object" );
  }

  // ==============================================================================
  /// gives access to the content of the iterator
  // ==============================================================================
  template <typename Val> INLINE
  Val& ListIterator<Val>::operator*()  {
    if ( __bucket )
      return __bucket->__val;
    else
      GUM_ERROR( UndefinedIteratorValue, "Accessing a NULL object" );
  }

  // ==============================================================================
  /// gives access to the content of the iterator
  // ==============================================================================
  template <typename Val> INLINE
  const Val& ListIterator<Val>::operator*()
    const  {
    if ( __bucket )
      return __bucket->__val;
    else
      GUM_ERROR( UndefinedIteratorValue, "Accessing a NULL object" );
  }

  // ==============================================================================
  /// positions the iterator to the end of the list
  // ==============================================================================
  template <typename Val> INLINE
  void ListIterator<Val>::setToEnd()  {
    clear();
  }

  // ==============================================================================
  /// returns a bool indicating whether the iterator points to the end of the list
  // ==============================================================================
  template <typename Val> INLINE
  bool ListIterator<Val>::isEnd() const  {
    if ( __null_pointing )
      return ( !__next_current_bucket && !__prev_current_bucket );
    else
      return ( !__bucket );
  }

  // ==============================================================================
  /// A \c << operator for List
  // ==============================================================================
  template <typename Val>
  std::ostream& operator<< ( std::ostream& stream, const ListBase<Val>& list ) {
    return list.print( stream );
  }

  // ==============================================================================
  /// A \c << operator for List
  // ==============================================================================
  template <typename Val>
  std::ostream& operator<< ( std::ostream& stream, const List<Val>& list ) {
    stream<<list.toString();
    return stream;
  }


} /* namespace gum */


  
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
