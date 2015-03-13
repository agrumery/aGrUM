/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr   *
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

#include <agrum/multidim/eval/formula.h>

#include <agrum/config.h>

namespace gum {

/***************************************************************************
 *                            Class FormulaPart                            *
 ***************************************************************************/

  FormulaPart::FormulaPart(token_type t, double n):
    type(t), number(n), character('\0') 
  {
    GUM_CONSTRUCTOR( FormulaPart );
  }

  FormulaPart::FormulaPart(token_type t, char c):
    type(t), number(NAN), character(c) 
  {
    GUM_CONSTRUCTOR( FormulaPart );
  }

  FormulaPart::FormulaPart(const FormulaPart & source):
    type(source.type), number(source.number), character(source.character) 
  {
    GUM_CONS_CPY( FormulaPart );
  }

  FormulaPart::~FormulaPart() {
    GUM_DESTRUCTOR( FormulaPart );
  }

  FormulaPart&
    FormulaPart::operator= (const FormulaPart & source) {
      type = source.type;
      number = source.number;
      character = source.character;
      return *this;
    }

  std::string
  FormulaPart::str() const {
    std::ostringstream s;
    switch (type) {
      case token_type::NUMBER: {
                                 s << number; 
                                 break;
                               }
      case token_type::PARENTHESIS:
      case token_type::OPERATOR: {
                                   s << character;
                                   break;
                                 }

      default: { GUM_ERROR(OperationNotAllowed, "unknown type"); }
    }
    return s.str();
  }

  bool
  FormulaPart::isLeftAssociative() const {
    switch (character) {
      case '+':
      case '-':
      case '*':
      case '/': {
                  return true;
                }

      case '^': {
                  return false;
                }

      default : {
                  GUM_ERROR( OperationNotAllowed, "A - not an operator");
                }
    }
  }

  bool 
  FormulaPart::isRightAssociative() const {
    return not isLeftAssociative();
  }

  int 
  FormulaPart::precedence() const {
    switch (character) {
      case '+':
      case '-': {
                  return 2;
                }

      case '*':
      case '/': {
                  return 3;
                }

      case '^': {
                  return 4;
                }

      default : {
                  GUM_ERROR( OperationNotAllowed, "B - not an operator");
                }
    }
  }

  size_t
  FormulaPart::argc() const {
    switch (character) {
      case '-': {
                  return (size_t) 1;
                }
      case '+':
      case '*':
      case '/': 
      case '^': {
                  return (size_t) 2;
                }

      default : {
                  GUM_ERROR( OperationNotAllowed, "C - not an operator");
                }
    }
  }

  /// Args are backwards !
  double
  FormulaPart::__eval(const std::vector<FormulaPart>& args) const {
    switch (character) {
      case '+': {
                  return args[1].number + args[0].number;
                }

      case '-': {
                  return args[1].number - args[0].number;
                }

      case '*': {
                  return args[1].number * args[0].number;
                }

      case '/': {
                  return args[1].number / args[0].number;
                }

      case '^': {
                  return std::pow(args[1].number, args[0].number);
                }

      default : {
                  GUM_ERROR( OperationNotAllowed, "D - not an operator");
                }
    }
  }

  /// Args are backwards !
  FormulaPart
  FormulaPart::eval(const std::vector<FormulaPart>& args) const {
    return FormulaPart(token_type::NUMBER, __eval(args));
  }

/***************************************************************************
 *                            Class Formula                                *
 ***************************************************************************/

  Formula::Formula() {
    GUM_CONSTRUCTOR( Formula );
  }

  Formula::Formula(const Formula& source):
    __heap(source.__heap), __output(source.__output), __stack(source.__stack)
  {
    GUM_CONS_CPY( Formula );
  }

  Formula::~Formula() {
    GUM_DESTRUCTOR( Formula );
  } 

  Formula &
  Formula::operator=(const Formula & source) {
    __heap = source.__heap;
    __output = source.__output;
    __stack =  source.__stack;
    return *this;
  }

  double 
  Formula::result() const {
    std::stack<FormulaPart> stack;
    if (output().empty()) {
      GUM_ERROR(OperationNotAllowed, "no output found");
    }

    for ( auto item: output() ) {


      switch (item.type) {
        case FormulaPart::token_type::NUMBER: {
                                   stack.push(item);
                                   break;
                                 }

        case FormulaPart::token_type::OPERATOR: {
                                     __reduceOperator(item, stack);
                                     break;
                                   }

        default: {
                   GUM_ERROR(OperationNotAllowed, "expecting numbers, operators or functions");
                 }
      }

    }

    if (stack.size() != 1) {

      GUM_ERROR(OperationNotAllowed, "too many inputs");

    } else if (stack.top().type != FormulaPart::token_type::NUMBER) {

      GUM_ERROR(OperationNotAllowed, "too many inputs");
    }
    return stack.top().number;
  }

  void
  Formula::push_number(const double &v) {
    output().push_back(FormulaPart(FormulaPart::token_type::NUMBER, v));
  }

  bool
  Formula::__popOperator(FormulaPart o) {
    if (stack().empty() or stack().top().type != FormulaPart::token_type::OPERATOR) {
      return false;
    }

    if (o.isLeftAssociative() and o.precedence() <= stack().top().precedence() ) {
      return true;
    }

    if (o.isRightAssociative() and o.precedence() < stack().top().precedence() ) {
      return true;
    }

    return false;
  }

  void
  Formula::push_operator(char o) {
    FormulaPart t(FormulaPart::token_type::OPERATOR, o);

    while ( __popOperator(t) ) {
      output().push_back(stack().top());
      stack().pop();
    }

    stack().push(t);
  }

  void
  Formula::push_leftParenthesis() {
    FormulaPart t(FormulaPart::token_type::PARENTHESIS, '(');
    stack().push(t);
  }

  void
  Formula::push_rightParenthesis() {

    while ( (not stack().empty()) and ( stack().top().character != '(' ) ) {

      output().push_back( stack().top() );
      stack().pop();

    }

    if (stack().empty()) {

      GUM_ERROR(OperationNotAllowed, "expecting '('");

    } else if (stack().top().character != '(') {

      GUM_ERROR(OperationNotAllowed, "expecting '('");

    }

    stack().pop();

  }

  void
  Formula::finalize() {

    while (not stack().empty()) {
      if (stack().top().character == '(') {

        GUM_ERROR(OperationNotAllowed, "expecting ')'");

      }

      output().push_back( stack().top() );
      stack().pop();

    }

  }

  void
  Formula::__reduceOperator(FormulaPart item, std::stack<FormulaPart> &stack) const {

    std::vector<FormulaPart> args;

    if (stack.size() < item.argc()) {
      GUM_ERROR(OperationNotAllowed, "not enought inputs for " + item.character);
    }

    // We need to handle the minus vs negative case
    if (item.character == '-') {

      args.push_back(stack.top());
      stack.pop();

      if (not stack.empty()) {
        if (stack.top().type == FormulaPart::token_type::NUMBER) {
          args.push_back(stack.top());
          stack.pop();
        }
      }

      // It is a negative sign 
      if (args.size() == 1) {
        args.push_back(FormulaPart(FormulaPart::token_type::NUMBER, 0.0));
      }

    } else  {

      while (item.argc() > args.size()) {

        args.push_back(stack.top());
        stack.pop();

      }
    }

    stack.push(item.eval(args));
  }

  std::stack<Formula>&
  Formula::heap() {
    return __heap;
  }

  const std::stack<Formula>&
  Formula::heap() const {
    return __heap;
  }

  std::vector<FormulaPart>&
  Formula::output() {
    return __output;
  } 

  const std::vector<FormulaPart>&
  Formula::output() const {
    return __output;
  } 

  std::stack<FormulaPart>&
  Formula::stack() {
    return __stack;
  }

  const std::stack<FormulaPart>&
  Formula::stack() const {
    return __stack;
  }

} // namespace gum
