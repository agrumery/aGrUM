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
 * @brief Represents an identified causal formula and its query context.
 */

#ifndef GUM_CAUSAL_FORMULA_H
#define GUM_CAUSAL_FORMULA_H

#include <memory>
#include <string>
#include <vector>

#include <agrum/CM/causalModel.h>
#include <agrum/CM/tools/doAST.h>

namespace gum {

  /**
   * @class CausalFormula
   * @brief A container for an identified causal query.
   *
   * This class binds an identified formula (as an `ASTtree`) to the context
   * of the query that produced it: the `CausalModel` and the sets of variables
   * for `on`, `doing`, and `knowing`.
   *
   * It provides methods to:
   * - Evaluate the formula to get a numeric `Tensor` (`eval`).
   * - Display the formula as a string (`toString`) or LaTeX (`toLatex`).
   * - Display the original query that generated the formula (`latexQuery`).
   *
   * @tparam GUM_SCALAR The numeric type for tensor computations.
   */
  template < GUM_Numeric GUM_SCALAR >
  class CausalFormula {
    public:
    /**
     * @brief Constructs a CausalFormula object (variables given by names).
     *
     * @param cm The causal model against which the formula was identified.
     * @param root The identified formula as an Abstract Syntax Tree (ownership moved).
     * @param on The set of outcome variable names (the "Y" in P(Y|...)).
     * @param doing The set of intervention variable names (the "do(X)")).
     * @param knowing The set of conditioning variable names (optional evidence).
     * @param explanation A human-readable explanation of the identification method used (optional).
     */
    CausalFormula(const CausalModel< GUM_SCALAR >&         cm,
                  std::unique_ptr< ASTtree< GUM_SCALAR > > root,
                  const Set< std::string >&                on,
                  const Set< std::string >&                doing,
                  const Set< std::string >&                knowing     = Set< std::string >{},
                  std::string_view                         explanation = "");

    /**
     * @brief Constructs a CausalFormula object (variables given by node ids).
     *
     * @param cm The causal model against which the formula was identified.
     * @param root The identified formula as an Abstract Syntax Tree (ownership moved).
     * @param on The set of outcome variable ids (the "Y" in P(Y|...)).
     * @param doing The set of intervention variable ids (the "do(X)")).
     * @param knowing The set of conditioning variable ids (optional evidence).
     * @param explanation A human-readable explanation of the identification method used (optional).
     */
    CausalFormula(const CausalModel< GUM_SCALAR >&         cm,
                  std::unique_ptr< ASTtree< GUM_SCALAR > > root,
                  const NodeSet&                           on,
                  const NodeSet&                           doing,
                  const NodeSet&                           knowing     = NodeSet{},
                  std::string_view                         explanation = "");

    /// @brief Evaluates the formula's AST to compute the resulting probability distribution.
    Tensor< GUM_SCALAR > eval() const;

    /// @brief Generates a string representation of the formula's AST.
    std::string toString() const;

    /// @brief Generates a full LaTeX equation: Query = Formula.
    std::string toLatex(std::string_view doOperatorPrefix = "do(",
                        std::string_view doOperatorSuffix = ")") const;

    /**
     * @brief Generates a LaTeX representation of the original query, e.g., P(Y | do(X), Z).
     * @note This version does not yet support specific variable values.
     */
    std::string latexQuery(std::string_view doOperatorPrefix = "do(",
                           std::string_view doOperatorSuffix = ")") const;

    /// @brief Creates a deep copy of the CausalFormula (including its AST).
    std::unique_ptr< CausalFormula< GUM_SCALAR > > copy() const;

    /**
     * @brief Whether the causal effect has been identified.
     *
     * Returns true iff this formula has a valid AST (i.e., identification
     * succeeded and evaluation is possible). When this returns false,
     * calling root() or eval() will throw.
     *
     * @return true if an identification AST exists, false otherwise.
     */
    bool isIdentified() const noexcept;

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
    const ASTtree< GUM_SCALAR >& root() const;

    // --- Accessors ---
    const CausalModel< GUM_SCALAR >& cm() const { return _cm; }

    const NodeSet& on() const { return _on; }

    const NodeSet& doing() const { return _doing; }

    const NodeSet& knowing() const { return _knowing; }

    const std::string& explanation() const { return _explanation; }

    /// Convenience: return names corresponding to stored node ids (sorted).
    std::vector< std::string > onNames() const;
    std::vector< std::string > doingNames() const;
    std::vector< std::string > knowingNames() const;


    private:
    /// @brief Convert a set of names to a set of node ids (validates existence).
    static NodeSet _toNodeSetFromNames_(const CausalModel< GUM_SCALAR >& cm,
                                        const Set< std::string >&        names);

    /**
     * @brief Verifies that all variables (_on, _knowing, _doing) exist in the BN.
     * @throw gum::NotFound if any variable id is not found.
     */
    void _ensureVariablesExist() const;

    /**
     * @brief Verifies that all variables (_on, _knowing, _doing) have no intersection.
     * @throw gum::OperationNotAllowed if any variable appears in two roles.
     */
    void _ensureNoVariablesOverlap() const;

    const CausalModel< GUM_SCALAR >&         _cm;
    std::unique_ptr< ASTtree< GUM_SCALAR > > _root;
    const NodeSet                            _on;
    const NodeSet                            _doing;
    const NodeSet                            _knowing;
    const std::string                        _explanation;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class CausalFormula< double >;
#endif

}   // namespace gum

#include <agrum/CM/tools/causalFormula_tpl.h>

#endif   // GUM_CAUSAL_FORMULA_H
