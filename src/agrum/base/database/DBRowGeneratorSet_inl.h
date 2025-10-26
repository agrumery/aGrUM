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

#  include <agrum/base/database/DBRowGeneratorSet.h>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE DBRowGeneratorSet::DBRowGeneratorSet() { GUM_CONSTRUCTOR(DBRowGeneratorSet); }

    /// move constructor
    INLINE DBRowGeneratorSet::DBRowGeneratorSet(DBRowGeneratorSet&& from) :
        _generators_(std::move(from._generators_)), _nb_generators_(from._nb_generators_),
        _output_row_(from._output_row_),
        _setInputRow_performed_(std::move(from._setInputRow_performed_)) {
      GUM_CONS_MOV(DBRowGeneratorSet);
    }

    /// returns the ith generator
    INLINE DBRowGenerator& DBRowGeneratorSet::operator[](const std::size_t i) {
      return *(_generators_[i]);
    }

    /// returns the ith generator
    INLINE const DBRowGenerator& DBRowGeneratorSet::operator[](const std::size_t i) const {
      return *(_generators_[i]);
    }

    /// returns the number of generators
    INLINE std::size_t DBRowGeneratorSet::nbGenerators() const noexcept { return _nb_generators_; }

    /// returns the number of generators (alias for nbGenerators)
    INLINE std::size_t DBRowGeneratorSet::size() const noexcept { return _nb_generators_; }

    /// returns true if there are still rows that can be output by the RowFilter
    INLINE bool DBRowGeneratorSet::hasRows() { return _output_row_ != nullptr; }

    /// sets the input row from which the generator will create new rows
    INLINE bool DBRowGeneratorSet::setInputRow(const DBRow< DBTranslatedValue >& input_row) {
      // reset all the generators: ask them to use method setInputDBRow
      if (hasRows())
        for (auto& performed: _setInputRow_performed_)
          performed = 0;

      // now, parse the row generation tree
      return _produceNextRow_(&input_row, std::size_t(0));
    }

    /// generate new rows from the input row
    INLINE const DBRow< DBTranslatedValue >& DBRowGeneratorSet::generate() {
      // get the row that we should return
      const auto row = _output_row_;

      // we should ask the last generator to produce the next output row
      _produceNextRow_(_output_row_, _nb_generators_ - 1);

      return *row;
    }

    /// resets the filter
    INLINE void DBRowGeneratorSet::reset() {
      for (auto gen: _generators_)
        gen->reset();
      for (auto& performed: _setInputRow_performed_)
        performed = 0;
      _output_row_ = nullptr;
    }

    /// sets the columns of interest: the output DBRow needs only
    /// contain values fot these columns
    INLINE void DBRowGeneratorSet::setColumnsOfInterest(
        const std::vector< std::size_t >& cols_of_interest) {
      // check that no output row generation is still active
      if (_output_row_ != nullptr)
        GUM_ERROR(OperationNotAllowed,
                  "you cannot change the columns of interest while a "
                  "generation is still being processed");
      for (auto gen: _generators_)
        gen->setColumnsOfInterest(cols_of_interest);
    }

    /// sets the columns of interest: the output DBRow needs only
    /// contain values fot these columns
    INLINE void
        DBRowGeneratorSet::setColumnsOfInterest(std::vector< std::size_t >&& cols_of_interest) {
      if (_output_row_ != nullptr)
        GUM_ERROR(OperationNotAllowed,
                  "you cannot change the columns of interest while a "
                  "generation is still being processed");
      for (auto gen: _generators_)
        gen->setColumnsOfInterest(cols_of_interest);
    }

    /// returns the current set of columns of interest
    INLINE const std::vector< std::size_t >& DBRowGeneratorSet::columnsOfInterest() const {
      if (_nb_generators_ == std::size_t(0)) {
        GUM_ERROR(UndefinedElement,
                  "there are no generators yet in the generator set, so "
                  "there are no columns of interest");
      }
      return _generators_[0]->columnsOfInterest();
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
