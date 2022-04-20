/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief The common class for the tabular database tables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/tools/database/IDatabaseTable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// insert new rows at the end of the database
    void IDatabaseTableInsert4DBCell< true >::insertRows(
       const typename IDatabaseTableInsert4DBCell< true >::template DBVector<
          DBVector< std::string > >& new_rows) {
      for (const auto& new_row: new_rows)
        this->insertRow(new_row);
    }


    /// insert new rows at the end of the database
    void IDatabaseTableInsert4DBCell< false >::insertRows(
       const typename IDatabaseTableInsert4DBCell< false >::template DBVector<
          DBVector< std::string > >& new_rows) {
      for (const auto& new_row: new_rows)
        this->insertRow(new_row);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
