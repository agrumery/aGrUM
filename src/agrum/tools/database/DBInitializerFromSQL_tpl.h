/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
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
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifdef ODBC_
#  include <clocale>

#  ifndef DOXYGEN_SHOULD_SKIP_THIS

#    include <agrum/tools/database/DBInitializerFromSQL.h>


namespace gum {

  namespace learning {


    /// perform a connection from a connection string
    template < template < typename > class ALLOC >
    void DBInitializerFromSQL< ALLOC >:: _connect_(
       const std::string& connection_string,
       long               timeout) {
      // analyze the connection string: either this is a user-defined connection
      // string or this is an aGrUM-constructed one derived from a datasource,
      // a login and a password
      bool agrum_connection = (connection_string.size() > 4)
                           && (connection_string.compare(0, 4, "gum ") == 0);

      // perform the connection to the database
      if (!agrum_connection) {
         _connection_.connect(connection_string, timeout);
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
        std::string dataSource
           = connection_string.substr(deb_index, end_index - deb_index);

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
        std::string login
           = connection_string.substr(deb_index, end_index - deb_index);

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
        std::string password
           = connection_string.substr(deb_index, end_index - deb_index);

         _connection_.connect(dataSource, login, password, timeout);
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
         _connection_string_(connection_string),  _query_(query),  _timeout_(timeout),
         _var_names_(alloc),  _parser_(alloc) {
      // save the current locale because the connection to the database
      // will change it
      const std::string current_locale = std::setlocale(LC_NUMERIC, NULL);

      // perform the connection
       _connect_(connection_string, timeout);

      // restore the locale
      std::setlocale(LC_NUMERIC, current_locale.c_str());

      // ask the parser to execute the query
       _parser_.useNewQuery( _connection_,  _query_);

      // store the names of the columns into the intializer
      const std::size_t nb_cols = std::size_t( _parser_.nbColumns());
      for (std::size_t i = 0; i < nb_cols; ++i) {
         _var_names_.push_back( _parser_.columnName(i));
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
        DBInitializerFromSQL< ALLOC >(from. _connection_string_,
                                      from. _query_,
                                      from. _timeout_,
                                      alloc) {}


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
        DBInitializerFromSQL< ALLOC >(from. _connection_string_,
                                      from. _query_,
                                      from. _timeout_,
                                      alloc) {}

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
    DBInitializerFromSQL< ALLOC >& DBInitializerFromSQL< ALLOC >::operator=(
       const DBInitializerFromSQL< ALLOC >& from) {
      if (this != &from) {
        IDBInitializer< ALLOC >::operator=(from);
        // check if the connection parameters have changed
        const bool connexion_changed
           = ( _connection_string_ != from. _connection_string_);

        // save the new connection parameters
         _connection_string_ = from. _connection_string_;
         _query_             = from. _query_;
         _timeout_           = from. _timeout_;

        // recreate the connection if needed
        if (connexion_changed) {
          if ( _connection_.connected())  _connection_.disconnect();
           _connect_( _connection_string_,  _timeout_);
        }

        // initiate the SQL parser
         _parser_.useNewQuery( _connection_,  _query_);

        // store the names of the columns into the intializer
         _var_names_.clear();
        const std::size_t nb_cols = std::size_t( _parser_.nbColumns());
        for (std::size_t i = 0; i < nb_cols; ++i) {
           _var_names_.push_back( _parser_.columnName(i));
        }
      }

      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    DBInitializerFromSQL< ALLOC >& DBInitializerFromSQL< ALLOC >::operator=(
       DBInitializerFromSQL< ALLOC >&& from) {
      return operator=(from);
    }


    /// returns the names of the variables
    template < template < typename > class ALLOC >
    INLINE std::vector< std::string, ALLOC< std::string > >
           DBInitializerFromSQL< ALLOC >::variableNames_() {
      return  _var_names_;
    }


    /// returns the content of the current row using strings
    template < template < typename > class ALLOC >
    INLINE const std::vector< std::string, ALLOC< std::string > >&
                 DBInitializerFromSQL< ALLOC >::currentStringRow_() {
      return  _parser_.current();
    }


    /// indicates whether there is a next row to read (and point on it)
    template < template < typename > class ALLOC >
    INLINE bool DBInitializerFromSQL< ALLOC >::nextRow_() {
      return  _parser_.next();
    }


  } /* namespace learning */

} /* namespace gum */

#  endif /* DOXYGEN_SHOULD_SKIP_THIS */

#endif /* ODBC_ */
