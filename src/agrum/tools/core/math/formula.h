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
 * @brief Headers files for the gum::FormulaPart and gum::Formula classes.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_MATH_FORMULA_H
#define GUM_MATH_FORMULA_H

#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include <agrum/agrum.h>
#include <agrum/tools/core/hashTable.h>

namespace gum {

  namespace formula {
    class Scanner;
    class Parser;
  }   // namespace formula

  /**
   * @class FormulaPart
   * @headerfile formula.h <agrum/tools/core/math/formula.h>
   * @brief Represents part of a formula.
   * @ingroup math_group
   *
   * This class is used by the gum::Formula class to store intermediate results
   * when solving the formula using the Shuntin-yard algorithm.
   */
  class FormulaPart {
    public:
    /// The tokens constituting a formula.
    enum token_type {
      NUMBER,
      OPERATOR,
      PARENTHESIS,
      NIL,
      FUNCTION,
      ARG_SEP
    };

    /// The functions allowed in a formula.
    enum token_function {
      exp,
      log,
      ln,
      pow,
      sqrt,
      nil
    };

    /// The token_type stored by this gum::FormulaPart.
    token_type type;

    /**
     * @brief The value stored by this gum::FormulaPart
     *
     * @warning Only one of these three members will hold the value, given the
     * type of this gum::FormulaPart.
     */
    /// @{
    double         number;
    char           character;
    token_function function;
    /// @}

    // ========================================================================
    /// @name Constructors and destructor
    // ========================================================================
    /// @{

    /**
     * @brief Class constructor.
     */
    FormulaPart();

    /**
     * @brief Constructor for doubles.
     * @param t The token_type of this gum::FormulaPart.
     * @param n The value of this gum::FormulaPart.
     */
    FormulaPart(token_type t, double n);

    /**
     * @brief Constructor for chars.
     * @param t The token_type of this gum::FormulaPart.
     * @param c The value of this gum::FormulaPart.
     */
    FormulaPart(token_type t, char c);

    /**
     * @brief Constructor for functions.
     * @param t The token_type of this gum::FormulaPart.
     * @param func The value of this gum::FormulaPart.
     */
    FormulaPart(token_type t, token_function func);

    /**
     * @brief Copy constructor.
     * @param source The gum::FormulaPart to copy.
     */
    FormulaPart(const FormulaPart& source);

    /**
     * @brief Move constructor.
     * @param source The gum::FormulaPart to move.
     */
    FormulaPart(FormulaPart&& source);

    /**
     * @brief Class destuctor.
     */
    ~FormulaPart();

    /// @}
    // ========================================================================
    /// @name Operators
    // ========================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @name source The gum::FormulaPart to copy.
     * @return Returns this gum::FormulaPart.
     */
    FormulaPart& operator=(const FormulaPart& source);

    /**
     * @brief Move operator.
     * @name source The gum::FormulaPart to copy.
     * @return Returns this gum::FormulaPart.
     */
    FormulaPart& operator=(FormulaPart&& source);

    /// @}
    // ========================================================================
    /// @name Getters and setters
    // ========================================================================
    /// @{

    /**
     * @brief Returns a string representation of this gum::FormulaPart value.
     * @return Returns a string representation of this gum::FormulaPart value.
     */
    std::string str() const;

    /**
     * @brief Returns true if this gum::FormulaPart is left associative.
     * @return Returns true if this gum::FormulaPart is left associative.
     * @throw OperationNotAllowed Raised if the value stored is not an
     * operator.
     */
    bool isLeftAssociative() const;

    /**
     * @brief Returns true if this gum::FormulaPart is right associative.
     * @return Returns true if this gum::FormulaPart is right associative.
     * @throw OperationNotAllowed Raised if the value stored is not an
     * operator.
     */
    bool isRightAssociative() const;

    /**
     * @brief Returns the precedence priority of the value stored in this
     * gum::FormulaPart.
     * @return Returns the precedence priority of the value stored in this
     * gum::FormulaPart.
     * @throw OperationNotAllowed Raised if the value stored is not an
     * operator.
     */
    int precedence() const;

    /**
     * @brief Returns the number of argument of the function stored in this
     * gum::FormulaPart.
     * @return Returns the number of argument of the function stored in this
     * gum::FormulaPart.
     * @throw OperationNotAllowed Raised if the value stored is not a function.
     */
    size_t argc() const;

    /**
     * @brief Returns the evaluation of the vector of gum::FormulaPart as
     * arguments of the value stored in this gum::FormulaPart.
     *
     * @warning Args must be backwards !
     *
     * @param args The arguments, in backards, passed to the value stored in
     * this gum::FormulaPart.
     * @return Returns the evaluation of the vector of gum::FormulaPart as
     * arguments of the value stored in this gum::FormulaPart.
     * @throw OperationNotAllowed Raised if the value stored is neither a
     * function nor an operator.
     */
    FormulaPart eval(const std::vector< FormulaPart >& args) const;

    /// @}

    private:
    /**
     * @brief Returns the evaluation of the vector of gum::FormulaPart as
     * arguments of the value stored in this gum::FormulaPart.
     *
     * @warning Args must be backwards !
     *
     * @param args The arguments, in backards, passed to the value stored in
     * this gum::FormulaPart.
     * @return Returns the evaluation of the vector of gum::FormulaPart as
     * arguments of the value stored in this gum::FormulaPart.
     * @throw OperationNotAllowed Raised if the value stored is not an
     * operator.
     */
    double _operator_eval_(const std::vector< FormulaPart >& args) const;

    /**
     * @brief Returns the evaluation of the vector of gum::FormulaPart as
     * arguments of the value stored in this gum::FormulaPart.
     *
     * @warning Args must be backwards !
     *
     * @param args The arguments, in backards, passed to the value stored in
     * this gum::FormulaPart.
     * @return Returns the evaluation of the vector of gum::FormulaPart as
     * arguments of the value stored in this gum::FormulaPart.
     * @throw OperationNotAllowed Raised if the value stored is not a
     * function.
     */
    double _function_eval_(const std::vector< FormulaPart >& args) const;

    /**
     * @brief Returns the number of arguments expected by the operator stored
     * in this gum::FormulaPart.
     * @return Returns the number of arguments expected by the operator stored
     * in this gum::FormulaPart.
     */
    size_t _operator_argc_() const;

    /**
     * @brief Returns the number of arguments expected by the function stored
     * in this gum::FormulaPart.
     * @return Returns the number of arguments expected by the function stored
     * in this gum::FormulaPart.
     */
    size_t _function_argc_() const;
  };

  // extern class gum::formula::Parser;
  /**
   * @brief Evaluates a string as a algebraic formula.
   * @ingroup math_group
   *
   * Implementation of the Shunting-yard algorithm to convert infix notation to
   * RPN. The gum::Formula::result() method implements the postfix algorithm to
   * compute the formula result.
   *
   * @warning Checking is only done when evaluating the formula !
   */
  class Formula {
    friend class gum::formula::Parser;

    public:
    // ========================================================================
    /// @name Constructors and destructor
    // ========================================================================
    /// @{

    /**
     * Constructor
     */
    /// @{
    Formula(short s);
    Formula(unsigned short us);
    Formula(int i);
    Formula(unsigned int ui);
    Formula(long l);
    Formula(unsigned long ul);
    Formula(long long l);
    Formula(unsigned long long ul);
    Formula(float f);
    Formula(double d);
    /// @}

    /**
     * @brief Class constructor.
     * @param f An algebraic formula.
     */
    Formula(const std::string& f);

    /**
     * @brief Copy constructor.
     * @param source The gum::Formula to copy.
     */
    Formula(const Formula& source);

    /**
     * @brief Move constructor.
     * @param source The gum::Formula to move.
     */
    Formula(Formula&& source);

    /**
     * @brief Class destructor.
     */
    ~Formula();

    /// @}
    // ========================================================================
    /// @name Operators
    // ========================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param source The gum::Formula to copy.
     * @return Returns this gum::Formula.
     */
    Formula& operator=(const Formula& source);

    /**
     * @brief Move operator.
     * @param source The gum::Formula to move.
     * @return Returns this gum::Formula.
     */
    Formula& operator=(Formula&& source);

    /**
     * @brief Allows implicit conversion to doubles.
     */
    explicit operator double() const { return result(); }

    /// @}
    // ========================================================================
    /// @name Accessors & modifiers
    // ========================================================================
    /// @{

    /**
     * @brief Returns the variables used by this gum::Formula.
     * @return Returns the variables used by this gum::Formula.
     */
    HashTable< std::string, double >& variables();

    /**
     * @brief Returns the variables used by this gum::Formula.
     * @return Returns the variables used by this gum::Formula.
     */
    const HashTable< std::string, double >& variables() const;

    /**
     * @brief Returns the result of this gum::Formula.
     *
     * Each call to Formula::result() will reevaluate the formulas result.
     *
     * @return Returns the result of this gum::Formula.
     */
    double result() const;

    /**
     * @brief Returns the formula.
     */
    const std::string& formula() const;

    /**
     * @brief Returns the formula.
     */
    std::string& formula();

    private:
    /// @}
    // ========================================================================
    /// @name Private accessors & modifiers used by the Formula Parser
    // ========================================================================
    /// @{

    /**
     * @brief Push a number in the formula.
     * @param v The number to push.
     */
    void _push_number_(const double& v);

    /**
     * @brief Push an operator in the formula.
     * @param o The operator to push.
     */
    void _push_operator_(char o);

    /**
     * @brief Push a left parenthesis in the formula.
     */
    void _push_leftParenthesis_();

    /**
     * @brief Push a right parenthesis in the formula.
     */
    void _push_rightParenthesis_();

    /**
     * @brief Push a function in the formula.
     * @param func The function to push.
     */
    void _push_function_(const std::string& func);

    /**
     * @brief Push a variable in the formula.
     */
    void _push_variable_(const std::string& var);

    /**
     * @brief Use this if you don't know if ident is a function or a variable.
     */
    void _push_identifier_(const std::string& ident);

    /**
     * @brief Push a comma in the formula.
     */
    void _push_comma_();

    /**
     * @brief Finalize the formula and prepare it for evaluation.
     */
    void _finalize_();

    /// @}

    /// The formula to evaluate.
    std::string _formula_;

    /// The scanner used by the formula.
    std::unique_ptr< gum::formula::Scanner > _scanner_;

    /// The parser used by the formula.
    std::unique_ptr< gum::formula::Parser > _parser_;

    /// The last token added to the formula.
    FormulaPart _last_token_;

    /// The output stack, will contain one value after evaluation.
    std::vector< FormulaPart > _output_;

    /// A stack used during evaluation.
    std::stack< FormulaPart > _stack_;

    /// The variables available in this formula.
    HashTable< std::string, double > _variables_;

    /**
     * @brief Initialise the formula scanner and parser.
     */
    void _initialise_();

    /**
     * @brief Pop the operator in the inner formula's stack.
     * @param o The operator to pop.
     * @return Returns true if the operator was popped.
     */
    bool _popOperator_(FormulaPart o);

    /**
     * @brief Evaluate an operator or function and push its result.
     * @param item The operator or function to reduce.
     * @param stack The stack to evaluate.
     */
    void _reduceOperatorOrFunction_(FormulaPart item, std::stack< FormulaPart >& stack) const;

    /**
     * @brief Push an unary operator.
     * @param o The unary operator to push.
     */
    void _push_unaryOperator_(char o);

    /**
     * @brief Push an operator.
     * @param t The operator to push.
     */
    void _push_operator_(FormulaPart t);

    /**
     * @brief Returns true if o is an unary operator.
     * @return Returns true if o is an unary operator.
     */
    bool _isUnaryOperator_(char o);

    /**
     * @brief Push the gum::FormulaPart in the output vector.
     * @param t The gum::FormulaPart to push.
     */
    void _push_output_(FormulaPart t);

    /**
     * @brief Push the gum::FormulaPart in the stack.
     * @param t The gum::FormulaPart to push.
     */
    void _push_stack_(FormulaPart t);
  };

  // // ========================================================================
  // /// @name Arithmetic Operators
  // // ========================================================================
  // /// @{

  Formula operator-(const Formula& a);

  Formula operator+(const Formula& a, const Formula& b);

  Formula operator-(const Formula& a, const Formula& b);

  Formula operator*(const Formula& a, const Formula& b);

  Formula operator/(const Formula& a, const Formula& b);

  std::string to_string(const Formula& f);

  std::ostream& operator<<(std::ostream& os, const Formula& f);

  // /// @}

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/core/math/formula_inl.h>
#endif   // GUM_NO_INLINE

#endif /* GUM_MATH_FORMULA_H */
