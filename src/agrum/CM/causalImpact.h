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

#ifndef GUM_CAUSAL_IMPACT_H
#define GUM_CAUSAL_IMPACT_H

#include <memory>
#include <string>
#include <vector>

#include <agrum/CM/tools/causalFormula.h>
#include <agrum/CM/tools/doCalculus.h>
#include <agrum/CM/tools/separation.h>

namespace gum {

  template < GUM_Numeric GUM_SCALAR >
  class CausalImpact;
  template < GUM_Numeric GUM_SCALAR >
  class Tensor;

  /**
   * Standalone helper that mirrors the Python function `causalImpact(...)`.
   * Returns: (symbolic CausalFormula, numeric Tensor, explanation string).
   * If not identifiable, the formula carries a null AST and the tensor is empty.
   */
  template < GUM_Numeric GUM_SCALAR >
  std::tuple< CausalImpact< GUM_SCALAR >, Tensor< GUM_SCALAR >, std::string >
      causalImpact(const CausalModel< GUM_SCALAR >&             cm,
                   const Set< std::string >&                    on,
                   const Set< std::string >&                    doing,
                   const Set< std::string >&                    knowing = Set< std::string >(),
                   const HashTable< std::string, std::string >& values
                   = HashTable< std::string, std::string >());

  /**
   * @class CausalImpact
   * @brief Builds a CausalFormula for a query (d-sep -> backdoor -> frontdoor -> (optional)
   * do-calculus).
   *
   * With your CausalFormula’s API (no default ctor, non-assignable), we compute the
   * AST first and initialize `result` in the initializer list (single shot).
   */
  template < GUM_Numeric GUM_SCALAR >
  class CausalImpact {
    public:
    // --- Forwarded accessors for wrapping ---
    /// @brief Evaluates the formula's AST to compute the resulting probability distribution.
    Tensor< GUM_SCALAR > eval() const { return _resultFormula_.eval(); }

    /// @brief Generates a string representation of the formula's AST.
    [[nodiscard]] std::string toString() const { return _resultFormula_.toString(); }

    /// @brief Generates a full LaTeX equation: Query = Formula.
    [[nodiscard]] std::string toLatex(std::string_view doOperatorPrefix = "do(",
                                      std::string_view doOperatorSuffix = ")") const {
      return _resultFormula_.toLatex(doOperatorPrefix, doOperatorSuffix);
    }

    /**
     * @brief Generates a LaTeX representation of the original query, e.g., P(Y | do(X), Z).
     * @note This version does not yet support specific variable values.
     */
    [[nodiscard]] std::string latexQuery(std::string_view doOperatorPrefix = "do(",
                                         std::string_view doOperatorSuffix = ")") const {
      return _resultFormula_.latexQuery(doOperatorPrefix, doOperatorSuffix);
    }

    /**
     * @brief Whether the causal effect has been identified.
     *
     * Returns true iff this formula has a valid AST (i.e., identification
     * succeeded and evaluation is possible). When this returns false,
     * calling root() or eval() will throw.
     *
     * @return true if an identification AST exists, false otherwise.
     */
    [[nodiscard]] bool isIdentified() const noexcept { return _resultFormula_.isIdentified(); }

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
    const ASTtree< GUM_SCALAR >& root() const { return _resultFormula_.root(); }

    // --- Accessors ---
    /**
     * @brief Access the underlying CausalFormula result.
     * @return const reference to the identified CausalFormula.
     */
    const CausalFormula< GUM_SCALAR >& getResult() const { return _resultFormula_; }

    const CausalModel< GUM_SCALAR >& cm() const { return _resultFormula_.cm(); }

    [[nodiscard]] const NodeSet& on() const { return _resultFormula_.on(); }

    [[nodiscard]] const NodeSet& doing() const { return _resultFormula_.doing(); }

    [[nodiscard]] const NodeSet& knowing() const { return _resultFormula_.knowing(); }

    [[nodiscard]] const std::string& explanation() const { return _resultFormula_.explanation(); }

    /// Convenience: return names corresponding to stored node ids (sorted).
    [[nodiscard]] std::vector< std::string > onNames() const { return _resultFormula_.onNames(); }

    [[nodiscard]] std::vector< std::string > doingNames() const {
      return _resultFormula_.doingNames();
    }

    [[nodiscard]] std::vector< std::string > knowingNames() const {
      return _resultFormula_.knowingNames();
    }

    /**
     * @brief Constructs a CausalImpact object using variable names.
     *
     * @param cm The causal model to use for identification.
     * @param on The set of variable names representing the outcome(s) of interest.
     * @param doing The set of variable names representing the intervention(s) (do-variables).
     * @param knowing The set of variable names representing observed covariates (optional).
     * @param directDoCalculus If true, bypasses Backdoor and Frontdoor and directly uses the ID
     * algorithm for identification.
     * @warning Setting directDoCalculus = true is NOT advisable. This will bypass the Backdoor and
     * Frontdoor criteria and use the ID algorithm directly.
     */
    CausalImpact(const CausalModel< GUM_SCALAR >& cm,
                 const Set< std::string >&        on,
                 const Set< std::string >&        doing,
                 const Set< std::string >&        knowing          = Set< std::string >(),
                 bool                             directDoCalculus = false);

    /**
     * @brief Constructs a CausalImpact object using node IDs.
     *
     * @param cm The causal model to use for identification.
     * @param on The set of node IDs representing the outcome(s) of interest.
     * @param doing The set of node IDs representing the intervention(s) (do-variables).
     * @param knowing The set of node IDs representing observed covariates (optional).
     * @param directDoCalculus If true, bypasses Backdoor and Frontdoor and directly uses the ID
     * algorithm for identification.
     * @warning Setting directDoCalculus = true is NOT advisable. This will bypass the Backdoor and
     * Frontdoor criteria and use the ID algorithm directly.
     */
    CausalImpact(const CausalModel< GUM_SCALAR >& cm,
                 const NodeSet&                   on,
                 const NodeSet&                   doing,
                 const NodeSet&                   knowing          = NodeSet(),
                 bool                             directDoCalculus = false);

    private:
    /// If true, skip backdoor/frontdoor and use do-calculus directly
    bool _directDoCalculus_ = false;

    /// Identified symbolic formula. If not identifiable, its AST may be nullptr and
    /// its explanation string (6th ctor arg) will say so.
    CausalFormula< GUM_SCALAR > _resultFormula_;


    // helpers
    static Set< std::string > _idsToNames_(const CausalModel< GUM_SCALAR >& cm, const NodeSet& ids);
    static NodeSet            _namesToIds_(const CausalModel< GUM_SCALAR >& cm,
                                           const Set< std::string >&        names);
    static bool               _disjoint_(const Set< std::string >& a,
                                         const Set< std::string >& b,
                                         const Set< std::string >& c);

    // core builders that return the fully-formed formula (for initializer list)
    static CausalFormula< GUM_SCALAR > _buildFromNames_(const CausalModel< GUM_SCALAR >& cm,
                                                        const Set< std::string >&        on,
                                                        const Set< std::string >&        doing,
                                                        const Set< std::string >&        knowing,
                                                        bool directDoCalculus);

    static CausalFormula< GUM_SCALAR > _buildFromIds_(const CausalModel< GUM_SCALAR >& cm,
                                                      const NodeSet&                   on,
                                                      const NodeSet&                   doing,
                                                      const NodeSet&                   knowing,
                                                      bool directDoCalculus);
  };

  /**
   * @brief Creates an instance for a tensor based on a HashTable.
   *
   * @param tensor The target tensor whose variables define the instantiation’s scope.
   * @param values Mapping from variable name to label that must be fully applicable.
   * @return A gum::Instantiation positioned according to @p values.
   *
   * @throws NotFound If a variable name in @p values is not a dimension of @p tensor.
   * @throws NotFound If a label in @p values is not in the corresponding variable’s domain.
   */
  template < GUM_Numeric GUM_SCALAR >
  inline Instantiation
      makeInstantiationFromValues(const Tensor< GUM_SCALAR >&                  tensor,
                                  const HashTable< std::string, std::string >& values);


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class CausalImpact< double >;
#endif

}   // namespace gum

#include <agrum/CM/causalImpact_tpl.h>

#endif   // GUM_CAUSAL_IMPACT_H
