/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

/**
 * @file
 * @brief Provides C++20 concepts used in aGrUM.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_CONCEPTS_H
#define GUM_CONCEPTS_H

#include <concepts>
#include <limits>

#include <type_traits>

namespace gum {

  /// @ingroup configuration_group
  /// @{

  /**
   * @brief Concept checking that std::numeric_limits is specialized for a type.
   *
   * This concept verifies that the type has proper numeric_limits support,
   * including min(), max(), and infinity() for use in aggregation operations.
   *
   * @tparam T The type to check
   */
  template < typename T >
  concept GUM_HasNumericLimits = requires {
    { std::numeric_limits< T >::min() } -> std::convertible_to< T >;
    { std::numeric_limits< T >::max() } -> std::convertible_to< T >;
    { std::numeric_limits< T >::infinity() } -> std::convertible_to< T >;
    { std::numeric_limits< T >::is_specialized } -> std::convertible_to< bool >;
  };

  /**
   * @brief Concept defining requirements for scalar types used as GUM_SCALAR.
   *
   * A type satisfies IsGumScalar if it supports:
   * - Arithmetic operations (+, -, *, /, +=, *=)
   * - Comparison operators (<, >, ==)
   * - std::numeric_limits specialization (min, max, infinity)
   * - Construction from numeric literals (int, double)
   * - Copyable and movable semantics
   *
   * This concept is designed for types like double, float, or custom numeric
   * types. It explicitly excludes complex container types like those derived
   * from MultiDimImplementation, which are containers of scalars rather than
   * scalars themselves.
   *
   * @tparam T The type to check
   *
   * @note Built-in arithmetic types (double, float, int) automatically satisfy
   *       this concept via the std::is_arithmetic_v check.
   */
  template < typename T >
  concept GUM_IsGumScalar
      = std::is_arithmetic_v< T >
     || (
            // Custom numeric type requirements:

            // Must be copyable and movable
            std::copyable< T > && std::movable< T > &&

            // Must be default constructible (for containers)
            std::default_initializable< T > &&

            // Must be constructible from common numeric literals
            std::constructible_from< T, int > && std::constructible_from< T, double > &&

            // Must support all arithmetic operations
            requires(T a, T b) {
              { a + b } -> std::convertible_to< T >;
              { a - b } -> std::convertible_to< T >;
              { a * b } -> std::convertible_to< T >;
              { a / b } -> std::convertible_to< T >;
              { a += b } -> std::same_as< T& >;
              { a *= b } -> std::same_as< T& >;
              { -a } -> std::convertible_to< T >;   // unary minus
            } &&

            // Must be totally ordered (supports <, >, <=, >=, ==, !=)
            std::totally_ordered< T >);

  /**
   * @brief Complete concept for GUM_SCALAR template parameter.
   *
   * Combines IsGumScalar arithmetic requirements with HasNumericLimits
   * to ensure the type can be used in all aGrUM operations including:
   * - Tensor operations and aggregations
   * - Probability computations
   * - Min/max projections (using numeric_limits::min/max as neutral elements)
   * - Continuous variables (using numeric_limits::infinity for bounds)
   *
   * @tparam T The type to check
   *
   * @code
   * // Usage example:
   * template < GUM_Numeric GUM_SCALAR >
   * class BayesNet { ... };
   *
   * template < GUM_Numeric GUM_SCALAR >
   * class Tensor { ... };
   * @endcode
   */
  template < typename T >
  concept GUM_Numeric = GUM_IsGumScalar< T > && GUM_HasNumericLimits< T >;

  /// @}

}   // namespace gum

#endif   // GUM_CONCEPTS_H
