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
 * @brief template implementations of priority queues
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <string>
#include <sstream>
#include "priorityQueue.h"


#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {


  // ==============================================================================
  /// basic constructor
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  PriorityQueue<Val,Priority,Cmp>::PriorityQueue ( Cmp compare,
                                                   bool uniqueness,
                                                   Size capacity ) :
    __heap( capacity ), __indices( capacity >> 1, true, uniqueness ),
    __nb_elements( 0 ), __cmp( compare ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( PriorityQueue );
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  PriorityQueue<Val,Priority,Cmp>::PriorityQueue
  ( const PriorityQueue<Val,Priority,Cmp>& from ) :
    __heap( from.__heap.size() ),
    __indices( from.__indices ),
    __nb_elements( from.__nb_elements ),
    __cmp( from.__cmp ) {
    // for debugging purposes
    GUM_CONS_CPY( PriorityQueue );

    // fill the heap structure
    HashTableConstIterator< Val,std::vector<Size> > iter;
    typename std::vector<Size>::const_iterator iter_index;

    try {
      for ( iter = __indices.begin(); iter != __indices.end(); ++iter ) {
        for ( iter_index = iter->begin(); iter_index != iter->end();
              ++iter_index ) {
          const std::pair<Priority,Val*>& from_pair = *from.__heap[*iter_index];
          __heap[*iter_index] =
            new std::pair<Priority,Val*> ( from_pair.first,
                                           const_cast<Val*>( &iter.key() ) );
        }
      }
    }
    catch ( ... ) {  // if there was a problem, deallocate everything
      for ( HashTableConstIterator< Val,std::vector<Size> >
              iter2 = __indices.begin(); iter2!=iter; ++iter2 )
        for ( typename std::vector<Size>::const_iterator
                iter_index2 = iter2->begin();
              iter_index2 != iter2->end(); ++iter_index2 )
          delete __heap[*iter_index2];

      for ( typename std::vector<Size>::const_iterator iter_index2 = iter->begin();
            iter_index2 != iter_index; ++iter_index2 )
        delete __heap[*iter_index2];

      throw;
    }
  }

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  PriorityQueue<Val,Priority,Cmp>& PriorityQueue<Val,Priority,Cmp>::operator=
  ( const PriorityQueue<Val,Priority,Cmp>& from ) {
    // avoid self assignment
    if ( this != &from ) {
      // for debugging purposes
      GUM_OP_CPY( PriorityQueue );

      // set the comprison function
      __cmp = from.__cmp;

      try {
        // if need be, resize the current heap
        if ( __heap.size() < from.__nb_elements )
          __heap.resize( from.__nb_elements );

        // reassign the pairs (val/indices)
        __indices = from.__indices;
      }
      catch ( ... ) {
        for ( Size i = 0; i < __nb_elements; ++i )
          delete __heap[i];

        __heap.clear();
        __indices.clear();
        __nb_elements = 0;

        throw;
      }

      // remove the last elements from heap so that heap has not more elements
      // than from.heap
      while ( __nb_elements > from.__nb_elements ) {
        delete __heap[--__nb_elements];
        __heap.pop_back();
      }

      // fill properly the heap structure
      HashTableConstIterator< Val,std::vector<Size> > iter;
      typename std::vector<Size>::const_iterator iter_index;

      try {
        for ( iter = __indices.begin(); iter != __indices.end(); ++iter )
          for ( iter_index = iter->begin(); iter_index != iter->end();
                ++iter_index ) {
            const std::pair<Priority, Val*>& from_pair = *from.__heap[*iter_index];

            if ( *iter_index < __nb_elements ) {
              __heap[*iter_index]->first = from_pair.first;
              __heap[*iter_index]->second = const_cast<Val*>( &iter.key() );
            }
            else
              __heap[*iter_index] = new std::pair<Priority, Val*>
                ( from_pair.first, const_cast<Val*>( &iter.key() ) );
          }

        __nb_elements = from.__nb_elements;
      }
      catch ( ... ) { // in case something went wrong, clear the queue
        HashTableConstIterator< Val,std::vector<Size> > iter2;
        typename std::vector<Size>::const_iterator iter_index2;

        for ( iter2 = __indices.begin(); iter2 != iter; ++iter2 ) {
          for ( iter_index2=iter2->begin(); iter_index2!=iter2->end();
                ++iter_index2 ) {
            delete __heap[*iter_index2];
            __heap[*iter_index2] = 0;
          }
        }

        for ( iter_index2 = iter->begin(); iter_index2 != iter_index;
              ++iter_index2 ) {
          delete __heap[*iter_index2];
          __heap[*iter_index2] = 0;
        }

        for ( Size i = 0; i < __nb_elements; ++i )
          if (__heap[i]) delete __heap[i];

        __indices.clear();
        __heap.clear();
        __nb_elements = 0;

        throw;
      }
    }

    return *this;
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  PriorityQueue<Val,Priority,Cmp>::~PriorityQueue() {
    // for debugging purposes
    GUM_DESTRUCTOR( PriorityQueue );

    // remove the pointers from the heap
    for ( Size i = 0; i < __nb_elements; ++i )
      delete __heap[i];
  }

  // ==============================================================================
  /// returns the element at the top of the priority queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  const Val& PriorityQueue<Val,Priority,Cmp>::top() const {
    if ( !__nb_elements ) {
      GUM_ERROR( NotFound, "empty priority queue" );
    }

    return *( __heap[0]->second );
  }

  // ==============================================================================
  /// returns the priority of the top element
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  const Priority& PriorityQueue<Val,Priority,Cmp>::topPriority() const {
    if ( !__nb_elements ) {
      GUM_ERROR( NotFound, "empty priority queue" );
    }

    return __heap[0]->first;
  }

  // ==============================================================================
  /// returns the number of elements in the priority queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  Size PriorityQueue<Val,Priority,Cmp>::size() const  {
    return __nb_elements;
  }

  // ==============================================================================
  /// return the size of the array storing the priority queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  Size PriorityQueue<Val,Priority,Cmp>::capacity() const  {
    return __heap.size();
  }

  // ==============================================================================
  /// changes the size of the array storing the priority queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  void PriorityQueue<Val,Priority,Cmp>::resize( Size new_size ) {
    if ( new_size < __nb_elements ) return;

    __heap.resize( new_size );
    __indices.resize( new_size / 2);
  }

  // ==============================================================================
  /// removes all the elements from the queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  void PriorityQueue<Val,Priority,Cmp>::clear() {
    // remove the pointers from the heap
    for ( Size i = 0; i < __nb_elements; ++i )
      delete __heap[i];

    __nb_elements = 0;
    __heap.clear();
    __indices.clear();
  }

  // ==============================================================================
  /// removes the element at index elt from the priority queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  void PriorityQueue<Val,Priority,Cmp>::erase( Size index ) {
    if ( index >= __nb_elements ) return;

    // get the element from both the heap and the hashtable
    const Val& del_val = *( __heap[index]->second );
    std::vector<Size>& vect_index = __indices[del_val];

    // put the last element at the "index" location
    std::pair<Priority, Val*>* last = __heap[__nb_elements - 1];
    delete __heap[index];
    __heap.pop_back();
    --__nb_elements;

    if ( vect_index.size() == 1 )
      __indices.erase( del_val );
    else {
      Size i;
      for ( i = 0; i != vect_index.size(); ++i )
        if ( vect_index[i] == index ) break;

      vect_index[i] = vect_index[vect_index.size() - 1];
      vect_index.pop_back();
    }

    if ( !__nb_elements || ( index == __nb_elements ) )
      return;

    // restore the heap property
    Size i = index;

    for ( Size j = ( index << 1 ) + 1; j < __nb_elements;
          i = j, j = ( j << 1 ) + 1 ) {
      // let j be the max child
      if (( j+1 < __nb_elements ) &&
          __cmp( __heap[j+1]->first,__heap[j]->first ) ) ++j;

      // if "last" is lower than heap[j], "last" must be stored at index i
      if ( __cmp( last->first,__heap[j]->first ) ) break;

      // else pull up the jth node
      __heap[i] = __heap[j];

      std::vector<Size>& vect_index = __indices[*( __heap[i]->second )];

      Size jj = j, ii = i;
      for ( Size k = 0; k != vect_index.size(); ++k )
        if ( vect_index[k] == jj ) {
          vect_index[k] = ii;
          break;
        }
    }

    // put "last" back into the heap
    __heap[i] = last;

    Size ii = i, nb = __nb_elements;
    std::vector<Size>& vect_index_i = __indices[*( __heap[i]->second )];

    for ( Size k = 0; k != vect_index_i.size(); ++k )
      if ( vect_index_i[k] == nb ) {
        vect_index_i[k] = ii;
        break;
      }
  }

  // ==============================================================================
  /// removes a given element from the priority queue (but does not return it)
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  void PriorityQueue<Val,Priority,Cmp>::eraseByVal( const Val& val ) {
    try { erase( __indices[val][0] ); }
    catch ( NotFound& ) { }
  }

  // ==============================================================================
  /// removes the top of the priority queue (but does not return it)
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  void PriorityQueue<Val,Priority,Cmp>::eraseTop() {
    // if the heap is empty, do nothing
    if ( !__nb_elements ) return;

    erase( 0 );
  }

  // ==============================================================================
  /// removes the top element from the priority queue and return it
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  Val PriorityQueue<Val,Priority,Cmp>::pop() {
    if ( !__nb_elements ) {
      GUM_ERROR( NotFound, "empty priority queue" );
    }

    Val v = *( __heap[0]->second );

    erase( 0 );

    return v;
  }

  // ==============================================================================
  /// returns a hashtable the keys of which are the values stored in the queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  const HashTable< Val,std::vector<Size> >&
  PriorityQueue<Val,Priority,Cmp>::allValues() const  {
    return __indices;
  }

  // ==============================================================================
  /// inserts a new (a copy) element in the priority queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  Size PriorityQueue<Val,Priority,Cmp>::insert( const Priority& priority,
                                                const Val& val ) {
    // check whether val already exists in the queue
    bool val_exists = __indices.exists( val );

    // in uniqueness mode, check that val does not already exist
    if ( __indices.keyUniquenessPolicy() && val_exists ) {
      GUM_ERROR( DuplicateElement, "val already exists in the priority queue" );
    }

    // create the entry in the indices hashtable
    if ( ! val_exists )
      __indices.insert( val, std::vector<Size>() );

    const Val& indval = __indices.key( val );

    // create a new element at the end of the heap
    std::pair<Priority, Val*> *bucket;

    try {
      bucket = new std::pair<Priority, Val*> ( priority,
                                               const_cast<Val*>( &indval ) );
    }
    catch ( ... ) {
      std::vector<Size>& vect_index = __indices[val];

      if ( vect_index.size() == 0 )
        __indices.erase( val );

      throw;
    }

    __heap.push_back( bucket );
    ++__nb_elements;

    // restore the heap property
    Size i = __nb_elements - 1;
    for ( Size j = ( i-1 ) >> 1; i && __cmp( priority,__heap[j]->first );
          i=j, j = ( j-1 ) >> 1 ) {
      __heap[i] = __heap[j];
      std::vector<Size>& vect_index = __indices[*( __heap[i]->second )];

      for ( Size k = 0; k != vect_index.size(); ++k )
        if ( vect_index[k] == j ) {
          vect_index[k] = i;
          break;
        }
    }

    // put the new bucket into the heap
    __heap[i] = bucket;
    __indices[val].push_back( i );

    return i;
  }

  // ==============================================================================
  /// indicates whether the priority queue is empty
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  bool PriorityQueue<Val,Priority,Cmp>::empty() const  {
    return ( __nb_elements == 0 );
  }

  // ==============================================================================
  /// indicates whether the priority queue contains a given value
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  bool PriorityQueue<Val,Priority,Cmp>::contains( const Val& val ) const  {
    return __indices.exists( val );
  }

  // ==============================================================================
  /// returns the element at position "index" in the priority queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  const Val& PriorityQueue<Val,Priority,Cmp>::operator[]( Size index )
    const  {
    if ( index >= __nb_elements ) {
      GUM_ERROR( NotFound, "not enough elements in the PriorityQueue" );
    }
    return *( __heap[index]->second );
  }

  // ==============================================================================
  /// displays the content of the queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  std::string PriorityQueue<Val,Priority,Cmp>::toString() const {
    bool deja = false;
    std::stringstream stream;
    stream << "[";

    for ( Size i = 0; i != __nb_elements; ++i, deja=true ) {
      if ( deja ) stream << " , ";

      stream << "(" << __heap[i]->first << " , " << *( __heap[i]->second ) << ")";
    }

    stream << "]";

    return stream.str();
  }

  // ==============================================================================
  /// changes the size of the internal structure storing the priority queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  Size
  PriorityQueue<Val,Priority,Cmp>::setPriority( Size index,
                                                const Priority& new_priority ) {
    // check whether the element the priority of which should be changed exists
    if ( index >= __nb_elements ) {
      GUM_ERROR( NotFound, "not enough elements in the PriorityQueue" );
    }

    // get the element itself
    std::pair<Priority, Val*>* val = __heap[index];

    val->first = new_priority;

    // restore the heap property
    Size i, j, new_index = index;

    // move val upward if needed
    for ( i = index, j = ( i-1 ) >> 1;
          i && __cmp( new_priority, __heap[j]->first );
          new_index = j, i = j, j = ( j-1 ) >> 1 ) {
      __heap[i] = __heap[j];
      std::vector<Size>& vect_index = __indices[*( __heap[i]->second )];

      for ( Size k = 0; k != vect_index.size(); ++k ) {
        if ( vect_index[k] == j ) {
          vect_index[k] = i;
          break;
        }
      }
    }

    // move val downward if needed
    for ( j = ( i << 1 ) + 1; j < __nb_elements;
          new_index = j, i = j, j = ( j << 1 ) + 1 ) {
      // let j be the max child
      if (( j+1 < __nb_elements ) &&
          __cmp( __heap[j+1]->first,__heap[j]->first ) ) ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if ( __cmp( new_priority,__heap[j]->first ) ) break;

      // else pull up the jth node
      __heap[i] = __heap[j];

      std::vector<Size>& vect_index = __indices[*( __heap[i]->second )];

      Size jj = j, ii = i;
      for ( Size k = 0; k != vect_index.size(); ++k ) {
        if ( vect_index[k] == jj ) {
          vect_index[k] = ii;
          break;
        }
      }
    }

    // update the index of val
    if ( index != new_index ) {
      __heap[new_index] = val;
      std::vector<Size>& vect_index = __indices[*( val->second )];

      for ( Size k = 0; k != vect_index.size(); ++k ) {
        if ( vect_index[k] == index ) {
          vect_index[k] = new_index;
          break;
        }
      }
    }

    return new_index;
  }

  // ==============================================================================
  /// modifies the priority of each instance of a given element
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  void PriorityQueue<Val,Priority,Cmp>::setPriorityByVal
  ( const Val& elt, const Priority& new_priority ) {
    try {
      std::vector<Size>& vect_index = __indices[elt];

      for ( Size i = 0; i < vect_index.size(); ++i )
        setPriority( vect_index[i], new_priority );
    }
    catch ( ... ) { }
  }

  // ==============================================================================
  /// modifies the priority of each instance of a given element
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  const Priority&
  PriorityQueue<Val,Priority,Cmp>::priorityByVal( const Val& elt ) const {
    return __heap[__indices[elt][0]]->first;
  }

  // ==============================================================================
  /// returns the current uniqueness policy
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  bool PriorityQueue<Val,Priority,Cmp>::uniquenessPolicy() const  {
    return __indices.keyUniquenessPolicy();
  }

  // ==============================================================================
  /** @brief enables the user to change dynamically the policy for checking
   * whether there can exist several identical elements in the queue */
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  void PriorityQueue<Val,Priority,Cmp>::setUniquenessPolicy
  ( const bool new_policy )  {
    __indices.setKeyUniquenessPolicy( new_policy );
  }

  // ==============================================================================
  /// A \c << operator for priority queues
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  std::ostream& operator<< ( std::ostream& stream,
                             const  PriorityQueue<Val,Priority,Cmp>& queue ) {
    stream << queue.toString ();
    return stream;
  }


} /* namespace gum */


#endif  // DOXYGEN_SHOULD_SKIP_THIS

