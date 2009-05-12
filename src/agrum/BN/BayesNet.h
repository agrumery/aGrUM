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
 * @brief Class representing Bayesian networks
 *
 * @author Lionel Torti
 */
#ifndef GUM_BAYES_NET_H
#define GUM_BAYES_NET_H
// ============================================================================
#include <utility>
// ============================================================================
#include <agrum/core/debug.h>
#include <agrum/core/exceptions.h>
#include <agrum/core/hashTable.h>
// ============================================================================
#include <agrum/graphs/DAG.h>
#include <agrum/graphs/undiGraph.h>
// ============================================================================
#include <agrum/multidim/potential.h>
// ============================================================================
#include <agrum/BN/variableNodeMap.h>
// ============================================================================
namespace gum {
/**
 * @class BayesNet
 * @brief Class representing a Bayesian Network.
 * @ingroup bn_group
 *
 * After a variable is added to the BN, if it's domain size changes, then the
 * data in it's CPT is lost.
 *
 * For simplicity, building should follow these steps:
 *
 * 1) Instanciate your variables and add them to the BN
 * 2) Add all arcs needed
 * 3) Fill the variable's CPT table
 */
template<typename T_DATA>
class BayesNet: protected VariableNodeMap {

  public:
  // ===========================================================================
  /// @name Constructors / Destructors
  // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    BayesNet();

    /**
     * Destructor.
     */
    ~BayesNet();

    /**
     * Copy constructor.
     */
    BayesNet(const BayesNet<T_DATA>& source);

    /**
     * Copy operator.
     */
    BayesNet<T_DATA>& operator=(const BayesNet<T_DATA>& source);

    /// @}
  // ===========================================================================
  /// @name Variable manipulation methods.
  // ===========================================================================
    /// @{

    /**
     * Returns a constant reference over a variabe given it's node id.
     * @throw NotFound If no variable's id matches varId.
     */
    const DiscreteVariable& variable(NodeId id) const;

    /**
     * Return id node from discrete var pointer.
     * @throw NotFound If no variable matches var.
     */
    NodeId nodeId(const DiscreteVariable &var) const;

    /**
     * Returns the CPT of a variable.
     * @throw NotFound If no variable's id matches varId.
     */
    const Potential<T_DATA>& cpt(NodeId varId) const;

    /**
     * Returns a constant reference over the dag of this Bayes Net.
     */
    const DAG& dag() const;

    /**
     * Returns the number of variables in this bayes net.
     */
    Idx size() const;

    /**
     * Retursn true if this bayes net is empty.
     */
    bool empty() const;

    /**
     * Add a variable, it's associate node and it's CPT. The id of the new
     * variable is automatically generated.
     *
     * The implementation of the Potential is by default a MultiDimArray.
     *
     * @param variable The variable added by copy.
     * @return the id of the added variable.
     */
    NodeId add(const DiscreteVariable& variable);

    /**
     * Add a variable, it's associate node and it's CPT. The id of the new
     * variable is automatically generated.
     *
     * @param variable The variable added by copy.
     * @param aContent The content used for the variable potential.
     * @return the id of the added variable.
     */
    NodeId add(const DiscreteVariable& variable,
               MultiDimImplementation<T_DATA> *aContent);

    /**
     * Erase a Variable from the network and remove the variable from
     * all childs of id.
     * If no variable matches the id, then nothing is done.
     *
     * @param id The id of the variable to erase.
     */
    void erase(NodeId id);

    /**
     * Erase a Variable from the network and remove the variable from
     * all childs of var.
     * If no variable matches, then nothing is done.
     *
     * @param var The reference on the variable to remove.
     */
    void erase(const DiscreteVariable& var);

    /**
     * Shortcut for this->dag().beginNodes().
     */
    const NodeSetIterator beginNodes() const;

    /**
     * Shortcut for this->dag().endNodes().
     */
    const NodeSetIterator endNodes() const;

    /// @}
  // ===========================================================================
  /// @name Arc manipulation methods.
  // ===========================================================================
    /// @{

    /**
     * Add an arc in the BN, and update arc.head's CPT.
     *
     * @param arc The arc added.
     * @return Returns a reference on the real copy added.
     * @throw InvalidEdge If arc.tail and/or arc.head are not in the BN.
     */
    void insertArc(const Arc& arc);

    /**
     * Add an arc in the BN, and update arc.head's CPT.
     *
     * @param head and
     * @param tail as NodeId
     * @return Returns a reference on the real copy added.
     * @throw InvalidEdge If arc.tail and/or arc.head are not in the BN.
     */
    void insertArc(NodeId tail, NodeId head);

    /**
     * Removes an arc in the BN, and update head's CTP.
     *
     * If (tail, head) doesn't exist, the nothing happens.
     * @param arc The arc removed.
     */
    void eraseArc(const Arc& arc);

    /**
     * Removes an arc in the BN, and update head's CTP.
     *
     * If (tail, head) doesn't exist, the nothing happens.
     * @param tail The id of the node (variable) at the tail of the arc.
     * @param head The id of the node (variable) at the head of the arc.
     */
    void eraseArc(NodeId tail, NodeId head);

    /**
     * Shortcut for this->dag().beginArcs().
     */
    const ArcSetIterator beginArcs() const;

    /**
     * Shortcut for this->dag().endArcs().
     */
    const ArcSetIterator& endArcs() const;

    /// @}
  // ===========================================================================
  /// @name Graphical methods
  // ===========================================================================
    /// @{

    /**
     * The node's id are coherent with the variables and nodes of the topology.
     * @param clear If false returns the previously created moral graph.
     */
    const UndiGraph& moralGraph(bool clear=true) const;

    /**
     * The topological order stays the same as long as no variable or arcs are
     * added or erased from the topology.
     * @param clear If false returns the previously created topology.
     */
    const Sequence<NodeId>& getTopologicalOrder(bool clear=true) const;

    /// @}
  // ===========================================================================
  /// @name Deprecated methods
  // ===========================================================================
    /// @{

    /**
     * @warning Deprecated: use add(const DiscreteVariable&).
     */
    virtual NodeId addVariable(const DiscreteVariable& variable);

    /**
     * @warning Deprecated: use add(const DiscreteVariable&, MultiDimImplementation*)
     */
    virtual NodeId addVariable(const DiscreteVariable& variable,
                               MultiDimImplementation<T_DATA> *aContent);

    /**
     * @warning Deprecated: use erase(NodeId) instead.
     */
    virtual void eraseVariable(NodeId id);

    /// @}

  private:

  // ===========================================================================
  /// @name Illegal methods
  // ===========================================================================
    /// @{


    /// @}

    /// The DAG of this bayes net.
    DAG __dag;

    /// Mapping between the variable's id and their CPT.
    /// Property< Potential< T_DATA >* >::onNodes __probaMap;
    HashTable<NodeId, Potential<T_DATA>* > __probaMap;

    /// The moral graph of this bayes net.
    mutable UndiGraph* __moralGraph;

    /// The topology sequence of this bayes net.
    mutable Sequence<NodeId>* __topologicalOrder;

    /// Add all the dag's root nodes in __topologicalOrder
    void __getRootTopologyLevel(NodeSet& uncheckedNodes) const;

    // Add the next level of nodes in the topological order
    void __getNextTopologyLevel(NodeSet& uncheckedNodes) const;
};
} /* namespace gum */
// ============================================================================
#include <agrum/BN/BayesNet.tcc>
// ============================================================================
#endif /* GUM_BAYES_NET_H */
// ============================================================================
