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
 *
 * @brief the pattern used by all the projections of multidimensional tables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// check if we allowed these patterns to be used
#ifndef GUM_PROJECTION_PATTERN_ALLOWED

// #warning To use projectionPattern, you must define
// GUM_PROJECTION_PATTERN_ALLOWED

#else
namespace gum {

  // a specialized function for projecting a multiDimArray over a subset of its
  // vars

#  ifdef GUM_MULTI_DIM_PROJECTION_NAME
#    define GUM_MULTI_DIM_PROJECTION_TYPE GUM_SCALAR

  template < typename GUM_SCALAR >
  MultiDimArray< GUM_SCALAR >* GUM_MULTI_DIM_PROJECTION_NAME(
      const MultiDimArray< GUM_SCALAR >* table,
      const gum::VariableSet&            del_vars)
#  endif

  // clang-format off

#ifdef GUM_MULTI_DIM_PROJECTION_POINTER_NAME
#define GUM_MULTI_DIM_PROJECTION_TYPE GUM_SCALAR *
#define GUM_MULTI_DIM_PROJECTION_POINTER
  template <typename GUM_SCALAR>
  MultiDimArray<GUM_SCALAR*>* GUM_MULTI_DIM_PROJECTION_POINTER_NAME(
      const MultiDimArray<GUM_SCALAR*>* table,
      const VariableSet& del_vars )
#endif

#ifdef GUM_MULTI_DIM_PROJECTION_NAME_F
#define GUM_MULTI_DIM_PROJECTION_TYPE GUM_SCALAR
  template <typename GUM_SCALAR>
  MultiDimArray<GUM_SCALAR>* GUM_MULTI_DIM_PROJECTION_NAME_F(
      const MultiDimArray<GUM_SCALAR>* table,
      const VariableSet& del_vars,
      GUM_SCALAR ( *f )( const GUM_SCALAR&, const GUM_SCALAR& ) )
#endif

#ifdef GUM_MULTI_DIM_PROJECTION_POINTER_NAME_F
#define GUM_MULTI_DIM_PROJECTION_TYPE GUM_SCALAR *
#define GUM_MULTI_DIM_PROJECTION_POINTER
  template <typename GUM_SCALAR>
  MultiDimArray<GUM_SCALAR*>* GUM_MULTI_DIM_PROJECTION_POINTER_NAME_F(
      const MultiDimArray<GUM_SCALAR*>* table,
      const VariableSet& del_vars,
      GUM_SCALAR* ( *f )( const GUM_SCALAR const*,
                          const GUM_SCALAR const* ) )
#endif

#ifdef GUM_MULTI_DIM_PROJECTION_IMPL2ARRAY_NAME
#define GUM_MULTI_DIM_PROJECTION_TYPE GUM_SCALAR
  template <typename GUM_SCALAR>
  MultiDimImplementation<GUM_SCALAR>*
  GUM_MULTI_DIM_PROJECTION_IMPL2ARRAY_NAME(
      const MultiDimImplementation<GUM_SCALAR>* ttable,
      const VariableSet& del_vars )
#endif

#ifdef GUM_MULTI_DIM_PROJECTION_POINTER_IMPL2ARRAY_NAME
#define GUM_MULTI_DIM_PROJECTION_TYPE GUM_SCALAR *
#define GUM_MULTI_DIM_PROJECTION_POINTER
            template <typename GUM_SCALAR>
            MultiDimImplementation<GUM_SCALAR*>*
            GUM_MULTI_DIM_PROJECTION_POINTER_IMPL2ARRAY_NAME(
                const MultiDimImplementation<GUM_SCALAR*>* ttable,
                const VariableSet& del_vars )
#endif

  // clang-format on

  {

#  ifdef GUM_MULTI_DIM_PROJECTION_IMPL2ARRAY_NAME
    const MultiDimArray< GUM_SCALAR >* table
        = reinterpret_cast< const MultiDimArray< GUM_SCALAR >* >(ttable);
#  endif

#  ifdef GUM_MULTI_DIM_PROJECTION_POINTER_IMPL2ARRAY_NAME
    const MultiDimArray< GUM_SCALAR* >* table
        = reinterpret_cast< const MultiDimArray< GUM_SCALAR* >* >(ttable);
#  endif

    // create the neutral element used to fill the result upon its
    // creation
    const GUM_SCALAR neutral_element = GUM_MULTI_DIM_PROJECTION_NEUTRAL;

    // first, compute whether we should loop over table or over the projected
    // table first to get a faster algorithm.
    const Sequence< const DiscreteVariable* >& table_vars = table->variablesSequence();
    bool need_swapping = table_vars.size() >= 2 * del_vars.size();

    if (!need_swapping) {
      // Compute the variables that belong to both the projection set and
      // table.  Store the domain size of the Cartesian product of these
      // variables (result_domain_size) as well as the domain size of the
      // Cartesian product of the variables of table that do not belong to
      // projection set, i.e., those variables that belong to table but not to
      // del_vars (table_alone_domain_size).  In addition, store the number of
      // increments in the computation loops at the end of the function before
      // which the variables of the projection set need be incremented (vector
      // before incr).
      std::vector< Idx >                  table_and_result_offset;
      std::vector< Idx >                  table_and_result_domain;
      std::vector< Idx >                  before_incr;
      unsigned int                        nb_positive_before_incr = 0;
      Idx                                 table_alone_domain_size = 1;
      Idx                                 result_domain_size      = 1;
      Idx                                 table_domain_size       = 1;
      Sequence< const DiscreteVariable* > result_varSeq;
      {
        Idx  tmp_before_incr = 1;
        bool has_before_incr = false;

        for (const auto var: table_vars) {
          table_domain_size *= var->domainSize();

          if (!del_vars.exists(var)) {
            if (has_before_incr) {
              before_incr.push_back(tmp_before_incr - 1);
              has_before_incr = false;
              ++nb_positive_before_incr;
            } else {
              before_incr.push_back(0);
            }

            table_and_result_domain.push_back(var->domainSize());
            table_and_result_offset.push_back(result_domain_size);
            result_domain_size *= var->domainSize();
            tmp_before_incr = 1;
            result_varSeq << var;
          } else {
            tmp_before_incr *= var->domainSize();
            has_before_incr = true;
            table_alone_domain_size *= var->domainSize();
          }
        }
      }
      std::vector< Idx > table_and_result_value = table_and_result_domain;
      std::vector< Idx > current_incr           = before_incr;
      std::vector< Idx > table_and_result_down  = table_and_result_offset;

      for (unsigned int i = 0; i < table_and_result_down.size(); ++i) {
        table_and_result_down[i] *= (table_and_result_domain[i] - 1);
      }

      // create a table "result" containing only the variables of the
      // projection: the variables are stored in the order in which they appear
      // in "table" Hence, ++ operations on an instantiation on table will more
      // or less correspond to a ++ operation on an instantiation on result
      MultiDimArray< GUM_MULTI_DIM_PROJECTION_TYPE >* result
          = new MultiDimArray< GUM_MULTI_DIM_PROJECTION_TYPE >;

      if (!result_varSeq.size()) { return result; }

      result->beginMultipleChanges();

      for (const auto var: result_varSeq)
        *result << *var;

// fill the matrix with the neutral element
#  ifdef GUM_MULTI_DIM_PROJECTION_POINTER
      result->endMultipleChanges();

      for (Idx i = 0; i < result_domain_size; ++i) {
        result->unsafeSet(i, new GUM_SCALAR(neutral_element));
      }

#  else
      result->endMultipleChanges(neutral_element);
#  endif

      // compute the projection: first loop over the variables X's in table
      // that do not belong to result and, for each value of these X's, loop
      // over the variables in both table and result. As such, in the internal
      // loop, the offsets of "result" need only be incremented as usually to
      // parse appropriately this table. For result, the problem is slightly
      // more complicated: in the outer for loop, we shall always reset
      // resul_offset to 0.  For the inner loop, result_offset should be
      // incremented (++) only when t1 before_incr[xxx] steps in the loop have
      // already been made.

      // but before doing so, check whether there exist positive_before_incr.
      // If this is not the case, optimize by not using before_incr at all
      GUM_MULTI_DIM_PROJECTION_TYPE* pt
          = const_cast< GUM_MULTI_DIM_PROJECTION_TYPE* >(&(table->unsafeGet(0)));
      GUM_MULTI_DIM_PROJECTION_TYPE* pres
          = const_cast< GUM_MULTI_DIM_PROJECTION_TYPE* >(&(result->unsafeGet(0)));
      GUM_MULTI_DIM_PROJECTION_TYPE* pres_deb = pres;

      if (!nb_positive_before_incr) {
        for (Idx i = 0; i < table_alone_domain_size; ++i) {
          for (Idx j = 0; j < result_domain_size; ++j) {
#  ifdef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
            GUM_MULTI_DIM_PROJECTION(*pres, *pt);
#  else
#    ifdef GUM_MULTI_DIM_PROJECTION_POINTER
            **pres = GUM_MULTI_DIM_PROJECTION(*pres, *pt);
#    else
            *pres = GUM_MULTI_DIM_PROJECTION(*pres, *pt);
#    endif
#  endif

            // update the offset of table and result
            ++pt;
            ++pres;
          }

          // update the offset of result
          pres = pres_deb;
        }
      } else {
        // here there are positive before_incr and we should use them to know
        // when result_offset needs be changed
        Idx result_offset = 0;

        for (Idx i = 0; i < table_domain_size; ++i) {
#  ifdef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
          GUM_MULTI_DIM_PROJECTION(pres[result_offset], *pt);
#  else
#    ifdef GUM_MULTI_DIM_PROJECTION_POINTER
          *(pres[result_offset]) = GUM_MULTI_DIM_PROJECTION(pres[result_offset], *pt);
#    else
          pres[result_offset] = GUM_MULTI_DIM_PROJECTION(pres[result_offset], *pt);
#    endif
#  endif

          // update the offset of table
          ++pt;

          // update the offset of result
          for (unsigned int k = 0; k < current_incr.size(); ++k) {
            // check if we need modify result_offset
            if (current_incr[k]) {
              --current_incr[k];
              break;
            }

            current_incr[k] = before_incr[k];

            // here we shall modify result_offset
            --table_and_result_value[k];

            if (table_and_result_value[k]) {
              result_offset += table_and_result_offset[k];
              break;
            }

            table_and_result_value[k] = table_and_result_domain[k];
            result_offset -= table_and_result_down[k];
          }
        }
      }

      return result;
    } else {   // need_swapping = true

      // Compute the variables that are in t1 but not in t2. For these
      // variables, get the increment in the offset of t1 that would result
      // from an increment in one of these variables (vector t1_alone_offset).
      // Also store the domain size of these variables (vector t1_alone_domain)
      // and, for the computation loops at the end of this function, |variable|
      // - the current values of these variables (vector t1_alone_value). All
      // these vectors reference the variables of t1 \ t2 in the order in which
      // they appear in seq1. Keep as well the size of the Cartesian product of
      // these variables.
      std::vector< Idx >                        table_alone_offset;
      std::vector< Idx >                        table_alone_domain;
      Idx                                       offset                  = 1;
      Idx                                       table_alone_domain_size = 1;
      HashTable< const DiscreteVariable*, Idx > var1offset(table_vars.size());

      for (const auto var: table_vars) {
        if (del_vars.exists(var)) {
          table_alone_domain.push_back(var->domainSize());
          table_alone_offset.push_back(offset);
          table_alone_domain_size *= var->domainSize();
        }

        var1offset.insert(var, offset);
        offset *= var->domainSize();
      }

      std::vector< Idx > table_alone_value = table_alone_domain;
      std::vector< Idx > table_alone_down  = table_alone_offset;

      for (unsigned int i = 0; i < table_alone_down.size(); ++i)
        table_alone_down[i] *= (table_alone_domain[i] - 1);

      // Compute the same vectors for the variables that belong to both t1 and
      // t2.  In this case, All these vectors reference the variables in the
      // order in which they appear in seq2. In addition, store the number of
      // increments in the computation loops at the end of the function before
      // which the variables of t1 cap t2 need be incremented (vector
      // t1_and_t2_before incr).  Similarly, store the number of such
      // increments currently still needed before the next incrementation of
      // the variables of t1 cap t2. Keep as well the size of the Cartesian
      // product of these variables.
      Sequence< const DiscreteVariable* > result_varSeq;
      std::vector< Idx >                  table_and_result_offset;
      std::vector< Idx >                  table_and_result_domain;
      Idx                                 result_domain_size = 1;
      bool                                has_before_incr    = false;
      bool                                found_proj_var     = false;

      for (const auto var: table_vars) {
        if (!del_vars.exists(var)) {
          table_and_result_domain.push_back(var->domainSize());
          table_and_result_offset.push_back(var1offset[var]);
          found_proj_var = true;
          result_domain_size *= var->domainSize();
          result_varSeq << var;
        } else {
          if (found_proj_var) has_before_incr = true;
        }
      }

      std::vector< Idx > table_and_result_value = table_and_result_domain;
      std::vector< Idx > table_and_result_down  = table_and_result_offset;

      for (unsigned int i = 0; i < table_and_result_down.size(); ++i) {
        table_and_result_down[i] *= (table_and_result_domain[i] - 1);
      }

      // create a table "result" containing only the variables of the
      // projection: the variables are stored in the order in which they appear
      // in "table" Hence, ++ operations on an instantiation on table will more
      // or less correspond to a ++ operation on an instantiation on result
      MultiDimArray< GUM_MULTI_DIM_PROJECTION_TYPE >* result
          = new MultiDimArray< GUM_MULTI_DIM_PROJECTION_TYPE >;
      result->beginMultipleChanges();

      for (const auto var: result_varSeq)
        *result << *var;

#  ifdef GUM_MULTI_DIM_PROJECTION_POINTER
      result->endMultipleChanges();

      // fill the matrix with the neutral element
      for (Idx i = 0; i < result_domain_size; ++i) {
        result->unsafeSet(i, new GUM_SCALAR(neutral_element));
      }

#  else
      result->endMultipleChanges(neutral_element);
#  endif

      // compute the sum: first loop over the variables X's both in table and
      // in result and, for each value of these X's, loop over the variables
      // that are in table but not result. As such, in the internal loop, there
      // is no increment in the offset of "result", and in the outer loop, this
      // offset is incremented using a simple ++ operator.  For table, the
      // problem is slightly more complicated: in the outer for loop, we shall
      // increment the variables of table cap result according to vectors
      // table_and_result_xxx. Each time a variable of these vectors has been
      // incremented up to its max, we shall put it down to 0 and increment the
      // next one, and so on. For the inner loop, this is similar except that
      // we shall do these operations only when before_incr[xxx] steps in the
      // loop have already been made.
      GUM_MULTI_DIM_PROJECTION_TYPE* pt
          = const_cast< GUM_MULTI_DIM_PROJECTION_TYPE* >(&(table->unsafeGet(0)));
      GUM_MULTI_DIM_PROJECTION_TYPE* pres
          = const_cast< GUM_MULTI_DIM_PROJECTION_TYPE* >(&(result->unsafeGet(0)));

      // but before doing so, check whether there exist positive_before_incr.
      // If this is not the case, optimize by not using before_incr at all
      if (!has_before_incr) {
        for (Idx i = 0; i < result_domain_size; ++i) {
          for (Idx j = 0; j < table_alone_domain_size; ++j) {
#  ifdef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
            GUM_MULTI_DIM_PROJECTION(*pres, *pt);
#  else
#    ifdef GUM_MULTI_DIM_PROJECTION_POINTER
            **pres = GUM_MULTI_DIM_PROJECTION(*pres, *pt);
#    else
            *pres = GUM_MULTI_DIM_PROJECTION(*pres, *pt);
#    endif
#  endif

            // update the offset of table
            ++pt;
          }

          // update the offset of result
          ++pres;
        }
      } else {
        // here there are positive before_incr and we should use them to know
        // when result_offset needs be changed
        Idx table_offset = 0;

        for (Idx j = 0; j < result_domain_size; ++j) {
          for (Idx i = 0; i < table_alone_domain_size; ++i) {
#  ifdef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
            GUM_MULTI_DIM_PROJECTION(*pres, pt[table_offset]);
#  else
#    ifdef GUM_MULTI_DIM_PROJECTION_POINTER
            **pres = GUM_MULTI_DIM_PROJECTION(*pres, pt[table_offset]);
#    else
            *pres = GUM_MULTI_DIM_PROJECTION(*pres, pt[table_offset]);
#    endif
#  endif

            // update the increment of table for the inner loop
            for (unsigned int k = 0; k < table_alone_value.size(); ++k) {
              --table_alone_value[k];

              if (table_alone_value[k]) {
                table_offset += table_alone_offset[k];
                break;
              }

              table_alone_value[k] = table_alone_domain[k];
              table_offset -= table_alone_down[k];
            }
          }

          // update the offset of table for the outer loop
          for (unsigned int k = 0; k < table_and_result_value.size(); ++k) {
            --table_and_result_value[k];

            if (table_and_result_value[k]) {
              table_offset += table_and_result_offset[k];
              break;
            }

            table_and_result_value[k] = table_and_result_domain[k];
            table_offset -= table_and_result_down[k];
          }

          // update the offset of result for the outer loop
          //++result_offset;
          ++pres;
        }
      }

      return result;
    }
  }

#  undef GUM_MULTI_DIM_PROJECTION_TYPE

#  ifdef GUM_MULTI_DIM_PROJECTION_POINTER
#    undef GUM_MULTI_DIM_PROJECTION_POINTER
#  endif
}   // namespace gum
#endif /* GUM_PROJECTION_PATTERN_ALLOWED */
