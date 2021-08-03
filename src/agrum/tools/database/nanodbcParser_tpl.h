/***************************************************************************
 *   Copyright (c) 2005-2020 by Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)  *
 *   info_at_agrum_dot_org                                               *
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
 * @author Lionel TORTI, Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifdef ODBC_

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/tools/database/nanodbcParser.h>

namespace gum {

  namespace learning {


    /// Default constructor
    template <template<typename> class ALLOC>
    NanodbcParser<ALLOC>::NanodbcParser( const ALLOC<std::string>& alloc )
      :  _data_( alloc ) {
      GUM_CONSTRUCTOR( NanodbcParser );
    }
    
    
    /// constructor that executes an SQL query if the connection is active
    template <template<typename> class ALLOC>
    NanodbcParser<ALLOC>::NanodbcParser( nanodbc::connection& connexion,
                                         const std::string& query,
                                         const ALLOC<std::string>& alloc )
      :  _data_( alloc ) {
      // check if there is a connection. If so, execute the query
      if ( connexion.connected () ) {
         _result_ =  nanodbc::execute( connexion, query );
         _data_.resize ( std::size_t (  _result_.columns() ) );
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
        if (  _result_.next() ) {
          const std::size_t nb_cols = std::size_t (  _result_.columns() );
          char str[100]; // buffer for retrieving floats
          for ( std::size_t i = 0; i < nb_cols; ++i ) {
            const short pos ( i );
            try {
              const int type =  _result_.column_datatype ( pos );

              // if the column contains a numeric field, we should use
              // method get<float>, otherwise a get<string> should be ok
              // WARNING: using a get<string> to get the content of a
              // real-valued field will provide incorrect results
              if ( ( type >= SQL_NUMERIC ) && ( type <= SQL_DOUBLE ) ) {
                sprintf ( str, "%g",  _result_.get<float>( pos ) );
                 _data_[i] = str;
              }
              else {
                 _data_[i] =  _result_.get<std::string>( pos );
              }
            } catch ( nanodbc::null_access_error& e ) {
               _data_[i] = "NULL";
            }
          }
          ++ _nb_line_;
          return true;
        }
      } catch ( std::runtime_error& e ) {
        GUM_ERROR( DatabaseError, std::string( e.what() ) )
      }
       _data_.clear();
      return false;
    }

    
    // return the current number line
    template <template<typename> class ALLOC>
    INLINE std::size_t NanodbcParser<ALLOC>::nbLine() const {
      return  _nb_line_ >= 1 ?  _nb_line_ - 1 : std::size_t(0);
    }

    
    // returns the current parsed line.
    template <template<typename> class ALLOC>
    INLINE
    const std::vector<std::string,ALLOC<std::string>>&
    NanodbcParser<ALLOC>::current() const {
      if ( !  _data_.empty () ) {
        return  _data_;
      }

      GUM_ERROR( NullElement, "No parsed data" )
    }

    
    /// start a new query
    template <template<typename> class ALLOC>
    void NanodbcParser<ALLOC>::useNewQuery ( nanodbc::connection& connexion,
                                             const std::string& query ) {
       _result_ = nanodbc::execute( connexion, query );
       _data_.resize ( std::size_t (  _result_.columns() ) );
       _nb_line_ = std::size_t(0);
    }

    
    /// returns the number of columns in the query result
    template <template<typename> class ALLOC>
    INLINE std::size_t NanodbcParser<ALLOC>::nbColumns () const {
      return std::size_t (  _result_.columns () );
    }

    
    /// returns the name of the ith column
    template <template<typename> class ALLOC>
    INLINE std::string
    NanodbcParser<ALLOC>::columnName ( const std::size_t i ) const {
      return  _result_.column_name( i );
    }


  }  // namespace learning

}  // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#endif  // ODBC_
