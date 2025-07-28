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
 * @brief A container for registering projection functions on
 * multiDimImplementations
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_PROJECTION_REGISTER_4_MULTI_DIM_H
#define GUM_PROJECTION_REGISTER_4_MULTI_DIM_H

#include <atomic>
#include <iostream>
#include <string>
#include <utility>

#include <agrum/base/variables/discreteVariable.h>

namespace gum {

  // the base object used by the projections
  template < typename GUM_SCALAR >
  class MultiDimImplementation;

  // ===========================================================================
  // ===                  GUM_MULTI_DIM_PROJECTION_REGISTER                  ===
  // ===========================================================================
  // clang-format off
  /**
   * @class ProjectionRegister4MultiDim
   * @headerfile projectionRegister4MultiDim.h <agrum/base/multidim/operators/projectionRegister4MultiDim.h>
   * @ingroup multidim_op_group
   *
   * @brief A container for registering projection functions on
   * multiDimImplementations, i.e., functions projecting tables over a subset
   * of their variables
   */
  // clang-format on

  template < typename GUM_SCALAR >
  class ProjectionRegister4MultiDim {
    public:
    /// the type of functions used by the register
    using ProjectionPtr
        = MultiDimImplementation< GUM_SCALAR >* (*)(const MultiDimImplementation< GUM_SCALAR >*,
                                                    const gum::VariableSet&);

    // ========================================================================
    /// @name Accessors / Modifiers
    // ========================================================================
    /// @{

    /**
     * @brief adds a new entry into the register
     *
     * This method inserts a new function (\e newFunction) taking a multiDim of
     * type \e type_multidim (which actually inherit from
     * MultiDimImplementation) and a set of discrete variables (to remove from
     * the multidim) in arguments.  This new function's purpose is to achieve
     * the projection described by \e projection_name. For instance, if
     * projection_name is "max", "min", "sum" or "product", the new function
     * performs the usual algebraic operations.
     *
     * @param projection_name describes the name of the operation performed by
     * newFunction. Usual operation names are "min", "max", "sum", "product"
     *
     * @param type_multidim the \e real type of the multiDim taken in argument
     * by function \e newFunction
     *
     * @param newFunction a pointer to the new function to register. Note that
     * although \e newFunction actually performs an operation on multiDims of
     * type \e type_multidim, it should be declared as taking in argument two
     * MultiDimImplementations. This constraint is imposed by the C++ typing
     * system
     */
    void insert(const std::string& projection_name,
                const std::string& type_multidim,
                ProjectionPtr      newFunction);

    /**
     * @brief removes a given entry from the register
     *
     * Removes the function, if any, that performs the projection described by
     * \e projection_name, and that takes in argument a multiDim of type \e
     * type_multidim
     *
     * @param projection_name describes the name of the projection performed by
     * the function to remove. Usual projection names are "min", "max", "sum",
     * "product"
     *
     * @param type_multidim the \e real type of the multiDim taken in argument
     * by the function to remove
     */
    void erase(const std::string& projection_name, const std::string& type_multidim);

    /**
     * @brief indicates whether a given entry exists in the register
     *
     * indicates if the register contains a function that performs the
     * projection described by \e projection_name, and that takes in argument a
     * multiDim of type \e type_multidim.
     *
     * @param projection_name describes the name of the projection performed by
     * the function we look for. Usual projection names are "min", "max",
     * "sum", "product"
     *
     * @param type_multidim the \e real type of the multiDim taken in argument
     * by the function we look for
     */
    bool exists(const std::string& projection_name, const std::string& type_multidim) const;

    /**
     * @brief returns the specialized projection operator assigned to a given
     * type of MultiDimImplementation
     *
     * Returns the function, if any, that performs the projection described by
     * \e projection_name, and that takes in argument a multiDim of type \e
     * type_multidim
     *
     * @param projection_name describes the name of the projection performed by
     * the function we look for. Usual projection names are "min", "max",
     * "sum", "product"
     *
     * @param type_multidim the \e real type of the multiDim taken in argument
     * by the function we look for
     *
     * @throws NotFound exception is thrown if the operator we look for does
     * not exist within this register.
     */
    ProjectionPtr get(const std::string& projection_name, const std::string& type_multidim) const;

    /// @}
    // ========================================================================
    /// @name Named Constructors
    // ========================================================================
    /// @{

    /**
     * @brief a named constructor that constructs one and only one Register per
     * data type
     *
     * Note that this constructor prevents the famous init order fiasco
     */
    static ProjectionRegister4MultiDim& Register();

    /// @}

    private:
    // ========================================================================
    /// @name Constructors / Destructors
    // ========================================================================
    /// @{

    /// Default constructor: creates an empty register
    ProjectionRegister4MultiDim();

    /// Copy operator: never to be used
    ProjectionRegister4MultiDim(const ProjectionRegister4MultiDim&);

    /// Destructor
    ~ProjectionRegister4MultiDim();

    /// @}

    /// The set of associations for a given projection type
    using ProjectionSet = HashTable< std::string, ProjectionPtr >;

    /**
     * @brief A mapping from the types of MultiDimImplementations to projection
     * operators
     *
     * In this type, the strings represent the very types of the
     * MultiDimImplementations that will be combined. Hence, to a subtype of
     * MultiDimImplementation is associated a function to project this subtype
     * of hypermatrix (the ProjectionPtr)
     */
    HashTable< std::string, ProjectionSet* > _set_;
  };

  /// A function to more easily register new projection functions in MultiDims
  template < typename GUM_SCALAR >
  void registerProjection(
      const std::string&                                                projection_name,
      const std::string&                                                type_multidim,
      typename ProjectionRegister4MultiDim< GUM_SCALAR >::ProjectionPtr function);

  // a display operator for ProjectionPtrs

} /* namespace gum */

// always include the implementations
#include <agrum/base/multidim/utils/operators/projectionRegister4MultiDim_tpl.h>

#endif /* GUM_PROJECTION_REGISTER_MULTI_DIM_H */
