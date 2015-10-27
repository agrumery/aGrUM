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

/**
 * @file
 * @brief Set of pairs of elements with fast search for both elements.
 *
 * Bijections are some kind of sets of pairs (T1,T2) with the additional
 * feature as compared to Sets: we can search very quickly T2's elements when
 * knowing T1 and T1's elements when knowing T2.
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

  template <typename T1, typename T2>
  class BijectionIteratorSafe;
  template <typename T1, typename T2>
  class BijectionIterator;
  template <typename T1, typename T2, typename Alloc, bool>
  class BijectionImplementation;
  template <typename T1, typename T2, typename Alloc>
  class Bijection;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


  // ===========================================================================
  // ===                NON SCALAR BIJECTION IMPLEMENTATION                  ===
  // ===========================================================================

  /**
   * @class BijectionImplementation bijection.h <agrum/core/bijection.h>
   * @brief A non scalar implementation of a Bijection.
   * @ingroup bijection_group
   *
   * This class is designed for modeling a gum::Bijection between two sets, the
   * idea is following :
   *  - We want to create a gum::Bjection relation between type T1 and type T2,
   *  - For x in T1, there exists only one y in T2 associated to x,
   *  - For y in T2, there exists only one x in T1 associated to y,
   *  - The user inserts all the (x, y) associations and can search efficiently
   *  the values thus associated.
   *
   * @tparam T1 The first type of elements in the gum::Bjection.
   * @tparam T2 The second type of elements in the gum::Bjection.
   * @tparam Alloc The allocator used for allocating memory.
   * @tparam Gen If true, this will be replaced by a implementation omptimized
   * for non-scalar types.
   */
  template <typename T1, typename T2, typename Alloc, bool Gen>
  class BijectionImplementation {

    public:

    /// types for STL compliance
    /// @{
    using type1_type = T1;
    using type1_reference = T1&;
    using type1_const_reference = const T1&;
    using type1_pointer = T1*;
    using type1_const_pointer = const T1*;
    using type2_type = T2;
    using type2_reference = T2&;
    using type2_const_reference = const T2&;
    using type2_pointer = T2*;
    using type2_const_pointer = const T2*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = Alloc;
    using iterator = BijectionIterator<T1, T2>;
    using const_iterator = BijectionIterator<T1, T2>;
    using iterator_safe = BijectionIteratorSafe<T1, T2>;
    using const_iterator_safe = BijectionIteratorSafe<T1, T2>;
    using allocator12_type =
        typename Alloc::template rebind<std::pair<T1, T2*>>::other;
    using allocator21_type =
        typename Alloc::template rebind<std::pair<T2, T1*>>::other;
    /// @}

    private:

    /**
     * @brief Default constructor: creates a gum::Bijection without any
     * association.
     * @param size The Bijection starting size.
     * @param resize_policy If true, the gum::Bijection will resize itself
     * automatically.
     */
    BijectionImplementation( Size size, bool resize_policy );

    /**
     * @brief Initializer list constructor.
     * @param list The initialize list.
     */
    BijectionImplementation( std::initializer_list<std::pair<T1, T2>> list );

    /**
     * @brief Copy constructor.
     * @param toCopy Bijection to copy.
     */
    BijectionImplementation(
        const BijectionImplementation<T1, T2, Alloc, Gen>& toCopy );

    /**
     * @brief Generalized copy constructor.
     * @param toCopy Bijection to copy.
     */
    template <typename OtherAlloc>
    BijectionImplementation(
        const BijectionImplementation<T1, T2, OtherAlloc, Gen>& toCopy );

    /**
     * @brief Move constructor.
     * @param from Bijection to move.
     */
    BijectionImplementation(
        BijectionImplementation<T1, T2, Alloc, Gen>&& from ) noexcept;

    public:
 
    // ============================================================================
    /// @name Constructors/destructors
    // ============================================================================
    /// @{

    /**
     * @brief Destructor.
     */
    ~BijectionImplementation();

    /// @}

    private:

    /**
     * @brief Copy operator.
     * @param toCopy Bijection to copy.
     * @return Returns the gum::Bijection in which the copy was made.
     */
    BijectionImplementation<T1, T2, Alloc, Gen>&
    operator=( const BijectionImplementation<T1, T2, Alloc, Gen>& toCopy );

    /**
     * @brief Generalized copy operator.
     * @param toCopy Bijection to copy.
     * @return Returns the gum::Bijection in which the copy was made.
     */
    template <typename OtherAlloc>
    BijectionImplementation<T1, T2, Alloc, Gen>&
    operator=( const BijectionImplementation<T1, T2, OtherAlloc, Gen>& toCopy );

    /**
     * @brief Move operator.
     * @param toCopy Bijection to move
     * @return Returns the moved gum::Bijection in which the move was made.
     */
    BijectionImplementation<T1, T2, Alloc, Gen>&
    operator=( BijectionImplementation<T1, T2, Alloc, Gen>&& toCopy );

    public:

    // ============================================================================
    /// @name Iterators
    // ============================================================================
    /// @{

    /** 
     * @brief Returns the unsafe iterator at the beginning of the
     * gum::Bijection.
     *
     * Unsafe iterators are a little bit faster than safe ones. But this speed
     * is at the expense of safety: if you point to an element that is deleted,
     * then try to access it or trying to operate a ++ will most certainly
     * result in a segfault. So, Unsafe iterators should only be used to parse
     * gum::Bjection where no element is ever deleted. If unsure, prefer using
     * safe iterators.
     *
     * Note that the notion of a beginning/end of a gum::Bjection is rather fuzzy.
     * What is important here is that for an instance bij of this class:
     *
     * @code
     * for(iterator iter = bij.begin(); iter != bij.end(); ++iter) {
     *   // will parse all the associations.
     * }
     * @endcode
     */
    iterator begin() const;
    
    /**
     * @brief Returns the constant unsafe iterator at the beginning of the
     * gum::Bjection.
     *
     * Unsafe iterators are a little bit faster than safe ones. But this speed
     * is at the expense of safety: if you point to an element that is deleted,
     * then try to access it or trying to operate a ++ will most certainly
     * result in a segfault. So, Unsafe iterators should only be used to parse
     * gum::Bjection where no element is ever deleted. If unsure, prefer using
     * safe iterators.
     *
     * Note that the notion of a beginning/end of a gum::Bjection is rather
     * fuzzy.  What is important here is that for an instance bij of this
     * class:
     *
     * @code
     * for (iterator iter = bij.cbegin(); iter != bij.cend(); ++iter) {
     *   // will parse all the association
     * }
     * @endcode
     */
    const_iterator cbegin() const;

    /** 
     * @brief Returns the unsafe iterator at the end of the gum::Bijection.
     *
     * Unsafe iterators are a little bit faster than safe ones. But this speed
     * is at the expense of safety: if you point to an element that is deleted,
     * then try to access it or trying to operate a ++ will most certainly
     * result in a segfault. So, Unsafe iterators should only be used to parse
     * gum::Bijection where no element is ever deleted. If unsure, prefer using
     * safe iterators.
     *
     * Note that the notion of a beginning/end of a gum::Bijection is rather
     * fuzzy.  What is important here is that for an instance bij of this
     * class:
     *
     * @code
     * for(iterator iter = bij.begin(); iter != bij.end(); ++iter) {
     *   // loops will parse all the associations 
     * }
     * @endcode
     */
    const iterator& end() const noexcept;

    /** 
     * @brief Returns the constant iterator at the end of the gum::Bijection.
     *
     * Unsafe iterators are a little bit faster than safe ones. But this speed
     * is at the expense of safety: if you point to an element that is deleted,
     * then try to access it or trying to operate a ++ will most certainly
     * result in a segfault. So, Unsafe iterators should only be used to parse
     * gum::Bijection where no element is ever deleted. If unsure, prefer using
     * safe iterators.
     *
     * Note that the notion of a beginning/end of a gum::Bijection is rather fuzzy.
     * What is important here is that for an instance bij of this class:
     *
     * @code
     * for (iterator iter = bij.cbegin(); iter != bij.cend(); ++iter) {
     *   // loops will parse all the associations
     * }
     * @endcode
     */
    const const_iterator& cend() const noexcept;

    /**
     * @brief Returns the safe iterator at the beginning of the gum::Bijection.
     *
     * Safe iterators are slightly slower than unsafe iterators. However, they
     * guarantee that no segmentation fault can ever occur when trying to
     * access the element they point to or when applying a ++ operator. When no
     * element of the gum::Bijection is to be deleted during the parsing of the
     * gum::Bijection (as for instance when you parse the gum::Bijection to
     * display its content), prefer using the unsafe iterators, which are a
     * little bit faster and cannot, in this case, produce segfaults.
     *
     * Note that the notion of a beginning/end of a gum::Bijection is rather
     * fuzzy.  What is important here is that for an instance bij of this
     * class:
     *
     * @code
     * for (iterator iter = bij.beginSafe(); iter != bij.endSafe(); ++iter) {
     *   // loops will parse all the associations
     * }
     * @endcode
     */
    iterator_safe beginSafe() const;

    /**
     * @brief Returns the constant safe iterator at the begining of the
     * gum::Bijection.
     *
     * Safe iterators are slightly slower than unsafe iterators. However, they
     * guarantee that no segmentation fault can ever occur when trying to
     * access the element they point to or when applying a ++ operator. When no
     * element of the gum::Bijection is to be deleted during the parsing of the
     * gum::Bijection (as for instance when you parse the bijection to display
     * its content), prefer using the unsafe iterators, which are a little bit
     * faster and cannot, in this case, produce segfaults.
     *
     * Note that the notion of a beginning/end of a gum::Bijection is rather
     * fuzzy.  What is important here is that for an instance bij of this
     * class:
     *
     * @code
     * for (iterator iter = bij.cbeginSafe(); iter != bij.cendSafe(); ++iter) {
     *   // loops will parse all the associations 
     * }
     * @encode
     */
    const_iterator_safe cbeginSafe() const;

    /** 
     * @brief Returns the safe iterator at the end of the gum::Bijection.
     *
     * Safe iterators are slightly slower than unsafe iterators. However, they
     * guarantee that no segmentation fault can ever occur when trying to
     * access the element they point to or when applying a ++ operator. When no
     * element of the gum::Bijection is to be deleted during the parsing of the
     * gum::Bijection (as for instance when you parse the gum::Bijection to
     * display its content), prefer using the unsafe iterators, which are a
     * little bit faster and cannot, in this case, produce segfaults.
     *
     * Note that the notion of a beginning/end of a gum::Bijection is rather fuzzy.
     * What is important here is that for an instance bij of this class:
     *
     * @code
     * for (iterator iter = bij.beginSafe(); iter != bij.endSafe(); ++iter) {
     *   // loops will parse all the associations 
     * }
     * @endcode
     */
    const iterator_safe& endSafe() const noexcept;

    /**
     * @brief Returns the constant safe iterator at the end of the
     * gum::Bijection.
     *
     * Safe iterators are slightly slower than unsafe iterators. However, they
     * guarantee that no segmentation fault can ever occur when trying to
     * access the element they point to or when applying a ++ operator. When no
     * element of the gum::Bijection is to be deleted during the parsing of the
     * gum::Bijection (as for instance when you parse the gum::Bijection to
     * display its content), prefer using the unsafe iterators, which are a
     * little bit faster and cannot, in this case, produce segfaults.
     *
     * Note that the notion of a beginning/end of a gum::Bijection is rather
     * fuzzy.  What is important here is that for an instance bij of this
     * class:
     *
     * @code
     * for (iterator iter = bij.cbeginSafe(); iter != bij.cendSafe(); ++iter) {
     *   // loops will parse all the associations 
     * }
     * @endcode
     */
    const const_iterator_safe& cendSafe() const noexcept;

    /**
     * @brief Returns the safe end iterator for other classes' statics.
     *
     * To reduce the gum::Bijection memory consumption (which are heavily used
     * in aGrUM) while allowing fast for loops, end iterators are created just
     * once as a static member of a non-template gum::Bijection. While this
     * scheme is efficient and it works quite effectively, it has a drawback:
     * other classes with static members using the
     * BijectionImplementation::end() iterator may fail to work due to the well
     * known "static initialization order fiasco" (see Marshall Cline's C++ FAQ
     * for more details about this C++ feature).
     *
     * So what is the problem?  Consider a class, say X, containing a
     * gum::Bijection that stores all its elements in a convenient way. To
     * reduce memory consumption, X::end iterator is a static member that is
     * initialized with a gum::Bijection::end iterator. If the compiler decides
     * to initialize X::end before initializing gum::Bijection::end, then
     * X::end will be in an incoherent state.
     *
     * Unfortunately, we cannot know for sure in which order static members
     * will be initialized (the order is a compiler's decision). Hence, we
     * shall enfore the fact that gum::Bijection::end is initialized before
     * X::end. Using method gum::Bijection::end4Statics will ensure this fact:
     * it uses the C++ "construct on first use" idiom (see the C++ FAQ) that
     * ensures that the order fiasco is avoided. More precisely, end4Statics
     * uses a global variable that is the very end iterator used by all
     * gum::Bijection. Now, this induces a small overhead. So, we also provide
     * a gum::Bijection::end() method that returns the gum::Bijection::end
     * iterator without this small overhead, but assuming that function
     * end4Statics has already been called once (which is always the case) when
     * a gum::Bijection has been created.
     *
     * So, to summarize: when initializing static members use endSafe4Statics()
     * and in all the other cases, use endSafe().
     */
    static const iterator_safe& endSafe4Statics();

    /**
     * @brief Returns the unsafe end iterator for other classes' statics.
     *
     * To reduce the gum::Bijection memory consumption (which are heavily used
     * in aGrUM) while allowing fast for loops, end iterators are created just
     * once as a static member of a non-template gum::Bijection. While this
     * scheme is efficient and it works quite effectively, it has a drawback:
     * other classes with static members using the
     * BijectionImplementation::end() iterator may fail to work due to the well
     * known "static initialization order fiasco" (see Marshall Cline's C++ FAQ
     * for more details about this C++ feature).
     *
     * So what is the problem?  Consider a class, say X, containing a
     * gum::Bijection that stores all its elements in a convenient way. To
     * reduce memory consumption, X::end iterator is a static member that is
     * initialized with a gum::Bijection::end iterator. If the compiler decides
     * to initialize X::end before initializing gum::Bijection::end, then
     * X::end will be in an incoherent state.
     *
     * Unfortunately, we cannot know for sure in which order static members
     * will be initialized (the order is a compiler's decision). Hence, we
     * shall enfore the fact that gum::Bijection::end is initialized before
     * X::end. Using method gum::Bijection::end4Statics will ensure this fact:
     * it uses the C++ "construct on first use" idiom (see the C++ FAQ) that
     * ensures that the order fiasco is avoided. More precisely, end4Statics
     * uses a global variable that is the very end iterator used by all
     * gum::Bijection. Now, this induces a small overhead. So, we also provide
     * a gum::Bijection::end() method that returns the gum::Bijection::end
     * iterator without this small overhead, but assuming that function
     * end4Statics has already been called once (which is always the case) when
     * a gum::Bijection has been created.
     *
     * So, to summarize: when initializing static members use end4Statics()
     * and in all the other cases, use end().
     */
    static const iterator& end4Statics();

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Returns the first value of a pair given its second value.
     * @param second The second value of a pair in the gum::Bijection.
     * @return Returns the first value of a pair given its second value.
     * @throws NotFound Raised if the element cannot be found.
     */
    const T1& first( const T2& second ) const;

    /**
     * @brief Returns the first value of a pair given its second value or
     * default_val if second is unfound. 
     * @param second The second value of a pair in the gum::Bijection.
     * @param default_val The default value returned if second is not in the
     * gum::Bijection.
     * @return Returns the first value of a pair given its second value or
     * default_val if second is not in the bjection.
     */
    const T1& firstWithDefault( const T2& second, const T1& default_val ) const;

    /**
     * @brief Returns the second value of a pair given its first value.
     * @param first The first value of a pair in the gum::Bijection.
     * @return Returns the second value of a pair given its first value.
     * @throws NotFound Raised if the element cannot be found.
     */
    const T2& second( const T1& first ) const;

    /**
     * @brief Returns the second value of a pair given its first value or
     * default_val if first is unfound.
     * @param second The second value of a pair in the gum::Bijection.
     * @param default_val The default value returned if first is not in the
     * gum::Bijection.
     * @return Returns the second value of a pair given its first value or
     * default_val if first is not in the bjection.
     */
    const T2& secondWithDefault( const T1& second, const T2& default_val ) const;

    /**
     * @brief Returns true if first is the first element in a pair in the
     * gum::Bijection.
     * @param first The element tested for existence.
     * @return Returns true if first is in the first element in a pair in the
     * gum::Bijection.
     */
    bool existsFirst( const T1& first ) const;

    /**
     * @brief Returns true if second is the second element in a pair in the
     * gum::Bijection.
     * @param second The element tested for existence.
     * @return Returns true if second is in the second element in a pair in the
     * gum::Bijection.
     */
    bool existsSecond( const T2& second ) const;

    /** 
     * @brief Inserts a new association in the gum::Bijection.
     *
     * The values are added by copy.
     *
     * @param first The first element of the pair to insert.
     * @param second The second element of the pair to insert.
     * @throws DuplicateElement Raised if the association already exists.
     */
    void insert( const T1& first, const T2& second );

    /** 
     * @brief Inserts a new association in the gum::Bijection.
     *
     * The values are moved in the gum::Bijection.
     *
     * @param first The first element of the pair to insert.
     * @param second The second element of the pair to insert.
     * @throws DuplicateElement Raised if the association already exists.
     */
    void insert( T1&& first, T2&& second );

    /**
     * @brief Emplace a new element in the gum::Bijection.
     *
     * The emplace method allows to construct directly an element of type Key
     * by passing to its constructor all the arguments it needs.
     *
     * @param args the arguments passed to the constructor
     * @throws DuplicateElement exception is thrown if the association already
     * exists 
     */
    template <typename... Args>
    void emplace( Args&&... args );

    /**
     * @brief Removes all the associations from the gum::Bijection.
     */
    void clear();

    /**
     * @brief Returns true if the gum::Bijection doesn't contain any
     * association.
     * @return Returns true if the gum::Bijection doesn't contain any
     * association.
     */
    bool empty() const noexcept;

    /**
     * @brief Returns the number of associations stored within the
     * gum::Bijection.
     * @return Returns the number of associations stored within the
     * gum::Bijection.
     */
    Size size() const noexcept;

    /** 
     * @brief Erases an association containing the given first element.
     *
     * If the element cannot be found, nothing is done. In particular, no
     * exception is raised. 
     *
     * @param first The first element of a pair in the gum::Bijection.
     */
    void eraseFirst( const T1& first );

    /** 
     * @brief Erases an association containing the given second element.
     *
     * If the element cannot be found, nothing is done. In particular, no
     * exception is raised. 
     *
     * @param second The second element of a pair in the gum::Bijection.
     */
    void eraseSecond( const T2& second );

    /**
     * @brief Returns a friendly representatin of the gum::Bijection.
     * @return Returns a friendly representatin of the gum::Bijection.
     */
    std::string toString() const;

    /// @}
    // ============================================================================
    /// @name Fine tuning
    // ============================================================================
    /// @{

    /**
     * @brief Returns the number of hashtables slots used.
     * @return Returns the number of hashtables slots used.
     */
    Size capacity() const noexcept;

    /**
     * @brief Manually resize the gum::Bijection.
     *
     * See gum::HashTable::resize(gum::Size)
     *
     * @param new_size The gum::Bijection new size.
     */
    void resize( Size new_size );

    /**
     * @brief Change the gum::Bijection resizing policy.
     *
     * See gum::HashTable::setResizePolicy( const bool );
     *
     * @param new_policy If true, the gum::Bijection will resize automatically.
     */
    void setResizePolicy( const bool new_policy ) noexcept;

    /**
     * @brief Returns true if the resize policy is automatic.
     *
     * See gum::HashTable::resizePolicy().
     *
     * @return Returns true if the resize policy is automatic.
     */
    bool resizePolicy() const noexcept;

    /// @}

    private:
 
    /// Alias for more readable code
    /// @{
    using HashTable12 = HashTable<T1, T2*, allocator12_type>;
    using HashTable21 = HashTable<T2, T1*, allocator21_type>;
    /// @}

    /// a friend to speed-up accesses
    /// @{
    friend class BijectionIteratorSafe<T1, T2>;
    friend class BijectionIterator<T1, T2>;
    friend class Bijection<T1, T2, Alloc>;
    template <typename TT1, typename TT2, typename A, bool>
    friend class BijectionImplementation;
    /// @}

    // Below, we create the two gum::HashTable used by the gum::Bijection. Note
    // that the values of these gum::HashTable are pointers. This enables to
    // create only once objects (T1,T2). When using gum::Bijection with large
    // size objects, this feature is of particular interest.

    /// The gum::HashTable associating T2 objects to T1 objects
    HashTable12 __firstToSecond;

    /// The gum::HashTable associating T1 objects to T2 objects
    HashTable21 __secondToFirst;

    /**
     * @brief A function that performs a complete copy of another gum::Bijection.
     * @warning this function assumes that "this" is an empty gum::Bijection.
     * If it is not the case, use function clear() before calling __copy. 
     *
     * @param source The source from copied into this gum::Bijection.
     * @tparam OtherAlloc The allocator used by source.
     */
    template <typename OtherAlloc>
    void __copy( const HashTable<T1, T2*, OtherAlloc>& source );

    /**
     * @brief Inserts a new association into the gum::Bijection.
     * @param first The first object in the association.
     * @param second The second object in the association.
     * @return Returns a pointer toward the inserted association.
     */
    typename HashTable12::value_type* __insert( const T1& first,
                                                const T2& second );

    /**
     * @brief Inserts a new association into the gum::Bijection.
     * @param first The first object in the association.
     * @param second The second object in the association.
     * @return Returns a pointer toward the inserted association.
     */
    typename HashTable12::value_type* __insert( T1&& first, T2&& second );
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  // ===========================================================================
  // ===                  SCALAR BIJECTION IMPLEMENTATION                    ===
  // ===========================================================================

  /**
   * @class BijectionImplementation bijection.h <agrum/core/bijection.h>
   * @brief A non scalar implementation of a Bijection.
   * @ingroup bijection_group
   *
   * This class is designed for modeling a gum::Bijection between two sets, the
   * idea is following :
   *  - We want to create a gum::Bjection relation between type T1 and type T2,
   *  - For x in T1, there exists only one y in T2 associated to x,
   *  - For y in T2, there exists only one x in T1 associated to y,
   *  - The user inserts all the (x, y) associations and can search efficiently
   *  the values thus associated.
   *
   *  This class differs from the previous one by being optimized for
   *  non-scalar types.
   *
   * @tparam T1 The first type of elements in the gum::Bjection.
   * @tparam T2 The second type of elements in the gum::Bjection.
   * @tparam Alloc The allocator used for allocating memory.
   * @tparam Gen If true, this will be replaced by a implementation omptimized
   * for non-scalar types.
   */
  template <typename T1, typename T2, typename Alloc>
  class BijectionImplementation<T1, T2, Alloc, true> {

    public:

    /// types for STL compliance
    /// @{
    using type1_type = T1;
    using type1_reference = T1&;
    using type1_const_reference = const T1&;
    using type1_pointer = T1*;
    using type1_const_pointer = const T1*;
    using type2_type = T2;
    using type2_reference = T2&;
    using type2_const_reference = const T2&;
    using type2_pointer = T2*;
    using type2_const_pointer = const T2*;
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
 
    /**
     * @brief Default constructor: creates a gum::Bijection without any
     * association.
     * @param size The Bijection starting size.
     * @param resize_policy If true, the gum::Bijection will resize itself
     * automatically.
     */
    BijectionImplementation( Size size, bool resize_policy );

    /**
     * @brief Initializer list constructor.
     * @param list The initialize list.
     */
    BijectionImplementation( std::initializer_list<std::pair<T1, T2>> list );

    /**
     * @brief Copy constructor.
     * @param toCopy Bijection to copy.
     */
    BijectionImplementation(
        const BijectionImplementation<T1, T2, Alloc, true>& toCopy );

    /**
     * @brief Generalized copy constructor.
     * @param toCopy Bijection to copy.
     */
    template <typename OtherAlloc>
    BijectionImplementation(
        const BijectionImplementation<T1, T2, OtherAlloc, true>& toCopy );

    /**
     * @brief Move constructor.
     * @param from Bijection to move.
     */
    BijectionImplementation(
        BijectionImplementation<T1, T2, Alloc, true>&& from ) noexcept;

    public:
 
    // ============================================================================
    /// @name Constructors/destructors
    // ============================================================================
    /// @{

    /**
     * @brief Destructor.
     */
    ~BijectionImplementation();

    /// @}

    private:

    /**
     * @brief Copy operator.
     * @param toCopy Bijection to copy.
     * @return Returns the gum::Bijection in which the copy was made.
     */
    BijectionImplementation<T1, T2, Alloc, true>&
    operator=( const BijectionImplementation<T1, T2, Alloc, true>& toCopy );

    /**
     * @brief Generalized copy operator.
     * @param toCopy Bijection to copy.
     * @return Returns the gum::Bijection in which the copy was made.
     */
    template <typename OtherAlloc>
    BijectionImplementation<T1, T2, Alloc, true>& operator=(
        const BijectionImplementation<T1, T2, OtherAlloc, true>& toCopy );

    /**
     * @brief Move operator.
     * @param toCopy Bijection to move
     * @return Returns the moved gum::Bijection in which the move was made.
     */
    BijectionImplementation<T1, T2, Alloc, true>&
    operator=( BijectionImplementation<T1, T2, Alloc, true>&& from );

    public:

    // ============================================================================
    /// @name Iterators
    // ============================================================================
    /// @{

    /** 
     * @brief Returns the unsafe iterator at the beginning of the
     * gum::Bijection.
     *
     * Unsafe iterators are a little bit faster than safe ones. But this speed
     * is at the expense of safety: if you point to an element that is deleted,
     * then try to access it or trying to operate a ++ will most certainly
     * result in a segfault. So, Unsafe iterators should only be used to parse
     * gum::Bjection where no element is ever deleted. If unsure, prefer using
     * safe iterators.
     *
     * Note that the notion of a beginning/end of a gum::Bjection is rather fuzzy.
     * What is important here is that for an instance bij of this class:
     *
     * @code
     * for(iterator iter = bij.begin(); iter != bij.end(); ++iter) {
     *   // will parse all the associations.
     * }
     * @endcode
     */
    iterator begin() const;
    
    /**
     * @brief Returns the constant unsafe iterator at the beginning of the
     * gum::Bjection.
     *
     * Unsafe iterators are a little bit faster than safe ones. But this speed
     * is at the expense of safety: if you point to an element that is deleted,
     * then try to access it or trying to operate a ++ will most certainly
     * result in a segfault. So, Unsafe iterators should only be used to parse
     * gum::Bjection where no element is ever deleted. If unsure, prefer using
     * safe iterators.
     *
     * Note that the notion of a beginning/end of a gum::Bjection is rather
     * fuzzy.  What is important here is that for an instance bij of this
     * class:
     *
     * @code
     * for (iterator iter = bij.cbegin(); iter != bij.cend(); ++iter) {
     *   // will parse all the association
     * }
     * @endcode
     */
    const_iterator cbegin() const;

    /** 
     * @brief Returns the unsafe iterator at the end of the gum::Bijection.
     *
     * Unsafe iterators are a little bit faster than safe ones. But this speed
     * is at the expense of safety: if you point to an element that is deleted,
     * then try to access it or trying to operate a ++ will most certainly
     * result in a segfault. So, Unsafe iterators should only be used to parse
     * gum::Bijection where no element is ever deleted. If unsure, prefer using
     * safe iterators.
     *
     * Note that the notion of a beginning/end of a gum::Bijection is rather
     * fuzzy.  What is important here is that for an instance bij of this
     * class:
     *
     * @code
     * for(iterator iter = bij.begin(); iter != bij.end(); ++iter) {
     *   // loops will parse all the associations 
     * }
     * @endcode
     */
    const iterator& end() const noexcept;

    /** 
     * @brief Returns the constant iterator at the end of the gum::Bijection.
     *
     * Unsafe iterators are a little bit faster than safe ones. But this speed
     * is at the expense of safety: if you point to an element that is deleted,
     * then try to access it or trying to operate a ++ will most certainly
     * result in a segfault. So, Unsafe iterators should only be used to parse
     * gum::Bijection where no element is ever deleted. If unsure, prefer using
     * safe iterators.
     *
     * Note that the notion of a beginning/end of a gum::Bijection is rather fuzzy.
     * What is important here is that for an instance bij of this class:
     *
     * @code
     * for (iterator iter = bij.cbegin(); iter != bij.cend(); ++iter) {
     *   // loops will parse all the associations
     * }
     * @endcode
     */
    const const_iterator& cend() const noexcept;

    /**
     * @brief Returns the safe iterator at the beginning of the gum::Bijection.
     *
     * Safe iterators are slightly slower than unsafe iterators. However, they
     * guarantee that no segmentation fault can ever occur when trying to
     * access the element they point to or when applying a ++ operator. When no
     * element of the gum::Bijection is to be deleted during the parsing of the
     * gum::Bijection (as for instance when you parse the gum::Bijection to
     * display its content), prefer using the unsafe iterators, which are a
     * little bit faster and cannot, in this case, produce segfaults.
     *
     * Note that the notion of a beginning/end of a gum::Bijection is rather
     * fuzzy.  What is important here is that for an instance bij of this
     * class:
     *
     * @code
     * for (iterator iter = bij.beginSafe(); iter != bij.endSafe(); ++iter) {
     *   // loops will parse all the associations
     * }
     * @endcode
     */
    iterator_safe beginSafe() const;

    /**
     * @brief Returns the constant safe iterator at the begining of the
     * gum::Bijection.
     *
     * Safe iterators are slightly slower than unsafe iterators. However, they
     * guarantee that no segmentation fault can ever occur when trying to
     * access the element they point to or when applying a ++ operator. When no
     * element of the gum::Bijection is to be deleted during the parsing of the
     * gum::Bijection (as for instance when you parse the bijection to display
     * its content), prefer using the unsafe iterators, which are a little bit
     * faster and cannot, in this case, produce segfaults.
     *
     * Note that the notion of a beginning/end of a gum::Bijection is rather
     * fuzzy.  What is important here is that for an instance bij of this
     * class:
     *
     * @code
     * for (iterator iter = bij.cbeginSafe(); iter != bij.cendSafe(); ++iter) {
     *   // loops will parse all the associations 
     * }
     * @encode
     */
    const_iterator_safe cbeginSafe() const;

    /** 
     * @brief Returns the safe iterator at the end of the gum::Bijection.
     *
     * Safe iterators are slightly slower than unsafe iterators. However, they
     * guarantee that no segmentation fault can ever occur when trying to
     * access the element they point to or when applying a ++ operator. When no
     * element of the gum::Bijection is to be deleted during the parsing of the
     * gum::Bijection (as for instance when you parse the gum::Bijection to
     * display its content), prefer using the unsafe iterators, which are a
     * little bit faster and cannot, in this case, produce segfaults.
     *
     * Note that the notion of a beginning/end of a gum::Bijection is rather fuzzy.
     * What is important here is that for an instance bij of this class:
     *
     * @code
     * for (iterator iter = bij.beginSafe(); iter != bij.endSafe(); ++iter) {
     *   // loops will parse all the associations 
     * }
     * @endcode
     */
    const iterator_safe& endSafe() const noexcept;

    /**
     * @brief Returns the constant safe iterator at the end of the
     * gum::Bijection.
     *
     * Safe iterators are slightly slower than unsafe iterators. However, they
     * guarantee that no segmentation fault can ever occur when trying to
     * access the element they point to or when applying a ++ operator. When no
     * element of the gum::Bijection is to be deleted during the parsing of the
     * gum::Bijection (as for instance when you parse the gum::Bijection to
     * display its content), prefer using the unsafe iterators, which are a
     * little bit faster and cannot, in this case, produce segfaults.
     *
     * Note that the notion of a beginning/end of a gum::Bijection is rather
     * fuzzy.  What is important here is that for an instance bij of this
     * class:
     *
     * @code
     * for (iterator iter = bij.cbeginSafe(); iter != bij.cendSafe(); ++iter) {
     *   // loops will parse all the associations 
     * }
     * @endcode
     */
    const const_iterator_safe& cendSafe() const noexcept;

    /**
     * @brief Returns the safe end iterator for other classes' statics.
     *
     * To reduce the gum::Bijection memory consumption (which are heavily used
     * in aGrUM) while allowing fast for loops, end iterators are created just
     * once as a static member of a non-template gum::Bijection. While this
     * scheme is efficient and it works quite effectively, it has a drawback:
     * other classes with static members using the
     * BijectionImplementation::end() iterator may fail to work due to the well
     * known "static initialization order fiasco" (see Marshall Cline's C++ FAQ
     * for more details about this C++ feature).
     *
     * So what is the problem?  Consider a class, say X, containing a
     * gum::Bijection that stores all its elements in a convenient way. To
     * reduce memory consumption, X::end iterator is a static member that is
     * initialized with a gum::Bijection::end iterator. If the compiler decides
     * to initialize X::end before initializing gum::Bijection::end, then
     * X::end will be in an incoherent state.
     *
     * Unfortunately, we cannot know for sure in which order static members
     * will be initialized (the order is a compiler's decision). Hence, we
     * shall enfore the fact that gum::Bijection::end is initialized before
     * X::end. Using method gum::Bijection::end4Statics will ensure this fact:
     * it uses the C++ "construct on first use" idiom (see the C++ FAQ) that
     * ensures that the order fiasco is avoided. More precisely, end4Statics
     * uses a global variable that is the very end iterator used by all
     * gum::Bijection. Now, this induces a small overhead. So, we also provide
     * a gum::Bijection::end() method that returns the gum::Bijection::end
     * iterator without this small overhead, but assuming that function
     * end4Statics has already been called once (which is always the case) when
     * a gum::Bijection has been created.
     *
     * So, to summarize: when initializing static members use endSafe4Statics()
     * and in all the other cases, use endSafe().
     */
    static const iterator_safe& endSafe4Statics();

    /**
     * @brief Returns the unsafe end iterator for other classes' statics.
     *
     * To reduce the gum::Bijection memory consumption (which are heavily used
     * in aGrUM) while allowing fast for loops, end iterators are created just
     * once as a static member of a non-template gum::Bijection. While this
     * scheme is efficient and it works quite effectively, it has a drawback:
     * other classes with static members using the
     * BijectionImplementation::end() iterator may fail to work due to the well
     * known "static initialization order fiasco" (see Marshall Cline's C++ FAQ
     * for more details about this C++ feature).
     *
     * So what is the problem?  Consider a class, say X, containing a
     * gum::Bijection that stores all its elements in a convenient way. To
     * reduce memory consumption, X::end iterator is a static member that is
     * initialized with a gum::Bijection::end iterator. If the compiler decides
     * to initialize X::end before initializing gum::Bijection::end, then
     * X::end will be in an incoherent state.
     *
     * Unfortunately, we cannot know for sure in which order static members
     * will be initialized (the order is a compiler's decision). Hence, we
     * shall enfore the fact that gum::Bijection::end is initialized before
     * X::end. Using method gum::Bijection::end4Statics will ensure this fact:
     * it uses the C++ "construct on first use" idiom (see the C++ FAQ) that
     * ensures that the order fiasco is avoided. More precisely, end4Statics
     * uses a global variable that is the very end iterator used by all
     * gum::Bijection. Now, this induces a small overhead. So, we also provide
     * a gum::Bijection::end() method that returns the gum::Bijection::end
     * iterator without this small overhead, but assuming that function
     * end4Statics has already been called once (which is always the case) when
     * a gum::Bijection has been created.
     *
     * So, to summarize: when initializing static members use end4Statics()
     * and in all the other cases, use end().
     */
    static const iterator& end4Statics();

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Returns the first value of a pair given its second value.
     * @param second The second value of a pair in the gum::Bijection.
     * @return Returns the first value of a pair given its second value.
     * @throws NotFound Raised if the element cannot be found.
     */
    const T1& first( T2 second ) const;

    /**
     * @brief Returns the first value of a pair given its second value or
     * default_val if second is unfound. 
     * @param second The second value of a pair in the gum::Bijection.
     * @param default_val The default value returned if second is not in the
     * gum::Bijection.
     * @return Returns the first value of a pair given its second value or
     * default_val if second is not in the bjection.
     */
    const T1& firstWithDefault( T2 second, T1 default_val ) const;

    /**
     * @brief Returns the second value of a pair given its first value.
     * @param first The first value of a pair in the gum::Bijection.
     * @return Returns the second value of a pair given its first value.
     * @throws NotFound Raised if the element cannot be found.
     */
    const T2& second( T1 first ) const;

    /**
     * @brief Returns the second value of a pair given its first value or
     * default_val if first is unfound.
     * @param second The second value of a pair in the gum::Bijection.
     * @param default_val The default value returned if first is not in the
     * gum::Bijection.
     * @return Returns the second value of a pair given its first value or
     * default_val if first is not in the bjection.
     */
    const T2& secondWithDefault( T1 first, T2 default_val ) const;

    /**
     * @brief Returns true if first is in the first element in a pair in the
     * gum::Bijection.
     * @param first The element tested for existence.
     * @return Returns true if first is in the first element in a pair in the
     * gum::Bijection.
     */
    bool existsFirst( T1 first ) const;

    /**
     * @brief Returns true if second is in the second element in a pair in the
     * gum::Bijection.
     * @param first The element tested for existence.
     * @return Returns true if second is in the second element in a pair in the
     * gum::Bijection.
     */
    bool existsSecond( T2 second ) const;

    /** 
     * @brief Inserts a new association in the gum::Bijection.
     *
     * The values are added by copy.
     *
     * @param first The first element of the pair to insert.
     * @param second The second element of the pair to insert.
     * @throws DuplicateElement Raised if the association already exists.
     */
    void insert( T1 first, T2 second );

    /**
     * @brief Emplace a new element in the gum::Bijection.
     *
     * The emplace method allows to construct directly an element of type Key
     * by passing to its constructor all the arguments it needs.
     *
     * @param args the arguments passed to the constructor
     * @throws DuplicateElement exception is thrown if the association already
     * exists 
     */
    template <typename... Args>
    void emplace( Args&&... args );

    /**
     * @brief Removes all the associations from the gum::Bijection.
     */
    void clear();

    /**
     * @brief Returns true if the gum::Bijection doesn't contain any
     * association.
     * @return Returns true if the gum::Bijection doesn't contain any
     * association.
     */
    bool empty() const noexcept;

    /**
     * @brief Returns the number of associations stored within the
     * gum::Bijection.
     * @return Returns the number of associations stored within the
     * gum::Bijection.
     */
    Size size() const noexcept;

    /** 
     * @brief Erases an association containing the given first element.
     *
     * If the element cannot be found, nothing is done. In particular, no
     * exception is raised. 
     *
     * @param first The first element of a pair in the gum::Bijection.
     */
    void eraseFirst( T1 first );

    /** 
     * @brief Erases an association containing the given second element.
     *
     * If the element cannot be found, nothing is done. In particular, no
     * exception is raised. 
     *
     * @param first The second element of a pair in the gum::Bijection.
     */
    void eraseSecond( T2 second );

    /**
     * @brief Returns a friendly representatin of the gum::Bijection.
     * @return Returns a friendly representatin of the gum::Bijection.
     */
    std::string toString() const;

    /// @}
    // ============================================================================
    /// @name Fine tuning
    // ============================================================================
    /// @{

    /**
     * @brief Returns the number of hashtables slots used.
     * @return Returns the number of hashtables slots used.
     */
    Size capacity() const noexcept;

    /**
     * @brief Manually resize the gum::Bijection.
     *
     * See gum::HashTable::resize(gum::Size)
     *
     * @param new_size The gum::Bijection new size.
     */
    void resize( Size new_size );

    /**
     * @brief Change the gum::Bijection resizing policy.
     *
     * See gum::HashTable::setResizePolicy( const bool );
     *
     * @param If true, the gum::Bijection will resize automatically.
     */
    void setResizePolicy( const bool new_policy ) noexcept;

    /**
     * @brief Returns true if the resize policy is automatic.
     *
     * See gum::HashTable::resizePolicy().
     *
     * @return Returns true if the resize policy is automatic.
     */
    bool resizePolicy() const noexcept;

    /// @}

    private:

    /// Alias for more readable code
    /// @{
    using HashTable12 = HashTable<T1, T2, allocator12_type>;
    using HashTable21 = HashTable<T2, T1, allocator21_type>;
    /// @}

    /// a friend to speed-up accesses
    /// @{
    friend class BijectionIteratorSafe<T1, T2>;
    friend class BijectionIterator<T1, T2>;
    friend class Bijection<T1, T2, Alloc>;
    template <typename TT1, typename TT2, typename A, bool>
    friend class BijectionImplementation;
    /// @}

    // Below, we create the two gum::HashTable used by the gum::Bijection. Note
    // that the values of these gum::HashTable are pointers. This enables to
    // create only once objects (T1,T2). When using gum::Bijection with large
    // size objects, this feature is of particular interest.

    /// hashtable associating T2 scalars to T1 scalars
    HashTable12 __firstToSecond;

    /// hashtable associating T1 scalars to T2 scalars
    HashTable21 __secondToFirst;

    /**
     * @brief A function that performs a complete copy of another gum::Bijection.
     * @warning this function assumes that "this" is an empty gum::Bijection.
     * If it is not the case, use function clear() before calling __copy. 
     *
     * @param source The source from copied into this gum::Bijection.
     * @tparam OtherAlloc The allocator used by source.
     */
    template <typename OtherAlloc>
    void __copy( const HashTable<T1, T2, OtherAlloc>& f2s );

    /**
     * @brief Inserts a new association into the gum::Bijection.
     * @param first The first object in the association.
     * @param second The second object in the association.
     * @return Returns a pointer toward the inserted association.
     */
    void __insert( const T1 first, const T2 second );
  };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  /**
   * @class Bijection bijection.h <agrum/core/bijection.h>
   * @brief Set of pairs of elements with fast search for both elements.
   * @ingroup basicstruct_group
   * @ingroup bijection_group
   *
   * This class is designed for modeling a gum::Bijection between two sets, the
   * idea is following :
   *  - We want to create a gum::Bjection relation between type T1 and type T2,
   *  - For x in T1, there exists only one y in T2 associated to x,
   *  - For y in T2, there exists only one x in T1 associated to y,
   *  - The user inserts all the (x, y) associations and can search efficiently
   *  the values thus associated.
   *
   * @tparam T1 The first type of elements in the gum::Bjection.
   * @tparam T2 The second type of elements in the gum::Bjection.
   * @tparam Alloc The allocator used for allocating memory.
   */
  template <typename T1, typename T2, typename Alloc = std::allocator<T2>>
  class Bijection
      : public BijectionImplementation<T1,
                                       T2,
                                       Alloc,
                                       std::is_scalar<T1>::value &&
                                           std::is_scalar<T2>::value> {
    public:
    /// types for STL compliance
    /// @{
    using type1_type = T1;
    using type1_reference = T1&;
    using type1_const_reference = const T1&;
    using type1_pointer = T1*;
    using type1_const_pointer = const T1*;
    using type2_type = T2;
    using type2_reference = T2&;
    using type2_const_reference = const T2&;
    using type2_pointer = T2*;
    using type2_const_pointer = const T2*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = Alloc;
    using iterator = BijectionIterator<T1, T2>;
    using const_iterator = BijectionIterator<T1, T2>;
    using iterator_safe = BijectionIteratorSafe<T1, T2>;
    using const_iterator_safe = BijectionIteratorSafe<T1, T2>;

    using allocator1_type = typename Alloc::template rebind<T1*>::other;
    using allocator2_type = typename Alloc::template rebind<T2*>::other;
    /// @}

    /// The Implementation of this gum::Bijection.
    using Implementation =
        BijectionImplementation<T1,
                                T2,
                                Alloc,
                                std::is_scalar<T1>::value &&
                                    std::is_scalar<T2>::value>;

    // ============================================================================
    /// @name Constructors/destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor: creates a gum::Bijection without any
     * association.
     * @param size The gum::Bijection starting size.
     * @param resize_policy If tru, the gum::Bijection will be automatically
     * resized.
     */
    Bijection( Size size = HashTableConst::default_size,
               bool resize_policy = HashTableConst::default_resize_policy );

    /**
     * @brief Initializer list constructor.
     * @param list The initialisation list.
     */
    Bijection( std::initializer_list<std::pair<T1, T2>> list );

    /**
     * @brief Copy constructor.
     * @param toCopy The gum::Bijection to copy.
     */
    Bijection( const Bijection<T1, T2, Alloc>& toCopy );

    /**
     * @brief Generalized copy constructor.
     * @param toCopy The gum::Bijection to copy.
     * @tparam The gum::Bijection to copy allocator's type.
     */
    template <typename OtherAlloc>
    Bijection( const Bijection<T1, T2, OtherAlloc>& toCopy );

    /**
     * @brief Move constructor.
     * @param from The gum::Bijection to move from.
     */
    Bijection( Bijection<T1, T2, Alloc>&& from ) noexcept;

    /**
     * @brief Class destructor.
     */
    ~Bijection();

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param toCopy The gum::Bijection to copy.
     * @return Returns this gum::Bijection.
     */
    Bijection<T1, T2, Alloc>&
    operator=( const Bijection<T1, T2, Alloc>& toCopy );

    /**
     * @brief Generalized copy operator.
     * @param toCopy The gum::Bijection to copy.
     * @tparam OtherAlloc The gum::Bijection to copy allocator's type.
     */
    template <typename OtherAlloc>
    Bijection<T1, T2, Alloc>&
    operator=( const Bijection<T1, T2, OtherAlloc>& toCopy );

    /**
     * @brief Move operator.
     * @param bij The gum::Bijection to move from.
     */
    Bijection<T1, T2, Alloc>& operator=( Bijection<T1, T2, Alloc>&& bij );

    /// @}
  };

  /**
   * @brief For friendly display of the content of the gum::Bijection.
   *
   * @param bijection The gum::Bijection to display.
   * @tparam T1 The first type of elements in the gum::Bjection.
   * @tparam T2 The second type of elements in the gum::Bjection.
   * @tparam Alloc The allocator used for allocating memory.
   * @return The stream in which the gum::Bijection is displayed.
   */
  template <typename T1, typename T2, typename Alloc>
  std::ostream& operator<<( std::ostream&, const Bijection<T1, T2, Alloc>& bijection);

} /* namespace gum */

// always include the template implementations
#include <agrum/core/bijection.tcc>

#endif /* GUM_BIJECTION_H */
