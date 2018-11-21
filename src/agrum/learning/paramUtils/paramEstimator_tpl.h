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
 * @brief the base class for estimating parameters of CPTs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// returns the allocator used by the score
    template < template < typename > class ALLOC >
    INLINE typename ParamEstimator2< ALLOC >::allocator_type
      ParamEstimator2< ALLOC >::getAllocator() const {
      return _counter.getAllocator();
    }


    /// default constructor
    template < template < typename > class ALLOC >
    ParamEstimator2< ALLOC >::ParamEstimator2(
      const DBRowGeneratorParser< ALLOC >& parser,
      const Apriori2< ALLOC >&             external_apriori,
      const Apriori2< ALLOC >&             score_internal_apriori,
      const std::vector< std::pair< std::size_t, std::size_t >,
                         ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename ParamEstimator2< ALLOC >::allocator_type&      alloc) :
        _counter(parser, ranges, nodeId2columns, alloc) {
      // copy the a prioris
      _external_apriori = external_apriori.clone(alloc);
      try {
        _score_internal_apriori = score_internal_apriori.clone();
      } catch (...) {
        ALLOC< Apriori2< ALLOC > > allocator(alloc);
        allocator.destroy(_external_apriori);
        allocator.deallocate(_external_apriori, 1);
        throw;
      }

      GUM_CONSTRUCTOR(ParamEstimator2);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    ParamEstimator2< ALLOC >::ParamEstimator2(
      const DBRowGeneratorParser< ALLOC >& parser,
      const Apriori2< ALLOC >&             external_apriori,
      const Apriori2< ALLOC >&             score_internal_apriori,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename ParamEstimator2< ALLOC >::allocator_type&      alloc) :
        _counter(parser, nodeId2columns, alloc) {
      // copy the a prioris
      _external_apriori = external_apriori.clone(alloc);
      try {
        _score_internal_apriori = score_internal_apriori.clone();
      } catch (...) {
        ALLOC< Apriori2< ALLOC > > allocator(alloc);
        allocator.destroy(_external_apriori);
        allocator.deallocate(_external_apriori, 1);
        throw;
      }

      GUM_CONSTRUCTOR(ParamEstimator2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ParamEstimator2< ALLOC >::ParamEstimator2(
      const ParamEstimator2< ALLOC >&                          from,
      const typename ParamEstimator2< ALLOC >::allocator_type& alloc) :
        _external_apriori(from._external_apriori->clone(alloc)),
        _score_internal_apriori(from._score_internal_apriori->clone(alloc)),
        _counter(from._counter, alloc) {
      GUM_CONS_CPY(ParamEstimator2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE ParamEstimator2< ALLOC >::ParamEstimator2(
      const ParamEstimator2< ALLOC >& from) :
        ParamEstimator2< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE ParamEstimator2< ALLOC >::ParamEstimator2(
      ParamEstimator2< ALLOC >&&                               from,
      const typename ParamEstimator2< ALLOC >::allocator_type& alloc) :
        _external_apriori(from._external_apriori),
        _score_internal_apriori(from._score_internal_apriori),
        _counter(std::move(from._counter), alloc) {
      from._external_apriori = nullptr;
      from._score_internal_apriori = nullptr;
      GUM_CONS_MOV(ParamEstimator2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE
      ParamEstimator2< ALLOC >::ParamEstimator2(ParamEstimator2< ALLOC >&& from) :
        ParamEstimator2< ALLOC >(std::move(from), from.getAllocator()) {}


    /// destructor
    template < template < typename > class ALLOC >
    ParamEstimator2< ALLOC >::~ParamEstimator2() {
      ALLOC< Apriori2< ALLOC > > allocator(this->getAllocator());
      if (_external_apriori != nullptr) {
        allocator.destroy(_external_apriori);
        allocator.deallocate(_external_apriori, 1);
      }

      if (_score_internal_apriori != nullptr) {
        allocator.destroy(_score_internal_apriori);
        allocator.deallocate(_score_internal_apriori, 1);
      }

      GUM_DESTRUCTOR(ParamEstimator2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    ParamEstimator2< ALLOC >& ParamEstimator2< ALLOC >::
                              operator=(const ParamEstimator2< ALLOC >& from) {
      if (this != &from) {
        ALLOC< Apriori2< ALLOC > > allocator(this->getAllocator());
        if (_external_apriori != nullptr) {
          allocator.destroy(_external_apriori);
          allocator.deallocate(_external_apriori, 1);
          _external_apriori = nullptr;
        }
        _external_apriori = from._external_apriori->clone(this->getAllocator());

        if (_score_internal_apriori != nullptr) {
          allocator.destroy(_score_internal_apriori);
          allocator.deallocate(_score_internal_apriori, 1);
          _external_apriori = nullptr;
        }
        _score_internal_apriori =
          from._score_internal_apriori->clone(this->getAllocator());

        _counter = from._counter;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    ParamEstimator2< ALLOC >& ParamEstimator2< ALLOC >::
                              operator=(ParamEstimator2< ALLOC >&& from) {
      if (this != &from) {
        _external_apriori = from._external_apriori;
        _score_internal_apriori = from._score_internal_apriori;
        _counter = std::move(from._counter);
        from._external_apriori = nullptr;
        from._score_internal_apriori = nullptr;
      }
      return *this;
    }


    /// clears all the data structures from memory
    template < template < typename > class ALLOC >
    INLINE void ParamEstimator2< ALLOC >::clear() {
      _counter.clear();
    }


    /// changes the max number of threads used to parse the database
    template < template < typename > class ALLOC >
    void ParamEstimator2< ALLOC >::setMaxNbThreads(std::size_t nb) const {
      _counter.setMaxNbThreads(nb);
    }


    /// returns the number of threads used to parse the database
    template < template < typename > class ALLOC >
    std::size_t ParamEstimator2< ALLOC >::nbThreads() const {
      return _counter.nbThreads();
    }


    /// returns the CPT's parameters corresponding to a given target node
    template < template < typename > class ALLOC >
    INLINE std::vector< double, ALLOC< double > >
    ParamEstimator2< ALLOC >::parameters(const NodeId target_node) {
      return parameters(target_node,_empty_nodevect);
    }
    

    // check the coherency between the parameters passed to setParameters functions
    template < template < typename > class ALLOC >
    template < typename GUM_SCALAR >
    void ParamEstimator2< ALLOC >::__checkParameters(
      const NodeId                                  target_node,
      const std::vector< NodeId, ALLOC< NodeId > >& conditioning_nodes,
      Potential< GUM_SCALAR >&                      pot) {
      // check that the nodes passed in arguments correspond to those of pot
      const Sequence< const DiscreteVariable* >& vars = pot.variablesSequence();
      if (vars.size() == 0) {
        GUM_ERROR(SizeError, "the potential contains no variable");
      }

      const auto& database = _counter.database();
      const auto& node2cols = _counter.nodeId2Columns();
      if (node2cols.empty()) {
        if (database.domainSize(target_node) != vars[0]->domainSize()) {
          GUM_ERROR(SizeError,
                    "Variable "
                      << vars[0]->name() << "of the potential to be filled "
                      << "has a domain size of " << vars[0]->domainSize()
                      << ", which is different from that of node " << target_node
                      << " which is equal to "
                      << database.domainSize(target_node));
        }
        for (std::size_t i = 1; i < vars.size(); ++i) {
          if (database.domainSize(conditioning_nodes[i - 1])
              != vars[i]->domainSize()) {
            GUM_ERROR(SizeError,
                      "Variable "
                        << vars[i]->name() << "of the potential to be filled "
                        << "has a domain size of " << vars[i]->domainSize()
                        << ", which is different from that of node "
                        << conditioning_nodes[i - 1] << " which is equal to "
                        << database.domainSize(conditioning_nodes[i - 1]));
          }
        }
      } else {
        std::size_t col = node2cols.second(target_node);
        if (database.domainSize(col) != vars[0]->domainSize()) {
          GUM_ERROR(SizeError,
                    "Variable "
                      << vars[0]->name() << "of the potential to be filled "
                      << "has a domain size of " << vars[0]->domainSize()
                      << ", which is different from that of node " << target_node
                      << " which is equal to " << database.domainSize(col));
        }
        for (std::size_t i = 1; i < vars.size(); ++i) {
          col = node2cols.second(conditioning_nodes[i - 1]);
          if (database.domainSize(col) != vars[i]->domainSize()) {
            GUM_ERROR(SizeError,
                      "Variable "
                        << vars[i]->name() << "of the potential to be filled "
                        << "has a domain size of " << vars[i]->domainSize()
                        << ", which is different from that of node "
                        << conditioning_nodes[i - 1] << " which is equal to "
                        << database.domainSize(col));
          }
        }
      }
    }


    /// sets the CPT's parameters corresponding to a given nodeset
    template < template < typename > class ALLOC >
    template < typename GUM_SCALAR >
    INLINE void ParamEstimator2< ALLOC >::setParameters(
      const NodeId                                  target_node,
      const std::vector< NodeId, ALLOC< NodeId > >& conditioning_nodes,
      Potential< GUM_SCALAR >&                      pot) {
      __checkParameters(target_node, conditioning_nodes, pot);

      const std::vector< double, ALLOC< double > > nc(
        parameters(target_node, conditioning_nodes));
      pot.fillWith(std::vector< GUM_SCALAR >(nc.begin(), nc.end()));
    }


    /// returns the mapping from ids to column positions in the database
    template < template < typename > class ALLOC >
    INLINE const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                 ParamEstimator2< ALLOC >::nodeId2Columns() const {
      return _counter.nodeId2Columns();
    }


    /// returns the database on which we perform the counts
    template < template < typename > class ALLOC >
    INLINE const DatabaseTable< ALLOC >&
                 ParamEstimator2< ALLOC >::database() const {
      return _counter.database();
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
