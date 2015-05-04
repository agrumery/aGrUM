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
 * @brief A container for registering projection functions on multiDimImplementations
 *
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_PROJECTION_REGISTER_4_MULTI_DIM_H
#define GUM_PROJECTION_REGISTER_4_MULTI_DIM_H

#include <iostream>
#include <string>
#include <utility>
#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>
#include <agrum/variables/discreteVariable.h>

namespace gum {

  // the base object used by the projections
  template <typename GUM_SCALAR> class MultiDimImplementation;

  /* =========================================================================== */
  /* ===                  GUM_MULTI_DIM_PROJECTION_REGISTER                  === */
  /* =========================================================================== */
  /** @class ProjectionRegister4MultiDim
   * @brief A container for registering projection functions on
   * multiDimImplementations, i.e., functions projecting tables over a subset of
   * their variables
   *
   * @ingroup multidim_group */
  /* =========================================================================== */
  template <typename GUM_SCALAR> class ProjectionRegister4MultiDim {
    public:
    /// the type of functions used by the register
    typedef MultiDimImplementation<GUM_SCALAR> *(*ProjectionPtr)(
        const MultiDimImplementation<GUM_SCALAR> *,
        const Set<const DiscreteVariable *> &);

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// adds a new entry into the register
    /** This method inserts a new function (\e newFunction) taking a multiDim of
     * type \e type_multidim (which actually inherit from MultiDimImplementation)
     * and a set of discrete variables (to remove from the multidim) in arguments.
     * This new function's purpose is to achieve the projection described by
     * \e projection_name. For instance, if projection_name is "max", "min", "sum"
     * or "product", the new function performs the usual algebraic operations.
     * @param projection_name describes the name of the operation performed by
     * newFunction. Usual operation names are "min", "max", "sum", "product"
     * @param type_multidim the \e real type of the multiDim taken in argument
     * by function \e newFunction
     * @param newFunction a pointer to the new function to register. Note that
     * although \e newFunction actually performs an operation on multiDims of
     * type \e type_multidim, it should be declared as taking in argument
     * two MultiDimImplementations. This constraint is imposed by the C++ typing
     * system */
    void insert(const std::string &projection_name, const std::string &type_multidim,
                ProjectionPtr newFunction);

    /// removes a given entry from the register
    /** removes the function, if any, that performs the projection described by
     * \e projection_name, and that takes in argument a multiDim of type
     * \e type_multidim
     * @param projection_name describes the name of the projection performed by
     * the function to remove. Usual projection names are "min", "max", "sum",
     * "product"
     * @param type_multidim the \e real type of the multiDim taken in argument by
     * the function to remove */
    void erase(const std::string &projection_name, const std::string &type_multidim);

    /// indicates whether a given entry exists in the register
    /** indicates if the register contains a function that performs the projection
     * described by \e projection_name, and that takes in argument a multiDim of
     * type \e type_multidim.
     * @param projection_name describes the name of the projection performed by
     * the function we look for. Usual projection names are "min", "max", "sum",
     * "product"
     * @param type_multidim the \e real type of the multiDim taken in argument by
     * the function we look for */
    bool exists(const std::string &projection_name,
                const std::string &type_multidim) const;

    /** @brief returns the specialized projection operator assigned to a given type
     * of MultiDimImplementation
     *
     * returns the function, if any, that performs the projection described by
     * \e projection_name, and that takes in argument a multiDim of type
     * \e type_multidim
     * @param projection_name describes the name of the projection performed by
     * the function we look for. Usual projection names are "min", "max", "sum",
     * "product"
     * @param type_multidim the \e real type of the multiDim taken in argument by
     * the function we look for
     * @throws NotFound exception is thrown if the operator we look for does not
     * exist within this register. */
    ProjectionPtr get(const std::string &projection_name,
                      const std::string &type_multidim) const;

    /// @}

    // ############################################################################
    /// @name Named Constructors
    // ############################################################################
    /// @{

    /// a named constructor that constructs one and only one Register per data type
    /** Note that this constructor prevents the famous init order fiasco */
    static ProjectionRegister4MultiDim &Register();

    /// @}

    private:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// Default constructor: creates an empty register
    ProjectionRegister4MultiDim();

    /// copy operator: never to be used
    ProjectionRegister4MultiDim(const ProjectionRegister4MultiDim &);

    /// destructor
    ~ProjectionRegister4MultiDim();

    /// @}

    /// the set of associations for a given projection type
    typedef HashTable<std::string, ProjectionPtr> ProjectionSet;

    /// a mapping from the types of MultiDimImplementations to projection operators
    /** In this type, the strings represent the very types of the
     * MultiDimImplementations that will be combined. Hence, to a subtype of
     * MultiDimImplementation is associated a function to project this subtype of
     * hypermatrix (the ProjectionPtr) */
    HashTable<std::string, ProjectionSet *> __set;
  };

  /// a function to more easily register new projection functions in MultiDims

  template <typename GUM_SCALAR>
  void registerProjection(
      const std::string &projection_name, const std::string &type_multidim,
      typename ProjectionRegister4MultiDim<GUM_SCALAR>::ProjectionPtr function);

  // a display operator for ProjectionPtrs

} /* namespace gum */

// always include the implementations
#include <agrum/multidim/operators/projectionRegister4MultiDim.tcc>

#endif /* GUM_PROJECTION_REGISTER_MULTI_DIM_H */
