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
 * @brief Implementation of the KTBNDatabaseGenerator class.
 *
 * @author Seth AGUILA & Anis KHACEF
 */

#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <numeric>
#include <optional>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

#include <agrum/base/variables/IDiscretizedVariable.h>
#include <agrum/KTBN/database/KTBNDatabaseGenerator.h>

#include <agrum/base/core/utils_random.h>
#include <unordered_map>

namespace gum::learning {

  template < GUM_Numeric GUM_SCALAR >
  INLINE Size KTBNDatabaseGenerator< GUM_SCALAR >::nbVars() const {
    return _nbVars_;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNDatabaseGenerator< GUM_SCALAR >::KTBNDatabaseGenerator(const KTBN< GUM_SCALAR >& kdbn) :
      _template_(kdbn.toBN()), _k_(kdbn.k()) {
    GUM_CONSTRUCTOR(KTBNDatabaseGenerator)
    _build_(kdbn);
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNDatabaseGenerator< GUM_SCALAR >::~KTBNDatabaseGenerator() {
    GUM_DESTRUCTOR(KTBNDatabaseGenerator)
  }

  template < GUM_Numeric GUM_SCALAR >
  std::pair< std::string, int > KTBNDatabaseGenerator< GUM_SCALAR >::_decode_(
      const std::string&                       name,
      const std::unordered_set< std::string >& temporalSet) {
    if (!name.empty() && name.back() == ']') {
      const auto p = name.rfind('[');
      if (p != std::string::npos && p + 1 < name.size() - 1) {
        const std::string base   = name.substr(0, p);
        const std::string digits = name.substr(p + 1, name.size() - p - 2);
        if (temporalSet.contains(base)
            && std::all_of(digits.begin(), digits.end(), [](unsigned char c) {
                 return std::isdigit(c) != 0;
               }))
          return {base, std::stoi(digits)};
      }
    }
    return {name, KTBN< GUM_SCALAR >::ATEMPORAL};
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNDatabaseGenerator< GUM_SCALAR >::_build_(const KTBN< GUM_SCALAR >& kdbn) {
    const std::unordered_set< std::string >& temporalSet = kdbn.temporalVarNames();

    // 1) canonical column numbering: one column per base variable (temporal
    //    variables first, then atemporal ones).
    std::unordered_map< std::string, Idx > colByName;   // local: only needed during construction
    _nbVars_ = 0;
    for (const std::string& base: temporalSet) {
      _baseCols_.push_back(base);
      colByName.emplace(base, _nbVars_++);
    }
    for (const std::string& base: kdbn.atemporalVarNames()) {
      _baseCols_.push_back(base);
      colByName.emplace(base, _nbVars_++);
    }

    // 2) precompile every template node (and its parents) in topological order,
    //    so that drawSamples() never has to parse a name again. In the same pass we
    //    pick one representative variable per column (_vars_, for label rendering)
    //    and build the shared instantiation (_inst_, reused for every draw). All
    //    cached pointers refer to _template_ (our own copy), so they outlive the
    //    source k-DBN.
    _vars_.resize(_nbVars_);
    const int lastSlice = int(_k_) - 1;
    for (const NodeId n: _template_.topologicalOrder()) {
      const DiscreteVariable&     v   = _template_.variable(n);
      const Tensor< GUM_SCALAR >& cpt = _template_.cpt(n);
      const auto [base, slice]        = _decode_(v.name(), temporalSet);

      NodeRef nr;
      nr.var   = &v;
      nr.cpt   = &cpt;
      nr.slice = slice;
      nr.col   = colByName[base];

      for (Idx i = 1; i < cpt.nbrDim(); ++i) {
        const DiscreteVariable& pv = cpt.variable(i);
        const auto [pbase, pslice] = _decode_(pv.name(), temporalSet);
        const bool pAtemporal      = (pslice == KTBN< GUM_SCALAR >::ATEMPORAL);

        ParentRef pr;
        pr.var         = &pv;
        pr.col         = colByName[pbase];
        pr.isAtemporal = pAtemporal;
        pr.lag         = pAtemporal ? KTBN< GUM_SCALAR >::ATEMPORAL : slice - pslice;
        nr.parents.push_back(std::move(pr));
      }

      _vars_[nr.col] = &v;
      _inst_.add(v);
      if (slice == lastSlice) _kernel_.push_back(_nodes_.size());
      _nodes_.push_back(std::move(nr));
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  Idx KTBNDatabaseGenerator< GUM_SCALAR >::_drawVar_(const DiscreteVariable&     var,
                                                     const Tensor< GUM_SCALAR >& cpt,
                                                     double&                     log2likelihood) {
    const double threshold = gum::randomProba();
    double       cumulProb = 0.0;
    for (_inst_.setFirstVar(var); !_inst_.end(); _inst_.incVar(var)) {
      cumulProb += cpt[_inst_];
      if (cumulProb > threshold) break;
    }
    if (_inst_.end()) _inst_.setLastVar(var);
    log2likelihood += std::log2(cpt[_inst_]);
    return _inst_.val(var);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< double >
      KTBNDatabaseGenerator< GUM_SCALAR >::drawSamples(Size             nbSamples,
                                                       Size             nbTimeSlices,
                                                       std::string_view dirPath,
                                                       std::string_view csvBaseName,
                                                       VarOrderMode     mode,
                                                       bool             useLabels,
                                                       std::string      csvSeparator) {
    // fixed horizon: every trajectory shares nbTimeSlices (perTraj == nullptr)
    return _drawSamples_(nbSamples,
                         nbTimeSlices,
                         nullptr,
                         dirPath,
                         csvBaseName,
                         mode,
                         useLabels,
                         csvSeparator);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< double >
      KTBNDatabaseGenerator< GUM_SCALAR >::drawSamples(const std::vector< Size >& nbTimeSlices,
                                                       std::string_view           dirPath,
                                                       std::string_view           csvBaseName,
                                                       VarOrderMode               mode,
                                                       bool                       useLabels,
                                                       std::string                csvSeparator) {
    // per-trajectory horizons: nbTimeSlices[i] is trajectory i's length
    return _drawSamples_(nbTimeSlices.size(),
                         0,
                         &nbTimeSlices,
                         dirPath,
                         csvBaseName,
                         mode,
                         useLabels,
                         csvSeparator);
  }

  /// The single worker behind both public drawSamples() overloads. Trajectory i's
  /// horizon T is read from @p perTraj when given, else from the shared @p fixedLen.
  /// Each trajectory is sampled into a flat row-major buffer (T × _nbVars_, in
  /// canonical column order) in two phases and written straight to its own CSV:
  ///
  /// Phase 1 — initial k slices (0..k-1): no history yet, so every node of the
  /// k-slice template is drawn from scratch in topological order by inverse-CDF.
  ///
  /// Phase 2 — transition (slices k..T-1): only the slice-(k-1) nodes (the kernel,
  /// already topological) are drawn, each parent read from the row at time (t - lag).
  template < GUM_Numeric GUM_SCALAR >
  std::vector< double >
      KTBNDatabaseGenerator< GUM_SCALAR >::_drawSamples_(Size                       nbSamples,
                                                         Size                       fixedLen,
                                                         const std::vector< Size >* perTraj,
                                                         std::string_view           dirPath,
                                                         std::string_view           csvBaseName,
                                                         VarOrderMode               mode,
                                                         bool                       useLabels,
                                                         const std::string&         csvSeparator) {
    // horizon of trajectory i: from perTraj when given, else the shared fixedLen
    const auto lengthAt = [&](Idx i) { return perTraj ? (*perTraj)[i] : fixedLen; };

    // validate everything up front, before any file is created
    if (csvSeparator.find('\n') != std::string::npos)
      GUM_ERROR(InvalidArgument, "csvSeparator must not contain end-line characters")
    for (Idx i = 0; i < nbSamples; ++i)
      if (lengthAt(i) < _k_)
        GUM_ERROR(OperationNotAllowed, "nbTimeSlices=" << lengthAt(i) << " must be >= k=" << _k_)

    // decide the column order once (shared by every trajectory file):
    // colOrder[i] is the canonical column written at output position i.
    std::vector< Idx > colOrder;
    switch (mode) {
      case VarOrderMode::RANDOM : setVarOrderRandomized(colOrder); break;
      case VarOrderMode::TOPOLOGICAL : setVarOrderTopological(colOrder); break;
      case VarOrderMode::ANTI_TOPOLOGICAL : setVarOrderAntiTopological(colOrder); break;
      default : GUM_ERROR(InvalidArgument, "unknown VarOrderMode")
    }

    const std::filesystem::path dir{dirPath};
    const std::string           stem{csvBaseName};
    std::filesystem::create_directories(dir);   // ensure the destination exists

    std::vector< double > log2Ls;
    log2Ls.reserve(nbSamples);

    const bool             hasListener = onProgress.hasListener();
    std::optional< Timer > timer;
    int                    progress = 0;
    if (hasListener) {
      timer.emplace();
      GUM_EMIT2(onProgress, 0, 0.0);
    }

    // generate one trajectory at a time; each is sampled then written to its file
    for (std::size_t i = 0; i < nbSamples; ++i) {
      const Size T = lengthAt(i);

      // flat trajectory buffer (row-major) in canonical column order: the value at
      // time t, base column c is traj[t * _nbVars_ + c]. Sampling uses the cached
      // integer columns (nr.col / pr.col) — no name lookup in the hot loop.
      std::vector< Idx > traj(T * _nbVars_);
      double             log2L = 0;

      // Phase 1: draw the first k time steps from scratch (no history available yet)
      for (const NodeRef& nr: _nodes_) {
        for (const ParentRef& pr: nr.parents) {
          // an atemporal parent has the same value on every row, stored at row 0
          const Size parentRow = pr.isAtemporal ? 0 : nr.slice - pr.lag;
          _inst_.chgVal(*pr.var, traj[parentRow * _nbVars_ + pr.col]);
        }
        const Idx drawn = _drawVar_(*nr.var, *nr.cpt, log2L);
        if (nr.slice == KTBN< GUM_SCALAR >::ATEMPORAL)
          for (Size t = 0; t < T; ++t)
            traj[t * _nbVars_ + nr.col] = drawn;
        else traj[nr.slice * _nbVars_ + nr.col] = drawn;
      }

      // Phase 2: extend to T-1 by sliding the transition kernel forward one step at a time
      for (Size t = _k_; t < T; ++t) {
        for (const Idx kernelIdx: _kernel_) {
          const NodeRef& nr = _nodes_[kernelIdx];
          for (const ParentRef& pr: nr.parents) {
            const Size parentRow = pr.isAtemporal ? 0 : t - pr.lag;
            _inst_.chgVal(*pr.var, traj[parentRow * _nbVars_ + pr.col]);
          }
          traj[t * _nbVars_ + nr.col] = _drawVar_(*nr.var, *nr.cpt, log2L);
        }
      }

      log2Ls.push_back(log2L);
      const std::filesystem::path file = dir / (stem + std::to_string(i + 1) + ".csv");
      _writeTrajectory_(file.string(), traj, T, useLabels, csvSeparator, colOrder);

      if (hasListener) {
        const int p = int((i * 100) / nbSamples);
        if (p != progress) {
          progress = p;
          GUM_EMIT2(onProgress, progress, timer->step());
        }
      }
    }

    if (hasListener) {
      std::stringstream ss;
      ss << nbSamples << " trajectories generated in " << timer->step() << " s.";
      GUM_EMIT1(onStop, ss.str());
    }

    return log2Ls;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNDatabaseGenerator< GUM_SCALAR >::setDiscretizedLabelModeRandom() {
    _discretizedLabelMode_ = DiscretizedLabelMode::RANDOM;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNDatabaseGenerator< GUM_SCALAR >::setDiscretizedLabelModeMedian() {
    _discretizedLabelMode_ = DiscretizedLabelMode::MEDIAN;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNDatabaseGenerator< GUM_SCALAR >::setDiscretizedLabelModeInterval() {
    _discretizedLabelMode_ = DiscretizedLabelMode::INTERVAL;
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string KTBNDatabaseGenerator< GUM_SCALAR >::_label_(Idx col, Idx idx) const {
    const DiscreteVariable& v = *_vars_[col];
    if (v.varType() == VarType::DISCRETIZED) {
      switch (_discretizedLabelMode_) {
        case DiscretizedLabelMode::MEDIAN : return std::to_string(v.numerical(idx));
        case DiscretizedLabelMode::RANDOM :
          return std::to_string(static_cast< const IDiscretizedVariable& >(v).draw(idx));
        case DiscretizedLabelMode::INTERVAL : return v.label(idx);
        default : GUM_ERROR(FatalError, "unknown DiscretizedLabelMode")
      }
    }
    return v.label(idx);
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNDatabaseGenerator< GUM_SCALAR >::_writeTrajectory_(
      std::string_view          csvFileURL,
      const std::vector< Idx >& traj,
      Size                      nbTimeSlices,
      bool                      useLabels,
      const std::string&        csvSeparator,
      const std::vector< Idx >& colOrder) const {
    std::ofstream os(std::filesystem::path{csvFileURL}, std::ofstream::out);
    if (!os) GUM_ERROR(IOError, "could not open '" << csvFileURL << "' for writing")

    // header: one column per base variable, in the chosen output order
    bool firstCol = true;
    for (const Idx col: colOrder) {
      if (!firstCol) os << csvSeparator;
      os << _baseCols_[col];
      firstCol = false;
    }
    os << "\n";

    for (Size t = 0; t < nbTimeSlices; ++t) {
      const Size base = t * _nbVars_;
      firstCol        = true;
      for (const Idx col: colOrder) {
        if (!firstCol) os << csvSeparator;
        os << (useLabels ? _label_(col, traj[base + col]) : std::to_string(traj[base + col]));
        firstCol = false;
      }
      os << "\n";
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNDatabaseGenerator< GUM_SCALAR >::setVarOrderRandomized(
      std::vector< Idx >& colOrder) const {
    colOrder.resize(_nbVars_);
    std::iota(colOrder.begin(), colOrder.end(), 0);
    std::shuffle(colOrder.begin(), colOrder.end(), gum::randomGenerator());
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNDatabaseGenerator< GUM_SCALAR >::setVarOrderTopological(
      std::vector< Idx >& colOrder) const {
    // The column order is a topological sort of the "contemporaneous" DAG over
    // base columns. Only two kinds of arc constrain the column order:
    //   * atemporal -> atemporal   — orders the atemporal block
    //   * same-slice kernel arcs   — lag-0 arcs at slice k-1; order the temporal block
    // Lagged arcs are resolved at sampling time, not here; atemporal -> temporal
    // arcs need no edge since every atemporal column already precedes every
    // temporal one (the blocks are emitted in that order below).
    std::vector< std::vector< Idx > > children(_nbVars_);
    std::vector< int >                indegree(_nbVars_, 0);
    std::vector< bool >               isAtemporal(_nbVars_, false);

    auto addEdge = [&](Idx parent, Idx child) {
      children[parent].push_back(child);
      ++indegree[child];
    };

    // (1) build the contemporaneous DAG, one arc set per block.
    const int lastSlice = int(_k_) - 1;
    for (const NodeRef& nf: _nodes_) {
      if (nf.slice == KTBN< GUM_SCALAR >::ATEMPORAL) {
        isAtemporal[nf.col] = true;
        for (const ParentRef& pr: nf.parents) {
          if (!pr.isAtemporal)
            GUM_ERROR(FatalError,
                      "temporal variable '"
                          << _baseCols_[pr.col] << "' is a parent of atemporal variable '"
                          << _baseCols_[nf.col] << "' — violates the k-DBN invariant")
          addEdge(pr.col, nf.col);
        }
      } else if (nf.slice == lastSlice) {
        for (const ParentRef& pr: nf.parents)
          if (!pr.isAtemporal && pr.lag == 0) addEdge(pr.col, nf.col);
      }
    }

    // (2) Kahn's sort of one block, seeded in ascending col order so the result
    //     is deterministic (the seed is canonical and edges follow _nodes_).
    auto topoSortBlock = [&](bool atempBlock) {
      std::queue< Idx > q;
      for (Idx c = 0; c < _nbVars_; ++c)
        if (isAtemporal[c] == atempBlock && indegree[c] == 0) q.push(c);
      while (!q.empty()) {
        const Idx cur = q.front();
        q.pop();
        colOrder.push_back(cur);
        for (const Idx ch: children[cur])
          if (--indegree[ch] == 0) q.push(ch);
      }
    };

    // (3) atemporal block first, then the temporal kernel order.
    colOrder.clear();
    topoSortBlock(true);
    topoSortBlock(false);
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNDatabaseGenerator< GUM_SCALAR >::setVarOrderAntiTopological(
      std::vector< Idx >& colOrder) const {
    setVarOrderTopological(colOrder);
    std::reverse(colOrder.begin(), colOrder.end());
  }

}   // namespace gum::learning
