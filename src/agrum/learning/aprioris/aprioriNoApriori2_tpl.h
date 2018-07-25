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
 * @brief the no a priori class: corresponds to 0 weight-sample
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori2< ALLOC >::AprioriNoApriori2(
      const DatabaseTable< ALLOC >&                                 database,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename AprioriNoApriori2< ALLOC >::allocator_type&    alloc) :
        Apriori2< ALLOC >(database, nodeId2columns, alloc) {
      Apriori2< ALLOC >::setWeight(0.0);
      GUM_CONSTRUCTOR(AprioriNoApriori2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori2< ALLOC >::AprioriNoApriori2(
      const AprioriNoApriori2< ALLOC >&                          from,
      const typename AprioriNoApriori2< ALLOC >::allocator_type& alloc) :
        Apriori2< ALLOC >(from, alloc) {
      GUM_CONS_CPY(AprioriNoApriori2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori2< ALLOC >::AprioriNoApriori2(
      const AprioriNoApriori2< ALLOC >& from) :
        AprioriNoApriori2< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori2< ALLOC >::AprioriNoApriori2(
      AprioriNoApriori2< ALLOC >&&                               from,
      const typename AprioriNoApriori2< ALLOC >::allocator_type& alloc) :
        Apriori2< ALLOC >(std::move(from), alloc) {
      GUM_CONS_MOV(AprioriNoApriori2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori2< ALLOC >::AprioriNoApriori2(
      AprioriNoApriori2< ALLOC >&& from) :
        AprioriNoApriori2< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    AprioriNoApriori2< ALLOC >* AprioriNoApriori2< ALLOC >::clone(
      const typename AprioriNoApriori2< ALLOC >::allocator_type& alloc) const {
      ALLOC< AprioriNoApriori2< ALLOC > > allocator(alloc);
      AprioriNoApriori2< ALLOC >*         apriori = allocator.allocate(1);
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
    INLINE AprioriNoApriori2< ALLOC >* AprioriNoApriori2< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori2< ALLOC >::~AprioriNoApriori2() {
      GUM_DESTRUCTOR(AprioriNoApriori2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori2< ALLOC >& AprioriNoApriori2< ALLOC >::
                                       operator=(const AprioriNoApriori2< ALLOC >& from) {
      Apriori2< ALLOC >::operator=(from);
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE AprioriNoApriori2< ALLOC >& AprioriNoApriori2< ALLOC >::
                                       operator=(AprioriNoApriori2< ALLOC >&& from) {
      Apriori2< ALLOC >::operator=(std::move(from));
      return *this;
    }


    /// sets the weight of the a priori
    template < template < typename > class ALLOC >
    INLINE void AprioriNoApriori2< ALLOC >::setWeight(const double) {}


    /// indicates whether an apriori is of a certain type
    template < template < typename > class ALLOC >
    INLINE bool AprioriNoApriori2< ALLOC >::isOfType(const std::string& type) {
      return AprioriNoAprioriType::isOfType(type);
    }


    /// returns the type of the apriori
    template < template < typename > class ALLOC >
    INLINE const std::string& AprioriNoApriori2< ALLOC >::getType() const {
      return AprioriNoAprioriType::type;
    }


    /// returns the apriori vector all the variables in the idset
    template < template < typename > class ALLOC >
    INLINE void AprioriNoApriori2< ALLOC >::addAllApriori(
      const IdSet2< ALLOC >&                  idset,
      std::vector< double, ALLOC< double > >& counts) {}


    /// returns the apriori vector over only the conditioning set of an idset
    template < template < typename > class ALLOC >
    INLINE void AprioriNoApriori2< ALLOC >::addConditioningApriori(
      const IdSet2< ALLOC >&                  idset,
      std::vector< double, ALLOC< double > >& counts) {}


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
