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
 * @author Christophe GONZALES and Jean-Philippe DUBUS
 */
#ifndef GUM_BIJECTION_H
#define GUM_BIJECTION_H

#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>
#include <initializer_list>

#include <agrum/core/hashTable.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  template <typename T1, typename T2> class BijectionIteratorSafe;
  template <typename T1, typename T2> class BijectionIterator;
  template <typename T1, typename T2, typename Alloc, bool>
  class BijectionImplementation;
  template <typename T1, typename T2, typename Alloc> class Bijection;

  // a class used to create the static iterator used by Bijections. The aim of
  // using this class rather than just creating __BijectionIterEnd as a global
  // variable is to prevent other classes to access and modify __BijectionIterEnd
  class BijectionIteratorStaticEnd {
    private:
    // the safe iterator used by everyone
    static const BijectionIteratorSafe<int, int> *__BijectionIterEndSafe;

    // creates (if needed) and returns the iterator __BijectionIterEnd
    static const BijectionIteratorSafe<int, int> *endSafe4Statics();

    // the unsafe iterator used by everyone
    static const BijectionIterator<int, int> *__BijectionIterEnd;

    // creates (if needed) and returns the iterator __BijectionIterEnd
    static const BijectionIterator<int, int> *end4Statics();

    // friends that have access to the iterator
    template <typename T1, typename T2, typename Alloc, bool>
    friend class BijectionImplementation;
  };

  // dummy classes that will enable discriminate without overhead between
  // scalars and non-scalars operators * and ->
  template <bool gen> struct BijectionIteratorGet {
    template <typename T> INLINE static const T &op_second(const T *x) { return *x; }
  };

  template <> struct BijectionIteratorGet<true> {
    template <typename T> INLINE static const T &op_second(const T &x) { return x; }
  };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  /* =========================================================================== */
  /* ===                NON SCALAR BIJECTION IMPLEMENTATION                  === */
  /* =========================================================================== */
  /**
   * This class is designed for modeling a bijection between two sets, the idea is
   * following :
   * - we want to create a bijection relation between type T1 and type T2
   * - for x in T1, there exists only one y in T2 associated to x
   * - for y in T2, there exists only one x in T1 associated to y
   * - the user inserts all the (x, y) associations and can search efficiently the
   * values thus associated
   */
  template <typename T1, typename T2, typename Alloc, bool Gen>
  class BijectionImplementation {
    public:
    /// types for STL compliance
    /// @{
    using type1_type = T1;
    using type1_reference = T1 &;
    using type1_const_reference = const T1 &;
    using type1_pointer = T1 *;
    using type1_const_pointer = const T1 *;
    using type2_type = T2;
    using type2_reference = T2 &;
    using type2_const_reference = const T2 &;
    using type2_pointer = T2 *;
    using type2_const_pointer = const T2 *;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = Alloc;
    using iterator = BijectionIterator<T1, T2>;
    using const_iterator = BijectionIterator<T1, T2>;
    using iterator_safe = BijectionIteratorSafe<T1, T2>;
    using const_iterator_safe = BijectionIteratorSafe<T1, T2>;

    using allocator12_type =
        typename Alloc::template rebind<std::pair<T1, T2 *>>::other;
    using allocator21_type =
        typename Alloc::template rebind<std::pair<T2, T1 *>>::other;
    /// @}

    private:
    /// Default constructor: creates a bijection without any association
    BijectionImplementation(Size size, bool resize_policy);

    /// initializer list constructor
    BijectionImplementation(std::initializer_list<std::pair<T1, T2>> list);

    /// Copy constructor
    /** @param toCopy Bijection to copy */
    BijectionImplementation(
        const BijectionImplementation<T1, T2, Alloc, Gen> &toCopy);

    /// generalized copy constructor
    template <typename OtherAlloc>
    BijectionImplementation(
        const BijectionImplementation<T1, T2, OtherAlloc, Gen> &toCopy);

    /// move constructor
    BijectionImplementation(
        BijectionImplementation<T1, T2, Alloc, Gen> &&from) noexcept;

    public:
    // ############################################################################
    /// @name Constructors/destructors
    // ############################################################################
    /// @{

    /// destructor
    ~BijectionImplementation();

    /// @}

    private:
    /// Copy operator
    /** @param toCopy Bijection to copy */
    BijectionImplementation<T1, T2, Alloc, Gen> &
    operator=(const BijectionImplementation<T1, T2, Alloc, Gen> &toCopy);

    /// generalized copy operator
    /** @param toCopy Bijection to copy */
    template <typename OtherAlloc>
    BijectionImplementation<T1, T2, Alloc, Gen> &
    operator=(const BijectionImplementation<T1, T2, OtherAlloc, Gen> &toCopy);

    /// move operator
    /** @param toCopy Bijection to move */
    BijectionImplementation<T1, T2, Alloc, Gen> &
    operator=(BijectionImplementation<T1, T2, Alloc, Gen> &&toCopy);

    public:
    // ############################################################################
    /// @name Iterators
    // ############################################################################
    /// @{

    /// returns the unsafe iterator at the beginning of the bijection
    /** Unsafe iterators are a little bit faster than safe ones. But this speed
     * is at the expense of safety: if you point to an element that is deleted,
     * then try to access it or trying to operate a ++ will most certainly
     * result in a segfault. So, Unsafe iterators should only be used to parse
     * bijections where no element is ever deleted. If unsure, prefer using
     * safe iterators.
     *
     * Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    iterator begin() const;

    /// returns the unsafe iterator at the beginning of the bijection
    /** Unsafe iterators are a little bit faster than safe ones. But this speed
     * is at the expense of safety: if you point to an element that is deleted,
     * then try to access it or trying to operate a ++ will most certainly
     * result in a segfault. So, Unsafe iterators should only be used to parse
     * bijections where no element is ever deleted. If unsure, prefer using
     * safe iterators.
     *
     * Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    const_iterator cbegin() const;

    /// returns the unsafe iterator to the end of the bijection
    /** Unsafe iterators are a little bit faster than safe ones. But this speed
     * is at the expense of safety: if you point to an element that is deleted,
     * then try to access it or trying to operate a ++ will most certainly
     * result in a segfault. So, Unsafe iterators should only be used to parse
     * bijections where no element is ever deleted. If unsure, prefer using
     * safe iterators.
     *
     * Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    const iterator &end() const noexcept;

    /// returns the iterator to the end of the bijection
    /** Unsafe iterators are a little bit faster than safe ones. But this speed
     * is at the expense of safety: if you point to an element that is deleted,
     * then try to access it or trying to operate a ++ will most certainly
     * result in a segfault. So, Unsafe iterators should only be used to parse
     * bijections where no element is ever deleted. If unsure, prefer using
     * safe iterators.
     *
     * Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    const const_iterator &cend() const noexcept;

    /// returns the safe iterator at the beginning of the bijection
    /** Safe iterators are slightly slower than unsafe iterators. However, they
     * guarantee that no segmentation fault can ever occur when trying to access
     * the element they point to or when applying a ++ operator. When no element
     * of the bijection is to be deleted during the parsing of the bijection
     * (as for instance when you parse the bijection to display its content),
     * prefer using the unsafe iterators, which are a little bit faster and cannot,
     * in this case, produce segfaults.
     *
     * Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = beginSafe (); iter != endSafe (); ++iter) loops will
     * parse all the associations */
    iterator_safe beginSafe() const;

    /// returns the iterator at the beginning of the bijection
    /** Safe iterators are slightly slower than unsafe iterators. However, they
     * guarantee that no segmentation fault can ever occur when trying to access
     * the element they point to or when applying a ++ operator. When no element
     * of the bijection is to be deleted during the parsing of the bijection
     * (as for instance when you parse the bijection to display its content),
     * prefer using the unsafe iterators, which are a little bit faster and cannot,
     * in this case, produce segfaults.
     *
     * Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = beginSafe (); iter != endSafe (); ++iter) loops will
     * parse all the associations */
    const_iterator_safe cbeginSafe() const;

    /// returns the safe iterator to the end of the bijection
    /** Safe iterators are slightly slower than unsafe iterators. However, they
     * guarantee that no segmentation fault can ever occur when trying to access
     * the element they point to or when applying a ++ operator. When no element
     * of the bijection is to be deleted during the parsing of the bijection
     * (as for instance when you parse the bijection to display its content),
     * prefer using the unsafe iterators, which are a little bit faster and cannot,
     * in this case, produce segfaults.
     *
     * Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    const iterator_safe &endSafe() const noexcept;

    /// returns the safe iterator to the end of the bijection
    /** Safe iterators are slightly slower than unsafe iterators. However, they
     * guarantee that no segmentation fault can ever occur when trying to access
     * the element they point to or when applying a ++ operator. When no element
     * of the bijection is to be deleted during the parsing of the bijection
     * (as for instance when you parse the bijection to display its content),
     * prefer using the unsafe iterators, which are a little bit faster and cannot,
     * in this case, produce segfaults.
     *
     * Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    const const_iterator_safe &cendSafe() const noexcept;

    /** @brief returns the safe end iterator for other classes' statics (read the
     * detailed description of this method)
     *
     * To reduce the Bijections memory consumption (which are heavily used in
     * aGrUM) while allowing fast for(iter=begin(); iter!=end();++iter) loops, end
     * iterators are created just once as a static member of a non-template
     * Bijection. While this scheme is efficient and it works quite effectively
     * when manipulating bijections, it has a drawback: other classes with static
     * members using the Bijection's end() iterator may fail to work due to the
     * well known "static initialization order fiasco" (see Marshall Cline's C++
     * FAQ for more details about this C++ feature). OK, so what is the problem?
     * Consider a class, say X, containing a Bijection that stores all its elements
     * in a convenient way. To reduce memory consumption, X::end iterator is a
     * static member that is initialized with a Bijection::end iterator. If the
     * compiler decides to initialize X::end before initializing Bijection::end,
     * then X::end will be in an incoherent state. Unfortunately, we cannot know
     * for sure in which order static members will be initialized (the order is a
     * compiler's decision). Hence, we shall enfore the fact that Bijection::end
     * is initialized before X::end. Using method Bijection::end4Statics will
     * ensure this fact: it uses the C++ "construct on first use" idiom (see the
     * C++ FAQ) that ensures that the order fiasco is avoided. More precisely,
     * end4Statics uses a global variable that is the very end iterator used by
     * all Bijections. Now, this induces a small overhead. So, we also provide a
     * Bijection::end() method that returns the Bijection::end iterator without
     * this small overhead, but assuming that function end4Statics has already
     * been called once (which is always the case) when a Bijection has been
     * created.
     *
     * So, to summarize: when initializing static members, use endSafe4Statics()
     * rather than endSafe (). In all the other cases, use simply the usual
     * method endSafe (). */
    static const iterator_safe &endSafe4Statics();

    /** @brief returns the unsafe end iterator for other classes' statics (read the
     * detailed description of this method)
     *
     * To reduce the Bijections memory consumption (which are heavily used in
     * aGrUM) while allowing fast for(iter=begin(); iter!=end();++iter) loops, end
     * iterators are created just once as a static member of a non-template
     * Bijection. While this scheme is efficient and it works quite effectively
     * when manipulating bijections, it has a drawback: other classes with static
     * members using the Bijection's end() iterator may fail to work due to the
     * well known "static initialization order fiasco" (see Marshall Cline's C++
     * FAQ for more details about this C++ feature). OK, so what is the problem?
     * Consider a class, say X, containing a Bijection that stores all its elements
     * in a convenient way. To reduce memory consumption, X::end iterator is a
     * static member that is initialized with a Bijection::end iterator. If the
     * compiler decides to initialize X::end before initializing Bijection::end,
     * then X::end will be in an incoherent state. Unfortunately, we cannot know
     * for sure in which order static members will be initialized (the order is a
     * compiler's decision). Hence, we shall enfore the fact that Bijection::end
     * is initialized before X::end. Using method Bijection::end4Statics will
     * ensure this fact: it uses the C++ "construct on first use" idiom (see the
     * C++ FAQ) that ensures that the order fiasco is avoided. More precisely,
     * end4Statics uses a global variable that is the very end iterator used by
     * all Bijections. Now, this induces a small overhead. So, we also provide a
     * Bijection::end() method that returns the Bijection::end iterator without
     * this small overhead, but assuming that function end4Statics has already
     * been called once (which is always the case) when a Bijection has been
     * created.
     *
     * So, to summarize: when initializing static members, use end4Statics() rather
     * than end(). In all the other cases, use simply the usual method end(). */
    static const iterator &end4Statics();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// returns the value associated to the element passed in argument
    /** @throws NotFound exception is thrown if the element cannot be found. */
    const T1 &first(const T2 &second) const;

    /** @brief Same method as first, but if the value is not found, a default
     * value is inserted into the bijection */
    const T1 &firstWithDefault(const T2 &second, const T1 &default_val) const;

    /// returns the value associated to the element passed in argument
    /** @throws NotFound exception is thrown if the element cannot be found. */
    const T2 &second(const T1 &first) const;

    /** @brief Same method as second, but if the value is not found, a default
     * value is inserted into the bijection */
    const T2 &secondWithDefault(const T1 &first, const T2 &default_val) const;

    /// Test whether the bijection contains the "first" value
    bool existsFirst(const T1 &first) const;

    /// Test whether the bijection contains the "second" value
    bool existsSecond(const T2 &second) const;

    /// inserts a new association in the bijection
    /** Note that what is actually inserted into the bijection is a copy of
     * the pair (first,second)
     * @throws DuplicateElement exception is thrown if the association
     * already exists */
    void insert(const T1 &first, const T2 &second);

    /// inserts a new association in the bijection
    /** @throws DuplicateElement exception is thrown if the association
     * already exists */
    void insert(T1 &&first, T2 &&second);

    /// emplace a new element in the bijection
    /** emplace is a method that allows to construct directly an element of
     * type Key by passing to its constructor all the arguments it needs
     * @param args the arguments passed to the constructor
     * @throws DuplicateElement exception is thrown if the association
     * already exists  */
    template <typename... Args> void emplace(Args &&... args);

    /// removes all the associations from the bijection
    void clear();

    /// returns true if the bijection doesn't contain any association
    bool empty() const noexcept;

    /// returns the number of associations stored within the bijection
    Size size() const noexcept;

    /// erases an association containing the given first element
    /** If the element cannot be found, nothing is done. In particular, no
     * exception is raised. */
    void eraseFirst(const T1 &first);

    /// erase an association containing the given second element
    /** If the element cannot be found, nothing is done. In particular, no
     * exception is raised. */
    void eraseSecond(const T2 &second);

    /// friendly displays the content of the CliqueGraph
    std::string toString() const;

    /// @}

    // ############################################################################
    /// @name Fine tuning
    // ############################################################################
    /// @{

    /// returns the number of hashtables' slots used (@sa hashTable's capacity)
    Size capacity() const noexcept;

    /// similar to the hashtable's resize
    void resize(Size new_size);

    /// enables the user to change dynamically the resizing policy
    /** @sa HashTable's setResizePolicy */
    void setResizePolicy(const bool new_policy) noexcept;

    /// returns the current resizing policy
    /** @sa HashTable's resizePolicy */
    bool resizePolicy() const noexcept;

    /// @}

    private:
    using HashTable12 = HashTable<T1, T2 *, allocator12_type>;
    using HashTable21 = HashTable<T2, T1 *, allocator21_type>;

    /// a friend to speed-up accesses
    friend class BijectionIteratorSafe<T1, T2>;
    friend class BijectionIterator<T1, T2>;
    friend class Bijection<T1, T2, Alloc>;
    template <typename TT1, typename TT2, typename A, bool>
    friend class BijectionImplementation;

    // below, we create the two hashtables used by the bijection. Note that
    // the values of these hashtables are actually pointers. This enables to
    // create only once objects (T1,T2). When using bijections with large
    // size objects, this feature is of particular interest

    /// hashtable associating T2 objects to T1 objects
    HashTable12 __firstToSecond;

    /// hashtable associating T1 objects to T2 objects
    HashTable21 __secondToFirst;

    /// a function that performs a complete copy of another bijection
    /** @warning this function assumes that "this" is an empty bijection. If this
     * is not the case, use function clear() before calling __copy. */
    template <typename OtherAlloc>
    void __copy(const HashTable<T1, T2 *, OtherAlloc> &f2s);

    /// inserts a new association into the bijection
    typename HashTable12::value_type *__insert(const T1 &first, const T2 &second);

    /// inserts a new association into the bijection
    typename HashTable12::value_type *__insert(T1 &&first, T2 &&second);
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  /* =========================================================================== */
  /* ===                  SCALAR BIJECTION IMPLEMENTATION                    === */
  /* =========================================================================== */
  /**
   * This class is designed for modeling a bijection between two sets, the idea is
   * following :
   * - we want to create a bijection relation between type T1* and type T2*
   * - for x in T1*, there exists only one y in T2* associated to x
   * - for y in T2*, there exists only one x in T1* associated to y
   * - the user inserts all the (x, y) associations and can search efficiently the
   * values thus associated
   */
  template <typename T1, typename T2, typename Alloc>
  class BijectionImplementation<T1, T2, Alloc, true> {
    public:
    /// types for STL compliance
    /// @{
    using type1_type = T1;
    using type1_reference = T1 &;
    using type1_const_reference = const T1 &;
    using type1_pointer = T1 *;
    using type1_const_pointer = const T1 *;
    using type2_type = T2;
    using type2_reference = T2 &;
    using type2_const_reference = const T2 &;
    using type2_pointer = T2 *;
    using type2_const_pointer = const T2 *;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = Alloc;
    using iterator = BijectionIterator<T1, T2>;
    using const_iterator = BijectionIterator<T1, T2>;
    using iterator_safe = BijectionIteratorSafe<T1, T2>;
    using const_iterator_safe = BijectionIteratorSafe<T1, T2>;

    using allocator12_type =
        typename Alloc::template rebind<std::pair<T1, T2>>::other;
    using allocator21_type =
        typename Alloc::template rebind<std::pair<T2, T1>>::other;
    /// @}

    private:
    /// Default constructor: creates a bijection without association
    BijectionImplementation(Size size, bool resize_policy);

    /// initializer list constructor
    BijectionImplementation(std::initializer_list<std::pair<T1, T2>> list);

    /// Copy constructor
    /** @param toCopy Bijection to copy */
    BijectionImplementation(
        const BijectionImplementation<T1, T2, Alloc, true> &toCopy);

    /// Generalized copy constructor
    /** @param toCopy Bijection to copy */
    template <typename OtherAlloc>
    BijectionImplementation(
        const BijectionImplementation<T1, T2, OtherAlloc, true> &toCopy);

    /// move constructor
    BijectionImplementation(
        BijectionImplementation<T1, T2, Alloc, true> &&from) noexcept;

    public:
    // ############################################################################
    /// @name Constructors/destructors
    // ############################################################################
    /// @{

    /// destructor
    ~BijectionImplementation();

    /// @}

    private:
    /// Copy operator
    /** @param toCopy Bijection to copy */
    BijectionImplementation<T1, T2, Alloc, true> &
    operator=(const BijectionImplementation<T1, T2, Alloc, true> &toCopy);

    /// Generalized copy operator
    /** @param toCopy Bijection to copy */
    template <typename OtherAlloc>
    BijectionImplementation<T1, T2, Alloc, true> &
    operator=(const BijectionImplementation<T1, T2, OtherAlloc, true> &toCopy);

    /// move operator
    BijectionImplementation<T1, T2, Alloc, true> &
    operator=(BijectionImplementation<T1, T2, Alloc, true> &&from);

    public:
    // ############################################################################
    /// @name Iterators
    // ############################################################################
    /// @{

    /// returns the unsafe iterator at the beginning of the bijection
    /** Unsafe iterators are a little bit faster than safe ones. But this speed
     * is at the expense of safety: if you point to an element that is deleted,
     * then try to access it or trying to operate a ++ will most certainly
     * result in a segfault. So, Unsafe iterators should only be used to parse
     * bijections where no element is ever deleted. If unsure, prefer using
     * safe iterators.
     *
     * Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    iterator begin() const;

    /// returns the unsafe iterator at the beginning of the bijection
    /** Unsafe iterators are a little bit faster than safe ones. But this speed
     * is at the expense of safety: if you point to an element that is deleted,
     * then try to access it or trying to operate a ++ will most certainly
     * result in a segfault. So, Unsafe iterators should only be used to parse
     * bijections where no element is ever deleted. If unsure, prefer using
     * safe iterators.
     *
     * Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    const_iterator cbegin() const;

    /// returns the unsafe iterator to the end of the bijection
    /** Unsafe iterators are a little bit faster than safe ones. But this speed
     * is at the expense of safety: if you point to an element that is deleted,
     * then try to access it or trying to operate a ++ will most certainly
     * result in a segfault. So, Unsafe iterators should only be used to parse
     * bijections where no element is ever deleted. If unsure, prefer using
     * safe iterators.
     *
     * Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    const iterator &end() const noexcept;

    /// returns the iterator to the end of the bijection
    /** Unsafe iterators are a little bit faster than safe ones. But this speed
     * is at the expense of safety: if you point to an element that is deleted,
     * then try to access it or trying to operate a ++ will most certainly
     * result in a segfault. So, Unsafe iterators should only be used to parse
     * bijections where no element is ever deleted. If unsure, prefer using
     * safe iterators.
     *
     * Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    const const_iterator &cend() const noexcept;

    /// returns the safe iterator at the beginning of the bijection
    /** Safe iterators are slightly slower than unsafe iterators. However, they
     * guarantee that no segmentation fault can ever occur when trying to access
     * the element they point to or when applying a ++ operator. When no element
     * of the bijection is to be deleted during the parsing of the bijection
     * (as for instance when you parse the bijection to display its content),
     * prefer using the unsafe iterators, which are a little bit faster and cannot,
     * in this case, produce segfaults.
     *
     * Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = beginSafe (); iter != endSafe (); ++iter) loops will
     * parse all the associations */
    iterator_safe beginSafe() const;

    /// returns the iterator at the beginning of the bijection
    /** Safe iterators are slightly slower than unsafe iterators. However, they
     * guarantee that no segmentation fault can ever occur when trying to access
     * the element they point to or when applying a ++ operator. When no element
     * of the bijection is to be deleted during the parsing of the bijection
     * (as for instance when you parse the bijection to display its content),
     * prefer using the unsafe iterators, which are a little bit faster and cannot,
     * in this case, produce segfaults.
     *
     * Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = beginSafe (); iter != endSafe (); ++iter) loops will
     * parse all the associations */
    const_iterator_safe cbeginSafe() const;

    /// returns the safe iterator to the end of the bijection
    /** Safe iterators are slightly slower than unsafe iterators. However, they
     * guarantee that no segmentation fault can ever occur when trying to access
     * the element they point to or when applying a ++ operator. When no element
     * of the bijection is to be deleted during the parsing of the bijection
     * (as for instance when you parse the bijection to display its content),
     * prefer using the unsafe iterators, which are a little bit faster and cannot,
     * in this case, produce segfaults.
     *
     * Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    const iterator_safe &endSafe() const noexcept;

    /// returns the safe iterator to the end of the bijection
    /** Safe iterators are slightly slower than unsafe iterators. However, they
     * guarantee that no segmentation fault can ever occur when trying to access
     * the element they point to or when applying a ++ operator. When no element
     * of the bijection is to be deleted during the parsing of the bijection
     * (as for instance when you parse the bijection to display its content),
     * prefer using the unsafe iterators, which are a little bit faster and cannot,
     * in this case, produce segfaults.
     *
     * Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    const const_iterator_safe &cendSafe() const noexcept;

    /** @brief returns the safe end iterator for other classes' statics (read the
     * detailed description of this method)
     *
     * To reduce the Bijections memory consumption (which are heavily used in
     * aGrUM) while allowing fast for(iter=begin(); iter!=end();++iter) loops, end
     * iterators are created just once as a static member of a non-template
     * Bijection. While this scheme is efficient and it works quite effectively
     * when manipulating bijections, it has a drawback: other classes with static
     * members using the Bijection's end() iterator may fail to work due to the
     * well known "static initialization order fiasco" (see Marshall Cline's C++
     * FAQ for more details about this C++ feature). OK, so what is the problem?
     * Consider a class, say X, containing a Bijection that stores all its elements
     * in a convenient way. To reduce memory consumption, X::end iterator is a
     * static member that is initialized with a Bijection::end iterator. If the
     * compiler decides to initialize X::end before initializing Bijection::end,
     * then X::end will be in an incoherent state. Unfortunately, we cannot know
     * for sure in which order static members will be initialized (the order is a
     * compiler's decision). Hence, we shall enfore the fact that Bijection::end
     * is initialized before X::end. Using method Bijection::end4Statics will
     * ensure this fact: it uses the C++ "construct on first use" idiom (see the
     * C++ FAQ) that ensures that the order fiasco is avoided. More precisely,
     * end4Statics uses a global variable that is the very end iterator used by
     * all Bijections. Now, this induces a small overhead. So, we also provide a
     * Bijection::end() method that returns the Bijection::end iterator without
     * this small overhead, but assuming that function end4Statics has already
     * been called once (which is always the case) when a Bijection has been
     * created.
     *
     * So, to summarize: when initializing static members, use endSafe4Statics()
     * rather than endSafe (). In all the other cases, use simply the usual
     * method endSafe (). */
    static const iterator_safe &endSafe4Statics();

    /** @brief returns the unsafe end iterator for other classes' statics (read the
     * detailed description of this method)
     *
     * To reduce the Bijections memory consumption (which are heavily used in
     * aGrUM) while allowing fast for(iter=begin(); iter!=end();++iter) loops, end
     * iterators are created just once as a static member of a non-template
     * Bijection. While this scheme is efficient and it works quite effectively
     * when manipulating bijections, it has a drawback: other classes with static
     * members using the Bijection's end() iterator may fail to work due to the
     * well known "static initialization order fiasco" (see Marshall Cline's C++
     * FAQ for more details about this C++ feature). OK, so what is the problem?
     * Consider a class, say X, containing a Bijection that stores all its elements
     * in a convenient way. To reduce memory consumption, X::end iterator is a
     * static member that is initialized with a Bijection::end iterator. If the
     * compiler decides to initialize X::end before initializing Bijection::end,
     * then X::end will be in an incoherent state. Unfortunately, we cannot know
     * for sure in which order static members will be initialized (the order is a
     * compiler's decision). Hence, we shall enfore the fact that Bijection::end
     * is initialized before X::end. Using method Bijection::end4Statics will
     * ensure this fact: it uses the C++ "construct on first use" idiom (see the
     * C++ FAQ) that ensures that the order fiasco is avoided. More precisely,
     * end4Statics uses a global variable that is the very end iterator used by
     * all Bijections. Now, this induces a small overhead. So, we also provide a
     * Bijection::end() method that returns the Bijection::end iterator without
     * this small overhead, but assuming that function end4Statics has already
     * been called once (which is always the case) when a Bijection has been
     * created.
     *
     * So, to summarize: when initializing static members, use end4Statics() rather
     * than end(). In all the other cases, use simply the usual method end(). */
    static const iterator &end4Statics();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// returns the value associated to the element passed in argument
    /** @throws NotFound exception is thrown if the element cannot be found. */
    const T1 &first(const T2 second) const;

    /** @brief Same method as first, but if the value is not found, a default
     * value is inserted into the bijection */
    const T1 &firstWithDefault(const T2 second, const T1 default_val) const;

    /// returns the value associated to the element passed in argument
    /** @throws NotFound exception is thrown if the element cannot be found. */
    const T2 &second(const T1 first) const;

    /** @brief Same method as second, but if the value is not found, a default
     * value is inserted into the bijection */
    const T2 &secondWithDefault(const T1 first, const T2 default_val) const;

    /// Test whether the bijection contains the "first" value
    bool existsFirst(const T1 first) const;

    /// Test whether the bijection contains the "second" value
    bool existsSecond(const T2 second) const;

    /// inserts a new association in the bijection
    /**@throws DuplicateElement exception is thrown if the association
     * already exists */
    void insert(const T1 first, const T2 second);

    /// emplace a new element in the bijection
    /** emplace is a method that allows to construct directly an element of
     * type Key by passing to its constructor all the arguments it needs
     * @param args the arguments passed to the constructor
     * @throws DuplicateElement exception is thrown if the association
     * already exists  */
    template <typename... Args> void emplace(Args &&... args);

    /// removes all the associations from the bijection
    void clear();

    /// returns true if the bijection doesn't contain any association
    bool empty() const noexcept;

    /// returns the number of associations stored within the bijection
    Size size() const noexcept;

    /// erases an association containing the given first element
    /** If the element cannot be found, nothing is done. In particular, no
     * exception is raised. */
    void eraseFirst(const T1 first);

    /// erase an association containing the given second element
    /** If the element cannot be found, nothing is done. In particular, no
     * exception is raised. */
    void eraseSecond(const T2 second);

    /// friendly displays the content of the bijection
    std::string toString() const;

    /// @}

    // ############################################################################
    /// @name Fine tuning
    // ############################################################################
    /// @{

    /// returns the number of hashtables' slots used (@sa hashTable's capacity)
    Size capacity() const noexcept;

    /// similar to the hashtable's resize
    void resize(Size new_size);

    /// enables the user to change dynamically the resizing policy
    /** @sa HashTable's setResizePolicy */
    void setResizePolicy(const bool new_policy) noexcept;

    /// returns the current resizing policy
    /** @sa HashTable's resizePolicy */
    bool resizePolicy() const noexcept;

    /// @}

    private:
    using HashTable12 = HashTable<T1, T2, allocator12_type>;
    using HashTable21 = HashTable<T2, T1, allocator21_type>;

    /// a friend to speed-up accesses
    friend class BijectionIteratorSafe<T1, T2>;
    friend class BijectionIterator<T1, T2>;
    friend class Bijection<T1, T2, Alloc>;
    template <typename TT1, typename TT2, typename A, bool>
    friend class BijectionImplementation;

    /// hashtable associating T2 scalars to T1 scalars
    HashTable12 __firstToSecond;

    /// hashtable associating T1 scalars to T2 scalars
    HashTable21 __secondToFirst;

    /// a function that performs a complete copy of another bijection
    /** @warning this function assumes that "this" is an empty bijection. If this
     * is not the case, use function clear() before calling __copy. */
    template <typename OtherAlloc>
    void __copy(const HashTable<T1, T2, OtherAlloc> &f2s);

    /// inserts a new association in the bijection
    void __insert(const T1 first, const T2 second);
  };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  template <typename T1, typename T2, typename Alloc = std::allocator<T2>>
  class Bijection
      : public BijectionImplementation<
            T1, T2, Alloc, std::is_scalar<T1>::value && std::is_scalar<T2>::value> {
    public:
    /// types for STL compliance
    /// @{
    using type1_type = T1;
    using type1_reference = T1 &;
    using type1_const_reference = const T1 &;
    using type1_pointer = T1 *;
    using type1_const_pointer = const T1 *;
    using type2_type = T2;
    using type2_reference = T2 &;
    using type2_const_reference = const T2 &;
    using type2_pointer = T2 *;
    using type2_const_pointer = const T2 *;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = Alloc;
    using iterator = BijectionIterator<T1, T2>;
    using const_iterator = BijectionIterator<T1, T2>;
    using iterator_safe = BijectionIteratorSafe<T1, T2>;
    using const_iterator_safe = BijectionIteratorSafe<T1, T2>;

    using allocator1_type = typename Alloc::template rebind<T1 *>::other;
    using allocator2_type = typename Alloc::template rebind<T2 *>::other;
    /// @}

    using Implementation = BijectionImplementation<
        T1, T2, Alloc, std::is_scalar<T1>::value && std::is_scalar<T2>::value>;

    // ############################################################################
    /// @name Constructors/destructors
    // ############################################################################
    /// @{

    /// Default constructor: creates a bijection without any association
    Bijection(Size size = HashTableConst::default_size,
              bool resize_policy = HashTableConst::default_resize_policy);

    /// initializer list constructor
    Bijection(std::initializer_list<std::pair<T1, T2>> list);

    /// Copy constructor
    /** @param toCopy Bijection to copy */
    Bijection(const Bijection<T1, T2, Alloc> &toCopy);

    /// Generalized copy constructor
    /** @param toCopy Bijection to copy */
    template <typename OtherAlloc>
    Bijection(const Bijection<T1, T2, OtherAlloc> &toCopy);

    /// move constructor
    Bijection(Bijection<T1, T2, Alloc> &&from) noexcept;

    /// destructor
    ~Bijection();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    Bijection<T1, T2, Alloc> &operator=(const Bijection<T1, T2, Alloc> &toCopy);

    /// generalized copy operator
    template <typename OtherAlloc>
    Bijection<T1, T2, Alloc> &operator=(const Bijection<T1, T2, OtherAlloc> &toCopy);

    /// move operator
    Bijection<T1, T2, Alloc> &operator=(Bijection<T1, T2, Alloc> &&bij);

    /// @}
  };

  /* =========================================================================== */
  /* ===                      BIJECTION SAFE ITERATORS                       === */
  /* =========================================================================== */
  /** @class BijectionIteratorSafe
   * @brief Safe iterators for bijection
   * @ingroup basicstruct_group
   */
  template <typename T1, typename T2> class BijectionIteratorSafe {

    template <typename TT1, typename TT2, typename Alloc, bool>
    friend class BijectionImplementation;

    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using type1_type = T1;
    using type1_reference = T1 &;
    using type1_const_reference = const T1 &;
    using type1_pointer = T1 *;
    using type1_const_pointer = const T1 *;
    using type2_type = T2;
    using type2_reference = T2 &;
    using type2_const_reference = const T2 &;
    using type2_pointer = T2 *;
    using type2_const_pointer = const T2 *;
    using difference_type = std::ptrdiff_t;
    /// @}

    private:
    // dummy classes that will enable discriminate without overhead between
    // scalars and non-scalars functions second in iterators
    using Getter =
        BijectionIteratorGet<std::is_scalar<T1>::value && std::is_scalar<T2>::value>;

    /// begin constructor
    /** By default, the iterator points to the starting point of the bijection */
    template <typename Alloc, bool Gen>
    BijectionIteratorSafe(
        const BijectionImplementation<T1, T2, Alloc, Gen> &bijection);

    public:
    // ############################################################################
    /// @name Constructors/destructors
    // ############################################################################
    /// @{

    /// Default constructor
    BijectionIteratorSafe() noexcept;

    /// Default constructor
    template <typename Alloc>
    BijectionIteratorSafe(const Bijection<T1, T2, Alloc> &bijection);

    /// Copy constructor
    BijectionIteratorSafe(const BijectionIteratorSafe<T1, T2> &from);

    /// move constructor
    BijectionIteratorSafe(BijectionIteratorSafe<T1, T2> &&from) noexcept;

    /// Destructor
    ~BijectionIteratorSafe() noexcept;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// Copy operator
    BijectionIteratorSafe<T1, T2> &
    operator=(const BijectionIteratorSafe<T1, T2> &toCopy);

    /// move operator
    BijectionIteratorSafe<T1, T2> &
    operator=(BijectionIteratorSafe<T1, T2> &&toCopy) noexcept;

    /// Go to the next association (if it exists)
    /** @warning if the iterator points to end(), nothing is done */
    BijectionIteratorSafe<T1, T2> &operator++() noexcept;

    /// moves the iterator by nb elements
    /** @warning if the iterator points to end(), nothing is done. If there are
     * nb or fewer elements to parse to reach the end of the bijection, then
     * this method makes the iterator point to end () */
    BijectionIteratorSafe<T1, T2> &operator+=(unsigned int nb) noexcept;

    /// returns a new iterator
    /** @warning if the iterator points to end(), the resulting iterator also
     * points to end (). If there are nb or fewer elements to parse to reach the
     * end of the bijection, then the resulting iterator points to end () */
    BijectionIteratorSafe<T1, T2> operator+(unsigned int nb) noexcept;

    /// Comparison of iterators
    bool operator!=(const BijectionIteratorSafe<T1, T2> &toCompare) const noexcept;

    /// Comparison of iterators
    bool operator==(const BijectionIteratorSafe<T1, T2> &toCompare) const noexcept;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// returns the first element of the current association
    /** @throws UndefinedIteratorValue exception is thrown when the iterator does
     * not point to a valid element of the bijection */
    const T1 &first() const;

    /// returns the second element of the current association
    /** @throws UndefinedIteratorValue exception is thrown when the iterator does
     * not point to a valid element of the bijection */
    const T2 &second() const;

    /// @}

    private:
    using HashTable12 = typename std::conditional<
        std::is_scalar<T1>::value && std::is_scalar<T2>::value,
        HashTable<T1, T2, std::allocator<std::pair<T1, T2>>>,
        HashTable<T1, T2 *, std::allocator<std::pair<T1, T2 *>>>>::type;
    using HashIter = typename HashTable12::const_iterator_safe;

    /// the hashTable iterator that actually does all the job
    HashIter __iter;
  };

  /* =========================================================================== */
  /* ===                     BIJECTION UNSAFE ITERATORS                      === */
  /* =========================================================================== */
  /** @class BijectionIterator
   * @brief Unsafe iterators for bijection
   * @ingroup basicstruct_group
   */
  template <typename T1, typename T2> class BijectionIterator {

    template <typename TT1, typename TT2, typename Alloc, bool>
    friend class BijectionImplementation;

    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using type1_type = T1;
    using type1_reference = T1 &;
    using type1_const_reference = const T1 &;
    using type1_pointer = T1 *;
    using type1_const_pointer = const T1 *;
    using type2_type = T2;
    using type2_reference = T2 &;
    using type2_const_reference = const T2 &;
    using type2_pointer = T2 *;
    using type2_const_pointer = const T2 *;
    using difference_type = std::ptrdiff_t;
    /// @}

    private:
    // dummy classes that will enable discriminate without overhead between
    // scalars and non-scalars functions second in iterators
    using Getter =
        BijectionIteratorGet<std::is_scalar<T1>::value && std::is_scalar<T2>::value>;

    /// begin constructor
    /** By default, the iterator points to the starting point of the bijection */
    template <typename Alloc, bool Gen>
    BijectionIterator(const BijectionImplementation<T1, T2, Alloc, Gen> &bijection);

    public:
    // ############################################################################
    /// @name Constructors/destructors
    // ############################################################################
    /// @{

    /// Default constructor
    BijectionIterator() noexcept;

    /// Default constructor
    template <typename Alloc>
    BijectionIterator(const Bijection<T1, T2, Alloc> &bijection);

    /// Copy constructor
    BijectionIterator(const BijectionIterator<T1, T2> &from);

    /// move constructor
    BijectionIterator(BijectionIterator<T1, T2> &&from) noexcept;

    /// Destructor
    ~BijectionIterator() noexcept;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// Copy operator
    BijectionIterator<T1, T2> &operator=(const BijectionIterator<T1, T2> &toCopy);

    /// move operator
    BijectionIterator<T1, T2> &
    operator=(BijectionIterator<T1, T2> &&toCopy) noexcept;

    /// Go to the next association (if it exists)
    /** @warning if the iterator points to end(), nothing is done */
    BijectionIterator<T1, T2> &operator++() noexcept;

    /// moves the iterator by nb elements
    /** @warning if the iterator points to end(), nothing is done. If there are
     * nb or fewer elements to parse to reach the end of the bijection, then
     * this method makes the iterator point to end () */
    BijectionIterator<T1, T2> &operator+=(unsigned int nb) noexcept;

    /// returns a new iterator
    /** @warning if the iterator points to end(), the resulting iterator also
     * points to end (). If there are nb or fewer elements to parse to reach the
     * end of the bijection, then the resulting iterator points to end () */
    BijectionIterator<T1, T2> operator+(unsigned int nb) noexcept;

    /// Comparison of iterators
    bool operator!=(const BijectionIterator<T1, T2> &toCompare) const noexcept;

    /// Comparison of iterators
    bool operator==(const BijectionIterator<T1, T2> &toCompare) const noexcept;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// returns the first element of the current association
    /** @throws UndefinedIteratorValue exception is thrown when the iterator does
     * not point to a valid element of the bijection */
    const T1 &first() const;

    /// returns the second element of the current association
    /** @throws UndefinedIteratorValue exception is thrown when the iterator does
     * not point to a valid element of the bijection */
    const T2 &second() const;

    /// @}

    private:
    using HashTable12 = typename std::conditional<
        std::is_scalar<T1>::value && std::is_scalar<T2>::value,
        HashTable<T1, T2, std::allocator<std::pair<T1, T2>>>,
        HashTable<T1, T2 *, std::allocator<std::pair<T1, T2 *>>>>::type;
    using HashIter = typename HashTable12::const_iterator;

    /// the hashTable iterator that actually does all the job
    HashIter __iter;
  };

  /// for friendly displaying the content of bijections
  template <typename T1, typename T2, typename Alloc>
  std::ostream &operator<<(std::ostream &, const Bijection<T1, T2, Alloc> &);

} /* namespace gum */

#include <agrum/core/bijection.tcc>

#endif /* GUM_BIJECTION_H */
