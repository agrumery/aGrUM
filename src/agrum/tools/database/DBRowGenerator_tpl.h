/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
    template < template < typename > class ALLOC >
    DBRowGenerator< ALLOC >::DBRowGenerator(
       const std::vector< DBTranslatedValueType, ALLOC< DBTranslatedValueType > > column_types,
       const DBRowGeneratorGoal                                                   goal,
       const typename DBRowGenerator< ALLOC >::allocator_type&                    alloc) :
        column_types_(column_types, alloc),
        columns_of_interest_(alloc), goal_(goal) {
      GUM_CONSTRUCTOR(DBRowGenerator);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DBRowGenerator< ALLOC >::DBRowGenerator(
       const DBRowGenerator< ALLOC >&                          from,
       const typename DBRowGenerator< ALLOC >::allocator_type& alloc) :
        nb_remaining_output_rows_(from.nb_remaining_output_rows_),
        column_types_(from.column_types_, alloc),
        columns_of_interest_(from.columns_of_interest_, alloc), goal_(from.goal_) {
      GUM_CONS_CPY(DBRowGenerator);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE DBRowGenerator< ALLOC >::DBRowGenerator(const DBRowGenerator< ALLOC >& from) :
        DBRowGenerator< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DBRowGenerator< ALLOC >::DBRowGenerator(
       DBRowGenerator< ALLOC >&&                               from,
       const typename DBRowGenerator< ALLOC >::allocator_type& alloc) :
        nb_remaining_output_rows_(from.nb_remaining_output_rows_),
        column_types_(std::move(from.column_types_), alloc),
        columns_of_interest_(std::move(from.columns_of_interest_), alloc), goal_(from.goal_) {
      GUM_CONS_MOV(DBRowGenerator);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE DBRowGenerator< ALLOC >::DBRowGenerator(DBRowGenerator< ALLOC >&& from) :
        DBRowGenerator< ALLOC >(std::move(from), from.getAllocator()) {}


    /// destructor
    template < template < typename > class ALLOC >
    INLINE DBRowGenerator< ALLOC >::~DBRowGenerator() {
      GUM_DESTRUCTOR(DBRowGenerator);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE DBRowGenerator< ALLOC >&
       DBRowGenerator< ALLOC >::operator=(const DBRowGenerator< ALLOC >& from) {
      nb_remaining_output_rows_ = from.nb_remaining_output_rows_;
      column_types_             = from.column_types_;
      columns_of_interest_      = from.columns_of_interest_;
      goal_                     = from.goal_;
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE DBRowGenerator< ALLOC >&
       DBRowGenerator< ALLOC >::operator=(DBRowGenerator< ALLOC >&& from) {
      nb_remaining_output_rows_ = from.nb_remaining_output_rows_;
      column_types_             = std::move(from.column_types_);
      columns_of_interest_      = std::move(from.columns_of_interest_);
      goal_                     = from.goal_;
      return *this;
    }


    /// returns true if there are still rows that can be output by the RowFilter
    template < template < typename > class ALLOC >
    INLINE bool DBRowGenerator< ALLOC >::hasRows() {
      return nb_remaining_output_rows_ != std::size_t(0);
    }


    /// sets the input row from which the generator will create new rows
    template < template < typename > class ALLOC >
    INLINE bool DBRowGenerator< ALLOC >::setInputRow(const DBRow< DBTranslatedValue, ALLOC >& row) {
      nb_remaining_output_rows_ = computeRows_(row);
      return hasRows();
    }


    /// decrease the number of remaining output rows
    template < template < typename > class ALLOC >
    INLINE void DBRowGenerator< ALLOC >::decreaseRemainingRows() {
      --nb_remaining_output_rows_;
    }


    /// resets the filter
    template < template < typename > class ALLOC >
    INLINE void DBRowGenerator< ALLOC >::reset() {
      nb_remaining_output_rows_ = 0;
    }


    // sets the columns of interest: the output DBRow needs only
    // contain values fot these columns
    template < template < typename > class ALLOC >
    INLINE void DBRowGenerator< ALLOC >::setColumnsOfInterest(
       const std::vector< std::size_t, ALLOC< std::size_t > >& cols_of_interest) {
      columns_of_interest_ = cols_of_interest;
    }


    // sets the columns of interest: the output DBRow needs only
    // contain values fot these columns
    template < template < typename > class ALLOC >
    INLINE void DBRowGenerator< ALLOC >::setColumnsOfInterest(
       std::vector< std::size_t, ALLOC< std::size_t > >&& cols_of_interest) {
      columns_of_interest_ = std::move(cols_of_interest);
    }


    /// returns the current set of columns of interest
    template < template < typename > class ALLOC >
    INLINE const std::vector< std::size_t, ALLOC< std::size_t > >&
                 DBRowGenerator< ALLOC >::columnsOfInterest() const {
      return columns_of_interest_;
    }


    /// returns the allocator used
    template < template < typename > class ALLOC >
    typename DBRowGenerator< ALLOC >::allocator_type DBRowGenerator< ALLOC >::getAllocator() const {
      return columns_of_interest_.get_allocator();
    }


    /// returns the goal of the DBRowGenerator
    template < template < typename > class ALLOC >
    INLINE DBRowGeneratorGoal DBRowGenerator< ALLOC >::goal() const {
      return goal_;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
