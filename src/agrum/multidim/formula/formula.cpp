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

#include <agrum/multidim/formula/formula.h>

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

  std::string func2str(FormulaPart::token_function func) {
    switch (func) {
      case FormulaPart::token_function::exp: { return "exp"; } 
      case FormulaPart::token_function::log: { return "log"; } 
      case FormulaPart::token_function::ln: { return "ln"; } 
      case FormulaPart::token_function::pow: { return "pow"; } 
      case FormulaPart::token_function::sqrt: { return "sqrt"; } 
      case FormulaPart::token_function::bernoulli: { return "bernoulli"; } 
      case FormulaPart::token_function::binomial: { return "binomial"; }
      case FormulaPart::token_function::geometric: { return "geometric"; } 
      case FormulaPart::token_function::negative_binomial: { return "negative_binomial"; } 
      case FormulaPart::token_function::poisson: { return "poisson"; } 
      case FormulaPart::token_function::exponential: { return "exponential"; } 
      case FormulaPart::token_function::gamma: { return "gamma"; } 
      case FormulaPart::token_function::weibull: { return "weibull"; }
      case FormulaPart::token_function::extreme_value: { return "extreme_value"; } 
      case FormulaPart::token_function::normal: { return "normal"; } 
      case FormulaPart::token_function::lognormal: { return "lognormal"; } 
      case FormulaPart::token_function::chi_squared: { return "chi_squared"; } 
      case FormulaPart::token_function::cauchy: { return "cauchy"; } 
      case FormulaPart::token_function::fisher_f: { return "fisher_f"; } 
      case FormulaPart::token_function::student_t: { return "student_t"; }
      case FormulaPart::token_function::discrete: { return "discrete"; } 
      case FormulaPart::token_function::piecewise_constant: { return "piecewise_constant"; } 
      case FormulaPart::token_function::piecewise_linear: { return "piecewise_linear"; }
      case FormulaPart::token_function::nil: { return "nil"; }
      default: { GUM_ERROR( OperationNotAllowed, "unknown function" ); }
    }
  }

  FormulaPart::FormulaPart():
    type(token_type::NIL), number(NAN), character('\0'), function(nil)
  {
    GUM_CONSTRUCTOR( FormulaPart );
  }

  FormulaPart::FormulaPart(token_type t, double n):
    type(t), number(n), character('\0') , function(nil)
  {
    GUM_CONSTRUCTOR( FormulaPart );
  }

  FormulaPart::FormulaPart(token_type t, char c):
    type(t), number(NAN), character(c), function(nil)
  {
    GUM_CONSTRUCTOR( FormulaPart );
  }

  FormulaPart::FormulaPart(token_type t, token_function func):
    type(t), number(NAN), character('\0'), function(func) 
  {
    GUM_CONSTRUCTOR( FormulaPart );
  }

  FormulaPart::FormulaPart(const FormulaPart & source):
    type(source.type), number(source.number),
    character(source.character), function(source.function)
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
    function = source.function;

    return *this;
  }

  std::string
  FormulaPart::str() const {
    std::ostringstream s;
    switch (type) {
      case token_type::NUMBER:
        {
          s << number; 
          break;
        }

      case token_type::PARENTHESIS:
      case token_type::OPERATOR:
        {
          if (character == '\0') {
            s << "\\0";
          } else {
            s << character;
          }
          break;
        }

      case token_type::FUNCTION:
      {
        s << func2str(function);
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
    switch (type) {
      case OPERATOR:
        {
          return __operator_argc();
        }

      case FUNCTION:
        {
          return __function_argc();
        }

        default:
        {
          GUM_ERROR( OperationNotAllowed, "expecting a function or an operator");
        }
    }
  }

  size_t
  FormulaPart::__operator_argc() const {
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

  size_t
  FormulaPart::__function_argc() const {
    switch (function) {
      case FormulaPart::token_function::exp: { return 1; } 
      case FormulaPart::token_function::log: { return 1; } 
      case FormulaPart::token_function::ln: { return 1; } 
      case FormulaPart::token_function::pow: { return 2; } 
      case FormulaPart::token_function::sqrt: { return 1; } 
      // case FormulaPart::token_function::bernoulli: { return "bernoulli"; } 
      // case FormulaPart::token_function::binomial: { return "binomial"; }
      // case FormulaPart::token_function::geometric: { return "geometric"; } 
      // case FormulaPart::token_function::negative_binomial: { return "negative_binomial"; } 
      // case FormulaPart::token_function::poisson: { return "poisson"; } 
      // case FormulaPart::token_function::exponential: { return "exponential"; } 
      // case FormulaPart::token_function::gamma: { return "gamma"; } 
      // case FormulaPart::token_function::weibull: { return "weibull"; }
      // case FormulaPart::token_function::extreme_value: { return "extreme_value"; } 
      // case FormulaPart::token_function::normal: { return "normal"; } 
      // case FormulaPart::token_function::lognormal: { return "lognormal"; } 
      // case FormulaPart::token_function::chi_squared: { return "chi_squared"; } 
      // case FormulaPart::token_function::cauchy: { return "cauchy"; } 
      // case FormulaPart::token_function::fisher_f: { return "fisher_f"; } 
      // case FormulaPart::token_function::student_t: { return "student_t"; }
      // case FormulaPart::token_function::discrete: { return "discrete"; } 
      // case FormulaPart::token_function::piecewise_constant: { return "piecewise_constant"; } 
      // case FormulaPart::token_function::piecewise_linear: { return "piecewise_linear"; }
      // case FormulaPart::token_function::nil: { return "nil"; }
      default: { GUM_ERROR( OperationNotAllowed, "unknown function" ); }
    }

  }

  /// Args are backwards !
  double
  FormulaPart::__operator_eval(const std::vector<FormulaPart>& args) const {
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
  double
  FormulaPart::__function_eval(const std::vector<FormulaPart>& args) const {
    switch (function) {
      case FormulaPart::token_function::exp: { return std::exp(args[0].number); } 
      case FormulaPart::token_function::log: { return std::log(args[0].number); } 
      case FormulaPart::token_function::ln: { return std::log2(args[0].number); } 
      case FormulaPart::token_function::pow: { return std::pow(args[1].number, args[0].number); } 
      case FormulaPart::token_function::sqrt: { return std::sqrt(args[0].number); } 
      // case FormulaPart::token_function::bernoulli: { return "bernoulli"; } 
      // case FormulaPart::token_function::binomial: { return "binomial"; }
      // case FormulaPart::token_function::geometric: { return "geometric"; } 
      // case FormulaPart::token_function::negative_binomial: { return "negative_binomial"; } 
      // case FormulaPart::token_function::poisson: { return "poisson"; } 
      // case FormulaPart::token_function::exponential: { return "exponential"; } 
      // case FormulaPart::token_function::gamma: { return "gamma"; } 
      // case FormulaPart::token_function::weibull: { return "weibull"; }
      // case FormulaPart::token_function::extreme_value: { return "extreme_value"; } 
      // case FormulaPart::token_function::normal: { return "normal"; } 
      // case FormulaPart::token_function::lognormal: { return "lognormal"; } 
      // case FormulaPart::token_function::chi_squared: { return "chi_squared"; } 
      // case FormulaPart::token_function::cauchy: { return "cauchy"; } 
      // case FormulaPart::token_function::fisher_f: { return "fisher_f"; } 
      // case FormulaPart::token_function::student_t: { return "student_t"; }
      // case FormulaPart::token_function::discrete: { return "discrete"; } 
      // case FormulaPart::token_function::piecewise_constant: { return "piecewise_constant"; } 
      // case FormulaPart::token_function::piecewise_linear: { return "piecewise_linear"; }
      // case FormulaPart::token_function::nil: { return "nil"; }
      default: { GUM_ERROR( OperationNotAllowed, "unknown function" ); }
    }
  }

  /// Args are backwards !
  FormulaPart
  FormulaPart::eval(const std::vector<FormulaPart>& args) const {
    switch (type) {
      case OPERATOR:
        {
          return FormulaPart(token_type::NUMBER, __operator_eval(args));
        }

      case FUNCTION:
        {
          return FormulaPart(token_type::NUMBER, __function_eval(args));
        }

      default:
        {
          GUM_ERROR( OperationNotAllowed, "cannot evaluate expression" );
        }
    }
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
        case FormulaPart::token_type::NUMBER:
          {
            stack.push(item);
            break;
          }

        case FormulaPart::token_type::OPERATOR: 
        case FormulaPart::token_type::FUNCTION:
          {
            __reduceOperatorOrFunction(item, stack);
            break;
          }

        default:
          {
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
      case FormulaPart::token_type::ARG_SEP:
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
    
    if ( stack().top().type == FormulaPart::token_type::FUNCTION ) {

      __push_output(stack().top());
      stack().pop();

    }
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
  Formula::__reduceOperatorOrFunction(FormulaPart item, std::stack<FormulaPart> &stack) const {

    std::vector<FormulaPart> args;

    if (stack.size() < item.argc()) {
      GUM_ERROR(OperationNotAllowed, "not enought inputs ");
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
  Formula::__push_output(FormulaPart t) {
    __output.push_back(t);
    __last_token = t;
  }

  void
  Formula::__push_stack(FormulaPart t) {
    __stack.push(t);
    __last_token = t;
  }

  void
  Formula::push_function(const std::string & func) {

    if (func == "exp") {

      FormulaPart t(FormulaPart::token_type::FUNCTION, FormulaPart::token_function::exp);
      __push_stack(t);

    } else if (func == "log") {

      FormulaPart t(FormulaPart::token_type::FUNCTION, FormulaPart::token_function::log);
      __push_stack(t);

    } else if (func == "ln") {

      FormulaPart t(FormulaPart::token_type::FUNCTION, FormulaPart::token_function::ln);
      __push_stack(t);

    } else if (func == "pow") {

      FormulaPart t(FormulaPart::token_type::FUNCTION, FormulaPart::token_function::pow);
      __push_stack(t);

    } else if (func == "sqrt") {

      FormulaPart t(FormulaPart::token_type::FUNCTION, FormulaPart::token_function::sqrt);
      __push_stack(t);

    } else {

      GUM_ERROR( OperationNotAllowed, "unknown function" );

    }

  }

  void 
  Formula::push_comma() {
    while ( (not stack().empty()) and (stack().top().character != '(' )) {
      __push_output(stack().top());
      stack().pop();
    }
    
    if (stack().empty() or stack().top().character != '(' ) {
      GUM_ERROR( OperationNotAllowed, "expecting a '('" );
    }

    __last_token = FormulaPart(FormulaPart::token_type::ARG_SEP, ',');
  }

  HashTable<std::string, double> &
  Formula::variables() {
    return __variables;
  }

  const HashTable<std::string, double> &
  Formula::variables() const {
    return __variables;
  }

  void 
  Formula::push_variable(const std::string & var) {

    if (__variables.exists(var)) {

      push_number(__variables[var]);

    } else {

      GUM_ERROR( OperationNotAllowed, "unknonw variable" );

    }

  }

  void
  Formula::push_identifier(const std::string & ident) {
    try {

      push_function( ident );

    } catch (OperationNotAllowed& error) {

      try {

        push_variable( ident );

      } catch (OperationNotAllowed& error) {

        GUM_ERROR( OperationNotAllowed, "unknown identifier" );

      }
    }

  }

} // namespace gum
