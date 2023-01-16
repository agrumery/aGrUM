/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#ifndef GUM_DAGMODEL_H
#define GUM_DAGMODEL_H

#include <agrum/agrum.h>
#include <agrum/tools/graphicalModels/graphicalModel.h>


namespace gum {

  /**
   * @class DAGmodel
   * @headerfile DAGmodel.h <agrum/tools/graphicalModels/DAGmodel.h>
   * Virtual base class for PGMs using a DAG
   *
   */
  class DAGmodel: public GraphicalModel {
    public:
    /// @name Constructors / Destructors
    /// @{

    /**
     * Default constructor.
     */
    DAGmodel();

    /**
     * Destructor.
     */
    virtual ~DAGmodel();

    /**
     * Copy constructor. Do nothing.
     */
    DAGmodel(const DAGmodel& source);

    /// @}

    /// @name Variable manipulation methods.
    /// @{
    /**
     * Returns a constant reference to the dag of this Bayes Net.
     */
    const DAG& dag() const;

    /**
     * Returns the number of variables in this Directed Graphical Model.
     */
    virtual Size size() const final;

    /**
     * Returns the number of arcs in this Directed Graphical Model.
     */
    Size sizeArcs() const;

    const NodeGraphPart& nodes() const final;

    /**
     * Return true if this node exists in this graphical model.
     */
    bool exists(NodeId node) const final;

    /// @}

    /// @name Arc manipulation methods.
    /// @{
    const ArcSet& arcs() const;

    /// return true if the arc tail->head exists in the DAGmodel
    /**
     *
     * @param tail the nodeId (or the name) of the tail in tail->head
     * @param head the nodeId (or the name) of the head in tail->head
     * @return true if the arc exists
     */
    bool existsArc(const NodeId tail, const NodeId head) const;
    bool existsArc(const std::string& nametail, const std::string& namehead) const;

    /// returns the set of nodes with arc ingoing to a given node
    /** Note that the set of nodes returned may be empty if no arc within the
     * ArcGraphPart is ingoing into the given node.
     * @param id the node which is the head of an arc with the returned nodes
     * @param name the name of the node the node which is the head of an arc with
     * the returned nodes*/
    const NodeSet& parents(const NodeId id) const;
    const NodeSet& parents(const std::string& name) const;

    /// returns the parents of a set of nodes
    NodeSet parents(const NodeSet& ids) const;
    NodeSet parents(const std::vector< std::string >& names) const;

    /// returns the parents of a node and the node
    /**
     * @param id the node which is the head of an arc with the returned nodes
     * @param name the name of the node the node which is the head of an arc with
     * the returned nodes*/
    NodeSet family(const NodeId id) const;
    NodeSet family(const std::string& name) const;

    /// returns the set of nodes with arc outgoing from a given node
    /** Note that the set of nodes returned may be empty if no node
     * is outgoing from the given node.
     * @param id the node which is the tail of an arc with the returned nodes
     * @param name the name of the node which is the tail of an arc with the
     * returned nodes */
    const NodeSet& children(const NodeId id) const;
    const NodeSet& children(const std::string& name) const;

    /// returns the children of a set of nodes
    NodeSet children(const NodeSet& ids) const;
    NodeSet children(const std::vector< std::string >& names) const;

    /// returns the set of nodes with directed path outgoing from a given node
    /** Note that the set of nodes returned may be empty if no path within the
     * ArcGraphPart is outgoing from the given node.
     * @param id the node which is the tail of a directed path with the returned
     * nodes
     * @param name the name of the node which is the tail of a directed path with
     * the returned nodes */
    NodeSet descendants(const NodeId id) const;
    NodeSet descendants(const std::string& name) const;

    /// returns the set of nodes with directed path ingoing to a given node
    /** Note that the set of nodes returned may be empty if no path within the
     * ArcGraphPart is ingoing to the given node.
     * @param id the node which is the head of a directed path with the returned
     * nodes
     * @param name the name of the node which is the head of a directed path with
     * the returned nodes */
    NodeSet ancestors(const NodeId id) const;
    NodeSet ancestors(const std::string& name) const;
    /// @}

    /// @name Graphical methods
    /// @{

    /** build a UndiGraph by moralizing the Ancestral Graph of a set of Nodes
     *
     * @param nodes the set of nodeId
     * @param nodenames the vector of names of nodes
     * @return the moralized ancestral graph
     */
    UndiGraph moralizedAncestralGraph(const NodeSet& nodes) const;
    UndiGraph moralizedAncestralGraph(const std::vector< std::string >& nodenames) const;

    /** check if node X and node Y are independent given nodes Z
     */
    bool isIndependent(NodeId X, NodeId Y, const NodeSet& Z) const final;

    /** check if nodes X and nodes Y are independent given nodes Z
     */
    bool isIndependent(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const final;

    bool isIndependent(const std::string&                Xname,
                       const std::string&                Yname,
                       const std::vector< std::string >& Znames) const {
      return isIndependent(idFromName(Xname), idFromName(Yname), nodeset(Znames));
    };

    bool isIndependent(const std::vector< std::string >& Xnames,
                       const std::vector< std::string >& Ynames,
                       const std::vector< std::string >& Znames) const {
      return isIndependent(nodeset(Xnames), nodeset(Ynames), nodeset(Znames));
    };
    /**
     * The node's id are coherent with the variables and nodes of the topology.
     */
    UndiGraph moralGraph() const;

    /**
     * The topological order stays the same as long as no variable or arcs are
     * added or erased src the topology.
     * @param clear If false returns the previously created topology.
     */
    Sequence< NodeId > topologicalOrder() const;
    /// @}

    /// @return true if all the named node are the same and all the named arcs are
    /// the same
    bool hasSameStructure(const DAGmodel& other);

    protected:
    /**
     * Private copy operator.
     */
    DAGmodel& operator=(const DAGmodel& source);

    /// The DAG of this Directed Graphical Model.
    DAG dag_;
  };
}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphicalModels/DAGmodel_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_DAGMODEL_H */
