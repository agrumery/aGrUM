/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


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
        const Prior&                                                external_prior,
        const Prior&                                                score_internal_prior,
        const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
        const Bijection< NodeId, std::size_t >&                     nodeId2columns) :
        counter_(parser, ranges, nodeId2columns) {
      // copy the a prioris
      external_prior_ = external_prior.clone();
      try {
        score_internal_prior_ = score_internal_prior.clone();
      } catch (...) {
        delete external_prior_;
        throw;
      }

      GUM_CONSTRUCTOR(ParamEstimator);
    }

    /// default constructor
    ParamEstimator::ParamEstimator(const DBRowGeneratorParser&             parser,
                                   const Prior&                            external_prior,
                                   const Prior&                            score_internal_prior,
                                   const Bijection< NodeId, std::size_t >& nodeId2columns) :
        counter_(parser, nodeId2columns) {
      // copy the a prioris
      external_prior_ = external_prior.clone();
      try {
        score_internal_prior_ = score_internal_prior.clone();
      } catch (...) {
        delete external_prior_;
        throw;
      }

      GUM_CONSTRUCTOR(ParamEstimator);
    }

    /// destructor
    ParamEstimator::~ParamEstimator() {
      if (external_prior_ != nullptr) delete external_prior_;
      if (score_internal_prior_ != nullptr) delete score_internal_prior_;

      GUM_DESTRUCTOR(ParamEstimator);
    }

    /// copy operator
    ParamEstimator& ParamEstimator::operator=(const ParamEstimator& from) {
      if (this != &from) {
        if (external_prior_ != nullptr) {
          delete external_prior_;
          external_prior_ = nullptr;
        }
        external_prior_ = from.external_prior_->clone();

        if (score_internal_prior_ != nullptr) {
          delete score_internal_prior_;
          external_prior_ = nullptr;
        }
        score_internal_prior_ = from.score_internal_prior_->clone();

        counter_ = from.counter_;
      }
      return *this;
    }

    /// move operator
    ParamEstimator& ParamEstimator::operator=(ParamEstimator&& from) {
      if (this != &from) {
        external_prior_            = from.external_prior_;
        score_internal_prior_      = from.score_internal_prior_;
        counter_                   = std::move(from.counter_);
        from.external_prior_       = nullptr;
        from.score_internal_prior_ = nullptr;
      }
      return *this;
    }

    /// sets new ranges to perform the counts used by the score
    /** @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
     * indices. The counts are then performed only on the union of the
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
