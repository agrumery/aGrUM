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
 * @brief Pruning of the domains of the variables of a Bayesian network by the hard evidence
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_EVIDENCE_PRUNING_H
#define GUM_EVIDENCE_PRUNING_H

#include <vector>

#include <agrum/BN/IBayesNet.h>

namespace gum {

  /**
   * @brief Prunes the domains of the variables by generalized arc consistency on the null
   * probabilities of the CPTs, starting from the hard evidence.
   *
   * A value of a variable is kept if, in each CPT containing the variable, it belongs to an entry
   * with a positive probability whose values are all kept. The hard evidence restricts the domain
   * of its variable to the observed value.
   *
   * The pruning is sound: an empty domain proves that the evidence has a null probability. It is
   * not complete: an incompatibility involving several CPTs together (e.g. a parity constraint
   * over a chain of XOR) may not be detected.
   *
   * Complexity: each CPT with a null probability is revised each time the domain of one of its
   * variables shrinks, and a revision is linear in the size of the CPT.
   *
   * @param bn the Bayesian network.
   * @param hardEv the hard evidence (node -> observed value), may be nullptr.
   * @param allowed (output) for each node, the values that may belong to a sample with a positive
   * probability (filled even if a domain becomes empty, but then meaningless).
   * @return false if a domain becomes empty (the evidence has a null probability).
   */
  template < GUM_Numeric GUM_SCALAR >
  bool pruneDomainsByEvidence(const IBayesNet< GUM_SCALAR >&       bn,
                              const NodeProperty< Idx >*           hardEv,
                              NodeProperty< std::vector< bool > >& allowed);

  /**
   * @brief Checks that the pruning by the hard evidence leaves no domain empty.
   *
   * @param bn the Bayesian network.
   * @param hardEv the hard evidence (node -> observed value).
   * @throw IncompatibleEvidence if the pruning (see pruneDomainsByEvidence) proves that the
   * evidence has a null probability.
   */
  template < GUM_Numeric GUM_SCALAR >
  void checkEvidenceByPruning(const IBayesNet< GUM_SCALAR >& bn, const NodeProperty< Idx >& hardEv);

}   // namespace gum

#include <agrum/BN/inference/tools/evidencePruning_tpl.h>
#endif   // GUM_EVIDENCE_PRUNING_H
