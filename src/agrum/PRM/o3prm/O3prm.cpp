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
 * @brief Implementation for the AST of the O3PRM language.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <agrum/PRM/o3prm/O3prm.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      O3Position::O3Position()
          : __file( "" )
          , __line( 0 )
          , __column( 0 ) {
        GUM_CONSTRUCTOR( O3Position );
      }

      O3Position::O3Position( const std::string& file, int line, int column )
          : __file( file )
          , __line( line )
          , __column( column ) {
        GUM_CONSTRUCTOR( O3Position );
      }

      O3Position::O3Position( const O3Position& src )
          : __file( src.__file )
          , __line( src.__line )
          , __column( src.__column ) {
        GUM_CONS_CPY( O3Position );
      }

      O3Position::O3Position( O3Position&& src )
          : __file( std::move( src.__file ) )
          , __line( std::move( src.__line ) )
          , __column( std::move( src.__column ) ) {
        GUM_CONS_MOV( O3Position );
      }

      O3Position::~O3Position() { GUM_DESTRUCTOR( O3Position ); }

      O3Position& O3Position::operator=( const O3Position& src ) {
        if ( this == &src ) {
          return *this;
        }
        __file = src.__file;
        __line = src.__line;
        __column = src.__column;
        return *this;
      }

      O3Position& O3Position::operator=( O3Position&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __file = std::move( src.__file );
        __line = std::move( src.__line );
        __column = std::move( src.__column );
        return *this;
      }

      std::string&       O3Position::file() { return __file; }
      const std::string& O3Position::file() const { return __file; }

      int& O3Position::line() { return __line; }
      int  O3Position::line() const { return __line; }

      int& O3Position::column() { return __column; }
      int  O3Position::column() const { return __column; }

      O3Integer::O3Integer()
          : __pos()
          , __value( 0 ) {
        GUM_CONSTRUCTOR( O3Integer );
      }

      O3Integer::O3Integer( const O3Position& pos, int value )
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

      const O3Position& O3Integer::position() const { return __pos; }
      O3Position&       O3Integer::position() { return __pos; }

      int  O3Integer::value() const { return __value; }
      int& O3Integer::value() { return __value; }

      O3Float::O3Float() { GUM_CONSTRUCTOR( O3Float ); }

      O3Float::O3Float( const O3Position& pos, float value )
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

      const O3Position& O3Float::position() const { return __pos; }
      O3Position&       O3Float::position() { return __pos; }

      float  O3Float::value() const { return __value; }
      float& O3Float::value() { return __value; }

      O3Formula::O3Formula()
          : __pos()
          , __formula( std::unique_ptr<Formula>( new Formula( "" ) ) ) {
        GUM_CONSTRUCTOR( O3Formula );
      }

      O3Formula::O3Formula( const O3Position& pos, const Formula& formula )
          : __pos( pos )
          , __formula( std::unique_ptr<Formula>( new Formula( formula ) ) ) {
        GUM_CONSTRUCTOR( O3Formula );
      }

      O3Formula::O3Formula( const O3Formula& src )
          : __pos( src.__pos )
          , __formula( std::unique_ptr<Formula>( new Formula( src.formula() ) ) ) {
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

      const O3Position& O3Formula::position() const { return __pos; }
      O3Position&       O3Formula::position() { return __pos; }

      const Formula& O3Formula::formula() const { return *__formula; }
      Formula&       O3Formula::formula() { return *__formula; }

      O3Label::O3Label()
          : __pos()
          , __label() {
        GUM_CONSTRUCTOR( O3Label );
      }

      O3Label::O3Label( const O3Position& pos, const std::string& label )
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

      const O3Position& O3Label::position() const { return __pos; }
      O3Position&       O3Label::position() { return __pos; }

      const std::string& O3Label::label() const { return __label; }
      std::string&       O3Label::label() { return __label; }

      O3Type::O3Type() { GUM_CONSTRUCTOR( O3Type ); }

      O3Type::O3Type( const O3Type& src )
          : __pos( src.__pos )
          , __name( src.__name )
          , __superLabel( src.__superLabel )
          , __labels( src.__labels ) {
        GUM_CONS_CPY( O3Type );
      }

      O3Type::O3Type( O3Type&& src )
          : __pos( std::move( src.__pos ) )
          , __name( std::move( src.__name ) )
          , __superLabel( std::move( src.__superLabel ) )
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
        __superLabel = src.__superLabel;
        __labels = src.__labels;
        return *this;
      }

      O3Type& O3Type::operator=( O3Type&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = std::move( src.__pos );
        __name = std::move( src.__name );
        __superLabel = std::move( src.__superLabel );
        __labels = std::move( src.__labels );
        return *this;
      }

      O3Label&       O3Type::name() { return __name; }
      const O3Label& O3Type::name() const { return __name; }

      O3Label&       O3Type::superLabel() { return __superLabel; }
      const O3Label& O3Type::superLabel() const { return __superLabel; }

      O3Type::LabelMap&       O3Type::labels() { return __labels; }
      const O3Type::LabelMap& O3Type::labels() const { return __labels; }

      O3Position&       O3Type::position() { return __pos; }
      const O3Position& O3Type::position() const { return __pos; }

      O3IntType::O3IntType() { GUM_CONSTRUCTOR( O3IntType ); }

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

      O3Label&       O3IntType::name() { return __name; }
      const O3Label& O3IntType::name() const { return __name; }

      O3Integer&       O3IntType::start() { return __start; }
      const O3Integer& O3IntType::start() const { return __start; }

      O3Integer&       O3IntType::end() { return __end; }
      const O3Integer& O3IntType::end() const { return __end; }

      O3Position&       O3IntType::position() { return __pos; }
      const O3Position& O3IntType::position() const { return __pos; }

      O3RealType::O3RealType() { GUM_CONSTRUCTOR( O3RealType ); }

      O3RealType::O3RealType( const O3RealType& src )
          : __pos( src.__pos )
          , __name( src.__name )
          , __values( src.__values ) {
        GUM_CONS_CPY( O3RealType );
      }

      O3RealType::O3RealType( O3RealType&& src )
          : __pos( std::move( src.__pos ) )
          , __name( std::move( src.__name ) )
          , __values( std::move( src.__values ) ) {
        GUM_CONS_MOV( O3RealType );
      }

      O3RealType::~O3RealType() { GUM_DESTRUCTOR( O3RealType ); }

      O3RealType& O3RealType::operator=( const O3RealType& src ) {
        if ( this == &src ) {
          return *this;
        }

        __pos = src.__pos;
        __name = src.__name;
        __values = src.__values;
        return *this;
      }

      O3RealType& O3RealType::operator=( O3RealType&& src ) {
        if ( this == &src ) {
          return *this;
        }

        __pos = std::move( src.__pos );
        __name = std::move( src.__name );
        __values = std::move( src.__values );
        return *this;
      }

      O3Position&       O3RealType::position() { return __pos; }
      const O3Position& O3RealType::position() const { return __pos; }

      O3Label& O3RealType::name() { return __name; }

      const O3Label& O3RealType::name() const { return __name; }

      std::vector<O3Float>& O3RealType::values() { return __values; }

      const std::vector<O3Float>& O3RealType::values() const { return __values; }

      O3PRM::O3PRM() {
        GUM_CONSTRUCTOR( O3PRM );
        // Creating the boolean type
        auto name = O3Label( O3Position(), "boolean" );
        auto f = O3Label( O3Position(), "false" );
        auto t = O3Label( O3Position(), "true" );
        auto labels = O3Type::LabelMap();
        labels.push_back( O3Type::LabelPair( f, O3Label() ) );
        labels.push_back( O3Type::LabelPair( t, O3Label() ) );
        auto boolean = std::unique_ptr<O3Type>( new O3Type() );
        boolean->name() = std::move( name );
        boolean->labels() = std::move( labels );
        __types.push_back( std::move( boolean ) );
      }

      O3PRM::O3PRM( const O3PRM& src ) {
        GUM_CONS_CPY( O3PRM );
        for ( const auto& t : src.__types ) {
          __types.emplace_back( new O3Type( *t ) );
        }
        for ( const auto& t : src.__int_types ) {
          __int_types.emplace_back( new O3IntType( *t ) );
        }
        for ( const auto& t : src.__real_types ) {
          __real_types.emplace_back( new O3RealType( *t ) );
        }
        for ( const auto& i : src.__interfaces ) {
          __interfaces.emplace_back( new O3Interface( *i ) );
        }
        for ( const auto& c : src.__classes ) {
          __classes.emplace_back( new O3Class( *c ) );
        }
        for ( const auto& s : src.__systems ) {
          __systems.emplace_back( new O3System( *s ) );
        }
        for ( const auto& i : src.__imports ) {
          __imports.emplace_back( new O3Import( *i ) );
        }
      }

      O3PRM::O3PRM( O3PRM&& src )
          : __types( std::move( src.__types ) )
          , __int_types( std::move( src.__int_types ) )
          , __real_types( std::move( src.__real_types ) )
          , __interfaces( std::move( src.__interfaces ) )
          , __classes( std::move( src.__classes ) )
          , __systems( std::move( src.__systems ) )
          , __imports( std::move( src.__imports ) ) {
        GUM_CONS_MOV( O3PRM );
      }

      O3PRM::~O3PRM() { GUM_DESTRUCTOR( O3PRM ); }

      O3PRM& O3PRM::operator=( const O3PRM& src ) {
        if ( this == &src ) {
          return *this;
        }
        for ( const auto& t : src.__types ) {
          __types.emplace_back( new O3Type( *t ) );
        }
        for ( const auto& t : src.__int_types ) {
          __int_types.emplace_back( new O3IntType( *t ) );
        }
        for ( const auto& t : src.__real_types ) {
          __real_types.emplace_back( new O3RealType( *t ) );
        }
        for ( const auto& i : src.__interfaces ) {
          __interfaces.emplace_back( new O3Interface( *i ) );
        }
        for ( const auto& c : src.__classes ) {
          __classes.emplace_back( new O3Class( *c ) );
        }
        for ( const auto& s : src.__systems ) {
          __systems.emplace_back( new O3System( *s ) );
        }
        for ( const auto& i : src.__imports ) {
          __imports.emplace_back( new O3Import( *i ) );
        }
        return *this;
      }

      O3PRM& O3PRM::operator=( O3PRM&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __types = std::move( src.__types );
        __int_types = std::move( src.__int_types );
        __real_types = std::move( src.__real_types );
        __interfaces = std::move( src.__interfaces );
        __classes = std::move( src.__classes );
        __systems = std::move( src.__systems );
        __imports = std::move( src.__imports );
        return *this;
      }

      O3PRM::O3TypeList&       O3PRM::types() { return __types; }
      const O3PRM::O3TypeList& O3PRM::types() const { return __types; }

      O3PRM::O3IntTypeList&       O3PRM::int_types() { return __int_types; }
      const O3PRM::O3IntTypeList& O3PRM::int_types() const { return __int_types; }

      O3PRM::O3RealTypeList&       O3PRM::real_types() { return __real_types; }
      const O3PRM::O3RealTypeList& O3PRM::real_types() const {
        return __real_types;
      }

      O3PRM::O3InterfaceList&       O3PRM::interfaces() { return __interfaces; }
      const O3PRM::O3InterfaceList& O3PRM::interfaces() const {
        return __interfaces;
      }

      O3PRM::O3ClassList& O3PRM::classes() { return __classes; }

      const O3PRM::O3ClassList& O3PRM::classes() const { return __classes; }

      O3PRM::O3SystemList& O3PRM::systems() { return __systems; }

      const O3PRM::O3SystemList& O3PRM::systems() const { return __systems; }

      O3PRM::O3ImportList& O3PRM::imports() { return __imports; }

      const O3PRM::O3ImportList& O3PRM::imports() const { return __imports; }

      O3InterfaceElement::O3InterfaceElement()
          : __isArray( false ) {
        GUM_CONSTRUCTOR( O3InterfaceElement );
      }

      O3InterfaceElement::O3InterfaceElement( const O3Label& type,
                                              const O3Label& name,
                                              bool           isArray )
          : __type( type )
          , __name( name )
          , __isArray( isArray ) {
        GUM_CONSTRUCTOR( O3InterfaceElement );
      }

      O3InterfaceElement::O3InterfaceElement( const O3InterfaceElement& src )
          : __type( src.__type )
          , __name( src.__name )
          , __isArray( src.__isArray ) {
        GUM_CONS_CPY( O3InterfaceElement );
      }

      O3InterfaceElement::O3InterfaceElement( O3InterfaceElement&& src )
          : __type( std::move( src.__type ) )
          , __name( std::move( src.__name ) )
          , __isArray( std::move( src.__isArray ) ) {
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
        __isArray = src.__isArray;
        return *this;
      }

      O3InterfaceElement& O3InterfaceElement::
      operator=( O3InterfaceElement&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __type = std::move( src.__type );
        __name = std::move( src.__name );
        __isArray = std::move( src.__isArray );
        return *this;
      }

      O3Label&       O3InterfaceElement::type() { return __type; }
      const O3Label& O3InterfaceElement::type() const { return __type; }

      O3Label&       O3InterfaceElement::name() { return __name; }
      const O3Label& O3InterfaceElement::name() const { return __name; }

      bool& O3InterfaceElement::isArray() { return __isArray; }
      bool  O3InterfaceElement::isArray() const { return __isArray; }

      O3Interface::O3Interface()
          : __elts( new O3InterfaceElementList() ) {
        GUM_CONSTRUCTOR( O3Interface );
      }

      O3Interface::O3Interface( const O3Interface& src )
          : __pos( src.__pos )
          , __name( src.__name )
          , __superLabel( src.__superLabel ) {
        GUM_CONS_CPY( O3Interface );
        auto copy = new O3InterfaceElementList( src.elements() );
        __elts = std::unique_ptr<O3InterfaceElementList>( copy );
      }

      O3Interface::O3Interface( O3Interface&& src )
          : __pos( std::move( src.__pos ) )
          , __name( std::move( src.__name ) )
          , __superLabel( std::move( src.__superLabel ) )
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
        __superLabel = src.__superLabel;
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
        __superLabel = std::move( src.__superLabel );
        __elts = std::move( src.__elts );
        return *this;
      }

      O3Position&       O3Interface::position() { return __pos; }
      const O3Position& O3Interface::position() const { return __pos; }

      O3Label&       O3Interface::name() { return __name; }
      const O3Label& O3Interface::name() const { return __name; }

      O3Label&       O3Interface::superLabel() { return __superLabel; }
      const O3Label& O3Interface::superLabel() const { return __superLabel; }

      O3Interface::O3InterfaceElementList& O3Interface::elements() {
        return *__elts;
      }
      const O3Interface::O3InterfaceElementList& O3Interface::elements() const {
        return *__elts;
      }

      O3Attribute::O3Attribute() { GUM_CONSTRUCTOR( O3Attribute ); }

      O3Attribute::O3Attribute( const O3Label&     type,
                                const O3Label&     name,
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

      O3Label&       O3Attribute::type() { return __type; }
      const O3Label& O3Attribute::type() const { return __type; }

      O3Label&       O3Attribute::name() { return __name; }
      const O3Label& O3Attribute::name() const { return __name; }

      O3Attribute::O3LabelList&       O3Attribute::parents() { return __parents; }
      const O3Attribute::O3LabelList& O3Attribute::parents() const {
        return __parents;
      }

      O3RawCPT::O3RawCPT()
          : O3Attribute()
          , __values{new O3FormulaList{}} {
        GUM_CONSTRUCTOR( O3RawCPT );
      }

      O3RawCPT::O3RawCPT( const O3Label&       type,
                          const O3Label&       name,
                          const O3LabelList&   parents,
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

      O3RawCPT::O3FormulaList& O3RawCPT::values() { return *__values; }

      const O3RawCPT::O3FormulaList& O3RawCPT::values() const { return *__values; }

      std::unique_ptr<O3Attribute> O3RawCPT::copy() const {
        auto copy = new O3RawCPT( *this );
        return std::unique_ptr<O3Attribute>( copy );
      }

      O3RuleCPT::O3RuleCPT()
          : O3Attribute()
          , __rules{new O3RuleList{}} {
        GUM_CONSTRUCTOR( O3RuleCPT );
      }

      O3RuleCPT::O3RuleCPT( const O3Label&                  type,
                            const O3Label&                  name,
                            const O3Attribute::O3LabelList& parents,
                            O3RuleList&&                    rules )
          : O3Attribute( type, name, parents )
          , __rules( std::unique_ptr<O3RuleList>( new O3RuleList( rules ) ) ) {
        GUM_CONSTRUCTOR( O3RuleCPT );
      }

      O3RuleCPT::O3RuleCPT( const O3RuleCPT& src )
          : O3Attribute( src )
          , __rules(
                std::unique_ptr<O3RuleList>( new O3RuleList( src.rules() ) ) ) {
        GUM_CONS_CPY( O3RuleCPT );
      }

      O3RuleCPT::O3RuleCPT( O3RuleCPT&& src )
          : O3Attribute( src )
          , __rules( std::move( src.__rules ) ) {
        GUM_CONS_MOV( O3RuleCPT );
      }

      O3RuleCPT::~O3RuleCPT() { GUM_DESTRUCTOR( O3RuleCPT ); }

      O3RuleCPT& O3RuleCPT::operator=( const O3RuleCPT& src ) {
        if ( this == &src ) {
          return *this;
        }

        O3Attribute::operator=( src );
        __rules = std::unique_ptr<O3RuleList>( new O3RuleList( src.rules() ) );
        return *this;
      }

      O3RuleCPT& O3RuleCPT::operator=( O3RuleCPT&& src ) {
        if ( this == &src ) {
          return *this;
        }

        O3Attribute::operator=( src );
        __rules = std::move( src.__rules );
        return *this;
      }

      O3RuleCPT::O3RuleList&       O3RuleCPT::rules() { return *__rules; }
      const O3RuleCPT::O3RuleList& O3RuleCPT::rules() const { return *__rules; }

      std::unique_ptr<O3Attribute> O3RuleCPT::copy() const {
        auto copy = new O3RuleCPT( *this );
        return std::unique_ptr<O3Attribute>( copy );
      }

      O3Class::O3Class() {
        GUM_CONSTRUCTOR( O3Class );

        auto i = new O3LabelList();
        __interfaces = std::unique_ptr<O3LabelList>( i );

        auto p = new O3ParameterList();
        __params = std::unique_ptr<O3ParameterList>( p );

        auto r = new O3ReferenceSlotList();
        __refs = std::unique_ptr<O3ReferenceSlotList>( r );

        auto e = new O3AttributeList();
        __attrs = std::unique_ptr<O3AttributeList>( e );

        auto a = new O3AggregateList();
        __aggs = std::unique_ptr<O3AggregateList>( a );
      }

      O3Class::O3Class( const O3Class& src )
          : __pos( src.__pos )
          , __name( src.__name )
          , __superLabel( src.__superLabel ) {
        GUM_CONS_CPY( O3Class );
        auto i = new O3LabelList( src.interfaces() );
        __interfaces = std::unique_ptr<O3LabelList>( i );

        auto p = new O3ParameterList( src.parameters() );
        __params = std::unique_ptr<O3ParameterList>( p );

        auto r = new O3ReferenceSlotList( src.referenceSlots() );
        __refs = std::unique_ptr<O3ReferenceSlotList>( r );

        auto e = new O3AttributeList();
        __attrs = std::unique_ptr<O3AttributeList>( e );
        for ( const auto& elt : src.attributes() ) {
          __attrs->push_back( elt->copy() );
        }

        auto a = new O3AggregateList( src.aggregates() );
        __aggs = std::unique_ptr<O3AggregateList>( a );
      }

      O3Class::O3Class( O3Class&& src )
          : __pos( std::move( src.__pos ) )
          , __name( std::move( src.__name ) )
          , __superLabel( std::move( src.__superLabel ) )
          , __interfaces( std::move( src.__interfaces ) )
          , __params( std::move( src.__params ) )
          , __refs( std::move( src.__refs ) )
          , __attrs( std::move( src.__attrs ) )
          , __aggs( std::move( src.__aggs ) ) {
        GUM_CONS_MOV( O3Class );
      }

      O3Class::~O3Class() { GUM_DESTRUCTOR( O3Class ); }

      O3Class& O3Class::operator=( const O3Class& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = src.__pos;
        __name = src.__name;
        __superLabel = src.__superLabel;

        auto i = new O3LabelList( src.interfaces() );
        __interfaces = std::unique_ptr<O3LabelList>( i );

        auto p = new O3ParameterList( src.parameters() );
        __params = std::unique_ptr<O3ParameterList>( p );

        auto r = new O3ReferenceSlotList( src.referenceSlots() );
        __refs = std::unique_ptr<O3ReferenceSlotList>( r );

        auto e = new O3AttributeList();
        __attrs = std::unique_ptr<O3AttributeList>( e );
        for ( const auto& elt : src.attributes() ) {
          __attrs->push_back( elt->copy() );
        }

        auto a = new O3AggregateList( src.aggregates() );
        __aggs = std::unique_ptr<O3AggregateList>( a );
        return *this;
      }

      O3Class& O3Class::operator=( O3Class&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __pos = std::move( src.__pos );
        __name = std::move( src.__name );
        __superLabel = std::move( src.__superLabel );
        __interfaces = std::move( src.__interfaces );
        __params = std::move( src.__params );
        __refs = std::move( src.__refs );
        __attrs = std::move( src.__attrs );
        __aggs = std::move( src.__aggs );
        return *this;
      }

      const O3Position& O3Class::position() const { return __pos; }
      O3Position&       O3Class::position() { return __pos; }

      const O3Label& O3Class::name() const { return __name; }
      O3Label&       O3Class::name() { return __name; }

      const O3Label& O3Class::superLabel() const { return __superLabel; }
      O3Label&       O3Class::superLabel() { return __superLabel; }

      const O3Class::O3LabelList& O3Class::interfaces() const {
        return *__interfaces;
      }
      O3Class::O3LabelList& O3Class::interfaces() { return *__interfaces; }

      const O3Class::O3ParameterList& O3Class::parameters() const {
        return *__params;
      }
      O3Class::O3ParameterList& O3Class::parameters() { return *__params; }

      const O3Class::O3ReferenceSlotList& O3Class::referenceSlots() const {
        return *__refs;
      }
      O3Class::O3ReferenceSlotList& O3Class::referenceSlots() { return *__refs; }

      O3Class::O3AttributeList& O3Class::attributes() { return *__attrs; }

      const O3Class::O3AttributeList& O3Class::attributes() const {
        return *__attrs;
      }

      O3Class::O3AggregateList&       O3Class::aggregates() { return *__aggs; }
      const O3Class::O3AggregateList& O3Class::aggregates() const {
        return *__aggs;
      }

      O3Parameter::O3Parameter()
          : __type( O3Parameter::PRMType::NONE ) {
        GUM_CONSTRUCTOR( O3Parameter );
      }

      O3Parameter::O3Parameter( const O3Position& pos,
                                const O3Label&    name,
                                const O3Integer&  value )
          : __type( O3Parameter::PRMType::INT )
          , __pos( pos )
          , __name( name )
          , __value( O3Float( value.position(), (float)value.value() ) ) {
        GUM_CONSTRUCTOR( O3Parameter );
      }

      O3Parameter::O3Parameter( const O3Position& pos,
                                const O3Label&    name,
                                const O3Float&    value )
          : __type( O3Parameter::PRMType::FLOAT )
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

      O3Parameter::~O3Parameter() { GUM_DESTRUCTOR( O3Parameter ); }

      O3Parameter& O3Parameter::operator=( const O3Parameter& src ) {
        if ( this == &src ) {
          return *this;
        }
        __type = src.__type;
        __pos = src.__pos;
        __name = src.__name;
        __value = src.__value;
        return *this;
      }

      O3Parameter& O3Parameter::operator=( O3Parameter&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __type = std::move( src.__type );
        __pos = std::move( src.__pos );
        __name = std::move( src.__name );
        __value = std::move( src.__value );
        return *this;
      }

      O3Parameter::PRMType& O3Parameter::type() { return __type; }
      O3Parameter::PRMType  O3Parameter::type() const { return __type; }

      O3Position&       O3Parameter::position() { return __pos; }
      const O3Position& O3Parameter::position() const { return __pos; }

      O3Label&       O3Parameter::name() { return __name; }
      const O3Label& O3Parameter::name() const { return __name; }

      O3Float&       O3Parameter::value() { return __value; }
      const O3Float& O3Parameter::value() const { return __value; }

      O3ReferenceSlot::O3ReferenceSlot() { GUM_CONSTRUCTOR( O3ReferenceSlot ); }

      O3ReferenceSlot::O3ReferenceSlot( const O3Label& type,
                                        const O3Label& name,
                                        bool           isArray )
          : __type( type )
          , __name( name )
          , __isArray( isArray ) {
        GUM_CONSTRUCTOR( O3ReferenceSlot );
      }

      O3ReferenceSlot::O3ReferenceSlot( const O3ReferenceSlot& src )
          : __type( src.__type )
          , __name( src.__name )
          , __isArray( src.__isArray ) {
        GUM_CONS_CPY( O3ReferenceSlot );
      }

      O3ReferenceSlot::O3ReferenceSlot( O3ReferenceSlot&& src )
          : __type( std::move( src.__type ) )
          , __name( std::move( src.__name ) )
          , __isArray( std::move( src.__isArray ) ) {
        GUM_CONS_MOV( O3ReferenceSlot );
      }

      O3ReferenceSlot::~O3ReferenceSlot() { GUM_DESTRUCTOR( O3ReferenceSlot ); }

      O3ReferenceSlot& O3ReferenceSlot::operator=( const O3ReferenceSlot& src ) {
        if ( this == &src ) {
          return *this;
        }
        __type = src.__type;
        __name = src.__name;
        __isArray = src.__isArray;
        return *this;
      }

      O3ReferenceSlot& O3ReferenceSlot::operator=( O3ReferenceSlot&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __type = std::move( src.__type );
        __name = std::move( src.__name );
        __isArray = std::move( src.__isArray );
        return *this;
      }

      O3Label&       O3ReferenceSlot::type() { return __type; }
      const O3Label& O3ReferenceSlot::type() const { return __type; }

      O3Label&       O3ReferenceSlot::name() { return __name; }
      const O3Label& O3ReferenceSlot::name() const { return __name; }

      bool& O3ReferenceSlot::isArray() { return __isArray; }
      bool  O3ReferenceSlot::isArray() const { return __isArray; }

      O3Aggregate::O3Aggregate()
          : __variableType()
          , __aggregateType()
          , __name()
          , __parents()
          , __parameters() {
        GUM_CONSTRUCTOR( O3Aggregate );
      }

      O3Aggregate::O3Aggregate( const O3Aggregate& src )
          : __variableType( src.__variableType )
          , __aggregateType( src.__aggregateType )
          , __name( src.__name )
          , __parents( src.__parents )
          , __parameters( src.__parameters ) {
        GUM_CONS_CPY( O3Aggregate );
      }

      O3Aggregate::O3Aggregate( O3Aggregate&& src )
          : __variableType( std::move( src.__variableType ) )
          , __aggregateType( std::move( src.__aggregateType ) )
          , __name( std::move( src.__name ) )
          , __parents( std::move( src.__parents ) )
          , __parameters( std::move( src.__parameters ) ) {
        GUM_CONS_MOV( O3Aggregate );
      }

      O3Aggregate::~O3Aggregate() { GUM_DESTRUCTOR( O3Aggregate ); }

      O3Aggregate& O3Aggregate::operator=( const O3Aggregate& src ) {
        if ( this == &src ) {
          return *this;
        }
        __aggregateType = src.__aggregateType;
        __variableType = src.__variableType;
        __name = src.__name;
        __parents = src.__parents;
        __parameters = src.__parameters;
        return *this;
      }

      O3Aggregate& O3Aggregate::operator=( O3Aggregate&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __aggregateType = std::move( src.__aggregateType );
        __variableType = std::move( src.__variableType );
        __name = std::move( src.__name );
        __parents = std::move( src.__parents );
        __parameters = std::move( src.__parameters );
        return *this;
      }

      O3Label& O3Aggregate::variableType() { return __variableType; }

      const O3Label& O3Aggregate::variableType() const { return __variableType; }

      O3Label& O3Aggregate::aggregateType() { return __aggregateType; }

      const O3Label& O3Aggregate::aggregateType() const { return __aggregateType; }

      O3Label& O3Aggregate::name() { return __name; }

      const O3Label& O3Aggregate::name() const { return __name; }

      O3Aggregate::O3LabelList& O3Aggregate::parents() { return __parents; }

      const O3Aggregate::O3LabelList& O3Aggregate::parents() const {
        return __parents;
      }

      O3Aggregate::O3LabelList& O3Aggregate::parameters() { return __parameters; }

      const O3Aggregate::O3LabelList& O3Aggregate::parameters() const {
        return __parameters;
      }

      O3Assignment::O3Assignment() { GUM_CONSTRUCTOR( O3Assignment ); }

      O3Assignment::O3Assignment( const O3Assignment& src )
          : __leftInstance( src.__leftInstance )
          , __leftIndex( src.__leftIndex )
          , __leftReference( src.__leftReference )
          , __rightInstance( src.__rightInstance )
          , __rightIndex( src.__rightIndex ) {
        GUM_CONS_CPY( O3Assignment );
      }

      O3Assignment::O3Assignment( O3Assignment&& src )
          : __leftInstance( std::move( src.__leftInstance ) )
          , __leftIndex( std::move( src.__leftIndex ) )
          , __leftReference( std::move( src.__leftReference ) )
          , __rightInstance( std::move( src.__rightInstance ) )
          , __rightIndex( std::move( src.__rightIndex ) ) {
        GUM_CONS_CPY( O3Assignment );
      }

      O3Assignment::~O3Assignment() { GUM_DESTRUCTOR( O3Assignment ); }

      O3Assignment& O3Assignment::operator=( const O3Assignment& src ) {
        if ( this == &src ) {
          return *this;
        }
        __leftInstance = src.__leftInstance;
        __leftIndex = src.__leftIndex;
        __leftReference = src.__leftReference;
        __rightInstance = src.__rightInstance;
        __rightIndex = src.__rightIndex;
        return *this;
      }

      O3Assignment& O3Assignment::operator=( O3Assignment&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __leftInstance = std::move( src.__leftInstance );
        __leftIndex = std::move( src.__leftIndex );
        __leftReference = std::move( src.__leftReference );
        __rightInstance = std::move( src.__rightInstance );
        __rightIndex = std::move( src.__rightIndex );
        return *this;
      }

      const O3Label& O3Assignment::leftInstance() const { return __leftInstance; }

      O3Label& O3Assignment::leftInstance() { return __leftInstance; }

      const O3Integer& O3Assignment::leftIndex() const { return __leftIndex; }

      O3Integer& O3Assignment::leftIndex() { return __leftIndex; }

      const O3Label& O3Assignment::leftReference() const {
        return __leftReference;
      }

      O3Label& O3Assignment::leftReference() { return __leftReference; }

      const O3Label& O3Assignment::rightInstance() const {
        return __rightInstance;
      }

      O3Label& O3Assignment::rightInstance() { return __rightInstance; }

      O3Integer& O3Assignment::rightIndex() { return __rightIndex; }

      const O3Integer& O3Assignment::rightIndex() const { return __rightIndex; }

      O3Increment::O3Increment() { GUM_CONSTRUCTOR( O3Increment ); }

      O3Increment::O3Increment( const O3Increment& src )
          : __leftInstance( src.__leftInstance )
          , __leftIndex( src.__leftIndex )
          , __leftReference( src.__leftReference )
          , __rightInstance( src.__rightInstance )
          , __rightIndex( src.__rightIndex ) {
        GUM_CONS_CPY( O3Increment );
      }

      O3Increment::O3Increment( O3Increment&& src )
          : __leftInstance( std::move( src.__leftInstance ) )
          , __leftIndex( std::move( src.__leftIndex ) )
          , __leftReference( std::move( src.__leftReference ) )
          , __rightInstance( std::move( src.__rightInstance ) )
          , __rightIndex( std::move( src.__rightIndex ) ) {
        GUM_CONS_CPY( O3Increment );
      }

      O3Increment::~O3Increment() { GUM_DESTRUCTOR( O3Increment ); }

      O3Increment& O3Increment::operator=( const O3Increment& src ) {
        if ( this == &src ) {
          return *this;
        }
        __leftInstance = src.__leftInstance;
        __leftIndex = src.__leftIndex;
        __leftReference = src.__leftReference;
        __rightInstance = src.__rightInstance;
        __rightIndex = src.__rightIndex;
        return *this;
      }

      O3Increment& O3Increment::operator=( O3Increment&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __leftInstance = std::move( src.__leftInstance );
        __leftIndex = std::move( src.__leftIndex );
        __leftReference = std::move( src.__leftReference );
        __rightInstance = std::move( src.__rightInstance );
        __rightIndex = std::move( src.__rightIndex );
        return *this;
      }

      const O3Label& O3Increment::leftInstance() const { return __leftInstance; }

      O3Label& O3Increment::leftInstance() { return __leftInstance; }

      const O3Integer& O3Increment::leftIndex() const { return __leftIndex; }

      O3Integer& O3Increment::leftIndex() { return __leftIndex; }

      const O3Label& O3Increment::leftReference() const { return __leftReference; }

      O3Label& O3Increment::leftReference() { return __leftReference; }

      const O3Label& O3Increment::rightInstance() const { return __rightInstance; }

      O3Integer&       O3Increment::rightIndex() { return __rightIndex; }
      const O3Integer& O3Increment::rightIndex() const { return __rightIndex; }

      O3Label& O3Increment::rightInstance() { return __rightInstance; }

      O3Instance::O3Instance() { GUM_CONSTRUCTOR( O3Instance ); }

      O3Instance::O3Instance( const O3Instance& src )
          : __type( src.__type )
          , __name( src.__name )
          , __size( src.__size )
          , __parameters( src.__parameters ) {
        GUM_CONS_CPY( O3Instance );
      }

      O3Instance::O3Instance( O3Instance&& src )
          : __type( std::move( src.__type ) )
          , __name( std::move( src.__name ) )
          , __size( std::move( src.__size ) )
          , __parameters( std::move( src.__parameters ) ) {
        GUM_CONS_MOV( O3Instance );
      }

      O3Instance::~O3Instance() { GUM_DESTRUCTOR( O3Instance ); }

      O3Instance& O3Instance::operator=( const O3Instance& src ) {
        if ( this == &src ) {
          return *this;
        }
        __type = src.__type;
        __name = src.__name;
        __size = src.__size;
        __parameters = src.__parameters;
        return *this;
      }

      O3Instance& O3Instance::operator=( O3Instance&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __type = std::move( src.__type );
        __name = std::move( src.__name );
        __size = std::move( src.__size );
        __parameters = std::move( src.__parameters );
        return *this;
      }

      const O3Label& O3Instance::type() const { return __type; }

      O3Label& O3Instance::type() { return __type; }

      const O3Label& O3Instance::name() const { return __name; }

      O3Label& O3Instance::name() { return __name; }

      const O3Integer& O3Instance::size() const { return __size; }

      O3Integer& O3Instance::size() { return __size; }

      const O3Instance::O3InstanceParameterList& O3Instance::parameters() const {
        return __parameters;
      }

      O3Instance::O3InstanceParameterList& O3Instance::parameters() {
        return __parameters;
      }

      O3System::O3System() { GUM_CONSTRUCTOR( O3System ); }

      O3System::O3System( const O3System& src )
          : __name( src.__name )
          , __instances( src.__instances )
          , __assigments( src.__assigments )
          , __increments( src.__increments ) {
        GUM_CONS_CPY( O3System );
      }

      O3System::O3System( O3System&& src )
          : __name( std::move( src.__name ) )
          , __instances( std::move( src.__instances ) )
          , __assigments( std::move( src.__assigments ) )
          , __increments( std::move( src.__increments ) ) {
        GUM_CONS_MOV( O3System );
      }

      O3System::~O3System() { GUM_DESTRUCTOR( O3System ); }

      O3System& O3System::operator=( const O3System& src ) {
        if ( this == &src ) {
          return *this;
        }
        __name = src.__name;
        __instances = src.__instances;
        __assigments = src.__assigments;
        __increments = src.__increments;
        return *this;
      }

      O3System& O3System::operator=( O3System&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __name = std::move( src.__name );
        __instances = std::move( src.__instances );
        __assigments = std::move( src.__assigments );
        __increments = std::move( src.__increments );
        return *this;
      }

      const O3Label& O3System::name() const { return __name; }

      O3Label& O3System::name() { return __name; }

      const O3System::O3InstanceList& O3System::instances() const {
        return __instances;
      }

      O3System::O3InstanceList& O3System::instances() { return __instances; }

      const O3System::O3AssignmentList& O3System::assignments() const {
        return __assigments;
      }

      O3System::O3AssignmentList& O3System::assignments() { return __assigments; }

      const O3System::O3IncrementList& O3System::increments() const {
        return __increments;
      }

      O3System::O3IncrementList& O3System::increments() { return __increments; }

      std::ostream& operator<<( std::ostream& o, const O3Label& src ) {
        o << src.label();
        return o;
      }

      O3InstanceParameter::O3InstanceParameter()
          : __isInteger( false ) {
        GUM_CONSTRUCTOR( O3InstanceParameter );
      }

      O3InstanceParameter::O3InstanceParameter( const O3InstanceParameter& src )
          : __name( src.__name )
          , __value( src.__value )
          , __isInteger( src.__isInteger ) {
        GUM_CONS_CPY( O3InstanceParameter );
      }

      O3InstanceParameter::O3InstanceParameter( O3InstanceParameter&& src )
          : __name( std::move( src.__name ) )
          , __value( std::move( src.__value ) )
          , __isInteger( std::move( src.__isInteger ) ) {
        GUM_CONS_MOV( O3InstanceParameter );
      }

      O3InstanceParameter::~O3InstanceParameter() {
        GUM_DESTRUCTOR( O3InstanceParameter );
      }

      O3InstanceParameter& O3InstanceParameter::
      operator=( const O3InstanceParameter& src ) {
        if ( this == &src ) {
          return *this;
        }
        __name = src.__name;
        __value = src.__value;
        __isInteger = src.__isInteger;
        return *this;
      }

      O3InstanceParameter& O3InstanceParameter::
      operator=( O3InstanceParameter&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __name = std::move( src.__name );
        __value = std::move( src.__value );
        __isInteger = std::move( src.__isInteger );
        return *this;
      }

      const O3Label& O3InstanceParameter::name() const { return __name; }

      O3Label& O3InstanceParameter::name() { return __name; }

      const O3Float& O3InstanceParameter::value() const { return __value; }

      O3Float& O3InstanceParameter::value() { return __value; }

      bool O3InstanceParameter::isInteger() const { return __isInteger; }

      bool& O3InstanceParameter::isInteger() { return __isInteger; }

      O3Import::O3Import() { GUM_CONSTRUCTOR( O3Import ); }

      O3Import::O3Import( const O3Import& src )
          : __import( src.__import ) {
        GUM_CONSTRUCTOR( O3Import );
      }

      O3Import::O3Import( O3Import&& src )
          : __import( std::move( src.__import ) ) {
        GUM_CONS_MOV( O3Import );
      }

      O3Import::~O3Import() { GUM_DESTRUCTOR( O3Import ); }

      O3Import& O3Import::operator=( const O3Import& src ) {
        if ( this == &src ) {
          return *this;
        }
        __import = src.__import;
        return *this;
      }

      O3Import& O3Import::operator=( O3Import&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __import = std::move( src.__import );
        return *this;
      }

      const O3Label& O3Import::import() const { return __import; }

      O3Label& O3Import::import() { return __import; }

    }  // o3prm
  }    // prm
}  // gum
