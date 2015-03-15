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
/** @file
 * @brief Headers of Formula.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_FORMULA_FORMULA_H
#define GUM_FORMULA_FORMULA_H

#include <vector>
#include <stack>
#include <string>
#include <sstream>
#include <iostream>

#include <agrum/core/hashTable.h>

namespace gum {

  class FormulaPart {
    public:
      enum token_type { NUMBER, OPERATOR, PARENTHESIS, NIL, FUNCTION, ARG_SEP };
      enum token_function { exp, log, ln, pow, sqrt, bernoulli, binomial,
        geometric, negative_binomial, poisson, exponential, gamma, weibull,
        extreme_value, normal, lognormal, chi_squared, cauchy, fisher_f, student_t,
        discrete, piecewise_constant, piecewise_linear, nil };

      token_type type;
      double number;
      char character;
      token_function function;

      FormulaPart();
      FormulaPart(token_type t, double n);
      FormulaPart(token_type t, char c);
      FormulaPart(token_type t, token_function);

      FormulaPart(const FormulaPart & source);

      ~FormulaPart();

      FormulaPart& operator= (const FormulaPart & source);

      std::string str() const;

      bool isLeftAssociative() const;

      bool isRightAssociative() const;

      int precedence() const;

      size_t argc() const;

      /// Args are backwards !
      FormulaPart eval(const std::vector<FormulaPart>& args) const;
      
    private:
      /// Args are backwards !
      double __operator_eval(const std::vector<FormulaPart>& args) const;
      double __function_eval(const std::vector<FormulaPart>& args) const;

      size_t __operator_argc() const;
      size_t __function_argc() const;
  };

  /// Implementation of the Shunting-yard algorithm to convert infix notation
  /// to RPN.
  /// The result() method implements the postfix aglorithm to compute the
  /// formula result. 
  class Formula {

    public:

      Formula();
      Formula(const Formula & source);
      ~Formula();

      Formula & operator=(const Formula & source);

      HashTable<std::string, double> & variables();

      const HashTable<std::string, double> & variables() const;

      double result() const;

      void push_number(const double &v);

      void push_operator(char o);

      void push_leftParenthesis();

      void push_rightParenthesis();

      void push_function(const std::string & func);

      void push_variable(const std::string & var);

      /// Use this if you don't know is ident is a function or a variable.
      void push_identifier(const std::string & ident);

      void push_comma();

      void finalize();

    private:
      FormulaPart __last_token;

      std::vector<FormulaPart> __output;
      std::stack<FormulaPart> __stack;

      HashTable<std::string, double> __variables;

      std::vector<FormulaPart>& output();
      const std::vector<FormulaPart>& output() const;

      std::stack<FormulaPart>& stack();
      const std::stack<FormulaPart>& stack() const;

      bool __popOperator(FormulaPart o);

      void __reduceOperatorOrFunction(FormulaPart item,
                                      std::stack<FormulaPart> &stack) const;

      void __push_unaryOperator(char o);

      void __push_operator(FormulaPart t);

      bool __isUnaryOperator(char o);

      void __push_output(FormulaPart t);

      void __push_stack(FormulaPart t);

  };

} /* namespace gum */

#endif /* GUM_FORMULA_FORMULA_H */

