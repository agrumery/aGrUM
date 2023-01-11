/***************************************************************************
 *   Copyright (c) 2005-2023 by Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)  *
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

#ifdef _ODBC

#  ifndef DOXYGEN_SHOULD_SKIP_THIS

#    include <agrum/tools/database/nanodbcParser.h>

namespace gum {

  namespace learning {

    // return the current number line
    INLINE std::size_t NanodbcParser::nbLine() const {
      return _nb_line_ >= 1 ? _nb_line_ - 1 : std::size_t(0);
    }


    // returns the current parsed line.
    INLINE
    const std::vector< std::string >& NanodbcParser::current() const {
      if (!_data_.empty()) { return _data_; }

      GUM_ERROR(NullElement, "No parsed data")
    }


    /// returns the number of columns in the query result
    INLINE std::size_t NanodbcParser::nbColumns() const { return std::size_t(_result_.columns()); }


    /// returns the name of the ith column
    INLINE std::string NanodbcParser::columnName(const std::size_t i) const {
      return _result_.column_name(i);
    }


  }   // namespace learning

}   // namespace gum

#  endif /* DOXYGEN_SHOULD_SKIP_THIS */

#endif   // _ODBC
