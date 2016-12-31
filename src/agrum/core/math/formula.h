/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
/**
 * @file
 * @brief Headers files for the gum::FormulaPart and gum::Formula classes.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_MATH_FORMULA_H
#define GUM_MATH_FORMULA_H

#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include <agrum/config.h>
#include <agrum/core/hashTable.h>

namespace gum {

  namespace formula {
    class Scanner;
    class Parser;
  }

  /**
   * @class FormulaPart formula.h <agrum/core/math/formula.h>
   * @brief Represents part of a formula.
   * @ingroup math_group
   *
   * This class is used by the gum::Formula class to store intermediate results
   * when solving the formula using the Shuntin-yard algorithm.
   */
  class FormulaPart {
    public:
    /// The tokens constituting a formula.
    enum token_type { NUMBER, OPERATOR, PARENTHESIS, NIL, FUNCTION, ARG_SEP };

    /// The functions allowed in a formula.
    enum token_function { exp, log, ln, pow, sqrt, nil };

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
    FormulaPart( token_type t, double n );

    /**
     * @brief Constructor for chars.
     * @param t The token_type of this gum::FormulaPart.
     * @param c The value of this gum::FormulaPart.
     */
    FormulaPart( token_type t, char c );

    /**
     * @brief Constructor for functions.
     * @param t The token_type of this gum::FormulaPart.
     * @param func The value of this gum::FormulaPart.
     */
    FormulaPart( token_type t, token_function func );

    /**
     * @brief Copy constructor.
     * @param source The gum::FormulaPart to copy.
     */
    FormulaPart( const FormulaPart& source );

    /**
     * @brief Move constructor.
     * @param source The gum::FormulaPart to move.
     */
    FormulaPart( FormulaPart&& source );

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
    FormulaPart& operator=( const FormulaPart& source );

    /**
     * @brief Move operator.
     * @name source The gum::FormulaPart to copy.
     * @return Returns this gum::FormulaPart.
     */
    FormulaPart& operator=( FormulaPart&& source );

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
    FormulaPart eval( const std::vector<FormulaPart>& args ) const;

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
    double __operator_eval( const std::vector<FormulaPart>& args ) const;

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
    double __function_eval( const std::vector<FormulaPart>& args ) const;

    /**
     * @brief Returns the number of arguments expected by the operator stored
     * in this gum::FormulaPart.
     * @return Returns the number of arguments expected by the operator stored
     * in this gum::FormulaPart.
     */
    size_t __operator_argc() const;

    /**
     * @brief Returns the number of arguments expected by the function stored
     * in this gum::FormulaPart.
     * @return Returns the number of arguments expected by the function stored
     * in this gum::FormulaPart.
     */
    size_t __function_argc() const;
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
    Formula( short s );
    Formula( unsigned short us );
    Formula( int i );
    Formula( unsigned int ui );
    Formula( long l );
    Formula( unsigned long ul );
    Formula( long long l );
    Formula( unsigned long long ul );
    Formula( float f );
    Formula( double d );
    /// @}

    /**
     * @brief Class constructor.
     * @param f An algebraic formula.
     */
    Formula( const std::string& f );

    /**
     * @brief Copy constructor.
     * @param source The gum::Formula to copy.
     */
    Formula( const Formula& source );

    /**
     * @brief Move constructor.
     * @param source The gum::Formula to move.
     */
    Formula( Formula&& source );

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
    Formula& operator=( const Formula& source );

    /**
     * @brief Move operator.
     * @param source The gum::Formula to move.
     * @return Returns this gum::Formula.
     */
    Formula& operator=( Formula&& source );

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
    HashTable<std::string, double>& variables();

    /**
     * @brief Returns the variables used by this gum::Formula.
     * @return Returns the variables used by this gum::Formula.
     */
    const HashTable<std::string, double>& variables() const;

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
    void __push_number( const double& v );

    /**
     * @brief Push an operator in the formula.
     * @param o The operator to push.
     */
    void __push_operator( char o );

    /**
     * @brief Push a left parenthesis in the formula.
     */
    void __push_leftParenthesis();

    /**
     * @brief Push a right parenthesis in the formula.
     */
    void __push_rightParenthesis();

    /**
     * @brief Push a function in the formula.
     * @param func The function to push.
     */
    void __push_function( const std::string& func );

    /**
     * @brief Push a variable in the formula.
     */
    void __push_variable( const std::string& var );

    /**
     * @brief Use this if you don't know if ident is a function or a variable.
     */
    void __push_identifier( const std::string& ident );

    /**
     * @brief Push a comma in the formula.
     */
    void __push_comma();

    /**
     * @brief Finalize the formula and prepare it for evaluation.
     */
    void __finalize();

    /// @}

    /// The formula to evaluate.
    std::string __formula;

    /// The scanner used by the formula.
    std::unique_ptr<gum::formula::Scanner> __scanner;

    /// The parser used by the formula.
    std::unique_ptr<gum::formula::Parser> __parser;

    /// The last token added to the formula.
    FormulaPart __last_token;

    /// The output stack, will contain one value after evaluation.
    std::vector<FormulaPart> __output;

    /// A stack used during evaluation.
    std::stack<FormulaPart> __stack;

    /// The variables available in this formula.
    HashTable<std::string, double> __variables;

    /**
     * @brief Initialise the formula scanner and parser.
     */
    void __initialise();

    /**
     * @brief Pop the operator in the inner formula's stack.
     * @param o The operator to pop.
     * @return Returns true if the operator was popped.
     */
    bool __popOperator( FormulaPart o );

    /**
     * @brief Evaluate an operator or function and push its result.
     * @param item The operator or function to reduce.
     * @param stack The stack to evaluate.
     */
    void __reduceOperatorOrFunction( FormulaPart              item,
                                     std::stack<FormulaPart>& stack ) const;

    /**
     * @brief Push an unary operator.
     * @param o The unary operator to push.
     */
    void __push_unaryOperator( char o );

    /**
     * @brief Push an operator.
     * @param t The operator to push.
     */
    void __push_operator( FormulaPart t );

    /**
     * @brief Returns true if o is an unary operator.
     * @return Returns true if o is an unary operator.
     */
    bool __isUnaryOperator( char o );

    /**
     * @brief Push the gum::FormulaPart in the output vector.
     * @param t The gum::FormulaPart to push.
     */
    void __push_output( FormulaPart t );

    /**
     * @brief Push the gum::FormulaPart in the stack.
     * @param t The gum::FormulaPart to push.
     */
    void __push_stack( FormulaPart t );
  };

  // // ========================================================================
  // /// @name Arithmetic Operators
  // // ========================================================================
  // /// @{

  Formula operator-( const Formula& a );

  Formula operator+( const Formula& a, const Formula& b );

  Formula operator-( const Formula& a, const Formula& b );

  Formula operator*( const Formula& a, const Formula& b );

  Formula operator/( const Formula& a, const Formula& b );

  std::string to_string( const Formula& f );

  std::ostream& operator<<( std::ostream& os, const Formula& f );

  // /// @}

} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/core/math/formula_inl.h>
#endif  // GUM_NO_INLINE

#endif /* GUM_MATH_FORMULA_H */
