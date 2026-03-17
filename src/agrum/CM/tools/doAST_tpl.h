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
 * @brief Template implementation of CausalModel/doAST.h
 */

#pragma once

#include <algorithm>
#include <sstream>

#include <agrum/BN/IBayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/CM/tools/doAST.h>

namespace gum {

  // ================================================================
  // ASTtree
  // ================================================================
  template < GUM_Numeric GUM_SCALAR >
  ASTtree< GUM_SCALAR >::ASTtree(std::string_view type) : _type(type) {}

  template < GUM_Numeric GUM_SCALAR >
  std::string ASTtree< GUM_SCALAR >::toLatex(HashTable< std::string, int > nameOccur) const {
    // We do not rely on nameOccur mutation at this stage; pass through.
    return fastToLatex(nameOccur);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string ASTtree< GUM_SCALAR >::_latexCorrect(std::string_view               srcName,
                                                   HashTable< std::string, int >& nameOccur) {
    int       count = nameOccur.getWithDefault(std::string{srcName}, 0);
    const int nbr   = (count > 1) ? (count - 1) : 0;
    // append nbr primes
    return std::string{srcName} + std::string(static_cast< size_t >(nbr), '\'');
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< std::string >
      ASTtree< GUM_SCALAR >::_latexCorrect(const Set< std::string >&      srcNames,
                                           HashTable< std::string, int >& nameOccur) {
    // Transform each name using the single-name overload, then sort.
    std::vector< std::string > out;
    for (const auto& n: srcNames) {
      out.push_back(_latexCorrect(n, nameOccur));
    }
    std::sort(out.begin(), out.end());
    return out;
  }

  // ================================================================
  // ASTBinaryOp
  // ================================================================
  template < GUM_Numeric GUM_SCALAR >
  ASTBinaryOp< GUM_SCALAR >::ASTBinaryOp(std::string_view                         type,
                                         std::unique_ptr< ASTtree< GUM_SCALAR > > op1,
                                         std::unique_ptr< ASTtree< GUM_SCALAR > > op2) :
      ASTtree< GUM_SCALAR >(type), _op1(std::move(op1)), _op2(std::move(op2)) {}

  template < GUM_Numeric GUM_SCALAR >
  std::string ASTBinaryOp< GUM_SCALAR >::toString(std::string_view prefix) const {
    std::stringstream s;
    s << prefix << this->_type << "\n";
    s << _op1->toString(std::string{prefix} + ASTtree< GUM_SCALAR >::CONTINUE_PREFIX) << "\n";
    s << _op2->toString(std::string{prefix} + ASTtree< GUM_SCALAR >::CONTINUE_PREFIX);
    return s.str();
  }

  // ================================================================
  // ASTplus (ASTBinaryOp)
  // ================================================================
  template < GUM_Numeric GUM_SCALAR >
  ASTplus< GUM_SCALAR >::ASTplus(std::unique_ptr< ASTtree< GUM_SCALAR > > op1,
                                 std::unique_ptr< ASTtree< GUM_SCALAR > > op2) :
      ASTBinaryOp< GUM_SCALAR >("+", std::move(op1), std::move(op2)) {}

  template < GUM_Numeric GUM_SCALAR >
  std::string
      ASTplus< GUM_SCALAR >::protectToLatex(HashTable< std::string, int >& nameOccur) const {
    return "\\left(" + fastToLatex(nameOccur) + "\\right)";
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string ASTplus< GUM_SCALAR >::fastToLatex(HashTable< std::string, int >& nameOccur) const {
    return this->_op1->fastToLatex(nameOccur) + "+" + this->_op2->fastToLatex(nameOccur);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::unique_ptr< ASTtree< GUM_SCALAR > > ASTplus< GUM_SCALAR >::copy() const {
    auto l = this->_op1->copy();
    auto r = this->_op2->copy();
    return std::make_unique< ASTplus< GUM_SCALAR > >(std::move(l), std::move(r));
  }

  template < GUM_Numeric GUM_SCALAR >
  Tensor< GUM_SCALAR > ASTplus< GUM_SCALAR >::eval(const IBayesNet< GUM_SCALAR >& bn) const {
    return this->_op1->eval(bn) + this->_op2->eval(bn);
  }

  // ================================================================
  // ASTminus (ASTBinaryOp)
  // ================================================================
  template < GUM_Numeric GUM_SCALAR >
  ASTminus< GUM_SCALAR >::ASTminus(std::unique_ptr< ASTtree< GUM_SCALAR > > op1,
                                   std::unique_ptr< ASTtree< GUM_SCALAR > > op2) :
      ASTBinaryOp< GUM_SCALAR >("-", std::move(op1), std::move(op2)) {}

  template < GUM_Numeric GUM_SCALAR >
  std::string
      ASTminus< GUM_SCALAR >::protectToLatex(HashTable< std::string, int >& nameOccur) const {
    return "\\left(" + fastToLatex(nameOccur) + "\\right)";
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string ASTminus< GUM_SCALAR >::fastToLatex(HashTable< std::string, int >& nameOccur) const {
    return this->_op1->fastToLatex(nameOccur) + "-" + this->_op2->fastToLatex(nameOccur);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::unique_ptr< ASTtree< GUM_SCALAR > > ASTminus< GUM_SCALAR >::copy() const {
    auto l = this->_op1->copy();
    auto r = this->_op2->copy();
    return std::make_unique< ASTminus< GUM_SCALAR > >(std::move(l), std::move(r));
  }

  template < GUM_Numeric GUM_SCALAR >
  Tensor< GUM_SCALAR > ASTminus< GUM_SCALAR >::eval(const IBayesNet< GUM_SCALAR >& bn) const {
    return this->_op1->eval(bn) - this->_op2->eval(bn);
  }

  // ================================================================
  // ASTmult (ASTBinaryOp)
  // ================================================================
  template < GUM_Numeric GUM_SCALAR >
  ASTmult< GUM_SCALAR >::ASTmult(std::unique_ptr< ASTtree< GUM_SCALAR > > op1,
                                 std::unique_ptr< ASTtree< GUM_SCALAR > > op2) :
      ASTBinaryOp< GUM_SCALAR >("*", std::move(op1), std::move(op2)) {}

  template < GUM_Numeric GUM_SCALAR >
  std::string
      ASTmult< GUM_SCALAR >::protectToLatex(HashTable< std::string, int >& nameOccur) const {
    return this->_op1->protectToLatex(nameOccur) + " \\cdot "
         + this->_op2->protectToLatex(nameOccur);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string ASTmult< GUM_SCALAR >::fastToLatex(HashTable< std::string, int >& nameOccur) const {
    return this->_op1->fastToLatex(nameOccur) + " \\cdot " + this->_op2->fastToLatex(nameOccur);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::unique_ptr< ASTtree< GUM_SCALAR > > ASTmult< GUM_SCALAR >::copy() const {
    auto l = this->_op1->copy();
    auto r = this->_op2->copy();
    return std::make_unique< ASTmult< GUM_SCALAR > >(std::move(l), std::move(r));
  }

  template < GUM_Numeric GUM_SCALAR >
  Tensor< GUM_SCALAR > ASTmult< GUM_SCALAR >::eval(const IBayesNet< GUM_SCALAR >& bn) const {
    return this->_op1->eval(bn) * this->_op2->eval(bn);
  }

  // ================================================================
  // ASTdiv (ASTBinaryOp)
  // ================================================================
  template < GUM_Numeric GUM_SCALAR >
  ASTdiv< GUM_SCALAR >::ASTdiv(std::unique_ptr< ASTtree< GUM_SCALAR > > op1,
                               std::unique_ptr< ASTtree< GUM_SCALAR > > op2) :
      ASTBinaryOp< GUM_SCALAR >("/", std::move(op1), std::move(op2)) {}

  template < GUM_Numeric GUM_SCALAR >
  std::string ASTdiv< GUM_SCALAR >::protectToLatex(HashTable< std::string, int >& nameOccur) const {
    return fastToLatex(nameOccur);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string ASTdiv< GUM_SCALAR >::fastToLatex(HashTable< std::string, int >& nameOccur) const {
    return " \\frac {" + this->_op1->fastToLatex(nameOccur) + "}{"
         + this->_op2->fastToLatex(nameOccur) + "}";
  }

  template < GUM_Numeric GUM_SCALAR >
  std::unique_ptr< ASTtree< GUM_SCALAR > > ASTdiv< GUM_SCALAR >::copy() const {
    auto l = this->_op1->copy();
    auto r = this->_op2->copy();
    return std::make_unique< ASTdiv< GUM_SCALAR > >(std::move(l), std::move(r));
  }

  template < GUM_Numeric GUM_SCALAR >
  Tensor< GUM_SCALAR > ASTdiv< GUM_SCALAR >::eval(const IBayesNet< GUM_SCALAR >& bn) const {
    return this->_op1->eval(bn) / this->_op2->eval(bn);
  }

  // ================================================================
  // ASTposteriorProba   :  P_bn(vars | knw_min)
  // ================================================================
  template < GUM_Numeric GUM_SCALAR >
  ASTposteriorProba< GUM_SCALAR >::ASTposteriorProba(const DAGmodel&           bn,
                                                     const Set< std::string >& vars,
                                                     const Set< std::string >& knw) :
      ASTposteriorProba(vars, _compute_knw_from_bn(bn, vars, knw)) {}

  template < GUM_Numeric GUM_SCALAR >
  ASTposteriorProba< GUM_SCALAR >::ASTposteriorProba(
      const DAG&                              dag,
      const Bijection< NodeId, std::string >& id2name,
      const Set< std::string >&               vars,
      const Set< std::string >&               knw) :
      ASTposteriorProba(vars, _compute_knw_from_dag(dag, id2name, vars, knw)) {}

  template < GUM_Numeric GUM_SCALAR >
  ASTposteriorProba< GUM_SCALAR >::ASTposteriorProba(const Set< std::string >& vars,
                                                     const Set< std::string >& knw) :
      ASTtree< GUM_SCALAR >("_posterior_"), _vars(vars), _knw(knw) {
    _ensure_nonempty(vars);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string ASTposteriorProba< GUM_SCALAR >::toString(std::string_view prefix) const {
    std::stringstream s;
    s << "P(";

    // Share the occurrence map across both sides so duplicate
    // names (if any) get consistent LaTeX suffixes.
    gum::HashTable< std::string, int > occur;

    // LEFT: variables, sorted & latex-corrected
    auto left  = ASTtree< GUM_SCALAR >::_latexCorrect(_vars, occur);
    bool first = true;
    for (const auto& v: left) {
      if (!first) s << ",";
      s << v;
      first = false;
    }

    // RIGHT: conditioning set, sorted & latex-corrected
    if (!_knw.empty()) {
      s << "|";
      auto right = ASTtree< GUM_SCALAR >::_latexCorrect(_knw, occur);
      first      = true;
      for (const auto& k: right) {
        if (!first) s << ",";
        s << k;
        first = false;
      }
    }

    s << ")";
    return std::string{prefix} + s.str();
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string ASTposteriorProba< GUM_SCALAR >::protectToLatex(
      HashTable< std::string, int >& nameOccur) const {
    return fastToLatex(nameOccur);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string
      ASTposteriorProba< GUM_SCALAR >::fastToLatex(HashTable< std::string, int >& nameOccur) const {
    std::stringstream s;
    s << "P\\left(";

    // vars
    {
      auto corr  = ASTtree< GUM_SCALAR >::_latexCorrect(_vars, nameOccur);
      bool first = true;
      for (const auto& v: corr) {
        if (!first) s << ",";
        s << v;
        first = false;
      }
    }

    // | knw
    if (!_knw.empty()) {
      s << "\\mid ";
      auto corr  = ASTtree< GUM_SCALAR >::_latexCorrect(_knw, nameOccur);
      bool first = true;
      for (const auto& k: corr) {
        if (!first) s << ",";
        s << k;
        first = false;
      }
    }

    s << "\\right)";
    return s.str();
  }

  template < GUM_Numeric GUM_SCALAR >
  std::unique_ptr< ASTtree< GUM_SCALAR > > ASTposteriorProba< GUM_SCALAR >::copy() const {
    // Avoid recomputing minimal set: use private direct-ctor tag declared in header
    return std::make_unique< ASTposteriorProba< GUM_SCALAR > >(_vars, _knw);
  }

  template < GUM_Numeric GUM_SCALAR >
  Tensor< GUM_SCALAR >
      ASTposteriorProba< GUM_SCALAR >::eval(const IBayesNet< GUM_SCALAR >& contextual_bn) const {
    // Build NodeSets from names
    NodeSet set_vars, set_knw;
    for (const auto& v: _vars)
      set_vars.insert(contextual_bn.idFromName(v));
    for (const auto& k: _knw)
      set_knw.insert(contextual_bn.idFromName(k));


    // --- simple CPT case: P(var | parents(var)) ---
    if (_vars.size() == 1) {
      const auto& x  = *_vars.begin();
      NodeId      ix = contextual_bn.idFromName(x);

      // Directly compare NodeSets: parents(ix) vs set_knw
      if (contextual_bn.parents(ix) == set_knw) { return contextual_bn.cpt(ix); }
    }


    // --- otherwise: use inference ---
    LazyPropagation< GUM_SCALAR > ie(&contextual_bn);
    Tensor< GUM_SCALAR >          p;

    if (_knw.empty()) {
      ie.addJointTarget(set_vars);
      ie.makeInference();
      p = ie.jointPosterior(set_vars);
    } else {
      ie.addJointTarget(set_vars);
      ie.makeInference();
      p = ie.evidenceJointImpact(set_vars, set_knw);
    }


    return p;
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE void ASTposteriorProba< GUM_SCALAR >::_ensure_nonempty(const Set< std::string >& vars) {
    if (vars.empty()) { GUM_ERROR(InvalidArgument, "ASTposteriorProba: vars must not be empty"); }
  }

  template < GUM_Numeric GUM_SCALAR >
  Set< std::string >
      ASTposteriorProba< GUM_SCALAR >::_compute_knw_from_bn(const DAGmodel&           bn,
                                                            const Set< std::string >& vars,
                                                            const Set< std::string >& knw) {
    NodeSet varIds, knwIds;
    for (const auto& v: vars)
      varIds.insert(bn.idFromName(v));
    for (const auto& k: knw)
      knwIds.insert(bn.idFromName(k));
    Set< std::string > out;
    const auto         minK = bn.minimalCondSet(varIds, knwIds);
    for (auto nid: minK)
      out.insert(bn.variable(nid).name());
    return out;
  }

  template < GUM_Numeric GUM_SCALAR >
  Set< std::string > ASTposteriorProba< GUM_SCALAR >::_compute_knw_from_dag(
      const DAG&                              dag,
      const Bijection< NodeId, std::string >& id2name,
      const Set< std::string >&               vars,
      const Set< std::string >&               knw) {
    NodeSet varIds, knwIds;
    for (const auto& v: vars)
      varIds.insert(id2name.first(v));
    for (const auto& k: knw)
      knwIds.insert(id2name.first(k));
    Set< std::string > out;
    const auto         minK = dag.minimalCondSet(varIds, knwIds);
    for (auto nid: minK)
      out.insert(id2name.second(nid));
    return out;
  }

  // ================================================================
  // ASTjointProba :  P(vars) in observational BN
  // ================================================================
  template < GUM_Numeric GUM_SCALAR >
  ASTjointProba< GUM_SCALAR >::ASTjointProba(const Set< std::string >& varNames) :
      ASTtree< GUM_SCALAR >("_joint_"), _varNames(varNames) {
    // validate that vars is not empty
    if (varNames.empty()) { GUM_ERROR(InvalidArgument, "ASTjointProba: vars must not be empty"); }
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string ASTjointProba< GUM_SCALAR >::toString(std::string_view prefix) const {
    std::stringstream s;
    s << "joint P(";

    // Build a consistent, sorted, LaTeX-corrected list of names
    gum::HashTable< std::string, int > occur;   // tracks suffixes if needed
    auto names = ASTtree< GUM_SCALAR >::_latexCorrect(_varNames, occur);

    bool first = true;
    for (const auto& v: names) {
      if (!first) s << ",";
      s << v;
      first = false;
    }

    s << ")";
    return std::string{prefix} + s.str();
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string
      ASTjointProba< GUM_SCALAR >::protectToLatex(HashTable< std::string, int >& nameOccur) const {
    return fastToLatex(nameOccur);
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string
      ASTjointProba< GUM_SCALAR >::fastToLatex(HashTable< std::string, int >& nameOccur) const {
    std::stringstream s;
    s << "P\\left(";
    auto corr  = ASTtree< GUM_SCALAR >::_latexCorrect(_varNames, nameOccur);
    bool first = true;
    for (const auto& v: corr) {
      if (!first) s << ",";
      s << v;
      first = false;
    }
    s << "\\right)";
    return s.str();
  }

  template < GUM_Numeric GUM_SCALAR >
  std::unique_ptr< ASTtree< GUM_SCALAR > > ASTjointProba< GUM_SCALAR >::copy() const {
    return std::make_unique< ASTjointProba< GUM_SCALAR > >(_varNames);
  }

  template < GUM_Numeric GUM_SCALAR >
  Tensor< GUM_SCALAR >
      ASTjointProba< GUM_SCALAR >::eval(const IBayesNet< GUM_SCALAR >& contextual_bn) const {
    NodeSet ids;
    for (const auto& v: _varNames)
      ids.insert(contextual_bn.idFromName(v));

    LazyPropagation< GUM_SCALAR > ie(&contextual_bn);
    if (ids.size() == 1) {
      // Special case: single variable, use its CPT directly
      NodeId id = *ids.begin();
      ie.addTarget(id);
      ie.makeInference();
      return ie.posterior(id);
    } else {
      // General case: add all variables as joint target
      ie.addJointTarget(ids);
      ie.makeInference();
      return ie.jointPosterior(ids);
    }
  }

  // ================================================================
  // ASTsum  :  sum out over variable of sub-term
  // ================================================================
  template < GUM_Numeric GUM_SCALAR >
  ASTsum< GUM_SCALAR >::ASTsum(std::string_view                         var,
                               std::unique_ptr< ASTtree< GUM_SCALAR > > term) :
      ASTtree< GUM_SCALAR >("_sum_"), _var(var), _term(std::move(term)) {}

  template < GUM_Numeric GUM_SCALAR >
  ASTsum< GUM_SCALAR >::ASTsum(const std::vector< std::string >&        vars,
                               std::unique_ptr< ASTtree< GUM_SCALAR > > term) :
      ASTtree< GUM_SCALAR >("_sum_") {
    if (vars.empty()) { throw gum::InvalidArgument("ASTsum: variable list cannot be empty"); }

    _var = vars.front();

    if (vars.size() > 1) {
      std::vector< std::string > tail(vars.begin() + 1, vars.end());
      _term = std::make_unique< ASTsum< GUM_SCALAR > >(tail, std::move(term));
    } else {
      _term = std::move(term);
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string ASTsum< GUM_SCALAR >::toString(std::string_view prefix) const {
    std::stringstream s;
    s << prefix << "sum on " << _var << " for\n";
    s << _term->toString(std::string{prefix} + ASTtree< GUM_SCALAR >::CONTINUE_PREFIX);
    return s.str();
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string ASTsum< GUM_SCALAR >::protectToLatex(HashTable< std::string, int >& nameOccur) const {
    return "\\left(" + fastToLatex(nameOccur) + "\\right)";
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string ASTsum< GUM_SCALAR >::fastToLatex(HashTable< std::string, int >& nameOccur) const {
    // Flatten chained sums: collect all summed variables along the _term chain.
    std::vector< std::string >   vars;
    const ASTtree< GUM_SCALAR >* a = this;
    while (auto s = dynamic_cast< const ASTsum< GUM_SCALAR >* >(a)) {
      if (!nameOccur.exists(s->_var)) nameOccur.insert(s->_var, 0);
      nameOccur[s->_var] += 1;
      vars.push_back(s->_var);
      a = &s->term();   // move down the chain
    }

    // Build corrected (prime-adjusted) names, then sort.
    Set< std::string > varNames;
    for (const auto& v: vars) {
      varNames.insert(v);
    }
    auto corr = ASTtree< GUM_SCALAR >::_latexCorrect(varNames, nameOccur);

    // Join corrected names with commas
    std::stringstream names;
    for (size_t i = 0; i < corr.size(); ++i) {
      if (i) names << ",";
      names << corr[i];
    }

    // Inner formula is the first non-sum node's latex with current nameOccur
    std::stringstream out;
    out << "\\sum_{" << names.str() << "}{" << a->protectToLatex(nameOccur) << "}";

    // Restore nameOccur (decrement each var)
    for (const auto& v: vars) {
      // nameOccur[v] is guaranteed to exist here
      nameOccur[v] -= 1;
    }

    return out.str();
  }

  template < GUM_Numeric GUM_SCALAR >
  std::unique_ptr< ASTtree< GUM_SCALAR > > ASTsum< GUM_SCALAR >::copy() const {
    return std::make_unique< ASTsum< GUM_SCALAR > >(_var, _term->copy());
  }

  template < GUM_Numeric GUM_SCALAR >
  Tensor< GUM_SCALAR >
      ASTsum< GUM_SCALAR >::eval(const IBayesNet< GUM_SCALAR >& contextual_bn) const {
    auto base = _term->eval(contextual_bn);

    const auto& dv = contextual_bn.variable(contextual_bn.idFromName(_var));
    VariableSet dv_set;
    dv_set.insert(&dv);
    return base.sumOut(dv_set);
  }

  // ================================================================
  // Utility: productOfTrees
  // ================================================================
  template < GUM_Numeric GUM_SCALAR >
  std::unique_ptr< ASTtree< GUM_SCALAR > >
      productOfTrees(std::vector< std::unique_ptr< ASTtree< GUM_SCALAR > > >&& lterms) {
    if (lterms.empty()) { GUM_ERROR(InvalidArgument, "productOfTrees requires at least one term"); }

    auto it  = lterms.begin();
    auto end = lterms.end();

    // Take the first term as accumulator
    std::unique_ptr< ASTtree< GUM_SCALAR > > acc = std::move(*it);
    ++it;

    // Left-associate: (((t1 * t2) * t3) * ...)
    for (; it != end; ++it) {
      acc = std::make_unique< ASTmult< GUM_SCALAR > >(std::move(acc), std::move(*it));
    }
    return acc;
  }

}   // namespace gum
