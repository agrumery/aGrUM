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
 * @brief the pattern used by all "basename" binary operators
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// check if we allowed these patterns to be used
#ifndef GUM_OPERATOR_PATTERN_ALLOWED

// #warning To use operatorPattern4MultiDimBase.h, you must define
// GUM_OPERATOR_PATTERN_ALLOWED

#else

namespace gum {

  // a specialized function for combining two multiDimImplementations whose
  // real data type is unknown to us

#  ifdef GUM_MULTI_DIM_OPERATOR_NAME
#    define GUM_MULTI_DIM_OPERATOR_TYPE T
  template < typename T >
  MultiDimImplementation< T >* GUM_MULTI_DIM_OPERATOR_NAME(const MultiDimImplementation< T >* t1,
                                                           const MultiDimImplementation< T >* t2)
#  endif

  // clang-format off

#ifdef GUM_MULTI_DIM_OPERATOR_POINTER_NAME
#define GUM_MULTI_DIM_OPERATOR_TYPE T *
  template <typename T>
  MultiDimImplementation<T*>* GUM_MULTI_DIM_OPERATOR_POINTER_NAME(
      const MultiDimImplementation<T*>* t1,
      const MultiDimImplementation<T*>* t2 )
#endif

#ifdef GUM_MULTI_DIM_OPERATOR_NAME_F
#define GUM_MULTI_DIM_OPERATOR_TYPE T
  template <typename T>
  MultiDimImplementation<T>* GUM_MULTI_DIM_OPERATOR_NAME_F(
      const MultiDimImplementation<T>* t1,
      const MultiDimImplementation<T>* t2,
      const T ( *f )( const T&, const T& ) )
#endif

#ifdef GUM_MULTI_DIM_OPERATOR_POINTER_NAME_F
#define GUM_MULTI_DIM_OPERATOR_TYPE T *
  template <typename T>
  MultiDimImplementation<T*>* GUM_MULTI_DIM_OPERATOR_POINTER_NAME_F(
      const MultiDimImplementation<T*>* t1,
      const MultiDimImplementation<T*>* t2,
      const T ( *f )( const T*, const T* ) )
#endif

  // clang-format on

  {

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
    // t1_and_t2_xxx). For each set, we get the variables of the table
    // (txxx_var) and the domain size of the variable (txxx_domain). In
    // addition, we compute the domain size of the Cartesian product of the
    // variables in each of the 3 sets. Given these data, we will be able to
    // parse both t1, t2 and the result table t1+t2.
    std::vector< const DiscreteVariable* > t1_alone_var;
    std::vector< Idx >                     t1_alone_domain;
    Idx                                    t1_alone_domain_size = 1;

    std::vector< const DiscreteVariable* > t2_alone_var;
    std::vector< Idx >                     t2_alone_domain;
    Idx                                    t2_alone_domain_size = 1;

    std::vector< const DiscreteVariable* > t1_and_t2_var;
    std::vector< Idx >                     t1_and_t2_domain;
    Idx                                    t1_and_t2_domain_size = 1;

    {
      for (const auto var: t1_vars)
        if (t2_vars.exists(var)) {
          t1_and_t2_domain.push_back(var->domainSize());
          t1_and_t2_var.push_back(var);
          t1_and_t2_domain_size *= var->domainSize();
        } else {
          t1_alone_domain.push_back(var->domainSize());
          t1_alone_var.push_back(var);
          t1_alone_domain_size *= var->domainSize();
        }

      for (const auto var: t2_vars)
        if (!t1_vars.exists(var)) {
          t2_alone_domain.push_back(var->domainSize());
          t2_alone_var.push_back(var);
          t2_alone_domain_size *= var->domainSize();
        }
    }

    // a Boolean indicating whether the variables that t1 and t2 have in common
    // are the first variables and are in the same order. When this is true,
    // computations can be performed faster
    bool t1_and_t2_begin_vars = false;

    if (t1_and_t2_var.size()) {
      unsigned int nb_t1_t2_vars = 0;

      for (const auto var: t1_vars) {
        if (var != t1_and_t2_var[nb_t1_t2_vars]) break;
        nb_t1_t2_vars += 1;
      }

      if (nb_t1_t2_vars == t1_and_t2_var.size()) {
        nb_t1_t2_vars = 0;

        for (auto iter = t2_vars.begin(); nb_t1_t2_vars != t1_and_t2_var.size();
             ++iter, ++nb_t1_t2_vars)
          if (*iter != t1_and_t2_var[nb_t1_t2_vars]) break;

        if (nb_t1_t2_vars == t1_and_t2_var.size()) t1_and_t2_begin_vars = true;
      }
    }

    // when we will parse t1 and t2 to fill the result table t1+t2, we will use
    // variables txxx_value : at the beginning they are initialized to the
    // domain size of the variables (which are, themselves initialized to 0).
    // Each time we increment a variable, its corresponding txxx_value is
    // decreased by 1.  When the latter is equal to 0, this means that the
    // variable itself should be reinitialized to 0 as well and that the next
    // variable of the table should be increased (that is, this is similar to
    // increasing 9 to 10).
    std::vector< Idx > t1_and_t2_value = t1_and_t2_domain;
    std::vector< Idx > t1_alone_value  = t1_alone_domain;
    std::vector< Idx > t2_alone_value  = t2_alone_domain;

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
    // corresponds to the variables that belongs both to t1 and t2. The middle
    // loop to the variables that belong to t2 but not to t1. Finally, the
    // outer loop corresponds to the variables that belong to t1 but not t2.
    Idx result_offset = 0;
    // TODO: change the followings lines into:
    // Instantiation t2_inst(t2);
    // Instantiation t1_inst(t1);
    // Instantiation t1_alone_begin_inst(t1);
    // when Potentials will support thread-safe creations of Instantiations
    Instantiation t2_inst;
    Instantiation t1_inst;
    Instantiation t1_alone_begin_inst;
    for (const auto var: t2_vars)
      t2_inst.add(*var);
    for (const auto var: t1_vars)
      t1_inst.add(*var);
    for (const auto var: t1_vars)
      t1_alone_begin_inst.add(*var);

    // test if all the variables in common in t1 and t2 are the first variables
    // and are in the same order. In this case, we can speed-up the
    // incrementation processes
    if (t1_and_t2_begin_vars) {
      for (Idx i = 0; i < t1_alone_domain_size; ++i) {
        t2_inst.setFirst();
        t1_alone_begin_inst = t1_inst;

        for (Idx j = 0; j < t2_alone_domain_size; ++j) {
          t1_inst = t1_alone_begin_inst;

          for (Idx z = 0; z < t1_and_t2_domain_size; ++z) {
            result->unsafeSet(result_offset,
                              GUM_MULTI_DIM_OPERATOR(t1->get(t1_inst), t2->get(t2_inst)));

            ++result_offset;

            // update the offset of both t1 and t2
            ++t1_inst;
            ++t2_inst;
          }
        }
      }
    } else {
      for (Idx i = 0; i < t1_alone_domain_size; ++i) {
        t2_inst.setFirst();
        t1_alone_begin_inst = t1_inst;

        for (Idx j = 0; j < t2_alone_domain_size; ++j) {
          t1_inst = t1_alone_begin_inst;

          for (Idx z = 0; z < t1_and_t2_domain_size; ++z) {
            result->unsafeSet(result_offset,
                              GUM_MULTI_DIM_OPERATOR(t1->get(t1_inst), t2->get(t2_inst)));

            ++result_offset;

            // update the offset of both t1 and t2
            for (unsigned int k = 0; k < t1_and_t2_value.size(); ++k) {
              --t1_and_t2_value[k];

              if (t1_and_t2_value[k]) {
                t1_inst.incVar(*(t1_and_t2_var[k]));
                t2_inst.incVar(*(t1_and_t2_var[k]));
                break;
              }

              t1_and_t2_value[k] = t1_and_t2_domain[k];
              t1_inst.setFirstVar(*(t1_and_t2_var[k]));
              t2_inst.setFirstVar(*(t1_and_t2_var[k]));
            }
          }

          // update the offset of t2 alone
          for (unsigned int k = 0; k < t2_alone_value.size(); ++k) {
            --t2_alone_value[k];

            if (t2_alone_value[k]) {
              t2_inst.incVar(*(t2_alone_var[k]));
              break;
            }

            t2_alone_value[k] = t2_alone_domain[k];
            t2_inst.setFirstVar(*(t2_alone_var[k]));
          }
        }

        // update the offset of t1 alone
        for (unsigned int k = 0; k < t1_alone_value.size(); ++k) {
          --t1_alone_value[k];

          if (t1_alone_value[k]) {
            t1_inst.incVar(*(t1_alone_var[k]));
            break;
          }

          t1_alone_value[k] = t1_alone_domain[k];
          t1_inst.setFirstVar(*(t1_alone_var[k]));
        }
      }
    }

    return result;
  }

#  undef GUM_MULTI_DIM_OPERATOR_TYPE
}   // namespace gum
#endif /* GUM_OPERATOR_PATTERN_ALLOWED */
