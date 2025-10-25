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
 * @brief The implementation of raw tabular databases stored in memory (RAM)
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/base/database/rawDatabaseTable.h>

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
