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

#include <memory>
#include <utility>

#include <agrum/base/core/bijection.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/CM/counterfactual.h>

namespace gum {

  // ============================== CONSTRUCTORS ===============================

  template < GUM_Numeric GUM_ELEMENT >
  Counterfactual< GUM_ELEMENT >::Counterfactual(const CausalModel< GUM_ELEMENT >&    cm,
                                                const Set< std::string >&            on,
                                                const Set< std::string >&            whatif,
                                                const HashTable< VarName, ValName >& profile,
                                                const HashTable< VarName, ValName >& values) :
      _cm(cm), _on(on), _whatif(whatif), _profile(profile), _values(values),
      _twin(counterFactualModel(cm, whatif)), _adaptedValue(), _ci(nullptr) {
    for (const auto& name: on) {
      if (whatif.contains(name)) {
        GUM_ERROR(InvalidArgument,
                  "The variable " << name << " cannot be both in 'on' and in 'whatif'.");
      }
    }
    run();
  }

  template < GUM_Numeric GUM_ELEMENT >
  Counterfactual< GUM_ELEMENT >::Counterfactual(const CausalModel< GUM_ELEMENT >& cm,
                                                const NodeSet&                    onIds,
                                                const NodeSet&                    whatifIds,
                                                const HashTable< NodeId, Idx >&   profileIds,
                                                const HashTable< NodeId, Idx >&   valuesIds) :
      Counterfactual< GUM_ELEMENT >(cm,
                                    _idsToNames_(cm, onIds),
                                    _idsToNames_(cm, whatifIds),
                                    _idAssignToNameAssign_(cm, profileIds),
                                    _idAssignToNameAssign_(cm, valuesIds)) {}

  // ============================== TWIN NETWORK ===============================

  template < GUM_Numeric GUM_ELEMENT >
  std::string Counterfactual< GUM_ELEMENT >::copyName(const std::string& name) {
    return name + "*";
  }

  template < GUM_Numeric GUM_ELEMENT >
  CausalModel< GUM_ELEMENT >
      Counterfactual< GUM_ELEMENT >::counterFactualModel(const CausalModel< GUM_ELEMENT >& cm,
                                                         const Set< std::string >&         whatif) {
    NodeSet whatifIds;
    for (const auto& w: whatif) {
      whatifIds.insert(cm.idFromName(w));
    }
    return counterFactualModel(cm, whatifIds);
  }

  template < GUM_Numeric GUM_ELEMENT >
  CausalModel< GUM_ELEMENT >
      Counterfactual< GUM_ELEMENT >::counterFactualModel(const CausalModel< GUM_ELEMENT >& cm,
                                                         const NodeSet& whatifIds) {
    // a latent variable has no distribution: the twin network cannot be built
    if (!cm.latentVariablesIds().empty()) {
      GUM_ERROR(InvalidArgument,
                "A counterfactual requires a causal model without latent variables (their "
                "distributions, needed for the abduction, are unknown).");
    }

    const auto& bn = cm.observationalBN();

    // the counterfactual world: copies of the whatif variables and of their descendants, the other
    // variables (in particular the parentless ones, the exogenous causes) being shared by both
    // worlds
    NodeSet copied = whatifIds;
    for (const auto x: whatifIds) {
      for (const auto d: bn.descendants(x)) {
        copied.insert(d);
      }
    }

    BayesNet< GUM_ELEMENT >     twin(bn);   // the real world
    Bijection< NodeId, NodeId > copyOf;     // original -> copy
    for (const auto v: bn.topologicalOrder()) {
      if (!copied.contains(v)) continue;
      const auto name = copyName(bn.variable(v).name());
      if (bn.exists(name)) {
        GUM_ERROR(InvalidArgument,
                  "The name " << name
                              << " of a counterfactual copy is already used in the causal model.");
      }
      std::unique_ptr< DiscreteVariable > var(bn.variable(v).clone());
      var->setName(name);
      copyOf.insert(v, twin.add(*var));
    }

    for (const auto v: copied) {
      const NodeId c = copyOf.second(v);
      if (whatifIds.contains(v)) {
        // the intervened copy has no cause: a uniform prior, its value is given as evidence
        twin.cpt(c).fillWith(GUM_ELEMENT(1)).normalize();
        continue;
      }
      for (const auto p: bn.parents(v)) {
        twin.addArc(copyOf.existsFirst(p) ? copyOf.second(p) : p, c);
      }
      // the copy has the CPT of its original (variables matched by name)
      const auto&                cpt = twin.cpt(c);
      std::vector< std::string > mapSrc;
      for (const auto var: cpt.variablesSequence()) {
        const auto&  name = var->name();
        const NodeId n    = twin.idFromName(name);
        mapSrc.push_back(copyOf.existsSecond(n) ? bn.variable(copyOf.first(n)).name() : name);
      }
      cpt.fillWith(bn.cpt(v), mapSrc);
    }

    return CausalModel< GUM_ELEMENT >(twin);
  }

  template < GUM_Numeric GUM_ELEMENT >
  CausalModel< GUM_ELEMENT > Counterfactual< GUM_ELEMENT >::counterFactualModel(
      const CausalModel< GUM_ELEMENT >& cm,
      const HashTable< VarName, ValName >& /*profile*/,
      const Set< std::string >& whatif) {
    return counterFactualModel(cm, whatif);
  }

  template < GUM_Numeric GUM_ELEMENT >
  CausalModel< GUM_ELEMENT > Counterfactual< GUM_ELEMENT >::counterFactualModel(
      const CausalModel< GUM_ELEMENT >& cm,
      const HashTable< NodeId, Idx >& /*profile*/,
      const NodeSet& whatifIds) {
    return counterFactualModel(cm, whatifIds);
  }

  // ================================ EXECUTION ================================

  template < GUM_Numeric GUM_ELEMENT >
  void Counterfactual< GUM_ELEMENT >::run() {
    const auto& twin = _twin.observationalBN();

    // the name of a variable in the counterfactual world
    const auto inCounterfactualWorld = [&twin](const std::string& name) {
      return twin.exists(copyName(name)) ? copyName(name) : name;
    };

    // the profile is observed in the real world
    LazyPropagation< GUM_ELEMENT > ie(&twin);
    for (const auto& [name, label]: _profile) {
      ie.addEvidence(name, label);
    }
    NodeSet targets;
    NodeSet whatifCopies;
    for (const auto& name: _on)
      targets.insert(twin.idFromName(inCounterfactualWorld(name)));
    for (const auto& name: _whatif) {
      const NodeId c = twin.idFromName(copyName(name));
      targets.insert(c);
      whatifCopies.insert(c);
    }
    ie.addJointTarget(targets);
    ie.makeInference();
    const auto joint = ie.jointPosterior(targets);

    // the copies of whatif have no parent and no observed descendant: they are independent of the
    // profile and P(on* | whatif*, profile) = P(on*, whatif* | profile) / P(whatif*)
    VariableSet copies;
    for (const auto c: whatifCopies)
      copies.insert(&twin.variable(c));
    Tensor< GUM_ELEMENT > adj = joint / joint.sumIn(copies);

    // back to the variables of the original model
    adj = _adaptToOriginalVariables_(adj, _cm);

    // slice by the counterfactual values (partial instantiation, safe if some names are absent)
    if (!_values.empty() && adj.nbrDim() > 0) {
      auto I = makeInstantiationFromValues(adj, _values);   // keeps only vars present in `adj`
      if (I.nbrDim() > 0) adj = adj.extract(I);
    }
    _adaptedValue = std::move(adj);
  }

  // =============================== ADAPTATION ================================

  template < GUM_Numeric GUM_ELEMENT >
  Tensor< GUM_ELEMENT > Counterfactual< GUM_ELEMENT >::_adaptToOriginalVariables_(
      const Tensor< GUM_ELEMENT >&      adj,
      const CausalModel< GUM_ELEMENT >& cm) {
    Tensor< GUM_ELEMENT > res;
    const auto&           bn = cm.observationalBN();

    // keep adj variable order, the copy V* being replaced by V
    std::vector< std::string > mapSrc;
    for (const auto v: adj.variablesSequence()) {
      std::string name = v->name();
      if (!bn.exists(name)) name.pop_back();   // V* -> V
      res.add(bn.variableFromName(name));
      mapSrc.push_back(v->name());
    }
    res.fillWith(adj, mapSrc);
    return res;
  }

  // ================================= ACCESSORS ==================================

  template < GUM_Numeric GUM_ELEMENT >
  const CausalModel< GUM_ELEMENT >& Counterfactual< GUM_ELEMENT >::originalModel() const {
    return _cm;
  }

  template < GUM_Numeric GUM_ELEMENT >
  const CausalModel< GUM_ELEMENT >& Counterfactual< GUM_ELEMENT >::twinModel() const {
    return _twin;
  }

  template < GUM_Numeric GUM_ELEMENT >
  const CausalImpact< GUM_ELEMENT >& Counterfactual< GUM_ELEMENT >::impact() const {
    if (_ci == nullptr) {
      // P(on* | do(whatif*), profile) in the twin network
      const auto&        twin = _twin.observationalBN();
      Set< std::string > onCopies, whatifCopies, knowing;
      for (const auto& name: _on)
        onCopies.insert(twin.exists(copyName(name)) ? copyName(name) : name);
      for (const auto& name: _whatif)
        whatifCopies.insert(copyName(name));
      for (const auto& [name, label]: _profile)
        knowing.insert(name);
      _ci = std::make_unique< CausalImpact< GUM_ELEMENT > >(_twin, onCopies, whatifCopies, knowing);
    }
    return *_ci;
  }

  template < GUM_Numeric GUM_ELEMENT >
  const Tensor< GUM_ELEMENT >& Counterfactual< GUM_ELEMENT >::value() const {
    return _adaptedValue;
  }

  template < GUM_Numeric GUM_ELEMENT >
  const Set< std::string >& Counterfactual< GUM_ELEMENT >::on() const {
    return _on;
  }

  template < GUM_Numeric GUM_ELEMENT >
  const Set< std::string >& Counterfactual< GUM_ELEMENT >::whatif() const {
    return _whatif;
  }

  template < GUM_Numeric GUM_ELEMENT >
  const HashTable< typename Counterfactual< GUM_ELEMENT >::VarName,
                   typename Counterfactual< GUM_ELEMENT >::ValName >&
      Counterfactual< GUM_ELEMENT >::profile() const {
    return _profile;
  }

  template < GUM_Numeric GUM_ELEMENT >
  const HashTable< typename Counterfactual< GUM_ELEMENT >::VarName,
                   typename Counterfactual< GUM_ELEMENT >::ValName >&
      Counterfactual< GUM_ELEMENT >::values() const {
    return _values;
  }

  // ================================= PRINT ==================================

  template < GUM_Numeric GUM_ELEMENT >
  std::string Counterfactual< GUM_ELEMENT >::toString() const {
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

    if (_adaptedValue.nbrDim() > 0) {
      os << " value (adapted to original variables):\n";
      os << _adaptedValue << "\n";
    }

    return os.str();
  }

  // ================================= HELPERS ==================================

  template < GUM_Numeric GUM_ELEMENT >
  Set< std::string >
      Counterfactual< GUM_ELEMENT >::_idsToNames_(const CausalModel< GUM_ELEMENT >& cm,
                                                  const NodeSet&                    ids) {
    Set< std::string > out;
    const auto&        bn = cm.observationalBN();
    for (auto nid: ids) {
      const auto& var = bn.variable(nid);   // reference by id
      out.insert(var.name());
    }
    return out;
  }

  template < GUM_Numeric GUM_ELEMENT >
  HashTable< typename Counterfactual< GUM_ELEMENT >::VarName,
             typename Counterfactual< GUM_ELEMENT >::ValName >
      Counterfactual< GUM_ELEMENT >::_idAssignToNameAssign_(
          const CausalModel< GUM_ELEMENT >& cm,
          const HashTable< NodeId, Idx >&   idAssign) {
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

  template < GUM_Numeric GUM_ELEMENT >
  Tensor< GUM_ELEMENT > counterfactual(const CausalModel< GUM_ELEMENT >&            cm,
                                       const Set< std::string >&                    on,
                                       const Set< std::string >&                    whatif,
                                       const HashTable< std::string, std::string >& profile,
                                       const HashTable< std::string, std::string >& values) {
    // Use the class implementation and return the adapted tensor.
    Counterfactual< GUM_ELEMENT > cf(cm, on, whatif, profile, values);
    return cf.value();
  }

  template < GUM_Numeric GUM_ELEMENT >
  CausalModel< GUM_ELEMENT > counterfactualModel(const CausalModel< GUM_ELEMENT >& cm,
                                                 const Set< std::string >&         whatif) {
    return Counterfactual< GUM_ELEMENT >::counterFactualModel(cm, whatif);
  }

  template < GUM_Numeric GUM_ELEMENT >
  CausalModel< GUM_ELEMENT >
      counterfactualModel(const CausalModel< GUM_ELEMENT >& cm,
                          const HashTable< std::string, std::string >& /*profile*/,
                          const Set< std::string >& whatif) {
    return Counterfactual< GUM_ELEMENT >::counterFactualModel(cm, whatif);
  }

}   // namespace gum
