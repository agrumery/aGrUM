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





/** @file
 * @brief Source implementation of MaxParentsMCBayesNetGenerator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Ariele Maesano
 *
 */

#include <agrum/BN/generator/maxParentsMCBayesNetGenerator.h>

namespace gum {

  // Default constructor.
  // Use the SimpleCPTGenerator for generating the BNs CPT.
  template < typename GUM_SCALAR,
             template < class > class ICPTGenerator,
             template < class > class ICPTDisturber >
  INLINE MaxParentsMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::
      MaxParentsMCBayesNetGenerator(Size nbrNodes,
                                    Size maxArcs,
                                    Size maxModality,
                                    Size maxParents,
                                    Idx  iteration,
                                    Idx  p,
                                    Idx  q) :
      MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >(nbrNodes,
                                                                      maxArcs,
                                                                      maxModality,
                                                                      iteration,
                                                                      p,
                                                                      q) {
    if (maxParents == 0)
      GUM_ERROR(OperationNotAllowed,
                "maxParents must be at least equal to 1 to have a connexe graph")

    maxParents_ = maxParents;
    GUM_CONSTRUCTOR(MaxParentsMCBayesNetGenerator);
  }

  template < typename GUM_SCALAR,
             template < class > class ICPTGenerator,
             template < class > class ICPTDisturber >
  INLINE MaxParentsMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::
      MaxParentsMCBayesNetGenerator(BayesNet< GUM_SCALAR > bayesNet,
                                    Size                   maxParents,
                                    Idx                    iteration,
                                    Idx                    p,
                                    Idx                    q) :
      MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >(bayesNet, iteration, p, q) {
    maxParents_ = maxParents;
    GUM_CONSTRUCTOR(MaxParentsMCBayesNetGenerator);
  }

  // Destructor.
  template < typename GUM_SCALAR,
             template < class > class ICPTGenerator,
             template < class > class ICPTDisturber >
  INLINE MaxParentsMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::
      ~MaxParentsMCBayesNetGenerator() {
    GUM_DESTRUCTOR(MaxParentsMCBayesNetGenerator);
  }

  template < typename GUM_SCALAR,
             template < class > class ICPTGenerator,
             template < class > class ICPTDisturber >
  bool MaxParentsMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::
      _checkConditions_() {
    for (auto node: this->dag_.nodes())
      if (this->dag_.parents(node).size() > maxParents_) return false;

    return MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_checkConditions_();
  }

  template < typename GUM_SCALAR,
             template < class > class ICPTGenerator,
             template < class > class ICPTDisturber >
  INLINE Size
      MaxParentsMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::maxParents()
          const {
    return maxParents_;
  }

  template < typename GUM_SCALAR,
             template < class > class ICPTGenerator,
             template < class > class ICPTDisturber >
  INLINE void
      MaxParentsMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::setMaxParents(
          Size maxParents) {
    if (maxParents == 0)
      GUM_ERROR(OperationNotAllowed,
                "maxParents must be at least equal to 1 to have a connexe graph")

    maxParents_ = maxParents;
  }
} /* namespace gum */
