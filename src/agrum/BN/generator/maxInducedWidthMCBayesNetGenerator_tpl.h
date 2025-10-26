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
#pragma once


/** @file
 * @brief Source implementation of MaxInducedWidthMCBayesNetGenerator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Ariele Maesano
 *
 */

#include <agrum/BN/generator/maxInducedWidthMCBayesNetGenerator.h>

namespace gum {
#define MCBG MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >
#define IBNG IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >

  // Default constructor.
  // Use the SimpleCPTGenerator for generating the BNs CPT.
  template < typename GUM_SCALAR,
             template < typename > class ICPTGenerator,
             template < typename > class ICPTDisturber >
  INLINE MaxInducedWidthMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::
      MaxInducedWidthMCBayesNetGenerator(Size nbrNodes,
                                         Size maxArcs,
                                         Size maxModality,
                                         Size maxInducedWidth,
                                         Idx  iteration,
                                         Idx  p,
                                         Idx  q) :
      MCBG(nbrNodes, maxArcs, maxModality, iteration, p, q) {
    if (maxInducedWidth == 0)
      GUM_ERROR(OperationNotAllowed,
                "maxInducedWidth must be at least equal "
                "to 1 to have a connexe graph");

    maxlog10InducedWidth_ = maxInducedWidth;
    GUM_CONSTRUCTOR(MaxInducedWidthMCBayesNetGenerator);
  }

  template < typename GUM_SCALAR,
             template < typename > class ICPTGenerator,
             template < typename > class ICPTDisturber >
  INLINE MaxInducedWidthMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::
      MaxInducedWidthMCBayesNetGenerator(BayesNet< GUM_SCALAR > bayesNet,
                                         Size                   maxInducedWidth,
                                         Idx                    iteration,
                                         Idx                    p,
                                         Idx q) : MCBG(bayesNet, iteration, p, q) {
    maxlog10InducedWidth_ = maxInducedWidth;
    GUM_CONSTRUCTOR(MaxInducedWidthMCBayesNetGenerator);
  }

  // Use this constructor if you want to use a different policy for generating
  // CPT than the default one.
  // The cptGenerator will be erased when the destructor is called.
  // @param cptGenerator The policy used to generate CPT.
  /*template<typename GUM_SCALAR, template<class> class ICPTGenerator,
  template<class> class ICPDisturber>
  MaxInducedWidthMCBayesNetGenerator<GUM_SCALAR,ICPTGenerator,ICPTDisturber>::MaxInducedWidthMCBayesNetGenerator(
  CPTGenerator* cptGenerator,Size nbrNodes,  Idx p,Idx q,Idx iteration,float
  maxDensity , Size max_modality, Size maxInducedWidth):
  MCBG<GUM_SCALAR,ICPTGenerator,ICPTDisturber>(cptGenerator,
  nbrNodes,p,q,iteration, maxDensity,max_modality, maxInducedWidth){
      GUM_CONSTRUCTOR(MaxInducedWidthMCBayesNetGenerator);
  }*/

  // Destructor.
  template < typename GUM_SCALAR,
             template < typename > class ICPTGenerator,
             template < typename > class ICPTDisturber >
  INLINE MaxInducedWidthMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::
      ~MaxInducedWidthMCBayesNetGenerator() {
    GUM_DESTRUCTOR(MaxInducedWidthMCBayesNetGenerator);
    //    delete BayesNetGenerator<GUM_SCALAR>::cptGenerator_;
  }

  template < typename GUM_SCALAR,
             template < typename > class ICPTGenerator,
             template < typename > class ICPTDisturber >
  bool MaxInducedWidthMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::
      _checkConditions_() {
    NodeProperty< Size > _modalitiesMap_;

    for (auto node: this->dag_.nodes())
      _modalitiesMap_.insert(node, 2);   //@todo take modalities into account...by randomly add a
                                         //_modalitiesMap_ for instance  ...

    const auto           moralg = this->dag_.moralGraph();
    DefaultTriangulation tri(&moralg, &_modalitiesMap_);

    if (tri.maxLog10CliqueDomainSize() > maxlog10InducedWidth_) return false;

    return MCBG::_checkConditions_();
  }

  template < typename GUM_SCALAR,
             template < typename > class ICPTGenerator,
             template < typename > class ICPTDisturber >
  INLINE Size MaxInducedWidthMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::
      maxlog10InducedWidth() const {
    return maxlog10InducedWidth_;
  }

  template < typename GUM_SCALAR,
             template < typename > class ICPTGenerator,
             template < typename > class ICPTDisturber >
  INLINE void MaxInducedWidthMCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::
      setMaxlog10InducedWidth(Size maxlog10InducedWidth) {
    if (maxlog10InducedWidth == 0)
      GUM_ERROR(OperationNotAllowed,
                "maxInducedWidth must be at least equal "
                "to 1 to have a connexe graph");

    maxlog10InducedWidth_ = maxlog10InducedWidth;
  }
} /* namespace gum */
