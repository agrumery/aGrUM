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

#pragma once


/** @file
 * @brief Source implementation of IBayesNetGenerator
 *
 * @author Christophe GONZALES(_at_AMU), Pierre-Henri WUILLEMIN(_at_LIP6), Lionel TORTI
 *and Ariele-Paolo MAESANO
 *
 */
#include <agrum/base/variables/rangeVariable.h>
#include <agrum/BN/generator/IBayesNetGenerator.h>

namespace gum {

  // Default constructor.
  // Use the SimpleCPTGenerator for generating the BNs CPT.
  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::IBayesNetGenerator(Size nbrNodes,
                                                                             Size maxArcs,
                                                                             Size maxModality) :
      dag_() {
    GUM_CONSTRUCTOR(IBayesNetGenerator);
    nbrNodes_ = nbrNodes;

    if (maxArcs < nbrNodes - 1 || maxArcs > (nbrNodes * (nbrNodes - 1)) / 2)
      GUM_ERROR(OperationNotAllowed, " maxArcs value not possible ")

    if (maxModality < 2) GUM_ERROR(OperationNotAllowed, " maxModality must be at least equal to 2 ")

    maxArcs_     = maxArcs;
    maxModality_ = maxModality;
  }

  // Destructor.
  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::~IBayesNetGenerator() {
    GUM_DESTRUCTOR(IBayesNetGenerator);
    //    delete cptGenerator_;
  }

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  void IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::fillCPT(BayesNet< GUM_SCALAR >& bn) const {
    for (auto node: bn.nodes())
      this->generateCPT(bn.cpt(node).pos(bn.variable(node)), bn.cpt(node));
  }

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE Size IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::maxModality() const {
    return maxModality_;
  }

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE Size IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::nbrNodes() const {
    return nbrNodes_;
  }

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE Size IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::maxArcs() const {
    return maxArcs_;
  }

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE void IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::setMaxModality(Size maxModality) {
    if (maxModality < 2)
      GUM_ERROR(OperationNotAllowed, " maxModality must be at least equal to two ")

    maxModality_ = maxModality;
  }

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE void IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::setNbrNodes(Size nbrNodes) {
    if ((maxArcs_ < nbrNodes - 1) || (maxArcs_ > (nbrNodes * (nbrNodes - 1)) / 2))
      GUM_ERROR(OperationNotAllowed, " nbrNodes value not possible ")

    nbrNodes_ = nbrNodes;
  }

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE void IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::setMaxArcs(Size maxArcs) {
    if (maxArcs < nbrNodes_ - 1 || maxArcs > (nbrNodes_ * (nbrNodes_ - 1)) / 2)
      GUM_ERROR(OperationNotAllowed, " maxArcs value not possible ")

    maxArcs_ = maxArcs;
  }

  template < typename GUM_SCALAR, template < typename > class ICPTGenerator >
  INLINE void IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >::fromDAG(BayesNet< GUM_SCALAR >& bn) {
    bn.clear();

    const auto  width = (this->dag_.size() >= 100) ? 3 : 2;
    int         n     = 0;
    const auto& topo  = this->dag_.topologicalOrder();
    for (const auto node: topo) {
      std::stringstream strBuff;
      strBuff << "X" << std::setfill('0') << std::setw(width) << n++;
      bn.add(RangeVariable(strBuff.str(), "", 0, long(1 + randomValue(this->maxModality_ - 1))),
             node);
    }
    bn.beginTopologyTransformation();
    for (auto arc: this->dag_.arcs()) {
      bn.addArc(arc.tail(), arc.head());
    }
    bn.endTopologyTransformation();
  }


} /* namespace gum */
