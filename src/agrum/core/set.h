/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
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
/**
 * @file
 * @brief Sets of elements (i.e. the mathematical notion of a set)
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */

#ifndef GUM_SET_H
#define GUM_SET_H

#include <iostream>
#include <agrum/core/list.h>
#include <agrum/core/hashTable.h>


namespace gum {


  template<typename KEY> class SetIterator;

  
  template<class KEY>
  /**
   * @class Set
   * @brief Representation of a set
   *
   * A Set is a structure that contains arbitrary elements. Note that, as in
   * mathematics, an element cannot appear twice in a given set.
   *
   * @par Usage example:
   * @code
   * // creation of a set with 10 elements
   * Set<int> set;
   * for (int i = 0; i< 10; ++i)
   *   set<<i;
   *
   * // parse the set
   * for (Set<int>::iterator iter = set.begin ();
   *        iter != set.end (); ++iter) {
   *   // display the values
   *   cerr <<  *iter << endl;
   * }
   *
   * // parse the set in the other direction
   * for (Set<int>::iterator iter = set.rbegin ();
   *        iter != set.rend (); --iter)
   *   cerr <<  *iter << endl;
   *
   * // use an iterator to point the element we wish to erase
   * Set<int>::iterator iter = set.rbegin ();
   * set.erase ( iter );
   *
   * // check whether two iterators point toward the same element
   * Set<int>::iterator iter1 = set.begin();
   * Set<int>::iterator iter2 = set.rbegin();
   * if (iter1 != iter2)
   *   cerr << "iter1 and iter2 point toward different elements";
   *
   * @endcode
   *
   * @ingroup basicstruct_group
   */
  class Set {
  public:
    typedef SetIterator<KEY> iterator;

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{ 

    /// default constructor
    /** Sets rely on hashtables to store their items. The optional parameters
     * of this constructor enable a fine memory management of these hashtables
     * @param capacity the number of slots allocated to the hashtable (see the
     * HashTable default constructor)
     * @param resize_policy enables the hashtable to resize itself automatically
     * when its number of elements is sufficiently high that it induces slow
     * retrievals of elements */
    Set( Size capacity = GUM_HASHTABLE_DEFAULT_SIZE,  bool resize_policy = true );

    /// copy constructor
    Set( const Set<KEY>& aHT );

    /// destructor
    ~Set();

    /// @}



    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{
    
    /// copy operator
    Set<KEY>& operator=( const Set<KEY>& from );

    /// mathematical equality between two sets
    bool operator==( const Set<KEY>& s2 ) const;

    /// mathematical inequality between two sets
    bool operator!=( const Set<KEY>& s2 ) const;

    /// Intersection operator
    /** @return a Set containing the elements belonging both to this and s2. */
    Set<KEY> operator*( const Set<KEY>& s2 ) const;

    /// Union operator
    /** @return a new Set containing the union of the elements of this and s2 */
    Set<KEY> operator+( const Set<KEY>& s2 ) const;

    /// Disjunction operator
    /** @return a Set whose elements belong to this but not to s2 
     * @warning Unlike + and *, the - operator is not commutative! */
    Set<KEY> operator-( const Set<KEY>& s2 ) const;

    /// adds a new element to the set (alias for insert)
    Set<KEY>& operator<<( const KEY& k );

    /// removes an element from the set (alias for erase)
    Set<KEY>& operator>>( const KEY& k );

    /// @}


    
    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// inserts a new element in the set
    /** @warning if the set already contains the element, nothing is done.
     * In particular, it is not added to the set and no exception is thrown. */
    void insert( const KEY& k );

    /// erases an element from the set
    /** @warning if the set does not contain the element, nothing is done.
     * In particular, no exception is thrown. */    
    void erase( const KEY& k );

    /// erases an element from the set
    /** @warning if the set does not contain the element, nothing is done.
     * In particular, no exception is thrown. */    
    void erase( const iterator& k );

    /// removes all the elements, if any, from the set
    void clear();

    /// returns the number of elements in the set
    Size size() const;

    /// indicates whether a given elements belong to the set
    bool contains( const KEY& k ) const;

    /// indicates whether the set is the empty set
    bool empty() const;

    /// the usual begin iterator to parse the set
    iterator begin() const;

    /// the usual end iterator to parse the set
    const iterator& end() const;

    /// the usual rbegin iterator to parse the set
    iterator rbegin() const;

    /// the usual rend iterator to parse the set
    const iterator& rend() const;

    /// prints the content of the set
    std::string toString() const;

    /// @}
    

    // ############################################################################
    /// @name Fine tuning
    // ############################################################################
    /// @{

    /// returns the size of the underlying hashtable containing the set
    /** The method runs in constant time. */
    Size capacity() const ;

    /// changes the size of the underlying hashtable containing the set
    /** See HashTable's method resize for more details */
    void resize( Size capacity ) ;

    /** @brief enables the user to change dynamically the resizing policy of
     * the underlying hashtable
     *
     * @param new_policy true => the set updates dynamically its memory consumption
     * to guarantee that its elements are fast to retrieve. When new_policy is
     * false, the set will not try to change its memory size, hence resulting in
     * potentially slower accesses. */
    void setResizePolicy( const bool new_policy ) ;
    
    /// returns the current resizing policy of the underlying hashtable
    bool resizePolicy() const ;

    ///  @}

    

    // ############################################################################
    /// @name mapper
    // ############################################################################
    /// @{

    /// creates a hashtable of NEWKEY from the set
    /** @param f a function that maps KEY into a NEWKEY
     * @param size the size of the resulting hashtable. When equal to 0, a default
     * size is computed that is a good trade-off between space consumption and
     * efficiency of new elements insertions.
     * @warning The order in the resulting hashtable may not be similar to that of
     * the original set. Hence iterators on the former may not parse it in the same
     * order as iterators on the latter. */
    template <typename NEWKEY>
    HashTable<KEY,NEWKEY> hashMap( NEWKEY( *f )( const KEY& ),
                                   Size capacity = 0 ) const;

    /// creates a hashtable of NEWKEY from the set
    /** @param val the value taken by all the elements of the resulting hashtable
     * @param size the size of the resulting hashtable. When equal to 0, a default
     * size is computed that is a good trade-off between space consumption and
     * efficiency of new elements insertions.
     * @warning The order in the resulting hashtable may not be similar to that of
     * the original set. Hence iterators on the former may not parse it in the same
     * order as iterators on the latter. */
    template <typename NEWKEY>
    HashTable<KEY,NEWKEY> hashMap( const NEWKEY& val, Size size = 0 ) const;

   /// a method to create a List of NEWKEY from the list of nodes of the graph
    /** @param f a function that maps a node into a Assent
     * @warning the order of the Assent elements in the resulting list is
     * arbitrary */
    template <typename NEWKEY> List<NEWKEY>
    listMap( NEWKEY( *f )( const KEY& ) ) const;
    
    /// @}


  private:
    // friends 
    friend class SetIterator<KEY>;

    /// a set of X's is actually a hashtable whose keys are the X's
    HashTable<KEY,bool> __inside;

    /// to optimize the access to data, end and rend are computed once and for all
    iterator __it_end;
    iterator __it_rend;

    /// convert a hashtable into a set of keys
    Set( const HashTable<KEY,bool>& h );

  };


  
  /* =========================================================================== */
  /* ===                             SET ITERATORS                           === */
  /* =========================================================================== */
  /** @class SetIterator
   * @brief Iterators for the Set class
   **
   * Developers may consider using Set<x>::iterator instead of SetIterator<x>.
   * @par Usage example:
   * @code
   * // creation of a set with 10 elements
   * Set<int> set;
   * for (int i = 0; i< 10; ++i)
   *   set<<i;
   *
   * // parse the set
   * for (Set<int>::iterator iter = table.begin ();
   *        iter != table.end (); ++iter) {
   *   // display the values
   *   cerr <<  *iter << endl;
   * }
   *
   * // parse the hash table in the other direction
   * for (Set<int>::iterator iter = table.rbegin ();
   *        iter != table.rend (); --iter)
   *   cerr <<  *iter << endl;
   *
   * // check whether two iterators point toward the same element
   * Set<int>::iterator iter1 = table1.begin();
   * Set<int>::iterator iter2 = table1.rbegin();
   * if (iter1 != iter)
   *   cerr << "iter1 and iter2 point toward different elements";
   *
   * @endcode
   */
  /* =========================================================================== */
  template <typename KEY> class SetIterator {
  public:
    /** @brief an enumeration to position the iterator at the beginning or the end
     * of the set */
    enum Position {
      GUM_SET_ITERATOR_BEGIN,
      GUM_SET_ITERATOR_END,
      GUM_SET_ITERATOR_RBEGIN,
      GUM_SET_ITERATOR_REND
    };
  
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor: the iterator points toward nothing
    SetIterator( );

    /// creates an iterator for a given set
    /** By default, the iterator points to the beginning of the set, but, using
     * optional argument pos, you can make it point to end() */
    SetIterator( const Set<KEY>& from,
                 Position pos = GUM_SET_ITERATOR_BEGIN );

    /// copy constructor
    SetIterator( const SetIterator<KEY>& from);

    /// destructor
    ~SetIterator();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// increments the iterator
    SetIterator<KEY>& operator++();

    /// decrements the iterator
    SetIterator<KEY>& operator--();

    /// indicates whether two iterators point to different elements or sets
    bool operator!= ( const SetIterator<KEY> &from ) const;

    /// indicates whether two iterators point toward the same element of a same set
    bool operator== ( const SetIterator<KEY> &from ) const;

    /// assignment operator
    SetIterator<KEY>& operator=( const SetIterator<KEY> &from );

    /// returns the element pointed to by the iterator
    /** @throws UndefinedIteratorValue exception if the iterator does not point
     * to an element of the set (for instance if the set or the element previously
     * pointed to by the iterator have been deleted) */
    const KEY& operator*() const ;

    /// returns aointer to the element pointed to by the iterator
    /** @throws UndefinedIteratorValue exception if the iterator does not point
     * to an element of the set (for instance if the set or the element previously
     * pointed to by the iterator have been deleted) */
    const KEY* operator->() const ;

    
  private:
    /// for efficiency, Set should be able to modify the hashtable iterator
    friend class Set<KEY>;

    /// the underlying iterator for the set's hashtable containing the data 
    HashTableIterator<KEY,bool> __ht_iter;

    /// a function to update end/rend iterators
    /** This function is called by Set to update its end/rnd iterators.
     * @param iter __ht_iter will be set to iter */
    void __updatePosition( const HashTableIterator<KEY,bool>& iter );
  };

  
  /// a << operator for HashTableList
  template <typename KEY> std::ostream& operator<<
  ( std::ostream&, const Set<KEY>& );


} /* namespace gum */


/// always include the implementation of the templates
#include <agrum/core/set.tcc>

  
#endif  // GUM_SET_H
