/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
 *   info_at_agrum_dot_org
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
 * @brief the base class for all the independence tests used for learning

 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES (@AMU)
 */
#include <agrum/tools/stattests/pseudoCount.h>
#include <agrum/tools/stattests//idCondSet.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// returns the allocator used by the independence test
    template <  template < typename > class ALLOC >
    INLINE typename PseudoCount< ALLOC >::allocator_type
       PseudoCount< ALLOC >::getAllocator() const {
      return _counter.getAllocator();
    }

    /// default constructor
    template < template < typename > class ALLOC >
    INLINE PseudoCount< ALLOC >::PseudoCount(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              apriori,
       const std::vector< std::pair< std::size_t, std::size_t >,
                          ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                                 nodeId2columns,
       const typename PseudoCount< ALLOC >::allocator_type& alloc) :
        _apriori(apriori.clone(alloc)),
        _counter(parser, ranges, nodeId2columns, alloc) {
      GUM_CONSTRUCTOR(PseudoCount);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE PseudoCount< ALLOC >::PseudoCount(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              apriori,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                                 nodeId2columns,
       const typename PseudoCount< ALLOC >::allocator_type& alloc) :
        _apriori(apriori.clone(alloc)),
        _counter(parser, nodeId2columns, alloc) {
      GUM_CONSTRUCTOR(PseudoCount);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE PseudoCount< ALLOC >::PseudoCount(
       const PseudoCount< ALLOC >&                          from,
       const typename PseudoCount< ALLOC >::allocator_type& alloc) :
        _apriori(from._apriori->clone(alloc)),
        _counter(from._counter, alloc) {
      GUM_CONS_CPY(PseudoCount);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE PseudoCount< ALLOC >::PseudoCount(
       const PseudoCount< ALLOC >& from) :
        PseudoCount(from, from.getAllocator()) {}


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE PseudoCount< ALLOC >::PseudoCount(
       PseudoCount< ALLOC >&&                               from,
       const typename PseudoCount< ALLOC >::allocator_type& alloc) :
        _apriori(from._apriori),
        _counter(std::move(from._counter), alloc) {
      from._apriori = nullptr;
      GUM_CONS_MOV(PseudoCount);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE PseudoCount< ALLOC >::PseudoCount(
       PseudoCount< ALLOC >&& from) :
        PseudoCount(std::move(from), from.getAllocator()) {}


    /// destructor
    template < template < typename > class ALLOC >
    INLINE PseudoCount< ALLOC >::~PseudoCount() {
      if (_apriori != nullptr) {
        ALLOC< Apriori< ALLOC > > allocator(this->getAllocator());
        allocator.destroy(_apriori);
        allocator.deallocate(_apriori, 1);
      }
      GUM_DESTRUCTOR(PseudoCount);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    PseudoCount< ALLOC >& PseudoCount< ALLOC >::operator=(
       const PseudoCount< ALLOC >& from) {
      if (this != &from) {
        Apriori< ALLOC >*      new_apriori = from._apriori->clone();
        RecordCounter< ALLOC > new_counter = from._counter;

        if (_apriori != nullptr) {
          ALLOC< Apriori< ALLOC > > allocator(this->getAllocator());
          allocator.destroy(_apriori);
          allocator.deallocate(_apriori, 1);
        }

        _apriori = new_apriori;
        _counter = std::move(new_counter);
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    PseudoCount< ALLOC >&
       PseudoCount< ALLOC >::operator=(PseudoCount< ALLOC >&& from) {
      if (this != &from) {
        std::swap(_apriori, from._apriori);

        _counter = std::move(from._counter);
      }
      return *this;
    }


    /// changes the max number of threads used to parse the database
    template < template < typename > class ALLOC >
    INLINE void PseudoCount< ALLOC >::setMaxNbThreads(std::size_t nb) const {
      _counter.setMaxNbThreads(nb);
    }


    /// returns the number of threads used to parse the database
    template < template < typename > class ALLOC >
    INLINE std::size_t PseudoCount< ALLOC >::nbThreads() const {
      return _counter.nbThreads();
    }


    /** @brief changes the number min of rows a thread should process in a
     * multithreading context */
    template < template < typename > class ALLOC >
    INLINE void PseudoCount< ALLOC >::setMinNbRowsPerThread(
       const std::size_t nb) const {
      _counter.setMinNbRowsPerThread(nb);
    }


    /// returns the minimum of rows that each thread should process
    template < template < typename > class ALLOC >
    INLINE std::size_t PseudoCount< ALLOC >::minNbRowsPerThread() const {
      return _counter.minNbRowsPerThread();
    }


    /// sets new ranges to perform the countings used by the score
    /** @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
     * indices. The countings are then performed only on the union of the
     * rows [Xi,Yi), i in {1,...,n}. This is useful, e.g, when performing
     * cross validation tasks, in which part of the database should be ignored.
     * An empty set of ranges is equivalent to an interval [X,Y) ranging over
     * the whole database. */
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    void PseudoCount< ALLOC >::setRanges(
       const std::vector< std::pair< std::size_t, std::size_t >,
                          XALLOC< std::pair< std::size_t, std::size_t > > >&
          new_ranges) {
      std::vector< std::pair< std::size_t, std::size_t >,
                   ALLOC< std::pair< std::size_t, std::size_t > > >
         old_ranges = ranges();
      _counter.setRanges(new_ranges);
      if (old_ranges != ranges()) clear();
    }


    /// reset the ranges to the one range corresponding to the whole database
    template < template < typename > class ALLOC >
    void PseudoCount< ALLOC >::clearRanges() {
      std::vector< std::pair< std::size_t, std::size_t >,
                   ALLOC< std::pair< std::size_t, std::size_t > > >
         old_ranges = ranges();
      _counter.clearRanges();
    }


    /// returns the current ranges
    template < template < typename > class ALLOC >
    INLINE const std::vector< std::pair< std::size_t, std::size_t >,
                              ALLOC< std::pair< std::size_t, std::size_t > > >&
                 PseudoCount< ALLOC >::ranges() const {
      return _counter.ranges();
    }


    /// clears all the data structures from memory
    template < template < typename > class ALLOC >
    INLINE void PseudoCount< ALLOC >::clear() {
      _counter.clear();
    }



    /// return the mapping between the columns of the database and the node ids
    template < template < typename > class ALLOC >
    INLINE const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                 PseudoCount< ALLOC >::nodeId2Columns() const {
      return _counter.nodeId2Columns();
    }


    /// return the database used by the score
    template < template < typename > class ALLOC >
    INLINE const DatabaseTable< ALLOC >&
                 PseudoCount< ALLOC >::database() const {
      return _counter.database();
    }


    /// returns a counting vector where variables are marginalized from N_xyz
    /** @param node_2_marginalize indicates which node(s) shall be marginalized:
     * - 0 means that X should be marginalized
     * - 1 means that Y should be marginalized
     * - 2 means that Z should be marginalized
     */
    template < template < typename > class ALLOC >
    std::vector< double, ALLOC< double > > PseudoCount< ALLOC >::get(const std::vector<NodeId, ALLOC<NodeId> > &ids) {
      IdCondSet<ALLOC> idset(ids,false,true);
      std::vector< double, ALLOC< double > > N_xyz(
          this->_counter.counts(idset, true));
      const bool informative_external_apriori = this->_apriori->isInformative();
      if (informative_external_apriori)
        this->_apriori->addAllApriori(idset, N_xyz);
      return N_xyz;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */