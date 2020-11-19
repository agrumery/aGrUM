/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Class representing probabilistic DAG model
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */
#ifndef GUM_UGMODEL_H
#define GUM_UGMODEL_H

#include <agrum/agrum.h>
#include <agrum/tools/multidim/instantiation.h>
#include <agrum/tools/graphicalModels/graphicalModel.h>

#include <agrum/tools/graphs/undiGraph.h>

namespace gum {

  /**
   * @class UGmodel
   * @headerfile UGmodel.h <agrum/tools/graphicalModels/UGmodel.h>
   * Virtual base class for PGMs using a undirected graph
   *
   */
  class UGmodel: public GraphicalModel {
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
    virtual ~UGmodel();

    /**
     * Copy constructor. Do nothing.
     */
    UGmodel(const UGmodel& source);

    /// @}
    /// @name Getter and setters
    /// @{

    /// @}
    /// @name Variable manipulation methods.
    /// @{
    /**
     * Returns a constant reference to the dag of this Bayes Net.
     */
    const UndiGraph& graph() const;

    /**
     * Returns the number of variables in this Directed Graphical Model.
     */
    virtual Size size() const final;

    /**
     * Returns the number of arcs in this Directed Graphical Model.
     */
    Size sizeEdges() const;

    const NodeGraphPart& nodes() const;

    /**
     * Return true if this node exists in this graphical model.
     */
    bool exists(NodeId node) const final;

    /**
     * Returns a constant reference over a variable given it's node id.
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const DiscreteVariable& variable(NodeId id) const = 0;

    /**
     * Return id node src discrete var pointer.
     * @throw NotFound If no variable matches var.
     */
    virtual NodeId nodeId(const DiscreteVariable& var) const = 0;

    /// Getter by name
    /// @throw NotFound if no such name exists in the graph.
    virtual NodeId idFromName(const std::string& name) const = 0;

    /// Getter by name
    /// @throw NotFound if no such name exists in the graph.
    virtual const DiscreteVariable&
       variableFromName(const std::string& name) const = 0;

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
    bool existsEdge(const std::string& name1, const std::string& name2) const;

    /// returns the neighbours of a node as set of nodes
    /** Note that the set of nodes returned may be empty if no edge within the
     * EdgeGraphPart contains the given node.
     * @param id the node toward which the edge returned are pointing */
    const NodeSet& neighbours(const NodeId id) const;
    const NodeSet& neighbours(const std::string& name) const;

    /** check if X and Y are independent given Z
     */
    bool isIndependent(NodeId X, NodeId Y, const NodeSet& Z) const;
    bool isIndependent(const std::string&                Xname,
                       const std::string&                Yname,
                       const std::vector< std::string >& Znames) const;
    /// @}

    /// @return true if all the named node are the same and all the named arcs are
    /// the same
    bool hasSameStructure(const UGmodel& other);

    protected:
    /**
     * Private copy operator.
     */
    UGmodel& operator=(const UGmodel& source);

    /// The DAG of this Directed Graphical Model.
    UndiGraph graph_;
  };
}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphicalModels/UGmodel_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_UGMODEL_H */
