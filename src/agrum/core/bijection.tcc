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
 * @brief Class providing generic double hash tables
 *
 * @author Jean-Philippe DUBUS
 */


#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  // ==============================================================================
  // ==============================================================================
  /// Bijection::iterators
  // ==============================================================================
  // ==============================================================================

  // ==============================================================================
  /// Default constructor
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  BijectionIterator<T1,T2>::BijectionIterator () : __iter () {
    GUM_CONSTRUCTOR(BijectionIterator);
  }

  // ==============================================================================
  /// Constructor
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  BijectionIterator<T1,T2>::BijectionIterator
  ( const Bijection<T1,T2>& biject, BijectionIterator<T1,T2>::Position pos ) :
    __iter ( pos == GUM_BIJECTION_BEGIN ? biject.__firstToSecond.begin() :
             biject.__firstToSecond.end() ) {
    GUM_CONSTRUCTOR(BijectionIterator);
  }
  
  // ==============================================================================
  /// Destructor
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  BijectionIterator<T1,T2>::~BijectionIterator() {
    GUM_DESTRUCTOR(BijectionIterator);
  }
  
  // ==============================================================================
  /// Copy constructor
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  BijectionIterator<T1,T2>::BijectionIterator
  (const BijectionIterator<T1,T2>& toCopy) :
    __iter( toCopy.__iter ) {
    GUM_CONS_CPY(BijectionIterator);
  }

  // ==============================================================================
  /// Copy operator
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  BijectionIterator<T1,T2>&
  BijectionIterator<T1,T2>::operator=(const BijectionIterator<T1,T2>& toCopy) {
    __iter = toCopy.__iter;
    return *this;
  }
  
  // ==============================================================================
  /// return the first element of the current association
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  const T1& BijectionIterator<T1,T2>::first() const {
    return __iter.key();
  }

  // ==============================================================================
  /// return the second element of the current association
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  const T2& BijectionIterator<T1,T2>::second() const {
    return **__iter;
  }
  
  // ==============================================================================
  /// Go to the next association (if exists)
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  BijectionIterator<T1,T2>& BijectionIterator<T1,T2>::operator++() {
    ++__iter;
    return *this;
  }
  
  // ==============================================================================
  /// Comparison of iterators
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  bool BijectionIterator<T1,T2>::operator!=
  (const BijectionIterator<T1,T2>& toCompare) const {
    return __iter != toCompare.__iter;
  }

  // ==============================================================================
  /// Comparison of iterators
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  bool BijectionIterator<T1,T2>::operator==
  (const BijectionIterator<T1,T2>& toCompare) const {
    return __iter == toCompare.__iter;
  }




  
  
  // ==============================================================================
  // ==============================================================================
  /// Bijections
  // ==============================================================================
  // ==============================================================================

  // ==============================================================================
  // returns the end iterator for other classes' statics
  // ==============================================================================
  template <typename T1, typename T2>
  const BijectionIterator<T1,T2>& Bijection<T1,T2>::end4Statics() {
    return *(reinterpret_cast<const BijectionIterator<T1,T2>*>
             (BijectionIteratorStaticEnd::end4Statics ()));
  }
  
  // ==============================================================================
  /// Default constructor: creates a bijection without association
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  Bijection<T1,T2>::Bijection( Size size,
                               bool resize_policy ) :
    // warning: below, we create the internal hashTables with a key uniqueness
    // policy set to false because we will do the uniqueness tests ourselves (this
    // will speed-up the process)
    __firstToSecond(size, resize_policy, false),
    __secondToFirst(size, resize_policy, false) {
    GUM_CONSTRUCTOR( Bijection );

    // make sure the end() iterator is constructed properly
    end4Statics ();    
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  Bijection<T1,T2>::~Bijection() {
    GUM_DESTRUCTOR( Bijection );
  }
  
  // ==============================================================================
  /// returns the iterator at the beginning of the bijection
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  typename Bijection<T1,T2>::iterator
  Bijection<T1,T2>::begin() const {
    return BijectionIterator<T1,T2>( *this );
  }

  // ==============================================================================
  /// returns the iterator to the end of the bijection
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  const BijectionIterator<T1,T2>&
  Bijection<T1,T2>::end() const {
    return *(reinterpret_cast<const BijectionIterator<T1,T2>*>
             (BijectionIteratorStaticEnd::__BijectionIterEnd));
  }

  // ==============================================================================
  /// a function that performs a complete copy of another bijection
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  void Bijection<T1,T2>::__copy (const HashTable<T1,T2*>& f2s) {
    // parse f2s and perform copies
    for ( HashTableIterator<T1,T2*> iter = f2s.begin();
          iter != f2s.end(); ++iter ) {
      HashTableBucket<T1,T2*>* bucket1 =  
        __firstToSecond._insertAndGetBucket ( iter.key(), 0 );
      HashTableBucket<T1,T2*>* bucket2;
      try { bucket2 = __secondToFirst._insertAndGetBucket ( **iter, 0 ); }
      catch (...) { __firstToSecond.erase ( iter.key() ); throw; }
      bucket1->val = &(bucket2->key);
      bucket2->val = &(bucket1->key);
    }
    // note that __iter_end is actually a constant, whatever we add/remove
    // to/from __firstToSecond. As a consequence, it need not be updated
    // after __copy
  }

  
  // ==============================================================================
  /// Copy constructor
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  Bijection<T1,T2>::Bijection(const Bijection<T1,T2>& toCopy) {
    GUM_CONS_CPY( Bijection );
    __copy ( toCopy.__firstToSecond);

    // make sure the end() iterator is constructed properly
    end4Statics ();
  }

  // ==============================================================================
  /// removes all the associations from the bijection
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  void Bijection<T1,T2>::clear() {
    __firstToSecond.clear();
    __secondToFirst.clear();
    // note that __iter_end is actually a constant, whatever we add/remove
    // to/from __firstToSecond. As a consequence, it need not be updated
    // after the clear's
  }
  
  // ==============================================================================
  /// Copy operator
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  Bijection<T1,T2>& 
  Bijection<T1,T2>::operator=(const Bijection<T1,T2>& toCopy) {
    // avoid self assignment
    if ( this != &toCopy ) {
      clear ();
      __copy ( toCopy.__firstToSecond );
    }
    // note that __iter_end is actually a constant, whatever we add/remove
    // to/from __firstToSecond. As a consequence, it need not be updated
    // after __copy
    return *this;
  }

  // ==============================================================================
  /// returns the value associated to the element passed in argument
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  const T1& Bijection<T1,T2>::first(const T2& second) const {
    return *( __secondToFirst[second] );
  }

  // ==============================================================================
  /// returns the value associated to the element passed in argument
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  const T2& Bijection<T1,T2>::second(const T1& first) const {
    return *( __firstToSecond[first] );
  }

  // ==============================================================================
  /// Test whether the bijection contains the "first" value
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  bool Bijection<T1,T2>::existsFirst(const T1& first) const {
    return __firstToSecond.exists(first);
  }

  // ==============================================================================
  /// Test whether the bijection contains the "second" value
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  bool Bijection<T1,T2>::existsSecond(const T2& second) const {
    return __secondToFirst.exists(second);
  }
  
  // ==============================================================================
  /// inserts a new association in the bijection
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  HashTableBucket<T1,T2*>*
  Bijection<T1,T2>::__insert(const T1& first, const T2& second) {
    // check the uniqueness property
    if ( existsFirst(first) || existsSecond(second) ) {
      GUM_ERROR( DuplicateElement,
                 "the bijection contains an element with the same key" );
    }

    // insert copies of first and second
    HashTableBucket<T1,T2*>* bucket1 =
      __firstToSecond._insertAndGetBucket ( first, 0 );
    HashTableBucket<T1,T2*>* bucket2;
    try { bucket2 = __secondToFirst._insertAndGetBucket ( second, 0 ); }
    catch (...) { __firstToSecond.erase ( first ); throw; }
    bucket1->val = &(bucket2->key);
    bucket2->val = &(bucket1->key);

    return bucket1;
  }

  // ==============================================================================
  /// inserts a new association in the bijection
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  void Bijection<T1,T2>::insert(const T1& first, const T2& second) {
    __insert(first,second);
  }

  // ==============================================================================
  /** @brief Same method as first, but if the value is not found, a default
   * value is inserted into the bijection */
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  const T1&
  Bijection<T1,T2>::firstWithDefault(const T2& second, const T1& val) const {
    try { return first ( second ); }
    catch ( NotFound& ) { return __insert ( val, second )->key; }
  }

  // ==============================================================================
  /** @brief Same method as second, but if the value is not found, a default
   * value is inserted into the bijection */
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  const T2&
  Bijection<T1,T2>::secondWithDefault(const T1& first, const T2& val) const {
    try { return second ( first ); }
    catch ( NotFound& ) { return *( __insert (first,val)->val ); }
  }

  // ==============================================================================
  /// returns true if the bijection doesn't contain any relation
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  bool Bijection<T1,T2>::empty() const {
    GUM_ASSERT (__firstToSecond.empty() == __secondToFirst.empty());
    return __firstToSecond.empty();
  }

  // ==============================================================================
  /// returns the number of associations stored within the bijection
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  Size Bijection<T1,T2>::size() const {
    GUM_ASSERT(__firstToSecond.size() == __secondToFirst.size());
    return __firstToSecond.size();
  }

  // ==============================================================================
  /// erases an association containing the given first element
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  void Bijection<T1,T2>::eraseFirst(const T1& first) {
    try {
      __secondToFirst.erase( *__firstToSecond[first] );
      __firstToSecond.erase(first);
    } catch(NotFound&) { }
  }

  // ==============================================================================
  /// erase an association containing the given second element
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  void Bijection<T1,T2>::eraseSecond(const T2& second) {
    try {
      __firstToSecond.erase( *__secondToFirst[second] );
      __secondToFirst.erase(second);
    } catch(NotFound&) { }
  }

  // ==============================================================================
  /// returns the number of hashtables' slots used (@sa hashTable's capacity)
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  Size Bijection<T1,T2>::capacity() const {
    return __firstToSecond.capacity();
  }

  // ==============================================================================
  /// similar to the hashtable's resize
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  void Bijection<T1,T2>::resize( Size new_size ) {
    __firstToSecond.resize ( new_size );
    __secondToFirst.resize ( new_size );
  }

  // ==============================================================================
  /// enables the user to change dynamically the resizing policy
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  void Bijection<T1,T2>::setResizePolicy( const bool new_policy ) {
    __firstToSecond.setResizePolicy(new_policy);
    __secondToFirst.setResizePolicy(new_policy);
  }

  // ==============================================================================
  /// returns the current resizing policy
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  bool Bijection<T1,T2>::resizePolicy() const {
    return __firstToSecond.resizePolicy();
  }







  
  // ##############################################################################
  // ##############################################################################
  //
  // BELOW, CLASSES ARE SPECIALIZATIONS DESIGNED TO OPTIMIZE BIJECTIONS OF POINTERS
  //
  // ##############################################################################
  // ##############################################################################



  // ==============================================================================
  // ==============================================================================
  /// Bijection::iterators
  // ==============================================================================
  // ==============================================================================

  // ==============================================================================
  /// Default constructor
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  BijectionIterator<T1*,T2*>::BijectionIterator () : __iter () {
    GUM_CONSTRUCTOR(BijectionIterator);
  }

  // ==============================================================================
  /// Constructor
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  BijectionIterator<T1*,T2*>::BijectionIterator
  ( const Bijection<T1*,T2*>& biject, BijectionIterator<T1*,T2*>::Position pos ) :
    __iter ( pos == GUM_BIJECTION_BEGIN ? biject.__firstToSecond.begin() :
             biject.__firstToSecond.end() ) {
    GUM_CONSTRUCTOR(BijectionIterator);
  }
  
  // ==============================================================================
  /// Destructor
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  BijectionIterator<T1*,T2*>::~BijectionIterator() {
    GUM_DESTRUCTOR(BijectionIterator);
  }
  
  // ==============================================================================
  /// Copy constructor
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  BijectionIterator<T1*,T2*>::BijectionIterator
  (const BijectionIterator<T1*,T2*>& toCopy) :
    __iter( toCopy.__iter ) {
    GUM_CONS_CPY(BijectionIterator);
  }

  // ==============================================================================
  /// Copy operator
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  BijectionIterator<T1*,T2*>&
  BijectionIterator<T1*,T2*>::operator=(const BijectionIterator<T1*,T2*>& toCopy) {
    __iter = toCopy.__iter;
    return *this;
  }
  
  // ==============================================================================
  /// return the first element of the current association
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  T1* const BijectionIterator<T1*,T2*>::first() const {
    return __iter.key();
  }

  // ==============================================================================
  /// return the second element of the current association
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  T2* const BijectionIterator<T1*,T2*>::second() const {
    return *__iter;
  }
  
  // ==============================================================================
  /// Go to the next association (if exists)
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  BijectionIterator<T1*,T2*>& BijectionIterator<T1*,T2*>::operator++() {
    ++__iter;
    return *this;
  }
  
  // ==============================================================================
  /// Comparison of iterators
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  bool BijectionIterator<T1*,T2*>::operator!=
  (const BijectionIterator<T1*,T2*>& toCompare) const {
    return __iter != toCompare.__iter;
  }

  // ==============================================================================
  /// Comparison of iterators
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  bool BijectionIterator<T1*,T2*>::operator==
  (const BijectionIterator<T1*,T2*>& toCompare) const {
    return __iter == toCompare.__iter;
  }




  
  
  // ==============================================================================
  // ==============================================================================
  /// Bijections
  // ==============================================================================
  // ==============================================================================

  // ==============================================================================
  // returns the end iterator for other classes' statics
  // ==============================================================================
  template <typename T1, typename T2>
  const BijectionIterator<T1*,T2*>& Bijection<T1*,T2*>::end4Statics() {
    return *(reinterpret_cast<const BijectionIterator<T1*,T2*>*>
             (BijectionStarIteratorStaticEnd::end4Statics ()));
  }
  
  // ==============================================================================
  /// Default constructor: creates a bijection without association
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  Bijection<T1*,T2*>::Bijection( Size size,
                                 bool resize_policy ) :
    // warning: below, we create the internal hashTables with a key uniqueness
    // policy set to false because we will do the uniqueness tests ourselves (this
    // will speed-up the process)
    __firstToSecond(size, resize_policy, false),
    __secondToFirst(size, resize_policy, false) {
    GUM_CONSTRUCTOR( Bijection );

    // make sure the end() iterator is constructed properly
    end4Statics ();
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  Bijection<T1*,T2*>::~Bijection() {
    GUM_DESTRUCTOR( Bijection );
  }
  
  // ==============================================================================
  /// returns the iterator at the beginning of the bijection
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  typename Bijection<T1*,T2*>::iterator
  Bijection<T1*,T2*>::begin() const {
    return BijectionIterator<T1*,T2*>( *this );
  }

  // ==============================================================================
  /// returns the iterator to the end of the bijection
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  const BijectionIterator<T1*,T2*>&
  Bijection<T1*,T2*>::end() const {
    return *(reinterpret_cast<const BijectionIterator<T1*,T2*>*>
             (BijectionStarIteratorStaticEnd::__BijectionStarIterEnd)); 
  }

  // ==============================================================================
  /// a function that performs a complete copy of another bijection
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  void Bijection<T1*,T2*>::__copy (const HashTable<T1*,T2*>& f2s) {
    // parse f2s and perform copies
    for ( HashTableIterator<T1*,T2*> iter = f2s.begin();
          iter != f2s.end(); ++iter ) {
      __firstToSecond._insertAndGetBucket ( iter.key(), *iter );
      try { __secondToFirst._insertAndGetBucket ( *iter, iter.key() ); }
      catch (...) { __firstToSecond.erase ( iter.key() ); throw; }
    }
    // note that __iter_end is actually a constant, whatever we add/remove
    // to/from __firstToSecond. As a consequence, it need not be updated
    // after __copy
  }
  
  // ==============================================================================
  /// Copy constructor
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  Bijection<T1*,T2*>::Bijection(const Bijection<T1*,T2*>& toCopy) {
    GUM_CONS_CPY( Bijection );
    __copy ( toCopy.__firstToSecond);

    // make sure the end() iterator is constructed properly
    end4Statics ();
  }

  // ==============================================================================
  /// removes all the associations from the bijection
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  void Bijection<T1*,T2*>::clear() {
    __firstToSecond.clear();
    __secondToFirst.clear();
    // note that __iter_end is actually a constant, whatever we add/remove
    // to/from __firstToSecond. As a consequence, it need not be updated
    // after the clear's
  }
  
  // ==============================================================================
  /// Copy operator
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  Bijection<T1*,T2*>& 
  Bijection<T1*,T2*>::operator=(const Bijection<T1*,T2*>& toCopy) {
    // avoid self assignment
    if ( this != &toCopy ) {
      clear ();
      __copy ( toCopy.__firstToSecond );
    }
    // note that __iter_end is actually a constant, whatever we add/remove
    // to/from __firstToSecond. As a consequence, it need not be updated
    // after __copy
    return *this;
  }

  // ==============================================================================
  /// returns the value associated to the element passed in argument
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  T1* const Bijection<T1*,T2*>::first(T2* const second) const {
    return __secondToFirst[second];
  }

  // ==============================================================================
  /// returns the value associated to the element passed in argument
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  T2* const Bijection<T1*,T2*>::second(T1* const first) const {
    return __firstToSecond[first];
  }

  // ==============================================================================
  /// Test whether the bijection contains the "first" value
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  bool Bijection<T1*,T2*>::existsFirst(T1* const first) const {
    return __firstToSecond.exists(first);
  }

  // ==============================================================================
  /// Test whether the bijection contains the "second" value
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  bool Bijection<T1*,T2*>::existsSecond(T2* const second) const {
    return __secondToFirst.exists(second);
  }

  // ==============================================================================
  /// inserts a new association in the bijection
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  void Bijection<T1*,T2*>::__insert(T1* const first, T2* const second) {
    // check the uniqueness property
    if ( existsFirst(first) || existsSecond(second) ) {
      GUM_ERROR( DuplicateElement,
                 "the bijection contains an element with the same key" );
    }

    // insert copies of first and second
    __firstToSecond._insertAndGetBucket ( first, second );
    try { __secondToFirst._insertAndGetBucket ( second, first ); }
    catch (...) { __firstToSecond.erase ( first ); throw; }
  }

  // ==============================================================================
  /// inserts a new association in the bijection
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  void Bijection<T1*,T2*>::insert(T1* const first, T2* const second) {
    __insert(first,second);
  }

  // ==============================================================================
  /** @brief Same method as first, but if the value is not found, a default
   * value is inserted into the bijection */
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  T1* const
  Bijection<T1*,T2*>::firstWithDefault(T2* const second, T1* const val) const {
    try { return first ( second ); }
    catch ( NotFound& ) {
      __insert ( val, second );
      return val;
    }
  }

  // ==============================================================================
  /** @brief Same method as second, but if the value is not found, a default
   * value is inserted into the bijection */
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  T2* const
  Bijection<T1*,T2*>::secondWithDefault(T1* const first, T2* const val) const {
    try { return second ( first ); }
    catch ( NotFound& ) {
      __insert (first,val);
      return val;
    }
  }

  // ==============================================================================
  /// returns true if the bijection doesn't contain any relation
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  bool Bijection<T1*,T2*>::empty() const {
    GUM_ASSERT (__firstToSecond.empty() == __secondToFirst.empty());
    return __firstToSecond.empty();
  }

  // ==============================================================================
  /// returns the number of associations stored within the bijection
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  Size Bijection<T1*,T2*>::size() const {
    GUM_ASSERT(__firstToSecond.size() == __secondToFirst.size());
    return __firstToSecond.size();
  }

  // ==============================================================================
  /// erases an association containing the given first element
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  void Bijection<T1*,T2*>::eraseFirst(T1* const first) {
    try {
      __secondToFirst.erase( __firstToSecond[first] );
      __firstToSecond.erase(first);
    } catch(NotFound&) { }
  }

  // ==============================================================================
  /// erase an association containing the given second element
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  void Bijection<T1*,T2*>::eraseSecond(T2* const second) {
    try {
      __firstToSecond.erase( __secondToFirst[second] );
      __secondToFirst.erase(second);
    } catch(NotFound&) { }
  }

  // ==============================================================================
  /// returns the number of hashtables' slots used (@sa hashTable's capacity)
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  Size Bijection<T1*,T2*>::capacity() const {
    return __firstToSecond.capacity();
  }

  // ==============================================================================
  /// similar to the hashtable's resize
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  void Bijection<T1*,T2*>::resize( Size new_size ) {
    __firstToSecond.resize ( new_size );
    __secondToFirst.resize ( new_size );
  }

  // ==============================================================================
  /// enables the user to change dynamically the resizing policy
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  void Bijection<T1*,T2*>::setResizePolicy( const bool new_policy ) {
    __firstToSecond.setResizePolicy(new_policy);
    __secondToFirst.setResizePolicy(new_policy);
  }

  // ==============================================================================
  /// returns the current resizing policy
  // ==============================================================================
  template <typename T1, typename T2> INLINE
  bool Bijection<T1*,T2*>::resizePolicy() const {
    return __firstToSecond.resizePolicy();
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

