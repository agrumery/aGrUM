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
 * @brief template implementation for the gumNodeList class and its iterators
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#ifndef DOXYGEN_SHOULD_SKIP_THIS


/* ============================================================================== */
/* ============================================================================== */
/* ===                   IMPLEMENTATION OF THE GUM_NODE_LIST                  === */
/* ============================================================================== */
/* ============================================================================== */

// ================================================================================
/// basic constructor
// ================================================================================
template <typename Node>
gumNodeList<Node>::gumNodeList( gumSize size_param, bool resize_pol )
    :
    nodes( size_param,resize_pol ), iter_end( nodes,GUM_NODELIST_ITERATOR_END ),
    iter_rend( nodes,GUM_NODELIST_ITERATOR_REND ) {
  // for debugging purposes
  GUM_CONSTRUCTOR( gumNodeList );
}


// ================================================================================
/// adds a copy of \e node into the nodeList
// ================================================================================
template <typename Node> INLINE
Node& gumNodeList<Node>::insert( const Node& node ) {
  // note that the gumRefPtr below, by its very constructor, creates
  // a copy of node
  return *( nodes.insert( node.getID(), gumRefPtr<Node>( new Node( node ) ) ) );
}


// ================================================================================
/// adds a shallow copy of \e node into the nodeList
// ================================================================================
template <typename Node> INLINE
const gumRefPtr<Node>&
gumNodeList<Node>::shallowInsert( const gumRefPtr<Node>& node ) {
  // note that the gumRefPtr below, by its very constructor, creates
  // a copy of node
  return nodes.insert( node->getID(), node );
}


// ================================================================================
/// returns a pointer to a given node (allows shallow copies)
// ================================================================================
template <typename Node> INLINE
const gumRefPtr<Node>& gumNodeList<Node>::shallowGet( const gumId& id )
const  {
  return nodes[id];
}


// ================================================================================
/// returns a pointer to a given node (allows shallow copies)
// ================================================================================
template <typename Node> INLINE
const gumRefPtr<Node>& gumNodeList<Node>::shallowGet( const Node& node )
const  {
  return nodes[node.getID()];
}


// ================================================================================
/// copy constructor (either deep or shallow copy)
// ================================================================================
template <typename Node>
gumNodeList<Node>::gumNodeList( const gumNodeList<Node>& from,
                                gumCopyType copy_type )
    : nodes( 2,from.nodes.getResizePolicy() ) {
  // for debugging purposes
  GUM_CONS_CPY( gumNodeList );

  // checks whether we need to perform a shallow or a deep copy

  if ( copy_type == GUM_DEEP_COPY ) {
    nodes.resize( from.nodes.capacity() );
    // deep copy the content of from

    for ( gumHashTableIterator<gumId,gumRefPtr<Node> > iter = from.nodes.begin();
          iter != from.nodes.end(); ++iter )
      nodes.insert( iter.key(), gumRefPtr<Node>( new Node( **iter ) ) );
  } else
    nodes = from.nodes;

  // set properly end and rend
  iter_end._initialize( nodes, GUM_NODELIST_ITERATOR_END );

  iter_rend._initialize( nodes, GUM_NODELIST_ITERATOR_REND );
}


// ================================================================================
/// copy operator (deep copy)
// ================================================================================
template <typename Node>
gumNodeList<Node>& gumNodeList<Node>::operator= ( const gumNodeList<Node>& from ) {
  // avoid self assignment
  if ( this != &from ) {
    // for debugging purposes
    GUM_OP_CPY( gumNodeList );

    // remove the current content of the node list
    nodes.clear();

    // deep copy the content of from

    try {
      nodes.resize( from.nodes.capacity() );
      nodes.setResizePolicy( from.nodes.getResizePolicy() );

      for ( gumHashTableIterator<gumId,gumRefPtr<Node> > iter = from.nodes.begin();
            iter != from.nodes.end(); ++iter )
        nodes.insert( iter.key(), gumRefPtr<Node>( new Node( **iter ) ) );
    } catch ( ... ) {
      nodes.clear();
      iter_end._initialize( nodes, GUM_NODELIST_ITERATOR_END );
      iter_rend._initialize( nodes, GUM_NODELIST_ITERATOR_REND );
      throw;
    }

    // set properly end and rend
    iter_end._initialize( nodes, GUM_NODELIST_ITERATOR_END );

    iter_rend._initialize( nodes, GUM_NODELIST_ITERATOR_REND );
  }

  return *this;
}


// ================================================================================
/// destructor
// ================================================================================
template <typename Node> gumNodeList<Node>::~gumNodeList()  {
  // for debugging purposes
  GUM_DESTRUCTOR( gumNodeList );
}


// ================================================================================
/// copy operator (the new list shares its nodes with \e from)
// ================================================================================
template <typename Node>
gumNodeList<Node>& gumNodeList<Node>::shallowCopy( const gumNodeList<Node>& from ) {
  // avoid self assignment
  if ( this != &from ) {
    // fill this with the content of from
    nodes = from.nodes;

    // set properly end and rend
    iter_end._initialize( nodes, GUM_NODELIST_ITERATOR_END );
    iter_rend._initialize( nodes, GUM_NODELIST_ITERATOR_REND );
  }

  return *this;
}


// ================================================================================
/// returns a reference on the node the id of which is passed in argument
// ================================================================================
template <typename Node> INLINE
Node& gumNodeList<Node>::operator[]( const gumId& id )  {
  return *( nodes[id] );
}


// ================================================================================
/// returns a reference on the node the id of which is passed in argument
// ================================================================================
template <typename Node> INLINE
const Node& gumNodeList<Node>::operator[]( const gumId& id ) const {
  return *( nodes[id] );
}


// ================================================================================
/// returns the size of the nodes vector of the nodes hashtable
// ================================================================================
template <typename Node> INLINE
const gumSize& gumNodeList<Node>::capacity() const  {
  return nodes.size();
}


// ================================================================================
/// alias for operator []
// ================================================================================
template <typename Node> INLINE
Node& gumNodeList<Node>::get( const gumId& id )  {
  return operator[]( id );
}


// ================================================================================
/// alias for operator []
// ================================================================================
template <typename Node> INLINE
const Node& gumNodeList<Node>::get( const gumId& id ) const {
  return operator[]( id );
}


// ================================================================================
/// removes all the elements in the nodes list
// ================================================================================
template <typename Node> INLINE
void gumNodeList<Node>::clear() {
  nodes.clear();
}


// ================================================================================
/// indicates whether nodes list is empty
// ================================================================================
template <typename Node> INLINE
const bool gumNodeList<Node>::empty() const  {
  return nodes.empty();
}


// ================================================================================
/// checks whether there exists a node with a given ID in the nodes list
// ================================================================================
template <typename Node> INLINE
const bool gumNodeList<Node>::exists( const gumId& id ) const  {
  return nodes.exists( id );
}


// ================================================================================
/// enables the user to change dynamically the resizing policy.
// ================================================================================
template <typename Node> INLINE
void gumNodeList<Node>::setResizePolicy( const bool new_policy )  {
  nodes.setResizePolicy( new_policy );
}


// ================================================================================
/// returns the current resizing policy.
// ================================================================================
template <typename Node> INLINE
const bool gumNodeList<Node>::getResizePolicy() const  {
  return nodes.getResizePolicy();
}


// ================================================================================
/// changes the size of the nodes vector of the node list
// ================================================================================
template <typename Node> INLINE
void gumNodeList<Node>::resize( gumSize new_size ) {
  nodes.resize( new_size );
}


// ================================================================================
/// returns the number of nodes in the nodes list
// ================================================================================
template <typename Node> INLINE
const gumSize gumNodeList<Node>::size() const  {
  return nodes.size();
}

// ================================================================================
/// removes a node specified by its ID from the list of nodes
// ================================================================================
template <typename Node> INLINE
void gumNodeList<Node>::erase( const gumId& id ) {
  nodes.erase( id );
}


// ================================================================================
/// removes a given node from the list of nodes
// ================================================================================
template <typename Node> INLINE
void gumNodeList<Node>::erase( const Node& node ) {
  nodes.erase( node.getID() );
}


// ================================================================================
/// creates a gumNodeList of Assent from a gumNodeList of Node
// ================================================================================
template <typename Node>
template <typename Assent>
gumNodeList<Assent> gumNodeList<Node>::map( Assent( *f )( Node ),
    gumSize size ) const {
  // determine the proper size of the hashtable of the nodelist
  // by default, the size of the table is set so that the table does not take
  // too much space while allowing to add a few elements without resizing
  if ( size == 0 )
    size = std::max( 2UL, nodes.size() / 2 );

  // create a new nodelist
  gumNodeList<Assent> list( size );

  // fill the new hash table
  for ( gumHashTableIterator<gumId,gumRefPtr<Node> > iter = nodes.rbegin();
        iter != nodes.rend(); --iter ) {
    // create the Assent and insert a copy
    list.insert( f( **iter ) );
  }

  return list;
}


// ================================================================================
/// creates a gumNodeList of Assent from a gumNodeList of Node
// ================================================================================
template <typename Node>
template <typename Assent>
gumNodeList<Assent> gumNodeList<Node>::map( Assent( *f )( Node& ),
    gumSize size ) const {
  // determine the proper size of the hashtable of the nodelist
  // by default, the size of the table is set so that the table does not take
  // too much space while allowing to add a few elements without resizing
  if ( size == 0 )
    size = std::max( 2UL, nodes.size() / 2 );

  // create a new nodelist
  gumNodeList<Assent> list( size );

  // fill the new hash table
  for ( gumHashTableIterator<gumId,gumRefPtr<Node> > iter = nodes.rbegin();
        iter != nodes.rend(); --iter ) {
    // create the Assent and insert a copy
    list.insert( f( **iter ) );
  }

  return list;
}


// ================================================================================
/// creates a gumNodeList of Assent from a gumNodeList of Node
// ================================================================================
template <typename Node>
template <typename Assent>
gumNodeList<Assent> gumNodeList<Node>::map( Assent( *f )( const Node& ),
    gumSize size ) const {
  // determine the proper size of the hashtable of the nodelist
  // by default, the size of the table is set so that the table does not take
  // too much space while allowing to add a few elements without resizing
  if ( size == 0 )
    size = std::max( 2UL, nodes.size() / 2 );

  // create a new nodelist
  gumNodeList<Assent> list( size );

  // fill the new hash table
  for ( gumHashTableIterator<gumId,gumRefPtr<Node> > iter = nodes.rbegin();
        iter != nodes.rend(); --iter ) {
    // create the Assent and insert a copy
    list.insert( f( **iter ) );
  }

  return list;
}


// ================================================================================
/// creates a hashtable of Assent from a node list
// ================================================================================
template <typename Node>
template <typename Assent>
gumHashTable<gumId,Assent> gumNodeList<Node>::hashMap
( Assent( *f )( Node ), gumSize size ) const {
  // determine the proper size of the hashtable
  // by default, the size of the table is set so that the table does not take
  // too much space while allowing to add a few elements without resizing
  if ( size == 0 )
    size = std::max( 2UL, nodes.size() / 2 );

  // create a new table
  gumHashTable<gumId,Assent> table( size );

  // fill the new hash table
  for ( gumHashTableIterator<gumId,gumRefPtr<Node> > iter = nodes.rbegin();
        iter != nodes.rend(); --iter ) {
    table.insert( iter.key(), f( *( *iter ) ) );
  }

  return table;
}


// ================================================================================
/// creates a hashtable of Assent from a node list
// ================================================================================
template <typename Node>
template <typename Assent>
gumHashTable<gumId,Assent> gumNodeList<Node>::hashMap
( Assent( *f )( Node& ), gumSize size ) const {
  // determine the proper size of the hashtable
  // by default, the size of the table is set so that the table does not take
  // too much space while allowing to add a few elements without resizing
  if ( size == 0 )
    size = std::max( 2UL, nodes.size() / 2 );

  // create a new table
  gumHashTable<gumId,Assent> table( size );

  // fill the new hash table
  for ( gumHashTableIterator<gumId,gumRefPtr<Node> > iter = nodes.rbegin();
        iter != nodes.rend(); --iter ) {
    table.insert( iter.key(), f( *( *iter ) ) );
  }

  return table;
}


// ================================================================================
/// creates a hashtable of Assent from a node list
// ================================================================================
template <typename Node>
template <typename Assent>
gumHashTable<gumId,Assent> gumNodeList<Node>::hashMap
( Assent( *f )( const Node& ), gumSize size ) const {
  // determine the proper size of the hashtable
  // by default, the size of the table is set so that the table does not take
  // too much space while allowing to add a few elements without resizing
  if ( size == 0 )
    size = std::max( 2UL, nodes.size() / 2 );

  // create a new table
  gumHashTable<gumId,Assent> table( size );

  // fill the new hash table
  for ( gumHashTableIterator<gumId,gumRefPtr<Node> > iter = nodes.rbegin();
        iter != nodes.rend(); --iter ) {
    table.insert( iter.key(), f( *( *iter ) ) );
  }

  return table;
}


// ================================================================================
/// creates a hashtable of Assent from a node list
// ================================================================================
template <typename Node>
template <typename Assent>
gumHashTable<gumId,Assent> gumNodeList<Node>::hashMap
( const Assent& val, gumSize size ) const {
  // determine the proper size of the hashtable
  // by default, the size of the table is set so that the table does not take
  // too much space while allowing to add a few elements without resizing
  if ( size == 0 )
    size = std::max( 2UL, nodes.size() / 2 );

  // create a new table
  gumHashTable<gumId,Assent> table( size );

  // fill the new hash table
  for ( gumHashTableIterator<gumId,gumRefPtr<Node> > iter = nodes.rbegin();
        iter != nodes.rend(); --iter ) {
    table.insert( iter.key(), val );
  }

  return table;
}


// ================================================================================
/// a method to create a list of Assent from a node list
// ================================================================================
template <typename Node>
template <typename Assent>
gumList<Assent> gumNodeList<Node>::listMap( Assent( *f )( Node ) ) const {
  // create a new list
  gumList<Assent> list;

  // fill the new list

  for ( gumHashTableIterator<gumId,gumRefPtr<Node> > iter = nodes.begin();
        iter != nodes.end(); ++iter ) {
    list.pushBack( f( **iter ) );
  }

  return list;
}


// ================================================================================
/// a method to create a list of Assent from a node list
// ================================================================================
template <typename Node>
template <typename Assent>
gumList<Assent> gumNodeList<Node>::listMap( Assent( *f )( Node& ) ) const {
  // create a new list
  gumList<Assent> list;

  // fill the new list

  for ( gumHashTableIterator<gumId,gumRefPtr<Node> > iter = nodes.begin();
        iter != nodes.end(); ++iter ) {
    list.pushBack( f( **iter ) );
  }

  return list;
}


// ================================================================================
/// a method to create a list of Assent from a node list
// ================================================================================
template <typename Node>
template <typename Assent>
gumList<Assent> gumNodeList<Node>::listMap( Assent( *f )( const Node& ) ) const {
  // create a new list
  gumList<Assent> list;

  // fill the new list

  for ( gumHashTableIterator<gumId,gumRefPtr<Node> > iter = nodes.begin();
        iter != nodes.end(); ++iter ) {
    list.pushBack( f( **iter ) );
  }

  return list;
}


// ================================================================================
/// a method to create a list of Assent from a node list
// ================================================================================
template <typename Node>
template <typename Assent>
gumList<Assent> gumNodeList<Node>::listMap( const Assent& val ) const {
  // create a new list
  gumList<Assent> list;

  // fill the new list

  for ( gumSize i=0; i<size(); ++i )
    list.pushBack( val );

  return list;
}


// ================================================================================
/// returns an iterator pointing to the end of the hashtable
// ================================================================================
template <typename Node> INLINE
const gumNodeListIterator<Node>& gumNodeList<Node>::end() const  {
  return iter_end;
}


// ================================================================================
/// the iterator pointing to the rend (beginning) of the hashtable
// ================================================================================
template <typename Node> INLINE
const gumNodeListIterator<Node>& gumNodeList<Node>::rend() const  {
  return iter_rend;
}


// ================================================================================
/// the iterator pointing to the beginning of the hashtable
// ================================================================================
template <typename Node> INLINE
gumNodeListIterator<Node> gumNodeList<Node>::begin() const {
  return iterator( nodes, GUM_NODELIST_ITERATOR_BEGIN );
}


// ================================================================================
/// the iterator pointing to the rbegin (the last element) of the hashtable
// ================================================================================
template <typename Node> INLINE
gumNodeListIterator<Node> gumNodeList<Node>::rbegin() const {
  return iterator( nodes, GUM_NODELIST_ITERATOR_RBEGIN );
}


// ================================================================================
/// checks whether two lists of nodes contain the same elements
// ================================================================================
template <typename Node> INLINE
const bool gumNodeList<Node>::operator== ( const gumNodeList<Node>& from ) const {
  // check whether the lists have the same number of elements
  if ( nodes.size() != from.nodes.size() )
    return false;

  // parse both list and try to find different elements
  try {
    for ( gumHashTableIterator<gumId,gumRefPtr<Node> > iter = nodes.begin();
          iter != nodes.end(); ++iter ) {
      if ( **iter != from[iter.key()] )
        return false;
    }
  } catch ( gumNotFound& ) {
    return false;
  }

  return true;
}


// ================================================================================
/// checks whether two lists of nodes contain the same elements
// ================================================================================
template <typename Node> INLINE
const bool gumNodeList<Node>::shallowEqual( const gumNodeList<Node>& from )
const  {
  return ( nodes == from.nodes );
}


// ================================================================================
/// checks whether two lists of nodes contain different elements
// ================================================================================
template <typename Node> INLINE
const bool gumNodeList<Node>::operator!= ( const gumNodeList<Node>& from ) const {
  // check whether the lists have the same number of elements
  if ( nodes.size() != from.nodes.size() )
    return true;

  // parse both list and try to find different elements
  try {
    for ( gumHashTableIterator<gumId,gumRefPtr<Node> > iter = nodes.begin();
          iter != nodes.end(); ++iter ) {
      if ( **iter != from[iter.key()] )
        return true;
    }
  } catch ( gumNotFound& ) {
    return true;
  }

  return false;
}


// ================================================================================
/// checks whether two lists of nodes contain different elements
// ================================================================================
template <typename Node> INLINE
const bool gumNodeList<Node>::shallowDifferent( const gumNodeList<Node>& from )
const  {
  return ( nodes != from.nodes );
}


// ================================================================================
/// a << operator for gumNodeList
// ================================================================================
template <typename Node>
std::ostream& operator<< ( std::ostream& stream, const gumNodeList<Node>& list ) {
  for ( gumNodeList<gumNode>::iterator iter=list.begin();iter!=list.end();++iter )
    stream<< *iter << " - ";

  return stream;
}






/* ============================================================================== */
/* ============================================================================== */
/* ===              IMPLEMENTATION OF THE GUM_NODE_LIST_ITERATOR              === */
/* ============================================================================== */
/* ============================================================================== */

// ================================================================================
/** this method is only used to construct the begin/rbegin/end/rend
 * iterators of the gumNodeLists */
// ================================================================================
template <typename Node> INLINE
void gumNodeListIterator<Node>::_initialize
( const gumHashTable<gumId,gumRefPtr<Node> >& nodes, int type )  {
  switch ( type ) {
    case GUM_NODELIST_ITERATOR_BEGIN:
      iter = new gumHashTableIterator<gumId,gumRefPtr<Node> > ( nodes.begin() );
      is_end = false;
      break;

    case GUM_NODELIST_ITERATOR_RBEGIN:
      iter = new gumHashTableIterator<gumId,gumRefPtr<Node> > ( nodes.rbegin() );
      is_end = false;
      break;

    case GUM_NODELIST_ITERATOR_END:
      iter = const_cast<gumHashTableIterator<gumId,gumRefPtr<Node> >*>( &nodes.end() );
      is_end = true;
      break;

    case GUM_NODELIST_ITERATOR_REND:
      iter = const_cast<gumHashTableIterator<gumId,gumRefPtr<Node> >*>( &nodes.rend() );
      is_end = true;
      break;

    default:
      GUM_ERROR( gumUndefinedIteratorValue, "case undefined" );
  }
}


// ================================================================================
/// basic constructor: an iterator that points to nothing
// ================================================================================
template <typename Node> INLINE
gumNodeListIterator<Node>::gumNodeListIterator()  : iter( 0 ), is_end( false ) {
  // for debugging purposes
  GUM_CONSTRUCTOR( gumNodeListIterator );
}


// ================================================================================
/// constructor for an iterator pointing to the 'ind_elt'th element
// ================================================================================
template <typename Node> INLINE
gumNodeListIterator<Node>::gumNodeListIterator( gumNodeList<Node>& list,
    gumSize ind_elt )
    :
    iter( new gumHashTableIterator<gumId,gumRefPtr<Node> > ( list.nodes, ind_elt ) ),
    is_end( false ) {
  // for debugging purposes
  GUM_CONSTRUCTOR( gumNodeListIterator );
}


// ================================================================================
/// copy constructor
// ================================================================================
template <typename Node> INLINE
gumNodeListIterator<Node>::gumNodeListIterator
( const gumNodeListIterator<Node>& from )  : iter( 0 ), is_end( false ) {
  // for debugging purposes
  GUM_CONS_CPY( gumNodeListIterator );

  if ( from.iter )
    iter = new gumHashTableIterator<gumId,gumRefPtr<Node> > ( *( from.iter ) );
}


// ================================================================================
/// copy operator
// ================================================================================
template <typename Node> INLINE
gumNodeListIterator<Node>& gumNodeListIterator<Node>::operator=
( const gumNodeListIterator<Node>& from )  {
  if ( this != &from ) {
    // for debugging purposes
    GUM_OP_CPY( gumNodeListIterator );

    if ( iter && !is_end ) delete iter;

    if ( from.iter )
      iter = new gumHashTableIterator<gumId,gumRefPtr<Node> > ( *( from.iter ) );
    else
      iter = 0;

    is_end = false;
  }

  return *this;
}


// ================================================================================
/// destructor
// ================================================================================
template <typename Node> INLINE
gumNodeListIterator<Node>::~gumNodeListIterator()  {
  // for debugging purposes
  GUM_DESTRUCTOR( gumNodeListIterator );

  if ( !is_end && iter ) delete iter;
}


// ================================================================================
/// returns the ID of the node pointed to by the current iterator
// ================================================================================
template <typename Node> INLINE
const gumId& gumNodeListIterator<Node>::getID() const {
  if ( !iter ) GUM_ERROR( gumUndefinedIteratorValue, "null iterator" );

  return iter->key();
}


// ================================================================================
/// makes the iterator point to the next element in the node list.
// ================================================================================
template <typename Node> INLINE
gumNodeListIterator<Node>& gumNodeListIterator<Node>::operator++()  {
  if ( iter ) ++*iter;

  return *this;
}

// ================================================================================
/// makes the iterator point to the preceding element in the node list
// ================================================================================
template <typename Node> INLINE
gumNodeListIterator<Node>& gumNodeListIterator<Node>::operator--()  {
  if ( iter ) --*iter;

  return *this;
}


// ================================================================================
/// checks whether two iterators point to different nodes
// ================================================================================
template <typename Node> INLINE
const bool gumNodeListIterator<Node>::operator!=
( const gumNodeListIterator<Node>& from ) const  {
  if ( iter && from.iter )
    return ( *iter != *( from.iter ) );

  return true;
}


// ================================================================================
/// checks whether two iterators point to the same node
// ================================================================================
template <typename Node> INLINE
const bool gumNodeListIterator<Node>::operator==
( const gumNodeListIterator<Node>& from ) const  {
  if ( iter && from.iter )
    return ( *iter == *( from.iter ) );

  return false;
}


// ================================================================================
/// returns a reference on the node pointed to by the iterator
// ================================================================================
template <typename Node> INLINE
const Node& gumNodeListIterator<Node>::operator*()
const  {
  if ( !iter ) GUM_ERROR( gumUndefinedIteratorValue, "null iterator" );

  return ***iter;
}


// ================================================================================
/// returns a reference on the node pointed to by the iterator
// ================================================================================
template <typename Node> INLINE
Node& gumNodeListIterator<Node>::operator*()  {
  if ( !iter ) GUM_ERROR( gumUndefinedIteratorValue, "null iterator" );

  return ***iter;
}


// ================================================================================
/// returns a shallow reference on the node pointed to by the iterator
// ================================================================================
template <typename Node> INLINE
const gumRefPtr<Node>& gumNodeListIterator<Node>::shallowGet()
const  {
  if ( !iter ) GUM_ERROR( gumUndefinedIteratorValue, "null iterator" );

  return **iter;
}


// ================================================================================
/// dereferences the value pointed to by the iterator
// ================================================================================
template <typename Node> INLINE
Node* gumNodeListIterator<Node>::operator->()
const  {
  if ( !iter ) GUM_ERROR( gumUndefinedIteratorValue, "null iterator" );

  return ( **iter ).operator->();
}


// ================================================================================
/** this constructor is only used to construct the begin/rbegin/end/rend
 * iterators of the gumNodeLists */
// ================================================================================
template <typename Node>
gumNodeListIterator<Node>::gumNodeListIterator
( const gumHashTable<gumId,gumRefPtr<Node> >& nodes, int type )  :
    iter( 0 ) {
  // for debugging purposes
  GUM_CONSTRUCTOR( gumNodeListIterator );

  _initialize( nodes,type );
}


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

