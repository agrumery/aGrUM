/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


#include <agrum/base/core/math/formula.h>

namespace gum {

  // ==========================================================================
  // ===                         Class FormulaPart                          ===
  // ==========================================================================



  INLINE
  bool FormulaPart::isRightAssociative() const {
    switch (character) {
      case '_' : {
        return false;
      }
      default : {
        return !isLeftAssociative();
      }
    }
  }





  /// Args are backwards !

  /// Args are backwards !

  /// Args are backwards !

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
      case FormulaPart::token_type::OPERATOR :
      case FormulaPart::token_type::NIL :
      case FormulaPart::token_type::ARG_SEP : {
        return o == '-';
      }

      case FormulaPart::token_type::PARENTHESIS : {
        return (o == '-') && (_last_token_.character == '(');
      }

      default : {
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
  void Formula::_push_variable_(std::string_view var) {
    if (_variables_.exists(var)) {
      _push_number_(_variables_[var]);

    } else {
      GUM_ERROR(OperationNotAllowed, "unknonw variable")
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

  INLINE
  Formula::operator double() const { return result(); }

}   // namespace gum
