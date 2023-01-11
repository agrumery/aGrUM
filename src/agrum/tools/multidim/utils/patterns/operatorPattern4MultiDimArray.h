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
 * @brief the pattern used by all binary MultiDimArrays operators
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// check if we allowed these patterns to be used
#ifndef GUM_OPERATOR_PATTERN_ALLOWED

// #warning To use operatorPattern4MultiDimArray.h, you must define
// GUM_OPERATOR_PATTERN_ALLOWED

#else

namespace gum {

  // a specialized function for combining two multiDimArrays

#  ifdef GUM_MULTI_DIM_OPERATOR_NAME
#    define GUM_MULTI_DIM_OPERATOR_TYPE T
  template < typename T >
  MultiDimArray< T >* GUM_MULTI_DIM_OPERATOR_NAME(const MultiDimArray< T >* t1,
                                                  const MultiDimArray< T >* t2)
#  endif

  // clang-format off

#ifdef GUM_MULTI_DIM_OPERATOR_POINTER_NAME
#define GUM_MULTI_DIM_OPERATOR_TYPE T *
  template <typename T>
  MultiDimArray<T*>* GUM_MULTI_DIM_OPERATOR_POINTER_NAME(
      const MultiDimArray<T*>* t1, const MultiDimArray<T*>* t2 )
#endif

#ifdef GUM_MULTI_DIM_OPERATOR_NAME_F
#define GUM_MULTI_DIM_OPERATOR_TYPE T
  template <typename T>
  MultiDimArray<T>* GUM_MULTI_DIM_OPERATOR_NAME_F(
      const MultiDimArray<T>* t1,
      const MultiDimArray<T>* t2,
      const T ( *f )( const T&, const T& ) )
#endif

#ifdef GUM_MULTI_DIM_OPERATOR_POINTER_NAME_F
#define GUM_MULTI_DIM_OPERATOR_TYPE T *
  template <typename T>
  MultiDimArray<T*>* GUM_MULTI_DIM_OPERATOR_POINTER_NAME_F(
      const MultiDimArray<T*>* t1,
      const MultiDimArray<T*>* t2,
      const T* ( *f )( const T*, const T* ) )
#endif

#ifdef GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME
#define GUM_MULTI_DIM_OPERATOR_TYPE T
  template <typename T>
  MultiDimImplementation<T>* GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME(
      const MultiDimImplementation<T>* tt1,
      const MultiDimImplementation<T>* tt2 )
#endif

#ifdef GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME
#define GUM_MULTI_DIM_OPERATOR_TYPE T *
  template <typename T>
  MultiDimImplementation<T*>*
  GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME(
      const MultiDimImplementation<T*>* tt1,
      const MultiDimImplementation<T*>* tt2 )
#endif

  // clang-format on

  {

#  ifdef GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME
    const MultiDimArray< T >*  t1 = reinterpret_cast< const MultiDimArray< T >* >(tt1);
    const MultiDimArray< T >*  t2 = reinterpret_cast< const MultiDimArray< T >* >(tt2);
#  endif

#  ifdef GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME
    const MultiDimArray< T* >* t1 = reinterpret_cast< const MultiDimArray< T* >* >(tt1);
    const MultiDimArray< T* >* t2 = reinterpret_cast< const MultiDimArray< T* >* >(tt2);
#  endif

    // get the variables of the tables
    const Sequence< const DiscreteVariable* >& t1_vars = t1->variablesSequence();
    const Sequence< const DiscreteVariable* >& t2_vars = t2->variablesSequence();

    // get the domain size of the tables' variables
    HashTable< const DiscreteVariable*, Idx > t1_offsets;
    {
      Idx current_offset = 1;

      for (const auto var: t1_vars) {
        t1_offsets.insert(var, current_offset);
        current_offset *= var->domainSize();
      }
    }
    HashTable< const DiscreteVariable*, Idx > t2_offsets;
    {
      Idx current_offset = 1;

      for (const auto var: t2_vars) {
        t2_offsets.insert(var, current_offset);
        current_offset *= var->domainSize();
      }
    }

    // we divide the variables of t1 and t2 into 3 separate sets: those that
    // belong only to t1 (variables t1_alone_xxx), those that belong only to t2
    // (variables t2_alone_xxx) and those that belong to both tables (variables
    // t1_and_t2_xxx). For each set, we define how an increment of a given
    // variable of the set changes the offset in the corresponding table
    // (variable txxx_offset) and what is the domain size of the variable
    // (txxx_domain). In addition, we compute the domain size of the Cartesian
    // product of the variables in each of the 3 sets. Given these data, we
    // will be able to parse both t1, t2 and the result table t1+t2 without
    // resorting to instantiations.
    std::vector< Idx > t1_alone_offset;
    std::vector< Idx > t1_alone_domain;
    Idx                t1_alone_domain_size = 1;

    std::vector< Idx > t2_alone_offset;
    std::vector< Idx > t2_alone_domain;
    Idx                t2_alone_domain_size = 1;

    std::vector< Idx >                     t1_and_t2_1_offset;
    std::vector< Idx >                     t1_and_t2_2_offset;
    std::vector< Idx >                     t1_and_t2_domain;
    std::vector< const DiscreteVariable* > t1_and_t2_vars;
    Idx                                    t1_and_t2_domain_size = 1;

    {
      for (const auto var: t1_vars)
        if (t2_vars.exists(var)) {
          t1_and_t2_domain.push_back(var->domainSize());
          t1_and_t2_1_offset.push_back(t1_offsets[var]);
          t1_and_t2_2_offset.push_back(t2_offsets[var]);
          t1_and_t2_vars.push_back(var);
          t1_and_t2_domain_size *= var->domainSize();
        } else {
          t1_alone_domain.push_back(var->domainSize());
          t1_alone_offset.push_back(t1_offsets[var]);
          t1_alone_domain_size *= var->domainSize();
        }

      for (const auto var: t2_vars)
        if (!t1_vars.exists(var)) {
          t2_alone_domain.push_back(var->domainSize());
          t2_alone_offset.push_back(t2_offsets[var]);
          t2_alone_domain_size *= var->domainSize();
        }
    }

    // a Boolean indicating whether the variables that t1 and t2 have in common
    // are the first variables and are in the same order. When this is true,
    // computations can be performed faster
    bool t1_and_t2_begin_vars = false;

    if (t1_and_t2_vars.size()) {
      unsigned int nb_t1_t2_vars = 0;

      for (auto iter = t1_vars.begin(); nb_t1_t2_vars != t1_and_t2_vars.size();
           ++iter, ++nb_t1_t2_vars)
        if (*iter != t1_and_t2_vars[nb_t1_t2_vars]) break;

      if (nb_t1_t2_vars == t1_and_t2_vars.size()) {
        nb_t1_t2_vars = 0;

        for (auto iter = t2_vars.begin(); nb_t1_t2_vars != t1_and_t2_vars.size();
             ++iter, ++nb_t1_t2_vars)
          if (*iter != t1_and_t2_vars[nb_t1_t2_vars]) break;

        if (nb_t1_t2_vars == t1_and_t2_vars.size()) { t1_and_t2_begin_vars = true; }
      }
    }

    // when we will parse t1 and t2 to fill the result table t1+t2, we will use
    // variables txxx_value : at the beginning they are initialized to the
    // domain size of the variables (which are, themselves initialized to 0).
    // Each time we increment a variable (that is, we increase the offset of
    // the table by txxx_offset), its corresponding txxx_value is decreased by
    // 1.  When the latter is equal to 0, this means that the variable itself
    // should be reinitialized to 0 as well and that the next variable of the
    // table should be increased (that is, this is similar to increasing 9 to
    // 10). As such the offset of txxx should be decreased by txxx_offset * the
    // domain size of the variable.  This quantity is precisely what is stored
    // into variables txxx_down.
    std::vector< Idx > t1_and_t2_value  = t1_and_t2_domain;
    std::vector< Idx > t1_and_t2_1_down = t1_and_t2_1_offset;
    std::vector< Idx > t1_and_t2_2_down = t1_and_t2_2_offset;

    for (unsigned int i = 0; i < t1_and_t2_domain.size(); ++i) {
      t1_and_t2_1_down[i] *= (t1_and_t2_domain[i] - 1);
      t1_and_t2_2_down[i] *= (t1_and_t2_domain[i] - 1);
    }

    std::vector< Idx > t1_alone_value = t1_alone_domain;
    std::vector< Idx > t1_alone_down  = t1_alone_offset;

    for (unsigned int i = 0; i < t1_alone_domain.size(); ++i) {
      t1_alone_down[i] *= (t1_alone_domain[i] - 1);
    }

    std::vector< Idx > t2_alone_value = t2_alone_domain;
    std::vector< Idx > t2_alone_down  = t2_alone_offset;

    for (unsigned int i = 0; i < t2_alone_domain.size(); ++i) {
      t2_alone_down[i] *= (t2_alone_domain[i] - 1);
    }

    // create a table "result" containing all the variables: the first
    // variables are those that belong to both t1 and t2. The next variables
    // are those that belong to t2 but not to t1. Finally, the last variables
    // are those that belong to t1 but not t2. This order will be used in the
    // next for loops.
    MultiDimArray< GUM_MULTI_DIM_OPERATOR_TYPE >* result
       = new MultiDimArray< GUM_MULTI_DIM_OPERATOR_TYPE >;
    result->beginMultipleChanges();

    for (const auto var: t1_vars)
      if (t2_vars.exists(var)) *result << *var;

    for (const auto var: t2_vars)
      if (!t1_vars.exists(var)) *result << *var;

    for (const auto var: t1_vars)
      if (!t2_vars.exists(var)) *result << *var;

    result->endMultipleChanges();

    // here we fill result. The idea is to use 3 loops. The innermost loop
    // corresponds to the variables that belongs both to t1 and t2.  The middle
    // loop to the variables that belong to t2 but not to t1. Finally, the
    // outer loop corresponds to the variables that belong to t1 but not t2.
    GUM_MULTI_DIM_OPERATOR_TYPE* pt1
       = const_cast< GUM_MULTI_DIM_OPERATOR_TYPE* >(&(t1->unsafeGet(0)));
    GUM_MULTI_DIM_OPERATOR_TYPE* pt2
       = const_cast< GUM_MULTI_DIM_OPERATOR_TYPE* >(&(t2->unsafeGet(0)));
    GUM_MULTI_DIM_OPERATOR_TYPE* pres
       = const_cast< GUM_MULTI_DIM_OPERATOR_TYPE* >(&(result->unsafeGet(0)));
    GUM_MULTI_DIM_OPERATOR_TYPE* pt2_deb = pt2;
    GUM_MULTI_DIM_OPERATOR_TYPE* pt1_alone_begin;

    // test if all the variables in common in t1 and t2 are the first variables
    // and are in the same order. In this case, we can speed-up the
    // incrementation processes
    if (t1_and_t2_begin_vars) {
      for (Idx i = 0; i < t1_alone_domain_size; ++i) {
        pt2             = pt2_deb;
        pt1_alone_begin = pt1;

        for (Idx j = 0; j < t2_alone_domain_size; ++j) {
          pt1 = pt1_alone_begin;

          for (Idx z = 0; z < t1_and_t2_domain_size; ++z) {
            *pres = GUM_MULTI_DIM_OPERATOR(*pt1, *pt2);
            ++pres;

            // update the offset of both t1 and t2
            ++pt1;
            ++pt2;
          }
        }
      }
    } else {
      Idx t1_offset             = 0;
      Idx t2_offset             = 0;
      Idx t1_alone_begin_offset = 0;

      for (Idx i = 0; i < t1_alone_domain_size; ++i) {
        t2_offset             = 0;
        t1_alone_begin_offset = t1_offset;

        for (Idx j = 0; j < t2_alone_domain_size; ++j) {
          t1_offset = t1_alone_begin_offset;

          for (Idx z = 0; z < t1_and_t2_domain_size; ++z) {
            *pres = GUM_MULTI_DIM_OPERATOR(pt1[t1_offset], pt2[t2_offset]);
            ++pres;

            // update the offset of both t1 and t2
            for (unsigned int k = 0; k < t1_and_t2_value.size(); ++k) {
              if (--t1_and_t2_value[k]) {
                t1_offset += t1_and_t2_1_offset[k];
                t2_offset += t1_and_t2_2_offset[k];
                break;
              }

              t1_and_t2_value[k] = t1_and_t2_domain[k];
              t1_offset -= t1_and_t2_1_down[k];
              t2_offset -= t1_and_t2_2_down[k];
            }
          }

          // update the offset of t2 alone
          for (unsigned int k = 0; k < t2_alone_value.size(); ++k) {
            if (--t2_alone_value[k]) {
              t2_offset += t2_alone_offset[k];
              break;
            }

            t2_alone_value[k] = t2_alone_domain[k];
            t2_offset -= t2_alone_down[k];
          }
        }

        // update the offset of t1 alone
        for (unsigned int k = 0; k < t1_alone_value.size(); ++k) {
          if (--t1_alone_value[k]) {
            t1_offset += t1_alone_offset[k];
            break;
          }

          t1_alone_value[k] = t1_alone_domain[k];
          t1_offset -= t1_alone_down[k];
        }
      }
    }

    return result;
  }

#  undef GUM_MULTI_DIM_OPERATOR_TYPE
}   // namespace gum

#endif /* GUM_OPERATOR_PATTERN_ALLOWED */
