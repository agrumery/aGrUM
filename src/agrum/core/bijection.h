/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
/** @file
 * @brief Set of pairs of elements with fast search for both elements
 *
 * Bijections are some kind of sets of pairs (T1,T2) with the additional feature
 * as compared to Sets: we can search very quickly T2's elements when knowing T1
 * and T1's elements when knowing T2.
 *
 * @author Jean-Philippe DUBUS / Christophe Gonzales
 */

#ifndef GUM_BIJECTION_H
#define GUM_BIJECTION_H


#include <agrum/core/hashTable.h>


namespace gum {
 
  
  // ==============================================================================
  // ==============================================================================
  /// Iterators for bijection
  // ==============================================================================
  // ==============================================================================
  template <typename T1, typename T2>
  class BijectionIterator {
  public:

    /// the possible positions for the iterators 
    enum BijectionPosition {
      GUM_BIJECTION_BEGIN,
      GUM_BIJECTION_END
    };

    
    // ############################################################################
    /// @name Constructors/destructors
    // ############################################################################
    /// @{
    // ============================================================================
    /// Default constructor
    // ============================================================================
    BijectionIterator ();

    // ============================================================================
    /// Constructor
    /** By default, the iterator points to the starting point of the bijection */
    // ============================================================================
    BijectionIterator ( const Bijection<T1,T2>& bijection,
                        BijectionPosition pos = GUM_BIJECTION_BEGIN);

    // ============================================================================
    /// Copy constructor
    // ============================================================================
    BijectionIterator (const BijectionIterator<T1,T2>& toCopy);

    // ============================================================================
    /// Destructor
    // ============================================================================
    virtual ~BijectionIterator();

    /// @}

    
    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{
    // ============================================================================
    /// Copy operator
    // ============================================================================
    BijectionIterator<T1,T2>&
    operator=(const BijectionIterator<T1,T2>& toCopy);

    // ============================================================================
    /// Go to the next association (if it exists)
    // ============================================================================
    BijectionIterator<T1,T2>& operator++();

    // ============================================================================
    /// Go to the previous association (if it exists)
    // ============================================================================
    BijectionIterator<T1,T2>& operator--();

    // ============================================================================
    /// Comparaison iterators
    // ============================================================================
    bool operator!=(const BijectionIterator<T1,T2>& toCompare) const;
    bool operator==(const BijectionIterator<T1,T2>& toCompare) const;

    /// @}


    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{
   
    // ============================================================================
    /// returns the first element of the current association
    // ============================================================================
    const T1& first() const;

    // ============================================================================
    /// returns the second element of the current association
    // ============================================================================
    const T2& second() const;

    /// @}

    
  private:
    /// the hashTable iterator that actually does all the job
    typename HashTable<T1,T2*>::iterator __iter;

  };




  

  // ==============================================================================
  // ==============================================================================
  /**
   * This class is designed for modeling a bijection between two sets, the idea is
   * following :
   * - we want to create a bijection relation between type T1 and type T2
   * - for x in T1, there exists only one y in T2 associated to x
   * - for y in T2, there exists only one x in T1 associated to y
   * - the user inserts all the (x, y) associations and can search efficiently the
   * values thus associated
   */
  // ==============================================================================
  // ==============================================================================
  template <typename T1, typename T2>
  class Bijection {
  public:
    typedef BijectionIterator<T1,T2> iterator;

    // ############################################################################
    /// @name Constructors/destructors
    // ############################################################################
    /// @{

    // ============================================================================
    /// Default constructor: creates a bijection without association
    // ============================================================================
    Bijection(Size size = GUM_HASHTABLE_DEFAULT_SIZE,
	      bool resize_policy = GUM_HASHTABLE_DEFAULT_RESIZE_POLICY);

    // ============================================================================
    /// Copy constructor
    /** @param toCopy Bijection to copy */
    // ============================================================================
    Bijection(const Bijection<T1,T2>& toCopy);

    // ============================================================================
    /// destructor
    // ============================================================================
    virtual ~Bijection();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    // ============================================================================
    /// Copy operator
    /** @param toCopy Bijection to copy */
    // ============================================================================
    Bijection<T1,T2>& operator=(const Bijection<T1,T2>& toCopy);

    /// @}


    // ############################################################################
    /// @name Iterators
    // ############################################################################
    /// @{

    // ============================================================================
    /// returns the iterator at the beginning of the bijection
    /** Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    // ============================================================================
    iterator begin() const;

    // ============================================================================
    /// returns the iterator to the end of the bijection
    /** Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    // ============================================================================
    const iterator& end() const;

    /// @}


    
    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    // ============================================================================
    /// returns the value associated to the element passed in argument
    /** @throws NotFound exception is thrown if the element cannot be found. */
    // ============================================================================
    const T1& first(const T2& second) const;

    // ============================================================================
    /** @brief Same method as first, but if the value is not found, a default
     * value is inserted into the bijection */
    // ============================================================================
    const T1& firstWithDefault(const T2& second, const T1& default_val) const;
 
    // ============================================================================
    /// returns the value associated to the element passed in argument
    /** @throws NotFound exception is thrown if the element cannot be found. */
    // ============================================================================
    const T2& second(const T1& first) const;

    // ============================================================================
    /** @brief Same method as second, but if the value is not found, a default
     * value is inserted into the bijection */
    // ============================================================================
    const T2& secondWithDefault(const T1& first, const T2& default_val) const;

    // ============================================================================
    /// Test whether the bijection contains the "first" value
    // ============================================================================
    bool existsFirst(const T1& first) const;
    
    // ============================================================================
    /// Test whether the bijection contains the "second" value
    // ============================================================================
    bool existsSecond(const T2& second) const;

    // ============================================================================
    /// inserts a new association in the bijection
    /** Note that what is actually inserted into the bijection is a copy of
     * the pair (first,second)
     * @throws DuplicateElement exception is thrown if the association
     * already exists */
    // ============================================================================
    void insert(const T1& first, const T2& second);

    // ============================================================================
    /// removes all the associations from the bijection
    // ============================================================================
    void clear();

    // ============================================================================
    /// returns true if the bijection doesn't contain any association
    // ============================================================================
    bool empty() const;

    // ============================================================================
    /// returns the number of associations stored within the bijection
    // ============================================================================
    Size size() const;

    // ============================================================================
    /// erases an association containing the given first element
    /** If the element cannot be found, nothing is done. In particular, no
     * exception is raised. */
    // ============================================================================
    void eraseFirst(const T1& first);

    // ============================================================================
    /// erase an association containing the given second element
    /** If the element cannot be found, nothing is done. In particular, no
     * exception is raised. */
    // ============================================================================
    void eraseSecond(const T2& second);

    /// @}
    

    // ############################################################################
    /// @name Fine tuning
    // ############################################################################
    /// @{
    // ============================================================================
    /// returns the number of hashtables' slots used (@sa hashTable's capacity)
    // ============================================================================
    Size capacity() const ;

    // ============================================================================
    /// similar to the hashtable's resize
    // ============================================================================
    void resize( Size new_size );

    // ============================================================================
    /// enables the user to change dynamically the resizing policy
    /** @sa HashTable's setResizePolicy */
    // ============================================================================
    void setResizePolicy( const bool new_policy ) ;

    // ============================================================================
    /// returns the current resizing policy
    /** @sa HashTable's resizePolicy */
    // ============================================================================
    bool resizePolicy() const ;

    /// @}

    
  private:
    /// a friend to speed-up accesses
    friend class BijectionIterator<T1,T2>;
  
    // below, we create the two hashtables used by the bijection. Note that
    // the values of these hashtables are actually pointers. This enables to
    // create only once objects (T1,T2). When using bijections with large
    // size objects, this feature is of particular interest
    
    /// hashtable associating T2 objects to T1 objects
    HashTable<T1,T2*> __firstToSecond;

    /// hashtable associating T1 objects to T2 objects
    HashTable<T2,T1*> __secondToFirst;

    /// an end iterator to speed-up accesses
    BijectionIterator<T1,T2> __iter_end;
    // WARNING: bijection iterators use Bijection's hashtable in reverse order,
    // that is, when you do a for (BijectionIterator<T1,T2> iter = B.begin();
    // iter != B.end(); ++iter), what is actually done is something like:
    // (HashtableIterator<T1,T2> iter = B.hash.rbegin(); iter != B.hash.rend();
    // --iter). This may seem strange but this is actually an optimization of the
    // code: Bijection iterators contain hashtableIterators that perform all the
    // job. Unfortunately, each time we add/remove an element to/from the
    // hashtable, or each time we update the size of the latter, the end() iterator
    // of the hashtable is updated. Hence, if we did not use this reverse order,
    // we should update the BijectionIterator::end() accordingly. By using the
    // reverse order, the BijectionIterator::end contains a HashTableIterator::rend
    // which, fortunately, is insensitive to modifications in the hashtable.

    
    
    // ============================================================================
    /// a function that performs a complete copy of another bijection
    /** @warning this function assumes that "this" is an empty bijection. If this
     * is not the case, use function clear() before calling __copy. */
    // ============================================================================
    void __copy (const HashTable<T1,T2*>& f2s); 

    // ============================================================================
    /// inserts a new association in the bijection
    // ============================================================================
    HashTableBucket<T1,T2*>* __insert(const T1& first, const T2& second);
    
  };



  
  

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  
  // ##############################################################################
  // ##############################################################################
  //
  // BELOW, CLASSES ARE SPECIALIZATIONS DESIGNED TO OPTIMIZE BIJECTIONS OF POINTERS
  //
  // ##############################################################################
  // ##############################################################################



  // ==============================================================================
  // ==============================================================================
  /// Iterators for bijection
  // ==============================================================================
  // ==============================================================================
  template <typename T1, typename T2>
  class BijectionIterator<T1*,T2*> {
  public:

    /// the possible positions for the iterators 
    enum BijectionPosition {
      GUM_BIJECTION_BEGIN,
      GUM_BIJECTION_END
    };

    
    // ############################################################################
    /// @name Constructors/destructors
    // ############################################################################
    /// @{
    // ============================================================================
    /// Default constructor
    // ============================================================================
    BijectionIterator ();

    // ============================================================================
    /// Constructor
    /** By default, the iterator points to the starting point of the bijection */
    // ============================================================================
    BijectionIterator ( const Bijection<T1*,T2*>& bijection,
                        BijectionPosition pos = GUM_BIJECTION_BEGIN);

    // ============================================================================
    /// Copy constructor
    // ============================================================================
    BijectionIterator (const BijectionIterator<T1*,T2*>& toCopy);

    // ============================================================================
    /// Destructor
    // ============================================================================
    virtual ~BijectionIterator();

    /// @}

    
    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{
    // ============================================================================
    /// Copy operator
    // ============================================================================
    BijectionIterator<T1*,T2*>&
    operator=(const BijectionIterator<T1*,T2*>& toCopy);

    // ============================================================================
    /// Go to the next association (if it exists)
    // ============================================================================
    BijectionIterator<T1*,T2*>& operator++();

    // ============================================================================
    /// Go to the previous association (if it exists)
    // ============================================================================
    BijectionIterator<T1*,T2*>& operator--();

    // ============================================================================
    /// Comparaison iterators
    // ============================================================================
    bool operator!=(const BijectionIterator<T1*,T2*>& toCompare) const;
    bool operator==(const BijectionIterator<T1*,T2*>& toCompare) const;

    /// @}


    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{
   
    // ============================================================================
    /// returns the first element of the current association
    // ============================================================================
    T1* const first() const;

    // ============================================================================
    /// returns the second element of the current association
    // ============================================================================
    T2* const second() const;

    /// @}

    
  private:
    /// the hashTable iterator that actually does all the job
    typename HashTable<T1*,T2*>::iterator __iter;

  };




  

  // ==============================================================================
  // ==============================================================================
  /**
   * This class is designed for modeling a bijection between two sets, the idea is
   * following :
   * - we want to create a bijection relation between type T1* and type T2
   * - for x in T1*, there exists only one y in T2 associated to x
   * - for y in T2, there exists only one x in T1* associated to y
   * - the user inserts all the (x, y) associations and can search efficiently the
   * values thus associated
   */
  // ==============================================================================
  // ==============================================================================
  template <typename T1, typename T2>
  class Bijection<T1*,T2*> {
  public:
    typedef BijectionIterator<T1*,T2*> iterator;

    // ############################################################################
    /// @name Constructors/destructors
    // ############################################################################
    /// @{

    // ============================================================================
    /// Default constructor: creates a bijection without association
    // ============================================================================
    Bijection(Size size = GUM_HASHTABLE_DEFAULT_SIZE,
	      bool resize_policy = GUM_HASHTABLE_DEFAULT_RESIZE_POLICY);

    // ============================================================================
    /// Copy constructor
    /** @param toCopy Bijection to copy */
    // ============================================================================
    Bijection(const Bijection<T1*,T2*>& toCopy);

    // ============================================================================
    /// destructor
    // ============================================================================
    virtual ~Bijection();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    // ============================================================================
    /// Copy operator
    /** @param toCopy Bijection to copy */
    // ============================================================================
    Bijection<T1*,T2*>& operator=(const Bijection<T1*,T2*>& toCopy);

    /// @}


    // ############################################################################
    /// @name Iterators
    // ############################################################################
    /// @{

    // ============================================================================
    /// returns the iterator at the beginning of the bijection
    /** Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    // ============================================================================
    iterator begin() const;

    // ============================================================================
    /// returns the iterator to the end of the bijection
    /** Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    // ============================================================================
    const iterator& end() const;

    /// @}


    
    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    // ============================================================================
    /// returns the value associated to the element passed in argument
    /** @throws NotFound exception is thrown if the element cannot be found. */
    // ============================================================================
    T1* const first(T2* const second) const;

    // ============================================================================
    /** @brief Same method as first, but if the value is not found, a default
     * value is inserted into the bijection */
    // ============================================================================
    T1* const firstWithDefault(T2* const second, T1* const default_val) const;
 
    // ============================================================================
    /// returns the value associated to the element passed in argument
    /** @throws NotFound exception is thrown if the element cannot be found. */
    // ============================================================================
    T2* const second(T1* const first) const;

    // ============================================================================
    /** @brief Same method as second, but if the value is not found, a default
     * value is inserted into the bijection */
    // ============================================================================
    T2* const secondWithDefault(T1* const first, T2* const default_val) const;

    // ============================================================================
    /// Test whether the bijection contains the "first" value
    // ============================================================================
    bool existsFirst(T1* const first) const;
    
    // ============================================================================
    /// Test whether the bijection contains the "second" value
    // ============================================================================
    bool existsSecond(T2* const second) const;

    // ============================================================================
    /// inserts a new association in the bijection
    /** Note that what is actually inserted into the bijection is a copy of
     * the pair (first,second)
     * @throws DuplicateElement exception is thrown if the association
     * already exists */
    // ============================================================================
    void insert(T1* const first, T2* const second);

    // ============================================================================
    /// removes all the associations from the bijection
    // ============================================================================
    void clear();

    // ============================================================================
    /// returns true if the bijection doesn't contain any association
    // ============================================================================
    bool empty() const;

    // ============================================================================
    /// returns the number of associations stored within the bijection
    // ============================================================================
    Size size() const;

    // ============================================================================
    /// erases an association containing the given first element
    /** If the element cannot be found, nothing is done. In particular, no
     * exception is raised. */
    // ============================================================================
    void eraseFirst(T1* const first);

    // ============================================================================
    /// erase an association containing the given second element
    /** If the element cannot be found, nothing is done. In particular, no
     * exception is raised. */
    // ============================================================================
    void eraseSecond(T2* const second);

    /// @}
    

    // ############################################################################
    /// @name Fine tuning
    // ############################################################################
    /// @{
    // ============================================================================
    /// returns the number of hashtables' slots used (@sa hashTable's capacity)
    // ============================================================================
    Size capacity() const ;

    // ============================================================================
    /// similar to the hashtable's resize
    // ============================================================================
    void resize( Size new_size );

    // ============================================================================
    /// enables the user to change dynamically the resizing policy
    /** @sa HashTable's setResizePolicy */
    // ============================================================================
    void setResizePolicy( const bool new_policy ) ;

    // ============================================================================
    /// returns the current resizing policy
    /** @sa HashTable's resizePolicy */
    // ============================================================================
    bool resizePolicy() const ;

    /// @}

    
  private:
    /// a friend to speed-up accesses
    friend class BijectionIterator<T1*,T2*>;
  
    // below, we create the two hashtables used by the bijection. Note that
    // the values of these hashtables are actually pointers. This enables to
    // create only once objects (T1*,T2*). When using bijections with large
    // size objects, this feature is of particular interest
    
    /// hashtable associating T2* objects to T1* objects
    HashTable<T1*,T2*> __firstToSecond;

    /// hashtable associating T1* objects to T2* objects
    HashTable<T2*,T1*> __secondToFirst;

    /// an end iterator to speed-up accesses
    BijectionIterator<T1*,T2*> __iter_end;
    // WARNING: bijection iterators use Bijection's hashtable in reverse order,
    // that is, when you do a for (BijectionIterator<T1*,T2*> iter = B.begin();
    // iter != B.end(); ++iter), what is actually done is something like:
    // (HashtableIterator<T1*,T2*> iter = B.hash.rbegin(); iter != B.hash.rend();
    // --iter). This may seem strange but this is actually an optimization of the
    // code: Bijection iterators contain hashtableIterators that perform all the
    // job. Unfortunately, each time we add/remove an element to/from the
    // hashtable, or each time we update the size of the latter, the end() iterator
    // of the hashtable is updated. Hence, if we did not use this reverse order,
    // we should update the BijectionIterator::end() accordingly. By using the
    // reverse order, the BijectionIterator::end contains a HashTableIterator::rend
    // which, fortunately, is insensitive to modifications in the hashtable.

    
    
    // ============================================================================
    /// a function that performs a complete copy of another bijection
    /** @warning this function assumes that "this" is an empty bijection. If this
     * is not the case, use function clear() before calling __copy. */
    // ============================================================================
    void __copy (const HashTable<T1*,T2*>& f2s); 

    // ============================================================================
    /// inserts a new association in the bijection
    // ============================================================================
    void __insert(T1* const first, T2* const second);
    
  };

  
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

    
} /* namespace gum */


#include <agrum/core/bijection.tcc>


#endif /* GUM_BIJECTION_H */

