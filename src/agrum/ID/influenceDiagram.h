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
 * @brief Class representing Influence Diagrams
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 *
 */

#ifndef GUM_INF_DIAG_H
#define GUM_INF_DIAG_H

#include <string>
#include <utility>

#include <agrum/agrum.h>

#include <agrum/tools/graphicalModels/DAGmodel.h>
#include <agrum/tools/multidim/potential.h>

namespace gum {

  /**
   * @class InfluenceDiagram
   * @headerfile influenceDiagram.h <agrum/ID/influenceDiagram.h>
   * @brief Class representing an Influence Diagram
   * @ingroup InfluenceDiagram_group
   *
   */
  template < typename GUM_SCALAR >
  class InfluenceDiagram: public DAGmodel {
    // friend class InfluenceDiagramFactory<GUM_SCALAR>;
    public:
    /**
     * Create an Influence Diagram with a dot-like syntax which specifies:
     *   - the structure "a->*b->$c;b->d<-*e;".
     *   - the prefix of a variable can be :
     *      - nothing : chance node (a,d)
     *      - * :  decision node (*b)
     *      - $ : utility node ($c)     *
     *   - the type of the chance or decision variables with different syntax:
     *     + by default, a variable is a gum::RangeVariable using the default
     * domainSize (second argument)
     *     + with "a[10]", the variable is a gum::RangeVariable using 10 as
     * domainSize (from 0 to 9)
     *     + with "a[3,7]", the variable is a gum::RangeVariable using a domainSize
     * from 3 to 7
     *     + with "a[1,3.14,5,6.2]", the variable is a gum::DiscretizedVariable
     * using the given ticks (at least 3 values)
     *     + with "a{top|middle|bottom}", the variable is a gum::LabelizedVariable
     * using the given labels.
     *
     * Note that if the dot-like string contains such a specification more than
     * once for a variable, the first specification will be used.
     *
     * @param dotlike the string containing the specification
     * @param domainSize the default domain size for chance and decision variables
     * @return the resulting influence diagram
     */
    static InfluenceDiagram< GUM_SCALAR > fastPrototype(const std::string& dotlike,
                                                        Size               domainSize = 2);

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
    ~InfluenceDiagram() override;

    /**
     * Copy Constructor
     */
    InfluenceDiagram(const InfluenceDiagram< GUM_SCALAR >& source);

    /**
     * Copy Operator
     */
    InfluenceDiagram< GUM_SCALAR >& operator=(const InfluenceDiagram< GUM_SCALAR >& source);

    /// @}

    /// @return Returns a dot representation of this Influence Diagram.
    std::string toDot() const;

    /// @return Returns a string representation of this Influence Diagram.
    std::string toString() const;

    void clear();

    // ===========================================================================
    /// @name Variable manipulation methods.
    // ===========================================================================
    /// @{

    /**
     * Returns the CPT of a potential variable.
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const Potential< GUM_SCALAR >& cpt(NodeId varId) const;
    virtual const Potential< GUM_SCALAR >& cpt(std::string name) const final {
      return cpt(idFromName(name));
    };

    /**
     * Returns the utility table of a utility node.
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const Potential< GUM_SCALAR >& utility(NodeId varId) const;
    virtual const Potential< GUM_SCALAR >& utility(std::string name) const final {
      return utility(idFromName(name));
    };

    /**
     * Returns a constant reference to the VariableNodeMap of this Influence
     * Diagram
     */
    const VariableNodeMap& variableNodeMap() const final;

    /**
     * Returns true if node is a utility one
     */
    bool isUtilityNode(NodeId varId) const;
    bool isUtilityNode(const std::string& name) const { return isUtilityNode(idFromName(name)); };
    /**
     * Returns true if node is a decision one
     */
    bool isDecisionNode(NodeId varId) const;
    bool isDecisionNode(const std::string& name) const { return isDecisionNode(idFromName(name)); };

    /**
     * Returns true if node is a chance one
     */
    bool isChanceNode(NodeId varId) const;
    bool isChanceNode(const std::string& name) const { return isChanceNode(idFromName(name)); };
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
     * Returns a constant reference over a variable given it's node id.
     * @throw NotFound If no variable's id matches varId.
     */
    const DiscreteVariable& variable(NodeId id) const final;
    const DiscreteVariable& variable(const std::string& name) const {
      return variable(idFromName(name));
    };

    /**
     * Return id node from discrete var pointer.
     * @throw NotFound If no variable matches var.
     */
    NodeId nodeId(const DiscreteVariable& var) const final;

    /// Getter by name
    /// @throw NotFound if no such name exists in the graph.
    NodeId idFromName(const std::string& name) const final;

    /// Getter by name
    /// @throw NotFound if no such name exists in the graph.
    const DiscreteVariable& variableFromName(const std::string& name) const final;

    /**
     * Add a chance variable, it's associate node and it's CPT. The id of the
     *new variable is automatically generated.
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
    NodeId add(const DiscreteVariable& variable, NodeId id = 0);

    /**
     * Add a chance variable, it's associate node and it's CPT. The id of the new
     * variable is automatically generated.
     *
     * The implementation of the Potential is by default a MultiDimArray.
     *
     * @param variable The variable added by copy.
     * @param id The chosen id. If 0, the NodeGraphPart will choose.
     * @warning give an id (not 0) should be reserved for rare and specific
     *situations
     *!!!
     * @return the id of the added variable.
     * @throws DuplicateElement if id(<>0) is already used
     */
    NodeId addChanceNode(const DiscreteVariable& variable, NodeId id = 0);

    /**
     * Add a utility variable, it's associate node and it's UT. The id of the
     *new variable is automatically generated.
     *
     * The implementation of the Utility is by default a MultiDimArray.
     *
     * @param variable The variable added by copy.
     * @param id The chosen id. If 0, the NodeGraphPart will choose.
     * @warning give an id (not 0) should be reserved for rare and specific
     *situations !!!
     * @return the id of the added variable.
     * @throw InvalidArgument If variable has more than one label
     * @throws DuplicateElement if id(<>0) is already used
     */
    NodeId addUtilityNode(const DiscreteVariable& variable, NodeId id = 0);

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
    NodeId addDecisionNode(const DiscreteVariable& variable, NodeId id = 0);

    /**
     * Add a chance variable, it's associate node and it's CPT. The id of the
     *new
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
    NodeId addChanceNode(const DiscreteVariable&               variable,
                         MultiDimImplementation< GUM_SCALAR >* aContent,
                         NodeId                                id = 0);

    /**
     * Add a chance variable, it's associate node and it's CPT. The id of the
     *new
     * variable is automatically generated.
     *
     * @param variable The variable added by copy.
     * @param aContent The content used for the variable utility.
     * @param id The chosen id. If 0, the NodeGraphPart will choose.
     * @warning give an id (not 0) should be reserved for rare and specific
     *situations !!!
     * @throw InvalidArgument If variable has more than one label
     * @throws DuplicateElement if id(<>0) is already used
     */
    NodeId addUtilityNode(const DiscreteVariable&               variable,
                          MultiDimImplementation< GUM_SCALAR >* aContent,
                          NodeId                                id = 0);


    /**
     * Add a chance, a decision or an utility variable from a description in "fast" syntax.
     *   - see addChancdeNode for the syntax of the type of variable
     *   - if the description starts with "*", its a decision node
     *   - if the description starts with "$", irs an utility node
     *   - otherwise, it is a chance node.
     *
     * @param fast_description(: str) following "fast" syntax description
     * @param default_nbrmod(: int) nbr of modality if fast_description do not indicate it.
     * default_nbrmod=1 is the way to create a variable with only one value (for instance for
     * reward
     * in influence diagram).
     *
     * @throws DuplicateLabel Raised if variable.name() is already used in this
     *                        gum::InfluenceDiagram.
     * @throws NotAllowed if nbrmod<2
     */
    NodeId add(const std::string& fast_description, unsigned int default_nbrmod = 2);

    /**
     * Add a chance variable from a description in "fast" syntax.
     *   - a : range variable from 0 to default_nbrmod-1
     *   - a[5] : range variable from 0 to 5
     *   - a[-3,5] : range variable from -3 to 5
     *   - a[1,3.14,5,3] : discretized variable
     *   - a{x|y|z} : labelized variable
     *   - a{-3|0|3|100} : integer variable
     *
     * @param fast_description(: str) following "fast" syntax description
     * @param default_nbrmod(: int) nbr of modality if fast_description do not indicate it.
     * default_nbrmod=1 is the way to create a variable with only one value (for instance for
     * reward
     * in influence diagram).
     *
     * @throws DuplicateLabel Raised if variable.name() is already used in this
     *                        gum::InfluenceDiagram.
     * @throws NotAllowed if nbrmod<2
     */
    NodeId addChanceNode(const std::string& fast_description, unsigned int default_nbrmod = 2);

    /**
     * Add a utility variable from a description in "fast" syntax with only one value.
     *   - a : range variable from 0 to default_nbrmod-1
     *   - a[x,x] : range variable from x to x
     *   - a{value} : labelized variable with only one label "value"
     *
     * @param fast_description(: str) following "fast" syntax description
     */
    NodeId addUtilityNode(const std::string& fast_description);

    /**
     * Add a decision variable from a description in "fast" syntax.
     *   - a : range variable from 0 to default_nbrmod-1
     *   - a[5] : range variable from 0 to 5
     *   - a[-3,5] : range variable from -3 to 5
     *   - a[1,3.14,5,3] : discretized variable
     *   - a{x|y|z} : labelized variable
     *   - a{-3|0|3|100} : integer variable
     *
     * @param fast_description(: str) following "fast" syntax description
     * @param default_nbrmod(: int) nbr of modality if fast_description do not indicate it.
     *
     * @throws DuplicateLabel Raised if variable.name() is already used in this
     *                        gum::InfluenceDiagram.
     * @throws NotAllowed if nbrmod<2
     */
    NodeId addDecisionNode(const std::string& fast_description, unsigned int default_nbrmod = 2);


    /**
     * Erase a Variable from the network and remove the variable from
     * all his children.
     * If no variable matches the id, then nothing is done.
     *
     * @param id The id of the variable to erase.
     */
    void erase(NodeId id);
    void erase(const std::string& name) { erase(idFromName(name)); };

    /**
     * Erase a Variable from the network and remove the variable from
     * all his children.
     * If no variable matches, then nothing is done.
     *
     * @param var The reference on the variable to remove.
     */
    void erase(const DiscreteVariable& var);

    /** we allow the user to change the name of a variable
     * @throws DuplicateLabel if this name already exists
     * @throws NotFound Raised if no nodes matches id.
     */
    void changeVariableName(NodeId id, const std::string& new_name);
    void changeVariableName(const std::string& name, const std::string& new_name) {
      changeVariableName(idFromName(name), new_name);
    }

    /// @}
    // ===========================================================================
    /// @name Arc manipulation methods.
    // ===========================================================================
    /// @{

    /**
     * Add an arc in the ID, and update diagram's potential nodes cpt if
     *necessary.
     *
     * @param head and
     * @param tail as NodeId
     * @throw InvalidEdge If arc.tail and/or arc.head are not in the ID.
     * @throw InvalidEdge if tail is a utility node
     */
    void addArc(NodeId tail, NodeId head);
    void addArc(const std::string& tail, const std::string& head) {
      addArc(idFromName(tail), idFromName(head));
    }

    /**
     * Removes an arc in the ID, and update diagram's potential nodes cpt if
     *necessary.
     *
     * If (tail, head) doesn't exist, the nothing happens.
     * @param arc The arc removed.
     */
    void eraseArc(const Arc& arc);

    /**
     * Removes an arc in the ID, and update diagram's potential nodes cpt if
     *necessary.
     *
     * If (tail, head) doesn't exist, the nothing happens.
     * @param head and
     * @param tail as NodeId
     */
    void eraseArc(NodeId tail, NodeId head);
    void eraseArc(const std::string& tail, const std::string& head) {
      eraseArc(idFromName(tail), idFromName(head));
    }

    /// @}

    // ===========================================================================
    /// @name Decisions methods
    // ===========================================================================
    /// @{

    /**
     * True if a directed path exist with all decision nodes
     */
    bool decisionOrderExists() const;

    /**
     * Returns the temporal Graph
     */
    gum::DAG* getDecisionGraph() const;

    /**
     * Returns the sequence of decision nodes in the directed path
     * @throw NotFound if such a path does not exist
     */
    std::vector< NodeId > decisionOrder() const;

    /**
     * Returns true if a path exists between two nodes
     */
    bool existsPathBetween(NodeId src, NodeId dest) const;
    bool existsPathBetween(const std::string& src, const std::string& dest) const {
      return existsPathBetween(idFromName(src), idFromName(dest));
    }

    /**
     * Returns partial temporal ordering
     * @throw NotFound if such a sequence does not exist
     */
    const List< NodeSet >& getPartialTemporalOrder(bool clear = true) const;

    /// @}

    protected:
    /// Returns the moral graph of this InfluenceDiagram.
    virtual void moralGraph_(UndiGraph& graph) const;

    /**
     * Removing ancient table
     */
    void removeTables_();

    /**
     * Copying tables from another influence diagram
     */
    void copyStructureAndTables_(const InfluenceDiagram< GUM_SCALAR >& IDsource);

    /**
     * Add a node
     */
    NodeId addNode_(const DiscreteVariable& variableType, NodeId DesiredId);

    /**
     * Returns the list of children decision for a given nodeId
     */
    Sequence< NodeId > getChildrenDecision_(NodeId parentDecision) const;

    private:
    /// Mapping between id and variable
    VariableNodeMap _variableMap_;

    /// Mapping between potential variable's id and their CPT
    NodeProperty< Potential< GUM_SCALAR >* > _potentialMap_;
    /// Mapping between utility variable's id and their utility table
    NodeProperty< Potential< GUM_SCALAR >* > _utilityMap_;

    /// The temporal order
    mutable List< NodeSet > _temporalOrder_;
  };

} /* namespace gum */

#include <agrum/ID/influenceDiagram_tpl.h>

#endif /* GUM_INF_DIAG_H */
