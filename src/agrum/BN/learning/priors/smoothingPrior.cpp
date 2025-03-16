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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





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
