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
 * @par Usage example:
 * @code
 * // creation of a set with 10 elements
 * Set<int> set;
 * for (int i = 0; i< 10; ++i)
 *   set<<i;
 * Set<int> set2 { 1, 2, 3 };
 *
 * // parse the set
 * for (Set<int>::iterator iter = set.begin ();
 *      iter != set.end (); ++iter) {
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
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */
#ifndef GUM_SET_H
#define GUM_SET_H

#include <iostream>
#include <initializer_list>

#include <agrum/core/list.h>
#include <agrum/core/hashTable.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  template <typename Key> class SetIteratorSafe;
  template <typename Key> class SetIterator;
  template <typename Key, typename Alloc> class Set;

  template <typename Key> using SetConstIterator = SetIterator<Key>;
  template <typename Key> using SetConstIteratorSafe = SetIteratorSafe<Key>;

  // a class used to create the static iterator used by Sets. The aim of
  // using this class rather than just creating __SetIterEnd as a global
  // variable is to prevent other classes to access and modify __SetIterEnd
  class SetIteratorStaticEnd {
    private:
    // the safe iterator used by everyone
    static const SetIteratorSafe<int> *__SetIterEndSafe;

    // the unsafe iterator used by everyone
    static const SetIterator<int> *__SetIterEnd;

    // creates (if needed) and returns the iterator __SetIterEndSafe
    static const SetIteratorSafe<int> *endSafe4Statics();

    // creates (if needed) and returns the iterator __SetIterEndSafe
    static const SetIteratorSafe<int> *constEndSafe4Statics();

    // creates (if needed) and returns the iterator __SetIterEnd
    static const SetIterator<int> *end4Statics();

    // creates (if needed) and returns the iterator __SetIterEnd
    static const SetIterator<int> *constEnd4Statics();

    // friends that have access to the iterator
    template <typename Key, typename Alloc> friend class Set;
  };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  /* =========================================================================== */
  /* ===                               GUM_SET                               === */
  /* =========================================================================== */
  /**
   * @class Set
   * @ingroup basicstruct_group
   * @brief Representation of a set
   *
   * A Set is a structure that contains arbitrary elements. Note that, as in
   * mathematics, an element cannot appear twice in a given set. Sets have unsafe
   * and safe iterators. The safe iterators (SetIteratorSafe<> a.k.a.
   * Set<>::iterator_safe are slightly slower than the unsafe ones
   * (SetIterator<> a.k.a. Set<>::iterator) but they guarantee that even if they
   * point to a deleted element, using their operators ++ or * cannot produce a
   * segfault. In such cases, they simply raise an exception. On the contrary,
   * unsafe iterators should @b never be used on elements that can be deleted
   * because, as in the STL, they will most probably produce a segfault.
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
   * for (Set<int>::iterator iter = set.begin ();
   *      iter != set.end (); ++iter) {
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
   */
  template <typename Key, typename Alloc = std::allocator<Key>> class Set {
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
    using iterator = SetIterator<Key>;
    using const_iterator = SetIterator<Key>;
    using iterator_safe = SetIteratorSafe<Key>;
    using const_iterator_safe = SetIteratorSafe<Key>;
    /// @}

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
    explicit Set(Size capacity = HashTableConst::default_size,
                 bool resize_policy = true);

    /// initializer list constructor
    explicit Set(std::initializer_list<Key> list);

    /// copy constructor
    Set(const Set<Key, Alloc> &aHT);

    /// generalized copy constructor
    template <typename OtherAlloc> Set(const Set<Key, OtherAlloc> &aHT);

    /// move constructor
    Set(Set<Key, Alloc> &&aHT);

    /// destructor
    ~Set();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    Set<Key, Alloc> &operator=(const Set<Key, Alloc> &from);

    /// generalized copy operator
    template <typename OtherAlloc>
    Set<Key, Alloc> &operator=(const Set<Key, OtherAlloc> &from);

    /// move operator
    Set<Key, Alloc> &operator=(Set<Key, Alloc> &&from);

    /// mathematical equality between two sets
    template <typename OtherAlloc>
    bool operator==(const Set<Key, OtherAlloc> &s2) const;

    /// mathematical inequality between two sets
    template <typename OtherAlloc>
    bool operator!=(const Set<Key, OtherAlloc> &s2) const;

    /// Intersection operator
    /** @return a Set containing the elements belonging both to this and s2. */
    template <typename OtherAlloc>
    Set<Key, Alloc> operator*(const Set<Key, OtherAlloc> &s2) const;

    /// Union operator
    /** @return a new Set containing the union of the elements of this and s2 */
    template <typename OtherAlloc>
    Set<Key, Alloc> operator+(const Set<Key, OtherAlloc> &s2) const;

    /// Disjunction operator
    /** @return a Set whose elements belong to this but not to s2
     * @warning Unlike + and *, the - operator is not commutative! */
    template <typename OtherAlloc>
    Set<Key, Alloc> operator-(const Set<Key, OtherAlloc> &s2) const;

    /// adds a new element to the set (alias for insert)
    Set<Key, Alloc> &operator<<(const Key &k);

    /// adds a new element to the set (alias for insert)
    Set<Key, Alloc> &operator<<(Key &&k);

    /// removes an element from the set (alias for erase)
    Set<Key, Alloc> &operator>>(const Key &k);

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// inserts a new element into the set
    /** @warning if the set already contains the element, nothing is done.
     * In particular, it is not added to the set and no exception is thrown. */
    void insert(const Key &k);

    /// inserts a new element into the set
    /** @warning if the set already contains the element, nothing is done.
     * In particular, it is not added to the set and no exception is thrown. */
    void insert(Key &&k);

    /// emplace a new element in the set
    /** emplace is a method that allows to construct directly an element of
     * type Key by passing to its constructor all the arguments it needs
     * @param args the arguments passed to the constructor
     * @warning if the set already contains the element, nothing is done.
     * In particular, it is not added to the set and no exception is thrown. */
    template <typename... Args> void emplace(Args &&... args);

    /// erases an element from the set
    /** @warning if the set does not contain the element, nothing is done.
     * In particular, no exception is thrown. */
    void erase(const Key &k);

    /// erases an element from the set
    /** @warning if the set does not contain the element, nothing is done.
     * In particular, no exception is thrown. */
    void erase(const iterator_safe &k);

    /// removes all the elements, if any, from the set
    void clear();

    /// returns the number of elements in the set
    Size size() const noexcept;

    /// indicates whether a given elements belong to the set
    bool contains(const Key &k) const;

    /// indicates whether a given elements belong to the set
    bool exists(const Key &k) const;

    /// indicates whether the set is the empty set
    bool empty() const noexcept;

    /// prints the content of the set
    std::string toString() const;

    /// @}

    // ############################################################################
    /// @name Iterators
    // ############################################################################
    /// @{

    /// the usual safe begin iterator to parse the set
    iterator_safe beginSafe() const;

    /// the usual safe begin iterator to parse the set
    const_iterator_safe cbeginSafe() const;

    /// the usual safe end iterator to parse the set
    const iterator_safe &endSafe() const noexcept;

    /// the usual safe end iterator to parse the set
    const const_iterator_safe &cendSafe() const noexcept;

    /// the usual unsafe begin iterator to parse the set
    iterator begin() const;

    /// the usual unsafe begin iterator to parse the set
    const_iterator cbegin() const;

    /// the usual unsafe end iterator to parse the set
    const iterator &end() const noexcept;

    /// the usual unsafe end iterator to parse the set
    const const_iterator &cend() const noexcept;

    /** @brief returns the end iterator for other classes' statics (read the
     * detailed description of this method)
     *
     * To reduce the Sets memory consumption (which are heavily used in aGrUM)
     * while allowing fast for(iter=begin(); iter!=end();++iter) loops, end
     * iterators are created just once as a static member of a non-template Set.
     * While this scheme is efficient and it works quite effectively when
     * manipulating sets, it has a drawback: other classes with static members
     * using the Set's end() iterator may fail to work due to the well known
     * "static initialization order fiasco" (see Marshall Cline's C++ FAQ for more
     * details about this C++ feature). OK, so what is the problem? Consider a
     * class, say X, containing a Set that stores all its elements in a convenient
     * way. To reduce memory consumption, X::end iterator is a static member that
     * is initialized with a Set::end iterator. If the compiler decides to
     * initialize X::end before initializing Set::end, then X::end will be in an
     * incoherent state. Unfortunately, we cannot know for sure in which order
     * static members will be initialized (the order is a compiler's decision).
     * Hence, we shall enfore the fact that Set::end is initialized before X::end.
     * Using method Set::end4Statics will ensure this fact: it uses the C++
     * "construct on first use" idiom (see the C++ FAQ) that ensures that the
     * order fiasco is avoided. More precisely, end4Statics uses a global
     * variable that is the very end iterator used by all Sets. Now, this induces
     * a small overhead. So, we also provide a Set::end() method that returns the
     * Set::end iterator without this small overhead, but assuming that function
     * end4Statics has already been called once (which is always the case) when a
     * Set has been created.
     *
     * So, to summarize: when initializing static members, use end4Statics() rather
     * than end(). In all the other cases, use simply the usual method end(). */
    static const iterator &end4Statics();

    /** @brief returns the end iterator for other classes' statics (read the
     * detailed description of this method)
     *
     * To reduce the Sets memory consumption (which are heavily used in aGrUM)
     * while allowing fast for(iter=begin(); iter!=end();++iter) loops, end
     * iterators are created just once as a static member of a non-template Set.
     * While this scheme is efficient and it works quite effectively when
     * manipulating sets, it has a drawback: other classes with static members
     * using the Set's end() iterator may fail to work due to the well known
     * "static initialization order fiasco" (see Marshall Cline's C++ FAQ for more
     * details about this C++ feature). OK, so what is the problem? Consider a
     * class, say X, containing a Set that stores all its elements in a convenient
     * way. To reduce memory consumption, X::end iterator is a static member that
     * is initialized with a Set::end iterator. If the compiler decides to
     * initialize X::end before initializing Set::end, then X::end will be in an
     * incoherent state. Unfortunately, we cannot know for sure in which order
     * static members will be initialized (the order is a compiler's decision).
     * Hence, we shall enfore the fact that Set::end is initialized before X::end.
     * Using method Set::end4Statics will ensure this fact: it uses the C++
     * "construct on first use" idiom (see the C++ FAQ) that ensures that the
     * order fiasco is avoided. More precisely, end4Statics uses a global
     * variable that is the very end iterator used by all Sets. Now, this induces
     * a small overhead. So, we also provide a Set::end() method that returns the
     * Set::end iterator without this small overhead, but assuming that function
     * end4Statics has already been called once (which is always the case) when a
     * Set has been created.
     *
     * So, to summarize: when initializing static members, use constEnd4Statics()
     * rather than cend(). In all the other cases, use simply the usual method
     * cend(). */
    static const const_iterator &constEnd4Statics();

    /** @brief returns the end iterator for other classes' statics (read the
     * detailed description of this method)
     *
     * To reduce the Sets memory consumption (which are heavily used in aGrUM)
     * while allowing fast for(iter=begin(); iter!=end();++iter) loops, end
     * iterators are created just once as a static member of a non-template Set.
     * While this scheme is efficient and it works quite effectively when
     * manipulating sets, it has a drawback: other classes with static members
     * using the Set's end() iterator may fail to work due to the well known
     * "static initialization order fiasco" (see Marshall Cline's C++ FAQ for more
     * details about this C++ feature). OK, so what is the problem? Consider a
     * class, say X, containing a Set that stores all its elements in a convenient
     * way. To reduce memory consumption, X::end iterator is a static member that
     * is initialized with a Set::end iterator. If the compiler decides to
     * initialize X::end before initializing Set::end, then X::end will be in an
     * incoherent state. Unfortunately, we cannot know for sure in which order
     * static members will be initialized (the order is a compiler's decision).
     * Hence, we shall enfore the fact that Set::end is initialized before X::end.
     * Using method Set::end4Statics will ensure this fact: it uses the C++
     * "construct on first use" idiom (see the C++ FAQ) that ensures that the
     * order fiasco is avoided. More precisely, end4Statics uses a global
     * variable that is the very end iterator used by all Sets. Now, this induces
     * a small overhead. So, we also provide a Set::end() method that returns the
     * Set::end iterator without this small overhead, but assuming that function
     * end4Statics has already been called once (which is always the case) when a
     * Set has been created.
     *
     * So, to summarize: when initializing static members, use endSafe4Statics()
     * rather than endSafe (). In all the other cases, use simply the usual
     * method endSafe (). */
    static const iterator_safe &endSafe4Statics();

    /** @brief returns the end iterator for other classes' statics (read the
     * detailed description of this method)
     *
     * To reduce the Sets memory consumption (which are heavily used in aGrUM)
     * while allowing fast for(iter=begin(); iter!=end();++iter) loops, end
     * iterators are created just once as a static member of a non-template Set.
     * While this scheme is efficient and it works quite effectively when
     * manipulating sets, it has a drawback: other classes with static members
     * using the Set's end() iterator may fail to work due to the well known
     * "static initialization order fiasco" (see Marshall Cline's C++ FAQ for more
     * details about this C++ feature). OK, so what is the problem? Consider a
     * class, say X, containing a Set that stores all its elements in a convenient
     * way. To reduce memory consumption, X::end iterator is a static member that
     * is initialized with a Set::end iterator. If the compiler decides to
     * initialize X::end before initializing Set::end, then X::end will be in an
     * incoherent state. Unfortunately, we cannot know for sure in which order
     * static members will be initialized (the order is a compiler's decision).
     * Hence, we shall enfore the fact that Set::end is initialized before X::end.
     * Using method Set::end4Statics will ensure this fact: it uses the C++
     * "construct on first use" idiom (see the C++ FAQ) that ensures that the
     * order fiasco is avoided. More precisely, end4Statics uses a global
     * variable that is the very end iterator used by all Sets. Now, this induces
     * a small overhead. So, we also provide a Set::end() method that returns the
     * Set::end iterator without this small overhead, but assuming that function
     * end4Statics has already been called once (which is always the case) when a
     * Set has been created.
     *
     * So, to summarize: when initializing static members, use
     * constEndSafe4Statics() rather than cendSafe(). In all the other cases, use
     * simply the usual method cendSafe (). */
    static const const_iterator_safe &constEndSafe4Statics();
    /// @}

    // ############################################################################
    /// @name Fine tuning
    // ############################################################################
    /// @{

    /// returns the capacity of the underlying hashtable containing the set
    /** The method runs in constant time. */
    Size capacity() const;

    /// changes the size of the underlying hashtable containing the set
    /** See HashTable's method resize for more details */
    void resize(Size new_capacity);

    /** @brief enables the user to change dynamically the resizing policy of
     * the underlying hashtable
     *
     * @param new_policy true => the set updates dynamically its memory consumption
     * to guarantee that its elements are fast to retrieve. When new_policy is
     * false, the set will not try to change its memory size, hence resulting in
     * potentially slower accesses. */
    void setResizePolicy(const bool new_policy);

    /// returns the current resizing policy of the underlying hashtable
    bool resizePolicy() const;

    ///  @}

    // ############################################################################
    /// @name mapper
    // ############################################################################
    /// @{

    /// creates a hashtable of NewKey from the set
    /** @param f a function that maps Key into a NewKey
     * @param capacity the size of the resulting hashtable. When equal to 0,
     * a default size is computed that is a good trade-off between space
     * consumption and efficiency of new elements insertions.
     * @warning The order in the resulting hashtable may not be similar to that of
     * the original set. Hence iterators on the former may not parse it in the same
     * order as iterators on the latter. */
    template <typename NewKey,
              typename NewAlloc = typename Alloc::template rebind<NewKey>::other>
    HashTable<Key, NewKey, NewAlloc> hashMap(NewKey (*f)(const Key &),
                                             Size capacity = 0) const;

    /// creates a hashtable of NewKey from the set
    /** @param val the value taken by all the elements of the resulting hashtable
     * @param size the size of the resulting hashtable. When equal to 0, a default
     * size is computed that is a good trade-off between space consumption and
     * efficiency of new elements insertions.
     * @warning The order in the resulting hashtable may not be similar to that of
     * the original set. Hence iterators on the former may not parse it in the same
     * order as iterators on the latter. */
    template <typename NewKey,
              typename NewAlloc = typename Alloc::template rebind<NewKey>::other>
    HashTable<Key, NewKey, NewAlloc> hashMap(const NewKey &val, Size size = 0) const;

    /// a method to create a List of NewKey from the set
    /** @param f a function that maps a Key into a NewKey
     * @warning the order of the NewKey elements in the resulting list is
     * arbitrary */
    template <typename NewKey,
              typename NewAlloc = typename Alloc::template rebind<NewKey>::other>
    List<NewKey, NewAlloc> listMap(NewKey (*f)(const Key &)) const;

    /// @}

    private:
    // friends
    friend class SetIterator<Key>;
    friend class SetIteratorSafe<Key>;
    template <typename K, typename A> friend class Set;

    /// a set of X's is actually a hashtable whose keys are the X's
    HashTable<Key, bool, Alloc> __inside;

    /// convert a hashtable into a set of keys
    Set(const HashTable<Key, bool, Alloc> &h);
  };

  /* =========================================================================== */
  /* ===                          SAFE SET ITERATORS                         === */
  /* =========================================================================== */
  /** @class SetIteratorSafe
   * @brief Safe iterators for the Set class
   **
   * Developers may consider using Set<x>::iterator_safe instead of
   * SetIteratorSafe<x>.
   * @par Usage example:
   * @code
   * // creation of a set with 10 elements
   * Set<int> set;
   * for (int i = 0; i< 10; ++i)
   *   set<<i;
   *
   * // parse the set
   * for (Set<int>::iterator_safe iter = table.beginSafe ();
   *        iter != table.endSafe (); ++iter) {
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
   */
  template <typename Key> class SetIteratorSafe {
    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using value_type = Key;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using difference_type = std::ptrdiff_t;
    /// @}

    /** @brief an enumeration to position the iterator at the beginning or the end
     * of the set */
    enum Position { BEGIN, END };

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor: the iterator points toward nothing
    SetIteratorSafe();

    /// creates an iterator for a given set
    /** By default, the iterator points to the beginning of the set, but, using
     * optional argument pos, you can make it point to end() */
    template <typename Alloc>
    SetIteratorSafe(const Set<Key, Alloc> &from, Position pos = BEGIN);

    /// copy constructor
    SetIteratorSafe(const SetIteratorSafe<Key> &from);

    /// copy constructor
    explicit SetIteratorSafe(const SetIterator<Key> &from);

    /// move constructor
    SetIteratorSafe(SetIteratorSafe<Key> &&from);

    /// destructor
    ~SetIteratorSafe() noexcept;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// assignment operator
    SetIteratorSafe<Key> &operator=(const SetIteratorSafe<Key> &from);

    /// assignment operator
    SetIteratorSafe<Key> &operator=(const SetIterator<Key> &from);

    /// move operator
    SetIteratorSafe<Key> &operator=(SetIteratorSafe<Key> &&from) noexcept;

    /// increments the iterator
    SetIteratorSafe<Key> &operator++() noexcept;

    /// makes the iterator point to i elements further in the set
    SetIteratorSafe<Key> &operator+=(unsigned int) noexcept;

    /// returns a new iterator
    SetIteratorSafe<Key> operator+(unsigned int) const;

    /// indicates whether two iterators point to different elements or sets
    bool operator!=(const SetIteratorSafe<Key> &from) const noexcept;

    /// indicates whether two iterators point toward the same element of a same set
    bool operator==(const SetIteratorSafe<Key> &from) const noexcept;

    /// returns the element pointed to by the iterator
    /** @throws UndefinedIteratorValue exception if the iterator does not point
     * to an element of the set (for instance if the set or the element previously
     * pointed to by the iterator have been deleted) */
    const Key &operator*() const;

    /// returns a pointer to the element pointed to by the iterator
    /** @throws UndefinedIteratorValue exception if the iterator does not point
     * to an element of the set (for instance if the set or the element previously
     * pointed to by the iterator have been deleted) */
    const Key *operator->() const;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /** @brief makes the iterator point toward nothing (in particular, it is not
     * related anymore to its current set) */
    void clear() noexcept;

    /// @}

    private:
    /// for efficiency, Set should be able to modify the hashtable iterator
    template <typename K, typename A> friend class Set;

    /// the underlying iterator for the set's hashtable containing the data
    HashTableConstIteratorSafe<Key, bool> __ht_iter;
  };

  /* =========================================================================== */
  /* ===                         UNSAFE SET ITERATORS                        === */
  /* =========================================================================== */
  /** @class SetIterator
   * @brief Unsafe iterators for the Set class
   **
   * Developers may consider using Set<x>::iterator instead of SetIterator<x>.
   * @warning Use SetIterator only if you are sure that the iterator will never
   * point to a deleted element. Pointing to a deleted element will most probably
   * result in a segfault. If you are unsure, prefer using the safe iterators
   * Set<>::iterator_safe.
   * @par Usage example:
   * @code
   * // creation of a set with 10 elements
   * Set<int> set;
   * for (int i = 0; i< 10; ++i)
   *   set<<i;
   *
   * // parse the set
   * for (Set<int>::iterator iter = table.begin (); iter != table.end (); ++iter) {
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
   */
  template <typename Key> class SetIterator {
    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using value_type = Key;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using difference_type = std::ptrdiff_t;
    /// @}

    /** @brief an enumeration to position the iterator at the beginning or the end
     * of the set */
    enum Position { BEGIN, END };

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor: the iterator points toward nothing
    SetIterator() noexcept;

    /// creates an iterator for a given set
    /** By default, the iterator points to the beginning of the set, but, using
     * optional argument pos, you can make it point to end() */
    template <typename Alloc>
    SetIterator(const Set<Key, Alloc> &from, Position pos = BEGIN);

    /// copy constructor
    SetIterator(const SetIterator<Key> &from) noexcept;

    /// move constructor
    SetIterator(SetIterator<Key> &&from) noexcept;

    /// destructor
    ~SetIterator() noexcept;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// assignment operator
    SetIterator<Key> &operator=(const SetIterator<Key> &from) noexcept;

    /// move operator
    SetIterator<Key> &operator=(SetIterator<Key> &&from) noexcept;

    /// increments the iterator
    SetIterator<Key> &operator++() noexcept;

    /// makes the iterator point to i elements further in the set
    SetIterator<Key> &operator+=(unsigned int) noexcept;

    /// returns a new iterator
    SetIterator<Key> operator+(unsigned int) const noexcept;

    /// indicates whether two iterators point to different elements or sets
    bool operator!=(const SetIterator<Key> &from) const noexcept;

    /// indicates whether two iterators point toward the same element of a same set
    bool operator==(const SetIterator<Key> &from) const noexcept;

    /// returns the element pointed to by the iterator
    /** @throws UndefinedIteratorValue exception if the iterator does not point
     * to an element of the set (for instance if the set or the element previously
     * pointed to by the iterator have been deleted) */
    const Key &operator*() const;

    /// returns a pointer to the element pointed to by the iterator
    /** @throws UndefinedIteratorValue exception if the iterator does not point
     * to an element of the set (for instance if the set or the element previously
     * pointed to by the iterator have been deleted) */
    const Key *operator->() const;

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /** @brief makes the iterator point toward nothing (in particular, it is not
     * related anymore to its current set) */
    void clear() noexcept;

    /// @}

    private:
    /// for efficiency, Set should be able to modify the hashtable iterator
    template <typename K, typename A> friend class Set;
    friend class SetIteratorSafe<Key>;

    /// the underlying iterator for the set's hashtable containing the data
    HashTableConstIterator<Key, bool> __ht_iter;
  };

  /// a << operator for HashTableList
  template <typename Key, typename Alloc>
  std::ostream &operator<<(std::ostream &, const Set<Key, Alloc> &);

} /* namespace gum */

/// always include the implementation of the templates
#include <agrum/core/set.tcc>

#endif // GUM_SET_H
