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
 * @brief Headers of the IMDDI class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_IMDDI_H
#define GUM_IMDDI_H
// =========================================================================
// =========================================================================
#include <agrum/FMDP/learning/datastructure/incrementalGraphLearner.h>
#include <agrum/FMDP/learning/datastructure/leaves/leafAggregator.h>
#include <agrum/FMDP/learning/datastructure/variableselector.h>
// =========================================================================
// =========================================================================

namespace gum {

  /**
   * @class IMDDI
   * @headerfile imddi.h <agrum/FMDP/planning/FunctionGraph/imddi.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

  template < TESTNAME AttributeSelection, bool isScalar = false >
  class IMDDI: public IncrementalGraphLearner< AttributeSelection, isScalar > {
    public:
    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{

    // ==========================================================================
    /// Variable Learner constructor
    // ==========================================================================
    IMDDI(MultiDimFunctionGraph< double >* target,
          double                           attributeSelectionThreshold,
          double                           pairSelectionThreshold,
          Set< const DiscreteVariable* >   attributeListe,
          const DiscreteVariable*          learnedValue);

    // ==========================================================================
    /// Reward Learner constructor
    // ==========================================================================
    IMDDI(MultiDimFunctionGraph< double >* target,
          double                           attributeSelectionThreshold,
          double                           pairSelectionThreshold,
          Set< const DiscreteVariable* >   attributeListe);

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
    void addObservation(const Observation*);

    protected:
    void updateNodeWithObservation_(const Observation* newObs, NodeId currentNodeId);

    public:
    // ==========================================================================
    /// Updates the tree after a new observation has been added
    // ==========================================================================
    void updateGraph();

    protected:
    NodeId insertLeafNode_(NodeDatabase< AttributeSelection, isScalar >* nDB,
                           const DiscreteVariable*                       boundVar,
                           Set< const Observation* >*                    sonsMap);

    void chgNodeBoundVar_(NodeId chgedNodeId, const DiscreteVariable* desiredVar);

    void removeNode_(NodeId removedNodeId);

    private:
    void _addLeaf_(NodeId);
    void _removeLeaf_(NodeId);

    /// @}

    private:
    // ###################################################################
    /// @name Updating private methods
    // ###################################################################
    /// @{

    // ==========================================================================
    /// Computes the score of the given variables for the given node
    // ==========================================================================
    void _updateScore_(const DiscreteVariable*, NodeId, VariableSelector& vs);
    void _downdateScore_(const DiscreteVariable*, NodeId, VariableSelector& vs);

    // ==========================================================================
    /// For each node in the given set, this methods checks whether or not
    /// we should installed the given variable as a test.
    /// If so, the node is updated
    // ==========================================================================
    void _updateNodeSet_(Set< NodeId >&, const DiscreteVariable*, VariableSelector&);


    public:
    // ==========================================================================
    ///
    // ==========================================================================
    void updateFunctionGraph();

    private:
    void   _rebuildFunctionGraph_();
    NodeId _insertLeafInFunctionGraph_(AbstractLeaf*, Int2Type< true >);
    NodeId _insertLeafInFunctionGraph_(AbstractLeaf*, Int2Type< false >);

    /// @}
    ///
    public:
    void insertSetOfVars(MultiDimFunctionGraph< double >* ret) const {
      for (SequenceIteratorSafe< const DiscreteVariable* > varIter = _varOrder_.beginSafe();
           varIter != _varOrder_.endSafe();
           ++varIter)
        ret->add(**varIter);
    }

    private:
    Sequence< const DiscreteVariable* > _varOrder_;

    LeafAggregator _lg_;

    HashTable< NodeId, AbstractLeaf* > _leafMap_;

    /// The total number of observation added to this tree
    Idx _nbTotalObservation_;

    /// The threshold above which we consider variables to be dependant
    double _attributeSelectionThreshold_;

    /// The threshold above which two leaves does not share the same probability
    /// distribution
    // double  _pairSelectionThreshold_;
  };


} /* namespace gum */

#include <agrum/FMDP/learning/datastructure/imddi_tpl.h>

#endif   // GUM_IMDDI_H
