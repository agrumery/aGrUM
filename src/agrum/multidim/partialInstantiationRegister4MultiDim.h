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
 * @brief A container for registering partial instantiations on
 * multiDimImplementations
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_PARTIAL_INSTANTIATION_REGISTER_4_MULTI_DIM_H
#define GUM_PARTIAL_INSTANTIATION_REGISTER_4_MULTI_DIM_H

#include <iostream>
#include <string>
#include <utility>
#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>
#include <agrum/variables/discreteVariable.h>

namespace gum {

  // the base object used by the partial instantiations
  template <typename GUM_SCALAR> class MultiDimImplementation;

  /* =========================================================================== */
  /* ===            GUM_MULTI_DIM_PARTIAL_INSTANTIATION_REGISTER             === */
  /* =========================================================================== */
  /** @class PartialInstantiationRegister4MultiDim
   * @brief A container for registering partial instantiation functions on
   * multiDimImplementations, i.e., functions assigning values to subsets of
   * the variables of some tables
   *
   * @ingroup multidim_group */
  /* =========================================================================== */
  template <typename GUM_SCALAR> class PartialInstantiationRegister4MultiDim {
    public:
    /// the type of functions used by the register
    typedef MultiDimImplementation<GUM_SCALAR> *(*PartialInstantiationPtr)(
        const MultiDimImplementation<GUM_SCALAR> *,
        const HashTable<const DiscreteVariable *, Idx> &);

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// adds a new entry into the register
    /** This method inserts a new function (\e newFunction) taking a multiDim of
     * type \e type_multidim (which actually inherit from MultiDimImplementation)
     * and a hashTable assigning to some variables their values (actually the
     * index of their values) in arguments.
     * This new function's purpose is to achieve the instantiation of the
     * variables of the hashtable within the multiDim. As such, it returns the
     * table without these variables.
     * @param instantiation_func_name describes the name of the instantiation
     * function
     * @param type_multidim the \e real type of the multiDim taken in argument
     * by function \e newFunction
     * @param newFunction a pointer to the new function to register. Note that
     * although \e newFunction actually performs an operation on multiDims of
     * type \e type_multidim, it should be declared as taking in argument
     * two MultiDimImplementations. This constraint is imposed by the C++ typing
     * system */
    void insert(const std::string &instantiation_func_name,
                const std::string &type_multidim,
                PartialInstantiationPtr newFunction);

    /// removes a given entry from the register
    /** removes the function, if any, that performs the instantiation described by
     * \e instantiation_func_name, and that takes in argument a multiDim of type
     * \e type_multidim
     * @param instantiation_func_name describes the name of the instantiation
     * performed by the function to remove.
     * @param type_multidim the \e real type of the multiDim taken in argument by
     * the function to remove */
    void erase(const std::string &instantiation_func_name,
               const std::string &type_multidim);

    /// indicates whether a given entry exists in the register
    /** indicates if the register contains a function that performs the
     * instantiation described by \e instantiation_func_name, and that takes in
     * argument a multiDim of type \e type_multidim.
     * @param instantiation_func_name describes the name of the instantiation
     * performed by the function we look for.
     * @param type_multidim the \e real type of the multiDim taken in argument by
     * the function we look for */
    bool exists(const std::string &instantiation_func_name,
                const std::string &type_multidim) const;

    /** @brief returns the specialized partial instantiation operator assigned to
     * a given type of MultiDimImplementation
     *
     * @returns the function, if any, that performs the partial instantiation
     * described by \e instantiation_name, and that takes in argument a multiDim
     * of type \e type_multidim
     * @param instantiation_func_name describes the name of the partial instantiation
     * performed by the function we look for.
     * @param type_multidim the \e real type of the multiDim taken in argument by
     * the function we look for
     * @throws NotFound exception is thrown if the operator we look for does not
     * exist within this register. */
    PartialInstantiationPtr get(const std::string &instantiation_func_name,
                                const std::string &type_multidim) const;

    /// @}

    // ############################################################################
    /// @name Named Constructors
    // ############################################################################
    /// @{

    /// a named constructor that constructs one and only one Register per data type
    /** Note that this constructor prevents the famous init order fiasco */
    static PartialInstantiationRegister4MultiDim &Register();

    /// @}

    private:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// Default constructor: creates an empty register
    PartialInstantiationRegister4MultiDim();

    /// copy operator: never to be used
    PartialInstantiationRegister4MultiDim(
        const PartialInstantiationRegister4MultiDim &);

    /// destructor
    ~PartialInstantiationRegister4MultiDim();

    /// @}

    /// the set of associations for a given partial instantiation type
    typedef HashTable<std::string, PartialInstantiationPtr> PartialInstantiationSet;

    /** @brief a mapping from the types of MultiDimImplementations to partial
     * instantiation operators
     *
     * In this type, the strings represent the very types of the
     * MultiDimImplementations that will be combined. Hence, to a subtype of
     * MultiDimImplementation is associated a function to partially instantiate
     * this subtype of hypermatrix (the PartialInstantiationPtr) */
    HashTable<std::string, PartialInstantiationSet *> __set;
  };

  /// a function to more easily register new instantiation functions in MultiDims

  template <typename GUM_SCALAR>
  void
  registerPartialInstantiation(const std::string &instantiation_func_name,
                               const std::string &type_multidim,
                               typename PartialInstantiationRegister4MultiDim<
                                   GUM_SCALAR>::PartialInstantiationPtr function);

  // a display operator for PartialInstantiationPtrs

} /* namespace gum */

// always include the implementations
#include <agrum/multidim/partialInstantiationRegister4MultiDim.tcc>

#endif /* GUM_PARTIAL_INSTANTIATION_REGISTER_MULTI_DIM_H */
