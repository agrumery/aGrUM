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
 * @brief Implementation of gum::learning::PseudoCount
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/base/stattests/pseudoCount.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/stattests/pseudoCount_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// copy operator
    PseudoCount& PseudoCount::operator=(const PseudoCount& from) {
      if (this != &from) {
        Prior*        new_prior   = from.prior_->clone();
        RecordCounter new_counter = from.counter_;

        if (prior_ != nullptr) delete prior_;

        prior_   = new_prior;
        counter_ = std::move(new_counter);
      }
      return *this;
    }

    /// move operator
    PseudoCount& PseudoCount::operator=(PseudoCount&& from) {
      if (this != &from) {
        std::swap(prior_, from.prior_);

        counter_ = std::move(from.counter_);
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
    void PseudoCount::setRanges(
        const std::vector< std::pair< std::size_t, std::size_t > >& new_ranges) {
      std::vector< std::pair< std::size_t, std::size_t > > old_ranges = ranges();
      counter_.setRanges(new_ranges);
      if (old_ranges != ranges()) clear();
    }

    /// reset the ranges to the one range corresponding to the whole database
    void PseudoCount::clearRanges() {
      std::vector< std::pair< std::size_t, std::size_t > > old_ranges = ranges();
      counter_.clearRanges();
    }

    /// returns a counting vector where variables are marginalized from N_xyz
    /** @param node_2_marginalize indicates which node(s) shall be marginalized:
     * - 0 means that X should be marginalized
     * - 1 means that Y should be marginalized
     * - 2 means that Z should be marginalized
     */
    std::vector< double > PseudoCount::get(const std::vector< NodeId >& ids) {
      IdCondSet             idset(ids, false, true);
      std::vector< double > N_xyz(this->counter_.counts(idset, true));
      const bool            informative_external_prior = this->prior_->isInformative();
      if (informative_external_prior) this->prior_->addJointPseudoCount(idset, N_xyz);
      return N_xyz;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
