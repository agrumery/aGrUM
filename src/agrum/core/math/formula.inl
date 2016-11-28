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

namespace gum {

  // ==========================================================================
  // ===                         Class FormulaPart                          ===
  // ==========================================================================


  INLINE
  bool FormulaPart::isLeftAssociative() const {
    switch ( character ) {
      case '+':
      case '-':
      case '*':
      case '/': {
        return true;
      }

      case '_': {
        return false;
      }
      case '^': {
        return false;
      }

      default: { GUM_ERROR( OperationNotAllowed, "A - not an operator" ); }
    }
  }

  INLINE
  bool FormulaPart::isRightAssociative() const {
    switch ( character ) {
      case '_': {
        return false;
      }
      default: { return !isLeftAssociative(); }
    }
  }

  INLINE
  int FormulaPart::precedence() const {
    switch ( character ) {
      case '+':
      case '-': {
        return 2;
      }

      case '*':
      case '/': {
        return 3;
      }

      case '^': {
        return 4;
      }

      case '_': {
        return 5;
      }

      default: { GUM_ERROR( OperationNotAllowed, "B - not an operator" ); }
    }
  }

  INLINE
  size_t FormulaPart::argc() const {
    switch ( type ) {
      case OPERATOR: {
        return __operator_argc();
      }

      case FUNCTION: {
        return __function_argc();
      }

      default: {
        GUM_ERROR( OperationNotAllowed, "expecting a function or an operator" );
      }
    }
  }

  INLINE
  size_t FormulaPart::__operator_argc() const {
    switch ( character ) {
      case '_': {
        return (size_t)1;
      }
      case '+':
      case '-':
      case '*':
      case '/':
      case '^': {
        return (size_t)2;
      }

      default: { GUM_ERROR( OperationNotAllowed, "C - not an operator" ); }
    }
  }

  INLINE
  size_t FormulaPart::__function_argc() const {
    switch ( function ) {
      case FormulaPart::token_function::exp: {
        return 1;
      }
      case FormulaPart::token_function::log: {
        return 1;
      }
      case FormulaPart::token_function::ln: {
        return 1;
      }
      case FormulaPart::token_function::pow: {
        return 2;
      }
      case FormulaPart::token_function::sqrt: {
        return 1;
      }
      // case FormulaPart::token_function::nil: { return "nil"; }
      default: { GUM_ERROR( OperationNotAllowed, "unknown function" ); }
    }
  }

  /// Args are backwards !
  INLINE
  double
  FormulaPart::__operator_eval( const std::vector<FormulaPart>& args ) const {
    switch ( character ) {
      case '+': {
        return args[1].number + args[0].number;
      }

      case '-': {
        return args[1].number - args[0].number;
      }

      case '*': {
        return args[1].number * args[0].number;
      }

      case '/': {
        return args[1].number / args[0].number;
      }

      case '^': {
        return std::pow( args[1].number, args[0].number );
      }

      case '_': {
        return 0 - args[0].number;
      }

      default: { GUM_ERROR( OperationNotAllowed, "D - not an operator" ); }
    }
  }

  /// Args are backwards !
  INLINE
  double
  FormulaPart::__function_eval( const std::vector<FormulaPart>& args ) const {
    switch ( function ) {
      case FormulaPart::token_function::exp: {
        return std::exp( args[0].number );
      }
      case FormulaPart::token_function::log: {
        return std::log( args[0].number );
      }
      case FormulaPart::token_function::ln: {
        return std::log2( args[0].number );
      }
      case FormulaPart::token_function::pow: {
        return std::pow( args[1].number, args[0].number );
      }
      case FormulaPart::token_function::sqrt: {
        return std::sqrt( args[0].number );
      }
      // case FormulaPart::token_function::nil: { return "nil"; }
      default: { GUM_ERROR( OperationNotAllowed, "unknown function" ); }
    }
  }

  /// Args are backwards !
  INLINE
  FormulaPart FormulaPart::eval( const std::vector<FormulaPart>& args ) const {
    switch ( type ) {
      case OPERATOR: {
        return FormulaPart( token_type::NUMBER, __operator_eval( args ) );
      }

      case FUNCTION: {
        return FormulaPart( token_type::NUMBER, __function_eval( args ) );
      }

      default: { GUM_ERROR( OperationNotAllowed, "cannot evaluate expression" ); }
    }
  }

  // ==========================================================================
  // ===                        Class Formula                               ===
  // ==========================================================================


  INLINE
  const std::string& Formula::formula() const { return __formula; }

  INLINE
  std::string& Formula::formula() { return __formula; }

  INLINE
  void Formula::__push_number( const double& v ) {
    FormulaPart t( FormulaPart::token_type::NUMBER, v );
    __push_output( t );
  }

  INLINE
  bool Formula::__popOperator( FormulaPart o ) {

    if ( __stack.empty() ||
         __stack.top().type != FormulaPart::token_type::OPERATOR ) {
      return false;
    }

    if ( o.isLeftAssociative() && o.precedence() <= __stack.top().precedence() ) {
      return true;
    }

    if ( o.isRightAssociative() && o.precedence() < __stack.top().precedence() ) {
      return true;
    }

    return false;
  }

  INLINE
  void Formula::__push_operator( char o ) {

    if ( __isUnaryOperator( o ) ) {

      __push_unaryOperator( o );

    } else {

      FormulaPart t( FormulaPart::token_type::OPERATOR, o );
      __push_operator( t );
    }
  }

  INLINE
  bool Formula::__isUnaryOperator( char o ) {
    switch ( __last_token.type ) {

      case FormulaPart::token_type::OPERATOR:
      case FormulaPart::token_type::NIL:
      case FormulaPart::token_type::ARG_SEP: {
        return o == '-';
      }

      case FormulaPart::token_type::PARENTHESIS: {
        return ( o == '-' ) && ( __last_token.character == '(' );
      }

      default: { return false; }
    }
  }

  INLINE
  void Formula::__push_unaryOperator( char o ) {

    // Only unary operator is the negative sign -
    FormulaPart t( FormulaPart::token_type::OPERATOR, '_' );
    __push_operator( t );
  }

  INLINE
  void Formula::__push_operator( FormulaPart t ) {

    while ( __popOperator( t ) ) {
      __push_output( __stack.top() );
      __stack.pop();
    }

    __push_stack( t );
  }

  INLINE
  void Formula::__push_leftParenthesis() {

    FormulaPart t( FormulaPart::token_type::PARENTHESIS, '(' );
    __push_stack( t );
  }

  INLINE
  void Formula::__push_rightParenthesis() {

    while ( ( !__stack.empty() ) && ( __stack.top().character != '(' ) ) {
      __push_output( __stack.top() );
      __stack.pop();
    }

    if ( __stack.empty() ) {

      GUM_ERROR( OperationNotAllowed, "expecting '('" );

    } else if ( __stack.top().character != '(' ) {

      GUM_ERROR( OperationNotAllowed, "expecting '('" );
    }

    __stack.pop();

    if ( ( !__stack.empty() ) &&
         __stack.top().type == FormulaPart::token_type::FUNCTION ) {

      __push_output( __stack.top() );
      __stack.pop();
    }
    __last_token = FormulaPart( FormulaPart::token_type::PARENTHESIS, ')' );
  }

  INLINE
  void Formula::__finalize() {

    while ( !__stack.empty() ) {

      if ( __stack.top().character == '(' ) {

        GUM_ERROR( OperationNotAllowed, "expecting ')'" );
      }

      __push_output( __stack.top() );
      __stack.pop();
    }
  }

  INLINE
  void
  Formula::__reduceOperatorOrFunction( FormulaPart              item,
                                       std::stack<FormulaPart>& stack ) const {

    std::vector<FormulaPart> args;

    if ( stack.size() < item.argc() ) {
      GUM_ERROR( OperationNotAllowed, "not enought inputs " );
    }

    while ( item.argc() > args.size() ) {

      args.push_back( stack.top() );
      stack.pop();
    }

    stack.push( item.eval( args ) );
  }

  INLINE
  void Formula::__push_output( FormulaPart t ) {
    __output.push_back( t );
    __last_token = t;
  }

  INLINE
  void Formula::__push_stack( FormulaPart t ) {
    __stack.push( t );
    __last_token = t;
  }

  INLINE
  void Formula::__push_function( const std::string& func ) {

    if ( func == "exp" ) {

      FormulaPart t( FormulaPart::token_type::FUNCTION,
                     FormulaPart::token_function::exp );
      __push_stack( t );

    } else if ( func == "log" ) {

      FormulaPart t( FormulaPart::token_type::FUNCTION,
                     FormulaPart::token_function::log );
      __push_stack( t );

    } else if ( func == "ln" ) {

      FormulaPart t( FormulaPart::token_type::FUNCTION,
                     FormulaPart::token_function::ln );
      __push_stack( t );

    } else if ( func == "pow" ) {

      FormulaPart t( FormulaPart::token_type::FUNCTION,
                     FormulaPart::token_function::pow );
      __push_stack( t );

    } else if ( func == "sqrt" ) {

      FormulaPart t( FormulaPart::token_type::FUNCTION,
                     FormulaPart::token_function::sqrt );
      __push_stack( t );

    } else {

      GUM_ERROR( OperationNotAllowed, "unknown function" );
    }
  }

  INLINE
  void Formula::__push_comma() {
    while ( ( !__stack.empty() ) && ( __stack.top().character != '(' ) ) {
      __push_output( __stack.top() );
      __stack.pop();
    }

    if ( __stack.empty() || __stack.top().character != '(' ) {
      GUM_ERROR( OperationNotAllowed, "expecting a '('" );
    }

    __last_token = FormulaPart( FormulaPart::token_type::ARG_SEP, ',' );
  }

  INLINE
  HashTable<std::string, double>& Formula::variables() { return __variables; }

  INLINE
  const HashTable<std::string, double>& Formula::variables() const {
    return __variables;
  }

  INLINE
  void Formula::__push_variable( const std::string& var ) {

    if ( __variables.exists( var ) ) {

      __push_number( __variables[var] );

    } else {

      GUM_ERROR( OperationNotAllowed, "unknonw variable" );
    }
  }

  INLINE
  void Formula::__push_identifier( const std::string& ident ) {
    try {

      __push_function( ident );

    } catch ( OperationNotAllowed& ) {

      try {

        __push_variable( ident );

      } catch ( OperationNotAllowed& ) {

        GUM_ERROR( OperationNotAllowed, "unknown identifier" );
      }
    }
  }

  // ========================================================================
  //  @name Arithmetic Operators
  // ========================================================================

  INLINE
  Formula operator-( const Formula& a ) {
    return Formula( std::to_string( -1 * a.result() ) );
  }

  INLINE
  Formula operator+( const Formula& a, const Formula& b ) {
    return Formula( std::to_string( a.result() + b.result() ) );
  }

  INLINE
  Formula operator-( const Formula& a, const Formula& b ) {
    return Formula( std::to_string( a.result() - b.result() ) );
  }

  INLINE
  Formula operator*( const Formula& a, const Formula& b ) {
    return Formula( std::to_string( a.result() * b.result() ) );
  }

  INLINE
  Formula operator/( const Formula& a, const Formula& b ) {
    return Formula( std::to_string( a.result() / b.result() ) );
  }

  INLINE
  std::string to_string( const Formula& f ) {
    return std::to_string( f.result() );
  }

  INLINE
  std::ostream& operator<<( std::ostream& os, const Formula& f ) {
    os << f.result();
    return os;
  }

}  // namespace gum
