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
 * @brief Efficient functionals for projecting multiDimensional tables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_PROJECTIONS_4_MULTI_DIM_H
#define GUM_PROJECTIONS_4_MULTI_DIM_H

#include <agrum/tools/multidim/implementations/multiDimArray.h>
#include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>
#include <atomic>
#include <mutex>

namespace gum {

  // ==========================================================================
  /// @name Project Max for MultiDim
  // ==========================================================================
  /// @{

  /**
   * @brief a specialized function for projecting a multiDimArray using a Max
   * operator
   * @ingroup multidim_op_group
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a MultiDimArray
   * defined over A,B,C,D and if del_vars = {A,C}, then the function returns a
   * table over B,D defined as Max_{A,C} table (A,B,C,D).  @return function
   * ProjectMaxMultiDimArrays allocates on the heap the new T multiDimArray and
   * returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMax on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimArray< GUM_SCALAR >*
     projectMaxMultiDimArray(const MultiDimArray< GUM_SCALAR >*    table,
                             const Set< const DiscreteVariable* >& del_vars);

  /**
   * @brief a specialized function for projecting a multiDimArray using a Max
   * operator
   * @ingroup multidim_op_group
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a MultiDimArray
   * defined over A,B,C,D and if del_vars = {A,C}, then the function returns a
   * table over B,D defined as Max_{A,C} table (A,B,C,D).  @return function
   * ProjectMaxMultiDimArrays allocates on the heap the new T multiDimArray and
   * returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMax on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >*
     projectMaxMultiDimArray(const MultiDimImplementation< GUM_SCALAR >* table,
                             const Set< const DiscreteVariable* >&       del_vars);

  /**
   * @brief a specialized function for projecting a multiDimArray using a Max
   * operator
   * @ingroup multidim_op_group
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a MultiDimArray
   * defined over A,B,C,D and if del_vars = {A,C}, then the function returns a
   * table over B,D defined as Max_{A,C} table (A,B,C,D).  @return function
   * ProjectMaxMultiDimArrays allocates on the heap the new T multiDimArray and
   * returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMax on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimArray< GUM_SCALAR* >*
     projectMaxMultiDimArray4Pointers(const MultiDimArray< GUM_SCALAR* >*   table,
                                      const Set< const DiscreteVariable* >& del_vars);

  /**
   * @brief a specialized function for projecting a multiDimArray using a Max
   * operator
   * @ingroup multidim_op_group
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a MultiDimArray
   * defined over A,B,C,D and if del_vars = {A,C}, then the function returns a
   * table over B,D defined as Max_{A,C} table (A,B,C,D).  @return function
   * ProjectMaxMultiDimArrays allocates on the heap the new T multiDimArray and
   * returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMax on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR* >*
     projectMaxMultiDimArray4Pointers(const MultiDimImplementation< GUM_SCALAR* >* table,
                                      const Set< const DiscreteVariable* >&        del_vars);

  // @}
  // ==========================================================================
  /// @name Project Min for MultiDim
  // ==========================================================================
  /// @{

  /**
   * @brief a specialized function for projecting a multiDimArray using a Min
   * operator
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a MultiDimArray
   * defined over A,B,C,D and if del_vars = {A,C}, then the function returns a
   * table over B,D defined as Min_{A,C} table (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMin on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimArray< GUM_SCALAR >*
     projectMinMultiDimArray(const MultiDimArray< GUM_SCALAR >*    table,
                             const Set< const DiscreteVariable* >& del_vars);

  /**
   * @brief a specialized function for projecting a multiDimArray using a Min
   * operator
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a MultiDimArray
   * defined over A,B,C,D and if del_vars = {A,C}, then the function returns a
   * table over B,D defined as Min_{A,C} table (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMin on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >*
     projectMinMultiDimArray(const MultiDimImplementation< GUM_SCALAR >* table,
                             const Set< const DiscreteVariable* >&       del_vars);

  /**
   * @brief a specialized function for projecting a multiDimArray using a Min
   * operator
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a MultiDimArray
   * defined over A,B,C,D and if del_vars = {A,C}, then the function returns a
   * table over B,D defined as Min_{A,C} table (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMin on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimArray< GUM_SCALAR* >*
     projectMinMultiDimArray4Pointers(const MultiDimArray< GUM_SCALAR* >*   table,
                                      const Set< const DiscreteVariable* >& del_vars);

  /**
   * @brief a specialized function for projecting a multiDimArray using a Min
   * operator
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a MultiDimArray
   * defined over A,B,C,D and if del_vars = {A,C}, then the function returns a
   * table over B,D defined as Min_{A,C} table (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMin on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR* >*
     projectMinMultiDimArray4Pointers(const MultiDimImplementation< GUM_SCALAR* >* table,
                                      const Set< const DiscreteVariable* >&        del_vars);

  // @}
  // ==========================================================================
  /// @name Project Sum for MultiDim
  // ==========================================================================
  /// @{

  /**
   * @brief a specialized function for projecting a multiDimArray using a
   * summation
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a MultiDimArray
   * defined over A,B,C,D and if del_vars = {A,C}, then the function returns a
   * table over B,D defined as Sum_{A,C} table (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectSum on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimArray< GUM_SCALAR >*
     projectSumMultiDimArray(const MultiDimArray< GUM_SCALAR >*    table,
                             const Set< const DiscreteVariable* >& del_vars);

  /**
   * @brief a specialized function for projecting a multiDimArray using a
   * summation
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a MultiDimArray
   * defined over A,B,C,D and if del_vars = {A,C}, then the function returns a
   * table over B,D defined as Sum_{A,C} table (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectSum on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >*
     projectSumMultiDimArray(const MultiDimImplementation< GUM_SCALAR >* table,
                             const Set< const DiscreteVariable* >&       del_vars);

  /**
   * @brief a specialized function for projecting a multiDimArray using a
   * summation
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a MultiDimArray
   * defined over A,B,C,D and if del_vars = {A,C}, then the function returns a
   * table over B,D defined as Sum_{A,C} table (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectSum on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimArray< GUM_SCALAR* >*
     projectSumMultiDimArray4Pointers(const MultiDimArray< GUM_SCALAR* >*   table,
                                      const Set< const DiscreteVariable* >& del_vars);

  /**
   * @brief a specialized function for projecting a multiDimArray using a
   * summation
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a MultiDimArray
   * defined over A,B,C,D and if del_vars = {A,C}, then the function returns a
   * table over B,D defined as Sum_{A,C} table (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectSum on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR* >*
     projectSumMultiDimArray4Pointers(const MultiDimImplementation< GUM_SCALAR* >* table,
                                      const Set< const DiscreteVariable* >&        del_vars);

  // @}
  // ==========================================================================
  /// @name Project Product for MultiDim
  // ==========================================================================
  /// @{

  /**
   * @brief a specialized function for projecting a multiDimArray using
   * products
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a MultiDimArray
   * defined over A,B,C,D and if del_vars = {A,C}, then the function returns a
   * table over B,D defined as Prod_{A,C} table (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectProduct on MultiDimImplementation. This operator
   * will take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimArray< GUM_SCALAR >*
     projectProductMultiDimArray(const MultiDimArray< GUM_SCALAR >*    table,
                                 const Set< const DiscreteVariable* >& del_vars);

  /**
   * @brief a specialized function for projecting a multiDimArray using
   * products
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a MultiDimArray
   * defined over A,B,C,D and if del_vars = {A,C}, then the function returns a
   * table over B,D defined as Prod_{A,C} table (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectProduct on MultiDimImplementation. This operator
   * will take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >*
     projectProductMultiDimArray(const MultiDimImplementation< GUM_SCALAR >* table,
                                 const Set< const DiscreteVariable* >&       del_vars);

  /**
   * @brief a specialized function for projecting a multiDimArray using
   * products
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a MultiDimArray
   * defined over A,B,C,D and if del_vars = {A,C}, then the function returns a
   * table over B,D defined as Prod_{A,C} table (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectProduct on MultiDimImplementation. This operator
   * will take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimArray< GUM_SCALAR* >*
     projectProductMultiDimArray4Pointers(const MultiDimArray< GUM_SCALAR* >*   table,
                                          const Set< const DiscreteVariable* >& del_vars);

  /**
   * @brief a specialized function for projecting a multiDimArray using
   * products
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a MultiDimArray
   * defined over A,B,C,D and if del_vars = {A,C}, then the function returns a
   * table over B,D defined as Prod_{A,C} table (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectProduct on MultiDimImplementation. This operator
   * will take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR* >*
     projectProductMultiDimArray4Pointers(const MultiDimImplementation< GUM_SCALAR* >* table,
                                          const Set< const DiscreteVariable* >&        del_vars);

  // @}
  // ==========================================================================
  /// @name Project Function for MultiDim
  // ==========================================================================
  /// @{

  /**
   * @brief a specialized function for functionally projecting a multiDimArray
   *
   * The function produces a tensorial functional combination of t1 and t2,
   * that is, if t1 and t2 are multiDimArrays over variables A,B,C,D and D,B,E
   * respectively, function combine2MultiDimArrays will return a multiDimArray
   * T defined as T[A,B,C,D,E] = f ( t1[A,B,C,D],t2[D,B,E] ) for all A,B,C,D,E.
   *
   * @return function combine2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it.
   */
  template < typename GUM_SCALAR >
  MultiDimArray< GUM_SCALAR >* ProjectFuncMultiDimArray(const MultiDimArray< GUM_SCALAR >* t1,
                                                        const MultiDimArray< GUM_SCALAR >* t2,
                                                        const GUM_SCALAR (*f)(const GUM_SCALAR&,
                                                                              const GUM_SCALAR&));

  /**
   * @brief a specialized function for functionally projecting a multiDimArray
   *
   * The function produces a tensorial functional combination of t1 and t2,
   * that is, if t1 and t2 are multiDimArrays over variables A,B,C,D and D,B,E
   * respectively, function combine2MultiDimArrays will return a multiDimArray
   * T defined as T[A,B,C,D,E] = f ( t1[A,B,C,D],t2[D,B,E] ) for all A,B,C,D,E.
   *
   * @return function combine2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >*
     combine2MultiDimArrays(const MultiDimImplementation< GUM_SCALAR >* t1,
                            const MultiDimImplementation< GUM_SCALAR >* t2,
                            const GUM_SCALAR (*f)(const GUM_SCALAR&, const GUM_SCALAR&));

  // @}
  // ==========================================================================
  /// @name Projectors For Function Graphs
  // ==========================================================================
  /// @{

  /**
   * @brief a specialized function for projecting a multiDimFunctionGraph using
   * a Max operator
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a
   * MultiDimFunctionGraph defined over A,B,C,D and if del_vars = {A,C}, then
   * the function returns a table over B,D defined as Max_{A,C} table
   * (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimFunctionGraph allocates on the heap the
   * new T multiDimFunctionGraph and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMax on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
     projectMaxMultiDimFunctionGraph(const MultiDimFunctionGraph< GUM_SCALAR >* table,
                                     const Set< const DiscreteVariable* >&      del_vars);

  /**
   * @brief a specialized function for projecting a multiDimFunctionGraph using
   * a Max operator
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a
   * MultiDimFunctionGraph defined over A,B,C,D and if del_vars = {A,C}, then
   * the function returns a table over B,D defined as Max_{A,C} table
   * (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimFunctionGraph allocates on the heap the
   * new T multiDimFunctionGraph and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMax on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >*
     projectMaxMultiDimFunctionGraph(const MultiDimImplementation< GUM_SCALAR >* table,
                                     const Set< const DiscreteVariable* >&       del_vars);


  /**
   * @brief a specialized function for projecting a multiDimFunctionGraph
   * using a Min operator
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a
   * MultiDimFunctionGraph defined over A,B,C,D and if del_vars = {A,C}, then
   * the function returns a table over B,D defined as Min_{A,C} table
   * (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimFunctionGraphs allocates on the heap
   * the new T multiDimFunctionGraph and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMin on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
     projectMinMultiDimFunctionGraph(const MultiDimFunctionGraph< GUM_SCALAR >* table,
                                     const Set< const DiscreteVariable* >&      del_vars);

  /**
   * @brief a specialized function for projecting a multiDimFunctionGraph
   * using a Min operator
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a
   * MultiDimFunctionGraph defined over A,B,C,D and if del_vars = {A,C}, then
   * the function returns a table over B,D defined as Min_{A,C} table
   * (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimFunctionGraphs allocates on the heap
   * the new T multiDimFunctionGraph and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMin on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >*
     projectMinMultiDimFunctionGraph(const MultiDimImplementation< GUM_SCALAR >* table,
                                     const Set< const DiscreteVariable* >&       del_vars);


  /**
   * @brief a specialized function for projecting a multiDimFunctionGraph using
   * a summation
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a
   * MultiDimFunctionGraph defined over A,B,C,D and if del_vars = {A,C}, then
   * the function returns a table over B,D defined as Sum_{A,C} table
   * (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimFunctionGraphs allocates on the heap
   * the new T multiDimFunctionGraph and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectSum on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
     projectSumMultiDimFunctionGraph(const MultiDimFunctionGraph< GUM_SCALAR >* table,
                                     const Set< const DiscreteVariable* >&      del_vars);

  /**
   * @brief a specialized function for projecting a multiDimFunctionGraph using
   * a summation
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a
   * MultiDimFunctionGraph defined over A,B,C,D and if del_vars = {A,C}, then
   * the function returns a table over B,D defined as Sum_{A,C} table
   * (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimFunctionGraphs allocates on the heap
   * the new T multiDimFunctionGraph and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectSum on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >*
     projectSumMultiDimFunctionGraph(const MultiDimImplementation< GUM_SCALAR >* table,
                                     const Set< const DiscreteVariable* >&       del_vars);


  /**
   * @brief a specialized function for projecting a multiDimFunctionGraph using
   * products
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a
   * MultiDimFunctionGraph defined over A,B,C,D and if del_vars = {A,C}, then
   * the function returns a table over B,D defined as Prod_{A,C} table
   * (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimFunctionGraphs allocates on the heap
   * the new T multiDimFunctionGraph and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectProduct on MultiDimImplementation. This operator
   * will take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
     projectProductMultiDimFunctionGraph(const MultiDimFunctionGraph< GUM_SCALAR >* table,
                                         const Set< const DiscreteVariable* >&      del_vars);

  /**
   * @brief a specialized function for projecting a multiDimFunctionGraph using
   * products
   *
   * The function projects a table over the subset of its variables NOT IN the
   * set of variables del_vars. For instance, if table is a
   * MultiDimFunctionGraph defined over A,B,C,D and if del_vars = {A,C}, then
   * the function returns a table over B,D defined as Prod_{A,C} table
   * (A,B,C,D).
   *
   * @return function ProjectMaxMultiDimFunctionGraphs allocates on the heap
   * the new T multiDimFunctionGraph and returns it
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectProduct on MultiDimImplementation. This operator
   * will take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >*
     projectProductMultiDimFunctionGraph(const MultiDimImplementation< GUM_SCALAR >* table,
                                         const Set< const DiscreteVariable* >&       del_vars);

  /// @}

  /// The function to be used to project a MultiDimImplementation using a Max
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >*
     projectMax(const MultiDimImplementation< GUM_SCALAR >& table,
                const Set< const DiscreteVariable* >&       del_vars);

  /// The function to be used to project a MultiDimImplementation using a Min
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >*
     projectMin(const MultiDimImplementation< GUM_SCALAR >& table,
                const Set< const DiscreteVariable* >&       del_vars);

  /// The function to be used to project a MultiDimImplementation using a Sum
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >*
     projectSum(const MultiDimImplementation< GUM_SCALAR >& table,
                const Set< const DiscreteVariable* >&       del_vars);

  /// The function to be used to project a MultiDimImplementation using a
  /// Product
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >*
     projectProduct(const MultiDimImplementation< GUM_SCALAR >& table,
                    const Set< const DiscreteVariable* >&       del_vars);


  // ==========================================================================
  // DO NOT FORGET TO REGISTER YOUR PROJECTION FUNCTIONS AT THE END OF FILE
  // projections4MultiDim_tpl.h
  // ==========================================================================

  /**
   * @brief the function used to register all the projection operators on
   * multidimImplementations over non-pointers types.
   */
  template < typename GUM_SCALAR >
  void projections4MultiDimInit();

  /**
   * @brief the function used to register all the projections on
   * multidimImplementations over pointers types.
   */
  template < typename GUM_SCALAR >
  void pointerProjections4MultiDimInit();

  /**
   * @brief a class used to register projections over non-pointers types
   *
   * This class is of course completely redundant with function
   * projections4MultiDimInit. Its aim is to enable specialization of function
   * projections4MultiDimInit for pointer types: C++ allows partial
   * specialization of templated classes (e.g., create different
   * implementations for C<T> and C<T*>) but it does not allows partial
   * specialization for functions. Hence, by creating a class the primary
   * purpose of which is to run function projections4MultiDimInit, we allow
   * this partial specialization. This is most effective to produce different
   * codes for pointer types and non-pointer types.
   */
  template < typename GUM_SCALAR >
  struct Projections4MultiDimInitialize {
    void init() { projections4MultiDimInit< GUM_SCALAR >(); };
  };

  /**
   * @brief a class used to register projections over pointers types
   *
   * This class is of course completely redundant with function
   * pointerProjections4MultiDimInit. Its aim is to enable different
   * implementations of the projections for multidims depending in whether
   * these multidim contain pointers or not. Actually, C++ allows partial
   * specialization of templated classes (e.g., create different
   * implementations for C<T> and C<T*>) but it does not allows partial
   * specialization for functions. Hence, by creating a class the primary
   * purpose of which is to run function projections4MultiDimInit or
   * pointerProjections4MultiDimInit, we allow this partial specialization to
   * obtain.
   */
  template < typename GUM_SCALAR >
  struct Projections4MultiDimInitialize< GUM_SCALAR* > {
    void init() { pointerProjections4MultiDimInit< GUM_SCALAR >(); };
  };

} /* namespace gum */

// always include the templatized implementations
#include <agrum/tools/multidim/utils/operators/projections4MultiDim_tpl.h>

#endif /* GUM_PROJECTIONS_4_MULTI_DIM_H */
