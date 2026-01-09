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
/**
 * @file
 * @brief Template implementation for CausalFormula.
 */
#pragma once


#include <algorithm>
#include <sstream>
#include <vector>

#include <agrum/CM/tools/causalFormula.h>

namespace gum {

  // ---------- helpers ----------

  template < GUM_Numeric GUM_SCALAR >
  NodeSet CausalFormula< GUM_SCALAR >::_toNodeSetFromNames_(const CausalModel< GUM_SCALAR >& cm,
                                                            const NameSet& names) {
    NodeSet     ids;
    const auto& bn = cm.observationalBN();
    for (const auto& n: names) {
      ids.insert(bn.idFromName(n));   // throws NotFound if unknown
    }
    return ids;
  }

  template < GUM_Numeric GUM_SCALAR >
  void CausalFormula< GUM_SCALAR >::_ensureVariablesExist() const {
    const auto& bn = _cm.observationalBN();
    // Touch the variable object to ensure validity (throws NotFound if invalid)
    for (const auto& id: _on)
      (void)bn.variable(id);
    for (const auto& id: _doing)
      (void)bn.variable(id);
    for (const auto& id: _knowing)
      (void)bn.variable(id);
  }

  template < GUM_Numeric GUM_SCALAR >
  void CausalFormula< GUM_SCALAR >::_ensureNoVariablesOverlap() const {
    // Ensure pairwise disjointness: on ∩ doing = ∅, on ∩ knowing = ∅, doing ∩ knowing = ∅
    for (const auto& id: _on) {
      if (_doing.contains(id))
        GUM_ERROR(OperationNotAllowed, "Variable cannot be both in 'on' and 'doing'");
      if (_knowing.contains(id))
        GUM_ERROR(OperationNotAllowed, "Variable cannot be both in 'on' and 'knowing'");
    }
    for (const auto& id: _doing) {
      if (_knowing.contains(id))
        GUM_ERROR(OperationNotAllowed, "Variable cannot be both in 'doing' and 'knowing'");
    }
  }

  // ---------- constructors ----------

  template < GUM_Numeric GUM_SCALAR >
  CausalFormula< GUM_SCALAR >::CausalFormula(const CausalModel< GUM_SCALAR >&         cm,
                                             std::unique_ptr< ASTtree< GUM_SCALAR > > root,
                                             const NameSet&                           on,
                                             const NameSet&                           doing,
                                             const NameSet&                           knowing,
                                             const std::string&                       explanation) :
      _cm(cm), _root(std::move(root)), _on(_toNodeSetFromNames_(cm, on)),
      _doing(_toNodeSetFromNames_(cm, doing)), _knowing(_toNodeSetFromNames_(cm, knowing)),
      _explanation(explanation) {
    _ensureVariablesExist();
    _ensureNoVariablesOverlap();
  }

  template < GUM_Numeric GUM_SCALAR >
  CausalFormula< GUM_SCALAR >::CausalFormula(const CausalModel< GUM_SCALAR >&         cm,
                                             std::unique_ptr< ASTtree< GUM_SCALAR > > root,
                                             const NodeSet&                           on,
                                             const NodeSet&                           doing,
                                             const NodeSet&                           knowing,
                                             const std::string&                       explanation) :
      _cm(cm), _root(std::move(root)), _on(on), _doing(doing), _knowing(knowing),
      _explanation(explanation) {
    _ensureVariablesExist();
    _ensureNoVariablesOverlap();
  }

  // ---------- core API ----------

  // causalFormula_tpl.h (or inline in the header)
  template < GUM_Numeric GUM_SCALAR >
  inline bool CausalFormula< GUM_SCALAR >::isIdentified() const noexcept {
    return static_cast< bool >(_root);
  }

  template < GUM_Numeric GUM_SCALAR >
  inline const ASTtree< GUM_SCALAR >& CausalFormula< GUM_SCALAR >::root() const {
    if (!_root) GUM_ERROR(OperationNotAllowed, "No AST: effect not identified.");
    return *_root;
  }

  template < GUM_Numeric GUM_SCALAR >
  Tensor< GUM_SCALAR > CausalFormula< GUM_SCALAR >::eval() const {
    if (!_root) {
      GUM_ERROR(gum::OperationNotAllowed,
                "CausalFormula::eval() called on a non-identified formula.");
    }
    return _root->eval(_cm.observationalBN());
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string CausalFormula< GUM_SCALAR >::toString() const {
    if (!_root) return "[unidentifiable] " + _explanation;
    return _root->toString();
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string CausalFormula< GUM_SCALAR >::toLatex(const std::string& doOperatorPrefix,
                                                   const std::string& doOperatorSuffix) const {
    // Track variable name occurrences for prime management in AST LaTeX
    HashTable< std::string, int > nameOccur;

    const auto& bn       = _cm.observationalBN();
    auto        addNames = [&](const NodeSet& S) {
      for (const auto& id: S) {
        const auto& v    = bn.variable(id);
        const auto& name = v.name();
        if (!nameOccur.exists(name)) nameOccur.insert(name, 1);
      }
    };
    addNames(_on);
    addNames(_doing);
    addNames(_knowing);

    return latexQuery(doOperatorPrefix, doOperatorSuffix) + " = " + _root->toLatex(nameOccur);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string CausalFormula< GUM_SCALAR >::latexQuery(const std::string& doOperatorPrefix,
                                                      const std::string& doOperatorSuffix) const {
    const auto& bn = _cm.observationalBN();

    auto namesSorted = [&](const NodeSet& S) {
      std::vector< std::string > res;
      res.reserve(S.size());
      for (const auto& id: S)
        res.emplace_back(bn.variable(id).name());
      std::sort(res.begin(), res.end());
      return res;
    };

    const auto onNames      = namesSorted(_on);
    const auto doingNames   = namesSorted(_doing);
    const auto knowingNames = namesSorted(_knowing);

    std::stringstream ss;
    ss << "P\\left(";

    for (size_t i = 0; i < onNames.size(); ++i) {
      ss << onNames[i];
      if (i + 1 < onNames.size()) ss << ",";
    }

    if (!doingNames.empty() || !knowingNames.empty()) {
      ss << " \\mid ";

      if (!doingNames.empty()) {
        ss << doOperatorPrefix << doingNames.front();
        for (size_t i = 1; i < doingNames.size(); ++i)
          ss << "," << doingNames[i];
        ss << doOperatorSuffix;
      }

      if (!knowingNames.empty()) {
        if (!doingNames.empty()) ss << ",";
        ss << knowingNames.front();
        for (size_t i = 1; i < knowingNames.size(); ++i)
          ss << "," << knowingNames[i];
      }
    }

    ss << "\\right)";
    return ss.str();
  }

  template < GUM_Numeric GUM_SCALAR >
  std::unique_ptr< CausalFormula< GUM_SCALAR > > CausalFormula< GUM_SCALAR >::copy() const {
    return std::make_unique< CausalFormula< GUM_SCALAR > >(_cm,
                                                           _root->copy(),
                                                           _on,
                                                           _doing,
                                                           _knowing);
  }

  // ---------- convenience name accessors ----------

  template < GUM_Numeric GUM_SCALAR >
  std::vector< std::string > CausalFormula< GUM_SCALAR >::onNames() const {
    const auto&                bn = _cm.observationalBN();
    std::vector< std::string > out;
    out.reserve(_on.size());
    for (const auto& id: _on)
      out.emplace_back(bn.variable(id).name());
    std::sort(out.begin(), out.end());
    return out;
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< std::string > CausalFormula< GUM_SCALAR >::doingNames() const {
    const auto&                bn = _cm.observationalBN();
    std::vector< std::string > out;
    out.reserve(_doing.size());
    for (const auto& id: _doing)
      out.emplace_back(bn.variable(id).name());
    std::sort(out.begin(), out.end());
    return out;
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< std::string > CausalFormula< GUM_SCALAR >::knowingNames() const {
    const auto&                bn = _cm.observationalBN();
    std::vector< std::string > out;
    out.reserve(_knowing.size());
    for (const auto& id: _knowing)
      out.emplace_back(bn.variable(id).name());
    std::sort(out.begin(), out.end());
    return out;
  }

}   // namespace gum
