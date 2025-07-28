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
 * @brief the pattern used by all the partial instantiations of multidimensional
 * tables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/multidim/instantiation.h>

// check if we allowed these patterns to be used
#ifndef GUM_PARTIAL_INSTANTIATION_PATTERN_ALLOWED

// #warning To use partialIntantiationPattern, you must define
// GUM_PARTIAL_INSTANTIATION_PATTERN_ALLOWED

#else
namespace gum {

  // a specialized function instantiating some variables of a table and returning
  // the result

#  ifdef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME
#    define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_TYPE GUM_SCALAR

  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >* GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME(
      const MultiDimImplementation< GUM_SCALAR >*      table,
      const HashTable< const DiscreteVariable*, Idx >& inst_vars)
#  endif

  // clang-format off

#ifdef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER_NAME
#define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_TYPE GUM_SCALAR *
#define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER
  template <typename GUM_SCALAR>
  MultiDimImplementation<GUM_SCALAR*>*
  GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER_NAME(
      const MultiDimImplementation<GUM_SCALAR*>* table,
      const HashTable<const DiscreteVariable*, Idx>& inst_vars )
#endif

  // clang-format on

  {

    // get the variables of the uninstantiated table
    const Sequence< const DiscreteVariable* >& table_vars = table->variablesSequence();

    // Compute the offset of the variables. In addition, get the offset in
    // table induced by the instantiation inst_var
    Idx                                       table_alone_offset = 0;
    Idx                                       offset             = 1;
    HashTable< const DiscreteVariable*, Idx > var1offset(table_vars.size());

    for (const auto var: table_vars) {
      if (inst_vars.exists(var)) { table_alone_offset += inst_vars[var] * offset; }

      var1offset.insert(var, offset);
      offset *= var->domainSize();
    }

    // Compute the sequence of variables in the result table.  Compute as
    // well the offsets and the domain size of the variables that belong to
    // result. Finally, compute has_before_incr: this is a Boolean indicating
    // whether the instantiated variables are the last variables in the
    // variables sequence of table (true) or not (false). If this Boolean is
    // true, then we can fill result by parsing both table and result using
    // only 1-increments.
    Sequence< const DiscreteVariable* > result_varSeq;
    std::vector< Idx >                  table_and_result_offset;
    std::vector< Idx >                  table_and_result_domain;
    Idx                                 result_domain_size = 1;
    bool                                has_before_incr    = true;
    bool                                found_inst_var     = false;

    for (const auto var: table_vars) {
      if (!inst_vars.exists(var)) {
        table_and_result_domain.push_back(var->domainSize());
        table_and_result_offset.push_back(var1offset[var]);
        result_domain_size *= var->domainSize();
        result_varSeq << var;

        if (found_inst_var) has_before_incr = false;
      } else {
        found_inst_var = true;
      }
    }

    // table_and_result_value is a vector indictating, for each
    // uninstantiated variable, how many increments we can still perform on
    // that variable before we must perform a "major" increment: for
    // instance, let A and B be two variables of size 10. Then, if
    // table_and_result_value[A] = 3 and table_and_result_value[B] = 2, this
    // means that the offset they represent is 78 (10^2 - 32). If we still
    // increment B twice, then the offset should be 80, which means that we
    // shall increment A once and decrease B by 10.  The value by which
    // variables shall be decreased is indicated in table_and_result_down
    std::vector< Idx > table_and_result_value = table_and_result_domain;
    std::vector< Idx > table_and_result_down  = table_and_result_offset;

    for (unsigned int i = 0; i < table_and_result_down.size(); ++i)
      table_and_result_down[i] *= (table_and_result_domain[i] - 1);

    // create a table "result" containing only the variables that are not
    // instantiated: the variables are stored in the order in which they
    // appear in "table". Hence, ++ operations on an instantiation on table
    // will more or less correspond to a ++ operation on an instantiation on
    // result
    MultiDimArray< GUM_MULTI_DIM_PARTIAL_INSTANTIATION_TYPE >* result
        = new MultiDimArray< GUM_MULTI_DIM_PARTIAL_INSTANTIATION_TYPE >;
    result->beginMultipleChanges();

    for (const auto var: result_varSeq)
      *result << *var;

    result->endMultipleChanges();

#  ifdef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER
    // fill the matrix with any element
    {
      // TODO: change into Instantiation table_inst(table); when Tensors will support
      // thread-safe creations of Instantiations
      Instantiation table_inst;
      for (const auto var: table->variablesSequence())
        table_inst.add(*var);
      const GUM_SCALAR& any_element = *(table->get(table_inst));

      for (Idx i = 0; i < result_domain_size; ++i) {
        result->unsafeSet(i, new GUM_SCALAR(any_element));
      }
    }
#  endif /* GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER */

    // compute the result: it is now sufficient to loop over the variables
    // that were not instantiated. ptable and presult are pointers on the
    // arrays that are directly used for this loop
    GUM_MULTI_DIM_PARTIAL_INSTANTIATION_TYPE* presult
        = const_cast< GUM_MULTI_DIM_PARTIAL_INSTANTIATION_TYPE* >(&(result->unsafeGet(0)));
    // TODO: change into Instantiation table_inst(table); when Tensors will support
    // thread-safe creations of Instantiations
    Instantiation table_inst;
    for (const auto var: table->variablesSequence())
      table_inst.add(*var);
    table_inst += table_alone_offset;

    // but before doing so, check whether the instantiated variables are the
    // last ones or not. If so, we can optimize the parsing of ptable and
    // presult as both tables need be parsed using only 1-increments
    if (has_before_incr) {
      for (Idx i = 0; i < result_domain_size; ++i) {
#  ifdef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER
        **presult = *(table->get(table_inst));
#  else
        *presult = table->get(table_inst);
#  endif

        // update the offset of result and table
        ++table_inst;
        ++presult;
      }
    } else {
      // here, some uninstantiated variables exist after the instantiated
      // ones in the variables sequence of table. So, we must perform a more
      // complicated parsing of ptable
      for (Idx j = 0; j < result_domain_size; ++j) {
#  ifdef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER
        **presult = *(table->get(table_inst));
#  else
        *presult = table->get(table_inst);
#  endif

        // update the offset of table for the outer loop
        for (unsigned int k = 0; k < table_and_result_value.size(); ++k) {
          --table_and_result_value[k];

          if (table_and_result_value[k]) {
            table_inst += table_and_result_offset[k];
            break;
          }

          table_and_result_value[k] = table_and_result_domain[k];
          table_inst -= table_and_result_down[k];
        }

        // update the offset of result for the outer loop
        ++presult;
      }
    }

    return result;
  }

#  undef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_TYPE

#  ifdef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER
#    undef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER
#  endif

} /* End of namespace gum */

#endif /* GUM_PARTIAL_INSTANTIATION_PATTERN_ALLOWED */
