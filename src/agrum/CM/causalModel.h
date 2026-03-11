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


#ifndef GUM_CAUSAL_MODEL_H
#define GUM_CAUSAL_MODEL_H

#include <optional>
#include <string>

#include <agrum/BN/BayesNet.h>

namespace gum {

  // forward declaration:
  template < GUM_Numeric GUM_SCALAR >
  class ASTtree;

  /// LatentDescriptorIds = (latentName, (child1Id, child2Id, ...))
  /**
   * @brief Compact descriptor for a single latent variable and its observed children.
   *
   * The first element is the **name** of the latent to create; the second element is
   * the list of **NodeId**s of observed children the latent points to in the causal DAG.
   */
  using LatentDescriptorIds = std::pair< std::string, std::vector< NodeId > >;

  /// Collection of latent descriptors (see LatentDescriptorIds).
  using LatentDescriptorVector = std::vector< LatentDescriptorIds >;

  /**
   * @class CausalModel
   * @brief A causal model pairing an observational BayesNet with a causal DAG.
   *
   * The class is **DAG-centric** for causality: all causal-structure queries and edits
   * happen on `_causalDAG_`, which may include latent variables that do not appear
   * in `_observationalBN_`. Name/Id bookkeeping is handled so you can work with either.
   *
   * ### Invariants / expectations
   * - `_observationalBN_` corresponds to the observed part (no latents).
   * - `_causalDAG_` contains all observed variables and any added latent variables.
   * - When adding a latent with children, the latent becomes a parent of all listed children
   *   in the **causal DAG**. The `assumeNonSpurious` flag controls whether existing arcs between
   *   those children are preserved or may be adjusted (implementation dependent).
   */
  template < GUM_Numeric GUM_SCALAR >
  class CausalModel {
    private:
    /// The underlying BayesNet representing the observed part of the model.
    BayesNet< GUM_SCALAR > _observationalBN_;

    /// The underlying DAG representing the causal structure (observed + latent).
    DAG _causalDAG_;

    /// Bidirectional mapping between node ids and variable names (observed + latent).
    Bijection< NodeId, std::string > _id2name_;

    public:
    /// Default constructor disabled: a causal model must be built from a BN.
    CausalModel() = delete;

    /**
     * @brief Construct a causal model with no latent variables.
     * @param observationalBN the observed Bayesian network
     *
     * Initializes the causal DAG as the BN's DAG (observed-only).
     */
    explicit CausalModel(const BayesNet< GUM_SCALAR >& observationalBN) :
        _observationalBN_(observationalBN), _causalDAG_(observationalBN.dag()) {}

    /**
     * @brief Construct a causal model and add a list of latent confounders.
     * @param observationalBN the observed Bayesian network
     * @param latentVarsDescriptor list of (latentName, childrenIds) descriptors
     * @param assumeNonSpurious whether to preserve existing arcs among the latent's children
     *
     * Each latent is created and added as a parent of the provided children in the
     * **causal DAG**. The observed BN remains unchanged.
     */
    explicit CausalModel(const BayesNet< GUM_SCALAR >& observationalBN,
                         const LatentDescriptorVector& latentVarsDescriptor,
                         bool                          assumeNonSpurious = false);

    /// Copy constructor
    CausalModel(const CausalModel& other) { GUM_CONS_CPY(CausalModel) };

    /// Move constructor
    CausalModel(CausalModel&& other) noexcept { GUM_CONS_MOV(CausalModel) };

    /// Destructor
    ~CausalModel() { GUM_DESTRUCTOR(CausalModel) };

    /// Copy assignment
    CausalModel& operator=(const CausalModel& other) = default;

    /// Move assignment
    CausalModel& operator=(CausalModel&& other) noexcept = default;

    /**
     * @brief Add a latent variable by **names** of its observed children.
     * @param latentName new latent variable name
     * @param childrenOfLatent names of observed children
     * @param assumeNonSpurious preserve existing arcs among the children if true
     *
     * A new latent node is inserted into the causal DAG and connected as a parent
     * of each listed child. The observed BN is not modified.
     *
     * @warning When `assumeNonSpurious == false` and the latent has children
     *          \f$\{X_1,\ldots,X_n\}\f$, any existing arcs \f$X_i \rightarrow X_j\f$
     *          **among those children** may be removed from the causal DAG.
     *          To preserve some child→child arcs while dropping others, consider
     *          creating separate latents: one with `assumeNonSpurious = true` for the subset
     *          to preserve, and another with `assumeNonSpurious = false` for the subset to drop.
     *          A finer-grained edit API may be added later.
     */
    void addLatentVariable(const std::string&                latentName,
                           const std::vector< std::string >& childrenOfLatent,
                           bool                              assumeNonSpurious = false);

    /**
     * @brief Add a latent variable by **NodeId**s of its observed children.
     * @param latentName new latent variable name
     * @param childrenOfLatent node ids of observed children
     * @param assumeNonSpurious preserve existing arcs among the children if true.
     *
     * A new latent node is inserted into the causal DAG and connected as a parent
     * of each listed child. The observed BN is not modified.
     *
     * @warning When `assumeNonSpurious == false` and the latent has children
     *          \f$\{X_1,\ldots,X_n\}\f$, any existing arcs \f$X_i \rightarrow X_j\f$
     *          **among those children** may be removed from the causal DAG.
     *          To preserve some child→child arcs while dropping others, consider
     *          creating separate latents: one with `assumeNonSpurious = true` for the subset
     *          to preserve, and another with `assumeNonSpurious = false` for the subset to drop.
     *          A finer-grained edit API may be added later.
     */
    void addLatentVariable(const std::string&           latentName,
                           const std::vector< NodeId >& childrenOfLatent,
                           bool                         assumeNonSpurious = false);

    /// @brief Whether a causal arc x → y exists (by ids) in the causal DAG.
    bool existsArc(NodeId x, NodeId y) const;

    /// @brief Whether a causal arc x → y exists (by names) in the causal DAG.
    bool existsArc(const std::string& x, const std::string& y) const;

    /**
     * @brief Mark an arc as spurious: present in observationalBN, removed from causalDAG.
     * Throws if arc is not present in observationalBN.
     */
    void assumeSpurious(NodeId x, NodeId y);
    void assumeSpurious(const std::string& x, const std::string& y);

    /**
     * @brief Mark an arc as non-spurious: present in observationalBN, added to causalDAG.
     * Throws if arc is not present in observationalBN.
     */
    void assumeNonSpurious(NodeId x, NodeId y);
    void assumeNonSpurious(const std::string& x, const std::string& y);

    /**
     * @brief Returns true if arc is present in observationalBN and absent in causalDAG.
     */
    bool isAssumedSpurious(NodeId x, NodeId y) const;
    bool isAssumedSpurious(const std::string& x, const std::string& y) const;

    // ======================================================================
    // Door-criteria conveniences
    // ======================================================================

    /**
     * @brief Find a backdoor adjustment set \(Z\) between `cause` and `effect`
     *        (IDs only).
     *
     * Enumerates admissible backdoor sets in the current causal DAG
     * (excluding latent variables) and returns the **first** valid one found.
     * Returns `std::nullopt` if no backdoor set exists at all.
     * Note: an **empty** set is a valid backdoor when X has no back-door paths.
     *
     * Preconditions:
     *  - `cause` and `effect` must be **observed** variables of the model.
     *
     * @param cause  NodeId of the cause \(X\).
     * @param effect NodeId of the effect \(Y\).
     * @return std::optional<NodeSet> The backdoor adjustment set \(Z\) as NodeIds,
     *                                or std::nullopt if no backdoor set exists.
     */
    std::optional< NodeSet > backDoor(NodeId cause, NodeId effect) const;

    /**
     * @brief Find a frontdoor adjustment set \(Z\) between `cause` and `effect`
     *        (IDs only).
     *
     * Enumerates admissible frontdoor sets in the current causal DAG
     * (excluding latent variables) and returns the **first** valid one found.
     * Returns `std::nullopt` if no frontdoor set exists at all.
     * Note: an **empty** set is a valid frontdoor in degenerate cases.
     *
     * Preconditions:
     *  - `cause` and `effect` must be **observed** variables of the model.
     *
     * @param cause  NodeId of the cause \(X\).
     * @param effect NodeId of the effect \(Y\).
     * @return std::optional<NodeSet> The frontdoor adjustment set \(Z\) as NodeIds,
     *                                or std::nullopt if no frontdoor set exists.
     */
    std::optional< NodeSet > frontDoor(NodeId cause, NodeId effect) const;


    /**
     * @brief Induced causal submodel on a subset of nodes.
     * @param cm a source causal model
     * @param subset node set to keep (observed + latent)
     * @return A new CausalModel induced by the subset (both BN/DAG restricted appropriately).
     */
    CausalModel< GUM_SCALAR > inducedCausalSubModel(const CausalModel< GUM_SCALAR >& cm,
                                                    NodeSet                          subset) const;

    /**
     * @brief DOT representation of the causal DAG (observed + latent).
     * @param SHOW_LATENT_NAMES  If true, display latent names explicitly.
     * @param NODE_BG            Node background color (hex code like "#404040" or Graphviz color
     * name like "lightgray").
     * @param NODE_FG            Node label/text color (hex code or Graphviz color name).
     * @param EDGE_COL           Edge color (hex code or Graphviz color name).
     * @return A string containing a Graphviz/DOT graph.
     */
    std::string toDot(const bool  SHOW_LATENT_NAMES = false,
                      const char* NODE_BG           = "#404040",
                      const char* NODE_FG           = "white",
                      const char* EDGE_COL          = "#4A4A4A") const;

    /// @brief Observational BN (observed variables only).
    const BayesNet< GUM_SCALAR >& observationalBN() const { return _observationalBN_; }

    /// @brief Causal DAG (observed + latent variables).
    const DAG& causalDAG() const { return _causalDAG_; }

    /// @brief All variable names appearing in the causal model (observed + latent).
    Set< std::string > names() const;

    /// @brief Node id from variable name (observed or latent).
    NodeId idFromName(const std::string& name) const;

    /// @brief Variable name from node id (observed or latent).
    std::string nameFromId(NodeId id) const;

    /// @brief Bidirectional mapping between node ids and variable names
    Bijection< NodeId, std::string > id2name(bool includeLatentVariable = false) const;

    /// @brief Node ids of all latent variables.
    NodeSet latentVariablesIds() const;

    /// @brief Names of all latent variables.
    Set< std::string > latentVariablesNames() const;

    /// @brief Parents of a node (by id) in the causal DAG (including latents).
    NodeSet parents(NodeId x) const;

    /// @brief Parents of a node (by name) in the causal DAG (including latents).
    NodeSet parents(const std::string& name) const;

    /// @brief Children of a node (by id) in the causal DAG (including latents).
    NodeSet children(NodeId x) const;

    /// @brief Children of a node (by name) in the causal DAG (including latents).
    NodeSet children(const std::string& name) const;

    /**
     * @brief Weakly connected components of the causal DAG.
     * @return A table mapping a representative NodeId to the NodeSet of nodes in its component.
     */
    HashTable< NodeId, NodeSet > connectedComponents() const;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class CausalModel< double >;
#endif

}   // namespace gum

#include <agrum/CM/causalModel_tpl.h>

#endif   // GUM_CAUSAL_MODEL_H
