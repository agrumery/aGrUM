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
 * @brief Efficient functionals for combining multiDims
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_OPERATORS_4_MULTI_DIM_H
#define GUM_OPERATORS_4_MULTI_DIM_H

#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/multiDimDecisionDiagram.h>
#include <agrum/multidim/multiDimDecorator.h>
#include <agrum/core/timer.h>

namespace gum {

  /// a specialized function for summing two multiDimArrays
  /** The function produces a tensorial addition of t1 and t2, that is, if t1 and
   * t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively, function
   * add2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] + t2[D,B,E] for all A,B,C,D,E.
   * @return function add2MultiDimArrays allocates on the heap the
   * new T multiDimArray and returns it
   * @warning In practice, do not use this function but rather operator+ on
   * multiDimDecorators. This operator will take care to select this function
   * if it is appropriate for your addition. */
  template <typename T>
  MultiDimArray<T> *add2MultiDimArrays(const MultiDimArray<T> *t1,
                                       const MultiDimArray<T> *t2);

  template <typename T>
  MultiDimImplementation<T> *add2MultiDimArrays(const MultiDimImplementation<T> *t1,
                                                const MultiDimImplementation<T> *t2);

  template <typename T>
  MultiDimArray<T *> *add2MultiDimArrays4Pointers(const MultiDimArray<T *> *t1,
                                                  const MultiDimArray<T *> *t2);

  template <typename T>
  MultiDimImplementation<T *> *
  add2MultiDimArrays4Pointers(const MultiDimImplementation<T *> *t1,
                              const MultiDimImplementation<T *> *t2);

  template <typename T>
  MultiDimDecisionDiagramBase<T> *
  add2MultiDimDecisionDiagrams(const MultiDimDecisionDiagramBase<T> *t1,
                               const MultiDimDecisionDiagramBase<T> *t2);

  template <typename T>
  MultiDimImplementation<T> *
  add2MultiDimDecisionDiagrams(const MultiDimImplementation<T> *t1,
                               const MultiDimImplementation<T> *t2);

  /// a specialized function for subtracting two multiDimArrays
  /** The function produces a tensorial subtraction of t2 from t1, that is, if t1
   * and t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function subtract2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] - t2[D,B,E] for all A,B,C,D,E.
   * @return function subtract2MultiDimArrays allocates on the heap the new
   * T multiDimArray and returns it
   * @warning In practice, do not use this function but rather operator- on
   * multiDimDecorators. This operator will take care to select this function
   * if it is appropriate for your subtraction. */
  template <typename T>
  MultiDimArray<T> *subtract2MultiDimArrays(const MultiDimArray<T> *t1,
                                            const MultiDimArray<T> *t2);

  template <typename T>
  MultiDimImplementation<T> *
  subtract2MultiDimArrays(const MultiDimImplementation<T> *t1,
                          const MultiDimImplementation<T> *t2);

  template <typename T>
  MultiDimArray<T *> *subtract2MultiDimArrays4Pointers(const MultiDimArray<T *> *t1,
                                                       const MultiDimArray<T *> *t2);

  template <typename T>
  MultiDimImplementation<T *> *
  subtract2MultiDimArrays4Pointers(const MultiDimImplementation<T *> *t1,
                                   const MultiDimImplementation<T *> *t2);

  template <typename T>
  MultiDimDecisionDiagramBase<T> *
  subtract2MultiDimDecisionDiagrams(const MultiDimDecisionDiagramBase<T> *t1,
                                    const MultiDimDecisionDiagramBase<T> *t2);

  template <typename T>
  MultiDimImplementation<T> *
  subtract2MultiDimDecisionDiagrams(const MultiDimImplementation<T> *t1,
                                    const MultiDimImplementation<T> *t2);

  /// a specialized function for multiplying two multiDimArrays
  /** The function produces a tensorial product of t2 from t1, that is, if t1
   * and t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function multiply2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] * t2[D,B,E] for all A,B,C,D,E.
   * @return function multiply2MultiDimArrays allocates on the heap the new
   * T multiDimArray and returns it
   * @warning In practice, do not use this function but rather operator* on
   * multiDimDecorators. This operator will take care to select this function
   * if it is appropriate for your multiplication. */
  template <typename T>
  MultiDimArray<T> *multiply2MultiDimArrays(const MultiDimArray<T> *t1,
                                            const MultiDimArray<T> *t2);

  template <typename T>
  MultiDimImplementation<T> *
  multiply2MultiDimArrays(const MultiDimImplementation<T> *t1,
                          const MultiDimImplementation<T> *t2);

  template <typename T>
  MultiDimArray<T *> *multiply2MultiDimArrays4Pointers(const MultiDimArray<T *> *t1,
                                                       const MultiDimArray<T *> *t2);

  template <typename T>
  MultiDimImplementation<T *> *
  multiply2MultiDimArrays4Pointers(const MultiDimImplementation<T *> *t1,
                                   const MultiDimImplementation<T *> *t2);

  template <typename T>
  MultiDimDecisionDiagramBase<T> *
  multiply2MultiDimDecisionDiagrams(const MultiDimDecisionDiagramBase<T> *t1,
                                    const MultiDimDecisionDiagramBase<T> *t2);

  template <typename T>
  MultiDimImplementation<T> *
  multiply2MultiDimDecisionDiagrams(const MultiDimImplementation<T> *t1,
                                    const MultiDimImplementation<T> *t2);

  /// a specialized function for dividing two multiDimArrays
  /** The function produces a tensorial division of t1 by t2, that is, if t1
   * and t2 are multiDimArrays over variables A,B,C,D and D,B,E respectively,
   * function divide2MultiDimArrays will return a multiDimArray T defined as
   * T[A,B,C,D,E] = t1[A,B,C,D] / t2[D,B,E] for all A,B,C,D,E.
   * @return function divide2MultiDimArrays allocates on the heap the new
   * T multiDimArray and returns it
   * @warning In practice, do not use this function but rather operator/ on
   * multiDimDecorators. This operator will take care to select this function
   * if it is appropriate for your division. */
  template <typename T>
  MultiDimArray<T> *divide2MultiDimArrays(const MultiDimArray<T> *t1,
                                          const MultiDimArray<T> *t2);

  template <typename T>
  MultiDimImplementation<T> *
  divide2MultiDimArrays(const MultiDimImplementation<T> *t1,
                        const MultiDimImplementation<T> *t2);

  template <typename T>
  MultiDimArray<T *> *divide2MultiDimArrays4Pointers(const MultiDimArray<T *> *t1,
                                                     const MultiDimArray<T *> *t2);

  template <typename T>
  MultiDimImplementation<T *> *
  divide2MultiDimArrays4Pointers(const MultiDimImplementation<T *> *t1,
                                 const MultiDimImplementation<T *> *t2);

  template <typename T>
  MultiDimDecisionDiagramBase<T> *
  divide2MultiDimDecisionDiagrams(const MultiDimDecisionDiagramBase<T> *t1,
                                  const MultiDimDecisionDiagramBase<T> *t2);

  template <typename T>
  MultiDimImplementation<T> *
  divide2MultiDimDecisionDiagrams(const MultiDimImplementation<T> *t1,
                                  const MultiDimImplementation<T> *t2);

  /// a specialized function for compare value from two multiDimDecisionDiagramBases
  /// and gets the maximal
  /** The function produces a tensorial comparison between t1 and t2, that is, if t1
   * and t2 are multiDimDecisionDiagramBases over variables A,B,C,D and D,B,E
   * respectively,
   * function maximise2MultiDimDecisionDiagramBases will return a
   * multiDimDecisionDiagramBase T defined as
   * T[A,B,C,D,E] = max( t1[A,B,C,D], t2[D,B,E] ) for all A,B,C,D,E.
   * @return function maximise2MultiDimDecisionDiagramBases allocates on the heap the
   * new
   * T multiDimDecisionDiagramBase and returns it
   */
  template <typename T>
  MultiDimDecisionDiagramBase<T> *
  maximise2MultiDimDecisionDiagrams(const MultiDimDecisionDiagramBase<T> *t1,
                                    const MultiDimDecisionDiagramBase<T> *t2);

  template <typename T>
  MultiDimImplementation<T> *
  maximise2MultiDimDecisionDiagrams(const MultiDimImplementation<T> *t1,
                                    const MultiDimImplementation<T> *t2);

  /// a specialized function for functionally combining two multiDimArrays
  /** The function produces a tensorial functional combination of t1 and t2, that
   * is, if t1 and t2 are multiDimArrays over variables A,B,C,D and D,B,E
   * respectively, function combine2MultiDimArrays will return a multiDimArray T
   * defined as T[A,B,C,D,E] = f ( t1[A,B,C,D],t2[D,B,E] ) for all A,B,C,D,E.
   * @return function combine2MultiDimArrays allocates on the heap the new
   * T multiDimArray and returns it */
  //   template<typename T>
  //   MultiDimArray<T>*
  //   combine2MultiDimArrays ( const MultiDimArray<T>* t1,
  //                            const MultiDimArray<T>* t2,
  //                            const T (*f) ( const T&, const T&) );
  //
  //   template<typename T>
  //   MultiDimImplementation<T>*
  //   combine2MultiDimArrays ( const MultiDimImplementation<T>* t1,
  //                            const MultiDimImplementation<T>* t2,
  //                            const T (*f) ( const T&, const T&) );

  // the operators that should be used to select appropriately the functions
  // to combine multiDimDecorators

  /// the function to be used to add two MultiDimImplementations
  template <typename T>
  MultiDimImplementation<T> *operator+(const MultiDimImplementation<T> &,
                                       const MultiDimImplementation<T> &);

  /// the function to be used to add two MultiDimDecorators
  template <typename T>
  MultiDimImplementation<T> *operator+(const MultiDimDecorator<T> &,
                                       const MultiDimDecorator<T> &);

  /// the function to be used to subtract two MultiDimImplementations
  template <typename T>
  MultiDimImplementation<T> *operator-(const MultiDimImplementation<T> &,
                                       const MultiDimImplementation<T> &);

  /// the function to be used to subtract two MultiDimDecorators
  template <typename T>
  MultiDimImplementation<T> *operator-(const MultiDimDecorator<T> &,
                                       const MultiDimDecorator<T> &);

  /// the function to be used to multiply two MultiDimImplementations
  template <typename T>
  MultiDimImplementation<T> *operator*(const MultiDimImplementation<T> &,
                                       const MultiDimImplementation<T> &);

  /// the function to be used to multiply two MultiDimDecorators
  template <typename T>
  MultiDimImplementation<T> *operator*(const MultiDimDecorator<T> &,
                                       const MultiDimDecorator<T> &);

  /// the function to be used to divide two MultiDimImplementations
  template <typename T>
  MultiDimImplementation<T> *operator/(const MultiDimImplementation<T> &,
                                       const MultiDimImplementation<T> &);

  /// the function to be used to divide two MultiDimDecorators
  template <typename T>
  MultiDimImplementation<T> *operator/(const MultiDimDecorator<T> &,
                                       const MultiDimDecorator<T> &);

  //
  // DO NOT FORGET TO REGISTER YOUR BINARY FUNCTIONS AT THE END OF FILE
  // operators4MultiDim.tcc
  //

  /** @brief the function used to register all the operators on
   * multidimImplementations over non-pointers types */
  template <typename GUM_SCALAR> void operators4MultiDimInit();

  /** @brief the function used to register all the operators on
   * multidimImplementations over pointers types */
  template <typename GUM_SCALAR> void pointerOperators4MultiDimInit();

  /// a class used to register operators over non-pointers types
  /** This class is of course completely redundant with function
   * operators4MultiDimInit. Its aim is to enable specialization of function
   * operators4MultiDimInit for pointer types: C++ allows partial specialization
   * of templated classes (e.g., create different implementations for C<T> and
   * C<T*>) but it does not allows partial specialization for functions. Hence,
   * by creating a class the primary purpose of which is to run function
   * operators4MultiDimInit, we allow this partial specialization. This is
   * most effective to produce different codes for pointer types and non-pointer
   * types. */
  template <typename GUM_SCALAR> struct Operators4MultiDimInitialize {
    void init() { operators4MultiDimInit<GUM_SCALAR>(); };
  };

  /// a class used to register operators over pointers types
  /** This class is of course completely redundant with function
   * pointerOperators4MultiDimInit. Its aim is to enable different implementations
   * of the operators for multidims depending in whether these multidim contain
   * pointers or not. Actually, C++ allows partial specialization
   * of templated classes (e.g., create different implementations for C<T> and
   * C<T*>) but it does not allows partial specialization for functions. Hence,
   * by creating a class the primary purpose of which is to run function
   * operators4MultiDimInit or pointerOperators4MultiDimInit, we allow this
   * partial specialization to obtain. */
  template <typename GUM_SCALAR> struct Operators4MultiDimInitialize<GUM_SCALAR *> {
    void init() { pointerOperators4MultiDimInit<GUM_SCALAR>(); };
  };

} /* namespace gum */

// always include the templatized implementations
#include <agrum/multidim/operators/operators4MultiDim.tcc>

#endif /* GUM_OPERATORS_4_MULTI_DIM_H */
