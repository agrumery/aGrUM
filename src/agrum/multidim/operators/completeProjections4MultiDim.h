/***************************************************************************
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
/** @file
 * @brief Efficient functionals for projecting multidim tables over all their
 * variables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_COMPLETE_PROJECTIONS_4_MULTI_DIM_H
#define GUM_COMPLETE_PROJECTIONS_4_MULTI_DIM_H

#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/multiDimDecisionDiagram.h>
#include <agrum/multidim/multiDimDecorator.h>
#include <agrum/multidim/instantiation.h>

namespace gum {

  /// a specialized function for projecting a multiDimArray using a Max operator
  /** The function projects a table over all its variables and, thus, returns
   * the max value stored in the table
   * @return the max value stored in the multiDimArray
   * @param table a pointer on the multidim table to be projected
   * @param instantiation if instantiation is different from 0, then the
   * projection function stores in the instantation the values of the
   * variables that led to the max (hence, the argmax).
   * @warning In practice, do not use this function but rather operator/function
   * projectMax on multiDimDecorators. This operator will take care to
   * select this function if it is appropriate for your projection. */
  template <typename GUM_SCALAR>
  GUM_SCALAR projectMaxMultiDimArray(const MultiDimArray<GUM_SCALAR> *table,
                                     Instantiation *instantiation = 0);

  template <typename GUM_SCALAR>
  GUM_SCALAR projectMaxMultiDimArray(const MultiDimImplementation<GUM_SCALAR> *table,
                                     Instantiation *instantiation = 0);

  template <typename GUM_SCALAR>
  GUM_SCALAR *
  projectMaxMultiDimArray4Pointers(const MultiDimArray<GUM_SCALAR *> *table,
                                   Instantiation *instantiation = 0);

  template <typename GUM_SCALAR>
  GUM_SCALAR *
  projectMaxMultiDimArray4Pointers(const MultiDimImplementation<GUM_SCALAR *> *table,
                                   Instantiation *instantiation = 0);

  /// a specialized function for projecting a multiDimArray using a Min operator
  /** The function projects a table over all its variables and, thus, returns
   * the min value stored in the table
   * @return the min value stored in the multiDimArray
   * @param table a pointer on the multidim table to be projected
   * @param instantiation if instantiation is different from 0, then the
   * projection function stores in the instantation the values of the
   * variables that led to the min (hence, the argmin).
   * @warning In practice, do not use this function but rather operator/function
   * projectMin on multiDimDecorators. This operator will take care to
   * select this function if it is appropriate for your projection. */
  template <typename GUM_SCALAR>
  GUM_SCALAR projectMinMultiDimArray(const MultiDimArray<GUM_SCALAR> *table,
                                     Instantiation *instantiation = 0);

  template <typename GUM_SCALAR>
  GUM_SCALAR projectMinMultiDimArray(const MultiDimImplementation<GUM_SCALAR> *table,
                                     Instantiation *instantiation = 0);

  template <typename GUM_SCALAR>
  GUM_SCALAR *
  projectMinMultiDimArray4Pointers(const MultiDimArray<GUM_SCALAR *> *table,
                                   Instantiation *instantiation = 0);

  template <typename GUM_SCALAR>
  GUM_SCALAR *
  projectMinMultiDimArray4Pointers(const MultiDimImplementation<GUM_SCALAR *> *table,
                                   Instantiation *instantiation = 0);

  /// a specialized function for projecting a multiDimArray using a Sum operator
  /** The function projects a table over all its variables and, thus, returns
   * the sum of the values stored in the table
   * @return the sum of the values stored in the multiDimArray
   * @param table a pointer on the multidim table to be projected
   * @param instantiation It shall always be 0 else instantiation is updated with
   * a meaningless value. In addition, setting instantiation to a value different
   * than 0 will induce more computations and thus will slow down the projection.
   * @warning In practice, do not use this function but rather operator/function
   * projectSum on multiDimDecorators. This operator will take care to
   * select this function if it is appropriate for your projection. */
  template <typename GUM_SCALAR>
  GUM_SCALAR projectSumMultiDimArray(const MultiDimArray<GUM_SCALAR> *table,
                                     Instantiation *instantiation = 0);

  template <typename GUM_SCALAR>
  GUM_SCALAR projectSumMultiDimArray(const MultiDimImplementation<GUM_SCALAR> *table,
                                     Instantiation *instantiation = 0);

  template <typename GUM_SCALAR>
  GUM_SCALAR *
  projectSumMultiDimArray4Pointers(const MultiDimArray<GUM_SCALAR *> *table,
                                   Instantiation *instantiation = 0);

  template <typename GUM_SCALAR>
  GUM_SCALAR *
  projectSumMultiDimArray4Pointers(const MultiDimImplementation<GUM_SCALAR *> *table,
                                   Instantiation *instantiation = 0);

  /// a function for projecting a multiDimArray using a Product operator
  /** The function projects a table over all its variables and, thus, returns
   * the product of the values stored in the table
   * @return the product of the values stored in the multiDimArray
   * @param table a pointer on the multidim table to be projected
   * @param instantiation It shall always be 0 else instantiation is updated with
   * a meaningless value. In addition, setting instantiation to a value different
   * than 0 will induce more computations and thus will slow down the projection.
   * @warning In practice, do not use this function but rather operator/function
   * projectProduct on multiDimDecorators. This operator will take care to
   * select this function if it is appropriate for your projection. */
  template <typename GUM_SCALAR>
  GUM_SCALAR projectProductMultiDimArray(const MultiDimArray<GUM_SCALAR> *table,
                                         Instantiation *instantiation = 0);

  template <typename GUM_SCALAR>
  GUM_SCALAR
  projectProductMultiDimArray(const MultiDimImplementation<GUM_SCALAR> *table,
                              Instantiation *instantiation = 0);

  template <typename GUM_SCALAR>
  GUM_SCALAR *
  projectProductMultiDimArray4Pointers(const MultiDimArray<GUM_SCALAR *> *table,
                                       Instantiation *instantiation = 0);

  template <typename GUM_SCALAR>
  GUM_SCALAR *projectProductMultiDimArray4Pointers(
      const MultiDimImplementation<GUM_SCALAR *> *table,
      Instantiation *instantiation = 0);

  // the operators that should be used to select appropriately the functions
  // to project multiDimDecorators

  /// the function to be used to project a MultiDimImplementation using a Max
  template <typename GUM_SCALAR>
  GUM_SCALAR projectMax(const MultiDimImplementation<GUM_SCALAR> &table,
                        Instantiation *instantiation = 0);

  /// the function to be used to project a MultiDimDecorator using a Max
  template <typename GUM_SCALAR>
  GUM_SCALAR projectMax(const MultiDimDecorator<GUM_SCALAR> &,
                        Instantiation *instantiation = 0);

  /// the function to be used to project a MultiDimImplementation using a Min
  template <typename GUM_SCALAR>
  GUM_SCALAR projectMin(const MultiDimImplementation<GUM_SCALAR> &table,
                        Instantiation *instantiation = 0);

  /// the function to be used to project a MultiDimDecorator using a Min
  template <typename GUM_SCALAR>
  GUM_SCALAR projectMin(const MultiDimDecorator<GUM_SCALAR> &,
                        Instantiation *instantiation = 0);

  /// the function to be used to project a MultiDimImplementation using a sum
  template <typename GUM_SCALAR>
  GUM_SCALAR projectSum(const MultiDimImplementation<GUM_SCALAR> &table,
                        Instantiation *instantiation = 0);

  /// the function to be used to project a MultiDimDecorator using a sum
  template <typename GUM_SCALAR>
  GUM_SCALAR projectSum(const MultiDimDecorator<GUM_SCALAR> &,
                        Instantiation *instantiation = 0);

  /// the function to be used to project a MultiDimImplementation using a Product
  template <typename GUM_SCALAR>
  GUM_SCALAR projectProduct(const MultiDimImplementation<GUM_SCALAR> &table,
                            Instantiation *instantiation = 0);

  /// the function to be used to project a MultiDimDecorator using a Product
  template <typename GUM_SCALAR>
  GUM_SCALAR projectProduct(const MultiDimDecorator<GUM_SCALAR> &,
                            Instantiation *instantiation = 0);

  //
  // DO NOT FORGET TO REGISTER YOUR PROJECTION FUNCTIONS AT THE END OF FILE
  // completeProjections4MultiDim.tcc
  //

  /** @brief the function used to register all the complete projection
   * operators on multidimImplementations over non-pointers types */
  template <typename GUM_SCALAR> void completeProjections4MultiDimInit();

  /** @brief the function used to register all the complete projections on
   * multidimImplementations over pointers types */
  template <typename GUM_SCALAR> void pointerCompleteProjections4MultiDimInit();

  /// a class used to register complete projections over non-pointers types
  /** This class is of course completely redundant with function
   * completeProjections4MultiDimInit. Its aim is to enable specialization of
   * function completeProjections4MultiDimInit for pointer types: C++ allows
   * partial specialization of templated classes (e.g., create different
   * implementations for C<T> and C<T*>) but it does not allows partial
   * specialization for functions. Hence, by creating a class the primary purpose
   * of which is to run function completeProjections4MultiDimInit, we allow this
   * partial specialization. This is most effective to produce different codes
   * for pointer types and non-pointer types. */
  template <typename GUM_SCALAR> struct CompleteProjections4MultiDimInitialize {
    void init() { completeProjections4MultiDimInit<GUM_SCALAR>(); };
  };

  /// a class used to register complete projections over pointers types
  /** This class is of course completely redundant with function
   * pointerCompleteProjections4MultiDimInit. Its aim is to enable different
   * implementations of the projections for multidims depending in whether these
   * multidim contain pointers or not. Actually, C++ allows partial specialization
   * of templated classes (e.g., create different implementations for C<T> and
   * C<T*>) but it does not allows partial specialization for functions. Hence,
   * by creating a class the primary purpose of which is to run function
   * completeProjections4MultiDimInit or pointerCompleteProjections4MultiDimInit,
   * we allow this partial specialization to obtain. */
  template <typename GUM_SCALAR>
  struct CompleteProjections4MultiDimInitialize<GUM_SCALAR *> {
    void init() { pointerCompleteProjections4MultiDimInit<GUM_SCALAR>(); };
  };

} /* namespace gum */

// always include the templatized implementations
#include <agrum/multidim/operators/completeProjections4MultiDim.tcc>

#endif /* GUM_COMPLETE_PROJECTIONS_4_MULTI_DIM_H */
