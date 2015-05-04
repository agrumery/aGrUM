
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
 * @brief Class allowing creation of Algebraic Decision Diagram
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 *
 */
#ifndef GUM_MULTI_DIM_DECISION_DIAGRAM_FACTORY_BASE_H
#define GUM_MULTI_DIM_DECISION_DIAGRAM_FACTORY_BASE_H

#include <agrum/core/hashTable.h>
#include <agrum/core/bijection.h>
#include <agrum/core/algorithms/approximationPolicy/approximationPolicy.h>

#include <agrum/multidim/multiDimDecisionDiagramBase.h>

#include <agrum/graphs/diGraph.h>

namespace gum {

  template <typename GUM_SCALAR> class MultiDimDecisionDiagramBase;

  /**
   * @class MultiDimDecisionDiagramFactoryBase multiDimDecisionDiagramFactoryBase.h
   *<agrum/multidim/multiDimDecisionDiagramFactoryBase.h>
   * @brief Class implementingting an algebraic decision diagram factory
   * Can work either with checks on variables to ensure order property is respected
   *in diagram or without (NoVariableCheckMode)
   * @ingroup multidim_group
   *
   */
  template <typename GUM_SCALAR>

  class MultiDimDecisionDiagramFactoryBase
      : public virtual ApproximationPolicy<GUM_SCALAR> {

    public:
    // ===========================================================================
    /// @name Constructors, Destructors.
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    MultiDimDecisionDiagramFactoryBase();

    /**
     * Destructor.
     * @warning Since getMultiDimDecisionDiagram does a copy of DecisionDiagram
     * deleting this factory will not destroy properties on node of any instantiated
     * DecisionDiagram.
     */
    virtual ~MultiDimDecisionDiagramFactoryBase();

    /// @}

    // ===========================================================================
    /// @name Graph manipulation methods.
    // ===========================================================================
    /// @{

    /**
     * Returns the sequence of variables on which is based the diagram construction
     */
    const Sequence<const DiscreteVariable *> &variablesSequence();

    /**
     * To be done before any insertion of node linked to variable.
     * Specifies the order between variable in the diagram
     *
     * @param s a sequence containing the variable (wich will be the referent )
     */
    void setVariablesSequence(Sequence<const DiscreteVariable *> s);

    // ===========================================================================
    /// @name Nodes manipulation methods.
    // ===========================================================================
    /// @{

    /**
     * Sets root node of decision diagram
     */
    void setRootNode(const NodeId nody);

    /**
     * Adds a variable and its associate non terminal node. The id of the new
     * variable is automatically generated.
     *
     * @param var The variable added by copy.
     * @throw OperationNotAllowed if no sequence of variable have yet been specified
     * @return the id of the added variable.
     */
    ///@{
    NodeId addNonTerminalNode(const DiscreteVariable *var);
    NodeId unsafeAddNonTerminalNode(const DiscreteVariable *var);
    ///@}

    /**
     * Adds a non-terminal node in the diagram linked to given variable.
     * Inserts also arc between that node and mentionned node in the given hashtable
     * @param var the variable added by copy
     * @param nodeArcMap the variables to link
     * @param defaultArcTo the variable to link by default (optional)
     * @throw OperationNotAllowed if no sequence of variable have yet been specified
     * Returns the id of that node
     */
    ///@{
    NodeId addNonTerminalNodeWithArcs(const DiscreteVariable *var,
                                      const std::vector<NodeId> &nodeArcMap,
                                      NodeId defaultArcTo = 0);
    NodeId unsafeAddNonTerminalNodeWithArcs(const DiscreteVariable *var,
                                            const std::vector<NodeId> &nodeArcMap,
                                            NodeId defaultArcTo = 0);
    ///@}

    /**
     * Checks if a node with same variable, same sons and same default son does not
     * already exists in diagram
     * returns a pair constituing of a boolean that indicates if such a node exists,
     * and his nodeid
     */
    std::pair<bool, NodeId> checkredundancy(const DiscreteVariable *var,
                                            const std::vector<NodeId> &nodeArcMap,
                                            NodeId defaultArcTo = 0);

    /**
     * Adds a value and it's associate terminal node. The id of the new
     * variable is automatically generated.
     *
     * @param value The value added by copy.
     * @return the id of the added variable.
     *
     * If a terminal node with such value already exists, its value will be return
     *instead.
     */
    NodeId addTerminalNode(const GUM_SCALAR &value);

    /**
     * Erases a node from the diagram
     * If no variable matches the id, then nothing is done.
     *
     * @param id The id of the variable to erase.
     * @throw NotFound if node isn't in diagram
     */
    void eraseNode(NodeId id);

    /// @}

    // ===========================================================================
    /// @name Arcs manipulation methods.
    // ===========================================================================
    /// @{

    /**
     * Adds an arc in the DD
     *
     * @param from and
     * @param to as NodeId
     * @param modality the modality on which arc is bind
     * @throw NotFound If from and/or tail are not in the DD.
     * @throw InvalidNode if head is a terminal node
     * @throw OperationNotAllowed if arc doesn't respect variable order property
     * @throw DuplicateElement if another arc linking those nodes already exists
     */
    ///@{
    /// @deprecated use addArc instead
    GUM_DEPRECATED(void insertArc(NodeId from, NodeId to, Idx modality));

    /// @deprecated use addArc instead
    GUM_DEPRECATED(void unsafeInsertArc(NodeId from, NodeId to, Idx modality));

    void addArc(NodeId from, NodeId to, Idx modality);
    void unsafeAddArc(NodeId from, NodeId to, Idx modality);
    ///@}

    /**
     * Adds a default arc in the DD
     *
     * @param from and
     * @param to as NodeId
     * @throw NotFound If from and/or tail are not in the DD.
     * @throw InvalidNode if head is a terminal node
     * @throw OperationNotAllowed if arc doesn't respect variable order property
     * @throw DuplicateElement if another arc linking those nodes already exists
     */
    GUM_DEPRECATED(void insertDefaultArc(NodeId from, NodeId to));
    GUM_DEPRECATED(void unsafeInsertDefaultArc(NodeId from, NodeId to));
    void addDefaultArc(NodeId from, NodeId to);
    void unsafeAddDefaultArc(NodeId from, NodeId to);

    /**
     * Erases an arc in the DD
     *
     * @param from and
     * @param to as NodeId
     * @throw InvalidArc If arc does not exist
     * @warning due to the possibility that several arc with different value have the
     *same from and to,
     * if several arcs have different value but same parent and child, this method
     *will erase all of them .
     * If you want to erase a specific one, use eraseSpecificArc
     */
    void eraseArc(NodeId from, NodeId to);

    /**
     * Erases an arc in the DD
     *
     * @param from and
     * @param to as NodeId
     * @param modality the modality corresponding to the to delete arc
     * @throw InvalidArc If arc does not exist
     */
    void eraseSpecificArc(NodeId from, NodeId to, Idx modality);

    /// @}

    // ===========================================================================
    /// @name Getting methods
    // ===========================================================================
    /// @{

    /**
     * Displays the current DecisionDiagram structure
     */
    void showProperties();

    /**
     * Returns the value of associated node if its a terminal one
     * @throw NotFound if it's not a terminal node
     */
    GUM_SCALAR nodeValue(NodeId node);

    /// @}

    /**
     * Returns the multidimDecisionDiagram made
     */
    virtual MultiDimDecisionDiagramBase<GUM_SCALAR> *
    getMultiDimDecisionDiagram(bool fillWithDefaultArc = true,
                               GUM_SCALAR defaultValue = 0,
                               bool doCompress = false) = 0;

    /**
     * Sets the factory from an already existing diagram
     */
    void setMultiDimDecisionDiagram(
        const MultiDimDecisionDiagramBase<GUM_SCALAR> *source);

    /**
     * Swaps two variables in the build on diagram
     */
    void swap(const DiscreteVariable *x, const DiscreteVariable *y);

    /**
     * Resets the factory
     */
    void clear();

    /**
     * Puts the factory in a mode where no verification are made on variable
     * ( including arc respecting a variable order )
     * Helpful when diagram is build on stream
     */
    void putOnNoVariableCheckMode();

    /**
     * Puts the factory out of no check on variable mode
     */
    void putOffNoVariableCheckMode();

    protected:
    /**
     * Finds an order of variable compatible to the diagram
     */
    Sequence<const DiscreteVariable *> _findVariableOrder();

    /// Boolean precising if we are in check on variable mode or not
    bool _noVariableCheckMode;

    /// The order of variables in the diagram
    Sequence<const DiscreteVariable *> _varsSeq;

    /// The algebraic decision diagram model
    DiGraph _model;

    /// Mapping between id and variable
    NodeProperty<const DiscreteVariable *> _varMap;

    /// Mapping between terminal nodes and their values
    Bijection<NodeId, GUM_SCALAR> _valueMap;

    /// Mapping between variable's values and associated node
    NodeProperty<std::vector<NodeId> *> _arcMap;

    /// Mapping between variable's values and associated node
    NodeProperty<NodeId> _defaultArcMap;

    /// Mapping between variable and nodes tied to this var
    HashTable<const DiscreteVariable *, List<NodeId> *> _var2NodeIdMap;

    /// Mapping between variable and nodes tied to this var
    HashTable<const DiscreteVariable *, std::vector<Idx> *> _varUsedModalitiesMap;

    /// NodeId of decision diagram root node
    NodeId _rootId;
  };

} /* namespace gum */

#include <agrum/multidim/multiDimDecisionDiagramFactoryBase.tcc>

#endif /* GUM_MULTI_DIM_DECISION_DIAGRAM_FACTORY_BASE_H */

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
