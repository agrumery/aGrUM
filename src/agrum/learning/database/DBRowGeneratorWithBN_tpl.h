
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
 * @brief A DBRowGenerator class that returns incomplete rows as EM would do
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <agrum/learning/database/DBRowGeneratorIdentity.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// returns the allocator used
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    INLINE typename DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::allocator_type
       DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::getAllocator() const {
      return DBRowGenerator< ALLOC >::getAllocator();
    }


    /// default constructor
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::DBRowGeneratorWithBN(
       const std::vector< DBTranslatedValueType, ALLOC< DBTranslatedValueType > >
                                     column_types,
       const BayesNet< GUM_SCALAR >& bn,
       const DBRowGeneratorGoal      goal,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
          nodeId2columns,
       const typename DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::allocator_type&
          alloc) :
        DBRowGenerator< ALLOC >(column_types, goal, alloc),
        _bn(&bn), _nodeId2columns(nodeId2columns) {
      GUM_CONSTRUCTOR(DBRowGeneratorWithBN);
    }


    /// copy constructor with a given allocator
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::DBRowGeneratorWithBN(
       const DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >& from,
       const typename DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::allocator_type&
          alloc) :
        DBRowGenerator< ALLOC >(from, alloc),
        _bn(from._bn), _nodeId2columns(from._nodeId2columns) {
      GUM_CONS_CPY(DBRowGeneratorWithBN);
    }


    /// copy constructor
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::DBRowGeneratorWithBN(
       const DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >& from) :
        DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::DBRowGeneratorWithBN(
       DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >&& from,
       const typename DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::allocator_type&
          alloc) :
        DBRowGenerator< ALLOC >(std::move(from), alloc),
        _bn(from._bn), _nodeId2columns(std::move(from._nodeId2columns)) {
      GUM_CONS_MOV(DBRowGeneratorWithBN);
    }


    /// move constructor
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::DBRowGeneratorWithBN(
       DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >&& from) :
        DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >(std::move(from),
                                                  from.getAllocator()) {}


    /// destructor
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::~DBRowGeneratorWithBN() {
      GUM_DESTRUCTOR(DBRowGeneratorWithBN);
    }


    /// copy operator
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >&
          DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::
          operator=(const DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >& from) {
      if (this != &from) {
        DBRowGenerator< ALLOC >::operator=(from);
        _bn = from._bn;
        _nodeId2columns = from._nodeId2columns;
      }

      return *this;
    }


    /// move operator
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >&
          DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::
          operator=(DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >&& from) {
      if (this != &from) {
        DBRowGenerator< ALLOC >::operator=(std::move(from));
        _bn = from._bn;
        _nodeId2columns = std::move(from._nodeId2columns);
      }

      return *this;
    }


    /// assign a new Bayes net to the generator
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    INLINE void DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::setBayesNet(
       const BayesNet< GUM_SCALAR >& new_bn) {
      _bn = &new_bn;
    }


    /// returns the Bayes net used by the generator
    template < typename GUM_SCALAR, template < typename > class ALLOC >
    INLINE const BayesNet< GUM_SCALAR >&
                 DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >::getBayesNet() const {
      return *_bn;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
