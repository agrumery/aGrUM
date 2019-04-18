
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
 * @brief A filtered row generator that returns exactly the rows it gets in
 *input
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <agrum/learning/database/DBRowGeneratorIdentity.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// returns the allocator used
    template < template < typename > class ALLOC >
    INLINE typename DBRowGeneratorIdentity< ALLOC >::allocator_type
       DBRowGeneratorIdentity< ALLOC >::getAllocator() const {
      return DBRowGenerator< ALLOC >::getAllocator();
    }


    /// default constructor
    template < template < typename > class ALLOC >
    DBRowGeneratorIdentity< ALLOC >::DBRowGeneratorIdentity(
       const std::vector< DBTranslatedValueType, ALLOC< DBTranslatedValueType > >
                                                                       column_types,
       const typename DBRowGeneratorIdentity< ALLOC >::allocator_type& alloc) :
        DBRowGenerator< ALLOC >(
           column_types,
           DBRowGeneratorGoal::OTHER_THINGS_THAN_REMOVE_MISSING_VALUES,
           alloc) {
      GUM_CONSTRUCTOR(DBRowGeneratorIdentity);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBRowGeneratorIdentity< ALLOC >::DBRowGeneratorIdentity(
       const DBRowGeneratorIdentity< ALLOC >&                          from,
       const typename DBRowGeneratorIdentity< ALLOC >::allocator_type& alloc) :
        DBRowGenerator< ALLOC >(from, alloc),
        __input_row(from.__input_row) {
      GUM_CONS_CPY(DBRowGeneratorIdentity);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    DBRowGeneratorIdentity< ALLOC >::DBRowGeneratorIdentity(
       const DBRowGeneratorIdentity< ALLOC >& from) :
        DBRowGeneratorIdentity< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    DBRowGeneratorIdentity< ALLOC >::DBRowGeneratorIdentity(
       DBRowGeneratorIdentity< ALLOC >&&                               from,
       const typename DBRowGeneratorIdentity< ALLOC >::allocator_type& alloc) :
        DBRowGenerator< ALLOC >(std::move(from), alloc),
        __input_row(from.__input_row) {
      GUM_CONS_MOV(DBRowGeneratorIdentity);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    DBRowGeneratorIdentity< ALLOC >::DBRowGeneratorIdentity(
       DBRowGeneratorIdentity< ALLOC >&& from) :
        DBRowGeneratorIdentity< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBRowGeneratorIdentity< ALLOC >* DBRowGeneratorIdentity< ALLOC >::clone(
       const typename DBRowGeneratorIdentity< ALLOC >::allocator_type& alloc)
       const {
      ALLOC< DBRowGeneratorIdentity< ALLOC > > allocator(alloc);
      DBRowGeneratorIdentity< ALLOC >*         generator = allocator.allocate(1);
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
    DBRowGeneratorIdentity< ALLOC >*
       DBRowGeneratorIdentity< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    DBRowGeneratorIdentity< ALLOC >::~DBRowGeneratorIdentity() {
      GUM_DESTRUCTOR(DBRowGeneratorIdentity);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    DBRowGeneratorIdentity< ALLOC >& DBRowGeneratorIdentity< ALLOC >::
                                     operator=(const DBRowGeneratorIdentity< ALLOC >& from) {
      DBRowGenerator< ALLOC >::operator=(from);
      __input_row = from.__input_row;
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    DBRowGeneratorIdentity< ALLOC >& DBRowGeneratorIdentity< ALLOC >::
                                     operator=(DBRowGeneratorIdentity< ALLOC >&& from) {
      DBRowGenerator< ALLOC >::operator=(std::move(from));
      __input_row = from.__input_row;
      return *this;
    }


    /// generates new lines from those the generator gets in input
    template < template < typename > class ALLOC >
    INLINE const DBRow< DBTranslatedValue, ALLOC >&
                 DBRowGeneratorIdentity< ALLOC >::generate() {
      this->decreaseRemainingRows();
      return *__input_row;
    }


    /// computes the rows it will provide in output
    template < template < typename > class ALLOC >
    INLINE std::size_t DBRowGeneratorIdentity< ALLOC >::_computeRows(
       const DBRow< DBTranslatedValue, ALLOC >& row) {
      __input_row = &row;
      return std::size_t(1);
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
