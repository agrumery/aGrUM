/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Header file of gum::Sequence, a class for storing (ordered) sequences
 * of objects.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_SEQUENCE_H
#define GUM_SEQUENCE_H

#include <initializer_list>
#include <limits>
#include <type_traits>
#include <vector>

#include <agrum/agrum.h>
#include <agrum/tools/core/set.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  template < typename Key, bool >
  class SequenceImplementation;
  template < typename Key >
  class Sequence;
  template < typename Key >
  class SequenceIteratorSafe;
  template < typename Key >
  using SequenceIterator = SequenceIteratorSafe< Key >;
  template < typename Key >
  using SequenceConstIterator = SequenceIteratorSafe< Key >;
#endif

  // ===========================================================================
  // ===                 NON SCALAR GUM SEQUENCE IMPLEMENTATION              ===
  // ===========================================================================
  /**
   * @class SequenceImplementation
   * @headerfile sequence.h <agrum/tools/core/sequence.h>
   * @brief The internal class for storing (ordered) sequences of objects.
   * @ingroup sequence_group
   *
   * A SequenceImplementation<Key,bool Gen> is a specialized version of
   * of a Sequence<Key>. It shall not be used by itself but rather
   * through the Sequence class. A SequenceImplementation is quite similar to a
   * vector<Key> in that it stores an ordered set of elements. The main
   * difference between these two data structures lies in the fact that, given
   * a key, it is possible to retrieve from a SequenceImplementation the index
   * in the vector where the key lies in O(1). As a result, it is not possible
   * to insert a given element twice in the sequence, that is, all the Keys
   * must be different.
   *
   * When the Boolean template parameter gen is false, SequenceImplementation
   * implements a very generic sequence. This allows having Sequences
   * containing elements of virtually any class or type. When the Boolean gen
   * is equal to true, the SequenceImplementation shall contain only scalar
   * types (integers, floats, pointers, etc). As such, knowning that the
   * element is a scalar enables to optimize the code of the sequences.
   * Determining whether gen should be set to true or false is not left to the
   * developper but is determined by the compiler itself at compile time.
   *
   * @tparam Key The elements type stored in the sequence.
   * @tparam Gen Used for meta-programation.
   */
  template < typename Key, bool Gen >
  class SequenceImplementation {
    /// Friends to speed up access
    /// @{
    friend class SequenceIteratorSafe< Key >;
    friend class Sequence< Key >;
    /// @}

    public:
    /// Types for STL compliance.
    /// @{
    using value_type          = Key;
    using reference           = Key&;
    using const_reference     = const Key&;
    using pointer             = Key*;
    using const_pointer       = const Key*;
    using size_type           = std::size_t;
    using difference_type     = std::ptrdiff_t;
    using iterator            = SequenceIterator< Key >;
    using const_iterator      = SequenceIterator< Key >;
    using iterator_safe       = SequenceIteratorSafe< Key >;
    using const_iterator_safe = SequenceIteratorSafe< Key >;
    /// @}

    private:
    // ============================================================================
    /// @name Constructors
    // ============================================================================
    /// @{
    /**
     * @brief Default constructor.
     * @param size_param The intial size of the gum::SequenceImplementation.
     */
    SequenceImplementation(Size size_param = HashTableConst::default_size);

    /**
     * @brief Initializer list constructor.
     * @param list The initializer list.
     */
    SequenceImplementation(std::initializer_list< Key > list);

    /**
     * @brief Copy constructor.
     *
     * @param aSeq The sequence the elements of which will be copied.
     * @warning The elements of the newly constructed sequence are copies of
     * those in aSeq.
     */
    SequenceImplementation(const SequenceImplementation< Key, Gen >& aSeq);

    /**
     * @brief Move constructor.
     * @param aSeq The gum::SequenceImplementation to move/
     */
    SequenceImplementation(SequenceImplementation< Key, Gen >&& aSeq);

    /// @}

    public:
    // ============================================================================
    /// @name Destructor
    // ============================================================================
    /// @{

    /**
     * @brief Class destructor.
     */
    ~SequenceImplementation() noexcept;

    /// @}
    // ============================================================================
    /// @name Iterators
    // ============================================================================
    /// @{

    /**
     * @brief Returns a safe begin iterator.
     * @return Returns a safe begin iterator.
     */
    iterator_safe beginSafe() const;

    /**
     * @brief Returns a safe rbegin iterator.
     * @return Returns a safe rbegin iterator.
     */
    iterator_safe rbeginSafe() const;

    /**
     * @brief Returns the safe end iterator.
     * @return Returns the safe end iterator.
     */
    const iterator_safe& endSafe() const noexcept;

    /**
     * @brief Returns the safe rend iterator.
     * @return Returns the safe rend iterator.
     */
    const iterator_safe& rendSafe() const noexcept;

    /**
     * @brief Returns an unsafe begin iterator.
     * @return Returns an unsafe begin iterator.
     */
    iterator begin() const;

    /**
     * @brief Returns an unsafe rbegin iterator.
     * @return Returns an unsafe rbegin iterator.
     */
    iterator rbegin() const;

    /**
     * @brief Returns the unsafe end iterator.
     * @return Returns the unsafe end iterator.
     */
    const iterator& end() const noexcept;

    /**
     * @brief Returns the unsafe rend iterator.
     * @return Returns the unsafe rend iterator.
     */
    const iterator& rend() const noexcept;

    /// @}

    private:
    // ============================================================================
    /// @name Private Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param aSeq The sequence to copy.
     * @return Returns a ref to this.
     */
    SequenceImplementation< Key, Gen >& operator=(const SequenceImplementation< Key, Gen >& aSeq);

    /**
     * @brief Move operator.
     * @param aSeq The sequence to move.
     * @return Returns a ref to this.
     */
    SequenceImplementation< Key, Gen >& operator=(SequenceImplementation< Key, Gen >&& aSeq);

    /// @}

    public:
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Insert k at the end of the sequence (synonym for insert).
     * @param k The key we wish to insert in the sequence.
     * @return Returns this gum::SequenceImplementation.
     * @throw DuplicateElement Raised if the sequence contains already k.
     */
    SequenceImplementation< Key, Gen >& operator<<(const Key& k);

    /**
     * @brief Insert k at the end of the sequence (synonym for insert).
     * @param k The key we wish to insert in the sequence.
     * @return Returns this gum::SequenceImplementation.
     * @throw DuplicateElement Raised if the sequence contains already k.
     */
    SequenceImplementation< Key, Gen >& operator<<(Key&& k);

    /**
     * @brief Remove k in the sequence (synonym for erase).
     *
     * If the element cannot be found, the function does nothing. In
     * particular, it throws no exception.
     *
     * @param k The key we wish to remove.
     * @return Returns this gum::SequenceImplementation.
     */
    SequenceImplementation< Key, Gen >& operator>>(const Key& k);

    /**
     * @brief Returns the element at position i (synonym for atPos).
     * @param i The position of the element to return.
     * @return Returns the element at position i.
     * @throw OutOfBounds Raised if the element does not exist.
     */
    const Key& operator[](Idx i) const;

    /**
     * @brief Returns true if the content of k equals that of *this.
     *
     * Note that two sequences are equal if and only if they contain the same
     * Keys (using Key::operator==) in the same order.
     *
     * @param k The other gum::SequenceImplementation.
     * @brief Returns true if both gum::SequenceImplementation are equal.
     */
    bool operator==(const SequenceImplementation< Key, Gen >& k) const;

    /**
     * @brief Returns true if the content of k is different from that of *this.
     *
     * Note that two sequences are equal if and only if they contain the same
     * variables (using Key::operator==) in the same order.
     *
     * @param k The other gum::SequenceImplementation.
     * @brief Returns true if both gum::SequenceImplementation are not equal.
     */
    bool operator!=(const SequenceImplementation< Key, Gen >& k) const;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Clear the sequence.
     */
    void clear();

    /**
     * @brief Returns the size of the sequence.
     * @return Returns the size of the sequence.
     */
    Size size() const noexcept;

    /**
     * @brief Return true if empty.
     * @return Return true if empty.
     */
    bool empty() const noexcept;

    /**
     * @brief Check the existence of k in the sequence.
     *
     * The complexity is \f$o(1)\f$.
     *
     * @param k The key to check for existence.
     * @return Returns true if k is in the gum::SequenceImplementation.
     */
    bool exists(const Key& k) const;

    /**
     * @brief Insert an element at the end of the sequence.
     *
     * The complexity is \f$o(1)\f$.
     *
     * @param k The element to insert.
     * @throw DuplicateElement Raised if the sequence contains already k.
     */
    void insert(const Key& k);

    /**
     * @brief Move an element at the end of the sequence.
     *
     * The complexity is \f$o(1)\f$.
     *
     * @param k The element to insert.
     * @throw DuplicateElement Raised if the sequence contains already k.
     */
    void insert(Key&& k);

    /**
     * @brief Emplace a new element in the sequence.
     *
     * The emplace is a method that allows to construct directly an element of
     * type Key by passing to its constructor all the arguments it needs.
     *
     * @tparam Args The arguments types passed to the constructor.
     * @param args The arguments passed to the constructor.
     * @throw DuplicateElement Raised if the sequence contains already k.
     */
    template < typename... Args >
    void emplace(Args&&... args);

    /**
     * @brief Remove an element from the sequence.
     *
     * If the element cannot be found, the function does nothing. In
     * particular, it throws no exception. Complexity \f$o(n)\f$ (need to
     * change the position of at most the n elements).
     *
     * @param k The element to remove.
     */
    void erase(const Key& k);

    /**
     * @brief Remove from the sequence the element pointed to by the iterator.
     *
     * If the element cannot be found, the function does nothing. In
     * particular, it throws no exception. Complexity \f$o(n)\f$ (need to
     * change the position of at most the n elements)
     *
     * @param k The iterator poiting to the element to remove.
     */
    void erase(const iterator_safe& k);

    /**
     * @brief Returns the object at the pos i.
     * @param i The position of the element to return.
     * @return Returns the object at the pos i.
     * @throw NotFound Raised if the element does not exist.
     */
    const Key& atPos(Idx i) const;

    /**
     * @brief Returns the position of the object passed in argument (if it
     * exists).
     * @param key The element for which the positon is returned.
     * @return Returns the position of the object passed in argument.
     * @throw NotFound Raised if the element does not exist.
     */
    Idx pos(const Key& key) const;

    /**
     * @brief Change the value.
     *
     * @param i The element's position.
     * @param newKey The element's new value.
     * @throw NotFound Raised if the element does not exist.
     * @throw DuplicateElement Raised if newKey alreay exists.
     */
    void setAtPos(Idx i, const Key& newKey);

    /**
     * @brief Change the value.
     *
     * @param i The element's position.
     * @param newKey The element's new value.
     * @throw NotFound Raised if the element does not exist.
     * @throw DuplicateElement Raised if newKey alreay exists.
     */
    void setAtPos(Idx i, Key&& newKey);

    /**
     * @brief Swap index.
     * @param i The index of the first elt to swap.
     * @param j The index of the other elt to swap.
     */
    void swap(Idx i, Idx j);

    /**
     * @brief Returns the first element of the element.
     * @return Returns the first element of the element.
     * @throw NotFound Raised if the sequence is empty.
     */
    const Key& front() const;

    /**
     * @brief Returns the last element of the sequence.
     * @return Returns the last element of the sequence.
     * @throw NotFound Raised if the sequence is empty.
     */
    const Key& back() const;

    /**
     * @brief Displays the content of the sequence.
     * @return The content of the sequence.
     */
    std::string toString() const;

    /**
     * @brief Modifies the size of the internal structures of the sequence.
     *
     * This function is provided for optimization issues. When you know you
     * will have to insert elements into the sequence, it may be faster to use
     * this function prior to the additions because it will change once and for
     * all the sizes of all the internal containers. Note that if you provide a
     * size that is smaller than the number of elements of the sequence, the
     * function will not modify anything.
     *
     * @param new_size The internal structure new size.
     */
    void resize(Size new_size);

    /// @}

    private:
    /// Keep track of the position of the element in v (for fast retrieval).
    HashTable< Key, Idx > _h_;

    /// The set of the elements stored into the sequence.
    std::vector< Key* > _v_;

    // Note that, using Key* in  _v_, we actually store the Key only once in the
    // sequence (that is, within  _h_). This enables storing big objects within
    // sequences without having memory overhead.

    /// Stores the end iterator for fast access.
    SequenceIteratorSafe< Key > _end_safe_;

    /// Stores the rend iterator for fast access.
    SequenceIteratorSafe< Key > _rend_safe_;

    /**
     * @brief A method to update the end iterator after changes in the
     * sequence.
     */
    void _update_end_() noexcept;

    /**
     * @brief Clears the current sequence and fill it with copies the element
     * of aSeq.
     * @param aSeq The gum::SequenceImplementation to copy.
     */
    void _copy_(const SequenceImplementation< Key, Gen >& aSeq);

    /**
     * @brief Insert an element at the end of the sequence.
     *
     * @param bucket The bucket holing the store to insert.
     */
    void _insert_(HashTableBucket< Key, Idx >&& bucket);
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  // ===========================================================================
  // ===          GUM_SEQUENCE_IMPLEMENTATION OPTIMIZED FOR SCALARS          ===
  // ===========================================================================
  /**
   * @class SequenceImplementation
   * @headerfile sequence.h <agrum/tools/core/sequence.h>
   * @brief The class for storing (ordered) sequences of scalar objects
   * @ingroup sequence_group
   *
   * A Sequence<Key> is quite similar to a vector<Key> in that it stores an
   * ordered set of elements. The main difference between these two data
   * structures lies in the fact that, given a key, it is possible to retrieve
   * from a Sequence<Key> the index in the vector where the key lies in O(1).
   * As a result, it is not possible to insert a given element twice in the
   * sequence, that is, all the Keys must be different.
   *
   * @tparam Key The elements type stored in the sequence.
   * @tapram Gen Used for meta-programation.
   */
  template < typename Key >
  class SequenceImplementation< Key, true > {
    /// Friends to speed up access
    /// @{
    friend class SequenceIteratorSafe< Key >;
    friend class Sequence< Key >;
    /// @}

    public:
    /// Types for STL compliance.
    /// @{
    using value_type          = Key;
    using size_type           = std::size_t;
    using difference_type     = std::ptrdiff_t;
    using iterator            = SequenceIterator< Key >;
    using const_iterator      = SequenceIterator< Key >;
    using iterator_safe       = SequenceIteratorSafe< Key >;
    using const_iterator_safe = SequenceIteratorSafe< Key >;
    /// @}

    private:
    // ============================================================================
    /// @name Constructors
    // ============================================================================
    /// @{
    /**
     * @brief Default constructor.
     * @param size_param The intial size of the gum::SequenceImplementation.
     */
    SequenceImplementation(Size size_param = HashTableConst::default_size);

    /**
     * @brief Initializer list constructor.
     * @param list The initializer list.
     */
    SequenceImplementation(std::initializer_list< Key > list);

    /**
     * @brief Copy constructor.
     *
     * @param aSeq The sequence the elements of which will be copied.
     * @warning The elements of the newly constructed sequence are copies of
     * those in aSeq.
     */
    SequenceImplementation(const SequenceImplementation< Key, true >& aSeq);

    /**
     * @brief Move constructor.
     * @param aSeq The gum::SequenceImplementation to move/
     */
    SequenceImplementation(SequenceImplementation< Key, true >&& aSeq);

    /// @}
    public:
    // ============================================================================
    /// @name Destructor
    // ============================================================================
    /// @{

    /**
     * @brief Class destructor.
     */
    ~SequenceImplementation() noexcept;

    /// @}
    // ============================================================================
    /// @name Iterators
    // ============================================================================
    /// @{

    /**
     * @brief Returns a safe begin iterator.
     * @return Returns a safe begin iterator.
     */
    iterator_safe beginSafe() const;

    /**
     * @brief Returns a safe rbegin iterator.
     * @return Returns a safe rbegin iterator.
     */
    iterator_safe rbeginSafe() const;

    /**
     * @brief Returns the safe end iterator.
     * @return Returns the safe end iterator.
     */
    const iterator_safe& endSafe() const noexcept;

    /**
     * @brief Returns the safe rend iterator.
     * @return Returns the safe rend iterator.
     */
    const iterator_safe& rendSafe() const noexcept;

    /**
     * @brief Returns an unsafe begin iterator.
     * @return Returns an unsafe begin iterator.
     */
    iterator begin() const;

    /**
     * @brief Returns an unsafe rbegin iterator.
     * @return Returns an unsafe rbegin iterator.
     */
    iterator rbegin() const;

    /**
     * @brief Returns the unsafe end iterator.
     * @return Returns the unsafe end iterator.
     */
    const iterator& end() const noexcept;

    /**
     * @brief Returns the unsafe rend iterator.
     * @return Returns the unsafe rend iterator.
     */
    const iterator& rend() const noexcept;

    /// @}

    private:
    // ============================================================================
    /// @name Private Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param aSeq The sequence to copy.
     * @return Returns a ref to this.
     */
    SequenceImplementation< Key, true >& operator=(const SequenceImplementation< Key, true >& aSeq);

    /**
     * @brief Move operator.
     * @param aSeq The sequence to move.
     * @return Returns a ref to this.
     */
    SequenceImplementation< Key, true >& operator=(SequenceImplementation< Key, true >&& aSeq);

    /// @}

    public:
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Insert k at the end of the sequence (synonym for insert).
     * @param k The key we wish to insert in the sequence.
     * @return Returns this gum::SequenceImplementation.
     * @throw DuplicateElement Raised if the sequence contains already k.
     */
    SequenceImplementation< Key, true >& operator<<(Key k);

    /**
     * @brief Remove k in the sequence (synonym for erase).
     *
     * If the element cannot be found, the function does nothing. In
     * particular, it throws no exception.
     *
     * @param k The key we wish to remove.
     * @return Returns this gum::SequenceImplementation.
     */
    SequenceImplementation< Key, true >& operator>>(Key k);

    /**
     * @brief Returns the element at position i (synonym for atPos).
     * @param i The position of the element to return.
     * @return Returns the element at position i.
     * @throw OutOfBounds Raised if the element does not exist.
     */
    const Key& operator[](Idx i) const;

    /**
     * @brief Returns true if the content of k equals that of *this.
     *
     * Note that two sequences are equal if and only if they contain the same
     * Keys (using Key::operator==) in the same order.
     *
     * @param k The other gum::SequenceImplementation.
     * @brief Returns true if both gum::SequenceImplementation are equal.
     */
    bool operator==(const SequenceImplementation< Key, true >& k) const;

    /**
     * @brief Returns true if the content of k is different from that of *this.
     *
     * Note that two sequences are equal if and only if they contain the same
     * variables (using Key::operator==) in the same order.
     *
     * @param k The other gum::SequenceImplementation.
     * @brief Returns true if both gum::SequenceImplementation are not equal.
     */
    bool operator!=(const SequenceImplementation< Key, true >& k) const;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Clear the sequence.
     */
    void clear();

    /**
     * @brief Returns the size of the sequence.
     * @return Returns the size of the sequence.
     */
    Size size() const noexcept;

    /**
     * @brief Return true if empty.
     * @return Return true if empty.
     */
    bool empty() const noexcept;

    /**
     * @brief Check the existence of k in the sequence.
     *
     * The complexity is \f$o(1)\f$.
     *
     * @param k The key to check for existence.
     * @return Returns true if k is in the gum::SequenceImplementation.
     */
    bool exists(Key k) const;

    /**
     * @brief Insert an element at the end of the sequence.
     *
     * The complexity is \f$o(1)\f$.
     *
     * @param k The element to insert.
     * @throw DuplicateElement Raised if the sequence contains already k.
     */
    void insert(Key k);

    /**
     * @brief Emplace a new element in the sequence.
     *
     * The emplace is a method that allows to construct directly an element of
     * type Key by passing to its constructor all the arguments it needs.
     *
     * @tparam Args The arguments types passed to the constructor.
     * @param args The arguments passed to the constructor.
     * @throw DuplicateElement Raised if the sequence contains already k.
     */
    template < typename... Args >
    void emplace(Args&&... args);

    /**
     * @brief Remove an element from the sequence.
     *
     * If the element cannot be found, the function does nothing. In
     * particular, it throws no exception. Complexity \f$o(n)\f$ (need to
     * change the position of at most the n elements).
     *
     * @param k The element to remove.
     */
    void erase(Key k);

    /**
     * @brief Remove from the sequence the element pointed to by the iterator.
     *
     * If the element cannot be found, the function does nothing. In
     * particular, it throws no exception. Complexity \f$o(n)\f$ (need to
     * change the position of at most the n elements)
     *
     * @param k The iterator poiting to the element to remove.
     */
    void erase(const iterator_safe& k);

    /**
     * @brief Returns the object at the pos i.
     * @param i The position of the element to return.
     * @return Returns the object at the pos i.
     * @throw NotFound Raised if the element does not exist.
     */
    const Key& atPos(Idx i) const;

    /**
     * @brief Returns the position of the object passed in argument (if it
     * exists).
     * @param key The element for which the positon is returned.
     * @rerurn Returns the position of the object passed in argument.
     * @throw NotFound Raised if the element does not exist.
     */
    Idx pos(Key key) const;

    /**
     * @brief Change the value.
     *
     * @param i The element's position.
     * @param newKey The element's new value.
     * @throw NotFound Raised if the element does not exist.
     * @throw DuplicateElement Raised if newKey alreay exists.
     */
    void setAtPos(Idx i, Key newKey);

    /**
     * @brief Swap index.
     * @param i The index of the first elt to swap.
     * @param j The index of the other elt to swap.
     */
    void swap(Idx i, Idx j);

    /**
     * @brief Returns the first element of the element.
     * @return Returns the first element of the element.
     * @throw NotFound Raised if the sequence is empty.
     */
    const Key& front() const;

    /**
     * @brief Returns the last element of the sequence.
     * @return Returns the last element of the sequence.
     * @throw NotFound Raised if the sequence is empty.
     */
    const Key& back() const;

    /**
     * @brief Displays the content of the sequence.
     * @return The content of the sequence.
     */
    std::string toString() const;

    /**
     * @brief Modifies the size of the internal structures of the sequence.
     *
     * This function is provided for optimization issues. When you know you
     * will have to insert elements into the sequence, it may be faster to use
     * this function prior to the additions because it will change once and for
     * all the sizes of all the internal containers. Note that if you provide a
     * size that is smaller than the number of elements of the sequence, the
     * function will not modify anything.
     *
     * @param new_size The internal structure new size.
     */
    void resize(Size new_size);

    /// @}

    private:
    /// Keep track of the position of the element in v (for fast retrieval).
    HashTable< Key, Idx > _h_;

    /// The set of the elements stored into the sequence.
    std::vector< Key > _v_;

    /// Stores the end iterator for fast access.
    SequenceIteratorSafe< Key > _end_safe_;

    /// Stores the rend iterator for fast access.
    SequenceIteratorSafe< Key > _rend_safe_;

    /**
     * @brief A method to update the end iterator after changes in the
     * sequence.
     */
    void _update_end_() noexcept;

    /**
     * @brief Clears the current sequence and fill it with copies the element
     * of aSeq.
     * @para aSeq The gum::SequenceImplementation to copy.
     */
    void _copy_(const SequenceImplementation< Key, true >& aSeq);

    /**
     * @brief Insert an element at the end of the sequence.
     *
     * @param bucket The bucket holing the store to insert.
     */
    void _insert_(Key k);
  };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  // ===========================================================================
  // ===                             GUM_SEQUENCE                            ===
  // ===========================================================================
  /**
   * @class Sequence
   * @headerfile sequence.h <agrum/tools/core/sequence.h>
   * @brief The generic class for storing (ordered) sequences of objects.
   * @ingroup basicstruct_group
   * @ingroup sequence_group
   *
   * A gum::Sequence<Key> is quite similar to a std::vector<Key> in that it
   * stores an ordered set of elements. The main difference between these two
   * data structures lies in the fact that, given a key, it is possible to
   * retrieve from a gum::Sequence<Key> the index in the vector where the key
   * lies in O(1).  As a result, it is not possible to insert a given element
   * twice in the sequence, that is, all the Keys must be different.
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
   * for ( auto iter = seq.begin (); iter != seq.end (); ++iter )
   *   std::cout << *iter << std::endl;
   * for ( auto iter = seq.rbegin (); iter != seq.rend (); --iter )
   *   std::cout << *iter << std::endl;
   * for ( auto iter = seq.begin (); iter != seq.end (); ++iter )
   *   std::cout << iter->size () << std::endl;
   * @endcode
   *
   * @tparam Key The elements type stored in the sequence.
   */
  template < typename Key >
  class Sequence: public SequenceImplementation< Key, std::is_scalar< Key >::value > {
    public:
    /// Types for STL compliance.
    /// @{
    using value_type          = Key;
    using reference           = Key&;
    using const_reference     = const Key&;
    using pointer             = Key*;
    using const_pointer       = const Key*;
    using size_type           = std::size_t;
    using difference_type     = std::ptrdiff_t;
    using iterator            = SequenceIterator< Key >;
    using const_iterator      = SequenceIterator< Key >;
    using iterator_safe       = SequenceIteratorSafe< Key >;
    using const_iterator_safe = SequenceIteratorSafe< Key >;
    /// @}

    /// The gum::Sequence implementation.
    using Implementation = SequenceImplementation< Key, std::is_scalar< Key >::value >;

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    ///@{

    /**
     * @brief Default constructor.
     * @param size_param The intial size of the gum::SequenceImplementation.
     */
    Sequence(Size size_param = HashTableConst::default_size);

    /**
     * @brief Initializer list constructor.
     * @param list The initializer list.
     */
    Sequence(std::initializer_list< Key > list);

    /**
     * @brief Copy constructor.
     *
     * @param aSeq The sequence the elements of which will be copied.
     * @warning The elements of the newly constructed sequence are copies of
     * those in aSeq.
     */
    Sequence(const Sequence< Key >& aSeq);

    /**
     * @brief Move constructor.
     * @param aSeq The gum::Sequence to move/
     */
    Sequence(Sequence< Key >&& aSeq);

    /**
     * @brief Class destructor.
     */
    ~Sequence() noexcept;

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param aSeq The sequence to copy.
     * @return Returns a ref to this.
     */
    Sequence< Key >& operator=(const Sequence< Key >& aSeq);

    /**
     * @brief Move operator.
     * @param aSeq The sequence to move.
     * @return Returns a ref to this.
     */
    Sequence< Key >& operator=(Sequence< Key >&& aSeq);

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Difference between two sequences as a Set<Key> = this \ seq.
     *
     * @param seq The gum::Sequence to substract of this.
     * @return Returns the set difference : *this \ seq.
     */
    Set< Key > diffSet(const Sequence< Key >& seq) const;

    /// @}
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  // dummy classes that will enable discriminate without overhead between
  // scalars and non-scalars operators * and ->
  template < bool gen >
  struct SequenceIteratorGet {
    template < typename Key >
    INLINE static const Key& op_star(const Key* x) {
      return *x;
    }
    template < typename Key >
    INLINE static const Key* op_arrow(const Key* x) {
      return x;
    }
  };

  template <>
  struct SequenceIteratorGet< true > {
    template < typename Key >
    INLINE static const Key& op_star(const Key& x) {
      return x;
    }
    template < typename Key >
    INLINE static const Key* op_arrow(const Key& x) {
      return &x;
    }
  };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  // ===========================================================================
  // class SequenceIteratorSafe
  // ===========================================================================
  /**
   * @class SequenceIteratorSafe
   * @headerfile sequence.h <agrum/tools/core/sequence.h>
   * @brief Safe iterators for Sequence.
   * @ingroup sequence_group
   *
   * This iterator enables to parse the elements in the sequence. Note that
   * this is similar to a const safe iterator because the elements that can be
   * accessed in the sequence are constant.
   *
   * @par Usage example:
   * @code
   * // creation of a sequence
   * Sequence<int> seq { 1, 2, 3, 4 };
   * Sequence<string> seq2;
   *
   * // creation of iterators
   * auto iter1 = seq.beginSafe (); // points to 1
   * auto iiter2 = iter1;
   * auto iiter3 = std::move ( iter1 );
   *
   * // parsing the sequence
   * for ( auto iter = seq.beginSafe (); iter != seq.endSafe (); ++iter )
   *   std::cout << *iter << std::endl;
   * for ( auto iter = seq.rbeginSafe (); iter != seq.rend (); --iter )
   *   std::cout << *iter << std::endl;
   * for ( auto iter = seq.beginSafe (); iter != seq.end (); ++iter )
   *   std::cout << iter->size () << std::endl;
   * @endcode
   *
   * @tparam Key The type of elements stored in the gum::Sequence.
   */
  template < typename Key >
  class SequenceIteratorSafe {
    /// Friend to speed up access.
    template < typename K, bool >
    friend class SequenceImplementation;

    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = Key;
    using reference         = Key&;
    using const_reference   = const Key&;
    using pointer           = Key*;
    using const_pointer     = const Key*;
    using difference_type   = std::ptrdiff_t;
    /// @}

    private:
    /// The Getter used by this iterator.
    using Getter = SequenceIteratorGet< std::is_scalar< Key >::value >;


    /**
     * @brief Constructor, always give a valid iterator (even if pos too
     * large).
     *
     * @warning if pos is greater than the size of the sequence, the iterator
     * is made pointing to end().
     *
     * @tparam Gen Used for meta-programation.
     * @param seq The sequence.
     * @param pos Indicates to which position of the sequence the iterator
     * should be pointing. By default, the iterator points to begin().
     */
    template < bool Gen >
    SequenceIteratorSafe(const SequenceImplementation< Key, Gen >& seq, Idx pos = 0) noexcept;

    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    ///@{


    /**
     * @brief Constructor, always give a valid iterator (even if pos too
     * large).
     *
     * @warning if pos is greater than the size of the sequence, the iterator
     * is made pointing to end().
     *
     * @param seq the sequence
     * @param pos indicates to which position of the sequence the iterator
     * should be pointing. By default, the iterator points to begin()
     */
    SequenceIteratorSafe(const Sequence< Key >& seq, Idx pos = 0) noexcept;

    /**
     * @brief Copy constructor.
     * @param source The iterator to copy.
     */
    SequenceIteratorSafe(const SequenceIteratorSafe< Key >& source) noexcept;

    /**
     * @brief Move constructor.
     * @param source The iterator to move.
     */
    SequenceIteratorSafe(SequenceIteratorSafe< Key >&& source) noexcept;

    /**
     * @brief Class destructor.
     */
    ~SequenceIteratorSafe() noexcept;

    ///@}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    ///@{

    /**
     * @brief Copy operator.
     * @param source The iterator to copy.
     * @return Returns this iterator.
     */
    SequenceIteratorSafe< Key >& operator=(const SequenceIteratorSafe< Key >& source) noexcept;

    /**
     * @brief Move operator.
     * @param source The iterator to move.
     * @return Returns this iterator.
     */
    SequenceIteratorSafe< Key >& operator=(SequenceIteratorSafe< Key >&& source) noexcept;

    /**
     * @brief Point the iterator to the next value in the sequence.
     *
     * @warning if the iterator already points to end(), it is unaltered.
     * @return Returns this iterator.
     */
    SequenceIteratorSafe< Key >& operator++() noexcept;

    /**
     * @brief Point the iterator to the preceding value in the sequence.
     *
     * @warning If the iterator already points to rend(), it is unaltered.
     *
     * @return Returns this iterator.
     */
    SequenceIteratorSafe< Key >& operator--() noexcept;

    /**
     * @brief Makes the iterator point to i elements further in the sequence.
     *
     * @warning If moving the iterator nb would make it point outside the
     * Sequence, then iterator is moved to end or rend, depending on the sign
     * of nb.
     *
     * @param nb The number of steps to move the iterator.
     * @return Returns this iterator.
     */
    SequenceIteratorSafe< Key >& operator+=(Size nb) noexcept;

    /**
     * @brief Makes the iterator point to i elements further in the sequence.
     *
     * @warning If moving the iterator nb would make it point outside the
     * Sequence, then iterator is moved to end or rend, depending on the
     * sign of nb.
     *
     * @param nb The number of steps to move the iterator.
     * @return Returns this iterator.
     */
    SequenceIteratorSafe< Key >& operator-=(Size nb) noexcept;

    /**
     * @brief Returns a new iterator.
     *
     * @warning The created iterator should point outside the Sequence, then it
     * is set either to end or rend, depending on the sign of nb.
     *
     * @param nb The number of steps the created iterator is ahead of this.
     * @return Returns a new iterator.
     */
    SequenceIteratorSafe< Key > operator+(Size nb) noexcept;

    /**
     * @brief Returns a new iterator.
     *
     * @warning The created iterator should point outside the Sequence, then it
     * is set either to end or rend, depending on the sign of nb.
     *
     * @param nb The number of steps the created iterator is behind of this.
     * @brief Returns a new iterator.
     */
    SequenceIteratorSafe< Key > operator-(Size nb) noexcept;

    /**
     * @brief Checks whether two iterators are pointing to different elements.
     *
     * @param source The iterator to test for inequality.
     * @return Returns true if both iterators are not equal.
     */
    bool operator!=(const SequenceIteratorSafe< Key >& source) const noexcept;

    /**
     * @brief Checks whether two iterators are pointing to the same elements.
     *
     * @param source The iterator to test for equality.
     * @return Returns true if both iterators are equal.
     */
    bool operator==(const SequenceIteratorSafe< Key >& source) const noexcept;

    /**
     * @brief Returns the value pointed to by the iterator.
     * @return Returns the value pointed to by the iterator.
     * @throw UndefinedIteratorValue Raised on end() or rend().
     */
    const Key& operator*() const;

    /**
     * @brief Returns the value pointed to by the iterator (works only for
     * non-scalars).
     * @return Returns the value pointed to by the iterator (works only for
     * non-scalars).
     */
    const Key* operator->() const;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Returns the position of the iterator in the sequence.
     * @return Returns the position of the iterator in the sequence.
     * @throw UndefinedIteratorValue Raised on end() or rend().
     */
    Idx pos() const;

    /// @}

    private:
    /// The index in the sequence's vector where the iterator is pointing.
    Idx _iterator_;

    /// The sequence pointed to by the iterator (by default, key is a scalar).
    const SequenceImplementation< Key, std::is_scalar< Key >::value >* _seq_;

    /// The iterator points to the posth element (0 = beginning of the
    /// sequence).
    void _setPos_(Idx pos) noexcept;

    /// The iterator points to rend.
    void _setAtRend_() noexcept;

    /// The iterator points to the end (which is pos size()-1).
    void _setAtEnd_() noexcept;
  };

  /// @brief A << operator for displaying the content of the Sequence.
  template < typename Key >
  std::ostream& operator<<(std::ostream& stream, const Sequence< Key >& s);

} /* namespace gum */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::Sequence< int >;
extern template class gum::Sequence< long >;
extern template class gum::Sequence< double >;
extern template class gum::Sequence< std::string >;
#endif


// always include the implementation of the templates
#include <agrum/tools/core/sequence_tpl.h>

#endif   // GUM_SEQUENCE_H
