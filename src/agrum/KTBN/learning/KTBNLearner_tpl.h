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
 * @brief Implementation of the KTBNLearner class.
 *
 * @author Seth AGUILA & Anis KHACEF
 */

#include <filesystem>
#include <fstream>
#include <sstream>

#include <agrum/base/database/CSVParser.h>
#include <agrum/base/database/databaseTable.h>
#include <agrum/KTBN/learning/KTBNLearner.h>

namespace gum::learning {

  // =========================================================================
  // Internal-learner fan-out helpers
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  template < class F >
  void KTBNLearner< GUM_SCALAR >::_forEachLearner_(F&& f) {
    f(*_transitionLearner_);
    f(*_initialLearner_);
    if (_atemporalLearner_) f(*_atemporalLearner_);
  }

  template < GUM_Numeric GUM_SCALAR >
  template < class F >
  void KTBNLearner< GUM_SCALAR >::_forOwningLearner_(std::string_view tail,
                                                     std::string_view head,
                                                     F&&              f) {
    const int  last      = static_cast< int >(_prior_ktbn_.k()) - 1;
    const int  tailSlice = _determineNode_(std::string{tail}).second;
    const int  headSlice = _determineNode_(std::string{head}).second;
    const bool bothAtemp = (tailSlice == KTBN< GUM_SCALAR >::ATEMPORAL
                            && headSlice == KTBN< GUM_SCALAR >::ATEMPORAL);
    if (headSlice == last) f(*_transitionLearner_);
    else if (bothAtemp && _atemporalLearner_) f(*_atemporalLearner_);
    else if (tailSlice != last) f(*_initialLearner_);
    // else: the head is at a past slice but the tail sits on the kernel one,
    // i.e. a backward arc. No table can hold it -- the initial one has no
    // slice k-1 column -- and the k-TBN forbids it anyway, so there is
    // nothing to constrain. Silently nothing, as the old broadcast did
    // through its "tailSlice != last && headSlice != last" guard.
  }

  template < GUM_Numeric GUM_SCALAR >
  template < class F >
  void KTBNLearner< GUM_SCALAR >::_forEachAllSlicesPair_(std::string_view tailBase,
                                                         std::string_view headBase,
                                                         F&&              f) const {
    if (!_isKnownBase_(tailBase))
      GUM_ERROR(InvalidArgument,
                "unknown base variable '" << tailBase
                                          << "': it is not one of this learner's variables")
    if (!_isKnownBase_(headBase))
      GUM_ERROR(InvalidArgument,
                "unknown base variable '" << headBase
                                          << "': it is not one of this learner's variables")

    constexpr int AT        = KTBN< GUM_SCALAR >::ATEMPORAL;
    const int     k         = static_cast< int >(_prior_ktbn_.k());
    const bool    tailAtemp = _prior_ktbn_.atemporalVarNames().contains(std::string{tailBase});
    const bool    headAtemp = _prior_ktbn_.atemporalVarNames().contains(std::string{headBase});

    if (tailAtemp && headAtemp) {
      f(AT, AT);
    } else if (tailAtemp) {
      for (int hs = 0; hs < k; ++hs)
        f(AT, hs);
    } else if (headAtemp) {
      // temporal tail -> atemporal head is already structurally impossible
    } else {
      for (int ts = 0; ts < k; ++ts)
        for (int hs = ts; hs < k; ++hs)
          f(ts, hs);
    }
  }

  // =========================================================================
  // Constructors / Destructors
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >::KTBNLearner(
      std::string_view                         dirPath,
      std::string_view                         csvBaseName,
      Size                                     nbSamples,
      Size                                     k,
      const std::unordered_set< std::string >& atemporalVars,
      const std::vector< std::string >&        missingSymbols,
      bool                                     induceTypes,
      bool                                     ignoreMissingSymbols) :
      _ignoreMissingSymbols_(ignoreMissingSymbols),
      _prior_ktbn_(_buildPriorFromCSV_(dirPath, csvBaseName, k, atemporalVars, missingSymbols, induceTypes)) {
    // k >= 2 is already enforced by _buildPriorFromCSV_ during member initialisation
    if (nbSamples == 0) GUM_ERROR(InvalidArgument, "KTBNLearner needs at least one sample")
    GUM_CONSTRUCTOR(KTBNLearner)
    try {
      _build_(dirPath, csvBaseName, nbSamples, missingSymbols);
    } catch (const gum::UnknownLabelInDatabase&) {
      // Domains are inferred from trajectory 1 alone, so a variable whose full
      // domain is absent there fails once a later trajectory shows a new label.
      // Atemporal variables are the usual culprits: constant within a trajectory,
      // they reveal only one value per file. Re-throw with a KTBN-specific hint.
      GUM_DESTRUCTOR(KTBNLearner)
      GUM_ERROR(UnknownLabelInDatabase,
                "KTBNLearner CSV constructor: an unknown label was encountered while "
                "reading the trajectory CSVs. The variable domains are inferred from "
                "the first CSV alone, so any variable whose modalities are not all "
                "present in trajectory 1 will trigger this error (atemporal variables "
                "are especially prone: each trajectory holds a single constant value "
                "for them, so at most one label appears in trajectory 1). "
                "Use the BN-schema constructor "
                "KTBNLearner(dir, base, n, k, bn, atemporals) to supply the full "
                "variable domains explicitly.")
    } catch (...) {
      GUM_DESTRUCTOR(KTBNLearner)
      throw;
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >::KTBNLearner(
      std::string_view                  dirPath,
      std::string_view                  csvBaseName,
      Size                              nbSamples,
      Size                              k,
      const std::vector< std::string >& missingSymbols,
      bool                              induceTypes,
      bool                              ignoreMissingSymbols) :
      // _inferAtemporalVars_ checks k>=2 itself, before opening anything (see
      // its declaration) — this initialiser-list call runs before the
      // delegated-to constructor's own body, so that check cannot be left to
      // _buildPriorFromCSV_ the way the explicit constructor leaves it.
      // Delegates to the explicit-atemporalVars constructor for the rest
      // (including the UnknownLabelInDatabase hint); _inferAtemporalVars_
      // needs nbSamples, unlike _buildPriorFromCSV_, since one trajectory
      // alone cannot show that a value stays constant.
      KTBNLearner(dirPath, csvBaseName, nbSamples, k,
                  _inferAtemporalVars_(dirPath, csvBaseName, nbSamples, k, missingSymbols),
                  missingSymbols, induceTypes, ignoreMissingSymbols) {}

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >::KTBNLearner(
      std::string_view                         dirPath,
      std::string_view                         csvBaseName,
      Size                                     nbSamples,
      Size                                     k,
      const BayesNet< GUM_SCALAR >&            bn,
      const std::unordered_set< std::string >& atemporalVars,
      const std::vector< std::string >&        missingSymbols,
      bool                                     ignoreMissingSymbols) :
      _ignoreMissingSymbols_(ignoreMissingSymbols),
      _prior_ktbn_(_buildPriorFromBN_(k, bn, atemporalVars)) {
    // k >= 2 is already enforced by _buildPriorFromBN_ during member initialisation
    if (nbSamples == 0) GUM_ERROR(InvalidArgument, "KTBNLearner needs at least one sample")
    GUM_CONSTRUCTOR(KTBNLearner)
    try {
      _build_(dirPath, csvBaseName, nbSamples, missingSymbols);
    } catch (...) {
      GUM_DESTRUCTOR(KTBNLearner)
      throw;
    }
  }
    

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >::~KTBNLearner() {
    GUM_DESTRUCTOR(KTBNLearner)
  }

  // =========================================================================
  // Main learning methods
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBN< GUM_SCALAR > KTBNLearner< GUM_SCALAR >::learnKTBN() {
    // Honour the possible-edge whitelist for atemporal->atemporal arcs: when a
    // whitelist is active (>=1 possible edge with a temporal endpoint) but no
    // atemporal->atemporal edge was whitelisted, the atemporal learner has an
    // empty (hence unrestricted) list. Skip it entirely so no atemporal arc is
    // produced; _assemble_ then leaves the atemporal variables as roots.
    const bool suppressAtemporal =
        (_nbTemporalPossibleEdges_ > 0) && (_nbAtemporalPossibleEdges_ == 0);
    BayesNet< GUM_SCALAR > transitionBN = _transitionLearner_->learnBN();
    BayesNet< GUM_SCALAR > initialBN    = _initialLearner_->learnBN();
    BayesNet< GUM_SCALAR > atemporalBN  = (_atemporalLearner_ && !suppressAtemporal)
                                            ? _atemporalLearner_->learnBN()
                                            : BayesNet< GUM_SCALAR >{};
    return _assemble_(transitionBN, initialBN, atemporalBN);
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBN< GUM_SCALAR > KTBNLearner< GUM_SCALAR >::learnParameters(
      const KTBN< GUM_SCALAR >& structure,
       bool takeIntoAccountScore) {
    if (structure.k() != _prior_ktbn_.k())
      GUM_ERROR(InvalidArgument,
                "learnParameters: structure has k=" << structure.k()
                    << " but this learner was built with k=" << _prior_ktbn_.k())
    const int k = (int)_prior_ktbn_.k();

    // Build one DAG per internal table, in that table's own NodeId space, by
    // routing each of structure's (base, slice) arcs to the table that owns its
    // head: slice k-1 goes to the transition table, atemporal heads go to the
    // atemporal table (guaranteed atemporal-tailed too — a temporal variable can
    // never be a parent of an atemporal one), everything else (slices 0..k-2,
    // whether the tail is temporal or atemporal) goes to the initial table.
    DAG transitionDAG;
    DAG initialDAG;
    DAG atemporalDAG;

    const std::size_t nbTransNodes = _transitionLearner_->database().nbVariables();
    for (std::size_t id = 0; id < nbTransNodes; ++id)
      transitionDAG.addNodeWithId(NodeId(id));

    const std::size_t nbInitNodes = _initialLearner_->database().nbVariables();
    for (std::size_t id = 0; id < nbInitNodes; ++id)
      initialDAG.addNodeWithId(NodeId(id));

    if (_atemporalLearner_) {
      const std::size_t nbAtemNodes = _atemporalLearner_->database().nbVariables();
      for (std::size_t id = 0; id < nbAtemNodes; ++id)
        atemporalDAG.addNodeWithId(NodeId(id));
    }

    for (const auto& [tail, head] : structure.arcs()) {
      const auto& [tailBase, tailSlice] = tail;
      const auto& [headBase, headSlice] = head;
      const std::string tailName = _encode_(tailBase, tailSlice);
      const std::string headName = _encode_(headBase, headSlice);

      if (headSlice == k - 1) {
        transitionDAG.addArc(_transitionLearner_->idFromName(tailName),
                              _transitionLearner_->idFromName(headName));
      } else if (headSlice == KTBN< GUM_SCALAR >::ATEMPORAL) {
        if (_atemporalLearner_)
          atemporalDAG.addArc(_atemporalLearner_->idFromName(tailName),
                               _atemporalLearner_->idFromName(headName));
      } else {
        initialDAG.addArc(_initialLearner_->idFromName(tailName),
                           _initialLearner_->idFromName(headName));
      }
    }

    BayesNet< GUM_SCALAR > transitionBN
        = _transitionLearner_->learnParameters(transitionDAG, takeIntoAccountScore);
    BayesNet< GUM_SCALAR > initialBN
        = _initialLearner_->learnParameters(initialDAG, takeIntoAccountScore);
    BayesNet< GUM_SCALAR > atemporalBN
        = _atemporalLearner_ ? _atemporalLearner_->learnParameters(atemporalDAG, takeIntoAccountScore)
                              : BayesNet< GUM_SCALAR >{};

    return _assemble_(transitionBN, initialBN, atemporalBN);
  }

  // =========================================================================
  // Score selection
  // =========================================================================


  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::useScoreAIC() {
    _forEachLearner_([](BNLearner< GUM_SCALAR >& l) { l.useScoreAIC(); });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::useScoreBD() {
    _forEachLearner_([](BNLearner< GUM_SCALAR >& l) { l.useScoreBD(); });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::useScoreBDeu() {
    _forEachLearner_([](BNLearner< GUM_SCALAR >& l) { l.useScoreBDeu(); });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::useScoreBIC() {
    _forEachLearner_([](BNLearner< GUM_SCALAR >& l) { l.useScoreBIC(); });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::useScoreLog2Likelihood() {
    _forEachLearner_([](BNLearner< GUM_SCALAR >& l) { l.useScoreLog2Likelihood(); });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::useScoreMDL() {
    _forEachLearner_([](BNLearner< GUM_SCALAR >& l) { l.useScoreMDL(); });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNLearner< GUM_SCALAR >::useScorefNML() {
    _forEachLearner_([](BNLearner< GUM_SCALAR >& l) { l.useScorefNML(); });
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string KTBNLearner< GUM_SCALAR >::checkScorePriorCompatibility() const {
    // Every score/prior setter is applied uniformly to all internal learners, so
    // their configurations are identical: a single check on the transition learner
    // is representative of the whole KTBNLearner.
    return _transitionLearner_->checkScorePriorCompatibility();
  }

  // =========================================================================
  // Algorithm selection
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::useGreedyHillClimbing() {
    _forEachLearner_([](BNLearner< GUM_SCALAR >& l) { l.useGreedyHillClimbing(); });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::useExtendedGreedyHillClimbing() {
    _forEachLearner_([](BNLearner< GUM_SCALAR >& l) { l.useExtendedGreedyHillClimbing(); });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >&
      KTBNLearner< GUM_SCALAR >::useLocalSearchWithTabuList(Size tabu_size, Size nb_decrease) {
    _forEachLearner_(
        [&](BNLearner< GUM_SCALAR >& l) { l.useLocalSearchWithTabuList(tabu_size, nb_decrease); });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::useMIIC() {
    _forEachLearner_([](BNLearner< GUM_SCALAR >& l) { l.useMIIC(); });
    return *this;
  }

  // =========================================================================
  // MIIC correction
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::useNMLCorrection() {
    _forEachLearner_([](BNLearner< GUM_SCALAR >& l) { l.useNMLCorrection(); });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::useMDLCorrection() {
    _forEachLearner_([](BNLearner< GUM_SCALAR >& l) { l.useMDLCorrection(); });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::useNoCorrection() {
    _forEachLearner_([](BNLearner< GUM_SCALAR >& l) { l.useNoCorrection(); });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< std::pair< std::string, std::string > >
      KTBNLearner< GUM_SCALAR >::latentVariables() const {
    // Each learner reports latent arcs in its own NodeId space, so translate each
    // Arc to an (engine-name, engine-name) pair before merging. A pair among
    // slices 0..k-2 can be flagged by both the transition and the initial learner,
    // so deduplicate on a tab-joined key (engine names never contain a tab).
    // Propagates BNLearner's OperationNotAllowed if MIIC is not selected.
    std::vector< std::pair< std::string, std::string > > result;
    std::unordered_set< std::string >                    seen;

    auto collect = [&](const std::unique_ptr< BNLearner< GUM_SCALAR > >& learner) {
      if (!learner) return;
      for (const auto& arc : learner->latentVariables()) {
        std::string tail = learner->nameFromId(arc.tail());
        std::string head = learner->nameFromId(arc.head());
        if (seen.insert(tail + '\t' + head).second)
          result.emplace_back(std::move(tail), std::move(head));
      }
    };
    collect(_transitionLearner_);
    collect(_initialLearner_);
    collect(_atemporalLearner_);
    return result;
  }

  // =========================================================================
  // Prior selection
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::useSmoothingPrior(double weight) {
    _forEachLearner_([&](BNLearner< GUM_SCALAR >& l) { l.useSmoothingPrior(weight); });
    return *this;
  }

  // =========================================================================
  // Structural constraints
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::addForbiddenArc(std::string_view tailNode,
                                                                        std::string_view headNode) {
    _forOwningLearner_(tailNode, headNode, [&](BNLearner< GUM_SCALAR >& l) {
      l.addForbiddenArc(tailNode, headNode);
    });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::addForbiddenArc(std::string_view tailBase,
                                                                        int              tailSlice,
                                                                        std::string_view headBase,
                                                                        int headSlice) {
    // wrapper: encode (base, slice) -> engine name and delegate to the string overload
    return addForbiddenArc(_encode_(tailBase, tailSlice), _encode_(headBase, headSlice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::eraseForbiddenArc(std::string_view tailNode,
                                                                          std::string_view headNode) {
    // refuse to lift an invariant the k-TBN definition enforces (shared with addMandatoryArc)
    _checkArcTemporallyFeasible_(tailNode, headNode, "un-forbid the arc");

    _forOwningLearner_(tailNode, headNode, [&](BNLearner< GUM_SCALAR >& l) {
      l.eraseForbiddenArc(tailNode, headNode);
    });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::eraseForbiddenArc(std::string_view tailBase,
                                                                          int              tailSlice,
                                                                          std::string_view headBase,
                                                                          int headSlice) {
    return eraseForbiddenArc(_encode_(tailBase, tailSlice), _encode_(headBase, headSlice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::addMandatoryArc(std::string_view tailNode,
                                                                        std::string_view headNode) {
    // Unlike forbidding, a mandatory arc must be feasible: reject up front the arcs
    // the k-TBN can never contain (shared with eraseForbiddenArc) rather than
    // letting them crash later in the wrong learner. Kept here, not in
    // _forOwningLearner_: the check is deliberately asymmetric across the four
    // setters (see its declaration).
    _checkArcTemporallyFeasible_(tailNode, headNode, "force the mandatory arc");

    _forOwningLearner_(tailNode, headNode, [&](BNLearner< GUM_SCALAR >& l) {
      l.addMandatoryArc(tailNode, headNode);
    });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::addMandatoryArc(std::string_view tailBase,
                                                                        int              tailSlice,
                                                                        std::string_view headBase,
                                                                        int headSlice) {
    // wrapper: encode (base, slice) -> engine name and delegate to the string overload
    return addMandatoryArc(_encode_(tailBase, tailSlice), _encode_(headBase, headSlice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::eraseMandatoryArc(std::string_view tailNode,
                                                                          std::string_view headNode) {
    // no feasibility check: a backward-in-time arc can never have been added, so
    // erasing it is harmless -- it resolves to a no-op on the owning learner.
    _forOwningLearner_(tailNode, headNode, [&](BNLearner< GUM_SCALAR >& l) {
      l.eraseMandatoryArc(tailNode, headNode);
    });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::eraseMandatoryArc(std::string_view tailBase,
                                                                          int              tailSlice,
                                                                          std::string_view headBase,
                                                                          int headSlice) {
    return eraseMandatoryArc(_encode_(tailBase, tailSlice), _encode_(headBase, headSlice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >&KTBNLearner< GUM_SCALAR >::addForbiddenIntraSliceArc(std::string_view tailBase,
                                                           std::string_view headBase) {
    // validate both endpoints BEFORE touching any learner, so a rejected call
    // leaves no partially-applied constraint behind
    _checkBaseIsTemporal_(tailBase, "an intra-slice constraint");
    _checkBaseIsTemporal_(headBase, "an intra-slice constraint");
    const int k = static_cast< int >(_prior_ktbn_.k());
    for (int t = 0; t < k; ++t)
      addForbiddenArc(tailBase, t, headBase, t);
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >&
      KTBNLearner< GUM_SCALAR >::eraseForbiddenIntraSliceArc(std::string_view tailBase,
                                                             std::string_view headBase) {
    _checkBaseIsTemporal_(tailBase, "an intra-slice constraint");
    _checkBaseIsTemporal_(headBase, "an intra-slice constraint");
    const int k = static_cast< int >(_prior_ktbn_.k());
    for (int t = 0; t < k; ++t)
      eraseForbiddenArc(tailBase, t, headBase, t);
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >&
      KTBNLearner< GUM_SCALAR >::addForbiddenArcAllSlices(std::string_view tailBase,
                                                          std::string_view headBase) {
    _forEachAllSlicesPair_(tailBase, headBase, [&](int ts, int hs) {
      addForbiddenArc(tailBase, ts, headBase, hs);
    });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >&
      KTBNLearner< GUM_SCALAR >::eraseForbiddenArcAllSlices(std::string_view tailBase,
                                                            std::string_view headBase) {
    _forEachAllSlicesPair_(tailBase, headBase, [&](int ts, int hs) {
      eraseForbiddenArc(tailBase, ts, headBase, hs);
    });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::addNoParentNode(std::string_view base,
int              slice) {
    return addNoParentNode(_encode_(base, slice));                                                                        
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::addNoParentNode(
      std::string_view name) {
    const int  slice   = _determineNode_(std::string{name}).second;
    const bool isAtemp = (slice == KTBN< GUM_SCALAR >::ATEMPORAL);
    if (isAtemp && _atemporalLearner_) {
      // Atemporals are already forced roots in transition/initial learners by _build_.
      // The user constraint is only meaningful in the atemporal learner.
      _atemporalLearner_->addNoParentNode(name);
    } else if (!isAtemp) {
      _transitionLearner_->addNoParentNode(name);
      if (slice < (int)_prior_ktbn_.k() - 1)
        _initialLearner_->addNoParentNode(name);
    }
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::eraseNoParentNode(std::string_view base,
                                                                          int              slice) {
    return eraseNoParentNode(_encode_(base, slice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::eraseNoParentNode(
      std::string_view name) {
    const int  slice   = _determineNode_(std::string{name}).second;
    const int  last    = (int)_prior_ktbn_.k() - 1;
    const bool isAtemp = (slice == KTBN< GUM_SCALAR >::ATEMPORAL);
    if (isAtemp && _atemporalLearner_)
      _atemporalLearner_->eraseNoParentNode(name);
    else if (slice == last)
      _transitionLearner_->eraseNoParentNode(name);
    else if (!isAtemp)
      // Past slices keep their transition-learner root constraint (confines learning
      // to the kernel); only lift the constraint in the initial learner.
      _initialLearner_->eraseNoParentNode(name);
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::addNoChildrenNode(std::string_view base,
                                                                          int              slice) {
    return addNoChildrenNode(_encode_(base, slice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::addNoChildrenNode(
      std::string_view name) {
    const int  slice   = _determineNode_(std::string{name}).second;
    const bool isAtemp = (slice == KTBN< GUM_SCALAR >::ATEMPORAL);
    _transitionLearner_->addNoChildrenNode(name);
    if (slice < (int)_prior_ktbn_.k() - 1)
      _initialLearner_->addNoChildrenNode(name);
    if (isAtemp && _atemporalLearner_)
      _atemporalLearner_->addNoChildrenNode(name);
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::eraseNoChildrenNode(
      std::string_view base, int slice) {
    return eraseNoChildrenNode(_encode_(base, slice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::eraseNoChildrenNode(
      std::string_view name) {
    const int  slice   = _determineNode_(std::string{name}).second;
    const bool isAtemp = (slice == KTBN< GUM_SCALAR >::ATEMPORAL);
    _transitionLearner_->eraseNoChildrenNode(name);
    if (slice < (int)_prior_ktbn_.k() - 1)
      _initialLearner_->eraseNoChildrenNode(name);
    if (isAtemp && _atemporalLearner_)
      _atemporalLearner_->eraseNoChildrenNode(name);
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::addPossibleEdge(std::string_view tailBase,
                                                                        int              tailSlice,
                                                                        std::string_view headBase,
                                                                        int              headSlice) {
    // encode (base, slice) -> engine name and delegate to the string overload, which
    // owns the full routing (atemporal->atemporal to the atemporal learner, slice-k-1 guard)
    return addPossibleEdge(_encode_(tailBase, tailSlice), _encode_(headBase, headSlice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::erasePossibleEdge(std::string_view tailBase,
                                                                          int              tailSlice,
                                                                          std::string_view headBase,
                                                                          int              headSlice) {
    return erasePossibleEdge(_encode_(tailBase, tailSlice), _encode_(headBase, headSlice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::addPossibleEdge(std::string_view tail,
                                                                        std::string_view head) {
    const int last      = (int)_prior_ktbn_.k() - 1;
    const int tailSlice = _determineNode_(std::string{tail}).second;
    const int headSlice = _determineNode_(std::string{head}).second;

    // Atemporal names exist as forced roots in every table, so whitelisting them
    // here too keeps the whole-model promise ("only listed edges are explored")
    // even when the edge can fire in just one learner: the no-parent constraint
    // on atemporal heads still vetoes it there (constraints compound, never
    // override), so this can only shrink the candidate set, never widen it.
    _transitionLearner_->addPossibleEdge(tail, head);
    // forward to the initial learner only when both endpoints exist there, i.e. both
    // are at a past slice (< k-1); atemporals (slice -1) satisfy this automatically.
    if (tailSlice < last && headSlice < last) _initialLearner_->addPossibleEdge(tail, head);

    if (tailSlice == KTBN< GUM_SCALAR >::ATEMPORAL && headSlice == KTBN< GUM_SCALAR >::ATEMPORAL) {
      ++_nbAtemporalPossibleEdges_;
      if (_atemporalLearner_) _atemporalLearner_->addPossibleEdge(tail, head);
    } else {
      ++_nbTemporalPossibleEdges_;
    }
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::erasePossibleEdge(std::string_view tail,
                                                                          std::string_view head) {
    const int last      = (int)_prior_ktbn_.k() - 1;
    const int tailSlice = _determineNode_(std::string{tail}).second;
    const int headSlice = _determineNode_(std::string{head}).second;

    _transitionLearner_->erasePossibleEdge(tail, head);
    if (tailSlice < last && headSlice < last) _initialLearner_->erasePossibleEdge(tail, head);

    if (tailSlice == KTBN< GUM_SCALAR >::ATEMPORAL && headSlice == KTBN< GUM_SCALAR >::ATEMPORAL) {
      if (_nbAtemporalPossibleEdges_ > 0) --_nbAtemporalPossibleEdges_;
      if (_atemporalLearner_) _atemporalLearner_->erasePossibleEdge(tail, head);
    } else {
      if (_nbTemporalPossibleEdges_ > 0) --_nbTemporalPossibleEdges_;
    }
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::allowArcAdditions(bool allow) {
    _forEachLearner_([&](BNLearner< GUM_SCALAR >& l) { l.allowArcAdditions(allow); });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::allowArcDeletions(bool allow) {
    _forEachLearner_([&](BNLearner< GUM_SCALAR >& l) { l.allowArcDeletions(allow); });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::allowArcReversals(bool allow) {
    _forEachLearner_([&](BNLearner< GUM_SCALAR >& l) { l.allowArcReversals(allow); });
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNLearner< GUM_SCALAR >& KTBNLearner< GUM_SCALAR >::setMaxIndegree(Size max_indegree) {
    _forEachLearner_([&](BNLearner< GUM_SCALAR >& l) { l.setMaxIndegree(max_indegree); });
    return *this;
  }

  // =========================================================================
  // Diagnostics
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  Size KTBNLearner< GUM_SCALAR >::k() const {
    return _prior_ktbn_.k();
  }


  template < GUM_Numeric GUM_SCALAR >
  Size KTBNLearner< GUM_SCALAR >::nbCols() const {
    return _prior_ktbn_.nbTemporalVars() + _prior_ktbn_.nbAtemporalVars();
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< Size > KTBNLearner< GUM_SCALAR >::nbRows() const {
    // Raw trajectory lengths captured by _build_(), one entry per sample. Unlike
    // BNLearner::nbRows() (a single flat-table row count), a trajectory learner
    // has one length per sequence, so the per-sample vector is the natural analog.
    return _nbTimeSlices_;
  }

  template < GUM_Numeric GUM_SCALAR >
  bool KTBNLearner< GUM_SCALAR >::isConstraintBased() const {
    return _transitionLearner_->isConstraintBased();
  }

  template < GUM_Numeric GUM_SCALAR >
  bool KTBNLearner< GUM_SCALAR >::isScoreBased() const {
    return _transitionLearner_->isScoreBased();
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string KTBNLearner< GUM_SCALAR >::toString() const {
    // Emit a k-TBN-specific header, then delegate score/algo/prior/constraint
    // details to each internal learner's toString().
    std::stringstream s;
    s << "k                 : " << k() << '\n';
    s << "Variables         : " << nbCols() << " (" << _prior_ktbn_.nbTemporalVars()
      << " temporal, " << _prior_ktbn_.nbAtemporalVars() << " atemporal)" << '\n';
    s << "Transition rows   : " << _transitionLearner_->nbRows() << '\n';
    s << "Initial rows      : " << _initialLearner_->nbRows() << '\n';
    s << '\n';
    s << "=== Transition learner (arcs into slice " << (k() - 1) << ") ===" << '\n';
    s << _transitionLearner_->toString();
    s << '\n';
    s << "=== Initial learner (slices 0.." << (k() - 2) << ") ===" << '\n';
    s << _initialLearner_->toString();
    if (_atemporalLearner_) {
      s << '\n';
      s << "=== Atemporal learner (atemporal->atemporal arcs) ===" << '\n';
      s << _atemporalLearner_->toString();
    }
    return s.str();
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< std::tuple< std::string, std::string, std::string > >
      KTBNLearner< GUM_SCALAR >::state() const {
    // settings are applied identically to every internal learner, so the
    // transition learner's state represents the whole KTBNLearner.
    auto result = _transitionLearner_->state();

    // The transition learner lists every slice of every temporal variable
    // (e.g. "W[0][2], W[1][2], W[2][2], W[3][2]"). Replace that row with one
    // entry per base variable (e.g. "W[2]") built directly from the KTBN's own
    // variable sets — no string parsing, robust to any base name.
    for (auto& [key, val, comment] : result) {
      if (key != "Variables") continue;
      // Build base -> domainSize from the prior KTBN (slice 0 for temporal, AT for atemporal).
      std::string collapsed;
      bool        first = true;
      auto        emit  = [&](const std::string& base, int slice) {
        const auto& var  = _prior_ktbn_.variable(base, slice);
        if (!first) collapsed += ", ";
        collapsed += base + "[" + std::to_string(var.domainSize()) + "]";
        first = false;
      };
      for (const auto& base : _prior_ktbn_.temporalVarNames())  emit(base, 0);
      for (const auto& base : _prior_ktbn_.atemporalVarNames()) emit(base, KTBN< GUM_SCALAR >::ATEMPORAL);
      val = collapsed;
      break;
    }
    return result;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNLearner< GUM_SCALAR >::copyState(const KTBNLearner< GUM_SCALAR >& learner) {
    // As in BNLearner::copyState, only score/algorithm/prior/constraint settings
    // are copied, never the database. Both sides must be structurally compatible
    // (same k, same temporal/atemporal partition) for the name-based constraints
    // to mean the same thing. The atemporal learner is copied only when both
    // sides have one (absent when there are <= 1 atemporal vars).
    _transitionLearner_->copyState(*learner._transitionLearner_);
    _initialLearner_->copyState(*learner._initialLearner_);
    if (_atemporalLearner_ && learner._atemporalLearner_)
      _atemporalLearner_->copyState(*learner._atemporalLearner_);

    // KTBNLearner-only state (BNLearner::copyState can't carry it): learnKTBN reads
    // these to suppress the atemporal learner under a temporal-only whitelist
    _nbTemporalPossibleEdges_  = learner._nbTemporalPossibleEdges_;
    _nbAtemporalPossibleEdges_ = learner._nbAtemporalPossibleEdges_;
  }

  // =========================================================================
  // Database accessors
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  Size KTBNLearner< GUM_SCALAR >::nbSamples() const {
    // the initial table holds exactly one row per trajectory, so its row count
    // is the number of trajectory files given to the constructor.
    return _initialLearner_->nbRows();
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE bool KTBNLearner< GUM_SCALAR >::isIgnoringMissingSymbols() const {
    return _ignoreMissingSymbols_;
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE Size KTBNLearner< GUM_SCALAR >::nbDroppedRows() const {
    return _nbDroppedRows_;
  }

  template < GUM_Numeric GUM_SCALAR >
  bool KTBNLearner< GUM_SCALAR >::hasMissingValues() const {
    return _transitionLearner_->hasMissingValues() || _initialLearner_->hasMissingValues()
        || (_atemporalLearner_ && _atemporalLearner_->hasMissingValues());
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< std::string > KTBNLearner< GUM_SCALAR >::names() const {
    // The first nbCols() columns of the transition table are the base variables
    // (slice-0 temporals then atemporals); the remaining columns are just later
    // slices of those same temporals. So strip the slice suffix off the first
    // nbCols() engine names to get each base variable exactly once.
    const auto& engine = _transitionLearner_->names();
    const Size  n      = nbCols();
    std::vector< std::string > result;
    result.reserve(n);
    for (Size i = 0; i < n; ++i)
      result.push_back(_determineNode_(engine[i]).first);
    return result;
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< Size > KTBNLearner< GUM_SCALAR >::domainSizes() const {
    // Mirror names(): the first nbCols() columns are the base variables (a temporal
    // variable's domain size is the same across all its slices), so keep only those.
    const auto& all = _transitionLearner_->domainSizes();
    return std::vector< Size >(all.begin(), all.begin() + nbCols());
  }

  template < GUM_Numeric GUM_SCALAR >
  Size KTBNLearner< GUM_SCALAR >::domainSize(std::string_view base) const {
    // Resolve the base name to an engine name in the transition table: a temporal
    // variable is addressed at slice 0, everything else keeps its bare name (which
    // also lets a raw engine name pass through). An unknown name reaches
    // domainSize() unchanged and throws MissingVariableInDatabase.
    const std::string b{base};
    const int slice = _prior_ktbn_.temporalVarNames().contains(b) ? 0
                                                                  : KTBN< GUM_SCALAR >::ATEMPORAL;
    return _transitionLearner_->domainSize(_encode_(b, slice));
  }

  // =========================================================================
  // Private helpers
  // =========================================================================


  template < GUM_Numeric GUM_SCALAR >
  std::unordered_set< std::string > KTBNLearner< GUM_SCALAR >::_inferAtemporalVars_(
      std::string_view                  dirPath,
      std::string_view                  csvBaseName,
      Size                              nbSamples,
      Size                              k,
      const std::vector< std::string >& missingSymbols) {
    IKTBNLearner< GUM_SCALAR >::_checkMinimalOrder_(k, "k");
    return IKTBNLearner< GUM_SCALAR >::_scanConstantColumns_(dirPath, csvBaseName, nbSamples,
                                                              missingSymbols);
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBN< GUM_SCALAR > KTBNLearner< GUM_SCALAR >::_buildPriorFromCSV_(
      std::string_view                         dirPath,
      std::string_view                         csvBaseName,
      Size                                     k,
      const std::unordered_set< std::string >& atemporalVars,
      const std::vector< std::string >&        missingSymbols,
      bool                                     induceTypes) {
    IKTBNLearner< GUM_SCALAR >::_checkMinimalOrder_(k, "k");

    namespace fs = std::filesystem;
    const std::string firstCSV =
        (fs::path{dirPath} / (std::string{csvBaseName} + "1.csv")).string();
    // _build_() also reads trajectory 1 (its i=0 pass). The double-read is
    // unavoidable: this function runs in the member-initialiser list, before
    // the object (and hence _build_()) exists, and _build_() must read every
    // trajectory.

    // BNLearner construction runs induceTypes on the CSV and populates all
    // column translators with properly-typed DiscreteVariables.
    const BNLearner< GUM_SCALAR > tmpLearner(firstCSV, missingSymbols, induceTypes);

    // Pull typed variables from the translator set. translatorSafe(i) bounds-checks,
    // guarding against any names()/translator-set mismatch.
    const DBTranslatorSet&            translators = tmpLearner.database().translatorSet();
    const std::vector< std::string >& names       = tmpLearner.names();

    KTBN< GUM_SCALAR > prior(k);
    for (std::size_t i = 0; i < names.size(); ++i) {
      prior.add(static_cast< const DiscreteVariable& >(*translators.translatorSafe(i).variable()),
                !atemporalVars.contains(names[i]));
    }

    // Every declared atemporal name must appear in the CSV header.
    for (const std::string& aname : atemporalVars)
      if (!prior.exists(aname))
        GUM_ERROR(InvalidArgument,
                  "atemporal variable '" << aname << "' not found in the CSV header")
    return prior;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBN< GUM_SCALAR > KTBNLearner< GUM_SCALAR >::_buildPriorFromBN_(
      Size                                     k,
      const BayesNet< GUM_SCALAR >&            bn,
      const std::unordered_set< std::string >& atemporalVars) {
    IKTBNLearner< GUM_SCALAR >::_checkMinimalOrder_(k, "k");

    KTBN< GUM_SCALAR > prior(k);
    // bn.nodes() iterates in hash order (unspecified). This is harmless because
    // KTBN looks up variables by name, not by insertion index.
    for (const NodeId node : bn.nodes()) {
      const DiscreteVariable& var = bn.variable(node);
      prior.add(var, !atemporalVars.contains(var.name()));
    }
    for (const std::string& aname : atemporalVars)
      if (!prior.exists(aname))
        GUM_ERROR(InvalidArgument,
                  "atemporal variable '" << aname << "' not found in the BN")
    return prior;
  }


  template < GUM_Numeric GUM_SCALAR >
  void KTBNLearner< GUM_SCALAR >::_build_(std::string_view                  dirPath,
                                          std::string_view                  csvBaseName,
                                          Size                              nbSamples,
                                          const std::vector< std::string >& missingSymbols)
  {
    Size k = _prior_ktbn_.k();
    Size nbTempVars = _prior_ktbn_.temporalVarNames().size();
    Size nbAtempVars = _prior_ktbn_.atemporalVarNames().size();

    // tables are default-constructed here; translators are inserted per-column
    // in the i==0 block once the header (column order) is known
    DatabaseTable transitionTable(missingSymbols);
    DatabaseTable initTable(missingSymbols);
    DatabaseTable atemporalTable(missingSymbols);

    // Complete-case selection, at row granularity.
    //
    // aGrUM's structure learning refuses a database holding any missing value
    // outright (IBNLearner::learnDag_), so an incomplete row cannot simply be
    // handed over: learnKTBN() would throw on any trajectory with a gap. A row
    // that carries one is therefore dropped here, and everything downstream sees
    // a database with no missing value at all.
    //
    // This mirrors what the cross-k score already does per instance
    // (KTBNAdaptiveLearner::_forEachScoredNode_ skips any instance whose family
    // is not fully observed), so the two layers agree on which data counts.
    //
    // The unit differs per table: a transition row is a whole width-k window, so
    // one gap costs up to k windows; an initial or atemporal row is the whole
    // trajectory's contribution to that table.
    const std::unordered_set< std::string > missingSet(missingSymbols.begin(),
                                                       missingSymbols.end());
    const auto insertIfComplete = [&](DatabaseTable& table, const std::vector< std::string >& r) {
      if (_ignoreMissingSymbols_) {
        for (const auto& cell: r)
          if (missingSet.contains(cell)) {
            ++_nbDroppedRows_;
            return;
          }
      }
      table.insertRow(r);
    };

    const std::filesystem::path dir{dirPath};
    const std::string           stem{csvBaseName};

    const std::size_t transRowSize = nbAtempVars + nbTempVars * k;
    const std::size_t initRowSize  = nbAtempVars + nbTempVars * (k - 1);
    const std::size_t atempRowSize = nbAtempVars;

    std::vector<std::string> header;        // column order, captured from trajectory 1
    std::unordered_set<Size> atemVarsCols;  // atemporal column indices

    // reused across trajectories (capacity is kept between iterations)
    std::vector<std::vector<std::string>> buffer;
    std::vector<std::string>              row;
    row.reserve(transRowSize);
    _nbTimeSlices_.reserve(nbSamples);   // one raw length per trajectory (see nbRows())

    for (Size i=0;  i<nbSamples; ++i){

      // open the i-th trajectory (each file is opened exactly once)
      const std::filesystem::path file = dir / (stem + std::to_string(i+1) + ".csv");
      std::ifstream is(file, std::ifstream::in);
      if (!is.is_open()) GUM_ERROR(gum::IOError, "Cannot open " << file.string());

      CSVParser parser(is, file.string());
      parser.next();

      if (i == 0){
        // first file only: capture the column order, derive the atemporal columns
        // from _prior_ktbn_, build the bracket-encoded names and finish setting up the
        // tables (must happen before any insertRow)
        const auto& rawHeader = parser.current();
        header.assign(rawHeader.begin(), rawHeader.end());
        for (std::size_t c = 0; c < header.size(); ++c)
          if (_prior_ktbn_.atemporalVarNames().contains(header[c]))
            atemVarsCols.insert(c);

        // every schema variable must appear in the data, else _assemble_ (used
        // by both learnKTBN and learnParameters) would later fail with an
        // opaque NotFound when resolving bracket names
        {
          const std::unordered_set< std::string > headerSet(header.begin(), header.end());
          auto requirePresent = [&](const std::string& base) {
            if (!headerSet.contains(base))
              GUM_ERROR(MissingVariableInDatabase,
                        "schema variable '" << base << "' is absent from '" << file.string() << "'")
          };
          for (const auto& base : _prior_ktbn_.temporalVarNames())  requirePresent(base);
          for (const auto& base : _prior_ktbn_.atemporalVarNames()) requirePresent(base);

          // conversely, every CSV column must be a known schema variable, else the
          // translator-insertion loop below would fail with an opaque NotFound
          // when resolving it against _prior_ktbn_
          for (const std::string& col : header)
            if (!_prior_ktbn_.exists(col))
              GUM_ERROR(MissingVariableInDatabase,
                        "CSV column '" << col << "' in '" << file.string()
                            << "' is not declared as a variable of this KTBNLearner")
        }

        // insert one translator per column into each table; every schema variable
        // has a concrete domain (user-supplied, or discovered by _buildPriorFromCSV_), so
        // insertTranslator picks the matching translator type from the variable.
        std::vector<std::string> varNamesTran;
        std::vector<std::string> varNamesInit;
        std::vector<std::string> varNamesAtemp;
        {
          auto insertTrans = [&](DatabaseTable& table, const std::string& base,
                                 int slice, std::size_t col) {
            table.insertTranslator(_prior_ktbn_.variable(base, slice), col, missingSymbols);
          };

          // translators and their engine names are built in lockstep, column by
          // column, so a translator's table position and its name can never drift
          // apart (unlike keeping two separately-indexed passes in sync by hand).
          varNamesTran.reserve(transRowSize);
          varNamesInit.reserve(initRowSize);
          varNamesAtemp.reserve(atempRowSize);

          std::size_t tcol = 0, icol = 0, acol = 0;
          for (std::size_t c = 0; c < header.size(); ++c) {
            const int slice = atemVarsCols.contains(c)
                                ? KTBN< GUM_SCALAR >::ATEMPORAL : 0;
            insertTrans(transitionTable, header[c], slice, tcol++);
            varNamesTran.push_back(_encode_(header[c], slice));
            insertTrans(initTable, header[c], slice, icol++);
            varNamesInit.push_back(_encode_(header[c], slice));
            if (atemVarsCols.contains(c)) {
              insertTrans(atemporalTable, header[c], KTBN< GUM_SCALAR >::ATEMPORAL, acol++);
              varNamesAtemp.push_back(header[c]);
            }
          }
          for (Size slice = 1; slice < k - 1; ++slice)
            for (std::size_t c = 0; c < header.size(); ++c)
              if (!atemVarsCols.contains(c)) {
                insertTrans(initTable, header[c], (int)slice, icol++);
                varNamesInit.push_back(_encode_(header[c], (int)slice));
              }
          for (Size slice = 1; slice < k; ++slice)
            for (std::size_t c = 0; c < header.size(); ++c)
              if (!atemVarsCols.contains(c)) {
                insertTrans(transitionTable, header[c], (int)slice, tcol++);
                varNamesTran.push_back(_encode_(header[c], (int)slice));
              }
        }

        transitionTable.setVariableNames(varNamesTran, false);
        initTable.setVariableNames(varNamesInit, false);
        atemporalTable.setVariableNames(varNamesAtemp, false);
      } else {
        // later files: validate the header against trajectory 1's without copying it
        const auto& raw  = parser.current();
        bool        same = (raw.size() == header.size());
        for (std::size_t c = 0; same && c < header.size(); ++c)
          same = (raw[c] == header[c]);
        if (!same)
          GUM_ERROR(InvalidArgument,
                    "Header of " << file.string() << " differs from trajectory 1");
      }

      buffer.clear();
      while (parser.next()) {
        const auto& tokens = parser.current();
        if (tokens.size() != header.size())
          GUM_ERROR(InvalidArgument,
                    "Trajectory " << (i + 1) << ", row " << parser.nbLine()
                    << ": expected " << header.size() << " columns, got "
                    << tokens.size());
        buffer.push_back({tokens.begin(), tokens.end()});
      }

      if (buffer.size() < k)
        GUM_ERROR(OperationNotAllowed,
                  "Trajectory " << (i + 1) << " has " << buffer.size()
                  << " time steps but at least k=" << k << " are required");

      // record this trajectory's raw length (number of time steps), exposed by nbRows()
      _nbTimeSlices_.push_back(buffer.size());

      // An atemporal column is constant down the trajectory, so any row carries
      // its value -- but row 0's may be the missing one, and reading row 0 blindly
      // would then drop every row this trajectory feeds. Resolve each once, from
      // its first non-missing occurrence. Same rule the score walk applies.
      std::unordered_map< Size, std::string > atempValue;
      for (const Size col: atemVarsCols)
        for (const auto& r: buffer)
          if (!missingSet.contains(r[col])) {
            atempValue[col] = r[col];
            break;
          }

      // value of column col at time tt: the resolved constant for an atemporal
      // column, the row's own cell for a temporal one. A column missing all the
      // way down has no resolved value, so row 0's marker stands and the row is
      // dropped like any other incomplete one.
      const auto cellAt = [&](std::size_t tt, Size col) -> const std::string& {
        if (!atemVarsCols.contains(col)) return buffer[tt][col];
        const auto it = atempValue.find(col);
        return (it == atempValue.end()) ? buffer[0][col] : it->second;
      };

      // transition table: sliding windows of width k
      for (std::size_t t = 0; t + k <= buffer.size(); ++t) {
        row.clear();
        // slice 0 carries the atemporal columns; later slices skip them so atemporal
        // variables aren't repeated k-1 times in each row
        for(Size col = 0; col < buffer[0].size(); ++col){
          row.push_back(cellAt(t, col));
        }
        for (Size slice = 1; slice < k; ++slice){
          for(Size col = 0; col < buffer[0].size(); ++col){
            if (!atemVarsCols.contains(col)){
              row.push_back(buffer[t+slice][col]);
            }
          }
        }
        insertIfComplete(transitionTable, row);
      }

      // initial table: first k-1 slices, one row per trajectory
      row.clear();
      for(Size col = 0; col < buffer[0].size(); ++col){
        row.push_back(cellAt(0, col));
      }
      for (Size slice = 1; slice < k-1; ++slice){
        for(Size col = 0; col < buffer[0].size(); ++col){
          if (!atemVarsCols.contains(col)){
            row.push_back(buffer[slice][col]);
          }
        }
      }
      insertIfComplete(initTable, row);

      // atemporal table: one row per trajectory, atemporal columns only. Their
      // value is constant across the trajectory, so any time step works — take 0.
      // Skipped when fewer than 2 atemporal variables exist: a single atemporal
      // variable has no possible atemporal->atemporal arcs, so no learner is built.
      if (nbAtempVars > 1) {
        row.clear();
        for (Size col = 0; col < buffer[0].size(); ++col)
          if (atemVarsCols.contains(col)) row.push_back(cellAt(0, col));
        insertIfComplete(atemporalTable, row);
      }
    }

    // Dropping incomplete rows can empty a table outright -- every transition
    // window straddling a gap, or every trajectory's initial block incomplete.
    // The internal learner would then fail obscurely on an empty database, so
    // say what actually happened.
    if (_ignoreMissingSymbols_ && (transitionTable.nbRows() == 0 || initTable.nbRows() == 0))
      GUM_ERROR(OperationNotAllowed,
                "every row was dropped as incomplete (" << _nbDroppedRows_
                    << " in total): no fully observed transition window (or initial block) is left "
                       "to learn from. The trajectories are too sparsely observed for k=" << k << ".")

    // Variables are already typed upstream (template / first-trajectory learner),
    // so no induceTypes pass is needed here — just canonicalize the value codes.
    transitionTable.reorder();
    initTable.reorder();

    _transitionLearner_ = std::make_unique<BNLearner<GUM_SCALAR>>(transitionTable);
    _initialLearner_ = std::make_unique<BNLearner<GUM_SCALAR>>(initTable);

    if (nbAtempVars > 1) {
      atemporalTable.reorder();
      _atemporalLearner_ = std::make_unique<BNLearner<GUM_SCALAR>>(atemporalTable);
    }


    // Impose k-TBN temporal constraints on structure learning:
    //  - transition learner: past slices (0..k-2) are forced roots so only the
    //    present slice (k-1) receives new arcs.
    //  - atemporal variables are forced roots in transition/initial learners so
    //    their mutual structure is learned exclusively by the atemporal learner.
    //    No-parent in both learners covers every algorithm and also bans
    //    temporal→atemporal arcs.
    //  - initial learner: backward temporal arcs among past slices forbidden
    //    explicitly (honoured by MIIC and score-based algorithms alike).
    // These constraints shape structure search in learnKTBN(); they are inert
    // for learnParameters.
    const int   ki            = (int)k;
    const auto& temporalVars  = _prior_ktbn_.temporalVarNames();
    const auto& atemporalVars = _prior_ktbn_.atemporalVarNames();

    for (const auto& base : temporalVars)
      for (int slice = 0; slice < ki - 1; ++slice)
        _transitionLearner_->addNoParentNode(_encode_(base, slice));
    
    for (const auto& atemBase : atemporalVars){
      _transitionLearner_->addNoParentNode(atemBase);
      _initialLearner_->addNoParentNode(atemBase);
    }

    if (k > 2) {
      // backward-in-time arcs among the past slices 0..k-2 are forbidden
      // explicitly: this is the only form MIIC honours (it ignores slice order),
      // and score-based algorithms respect it too, so it fully covers the
      // constraint. A setSliceOrder() mirror was dropped here as redundant.
      for (const auto& tailBase : temporalVars)
        for (const auto& headBase : temporalVars)
          for (int tailSlice = 1; tailSlice < ki - 1; ++tailSlice)
            for (int headSlice = 0; headSlice < tailSlice; ++headSlice)
              _initialLearner_->addForbiddenArc(_encode_(tailBase, tailSlice),
                                               _encode_(headBase, headSlice));
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  const std::unordered_set< std::string >&
      KTBNLearner< GUM_SCALAR >::_atemporalVarNames_() const {
    return _prior_ktbn_.atemporalVarNames();
  }

  template < GUM_Numeric GUM_SCALAR >
  bool KTBNLearner< GUM_SCALAR >::_isKnownBase_(std::string_view base) const {
    const std::string b{base};
    return _prior_ktbn_.temporalVarNames().contains(b)
        || _prior_ktbn_.atemporalVarNames().contains(b);
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBN< GUM_SCALAR > KTBNLearner< GUM_SCALAR >::_assemble_(const BayesNet< GUM_SCALAR >& transitionBN,
                                                           const BayesNet< GUM_SCALAR >& initialBN,
                                                           const BayesNet< GUM_SCALAR >& atemporalBN) const {
    const int k = (int)_prior_ktbn_.k();

    // 1. Base: initialBN provides slices 0..k-2 (temporal structure + CPTs, and
    //    atemporal→temporal arcs). Atemporal variables enter only as forced roots;
    //    steps 4 and 6 overwrite their mutual structure and CPTs from atemporalBN
    //    (transitionBN repeats each atemporal value k-fold — not independent samples).
    BayesNet< GUM_SCALAR > bn(initialBN);

    // 2. Add the slice k-1 variables from transitionBN (absent from initialBN).
    for (const auto& base : _prior_ktbn_.temporalVarNames()) {
      const std::string name    = _encode_(base, k - 1);
      const NodeId      transId = transitionBN.idFromName(name);
      bn.add(transitionBN.variable(transId));
    }

    // 3. Add the arcs arriving at slice k-1 from transitionBN
    for (const auto& base : _prior_ktbn_.temporalVarNames()) {
      const std::string headName  = _encode_(base, k - 1);
      const NodeId      transHead = transitionBN.idFromName(headName);
      for (const NodeId transParent : transitionBN.parents(transHead)) {
        const std::string& parentName = transitionBN.variable(transParent).name();
        bn.addArc(parentName, headName);
      }
    }

    // 4. Add the atemporal -> atemporal arcs from atemporalBN. Skipped when
    //    atemporalBN is an empty placeholder: either <= 1 atemporal variable (no
    //    learner is built, see _build_()) or the learner was suppressed by a
    //    temporal-only possible-edge whitelist (see learnKTBN()). Either way the
    //    atemporal variables already sit in bn as roots, via initialBN.
    if (atemporalBN.size() != 0) {
      for (const auto& atemBase : _prior_ktbn_.atemporalVarNames()){
        const NodeId atemTail = atemporalBN.idFromName(atemBase);
        for (const NodeId atemChild : atemporalBN.children(atemTail)){
          const std::string& childName = atemporalBN.variable(atemChild).name();
          bn.addArc(atemBase, childName);
        }
      }
    }

    // 5. Fill the slice k-1 CPTs from transitionBN
    for (const auto& base : _prior_ktbn_.temporalVarNames()) {
      const std::string name    = _encode_(base, k - 1);
      const NodeId      bnId    = bn.idFromName(name);
      const NodeId      transId = transitionBN.idFromName(name);
      bn.cpt(bnId).fillWith(transitionBN.cpt(transId));
    }

    // 6. Fill the atemporal CPTs from atemporalBN (same guard as step 4); when
    //    skipped, the atemporal variables keep their initialBN marginal.
    if (atemporalBN.size() != 0) {
      for (const auto& atemBase : _prior_ktbn_.atemporalVarNames()) {
        const NodeId bnId    = bn.idFromName(atemBase);
        const NodeId atemId = atemporalBN.idFromName(atemBase);
        bn.cpt(bnId).fillWith(atemporalBN.cpt(atemId));
      }
    }

    // 7. Convert the assembled flat BN into a KTBN: fromBN infers k from the
    //    highest bracket index and validates temporal causality.
    return KTBN< GUM_SCALAR >::fromBN(bn);
  }

}   // namespace gum::learning
