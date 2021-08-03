/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


/**
 * @file
 * @brief Implementation for the AST of the O3PRM language.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Lionel TORTI
 */

#include <agrum/PRM/o3prm/O3prm.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      O3Position::O3Position() : _file_(""), _line_(0), _column_(0) { GUM_CONSTRUCTOR(O3Position); }

      O3Position::O3Position(const std::string& file, int line, int column) :
          _file_(file), _line_(line), _column_(column) {
        GUM_CONSTRUCTOR(O3Position);
      }

      O3Position::O3Position(const O3Position& src) :
          _file_(src._file_), _line_(src._line_), _column_(src._column_) {
        GUM_CONS_CPY(O3Position);
      }

      O3Position::O3Position(O3Position&& src) :
          _file_(std::move(src._file_)), _line_(std::move(src._line_)),
          _column_(std::move(src._column_)) {
        GUM_CONS_MOV(O3Position);
      }

      O3Position::~O3Position() {
        GUM_DESTRUCTOR(O3Position);
        ;
      }

      O3Position& O3Position::operator=(const O3Position& src) {
        if (this == &src) { return *this; }
        _file_   = src._file_;
        _line_   = src._line_;
        _column_ = src._column_;
        return *this;
      }

      O3Position& O3Position::operator=(O3Position&& src) {
        if (this == &src) { return *this; }
        _file_   = std::move(src._file_);
        _line_   = std::move(src._line_);
        _column_ = std::move(src._column_);
        return *this;
      }

      std::string&       O3Position::file() { return _file_; }
      const std::string& O3Position::file() const { return _file_; }

      int& O3Position::line() { return _line_; }
      int  O3Position::line() const { return _line_; }

      int& O3Position::column() { return _column_; }
      int  O3Position::column() const { return _column_; }

      O3Integer::O3Integer() : _pos_(), _value_(0) {
        GUM_CONSTRUCTOR(O3Integer);
        ;
      }

      O3Integer::O3Integer(const O3Position& pos, int value) : _pos_(pos), _value_(value) {
        GUM_CONSTRUCTOR(O3Integer);
      }

      O3Integer::O3Integer(const O3Integer& src) : _pos_(src._pos_), _value_(src._value_) {
        GUM_CONS_CPY(O3Integer);
      }

      O3Integer::O3Integer(O3Integer&& src) :
          _pos_(std::move(src._pos_)), _value_(std::move(src._value_)) {
        GUM_CONS_MOV(O3Integer);
      }

      O3Integer::~O3Integer() {
        GUM_DESTRUCTOR(O3Integer);
        ;
      }

      O3Integer& O3Integer::operator=(const O3Integer& src) {
        if (this == &src) { return *this; }
        _pos_   = src._pos_;
        _value_ = src._value_;
        return *this;
      }

      O3Integer& O3Integer::operator=(O3Integer&& src) {
        if (this == &src) { return *this; }
        _pos_   = std::move(src._pos_);
        _value_ = std::move(src._value_);
        return *this;
      }

      const O3Position& O3Integer::position() const { return _pos_; }
      O3Position&       O3Integer::position() { return _pos_; }

      int  O3Integer::value() const { return _value_; }
      int& O3Integer::value() { return _value_; }

      O3Float::O3Float() {
        GUM_CONSTRUCTOR(O3Float);
        ;
      }

      O3Float::O3Float(const O3Position& pos, float value) : _pos_(pos), _value_(value) {
        GUM_CONSTRUCTOR(O3Float);
      }

      O3Float::O3Float(const O3Float& src) : _pos_(src._pos_), _value_(src._value_) {
        GUM_CONS_CPY(O3Float);
      }

      O3Float::O3Float(O3Float&& src) :
          _pos_(std::move(src._pos_)), _value_(std::move(src._value_)) {
        GUM_CONS_MOV(O3Float);
      }

      O3Float::~O3Float() {
        GUM_DESTRUCTOR(O3Float);
        ;
      }

      O3Float& O3Float::operator=(const O3Float& src) {
        if (this == &src) { return *this; }
        _pos_   = src._pos_;
        _value_ = src._value_;
        return *this;
      }

      O3Float& O3Float::operator=(O3Float&& src) {
        if (this == &src) { return *this; }
        _pos_   = std::move(src._pos_);
        _value_ = std::move(src._value_);
        return *this;
      }

      const O3Position& O3Float::position() const { return _pos_; }
      O3Position&       O3Float::position() { return _pos_; }

      float  O3Float::value() const { return _value_; }
      float& O3Float::value() { return _value_; }

      O3Formula::O3Formula() : _pos_(), _formula_(std::unique_ptr< Formula >(new Formula(""))) {
        GUM_CONSTRUCTOR(O3Formula);
      }

      O3Formula::O3Formula(const O3Position& pos, const Formula& formula) :
          _pos_(pos), _formula_(std::unique_ptr< Formula >(new Formula(formula))) {
        GUM_CONSTRUCTOR(O3Formula);
      }

      O3Formula::O3Formula(const O3Formula& src) :
          _pos_(src._pos_), _formula_(std::unique_ptr< Formula >(new Formula(src.formula()))) {
        GUM_CONS_CPY(O3Formula);
      }

      O3Formula::O3Formula(O3Formula&& src) :
          _pos_(std::move(src._pos_)), _formula_(std::move(src._formula_)) {
        GUM_CONS_MOV(O3Formula);
      }

      O3Formula::~O3Formula() {
        GUM_DESTRUCTOR(O3Formula);
        ;
      }

      O3Formula& O3Formula::operator=(const O3Formula& src) {
        if (this == &src) { return *this; }
        _pos_     = src._pos_;
        _formula_ = std::unique_ptr< Formula >(new Formula(src.formula()));
        return *this;
      }

      O3Formula& O3Formula::operator=(O3Formula&& src) {
        if (this == &src) { return *this; }
        _pos_     = std::move(src._pos_);
        _formula_ = std::move(src._formula_);
        return *this;
      }

      const O3Position& O3Formula::position() const { return _pos_; }
      O3Position&       O3Formula::position() { return _pos_; }

      const Formula& O3Formula::formula() const { return *_formula_; }
      Formula&       O3Formula::formula() { return *_formula_; }

      O3Label::O3Label() : _pos_(), _label_() {
        GUM_CONSTRUCTOR(O3Label);
        ;
      }

      O3Label::O3Label(const O3Position& pos, const std::string& label) :
          _pos_(pos), _label_(label) {
        GUM_CONSTRUCTOR(O3Label);
      }

      O3Label::O3Label(const O3Label& src) : _pos_(src._pos_), _label_(src._label_) {
        GUM_CONS_CPY(O3Label);
      }

      O3Label::O3Label(O3Label&& src) :
          _pos_(std::move(src._pos_)), _label_(std::move(src._label_)) {
        GUM_CONS_MOV(O3Label);
      }

      O3Label::~O3Label() {
        GUM_DESTRUCTOR(O3Label);
        ;
      }

      O3Label& O3Label::operator=(const O3Label& src) {
        if (this == &src) { return *this; }
        _pos_   = src._pos_;
        _label_ = src._label_;
        return *this;
      }

      O3Label& O3Label::operator=(O3Label&& src) {
        if (this == &src) { return *this; }
        _pos_   = std::move(src._pos_);
        _label_ = std::move(src._label_);
        return *this;
      }

      const O3Position& O3Label::position() const { return _pos_; }
      O3Position&       O3Label::position() { return _pos_; }

      const std::string& O3Label::label() const { return _label_; }
      std::string&       O3Label::label() { return _label_; }

      O3Type::O3Type() : _dep_flag_(false) {
        GUM_CONSTRUCTOR(O3Type);
        ;
      }

      O3Type::O3Type(const O3Type& src) :
          _pos_(src._pos_), _name_(src._name_), _superLabel_(src._superLabel_),
          _labels_(src._labels_), _dep_flag_(src._dep_flag_) {
        GUM_CONS_CPY(O3Type);
      }

      O3Type::O3Type(O3Type&& src) :
          _pos_(std::move(src._pos_)), _name_(std::move(src._name_)),
          _superLabel_(std::move(src._superLabel_)), _labels_(std::move(src._labels_)),
          _dep_flag_(std::move(src._dep_flag_)) {
        GUM_CONS_MOV(O3Type);
      }

      O3Type::~O3Type() {
        GUM_DESTRUCTOR(O3Type);
        ;
      }

      O3Type& O3Type::operator=(const O3Type& src) {
        if (this == &src) { return *this; }
        _pos_        = src._pos_;
        _name_       = src._name_;
        _superLabel_ = src._superLabel_;
        _labels_     = src._labels_;
        _dep_flag_   = src._dep_flag_;
        return *this;
      }

      O3Type& O3Type::operator=(O3Type&& src) {
        if (this == &src) { return *this; }
        _pos_        = std::move(src._pos_);
        _name_       = std::move(src._name_);
        _superLabel_ = std::move(src._superLabel_);
        _labels_     = std::move(src._labels_);
        _dep_flag_   = std::move(src._dep_flag_);
        return *this;
      }

      O3Label&       O3Type::name() { return _name_; }
      const O3Label& O3Type::name() const { return _name_; }

      O3Label&       O3Type::superLabel() { return _superLabel_; }
      const O3Label& O3Type::superLabel() const { return _superLabel_; }

      O3Type::LabelMap&       O3Type::labels() { return _labels_; }
      const O3Type::LabelMap& O3Type::labels() const { return _labels_; }

      O3Position&       O3Type::position() { return _pos_; }
      const O3Position& O3Type::position() const { return _pos_; }

      bool&       O3Type::deprecated() { return _dep_flag_; }
      const bool& O3Type::deprecated() const { return _dep_flag_; }

      O3IntType::O3IntType() : _dep_flag_(false) {
        GUM_CONSTRUCTOR(O3IntType);
        ;
      }

      O3IntType::O3IntType(const O3IntType& src) :
          _pos_(src._pos_), _name_(src._name_), _start_(src._start_), _end_(src._end_),
          _dep_flag_(src._dep_flag_) {
        GUM_CONS_CPY(O3IntType);
      }

      O3IntType::O3IntType(O3IntType&& src) :
          _pos_(std::move(src._pos_)), _name_(std::move(src._name_)),
          _start_(std::move(src._start_)), _end_(std::move(src._end_)),
          _dep_flag_(std::move(src._dep_flag_)) {
        GUM_CONS_MOV(O3IntType);
      }

      O3IntType::~O3IntType() {
        GUM_DESTRUCTOR(O3IntType);
        ;
      }

      O3IntType& O3IntType::operator=(const O3IntType& src) {
        if (this == &src) { return *this; }
        _pos_      = src._pos_;
        _name_     = src._name_;
        _start_    = src._start_;
        _end_      = src._end_;
        _dep_flag_ = src._dep_flag_;
        return *this;
      }

      O3IntType& O3IntType::operator=(O3IntType&& src) {
        if (this == &src) { return *this; }
        _pos_      = std::move(src._pos_);
        _name_     = std::move(src._name_);
        _start_    = std::move(src._start_);
        _end_      = std::move(src._end_);
        _dep_flag_ = std::move(src._dep_flag_);
        return *this;
      }

      O3Label&       O3IntType::name() { return _name_; }
      const O3Label& O3IntType::name() const { return _name_; }

      O3Integer&       O3IntType::start() { return _start_; }
      const O3Integer& O3IntType::start() const { return _start_; }

      O3Integer&       O3IntType::end() { return _end_; }
      const O3Integer& O3IntType::end() const { return _end_; }

      O3Position&       O3IntType::position() { return _pos_; }
      const O3Position& O3IntType::position() const { return _pos_; }

      bool&       O3IntType::deprecated() { return _dep_flag_; }
      const bool& O3IntType::deprecated() const { return _dep_flag_; }

      O3RealType::O3RealType() : _dep_flag_(false) {
        GUM_CONSTRUCTOR(O3RealType);
        ;
      }

      O3RealType::O3RealType(const O3RealType& src) :
          _pos_(src._pos_), _name_(src._name_), _values_(src._values_), _dep_flag_(src._dep_flag_) {
        GUM_CONS_CPY(O3RealType);
      }

      O3RealType::O3RealType(O3RealType&& src) :
          _pos_(std::move(src._pos_)), _name_(std::move(src._name_)),
          _values_(std::move(src._values_)), _dep_flag_(std::move(src._dep_flag_)) {
        GUM_CONS_MOV(O3RealType);
      }

      O3RealType::~O3RealType() {
        GUM_DESTRUCTOR(O3RealType);
        ;
      }

      O3RealType& O3RealType::operator=(const O3RealType& src) {
        if (this == &src) { return *this; }

        _pos_      = src._pos_;
        _name_     = src._name_;
        _values_   = src._values_;
        _dep_flag_ = src._dep_flag_;
        return *this;
      }

      O3RealType& O3RealType::operator=(O3RealType&& src) {
        if (this == &src) { return *this; }

        _pos_      = std::move(src._pos_);
        _name_     = std::move(src._name_);
        _values_   = std::move(src._values_);
        _dep_flag_ = std::move(src._dep_flag_);
        return *this;
      }

      O3Position&       O3RealType::position() { return _pos_; }
      const O3Position& O3RealType::position() const { return _pos_; }

      O3Label&       O3RealType::name() { return _name_; }
      const O3Label& O3RealType::name() const { return _name_; }

      std::vector< O3Float >&       O3RealType::values() { return _values_; }
      const std::vector< O3Float >& O3RealType::values() const { return _values_; }

      bool&       O3RealType::deprecated() { return _dep_flag_; }
      const bool& O3RealType::deprecated() const { return _dep_flag_; }

      O3PRM::O3PRM() {
        GUM_CONSTRUCTOR(O3PRM);
        // Creating the boolean type
        auto name   = O3Label(O3Position(), "boolean");
        auto f      = O3Label(O3Position(), "false");
        auto t      = O3Label(O3Position(), "true");
        auto labels = O3Type::LabelMap();
        labels.push_back(O3Type::LabelPair(f, O3Label()));
        labels.push_back(O3Type::LabelPair(t, O3Label()));
        auto boolean      = std::unique_ptr< O3Type >(new O3Type());
        boolean->name()   = std::move(name);
        boolean->labels() = std::move(labels);
        _types_.push_back(std::move(boolean));
      }

      O3PRM::O3PRM(const O3PRM& src) {
        GUM_CONS_CPY(O3PRM);
        for (const auto& t: src._types_) {
          _types_.emplace_back(new O3Type(*t));
        }
        for (const auto& t: src._int_types_) {
          _int_types_.emplace_back(new O3IntType(*t));
        }
        for (const auto& t: src._real_types_) {
          _real_types_.emplace_back(new O3RealType(*t));
        }
        for (const auto& i: src._interfaces_) {
          _interfaces_.emplace_back(new O3Interface(*i));
        }
        for (const auto& c: src._classes_) {
          _classes_.emplace_back(new O3Class(*c));
        }
        for (const auto& s: src._systems_) {
          _systems_.emplace_back(new O3System(*s));
        }
        for (const auto& i: src._imports_) {
          _imports_.emplace_back(new O3Import(*i));
        }
      }

      O3PRM::O3PRM(O3PRM&& src) :
          _types_(std::move(src._types_)), _int_types_(std::move(src._int_types_)),
          _real_types_(std::move(src._real_types_)), _interfaces_(std::move(src._interfaces_)),
          _classes_(std::move(src._classes_)), _systems_(std::move(src._systems_)),
          _imports_(std::move(src._imports_)) {
        GUM_CONS_MOV(O3PRM);
      }

      O3PRM::~O3PRM() {
        GUM_DESTRUCTOR(O3PRM);
        ;
      }

      O3PRM& O3PRM::operator=(const O3PRM& src) {
        if (this == &src) { return *this; }
        for (const auto& t: src._types_) {
          _types_.emplace_back(new O3Type(*t));
        }
        for (const auto& t: src._int_types_) {
          _int_types_.emplace_back(new O3IntType(*t));
        }
        for (const auto& t: src._real_types_) {
          _real_types_.emplace_back(new O3RealType(*t));
        }
        for (const auto& i: src._interfaces_) {
          _interfaces_.emplace_back(new O3Interface(*i));
        }
        for (const auto& c: src._classes_) {
          _classes_.emplace_back(new O3Class(*c));
        }
        for (const auto& s: src._systems_) {
          _systems_.emplace_back(new O3System(*s));
        }
        for (const auto& i: src._imports_) {
          _imports_.emplace_back(new O3Import(*i));
        }
        return *this;
      }

      O3PRM& O3PRM::operator=(O3PRM&& src) {
        if (this == &src) { return *this; }
        _types_      = std::move(src._types_);
        _int_types_  = std::move(src._int_types_);
        _real_types_ = std::move(src._real_types_);
        _interfaces_ = std::move(src._interfaces_);
        _classes_    = std::move(src._classes_);
        _systems_    = std::move(src._systems_);
        _imports_    = std::move(src._imports_);
        return *this;
      }

      O3PRM::O3TypeList&       O3PRM::types() { return _types_; }
      const O3PRM::O3TypeList& O3PRM::types() const { return _types_; }

      O3PRM::O3IntTypeList&       O3PRM::int_types() { return _int_types_; }
      const O3PRM::O3IntTypeList& O3PRM::int_types() const { return _int_types_; }

      O3PRM::O3RealTypeList&       O3PRM::real_types() { return _real_types_; }
      const O3PRM::O3RealTypeList& O3PRM::real_types() const { return _real_types_; }

      O3PRM::O3InterfaceList&       O3PRM::interfaces() { return _interfaces_; }
      const O3PRM::O3InterfaceList& O3PRM::interfaces() const { return _interfaces_; }

      O3PRM::O3ClassList& O3PRM::classes() { return _classes_; }

      const O3PRM::O3ClassList& O3PRM::classes() const { return _classes_; }

      O3PRM::O3SystemList& O3PRM::systems() { return _systems_; }

      const O3PRM::O3SystemList& O3PRM::systems() const { return _systems_; }

      O3PRM::O3ImportList& O3PRM::imports() { return _imports_; }

      const O3PRM::O3ImportList& O3PRM::imports() const { return _imports_; }

      O3InterfaceElement::O3InterfaceElement() : _isArray_(false) {
        GUM_CONSTRUCTOR(O3InterfaceElement);
      }

      O3InterfaceElement::O3InterfaceElement(const O3Label& type,
                                             const O3Label& name,
                                             bool           isArray) :
          _type_(type),
          _name_(name), _isArray_(isArray) {
        GUM_CONSTRUCTOR(O3InterfaceElement);
      }

      O3InterfaceElement::O3InterfaceElement(const O3InterfaceElement& src) :
          _type_(src._type_), _name_(src._name_), _isArray_(src._isArray_) {
        GUM_CONS_CPY(O3InterfaceElement);
      }

      O3InterfaceElement::O3InterfaceElement(O3InterfaceElement&& src) :
          _type_(std::move(src._type_)), _name_(std::move(src._name_)),
          _isArray_(std::move(src._isArray_)) {
        GUM_CONS_MOV(O3InterfaceElement);
      }

      O3InterfaceElement::~O3InterfaceElement() { GUM_DESTRUCTOR(O3InterfaceElement); }

      O3InterfaceElement& O3InterfaceElement::operator=(const O3InterfaceElement& src) {
        if (this == &src) { return *this; }
        _type_    = src._type_;
        _name_    = src._name_;
        _isArray_ = src._isArray_;
        return *this;
      }

      O3InterfaceElement& O3InterfaceElement::operator=(O3InterfaceElement&& src) {
        if (this == &src) { return *this; }
        _type_    = std::move(src._type_);
        _name_    = std::move(src._name_);
        _isArray_ = std::move(src._isArray_);
        return *this;
      }

      O3Label&       O3InterfaceElement::type() { return _type_; }
      const O3Label& O3InterfaceElement::type() const { return _type_; }

      O3Label&       O3InterfaceElement::name() { return _name_; }
      const O3Label& O3InterfaceElement::name() const { return _name_; }

      bool& O3InterfaceElement::isArray() { return _isArray_; }
      bool  O3InterfaceElement::isArray() const { return _isArray_; }

      O3Interface::O3Interface() : _elts_(new O3InterfaceElementList()) {
        GUM_CONSTRUCTOR(O3Interface);
      }

      O3Interface::O3Interface(const O3Interface& src) :
          _pos_(src._pos_), _name_(src._name_), _superLabel_(src._superLabel_) {
        GUM_CONS_CPY(O3Interface);
        auto copy = new O3InterfaceElementList(src.elements());
        _elts_    = std::unique_ptr< O3InterfaceElementList >(copy);
      }

      O3Interface::O3Interface(O3Interface&& src) :
          _pos_(std::move(src._pos_)), _name_(std::move(src._name_)),
          _superLabel_(std::move(src._superLabel_)), _elts_(std::move(src._elts_)) {
        GUM_CONS_MOV(O3Interface);
      }

      O3Interface::~O3Interface() {
        GUM_DESTRUCTOR(O3Interface);
        ;
      }

      O3Interface& O3Interface::operator=(const O3Interface& src) {
        if (this == &src) { return *this; }
        _pos_        = src._pos_;
        _name_       = src._name_;
        _superLabel_ = src._superLabel_;
        auto copy    = new O3InterfaceElementList(src.elements());
        _elts_       = std::unique_ptr< O3InterfaceElementList >(copy);
        return *this;
      }

      O3Interface& O3Interface::operator=(O3Interface&& src) {
        if (this == &src) { return *this; }
        _pos_        = std::move(src._pos_);
        _name_       = std::move(src._name_);
        _superLabel_ = std::move(src._superLabel_);
        _elts_       = std::move(src._elts_);
        return *this;
      }

      O3Position&       O3Interface::position() { return _pos_; }
      const O3Position& O3Interface::position() const { return _pos_; }

      O3Label&       O3Interface::name() { return _name_; }
      const O3Label& O3Interface::name() const { return _name_; }

      O3Label&       O3Interface::superLabel() { return _superLabel_; }
      const O3Label& O3Interface::superLabel() const { return _superLabel_; }

      O3Interface::O3InterfaceElementList&       O3Interface::elements() { return *_elts_; }
      const O3Interface::O3InterfaceElementList& O3Interface::elements() const { return *_elts_; }

      O3Attribute::O3Attribute() {
        GUM_CONSTRUCTOR(O3Attribute);
        ;
      }

      O3Attribute::O3Attribute(const O3Label&     type,
                               const O3Label&     name,
                               const O3LabelList& parents) :
          _type_(type),
          _name_(name), _parents_(parents) {
        GUM_CONSTRUCTOR(O3Attribute);
      }

      O3Attribute::O3Attribute(const O3Attribute& src) :
          _type_(src._type_), _name_(src._name_), _parents_(src._parents_) {
        GUM_CONS_CPY(O3Attribute);
      }

      O3Attribute::O3Attribute(O3Attribute&& src) :
          _type_(std::move(src._type_)), _name_(std::move(src._name_)),
          _parents_(std::move(src._parents_)) {
        GUM_CONS_MOV(O3Attribute);
      }

      O3Attribute::~O3Attribute() {
        GUM_DESTRUCTOR(O3Attribute);
        ;
      }

      O3Attribute& O3Attribute::operator=(const O3Attribute& src) {
        if (this == &src) { return *this; }
        _type_    = src._type_;
        _name_    = src._name_;
        _parents_ = src._parents_;
        return *this;
      }

      O3Attribute& O3Attribute::operator=(O3Attribute&& src) {
        if (this == &src) { return *this; }
        _type_    = std::move(src._type_);
        _name_    = std::move(src._name_);
        _parents_ = std::move(src._parents_);
        return *this;
      }

      O3Label&       O3Attribute::type() { return _type_; }
      const O3Label& O3Attribute::type() const { return _type_; }

      O3Label&       O3Attribute::name() { return _name_; }
      const O3Label& O3Attribute::name() const { return _name_; }

      O3Attribute::O3LabelList&       O3Attribute::parents() { return _parents_; }
      const O3Attribute::O3LabelList& O3Attribute::parents() const { return _parents_; }

      O3RawCPT::O3RawCPT() : O3Attribute(), _values_{new O3FormulaList{}} {
        GUM_CONSTRUCTOR(O3RawCPT);
      }

      O3RawCPT::O3RawCPT(const O3Label&       type,
                         const O3Label&       name,
                         const O3LabelList&   parents,
                         const O3FormulaList& values) :
          O3Attribute(type, name, parents) {
        GUM_CONSTRUCTOR(O3RawCPT);
        auto copy = new O3FormulaList(values);
        _values_  = std::unique_ptr< O3FormulaList >(copy);
      }

      O3RawCPT::O3RawCPT(const O3RawCPT& src) : O3Attribute(src) {
        GUM_CONS_CPY(O3RawCPT);
        auto copy = new O3FormulaList(*(src._values_));
        _values_  = std::unique_ptr< O3FormulaList >(copy);
      }

      O3RawCPT::O3RawCPT(O3RawCPT&& src) : O3Attribute(src), _values_(std::move(src._values_)) {
        GUM_CONS_MOV(O3RawCPT);
      }

      O3RawCPT::~O3RawCPT() {
        GUM_DESTRUCTOR(O3RawCPT);
        ;
      }

      O3RawCPT& O3RawCPT::operator=(const O3RawCPT& src) {
        if (this == &src) { return *this; }
        O3Attribute::operator=(src);
        auto         copy    = new O3FormulaList(*(src._values_));
        _values_             = std::unique_ptr< O3FormulaList >(copy);
        return *this;
      }

      O3RawCPT& O3RawCPT::operator=(O3RawCPT&& src) {
        if (this == &src) { return *this; }
        O3Attribute::operator=(src);
        _values_             = std::move(src._values_);
        return *this;
      }

      O3RawCPT::O3FormulaList& O3RawCPT::values() { return *_values_; }

      const O3RawCPT::O3FormulaList& O3RawCPT::values() const { return *_values_; }

      std::unique_ptr< O3Attribute > O3RawCPT::copy() const {
        auto copy = new O3RawCPT(*this);
        return std::unique_ptr< O3Attribute >(copy);
      }

      O3RuleCPT::O3RuleCPT() : O3Attribute(), _rules_{new O3RuleList{}} {
        GUM_CONSTRUCTOR(O3RuleCPT);
      }

      O3RuleCPT::O3RuleCPT(const O3Label&                  type,
                           const O3Label&                  name,
                           const O3Attribute::O3LabelList& parents,
                           O3RuleList&&                    rules) :
          O3Attribute(type, name, parents),
          _rules_(std::unique_ptr< O3RuleList >(new O3RuleList(rules))) {
        GUM_CONSTRUCTOR(O3RuleCPT);
      }

      O3RuleCPT::O3RuleCPT(const O3RuleCPT& src) :
          O3Attribute(src), _rules_(std::unique_ptr< O3RuleList >(new O3RuleList(src.rules()))) {
        GUM_CONS_CPY(O3RuleCPT);
      }

      O3RuleCPT::O3RuleCPT(O3RuleCPT&& src) : O3Attribute(src), _rules_(std::move(src._rules_)) {
        GUM_CONS_MOV(O3RuleCPT);
      }

      O3RuleCPT::~O3RuleCPT() {
        GUM_DESTRUCTOR(O3RuleCPT);
        ;
      }

      O3RuleCPT& O3RuleCPT::operator=(const O3RuleCPT& src) {
        if (this == &src) { return *this; }

        O3Attribute::operator=(src);
        _rules_              = std::unique_ptr< O3RuleList >(new O3RuleList(src.rules()));
        return *this;
      }

      O3RuleCPT& O3RuleCPT::operator=(O3RuleCPT&& src) {
        if (this == &src) { return *this; }

        O3Attribute::operator=(src);
        _rules_              = std::move(src._rules_);
        return *this;
      }

      O3RuleCPT::O3RuleList&       O3RuleCPT::rules() { return *_rules_; }
      const O3RuleCPT::O3RuleList& O3RuleCPT::rules() const { return *_rules_; }

      std::unique_ptr< O3Attribute > O3RuleCPT::copy() const {
        auto copy = new O3RuleCPT(*this);
        return std::unique_ptr< O3Attribute >(copy);
      }

      O3Class::O3Class() {
        GUM_CONSTRUCTOR(O3Class);

        auto i       = new O3LabelList();
        _interfaces_ = std::unique_ptr< O3LabelList >(i);

        auto p   = new O3ParameterList();
        _params_ = std::unique_ptr< O3ParameterList >(p);

        auto r = new O3ReferenceSlotList();
        _refs_ = std::unique_ptr< O3ReferenceSlotList >(r);

        auto e  = new O3AttributeList();
        _attrs_ = std::unique_ptr< O3AttributeList >(e);

        auto a = new O3AggregateList();
        _aggs_ = std::unique_ptr< O3AggregateList >(a);
      }

      O3Class::O3Class(const O3Class& src) :
          _pos_(src._pos_), _name_(src._name_), _superLabel_(src._superLabel_) {
        GUM_CONS_CPY(O3Class);
        auto i       = new O3LabelList(src.interfaces());
        _interfaces_ = std::unique_ptr< O3LabelList >(i);

        auto p   = new O3ParameterList(src.parameters());
        _params_ = std::unique_ptr< O3ParameterList >(p);

        auto r = new O3ReferenceSlotList(src.referenceSlots());
        _refs_ = std::unique_ptr< O3ReferenceSlotList >(r);

        auto e  = new O3AttributeList();
        _attrs_ = std::unique_ptr< O3AttributeList >(e);
        for (const auto& elt: src.attributes()) {
          _attrs_->push_back(elt->copy());
        }

        auto a = new O3AggregateList(src.aggregates());
        _aggs_ = std::unique_ptr< O3AggregateList >(a);
      }

      O3Class::O3Class(O3Class&& src) :
          _pos_(std::move(src._pos_)), _name_(std::move(src._name_)),
          _superLabel_(std::move(src._superLabel_)), _interfaces_(std::move(src._interfaces_)),
          _params_(std::move(src._params_)), _refs_(std::move(src._refs_)),
          _attrs_(std::move(src._attrs_)), _aggs_(std::move(src._aggs_)) {
        GUM_CONS_MOV(O3Class);
      }

      O3Class::~O3Class() {
        GUM_DESTRUCTOR(O3Class);
        ;
      }

      O3Class& O3Class::operator=(const O3Class& src) {
        if (this == &src) { return *this; }
        _pos_        = src._pos_;
        _name_       = src._name_;
        _superLabel_ = src._superLabel_;

        auto i       = new O3LabelList(src.interfaces());
        _interfaces_ = std::unique_ptr< O3LabelList >(i);

        auto p   = new O3ParameterList(src.parameters());
        _params_ = std::unique_ptr< O3ParameterList >(p);

        auto r = new O3ReferenceSlotList(src.referenceSlots());
        _refs_ = std::unique_ptr< O3ReferenceSlotList >(r);

        auto e  = new O3AttributeList();
        _attrs_ = std::unique_ptr< O3AttributeList >(e);
        for (const auto& elt: src.attributes()) {
          _attrs_->push_back(elt->copy());
        }

        auto a = new O3AggregateList(src.aggregates());
        _aggs_ = std::unique_ptr< O3AggregateList >(a);
        return *this;
      }

      O3Class& O3Class::operator=(O3Class&& src) {
        if (this == &src) { return *this; }
        _pos_        = std::move(src._pos_);
        _name_       = std::move(src._name_);
        _superLabel_ = std::move(src._superLabel_);
        _interfaces_ = std::move(src._interfaces_);
        _params_     = std::move(src._params_);
        _refs_       = std::move(src._refs_);
        _attrs_      = std::move(src._attrs_);
        _aggs_       = std::move(src._aggs_);
        return *this;
      }

      const O3Position& O3Class::position() const { return _pos_; }
      O3Position&       O3Class::position() { return _pos_; }

      const O3Label& O3Class::name() const { return _name_; }
      O3Label&       O3Class::name() { return _name_; }

      const O3Label& O3Class::superLabel() const { return _superLabel_; }
      O3Label&       O3Class::superLabel() { return _superLabel_; }

      const O3Class::O3LabelList& O3Class::interfaces() const { return *_interfaces_; }
      O3Class::O3LabelList&       O3Class::interfaces() { return *_interfaces_; }

      const O3Class::O3ParameterList& O3Class::parameters() const { return *_params_; }
      O3Class::O3ParameterList&       O3Class::parameters() { return *_params_; }

      const O3Class::O3ReferenceSlotList& O3Class::referenceSlots() const { return *_refs_; }
      O3Class::O3ReferenceSlotList&       O3Class::referenceSlots() { return *_refs_; }

      O3Class::O3AttributeList& O3Class::attributes() { return *_attrs_; }

      const O3Class::O3AttributeList& O3Class::attributes() const { return *_attrs_; }

      O3Class::O3AggregateList&       O3Class::aggregates() { return *_aggs_; }
      const O3Class::O3AggregateList& O3Class::aggregates() const { return *_aggs_; }

      O3Parameter::O3Parameter() : _type_(O3Parameter::PRMType::NONE) {
        GUM_CONSTRUCTOR(O3Parameter);
      }

      O3Parameter::O3Parameter(const O3Position& pos, const O3Label& name, const O3Integer& value) :
          _type_(O3Parameter::PRMType::INT), _pos_(pos), _name_(name),
          _value_(O3Float(value.position(), (float)value.value())) {
        GUM_CONSTRUCTOR(O3Parameter);
      }

      O3Parameter::O3Parameter(const O3Position& pos, const O3Label& name, const O3Float& value) :
          _type_(O3Parameter::PRMType::FLOAT), _pos_(pos), _name_(name), _value_(value) {
        GUM_CONSTRUCTOR(O3Parameter);
      }

      O3Parameter::O3Parameter(const O3Parameter& src) :
          _type_(src._type_), _pos_(src._pos_), _name_(src._name_), _value_(src._value_) {
        GUM_CONS_CPY(O3Parameter);
      }

      O3Parameter::O3Parameter(O3Parameter&& src) :
          _type_(std::move(src._type_)), _pos_(std::move(src._pos_)), _name_(std::move(src._name_)),
          _value_(std::move(src._value_)) {
        GUM_CONS_MOV(O3Parameter);
      }

      O3Parameter::~O3Parameter() {
        GUM_DESTRUCTOR(O3Parameter);
        ;
      }

      O3Parameter& O3Parameter::operator=(const O3Parameter& src) {
        if (this == &src) { return *this; }
        _type_  = src._type_;
        _pos_   = src._pos_;
        _name_  = src._name_;
        _value_ = src._value_;
        return *this;
      }

      O3Parameter& O3Parameter::operator=(O3Parameter&& src) {
        if (this == &src) { return *this; }
        _type_  = std::move(src._type_);
        _pos_   = std::move(src._pos_);
        _name_  = std::move(src._name_);
        _value_ = std::move(src._value_);
        return *this;
      }

      O3Parameter::PRMType& O3Parameter::type() { return _type_; }
      O3Parameter::PRMType  O3Parameter::type() const { return _type_; }

      O3Position&       O3Parameter::position() { return _pos_; }
      const O3Position& O3Parameter::position() const { return _pos_; }

      O3Label&       O3Parameter::name() { return _name_; }
      const O3Label& O3Parameter::name() const { return _name_; }

      O3Float&       O3Parameter::value() { return _value_; }
      const O3Float& O3Parameter::value() const { return _value_; }

      O3ReferenceSlot::O3ReferenceSlot() {
        GUM_CONSTRUCTOR(O3ReferenceSlot);
        ;
      }

      O3ReferenceSlot::O3ReferenceSlot(const O3Label& type, const O3Label& name, bool isArray) :
          _type_(type), _name_(name), _isArray_(isArray) {
        GUM_CONSTRUCTOR(O3ReferenceSlot);
      }

      O3ReferenceSlot::O3ReferenceSlot(const O3ReferenceSlot& src) :
          _type_(src._type_), _name_(src._name_), _isArray_(src._isArray_) {
        GUM_CONS_CPY(O3ReferenceSlot);
      }

      O3ReferenceSlot::O3ReferenceSlot(O3ReferenceSlot&& src) :
          _type_(std::move(src._type_)), _name_(std::move(src._name_)),
          _isArray_(std::move(src._isArray_)) {
        GUM_CONS_MOV(O3ReferenceSlot);
      }

      O3ReferenceSlot::~O3ReferenceSlot() {
        GUM_DESTRUCTOR(O3ReferenceSlot);
        ;
      }

      O3ReferenceSlot& O3ReferenceSlot::operator=(const O3ReferenceSlot& src) {
        if (this == &src) { return *this; }
        _type_    = src._type_;
        _name_    = src._name_;
        _isArray_ = src._isArray_;
        return *this;
      }

      O3ReferenceSlot& O3ReferenceSlot::operator=(O3ReferenceSlot&& src) {
        if (this == &src) { return *this; }
        _type_    = std::move(src._type_);
        _name_    = std::move(src._name_);
        _isArray_ = std::move(src._isArray_);
        return *this;
      }

      O3Label&       O3ReferenceSlot::type() { return _type_; }
      const O3Label& O3ReferenceSlot::type() const { return _type_; }

      O3Label&       O3ReferenceSlot::name() { return _name_; }
      const O3Label& O3ReferenceSlot::name() const { return _name_; }

      bool& O3ReferenceSlot::isArray() { return _isArray_; }
      bool  O3ReferenceSlot::isArray() const { return _isArray_; }

      O3Aggregate::O3Aggregate() :
          _variableType_(), _aggregateType_(), _name_(), _parents_(), _parameters_() {
        GUM_CONSTRUCTOR(O3Aggregate);
      }

      O3Aggregate::O3Aggregate(const O3Aggregate& src) :
          _variableType_(src._variableType_), _aggregateType_(src._aggregateType_),
          _name_(src._name_), _parents_(src._parents_), _parameters_(src._parameters_) {
        GUM_CONS_CPY(O3Aggregate);
      }

      O3Aggregate::O3Aggregate(O3Aggregate&& src) :
          _variableType_(std::move(src._variableType_)),
          _aggregateType_(std::move(src._aggregateType_)), _name_(std::move(src._name_)),
          _parents_(std::move(src._parents_)), _parameters_(std::move(src._parameters_)) {
        GUM_CONS_MOV(O3Aggregate);
      }

      O3Aggregate::~O3Aggregate() {
        GUM_DESTRUCTOR(O3Aggregate);
        ;
      }

      O3Aggregate& O3Aggregate::operator=(const O3Aggregate& src) {
        if (this == &src) { return *this; }
        _aggregateType_ = src._aggregateType_;
        _variableType_  = src._variableType_;
        _name_          = src._name_;
        _parents_       = src._parents_;
        _parameters_    = src._parameters_;
        return *this;
      }

      O3Aggregate& O3Aggregate::operator=(O3Aggregate&& src) {
        if (this == &src) { return *this; }
        _aggregateType_ = std::move(src._aggregateType_);
        _variableType_  = std::move(src._variableType_);
        _name_          = std::move(src._name_);
        _parents_       = std::move(src._parents_);
        _parameters_    = std::move(src._parameters_);
        return *this;
      }

      O3Label& O3Aggregate::variableType() { return _variableType_; }

      const O3Label& O3Aggregate::variableType() const { return _variableType_; }

      O3Label& O3Aggregate::aggregateType() { return _aggregateType_; }

      const O3Label& O3Aggregate::aggregateType() const { return _aggregateType_; }

      O3Label& O3Aggregate::name() { return _name_; }

      const O3Label& O3Aggregate::name() const { return _name_; }

      O3Aggregate::O3LabelList& O3Aggregate::parents() { return _parents_; }

      const O3Aggregate::O3LabelList& O3Aggregate::parents() const { return _parents_; }

      O3Aggregate::O3LabelList& O3Aggregate::parameters() { return _parameters_; }

      const O3Aggregate::O3LabelList& O3Aggregate::parameters() const { return _parameters_; }

      O3Assignment::O3Assignment() {
        GUM_CONSTRUCTOR(O3Assignment);
        ;
      }

      O3Assignment::O3Assignment(const O3Assignment& src) :
          _leftInstance_(src._leftInstance_), _leftIndex_(src._leftIndex_),
          _leftReference_(src._leftReference_), _rightInstance_(src._rightInstance_),
          _rightIndex_(src._rightIndex_) {
        GUM_CONS_CPY(O3Assignment);
      }

      O3Assignment::O3Assignment(O3Assignment&& src) :
          _leftInstance_(std::move(src._leftInstance_)), _leftIndex_(std::move(src._leftIndex_)),
          _leftReference_(std::move(src._leftReference_)),
          _rightInstance_(std::move(src._rightInstance_)),
          _rightIndex_(std::move(src._rightIndex_)) {
        GUM_CONS_CPY(O3Assignment);
      }

      O3Assignment::~O3Assignment() {
        GUM_DESTRUCTOR(O3Assignment);
        ;
      }

      O3Assignment& O3Assignment::operator=(const O3Assignment& src) {
        if (this == &src) { return *this; }
        _leftInstance_  = src._leftInstance_;
        _leftIndex_     = src._leftIndex_;
        _leftReference_ = src._leftReference_;
        _rightInstance_ = src._rightInstance_;
        _rightIndex_    = src._rightIndex_;
        return *this;
      }

      O3Assignment& O3Assignment::operator=(O3Assignment&& src) {
        if (this == &src) { return *this; }
        _leftInstance_  = std::move(src._leftInstance_);
        _leftIndex_     = std::move(src._leftIndex_);
        _leftReference_ = std::move(src._leftReference_);
        _rightInstance_ = std::move(src._rightInstance_);
        _rightIndex_    = std::move(src._rightIndex_);
        return *this;
      }

      const O3Label& O3Assignment::leftInstance() const { return _leftInstance_; }

      O3Label& O3Assignment::leftInstance() { return _leftInstance_; }

      const O3Integer& O3Assignment::leftIndex() const { return _leftIndex_; }

      O3Integer& O3Assignment::leftIndex() { return _leftIndex_; }

      const O3Label& O3Assignment::leftReference() const { return _leftReference_; }

      O3Label& O3Assignment::leftReference() { return _leftReference_; }

      const O3Label& O3Assignment::rightInstance() const { return _rightInstance_; }

      O3Label& O3Assignment::rightInstance() { return _rightInstance_; }

      O3Integer& O3Assignment::rightIndex() { return _rightIndex_; }

      const O3Integer& O3Assignment::rightIndex() const { return _rightIndex_; }

      O3Increment::O3Increment() {
        GUM_CONSTRUCTOR(O3Increment);
        ;
      }

      O3Increment::O3Increment(const O3Increment& src) :
          _leftInstance_(src._leftInstance_), _leftIndex_(src._leftIndex_),
          _leftReference_(src._leftReference_), _rightInstance_(src._rightInstance_),
          _rightIndex_(src._rightIndex_) {
        GUM_CONS_CPY(O3Increment);
      }

      O3Increment::O3Increment(O3Increment&& src) :
          _leftInstance_(std::move(src._leftInstance_)), _leftIndex_(std::move(src._leftIndex_)),
          _leftReference_(std::move(src._leftReference_)),
          _rightInstance_(std::move(src._rightInstance_)),
          _rightIndex_(std::move(src._rightIndex_)) {
        GUM_CONS_CPY(O3Increment);
      }

      O3Increment::~O3Increment() {
        GUM_DESTRUCTOR(O3Increment);
        ;
      }

      O3Increment& O3Increment::operator=(const O3Increment& src) {
        if (this == &src) { return *this; }
        _leftInstance_  = src._leftInstance_;
        _leftIndex_     = src._leftIndex_;
        _leftReference_ = src._leftReference_;
        _rightInstance_ = src._rightInstance_;
        _rightIndex_    = src._rightIndex_;
        return *this;
      }

      O3Increment& O3Increment::operator=(O3Increment&& src) {
        if (this == &src) { return *this; }
        _leftInstance_  = std::move(src._leftInstance_);
        _leftIndex_     = std::move(src._leftIndex_);
        _leftReference_ = std::move(src._leftReference_);
        _rightInstance_ = std::move(src._rightInstance_);
        _rightIndex_    = std::move(src._rightIndex_);
        return *this;
      }

      const O3Label& O3Increment::leftInstance() const { return _leftInstance_; }

      O3Label& O3Increment::leftInstance() { return _leftInstance_; }

      const O3Integer& O3Increment::leftIndex() const { return _leftIndex_; }

      O3Integer& O3Increment::leftIndex() { return _leftIndex_; }

      const O3Label& O3Increment::leftReference() const { return _leftReference_; }

      O3Label& O3Increment::leftReference() { return _leftReference_; }

      const O3Label& O3Increment::rightInstance() const { return _rightInstance_; }

      O3Integer&       O3Increment::rightIndex() { return _rightIndex_; }
      const O3Integer& O3Increment::rightIndex() const { return _rightIndex_; }

      O3Label& O3Increment::rightInstance() { return _rightInstance_; }

      O3Instance::O3Instance() {
        GUM_CONSTRUCTOR(O3Instance);
        ;
      }

      O3Instance::O3Instance(const O3Instance& src) :
          _type_(src._type_), _name_(src._name_), _size_(src._size_),
          _parameters_(src._parameters_) {
        GUM_CONS_CPY(O3Instance);
      }

      O3Instance::O3Instance(O3Instance&& src) :
          _type_(std::move(src._type_)), _name_(std::move(src._name_)),
          _size_(std::move(src._size_)), _parameters_(std::move(src._parameters_)) {
        GUM_CONS_MOV(O3Instance);
      }

      O3Instance::~O3Instance() {
        GUM_DESTRUCTOR(O3Instance);
        ;
      }

      O3Instance& O3Instance::operator=(const O3Instance& src) {
        if (this == &src) { return *this; }
        _type_       = src._type_;
        _name_       = src._name_;
        _size_       = src._size_;
        _parameters_ = src._parameters_;
        return *this;
      }

      O3Instance& O3Instance::operator=(O3Instance&& src) {
        if (this == &src) { return *this; }
        _type_       = std::move(src._type_);
        _name_       = std::move(src._name_);
        _size_       = std::move(src._size_);
        _parameters_ = std::move(src._parameters_);
        return *this;
      }

      const O3Label& O3Instance::type() const { return _type_; }

      O3Label& O3Instance::type() { return _type_; }

      const O3Label& O3Instance::name() const { return _name_; }

      O3Label& O3Instance::name() { return _name_; }

      const O3Integer& O3Instance::size() const { return _size_; }

      O3Integer& O3Instance::size() { return _size_; }

      const O3Instance::O3InstanceParameterList& O3Instance::parameters() const {
        return _parameters_;
      }

      O3Instance::O3InstanceParameterList& O3Instance::parameters() { return _parameters_; }

      O3System::O3System() {
        GUM_CONSTRUCTOR(O3System);
        ;
      }

      O3System::O3System(const O3System& src) :
          _name_(src._name_), _instances_(src._instances_), _assigments_(src._assigments_),
          _increments_(src._increments_) {
        GUM_CONS_CPY(O3System);
      }

      O3System::O3System(O3System&& src) :
          _name_(std::move(src._name_)), _instances_(std::move(src._instances_)),
          _assigments_(std::move(src._assigments_)), _increments_(std::move(src._increments_)) {
        GUM_CONS_MOV(O3System);
      }

      O3System::~O3System() {
        GUM_DESTRUCTOR(O3System);
        ;
      }

      O3System& O3System::operator=(const O3System& src) {
        if (this == &src) { return *this; }
        _name_       = src._name_;
        _instances_  = src._instances_;
        _assigments_ = src._assigments_;
        _increments_ = src._increments_;
        return *this;
      }

      O3System& O3System::operator=(O3System&& src) {
        if (this == &src) { return *this; }
        _name_       = std::move(src._name_);
        _instances_  = std::move(src._instances_);
        _assigments_ = std::move(src._assigments_);
        _increments_ = std::move(src._increments_);
        return *this;
      }

      const O3Label& O3System::name() const { return _name_; }

      O3Label& O3System::name() { return _name_; }

      const O3System::O3InstanceList& O3System::instances() const { return _instances_; }

      O3System::O3InstanceList& O3System::instances() { return _instances_; }

      const O3System::O3AssignmentList& O3System::assignments() const { return _assigments_; }

      O3System::O3AssignmentList& O3System::assignments() { return _assigments_; }

      const O3System::O3IncrementList& O3System::increments() const { return _increments_; }

      O3System::O3IncrementList& O3System::increments() { return _increments_; }

      std::ostream& operator<<(std::ostream& o, const O3Label& src) {
        o << src.label();
        return o;
      }

      O3InstanceParameter::O3InstanceParameter() : _isInteger_(false) {
        GUM_CONSTRUCTOR(O3InstanceParameter);
      }

      O3InstanceParameter::O3InstanceParameter(const O3InstanceParameter& src) :
          _name_(src._name_), _value_(src._value_), _isInteger_(src._isInteger_) {
        GUM_CONS_CPY(O3InstanceParameter);
      }

      O3InstanceParameter::O3InstanceParameter(O3InstanceParameter&& src) :
          _name_(std::move(src._name_)), _value_(std::move(src._value_)),
          _isInteger_(std::move(src._isInteger_)) {
        GUM_CONS_MOV(O3InstanceParameter);
      }

      O3InstanceParameter::~O3InstanceParameter() { GUM_DESTRUCTOR(O3InstanceParameter); }

      O3InstanceParameter& O3InstanceParameter::operator=(const O3InstanceParameter& src) {
        if (this == &src) { return *this; }
        _name_      = src._name_;
        _value_     = src._value_;
        _isInteger_ = src._isInteger_;
        return *this;
      }

      O3InstanceParameter& O3InstanceParameter::operator=(O3InstanceParameter&& src) {
        if (this == &src) { return *this; }
        _name_      = std::move(src._name_);
        _value_     = std::move(src._value_);
        _isInteger_ = std::move(src._isInteger_);
        return *this;
      }

      const O3Label& O3InstanceParameter::name() const { return _name_; }

      O3Label& O3InstanceParameter::name() { return _name_; }

      const O3Float& O3InstanceParameter::value() const { return _value_; }

      O3Float& O3InstanceParameter::value() { return _value_; }

      bool O3InstanceParameter::isInteger() const { return _isInteger_; }

      bool& O3InstanceParameter::isInteger() { return _isInteger_; }

      O3Import::O3Import() {
        GUM_CONSTRUCTOR(O3Import);
        ;
      }

      O3Import::O3Import(const O3Import& src) : _import_(src._import_) {
        GUM_CONSTRUCTOR(O3Import);
      }

      O3Import::O3Import(O3Import&& src) : _import_(std::move(src._import_)) {
        GUM_CONS_MOV(O3Import);
      }

      O3Import::~O3Import() {
        GUM_DESTRUCTOR(O3Import);
        ;
      }

      O3Import& O3Import::operator=(const O3Import& src) {
        if (this == &src) { return *this; }
        _import_ = src._import_;
        return *this;
      }

      O3Import& O3Import::operator=(O3Import&& src) {
        if (this == &src) { return *this; }
        _import_ = std::move(src._import_);
        return *this;
      }

      const O3Label& O3Import::import() const { return _import_; }

      O3Label& O3Import::import() { return _import_; }

    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum
