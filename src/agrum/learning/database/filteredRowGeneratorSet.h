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
 * @brief class for packing sets of generators into a row filter
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_FILTERED_ROW_GENERATOR_SET_H
#define GUM_LEARNING_FILTERED_ROW_GENERATOR_SET_H

#include <agrum/config.h>
#include <agrum/learning/database/filteredRow.h>
#include <agrum/learning/database/filteredRowGenerator.h>

namespace gum {

  namespace learning {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    // the general type for sets of filtered row generators
    template <typename... Generators> class FilteredRowGeneratorSet;

    // the end of the recurive definition of sets of filtered row generators
    template <> class FilteredRowGeneratorSet<> {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      FilteredRowGeneratorSet() noexcept {}

      /// copy constructor
      FilteredRowGeneratorSet(const FilteredRowGeneratorSet<> &from) noexcept
          : __input_row(from.__input_row) {}

      /// move constructor
      FilteredRowGeneratorSet(FilteredRowGeneratorSet<> &&from) noexcept
          : __input_row(from.__input_row) {}

      /// destructor
      ~FilteredRowGeneratorSet() noexcept {}

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      FilteredRowGeneratorSet<> &operator=(const FilteredRowGeneratorSet<> &from) {
        __input_row = from.__input_row;
        return *this;
      }

      /// move operator
      FilteredRowGeneratorSet<> &operator=(FilteredRowGeneratorSet<> &&from) {
        __input_row = from.__input_row;
        return *this;
      }

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// indicates whether there are still rows to generate
      bool hasRows() noexcept { return __input_row != nullptr; }

      /// sets the filtered row used as input to generate new output filtered rows
      bool setInputRow(FilteredRow &row) noexcept {
        __input_row = &row;
        return true;
      }

      /// clear the sets of rows to generate
      void reset() { __input_row = nullptr; }

      /// generates a new row and returns it
      FilteredRow &generate() {
        FilteredRow &row = *__input_row;
        reset();
        return row;
      }

      /// @}

      private:
      FilteredRow *__input_row{nullptr};
    };

#endif // DOXYGEN_SHOULD_SKIP_THIS

    /** @class FilteredRowGeneratorSet
     * @ingroup learning_group
     * @brief The class used to pack generators into a row filter
     *
     * Several generators can be used in sequence to generate new filtered rows
     * from that which is read from the database. This allows a high degree of
     * flexibility in the reading of the database. For instance, if we wish to
     * learn a dynamic Bayesian network from a database with some unobserved
     * variables that we wish to estimate through an EM algorithm, we just have
     * to pack an EM generator with a time slicer generator: the first one will
     * fill the holes in the database (hence producing several output rows from
     * a single database row) and each produced row will then be time sliced by
     * the second generator, hence resulting in a new set of rows.
     *
     * This class is not intended to be used as is. The user should prefer packing
     * generators into a row filter using the helper function make_generators,
     * which will return a FilteredRowGeneratorSet. The following code shows how
     * to do it:
     * @code
     * auto generators =  make_generators
     *   ( EMGenerator (),
     *     TimeSlicerGenerator () );
     * @endcode
     */
    template <typename Generator, typename... OtherGenerators>
    class FilteredRowGeneratorSet<Generator, OtherGenerators...>
        : public FilteredRowGeneratorSet<OtherGenerators...> {
      public:
      /// the type for the next generators to apply
      using NextGenerators = FilteredRowGeneratorSet<OtherGenerators...>;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      FilteredRowGeneratorSet(const Generator &first_generator,
                              const OtherGenerators &... next_generators) noexcept;

      /// copy constructor
      FilteredRowGeneratorSet(
          const FilteredRowGeneratorSet<Generator, OtherGenerators...> &from);

      /// move constructor
      FilteredRowGeneratorSet(
          FilteredRowGeneratorSet<Generator, OtherGenerators...> &&from) noexcept;

      /// destructor
      ~FilteredRowGeneratorSet() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      FilteredRowGeneratorSet<Generator, OtherGenerators...> &
      operator=(const FilteredRowGeneratorSet<Generator, OtherGenerators...> &);

      /// move operator
      FilteredRowGeneratorSet<Generator, OtherGenerators...> &
      operator=(FilteredRowGeneratorSet<Generator, OtherGenerators...> &&);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// returns true if there are still rows that can be output by the RowFilter
      bool hasRows() noexcept;

      /// sets the input row from which the generator will create new rows
      bool setInputRow(FilteredRow &row) noexcept;

      /// generate new rows from the input row
      FilteredRow &generate();

      /// resets the filter
      void reset();

      /// @}

      private:
      /// the first generator to apply
      Generator __first_generator;
    };

  } /* namespace learning */

} /* namespace gum */

// always include the template implementation
#include <agrum/learning/database/filteredRowGeneratorSet.tcc>

namespace gum {

  namespace learning {

    /// a function to create easily a FilteredRowGeneratorSet
    /** Below is an example of the use of this function. Basically, all the
     * arguments are FilteredRowGenerators
     * @code
     * auto my_gen = make_generators ( DuplicateGenerator (), IdGenerator () );
     * @endcode
     */
    template <typename... Args>
    constexpr FilteredRowGeneratorSet<Args...>
    make_generators(const Args &... args) {
      return FilteredRowGeneratorSet<Args...>(args...);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_FILTERED_ROW_GENERATOR_SET_H */
