
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
 * @brief A DBRowGenerator class that returns the rows that are complete
 * (fully observed) w.r.t. the nodes of interest
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <agrum/learning/database/DBRowGenerator4CompleteRows.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// returns the allocator used
    template < template < typename > class ALLOC >
    INLINE typename DBRowGenerator4CompleteRows< ALLOC >::allocator_type
       DBRowGenerator4CompleteRows< ALLOC >::getAllocator() const {
      return DBRowGenerator< ALLOC >::getAllocator();
    }


    /// default constructor
    template < template < typename > class ALLOC >
    DBRowGenerator4CompleteRows< ALLOC >::DBRowGenerator4CompleteRows(
       const std::vector< DBTranslatedValueType, ALLOC< DBTranslatedValueType > >
          column_types,
       const typename DBRowGenerator4CompleteRows< ALLOC >::allocator_type&
          alloc) :
        DBRowGenerator< ALLOC >(
           column_types, DBRowGeneratorGoal::ONLY_REMOVE_MISSING_VALUES, alloc) {
      GUM_CONSTRUCTOR(DBRowGenerator4CompleteRows);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBRowGenerator4CompleteRows< ALLOC >::DBRowGenerator4CompleteRows(
       const DBRowGenerator4CompleteRows< ALLOC >& from,
       const typename DBRowGenerator4CompleteRows< ALLOC >::allocator_type&
          alloc) :
        DBRowGenerator< ALLOC >(from, alloc),
        __input_row(from.__input_row) {
      GUM_CONS_CPY(DBRowGenerator4CompleteRows);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    DBRowGenerator4CompleteRows< ALLOC >::DBRowGenerator4CompleteRows(
       const DBRowGenerator4CompleteRows< ALLOC >& from) :
        DBRowGenerator4CompleteRows< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    DBRowGenerator4CompleteRows< ALLOC >::DBRowGenerator4CompleteRows(
       DBRowGenerator4CompleteRows< ALLOC >&& from,
       const typename DBRowGenerator4CompleteRows< ALLOC >::allocator_type&
          alloc) :
        DBRowGenerator< ALLOC >(std::move(from), alloc),
        __input_row(from.__input_row) {
      GUM_CONS_MOV(DBRowGenerator4CompleteRows);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    DBRowGenerator4CompleteRows< ALLOC >::DBRowGenerator4CompleteRows(
       DBRowGenerator4CompleteRows< ALLOC >&& from) :
        DBRowGenerator4CompleteRows< ALLOC >(std::move(from),
                                             from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBRowGenerator4CompleteRows< ALLOC >*
       DBRowGenerator4CompleteRows< ALLOC >::clone(
          const typename DBRowGenerator4CompleteRows< ALLOC >::allocator_type&
             alloc) const {
      ALLOC< DBRowGenerator4CompleteRows< ALLOC > > allocator(alloc);
      DBRowGenerator4CompleteRows< ALLOC >* generator = allocator.allocate(1);
      try {
        allocator.construct(generator, *this, alloc);
      } catch (...) {
        allocator.deallocate(generator, 1);
        throw;
      }
      return generator;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    DBRowGenerator4CompleteRows< ALLOC >*
       DBRowGenerator4CompleteRows< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    DBRowGenerator4CompleteRows< ALLOC >::~DBRowGenerator4CompleteRows() {
      GUM_DESTRUCTOR(DBRowGenerator4CompleteRows);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    DBRowGenerator4CompleteRows< ALLOC >& DBRowGenerator4CompleteRows< ALLOC >::
                                          operator=(const DBRowGenerator4CompleteRows< ALLOC >& from) {
      DBRowGenerator< ALLOC >::operator=(from);
      __input_row = from.__input_row;
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    DBRowGenerator4CompleteRows< ALLOC >& DBRowGenerator4CompleteRows< ALLOC >::
                                          operator=(DBRowGenerator4CompleteRows< ALLOC >&& from) {
      DBRowGenerator< ALLOC >::operator=(std::move(from));
      __input_row = from.__input_row;
      return *this;
    }


    /// generates new lines from those the generator gets in input
    template < template < typename > class ALLOC >
    INLINE const DBRow< DBTranslatedValue, ALLOC >&
                 DBRowGenerator4CompleteRows< ALLOC >::generate() {
      this->decreaseRemainingRows();
      return *__input_row;
    }


    /// computes the rows it will provide in output
    template < template < typename > class ALLOC >
    INLINE std::size_t DBRowGenerator4CompleteRows< ALLOC >::_computeRows(
       const DBRow< DBTranslatedValue, ALLOC >& row) {
      // check that all the values are observed
      const auto& xrow = row.row();
      for (const auto col : this->_columns_of_interest) {
        switch (this->_column_types[col]) {
          case DBTranslatedValueType::DISCRETE:
            if (xrow[col].discr_val == std::numeric_limits< std::size_t >::max()) {
              __input_row = nullptr;
              return std::size_t(0);
            }
            break;

          case DBTranslatedValueType::CONTINUOUS:
            if (xrow[col].cont_val == std::numeric_limits< float >::max()) {
              __input_row = nullptr;
              return std::size_t(0);
            }
            break;

          default:
            GUM_ERROR(NotImplementedYet,
                      "DBTranslatedValueType " << int(this->_column_types[col])
                                               << " is not supported yet");
            break;
        }
      }
      __input_row = &row;
      return std::size_t(1);
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
