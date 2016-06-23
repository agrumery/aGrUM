/*  *************************************************************************
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
/**
 * @file
 * @brief the pattern used by all binary MultiDimFunctionGraphs operators
 *
 * @author Jean-Christophe MAGNAN
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

// check if we allowed these patterns to be used
#ifndef GUM_OPERATOR_PATTERN_ALLOWED

#warning To use operatorPattern4MultiDimFunctionGraph.h, you must define GUM_OPERATOR_PATTERN_ALLOWED

#else

#include <agrum/core/functors.h>

#include <agrum/multidim/FunctionGraphUtilities/operators/multiDimFunctionGraphOperator.h>
#include <agrum/multidim/multiDimFunctionGraph.h>
#include <agrum/multidim/multiDimImplementation.h>

namespace gum {

/**
 * This specialized function combines two MultiDimFunctionGraphs.  Any
 * operation (addition, subtraction, multiplication, ...) must call this
 * methods.
 **/

#ifdef GUM_MULTI_DIM_OPERATOR_NAME
#define GUM_MULTI_DIM_OPERATOR_TYPE GUM_SCALAR
  template <typename GUM_SCALAR>
  MultiDimFunctionGraph<GUM_SCALAR>*
  GUM_MULTI_DIM_OPERATOR_NAME( const MultiDimFunctionGraph<GUM_SCALAR>* dg1,
                               const MultiDimFunctionGraph<GUM_SCALAR>* dg2 )
#endif

// clang-format off

#ifdef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME
#define GUM_MULTI_DIM_OPERATOR_TYPE GUM_SCALAR
  template <typename GUM_SCALAR>
  MultiDimImplementation<GUM_SCALAR>*
  GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME(
      const MultiDimImplementation<GUM_SCALAR>* tt1,
      const MultiDimImplementation<GUM_SCALAR>* tt2 ) 
#endif

  // clang-format on

  {

#ifdef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME
    const MultiDimFunctionGraph<GUM_SCALAR>* dg1 =
        reinterpret_cast<const MultiDimFunctionGraph<GUM_SCALAR>*>( tt1 );
    const MultiDimFunctionGraph<GUM_SCALAR>* dg2 =
        reinterpret_cast<const MultiDimFunctionGraph<GUM_SCALAR>*>( tt2 );
#endif
    MultiDimFunctionGraphOperator<GUM_MULTI_DIM_OPERATOR_TYPE,
                                  GUM_MULTI_DIM_OPERATOR>
        ope( dg1, dg2 );

    return ope.compute();
  }
#undef GUM_MULTI_DIM_OPERATOR_TYPE

}  // End of namespace gum
#endif /* GUM_OPERATOR_PATTERN_ALLOWED */
