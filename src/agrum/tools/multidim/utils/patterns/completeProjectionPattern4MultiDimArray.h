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
 * @brief the pattern used by all the projections of multidim tables over all
 * their variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// check if we allowed these patterns to be used
#ifndef GUM_COMPLETE_PROJECTION_PATTERN_ALLOWED

// #warning To use completeProjectionPattern, you must define
// GUM_COMPLETE_PROJECTION_PATTERN_ALLOWED

#else
namespace gum {

  // a specialized function for projecting a multiDimArray over all its variables

#  ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME
#    define GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE GUM_SCALAR
  template < typename GUM_SCALAR >
  GUM_SCALAR GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME(const MultiDimArray< GUM_SCALAR >* table,
                                                    Instantiation* instantiation)
#  endif

  // clang-format off

#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE GUM_SCALAR *
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
  template <typename GUM_SCALAR>
  GUM_SCALAR* GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME(
      const MultiDimArray<GUM_SCALAR*>* table,
      Instantiation* instantiation )
#endif

#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME_F
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE GUM_SCALAR
  template <typename GUM_SCALAR>
  GUM_SCALAR GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME_F(
      const MultiDimArray<GUM_SCALAR>* table,
      GUM_SCALAR ( *f )( const GUM_SCALAR&,
                         const GUM_SCALAR& ),
                         Instantiation* instantiation )
#endif

#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME_F
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE GUM_SCALAR *
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
  template <typename GUM_SCALAR>
  GUM_SCALAR* GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME_F(
      const MultiDimArray<GUM_SCALAR*>* table,
      GUM_SCALAR* ( *f )( const GUM_SCALAR const*,
                          const GUM_SCALAR const* ),
                          Instantiation* instantiation )
#endif

#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_IMPL2ARRAY_NAME
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE GUM_SCALAR
  template <typename GUM_SCALAR>
  GUM_SCALAR GUM_MULTI_DIM_COMPLETE_PROJECTION_IMPL2ARRAY_NAME(
      const MultiDimImplementation<GUM_SCALAR>* ttable,
      Instantiation* instantiation )
#endif

#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_IMPL2ARRAY_NAME
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE GUM_SCALAR *
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
  template <typename GUM_SCALAR>
  GUM_SCALAR*
  GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_IMPL2ARRAY_NAME(
      const MultiDimImplementation<GUM_SCALAR*>* ttable,
      Instantiation* instantiation )
#endif

  // clang-format on

  {

#  ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_IMPL2ARRAY_NAME
    const auto table = reinterpret_cast< const MultiDimArray< GUM_SCALAR >* >(ttable);
#  endif

#  ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_IMPL2ARRAY_NAME
    const auto table = reinterpret_cast< const MultiDimArray< GUM_SCALAR* >* >(ttable);
#  endif

    // first, compute the domain size of the table:
    Size nb_elt = table->domainSize();

    // now, parse the table and compute the projection. Start with the
    // neutral element
    GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE* ptable
       = const_cast< GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE* >(&(table->unsafeGet(0)));
    GUM_SCALAR current_val = GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL;

    if (instantiation) {
      GUM_SCALAR                              old_val     = current_val;
      GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE* current_ptr = ptable;
      GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE* deb_ptr     = ptable;

      for (Size i = 0; i < nb_elt; ++i, ++ptable) {
#  ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
        GUM_MULTI_DIM_COMPLETE_PROJECTION(&current_val, *ptable);
#  else
        GUM_MULTI_DIM_COMPLETE_PROJECTION(current_val, *ptable);
#  endif

        if (current_val != old_val) {
          old_val     = current_val;
          current_ptr = ptable;
        }
      }

      // put the table's variables into the instantiation
      instantiation->forgetMaster();
      instantiation->clear();
      const Sequence< const DiscreteVariable* >& table_vars = table->variablesSequence();

      for (const auto var: table_vars)
        instantiation->add(*var);

      // return the result
      table->fromOffset(*instantiation, Size(current_ptr - deb_ptr));
#  ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
      return *current_ptr;
#  else
      return current_val;
#  endif /* GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER */
    } else {
#  ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
      GUM_SCALAR                              old_val     = current_val;
      GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE* current_ptr = ptable;

      for (Size i = 0; i < nb_elt; ++i, ++ptable) {
        GUM_MULTI_DIM_COMPLETE_PROJECTION(&current_val, *ptable);

        if (current_val != old_val) {
          old_val     = current_val;
          current_ptr = ptable;
        }
      }

      return *current_ptr;
#  else

      for (Size i = 0; i < nb_elt; ++i, ++ptable) {
        GUM_MULTI_DIM_COMPLETE_PROJECTION(current_val, *ptable);
      }

      return current_val;
#  endif /* GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER  */
    }
  }

#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE

#  ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
#    undef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
#  endif

} /* End of namespace gum */

#endif /* GUM_COMPLETE_PROJECTION_PATTERN_ALLOWED */
