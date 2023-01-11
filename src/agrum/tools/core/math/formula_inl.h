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
        return _operator_argc_();
      }

      case FUNCTION: {
        return _function_argc_();
      }

      default: {
        GUM_ERROR(OperationNotAllowed, "expecting a function or an operator")
      }
    }
  }

  INLINE
  size_t FormulaPart::_operator_argc_() const {
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
  size_t FormulaPart::_function_argc_() const {
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
  double FormulaPart::_operator_eval_(const std::vector< FormulaPart >& args) const {
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
  double FormulaPart::_function_eval_(const std::vector< FormulaPart >& args) const {
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
        return FormulaPart(token_type::NUMBER, _operator_eval_(args));
      }

      case FUNCTION: {
        return FormulaPart(token_type::NUMBER, _function_eval_(args));
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
  const std::string& Formula::formula() const { return _formula_; }

  INLINE
  std::string& Formula::formula() { return _formula_; }

  INLINE
  void Formula::_push_number_(const double& v) {
    FormulaPart t(FormulaPart::token_type::NUMBER, v);
    _push_output_(t);
  }

  INLINE
  bool Formula::_popOperator_(FormulaPart o) {
    if (_stack_.empty() || _stack_.top().type != FormulaPart::token_type::OPERATOR) {
      return false;
    }

    if (o.isLeftAssociative() && o.precedence() <= _stack_.top().precedence()) { return true; }

    if (o.isRightAssociative() && o.precedence() < _stack_.top().precedence()) { return true; }

    return false;
  }

  INLINE
  void Formula::_push_operator_(char o) {
    if (_isUnaryOperator_(o)) {
      _push_unaryOperator_(o);

    } else {
      FormulaPart t(FormulaPart::token_type::OPERATOR, o);
      _push_operator_(t);
    }
  }

  INLINE
  bool Formula::_isUnaryOperator_(char o) {
    switch (_last_token_.type) {
      case FormulaPart::token_type::OPERATOR:
      case FormulaPart::token_type::NIL:
      case FormulaPart::token_type::ARG_SEP: {
        return o == '-';
      }

      case FormulaPart::token_type::PARENTHESIS: {
        return (o == '-') && (_last_token_.character == '(');
      }

      default: {
        return false;
      }
    }
  }

  INLINE
  void Formula::_push_unaryOperator_(char o) {
    // Only unary operator is the negative sign -
    FormulaPart t(FormulaPart::token_type::OPERATOR, '_');
    _push_operator_(t);
  }

  INLINE
  void Formula::_push_operator_(FormulaPart t) {
    while (_popOperator_(t)) {
      _push_output_(_stack_.top());
      _stack_.pop();
    }

    _push_stack_(t);
  }

  INLINE
  void Formula::_push_leftParenthesis_() {
    FormulaPart t(FormulaPart::token_type::PARENTHESIS, '(');
    _push_stack_(t);
  }

  INLINE
  void Formula::_push_rightParenthesis_() {
    while ((!_stack_.empty()) && (_stack_.top().character != '(')) {
      _push_output_(_stack_.top());
      _stack_.pop();
    }

    if (_stack_.empty()) {
      GUM_ERROR(OperationNotAllowed, "expecting '('")

    } else if (_stack_.top().character != '(') {
      GUM_ERROR(OperationNotAllowed, "expecting '('")
    }

    _stack_.pop();

    if ((!_stack_.empty()) && _stack_.top().type == FormulaPart::token_type::FUNCTION) {
      _push_output_(_stack_.top());
      _stack_.pop();
    }
    _last_token_ = FormulaPart(FormulaPart::token_type::PARENTHESIS, ')');
  }

  INLINE
  void Formula::_finalize_() {
    while (!_stack_.empty()) {
      if (_stack_.top().character == '(') { GUM_ERROR(OperationNotAllowed, "expecting ')'") }

      _push_output_(_stack_.top());
      _stack_.pop();
    }
  }

  INLINE
  void Formula::_reduceOperatorOrFunction_(FormulaPart                item,
                                           std::stack< FormulaPart >& stack) const {
    std::vector< FormulaPart > args;

    if (stack.size() < item.argc()) { GUM_ERROR(OperationNotAllowed, "not enought inputs ") }

    while (item.argc() > args.size()) {
      args.push_back(stack.top());
      stack.pop();
    }

    stack.push(item.eval(args));
  }

  INLINE
  void Formula::_push_output_(FormulaPart t) {
    _output_.push_back(t);
    _last_token_ = t;
  }

  INLINE
  void Formula::_push_stack_(FormulaPart t) {
    _stack_.push(t);
    _last_token_ = t;
  }

  INLINE
  void Formula::_push_function_(const std::string& func) {
    if (func == "exp") {
      FormulaPart t(FormulaPart::token_type::FUNCTION, FormulaPart::token_function::exp);
      _push_stack_(t);

    } else if (func == "log") {
      FormulaPart t(FormulaPart::token_type::FUNCTION, FormulaPart::token_function::log);
      _push_stack_(t);

    } else if (func == "ln") {
      FormulaPart t(FormulaPart::token_type::FUNCTION, FormulaPart::token_function::ln);
      _push_stack_(t);

    } else if (func == "pow") {
      FormulaPart t(FormulaPart::token_type::FUNCTION, FormulaPart::token_function::pow);
      _push_stack_(t);

    } else if (func == "sqrt") {
      FormulaPart t(FormulaPart::token_type::FUNCTION, FormulaPart::token_function::sqrt);
      _push_stack_(t);

    } else {
      GUM_ERROR(OperationNotAllowed, "unknown function")
    }
  }

  INLINE
  void Formula::_push_comma_() {
    while ((!_stack_.empty()) && (_stack_.top().character != '(')) {
      _push_output_(_stack_.top());
      _stack_.pop();
    }

    if (_stack_.empty() || _stack_.top().character != '(') {
      GUM_ERROR(OperationNotAllowed, "expecting a '('")
    }

    _last_token_ = FormulaPart(FormulaPart::token_type::ARG_SEP, ',');
  }

  INLINE
  HashTable< std::string, double >& Formula::variables() { return _variables_; }

  INLINE
  const HashTable< std::string, double >& Formula::variables() const { return _variables_; }

  INLINE
  void Formula::_push_variable_(const std::string& var) {
    if (_variables_.exists(var)) {
      _push_number_(_variables_[var]);

    } else {
      GUM_ERROR(OperationNotAllowed, "unknonw variable")
    }
  }

  INLINE
  void Formula::_push_identifier_(const std::string& ident) {
    try {
      _push_function_(ident);

    } catch (OperationNotAllowed const&) {
      try {
        _push_variable_(ident);

      } catch (OperationNotAllowed const&) { GUM_ERROR(OperationNotAllowed, "unknown identifier") }
    }
  }

  // ========================================================================
  //  @name Arithmetic Operators
  // ========================================================================

  INLINE
  Formula operator-(const Formula& a) { return Formula(std::to_string(-1 * a.result())); }

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
