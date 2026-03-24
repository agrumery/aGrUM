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
 * @brief Do-calculus utilities and AST builders bound to a single CausalModel.
 *
 * This class mirrors the layout of pyAgrum's `_doCalculus.py`: it provides
 * helpers to construct adjustment formulas (backdoor/frontdoor) as ASTs and
 * the public entry points for general identification à la Shpitser–Pearl.
 */
#pragma once

#include <algorithm>
#include <queue>
#include <sstream>
#include <utility>

#include <agrum/CM/tools/causalFormula.h>
#include <agrum/CM/tools/doCalculus.h>
#include <agrum/CM/tools/separation.h>

#include <unordered_map>
#include <unordered_set>

namespace gum {


  namespace {
    // format a vector of strings as "{a, b, c}"
    inline std::string _fmtBraceList_(std::vector< std::string > v) {
      std::sort(v.begin(), v.end());
      std::ostringstream os;
      os << "{";
      for (size_t i = 0; i < v.size(); ++i) {
        if (i) os << ", ";
        os << v[i];
      }
      os << "}";
      return os.str();
    }

  }   // anonymous namespace

  /* ========================================================================== */
  /*                              Backdoor / Frontdoor                          */
  /* ========================================================================== */

  template < GUM_Numeric GUM_SCALAR >
  typename DoCalculus< GUM_SCALAR >::FormulaPtr
      DoCalculus< GUM_SCALAR >::getBackDoorTree(NodeId         cause,
                                                NodeId         effect,
                                                const NodeSet& zset) const {
    // Preconditions: cause/effect must be observed (non-latent), Z must not contain latents.
    const NodeSet lat = _cm.latentVariablesIds();
    if (lat.contains(cause) || lat.contains(effect)) {
      GUM_ERROR(InvalidArgument,
                "getBackDoorTree: 'cause' and 'effect' must be observed (non-latent).");
    }
    for (auto z: zset) {
      if (lat.contains(z)) {
        GUM_ERROR(InvalidArgument, "getBackDoorTree: zset contains a latent variable.");
      }
    }

    // ids -> names
    const auto&       dag   = _cm.causalDAG();
    auto              mapO  = _cm.id2name(/*includeLatentVariables=*/false);
    const std::string xName = _cm.nameFromId(cause);
    const std::string yName = _cm.nameFromId(effect);

    Set< std::string >         zNames;
    std::vector< std::string > zOrder;
    zOrder.reserve(zset.size());
    for (auto z: zset) {
      const std::string zn = _cm.nameFromId(z);
      zNames.insert(zn);
      zOrder.push_back(zn);
    }
    std::sort(zOrder.begin(), zOrder.end());

    // P(y | x, z)
    Set< std::string > lhsY;
    lhsY.insert(yName);
    Set< std::string > knw;
    knw.insert(xName);
    for (const auto& zn: zNames)
      knw.insert(zn);
    auto Pyxz = std::make_unique< ASTposteriorProba< GUM_SCALAR > >(dag, mapO, lhsY, knw);

    if (zNames.empty()) {
      // returns P(y | x)
      return Pyxz;
    }

    // P(z)
    auto Pz = std::make_unique< ASTjointProba< GUM_SCALAR > >(zNames);

    // P(y|x,z) * P(z), then sum over Z
    std::vector< std::unique_ptr< ASTtree< GUM_SCALAR > > > terms;
    terms.emplace_back(std::move(Pyxz));
    terms.emplace_back(std::move(Pz));
    auto prod = productOfTrees< GUM_SCALAR >(std::move(terms));

    return std::make_unique< ASTsum< GUM_SCALAR > >(zOrder, std::move(prod));
  }

  template < GUM_Numeric GUM_SCALAR >
  typename DoCalculus< GUM_SCALAR >::FormulaPtr
      DoCalculus< GUM_SCALAR >::getBackDoorTree(std::string_view causeName,
                                                std::string_view effectName,
                                                const NameList&  zNames) const {
    const NodeId x = _cm.idFromName(causeName);
    const NodeId y = _cm.idFromName(effectName);

    NodeSet Z;
    for (const auto& zn: zNames)
      Z.insert(_cm.idFromName(zn));

    return getBackDoorTree(x, y, Z);
  }

  template < GUM_Numeric GUM_SCALAR >
  typename DoCalculus< GUM_SCALAR >::FormulaPtr
      DoCalculus< GUM_SCALAR >::getFrontDoorTree(NodeId         cause,
                                                 NodeId         effect,
                                                 const NodeSet& zset) const {
    // --- Preconditions ---
    const NodeSet lat = _cm.latentVariablesIds();
    if (lat.contains(cause) || lat.contains(effect)) {
      GUM_ERROR(InvalidArgument,
                "getFrontDoorTree: 'cause' and 'effect' must be observed (non-latent).");
    }
    if (zset.empty()) {
      GUM_ERROR(InvalidArgument, "getFrontDoorTree: zset must be non-empty for frontdoor.");
    }
    if (zset.contains(cause) || zset.contains(effect)) {
      GUM_ERROR(InvalidArgument, "getFrontDoorTree: zset must be disjoint from {cause,effect}.");
    }
    for (auto z: zset) {
      if (lat.contains(z)) {
        GUM_ERROR(InvalidArgument, "getFrontDoorTree: zset contains a latent variable.");
      }
    }

    // --- ids → names ---
    const std::string xName = _cm.nameFromId(cause);
    const std::string yName = _cm.nameFromId(effect);

    Set< std::string >         zNames;
    std::vector< std::string > zOrder;
    zOrder.reserve(zset.size());
    for (auto z: zset) {
      const auto zn = _cm.nameFromId(z);
      zNames.insert(zn);
      zOrder.push_back(zn);
    }
    std::sort(zOrder.begin(), zOrder.end());   // deterministic

    // --- Build P(Z | X) using DAG ctor (observed-only mapping inferred from model) ---
    const auto& dag  = _cm.causalDAG();
    auto        mapO = _cm.id2name(/*includeLatentVariables=*/false);

    Set< std::string > condX;
    condX.insert(xName);
    auto Pz_given_x = std::make_unique< ASTposteriorProba< GUM_SCALAR > >(dag, mapO, zNames, condX);

    // --- Build sum_x P(Y | X, Z) * P(X) ---
    Set< std::string > ySet;
    ySet.insert(yName);

    Set< std::string > condXZ = zNames;   // copy
    condXZ.insert(xName);
    auto Py_given_xz = std::make_unique< ASTposteriorProba< GUM_SCALAR > >(dag, mapO, ySet, condXZ);

    Set< std::string > xSet;
    xSet.insert(xName);
    auto Px = std::make_unique< ASTjointProba< GUM_SCALAR > >(xSet);

    auto inner_prod
        = std::make_unique< ASTmult< GUM_SCALAR > >(std::move(Py_given_xz), std::move(Px));
    auto inner_sum = std::make_unique< ASTsum< GUM_SCALAR > >(std::vector< std::string >{xName},
                                                              std::move(inner_prod));

    // --- P(Z|X) * (sum_x ...) then sum over Z ---
    auto outer_prod
        = std::make_unique< ASTmult< GUM_SCALAR > >(std::move(Pz_given_x), std::move(inner_sum));
    return std::make_unique< ASTsum< GUM_SCALAR > >(zOrder, std::move(outer_prod));
  }

  template < GUM_Numeric GUM_SCALAR >
  typename DoCalculus< GUM_SCALAR >::FormulaPtr
      DoCalculus< GUM_SCALAR >::getFrontDoorTree(std::string_view causeName,
                                                 std::string_view effectName,
                                                 const NameList&  zNames) const {
    const NodeId x = _cm.idFromName(causeName);
    const NodeId y = _cm.idFromName(effectName);

    NodeSet Z;
    for (const auto& zn: zNames)
      Z.insert(_cm.idFromName(zn));

    return getFrontDoorTree(x, y, Z);
  }

  /* ========================================================================== */
  /*                               Helper utilities                             */
  /* ========================================================================== */

  template < GUM_Numeric GUM_SCALAR >
  std::string DoCalculus< GUM_SCALAR >::_hedgeExceptionMsg(const CausalModel< GUM_SCALAR >& cm,
                                                           const NodeSet&                   Y,
                                                           const NodeSet&                   X,
                                                           const NodeSet&                   V,
                                                           const NodeSet&                   S) {
    // turn NodeSets into sorted name lists
    auto namesOf = [&](const NodeSet& Ns) {
      std::vector< std::string > out;
      out.reserve(Ns.size());
      for (auto id: Ns)
        out.push_back(cm.nameFromId(id));
      std::sort(out.begin(), out.end());
      return out;
    };

    const auto yN = namesOf(Y);
    const auto xN = namesOf(X);
    const auto vN = namesOf(V);
    const auto sN = namesOf(S);

    std::ostringstream msg;
    msg << "Not identifiable: hedge detected. "
        << "Target P" << _fmtBraceList_(yN) << " | do" << _fmtBraceList_(xN)
        << ". Witness: V = " << _fmtBraceList_(vN)
        << " is a single c-component and S = " << _fmtBraceList_(sN)
        << " is a c-component in both G and G\\X (forms a hedge). "
        << "Interpretation: unblocked confounding remains under intervention; "
           "the effect cannot be expressed from the observational distribution "
           "using current graph structure.";
    return msg.str();
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< NodeSet >
      DoCalculus< GUM_SCALAR >::_cDecomposition_(const CausalModel< GUM_SCALAR >& cm) const {
    // c-components: undirected connectivity among observed children of the same latent.
    const NodeSet lat = cm.latentVariablesIds();
    const NodeSet V   = cm.observationalBN().dag().nodes().asNodeSet();

    // adjacency among observed nodes, via latent co-children
    std::unordered_map< NodeId, std::vector< NodeId > > adj;
    for (auto v: V)
      adj[v];   // ensure key exists

    for (auto h: lat) {
      const auto& ch = cm.children(h);
      // connect all observed pairs of children with an undirected edge
      for (auto it1 = ch.begin(); it1 != ch.end(); ++it1) {
        if (lat.contains(*it1)) continue;
        auto it2 = it1;
        ++it2;
        for (; it2 != ch.end(); ++it2) {
          if (lat.contains(*it2)) continue;
          adj[*it1].push_back(*it2);
          adj[*it2].push_back(*it1);
        }
      }
    }

    // BFS/DFS over observed nodes
    std::vector< NodeSet >       comps;
    std::unordered_set< NodeId > seen;
    for (auto v: V) {
      if (seen.count(v)) continue;
      NodeSet              comp;
      std::queue< NodeId > q;
      q.push(v);
      seen.insert(v);
      comp.insert(v);
      while (!q.empty()) {
        auto u = q.front();
        q.pop();
        for (auto w: adj[u]) {
          if (!seen.count(w)) {
            seen.insert(w);
            comp.insert(w);
            q.push(w);
          }
        }
      }
      comps.push_back(std::move(comp));
    }
    return comps;
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< NodeSet >
      DoCalculus< GUM_SCALAR >::_cDecompositionOn_(const CausalModel< GUM_SCALAR >& sub) const {
    return _cDecomposition_(sub);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< NodeId >
      DoCalculus< GUM_SCALAR >::_topoObserved_(const CausalModel< GUM_SCALAR >& cm) const {
    const auto& dag = cm.observationalBN().dag();
    const NodeSet V = dag.nodes().asNodeSet();

    // indegree in the observed DAG
    std::unordered_map< NodeId, Size > indeg;
    for (auto v: V)
      indeg[v] = dag.parents(v).size();

    std::vector< NodeId > order;
    order.reserve(V.size());

    // Kahn on observed DAG only
    std::queue< NodeId > q;
    for (const auto& kv: indeg)
      if (kv.second == 0) q.push(kv.first);

    while (!q.empty()) {
      const NodeId u = q.front();
      q.pop();
      order.push_back(u);

      for (auto c: dag.children(u)) {
        if (indeg[c] > 0) {
          --indeg[c];
          if (indeg[c] == 0)
            q.push(c);
        }
      }
    }
    return order;
  }

  template < GUM_Numeric GUM_SCALAR >
  NodeSet DoCalculus< GUM_SCALAR >::_ancestorsIn_(const DAG& g, const NodeSet& T) {
    NodeSet res = T * g.nodes().asNodeSet();   // keep only nodes that actually exist in g

    std::queue< NodeId > q;
    for (auto t: res)
      q.push(t);

    while (!q.empty()) {
      NodeId u = q.front();
      q.pop();
      for (auto p: g.parents(u)) {
        if (!res.contains(p)) {
          res.insert(p);
          q.push(p);
        }
      }
    }
    return res;
  }

  template < GUM_Numeric GUM_SCALAR >
  DAG DoCalculus< GUM_SCALAR >::_removeIncomingInto_(const DAG& dag, const NodeSet& X) {
    DAG g = dag;                // copy
    for (auto x: X) {
      auto ps = g.parents(x);   // snapshot
      for (auto p: ps)
        g.eraseArc(Arc(p, x));
    }
    return g;
  }

  template < GUM_Numeric GUM_SCALAR >
  DAG DoCalculus< GUM_SCALAR >::_removeInIntoDoing_outOfKnowing_(const DAG&     dag,
                                                                 const NodeSet& doing,
                                                                 const NodeSet& knowing) {
    DAG g = dag;   // copy
    for (auto x: doing) {
      auto ps = g.parents(x);
      for (auto p: ps)
        g.eraseArc(Arc(p, x));
    }
    for (auto z: knowing) {
      auto ch = g.children(z);
      for (auto c: ch)
        g.eraseArc(Arc(z, c));
    }
    return g;
  }

  /* ========================================================================== */
  /*                             General identification                         */
  /* ========================================================================== */

  template < GUM_Numeric GUM_SCALAR >
  typename DoCalculus< GUM_SCALAR >::FormulaPtr
      DoCalculus< GUM_SCALAR >::identifyingIntervention(const NodeSet& Y, const NodeSet& X) const {
    return _ID_(_cm, Y, X, nullptr);
  }

  template < GUM_Numeric GUM_SCALAR >
  typename DoCalculus< GUM_SCALAR >::FormulaPtr
      DoCalculus< GUM_SCALAR >::identifyingIntervention(const NameList& Y,
                                                        const NameList& X) const {
    NodeSet YY, XX;
    for (const auto& s: Y)
      YY.insert(_cm.idFromName(s));
    for (const auto& s: X)
      XX.insert(_cm.idFromName(s));
    return identifyingIntervention(YY, XX);
  }

  template < GUM_Numeric GUM_SCALAR >
  typename DoCalculus< GUM_SCALAR >::FormulaPtr
      DoCalculus< GUM_SCALAR >::doCalculus(const NodeSet& on, const NodeSet& doing) const {
    return identifyingIntervention(on, doing);
  }

  template < GUM_Numeric GUM_SCALAR >
  typename DoCalculus< GUM_SCALAR >::FormulaPtr
      DoCalculus< GUM_SCALAR >::doCalculus(const NameList& on, const NameList& doing) const {
    return identifyingIntervention(on, doing);
  }

  template < GUM_Numeric GUM_SCALAR >
  typename DoCalculus< GUM_SCALAR >::FormulaPtr
      DoCalculus< GUM_SCALAR >::doCalculusWithObservation(const NodeSet& on,
                                                          const NodeSet& doing,
                                                          const NodeSet& knowing) const {
    // IDC reduction: try to promote any Z∈knowing to doing if Z ⟂ on | doing ∪ (knowing\{Z})
    // in the graph where incoming into doing and outgoing from knowing are removed.
    if (!knowing.empty()) {
      DAG gmod = _removeInIntoDoing_outOfKnowing_(_cm.causalDAG(), doing, knowing);

      for (auto z: knowing) {
        NodeSet Zi;
        Zi.insert(z);
        NodeSet Kminus = knowing - Zi;
        if (Separation::isDSeparated(gmod, Zi, on, doing + Kminus)) {
          // recurse with z moved to doing
          NodeSet doing2   = doing + Zi;
          NodeSet knowing2 = Kminus;
          return doCalculusWithObservation(on, doing2, knowing2);
        }
      }
    }

    // Fallback: ratio P(on ∪ knowing | do(doing)) / P(knowing | do(doing))
    NameList Yw, W;
    for (auto y: on)
      Yw.push_back(_cm.nameFromId(y));
    for (auto w: knowing)
      W.push_back(_cm.nameFromId(w));

    NameList Xn;
    for (auto x: doing)
      Xn.push_back(_cm.nameFromId(x));

    // numerator: ID on Y ∪ W ; denominator: ID on W (if W empty, denominator is 1)
    NodeSet onUkn = on + knowing;

    auto num = identifyingIntervention(onUkn, doing);
    if (knowing.empty()) {
      // P(Y|do(X)) when knowing is empty: just return numerator
      return num;
    }
    auto den = identifyingIntervention(knowing, doing);
    return std::make_unique< ASTdiv< GUM_SCALAR > >(std::move(num), std::move(den));
  }

  template < GUM_Numeric GUM_SCALAR >
  typename DoCalculus< GUM_SCALAR >::FormulaPtr
      DoCalculus< GUM_SCALAR >::doCalculusWithObservation(const NameList& on,
                                                          const NameList& doing,
                                                          const NameList& knowing) const {
    NodeSet ON, DO, KN;
    for (const auto& s: on)
      ON.insert(_cm.idFromName(s));
    for (const auto& s: doing)
      DO.insert(_cm.idFromName(s));
    for (const auto& s: knowing)
      KN.insert(_cm.idFromName(s));
    return doCalculusWithObservation(ON, DO, KN);
  }

  /* ========================================================================== */
  /*                                  ID core                                   */
  /* ========================================================================== */

  template < GUM_Numeric GUM_SCALAR >
  typename DoCalculus< GUM_SCALAR >::FormulaPtr
      DoCalculus< GUM_SCALAR >::_ID_(const CausalModel< GUM_SCALAR >&         cm,
                                     const NodeSet&                           Y,
                                     const NodeSet&                           X,
                                     std::unique_ptr< ASTtree< GUM_SCALAR > > P) const {
    const NodeSet lat = cm.latentVariablesIds();
    const NodeSet V   = cm.observationalBN().dag().nodes().asNodeSet();

    // --- 1) if X is empty ---
    if (X.empty()) {
      if (!P) {
        // return P(Y)
        Set< std::string > Yn;
        for (auto y: Y)
          Yn.insert(cm.nameFromId(y));
        return std::make_unique< ASTjointProba< GUM_SCALAR > >(Yn);
      }
      // return sum_{V\Y} P
      NodeSet Vy = V - Y;
      if (Vy.empty()) return std::move(P);
      std::vector< std::string > tosum;
      tosum.reserve(Vy.size());
      for (auto v: Vy)
        tosum.push_back(cm.nameFromId(v));
      return std::make_unique< ASTsum< GUM_SCALAR > >(tosum, std::move(P));
    }

    // --- 2) restrict to ancestors of Y in G ---
    {
      DAG     g = cm.causalDAG();
      NodeSet A = _ancestorsIn_(g, Y);
      A += Y;
      // Only observed ancestors (V is observed-only)
      NodeSet Aobs = A - lat;
      if (Aobs.size() != V.size()) {
        // sum out V \ Aobs from P if present, recurse on induced submodel G[Aobs]
        if (P) {
          NodeSet VminusA = V - Aobs;
          if (!VminusA.empty()) {
            std::vector< std::string > tosum;
            tosum.reserve(VminusA.size());
            for (auto v: VminusA)
              tosum.push_back(cm.nameFromId(v));
            P = std::make_unique< ASTsum< GUM_SCALAR > >(tosum, std::move(P));
          }
        }
        auto    sub = cm.inducedCausalSubModel(cm, Aobs);
        NodeSet XA  = X * Aobs;
        return _ID_(sub, Y, XA, std::move(P));
      }
    }


    // --- 3) W := (V\X) \ An_{G_{\overline X}}(Y); if non-empty, move W to X and recurse ---
    {
      DAG     gXbar = _removeIncomingInto_(cm.causalDAG(), X);
      NodeSet AnY   = _ancestorsIn_(gXbar, Y);
      AnY += Y;
      NodeSet W = (V - X) - AnY;
      if (!W.empty()) { return _ID_(cm, Y, X + W, std::move(P)); }
    }

    // --- 4) c-decompose G[V\X]; if >1 component, multiply and sum ---
    {
      auto gvx   = cm.inducedCausalSubModel(cm, V - X);
      auto icomp = _cDecompositionOn_(gvx);

      if (icomp.size() > 1) {
        // t = Π_i ID(S_i, V \ S_i)
        std::unique_ptr< ASTtree< GUM_SCALAR > > t;
        for (size_t i = 0; i < icomp.size(); ++i) {
          NodeSet  Si = icomp[i];
          NameList Sn;
          for (auto s: Si)
            Sn.push_back(cm.nameFromId(s));

          std::unique_ptr< ASTtree< GUM_SCALAR > > Pcopy
              = P ? std::unique_ptr< ASTtree< GUM_SCALAR > >(P->copy()) : nullptr;
          auto term = _ID_(cm, Si, V - Si, std::move(Pcopy));
          if (!t) t = std::move(term);
          else t = std::make_unique< ASTmult< GUM_SCALAR > >(std::move(term), std::move(t));
        }
        NodeSet Vyx = V - (X + Y);
        if (Vyx.empty()) return t;
        std::vector< std::string > tosum;
        tosum.reserve(Vyx.size());
        for (auto v: Vyx)
          tosum.push_back(cm.nameFromId(v));
        return std::make_unique< ASTsum< GUM_SCALAR > >(tosum, std::move(t));
      }

      // Only one component:
      NodeSet S   = icomp.front();
      auto    cdg = _cDecomposition_(cm);

      // --- 5) Hedge check: if full graph is one c-component, not identifiable
      if (cdg.size() == 1) {
        NodeSet all = cdg.front();
        if (all.size() == V.size()) {
          // build names for message
          Set< std::string > Vn, Sn;
          for (auto v: V)
            Vn.insert(cm.nameFromId(v));
          for (auto s: S)
            Sn.insert(cm.nameFromId(s));
          auto msg = _hedgeExceptionMsg(cm, Y, X, V, S);
          GUM_ERROR(HedgeException, msg);
        }
      }

      // --- 6) If S is also a c-component in G: product of local conditionals over S
      {
        // test membership: S ∈ cdg ?
        bool sInCdg = false;
        for (const auto& comp: cdg) {
          if (comp == S) {
            sInCdg = true;
            break;
          }
        }

        if (sInCdg) {
          // P_new := Π_{v∈S in topo order} P(v | v_π) under current P (if provided),
          // otherwise P from the BN (chain rule)
          const auto& bn    = cm.observationalBN();
          auto        order = _topoObserved_(cm);

          std::unique_ptr< ASTtree< GUM_SCALAR > > prod;

          // Make a name lookup for quick topo prefix
          std::unordered_map< NodeId, Size > pos;
          for (Size i = 0; i < order.size(); ++i)
            pos[order[i]] = i;

          // build in topo order filtered to S
          std::vector< NodeId > ordering;
          ordering.reserve(S.size());
          for (auto v: order)
            if (S.contains(v)) ordering.push_back(v);

          for (Size j = 0; j < ordering.size(); ++j) {
            auto v = ordering[j];
            std::unique_ptr< ASTtree< GUM_SCALAR > > term;
            if (!P) {
              // from BN: P(v | predecessors) using chain rule prefix
              Size               k = pos[v];
              Set< std::string > condNames;
              for (Size i = 0; i < k; ++i)
                condNames.insert(cm.nameFromId(order[i]));
              Set< std::string > lhs;
              lhs.insert(cm.nameFromId(v));
              if (condNames.empty()) term = std::make_unique< ASTjointProba< GUM_SCALAR > >(lhs);
              else term = std::make_unique< ASTposteriorProba< GUM_SCALAR > >(bn, lhs, condNames);
            } else {
              // from accumulated P: factorize Q[S] by full-order prefix marginals
              Size k = pos[v];
              std::vector< std::string > prefixNames;
              prefixNames.reserve(k + 1);
              for (Size i = 0; i <= k; ++i)
                prefixNames.push_back(cm.nameFromId(order[i]));

              // numerator = marginal of P on current prefix
              std::unique_ptr< ASTtree< GUM_SCALAR > > num(
                  P->copy());
              std::vector< std::string > sumNum;
              for (auto u: V) {
                const auto& un = cm.nameFromId(u);
                if (std::find(prefixNames.begin(), prefixNames.end(), un) == prefixNames.end()) {
                  sumNum.push_back(un);
                }
              }
              if (!sumNum.empty()) {
                num = std::make_unique< ASTsum< GUM_SCALAR > >(sumNum, std::move(num));
              }

              if (k == 0) {
                Set< std::string > lhs;
                lhs.insert(cm.nameFromId(v));
                term = std::make_unique< ASTjointProba< GUM_SCALAR > >(lhs);

              } else {
                // denominator = marginal of P on previous prefix
                std::unique_ptr< ASTtree< GUM_SCALAR > > den(
                    P->copy());
                std::vector< std::string > prevPrefix;
                prevPrefix.reserve(k);
                for (Size i = 0; i < k; ++i)
                  prevPrefix.push_back(cm.nameFromId(order[i]));
                std::vector< std::string > sumDen;
                for (auto u: V) {
                  const auto& un = cm.nameFromId(u);
                  if (std::find(prevPrefix.begin(), prevPrefix.end(), un) == prevPrefix.end()) {
                    sumDen.push_back(un);
                  }
                }
                if (!sumDen.empty()) {
                  den = std::make_unique< ASTsum< GUM_SCALAR > >(sumDen, std::move(den));
                }
                term = std::make_unique< ASTdiv< GUM_SCALAR > >(std::move(num), std::move(den));
              }
            }

            if (!prod) prod = std::move(term);
            else prod = std::make_unique< ASTmult< GUM_SCALAR > >(std::move(term), std::move(prod));
          }

          NodeSet Sy = S - Y;
          if (Sy.empty()) return prod;
          std::vector< std::string > tosum;
          tosum.reserve(Sy.size());
          for (auto v: Sy)
            tosum.push_back(cm.nameFromId(v));
          return std::make_unique< ASTsum< GUM_SCALAR > >(tosum, std::move(prod));
        }
      }

      // --- 7) Else: pick a c-component S' of G, build Q[S'] then recurse on G[S']
      {
        // choose the c-component of G that contains S (SP Step 7: S ⊆ S')
        NodeSet Spr;
        bool    found = false;
        for (const auto& comp: cdg) {
          if ((S - comp).empty()) {
            Spr   = comp;
            found = true;
            break;
          }
        }
        if (!found) {
          // Fallback should not happen; be conservative
          Spr = S;
        }

        const auto& bn    = cm.observationalBN();
        auto        order = _topoObserved_(cm);

        std::unique_ptr< ASTtree< GUM_SCALAR > > prod;

        std::unordered_map< NodeId, Size > pos;
        for (Size i = 0; i < order.size(); ++i)
          pos[order[i]] = i;

        std::vector< NodeId > ordering;
        ordering.reserve(Spr.size());
        for (auto v: order)
          if (Spr.contains(v)) ordering.push_back(v);

        for (Size j = 0; j < ordering.size(); ++j) {
          auto v = ordering[j];
          std::unique_ptr< ASTtree< GUM_SCALAR > > term;
          if (!P) {
            Size               k = pos[v];
            Set< std::string > condNames;
            for (Size i = 0; i < k; ++i)
              condNames.insert(cm.nameFromId(order[i]));
            Set< std::string > lhs;
            lhs.insert(cm.nameFromId(v));
            if (condNames.empty()) term = std::make_unique< ASTjointProba< GUM_SCALAR > >(lhs);
            else term = std::make_unique< ASTposteriorProba< GUM_SCALAR > >(bn, lhs, condNames);
          } else {
            // from accumulated P: factorize Q[Spr] by full-order prefix marginals
            Size k = pos[v];
            std::vector< std::string > prefixNames;
            prefixNames.reserve(k + 1);
            for (Size i = 0; i <= k; ++i)
              prefixNames.push_back(cm.nameFromId(order[i]));

            // numerator = marginal of P on current prefix
            std::unique_ptr< ASTtree< GUM_SCALAR > > num(
                P->copy());
            std::vector< std::string > sumNum;
            for (auto u: V) {
              const auto& un = cm.nameFromId(u);
              if (std::find(prefixNames.begin(), prefixNames.end(), un) == prefixNames.end()) {
                sumNum.push_back(un);
              }
            }
            if (!sumNum.empty()) {
              num = std::make_unique< ASTsum< GUM_SCALAR > >(sumNum, std::move(num));
            }

            if (k == 0) {
              Set< std::string > lhs;
              lhs.insert(cm.nameFromId(v));
              term = std::make_unique< ASTjointProba< GUM_SCALAR > >(lhs);

            } else {
              // denominator = marginal of P on previous prefix
              std::unique_ptr< ASTtree< GUM_SCALAR > > den(
                  P->copy());
              std::vector< std::string > prevPrefix;
              prevPrefix.reserve(k);
              for (Size i = 0; i < k; ++i)
                prevPrefix.push_back(cm.nameFromId(order[i]));
              std::vector< std::string > sumDen;
              for (auto u: V) {
                const auto& un = cm.nameFromId(u);
                if (std::find(prevPrefix.begin(), prevPrefix.end(), un) == prevPrefix.end()) {
                  sumDen.push_back(un);
                }
              }
              if (!sumDen.empty()) {
                den = std::make_unique< ASTsum< GUM_SCALAR > >(sumDen, std::move(den));
              }
              term = std::make_unique< ASTdiv< GUM_SCALAR > >(std::move(num), std::move(den));
            }
          }

          if (!prod) prod = std::move(term);
          else prod = std::make_unique< ASTmult< GUM_SCALAR > >(std::move(term), std::move(prod));
        }

        auto    sub  = cm.inducedCausalSubModel(cm, Spr);
        NodeSet Yspr = Y * Spr;   // <— important
        NodeSet Xspr = X * Spr;

        if (Yspr.empty()) {
          GUM_ERROR(OperationNotAllowed,
                    "Internal error in ID step 7: chosen C-component is disjoint from Y.");
        }

        return _ID_(sub, Yspr, Xspr, std::move(prod));
      }
    }
  }

}   // namespace gum
