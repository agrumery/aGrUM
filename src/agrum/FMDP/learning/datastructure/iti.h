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
 * @brief Headers of the ITI class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_ITI_H
#define GUM_ITI_H
// =========================================================================
#include <agrum/core/multiPriorityQueue.h>
// =========================================================================
#include <agrum/multidim/multiDimFunctionGraph.h>
// =========================================================================
#include <agrum/FMDP/learning/core/templateStrategy.h>
#include <agrum/FMDP/learning/datastructure/incrementalGraphLearner.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class ITI iti.h <agrum/FMDP/planning/FunctionGraph/iti.h>
   * @brief
   * @ingroup fmdp_group
   *
   * Learn a graphical representation of a function as a decision tree.
   * This learning is done incrementaly.
   * Hence first observation are add to the structure and then
   * the structure is updated.
   * Maintains two graph function : one internal for the learning
   * and a target which is updated on demand.
   *
   */

  template <TESTNAME AttributeSelection, bool isScalar = false>
  class ITI : public IncrementalGraphLearner<AttributeSelection, isScalar> {

    public:
    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{

    // ==========================================================================
    /**
     * ITI constructor for functions describing the behaviour of one variable
     * according to a set of other variable such as conditionnal probabilities
     * @param target : the MultiDimFunctionGraph in which we load the structure
     * @param attributeSelectionThreshold : threshold under which a node is not
     * installed (pe-pruning)
     * @param attributeListe : Set of vars on which we rely to explain the
     * behaviour of learned variable
     * @param learnedValue : the variable from which we try to learn the
     * behaviour
     */
    // ==========================================================================
    ITI( MultiDimFunctionGraph<double>* target,
         double                         attributeSelectionThreshold,
         Set<const DiscreteVariable*>   attributeListe,
         const DiscreteVariable*        learnedValue );

    // ==========================================================================
    /**
     * ITI constructeur for real functions. We try to predict the output of a
     * function f given a set of variable
     * @param target : the MultiDimFunctionGraph in which we load the structure
     * @param attributeSelectionThreshold : threshold under which a node is not
     * installed (pe-pruning)
     * @param attributeListe : Set of vars on which we rely to explain the
     * behaviour of learned function
     */
    // ==========================================================================
    ITI( MultiDimFunctionGraph<double>* target,
         double                         attributeSelectionThreshold,
         Set<const DiscreteVariable*>   attributeListe );

    // ==========================================================================
    /// Default destructor
    // ==========================================================================
    ~ITI() { GUM_DESTRUCTOR( ITI ); }

    /// @}

    // ###################################################################
    /// @name New Observation insertion methods
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /**
     * Inserts a new observation
     * @param obs the new observation to learn
     */
    // ==========================================================================
    void addObservation( const Observation* obs );

    protected:
    // ==========================================================================
    /**
     * Will update internal graph's NodeDatabase of given node with the new
     * observation
     * @param newObs
     * @param currentNodeId
     */
    // ==========================================================================
    void _updateNodeWithObservation( const Observation* newObs,
                                     NodeId             currentNodeId );

    /// @}

    // ###################################################################
    /// @name Graph Structure update methods
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Updates the internal graph after a new observation has been added
    // ==========================================================================
    void updateGraph();

    protected:
    // ==========================================================================
    /**
     * inserts a new node in internal graph
     * @param nDB : the associated database
     * @param boundVar : the associated variable
     * @return the newly created node's id
     */
    // ==========================================================================
    NodeId _insertNode( NodeDatabase<AttributeSelection, isScalar>* nDB,
                        const DiscreteVariable* boundVar );

    // ==========================================================================
    /**
     * Changes the associated variable of a node
     * @param chgedNodeId : the node to change
     * @param desiredVar : its new associated variable
     */
    // ==========================================================================
    void _chgNodeBoundVar( NodeId                  chgedNodeId,
                           const DiscreteVariable* desiredVar );

    // ==========================================================================
    /**
     * Removes a node from the internal graph
     * @param removedNodeId : the node to remove
     */
    // ==========================================================================
    void _removeNode( NodeId removedNodeId );

    /// @}

    public:
    // ###################################################################
    /// @name Function Graph Updating methods
    // ###################################################################
    /// @{

    // ==========================================================================
    /// Updates target to currently learned graph structure
    // ==========================================================================
    void updateFunctionGraph();

    private:
    // ==========================================================================
    /**
     * Inserts an internal node in the target
     * @param src the source node in internal graph
     * @return the mathcing node id in the target
     */
    // ==========================================================================
    NodeId __insertNodeInFunctionGraph( NodeId src );

    // ==========================================================================
    /**
     * Insert a terminal node in the target.
     * This function is a dispatcher that will call the right function according
     * to
     * the value of the template isScalar
     * @param src the source node in the learned graph
     * @return the matching node in the target
     */
    // ==========================================================================
    NodeId __insertTerminalNode( NodeId src ) {
      return __insertTerminalNode( src, Int2Type<isScalar>() );
    }

    // ==========================================================================
    /**
     * Insert a terminal node in the target.
     * This function is called if we're learning a real value function.
     * Inserts then a single value in target.
     * @param src the source node in the learned graph
     * @return the matching node in the target
     */
    // ==========================================================================
    NodeId __insertTerminalNode( NodeId src, Int2Type<true> );

    // ==========================================================================
    /**
     * Insert a terminal node in the target.
     * This function is called if we're learning the behaviour of a variable.
     * Inserts then this variable and the relevant value beneath into target.
     * @param src the source node in the learned graph
     * @return the matching node in the target
     */
    // ==========================================================================
    NodeId __insertTerminalNode( NodeId src, Int2Type<false> );

    /// @}


    protected:
    // ==========================================================================
    /**
     * @brief _insertSetOfVars
     * @param ret
     */
    // ==========================================================================
    void _insertSetOfVars( MultiDimFunctionGraph<double>* ret ) {
      for ( SetIteratorSafe<const DiscreteVariable*> varIter =
                this->_setOfVars.beginSafe();
            varIter != this->_setOfVars.endSafe();
            ++varIter )
        ret->add( **varIter );
    }

    private:
    /// Hashtable indicating if given node has been modified (upon receiving new
    /// exemple or through a transpose)
    /// The aim is not if we have revise the installed variable on that node
    HashTable<NodeId, bool> __staleTable;

    /// The total number of observation added to this tree
    Idx __nbTotalObservation;

    /// The threshold above which we consider variables to be dependant
    double __attributeSelectionThreshold;
  };


} /* namespace gum */

#include <agrum/FMDP/learning/datastructure/iti_tpl.h>

#endif  // GUM_ITI_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
