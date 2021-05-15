/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Source implementation of MaxParentsMCBayesNetGenerator
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Ariele Maesano
 *
 */

#include <agrum/BN/generator/maxParentsMCBayesNetGenerator.h>

namespace gum {

#ifdef _MSC_VER
#  define MCBG MCBayesNetGenerator
#else
#  define MCBG MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >
#endif

  // Default constructor.
  // Use the SimpleCPTGenerator for generating the BNs CPT.
  template < typename GUM_SCALAR,
             template < class >
             class ICPTGenerator,
             template < class >
             class ICPTDisturber >
  INLINE
     MaxParentsMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::
        MaxParentsMCBayesNetGenerator(Size nbrNodes,
                                      Size maxArcs,
                                      Size maxModality,
                                      Size maxParents,
                                      Idx  iteration,
                                      Idx  p,
                                      Idx  q) :
      MCBG(nbrNodes, maxArcs, maxModality, iteration, p, q) {
    if (maxParents == 0)
      GUM_ERROR(OperationNotAllowed,
                "maxParents must be at least equal to 1 to have a connexe graph")

    maxParents_ = maxParents;
    GUM_CONSTRUCTOR(MaxParentsMCBayesNetGenerator);
  }

  template < typename GUM_SCALAR,
             template < class >
             class ICPTGenerator,
             template < class >
             class ICPTDisturber >
  INLINE
     MaxParentsMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::
        MaxParentsMCBayesNetGenerator(BayesNet< GUM_SCALAR > bayesNet,
                                      Size                   maxParents,
                                      Idx                    iteration,
                                      Idx                    p,
                                      Idx                    q) :
      MCBG(bayesNet, iteration, p, q) {
    maxParents_ = maxParents;
    GUM_CONSTRUCTOR(MaxParentsMCBayesNetGenerator);
  }

  // Destructor.
  template < typename GUM_SCALAR,
             template < class >
             class ICPTGenerator,
             template < class >
             class ICPTDisturber >
  INLINE
     MaxParentsMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::
        ~MaxParentsMCBayesNetGenerator() {
    GUM_DESTRUCTOR(MaxParentsMCBayesNetGenerator);
  }

  template < typename GUM_SCALAR,
             template < class >
             class ICPTGenerator,
             template < class >
             class ICPTDisturber >
  bool MaxParentsMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::
     checkConditions__() {
    for (auto node: this->bayesNet_.nodes())
      if (this->bayesNet_.parents(node).size() > maxParents_) return false;

    return MCBG::checkConditions__();
  }

  template < typename GUM_SCALAR,
             template < class >
             class ICPTGenerator,
             template < class >
             class ICPTDisturber >
  INLINE Size MaxParentsMCBayesNetGenerator< GUM_SCALAR,
                                             ICPTGenerator,
                                             ICPTDisturber >::maxParents() const {
    return maxParents_;
  }
  template < typename GUM_SCALAR,
             template < class >
             class ICPTGenerator,
             template < class >
             class ICPTDisturber >
  INLINE void
     MaxParentsMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::
        setMaxParents(Size maxParents) {
    if (maxParents == 0)
      GUM_ERROR(OperationNotAllowed,
                "maxParents must be at least equal to 1 to have a connexe graph")

    maxParents_ = maxParents;
  }
} /* namespace gum */
