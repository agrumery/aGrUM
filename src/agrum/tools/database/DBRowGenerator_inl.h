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
 * @brief Template implementation of DBRowGenerator
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/tools/database/DBRowGenerator.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE DBRowGenerator::DBRowGenerator(const std::vector< DBTranslatedValueType >& column_types,
                                          const DBRowGeneratorGoal                    goal) :
        column_types_(column_types),
        goal_(goal) {
      GUM_CONSTRUCTOR(DBRowGenerator);
    }


    /// copy constructor
    INLINE DBRowGenerator::DBRowGenerator(const DBRowGenerator& from) :
        nb_remaining_output_rows_(from.nb_remaining_output_rows_),
        column_types_(from.column_types_), columns_of_interest_(from.columns_of_interest_),
        goal_(from.goal_) {
      GUM_CONS_CPY(DBRowGenerator);
    }


    /// move constructor
    INLINE DBRowGenerator::DBRowGenerator(DBRowGenerator&& from) :
        nb_remaining_output_rows_(from.nb_remaining_output_rows_),
        column_types_(std::move(from.column_types_)),
        columns_of_interest_(std::move(from.columns_of_interest_)), goal_(from.goal_) {
      GUM_CONS_MOV(DBRowGenerator);
    }


    /// destructor
    INLINE DBRowGenerator::~DBRowGenerator() { GUM_DESTRUCTOR(DBRowGenerator); }


    /// copy operator
    INLINE DBRowGenerator& DBRowGenerator::operator=(const DBRowGenerator& from) {
      nb_remaining_output_rows_ = from.nb_remaining_output_rows_;
      column_types_             = from.column_types_;
      columns_of_interest_      = from.columns_of_interest_;
      goal_                     = from.goal_;
      return *this;
    }


    /// move operator
    INLINE DBRowGenerator& DBRowGenerator::operator=(DBRowGenerator&& from) {
      nb_remaining_output_rows_ = from.nb_remaining_output_rows_;
      column_types_             = std::move(from.column_types_);
      columns_of_interest_      = std::move(from.columns_of_interest_);
      goal_                     = from.goal_;
      return *this;
    }


    /// returns true if there are still rows that can be output by the RowFilter
    INLINE bool DBRowGenerator::hasRows() { return nb_remaining_output_rows_ != std::size_t(0); }


    /// sets the input row from which the generator will create new rows
    INLINE bool DBRowGenerator::setInputRow(const DBRow< DBTranslatedValue >& row) {
      nb_remaining_output_rows_ = computeRows_(row);
      return hasRows();
    }


    /// decrease the number of remaining output rows
    INLINE void DBRowGenerator::decreaseRemainingRows() { --nb_remaining_output_rows_; }


    /// resets the filter
    INLINE void DBRowGenerator::reset() { nb_remaining_output_rows_ = 0; }


    // sets the columns of interest: the output DBRow needs only
    // contain values fot these columns
    INLINE void
       DBRowGenerator::setColumnsOfInterest(const std::vector< std::size_t >& cols_of_interest) {
      columns_of_interest_ = cols_of_interest;
    }


    // sets the columns of interest: the output DBRow needs only
    // contain values fot these columns
    INLINE void
       DBRowGenerator::setColumnsOfInterest(std::vector< std::size_t >&& cols_of_interest) {
      columns_of_interest_ = std::move(cols_of_interest);
    }


    /// returns the current set of columns of interest
    INLINE const std::vector< std::size_t >& DBRowGenerator::columnsOfInterest() const {
      return columns_of_interest_;
    }


    /// returns the goal of the DBRowGenerator
    INLINE DBRowGeneratorGoal DBRowGenerator::goal() const { return goal_; }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
