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
 * @brief the internal apriori for the scores without apriori (e.g., BD)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ScoreInternalNoApriori2< ALLOC >::ScoreInternalNoApriori2(
      const DatabaseTable< ALLOC >&                                 database,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename ScoreInternalNoApriori2< ALLOC >::allocator_type& alloc) :
        ScoreInternalApriori2< ALLOC >(database, nodeId2columns, alloc) {
      GUM_CONSTRUCTOR(ScoreInternalNoApriori2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ScoreInternalNoApriori2< ALLOC >::ScoreInternalNoApriori2(
      const ScoreInternalNoApriori2< ALLOC >&                          from,
      const typename ScoreInternalNoApriori2< ALLOC >::allocator_type& alloc) :
        ScoreInternalApriori2< ALLOC >(from, alloc) {
      GUM_CONS_CPY(ScoreInternalNoApriori2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE ScoreInternalNoApriori2< ALLOC >::ScoreInternalNoApriori2(
      const ScoreInternalNoApriori2< ALLOC >& from) :
        ScoreInternalNoApriori2(from, this->getAllocator()) {}


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE ScoreInternalNoApriori2< ALLOC >::ScoreInternalNoApriori2(
      ScoreInternalNoApriori2< ALLOC >&&                               from,
      const typename ScoreInternalNoApriori2< ALLOC >::allocator_type& alloc) :
        ScoreInternalApriori2< ALLOC >(std::move(from), alloc) {
      GUM_CONS_MOV(ScoreInternalNoApriori2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE ScoreInternalNoApriori2< ALLOC >::ScoreInternalNoApriori2(
      ScoreInternalNoApriori2< ALLOC >&& from) :
        ScoreInternalNoApriori2(std::move(from), this->getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    ScoreInternalNoApriori2< ALLOC >* ScoreInternalNoApriori2< ALLOC >::clone(
      const typename ScoreInternalNoApriori2< ALLOC >::allocator_type& alloc)
      const {
      ALLOC< ScoreInternalNoApriori2< ALLOC > > allocator(alloc);
      ScoreInternalNoApriori2< ALLOC >*         apriori = allocator.allocate(1);
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
    INLINE ScoreInternalNoApriori2< ALLOC >*
           ScoreInternalNoApriori2< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE ScoreInternalNoApriori2< ALLOC >::~ScoreInternalNoApriori2() {
      GUM_DESTRUCTOR(ScoreInternalNoApriori2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    ScoreInternalNoApriori2< ALLOC >& ScoreInternalNoApriori2< ALLOC >::
                                      operator=(const ScoreInternalNoApriori2< ALLOC >& from) {
      ScoreInternalApriori2< ALLOC >::operator=(from);
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    ScoreInternalNoApriori2< ALLOC >& ScoreInternalNoApriori2< ALLOC >::
                                      operator=(ScoreInternalNoApriori2< ALLOC >&& from) {
      ScoreInternalApriori2< ALLOC >::operator=(std::move(from));
      return *this;
    }


    /// returns the apriori vector all the variables in the idset
    template < template < typename > class ALLOC >
    INLINE void ScoreInternalNoApriori2< ALLOC >::addAllApriori(
      const IdSet2< ALLOC >&                  idset,
      std::vector< double, ALLOC< double > >& counts) {}


    /// returns the apriori vector over only the conditioning set of an idset
    template < template < typename > class ALLOC >
    INLINE void ScoreInternalNoApriori2< ALLOC >::addConditioningApriori(
      const IdSet2< ALLOC >&                  idset,
      std::vector< double, ALLOC< double > >& counts) {}


    /// indicates whether the apriori is potentially informative
    template < template < typename > class ALLOC >
    INLINE bool ScoreInternalNoApriori2< ALLOC >::isInformative() const {
      return false;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
