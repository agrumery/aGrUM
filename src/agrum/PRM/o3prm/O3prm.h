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


/**
 * @file
 * @brief Headers for the AST of the O3PRM language.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Lionel TORTI
 */

#include <string>
#include <utility>
#include <vector>

#include <agrum/agrum.h>
#include <agrum/tools/core/math/formula.h>

#ifndef GUM_PRM_O3PRM_PRM_H
#  define GUM_PRM_O3PRM_PRM_H

namespace gum {
  namespace prm {
    namespace o3prm {

      /**
       * @class O3Position
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3Position is part of the AST of the O3PRM language.
       */
      class O3Position {
        public:
        O3Position();
        O3Position(const std::string& filename, int line, int column);
        O3Position(const O3Position& src);
        O3Position(O3Position&& src);
        ~O3Position();

        O3Position& operator=(const O3Position& src);
        O3Position& operator=(O3Position&& src);

        std::string&       file();
        const std::string& file() const;

        int& line();
        int  line() const;

        int& column();
        int  column() const;

        private:
        std::string _file_;
        int         _line_;
        int         _column_;
      };

      /**
       * @class O3Formula
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3Formula is part of the AST of the O3PRM language.
       */
      class O3Formula {
        public:
        O3Formula();
        O3Formula(const O3Position& pos, const Formula& formula);
        O3Formula(const O3Formula& src);
        O3Formula(O3Formula&& src);
        ~O3Formula();

        O3Formula& operator=(const O3Formula& src);
        O3Formula& operator=(O3Formula&& src);

        O3Position&       position();
        const O3Position& position() const;

        Formula&       formula();
        const Formula& formula() const;

        private:
        O3Position                 _pos_;
        std::unique_ptr< Formula > _formula_;
      };

      /**
       * @class O3Float
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3Float is part of the AST of the O3PRM language.
       */
      class O3Float {
        public:
        O3Float();
        O3Float(const O3Position& pos, float value);
        O3Float(const O3Float& src);
        O3Float(O3Float&& src);
        ~O3Float();

        O3Float& operator=(const O3Float& src);
        O3Float& operator=(O3Float&& src);

        O3Position&       position();
        const O3Position& position() const;

        float& value();
        float  value() const;

        private:
        O3Position _pos_;
        float      _value_;
      };

      /**
       * @class O3Integer
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3Integer is part of the AST of the O3PRM language.
       */
      class O3Integer {
        public:
        O3Integer();
        O3Integer(const O3Position& pos, int value);
        O3Integer(const O3Integer& src);
        O3Integer(O3Integer&& src);
        ~O3Integer();

        O3Integer& operator=(const O3Integer& src);
        O3Integer& operator=(O3Integer&& src);

        O3Position&       position();
        const O3Position& position() const;

        int& value();
        int  value() const;

        private:
        O3Position _pos_;
        int        _value_;
      };

      /**
       * @class O3Label
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3Label is part of the AST of the O3PRM language.
       */
      class O3Label {
        public:
        O3Label();
        O3Label(const O3Position& pos, const std::string& label);
        O3Label(const O3Label& src);
        O3Label(O3Label&& src);
        ~O3Label();

        O3Label& operator=(const O3Label& src);
        O3Label& operator=(O3Label&& src);

        O3Position&       position();
        const O3Position& position() const;

        std::string&       label();
        const std::string& label() const;

        private:
        O3Position  _pos_;
        std::string _label_;
      };

      std::ostream& operator<<(std::ostream& o, const O3Label& src);

      /**
       * @class O3Type
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3Type is part of the AST of the O3PRM language.
       */
      class O3Type {
        public:
        using LabelPair = std::pair< O3Label, O3Label >;
        using LabelMap  = std::vector< LabelPair >;

        O3Type();
        O3Type(const O3Type& src);
        O3Type(O3Type&& src);
        ~O3Type();

        O3Type& operator=(const O3Type& src);
        O3Type& operator=(O3Type&& src);

        O3Position&       position();
        const O3Position& position() const;

        O3Label&       name();
        const O3Label& name() const;

        O3Label&       superLabel();
        const O3Label& superLabel() const;

        LabelMap&       labels();
        const LabelMap& labels() const;

        bool&       deprecated();
        const bool& deprecated() const;

        private:
        O3Position _pos_;
        O3Label    _name_;
        O3Label    _superLabel_;
        LabelMap   _labels_;
        bool       _dep_flag_;
      };

      /**
       * @class O3IntType
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3IntType is part of the AST of the O3PRM language.
       */
      class O3IntType {
        public:
        O3IntType();
        O3IntType(const O3IntType& src);
        O3IntType(O3IntType&& src);
        ~O3IntType();

        O3IntType& operator=(const O3IntType& src);
        O3IntType& operator=(O3IntType&& src);

        O3Position&       position();
        const O3Position& position() const;

        O3Label&       name();
        const O3Label& name() const;

        O3Integer&       start();
        const O3Integer& start() const;

        O3Integer&       end();
        const O3Integer& end() const;

        bool&       deprecated();
        const bool& deprecated() const;

        private:
        O3Position _pos_;
        O3Label    _name_;
        O3Integer  _start_;
        O3Integer  _end_;
        bool       _dep_flag_;
      };

      /**
       * @class O3RealType
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3RealType is part of the AST of the O3PRM language.
       */
      class O3RealType {
        public:
        O3RealType();
        O3RealType(const O3RealType& src);
        O3RealType(O3RealType&& src);
        ~O3RealType();

        O3RealType& operator=(const O3RealType& src);
        O3RealType& operator=(O3RealType&& src);

        O3Position&       position();
        const O3Position& position() const;

        O3Label&       name();
        const O3Label& name() const;

        std::vector< O3Float >&       values();
        const std::vector< O3Float >& values() const;

        bool&       deprecated();
        const bool& deprecated() const;

        private:
        O3Position             _pos_;
        O3Label                _name_;
        std::vector< O3Float > _values_;
        bool                   _dep_flag_;
      };

      /**
       * @class O3InterfaceElement
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3InterfaceElement is part of the AST of the O3PRM language.
       */
      class O3InterfaceElement {
        public:
        O3InterfaceElement();
        O3InterfaceElement(const O3Label& type, const O3Label& name, bool isArray);
        O3InterfaceElement(const O3InterfaceElement& src);
        O3InterfaceElement(O3InterfaceElement&& src);
        ~O3InterfaceElement();

        O3InterfaceElement& operator=(const O3InterfaceElement& src);
        O3InterfaceElement& operator=(O3InterfaceElement&& src);

        O3Label&       type();
        const O3Label& type() const;

        O3Label&       name();
        const O3Label& name() const;

        bool& isArray();
        bool  isArray() const;

        private:
        O3Label _type_;
        O3Label _name_;
        bool    _isArray_;
      };

      /**
       * @class O3Interface
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3Interface is part of the AST of the O3PRM language.
       */
      class O3Interface {
        public:
        using O3InterfaceElementList = std::vector< O3InterfaceElement >;

        O3Interface();
        O3Interface(const O3Interface& src);
        O3Interface(O3Interface&& src);
        ~O3Interface();

        O3Interface& operator=(const O3Interface& src);
        O3Interface& operator=(O3Interface&& src);

        O3Position&       position();
        const O3Position& position() const;

        O3Label&       name();
        const O3Label& name() const;

        O3Label&       superLabel();
        const O3Label& superLabel() const;

        O3InterfaceElementList&       elements();
        const O3InterfaceElementList& elements() const;

        private:
        O3Position                                _pos_;
        O3Label                                   _name_;
        O3Label                                   _superLabel_;
        std::unique_ptr< O3InterfaceElementList > _elts_;
      };

      /**
       * @class O3Parameter
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3Parameter is part of the AST of the O3PRM language.
       */
      class O3Parameter {
        public:
        enum class PRMType {
          NONE,
          INT,
          FLOAT
        };

        O3Parameter();
        O3Parameter(const O3Position& pos, const O3Label& name, const O3Integer& value);
        O3Parameter(const O3Position& pos, const O3Label& name, const O3Float& value);
        O3Parameter(const O3Parameter& src);
        O3Parameter(O3Parameter&& src);
        ~O3Parameter();

        O3Parameter& operator=(const O3Parameter& src);
        O3Parameter& operator=(O3Parameter&& src);

        PRMType& type();
        PRMType  type() const;

        O3Position&       position();
        const O3Position& position() const;

        O3Label&       name();
        const O3Label& name() const;

        O3Float&       value();
        const O3Float& value() const;

        private:
        PRMType    _type_;
        O3Position _pos_;
        O3Label    _name_;
        O3Float    _value_;
      };

      /**
       * @class O3ReferenceSlot
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3ReferenceSlot is part of the AST of the O3PRM language.
       */
      class O3ReferenceSlot {
        public:
        O3ReferenceSlot();
        O3ReferenceSlot(const O3Label& type, const O3Label& name, bool isArray);
        O3ReferenceSlot(const O3ReferenceSlot& src);
        O3ReferenceSlot(O3ReferenceSlot&& src);
        ~O3ReferenceSlot();

        O3ReferenceSlot& operator=(const O3ReferenceSlot& src);
        O3ReferenceSlot& operator=(O3ReferenceSlot&& src);

        O3Label&       type();
        const O3Label& type() const;

        O3Label&       name();
        const O3Label& name() const;

        bool& isArray();
        bool  isArray() const;

        private:
        O3Label _type_;
        O3Label _name_;
        bool    _isArray_;
      };

      /**
       * @class O3Attribute
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3Attribute is part of the AST of the O3PRM language.
       */
      class O3Attribute {
        public:
        using O3LabelList = std::vector< O3Label >;

        O3Attribute();
        O3Attribute(const O3Label& type, const O3Label& name, const O3LabelList& parents);
        O3Attribute(const O3Attribute& src);
        O3Attribute(O3Attribute&& src);
        virtual ~O3Attribute();

        virtual O3Attribute& operator=(const O3Attribute& src);
        virtual O3Attribute& operator=(O3Attribute&& src);

        virtual O3Label&       type();
        virtual const O3Label& type() const;

        virtual O3Label&       name();
        virtual const O3Label& name() const;

        virtual O3LabelList&       parents();
        virtual const O3LabelList& parents() const;

        virtual std::unique_ptr< O3Attribute > copy() const = 0;

        private:
        O3Label     _type_;
        O3Label     _name_;
        O3LabelList _parents_;
      };

      /**
       * @class O3RawCPT
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3RawCPT is part of the AST of the O3PRM language.
       */
      class O3RawCPT: public O3Attribute {
        public:
        using O3FormulaList = std::vector< O3Formula >;

        O3RawCPT();
        O3RawCPT(const O3Label&                  type,
                 const O3Label&                  name,
                 const O3Attribute::O3LabelList& parents,
                 const O3FormulaList&            values);
        O3RawCPT(const O3RawCPT& src);
        O3RawCPT(O3RawCPT&& src);
        ~O3RawCPT();

        virtual O3RawCPT& operator=(const O3RawCPT& src);
        virtual O3RawCPT& operator=(O3RawCPT&& src);

        virtual O3FormulaList&       values();
        virtual const O3FormulaList& values() const;

        virtual std::unique_ptr< O3Attribute > copy() const;

        private:
        std::unique_ptr< O3FormulaList > _values_;
      };

      /**
       * @class O3RuleCPT
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3RuleCPT is part of the AST of the O3PRM language.
       */
      class O3RuleCPT: public O3Attribute {
        public:
        using O3LabelList   = std::vector< O3Label >;
        using O3FormulaList = std::vector< O3Formula >;
        using O3Rule        = std::pair< O3LabelList, O3FormulaList >;
        using O3RuleList    = std::vector< O3Rule >;

        O3RuleCPT();
        O3RuleCPT(const O3Label&                  type,
                  const O3Label&                  name,
                  const O3Attribute::O3LabelList& parents,
                  O3RuleList&&                    rules);
        O3RuleCPT(const O3RuleCPT& src);
        O3RuleCPT(O3RuleCPT&& src);
        ~O3RuleCPT();

        virtual O3RuleCPT& operator=(const O3RuleCPT& src);
        virtual O3RuleCPT& operator=(O3RuleCPT&& src);

        virtual O3RuleList&       rules();
        virtual const O3RuleList& rules() const;

        virtual std::unique_ptr< O3Attribute > copy() const;

        private:
        std::unique_ptr< O3RuleList > _rules_;
      };

      /**
       * @class O3Aggregate
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3Aggregate is part of the AST of the O3PRM language.
       */
      class O3Aggregate {
        public:
        using O3LabelList = std::vector< O3Label >;

        O3Aggregate();
        O3Aggregate(const O3Aggregate& src);
        O3Aggregate(O3Aggregate&& src);
        virtual ~O3Aggregate();

        O3Aggregate& operator=(const O3Aggregate& src);
        O3Aggregate& operator=(O3Aggregate&& src);

        O3Label&       variableType();
        const O3Label& variableType() const;

        O3Label&       aggregateType();
        const O3Label& aggregateType() const;

        O3Label&       name();
        const O3Label& name() const;

        O3LabelList&       parents();
        const O3LabelList& parents() const;

        O3LabelList&       parameters();
        const O3LabelList& parameters() const;

        private:
        O3Label     _variableType_;
        O3Label     _aggregateType_;
        O3Label     _name_;
        O3LabelList _parents_;
        O3LabelList _parameters_;
      };

      /**
       * @class O3Class
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3Class is part of the AST of the O3PRM language.
       */
      class O3Class {
        public:
        using O3LabelList         = std::vector< O3Label >;
        using O3ParameterList     = std::vector< O3Parameter >;
        using O3ReferenceSlotList = std::vector< O3ReferenceSlot >;
        using O3AttributeList     = std::vector< std::unique_ptr< O3Attribute > >;
        using O3AggregateList     = std::vector< O3Aggregate >;

        O3Class();
        O3Class(const O3Class& src);
        O3Class(O3Class&& src);
        ~O3Class();

        O3Class& operator=(const O3Class& src);
        O3Class& operator=(O3Class&& src);

        O3Position&       position();
        const O3Position& position() const;

        O3Label&       name();
        const O3Label& name() const;

        O3Label&       superLabel();
        const O3Label& superLabel() const;

        O3LabelList&       interfaces();
        const O3LabelList& interfaces() const;

        O3ParameterList&       parameters();
        const O3ParameterList& parameters() const;

        O3ReferenceSlotList&       referenceSlots();
        const O3ReferenceSlotList& referenceSlots() const;

        O3AttributeList&       attributes();
        const O3AttributeList& attributes() const;

        O3AggregateList&       aggregates();
        const O3AggregateList& aggregates() const;

        private:
        O3Position                             _pos_;
        O3Label                                _name_;
        O3Label                                _superLabel_;
        std::unique_ptr< O3LabelList >         _interfaces_;
        std::unique_ptr< O3ParameterList >     _params_;
        std::unique_ptr< O3ReferenceSlotList > _refs_;
        std::unique_ptr< O3AttributeList >     _attrs_;
        std::unique_ptr< O3AggregateList >     _aggs_;
      };

      /**
       * @class O3Assignment
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3Assignment is part of the AST of the O3PRM language.
       */
      class O3Assignment {
        public:
        O3Assignment();
        O3Assignment(const O3Assignment& src);
        O3Assignment(O3Assignment&& src);
        ~O3Assignment();

        O3Assignment& operator=(const O3Assignment& src);
        O3Assignment& operator=(O3Assignment&& src);

        O3Label&       leftInstance();
        const O3Label& leftInstance() const;

        O3Integer&       leftIndex();
        const O3Integer& leftIndex() const;

        O3Label&       leftReference();
        const O3Label& leftReference() const;

        O3Label&       rightInstance();
        const O3Label& rightInstance() const;

        O3Integer&       rightIndex();
        const O3Integer& rightIndex() const;

        private:
        O3Label   _leftInstance_;
        O3Integer _leftIndex_;
        O3Label   _leftReference_;
        O3Label   _rightInstance_;
        O3Integer _rightIndex_;
      };

      /**
       * @class O3Increment
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3Increment is part of the AST of the O3PRM language.
       */
      class O3Increment {
        public:
        O3Increment();
        O3Increment(const O3Increment& src);
        O3Increment(O3Increment&& src);
        ~O3Increment();

        O3Increment& operator=(const O3Increment& src);
        O3Increment& operator=(O3Increment&& src);

        O3Label&       leftInstance();
        const O3Label& leftInstance() const;

        O3Integer&       leftIndex();
        const O3Integer& leftIndex() const;

        O3Label&       leftReference();
        const O3Label& leftReference() const;

        O3Label&       rightInstance();
        const O3Label& rightInstance() const;

        O3Integer&       rightIndex();
        const O3Integer& rightIndex() const;

        private:
        O3Label   _leftInstance_;
        O3Integer _leftIndex_;
        O3Label   _leftReference_;
        O3Label   _rightInstance_;
        O3Integer _rightIndex_;
      };

      /**
       * @class O3InstanceParameter
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3InstanceParameter is part of the AST of the O3PRM
       * language.
       */
      class O3InstanceParameter {
        public:
        O3InstanceParameter();
        O3InstanceParameter(const O3InstanceParameter& src);
        O3InstanceParameter(O3InstanceParameter&& src);
        ~O3InstanceParameter();

        O3InstanceParameter& operator=(const O3InstanceParameter& src);
        O3InstanceParameter& operator=(O3InstanceParameter&& src);

        O3Label&       name();
        const O3Label& name() const;

        O3Float&       value();
        const O3Float& value() const;

        bool& isInteger();
        bool  isInteger() const;

        private:
        O3Label _name_;
        O3Float _value_;
        bool    _isInteger_;
      };

      /**
       * @class O3Instance
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3Instance is part of the AST of the O3PRM language.
       */
      class O3Instance {
        public:
        using O3InstanceParameterList = std::vector< O3InstanceParameter >;

        O3Instance();
        O3Instance(const O3Instance& src);
        O3Instance(O3Instance&& src);
        ~O3Instance();

        O3Instance& operator=(const O3Instance& src);
        O3Instance& operator=(O3Instance&& src);

        O3Label&       type();
        const O3Label& type() const;

        O3Label&       name();
        const O3Label& name() const;

        O3Integer&       size();
        const O3Integer& size() const;

        O3InstanceParameterList&       parameters();
        const O3InstanceParameterList& parameters() const;

        private:
        O3Label                 _type_;
        O3Label                 _name_;
        O3Integer               _size_;
        O3InstanceParameterList _parameters_;
      };

      /**
       * @class O3System
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3System is part of the AST of the O3PRM language.
       */
      class O3System {
        public:
        using O3InstanceList   = std::vector< O3Instance >;
        using O3AssignmentList = std::vector< O3Assignment >;
        using O3IncrementList  = std::vector< O3Increment >;

        O3System();
        O3System(const O3System& src);
        O3System(O3System&& src);
        ~O3System();

        O3System& operator=(const O3System& src);
        O3System& operator=(O3System&& src);

        O3Label&       name();
        const O3Label& name() const;

        O3InstanceList&       instances();
        const O3InstanceList& instances() const;

        O3AssignmentList&       assignments();
        const O3AssignmentList& assignments() const;

        O3IncrementList&       increments();
        const O3IncrementList& increments() const;

        private:
        O3Label          _name_;
        O3InstanceList   _instances_;
        O3AssignmentList _assigments_;
        O3IncrementList  _increments_;
      };

      /**
       * @class O3Import
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3Import is part of the AST of the O3PRM language.
       */
      class O3Import {
        public:
        O3Import();
        O3Import(const O3Import& src);
        O3Import(O3Import&& src);
        ~O3Import();

        O3Import& operator=(const O3Import& src);
        O3Import& operator=(O3Import&& src);

        O3Label&       import();
        const O3Label& import() const;

        private:
        O3Label _import_;
      };

      /**
       * @class O3PRM
       * @headerfile O3prm.h <agrum/PRM/o3prm/O3prm.h>
       * @ingroup o3prm_group
       *
       * @brief The O3PRM is part of the AST of the O3PRM language.
       */
      class O3PRM {
        public:
        using O3TypeList      = std::vector< std::unique_ptr< O3Type > >;
        using O3IntTypeList   = std::vector< std::unique_ptr< O3IntType > >;
        using O3RealTypeList  = std::vector< std::unique_ptr< O3RealType > >;
        using O3InterfaceList = std::vector< std::unique_ptr< O3Interface > >;
        using O3ClassList     = std::vector< std::unique_ptr< O3Class > >;
        using O3SystemList    = std::vector< std::unique_ptr< O3System > >;
        using O3ImportList    = std::vector< std::unique_ptr< O3Import > >;

        O3PRM();
        O3PRM(const O3PRM& src);
        O3PRM(O3PRM&& src);
        ~O3PRM();

        O3PRM& operator=(const O3PRM& src);
        O3PRM& operator=(O3PRM&& src);

        O3TypeList&       types();
        const O3TypeList& types() const;

        O3IntTypeList&       int_types();
        const O3IntTypeList& int_types() const;

        O3RealTypeList&       real_types();
        const O3RealTypeList& real_types() const;

        O3InterfaceList&       interfaces();
        const O3InterfaceList& interfaces() const;

        O3ClassList&       classes();
        const O3ClassList& classes() const;

        O3SystemList&       systems();
        const O3SystemList& systems() const;

        O3ImportList&       imports();
        const O3ImportList& imports() const;

        private:
        O3TypeList      _types_;
        O3IntTypeList   _int_types_;
        O3RealTypeList  _real_types_;
        O3InterfaceList _interfaces_;
        O3ClassList     _classes_;
        O3SystemList    _systems_;
        O3ImportList    _imports_;
      };

    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum

#endif   // GUM_PRM_O3PRM_PRM_H
