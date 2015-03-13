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

#include <list>
#include <agrum/config.h>

namespace gum {

/***************************************************************************
 *                            Class FormulaPart                            *
 ***************************************************************************/

  // Helper function for debuging
  void print_stack(std::stack<FormulaPart> s) {
    std::cout << std::endl;
    std::list<FormulaPart> l;
    while (not s.empty()) {
      l.push_front(s.top());
      s.pop();
    }

    std::cout << "Stack: ";
    for (const auto & elt : l) {
      std::cout << elt.str() << " ";
    }
    std::cout << std::endl;
  }

  // Helper function for debuging
  void print_output(std::vector<FormulaPart> v) {
    std::cout << "Output: ";
    for (const auto & elt : v) {
      std::cout << elt.str() << " ";
    }
    std::cout << std::endl;
 }

  FormulaPart::FormulaPart():
    type(token_type::NIL), number(NAN), character('\0') 
  {
    GUM_CONSTRUCTOR( FormulaPart );
  }

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
      case '/':
        {
          return true;
        }

      case '_':
        {
          return false;
        }
      case '^':
        {
          return false;
        }

      default :
        {
          GUM_ERROR( OperationNotAllowed, "A - not an operator");
        }
    }
  }

  bool 
  FormulaPart::isRightAssociative() const {
    switch (character) {
      case '_':
        {
          return false;
        }
      default:
        {
          return not isLeftAssociative();
        }
    }
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
      
      case '_': {
                  return 5;
                }

      default : {
                  GUM_ERROR( OperationNotAllowed, "B - not an operator");
                }
    }
  }

  size_t
  FormulaPart::argc() const {
    switch (character) {
      case '_': {
                  return (size_t) 1;
                }
      case '+':
      case '-':
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
      case '+':
        {
          return args[1].number + args[0].number;
        }

      case '-':
        {
          return args[1].number - args[0].number;
        }

      case '*':
        {
          return args[1].number * args[0].number;
        }

      case '/':
        {
          return args[1].number / args[0].number;
        }

      case '^':
        {
          return std::pow(args[1].number, args[0].number);
        }

      case '_':
        {
          return 0 - args[0].number;
        }

      default :
        {
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

  Formula::Formula():
    __last_token(FormulaPart())
  {
    GUM_CONSTRUCTOR( Formula );
  }

  Formula::Formula(const Formula& source):
    __last_token(source.__last_token),
    __output(source.__output), __stack(source.__stack)
  {
    GUM_CONS_CPY( Formula );
  }

  Formula::~Formula() {
    GUM_DESTRUCTOR( Formula );
  } 

  Formula &
  Formula::operator=(const Formula & source) {
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
    FormulaPart t(FormulaPart::token_type::NUMBER, v);
    __push_output(t);
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

    if ( __isUnaryOperator(o) ) {

      __push_unaryOperator(o);

    } else {

      FormulaPart t(FormulaPart::token_type::OPERATOR, o);
      __push_operator(t);

    }
  }

  bool
  Formula::__isUnaryOperator(char o) {
    switch (__last_token.type) {

      case FormulaPart::token_type::OPERATOR:
      case FormulaPart::token_type::NIL:
        {
          return o == '-';
        }

      case FormulaPart::token_type::PARENTHESIS:
        {
          return o == '-' and __last_token.character == '(';
        }

      default:
        {
          return false;
        }

    }

  }

  void
  Formula::__push_unaryOperator(char o) {

    // Only unary operator is the negative sign -
    FormulaPart t(FormulaPart::token_type::OPERATOR, '_');
    __push_operator(t);

  }

  void
  Formula::__push_operator(FormulaPart t) {

    while ( __popOperator(t) ) {
      __push_output(stack().top());
      stack().pop();
    }

    __push_stack(t);
  }

  void
  Formula::push_leftParenthesis() {
    FormulaPart t(FormulaPart::token_type::PARENTHESIS, '(');
    __push_stack(t);
  }

  void
  Formula::push_rightParenthesis() {

    while ( (not stack().empty()) and ( stack().top().character != '(' ) ) {
      __push_output(stack().top());
      stack().pop();
    }

    if (stack().empty()) {

      GUM_ERROR(OperationNotAllowed, "expecting '('");

    } else if (stack().top().character != '(') {

      GUM_ERROR(OperationNotAllowed, "expecting '('");

    }

    stack().pop();
    __last_token = FormulaPart(FormulaPart::token_type::PARENTHESIS, ')');

  }

  void
  Formula::finalize() {

    while (not stack().empty()) {

      if (stack().top().character == '(') {

        GUM_ERROR(OperationNotAllowed, "expecting ')'");

      }

      __push_output(stack().top());
      stack().pop();

    }

  }

  void
  Formula::__reduceOperator(FormulaPart item, std::stack<FormulaPart> &stack) const {

    std::vector<FormulaPart> args;

    if (stack.size() < item.argc()) {
      GUM_ERROR(OperationNotAllowed, "not enought inputs for " + item.character);
    }

    while (item.argc() > args.size()) {

      args.push_back(stack.top());
      stack.pop();

    }

    stack.push(item.eval(args));
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

  void
  Formula::push_function(FormulaPart::token_functions func) {
  }

  void
  Formula::__push_output(FormulaPart t) {
    __output.push_back(t);
    __last_token = t;
  }

  void
  Formula::__push_stack(FormulaPart t) {
    __stack.push(t);
    __last_token = t;
  }

} // namespace gum
