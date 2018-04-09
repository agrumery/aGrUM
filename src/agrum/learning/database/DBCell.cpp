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
 * @brief The implementation of DBCells
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <cstdio>
#include <agrum/learning/database/DBCell.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/database/DBCell_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    // create the static members
    int DBCell::__string_max_index = 0;

    
    Bijection<std::string,int>& DBCell::__strings() {
#ifndef NDEBUG
      static bool first_time = true;
      if ( first_time ) {
        first_time = false;
        __debug__::__dec_creation(
            "Bijection", "__strings", 0, "BCell string bijection", 0 );
        __debug__::__dec_creation( "BijectionImplementation",
                                   "__strings",
                                   0,
                                   "BCell string bijection",
                                   0 );
        __debug__::__dec_creation(
            "HashTable", "__strings", 0, "BCell string bijection", 0 );
        __debug__::__dec_creation(
            "HashTable", "__strings", 0, "BCell string bijection", 0 );
      }
#endif
      static Bijection<std::string,int> strings;
      return strings;
    }

    
    // determines whether a string corresponds to an integer
    bool DBCell::isInteger ( const std::string& str ) {
      if( str.empty() ) return false;
                    
      // trim the string
      auto start_iter = str.begin() + str.find_first_not_of(" \t");
      auto end_iter   = str.begin() + str.find_last_not_of(" \t\r\n") + 1;

      if( start_iter == end_iter ) return false;

      // if the number is negative, pass the '-' sign
      if( *start_iter == '-' ) ++start_iter;

      // check wether we have a number
      for ( ; start_iter != end_iter; ++start_iter ) {
        if ( ( *start_iter < '0' ) || ( *start_iter > '9' ) ) return false;
      }

      return true;
    }

    
    // determines whether a string corresponds to an integer
    bool DBCell::isReal ( const std::string& str ) {
      if( str.empty() ) return false;
                    
      // trim the string
      auto start_iter = str.begin() + str.find_first_not_of(" \t");
      auto end_iter   = str.begin() + str.find_last_not_of(" \t\r\n") + 1;

      if( start_iter == end_iter ) return false;

      // check wether we have a number
      bool has_dot      = false;
      bool has_exponent = false;
      bool has_digit    = false;
      bool has_negation = false;
      for ( ; start_iter != end_iter; ++start_iter ) {
        if( *start_iter == '-' ) {
          if ( has_negation ) return false;
        }
        else if ( *start_iter == '.' ) {
          if ( has_dot || has_exponent ) return false;
          has_dot = true;
        }
        else if ( ( *start_iter == 'e' ) || ( *start_iter == 'E' ) ) {
          if ( has_exponent || ! has_digit ) return false;
          has_exponent = true;
          has_negation = false;
        }
        else if ( ( *start_iter < '0' ) || ( *start_iter > '9' ) ) return false;
        else has_digit = true;
      }

      return true;
    }


    // try to convert the content of the DBCell into another type
    bool DBCell::convertType( const EltType new_type ) {
      if ( new_type == __type ) return true;
      switch ( new_type ) {
        // ===================================
      case EltType::REAL:
        switch ( __type ) {
        case EltType::INTEGER:
          __val_real = float ( __val_integer );
          __type = EltType::REAL;
          return true;

        case EltType::STRING:
          try {
            const std::string& str = __strings().first( __val_index );
            if ( ! isReal ( str ) ) return false;
            __val_real = std::stof( str );
            __type = EltType::REAL;
            return true;
          }
          catch ( std::invalid_argument& ) {
            return false;
          }

        case EltType::MISSING:
          return false;
          
        default:
          GUM_ERROR( NotImplementedYet,
                     "type not supported by DBCell convertType" );
        }

        // ===================================
      case EltType::INTEGER:
        switch ( __type ) {
        case EltType::REAL:
          {
            const int nb = int ( __val_real );
            if ( nb == __val_real ) {
              __val_integer = nb;
              __type = EltType::INTEGER;
              return true;
            }
            else return false;
          }

        case EltType::STRING:
          try {
            const std::string& str = __strings().first( __val_index );
            if ( ! isInteger ( str ) ) return false;
            __val_integer = std::stoi( str );
            __type = EltType::INTEGER;
            return true;
          }
          catch ( std::invalid_argument& ) {
            return false;
          }

        case EltType::MISSING:
          return false;
          
        default:
          GUM_ERROR( NotImplementedYet,
                     "type not supported by DBCell convertType" );
        }

        // ===================================
      case EltType::STRING:
        switch ( __type ) {
        case EltType::REAL:
          {
            char buffer[100];
            sprintf( buffer, "%g", __val_real );
            const std::string str ( buffer );
            if ( !__strings().existsFirst( str ) ) {
              __strings().insert( str, __string_max_index );
              __val_index = __string_max_index;
              ++__string_max_index;
            }
            else {
              __val_index = __strings().second( str );
            }
          }
          __type = EltType::STRING;
          return true;

        case EltType::INTEGER:
          {
            
            const std::string str = std::to_string( __val_integer );
            if ( !__strings().existsFirst( str ) ) {
              __strings().insert( str, __string_max_index );
              __val_index = __string_max_index;
              ++__string_max_index;
            }
            else {
              __val_index = __strings().second( str );
            }
          }
          __type = EltType::STRING;
          return true;

        case EltType::MISSING:
          return false;
          
        default:
          GUM_ERROR( NotImplementedYet,
                     "type not supported by DBCell convertType" );
        }
        
        // ===================================
      case EltType::MISSING:
        __type = EltType::MISSING;
        return true;

      default:
        GUM_ERROR( NotImplementedYet,
                   "type not supported by DBCell convertType" );
      }

      return false;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
