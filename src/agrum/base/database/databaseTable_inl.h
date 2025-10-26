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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/base/database/databaseTable.h>

namespace gum {

  namespace learning {

    // copy constructor
    INLINE DatabaseTable::DatabaseTable(const DatabaseTable& from) :
        IDatabaseTable< DBTranslatedValue >(from), _translators_(from._translators_),
        _ignored_cols_(from._ignored_cols_) {
      GUM_CONS_CPY(DatabaseTable);
    }

    // move constructor
    INLINE DatabaseTable::DatabaseTable(DatabaseTable&& from) noexcept :
        IDatabaseTable< DBTranslatedValue >(std::move(from)),
        _translators_(std::move(from._translators_)),
        _ignored_cols_(std::move(from._ignored_cols_)) {
      GUM_CONS_MOV(DatabaseTable);
    }

    // destructor
    INLINE DatabaseTable::~DatabaseTable() { GUM_DESTRUCTOR(DatabaseTable); }

    /** @brief returns the indices corresponding either to the kth translator
     * or to all those that parse the kth column of the input dataset
     *
     * @warning the indices are sorted by deacreasing order */
    INLINE typename DatabaseTable::template DBVector< std::size_t >
        DatabaseTable::_getKthIndices_(const std::size_t k, const bool k_is_input_col) const {
      const std::size_t nb_trans = _translators_.size();
      if (!k_is_input_col) {
        if (k < nb_trans) return DBVector< std::size_t >{k};
        else return DBVector< std::size_t >();
      } else {
        DBVector< std::size_t > trans;
        for (std::size_t i = std::size_t(0), kk = nb_trans - 1; i < nb_trans; ++i, --kk) {
          if (_translators_.inputColumn(kk) == k) trans.push_back(kk);
        }
        return trans;
      }
    }

    /// returns the set of translators
    INLINE const DBTranslatorSet& DatabaseTable::translatorSet() const { return _translators_; }

    /** @brief returns the index corresponding either to the kth translator or
     * to that of the first translator parsing the kth column of the
     * input dataset */
    INLINE std::size_t DatabaseTable::_getKthIndex_(const std::size_t k,
                                                    const bool        k_is_input_col) const {
      if (k_is_input_col) {
        const std::size_t nb_trans = _translators_.size();
        for (std::size_t i = std::size_t(0); i < nb_trans; ++i) {
          if (_translators_.inputColumn(i) == k) { return i; }
        }
        return nb_trans + 1;
      } else {
        return k;
      }
    }

    /// returns the domain sizes of all the variables in the database table
    INLINE std::vector< std::size_t > DatabaseTable::domainSizes() const {
      const std::size_t          nb_trans = _translators_.size();
      std::vector< std::size_t > dom(nb_trans);
      for (std::size_t i = std::size_t(0); i < nb_trans; ++i) {
        dom[i] = _translators_.domainSize(i);
      }
      return dom;
    }

    /// performs a reordering of all the columns
    INLINE void DatabaseTable::reorder() {
      const std::size_t nb_trans = _translators_.size();
      for (std::size_t i = std::size_t(0); i < nb_trans; ++i)
        reorder(i, false);
    }

    /// insert a new DBRow at the end of the database
    INLINE void DatabaseTable::insertRow(
        typename DatabaseTable::template Row< DBTranslatedValue >&& new_row,
        const typename DatabaseTable::IsMissing                     contains_missing_data) {
      // check that the new rows values are compatible with the values of
      // the variables stored within the translators
      if (!_isRowCompatible_(new_row)) {
        if (new_row.size() != _translators_.size()) {
          GUM_ERROR(SizeError,
                    "The new row has " << new_row.size()
                                       << " elements whereas the database table has "
                                       << _translators_.size() << " columns")
        } else {
          GUM_ERROR(InvalidArgument, "the new row is not compatible with the current translators")
        }
      }

      IDatabaseTable< DBTranslatedValue >::insertRow(std::move(new_row), contains_missing_data);
    }

    /// insert a new row at the end of the database
    INLINE void DatabaseTable::insertRow(
        const typename DatabaseTable::template Row< DBTranslatedValue >& new_row,
        const typename DatabaseTable::IsMissing                          contains_missing_data) {
      // check that the new rows values are compatible with the values of
      // the variables stored within the translators
      if (!_isRowCompatible_(new_row)) {
        if (new_row.size() != _translators_.size()) {
          GUM_ERROR(SizeError,
                    "The new row has " << new_row.size()
                                       << " elements whereas the database table has "
                                       << _translators_.size() << " columns")
        } else {
          GUM_ERROR(InvalidArgument, "the new row is not compatible with the current translators")
        }
      }

      IDatabaseTable< DBTranslatedValue >::insertRow(new_row, contains_missing_data);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
