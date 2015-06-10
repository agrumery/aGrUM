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
 * @brief The base class for all filtered row generators
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_FILTERED_ROW_GENERATOR_H
#define GUM_LEARNING_FILTERED_ROW_GENERATOR_H

#include <agrum/config.h>
#include <agrum/learning/database/filteredRow.h>

namespace gum {

  namespace learning {

    /** @class FilteredRowGenerator
     * @ingroup learning_group
     * @brief The base class for all filtered row generators
     *
     * Generators take as input a filtered row resulting either from the
     * translation of the database cells from a set of CellFilters or from other
     * generators, and they output new filtered rows. An identity generator will
     * therefore return only one row which will correspond to the same filtered row
     * as given in input, whereas an EM generator will fill the holes in the row
     * and produce several rows as output (one for each filling possible, with
     * their corresponding weight).
     *
     * All filtered row generators should derive from this class. It takes care
     * of the interaction with the database. As such, the user who wish to create
     * a new generator, say for instance, one that outputs 3 times the input row,
     * just has to define the following class:
     * @code
     * class DuplicateGenerator : public FilteredRowGenerator {
     *   public:
     *   /// returns the next FilteredRow
     *   FilteredRow& generate () {
     *     --__nb;
     *     decreaseRemainingRows ();
     *     return *_input_row;
     *   }
     *
     *   /// the method that computes the set of FilteredRows
     *   unsigned int _computeRows () {
     *     __nb = 0;
     *     return 3;
     *   }
     *
     *   private:
     *   unsigned int __nb { 0 };
     * };
     * @endcode
     */
    class FilteredRowGenerator {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      FilteredRowGenerator() noexcept;

      /// copy constructor
      FilteredRowGenerator(const FilteredRowGenerator &from) noexcept;

      /// move constructor
      FilteredRowGenerator(FilteredRowGenerator &&from) noexcept;

      /// destructor
      virtual ~FilteredRowGenerator() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy constructor
      FilteredRowGenerator &operator=(const FilteredRowGenerator &) noexcept;

      /// move constructor
      FilteredRowGenerator &operator=(FilteredRowGenerator &&) noexcept;

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// returns true if there are still rows that can be output by the RowFilter
      bool hasRows() noexcept;

      /// sets the input row from which the generator will create new rows
      void setInputRow(FilteredRow &row) noexcept;

      /// generate new rows from the input row
      virtual FilteredRow &generate() = 0;

      /// decrease the number of remaining output rows
      void decreaseRemainingRows() noexcept;

      /// the method that computes the set of FilteredRows
      virtual unsigned int _computeRows() = 0;

      /// resets the filter
      virtual void reset();

      /// @}

      protected:
      /// the row used as input to generate the output FilteredRows
      FilteredRow *_input_row{nullptr};

      /// the number of output rows still to retrieve through the generate method
      unsigned int _nb_remaining_output_rows{0};
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/database/filteredRowGenerator.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_FILTERED_ROW_GENERATOR_H */
