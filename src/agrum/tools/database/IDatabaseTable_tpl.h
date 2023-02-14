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
 * @brief The implementation of the common class for tabular databases
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/tools/database/IDatabaseTable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    // ===========================================================================
    // Unsafe handlers
    // ===========================================================================

    // default constructor
    template < typename T_DATA >
    INLINE IDatabaseTable< T_DATA >::Handler::Handler(const IDatabaseTable< T_DATA >& db) :
        DBHandler< T_DATA >(), _db_(&db), _row_(&(db.content())),
        _end_index_(std::size_t(_row_->size())) {
      GUM_CONSTRUCTOR(IDatabaseTable::Handler);
    }


    // copy constructor
    template < typename T_DATA >
    INLINE IDatabaseTable< T_DATA >::Handler::Handler(
       const typename IDatabaseTable< T_DATA >::Handler& h) :
        DBHandler< T_DATA >(),
        _db_(h._db_), _row_(h._row_), _index_(h._index_), _begin_index_(h._begin_index_),
        _end_index_(h._end_index_) {
      GUM_CONS_CPY(IDatabaseTable::Handler);
    }


    // move constructor
    template < typename T_DATA >
    INLINE
       IDatabaseTable< T_DATA >::Handler::Handler(typename IDatabaseTable< T_DATA >::Handler&& h) :
        DBHandler< T_DATA >(),
        _db_(h._db_), _row_(h._row_), _index_(h._index_), _begin_index_(h._begin_index_),
        _end_index_(h._end_index_) {
      GUM_CONS_MOV(IDatabaseTable::Handler);
    }


    // destructor
    template < typename T_DATA >
    INLINE IDatabaseTable< T_DATA >::Handler::~Handler() {
      GUM_DESTRUCTOR(IDatabaseTable::Handler);
    }


    // copy operator
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::Handler& IDatabaseTable< T_DATA >::Handler::operator=(
       const typename IDatabaseTable< T_DATA >::Handler& h) {
      _db_          = h._db_;
      _row_         = h._row_;
      _index_       = h._index_;
      _begin_index_ = h._begin_index_;
      _end_index_   = h._end_index_;
      return *this;
    }


    // move operator
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::Handler& IDatabaseTable< T_DATA >::Handler::operator=(
       typename IDatabaseTable< T_DATA >::Handler&& h) {
      _db_          = h._db_;
      _row_         = h._row_;
      _index_       = h._index_;
      _begin_index_ = h._begin_index_;
      _end_index_   = h._end_index_;
      return *this;
    }


    // returns the current row pointed to by the handler
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::Handler::const_reference
       IDatabaseTable< T_DATA >::Handler::operator*() const {
      return _row_->operator[](_index_);
    }


    // Dereferences the value pointed to by the handler (unsafe version)
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::Handler::const_pointer
       IDatabaseTable< T_DATA >::Handler::operator->() const {
      return &(_row_->operator[](_index_));
    }


    // makes the handler point to the next row
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::Handler&
       IDatabaseTable< T_DATA >::Handler::operator++() {
      ++_index_;
      return *this;
    }


    // makes the handler point to the previous row
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::Handler&
       IDatabaseTable< T_DATA >::Handler::operator--() {
      if (_index_ > _begin_index_) --_index_;
      return *this;
    }


    //  moves the handler by i rows
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::Handler&
       IDatabaseTable< T_DATA >::Handler::operator+=(const std::size_t i) {
      _index_ += i;
      return *this;
    }


    //  moves back the handler by i rows
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::Handler&
       IDatabaseTable< T_DATA >::Handler::operator-=(const std::size_t i) {
      if (_index_ >= _begin_index_ + i) _index_ -= i;
      else _index_ = _begin_index_;
      return *this;
    }


    // checks whether two handlers point on the same row
    template < typename T_DATA >
    INLINE bool IDatabaseTable< T_DATA >::Handler::operator==(const Handler& handler) const {
      return _index_ == handler._index_;
    }


    // checks whether two handlers point to different rows
    template < typename T_DATA >
    INLINE bool IDatabaseTable< T_DATA >::Handler::operator!=(const Handler& handler) const {
      return _index_ != handler._index_;
    }


    // returns the number of rows managed by the handler
    template < typename T_DATA >
    INLINE std::size_t IDatabaseTable< T_DATA >::Handler::size() const {
      return _end_index_ - _begin_index_;
    }


    // return the number of rows of the whole database
    template < typename T_DATA >
    INLINE std::size_t IDatabaseTable< T_DATA >::Handler::DBSize() const {
      if (_row_ != nullptr) return _row_->size();
      else return std::size_t(0);
    }


    // returns the current row pointed to by the handler
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::Handler::const_reference
       IDatabaseTable< T_DATA >::Handler::rowSafe() const {
      if (_index_ >= _end_index_) { GUM_ERROR(OutOfBounds, "the handler has reached its end") }

      return _row_->operator[](_index_);
    }


    // returns the current row pointed to by the handler
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::Handler::reference
       IDatabaseTable< T_DATA >::Handler::rowSafe() {
      if (_index_ >= _end_index_) { GUM_ERROR(OutOfBounds, "the handler has reached its end") }

      return const_cast< Matrix< T_DATA >* >(_row_)->operator[](_index_);
    }


    // returns the current row pointed to by the handler (unsafe version)
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::Handler::const_reference
       IDatabaseTable< T_DATA >::Handler::row() const {
      return _row_->operator[](_index_);
    }


    // returns the current row pointed to by the handler (unsafe version)
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::Handler::reference
       IDatabaseTable< T_DATA >::Handler::row() {
      return const_cast< Matrix< T_DATA >* >(_row_)->operator[](_index_);
    }


    // makes the handler point to the next row
    template < typename T_DATA >
    INLINE void IDatabaseTable< T_DATA >::Handler::nextRow() {
      ++_index_;
    }


    // returns the number of the current row
    template < typename T_DATA >
    INLINE std::size_t IDatabaseTable< T_DATA >::Handler::numRow() const {
      return (_index_ >= _begin_index_) ? _index_ - _begin_index_ : 0;
    }

    // indicates whether the handler has reached its end or not
    template < typename T_DATA >
    INLINE bool IDatabaseTable< T_DATA >::Handler::hasRows() const {
      return (_index_ < _end_index_);
    }

    // puts the handler to the beginning of the database area it handles
    template < typename T_DATA >
    INLINE void IDatabaseTable< T_DATA >::Handler::reset() {
      _index_ = _begin_index_;
    }


    // returns a new handler that points to the beginning of the
    // database area of the current handler */
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::Handler
       IDatabaseTable< T_DATA >::Handler::begin() const {
      Handler handler(*this);
      handler.reset();
      return handler;
    }


    // returns a new handler that points to the end of the
    // database area of the current handler */
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::Handler
       IDatabaseTable< T_DATA >::Handler::end() const {
      Handler handler(*this);
      handler._index_ = _end_index_;
      return handler;
    }


    // sets the area in the database the handler will handle
    template < typename T_DATA >
    INLINE void IDatabaseTable< T_DATA >::Handler::setRange(std::size_t first, std::size_t last) {
      if (first > last) std::swap(first, last);

      // check that the end belongs to the database, else raise an exception
      if (_row_ == nullptr) GUM_ERROR(NullElement, "the handler does not point to any database")

      if (last > _row_->size())
        GUM_ERROR(SizeError,
                  "the database has fewer rows (" << _row_->size() << ") than the upper range ("
                                                  << last << ") specified to the handler")

      _begin_index_ = first;
      _end_index_   = last;
      _index_       = first;
    }


    // returns the current range of the handler
    template < typename T_DATA >
    INLINE std::pair< std::size_t, std::size_t > IDatabaseTable< T_DATA >::Handler::range() const {
      return std::pair< std::size_t, std::size_t >(_begin_index_, _end_index_);
    }


    // returns the names of the variables
    template < typename T_DATA >
    INLINE const typename IDatabaseTable< T_DATA >::Handler::template DBVector< std::string >&
       IDatabaseTable< T_DATA >::Handler::variableNames() const {
      return _db_->variableNames();
    }


    // returns the number of variables (columns) of the database
    template < typename T_DATA >
    INLINE std::size_t IDatabaseTable< T_DATA >::Handler::nbVariables() const {
      if (_db_ != nullptr) return _db_->variableNames().size();
      else return 0;
    }


    // returns a pointer on the database
    template < typename T_DATA >
    INLINE const IDatabaseTable< T_DATA >& IDatabaseTable< T_DATA >::Handler::database() const {
      if (_db_ == nullptr) {
        GUM_ERROR(NullElement, "The database handler does not point toward a database")
      }
      return *_db_;
    }


    // ===========================================================================
    // Safe handlers
    // ===========================================================================

    // attach a new handler to the database
    template < typename T_DATA >
    INLINE void IDatabaseTable< T_DATA >::HandlerSafe::_attachHandler_() {
      if (this->_db_ != nullptr) { this->_db_->_attachHandler_(this); }
    }


    // detach a handler
    template < typename T_DATA >
    INLINE void IDatabaseTable< T_DATA >::HandlerSafe::_detachHandler_() {
      if (this->_db_ != nullptr) { this->_db_->_detachHandler_(this); }
    }


    // default constructor
    template < typename T_DATA >
    INLINE IDatabaseTable< T_DATA >::HandlerSafe::HandlerSafe(const IDatabaseTable< T_DATA >& db) :
        IDatabaseTable< T_DATA >::Handler(db) {
      _attachHandler_();
      GUM_CONSTRUCTOR(IDatabaseTable::HandlerSafe);
    }


    // copy constructor
    template < typename T_DATA >
    INLINE IDatabaseTable< T_DATA >::HandlerSafe::HandlerSafe(
       const typename IDatabaseTable< T_DATA >::HandlerSafe& h) :
        IDatabaseTable< T_DATA >::Handler(h) {
      _attachHandler_();
      GUM_CONS_CPY(IDatabaseTable::HandlerSafe);
    }


    // move constructor
    template < typename T_DATA >
    INLINE IDatabaseTable< T_DATA >::HandlerSafe::HandlerSafe(
       typename IDatabaseTable< T_DATA >::HandlerSafe&& h) :
        IDatabaseTable< T_DATA >::Handler(std::move(h)) {
      _attachHandler_();
      GUM_CONS_MOV(IDatabaseTable::HandlerSafe);
    }


    // destructor
    template < typename T_DATA >
    INLINE IDatabaseTable< T_DATA >::HandlerSafe::~HandlerSafe() {
      _detachHandler_();
      GUM_DESTRUCTOR(IDatabaseTable::HandlerSafe);
    }


    // copy operator
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::HandlerSafe&
       IDatabaseTable< T_DATA >::HandlerSafe::operator=(
          const typename IDatabaseTable< T_DATA >::HandlerSafe& h) {
      if (this->_db_ != h._db_) {
        _detachHandler_();
        this->_db_ = h._db_;
        _attachHandler_();
      }

      IDatabaseTable< T_DATA >::Handler::operator=(h);
      return *this;
    }


    // copy operator
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::HandlerSafe&
       IDatabaseTable< T_DATA >::HandlerSafe::operator=(
          const typename IDatabaseTable< T_DATA >::Handler& h) {
      return this->operator=(dynamic_cast< const IDatabaseTable< T_DATA >::HandlerSafe& >(h));
    }


    // move operator
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::HandlerSafe&
       IDatabaseTable< T_DATA >::HandlerSafe::operator=(
          typename IDatabaseTable< T_DATA >::HandlerSafe&& h) {
      if (this->_db_ != h._db_) {
        _detachHandler_();
        this->_db_ = h._db_;
        _attachHandler_();
      }

      IDatabaseTable< T_DATA >::Handler::operator=(std::move(h));
      return *this;
    }


    // move operator
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::HandlerSafe&
       IDatabaseTable< T_DATA >::HandlerSafe::operator=(
          typename IDatabaseTable< T_DATA >::Handler&& h) {
      return this->operator=(std::move(dynamic_cast< IDatabaseTable< T_DATA >::HandlerSafe& >(h)));
    }


    // ===========================================================================
    // Database Tables
    // ===========================================================================

    // create the end iterators
    template < typename T_DATA >
    void IDatabaseTable< T_DATA >::_createEndIterators_() {
      _end_ = new iterator(*this);
      try {
        _end_safe_ = new iterator_safe(*this);
      } catch (...) {
        delete _end_;
        throw;
      }
    }


    // default constructor
    template < typename T_DATA >
    IDatabaseTable< T_DATA >::IDatabaseTable(
       const typename IDatabaseTable< T_DATA >::MissingValType& missing_symbols,
       const std::vector< std::string >&                        var_names) :
        variable_names_(var_names),
        missing_symbols_(missing_symbols) {
      // create the end iterators
      _createEndIterators_();

      GUM_CONSTRUCTOR(IDatabaseTable);
    }


    // copy constructor
    template < typename T_DATA >
    IDatabaseTable< T_DATA >::IDatabaseTable(const IDatabaseTable< T_DATA >& from) :
        variable_names_(from.variable_names_), rows_(from.rows_),
        missing_symbols_(from.missing_symbols_), has_row_missing_val_(from.has_row_missing_val_),
        max_nb_threads_(from.max_nb_threads_),
        min_nb_rows_per_thread_(from.min_nb_rows_per_thread_) {
      // create the end iterators
      _createEndIterators_();

      GUM_CONS_CPY(IDatabaseTable);
    }


    // move constructor
    template < typename T_DATA >
    IDatabaseTable< T_DATA >::IDatabaseTable(IDatabaseTable< T_DATA >&& from) :
        variable_names_(std::move(from.variable_names_)), rows_(std::move(from.rows_)),
        missing_symbols_(std::move(from.missing_symbols_)),
        has_row_missing_val_(std::move(from.has_row_missing_val_)),
        max_nb_threads_(from.max_nb_threads_),
        min_nb_rows_per_thread_(from.min_nb_rows_per_thread_) {
      // create the end iterators
      _createEndIterators_();

      GUM_CONS_MOV(IDatabaseTable);
    }


    // destructor
    template < typename T_DATA >
    IDatabaseTable< T_DATA >::~IDatabaseTable() {
      // indicate to all the handlers that we are destructing the database
      _safe_handlers_mutex_.lock();
      for (auto handler: _list_of_safe_handlers_) {
        handler->_db_        = nullptr;
        handler->_row_       = nullptr;
        handler->_end_index_ = 0;
        handler->_index_     = 0;
      }
      _safe_handlers_mutex_.unlock();

      delete _end_;
      delete _end_safe_;

      GUM_DESTRUCTOR(IDatabaseTable);
    }


    // copy operator
    template < typename T_DATA >
    IDatabaseTable< T_DATA >&
       IDatabaseTable< T_DATA >::operator=(const IDatabaseTable< T_DATA >& from) {
      if (this != &from) {
        // invalidate the current handlers
        _safe_handlers_mutex_.lock();
        for (auto handler: _list_of_safe_handlers_) {
          handler->_db_        = nullptr;
          handler->_row_       = nullptr;
          handler->_end_index_ = 0;
          handler->_index_     = 0;
        }
        _list_of_safe_handlers_.clear();
        _safe_handlers_mutex_.unlock();

        rows_                   = from.rows_;
        variable_names_         = from.variable_names_;
        missing_symbols_        = from.missing_symbols_;
        has_row_missing_val_    = from.has_row_missing_val_;
        max_nb_threads_         = from.max_nb_threads_;
        min_nb_rows_per_thread_ = from.min_nb_rows_per_thread_;

        // update the end iterators
        const std::size_t db_size = rows_.size();
        _end_->_index_            = db_size;
        _end_->_end_index_        = db_size;
        _end_safe_->_index_       = db_size;
        _end_safe_->_end_index_   = db_size;
      }

      return *this;
    }


    // move operator
    template < typename T_DATA >
    IDatabaseTable< T_DATA >& IDatabaseTable< T_DATA >::operator=(IDatabaseTable< T_DATA >&& from) {
      if (this != &from) {
        // invalidate the current handlers
        _safe_handlers_mutex_.lock();
        for (auto handler: _list_of_safe_handlers_) {
          handler->_db_        = nullptr;
          handler->_row_       = nullptr;
          handler->_end_index_ = 0;
          handler->_index_     = 0;
        }
        _safe_handlers_mutex_.unlock();

        rows_                   = std::move(from.rows_);
        variable_names_         = std::move(from.variable_names_);
        missing_symbols_        = std::move(from.missing_symbols_);
        has_row_missing_val_    = std::move(from.has_row_missing_val_);
        max_nb_threads_         = from.max_nb_threads_;
        min_nb_rows_per_thread_ = from.min_nb_rows_per_thread_;

        // update the end iterators
        const std::size_t db_size = rows_.size();
        _end_->_index_            = db_size;
        _end_->_end_index_        = db_size;
        _end_safe_->_index_       = db_size;
        _end_safe_->_end_index_   = db_size;
      }

      return *this;
    }


    // returns a new unsafe handler pointing to the beginning of the database
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::Handler IDatabaseTable< T_DATA >::begin() const {
      return Handler(*this);
    }


    // returns a new safe handler pointing to the beginning of the database
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::HandlerSafe
       IDatabaseTable< T_DATA >::beginSafe() const {
      return HandlerSafe(*this);
    }


    // returns a new unsafe handler pointing to the end of the database
    template < typename T_DATA >
    INLINE const typename IDatabaseTable< T_DATA >::Handler&
       IDatabaseTable< T_DATA >::end() const noexcept {
      return *_end_;
    }


    /// returns a new safe handler pointing to the beginning of the database
    template < typename T_DATA >
    INLINE const typename IDatabaseTable< T_DATA >::HandlerSafe&
       IDatabaseTable< T_DATA >::endSafe() const noexcept {
      return *_end_safe_;
    }


    // returns a new unsafe handler on the database
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::Handler IDatabaseTable< T_DATA >::handler() const {
      return Handler(*this);
    }


    // returns a new safe handler on the database
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::HandlerSafe
       IDatabaseTable< T_DATA >::handlerSafe() const {
      return HandlerSafe(*this);
    }


    // returns the content of the database
    template < typename T_DATA >
    INLINE const typename IDatabaseTable< T_DATA >::template Matrix< T_DATA >&
       IDatabaseTable< T_DATA >::content() const noexcept {
      return rows_;
    }


    /// indicates whether the database contains some missing values
    template < typename T_DATA >
    bool IDatabaseTable< T_DATA >::hasMissingValues() const {
      for (const auto& status: has_row_missing_val_)
        if (status == IsMissing::True) return true;
      return false;
    }


    /// indicates whether the kth row contains some missing values
    template < typename T_DATA >
    INLINE bool IDatabaseTable< T_DATA >::hasMissingValues(const std::size_t k) const {
      return has_row_missing_val_[k] == IsMissing::True;
    }


    // returns the variable names for all the columns
    template < typename T_DATA >
    INLINE const std::vector< std::string >&
                 IDatabaseTable< T_DATA >::variableNames() const noexcept {
      return variable_names_;
    }


    /// returns the name of the kth column of the database
    template < typename T_DATA >
    INLINE const std::string& IDatabaseTable< T_DATA >::variableName(const std::size_t k) const {
      if (variable_names_.size() <= k)
        GUM_ERROR(OutOfBounds, "the database does not contain Column #" << k)
      return variable_names_[k];
    }


    /// returns the index of the column whose name is passed in argument
    template < typename T_DATA >
    INLINE std::size_t
           IDatabaseTable< T_DATA >::columnFromVariableName(const std::string& name) const {
      const std::size_t size = variable_names_.size();
      for (std::size_t i = 0; i < size; ++i)
        if (variable_names_[i] == name) return i;

      GUM_ERROR(UndefinedElement, "the database contains no column whose name is " << name)
    }


    /// returns the indices of the columns whose name is passed in argument
    template < typename T_DATA >
    INLINE typename IDatabaseTable< T_DATA >::template DBVector< std::size_t >
       IDatabaseTable< T_DATA >::columnsFromVariableName(const std::string& name) const {
      const std::size_t       size = variable_names_.size();
      DBVector< std::size_t > cols;
      for (std::size_t i = 0; i < size; ++i)
        if (variable_names_[i] == name) cols.push_back(i);

      if (cols.empty())
        GUM_ERROR(UndefinedElement, "the database contains no column whose name is " << name)

      return cols;
    }


    // returns the number of variables (columns) of the database
    template < typename T_DATA >
    INLINE std::size_t IDatabaseTable< T_DATA >::nbVariables() const noexcept {
      return variable_names_.size();
    }


    // returns the number of records in the database
    template < typename T_DATA >
    INLINE std::size_t IDatabaseTable< T_DATA >::size() const noexcept {
      return rows_.size();
    }


    // returns the number of records in the database
    template < typename T_DATA >
    INLINE std::size_t IDatabaseTable< T_DATA >::nbRows() const noexcept {
      return rows_.size();
    }


    // indicates whether the database contains some records or not
    template < typename T_DATA >
    INLINE bool IDatabaseTable< T_DATA >::empty() const noexcept {
      return rows_.empty();
    }


    // update the handlers when the size of the database changes
    template < typename T_DATA >
    void IDatabaseTable< T_DATA >::_updateHandlers_(std::size_t new_size) const {
      const std::size_t db_size = rows_.size();

      _safe_handlers_mutex_.lock();
      for (auto handler: _list_of_safe_handlers_) {
        if ((handler->_end_index_ == db_size) || (handler->_end_index_ > new_size)) {
          handler->_end_index_ = new_size;
          // there is no need to update the index because, in safe handlers,
          // we always check that the index is less than end_index when trying
          // to access the rows
        }
      }
      _safe_handlers_mutex_.unlock();

      // update the end iterators
      _end_->_index_          = new_size;
      _end_->_end_index_      = new_size;
      _end_safe_->_index_     = new_size;
      _end_safe_->_end_index_ = new_size;
    }


    // attach a new handler to the database
    template < typename T_DATA >
    INLINE void IDatabaseTable< T_DATA >::_attachHandler_(HandlerSafe* handler) const {
      _safe_handlers_mutex_.lock();
      _list_of_safe_handlers_.push_back(handler);
      _safe_handlers_mutex_.unlock();
    }


    // detach a handler
    template < typename T_DATA >
    void IDatabaseTable< T_DATA >::_detachHandler_(HandlerSafe* handler) const {
      _safe_handlers_mutex_.lock();

      for (auto iter = _list_of_safe_handlers_.rbegin(); iter != _list_of_safe_handlers_.rend();
           ++iter) {
        if (*iter == handler) {
          *iter = _list_of_safe_handlers_.back();
          _list_of_safe_handlers_.pop_back();
          break;
        }
      }

      _safe_handlers_mutex_.unlock();
    }


    // checks whether a new row has the same size as the rest of the database
    template < typename T_DATA >
    INLINE bool IDatabaseTable< T_DATA >::isRowSizeOK_(const std::size_t size) const {
      return (size == variable_names_.size());
    }


    // insert a new DBRow at the end of the database
    template < typename T_DATA >
    INLINE void IDatabaseTable< T_DATA >::insertRow(
       const typename IDatabaseTable< T_DATA >::template Row< T_DATA >& row,
       const typename IDatabaseTable< T_DATA >::IsMissing               contains_missing) {
      // this will call the insertRow with a Row< T_DATA >&&
      this->insertRow(typename IDatabaseTable< T_DATA >::template Row< T_DATA >(row),
                      contains_missing);
    }


    // insert a new DBRow at the end of the database
    template < typename T_DATA >
    void IDatabaseTable< T_DATA >::insertRow(
       typename IDatabaseTable< T_DATA >::template Row< T_DATA >&& new_row,
       const typename IDatabaseTable< T_DATA >::IsMissing          contains_missing) {
      // check that the size of the row is the same as the rest of the database
      if (!isRowSizeOK_(new_row.size()))
        GUM_ERROR(SizeError,
                  "the new row is of size " << new_row.size()
                                            << ", which is different from the number of columns "
                                            << "of the database, i.e., " << variable_names_.size());

      _updateHandlers_(rows_.size() + 1);
      rows_.push_back(std::move(new_row));
      try {
        has_row_missing_val_.push_back(contains_missing);
      } catch (...) {
        rows_.pop_back();
        throw;
      }
    }


    // insert a set of new DBRow at the end of the database
    template < typename T_DATA >
    void IDatabaseTable< T_DATA >::insertRows(
       typename IDatabaseTable< T_DATA >::template Matrix< T_DATA >&& new_rows,
       const typename IDatabaseTable< T_DATA >::template DBVector<
          typename IDatabaseTable< T_DATA >::IsMissing >& rows_have_missing_vals) {
      if (new_rows.empty()) return;

      // check that the missing values indicators vector has the same size
      // as the new rows
      if (rows_have_missing_vals.size() != new_rows.size())
        GUM_ERROR(SizeError,
                  "the number of new rows (i.e., "
                     << new_rows.size()
                     << ") is different from the number of missing values indicators ("
                     << rows_have_missing_vals.size());

      // check that all the rows have the same size
      const std::size_t new_size = new_rows[0].size();

      for (const auto& row: new_rows) {
        if (row.size() != new_size) {
          GUM_ERROR(SizeError, "all the new rows do not have the same number of columns")
        }
      }

      // check that the sizes of the new rows are the same as the rest of
      // the database
      if (!isRowSizeOK_(new_size)) {
        GUM_ERROR(SizeError,
                  "the new rows have " << new_size
                                       << " columns, which is different from the number of columns "
                                       << "of the database, i.e., " << variable_names_.size());
      }

      const std::size_t nb_new_rows = new_rows.size();
      const std::size_t new_db_size = rows_.size() + nb_new_rows;

      rows_.reserve(new_db_size);
      has_row_missing_val_.reserve(new_db_size);

      for (std::size_t i = std::size_t(0); i < nb_new_rows; ++i) {
        rows_.push_back(std::move(new_rows[i]));
        has_row_missing_val_.push_back(rows_have_missing_vals[i]);
      }

      _updateHandlers_(new_db_size);
    }


    // insert a set of new DBRow at the end of the database
    template < typename T_DATA >
    void IDatabaseTable< T_DATA >::insertRows(
       const typename IDatabaseTable< T_DATA >::template Matrix< T_DATA >& new_rows,
       const typename IDatabaseTable< T_DATA >::template DBVector<
          typename IDatabaseTable< T_DATA >::IsMissing >& rows_have_missing_vals) {
      if (new_rows.empty()) return;

      // check that the missing values indicators vector has the same size
      // as the new rows
      if (rows_have_missing_vals.size() != new_rows.size())
        GUM_ERROR(SizeError,
                  "the number of new rows (i.e., "
                     << new_rows.size()
                     << ") is different from the number of missing values indicators ("
                     << rows_have_missing_vals.size());

      // check that all the rows have the same size
      const std::size_t new_size = new_rows[0].size();

      for (const auto& row: new_rows) {
        if (row.size() != new_size) {
          GUM_ERROR(SizeError, "all the new rows do not have the same number of columns")
        }
      }

      // check that the sizes of the new rows are the same as the rest of
      // the database
      std::size_t db_size = rows_.size();

      if (!isRowSizeOK_(new_size)) {
        GUM_ERROR(SizeError,
                  "the new rows have " << new_size
                                       << " columns, which is different from the number of columns "
                                       << "of the database, i.e., " << variable_names_.size());
      }

      const std::size_t nb_new_rows = new_rows.size();
      const std::size_t new_db_size = rows_.size() + nb_new_rows;

      rows_.reserve(new_db_size);
      has_row_missing_val_.reserve(new_db_size);

      for (std::size_t i = std::size_t(0); i < nb_new_rows; ++i) {
        rows_.push_back(new_rows[i]);
        has_row_missing_val_.push_back(rows_have_missing_vals[i]);
      }

      _updateHandlers_(db_size);
    }


    // erase a given row
    template < typename T_DATA >
    INLINE void IDatabaseTable< T_DATA >::eraseRow(std::size_t index) {
      const std::size_t db_size = rows_.size();

      if (index < db_size) {
        _updateHandlers_(db_size - 1);
        rows_.erase(rows_.begin() + index);
        has_row_missing_val_.erase(has_row_missing_val_.begin() + index);
      }
    }


    // erase the last row
    template < typename T_DATA >
    INLINE void IDatabaseTable< T_DATA >::eraseLastRow() {
      const std::size_t db_size = rows_.size();

      if (db_size) {
        _updateHandlers_(db_size - 1);
        rows_.pop_back();
        has_row_missing_val_.pop_back();
      }
    }


    // erase the first row
    template < typename T_DATA >
    INLINE void IDatabaseTable< T_DATA >::eraseFirstRow() {
      const std::size_t db_size = rows_.size();

      if (db_size) {
        _updateHandlers_(db_size - 1);
        rows_.erase(rows_.begin());
        has_row_missing_val_.erase(has_row_missing_val_.begin());
      }
    }


    // erase all the rows
    template < typename T_DATA >
    INLINE void IDatabaseTable< T_DATA >::eraseAllRows() {
      _updateHandlers_(0);
      rows_.clear();
      has_row_missing_val_.clear();
    }


    // erase the k first rows
    template < typename T_DATA >
    INLINE void IDatabaseTable< T_DATA >::eraseFirstRows(const std::size_t nb_rows) {
      const std::size_t db_size = rows_.size();

      if (nb_rows >= db_size) {
        eraseAllRows();
      } else {
        _updateHandlers_(db_size - nb_rows);
        rows_.erase(rows_.begin(), rows_.begin() + nb_rows);
        has_row_missing_val_.erase(has_row_missing_val_.begin(),
                                   has_row_missing_val_.begin() + nb_rows);
      }
    }


    // erase the k last rows
    template < typename T_DATA >
    INLINE void IDatabaseTable< T_DATA >::eraseLastRows(const std::size_t nb_rows) {
      const std::size_t db_size = rows_.size();

      if (nb_rows >= db_size) {
        eraseAllRows();
      } else {
        _updateHandlers_(db_size - nb_rows);
        rows_.erase(rows_.begin() + (db_size - nb_rows), rows_.begin() + db_size);
        has_row_missing_val_.erase(has_row_missing_val_.begin() + (db_size - nb_rows),
                                   has_row_missing_val_.begin() + db_size);
      }
    }


    // erase the rows from the debth to the endth (not included)
    template < typename T_DATA >
    INLINE void IDatabaseTable< T_DATA >::eraseRows(std::size_t deb, std::size_t end) {
      if (deb > end) std::swap(deb, end);

      const std::size_t db_size = rows_.size();

      if (end >= db_size) {
        if (deb >= db_size) {
          return;
        } else {
          eraseLastRows(db_size - deb);
        }
      } else {
        _updateHandlers_(db_size - (end - deb));
        rows_.erase(rows_.begin() + deb, rows_.begin() + end);
        has_row_missing_val_.erase(has_row_missing_val_.begin() + deb,
                                   has_row_missing_val_.begin() + end);
      }
    }


    // erase the content of the database, including the names of the variables
    template < typename T_DATA >
    INLINE void IDatabaseTable< T_DATA >::clear() {
      _updateHandlers_(0);
      rows_.clear();
      has_row_missing_val_.clear();
      variable_names_.clear();
    }


    // returns the set of symbols for the missing values
    template < typename T_DATA >
    INLINE const std::vector< std::string >& IDatabaseTable< T_DATA >::missingSymbols() const {
      return missing_symbols_;
    }


    /// changes the max number of threads that a database can use
    template < typename T_DATA >
    void IDatabaseTable< T_DATA >::setMaxNbThreads(const std::size_t nb) const {
      if (nb == std::size_t(0)) max_nb_threads_ = std::size_t(1);
      else max_nb_threads_ = nb;
    }


    /// returns the number of threads used to parse the database
    template < typename T_DATA >
    INLINE std::size_t IDatabaseTable< T_DATA >::nbThreads() const {
      return max_nb_threads_;
    }


    /** @brief changes the number min of rows a thread should process in a
     * multithreading context */
    template < typename T_DATA >
    void IDatabaseTable< T_DATA >::setMinNbRowsPerThread(const std::size_t nb) const {
      if (nb == std::size_t(0)) min_nb_rows_per_thread_ = std::size_t(1);
      else min_nb_rows_per_thread_ = nb;
    }


    /// returns the minimum of rows that each thread should process
    template < typename T_DATA >
    INLINE std::size_t IDatabaseTable< T_DATA >::minNbRowsPerThread() const {
      return min_nb_rows_per_thread_;
    }


    /// returns the number of threads used to process the current database content
    template < typename T_DATA >
    std::size_t IDatabaseTable< T_DATA >::nbProcessingThreads_() const {
      const std::size_t db_size    = nbRows();
      std::size_t       nb_threads = db_size / min_nb_rows_per_thread_;
      if (nb_threads < 1) nb_threads = 1;
      else if (nb_threads > max_nb_threads_) nb_threads = max_nb_threads_;

      return nb_threads;
    }


    /// returns the ranges that threads should process
    template < typename T_DATA >
    std::vector< std::pair< std::size_t, std::size_t > >
       IDatabaseTable< T_DATA >::rangesProcessingThreads_(const std::size_t nb_threads) const {
      std::vector< std::pair< std::size_t, std::size_t > > ranges;
      const std::size_t                                    db_size = nbRows();
      std::size_t nb_rows_per_thread                               = db_size / nb_threads;
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
        ranges.push_back(std::pair< std::size_t, std::size_t >(begin_index, end_index));
        begin_index = end_index;
      }

      return ranges;
    }


    /// assign a given weight to all the rows of the database
    template < typename T_DATA >
    void IDatabaseTable< T_DATA >::setAllRowsWeight(const double new_weight) {
      // determine the number of threads to use and the rows they should process
      const std::size_t nb_threads = nbProcessingThreads_();
      const std::vector< std::pair< std::size_t, std::size_t > > ranges
         = rangesProcessingThreads_(nb_threads);

      // perform the assignment:
      // we create the lambda that will be executed by all the threads
      auto threadedAssign = [this, &ranges, new_weight](const std::size_t this_thread,
                                                        const std::size_t nb_threads) -> void {
        const std::size_t begin_index = ranges[this_thread].first;
        const std::size_t end_index   = ranges[this_thread].second;

        for (std::size_t i = begin_index; i < end_index; ++i) {
          this->rows_[i].setWeight(new_weight);
        }
      };

      // launch the threads
      ThreadExecutor::execute(nb_threads, threadedAssign);
    }

    /// assigns a given weight to the ith row of the database
    template < typename T_DATA >
    void IDatabaseTable< T_DATA >::setWeight(const std::size_t i, const double weight) {
      // check that i is less than the number of rows
      const std::size_t dbsize = nbRows();
      if (i >= dbsize) {
        GUM_ERROR(OutOfBounds,
                  "it is impossible to set the weight of record #"
                     << i << " because the database contains only " << nbRows() << " records");
      }

      // check that the weight is positive
      if (weight < 0) {
        GUM_ERROR(OutOfBounds,
                  "it is impossible to set " << weight << " as a weight of record #" << i
                                             << "  because this weight is negative");
      }

      rows_[i].setWeight(weight);
    }


    /// returns the weight of the ith record
    template < typename T_DATA >
    double IDatabaseTable< T_DATA >::weight(const std::size_t i) const {
      // check that i is less than the number of rows
      const std::size_t dbsize = nbRows();
      if (i >= dbsize) {
        GUM_ERROR(OutOfBounds,
                  "it is impossible to get the weight of record #"
                     << i << " because the database contains only " << nbRows() << " records");
      }

      return rows_[i].weight();
    }


    /// returns the weight of the whole database
    template < typename T_DATA >
    double IDatabaseTable< T_DATA >::weight() const {
      double w = 0.0;
      for (const auto& row: rows_)
        w += row.weight();
      return w;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
