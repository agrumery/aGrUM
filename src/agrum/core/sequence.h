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
 * A Sequence<Key> is quite similar to a vector<Key> in that it stores an ordered
 * set of elements. The main difference between these two data structures lies in
 * the fact that, given a key, it is possible to retrieve from a Sequence<Key>
 * the index in the vector where the key lies in O(1). As a result, it is not
 * possible to insert a given element twice in the sequence, that is, all the
 * Keys must be different.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_SEQUENCE_H
#define GUM_SEQUENCE_H

#include <type_traits>
#include <vector>
#include <initializer_list>

#include <agrum/config.h>

#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>

namespace gum {

  template <typename Key, typename Alloc, bool> class SequenceImplementation;
  template <typename Key, typename Alloc> class Sequence;
  template <typename Key> class SequenceIteratorSafe;
  template <typename Key> using SequenceIterator = SequenceIteratorSafe<Key>;
  template <typename Key> using SequenceConstIterator = SequenceIteratorSafe<Key>;

  /* =========================================================================== */
  /* ===                 NON SCALAR GUM SEQUENCE IMPLEMENTATION              === */
  /* =========================================================================== */
  /** @class SequenceImplementation
   * @brief The internal class for storing (ordered) sequences of objects
   *
   * A SequenceImplementation<Key,Alloc,bool Gen> is a specialized version of
   * of a Sequence<Key,Alloc>. It shall not be used by itself but rather through
   * the Sequence class. A SequenceImplementation is quite similar to a
   * vector<Key> in that it stores an ordered set of elements. The main difference
   * between these two data structures lies in the fact that, given a key, it is
   * possible to retrieve from a SequenceImplementation the index in the vector
   * where the key lies in O(1). As a result, it is not possible to insert a given
   * element twice in the sequence, that is, all the Keys must be different.
   *
   * When the Boolean template parameter gen is false, SequenceImplementation
   * implements a very generic sequence. This allows having Sequences containing
   * elements of virtually any class or type. When the Boolean gen is equal to
   * true, the SequenceImplementation shall contain only scalar types (integers,
   * floats, pointers, etc). As such, knowning that the element is a scalar
   * enables to optimize the code of the sequences. Determining whether gen should
   * be set to true or false is not left to the developper but is determined by the
   * compiler itself at compile time.
   *
   * @par Usage example:
   * @code
   *
   * @endcode */
  template <typename Key, typename Alloc, bool Gen> class SequenceImplementation {

    template <typename K, typename A, bool> friend class SequenceImplementation;
    friend class SequenceIteratorSafe<Key>;
    friend class Sequence<Key, Alloc>;

    public:
    /// types for STL compliance
    /// @{
    using value_type = Key;
    using reference = Key &;
    using const_reference = const Key &;
    using pointer = Key *;
    using const_pointer = const Key *;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = Alloc;
    using iterator = SequenceIterator<Key>;
    using const_iterator = SequenceIterator<Key>;
    using iterator_safe = SequenceIteratorSafe<Key>;
    using const_iterator_safe = SequenceIteratorSafe<Key>;
    /// @}

    private:
    /// Default constructor
    SequenceImplementation(Size size_param = HashTableConst::default_size);

    /// initializer list constructor
    SequenceImplementation(std::initializer_list<Key> list);

    /// copy constructor
    /** @param aSeq the sequence the elements of which will be copied.
     * @warning The elements of the newly constructed sequence are copies of those
     * in aSeq */
    SequenceImplementation(const SequenceImplementation<Key, Alloc, Gen> &aSeq);

    /// generalized copy constructor
    template <typename OtherAlloc>
    SequenceImplementation(const SequenceImplementation<Key, OtherAlloc, Gen> &aSeq);

    /// move constructor
    SequenceImplementation(SequenceImplementation<Key, Alloc, Gen> &&aSeq);

    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// destructor
    ~SequenceImplementation() noexcept;

    /// @}

    // ############################################################################
    /// @name Iterators
    // ############################################################################
    /// @{

    /// returns a safe begin iterator
    iterator_safe beginSafe() const;

    /// returns a safe rbegin iterator
    iterator_safe rbeginSafe() const;

    /// returns the safe end iterator
    const iterator_safe &endSafe() const noexcept;

    /// returns the safe rend iterator
    const iterator_safe &rendSafe() const noexcept;

    /// returns an unsafe begin iterator
    iterator begin() const;

    /// returns an unsafe rbegin iterator
    iterator rbegin() const;

    /// returns the unsafe end iterator
    const iterator &end() const noexcept;

    /// returns the unsafe rend iterator
    const iterator &rend() const noexcept;

    /// @}

    private:
    /// copy operator
    /** @param aSeq : the copied sequence
     * @return a ref to this */
    SequenceImplementation<Key, Alloc, Gen> &
    operator=(const SequenceImplementation<Key, Alloc, Gen> &aSeq);

    /// generalized copy operator
    template <typename OtherAlloc>
    SequenceImplementation<Key, Alloc, Gen> &
    operator=(const SequenceImplementation<Key, OtherAlloc, Gen> &aSeq);

    /// move operator
    SequenceImplementation<Key, Alloc, Gen> &
    operator=(SequenceImplementation<Key, Alloc, Gen> &&aSeq);

    public:
    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// insert k at the end of the sequence (synonym for insert)
    /** @param k the key we wish to insert in the sequence
     * @return *this
     * @throw DuplicateElement is thrown if the sequence contains already k */
    SequenceImplementation<Key, Alloc, Gen> &operator<<(const Key &k);

    /// insert k at the end of the sequence (synonym for insert)
    /** @param k the key we wish to insert in the sequence
     * @return *this
     * @throw DuplicateElement is thrown if the sequence contains already k */
    SequenceImplementation<Key, Alloc, Gen> &operator<<(Key &&k);

    /// remove k in the sequence (synonym for erase)
    /** If the element cannot be found, the function does nothing. In particular,
     * it throws no exception.
     * @param k the key we wish to remove
     * @return *this */
    SequenceImplementation<Key, Alloc, Gen> &operator>>(const Key &k);

    /// returns the element at position i (synonym for atPos)
    /** @param i
     * @throw OutOfBounds is thrown if the element does not exist */
    const Key &operator[](Idx i) const;

    /// returns true if the content of k equals that of *this
    /** Note that two sequences are equal if and only if they contain the same
     * Keys (using Key::operator==) in the same order
     * @param k */
    template <typename OtherAlloc>
    bool operator==(const SequenceImplementation<Key, OtherAlloc, Gen> &k) const;

    /// returns true if the content of k is different from that of *this
    /** Note that two sequences are equal if and only if they contain the same
     * variables (using Key::operator==) in the same order
     * @param k */
    template <typename OtherAlloc>
    bool operator!=(const SequenceImplementation<Key, OtherAlloc, Gen> &k) const;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// clear the sequence
    void clear();

    /// returns the size of the sequence
    Size size() const noexcept;

    /// return true if empty
    bool empty() const noexcept;

    /// check the existence of k in the sequence
    /** complexity : \f$o(1)\f$ */
    bool exists(const Key &k) const;

    /// insert an element at the end of the sequence
    /** complexity : \f$o(1)\f$
     * @param k
     * @throw DuplicateElement is thrown if the sequence contains already k */
    void insert(const Key &k);

    /// move an element at the end of the sequence
    /** complexity : \f$o(1)\f$
     * @param k
     * @throw DuplicateElement is thrown if the sequence contains already k */
    void insert(Key &&k);

    /// emplace a new element in the sequence
    /** emplace is a method that allows to construct directly an element of
     * type Key by passing to its constructor all the arguments it needs
     * @param args the arguments passed to the constructor
     * @throw DuplicateElement is thrown if the sequence contains already k */
    template <typename... Args> void emplace(Args &&... args);

    /// remove an element from the sequence
    /** If the element cannot be found, the function does nothing. In particular,
     * it throws no exception. Complexity \f$o(n)\f$ (need to change the position
     * of at most the n elements)
     * @param k */
    void erase(const Key &k);

    /// remove from the sequence the element pointed to by the iterator
    /** If the element cannot be found, the function does nothing. In particular,
     * it throws no exception. Complexity \f$o(n)\f$ (need to change the position
     * of at most the n elements)
     * @param k */
    void erase(const iterator_safe &k);

    /// returns the object at the pos i
    /** @param i
     * @throw NotFound is thrown if the element does not exist */
    const Key &atPos(Idx i) const;

    /// returns the position of the object passed in argument (if it exists)
    /** @throw NotFound is thrown if the element does not exist */
    Idx pos(const Key &key) const;

    /// change the value
    /** @param i
     * @param newKey
     * @throw NotFound is thrown if the element does not exist
     * @throw DuplicateElement if newKey alreay exists */
    void setAtPos(Idx i, const Key &newKey);

    /// change the value
    /** @param i
     * @param newKey
     * @throw NotFound is thrown if the element does not exist
     * @throw DuplicateElement if newKey alreay exists */
    void setAtPos(Idx i, Key &&newKey);

    /// swap index
    /**
     * @param i the index of the first elt to swap
     * @param j the index of the other elt to swap
     */
    void swap(Idx i, Idx j);

    /// returns the first element of the element
    /**
     * @throw NotFound if the sequence is empty */
    const Key &front() const;

    /// returns the last element of the sequence
    /**
     * @throw NotFound if the sequence is empty */
    const Key &back() const;

    /// displays the content of the sequence
    std::string toString() const;

    /// modifies the size of the internal structures of the sequence
    /** This function is provided for optimization issues. When you know you will
     * have to insert elements into the sequence, it may be faster to use this
     * function prior to the additions because it will change once and for all
     * the sizes of all the internal containers. Note that if you provide a size
     * that is smaller than the number of elements of the sequence, the function
     * will not modify anything. */
    void resize(unsigned int new_size);

    /// @}

    private:
    /// keep track of the position of the element in v (for fast retrieval)
    HashTable<Key, Idx, Alloc> __h;

    /// the set of the elements stored into the sequence
    std::vector<Key *, Alloc> __v;
    // note that, using Key* in __v, we actually store the Key only once in the
    // sequence (that is, within __h). This enables storing big objects within
    // sequences without having memory overhead

    /// stores the end iterator for fast access
    SequenceIteratorSafe<Key> __end_safe;

    /// stores the rend iterator for fast access
    SequenceIteratorSafe<Key> __rend_safe;

    /// a method to update the end iterator after changes in the sequence
    void __update_end() noexcept;

    /// clears the current sequence and fill it with copies the element of aSeq
    template <typename OtherAlloc>
    void __copy(const SequenceImplementation<Key, OtherAlloc, Gen> &aSeq);

    /** @brief insert an element at the end of the sequence. private version
     * for internal use */
    void __insert(HashTableBucket<Key, Idx> &&bucket);
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  /* =========================================================================== */
  /* ===          GUM_SEQUENCE_IMPLEMENTATION OPTIMIZED FOR SCALARS          === */
  /* =========================================================================== */
  /** @class SequenceImplementation
   * @brief The class for storing (ordered) sequences of scalar objects
   *
   * A Sequence<Key> is quite similar to a vector<Key> in that it stores an ordered
   * set of elements. The main difference between these two data structures lies in
   * the fact that, given a key, it is possible to retrieve from a Sequence<Key>
   * the index in the vector where the key lies in O(1). As a result, it is not
   * possible to insert a given element twice in the sequence, that is, all the
   * Keys must be different. */
  template <typename Key, typename Alloc>
  class SequenceImplementation<Key, Alloc, true> {

    template <typename K, typename A, bool> friend class SequenceImplementation;
    friend class SequenceIteratorSafe<Key>;
    friend class Sequence<Key, Alloc>;

    public:
    /// types for STL compliance
    /// @{
    using value_type = Key;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = Alloc;
    using iterator = SequenceIterator<Key>;
    using const_iterator = SequenceIterator<Key>;
    using iterator_safe = SequenceIteratorSafe<Key>;
    using const_iterator_safe = SequenceIteratorSafe<Key>;
    /// @}

    private:
    /// Default constructor
    SequenceImplementation(Size size_param = HashTableConst::default_size);

    /// initializer list constructor
    SequenceImplementation(std::initializer_list<Key> list);

    /// copy constructor
    /** @param aSeq the sequence the elements of which will be copied.
     * @warning The elements of the newly constructed sequence are copies of those
     * in aSeq */
    SequenceImplementation(const SequenceImplementation<Key, Alloc, true> &aSeq);

    /// generalized copy constructor
    template <typename OtherAlloc>
    SequenceImplementation(
        const SequenceImplementation<Key, OtherAlloc, true> &aSeq);

    /// move operator
    SequenceImplementation(SequenceImplementation<Key, Alloc, true> &&aSeq);

    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// destructor
    ~SequenceImplementation() noexcept;

    /// @}

    // ############################################################################
    /// @name Iterators
    // ############################################################################
    /// @{

    /// returns the safe begin iterator
    iterator_safe beginSafe() const;

    /// returns the safe rbegin iterator
    iterator_safe rbeginSafe() const;

    /// returns the safe end iterator
    const iterator_safe &endSafe() const noexcept;

    /// returns the safe rend iterator
    const iterator_safe &rendSafe() const noexcept;

    /// returns an unsafe begin iterator
    iterator begin() const;

    /// returns an unsafe rbegin iterator
    iterator rbegin() const;

    /// returns the unsafe end iterator
    const iterator &end() const noexcept;

    /// returns the unsafe rend iterator
    const iterator &rend() const noexcept;

    /// @}

    private:
    /// copy operator
    /** @param aSeq : the copied sequence
     * @return a ref to this */
    SequenceImplementation<Key, Alloc, true> &
    operator=(const SequenceImplementation<Key, Alloc, true> &aSeq);

    /// generalized copy operator
    template <typename OtherAlloc>
    SequenceImplementation<Key, Alloc, true> &
    operator=(const SequenceImplementation<Key, OtherAlloc, true> &aSeq);

    /// move operator
    SequenceImplementation<Key, Alloc, true> &
    operator=(SequenceImplementation<Key, Alloc, true> &&aSeq);

    public:
    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// insert k at the end of the sequence (synonym for insert)
    /** @param k the key we wish to insert in the sequence
     * @return *this
     * @throw DuplicateElement is thrown if the sequence contains already k */
    SequenceImplementation<Key, Alloc, true> &operator<<(Key k);

    /// remove k in the sequence (synonym for remove)
    /** If the element cannot be found, the function does nothing. In particular,
     * it throws no exception.
     * @param k the key we wish to remove
     * @return *this */
    SequenceImplementation<Key, Alloc, true> &operator>>(Key k);

    /// returns the element at position i (synonym for atPos)
    /** @param i
     * @throw NotFound is thrown if the element does not exist */
    const Key &operator[](Idx i) const;

    /// returns true if the content of k equals that of *this
    /** Note that two sequences are equal if and only if they contain the same
     * Keys (using Key::operator==) in the same order
     * @param k */
    template <typename OtherAlloc>
    bool operator==(const SequenceImplementation<Key, OtherAlloc, true> &k) const;

    /// returns true if the content of k is different from that of *this
    /** Note that two sequences are equal if and only if they contain the same
     * variables (using Key::operator==) in the same order
     * @param k */
    template <typename OtherAlloc>
    bool operator!=(const SequenceImplementation<Key, OtherAlloc, true> &k) const;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// clear the sequence
    void clear();

    /// returns the size of the sequence
    Size size() const noexcept;

    /// return true if empty
    bool empty() const noexcept;

    /// check the existence of k in the sequence
    /** complexity : \f$o(1)\f$ */
    bool exists(Key k) const;

    /// insert an element at the end of the sequence
    /** complexity : \f$o(1)\f$
     * @param k
     * @throw DuplicateElement is thrown if the sequence contains already k */
    void insert(Key k);

    /// emplace a new element in the sequence
    /** emplace is a method that allows to construct directly an element of
     * type Key by passing to its constructor all the arguments it needs
     * @param args the arguments passed to the constructor
     * @throw DuplicateElement is thrown if the sequence contains already k */
    template <typename... Args> void emplace(Args &&... args);

    /// remove an element from the sequence
    /** If the element cannot be found, the function does nothing. In particular,
     * it throws no exception. Complexity \f$o(n)\f$ (need to change the position
     * of at most the n elements)
     * @param k */
    void erase(Key k);

    /// remove from the sequence the element pointed to by the iterator
    /** If the element cannot be found, the function does nothing. In particular,
     * it throws no exception. Complexity \f$o(n)\f$ (need to change the position
     * of at most the n elements)
     * @param k */
    void erase(const iterator_safe &);

    /// returns the object at the pos i
    /** @param i
     * @throw NotFound is thrown if the element does not exist */
    const Key &atPos(Idx i) const;

    /// returns the position of the object passed in argument (if it exists)
    /** @throw NotFound is thrown if the element does not exist */
    Idx pos(Key k) const;

    /// change the value of the ith element of the sequence
    /** @param i
     * @param newKey
     * @throw NotFound is thrown if the element does not exist
     * @throw DuplicateElement if newKey alreay exists */
    void setAtPos(Idx i, Key newKey);

    /// swap index
    /**
     * @param i the index of the first elt to swap
     * @param j the index of the other elt to swap
     */
    void swap(Idx i, Idx j);

    /// returns the first element
    /**
     * @throw NotFound if the sequence is empty */
    const Key &front() const;

    /// returns the last element
    /**
     * @throw NotFound if the sequence is empty */
    const Key &back() const;

    /// displays the content of the sequence
    std::string toString() const;

    /// modifies the size of the internal structures of the sequence
    /** This function is provided for optimization issues. When you know you will
     * have to insert elements into the sequence, it may be faster to use this
     * function prior to the additions because it will change once and for all
     * the sizes of all the internal containers. Note that if you provide a size
     * that is smaller than the number of elements of the sequence, the function
     * will not modify anything. */
    void resize(unsigned int new_size);

    /// @}

    private:
    /// keep track of the position of the element in v (for fast retrieval)
    HashTable<Key, Idx, Alloc> __h;

    /// the set of the elements stored into the sequence
    std::vector<Key, Alloc> __v;

    /// stores the end iterator for fast access
    SequenceIteratorSafe<Key> __end_safe;

    /// stores the rend iterator for fast access
    SequenceIteratorSafe<Key> __rend_safe;

    /// a method to update the end iterator after changes in the sequence
    void __update_end() noexcept;

    /// clears the current sequence and fill it with copies the element of aSeq
    template <typename OtherAlloc>
    void __copy(const SequenceImplementation<Key, OtherAlloc, true> &aSeq);

    /** @brief insert an element at the end of the sequence. private version
     * for internal use */
    void __insert(Key k);
  };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  /* =========================================================================== */
  /* ===                             GUM_SEQUENCE                            === */
  /* =========================================================================== */
  /** @class Sequence
   * @ingroup basicstruct_group
   * @brief The generic class for storing (ordered) sequences of objects
   *
   * A Sequence<Key> is quite similar to a vector<Key> in that it stores an ordered
   * set of elements. The main difference between these two data structures lies in
   * the fact that, given a key, it is possible to retrieve from a Sequence<Key>
   * the index in the vector where the key lies in O(1). As a result, it is not
   * possible to insert a given element twice in the sequence, that is, all the
   * Keys must be different.
   *
   * @par Usage example:
   * @code
   * // creation of a sequence
   * Sequence<int> seq { 1, 2, 3, 4 };
   * Sequence<string> seq2;
   *
   * // creation of safe iterators
   * SequenceIteratorSafe<int> iter1 = seq.beginSafe (); // points to 1
   * SequenceIteratorSafe<int> iter2 = iter1;
   * SequenceIteratorSafe<int> iter3 = std::move ( iter1 );
   *
   * // creation of unsafe iterators
   * SequenceIterator<int> xiter1 = seq.begin (); // points to 1
   * SequenceIterator<int> xiter2 = xiter1;
   * SequenceIterator<int> xiter3 = std::move ( xiter1 );
   *
   * // parsing the sequence
   * for ( SequenceIterator<int> iter = seq.begin ();
   *       iter != seq.end (); ++iter )
   *   std::cout << *iter << std::endl;
   * for ( SequenceIterator<int> iter = seq.rbegin ();
   *       iter != seq.rend (); --iter )
   *   std::cout << *iter << std::endl;
   * for ( SequenceIterator<string> iter = seq.begin ();
   *       iter != seq.end (); ++iter )
   *   std::cout << iter->size () << std::endl;
   * @endcode */
  template <typename Key, typename Alloc = std::allocator<Key>>
  class Sequence
      : public SequenceImplementation<Key, Alloc, std::is_scalar<Key>::value> {
    public:
    /// types for STL compliance
    /// @{
    using value_type = Key;
    using reference = Key &;
    using const_reference = const Key &;
    using pointer = Key *;
    using const_pointer = const Key *;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = Alloc;
    using iterator = SequenceIterator<Key>;
    using const_iterator = SequenceIterator<Key>;
    using iterator_safe = SequenceIteratorSafe<Key>;
    using const_iterator_safe = SequenceIteratorSafe<Key>;
    /// @}

    using Implementation =
        SequenceImplementation<Key, Alloc, std::is_scalar<Key>::value>;

    // ############################################################################
    /// @name constructors / destructors
    // ############################################################################
    ///@{

    /// Default constructor
    Sequence(Size size_param = HashTableConst::default_size);

    /// initializer list constructor
    Sequence(std::initializer_list<Key> list);

    /// copy constructor
    /** @param aSeq the sequence the elements of which will be copied.
     * @warning The elements of the newly constructed sequence are copies of those
     * in aSeq */
    Sequence(const Sequence<Key, Alloc> &aSeq);

    /// generalized copy constructor
    template <typename OtherAlloc> Sequence(const Sequence<Key, OtherAlloc> &aSeq);

    /// move constructor
    Sequence(Sequence<Key, Alloc> &&aSeq);

    /// destructor
    ~Sequence() noexcept;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    Sequence<Key, Alloc> &operator=(const Sequence<Key, Alloc> &aSeq);

    /// generalized copy operator
    template <typename OtherAlloc>
    Sequence<Key, Alloc> &operator=(const Sequence<Key, OtherAlloc> &aSeq);

    /// move operator
    Sequence<Key, Alloc> &operator=(Sequence<Key, Alloc> &&aSeq);

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// difference between two sequences as a Set<Key> = this \ seq
    /**
     * This function gives the set difference : *this \ seq
     */
    template <typename OtherAlloc>
    Set<Key, Alloc> diffSet(const Sequence<Key, OtherAlloc> &seq) const;

    /// @}
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  // dummy classes that will enable discriminate without overhead between
  // scalars and non-scalars operators * and ->
  template <bool gen> struct SequenceIteratorGet {
    template <typename Key> INLINE static const Key &op_star(const Key *x) {
      return *x;
    }
    template <typename Key> INLINE static const Key *op_arrow(const Key *x) {
      return x;
    }
  };

  template <> struct SequenceIteratorGet<true> {
    template <typename Key> INLINE static const Key &op_star(const Key &x) {
      return x;
    }
    template <typename Key> INLINE static const Key *op_arrow(const Key &x) {
      return &x;
    }
  };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  /* =========================================================================== */
  // class SequenceIteratorSafe
  /* =========================================================================== */
  /** @class SequenceIteratorSafe
   * @brief Safe iterators for Sequence.
   * @ingroup basicstruct_group
   *
   * This iterator enables to parse the elements in the sequence. Note that this
   * is similar to a const safe iterator because the elements that can be accessed
   * in the sequence are constant.
   *
   * @par Usage example:
   * @code
   * // creation of a sequence
   * Sequence<int> seq { 1, 2, 3, 4 };
   * Sequence<string> seq2;
   *
   * // creation of iterators
   * Sequence<int>::iterator_safe iter1 = seq.beginSafe (); // points to 1
   * Sequence<int>::iterator_safe iiter2 = iter1;
   * Sequence<int>::iterator_safe iiter3 = std::move ( iter1 );
   *
   * // parsing the sequence
   * for ( typename Sequence<int>::iterator_safe iter = seq.beginSafe ();
   *       iter != seq.endSafe (); ++iter )
   *   std::cout << *iter << std::endl;
   * for ( typename Sequence<int>::iterator_safe iter = seq.rbeginSafe ();
   *       iter != seq.rend (); --iter )
   *   std::cout << *iter << std::endl;
   * for ( typename Sequence<string>::iterator_safe iter = seq.beginSafe ();
   *       iter != seq.end (); ++iter )
   *   std::cout << iter->size () << std::endl;
   * @endcode
   */
  template <typename Key> class SequenceIteratorSafe {

    template <typename K, typename A, bool> friend class SequenceImplementation;

    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = Key;
    using reference = Key &;
    using const_reference = const Key &;
    using pointer = Key *;
    using const_pointer = const Key *;
    using difference_type = std::ptrdiff_t;
    /// @}

    private:
    using Getter = SequenceIteratorGet<std::is_scalar<Key>::value>;

    /// constructor: always give a valid iterator (even if pos too large)
    /**
     * @param seq the sequence
     * @param pos indicates to which position of the sequence the iterator should
     * be pointing. By default, the iterator points to begin()
     * @warning if pos is greater than the size of the sequence, the iterator
     * is made pointing to end() */
    template <typename Alloc, bool Gen>
    SequenceIteratorSafe(const SequenceImplementation<Key, Alloc, Gen> &seq,
                         Idx pos = 0) noexcept;

    public:
    // ############################################################################
    /// @name constructors / destructors
    // ############################################################################
    ///@{

    /// constructor: always give a valid iterator (even if pos too large)
    /**
     * @param seq the sequence
     * @param pos indicates to which position of the sequence the iterator should
     * be pointing. By default, the iterator points to begin()
     * @warning if pos is greater than the size of the sequence, the iterator
     * is made pointing to end() */
    template <typename Alloc>
    SequenceIteratorSafe(const Sequence<Key, Alloc> &seq, Idx pos = 0) noexcept;

    /// copy constructor
    SequenceIteratorSafe(const SequenceIteratorSafe<Key> &source) noexcept;

    /// move constructor
    SequenceIteratorSafe(SequenceIteratorSafe<Key> &&source) noexcept;

    /// destructor
    ~SequenceIteratorSafe() noexcept;

    ///@}

    // ############################################################################
    /// @name operators
    // ############################################################################
    ///@{

    /// copy operator
    SequenceIteratorSafe<Key> &
    operator=(const SequenceIteratorSafe<Key> &source) noexcept;

    /// move operator
    SequenceIteratorSafe<Key> &
    operator=(SequenceIteratorSafe<Key> &&source) noexcept;

    /// point the iterator to the next value in the sequence
    /** @warning if the iterator already points to end(), it is unaltered. */
    SequenceIteratorSafe<Key> &operator++() noexcept;

    /// point the iterator to the preceding value in the sequence
    /** @warning if the iterator already points to rend(), it is unaltered. */
    SequenceIteratorSafe<Key> &operator--() noexcept;

    /// makes the iterator point to i elements further in the sequence
    /** @param nb the number of steps to move the iterator
     * @warning if moving the iterator nb would make it point outside the
     * Sequence, then iterator is moved to end or rend, depending on the
     * sign of nb */
    SequenceIteratorSafe<Key> &operator+=(unsigned int nb) noexcept;

    /// makes the iterator point to i elements further in the sequence
    /** @param nb the number of steps to move the iterator
     * @warning if moving the iterator nb would make it point outside the
     * Sequence, then iterator is moved to end or rend, depending on the
     * sign of nb */
    SequenceIteratorSafe<Key> &operator-=(unsigned int nb) noexcept;

    /// returns a new iterator
    /** @param nb the number of steps the created iterator is ahead of this
     * @warning the created iterator should point outside the Sequence, then it
     * is set either to end or rend, depending on the sign of nb */
    SequenceIteratorSafe<Key> operator+(unsigned int nb) noexcept;

    /// returns a new iterator
    /** @param nb the number of steps the created iterator is behind of this
     * @warning the created iterator should point outside the Sequence, then it
     * is set either to end or rend, depending on the sign of nb */
    SequenceIteratorSafe<Key> operator-(unsigned int nb) noexcept;

    /// checks whether two iterators are pointing to different elements
    bool operator!=(const SequenceIteratorSafe<Key> &source) const noexcept;

    /// checks whether two iterators are pointing to the same element
    bool operator==(const SequenceIteratorSafe<Key> &source) const noexcept;

    /// returns the value pointed to by the iterator
    ///@throw UndefinedIteratorValue on end() or rend()
    const Key &operator*() const;

    /// returns the value pointed to by the iterator (works only for non-scalars)
    const Key *operator->() const;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// returns the position of the iterator in the sequence
    /** @throw UndefinedIteratorValue on end() or rend() */
    Idx pos() const;

    /// @}

    private:
    /// the index in the sequence's vector where the iterator is pointing
    Idx __iterator;

    /// the sequence pointed to by the iterator (by default, key is a scalar)
    const SequenceImplementation<Key, std::allocator<Key>,
                                 std::is_scalar<Key>::value> *__seq;

    /// the iterator points to the posth element (0 = beginning of the sequence).
    void __setPos(Idx pos) noexcept;

    /// the iterator points to rend
    void __setAtRend() noexcept;

    /// the iterator points to the end (which is pos size()-1)
    void __setAtEnd() noexcept;
  };

  /// a << operator for displaying the content of the Sequence
  template <typename Key, typename Alloc>
  std::ostream &operator<<(std::ostream &stream, const Sequence<Key, Alloc> &s);

} /* namespace gum */

/// always include the implementation of the templates
#include <agrum/core/sequence.tcc>

#endif /* GUM_SEQUENCE_H */
