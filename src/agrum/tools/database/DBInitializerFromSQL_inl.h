/**
 *
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifdef _ODBC

#  include <clocale>

#  ifndef DOXYGEN_SHOULD_SKIP_THIS

#    include <agrum/tools/database/DBInitializerFromSQL.h>

namespace gum {

  namespace learning {

    /// returns the names of the variables
    INLINE std::vector< std::string > DBInitializerFromSQL::variableNames_() { return _var_names_; }

    /// returns the content of the current row using strings
    INLINE const std::vector< std::string >& DBInitializerFromSQL::currentStringRow_() {
      return _parser_.current();
    }

    /// indicates whether there is a next row to read (and point on it)
    INLINE bool DBInitializerFromSQL::nextRow_() { return _parser_.next(); }


  } /* namespace learning */

} /* namespace gum */

#  endif /* DOXYGEN_SHOULD_SKIP_THIS */

#endif   /* _ODBC */
