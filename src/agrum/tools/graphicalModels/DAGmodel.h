/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES (@AMU)
 *
 */
#ifndef GUM_DAGMODEL_H
#define GUM_DAGMODEL_H

#include <agrum/agrum.h>
#include <agrum/tools/multidim/instantiation.h>
#include <agrum/tools/graphicalModels/graphicalModel.h>

#include <agrum/tools/graphs/DAG.h>
#include <agrum/tools/graphs/undiGraph.h>

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
    /// @name Getter and setters
    /// @{

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

    const NodeGraphPart& nodes() const;

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

    /// @name Arc manipulation methods.
    /// @{
    const ArcSet& arcs() const;

    /// returns the set of nodes with arc ingoing to a given node
    /** Note that the set of nodes returned may be empty if no arc within the
     * ArcGraphPart is ingoing into the given node.
     * @param id the node toward which the nodes returned share an arc*/
    const NodeSet& parents(const NodeId id) const;
    const NodeSet& parents(const std::string& name) const;

    /// returns the set of nodes with arc outgoing from a given node
    /** Note that the set of arcs returned may be empty if no arc within the
     * ArcGraphPart is outgoing from the given node.
     * @param id the node which is the tail of the arcs returned */
    const NodeSet& children(const NodeId id) const;
    const NodeSet& children(const std::string& name) const;
    /// @}

    /// @name Graphical methods
    /// @{

    /**
     * The node's id are coherent with the variables and nodes of the topology.
     * @param clear If false returns the previously created moral graph.
     */
    const UndiGraph& moralGraph(bool clear = true) const;

    /**
     * The topological order stays the same as long as no variable or arcs are
     * added or erased src the topology.
     * @param clear If false returns the previously created topology.
     */
    const Sequence< NodeId >& topologicalOrder(bool clear = true) const;

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
    DAG _dag;

    private:
    /// Returns the moral graph of this DAGModel.
    /// @warning __mutableMoralGraph is assumed to be valid and empty
    void __moralGraph() const;

    /// The moral graph of this Directed Graphical Model.
    mutable UndiGraph* __mutableMoralGraph;
  };
}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphicalModels/DAGmodel_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_DAGMODEL_H */