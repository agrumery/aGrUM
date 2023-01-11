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
 * @brief This files contains several function objects that are not (yet) defined
 *        in the STL
 *
 * Generically, function objects are instances of a class with member function
 * operator() defined.
 * This member function allows the object to be used with the same syntax as a
 * function call.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_FUNCTORS_H
#define GUM_FUNCTORS_H
// =========================================================================
#include <cstdlib>
// =========================================================================
#include <agrum/tools/core/argMaxSet.h>
// =========================================================================


namespace gum {

  /**
   * @struct Maximizes functors.h <agrum/tools/core/functors.h>
   * @brief Maximization function object class
   * @ingroup core
   *
   * Returns the maximum of its two arguments
   */
  template < class GUM_SCALAR >
  struct Maximizes {
    // ###########################################################################
    /// @name Operator()
    // ###########################################################################
    /// @{

    GUM_SCALAR operator()(const GUM_SCALAR& x, const GUM_SCALAR& y) const { return x >= y ? x : y; }

    /// @}

    using first_argument_type  = GUM_SCALAR;
    using second_argument_type = GUM_SCALAR;
    using result_type          = GUM_SCALAR;
  };

  /**
   * @struct Minimizes functors.h <agrum/tools/core/functors.h>
   * @brief Minimization function object class
   * @ingroup core
   *
   * Returns the minimum of its two arguments
   */
  template < class GUM_SCALAR >
  struct Minimizes {
    // ###########################################################################
    /// @name Operator()
    // ###########################################################################
    /// @{

    GUM_SCALAR operator()(const GUM_SCALAR& x, const GUM_SCALAR& y) const { return x <= y ? x : y; }

    /// @}

    using first_argument_type  = GUM_SCALAR;
    using second_argument_type = GUM_SCALAR;
    using result_type          = GUM_SCALAR;
  };

  /**
   * @struct ArgumentMaximises  functors.h <agrum/tools/core/functors.h>
   * @brief Arg Max function object class
   * @ingroup core
   *
   * @param Operator() takes two std::pairs.
   * First element in each pair is the values we compare to do the argmax.
   * Second element is the argument that leads to this value.
   *
   * @return best pair => the argument that is the arg max is ret.second
   */

  template < class GUM_SCALAR >
  struct ArgumentMaximises {
    // ###########################################################################
    /// @name Operator()
    // ###########################################################################
    /// @{

    GUM_SCALAR operator()(const GUM_SCALAR& x, const GUM_SCALAR& y) const {
      return x.first >= y.first ? x : y;
    }

    /// @}

    using first_argument_type  = GUM_SCALAR;
    using second_argument_type = GUM_SCALAR;
    using result_type          = GUM_SCALAR;
  };
}   // namespace gum

#endif   // GUM_FUNCTORS_H
