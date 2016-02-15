/**************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
*   {prenom.nom}_at_lip6.fr                                               *
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

/**
 * @file
 * @brief Headers for parsing function for the O3PRM language.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <string>
#include <vector>
#include <utility>

#include <agrum/config.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/math/formula.h>

#ifndef GUM_PRM_O3PRM_PRM_H
#define GUM_PRM_O3PRM_PRM_H

namespace gum {
  namespace prm {
    namespace o3prm {

      class Position {
        public:
        Position();
        Position( const std::string& filename, int line, int column );
        Position( const Position& src );
        Position( Position&& src );
        ~Position();
        Position& operator=( const Position& src );
        Position& operator=( Position&& src );
        const std::string& file() const;
        void file( const std::string& file );
        int line() const;
        void line( int line );
        int column() const;
        void column( int column );

        private:
        std::string __file;
        int __line;
        int __column;
      };

      class O3Formula {
        public:
        O3Formula();
        O3Formula( const Position& pos, const Formula& formula );
        O3Formula( const O3Formula& src );
        O3Formula( O3Formula&& src );
        ~O3Formula();
        O3Formula& operator=( const O3Formula& src );
        O3Formula& operator=( O3Formula&& src );

        const Position& position() const;
        void position( const Position& pos );

        const Formula& formula() const;
        Formula& formula();

        private:
        Position __pos;
        std::unique_ptr<Formula> __formula;
      };

      class O3Float {
        public:
        O3Float();
        O3Float( const Position& pos, float value );
        O3Float( const O3Float& src );
        O3Float( O3Float&& src );
        ~O3Float();
        O3Float& operator=( const O3Float& src );
        O3Float& operator=( O3Float&& src );

        const Position& position() const;
        void position( const Position& pos );

        float value() const;
        void value( float f );

        private:
        Position __pos;
        float __value;
      };

      class O3Integer {
        public:
        O3Integer();
        O3Integer( const Position& pos, int value );
        O3Integer( const O3Integer& src );
        O3Integer( O3Integer&& src );
        ~O3Integer();
        O3Integer& operator=( const O3Integer& src );
        O3Integer& operator=( O3Integer&& src );

        const Position& position() const;
        void position( const Position& pos );

        int value() const;
        void value( int i );

        private:
        Position __pos;
        int __value;
      };

      class O3Label {
        public:
        O3Label();
        O3Label( const Position& pos, const std::string& label );
        O3Label( const O3Label& src );
        O3Label( O3Label&& src );
        ~O3Label();
        O3Label& operator=( const O3Label& src );
        O3Label& operator=( O3Label&& src );

        const Position& position() const;
        void position( const Position& pos );

        const std::string& label() const;
        void label( const std::string& label );

        private:
        Position __pos;
        std::string __label;
      };

      std::ostream& operator<<( std::ostream& o, const O3Label& src ); 

      class O3Type {
        public:
        using LabelPair = std::pair<O3Label, O3Label>;
        using LabelMap = std::vector<LabelPair>;
        O3Type();
        O3Type( const Position& pos,
                const O3Label& name,
                const O3Label& super,
                const LabelMap& labels );
        O3Type( const O3Type& src );
        O3Type( O3Type&& src );
        ~O3Type();
        O3Type& operator=( const O3Type& src );
        O3Type& operator=( O3Type&& src );

        const O3Label& name() const;
        const O3Label& super() const;
        const LabelMap& labels() const;
        const Position& position() const;

        private:
        Position __pos;
        O3Label __name;
        O3Label __super;
        LabelMap __labels;
      };

      class O3IntType {
        public:
        O3IntType();
        O3IntType( const Position& pos,
                   const O3Label& name,
                   const O3Integer& start,
                   const O3Integer& end );
        O3IntType( const O3IntType& src );
        O3IntType( O3IntType&& src );
        ~O3IntType();
        O3IntType& operator=( const O3IntType& src );
        O3IntType& operator=( O3IntType&& src );

        const O3Label& name() const;
        const O3Integer& start() const;
        const O3Integer& end() const;
        const Position& position() const;

        private:
        Position __pos;
        O3Label __name;
        O3Integer __start;
        O3Integer __end;
      };

      class O3InterfaceElement {
        public:
        O3InterfaceElement( const O3Label& type, const O3Label& name );
        O3InterfaceElement( const O3InterfaceElement& src );
        O3InterfaceElement( O3InterfaceElement&& src );
        ~O3InterfaceElement();
        O3InterfaceElement& operator=( const O3InterfaceElement& src );
        O3InterfaceElement& operator=( O3InterfaceElement&& src );

        const O3Label& type() const;
        const O3Label& name() const;

        private:
        O3Label __type;
        O3Label __name;
      };

      class O3Interface {
        public:
        using O3InterfaceElementList = std::vector<O3InterfaceElement>;

        O3Interface( const Position& pos,
                     const O3Label& name,
                     const O3Label& super,
                     const O3InterfaceElementList& elts );
        O3Interface( const O3Interface& src );
        O3Interface( O3Interface&& src );
        ~O3Interface();
        O3Interface& operator=( const O3Interface& src );
        O3Interface& operator=( O3Interface&& src );

        const Position& position() const;
        const O3Label& name() const;
        const O3Label& super() const;
        const O3InterfaceElementList& elements() const;

        private:
        Position __pos;
        O3Label __name;
        O3Label __super;
        std::unique_ptr<O3InterfaceElementList> __elts;
      };

      class O3Parameter {
        public:
        enum class Type { INT, FLOAT };

        O3Parameter( const Position& pos,
                     const O3Label& name,
                     const O3Integer& value );
        O3Parameter( const Position& pos,
                     const O3Label& name,
                     const O3Float& value );
        O3Parameter( const O3Parameter& src );
        O3Parameter( O3Parameter&& src );
        ~O3Parameter();
        O3Parameter& operator=( const O3Parameter& src );
        O3Parameter& operator=( O3Parameter&& src );

        Type type() const;
        const Position& position() const;
        const O3Label& name() const;
        const O3Float& value() const;

        private:
        Type __type;
        Position __pos;
        O3Label __name;
        O3Float __value;
      };

      class O3ReferenceSlot {
        public:
        O3ReferenceSlot( const O3Label& type,
                         const O3Label& name,
                         bool isArray );
        O3ReferenceSlot( const O3ReferenceSlot& src );
        O3ReferenceSlot( O3ReferenceSlot&& src );
        ~O3ReferenceSlot();
        O3ReferenceSlot& operator=( const O3ReferenceSlot& src );
        O3ReferenceSlot& operator=( O3ReferenceSlot&& src );

        const O3Label& type() const;
        const O3Label& name() const;
        bool isArray() const;

        private:
        O3Label __type;
        O3Label __name;
        bool __isArray;
      };

      class O3Attribute {
        public:
        using O3LabelList = std::vector<O3Label>;
        O3Attribute( const O3Label& type,
                     const O3Label& name,
                     const O3LabelList& parents );
        O3Attribute( const O3Attribute& src );
        O3Attribute( O3Attribute&& src );
        virtual ~O3Attribute();
        O3Attribute& operator=( const O3Attribute& src );
        O3Attribute& operator=( O3Attribute&& src );

        const O3Label& type() const;
        const O3Label& name() const;
        const O3LabelList& parents() const;

        virtual std::unique_ptr<O3Attribute> copy() const = 0;

        private:
        O3Label __type;
        O3Label __name;
        O3LabelList __parents;
      };

      class O3RawCPT : public O3Attribute {
        public:
        using O3FormulaList = std::vector<O3Formula>;
        O3RawCPT( const O3Label& type,
                  const O3Label& name,
                  const O3Attribute::O3LabelList& parents,
                  const O3FormulaList& values );
        O3RawCPT( const O3RawCPT& src );
        O3RawCPT( O3RawCPT&& src );
        ~O3RawCPT();
        O3RawCPT& operator=( const O3RawCPT& src );
        O3RawCPT& operator=( O3RawCPT&& src );

        O3FormulaList& values();
        const O3FormulaList& values() const;

        virtual std::unique_ptr<O3Attribute> copy() const;

        private:
        std::unique_ptr<O3FormulaList> __values;
      };

      class O3RuleCPT : public O3Attribute {
        public:
        using O3LabelList = std::vector<O3Label>;
        using O3FormulaList = std::vector<O3Formula>;
        using O3Rule = std::pair<O3LabelList, O3FormulaList>;
        using O3RuleList = std::vector<O3Rule>;
        O3RuleCPT( const O3Label& type,
                   const O3Label& name,
                   const O3Attribute::O3LabelList& parents,
                   O3RuleList&& rules );
        O3RuleCPT( const O3RuleCPT& src );
        O3RuleCPT( O3RuleCPT&& src );
        ~O3RuleCPT();
        O3RuleCPT& operator=( const O3RuleCPT& src );
        O3RuleCPT& operator=( O3RuleCPT&& src );

        O3RuleList& rules();
        const O3RuleList& rules() const;

        virtual std::unique_ptr<O3Attribute> copy() const;

        private:
        std::unique_ptr<O3RuleList> __rules;
      };

      class O3Aggregate {
        public:
        using O3LabelList = std::vector<O3Label>;
        O3Aggregate();
        O3Aggregate( const O3Aggregate& src );
        O3Aggregate( O3Aggregate&& src );
        virtual ~O3Aggregate();
        O3Aggregate& operator=( const O3Aggregate& src );
        O3Aggregate& operator=( O3Aggregate&& src );

        O3Label& variableType();
        const O3Label& variableType() const;
        O3Label& aggregateType();
        const O3Label& aggregateType() const;
        O3Label& name();
        const O3Label& name() const;
        O3LabelList& parents();
        const O3LabelList& parents() const;
        O3LabelList& parameters();
        const O3LabelList& parameters() const;

        private:
        O3Label __variableType;
        O3Label __aggregateType;
        O3Label __name;
        O3LabelList __parents;
        O3LabelList __parameters;
      };

      class O3Class {
        public:
        using O3LabelList = std::vector<O3Label>;
        using O3ParameterList = std::vector<O3Parameter>;
        using O3ReferenceSlotList = std::vector<O3ReferenceSlot>;
        using O3AttributeList = std::vector<std::unique_ptr<O3Attribute>>;
        using O3AggregateList = std::vector<O3Aggregate>;

        O3Class();
        O3Class( const O3Class& src );
        O3Class( O3Class&& src );
        ~O3Class();
        O3Class& operator=( const O3Class& src );
        O3Class& operator=( O3Class&& src );

        const Position& position() const;
        Position& position();

        const O3Label& name() const;
        O3Label& name();

        const O3Label& super() const;
        O3Label& super();

        const O3LabelList& interfaces() const;
        O3LabelList& interfaces();

        const O3ParameterList& parameters() const;
        O3ParameterList& parameters();

        const O3ReferenceSlotList& referenceSlots() const;
        O3ReferenceSlotList& referenceSlots();

        const O3AttributeList& attributes() const;
        O3AttributeList& attributes();

        const O3AggregateList& aggregates() const;
        O3AggregateList& aggregates();

        private:
        Position __pos;
        O3Label __name;
        O3Label __super;
        std::unique_ptr<O3LabelList> __interfaces;
        std::unique_ptr<O3ParameterList> __params;
        std::unique_ptr<O3ReferenceSlotList> __refs;
        std::unique_ptr<O3AttributeList> __attrs;
        std::unique_ptr<O3AggregateList> __aggs;
      };

      class O3Assignment {
        public:

        O3Assignment();
        O3Assignment( const O3Assignment& src );
        O3Assignment( O3Assignment&& src );
        ~O3Assignment();
        O3Assignment& operator=( const O3Assignment& src );
        O3Assignment& operator=( O3Assignment&& src );

        const O3Label& leftInstance() const;
        O3Label& leftInstance();

        const O3Label& leftReference() const;
        O3Label& leftReference();

        const O3Label& rightInstance() const;
        O3Label& rightInstance();

        private:
        O3Label __leftInstance;
        O3Label __leftReference;
        O3Label __rightInstance;
      };

      class O3Increment {
        public:

        O3Increment();
        O3Increment( const O3Increment& src );
        O3Increment( O3Increment&& src );
        ~O3Increment();
        O3Increment& operator=( const O3Increment& src );
        O3Increment& operator=( O3Increment&& src );

        const O3Label& leftInstance() const;
        O3Label& leftInstance();

        const O3Label& leftReference() const;
        O3Label& leftReference();

        const O3Label& rightInstance() const;
        O3Label& rightInstance();

        private:
        O3Label __leftInstance;
        O3Label __leftReference;
        O3Label __rightInstance;
      };

      class O3InstanceParameter {
        public:
        O3InstanceParameter();
        O3InstanceParameter( const O3InstanceParameter& src );
        O3InstanceParameter( O3InstanceParameter&& src );
        ~O3InstanceParameter();
        O3InstanceParameter& operator=( const O3InstanceParameter& src );
        O3InstanceParameter& operator=( O3InstanceParameter&& src );

        const O3Label& name() const;
        O3Label& name();

        const O3Float& value() const;
        O3Float& value();

        bool isInteger() const;
        bool& isInteger();

        private:
        O3Label __name;
        O3Float __value;
        bool __isInteger;
      };

      class O3Instance {
        public:
        using O3InstanceParameterList = std::vector<O3InstanceParameter>;

        O3Instance();
        O3Instance( const O3Instance& src );
        O3Instance( O3Instance&& src );
        ~O3Instance();
        O3Instance& operator=( const O3Instance& src );
        O3Instance& operator=( O3Instance&& src );

        const O3Label& type() const;
        O3Label& type();

        const O3Label& name() const;
        O3Label& name();

        const O3Integer& size() const;
        O3Integer& size();

        const O3InstanceParameterList& parameters() const;
        O3InstanceParameterList& parameters();

        private:
        O3Label __type;
        O3Label __name;
        O3Integer __size;
        O3InstanceParameterList __parameters;
      };

      class O3System {
        public:
        using O3InstanceList = std::vector<O3Instance>;
        using O3AssignmentList = std::vector<O3Assignment>;
        using O3IncrementList = std::vector<O3Increment>;

        O3System();
        O3System( const O3System& src );
        O3System( O3System&& src );
        ~O3System();
        O3System& operator=( const O3System& src );
        O3System& operator=( O3System&& src );

        const O3Label& name() const;
        O3Label& name();

        const O3InstanceList& instances() const;
        O3InstanceList& instances();

        const O3AssignmentList& assignments() const;
        O3AssignmentList& assignments();

        const O3IncrementList& increments() const;
        O3IncrementList& increments();

        private:
        O3Label __name;
        O3InstanceList __instances;
        O3AssignmentList __assigments;
        O3IncrementList __increments;
      };

      class O3Import {
        public:
        O3Import();
        O3Import(const O3Import& src);
        O3Import(O3Import&& src);
        ~O3Import();
        O3Import& operator=(const O3Import& src);
        O3Import& operator=(O3Import&& src);

        const O3Label& import() const;
        O3Label& import();

        private:
        O3Label __import;

      };

      class O3PRM {
        public:
        using O3TypeList = std::vector<std::unique_ptr<O3Type>>;
        using O3IntTypeList = std::vector<std::unique_ptr<O3IntType>>;
        using O3InterfaceList = std::vector<std::unique_ptr<O3Interface>>;
        using O3ClassList = std::vector<std::unique_ptr<O3Class>>;
        using O3SystemList = std::vector<std::unique_ptr<O3System>>;
        using O3ImportList = std::vector<std::unique_ptr<O3Import>>;

        O3PRM();
        O3PRM( const O3PRM& src );
        O3PRM( O3PRM&& src );
        ~O3PRM();
        O3PRM& operator=( const O3PRM& src );
        O3PRM& operator=( O3PRM&& src );

        O3TypeList& types();
        const O3TypeList& types() const;

        O3IntTypeList& int_types();
        const O3IntTypeList& int_types() const;

        O3InterfaceList& interfaces();
        const O3InterfaceList& interfaces() const;

        O3ClassList& classes();
        const O3ClassList& classes() const;

        O3SystemList& systems();
        const O3SystemList& systems() const;

        O3ImportList& imports();
        const O3ImportList& imports() const;

        private:
        O3TypeList __types;
        O3IntTypeList __int_types;
        O3InterfaceList __interfaces;
        O3ClassList __classes;
        O3SystemList __systems;
        O3ImportList __imports;
      };

    }  // o3prm
  }    // prm
}  // gum

#endif  // GUM_PRM_O3PRM_PRM_H

