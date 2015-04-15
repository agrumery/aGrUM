/***************************************************************************
*   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
*   {prenom.nom}_at_lip6.fr                                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
/**
* @file
* @brief Class representing probabilistic DAG model
*
* @author Pierre-Henri WUILLEMIN and Christophe GONZALES
*
*/
#ifndef GUM_DAGMODEL_H
#define GUM_DAGMODEL_H
#include <agrum/config.h>

#include <agrum/graphs/DAG.h>
#include <agrum/graphs/undiGraph.h>

#include <agrum/graphicalModels/variableNodeMap.h>
#include <agrum/multidim/instantiation.h>

namespace gum {

  /**
  * @class DAGmodel DAGmodel.h <agrum/graphicalModels/DAGmodel.h>
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
    DAGmodel(const DAGmodel &source);

    /// @}
    /// @name Getter and setters
    /// @{

    /**
     * Return the value of the property name of this DAGModel.
     * @throw NotFound Raised if no name property is found.
     */
    const std::string &property(const std::string &name) const;

    /**
     * Return the value of the property name of this DAGModel.
     * return byDefault if the property name is not found
     */
    const std::string &propertyWithDefault(const std::string &name,
                                           const std::string &byDefault) const;

    /**
     * Add or change a property of this DAGModel.
     */
    void setProperty(const std::string &name, const std::string &value);

    /// @}
    /// @name Variable manipulation methods.
    /// @{
    /**
     * Returns a constant reference to the dag of this Bayes Net.
     */
    const DAG &dag() const;

    /**
    * Returns a constant reference to the VariableNodeMap of this Directed Graphical
    * Model
    */
    virtual const VariableNodeMap &variableNodeMap() const = 0;

    /**
     * Returns the number of variables in this Directed Graphical Model.
     */
    Size size() const;

    /**
     * Returns the number of arcs in this Directed Graphical Model.
     */
    Size sizeArcs() const;

    /**
     * Returns the number of arcs in this Directed Graphical Model.
     *
     * @deprecated Please use sizeArcs
     */
    GUM_DEPRECATED(Size nbrArcs() const);

    /**
     * Retursn true if this Directed Graphical Model is empty.
     */
    bool empty() const;

    /**
     * Shortcut for this->dag().beginNodes()
     *
     * @deprecated use for(auto node:obj.nodes()) instead
     */
    GUM_DEPRECATED(const DAG::NodeIterator beginNodes() const);

    /**
     * Shortcut for this->dag().endNodes()
     *
     * @deprecated use for(auto node:obj.nodes()) instead
     */
    GUM_DEPRECATED(const DAG::NodeIterator endNodes() const);

    const NodeGraphPart &nodes() const;

    /**
    * Returns a constant reference over a variabe given it's node id.
    * @throw NotFound If no variable's id matches varId.
    */
    virtual const DiscreteVariable &variable(NodeId id) const = 0;

    /**
    * Return id node src discrete var pointer.
    * @throw NotFound If no variable matches var.
    */
    virtual NodeId nodeId(const DiscreteVariable &var) const = 0;

    /// Getter by name
    /// @throw NotFound if no such name exists in the graph.
    virtual NodeId idFromName(const std::string &name) const = 0;

    /// Getter by name
    /// @throw NotFound if no such name exists in the graph.
    virtual const DiscreteVariable &
    variableFromName(const std::string &name) const = 0;

    /// Get an instantiation over all the variables of the model
    virtual void completeInstantiation(Instantiation &I) const;
    /// @}

    /// @name Arc manipulation methods.
    /// @{

    /**
     * Shortcut for this->dag().beginArcs().
     *
     * @deprecated Use for(auto arc : obj.arcs()) instead
     */
    GUM_DEPRECATED(const DAG::ArcIterator beginArcs() const);

    /**
     * Shortcut for this->dag().endArcs().
     *
     * @deprecated Use for(auto arc : obj.arcs()) instead
     */
    GUM_DEPRECATED(const DAG::ArcIterator &endArcs() const);

    const ArcSet &arcs(void) const;
    /// @}

    /// @name Graphical methods
    /// @{

    /**
     * The node's id are coherent with the variables and nodes of the topology.
     * @param clear If false returns the previously created moral graph.
     */
    const UndiGraph &moralGraph(bool clear = true) const;

    /**
     * The topological order stays the same as long as no variable or arcs are
     * added or erased src the topology.
     * @param clear If false returns the previously created topology.
     */
    const Sequence<NodeId> &topologicalOrder(bool clear = true) const;

    /// @}

    /// @return Returns the log10 domain size of the joint probabilty for the
    /// Directed Graphical Model
    double log10DomainSize(void) const;

    protected:
    /**
     * Private copy operator.
     */
    DAGmodel &operator=(const DAGmodel &source);

    /// The DAG of this Directed Graphical Model.
    DAG _dag;

    private:
    /// Returns the moral graph of this DAGModel.
    /// @warning __mutableMoralGraph is assumed to be valid and empty
    void __moralGraph() const;

    /// Returns a topological order of this DAGModel.
    /// @warning __mutableTopologicalOrder is assumed to be valid and empty
    void __topologicalOrder() const;

    /// The moral graph of this Directed Graphical Model.
    mutable UndiGraph *__mutableMoralGraph;

    /// The topology sequence of this Directed Graphical Model.
    mutable Sequence<NodeId> *__mutableTopologicalOrder;

    /// The properties of this Directed Graphical Model.
    /// Initialized using a lazy instantiation.
    mutable HashTable<std::string, std::string> *__propertiesMap;

    /// Return the properties of this Directed Graphical Model and initialize the
    /// hash table is
    /// necessary.
    HashTable<std::string, std::string> &__properties() const;
  };
} // gum

#ifndef GUM_NO_INLINE
#include <agrum/graphicalModels/DAGmodel.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_DAGMODEL_H */
