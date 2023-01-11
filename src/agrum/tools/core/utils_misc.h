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
 * @brief Utilities for aGrUM.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_UTILS_H
#define GUM_UTILS_H

#include <cstdlib>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>
#include <tuple>
#include <string>
#include <type_traits>


#ifdef GUM_DEBUG_MODE
#  define GUM_CAST dynamic_cast
#else
#  define GUM_CAST static_cast
#endif   // GUM_DEBUG_MODE

#include <agrum/agrum.h>

#include <agrum/tools/core/utils_dir.h>
#include <agrum/tools/core/utils_random.h>
#include <agrum/tools/core/utils_string.h>

namespace std {

  /// @ingroup utilities_group
  /// @{

  /**
   * @brief 'std::cout<<' operator for pairs.
   * @tparam T1 The pair's first's type.
   * @tparam T2 The pair's second's type.
   * @param stream The stream to print to.
   * @param val The pair to print to stream.
   * @return Returns stream.
   */
  template < typename T1, typename T2 >
  ostream& operator<<(ostream& stream, const pair< T1, T2 >& val);

  /**
   * @brief 'std::cout<<' operator for vectors.
   * @param stream The stream to print to.
   * @tparam t The std::tuple
   * @param val The std::vector to print to stream.
   * @return Returns stream.
   */
  template < typename T0, typename... T >
  std::ostream& operator<<(std::ostream& os, const std::tuple< T0, T... >& t);


  /**
   * @brief 'std::cout<<' operator for vectors.
   * @tparam T The vector's elements type.
   * @param stream The stream to print to.
   * @param val The std::vector to print to stream.
   * @return Returns stream.
   */
  template < typename T >
  ostream& operator<<(ostream& stream, const vector< T >& val);

  /**   *
   * @brief check if a vector consists in unique values (no duplicate).
   * @tparam T The vector's elements type.
   * @param x the vector
   * @return Returns true if the vector has no duplicate.
   */
  template < typename T >
  bool hasUniqueElts(std::vector< T > const& x);

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
  template < typename T1, typename T2 >
  using forbidden_type = typename std::enable_if< !std::is_same< T1, T2 >::value, int >::type;

  /**
   * @brief Implements a stream with the same behaviour as /dev/null.
   */
  struct NullStream: std::ostream {
    NullStream() : std::ios(0), std::ostream(0) {}
  };

  /**
   * @brief Cross-platform replacement for memcmp.
   * @param in_ A pointer to the block of memory to copy.
   * @param out_ A pointer to the block of memory receiving copy.
   * @param size Number of bytes to copy.
   * @return Returns true if OK.
   */
  bool Memcmp(const void* const in_, const void* const out_, unsigned long size);

  /**
   * @brief Used for debug purpose.
   */
  void _atexit_();

  /**
   * @brief Indicate whether two elements are (almost) different or not.
   * @tparam T The type of the elements to compare.
   */
  template < typename T >
  struct AlmostDifferent {
    bool operator()(const T& t1, const T& t2) {
      if (t1 == t2) return false;
      else if (t1 == 0) return (std::abs(t2) > 1e-5);
      else return (std::abs(t2 - t1) / t1 > 1e-5);
    }
  };

  /**
   * @brief Indicate whether two elements are (almost) different or not.
   * @tparam T The type of the elements to compare.
   */
  template < typename T >
  struct AlmostDifferent< T* > {
    bool operator()(const T* t1, const T* t2) { return (t1 != t2); }
  };

  /// @}

} /* namespace gum */

// Always include the template implementations
#include <agrum/tools/core/utils_misc_tpl.h>

#endif /* GUM_UTILS_H */
