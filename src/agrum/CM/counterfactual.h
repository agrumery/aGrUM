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
#ifndef GUM_COUNTERFACTUAL_H
#define GUM_COUNTERFACTUAL_H

#include <ostream>
#include <string>
#include <memory>

#include <agrum/CM/causalImpact.h>


namespace gum {

using NameSet           = Set<std::string>;
using VariableName      = std::string;
using VariableValueName = std::string;
using VariableValueId   = Idx;

/**
 * @class Counterfactual
 * @brief Computes a counterfactual distribution by building a twin model, then
 *        evaluating a causal effect on that twin and adapting the result back
 *        to the original model’s variables.
 *
 * High-level workflow:
 *  1. Build the **twin** causal model with ::counterFactualModel:
 *     - Start from the observational BN.
 *     - Identify *idiosyncratic* variables (parentless nodes) and remove any
 *       that are in `whatif` or latent.
 *     - Run inference on the original BN with the optional `profile` evidence
 *       and replace the priors of idiosyncratic variables in the twin by their
 *       posteriors from the original BN.
 *  2. On the twin model, construct a `CausalImpact` for the query
 *     P(on | do(whatif), …) and **evaluate** it numerically.
 *  3. If `values` is provided, **slice** the numeric tensor using only the
 *     assignments present in the result (safe partial instantiation).
 *  4. **Adapt** the evaluated tensor back to the original model by rebuilding a
 *     tensor with the same variable names (and order) as produced by the twin
 *     evaluation, but bound to the original BN’s variables.
 *
 * Constructors:
 *  - Names API:
 *      Counterfactual(cm, on, whatif, profile, values)
 *    where `on` and `whatif` are sets of variable names; `profile` and `values`
 *    are name→label assignments. `profile` is used as observational evidence
 *    when computing posteriors for the idiosyncratic roots; `values` gives the
 *    specific do-assignments when evaluating the effect and for optional
 *    numeric slicing.
 *  - IDs API:
 *      Counterfactual(cm, onIds, whatifIds, profileIds, valuesIds)
 *    same semantics, using node/value identifiers.
 *
 * Results:
 *  - `value()` returns the numeric tensor adapted to the original model’s
 *    variables (order mirrors the twin evaluation’s variable sequence).
 *  - `getResult()` exposes the symbolic `CausalFormula` produced on the twin
 *    model (variable names match those of the original BN).
 *
 * Notes:
 *  - `profile` and `values` can be empty.
 *  - `values` is **not** required to build the formula; it is used for the twin
 *    evaluation and optional numeric slicing.
 *  - Exceptions from evidence insertion, inference, CPT replacement and tensor
 *    building may propagate (e.g., unknown variable/label names, inconsistent
 *    domains).
 *
 * @tparam GUM_SCALAR Numeric type used in potentials (e.g., double).
 */
template <typename GUM_SCALAR>
class Counterfactual {
public:
  using VarName = VariableName;
  using ValName = VariableValueName;

  // ---------- Names-based constructor ----------
  /**
   * @brief Construct a counterfactual query using variable **names**.
   *
   * Builds a twin model from @p cm using @p profile as observational evidence
   * to update idiosyncratic roots, then evaluates the causal effect
   * P(on | do(whatif), …). If @p values is provided, the evaluated tensor is
   * optionally sliced by these assignments (only for variables present in the
   * result), and finally adapted to the original BN’s variables.
   *
   * @param cm       Source causal model (provides the observational BN).
   * @param on       Set of variable names to query in the effect.
   * @param whatif   Set of variable names intervened by do(·).
   * @param profile  Name→label evidence used when computing posteriors in the
   *                 original BN for idiosyncratic roots. May be empty.
   * @param values   Name→label assignments for the intervention values to use
   *                 during numeric evaluation and optional slicing. May be empty.
   */
  Counterfactual(const CausalModel<GUM_SCALAR>& cm,
                 const NameSet& on                               = NameSet(),
                 const NameSet& whatif                           = NameSet(),
                 const HashTable<VarName, ValName>& profile      = HashTable<VarName, ValName>(),
                 const HashTable<VarName, ValName>& values       = HashTable<VarName, ValName>());

  // ---------- IDs-based constructor (NodeSet overload) ----------
    /**
     * @brief Construct a counterfactual query using **IDs** (nodes/value IDs).
     *
     * Same semantics as the names-based constructor, but using node identifiers
     * and value identifiers directly.
     *
     * @param cm         Source causal model.
     * @param onIds      Node IDs to query in the effect.
     * @param whatifIds  Node IDs to intervene on via do(·).
     * @param profileIds NodeId→ValueId evidence for posterior update of
     *                   idiosyncratic roots. May be empty.
     * @param valuesIds  NodeId→ValueId assignments for the intervention values
     *                   during numeric evaluation and optional slicing. May be empty.
     */
  Counterfactual(const CausalModel<GUM_SCALAR>& cm,
                 const NodeSet& onIds,
                 const NodeSet& whatifIds,
                 const HashTable<NodeId, VariableValueId>& profileIds = HashTable<NodeId, VariableValueId>(),
                 const HashTable<NodeId, VariableValueId>& valuesIds  = HashTable<NodeId, VariableValueId>());

  // ---------- Twin builders (overloaded like CausalImpact) ----------
  static CausalModel<GUM_SCALAR>
  counterFactualModel(const CausalModel<GUM_SCALAR>& cm,
                      const HashTable<VarName, ValName>& profile,
                      const NameSet& whatif);

  static CausalModel<GUM_SCALAR>
  counterFactualModel(const CausalModel<GUM_SCALAR>& cm,
                      const HashTable<NodeId, VariableValueId>& profileIds,
                      const NodeSet& whatifIds);

  /// Rebuild the internal CausalImpact (ctor already calls this).
  void run();

  // ---------- Accessors ----------
  const CausalModel<GUM_SCALAR>&       originalModel()  const { return _cm; }
  const CausalModel<GUM_SCALAR>&       twinModel()      const { return _twin; }
    /**
     * @brief Symbolic result produced on the twin model.
     *
     * Exposes the `CausalFormula` computed by the internal `CausalImpact` built
     * over the twin model. Variable names follow those of the original BN.
     *
     * @return const CausalFormula<GUM_SCALAR>& The causal formula.
     */
  const CausalFormula<GUM_SCALAR>&     getResult()      const { return _ciResult(); }
    /**
     * @brief Numeric result adapted to the original model’s variables.
     *
     * The tensor’s variable sequence mirrors that of the evaluated effect on the
     * twin, but each variable object belongs to the original BN (so downstream
     * code may use original variable references).
     *
     * @return const Tensor<GUM_SCALAR>& The adapted numeric tensor. It may be
     *         empty (nbrDim()==0) if the effect is not identifiable or if
     *         evaluation failed.
     */
  const Tensor<GUM_SCALAR>&            value()          const { return _adaptedValue; }

  const NameSet&                       on()             const { return _on; }
  const NameSet&                       whatif()         const { return _whatif; }
  const HashTable<VarName,ValName>&    profile()        const { return _profile; }
  const HashTable<VarName,ValName>&    values()         const { return _values; }

  void print(std::ostream& os) const;

private:
  static Tensor<GUM_SCALAR>
  _adaptToOriginalVariables_(const Tensor<GUM_SCALAR>& adj,
                             const CausalModel<GUM_SCALAR>& cm);

  const CausalFormula<GUM_SCALAR>& _ciResult() const;

  static NameSet _idsToNames_(const CausalModel<GUM_SCALAR>& cm, const NodeSet& ids);
  static HashTable<VarName, ValName>
  _idAssignToNameAssign_(const CausalModel<GUM_SCALAR>& cm, const HashTable<NodeId, VariableValueId>& idAssign);

private:
  const CausalModel<GUM_SCALAR>& _cm;
  NameSet                        _on;
  NameSet                        _whatif;
  HashTable<VarName, ValName>    _profile;
  HashTable<VarName, ValName>    _values;

  CausalModel<GUM_SCALAR>        _twin;
  Tensor<GUM_SCALAR>             _adaptedValue;

  std::unique_ptr< CausalImpact<GUM_SCALAR> > _ci;

};

// ============================================================================
// Standalone helpers (Python-parity)
// ============================================================================

/**
 * \brief Compute a counterfactual distribution using the high-level API.
 *
 * Mirrors the Python function `counterfactual(cm, profile, on, whatif, values)`.
 * Internally constructs a Counterfactual<GUM_SCALAR> instance and returns
 * its evaluated tensor adapted to variables from \p cm.
 *
 * @tparam GUM_SCALAR numeric scalar type (e.g., double)
 * @param cm       Causal model (original, observational world)
 * @param on       Names of variables of interest (can be a singleton)
 * @param whatif   Intervention targets (do-variables) by name
 * @param profile  Optional evidence on original world (name -> label)
 * @param values   Optional intervention values (name -> label). If empty,
 *                 the result spans all values of the \p whatif variables.
 * @return Tensor<GUM_SCALAR> probability distribution over \p on
 *         (variables are those of \p cm).
 */
template <typename GUM_SCALAR>
Tensor<GUM_SCALAR> counterfactual(
  const CausalModel<GUM_SCALAR>& cm,
  const NameSet& on,
  const NameSet& whatif,
  const HashTable<std::string, std::string>& profile = HashTable<std::string,std::string>(),
  const HashTable<std::string, std::string>& values  = HashTable<std::string,std::string>());

/**
 * \brief Build the twin causal model.
 *
 * Mirrors the Python function `counterfactualModel(cm, profile, whatif)`.
 * It computes posteriors of idiosyncratic nodes in the original BN given
 * \p profile and replaces their priors in a cloned model.
 *
 * @tparam GUM_SCALAR numeric scalar type (e.g., double)
 * @param cm       Original causal model
 * @param profile  Optional evidence on the original BN (name -> label)
 * @param whatif   Intervention targets (do-variables) by name
 * @return CausalModel<GUM_SCALAR> the twin causal model
 */
template <typename GUM_SCALAR>
CausalModel<GUM_SCALAR> counterfactualModel(
  const CausalModel<GUM_SCALAR>& cm,
  const HashTable<std::string, std::string>& profile,
  const NameSet& whatif);

}  // namespace gum

#include <agrum/CM/counterfactual_tpl.h>

#endif  // GUM_COUNTERFACTUAL_H

