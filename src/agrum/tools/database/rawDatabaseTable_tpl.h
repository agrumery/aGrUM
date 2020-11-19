/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief The implementation of raw tabular databases stored in memory (RAM)
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <agrum/tools/database/rawDatabaseTable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    // default constructor
    template < template < typename > class ALLOC >
    template < template < typename > class VARALLOC,
               template < typename >
               class MISSALLOC >
    INLINE RawDatabaseTable< ALLOC >::RawDatabaseTable(
       const typename RawDatabaseTable< ALLOC >::template MissingValType<
          MISSALLOC >&                                            missing_symbols,
       const std::vector< std::string, VARALLOC< std::string > >& var_names,
       const typename RawDatabaseTable< ALLOC >::allocator_type&  alloc) :
        IDatabaseTable< DBCell, ALLOC >(missing_symbols, var_names, alloc),
        ignored_cols__(alloc) {
      GUM_CONSTRUCTOR(RawDatabaseTable);
    }


    // default constructor
    template < template < typename > class ALLOC >
    template < template < typename > class MISSALLOC >
    INLINE RawDatabaseTable< ALLOC >::RawDatabaseTable(
       const typename RawDatabaseTable< ALLOC >::template MissingValType<
          MISSALLOC >&                                           missing_symbols,
       const typename RawDatabaseTable< ALLOC >::allocator_type& alloc) :
        IDatabaseTable< DBCell, ALLOC >(
           missing_symbols,
           std::vector< std::string, ALLOC< std::string > >(),
           alloc),
        ignored_cols__(alloc) {
      GUM_CONSTRUCTOR(RawDatabaseTable);
    }


    // default constructor
    template < template < typename > class ALLOC >
    INLINE RawDatabaseTable< ALLOC >::RawDatabaseTable(
       const typename RawDatabaseTable< ALLOC >::allocator_type& alloc) :
        IDatabaseTable< DBCell, ALLOC >(
           std::vector< std::string, ALLOC< std::string > >(),
           std::vector< std::string, ALLOC< std::string > >(),
           alloc),
        ignored_cols__(alloc) {
      GUM_CONSTRUCTOR(RawDatabaseTable);
    }


    // copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE RawDatabaseTable< ALLOC >::RawDatabaseTable(
       const RawDatabaseTable< ALLOC >&                          from,
       const typename RawDatabaseTable< ALLOC >::allocator_type& alloc) :
        IDatabaseTable< DBCell, ALLOC >(from, alloc),
        ignored_cols__(from.ignored_cols__, alloc) {
      GUM_CONS_CPY(RawDatabaseTable);
    }

    // copy constructor
    template < template < typename > class ALLOC >
    INLINE RawDatabaseTable< ALLOC >::RawDatabaseTable(
       const RawDatabaseTable< ALLOC >& from) :
        RawDatabaseTable< ALLOC >(from, from.getAllocator()) {}


    // move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE RawDatabaseTable< ALLOC >::RawDatabaseTable(
       RawDatabaseTable< ALLOC >&&                               from,
       const typename RawDatabaseTable< ALLOC >::allocator_type& alloc) :
        IDatabaseTable< DBCell, ALLOC >(std::move(from), alloc),
        ignored_cols__(std::move(from.ignored_cols__)) {
      GUM_CONS_MOV(RawDatabaseTable);
    }


    // move constructor
    template < template < typename > class ALLOC >
    INLINE RawDatabaseTable< ALLOC >::RawDatabaseTable(
       RawDatabaseTable< ALLOC >&& from) :
        RawDatabaseTable< ALLOC >(std::move(from), from.getAllocator()) {}


    // virtual copy constructor
    template < template < typename > class ALLOC >
    RawDatabaseTable< ALLOC >* RawDatabaseTable< ALLOC >::clone(
       const typename RawDatabaseTable< ALLOC >::allocator_type& alloc) const {
      ALLOC< RawDatabaseTable< ALLOC > > allocator(alloc);
      RawDatabaseTable< ALLOC >*         new_db = allocator.allocate(1);
      try {
        allocator.construct(new_db, *this, alloc);
      } catch (...) {
        allocator.deallocate(new_db, 1);
        throw;
      }

      return new_db;
    }


    // virtual copy constructor
    template < template < typename > class ALLOC >
    RawDatabaseTable< ALLOC >* RawDatabaseTable< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    // destructor
    template < template < typename > class ALLOC >
    INLINE RawDatabaseTable< ALLOC >::~RawDatabaseTable() {
      GUM_DESTRUCTOR(RawDatabaseTable);
    }


    // copy operator
    template < template < typename > class ALLOC >
    RawDatabaseTable< ALLOC >& RawDatabaseTable< ALLOC >::operator=(
       const RawDatabaseTable< ALLOC >& from) {
      if (this != &from) {
        IDatabaseTable< DBCell, ALLOC >::operator=(from);
        ignored_cols__ = from.ignored_cols__;
      }
      return *this;
    }


    // move constructor
    template < template < typename > class ALLOC >
    RawDatabaseTable< ALLOC >&
       RawDatabaseTable< ALLOC >::operator=(RawDatabaseTable< ALLOC >&& from) {
      if (this != &from) {
        IDatabaseTable< DBCell, ALLOC >::operator=(std::move(from));
        ignored_cols__ = std::move(from.ignored_cols__);
      }
      return *this;
    }


    // sets the names of the variables
    template < template < typename > class ALLOC >
    void RawDatabaseTable< ALLOC >::setVariableNames(
       const std::vector< std::string, ALLOC< std::string > >& names,
       const bool from_external_object) {
      const std::size_t size = names.size();
      const std::size_t ignored_cols_size = ignored_cols__.size();

      if (!from_external_object || !ignored_cols_size) {
        if (this->rows_.empty() || (size == this->rows_[0].size())) {
          this->variable_names_ = names;
        } else {
          GUM_ERROR(
             SizeError,
             "the number of variable's names (i.e., "
                << size << ") does not correspond to the number of columns of the "
                << "raw database table (i.e.," << this->rows_[0].size() << ")");
        }
      } else {
        // check that the size of the names vector (after removing the ignored
        // columns) is the same as the rest of the database
        std::size_t ignored_size = std::size_t(0);

        // find the number of ignored cols
        for (auto iter = ignored_cols__.rbegin(), rend = ignored_cols__.rend();
             iter != rend;
             ++iter, ++ignored_size) {
          if (*iter < size) { break; }
        }
        ignored_size = ignored_cols_size - ignored_size;

        if (this->rows_.empty()
            || (size == this->rows_[0].size() + ignored_size)) {
          DBVector< std::string > new_names;
          for (std::size_t i = std::size_t(0), j = std::size_t(0); i < size; ++i) {
            if (i != ignored_cols__[j]) {
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
                       << "raw database table (i.e.," << this->rows_[0].size()
                       << ")");
        }
      }
    }


    /// makes the database table ignore from now on the kth column
    template < template < typename > class ALLOC >
    void RawDatabaseTable< ALLOC >::ignoreColumn(const std::size_t k,
                                                 const bool from_external_object) {
      // first, compute the value that k would have in an external database
      // and compute where the new value should be inserted
      std::size_t       i;   // where to insert the new k into the ignored colums
      std::size_t       kk = k;   // kk = k value for an external database
      const std::size_t size = ignored_cols__.size();

      if (from_external_object) {
        for (i = std::size_t(0); i < size; ++i) {
          if (k <= ignored_cols__[i]) {
            if (k == ignored_cols__[i]) return;
            break;
          }
        }
      } else {
        for (i = std::size_t(0); i < size; ++i, ++kk) {
          if (kk <= ignored_cols__[i]) {
            if (kk == ignored_cols__[i]) return;
            break;
          }
        }
      }

      // the column of rows__ and variable_names_ impacted by the ignoreColumn
      // operation is therefore equal to kk-i. So, we should check that such
      // a column exists and, if so, we should remove the column from rows__
      // and from variable_names_. Note that if there is no more variable,
      // rows__ should become empty
      const std::size_t col = kk - i;
      if (col < this->variable_names_.size()) {
        this->variable_names_.erase(this->variable_names_.begin() + col);
        if (this->variable_names_.empty()) {
          IDatabaseTable< DBCell, ALLOC >::eraseAllRows();
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
                if (!has_missing_val)
                  this->has_row_missing_val_[i] = IsMissing::False;
              }
              row.erase(row.begin() + col);
            }
          }
        }
      }

      // here, we know that we should insert kk at the ith index of ignored_cols__
      ignored_cols__.push_back(std::size_t(0));
      for (std::size_t j = size; j > i; --j)
        ignored_cols__[j] = ignored_cols__[j - 1];
      ignored_cols__[i] = kk;
    }


    /// returns  the set of ignored columns
    template < template < typename > class ALLOC >
    INLINE const typename RawDatabaseTable< ALLOC >::template DBVector<
       std::size_t >
       RawDatabaseTable< ALLOC >::ignoredColumns() const {
      return ignored_cols__;
    }


    /// returns the set of columns parsed
    template < template < typename > class ALLOC >
    const typename RawDatabaseTable< ALLOC >::template DBVector< std::size_t >
       RawDatabaseTable< ALLOC >::inputColumns() const {
      const auto& data = IDatabaseTable< DBCell, ALLOC >::content();
      if (data.empty()) { return DBVector< std::size_t >(); }

      const std::size_t       size = data[0].size();
      const std::size_t       ignored_cols_size = ignored_cols__.size();
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
          if (i != ignored_cols__[j]) {
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


    // translates a string into a DBCell and returns it
    template < template < typename > class ALLOC >
    INLINE DBCell
       RawDatabaseTable< ALLOC >::convert__(const std::string& elt) const {
      return DBCell::bestDBCell(elt, this->missing_symbols_);
    }


    // insert a new row at the end of the database
    template < template < typename > class ALLOC >
    void RawDatabaseTable< ALLOC >::insertRow(
       const std::vector< std::string, ALLOC< std::string > >& new_row) {
      // check that the size of the row (after removing the ignored columns) is
      // the same as the rest of the database
      const std::size_t row_size = new_row.size();
      const std::size_t ignored_cols_size = ignored_cols__.size();
      std::size_t       ignored_size = std::size_t(0);
      if (ignored_cols_size) {
        // find the number of ignored cols
        for (auto iter = ignored_cols__.rbegin(), rend = ignored_cols__.rend();
             iter != rend;
             ++iter, ++ignored_size) {
          if (*iter < row_size) { break; }
        }
        ignored_size = ignored_cols_size - ignored_size;
      }

      if (!this->isRowSizeOK_(row_size - ignored_size)) {
        GUM_ERROR(SizeError,
                  "the new row has "
                     << (row_size - ignored_size)
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
          const DBCell new_cell(this->convert__(elt));
          if (new_cell.isMissing()) has_missing_val = true;
          dbrow.pushBack(new_cell);
        }
      } else {
        for (std::size_t i = std::size_t(0), j = std::size_t(0); i < row_size;
             ++i) {
          if (i != ignored_cols__[j]) {
            const DBCell new_cell(this->convert__(new_row[i]));
            if (new_cell.isMissing()) has_missing_val = true;
            dbrow.pushBack(new_cell);
          } else {
            if (++j == ignored_size) {
              for (++i; i < row_size; ++i) {
                const DBCell new_cell(this->convert__(new_row[i]));
                if (new_cell.isMissing()) has_missing_val = true;
                dbrow.pushBack(new_cell);
              }
            }
          }
        }
      }

      IDatabaseTable< DBCell, ALLOC >::insertRow(
         std::move(dbrow), has_missing_val ? IsMissing::True : IsMissing::False);
    }


    // erase the content of the database, including the names of the variables
    template < template < typename > class ALLOC >
    void RawDatabaseTable< ALLOC >::clear() {
      ignored_cols__.clear();
      IDatabaseTable< DBCell, ALLOC >::clear();
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
