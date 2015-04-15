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
 * @brief Efficient functionals for instantiating some variables in
 * multiDimensional tables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_PARTIAL_INSTANTIATION_4_MULTI_DIM_H
#define GUM_PARTIAL_INSTANTIATION_4_MULTI_DIM_H

#include <agrum/core/set.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/multiDimDecisionDiagram.h>
#include <agrum/multidim/multiDimDecorator.h>

namespace gum {

  /// a specialized function for instantiating variables in a multiDimArray
  /** The function instantiates some variables in a multiDimArray. For instance,
   * if table is a MultiDimArray defined over A,B,C,D and if inst_vars =
   * {<A=a1>,<C=c2>}, then the function returns a table over B,D defined as
   * table (a1,B,c2,D).
   * @return function partialInstantiationMultiDimArray allocates on the heap the
   * new T multiDimArray and returns it
   * @warning In practice, do not use this function but rather operator/function
   * partialInstantiation on multiDimDecorators. This operator will take care to
   * select this function if it is appropriate for your instantiation. */
  template <typename GUM_SCALAR>
  MultiDimArray<GUM_SCALAR> *partialInstantiationMultiDimArray(
      const MultiDimArray<GUM_SCALAR> *table,
      const HashTable<const DiscreteVariable *, Idx> &inst_vars);

  template <typename GUM_SCALAR>
  MultiDimImplementation<GUM_SCALAR> *partialInstantiationMultiDimArray(
      const MultiDimImplementation<GUM_SCALAR> *table,
      const HashTable<const DiscreteVariable *, Idx> &inst_vars);

  template <typename GUM_SCALAR>
  MultiDimArray<GUM_SCALAR *> *partialInstantiationMultiDimArray4Pointers(
      const MultiDimArray<GUM_SCALAR *> *table,
      const HashTable<const DiscreteVariable *, Idx> &inst_vars);

  template <typename GUM_SCALAR>
  MultiDimImplementation<GUM_SCALAR *> *partialInstantiationMultiDimArray4Pointers(
      const MultiDimImplementation<GUM_SCALAR *> *table,
      const HashTable<const DiscreteVariable *, Idx> &inst_vars);

  // the operators that should be used to select appropriately the functions
  // to instantiate multiDimDecorators

  /// the function to be used to instantiate a MultiDimImplementation
  template <typename GUM_SCALAR>
  MultiDimImplementation<GUM_SCALAR> *
  partialInstantiation(const MultiDimImplementation<GUM_SCALAR> &table,
                       const HashTable<const DiscreteVariable *, Idx> &inst_vars);

  /// the function to be used to instantiate a MultiDimDecorator
  template <typename GUM_SCALAR>
  MultiDimImplementation<GUM_SCALAR> *
  partialInstantiation(const MultiDimDecorator<GUM_SCALAR> &table,
                       const HashTable<const DiscreteVariable *, Idx> &inst_vars);

  //
  // DO NOT FORGET TO REGISTER YOUR PARTIAL INSTANTIATION FUNCTIONS AT THE END
  // OF FILE partialInstantiation4MultiDim.tcc
  //

  /** @brief the function used to register all the instantiation operators on
   * multidimImplementations over non-pointers types */
  template <typename GUM_SCALAR> void partialInstantiation4MultiDimInit();

  /** @brief the function used to register all the instantiations on
   * multidimImplementations over pointers types */
  template <typename GUM_SCALAR> void pointerPartialInstantiation4MultiDimInit();

  /// a class used to register instantiation functions over non-pointers types
  /** This class is of course completely redundant with function
   * partialInstantiation4MultiDimInit. Its aim is to enable specialization of
   * function partialInstantiation4MultiDimInit for pointer types: C++ allows
   * partial specialization of templated classes (e.g., create different
   * implementations for C<T> and C<T*>) but it does not allows partial
   * specialization for functions. Hence, by creating a class the primary purpose
   * of which is to run function partialInstantiation4MultiDimInit, we allow this
   * partial specialization. This is most effective to produce different codes
   * for pointer types and non-pointer types. */
  template <typename GUM_SCALAR> struct PartialInstantiation4MultiDimInitialize {
    void init() { partialInstantiation4MultiDimInit<GUM_SCALAR>(); };
  };

  /// a class used to register instantiation functions over pointers types
  /** This class is of course completely redundant with function
   * pointerPartialInstantiation4MultiDimInit. Its aim is to enable different
   * implementations of the instantiation functions for multidims depending in
   * whether these multidim contain pointers or not. Actually, C++ allows partial
   * specialization of templated classes (e.g., create different implementations
   * for C<T> and C<T*>) but it does not allows partial specialization for
   * functions. Hence, by creating a class the primary purpose of which is to run
   * function partialInstantiation4MultiDimInit or
   * pointerPartialInstantiation4MultiDimInit, we allow this
   * partial specialization to obtain. */
  template <typename GUM_SCALAR>
  struct PartialInstantiation4MultiDimInitialize<GUM_SCALAR *> {
    void init() { pointerPartialInstantiation4MultiDimInit<GUM_SCALAR>(); };
  };

} /* namespace gum */

// always include the templatized implementations
#include <agrum/multidim/partialInstantiation4MultiDim.tcc>

#endif /* GUM_PARTIAL_INSTANTIATION_4_MULTI_DIM_H */
