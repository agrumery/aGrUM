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
 * @brief Headers of the IMDDI class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_IMDDI_H
#define GUM_IMDDI_H
// =========================================================================
#include <agrum/multidim/multiDimFunctionGraph.h>
// =========================================================================
#include <agrum/FMDP/learning/datastructure/incrementalGraphLearner.h>
#include <agrum/FMDP/learning/datastructure/leaves/abstractLeaf.h>
#include <agrum/FMDP/learning/datastructure/leaves/concreteLeaf.h>
#include <agrum/FMDP/learning/datastructure/leaves/leafAggregator.h>
#include <agrum/FMDP/learning/datastructure/variableselector.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class IMDDI imddi.h <agrum/FMDP/planning/FunctionGraph/imddi.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

  template <TESTNAME AttributeSelection, bool isScalar = false>
  class IMDDI : public IncrementalGraphLearner<AttributeSelection, isScalar> {

    public:
    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{

    // ==========================================================================
    /// Variable Learner constructor
    // ==========================================================================
    IMDDI( MultiDimFunctionGraph<double>* target,
           double                         attributeSelectionThreshold,
           double                         pairSelectionThreshold,
           Set<const DiscreteVariable*>   attributeListe,
           const DiscreteVariable*        learnedValue );

    // ==========================================================================
    /// Reward Learner constructor
    // ==========================================================================
    IMDDI( MultiDimFunctionGraph<double>* target,
           double                         attributeSelectionThreshold,
           double                         pairSelectionThreshold,
           Set<const DiscreteVariable*>   attributeListe );

    // ==========================================================================
    /// Default destructor
    // ==========================================================================
    ~IMDDI();

    /// @}

    // ###################################################################
    /// @name Incrementals methods
    // ###################################################################
    /// @{

    // ==========================================================================
    /// Adds a new observation to the structure
    // ==========================================================================
    void addObservation( const Observation* );

    protected:
    void _updateNodeWithObservation( const Observation* newObs,
                                     NodeId             currentNodeId );

    public:
    // ==========================================================================
    /// Updates the tree after a new observation has been added
    // ==========================================================================
    void updateGraph();

    protected:
    NodeId _insertLeafNode( NodeDatabase<AttributeSelection, isScalar>* nDB,
                            const DiscreteVariable*  boundVar,
                            Set<const Observation*>* sonsMap );

    void _chgNodeBoundVar( NodeId                  chgedNodeId,
                           const DiscreteVariable* desiredVar );

    void _removeNode( NodeId removedNodeId );

    private:
    void __addLeaf( NodeId );
    void __removeLeaf( NodeId );

    /// @}

    private:
    // ###################################################################
    /// @name Updating private methods
    // ###################################################################
    /// @{

    // ==========================================================================
    /// Computes the score of the given variables for the given node
    // ==========================================================================
    void __updateScore( const DiscreteVariable*, NodeId, VariableSelector& vs );
    void __downdateScore( const DiscreteVariable*, NodeId, VariableSelector& vs );

    // ==========================================================================
    /// For each node in the given set, this methods checks whether or not
    /// we should installed the given variable as a test.
    /// If so, the node is updated
    // ==========================================================================
    void
    __updateNodeSet( Set<NodeId>&, const DiscreteVariable*, VariableSelector& );


    public:
    // ==========================================================================
    ///
    // ==========================================================================
    void updateFunctionGraph();

    private:
    void   __rebuildFunctionGraph();
    NodeId __insertLeafInFunctionGraph( AbstractLeaf*, Int2Type<true> );
    NodeId __insertLeafInFunctionGraph( AbstractLeaf*, Int2Type<false> );

    /// @}
    ///
    public:
    void insertSetOfVars( MultiDimFunctionGraph<double>* ret ) const {
      for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
                __varOrder.beginSafe();
            varIter != __varOrder.endSafe();
            ++varIter )
        ret->add( **varIter );
    }

    private:
    Sequence<const DiscreteVariable*> __varOrder;

    LeafAggregator __lg;

    HashTable<NodeId, AbstractLeaf*> __leafMap;

    /// The total number of observation added to this tree
    Idx __nbTotalObservation;

    /// The threshold above which we consider variables to be dependant
    double __attributeSelectionThreshold;

    /// The threshold above which two leaves does not share the same probability
    /// distribution
    // double __pairSelectionThreshold;
  };


} /* namespace gum */

#include <agrum/FMDP/learning/datastructure/imddi_tpl.h>

#endif  // GUM_IMDDI_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
