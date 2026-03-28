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
 * @brief Abstract Syntax Tree (AST) for algebraic probability expressions
 *        used in do-Calculus.
 */

#ifndef DO_AST_H
#define DO_AST_H

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/BN/IBayesNet.h>

#include <string_view>

namespace gum {

  template < GUM_Numeric GUM_SCALAR >
  class IBayesNet;
  template < GUM_Numeric GUM_SCALAR >
  class Tensor;

  // ================================================================
  // ASTtree
  // ================================================================
  /**
   * @class ASTtree
   * @brief Root abstract node for the AST of algebraic expressions.
   *
   * This hierarchy represents symbolic probability expressions (e.g.,
   * \( \mathbb{P}(Y\mid X) \), products, sums) that can be:
   *  - pretty-printed (string / LaTeX),
   *  - structurally copied,
   *  - evaluated into a numeric `Tensor<GUM_SCALAR>` w.r.t. a contextual
   *    `IBayesNet<GUM_SCALAR>`.
   *
   * ### Ownership & semantics
   * - Nodes are move-enabled and non-copyable to avoid expensive deep copies.
   * - `eval` interprets the node in the context of a given BN and returns a
   *   factor/tensor on the relevant variables.
   * - LaTeX helpers maintain name hygiene using `nameOccur` to avoid clashes and
   *   to produce stable, readable notation.
   */
  template < GUM_Numeric GUM_SCALAR >
  class ASTtree {
    public:
    /// Construct an AST node with a descriptive `type` string (used in dumps).
    explicit ASTtree(std::string_view type);

    virtual ~ASTtree() { GUM_DESTRUCTOR(ASTtree) };

    ASTtree(const ASTtree&)            = delete;
    ASTtree& operator=(const ASTtree&) = delete;

    ASTtree(ASTtree&&) noexcept { GUM_CONS_MOV(ASTtree) };

    ASTtree& operator=(ASTtree&&) noexcept = default;

    /// @return the runtime type tag (human-readable)
    const std::string& type() const noexcept { return _type; }

    /**
     * @brief Human-readable multi-line rendering (for debugging / logs).
     * @param prefix indentation prefix propagated to children
     */
    virtual std::string toString(std::string_view prefix = "") const = 0;

    /**
     * @brief LaTeX rendering with full name protection.
     *
     * Produces LaTeX with variable-name disambiguation, using `nameOccur` as a
     * registry of encountered names and their occurrence counts.
     *
     * @param nameOccur symbol occurrence table (updated in place)
     */
    virtual std::string protectToLatex(HashTable< std::string, int >& nameOccur) const = 0;

    /**
     * @brief Fast LaTeX rendering (lighter protection).
     *
     * Assumes prior hygiene of names; typically avoids heavier bookkeeping
     * performed in `protectToLatex`.
     *
     * @param nameOccur symbol occurrence table (may be consulted/updated)
     */
    virtual std::string fastToLatex(HashTable< std::string, int >& nameOccur) const = 0;

    /**
     * @brief Convenience wrapper using an empty occurrence table.
     * @return LaTeX string for the whole sub-tree.
     */
    std::string toLatex(HashTable< std::string, int > nameOccur
                        = HashTable< std::string, int >()) const;

    /// @brief Deep clone of the sub-tree.
    virtual std::unique_ptr< ASTtree< GUM_SCALAR > > copy() const = 0;

    /**
     * @brief Evaluate the expression against a contextual BN.
     * @param contextual_bn Bayesian network providing CPTs and variable domains
     * @return resulting factor/tensor
     */
    virtual Tensor< GUM_SCALAR > eval(const IBayesNet< GUM_SCALAR >& contextual_bn) const = 0;

    protected:
    /// Prefix used to draw tree branches in `toString`.
    static constexpr const char* CONTINUE_PREFIX = "| ";
    std::string                  _type;

    /// @brief Sanitize a single variable name for LaTeX and ensure uniqueness.
    static std::string _latexCorrect(std::string_view               srcName,
                                     HashTable< std::string, int >& nameOccur);
    /// @brief Sanitize a set of variable names for LaTeX and ensure uniqueness.
    static std::vector< std::string > _latexCorrect(const Set< std::string >&      srcNames,
                                                    HashTable< std::string, int >& nameOccur);
  };

  // ================================================================
  // ASTBinaryOp
  // ================================================================
  /**
   * @class ASTBinaryOp
   * @brief Base class for binary algebraic operators (e.g., +, -, ×, ÷).
   *
   * Holds and owns two operand subtrees. Derived nodes implement the actual
   * semantics (`eval`) and LaTeX rendering.
   */
  template < GUM_Numeric GUM_SCALAR >
  class ASTBinaryOp: public ASTtree< GUM_SCALAR > {
    public:
    /**
     * @param typ textual operator tag (e.g., "plus", "mult")
     * @param op1 left operand (owned)
     * @param op2 right operand (owned)
     */
    ASTBinaryOp(std::string_view                         typ,
                std::unique_ptr< ASTtree< GUM_SCALAR > > op1,
                std::unique_ptr< ASTtree< GUM_SCALAR > > op2);

    ASTBinaryOp(const ASTBinaryOp&)                = delete;
    ASTBinaryOp& operator=(const ASTBinaryOp&)     = delete;
    ASTBinaryOp(ASTBinaryOp&&) noexcept            = default;
    ASTBinaryOp& operator=(ASTBinaryOp&&) noexcept = default;

    /// @return left operand
    const ASTtree< GUM_SCALAR >& op1() const { return *_op1; }

    /// @return right operand
    const ASTtree< GUM_SCALAR >& op2() const { return *_op2; }

    /// @copydoc ASTtree::toString
    std::string toString(std::string_view prefix = "") const override;

    protected:
    std::unique_ptr< ASTtree< GUM_SCALAR > > _op1, _op2;
  };

  // ================================================================
  // ASTplus (ASTBinaryOp)
  // ================================================================
  /**
   * @class ASTplus
   * @brief Sum of two AST sub-expressions.
   *
   * LaTeX: renders as \( (\cdot) + (\cdot) \). `eval` computes element-wise sum
   * on aligned tensors (assuming compatible scopes/shapes).
   */
  template < GUM_Numeric GUM_SCALAR >
  class ASTplus: public ASTBinaryOp< GUM_SCALAR > {
    public:
    ASTplus(std::unique_ptr< ASTtree< GUM_SCALAR > > op1,
            std::unique_ptr< ASTtree< GUM_SCALAR > > op2);

    std::string protectToLatex(HashTable< std::string, int >& nameOccur) const override;
    std::string fastToLatex(HashTable< std::string, int >& nameOccur) const override;
    std::unique_ptr< ASTtree< GUM_SCALAR > > copy() const override;
    Tensor< GUM_SCALAR > eval(const IBayesNet< GUM_SCALAR >& contextual_bn) const override;
  };

  // ================================================================
  // ASTminus (ASTBinaryOp)
  // ================================================================
  /**
   * @class ASTminus
   * @brief Difference of two AST sub-expressions (left minus right).
   *
   * LaTeX: renders as \( (\cdot) - (\cdot) \). `eval` computes element-wise
   * subtraction on aligned tensors.
   */
  template < GUM_Numeric GUM_SCALAR >
  class ASTminus: public ASTBinaryOp< GUM_SCALAR > {
    public:
    ASTminus(std::unique_ptr< ASTtree< GUM_SCALAR > > op1,
             std::unique_ptr< ASTtree< GUM_SCALAR > > op2);

    std::string protectToLatex(HashTable< std::string, int >& nameOccur) const override;
    std::string fastToLatex(HashTable< std::string, int >& nameOccur) const override;
    std::unique_ptr< ASTtree< GUM_SCALAR > > copy() const override;
    Tensor< GUM_SCALAR > eval(const IBayesNet< GUM_SCALAR >& contextual_bn) const override;
  };

  // ================================================================
  // ASTmult (ASTBinaryOp)
  // ================================================================
  /**
   * @class ASTmult
   * @brief Elementwise product of two AST sub-expressions.
   *
   */
  template < GUM_Numeric GUM_SCALAR >
  class ASTmult: public ASTBinaryOp< GUM_SCALAR > {
    public:
    ASTmult(std::unique_ptr< ASTtree< GUM_SCALAR > > op1,
            std::unique_ptr< ASTtree< GUM_SCALAR > > op2);

    std::string protectToLatex(HashTable< std::string, int >& nameOccur) const override;
    std::string fastToLatex(HashTable< std::string, int >& nameOccur) const override;
    std::unique_ptr< ASTtree< GUM_SCALAR > > copy() const override;
    Tensor< GUM_SCALAR > eval(const IBayesNet< GUM_SCALAR >& contextual_bn) const override;
  };

  // ================================================================
  // ASTdiv (ASTBinaryOp)
  // ================================================================
  /**
   * @class ASTdiv
   * @brief Elementwise division of two AST sub-expressions (left / right).
   *
   */
  template < GUM_Numeric GUM_SCALAR >
  class ASTdiv: public ASTBinaryOp< GUM_SCALAR > {
    public:
    ASTdiv(std::unique_ptr< ASTtree< GUM_SCALAR > > op1,
           std::unique_ptr< ASTtree< GUM_SCALAR > > op2);

    std::string protectToLatex(HashTable< std::string, int >& nameOccur) const override;
    std::string fastToLatex(HashTable< std::string, int >& nameOccur) const override;
    std::unique_ptr< ASTtree< GUM_SCALAR > > copy() const override;
    Tensor< GUM_SCALAR > eval(const IBayesNet< GUM_SCALAR >& contextual_bn) const override;
  };

  // ================================================================
  // ASTposteriorProba   :  P_bn(vars | knw_min)
  // ================================================================
  /**
   * @class ASTposteriorProba
   * @brief Posterior probability term \( \mathbb{P}_{bn}(\mathrm{vars}\mid\mathrm{knw}) \).
   *
   * Two construction modes are provided:
   *  1. From a BN and raw sets `(vars, knw)`: the constructor performs a minimal
   *     knowledge reduction (removing barren/irrelevant evidence) before storing.
   *  2. From pre-minimalized sets: assumes `knw` is already minimal.
   *
   * `eval` queries the contextual BN and returns the corresponding posterior
   * tensor.
   */
  template < GUM_Numeric GUM_SCALAR >
  class ASTposteriorProba: public ASTtree< GUM_SCALAR > {
    public:
    /// Constructor for \( \mathbb{P}_{bn}(\mathrm{vars}\mid\mathrm{knw}) \); `knw` will be
    /// minimalized using `bn`.
    explicit ASTposteriorProba(const DAGmodel&           bn,
                               const Set< std::string >& vars,
                               const Set< std::string >& knw);

    /// Constructor for \( \mathbb{P}_{bn}(\mathrm{vars}\mid\mathrm{knw}) \); `knw` will be
    /// minimalized using `dag`.
    explicit ASTposteriorProba(const DAG&                              bn,
                               const Bijection< NodeId, std::string >& id2name,
                               const Set< std::string >&               vars,
                               const Set< std::string >&               knw);

    /// Constructor for \( \mathbb{P}_{bn}(\mathrm{vars}\mid\mathrm{knw}) \) with already
    /// minimalized `knw`.
    explicit ASTposteriorProba(const Set< std::string >& vars, const Set< std::string >& knw);

    /// @return names of variables in the left-hand side (the “query” set)
    const Set< std::string >& vars() const noexcept { return _vars; }

    /// @return names in the conditioning set (minimalized)
    const Set< std::string >& knw() const noexcept { return _knw; }

    /// @copydoc ASTtree::toString
    std::string toString(std::string_view prefix = "") const override;
    /// @copydoc ASTtree::protectToLatex
    std::string protectToLatex(HashTable< std::string, int >& nameOccur) const override;
    /// @copydoc ASTtree::fastToLatex
    std::string fastToLatex(HashTable< std::string, int >& nameOccur) const override;

    /// @copydoc ASTtree::copy
    std::unique_ptr< ASTtree< GUM_SCALAR > > copy() const override;
    /// @copydoc ASTtree::eval
    Tensor< GUM_SCALAR > eval(const IBayesNet< GUM_SCALAR >& contextual_bn) const override;

    private:
    Set< std::string > _vars;   //!< names of conditioned variables
    Set< std::string > _knw;    //!< names of conditioning variables (already minimalized)


    static void _ensure_nonempty(const Set< std::string >& vars);

    static Set< std::string > _compute_knw_from_bn(const DAGmodel&           bn,
                                                   const Set< std::string >& vars,
                                                   const Set< std::string >& knw);

    static Set< std::string > _compute_knw_from_dag(const DAG&                              dag,
                                                    const Bijection< NodeId, std::string >& id2name,
                                                    const Set< std::string >&               vars,
                                                    const Set< std::string >&               knw);
  };

  // ================================================================
  // ASTjointProba :  P(vars) in observational BN
  // ================================================================
  /**
   * @class ASTjointProba
   * @brief Joint probability term \( \mathbb{P}(\mathrm{vars}) \) in an observational BN.
   *
   * `eval` queries the contextual BN for the joint over the listed variables.
   */
  template < GUM_Numeric GUM_SCALAR >
  class ASTjointProba: public ASTtree< GUM_SCALAR > {
    public:
    /// Build a joint \(\mathbb{P}(\mathrm{varNames})\).
    explicit ASTjointProba(const Set< std::string >& varNames);

    /// @return variable names in the joint
    const Set< std::string >& varNames() const noexcept { return _varNames; }

    /// @copydoc ASTtree::toString
    std::string toString(std::string_view prefix = "") const override;
    /// @copydoc ASTtree::protectToLatex
    std::string protectToLatex(HashTable< std::string, int >& nameOccur) const override;
    /// @copydoc ASTtree::fastToLatex
    std::string fastToLatex(HashTable< std::string, int >& nameOccur) const override;

    /// @copydoc ASTtree::copy
    std::unique_ptr< ASTtree< GUM_SCALAR > > copy() const override;
    /// @copydoc ASTtree::eval
    Tensor< GUM_SCALAR > eval(const IBayesNet< GUM_SCALAR >& contextual_bn) const override;

    private:
    Set< std::string > _varNames;
  };

  // ================================================================
  // ASTsum  :  sum out over variable of sub-term
  // ================================================================
  /**
   * @class ASTsum
   * @brief Summation (marginalization) over one or more variables.
   *
   * Represents \( \sum_{v} \text{term} \) or a nested version for several
   * variables. Operationally, `eval` marginalizes the variable(s) from the
   * evaluated subterm tensor.
   */
  template < GUM_Numeric GUM_SCALAR >
  class ASTsum: public ASTtree< GUM_SCALAR > {
    public:
    /// Single-variable summation \( \sum_{\text{var}} \text{term} \).
    ASTsum(std::string_view var, std::unique_ptr< ASTtree< GUM_SCALAR > > term);

    /**
     * @brief Multi-variable overload: recursively builds nested sums in the
     *        order of `vars[0], vars[1], ...`.
     */
    ASTsum(const std::vector< std::string >& vars, std::unique_ptr< ASTtree< GUM_SCALAR > > term);

    /// @return the subterm being summed over
    const ASTtree< GUM_SCALAR >& term() const { return *_term; }

    /// @copydoc ASTtree::toString
    std::string toString(std::string_view prefix = "") const override;
    /// @copydoc ASTtree::protectToLatex
    std::string protectToLatex(HashTable< std::string, int >& nameOccur) const override;
    /// @copydoc ASTtree::fastToLatex
    std::string fastToLatex(HashTable< std::string, int >& nameOccur) const override;

    /// @copydoc ASTtree::copy
    std::unique_ptr< ASTtree< GUM_SCALAR > > copy() const override;
    /// @copydoc ASTtree::eval
    Tensor< GUM_SCALAR > eval(const IBayesNet< GUM_SCALAR >& contextual_bn) const override;

    private:
    std::string                              _var;    //!< variable to eliminate
    std::unique_ptr< ASTtree< GUM_SCALAR > > _term;   //!< sub-expression
  };

  // ================================================================
  // productOfTrees  (utility function)
  // ================================================================
  /**
   * @brief Build a product AST from a list of terms.
   *
   * Consumes the vector of unique_ptrs and returns a single AST representing
   * their Elementwise product (e.g., left-associated chain of `ASTmult` nodes).
   *
   * @param lterms list of term nodes (ownership transferred)
   * @return product AST
   */
  template < GUM_Numeric GUM_SCALAR >
  std::unique_ptr< ASTtree< GUM_SCALAR > >
      productOfTrees(std::vector< std::unique_ptr< ASTtree< GUM_SCALAR > > >&& lterms);


// ================================================================
// Extern template declarations
// ================================================================
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class ASTtree< double >;
  extern template class ASTplus< double >;
  extern template class ASTminus< double >;
  extern template class ASTmult< double >;
  extern template class ASTdiv< double >;
  extern template class ASTposteriorProba< double >;
  extern template class ASTjointProba< double >;
  extern template class ASTsum< double >;
  extern template std::unique_ptr< ASTtree< double > >
      productOfTrees< double >(std::vector< std::unique_ptr< ASTtree< double > > >&&);
#endif

}   // namespace gum

#include <agrum/CM/tools/doAST_tpl.h>

#endif   // DO_AST_H
