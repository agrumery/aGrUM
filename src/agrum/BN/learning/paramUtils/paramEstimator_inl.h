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
 * @brief the base class for estimating parameters of CPTs
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/BN/learning/paramUtils/paramEstimator.h>

namespace gum {

  namespace learning {

    /// copy constructor
    INLINE ParamEstimator::ParamEstimator(const ParamEstimator& from) :
        external_prior_(from.external_prior_->clone()),
        score_internal_prior_(from.score_internal_prior_->clone()), counter_(from.counter_) {
      GUM_CONS_CPY(ParamEstimator);
    }


    /// move constructor
    INLINE ParamEstimator::ParamEstimator(ParamEstimator&& from) :
        external_prior_(from.external_prior_), score_internal_prior_(from.score_internal_prior_),
        counter_(std::move(from.counter_)) {
      from.external_prior_       = nullptr;
      from.score_internal_prior_ = nullptr;
      GUM_CONS_MOV(ParamEstimator);
    }


    /// clears all the data structures from memory
    INLINE void ParamEstimator::clear() { counter_.clear(); }


    /// changes the max number of threads used to parse the database
    INLINE void ParamEstimator::setNumberOfThreads(Size nb) { counter_.setNumberOfThreads(nb); }


    /// returns the current max number of threads of the scheduler
    INLINE Size ParamEstimator::getNumberOfThreads() const { return counter_.getNumberOfThreads(); }


    /// indicates whether the user set herself the number of threads
    INLINE bool ParamEstimator::isGumNumberOfThreadsOverriden() const {
      return counter_.isGumNumberOfThreadsOverriden();
    }


    /** @brief changes the number min of rows a thread should process in a
     * multithreading context */
    INLINE void ParamEstimator::setMinNbRowsPerThread(const std::size_t nb) const {
      counter_.setMinNbRowsPerThread(nb);
    }


    /// returns the minimum of rows that each thread should process
    INLINE std::size_t ParamEstimator::minNbRowsPerThread() const {
      return counter_.minNbRowsPerThread();
    }


    /// returns the current ranges
    INLINE const std::vector< std::pair< std::size_t, std::size_t > >&
                 ParamEstimator::ranges() const {
      return counter_.ranges();
    }


    /// returns the CPT's parameters corresponding to a given target node
    INLINE std::vector< double > ParamEstimator::parameters(const NodeId target_node) {
      return parameters(target_node, empty_nodevect_);
    }


    /// returns the mapping from ids to column positions in the database
    INLINE const Bijection< NodeId, std::size_t >& ParamEstimator::nodeId2Columns() const {
      return counter_.nodeId2Columns();
    }


    /// returns the database on which we perform the counts
    INLINE const DatabaseTable& ParamEstimator::database() const { return counter_.database(); }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
