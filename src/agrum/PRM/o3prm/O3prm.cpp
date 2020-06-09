
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 * @author Lionel TORTI
 */

#include <agrum/PRM/o3prm/O3prm.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      O3Position::O3Position() : file__(""), line__(0), column__(0) {
        GUM_CONSTRUCTOR(O3Position);
      }

      O3Position::O3Position(const std::string& file, int line, int column) :
          file__(file), line__(line), column__(column) {
        GUM_CONSTRUCTOR(O3Position);
      }

      O3Position::O3Position(const O3Position& src) :
          file__(src.file__), line__(src.line__), column__(src.column__) {
        GUM_CONS_CPY(O3Position);
      }

      O3Position::O3Position(O3Position&& src) :
          file__(std::move(src.file__)), line__(std::move(src.line__)),
          column__(std::move(src.column__)) {
        GUM_CONS_MOV(O3Position);
      }

      O3Position::~O3Position() { GUM_DESTRUCTOR(O3Position); }

      O3Position& O3Position::operator=(const O3Position& src) {
        if (this == &src) { return *this; }
        file__ = src.file__;
        line__ = src.line__;
        column__ = src.column__;
        return *this;
      }

      O3Position& O3Position::operator=(O3Position&& src) {
        if (this == &src) { return *this; }
        file__ = std::move(src.file__);
        line__ = std::move(src.line__);
        column__ = std::move(src.column__);
        return *this;
      }

      std::string&       O3Position::file() { return file__; }
      const std::string& O3Position::file() const { return file__; }

      int& O3Position::line() { return line__; }
      int  O3Position::line() const { return line__; }

      int& O3Position::column() { return column__; }
      int  O3Position::column() const { return column__; }

      O3Integer::O3Integer() : pos__(), value__(0) { GUM_CONSTRUCTOR(O3Integer); }

      O3Integer::O3Integer(const O3Position& pos, int value) :
          pos__(pos), value__(value) {
        GUM_CONSTRUCTOR(O3Integer);
      }

      O3Integer::O3Integer(const O3Integer& src) :
          pos__(src.pos__), value__(src.value__) {
        GUM_CONS_CPY(O3Integer);
      }

      O3Integer::O3Integer(O3Integer&& src) :
          pos__(std::move(src.pos__)), value__(std::move(src.value__)) {
        GUM_CONS_MOV(O3Integer);
      }

      O3Integer::~O3Integer() { GUM_DESTRUCTOR(O3Integer); }

      O3Integer& O3Integer::operator=(const O3Integer& src) {
        if (this == &src) { return *this; }
        pos__ = src.pos__;
        value__ = src.value__;
        return *this;
      }

      O3Integer& O3Integer::operator=(O3Integer&& src) {
        if (this == &src) { return *this; }
        pos__ = std::move(src.pos__);
        value__ = std::move(src.value__);
        return *this;
      }

      const O3Position& O3Integer::position() const { return pos__; }
      O3Position&       O3Integer::position() { return pos__; }

      int  O3Integer::value() const { return value__; }
      int& O3Integer::value() { return value__; }

      O3Float::O3Float() { GUM_CONSTRUCTOR(O3Float); }

      O3Float::O3Float(const O3Position& pos, float value) :
          pos__(pos), value__(value) {
        GUM_CONSTRUCTOR(O3Float);
      }

      O3Float::O3Float(const O3Float& src) :
          pos__(src.pos__), value__(src.value__) {
        GUM_CONS_CPY(O3Float);
      }

      O3Float::O3Float(O3Float&& src) :
          pos__(std::move(src.pos__)), value__(std::move(src.value__)) {
        GUM_CONS_MOV(O3Float);
      }

      O3Float::~O3Float() { GUM_DESTRUCTOR(O3Float); }

      O3Float& O3Float::operator=(const O3Float& src) {
        if (this == &src) { return *this; }
        pos__ = src.pos__;
        value__ = src.value__;
        return *this;
      }

      O3Float& O3Float::operator=(O3Float&& src) {
        if (this == &src) { return *this; }
        pos__ = std::move(src.pos__);
        value__ = std::move(src.value__);
        return *this;
      }

      const O3Position& O3Float::position() const { return pos__; }
      O3Position&       O3Float::position() { return pos__; }

      float  O3Float::value() const { return value__; }
      float& O3Float::value() { return value__; }

      O3Formula::O3Formula() :
          pos__(), formula__(std::unique_ptr< Formula >(new Formula(""))) {
        GUM_CONSTRUCTOR(O3Formula);
      }

      O3Formula::O3Formula(const O3Position& pos, const Formula& formula) :
          pos__(pos), formula__(std::unique_ptr< Formula >(new Formula(formula))) {
        GUM_CONSTRUCTOR(O3Formula);
      }

      O3Formula::O3Formula(const O3Formula& src) :
          pos__(src.pos__),
          formula__(std::unique_ptr< Formula >(new Formula(src.formula()))) {
        GUM_CONS_CPY(O3Formula);
      }

      O3Formula::O3Formula(O3Formula&& src) :
          pos__(std::move(src.pos__)), formula__(std::move(src.formula__)) {
        GUM_CONS_MOV(O3Formula);
      }

      O3Formula::~O3Formula() { GUM_DESTRUCTOR(O3Formula); }

      O3Formula& O3Formula::operator=(const O3Formula& src) {
        if (this == &src) { return *this; }
        pos__ = src.pos__;
        formula__ = std::unique_ptr< Formula >(new Formula(src.formula()));
        return *this;
      }

      O3Formula& O3Formula::operator=(O3Formula&& src) {
        if (this == &src) { return *this; }
        pos__ = std::move(src.pos__);
        formula__ = std::move(src.formula__);
        return *this;
      }

      const O3Position& O3Formula::position() const { return pos__; }
      O3Position&       O3Formula::position() { return pos__; }

      const Formula& O3Formula::formula() const { return *formula__; }
      Formula&       O3Formula::formula() { return *formula__; }

      O3Label::O3Label() : pos__(), label__() { GUM_CONSTRUCTOR(O3Label); }

      O3Label::O3Label(const O3Position& pos, const std::string& label) :
          pos__(pos), label__(label) {
        GUM_CONSTRUCTOR(O3Label);
      }

      O3Label::O3Label(const O3Label& src) :
          pos__(src.pos__), label__(src.label__) {
        GUM_CONS_CPY(O3Label);
      }

      O3Label::O3Label(O3Label&& src) :
          pos__(std::move(src.pos__)), label__(std::move(src.label__)) {
        GUM_CONS_MOV(O3Label);
      }

      O3Label::~O3Label() { GUM_DESTRUCTOR(O3Label); }

      O3Label& O3Label::operator=(const O3Label& src) {
        if (this == &src) { return *this; }
        pos__ = src.pos__;
        label__ = src.label__;
        return *this;
      }

      O3Label& O3Label::operator=(O3Label&& src) {
        if (this == &src) { return *this; }
        pos__ = std::move(src.pos__);
        label__ = std::move(src.label__);
        return *this;
      }

      const O3Position& O3Label::position() const { return pos__; }
      O3Position&       O3Label::position() { return pos__; }

      const std::string& O3Label::label() const { return label__; }
      std::string&       O3Label::label() { return label__; }

      O3Type::O3Type() : dep_flag__(false) { GUM_CONSTRUCTOR(O3Type); }

      O3Type::O3Type(const O3Type& src) :
          pos__(src.pos__), name__(src.name__), superLabel__(src.superLabel__),
          labels__(src.labels__), dep_flag__(src.dep_flag__) {
        GUM_CONS_CPY(O3Type);
      }

      O3Type::O3Type(O3Type&& src) :
          pos__(std::move(src.pos__)), name__(std::move(src.name__)),
          superLabel__(std::move(src.superLabel__)),
          labels__(std::move(src.labels__)),
          dep_flag__(std::move(src.dep_flag__)) {
        GUM_CONS_MOV(O3Type);
      }

      O3Type::~O3Type() { GUM_DESTRUCTOR(O3Type); }

      O3Type& O3Type::operator=(const O3Type& src) {
        if (this == &src) { return *this; }
        pos__ = src.pos__;
        name__ = src.name__;
        superLabel__ = src.superLabel__;
        labels__ = src.labels__;
        dep_flag__ = src.dep_flag__;
        return *this;
      }

      O3Type& O3Type::operator=(O3Type&& src) {
        if (this == &src) { return *this; }
        pos__ = std::move(src.pos__);
        name__ = std::move(src.name__);
        superLabel__ = std::move(src.superLabel__);
        labels__ = std::move(src.labels__);
        dep_flag__ = std::move(src.dep_flag__);
        return *this;
      }

      O3Label&       O3Type::name() { return name__; }
      const O3Label& O3Type::name() const { return name__; }

      O3Label&       O3Type::superLabel() { return superLabel__; }
      const O3Label& O3Type::superLabel() const { return superLabel__; }

      O3Type::LabelMap&       O3Type::labels() { return labels__; }
      const O3Type::LabelMap& O3Type::labels() const { return labels__; }

      O3Position&       O3Type::position() { return pos__; }
      const O3Position& O3Type::position() const { return pos__; }

      bool&       O3Type::deprecated() { return dep_flag__; }
      const bool& O3Type::deprecated() const { return dep_flag__; }

      O3IntType::O3IntType() : dep_flag__(false) { GUM_CONSTRUCTOR(O3IntType); }

      O3IntType::O3IntType(const O3IntType& src) :
          pos__(src.pos__), name__(src.name__), start__(src.start__),
          end__(src.end__), dep_flag__(src.dep_flag__) {
        GUM_CONS_CPY(O3IntType);
      }

      O3IntType::O3IntType(O3IntType&& src) :
          pos__(std::move(src.pos__)), name__(std::move(src.name__)),
          start__(std::move(src.start__)), end__(std::move(src.end__)),
          dep_flag__(std::move(src.dep_flag__)) {
        GUM_CONS_MOV(O3IntType);
      }

      O3IntType::~O3IntType() { GUM_DESTRUCTOR(O3IntType); }

      O3IntType& O3IntType::operator=(const O3IntType& src) {
        if (this == &src) { return *this; }
        pos__ = src.pos__;
        name__ = src.name__;
        start__ = src.start__;
        end__ = src.end__;
        dep_flag__ = src.dep_flag__;
        return *this;
      }

      O3IntType& O3IntType::operator=(O3IntType&& src) {
        if (this == &src) { return *this; }
        pos__ = std::move(src.pos__);
        name__ = std::move(src.name__);
        start__ = std::move(src.start__);
        end__ = std::move(src.end__);
        dep_flag__ = std::move(src.dep_flag__);
        return *this;
      }

      O3Label&       O3IntType::name() { return name__; }
      const O3Label& O3IntType::name() const { return name__; }

      O3Integer&       O3IntType::start() { return start__; }
      const O3Integer& O3IntType::start() const { return start__; }

      O3Integer&       O3IntType::end() { return end__; }
      const O3Integer& O3IntType::end() const { return end__; }

      O3Position&       O3IntType::position() { return pos__; }
      const O3Position& O3IntType::position() const { return pos__; }

      bool&       O3IntType::deprecated() { return dep_flag__; }
      const bool& O3IntType::deprecated() const { return dep_flag__; }

      O3RealType::O3RealType() : dep_flag__(false) { GUM_CONSTRUCTOR(O3RealType); }

      O3RealType::O3RealType(const O3RealType& src) :
          pos__(src.pos__), name__(src.name__), values__(src.values__),
          dep_flag__(src.dep_flag__) {
        GUM_CONS_CPY(O3RealType);
      }

      O3RealType::O3RealType(O3RealType&& src) :
          pos__(std::move(src.pos__)), name__(std::move(src.name__)),
          values__(std::move(src.values__)),
          dep_flag__(std::move(src.dep_flag__)) {
        GUM_CONS_MOV(O3RealType);
      }

      O3RealType::~O3RealType() { GUM_DESTRUCTOR(O3RealType); }

      O3RealType& O3RealType::operator=(const O3RealType& src) {
        if (this == &src) { return *this; }

        pos__ = src.pos__;
        name__ = src.name__;
        values__ = src.values__;
        dep_flag__ = src.dep_flag__;
        return *this;
      }

      O3RealType& O3RealType::operator=(O3RealType&& src) {
        if (this == &src) { return *this; }

        pos__ = std::move(src.pos__);
        name__ = std::move(src.name__);
        values__ = std::move(src.values__);
        dep_flag__ = std::move(src.dep_flag__);
        return *this;
      }

      O3Position&       O3RealType::position() { return pos__; }
      const O3Position& O3RealType::position() const { return pos__; }

      O3Label&       O3RealType::name() { return name__; }
      const O3Label& O3RealType::name() const { return name__; }

      std::vector< O3Float >&       O3RealType::values() { return values__; }
      const std::vector< O3Float >& O3RealType::values() const { return values__; }

      bool&       O3RealType::deprecated() { return dep_flag__; }
      const bool& O3RealType::deprecated() const { return dep_flag__; }

      O3PRM::O3PRM() {
        GUM_CONSTRUCTOR(O3PRM);
        // Creating the boolean type
        auto name = O3Label(O3Position(), "boolean");
        auto f = O3Label(O3Position(), "false");
        auto t = O3Label(O3Position(), "true");
        auto labels = O3Type::LabelMap();
        labels.push_back(O3Type::LabelPair(f, O3Label()));
        labels.push_back(O3Type::LabelPair(t, O3Label()));
        auto boolean = std::unique_ptr< O3Type >(new O3Type());
        boolean->name() = std::move(name);
        boolean->labels() = std::move(labels);
        types__.push_back(std::move(boolean));
      }

      O3PRM::O3PRM(const O3PRM& src) {
        GUM_CONS_CPY(O3PRM);
        for (const auto& t: src.types__) {
          types__.emplace_back(new O3Type(*t));
        }
        for (const auto& t: src.int_types__) {
          int_types__.emplace_back(new O3IntType(*t));
        }
        for (const auto& t: src.real_types__) {
          real_types__.emplace_back(new O3RealType(*t));
        }
        for (const auto& i: src.interfaces__) {
          interfaces__.emplace_back(new O3Interface(*i));
        }
        for (const auto& c: src.classes__) {
          classes__.emplace_back(new O3Class(*c));
        }
        for (const auto& s: src.systems__) {
          systems__.emplace_back(new O3System(*s));
        }
        for (const auto& i: src.imports__) {
          imports__.emplace_back(new O3Import(*i));
        }
      }

      O3PRM::O3PRM(O3PRM&& src) :
          types__(std::move(src.types__)), int_types__(std::move(src.int_types__)),
          real_types__(std::move(src.real_types__)),
          interfaces__(std::move(src.interfaces__)),
          classes__(std::move(src.classes__)), systems__(std::move(src.systems__)),
          imports__(std::move(src.imports__)) {
        GUM_CONS_MOV(O3PRM);
      }

      O3PRM::~O3PRM() { GUM_DESTRUCTOR(O3PRM); }

      O3PRM& O3PRM::operator=(const O3PRM& src) {
        if (this == &src) { return *this; }
        for (const auto& t: src.types__) {
          types__.emplace_back(new O3Type(*t));
        }
        for (const auto& t: src.int_types__) {
          int_types__.emplace_back(new O3IntType(*t));
        }
        for (const auto& t: src.real_types__) {
          real_types__.emplace_back(new O3RealType(*t));
        }
        for (const auto& i: src.interfaces__) {
          interfaces__.emplace_back(new O3Interface(*i));
        }
        for (const auto& c: src.classes__) {
          classes__.emplace_back(new O3Class(*c));
        }
        for (const auto& s: src.systems__) {
          systems__.emplace_back(new O3System(*s));
        }
        for (const auto& i: src.imports__) {
          imports__.emplace_back(new O3Import(*i));
        }
        return *this;
      }

      O3PRM& O3PRM::operator=(O3PRM&& src) {
        if (this == &src) { return *this; }
        types__ = std::move(src.types__);
        int_types__ = std::move(src.int_types__);
        real_types__ = std::move(src.real_types__);
        interfaces__ = std::move(src.interfaces__);
        classes__ = std::move(src.classes__);
        systems__ = std::move(src.systems__);
        imports__ = std::move(src.imports__);
        return *this;
      }

      O3PRM::O3TypeList&       O3PRM::types() { return types__; }
      const O3PRM::O3TypeList& O3PRM::types() const { return types__; }

      O3PRM::O3IntTypeList&       O3PRM::int_types() { return int_types__; }
      const O3PRM::O3IntTypeList& O3PRM::int_types() const { return int_types__; }

      O3PRM::O3RealTypeList&       O3PRM::real_types() { return real_types__; }
      const O3PRM::O3RealTypeList& O3PRM::real_types() const {
        return real_types__;
      }

      O3PRM::O3InterfaceList&       O3PRM::interfaces() { return interfaces__; }
      const O3PRM::O3InterfaceList& O3PRM::interfaces() const {
        return interfaces__;
      }

      O3PRM::O3ClassList& O3PRM::classes() { return classes__; }

      const O3PRM::O3ClassList& O3PRM::classes() const { return classes__; }

      O3PRM::O3SystemList& O3PRM::systems() { return systems__; }

      const O3PRM::O3SystemList& O3PRM::systems() const { return systems__; }

      O3PRM::O3ImportList& O3PRM::imports() { return imports__; }

      const O3PRM::O3ImportList& O3PRM::imports() const { return imports__; }

      O3InterfaceElement::O3InterfaceElement() : isArray__(false) {
        GUM_CONSTRUCTOR(O3InterfaceElement);
      }

      O3InterfaceElement::O3InterfaceElement(const O3Label& type,
                                             const O3Label& name,
                                             bool           isArray) :
          type__(type),
          name__(name), isArray__(isArray) {
        GUM_CONSTRUCTOR(O3InterfaceElement);
      }

      O3InterfaceElement::O3InterfaceElement(const O3InterfaceElement& src) :
          type__(src.type__), name__(src.name__), isArray__(src.isArray__) {
        GUM_CONS_CPY(O3InterfaceElement);
      }

      O3InterfaceElement::O3InterfaceElement(O3InterfaceElement&& src) :
          type__(std::move(src.type__)), name__(std::move(src.name__)),
          isArray__(std::move(src.isArray__)) {
        GUM_CONS_MOV(O3InterfaceElement);
      }

      O3InterfaceElement::~O3InterfaceElement() {
        GUM_DESTRUCTOR(O3InterfaceElement);
      }

      O3InterfaceElement&
         O3InterfaceElement::operator=(const O3InterfaceElement& src) {
        if (this == &src) { return *this; }
        type__ = src.type__;
        name__ = src.name__;
        isArray__ = src.isArray__;
        return *this;
      }

      O3InterfaceElement& O3InterfaceElement::operator=(O3InterfaceElement&& src) {
        if (this == &src) { return *this; }
        type__ = std::move(src.type__);
        name__ = std::move(src.name__);
        isArray__ = std::move(src.isArray__);
        return *this;
      }

      O3Label&       O3InterfaceElement::type() { return type__; }
      const O3Label& O3InterfaceElement::type() const { return type__; }

      O3Label&       O3InterfaceElement::name() { return name__; }
      const O3Label& O3InterfaceElement::name() const { return name__; }

      bool& O3InterfaceElement::isArray() { return isArray__; }
      bool  O3InterfaceElement::isArray() const { return isArray__; }

      O3Interface::O3Interface() : elts__(new O3InterfaceElementList()) {
        GUM_CONSTRUCTOR(O3Interface);
      }

      O3Interface::O3Interface(const O3Interface& src) :
          pos__(src.pos__), name__(src.name__), superLabel__(src.superLabel__) {
        GUM_CONS_CPY(O3Interface);
        auto copy = new O3InterfaceElementList(src.elements());
        elts__ = std::unique_ptr< O3InterfaceElementList >(copy);
      }

      O3Interface::O3Interface(O3Interface&& src) :
          pos__(std::move(src.pos__)), name__(std::move(src.name__)),
          superLabel__(std::move(src.superLabel__)),
          elts__(std::move(src.elts__)) {
        GUM_CONS_MOV(O3Interface);
      }

      O3Interface::~O3Interface() { GUM_DESTRUCTOR(O3Interface); }

      O3Interface& O3Interface::operator=(const O3Interface& src) {
        if (this == &src) { return *this; }
        pos__ = src.pos__;
        name__ = src.name__;
        superLabel__ = src.superLabel__;
        auto copy = new O3InterfaceElementList(src.elements());
        elts__ = std::unique_ptr< O3InterfaceElementList >(copy);
        return *this;
      }

      O3Interface& O3Interface::operator=(O3Interface&& src) {
        if (this == &src) { return *this; }
        pos__ = std::move(src.pos__);
        name__ = std::move(src.name__);
        superLabel__ = std::move(src.superLabel__);
        elts__ = std::move(src.elts__);
        return *this;
      }

      O3Position&       O3Interface::position() { return pos__; }
      const O3Position& O3Interface::position() const { return pos__; }

      O3Label&       O3Interface::name() { return name__; }
      const O3Label& O3Interface::name() const { return name__; }

      O3Label&       O3Interface::superLabel() { return superLabel__; }
      const O3Label& O3Interface::superLabel() const { return superLabel__; }

      O3Interface::O3InterfaceElementList& O3Interface::elements() {
        return *elts__;
      }
      const O3Interface::O3InterfaceElementList& O3Interface::elements() const {
        return *elts__;
      }

      O3Attribute::O3Attribute() { GUM_CONSTRUCTOR(O3Attribute); }

      O3Attribute::O3Attribute(const O3Label&     type,
                               const O3Label&     name,
                               const O3LabelList& parents) :
          type__(type),
          name__(name), parents__(parents) {
        GUM_CONSTRUCTOR(O3Attribute);
      }

      O3Attribute::O3Attribute(const O3Attribute& src) :
          type__(src.type__), name__(src.name__), parents__(src.parents__) {
        GUM_CONS_CPY(O3Attribute);
      }

      O3Attribute::O3Attribute(O3Attribute&& src) :
          type__(std::move(src.type__)), name__(std::move(src.name__)),
          parents__(std::move(src.parents__)) {
        GUM_CONS_MOV(O3Attribute);
      }

      O3Attribute::~O3Attribute() { GUM_DESTRUCTOR(O3Attribute); }

      O3Attribute& O3Attribute::operator=(const O3Attribute& src) {
        if (this == &src) { return *this; }
        type__ = src.type__;
        name__ = src.name__;
        parents__ = src.parents__;
        return *this;
      }

      O3Attribute& O3Attribute::operator=(O3Attribute&& src) {
        if (this == &src) { return *this; }
        type__ = std::move(src.type__);
        name__ = std::move(src.name__);
        parents__ = std::move(src.parents__);
        return *this;
      }

      O3Label&       O3Attribute::type() { return type__; }
      const O3Label& O3Attribute::type() const { return type__; }

      O3Label&       O3Attribute::name() { return name__; }
      const O3Label& O3Attribute::name() const { return name__; }

      O3Attribute::O3LabelList&       O3Attribute::parents() { return parents__; }
      const O3Attribute::O3LabelList& O3Attribute::parents() const {
        return parents__;
      }

      O3RawCPT::O3RawCPT() : O3Attribute(), values__{new O3FormulaList{}} {
        GUM_CONSTRUCTOR(O3RawCPT);
      }

      O3RawCPT::O3RawCPT(const O3Label&       type,
                         const O3Label&       name,
                         const O3LabelList&   parents,
                         const O3FormulaList& values) :
          O3Attribute(type, name, parents) {
        GUM_CONSTRUCTOR(O3RawCPT);
        auto copy = new O3FormulaList(values);
        values__ = std::unique_ptr< O3FormulaList >(copy);
      }

      O3RawCPT::O3RawCPT(const O3RawCPT& src) : O3Attribute(src) {
        GUM_CONS_CPY(O3RawCPT);
        auto copy = new O3FormulaList(*(src.values__));
        values__ = std::unique_ptr< O3FormulaList >(copy);
      }

      O3RawCPT::O3RawCPT(O3RawCPT&& src) :
          O3Attribute(src), values__(std::move(src.values__)) {
        GUM_CONS_MOV(O3RawCPT);
      }

      O3RawCPT::~O3RawCPT() { GUM_DESTRUCTOR(O3RawCPT); }

      O3RawCPT& O3RawCPT::operator=(const O3RawCPT& src) {
        if (this == &src) { return *this; }
        O3Attribute::operator=(src);
        auto         copy = new O3FormulaList(*(src.values__));
        values__ = std::unique_ptr< O3FormulaList >(copy);
        return *this;
      }

      O3RawCPT& O3RawCPT::operator=(O3RawCPT&& src) {
        if (this == &src) { return *this; }
        O3Attribute::operator=(src);
        values__ = std::move(src.values__);
        return *this;
      }

      O3RawCPT::O3FormulaList& O3RawCPT::values() { return *values__; }

      const O3RawCPT::O3FormulaList& O3RawCPT::values() const { return *values__; }

      std::unique_ptr< O3Attribute > O3RawCPT::copy() const {
        auto copy = new O3RawCPT(*this);
        return std::unique_ptr< O3Attribute >(copy);
      }

      O3RuleCPT::O3RuleCPT() : O3Attribute(), rules__{new O3RuleList{}} {
        GUM_CONSTRUCTOR(O3RuleCPT);
      }

      O3RuleCPT::O3RuleCPT(const O3Label&                  type,
                           const O3Label&                  name,
                           const O3Attribute::O3LabelList& parents,
                           O3RuleList&&                    rules) :
          O3Attribute(type, name, parents),
          rules__(std::unique_ptr< O3RuleList >(new O3RuleList(rules))) {
        GUM_CONSTRUCTOR(O3RuleCPT);
      }

      O3RuleCPT::O3RuleCPT(const O3RuleCPT& src) :
          O3Attribute(src),
          rules__(std::unique_ptr< O3RuleList >(new O3RuleList(src.rules()))) {
        GUM_CONS_CPY(O3RuleCPT);
      }

      O3RuleCPT::O3RuleCPT(O3RuleCPT&& src) :
          O3Attribute(src), rules__(std::move(src.rules__)) {
        GUM_CONS_MOV(O3RuleCPT);
      }

      O3RuleCPT::~O3RuleCPT() { GUM_DESTRUCTOR(O3RuleCPT); }

      O3RuleCPT& O3RuleCPT::operator=(const O3RuleCPT& src) {
        if (this == &src) { return *this; }

        O3Attribute::operator=(src);
        rules__ = std::unique_ptr< O3RuleList >(new O3RuleList(src.rules()));
        return *this;
      }

      O3RuleCPT& O3RuleCPT::operator=(O3RuleCPT&& src) {
        if (this == &src) { return *this; }

        O3Attribute::operator=(src);
        rules__ = std::move(src.rules__);
        return *this;
      }

      O3RuleCPT::O3RuleList&       O3RuleCPT::rules() { return *rules__; }
      const O3RuleCPT::O3RuleList& O3RuleCPT::rules() const { return *rules__; }

      std::unique_ptr< O3Attribute > O3RuleCPT::copy() const {
        auto copy = new O3RuleCPT(*this);
        return std::unique_ptr< O3Attribute >(copy);
      }

      O3Class::O3Class() {
        GUM_CONSTRUCTOR(O3Class);

        auto i = new O3LabelList();
        interfaces__ = std::unique_ptr< O3LabelList >(i);

        auto p = new O3ParameterList();
        params__ = std::unique_ptr< O3ParameterList >(p);

        auto r = new O3ReferenceSlotList();
        refs__ = std::unique_ptr< O3ReferenceSlotList >(r);

        auto e = new O3AttributeList();
        attrs__ = std::unique_ptr< O3AttributeList >(e);

        auto a = new O3AggregateList();
        aggs__ = std::unique_ptr< O3AggregateList >(a);
      }

      O3Class::O3Class(const O3Class& src) :
          pos__(src.pos__), name__(src.name__), superLabel__(src.superLabel__) {
        GUM_CONS_CPY(O3Class);
        auto i = new O3LabelList(src.interfaces());
        interfaces__ = std::unique_ptr< O3LabelList >(i);

        auto p = new O3ParameterList(src.parameters());
        params__ = std::unique_ptr< O3ParameterList >(p);

        auto r = new O3ReferenceSlotList(src.referenceSlots());
        refs__ = std::unique_ptr< O3ReferenceSlotList >(r);

        auto e = new O3AttributeList();
        attrs__ = std::unique_ptr< O3AttributeList >(e);
        for (const auto& elt: src.attributes()) {
          attrs__->push_back(elt->copy());
        }

        auto a = new O3AggregateList(src.aggregates());
        aggs__ = std::unique_ptr< O3AggregateList >(a);
      }

      O3Class::O3Class(O3Class&& src) :
          pos__(std::move(src.pos__)), name__(std::move(src.name__)),
          superLabel__(std::move(src.superLabel__)),
          interfaces__(std::move(src.interfaces__)),
          params__(std::move(src.params__)), refs__(std::move(src.refs__)),
          attrs__(std::move(src.attrs__)), aggs__(std::move(src.aggs__)) {
        GUM_CONS_MOV(O3Class);
      }

      O3Class::~O3Class() { GUM_DESTRUCTOR(O3Class); }

      O3Class& O3Class::operator=(const O3Class& src) {
        if (this == &src) { return *this; }
        pos__ = src.pos__;
        name__ = src.name__;
        superLabel__ = src.superLabel__;

        auto i = new O3LabelList(src.interfaces());
        interfaces__ = std::unique_ptr< O3LabelList >(i);

        auto p = new O3ParameterList(src.parameters());
        params__ = std::unique_ptr< O3ParameterList >(p);

        auto r = new O3ReferenceSlotList(src.referenceSlots());
        refs__ = std::unique_ptr< O3ReferenceSlotList >(r);

        auto e = new O3AttributeList();
        attrs__ = std::unique_ptr< O3AttributeList >(e);
        for (const auto& elt: src.attributes()) {
          attrs__->push_back(elt->copy());
        }

        auto a = new O3AggregateList(src.aggregates());
        aggs__ = std::unique_ptr< O3AggregateList >(a);
        return *this;
      }

      O3Class& O3Class::operator=(O3Class&& src) {
        if (this == &src) { return *this; }
        pos__ = std::move(src.pos__);
        name__ = std::move(src.name__);
        superLabel__ = std::move(src.superLabel__);
        interfaces__ = std::move(src.interfaces__);
        params__ = std::move(src.params__);
        refs__ = std::move(src.refs__);
        attrs__ = std::move(src.attrs__);
        aggs__ = std::move(src.aggs__);
        return *this;
      }

      const O3Position& O3Class::position() const { return pos__; }
      O3Position&       O3Class::position() { return pos__; }

      const O3Label& O3Class::name() const { return name__; }
      O3Label&       O3Class::name() { return name__; }

      const O3Label& O3Class::superLabel() const { return superLabel__; }
      O3Label&       O3Class::superLabel() { return superLabel__; }

      const O3Class::O3LabelList& O3Class::interfaces() const {
        return *interfaces__;
      }
      O3Class::O3LabelList& O3Class::interfaces() { return *interfaces__; }

      const O3Class::O3ParameterList& O3Class::parameters() const {
        return *params__;
      }
      O3Class::O3ParameterList& O3Class::parameters() { return *params__; }

      const O3Class::O3ReferenceSlotList& O3Class::referenceSlots() const {
        return *refs__;
      }
      O3Class::O3ReferenceSlotList& O3Class::referenceSlots() { return *refs__; }

      O3Class::O3AttributeList& O3Class::attributes() { return *attrs__; }

      const O3Class::O3AttributeList& O3Class::attributes() const {
        return *attrs__;
      }

      O3Class::O3AggregateList&       O3Class::aggregates() { return *aggs__; }
      const O3Class::O3AggregateList& O3Class::aggregates() const {
        return *aggs__;
      }

      O3Parameter::O3Parameter() : type__(O3Parameter::PRMType::NONE) {
        GUM_CONSTRUCTOR(O3Parameter);
      }

      O3Parameter::O3Parameter(const O3Position& pos,
                               const O3Label&    name,
                               const O3Integer&  value) :
          type__(O3Parameter::PRMType::INT),
          pos__(pos), name__(name),
          value__(O3Float(value.position(), (float)value.value())) {
        GUM_CONSTRUCTOR(O3Parameter);
      }

      O3Parameter::O3Parameter(const O3Position& pos,
                               const O3Label&    name,
                               const O3Float&    value) :
          type__(O3Parameter::PRMType::FLOAT),
          pos__(pos), name__(name), value__(value) {
        GUM_CONSTRUCTOR(O3Parameter);
      }

      O3Parameter::O3Parameter(const O3Parameter& src) :
          type__(src.type__), pos__(src.pos__), name__(src.name__),
          value__(src.value__) {
        GUM_CONS_CPY(O3Parameter);
      }

      O3Parameter::O3Parameter(O3Parameter&& src) :
          type__(std::move(src.type__)), pos__(std::move(src.pos__)),
          name__(std::move(src.name__)), value__(std::move(src.value__)) {
        GUM_CONS_MOV(O3Parameter);
      }

      O3Parameter::~O3Parameter() { GUM_DESTRUCTOR(O3Parameter); }

      O3Parameter& O3Parameter::operator=(const O3Parameter& src) {
        if (this == &src) { return *this; }
        type__ = src.type__;
        pos__ = src.pos__;
        name__ = src.name__;
        value__ = src.value__;
        return *this;
      }

      O3Parameter& O3Parameter::operator=(O3Parameter&& src) {
        if (this == &src) { return *this; }
        type__ = std::move(src.type__);
        pos__ = std::move(src.pos__);
        name__ = std::move(src.name__);
        value__ = std::move(src.value__);
        return *this;
      }

      O3Parameter::PRMType& O3Parameter::type() { return type__; }
      O3Parameter::PRMType  O3Parameter::type() const { return type__; }

      O3Position&       O3Parameter::position() { return pos__; }
      const O3Position& O3Parameter::position() const { return pos__; }

      O3Label&       O3Parameter::name() { return name__; }
      const O3Label& O3Parameter::name() const { return name__; }

      O3Float&       O3Parameter::value() { return value__; }
      const O3Float& O3Parameter::value() const { return value__; }

      O3ReferenceSlot::O3ReferenceSlot() { GUM_CONSTRUCTOR(O3ReferenceSlot); }

      O3ReferenceSlot::O3ReferenceSlot(const O3Label& type,
                                       const O3Label& name,
                                       bool           isArray) :
          type__(type),
          name__(name), isArray__(isArray) {
        GUM_CONSTRUCTOR(O3ReferenceSlot);
      }

      O3ReferenceSlot::O3ReferenceSlot(const O3ReferenceSlot& src) :
          type__(src.type__), name__(src.name__), isArray__(src.isArray__) {
        GUM_CONS_CPY(O3ReferenceSlot);
      }

      O3ReferenceSlot::O3ReferenceSlot(O3ReferenceSlot&& src) :
          type__(std::move(src.type__)), name__(std::move(src.name__)),
          isArray__(std::move(src.isArray__)) {
        GUM_CONS_MOV(O3ReferenceSlot);
      }

      O3ReferenceSlot::~O3ReferenceSlot() { GUM_DESTRUCTOR(O3ReferenceSlot); }

      O3ReferenceSlot& O3ReferenceSlot::operator=(const O3ReferenceSlot& src) {
        if (this == &src) { return *this; }
        type__ = src.type__;
        name__ = src.name__;
        isArray__ = src.isArray__;
        return *this;
      }

      O3ReferenceSlot& O3ReferenceSlot::operator=(O3ReferenceSlot&& src) {
        if (this == &src) { return *this; }
        type__ = std::move(src.type__);
        name__ = std::move(src.name__);
        isArray__ = std::move(src.isArray__);
        return *this;
      }

      O3Label&       O3ReferenceSlot::type() { return type__; }
      const O3Label& O3ReferenceSlot::type() const { return type__; }

      O3Label&       O3ReferenceSlot::name() { return name__; }
      const O3Label& O3ReferenceSlot::name() const { return name__; }

      bool& O3ReferenceSlot::isArray() { return isArray__; }
      bool  O3ReferenceSlot::isArray() const { return isArray__; }

      O3Aggregate::O3Aggregate() :
          variableType__(), aggregateType__(), name__(), parents__(),
          parameters__() {
        GUM_CONSTRUCTOR(O3Aggregate);
      }

      O3Aggregate::O3Aggregate(const O3Aggregate& src) :
          variableType__(src.variableType__), aggregateType__(src.aggregateType__),
          name__(src.name__), parents__(src.parents__),
          parameters__(src.parameters__) {
        GUM_CONS_CPY(O3Aggregate);
      }

      O3Aggregate::O3Aggregate(O3Aggregate&& src) :
          variableType__(std::move(src.variableType__)),
          aggregateType__(std::move(src.aggregateType__)),
          name__(std::move(src.name__)), parents__(std::move(src.parents__)),
          parameters__(std::move(src.parameters__)) {
        GUM_CONS_MOV(O3Aggregate);
      }

      O3Aggregate::~O3Aggregate() { GUM_DESTRUCTOR(O3Aggregate); }

      O3Aggregate& O3Aggregate::operator=(const O3Aggregate& src) {
        if (this == &src) { return *this; }
        aggregateType__ = src.aggregateType__;
        variableType__ = src.variableType__;
        name__ = src.name__;
        parents__ = src.parents__;
        parameters__ = src.parameters__;
        return *this;
      }

      O3Aggregate& O3Aggregate::operator=(O3Aggregate&& src) {
        if (this == &src) { return *this; }
        aggregateType__ = std::move(src.aggregateType__);
        variableType__ = std::move(src.variableType__);
        name__ = std::move(src.name__);
        parents__ = std::move(src.parents__);
        parameters__ = std::move(src.parameters__);
        return *this;
      }

      O3Label& O3Aggregate::variableType() { return variableType__; }

      const O3Label& O3Aggregate::variableType() const { return variableType__; }

      O3Label& O3Aggregate::aggregateType() { return aggregateType__; }

      const O3Label& O3Aggregate::aggregateType() const { return aggregateType__; }

      O3Label& O3Aggregate::name() { return name__; }

      const O3Label& O3Aggregate::name() const { return name__; }

      O3Aggregate::O3LabelList& O3Aggregate::parents() { return parents__; }

      const O3Aggregate::O3LabelList& O3Aggregate::parents() const {
        return parents__;
      }

      O3Aggregate::O3LabelList& O3Aggregate::parameters() { return parameters__; }

      const O3Aggregate::O3LabelList& O3Aggregate::parameters() const {
        return parameters__;
      }

      O3Assignment::O3Assignment() { GUM_CONSTRUCTOR(O3Assignment); }

      O3Assignment::O3Assignment(const O3Assignment& src) :
          leftInstance__(src.leftInstance__), leftIndex__(src.leftIndex__),
          leftReference__(src.leftReference__),
          rightInstance__(src.rightInstance__), rightIndex__(src.rightIndex__) {
        GUM_CONS_CPY(O3Assignment);
      }

      O3Assignment::O3Assignment(O3Assignment&& src) :
          leftInstance__(std::move(src.leftInstance__)),
          leftIndex__(std::move(src.leftIndex__)),
          leftReference__(std::move(src.leftReference__)),
          rightInstance__(std::move(src.rightInstance__)),
          rightIndex__(std::move(src.rightIndex__)) {
        GUM_CONS_CPY(O3Assignment);
      }

      O3Assignment::~O3Assignment() { GUM_DESTRUCTOR(O3Assignment); }

      O3Assignment& O3Assignment::operator=(const O3Assignment& src) {
        if (this == &src) { return *this; }
        leftInstance__ = src.leftInstance__;
        leftIndex__ = src.leftIndex__;
        leftReference__ = src.leftReference__;
        rightInstance__ = src.rightInstance__;
        rightIndex__ = src.rightIndex__;
        return *this;
      }

      O3Assignment& O3Assignment::operator=(O3Assignment&& src) {
        if (this == &src) { return *this; }
        leftInstance__ = std::move(src.leftInstance__);
        leftIndex__ = std::move(src.leftIndex__);
        leftReference__ = std::move(src.leftReference__);
        rightInstance__ = std::move(src.rightInstance__);
        rightIndex__ = std::move(src.rightIndex__);
        return *this;
      }

      const O3Label& O3Assignment::leftInstance() const { return leftInstance__; }

      O3Label& O3Assignment::leftInstance() { return leftInstance__; }

      const O3Integer& O3Assignment::leftIndex() const { return leftIndex__; }

      O3Integer& O3Assignment::leftIndex() { return leftIndex__; }

      const O3Label& O3Assignment::leftReference() const {
        return leftReference__;
      }

      O3Label& O3Assignment::leftReference() { return leftReference__; }

      const O3Label& O3Assignment::rightInstance() const {
        return rightInstance__;
      }

      O3Label& O3Assignment::rightInstance() { return rightInstance__; }

      O3Integer& O3Assignment::rightIndex() { return rightIndex__; }

      const O3Integer& O3Assignment::rightIndex() const { return rightIndex__; }

      O3Increment::O3Increment() { GUM_CONSTRUCTOR(O3Increment); }

      O3Increment::O3Increment(const O3Increment& src) :
          leftInstance__(src.leftInstance__), leftIndex__(src.leftIndex__),
          leftReference__(src.leftReference__),
          rightInstance__(src.rightInstance__), rightIndex__(src.rightIndex__) {
        GUM_CONS_CPY(O3Increment);
      }

      O3Increment::O3Increment(O3Increment&& src) :
          leftInstance__(std::move(src.leftInstance__)),
          leftIndex__(std::move(src.leftIndex__)),
          leftReference__(std::move(src.leftReference__)),
          rightInstance__(std::move(src.rightInstance__)),
          rightIndex__(std::move(src.rightIndex__)) {
        GUM_CONS_CPY(O3Increment);
      }

      O3Increment::~O3Increment() { GUM_DESTRUCTOR(O3Increment); }

      O3Increment& O3Increment::operator=(const O3Increment& src) {
        if (this == &src) { return *this; }
        leftInstance__ = src.leftInstance__;
        leftIndex__ = src.leftIndex__;
        leftReference__ = src.leftReference__;
        rightInstance__ = src.rightInstance__;
        rightIndex__ = src.rightIndex__;
        return *this;
      }

      O3Increment& O3Increment::operator=(O3Increment&& src) {
        if (this == &src) { return *this; }
        leftInstance__ = std::move(src.leftInstance__);
        leftIndex__ = std::move(src.leftIndex__);
        leftReference__ = std::move(src.leftReference__);
        rightInstance__ = std::move(src.rightInstance__);
        rightIndex__ = std::move(src.rightIndex__);
        return *this;
      }

      const O3Label& O3Increment::leftInstance() const { return leftInstance__; }

      O3Label& O3Increment::leftInstance() { return leftInstance__; }

      const O3Integer& O3Increment::leftIndex() const { return leftIndex__; }

      O3Integer& O3Increment::leftIndex() { return leftIndex__; }

      const O3Label& O3Increment::leftReference() const { return leftReference__; }

      O3Label& O3Increment::leftReference() { return leftReference__; }

      const O3Label& O3Increment::rightInstance() const { return rightInstance__; }

      O3Integer&       O3Increment::rightIndex() { return rightIndex__; }
      const O3Integer& O3Increment::rightIndex() const { return rightIndex__; }

      O3Label& O3Increment::rightInstance() { return rightInstance__; }

      O3Instance::O3Instance() { GUM_CONSTRUCTOR(O3Instance); }

      O3Instance::O3Instance(const O3Instance& src) :
          type__(src.type__), name__(src.name__), size__(src.size__),
          parameters__(src.parameters__) {
        GUM_CONS_CPY(O3Instance);
      }

      O3Instance::O3Instance(O3Instance&& src) :
          type__(std::move(src.type__)), name__(std::move(src.name__)),
          size__(std::move(src.size__)),
          parameters__(std::move(src.parameters__)) {
        GUM_CONS_MOV(O3Instance);
      }

      O3Instance::~O3Instance() { GUM_DESTRUCTOR(O3Instance); }

      O3Instance& O3Instance::operator=(const O3Instance& src) {
        if (this == &src) { return *this; }
        type__ = src.type__;
        name__ = src.name__;
        size__ = src.size__;
        parameters__ = src.parameters__;
        return *this;
      }

      O3Instance& O3Instance::operator=(O3Instance&& src) {
        if (this == &src) { return *this; }
        type__ = std::move(src.type__);
        name__ = std::move(src.name__);
        size__ = std::move(src.size__);
        parameters__ = std::move(src.parameters__);
        return *this;
      }

      const O3Label& O3Instance::type() const { return type__; }

      O3Label& O3Instance::type() { return type__; }

      const O3Label& O3Instance::name() const { return name__; }

      O3Label& O3Instance::name() { return name__; }

      const O3Integer& O3Instance::size() const { return size__; }

      O3Integer& O3Instance::size() { return size__; }

      const O3Instance::O3InstanceParameterList& O3Instance::parameters() const {
        return parameters__;
      }

      O3Instance::O3InstanceParameterList& O3Instance::parameters() {
        return parameters__;
      }

      O3System::O3System() { GUM_CONSTRUCTOR(O3System); }

      O3System::O3System(const O3System& src) :
          name__(src.name__), instances__(src.instances__),
          assigments__(src.assigments__), increments__(src.increments__) {
        GUM_CONS_CPY(O3System);
      }

      O3System::O3System(O3System&& src) :
          name__(std::move(src.name__)), instances__(std::move(src.instances__)),
          assigments__(std::move(src.assigments__)),
          increments__(std::move(src.increments__)) {
        GUM_CONS_MOV(O3System);
      }

      O3System::~O3System() { GUM_DESTRUCTOR(O3System); }

      O3System& O3System::operator=(const O3System& src) {
        if (this == &src) { return *this; }
        name__ = src.name__;
        instances__ = src.instances__;
        assigments__ = src.assigments__;
        increments__ = src.increments__;
        return *this;
      }

      O3System& O3System::operator=(O3System&& src) {
        if (this == &src) { return *this; }
        name__ = std::move(src.name__);
        instances__ = std::move(src.instances__);
        assigments__ = std::move(src.assigments__);
        increments__ = std::move(src.increments__);
        return *this;
      }

      const O3Label& O3System::name() const { return name__; }

      O3Label& O3System::name() { return name__; }

      const O3System::O3InstanceList& O3System::instances() const {
        return instances__;
      }

      O3System::O3InstanceList& O3System::instances() { return instances__; }

      const O3System::O3AssignmentList& O3System::assignments() const {
        return assigments__;
      }

      O3System::O3AssignmentList& O3System::assignments() { return assigments__; }

      const O3System::O3IncrementList& O3System::increments() const {
        return increments__;
      }

      O3System::O3IncrementList& O3System::increments() { return increments__; }

      std::ostream& operator<<(std::ostream& o, const O3Label& src) {
        o << src.label();
        return o;
      }

      O3InstanceParameter::O3InstanceParameter() : isInteger__(false) {
        GUM_CONSTRUCTOR(O3InstanceParameter);
      }

      O3InstanceParameter::O3InstanceParameter(const O3InstanceParameter& src) :
          name__(src.name__), value__(src.value__), isInteger__(src.isInteger__) {
        GUM_CONS_CPY(O3InstanceParameter);
      }

      O3InstanceParameter::O3InstanceParameter(O3InstanceParameter&& src) :
          name__(std::move(src.name__)), value__(std::move(src.value__)),
          isInteger__(std::move(src.isInteger__)) {
        GUM_CONS_MOV(O3InstanceParameter);
      }

      O3InstanceParameter::~O3InstanceParameter() {
        GUM_DESTRUCTOR(O3InstanceParameter);
      }

      O3InstanceParameter&
         O3InstanceParameter::operator=(const O3InstanceParameter& src) {
        if (this == &src) { return *this; }
        name__ = src.name__;
        value__ = src.value__;
        isInteger__ = src.isInteger__;
        return *this;
      }

      O3InstanceParameter&
         O3InstanceParameter::operator=(O3InstanceParameter&& src) {
        if (this == &src) { return *this; }
        name__ = std::move(src.name__);
        value__ = std::move(src.value__);
        isInteger__ = std::move(src.isInteger__);
        return *this;
      }

      const O3Label& O3InstanceParameter::name() const { return name__; }

      O3Label& O3InstanceParameter::name() { return name__; }

      const O3Float& O3InstanceParameter::value() const { return value__; }

      O3Float& O3InstanceParameter::value() { return value__; }

      bool O3InstanceParameter::isInteger() const { return isInteger__; }

      bool& O3InstanceParameter::isInteger() { return isInteger__; }

      O3Import::O3Import() { GUM_CONSTRUCTOR(O3Import); }

      O3Import::O3Import(const O3Import& src) : import__(src.import__) {
        GUM_CONSTRUCTOR(O3Import);
      }

      O3Import::O3Import(O3Import&& src) : import__(std::move(src.import__)) {
        GUM_CONS_MOV(O3Import);
      }

      O3Import::~O3Import() { GUM_DESTRUCTOR(O3Import); }

      O3Import& O3Import::operator=(const O3Import& src) {
        if (this == &src) { return *this; }
        import__ = src.import__;
        return *this;
      }

      O3Import& O3Import::operator=(O3Import&& src) {
        if (this == &src) { return *this; }
        import__ = std::move(src.import__);
        return *this;
      }

      const O3Label& O3Import::import() const { return import__; }

      O3Label& O3Import::import() { return import__; }

    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum
