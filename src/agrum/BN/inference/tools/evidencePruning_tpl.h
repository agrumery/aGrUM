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


/**
 * @file
 * @brief Implementation of the pruning of the domains by the hard evidence
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/inference/tools/evidencePruning.h>

namespace gum {

  template < GUM_Numeric GUM_SCALAR >
  bool pruneDomainsByEvidence(const IBayesNet< GUM_SCALAR >&       bn,
                              const NodeProperty< Idx >*           hardEv,
                              NodeProperty< std::vector< bool > >& allowed) {
    allowed.clear();
    for (const auto node: bn.nodes()) {
      const auto domainSize = bn.variable(node).domainSize();
      if (hardEv != nullptr && hardEv->exists(node)) {
        std::vector< bool > observed(domainSize, false);
        observed[(*hardEv)[node]] = true;
        allowed.insert(node, std::move(observed));
      } else {
        allowed.insert(node, std::vector< bool >(domainSize, true));
      }
    }

    // only the CPTs with null probabilities may prune values
    NodeSet               withZeros;
    NodeSet               inQueue;
    std::vector< NodeId > queue;
    for (const auto node: bn.nodes())
      if (bn.cpt(node).min() == GUM_SCALAR(0)) {
        withZeros.insert(node);
        inQueue.insert(node);
        queue.push_back(node);
      }

    while (!queue.empty()) {
      const NodeId f = queue.back();
      queue.pop_back();
      inQueue.erase(f);

      // the supported values: those in a positive entry of the CPT whose values are all allowed
      const auto&                        cpt  = bn.cpt(f);
      const auto&                        vars = cpt.variablesSequence();
      std::vector< NodeId >              ids;
      std::vector< std::vector< bool > > supported;
      for (const auto var: vars) {
        ids.push_back(bn.nodeId(*var));
        supported.emplace_back(var->domainSize(), false);
      }
      Instantiation I(cpt);
      for (I.setFirst(); !I.end(); I.inc()) {
        if (cpt[I] == GUM_SCALAR(0)) continue;
        bool isAllowed = true;
        for (Idx k = 0; k < ids.size() && isAllowed; ++k)
          isAllowed = allowed[ids[k]][I.val(k)];
        if (isAllowed)
          for (Idx k = 0; k < ids.size(); ++k)
            supported[k][I.val(k)] = true;
      }

      // removing the unsupported values
      for (Idx k = 0; k < ids.size(); ++k) {
        auto& domain  = allowed[ids[k]];
        bool  changed = false;
        bool  empty   = true;
        for (Idx val = 0; val < domain.size(); ++val) {
          if (domain[val] && !supported[k][val]) {
            domain[val] = false;
            changed     = true;
          }
          if (domain[val]) empty = false;
        }
        if (empty) return false;

        if (changed) {   // the CPTs containing this node have to be revised
          auto revise = [&](NodeId g) {
            if (withZeros.contains(g) && !inQueue.contains(g)) {
              inQueue.insert(g);
              queue.push_back(g);
            }
          };
          revise(ids[k]);
          for (const auto child: bn.children(ids[k]))
            revise(child);
        }
      }
    }
    return true;
  }

  template < GUM_Numeric GUM_SCALAR >
  void checkEvidenceByPruning(const IBayesNet< GUM_SCALAR >& bn,
                              const NodeProperty< Idx >&     hardEv) {
    if (hardEv.empty()) return;   // without evidence, every domain keeps a value
    NodeProperty< std::vector< bool > > allowed;
    if (!pruneDomainsByEvidence(bn, &hardEv, allowed)) {
      GUM_ERROR(IncompatibleEvidence,
                "The hard evidence has a null probability (incompatible with the model)")
    }
  }

}   // namespace gum
