/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Implementation of gum::learning::PseudoCount
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/tools/stattests/pseudoCount.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/stattests/pseudoCount_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// copy operator
    PseudoCount& PseudoCount::operator=(const PseudoCount& from) {
      if (this != &from) {
        Prior*        new_apriori = from.apriori_->clone();
        RecordCounter new_counter = from.counter_;

        if (apriori_ != nullptr) delete apriori_;

        apriori_ = new_apriori;
        counter_ = std::move(new_counter);
      }
      return *this;
    }


    /// move operator
    PseudoCount& PseudoCount::operator=(PseudoCount&& from) {
      if (this != &from) {
        std::swap(apriori_, from.apriori_);

        counter_ = std::move(from.counter_);
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
      const bool            informative_external_apriori = this->apriori_->isInformative();
      if (informative_external_apriori) this->apriori_->addAllApriori(idset, N_xyz);
      return N_xyz;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
