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
#ifndef GUM_CAUSAL_IMPACT_H
#define GUM_CAUSAL_IMPACT_H

#include <string>
#include <vector>
#include <memory>

#include <agrum/CM/causalModel.h>
#include <agrum/CM/tools/separation.h>
#include <agrum/CM/tools/doCalculus.h>
#include <agrum/CM/tools/doorCriteria.h>
#include <agrum/CM/tools/doAST.h>
#include <agrum/CM/tools/causalFormula.h>
#include <agrum/CM/tools/hedgeException.h>

namespace gum {

using NameSet           = Set<std::string>;
using VariableName      = std::string;
using VariableValueName = std::string;
using VariableValueId   = Idx;

/**
 * @class CausalImpact
 * @brief Builds a CausalFormula for a query (d-sep -> backdoor -> frontdoor -> (optional) do-calculus).
 *
 * With your CausalFormula’s API (no default ctor, non-assignable), we compute the
 * AST first and initialize `result` in the initializer list (single shot).
 */
template <typename GUM_SCALAR>
class CausalImpact {
public:
  /// Identified symbolic formula. If not identifiable, its AST may be nullptr and
  /// its explanation string (6th ctor arg) will say so.
  CausalFormula<GUM_SCALAR> result;

  /// If true, skip backdoor/frontdoor and use do-calculus directly
  bool directDoCalculus_ = false;

  // --- Forwarded accessors for wrapping ---
  /// @brief Evaluates the formula's AST to compute the resulting probability distribution.
  Tensor<GUM_SCALAR> eval() const { return result.eval(); }

  /// @brief Generates a string representation of the formula's AST.
  std::string toString() const { return result.toString(); }

  /// @brief Generates a full LaTeX equation: Query = Formula.
  std::string toLatex() const { return result.toLatex(); }

  /**
   * @brief Generates a LaTeX representation of the original query, e.g., P(Y | do(X), Z).
   * @note This version does not yet support specific variable values.
   */
  std::string latexQuery() const { return result.latexQuery(); }

  /**
   * @brief Whether the causal effect has been identified.
   *
   * Returns true iff this formula has a valid AST (i.e., identification
   * succeeded and evaluation is possible). When this returns false,
   * calling root() or eval() will throw.
   *
   * @return true if an identification AST exists, false otherwise.
   */
  bool isIdentified() const noexcept { return result.isIdentified(); }

  /**
   * @brief Access the root AST node of the identified formula.
   *
   * Use this to inspect the symbolic structure of the identified effect.
   * @warning This raises an exception if the effect is not identified.
   *
   * @throws gum::OperationNotAllowed if no AST is available (i.e., the effect
   *         is not identifiable with the current methods).
   * @return const ASTtree<GUM_SCALAR>& reference to the AST root.
   */
  const ASTtree<GUM_SCALAR>& root() const { return result.root(); }

  // --- Accessors ---
  const CausalModel<GUM_SCALAR>& cm() const { return result.cm(); }
  const NodeSet& on() const { return result.on(); }
  const NodeSet& doing() const { return result.doing(); }
  const NodeSet& knowing() const { return result.knowing(); }
  const std::string& explanation() const { return result.explanation(); }

  /// Convenience: return names corresponding to stored node ids (sorted).
  std::vector<std::string> onNames() const { return result.onNames(); }
  std::vector<std::string> doingNames() const { return result.doingNames(); }
  std::vector<std::string> knowingNames() const { return result.knowingNames(); }


  // Names interface
  /**
   * @brief Constructs a CausalImpact object using variable names.
   *
   * @param cm The causal model to use for identification.
   * @param on The set of variable names representing the outcome(s) of interest.
   * @param doing The set of variable names representing the intervention(s) (do-variables).
   * @param knowing The set of variable names representing observed covariates (optional).
   * @param values A hashtable mapping variable names to their specific values (optional).
   * @param directDoCalculus If true, bypasses Backdoor and Frontdoor and directly uses the ID algorithm for identification.
   * @warning Setting directDoCalculus = true is NOT advisable. This will bypass the Backdoor and Frontdoor criteria and use the ID algorithm directly.
   */
  CausalImpact(const CausalModel<GUM_SCALAR>& cm,
               const NameSet&                 on,
               const NameSet&                 doing,
               const NameSet&                 knowing = NameSet{},
               const HashTable<VariableName, VariableValueName>& /*values*/ = HashTable<VariableName, VariableValueName>(),
               bool directDoCalculus = false);

  // IDs interface
  /**
   * @brief Constructs a CausalImpact object using node IDs.
   *
   * @param cm The causal model to use for identification.
   * @param on The set of node IDs representing the outcome(s) of interest.
   * @param doing The set of node IDs representing the intervention(s) (do-variables).
   * @param knowing The set of node IDs representing observed covariates (optional).
   * @param values A hashtable mapping node IDs to their specific values (optional).
   * @param directDoCalculus If true, bypasses Backdoor and Frontdoor and directly uses the ID algorithm for identification.
   * @warning Setting directDoCalculus = true is NOT advisable. This will bypass the Backdoor and Frontdoor criteria and use the ID algorithm directly.
   */
  CausalImpact(const CausalModel<GUM_SCALAR>&            cm,
               const NodeSet&                            on,
               const NodeSet&                            doing,
               const NodeSet&                            knowing = NodeSet{},
               const HashTable<NodeId, VariableValueId>& /*values*/ = HashTable<NodeId, VariableValueId>(),
               bool directDoCalculus = false);

private:
  // helpers
  static NameSet _idsToNames_(const CausalModel<GUM_SCALAR>& cm, const NodeSet& ids);
  static NodeSet _namesToIds_(const CausalModel<GUM_SCALAR>& cm, const NameSet& names);
  static bool    _disjoint_(const NameSet& a, const NameSet& b, const NameSet& c);

  // core builders that return the fully-formed formula (for initializer list)
  static CausalFormula<GUM_SCALAR>
  _buildFromNames_(const CausalModel<GUM_SCALAR>& cm,
                   const NameSet&                 on,
                   const NameSet&                 doing,
                   const NameSet&                 knowing,
                   bool directDoCalculus);

  static CausalFormula<GUM_SCALAR>
  _buildFromIds_(const CausalModel<GUM_SCALAR>& cm,
                 const NodeSet&                 on,
                 const NodeSet&                 doing,
                 const NodeSet&                 knowing,
                 bool directDoCalculus);
};

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class CausalImpact<double>;
#endif

} // namespace gum

#include <agrum/CM/causalImpact_tpl.h>

#endif // GUM_CAUSAL_IMPACT_H
