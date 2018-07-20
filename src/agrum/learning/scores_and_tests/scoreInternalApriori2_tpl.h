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
 * @brief the base class for all the score's internal aprioris
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// returns the allocator used by the translator
    template < template < typename > class ALLOC >
    INLINE typename ScoreInternalApriori2< ALLOC >::allocator_type
    ScoreInternalApriori2< ALLOC >::getAllocator() const {
      return *this;
    }

    
    /// default constructor
    template < template < typename > class ALLOC >
    INLINE ScoreInternalApriori2< ALLOC >::ScoreInternalApriori2(
      const DatabaseTable< ALLOC >&                                 database,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename ScoreInternalApriori2< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        _database(&database), _nodeId2columns(nodeId2columns) {
      GUM_CONSTRUCTOR(ScoreInternalApriori2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ScoreInternalApriori2< ALLOC >::ScoreInternalApriori2(
      const ScoreInternalApriori2< ALLOC >&                          from,
      const typename ScoreInternalApriori2< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        _database(from._database), _nodeId2columns(from._nodeId2columns) {
      GUM_CONS_CPY(ScoreInternalApriori2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE ScoreInternalApriori2< ALLOC >::ScoreInternalApriori2(const ScoreInternalApriori2< ALLOC >& from) :
        ScoreInternalApriori2(from, this->getAllocator()) {}


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE ScoreInternalApriori2< ALLOC >::ScoreInternalApriori2(
      ScoreInternalApriori2< ALLOC >&&                               from,
      const typename ScoreInternalApriori2< ALLOC >::allocator_type& alloc) :
        ALLOC< NodeId >(alloc),
        _database(from._database),
        _nodeId2columns(std::move(from._nodeId2columns)) {
      GUM_CONS_MOV(ScoreInternalApriori2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE ScoreInternalApriori2< ALLOC >::ScoreInternalApriori2(ScoreInternalApriori2< ALLOC >&& from) :
        ScoreInternalApriori2(std::move(from), this->getAllocator()) {}


    /// destructor
    template < template < typename > class ALLOC >
    INLINE ScoreInternalApriori2< ALLOC >::~ScoreInternalApriori2() {
      GUM_DESTRUCTOR(ScoreInternalApriori2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    ScoreInternalApriori2< ALLOC >& ScoreInternalApriori2< ALLOC >::
                       operator=(const ScoreInternalApriori2< ALLOC >& from) {
      if (this != &from) {
        _nodeId2columns = from._nodeId2columns;
        _database = from._database;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    ScoreInternalApriori2< ALLOC >&
    ScoreInternalApriori2< ALLOC >::operator=(ScoreInternalApriori2< ALLOC >&& from) {
      if (this != &from) {
        _nodeId2columns = std::move(from._nodeId2columns);
        _database = from._database;
      }
      return *this;
    }

    
    /// indicates whether the apriori is potentially informative
   template < template < typename > class ALLOC >
    INLINE bool
      ScoreInternalApriori2< ALLOC >::isInformative() const {
      return true;
    }
  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
