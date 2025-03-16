/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





/** @file
 * @brief The class for initializing DatabaseTable and RawDatabaseTable
 * instances from SQL databases
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/database/DBInitializerFromSQL.h>

#ifdef _ODBC

#  ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#    ifdef GUM_NO_INLINE
#      include <agrum/base/database/DBInitializerFromSQL_inl.h>
#    endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// perform a connection from a connection string
    void DBInitializerFromSQL::_connect_(const std::string& connection_string, long timeout) {
      // analyze the connection string: either this is a user-defined connection
      // string or this is an aGrUM-constructed one derived from a datasource,
      // a login and a password
      bool agrum_connection
          = (connection_string.size() > 4) && (connection_string.compare(0, 4, "gum ") == 0);

      // perform the connection to the database
      if (!agrum_connection) {
        _connection_.connect(connection_string, timeout);
      } else {
        std::size_t       deb_index, end_index;
        const std::string delimiter = "|";

        deb_index = connection_string.find(delimiter, 0);
        if (deb_index == std::string::npos)
          GUM_ERROR(DatabaseError,
                    "could not determine the datasource from string " << connection_string);
        deb_index += std::size_t(1);
        end_index = connection_string.find(delimiter, deb_index);
        if (end_index == std::string::npos)
          GUM_ERROR(DatabaseError,
                    "could not determine the datasource from string " << connection_string);
        std::string dataSource = connection_string.substr(deb_index, end_index - deb_index);

        deb_index = connection_string.find(delimiter, end_index + std::size_t(1));
        if (deb_index == std::string::npos)
          GUM_ERROR(DatabaseError,
                    "could not determine the database login from string " << connection_string);
        deb_index += std::size_t(1);
        end_index = connection_string.find(delimiter, deb_index);
        if (end_index == std::string::npos)
          GUM_ERROR(DatabaseError,
                    "could not determine the database login from string " << connection_string);
        std::string login = connection_string.substr(deb_index, end_index - deb_index);

        deb_index = connection_string.find(delimiter, end_index + std::size_t(1));
        if (deb_index == std::string::npos)
          GUM_ERROR(DatabaseError,
                    "could not determine the database password from string " << connection_string);
        deb_index += std::size_t(1);
        end_index = connection_string.find(delimiter, deb_index);
        if (end_index == std::string::npos)
          GUM_ERROR(DatabaseError,
                    "could not determine the database password from string " << connection_string);
        std::string password = connection_string.substr(deb_index, end_index - deb_index);

        _connection_.connect(dataSource, login, password, timeout);
      }
    }

    /// default constructor
    DBInitializerFromSQL::DBInitializerFromSQL(const std::string& connection_string,
                                               const std::string& query,
                                               long               timeout) :
        IDBInitializer(IDBInitializer::InputType::STRING), _connection_string_(connection_string),
        _query_(query), _timeout_(timeout) {
      // save the current locale because the connection to the database
      // will change it
      const std::string current_locale = std::setlocale(LC_NUMERIC, NULL);

      // perform the connection
      _connect_(connection_string, timeout);

      // restore the locale
      std::setlocale(LC_NUMERIC, current_locale.c_str());

      // ask the parser to execute the query
      _parser_.useNewQuery(_connection_, _query_);

      // store the names of the columns into the intializer
      const std::size_t nb_cols = std::size_t(_parser_.nbColumns());
      for (std::size_t i = 0; i < nb_cols; ++i) {
        _var_names_.push_back(_parser_.columnName(i));
      }

      GUM_CONSTRUCTOR(DBInitializerFromSQL);
    }

    /// default constructor
    DBInitializerFromSQL::DBInitializerFromSQL(const std::string& dataSource,
                                               const std::string& login,
                                               const std::string& password,
                                               const std::string& query,
                                               long               timeout) :
        DBInitializerFromSQL("gum datasource=|" + dataSource + "|; login=|" + login
                                 + "|; password=|" + password + "|",
                             query,
                             timeout) {}

    /// copy constructor
    DBInitializerFromSQL::DBInitializerFromSQL(const DBInitializerFromSQL& from) :
        DBInitializerFromSQL(from._connection_string_, from._query_, from._timeout_) {}

    /// move constructor
    DBInitializerFromSQL::DBInitializerFromSQL(DBInitializerFromSQL&& from) :
        DBInitializerFromSQL(from._connection_string_, from._query_, from._timeout_) {}

    /// virtual copy constructor
    DBInitializerFromSQL* DBInitializerFromSQL::clone() const {
      return new DBInitializerFromSQL(*this);
    }

    /// destructor
    DBInitializerFromSQL::~DBInitializerFromSQL() { GUM_DESTRUCTOR(DBInitializerFromSQL); }

    /// copy operator
    DBInitializerFromSQL& DBInitializerFromSQL::operator=(const DBInitializerFromSQL& from) {
      if (this != &from) {
        IDBInitializer::operator=(from);
        // check if the connection parameters have changed
        const bool connexion_changed = (_connection_string_ != from._connection_string_);

        // save the new connection parameters
        _connection_string_ = from._connection_string_;
        _query_             = from._query_;
        _timeout_           = from._timeout_;

        // recreate the connection if needed
        if (connexion_changed) {
          if (_connection_.connected()) _connection_.disconnect();
          _connect_(_connection_string_, _timeout_);
        }

        // initiate the SQL parser
        _parser_.useNewQuery(_connection_, _query_);

        // store the names of the columns into the intializer
        _var_names_.clear();
        const std::size_t nb_cols = std::size_t(_parser_.nbColumns());
        for (std::size_t i = 0; i < nb_cols; ++i) {
          _var_names_.push_back(_parser_.columnName(i));
        }
      }

      return *this;
    }

    /// move operator
    DBInitializerFromSQL& DBInitializerFromSQL::operator=(DBInitializerFromSQL&& from) {
      return operator=(from);
    }


  } /* namespace learning */

} /* namespace gum */

#  endif /* DOXYGEN_SHOULD_SKIP_THIS */

#endif   // _ODBC
