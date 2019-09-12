
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#ifndef GUM_DAGMODEL_H
#define GUM_DAGMODEL_H
#include <agrum/agrum.h>
#include <agrum/multidim/instantiation.h>

#include <agrum/graphs/DAG.h>
#include <agrum/graphs/undiGraph.h>

#include <agrum/graphicalModels/variableNodeMap.h>

namespace gum {

  /**
   * @class DAGmodel
   * @headerfile DAGmodel.h <agrum/graphicalModels/DAGmodel.h>
   * Virtual base class for PGMs using a DAG
   *
   */
  class DAGmodel {
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

    /**
     * Return the value of the property name of this DAGModel.
     * @throw NotFound Raised if no name property is found.
     */
    const std::string& property(const std::string& name) const;

    /**
     * Return the value of the property name of this DAGModel.
     * return byDefault if the property name is not found
     */
    const std::string& propertyWithDefault(const std::string& name,
                                           const std::string& byDefault) const;

    /**
     * Add or change a property of this DAGModel.
     */
    void setProperty(const std::string& name, const std::string& value);

    /// @}
    /// @name Variable manipulation methods.
    /// @{
    /**
     * Returns a constant reference to the dag of this Bayes Net.
     */
    const DAG& dag() const;

    /**
     * Returns a constant reference to the VariableNodeMap of this Directed
     * Graphical
     * Model
     */
    virtual const VariableNodeMap& variableNodeMap() const = 0;

    /**
     * Returns the number of variables in this Directed Graphical Model.
     */
    Size size() const;

    /**
     * Returns the number of arcs in this Directed Graphical Model.
     */
    Size sizeArcs() const;

    /**
     * Retursn true if this Directed Graphical Model is empty.
     */
    bool empty() const;

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

    /// Get an instantiation over all the variables of the model
    virtual Instantiation completeInstantiation() const final;
    /// @}

    /// @name Arc manipulation methods.
    /// @{
    const ArcSet& arcs() const;

    /// returns the set of nodes with arc ingoing to a given node
    /** Note that the set of arcs returned may be empty if no arc within the
     * ArcGraphPart is ingoing into the given node.
     * @param id the node toward which the arcs returned are pointing */
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

    /// @return Returns the log10 domain size of the joint probabilty for the
    /// Directed Graphical Model
    double log10DomainSize() const;

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

    /// The properties of this Directed Graphical Model.
    /// Initialized using a lazy instantiation.
    mutable HashTable< std::string, std::string >* __propertiesMap;

    /// Return the properties of this Directed Graphical Model and initialize
    /// the
    /// hash table is
    /// necessary.
    HashTable< std::string, std::string >& __properties() const;
  };
}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/graphicalModels/DAGmodel_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_DAGMODEL_H */
