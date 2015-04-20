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
/** @file
 * @brief The inlined implementation of DBCells
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    // ===========================================================================
    // Handlers
    // ===========================================================================

    /// attach a new handler to the database
    INLINE void DatabaseVectInRAM::Handler::__attachHandler() {
      if (__db) {
        __db->__list_of_handlers.push_back(this);
      }
    }

    /// detach a handler
    INLINE void DatabaseVectInRAM::Handler::__detachHandler() {
      if (__db) {
        std::vector<Handler *> &handlers = __db->__list_of_handlers;

        for (auto iter = handlers.rbegin(); iter != handlers.rend(); ++iter) {
          if (*iter == this) {
            *iter = handlers.back();
            handlers.pop_back();
          }
        }
      }
    }

    /// default constructor
    INLINE DatabaseVectInRAM::Handler::Handler(const DatabaseVectInRAM &db)
        : __db(&db), __row(&(db.content())), __end_index(__row->size()) {
      __attachHandler();
      GUM_CONSTRUCTOR(DatabaseVectInRAM::Handler);
    }

    /// copy constructor
    INLINE
    DatabaseVectInRAM::Handler::Handler(const DatabaseVectInRAM::Handler &h)
        : __db(h.__db), __row(h.__row), __index(h.__index),
          __begin_index(h.__begin_index), __end_index(h.__end_index) {
      __attachHandler();
      GUM_CONS_CPY(DatabaseVectInRAM::Handler);
    }

    /// move constructor
    INLINE
    DatabaseVectInRAM::Handler::Handler(DatabaseVectInRAM::Handler &&h)
        : __db(h.__db), __row(h.__row), __index(h.__index),
          __begin_index(h.__begin_index), __end_index(h.__end_index) {
      __attachHandler();
      GUM_CONS_MOV(DatabaseVectInRAM::Handler);
    }

    /// destructor
    INLINE DatabaseVectInRAM::Handler::~Handler() {
      __detachHandler();
      GUM_DESTRUCTOR(DatabaseVectInRAM::Handler);
    }

    /// copy operator
    INLINE
    DatabaseVectInRAM::Handler &DatabaseVectInRAM::Handler::
    operator=(const DatabaseVectInRAM::Handler &h) {
      if (__db != h.__db) {
        __detachHandler();
        __db = h.__db;
        __attachHandler();
      }

      __row = h.__row;
      __index = h.__index;
      __begin_index = h.__begin_index;
      __end_index = h.__end_index;
      return *this;
    }

    /// move operator
    INLINE
    DatabaseVectInRAM::Handler &DatabaseVectInRAM::Handler::
    operator=(DatabaseVectInRAM::Handler &&h) {
      if (__db != h.__db) {
        __detachHandler();
        __db = h.__db;
        __attachHandler();
      }

      __row = h.__row;
      __index = h.__index;
      __begin_index = h.__begin_index;
      __end_index = h.__end_index;
      return *this;
    }

    /// returns the number of rows managed by the handler
    INLINE unsigned long DatabaseVectInRAM::Handler::size() const noexcept {
      return __end_index - __begin_index;
    }

    /// return the number of rows of the whole database
    INLINE unsigned long DatabaseVectInRAM::Handler::DBSize() const noexcept {
      return __row->size();
    }

    /// returns the current row pointed to by the handler
    INLINE const DBRow &DatabaseVectInRAM::Handler::rowSafe() const {
      if (__index >= __end_index) {
        GUM_ERROR(OutOfBounds, "the handler has reached its end");
      }

      return __row->operator[](__index);
    }

    /// returns the current row pointed to by the handler
    INLINE DBRow &DatabaseVectInRAM::Handler::rowSafe() {
      if (__index >= __end_index) {
        GUM_ERROR(OutOfBounds, "the handler has reached its end");
      }

      return const_cast<std::vector<DBRow> *>(__row)->operator[](__index);
    }

    /// returns the current row pointed to by the handler
    INLINE const DBRow &DatabaseVectInRAM::Handler::row() const {
      return __row->operator[](__index);
    }

    /// returns the current row pointed to by the handler
    INLINE DBRow &DatabaseVectInRAM::Handler::row() {
      return const_cast<std::vector<DBRow> *>(__row)->operator[](__index);
    }

    /// makes the handler point to the next row
    INLINE void DatabaseVectInRAM::Handler::nextRow() noexcept { ++__index; }

    /// number of row
    INLINE Idx DatabaseVectInRAM::Handler::numRow() const noexcept {
      return (__index >= __begin_index) ? __index + 1 - __begin_index : 0;
    }

    /// indicates whether the handler has reached its end or not
    INLINE bool DatabaseVectInRAM::Handler::hasRows() const noexcept {
      return (__index < __end_index);
    }

    /// puts the handler to the beginning of the database area it handles
    INLINE void DatabaseVectInRAM::Handler::reset() noexcept {
      __index = __begin_index;
    }

    /// sets the area in the database the handler will handle
    INLINE void DatabaseVectInRAM::Handler::setRange(unsigned long begin,
                                                     unsigned long end) noexcept {
      if (begin > end)
        std::swap(begin, end);

      // check that the end belongs to the database, else raise an exception
      if (end > __row->size()) {
        GUM_ERROR(SizeError, "the database has fewer rows than the upper range "
                             "specified to the handler");
      }

      __begin_index = begin;
      __end_index = end;
      __index = begin;
    }

    /// returns the current range of the handler
    INLINE std::pair<unsigned long, unsigned long>
    DatabaseVectInRAM::Handler::range() const noexcept {
      return std::pair<unsigned long, unsigned long>(__begin_index, __end_index);
    }

    /// returns the names of the variables
    INLINE const std::vector<std::string> &
    DatabaseVectInRAM::Handler::variableNames() const noexcept {
      return __db->variableNames();
    }

    /// returns the number of variables (columns) of the database
    INLINE unsigned int DatabaseVectInRAM::Handler::nbVariables() const noexcept {
      return __db->variableNames().size();
    }

    // ===========================================================================
    // Database VectInRAMs
    // ===========================================================================

    /// default constructor
    INLINE DatabaseVectInRAM::DatabaseVectInRAM() {
      GUM_CONSTRUCTOR(DatabaseVectInRAM);
    }

    /// copy constructor
    INLINE DatabaseVectInRAM::DatabaseVectInRAM(const DatabaseVectInRAM &from)
        : __data(from.__data), __variable_names(from.__variable_names) {
      GUM_CONS_CPY(DatabaseVectInRAM);
    }

    /// move constructor
    INLINE DatabaseVectInRAM::DatabaseVectInRAM(DatabaseVectInRAM &&from)
        : __data(std::move(from.__data)),
          __variable_names(std::move(from.__variable_names)) {
      GUM_CONS_MOV(DatabaseVectInRAM);
    }

    /// destructor
    INLINE DatabaseVectInRAM::~DatabaseVectInRAM() {
      // indicate to all the handlers that we are destructing the database
      for (auto handler : __list_of_handlers) {
        handler->__db = nullptr;
        handler->__row = nullptr;
        handler->__end_index = 0;
        handler->__index = 0;
      }

      GUM_DESTRUCTOR(DatabaseVectInRAM);
    }

    /// copy operator
    INLINE DatabaseVectInRAM &DatabaseVectInRAM::
    operator=(const DatabaseVectInRAM &from) {
      if (this != &from) {
        // invalidate the current handlers
        for (auto handler : __list_of_handlers) {
          handler->__db = nullptr;
          handler->__row = nullptr;
          handler->__end_index = 0;
          handler->__index = 0;
        }

        __data = from.__data;
        __variable_names = from.__variable_names;
      }

      return *this;
    }

    /// move constructor
    INLINE DatabaseVectInRAM &DatabaseVectInRAM::
    operator=(DatabaseVectInRAM &&from) {
      if (this != &from) {
        // invalidate the current handlers
        for (auto handler : __list_of_handlers) {
          handler->__db = nullptr;
          handler->__row = nullptr;
          handler->__end_index = 0;
          handler->__index = 0;
        }

        __data = std::move(from.__data);
        __variable_names = std::move(from.__variable_names);
      }

      return *this;
    }

    /// returns the content of the database
    INLINE const std::vector<DBRow> &DatabaseVectInRAM::content() const noexcept {
      return __data;
    }

    /// returns the content of the database
    INLINE std::vector<DBRow> &DatabaseVectInRAM::_content() noexcept {
      return __data;
    }

    /// returns the variable names for all the columns
    INLINE const std::vector<std::string> &DatabaseVectInRAM::variableNames() const
        noexcept {
      return __variable_names;
    }

    /// returns the variable names for all the columns
    INLINE std::vector<std::string> &DatabaseVectInRAM::_variableNames() noexcept {
      return __variable_names;
    }

    /// sets the names of the variables
    INLINE void
    DatabaseVectInRAM::setVariableNames(const std::vector<std::string> &names) {
      if (__data.empty() || (names.size() == __data[0].size())) {
        __variable_names = names;
      } else {
        GUM_ERROR(SizeError, "the number of variable's names does not "
                             "correspond to the number of columns of the database");
      }
    }

    /// returns the number of variables (columns) of the database
    INLINE unsigned int DatabaseVectInRAM::nbVariables() const noexcept {
      if (__data.empty())
        return __variable_names.size();
      else
        return __data[0].size();
    }

    /// returns a new handler on the database
    INLINE DatabaseVectInRAM::Handler DatabaseVectInRAM::handler() const {
      return Handler(*this);
    }

    /// update the handlers when the size of the database changes
    INLINE void DatabaseVectInRAM::__updateHandlers(unsigned long new_size) {
      unsigned int db_size = __data.size();

      for (auto handler : __list_of_handlers) {
        if ((handler->__end_index == db_size) || (handler->__end_index > new_size)) {
          handler->__end_index = new_size;
          // there is no need to update the index because we always check
          // that the index is less than end_index when trying to access the rows
        }
      }
    }

    /// insert a new DBRow at the end of the database
    INLINE void DatabaseVectInRAM::insertDBRow(const DBRow &new_row) {
      // check that the size of the row is the same as the rest of the database
      const unsigned long db_size = __data.size();

      if (db_size && (new_row.size() != __data[0].size())) {
        GUM_ERROR(SizeError, "the new row has not the same size as the "
                             "rest of the database");
      }

      const unsigned int nb_cols = __variable_names.size();

      if (nb_cols && (new_row.size() != nb_cols)) {
        GUM_ERROR(SizeError, "the new row has not the same size as the "
                             "rest of the database");
      }

      __updateHandlers(db_size + 1);
      __data.push_back(new_row);
    }

    /// insert a new DBRow at the end of the database
    INLINE void DatabaseVectInRAM::insertDBRow(DBRow &&new_row) {
      // check that the size of the row is the same as the rest of the database
      const unsigned long db_size = __data.size();

      if (db_size && (new_row.size() != __data[0].size())) {
        GUM_ERROR(SizeError, "the new row has not the same size as the "
                             "rest of the database");
      }

      const unsigned int nb_cols = __variable_names.size();

      if (nb_cols && (new_row.size() != nb_cols)) {
        GUM_ERROR(SizeError, "the new row has not the same size as the "
                             "rest of the database");
      }

      __updateHandlers(db_size + 1);
      __data.push_back(std::move(new_row));
    }

    /// insert a set of new DBRow at the end of the database
    INLINE void DatabaseVectInRAM::insertDBRows(const std::vector<DBRow> &new_rows) {
      if (new_rows.empty())
        return;

      // check that all the rows have the same size
      const unsigned int new_size = new_rows[0].size();

      for (const auto &row : new_rows) {
        if (row.size() != new_size) {
          GUM_ERROR(SizeError, "all the new rows do not have the same "
                               "nunber of columns");
        }
      }

      // check that the sizes of the new rows are the same as the rest of
      // the database
      const unsigned long db_size = __data.size();
      const unsigned int nb_cols = __variable_names.size();

      if (db_size && (__data[0].size() != new_size)) {
        GUM_ERROR(SizeError, "the new rows have not the same size as the "
                             "rest of the database");
      }

      if (nb_cols && (new_size != nb_cols)) {
        GUM_ERROR(SizeError, "the new rows have not the same size as the "
                             "number of columns in the database");
      }

      __updateHandlers(db_size + new_rows.size());

      for (const auto &row : new_rows) {
        __data.push_back(row);
      }
    }

    /// insert a set of new DBRow at the end of the database
    INLINE void DatabaseVectInRAM::insertDBRows(std::vector<DBRow> &&new_rows) {
      if (new_rows.empty())
        return;

      // check that all the rows have the same size
      const unsigned int new_size = new_rows[0].size();

      for (const auto &row : new_rows) {
        if (row.size() != new_size) {
          GUM_ERROR(SizeError, "all the new rows do not have the same "
                               "nunber of columns");
        }
      }

      // check that the sizes of the new rows are the same as the rest of
      // the database
      const unsigned long db_size = __data.size();
      const unsigned int nb_cols = __variable_names.size();

      if (db_size && (__data[0].size() != new_size)) {
        GUM_ERROR(SizeError, "the new rows have not the same size as the "
                             "rest of the database");
      }

      if (nb_cols && (new_size != nb_cols)) {
        GUM_ERROR(SizeError, "the new rows have not the same size as the "
                             "number of columns in the database");
      }

      __updateHandlers(db_size + new_rows.size());

      for (auto row : new_rows) {
        __data.push_back(std::move(row));
      }
    }

    /// erase a given row
    INLINE void DatabaseVectInRAM::eraseDBRow(unsigned long index) {
      unsigned long db_size = __data.size();

      if (index < db_size) {
        __updateHandlers(db_size - 1);
        __data.erase(__data.begin() + index);
      }
    }

    /// erase the last row
    INLINE void DatabaseVectInRAM::eraseLastDBRow() {
      unsigned long db_size = __data.size();

      if (db_size) {
        __updateHandlers(db_size - 1);
        __data.pop_back();
      }
    }

    /// erase the first row
    INLINE void DatabaseVectInRAM::eraseFirstDBRow() {
      unsigned long db_size = __data.size();

      if (db_size) {
        __updateHandlers(db_size - 1);
        __data.erase(__data.begin());
      }
    }

    /// erase all the rows
    INLINE void DatabaseVectInRAM::eraseAllDBRows() {
      __updateHandlers(0);
      __data.clear();
    }

    /// erase the k first rows
    INLINE void DatabaseVectInRAM::eraseFirstDBRows(unsigned long nb_rows) {
      unsigned long db_size = __data.size();

      if (nb_rows >= db_size) {
        eraseAllDBRows();
      } else {
        __updateHandlers(db_size - nb_rows);
        __data.erase(__data.begin(), __data.begin() + nb_rows);
      }
    }

    /// erase the k last rows
    INLINE void DatabaseVectInRAM::eraseLastDBRows(unsigned long nb_rows) {
      unsigned long db_size = __data.size();

      if (nb_rows >= db_size) {
        eraseAllDBRows();
      } else {
        __updateHandlers(db_size - nb_rows);
        __data.erase(__data.begin() + (db_size - nb_rows), __data.begin() + db_size);
      }
    }

    /// erase the rows from the debth to the endth (not included)
    INLINE void DatabaseVectInRAM::eraseDBRows(unsigned long deb,
                                               unsigned long end) {
      if (deb > end)
        std::swap(deb, end);

      unsigned long db_size = __data.size();

      if (end >= db_size) {
        if (deb >= db_size) {
          return;
        } else {
          eraseLastDBRows(db_size - deb);
        }
      } else {
        __updateHandlers(db_size - (end - deb));
        __data.erase(__data.begin() + deb, __data.begin() + end);
      }
    }

    /// erase the content of the database, including the names of the variables
    INLINE void DatabaseVectInRAM::clear() {
      __updateHandlers(0);
      __data.clear();
      __variable_names.clear();
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
