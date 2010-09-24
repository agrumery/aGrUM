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
 * @brief Implementation of the Set
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <sstream>

#include <agrum/core/set.h>

namespace gum {

  /* =========================================================================== */
  /* ===                             SET ITERATORS                           === */
  /* =========================================================================== */

  /// default constructor: the iterator points toward nothing
  template<typename KEY> INLINE
  SetIterator<KEY>::SetIterator( ) : __ht_iter() {
    GUM_CONSTRUCTOR ( SetIterator );
  }


  /// creates an iterator for a given set
  template<typename KEY> INLINE
  SetIterator<KEY>::SetIterator ( const Set<KEY>& set, Position pos ) :
      __ht_iter ( pos == GUM_SET_ITERATOR_END ?
                  set.__inside.end() : set.__inside.begin() ) {
    GUM_CONSTRUCTOR ( SetIterator );
  }


  /// copy constructor
  template<typename KEY> INLINE
  SetIterator<KEY>::SetIterator ( const SetIterator<KEY>& iter ) :
      __ht_iter ( iter.__ht_iter ) {
    GUM_CONS_CPY ( SetIterator );
  }


  /// destructor
  template<typename KEY> INLINE
  SetIterator<KEY>::~SetIterator() {
    GUM_DESTRUCTOR ( SetIterator );
  }


  /// indicates whether two iterators point to different elements or sets
  template<typename KEY> INLINE
  bool SetIterator<KEY>::operator!= ( const SetIterator<KEY> &from ) const {
    return __ht_iter != from.__ht_iter;
  }


  /// indicates whether two iterators point toward the same element of a same set
  template<typename KEY> INLINE
  bool SetIterator<KEY>::operator== ( const SetIterator<KEY> &from ) const {
    return __ht_iter == from.__ht_iter;
  }


  /// assignment operator
  template<typename KEY> INLINE
  SetIterator<KEY>& SetIterator<KEY>::operator= ( const SetIterator<KEY> &from ) {
    __ht_iter = from.__ht_iter;
    return *this;
  }


  /// returns the element pointed to by the iterator
  template<typename KEY> INLINE
  const KEY& SetIterator<KEY>::operator*() const {
    // note that, if the hashtable's iterator points toward nothing, it will
    // raise an UndefinedIteratorValue exception
    return __ht_iter.key();
  }


  /// returns aointer to the element pointed to by the iterator
  template<typename KEY> INLINE
  const KEY* SetIterator<KEY>::operator->() const {
    // note that, if the hashtable's iterator points toward nothing, it will
    // raise an UndefinedIteratorValue exception
    return & ( __ht_iter.key() );
  }


  /// increments the iterator
  template<typename KEY> INLINE
  SetIterator<KEY>& SetIterator<KEY>::operator++() {
    // note that, if the hashtable's iterator points toward nothing, the
    // hashtable's iterator incrementation will do nothing. In particular, it
    // will not segfault.
    ++__ht_iter;
    return *this;
  }







  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                                 SETS                                === */
  /* =========================================================================== */
  /* =========================================================================== */

  // returns the end iterator for other classes' statics
  template <typename KEY>
  const SetIterator<KEY>& Set<KEY>::end4Statics() {
    return * ( reinterpret_cast<const SetIterator<KEY>*>
               ( SetIteratorStaticEnd::end4Statics () ) );
  }

  // returns the end iterator for other classes' statics
  template <typename KEY>
  const SetIterator<KEY>& Set<KEY>::constEnd4Statics() {
    return * ( reinterpret_cast<const SetIterator<KEY>*>
               ( SetIteratorStaticEnd::constEnd4Statics () ) );
  }

  /// default constructor
  template<typename KEY>
  Set<KEY>::Set ( Size capacity ,  bool resize_policy ) :
      // create the hash table without key uniqueness policy (as we will check
      // ourselves the uniqueness of KEYs before inserting new elements)
      __inside ( capacity, resize_policy, false ) {
    GUM_CONSTRUCTOR ( Set );

    // make sure the end() iterator is constructed properly
    end4Statics ();
  }


  /// convert a hashtable into a set of keys
  template<typename KEY>
  Set<KEY>::Set ( const HashTable<KEY, bool>& h ) :
      __inside ( h ) {
    GUM_CONSTRUCTOR ( Set );
    // ensure that __inside's uniqueness policy is set to false as this
    // will speed-up insertions of new elements
    __inside.setKeyUniquenessPolicy ( false );

    // make sure the end() iterator is constructed properly
    end4Statics ();
  }


  /// copy constructor
  template<typename KEY>
  Set<KEY>::Set ( const Set<KEY>& s ) :
      __inside ( s.__inside ) {
    GUM_CONS_CPY ( Set );

    // make sure the end() iterator is constructed properly
    end4Statics ();
  }


  /// destructor
  template<typename KEY>
  Set<KEY>::Set::~Set() {
    GUM_DESTRUCTOR ( Set );
  }


  /// the usual begin iterator to parse the set
  template<typename KEY> INLINE
  typename Set<KEY>::const_iterator Set<KEY>::begin() const {
    return SetIterator<KEY> ( *this );
  }


  /// the usual begin iterator to parse the set
  template<typename KEY> INLINE
  SetIterator<KEY> Set<KEY>::begin() {
    return SetIterator<KEY> ( *this );
  }


  /// the usual end iterator to parse the set
  template<typename KEY> INLINE
  const typename Set<KEY>::const_iterator& Set<KEY>::end() const {
    return * ( reinterpret_cast<const SetIterator<KEY>*>
               ( SetIteratorStaticEnd::__SetIterEnd ) );
  }


  /// the usual end iterator to parse the set
  template<typename KEY> INLINE
  const SetIterator<KEY>& Set<KEY>::end() {
    return * ( reinterpret_cast<const SetIterator<KEY>*>
               ( SetIteratorStaticEnd::__SetIterEnd ) );
  }


  /// removes all the elements, if any, from the set
  template<typename KEY> INLINE
  void Set<KEY>::clear( ) {
    // first we remove all the elements from the hashtable actually containing
    // the elements of the set. Note that, doing so, all the hashtable iterators
    // will be updated as well. In turn, this will imply that, whenever an
    // operation will be performed on a SetIterator, this will raise an exception.
    __inside.clear();

    // Note that actually there is no need to update the end iterator as this one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }


  /// copy operator
  template<typename KEY>
  Set<KEY>& Set<KEY>::operator= ( const Set<KEY>& s ) {
    // avoid self assignment
    if ( &s != this ) {
      // remove the old content of the set. Actually, we remove all the elements
      // from the underlying hashtable. Note that, doing so, all the hashtable
      // iterators will be updated as well. In turn, this will imply that, whenever
      // an operation will be performed on a SetIterator, this will raise an
      // exception.
      clear();

      // prepare the set for its new data
      resize ( s.capacity() );
      setResizePolicy ( s.resizePolicy() );

      // copy the set
      __inside = s.__inside;

      // Note that actually there is no need to update the end iterator as this one
      // is not affected by changes within hashtables (adding/deleting elements).
      // Hence, for speedup, we do not update the end iterator
    }

    return *this;
  }


  /// indicates whether a given elements belong to the set
  template<typename KEY> INLINE
  bool Set<KEY>::contains ( const KEY& k ) const {
    return __inside.exists ( k );
  }

  /// indicates whether a given elements belong to the set
  template<typename KEY> INLINE
  bool Set<KEY>::exists ( const KEY& k ) const {
    return __inside.exists ( k );
  }

  /// inserts a new element in the set
  template<typename KEY> INLINE
  void Set<KEY>::insert ( const KEY& k ) {
    // WARNING: we shall always test whether k already belongs to the set before
    // trying to insert it because we set __inside's key uniqueness policy to
    // false
    if ( ! contains ( k ) ) {
      // insert the element
      __inside.insert ( k, true );

      // Note that actually there is no need to update the end iterator as this one
      // is not affected by changes within hashtables (adding/deleting elements).
      // Hence, for speedup, we do not update the end iterator
    }
  }


  /// erases an element from the set
  template<typename KEY> INLINE
  void Set<KEY>::erase ( const KEY& k ) {
    // erase the element (if it exists)
    __inside.erase ( k );

    // Note that actually there is no need to update the end iterator as this one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }


  /// erases an element from the set
  template<typename KEY> INLINE
  void Set<KEY>::erase ( const SetIterator<KEY>& iter ) {
    // erase the element
    __inside.erase ( iter.__ht_iter );

    // Note that actually there is no need to update the end iterator as this one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }


  /// adds a new element to the set
  template<typename KEY> INLINE
  Set<KEY>& Set<KEY>::operator<< ( const KEY& k ) {
    insert ( k );
    return ( *this );
  }


  /// removes an element from the set
  template<typename KEY> INLINE
  Set<KEY>& Set<KEY>::operator>> ( const KEY& k ) {
    erase ( k );
    return ( *this );
  }


  /// returns the number of elements in the set
  template<typename KEY> INLINE
  Size Set<KEY>::size() const {
    return __inside.size();
  }


  /// indicates whether the set is the empty set
  template<typename KEY> INLINE
  bool Set<KEY>::empty( ) const {
    return __inside.empty();
  }


  /// mathematical equality between two sets
  template<typename KEY>
  bool Set<KEY>::operator== ( const Set<KEY>& s2 ) const {
    const HashTable<KEY, bool>& h2 = s2.__inside;

    // check whether both sets have the same number of elements

    if ( size() != h2.size() ) return false;

    // check the content of the sets
    for ( HashTableConstIterator<KEY, bool> iter = __inside.begin();
          iter != __inside.end(); ++iter ) {
      if ( ! h2.exists ( iter.key() ) )
        return false;
    }

    return true;
  }


  /// mathematical inequality between two sets
  template<typename KEY> INLINE
  bool Set<KEY>::operator!= ( const Set<KEY>& s2 ) const {
    return ! ( operator== ( s2 ) );
  }


  /// Intersection operator
  template<typename KEY>
  Set<KEY> Set<KEY>::operator* ( const Set<KEY>& s2 ) const {
    Set<KEY> res;
    const HashTable<KEY, bool>& h2 = s2.__inside;
    HashTable<KEY, bool>& h_r = res.__inside;

    if ( size() < h2.size() ) {
      for ( HashTableConstIterator<KEY, bool> iter = __inside.begin();
            iter != __inside.end(); ++iter ) {
        if ( h2.exists ( iter.key() ) )
          h_r.insert ( iter.key(), true );
      }
    } else {
      for ( HashTableConstIterator<KEY, bool> iter = h2.begin();
            iter != h2.end(); ++iter ) {
        if ( __inside.exists ( iter.key() ) )
          h_r.insert ( iter.key(), true );
      }
    }

    return res;
  }


  /// Union operator
  template<typename KEY>
  Set<KEY> Set<KEY>::operator+ ( const Set<KEY>& s2 ) const {
    Set<KEY> res = *this;
    const HashTable<KEY, bool>& h2 = s2.__inside;
    HashTable<KEY, bool>& h_r = res.__inside;

    for ( HashTableConstIterator<KEY, bool> iter = h2.begin();
          iter != h2.end(); ++iter ) {
      if ( ! h_r.exists ( iter.key() ) )
        h_r.insert ( iter.key(), true );
    }

    return res;
  }


  /// Disjunction operator
  template<typename KEY>
  Set<KEY> Set<KEY>::operator- ( const Set<KEY>& s2 ) const {
    Set<KEY> res;
    const HashTable<KEY, bool>& h2 = s2.__inside;
    HashTable<KEY, bool>& h_r = res.__inside;

    for ( HashTableConstIterator<KEY, bool> iter = __inside.begin();
          iter != __inside.end();++iter )
      if ( ! h2.exists ( iter.key() ) )
        h_r.insert ( iter.key(), true );

    return res;
  }


  /// to display the content of the set
  template<typename KEY> INLINE
  std::string Set<KEY>::toString() const {
    std::stringstream out;
    bool first = true;
    out << "{";

    for ( iterator iter = begin();iter != end();++iter ) {
      if ( first ) {
        out << *iter;
        first = false;
      } else {
        out << "," << *iter;
      }
    }

    out << "}";

    std::string res;
    out >> res;
    return res;
  }


  /// to friendly display the content of the set
  template <typename Key> std::ostream& operator<<
  ( std::ostream& stream, const Set<Key>& set ) {
    stream << set.toString();
    return stream;
  }


  /// returns the size of the underlying hashtable containing the set
  template<typename KEY> INLINE
  Size Set<KEY>::capacity() const {
    return __inside.capacity();
  }


  /// changes the size of the underlying hashtable
  template<typename KEY> INLINE
  void Set<KEY>::resize ( Size new_size ) {
    __inside.resize ( new_size );

    // Note that actually there is no need to update the end iterator as this one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }


  /// enables the user to change dynamically the resizing policy of the
  /// underlying hashtable
  template<typename KEY> INLINE
  void Set<KEY>::setResizePolicy ( const bool new_policy ) {
    __inside.setResizePolicy ( new_policy );

    // Note that actually there is no need to update the end iterator as this one
    // is not affected by changes within hashtables (adding/deleting elements).
    // Hence, for speedup, we do not update the end iterator
  }


  /// returns the current resizing policy of the underlying hashtable
  template<typename KEY> INLINE
  bool Set<KEY>::resizePolicy() const {
    return __inside.resizePolicy();
  }


  /// creates a hashtable of NEWKEY from the set
  template<typename KEY>
  template <typename NEWKEY>
  HashTable<KEY, NEWKEY>
  Set<KEY>::hashMap ( NEWKEY ( *f ) ( const KEY& ), Size size ) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without resizing
    if ( size == 0 )
      size = std::max ( 2UL, __inside.size() / 2 );

    // create a new table
    HashTable<KEY, NEWKEY> table ( size );

    // fill the new hash table
    for ( HashTableConstIterator<KEY, bool > iter = __inside.begin();
          iter != __inside.end(); ++iter ) {
      table.insert ( iter.key(), f ( iter.key() ) );
    }

    return table;
  }


  /// creates a hashtable of NEWKEY from the set
  template<typename KEY>
  template <typename NEWKEY>
  HashTable<KEY, NEWKEY> Set<KEY>::hashMap ( const NEWKEY& val, Size size ) const {
    // determine the proper size of the hashtable
    // by default, the size of the table is set so that the table does not take
    // too much space while allowing to add a few elements without resizing
    if ( size == 0 )
      size = std::max ( 2UL, __inside.size() / 2 );

    // create a new table
    HashTable<KEY, NEWKEY> table ( size );

    // fill the new hash table
    for ( HashTableConstIterator<KEY, bool > iter = __inside.begin();
          iter != __inside.end(); ++iter ) {
      table.insert ( iter.key(), val );
    }

    return table;
  }


  /// a method to create a list of NEWKEY from the set
  template<typename KEY>
  template <typename NEWKEY>
  List<NEWKEY> Set<KEY>::listMap ( NEWKEY ( *f ) ( const KEY& ) ) const {
    // create a new list
    List<NEWKEY> list;

    // fill the new list

    for ( HashTableConstIterator<KEY, bool > iter = __inside.begin();
          iter != __inside.end(); ++iter ) {
      list.pushBack ( f ( iter.key() ) );
    }

    return list;
  }



} /* namespace gum */


#endif    // DOXYGEN_SHOULD_SKIP_THIS
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
