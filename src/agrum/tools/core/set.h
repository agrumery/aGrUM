/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Sets of elements (i.e. the mathematical notion of a set).
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_SET_H
#define GUM_SET_H

#include <initializer_list>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/tools/core/debug.h>
#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/core/list.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  template < typename Key >
  class SetIteratorSafe;
  template < typename Key >
  class SetIterator;
  template < typename Key >
  class Set;

  template < typename Key >
  using SetConstIterator = SetIterator< Key >;
  template < typename Key >
  using SetConstIteratorSafe = SetIteratorSafe< Key >;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


  // ===========================================================================
  // ===                               GUM_SET                               ===
  // ===========================================================================

  /**
   * @class Set
   * @headerfile set.h <agrum/tools/core/set.h>
   * @brief Representation of a set
   * @ingroup basicstruct_group
   * @ingroup set_group
   *
   * A Set is a structure that contains arbitrary elements. Note that, as in
   * mathematics, an element cannot appear twice in a given set. Sets have
   * unsafe and safe iterators. The safe iterators (SetIteratorSafe<> a.k.a.
   * Set<>::iterator_safe are slightly slower than the unsafe ones
   * (SetIterator<> a.k.a. Set<>::iterator) but they guarantee that even if
   * they point to a deleted element, using their operators ++ or * cannot
   * produce a segfault. In such cases, they simply raise an exception. On the
   * contrary, unsafe iterators should @b never be used on elements that can be
   * deleted because, as in the STL, they will most probably produce a
   * segfault.
   *
   * @par Usage example:
   * @code
   * // creation of a set with 10 elements
   * Set<int> set;
   * for (int i = 0; i< 10; ++i)
   *   set<<i;
   * Set<int> set2 { 1, 2, 3 };
   *
   * // parse the set
   * for (const auto iter = set.begin (); iter != set.end (); ++iter) {
   *   // display the values
   *   cerr <<  *iter << endl;
   * }
   *
   * // use an iterator to point the element we wish to erase
   * Set<int>::iterator iter = set.begin ();
   * set.erase ( iter );
   *
   * // check whether two iterators point toward the same element
   * Set<int>::iterator iter1 = set.begin();
   * Set<int>::iterator iter2 = set.end();
   * if (iter1 != iter2)
   *   cerr << "iter1 and iter2 point toward different elements";
   *
   * @endcode
   *
   * @tparam Key The elements type.
   */
  template < typename Key >
  class Set {
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
    using iterator            = SetIterator< Key >;
    using const_iterator      = SetIterator< Key >;
    using iterator_safe       = SetIteratorSafe< Key >;
    using const_iterator_safe = SetIteratorSafe< Key >;
    /// @}

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     *
     * Sets rely on hashtables to store their items. The optional parameters of
     * this constructor enable a fine memory management of these hashtables.
     *
     * @param capacity The number of slots allocated to the hashtable (see the
     * HashTable default constructor)
     * @param resize_policy Enables the hashtable to resize itself
     * automatically when its number of elements is sufficiently high that it
     * induces slow retrievals of elements.
     */
    explicit Set(Size capacity = HashTableConst::default_size, bool resize_policy = true);

    /**
     * @brief Initializer list constructor.
     * @param list The initializer list.
     */
    Set(std::initializer_list< Key > list);

    /**
     * @brief Copy constructor.
     * @param aHT The gum::Set to copy.
     */
    Set(const Set< Key >& aHT);

    /**
     * @brief Move constructor.
     * @param aHT The gum::Set to move.
     */
    Set(Set< Key >&& aHT);

    /**
     * @brief Class destructor.
     */
    ~Set();

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param from The gum::Set to copy.
     * @return Returns this gum::Set.
     */
    Set< Key >& operator=(const Set< Key >& from);

    /**
     * @brief Move operator.
     * @param from The gum::Set to move.
     * @return Returns this gum::Set.
     */
    Set< Key >& operator=(Set< Key >&& from);

    /**
     * @brief Mathematical equality between two sets.
     * @param s2 The gum::Set to test for equality.
     * @return Returns true if both gum::Set are equal.
     */
    bool operator==(const Set< Key >& s2) const;

    /**
     * @brief Mathematical inequality between two sets.
     * @param s2 The gum::Set to test for inequality.
     * @return Returns true if both gum::Set are not equal.
     */
    bool operator!=(const Set< Key >& s2) const;

    /**
     * @brief Intersection update operator
     * @param s2 The gum::Set to intersect.
     * @return Returns this. Now this contains the elements belonging
     * both to this and s2.
     */
    const Set< Key >& operator*=(const Set< Key >& s2);

    /**
     * @brief Intersection operator.
     * @param s2 The gum::Set to intersect.
     * @return Returns a Set containing the elements belonging both to this and
     * s2.
     */
    Set< Key > operator*(const Set< Key >& s2) const;

    /**
     * @brief Union update operator
     * @param s2 The gum::Set to update
     * @return Returns this. Now this contains the elements belonging
     * both to this or to s2.
     */
    const Set< Key >& operator+=(const Set< Key >& s2);

    /**
     * @brief Union operator.
     * @param s2 The gum::Set to union.
     * @return Returns a new Set containing the union of the elements of this
     * and s2.
     */
    Set< Key > operator+(const Set< Key >& s2) const;


    /**
     * @brief Disjunction operator.
     * @param s2 The gum::Set to disjunct.
     * @return Returns a Set whose elements belong to this but not to s2.
     * @warning Unlike + and *, the - operator is not commutative!
     */
    Set< Key > operator-(const Set< Key >& s2) const;

    /**
     * @brief Adds a new element to the set (alias for insert).
     * @param k The new element to add.
     * @return Returns this gum::Set.
     */
    Set< Key >& operator<<(const Key& k);

    /**
     * @brief Adds a new element to the set (alias for insert).
     * @param k The new element to add.
     * @return Returns this gum::Set.
     */
    Set< Key >& operator<<(Key&& k);

    /**
     * @brief Removes an element from the set (alias for erase).
     * @param k The element to remove.
     * @return Return this gum::Set.
     */
    Set< Key >& operator>>(const Key& k);

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Inserts a new element into the set.
     * @param k The new element to insert.
     * @warning if the set already contains the element, nothing is done. In
     * particular, it is not added to the set and no exception is thrown.
     */
    void insert(const Key& k);

    /**
     * @brief Inserts a new element into the set.
     * @param k The new element to insert.
     * @warning if the set already contains the element, nothing is done.  In
     * particular, it is not added to the set and no exception is thrown.
     */
    void insert(Key&& k);

    /**
     * @brief Emplace a new element in the set.
     *
     * Emplace is a method that allows to construct directly an element of type
     * Key by passing to its constructor all the arguments it needs.
     *
     * @param args the arguments passed to the constructor
     * @warning if the set already contains the element, nothing is done. In
     * particular, it is not added to the set and no exception is thrown.
     */
    template < typename... Args >
    void emplace(Args&&... args);

    /**
     * @brief Erases an element from the set.
     *
     * @param k The element to remove.
     * @warning if the set does not contain the element, nothing is done.  In
     * particular, no exception is thrown.
     */
    void erase(const Key& k);

    /**
     * @brief Erases an element from the set.
     * @param k The iterator pointing to the element to remove.
     * @warning if the set does not contain the element, nothing is done.  In
     * particular, no exception is thrown.
     */
    void erase(const iterator_safe& k);

    /**
     * @brief Removes all the elements, if any, from the set.
     */
    void clear();

    /**
     * @brief Returns the number of elements in the set.
     * @return Returns the number of elements in the set.
     */
    Size size() const noexcept;

    /**
     * @brief Indicates whether a given elements belong to the set.
     * @return Returns true if a given elements belong to the set.
     */
    bool contains(const Key& k) const;

    /**
     * @return Returns true if *this is a proper subset of s
     */
    bool isProperSubsetOf(const Set< Key >& s) const;

    /**
     * @return Returns true if *this is a proper superset of s
     */
    bool isProperSupersetOf(const Set< Key >& s) const;

    /**
     * @return Returns true if *this is a subset of s (or equal to s)
     */
    bool isSubsetOrEqual(const Set< Key >& s) const;

    /**
     * @return Returns true if *this is a superset of s (or equal to s)
     */
    bool isSupersetOrEqual(const Set< Key >& s) const;

    /**
     * @brief Indicates whether a given elements belong to the set.
     * @return Returns true if a given elements belong to the set.
     */
    bool exists(const Key& k) const;

    /**
     * @brief Indicates whether the set is the empty set.
     * @return Returns true if the set is empty.
     */
    bool empty() const noexcept;

    /**
     * @brief Prints the content of the set.
     * @return Returns the content of the set.
     */
    std::string toString() const;

    /// @}
    // ============================================================================
    /// @name Iterators
    // ============================================================================
    /// @{

    /**
     * @brief The usual safe begin iterator to parse the set.
     * @return Returns The usual safe begin iterator to parse the set.
     */
    iterator_safe beginSafe() const;

    /**
     * @brief The usual safe begin iterator to parse the set.
     * @return Returns the usual safe begin iterator to parse the set.
     */
    const_iterator_safe cbeginSafe() const;

    /**
     * @brief The usual safe end iterator to parse the set.
     * @return Returns the usual safe end iterator to parse the set.
     */
    const iterator_safe& endSafe() const noexcept;

    /**
     * @brief The usual safe end iterator to parse the set.
     * @return Returns the usual safe end iterator to parse the set.
     */
    const const_iterator_safe& cendSafe() const noexcept;

    /**
     * @brief The usual unsafe begin iterator to parse the set.
     * @return Returns the usual unsafe begin iterator to parse the set.
     */
    iterator begin() const;

    /**
     * @brief The usual unsafe begin iterator to parse the set.
     * @return Returns the usual unsafe begin iterator to parse the set.
     */
    const_iterator cbegin() const;

    /**
     * @brief The usual unsafe end iterator to parse the set.
     * @return Returns the usual unsafe end iterator to parse the set.
     */
    const iterator& end() const noexcept;

    /**
     * @brief The usual unsafe end iterator to parse the set.
     * @return Returns the usual unsafe end iterator to parse the set.
     */
    const const_iterator& cend() const noexcept;

    /// @}

    // ============================================================================
    /// @name Fine tuning
    // ============================================================================
    /// @{

    /**
     * @brief Returns the capacity of the underlying hash table containing the
     * set.
     *
     * The method runs in constant time.
     *
     * @return Returns the capacity of the underlying hash table containing the
     * set.
     */
    Size capacity() const;

    /**
     * @brief Changes the size of the underlying hash table containing the set.
     *
     * See gum::HashTable::resize(Size) method resize for more details.
     *
     * @param new_capacity The underlying hash table new size.
     */
    void resize(Size new_capacity);

    /**
     * @brief Enables the user to change dynamically the resizing policy of the
     * underlying hash table.
     *
     * When new_policy is false, the set will not try to change its memory
     * size, hence resulting in potentially slower accesses.
     *
     * @param new_policy If true the set updates dynamically its memory
     * consumption to guarantee that its elements are fast to retrieve.
     */
    void setResizePolicy(const bool new_policy);

    /**
     * @brief Returns the current resizing policy of the underlying hash table.
     * @return Returns the current resizing policy of the underlying hash
     * table.
     */
    bool resizePolicy() const;

    ///  @}
    // ============================================================================
    /// @name Mapper
    // ============================================================================
    /// @{

    /**
     * @brief Creates a hashtable of NewKey from the set.
     *
     * @warning The order in the resulting hashtable may not be similar to that
     * of the original set. Hence iterators on the former may not parse it in
     * the same order as iterators on the latter.
     *
     * @param f A function that maps Key into a NewKey
     * @param capacity The size of the resulting hashtable. When equal to 0, a
     * default size is computed that is a good trade-off between space
     * consumption and efficiency of new elements insertions.
     */
    template < typename NewKey >
    HashTable< Key, NewKey > hashMap(NewKey (*f)(const Key&), Size capacity = 0) const;

    /**
     * @brief Creates a hash table of NewKey from the set.
     *
     * @warning The order in the resulting hash table may not be similar to
     * that of the original set. Hence iterators on the former may not parse it
     * in the same order as iterators on the latter.
     *
     * @param val The value taken by all the elements of the resulting
     * hashtable.
     * @param size The size of the resulting hash table. When equal to 0, a
     * default size is computed that is a good trade-off between space
     * consumption and efficiency of new elements insertions.
     */
    template < typename NewKey >
    HashTable< Key, NewKey > hashMap(const NewKey& val, Size size = 0) const;

    /**
     * @brief A method to create a List of NewKey from the set.
     *
     * @warning The order of the NewKey elements in the resulting list is
     * arbitrary.
     *
     * @param f A function that maps a Key into a NewKey
     */
    template < typename NewKey >
    List< NewKey > listMap(NewKey (*f)(const Key&)) const;

    /// @}

    private:
    /// Friends to speed up access
    /// @{
    friend class SetIterator< Key >;
    friend class SetIteratorSafe< Key >;
    /// @}

    /// A set of X's is actually a hash table whose keys are the X's.
    HashTable< Key, bool > _inside_;

    /// Convert a hash table into a set of keys.
    Set(const HashTable< Key, bool >& h);
  };


  // ===========================================================================
  // ===                          SAFE SET ITERATORS                         ===
  // ===========================================================================

  /**
   * @class SetIteratorSafe
   * @headerfile set.h <agrum/tools/core/set.h>
   * @brief Safe iterators for the Set class
   * @ingroup set_group
   *
   * Developers may consider using Set<x>::iterator_safe instead of
   * SetIteratorSafe<x>.
   *
   * @par Usage example:
   * @code
   * // creation of a set with 10 elements
   * Set<int> set;
   * for (int i = 0; i< 10; ++i)
   *   set<<i;
   *
   * // parse the set
   * for (const auto iter = table.beginSafe (); iter != table.endSafe (); ++iter) {
   *   // display the values
   *   cerr <<  *iter << endl;
   * }
   *
   * // check whether two iterators point toward the same element
   * Set<int>::iterator_safe iter1 = table1.beginSafe();
   * Set<int>::iterator_safe iter2 = table1.endSafe();
   * if (iter1 != iter)
   *   cerr << "iter1 and iter2 point toward different elements";
   *
   * @endcode
   *
   * @tparam Key The elements type.
   */
  template < typename Key >
  class SetIteratorSafe {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using value_type        = Key;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using difference_type   = std::ptrdiff_t;
    /// @}

    /**
     * @brief An enumeration to position the iterator at the beginning or the
     * end of the set.
     */
    enum Position {
      BEGIN,
      END
    };

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor: the iterator points toward nothing.
     */
    explicit SetIteratorSafe();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static endSafe iterator
    // only set.cpp should use this constructor
    explicit consteval SetIteratorSafe(StaticInitializer init) noexcept : _ht_iter_(init) {}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /**
     * @brief Creates an iterator for a given set.
     *
     * By default, the iterator points to the beginning of the set, but, using
     * optional argument pos, you can make it point to end().
     *
     * @param from The gum::Set to iterate over.
     * @param pos Where to start iterating.
     */
    SetIteratorSafe(const Set< Key >& from, Position pos = BEGIN);

    /**
     * @brief Copy constructor.
     * @param from The iterator to copy.
     */
    SetIteratorSafe(const SetIteratorSafe< Key >& from);

    /**
     * @brief Copy constructor.
     * @param from The iterator to copy.
     */
    explicit SetIteratorSafe(const SetIterator< Key >& from);

    /**
     * @brief Move constructor.
     * @param from The iterator to move.
     */
    SetIteratorSafe(SetIteratorSafe< Key >&& from);

    /**
     * Class destructor.
     */
    ~SetIteratorSafe() noexcept;

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Assignment operator.
     * @param from The iterator to copy.
     * @return Returns this iterator.
     */
    SetIteratorSafe< Key >& operator=(const SetIteratorSafe< Key >& from);

    /**
     * @brief Assignment operator.
     * @param from The iterator to copy.
     * @return Returns this iterator.
     */
    SetIteratorSafe< Key >& operator=(const SetIterator< Key >& from);

    /**
     * @brief Assignment operator.
     * @param from The iterator to move.
     * @return Returns this iterator.
     */
    SetIteratorSafe< Key >& operator=(SetIteratorSafe< Key >&& from) noexcept;

    /**
     * @brief Increments the iterator.
     * @return This iterator.
     */
    SetIteratorSafe< Key >& operator++() noexcept;

    /**
     * @brief Makes the iterator point to i elements further in the set.
     * @param i The number of increments.
     * @return Returns this iterator.
     */
    SetIteratorSafe< Key >& operator+=(Size i) noexcept;

    /**
     * @brief Returns a new iterator.
     * @param i The number of increments.
     * @return Returns a new iterator.
     */
    SetIteratorSafe< Key > operator+(Size i) const;

    /**
     * @brief Indicates whether two iterators point to different elements or
     * sets.
     * @param from The iterator to test for inequality.
     * @return Returns true if both iterator are not equal.
     */
    bool operator!=(const SetIteratorSafe< Key >& from) const noexcept;

    /**
     * @brief Indicates whether two iterators point toward the same element of
     * a same set.
     * @param from The iterator to test for equality.
     * @return Returns true if both iterator are equal.
     */
    bool operator==(const SetIteratorSafe< Key >& from) const noexcept;

    /**
     * @brief Returns the element pointed to by the iterator.
     *
     * @throws UndefinedIteratorValue Raised if the iterator does not point
     * to an element of the set (for instance if the set or the element
     * previously pointed to by the iterator have been deleted).
     *
     * @return Returns the element pointed to by the iterator.
     */
    const Key& operator*() const;

    /**
     * @brief Returns a pointer to the element pointed to by the iterator.
     *
     * @throws UndefinedIteratorValue Raised if the iterator does not point
     * to an element of the set (for instance if the set or the element
     * previously pointed to by the iterator have been deleted).
     *
     * @return Returns a pointer to the element pointed to by the iterator.
     */
    const Key* operator->() const;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief makes the iterator point toward nothing (in particular, it is not
     * related anymore to its current set).
     */
    void clear() noexcept;

    /// @}

    private:
    /// For efficiency, Set should be able to modify the hash table iterator.
    friend class Set< Key >;

    /// The underlying iterator for the set's hash table containing the data.
    HashTableConstIteratorSafe< Key, bool > _ht_iter_;
  };

  // ===========================================================================
  // ===                         UNSAFE SET ITERATORS                        ===
  // ===========================================================================

  /**
   * @class SetIterator
   * @headerfile set.h <agrum/tools/core/set.h>
   * @brief Unsafe iterators for the Set class.
   * @ingroup set_group
   *
   * Developers may consider using Set<x>::iterator instead of SetIterator<x>.
   *
   * @warning Use SetIterator only if you are sure that the iterator will never
   * point to a deleted element. Pointing to a deleted element will most
   * probably result in a segfault. If you are unsure, prefer using the safe
   * iterators Set<>::iterator_safe.
   *
   * @par Usage example:
   * @code
   * // creation of a set with 10 elements
   * Set<int> set;
   * for (int i = 0; i< 10; ++i)
   *   set<<i;
   *
   * // parse the set
   * for (const auto iter = table.begin (); iter != table.end (); *++iter) {
   *   // display the values
   *   cerr <<  *iter << endl;
   * }
   *
   * // check whether two iterators point toward the same element
   * Set<int>::iterator iter1 = table1.begin();
   * Set<int>::iterator iter2 = table1.end();
   * if (iter1 != iter)
   *   cerr << "iter1 and iter2 point toward different elements";
   *
   * @endcode
   *
   * @tparam Key The elements type.
   */
  template < typename Key >
  class SetIterator {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using value_type        = Key;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using difference_type   = std::ptrdiff_t;
    /// @}

    /**
     * @brief An enumeration to position the iterator at the beginning or the
     * end of the set.
     */
    enum Position {
      BEGIN,
      END
    };

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor: the iterator points toward nothing.
     */
    explicit SetIterator() noexcept;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static end iterator
    // only set.cpp should use this constructor
    explicit consteval SetIterator(StaticInitializer init) noexcept : _ht_iter_(init) {}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /**
     * @brief Creates an iterator for a given set.
     *
     * By default, the iterator points to the beginning of the set, but, using
     * optional argument pos, you can make it point to end().
     *
     * @param from The gum::Set to iterator over.
     * @param pos Where to start iterating.
     */
    SetIterator(const Set< Key >& from, Position pos = BEGIN);

    /**
     * @brief Copy constructor.
     * @param from The iterator to copy.
     */
    SetIterator(const SetIterator< Key >& from) noexcept;

    /**
     * @brief Move constructor.
     * @param from The iterator to move.
     */
    SetIterator(SetIterator< Key >&& from) noexcept;

    /**
     * @brief Class destructor.
     */
    ~SetIterator() noexcept;

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Assignment operator.
     * @param from The iterator to copy.
     * @return Returns this iterator.
     */
    SetIterator< Key >& operator=(const SetIterator< Key >& from) noexcept;

    /**
     * @brief Assignment operator.
     * @param from The iterator to copy.
     * @return Returns this iterator.
     */
    SetIterator< Key >& operator=(SetIterator< Key >&& from) noexcept;

    /**
     * @brief Increments the iterator.
     * @return This iterator.
     */
    SetIterator< Key >& operator++() noexcept;

    /**
     * @brief Makes the iterator point to i elements further in the set.
     * @param i The number of increments.
     * @return Returns this iterator.
     */
    SetIterator< Key >& operator+=(Size i) noexcept;

    /**
     * @brief Returns a new iterator.
     * @param i The number of increments.
     * @return Returns a new iterator.
     */
    SetIterator< Key > operator+(Size i) const noexcept;

    /**
     * @brief Indicates whether two iterators point to different elements or
     * sets.
     * @param from The iterator to test for inequality.
     * @return Returns true if both iterator are not equal.
     */
    bool operator!=(const SetIterator< Key >& from) const noexcept;

    /**
     * @brief Indicates whether two iterators point toward the same element of
     * a same set.
     * @param from The iterator to test for equality.
     * @return Returns true if both iterator are equal.
     */
    bool operator==(const SetIterator< Key >& from) const noexcept;

    /**
     * @brief Returns the element pointed to by the iterator.
     *
     * @throws UndefinedIteratorValue Raised if the iterator does not point
     * to an element of the set (for instance if the set or the element
     * previously pointed to by the iterator have been deleted).
     *
     * @return Returns the element pointed to by the iterator.
     */
    const Key& operator*() const;

    /**
     * @brief Returns a pointer to the element pointed to by the iterator.
     *
     * @throws UndefinedIteratorValue Raised if the iterator does not point
     * to an element of the set (for instance if the set or the element
     * previously pointed to by the iterator have been deleted).
     *
     * @return Returns a pointer to the element pointed to by the iterator.
     */
    const Key* operator->() const;

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief makes the iterator point toward nothing (in particular, it is not
     * related anymore to its current set).
     */
    void clear() noexcept;

    /// @}

    private:
    /// For efficiency, Set should be able to modify the hash table iterator.
    friend class Set< Key >;
    friend class SetIteratorSafe< Key >;

    /// The underlying iterator for the set's hash table containing the data.
    HashTableConstIterator< Key, bool > _ht_iter_;
  };


  /// @brief A << operator for HashTableList.
  template < typename Key >
  std::ostream& operator<<(std::ostream&, const Set< Key >&);


  /// the hash function for sets of int
  template < typename T >
  class HashFunc< Set< T > >: public HashFuncBase< Set< T > > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const Set< T >& key);

    /// computes the hashed value of a key
    virtual Size operator()(const Set< T >& key) const override final;
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // _static_Set_end_ is a 'constant' iterator initialized at compile time
  // that represents the end iterators for all sets (whatever their
  // type). This global variable avoids creating the same iterators within every
  // Set instance (this would be quite inefficient as end is precisely
  // identical for all sets). The same hold for safe end iterators.
  // The type of _Set_end_ is a pointer to void because C++ allows
  // pointers to void to be cast into pointers to other types (and conversely).
  // This avoids the painful strict-aliasing rule warning
  extern const SetIterator< int >     _static_Set_end_;
  extern const SetIteratorSafe< int > _static_Set_end_safe_;

  inline constexpr void* const _Set_end_      = (void* const)&_static_Set_end_;
  inline constexpr void* const _Set_end_safe_ = (void* const)&_static_Set_end_safe_;
#endif   // DOXYGEN_SHOULD_SKIP_THIS

} /* namespace gum */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::Set< int >;
extern template class gum::Set< long >;
extern template class gum::Set< unsigned int >;
extern template class gum::Set< unsigned long >;
extern template class gum::Set< double >;
extern template class gum::Set< std::string >;
#endif


// always include the implementation of the templates
#include <agrum/tools/core/set_tpl.h>

#endif   // GUM_SET_H
