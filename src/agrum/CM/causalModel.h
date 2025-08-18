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

#include <agrum/base/core/bijection.h>
#include <agrum/base/core/hashTable.h>
#include <agrum/base/core/list.h>
#include <agrum/base/core/set.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/base/graphs/DAG.h>

namespace gum {


  /// LatentDescriptorNames = (latentName, (child1, child2, ...))
  using LatentDescriptorNames = std::pair<std::string, List<std::string>>;

  /// LatentDescriptorIds = (latentName, (child1Id, child2Id, ...))
  using LatentDescriptorIds = std::pair<std::string, List<NodeId>>;


  /// Unified descriptor: either names or ids
  using LatentDescriptor = std::variant<LatentDescriptorNames, LatentDescriptorIds>;
  using LatentDescriptorList = List<LatentDescriptor>;


  /// CausalModel is a class representing a causal model, which is a directed acyclic graph (DAG)
  /// where nodes represent variables and edges represent causal relationships.
  template <typename GUM_SCALAR>
  class CausalModel {
    private:
    /// The underlying BayesNet representing the observed part of the model
    BayesNet<GUM_SCALAR> _observedBN_;

    /// The underlying DAG representing the causal structure of the model
    DAG _causalDAG_;

    /// Set of latent variable IDs
    NodeSet _latentIds_;

    /// Set of latent variable names
    Set<std::string> _latentNames_;

    /// List of names
    Set<std::string> _names_;

    /// Bookkeeping: name -> nodeId
    HashTable<std::string, NodeId> _idFromName_;

    // Bookkeeping: nodeId -> name
    HashTable<NodeId, std::string> _nameFromId_;


   public:
    /// Default constructor
    CausalModel() = delete;

    /// constructor with no latent variables
    explicit CausalModel(const BayesNet<GUM_SCALAR>& observedBN)
        : _observedBN_(observedBN),
          _causalDAG_(observedBN.dag()),
          _names_(BN2names(observedBN)),
          _idFromName_(BN2idFromName(observedBN)),
          _nameFromId_(BN2nameFromId(observedBN)) {}

    /// constructor with LatentDescriptorList
    explicit CausalModel(const BayesNet<GUM_SCALAR>& observedBN,
                    const LatentDescriptorList& latentVarsDescriptor,
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
    void addLatentVariable(const std::string& latentName, const List<std::string>& childrenOfLatent, bool keepArcs = false);

    /// Add a latent variable with its children using NodeIds
    void addLatentVariable(const std::string& latentName, const List<NodeId>& childrenOfLatent, bool keepArcs = false);

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

    /// Returns friendly display of the causal DAG in DOT format
    std::string toDot() const;

    /// Returns the underlying BayesNet representing the observed part of the model
    const BayesNet<GUM_SCALAR>& observedBayesNet() const {
      return _observedBN_;
    }
    /// Returns the underlying DAG representing the causal structure of the model
    const DAG& causalDAG() const {
      return _causalDAG_;
    }

    /// Returns the set of all variable names in the causal model (observed + latent).
    const Set<std::string>& names() const;

    /// Returns the node id for a given variable name (observed or latent).
    const NodeId idFromName(const std::string& name) const;

    /// Returns the variable name for a given node id (observed or latent).
    const std::string& nameFromId(NodeId id) const;

    /// Returns the list of latent variable Ids
    const NodeSet& latentVariablesIds() const;

    /// Returns the list of latent variable names
    const Set<std::string>& latentVariablesNames() const;

    /// connected components from a graph/graphical models
    // HashTable<Size, NodeSet> connectedComponents() const;

    /// Static function to compute list of names in a BN
    static Set<std::string> BN2names(const BayesNet<GUM_SCALAR>& bn);

    /// Static function to compute _idFromName_ mapping
    static HashTable<std::string, NodeId> BN2idFromName(const BayesNet<GUM_SCALAR>& bn);

    /// Static function to compute _nameFromId_ mapping
    static HashTable<NodeId, std::string> BN2nameFromId(const BayesNet<GUM_SCALAR>& bn);


  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class CausalModel<double>;
#endif

} // namespace gum

#include <agrum/CM/causalModel_tpl.h>

#endif // GUM_CAUSAL_MODEL_H