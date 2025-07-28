/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Headers of the RMax planer class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

// =========================================================================
#ifndef GUM_ADAPTIVE_RMAX_PLANER_H
#define GUM_ADAPTIVE_RMAX_PLANER_H
// =========================================================================
#include <agrum/FMDP/learning/fmdpLearner.h>
#include <agrum/FMDP/planning/structuredPlaner.h>
#include <agrum/FMDP/SDyna/Strategies/IDecisionStrategy.h>
#include <agrum/FMDP/simulation/statesCounter.h>

// =========================================================================

namespace gum {

  /**
   * @class AdaptiveRMaxPlaner adaptiveRMaxPlaner.h
   * <agrum/FMDP/planning/adaptiveRMaxPlaner.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   *
   * Perform a RMax planning on given in parameter factored markov decision
   * process
   *
   */
  class AdaptiveRMaxPlaner: public StructuredPlaner< double >, public IDecisionStrategy {
    // ###################################################################
    /// @name
    // ###################################################################
    /// @{

    public:
    // ==========================================================================
    ///
    // ==========================================================================
    static AdaptiveRMaxPlaner* ReducedAndOrderedInstance(const ILearningStrategy* learner,
                                                         double discountFactor = 0.9,
                                                         double epsilon        = 0.00001,
                                                         bool   verbose        = true) {
      return new AdaptiveRMaxPlaner(new MDDOperatorStrategy< double >(),
                                    discountFactor,
                                    epsilon,
                                    learner,
                                    verbose);
    }

    // ==========================================================================
    ///
    // ==========================================================================
    static AdaptiveRMaxPlaner* TreeInstance(const ILearningStrategy* learner,
                                            double                   discountFactor = 0.9,
                                            double                   epsilon        = 0.00001,
                                            bool                     verbose        = true) {
      return new AdaptiveRMaxPlaner(new TreeOperatorStrategy< double >(),
                                    discountFactor,
                                    epsilon,
                                    learner,
                                    verbose);
    }

    /// @}

    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{

    private:
    // ==========================================================================
    /// Default constructor
    // ==========================================================================
    AdaptiveRMaxPlaner(IOperatorStrategy< double >* opi,
                       double                       discountFactor,
                       double                       epsilon,
                       const ILearningStrategy*     learner,
                       bool                         verbose);

    // ==========================================================================
    /// Default destructor
    // ==========================================================================

    public:
    ~AdaptiveRMaxPlaner();

    /// @}


    // ###################################################################
    /// @name Planning Methods
    // ###################################################################
    /// @{

    public:
    // ==========================================================================
    /**
     * Initializes data structure needed for making the planning
     * @warning No calling this methods before starting the first makePlaninng
     * will surely and definitely result in a crash
     */
    // ==========================================================================
    void initialize(const FMDP< double >* fmdp);


    // ==========================================================================
    /**
     * Performs a value iteration
     *
     * @param nbStep : enables you to specify how many value iterations you wish
     * to do.
     * makePlanning will then stop whether when optimal value function is reach
     * or when nbStep have been performed
     */
    // ==========================================================================
    void makePlanning(Idx nbStep = 1000000);

    /// @}


    // ###################################################################
    /// @name Value Iteration Methods
    // ###################################################################
    /// @{

    protected:
    // ==========================================================================
    ///
    // ==========================================================================
    virtual void initVFunction_();

    // ==========================================================================
    /// Performs a single step of value iteration
    // ==========================================================================
    virtual MultiDimFunctionGraph< double >* valueIteration_();

    /// @}


    // ###################################################################
    /// @name Optimal policy extraction methods
    // ###################################################################
    /// @{

    protected:
    // ==========================================================================
    /// Perform the required tasks to extract an optimal policy
    // ==========================================================================
    virtual void evalPolicy_();

    /// @}

    private:
    void _makeRMaxFunctionGraphs_();

    std::pair< NodeId, NodeId > _visitLearner_(const IVisitableGraphLearner*,
                                               NodeId currentNodeId,
                                               MultiDimFunctionGraph< double >*,
                                               MultiDimFunctionGraph< double >*);
    void                        _clearTables_();

    private:
    HashTable< Idx, MultiDimFunctionGraph< double >* > _actionsRMaxTable_;
    HashTable< Idx, MultiDimFunctionGraph< double >* > _actionsBoolTable_;
    const ILearningStrategy*                           _fmdpLearner_;

    double _rThreshold_;
    double _rmax_;


    // ###################################################################
    /// @name Incremental methods
    // ###################################################################
    /// @{

    public:
    void checkState(const Instantiation& newState, Idx actionId) {
      if (!_initializedTable_[actionId]) {
        _counterTable_[actionId]->reset(newState);
        _initializedTable_[actionId] = true;
      } else _counterTable_[actionId]->incState(newState);
    }

    private:
    HashTable< Idx, StatesCounter* > _counterTable_;
    HashTable< Idx, bool >           _initializedTable_;

    bool _initialized_;
    /// @}
  };

} /* namespace gum */

#endif   // GUM_ADAPTIVE_RMAX_PLANER_H
