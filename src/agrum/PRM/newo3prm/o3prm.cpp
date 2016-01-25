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
        __file = src.__file;
        __line = src.__line;
        __column = src.__column;
        return *this;
      }

      Position& Position::operator=( Position&& src ) {
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
        __pos = src.__pos;
        __label = src.__label;
        return *this;
      }

      O3Label& O3Label::operator=( O3Label&& src ) {
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
          LabelMap& labels )
        : __position( pos )
          , __name( name )
          , __super( super )
          , __labels( labels ) {
            GUM_CONSTRUCTOR( O3Type );
          }

      O3Type::O3Type( const O3Type& src )
          : __position( src.__position )
          , __name( src.__name )
          , __super( src.__super )
          , __labels( src.__labels ) {
        GUM_CONS_CPY( O3Type );
      }

      O3Type::O3Type( O3Type&& src )
          : __position( std::move( src.__position ) )
          , __name( std::move( src.__name ) )
          , __super( std::move( src.__super ) )
          , __labels( std::move( src.__labels ) ) {
        GUM_CONS_MOV( O3Type );
      }

      O3Type::~O3Type() { GUM_DESTRUCTOR( O3Type ); }

      O3Type& O3Type::operator=( const O3Type& src ) {
        __position = src.__position;
        __name = src.__name;
        __super = src.__super;
        __labels = src.__labels;
        return *this;
      }

      O3Type& O3Type::operator=( O3Type&& src ) {
        __position = std::move( src.__position );
        __name = std::move( src.__name );
        __super = std::move( src.__super );
        __labels = std::move( src.__labels );
        return *this;
      }

      const O3Label& O3Type::name() const { return __name; }
      const O3Label& O3Type::super() const { return __super; }
      const O3Type::LabelMap& O3Type::labels() const { return __labels; }
      const Position& O3Type::position() const { return __position; }

      O3PRM::O3PRM() { GUM_CONSTRUCTOR( O3PRM ); }

      O3PRM::O3PRM( const O3PRM& src )
          : __types( src.__types ) {
        GUM_CONS_CPY( O3PRM );
      }

      O3PRM::O3PRM( O3PRM&& src )
          : __types( std::move( src.__types ) ) {
        GUM_CONS_MOV( O3PRM );
      }

      O3PRM::~O3PRM() { GUM_DESTRUCTOR( O3PRM ); }

      O3PRM& O3PRM::operator=( const O3PRM& src ) {
        __types = src.__types;
        return *this;
      }

      O3PRM& O3PRM::operator=( O3PRM&& src ) {
        __types = std::move( src.__types );
        return *this;
      }

      std::vector<O3Type>& O3PRM::types() { return __types; }
      const std::vector<O3Type>& O3PRM::types() const { return __types; }

    }  // o3prm
  }    // prm
}  // gum
