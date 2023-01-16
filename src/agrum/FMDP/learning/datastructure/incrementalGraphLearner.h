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
// =========================================================================
#include <agrum/FMDP/learning/datastructure/IVisitableGraphLearner.h>
#include <agrum/FMDP/learning/datastructure/nodeDatabase.h>
// =========================================================================
// =========================================================================

namespace gum {

  /**
   * @class IncrementalGraphLearner incrementalGraphLearner.h
   * <agrum/FMDP/learning/datastructure/incrementalGraphLearner>
   * @brief
   * @ingroup fmdp_group
   *
   * Abstract class for incrementally learn a graphical representation of a
   * function.
   * Can handle both function of real values, and function explaining the
   * behaviour
   * of a variable given set of other variables (as typically in conditional
   * probabilities)
   *
   * Maintains two graph in memory, one which is incrementally updated and the
   * other one
   * which is updated on demand and is usable by the outside.
   *
   */
  template < TESTNAME AttributeSelection, bool isScalar = false >
  class IncrementalGraphLearner: public IVisitableGraphLearner {
    using ValueType = typename ValueSelect< isScalar, double, Idx >::type;

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
    IncrementalGraphLearner(MultiDimFunctionGraph< double >* target,
                            Set< const DiscreteVariable* >   attributesSet,
                            const DiscreteVariable*          learnVariable);

    // ==========================================================================
    /// Default destructor
    // ==========================================================================
    virtual ~IncrementalGraphLearner();

    private:
    // ==========================================================================
    /// Template function dispatcher
    // ==========================================================================
    void _clearValue_() { _clearValue_(Int2Type< isScalar >()); }

    // ==========================================================================
    /// In the case where we're learning a function of real values
    /// this has to be wiped out upon destruction (to be deprecated)
    // ==========================================================================
    void _clearValue_(Int2Type< true >) { delete value_; }

    // ==========================================================================
    /// In case where we're learning function of variable behaviour,
    /// this should do nothing
    // ==========================================================================
    void _clearValue_(Int2Type< false >) {}

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
    virtual void addObservation(const Observation* obs);

    private:
    // ==========================================================================
    /**
     * Get value assumed by studied variable for current observation
     */
    // ==========================================================================
    void _assumeValue_(const Observation* obs) { _assumeValue_(obs, Int2Type< isScalar >()); }
    void _assumeValue_(const Observation* obs, Int2Type< true >) {
      if (!valueAssumed_.exists(obs->reward())) valueAssumed_ << obs->reward();
    }
    void _assumeValue_(const Observation* obs, Int2Type< false >) {
      if (!valueAssumed_.exists(obs->modality(value_))) valueAssumed_ << obs->modality(value_);
    }


    // ==========================================================================
    /**
     * Seek modality assumed in obs for given var
     */
    // ==========================================================================
    Idx _branchObs_(const Observation* obs, const DiscreteVariable* var) {
      return _branchObs_(obs, var, Int2Type< isScalar >());
    }
    Idx _branchObs_(const Observation* obs, const DiscreteVariable* var, Int2Type< true >) {
      return obs->rModality(var);
    }
    Idx _branchObs_(const Observation* obs, const DiscreteVariable* var, Int2Type< false >) {
      return obs->modality(var);
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
    virtual void updateNodeWithObservation_(const Observation* newObs, NodeId currentNodeId) {
      nodeId2Database_[currentNodeId]->addObservation(newObs);
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
    virtual void updateVar(const DiscreteVariable*);

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
    void updateNode_(NodeId nody, Set< const DiscreteVariable* >& bestVars);

    // ==========================================================================
    /// Turns the given node into a leaf if not already so
    // ==========================================================================
    virtual void convertNode2Leaf_(NodeId);

    // ==========================================================================
    /// Installs given variable to the given node, ensuring that the variable
    /// is not present in its subtree
    // ==========================================================================
    virtual void transpose_(NodeId, const DiscreteVariable*);

    // ==========================================================================
    /**
     * inserts a new node in internal graph
     * @param nDB : the associated database
     * @param boundVar : the associated variable
     * @return the newly created node's id
     */
    // ==========================================================================
    virtual NodeId insertNode_(NodeDatabase< AttributeSelection, isScalar >* nDB,
                               const DiscreteVariable*                       boundVar);

    // ==========================================================================
    /**
     * inserts a new internal node in internal graph
     * @param nDB : the associated database
     * @param boundVar : the associated variable
     * @param sonsMap : a table giving node's sons node
     * @return the newly created node's id
     */
    // ==========================================================================
    virtual NodeId insertInternalNode_(NodeDatabase< AttributeSelection, isScalar >* nDB,
                                       const DiscreteVariable*                       boundVar,
                                       NodeId*                                       sonsMap);

    // ==========================================================================
    /**
     * inserts a new leaf node in internal graohs
     * @param nDB : the associated database
     * @param boundVar : the associated variable
     * @param obsSet : the set of observation this leaf retains
     * @return the newly created node's id
     */
    // ==========================================================================
    virtual NodeId insertLeafNode_(NodeDatabase< AttributeSelection, isScalar >* nDB,
                                   const DiscreteVariable*                       boundVar,
                                   Set< const Observation* >*                    obsSet);

    // ==========================================================================
    /**
     * Changes the associated variable of a node
     * @param chgedNodeId : the node to change
     * @param desiredVar : its new associated variable
     */
    // ==========================================================================
    virtual void chgNodeBoundVar_(NodeId chgedNodeId, const DiscreteVariable* desiredVar);

    // ==========================================================================
    /**
     * Removes a node from the internal graph
     * @param removedNodeId : the node to remove
     */
    // ==========================================================================
    virtual void removeNode_(NodeId removedNodeId);

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
    Size size() { return nodeVarMap_.size(); }


    // ###################################################################
    /// @name Visit Methods
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    ///
    // ==========================================================================
    NodeId root() const { return this->root_; }

    // ==========================================================================
    ///
    // ==========================================================================
    bool isTerminal(NodeId ni) const { return !this->nodeSonsMap_.exists(ni); }

    // ==========================================================================
    ///
    // ==========================================================================
    const DiscreteVariable* nodeVar(NodeId ni) const { return this->nodeVarMap_[ni]; }

    // ==========================================================================
    ///
    // ==========================================================================
    NodeId nodeSon(NodeId ni, Idx modality) const { return this->nodeSonsMap_[ni][modality]; }

    // ==========================================================================
    ///
    // ==========================================================================
    Idx nodeNbObservation(NodeId ni) const { return this->nodeId2Database_[ni]->nbObservation(); }

    // ==========================================================================
    ///
    // ==========================================================================
    virtual void insertSetOfVars(MultiDimFunctionGraph< double >* ret) const {
      for (SetIteratorSafe< const DiscreteVariable* > varIter = setOfVars_.beginSafe();
           varIter != setOfVars_.endSafe();
           ++varIter)
        ret->add(**varIter);
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
    NodeGraphPart model_;

    // ==========================================================================
    /// The root of the ordered tree
    // ==========================================================================
    NodeId root_;

    // ==========================================================================
    /// Gives for any node its associated variable
    // ==========================================================================
    HashTable< NodeId, const DiscreteVariable* > nodeVarMap_;

    // ==========================================================================
    /// A table giving for any node a table mapping to its son
    /// idx is the modality of associated variable
    // ==========================================================================
    HashTable< NodeId, NodeId* > nodeSonsMap_;

    // ==========================================================================
    /// Associates to any variable the list of all nodes associated to
    /// this variable
    // ==========================================================================
    HashTable< const DiscreteVariable*, LinkedList< NodeId >* > var2Node_;

    // ==========================================================================
    /// This hashtable binds every node to an associated NodeDatabase
    /// which handles every observation that concerns that node
    // ==========================================================================
    HashTable< NodeId, NodeDatabase< AttributeSelection, isScalar >* > nodeId2Database_;

    // ==========================================================================
    /// This hashtable binds to every leaf an associated set of all
    /// hte observations compatible with it
    // ==========================================================================
    HashTable< NodeId, Set< const Observation* >* > leafDatabase_;

    /// @}


    /// The final diagram we're building
    MultiDimFunctionGraph< double >* target_;

    Set< const DiscreteVariable* > setOfVars_;

    const DiscreteVariable* value_;
    Sequence< ValueType >   valueAssumed_;

    bool needUpdate_;
  };


} /* namespace gum */

#include <agrum/FMDP/learning/datastructure/incrementalGraphLearner_tpl.h>

#endif   // GUM_INCREMENTAL_GRAPH_LEARNER_H
