/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
/** @file
 * @brief The inlined implementation of DBCells
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/learning/database/DBCell.h>
namespace gum {

  namespace learning {

    /// default constructor
    INLINE DBCell::DBCell() { GUM_CONSTRUCTOR( DBCell ); }

    /// constructor for a number
    INLINE DBCell::DBCell( double nb )
        : __value( nb ) {
      GUM_CONSTRUCTOR( DBCell );
    }

    /// constructor for a string
    INLINE DBCell::DBCell( const std::string& str )
        : __type( DBCell::EltType::STRING ) {
      // store the string into the static list of strings
      if ( !__strings().existsFirst( str ) ) {
        __strings().insert( str, __string_max_index );
        __index = __string_max_index;
        ++__string_max_index;
      } else {
        __index = __strings().second( str );
      }

      GUM_CONSTRUCTOR( DBCell );
    }

    /// copy constructor
    INLINE DBCell::DBCell( const DBCell& from )
        : __type( from.__type ) {
      std::memcpy( &__value, &( from.__value ), sizeof( Real ) );

      // for debugging
      GUM_CONS_CPY( DBCell );
    }

    /// move constructor
    INLINE DBCell::DBCell( DBCell&& from )
        : __type( from.__type ) {
      std::memcpy( &__value, &( from.__value ), sizeof( Real ) );

      // for debugging
      GUM_CONS_MOV( DBCell );
    }

    /// destructor
    INLINE DBCell::~DBCell() { GUM_DESTRUCTOR( DBCell ); }

    /// copy operator
    INLINE DBCell& DBCell::operator=( const DBCell& from ) {
      if ( this != &from ) {
        __type = from.__type;
        std::memcpy( &__value, &( from.__value ), sizeof( Real ) );
      }

      return *this;
    }

    /// move operator
    INLINE DBCell& DBCell::operator=( DBCell&& from ) {
      if ( this != &from ) {
        __type = from.__type;
        std::memcpy( &__value, &( from.__value ), sizeof( Real ) );
      }

      return *this;
    }

    /// unsafe set operator (assumes that the preceding type is of the same
    /// type)
    INLINE DBCell& DBCell::operator=( double x ) noexcept {
      __type = EltType::REAL;
      __value = x;
      return *this;
    }

    /// unsafe set operator (assumes that the preceding type is of the same
    /// type)
    INLINE DBCell& DBCell::operator=( const std::string& str ) noexcept {
      if ( !__strings().existsFirst( str ) ) {
        __strings().insert( str, __string_max_index );
        __index = __string_max_index;
        ++__string_max_index;
      } else {
        __index = __strings().second( str );
      }

      return *this;
    }

    /// returns the DBcell as a double (without checking its type)
    INLINE double DBCell::getReal() const noexcept { return __value; }

    /// returns the DBcell as a double (safe with type checking)
    INLINE double DBCell::getRealSafe() const {
      if ( __type == EltType::REAL )
        return __value;
      else
        GUM_ERROR( TypeError, "the DBCell does not contain a double" );
    }

    /// unsafe set (assumes that the preceding type is of the same type)
    INLINE void DBCell::setReal( double x ) { __value = x; }

    /// sets the content of the DBCell (safe type checking)
    INLINE void DBCell::setRealSafe( double elt ) {
      __type = EltType::REAL;
      __value = elt;
    }

    /// sets the content of the DBCell from a string
    INLINE void DBCell::__setRealFromStringSafe( const std::string& elt ) {
      __value = stof( elt );
      __type = EltType::REAL;
    }

    /// returns the DBcell as a string (without checking its type)
    INLINE const std::string& DBCell::getString() const noexcept {
      return __strings().first( __index );
    }

    /// returns the DBcell as a string (safe with type checking)
    INLINE const std::string& DBCell::getStringSafe() const {
      if ( __type == EltType::STRING )
        return __strings().first( __index );
      else
        GUM_ERROR( TypeError, "the DBCell does not contain a string" );
    }

    /// returns the DBcell as a string index (without checking its type)
    INLINE int DBCell::getStringIndex() const noexcept { return __index; }

    /// returns the DBcell as a string (safe with type checking)
    INLINE int DBCell::getStringIndexSafe() const {
      if ( __type == EltType::STRING )
        return __index;
      else
        GUM_ERROR( TypeError, "the DBCell does not contain a string" );
    }

    /// returns the DBcell as a string (without checking its type)
    INLINE const std::string& DBCell::getString( Idx index ) {
      return __strings().first( index );
    }

    /// unsafe set (assumes that the preceding type is of the same type)
    INLINE void DBCell::setString( const std::string& str ) {
      if ( !__strings().existsFirst( str ) ) {
        __strings().insert( str, __string_max_index );
        __index = __string_max_index;
        ++__string_max_index;
      } else {
        __index = __strings().second( str );
      }
    }

    /// sets the content of the DBCell (safe type checking)
    INLINE void DBCell::setStringSafe( const std::string& str ) {
      if ( !__strings().existsFirst( str ) ) {
        __strings().insert( str, __string_max_index );
        __index = __string_max_index;
        ++__string_max_index;
      } else {
        __index = __strings().second( str );
      }
      __type = EltType::STRING;
    }

    /// sets the DBCell as a missing element
    INLINE void DBCell::setMissingSafe() { __type = EltType::MISSING; }

    /// safely sets the content of the DBCell with the best possible type
    INLINE void DBCell::setBestTypeSafe( const std::string& elt ) {
      // try to convert the string into a double
      try {
        __setRealFromStringSafe( elt );
        return;
      } catch ( std::invalid_argument& ) {
      }

      // here, the best type is a string
      setStringSafe( elt );
    }

    /// returns the current type of the DBCell
    INLINE DBCell::EltType DBCell::type() const noexcept { return __type; }

    /// try to convert the content of the DBCell into another type
    INLINE bool DBCell::convertType( EltType new_type ) {
      if ( new_type == __type ) return true;
      switch ( new_type ) {
        // ===================================
        case REAL:
          switch ( __type ) {
            case STRING:
              try {
                __value = stof( __strings().first( __index ) );
                __type = EltType::REAL;
                return true;
              } catch ( std::invalid_argument& ) {
                return false;
              }

            case MISSING:
              __type = EltType::MISSING;
              return true;

            default:
              GUM_ERROR( TypeError, "type not supported by DBCell convertType" );
          }

        // ===================================
        case STRING:
          switch ( __type ) {
            case REAL: {
              std::string str = std::to_string( __value );
              if ( !__strings().existsFirst( str ) ) {
                __strings().insert( str, __string_max_index );
                __index = __string_max_index;
                ++__string_max_index;
              } else {
                __index = __strings().second( str );
              }
              __type = EltType::STRING;
            }
              return true;

            case MISSING:
              __type = EltType::MISSING;
              return true;

            default:
              GUM_ERROR( TypeError, "type not supported by DBCell convertType" );
          }

        // ===================================
        case MISSING:
          setMissingSafe();
          return true;

        default:
          GUM_ERROR( TypeError, "type not supported by DBCell convertType" );
      }

      return false;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
