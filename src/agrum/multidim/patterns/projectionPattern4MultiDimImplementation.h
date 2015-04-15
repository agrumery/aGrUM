/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
/** @file
 * @brief the pattern used by all the MultiDimImplementation projections
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN */

// check if we allowed these patterns to be used
#ifndef GUM_PROJECTION_PATTERN_ALLOWED

#warning To use projectionPattern4MultiDimImplementation.h, you must define GUM_PROJECTION_PATTERN_ALLOWED

#else

namespace gum {

#ifdef GUM_MULTI_DIM_PROJECTION_NAME
  template <typename GUM_SCALAR>
  MultiDimImplementation<GUM_SCALAR> *
  GUM_MULTI_DIM_PROJECTION_NAME(const MultiDimImplementation<GUM_SCALAR> &ttable,
                                const Set<const DiscreteVariable *> &del_vars) {
#endif

    typename ProjectionRegister4MultiDim<GUM_SCALAR>::ProjectionPtr func;

    // get the appropriate function to perform the operation
    try {
      // try to find func(ttable,del_vars) in the register
      func = ProjectionRegister4MultiDim<GUM_SCALAR>::Register().get(
          GUM_MULTI_DIM_PROJECTION_FUNC_NAME, ttable.name());
    } catch (NotFound &) {
      func = ProjectionRegister4MultiDim<GUM_SCALAR>::Register().get(
          GUM_MULTI_DIM_PROJECTION_FUNC_NAME, ttable.basename());
    }

    // perform the projection
    return func(&ttable, del_vars);
  }
} /* end of namespace GUM */
#endif /* GUM_PROJECTION_PATTERN_ALLOWED */
