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
#ifndef GUM_CAUSAL_MODEL_H
#define GUM_CAUSAL_MODEL_H

#include <string>
#include <variant>

#include <agrum/BN/BayesNet.h>

namespace gum {

  /// LatentDescriptorIds = (latentName, (child1Id, child2Id, ...))
  using LatentDescriptorIds = std::pair<std::string, std::vector<NodeId>>;
  using LatentDescriptorVector = std::vector<LatentDescriptorIds>;


  /// CausalModel is a class representing a causal model, which is a directed acyclic graph (DAG)
  /// where nodes represent variables and edges represent causal relationships.
  template <typename GUM_SCALAR>
  class CausalModel {
    private:
    /// The underlying BayesNet representing the observed part of the model
    BayesNet<GUM_SCALAR> _observationalBN_;

    /// The underlying DAG representing the causal structure of the model
    DAG _causalDAG_;

    /// Bookkeeping: name <-> nodeId
    Bijection<NodeId, std::string> _id2name_;


   public:
    /// Default constructor
    CausalModel() = delete;

    /// constructor with no latent variables
    explicit CausalModel(const BayesNet<GUM_SCALAR>& observationalBN)
        : _observationalBN_(observationalBN),
          _causalDAG_(observationalBN.dag()) {}

    /// constructor with LatentDescriptorVector
    explicit CausalModel(const BayesNet<GUM_SCALAR>& observationalBN,
                    const LatentDescriptorVector& latentVarsDescriptor,
                    bool keepArcs = false);


    /// Copy constructor
    CausalModel(const CausalModel& other) = default;

    /// Move constructor
    CausalModel(CausalModel&& other) noexcept = default;

    /// Destructor
    ~CausalModel() = default;

    /// Assignment operator
    CausalModel& operator=(const CausalModel& other) = default;

    /// Move assignment operator
    CausalModel& operator=(CausalModel&& other) noexcept = default;

    /// Add a latent variable with its children using names
    void addLatentVariable(const std::string& latentName, const std::vector<std::string>& childrenOfLatent, bool keepArcs = false);

    /// Add a latent variable with its children using NodeIds
    void addLatentVariable(const std::string& latentName, const std::vector<NodeId>& childrenOfLatent, bool keepArcs = false);

    /// Add a causal arc x->y using NodeId
    void addCausalArc(NodeId x, NodeId y);

    /// Add causal arc x->y using variable names
    void addCausalArc(const std::string& x, const std::string& y);

    /// Remove a causal arc x->y using NodeId
    void eraseCausalArc(NodeId x, NodeId y);

    /// Remove a causal arc x->y using variable names
    void eraseCausalArc(const std::string& x, const std::string& y);

    /// Check if a causal arc x->y exists using NodeId
    bool existsArc(NodeId x, NodeId y) const;

    /// Check if a causal arc x->y exists using variable names
    bool existsArc(const std::string& x, const std::string& y) const;

    /// Create an causal model induced by a subset of nodes.
    CausalModel<GUM_SCALAR>
    inducedCausalSubModel(const CausalModel<GUM_SCALAR>& cm, NodeSet subset) const;

    /// Returns friendly display of the causal DAG in DOT format
    std::string toDot(const bool   SHOW_LATENT_NAMES = false,
                      const char* NODE_BG  = "#404040",
                      const char* NODE_FG = "white",
                      const char* EDGE_COL = "#4A4A4A") const;

    /// Returns the underlying BayesNet representing the observed part of the model
    const BayesNet<GUM_SCALAR>& observationalBN() const {
      return _observationalBN_;
    }
    /// Returns the underlying DAG representing the causal structure of the model
    const DAG& causalDAG() const {
      return _causalDAG_;
    }

    /// Returns the set of all variable names in the causal model (observed + latent).
    Set<std::string> names() const;

    /// Returns the node id for a given variable name (observed or latent).
    NodeId idFromName(const std::string& name) const;

    /// Returns the variable name for a given node id (observed or latent).
    std::string nameFromId(NodeId id) const;

    /// Returns the NodeSet of latent variable Ids
    NodeSet latentVariablesIds() const;

    /// Returns the Set of latent variable names
    Set<std::string> latentVariablesNames() const;

    /// Parents of a node (by NodeId) in the causal DAG (including latents)
    NodeSet parents(NodeId x) const;

    /// Parents of a node (by name) in the causal DAG (including latents)
    NodeSet parents(const std::string& name) const;

    /// Children of a node (by NodeId) in the causal DAG (including latents)
    NodeSet children(NodeId x) const;

    /// Children of a node (by name) in the causal DAG (including latents)
    NodeSet children(const std::string& name) const;

    /// connected components of the causal DAG (observed + latent)
    HashTable<NodeId, NodeSet> connectedComponents() const;

  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class CausalModel<double>;
#endif

} // namespace gum

#include <agrum/CM/causalModel_tpl.h>

#endif // GUM_CAUSAL_MODEL_H