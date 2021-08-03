/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// returns the allocator used by the independence test
    template < template < typename > class ALLOC >
    INLINE typename IndependenceTest< ALLOC >::allocator_type
       IndependenceTest< ALLOC >::getAllocator() const {
      return counter_.getAllocator();
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IndependenceTest< ALLOC >::IndependenceTest(
       const DBRowGeneratorParser< ALLOC >&                                 parser,
       const Apriori< ALLOC >&                                              apriori,
       const std::vector< std::pair< std::size_t, std::size_t >,
                          ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&        nodeId2columns,
       const typename IndependenceTest< ALLOC >::allocator_type&            alloc) :
        apriori_(apriori.clone(alloc)),
        counter_(parser, ranges, nodeId2columns, alloc), cache_(alloc) {
      GUM_CONSTRUCTOR(IndependenceTest);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IndependenceTest< ALLOC >::IndependenceTest(
       const DBRowGeneratorParser< ALLOC >&                          parser,
       const Apriori< ALLOC >&                                       apriori,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
       const typename IndependenceTest< ALLOC >::allocator_type&     alloc) :
        apriori_(apriori.clone(alloc)),
        counter_(parser, nodeId2columns, alloc), cache_(alloc) {
      GUM_CONSTRUCTOR(IndependenceTest);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IndependenceTest< ALLOC >::IndependenceTest(
       const IndependenceTest< ALLOC >&                          from,
       const typename IndependenceTest< ALLOC >::allocator_type& alloc) :
        apriori_(from.apriori_->clone(alloc)),
        counter_(from.counter_, alloc), cache_(from.cache_, alloc), use_cache_(from.use_cache_) {
      GUM_CONS_CPY(IndependenceTest);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE IndependenceTest< ALLOC >::IndependenceTest(const IndependenceTest< ALLOC >& from) :
        IndependenceTest(from, from.getAllocator()) {}


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE IndependenceTest< ALLOC >::IndependenceTest(
       IndependenceTest< ALLOC >&&                               from,
       const typename IndependenceTest< ALLOC >::allocator_type& alloc) :
        apriori_(from.apriori_),
        counter_(std::move(from.counter_), alloc), cache_(std::move(from.cache_), alloc),
        use_cache_(from.use_cache_) {
      from.apriori_ = nullptr;
      GUM_CONS_MOV(IndependenceTest);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE IndependenceTest< ALLOC >::IndependenceTest(IndependenceTest< ALLOC >&& from) :
        IndependenceTest(std::move(from), from.getAllocator()) {}


    /// destructor
    template < template < typename > class ALLOC >
    INLINE IndependenceTest< ALLOC >::~IndependenceTest() {
      if (apriori_ != nullptr) {
        ALLOC< Apriori< ALLOC > > allocator(this->getAllocator());
        allocator.destroy(apriori_);
        allocator.deallocate(apriori_, 1);
      }
      GUM_DESTRUCTOR(IndependenceTest);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    IndependenceTest< ALLOC >&
       IndependenceTest< ALLOC >::operator=(const IndependenceTest< ALLOC >& from) {
      if (this != &from) {
        Apriori< ALLOC >*      new_apriori = from.apriori_->clone();
        RecordCounter< ALLOC > new_counter = from.counter_;
        ScoringCache< ALLOC >  new_cache   = from.cache_;

        if (apriori_ != nullptr) {
          ALLOC< Apriori< ALLOC > > allocator(this->getAllocator());
          allocator.destroy(apriori_);
          allocator.deallocate(apriori_, 1);
        }

        apriori_ = new_apriori;
        counter_ = std::move(new_counter);
        cache_   = std::move(new_cache);

        use_cache_ = from.use_cache_;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    IndependenceTest< ALLOC >&
       IndependenceTest< ALLOC >::operator=(IndependenceTest< ALLOC >&& from) {
      if (this != &from) {
        std::swap(apriori_, from.apriori_);

        counter_   = std::move(from.counter_);
        cache_     = std::move(from.cache_);
        use_cache_ = from.use_cache_;
      }
      return *this;
    }


    /// changes the max number of threads used to parse the database
    template < template < typename > class ALLOC >
    INLINE void IndependenceTest< ALLOC >::setMaxNbThreads(std::size_t nb) const {
      counter_.setMaxNbThreads(nb);
    }


    /// returns the number of threads used to parse the database
    template < template < typename > class ALLOC >
    INLINE std::size_t IndependenceTest< ALLOC >::nbThreads() const {
      return counter_.nbThreads();
    }


    /** @brief changes the number min of rows a thread should process in a
     * multithreading context */
    template < template < typename > class ALLOC >
    INLINE void IndependenceTest< ALLOC >::setMinNbRowsPerThread(const std::size_t nb) const {
      counter_.setMinNbRowsPerThread(nb);
    }


    /// returns the minimum of rows that each thread should process
    template < template < typename > class ALLOC >
    INLINE std::size_t IndependenceTest< ALLOC >::minNbRowsPerThread() const {
      return counter_.minNbRowsPerThread();
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
    void IndependenceTest< ALLOC >::setRanges(
       const std::vector< std::pair< std::size_t, std::size_t >,
                          XALLOC< std::pair< std::size_t, std::size_t > > >& new_ranges) {
      std::vector< std::pair< std::size_t, std::size_t >,
                   ALLOC< std::pair< std::size_t, std::size_t > > >
         old_ranges = ranges();
      counter_.setRanges(new_ranges);
      if (old_ranges != ranges()) clear();
    }


    /// reset the ranges to the one range corresponding to the whole database
    template < template < typename > class ALLOC >
    void IndependenceTest< ALLOC >::clearRanges() {
      std::vector< std::pair< std::size_t, std::size_t >,
                   ALLOC< std::pair< std::size_t, std::size_t > > >
         old_ranges = ranges();
      counter_.clearRanges();
      if (old_ranges != ranges()) clear();
    }


    /// returns the current ranges
    template < template < typename > class ALLOC >
    INLINE const std::vector< std::pair< std::size_t, std::size_t >,
                              ALLOC< std::pair< std::size_t, std::size_t > > >&
                 IndependenceTest< ALLOC >::ranges() const {
      return counter_.ranges();
    }


    /// returns the score of a pair of nodes
    template < template < typename > class ALLOC >
    INLINE double IndependenceTest< ALLOC >::score(const NodeId var1, const NodeId var2) {
      IdCondSet< ALLOC > idset(var1, var2, empty_ids_, false, true, this->getAllocator());
      if (use_cache_) {
        try {
          return cache_.score(idset);
        } catch (NotFound&) {}
        double the_score = score_(idset);
        cache_.insert(std::move(idset), the_score);
        return the_score;
      } else {
        return score_(std::move(idset));
      }
    }


    /// returns the score of a pair of nodes given some other nodes
    template < template < typename > class ALLOC >
    INLINE double
       IndependenceTest< ALLOC >::score(const NodeId                                  var1,
                                        const NodeId                                  var2,
                                        const std::vector< NodeId, ALLOC< NodeId > >& rhs_ids) {
      IdCondSet< ALLOC > idset(var1, var2, rhs_ids, false, false, this->getAllocator());
      if (use_cache_) {
        try {
          return cache_.score(idset);
        } catch (NotFound&) {}
        double the_score = score_(idset);
        cache_.insert(std::move(idset), the_score);
        return the_score;
      } else {
        return score_(idset);
      }
    }


    /// clears all the data structures from memory
    template < template < typename > class ALLOC >
    INLINE void IndependenceTest< ALLOC >::clear() {
      counter_.clear();
      cache_.clear();
    }


    /// clears the current cache (clear nodesets as well)
    template < template < typename > class ALLOC >
    INLINE void IndependenceTest< ALLOC >::clearCache() {
      cache_.clear();
    }


    /// turn on/off the use of a cache of the previously computed score
    template < template < typename > class ALLOC >
    INLINE void IndependenceTest< ALLOC >::useCache(const bool on_off) {
      use_cache_ = on_off;
    }


    /// return the mapping between the columns of the database and the node ids
    template < template < typename > class ALLOC >
    INLINE const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
                 IndependenceTest< ALLOC >::nodeId2Columns() const {
      return counter_.nodeId2Columns();
    }


    /// return the database used by the score
    template < template < typename > class ALLOC >
    INLINE const DatabaseTable< ALLOC >& IndependenceTest< ALLOC >::database() const {
      return counter_.database();
    }


    /// returns a counting vector where variables are marginalized from N_xyz
    /** @param node_2_marginalize indicates which node(s) shall be marginalized:
     * - 0 means that X should be marginalized
     * - 1 means that Y should be marginalized
     * - 2 means that Z should be marginalized
     */
    template < template < typename > class ALLOC >
    std::vector< double, ALLOC< double > > IndependenceTest< ALLOC >::marginalize_(
       const std::size_t                             node_2_marginalize,
       const std::size_t                             X_size,
       const std::size_t                             Y_size,
       const std::size_t                             Z_size,
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
      if (node_2_marginalize == std::size_t(0)) {   // marginalize X
        for (std::size_t yz = std::size_t(0), xyz = std::size_t(0); yz < out_size; ++yz) {
          for (std::size_t x = std::size_t(0); x < X_size; ++x, ++xyz) {
            res[yz] += N_xyz[xyz];
          }
        }
      } else if (node_2_marginalize == std::size_t(1)) {   // marginalize Y
        for (std::size_t z = std::size_t(0), xyz = std::size_t(0), beg_xz = std::size_t(0);
             z < Z_size;
             ++z, beg_xz += X_size) {
          for (std::size_t y = std::size_t(0); y < Y_size; ++y) {
            for (std::size_t x = std::size_t(0), xz = beg_xz; x < X_size; ++x, ++xz, ++xyz) {
              res[xz] += N_xyz[xyz];
            }
          }
        }
      } else if (node_2_marginalize == std::size_t(2)) {   // marginalize X and Y
        const std::size_t XY_size = X_size * Y_size;
        for (std::size_t z = std::size_t(0), xyz = std::size_t(0); z < out_size; ++z) {
          for (std::size_t xy = std::size_t(0); xy < XY_size; ++xy, ++xyz) {
            res[z] += N_xyz[xyz];
          }
        }
      } else {
        GUM_ERROR(NotImplementedYet,
                  "_marginalize not implemented for nodeset " << node_2_marginalize);
      }

      return res;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
