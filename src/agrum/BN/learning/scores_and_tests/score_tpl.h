
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
 * @brief the base class for all the scores used for learning (BIC, BDeu, etc)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// returns the allocator used by the translator
    template < template < typename > class ALLOC >
    INLINE typename Score< ALLOC >::allocator_type
       Score< ALLOC >::getAllocator() const {
      return _counter.getAllocator();
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE Score< ALLOC >::Score(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              apriori,
       const std::vector< std::pair< std::size_t, std::size_t >,
                          ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                      nodeId2columns,
       const typename Score< ALLOC >::allocator_type& alloc) :
        _apriori(apriori.clone(alloc)),
        _counter(parser, ranges, nodeId2columns, alloc), _cache(alloc) {
      GUM_CONSTRUCTOR(Score);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE Score< ALLOC >::Score(
       const DBRowGeneratorParser< ALLOC >& parser,
       const Apriori< ALLOC >&              apriori,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                                                      nodeId2columns,
       const typename Score< ALLOC >::allocator_type& alloc) :
        _apriori(apriori.clone(alloc)),
        _counter(parser, nodeId2columns, alloc), _cache(alloc) {
      GUM_CONSTRUCTOR(Score);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE Score< ALLOC >::Score(
       const Score< ALLOC >&                          from,
       const typename Score< ALLOC >::allocator_type& alloc) :
        _apriori(from._apriori->clone(alloc)),
        _counter(from._counter, alloc), _cache(from._cache, alloc),
        _use_cache(from._use_cache) {
      GUM_CONS_CPY(Score);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE Score< ALLOC >::Score(const Score< ALLOC >& from) :
        Score(from, from.getAllocator()) {}


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE Score< ALLOC >::Score(
       Score< ALLOC >&&                               from,
       const typename Score< ALLOC >::allocator_type& alloc) :
        _apriori(from._apriori),
        _counter(std::move(from._counter), alloc),
        _cache(std::move(from._cache), alloc), _use_cache(from._use_cache) {
      from._apriori = nullptr;
      GUM_CONS_MOV(Score);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE Score< ALLOC >::Score(Score< ALLOC >&& from) :
        Score(std::move(from), from.getAllocator()) {}


    /// destructor
    template < template < typename > class ALLOC >
    INLINE Score< ALLOC >::~Score() {
      if (_apriori != nullptr) {
        ALLOC< Apriori< ALLOC > > allocator(this->getAllocator());
        allocator.destroy(_apriori);
        allocator.deallocate(_apriori, 1);
      }
      GUM_DESTRUCTOR(Score);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    Score< ALLOC >& Score< ALLOC >::operator=(const Score< ALLOC >& from) {
      if (this != &from) {
        Apriori< ALLOC >*      new_apriori = from._apriori->clone();
        RecordCounter< ALLOC > new_counter = from._counter;
        ScoringCache< ALLOC >  new_cache = from._cache;

        if (_apriori != nullptr) {
          ALLOC< Apriori< ALLOC > > allocator(this->getAllocator());
          allocator.destroy(_apriori);
          allocator.deallocate(_apriori, 1);
        }

        _apriori = new_apriori;
        _counter = std::move(new_counter);
        _cache = std::move(new_cache);

        _use_cache = from._use_cache;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    Score< ALLOC >& Score< ALLOC >::operator=(Score< ALLOC >&& from) {
      if (this != &from) {
        std::swap(_apriori, from._apriori);

        _counter = std::move(from._counter);
        _cache = std::move(from._cache);
        _use_cache = from._use_cache;
      }
      return *this;
    }


    /// changes the max number of threads used to parse the database
    template < template < typename > class ALLOC >
    INLINE void Score< ALLOC >::setMaxNbThreads(std::size_t nb) const {
      _counter.setMaxNbThreads(nb);
    }


    /// returns the number of threads used to parse the database
    template < template < typename > class ALLOC >
    INLINE std::size_t Score< ALLOC >::nbThreads() const {
      return _counter.nbThreads();
    }


    /** @brief changes the number min of rows a thread should process in a
     * multithreading context */
    template < template < typename > class ALLOC >
    INLINE void Score< ALLOC >::setMinNbRowsPerThread(const std::size_t nb) const {
      _counter.setMinNbRowsPerThread(nb);
    }


    /// returns the minimum of rows that each thread should process
    template < template < typename > class ALLOC >
    INLINE std::size_t Score< ALLOC >::minNbRowsPerThread() const {
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
    void Score< ALLOC >::setRanges(
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
    void Score< ALLOC >::clearRanges() {
      std::vector< std::pair< std::size_t, std::size_t >,
                   ALLOC< std::pair< std::size_t, std::size_t > > >
         old_ranges = ranges();
      _counter.clearRanges();
      if (old_ranges != ranges()) clear();
    }


    /// returns the current ranges
    template < template < typename > class ALLOC >
    INLINE const std::vector< std::pair< std::size_t, std::size_t >,
                              ALLOC< std::pair< std::size_t, std::size_t > > >&
                 Score< ALLOC >::ranges() const {
      return _counter.ranges();
    }


    /// returns the score of a single node
    template < template < typename > class ALLOC >
    INLINE double Score< ALLOC >::score(const NodeId var) {
      IdSet< ALLOC > idset(var, _empty_ids, true, this->getAllocator());
      if (_use_cache) {
        try {
          return _cache.score(idset);
        } catch (NotFound&) {}
        double the_score = _score(idset);
        _cache.insert(std::move(idset), the_score);
        return the_score;
      } else {
        return _score(std::move(idset));
      }
    }


    /// returns the score of a single node given some other nodes
    /** @param var1 the variable on the left side of the conditioning bar
     * @param rhs_ids the set of variables on the right side of the
     * conditioning bar */
    template < template < typename > class ALLOC >
    INLINE double Score< ALLOC >::score(
       const NodeId var, const std::vector< NodeId, ALLOC< NodeId > >& rhs_ids) {
      IdSet< ALLOC > idset(var, rhs_ids, false, this->getAllocator());
      if (_use_cache) {
        try {
          return _cache.score(idset);
        } catch (NotFound&) {}
        double the_score = _score(idset);
        _cache.insert(std::move(idset), the_score);
        return the_score;
      } else {
        return _score(idset);
      }
    }


    /// clears all the data structures from memory
    template < template < typename > class ALLOC >
    INLINE void Score< ALLOC >::clear() {
      _counter.clear();
      _cache.clear();
    }


    /// clears the current cache (clear nodesets as well)
    template < template < typename > class ALLOC >
    INLINE void Score< ALLOC >::clearCache() {
      _cache.clear();
    }


    /// turn on/off the use of a cache of the previously computed score
    template < template < typename > class ALLOC >
    INLINE void Score< ALLOC >::useCache(const bool on_off) {
      _use_cache = on_off;
    }


    /// indicates whether the score uses a cache
    template < template < typename > class ALLOC >
    INLINE bool Score< ALLOC >::isUsingCache() const {
      return _use_cache;
    }


    /// return the mapping between the columns of the database and the node ids
    template < template < typename > class ALLOC >
    INLINE const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                 Score< ALLOC >::nodeId2Columns() const {
      return _counter.nodeId2Columns();
    }


    /// return the database used by the score
    template < template < typename > class ALLOC >
    INLINE const DatabaseTable< ALLOC >& Score< ALLOC >::database() const {
      return _counter.database();
    }

    /// returns a counting vector where variables are marginalized from N_xyz
    /** @param X_size the domain size of the variable to marginalize (this
     * is the first variable in table N_xyz
     * @param N_xyz a counting vector of dimension X * cond_vars (in this order)
     */
    template < template < typename > class ALLOC >
    std::vector< double, ALLOC< double > > Score< ALLOC >::_marginalize(
       const NodeId                                  X_id,
       const std::vector< double, ALLOC< double > >& N_xyz) const {
      // compute the domain sizes of the varible on the left hand side
      // of the conditioning bar
      const auto&       nodeId2cols = this->_counter.nodeId2Columns();
      const auto&       database = this->_counter.database();
      const std::size_t X_size = database.domainSize(
         nodeId2cols.empty() ? X_id : nodeId2cols.second(X_id));

      // determine the size of the output vector
      std::size_t out_size = N_xyz.size() / X_size;

      // allocate the output vector
      std::vector< double, ALLOC< double > > res(out_size, 0.0);

      // fill the vector:
      std::size_t xyz = std::size_t(0);
      for (std::size_t z = std::size_t(0); z < out_size; ++z) {
        for (std::size_t x = std::size_t(0); x < X_size; ++x, ++xyz) {
          res[z] += N_xyz[xyz];
        }
      }

      return res;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
