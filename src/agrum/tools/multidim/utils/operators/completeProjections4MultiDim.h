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
 * @brief Efficient functionals for projecting multidim tables over all their
 * variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_COMPLETE_PROJECTIONS_4_MULTI_DIM_H
#define GUM_COMPLETE_PROJECTIONS_4_MULTI_DIM_H

#include <agrum/tools/multidim/implementations/multiDimArray.h>
#include <atomic>
#include <mutex>

namespace gum {

  // ==========================================================================
  /// @name Project Max Operators
  // ==========================================================================
  /// @{

  /**
   * @brief A specialized function for projecting a multiDimArray using a Max
   * operator.
   * @ingroup multidim_op_group
   *
   * The function projects a table over all its variables and, thus, returns
   * the max value stored in the table.
   *
   * @param table a pointer on the multidim table to be projected
   *
   * @param instantiation if instantiation is different from 0, then the
   * projection function stores in the instantation the values of the variables
   * that led to the max (hence, the argmax).
   *
   * @return the max value stored in the multiDimArray
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMax on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR projectMaxMultiDimArray(const MultiDimArray< GUM_SCALAR >* table,
                                     Instantiation*                     instantiation = 0);

  /**
   * @brief A specialized function for projecting a multiDimArray using a Max
   * operator.
   * @ingroup multidim_op_group
   *
   * The function projects a table over all its variables and, thus, returns
   * the max value stored in the table.
   *
   * @param table a pointer on the multidim table to be projected
   *
   * @param instantiation if instantiation is different from 0, then the
   * projection function stores in the instantation the values of the variables
   * that led to the max (hence, the argmax).
   *
   * @return the max value stored in the multiDimArray
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMax on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR projectMaxMultiDimArray(const MultiDimImplementation< GUM_SCALAR >* table,
                                     Instantiation*                              instantiation = 0);

  /**
   * @brief A specialized function for projecting a multiDimArray using a Max
   * operator.
   * @ingroup multidim_op_group
   *
   * The function projects a table over all its variables and, thus, returns
   * the max value stored in the table.
   *
   * @param table a pointer on the multidim table to be projected
   *
   * @param instantiation if instantiation is different from 0, then the
   * projection function stores in the instantation the values of the variables
   * that led to the max (hence, the argmax).
   *
   * @return the max value stored in the multiDimArray
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMax on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR* projectMaxMultiDimArray4Pointers(const MultiDimArray< GUM_SCALAR* >* table,
                                               Instantiation* instantiation = 0);

  /**
   * @brief A specialized function for projecting a multiDimArray using a Max
   * operator.
   * @ingroup multidim_op_group
   *
   * The function projects a table over all its variables and, thus, returns
   * the max value stored in the table.
   *
   * @param table a pointer on the multidim table to be projected
   *
   * @param instantiation if instantiation is different from 0, then the
   * projection function stores in the instantation the values of the variables
   * that led to the max (hence, the argmax).
   *
   * @return the max value stored in the multiDimArray
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMax on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR* projectMaxMultiDimArray4Pointers(const MultiDimImplementation< GUM_SCALAR* >* table,
                                               Instantiation* instantiation = 0);

  /// @}
  // ==========================================================================
  /// @name Project Min Operators
  // ==========================================================================
  /// @{

  /**
   * @brief A specialized function for projecting a multiDimArray using a Min
   * operator.
   * @ingroup multidim_op_group
   *
   * The function projects a table over all its variables and, thus, returns
   * the min value stored in the table
   *
   * @return the min value stored in the multiDimArray
   *
   * @param table a pointer on the multidim table to be projected
   *
   * @param instantiation if instantiation is different from 0, then the
   * projection function stores in the instantation the values of the variables
   * that led to the min (hence, the argmin).
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMin on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR projectMinMultiDimArray(const MultiDimArray< GUM_SCALAR >* table,
                                     Instantiation*                     instantiation = 0);

  /**
   * @brief A specialized function for projecting a multiDimArray using a Min
   * operator.
   * @ingroup multidim_op_group
   *
   * The function projects a table over all its variables and, thus, returns
   * the min value stored in the table
   *
   * @return the min value stored in the multiDimArray
   *
   * @param table a pointer on the multidim table to be projected
   *
   * @param instantiation if instantiation is different from 0, then the
   * projection function stores in the instantation the values of the variables
   * that led to the min (hence, the argmin).
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMin on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR projectMinMultiDimArray(const MultiDimImplementation< GUM_SCALAR >* table,
                                     Instantiation*                              instantiation = 0);

  /**
   * @brief A specialized function for projecting a multiDimArray using a Min
   * operator.
   * @ingroup multidim_op_group
   *
   * The function projects a table over all its variables and, thus, returns
   * the min value stored in the table
   *
   * @return the min value stored in the multiDimArray
   *
   * @param table a pointer on the multidim table to be projected
   *
   * @param instantiation if instantiation is different from 0, then the
   * projection function stores in the instantation the values of the variables
   * that led to the min (hence, the argmin).
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMin on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR* projectMinMultiDimArray4Pointers(const MultiDimArray< GUM_SCALAR* >* table,
                                               Instantiation* instantiation = 0);

  /**
   * @brief A specialized function for projecting a multiDimArray using a Min
   * operator.
   * @ingroup multidim_op_group
   *
   * The function projects a table over all its variables and, thus, returns
   * the min value stored in the table
   *
   * @return the min value stored in the multiDimArray
   *
   * @param table a pointer on the multidim table to be projected
   *
   * @param instantiation if instantiation is different from 0, then the
   * projection function stores in the instantation the values of the variables
   * that led to the min (hence, the argmin).
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectMin on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR* projectMinMultiDimArray4Pointers(const MultiDimImplementation< GUM_SCALAR* >* table,
                                               Instantiation* instantiation = 0);

  /// @}
  // ==========================================================================
  /// @name Project Sum Operators
  // ==========================================================================
  /// @{

  /**
   * @brief a specialized function for projecting a multiDimArray using a Sum
   * operator
   * @ingroup multidim_op_group
   *
   * The function projects a table over all its variables and, thus, returns
   * the sum of the values stored in the table
   *
   * @return the sum of the values stored in the multiDimArray
   *
   * @param table a pointer on the multidim table to be projected
   *
   * @param instantiation It shall always be 0 else instantiation is updated
   * with a meaningless value. In addition, setting instantiation to a value
   * different than 0 will induce more computations and thus will slow down the
   * projection.
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectSum on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR projectSumMultiDimArray(const MultiDimArray< GUM_SCALAR >* table,
                                     Instantiation*                     instantiation = 0);

  /**
   * @brief a specialized function for projecting a multiDimArray using a Sum
   * operator
   * @ingroup multidim_op_group
   *
   * The function projects a table over all its variables and, thus, returns
   * the sum of the values stored in the table
   *
   * @return the sum of the values stored in the multiDimArray
   *
   * @param table a pointer on the multidim table to be projected
   *
   * @param instantiation It shall always be 0 else instantiation is updated
   * with a meaningless value. In addition, setting instantiation to a value
   * different than 0 will induce more computations and thus will slow down the
   * projection.
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectSum on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR projectSumMultiDimArray(const MultiDimImplementation< GUM_SCALAR >* table,
                                     Instantiation*                              instantiation = 0);

  /**
   * @brief a specialized function for projecting a multiDimArray using a Sum
   * operator
   * @ingroup multidim_op_group
   *
   * The function projects a table over all its variables and, thus, returns
   * the sum of the values stored in the table
   *
   * @return the sum of the values stored in the multiDimArray
   *
   * @param table a pointer on the multidim table to be projected
   *
   * @param instantiation It shall always be 0 else instantiation is updated
   * with a meaningless value. In addition, setting instantiation to a value
   * different than 0 will induce more computations and thus will slow down the
   * projection.
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectSum on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR* projectSumMultiDimArray4Pointers(const MultiDimArray< GUM_SCALAR* >* table,
                                               Instantiation* instantiation = 0);

  /**
   * @brief a specialized function for projecting a multiDimArray using a Sum
   * operator
   * @ingroup multidim_op_group
   *
   * The function projects a table over all its variables and, thus, returns
   * the sum of the values stored in the table
   *
   * @return the sum of the values stored in the multiDimArray
   *
   * @param table a pointer on the multidim table to be projected
   *
   * @param instantiation It shall always be 0 else instantiation is updated
   * with a meaningless value. In addition, setting instantiation to a value
   * different than 0 will induce more computations and thus will slow down the
   * projection.
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectSum on MultiDimImplementation. This operator will
   * take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR* projectSumMultiDimArray4Pointers(const MultiDimImplementation< GUM_SCALAR* >* table,
                                               Instantiation* instantiation = 0);

  /// @}
  // ==========================================================================
  /// @name Project Multiply Operators
  // ==========================================================================
  /// @{

  /**
   * @brief a function for projecting a multiDimArray using a Product operator
   * @ingroup multidim_op_group
   *
   * The function projects a table over all its variables and, thus, returns
   * the product of the values stored in the table
   *
   * @return the product of the values stored in the multiDimArray
   *
   * @param table a pointer on the multidim table to be projected
   *
   * @param instantiation It shall always be 0 else instantiation is updated
   * with a meaningless value. In addition, setting instantiation to a value
   * different than 0 will induce more computations and thus will slow down the
   * projection.
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectProduct on MultiDimImplementation. This operator
   * will take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR projectProductMultiDimArray(const MultiDimArray< GUM_SCALAR >* table,
                                         Instantiation*                     instantiation = 0);

  /**
   * @brief a function for projecting a multiDimArray using a Product operator
   * @ingroup multidim_op_group
   *
   * The function projects a table over all its variables and, thus, returns
   * the product of the values stored in the table
   *
   * @return the product of the values stored in the multiDimArray
   *
   * @param table a pointer on the multidim table to be projected
   *
   * @param instantiation It shall always be 0 else instantiation is updated
   * with a meaningless value. In addition, setting instantiation to a value
   * different than 0 will induce more computations and thus will slow down the
   * projection.
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectProduct on MultiDimImplementation. This operator
   * will take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR projectProductMultiDimArray(const MultiDimImplementation< GUM_SCALAR >* table,
                                         Instantiation* instantiation = 0);

  /**
   * @brief a function for projecting a multiDimArray using a Product operator
   * @ingroup multidim_op_group
   *
   * The function projects a table over all its variables and, thus, returns
   * the product of the values stored in the table
   *
   * @return the product of the values stored in the multiDimArray
   *
   * @param table a pointer on the multidim table to be projected
   *
   * @param instantiation It shall always be 0 else instantiation is updated
   * with a meaningless value. In addition, setting instantiation to a value
   * different than 0 will induce more computations and thus will slow down the
   * projection.
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectProduct on MultiDimImplementation. This operator
   * will take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR* projectProductMultiDimArray4Pointers(const MultiDimArray< GUM_SCALAR* >* table,
                                                   Instantiation* instantiation = 0);

  /**
   * @brief a function for projecting a multiDimArray using a Product operator
   * @ingroup multidim_op_group
   *
   * The function projects a table over all its variables and, thus, returns
   * the product of the values stored in the table
   *
   * @return the product of the values stored in the multiDimArray
   *
   * @param table a pointer on the multidim table to be projected
   *
   * @param instantiation It shall always be 0 else instantiation is updated
   * with a meaningless value. In addition, setting instantiation to a value
   * different than 0 will induce more computations and thus will slow down the
   * projection.
   *
   * @warning In practice, do not use this function but rather
   * operator/function projectProduct on MultiDimImplementation. This operator
   * will take care to select this function if it is appropriate for your
   * projection.
   */
  template < typename GUM_SCALAR >
  GUM_SCALAR*
     projectProductMultiDimArray4Pointers(const MultiDimImplementation< GUM_SCALAR* >* table,
                                          Instantiation* instantiation = 0);

  /// @}

  /// the function to be used to project a MultiDimImplementation using a Max
  template < typename GUM_SCALAR >
  GUM_SCALAR projectMax(const MultiDimImplementation< GUM_SCALAR >& table,
                        Instantiation*                              instantiation = 0);

  /// the function to be used to project a MultiDimImplementation using a Min
  template < typename GUM_SCALAR >
  GUM_SCALAR projectMin(const MultiDimImplementation< GUM_SCALAR >& table,
                        Instantiation*                              instantiation = 0);

  /// the function to be used to project a MultiDimImplementation using a sum
  template < typename GUM_SCALAR >
  GUM_SCALAR projectSum(const MultiDimImplementation< GUM_SCALAR >& table,
                        Instantiation*                              instantiation = 0);

  /// the function to be used to project a MultiDimImplementation using a
  /// Product
  template < typename GUM_SCALAR >
  GUM_SCALAR projectProduct(const MultiDimImplementation< GUM_SCALAR >& table,
                            Instantiation*                              instantiation = 0);

  // ==========================================================================
  // DO NOT FORGET TO REGISTER YOUR PROJECTION FUNCTIONS AT THE END OF FILE
  // completeProjections4MultiDim_tpl.h
  // ==========================================================================

  /**
   * @brief the function used to register all the complete projection operators
   * on multidimImplementations over non-pointers types.
   * @ingroup multidim_op_group
   */
  template < typename GUM_SCALAR >
  void completeProjections4MultiDimInit();

  /**
   * @brief The function used to register all the complete projections on
   * multidimImplementations over pointers types.
   * @ingroup multidim_op_group
   */
  template < typename GUM_SCALAR >
  void pointerCompleteProjections4MultiDimInit();

  /**
   * @brief a class used to register complete projections over non-pointers
   * types
   * @ingroup multidim_op_group
   *
   * This class is of course completely redundant with function
   * completeProjections4MultiDimInit. Its aim is to enable specialization of
   * function completeProjections4MultiDimInit for pointer types: C++ allows
   * partial specialization of templated classes (e.g., create different
   * implementations for C<T> and C<T*>) but it does not allows partial
   * specialization for functions. Hence, by creating a class the primary
   * purpose of which is to run function completeProjections4MultiDimInit, we
   * allow this partial specialization. This is most effective to produce
   * different codes for pointer types and non-pointer types.
   */
  template < typename GUM_SCALAR >
  struct CompleteProjections4MultiDimInitialize {
    void init() { completeProjections4MultiDimInit< GUM_SCALAR >(); };
  };

  /**
   * @brief a class used to register complete projections over pointers types
   * @ingroup multidim_op_group
   *
   * This class is of course completely redundant with function
   * pointerCompleteProjections4MultiDimInit. Its aim is to enable different
   * implementations of the projections for multidims depending in whether
   * these multidim contain pointers or not. Actually, C++ allows partial
   * specialization of templated classes (e.g., create different
   * implementations for C<T> and C<T*>) but it does not allows partial
   * specialization for functions. Hence, by creating a class the primary
   * purpose of which is to run function completeProjections4MultiDimInit or
   * pointerCompleteProjections4MultiDimInit, we allow this partial
   * specialization to obtain.
   * */
  template < typename GUM_SCALAR >
  struct CompleteProjections4MultiDimInitialize< GUM_SCALAR* > {
    void init() { pointerCompleteProjections4MultiDimInit< GUM_SCALAR >(); };
  };

} /* namespace gum */

// always include the templatized implementations
#include <agrum/tools/multidim/utils/operators/completeProjections4MultiDim_tpl.h>

#endif /* GUM_COMPLETE_PROJECTIONS_4_MULTI_DIM_H */
