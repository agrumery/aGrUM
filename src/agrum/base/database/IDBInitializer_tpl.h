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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/
#pragma once


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
        case InputType::STRING : _fillDatabaseFromStrings_(database, retry_insertion); return;

        case InputType::DBCELL : _fillDatabaseFromDBCells_(database, retry_insertion); return;
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
