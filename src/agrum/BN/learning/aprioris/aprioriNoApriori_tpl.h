
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @brief the no a priori class: corresponds to 0 weight-sample
 *
 * @author Christophe GONZALES (@AMU) and Pierre-Henri WUILLEMIN (@LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori< ALLOC >::AprioriNoApriori(
       const DatabaseTable< ALLOC >& database,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                                 nodeId2columns,
       const typename AprioriNoApriori< ALLOC >::allocator_type& alloc) :
        Apriori< ALLOC >(database, nodeId2columns, alloc) {
      Apriori< ALLOC >::setWeight(0.0);
      GUM_CONSTRUCTOR(AprioriNoApriori);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori< ALLOC >::AprioriNoApriori(
       const AprioriNoApriori< ALLOC >&                          from,
       const typename AprioriNoApriori< ALLOC >::allocator_type& alloc) :
        Apriori< ALLOC >(from, alloc) {
      GUM_CONS_CPY(AprioriNoApriori);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori< ALLOC >::AprioriNoApriori(
       const AprioriNoApriori< ALLOC >& from) :
        AprioriNoApriori< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori< ALLOC >::AprioriNoApriori(
       AprioriNoApriori< ALLOC >&&                               from,
       const typename AprioriNoApriori< ALLOC >::allocator_type& alloc) :
        Apriori< ALLOC >(std::move(from), alloc) {
      GUM_CONS_MOV(AprioriNoApriori);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori< ALLOC >::AprioriNoApriori(
       AprioriNoApriori< ALLOC >&& from) :
        AprioriNoApriori< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    AprioriNoApriori< ALLOC >* AprioriNoApriori< ALLOC >::clone(
       const typename AprioriNoApriori< ALLOC >::allocator_type& alloc) const {
      ALLOC< AprioriNoApriori< ALLOC > > allocator(alloc);
      AprioriNoApriori< ALLOC >*         apriori = allocator.allocate(1);
      try {
        allocator.construct(apriori, *this, alloc);
      } catch (...) {
        allocator.deallocate(apriori, 1);
        throw;
      }

      return apriori;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori< ALLOC >* AprioriNoApriori< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori< ALLOC >::~AprioriNoApriori() {
      GUM_DESTRUCTOR(AprioriNoApriori);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori< ALLOC >& AprioriNoApriori< ALLOC >::operator=(
       const AprioriNoApriori< ALLOC >& from) {
      Apriori< ALLOC >::operator=(from);
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori< ALLOC >&
       AprioriNoApriori< ALLOC >::operator=(AprioriNoApriori< ALLOC >&& from) {
      Apriori< ALLOC >::operator=(std::move(from));
      return *this;
    }


    /// sets the weight of the a priori
    template < template < typename > class ALLOC >
    INLINE void AprioriNoApriori< ALLOC >::setWeight(const double) {}


    /// indicates whether an apriori is of a certain type
    template < template < typename > class ALLOC >
    INLINE bool AprioriNoApriori< ALLOC >::isOfType(const std::string& type) {
      return AprioriNoAprioriType::isOfType(type);
    }


    /// returns the type of the apriori
    template < template < typename > class ALLOC >
    INLINE const std::string& AprioriNoApriori< ALLOC >::getType() const {
      return AprioriNoAprioriType::type;
    }


    /// indicates whether the apriori is potentially informative
    template < template < typename > class ALLOC >
    INLINE bool AprioriNoApriori< ALLOC >::isInformative() const {
      return false;
    }


    /// returns the apriori vector all the variables in the idset
    template < template < typename > class ALLOC >
    INLINE void AprioriNoApriori< ALLOC >::addAllApriori(
       const IdCondSet< ALLOC >&                   idset,
       std::vector< double, ALLOC< double > >& counts) {}


    /// returns the apriori vector over only the conditioning set of an idset
    template < template < typename > class ALLOC >
    INLINE void AprioriNoApriori< ALLOC >::addConditioningApriori(
       const IdCondSet< ALLOC >&                   idset,
       std::vector< double, ALLOC< double > >& counts) {}


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */