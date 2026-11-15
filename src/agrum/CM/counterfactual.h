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

#ifndef GUM_COUNTERFACTUAL_H
#define GUM_COUNTERFACTUAL_H

#include <memory>
#include <ostream>
#include <string>

#include <agrum/CM/causalImpact.h>

namespace gum {
  /**
   * @class Counterfactual
   * @brief Computes a counterfactual distribution P(on_{whatif=values} | profile) by inference in
   *        the twin network of Balke and Pearl.
   *
   * Workflow:
   *  1. Build the twin network with counterFactualModel(): the real world (the observational BN)
   *     and the counterfactual world (copies `V*` of the whatif variables and of their
   * descendants), both worlds sharing the other variables, in particular the parentless ones.
   *  2. Infer in the twin network the joint posterior of the copies of `on` and `whatif` given the
   *     `profile` (evidence on the real world). The copies of the whatif variables have no parent
   *     and no observed descendant: dividing by their prior gives P(on* | whatif*, profile). The
   *     abduction is exact: the dependencies created by the profile between the shared variables
   *     are kept.
   *  3. Rename the copies to the variables of the original model and, if `values` is given, slice
   *     the result by these assignments.
   *
   * Constructors:
   *  - Names API: Counterfactual(cm, on, whatif, profile, values), `on` and `whatif` being sets of
   *    variable names, `profile` and `values` name→label assignments.
   *  - IDs API: Counterfactual(cm, onIds, whatifIds, profileIds, valuesIds), same semantics.
   *
   * Notes:
   *  - `profile` and `values` can be empty; without `values`, the result spans all the values of
   * the whatif variables.
   *  - The result is the counterfactual of the structural model only if every variable with parents
   *    is a deterministic function of them (all the uncertainty in the parentless variables).
   *
   * @throw InvalidArgument if the causal model has latent variables, or if a variable is both in
   *        `on` and in `whatif`.
   *
   * @tparam GUM_ELEMENT Numeric type used in potentials (e.g., double).
   */
  template < GUM_Numeric GUM_ELEMENT >
  class Counterfactual {
    public:
    using VarName = std::string;
    using ValName = std::string;

    // ---------- Names-based constructor ----------
    /**
     * @brief Construct a counterfactual query using variable **names**.
     *
     * Builds the twin network of @p cm for @p whatif and computes P(on* | whatif*, profile) in it,
     * sliced by @p values and adapted to the original BN's variables.
     *
     * @param cm       Source causal model (provides the observational BN).
     * @param on       Set of variable names to query.
     * @param whatif   Set of variable names intervened by do(·).
     * @param profile  Name→label evidence observed in the real world. May be empty.
     * @param values   Name→label counterfactual values of the whatif variables, used to slice
     *                 the result. May be empty.
     */
    Counterfactual(const CausalModel< GUM_ELEMENT >&    cm,
                   const Set< std::string >&            on,
                   const Set< std::string >&            whatif,
                   const HashTable< VarName, ValName >& profile = HashTable< VarName, ValName >(),
                   const HashTable< VarName, ValName >& values  = HashTable< VarName, ValName >());

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
     * @param profileIds NodeId→ValueId evidence observed in the real world. May be empty.
     * @param valuesIds  NodeId→ValueId counterfactual values of the whatif variables. May be
     *                   empty.
     */
    Counterfactual(const CausalModel< GUM_ELEMENT >& cm,
                   const NodeSet&                    onIds,
                   const NodeSet&                    whatifIds,
                   const HashTable< NodeId, Idx >&   profileIds = HashTable< NodeId, Idx >(),
                   const HashTable< NodeId, Idx >&   valuesIds  = HashTable< NodeId, Idx >());

    // ---------- Twin network builders ----------
    /**
     * @brief Build the twin network of Balke and Pearl for interventions on @p whatif.
     *
     * The returned model contains the real world (the observational BN of @p cm) and the
     * counterfactual world: a copy `V*` (see copyName()) of each variable of @p whatif and of each
     * of their descendants. A copy of a whatif variable has no parent and a uniform prior; any
     * other copy has the CPT of its original, its parents being the copies of the original parents
     * when they exist, the original parents otherwise. The variables that are not copied, in
     * particular the parentless ones, are shared by both worlds. The counterfactual distribution is
     * a posterior in this network: the profile is the evidence of the real world, the
     * counterfactual values are those of the copies.
     *
     * @param cm     Original causal model.
     * @param whatif Names of the intervened variables.
     * @return CausalModel<GUM_ELEMENT> the twin network.
     * @throw InvalidArgument if @p cm has latent variables (their distributions are unknown) or if
     * the name of a copy is already used in @p cm.
     */
    static CausalModel< GUM_ELEMENT > counterFactualModel(const CausalModel< GUM_ELEMENT >& cm,
                                                          const Set< std::string >&         whatif);

    /// Same as counterFactualModel(cm, whatif), the intervened variables given by their ids.
    static CausalModel< GUM_ELEMENT > counterFactualModel(const CausalModel< GUM_ELEMENT >& cm,
                                                          const NodeSet& whatifIds);

    /// @deprecated deprecated after 3.3.2: the twin network does not depend on the profile, use
    /// counterFactualModel(cm, whatif).
    [[deprecated("Counterfactual::counterFactualModel(cm, profile, whatif) is deprecated after "
                 "3.3.2, use counterFactualModel(cm, whatif)")]]
    static CausalModel< GUM_ELEMENT >
        counterFactualModel(const CausalModel< GUM_ELEMENT >&    cm,
                            const HashTable< VarName, ValName >& profile,
                            const Set< std::string >&            whatif);

    /// @deprecated deprecated after 3.3.2: the twin network does not depend on the profile, use
    /// counterFactualModel(cm, whatifIds).
    [[deprecated("Counterfactual::counterFactualModel(cm, profileIds, whatifIds) is deprecated "
                 "after 3.3.2, use counterFactualModel(cm, whatifIds)")]]
    static CausalModel< GUM_ELEMENT >
        counterFactualModel(const CausalModel< GUM_ELEMENT >& cm,
                            const HashTable< NodeId, Idx >&   profileIds,
                            const NodeSet&                    whatifIds);

    /// The name of the copy of the variable @p name in the twin network (`name*`).
    static std::string copyName(const std::string& name);

    /// Compute the counterfactual distribution by inference in the twin network (the ctor calls
    /// it).
    void run();

    // ---------- Accessors ----------
    const CausalModel< GUM_ELEMENT >& originalModel() const;

    const CausalModel< GUM_ELEMENT >& twinModel() const;

    /**
     * @brief The causal impact P(on* | do(whatif*), profile) in the twin network.
     *
     * Built on demand, it is not used to compute value(): the counterfactual distribution is a
     * posterior in the twin network.
     *
     * @deprecated deprecated after 3.3.2: the counterfactual is computed by inference in the twin
     * network, use twinModel() and value().
     */
    [[deprecated("Counterfactual::impact is deprecated after 3.3.2, use twinModel() and value()")]]
    const CausalImpact< GUM_ELEMENT >& impact() const;

    /**
     * @brief Numeric result adapted to the original model’s variables.
     *
     * P(on | whatif, profile) in the counterfactual world (without the variables sliced by
     * `values`), each variable object belonging to the original BN (so downstream code may use
     * original variable references).
     *
     * @return const Tensor<GUM_ELEMENT>& The adapted numeric tensor.
     */
    const Tensor< GUM_ELEMENT >& value() const;

    const Set< std::string >& on() const;

    const Set< std::string >& whatif() const;

    const HashTable< VarName, ValName >& profile() const;

    const HashTable< VarName, ValName >& values() const;

    std::string toString() const;

    private:
    static Tensor< GUM_ELEMENT > _adaptToOriginalVariables_(const Tensor< GUM_ELEMENT >&      adj,
                                                            const CausalModel< GUM_ELEMENT >& cm);


    static Set< std::string > _idsToNames_(const CausalModel< GUM_ELEMENT >& cm,
                                           const NodeSet&                    ids);
    static HashTable< VarName, ValName >
        _idAssignToNameAssign_(const CausalModel< GUM_ELEMENT >& cm,
                               const HashTable< NodeId, Idx >&   idAssign);

    private:
    const CausalModel< GUM_ELEMENT >& _cm;
    Set< std::string >                _on;
    Set< std::string >                _whatif;
    HashTable< VarName, ValName >     _profile;
    HashTable< VarName, ValName >     _values;

    CausalModel< GUM_ELEMENT > _twin;
    Tensor< GUM_ELEMENT >      _adaptedValue;

    mutable std::unique_ptr< CausalImpact< GUM_ELEMENT > > _ci;
  };

  // ============================================================================
  // Standalone helpers (Python-parity)
  // ============================================================================

  /**
   * \brief Compute a counterfactual distribution using the high-level API.
   *
   * Mirrors the Python function `counterfactual(cm, profile, on, whatif, values)`.
   * Internally constructs a Counterfactual<GUM_ELEMENT> instance and returns
   * its evaluated tensor adapted to variables from \p cm.
   *
   * @tparam GUM_ELEMENT numeric scalar type (e.g., double)
   * @param cm       Causal model (original, observational world)
   * @param on       Names of variables of interest (can be a singleton)
   * @param whatif   Intervention targets (do-variables) by name
   * @param profile  Optional evidence on original world (name -> label)
   * @param values   Optional intervention values (name -> label). If empty,
   *                 the result spans all values of the \p whatif variables.
   * @return Tensor<GUM_ELEMENT> probability distribution over \p on
   *         (variables are those of \p cm).
   */
  template < GUM_Numeric GUM_ELEMENT >
  PYGUM_PUBLIC Tensor< GUM_ELEMENT >
               counterfactual(const CausalModel< GUM_ELEMENT >&            cm,
                              const Set< std::string >&                    on,
                              const Set< std::string >&                    whatif,
                              const HashTable< std::string, std::string >& profile
                              = HashTable< std::string, std::string >(),
                              const HashTable< std::string, std::string >& values
                              = HashTable< std::string, std::string >());

  /**
   * \brief Build the twin network of Balke and Pearl (see Counterfactual::counterFactualModel).
   *
   * Mirrors the Python function `counterfactualModel(cm, whatif)`.
   *
   * @tparam GUM_ELEMENT numeric scalar type (e.g., double)
   * @param cm       Original causal model
   * @param whatif   Intervention targets (do-variables) by name
   * @return CausalModel<GUM_ELEMENT> the twin network
   * @throw InvalidArgument if \p cm has latent variables
   */
  template < GUM_Numeric GUM_ELEMENT >
  CausalModel< GUM_ELEMENT > counterfactualModel(const CausalModel< GUM_ELEMENT >& cm,
                                                 const Set< std::string >&         whatif);

  /// @deprecated deprecated after 3.3.2: the twin network does not depend on the profile, use
  /// counterfactualModel(cm, whatif).
  template < GUM_Numeric GUM_ELEMENT >
  [[deprecated("counterfactualModel(cm, profile, whatif) is deprecated after 3.3.2, use "
               "counterfactualModel(cm, whatif)")]]
  CausalModel< GUM_ELEMENT >
      counterfactualModel(const CausalModel< GUM_ELEMENT >&            cm,
                          const HashTable< std::string, std::string >& profile,
                          const Set< std::string >&                    whatif);

}   // namespace gum

#include <agrum/CM/counterfactual_tpl.h>

#endif   // GUM_COUNTERFACTUAL_H
