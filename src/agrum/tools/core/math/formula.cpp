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

  FormulaPart::FormulaPart() :
      type(token_type::NIL), number(NAN), character('\0'), function(nil) {
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
      character(std::move(source.character)),
      function(std::move(source.function)) {
    GUM_CONS_MOV(FormulaPart);
  }

  FormulaPart::~FormulaPart() { GUM_DESTRUCTOR(FormulaPart); }

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

  void Formula::initialise__() {
    auto c_str   = (unsigned char*)formula__.c_str();
    auto scanner = new gum::formula::Scanner(c_str, (int)formula__.size());
    scanner__    = std::unique_ptr< gum::formula::Scanner >(scanner);

    auto parser = new gum::formula::Parser(scanner);
    parser__    = std::unique_ptr< gum::formula::Parser >(parser);
    parser__->formula(this);
  }

  Formula::Formula(short s) :
      formula__(std::to_string(s)), last_token__(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    initialise__();
  }

  Formula::Formula(unsigned short us) :
      formula__(std::to_string(us)), last_token__(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    initialise__();
  }

  Formula::Formula(int i) :
      formula__(std::to_string(i)), last_token__(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    initialise__();
  }

  Formula::Formula(unsigned int ui) :
      formula__(std::to_string(ui)), last_token__(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    initialise__();
  }

  Formula::Formula(long l) :
      formula__(std::to_string(l)), last_token__(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    initialise__();
  }

  Formula::Formula(unsigned long ul) :
      formula__(std::to_string(ul)), last_token__(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    initialise__();
  }

  Formula::Formula(long long l) :
      formula__(std::to_string(l)), last_token__(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    initialise__();
  }

  Formula::Formula(unsigned long long ul) :
      formula__(std::to_string(ul)), last_token__(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    initialise__();
  }

  Formula::Formula(float f) :
      formula__(std::to_string(f)), last_token__(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    initialise__();
  }

  Formula::Formula(double d) :
      formula__(std::to_string(d)), last_token__(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);
    initialise__();
  }

  Formula::Formula(const std::string& f) :
      formula__(f), last_token__(FormulaPart()) {
    GUM_CONSTRUCTOR(Formula);

    initialise__();
  }

  Formula::Formula(const Formula& source) :
      formula__(source.formula__), last_token__(source.last_token__),
      output__(source.output__), stack__(source.stack__) {
    GUM_CONS_CPY(Formula);

    initialise__();
  }

  Formula::Formula(Formula&& source) :
      formula__(std::move(source.formula__)),
      scanner__(std::move(source.scanner__)), parser__(std::move(source.parser__)),
      last_token__(std::move(source.last_token__)),
      output__(std::move(source.output__)), stack__(std::move(source.stack__)) {
    GUM_CONS_CPY(Formula);

    parser__->formula(this);
  }

  Formula::~Formula() { GUM_DESTRUCTOR(Formula); }

  Formula& Formula::operator=(const Formula& source) {
    if (this == &source) { return *this; }

    formula__    = source.formula__;
    last_token__ = source.last_token__;
    output__     = source.output__;
    stack__      = source.stack__;

    initialise__();

    return *this;
  }

  Formula& Formula::operator=(Formula&& source) {
    if (this == &source) { return *this; }

    formula__ = std::move(source.formula__);
    scanner__ = std::move(source.scanner__);
    parser__  = std::move(source.parser__);
    parser__->formula(this);
    last_token__ = std::move(source.last_token__);
    output__     = std::move(source.output__);
    stack__      = std::move(source.stack__);

    return *this;
  }

  double Formula::result() const {
    parser__->Parse();

    std::stack< FormulaPart > stack;
    if (output__.empty()) { GUM_ERROR(OperationNotAllowed, "no output found"); }

    for (auto item: output__) {
      switch (item.type) {
        case FormulaPart::token_type::NUMBER: {
          stack.push(item);
          break;
        }

        case FormulaPart::token_type::OPERATOR:
        case FormulaPart::token_type::FUNCTION: {
          reduceOperatorOrFunction__(item, stack);
          break;
        }

        default: {
          GUM_ERROR(OperationNotAllowed,
                    "expecting numbers, operators or functions");
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
