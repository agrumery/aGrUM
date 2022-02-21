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


/**
 * @file
 * @brief Class to include at least once this header
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/BN/learning/paramUtils/paramEstimator.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/BN/learning/paramUtils/paramEstimator_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    ParamEstimator::ParamEstimator(
       const DBRowGeneratorParser&                                 parser,
       const Apriori&                                              external_apriori,
       const Apriori&                                              score_internal_apriori,
       const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
       const Bijection< NodeId, std::size_t >&                     nodeId2columns) :
        counter_(parser, ranges, nodeId2columns) {
      // copy the a prioris
      external_apriori_ = external_apriori.clone();
      try {
        score_internal_apriori_ = score_internal_apriori.clone();
      } catch (...) {
        delete external_apriori_;
        throw;
      }

      GUM_CONSTRUCTOR(ParamEstimator);
    }


    /// default constructor
    ParamEstimator::ParamEstimator(const DBRowGeneratorParser&             parser,
                                   const Apriori&                          external_apriori,
                                   const Apriori&                          score_internal_apriori,
                                   const Bijection< NodeId, std::size_t >& nodeId2columns) :
        counter_(parser, nodeId2columns) {
      // copy the a prioris
      external_apriori_ = external_apriori.clone();
      try {
        score_internal_apriori_ = score_internal_apriori.clone();
      } catch (...) {
        delete external_apriori_;
        throw;
      }

      GUM_CONSTRUCTOR(ParamEstimator);
    }


    /// destructor
    ParamEstimator::~ParamEstimator() {
      if (external_apriori_ != nullptr) delete external_apriori_;
      if (score_internal_apriori_ != nullptr) delete score_internal_apriori_;

      GUM_DESTRUCTOR(ParamEstimator);
    }


    /// copy operator
    ParamEstimator& ParamEstimator::operator=(const ParamEstimator& from) {
      if (this != &from) {
        if (external_apriori_ != nullptr) {
          delete external_apriori_;
          external_apriori_ = nullptr;
        }
        external_apriori_ = from.external_apriori_->clone();

        if (score_internal_apriori_ != nullptr) {
          delete score_internal_apriori_;
          external_apriori_ = nullptr;
        }
        score_internal_apriori_ = from.score_internal_apriori_->clone();

        counter_ = from.counter_;
      }
      return *this;
    }


    /// move operator
    ParamEstimator& ParamEstimator::operator=(ParamEstimator&& from) {
      if (this != &from) {
        external_apriori_            = from.external_apriori_;
        score_internal_apriori_      = from.score_internal_apriori_;
        counter_                     = std::move(from.counter_);
        from.external_apriori_       = nullptr;
        from.score_internal_apriori_ = nullptr;
      }
      return *this;
    }


    /// sets new ranges to perform the countings used by the score
    /** @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
     * indices. The countings are then performed only on the union of the
     * rows [Xi,Yi), i in {1,...,n}. This is useful, e.g, when performing
     * cross validation tasks, in which part of the database should be ignored.
     * An empty set of ranges is equivalent to an interval [X,Y) ranging over
     * the whole database. */
    void ParamEstimator::setRanges(
       const std::vector< std::pair< std::size_t, std::size_t > >& new_ranges) {
      std::vector< std::pair< std::size_t, std::size_t > > old_ranges = ranges();
      counter_.setRanges(new_ranges);
      if (old_ranges != ranges()) clear();
    }


    /// reset the ranges to the one range corresponding to the whole database
    void ParamEstimator::clearRanges() {
      std::vector< std::pair< std::size_t, std::size_t > > old_ranges = ranges();
      counter_.clearRanges();
      if (old_ranges != ranges()) clear();
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
