
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
#  include <clocale>

#  ifndef DOXYGEN_SHOULD_SKIP_THIS

#    include <agrum/learning/database/DBInitializerFromSQL.h>


namespace gum {

  namespace learning {


    /// perform a connection from a connection string
    template < template < typename > class ALLOC >
    void DBInitializerFromSQL< ALLOC >::__connect(
       const std::string& connection_string, long timeout) {
      // analyze the connection string: either this is a user-defined connection
      // string or this is an aGrUM-constructed one derived from a datasource,
      // a login and a password
      bool agrum_connection = (connection_string.size() > 4)
                              && (connection_string.compare(0, 4, "gum ") == 0);

      // perform the connection to the database
      if (!agrum_connection) {
        __connection.connect(connection_string, timeout);
      } else {
        std::size_t       deb_index, end_index;
        const std::string delimiter = "|";

        deb_index = connection_string.find(delimiter, 0);
        if (deb_index == std::string::npos)
          GUM_ERROR(DatabaseError,
                    "could not determine the datasource from string "
                       << connection_string);
        deb_index += std::size_t(1);
        end_index = connection_string.find(delimiter, deb_index);
        if (end_index == std::string::npos)
          GUM_ERROR(DatabaseError,
                    "could not determine the datasource from string "
                       << connection_string);
        std::string dataSource =
           connection_string.substr(deb_index, end_index - deb_index);

        deb_index = connection_string.find(delimiter, end_index + std::size_t(1));
        if (deb_index == std::string::npos)
          GUM_ERROR(DatabaseError,
                    "could not determine the database login from string "
                       << connection_string);
        deb_index += std::size_t(1);
        end_index = connection_string.find(delimiter, deb_index);
        if (end_index == std::string::npos)
          GUM_ERROR(DatabaseError,
                    "could not determine the database login from string "
                       << connection_string);
        std::string login =
           connection_string.substr(deb_index, end_index - deb_index);

        deb_index = connection_string.find(delimiter, end_index + std::size_t(1));
        if (deb_index == std::string::npos)
          GUM_ERROR(DatabaseError,
                    "could not determine the database password from string "
                       << connection_string);
        deb_index += std::size_t(1);
        end_index = connection_string.find(delimiter, deb_index);
        if (end_index == std::string::npos)
          GUM_ERROR(DatabaseError,
                    "could not determine the database password from string "
                       << connection_string);
        std::string password =
           connection_string.substr(deb_index, end_index - deb_index);

        __connection.connect(dataSource, login, password, timeout);
      }
    }


    /// default constructor
    template < template < typename > class ALLOC >
    DBInitializerFromSQL< ALLOC >::DBInitializerFromSQL(
       const std::string& connection_string,
       const std::string& query,
       long               timeout,
       const typename DBInitializerFromSQL< ALLOC >::allocator_type& alloc) :
        IDBInitializer< ALLOC >(IDBInitializer< ALLOC >::InputType::STRING, alloc),
        __connection_string(connection_string), __query(query), __timeout(timeout),
        __var_names(alloc), __parser(alloc) {
      // save the current locale because the connection to the database
      // will change it
      const std::string current_locale = std::setlocale(LC_NUMERIC, NULL);

      // perform the connection
      __connect(connection_string, timeout);

      // restore the locale
      std::setlocale(LC_NUMERIC, current_locale.c_str());

      // ask the parser to execute the query
      __parser.useNewQuery(__connection, __query);

      // store the names of the columns into the intializer
      const std::size_t nb_cols = std::size_t(__parser.nbColumns());
      for (std::size_t i = 0; i < nb_cols; ++i) {
        __var_names.push_back(__parser.columnName(i));
      }

      GUM_CONSTRUCTOR(DBInitializerFromSQL);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    DBInitializerFromSQL< ALLOC >::DBInitializerFromSQL(
       const std::string&                                            dataSource,
       const std::string&                                            login,
       const std::string&                                            password,
       const std::string&                                            query,
       long                                                          timeout,
       const typename DBInitializerFromSQL< ALLOC >::allocator_type& alloc) :
        DBInitializerFromSQL< ALLOC >("gum datasource=|" + dataSource
                                         + "|; login=|" + login + "|; password=|"
                                         + password + "|",
                                      query,
                                      timeout,
                                      alloc) {}


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBInitializerFromSQL< ALLOC >::DBInitializerFromSQL(
       const DBInitializerFromSQL< ALLOC >&                          from,
       const typename DBInitializerFromSQL< ALLOC >::allocator_type& alloc) :
        DBInitializerFromSQL< ALLOC >(
           from.__connection_string, from.__query, from.__timeout, alloc) {}


    /// copy constructor
    template < template < typename > class ALLOC >
    DBInitializerFromSQL< ALLOC >::DBInitializerFromSQL(
       const DBInitializerFromSQL< ALLOC >& from) :
        DBInitializerFromSQL< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    DBInitializerFromSQL< ALLOC >::DBInitializerFromSQL(
       DBInitializerFromSQL< ALLOC >&&                               from,
       const typename DBInitializerFromSQL< ALLOC >::allocator_type& alloc) :
        DBInitializerFromSQL< ALLOC >(
           from.__connection_string, from.__query, from.__timeout, alloc) {}

    /// move constructor
    template < template < typename > class ALLOC >
    DBInitializerFromSQL< ALLOC >::DBInitializerFromSQL(
       DBInitializerFromSQL< ALLOC >&& from) :
        DBInitializerFromSQL< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBInitializerFromSQL< ALLOC >* DBInitializerFromSQL< ALLOC >::clone(
       const typename DBInitializerFromSQL< ALLOC >::allocator_type& alloc) const {
      ALLOC< DBInitializerFromSQL< ALLOC > > allocator(alloc);
      DBInitializerFromSQL< ALLOC >* new_initializer = allocator.allocate(1);
      try {
        allocator.construct(new_initializer, *this, alloc);
      } catch (...) {
        allocator.deallocate(new_initializer, 1);
        throw;
      }

      return new_initializer;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    DBInitializerFromSQL< ALLOC >* DBInitializerFromSQL< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    DBInitializerFromSQL< ALLOC >::~DBInitializerFromSQL() {
      GUM_DESTRUCTOR(DBInitializerFromSQL);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    DBInitializerFromSQL< ALLOC >& DBInitializerFromSQL< ALLOC >::
                                   operator=(const DBInitializerFromSQL< ALLOC >& from) {
      if (this != &from) {
        IDBInitializer< ALLOC >::operator=(from);
        // check if the connection parameters have changed
        const bool connexion_changed =
           (__connection_string != from.__connection_string);

        // save the new connection parameters
        __connection_string = from.__connection_string;
        __query = from.__query;
        __timeout = from.__timeout;

        // recreate the connection if needed
        if (connexion_changed) {
          if (__connection.connected()) __connection.disconnect();
          __connect(__connection_string, __timeout);
        }

        // initiate the SQL parser
        __parser.useNewQuery(__connection, __query);

        // store the names of the columns into the intializer
        __var_names.clear();
        const std::size_t nb_cols = std::size_t(__parser.nbColumns());
        for (std::size_t i = 0; i < nb_cols; ++i) {
          __var_names.push_back(__parser.columnName(i));
        }
      }

      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    DBInitializerFromSQL< ALLOC >& DBInitializerFromSQL< ALLOC >::
                                   operator=(DBInitializerFromSQL< ALLOC >&& from) {
      return operator=(from);
    }


    /// returns the names of the variables
    template < template < typename > class ALLOC >
    INLINE std::vector< std::string, ALLOC< std::string > >
           DBInitializerFromSQL< ALLOC >::_variableNames() {
      return __var_names;
    }


    /// returns the content of the current row using strings
    template < template < typename > class ALLOC >
    INLINE const std::vector< std::string, ALLOC< std::string > >&
                 DBInitializerFromSQL< ALLOC >::_currentStringRow() {
      return __parser.current();
    }


    /// indicates whether there is a next row to read (and point on it)
    template < template < typename > class ALLOC >
    INLINE bool DBInitializerFromSQL< ALLOC >::_nextRow() {
      return __parser.next();
    }


  } /* namespace learning */

} /* namespace gum */

#  endif /* DOXYGEN_SHOULD_SKIP_THIS */

#endif /* _ODBC */
