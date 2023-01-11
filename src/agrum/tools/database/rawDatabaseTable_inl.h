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
 * @brief The implementation of raw tabular databases stored in memory (RAM)
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/tools/database/rawDatabaseTable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    // default constructor
    INLINE RawDatabaseTable::RawDatabaseTable(
       const typename RawDatabaseTable::MissingValType& missing_symbols,
       const std::vector< std::string >&                var_names) :
        IDatabaseTable< DBCell >(missing_symbols, var_names) {
      GUM_CONSTRUCTOR(RawDatabaseTable);
    }


    // default constructor
    INLINE RawDatabaseTable::RawDatabaseTable(
       const typename RawDatabaseTable::MissingValType& missing_symbols) :
        IDatabaseTable< DBCell >(missing_symbols, std::vector< std::string >()) {
      GUM_CONSTRUCTOR(RawDatabaseTable);
    }


    // default constructor
    INLINE RawDatabaseTable::RawDatabaseTable() :
        IDatabaseTable< DBCell >(std::vector< std::string >(), std::vector< std::string >()) {
      GUM_CONSTRUCTOR(RawDatabaseTable);
    }


    // copy constructor
    INLINE RawDatabaseTable::RawDatabaseTable(const RawDatabaseTable& from) :
        IDatabaseTable< DBCell >(from), _ignored_cols_(from._ignored_cols_) {
      GUM_CONS_CPY(RawDatabaseTable);
    }


    // move constructor
    INLINE RawDatabaseTable::RawDatabaseTable(RawDatabaseTable&& from) :
        IDatabaseTable< DBCell >(std::move(from)), _ignored_cols_(std::move(from._ignored_cols_)) {
      GUM_CONS_MOV(RawDatabaseTable);
    }


    // destructor
    INLINE RawDatabaseTable::~RawDatabaseTable() { GUM_DESTRUCTOR(RawDatabaseTable); }


    /// returns  the set of ignored columns
    INLINE const typename RawDatabaseTable::template DBVector< std::size_t >
       RawDatabaseTable::ignoredColumns() const {
      return _ignored_cols_;
    }


    // translates a string into a DBCell and returns it
    INLINE DBCell RawDatabaseTable::_convert_(const std::string& elt) const {
      return DBCell::bestDBCell(elt, this->missing_symbols_);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
