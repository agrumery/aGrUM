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
 * @brief Efficient functionals for combining multiDims
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_OPERATORS_4_MULTI_DIM_H
#define GUM_OPERATORS_4_MULTI_DIM_H

#include <agrum/tools/core/timer.h>
#include <agrum/tools/multidim/implementations/multiDimArray.h>
#include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>
#include <atomic>
#include <mutex>

namespace gum {

  // ==========================================================================
  /// @name Add MultiDims
  // ==========================================================================
  /// @{

  /**
   * @brief a specialized function for summing two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial addition of t1 and t2, that is, if t1
   * and t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function add2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] + t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function add2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator+ on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your addition.
   */
  template < typename T >
  MultiDimArray< T >* add2MultiDimArrays(const MultiDimArray< T >* t1,
                                         const MultiDimArray< T >* t2);

  /**
   * @brief a specialized function for summing two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial addition of t1 and t2, that is, if t1
   * and t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function add2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] + t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function add2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator+ on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your addition.
   */
  template < typename T >
  MultiDimImplementation< T >* add2MultiDimArrays(const MultiDimImplementation< T >* t1,
                                                  const MultiDimImplementation< T >* t2);

  /**
   * @brief a specialized function for summing two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial addition of t1 and t2, that is, if t1
   * and t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function add2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] + t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function add2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator+ on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your addition.
   */
  template < typename T >
  MultiDimArray< T* >* add2MultiDimArrays4Pointers(const MultiDimArray< T* >* t1,
                                                   const MultiDimArray< T* >* t2);

  /**
   * @brief a specialized function for summing two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial addition of t1 and t2, that is, if t1
   * and t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function add2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] + t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function add2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator+ on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your addition.
   */
  template < typename T >
  MultiDimImplementation< T* >* add2MultiDimArrays4Pointers(const MultiDimImplementation< T* >* t1,
                                                            const MultiDimImplementation< T* >* t2);

  /**
   * @brief a specialized function for summing two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial addition of t1 and t2, that is, if t1
   * and t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function add2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] + t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function add2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator+ on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your addition.
   */
  template < typename T >
  MultiDimFunctionGraph< T >* add2MultiDimFunctionGraphs(const MultiDimFunctionGraph< T >* t1,
                                                         const MultiDimFunctionGraph< T >* t2);

  /**
   * @brief a specialized function for summing two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial addition of t1 and t2, that is, if t1
   * and t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function add2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] + t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function add2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator+ on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your addition.
   */
  template < typename T >
  MultiDimImplementation< T >* add2MultiDimFunctionGraphs(const MultiDimImplementation< T >* t1,
                                                          const MultiDimImplementation< T >* t2);

  /// @}
  // ==========================================================================
  /// @name Substract MultiDims
  // ==========================================================================
  /// @{

  /**
   * @brief a specialized function for subtracting two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial subtraction of t2 from t1, that is, if
   * t1 and t2 are multiDimArrays over variables A,B,C,D and D,B,E
   * respectively, function subtract2MultiDimArrays will return a multiDimArray
   * T defined as T[A,B,C,D,E] = t1[A,B,C,D] - t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function subtract2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator- on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your subtraction.
   */
  template < typename T >
  MultiDimArray< T >* subtract2MultiDimArrays(const MultiDimArray< T >* t1,
                                              const MultiDimArray< T >* t2);

  /**
   * @brief a specialized function for subtracting two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial subtraction of t2 from t1, that is, if
   * t1 and t2 are multiDimArrays over variables A,B,C,D and D,B,E
   * respectively, function subtract2MultiDimArrays will return a multiDimArray
   * T defined as T[A,B,C,D,E] = t1[A,B,C,D] - t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function subtract2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator- on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your subtraction.
   */
  template < typename T >
  MultiDimImplementation< T >* subtract2MultiDimArrays(const MultiDimImplementation< T >* t1,
                                                       const MultiDimImplementation< T >* t2);

  /**
   * @brief a specialized function for subtracting two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial subtraction of t2 from t1, that is, if
   * t1 and t2 are multiDimArrays over variables A,B,C,D and D,B,E
   * respectively, function subtract2MultiDimArrays will return a multiDimArray
   * T defined as T[A,B,C,D,E] = t1[A,B,C,D] - t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function subtract2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator- on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your subtraction.
   */
  template < typename T >
  MultiDimArray< T* >* subtract2MultiDimArrays4Pointers(const MultiDimArray< T* >* t1,
                                                        const MultiDimArray< T* >* t2);

  /**
   * @brief a specialized function for subtracting two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial subtraction of t2 from t1, that is, if
   * t1 and t2 are multiDimArrays over variables A,B,C,D and D,B,E
   * respectively, function subtract2MultiDimArrays will return a multiDimArray
   * T defined as T[A,B,C,D,E] = t1[A,B,C,D] - t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function subtract2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator- on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your subtraction.
   */
  template < typename T >
  MultiDimImplementation< T* >*
     subtract2MultiDimArrays4Pointers(const MultiDimImplementation< T* >* t1,
                                      const MultiDimImplementation< T* >* t2);

  /**
   * @brief a specialized function for subtracting two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial subtraction of t2 from t1, that is, if
   * t1 and t2 are multiDimArrays over variables A,B,C,D and D,B,E
   * respectively, function subtract2MultiDimArrays will return a multiDimArray
   * T defined as T[A,B,C,D,E] = t1[A,B,C,D] - t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function subtract2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator- on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your subtraction.
   */
  template < typename T >
  MultiDimFunctionGraph< T >* subtract2MultiDimFunctionGraphs(const MultiDimFunctionGraph< T >* t1,
                                                              const MultiDimFunctionGraph< T >* t2);

  /**
   * @brief a specialized function for subtracting two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial subtraction of t2 from t1, that is, if
   * t1 and t2 are multiDimArrays over variables A,B,C,D and D,B,E
   * respectively, function subtract2MultiDimArrays will return a multiDimArray
   * T defined as T[A,B,C,D,E] = t1[A,B,C,D] - t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function subtract2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator- on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your subtraction.
   */
  template < typename T >
  MultiDimImplementation< T >*
     subtract2MultiDimFunctionGraphs(const MultiDimImplementation< T >* t1,
                                     const MultiDimImplementation< T >* t2);

  /// @}
  // ==========================================================================
  /// @name Multiply MultiDims
  // ==========================================================================
  /// @{

  /**
   * @brief a specialized function for multiplying two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial product of t2 from t1, that is, if t1
   * and t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function multiply2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] * t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function multiply2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator* on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your multiplication.
   */
  template < typename T >
  MultiDimArray< T >* multiply2MultiDimArrays(const MultiDimArray< T >* t1,
                                              const MultiDimArray< T >* t2);

  /**
   * @brief a specialized function for multiplying two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial product of t2 from t1, that is, if t1
   * and t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function multiply2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] * t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function multiply2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator* on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your multiplication.
   */
  template < typename T >
  MultiDimImplementation< T >* multiply2MultiDimArrays(const MultiDimImplementation< T >* t1,
                                                       const MultiDimImplementation< T >* t2);

  /**
   * @brief a specialized function for multiplying two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial product of t2 from t1, that is, if t1
   * and t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function multiply2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] * t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function multiply2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator* on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your multiplication.
   */
  template < typename T >
  MultiDimArray< T* >* multiply2MultiDimArrays4Pointers(const MultiDimArray< T* >* t1,
                                                        const MultiDimArray< T* >* t2);

  /**
   * @brief a specialized function for multiplying two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial product of t2 from t1, that is, if t1
   * and t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function multiply2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] * t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function multiply2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator* on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your multiplication.
   */
  template < typename T >
  MultiDimImplementation< T* >*
     multiply2MultiDimArrays4Pointers(const MultiDimImplementation< T* >* t1,
                                      const MultiDimImplementation< T* >* t2);

  /**
   * @brief a specialized function for multiplying two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial product of t2 from t1, that is, if t1
   * and t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function multiply2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] * t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function multiply2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator* on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your multiplication.
   */
  template < typename T >
  MultiDimFunctionGraph< T >* multiply2MultiDimFunctionGraphs(const MultiDimFunctionGraph< T >* t1,
                                                              const MultiDimFunctionGraph< T >* t2);

  /**
   * @brief a specialized function for multiplying two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial product of t2 from t1, that is, if t1
   * and t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function multiply2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] * t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function multiply2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator* on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your multiplication.
   */
  template < typename T >
  MultiDimImplementation< T >*
     multiply2MultiDimFunctionGraphs(const MultiDimImplementation< T >* t1,
                                     const MultiDimImplementation< T >* t2);

  /// @}
  // ==========================================================================
  /// @name Divide MultiDims
  // ==========================================================================
  /// @{

  /**
   * @brief a specialized function for dividing two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial division of t1 by t2, that is, if t1 and
   * t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function divide2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] / t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function divide2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator/ on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your division.
   */
  template < typename T >
  MultiDimArray< T >* divide2MultiDimArrays(const MultiDimArray< T >* t1,
                                            const MultiDimArray< T >* t2);

  /**
   * @brief a specialized function for dividing two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial division of t1 by t2, that is, if t1 and
   * t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function divide2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] / t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function divide2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator/ on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your division.
   */
  template < typename T >
  MultiDimImplementation< T >* divide2MultiDimArrays(const MultiDimImplementation< T >* t1,
                                                     const MultiDimImplementation< T >* t2);

  /**
   * @brief a specialized function for dividing two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial division of t1 by t2, that is, if t1 and
   * t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function divide2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] / t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function divide2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator/ on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your division.
   */
  template < typename T >
  MultiDimArray< T* >* divide2MultiDimArrays4Pointers(const MultiDimArray< T* >* t1,
                                                      const MultiDimArray< T* >* t2);

  /**
   * @brief a specialized function for dividing two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial division of t1 by t2, that is, if t1 and
   * t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function divide2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] / t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function divide2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator/ on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your division.
   */
  template < typename T >
  MultiDimImplementation< T* >*
     divide2MultiDimArrays4Pointers(const MultiDimImplementation< T* >* t1,
                                    const MultiDimImplementation< T* >* t2);

  /**
   * @brief a specialized function for dividing two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial division of t1 by t2, that is, if t1 and
   * t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function divide2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] / t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function divide2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator/ on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your division.
   */
  template < typename T >
  MultiDimFunctionGraph< T >* divide2MultiDimFunctionGraphs(const MultiDimFunctionGraph< T >* t1,
                                                            const MultiDimFunctionGraph< T >* t2);

  /**
   * @brief a specialized function for dividing two multiDimArrays
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial division of t1 by t2, that is, if t1 and
   * t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function divide2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] / t2[D,B,E] for all A,B,C,D,E.
   *
   * @return function divide2MultiDimArrays allocates on the heap the new T
   * multiDimArray and returns it
   *
   * @warning In practice, do not use this function but rather operator/ on
   * multiDimImplementations. This operator will take care to select this
   * function if it is appropriate for your division.
   */
  template < typename T >
  MultiDimImplementation< T >* divide2MultiDimFunctionGraphs(const MultiDimImplementation< T >* t1,
                                                             const MultiDimImplementation< T >* t2);

  /// @}
  // ==========================================================================
  /// @name Maximise MultiDims
  // ==========================================================================
  /// @{

  /**
   * @brief a specialized function for compare value from two
   * multiDimFunctionGraphs and gets the maximal
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial comparison between t1 and t2, that is,
   * if t1 and t2 are multiDimFunctionGraphs over variables A,B,C,D and D,B,E
   * respectively, function maximise2MultiDimFunctionGraphs will return a
   * multiDimFunctionGraph T defined as T[A,B,C,D,E] = max( t1[A,B,C,D],
   * t2[D,B,E] ) for all A,B,C,D,E.
   *
   * @return function maximise2MultiDimFunctionGraphs allocates on the heap the
   * new T multiDimFunctionGraph and returns it
   */
  template < typename T >
  MultiDimFunctionGraph< T >* maximise2MultiDimFunctionGraphs(const MultiDimFunctionGraph< T >* t1,
                                                              const MultiDimFunctionGraph< T >* t2);

  /**
   * @brief a specialized function for compare value from two
   * multiDimFunctionGraphs and gets the maximal
   * @ingroup multidim_op_group
   *
   * The function produces a tensorial comparison between t1 and t2, that is,
   * if t1 and t2 are multiDimFunctionGraphs over variables A,B,C,D and D,B,E
   * respectively, function maximise2MultiDimFunctionGraphs will return a
   * multiDimFunctionGraph T defined as T[A,B,C,D,E] = max( t1[A,B,C,D],
   * t2[D,B,E] ) for all A,B,C,D,E.
   *
   * @return function maximise2MultiDimFunctionGraphs allocates on the heap the
   * new T multiDimFunctionGraph and returns it
   */
  template < typename T >
  MultiDimImplementation< T >*
     maximise2MultiDimFunctionGraphs(const MultiDimImplementation< T >* t1,
                                     const MultiDimImplementation< T >* t2);

  /// @}
  // ==========================================================================
  /// @name Operators over MultiDim
  // ==========================================================================
  /// @{

  /// The function to be used to add two MultiDimImplementations
  /// @ingroup multidim_op_group
  template < typename T >
  MultiDimImplementation< T >* operator+(const MultiDimImplementation< T >&,
                                         const MultiDimImplementation< T >&);

  /// The function to be used to subtract two MultiDimImplementations
  /// @ingroup multidim_op_group
  template < typename T >
  MultiDimImplementation< T >* operator-(const MultiDimImplementation< T >&,
                                         const MultiDimImplementation< T >&);

  /// The function to be used to multiply two MultiDimImplementations
  /// @ingroup multidim_op_group
  template < typename T >
  MultiDimImplementation< T >* operator*(const MultiDimImplementation< T >&,
                                         const MultiDimImplementation< T >&);

  /// The function to be used to divide two MultiDimImplementations
  /// @ingroup multidim_op_group
  template < typename T >
  MultiDimImplementation< T >* operator/(const MultiDimImplementation< T >&,
                                         const MultiDimImplementation< T >&);

  /// @}

  // ==========================================================================
  // DO NOT FORGET TO REGISTER YOUR BINARY FUNCTIONS AT THE END OF FILE
  // operators4MultiDim_tpl.h
  // ==========================================================================

  /**
   * @brief the function used to register all the operators on
   * multidimImplementations over non-pointers types.
   * @ingroup multidim_op_group
   */
  template < typename GUM_SCALAR >
  void operators4MultiDimInit();

  /**
   * @brief the function used to register all the operators on
   * multidimImplementations over pointers types.
   * @ingroup multidim_op_group
   */
  template < typename GUM_SCALAR >
  void pointerOperators4MultiDimInit();

  /**
   * @brief a class used to register operators over non-pointers types
   * @ingroup multidim_op_group
   *
   * This class is of course completely redundant with function
   * operators4MultiDimInit. Its aim is to enable specialization of function
   * operators4MultiDimInit for pointer types: C++ allows partial
   * specialization of templated classes (e.g., create different
   * implementations for C<T> and C<T*>) but it does not allows partial
   * specialization for functions. Hence, by creating a class the primary
   * purpose of which is to run function operators4MultiDimInit, we allow this
   * partial specialization. This is most effective to produce different codes
   * for pointer types and non-pointer types.
   */
  template < typename GUM_SCALAR >
  struct Operators4MultiDimInitialize {
    void init() { operators4MultiDimInit< GUM_SCALAR >(); };
  };

  /**
   * @brief a class used to register operators over pointers types
   * @ingroup multidim_op_group
   *
   * This class is of course completely redundant with function
   * pointerOperators4MultiDimInit. Its aim is to enable different
   * implementations of the operators for multidims depending in whether these
   * multidim contain pointers or not. Actually, C++ allows partial
   * specialization of templated classes (e.g., create different
   * implementations for C<T> and C<T*>) but it does not allows partial
   * specialization for functions. Hence, by creating a class the primary
   * purpose of which is to run function operators4MultiDimInit or
   * pointerOperators4MultiDimInit, we allow this partial specialization to
   * obtain.
   */
  template < typename GUM_SCALAR >
  struct Operators4MultiDimInitialize< GUM_SCALAR* > {
    void init() { pointerOperators4MultiDimInit< GUM_SCALAR >(); };
  };

  /// @}

} /* namespace gum */

// always include the templatized implementations
#include <agrum/tools/multidim/utils/operators/operators4MultiDim_tpl.h>

#endif /* GUM_OPERATORS_4_MULTI_DIM_H */
