/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr   *
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

#include <agrum/core/math/formula.h>

// Keep this here because of cyclic dependencies
#include <agrum/core/math/cocoR/Parser.h>
#include <agrum/core/math/cocoR/Scanner.h>

namespace gum {

  // ==========================================================================
  // ===                         Class FormulaPart                          ===
  // ==========================================================================

  // Helper function for debuging
  void print_stack( std::stack<FormulaPart> s ) {
    std::cout << std::endl;
    std::list<FormulaPart> l;
    while ( !s.empty() ) {
      l.push_front( s.top() );
      s.pop();
    }

    std::cout << "Stack: ";
    for ( const auto& elt : l ) {
      std::cout << elt.str() << " ";
    }
    std::cout << std::endl;
  }

  // Helper function for debuging
  void print_output( std::vector<FormulaPart> v ) {
    std::cout << "Output: ";
    for ( const auto& elt : v ) {
      std::cout << elt.str() << " ";
    }
    std::cout << std::endl;
  }

  std::string func2str( FormulaPart::token_function func ) {
    switch ( func ) {
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
      default: { GUM_ERROR( OperationNotAllowed, "unknown function" ); }
    }
  }

  FormulaPart::FormulaPart()
      : type( token_type::NIL )
      , number( NAN )
      , character( '\0' )
      , function( nil ) {
    GUM_CONSTRUCTOR( FormulaPart );
  }

  FormulaPart::FormulaPart( token_type t, double n )
      : type( t )
      , number( n )
      , character( '\0' )
      , function( nil ) {
    GUM_CONSTRUCTOR( FormulaPart );
  }

  FormulaPart::FormulaPart( token_type t, char c )
      : type( t )
      , number( NAN )
      , character( c )
      , function( nil ) {
    GUM_CONSTRUCTOR( FormulaPart );
  }

  FormulaPart::FormulaPart( token_type t, token_function func )
      : type( t )
      , number( NAN )
      , character( '\0' )
      , function( func ) {
    GUM_CONSTRUCTOR( FormulaPart );
  }

  FormulaPart::FormulaPart( const FormulaPart& source )
      : type( source.type )
      , number( source.number )
      , character( source.character )
      , function( source.function ) {
    GUM_CONS_CPY( FormulaPart );
  }

  FormulaPart::FormulaPart( FormulaPart&& source )
      : type( std::move( source.type ) )
      , number( std::move( source.number ) )
      , character( std::move( source.character ) )
      , function( std::move( source.function ) ) {
    GUM_CONS_MOV( FormulaPart );
  }

  FormulaPart::~FormulaPart() { GUM_DESTRUCTOR( FormulaPart ); }

  FormulaPart& FormulaPart::operator=( const FormulaPart& source ) {
    if ( this == &source ) {
      return *this;
    }

    type = source.type;
    number = source.number;
    character = source.character;
    function = source.function;

    return *this;
  }

  FormulaPart& FormulaPart::operator=( FormulaPart&& source ) {
    if ( this == &source ) {
      return *this;
    }

    type = std::move( source.type );
    number = std::move( source.number );
    character = std::move( source.character );
    function = std::move( source.function );

    return *this;
  }

  std::string FormulaPart::str() const {
    std::ostringstream s;
    switch ( type ) {
      case token_type::NUMBER: {
        s << number;
        break;
      }

      case token_type::PARENTHESIS:
      case token_type::OPERATOR: {
        if ( character == '\0' ) {
          s << "\\0";
        } else {
          s << character;
        }
        break;
      }

      case token_type::FUNCTION: {
        s << func2str( function );
        break;
      }

      default: { GUM_ERROR( OperationNotAllowed, "unknown type" ); }
    }
    return s.str();
  }

  // ==========================================================================
  // ===                        Class Formula                               ===
  // ==========================================================================

  void Formula::__initialise() {
    auto c_str = (unsigned char*)__formula.c_str();
    auto scanner = new gum::formula::Scanner( c_str, (int)__formula.size() );
    __scanner = std::unique_ptr<gum::formula::Scanner>( scanner );

    auto parser = new gum::formula::Parser( scanner );
    __parser = std::unique_ptr<gum::formula::Parser>( parser );
    __parser->formula( this );
  }

  Formula::Formula( short s )
      : __formula( std::to_string( s ) )
      , __last_token( FormulaPart() ) {
    GUM_CONSTRUCTOR( Formula );
    __initialise();
  }

  Formula::Formula( unsigned short us )
      : __formula( std::to_string( us ) )
      , __last_token( FormulaPart() ) {
    GUM_CONSTRUCTOR( Formula );
    __initialise();
  }

  Formula::Formula( int i )
      : __formula( std::to_string( i ) )
      , __last_token( FormulaPart() ) {
    GUM_CONSTRUCTOR( Formula );
    __initialise();
  }

  Formula::Formula( unsigned int ui )
      : __formula( std::to_string( ui ) )
      , __last_token( FormulaPart() ) {
    GUM_CONSTRUCTOR( Formula );
    __initialise();
  }

  Formula::Formula( long l )
      : __formula( std::to_string( l ) )
      , __last_token( FormulaPart() ) {
    GUM_CONSTRUCTOR( Formula );
    __initialise();
  }

  Formula::Formula( unsigned long ul )
      : __formula( std::to_string( ul ) )
      , __last_token( FormulaPart() ) {
    GUM_CONSTRUCTOR( Formula );
    __initialise();
  }

  Formula::Formula( long long l )
      : __formula( std::to_string( l ) )
      , __last_token( FormulaPart() ) {
    GUM_CONSTRUCTOR( Formula );
    __initialise();
  }

  Formula::Formula( unsigned long long ul )
      : __formula( std::to_string( ul ) )
      , __last_token( FormulaPart() ) {
    GUM_CONSTRUCTOR( Formula );
    __initialise();
  }

  Formula::Formula( float f )
      : __formula( std::to_string( f ) )
      , __last_token( FormulaPart() ) {
    GUM_CONSTRUCTOR( Formula );
    __initialise();
  }

  Formula::Formula( double d )
      : __formula( std::to_string( d ) )
      , __last_token( FormulaPart() ) {
    GUM_CONSTRUCTOR( Formula );
    __initialise();
  }

  Formula::Formula( const std::string& f )
      : __formula( f )
      , __last_token( FormulaPart() ) {
    GUM_CONSTRUCTOR( Formula );

    __initialise();
  }

  Formula::Formula( const Formula& source )
      : __formula( source.__formula )
      , __last_token( source.__last_token )
      , __output( source.__output )
      , __stack( source.__stack ) {
    GUM_CONS_CPY( Formula );

    __initialise();
  }

  Formula::Formula( Formula&& source )
      : __formula( std::move( source.__formula ) )
      , __scanner( std::move( source.__scanner ) )
      , __parser( std::move( source.__parser ) )
      , __last_token( std::move( source.__last_token ) )
      , __output( std::move( source.__output ) )
      , __stack( std::move( source.__stack ) ) {
    GUM_CONS_CPY( Formula );

    __parser->formula( this );
  }

  Formula::~Formula() { GUM_DESTRUCTOR( Formula ); }

  Formula& Formula::operator=( const Formula& source ) {
    if ( this == &source ) {
      return *this;
    }

    __formula = source.__formula;
    __last_token = source.__last_token;
    __output = source.__output;
    __stack = source.__stack;

    __initialise();

    return *this;
  }

  Formula& Formula::operator=( Formula&& source ) {
    if ( this == &source ) {
      return *this;
    }

    __formula = std::move( source.__formula );
    __scanner = std::move( source.__scanner );
    __parser = std::move( source.__parser );
    __parser->formula( this );
    __last_token = std::move( source.__last_token );
    __output = std::move( source.__output );
    __stack = std::move( source.__stack );

    return *this;
  }

  double Formula::result() const {

    __parser->Parse();

    std::stack<FormulaPart> stack;
    if ( __output.empty() ) {
      GUM_ERROR( OperationNotAllowed, "no output found" );
    }

    for ( auto item : __output ) {


      switch ( item.type ) {
        case FormulaPart::token_type::NUMBER: {
          stack.push( item );
          break;
        }

        case FormulaPart::token_type::OPERATOR:
        case FormulaPart::token_type::FUNCTION: {
          __reduceOperatorOrFunction( item, stack );
          break;
        }

        default: {
          GUM_ERROR( OperationNotAllowed,
                     "expecting numbers, operators or functions" );
        }
      }
    }

    if ( stack.size() != 1 ) {

      GUM_ERROR( OperationNotAllowed, "too many inputs" );

    } else if ( stack.top().type != FormulaPart::token_type::NUMBER ) {

      GUM_ERROR( OperationNotAllowed, "too many inputs" );
    }
    return stack.top().number;
  }

}  // namespace gum

#ifdef GUM_NO_INLINE
#include <agrum/core/math/formula_inl.h>
#endif  // GUM_NO_INLINE
