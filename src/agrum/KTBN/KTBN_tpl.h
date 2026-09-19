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
 * @brief Template implementation of the gum::KTBN class.
 *
 * @author Seth AGUILA & Anis KHACEF
 */

#pragma once

#include <algorithm>
#include <cctype>
#include <format>
#include <map>
#include <set>
#include <sstream>
#include <tuple>

#include <agrum/base/variables/allDiscreteVariables.h>
#include <agrum/BN/io/GUM/GumBNReader.h>
#include <agrum/BN/io/GUM/GumBNWriter.h>
#include <agrum/KTBN/KTBN.h>

namespace gum {

  // ===========================================================================
  // KTBNModality
  // ===========================================================================

  template < std::integral T >
  KTBNModality::KTBNModality(T modality) : isLabel(false), index(static_cast< Idx >(modality)) {}

  // ===========================================================================
  // Private naming helpers (the only place names are produced / parsed)
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  INLINE std::string KTBN< GUM_SCALAR >::_encode_(std::string_view base, int slice) const {
    if (slice == ATEMPORAL) return std::string{base};
    return std::string{base} + '[' + std::to_string(slice) + ']';
  }

  template < GUM_Numeric GUM_SCALAR >
  std::pair< std::string, int > KTBN< GUM_SCALAR >::_decodeName_(std::string_view name) const {
    const std::size_t bracketPos = name.rfind('[');
    if (bracketPos == std::string_view::npos) return {std::string{name}, ATEMPORAL};

    const std::string_view bracketContent = name.substr(bracketPos + 1);
    if (bracketContent.empty() || bracketContent.back() != ']')
      return {std::string{name}, ATEMPORAL};

    const std::string_view digits = bracketContent.substr(0, bracketContent.size() - 1);
    if (digits.empty()) return {std::string{name}, ATEMPORAL};
    for (const char c: digits)
      if (std::isdigit(static_cast< unsigned char >(c)) == 0) return {std::string{name}, ATEMPORAL};

    int slice{};
    try {
      slice = std::stoi(std::string{digits});
    } catch (const std::out_of_range&) {
      GUM_ERROR(InvalidArgument,
                "Node name '" << name << "' has a slice index too large to represent as int.")
    }
    return {std::string{name.substr(0, bracketPos)}, slice};
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE std::pair< std::string, int >
         KTBN< GUM_SCALAR >::_determineNode_(const std::string& name) const {
    // Atemporal and orphan-bracket nodes are registered in _atemporal_ and map to ATEMPORAL;
    // any other name is a temporal "base[t]" parsed by _decodeName_.
    if (_atemporal_.contains(name)) return {name, ATEMPORAL};
    return _decodeName_(name);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< std::pair< std::string, int > >
      KTBN< GUM_SCALAR >::_determineNodeSet_(const NodeSet& ids) const {
    std::vector< std::pair< std::string, int > > result;
    result.reserve(ids.size());
    for (const NodeId id: ids)
      result.push_back(_determineNode_(_bn_.variable(id).name()));
    return result;
  }

  // ===========================================================================
  // Constructors and destructor
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBN< GUM_SCALAR >::KTBN(Size k) : _k_(k) {
    if (k == 0) GUM_ERROR(InvalidArgument, "A k-DBN must have an order k >= 1.")
    GUM_CONSTRUCTOR(KTBN)
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBN< GUM_SCALAR >::~KTBN() {
    GUM_DESTRUCTOR(KTBN)
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBN< GUM_SCALAR >::KTBN(const KTBN< GUM_SCALAR >& source) :
      _k_(source._k_), _bn_(source._bn_), _temporal_(source._temporal_),
      _atemporal_(source._atemporal_) {
    GUM_CONS_CPY(KTBN)
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBN< GUM_SCALAR >::KTBN(KTBN< GUM_SCALAR >&& source) noexcept :
      _k_(source._k_), _bn_(std::move(source._bn_)), _temporal_(std::move(source._temporal_)),
      _atemporal_(std::move(source._atemporal_)) {
    GUM_CONS_MOV(KTBN)
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBN< GUM_SCALAR >& KTBN< GUM_SCALAR >::operator=(const KTBN< GUM_SCALAR >& source) {
    if (this != &source) {
      GUM_OP_CPY(KTBN);
      _k_         = source._k_;
      _bn_        = source._bn_;
      _temporal_  = source._temporal_;
      _atemporal_ = source._atemporal_;
    }
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBN< GUM_SCALAR >& KTBN< GUM_SCALAR >::operator=(KTBN< GUM_SCALAR >&& source) noexcept {
    if (this != &source) {
      GUM_OP_MOV(KTBN);
      _k_         = source._k_;
      _bn_        = std::move(source._bn_);
      _temporal_  = std::move(source._temporal_);
      _atemporal_ = std::move(source._atemporal_);
    }
    return *this;
  }

  // ===========================================================================
  // Accessors
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  INLINE Size KTBN< GUM_SCALAR >::k() const {
    return _k_;
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE Size KTBN< GUM_SCALAR >::size() const {
    return _bn_.size();
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE Size KTBN< GUM_SCALAR >::sizeArcs() const {
    return _bn_.sizeArcs();
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE bool KTBN< GUM_SCALAR >::empty() const {
    return _bn_.empty();
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBN< GUM_SCALAR >::clear() {
    _bn_.clear();
    _temporal_.clear();
    _atemporal_.clear();
  }

  // ===========================================================================
  // Variable management
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  void KTBN< GUM_SCALAR >::_validateAdd_(const std::string& base, bool temporal) const {
    const auto& ownSet   = temporal ? _temporal_ : _atemporal_;
    const auto& otherSet = temporal ? _atemporal_ : _temporal_;

    if (ownSet.contains(base))
      GUM_ERROR(DuplicateLabel,
                (temporal ? "A temporal process '" : "An atemporal variable '")
                    << base << "' already exists.")
    if (otherSet.contains(base))
      GUM_ERROR(DuplicateLabel,
                (temporal ? "Cannot add temporal process '" : "Cannot add atemporal variable '")
                    << base << "': " << (temporal ? "an atemporal variable" : "a temporal process")
                    << " with that name already exists.")

    if (temporal) {
      for (Size t = 0; t < _k_; ++t) {
        const std::string encoded = _encode_(base, static_cast< int >(t));
        if (_atemporal_.contains(encoded))
          GUM_ERROR(InvalidArgument,
                    "Temporal process '" << base << "' at slice " << t << " would produce node '"
                                         << encoded << "' which conflicts with atemporal variable '"
                                         << encoded << "'.")
      }
    } else {
      const auto [decodedBase, decodedSlice] = _decodeName_(base);
      if (decodedSlice != ATEMPORAL && _temporal_.contains(decodedBase)) {
        // Bracket notation over an existing temporal process is reserved in
        // FULL, whatever the index: _decodeName_ has no upper bound, so
        // "X[999]" decodes to (X, 999) and would shadow the process even though
        // no such node exists. Only the in-range case can claim an actual node
        // collision -- promising one for an out-of-range index would send the
        // caller looking for a node that was never there.
        if (Size(decodedSlice) < _k_)
          GUM_ERROR(InvalidArgument,
                    "Atemporal variable name '" << base << "' conflicts with temporal process '"
                                                << decodedBase
                                                << "': that name is already used by its slice "
                                                   "nodes.")
        GUM_ERROR(InvalidArgument,
                  "Atemporal variable name '"
                      << base << "' is invalid: '" << decodedBase
                      << "' is a temporal process, so every bracket-suffixed name over it is "
                         "reserved -- including slice "
                      << decodedSlice << ", beyond the current order k=" << _k_ << ".")
      }
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBN< GUM_SCALAR >::add(const DiscreteVariable& var, bool temporal) {
    const std::string base = var.name();

    if (temporal) {
      _validateAdd_(base, true);
      for (Size t = 0; t < _k_; ++t) {
        // clone() to rename; BayesNet::add() clones again internally (unavoidable via public API).
        std::unique_ptr< DiscreteVariable > clone(var.clone());
        clone->setName(_encode_(base, static_cast< int >(t)));
        _bn_.add(*clone);
      }
      _temporal_.insert(base);
    } else {
      _validateAdd_(base, false);
      _bn_.add(var);
      _atemporal_.insert(base);
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBN< GUM_SCALAR >::add(std::string_view fast_description,
                               bool             temporal,
                               unsigned int     default_nbrmod) {
    auto v = fastVariable< GUM_SCALAR >(std::string{fast_description}, Size(default_nbrmod));
    add(*v, temporal);
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE void KTBN< GUM_SCALAR >::addTemporal(const DiscreteVariable& var) {
    add(var, true);
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE void KTBN< GUM_SCALAR >::addAtemporal(const DiscreteVariable& var) {
    add(var, false);
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE void KTBN< GUM_SCALAR >::addTemporal(std::string_view fast_description,
                                              unsigned int     default_nbrmod) {
    add(fast_description, true, default_nbrmod);
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE void KTBN< GUM_SCALAR >::addAtemporal(std::string_view fast_description,
                                               unsigned int     default_nbrmod) {
    add(fast_description, false, default_nbrmod);
  }

  // ===========================================================================
  // Variable queries
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  INLINE bool KTBN< GUM_SCALAR >::exists(std::string_view base) const {
    const std::string baseStr{base};
    return _temporal_.contains(baseStr) || _atemporal_.contains(baseStr);
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE const std::unordered_set< std::string >& KTBN< GUM_SCALAR >::temporalVarNames() const {
    return _temporal_;
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE const std::unordered_set< std::string >& KTBN< GUM_SCALAR >::atemporalVarNames() const {
    return _atemporal_;
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE Size KTBN< GUM_SCALAR >::nbTemporalVars() const {
    return _temporal_.size();
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE Size KTBN< GUM_SCALAR >::nbAtemporalVars() const {
    return _atemporal_.size();
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< std::pair< std::string, int > > KTBN< GUM_SCALAR >::nodes() const {
    std::vector< std::pair< std::string, int > > result;
    result.reserve(size());
    for (const auto& a: _atemporal_)
      result.emplace_back(a, ATEMPORAL);
    for (const auto& p: _temporal_)
      for (Size t = 0; t < _k_; ++t)
        result.emplace_back(p, static_cast< int >(t));
    return result;
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< std::pair< std::string, int > > KTBN< GUM_SCALAR >::parents(std::string_view base,
                                                                           int slice) const {
    return _determineNodeSet_(_bn_.parents(_validateVariable_(base, slice)));
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< std::pair< std::string, int > >
      KTBN< GUM_SCALAR >::parents(std::string_view node_name) const {
    const auto [b, s] = _determineNode_(std::string{node_name});
    return parents(b, s);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< std::pair< std::string, int > > KTBN< GUM_SCALAR >::children(std::string_view base,
                                                                            int slice) const {
    return _determineNodeSet_(_bn_.children(_validateVariable_(base, slice)));
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< std::pair< std::string, int > >
      KTBN< GUM_SCALAR >::children(std::string_view node_name) const {
    const auto [b, s] = _determineNode_(std::string{node_name});
    return children(b, s);
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBN< GUM_SCALAR >::erase(std::string_view base) {
    const std::string baseStr{base};

    if (_temporal_.contains(baseStr)) {
      for (Size t = 0; t < _k_; ++t)
        _bn_.erase(_encode_(baseStr, static_cast< int >(t)));
      _temporal_.erase(baseStr);
    } else if (_atemporal_.contains(baseStr)) {
      _bn_.erase(baseStr);
      _atemporal_.erase(baseStr);
    } else {
      GUM_ERROR(NotFound, "No variable named '" << baseStr << "' in the k-DBN.")
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBN< GUM_SCALAR >::changeVariableName(std::string_view oldBase, std::string_view newBase) {
    const std::string oldStr{oldBase};
    const std::string newStr{newBase};

    if (oldStr == newStr) return;
    if (newStr.empty()) GUM_ERROR(InvalidArgument, "New base name must not be empty.")

    if (!_temporal_.contains(oldStr) && !_atemporal_.contains(oldStr))
      GUM_ERROR(NotFound, "No variable named '" << oldStr << "' in the k-DBN.")
    if (_temporal_.contains(newStr) || _atemporal_.contains(newStr))
      GUM_ERROR(DuplicateLabel, "A variable named '" << newStr << "' already exists.")

    if (_temporal_.contains(oldStr)) {
      for (Size t = 0; t < _k_; ++t)
        if (_atemporal_.contains(_encode_(newStr, static_cast< int >(t))))
          GUM_ERROR(InvalidArgument,
                    "Renaming to '" << newStr << "': slice " << t
                                    << " collides with an atemporal variable.")
      for (Size t = 0; t < _k_; ++t)
        _bn_.changeVariableName(_encode_(oldStr, static_cast< int >(t)),
                                _encode_(newStr, static_cast< int >(t)));
      _temporal_.erase(oldStr);
      _temporal_.insert(newStr);
    } else {
      const auto [decodedBase, decodedSlice] = _decodeName_(newStr);
      if (decodedSlice != ATEMPORAL && _temporal_.contains(decodedBase))
        GUM_ERROR(InvalidArgument,
                  "'" << newStr << "' conflicts with temporal process '" << decodedBase
                      << "': that name is already used by its slice nodes.")
      _bn_.changeVariableName(oldStr, newStr);
      _atemporal_.erase(oldStr);
      _atemporal_.insert(newStr);
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  NodeId KTBN< GUM_SCALAR >::_validateVariable_(std::string_view base, int slice) const {
    const std::string baseStr{base};

    if (slice == ATEMPORAL) {
      if (!_atemporal_.contains(baseStr)) {
        if (_temporal_.contains(baseStr))
          GUM_ERROR(OperationNotAllowed,
                    "'" << baseStr << "' is a temporal process but is used as atemporal.")
        GUM_ERROR(NotFound, "There is no atemporal variable named '" << baseStr << "'.")
      }
      return _bn_.idFromName(baseStr);
    }

    if (!_temporal_.contains(baseStr)) {
      if (_atemporal_.contains(baseStr))
        GUM_ERROR(OperationNotAllowed,
                  "'" << baseStr << "' is an atemporal variable but is used at slice " << slice
                      << ".")
      GUM_ERROR(NotFound, "There is no temporal process named '" << baseStr << "'.")
    }
    if (slice < 0 || Size(slice) >= _k_)
      GUM_ERROR(OutOfBounds,
                "Slice " << slice << " is out of [0," << (_k_ - 1) << "] for process '" << baseStr
                         << "'.")
    return _bn_.idFromName(_encode_(baseStr, slice));
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE const DiscreteVariable& KTBN< GUM_SCALAR >::variable(std::string_view base,
                                                              int              slice) const {
    return _bn_.variable(_validateVariable_(base, slice));
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE const DiscreteVariable& KTBN< GUM_SCALAR >::variable(std::string_view node_name) const {
    const auto [b, s] = _determineNode_(std::string{node_name});
    return variable(b, s);
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE int KTBN< GUM_SCALAR >::timeSlice(const DiscreteVariable& var) const {
    _bn_.idFromName(var.name());   // throws NotFound if var is not in this k-DBN
    return _determineNode_(var.name()).second;
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE std::string KTBN< GUM_SCALAR >::baseName(const DiscreteVariable& var) const {
    _bn_.idFromName(var.name());   // throws NotFound if var is not in this k-DBN
    return _determineNode_(var.name()).first;
  }

  // ===========================================================================
  // Arc management
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  void KTBN< GUM_SCALAR >::addArc(std::string_view tailBase,
                                  int              tailSlice,
                                  std::string_view headBase,
                                  int              headSlice) {
    const NodeId tail = _validateVariable_(tailBase, tailSlice);
    const NodeId head = _validateVariable_(headBase, headSlice);

    if (headSlice == ATEMPORAL) {
      if (tailSlice != ATEMPORAL)
        GUM_ERROR(OperationNotAllowed,
                  "A temporal variable cannot be a parent of the atemporal variable '" << headBase
                                                                                       << "'.")
    } else if (tailSlice != ATEMPORAL && tailSlice > headSlice) {
      GUM_ERROR(OperationNotAllowed,
                "An arc cannot go from a future slice (" << tailSlice << ") to a past slice ("
                                                         << headSlice << ").")
    }

    _bn_.addArc(tail, head);
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBN< GUM_SCALAR >::eraseArc(std::string_view tailBase,
                                    int              tailSlice,
                                    std::string_view headBase,
                                    int              headSlice) {
    _bn_.eraseArc(_validateVariable_(tailBase, tailSlice), _validateVariable_(headBase, headSlice));
  }

  template < GUM_Numeric GUM_SCALAR >
  bool KTBN< GUM_SCALAR >::existsArc(std::string_view tailBase,
                                     int              tailSlice,
                                     std::string_view headBase,
                                     int              headSlice) const {
    return _bn_.existsArc(_validateVariable_(tailBase, tailSlice),
                          _validateVariable_(headBase, headSlice));
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBN< GUM_SCALAR >::addArc(std::string_view tail, std::string_view head) {
    const auto [tb, ts] = _determineNode_(std::string{tail});
    const auto [hb, hs] = _determineNode_(std::string{head});
    addArc(tb, ts, hb, hs);
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBN< GUM_SCALAR >::eraseArc(std::string_view tail, std::string_view head) {
    const auto [tb, ts] = _determineNode_(std::string{tail});
    const auto [hb, hs] = _determineNode_(std::string{head});
    eraseArc(tb, ts, hb, hs);
  }

  template < GUM_Numeric GUM_SCALAR >
  bool KTBN< GUM_SCALAR >::existsArc(std::string_view tail, std::string_view head) const {
    const auto [tb, ts] = _determineNode_(std::string{tail});
    const auto [hb, hs] = _determineNode_(std::string{head});
    return existsArc(tb, ts, hb, hs);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< std::pair< std::pair< std::string, int >, std::pair< std::string, int > > >
      KTBN< GUM_SCALAR >::arcs() const {
    std::vector< std::pair< std::pair< std::string, int >, std::pair< std::string, int > > > result;
    result.reserve(_bn_.sizeArcs());
    for (const auto& arc: _bn_.arcs()) {
      result.emplace_back(_determineNode_(_bn_.variable(arc.tail()).name()),
                          _determineNode_(_bn_.variable(arc.head()).name()));
    }
    return result;
  }

  // ===========================================================================
  // Conditional probability tables
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >& KTBN< GUM_SCALAR >::cpt(std::string_view base,
                                                             int              slice) const {
    return _bn_.cpt(_validateVariable_(base, slice));
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >& KTBN< GUM_SCALAR >::cpt(std::string_view node_name) const {
    const auto [b, s] = _determineNode_(std::string{node_name});
    return cpt(b, s);
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE void KTBN< GUM_SCALAR >::generateCPTs() const {
    _bn_.generateCPTs();
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE void KTBN< GUM_SCALAR >::generateCPT(std::string_view base, int slice) const {
    _bn_.generateCPT(_validateVariable_(base, slice));
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE void KTBN< GUM_SCALAR >::generateCPT(std::string_view node_name) const {
    const auto [b, s] = _determineNode_(std::string{node_name});
    generateCPT(b, s);
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBN< GUM_SCALAR >::fillCPT(
      std::string_view                                               base,
      int                                                            slice,
      const std::map< std::pair< std::string, int >, KTBNModality >& parents,
      const std::vector< GUM_SCALAR >&                               distribution) const {
    const NodeId                id   = _validateVariable_(base, slice);
    const Tensor< GUM_SCALAR >& cpt  = _bn_.cpt(id);
    const DiscreteVariable&     self = _bn_.variable(id);

    if (distribution.size() != self.domainSize())
      GUM_ERROR(SizeError,
                "fillCPT: distribution has " << distribution.size() << " value(s) but '" << base
                                             << "' has " << self.domainSize() << " modalities.")

    const Size nbParents = cpt.nbrDim() - 1;
    if (parents.size() != nbParents)
      GUM_ERROR(SizeError,
                "fillCPT: " << parents.size() << " parent value(s) given but the node has "
                            << nbParents << " parent(s); every parent must be specified.")

    // Address each parent by its (base, slice) identity — order-independent.
    // No duplicate-parent check needed: a dictionary key is unique by
    // construction, and here each (base, slice) pair names exactly one node,
    // so no two distinct keys can alias the same parent.
    Instantiation inst(cpt);
    for (const auto& [parNode, parVal]: parents) {
      const auto& [parBase, parSlice] = parNode;
      const NodeId            parId   = _validateVariable_(parBase, parSlice);
      const DiscreteVariable& parVar  = _bn_.variable(parId);
      if (parId == id || !cpt.contains(parVar))
        GUM_ERROR(InvalidArgument,
                  "fillCPT: '" << parBase << "' is not a parent of the target node.")
      inst.chgVal(parVar, parVal.toIndex(parVar));
    }

    // Write the whole conditional distribution over the node's own modalities.
    for (Idx m = 0; m < self.domainSize(); ++m) {
      inst.chgVal(self, m);
      cpt.set(inst, distribution[m]);
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBN< GUM_SCALAR >::fillCPT(
      std::string_view node_name,
      const std::map< std::variant< std::string, std::pair< std::string, int > >, KTBNModality >&
                                       parents,
      const std::vector< GUM_SCALAR >& distribution) const {
    const NodeId                id   = _bn_.idFromName(std::string{node_name});
    const Tensor< GUM_SCALAR >& cpt  = _bn_.cpt(id);
    const DiscreteVariable&     self = _bn_.variable(id);

    if (distribution.size() != self.domainSize())
      GUM_ERROR(SizeError,
                "fillCPT: distribution has " << distribution.size() << " value(s) but '"
                                             << node_name << "' has " << self.domainSize()
                                             << " modalities.")

    const Size nbParents = cpt.nbrDim() - 1;
    if (parents.size() != nbParents)
      GUM_ERROR(SizeError,
                "fillCPT: " << parents.size() << " parent value(s) given but the node has "
                            << nbParents << " parent(s); every parent must be specified.")

    // Unlike the (base, slice)-keyed overload above, duplicates ARE possible here
    // despite unique map keys: "X[0]" and (base="X", slice=0) compare unequal as
    // std::variant values yet name the same node. Both are resolved to an engine
    // name first, so the seen-check below can catch the alias.
    Instantiation inst(cpt);
    NodeSet       seen;
    for (const auto& [parKey, parVal]: parents) {
      const std::string parName
          = std::holds_alternative< std::string >(parKey)
              ? std::get< std::string >(parKey)
              : _encode_(std::get< std::pair< std::string, int > >(parKey).first,
                         std::get< std::pair< std::string, int > >(parKey).second);
      const NodeId            parId  = _bn_.idFromName(parName);
      const DiscreteVariable& parVar = _bn_.variable(parId);
      if (parId == id || !cpt.contains(parVar))
        GUM_ERROR(InvalidArgument,
                  "fillCPT: '" << parName << "' is not a parent of the target node.")
      if (seen.contains(parId))
        GUM_ERROR(InvalidArgument, "fillCPT: parent '" << parName << "' is listed more than once.")
      seen.insert(parId);
      inst.chgVal(parVar, parVal.toIndex(parVar));
    }

    for (Idx m = 0; m < self.domainSize(); ++m) {
      inst.chgVal(self, m);
      cpt.set(inst, distribution[m]);
    }
  }

  // ===========================================================================
  // Transformations
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  BayesNet< GUM_SCALAR > KTBN< GUM_SCALAR >::toBN() const {
    return BayesNet< GUM_SCALAR >(_bn_);
  }

  template < GUM_Numeric GUM_SCALAR >
  BayesNet< GUM_SCALAR > KTBN< GUM_SCALAR >::unroll(Size nbTimeSlices) const {
    if (nbTimeSlices < _k_)
      GUM_ERROR(OperationNotAllowed,
                "Cannot unroll over " << nbTimeSlices << " slices: fewer than the order k=" << _k_
                                      << ".")

    BayesNet< GUM_SCALAR > unrolled;
    const int              kernelSlice = static_cast< int >(_k_ - 1);

    // 1. atemporal variables (kept as-is)
    for (const auto& a: _atemporal_) {
      unrolled.add(_bn_.variable(_bn_.idFromName(a)));
    }

    // 2. temporal variables, instantiated for every slice 0..nbTimeSlices-1
    for (const auto& p: _temporal_) {
      const DiscreteVariable& templateVar
          = _bn_.variable(_bn_.idFromName(_encode_(p, kernelSlice)));
      for (Size t = 0; t < nbTimeSlices; ++t) {
        // clone() to rename; BayesNet::add() clones again internally (unavoidable via public API).
        std::unique_ptr< DiscreteVariable > clone(templateVar.clone());
        clone->setName(_encode_(p, static_cast< int >(t)));
        unrolled.add(*clone);
      }
    }

    // 3. template arcs (slices 0..k-1) are copied verbatim: the engine names of
    //    the template already match the unrolled names for those slices.
    for (const auto& arc: _bn_.arcs()) {
      unrolled.addArc(_bn_.variable(arc.tail()).name(), _bn_.variable(arc.head()).name());
    }

    // 4. CPTs of the template nodes (slices 0..k-1): copied by name.
    for (const NodeId n: _bn_.nodes()) {
      unrolled.cpt(_bn_.variable(n).name()).fillWith(_bn_.cpt(n));
    }

    // 5. transition kernel: for each extra slice t = k..nbTimeSlices-1,
    //    add arcs and fill the CPT in one pass using lags computed once per process.
    for (const auto& p: _temporal_) {
      const NodeId                lastSliceNodeId = _bn_.idFromName(_encode_(p, kernelSlice));
      const Tensor< GUM_SCALAR >& templateCpt     = _bn_.cpt(lastSliceNodeId);

      // (parBase, lag): lag == ATEMPORAL for static parents, otherwise lag = (k-1) - parSlice.
      std::vector< std::pair< std::string, int > > lags;
      for (const auto& [parBase, parSlice]: parents(p, kernelSlice)) {
        const int lag = (parSlice == ATEMPORAL) ? ATEMPORAL : kernelSlice - parSlice;
        lags.emplace_back(parBase, lag);
      }

      HashTable< std::string, std::string > unrolledToTemplate;
      std::vector< std::string >            templateVarNames;

      for (Size t = _k_; t < nbTimeSlices; ++t) {
        const std::string child = _encode_(p, static_cast< int >(t));

        // Add arcs and build the unrolled->template name mapping simultaneously.
        unrolledToTemplate.clear();
        unrolledToTemplate.insert(child, _encode_(p, kernelSlice));
        for (const auto& [parBase, lag]: lags) {
          if (lag == ATEMPORAL) {
            unrolled.addArc(parBase, child);
            unrolledToTemplate.insert(parBase, parBase);
          } else {
            const std::string parName = _encode_(parBase, static_cast< int >(t) - lag);
            unrolled.addArc(parName, child);
            unrolledToTemplate.insert(parName, _encode_(parBase, kernelSlice - lag));
          }
        }

        // Fill the CPT using the mapping built above.
        const Tensor< GUM_SCALAR >& unrolledCpt = unrolled.cpt(child);
        templateVarNames.clear();
        templateVarNames.reserve(unrolledCpt.nbrDim());
        for (Idx i = 0; i < unrolledCpt.nbrDim(); ++i) {
          templateVarNames.push_back(unrolledToTemplate[unrolledCpt.variable(i).name()]);
        }
        unrolledCpt.fillWith(templateCpt, templateVarNames);
      }
    }

    return unrolled;
  }

  // ===========================================================================
  // Persistence and conversion
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  std::pair< std::string, bool > KTBN< GUM_SCALAR >::_resolveGumFormat_(std::string_view filename) {
    // The extension selects the format: ".jgum" is text, anything else is binary
    // and gets a ".bgum" extension appended if missing.
    std::string filepath{filename};
    const bool  text = filepath.ends_with(".jgum");
    if (!text && !filepath.ends_with(".bgum")) filepath += ".bgum";
    return {std::move(filepath), !text};   // .second = binary
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBN< GUM_SCALAR >::save(std::string_view filename) const {
    const auto [filepath, binary] = _resolveGumFormat_(filename);

    // Persist the temporal/atemporal classification (and k) as BN properties so that
    // load() restores the k-DBN exactly, instead of re-deriving it heuristically from
    // node names (ambiguous for k=1 processes and bracket-named atemporal variables).
    // Separator is ','; '\' and ',' inside names are backslash-escaped so any name is safe.
    auto join = [](const std::unordered_set< std::string >& names) {
      std::string out;
      for (const auto& n: names) {
        if (!out.empty()) out += ',';
        for (const char c: n) {
          if (c == '\\' || c == ',') out += '\\';
          out += c;
        }
      }
      return out;
    };

    BayesNet< GUM_SCALAR > annotated = _bn_;
    annotated.setProperty("KTBN.k", std::to_string(_k_));
    annotated.setProperty("KTBN.temporal", join(_temporal_));
    annotated.setProperty("KTBN.atemporal", join(_atemporal_));
    GumBNWriter< GUM_SCALAR > writer(binary);
    writer.write(filepath, annotated);
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBN< GUM_SCALAR > KTBN< GUM_SCALAR >::load(std::string_view filename) {
    const auto [filepath, binary] = _resolveGumFormat_(filename);

    BayesNet< GUM_SCALAR >    bn;
    GumBNReader< GUM_SCALAR > reader(&bn, filepath, binary);
    const Size                nbErr = reader.proceed();
    if (nbErr > 0) {
      std::stringstream stream;
      reader.showElegantErrorsAndWarnings(stream);
      reader.showErrorCounts(stream);
      GUM_ERROR(IOError, "KTBN::load: " << stream.str())
    }

    // A file written by save() carries the classification as properties: restore it
    // directly. Otherwise fall back to fromBN(), which re-derives it from node names.
    if (!(bn.existsProperty("KTBN.k") && bn.existsProperty("KTBN.temporal")
          && bn.existsProperty("KTBN.atemporal")))
      return fromBN(bn);

    auto split = [](std::string_view csv, std::unordered_set< std::string >& out) {
      std::string current;
      bool        escaped = false;
      for (const char c: csv) {
        if (escaped) {
          current += c;
          escaped = false;
        } else if (c == '\\') {
          escaped = true;
        } else if (c == ',') {
          if (!current.empty()) out.insert(current);
          current.clear();
        } else {
          current += c;
        }
      }
      if (!current.empty()) out.insert(current);
    };

    Size k_val{};
    try {
      k_val = static_cast< Size >(std::stoul(bn.property("KTBN.k")));
    } catch (const std::exception& e) {
      GUM_ERROR(IOError,
                "KTBN::load: malformed KTBN.k property ('" << bn.property("KTBN.k")
                                                           << "'): " << e.what())
    }
    KTBN< GUM_SCALAR > res(k_val);
    res._bn_ = bn;
    split(bn.property("KTBN.temporal"), res._temporal_);
    split(bn.property("KTBN.atemporal"), res._atemporal_);

    return res;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBN< GUM_SCALAR >
      KTBN< GUM_SCALAR >::fromBN(const BayesNet< GUM_SCALAR >&            bn,
                                 const std::unordered_set< std::string >& atemporalNodes,
                                 std::vector< std::string >*              warnings) {
    KTBN< GUM_SCALAR > res(1);   // _determineNodesFromBN_ below will modify this k=1
    res._bn_ = bn;
    res._determineNodesFromBN_(atemporalNodes, warnings);
    return res;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBN< GUM_SCALAR >::_determineNodesFromBN_(
      const std::unordered_set< std::string >& atemporalNodes,
      std::vector< std::string >*              warnings) {
    _temporal_.clear();
    _atemporal_.clear();

    // a declared name must be a node of the BN: checked before any mutation, so
    // a typo cannot leave the object half-built
    for (const std::string& name: atemporalNodes)
      if (!_bn_.exists(name))
        GUM_ERROR(NotFound, "fromBN: '" << name << "' is not a node of the BN.")

    const auto warn = [warnings](const std::string& message) {
      if (warnings != nullptr) warnings->push_back(message);
    };

    // fromBN()'s bracket-free convention: a name ending in a run of digits
    // denotes a temporal variable at the timeslice given by that integer (base =
    // everything before the run); a name with no trailing digit is atemporal.
    // Purely syntactic, like _decodeName_, but that one expects the engine's own
    // base[t] convention. Used below only when the source BN carries no bracket
    // at all (see hasBracket).
    const auto decodeTrailingSlice = [](std::string_view name) -> std::pair< std::string, int > {
      std::size_t pos = name.size();
      while (pos > 0 && std::isdigit(static_cast< unsigned char >(name[pos - 1])))
        --pos;
      if (pos == name.size()) return {std::string{name}, ATEMPORAL};

      const std::string_view digits = name.substr(pos);
      int                    slice{};
      try {
        slice = std::stoi(std::string{digits});
      } catch (const std::out_of_range&) {
        GUM_ERROR(InvalidArgument,
                  "Node name '" << name << "' has a slice index too large to represent as int.")
      }
      return {std::string{name.substr(0, pos)}, slice};
    };

    // The two conventions never mix within one graph: if any node name already
    // carries the engine's own base[t] bracket notation, the WHOLE graph is read
    // that way (legacy behaviour -- what KTBNLearner and toBN() round-trips
    // produce); only when no node carries a bracket at all does every node get
    // read via the trailing-integer convention above. Nodes declared in
    // atemporalNodes are skipped here: their shape says nothing about the rest
    // of the graph's convention, so a bracket-shaped one (e.g. an orphan
    // "Y[0]" named atemporal on purpose, see below) must not force
    // bracket-reading onto otherwise bracket-free temporal nodes.
    bool hasBracket = false;
    for (const NodeId n: _bn_.nodes()) {
      const std::string& name = _bn_.variable(n).name();
      if (atemporalNodes.contains(name)) continue;
      if (_decodeName_(name).second != ATEMPORAL) {
        hasBracket = true;
        break;
      }
    }

    // first pass: determine every node, collecting the slices seen per process
    std::vector< std::string >                         discovered;   // temporal bases, in order
    HashTable< std::string, HashTable< int, NodeId > > slicesPerProcess;
    int                                                maxSlice = -1;

    for (const NodeId n: _bn_.nodes()) {
      const std::string& name = _bn_.variable(n).name();

      // an explicitly declared node is atemporal whatever its shape, trailing
      // digits included: it never enters slicesPerProcess, so the completeness
      // rules below never see it and never warn about it
      if (atemporalNodes.contains(name)) {
        _atemporal_.insert(name);
        continue;
      }

      const auto [base, slice] = hasBracket ? _decodeName_(name) : decodeTrailingSlice(name);

      if (slice == ATEMPORAL) {
        _atemporal_.insert(base);
      } else {
        if (!slicesPerProcess.exists(base)) {
          slicesPerProcess.insert(base, HashTable< int, NodeId >());
          discovered.push_back(base);
        }

        if (slicesPerProcess[base].exists(slice))
          GUM_ERROR(OperationNotAllowed,
                    "Two variables map to process '" << base << "' at slice " << slice << ".")

        slicesPerProcess[base].insert(slice, n);

        if (slice > maxSlice) maxSlice = slice;
      }
    }

    _k_ = (maxSlice < 0) ? Size(1) : Size(maxSlice + 1);

    // second pass: register the complete processes. A group that does not cover
    // every slice 0..k-1 is NOT rejected: each of its nodes becomes an atemporal
    // variable, original name kept, and a warning is recorded. Only a base used
    // BOTH bare and temporal-shaped stays an error -- there the two readings
    // collide on one name, and no reclassification can resolve that.
    const char* const conventionNoun = hasBracket ? "bracket" : "digit-suffixed";
    for (const auto& base: discovered) {
      const HashTable< int, NodeId >& sliceMap = slicesPerProcess[base];

      if (_atemporal_.contains(base))
        GUM_ERROR(OperationNotAllowed,
                  "Base name '" << base << "' is used both as an atemporal variable (bare node '"
                                << base << "') and as a temporal process (via " << conventionNoun
                                << " nodes). " << "Rename one of them before calling fromBN().")

      std::string missing;
      for (Size t = 0; t < _k_; ++t)
        if (!sliceMap.exists(static_cast< int >(t))) {
          if (!missing.empty()) missing += ", ";
          missing += std::to_string(t);
        }

      if (missing.empty()) {
        // Every slice of a process must carry the SAME variable. add() cannot
        // break this -- it clones one variable into k instances -- so fromBN()
        // is the only way in, and nothing downstream re-checks: KTBNInference
        // sizes every ring slot of a process from the kernel slice alone, so a
        // slice with a divergent domain would be triangulated against the wrong
        // size and then filled with a tensor of another dimension.
        // Names necessarily differ between slices, and Variable::operator==
        // compares them, so the reference is renamed onto each slice in turn.
        const DiscreteVariable&             ref = _bn_.variable(sliceMap[0]);
        std::unique_ptr< DiscreteVariable > probe(ref.clone());
        for (Size t = 1; t < _k_; ++t) {
          const DiscreteVariable& other = _bn_.variable(sliceMap[static_cast< int >(t)]);
          probe->setName(other.name());
          if (!(*probe == other))
            GUM_ERROR(OperationNotAllowed,
                      "The temporal process '"
                          << base << "' has mismatched slice variables: '" << ref.name() << "' is "
                          << ref.domain() << " but '" << other.name() << "' is " << other.domain()
                          << ". Every slice of a process must have the same type and domain.")
        }
        _temporal_.insert(base);
        if (!hasBracket) {
          // Under the trailing-integer convention the slices just matched above
          // carry no bracket notation yet: rename them onto the engine's
          // canonical base[t] form -- the invariant every other method (add(),
          // unroll(), rename(), ...) relies on. Under the bracket convention
          // source names are already canonical, so nothing to do here.
          for (Size t = 0; t < _k_; ++t)
            _bn_.changeVariableName(_bn_.variable(sliceMap[static_cast< int >(t)]).name(),
                                    _encode_(base, static_cast< int >(t)));
        }
        continue;
      }

      std::string reclassified;
      for (auto it = sliceMap.cbegin(); it != sliceMap.cend(); ++it) {
        const std::string& nodeName = _bn_.variable(it.val()).name();
        _atemporal_.insert(nodeName);
        if (!reclassified.empty()) reclassified += ", ";
        reclassified += "'" + nodeName + "'";
      }
      warn("Node(s) " + reclassified + " look temporal (base='" + base + "', " + conventionNoun
           + " convention) but the process is missing slice(s) " + missing
           + " for k=" + std::to_string(_k_)
           + ": they are classified as atemporal variables, original name kept. Pass them in "
             "fromBN()'s atemporalNodes argument to make that explicit and silence this warning.");
    }

    // Every surviving process holds exactly the slices 0..k-1, so k is still the
    // one the first pass computed -- unless none survived, in which case the
    // largest slice index was contributed by a group that is now atemporal and
    // k has nothing left to describe.
    if (_temporal_.empty()) _k_ = Size(1);


    // third pass: validate temporal causality of the foreign arcs.
    for (const auto& arc: _bn_.arcs()) {
      const int tailSlice = _determineNode_(_bn_.variable(arc.tail()).name()).second;
      const int headSlice = _determineNode_(_bn_.variable(arc.head()).name()).second;
      if (headSlice == ATEMPORAL && tailSlice != ATEMPORAL)
        GUM_ERROR(OperationNotAllowed,
                  "The network has a temporal->atemporal arc into '"
                      << _bn_.variable(arc.head()).name() << "'.")
      if (headSlice != ATEMPORAL && tailSlice != ATEMPORAL && tailSlice > headSlice)
        GUM_ERROR(OperationNotAllowed,
                  "The network has a future->past arc " << _bn_.variable(arc.tail()).name() << "->"
                                                        << _bn_.variable(arc.head()).name() << ".")
    }
  }

  // ===========================================================================
  // Various
  // ===========================================================================

  template < GUM_Numeric GUM_SCALAR >
  std::string KTBN< GUM_SCALAR >::toString() const {
    const auto join = [](const std::vector< std::string >& v) {
      std::string s;
      for (const auto& n: v) {
        if (!s.empty()) s += ", ";
        s += n;
      }
      return s;
    };

    const std::vector< std::string > temporalNames(_temporal_.begin(), _temporal_.end());
    const std::vector< std::string > atemporalNames(_atemporal_.begin(), _atemporal_.end());

    std::string arcs;
    for (const auto& arc: _bn_.arcs())
      arcs += std::format("    {} -> {}\n",
                          _bn_.variable(arc.tail()).name(),
                          _bn_.variable(arc.head()).name());

    return std::format("k-TBN (k={}, {} nodes, {} arcs)\n"
                       "  temporal processes ({}): {}\n"
                       "  atemporal variables ({}): {}\n"
                       "\n  arcs ({}):\n{}",
                       _k_,
                       _bn_.size(),
                       _bn_.sizeArcs(),
                       _temporal_.size(),
                       join(temporalNames),
                       _atemporal_.size(),
                       join(atemporalNames),
                       _bn_.sizeArcs(),
                       arcs);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string KTBN< GUM_SCALAR >::toDot() const {
    return _timeSlicesToDot_(_bn_, false);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string KTBN< GUM_SCALAR >::toUnrolledDot(Size T, bool highlightReplicated) const {
    if (T < _k_)
      GUM_ERROR(OperationNotAllowed, "toUnrolledDot: T=" << T << " must be >= k=" << _k_ << ".")
    return _timeSlicesToDot_(unroll(T), highlightReplicated);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string KTBN< GUM_SCALAR >::_escapeDot_(std::string_view name) {
    std::string out;
    out.reserve(name.size());
    for (const char c: name) {
      if (c == '"') out += '\\';
      out += c;
    }
    return out;
  }

  // Direct port of pyAgrum's pyagrum.lib.dynamicBN._TimeSlicesToDot: groups nodes by
  // timeslice (one cluster per slice, atemporal nodes ungrouped), draws the real arcs
  // unconstrained, then chains each temporal variable across consecutive slices with
  // invisible edges so every cluster keeps the same vertical variable order.
  template < GUM_Numeric GUM_SCALAR >
  std::string KTBN< GUM_SCALAR >::_timeSlicesToDot_(const BayesNet< GUM_SCALAR >& bn,
                                                    bool highlightReplicated) const {

    // Group (full name, base label) by timeslice. std::map keeps keys sorted, and
    // ATEMPORAL == -1 so atemporal variables naturally sort first, followed by
    // increasing slice indices — mirroring pyAgrum's noTimeCluster-then-slices order.
    std::map< int, std::vector< std::pair< std::string, std::string > > > timeslices;
    for (const NodeId n: bn.nodes()) {
      const std::string& name  = bn.variable(n).name();
      const auto [base, slice] = _decodeName_(name);
      timeslices[slice].emplace_back(name, base);
    }

    std::stringstream dot;
    dot << "digraph KTBN {\n";
    dot << "  rankdir=LR;\n";
    dot << "  splines=ortho;\n";
    dot << "  node [color=\"#000000\", fillcolor=white, style=filled];\n\n";

    for (auto& [slice, nodes]: timeslices) {
      std::sort(nodes.begin(), nodes.end());
      if (slice == ATEMPORAL) {
        dot << "  subgraph cluster_atemporal {\n";
        dot << "    label=\"atemporal\";\n";
        dot << "    style=filled;\n";
        dot << "    bgcolor=\"lightyellow\";\n";
        for (const auto& [full, label]: nodes)
          dot << "    \"" << _escapeDot_(full) << "\" [label=\"" << _escapeDot_(label) << "\"];\n";
        dot << "  }\n";
      } else {
        const bool replicated = highlightReplicated && Size(slice) >= _k_;
        dot << "  subgraph cluster_" << slice << " {\n";
        dot << "    label=\"Time slice " << slice << "\";\n";
        dot << "    style=filled;\n";
        dot << "    bgcolor=\"" << (replicated ? "lightcyan" : "#DDDDDD") << "\";\n";
        for (const auto& [full, label]: nodes)
          dot << "    \"" << _escapeDot_(full) << "\" [label=\"" << _escapeDot_(label) << "\"];\n";
        dot << "  }\n";
      }
      dot << "\n";
    }

    dot << "  edge [color=black, constraint=false];\n";
    for (const auto& arc: bn.arcs())
      dot << "  \"" << _escapeDot_(bn.variable(arc.tail()).name()) << "\" -> \""
          << _escapeDot_(bn.variable(arc.head()).name()) << "\";\n";

    dot << "\n  edge [style=invis, constraint=true];\n";
    if (const auto it0 = timeslices.find(0); it0 != timeslices.end()) {
      for (const auto& node0: it0->second) {
        const std::string& label = node0.second;
        int                prec  = ATEMPORAL;
        bool               first = true;
        for (const auto& [slice, nodes]: timeslices) {
          if (slice == ATEMPORAL) continue;
          if (!first)
            dot << "  \"" << _escapeDot_(_encode_(label, prec)) << "\" -> \""
                << _escapeDot_(_encode_(label, slice)) << "\";\n";
          prec  = slice;
          first = false;
        }
      }
    }

    dot << "}\n";
    return dot.str();
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE std::string KTBN< GUM_SCALAR >::bnToDot() const {
    return _bn_.toDot();
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string KTBN< GUM_SCALAR >::summaryGraph() const {
    std::set< std::string > baseNames(_temporal_.begin(), _temporal_.end());
    baseNames.insert(_atemporal_.begin(), _atemporal_.end());

    const int lastSlice = static_cast< int >(_k_) - 1;
    std::set< std::tuple< std::string, std::string, int > > edges;
    for (const auto& arc: _bn_.arcs()) {
      const auto [tailBase, tailSlice] = _decodeName_(_bn_.variable(arc.tail()).name());
      const auto [headBase, headSlice] = _decodeName_(_bn_.variable(arc.head()).name());
      if (headSlice != lastSlice) continue;   // not part of the repeated transition kernel

      const int lag = (tailSlice == ATEMPORAL) ? ATEMPORAL : headSlice - tailSlice;
      edges.emplace(tailBase, headBase, lag);
    }

    std::stringstream dot;
    dot << "digraph KTBN {\n";
    dot << "  rankdir=LR;\n";
    dot << "  node [color=\"#000000\", fillcolor=white, style=filled];\n\n";

    for (const auto& base: baseNames)
      dot << "  \"" << _escapeDot_(base) << "\";\n";
    dot << "\n";

    for (const auto& [tailBase, headBase, lag]: edges) {
      dot << "  \"" << _escapeDot_(tailBase) << "\" -> \"" << _escapeDot_(headBase) << "\"";
      if (lag != ATEMPORAL) dot << " [label=\"" << lag << "\"]";
      dot << ";\n";
    }

    dot << "}\n";
    return dot.str();
  }

  template < GUM_Numeric GUM_SCALAR >
  std::ostream& operator<<(std::ostream& output, const KTBN< GUM_SCALAR >& kdbn) {
    output << kdbn.toString();
    return output;
  }

}   // namespace gum
