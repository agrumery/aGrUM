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
 * @brief Do-calculus utilities and AST builders bound to a single CausalModel.
 *
 * This class mirrors the layout of pyAgrum's `_doCalculus.py`: it provides
 * helpers to construct adjustment formulas (backdoor/frontdoor) as ASTs and
 * the public entry points for general identification à la Shpitser–Pearl.
 */
#ifndef GUM_DO_CALCULUS_H
#define GUM_DO_CALCULUS_H

#include <memory>
#include <string>
#include <vector>

#include <agrum/base/graphs/DAG.h>         // NodeId, NodeSet
#include <agrum/CM/causalModel.h>          // CausalModel<GUM_SCALAR>
#include <agrum/CM/doAST.h>                // ASTtree<GUM_SCALAR> and nodes

namespace gum {

/**
 * @class DoCalculus
 * @brief Instance-based do-calculus utilities bound to a single CausalModel.
 *
 * This class provides:
 *  - AST builders for standard adjustment formulas:
 *      * Backdoor:  \f$\sum_{Z} P(y \mid x, z)\,P(z)\f$
 *      * Frontdoor: \f$\sum_{Z} P(z \mid x)\,\sum_{x} P(y \mid x, z)\,P(x)\f$
 *  - Public entry points for general identification:
 *      * identifyingIntervention(Y, X)
 *      * doCalculus(on, doing)
 *      * doCalculusWithObservation(on, doing, knowing)
 *
 * The returned ASTs are **evaluated against the observational BN** of the
 * bound causal model (i.e., \c model().observationalBN()).
 *
 * **Naming correspondences with pyAgrum:**
 *  - getBackDoorTree            ⇨ pyagrum.causal._doCalculus.getBackDoorTree
 *  - getFrontDoorTree           ⇨ pyagrum.causal._doCalculus.getFrontDoorTree
 *  - identifyingIntervention    ⇨ pyagrum.causal._doCalculus.identifyingIntervention
 *  - doCalculus                 ⇨ pyagrum.causal._doCalculus.doCalculus
 *  - doCalculusWithObservation  ⇨ pyagrum.causal._doCalculus.doCalculusWithObservation
 *
 * @tparam GUM_SCALAR numeric type used by the underlying BN and tensors.
 */
template <typename GUM_SCALAR>
class DoCalculus {
public:
  /// Owned pointer to an AST representing a probability expression.
  using FormulaPtr = std::unique_ptr<ASTtree<GUM_SCALAR>>;

  /// Convenience container for name-based overloads.
  using NameList = std::vector<std::string>;

  /* ------------------------ Construction ------------------------ */

  /**
   * @brief Construct a DoCalculus helper bound to a causal model.
   * @param cm Causal model (observational BN + causal DAG).
   */
  explicit DoCalculus(const CausalModel<GUM_SCALAR>& cm) noexcept : _cm{cm} {}

  /**
   * @brief Accessor for the bound causal model.
   * @return Const reference to the causal model used by this instance.
   */
  inline const CausalModel<GUM_SCALAR>& model() const noexcept { return _cm; }

  /* -------------------- Backdoor / Frontdoor -------------------- */

  /**
   * @brief Build the backdoor-adjustment AST for a given adjustment set \f$Z\f$.
   *
   * Encodes:
   * \f[
   *   \sum_{Z}\; \mathbb{P}(y \mid x, z)\; \mathbb{P}(z)
   * \f]
   * where \(x=\) \p cause, \(y=\) \p effect, and \(Z=\) \p zset (all **observed**).
   *
   * Notes
   * - This function does **not** verify that \p zset satisfies the backdoor criterion.
   *   Use DoorCriteria first if needed. Latent variables are forbidden in \p zset.
   *
   * @param cause  NodeId of \(x\) (must be observed).
   * @param effect NodeId of \(y\) (must be observed).
   * @param zset   NodeIds of adjustment set \(Z\) (observed only).
   * @return Owned AST representing the backdoor formula.
   * @throw InvalidArgument if \p cause/\p effect are latent, or \p zset contains a latent.
   * @throw NotFound if any NodeId does not belong to the model.
   */
  FormulaPtr getBackDoorTree(NodeId cause, NodeId effect, const NodeSet& zset) const;

  /**
   * @brief Name-based overload of getBackDoorTree.
   * @param causeName  name of \(x\)
   * @param effectName name of \(y\)
   * @param zNames     names of \(Z\)
   * @return Owned AST representing the backdoor formula.
   * @throw InvalidArgument / NotFound on invalid names or latent usage.
   */
  FormulaPtr getBackDoorTree(const std::string& causeName,
                             const std::string& effectName,
                             const NameList&    zNames) const;

  /**
   * @brief Build the frontdoor-adjustment AST for a given mediator set \f$Z\f$.
   *
   * Encodes:
   * \f[
   *   \sum_{Z}\; \mathbb{P}(z \mid x)\;\; \sum_{x}\; \mathbb{P}(y \mid x, z)\; \mathbb{P}(x)
   * \f]
   * where \(x=\) \p cause, \(y=\) \p effect, and \(Z=\) \p zset (all **observed**).
   *
   * Notes
   * - This function does **not** verify that \p zset satisfies the frontdoor criterion.
   *   Use DoorCriteria first if needed. Latent variables are forbidden in \p zset.
   * - \p zset must be **non-empty** for a meaningful frontdoor adjustment.
   *
   * @param cause  NodeId of \(x\) (must be observed).
   * @param effect NodeId of \(y\) (must be observed).
   * @param zset   NodeIds of mediator set \(Z\) (observed; non-empty).
   * @return Owned AST representing the frontdoor formula.
   * @throw InvalidArgument if \p cause/\p effect are latent, or \p zset contains a latent, or \p zset is empty.
   * @throw NotFound if any NodeId does not belong to the model.
   */
  FormulaPtr getFrontDoorTree(NodeId cause, NodeId effect, const NodeSet& zset) const;

  /**
   * @brief Name-based overload of getFrontDoorTree.
   * @param causeName  name of \(x\)
   * @param effectName name of \(y\)
   * @param zNames     names of \(Z\) (non-empty)
   * @return Owned AST representing the frontdoor formula.
   * @throw InvalidArgument / NotFound on invalid names or latent usage.
   */
  FormulaPtr getFrontDoorTree(const std::string& causeName,
                              const std::string& effectName,
                              const NameList&    zNames) const;

  /* --------------------- General identification ----------------- */

  /**
   * @brief Identify \f$P(Y \mid do(X))\f$ via do-calculus (Shpitser–Pearl).
   *
   * @param Y Set of **observed** query nodes.
   * @param X Set of **observed** intervention nodes.
   * @return Owned AST representing an identified expression for \f$P(Y \mid do(X))\f$.
   * @warning May throw when the effect is not identifiable in the model (hedge), or
   *          when this branch is not implemented yet.
   * @throw InvalidArgument if \p Y or \p X contains latent nodes or unknown ids.
   * @throw NotFound if any NodeId does not belong to the model.
   *
   * @note This is the C++ counterpart of
   *       `pyagrum.causal._doCalculus.identifyingIntervention(cm, Y, X)`.
   */
  FormulaPtr identifyingIntervention(const NodeSet& Y, const NodeSet& X) const;

  /**
   * @brief Name-based overload of identifyingIntervention.
   * @param Y Names of query variables.
   * @param X Names of intervention variables.
   * @return Owned AST representing an identified expression for \f$P(Y \mid do(X))\f$.
   */
  FormulaPtr identifyingIntervention(const NameList& Y, const NameList& X) const;

  /**
   * @brief Compute an identified AST for \f$P(\text{on} \mid do(\text{doing}))\f$.
   *
   * Thin wrapper around identifyingIntervention with friendlier parameter names.
   *
   * @param on    Set of **observed** query nodes.
   * @param doing Set of **observed** intervention nodes.
   * @return Owned AST representing \f$P(\text{on} \mid do(\text{doing}))\f$ if identifiable.
   * @throw Same as identifyingIntervention.
   */
  FormulaPtr doCalculus(const NodeSet& on, const NodeSet& doing) const;

  /// Name-based overload of doCalculus.
  FormulaPtr doCalculus(const NameList& on, const NameList& doing) const;

  /**
   * @brief Compute an identified AST for \f$P(\text{on} \mid do(\text{doing}), \text{knowing})\f$.
   *
   * Mirrors pyAgrum's `doCalculusWithObservation`. Internally, this typically
   * reduces to a ratio of two identified ASTs:
   * \f$ P(\text{on} \cup \text{knowing} \mid do(\text{doing})) / P(\text{knowing} \mid do(\text{doing})) \f$,
   * plus optional graph reductions.
   *
   * @param on      Set of **observed** query nodes.
   * @param doing   Set of **observed** intervention nodes.
   * @param knowing Set of **observed** conditioned nodes (may be empty).
   * @return Owned AST representing the conditional interventional distribution.
   * @throw InvalidArgument / NotFound on invalid inputs; may throw if not yet implemented.
   */
  FormulaPtr doCalculusWithObservation(const NodeSet& on,
                                       const NodeSet& doing,
                                       const NodeSet& knowing) const;

  /// Name-based overload of doCalculusWithObservation.
  FormulaPtr doCalculusWithObservation(const NameList& on,
                                       const NameList& doing,
                                       const NameList& knowing) const;

private:
  /// Bound causal model (observational BN + causal DAG + latent bookkeeping).
  const CausalModel<GUM_SCALAR>& _cm;

  // --- Internal helpers (not part of the public API) ---
  // You can keep recursive/accumulator variants private in the .cpp, e.g.:
  // FormulaPtr _identifyingInterventionImpl(const NodeSet& Y,
  //                                         const NodeSet& X,
  //                                         const ASTtree<GUM_SCALAR>* P) const;
};

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class DoCalculus<double>;
#endif

} // namespace gum

#include <agrum/CM/doCalculus_tpl.h>

#endif // GUM_DO_CALCULUS_H
