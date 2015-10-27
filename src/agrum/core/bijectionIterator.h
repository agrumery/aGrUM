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
 * @brief Iterators for gum::Bijection.
 *
 * @author Christophe GONZALES and Jean-Philippe DUBUS
 */
#ifndef GUM_BIJECTION_ITERATOR_H
#define GUM_BIJECTION_ITERATOR_H

#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>
#include <initializer_list>

#include <agrum/config.h>
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


  /**
   * @class BijectionIteratorStaticEnd
   * @headerfile bijectionIterator.h <agrum/core/bijectionIterator.h>
   * @brief A class which creates the static iterator used by gim::Bijections.
   * @ingroup bijection_group
   *
   * The aim of using this class rather than just creating __BijectionIterEnd
   * as a global variable is to prevent other classes to access and modify
   * __BijectionIterEnd.
   */
  class BijectionIteratorStaticEnd {

    /// Friends that have access to the iterator
    template <typename T1, typename T2, typename Alloc, bool>
    friend class BijectionImplementation;

    private:
    /// The safe iterator used by everyone
    static const BijectionIteratorSafe<int, int>* __BijectionIterEndSafe;
 
    /// The unsafe iterator used by everyone.
    static const BijectionIterator<int, int>* __BijectionIterEnd;

    /**
     * @brief Creates (if needed) and returns the iterator __BijectionIterEndSafe
     * @return Returns __BijectionIterEndSafe.
     */
    static const BijectionIteratorSafe<int, int>* endSafe4Statics();

    /**
     * @brief Creates (if needed) and returns the iterator __BijectionIterEnd.
     * @return Returns __BijectionIterEnd;
     */
    static const BijectionIterator<int, int>* end4Statics();

  };

  /**
   * @class BijectionIteratorGet
   * @headerfile bijectionIterator.h <agrum/core/bijectionIterator.h>
   * @brief Dummy classes for discriminating scalars and non-scalars operators
   * and -> wihtout any overhead.
   * @ingroup bijection_group
   *
   * This will be used for scalar types.
   */
  template <bool gen>
  struct BijectionIteratorGet {

    /**
     * @brief Returns a refeence over a pointer.
     * @param x The pointer for which a reference is returned.
     * @return Returns a reference over x.
     */
    template <typename T>
    INLINE static const T& op_second( const T* x ) {
      return *x;
    }

  };

  /**
   * @class BijectionIteratorGet bijectionIterator.h <agrum/core/bijectionIterator.h>
   * @brief Dummy classes for discriminating scalars and non-scalars operators
   * and -> wihtout any overhead.
   * @ingroup bijection_group
   *
   * This will be used for non-scala types.
   */
  template <>
  struct BijectionIteratorGet<true> {

    /**
     * @brief Returns a reference.
     * @param x A reference.
     * @return Returns the reference x.
     */
    template <typename T>
    INLINE static const T& op_second( const T& x ) {
      return x;
    }
  };

  // ===========================================================================
  // ===                      BIJECTION SAFE ITERATORS                       ===
  // ===========================================================================

  /**
   * @class BijectionIteratorSafe bijectionIterator.h <agrum/core/bijectionIterator.h>
   * @brief Safe iterators for bijectionIterator.
   * @ingroup bijection_group
   *
   * @tparam T1 The first type of elements in the gum::Bjection.
   * @tparam T2 The second type of elements in the gum::Bjection.
   */
  template <typename T1, typename T2>
  class BijectionIteratorSafe {

    template <typename TT1, typename TT2, typename Alloc, bool>
    friend class BijectionImplementation;

    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::forward_iterator_tag;
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
    using difference_type = std::ptrdiff_t;
    /// @}

    private:
    /**
     * Dummy classes that will enable discriminate without overhead between
     * scalars and non-scalars functions second in iterators
     */
    using Getter = BijectionIteratorGet<std::is_scalar<T1>::value &&
                                        std::is_scalar<T2>::value>;

    /**
     * @brief Begin constructor.
     * By default, the iterator points to the starting point of the
     * gum::Bijection.
     * @param bijection The gum::Bijection to iterate onto.
     */
    template <typename Alloc, bool Gen>
    BijectionIteratorSafe(
        const BijectionImplementation<T1, T2, Alloc, Gen>& bijection );

    public:
    // ============================================================================
    /// @name Constructors and destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    BijectionIteratorSafe() noexcept;

    /**
     * @brief Genereliazed default constructor.
     * @tparam Alloc The gum::Bijection allocator's type.
     */
    template <typename Alloc>
    BijectionIteratorSafe( const Bijection<T1, T2, Alloc>& bijection );

    /**
     * @brief Copy constructor.
     * @param from The gum::BijectionIteratorSafe to copy.
     */
    BijectionIteratorSafe( const BijectionIteratorSafe<T1, T2>& from );

    /**
     * @brief Move constructor.
     * @param from The gum::BijectionIteratorSafe to move.
     */
    BijectionIteratorSafe( BijectionIteratorSafe<T1, T2>&& from ) noexcept;

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
    BijectionIteratorSafe<T1, T2>&
    operator=( const BijectionIteratorSafe<T1, T2>& toCopy );

    /**
     * @brief Move operator.
     * @param toMove The gum::BijectionIteratorSafe to move.
     * @return Returns this gum::BijectionIteratorSafe.
     */
    BijectionIteratorSafe<T1, T2>&
    operator=( BijectionIteratorSafe<T1, T2>&& toMove ) noexcept;

    /**
     * @brief Go to the next association, if it exists.
     *
     * If the iterator points to end(), nothing happens.
     * @return Returns this gum::BijectionIteratorSafe.
     */
    BijectionIteratorSafe<T1, T2>& operator++() noexcept;

    /**
     * @brief Moves the iterator by nb elements.
     *
     * If the iterator points to end(), nothing is done. If there are
     * nb or fewer elements to parse to reach the end of the bijection, then
     * this method makes the iterator point to gum::BijectionIteratorSafe::end().
     *
     * @param nb The number of steps by wich the iterator moves.
     * @return Returns this gum::BijectionIteratorSafe.
     */
    BijectionIteratorSafe<T1, T2>& operator+=( unsigned int nb ) noexcept;

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
    BijectionIteratorSafe<T1, T2> operator+( unsigned int nb ) noexcept;

    /**
     * @brief Inequality operator.
     * @param toCompare The gum::BijectionIteratorSafe to compare.
     * @return Returns true if they differ.
     */
    bool operator!=( const BijectionIteratorSafe<T1, T2>& toCompare ) const
        noexcept;

    /**
     * @brief Equality operator.
     * @param toCompare The gum::BijectionIteratorSafe to compare.
     * @return Returns true if they are equal.
     */
    bool operator==( const BijectionIteratorSafe<T1, T2>& toCompare ) const
        noexcept;

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
    using HashTable12 = typename std::conditional<
        std::is_scalar<T1>::value && std::is_scalar<T2>::value,
        HashTable<T1, T2, std::allocator<std::pair<T1, T2>>>,
        HashTable<T1, T2*, std::allocator<std::pair<T1, T2*>>>>::type;
 
    /// Alias for one of the internal gum::HastTableIterator of the
    /// gum::Bijection.
    using HashIter = typename HashTable12::const_iterator_safe;

    /// The hashTable iterator that actually does all the job.
    HashIter __iter;
  };

  // ===========================================================================
  // ===                     BIJECTION UNSAFE ITERATORS                      ===
  // ===========================================================================
  /**
   * @class BijectionIterator
   * @headerfile bijectionIterator.h <agrum/core/bijectionIterator.h>
   * @brief Unsafe iterators for bijection.
   * @ingroup bijection_group
   *
   * @tparam T1 The first type of elements in the gum::Bjection.
   * @tparam T2 The second type of elements in the gum::Bjection.
   */
  template <typename T1, typename T2>
  class BijectionIterator {

    template <typename TT1, typename TT2, typename Alloc, bool>
    friend class BijectionImplementation;

    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::forward_iterator_tag;
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
    using difference_type = std::ptrdiff_t;
    /// @}

    private:
    /**
     * Dummy classes that will enable discriminate without overhead between
     * scalars and non-scalars functions second in iterators
     */
    using Getter = BijectionIteratorGet<std::is_scalar<T1>::value &&
                                        std::is_scalar<T2>::value>;

    /**
     * @brief Begin constructor.
     * By default, the iterator points to the starting point of the bijection.
     */
    template <typename Alloc, bool Gen>
    BijectionIterator(
        const BijectionImplementation<T1, T2, Alloc, Gen>& bijection );

    public:
    // ============================================================================
    /// @name Constructors/destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    BijectionIterator() noexcept;

    /**
     * @brief Default constructor.
     * @param bijection The gum::Bijection to iterate onto.
     */
    template <typename Alloc>
    BijectionIterator( const Bijection<T1, T2, Alloc>& bijection );

    /**
     * @brief Copy constructor.
     * @param from The gum::BijectionIterator to copy.
     */
    BijectionIterator( const BijectionIterator<T1, T2>& from );

    /**
     * @brief Move constructor.
     * @param from The gum::BijectionIterator to move.
     */
    BijectionIterator( BijectionIterator<T1, T2>&& from ) noexcept;

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
    BijectionIterator<T1, T2>&
    operator=( const BijectionIterator<T1, T2>& toCopy );

    /**
     * @brief Move operator.
     * @param toMove The gum::BijectionIterator to move.
     * @return Returns this gum::BijectionIterator.
     */
    BijectionIterator<T1, T2>&
    operator=( BijectionIterator<T1, T2>&& toMove ) noexcept;

    /**
     * @brief Go to the next association, if it exists.
     *
     * If the iterator points to gum::Bijection::end(), nothing is done.
     *
     * @return Return sthis gum::BijectionIterator.
     */
    BijectionIterator<T1, T2>& operator++() noexcept;

    /**
     * @brief Moves the iterator by nb elements.
     * 
     * If the iterator points to gum::Bijection::end(), nothing is done. If there are nb or
     * fewer elements to parse to reach the end of the bijection, then this
     * method makes the iterator point to gum::Bijection::end().
     * 
     * @param nb The number of steps by wich the iterator moves.
     * @return Returns this gum::BijectionIterator.
     */
    BijectionIterator<T1, T2>& operator+=( unsigned int nb ) noexcept;

    /**
     * @brief Return a new iterator.
     *
     * If the iterator points to gum::Bijection::end(), the resulting iterator
     * also points to gum::Bijection::end(). If there are nb or fewer elements
     * to parse to reach the end of the gum::Bijection, then the resulting
     * iterator points to gum::Bijection::end().
     */
    BijectionIterator<T1, T2> operator+( unsigned int nb ) noexcept;

    /**
     * @brief Inequality operator.
     * @param toCompare The gum::BijectionIteratorSafe to compare.
     * @return Returns true if they differ.
     */
    bool operator!=( const BijectionIterator<T1, T2>& toCompare ) const
        noexcept;

    /**
     * @brief Equality operator.
     * @param toCompare The gum::BijectionIteratorSafe to compare.
     * @return Returns true if they are equal.
     */
    bool operator==( const BijectionIterator<T1, T2>& toCompare ) const
        noexcept;

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
    using HashTable12 = typename std::conditional<
        std::is_scalar<T1>::value && std::is_scalar<T2>::value,
        HashTable<T1, T2, std::allocator<std::pair<T1, T2>>>,
        HashTable<T1, T2*, std::allocator<std::pair<T1, T2*>>>>::type;
    using HashIter = typename HashTable12::const_iterator;

    /// The hashTable iterator that actually does all the job.
    HashIter __iter;
  };

} /* namespace gum */

// always include the template implementations
#include <agrum/core/bijectionIterator.tcc>

#endif // GUM_BIJECTION_ITERATOR_H
