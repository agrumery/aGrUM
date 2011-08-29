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
 * @brief template implementations of heaps
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <string>
#include <sstream>


#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease IDE parser
#include <agrum/core/heap.h>

namespace gum {


  // ==============================================================================
  /// basic constructor. Creates an empty heap
  // ==============================================================================
  template <typename Val, typename Cmp>
  Heap<Val,Cmp>::Heap( Cmp compare, Size capacity ) :
      __heap( capacity ), __nb_elements( 0 ), __cmp( compare ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Heap );
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  template <typename Val, typename Cmp>
  Heap<Val,Cmp>::Heap( const Heap<Val,Cmp>& from ) :
      __heap( from.__heap.size() ), __nb_elements( 0 ), __cmp( from.__cmp ) {
    // for debugging purposes
    GUM_CONS_CPY( Heap );

    // copy the elements of from.__heap
    Size i;
    try {
      for ( i = 0; i < from.__nb_elements; ++i )
        __heap[i] = new Val( *from.__heap[i] );
    } catch ( ... ) { // in case all the allocations could not be performed
      for ( Size j = 0; j < i; ++j )
        delete __heap[j];

      throw;
    }

    __nb_elements = from.__nb_elements;
  }

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  template <typename Val, typename Cmp>
  Heap<Val,Cmp>& Heap<Val,Cmp>::operator= ( const Heap<Val,Cmp>& from ) {
    // avoid self assignment
    if ( this != &from ) {
      // for debugging purposes
      GUM_OP_CPY( Heap );

      // set the comparison function
      __cmp = from.__cmp;

      try {
        // if need be, resize the current heap
        if ( __heap.size() < from.__nb_elements )
          __heap.resize( from.__nb_elements );

        // remove the last elements from heap so that heap has not more elements
        // than from.heap
        while ( __nb_elements > from.__nb_elements ) {
          delete __heap[--__nb_elements];
          __heap.pop_back();
        }

        // change all the current elements of heap by those in from
        Size i;
        for ( i = 0; i < __nb_elements; ++i )
          *__heap[i] = *from.__heap[i];

        // create the elements that are lacking
        for ( ; __nb_elements < from.__nb_elements; ++__nb_elements )
          __heap[__nb_elements] = new Val( *from.__heap[__nb_elements] );
      } catch ( ... ) {
        for ( Size i = 0; i < __nb_elements; ++i )
          delete __heap[i];

        __nb_elements = 0;

        throw;
      }
    }

    return *this;
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template <typename Val, typename Cmp>
  Heap<Val,Cmp>::~Heap() {
    // for debugging purposes
    GUM_DESTRUCTOR( Heap );

    // remove the pointers from the heap
    for ( Size i = 0; i < __nb_elements; ++i )
      delete __heap[i];
  }

  // ==============================================================================
  /// returns the element at the top of the heap
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  const Val& Heap<Val,Cmp>::top() const  {
    if ( !__nb_elements ) {
      GUM_ERROR( NotFound, "empty heap" );
    }

    return *__heap[0];
  }

  // ==============================================================================
  /// returns the number of elements in the heap
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  Size Heap<Val,Cmp>::size() const  {
    return __nb_elements;
  }

  // ==============================================================================
  /// return the size of the array storing the heap
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  Size Heap<Val,Cmp>::capacity() const  {
    return __heap.size();
  }

  // ==============================================================================
  /// changes the size of the array storing the heap
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  void Heap<Val,Cmp>::resize( Size new_size ) {
    if ( new_size < __nb_elements ) return;

    __heap.resize( new_size );
  }

  // ==============================================================================
  /// removes the element at position 'index' from the heap
  // ==============================================================================
  template <typename Val, typename Cmp>
  void Heap<Val,Cmp>::erase( Size index ) {
    if ( index >= __nb_elements ) return;

    // remove the element and put the last element in its place
    Val* last = __heap[__nb_elements - 1];
    delete __heap[index];

    __heap.pop_back();
    --__nb_elements;

    if ( !__nb_elements || ( index == __nb_elements ) ) return;

    // restore the heap property
    Size i = index;
    for ( Size j= ( index << 1 ) + 1; j < __nb_elements;
          i = j, j = ( j << 1 ) + 1 ) {
      // let j be the max child
      if (( j+1 < __nb_elements ) && __cmp( *__heap[j+1],*__heap[j] ) ) ++j;

      // if "last" is smaller than __heap[j], "last" must be stored at index i
      if ( __cmp( *last,*__heap[j] ) ) break;
      __heap[i] = __heap[j];
    }

    __heap[i] = last;
  }

  // ==============================================================================
  /// removes a given element from the heap (but does not return it)
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  void Heap<Val,Cmp>::eraseByVal( const Val& val ) {
    // find val in the heap
    for ( Size i = 0; i < __nb_elements; ++i )
      if ( *__heap[i] == val ) {
        erase( i );
        break;
      }
  }

  // ==============================================================================
  /// removes the top of the heap (but does not return it)
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  void Heap<Val,Cmp>::eraseTop() {
    // if the heap is empty, do nothing
    if ( !__nb_elements ) return;

    erase( 0 );
  }

  // ==============================================================================
  /// removes the top element from the heap and return it
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  Val Heap<Val,Cmp>::pop() {
    if ( !__nb_elements ) {
      GUM_ERROR( NotFound, "empty heap" );
    }

    Val v = *__heap[0];

    erase( 0 );

    return v;
  }

  // ==============================================================================
  /// inserts a new (a copy) element in the heap
  // ==============================================================================
  template <typename Val, typename Cmp>
  Size Heap<Val,Cmp>::insert( const Val& val ) {
    // create a new element at the end of the heap
    Val* bucket = new Val( val );
    __heap.push_back( bucket );
    ++__nb_elements;

    // restore the heap property
    Size i = __nb_elements - 1;
    for ( Size j = ( i-1 ) >> 1; i && __cmp( val,*__heap[j] );
          i=j, j = ( j-1 ) >> 1 )
      __heap[i] = __heap[j];

    __heap[i] = bucket;

    return i;
  }

  // ==============================================================================
  /// indicates whether the heap is empty
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  bool Heap<Val,Cmp>::empty() const  {
    return ( __nb_elements == 0 );
  }

  // ==============================================================================
  /// indicates whether the heap contains a given value
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  bool Heap<Val,Cmp>::contains( const Val& val ) const  {
    for ( Size i = 0; i < __nb_elements; ++i )
      if ( *__heap[i] == val )
        return true;

    return false;
  }

  // ==============================================================================
  /// returns the element at index elt from the heap
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  const Val& Heap<Val,Cmp>::operator[]( Size index ) const  {
    // check if the element exists
    if ( index >= __nb_elements ) {
      GUM_ERROR( NotFound, "not enough elements in the heap" );
    }

    return *__heap[index];
  }

  // ==============================================================================
  /// displays the content of the heap
  // ==============================================================================
  template <typename Val, typename Cmp>
  std::string Heap<Val,Cmp>::toString() const {
    bool deja = false;
    std::stringstream stream;
    stream << "[";

    for ( Size i = 0; i != __nb_elements; ++i, deja=true ) {
      if ( deja ) stream << " , ";

      stream << *__heap[i];
    }

    stream << "]";

    return stream.str();
  }


  // ==============================================================================
  /// A \c << operator for Heap
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  std::ostream& operator<< ( std::ostream& stream, const Heap<Val,Cmp>& heap ) {
    stream << heap.toString();
    return stream;
  }


} /* namespace gum */


#endif  // DOXYGEN_SHOULD_SKIP_THIS

