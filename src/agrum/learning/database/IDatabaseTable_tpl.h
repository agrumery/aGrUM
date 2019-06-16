
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
 * @brief The implementation of the common class for tabular databases
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <agrum/learning/database/IDatabaseTable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    // ===========================================================================
    // Unsafe handlers
    // ===========================================================================

    // default constructor
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE IDatabaseTable< T_DATA, ALLOC >::Handler::Handler(
       const IDatabaseTable< T_DATA, ALLOC >& db) :
        DBHandler< T_DATA, ALLOC >(),
        __db(&db), __row(&(db.content())),
        __end_index(std::size_t(__row->size())) {
      GUM_CONSTRUCTOR(IDatabaseTable::Handler);
    }


    // copy constructor
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE IDatabaseTable< T_DATA, ALLOC >::Handler::Handler(
       const typename IDatabaseTable< T_DATA, ALLOC >::Handler& h) :
        DBHandler< T_DATA, ALLOC >(),
        __db(h.__db), __row(h.__row), __index(h.__index),
        __begin_index(h.__begin_index), __end_index(h.__end_index) {
      GUM_CONS_CPY(IDatabaseTable::Handler);
    }


    // move constructor
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE IDatabaseTable< T_DATA, ALLOC >::Handler::Handler(
       typename IDatabaseTable< T_DATA, ALLOC >::Handler&& h) :
        DBHandler< T_DATA, ALLOC >(),
        __db(h.__db), __row(h.__row), __index(h.__index),
        __begin_index(h.__begin_index), __end_index(h.__end_index) {
      GUM_CONS_MOV(IDatabaseTable::Handler);
    }


    // destructor
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE IDatabaseTable< T_DATA, ALLOC >::Handler::~Handler() {
      GUM_DESTRUCTOR(IDatabaseTable::Handler);
    }


    // copy operator
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::Handler&
          IDatabaseTable< T_DATA, ALLOC >::Handler::
          operator=(const typename IDatabaseTable< T_DATA, ALLOC >::Handler& h) {
      __db = h.__db;
      __row = h.__row;
      __index = h.__index;
      __begin_index = h.__begin_index;
      __end_index = h.__end_index;
      return *this;
    }


    // move operator
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::Handler&
          IDatabaseTable< T_DATA, ALLOC >::Handler::
          operator=(typename IDatabaseTable< T_DATA, ALLOC >::Handler&& h) {
      __db = h.__db;
      __row = h.__row;
      __index = h.__index;
      __begin_index = h.__begin_index;
      __end_index = h.__end_index;
      return *this;
    }


    // returns the current row pointed to by the handler
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::Handler::const_reference
       IDatabaseTable< T_DATA, ALLOC >::Handler::operator*() const {
      return __row->operator[](__index);
    }


    // Dereferences the value pointed to by the handler (unsafe version)
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::Handler::const_pointer
       IDatabaseTable< T_DATA, ALLOC >::Handler::operator->() const {
      return &(__row->operator[](__index));
    }


    // makes the handler point to the next row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::Handler&
       IDatabaseTable< T_DATA, ALLOC >::Handler::operator++() {
      ++__index;
      return *this;
    }


    // makes the handler point to the previous row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::Handler&
       IDatabaseTable< T_DATA, ALLOC >::Handler::operator--() {
      if (__index > __begin_index) --__index;
      return *this;
    }


    //  moves the handler by i rows
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::Handler&
       IDatabaseTable< T_DATA, ALLOC >::Handler::operator+=(const std::size_t i) {
      __index += i;
      return *this;
    }


    //  moves back the handler by i rows
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::Handler&
       IDatabaseTable< T_DATA, ALLOC >::Handler::operator-=(const std::size_t i) {
      if (__index >= __begin_index + i)
        __index -= i;
      else
        __index = __begin_index;
      return *this;
    }


    // checks whether two handlers point on the same row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE bool IDatabaseTable< T_DATA, ALLOC >::Handler::
                operator==(const Handler& handler) const {
      return __index == handler.__index;
    }


    // checks whether two handlers point to different rows
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE bool IDatabaseTable< T_DATA, ALLOC >::Handler::
                operator!=(const Handler& handler) const {
      return __index != handler.__index;
    }


    // returns the number of rows managed by the handler
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE std::size_t IDatabaseTable< T_DATA, ALLOC >::Handler::size() const {
      return __end_index - __begin_index;
    }


    // return the number of rows of the whole database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE std::size_t IDatabaseTable< T_DATA, ALLOC >::Handler::DBSize() const {
      if (__row != nullptr)
        return __row->size();
      else
        return std::size_t(0);
    }


    // returns the current row pointed to by the handler
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::Handler::const_reference
       IDatabaseTable< T_DATA, ALLOC >::Handler::rowSafe() const {
      if (__index >= __end_index) {
        GUM_ERROR(OutOfBounds, "the handler has reached its end");
      }

      return __row->operator[](__index);
    }


    // returns the current row pointed to by the handler
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::Handler::reference
       IDatabaseTable< T_DATA, ALLOC >::Handler::rowSafe() {
      if (__index >= __end_index) {
        GUM_ERROR(OutOfBounds, "the handler has reached its end");
      }

      return const_cast< Matrix< T_DATA >* >(__row)->operator[](__index);
    }


    // returns the current row pointed to by the handler (unsafe version)
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::Handler::const_reference
       IDatabaseTable< T_DATA, ALLOC >::Handler::row() const {
      return __row->operator[](__index);
    }


    // returns the current row pointed to by the handler (unsafe version)
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::Handler::reference
       IDatabaseTable< T_DATA, ALLOC >::Handler::row() {
      return const_cast< Matrix< T_DATA >* >(__row)->operator[](__index);
    }


    // makes the handler point to the next row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void IDatabaseTable< T_DATA, ALLOC >::Handler::nextRow() {
      ++__index;
    }


    // returns the number of the current row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE std::size_t IDatabaseTable< T_DATA, ALLOC >::Handler::numRow() const {
      return (__index >= __begin_index) ? __index - __begin_index : 0;
    }

    // indicates whether the handler has reached its end or not
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE bool IDatabaseTable< T_DATA, ALLOC >::Handler::hasRows() const {
      return (__index < __end_index);
    }

    // puts the handler to the beginning of the database area it handles
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void IDatabaseTable< T_DATA, ALLOC >::Handler::reset() {
      __index = __begin_index;
    }


    // returns a new handler that points to the beginning of the
    // database area of the current handler */
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::Handler
       IDatabaseTable< T_DATA, ALLOC >::Handler::begin() const {
      Handler handler(*this);
      handler.reset();
      return handler;
    }


    // returns a new handler that points to the end of the
    // database area of the current handler */
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::Handler
       IDatabaseTable< T_DATA, ALLOC >::Handler::end() const {
      Handler handler(*this);
      handler.__index = __end_index;
      return handler;
    }


    // sets the area in the database the handler will handle
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void
       IDatabaseTable< T_DATA, ALLOC >::Handler::setRange(std::size_t begin,
                                                          std::size_t end) {
      if (begin > end) std::swap(begin, end);

      // check that the end belongs to the database, else raise an exception
      if (__row == nullptr) {
        GUM_ERROR(NullElement, "the handler does not point to any database");
      }
      if (end > __row->size()) {
        GUM_ERROR(SizeError,
                  "the database has fewer rows ("
                     << __row->size() << ") than the upper range (" << end
                     << ") specified to the handler");
      }

      __begin_index = begin;
      __end_index = end;
      __index = begin;
    }


    // returns the current range of the handler
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE std::pair< std::size_t, std::size_t >
           IDatabaseTable< T_DATA, ALLOC >::Handler::range() const {
      return std::pair< std::size_t, std::size_t >(__begin_index, __end_index);
    }


    // returns the names of the variables
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE const typename IDatabaseTable< T_DATA, ALLOC >::Handler::
       template DBVector< std::string >&
       IDatabaseTable< T_DATA, ALLOC >::Handler::variableNames() const {
      return __db->variableNames();
    }


    // returns the number of variables (columns) of the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE std::size_t
           IDatabaseTable< T_DATA, ALLOC >::Handler::nbVariables() const {
      if (__db != nullptr)
        return __db->variableNames().size();
      else
        return 0;
    }


    // returns a pointer on the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE const IDatabaseTable< T_DATA, ALLOC >&
                 IDatabaseTable< T_DATA, ALLOC >::Handler::database() const {
      if (__db == nullptr) {
        GUM_ERROR(NullElement,
                  "The database handler does not point toward a database");
      }
      return *__db;
    }


    // ===========================================================================
    // Safe handlers
    // ===========================================================================

    // attach a new handler to the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void IDatabaseTable< T_DATA, ALLOC >::HandlerSafe::__attachHandler() {
      if (this->__db != nullptr) { this->__db->__attachHandler(this); }
    }


    // detach a handler
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void IDatabaseTable< T_DATA, ALLOC >::HandlerSafe::__detachHandler() {
      if (this->__db != nullptr) { this->__db->__detachHandler(this); }
    }


    // default constructor
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE IDatabaseTable< T_DATA, ALLOC >::HandlerSafe::HandlerSafe(
       const IDatabaseTable< T_DATA, ALLOC >& db) :
        IDatabaseTable< T_DATA, ALLOC >::Handler(db) {
      __attachHandler();
      GUM_CONSTRUCTOR(IDatabaseTable::HandlerSafe);
    }


    // copy constructor
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE IDatabaseTable< T_DATA, ALLOC >::HandlerSafe::HandlerSafe(
       const typename IDatabaseTable< T_DATA, ALLOC >::HandlerSafe& h) :
        IDatabaseTable< T_DATA, ALLOC >::Handler(h) {
      __attachHandler();
      GUM_CONS_CPY(IDatabaseTable::HandlerSafe);
    }


    // move constructor
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE IDatabaseTable< T_DATA, ALLOC >::HandlerSafe::HandlerSafe(
       typename IDatabaseTable< T_DATA, ALLOC >::HandlerSafe&& h) :
        IDatabaseTable< T_DATA, ALLOC >::Handler(std::move(h)) {
      __attachHandler();
      GUM_CONS_MOV(IDatabaseTable::HandlerSafe);
    }


    // destructor
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE IDatabaseTable< T_DATA, ALLOC >::HandlerSafe::~HandlerSafe() {
      __detachHandler();
      GUM_DESTRUCTOR(IDatabaseTable::HandlerSafe);
    }


    // copy operator
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::HandlerSafe&
       IDatabaseTable< T_DATA, ALLOC >::HandlerSafe::operator=(
          const typename IDatabaseTable< T_DATA, ALLOC >::HandlerSafe& h) {
      if (this->__db != h.__db) {
        __detachHandler();
        this->__db = h.__db;
        __attachHandler();
      }

      IDatabaseTable< T_DATA, ALLOC >::Handler::operator=(h);
      return *this;
    }


    // copy operator
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::HandlerSafe&
          IDatabaseTable< T_DATA, ALLOC >::HandlerSafe::
          operator=(const typename IDatabaseTable< T_DATA, ALLOC >::Handler& h) {
      return this->operator=(
         dynamic_cast< const IDatabaseTable< T_DATA, ALLOC >::HandlerSafe& >(h));
    }


    // move operator
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::HandlerSafe&
          IDatabaseTable< T_DATA, ALLOC >::HandlerSafe::
          operator=(typename IDatabaseTable< T_DATA, ALLOC >::HandlerSafe&& h) {
      if (this->__db != h.__db) {
        __detachHandler();
        this->__db = h.__db;
        __attachHandler();
      }

      IDatabaseTable< T_DATA, ALLOC >::Handler::operator=(std::move(h));
      return *this;
    }


    // move operator
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::HandlerSafe&
          IDatabaseTable< T_DATA, ALLOC >::HandlerSafe::
          operator=(typename IDatabaseTable< T_DATA, ALLOC >::Handler&& h) {
      return this->operator=(std::move(
         dynamic_cast< IDatabaseTable< T_DATA, ALLOC >::HandlerSafe& >(h)));
    }


    // ===========================================================================
    // Database Tables
    // ===========================================================================

    // returns the allocator of the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE ALLOC< T_DATA > IDatabaseTable< T_DATA, ALLOC >::getAllocator() const {
      return ALLOC< T_DATA >(*this);
    }


    // create the end iterators
    template < typename T_DATA, template < typename > class ALLOC >
    void IDatabaseTable< T_DATA, ALLOC >::__createEndIterators() {
      const IDatabaseTable< T_DATA, ALLOC >& db = *this;
      ALLOC< iterator >                      allocator1(*this);
      __end = allocator1.allocate(1);
      try {
        allocator1.construct(__end, db);
      } catch (...) {
        allocator1.deallocate(__end, 1);
        throw;
      }

      ALLOC< iterator_safe > allocator2(*this);
      try {
        __end_safe = allocator2.allocate(1);
        try {
          allocator2.construct(__end_safe, *this);
        } catch (...) {
          allocator2.deallocate(__end_safe, 1);
          throw;
        }
      } catch (...) {
        allocator1.destroy(__end);
        allocator1.deallocate(__end, 1);
        throw;
      }
    }


    // default constructor
    template < typename T_DATA, template < typename > class ALLOC >
    template < template < typename > class VARALLOC,
               template < typename >
               class MISSALLOC >
    IDatabaseTable< T_DATA, ALLOC >::IDatabaseTable(
       const typename IDatabaseTable< T_DATA, ALLOC >::template MissingValType<
          MISSALLOC >&                                            missing_symbols,
       const std::vector< std::string, VARALLOC< std::string > >& var_names,
       const ALLOC< T_DATA >&                                     alloc) :
        ALLOC< T_DATA >(alloc),
        _variable_names(alloc), _rows(alloc), _missing_symbols(alloc),
        _has_row_missing_val(alloc), __list_of_safe_handlers(alloc) {
      // copy the names
      _variable_names.reserve(var_names.size());
      for (const auto& name : var_names)
        _variable_names.push_back(name);

      // copy the missing symbols
      _missing_symbols.reserve(missing_symbols.size());
      for (const auto& missing_symbol : missing_symbols)
        _missing_symbols.push_back(missing_symbol);

      // create the end iterators
      __createEndIterators();

      GUM_CONSTRUCTOR(IDatabaseTable);
    }


    // copy constructor with a given allocator
    template < typename T_DATA, template < typename > class ALLOC >
    IDatabaseTable< T_DATA, ALLOC >::IDatabaseTable(
       const IDatabaseTable< T_DATA, ALLOC >&                          from,
       const typename IDatabaseTable< T_DATA, ALLOC >::allocator_type& alloc) :
        ALLOC< T_DATA >(alloc),
        _variable_names(from._variable_names, alloc), _rows(from._rows, alloc),
        _missing_symbols(from._missing_symbols, alloc),
        _has_row_missing_val(from._has_row_missing_val, alloc),
        _max_nb_threads(from._max_nb_threads),
        _min_nb_rows_per_thread(from._min_nb_rows_per_thread),
        __list_of_safe_handlers(alloc) {
      // create the end iterators
      __createEndIterators();

      GUM_CONS_CPY(IDatabaseTable);
    }


    // copy constructor
    template < typename T_DATA, template < typename > class ALLOC >
    IDatabaseTable< T_DATA, ALLOC >::IDatabaseTable(
       const IDatabaseTable< T_DATA, ALLOC >& from) :
        IDatabaseTable< T_DATA, ALLOC >(from, from.getAllocator()) {}


    // move constructor with a given allocator
    template < typename T_DATA, template < typename > class ALLOC >
    IDatabaseTable< T_DATA, ALLOC >::IDatabaseTable(
       IDatabaseTable< T_DATA, ALLOC >&&                               from,
       const typename IDatabaseTable< T_DATA, ALLOC >::allocator_type& alloc) :
        ALLOC< T_DATA >(alloc),
        _variable_names(std::move(from._variable_names), alloc),
        _rows(std::move(from._rows), alloc),
        _missing_symbols(std::move(from._missing_symbols), alloc),
        _has_row_missing_val(std::move(from._has_row_missing_val), alloc),
        _max_nb_threads(from._max_nb_threads),
        _min_nb_rows_per_thread(from._min_nb_rows_per_thread),
        __list_of_safe_handlers(alloc) {
      // create the end iterators
      __createEndIterators();

      GUM_CONS_MOV(IDatabaseTable);
    }


    // move constructor
    template < typename T_DATA, template < typename > class ALLOC >
    IDatabaseTable< T_DATA, ALLOC >::IDatabaseTable(
       IDatabaseTable< T_DATA, ALLOC >&& from) :
        IDatabaseTable< T_DATA, ALLOC >(std::move(from), from.getAllocator()) {}


    // destructor
    template < typename T_DATA, template < typename > class ALLOC >
    IDatabaseTable< T_DATA, ALLOC >::~IDatabaseTable() {
      // indicate to all the handlers that we are destructing the database
      __safe_handlers_mutex.lock();
      for (auto handler : __list_of_safe_handlers) {
        handler->__db = nullptr;
        handler->__row = nullptr;
        handler->__end_index = 0;
        handler->__index = 0;
      }
      __safe_handlers_mutex.unlock();

      ALLOC< iterator > allocator1(this->getAllocator());
      allocator1.destroy(__end);
      allocator1.deallocate(__end, 1);

      ALLOC< iterator_safe > allocator2(this->getAllocator());
      allocator2.destroy(__end_safe);
      allocator2.deallocate(__end_safe, 1);

      GUM_DESTRUCTOR(IDatabaseTable);
    }


    // copy operator
    template < typename T_DATA, template < typename > class ALLOC >
    IDatabaseTable< T_DATA, ALLOC >& IDatabaseTable< T_DATA, ALLOC >::
                                     operator=(const IDatabaseTable< T_DATA, ALLOC >& from) {
      if (this != &from) {
        // invalidate the current handlers
        __safe_handlers_mutex.lock();
        for (auto handler : __list_of_safe_handlers) {
          handler->__db = nullptr;
          handler->__row = nullptr;
          handler->__end_index = 0;
          handler->__index = 0;
        }
        __list_of_safe_handlers.clear();
        __safe_handlers_mutex.unlock();

        _rows = from._rows;
        _variable_names = from._variable_names;
        _missing_symbols = from._missing_symbols;
        _has_row_missing_val = from._has_row_missing_val;
        _max_nb_threads = from._max_nb_threads;
        _min_nb_rows_per_thread = from._min_nb_rows_per_thread;

        // update the end iterators
        const std::size_t db_size = _rows.size();
        __end->__index = db_size;
        __end->__end_index = db_size;
        __end_safe->__index = db_size;
        __end_safe->__end_index = db_size;
      }

      return *this;
    }


    // move operator
    template < typename T_DATA, template < typename > class ALLOC >
    IDatabaseTable< T_DATA, ALLOC >& IDatabaseTable< T_DATA, ALLOC >::
                                     operator=(IDatabaseTable< T_DATA, ALLOC >&& from) {
      if (this != &from) {
        // invalidate the current handlers
        __safe_handlers_mutex.lock();
        for (auto handler : __list_of_safe_handlers) {
          handler->__db = nullptr;
          handler->__row = nullptr;
          handler->__end_index = 0;
          handler->__index = 0;
        }
        __safe_handlers_mutex.unlock();

        _rows = std::move(from._rows);
        _variable_names = std::move(from._variable_names);
        _missing_symbols = std::move(from._missing_symbols);
        _has_row_missing_val = std::move(from._has_row_missing_val);
        _max_nb_threads = from._max_nb_threads;
        _min_nb_rows_per_thread = from._min_nb_rows_per_thread;

        // update the end iterators
        const std::size_t db_size = _rows.size();
        __end->__index = db_size;
        __end->__end_index = db_size;
        __end_safe->__index = db_size;
        __end_safe->__end_index = db_size;
      }

      return *this;
    }


    // returns a new unsafe handler pointing to the beginning of the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::Handler
       IDatabaseTable< T_DATA, ALLOC >::begin() const {
      return Handler(*this);
    }


    // returns a new safe handler pointing to the beginning of the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::HandlerSafe
       IDatabaseTable< T_DATA, ALLOC >::beginSafe() const {
      return HandlerSafe(*this);
    }


    // returns a new unsafe handler pointing to the end of the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE const typename IDatabaseTable< T_DATA, ALLOC >::Handler&
       IDatabaseTable< T_DATA, ALLOC >::end() const noexcept {
      return *__end;
    }


    /// returns a new safe handler pointing to the beginning of the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE const typename IDatabaseTable< T_DATA, ALLOC >::HandlerSafe&
       IDatabaseTable< T_DATA, ALLOC >::endSafe() const noexcept {
      return *__end_safe;
    }


    // returns a new unsafe handler on the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::Handler
       IDatabaseTable< T_DATA, ALLOC >::handler() const {
      return Handler(*this);
    }


    // returns a new safe handler on the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE typename IDatabaseTable< T_DATA, ALLOC >::HandlerSafe
       IDatabaseTable< T_DATA, ALLOC >::handlerSafe() const {
      return HandlerSafe(*this);
    }


    // returns the content of the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE const typename IDatabaseTable< T_DATA,
                                          ALLOC >::template Matrix< T_DATA >&
       IDatabaseTable< T_DATA, ALLOC >::content() const noexcept {
      return _rows;
    }


    /// indicates whether the database contains some missing values
    template < typename T_DATA, template < typename > class ALLOC >
    bool IDatabaseTable< T_DATA, ALLOC >::hasMissingValues() const {
      for (const auto& status : _has_row_missing_val)
        if (status == IsMissing::True) return true;
      return false;
    }


    /// indicates whether the kth row contains some missing values
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE bool IDatabaseTable< T_DATA, ALLOC >::hasMissingValues(
       const std::size_t k) const {
      return _has_row_missing_val[k] == IsMissing::True;
    }


    // returns the variable names for all the columns
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE const std::vector< std::string, ALLOC< std::string > >&
                 IDatabaseTable< T_DATA, ALLOC >::variableNames() const noexcept {
      return _variable_names;
    }


    // sets the names of the variables
    template < typename T_DATA, template < typename > class ALLOC >
    template < template < typename > class OTHER_ALLOC >
    void IDatabaseTable< T_DATA, ALLOC >::setVariableNames(
       const std::vector< std::string, OTHER_ALLOC< std::string > >& names,
       const bool from_external_object) {
      // copy the variable names into a vector allocated with the allocator
      // used by the database
      const std::size_t       size = names.size();
      DBVector< std::string > variable_names(size);
      for (std::size_t i = 0; i < size; ++i)
        variable_names[i] = names[i];

      this->setVariableNames(variable_names, from_external_object);
    }


    /// returns the name of the kth column of the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE const std::string&
                 IDatabaseTable< T_DATA, ALLOC >::variableName(const std::size_t k) const {
      if (_variable_names.size() <= k)
        GUM_ERROR(OutOfBounds, "the database does not contain Column #" << k);
      return _variable_names[k];
    }


    /// returns the index of the column whose name is passed in argument
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE std::size_t IDatabaseTable< T_DATA, ALLOC >::columnFromVariableName(
       const std::string& name) const {
      const std::size_t size = _variable_names.size();
      for (std::size_t i = 0; i < size; ++i)
        if (_variable_names[i] == name) return i;

      GUM_ERROR(UndefinedElement,
                "the database contains no column whose name is " << name);
    }


    /// returns the indices of the columns whose name is passed in argument
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE
       typename IDatabaseTable< T_DATA, ALLOC >::template DBVector< std::size_t >
       IDatabaseTable< T_DATA, ALLOC >::columnsFromVariableName(
          const std::string& name) const {
      const std::size_t       size = _variable_names.size();
      DBVector< std::size_t > cols;
      for (std::size_t i = 0; i < size; ++i)
        if (_variable_names[i] == name) cols.push_back(i);

      if (cols.empty())
        GUM_ERROR(UndefinedElement,
                  "the database contains no column whose name is " << name);

      return cols;
    }


    // returns the number of variables (columns) of the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE std::size_t IDatabaseTable< T_DATA, ALLOC >::nbVariables() const
       noexcept {
      return _variable_names.size();
    }


    // returns the number of records in the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE std::size_t IDatabaseTable< T_DATA, ALLOC >::size() const noexcept {
      return _rows.size();
    }


    // returns the number of records in the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE std::size_t IDatabaseTable< T_DATA, ALLOC >::nbRows() const noexcept {
      return _rows.size();
    }


    // indicates whether the database contains some records or not
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE bool IDatabaseTable< T_DATA, ALLOC >::empty() const noexcept {
      return _rows.empty();
    }


    // update the handlers when the size of the database changes
    template < typename T_DATA, template < typename > class ALLOC >
    void IDatabaseTable< T_DATA, ALLOC >::__updateHandlers(
       std::size_t new_size) const {
      const std::size_t db_size = _rows.size();

      __safe_handlers_mutex.lock();
      for (auto handler : __list_of_safe_handlers) {
        if ((handler->__end_index == db_size)
            || (handler->__end_index > new_size)) {
          handler->__end_index = new_size;
          // there is no need to update the index because, in safe handlers,
          // we always check that the index is less than end_index when trying
          // to access the rows
        }
      }
      __safe_handlers_mutex.unlock();

      // update the end iterators
      __end->__index = new_size;
      __end->__end_index = new_size;
      __end_safe->__index = new_size;
      __end_safe->__end_index = new_size;
    }


    // attach a new handler to the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void IDatabaseTable< T_DATA, ALLOC >::__attachHandler(
       HandlerSafe* handler) const {
      __safe_handlers_mutex.lock();
      __list_of_safe_handlers.push_back(handler);
      __safe_handlers_mutex.unlock();
    }


    // detach a handler
    template < typename T_DATA, template < typename > class ALLOC >
    void IDatabaseTable< T_DATA, ALLOC >::__detachHandler(
       HandlerSafe* handler) const {
      __safe_handlers_mutex.lock();

      for (auto iter = __list_of_safe_handlers.rbegin();
           iter != __list_of_safe_handlers.rend();
           ++iter) {
        if (*iter == handler) {
          *iter = __list_of_safe_handlers.back();
          __list_of_safe_handlers.pop_back();
          break;
        }
      }

      __safe_handlers_mutex.unlock();
    }


    // checks whether a new row has the same size as the rest of the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE bool IDatabaseTable< T_DATA, ALLOC >::_isRowSizeOK(
       const std::size_t size) const {
      return (size == _variable_names.size());
    }


    // insert a new row at the end of the database
    template < typename T_DATA, template < typename > class ALLOC >
    template < template < typename > class OTHER_ALLOC >
    void IDatabaseTable< T_DATA, ALLOC >::insertRow(
       const std::vector< std::string, OTHER_ALLOC< std::string > >& new_row) {
      const std::size_t                                size = new_row.size();
      std::vector< std::string, ALLOC< std::string > > good_typed_row(size);
      for (std::size_t i = 0; i < size; ++i)
        good_typed_row[i] = new_row[i];
      this->insertRow(good_typed_row);
    }


    // insert a new DBRow at the end of the database
    template < typename T_DATA, template < typename > class ALLOC >
    void IDatabaseTable< T_DATA, ALLOC >::insertRow(
       typename IDatabaseTable< T_DATA, ALLOC >::template Row< T_DATA >&& new_row,
       const typename IDatabaseTable< T_DATA, ALLOC >::IsMissing
          contains_missing) {
      // check that the size of the row is the same as the rest of the database
      if (!_isRowSizeOK(new_row.size()))
        GUM_ERROR(SizeError,
                  "the new row is of size "
                     << new_row.size()
                     << ", which is different from the number of columns "
                     << "of the database, i.e., " << _variable_names.size());

      __updateHandlers(_rows.size() + 1);
      _rows.push_back(std::move(new_row));
      try {
        _has_row_missing_val.push_back(contains_missing);
      } catch (...) {
        _rows.pop_back();
        throw;
      }
    }


    // insert a new DBRow at the end of the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void IDatabaseTable< T_DATA, ALLOC >::insertRow(
       const typename IDatabaseTable< T_DATA, ALLOC >::template Row< T_DATA >& row,
       const typename IDatabaseTable< T_DATA, ALLOC >::IsMissing
          contains_missing) {
      this->insertRow(
         typename IDatabaseTable< T_DATA, ALLOC >::template Row< T_DATA >(row),
         contains_missing);
    }


    // insert a set of new DBRow at the end of the database
    template < typename T_DATA, template < typename > class ALLOC >
    void IDatabaseTable< T_DATA, ALLOC >::insertRows(
       typename IDatabaseTable< T_DATA, ALLOC >::template Matrix< T_DATA >&&
          new_rows,
       const typename IDatabaseTable< T_DATA, ALLOC >::template DBVector<
          typename IDatabaseTable< T_DATA, ALLOC >::IsMissing >&
          rows_have_missing_vals) {
      if (new_rows.empty()) return;

      // check that the missing values indicators vector has the same size
      // as the new rows
      if (rows_have_missing_vals.size() != new_rows.size())
        GUM_ERROR(
           SizeError,
           "the number of new rows (i.e., "
              << new_rows.size()
              << ") is different from the number of missing values indicators ("
              << rows_have_missing_vals.size());

      // check that all the rows have the same size
      const std::size_t new_size = new_rows[0].size();

      for (const auto& row : new_rows) {
        if (row.size() != new_size) {
          GUM_ERROR(SizeError,
                    "all the new rows do not have the same number of columns");
        }
      }

      // check that the sizes of the new rows are the same as the rest of
      // the database
      if (!_isRowSizeOK(new_size)) {
        GUM_ERROR(SizeError,
                  "the new rows have "
                     << new_size
                     << " columns, which is different from the number of columns "
                     << "of the database, i.e., " << _variable_names.size());
      }

      const std::size_t nb_new_rows = new_rows.size();
      const std::size_t new_db_size = _rows.size() + nb_new_rows;

      _rows.reserve(new_db_size);
      _has_row_missing_val.reserve(new_db_size);

      for (std::size_t i = std::size_t(0); i < nb_new_rows; ++i) {
        _rows.push_back(std::move(new_rows[i]));
        _has_row_missing_val.push_back(rows_have_missing_vals[i]);
      }

      __updateHandlers(new_db_size);
    }


    // insert a set of new DBRow at the end of the database
    template < typename T_DATA, template < typename > class ALLOC >
    void IDatabaseTable< T_DATA, ALLOC >::insertRows(
       const typename IDatabaseTable< T_DATA, ALLOC >::template Matrix< T_DATA >&
          new_rows,
       const typename IDatabaseTable< T_DATA, ALLOC >::template DBVector<
          typename IDatabaseTable< T_DATA, ALLOC >::IsMissing >&
          rows_have_missing_vals) {
      if (new_rows.empty()) return;

      // check that the missing values indicators vector has the same size
      // as the new rows
      if (rows_have_missing_vals.size() != new_rows.size())
        GUM_ERROR(
           SizeError,
           "the number of new rows (i.e., "
              << new_rows.size()
              << ") is different from the number of missing values indicators ("
              << rows_have_missing_vals.size());

      // check that all the rows have the same size
      const std::size_t new_size = new_rows[0].size();

      for (const auto& row : new_rows) {
        if (row.size() != new_size) {
          GUM_ERROR(SizeError,
                    "all the new rows do not have the same number of columns");
        }
      }

      // check that the sizes of the new rows are the same as the rest of
      // the database
      std::size_t db_size = _rows.size();

      if (!_isRowSizeOK(new_size)) {
        GUM_ERROR(SizeError,
                  "the new rows have "
                     << new_size
                     << " columns, which is different from the number of columns "
                     << "of the database, i.e., " << _variable_names.size());
      }

      const std::size_t nb_new_rows = new_rows.size();
      const std::size_t new_db_size = _rows.size() + nb_new_rows;

      _rows.reserve(new_db_size);
      _has_row_missing_val.reserve(new_db_size);

      for (std::size_t i = std::size_t(0); i < nb_new_rows; ++i) {
        _rows.push_back(new_rows[i]);
        _has_row_missing_val.push_back(rows_have_missing_vals[i]);
      }

      __updateHandlers(db_size);
    }


    // erase a given row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void IDatabaseTable< T_DATA, ALLOC >::eraseRow(std::size_t index) {
      const std::size_t db_size = _rows.size();

      if (index < db_size) {
        __updateHandlers(db_size - 1);
        _rows.erase(_rows.begin() + index);
        _has_row_missing_val.erase(_has_row_missing_val.begin() + index);
      }
    }


    // erase the last row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void IDatabaseTable< T_DATA, ALLOC >::eraseLastRow() {
      const std::size_t db_size = _rows.size();

      if (db_size) {
        __updateHandlers(db_size - 1);
        _rows.pop_back();
        _has_row_missing_val.pop_back();
      }
    }


    // erase the first row
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void IDatabaseTable< T_DATA, ALLOC >::eraseFirstRow() {
      const std::size_t db_size = _rows.size();

      if (db_size) {
        __updateHandlers(db_size - 1);
        _rows.erase(_rows.begin());
        _has_row_missing_val.erase(_has_row_missing_val.begin());
      }
    }


    // erase all the rows
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void IDatabaseTable< T_DATA, ALLOC >::eraseAllRows() {
      __updateHandlers(0);
      _rows.clear();
      _has_row_missing_val.clear();
    }


    // erase the k first rows
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void
       IDatabaseTable< T_DATA, ALLOC >::eraseFirstRows(const std::size_t nb_rows) {
      const std::size_t db_size = _rows.size();

      if (nb_rows >= db_size) {
        eraseAllRows();
      } else {
        __updateHandlers(db_size - nb_rows);
        _rows.erase(_rows.begin(), _rows.begin() + nb_rows);
        _has_row_missing_val.erase(_has_row_missing_val.begin(),
                                   _has_row_missing_val.begin() + nb_rows);
      }
    }


    // erase the k last rows
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void
       IDatabaseTable< T_DATA, ALLOC >::eraseLastRows(const std::size_t nb_rows) {
      const std::size_t db_size = _rows.size();

      if (nb_rows >= db_size) {
        eraseAllRows();
      } else {
        __updateHandlers(db_size - nb_rows);
        _rows.erase(_rows.begin() + (db_size - nb_rows), _rows.begin() + db_size);
        _has_row_missing_val.erase(_has_row_missing_val.begin()
                                      + (db_size - nb_rows),
                                   _has_row_missing_val.begin() + db_size);
      }
    }


    // erase the rows from the debth to the endth (not included)
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void IDatabaseTable< T_DATA, ALLOC >::eraseRows(std::size_t deb,
                                                           std::size_t end) {
      if (deb > end) std::swap(deb, end);

      const std::size_t db_size = _rows.size();

      if (end >= db_size) {
        if (deb >= db_size) {
          return;
        } else {
          eraseLastRows(db_size - deb);
        }
      } else {
        __updateHandlers(db_size - (end - deb));
        _rows.erase(_rows.begin() + deb, _rows.begin() + end);
        _has_row_missing_val.erase(_has_row_missing_val.begin() + deb,
                                   _has_row_missing_val.begin() + end);
      }
    }


    // erase the content of the database, including the names of the variables
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE void IDatabaseTable< T_DATA, ALLOC >::clear() {
      __updateHandlers(0);
      _rows.clear();
      _has_row_missing_val.clear();
      _variable_names.clear();
    }


    // returns the set of symbols for the missing values
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE const std::vector< std::string, ALLOC< std::string > >&
                 IDatabaseTable< T_DATA, ALLOC >::missingSymbols() const {
      return _missing_symbols;
    }


    /// changes the max number of threads that a database can use
    template < typename T_DATA, template < typename > class ALLOC >
    void IDatabaseTable< T_DATA, ALLOC >::setMaxNbThreads(
       const std::size_t nb) const {
      if (nb == std::size_t(0))
        _max_nb_threads = std::size_t(1);
      else
        _max_nb_threads = nb;
    }


    /// returns the number of threads used to parse the database
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE std::size_t IDatabaseTable< T_DATA, ALLOC >::nbThreads() const {
      return _max_nb_threads;
    }


    /** @brief changes the number min of rows a thread should process in a
     * multithreading context */
    template < typename T_DATA, template < typename > class ALLOC >
    void IDatabaseTable< T_DATA, ALLOC >::setMinNbRowsPerThread(
       const std::size_t nb) const {
      if (nb == std::size_t(0))
        _min_nb_rows_per_thread = std::size_t(1);
      else
        _min_nb_rows_per_thread = nb;
    }


    /// returns the minimum of rows that each thread should process
    template < typename T_DATA, template < typename > class ALLOC >
    INLINE std::size_t
           IDatabaseTable< T_DATA, ALLOC >::minNbRowsPerThread() const {
      return _min_nb_rows_per_thread;
    }


    /// insert new rows at the end of the database
    template < template < typename > class ALLOC >
    void IDatabaseTableInsert4DBCell< ALLOC, true >::insertRows(
       const typename IDatabaseTableInsert4DBCell< ALLOC, true >::
          template DBVector< DBVector< std::string > >& new_rows) {
      for (const auto& new_row : new_rows)
        this->insertRow(new_row);
    }


    /// insert new rows at the end of the database
    template < template < typename > class ALLOC >
    void IDatabaseTableInsert4DBCell< ALLOC, false >::insertRows(
       const typename IDatabaseTableInsert4DBCell< ALLOC, false >::
          template DBVector< DBVector< std::string > >& new_rows) {
      for (const auto& new_row : new_rows)
        this->insertRow(new_row);
    }


    /// assign a given weight to all the rows of the database
    template < typename T_DATA, template < typename > class ALLOC >
    void
       IDatabaseTable< T_DATA, ALLOC >::setAllRowsWeight(const double new_weight) {
      // determine the number of threads to use and the number of rows
      // they should process
      std::vector< std::pair< std::size_t, std::size_t > > ranges;
      const std::size_t                                    db_size = nbRows();
      std::size_t nb_threads = db_size / _min_nb_rows_per_thread;
      if (nb_threads < 1)
        nb_threads = 1;
      else if (nb_threads > _max_nb_threads)
        nb_threads = _max_nb_threads;
      std::size_t nb_rows_per_thread = db_size / nb_threads;
      std::size_t rest_rows = db_size - nb_rows_per_thread * nb_threads;

      // assign to threads the ranges over which they should change the
      // rows weights
      std::size_t begin_index = std::size_t(0);
      for (std::size_t i = std::size_t(0); i < nb_threads; ++i) {
        std::size_t end_index = begin_index + nb_rows_per_thread;
        if (rest_rows != std::size_t(0)) {
          ++end_index;
          --rest_rows;
        }
        ranges.push_back(
           std::pair< std::size_t, std::size_t >(begin_index, end_index));
        begin_index = end_index;
      }

      // perform the assignment:
      // launch the threads
      // here we use openMP for launching the threads because, experimentally,
      // it seems to provide results that are twice as fast as the results
      // with the std::thread
#  pragma omp parallel num_threads(int(nb_threads))
      {
        // get the number of the thread
        const std::size_t this_thread = getThreadNumber();
        const std::size_t begin_index = ranges[this_thread].first;
        const std::size_t end_index = ranges[this_thread].second;

        for (std::size_t i = begin_index; i < end_index; ++i) {
          _rows[i].setWeight(new_weight);
        }
      }
    }

    /// assigns a given weight to the ith row of the database
    template < typename T_DATA, template < typename > class ALLOC >
    void IDatabaseTable< T_DATA, ALLOC >::setWeight(const std::size_t i,
                                                    const double      weight) {
      // check that i is less than the number of rows
      const std::size_t dbsize = nbRows();
      if (i >= dbsize) {
        std::string str;
        switch (i) {
          case 1: str = "st"; break;
          case 2: str = "nd"; break;
          default: str = "th";
        }
        GUM_ERROR(OutOfBounds,
                  "it is impossible to set the weight of the "
                     << i << str << " record because the database contains only "
                     << nbRows() << " records");
      }

      // check that the weight is positive
      if (weight < 0) {
        std::string str;
        switch (i) {
          case 1: str = "st"; break;
          case 2: str = "nd"; break;
          default: str = "th";
        }
        GUM_ERROR(OutOfBounds,
                  "it is impossible to set "
                     << weight << " as a weight of the " << i << str
                     << " record because this weight is negative");
      }

      _rows[i].setWeight(weight);
    }


    /// returns the weight of the ith record
    template < typename T_DATA, template < typename > class ALLOC >
    double IDatabaseTable< T_DATA, ALLOC >::weight(const std::size_t i) const {
      // check that i is less than the number of rows
      const std::size_t dbsize = nbRows();
      if (i >= dbsize) {
        std::string str;
        switch (i) {
          case 1: str = "st"; break;
          case 2: str = "nd"; break;
          default: str = "th";
        }
        GUM_ERROR(OutOfBounds,
                  "it is impossible to get the weight of the "
                     << i << str << " record because the database contains only "
                     << nbRows() << " records");
      }

      return _rows[i].weight();
    }


    /// returns the weight of the whole database
    template < typename T_DATA, template < typename > class ALLOC >
    double IDatabaseTable< T_DATA, ALLOC >::weight() const {
      double w = 0.0;
      for (const auto& row : _rows)
        w += row.weight();
      return w;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
