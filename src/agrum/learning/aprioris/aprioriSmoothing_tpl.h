
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
 * @brief the smooth a priori: adds a weight w to all the countings
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE AprioriSmoothing< ALLOC >::AprioriSmoothing(
       const DatabaseTable< ALLOC >& database,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                                 nodeId2columns,
       const typename AprioriSmoothing< ALLOC >::allocator_type& alloc) :
        Apriori< ALLOC >(database, nodeId2columns, alloc) {
      GUM_CONSTRUCTOR(AprioriSmoothing);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriSmoothing< ALLOC >::AprioriSmoothing(
       const AprioriSmoothing< ALLOC >&                          from,
       const typename AprioriSmoothing< ALLOC >::allocator_type& alloc) :
        Apriori< ALLOC >(from, alloc) {
      GUM_CONS_CPY(AprioriSmoothing);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE AprioriSmoothing< ALLOC >::AprioriSmoothing(
       const AprioriSmoothing< ALLOC >& from) :
        AprioriSmoothing< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriSmoothing< ALLOC >::AprioriSmoothing(
       AprioriSmoothing< ALLOC >&&                               from,
       const typename AprioriSmoothing< ALLOC >::allocator_type& alloc) :
        Apriori< ALLOC >(std::move(from), alloc) {
      GUM_CONS_MOV(AprioriSmoothing);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE AprioriSmoothing< ALLOC >::AprioriSmoothing(
       AprioriSmoothing< ALLOC >&& from) :
        AprioriSmoothing< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    AprioriSmoothing< ALLOC >* AprioriSmoothing< ALLOC >::clone(
       const typename AprioriSmoothing< ALLOC >::allocator_type& alloc) const {
      ALLOC< AprioriSmoothing< ALLOC > > allocator(alloc);
      AprioriSmoothing< ALLOC >*         apriori = allocator.allocate(1);
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
    INLINE AprioriSmoothing< ALLOC >* AprioriSmoothing< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE AprioriSmoothing< ALLOC >::~AprioriSmoothing() {
      GUM_DESTRUCTOR(AprioriSmoothing);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE AprioriSmoothing< ALLOC >& AprioriSmoothing< ALLOC >::
                                      operator=(const AprioriSmoothing< ALLOC >& from) {
      Apriori< ALLOC >::operator=(from);
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE AprioriSmoothing< ALLOC >& AprioriSmoothing< ALLOC >::
                                      operator=(AprioriSmoothing< ALLOC >&& from) {
      Apriori< ALLOC >::operator=(std::move(from));
      return *this;
    }


    /// indicates whether an apriori is of a certain type
    template < template < typename > class ALLOC >
    INLINE bool AprioriSmoothing< ALLOC >::isOfType(const std::string& type) {
      return AprioriSmoothingType::isOfType(type);
    }


    /// returns the type of the apriori
    template < template < typename > class ALLOC >
    INLINE const std::string& AprioriSmoothing< ALLOC >::getType() const {
      return AprioriSmoothingType::type;
    }


    /// indicates whether the apriori is potentially informative
    template < template < typename > class ALLOC >
    INLINE bool AprioriSmoothing< ALLOC >::isInformative() const {
      return this->_weight != 0.0;
    }


    /// returns the apriori vector all the variables in the idset
    template < template < typename > class ALLOC >
    INLINE void AprioriSmoothing< ALLOC >::addAllApriori(
       const IdSet< ALLOC >&                   idset,
       std::vector< double, ALLOC< double > >& counts) {
      // if the idset is empty or the weight is zero, the apriori is also empty
      if (idset.empty() || (this->_weight == 0.0)) return;

      // otherwise, add the weight to all the cells in the counting vector
      for (auto& count : counts)
        count += this->_weight;
    }


    /// returns the apriori vector over only the conditioning set of an idset
    template < template < typename > class ALLOC >
    void AprioriSmoothing< ALLOC >::addConditioningApriori(
       const IdSet< ALLOC >&                   idset,
       std::vector< double, ALLOC< double > >& counts) {
      // if the conditioning set is empty or the weight is equal to zero,
      // the apriori is also empty
      if ((idset.size() == idset.nbLHSIds()) || (this->_weight == 0.0)
          || (idset.nbLHSIds() == std::size_t(0)))
        return;

      // compute the weight of the conditioning set
      double weight = this->_weight;
      if (this->_nodeId2columns.empty()) {
        for (std::size_t i = std::size_t(0); i < idset.nbLHSIds(); ++i) {
          weight *= this->_database->domainSize(idset[i]);
        }
      } else {
        for (std::size_t i = std::size_t(0); i < idset.nbLHSIds(); ++i) {
          weight *=
             this->_database->domainSize(this->_nodeId2columns.second(idset[i]));
        }
      }

      // add the weight to the counting vector
      for (auto& count : counts)
        count += weight;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
