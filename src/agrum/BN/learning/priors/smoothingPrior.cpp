/**
 *
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief the smooth a priori: adds a weight w to all the counting
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/BN/learning/priors/smoothingPrior.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/BN/learning/priors/smoothingPrior_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum::learning {

  /// returns the prior vector over only the conditioning set of an idset
  void SmoothingPrior::addConditioningPseudoCount(const IdCondSet&       idset,
                                                  std::vector< double >& counts) {
    // if the conditioning set is empty or the weight is equal to zero,
    // the prior is also empty
    if ((idset.size() == idset.nbLHSIds()) || (this->weight_ == 0.0)
        || (idset.nbLHSIds() == std::size_t(0)))
      return;

    // compute the weight of the conditioning set
    double weight = this->weight_;
    if (this->nodeId2columns_.empty()) {
      for (auto i = std::size_t(0); i < idset.nbLHSIds(); ++i) {
        weight *= double(this->database_->domainSize(idset[i]));
      }
    } else {
      for (auto i = std::size_t(0); i < idset.nbLHSIds(); ++i) {
        weight *= double(this->database_->domainSize(this->nodeId2columns_.second(idset[i])));
      }
    }

    // add the weight to the counting vector
    for (auto& count: counts)
      count += weight;
  }

}   // namespace gum::learning

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
