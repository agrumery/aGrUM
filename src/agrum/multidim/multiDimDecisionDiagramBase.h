
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
 * @brief
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 *
 */
#ifndef GUM_MULTI_DIM_DECISION_DIAGRAM_BASE_H
#define GUM_MULTI_DIM_DECISION_DIAGRAM_BASE_H

#include <string>

#include <agrum/core/hashTable.h>
#include <agrum/core/bijection.h>
#include <agrum/core/algorithms/approximationPolicy/approximationPolicy.h>

#include <agrum/multidim/multiDimReadOnly.h>
#include <agrum/multidim/multiDimDecisionDiagramFactoryBase.h>

#include <agrum/graphs/nodeGraphPart.h>

namespace gum {

  template <typename GUM_SCALAR> class MultiDimDecisionDiagramFactoryBase;

  /**
   * @class MultiDimDecisionDiagramBase multiDimDecisionDiagramBase.h
   *<agrum/multidim/multiDimDecisionDiagramBase.h>
   * @brief Class implementingting an decision diagram model
   * Contains all methods used by multidimdecisiondiagram, except for those
   *concerning approximation policy
   * For concrete usage of such multidim, please refer to multiDimDecisionDiagram
   * @ingroup multidim_group
   *
   */
  template <typename GUM_SCALAR>

  class MultiDimDecisionDiagramBase
      : public MultiDimReadOnly<GUM_SCALAR>,
        public virtual ApproximationPolicy<GUM_SCALAR> {

    public:
    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    MultiDimDecisionDiagramBase();

    /**
     * Copy constructor.
     * @warning Deprecatedbecause of multiplyByScalar, please consider instead
     * newFactory
     * and then either copy or multiplyByScalar
     */
    MultiDimDecisionDiagramBase(
        const MultiDimDecisionDiagramBase<GUM_SCALAR> &source);

    /**
     * Destructor.
     */
    virtual ~MultiDimDecisionDiagramBase();

    /// @}

    // ===========================================================================
    /// @name Accessors/Modifiers
    // ===========================================================================
    /// @{

    /**
     * Returns the name of the miltiDim implementation
     */
    virtual const std::string &name() const;

    /**
     * Returns valued pointed by inst
     */
    virtual GUM_SCALAR get(const Instantiation &inst) const;

    /**
     * Returns the terminal node associated to value pointed by inst
     */
    virtual NodeId getNode(const Instantiation &inst) const;

    /// @}

    // ===========================================================================
    /// @name Implementation of MultiDimInterface
    // ===========================================================================
    /// @{

    /**
     * Adds a new var to the variables of the multidimensional matrix.
     * @throw OperationNotAllowed cause it's not authorize on read only multidim
     */
    virtual void add(const DiscreteVariable &v);

    /**
     * Removes a var from the variables of the multidimensional matrix.
     * @throw OperationNotAllowed cause it's not authorize on read only multidim
     */
    virtual void erase(const DiscreteVariable &v);

    /**
     * Returns the real number of parameter used in this table.
     */
    virtual Size realSize() const;

    /**
     * Indicates if diagram has no value inside at all
     */
    bool empty() const;

    /// @}

    // ===========================================================================
    /// @name Slave management and extension due to slave management
    // ===========================================================================
    /// @{

    /**
     * Listen to change in a given Instantiation virtual
     */
    virtual void changeNotification(Instantiation &i, const DiscreteVariable *var,
                                    const Idx &oldval, const Idx &newval);

    /**
     * Listen to setFirst in a given Instantiation
     */
    virtual void setFirstNotification(Instantiation &i);

    /**
     * Listen to setLast in a given Instantiation
     */
    virtual void setLastNotification(Instantiation &i);

    /**
     * Listen to increment in a given Instantiation
     */
    virtual void setIncNotification(Instantiation &i);

    /**
     * Listen to decrement in a given Instantiation
     */
    virtual void setDecNotification(Instantiation &i);

    /**
     * Listen to an assignement of value in a given Instantiation
     */
    virtual void setChangeNotification(Instantiation &i);

    /// @}

    // ===========================================================================
    /// @name Copy Methods
    // ===========================================================================
    /// @{

    /**
     * Makes a copy of given decision diagram
     * boolean parameter is used to indicates if further modifications will be made
     * before considering
     * this multidim as a read only
     * @throw OperationNotAllowed if already instantiate cause this is a read only
     */
    void copy(const MultiDimDecisionDiagramBase<GUM_SCALAR> &src,
              bool fullInstanciation = true);

    /**
     * Removes all variables in this Container and copy content from src, variable
     * included
     * @throw OperationNotAllowed cause this is a read only and a decision diagram (
     * which
     * makes quiet complicate any copy operation as a matter of fact )
     */
    void copy(const MultiDimContainer<GUM_SCALAR> &src);

    /**
     * Performs a copy of given in parameter table plus a change of variable based
     *upon bijection given in parameter.
     **/
    void copyAndReassign(const MultiDimDecisionDiagramBase<GUM_SCALAR> *source,
                         const Bijection<const DiscreteVariable *,
                                         const DiscreteVariable *> &old2new);

    /// @}

    // ===========================================================================
    /// @name Various Methods
    // ===========================================================================
    /// @{

    /**
     * Displays the multidim.
     */
    virtual const std::string toString() const;

    /**
     * Displays the internal representation of i.
     */
    virtual const std::string toString(const Instantiation *i) const;

    /**
     * This method creates a clone of this object, without its content
     * (including variable), you must use this method if you want to ensure
     * that the generated object has the same type than the object containing
     * the called newFactory()
     * For example :
     *   MultiDimArray<double> y;
     *   MultiDimContainer<double>* x = y.newFactory();
     * Then x is a MultiDimArray<double>*
     *
     * @warning you must desallocate by yourself the memory
     * @return an empty clone of this object with the same type
     */
    MultiDimContainer<GUM_SCALAR> *newFactory() const = 0;

    /**
     * Displays the DecisionDiagramBase in the dot format
     */
    std::string toDot(const std::string = "no name") const;

    /**
     * Returns the number of variable truly present in diagram
     */
    Size diagramVarSize() const;

    /// @}

    // ===========================================================================
    /// @name Diagram Handlers
    // ===========================================================================
    /// @{

    /**
     * Returns the id of the root node from the diagram
     */
    const NodeId &root() const { return __root; };

    /**
     * Returns the node graph part
     */
    const NodeGraphPart &nodesMap() const { return __graph; };

    /**
     * Returns values map
     * @throw InvalidNode if node is terminal
     */
    const Bijection<NodeId, GUM_SCALAR> &valuesMap() const { return __valueMap; };

    /// Mapping between id and variable
    const HashTable<NodeId, const DiscreteVariable *> &variableMap() const {
      return __variableMap;
    };

    /// Mapping between var and node
    const HashTable<const DiscreteVariable *, List<NodeId> *> &
    var2NodeIdMap() const {
      return __var2NodeIdMap;
    };

    /// Mapping between var and node
    const HashTable<const DiscreteVariable *, std::vector<Idx> *> &
    varUsedModalitiesMap() const {
      return __varUsedModalitiesMap;
    };

    /// Mapping between variable's values and associated node
    const HashTable<NodeId, std::vector<NodeId> *> &arcMap() const {
      return __arcMap;
    };

    /// Mapping between variable's values and associated node
    const HashTable<NodeId, NodeId> &defaultArcMap() const {
      return __defaultArcMap;
    };

    /**
     * Returns true if node is a chance one
     */
    bool isTerminalNode(NodeId varId) const;

    /**
     * Returns value associated to given node
     * @throw InvalidNode if node isn't terminal
     */
    const GUM_SCALAR &nodeValue(NodeId n) const;
    const GUM_SCALAR &unsafeNodeValue(NodeId n) const;

    /**
     * Returns associated variable of given node
     * @throw InvalidNode if Node is terminal
     */
    const DiscreteVariable *nodeVariable(NodeId n) const;
    const DiscreteVariable *unsafeNodeVariable(NodeId n) const;

    /**
     * Returns node's sons map
     * @throw InvalidNode if node is terminal
     */
    const std::vector<NodeId> *nodeSons(NodeId n) const;
    const std::vector<NodeId> *unsafeNodeSons(NodeId n) const;

    /**
     * Returns true if node has a default son
     */
    bool hasNodeDefaultSon(NodeId n) const;
    bool unsafeHasNodeDefaultSon(NodeId n) const;

    /**
     * Returns node's default son
     * @throw InvalidNode if node is terminal
      // @throw NotFound if node doesn't have a default son
     */
    NodeId nodeDefaultSon(NodeId n) const;
    NodeId unsafeNodeDefaultSon(NodeId n) const;

    /**
     * Returns associated nodes of the variable pointed by the given node
     */
    const List<NodeId> *variableNodes(const DiscreteVariable *v) const;

    /**
     * Returns associated nodes of the variable pointed by the given node
     */
    const std::vector<Idx> *variableUsedModalities(const DiscreteVariable *v) const;

    /**
     * Returns true if variable is in diagram
     */
    bool isInDiagramVariable(const DiscreteVariable *v) const;

    /**
     * Returns a hashtable containing for each node a list of variable
     * This method looks, for each path in the diagram, if a var does not precede
     * others in
     * the given in parameter order.
     * The third parameter is the returned structure.
     */
    void findRetrogradeVariables(const Sequence<const DiscreteVariable *> *varsSeq,
                                 HashTable<NodeId, Set<const DiscreteVariable *> *> *
                                     retrogradeVariablesTable) const;

    /**
     * Extracts sub decision diagram from specified root node
     * @throw NotFound if node does not exist
     */
    MultiDimDecisionDiagramBase<GUM_SCALAR> *
    extractSubDecisionDiagram(const NodeId root) const;

    /**
     * Instantiates this diagram by multiplying leaf of given diagram by factor
     */
    void multiplyByScalar(const MultiDimDecisionDiagramBase<GUM_SCALAR> *m,
                          GUM_SCALAR factor);

    ///@}

    // ===========================================================================
    /// @name Approximation Handling functions
    // ===========================================================================
    /// @{

    /**
     * Returns a factory that used same approximation pattern
     */
    virtual MultiDimDecisionDiagramFactoryBase<GUM_SCALAR> *getFactory() const = 0;

    /**
     * Returns a factory that used same approximation pattern
     * Allows to set parameter for that approximation
     */
    virtual MultiDimDecisionDiagramFactoryBase<GUM_SCALAR> *
    getFactory(const ApproximationPolicy<GUM_SCALAR> &md) const = 0;

    ///@}

    // ===========================================================================
    /// @name Structure instantiation
    // ===========================================================================
    /// @{

    /**
     * Puts the multiDim in instantiation mode
     * @throw OperationNotAllowed if diagram has already been instanciated
     */
    void beginInstantiation();

    /**
     * Puts the multiDim out of instantiation mode
     */
    void endInstantiation();

    /**
     * Sets once and for all variable sequence.
     * @throw OperationNotAllowed if diagram has already been instanciated or if not
     * in instanciation mode
     */
    void setVariableSequence(const Sequence<const DiscreteVariable *> &varList);

    /**
     * Sets once and for all nodes of the diagram.
     * @throw OperationNotAllowed if diagram has already been instanciated or if not
     * in instanciation mode
     */
    void setDiagramNodes(const NodeGraphPart &model);

    /**
     * Sets the map linking variable to all nodes bond to it
     * @throw OperationNotAllowed if diagram has already been instanciated or if not
     * in instanciation mode
     */
    void setVar2NodeMap(
        const HashTable<const DiscreteVariable *, List<NodeId> *> var2NodeMap);

    /**
     * Sets the map linking variable to used modality in graph
     * @throw OperationNotAllowed if diagram has already been instanciated or if not
     * in instanciation mode
     */
    void setVarUsedModalitiesMap(const HashTable<
        const DiscreteVariable *, std::vector<Idx> *> varUsedModalitiesMap);

    /**
     * Binds once and for all nodes to variables.
     * @throw OperationNotAllowed if diagram has already been instanciated or if not
     * in instanciation mode
     */
    void setVariableMap(const NodeProperty<const DiscreteVariable *> &varMap);

    /**
     * Binds once and for all terminal nodes to value.
     * @throw OperationNotAllowed if diagram has already been instanciated or if not
     * in instanciation mode
     */
    void setValueMap(const Bijection<NodeId, GUM_SCALAR> &valueMap);

    /**
     * Links once and for all nodes of the graph.
     * @throw OperationNotAllowed if diagram has already been instanciated or if not
     * in instanciation mode
     */
    void setDiagramArcs(const NodeProperty<std::vector<NodeId> *> &arcMap,
                        const NodeProperty<NodeId> &defaultArcMap);

    /**
    * Sets once and for all root node.
    * @throw OperationNotAllowed if diagram has already been instanciated or if not
    * in instanciation mode
    */
    void setRoot(const NodeId &root);

    /// @}

    protected:
    /**
     * Returns data addressed by inst
     */
    virtual GUM_SCALAR &_get(const Instantiation &inst) const;

    /**
    * Supposed to replace var x by y. But not authorized in a
    * MultiDimDecisionDiagramBase
    * @throw OperationNotAllowed without condition.
    */
    virtual void _swap(const DiscreteVariable *x, const DiscreteVariable *y);

    private:
    /**
     * Fills the HashTable given in parameter with for each node a list of all
     * variable that preceeds it in diagram
     */
    void __makePreceedingVariablesLists(HashTable<
        NodeId, Set<const DiscreteVariable *> *> *preceedingVariablesTable) const;

    /**
     * Builds result as hashtable containing for each node a list of variable
     * This method looks, for each path in the diagram, if a var does not precede
     * others in
     * the given in parameter order.
     * Since it's a recursive deep first research, the second parameter give us the
     * current node id
     * the node from which we start that research.
     * The third parameter is the returned structure.
     * The foorth parameter is the sequence of variable in the currently explored
     * path.
     */
    void __findRetorgradeVariables(
        const Sequence<const DiscreteVariable *> *varsSeq, const NodeId currentNode,
        HashTable<NodeId, Set<const DiscreteVariable *> *> *retrogradeVarTable,
        HashTable<NodeId, Set<const DiscreteVariable *> *> *preceedingVariablesTable,
        List<NodeId> *visitedNodes) const;

    /// Mapping between terminal nodes and their values
    Bijection<NodeId, GUM_SCALAR> __valueMap;

    /// The graph hidden behin this multidim
    NodeGraphPart __graph;

    /// The id of the root of the graph
    NodeId __root;

    /// Mapping between id and variable
    NodeProperty<const DiscreteVariable *> __variableMap;

    /// Mapping between var and node
    HashTable<const DiscreteVariable *, List<NodeId> *> __var2NodeIdMap;

    /// Mapping between var and node
    HashTable<const DiscreteVariable *, std::vector<Idx> *> __varUsedModalitiesMap;

    /// Mapping between variable's values and associated node
    NodeProperty<std::vector<NodeId> *> __arcMap;

    /// Mapping between variable's values and associated node
    NodeProperty<NodeId> __defaultArcMap;

    /// Name of this multiDim
    std::string __name;

    /// Just a boolean to indicates if diagram has been instanciated or not
    bool __isInstanciated;

    /// And another one to indicates if diagram is in instanciation or not
    bool __instanciationModeOn;
  };

} /* namespace gum */

#include <agrum/multidim/multiDimDecisionDiagramBase.tcc>

#endif /* GUM_MULTI_DIM_DECISION_DIAGRAM_BASE_H */

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
