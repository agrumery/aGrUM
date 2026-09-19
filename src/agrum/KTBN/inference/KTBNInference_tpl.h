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
 * @brief Template implementation of gum::KTBNInference (interface algorithm).
 *
 * @author Seth AGUILA & Anis KHACEF
 */

#pragma once

#include <algorithm>
#include <cctype>
#include <cmath>
#include <limits>
#include <sstream>

#include <agrum/KTBN/inference/KTBNInference.h>

namespace gum {

  namespace {
    /// Sentinels for _belief_'s "which incoming message to leave out": a real
    /// clique id, or one of the two virtual inter-window edges, or nothing.
    constexpr NodeId KTBN_SKIP_NONE = std::numeric_limits< NodeId >::max();
    constexpr NodeId KTBN_SKIP_PREV = std::numeric_limits< NodeId >::max() - 1;
    constexpr NodeId KTBN_SKIP_NEXT = std::numeric_limits< NodeId >::max() - 2;
  }   // namespace

  // ===========================================================================
  // Constructor
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNInference< GUM_SCALAR >::KTBNInference(const KTBN< GUM_SCALAR >* ktbn) : _ktbn_(ktbn) {
    if (ktbn == nullptr) GUM_ERROR(InvalidArgument, "KTBNInference: the k-DBN is null.")
    _k_ = static_cast< int >(ktbn->k());

    // a deterministic base order (the KTBN's own sets are unordered): temporal
    // first, then atemporal, so a slot's base index alone says which it is.
    _temporalSorted_.assign(ktbn->temporalVarNames().begin(), ktbn->temporalVarNames().end());
    std::sort(_temporalSorted_.begin(), _temporalSorted_.end());
    _atemporalSorted_.assign(ktbn->atemporalVarNames().begin(), ktbn->atemporalVarNames().end());
    std::sort(_atemporalSorted_.begin(), _atemporalSorted_.end());

    _baseNames_  = _temporalSorted_;
    _nbTemporal_ = _baseNames_.size();
    _baseNames_.insert(_baseNames_.end(), _atemporalSorted_.begin(), _atemporalSorted_.end());
    for (std::size_t i = 0; i < _baseNames_.size(); ++i)
      _baseIdx_[_baseNames_[i]] = static_cast< int >(i);

    // compile windows for the default (all-bases) query; _markRequisite_()
    // recompiles only if a later target/observation set changes the subset
    _requisite_.assign(_baseNames_.size(), true);
    _buildWindows_();
  }

  // ===========================================================================
  // Name encoding / decoding / validation
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  std::string KTBNInference< GUM_SCALAR >::_encode_(const std::string& base, int slice) {
    if (slice == ATEMPORAL) return base;
    return base + "[" + std::to_string(slice) + "]";
  }

  template < GUM_Numeric GUM_SCALAR >
  std::pair< std::string, int >
      KTBNInference< GUM_SCALAR >::_determineNode_(const std::string& name) const {
    // a name registered as atemporal is atemporal, even if bracket-shaped
    if (_ktbn_->atemporalVarNames().find(name) != _ktbn_->atemporalVarNames().end())
      return {name, ATEMPORAL};

    // only a well-formed "base[digits]" suffix is temporal (mirrors
    // KTBN::_decodeName_); anything else falls through to atemporal
    const auto lb = name.rfind('[');
    if (lb == std::string::npos || name.back() != ']') return {name, ATEMPORAL};

    const std::string inner = name.substr(lb + 1, name.size() - lb - 2);
    if (inner.empty()) return {name, ATEMPORAL};
    for (const char c: inner)
      if (std::isdigit(static_cast< unsigned char >(c)) == 0) return {name, ATEMPORAL};

    int slice{};
    try {
      slice = std::stoi(inner);
    } catch (const std::out_of_range&) {
      GUM_ERROR(InvalidArgument,
                "Node name '" << name << "' has a slice index too large to represent as int.")
    }
    return {name.substr(0, lb), slice};
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::_validateNode_(const std::string& base, int slice) const {
    const bool temporal  = _isTemporal_(base);
    const bool atemporal = _isAtemporal_(base);

    if (!temporal && !atemporal) GUM_ERROR(NotFound, "Unknown variable '" << base << "'.")
    if (temporal && slice < 0)
      GUM_ERROR(InvalidArgument, "Temporal variable '" << base << "' requires a slice >= 0.")
    if (atemporal && slice != ATEMPORAL)
      GUM_ERROR(InvalidArgument, "Atemporal variable '" << base << "' has no time slice.")
  }

  template < GUM_Numeric GUM_SCALAR >
  const DiscreteVariable& KTBNInference< GUM_SCALAR >::_templateVar_(const std::string& base,
                                                                     int slice) const {
    if (slice == ATEMPORAL) return _ktbn_->variable(base, ATEMPORAL);
    return _ktbn_->variable(base, (slice < _k_) ? slice : _k_ - 1);
  }

  template < GUM_Numeric GUM_SCALAR >
  const DiscreteVariable* KTBNInference< GUM_SCALAR >::_varOfSlot_(const _Slot_& s, int t) const {
    if (s.lag == ATEMPORAL) return &_ktbn_->variable(_baseNames_[s.base], ATEMPORAL);
    return &_ktbn_->variable(_baseNames_[s.base], (t - s.lag) % _k_);
  }

  // ===========================================================================
  // Interventions
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::addIntervention(std::string_view    base,
                                                    int                 slice,
                                                    const KTBNModality& value) {
    const std::string b(base);
    _validateNode_(b, slice);
    _interventions_[_encode_(b, slice)] = value.toIndex(_templateVar_(b, slice));
    _done_                              = false;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::addIntervention(std::string_view    node_name,
                                                    const KTBNModality& value) {
    const auto [b, s] = _determineNode_(std::string(node_name));
    addIntervention(b, s, value);
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::addIntervention(
      const std::vector< std::pair< NodeKey, KTBNModality > >& interventions) {
    // resolve and validate the whole batch first, so a bad entry cannot leave
    // the engine with the preceding entries already applied
    std::vector< std::pair< std::string, Idx > > resolved;
    resolved.reserve(interventions.size());
    for (const auto& [key, value]: interventions) {
      const auto [b, s] = std::holds_alternative< std::string >(key)
                            ? _determineNode_(std::get< std::string >(key))
                            : std::get< std::pair< std::string, int > >(key);
      _validateNode_(b, s);
      resolved.emplace_back(_encode_(b, s), value.toIndex(_templateVar_(b, s)));
    }

    for (const auto& [name, idx]: resolved)
      _interventions_[name] = idx;
    if (!resolved.empty()) _done_ = false;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::eraseIntervention(std::string_view base, int slice) {
    _interventions_.erase(_encode_(std::string(base), slice));
    _done_ = false;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::eraseIntervention(std::string_view node_name) {
    const auto [b, s] = _determineNode_(std::string(node_name));
    eraseIntervention(b, s);
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::clearInterventions() {
    _interventions_.clear();
    _done_ = false;
  }

  template < GUM_Numeric GUM_SCALAR >
  bool KTBNInference< GUM_SCALAR >::hasIntervention(std::string_view base, int slice) const {
    return _interventions_.find(_encode_(std::string(base), slice)) != _interventions_.end();
  }

  template < GUM_Numeric GUM_SCALAR >
  bool KTBNInference< GUM_SCALAR >::hasIntervention(std::string_view node_name) const {
    const auto [b, s] = _determineNode_(std::string(node_name));
    return hasIntervention(b, s);
  }

  // ===========================================================================
  // Observations
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::addObservation(std::string_view    base,
                                                   int                 slice,
                                                   const KTBNModality& value) {
    const std::string b(base);
    _validateNode_(b, slice);
    const DiscreteVariable&   v   = _templateVar_(b, slice);
    const Idx                 idx = value.toIndex(v);
    std::vector< GUM_SCALAR > like(v.domainSize(), GUM_SCALAR(0));
    like[idx]                          = GUM_SCALAR(1);
    _observations_[_encode_(b, slice)] = std::move(like);
    _done_                             = false;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::addObservation(std::string_view    node_name,
                                                   const KTBNModality& value) {
    const auto [b, s] = _determineNode_(std::string(node_name));
    addObservation(b, s, value);
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::addObservation(std::string_view                 base,
                                                   int                              slice,
                                                   const std::vector< GUM_SCALAR >& likelihood) {
    const std::string b(base);
    _validateNode_(b, slice);
    const DiscreteVariable& v = _templateVar_(b, slice);
    if (likelihood.size() != v.domainSize())
      GUM_ERROR(InvalidArgument,
                "Soft observation on '" << _encode_(b, slice) << "' needs " << v.domainSize()
                                        << " values, got " << likelihood.size() << ".")
    GUM_SCALAR total = GUM_SCALAR(0);
    for (const auto x: likelihood) {
      if (x < GUM_SCALAR(0))
        GUM_ERROR(InvalidArgument,
                  "Soft observation on '" << _encode_(b, slice) << "' has a negative entry.")
      total += x;
    }
    if (total <= GUM_SCALAR(0))
      GUM_ERROR(InvalidArgument,
                "Soft observation on '" << _encode_(b, slice) << "' is all zeros: impossible.")
    _observations_[_encode_(b, slice)] = likelihood;
    _done_                             = false;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::addObservation(std::string_view                 node_name,
                                                   const std::vector< GUM_SCALAR >& likelihood) {
    const auto [b, s] = _determineNode_(std::string(node_name));
    addObservation(b, s, likelihood);
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::addObservations(
      const std::vector< std::pair< NodeKey, KTBNModality > >& observations) {
    // all-or-nothing, like the batch addIntervention()
    std::vector< std::pair< std::string, std::vector< GUM_SCALAR > > > resolved;
    resolved.reserve(observations.size());
    for (const auto& [key, value]: observations) {
      const auto [b, s] = std::holds_alternative< std::string >(key)
                            ? _determineNode_(std::get< std::string >(key))
                            : std::get< std::pair< std::string, int > >(key);
      _validateNode_(b, s);
      const DiscreteVariable&   v = _templateVar_(b, s);
      std::vector< GUM_SCALAR > like(v.domainSize(), GUM_SCALAR(0));
      like[value.toIndex(v)] = GUM_SCALAR(1);
      resolved.emplace_back(_encode_(b, s), std::move(like));
    }
    for (auto& [name, like]: resolved)
      _observations_[name] = std::move(like);
    if (!resolved.empty()) _done_ = false;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::eraseObservation(std::string_view base, int slice) {
    _observations_.erase(_encode_(std::string(base), slice));
    _done_ = false;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::eraseObservation(std::string_view node_name) {
    const auto [b, s] = _determineNode_(std::string(node_name));
    eraseObservation(b, s);
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::clearObservation() {
    _observations_.clear();
    _done_ = false;
  }

  template < GUM_Numeric GUM_SCALAR >
  bool KTBNInference< GUM_SCALAR >::hasObservation(std::string_view base, int slice) const {
    return _observations_.find(_encode_(std::string(base), slice)) != _observations_.end();
  }

  template < GUM_Numeric GUM_SCALAR >
  bool KTBNInference< GUM_SCALAR >::hasObservation(std::string_view node_name) const {
    const auto [b, s] = _determineNode_(std::string(node_name));
    return hasObservation(b, s);
  }

  template < GUM_Numeric GUM_SCALAR >
  bool KTBNInference< GUM_SCALAR >::hasObservation() const {
    return !_observations_.empty();
  }

  // ===========================================================================
  // Targets
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::addTarget(std::string_view base) {
    const std::string b(base);
    if (!_isTemporal_(b) && !_isAtemporal_(b))
      GUM_ERROR(NotFound, "Unknown variable '" << b << "'.")
    _targets_.insert(b);
    _targeted_mode_ = true;
    _done_          = false;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::eraseTarget(std::string_view base) {
    _targets_.erase(std::string(base));
    if (_targets_.empty()) _targeted_mode_ = false;
    _done_ = false;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::clearTargets() {
    _targets_.clear();
    _targeted_mode_ = false;
    _done_          = false;
  }

  template < GUM_Numeric GUM_SCALAR >
  bool KTBNInference< GUM_SCALAR >::isTarget(std::string_view base) const {
    const std::string b(base);
    if (!_targeted_mode_) return _isTemporal_(b) || _isAtemporal_(b);
    return _targets_.contains(b);
  }

  template < GUM_Numeric GUM_SCALAR >
  bool KTBNInference< GUM_SCALAR >::isInTargetMode() const {
    return _targeted_mode_;
  }

  // ===========================================================================
  // Structure: interfaces and window compilation
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  std::vector< typename KTBNInference< GUM_SCALAR >::_Slot_ >
      KTBNInference< GUM_SCALAR >::_familySlots_(int baseIdx, int t) const {
    std::vector< _Slot_ > out;
    const std::string&    b = _baseNames_[baseIdx];

    if (baseIdx >= static_cast< int >(_nbTemporal_)) {
      out.push_back({baseIdx, ATEMPORAL});
      for (const auto& [pb, ps]: _ktbn_->parents(b, ATEMPORAL)) {
        (void)ps;
        out.push_back({_baseIdx_.at(pb), ATEMPORAL});
      }
      return out;
    }

    // initial CPTs while inside the initial block, else the (time-invariant)
    // transition kernel -- why one window template serves every later t
    const int structSlice = (t <= _k_ - 1) ? t : _k_ - 1;
    out.push_back({baseIdx, 0});
    for (const auto& [pb, ps]: _ktbn_->parents(b, structSlice)) {
      if (ps == ATEMPORAL) out.push_back({_baseIdx_.at(pb), ATEMPORAL});
      else out.push_back({_baseIdx_.at(pb), structSlice - ps});
    }
    return out;
  }

  template < GUM_Numeric GUM_SCALAR >
  int KTBNInference< GUM_SCALAR >::_lastConsumerSlice_(int baseIdx, int s) const {
    int last = -1;

    // consumers inside the initial block (their parent sets are per-slice)
    for (int tc = 0; tc <= _k_ - 2; ++tc) {
      if (tc <= last) continue;
      for (int c = 0; c < static_cast< int >(_nbTemporal_); ++c) {
        if (!_requisite_[c]) continue;
        for (const auto& [pb, ps]: _ktbn_->parents(_baseNames_[c], tc)) {
          if (ps == ATEMPORAL) continue;
          if (ps == s && _baseIdx_.at(pb) == baseIdx) {
            last = tc;
            break;
          }
        }
        if (tc == last) break;
      }
    }

    // repeating block: the kernel reaches back _maxLag_ slices, so the last
    // consumer sits at s + maxLag
    if (_maxLag_[baseIdx] >= 1) {
      const int tc = s + _maxLag_[baseIdx];
      if (tc >= _k_ - 1 && tc > last) last = tc;
    }
    return last;
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< typename KTBNInference< GUM_SCALAR >::_Slot_ >
      KTBNInference< GUM_SCALAR >::_interfaceAfter_(int t) const {
    std::vector< _Slot_ > out;
    for (int i = 0; i < static_cast< int >(_nbTemporal_); ++i) {
      if (!_requisite_[i]) continue;
      for (int s = t; s >= 0 && t - s <= _k_ - 1; --s)
        if (_lastConsumerSlice_(i, s) > t) out.push_back({i, t - s});
    }
    // an atemporal variable is a parent at every slice, so it never leaves
    // the interface -- the one thing that couples the whole horizon
    for (int i = static_cast< int >(_nbTemporal_); i < static_cast< int >(_baseNames_.size()); ++i)
      if (_requisite_[i]) out.push_back({i, ATEMPORAL});
    return out;
  }

  template < GUM_Numeric GUM_SCALAR >
  typename KTBNInference< GUM_SCALAR >::_Window_
      KTBNInference< GUM_SCALAR >::_compileWindow_(const std::vector< _Slot_ >& Iprev,
                                                   const std::vector< _Slot_ >& Icur,
                                                   int                          t,
                                                   bool withAtemporalFamilies) const {
    _Window_ w;
    w.Iprev = Iprev;
    w.Icur  = Icur;

    // ---- node set: H_t = I_{t-1} U V_t (plus the atemporal layer at t == 0) --
    std::map< std::pair< int, int >, NodeId > id;
    UndiGraph                                 graph;
    NodeProperty< Size >                      domSizes;

    const auto ensure = [&](const _Slot_& s) -> NodeId {
      const auto key = std::make_pair(s.base, s.lag);
      const auto it  = id.find(key);
      if (it != id.end()) return it->second;
      const NodeId nid = graph.addNode();
      id[key]          = nid;
      if (static_cast< std::size_t >(nid) >= w.slotOfNode.size()) w.slotOfNode.resize(nid + 1);
      w.slotOfNode[nid] = s;
      domSizes.insert(nid,
                      (s.lag == ATEMPORAL)
                          ? _ktbn_->variable(_baseNames_[s.base], ATEMPORAL).domainSize()
                          : _ktbn_->variable(_baseNames_[s.base], _k_ - 1).domainSize());
      return nid;
    };

    for (const auto& s: Iprev)
      ensure(s);
    for (int i = 0; i < static_cast< int >(_nbTemporal_); ++i)
      if (_requisite_[i]) ensure({i, 0});
    if (withAtemporalFamilies)
      for (int i = static_cast< int >(_nbTemporal_); i < static_cast< int >(_baseNames_.size());
           ++i)
        if (_requisite_[i]) ensure({i, ATEMPORAL});
    for (const auto& s: Icur)
      ensure(s);

    if (graph.size() == 0) return w;   // nothing requisite: an empty window

    // ---- moralise: every family becomes a clique ----------------------------
    std::vector< std::pair< int, std::vector< NodeId > > > families;
    const auto                                             addFamily = [&](int b) {
      std::vector< NodeId > scope;
      for (const auto& s: _familySlots_(b, t))
        scope.push_back(ensure(s));
      for (std::size_t a = 0; a < scope.size(); ++a)
        for (std::size_t c = a + 1; c < scope.size(); ++c)
          if (!graph.existsEdge(scope[a], scope[c])) graph.addEdge(scope[a], scope[c]);
      families.emplace_back(b, std::move(scope));
    };
    for (int i = 0; i < static_cast< int >(_nbTemporal_); ++i)
      if (_requisite_[i]) addFamily(i);
    if (withAtemporalFamilies)
      for (int i = static_cast< int >(_nbTemporal_); i < static_cast< int >(_baseNames_.size());
           ++i)
        if (_requisite_[i]) addFamily(i);

    // ---- force each interface to be a clique --------------------------------
    // Murphy's constraint: one clique must hold the whole interface, so the
    // message crossing the slice boundary is a single potential.
    const auto makeClique = [&](const std::vector< _Slot_ >& slots) {
      std::vector< NodeId > ids;
      for (const auto& s: slots)
        ids.push_back(ensure(s));
      for (std::size_t a = 0; a < ids.size(); ++a)
        for (std::size_t c = a + 1; c < ids.size(); ++c)
          if (!graph.existsEdge(ids[a], ids[c])) graph.addEdge(ids[a], ids[c]);
    };
    makeClique(Iprev);
    makeClique(Icur);

    // ---- triangulate once; this tree is reused for every slice and horizon --
    DefaultTriangulation tri(&graph, &domSizes);
    w.jt = tri.junctionTree();

    const auto covers = [&](NodeId c, const std::vector< _Slot_ >& slots) {
      const NodeSet& cl = w.jt.clique(c);
      for (const auto& s: slots)
        if (!cl.contains(id.at({s.base, s.lag}))) return false;
      return true;
    };

    bool foundC = false, foundD = false;
    for (const NodeId c: w.jt.nodes()) {
      if (!foundC && covers(c, Icur)) {
        w.rootC = c;
        foundC  = true;
      }
      if (!foundD && covers(c, Iprev)) {
        w.rootD = c;
        foundD  = true;
      }
    }
    if (!foundC || !foundD)
      GUM_ERROR(FatalError, "KTBNInference: the window interfaces did not end up in a clique.")

    // ---- assign every family factor to one clique that covers it ------------
    for (const auto& [b, scope]: families) {
      bool placed = false;
      for (const NodeId c: w.jt.nodes()) {
        const NodeSet& cl = w.jt.clique(c);
        bool           ok = true;
        for (const NodeId n: scope)
          if (!cl.contains(n)) {
            ok = false;
            break;
          }
        if (ok) {
          w.factorsOf[c].push_back(b);
          placed = true;
          break;
        }
      }
      if (!placed)
        GUM_ERROR(FatalError, "KTBNInference: family of '" << _baseNames_[b] << "' fits no clique.")
    }

    // ---- where to read each base's own marginal (and place its observation) ----
    for (const auto& [key, nid]: id) {
      const int base = key.first;
      for (const NodeId c: w.jt.nodes())
        if (w.jt.clique(c).contains(nid)) {
          if (key.second == 0 || key.second == ATEMPORAL) w.selfClique[base] = c;
          break;
        }
    }

    // ---- stitch a junction FOREST into a junction tree ----------------------
    // Barren processes and unused atemporal variables sit in their own
    // components. Linking each to C_t with an EMPTY separator makes one tree
    // without touching any marginal -- the crossing message is just a scalar.
    {
      std::unordered_map< NodeId, int > comp;
      int                               nc = 0;
      for (const NodeId s: w.jt.nodes()) {
        if (comp.count(s) != 0) continue;
        std::vector< NodeId > q{s};
        comp[s] = nc;
        for (std::size_t i = 0; i < q.size(); ++i)
          for (const NodeId nb: w.jt.neighbours(q[i]))
            if (comp.count(nb) == 0) {
              comp[nb] = nc;
              q.push_back(nb);
            }
        ++nc;
      }
      if (nc > 1) {
        std::unordered_set< int > linked{comp.at(w.rootC)};
        for (const NodeId n: w.jt.nodes())
          if (linked.insert(comp.at(n)).second) w.jt.addEdge(w.rootC, n);
      }
    }

    // ---- root the tree at C_t and record a BFS order ------------------------
    std::unordered_set< NodeId > seen;
    w.bfs.push_back(w.rootC);
    seen.insert(w.rootC);
    w.parentOf[w.rootC] = w.rootC;
    for (std::size_t qi = 0; qi < w.bfs.size(); ++qi) {
      const NodeId cur = w.bfs[qi];
      for (const NodeId nb: w.jt.neighbours(cur))
        if (seen.insert(nb).second) {
          w.parentOf[nb] = cur;
          w.bfs.push_back(nb);
        }
    }
    // the forced interface cliques rule out a disconnected forest; assert it anyway
    if (w.bfs.size() != w.jt.size())
      GUM_ERROR(FatalError, "KTBNInference: the window junction tree is not connected.")

    return w;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::_buildWindows_() {
    // how long an occurrence must survive: the deepest lag the requisite
    // kernel still reads it at -- what bounds |I|
    _maxLag_.assign(_nbTemporal_, 0);
    for (int c = 0; c < static_cast< int >(_nbTemporal_); ++c) {
      if (!_requisite_[c]) continue;
      for (const auto& [pb, ps]: _ktbn_->parents(_baseNames_[c], _k_ - 1)) {
        if (ps == ATEMPORAL) continue;
        const int i   = _baseIdx_.at(pb);
        const int lag = (_k_ - 1) - ps;
        if (lag >= 1 && lag > _maxLag_[i]) _maxLag_[i] = lag;
      }
    }

    const auto shift = [](std::vector< _Slot_ > v) {
      for (auto& s: v)
        if (s.lag != ATEMPORAL) ++s.lag;
      return v;
    };

    // windows 0..k-1 cover slices 0..k-1; window k is the repeating one, used
    // from slice k on. For k>=2, k-1 and k are structurally identical, so the
    // extra compile is free; for k==1 they must differ, since slice 0 alone
    // carries the atemporal layer in.
    _windows_.clear();
    _windows_.reserve(_k_ + 1);
    std::vector< _Slot_ > prev;   // I_{t-1}, in slice-t coordinates
    for (int t = 0; t <= _k_; ++t) {
      std::vector< _Slot_ > cur = _interfaceAfter_(t);
      _windows_.push_back(_compileWindow_(prev, cur, t, /*withAtemporalFamilies=*/t == 0));
      prev = shift(std::move(cur));
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::_markRequisite_() {
    std::vector< bool > req(_baseNames_.size(), false);
    std::vector< int >  stack;

    const auto push = [&](const std::string& b) {
      const auto it = _baseIdx_.find(b);
      if (it == _baseIdx_.end()) return;
      if (!req[it->second]) {
        req[it->second] = true;
        stack.push_back(it->second);
      }
    };

    if (!_targeted_mode_) {
      for (const auto& b: _baseNames_)
        push(b);
    } else {
      for (const auto& b: _targets_)
        push(b);
    }
    // an observed node is requisite even if barren: its likelihood is what
    // revises everything upstream of it.
    for (const auto& [name, like]: _observations_) {
      (void)like;
      push(_determineNode_(name).first);
    }

    while (!stack.empty()) {
      const int i = stack.back();
      stack.pop_back();
      const std::string& b = _baseNames_[i];
      if (i < static_cast< int >(_nbTemporal_)) {
        for (int t = 0; t <= _k_ - 1; ++t)
          for (const auto& [pb, ps]: _ktbn_->parents(b, t)) {
            (void)ps;
            push(pb);
          }
      } else {
        for (const auto& [pb, ps]: _ktbn_->parents(b, ATEMPORAL)) {
          (void)ps;
          push(pb);
        }
      }
    }

    if (req != _requisite_) {
      _requisite_ = std::move(req);
      _buildWindows_();
    }
  }

  // ===========================================================================
  // Runtime: filling and message passing
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  const Tensor< GUM_SCALAR >& KTBNInference< GUM_SCALAR >::_buildKernel_(const std::string& p,
                                                                         int t) const {
    // The result depends on t only through t % k -- for the child directly, and
    // for each parent as (t - lag) % k, which is fixed once t % k is. So one
    // tensor per (process, phase) serves every slice of that phase.
    const std::pair< std::string, int > key{p, static_cast< int >(t % _k_)};
    const auto                          hit = _kernelCache_.find(key);
    if (hit != _kernelCache_.end()) return hit->second;

    // Builds P(p[t] | its absolute-time parents) by rolling the template
    // kernel P(p[k-1] | parents) forward onto slice t. The kernel's own axes
    // use ABSOLUTE-slice identity (variable(base, slice % k), the KTBN's
    // cycled per-process objects); srcNames uses the TEMPLATE's time-invariant
    // names, only to pick which axis of cpt(p, k-1) to copy from -- fillWith's
    // mapSrc maps by position, not name, so the two need not match.
    Tensor< GUM_SCALAR >       kernel;
    std::vector< std::string > srcNames;

    kernel << _ktbn_->variable(p, t % _k_);
    srcNames.push_back(_encode_(p, _k_ - 1));

    for (const auto& [parBase, parSlice]: _ktbn_->parents(p, _k_ - 1)) {
      if (parSlice == ATEMPORAL) {
        kernel << _ktbn_->variable(parBase, ATEMPORAL);
        srcNames.push_back(parBase);
      } else {
        const int lag = (_k_ - 1) - parSlice;
        kernel << _ktbn_->variable(parBase, (t - lag) % _k_);
        srcNames.push_back(_encode_(parBase, parSlice));
      }
    }
    kernel.fillWith(_ktbn_->cpt(p, _k_ - 1), srcNames);
    return _kernelCache_.emplace(key, std::move(kernel)).first->second;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::_applyTemporalObservations_(
      const _Window_&                                     w,
      int                                                 t,
      std::unordered_map< NodeId, Tensor< GUM_SCALAR > >& psi) const {
    for (const auto& [c, bases]: w.factorsOf)
      for (const int b: bases) {
        if (b >= static_cast< int >(_nbTemporal_)) continue;   // atemporal: already in the base
        const auto ite = _observations_.find(_encode_(_baseNames_[b], t));
        if (ite == _observations_.end()) continue;
        Tensor< GUM_SCALAR > ev;
        ev << _ktbn_->variable(_baseNames_[b], t % _k_);
        ev.fillWith(ite->second);
        psi.at(c) *= ev;
      }
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE Size KTBNInference< GUM_SCALAR >::_psiKey_(int t) const {
    // t < k: the initial windows, each its own. t >= k: the repeating window,
    // one slot per phase. Keying on t % k alone would collide slice 2 with
    // slice 5 at k=3 -- same phase, different window and different CPTs.
    return (t < static_cast< int >(_k_)) ? static_cast< Size >(t)
                                         : _k_ + static_cast< Size >(t % static_cast< int >(_k_));
  }

  template < GUM_Numeric GUM_SCALAR >
  const std::unordered_map< NodeId, Tensor< GUM_SCALAR > >&
      KTBNInference< GUM_SCALAR >::_windowPotentials_(const _Window_& w, int t) const {
    // do(X=x) replaces a CPT the base already applied, so the base is unusable
    if (_interventionSlices_.contains(t)) {
      _fillWindow_(w, t, _psiScratch_, true);
      return _psiScratch_;
    }

    const Size key = _psiKey_(t);
    if (!_psiCached_[key]) {
      _fillWindow_(w, t, _psiCache_[key], false);   // base: no slice-specific evidence
      _psiCached_[key] = true;
    }

    // unobserved: the base IS the answer, handed over without a copy
    if (!_observationSlices_.contains(t)) return _psiCache_[key];

    // observed: an observation multiplies ON TOP of the CPT, so the base still
    // holds. Copying it and applying the likelihood costs one pass per clique,
    // where a rebuild would redo the unit fill and every CPT product.
    _psiScratch_ = _psiCache_[key];
    _applyTemporalObservations_(w, t, _psiScratch_);
    return _psiScratch_;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::_fillWindow_(
      const _Window_&                                     w,
      int                                                 t,
      std::unordered_map< NodeId, Tensor< GUM_SCALAR > >& psi,
      bool                                                withTemporalEvidence) const {
    psi.clear();

    // every clique starts at the unit potential: even one that receives no
    // factor must contribute its variables' free mass to a summed-out message
    for (const NodeId c: w.jt.nodes()) {
      Tensor< GUM_SCALAR > p;
      for (const NodeId n: w.jt.clique(c))
        p << *_varOfSlot_(w.slotOfNode[n], t);
      p.fillWith(GUM_SCALAR(1));
      psi.emplace(c, std::move(p));
    }

    const auto applyBase = [&](int b, NodeId owner) {
      const std::string&      base  = _baseNames_[b];
      const bool              atemp = b >= static_cast< int >(_nbTemporal_);
      const std::string       name  = _encode_(base, atemp ? ATEMPORAL : t);
      const DiscreteVariable& var
          = atemp ? _ktbn_->variable(base, ATEMPORAL) : _ktbn_->variable(base, t % _k_);

      // do(base[t]=v): the point mass REPLACES the CPT, severing the node
      // from its own causes -- an observation never does this
      // In a base build the slice's own evidence is left out, so a temporal
      // node keeps its CPT and no likelihood is applied; atemporal evidence is
      // slice-independent and stays, being part of the periodic structure.
      const bool sliceEvidence = atemp || withTemporalEvidence;

      const auto itv = sliceEvidence ? _interventions_.find(name) : _interventions_.end();
      if (itv != _interventions_.end()) {
        psi.at(owner) *= Tensor< GUM_SCALAR >::deterministicTensor(var, itv->second);
      } else if (atemp) {
        psi.at(owner) *= _ktbn_->cpt(base, ATEMPORAL);
      } else if (t <= _k_ - 1) {
        psi.at(owner) *= _ktbn_->cpt(base, t);   // initial CPT: already on ring slots
      } else {
        psi.at(owner) *= _buildKernel_(base, t);
      }

      // base[t] observed: pure conditioning, multiplied in ON TOP of the CPT.
      const auto ite = sliceEvidence ? _observations_.find(name) : _observations_.end();
      if (ite != _observations_.end()) {
        Tensor< GUM_SCALAR > ev;
        ev << var;
        ev.fillWith(ite->second);
        psi.at(owner) *= ev;
      }
    };

    for (const auto& [c, bases]: w.factorsOf)
      for (const int b: bases)
        applyBase(b, c);
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE bool KTBNInference< GUM_SCALAR >::_Slot_::operator==(const _Slot_& o) const {
    return base == o.base && lag == o.lag;
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE bool KTBNInference< GUM_SCALAR >::_isTemporal_(const std::string& base) const {
    return _ktbn_->temporalVarNames().find(base) != _ktbn_->temporalVarNames().end();
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE bool KTBNInference< GUM_SCALAR >::_isAtemporal_(const std::string& base) const {
    return _ktbn_->atemporalVarNames().find(base) != _ktbn_->atemporalVarNames().end();
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE const typename KTBNInference< GUM_SCALAR >::_Window_&
      KTBNInference< GUM_SCALAR >::_windowAt_(int t) const {
    return _windows_[static_cast< std::size_t >(t < _k_ ? t : _k_)];
  }

  template < GUM_Numeric GUM_SCALAR >
  Tensor< GUM_SCALAR > KTBNInference< GUM_SCALAR >::_belief_(
      const _Window_&                                                      w,
      const std::unordered_map< NodeId, Tensor< GUM_SCALAR > >&            psi,
      const std::map< std::pair< NodeId, NodeId >, Tensor< GUM_SCALAR > >& msgs,
      const Tensor< GUM_SCALAR >*                                          inPrev,
      const Tensor< GUM_SCALAR >*                                          inNext,
      NodeId                                                               c,
      NodeId                                                               skip) const {
    Tensor< GUM_SCALAR > out = psi.at(c);
    for (const NodeId n: w.jt.neighbours(c)) {
      if (n == skip) continue;
      const auto it = msgs.find({n, c});
      if (it != msgs.end()) out *= it->second;
    }
    // the two virtual leaves: past hangs off D_t, future off C_t. Leaving one
    // out is how Shafer-Shenoy avoids echoing a message back -- no division needed
    if (inPrev != nullptr && c == w.rootD && skip != KTBN_SKIP_PREV) out *= *inPrev;
    if (inNext != nullptr && c == w.rootC && skip != KTBN_SKIP_NEXT) out *= *inNext;
    return out;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::_propagate_(
      const _Window_&                                                w,
      int                                                            t,
      const std::unordered_map< NodeId, Tensor< GUM_SCALAR > >&      psi,
      const Tensor< GUM_SCALAR >*                                    inPrev,
      const Tensor< GUM_SCALAR >*                                    inNext,
      bool                                                           distribute,
      std::map< std::pair< NodeId, NodeId >, Tensor< GUM_SCALAR > >& msgs) const {
    msgs.clear();
    if (w.bfs.empty()) return;

    const auto sepVars = [&](NodeId a, NodeId b) {
      VariableSet keep;
      for (const NodeId n: w.jt.separator(a, b))
        keep.insert(_varOfSlot_(w.slotOfNode[n], t));
      return keep;
    };

    // collect: reverse BFS visits every child before its parent
    for (auto it = w.bfs.rbegin(); it != w.bfs.rend(); ++it) {
      const NodeId j = *it;
      if (j == w.rootC) continue;
      const NodeId p = w.parentOf.at(j);
      msgs[{j, p}]   = _belief_(w, psi, msgs, inPrev, inNext, j, p).sumIn(sepVars(j, p));
    }
    if (!distribute) return;

    // distribute: BFS visits every parent before its children
    for (const NodeId j: w.bfs)
      for (const NodeId i: w.jt.neighbours(j)) {
        if (i == j || w.parentOf.at(i) != j) continue;   // children only
        msgs[{j, i}] = _belief_(w, psi, msgs, inPrev, inNext, j, i).sumIn(sepVars(j, i));
      }
  }

  // ===========================================================================
  // Inference
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::makeInference(Size nbTimeSlices) {
    if (nbTimeSlices == 0) GUM_ERROR(InvalidArgument, "makeInference: nbTimeSlices must be >= 1.")

    const int T      = static_cast< int >(nbTimeSlices);
    _horizon_        = nbTimeSlices;
    _logObservation_ = GUM_SCALAR(0);
    _posteriors_.clear();
    _kernelCache_.clear();   // CPT values may have been edited since the last run

    // Potential cache: same reason to clear, plus the evidence may have moved.
    _psiCache_.assign(2 * _k_, {});
    _psiCached_.assign(2 * _k_, false);
    _psiScratch_.clear();

    // Which slices are not the periodic ones. Decoding the (few) evidence keys
    // costs O(#evidence); probing every slice would cost what the cache saves.
    // Atemporal evidence decodes to ATEMPORAL and marks no slice: it applies at
    // every t alike and so belongs to the periodic structure.
    _observationSlices_.clear();
    _interventionSlices_.clear();
    for (const auto& [name, value]: _observations_) {
      (void)value;
      const int s = _determineNode_(name).second;
      if (s != ATEMPORAL) _observationSlices_.insert(s);
    }
    for (const auto& [name, value]: _interventions_) {
      (void)value;
      const int s = _determineNode_(name).second;
      if (s != ATEMPORAL) _interventionSlices_.insert(s);
    }
    _markRequisite_();

    const bool defaultAll = !_targeted_mode_;
    const auto targeted = [&](const std::string& b) { return defaultAll || _targets_.contains(b); };
    // nothing observed => backward messages are provably uniform, so the
    // sweep is skipped and only one window is ever live
    const bool smoothing = hasObservation();

    // pre-size the series: with smoothing the slices are filled back-to-front
    for (int i = 0; i < static_cast< int >(_baseNames_.size()); ++i) {
      const std::string& b = _baseNames_[i];
      if (!_requisite_[i] || !targeted(b)) continue;
      _Series_&         s = _posteriors_[b];
      const std::size_t n
          = (i < static_cast< int >(_nbTemporal_)) ? static_cast< std::size_t >(T) : std::size_t(1);
      s.vars.resize(n);
      s.tensors.resize(n);
    }

    const auto ifaceVars = [&](const std::vector< _Slot_ >& slots, int slice) {
      VariableSet keep;
      for (const auto& s: slots)
        keep.insert(_varOfSlot_(s, slice));
      return keep;
    };

    const auto readPosteriors
        = [&](const _Window_&                                                      w,
              int                                                                  slice,
              const std::unordered_map< NodeId, Tensor< GUM_SCALAR > >&            psi,
              const std::map< std::pair< NodeId, NodeId >, Tensor< GUM_SCALAR > >& msgs,
              const Tensor< GUM_SCALAR >*                                          inPrev,
              const Tensor< GUM_SCALAR >*                                          inNext) {
            const int lastBase = (slice == 0) ? static_cast< int >(_baseNames_.size())
                                              : static_cast< int >(_nbTemporal_);
            for (int i = 0; i < lastBase; ++i) {
              if (!_requisite_[i] || !targeted(_baseNames_[i])) continue;
              const auto itc = w.selfClique.find(i);
              if (itc == w.selfClique.end()) continue;
              const bool  atemp = i >= static_cast< int >(_nbTemporal_);
              VariableSet keep;
              keep.insert(atemp ? &_ktbn_->variable(_baseNames_[i], ATEMPORAL)
                                : &_ktbn_->variable(_baseNames_[i], slice % _k_));
              Tensor< GUM_SCALAR > m
                  = _belief_(w, psi, msgs, inPrev, inNext, itc->second, KTBN_SKIP_NONE).sumIn(keep);
              m.normalize();
              _snapshot_(_baseNames_[i], atemp ? ATEMPORAL : slice, m);
            }
          };

    std::map< std::pair< NodeId, NodeId >, Tensor< GUM_SCALAR > > msgs;

    // ---- forward sweep: carry m_t across each slice boundary ----------------
    // only the interface is retained per slice, and only if smoothing needs it
    std::vector< Tensor< GUM_SCALAR > > fwd;
    if (smoothing && T > 1) fwd.resize(static_cast< std::size_t >(T - 1));

    Tensor< GUM_SCALAR > prev;
    bool                 hasPrev = false;

    for (int t = 0; t < T; ++t) {
      const _Window_& w = _windowAt_(t);
      if (w.bfs.empty()) break;   // nothing requisite at all

      const auto& psi = _windowPotentials_(w, t);
      _propagate_(w, t, psi, hasPrev ? &prev : nullptr, nullptr, !smoothing, msgs);

      if (!smoothing) readPosteriors(w, t, psi, msgs, hasPrev ? &prev : nullptr, nullptr);

      Tensor< GUM_SCALAR > m
          = _belief_(w, psi, msgs, hasPrev ? &prev : nullptr, nullptr, w.rootC, KTBN_SKIP_NEXT)
                .sumIn(ifaceVars(w.Icur, t));
      const GUM_SCALAR mass = m.sum();
      if (!(mass > GUM_SCALAR(0)))
        GUM_ERROR(FatalError,
                  "makeInference: the observations have probability 0 under this model "
                  "(impossible at slice "
                      << t << ").")
      m.scale(GUM_SCALAR(1) / mass);
      _logObservation_ += static_cast< GUM_SCALAR >(std::log(static_cast< double >(mass)));

      if (t + 1 < T) {
        if (smoothing) fwd[static_cast< std::size_t >(t)] = m;
        prev    = std::move(m);
        hasPrev = true;
      }
    }

    // ---- backward sweep: carry r_t the other way and combine ---------------
    if (smoothing) {
      Tensor< GUM_SCALAR > nxt;
      bool                 hasNext = false;
      for (int t = T - 1; t >= 0; --t) {
        const _Window_& w = _windowAt_(t);
        if (w.bfs.empty()) break;

        const auto&                 psi = _windowPotentials_(w, t);
        const Tensor< GUM_SCALAR >* inPrev
            = (t > 0) ? &fwd[static_cast< std::size_t >(t - 1)] : nullptr;
        const Tensor< GUM_SCALAR >* inNext = hasNext ? &nxt : nullptr;
        _propagate_(w, t, psi, inPrev, inNext, true, msgs);

        readPosteriors(w, t, psi, msgs, inPrev, inNext);

        if (t > 0) {
          Tensor< GUM_SCALAR > r = _belief_(w, psi, msgs, inPrev, inNext, w.rootD, KTBN_SKIP_PREV)
                                       .sumIn(ifaceVars(w.Iprev, t));
          const GUM_SCALAR     mass = r.sum();
          // Symmetric with the forward sweep. In exact arithmetic this cannot
          // fire -- a null backward mass would mean P(e_{t:T} | I) = 0 for every
          // interface state, hence P(e) = 0, which the forward sweep just
          // disproved. In floating point it fires readily: the intra-window
          // product is formed BEFORE any normalisation, so near-deterministic
          // CPTs flush it to zero while the true value is merely tiny
          // (reproduced with transition probabilities of 1e-100 at T=4).
          //
          // Left unguarded the failure is silent and total: r stays all-zero,
          // becomes the next inNext, and zeroes every belief from here back to
          // slice 0 -- and readPosteriors' normalize() is a no-op on a zero sum,
          // so posterior() would return tensors of zeros looking like
          // distributions. Do NOT reuse the forward sweep's wording here: the
          // observations are *not* impossible, they underflowed.
          if (!(mass > GUM_SCALAR(0)))
            GUM_ERROR(FatalError,
                      "makeInference: the backward message leaving slice "
                          << t
                          << " underflowed to zero. The observations are not impossible -- the "
                             "forward sweep accepted them (log P(obs) = "
                          << _logObservation_
                          << ") -- but the model's probabilities are too extreme for this horizon "
                             "to be resolved in double precision. Soften the near-deterministic "
                             "CPT entries, or shorten the horizon / reduce the observations.")
          r.scale(GUM_SCALAR(1) / mass);
          nxt     = std::move(r);
          hasNext = true;
        }
      }
    }

    _done_ = true;
  }

  // ===========================================================================
  // Results
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  void KTBNInference< GUM_SCALAR >::_snapshot_(const std::string&          base,
                                               int                         slice,
                                               const Tensor< GUM_SCALAR >& marginal) {
    // owned, stably-named descriptor: marginal's axis is a reused ring object
    // whose name isn't base[slice], so fillWith is positional, not name-matched
    const std::size_t idx    = (slice == ATEMPORAL) ? 0u : static_cast< std::size_t >(slice);
    _Series_&         series = _posteriors_[base];
    if (series.tensors.size() <= idx) {
      series.tensors.resize(idx + 1);
      series.vars.resize(idx + 1);
    }

    std::unique_ptr< DiscreteVariable > outVar(_templateVar_(base, slice).clone());
    outVar->setName(_encode_(base, slice));

    Tensor< GUM_SCALAR > out;
    out << *outVar;
    out.fillWith(marginal, {marginal.variablesSequence().atPos(0)->name()});

    series.vars[idx]    = std::move(outVar);
    series.tensors[idx] = std::move(out);
  }

  template < GUM_Numeric GUM_SCALAR >
  const typename KTBNInference< GUM_SCALAR >::_Series_&
      KTBNInference< GUM_SCALAR >::_series_(const std::string& base) {
    // lazily (re)run with the last horizon; a horizon of 0 means never run
    if (!_done_) {
      if (_horizon_ == 0)
        GUM_ERROR(OperationNotAllowed, "call makeInference(nbTimeSlices) before querying.")
      makeInference(_horizon_);
    }
    const auto it = _posteriors_.find(base);
    if (it == _posteriors_.end())
      GUM_ERROR(UndefinedElement, "'" << base << "' is not a target of this inference.")
    return it->second;
  }

  template < GUM_Numeric GUM_SCALAR >
  const Tensor< GUM_SCALAR >& KTBNInference< GUM_SCALAR >::posterior(std::string_view base,
                                                                     int              slice) {
    const std::string b(base);
    const _Series_&   series = _series_(b);

    if (_isAtemporal_(b)) {
      if (slice != ATEMPORAL)
        GUM_ERROR(InvalidArgument, "Atemporal variable '" << b << "' has no time slice.")
      return series.tensors.front();
    }
    if (slice < 0 || static_cast< Size >(slice) >= series.tensors.size())
      GUM_ERROR(OutOfBounds,
                "Slice " << slice << " for '" << b << "' was not computed (horizon " << _horizon_
                         << ").")
    return series.tensors[static_cast< std::size_t >(slice)];
  }

  template < GUM_Numeric GUM_SCALAR >
  const Tensor< GUM_SCALAR >& KTBNInference< GUM_SCALAR >::posterior(std::string_view node_name) {
    const auto [b, s] = _determineNode_(std::string(node_name));
    return posterior(b, s);
  }

  template < GUM_Numeric GUM_SCALAR >
  const std::vector< Tensor< GUM_SCALAR > >&
      KTBNInference< GUM_SCALAR >::posteriors(std::string_view base) {
    return _series_(std::string(base)).tensors;
  }

  template < GUM_Numeric GUM_SCALAR >
  GUM_SCALAR KTBNInference< GUM_SCALAR >::logObservationProbability() {
    if (!_done_) {
      if (_horizon_ == 0)
        GUM_ERROR(OperationNotAllowed, "call makeInference(nbTimeSlices) before querying.")
      makeInference(_horizon_);
    }
    return _logObservation_;
  }

  template < GUM_Numeric GUM_SCALAR >
  GUM_SCALAR KTBNInference< GUM_SCALAR >::observationProbability() {
    return static_cast< GUM_SCALAR >(std::exp(static_cast< double >(logObservationProbability())));
  }

  // ===========================================================================
  // Various
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  const KTBN< GUM_SCALAR >& KTBNInference< GUM_SCALAR >::ktbn() const {
    return *_ktbn_;
  }

  template < GUM_Numeric GUM_SCALAR >
  const JunctionTree& KTBNInference< GUM_SCALAR >::windowJunctionTree() const {
    return _windows_[static_cast< std::size_t >(_k_)].jt;
  }

  template < GUM_Numeric GUM_SCALAR >
  Size KTBNInference< GUM_SCALAR >::interfaceSize() const {
    return static_cast< Size >(_windows_[static_cast< std::size_t >(_k_)].Icur.size());
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string KTBNInference< GUM_SCALAR >::toString() const {
    std::stringstream s;
    s << "KTBNInference (k=" << _k_ << ", interface=" << interfaceSize() << ")\n";
    s << "  interventions: {";
    bool first = true;
    for (const auto& [name, val]: _interventions_) {
      s << (first ? "" : ", ") << "do(" << name << "=" << val << ")";
      first = false;
    }
    s << "}\n  observations: {";
    first = true;
    for (const auto& [name, like]: _observations_) {
      (void)like;
      s << (first ? "" : ", ") << name;
      first = false;
    }
    s << "}\n  targets: ";
    if (!_targeted_mode_) s << "<all bases>";
    else {
      s << "{";
      first = true;
      for (const auto& name: _targets_) {
        s << (first ? "" : ", ") << name;
        first = false;
      }
      s << "}";
    }
    s << "\n  state: " << (_done_ ? "computed" : "not computed yet") << "\n";
    return s.str();
  }

}   // namespace gum
