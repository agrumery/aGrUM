/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it udnder the terms of the GNU General Public License as published by *
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
 * @brief Utilities for aGrUM.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_UTILS_H
#define GUM_UTILS_H

#include <cstdlib>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>
#include <vector>

// FLAG : DEBUG mode
//#undef  NDEBUG
//#define NDEBUG

#ifdef NDEBUG
#define GUM_CAST dynamic_cast
#else
#define GUM_CAST static_cast
#endif

#include <agrum/config.h>

#include <agrum/core/utils_dir.h>
#include <agrum/core/utils_random.h>
#include <agrum/core/utils_string.h>

namespace std {

  /// @ingroup utilities_group
  /// @{

  /**
   * @brief Used for hashtables (among other aGrUM classes).
   * @tparam T1 The pair's first's type.
   * @tparam T2 The pair's second's type.
   * @param stream The stream to print to.
   * @param val The pair to print to stream.
   * @return Returns stream.
   */
  template <typename T1, typename T2>
  ostream& operator<<( ostream& stream, const pair<T1, T2>& val );

  /**
   * @brief Used for hashtables (among other aGrUM classes).
   * @tparam T The vector's elements type.
   * @param stream The stream to print to.
   * @param val The std::vector to print to stream.
   * @return Returns stream.
   */
  template <typename T>
  ostream& operator<<( ostream& stream, const vector<T>& val );

  /// @}

} /* namespace std */

namespace gum {

  /// @ingroup utilities_group
  /// @{

  /**
   * @brief Forbidden_type<T1,T2> return the "int" type if T1 and T2 are of the
   * same type, else nothing.
   *
   * Use it as a guard in template specification :
   * @code
   * // Creates a template except if T is int or char
   * template<T,forbidden_type<T,int> =0,forbidden_type<T,char> =0> // ...
   * @endcode
   *
   * @param T1 The type to test for.
   * @param T2 The expected type.
   */
  template <typename T1, typename T2>
  using forbidden_type =
      typename std::enable_if<!std::is_same<T1, T2>::value, int>::type;

  /**
   * @brief Implements a stream with the same behaviour as /dev/null.
   */
  struct NullStream : std::ostream {
    NullStream()
        : std::ios( 0 )
        , std::ostream( 0 ) {}
  };

  /**
   * @brief Cross-platform replacement for memcmp.
   * @param _in A pointer to the block of memory to copy.
   * @param _out A pointer to the block of memory receiving copy.
   * @param size Number of bytes to copy.
   * @return Returns true if OK.
   */
  bool Memcmp( const void* const _in, const void* const _out, unsigned long size );

  /**
   * @brief Used for debug purpose.
   */
  void __atexit( void );

  /**
   * @brief Indicate whether two elements are (almost) different or not.
   * @tparam T The type of the elements to compare.
   */
  template <typename T>
  struct AlmostDifferent {
    bool operator()( const T& t1, const T& t2 ) {
      if ( t1 == t2 )
        return false;
      else if ( t1 == 0 )
        return ( std::abs( t2 ) > 1e-5 );
      else
        return ( std::abs( t2 - t1 ) / t1 > 1e-5 );
    }
  };

  /**
   * @brief Indicate whether two elements are (almost) different or not.
   * @tparam T The type of the elements to compare.
   */
  template <typename T>
  struct AlmostDifferent<T*> {
    bool operator()( const T* t1, const T* t2 ) { return ( t1 != t2 ); }
  };

  /// @}

} /* namespace gum */

// Always include the template implementations
#include <agrum/core/utils_misc_tpl.h>

#endif /* GUM_UTILS_H */
