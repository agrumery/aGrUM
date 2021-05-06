/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <agrum/tools/core/math/formula.h>

namespace gum {

  // ==========================================================================
  // ===                         Class FormulaPart                          ===
  // ==========================================================================


  INLINE
  bool FormulaPart::isLeftAssociative() const {
    switch (character) {
      case '+':
      case '-':
      case '*':
      case '/': {
        return true;
      }

      case '_': {
        return false;
      }
      case '^': {
        return false;
      }

      default: {
        GUM_ERROR(OperationNotAllowed, "A - not an operator")
      }
    }
  }

  INLINE
  bool FormulaPart::isRightAssociative() const {
    switch (character) {
      case '_': {
        return false;
      }
      default: {
        return !isLeftAssociative();
      }
    }
  }

  INLINE
  int FormulaPart::precedence() const {
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

      default: {
        GUM_ERROR(OperationNotAllowed, "B - not an operator")
      }
    }
  }

  INLINE
  size_t FormulaPart::argc() const {
    switch (type) {
      case OPERATOR: {
        return operator_argc__();
      }

      case FUNCTION: {
        return function_argc__();
      }

      default: {
        GUM_ERROR(OperationNotAllowed, "expecting a function or an operator")
      }
    }
  }

  INLINE
  size_t FormulaPart::operator_argc__() const {
    switch (character) {
      case '_': {
        return (size_t)1;
      }
      case '+':
      case '-':
      case '*':
      case '/':
      case '^': {
        return (size_t)2;
      }

      default: {
        GUM_ERROR(OperationNotAllowed, "C - not an operator")
      }
    }
  }

  INLINE
  size_t FormulaPart::function_argc__() const {
    switch (function) {
      case FormulaPart::token_function::exp: {
        return 1;
      }
      case FormulaPart::token_function::log: {
        return 1;
      }
      case FormulaPart::token_function::ln: {
        return 1;
      }
      case FormulaPart::token_function::pow: {
        return 2;
      }
      case FormulaPart::token_function::sqrt: {
        return 1;
      }
      // case FormulaPart::token_function::nil: { return "nil"; }
      default: {
        GUM_ERROR(OperationNotAllowed, "unknown function")
      }
    }
  }

  /// Args are backwards !
  INLINE
  double
     FormulaPart::operator_eval__(const std::vector< FormulaPart >& args) const {
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

      case '_': {
        return 0 - args[0].number;
      }

      default: {
        GUM_ERROR(OperationNotAllowed, "D - not an operator")
      }
    }
  }

  /// Args are backwards !
  INLINE
  double
     FormulaPart::function_eval__(const std::vector< FormulaPart >& args) const {
    switch (function) {
      case FormulaPart::token_function::exp: {
        return std::exp(args[0].number);
      }
      case FormulaPart::token_function::log: {
        return std::log(args[0].number);
      }
      case FormulaPart::token_function::ln: {
        return std::log2(args[0].number);
      }
      case FormulaPart::token_function::pow: {
        return std::pow(args[1].number, args[0].number);
      }
      case FormulaPart::token_function::sqrt: {
        return std::sqrt(args[0].number);
      }
      // case FormulaPart::token_function::nil: { return "nil"; }
      default: {
        GUM_ERROR(OperationNotAllowed, "unknown function")
      }
    }
  }

  /// Args are backwards !
  INLINE
  FormulaPart FormulaPart::eval(const std::vector< FormulaPart >& args) const {
    switch (type) {
      case OPERATOR: {
        return FormulaPart(token_type::NUMBER, operator_eval__(args));
      }

      case FUNCTION: {
        return FormulaPart(token_type::NUMBER, function_eval__(args));
      }

      default: {
        GUM_ERROR(OperationNotAllowed, "cannot evaluate expression")
      }
    }
  }

  // ==========================================================================
  // ===                        Class Formula                               ===
  // ==========================================================================


  INLINE
  const std::string& Formula::formula() const { return formula__; }

  INLINE
  std::string& Formula::formula() { return formula__; }

  INLINE
  void Formula::push_number__(const double& v) {
    FormulaPart t(FormulaPart::token_type::NUMBER, v);
    push_output__(t);
  }

  INLINE
  bool Formula::popOperator__(FormulaPart o) {
    if (stack__.empty()
        || stack__.top().type != FormulaPart::token_type::OPERATOR) {
      return false;
    }

    if (o.isLeftAssociative() && o.precedence() <= stack__.top().precedence()) {
      return true;
    }

    if (o.isRightAssociative() && o.precedence() < stack__.top().precedence()) {
      return true;
    }

    return false;
  }

  INLINE
  void Formula::push_operator__(char o) {
    if (isUnaryOperator__(o)) {
      push_unaryOperator__(o);

    } else {
      FormulaPart t(FormulaPart::token_type::OPERATOR, o);
      push_operator__(t);
    }
  }

  INLINE
  bool Formula::isUnaryOperator__(char o) {
    switch (last_token__.type) {
      case FormulaPart::token_type::OPERATOR:
      case FormulaPart::token_type::NIL:
      case FormulaPart::token_type::ARG_SEP: {
        return o == '-';
      }

      case FormulaPart::token_type::PARENTHESIS: {
        return (o == '-') && (last_token__.character == '(');
      }

      default: {
        return false;
      }
    }
  }

  INLINE
  void Formula::push_unaryOperator__(char o) {
    // Only unary operator is the negative sign -
    FormulaPart t(FormulaPart::token_type::OPERATOR, '_');
    push_operator__(t);
  }

  INLINE
  void Formula::push_operator__(FormulaPart t) {
    while (popOperator__(t)) {
      push_output__(stack__.top());
      stack__.pop();
    }

    push_stack__(t);
  }

  INLINE
  void Formula::push_leftParenthesis__() {
    FormulaPart t(FormulaPart::token_type::PARENTHESIS, '(');
    push_stack__(t);
  }

  INLINE
  void Formula::push_rightParenthesis__() {
    while ((!stack__.empty()) && (stack__.top().character != '(')) {
      push_output__(stack__.top());
      stack__.pop();
    }

    if (stack__.empty()) {
      GUM_ERROR(OperationNotAllowed, "expecting '('")

    } else if (stack__.top().character != '(') {
      GUM_ERROR(OperationNotAllowed, "expecting '('")
    }

    stack__.pop();

    if ((!stack__.empty())
        && stack__.top().type == FormulaPart::token_type::FUNCTION) {
      push_output__(stack__.top());
      stack__.pop();
    }
    last_token__ = FormulaPart(FormulaPart::token_type::PARENTHESIS, ')');
  }

  INLINE
  void Formula::finalize__() {
    while (!stack__.empty()) {
      if (stack__.top().character == '(') {
        GUM_ERROR(OperationNotAllowed, "expecting ')'")
      }

      push_output__(stack__.top());
      stack__.pop();
    }
  }

  INLINE
  void
     Formula::reduceOperatorOrFunction__(FormulaPart                item,
                                         std::stack< FormulaPart >& stack) const {
    std::vector< FormulaPart > args;

    if (stack.size() < item.argc()) {
      GUM_ERROR(OperationNotAllowed, "not enought inputs ")
    }

    while (item.argc() > args.size()) {
      args.push_back(stack.top());
      stack.pop();
    }

    stack.push(item.eval(args));
  }

  INLINE
  void Formula::push_output__(FormulaPart t) {
    output__.push_back(t);
    last_token__ = t;
  }

  INLINE
  void Formula::push_stack__(FormulaPart t) {
    stack__.push(t);
    last_token__ = t;
  }

  INLINE
  void Formula::push_function__(const std::string& func) {
    if (func == "exp") {
      FormulaPart t(FormulaPart::token_type::FUNCTION,
                    FormulaPart::token_function::exp);
      push_stack__(t);

    } else if (func == "log") {
      FormulaPart t(FormulaPart::token_type::FUNCTION,
                    FormulaPart::token_function::log);
      push_stack__(t);

    } else if (func == "ln") {
      FormulaPart t(FormulaPart::token_type::FUNCTION,
                    FormulaPart::token_function::ln);
      push_stack__(t);

    } else if (func == "pow") {
      FormulaPart t(FormulaPart::token_type::FUNCTION,
                    FormulaPart::token_function::pow);
      push_stack__(t);

    } else if (func == "sqrt") {
      FormulaPart t(FormulaPart::token_type::FUNCTION,
                    FormulaPart::token_function::sqrt);
      push_stack__(t);

    } else {
      GUM_ERROR(OperationNotAllowed, "unknown function")
    }
  }

  INLINE
  void Formula::push_comma__() {
    while ((!stack__.empty()) && (stack__.top().character != '(')) {
      push_output__(stack__.top());
      stack__.pop();
    }

    if (stack__.empty() || stack__.top().character != '(') {
      GUM_ERROR(OperationNotAllowed, "expecting a '('")
    }

    last_token__ = FormulaPart(FormulaPart::token_type::ARG_SEP, ',');
  }

  INLINE
  HashTable< std::string, double >& Formula::variables() { return variables__; }

  INLINE
  const HashTable< std::string, double >& Formula::variables() const {
    return variables__;
  }

  INLINE
  void Formula::push_variable__(const std::string& var) {
    if (variables__.exists(var)) {
      push_number__(variables__[var]);

    } else {
      GUM_ERROR(OperationNotAllowed, "unknonw variable")
    }
  }

  INLINE
  void Formula::push_identifier__(const std::string& ident) {
    try {
      push_function__(ident);

    } catch (OperationNotAllowed&) {
      try {
        push_variable__(ident);

      } catch (OperationNotAllowed&) {
        GUM_ERROR(OperationNotAllowed, "unknown identifier")
      }
    }
  }

  // ========================================================================
  //  @name Arithmetic Operators
  // ========================================================================

  INLINE
  Formula operator-(const Formula& a) {
    return Formula(std::to_string(-1 * a.result()));
  }

  INLINE
  Formula operator+(const Formula& a, const Formula& b) {
    return Formula(std::to_string(a.result() + b.result()));
  }

  INLINE
  Formula operator-(const Formula& a, const Formula& b) {
    return Formula(std::to_string(a.result() - b.result()));
  }

  INLINE
  Formula operator*(const Formula& a, const Formula& b) {
    return Formula(std::to_string(a.result() * b.result()));
  }

  INLINE
  Formula operator/(const Formula& a, const Formula& b) {
    return Formula(std::to_string(a.result() / b.result()));
  }

  INLINE
  std::string to_string(const Formula& f) { return std::to_string(f.result()); }

  INLINE
  std::ostream& operator<<(std::ostream& os, const Formula& f) {
    os << f.result();
    return os;
  }

}   // namespace gum
