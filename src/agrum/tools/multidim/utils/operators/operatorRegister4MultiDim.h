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
 * @brief A container for registering binary functions on
 * multiDimImplementations
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_OPERATOR_REGISTER_4_MULTI_DIM_H
#define GUM_OPERATOR_REGISTER_4_MULTI_DIM_H

#include <atomic>
#include <iostream>
#include <string>
#include <utility>

#include <agrum/tools/core/hashTable.h>


namespace gum {

  // the base object used by combinations
  template < typename GUM_SCALAR >
  class MultiDimImplementation;

  // ===========================================================================
  // ===                   GUM_MULTI_DIM_OPERATOR_REGISTER                   ===
  // ===========================================================================
  // clang-format off
  /**
   * @class OperatorRegister4MultiDim
   * @headerfile operatorRegister4MultiDim.h <agrum/multdim/operators/operatorRegister4MultiDim.h>
   * @ingroup multidim_op_group
   *
   * @brief A container for registering binary functions on
   * multiDimImplementations
   */
  // clang-format on
  template < typename GUM_SCALAR >
  class OperatorRegister4MultiDim {
    public:
    /// The type of functions used by the register
    using OperatorPtr
       = MultiDimImplementation< GUM_SCALAR >* (*)(const MultiDimImplementation< GUM_SCALAR >*,
                                                   const MultiDimImplementation< GUM_SCALAR >*);

    // ========================================================================
    /// @name Accessors / Modifiers
    // ========================================================================
    /// @{

    /**
     * @brief adds a new entry into the register
     *
     * This method inserts a new function (\e newFunction) taking two multiDim
     * in arguments of types \e type1 and type2 respectively (which actually
     * inherit from MultiDimImplementation). This new function's purpose is to
     * achieve the operator described by \e operation_name. For instance, if
     * operator_name is "+", "-", "/" or "*", the new function performs the
     * usual algebraic operations.
     *
     * @param operation_name describes the name of the operation performed by
     * newFunction. Usual operation names are "+", "*", "/", "-", "max", "min"
     *
     * @param type1 the \e real type of the first multiDim taken in argument by
     * function \e newFunction
     *
     * @param type2 the \e real type of the second multiDim taken in argument
     * by function \e newFunction
     *
     * @param newFunction a pointer to the new function to register. Note that
     * although \e newFunction actually performs an operation on multiDims of
     * types \e type1 and \e type2, it should be declared as taking in argument
     * two MultiDimImplementations. This constraint is imposed by the C++
     * typing system.
     */
    void insert(const std::string& operation_name,
                const std::string& type1,
                const std::string& type2,
                OperatorPtr        newFunction);

    /**
     * @brief removes a given entry from the register
     *
     * removes the function, if any, that performs the operation described by
     * \e operation_name, and that takes in argument a multiDim of type \e
     * type1 and one multiDim of type \e type2
     *
     * @param operation_name describes the name of the operation performed by
     * the function to remove. Usual operation names are "+", "*", "/", "-",
     * "max", "min"
     *
     * @param type1 the \e real type of the first multiDim taken in argument by
     * the function to remove
     *
     * @param type2 the \e real type of the second multiDim taken in argument
     * by the function to remove.
     */
    void
       erase(const std::string& operation_name, const std::string& type1, const std::string& type2);

    /**
     * @brief Indicates whether a given entry exists in the register.
     *
     * Indicates if the register contains a function that performs the
     * operation described by \e operation_name, and that takes in argument a
     * multiDim of type \e type1 and one multiDim of type \e type2
     *
     * @param operation_name describes the name of the operation performed by
     * the function we look for. Usual operation names are "+", "*", "/", "-",
     * "max", "min"
     *
     * @param type1 the \e real type of the first multiDim taken in argument by
     * the function we look for
     *
     * @param type2 the \e real type of the second multiDim taken in argument
     * by the function we look for
     */
    bool exists(const std::string& operation_name,
                const std::string& type1,
                const std::string& type2) const;

    /**
     * @brief returns the specialized operator assigned to a given pair of
     * MultiDimImplementations
     *
     * returns the function, if any, that performs the operation described by
     * \e operation_name, and that takes in argument a multiDim of type \e
     * type1 and one multiDim of type \e type2
     *
     * @param operation_name describes the name of the operation performed by
     * the function we look for. Usual operation names are "+", "*", "/", "-",
     * "max", "min"
     *
     * @param type1 the \e real type of the first multiDim taken in argument by
     * the function we look for
     *
     * @param type2 the \e real type of the second multiDim taken in argument
     * by the function we look for
     *
     * @throws NotFound exception is thrown if the operator we look for does
     * not exist within this register.
     */
    OperatorPtr get(const std::string& operation_name,
                    const std::string& type1,
                    const std::string& type2) const;

    /// @}
    // ========================================================================
    /// @name Named Constructors
    // ========================================================================
    /// @{

    /**
     * @brief A named constructor that constructs one and only one Register per
     * data type.
     *
     * Note that this constructor prevents the famous init order fiasco.
     */
    static OperatorRegister4MultiDim< GUM_SCALAR >& Register();

    /// @}

    private:
    // ========================================================================
    /// @name Constructors / Destructors
    // ========================================================================
    /// @{

    /// Default constructor: creates an empty register
    OperatorRegister4MultiDim();

    /// Copy operator: never to be used
    OperatorRegister4MultiDim(const OperatorRegister4MultiDim< GUM_SCALAR >&);

    /// Destructor
    ~OperatorRegister4MultiDim();

    /// @}

    /// The set of associations for a given operation type
    using OperatorSet = HashTable< std::pair< std::string, std::string >, OperatorPtr >;

    /**
     * @brief A mapping from pairs of types of MultiDimImplementations to
     * operators.
     *
     * In this type, the strings represent the very types of the
     * MultiDimImplementations that will be combined. To a pair of strings,
     * hence to a pair of types of MultiDimImplementations, is associated a
     * function to combine them (the OperatorPtr).
     */
    HashTable< std::string, OperatorSet* > _set_;
  };

  /// A function to more easily register new operators in MultiDims
  template < typename GUM_SCALAR >
  void registerOperator(const std::string& operation_name,
                        const std::string& type1,
                        const std::string& type2,
                        typename OperatorRegister4MultiDim< GUM_SCALAR >::OperatorPtr function);

} /* namespace gum */

// always include the implementations
#include <agrum/tools/multidim/utils/operators/operatorRegister4MultiDim_tpl.h>

#endif /* GUM_OPERATOR_REGISTER_MULTI_DIM_H */
