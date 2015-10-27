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
 * @brief Iterators for gum::Bijection 
 *
 * @author Jean-Philippe DUBUS, Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

// To simply IDE parsing
#include <agrum/core/bijectionIterator.h>

// This must stay here due to cyclic dependencies
#include <agrum/core/bijection.h>

namespace gum {

  // ===========================================================================
  // ===                      BIJECTION SAFE ITERATORS                       ===
  // ===========================================================================

  /// Default constructor
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2>::BijectionIteratorSafe() noexcept {
    GUM_CONSTRUCTOR( BijectionIteratorSafe );
  }

  /// Constructor
  template <typename T1, typename T2>
  template <typename Alloc, bool Gen>
  INLINE BijectionIteratorSafe<T1, T2>::BijectionIteratorSafe(
      const BijectionImplementation<T1, T2, Alloc, Gen>& bijection )
      : __iter{bijection.__firstToSecond.cbeginSafe()} {
    GUM_CONSTRUCTOR( BijectionIteratorSafe );
  }

  /// Constructor
  template <typename T1, typename T2>
  template <typename Alloc>
  INLINE BijectionIteratorSafe<T1, T2>::BijectionIteratorSafe(
      const Bijection<T1, T2, Alloc>& bijection )
      : __iter{bijection.__firstToSecond.cbeginSafe()} {
    GUM_CONSTRUCTOR( BijectionIteratorSafe );
  }

  /// Copy constructor
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2>::BijectionIteratorSafe(
      const BijectionIteratorSafe<T1, T2>& toCopy )
      : __iter{toCopy.__iter} {
    GUM_CONS_CPY( BijectionIteratorSafe );
  }

  /// move constructor
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2>::BijectionIteratorSafe(
      BijectionIteratorSafe<T1, T2>&& from ) noexcept
      : __iter{std::move( from.__iter )} {
    GUM_CONS_MOV( BijectionIteratorSafe );
  }

  /// Destructor
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2>::~BijectionIteratorSafe() noexcept {
    GUM_DESTRUCTOR( BijectionIteratorSafe );
  }

  /// Copy operator
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2>& BijectionIteratorSafe<T1, T2>::
  operator=( const BijectionIteratorSafe<T1, T2>& toCopy ) {
    __iter = toCopy.__iter;
    return *this;
  }

  /// move operator
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2>& BijectionIteratorSafe<T1, T2>::
  operator=( BijectionIteratorSafe<T1, T2>&& toCopy ) noexcept {
    __iter = std::move( toCopy.__iter );
    return *this;
  }

  /// Go to the next association (if exists)
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2>& BijectionIteratorSafe<T1, T2>::
  operator++() noexcept {
    ++__iter;
    return *this;
  }

  /// moves the iterator by nb elements
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2>& BijectionIteratorSafe<T1, T2>::
  operator+=( unsigned int nb ) noexcept {
    __iter += nb;
    return *this;
  }

  /// returns a new iterator
  template <typename T1, typename T2>
  INLINE BijectionIteratorSafe<T1, T2> BijectionIteratorSafe<T1, T2>::
  operator+( unsigned int nb ) noexcept {
    return BijectionIteratorSafe<T1, T2>{* this} += nb;
  }

  /// Comparison of iterators
  template <typename T1, typename T2>
  INLINE bool BijectionIteratorSafe<T1, T2>::
  operator!=( const BijectionIteratorSafe<T1, T2>& toCompare ) const noexcept {
    return __iter != toCompare.__iter;
  }

  /// Comparison of iterators
  template <typename T1, typename T2>
  INLINE bool BijectionIteratorSafe<T1, T2>::
  operator==( const BijectionIteratorSafe<T1, T2>& toCompare ) const noexcept {
    return __iter == toCompare.__iter;
  }

  /// return the first element of the current association
  template <typename T1, typename T2>
  INLINE const T1& BijectionIteratorSafe<T1, T2>::first() const {
    return __iter.key();
  }

  /// return the second element of the current association
  template <typename T1, typename T2>
  INLINE const T2& BijectionIteratorSafe<T1, T2>::second() const {
    return Getter::op_second( __iter.val() );
  }

  /* ===========================================================================
   */
  /* ===                     BIJECTION UNSAFE ITERATORS                      ===
   */
  /* ===========================================================================
   */

  /// Default constructor
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2>::BijectionIterator() noexcept {
    GUM_CONSTRUCTOR( BijectionIterator );
  }

  /// Constructor
  template <typename T1, typename T2>
  template <typename Alloc, bool Gen>
  INLINE BijectionIterator<T1, T2>::BijectionIterator(
      const BijectionImplementation<T1, T2, Alloc, Gen>& bijection )
      : __iter{bijection.__firstToSecond.cbegin()} {
    GUM_CONSTRUCTOR( BijectionIterator );
  }

  /// Constructor
  template <typename T1, typename T2>
  template <typename Alloc>
  INLINE BijectionIterator<T1, T2>::BijectionIterator(
      const Bijection<T1, T2, Alloc>& bijection )
      : __iter{bijection.__firstToSecond.cbegin()} {
    GUM_CONSTRUCTOR( BijectionIterator );
  }

  /// Copy constructor
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2>::BijectionIterator(
      const BijectionIterator<T1, T2>& toCopy )
      : __iter{toCopy.__iter} {
    GUM_CONS_CPY( BijectionIterator );
  }

  /// move constructor
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2>::BijectionIterator(
      BijectionIterator<T1, T2>&& from ) noexcept
      : __iter{std::move( from.__iter )} {
    GUM_CONS_MOV( BijectionIterator );
  }

  /// Destructor
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2>::~BijectionIterator() noexcept {
    GUM_DESTRUCTOR( BijectionIterator );
  }

  /// Copy operator
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2>& BijectionIterator<T1, T2>::
  operator=( const BijectionIterator<T1, T2>& toCopy ) {
    __iter = toCopy.__iter;
    return *this;
  }

  /// move operator
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2>& BijectionIterator<T1, T2>::
  operator=( BijectionIterator<T1, T2>&& toCopy ) noexcept {
    __iter = std::move( toCopy.__iter );
    return *this;
  }

  /// Go to the next association (if exists)
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2>& BijectionIterator<T1, T2>::
  operator++() noexcept {
    ++__iter;
    return *this;
  }

  /// moves the iterator by nb elements
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2>& BijectionIterator<T1, T2>::
  operator+=( unsigned int nb ) noexcept {
    __iter += nb;
    return *this;
  }

  /// returns a new iterator
  template <typename T1, typename T2>
  INLINE BijectionIterator<T1, T2> BijectionIterator<T1, T2>::
  operator+( unsigned int nb ) noexcept {
    return BijectionIterator<T1, T2>{* this} += nb;
  }

  /// Comparison of iterators
  template <typename T1, typename T2>
  INLINE bool BijectionIterator<T1, T2>::
  operator!=( const BijectionIterator<T1, T2>& toCompare ) const noexcept {
    return __iter != toCompare.__iter;
  }

  /// Comparison of iterators
  template <typename T1, typename T2>
  INLINE bool BijectionIterator<T1, T2>::
  operator==( const BijectionIterator<T1, T2>& toCompare ) const noexcept {
    return __iter == toCompare.__iter;
  }

  /// return the first element of the current association
  template <typename T1, typename T2>
  INLINE const T1& BijectionIterator<T1, T2>::first() const {
    return __iter.key();
  }

  /// return the second element of the current association
  template <typename T1, typename T2>
  INLINE const T2& BijectionIterator<T1, T2>::second() const {
    return Getter::op_second( __iter.val() );
  }

} // namespace gum
