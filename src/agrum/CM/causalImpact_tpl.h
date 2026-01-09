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

#include <agrum/CM/causalImpact.h>

namespace gum {

  // ---------- helpers ----------

  template < GUM_Numeric GUM_SCALAR >
  typename gum::NameSet
      CausalImpact< GUM_SCALAR >::_idsToNames_(const CausalModel< GUM_SCALAR >& cm,
                                               const NodeSet&                   ids) {
    NameSet out;
    for (auto id: ids)
      out.insert(cm.nameFromId(id));
    return out;
  }

  template < GUM_Numeric GUM_SCALAR >
  typename gum::NodeSet
      CausalImpact< GUM_SCALAR >::_namesToIds_(const CausalModel< GUM_SCALAR >& cm,
                                               const NameSet&                   names) {
    NodeSet out;
    for (const auto& n: names)
      out.insert(cm.idFromName(n));
    return out;
  }

  template < GUM_Numeric GUM_SCALAR >
  bool
      CausalImpact< GUM_SCALAR >::_disjoint_(const NameSet& a, const NameSet& b, const NameSet& c) {
    for (const auto& x: a)
      if (b.contains(x) || c.contains(x)) return false;
    for (const auto& x: b)
      if (c.contains(x)) return false;
    return true;
  }

  // ---------- ctors (initializer-list builds) ----------


  template < GUM_Numeric GUM_SCALAR >
  CausalImpact< GUM_SCALAR >::CausalImpact(const CausalModel< GUM_SCALAR >& cm,
                                           const NameSet&                   on,
                                           const NameSet&                   doing,
                                           const NameSet&                   knowing,
                                           bool                             directDoCalculus) :
      _directDoCalculus_{directDoCalculus},
      _resultFormula_(_buildFromNames_(cm, on, doing, knowing, directDoCalculus)) {}

  template < GUM_Numeric GUM_SCALAR >
  CausalImpact< GUM_SCALAR >::CausalImpact(const CausalModel< GUM_SCALAR >& cm,
                                           const NodeSet&                   on,
                                           const NodeSet&                   doing,
                                           const NodeSet&                   knowing,
                                           bool                             directDoCalculus) :
      _directDoCalculus_{directDoCalculus},
      _resultFormula_(_buildFromIds_(cm, on, doing, knowing, directDoCalculus)) {}

  // ---------- builders ----------

  template < GUM_Numeric GUM_SCALAR >
  CausalFormula< GUM_SCALAR >
      CausalImpact< GUM_SCALAR >::_buildFromNames_(const CausalModel< GUM_SCALAR >& cm,
                                                   const NameSet&                   on,
                                                   const NameSet&                   doing,
                                                   const NameSet&                   knowing,
                                                   bool directDoCalculus) {
    if (!_disjoint_(on, doing, knowing)) {
      GUM_ERROR(InvalidArgument,
                "The 3 parts of the query (on, doing, knowing) must be pairwise disjoint.");
    }

    const NodeSet i_on      = _namesToIds_(cm, on);
    const NodeSet i_doing   = _namesToIds_(cm, doing);
    const NodeSet i_knowing = _namesToIds_(cm, knowing);

    NodeSet cond = i_knowing;

    if (!directDoCalculus) {
      if (Separation::isDSeparated(cm.causalDAG(), i_doing, i_on, cond)) {
        // P(Y | K) in the observational BN
        auto ast = std::make_unique< ASTposteriorProba< GUM_SCALAR > >(cm.observationalBN(),
                                                                       on,
                                                                       knowing);
        return CausalFormula< GUM_SCALAR >(cm,
                                           std::move(ast),
                                           on,
                                           doing,
                                           knowing,
                                           "No causal effect (d-separated).");
      }

      // single-X, single-Y, empty-K: try BACKDOOR then FRONTDOOR
      if (doing.size() == 1 && on.size() == 1 && knowing.empty()) {
        const auto& Xname = *doing.begin();
        const auto& Yname = *on.begin();
        const auto  Xid   = cm.idFromName(Xname);
        const auto  Yid   = cm.idFromName(Yname);

        // --- Backdoor (Z may be empty; validate in G_{\underline X}) ---
        {
          NodeSet Z = cm.backDoor(Xid, Yid);   // can be ∅
          if (Separation::isBackdoorSeparated(cm.causalDAG(), NodeSet{Xid}, NodeSet{Yid}, Z)) {
            DoCalculus< GUM_SCALAR > dc(cm);
            auto                     ast = dc.getBackDoorTree(Xid, Yid, Z);
            return CausalFormula< GUM_SCALAR >(cm,
                                               std::move(ast),
                                               on,
                                               doing,
                                               knowing,
                                               "Identified via backdoor (adjustment).");
          }
        }

        // --- Frontdoor (no trivial FD; require non-empty mediator set) ---
        {
          NodeSet Z = cm.frontDoor(Xid, Yid);
          if (!Z.empty()) {
            DoCalculus< GUM_SCALAR > dc(cm);
            auto                     ast = dc.getFrontDoorTree(Xid, Yid, Z);
            return CausalFormula< GUM_SCALAR >(cm,
                                               std::move(ast),
                                               on,
                                               doing,
                                               knowing,
                                               "Identified via frontdoor (mediator adjustment).");
          }
        }
      }
    }

    // general do-calculus (when available in your DoCalculus)
    try {
      DoCalculus< GUM_SCALAR >                 dc(cm);
      std::unique_ptr< ASTtree< GUM_SCALAR > > ast
          = knowing.empty()
              ? dc.doCalculus(i_on, i_doing)                              // NodeSet overload
              : dc.doCalculusWithObservation(i_on, i_doing, i_knowing);   // NodeSet overload

      return CausalFormula< GUM_SCALAR >(cm,
                                         std::move(ast),
                                         on,
                                         doing,
                                         knowing,
                                         "Identified via do-calculus (ID/IDC).");
    } catch (const NotImplementedYet&) {
      // Not identifiable with current methods: return a “null-AST” formula with a message.
      return CausalFormula< GUM_SCALAR >(
          cm,
          std::unique_ptr< ASTtree< GUM_SCALAR > >{},
          on,
          doing,
          knowing,
          "Effect not identifiable with available methods (do-calculus not yet implemented).");
    } catch (const HedgeException& e) {
      return CausalFormula< GUM_SCALAR >(cm,
                                         std::unique_ptr< ASTtree< GUM_SCALAR > >{},
                                         on,
                                         doing,
                                         knowing,
                                         std::string("Effect not identifiable: ") + e.what());
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  CausalFormula< GUM_SCALAR >
      CausalImpact< GUM_SCALAR >::_buildFromIds_(const CausalModel< GUM_SCALAR >& cm,
                                                 const NodeSet&                   on,
                                                 const NodeSet&                   doing,
                                                 const NodeSet&                   knowing,
                                                 bool directDoCalculus) {
    // Reuse the names path to share logic/explanations
    const auto onN      = _idsToNames_(cm, on);
    const auto doingN   = _idsToNames_(cm, doing);
    const auto knowingN = _idsToNames_(cm, knowing);
    return _buildFromNames_(cm, onN, doingN, knowingN, directDoCalculus);
  }

  template < GUM_Numeric GUM_SCALAR >
  inline Instantiation
      makeInstantiationFromValues(const Tensor< GUM_SCALAR >&                  tensor,
                                  const HashTable< std::string, std::string >& values) {
    Instantiation I;   // start empty (partial instantiation)

    for (const auto& kv: values) {
      try {
        const auto&    var = tensor.variable(kv.first);   // skip if not in tensor
        const gum::Idx idx = var.index(kv.second);        // skip if label unknown
        I.add(var);                                       // add this dim only
        I.chgVal(var, idx);                               // fix its value
      } catch (const gum::NotFound&) {
        // variable or label not found: ignore (non-strict)
        continue;
      } catch (...) {
        // any other issue: ignore
        continue;
      }
    }
    return I;
  }

  template < GUM_Numeric GUM_SCALAR >
  std::tuple< CausalImpact< GUM_SCALAR >, Tensor< GUM_SCALAR >, std::string >
      causalImpact(const CausalModel< GUM_SCALAR >&                    cm,
                   const NameSet&                                      on,
                   const NameSet&                                      doing,
                   const NameSet&                                      knowing,
                   const HashTable< VariableName, VariableValueName >& values) {
    // Delegate to the dedicated class.
    CausalImpact< GUM_SCALAR > ci(cm, on, doing, knowing);

    // Numeric value: empty tensor if not identifiable
    Tensor< GUM_SCALAR > numeric;
    if (ci.isIdentified()) numeric = ci.eval();


    // If specific values were provided, strictly slice the tensor to those values
    if (!values.empty() && numeric.nbrDim() > 0) {
      const auto I
          = makeInstantiationFromValues(numeric, values);   // throws if any key/label is invalid
      numeric = numeric.extract(I);   // keep only remaining free dimensions (if any)
    }

    // Always return the explanation the class computed
    return std::make_tuple(std::move(ci), std::move(numeric), ci.explanation());
  }


}   // namespace gum
