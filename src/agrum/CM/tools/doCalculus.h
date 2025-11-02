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

#include <agrum/CM/causalModel.h>
#include <agrum/CM/tools/doAST.h>

namespace gum {

  /**
   * @class DoCalculus
   * @brief Instance-based do-calculus utilities bound to a single CausalModel.
   *
   * This class provides:
   *  - AST builders for standard adjustment formulas:
   *      * Backdoor:  \f$\sum_{Z} P(y \mid x, z)\,P(z)\f$
   *      * Frontdoor: \f$\sum_{Z} P(z \mid x)\,\sum_{x} P(y \mid x, z)\,P(x)\f$
   *  - Public entry points for general identification (ID/IDC):
   *      * identifyingIntervention(Y, X)
   *      * doCalculus(on, doing)
   *      * doCalculusWithObservation(on, doing, knowing)
   *
   * Returned ASTs are evaluated against the **observational BN** of the bound
   * causal model.
   */
  template < typename GUM_SCALAR >
  class DoCalculus {
    public:
    /// Owned pointer to an AST representing a probability expression.
    using FormulaPtr = std::unique_ptr< ASTtree< GUM_SCALAR > >;

    /// Convenience container for name-based overloads.
    using NameList = std::vector< std::string >;

    /* ------------------------ Construction ------------------------ */

    explicit DoCalculus(const CausalModel< GUM_SCALAR >& cm) noexcept : _cm{cm} {}

    inline const CausalModel< GUM_SCALAR >& model() const noexcept { return _cm; }

    /* -------------------- Backdoor / Frontdoor -------------------- */

    FormulaPtr getBackDoorTree(NodeId cause, NodeId effect, const NodeSet& zset) const;

    FormulaPtr getBackDoorTree(const std::string& causeName,
                               const std::string& effectName,
                               const NameList&    zNames) const;

    FormulaPtr getFrontDoorTree(NodeId cause, NodeId effect, const NodeSet& zset) const;

    FormulaPtr getFrontDoorTree(const std::string& causeName,
                                const std::string& effectName,
                                const NameList&    zNames) const;

    /* --------------------- General identification ----------------- */

    /// ID: identify P(Y | do(X)) (NodeId-based).
    FormulaPtr identifyingIntervention(const NodeSet& Y, const NodeSet& X) const;

    /// ID: identify P(Y | do(X)) (name-based).
    FormulaPtr identifyingIntervention(const NameList& Y, const NameList& X) const;

    /// Thin wrapper around ID: doCalculus(on, doing)
    FormulaPtr doCalculus(const NodeSet& on, const NodeSet& doing) const;
    FormulaPtr doCalculus(const NameList& on, const NameList& doing) const;

    /// IDC: identify P(on | do(doing), knowing)
    FormulaPtr doCalculusWithObservation(const NodeSet& on,
                                         const NodeSet& doing,
                                         const NodeSet& knowing) const;
    FormulaPtr doCalculusWithObservation(const NameList& on,
                                         const NameList& doing,
                                         const NameList& knowing) const;

    private:
    const CausalModel< GUM_SCALAR >& _cm;

    /* ---------- Helpers for ID/IDC (DAG-only; never mutate CausalModel) ---------- */

    /// c-decomposition (confounding components) among observed nodes.
    std::vector< NodeSet > _cDecomposition_(const CausalModel< GUM_SCALAR >& cm) const;

    /// c-decomposition on an induced submodel (observed nodes only).
    std::vector< NodeSet > _cDecompositionOn_(const CausalModel< GUM_SCALAR >& sub) const;

    /// Topological order over observed nodes (ignoring latent parents).
    std::vector< NodeId > _topoObserved_(const CausalModel< GUM_SCALAR >& cm) const;

    /// Ancestors (in DAG g) of a set T (including T).
    static NodeSet _ancestorsIn_(const DAG& g, const NodeSet& T);

    /// DAG copy with all incoming arcs into X removed (G_{\overline X}).
    static DAG _removeIncomingInto_(const DAG& dag, const NodeSet& X);

    /// IDC helper: DAG copy with incoming into doing removed AND outgoing from knowing removed.
    static DAG _removeInIntoDoing_outOfKnowing_(const DAG&     dag,
                                                const NodeSet& doing,
                                                const NodeSet& knowing);

    /// ID core (recursive), with optional accumulated distribution P (AST) to carry in
    /// decompositions.
    FormulaPtr _ID_(const CausalModel< GUM_SCALAR >&         cm,
                    const NodeSet&                           Y,
                    const NodeSet&                           X,
                    std::unique_ptr< ASTtree< GUM_SCALAR > > P) const;

    /// Format a hedge exception message.
    static std::string _hedgeExceptionMsg(const CausalModel< GUM_SCALAR >& cm,
                                          const NodeSet&                   Y,
                                          const NodeSet&                   X,
                                          const NodeSet&                   V,
                                          const NodeSet&                   S);
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class DoCalculus< double >;
#endif

}   // namespace gum

#include <agrum/CM/tools/doCalculus_tpl.h>

#endif   // GUM_DO_CALCULUS_H
