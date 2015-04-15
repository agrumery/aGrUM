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
 * @brief Template implementation of FilteredRowGenerator
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE FilteredRowGenerator::FilteredRowGenerator() noexcept {
      GUM_CONSTRUCTOR(FilteredRowGenerator);
    }

    /// copy constructor
    INLINE FilteredRowGenerator::FilteredRowGenerator(
        const FilteredRowGenerator &from) noexcept
        : _input_row(from._input_row),
          _nb_remaining_output_rows{from._nb_remaining_output_rows} {
      GUM_CONS_CPY(FilteredRowGenerator);
    }

    /// move constructor
    INLINE
    FilteredRowGenerator::FilteredRowGenerator(FilteredRowGenerator &&from) noexcept
        : _input_row(from._input_row),
          _nb_remaining_output_rows{from._nb_remaining_output_rows} {
      GUM_CONS_MOV(FilteredRowGenerator);
    }

    /// destructor
    INLINE FilteredRowGenerator::~FilteredRowGenerator() noexcept {
      GUM_DESTRUCTOR(FilteredRowGenerator);
    }

    /// copy operator
    INLINE FilteredRowGenerator &FilteredRowGenerator::
    operator=(const FilteredRowGenerator &from) noexcept {
      _input_row = from._input_row;
      _nb_remaining_output_rows = from._nb_remaining_output_rows;
      return *this;
    }

    /// move operator
    INLINE FilteredRowGenerator &FilteredRowGenerator::
    operator=(FilteredRowGenerator &&from) noexcept {
      _input_row = from._input_row;
      _nb_remaining_output_rows = from._nb_remaining_output_rows;
      return *this;
    }

    /// returns true if there are still rows that can be output by the RowFilter
    INLINE bool FilteredRowGenerator::hasRows() noexcept {
      return _nb_remaining_output_rows;
    }

    /// sets the input row from which the generator will create new rows
    INLINE void FilteredRowGenerator::setInputRow(FilteredRow &row) noexcept {
      _input_row = &row;
      _nb_remaining_output_rows = _computeRows();
    }

    /// decrease the number of remaining output rows
    INLINE void FilteredRowGenerator::decreaseRemainingRows() noexcept {
      --_nb_remaining_output_rows;
    }

    /// resets the filter
    INLINE void FilteredRowGenerator::reset() { _nb_remaining_output_rows = 0; }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
