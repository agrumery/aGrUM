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
 * @brief Template implementation of FilteredRowGeneratorSet
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <typename Generator, typename... OtherGenerators>
    INLINE
    FilteredRowGeneratorSet<Generator, OtherGenerators...>::FilteredRowGeneratorSet(
        const Generator &first_generator,
        const OtherGenerators &... next_generators) noexcept
        : NextGenerators(next_generators...),
          __first_generator(first_generator) {
      GUM_CONSTRUCTOR(FilteredRowGeneratorSet);
    }

    /// copy constructor
    template <typename Generator, typename... OtherGenerators>
    INLINE
    FilteredRowGeneratorSet<Generator, OtherGenerators...>::FilteredRowGeneratorSet(
        const FilteredRowGeneratorSet<Generator, OtherGenerators...> &from)
        : NextGenerators(from), __first_generator(from.__first_generator) {
      GUM_CONS_CPY(FilteredRowGeneratorSet);
    }

    /// move constructor
    template <typename Generator, typename... OtherGenerators>
    INLINE
    FilteredRowGeneratorSet<Generator, OtherGenerators...>::FilteredRowGeneratorSet(
        FilteredRowGeneratorSet<Generator, OtherGenerators...> &&from) noexcept
        : NextGenerators(std::move(from)),
          __first_generator(std::move(from.__first_generator)) {
      GUM_CONS_MOV(FilteredRowGeneratorSet);
    }

    /// destructor
    template <typename Generator, typename... OtherGenerators>
    INLINE FilteredRowGeneratorSet<
        Generator, OtherGenerators...>::~FilteredRowGeneratorSet() noexcept {
      GUM_DESTRUCTOR(FilteredRowGeneratorSet);
    }

    /// copy operator
    template <typename Generator, typename... OtherGenerators>
    INLINE FilteredRowGeneratorSet<Generator, OtherGenerators...> &
        FilteredRowGeneratorSet<Generator, OtherGenerators...>::
        operator=(
            const FilteredRowGeneratorSet<Generator, OtherGenerators...> &from) {
      if (this != &from) {
        NextGenerators::operator=(from);
        __first_generator = from.__first_generator;
      }
      return *this;
    }

    /// move operator
    template <typename Generator, typename... OtherGenerators>
    INLINE FilteredRowGeneratorSet<Generator, OtherGenerators...> &
        FilteredRowGeneratorSet<Generator, OtherGenerators...>::
        operator=(FilteredRowGeneratorSet<Generator, OtherGenerators...> &&from) {
      if (this != &from) {
        NextGenerators::operator=(std::move(from));
        __first_generator = std::move(from.__first_generator);
      }
      return *this;
    }

    /// returns true if there are still rows that can be output by the RowFilter
    template <typename Generator, typename... OtherGenerators>
    INLINE bool
    FilteredRowGeneratorSet<Generator, OtherGenerators...>::hasRows() noexcept {
      if (NextGenerators::hasRows())
        return true;
      while (__first_generator.hasRows()) {
        if (NextGenerators::setInputRow(__first_generator.generate())) {
          return true;
        }
      }
      return false;
    }

    /// sets the input row from which the generator will create new rows
    template <typename Generator, typename... OtherGenerators>
    INLINE bool FilteredRowGeneratorSet<Generator, OtherGenerators...>::setInputRow(
        FilteredRow &row) noexcept {
      __first_generator.setInputRow(row);
      while (__first_generator.hasRows()) {
        if (NextGenerators::setInputRow(__first_generator.generate())) {
          return true;
        }
      }

      return false;
    }

    /// generate new rows from the input row
    template <typename Generator, typename... OtherGenerators>
    INLINE FilteredRow &
    FilteredRowGeneratorSet<Generator, OtherGenerators...>::generate() {
      if (!NextGenerators::hasRows() && __first_generator.hasRows())
        NextGenerators::setInputRow(__first_generator.generate());
      return NextGenerators::generate();
    }

    /// resets the filter
    template <typename Generator, typename... OtherGenerators>
    INLINE void FilteredRowGeneratorSet<Generator, OtherGenerators...>::reset() {
      __first_generator.reset();
      NextGenerators::reset();
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
