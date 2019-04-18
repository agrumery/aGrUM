
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief The class for initializing DatabaseTable and RawDatabaseTable
 * instances from SQL databases
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifdef _ODBC

#  ifndef GUM_LEARNING_DB_INITILIALIZER_FROM_SQL_H
#    define GUM_LEARNING_DB_INITILIALIZER_FROM_SQL_H

#    include <agrum/agrum.h>
#    include <agrum/learning/database/IDBInitializer.h>
#    include <agrum/learning/database/nanodbcParser.h>

namespace gum {

  namespace learning {

    /** @class DBInitializerFromSQL
     * @headerfile DBInitializerFromSQL.h <agrum/learning/database/DBInitializerFromSQL.h>
     * @ingroup learning_database
     * @brief The class for initializing DatabaseTable and RawDatabaseTable
     * instances from SQL databases
     *
     * In aGrUM, the usual way to create DatabaseTable instances used by learning
     * algorithms is to use the 4-step process below:
     * -# Create an IDBInitializer instance (either a DBInitializerFromCSV or a
     *    DBInitializerFromSQL). This will enable to get the variables
     *    corresponding to the columns of the DatabaseTable.
     * -# Knowing these variables, create a DBTranslatorSet for encoding the
     *    lines of the CSV file or those of the SQL result into the appropriate
     *    values for the learning algorithms.
     * -# Create the DatabaseTable, passing it the DBTranslatorSet created
     *    in the preceding step. Use the IDBInitializer to provide the variables'
     *    names to the DatabaseTable.
     * -# Use the IDBInitializer to add the lines of the CSV file or those of the
     *    SQL result into the DatabaseTable.
     *
     * @par The following codes show the details of this processe:
     * @code
     * // 1/ use the initializer to parse all the columns/rows of a SQL result:
     * const std::string dataSource = "PostgreSQL";
     * const std::string login      = "gonzales";
     * const std::string password   = "agrum";
     * const std::string query      = "SELECT * FROM testunit.asia";
     * gum::learning::DBInitializerFromSQL<>
     *   initializer ( dataSource, login, password, query );
     * const auto& var_names = initializer.variableNames ();
     * const std::size_t nb_vars = var_names.size ();
     *
     * // we create as many translators as there are variables
     * gum::learning::DBTranslator4LabelizedVariable<> translator;
     * gum::learning::DBTranslatorSet<> translator_set;
     * for ( std::size_t i = 0; i < nb_vars; ++i )
     *   translator_set.insertTranslator ( translator, i );

     * // create a DatabaseTable with these translators. For the moment, the
     * // DatabaseTable will be empty, i.e., it will contain no row
     * gum::learning::DatabaseTable<> database ( translator_set );
     * database.setVariableNames( initializer.variableNames () );
     *
     * // use the DBInitializerFromSQL to fill the rows:
     * initializer.fillDatabase ( database );
     * // now, the database contains all the content of the ASIA database
     *
     *
     * // 2/ use an IDBInitializer to initialize a DatabaseTable, but ignore
     * // some columns.
     * gum::learning::DBInitializerFromSQL<>
     *   initializer2 ( dataSource, login, password, query );
     * gum::learning::DatabaseTable<> database2; // empty database
     *
     * // indicate which columns of the SQL result should be read
     * database2.insertTranslator ( translator, 1 );
     * database2.insertTranslator ( translator, 3 );
     * database2.insertTranslator ( translator, 4 );
     *
     * // sets the names of the columns correctly
     * database2.setVariableNames( initializer2.variableNames () );
     *
     * // fill the rows:
     * initializer2.fillDatabase ( database2 );
     * // now all the rows of the SQL result have been transferred into
     * // database2, but only columns 1, 3 and 4 of the SQL result have
     * // been kept.
     *
     *
     * // 3/ another possibility to initialize a DatabaseTable, ignoring
     * // some columns:
     * gum::learning::DBInitializerFromSQL<>
     *   initializer3 ( dataSource, login, password, query );
     * gum::learning::DatabaseTable<> database3 ( translator_set );
     * // here, database3 is an empty database but it contains already
     * // translators for all the columns of the SQL result. We shall now
     * // remove the columns/translators that are not wanted anymore
     * database3.ignoreColumn ( 0 );
     * database3.ignoreColumn ( 2 );
     * database3.ignoreColumn ( 5 );
     * database3.ignoreColumn ( 6 );
     * database3.ignoreColumn ( 7 );
     * // asia contains 8 columns. The above ignoreColumns keep only columns
     * // 1, 3 and 4.
     *
     * // sets the names of the columns correctly
     * database3.setVariableNames( initializer3.variableNames () );
     * // fill the rows:
     * initializer3.fillDatabase ( database3 );
     * // now all the rows of the SQL result have been transferred into
     * // database3, but only columns 1, 3 and 4 of the SQL result have
     * // been kept.
     * @endcode
     */
    template < template < typename > class ALLOC = std::allocator >
    class DBInitializerFromSQL : public IDBInitializer< ALLOC > {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< std::string >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor, especially for postgresql databases
      /** This will read the result of query and load it in memory.
       *
       * @param dataSource A declared dataSource in your odbc configuration
       * file (usually /etc/odbc.ini).
       * @param login The dataSource login.
       * @param password The dataSource password.
       * @param query The SQL query used as a database.
       * @param timeout Defines a timeout for accessing the SQL database, if 0
       * then no timeout is set.
       * @param alloc the allocator used to allocate all the data structures
       */
      DBInitializerFromSQL(const std::string&    dataSource,
                           const std::string&    login,
                           const std::string&    password,
                           const std::string&    query,
                           long                  timeout = 0L,
                           const allocator_type& alloc = allocator_type());

      /// default constructor, especially for sqlite databases
      /** This will read the result of query and load it in memory.
       *
       * @param connection_string a string specifying to nanODBC how to connect
       * to a SQL database
       * @param query The SQL query used as a database.
       * @param timeout Defines a timeout for accessing the SQL database, if 0
       * then no timeout is set.
       * @param alloc the allocator used to allocate all the data structures
       */
      DBInitializerFromSQL(const std::string&    connection_string,
                           const std::string&    query,
                           long                  timeout = 0L,
                           const allocator_type& alloc = allocator_type());

      /// copy constructor
      /** the new initializer points to the same SQL query as from, but
       * it reparses the result it from scratch. */
      DBInitializerFromSQL(const DBInitializerFromSQL< ALLOC >& from);

      /// copy constructor with a given allocator
      /** the new initializer points to the same SQL query as from, but
       * it reparses the result from scratch. */
      DBInitializerFromSQL(const DBInitializerFromSQL< ALLOC >& from,
                           const allocator_type&                alloc);

      /// move constructor
      DBInitializerFromSQL(DBInitializerFromSQL< ALLOC >&& from);

      /// move constructor with a given allocator
      DBInitializerFromSQL(DBInitializerFromSQL< ALLOC >&& from,
                           const allocator_type&           alloc);

      /// virtual copy constructor
      virtual DBInitializerFromSQL< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual DBInitializerFromSQL< ALLOC >*
         clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~DBInitializerFromSQL();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      /** the new initializer points to the same SQL query as from, but
       * it reparses the result from scratch. */
      DBInitializerFromSQL< ALLOC >&
         operator=(const DBInitializerFromSQL< ALLOC >& from);

      /// move operator
      /** the new initializer points to the same SQL query as from, but
       * it reparses the result from scratch. */
      DBInitializerFromSQL< ALLOC >&
         operator=(DBInitializerFromSQL< ALLOC >&& from);

      /// @}


      protected:
      /// returns the names of the variables
      virtual std::vector< std::string, ALLOC< std::string > >
         _variableNames() final;

      /// returns the content of the current row using strings
      virtual const std::vector< std::string, ALLOC< std::string > >&
         _currentStringRow() final;

      /// indicates whether there is a next row to read (and point on it)
      virtual bool _nextRow() final;


#    ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      // the string specifying how to connect to the database
      std::string __connection_string;

      // the current query
      std::string __query;

      // the timeout for executing the queries
      long __timeout;

      // the names of the columns in the query result
      std::vector< std::string, ALLOC< std::string > > __var_names;

      // the nanodbc connection to the database
      nanodbc::connection __connection;

      // the parser used for parsing the query results
      NanodbcParser< ALLOC > __parser;

      /// perform a connection from a connection string
      void __connect(const std::string& connection_string, long timeout);


#    endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

// always include the template implementation
#    include <agrum/learning/database/DBInitializerFromSQL_tpl.h>

#  endif /* GUM_LEARNING_DB_INITILIALIZER_FROM_SQL_H */

#endif /* _ODBC */
