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
 * @brief the base class for all the scores used for learning (BIC, BDeu, etc)
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE Score::Score(const DBRowGeneratorParser&                                 parser,
                        const Prior&                                                prior,
                        const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                        const Bijection< NodeId, std::size_t >& nodeId2columns) :
        prior_(prior.clone()),
        counter_(parser, ranges, nodeId2columns) {
      GUM_CONSTRUCTOR(Score);
    }


    /// default constructor
    INLINE Score::Score(const DBRowGeneratorParser&             parser,
                        const Prior&                            prior,
                        const Bijection< NodeId, std::size_t >& nodeId2columns) :
        prior_(prior.clone()),
        counter_(parser, nodeId2columns) {
      GUM_CONSTRUCTOR(Score);
    }


    /// copy constructor
    INLINE Score::Score(const Score& from) :
        prior_(from.prior_->clone()), counter_(from.counter_), cache_(from.cache_),
        use_cache_(from.use_cache_) {
      GUM_CONS_CPY(Score);
    }


    /// move constructor
    INLINE Score::Score(Score&& from) :
        prior_(from.prior_), counter_(std::move(from.counter_)), cache_(std::move(from.cache_)),
        use_cache_(from.use_cache_) {
      from.prior_ = nullptr;
      GUM_CONS_MOV(Score);
    }


    /// destructor
    INLINE Score::~Score() {
      if (prior_ != nullptr) delete prior_;
      GUM_DESTRUCTOR(Score);
    }


    /// changes the max number of threads used to parse the database
    INLINE void Score::setNumberOfThreads(Size nb) { counter_.setNumberOfThreads(nb); }


    /// returns the current max number of threads of the scheduler
    INLINE Size Score::getNumberOfThreads() const { return counter_.getNumberOfThreads(); }


    /// indicates whether the user set herself the number of threads
    INLINE bool Score::isGumNumberOfThreadsOverriden() const {
      return counter_.isGumNumberOfThreadsOverriden();
    }


    /** @brief changes the number min of rows a thread should process in a
     * multithreading context */
    INLINE void Score::setMinNbRowsPerThread(const std::size_t nb) const {
      counter_.setMinNbRowsPerThread(nb);
    }


    /// returns the minimum of rows that each thread should process
    INLINE std::size_t Score::minNbRowsPerThread() const { return counter_.minNbRowsPerThread(); }


    /// returns the current ranges
    INLINE const std::vector< std::pair< std::size_t, std::size_t > >& Score::ranges() const {
      return counter_.ranges();
    }


    /// returns the score of a single node
    INLINE double Score::score(const NodeId var) {
      IdCondSet idset(var, empty_ids_, true);
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


    /// returns the score of a single node given some other nodes
    /** @param var1 the variable on the left side of the conditioning bar
     * @param rhs_ids the set of variables on the right side of the
     * conditioning bar */
    INLINE double Score::score(const NodeId var, const std::vector< NodeId >& rhs_ids) {
      IdCondSet idset(var, rhs_ids, false);
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
    INLINE void Score::clear() {
      counter_.clear();
      cache_.clear();
    }


    /// clears the current cache (clear nodesets as well)
    INLINE void Score::clearCache() { cache_.clear(); }


    /// turn on/off the use of a cache of the previously computed score
    INLINE void Score::useCache(const bool on_off) { use_cache_ = on_off; }


    /// indicates whether the score uses a cache
    INLINE bool Score::isUsingCache() const { return use_cache_; }


    /// return the mapping between the columns of the database and the node ids
    INLINE const Bijection< NodeId, std::size_t >& Score::nodeId2Columns() const {
      return counter_.nodeId2Columns();
    }


    /// return the database used by the score
    INLINE const DatabaseTable& Score::database() const { return counter_.database(); }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
