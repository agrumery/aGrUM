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
/**
 * @file
 * @brief Class for parsing SQL results using Nanodbc.
 *
 * @author Lionel TORTI, Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifdef _ODBC

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/learning/database/nanodbcParser.h>

namespace gum {

  namespace learning {


    /// Default constructor
    template <template<typename> class ALLOC>
    NanodbcParser<ALLOC>::NanodbcParser( const ALLOC<std::string>& alloc )
      : __data( alloc ) {
      GUM_CONSTRUCTOR( NanodbcParser );
    }
    
    
    /// constructor that executes an SQL query if the connection is active
    template <template<typename> class ALLOC>
    NanodbcParser<ALLOC>::NanodbcParser( nanodbc::connection& connexion,
                                         const std::string& query,
                                         const ALLOC<std::string>& alloc )
      : __data( alloc ) {
      // check if there is a connection. If so, execute the query
      if ( connexion.connected () ) {
        __result =  nanodbc::execute( connexion, query );
        __data.resize ( std::size_t ( __result.columns() ) );
      }
      GUM_CONSTRUCTOR( NanodbcParser );
    }

    
    /// destructor
    template <template<typename> class ALLOC>
    NanodbcParser<ALLOC>::~NanodbcParser() {
      GUM_DESTRUCTOR( NanodbcParser );
    }

    
    /// Gets the next line of the SQL stream and parses it.
    template <template<typename> class ALLOC>
    bool NanodbcParser<ALLOC>::next() {
      /* Extract from sql.h:
         SQL data type codes 
         #define SQL_UNKNOWN_TYPE    0
         #define SQL_CHAR            1
         #define SQL_NUMERIC         2
         #define SQL_DECIMAL         3
         #define SQL_INTEGER         4
         #define SQL_SMALLINT        5
         #define SQL_FLOAT           6
         #define SQL_REAL            7
         #define SQL_DOUBLE          8
         #if (ODBCVER >= 0x0300)
         #define SQL_DATETIME        9
         #endif
         #define SQL_VARCHAR        12 */
      try {
        if ( __result.next() ) {
          const std::size_t nb_cols = std::size_t ( __result.columns() );
          char str[100]; // buffer for retrieving floats
          for ( std::size_t i = 0; i < nb_cols; ++i ) {
            const short pos ( i );
            try {
              const int type = __result.column_datatype ( pos );

              // if the column contains a numeric field, we should use
              // method get<float>, otherwise a get<string> should be ok
              // WARNING: using a get<string> to get the content of a
              // real-valued field will provide incorrect results
              if ( ( type >= SQL_NUMERIC ) && ( type <= SQL_DOUBLE ) ) {
                sprintf ( str, "%g", __result.get<float>( pos ) );
                __data[i] = str;
              }
              else {
                __data[i] = __result.get<std::string>( pos );
              }
            } catch ( nanodbc::null_access_error& e ) {
              __data[i] = "NULL";
            }
          }
          ++__nb_line;
          return true;
        }
      } catch ( std::runtime_error& e ) {
        GUM_ERROR( DatabaseError, std::string( e.what() ) );
      }
      __data.clear();
      return false;
    }

    
    // return the current number line
    template <template<typename> class ALLOC>
    INLINE std::size_t NanodbcParser<ALLOC>::nbLine() const {
      return __nb_line >= 1 ? __nb_line - 1 : std::size_t(0);
    }

    
    // returns the current parsed line.
    template <template<typename> class ALLOC>
    INLINE
    const std::vector<std::string,ALLOC<std::string>>&
    NanodbcParser<ALLOC>::current() const {
      if ( ! __data.empty () ) {
        return __data;
      }

      GUM_ERROR( NullElement, "No parsed data" );
    }

    
    /// start a new query
    template <template<typename> class ALLOC>
    void NanodbcParser<ALLOC>::useNewQuery ( nanodbc::connection& connexion,
                                             const std::string& query ) {
      __result = nanodbc::execute( connexion, query );
      __data.resize ( std::size_t ( __result.columns() ) );
      __nb_line = std::size_t(0);
    }

    
    /// returns the number of columns in the query result
    template <template<typename> class ALLOC>
    INLINE std::size_t NanodbcParser<ALLOC>::nbColumns () const {
      return std::size_t ( __result.columns () );
    }

    
    /// returns the name of the ith column
    template <template<typename> class ALLOC>
    INLINE std::string
    NanodbcParser<ALLOC>::columnName ( const std::size_t i ) const {
      return __result.column_name( i );
    }


  }  // namespace learning

}  // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#endif  // _ODBC
