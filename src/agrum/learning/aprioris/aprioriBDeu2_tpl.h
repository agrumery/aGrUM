/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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
    INLINE AprioriBDeu2< ALLOC >::AprioriBDeu2(
      const DatabaseTable< ALLOC >&                                 database,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename AprioriBDeu2< ALLOC >::allocator_type&         alloc) :
        Apriori2< ALLOC >(database, nodeId2columns, alloc) {
      GUM_CONSTRUCTOR(AprioriBDeu2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriBDeu2< ALLOC >::AprioriBDeu2(
      const AprioriBDeu2< ALLOC >&                          from,
      const typename AprioriBDeu2< ALLOC >::allocator_type& alloc) :
        Apriori2< ALLOC >(from, alloc) {
      GUM_CONS_CPY(AprioriBDeu2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE AprioriBDeu2< ALLOC >::AprioriBDeu2(const AprioriBDeu2< ALLOC >& from) :
        AprioriBDeu2< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriBDeu2< ALLOC >::AprioriBDeu2(
      AprioriBDeu2< ALLOC >&&                               from,
      const typename AprioriBDeu2< ALLOC >::allocator_type& alloc) :
        Apriori2< ALLOC >(std::move(from), alloc) {
      GUM_CONS_MOV(AprioriBDeu2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE AprioriBDeu2< ALLOC >::AprioriBDeu2(AprioriBDeu2< ALLOC >&& from) :
        AprioriBDeu2< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    AprioriBDeu2< ALLOC >* AprioriBDeu2< ALLOC >::clone(
      const typename AprioriBDeu2< ALLOC >::allocator_type& alloc) const {
      ALLOC< AprioriBDeu2< ALLOC > > allocator(alloc);
      AprioriBDeu2< ALLOC >*         apriori = allocator.allocate(1);
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
    INLINE AprioriBDeu2< ALLOC >* AprioriBDeu2< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE AprioriBDeu2< ALLOC >::~AprioriBDeu2() {
      GUM_DESTRUCTOR(AprioriBDeu2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE AprioriBDeu2< ALLOC >& AprioriBDeu2< ALLOC >::
                                  operator=(const AprioriBDeu2< ALLOC >& from) {
      Apriori2< ALLOC >::operator=(from);
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE AprioriBDeu2< ALLOC >& AprioriBDeu2< ALLOC >::
                                  operator=(AprioriBDeu2< ALLOC >&& from) {
      Apriori2< ALLOC >::operator=(std::move(from));
      return *this;
    }


    /// sets the effective sample size N' (alias of setEffectiveSampleSize ())
    template < template < typename > class ALLOC >
    INLINE void AprioriBDeu2< ALLOC >::setWeight(const double weight) {
      if (weight < 0.0) {
        GUM_ERROR(OutOfBounds,
                  "A negative weight (" << weight
                                        << ") is forbidden for the BDeu apriori");
      }
      this->_weight = weight;
    }


    /// sets the effective sample size N'
    template < template < typename > class ALLOC >
    INLINE void
      AprioriBDeu2< ALLOC >::setEffectiveSampleSize(const double weight) {
      setWeight(weight);
    }


    /// indicates whether an apriori is of a certain type
    template < template < typename > class ALLOC >
    INLINE bool AprioriBDeu2< ALLOC >::isOfType(const std::string& type) {
      return AprioriBDeuType::isOfType(type);
    }


    /// returns the type of the apriori
    template < template < typename > class ALLOC >
    INLINE const std::string& AprioriBDeu2< ALLOC >::getType() const {
      return AprioriBDeuType::type;
    }


    /// indicates whether the apriori is potentially informative
    template < template < typename > class ALLOC >
    INLINE bool AprioriBDeu2< ALLOC >::isInformative() const {
      return this->_weight != 0.0;
    }


    /// returns the apriori vector all the variables in the idset
    template < template < typename > class ALLOC >
    INLINE void AprioriBDeu2< ALLOC >::addAllApriori(
      const IdSet2< ALLOC >&                  idset,
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
    void AprioriBDeu2< ALLOC >::addConditioningApriori(
      const IdSet2< ALLOC >&                  idset,
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
