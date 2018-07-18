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
    INLINE AprioriSmoothing2< ALLOC >::AprioriSmoothing2(
      const DatabaseTable< ALLOC >&                                 database,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename AprioriSmoothing2< ALLOC >::allocator_type&    alloc) :
        Apriori2< ALLOC >(database, nodeId2columns, alloc) {
      GUM_CONSTRUCTOR(AprioriSmoothing2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriSmoothing2< ALLOC >::AprioriSmoothing2(
      const AprioriSmoothing2< ALLOC >&                          from,
      const typename AprioriSmoothing2< ALLOC >::allocator_type& alloc) :
        Apriori2< ALLOC >(from, alloc) {
      GUM_CONS_CPY(AprioriSmoothing2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE AprioriSmoothing2< ALLOC >::AprioriSmoothing2(
      const AprioriSmoothing2< ALLOC >& from) :
        AprioriSmoothing2< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriSmoothing2< ALLOC >::AprioriSmoothing2(
      AprioriSmoothing2< ALLOC >&&                               from,
      const typename AprioriSmoothing2< ALLOC >::allocator_type& alloc) :
        Apriori2< ALLOC >(std::move(from), alloc) {
      GUM_CONS_MOV(AprioriSmoothing2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE AprioriSmoothing2< ALLOC >::AprioriSmoothing2(
      AprioriSmoothing2< ALLOC >&& from) :
        AprioriSmoothing2< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    AprioriSmoothing2< ALLOC >* AprioriSmoothing2< ALLOC >::clone(
      const typename AprioriSmoothing2< ALLOC >::allocator_type& alloc) const {
      ALLOC< AprioriSmoothing2< ALLOC > > allocator(alloc);
      AprioriSmoothing2< ALLOC >*         apriori = allocator.allocate(1);
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
    INLINE AprioriSmoothing2< ALLOC >* AprioriSmoothing2< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE AprioriSmoothing2< ALLOC >::~AprioriSmoothing2() {
      GUM_DESTRUCTOR(AprioriSmoothing2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE AprioriSmoothing2< ALLOC >& AprioriSmoothing2< ALLOC >::
                                       operator=(const AprioriSmoothing2< ALLOC >& from) {
      Apriori2< ALLOC >::operator=(from);
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE AprioriSmoothing2< ALLOC >& AprioriSmoothing2< ALLOC >::
                                       operator=(AprioriSmoothing2< ALLOC >&& from) {
      Apriori2< ALLOC >::operator=(std::move(from));
      return *this;
    }


    /// indicates whether an apriori is of a certain type
    template < template < typename > class ALLOC >
    INLINE bool AprioriSmoothing2< ALLOC >::isOfType(const std::string& type) {
      return AprioriSmoothingType::isOfType(type);
    }


    /// returns the type of the apriori
    template < template < typename > class ALLOC >
    INLINE const std::string& AprioriSmoothing2< ALLOC >::getType() const {
      return AprioriSmoothingType::type;
    }


    /// returns the apriori vector all the variables in the idset
    template < template < typename > class ALLOC >
    std::vector< double, ALLOC< double > >
      AprioriSmoothing2< ALLOC >::getAllApriori(const IdSet2< ALLOC >& idset) {
      // if the idset is empty, the apriori is also empty
      if (idset.empty())
        return std::vector< double, ALLOC< double > >(this->getAllocator());

      // we determine the size of the counting vector
      std::size_t apriori_size = std::size_t(1);
      if (this->_nodeId2columns.empty()) {
        for (const auto id : idset) {
          apriori_size *= this->_database->domainSize(id);
        }
      } else {
        for (const auto id : idset) {
          apriori_size *=
            this->_database->domainSize(this->_nodeId2columns.second(id));
        }
      }

      return std::vector< double, ALLOC< double > >(
        apriori_size, this->_weight, this->getAllocator());
    }


    /// returns the apriori vector over only the conditioning set of an idset
    template < template < typename > class ALLOC >
    std::vector< double, ALLOC< double > >
      AprioriSmoothing2< ALLOC >::getConditioningApriori(
        const IdSet2< ALLOC >& idset) {
      // if the idset is empty, the apriori is also empty
      if (idset.size() == idset.nbLHSIds())
        return std::vector< double, ALLOC< double > >(this->getAllocator());

      // compute the weight of the conditioning set
      double weight = this->_weight;
      if ((idset.nbLHSIds() == std::size_t(0)) || (weight == 0.0))
        weight = 0.0;
      else {
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
      }

      // we determine the size of the counting vector
      std::size_t       apriori_size = std::size_t(1);
      const std::size_t idset_size = idset.size();
      if (this->_nodeId2columns.empty()) {
        for (std::size_t i = idset.nbLHSIds(); i < idset_size; ++i) {
          apriori_size *= this->_database->domainSize(idset[i]);
        }
      } else {
        for (std::size_t i = idset.nbLHSIds(); i < idset_size; ++i) {
          apriori_size *=
            this->_database->domainSize(this->_nodeId2columns.second(idset[i]));
        }
      }

      return std::vector< double, ALLOC< double > >(
        apriori_size, weight, this->getAllocator());
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
