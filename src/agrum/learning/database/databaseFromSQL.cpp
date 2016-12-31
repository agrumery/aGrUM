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
 * @brief The class representing a tabular database created from a SQL query.
 *
 * @author Lionel Torti, Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifdef _ODBC

#include <agrum/learning/database/databaseFromSQL.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/database/databaseFromSQL_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    DatabaseFromSQL::DatabaseFromSQL( const std::string&             dataSource,
                                      const std::string&             login,
                                      const std::string&             password,
                                      const std::string&             query,
                                      long                           timeout,
                                      const DBTransform&             transform,
                                      const std::vector<std::string> missingVal ) {
      try {
        nanodbc::connection cn( dataSource, login, password, timeout );
        nanodbc::result     res = nanodbc::execute( cn, query );

        NanodbcParser parser( res );

        // Get names from result's columns
        auto& names = _variableNames();
        for ( short i = 0; i < res.columns(); ++i ) {
          names.push_back( res.column_name( i ) );
        }

        // use the NanodbcParser to fill the vector of DBRows
        auto& vect = _content();

        while ( parser.next() ) {
          // read a new line in the input file and convert it into a DBRow
          const auto& row = parser.current();

          DBRow new_row( row.size() );

          for ( Idx i = 0; i < row.size(); ++i ) {
            new_row[i].setBestTypeSafe( row[i] );
          }

          // add the result into
          vect.push_back( new_row );
        }

        // if we wish to apply a DBTransform to preprocess the database
        transform.transform( vect, missingVal );

      } catch ( nanodbc::database_error& e ) {
        GUM_ERROR( DatabaseError, e.what() );
      }

      // for debugging purposes
      GUM_CONSTRUCTOR( DatabaseFromSQL );
    }

  } /* namespace learning */

} /* namespace gum */

#endif  // _ODBC
