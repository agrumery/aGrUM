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
 * @brief The class representing database filtered rows
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_FILTERED_ROW_H
#define GUM_LEARNING_FILTERED_ROW_H

#include <vector>
#include <agrum/config.h>

namespace gum {

  namespace learning {

    /** @class FilteredRow
     * @ingroup learning_group
     * @brief the class representing database rows that have been filtered (by
     * cell translators) and their weight.
     *
     * Once filtered, database cells are encoded as integers. The FilterRow is
     * thus essentially a container for a vector of integers. But to each row is
     * assigned a weight, which accounts for the weight of the row's observation
     * during the learning. Usually, this weight is equal to 1 but some row
     * generators may change this value. For instance, Expected-Maximization (EM)
     * generators will most certainly assign weights that are real number strictly
     * smaller than 1. Finally, database rows may have unobserved columns. The
     * FilteredRow also accounts for that and contains a field counting the number
     * of unobserved values. This can be used, for instance, by generators that
     * discard non-fully observed rows.
     */
    class FilteredRow {
      public:
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// returns the filtered row as a vector of integers
      std::vector<unsigned int> &row() noexcept { return __row; }

      /// returns a reference on the weight of the row
      float &weight() noexcept { return __weight; }

      /// returns the weight of the row
      float weight() const noexcept { return __weight; }

      /// returns a reference on the number of unobserved values
      unsigned int &unobserved() noexcept { return __nb_unobserved; }

      /// returns the number of unobserved values
      unsigned int unobserved() const noexcept { return __nb_unobserved; }

      /// returns the value at the ith column
      /** by convention, unobserved columns should have value
       * std::numeric_limits<unsigned int>::max () */
      unsigned int operator[](unsigned int i) const noexcept { return __row[i]; }

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// tests whether two rows are identical
      bool operator==(const FilteredRow &from) {
        return ((__weight == from.__weight) &&
                (__nb_unobserved == from.__nb_unobserved) && (__row == from.__row));
      }

      /// tests whether two rows are different
      bool operator!=(const FilteredRow &from) { return !operator==(from); }

      /// @}

      private:
      /// the integer-encoded filtered row
      std::vector<unsigned int> __row;

      /// the weight of the row
      float __weight{1};

      /// the number of unobserved values
      unsigned int __nb_unobserved{0};
    };

  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_FILTERED_ROW_H */
