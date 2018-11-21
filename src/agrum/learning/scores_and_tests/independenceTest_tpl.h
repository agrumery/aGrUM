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
 * @brief the base class for all the independence tests used for learning
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// returns the allocator used by the independence test
    template < template < typename > class ALLOC >
    INLINE typename IndependenceTest2< ALLOC >::allocator_type
      IndependenceTest2< ALLOC >::getAllocator() const {
      return _counter.getAllocator();
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IndependenceTest2< ALLOC >::IndependenceTest2(
      const DBRowGeneratorParser< ALLOC >&                                 parser,
      const Apriori2< ALLOC >&                                             apriori,
      const std::vector< std::pair< std::size_t, std::size_t >,
                         ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename IndependenceTest2< ALLOC >::allocator_type&    alloc) :
        _apriori(apriori.clone(alloc)),
        _counter(parser, ranges, nodeId2columns, alloc), _cache(alloc) {
      GUM_CONSTRUCTOR(IndependenceTest2);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IndependenceTest2< ALLOC >::IndependenceTest2(
      const DBRowGeneratorParser< ALLOC >&                          parser,
      const Apriori2< ALLOC >&                                      apriori,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename IndependenceTest2< ALLOC >::allocator_type&    alloc) :
        _apriori(apriori.clone(alloc)),
        _counter(parser, nodeId2columns, alloc), _cache(alloc) {
      GUM_CONSTRUCTOR(IndependenceTest2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IndependenceTest2< ALLOC >::IndependenceTest2(
      const IndependenceTest2< ALLOC >&                          from,
      const typename IndependenceTest2< ALLOC >::allocator_type& alloc) :
        _apriori(from._apriori->clone(alloc)),
        _counter(from._counter, alloc), _cache(from._cache, alloc),
        _use_cache(from._use_cache), _max_nb_threads(from._max_nb_threads),
        _min_nb_rows_per_thread(from._min_nb_rows_per_thread) {
      GUM_CONS_CPY(IndependenceTest2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE IndependenceTest2< ALLOC >::IndependenceTest2(
      const IndependenceTest2< ALLOC >& from) :
        IndependenceTest2(from, from.getAllocator()) {}


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE IndependenceTest2< ALLOC >::IndependenceTest2(
      IndependenceTest2< ALLOC >&&                               from,
      const typename IndependenceTest2< ALLOC >::allocator_type& alloc) :
        _apriori(from._apriori),
        _counter(std::move(from._counter), alloc),
        _cache(std::move(from._cache), alloc), _use_cache(from._use_cache),
        _max_nb_threads(from._max_nb_threads),
        _min_nb_rows_per_thread(from._min_nb_rows_per_thread) {
      from._apriori = nullptr;
      GUM_CONS_MOV(IndependenceTest2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE IndependenceTest2< ALLOC >::IndependenceTest2(
      IndependenceTest2< ALLOC >&& from) :
        IndependenceTest2(std::move(from), from.getAllocator()) {}


    /// destructor
    template < template < typename > class ALLOC >
    INLINE IndependenceTest2< ALLOC >::~IndependenceTest2() {
      if (_apriori != nullptr) {
        ALLOC< Apriori2< ALLOC > > allocator(this->getAllocator());
        allocator.destroy(_apriori);
        allocator.deallocate(_apriori, 1);
      }
      GUM_DESTRUCTOR(IndependenceTest2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    IndependenceTest2< ALLOC >& IndependenceTest2< ALLOC >::
                                operator=(const IndependenceTest2< ALLOC >& from) {
      if (this != &from) {
        Apriori2< ALLOC >*      new_apriori = from._apriori->clone();
        RecordCounter2< ALLOC > new_counter = from._counter;
        ScoringCache< ALLOC >   new_cache = from._cache;

        if (_apriori != nullptr) {
          ALLOC< Apriori2< ALLOC > > allocator(this->getAllocator());
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
    IndependenceTest2< ALLOC >& IndependenceTest2< ALLOC >::
                                operator=(IndependenceTest2< ALLOC >&& from) {
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
    INLINE void IndependenceTest2< ALLOC >::setMaxNbThreads(std::size_t nb) const {
      if (nb == std::size_t(0)) nb = std::size_t(1);
      _counter.setMaxNbThreads(nb);
      _max_nb_threads = nb;
    }


    /// returns the number of threads used to parse the database
    template < template < typename > class ALLOC >
    INLINE std::size_t IndependenceTest2< ALLOC >::nbThreads() const {
      return _max_nb_threads;
    }


    /// returns the score of a pair of nodes
    template < template < typename > class ALLOC >
    INLINE double IndependenceTest2< ALLOC >::score(const NodeId var1,
                                                    const NodeId var2) {
      IdSet2< ALLOC > idset(
        var1, var2, _empty_ids, false, true, this->getAllocator());
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


    /// returns the score of a pair of nodes given some other nodes
    template < template < typename > class ALLOC >
    INLINE double IndependenceTest2< ALLOC >::score(
      const NodeId                                  var1,
      const NodeId                                  var2,
      const std::vector< NodeId, ALLOC< NodeId > >& rhs_ids) {
      IdSet2< ALLOC > idset(
        var1, var2, rhs_ids, false, false, this->getAllocator());
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
    INLINE void IndependenceTest2< ALLOC >::clear() {
      _counter.clear();
      _cache.clear();
    }


    /// clears the current cache (clear nodesets as well)
    template < template < typename > class ALLOC >
    INLINE void IndependenceTest2< ALLOC >::clearCache() {
      _cache.clear();
    }


    /// turn on/off the use of a cache of the previously computed score
    template < template < typename > class ALLOC >
    INLINE void IndependenceTest2< ALLOC >::useCache(const bool on_off) {
      _use_cache = on_off;
    }


    /// return the mapping between the columns of the database and the node ids
    template < template < typename > class ALLOC >
    INLINE const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                 IndependenceTest2< ALLOC >::nodeId2Columns() const {
      return _counter.nodeId2Columns();
    }


    /// return the database used by the score
    template < template < typename > class ALLOC >
    INLINE const DatabaseTable< ALLOC >&
                 IndependenceTest2< ALLOC >::database() const {
      return _counter.database();
    }



    /// returns a counting vector where variables are marginalized from N_xyz
    /** @param node_2_marginalize indicates which node(s) shall be marginalized:
     * - 0 means that X should be marginalized
     * - 1 means that Y should be marginalized
     * - 2 means that Z should be marginalized
     */
    template < template < typename > class ALLOC >
    std::vector< double, ALLOC< double > >
    IndependenceTest2< ALLOC >::_marginalize (
      const std::size_t node_2_marginalize,
      const std::size_t X_size,
      const std::size_t Y_size,
      const std::size_t Z_size,
      const std::vector< double, ALLOC< double > >& N_xyz) const {
      // determine the size of the output vector
      std::size_t out_size = Z_size; 
      if (node_2_marginalize == std::size_t(0))
        out_size *= Y_size;
      else if (node_2_marginalize == std::size_t(1))
        out_size *= X_size;

      // allocate the output vector
      std::vector< double, ALLOC< double > > res(out_size, 0.0);
      
      // fill the vector:
      if (node_2_marginalize == std::size_t(0)) { // marginalize X
        for ( std::size_t yz = std::size_t(0), xyz = std::size_t(0);
              yz < out_size; ++yz) { 
          for ( std::size_t x = std::size_t(0); x < X_size; ++x, ++xyz) {
            res[yz] += N_xyz[xyz];
          }
        }
      }
      else if (node_2_marginalize == std::size_t(1)) { // marginalize Y
        for (std::size_t z = std::size_t(0), xyz = std::size_t(0),
               beg_xz = std::size_t(0); z < Z_size; ++z, beg_xz += X_size ) {
          for (std::size_t y = std::size_t(0); y < Y_size; ++y) {
            for (std::size_t x = std::size_t(0), xz = beg_xz; x < X_size;
                 ++x, ++xz, ++xyz) {
              res[xz] += N_xyz[xyz];
            }
          }
        }
      }
      else if (node_2_marginalize == std::size_t(2)) { // marginalize X and Y
        const std::size_t XY_size = X_size * Y_size;
        for ( std::size_t z = std::size_t(0), xyz = std::size_t(0);
              z < out_size; ++z) { 
          for ( std::size_t xy = std::size_t(0); xy < XY_size; ++xy, ++xyz) {
            res[z] += N_xyz[xyz];
          }
        }
      }
      else {
        GUM_ERROR (NotImplementedYet,
                   "_marginalize not implemented for nodeset "
                   << node_2_marginalize);
      }
 
      return res;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
