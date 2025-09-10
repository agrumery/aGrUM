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
 * @brief Represents an identified causal formula and its query context.
 */

#ifndef GUM_CAUSAL_FORMULA_H
#define GUM_CAUSAL_FORMULA_H

#include <memory>
#include <string>
#include <vector>

#include <agrum/CM/causalModel.h>
#include <agrum/CM/doAST.h>

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
template <typename GUM_SCALAR>
class CausalFormula {
public:
  /// Convenience alias for variable names.
  using NameSet = Set<std::string>;

  /**
   * @brief Constructs a CausalFormula object.
   *
   * @param cm The causal model against which the formula was identified.
   * @param root The identified formula as an Abstract Syntax Tree (ownership is moved).
   * @param on The set of outcome variables (the "Y" in P(Y|...)).
   * @param doing The set of intervention variables (the "do(X)").
   * @param knowing The set of conditioning variables (optional evidence).
   */
  CausalFormula(const CausalModel<GUM_SCALAR>& cm,
                std::unique_ptr<ASTtree<GUM_SCALAR>> root,
                const NameSet& on,
                const NameSet& doing,
                const NameSet& knowing = NameSet{});

  /// @brief Evaluates the formula's AST to compute the resulting probability distribution.
  Tensor<GUM_SCALAR> eval() const;

  /// @brief Generates a string representation of the formula's AST.
  std::string toString() const;

  /// @brief Generates a full LaTeX equation: Query = Formula.
  std::string toLatex() const;

  /**
   * @brief Generates a LaTeX representation of the original query, e.g., P(Y | do(X), Z).
   * @note This version does not yet support specific variable values.
   */
  std::string latexQuery() const;

  /// @brief Creates a deep copy of the CausalFormula (including its AST).
  std::unique_ptr<CausalFormula<GUM_SCALAR>> copy() const;

  // --- Accessors ---
  const CausalModel<GUM_SCALAR>& cm() const { return _cm; }
  const ASTtree<GUM_SCALAR>& root() const { return *_root; }
  const NameSet& on() const { return _on; }
  const NameSet& doing() const { return _doing; }
  const NameSet& knowing() const { return _knowing; }

  // --- Mutators (used by identification algorithms) ---
  void setDoing(const NameSet& newDoing) { _doing = newDoing; }
  void setKnowing(const NameSet& newKnowing) { _knowing = newKnowing; }

private:
  const CausalModel<GUM_SCALAR>& _cm;
  std::unique_ptr<ASTtree<GUM_SCALAR>> _root;
  NameSet _on;
  NameSet _doing;
  NameSet _knowing;
};

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class CausalFormula<double>;
#endif

} // namespace gum

#include <agrum/CM/causalFormula_tpl.h>

#endif // GUM_CAUSAL_FORMULA_H