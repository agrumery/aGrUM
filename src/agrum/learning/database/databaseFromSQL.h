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
 * @brief The class representing a tabular database created from an SQL query.
 *
 * @author Lionel Torti, Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifdef _ODBC

#ifndef GUM_DATABASE_FROM_SQL_H
#define GUM_DATABASE_FROM_SQL_H

#include <string>
#include <vector>

#include <agrum/core/exceptions.h>

#include <agrum/learning/database/DBTransform.h>
#include <agrum/learning/database/DBTransformIdentity.h>
#include <agrum/learning/database/databaseVectInRAM.h>
#include <agrum/learning/database/nanodbc/nanodbc.h>
#include <agrum/learning/database/nanodbcParser.h>

namespace gum {

  namespace learning {

    /**
     * @class DatabaseFromSQL
     * @brief The class representing a tabular database created from a SQL
     * query.
     * @ingroup learning_group
     */
    class DatabaseFromSQL : public DatabaseVectInRAM {
      public:
      /// the handler used for parsing the database
      using Handler = typename DatabaseVectInRAM::Handler;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /**
       * @brief Default constructor.
       *
       * This will read the result of query and load it in memory using the
       * DatabaseVectInRAM class.
       *
       * @param dataSource A declared datraSource in your odbc configuration
       * file (usually /etc/odbc.ini).
       * @param login The dataSource login.
       * @param password The dataSource password.
       * @param query The SQL query used as a database.
       * @param timeout Defines a timeout for accessing the SQL database, if 0
       * then no timeout is set.
       * @param transform Used for transforming data into expected types.
       * @param missingVal How missing values (NULL in SQL) will be
       * represented.
       */
      DatabaseFromSQL( const std::string& dataSource,
                       const std::string& login,
                       const std::string& password,
                       const std::string& query,
                       long               timeout = 0,
                       const DBTransform& transform = DBTransformIdentity(),
                       const std::vector<std::string> missingVal = {"NULL"} );

      /// Copy constructor
      DatabaseFromSQL( const DatabaseFromSQL& from );

      /// Move constructor
      DatabaseFromSQL( DatabaseFromSQL&& from );

      /// Destructor
      virtual ~DatabaseFromSQL();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      DatabaseFromSQL& operator=( const DatabaseFromSQL& from );

      /// move constructor
      DatabaseFromSQL& operator=( DatabaseFromSQL&& from );

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/database/databaseFromSQL_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_DATABASE_FROM_SQL_H */

#endif  // _ODBC
