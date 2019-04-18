
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
 * @brief the base class for all a prioris
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template < template < typename > class ALLOC >
    INLINE Apriori< ALLOC >::Apriori(
       const DatabaseTable< ALLOC >& database,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                        nodeId2columns,
       const typename Apriori< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        _database(&database), _nodeId2columns(nodeId2columns) {
      GUM_CONSTRUCTOR(Apriori);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE Apriori< ALLOC >::Apriori(
       const Apriori< ALLOC >&                          from,
       const typename Apriori< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        _weight(from._weight), _database(from._database),
        _nodeId2columns(from._nodeId2columns) {
      GUM_CONS_CPY(Apriori);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE Apriori< ALLOC >::Apriori(const Apriori< ALLOC >& from) :
        Apriori(from, from.getAllocator()) {}


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE Apriori< ALLOC >::Apriori(
       Apriori< ALLOC >&&                               from,
       const typename Apriori< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        _weight(from._weight), _database(from._database),
        _nodeId2columns(std::move(from._nodeId2columns)) {
      GUM_CONS_MOV(Apriori);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE Apriori< ALLOC >::Apriori(Apriori< ALLOC >&& from) :
        Apriori(std::move(from), from.getAllocator()) {}


    /// destructor
    template < template < typename > class ALLOC >
    INLINE Apriori< ALLOC >::~Apriori() {
      GUM_DESTRUCTOR(Apriori);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    Apriori< ALLOC >& Apriori< ALLOC >::operator=(const Apriori< ALLOC >& from) {
      if (this != &from) {
        _nodeId2columns = from._nodeId2columns;
        _weight = from._weight;
        _database = from._database;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    Apriori< ALLOC >& Apriori< ALLOC >::operator=(Apriori< ALLOC >&& from) {
      if (this != &from) {
        _nodeId2columns = std::move(from._nodeId2columns);
        _weight = from._weight;
        _database = from._database;
      }
      return *this;
    }


    /// sets the weight of the a priori (kind of effective sample size)
    template < template < typename > class ALLOC >
    INLINE void Apriori< ALLOC >::setWeight(const double weight) {
      if (weight < 0.0) {
        GUM_ERROR(OutOfBounds,
                  "A negative weight (" << weight
                                        << ") is forbidden for an apriori");
      }
      _weight = weight;
    }


    /// returns the weight assigned to the apriori
    template < template < typename > class ALLOC >
    INLINE double Apriori< ALLOC >::weight() const {
      return _weight;
    }


    /// returns the allocator used by the translator
    template < template < typename > class ALLOC >
    INLINE typename Apriori< ALLOC >::allocator_type
       Apriori< ALLOC >::getAllocator() const {
      return *this;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
