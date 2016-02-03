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
 * @brief Implementation for parsing function for the O3PRM language.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <agrum/PRM/newo3prm/o3prm.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      Position::Position()
          : __file( "" )
          , __line( 0 )
          , __column( 0 ) {
        GUM_CONSTRUCTOR( Position );
      }

      Position::Position( const std::string& file, int line, int column )
          : __file( file )
          , __line( line )
          , __column( column ) {
        GUM_CONSTRUCTOR( Position );
      }

      Position::Position( const Position& src )
          : __file( src.__file )
          , __line( src.__line )
          , __column( src.__column ) {
        GUM_CONS_CPY( Position );
      }

      Position::Position( Position&& src )
          : __file( std::move( src.__file ) )
          , __line( std::move( src.__line ) )
          , __column( std::move( src.__column ) ) {
        GUM_CONS_MOV( Position );
      }

      Position::~Position() { GUM_DESTRUCTOR( Position ); }

      Position& Position::operator=( const Position& src ) {
        if ( this == &src ) {
          return *this;
        }
        __file = src.__file;
        __line = src.__line;
        __column = src.__column;
        return *this;
      }

      Position& Position::operator=( Position&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __file = std::move( src.__file );
        __line = std::move( src.__line );
        __column = std::move( src.__column );
        return *this;
      }

      const std::string& Position::file() const { return __file; }
      void Position::file( const std::string& file ) { __file = file; }

      int Position::line() const { return __line; }
      void Position::line( int line ) { __line = line; }

      int Position::column() const { return __column; }
      void Position::column( int column ) { __column = column; }

      O3Integer::O3Integer() { GUM_CONSTRUCTOR( O3Integer ); }

      O3Integer::O3Integer( const Position& pos, int value )
          : __pos( pos )
          , __value( value ) {
        GUM_CONSTRUCTOR( O3Integer );
      }

      O3Integer::O3Integer( const O3Integer& src )
          : __pos( src.__pos )
          , __value( src.__value ) {
        GUM_CONS_CPY( O3Integer );
      }

      O3Integer::O3Integer( O3Integer&& src )
          : __pos( std::move( src.__pos ) )
          , __value( std::move( src.__value ) ) {
        GUM_CONS_MOV( O3Integer );
      }

      O3Integer::~O3Integer() { GUM_DESTRUCTOR( O3Integer ); }

      O3Integer& O3Integer::operator=( const O3Integer& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = src.__pos;
        __value = src.__value;
        return *this;
      }

      O3Integer& O3Integer::operator=( O3Integer&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = std::move( src.__pos );
        __value = std::move( src.__value );
        return *this;
      }

      const Position& O3Integer::position() const { return __pos; }

      void O3Integer::position( const Position& pos ) { __pos = pos; }

      int O3Integer::value() const { return __value; }
      void O3Integer::value( int i ) { __value = i; }

      O3Float::O3Float() { GUM_CONSTRUCTOR( O3Float ); }

      O3Float::O3Float( const Position& pos, float value )
          : __pos( pos )
          , __value( value ) {
        GUM_CONSTRUCTOR( O3Float );
      }

      O3Float::O3Float( const O3Float& src )
          : __pos( src.__pos )
          , __value( src.__value ) {
        GUM_CONS_CPY( O3Float );
      }

      O3Float::O3Float( O3Float&& src )
          : __pos( std::move( src.__pos ) )
          , __value( std::move( src.__value ) ) {
        GUM_CONS_MOV( O3Float );
      }

      O3Float::~O3Float() { GUM_DESTRUCTOR( O3Float ); }

      O3Float& O3Float::operator=( const O3Float& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = src.__pos;
        __value = src.__value;
        return *this;
      }

      O3Float& O3Float::operator=( O3Float&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = std::move( src.__pos );
        __value = std::move( src.__value );
        return *this;
      }

      const Position& O3Float::position() const { return __pos; }

      void O3Float::position( const Position& pos ) { __pos = pos; }

      float O3Float::value() const { return __value; }
      void O3Float::value( float f ) { __value = f; }

      O3Formula::O3Formula()
          : __pos()
          , __formula( std::unique_ptr<Formula>( new Formula( "" ) ) ) {
        GUM_CONSTRUCTOR( O3Formula );
      }

      O3Formula::O3Formula( const Position& pos, const Formula& formula )
          : __pos( pos )
          , __formula( std::unique_ptr<Formula>( new Formula( formula ) ) ) {
        GUM_CONSTRUCTOR( O3Formula );
      }

      O3Formula::O3Formula( const O3Formula& src )
          : __pos( src.__pos )
          , __formula(
                std::unique_ptr<Formula>( new Formula( src.formula() ) ) ) {
        GUM_CONS_CPY( O3Formula );
      }

      O3Formula::O3Formula( O3Formula&& src )
          : __pos( std::move( src.__pos ) )
          , __formula( std::move( src.__formula ) ) {
        GUM_CONS_MOV( O3Formula );
      }

      O3Formula::~O3Formula() { GUM_DESTRUCTOR( O3Formula ); }

      O3Formula& O3Formula::operator=( const O3Formula& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = src.__pos;
        __formula = std::unique_ptr<Formula>( new Formula( src.formula() ) );
        return *this;
      }

      O3Formula& O3Formula::operator=( O3Formula&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = std::move( src.__pos );
        __formula = std::move( src.__formula );
        return *this;
      }

      const Position& O3Formula::position() const { return __pos; }

      void O3Formula::position( const Position& pos ) { __pos = pos; }

      const Formula& O3Formula::formula() const { return *__formula; }
      Formula& O3Formula::formula() { return *__formula; }

      O3Label::O3Label()
          : __pos()
          , __label() {
        GUM_CONSTRUCTOR( O3Label );
      }

      O3Label::O3Label( const Position& pos, const std::string& label )
          : __pos( pos )
          , __label( label ) {
        GUM_CONSTRUCTOR( O3Label );
      }

      O3Label::O3Label( const O3Label& src )
          : __pos( src.__pos )
          , __label( src.__label ) {
        GUM_CONS_CPY( O3Label );
      }

      O3Label::O3Label( O3Label&& src )
          : __pos( std::move( src.__pos ) )
          , __label( std::move( src.__label ) ) {
        GUM_CONS_MOV( O3Label );
      }

      O3Label::~O3Label() { GUM_DESTRUCTOR( O3Label ); }

      O3Label& O3Label::operator=( const O3Label& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = src.__pos;
        __label = src.__label;
        return *this;
      }

      O3Label& O3Label::operator=( O3Label&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = std::move( src.__pos );
        __label = std::move( src.__label );
        return *this;
      }

      const Position& O3Label::position() const { return __pos; }

      void O3Label::position( const Position& pos ) { __pos = pos; }

      const std::string& O3Label::label() const { return __label; }

      void O3Label::label( const std::string& label ) { __label = label; }

      O3Type::O3Type( const Position& pos,
                      const O3Label& name,
                      const O3Label& super,
                      const LabelMap& labels )
          : __pos( pos )
          , __name( name )
          , __super( super )
          , __labels( labels ) {
        GUM_CONSTRUCTOR( O3Type );
      }

      O3Type::O3Type( const O3Type& src )
          : __pos( src.__pos )
          , __name( src.__name )
          , __super( src.__super )
          , __labels( src.__labels ) {
        GUM_CONS_CPY( O3Type );
      }

      O3Type::O3Type( O3Type&& src )
          : __pos( std::move( src.__pos ) )
          , __name( std::move( src.__name ) )
          , __super( std::move( src.__super ) )
          , __labels( std::move( src.__labels ) ) {
        GUM_CONS_MOV( O3Type );
      }

      O3Type::~O3Type() { GUM_DESTRUCTOR( O3Type ); }

      O3Type& O3Type::operator=( const O3Type& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = src.__pos;
        __name = src.__name;
        __super = src.__super;
        __labels = src.__labels;
        return *this;
      }

      O3Type& O3Type::operator=( O3Type&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = std::move( src.__pos );
        __name = std::move( src.__name );
        __super = std::move( src.__super );
        __labels = std::move( src.__labels );
        return *this;
      }

      const O3Label& O3Type::name() const { return __name; }
      const O3Label& O3Type::super() const { return __super; }
      const O3Type::LabelMap& O3Type::labels() const { return __labels; }
      const Position& O3Type::position() const { return __pos; }

      O3IntType::O3IntType() { GUM_CONSTRUCTOR( O3IntType ); }

      O3IntType::O3IntType( const Position& pos,
                            const O3Label& name,
                            const O3Integer& start,
                            const O3Integer& end )
          : __pos( pos )
          , __name( name )
          , __start( start )
          , __end( end ) {
        GUM_CONSTRUCTOR( O3IntType );
      }

      O3IntType::O3IntType( const O3IntType& src )
          : __pos( src.__pos )
          , __name( src.__name )
          , __start( src.__start )
          , __end( src.__end ) {
        GUM_CONS_CPY( O3IntType );
      }

      O3IntType::O3IntType( O3IntType&& src )
          : __pos( std::move( src.__pos ) )
          , __name( std::move( src.__name ) )
          , __start( std::move( src.__start ) )
          , __end( std::move( src.__end ) ) {
        GUM_CONS_MOV( O3IntType );
      }

      O3IntType::~O3IntType() { GUM_DESTRUCTOR( O3IntType ); }

      O3IntType& O3IntType::operator=( const O3IntType& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = src.__pos;
        __name = src.__name;
        __start = src.__start;
        __end = src.__end;
        return *this;
      }

      O3IntType& O3IntType::operator=( O3IntType&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = std::move( src.__pos );
        __name = std::move( src.__name );
        __start = std::move( src.__start );
        __end = std::move( src.__end );
        return *this;
      }

      const O3Label& O3IntType::name() const { return __name; }

      const O3Integer& O3IntType::start() const { return __start; }

      const O3Integer& O3IntType::end() const { return __end; }

      const Position& O3IntType::position() const { return __pos; }

      O3PRM::O3PRM() {
        GUM_CONSTRUCTOR( O3PRM );
        // Creating the boolean type
        auto name = O3Label( Position(), "boolean" );
        auto f = O3Label( Position(), "false" );
        auto t = O3Label( Position(), "true" );
        auto labels = O3Type::LabelMap();
        labels.push_back( O3Type::LabelPair( f, O3Label() ) );
        labels.push_back( O3Type::LabelPair( t, O3Label() ) );
        auto boolean = std::unique_ptr<O3Type>(
            new O3Type( Position(), name, O3Label(), labels ) );
        __types.push_back( std::move( boolean ) );
      }

      O3PRM::O3PRM( const O3PRM& src ) {
        GUM_CONS_CPY( O3PRM );
        for ( const auto& t : src.__types ) {
          __types.push_back( std::unique_ptr<O3Type>( new O3Type( *t ) ) );
        }
        for ( const auto& t : src.__int_types ) {
          __int_types.push_back(
              std::unique_ptr<O3IntType>( new O3IntType( *t ) ) );
        }
        for ( const auto& i : src.__interfaces ) {
          __interfaces.push_back(
              std::unique_ptr<O3Interface>( new O3Interface( *i ) ) );
        }
      }

      O3PRM::O3PRM( O3PRM&& src )
          : __types( std::move( src.__types ) )
          , __int_types( std::move( src.__int_types ) )
          , __interfaces( std::move( src.__interfaces ) ) {
        GUM_CONS_MOV( O3PRM );
      }

      O3PRM::~O3PRM() { GUM_DESTRUCTOR( O3PRM ); }

      O3PRM& O3PRM::operator=( const O3PRM& src ) {
        if ( this == &src ) {
          return *this;
        }
        for ( const auto& t : src.__types ) {
          __types.push_back( std::unique_ptr<O3Type>( new O3Type( *t ) ) );
        }
        for ( const auto& t : src.__int_types ) {
          __int_types.push_back(
              std::unique_ptr<O3IntType>( new O3IntType( *t ) ) );
        }
        for ( const auto& i : src.__interfaces ) {
          __interfaces.push_back(
              std::unique_ptr<O3Interface>( new O3Interface( *i ) ) );
        }
        return *this;
      }

      O3PRM& O3PRM::operator=( O3PRM&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __types = std::move( src.__types );
        __int_types = std::move( src.__int_types );
        __interfaces = std::move( src.__interfaces );
        return *this;
      }

      O3PRM::O3TypeList& O3PRM::types() { return __types; }
      const O3PRM::O3TypeList& O3PRM::types() const { return __types; }

      O3PRM::O3IntTypeList& O3PRM::int_types() { return __int_types; }
      const O3PRM::O3IntTypeList& O3PRM::int_types() const {
        return __int_types;
      }

      O3PRM::O3InterfaceList& O3PRM::interfaces() { return __interfaces; }
      const O3PRM::O3InterfaceList& O3PRM::interfaces() const {
        return __interfaces;
      }

      O3PRM::O3ClassList& O3PRM::classes() { return __classes; }

      const O3PRM::O3ClassList& O3PRM::classes() const { return __classes; }

      O3InterfaceElement::O3InterfaceElement( const O3Label& type,
                                              const O3Label& name )
          : __type( type )
          , __name( name ) {
        GUM_CONSTRUCTOR( O3InterfaceElement );
      }

      O3InterfaceElement::O3InterfaceElement( const O3InterfaceElement& src )
          : __type( src.__type )
          , __name( src.__name ) {
        GUM_CONS_CPY( O3InterfaceElement );
      }

      O3InterfaceElement::O3InterfaceElement( O3InterfaceElement&& src )
          : __type( std::move( src.__type ) )
          , __name( std::move( src.__name ) ) {
        GUM_CONS_MOV( O3InterfaceElement );
      }

      O3InterfaceElement::~O3InterfaceElement() {
        GUM_DESTRUCTOR( O3InterfaceElement );
      }

      O3InterfaceElement& O3InterfaceElement::
      operator=( const O3InterfaceElement& src ) {
        if ( this == &src ) {
          return *this;
        }
        __type = src.__type;
        __name = src.__name;
        return *this;
      }

      O3InterfaceElement& O3InterfaceElement::
      operator=( O3InterfaceElement&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __type = std::move( src.__type );
        __name = std::move( src.__name );
        return *this;
      }

      const O3Label& O3InterfaceElement::type() const { return __type; }
      const O3Label& O3InterfaceElement::name() const { return __name; }

      O3Interface::O3Interface( const Position& pos,
                                const O3Label& name,
                                const O3Label& super,
                                const O3InterfaceElementList& elts )
          : __pos( pos )
          , __name( name )
          , __super( super ) {
        GUM_CONSTRUCTOR( O3Interface );
        auto copy = new O3InterfaceElementList( elts );
        __elts = std::unique_ptr<O3InterfaceElementList>( copy );
      }

      O3Interface::O3Interface( const O3Interface& src )
          : __pos( src.__pos )
          , __name( src.__name )
          , __super( src.__super ) {
        GUM_CONS_CPY( O3Interface );
        auto copy = new O3InterfaceElementList( src.elements() );
        __elts = std::unique_ptr<O3InterfaceElementList>( copy );
      }

      O3Interface::O3Interface( O3Interface&& src )
          : __pos( std::move( src.__pos ) )
          , __name( std::move( src.__name ) )
          , __super( std::move( src.__super ) )
          , __elts( std::move( src.__elts ) ) {
        GUM_CONS_MOV( O3Interface );
      }

      O3Interface::~O3Interface() { GUM_DESTRUCTOR( O3Interface ); }

      O3Interface& O3Interface::operator=( const O3Interface& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = src.__pos;
        __name = src.__name;
        __super = src.__super;
        auto copy = new O3InterfaceElementList( src.elements() );
        __elts = std::unique_ptr<O3InterfaceElementList>( copy );
        return *this;
      }

      O3Interface& O3Interface::operator=( O3Interface&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = std::move( src.__pos );
        __name = std::move( src.__name );
        __super = std::move( src.__super );
        __elts = std::move( src.__elts );
        return *this;
      }

      const Position& O3Interface::position() const { return __pos; }

      const O3Label& O3Interface::name() const { return __name; }

      const O3Label& O3Interface::super() const { return __super; }

      const O3Interface::O3InterfaceElementList& O3Interface::elements() const {
        return *__elts;
      }

      O3Attribute::O3Attribute( const O3Label& type,
                                const O3Label& name,
                                const O3LabelList& parents )
          : __type( type )
          , __name( name )
          , __parents( parents ) {
        GUM_CONSTRUCTOR( O3Attribute );
      }

      O3Attribute::O3Attribute( const O3Attribute& src )
          : __type( src.__type )
          , __name( src.__name )
          , __parents( src.__parents ) {
        GUM_CONS_CPY( O3Attribute );
      }

      O3Attribute::O3Attribute( O3Attribute&& src )
          : __type( std::move( src.__type ) )
          , __name( std::move( src.__name ) )
          , __parents( std::move( src.__parents ) ) {
        GUM_CONS_MOV( O3Attribute );
      }

      O3Attribute::~O3Attribute() { GUM_DESTRUCTOR( O3Attribute ); }

      O3Attribute& O3Attribute::operator=( const O3Attribute& src ) {
        if ( this == &src ) {
          return *this;
        }
        __type = src.__type;
        __name = src.__name;
        __parents = src.__parents;
        return *this;
      }

      O3Attribute& O3Attribute::operator=( O3Attribute&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __type = std::move( src.__type );
        __name = std::move( src.__name );
        __parents = std::move( src.__parents );
        return *this;
      }

      const O3Label& O3Attribute::type() const { return __type; }
      const O3Label& O3Attribute::name() const { return __name; }
      const O3Attribute::O3LabelList& O3Attribute::parents() const {
        return __parents;
      }

      O3RawCPT::O3RawCPT( const O3Label& type,
                          const O3Label& name,
                          const O3LabelList& parents,
                          const O3FormulaList& values )
          : O3Attribute( type, name, parents ) {
        GUM_CONSTRUCTOR( O3RawCPT );
        auto copy = new O3FormulaList( values );
        __values = std::unique_ptr<O3FormulaList>( copy );
      }

      O3RawCPT::O3RawCPT( const O3RawCPT& src )
          : O3Attribute( src ) {
        GUM_CONS_CPY( O3RawCPT );
        auto copy = new O3FormulaList( *( src.__values ) );
        __values = std::unique_ptr<O3FormulaList>( copy );
      }

      O3RawCPT::O3RawCPT( O3RawCPT&& src )
          : O3Attribute( src )
          , __values( std::move( src.__values ) ) {
        GUM_CONS_MOV( O3RawCPT );
      }

      O3RawCPT::~O3RawCPT() { GUM_DESTRUCTOR( O3RawCPT ); }

      O3RawCPT& O3RawCPT::operator=( const O3RawCPT& src ) {
        if ( this == &src ) {
          return *this;
        }
        O3Attribute::operator=( src );
        auto copy = new O3FormulaList( *( src.__values ) );
        __values = std::unique_ptr<O3FormulaList>( copy );
        return *this;
      }

      O3RawCPT& O3RawCPT::operator=( O3RawCPT&& src ) {
        if ( this == &src ) {
          return *this;
        }
        O3Attribute::operator=( src );
        __values = std::move( src.__values );
        return *this;
      }

      O3RawCPT::O3FormulaList& O3RawCPT::values() {
        return *__values;
      }

      const O3RawCPT::O3FormulaList& O3RawCPT::values() const {
        return *__values;
      }

      std::unique_ptr<O3Attribute> O3RawCPT::copy() const {
        auto copy = new O3RawCPT( *this );
        return std::move( std::unique_ptr<O3Attribute>( copy ) );
      }

      O3RuleCPT::O3RuleCPT( const O3Label& type,
                            const O3Label& name,
                            const O3Attribute::O3LabelList& parents,
                            O3RuleList&& rules )
          : O3Attribute( type, name, parents )
          , __rules( std::unique_ptr<O3RuleList>( new O3RuleList( rules ) ) ) {
        GUM_CONSTRUCTOR( O3RuleCPT );
      }

      O3RuleCPT::O3RuleCPT( const O3RuleCPT::O3RuleCPT& src )
          : O3Attribute( src )
          , __rules(
                std::unique_ptr<O3RuleList>( new O3RuleList( src.rules() ) ) ) {
        GUM_CONS_CPY( O3RuleCPT );
      }

      O3RuleCPT::O3RuleCPT( O3RuleCPT::O3RuleCPT&& src )
          : O3Attribute( src )
          , __rules( std::move( src.__rules ) ) {
        GUM_CONS_MOV( O3RuleCPT );
      }

      O3RuleCPT::~O3RuleCPT() { GUM_DESTRUCTOR( O3RuleCPT ); }

      O3RuleCPT& O3RuleCPT::operator=( const O3RuleCPT::O3RuleCPT& src ) {
        if ( this == &src ) {
          return *this;
        }

        O3Attribute::operator=( src );
        __rules = std::unique_ptr<O3RuleList>( new O3RuleList( src.rules() ) );
        return *this;
      }

      O3RuleCPT& O3RuleCPT::operator=( O3RuleCPT::O3RuleCPT&& src ) {
        if ( this == &src ) {
          return *this;
        }

        O3Attribute::operator=( src );
        __rules = std::move( src.__rules );
        return *this;
      }

      O3RuleCPT::O3RuleList& O3RuleCPT::rules() { return *__rules; }
      const O3RuleCPT::O3RuleList& O3RuleCPT::rules() const { return *__rules; }

      std::unique_ptr<O3Attribute> O3RuleCPT::copy() const {
        auto copy = new O3RuleCPT( *this );
        return std::move( std::unique_ptr<O3Attribute>( copy ) );
      }

      O3Class::O3Class( const Position& pos,
                        const O3Label& name,
                        const O3Label& super,
                        const O3LabelList& interfaces,
                        const O3ParameterList& params,
                        const O3AttributeList& elts )
          : __pos( pos )
          , __name( name )
          , __super( super ) {
        GUM_CONSTRUCTOR( O3Class );
        auto i = new O3LabelList( interfaces );
        __interfaces = std::unique_ptr<O3LabelList>( i );
        auto p = new O3ParameterList( params );
        __params = std::unique_ptr<O3ParameterList>( p );
        auto e = new O3AttributeList();
        __elts = std::unique_ptr<O3AttributeList>( e );
        for ( const auto& elt : elts ) {
          __elts->push_back( elt->copy() );
        }
      }

      O3Class::O3Class( const O3Class& src )
          : __pos( src.__pos )
          , __name( src.__name )
          , __super( src.__super ) {
        GUM_CONS_CPY( O3Class );
        auto i = new O3LabelList( src.interfaces() );
        __interfaces = std::unique_ptr<O3LabelList>( i );
        auto p = new O3ParameterList( src.parameters() );
        __params = std::unique_ptr<O3ParameterList>( p );
        auto e = new O3AttributeList();
        __elts = std::unique_ptr<O3AttributeList>( e );
        for ( const auto& elt : src.elements() ) {
          __elts->push_back( elt->copy() );
        }
      }

      O3Class::O3Class( O3Class&& src )
          : __pos( std::move( src.__pos ) )
          , __name( std::move( src.__name ) )
          , __super( std::move( src.__super ) )
          , __interfaces( std::move( src.__interfaces ) )
          , __params( std::move( src.__params ) )
          , __elts( std::move( src.__elts ) ) {
        GUM_CONS_MOV( O3Class );
      }

      O3Class::~O3Class() { GUM_DESTRUCTOR( O3Class ); }

      O3Class& O3Class::operator=( const O3Class& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = src.__pos;
        __name = src.__name;
        __super = src.__super;
        auto i = new O3LabelList( src.interfaces() );
        __interfaces = std::unique_ptr<O3LabelList>( i );
        auto p = new O3ParameterList( src.parameters() );
        __params = std::unique_ptr<O3ParameterList>( p );
        auto e = new O3AttributeList();
        __elts = std::unique_ptr<O3AttributeList>( e );
        for ( const auto& elt : src.elements() ) {
          __elts->push_back( elt->copy() );
        }
        return *this;
      }

      O3Class& O3Class::operator=( O3Class&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = std::move( src.__pos );
        __name = std::move( src.__name );
        __super = std::move( src.__super );
        __interfaces = std::move( src.__interfaces );
        __params = std::move( src.__params );
        __elts = std::move( src.__elts );
        return *this;
      }

      const Position& O3Class::position() const { return __pos; }

      const O3Label& O3Class::name() const { return __name; }

      const O3Label& O3Class::super() const { return __super; }

      const O3Class::O3LabelList& O3Class::interfaces() const {
        return *__interfaces;
      }

      const O3Class::O3ParameterList& O3Class::parameters() const {
        return *__params;
      }

      O3Class::O3AttributeList& O3Class::elements() {
        return *__elts;
      }
      const O3Class::O3AttributeList& O3Class::elements() const {
        return *__elts;
      }

      O3Parameter::O3Parameter( const Position& pos,
                                const O3Label& name,
                                const O3Integer& value )
          : __type( O3Parameter::Type::INT )
          , __pos( pos )
          , __name( name )
          , __value( O3Float( value.position(), (float)value.value() ) ) {
        GUM_CONSTRUCTOR( O3Parameter );
      }

      O3Parameter::O3Parameter( const Position& pos,
                                const O3Label& name,
                                const O3Float& value )
          : __type( O3Parameter::Type::FLOAT )
          , __pos( pos )
          , __name( name )
          , __value( value ) {
        GUM_CONSTRUCTOR( O3Parameter );
      }

      O3Parameter::O3Parameter( const O3Parameter& src )
          : __type( src.__type )
          , __pos( src.__pos )
          , __name( src.__name )
          , __value( src.__value ) {
        GUM_CONS_CPY( O3Parameter );
      }

      O3Parameter::O3Parameter( O3Parameter&& src )
          : __type( std::move( src.__type ) )
          , __pos( std::move( src.__pos ) )
          , __name( std::move( src.__name ) )
          , __value( std::move( src.__value ) ) {
        GUM_CONS_MOV( O3Parameter );
      }

      O3Parameter::~O3Parameter() {
        GUM_DESTRUCTOR( O3Parameter );
      }

      O3Parameter& O3Parameter::operator=( const O3Parameter& src ) {
        if (this == &src) {
          return *this;
        }
        __type = src.__type;
        __pos = src.__pos;
        __name = src.__name;
        __value = src.__value;
        return *this;
      }

      O3Parameter& O3Parameter::operator=( O3Parameter&& src ) {
        if( this==&src) {
          return *this;
        }
        __type = std::move( src.__type );
        __pos = std::move(src.__pos);
        __name = std::move(src.__name);
        __value = std::move(src.__value);
        return *this;
      }

      O3Parameter::Type O3Parameter::type() const {
        return __type;
      }

      const Position& O3Parameter::position() const {
        return __pos;
      }

      const O3Label& O3Parameter::name() const {
        return __name;
      }

      const O3Float& O3Parameter::value() const {
        return __value;
      }

    }  // o3prm
  }    // prm
}  // gum
