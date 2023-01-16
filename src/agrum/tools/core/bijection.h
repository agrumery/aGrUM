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
 * @brief Set of pairs of elements with fast search for both elements.
 *
 * Bijections are some kind of sets of pairs (T1,T2) with the additional
 * feature as compared to Sets: we can search very quickly T2's elements when
 * knowing T1 and T1's elements when knowing T2.
 *
 * @author Christophe GONZALES(_at_AMU) and Jean-Philippe DUBUS
 */
#ifndef GUM_BIJECTION_H
#define GUM_BIJECTION_H

#include <initializer_list>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

#include <agrum/tools/core/hashTable.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  template < typename T1, typename T2 >
  class BijectionIteratorSafe;
  template < typename T1, typename T2 >
  class BijectionIterator;
  template < typename T1, typename T2, bool >
  class BijectionImplementation;
  template < typename T1, typename T2 >
  class Bijection;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


  // ===========================================================================
  // ===                NON SCALAR BIJECTION IMPLEMENTATION                  ===
  // ===========================================================================

  /**
   * @class BijectionImplementation
   * @headerfile bijection.h <agrum/tools/core/bijection.h>
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
   * @tparam Gen If true, this will be replaced by a implementation omptimized
   * for non-scalar types.
   */
  template < typename T1, typename T2, bool Gen >
  class BijectionImplementation {
    public:
    /// types for STL compliance
    /// @{
    using type1_type            = T1;
    using type1_reference       = T1&;
    using type1_const_reference = const T1&;
    using type1_pointer         = T1*;
    using type1_const_pointer   = const T1*;
    using type2_type            = T2;
    using type2_reference       = T2&;
    using type2_const_reference = const T2&;
    using type2_pointer         = T2*;
    using type2_const_pointer   = const T2*;
    using size_type             = std::size_t;
    using difference_type       = std::ptrdiff_t;
    using iterator              = BijectionIterator< T1, T2 >;
    using const_iterator        = BijectionIterator< T1, T2 >;
    using iterator_safe         = BijectionIteratorSafe< T1, T2 >;
    using const_iterator_safe   = BijectionIteratorSafe< T1, T2 >;
    /// @}

    private:
    /**
     * @brief Default constructor: creates a gum::Bijection without any
     * association.
     * @param size The Bijection starting size.
     * @param resize_policy If true, the gum::Bijection will resize itself
     * automatically.
     */
    BijectionImplementation(Size size, bool resize_policy);

    /**
     * @brief Initializer list constructor.
     * @param list The initialize list.
     */
    BijectionImplementation(std::initializer_list< std::pair< T1, T2 > > list);

    /**
     * @brief Copy constructor.
     * @param toCopy Bijection to copy.
     */
    BijectionImplementation(const BijectionImplementation< T1, T2, Gen >& toCopy);

    /**
     * @brief Move constructor.
     * @param from Bijection to move.
     */
    BijectionImplementation(BijectionImplementation< T1, T2, Gen >&& from) noexcept;

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
    BijectionImplementation< T1, T2, Gen >&
       operator=(const BijectionImplementation< T1, T2, Gen >& toCopy);

    /**
     * @brief Move operator.
     * @param toCopy Bijection to move
     * @return Returns the moved gum::Bijection in which the move was made.
     */
    BijectionImplementation< T1, T2, Gen >&
       operator=(BijectionImplementation< T1, T2, Gen >&& toCopy);

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
     * Note that the notion of a beginning/end of a gum::Bjection is rather
     * fuzzy.
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
     * Note that the notion of a beginning/end of a gum::Bijection is rather
     * fuzzy.
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
     * @endcode
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
     * Note that the notion of a beginning/end of a gum::Bijection is rather
     * fuzzy.
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
    const T1& first(const T2& second) const;

    /**
     * @brief Returns the first value of a pair given its second value or
     * default_val if second is unfound.
     * @param second The second value of a pair in the gum::Bijection.
     * @param default_val The default value returned if second is not in the
     * gum::Bijection.
     * @return Returns the first value of a pair given its second value or
     * default_val if second is not in the bjection.
     */
    const T1& firstWithDefault(const T2& second, const T1& default_val) const;

    /**
     * @brief Returns the second value of a pair given its first value.
     * @param first The first value of a pair in the gum::Bijection.
     * @return Returns the second value of a pair given its first value.
     * @throws NotFound Raised if the element cannot be found.
     */
    const T2& second(const T1& first) const;

    /**
     * @brief Returns the second value of a pair given its first value or
     * default_val if first is unfound.
     * @param second The second value of a pair in the gum::Bijection.
     * @param default_val The default value returned if first is not in the
     * gum::Bijection.
     * @return Returns the second value of a pair given its first value or
     * default_val if first is not in the bjection.
     */
    const T2& secondWithDefault(const T1& second, const T2& default_val) const;

    /**
     * @brief Returns true if first is the first element in a pair in the
     * gum::Bijection.
     * @param first The element tested for existence.
     * @return Returns true if first is in the first element in a pair in the
     * gum::Bijection.
     */
    bool existsFirst(const T1& first) const;

    /**
     * @brief Returns true if second is the second element in a pair in the
     * gum::Bijection.
     * @param second The element tested for existence.
     * @return Returns true if second is in the second element in a pair in the
     * gum::Bijection.
     */
    bool existsSecond(const T2& second) const;

    /**
     * @brief Inserts a new association in the gum::Bijection.
     *
     * The values are added by copy.
     *
     * @param first The first element of the pair to insert.
     * @param second The second element of the pair to insert.
     * @throws DuplicateElement Raised if the association already exists.
     */
    void insert(const T1& first, const T2& second);

    /**
     * @brief Inserts a new association in the gum::Bijection.
     *
     * The values are moved in the gum::Bijection.
     *
     * @param first The first element of the pair to insert.
     * @param second The second element of the pair to insert.
     * @throws DuplicateElement Raised if the association already exists.
     */
    void insert(T1&& first, T2&& second);

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
    template < typename... Args >
    void emplace(Args&&... args);

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
    void eraseFirst(const T1& first);

    /**
     * @brief Erases an association containing the given second element.
     *
     * If the element cannot be found, nothing is done. In particular, no
     * exception is raised.
     *
     * @param second The second element of a pair in the gum::Bijection.
     */
    void eraseSecond(const T2& second);

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
    void resize(Size new_size);

    /**
     * @brief Change the gum::Bijection resizing policy.
     *
     * See gum::HashTable::setResizePolicy( const bool );
     *
     * @param new_policy If true, the gum::Bijection will resize automatically.
     */
    void setResizePolicy(const bool new_policy) noexcept;

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
    using HashTable12 = HashTable< T1, T2* >;
    using HashTable21 = HashTable< T2, T1* >;
    /// @}

    /// a friend to speed-up accesses
    /// @{
    friend class BijectionIteratorSafe< T1, T2 >;
    friend class BijectionIterator< T1, T2 >;
    friend class Bijection< T1, T2 >;
    template < typename TT1, typename TT2, bool >
    friend class BijectionImplementation;
    /// @}

    // Below, we create the two gum::HashTable used by the gum::Bijection. Note
    // that the values of these gum::HashTable are pointers. This enables to
    // create only once objects (T1,T2). When using gum::Bijection with large
    // size objects, this feature is of particular interest.

    /// The gum::HashTable associating T2 objects to T1 objects
    HashTable12 _firstToSecond_;

    /// The gum::HashTable associating T1 objects to T2 objects
    HashTable21 _secondToFirst_;

    /**
     * @brief A function that performs a complete copy of another
     * gum::Bijection.
     * @warning this function assumes that "this" is an empty gum::Bijection.
     * If it is not the case, use function clear() before calling  _copy_.
     *
     * @param source The source from copied into this gum::Bijection.
     */
    void _copy_(const HashTable< T1, T2* >& source);

    /**
     * @brief Inserts a new association into the gum::Bijection.
     * @param first The first object in the association.
     * @param second The second object in the association.
     * @return Returns a pointer toward the inserted association.
     */
    typename HashTable12::value_type* _insert_(const T1& first, const T2& second);

    /**
     * @brief Inserts a new association into the gum::Bijection.
     * @param first The first object in the association.
     * @param second The second object in the association.
     * @return Returns a pointer toward the inserted association.
     */
    typename HashTable12::value_type* _insert_(T1&& first, T2&& second);
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  // ===========================================================================
  // ===                  SCALAR BIJECTION IMPLEMENTATION                    ===
  // ===========================================================================

  /**
   * @class BijectionImplementation
   * @headerfile bijection.h <agrum/tools/core/bijection.h>
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
   * @tparam Gen If true, this will be replaced by a implementation omptimized
   * for non-scalar types.
   */
  template < typename T1, typename T2 >
  class BijectionImplementation< T1, T2, true > {
    public:
    /// types for STL compliance
    /// @{
    using type1_type            = T1;
    using type1_reference       = T1&;
    using type1_const_reference = const T1&;
    using type1_pointer         = T1*;
    using type1_const_pointer   = const T1*;
    using type2_type            = T2;
    using type2_reference       = T2&;
    using type2_const_reference = const T2&;
    using type2_pointer         = T2*;
    using type2_const_pointer   = const T2*;
    using size_type             = std::size_t;
    using difference_type       = std::ptrdiff_t;
    using iterator              = BijectionIterator< T1, T2 >;
    using const_iterator        = BijectionIterator< T1, T2 >;
    using iterator_safe         = BijectionIteratorSafe< T1, T2 >;
    using const_iterator_safe   = BijectionIteratorSafe< T1, T2 >;
    /// @}

    private:
    /**
     * @brief Default constructor: creates a gum::Bijection without any
     * association.
     * @param size The Bijection starting size.
     * @param resize_policy If true, the gum::Bijection will resize itself
     * automatically.
     */
    BijectionImplementation(Size size, bool resize_policy);

    /**
     * @brief Initializer list constructor.
     * @param list The initialize list.
     */
    BijectionImplementation(std::initializer_list< std::pair< T1, T2 > > list);

    /**
     * @brief Copy constructor.
     * @param toCopy Bijection to copy.
     */
    BijectionImplementation(const BijectionImplementation< T1, T2, true >& toCopy);

    /**
     * @brief Move constructor.
     * @param from Bijection to move.
     */
    BijectionImplementation(BijectionImplementation< T1, T2, true >&& from) noexcept;

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
    BijectionImplementation< T1, T2, true >&
       operator=(const BijectionImplementation< T1, T2, true >& toCopy);

    /**
     * @brief Move operator.
     * @param toCopy Bijection to move
     * @return Returns the moved gum::Bijection in which the move was made.
     */
    BijectionImplementation< T1, T2, true >&
       operator=(BijectionImplementation< T1, T2, true >&& from);

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
     * Note that the notion of a beginning/end of a gum::Bjection is rather
     * fuzzy.
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
     * Note that the notion of a beginning/end of a gum::Bijection is rather
     * fuzzy.
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
     * @endcode
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
     * Note that the notion of a beginning/end of a gum::Bijection is rather
     * fuzzy.
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
    const T1& first(T2 second) const;

    /**
     * @brief Returns the first value of a pair given its second value or
     * default_val if second is unfound.
     * @param second The second value of a pair in the gum::Bijection.
     * @param default_val The default value returned if second is not in the
     * gum::Bijection.
     * @return Returns the first value of a pair given its second value or
     * default_val if second is not in the bjection.
     */
    const T1& firstWithDefault(T2 second, T1 default_val) const;

    /**
     * @brief Returns the second value of a pair given its first value.
     * @param first The first value of a pair in the gum::Bijection.
     * @return Returns the second value of a pair given its first value.
     * @throws NotFound Raised if the element cannot be found.
     */
    const T2& second(T1 first) const;

    /**
     * @brief Returns the second value of a pair given its first value or
     * default_val if first is unfound.
     * @param second The second value of a pair in the gum::Bijection.
     * @param default_val The default value returned if first is not in the
     * gum::Bijection.
     * @return Returns the second value of a pair given its first value or
     * default_val if first is not in the bjection.
     */
    const T2& secondWithDefault(T1 first, T2 default_val) const;

    /**
     * @brief Returns true if first is in the first element in a pair in the
     * gum::Bijection.
     * @param first The element tested for existence.
     * @return Returns true if first is in the first element in a pair in the
     * gum::Bijection.
     */
    bool existsFirst(T1 first) const;

    /**
     * @brief Returns true if second is in the second element in a pair in the
     * gum::Bijection.
     * @param first The element tested for existence.
     * @return Returns true if second is in the second element in a pair in the
     * gum::Bijection.
     */
    bool existsSecond(T2 second) const;

    /**
     * @brief Inserts a new association in the gum::Bijection.
     *
     * The values are added by copy.
     *
     * @param first The first element of the pair to insert.
     * @param second The second element of the pair to insert.
     * @throws DuplicateElement Raised if the association already exists.
     */
    void insert(T1 first, T2 second);

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
    template < typename... Args >
    void emplace(Args&&... args);

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
    void eraseFirst(T1 first);

    /**
     * @brief Erases an association containing the given second element.
     *
     * If the element cannot be found, nothing is done. In particular, no
     * exception is raised.
     *
     * @param first The second element of a pair in the gum::Bijection.
     */
    void eraseSecond(T2 second);

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
    void resize(Size new_size);

    /**
     * @brief Change the gum::Bijection resizing policy.
     *
     * See gum::HashTable::setResizePolicy( const bool );
     *
     * @param If true, the gum::Bijection will resize automatically.
     */
    void setResizePolicy(const bool new_policy) noexcept;

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
    using HashTable12 = HashTable< T1, T2 >;
    using HashTable21 = HashTable< T2, T1 >;
    /// @}

    /// a friend to speed-up accesses
    /// @{
    friend class BijectionIteratorSafe< T1, T2 >;
    friend class BijectionIterator< T1, T2 >;
    friend class Bijection< T1, T2 >;
    template < typename TT1, typename TT2, bool >
    friend class BijectionImplementation;
    /// @}

    // Below, we create the two gum::HashTable used by the gum::Bijection. Note
    // that the values of these gum::HashTable are pointers. This enables to
    // create only once objects (T1,T2). When using gum::Bijection with large
    // size objects, this feature is of particular interest.

    /// hashtable associating T2 scalars to T1 scalars
    HashTable12 _firstToSecond_;

    /// hashtable associating T1 scalars to T2 scalars
    HashTable21 _secondToFirst_;

    /**
     * @brief A function that performs a complete copy of another
     * gum::Bijection.
     * @warning this function assumes that "this" is an empty gum::Bijection.
     * If it is not the case, use function clear() before calling  _copy_.
     *
     * @param source The source from copied into this gum::Bijection.
     */
    void _copy_(const HashTable< T1, T2 >& f2s);

    /**
     * @brief Inserts a new association into the gum::Bijection.
     * @param first The first object in the association.
     * @param second The second object in the association.
     * @return Returns a pointer toward the inserted association.
     */
    void _insert_(const T1 first, const T2 second);
  };

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  /**
   * @class BijectionIteratorGet
   * @headerfile bijection.h <agrum/tools/core/bijection.h>
   * @brief Dummy classes for discriminating scalars and non-scalars operators
   * and -> wihtout any overhead.
   * @ingroup bijection_group
   *
   * This will be used for scalar types.
   */
  template < bool gen >
  struct BijectionIteratorGet {
    /**
     * @brief Returns a refeence over a pointer.
     * @param x The pointer for which a reference is returned.
     * @return Returns a reference over x.
     */
    template < typename T >
    INLINE static const T& op_second(const T* x) {
      return *x;
    }
  };

  /**
   * @class BijectionIteratorGet
   * @headerfile bijection.h <agrum/tools/core/bijection.h>
   * @brief Dummy classes for discriminating scalars and non-scalars operators
   * and -> wihtout any overhead.
   * @ingroup bijection_group
   *
   * This will be used for non-scala types.
   */
  template <>
  struct BijectionIteratorGet< true > {
    /**
     * @brief Returns a reference.
     * @param x A reference.
     * @return Returns the reference x.
     */
    template < typename T >
    INLINE static const T& op_second(const T& x) {
      return x;
    }
  };

  // ===========================================================================
  // ===                      BIJECTION SAFE ITERATORS                       ===
  // ===========================================================================

  /**
   * @class BijectionIteratorSafe
   * @headerfile bijection.h <agrum/tools/core/bijection.h>
   * @brief Safe iterators for bijectionIterator.
   * @ingroup bijection_group
   *
   * @tparam T1 The first type of elements in the gum::Bjection.
   * @tparam T2 The second type of elements in the gum::Bjection.
   */
  template < typename T1, typename T2 >
  class BijectionIteratorSafe {
    template < typename TT1, typename TT2, bool >
    friend class BijectionImplementation;

    public:
    /// types for STL compliance
    /// @{
    using iterator_category     = std::forward_iterator_tag;
    using type1_type            = T1;
    using type1_reference       = T1&;
    using type1_const_reference = const T1&;
    using type1_pointer         = T1*;
    using type1_const_pointer   = const T1*;
    using type2_type            = T2;
    using type2_reference       = T2&;
    using type2_const_reference = const T2&;
    using type2_pointer         = T2*;
    using type2_const_pointer   = const T2*;
    using difference_type       = std::ptrdiff_t;
    /// @}

    private:
    /**
     * Dummy classes that will enable discriminate without overhead between
     * scalars and non-scalars functions second in iterators
     */
    using Getter
       = BijectionIteratorGet< std::is_scalar< T1 >::value && std::is_scalar< T2 >::value >;

    /**
     * @brief Begin constructor.
     * By default, the iterator points to the starting point of the
     * gum::Bijection.
     * @param bijection The gum::Bijection to iterate onto.
     */
    template < bool Gen >
    BijectionIteratorSafe(const BijectionImplementation< T1, T2, Gen >& bijection);

    public:
    // ============================================================================
    /// @name Constructors and destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    explicit BijectionIteratorSafe() noexcept;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static endSafe iterator
    // only bijection.cpp should use this constructor
    explicit consteval BijectionIteratorSafe(StaticInitializer init) noexcept : _iter_(init) {}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /**
     * @brief Begin constructor.
     * By default, the iterator points to the starting point of the
     * gum::Bijection.
     * @param bijection The gum::Bijection to iterate onto.
     */
    BijectionIteratorSafe(const Bijection< T1, T2 >& bijection);

    /**
     * @brief Copy constructor.
     * @param from The gum::BijectionIteratorSafe to copy.
     */
    BijectionIteratorSafe(const BijectionIteratorSafe< T1, T2 >& from);

    /**
     * @brief Move constructor.
     * @param from The gum::BijectionIteratorSafe to move.
     */
    BijectionIteratorSafe(BijectionIteratorSafe< T1, T2 >&& from) noexcept;

    /**
     * @brief Class destructor.
     */
    ~BijectionIteratorSafe() noexcept;

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param toCopy The gum::BijectionIteratorSafe to copy.
     * @return Returns this gum::BijectionIteratorSafe.
     */
    BijectionIteratorSafe< T1, T2 >& operator=(const BijectionIteratorSafe< T1, T2 >& toCopy);

    /**
     * @brief Move operator.
     * @param toMove The gum::BijectionIteratorSafe to move.
     * @return Returns this gum::BijectionIteratorSafe.
     */
    BijectionIteratorSafe< T1, T2 >& operator=(BijectionIteratorSafe< T1, T2 >&& toMove) noexcept;

    /**
     * @brief Go to the next association, if it exists.
     *
     * If the iterator points to end(), nothing happens.
     * @return Returns this gum::BijectionIteratorSafe.
     */
    BijectionIteratorSafe< T1, T2 >& operator++() noexcept;

    /**
     * @brief Moves the iterator by nb elements.
     *
     * If the iterator points to end(), nothing is done. If there are
     * nb or fewer elements to parse to reach the end of the bijection, then
     * this method makes the iterator point to
     * gum::BijectionIteratorSafe::end().
     *
     * @param nb The number of steps by wich the iterator moves.
     * @return Returns this gum::BijectionIteratorSafe.
     */
    BijectionIteratorSafe< T1, T2 >& operator+=(Size nb) noexcept;

    /**
     * @brief Returns a new iterator.
     *
     * If the iterator points to end(), the resulting iterator also points to
     * end (). If there are nb or fewer elements to parse to reach the end of
     * the bijection, then the resulting iterator points to
     * gum::BijectionIteratorSafe::end().
     *
     * @param nb The number of steps by wich the iterator moves.
     * @return Returns this gum::BijectionIteratorSafe.
     */
    BijectionIteratorSafe< T1, T2 > operator+(Size nb) noexcept;

    /**
     * @brief Inequality operator.
     * @param toCompare The gum::BijectionIteratorSafe to compare.
     * @return Returns true if they differ.
     */
    bool operator!=(const BijectionIteratorSafe< T1, T2 >& toCompare) const noexcept;

    /**
     * @brief Equality operator.
     * @param toCompare The gum::BijectionIteratorSafe to compare.
     * @return Returns true if they are equal.
     */
    bool operator==(const BijectionIteratorSafe< T1, T2 >& toCompare) const noexcept;

    /// @}
    // ============================================================================
    /// @name Accessors/Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Returns the first element of the current association.
     * @return Returns the first element of the current association.
     * @throws UndefinedIteratorValue Raised when the iterator does not point
     * to a valid element of the bijection.
     */
    const T1& first() const;

    /**
     * @brief Returns the second element of the current association.
     * @return Returns the second element of the current association.
     * @throws UndefinedIteratorValue Raised when the iterator does not point
     * to a valid element of the bijection.
     */
    const T2& second() const;

    /// @}

    private:
    /// Alias for one of the internal gum::HashTable of the gum::Bijection.
    using HashTable12 =
       typename std::conditional< std::is_scalar< T1 >::value && std::is_scalar< T2 >::value,
                                  HashTable< T1, T2 >,
                                  HashTable< T1, T2* > >::type;

    /// Alias for one of the internal gum::HastTableIterator of the
    /// gum::Bijection.
    using HashIter = typename HashTable12::const_iterator_safe;

    /// The hashTable iterator that actually does all the job.
    HashIter _iter_;
  };

  // ===========================================================================
  // ===                     BIJECTION UNSAFE ITERATORS                      ===
  // ===========================================================================
  /**
   * @class BijectionIterator
   * @headerfile bijection.h <agrum/tools/core/bijection.h>
   * @brief Unsafe iterators for bijection.
   * @ingroup bijection_group
   *
   * @tparam T1 The first type of elements in the gum::Bjection.
   * @tparam T2 The second type of elements in the gum::Bjection.
   */
  template < typename T1, typename T2 >
  class BijectionIterator {
    template < typename TT1, typename TT2, bool >
    friend class BijectionImplementation;

    public:
    /// types for STL compliance
    /// @{
    using iterator_category     = std::forward_iterator_tag;
    using type1_type            = T1;
    using type1_reference       = T1&;
    using type1_const_reference = const T1&;
    using type1_pointer         = T1*;
    using type1_const_pointer   = const T1*;
    using type2_type            = T2;
    using type2_reference       = T2&;
    using type2_const_reference = const T2&;
    using type2_pointer         = T2*;
    using type2_const_pointer   = const T2*;
    using difference_type       = std::ptrdiff_t;
    /// @}

    private:
    /**
     * Dummy classes that will enable discriminate without overhead between
     * scalars and non-scalars functions second in iterators
     */
    using Getter
       = BijectionIteratorGet< std::is_scalar< T1 >::value && std::is_scalar< T2 >::value >;

    /**
     * @brief Begin constructor.
     * By default, the iterator points to the starting point of the bijection.
     */
    template < bool Gen >
    BijectionIterator(const BijectionImplementation< T1, T2, Gen >& bijection);

    public:
    // ============================================================================
    /// @name Constructors/destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    explicit BijectionIterator() noexcept;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // constructor for the static end iterator
    // only bijection.cpp should use this constructor
    explicit consteval BijectionIterator(StaticInitializer init) noexcept : _iter_(init) {}
#endif   // DOXYGEN_SHOULD_SKIP_THIS

    /**
     * @brief Default constructor.
     * @param bijection The gum::Bijection to iterate onto.
     */
    BijectionIterator(const Bijection< T1, T2 >& bijection);

    /**
     * @brief Copy constructor.
     * @param from The gum::BijectionIterator to copy.
     */
    BijectionIterator(const BijectionIterator< T1, T2 >& from);

    /**
     * @brief Move constructor.
     * @param from The gum::BijectionIterator to move.
     */
    BijectionIterator(BijectionIterator< T1, T2 >&& from) noexcept;

    /**
     * Class destructor.
     */
    ~BijectionIterator() noexcept;

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param toCopy The gum::BijectionIterator to copy.
     * @return Returns this gum::BijectionIterator.
     */
    BijectionIterator< T1, T2 >& operator=(const BijectionIterator< T1, T2 >& toCopy);

    /**
     * @brief Move operator.
     * @param toMove The gum::BijectionIterator to move.
     * @return Returns this gum::BijectionIterator.
     */
    BijectionIterator< T1, T2 >& operator=(BijectionIterator< T1, T2 >&& toMove) noexcept;

    /**
     * @brief Go to the next association, if it exists.
     *
     * If the iterator points to gum::Bijection::end(), nothing is done.
     *
     * @return Return sthis gum::BijectionIterator.
     */
    BijectionIterator< T1, T2 >& operator++() noexcept;

    /**
     * @brief Moves the iterator by nb elements.
     *
     * If the iterator points to gum::Bijection::end(), nothing is done. If
     * there are nb or
     * fewer elements to parse to reach the end of the bijection, then this
     * method makes the iterator point to gum::Bijection::end().
     *
     * @param nb The number of steps by wich the iterator moves.
     * @return Returns this gum::BijectionIterator.
     */
    BijectionIterator< T1, T2 >& operator+=(Size nb) noexcept;

    /**
     * @brief Return a new iterator.
     *
     * If the iterator points to gum::Bijection::end(), the resulting iterator
     * also points to gum::Bijection::end(). If there are nb or fewer elements
     * to parse to reach the end of the gum::Bijection, then the resulting
     * iterator points to gum::Bijection::end().
     */
    BijectionIterator< T1, T2 > operator+(Size nb) noexcept;

    /**
     * @brief Inequality operator.
     * @param toCompare The gum::BijectionIteratorSafe to compare.
     * @return Returns true if they differ.
     */
    bool operator!=(const BijectionIterator< T1, T2 >& toCompare) const noexcept;

    /**
     * @brief Equality operator.
     * @param toCompare The gum::BijectionIteratorSafe to compare.
     * @return Returns true if they are equal.
     */
    bool operator==(const BijectionIterator< T1, T2 >& toCompare) const noexcept;

    /// @}
    // ============================================================================
    /// @name Accessors/Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Returns the first element of the current association.
     * @return Returns the first element of the current association.
     * @throws UndefinedIteratorValue Raised when the iterator does not point
     * to a valid element of the bijection.
     */
    const T1& first() const;

    /**
     * @brief Returns the second element of the current association.
     * @return Returns the second element of the current association.
     * @throws UndefinedIteratorValue Raised when the iterator does not point
     * to a valid element of the bijection.
     */
    const T2& second() const;

    /// @}

    private:
    /// Alias for one of the internal gum::HashTable of the gum::Bijection.
    using HashTable12 =
       typename std::conditional< std::is_scalar< T1 >::value && std::is_scalar< T2 >::value,
                                  HashTable< T1, T2 >,
                                  HashTable< T1, T2* > >::type;
    using HashIter = typename HashTable12::const_iterator;

    /// The hashTable iterator that actually does all the job.
    HashIter _iter_;
  };

  /**
   * @class Bijection
   * @headerfile bijection.h <agrum/tools/core/bijection.h>
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
   */
  template < typename T1, typename T2 >
  class Bijection:
      public BijectionImplementation< T1,
                                      T2,
                                      std::is_scalar< T1 >::value && std::is_scalar< T2 >::value > {
    public:
    /// types for STL compliance
    /// @{
    using type1_type            = T1;
    using type1_reference       = T1&;
    using type1_const_reference = const T1&;
    using type1_pointer         = T1*;
    using type1_const_pointer   = const T1*;
    using type2_type            = T2;
    using type2_reference       = T2&;
    using type2_const_reference = const T2&;
    using type2_pointer         = T2*;
    using type2_const_pointer   = const T2*;
    using size_type             = std::size_t;
    using difference_type       = std::ptrdiff_t;
    using iterator              = BijectionIterator< T1, T2 >;
    using const_iterator        = BijectionIterator< T1, T2 >;
    using iterator_safe         = BijectionIteratorSafe< T1, T2 >;
    using const_iterator_safe   = BijectionIteratorSafe< T1, T2 >;
    /// @}

    /// The Implementation of this gum::Bijection.
    using Implementation
       = BijectionImplementation< T1,
                                  T2,
                                  std::is_scalar< T1 >::value && std::is_scalar< T2 >::value >;

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
    Bijection(Size size          = HashTableConst::default_size,
              bool resize_policy = HashTableConst::default_resize_policy);

    /**
     * @brief Initializer list constructor.
     * @param list The initialisation list.
     */
    Bijection(std::initializer_list< std::pair< T1, T2 > > list);

    /**
     * @brief Copy constructor.
     * @param toCopy The gum::Bijection to copy.
     */
    Bijection(const Bijection< T1, T2 >& toCopy);

    /**
     * @brief Move constructor.
     * @param from The gum::Bijection to move from.
     */
    Bijection(Bijection< T1, T2 >&& from) noexcept;

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
    Bijection< T1, T2 >& operator=(const Bijection< T1, T2 >& toCopy);

    /**
     * @brief Move operator.
     * @param bij The gum::Bijection to move from.
     */
    Bijection< T1, T2 >& operator=(Bijection< T1, T2 >&& bij);

    /// @}
  };


#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // _static_Bijection_end_ is a 'constant' iterator initialized at compile time
  // that represents the end iterators for all bijections (whatever their
  // type). This global variable avoids creating the same iterators within every
  // Bijection instance (this would be quite inefficient as end is precisely
  // identical for all bijections). The same hold for safe end iterators.
  // The type of _Bijection_end_ is a pointer to void because C++ allows
  // pointers to void to be cast into pointers to other types (and conversely).
  // This avoids the painful strict-aliasing rule warning
  extern const BijectionIterator< int, int >     _static_Bijection_end_;
  extern const BijectionIteratorSafe< int, int > _static_Bijection_end_safe_;

  inline constexpr void* const _Bijection_end_      = (void* const)&_static_Bijection_end_;
  inline constexpr void* const _Bijection_end_safe_ = (void* const)&_static_Bijection_end_safe_;
#endif   // DOXYGEN_SHOULD_SKIP_THIS


  /**
   * @brief For friendly display of the content of the gum::Bijection.
   *
   * @param bijection The gum::Bijection to display.
   * @tparam T1 The first type of elements in the gum::Bjection.
   * @tparam T2 The second type of elements in the gum::Bjection.
   * @return The stream in which the gum::Bijection is displayed.
   */
  template < typename T1, typename T2 >
  std::ostream& operator<<(std::ostream&, const Bijection< T1, T2 >& bijection);

} /* namespace gum */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::Bijection< int, int >;
extern template class gum::Bijection< std::string, std::string >;
#endif


// always include the template implementations
#include <agrum/tools/core/bijection_tpl.h>

#endif /* GUM_BIJECTION_H */
