
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
 * @brief the internal apriori for the BDeu score (N' / (r_i * q_i)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE AprioriBDeu< ALLOC >::AprioriBDeu(
       const DatabaseTable< ALLOC >& database,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                            nodeId2columns,
       const typename AprioriBDeu< ALLOC >::allocator_type& alloc) :
        Apriori< ALLOC >(database, nodeId2columns, alloc) {
      GUM_CONSTRUCTOR(AprioriBDeu);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriBDeu< ALLOC >::AprioriBDeu(
       const AprioriBDeu< ALLOC >&                          from,
       const typename AprioriBDeu< ALLOC >::allocator_type& alloc) :
        Apriori< ALLOC >(from, alloc) {
      GUM_CONS_CPY(AprioriBDeu);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE AprioriBDeu< ALLOC >::AprioriBDeu(const AprioriBDeu< ALLOC >& from) :
        AprioriBDeu< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriBDeu< ALLOC >::AprioriBDeu(
       AprioriBDeu< ALLOC >&&                               from,
       const typename AprioriBDeu< ALLOC >::allocator_type& alloc) :
        Apriori< ALLOC >(std::move(from), alloc) {
      GUM_CONS_MOV(AprioriBDeu);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE AprioriBDeu< ALLOC >::AprioriBDeu(AprioriBDeu< ALLOC >&& from) :
        AprioriBDeu< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    AprioriBDeu< ALLOC >* AprioriBDeu< ALLOC >::clone(
       const typename AprioriBDeu< ALLOC >::allocator_type& alloc) const {
      ALLOC< AprioriBDeu< ALLOC > > allocator(alloc);
      AprioriBDeu< ALLOC >*         apriori = allocator.allocate(1);
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
    INLINE AprioriBDeu< ALLOC >* AprioriBDeu< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE AprioriBDeu< ALLOC >::~AprioriBDeu() {
      GUM_DESTRUCTOR(AprioriBDeu);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE AprioriBDeu< ALLOC >& AprioriBDeu< ALLOC >::
                                 operator=(const AprioriBDeu< ALLOC >& from) {
      Apriori< ALLOC >::operator=(from);
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE AprioriBDeu< ALLOC >& AprioriBDeu< ALLOC >::
                                 operator=(AprioriBDeu< ALLOC >&& from) {
      Apriori< ALLOC >::operator=(std::move(from));
      return *this;
    }


    /// sets the effective sample size N' (alias of setEffectiveSampleSize ())
    template < template < typename > class ALLOC >
    INLINE void AprioriBDeu< ALLOC >::setWeight(const double weight) {
      if (weight < 0.0) {
        GUM_ERROR(OutOfBounds,
                  "A negative weight (" << weight
                                        << ") is forbidden for the BDeu apriori");
      }
      this->_weight = weight;
    }


    /// sets the effective sample size N'
    template < template < typename > class ALLOC >
    INLINE void AprioriBDeu< ALLOC >::setEffectiveSampleSize(const double weight) {
      setWeight(weight);
    }


    /// indicates whether an apriori is of a certain type
    template < template < typename > class ALLOC >
    INLINE bool AprioriBDeu< ALLOC >::isOfType(const std::string& type) {
      return AprioriBDeuType::isOfType(type);
    }


    /// returns the type of the apriori
    template < template < typename > class ALLOC >
    INLINE const std::string& AprioriBDeu< ALLOC >::getType() const {
      return AprioriBDeuType::type;
    }


    /// indicates whether the apriori is potentially informative
    template < template < typename > class ALLOC >
    INLINE bool AprioriBDeu< ALLOC >::isInformative() const {
      return this->_weight != 0.0;
    }


    /// returns the apriori vector all the variables in the idset
    template < template < typename > class ALLOC >
    INLINE void AprioriBDeu< ALLOC >::addAllApriori(
       const IdSet< ALLOC >&                   idset,
       std::vector< double, ALLOC< double > >& counts) {
      // if the idset is empty or the weight is zero, the apriori is also empty
      if (idset.empty() || (this->_weight == 0.0)) return;

      // otherwise, add the weight to all the cells in the counting vector
      const double weight = this->_weight / counts.size();
      for (auto& count : counts)
        count += weight;
    }


    /// returns the apriori vector over only the conditioning set of an idset
    template < template < typename > class ALLOC >
    void AprioriBDeu< ALLOC >::addConditioningApriori(
       const IdSet< ALLOC >&                   idset,
       std::vector< double, ALLOC< double > >& counts) {
      // if the conditioning set is empty or the weight is equal to zero,
      // the apriori is also empty
      if ((idset.size() == idset.nbLHSIds()) || (this->_weight == 0.0)
          || (idset.nbLHSIds() == std::size_t(0)))
        return;

      // add the weight to the counting vector
      const double weight = this->_weight / counts.size();
      for (auto& count : counts)
        count += weight;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
