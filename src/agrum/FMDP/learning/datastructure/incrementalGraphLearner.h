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
 * @brief Headers of the interface specifying functions to be implemented by any
 * incremental learner.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_INCREMENTAL_GRAPH_LEARNER_H
#define GUM_INCREMENTAL_GRAPH_LEARNER_H
// =========================================================================
// =========================================================================
#include <agrum/multidim/multiDimFunctionGraph.h>
// =========================================================================
#include <agrum/FMDP/learning/core/templateStrategy.h>
#include <agrum/FMDP/learning/datastructure/IVisitableGraphLearner.h>
#include <agrum/FMDP/learning/datastructure/nodeDatabase.h>
// =========================================================================
#include <agrum/multidim/FunctionGraphUtilities/link.h>
// =========================================================================

namespace gum {

  /**
   * @class IncrementalGraphLearner incrementalGraphLearner.h
   * <agrum/FMDP/learning/datastructure/incrementalGraphLearner>
   * @brief
   * @ingroup fmdp_group
   *
   * Abstract class for incrementaly learn a graphical representation of a
   * function.
   * Can handle both function of real values, and function explaining the
   * behaviour
   * of a variable given set of other variables (as typically in conditionnal
   * probabilities)
   *
   * Maintains two graph in memory, one which is incrementaly updated and the
   * other one
   * which is updated on demand and is usable by the outside.
   *
   */
  template <TESTNAME AttributeSelection, bool isScalar = false>
  class IncrementalGraphLearner : public IVisitableGraphLearner {

    typedef typename ValueSelect<isScalar, double, Idx>::type ValueType;

    public:
    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{

    // ==========================================================================
    /**
     * Default constructor
     * @param target : the output diagram usable by the outside
     * @param attributesSet : set of variables from which we try to describe the
     * learned function
     * @param learnVariable : if we tried to learn a the behaviour of a variable
     * given variable given another set of variables, this is the one. If we are
     * learning a function of real value, this is just a computationnal trick
     * (and is to be deprecated)
     */
    // ==========================================================================
    IncrementalGraphLearner( MultiDimFunctionGraph<double>* target,
                             Set<const DiscreteVariable*>   attributesSet,
                             const DiscreteVariable*        learnVariable );

    // ==========================================================================
    /// Default destructor
    // ==========================================================================
    virtual ~IncrementalGraphLearner();

    private:
    // ==========================================================================
    /// Template function dispatcher
    // ==========================================================================
    void __clearValue() { __clearValue( Int2Type<isScalar>() ); }

    // ==========================================================================
    /// In the case where we're learning a function of real values
    /// this has to be wiped out upon destruction (to be deprecated)
    // ==========================================================================
    void __clearValue( Int2Type<true> ) { delete _value; }

    // ==========================================================================
    /// In case where we're learning function of variable behaviour,
    /// this should do nothing
    // ==========================================================================
    void __clearValue( Int2Type<false> ) {}

    /// @}


    // ###################################################################
    /// @name New Observation insertion methods
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /**
     * Inserts a new observation
     */
    // ==========================================================================
    virtual void addObservation( const Observation* obs );

    private:
    // ==========================================================================
    /**
     * Get value assumed by studied variable for current observation
     */
    // ==========================================================================
    void __assumeValue( const Observation* obs ) {
      __assumeValue( obs, Int2Type<isScalar>() );
    }
    void __assumeValue( const Observation* obs, Int2Type<true> ) {
      if ( !_valueAssumed.exists( obs->reward() ) ) _valueAssumed << obs->reward();
    }
    void __assumeValue( const Observation* obs, Int2Type<false> ) {
      if ( !_valueAssumed.exists( obs->modality( _value ) ) )
        _valueAssumed << obs->modality( _value );
    }


    // ==========================================================================
    /**
     * Seek modality assumed in obs for given var
     */
    // ==========================================================================
    Idx __branchObs( const Observation* obs, const DiscreteVariable* var ) {
      return __branchObs( obs, var, Int2Type<isScalar>() );
    }
    Idx __branchObs( const Observation*      obs,
                     const DiscreteVariable* var,
                     Int2Type<true> ) {
      return obs->rModality( var );
    }
    Idx __branchObs( const Observation*      obs,
                     const DiscreteVariable* var,
                     Int2Type<false> ) {
      return obs->modality( var );
    }

    protected:
    // ==========================================================================
    /**
     * Will update internal graph's NodeDatabase of given node with the new
     * observation
     * @param newObs
     * @param currentNodeId
     */
    // ==========================================================================
    virtual void _updateNodeWithObservation( const Observation* newObs,
                                             NodeId             currentNodeId ) {
      _nodeId2Database[currentNodeId]->addObservation( newObs );
    }

    /// @}

    // ###################################################################
    /// @name Graph Structure update methods
    // ###################################################################
    /// @{

    public:
    // ==========================================================================
    /// If a new modality appears to exists for given variable,
    /// call this method to turn every associated node to this variable into
    /// leaf.
    /// Graph has then indeed to be revised
    // ==========================================================================
    virtual void updateVar( const DiscreteVariable* );

    // ==========================================================================
    /// Updates the tree after a new observation has been added
    // ==========================================================================
    virtual void updateGraph() = 0;

    protected:
    // ==========================================================================
    /**
     * From the given sets of node, selects randomly one and installs it
     * on given node. Chechks of course if node's current variable is not in
     * that
     * set first.
     * @param nody : the node we update
     * @param bestVars : the set of interessting vars to be installed here
     */
    // ==========================================================================
    void _updateNode( NodeId nody, Set<const DiscreteVariable*>& bestVars );

    // ==========================================================================
    /// Turns the given node into a leaf if not already so
    // ==========================================================================
    virtual void _convertNode2Leaf( NodeId );

    // ==========================================================================
    /// Installs given variable to the given node, ensuring that the variable
    /// is not present in its subtree
    // ==========================================================================
    virtual void _transpose( NodeId, const DiscreteVariable* );

    // ==========================================================================
    /**
     * inserts a new node in internal graph
     * @param nDB : the associated database
     * @param boundVar : the associated variable
     * @return the newly created node's id
     */
    // ==========================================================================
    virtual NodeId _insertNode( NodeDatabase<AttributeSelection, isScalar>* nDB,
                                const DiscreteVariable* boundVar );

    // ==========================================================================
    /**
     * inserts a new internal node in internal graph
     * @param nDB : the associated database
     * @param boundVar : the associated variable
     * @param sonsMap : a table giving node's sons node
     * @return the newly created node's id
     */
    // ==========================================================================
    virtual NodeId
    _insertInternalNode( NodeDatabase<AttributeSelection, isScalar>* nDB,
                         const DiscreteVariable* boundVar,
                         NodeId*                 sonsMap );

    // ==========================================================================
    /**
     * inserts a new leaf node in internal graohs
     * @param nDB : the associated database
     * @param boundVar : the associated variable
     * @param obsSet : the set of observation this leaf retains
     * @return the newly created node's id
     */
    // ==========================================================================
    virtual NodeId
    _insertLeafNode( NodeDatabase<AttributeSelection, isScalar>* nDB,
                     const DiscreteVariable*  boundVar,
                     Set<const Observation*>* obsSet );

    // ==========================================================================
    /**
     * Changes the associated variable of a node
     * @param chgedNodeId : the node to change
     * @param desiredVar : its new associated variable
     */
    // ==========================================================================
    virtual void _chgNodeBoundVar( NodeId                  chgedNodeId,
                                   const DiscreteVariable* desiredVar );

    // ==========================================================================
    /**
     * Removes a node from the internal graph
     * @param removedNodeId : the node to remove
     */
    // ==========================================================================
    virtual void _removeNode( NodeId removedNodeId );

    /// @}


    // ###################################################################
    /// @name Function Graph Updating methods
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Updates target to currently learned graph structure
    // ==========================================================================
    virtual void updateFunctionGraph() = 0;

    /// @}


    public:
    // ==========================================================================
    ///
    // ==========================================================================
    Size size() { return _nodeVarMap.size(); }


    // ###################################################################
    /// @name Visit Methods
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    ///
    // ==========================================================================
    NodeId root() const { return this->_root; }

    // ==========================================================================
    ///
    // ==========================================================================
    bool isTerminal( NodeId ni ) const { return !this->_nodeSonsMap.exists( ni ); }

    // ==========================================================================
    ///
    // ==========================================================================
    const DiscreteVariable* nodeVar( NodeId ni ) const {
      return this->_nodeVarMap[ni];
    }

    // ==========================================================================
    ///
    // ==========================================================================
    NodeId nodeSon( NodeId ni, Idx modality ) const {
      return this->_nodeSonsMap[ni][modality];
    }

    // ==========================================================================
    ///
    // ==========================================================================
    Idx nodeNbObservation( NodeId ni ) const {
      return this->_nodeId2Database[ni]->nbObservation();
    }

    // ==========================================================================
    ///
    // ==========================================================================
    virtual void insertSetOfVars( MultiDimFunctionGraph<double>* ret ) const {
      for ( SetIteratorSafe<const DiscreteVariable*> varIter =
                _setOfVars.beginSafe();
            varIter != _setOfVars.endSafe();
            ++varIter )
        ret->add( **varIter );
    }
    /// @}

    protected:
    /// @}

    // ###################################################################
    /// @name Model handling datastructures
    // ###################################################################
    /// @{

    // ==========================================================================
    /// The source of nodeId
    // ==========================================================================
    NodeGraphPart _model;

    // ==========================================================================
    /// The root of the ordered tree
    // ==========================================================================
    NodeId _root;

    // ==========================================================================
    /// Gives for any node its associated variable
    // ==========================================================================
    HashTable<NodeId, const DiscreteVariable*> _nodeVarMap;

    // ==========================================================================
    /// A table giving for any node a table mapping to its son
    /// idx is the modality of associated variable
    // ==========================================================================
    HashTable<NodeId, NodeId*> _nodeSonsMap;

    // ==========================================================================
    /// Associates to any variable the list of all nodes associated to
    /// this variable
    // ==========================================================================
    HashTable<const DiscreteVariable*, LinkedList<NodeId>*> _var2Node;

    // ==========================================================================
    /// This hashtable binds every node to an associated NodeDatabase
    /// which handles every observation that concerns that node
    // ==========================================================================
    HashTable<NodeId, NodeDatabase<AttributeSelection, isScalar>*>
        _nodeId2Database;

    // ==========================================================================
    /// This hashtable binds to every leaf an associated set of all
    /// hte observations compatible with it
    // ==========================================================================
    HashTable<NodeId, Set<const Observation*>*> _leafDatabase;

    /// @}


    /// The final diagram we're building
    MultiDimFunctionGraph<double>* _target;

    Set<const DiscreteVariable*> _setOfVars;

    const DiscreteVariable* _value;
    Sequence<ValueType>     _valueAssumed;

    bool _needUpdate;
  };


} /* namespace gum */

#include <agrum/FMDP/learning/datastructure/incrementalGraphLearner_tpl.h>

#endif  // GUM_INCREMENTAL_GRAPH_LEARNER_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
