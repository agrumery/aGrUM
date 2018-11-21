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
      const DBRowGeneratorParser< ALLOC >&                                 parser,
      const Apriori< ALLOC >&                                             apriori,
      const std::vector< std::pair< std::size_t, std::size_t >,
                         ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename Score< ALLOC >::allocator_type&               alloc) :
        _apriori(apriori.clone(alloc)),
        _counter(parser, ranges, nodeId2columns, alloc), _cache(alloc) {
      GUM_CONSTRUCTOR(Score);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE Score< ALLOC >::Score(
      const DBRowGeneratorParser< ALLOC >&                          parser,
      const Apriori< ALLOC >&                                      apriori,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename Score< ALLOC >::allocator_type&               alloc) :
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
        _use_cache(from._use_cache), _max_nb_threads(from._max_nb_threads),
        _min_nb_rows_per_thread(from._min_nb_rows_per_thread) {
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
        _cache(std::move(from._cache), alloc), _use_cache(from._use_cache),
        _max_nb_threads(from._max_nb_threads),
        _min_nb_rows_per_thread(from._min_nb_rows_per_thread) {
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
        ScoringCache< ALLOC >   new_cache = from._cache;

        if (_apriori != nullptr) {
          ALLOC< Apriori< ALLOC > > allocator(this->getAllocator());
          allocator.destroy(_apriori);
          allocator.deallocate(_apriori, 1);
        }

        _apriori = new_apriori;
        _counter = std::move(new_counter);
        _cache = std::move(new_cache);

        _use_cache = from._use_cache;
        _max_nb_threads = from._max_nb_threads;
        _min_nb_rows_per_thread = from._min_nb_rows_per_thread;
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
        _max_nb_threads = from._max_nb_threads;
        _min_nb_rows_per_thread = from._min_nb_rows_per_thread;
      }
      return *this;
    }


    /// changes the max number of threads used to parse the database
    template < template < typename > class ALLOC >
    INLINE void Score< ALLOC >::setMaxNbThreads(std::size_t nb) const {
      if (nb == std::size_t(0)) nb = std::size_t(1);
      _counter.setMaxNbThreads(nb);
      _max_nb_threads = nb;
    }


    /// returns the number of threads used to parse the database
    template < template < typename > class ALLOC >
    INLINE std::size_t Score< ALLOC >::nbThreads() const {
      return _max_nb_threads;
    }


    /** @brief changes the number min of rows a thread should process in a
     * multithreading context */
    template < template < typename > class ALLOC >
    INLINE void
    Score< ALLOC >::setMinNbRowsPerThread(const std::size_t nb) const {
      _counter.setMinNbRowsPerThread(nb);
    }
    

    /// returns the minimum of rows that each thread should process
    template < template < typename > class ALLOC >
    INLINE std::size_t Score< ALLOC >::minNbRowsPerThread() const {
      return _counter.minNbRowsPerThread();
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
    INLINE bool Score< ALLOC >::isUsingCache () const {
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


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
