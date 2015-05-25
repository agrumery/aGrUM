
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
 * @brief Class representing Influence Diagrams
 *
 * @author Jean-Christophe Magnan & Pierre_Henri WUILLEMIN
 *
 */
#ifndef GUM_INF_DIAG_H
#define GUM_INF_DIAG_H

#include <utility>
#include <string>

#include <agrum/config.h>

#include <agrum/core/hashTable.h>

#include <agrum/graphicalModels/DAGmodel.h>

#include <agrum/multidim/potential.h>
#include <agrum/multidim/utilityTable.h>

namespace gum {

  /**
   * @class InfluenceDiagram influenceDiagram.h <agrum/ID/influenceDiagram.h>
   * @brief Class representing an Influence Diagram
   * @ingroup InfluenceDiagram_group
   *
   */
  template <typename GUM_SCALAR> class InfluenceDiagram : public DAGmodel {

    // friend class InfluenceDiagramFactory<GUM_SCALAR>;

    public:
    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    InfluenceDiagram();

    /**
     * Destructor.
     */
    virtual ~InfluenceDiagram();

    /**
     * Copy Constructor
     */
    InfluenceDiagram(const InfluenceDiagram<GUM_SCALAR> &source);

    /**
     * Copy Operator
     */
    InfluenceDiagram<GUM_SCALAR> &
    operator=(const InfluenceDiagram<GUM_SCALAR> &source);

    /// @}

    /// @return Returns a dot representation of this Influence Diagram.
    std::string toDot(void) const;

    /// @return Returns a string representation of this Influence Diagram.
    std::string toString(void) const;

    // ===========================================================================
    /// @name Variable manipulation methods.
    // ===========================================================================
    /// @{

    /**
     * Returns the CPT of a potential variable.
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const Potential<GUM_SCALAR> &cpt(NodeId varId) const;

    /**
     * Returns the utility table of a utility node.
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const UtilityTable<GUM_SCALAR> &utility(NodeId varId) const;

    /**
     * Returns a constant reference to the VariableNodeMap of this Influence Diagram
     */
    virtual const VariableNodeMap &variableNodeMap() const;

    /**
     * Returns true if node is a utility one
     */
    bool isUtilityNode(NodeId varId) const;

    /**
     * Returns true if node is a decision one
     */
    bool isDecisionNode(NodeId varId) const;

    /**
     * Returns true if node is a chance one
     */
    bool isChanceNode(NodeId varId) const;

    /**
     * Returns the number of utility nodes
     */
    Size utilityNodeSize() const;

    /**
     * Returns the number of chance nodes
     */
    Size chanceNodeSize() const;

    /**
     * Returns the number of decision nodes
     */
    Size decisionNodeSize() const;

    /**
    * Returns a constant reference over a variabe given it's node id.
    * @throw NotFound If no variable's id matches varId.
    */
    virtual const DiscreteVariable &variable(NodeId id) const;

    /**
    * Return id node from discrete var pointer.
    * @throw NotFound If no variable matches var.
    */
    virtual NodeId nodeId(const DiscreteVariable &var) const;

    /// Getter by name
    /// @throw NotFound if no such name exists in the graph.
    virtual NodeId idFromName(const std::string &name) const;

    /// Getter by name
    /// @throw NotFound if no such name exists in the graph.
    virtual const DiscreteVariable &variableFromName(const std::string &name) const;

    /**
     * Add a chance variable, it's associate node and it's CPT. The id of the new
     * variable is automatically generated.
     *
     * The implementation of the Potential is by default a MultiDimArray.
     *
     * @param variable The variable added by copy.
     * @param id The chosen id. If 0, the NodeGraphPart will choose.
     * @warning give an id (not 0) should be reserved for rare and specific
     *situations !!!
     * @return the id of the added variable.
     * @throws DuplicateElement if id(<>0) is already used
     */
    NodeId add(const DiscreteVariable &variable, NodeId id = 0);

    /**
    * Add a chance variable, it's associate node and it's CPT. The id of the new
    * variable is automatically generated.
    *
    * The implementation of the Potential is by default a MultiDimArray.
    *
    * @param variable The variable added by copy.
    * @param id The chosen id. If 0, the NodeGraphPart will choose.
    * @warning give an id (not 0) should be reserved for rare and specific situations
    *!!!
    * @return the id of the added variable.
    * @throws DuplicateElement if id(<>0) is already used
    */
    NodeId addChanceNode(const DiscreteVariable &variable, NodeId id = 0);

    /**
     * Add a utility variable, it's associate node and it's UT. The id of the new
     * variable is automatically generated.
     *
     * The implementation of the Utility is by default a MultiDimArray.
     *
     * @param variable The variable added by copy.
     * @param id The chosen id. If 0, the NodeGraphPart will choose.
     * @warning give an id (not 0) should be reserved for rare and specific
     *situations !!!
     * @return the id of the added variable.
     * @throw InvalidAgrument If variable has more than one label
     * @throws DuplicateElement if id(<>0) is already used
     */
    NodeId addUtilityNode(const DiscreteVariable &variable, NodeId id = 0);

    /**
     * Add a decision variable. The id of the new
     * variable is automatically generated.
     *
     * @param variable The variable added by copy.
     * @param id The chosen id. If 0, the NodeGraphPart will choose.
     * @warning give an id (not 0) should be reserved for rare and specific
     *situations !!!
     * @return the id of the added variable.
     * @throws DuplicateElement if id(<>0) is already used
     */
    NodeId addDecisionNode(const DiscreteVariable &variable, NodeId id = 0);

    /**
     * Add a chance variable, it's associate node and it's CPT. The id of the new
     * variable is automatically generated.
     *
     * @param variable The variable added by copy.
     * @param aContent The content used for the variable potential.
     * @param id The chosen id. If 0, the NodeGraphPart will choose.
     * @warning give an id (not 0) should be reserved for rare and specific
     *situations !!!
     * @return the id of the added variable.
     * @throws DuplicateElement if id(<>0) is already used
     */
    NodeId addChanceNode(const DiscreteVariable &variable,
                         MultiDimImplementation<GUM_SCALAR> *aContent,
                         NodeId id = 0);

    /**
     * Add a chance variable, it's associate node and it's CPT. The id of the new
     * variable is automatically generated.
     *
     * @param variable The variable added by copy.
     * @param aContent The content used for the variable utility.
     * @param id The chosen id. If 0, the NodeGraphPart will choose.
     * @warning give an id (not 0) should be reserved for rare and specific
     *situations !!!
     * @throw InvalidAgrument If variable has more than one label
     * @throws DuplicateElement if id(<>0) is already used
     */
    NodeId addUtilityNode(const DiscreteVariable &variable,
                          MultiDimImplementation<GUM_SCALAR> *aContent,
                          NodeId id = 0);

    /**
     * Erase a Variable from the network and remove the variable from
     * all his childs.
     * If no variable matches the id, then nothing is done.
     *
     * @param id The id of the variable to erase.
     */
    void erase(NodeId id);

    /**
     * Erase a Variable from the network and remove the variable from
     * all his childs.
     * If no variable matches, then nothing is done.
     *
     * @param var The reference on the variable to remove.
     */
    void erase(const DiscreteVariable &var);

    /** we allow the user to change the name of a variable
    * @throws DuplicateLabel if this name already exists
    * @throws NotFound Raised if no nodes matches id.
    */
    void changeVariableName(NodeId id, const std::string &new_name);

    /// @}
    // ===========================================================================
    /// @name Arc manipulation methods.
    // ===========================================================================
    /// @{

    /**
     * Add an arc in the ID, and update diagram's potential nodes cpt if necessary.
     *
     * @param head and
     * @param tail as NodeId
     * @throw InvalidEdge If arc.tail and/or arc.head are not in the ID.
     * @throw InvalidEdge if tail is a utility node
     */
    void addArc(NodeId tail, NodeId head);

    /**
     * Add an arc in the ID, and update diagram's potential nodes cpt if necessary.
     *
     * @param head and
     * @param tail as NodeId
     * @throw InvalidEdge If arc.tail and/or arc.head are not in the ID.
     * @throw InvalidEdge if tail is a utility node
     * @deprecated This function has been deprecated. Please use @ref addArc instead
     */
    GUM_DEPRECATED(void insertArc(NodeId tail, NodeId head));

    /**
     * Removes an arc in the ID, and update diagram's potential nodes cpt if
     *necessary.
     *
     * If (tail, head) doesn't exist, the nothing happens.
     * @param arc The arc removed.
     */
    void eraseArc(const Arc &arc);

    /**
    * RRemoves an arc in the ID, and update diagram's potential nodes cpt if
    *necessary.
    *
    * If (tail, head) doesn't exist, the nothing happens.
    * @param head and
    * @param tail as NodeId
    */
    void eraseArc(NodeId tail, NodeId head);

    /// @}

    // ===========================================================================
    /// @name Decisions methods
    // ===========================================================================
    /// @{

    /**
     * True if a directed path exist with all decison nodes
     */
    bool decisionOrderExists() const;

    /**
     * Returns the temporal Graph
     */
    gum::DAG *getDecisionGraph() const;

    /**
     * Returns the sequence of decision nodes in the directed path
     * @throw NotFound if such a path does not exist
     */
    std::vector<NodeId> *getDecisionOrder() const;

    /**
     * Returns true if a path exists between two nodes
     */
    bool existsPathBetween(NodeId src, NodeId dest) const;

    /**
     * Returns partial temporal ordering
     * @throw NotFound if such a sequence does not exist
     */
    const List<NodeSet> &getPartialTemporalOrder(bool clear = true) const;

    /// @}

    protected:
    /// Returns the moral graph of this InfluenceDiagram.
    virtual void _moralGraph(UndiGraph &graph) const;

    /**
     * Removing ancient table
     */
    void _removeTables();

    /**
     * Copying tables from another influence diagram
     */
    void _copyTables(const InfluenceDiagram<GUM_SCALAR> &IDsource);

    /**
     * Add a node
     */
    NodeId _addNode(const DiscreteVariable &variableType, NodeId DesiredId);

    /**
     * Returns the list of children decision for a given nodeId
     */
    Sequence<NodeId> _getChildrenDecision(NodeId parentDecision) const;

    private:
    /// Mapping between id and variable
    VariableNodeMap __variableMap;

    /// Mapping between potential variable's id and their CPT
    NodeProperty<Potential<GUM_SCALAR> *> __potentialMap;
    /// Mapping between utility variable's id and their utility table
    NodeProperty<UtilityTable<GUM_SCALAR> *> __utilityMap;

    /// The temporal order
    mutable List<NodeSet> __temporalOrder;
  };

} /* namespace gum */

#include <agrum/ID/influenceDiagram.tcc>

#endif /* GUM_INF_DIAG_H */

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
