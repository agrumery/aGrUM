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
 * @brief Template implementation of gum::KTBNGenerator.
 *
 * @author Seth AGUILA & Anis KHACEF
 */

#pragma once

#include <cmath>
#include <utility>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/KTBN/generator/KTBNGenerator.h>

namespace gum {

  template < GUM_Numeric GUM_SCALAR >
  template < typename T >
  void KTBNGenerator< GUM_SCALAR >::_shuffle_(std::vector< T >& v) {
    for (std::size_t i = v.size(); i > 1; --i)
      std::swap(v[i - 1], v[randomValue(static_cast< Size >(i))]);
  }

  // ===========================================================================
  // Constructors / Destructor
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNGenerator< GUM_SCALAR >::KTBNGenerator(Size k,
                                             Size nbTemporal,
                                             Size nbAtemporal,
                                             Size maxArcs,
                                             Size maxModality) :
      _k_(k), _nbTemporal_(nbTemporal), _nbAtemporal_(nbAtemporal), _maxArcs_(maxArcs),
      _maxModality_(maxModality) {
    if (k == 0) GUM_ERROR(InvalidArgument, "KTBNGenerator: k must be >= 1.")
    if (maxModality < 2)
      GUM_ERROR(InvalidArgument, "KTBNGenerator: maxModality must be >= 2.")
    GUM_CONSTRUCTOR(KTBNGenerator)
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNGenerator< GUM_SCALAR >::~KTBNGenerator() {
    GUM_DESTRUCTOR(KTBNGenerator)
  }

  // ===========================================================================
  // Configuration
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNGenerator< GUM_SCALAR >& KTBNGenerator< GUM_SCALAR >::setDensity(double density) {
    if (density < 0.0 || density > 1.0)
      GUM_ERROR(OutOfBounds, "KTBNGenerator: density must lie in [0,1].")
    _density_ = density;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNGenerator< GUM_SCALAR >& KTBNGenerator< GUM_SCALAR >::setDomainRange(Size minModality,
                                                                          Size maxModality) {
    if (minModality < 2)
      GUM_ERROR(InvalidArgument, "KTBNGenerator: minModality must be >= 2.")
    if (maxModality < minModality)
      GUM_ERROR(InvalidArgument, "KTBNGenerator: maxModality must be >= minModality.")
    _minModality_ = minModality;
    _maxModality_ = maxModality;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNGenerator< GUM_SCALAR >& KTBNGenerator< GUM_SCALAR >::setMaxParents(Size maxParents) {
    _maxParents_ = maxParents;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNGenerator< GUM_SCALAR >& KTBNGenerator< GUM_SCALAR >::setGuaranteeOrder(bool on) {
    _guaranteeOrder_ = on;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNGenerator< GUM_SCALAR >& KTBNGenerator< GUM_SCALAR >::setGenerateCPTs(bool on) {
    _generateCPTs_ = on;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNGenerator< GUM_SCALAR >&
      KTBNGenerator< GUM_SCALAR >::setNamePrefixes(const std::string& temporal,
                                                   const std::string& atemporal) {
    if (temporal.empty() || atemporal.empty())
      GUM_ERROR(InvalidArgument, "KTBNGenerator: a name prefix cannot be empty.")
    if (temporal == atemporal)
      GUM_ERROR(InvalidArgument, "KTBNGenerator: the two name prefixes must differ.")
    _temporalPrefix_  = temporal;
    _atemporalPrefix_ = atemporal;
    return *this;
  }

  // ===========================================================================
  // Accessors
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  Size KTBNGenerator< GUM_SCALAR >::k() const {
    return _k_;
  }

  template < GUM_Numeric GUM_SCALAR >
  Size KTBNGenerator< GUM_SCALAR >::nbLegalArcs() const {
    const Size n = _nbTemporal_, m = _nbAtemporal_, k = _k_;
    // guarded against unsigned underflow: each term is 0 when its shape is degenerate
    const Size atemporalPairs = (m >= 2) ? m * (m - 1) / 2 : 0;   // atemporal -> atemporal
    const Size atemporalToAll = m * n * k;                        // atemporal -> temporal
    const Size slicePairs     = (k >= 2) ? k * (k - 1) / 2 : 0;
    const Size crossSlice     = n * n * slicePairs;               // temporal, lag >= 1
    const Size intraSlice     = ((n >= 2) ? n * (n - 1) / 2 : 0) * k;   // temporal, lag 0
    return atemporalPairs + atemporalToAll + crossSlice + intraSlice;
  }

  // ===========================================================================
  // Generation
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  std::vector< typename KTBNGenerator< GUM_SCALAR >::_Arc_ >
      KTBNGenerator< GUM_SCALAR >::_legalArcs_(const std::vector< std::string >& temporal,
                                               const std::vector< std::string >& atemporal,
                                               const std::vector< Size >&        tRank,
                                               const std::vector< Size >&        aRank) const {
    const int            k  = static_cast< int >(_k_);
    constexpr int        AT = KTBN< GUM_SCALAR >::ATEMPORAL;
    std::vector< _Arc_ > out;
    out.reserve(nbLegalArcs());

    // atemporal -> atemporal: only low rank to high rank, which keeps it acyclic
    for (std::size_t i = 0; i < atemporal.size(); ++i)
      for (std::size_t j = 0; j < atemporal.size(); ++j)
        if (aRank[i] < aRank[j]) out.push_back({atemporal[i], AT, atemporal[j], AT});

    // atemporal -> temporal, at any slice: never cyclic, since the k-DBN's own
    // rules already forbid a temporal variable from parenting an atemporal one.
    for (const auto& a: atemporal)
      for (const auto& b: temporal)
        for (int s = 0; s < k; ++s) out.push_back({a, AT, b, s});

    // temporal, lag >= 1: the slice index strictly increases, so never cyclic
    for (int s1 = 0; s1 < k; ++s1)
      for (int s2 = s1 + 1; s2 < k; ++s2)
        for (const auto& b1: temporal)
          for (const auto& b2: temporal) out.push_back({b1, s1, b2, s2});

    // temporal, lag 0: the other cycle-prone family, so rank-ordered as well
    for (int s = 0; s < k; ++s)
      for (std::size_t i = 0; i < temporal.size(); ++i)
        for (std::size_t j = 0; j < temporal.size(); ++j)
          if (tRank[i] < tRank[j]) out.push_back({temporal[i], s, temporal[j], s});

    return out;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNGenerator< GUM_SCALAR >::generateKTBN(KTBN< GUM_SCALAR >& out) {
    out = KTBN< GUM_SCALAR >(_k_);

    // ---- variables, with domain sizes drawn in [_minModality_, _maxModality_] ----
    const Size spread = _maxModality_ - _minModality_ + 1;
    std::vector< std::string > temporal, atemporal;
    temporal.reserve(_nbTemporal_);
    atemporal.reserve(_nbAtemporal_);

    for (Size i = 0; i < _nbTemporal_; ++i) {
      const std::string name = _temporalPrefix_ + std::to_string(i);
      out.add(LabelizedVariable(name, name, _minModality_ + randomValue(spread)), true);
      temporal.push_back(name);
    }
    for (Size i = 0; i < _nbAtemporal_; ++i) {
      const std::string name = _atemporalPrefix_ + std::to_string(i);
      out.add(LabelizedVariable(name, name, _minModality_ + randomValue(spread)), false);
      atemporal.push_back(name);
    }

    // ---- random ranks for the two cycle-prone arc families ----
    std::vector< Size > tPerm(_nbTemporal_), aPerm(_nbAtemporal_);
    for (Size i = 0; i < _nbTemporal_; ++i) tPerm[i] = i;
    for (Size i = 0; i < _nbAtemporal_; ++i) aPerm[i] = i;
    _shuffle_(tPerm);
    _shuffle_(aPerm);

    std::vector< Size > tRank(_nbTemporal_), aRank(_nbAtemporal_);
    for (Size i = 0; i < _nbTemporal_; ++i) tRank[tPerm[i]] = i;
    for (Size i = 0; i < _nbAtemporal_; ++i) aRank[aPerm[i]] = i;

    // ---- how many arcs to draw ----
    const Size legalCount = nbLegalArcs();
    Size       target     = (_maxArcs_ != 0)
                              ? _maxArcs_
                              : static_cast< Size >(std::llround(_density_ * double(legalCount)));
    if (target > legalCount) target = legalCount;

    Size added = 0;

    // ---- the guaranteed lag-(k-1) kernel arc ----
    // Slice 0 -> slice k-1 is the widest reach a k-slice template can express, so
    // this alone pins the model's effective order to k. Placed before the random
    // fill, and counted against the budget, so density still bounds the total.
    if (_guaranteeOrder_ && _k_ >= 2 && _nbTemporal_ >= 1) {
      const std::string& tail = temporal[randomValue(_nbTemporal_)];
      const std::string& head = temporal[randomValue(_nbTemporal_)];
      out.addArc(tail, 0, head, static_cast< int >(_k_) - 1);
      ++added;
    }

    // ---- fill with random legal arcs ----
    auto legal = _legalArcs_(temporal, atemporal, tRank, aRank);
    _shuffle_(legal);
    for (const auto& a: legal) {
      if (added >= target) break;
      if (out.existsArc(a.tailBase, a.tailSlice, a.headBase, a.headSlice)) continue;
      if (_maxParents_ != 0
          && static_cast< Size >(out.parents(a.headBase, a.headSlice).size()) >= _maxParents_)
        continue;
      out.addArc(a.tailBase, a.tailSlice, a.headBase, a.headSlice);
      ++added;
    }

    if (_generateCPTs_) out.generateCPTs();
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBN< GUM_SCALAR > KTBNGenerator< GUM_SCALAR >::generate() {
    KTBN< GUM_SCALAR > out(_k_);
    generateKTBN(out);
    return out;
  }

}   // namespace gum
