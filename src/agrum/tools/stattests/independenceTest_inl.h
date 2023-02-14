/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

    /// default constructor
    INLINE IndependenceTest::IndependenceTest(
       const DBRowGeneratorParser&                                 parser,
       const Prior&                                                prior,
       const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
       const Bijection< NodeId, std::size_t >&                     nodeId2columns) :
        prior_(prior.clone()),
        counter_(parser, ranges, nodeId2columns) {
      GUM_CONSTRUCTOR(IndependenceTest);
    }


    /// default constructor
    INLINE
    IndependenceTest::IndependenceTest(const DBRowGeneratorParser&             parser,
                                       const Prior&                            prior,
                                       const Bijection< NodeId, std::size_t >& nodeId2columns) :
        prior_(prior.clone()),
        counter_(parser, nodeId2columns) {
      GUM_CONSTRUCTOR(IndependenceTest);
    }


    /// copy constructor
    INLINE IndependenceTest::IndependenceTest(const IndependenceTest& from) :
        prior_(from.prior_->clone()), counter_(from.counter_), cache_(from.cache_),
        use_cache_(from.use_cache_) {
      GUM_CONS_CPY(IndependenceTest);
    }


    /// move constructor
    INLINE IndependenceTest::IndependenceTest(IndependenceTest&& from) :
        prior_(from.prior_), counter_(std::move(from.counter_)), cache_(std::move(from.cache_)),
        use_cache_(from.use_cache_) {
      from.prior_ = nullptr;
      GUM_CONS_MOV(IndependenceTest);
    }


    /// destructor
    INLINE IndependenceTest::~IndependenceTest() {
      if (prior_ != nullptr) delete prior_;
      GUM_DESTRUCTOR(IndependenceTest);
    }


    /// changes the max number of threads used to parse the database
    INLINE void IndependenceTest::setNumberOfThreads(Size nb) { counter_.setNumberOfThreads(nb); }


    /// returns the number of threads used to parse the database
    INLINE Size IndependenceTest::getNumberOfThreads() const {
      return counter_.getNumberOfThreads();
    }


    /// indicates whether the user set herself the number of threads
    INLINE bool IndependenceTest::isGumNumberOfThreadsOverriden() const {
      return counter_.isGumNumberOfThreadsOverriden();
    }


    /** @brief changes the number min of rows a thread should process in a
     * multithreading context */
    INLINE void IndependenceTest::setMinNbRowsPerThread(const std::size_t nb) const {
      counter_.setMinNbRowsPerThread(nb);
    }


    /// returns the minimum of rows that each thread should process
    INLINE std::size_t IndependenceTest::minNbRowsPerThread() const {
      return counter_.minNbRowsPerThread();
    }


    /// returns the current ranges
    INLINE const std::vector< std::pair< std::size_t, std::size_t > >&
                 IndependenceTest::ranges() const {
      return counter_.ranges();
    }


    /// returns the score of a pair of nodes
    INLINE double IndependenceTest::score(const NodeId var1, const NodeId var2) {
      IdCondSet idset(var1, var2, empty_ids_, false, true);
      if (use_cache_) {
        try {
          return cache_.score(idset);
        } catch (NotFound const&) {}
        double the_score = score_(idset);
        cache_.insert(std::move(idset), the_score);
        return the_score;
      } else {
        return score_(std::move(idset));
      }
    }


    /// returns the score of a pair of nodes given some other nodes
    INLINE double IndependenceTest::score(const NodeId                 var1,
                                          const NodeId                 var2,
                                          const std::vector< NodeId >& rhs_ids) {
      IdCondSet idset(var1, var2, rhs_ids, false, false);
      if (use_cache_) {
        try {
          return cache_.score(idset);
        } catch (NotFound const&) {}
        double the_score = score_(idset);
        cache_.insert(std::move(idset), the_score);
        return the_score;
      } else {
        return score_(idset);
      }
    }


    /// clears all the data structures from memory
    INLINE void IndependenceTest::clear() {
      counter_.clear();
      cache_.clear();
    }


    /// clears the current cache (clear nodesets as well)
    INLINE void IndependenceTest::clearCache() { cache_.clear(); }


    /// turn on/off the use of a cache of the previously computed score
    INLINE void IndependenceTest::useCache(const bool on_off) { use_cache_ = on_off; }


    /// return the mapping between the columns of the database and the node ids
    INLINE const Bijection< NodeId, std::size_t >& IndependenceTest::nodeId2Columns() const {
      return counter_.nodeId2Columns();
    }


    /// return the database used by the score
    INLINE const DatabaseTable& IndependenceTest::database() const { return counter_.database(); }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
