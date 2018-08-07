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
 * @brief the internal apriori for the K2 score = Laplace Apriori
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE AprioriK2< ALLOC >::AprioriK2(
      const DatabaseTable< ALLOC >&                                 database,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename AprioriK2< ALLOC >::allocator_type&    alloc) :
        AprioriSmoothing2< ALLOC >(database, nodeId2columns, alloc) {
      GUM_CONSTRUCTOR(AprioriK2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriK2< ALLOC >::AprioriK2(
      const AprioriK2< ALLOC >&                          from,
      const typename AprioriK2< ALLOC >::allocator_type& alloc) :
        AprioriSmoothing2< ALLOC >(from, alloc) {
      GUM_CONS_CPY(AprioriK2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE AprioriK2< ALLOC >::AprioriK2(
      const AprioriK2< ALLOC >& from) :
        AprioriK2< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriK2< ALLOC >::AprioriK2(
      AprioriK2< ALLOC >&&                               from,
      const typename AprioriK2< ALLOC >::allocator_type& alloc) :
        AprioriSmoothing2< ALLOC >(std::move(from), alloc) {
      GUM_CONS_MOV(AprioriK2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE AprioriK2< ALLOC >::AprioriK2(
      AprioriK2< ALLOC >&& from) :
        AprioriK2< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    AprioriK2< ALLOC >* AprioriK2< ALLOC >::clone(
      const typename AprioriK2< ALLOC >::allocator_type& alloc) const {
      ALLOC< AprioriK2< ALLOC > > allocator(alloc);
      AprioriK2< ALLOC >*         apriori = allocator.allocate(1);
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
    INLINE AprioriK2< ALLOC >* AprioriK2< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE AprioriK2< ALLOC >::~AprioriK2() {
      GUM_DESTRUCTOR(AprioriK2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE AprioriK2< ALLOC >& AprioriK2< ALLOC >::
                                       operator=(const AprioriK2< ALLOC >& from) {
      AprioriSmoothing2< ALLOC >::operator=(from);
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE AprioriK2< ALLOC >& AprioriK2< ALLOC >::
                                       operator=(AprioriK2< ALLOC >&& from) {
      AprioriSmoothing2< ALLOC >::operator=(std::move(from));
      return *this;
    }

    
    /// dummy set weight function: in K2, weights are always equal to 1
    template < template < typename > class ALLOC >
    INLINE void AprioriK2< ALLOC >::setWeight(const double weight) {}


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
