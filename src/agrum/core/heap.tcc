/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                                  *
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


namespace gum {


  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                        GUM_HEAP IMPLEMENTATION                       === */
  /* ============================================================================ */
  /* ============================================================================ */

  // ==============================================================================
  /// basic constructor. Creates an empty heap
  // ==============================================================================
  template <typename Val, typename Cmp>
  Heap<Val,Cmp>::Heap( Cmp compare, unsigned int size ) :
    heap( size ), nb_elements( 0 ), cmp( compare ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Heap );
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  template <typename Val, typename Cmp>
  Heap<Val,Cmp>::Heap( const Heap<Val,Cmp>& from ) :
    heap( from.heap.size() ), nb_elements( 0 ), cmp( from.cmp ) {
    // for debugging purposes
    GUM_CONS_CPY( Heap );
    // copy the elements of from.heap
    unsigned int i;

    try {
      for ( i = 0; i < from.nb_elements; ++i )
        heap[i] = new Val( *from.heap[i] );
    }

    // in case all the allocations could not be performed
    catch ( ... ) {
      for ( unsigned int j = 0; j < i; ++j )
        delete heap[j];

      throw;
    }

    nb_elements = from.nb_elements;
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
      // set the comprison function
      cmp = from.cmp;

      try {
        // if need be, resize the current heap
        if ( heap.size() < from.nb_elements )
          heap.resize( from.nb_elements );

        // remove the last elements from heap so that heap has not more elements
        // than from.heap
        while ( nb_elements > from.nb_elements ) {
          delete heap[--nb_elements];
          heap.pop_back();
        }

        // change all the current elements of heap by those in from
        unsigned int i;

        for ( i = 0; i < nb_elements; ++i )
          *heap[i] = *from.heap[i];

        // create the elements that are lacking
        for ( ; nb_elements < from.nb_elements; ++nb_elements )
          heap[nb_elements] = new Val( *from.heap[nb_elements] );
      } catch ( ... ) {
        for ( unsigned int i = 0; i < nb_elements; ++i )
          delete heap[i];

        nb_elements = 0;

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

    for ( unsigned int i = 0; i < nb_elements; ++i )
      delete heap[i];
  }

  // ==============================================================================
  /// returns the element at the top of the heap
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  const Val& Heap<Val,Cmp>::top() const  {
    if ( !nb_elements )
      GUM_ERROR( NotFound, "empty heap" );

    return *heap[0];
  }

  // ==============================================================================
  /// returns the number of elements in the heap
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  Size Heap<Val,Cmp>::size() const  {
    return nb_elements;
  }

  // ==============================================================================
  /// return the size of the array storing the heap
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  unsigned int Heap<Val,Cmp>::capacity() const  {
    return heap.size();
  }

  // ==============================================================================
  /// changes the size of the array storing the heap
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  void Heap<Val,Cmp>::resize( unsigned int new_size ) {
    if ( new_size < nb_elements ) return;

    heap.resize( new_size );
  }

  // ==============================================================================
  /// removes the element at position 'index' from the heap
  // ==============================================================================
  template <typename Val, typename Cmp>
  void Heap<Val,Cmp>::erase( unsigned int index ) {
    if ( index >= nb_elements ) return;

    // remove the element and put the last element in its place
    Val* last = heap[nb_elements - 1];

    delete heap[index];

    heap.pop_back();

    --nb_elements;

    if ( !nb_elements || ( index == nb_elements ) ) return;

    // restore the heap property
    unsigned int i = index;

    for ( unsigned int j= ( index << 1 ) + 1; j < nb_elements;
          i = j, j = ( j << 1 ) + 1 ) {
      // let j be the max child
      if (( j+1 < nb_elements ) && cmp( *heap[j+1],*heap[j] ) ) ++j;

      // if "last" is smaller than heap[j], "last" must be stored at index i
      if ( cmp( *last,*heap[j] ) ) break;

      heap[i] = heap[j];
    }

    heap[i] = last;
  }

  // ==============================================================================
  /// removes a given element from the heap (but does not return it)
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  void Heap<Val,Cmp>::eraseByVal( const Val& val ) {
    // find val in the heap
    for ( unsigned int i = 0; i < nb_elements; ++i )
      if ( *heap[i] == val ) {
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
    if ( !nb_elements ) return;

    erase( 0 );
  }

  // ==============================================================================
  /// removes the top element from the heap and return it
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  Val Heap<Val,Cmp>::pop() {
    if ( !nb_elements )
      GUM_ERROR( NotFound, "empty heap" );

    Val v = *heap[0];

    erase( 0 );

    return v;
  }

  // ==============================================================================
  /// inserts a new (a copy) element in the heap
  // ==============================================================================
  template <typename Val, typename Cmp>
  unsigned int Heap<Val,Cmp>::insert( const Val& val ) {
    // create a new element at the end of the heap
    Val* bucket = new Val( val );
    heap.push_back( bucket );
    ++nb_elements;
    // restore the heap property
    unsigned int i = nb_elements - 1;

    for ( unsigned int j = ( i-1 ) >> 1; i && cmp( val,*heap[j] );
          i=j, j = ( j-1 ) >> 1 )
      heap[i] = heap[j];

    heap[i] = bucket;

    return i;
  }

  // ==============================================================================
  /// indicates whether the heap is empty
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  bool Heap<Val,Cmp>::empty() const  {
    return ( nb_elements == 0 );
  }

  // ==============================================================================
  /// indicates whether the heap contains a given value
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  bool Heap<Val,Cmp>::exists( const Val& val ) const  {
    for ( unsigned int i = 0; i < nb_elements; ++i )
      if ( *heap[i] == val )
        return true;

    return false;
  }

  // ==============================================================================
  /// returns the element at index elt from the heap
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  const Val& Heap<Val,Cmp>::operator[]( unsigned int index )
    const  {
    // check if the element exists
    if ( index >= nb_elements )
      GUM_ERROR( NotFound, "not enough elements in the heap" );

    return *heap[index];
  }

  // ==============================================================================
  /// displays the content of the heap
  // ==============================================================================
  template <typename Val, typename Cmp>
  const std::string Heap<Val,Cmp>::toString() const {
    bool deja = false;
    std::stringstream stream;
    stream << "[";

    for ( unsigned int i = 0; i != nb_elements; ++i, deja=true ) {
      if ( deja ) stream << " , ";

      stream << *heap[i];
    }

    stream << "]";

    return stream.str();
  }




  
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                   GUM_PRIORITY_QUEUE IMPLEMENTATION                  === */
  /* ============================================================================ */
  /* ============================================================================ */

  // ==============================================================================
  /// basic constructor
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  PriorityQueue<Val,Priority,Cmp>::PriorityQueue
  ( Cmp compare, bool uniqueness, unsigned int size ) :
    heap( size ), indices( size, true, uniqueness ),
    nb_elements( 0 ), cmp( compare ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( PriorityQueue );
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  PriorityQueue<Val,Priority,Cmp>::PriorityQueue
  ( const PriorityQueue<Val,Priority,Cmp>& from ) :
    heap( from.heap.size() ), indices( from.indices ),
    nb_elements( from.nb_elements ), cmp( from.cmp ) {
    // for debugging purposes
    GUM_CONS_CPY( PriorityQueue );
    // fill the heap structure
    HashTableIterator< Val,std::vector<int> > iter;
    typename std::vector<int>::iterator iter_index;

    try {
      for ( iter = indices.begin(); iter != indices.end(); ++iter ) {
        for ( iter_index = iter->begin(); iter_index != iter->end(); ++iter_index ) {
          const std::pair<Priority,Val*>& from_pair = *from.heap[*iter_index];
          heap[*iter_index] =
            new std::pair<Priority,Val*> ( from_pair.first,
                                           const_cast<Val*>( &iter.key() ) );
        }
      }
    }

    // if there was a problem, deallocate everything
    catch ( ... ) {
      for ( HashTableIterator< Val,std::vector<int> > iter2 = indices.begin();
            iter2!=iter; ++iter2 )
        for ( typename std::vector<int>::iterator iter_index2 = iter2->begin();
              iter_index2 != iter2->end(); ++iter_index2 )
          delete heap[*iter_index2];

      for ( typename std::vector<int>::iterator iter_index2 = iter->begin();
            iter_index2 != iter_index; ++iter_index2 )
        delete heap[*iter_index2];

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
      cmp = from.cmp;

      try {
        // if need be, resize the current heap
        if ( heap.size() < from.nb_elements )
          heap.resize( from.nb_elements );

        // assign the pairs (val/indices)
        indices = from.indices;
      } catch ( ... ) {
        for ( unsigned int i = 0; i < nb_elements; ++i )
          delete heap[i];

        heap.clear();

        indices.clear();

        nb_elements = 0;

        throw;
      }

      // remove the last elements from heap so that heap has not more elements
      // than from.heap
      while ( nb_elements > from.nb_elements ) {
        delete heap[--nb_elements];
        heap.pop_back();
      }

      // fill properly the heap structure
      HashTableIterator< Val,std::vector<int> > iter;

      typename std::vector<int>::iterator iter_index;

      try {
        for ( iter = indices.begin(); iter != indices.end(); ++iter )
          for ( iter_index = iter->begin(); iter_index != iter->end();
                ++iter_index ) {
            const std::pair<Priority, Val*>& from_pair = *from.heap[*iter_index];

            if (( unsigned int ) *iter_index < nb_elements ) {
              heap[*iter_index]->first = from_pair.first;
              heap[*iter_index]->second = const_cast<Val*>( &iter.key() );
            } else
              heap[*iter_index] = new std::pair<Priority, Val*>
                ( from_pair.first, const_cast<Val*>( &iter.key() ) );
          }

        nb_elements = from.nb_elements;
      } catch ( ... ) {
        HashTableIterator< Val,std::vector<int> > iter2;
        typename std::vector<int>::iterator iter_index2;

        for ( iter2 = indices.begin(); iter2 != iter; ++iter2 )
          for ( iter_index2=iter2->begin(); iter_index2!=iter2->end();
                ++iter_index2 )
            delete heap[*iter_index2];

        for ( iter_index2 = iter->begin(); iter_index2 != iter_index; ++iter_index2 )
          delete heap[*iter_index2];

        indices.clear();

        heap.clear();

        nb_elements = 0;

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

    for ( unsigned int i = 0; i < nb_elements; ++i )
      delete heap[i];
  }

  // ==============================================================================
  /// returns the element at the top of the priority queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  const Val& PriorityQueue<Val,Priority,Cmp>::top() const {
    if ( !nb_elements )
      GUM_ERROR( NotFound, "empty priority queue" );

    return *( heap[0]->second );
  }

  // ==============================================================================
  /// returns the priority of the top element
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  const Priority& PriorityQueue<Val,Priority,Cmp>::getTopPriority() const {
    if ( !nb_elements )
      GUM_ERROR( NotFound, "empty priority queue" );

    return heap[0]->first;
  }

  // ==============================================================================
  /// returns the number of elements in the priority queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  Size PriorityQueue<Val,Priority,Cmp>::size() const  {
    return nb_elements;
  }

  // ==============================================================================
  /// return the size of the array storing the priority queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  Size PriorityQueue<Val,Priority,Cmp>::capacity() const  {
    return heap.size();
  }

  // ==============================================================================
  /// changes the size of the array storing the priority queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  void PriorityQueue<Val,Priority,Cmp>::resize( Size new_size ) {
    if ( new_size < nb_elements ) return;

    heap.resize( new_size );

    indices.resize( new_size );
  }

  // ==============================================================================
  /// removes all the elements from the queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  void PriorityQueue<Val,Priority,Cmp>::clear() {
    heap.clear();
    indices.clear();
    nb_elements = 0;
  }

  // ==============================================================================
  /// removes the element at index elt from the priority queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  void PriorityQueue<Val,Priority,Cmp>::erase( unsigned int index ) {
    if ( index >= nb_elements ) return;

    // get the element from both the heap and the hashtable
    const Val& del_val = *( heap[index]->second );

    std::vector<int>& vect_index = indices[del_val];

    // put the last element at the "index" location
    std::pair<Priority, Val*>* last = heap[nb_elements - 1];

    delete heap[index];

    heap.pop_back();

    --nb_elements;

    if ( vect_index.size() == 1 )
      indices.erase( del_val );
    else {
      unsigned int i;

      for ( i = 0; i != vect_index.size(); ++i )
        if ( vect_index[i] == ( int ) index ) break;

      vect_index[i] = vect_index[vect_index.size() - 1];

      vect_index.pop_back();
    }

    if ( !nb_elements || ( index == nb_elements ) )
      return;

    // restore the heap property
    unsigned int i = index;

    for ( unsigned int j = ( index << 1 ) + 1; j<nb_elements;
          i = j, j = ( j << 1 ) + 1 ) {
      // let j be the max child
      if (( j+1 < nb_elements ) && cmp( heap[j+1]->first,heap[j]->first ) ) ++j;

      // if "last" is lower than heap[j], "last" must be stored at index i
      if ( cmp( last->first,heap[j]->first ) ) break;

      // else pull up the jth node
      heap[i] = heap[j];

      std::vector<int>& vect_index = indices[*( heap[i]->second )];

      int jj = j, ii = i;

      for ( unsigned int k = 0; k != vect_index.size(); ++k )
        if ( vect_index[k] == jj ) {
          vect_index[k] = ii;
          break;
        }
    }

    // put "last" back into the heap
    heap[i] = last;

    int ii = i, nb = nb_elements;

    std::vector<int>& vect_index_i = indices[*( heap[i]->second )];

    for ( unsigned int k = 0; k != vect_index_i.size(); ++k )
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
    try { erase( indices[val][0] ); }
    catch ( NotFound& ) { }
  }

  // ==============================================================================
  /// removes the top of the priority queue (but does not return it)
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  void PriorityQueue<Val,Priority,Cmp>::eraseTop() {
    // if the heap is empty, do nothing
    if ( !nb_elements ) return;

    erase( 0 );
  }

  // ==============================================================================
  /// removes the top element from the priority queue and return it
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  Val PriorityQueue<Val,Priority,Cmp>::pop() {
    if ( !nb_elements )
      GUM_ERROR( NotFound, "empty priority queue" );

    Val v = *( heap[0]->second );

    erase( 0 );

    return v;
  }

  // ==============================================================================
  /// returns a hashtable the keys of which are the values stored in the queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  const HashTable< Id,std::vector<int> >&
  PriorityQueue<Val,Priority,Cmp>::getAllValues() const  {
    return indices;
  }

  // ==============================================================================
  /// inserts a new (a copy) element in the priority queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  unsigned int PriorityQueue<Val,Priority,Cmp>::insert( const Priority& priority,
                                                        const Val& val ) {
    // in uniqueness mode, check that val does not already exist
    if ( indices.keyUniquenessPolicy() && indices.exists( val ) )
      GUM_ERROR( DuplicateElement, "val already exists in the priority queue" );

    // create the entry in the indices hashtable
    if ( !indices.exists( val ) )
      indices.insert( val, std::vector<int>() );

    const Val& indval = indices.key( val );

    // create a new element at the end of the heap
    std::pair<Priority, Val*> *bucket;

    try {
      bucket = new std::pair<Priority, Val*> ( priority,
                                               const_cast<Val*>( &indval ) );
    } catch ( ... ) {
      std::vector<int>& vect_index = indices[val];

      if ( vect_index.size() == 0 )
        indices.erase( val );

      throw;
    }

    heap.push_back( bucket );

    ++nb_elements;
    // restore the heap property
    unsigned int i = nb_elements - 1;

    for ( int j = ( i-1 ) >> 1; i && cmp( priority,heap[j]->first );
          i=j, j = ( j-1 ) >> 1 ) {
      heap[i] = heap[j];
      std::vector<int>& vect_index = indices[*( heap[i]->second )];

      for ( unsigned int k = 0; k != vect_index.size(); ++k )
        if ( vect_index[k] == j ) {
          vect_index[k] = i;
          break;
        }
    }

    // put the new bucket into the heap
    heap[i] = bucket;

    indices[val].push_back( i );

    return i;
  }

  // ==============================================================================
  /// indicates whether the priority queue is empty
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  bool PriorityQueue<Val,Priority,Cmp>::empty() const  {
    return ( nb_elements == 0 );
  }

  // ==============================================================================
  /// indicates whether the priority queue contains a given value
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  bool PriorityQueue<Val,Priority,Cmp>::exists( const Val& val ) const  {
    return indices.exists( val );
  }

  // ==============================================================================
  /// returns the element at position "index" in the priority queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  const Val& PriorityQueue<Val,Priority,Cmp>::operator[]( unsigned int index )
    const  {
    return *( heap[index]->second );
  }

  // ==============================================================================
  /// displays the content of the queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  const std::string PriorityQueue<Val,Priority,Cmp>::toString()
    const {
    bool deja = false;
    std::stringstream stream;
    stream << "[";

    for ( unsigned int i = 0; i != nb_elements; ++i, deja=true ) {
      if ( deja ) stream << " , ";

      stream << "(" << heap[i]->first << " , " << *( heap[i]->second ) << ")";
    }

    stream << "]";

    return stream.str();
  }

  // ==============================================================================
  /// changes the size of the internal structure storing the priority queue
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  void PriorityQueue<Val,Priority,Cmp>::setPriority( unsigned int index,
                                                     const Priority& new_priority ) {
    // check whether the element the priority of which should be changed exists
    if ( index >= nb_elements ) return;

    // get the element itself
    std::pair<Priority, Val*>* val = heap[index];

    val->first = new_priority;

    // restore the heap property
    int i, j, new_index = index;

    // move val upward if needed
    for ( i = ( int ) index, j = ( i-1 ) >> 1;
          i && cmp( new_priority, heap[j]->first );
          new_index = j, i = j, j = ( j-1 ) >> 1 ) {
      heap[i] = heap[j];
      std::vector<int>& vect_index = indices[*( heap[i]->second )];

      for ( unsigned int k = 0; k != vect_index.size(); ++k ) {
        if ( vect_index[k] == j ) {
          vect_index[k] = i;
          break;
        }
      }
    }

    // move val downward if needed
    for ( j = ( i << 1 ) + 1; j < ( int ) nb_elements;
          new_index = j, i = j, j = ( j << 1 ) + 1 ) {
      // let j be the max child
      if (( j+1 < ( int ) nb_elements ) &&
          cmp( heap[j+1]->first,heap[j]->first ) ) ++j;

      // if "val" is lower than heap[j], "val" must be stored at index i
      if ( cmp( new_priority,heap[j]->first ) ) break;

      // else pull up the jth node
      heap[i] = heap[j];

      std::vector<int>& vect_index = indices[*( heap[i]->second )];

      int jj = j, ii = i;

      for ( unsigned int k = 0; k != vect_index.size(); ++k ) {
        if ( vect_index[k] == jj ) {
          vect_index[k] = ii;
          break;
        }
      }
    }

    // update the index of val
    if ( index != ( unsigned int ) new_index ) {
      heap[new_index] = val;
      std::vector<int>& vect_index = indices[*( val->second )];

      for ( unsigned int k = 0; k != vect_index.size(); ++k ) {
        if ( vect_index[k] == ( int ) index ) {
          vect_index[k] = new_index;
          break;
        }
      }
    }
  }

  // ==============================================================================
  /// modifies the priority of each instance of a given element
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp>
  void PriorityQueue<Val,Priority,Cmp>::setPriorityByVal
  ( const Val& elt, const Priority& new_priority ) {
    try {
      std::vector<int>& vect_index = indices[elt];

      for ( unsigned int i = 0; i < vect_index.size(); ++i )
        setPriority( vect_index[i], new_priority );
    } catch ( ... ) { }
  }

  // ==============================================================================
  /// modifies the priority of each instance of a given element
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  const Priority&
  PriorityQueue<Val,Priority,Cmp>::getPriorityByVal( const Val& elt ) const {
    return heap[indices[elt][0]]->first;
  }

  // ==============================================================================
  /// returns the current uniqueness policy
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  bool PriorityQueue<Val,Priority,Cmp>::getUniquenessPolicy() const  {
    return indices.keyUniquenessPolicy();
  }

  // ==============================================================================
  /** @brief enables the user to change dynamically the policy for checking
   * whether there can exist several identical elements in the queue */
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  void PriorityQueue<Val,Priority,Cmp>::setUniquenessPolicy
  ( const bool new_policy )  {
    indices.setKeyUniquenessPolicy( new_policy );
  }
  
  // ==============================================================================
  /// A \c << operator for Heap
  // ==============================================================================
  template <typename Val, typename Cmp> INLINE
  std::ostream& operator<< ( std::ostream& stream, const Heap<Val,Cmp>& heap ) {
    return heap.print( stream );
  }

  // ==============================================================================
  /// A \c << operator for priority queues
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> INLINE
  std::ostream& operator<< ( std::ostream& stream,
                             const  PriorityQueue<Val,Priority,Cmp>& heap ) {
    return heap.print( stream );
  }


} /* namespace gum */


#endif  // DOXYGEN_SHOULD_SKIP_THIS

