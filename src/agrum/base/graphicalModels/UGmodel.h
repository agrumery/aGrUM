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
 * @brief Class representing probabilistic undirected model
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#ifndef GUM_UGMODEL_H
#define GUM_UGMODEL_H

#include <agrum/agrum.h>

#include <agrum/base/graphicalModels/discreteGraphicalModel.h>

namespace gum {

  /**
   * @class UGmodel
   * @headerfile UGmodel.h <agrum/base/graphicalModels/UGmodel.h>
   * Virtual base class for PGMs using a undirected graph
   *
   */
  class UGmodel: public DiscreteGraphicalModel {
    public:
    /// @name Constructors / Destructors
    /// @{

    /**
     * Default constructor.
     */
    UGmodel();

    /**
     * Destructor.
     */
    ~UGmodel() override;

    /**
     * Copy constructor. Do nothing.
     */
    UGmodel(const UGmodel& source);

    /// move constructor
    UGmodel(UGmodel&& source);

    /// @}
    /// @name Getter and setters
    /// @{

    /// @}
    /// @name Variable manipulation methods.
    /// @{
    /**
     * Returns a named copy of the internal undirected graph: each node id is
     * assigned the name of the corresponding variable. O(n) — allocates a new graph.
     * For a stable reference (listeners, long-lived pointers), use internalGraph().
     */
    [[nodiscard]] UndiGraph graph() const;

    /**
     * Returns the number of variables in this Directed Graphical Model.
     */
    virtual Size size() const final;

    /**
     * Returns the number of arcs in this Directed Graphical Model.
     */
    Size sizeEdges() const;

    const NodeGraphPart& nodes() const final;

    /**
     * Return true if this node exists in this graphical model.
     */
    bool exists(NodeId node) const final;
    bool exists(std::string_view name) const final;

    /// @}

    /// @name Edge manipulation methods.
    /// @{
    const EdgeSet& edges() const;

    /// return true if the edge node1-node2 exists in the UGModel
    /**
     *
     * @param node1 the nodeId (or the name) of the node1
     * @param node2 the nodeId (or the name) of the node2
     * @return true if the edge exists
     */
    bool existsEdge(const NodeId node1, const NodeId node2) const;
    bool existsEdge(std::string_view name1, std::string_view name2) const;

    /// returns the neighbours of a node as set of nodes
    /** Note that the set of nodes returned may be empty if no edge within the
     * EdgeGraphPart contains the given node.
     * @param id the node toward which the edge returned are pointing */
    const NodeSet& neighbours(const NodeId id) const;
    const NodeSet& neighbours(std::string_view name) const;

    /** check if X and Y are independent given Z
     */
    bool isIndependent(NodeId X, NodeId Y, const NodeSet& Z) const final;
    /** check if nodes X and nodes Y are independent given nodes Z
     */
    bool isIndependent(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const final;

    bool isIndependent(std::string_view                  Xname,
                       std::string_view                  Yname,
                       const std::vector< std::string >& Znames) const {
      return isIndependent(idFromName(Xname), idFromName(Yname), nodeset(Znames));
    }

    bool isIndependent(const std::vector< std::string >& Xnames,
                       const std::vector< std::string >& Ynames,
                       const std::vector< std::string >& Znames) const {
      return isIndependent(nodeset(Xnames), nodeset(Ynames), nodeset(Znames));
    }

    /** check if nodes X and nodes Y are independent given nodes Z
     */
    // virtual bool isIndependent(const NodeSet& X, const NodeSet& Y, const
    // NodeSet& Z) const;

    /// @}

    /// @return true if all the named node are the same and all the named arcs are
    /// the same
    bool hasSameStructure(const UGmodel& other) const;

    /// Returns the connected components of the underlying undirected graph.
    /// Each node maps to the id of its component root.
    NodeProperty< NodeId > connectedComponents() const;

    /// returns the node as a NodeSet (in mixed graph : family is the node and its parents
    /**
     * @param id the node which is the head of an arc with the returned nodes
     * @param name the name of the node the node which is the head of an arc with
     * the returned nodes*/
    NodeSet family(const NodeId id) const final;
    NodeSet family(std::string_view name) const final;

    /// Returns a const reference to the internal (unnamed) graph. O(1), no copy.
    /// Use for stable references or pointers (graph listeners).
    /// For named node access, use graph() instead.
    const UndiGraph& internalGraph() const;

    protected:
    /**
     * Private copy operator.
     */
    UGmodel& operator=(const UGmodel& source);

    /// move assignment operator
    UGmodel& operator=(UGmodel&& source);

    /// The DAG of this Directed Graphical Model.
    UndiGraph graph_;
  };
}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/base/graphicalModels/UGmodel_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_UGMODEL_H */
