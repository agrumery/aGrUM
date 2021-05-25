/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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

// Keep this here because of cyclic dependencies
#include <agrum/tools/core/math/cocoR/Parser.h>
#include <agrum/tools/core/math/cocoR/Scanner.h>

namespace gum {

  // ==========================================================================
  // ===                         Class FormulaPart                          ===
  // ==========================================================================

  // Helper function for debuging
  void print_stack(std::stack< FormulaPart > s) {
    std::cout << std::endl;
    std::list< FormulaPart > l;
    while (!s.empty()) {
      l.push_front(s.top());
      s.pop();
    }

    std::cout << "Stack: ";
    for (const auto& elt: l) {
      std::cout << elt.str() << " ";
    }
    std::cout << std::endl;
  }

  // Helper function for debuging
  void print_output(std::vector< FormulaPart > v) {
    std::cout << "Output: ";
    for (const auto& elt: v) {
      std::cout << elt.str() << " ";
    }
    std::cout << std::endl;
  }

  std::string func2str(FormulaPart::token_function func) {
    switch (func) {
      case FormulaPart::token_function::exp: {
        return "exp";
      }
      case FormulaPart::token_function::log: {
        return "log";
      }
      case FormulaPart::token_function::ln: {
        return "ln";
      }
      case FormulaPart::token_function::pow: {
        return "pow";
      }
      case FormulaPart::token_function::sqrt: {
        return "sqrt";
      }
      case FormulaPart::token_function::nil: {
        return "nil";
      }
      default: {
        GUM_ERROR(OperationNotAllowed, "unknown function")
      }
    }
  }

  FormulaPart::FormulaPart() : type(token_type::NIL), number(NAN), character('\0'), function(nil) {
    GUM_CONSTRUCTOR(FormulaPart);
  }

  FormulaPart::FormulaPart(token_type t, double n) :
      type(t), number(n), character('\0'), function(nil) {
    GUM_CONSTRUCTOR(FormulaPart);
  }

  FormulaPart::FormulaPart(token_type t, char c) :
      type(t), number(NAN), character(c), function(nil) {
    GUM_CONSTRUCTOR(FormulaPart);
  }

  FormulaPart::FormulaPart(token_type t, token_function func) :
      type(t), number(NAN), character('\0'), function(func) {
    GUM_CONSTRUCTOR(FormulaPart);
  }

  FormulaPart::FormulaPart(const FormulaPart& source) :
      type(source.type), number(source.number), character(source.character),
      function(source.function) {
    GUM_CONS_CPY(FormulaPart);
  }

  FormulaPart::FormulaPart(FormulaPart&& source) :
      type(std::move(source.type)), number(std::move(source.number)),
      character(std::move(source.character)), function(std::move(source.function)) {
    GUM_CONS_MOV(FormulaPart);
  }

  FormulaPart::~FormulaPart() {
    GUM_DESTRUCTOR(FormulaPart);
    ;
  }

  FormulaPart& FormulaPart::operator=(const FormulaPart& source) {
    if (this == &source) { return *this; }

    type      = source.type;
    number    = source.number;
    character = source.character;
    function  = source.function;

    return *this;
  }

  FormulaPart& FormulaPart::operator=(FormulaPart&& source) {
    if (this == &source) { return *this; }

    type      = std::move(source.type);
    number    = std::move(source.number);
    character = std::move(source.character);
    function  = std::move(source.function);

    return *this;
  }

  std::string FormulaPart::str() const {
    std::ostringstream s;
    switch (type) {
      case token_type::NUMBER: {
        s << number;
        break;
      }

      case token_type::PARENTHESIS:
      case token_type::OPERATOR: {
        if (character == '\0') {
          s << "\\0";
        } else {
          s << character;
        }
        break;
      }

      case token_type::FUNCTION: {
        s << func2str(function);
        break;
      }

      default: {
        GUM_ERROR(OperationNotAllowed, "unknown type")
      }
    }
    return s.str();
  }

  // ==========================================================================
  // ===                        Class Formula                               ===
  // ==========================================================================

  void Formula::_initialise_() {
    auto c_str   = (unsigned char*)_formula_.c_str();
    auto scanner = new gum::formula::Scanner(c_str, (int)_formula_.size());
    _scanner_    = std::unique_ptr< gum::formula::Scanner >(scanner);

    auto parser = new gum::formula::Parser(scanner);
    _parser_    = std::unique_ptr< gum::formula::Parser >(parser);
    _parser_->formula(this);
  }

  Formula::Formula(short s) : _formula_(std::to_string(s)), _last_token_(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    _initialise_();
  }

  Formula::Formula(unsigned short us) : _formula_(std::to_string(us)), _last_token_(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    _initialise_();
  }

  Formula::Formula(int i) : _formula_(std::to_string(i)), _last_token_(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    _initialise_();
  }

  Formula::Formula(unsigned int ui) : _formula_(std::to_string(ui)), _last_token_(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    _initialise_();
  }

  Formula::Formula(long l) : _formula_(std::to_string(l)), _last_token_(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    _initialise_();
  }

  Formula::Formula(unsigned long ul) : _formula_(std::to_string(ul)), _last_token_(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    _initialise_();
  }

  Formula::Formula(long long l) : _formula_(std::to_string(l)), _last_token_(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    _initialise_();
  }

  Formula::Formula(unsigned long long ul) :
      _formula_(std::to_string(ul)), _last_token_(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    _initialise_();
  }

  Formula::Formula(float f) : _formula_(std::to_string(f)), _last_token_(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    _initialise_();
  }

  Formula::Formula(double d) : _formula_(std::to_string(d)), _last_token_(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    _initialise_();
  }

  Formula::Formula(const std::string& f) : _formula_(f), _last_token_(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);

    _initialise_();
  }

  Formula::Formula(const Formula& source) :
      _formula_(source._formula_), _last_token_(source._last_token_), _output_(source._output_),
      _stack_(source._stack_) {
    GUM_CONS_CPY(Formula);

    _initialise_();
  }

  Formula::Formula(Formula&& source) :
      _formula_(std::move(source._formula_)), _scanner_(std::move(source._scanner_)),
      _parser_(std::move(source._parser_)), _last_token_(std::move(source._last_token_)),
      _output_(std::move(source._output_)), _stack_(std::move(source._stack_)) {
    GUM_CONS_CPY(Formula);

    _parser_->formula(this);
  }

  Formula::~Formula() {
    GUM_DESTRUCTOR(Formula);
    ;
  }

  Formula& Formula::operator=(const Formula& source) {
    if (this == &source) { return *this; }

    _formula_    = source._formula_;
    _last_token_ = source._last_token_;
    _output_     = source._output_;
    _stack_      = source._stack_;

    _initialise_();

    return *this;
  }

  Formula& Formula::operator=(Formula&& source) {
    if (this == &source) { return *this; }

    _formula_ = std::move(source._formula_);
    _scanner_ = std::move(source._scanner_);
    _parser_  = std::move(source._parser_);
    _parser_->formula(this);
    _last_token_ = std::move(source._last_token_);
    _output_     = std::move(source._output_);
    _stack_      = std::move(source._stack_);

    return *this;
  }

  double Formula::result() const {
    _parser_->Parse();

    std::stack< FormulaPart > stack;
    if (_output_.empty()) { GUM_ERROR(OperationNotAllowed, "no output found") }

    for (auto item: _output_) {
      switch (item.type) {
        case FormulaPart::token_type::NUMBER: {
          stack.push(item);
          break;
        }

        case FormulaPart::token_type::OPERATOR:
        case FormulaPart::token_type::FUNCTION: {
          _reduceOperatorOrFunction_(item, stack);
          break;
        }

        default: {
          GUM_ERROR(OperationNotAllowed, "expecting numbers, operators or functions")
        }
      }
    }

    if (stack.size() != 1) {
      GUM_ERROR(OperationNotAllowed, "too many inputs")

    } else if (stack.top().type != FormulaPart::token_type::NUMBER) {
      GUM_ERROR(OperationNotAllowed, "too many inputs")
    }
    return stack.top().number;
  }

}   // namespace gum

#ifdef GUM_NO_INLINE
#  include <agrum/tools/core/math/formula_inl.h>
#endif   // GUM_NO_INLINE
