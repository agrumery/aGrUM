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
 * @brief the pattern used by all the "basename" projections of multidim tables
 * over all their variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// check if we allowed these patterns to be used
#ifndef GUM_COMPLETE_PROJECTION_PATTERN_ALLOWED

// #warning To use completeProjectionPattern, you must define
// GUM_COMPLETE_PROJECTION_PATTERN_ALLOWED

#else
namespace gum {

  // a specialized function for projecting a multiDimImplementation over a subset
  // of its variables

#  ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME
#    define GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE GUM_SCALAR

  template < typename GUM_SCALAR >
  GUM_SCALAR
      GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME(const MultiDimImplementation< GUM_SCALAR >* table,
                                             Instantiation* instantiation = 0)
#  endif

  // clang-format off

#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE GUM_SCALAR*
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
  template <typename GUM_SCALAR>
  GUM_SCALAR* GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME(
      const MultiDimImplementation<GUM_SCALAR*>* table,
      Instantiation* instantiation = 0 )
#endif

#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME_F
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE GUM_SCALAR
  template <typename GUM_SCALAR>
  GUM_SCALAR GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME_F(
      const MultiDimImplementation<GUM_SCALAR>* table,
      GUM_SCALAR ( *f )( const GUM_SCALAR&, const GUM_SCALAR& ),
      Instantiation* instantiation = 0 )
#endif

#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME_F
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE GUM_SCALAR*
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
  template <typename GUM_SCALAR>
  GUM_SCALAR* GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME_F(
      const MultiDimImplementation<GUM_SCALAR*>* table,
      GUM_SCALAR* ( *f )(const GUM_SCALAR const*,
        const GUM_SCALAR const*))
#endif

  // clang-format on

  {

#  ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME_F
    Instantiation* instantiation = 0;
#  endif   // GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME_F

    // first, compute the domain size of the table:
    Size nb_elt = table->domainSize();

    // now, parse the table and compute the projection. Start with the
    // neutral element
    // TODO: change into Instantiation table_inst(table); when Tensors will support
    // thread-safe creations of Instantiations
    Instantiation table_inst;
    for (const auto var: table->variablesSequence())
      table_inst.add(*var);
    GUM_SCALAR current_val = GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL;

    if (instantiation) {
      GUM_SCALAR old_val     = current_val;
      Size       current_ptr = 0;

      for (Size i = 0; i < nb_elt; ++i, ++table_inst) {
#  ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
        GUM_MULTI_DIM_COMPLETE_PROJECTION(&current_val, table->get(table_inst));
#  else
        GUM_MULTI_DIM_COMPLETE_PROJECTION(current_val, table->get(table_inst));
#  endif   // GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER

        if (current_val != old_val) {
          old_val     = current_val;
          current_ptr = i;
        }
      }

      // put the table's variables into the instantiation
      instantiation->forgetMaster();
      instantiation->clear();
      const Sequence< const DiscreteVariable* >& table_vars = table->variablesSequence();

      for (const auto var: table_vars)
        instantiation->add(*var);

      // return the result
      instantiation->operator+=(current_ptr);
      return table->get(*instantiation);
    } else {
#  ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
      GUM_SCALAR old_val     = current_val;
      Size       current_ptr = 0;

      for (Size i = 0; i < nb_elt; ++i, ++table_inst) {
        GUM_MULTI_DIM_COMPLETE_PROJECTION(&current_val, table->get(table_inst));

        if (current_val != old_val) {
          old_val     = current_val;
          current_ptr = i;
        }
      }

      table_inst.setFirst();
      table_inst += current_ptr return table->get(table_inst);
#  else

      for (Size i = 0; i < nb_elt; ++i, ++table_inst) {
        GUM_MULTI_DIM_COMPLETE_PROJECTION(current_val, table->get(table_inst));
      }

      return current_val;
#  endif   /* GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER */
    }
  }

#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE

#  ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
#    undef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
#  endif

} /* End of namespace gum */

#endif /* GUM_COMPLETE_PROJECTION_PATTERN_ALLOWED */
