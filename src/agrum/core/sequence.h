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
/** @file
 * @brief Class for storing (ordered) sequences of objects
 *
 * A Sequence<KEY> is quite similar to a vector<KEY> in that it stores an ordered
 * set of elements. The main difference between these two data structures lies in
 * the fact that, given a key, it is possible to retrieve from a Sequence<KEY>
 * the index in the vector where the key lies in O(1). As a result, it is not
 * possible to insert a given element twice in the sequence, that is, all the
 * KEYs must be different.
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */
#ifndef GUM_SEQUENCE_H
#define GUM_SEQUENCE_H

#include <vector>

#include <agrum/config.h>

#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>


namespace gum {


  template<typename KEY> class SequenceIterator;


  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                             GUM_SEQUENCE                            === */
  /* =========================================================================== */
  /* =========================================================================== */
  /** @class Sequence
   * @brief The class for storing (ordered) sequences of objects
   * @ingroup basicstruct_group
   *
   * A Sequence<KEY> is quite similar to a vector<KEY> in that it stores an ordered
   * set of elements. The main difference between these two data structures lies in
   * the fact that, given a key, it is possible to retrieve from a Sequence<KEY>
   * the index in the vector where the key lies in O(1). As a result, it is not
   * possible to insert a given element twice in the sequence, that is, all the
   * KEYs must be different. */
  /* =========================================================================== */
  template<typename KEY>

  class Sequence {

      friend class SequenceIterator<KEY>;

    public:
      typedef SequenceIterator<KEY> iterator;
      typedef SequenceIterator<KEY> const_iterator;


      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{

      // ============================================================================
      /// Default constructor
      // ============================================================================
      Sequence();

      // ============================================================================
      /// copy constructor
      /** @param aSeq the sequence the elements of which will be copied.
       * @warning The elements of the newly constructed sequence are copies of those
       * in aSeq */
      // ============================================================================
      Sequence( const Sequence<KEY>& aSeq );

      // ============================================================================
      /// destructor
      // ============================================================================
      ~Sequence();

      /// @}


      // ############################################################################
      /// @name Iterators
      // ############################################################################
      /// @{

      // ============================================================================
      /// begin iterator
      // ============================================================================
      iterator begin() const;

      // ============================================================================
      /// rbegin iterator
      // ============================================================================
      iterator rbegin() const;

      // ============================================================================
      /// end iterator
      // ============================================================================
      const iterator& end() const;

      // ============================================================================
      /// rend iterator
      // ============================================================================
      const iterator& rend() const;

      /// @}


      // ############################################################################
      /// @name Operators
      // ############################################################################
      /// @{

      // ============================================================================
      /// copy operator
      /** @param aSeq : the copied sequence
       * @return a ref to this */
      // ============================================================================
      Sequence<KEY>& operator= ( const Sequence<KEY>& aSeq );

      // ============================================================================
      /// insert k at the end of the sequence (synonym for insert)
      /** @param k the key we wish to insert in the sequence
       * @return *this
       * @throw DuplicateElement is thrown if the sequence contains already k */
      // ============================================================================
      Sequence<KEY>& operator<<( const KEY& k ) ;

      // ============================================================================
      /// remove k in the sequence (synonym for remove)
      /** If the element cannot be found, the function does nothing. In particular,
       * it throws no exception.
       * @param k the key we wish to remove
       * @return *this */
      // ============================================================================
      Sequence<KEY>& operator>>( const KEY& k );

      // ============================================================================
      /// returns the index of object k if it exists (synonym for pos)
      /** @param k the key the index of which we wish to find
       * @throw NotFound is thrown if k cannot be found in the sequence */
      // ============================================================================
      Idx operator[]( const KEY& k ) const ;

      // ============================================================================
      /// returns the element at position i (synonym for atPos)
      /** @param i
       * @throw NotFound is thrown if the element does not exist */
      // ============================================================================
      const KEY& operator[]( Idx i ) const ;

      // ============================================================================
      /// returns true if the content of k equals that of *this
      /** Note that two sequences are equal if and only if they contain the same
       * KEYs (using KEY::operator==) in the same order
       * @param k */
      // ============================================================================
      bool operator== ( const Sequence<KEY>& k ) const;

      // ============================================================================
      /// returns true if the content of k is different from that of *this
      /** Note that two sequences are equal if and only if they contain the same
       * variables (using KEY::operator==) in the same order
       * @param k */
      // ============================================================================
      bool operator!= ( const Sequence<KEY>& k ) const;

      /// @}


      // ############################################################################
      /// @name Accessors / Modifiers
      // ############################################################################
      /// @{

      // ============================================================================
      /// clear the sequence
      // ============================================================================
      void clear();

      // ============================================================================
      /// returns the size of the sequence
      // ============================================================================
      Size size() const ;

      // ============================================================================
      /// return true if empty
      // ============================================================================
      bool empty() const ;

      // ============================================================================
      /// check the existence of k in the sequence
      /** complexity : \f$o(1)\f$ */
      // ============================================================================
      bool exists( const KEY& k ) const ;

      // ============================================================================
      /// insert an element at the end of the sequence
      /** complexity : \f$o(1)\f$
       * @param k
       * @throw DuplicateElement is thrown if the sequence contains already k */
      // ============================================================================
      void insert( const KEY& k );

      // ============================================================================
      /// remove an element from the sequence
      /** If the element cannot be found, the function does nothing. In patricular,
       * it throws no exception. Complexity \f$o(n)\f$ (need to change the position
       * of at most the n elements)
       * @param k */
      // ============================================================================
      void erase( const KEY& k );

      // ============================================================================
      /// remove from the sequence the element pointed to by the iterator
      /** If the element cannot be found, the function does nothing. In patricular,
       * it throws no exception. Complexity \f$o(n)\f$ (need to change the position
       * of at most the n elements)
       * @param k */
      // ============================================================================
      void erase( const iterator& k );

      // ============================================================================
      /// returns the object at the pos i
      /** @param i
       * @throw NotFound is thrown if the element does not exist */
      // ============================================================================
      const KEY& atPos( Idx i ) const ;

      // ============================================================================
      /// returns the position of the object passed in argument (if it exists)
      /** @throw NotFound is thrown if the element does not exist */
      // ============================================================================
      Idx  pos( const KEY& key ) const ;

      /// change the value
      /** @param i
       * @param newKey
       * @throw NotFound is thrown if the element does not exist
       * @throw DuplicateElement if newKey alreay exists */
      void setAtPos( Idx i,const KEY& newKey );

      /// swap index
      /**
       * @param i
       * @param j
       */
      void swap( Idx i,Idx j );

      // ============================================================================
      /// returns the first element
      /**
       * @throw NotFound if the sequence is empty */
      // ============================================================================
      const KEY& front() const;

      // ============================================================================
      /// returns the last element
      /**
       * @throw NotFound if the sequence is empty */
      // ============================================================================
      const KEY& back() const;

      // ============================================================================
      /// displays the content of the sequence
      // ============================================================================
      std::string toString() const;

      // ============================================================================
      /// modifies the size of the internal structures of the sequence
      /** This function is provided for optimization issues. When you know you will
       * have to insert elements into the sequence, it may be faster to use this
       * function prior to the additions because it will change once and for all
       * the sizes of all the internal containers. Note that if you provide a size
       * that is smaller than the number of elements of the sequence, the function
       * will not modify anything. */
      // ============================================================================
      void resize( unsigned int new_size );

      /// difference between to sequence as a Set<KEY>
      /**
       * This function gives the set difference : *this \ seq
       */
      Set<KEY> diffSet( const Sequence<KEY>& seq) const;


      /// @}

    private:
      /// keep track of the position of the element in v (for fast retrieval)
      HashTable<KEY,Idx> __h;

      /// the set of the elements stored into the sequence
      std::vector<KEY*> __v;
      // note that, using KEY* in __v, we actually store the KEY only once in the
      // sequence (that is, within __h). This enables storing big objects within
      // sequences without having memory overhead

      /// stores the end iterator for fast access
      SequenceIterator<KEY> __end;

      /// stores the rend iterator for fast access
      SequenceIterator<KEY> __rend;


      // ============================================================================
      /// a method to update the end iterator after changes in the sequence
      // ============================================================================
      void __update_end();

      // ============================================================================
      /// clears the current sequence and fill it with copies the element of aSeq
      // ============================================================================
      void __copy( const Sequence<KEY>& aSeq );

      // ============================================================================
      /** @brief insert an element at the end of the sequence. private version
       * for internal use */
      // ============================================================================
      void __insert( const KEY& k );
  };








  /// @class SequenceIterator
  /**@brief iterator for Sequence.
   *
   * This iterator is a wrapper to the Idx __iterator. Internally,
   * __iterator belongs to the intervall [-1,seq.size()], -1 and seq.size()
   * being respectively rend and end.
   **/
  template<typename KEY>

  class SequenceIterator {

      friend class Sequence<KEY>;

    public:
      // ############################################################################
      /// @name constructors / destructors
      // ############################################################################
      ///@{
      // ============================================================================
      /// constructor: always give a valid iterator (even if pos too large)
      /**
      * @param seq the sequence
      * @param pos indicates to which position of the sequence the iterator should
       * be pointing. By default, the iterator points to begin()
       * @warning if pos is greater than the size of the sequence, the iterator
       * is made pointing to end() */
      // ============================================================================
      SequenceIterator( const Sequence<KEY>& seq, Idx pos=0 );

      // ============================================================================
      /// copy constructor
      // ============================================================================
      SequenceIterator( const SequenceIterator<KEY>& source );

      // ============================================================================
      /// destructor
      // ============================================================================
      ~SequenceIterator();

      ///@}


      // ############################################################################
      /// @name operators
      // ############################################################################
      ///@{
      // ============================================================================
      /// copy operator
      // ============================================================================
      SequenceIterator<KEY>& operator=( const SequenceIterator& source );

      // ============================================================================
      /// point the iterator to the next value in the sequence
      // ============================================================================
      SequenceIterator<KEY>& operator++();

      // ============================================================================
      /// point the iterator to the preceding value in the sequence
      // ============================================================================
      SequenceIterator<KEY>& operator--();

      // ============================================================================
      /// checks whether two iterators are pointing toward different elements
      // ============================================================================
      bool operator!=( const SequenceIterator<KEY>& source ) const;

      // ============================================================================
      /// checks whether two iterators are pointing toward the same element
      // ============================================================================
      bool operator==( const SequenceIterator<KEY>& source ) const;

      // ============================================================================
      /// returns the value pointed to by the iterator
      ///@throw UndefinedIteratorValue on end() or rend()
      // ============================================================================
      const KEY& operator*() const ;

      // ============================================================================
      /// returns the value pointed to by the iterator
      ///@throw UndefinedIteratorValue on end() or rend()
      // ============================================================================
      const KEY* operator->() const ;

      /// @}

      // ############################################################################
      /// @name Accessors / Modifiers
      // ############################################################################
      /// @{

      ///@throw UndefinedIteratorValue on end() or rend()
      Idx pos() const;

      /// @}



    private:
      /// the index in the sequence's vector where the iterator is pointing
      Idx __iterator;

      /// the sequence pointed to by the iterator
      const Sequence<KEY> *__seq;


      // ============================================================================
      /// the iterator points to the posth element (0 = beginning of the sequence).
      // ============================================================================
      void __setPos( Idx pos );

      // ============================================================================
      /// the iterator points to rend
      // ============================================================================
      void __setAtRend();

      // ============================================================================
      /// the iterator points to the end (which is pos size()-1)
      // ============================================================================
      void __setAtEnd();

  };


  // ==============================================================================
  /// a << operator for displaying the content of the Sequence
  // ==============================================================================
  template <typename KEY>
  std::ostream& operator<< ( std::ostream& stream, const Sequence<KEY>& s );










#ifndef DOXYGEN_SHOULD_SKIP_THIS


  // ##############################################################################
  // ##############################################################################
  //
  // BELOW, CLASSES ARE SPECIALIZATIONS DESIGNED TO OPTIMIZE SEQUENCES OF POINTERS
  //
  // ##############################################################################
  // ##############################################################################


  /* =========================================================================== */
  /* =========================================================================== */
  /* ===             GUM_SEQUENCE_ITERATOR OPTIMIZED FOR POINTERS            === */
  /* =========================================================================== */
  /* =========================================================================== */
  /// @class SequenceIterator
  /**@brief iterator for Sequence.
   *
   * This iterator is a wrapper to the Idx __iterator. Internally,
   * __iterator belongs to the intervall [-1,seq.size()], -1 and seq.size()
   * being respectively rend and end.
   **/
  template<typename KEY>

  class SequenceIterator<KEY*> {

      friend class Sequence<KEY*>;

    public:
      // ############################################################################
      /// @name constructors / destructors
      // ############################################################################
      ///@{
      // ============================================================================
      /// constructor: always give a valid iterator (even if pos too large)
      /** @param pos indicates to which position of the sequence the iterator should
       * be pointing. By default, the iterator points to begin()
       * @warning if pos is greater than the size of the sequence, the iterator
       * is made pointing to end() */
      // ============================================================================
      SequenceIterator( const Sequence<KEY*>& seq, Idx pos=0 );

      // ============================================================================
      /// copy constructor
      // ============================================================================
      SequenceIterator( const SequenceIterator<KEY*>& source );

      // ============================================================================
      /// destructor
      // ============================================================================
      ~SequenceIterator();

      ///@}


      // ############################################################################
      /// @name operators
      // ############################################################################
      ///@{
      // ============================================================================
      /// copy operator
      // ============================================================================
      SequenceIterator<KEY*>& operator=( const SequenceIterator& source );

      // ============================================================================
      /// point the iterator to the next value in the sequence
      // ============================================================================
      SequenceIterator<KEY*>& operator++();

      // ============================================================================
      /// point the iterator to the preceding value in the sequence
      // ============================================================================
      SequenceIterator<KEY*>& operator--();

      // ============================================================================
      /// checks whether two iterators are pointing toward different elements
      // ============================================================================
      bool operator!=( const SequenceIterator<KEY*>& source ) const;

      // ============================================================================
      /// checks whether two iterators are pointing toward the same element
      // ============================================================================
      bool operator==( const SequenceIterator<KEY*>& source ) const;

      // ============================================================================
      /// returns the value pointed to by the iterator
      ///@throw UndefinedIteratorValue on end() or rend()
      // ============================================================================
      KEY* const operator*() const ;

      /// @}


      // ############################################################################
      /// @name Accessors / Modifiers
      // ############################################################################
      /// @{

      ///@throw UndefinedIteratorValue on end() or rend()
      Idx pos() const;

      /// @}



    private:
      /// the index in the sequence's vector where the iterator is pointing
      Idx __iterator;

      /// the sequence pointed to by the iterator
      const Sequence<KEY*> *__seq;


      // ============================================================================
      /// the iterator points to the posth element (0 = beginning of the sequence).
      // ============================================================================
      void __setPos( Idx pos );

      // ============================================================================
      /// the iterator points to rend
      // ============================================================================
      void __setAtRend();

      // ============================================================================
      /// the iterator points to the end (which is pos size()-1)
      // ============================================================================
      void __setAtEnd();

  };



  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                 GUM_SEQUENCE OPTIMIZED FOR POINTERS                 === */
  /* =========================================================================== */
  /* =========================================================================== */
  /** @class Sequence
   * @brief The class for storing (ordered) sequences of objects
   * @ingroup basicstruct_group
   *
   * A Sequence<KEY> is quite similar to a vector<KEY> in that it stores an ordered
   * set of elements. The main difference between these two data structures lies in
   * the fact that, given a key, it is possible to retrieve from a Sequence<KEY>
   * the index in the vector where the key lies in O(1). As a result, it is not
   * possible to insert a given element twice in the sequence, that is, all the
   * KEYs must be different. */
  /* =========================================================================== */
  template<typename KEY>

  class Sequence<KEY*> {

      friend class SequenceIterator<KEY*>;

    public:
      typedef SequenceIterator<KEY*> iterator;
      typedef SequenceIterator<KEY*> const_iterator;


      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{

      // ============================================================================
      /// Default constructor
      // ============================================================================
      Sequence();

      // ============================================================================
      /// copy constructor
      /** @param aSeq the sequence the elements of which will be copied.
       * @warning The elements of the newly constructed sequence are copies of those
       * in aSeq */
      // ============================================================================
      Sequence( const Sequence<KEY*>& aSeq );

      // ============================================================================
      /// destructor
      // ============================================================================
      ~Sequence();

      /// @}


      // ############################################################################
      /// @name Iterators
      // ############################################################################
      /// @{

      // ============================================================================
      /// begin iterator
      // ============================================================================
      iterator begin() const;

      // ============================================================================
      /// rbegin iterator
      // ============================================================================
      iterator rbegin() const;

      // ============================================================================
      /// end iterator
      // ============================================================================
      const iterator& end() const;

      // ============================================================================
      /// rend iterator
      // ============================================================================
      const iterator& rend() const;

      /// @}


      // ############################################################################
      /// @name Operators
      // ############################################################################
      /// @{

      // ============================================================================
      /// copy operator
      /** @param aSeq : the copied sequence
       * @return a ref to this */
      // ============================================================================
      Sequence<KEY*>& operator= ( const Sequence<KEY*>& aSeq );

      // ============================================================================
      /// insert k at the end of the sequence (synonym for insert)
      /** @param k the key we wish to insert in the sequence
       * @return *this
       * @throw DuplicateElement is thrown if the sequence contains already k */
      // ============================================================================
      Sequence<KEY*>& operator<<( KEY* const k ) ;

      // ============================================================================
      /// remove k in the sequence (synonym for remove)
      /** If the element cannot be found, the function does nothing. In particular,
       * it throws no exception.
       * @param k the key we wish to remove
       * @return *this */
      // ============================================================================
      Sequence<KEY*>& operator>>( KEY* const k );

      // ============================================================================
      /// returns the index of object k if it exists (synonym for pos)
      /** @param k the key the index of which we wish to find
       * @throw NotFound is thrown if k cannot be found in the sequence */
      // ============================================================================
      Idx operator[]( KEY* const k ) const ;

      // ============================================================================
      /// returns the element at position i (synonym for atPos)
      /** @param i
       * @throw NotFound is thrown if the element does not exist */
      // ============================================================================
      KEY* const operator[]( Idx i ) const ;

      // ============================================================================
      /// returns true if the content of k equals that of *this
      /** Note that two sequences are equal if and only if they contain the same
       * KEY*s (using KEY*::operator==) in the same order
       * @param k */
      // ============================================================================
      bool operator== ( const Sequence<KEY*>& k ) const;

      // ============================================================================
      /// returns true if the content of k is different from that of *this
      /** Note that two sequences are equal if and only if they contain the same
       * variables (using KEY*::operator==) in the same order
       * @param k */
      // ============================================================================
      bool operator!= ( const Sequence<KEY*>& k ) const;

      /// @}


      // ############################################################################
      /// @name Accessors / Modifiers
      // ############################################################################
      /// @{

      // ============================================================================
      /// clear the sequence
      // ============================================================================
      void clear();

      // ============================================================================
      /// returns the size of the sequence
      // ============================================================================
      Size size() const ;

      // ============================================================================
      /// return true if empty
      // ============================================================================
      bool empty() const ;

      // ============================================================================
      /// check the existence of k in the sequence
      /** complexity : \f$o(1)\f$ */
      // ============================================================================
      bool exists( KEY* const k ) const ;

      // ============================================================================
      /// insert an element at the end of the sequence
      /** complexity : \f$o(1)\f$
       * @param k
       * @throw DuplicateElement is thrown if the sequence contains already k */
      // ============================================================================
      void insert( KEY* const k );

      // ============================================================================
      /// remove an element from the sequence
      /** If the element cannot be found, the function does nothing. In patricular,
       * it throws no exception. Complexity \f$o(n)\f$ (need to change the position
       * of at most the n elements)
       * @param k */
      // ============================================================================
      void erase( KEY* const k );

      // ============================================================================
      /// remove from the sequence the element pointed to by the iterator
      /** If the element cannot be found, the function does nothing. In patricular,
       * it throws no exception. Complexity \f$o(n)\f$ (need to change the position
       * of at most the n elements)
       * @param k */
      // ============================================================================
      void erase( const iterator& );

      // ============================================================================
      /// returns the object at the pos i
      /** @param i
       * @throw NotFound is thrown if the element does not exist */
      // ============================================================================
      KEY* const atPos( Idx i ) const ;

      // ============================================================================
      /// returns the position of the object passed in argument (if it exists)
      /** @throw NotFound is thrown if the element does not exist */
      // ============================================================================
      Idx  pos( KEY* const key ) const ;

      /// change the value
      /** @param i
       * @param newKey
       * @throw NotFound is thrown if the element does not exist
       * @throw DuplicateElement if newKey alreay exists */
      void setAtPos( Idx i, KEY* const newKey );

      /// swap index
      /**
       * @param i
       * @param j
       */
      void swap( Idx i,Idx j );

      // ============================================================================
      /// returns the first element
      /**
       * @throw NotFound if the sequence is empty */
      // ============================================================================
      KEY* const front() const;

      // ============================================================================
      /// returns the last element
      /**
       * @throw NotFound if the sequence is empty */
      // ============================================================================
      KEY* const back() const;

      // ============================================================================
      /// displays the content of the sequence
      // ============================================================================
      std::string toString() const;

      // ============================================================================
      /// modifies the size of the internal structures of the sequence
      /** This function is provided for optimization issues. When you know you will
       * have to insert elements into the sequence, it may be faster to use this
       * function prior to the additions because it will change once and for all
       * the sizes of all the internal containers. Note that if you provide a size
       * that is smaller than the number of elements of the sequence, the function
       * will not modify anything. */
      // ============================================================================
      void resize( unsigned int new_size );

      /// difference between to sequence as a Set<KEY>
      /**
       * This function gives the set difference : *this \ seq
       */
      Set<KEY*> diffSet( const Sequence<KEY*>& seq) const;

      /// @}


    private:
      /// keep track of the position of the element in v (for fast retrieval)
      HashTable<KEY*,Idx> __h;

      /// the set of the elements stored into the sequence
      std::vector<KEY*> __v;
      // note that, using KEY* in __v, we actually store the KEY only once in the
      // sequence (that is, within __h). This enables storing big objects within
      // sequences without having memory overhead

      /// stores the end iterator for fast access
      SequenceIterator<KEY*> __end;

      /// stores the rend iterator for fast access
      SequenceIterator<KEY*> __rend;


      // ============================================================================
      /// a method to update the end iterator after changes in the sequence
      // ============================================================================
      void __update_end();

      // ============================================================================
      /// clears the current sequence and fill it with copies the element of aSeq
      // ============================================================================
      void __copy( const Sequence<KEY*>& aSeq );

      // ============================================================================
      /** @brief insert an element at the end of the sequence. private version
       * for internal use */
      // ============================================================================
      void __insert( KEY* const k );
  };


#endif /* DOXYGEN_SHOULD_SKIP_THIS */



} /* namespace gum */


/// always include the implementation of the templates
#include <agrum/core/sequence.tcc>


#endif /* GUM_SEQUENCE_H */
