/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief The base class for initializing DatabaseTables and RawDatabaseTables
 * from files or sql databases
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// fills the rows of the database
    template < class DATABASE >
    INLINE void IDBInitializer::fillDatabase(DATABASE& database, const bool retry_insertion) {
      switch (_input_type_) {
        case InputType::STRING: _fillDatabaseFromStrings_(database, retry_insertion); return;

        case InputType::DBCELL: _fillDatabaseFromDBCells_(database, retry_insertion); return;
      }

      GUM_ERROR(NotImplementedYet,
                "fillDatabase has not been implemented yet for this "
                "type of IDBInitializerInputType");
    }


    /// fills the rows of the database using string inputs
    template < class DATABASE >
    void IDBInitializer::_fillDatabaseFromStrings_(DATABASE& database, const bool retry_insertion) {
      // if need be, try to reinsert the row that could not be inserted
      if (retry_insertion && _last_insertion_failed_) {
        database.insertRow(currentStringRow_());
        _last_insertion_failed_ = false;
      }

      // try to insert the next rows
      while (this->nextRow_()) {
        try {
          // read a new line in the input file and insert it into the database
          database.insertRow(currentStringRow_());
        } catch (...) {
          _last_insertion_failed_ = true;
          throw;
        }
      }
    }


    /// fills the rows of the database using DBCell inputs
    template < class DATABASE >
    void IDBInitializer::_fillDatabaseFromDBCells_(DATABASE& database, const bool retry_insertion) {
      // if need be, try to reinsert the row that could not be inserted
      if (retry_insertion && _last_insertion_failed_) {
        database.insertRow(currentDBCellRow_());
        _last_insertion_failed_ = false;
      }

      // try to insert the next rows
      while (this->nextRow_()) {
        try {
          // read a new line in the input file and insert it into the database
          database.insertRow(currentDBCellRow_());
        } catch (...) {
          _last_insertion_failed_ = true;
          throw;
        }
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
