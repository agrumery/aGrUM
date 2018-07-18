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
 * @brief the base class for all a prioris
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template < template < typename > class ALLOC >
    INLINE Apriori2< ALLOC >::Apriori2(
           const DatabaseTable<ALLOC>& database,
           const Bijection<NodeId,std::size_t,
                           ALLOC<std::size_t>>& nodeId2columns,
           const allocator_type& alloc ) :
      ALLOC<NodeId> ( alloc ),
      _database ( &database ),
      _nodeId2columns ( nodeId2columns ) {
      GUM_CONSTRUCTOR(Apriori2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE Apriori2< ALLOC >::Apriori2(
           const Apriori2< ALLOC >& from,
           const typename Apriori2< ALLOC >::allocator_type& alloc ) :
      ALLOC<NodeId> ( alloc ),
      _weight(from._weight),
      _database ( from._database ),
      _nodeId2columns ( from._nodeId2columns ) {
      GUM_CONS_CPY(Apriori2);
    }

    
    /// copy constructor
    template < template < typename > class ALLOC  >
    INLINE Apriori2< ALLOC >::Apriori2( const Apriori2< ALLOC >& from) :
      Apriori2 ( from, this->getAllocator () ) {}
    

    /// move constructor
    template < template < typename > class ALLOC >
    INLINE Apriori2< ALLOC >::Apriori2(
           Apriori2< ALLOC >&& from,
           const typename Apriori2< ALLOC >::allocator_type& alloc ) :
      ALLOC<NodeId> ( alloc ),
      _weight( from._weight ),
      _database ( from._database ),
      _nodeId2columns ( std::move ( from._nodeId2columns ) ) {
      GUM_CONS_MOV(Apriori2);
    }

    
    /// move constructor
    template < template < typename > class ALLOC >
    INLINE Apriori2< ALLOC >::Apriori2( Apriori2< ALLOC >&& from) :
      Apriori2 ( std::move ( from ), this->getAllocator () ) {}

      
    /// destructor
    template < template < typename > class ALLOC >
    INLINE Apriori2< ALLOC >::~Apriori2() {
      GUM_DESTRUCTOR(Apriori2);
    }

    
    /// copy operator
    template < template < typename > class ALLOC >
    Apriori2< ALLOC >&
    Apriori2< ALLOC >::operator=(const Apriori2< ALLOC >& from) {
      if ( this != &from ) {
        _nodeId2columns = from._nodeId2columns;
        _weight         = from._weight;
        _database       = from._database;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    Apriori2< ALLOC >&
    Apriori2< ALLOC >::operator=(Apriori2< ALLOC >&& from ) {
      if ( this != &from ) {
        _nodeId2columns = std::move ( from._nodeId2columns );
        _weight         = from._weight;
        _database       = from._database;
      }
      return *this;
    }

    
    /// sets the weight of the a priori (kind of effective sample size)
    template < template < typename > class ALLOC >
    INLINE void Apriori2< ALLOC >::setWeight(const double weight) {
      if (weight < 0.0) {
        GUM_ERROR(OutOfBounds,
                  "A negative weight (" << weight <<
                  ") is forbidden for an apriori");
      }
      _weight = weight;
    }
    

    /// returns the weight assigned to the apriori
    template < template < typename > class ALLOC >
    INLINE double Apriori2< ALLOC >::weight() const {
      return _weight;
    }

    
    /// returns the allocator used by the translator
    template < template < typename > class ALLOC >
    INLINE typename Apriori2< ALLOC >::allocator_type
    Apriori2< ALLOC >::getAllocator() const {
      return *this;
    }
  

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
