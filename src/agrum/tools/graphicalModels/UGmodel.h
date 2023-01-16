/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Class representing probabilistic undirected model
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#ifndef GUM_UGMODEL_H
#define GUM_UGMODEL_H

#include <agrum/agrum.h>
#include <agrum/tools/graphicalModels/graphicalModel.h>


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

    const NodeGraphPart& nodes() const final;

    /**
     * Return true if this node exists in this graphical model.
     */
    bool exists(NodeId node) const final;

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

    /** check if nodes X and nodes Y are independent given nodes Z
     */
    // virtual bool isIndependent(const NodeSet& X, const NodeSet& Y, const
    // NodeSet& Z) const;

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
