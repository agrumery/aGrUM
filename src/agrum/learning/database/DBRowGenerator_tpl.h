
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <agrum/learning/database/DBRowGenerator.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template < template < typename > class ALLOC >
    DBRowGenerator< ALLOC >::DBRowGenerator(
       const std::vector< DBTranslatedValueType, ALLOC< DBTranslatedValueType > >
                                                               column_types,
       const DBRowGeneratorGoal                                goal,
       const typename DBRowGenerator< ALLOC >::allocator_type& alloc) :
        _column_types(column_types, alloc),
        _columns_of_interest(alloc), _goal(goal) {
      GUM_CONSTRUCTOR(DBRowGenerator);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DBRowGenerator< ALLOC >::DBRowGenerator(
       const DBRowGenerator< ALLOC >&                          from,
       const typename DBRowGenerator< ALLOC >::allocator_type& alloc) :
        _nb_remaining_output_rows(from._nb_remaining_output_rows),
        _column_types(from._column_types, alloc),
        _columns_of_interest(from._columns_of_interest, alloc), _goal(from._goal) {
      GUM_CONS_CPY(DBRowGenerator);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE DBRowGenerator< ALLOC >::DBRowGenerator(
       const DBRowGenerator< ALLOC >& from) :
        DBRowGenerator< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DBRowGenerator< ALLOC >::DBRowGenerator(
       DBRowGenerator< ALLOC >&&                               from,
       const typename DBRowGenerator< ALLOC >::allocator_type& alloc) :
        _nb_remaining_output_rows(from._nb_remaining_output_rows),
        _column_types(std::move(from._column_types), alloc),
        _columns_of_interest(std::move(from._columns_of_interest), alloc),
        _goal(from._goal) {
      GUM_CONS_MOV(DBRowGenerator);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE
       DBRowGenerator< ALLOC >::DBRowGenerator(DBRowGenerator< ALLOC >&& from) :
        DBRowGenerator< ALLOC >(std::move(from), from.getAllocator()) {}


    /// destructor
    template < template < typename > class ALLOC >
    INLINE DBRowGenerator< ALLOC >::~DBRowGenerator() {
      GUM_DESTRUCTOR(DBRowGenerator);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE DBRowGenerator< ALLOC >& DBRowGenerator< ALLOC >::
                                    operator=(const DBRowGenerator< ALLOC >& from) {
      _nb_remaining_output_rows = from._nb_remaining_output_rows;
      _column_types = from._column_types;
      _columns_of_interest = from._columns_of_interest;
      _goal = from._goal;
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE DBRowGenerator< ALLOC >& DBRowGenerator< ALLOC >::
                                    operator=(DBRowGenerator< ALLOC >&& from) {
      _nb_remaining_output_rows = from._nb_remaining_output_rows;
      _column_types = std::move(from._column_types);
      _columns_of_interest = std::move(from._columns_of_interest);
      _goal = from._goal;
      return *this;
    }


    /// returns true if there are still rows that can be output by the RowFilter
    template < template < typename > class ALLOC >
    INLINE bool DBRowGenerator< ALLOC >::hasRows() {
      return _nb_remaining_output_rows != std::size_t(0);
    }


    /// sets the input row from which the generator will create new rows
    template < template < typename > class ALLOC >
    INLINE bool DBRowGenerator< ALLOC >::setInputRow(
       const DBRow< DBTranslatedValue, ALLOC >& row) {
      _nb_remaining_output_rows = _computeRows(row);
      return hasRows();
    }


    /// decrease the number of remaining output rows
    template < template < typename > class ALLOC >
    INLINE void DBRowGenerator< ALLOC >::decreaseRemainingRows() {
      --_nb_remaining_output_rows;
    }


    /// resets the filter
    template < template < typename > class ALLOC >
    INLINE void DBRowGenerator< ALLOC >::reset() {
      _nb_remaining_output_rows = 0;
    }


    // sets the columns of interest: the output DBRow needs only
    // contain values fot these columns
    template < template < typename > class ALLOC >
    INLINE void DBRowGenerator< ALLOC >::setColumnsOfInterest(
       const std::vector< std::size_t, ALLOC< std::size_t > >& cols_of_interest) {
      _columns_of_interest = cols_of_interest;
    }


    // sets the columns of interest: the output DBRow needs only
    // contain values fot these columns
    template < template < typename > class ALLOC >
    INLINE void DBRowGenerator< ALLOC >::setColumnsOfInterest(
       std::vector< std::size_t, ALLOC< std::size_t > >&& cols_of_interest) {
      _columns_of_interest = std::move(cols_of_interest);
    }


    /// returns the current set of columns of interest
    template < template < typename > class ALLOC >
    INLINE const std::vector< std::size_t, ALLOC< std::size_t > >&
                 DBRowGenerator< ALLOC >::columnsOfInterest() const {
      return _columns_of_interest;
    }


    /// returns the allocator used
    template < template < typename > class ALLOC >
    typename DBRowGenerator< ALLOC >::allocator_type
       DBRowGenerator< ALLOC >::getAllocator() const {
      return _columns_of_interest.get_allocator();
    }


    /// returns the goal of the DBRowGenerator
    template < template < typename > class ALLOC >
    INLINE DBRowGeneratorGoal DBRowGenerator< ALLOC >::goal() const {
      return _goal;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
