
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
#include <agrum/core/argMaxSet.h>
// =========================================================================


namespace gum {

  /**
   * @struct Maximizes functors.h <agrum/core/functors.h>
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

    GUM_SCALAR operator()(const GUM_SCALAR& x, const GUM_SCALAR& y) const {
      return x >= y ? x : y;
    }

    /// @}

    typedef GUM_SCALAR first_argument_type;
    typedef GUM_SCALAR second_argument_type;
    typedef GUM_SCALAR result_type;
  };

  /**
   * @struct Minimizes functors.h <agrum/core/functors.h>
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

    GUM_SCALAR operator()(const GUM_SCALAR& x, const GUM_SCALAR& y) const {
      return x <= y ? x : y;
    }

    /// @}

    typedef GUM_SCALAR first_argument_type;
    typedef GUM_SCALAR second_argument_type;
    typedef GUM_SCALAR result_type;
  };

  /**
   * @struct ArgumentMaximises  functors.h <agrum/core/functors.h>
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

    typedef GUM_SCALAR first_argument_type;
    typedef GUM_SCALAR second_argument_type;
    typedef GUM_SCALAR result_type;
  };
}   // namespace gum

#endif   // GUM_FUNCTORS_H
