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
 * @brief Headers for partial instantiation functions.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_PARTIAL_INSTANTIATION_4_MULTI_DIM_H
#define GUM_PARTIAL_INSTANTIATION_4_MULTI_DIM_H

#include <atomic>
#include <mutex>

#include <agrum/base/variables/discreteVariable.h>

// #include <agrum/base/multidim/implementations/multiDimImplementation.h>
// #include <agrum/base/multidim/implementations/multiDimArray.h>
// #include <agrum/base/multidim/implementations/multiDimDecorator.h>

namespace gum {

  template < typename GUM_SCALAR >
  class MultiDimImplementation;

  template < typename GUM_SCALAR >
  class MultiDimArray;

  template < typename GUM_SCALAR >
  class MultiDimDecorator;

  /**
   * @brief A specialized function for instantiating variables in a
   * multiDimArray.
   *
   * @warning In practice, do not use this function but rather
   * operator/function partialInstantiation on multiDimDecorators. This
   * operator will take care to select this function if it is appropriate for
   * your instantiation.
   *
   * The function instantiates some variables in a multiDimArray. For instance,
   * if table is a MultiDimArray defined over A,B,C,D and if inst_vars =
   * {<A=a1>,<C=c2>}, then the function returns a table over B,D defined as
   * table (a1,B,c2,D).
   *
   * @param table The table to instantiate.
   * @param inst_vars The variables instantiation.
   * @return Returns the partial instantiation of table as a new MultiDimArray.
   */
  template < typename GUM_SCALAR >
  MultiDimArray< GUM_SCALAR >*
      partialInstantiationMultiDimArray(const MultiDimArray< GUM_SCALAR >*               table,
                                        const HashTable< const DiscreteVariable*, Idx >& inst_vars);

  /**
   * @brief A specialized function for instantiating variables in a
   * MultiDimImplementation.
   *
   * @warning In practice, do not use this function but rather
   * operator/function partialInstantiation on multiDimDecorators. This
   * operator will take care to select this function if it is appropriate for
   * your instantiation.
   *
   * The function instantiates some variables in a multiDimArray. For instance,
   * if table is a MultiDimArray defined over A,B,C,D and if inst_vars =
   * {<A=a1>,<C=c2>}, then the function returns a table over B,D defined as
   * table (a1,B,c2,D).
   *
   * @param table The table to instantiate.
   * @param inst_vars The variables instantiation.
   * @return Returns the partial instantiation of table as a new
   * MultiDimImplementation.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >*
      partialInstantiationMultiDimArray(const MultiDimImplementation< GUM_SCALAR >*      table,
                                        const HashTable< const DiscreteVariable*, Idx >& inst_vars);

  /**
   * @brief A specialized function for instantiating variables in a
   * MultiDimArray.
   *
   * @warning In practice, do not use this function but rather
   * operator/function partialInstantiation on multiDimDecorators. This
   * operator will take care to select this function if it is appropriate for
   * your instantiation.
   *
   * The function instantiates some variables in a multiDimArray. For instance,
   * if table is a MultiDimArray defined over A,B,C,D and if inst_vars =
   * {<A=a1>,<C=c2>}, then the function returns a table over B,D defined as
   * table (a1,B,c2,D).
   *
   * @param table The table to instantiate.
   * @param inst_vars The variables instantiation.
   * @return Returns the partial instantiation of table as a new
   * MultiDimArray.
   */
  template < typename GUM_SCALAR >
  MultiDimArray< GUM_SCALAR* >* partialInstantiationMultiDimArray4Pointers(
      const MultiDimArray< GUM_SCALAR* >*              table,
      const HashTable< const DiscreteVariable*, Idx >& inst_vars);

  /**
   * @brief A specialized function for instantiating variables in a
   * MultiDimArray.
   *
   * @warning In practice, do not use this function but rather
   * operator/function partialInstantiation on multiDimDecorators. This
   * operator will take care to select this function if it is appropriate for
   * your instantiation.
   *
   * The function instantiates some variables in a multiDimArray. For instance,
   * if table is a MultiDimArray defined over A,B,C,D and if inst_vars =
   * {<A=a1>,<C=c2>}, then the function returns a table over B,D defined as
   * table (a1,B,c2,D).
   *
   * @param table The table to instantiate.
   * @param inst_vars The variables instantiation.
   * @return Returns the partial instantiation of table as a new
   * MultiDimImplementation.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR* >* partialInstantiationMultiDimArray4Pointers(
      const MultiDimImplementation< GUM_SCALAR* >*     table,
      const HashTable< const DiscreteVariable*, Idx >& inst_vars);

  /**
   * @brief Instantiate variables in a MultiDimImplementation.
   *
   * @param table The table to instantiate.
   * @param inst_vars The variables instantiation.
   * @return Returns the partial instantiation of table as a new
   * MultiDimImplementation.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >*
      partialInstantiation(const MultiDimImplementation< GUM_SCALAR >&      table,
                           const HashTable< const DiscreteVariable*, Idx >& inst_vars);

  /**
   * @brief Instantiate variables in a MultiDimDecorator.
   *
   * @param table The table to instantiate.
   * @param inst_vars The variables instantiation.
   * @return Returns the partial instantiation of table as a new
   * MultiDimImplementation.
   */
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >*
      partialInstantiation(const MultiDimDecorator< GUM_SCALAR >&           table,
                           const HashTable< const DiscreteVariable*, Idx >& inst_vars);

  // ==========================================================================
  // DO NOT FORGET TO REGISTER YOUR PARTIAL INSTANTIATION FUNCTIONS AT THE END
  // OF FILE partialInstantiation4MultiDim_tpl.h
  // ==========================================================================

  /**
   * @brief The function used to register all the instantiation operators on
   * multidimImplementations over non-pointers types.
   */
  template < typename GUM_SCALAR >
  void partialInstantiation4MultiDimInit();

  /**
   * @brief The function used to register all the instantiations on
   * multidimImplementations over pointers types.
   */
  template < typename GUM_SCALAR >
  void pointerPartialInstantiation4MultiDimInit();

  /**
   * @class PartialInstantiation4MultiDimInitialize
   * @brief A class used to register instantiation functions over non-pointers
   * types.
   *
   * This class is of course completely redundant with function
   * partialInstantiation4MultiDimInit. Its aim is to enable specialization of
   * function partialInstantiation4MultiDimInit for pointer types: C++ allows
   * partial specialization of templated classes (e.g., create different
   * implementations for C<T> and C<T*>) but it does not allows partial
   * specialization for functions. Hence, by creating a class the primary
   * purpose of which is to run function partialInstantiation4MultiDimInit, we
   * allow this partial specialization. This is most effective to produce
   * different codes for pointer types and non-pointer types.
   *
   * @tparam GUM_SCALAR The type of scalars stored in the multidimensional
   * table.
   */
  template < typename GUM_SCALAR >
  struct PartialInstantiation4MultiDimInitialize {
    /**
     * @brief Initialize the partial instantiation functions.
     */
    void init() { partialInstantiation4MultiDimInit< GUM_SCALAR >(); };
  };

  /**
   * @class PartialInstantiation4MultiDimInitialize
   * @brief A class used to register instantiation functions over pointers
   * types.
   *
   * This class is of course completely redundant with function
   * pointerPartialInstantiation4MultiDimInit. Its aim is to enable different
   * implementations of the instantiation functions for multidims depending in
   * whether these multidim contain pointers or not. Actually, C++ allows
   * partial specialization of templated classes (e.g., create different
   * implementations for C<T> and C<T*>) but it does not allows partial
   * specialization for functions. Hence, by creating a class the primary
   * purpose of which is to run function partialInstantiation4MultiDimInit or
   * pointerPartialInstantiation4MultiDimInit, we allow this partial
   * specialization to obtain.
   *
   * @tparam GUM_SCALAR The type of scalars stored in the multidimensional
   * table as pointer.
   */
  template < typename GUM_SCALAR >
  struct PartialInstantiation4MultiDimInitialize< GUM_SCALAR* > {
    /**
     * @brief Initialize the partial instantiation functions.
     */
    void init() { pointerPartialInstantiation4MultiDimInit< GUM_SCALAR >(); };
  };

} /* namespace gum */

// always include the templatized implementations
#include <agrum/base/multidim/utils/partialInstantiation4MultiDim_tpl.h>

#endif /* GUM_PARTIAL_INSTANTIATION_4_MULTI_DIM_H */
