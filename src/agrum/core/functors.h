/*********************************************************************************
 *    Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES *
 *   {prenom.nom}_at_lip6.fr *
 *                                                                               *
 *   This program is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation; either version 2 of the License, or *
 *   (at your option) any later version. *
 *                                                                               *
 *   This program is distributed in the hope that it will be useful, *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the *
 *   GNU General Public License for more details. *
 *                                                                               *
 *   You should have received a copy of the GNU General Public License *
 *   along with this program; if not, write to the *
 *   Free Software Foundation, Inc., *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
 ********************************************************************************/
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
   * @struct maximizes functors.h <agrum/multidim/patterns/functors.h>
   * @brief Maximization function object class
   * @ingroup core
   *
   * Returns the maximum of its two arguments
   */
  template <class GUM_SCALAR>
  struct Maximizes {

    // ###########################################################################
    /// @name Operator()
    // ###########################################################################
    /// @{

    GUM_SCALAR operator()( const GUM_SCALAR& x, const GUM_SCALAR& y ) const {
      return x >= y ? x : y;
    }

    /// @}

    typedef GUM_SCALAR first_argument_type;
    typedef GUM_SCALAR second_argument_type;
    typedef GUM_SCALAR result_type;
  };

  /**
   * @struct minimizes functors.h <agrum/multidim/patterns/functors.h>
   * @brief Minimization function object class
   * @ingroup core
   *
   * Returns the minimum of its two arguments
   */
  template <class GUM_SCALAR>
  struct Minimizes {

    // ###########################################################################
    /// @name Operator()
    // ###########################################################################
    /// @{

    GUM_SCALAR operator()( const GUM_SCALAR& x, const GUM_SCALAR& y ) const {
      return x <= y ? x : y;
    }

    /// @}

    typedef GUM_SCALAR first_argument_type;
    typedef GUM_SCALAR second_argument_type;
    typedef GUM_SCALAR result_type;
  };

  /**
   * @struct argmax functors.h <agrum/multidim/patterns/functors.h>
   * @brief Arg Max function object class
   * @ingroup core
   *
   * @param Operator() takes two std::pairs.
   * First element in each pair is the values we compare to do the argmax.
   * Second element is the argument that leads to this value.
   *
   * @return best pair => the argument that is the arg max is ret.second
   */

  template <class GUM_SCALAR>
  struct ArgumentMaximises {

    // ###########################################################################
    /// @name Operator()
    // ###########################################################################
    /// @{

    GUM_SCALAR operator()( const GUM_SCALAR& x, const GUM_SCALAR& y ) const {
      return x.first >= y.first ? x : y;
    }

    /// @}

    typedef GUM_SCALAR first_argument_type;
    typedef GUM_SCALAR second_argument_type;
    typedef GUM_SCALAR result_type;
  };
} /* End of namespace GUM */

#endif  // GUM_FUNCTORS_H
