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
 * @brief Sets of elements (i.e. the mathematical notion of a set).
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */
#ifndef GUM_SET_H
#define GUM_SET_H

#include <initializer_list>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/core/debug.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/list.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  template <typename Key>
  class SetIteratorSafe;
  template <typename Key>
  class SetIterator;
  template <typename Key, typename Alloc>
  class Set;

  template <typename Key>
  using SetConstIterator = SetIterator<Key>;
  template <typename Key>
  using SetConstIteratorSafe = SetIteratorSafe<Key>;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  /**
   * @class SetIteratorStaticEnd set.h <agrum/core/set.h>
   * @brief A class used to create the static iterator used by Sets.
   * @ingroup set_group
   *
   * The aim of using this class rather than just creating __SetIterEnd as a
   * global variable is to prevent other classes to access and modify
   * __SetIterEnd.
   */
  class SetIteratorStaticEnd {
    private:
    /**
     * @brief The safe iterator used by everyone.
     * @return Returns the safe iterator used by everyone.
     */
    static const SetIteratorSafe<int>* __SetIterEndSafe;

    /**
     * @brief The unsafe iterator used by everyone.
     * @return Returns the unsafe iterator used by everyone.
     */
    static const SetIterator<int>* __SetIterEnd;

    /**
     * @brief Creates (if needed) and returns the iterator __SetIterEndSafe.
     * @return Returns the iterator __SetIterEndSafe.
     */
    static const SetIteratorSafe<int>* endSafe4Statics();

    /**
     * @brief Creates (if needed) and returns the iterator __SetIterEndSafe.
     * @return Returns the iterator __SetIterEndSafe.
     */
    static const SetIteratorSafe<int>* constEndSafe4Statics();

    /**
     * @brief Creates (if needed) and returns the iterator __SetIterEnd.
     * @return Returns the iterator __SetIterEnd.
     */
    static const SetIterator<int>* end4Statics();

    /**
     * @brief Creates (if needed) and returns the iterator __SetIterEnd.
     * @return Returns the iterator __SetIterEnd.
     */
    static const SetIterator<int>* constEnd4Statics();

    /// Friends that have access to the iterator.
    template <typename Key, typename Alloc>
    friend class Set;
  };


  // ===========================================================================
  // ===                               GUM_SET                               ===
  // ===========================================================================

  /**
   * @class Set set.h <agrum/core/set.h>
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
   * for (auto iter = set.begin (); iter != set.end (); ++iter) {
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
   * @tparam Alloc The elements allocator.
   */
  template <typename Key, typename Alloc = std::allocator<Key>>
  class Set {
    public:
    /// Types for STL compliance.
    /// @{
    using value_type = Key;
    using reference = Key&;
    using const_reference = const Key&;
    using pointer = Key*;
    using const_pointer = const Key*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = Alloc;
    using iterator = SetIterator<Key>;
    using const_iterator = SetIterator<Key>;
    using iterator_safe = SetIteratorSafe<Key>;
    using const_iterator_safe = SetIteratorSafe<Key>;
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
    explicit Set( Size capacity = HashTableConst::default_size,
                  bool resize_policy = true );

    /**
     * @brief Initializer list constructor.
     * @param list The initializer list.
     */
    Set( std::initializer_list<Key> list );

    /**
     * @brief Copy constructor.
     * @param aHT The gum::Set to copy.
     */
    Set( const Set<Key, Alloc>& aHT );

    /**
     * @brief Generalized copy constructor.
     * @param aHT The gum::Set to copy.
     * @tparam OtherAlloc The other gum::Set allocator.
     */
    template <typename OtherAlloc>
    Set( const Set<Key, OtherAlloc>& aHT );

    /**
     * @brief Move constructor.
     * @param aHT The gum::Set to move.
     */
    Set( Set<Key, Alloc>&& aHT );

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
    Set<Key, Alloc>& operator=( const Set<Key, Alloc>& from );

    /**
     * @brief Generalized copy operator.
     * @param from The gum::Set to copy.
     * @tparam OtherAlloc The other gum::Set allocator.
     * @return Returns this gum::Set.
     */
    template <typename OtherAlloc>
    Set<Key, Alloc>& operator=( const Set<Key, OtherAlloc>& from );

    /**
     * @brief Move operator.
     * @param from The gum::Set to move.
     * @return Returns this gum::Set.
     */
    Set<Key, Alloc>& operator=( Set<Key, Alloc>&& from );

    /**
     * @brief Mathematical equality between two sets.
     * @param s2 The gum::Set to test for equality.
     * @tparam OtherAlloc The other gum::Set allocator.
     * @return Returns true if both gum::Set are equal.
     */
    template <typename OtherAlloc>
    bool operator==( const Set<Key, OtherAlloc>& s2 ) const;

    /**
     * @brief Mathematical inequality between two sets.
     * @param s2 The gum::Set to test for inequality.
     * @tparam OtherAlloc The other gum::Set allocator.
     * @return Returns true if both gum::Set are not equal.
     */
    template <typename OtherAlloc>
    bool operator!=( const Set<Key, OtherAlloc>& s2 ) const;

    /**
     * @brief Intersection update operator
     * @tparam OtherAlloc The other gum::Set allocator.
     * @param s2 The gum::Set to intersect.
     * @return Returns this. Now this contains the elements belonging
     * both to this and s2.
     */
    template <typename OtherAlloc>
    const Set<Key, Alloc>& operator*=( const Set<Key, OtherAlloc>& s2 );

    /**
     * @brief Intersection operator.
     * @tparam OtherAlloc The other gum::Set allocator.
     * @param s2 The gum::Set to intersect.
     * @return Returns a Set containing the elements belonging both to this and
     * s2.
     */
    template <typename OtherAlloc>
    Set<Key, Alloc> operator*( const Set<Key, OtherAlloc>& s2 ) const;

    /**
     * @brief Union update operator
     * @tparam OtherAlloc The other gum::Set allocator.
     * @param s2 The gum::Set to update
     * @return Returns this. Now this contains the elements belonging
     * both to this or to s2.
     */
    template <typename OtherAlloc>
    const Set<Key, Alloc>& operator+=( const Set<Key, OtherAlloc>& s2 );

    /**
     * @brief Union operator.
     * @tparam OtherAlloc The other gum::Set allocator.
     * @param s2 The gum::Set to union.
     * @return Returns a new Set containing the union of the elements of this
     * and s2.
     */
    template <typename OtherAlloc>
    Set<Key, Alloc> operator+( const Set<Key, OtherAlloc>& s2 ) const;


    /**
     * @brief Disjunction operator.
     * @tparam OtherAlloc The other gum::Set allocator.
     * @param s2 The gum::Set to disjunct.
     * @return Returns a Set whose elements belong to this but not to s2.
     * @warning Unlike + and *, the - operator is not commutative!
     */
    template <typename OtherAlloc>
    Set<Key, Alloc> operator-( const Set<Key, OtherAlloc>& s2 ) const;

    /**
     * @brief Adds a new element to the set (alias for insert).
     * @param k The new element to add.
     * @return Returns this gum::Set.
     */
    Set<Key, Alloc>& operator<<( const Key& k );

    /**
     * @brief Adds a new element to the set (alias for insert).
     * @param k The new element to add.
     * @return Returns this gum::Set.
     */
    Set<Key, Alloc>& operator<<( Key&& k );

    /**
     * @brief Removes an element from the set (alias for erase).
     * @param k The element to remove.
     * @return Return this gum::Set.
     */
    Set<Key, Alloc>& operator>>( const Key& k );

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
    void insert( const Key& k );

    /**
     * @brief Inserts a new element into the set.
     * @param k The new element to insert.
     * @warning if the set already contains the element, nothing is done.  In
     * particular, it is not added to the set and no exception is thrown.
     */
    void insert( Key&& k );

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
    template <typename... Args>
    void emplace( Args&&... args );

    /**
     * @brief Erases an element from the set.
     *
     * @param k The element to remove.
     * @warning if the set does not contain the element, nothing is done.  In
     * particular, no exception is thrown.
     */
    void erase( const Key& k );

    /**
     * @brief Erases an element from the set.
     * @param k The iterator pointing to the element to remove.
     * @warning if the set does not contain the element, nothing is done.  In
     * particular, no exception is thrown.
     */
    void erase( const iterator_safe& k );

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
    bool contains( const Key& k ) const;

    /**
     * @return Returns true if *this is a strict subset of s
     */
    template <typename OtherAlloc>
    bool isSubsetOf( const Set<Key, OtherAlloc>& s ) const;

    /**
     * @return Returns true if *this is a strict superset of s
     */
    template <typename OtherAlloc>
    bool isSupersetOf( const Set<Key, OtherAlloc>& s ) const;

    /**
     * @brief Indicates whether a given elements belong to the set.
     * @return Returns true if a given elements belong to the set.
     */
    bool exists( const Key& k ) const;

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

    /**
     * @brief Returns the end iterator for other classes' statics (read the
     * detailed description of this method).
     *
     * To reduce the Sets memory consumption (which are heavily used in aGrUM)
     * while allowing fast for(iter=begin(); iter!=end();++iter) loops, end
     * iterators are created just once as a static member of a non-template
     * Set.  While this scheme is efficient and it works quite effectively when
     * manipulating sets, it has a drawback: other classes with static members
     * using the Set's end() iterator may fail to work due to the well known
     * "static initialization order fiasco" (see Marshall Cline's C++ FAQ for
     * more details about this C++ feature). OK, so what is the problem?
     * Consider a class, say X, containing a Set that stores all its elements
     * in a convenient way. To reduce memory consumption, X::end iterator is a
     * static member that is initialized with a Set::end iterator. If the
     * compiler decides to initialize X::end before initializing Set::end, then
     * X::end will be in an incoherent state. Unfortunately, we cannot know for
     * sure in which order static members will be initialized (the order is a
     * compiler's decision).  Hence, we shall enfore the fact that Set::end is
     * initialized before X::end.  Using method Set::end4Statics will ensure
     * this fact: it uses the C++ "construct on first use" idiom (see the C++
     * FAQ) that ensures that the order fiasco is avoided. More precisely,
     * end4Statics uses a global variable that is the very end iterator used by
     * all Sets. Now, this induces a small overhead. So, we also provide a
     * Set::end() method that returns the Set::end iterator without this small
     * overhead, but assuming that function end4Statics has already been called
     * once (which is always the case) when a Set has been created.
     *
     * So, to summarize: when initializing static members, use end4Statics()
     * rather than end(). In all the other cases, use simply the usual method
     * end().
     *
     * @return Returns the end iterator for other classes' statics (read the
     * detailed description of this method).
     */
    static const iterator& end4Statics();

    /**
     * @brief Returns the end iterator for other classes' statics (read the
     * detailed description of this method).
     *
     * To reduce the Sets memory consumption (which are heavily used in aGrUM)
     * while allowing fast for(iter=begin(); iter!=end();++iter) loops, end
     * iterators are created just once as a static member of a non-template
     * Set.  While this scheme is efficient and it works quite effectively when
     * manipulating sets, it has a drawback: other classes with static members
     * using the Set's end() iterator may fail to work due to the well known
     * "static initialization order fiasco" (see Marshall Cline's C++ FAQ for
     * more details about this C++ feature). OK, so what is the problem?
     * Consider a class, say X, containing a Set that stores all its elements
     * in a convenient way. To reduce memory consumption, X::end iterator is a
     * static member that is initialized with a Set::end iterator. If the
     * compiler decides to initialize X::end before initializing Set::end, then
     * X::end will be in an incoherent state. Unfortunately, we cannot know for
     * sure in which order static members will be initialized (the order is a
     * compiler's decision).  Hence, we shall enfore the fact that Set::end is
     * initialized before X::end.  Using method Set::end4Statics will ensure
     * this fact: it uses the C++ "construct on first use" idiom (see the C++
     * FAQ) that ensures that the order fiasco is avoided. More precisely,
     * end4Statics uses a global variable that is the very end iterator used by
     * all Sets. Now, this induces a small overhead. So, we also provide a
     * Set::end() method that returns the Set::end iterator without this small
     * overhead, but assuming that function end4Statics has already been called
     * once (which is always the case) when a Set has been created.
     *
     * So, to summarize: when initializing static members, use
     * constEnd4Statics() rather than cend(). In all the other cases, use
     * simply the usual method cend().
     *
     * @return Returns the end iterator for other classes' statics (read the
     * detailed description of this method).
     */
    static const const_iterator& constEnd4Statics();

    /**
     * @brief Returns the end iterator for other classes' statics (read the
     * detailed description of this method).
     *
     * To reduce the Sets memory consumption (which are heavily used in aGrUM)
     * while allowing fast for(iter=begin(); iter!=end();++iter) loops, end
     * iterators are created just once as a static member of a non-template
     * Set.  While this scheme is efficient and it works quite effectively when
     * manipulating sets, it has a drawback: other classes with static members
     * using the Set's end() iterator may fail to work due to the well known
     * "static initialization order fiasco" (see Marshall Cline's C++ FAQ for
     * more details about this C++ feature). OK, so what is the problem?
     * Consider a class, say X, containing a Set that stores all its elements
     * in a convenient way. To reduce memory consumption, X::end iterator is a
     * static member that is initialized with a Set::end iterator. If the
     * compiler decides to initialize X::end before initializing Set::end, then
     * X::end will be in an incoherent state. Unfortunately, we cannot know for
     * sure in which order static members will be initialized (the order is a
     * compiler's decision).  Hence, we shall enfore the fact that Set::end is
     * initialized before X::end.  Using method Set::end4Statics will ensure
     * this fact: it uses the C++ "construct on first use" idiom (see the C++
     * FAQ) that ensures that the order fiasco is avoided. More precisely,
     * end4Statics uses a global variable that is the very end iterator used by
     * all Sets. Now, this induces a small overhead. So, we also provide a
     * Set::end() method that returns the Set::end iterator without this small
     * overhead, but assuming that function end4Statics has already been called
     * once (which is always the case) when a Set has been created.
     *
     * So, to summarize: when initializing static members, use
     * endSafe4Statics() rather than endSafe (). In all the other cases, use
     * simply the usual method endSafe ().
     *
     * @return Returns the end iterator for other classes' statics (read the
     * detailed description of this method).
     */
    static const iterator_safe& endSafe4Statics();

    /**
     * @brief Returns the end iterator for other classes' statics (read the
     * detailed description of this method).
     *
     * To reduce the Sets memory consumption (which are heavily used in aGrUM)
     * while allowing fast for(iter=begin(); iter!=end();++iter) loops, end
     * iterators are created just once as a static member of a non-template
     * Set.  While this scheme is efficient and it works quite effectively when
     * manipulating sets, it has a drawback: other classes with static members
     * using the Set's end() iterator may fail to work due to the well known
     * "static initialization order fiasco" (see Marshall Cline's C++ FAQ for
     * more details about this C++ feature). OK, so what is the problem?
     * Consider a class, say X, containing a Set that stores all its elements
     * in a convenient way. To reduce memory consumption, X::end iterator is a
     * static member that is initialized with a Set::end iterator. If the
     * compiler decides to initialize X::end before initializing Set::end, then
     * X::end will be in an incoherent state. Unfortunately, we cannot know for
     * sure in which order static members will be initialized (the order is a
     * compiler's decision).  Hence, we shall enfore the fact that Set::end is
     * initialized before X::end.  Using method Set::end4Statics will ensure
     * this fact: it uses the C++ "construct on first use" idiom (see the C++
     * FAQ) that ensures that the order fiasco is avoided. More precisely,
     * end4Statics uses a global variable that is the very end iterator used by
     * all Sets. Now, this induces a small overhead. So, we also provide a
     * Set::end() method that returns the Set::end iterator without this small
     * overhead, but assuming that function end4Statics has already been called
     * once (which is always the case) when a Set has been created.
     *
     * So, to summarize: when initializing static members, use
     * constEndSafe4Statics() rather than cendSafe(). In all the other cases,
     * use simply the usual method cendSafe ().
     *
     * @return Returns the end iterator for other classes' statics (read the
     * detailed description of this method).
     */
    static const const_iterator_safe& constEndSafe4Statics();

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
    void resize( Size new_capacity );

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
    void setResizePolicy( const bool new_policy );

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
    template <typename NewKey,
              typename NewAlloc = typename Alloc::template rebind<NewKey>::other>
    HashTable<Key, NewKey, NewAlloc> hashMap( NewKey ( *f )( const Key& ),
                                              Size capacity = 0 ) const;

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
    template <typename NewKey,
              typename NewAlloc = typename Alloc::template rebind<NewKey>::other>
    HashTable<Key, NewKey, NewAlloc> hashMap( const NewKey& val,
                                              Size size = 0 ) const;

    /**
     * @brief A method to create a List of NewKey from the set.
     *
     * @warning The order of the NewKey elements in the resulting list is
     * arbitrary.
     *
     * @param f A function that maps a Key into a NewKey
     */
    template <typename NewKey,
              typename NewAlloc = typename Alloc::template rebind<NewKey>::other>
    List<NewKey, NewAlloc> listMap( NewKey ( *f )( const Key& ) ) const;

    /// @}

    private:
    /// Friends to speed up access
    /// @{
    friend class SetIterator<Key>;
    friend class SetIteratorSafe<Key>;
    template <typename K, typename A>
    friend class Set;
    /// @}

    /// A set of X's is actually a hash table whose keys are the X's.
    HashTable<Key, bool, Alloc> __inside;

    /// Convert a hash table into a set of keys.
    Set( const HashTable<Key, bool, Alloc>& h );
  };


  // ===========================================================================
  // ===                          SAFE SET ITERATORS                         ===
  // ===========================================================================

  /**
   * @class SetIteratorSafe set.h <agrum/core/set.h>
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
   * for (auto iter = table.beginSafe (); iter != table.endSafe (); ++iter) {
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
  template <typename Key>
  class SetIteratorSafe {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using value_type = Key;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using difference_type = std::ptrdiff_t;
    /// @}

    /**
     * @brief An enumeration to position the iterator at the beginning or the
     * end of the set.
     */
    enum Position { BEGIN, END };

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor: the iterator points toward nothing.
     */
    SetIteratorSafe();

    /**
     * @brief Creates an iterator for a given set.
     *
     * By default, the iterator points to the beginning of the set, but, using
     * optional argument pos, you can make it point to end().
     *
     * @tparam Alloc The gum::Set allocator.
     * @param from The gum::Set to iterate over.
     * @param pos Where to start iterating.
     */
    template <typename Alloc>
    SetIteratorSafe( const Set<Key, Alloc>& from, Position pos = BEGIN );

    /**
     * @brief Copy constructor.
     * @param from The iterator to copy.
     */
    SetIteratorSafe( const SetIteratorSafe<Key>& from );

    /**
     * @brief Copy constructor.
     * @param from The iterator to copy.
     */
    explicit SetIteratorSafe( const SetIterator<Key>& from );

    /**
     * @brief Move constructor.
     * @param from The iterator to move.
     */
    SetIteratorSafe( SetIteratorSafe<Key>&& from );

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
    SetIteratorSafe<Key>& operator=( const SetIteratorSafe<Key>& from );

    /**
     * @brief Assignment operator.
     * @param from The iterator to copy.
     * @return Returns this iterator.
     */
    SetIteratorSafe<Key>& operator=( const SetIterator<Key>& from );

    /**
     * @brief Assignment operator.
     * @param from The iterator to move.
     * @return Returns this iterator.
     */
    SetIteratorSafe<Key>& operator=( SetIteratorSafe<Key>&& from ) noexcept;

    /**
     * @brief Increments the iterator.
     * @return This iterator.
     */
    SetIteratorSafe<Key>& operator++() noexcept;

    /**
     * @brief Makes the iterator point to i elements further in the set.
     * @param i The number of increments.
     * @return Returns this iterator.
     */
    SetIteratorSafe<Key>& operator+=( unsigned int i ) noexcept;

    /**
     * @brief Returns a new iterator.
     * @param i The number of increments.
     * @return Returns a new iterator.
     */
    SetIteratorSafe<Key> operator+( unsigned int i ) const;

    /**
     * @brief Indicates whether two iterators point to different elements or
     * sets.
     * @param from The iterator to test for inequality.
     * @return Returns true if both iterator are not equal.
     */
    bool operator!=( const SetIteratorSafe<Key>& from ) const noexcept;

    /**
     * @brief Indicates whether two iterators point toward the same element of
     * a same set.
     * @param from The iterator to test for equality.
     * @return Returns true if both iterator are equal.
     */
    bool operator==( const SetIteratorSafe<Key>& from ) const noexcept;

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
    template <typename K, typename A>
    friend class Set;

    /// The underlying iterator for the set's hash table containing the data.
    HashTableConstIteratorSafe<Key, bool> __ht_iter;
  };

  // ===========================================================================
  // ===                         UNSAFE SET ITERATORS                        ===
  // ===========================================================================

  /**
   * @class SetIterator set.h <agrum/core/set.h>
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
   * for (auto iter = table.begin (); iter != table.end (); *++iter) {
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
  template <typename Key>
  class SetIterator {
    public:
    /// Types for STL compliance.
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using value_type = Key;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using difference_type = std::ptrdiff_t;
    /// @}

    /**
     * @brief An enumeration to position the iterator at the beginning or the
     * end of the set.
     */
    enum Position { BEGIN, END };

    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor: the iterator points toward nothing.
     */
    SetIterator() noexcept;

    /**
     * @brief Creates an iterator for a given set.
     *
     * By default, the iterator points to the beginning of the set, but, using
     * optional argument pos, you can make it point to end().
     *
     * @tparam Alloc The gum::Set allocator.
     * @param from The gum::Set to iterator over.
     * @param pos Where to start iterating.
     */
    template <typename Alloc>
    SetIterator( const Set<Key, Alloc>& from, Position pos = BEGIN );

    /**
     * @brief Copy constructor.
     * @param from The iterator to copy.
     */
    SetIterator( const SetIterator<Key>& from ) noexcept;

    /**
     * @brief Move constructor.
     * @param from The iterator to move.
     */
    SetIterator( SetIterator<Key>&& from ) noexcept;

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
    SetIterator<Key>& operator=( const SetIterator<Key>& from ) noexcept;

    /**
     * @brief Assignment operator.
     * @param from The iterator to copy.
     * @return Returns this iterator.
     */
    SetIterator<Key>& operator=( SetIterator<Key>&& from ) noexcept;

    /**
     * @brief Increments the iterator.
     * @return This iterator.
     */
    SetIterator<Key>& operator++() noexcept;

    /**
     * @brief Makes the iterator point to i elements further in the set.
     * @param i The number of increments.
     * @return Returns this iterator.
     */
    SetIterator<Key>& operator+=( unsigned int i ) noexcept;

    /**
     * @brief Returns a new iterator.
     * @param i The number of increments.
     * @return Returns a new iterator.
     */
    SetIterator<Key> operator+( unsigned int i ) const noexcept;

    /**
     * @brief Indicates whether two iterators point to different elements or
     * sets.
     * @param from The iterator to test for inequality.
     * @return Returns true if both iterator are not equal.
     */
    bool operator!=( const SetIterator<Key>& from ) const noexcept;

    /**
     * @brief Indicates whether two iterators point toward the same element of
     * a same set.
     * @param from The iterator to test for equality.
     * @return Returns true if both iterator are equal.
     */
    bool operator==( const SetIterator<Key>& from ) const noexcept;

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
    template <typename K, typename A>
    friend class Set;
    friend class SetIteratorSafe<Key>;

    /// The underlying iterator for the set's hash table containing the data.
    HashTableConstIterator<Key, bool> __ht_iter;
  };

  /// @brief A << operator for HashTableList.
  template <typename Key, typename Alloc>
  std::ostream& operator<<( std::ostream&, const Set<Key, Alloc>& );


  /// the hash function for sets of int
  template <typename Alloc>
  class HashFunc<Set<int, Alloc>> : public HashFuncBase<Set<int, Alloc>> {
    public:
    /// computes the hashed value of a key
    Size operator()( const Set<int, Alloc>& key ) const;
  };

  /// the hash function for sets of unsigned int
  template <typename Alloc>
  class HashFunc<Set<unsigned int, Alloc>>
      : public HashFuncBase<Set<unsigned int, Alloc>> {
    public:
    /// computes the hashed value of a key
    Size operator()( const Set<unsigned int, Alloc>& key ) const;
  };

  /// the hash function for sets of long
  template <typename Alloc>
  class HashFunc<Set<long, Alloc>> : public HashFuncBase<Set<long, Alloc>> {
    public:
    /// computes the hashed value of a key
    Size operator()( const Set<long, Alloc>& key ) const;
  };

  /// the hash function for sets of unsigned long
  template <typename Alloc>
  class HashFunc<Set<unsigned long, Alloc>>
      : public HashFuncBase<Set<unsigned long, Alloc>> {
    public:
    /// computes the hashed value of a key
    Size operator()( const Set<unsigned long, Alloc>& key ) const;
  };

  /// the hash function for sets of float
  template <typename Alloc>
  class HashFunc<Set<float, Alloc>> : public HashFuncBase<Set<float, Alloc>> {
    public:
    /// computes the hashed value of a key
    Size operator()( const Set<float, Alloc>& key ) const;

    private:
    /// used for casting floats to Size
    typename HashFuncCastKey<float>::type __casting;
  };

  /// the hash function for sets of double
  template <typename Alloc>
  class HashFunc<Set<double, Alloc>> : public HashFuncBase<Set<double, Alloc>> {
    public:
    /// computes the hashed value of a key
    Size operator()( const Set<double, Alloc>& key ) const;

    private:
    /// used for casting floats to Size
    typename HashFuncCastKey<double>::type __casting;
  };


} /* namespace gum */


extern template class gum::Set<int>;
extern template class gum::Set<long>;
extern template class gum::Set<unsigned int>;
extern template class gum::Set<unsigned long>;
extern template class gum::Set<float>;
extern template class gum::Set<double>;
extern template class gum::Set<std::string>;


// always include the implementation of the templates
#include <agrum/core/set_tpl.h>

#endif  // GUM_SET_H
