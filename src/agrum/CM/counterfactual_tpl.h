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

#include <utility>

#include <agrum/CM/counterfactual.h>

namespace gum {

  // ============================== CONSTRUCTORS ===============================

  template < GUM_Numeric GUM_SCALAR >
  Counterfactual< GUM_SCALAR >::Counterfactual(const CausalModel< GUM_SCALAR >&     cm,
                                               const Set< std::string >&            on,
                                               const Set< std::string >&            whatif,
                                               const HashTable< VarName, ValName >& profile,
                                               const HashTable< VarName, ValName >& values) :
      _cm(cm), _on(on), _whatif(whatif), _profile(profile), _values(values),
      _twin(cm)   // copy-construct (clone semantics)
      ,
      _adaptedValue(), _ci(nullptr) {
    _twin = counterFactualModel(_cm, _profile, _whatif);
    run();
  }

  template < GUM_Numeric GUM_SCALAR >
  Counterfactual< GUM_SCALAR >::Counterfactual(const CausalModel< GUM_SCALAR >& cm,
                                               const NodeSet&                   onIds,
                                               const NodeSet&                   whatifIds,
                                               const HashTable< NodeId, Idx >&  profileIds,
                                               const HashTable< NodeId, Idx >&  valuesIds) :
      Counterfactual< GUM_SCALAR >(cm,
                                   _idsToNames_(cm, onIds),
                                   _idsToNames_(cm, whatifIds),
                                   _idAssignToNameAssign_(cm, profileIds),
                                   _idAssignToNameAssign_(cm, valuesIds)) {}

  // ============================== TWIN BUILDERS ==============================

  template < GUM_Numeric GUM_SCALAR >
  CausalModel< GUM_SCALAR > Counterfactual< GUM_SCALAR >::counterFactualModel(
      const CausalModel< GUM_SCALAR >&     cm,
      const HashTable< VarName, ValName >& profile,
      const Set< std::string >&            whatif) {
    const auto&               origBN = cm.observationalBN();
    CausalModel< GUM_SCALAR > twincm(origBN);   // copy (serves as clone)

    // 1) idiosyncratic = parentless \ (whatif ∪ latent)
    NodeSet whatifIds;
    for (const auto& w: whatif) {
      whatifIds.insert(cm.idFromName(w));
    }
    NodeSet idiosyncratic;
    for (auto nid: origBN.nodes()) {
      if (origBN.parents(nid).size() == 0) { idiosyncratic.insert(nid); }
    }
    for (auto id: whatifIds) {
      idiosyncratic.erase(id);
    }
    for (auto id: cm.latentVariablesIds()) {
      idiosyncratic.erase(id);
    }

    // 2) posterior(idiosyncratic | profile) in ORIGINAL BN
    gum::LazyPropagation< GUM_SCALAR > ie(&origBN);
    if (!profile.empty()) {
      for (const auto& kv: profile) {
        const auto&  varName = kv.first;
        const auto&  valName = kv.second;
        const NodeId nid     = cm.idFromName(varName);
        const auto&  var     = origBN.variable(nid);
        const Idx    valId   = var.index(valName);
        ie.addEvidence(nid, valId);
      }
    }
    ie.makeInference();
    // 3) Replace priors in the twin with posteriors from the original
    auto& twinBN = twincm.observationalBN();
    for (auto f: idiosyncratic) {
      const auto&                name   = origBN.variable(f).name();
      auto                       twinId = twinBN.idFromName(name);
      const Tensor< GUM_SCALAR > post   = ie.posterior(f);
      twinBN.cpt(twinId).fillWith(post);
    }
    return twincm;
  }

  template < GUM_Numeric GUM_SCALAR >
  CausalModel< GUM_SCALAR >
      Counterfactual< GUM_SCALAR >::counterFactualModel(const CausalModel< GUM_SCALAR >& cm,
                                                        const HashTable< NodeId, Idx >&  profileIds,
                                                        const NodeSet& whatifIds) {
    const auto& origBN = cm.observationalBN();
    // 1) idiosyncratic = parentless \ (whatif ∪ latent)
    NodeSet idiosyncratic;
    for (auto nid: origBN.nodes()) {
      if (origBN.parents(nid).size() == 0) { idiosyncratic.insert(nid); }
    }
    for (auto id: whatifIds) {
      idiosyncratic.erase(id);
    }
    for (auto id: cm.latentVariablesIds()) {
      idiosyncratic.erase(id);
    }

    // 2) posterior(idiosyncratic | profile) in ORIGINAL BN
    CausalModel< GUM_SCALAR > twincm(origBN);
    {
      gum::LazyPropagation< GUM_SCALAR > ie(&origBN);
      if (!profileIds.empty()) {
        for (const auto& kv: profileIds) {
          const NodeId nid   = kv.first;
          const Idx    valId = kv.second;
          ie.addEvidence(nid, valId);
        }
      }
      ie.makeInference();
      auto& twinBN = twincm.observationalBN();
      for (auto f: idiosyncratic) {
        const auto&                name   = origBN.variable(f).name();
        const NodeId               twinId = twinBN.idFromName(name);
        const Tensor< GUM_SCALAR > post   = ie.posterior(f);
        twinBN.cpt(twinId).fillWith(post);
      }
    }
    return twincm;
  }

  // ================================ EXECUTION ================================

  template < GUM_Numeric GUM_SCALAR >
  void Counterfactual< GUM_SCALAR >::run() {
    // Build symbolic effect on the twin
    _ci = std::make_unique< CausalImpact< GUM_SCALAR > >(_twin, _on, _whatif);

    // Numeric evaluation on the twin
    Tensor< GUM_SCALAR > adj = _ci->eval();

    // Slice by specified intervention values (partial instantiation, safe if some names are absent)
    if (!_values.empty() && adj.nbrDim() > 0) {
      auto I = makeInstantiationFromValues(adj, _values);   // keeps only vars present in `adj`
      if (I.nbrDim() > 0) adj = adj.extract(I);
    }

    // Adapt tensor to ORIGINAL model variables
    _adaptedValue = _adaptToOriginalVariables_(adj, _cm);
  }

  // =============================== ADAPTATION ================================

  template < GUM_Numeric GUM_SCALAR >
  Tensor< GUM_SCALAR > Counterfactual< GUM_SCALAR >::_adaptToOriginalVariables_(
      const Tensor< GUM_SCALAR >&      adj,
      const CausalModel< GUM_SCALAR >& cm) {
    Tensor< GUM_SCALAR > res;
    const auto&          bn = cm.observationalBN();

    // keep adj variable order
    for (const auto& v: adj.variablesSequence()) {
      res.add(bn.variableFromName(v->name()));
    }
    res.fillWith(adj);
    return res;
  }

  // ============================ RESULT ACCESSOR ==============================

  template < GUM_Numeric GUM_SCALAR >
  const CausalFormula< GUM_SCALAR >& Counterfactual< GUM_SCALAR >::_ciResult() const {
    return _ci->getResult();   // public accessor in CausalImpact
  }

  // ================================= PRINT ==================================

  template < GUM_Numeric GUM_SCALAR >
  std::string Counterfactual< GUM_SCALAR >::toString() const {
    std::stringstream os;
    os << "[Counterfactual]\n";

    os << " on = {";
    {
      bool first = true;
      for (const auto& s: _on) {
        if (!first) os << ", ";
        os << s;
        first = false;
      }
    }
    os << "}\n whatif = {";
    {
      bool first = true;
      for (const auto& s: _whatif) {
        if (!first) os << ", ";
        os << s;
        first = false;
      }
    }
    os << "}\n";

    if (!_profile.empty()) {
      os << " profile: ";
      bool first = true;
      for (const auto& [k, v]: _profile) {
        if (!first) os << ", ";
        os << k << "=" << v;
        first = false;
      }
      os << "\n";
    }

    if (!_values.empty()) {
      os << " values: ";
      bool first = true;
      for (const auto& [k, v]: _values) {
        if (!first) os << ", ";
        os << k << "=" << v;
        first = false;
      }
      os << "\n";
    }

    os << " result (symbolic): ";
    if (_ci && _ci->isIdentified()) os << _ciResult().toString() << "\n";
    else os << "<unidentified>\n";

    if (_adaptedValue.nbrDim() > 0) {
      os << " value (adapted to original variables):\n";
      os << _adaptedValue << "\n";
    }

    return os.str();
  }

  // ============================== HELPERS (IDs) ==============================

  template < GUM_Numeric GUM_SCALAR >
  Set< std::string > Counterfactual< GUM_SCALAR >::_idsToNames_(const CausalModel< GUM_SCALAR >& cm,
                                                                const NodeSet& ids) {
    Set< std::string > out;
    const auto&        bn = cm.observationalBN();
    for (auto nid: ids) {
      const auto& var = bn.variable(nid);   // reference by id
      out.insert(var.name());
    }
    return out;
  }

  template < GUM_Numeric GUM_SCALAR >
  HashTable< typename Counterfactual< GUM_SCALAR >::VarName,
             typename Counterfactual< GUM_SCALAR >::ValName >
      Counterfactual< GUM_SCALAR >::_idAssignToNameAssign_(
          const CausalModel< GUM_SCALAR >& cm,
          const HashTable< NodeId, Idx >&  idAssign) {
    HashTable< VarName, ValName > out;
    const auto&                   bn = cm.observationalBN();
    for (const auto& kv: idAssign) {
      const NodeId nid   = kv.first;
      const Idx    valId = kv.second;
      const auto&  var   = bn.variable(nid);   // reference by id
      out.insert(var.name(), var.label(valId));
    }
    return out;
  }

  // ============================================================================
  // Standalone helpers (Python-parity)
  // ============================================================================

  template < GUM_Numeric GUM_SCALAR >
  Tensor< GUM_SCALAR > counterfactual(const CausalModel< GUM_SCALAR >&             cm,
                                      const Set< std::string >&                    on,
                                      const Set< std::string >&                    whatif,
                                      const HashTable< std::string, std::string >& profile,
                                      const HashTable< std::string, std::string >& values) {
    // Use the class implementation and return the adapted tensor.
    Counterfactual< GUM_SCALAR > cf(cm, on, whatif, profile, values);
    return cf.value();
  }

  template < GUM_Numeric GUM_SCALAR >
  CausalModel< GUM_SCALAR >
      counterfactualModel(const CausalModel< GUM_SCALAR >&             cm,
                          const HashTable< std::string, std::string >& profile,
                          const Set< std::string >&                    whatif) {
    const auto& origBN = cm.observationalBN();

    // Clone model (copy semantics)
    CausalModel< GUM_SCALAR > twincm(origBN);

    // what-if ids
    NodeSet whatifIds;
    for (const auto& w: whatif)
      whatifIds.insert(cm.idFromName(w));

    // idiosyncratic = parentless \ (whatif ∪ latent)
    NodeSet idiosyncratic;
    for (auto nid: origBN.nodes())
      if (origBN.parents(nid).size() == 0) idiosyncratic.insert(nid);
    for (auto id: whatifIds)
      idiosyncratic.erase(id);
    for (auto id: cm.latentVariablesIds())
      idiosyncratic.erase(id);

    // Posteriors in the original BN
    LazyPropagation< GUM_SCALAR > ie(&origBN);
    if (!profile.empty()) {
      for (const auto& kv: profile) {
        const NodeId nid   = cm.idFromName(kv.first);
        const auto&  var   = origBN.variable(nid);
        const Idx    valId = var.index(kv.second);
        ie.addEvidence(nid, valId);
      }
    }
    ie.makeInference();

    // Update priors in the twin with posteriors
    auto& twinBN = twincm.observationalBN();
    for (auto f: idiosyncratic) {
      const auto&                name   = origBN.variable(f).name();
      const NodeId               twinId = twinBN.idFromName(name);
      const Tensor< GUM_SCALAR > post   = ie.posterior(f);
      twinBN.cpt(twinId).fillWith(post);
    }

    return twincm;
  }

}   // namespace gum
