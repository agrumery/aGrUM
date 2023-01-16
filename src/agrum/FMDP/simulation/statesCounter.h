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
 * @brief Headers of the States Counter class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

// =========================================================================
#ifndef GUM_STATES_COUNTER_H
#define GUM_STATES_COUNTER_H
// =========================================================================
// =========================================================================
#include <agrum/FMDP/learning/datastructure/IVisitableGraphLearner.h>
// =========================================================================
// =========================================================================

namespace gum {


  /**
   * @class StatesCounter statesCounter.h
   * <agrum/FMDP/simulation/statesCounter.h>
   * @brief Provides the necessary to check whether or not states have been
   * already visited.
   * @ingroup fmdp_group
   */
  class StatesCounter: public IVisitableGraphLearner {
    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    /**
     * Default constructor
     */
    StatesCounter();

    /**
     * Default destructor
     */
    ~StatesCounter();

    /// @}

    // ==========================================================================
    /// @name Miscelleanous methods
    // ==========================================================================
    /// @{

    void incState(const Instantiation&);

    void reset(const Instantiation&);

    const MultiDimFunctionGraph< int >* counter();

    /// @}


    // ###################################################################
    /// @name Visit Methods
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    ///
    // ==========================================================================
    NodeId root() const { return _counter_->root(); }

    // ==========================================================================
    ///
    // ==========================================================================
    bool isTerminal(NodeId ni) const { return _counter_->isTerminalNode(ni); }

    // ==========================================================================
    ///
    // ==========================================================================
    const DiscreteVariable* nodeVar(NodeId ni) const { return _counter_->node(ni)->nodeVar(); }

    // ==========================================================================
    ///
    // ==========================================================================
    NodeId nodeSon(NodeId ni, Idx modality) const { return _counter_->node(ni)->son(modality); }

    // ==========================================================================
    ///
    // ==========================================================================
    Idx nodeNbObservation(NodeId ni) const { return _counter_->nodeValue(ni); }

    void insertSetOfVars(MultiDimFunctionGraph< double >* ret) const {
      for (SequenceIteratorSafe< const DiscreteVariable* > varIter
           = _counter_->variablesSequence().beginSafe();
           varIter != _counter_->variablesSequence().endSafe();
           ++varIter)
        ret->add(**varIter);
    }


    /// @}

    private:
    void _incState_(const Instantiation&, NodeId, Idx, Size);

    MultiDimFunctionGraph< Size >* _counter_;

    Set< Instantiation* > _visitedStates_;
  };
} /* namespace gum */

#endif   // GUM_STATES_COUNTER_H
