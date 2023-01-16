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
 * @brief Headers for PartialInstantiationRegister4MultiDim.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_PARTIAL_INSTANTIATION_REGISTER_4_MULTI_DIM_H
#define GUM_PARTIAL_INSTANTIATION_REGISTER_4_MULTI_DIM_H

#include <agrum/tools/core/set.h>
#include <agrum/tools/variables/discreteVariable.h>
#include <iostream>
#include <string>
#include <utility>
#include <atomic>

namespace gum {

  // the base object used by the partial instantiations
  template < typename GUM_SCALAR >
  class MultiDimImplementation;

  // ==========================================================================
  // ===            GUM_MULTI_DIM_PARTIAL_INSTANTIATION_REGISTER            ===
  // ==========================================================================
  /**
   * @class PartialInstantiationRegister4MultiDim
   * @ingroup multidim_group
   *
   * @warning Doxygen does not like spanning command on multiple line, so we
   * could not configure it with the correct include directive. Use the
   * following code snippet to include this file.
   * @code
   * #include <agrum/tools/multidim/partialInstantiationRegister4MultiDim.h>
   * @endcode
   *
   * @brief A container for registering partial instantiation functions on
   * multiDimImplementations, i.e., functions assigning values to subsets of
   * the variables of some tables.
   *
   * @tparam GUM_SCALAR The type of scalars stored in the multidimensional
   * table.
   */
  template < typename GUM_SCALAR >
  class PartialInstantiationRegister4MultiDim {
    public:
    /// The type of functions used by the register
    using PartialInstantiationPtr
       = MultiDimImplementation< GUM_SCALAR >* (*)(const MultiDimImplementation< GUM_SCALAR >*,
                                                   const HashTable< const DiscreteVariable*,
                                                                    Idx >&);

    // ========================================================================
    /// @name Accessors / Modifiers
    // ========================================================================
    /// @{

    ///
    /**
     * @brief Adds a new entry into the register.
     *
     * This method inserts a new function (\e newFunction) taking a multiDim of
     * type \e type_multidim (which actually inherit from
     * MultiDimImplementation) and a hashTable assigning to some variables
     * their values (actually the index of their values) in arguments.  This
     * new function's purpose is to achieve the instantiation of the variables
     * of the hashtable within the multiDim. As such, it returns the table
     * without these variables.
     *
     * Note that although \e newFunction actually performs an operation on
     * multiDims of type \e type_multidim, it should be declared as taking in
     * argument two MultiDimImplementations. This constraint is imposed by the
     * C++ typing system.
     *
     * @param instantiation_func_name The name of the instantiation function.
     * @param type_multidim The \e real type of the multiDim taken in argument
     * by function \e newFunction.
     * @param newFunction A pointer to the new function to register.
     */
    void insert(const std::string&      instantiation_func_name,
                const std::string&      type_multidim,
                PartialInstantiationPtr newFunction);

    /**
     * @brief Removes a given entry from the register.
     *
     * Removes the function, if any, that performs the instantiation described
     * by \e instantiation_func_name, and that takes in argument a multiDim of
     * type \e type_multidim.
     *
     * @param instantiation_func_name The name of the instantiation performed
     * by the function to remove.
     * @param type_multidim The \e real type of the multiDim taken in argument
     * by the function to remove.
     */
    void erase(const std::string& instantiation_func_name, const std::string& type_multidim);

    /**
     * @brief Indicates whether a given entry exists in the register.
     *
     * Indicates if the register contains a function that performs the
     * instantiation described by \e instantiation_func_name, and that takes in
     * argument a multiDim of type \e type_multidim.
     *
     * @param instantiation_func_name The name of the instantiation performed
     * by the function we look for.
     * @param type_multidim The \e real type of the multiDim taken in argument
     * by the function we look for.
     * @return Returns true if a given entry exists in the register.
     */
    bool exists(const std::string& instantiation_func_name, const std::string& type_multidim) const;

    /**
     * @brief Returns the specialized partial instantiation operator assigned
     * to a given type of MultiDimImplementation.
     *
     * @param instantiation_func_name describes the name of the partial
     * instantiation performed by the function we look for.
     * @param type_multidim the \e real type of the multiDim taken in argument
     * by the function we look for
     * @returns Returns The function, if any, that performs the partial
     * instantiation
     * described by \e instantiation_name, and that takes in argument a multiDim
     * of type \e type_multidim.
     *
     * @throws NotFound Raised if the operator we look for does not exist
     * within this register.
     */
    PartialInstantiationPtr get(const std::string& instantiation_func_name,
                                const std::string& type_multidim) const;

    /// @}
    // =========================================================================
    /// @name Named Constructors
    // =========================================================================
    /// @{

    /**
     * @brief A named constructor that constructs one and only one Register per
     * data type.
     *
     * Note that this constructor prevents the famous init order fiasco.
     */
    static PartialInstantiationRegister4MultiDim& Register();

    /// @}

    private:
    // =========================================================================
    /// @name Constructors / Destructors
    // =========================================================================
    /// @{

    /**
     * @brief Default constructor: creates an empty register.
     */
    PartialInstantiationRegister4MultiDim();

    /**
     * @brief Copy operator: never to be used.
     */
    PartialInstantiationRegister4MultiDim(const PartialInstantiationRegister4MultiDim&);

    /**
     * @brief Class destructor.
     */
    ~PartialInstantiationRegister4MultiDim();

    /// @}

    /// The set of associations for a given partial instantiation type
    using PartialInstantiationSet = HashTable< std::string, PartialInstantiationPtr >;

    /**
     * @brief A mapping from the types of MultiDimImplementations to partial
     * instantiation operators.
     *
     * In this type, the strings represent the very types of the
     * MultiDimImplementations that will be combined. Hence, to a subtype of
     * MultiDimImplementation is associated a function to partially instantiate
     * this subtype of hypermatrix (the PartialInstantiationPtr).
     */
    HashTable< std::string, PartialInstantiationSet* > _set_;
  };

  /// A function to more easily register new instantiation functions in
  /// MultiDims
  template < typename GUM_SCALAR >
  void registerPartialInstantiation(
     const std::string& instantiation_func_name,
     const std::string& type_multidim,
     typename PartialInstantiationRegister4MultiDim< GUM_SCALAR >::PartialInstantiationPtr
        function);

} /* namespace gum */

// always include the implementations
#include <agrum/tools/multidim/utils/partialInstantiationRegister4MultiDim_tpl.h>

#endif /* GUM_PARTIAL_INSTANTIATION_REGISTER_MULTI_DIM_H */
