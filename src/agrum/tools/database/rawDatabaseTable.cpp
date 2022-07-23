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
 * @brief The table containing the raw/original data of a database
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/tools/database/rawDatabaseTable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/database/rawDatabaseTable_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    // virtual copy constructor
    RawDatabaseTable* RawDatabaseTable::clone() const { return new RawDatabaseTable(*this); }


    // copy operator
    RawDatabaseTable& RawDatabaseTable::operator=(const RawDatabaseTable& from) {
      if (this != &from) {
        IDatabaseTable< DBCell >::operator=(from);
        _ignored_cols_ = from._ignored_cols_;
      }
      return *this;
    }


    // move constructor
    RawDatabaseTable& RawDatabaseTable::operator=(RawDatabaseTable&& from) {
      if (this != &from) {
        IDatabaseTable< DBCell >::operator=(std::move(from));
        _ignored_cols_ = std::move(from._ignored_cols_);
      }
      return *this;
    }


    // sets the names of the variables
    void RawDatabaseTable::setVariableNames(const std::vector< std::string >& names,
                                            const bool from_external_object) {
      const std::size_t size              = names.size();
      const std::size_t ignored_cols_size = _ignored_cols_.size();

      if (!from_external_object || !ignored_cols_size) {
        if (this->rows_.empty() || (size == this->rows_[0].size())) {
          this->variable_names_ = names;
        } else {
          GUM_ERROR(SizeError,
                    "the number of variable's names (i.e., "
                       << size << ") does not correspond to the number of columns of the "
                       << "raw database table (i.e.," << this->rows_[0].size() << ")");
        }
      } else {
        // check that the size of the names vector (after removing the ignored
        // columns) is the same as the rest of the database
        std::size_t ignored_size = std::size_t(0);

        // find the number of ignored cols
        for (auto iter = _ignored_cols_.rbegin(), rend = _ignored_cols_.rend(); iter != rend;
             ++iter, ++ignored_size) {
          if (*iter < size) { break; }
        }
        ignored_size = ignored_cols_size - ignored_size;

        if (this->rows_.empty() || (size == this->rows_[0].size() + ignored_size)) {
          DBVector< std::string > new_names;
          for (std::size_t i = std::size_t(0), j = std::size_t(0); i < size; ++i) {
            if (i != _ignored_cols_[j]) {
              new_names.push_back(names[i]);
            } else {
              if (++j == ignored_cols_size) {
                for (++i; i < size; ++i) {
                  new_names.push_back(names[i]);
                }
              }
            }
          }
          this->variable_names_ = std::move(new_names);
          return;
        } else {
          GUM_ERROR(SizeError,
                    "the number of variable's names excluding the ignored "
                       << "columns (i.e., " << (size - ignored_size)
                       << ") does not correspond to the number of columns of the "
                       << "raw database table (i.e.," << this->rows_[0].size() << ")");
        }
      }
    }


    /// makes the database table ignore from now on the kth column
    void RawDatabaseTable::ignoreColumn(const std::size_t k, const bool from_external_object) {
      // first, compute the value that k would have in an external database
      // and compute where the new value should be inserted
      std::size_t       i;          // where to insert the new k into the ignored colums
      std::size_t       kk   = k;   // kk = k value for an external database
      const std::size_t size = _ignored_cols_.size();

      if (from_external_object) {
        for (i = std::size_t(0); i < size; ++i) {
          if (k <= _ignored_cols_[i]) {
            if (k == _ignored_cols_[i]) return;
            break;
          }
        }
      } else {
        for (i = std::size_t(0); i < size; ++i, ++kk) {
          if (kk <= _ignored_cols_[i]) {
            if (kk == _ignored_cols_[i]) return;
            break;
          }
        }
      }

      // the column of  _rows_ and variable_names_ impacted by the ignoreColumn
      // operation is therefore equal to kk-i. So, we should check that such
      // a column exists and, if so, we should remove the column from  _rows_
      // and from variable_names_. Note that if there is no more variable,
      //  _rows_ should become empty
      const std::size_t col = kk - i;
      if (col < this->variable_names_.size()) {
        this->variable_names_.erase(this->variable_names_.begin() + col);
        if (this->variable_names_.empty()) {
          IDatabaseTable< DBCell >::eraseAllRows();
        } else {
          const std::size_t nb_rows = this->rows_.size();
          if (nb_rows != std::size_t(0)) {
            const std::size_t nb_cols = this->rows_[0].size();
            for (std::size_t i = std::size_t(0); i < nb_rows; ++i) {
              auto& row = this->rows_[i].row();
              if (this->has_row_missing_val_[i] == IsMissing::True) {
                bool has_missing_val = false;
                for (std::size_t j = std::size_t(0); j < nb_cols; ++j) {
                  if ((j != col) && row[j].isMissing()) {
                    has_missing_val = true;
                    break;
                  }
                }
                if (!has_missing_val) this->has_row_missing_val_[i] = IsMissing::False;
              }
              row.erase(row.begin() + col);
            }
          }
        }
      }

      // here, we know that we should insert kk at the ith index of  _ignored_cols_
      _ignored_cols_.push_back(std::size_t(0));
      for (std::size_t j = size; j > i; --j)
        _ignored_cols_[j] = _ignored_cols_[j - 1];
      _ignored_cols_[i] = kk;
    }


    /// returns the set of columns parsed
    const typename RawDatabaseTable::template DBVector< std::size_t >
       RawDatabaseTable::inputColumns() const {
      const auto& data = IDatabaseTable< DBCell >::content();
      if (data.empty()) { return DBVector< std::size_t >(); }

      const std::size_t       size              = data[0].size();
      const std::size_t       ignored_cols_size = _ignored_cols_.size();
      DBVector< std::size_t > cols(size);

      if (!ignored_cols_size) {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          cols[i] = i;
        }
      } else {
        // fill the cols vector with consecutive values, excluding the
        // ignored columns
        std::size_t i = std::size_t(0);   // the consecutive values
        std::size_t k = std::size_t(0);   // the index in col where we save values
        std::size_t j = std::size_t(0);   // the index to parse the ignored columns
        while (true) {
          if (i != _ignored_cols_[j]) {
            cols[k] = i;
            if (++k == size) break;
          } else {
            if (++j == ignored_cols_size) {
              for (++i; k < size; ++i, ++k) {
                cols[k] = i;
              }
              break;
            }
          }
          ++i;
        }
      }

      return cols;
    }


    // insert a new row at the end of the database
    void RawDatabaseTable::insertRow(const std::vector< std::string >& new_row) {
      // check that the size of the row (after removing the ignored columns) is
      // the same as the rest of the database
      const std::size_t row_size          = new_row.size();
      const std::size_t ignored_cols_size = _ignored_cols_.size();
      std::size_t       ignored_size      = std::size_t(0);
      if (ignored_cols_size) {
        // find the number of ignored cols
        for (auto iter = _ignored_cols_.rbegin(), rend = _ignored_cols_.rend(); iter != rend;
             ++iter, ++ignored_size) {
          if (*iter < row_size) { break; }
        }
        ignored_size = ignored_cols_size - ignored_size;
      }

      if (!this->isRowSizeOK_(row_size - ignored_size)) {
        GUM_ERROR(SizeError,
                  "the new row has " << (row_size - ignored_size)
                                     << " elements whereas the raw database table has "
                                     << this->variable_names_.size() << " columns");
      }

      // create the dbrow that will contain the new data
      Row< DBCell > dbrow;
      dbrow.reserve(row_size - ignored_size);
      bool has_missing_val = false;

      // translate the row into T_data and put them into the newly created dbrow
      if (ignored_size == 0) {
        for (const auto& elt: new_row) {
          const DBCell new_cell(this->_convert_(elt));
          if (new_cell.isMissing()) has_missing_val = true;
          dbrow.pushBack(new_cell);
        }
      } else {
        for (std::size_t i = std::size_t(0), j = std::size_t(0); i < row_size; ++i) {
          if (i != _ignored_cols_[j]) {
            const DBCell new_cell(this->_convert_(new_row[i]));
            if (new_cell.isMissing()) has_missing_val = true;
            dbrow.pushBack(new_cell);
          } else {
            if (++j == ignored_size) {
              for (++i; i < row_size; ++i) {
                const DBCell new_cell(this->_convert_(new_row[i]));
                if (new_cell.isMissing()) has_missing_val = true;
                dbrow.pushBack(new_cell);
              }
            }
          }
        }
      }

      IDatabaseTable< DBCell >::insertRow(std::move(dbrow),
                                          has_missing_val ? IsMissing::True : IsMissing::False);
    }


    // erase the content of the database, including the names of the variables
    void RawDatabaseTable::clear() {
      _ignored_cols_.clear();
      IDatabaseTable< DBCell >::clear();
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
