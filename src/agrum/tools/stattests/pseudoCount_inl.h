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

 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#include <agrum/tools/stattests/pseudoCount.h>
#include <agrum/tools/stattests/idCondSet.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE
       PseudoCount::PseudoCount(const DBRowGeneratorParser&                                 parser,
                                const Apriori&                                              apriori,
                                const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                                const Bijection< NodeId, std::size_t >& nodeId2columns) :
        apriori_(apriori.clone()),
        counter_(parser, ranges, nodeId2columns) {
      GUM_CONSTRUCTOR(PseudoCount);
    }


    /// default constructor
    INLINE PseudoCount::PseudoCount(const DBRowGeneratorParser&             parser,
                                    const Apriori&                          apriori,
                                    const Bijection< NodeId, std::size_t >& nodeId2columns) :
        apriori_(apriori.clone()),
        counter_(parser, nodeId2columns) {
      GUM_CONSTRUCTOR(PseudoCount);
    }


    /// copy constructor
    INLINE PseudoCount::PseudoCount(const PseudoCount& from) :
        apriori_(from.apriori_->clone()), counter_(from.counter_) {
      GUM_CONS_CPY(PseudoCount);
    }


    /// move constructor
    INLINE PseudoCount::PseudoCount(PseudoCount&& from) :
        apriori_(from.apriori_), counter_(std::move(from.counter_)) {
      from.apriori_ = nullptr;
      GUM_CONS_MOV(PseudoCount);
    }


    /// destructor
    INLINE PseudoCount::~PseudoCount() {
      if (apriori_ != nullptr) delete apriori_;
      GUM_DESTRUCTOR(PseudoCount);
    }


    /// changes the max number of threads used to parse the database
    INLINE void PseudoCount::setMaxNbThreads(std::size_t nb) const { counter_.setMaxNbThreads(nb); }


    /// returns the number of threads used to parse the database
    INLINE std::size_t PseudoCount::nbThreads() const { return counter_.nbThreads(); }


    /** @brief changes the number min of rows a thread should process in a
     * multithreading context */
    INLINE void PseudoCount::setMinNbRowsPerThread(const std::size_t nb) const {
      counter_.setMinNbRowsPerThread(nb);
    }


    /// returns the minimum of rows that each thread should process
    INLINE std::size_t PseudoCount::minNbRowsPerThread() const {
      return counter_.minNbRowsPerThread();
    }


    /// returns the current ranges
    INLINE const std::vector< std::pair< std::size_t, std::size_t > >& PseudoCount::ranges() const {
      return counter_.ranges();
    }


    /// clears all the data structures from memory
    INLINE void PseudoCount::clear() { counter_.clear(); }


    /// return the mapping between the columns of the database and the node ids
    INLINE const Bijection< NodeId, std::size_t >& PseudoCount::nodeId2Columns() const {
      return counter_.nodeId2Columns();
    }


    /// return the database used by the score
    INLINE const DatabaseTable& PseudoCount::database() const { return counter_.database(); }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */