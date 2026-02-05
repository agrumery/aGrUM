/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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
 * @brief Implementation of gum::learning::Score.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/scores_and_tests/score.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/BN/learning/scores_and_tests/score_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// copy operator
    Score& Score::operator=(const Score& from) {
      if (this != &from) {
        Prior*        new_prior   = from.prior_->clone();
        RecordCounter new_counter = from.counter_;
        ScoringCache  new_cache   = from.cache_;

        if (prior_ != nullptr) delete prior_;

        prior_   = new_prior;
        counter_ = std::move(new_counter);
        cache_   = std::move(new_cache);

        use_cache_ = from.use_cache_;
      }
      return *this;
    }

    /// move operator
    Score& Score::operator=(Score&& from) {
      if (this != &from) {
        std::swap(prior_, from.prior_);

        counter_   = std::move(from.counter_);
        cache_     = std::move(from.cache_);
        use_cache_ = from.use_cache_;
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
    void Score::setRanges(const std::vector< std::pair< std::size_t, std::size_t > >& new_ranges) {
      std::vector< std::pair< std::size_t, std::size_t > > old_ranges = ranges();
      counter_.setRanges(new_ranges);
      if (old_ranges != ranges()) clear();
    }

    /// reset the ranges to the one range corresponding to the whole database
    void Score::clearRanges() {
      std::vector< std::pair< std::size_t, std::size_t > > old_ranges = ranges();
      counter_.clearRanges();
      if (old_ranges != ranges()) clear();
    }

    /// returns a counting vector where variables are marginalized from N_xyz
    /** @param X_size the domain size of the variable to marginalize (this
     * is the first variable in table N_xyz
     * @param N_xyz a counting vector of dimension X * cond_vars (in this order)
     */
    std::vector< double > Score::marginalize_(const NodeId                 X_id,
                                              const std::vector< double >& N_xyz) const {
      // compute the domain sizes of the varible on the left hand side
      // of the conditioning bar
      const auto&       nodeId2cols = this->counter_.nodeId2Columns();
      const auto&       database    = this->counter_.database();
      const std::size_t X_size
          = database.domainSize(nodeId2cols.empty() ? X_id : nodeId2cols.second(X_id));

      // determine the size of the output vector
      std::size_t out_size = N_xyz.size() / X_size;

      // allocate the output vector
      std::vector< double > res(out_size, 0.0);

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
