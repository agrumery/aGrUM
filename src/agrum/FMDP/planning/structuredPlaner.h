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
 * @brief Headers of the StructuredPlaner planer class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_STRUCTURED_PLANNING_H
#define GUM_STRUCTURED_PLANNING_H
// =========================================================================
#include <thread>
// =========================================================================
#include <agrum/core/argMaxSet.h>
#include <agrum/core/functors.h>
#include <agrum/core/inline.h>
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
// =========================================================================
#include <agrum/multidim/FunctionGraphUtilities/terminalNodePolicies/SetTerminalNodePolicy.h>
#include <agrum/multidim/multiDimFunctionGraph.h>
// =========================================================================
#include <agrum/FMDP/SDyna/Strategies/IPlanningStrategy.h>
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/planning/IOperatorStrategy.h>
#include <agrum/FMDP/planning/actionSet.h>
#include <agrum/FMDP/planning/mddOperatorStrategy.h>
#include <agrum/FMDP/planning/treeOperatorStrategy.h>
// =========================================================================

namespace gum {

  /**
   * @class StructuredPlaner structuredPlaner.h
   * <agrum/FMDP/planning/structuredPlaner.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   *
   * Perform a structure value iteration planning
   *
   * Pure virtual functions :
   *  _regress, _maximize, _argmaximize, _add and _subtract
   *  are a priori the ones to be respecified according to the used
   * datastructure
   *  (MDDs, DTs, BNs, ...)
   *
   */
  template <typename GUM_SCALAR>
  class StructuredPlaner : public IPlanningStrategy<GUM_SCALAR> {


    // ###################################################################
    /// @name
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    ///
    // ==========================================================================
    static StructuredPlaner<GUM_SCALAR>*
    spumddInstance( GUM_SCALAR discountFactor = 0.9,
                    GUM_SCALAR epsilon = 0.00001,
                    bool       verbose = true ) {
      return new StructuredPlaner<GUM_SCALAR>(
          new MDDOperatorStrategy<GUM_SCALAR>(),
          discountFactor,
          epsilon,
          verbose );
    }

    // ==========================================================================
    ///
    // ==========================================================================
    static StructuredPlaner<GUM_SCALAR>*
    sviInstance( GUM_SCALAR discountFactor = 0.9,
                 GUM_SCALAR epsilon = 0.00001,
                 bool       verbose = true ) {
      return new StructuredPlaner<GUM_SCALAR>(
          new TreeOperatorStrategy<GUM_SCALAR>(),
          discountFactor,
          epsilon,
          verbose );
    }

    /// @}

    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{
    protected:
    // ==========================================================================
    /// Default constructor
    // ==========================================================================
    StructuredPlaner( IOperatorStrategy<GUM_SCALAR>* opi,
                      GUM_SCALAR                     discountFactor,
                      GUM_SCALAR                     epsilon,
                      bool                           verbose );

    // ==========================================================================
    /// Default destructor
    // ==========================================================================
    public:
    virtual ~StructuredPlaner();

    /// @}

    // ###################################################################
    /// @name Datastructure access methods
    // ###################################################################
    /// @{

    public:
    // ==========================================================================
    /// Returns a const ptr on the Factored Markov Decision Process on which
    /// we're planning
    // ==========================================================================
    INLINE const FMDP<GUM_SCALAR>* fmdp() { return _fmdp; }

    // ==========================================================================
    /// Returns a const ptr on the value function computed so far
    // ==========================================================================
    INLINE const MultiDimFunctionGraph<GUM_SCALAR>* vFunction() {
      return _vFunction;
    }

    // ==========================================================================
    /// Returns vFunction computed so far current size
    // ==========================================================================
    virtual Size vFunctionSize() {
      return _vFunction != nullptr ? _vFunction->realSize() : 0;
    }

    // ==========================================================================
    /// Returns the best policy obtained so far
    // ==========================================================================
    INLINE const MultiDimFunctionGraph<ActionSet, SetTerminalNodePolicy>*
                 optimalPolicy() {
      return _optimalPolicy;
    }

    // ==========================================================================
    /// Returns optimalPolicy computed so far current size
    // ==========================================================================
    virtual Size optimalPolicySize() {
      return _optimalPolicy != nullptr ? _optimalPolicy->realSize() : 0;
    }

    // ==========================================================================
    /// Provide a better toDot for the optimal policy where the leaves have the
    /// action
    /// name instead of its id
    // ==========================================================================
    std::string optimalPolicy2String();

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
    virtual void initialize( const FMDP<GUM_SCALAR>* fmdp );


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
    virtual void makePlanning( Idx nbStep = 1000000 );

    /// @}


    // ###################################################################
    /// @name Value Iteration Methods
    // ###################################################################
    /// @{

    protected:
    // ==========================================================================
    ///
    // ==========================================================================
    virtual void _initVFunction();

    // ==========================================================================
    /// Performs a single step of value iteration
    // ==========================================================================
    virtual MultiDimFunctionGraph<GUM_SCALAR>* _valueIteration();

    // ==========================================================================
    /// Performs the P(s'|s,a).V^{t-1}(s') part of the value itération
    // ==========================================================================
    virtual MultiDimFunctionGraph<GUM_SCALAR>*
    _evalQaction( const MultiDimFunctionGraph<GUM_SCALAR>*, Idx );

    // ==========================================================================
    /// Performs max_a Q(s,a)
    /// @warning Performs also the deallocation of the QActions
    // ==========================================================================
    virtual MultiDimFunctionGraph<GUM_SCALAR>*
    _maximiseQactions( std::vector<MultiDimFunctionGraph<GUM_SCALAR>*>& );

    // ==========================================================================
    /// Performs min_i F_i
    /// @warning Performs also the deallocation of the F_i
    // ==========================================================================
    virtual MultiDimFunctionGraph<GUM_SCALAR>*
    _minimiseFunctions( std::vector<MultiDimFunctionGraph<GUM_SCALAR>*>& );

    // ==========================================================================
    /// Perform the R(s) + gamma . function
    /// @warning function is deleted, new one is returned
    // ==========================================================================
    virtual MultiDimFunctionGraph<GUM_SCALAR>*
    _addReward( MultiDimFunctionGraph<GUM_SCALAR>* function, Idx actionId = 0 );

    /// @}


    // ###################################################################
    /// @name Optimal policy extraction methods
    // ###################################################################
    /// @{

    protected:
    // ==========================================================================
    /// Perform the required tasks to extract an optimal policy
    // ==========================================================================
    virtual void _evalPolicy();

    // ==========================================================================
    /**
     * Creates a copy of given Qaction that can be exploit by a Argmax.
     * Hence, this step consists in replacing each lea by an ArgMaxSet
     * containing the value of the leaf and the actionId of the Qaction
     *
     * @param Qaction : the function graph we want to transform
     * @param actionId : the action Id associated to that graph
     *
     * @warning delete the original Qaction, returns its conversion
     */
    // ==========================================================================
    MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>*
    _makeArgMax( const MultiDimFunctionGraph<GUM_SCALAR>* Qaction, Idx actionId );

    private:
    // ==========================================================================
    /// Recursion part for the createArgMaxCopy
    // ==========================================================================
    NodeId __recurArgMaxCopy(
        NodeId,
        Idx,
        const MultiDimFunctionGraph<GUM_SCALAR>*,
        MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>*,
        HashTable<NodeId, NodeId>& );

    protected:
    // ==========================================================================
    /// Performs argmax_a Q(s,a)
    /// @warning Performs also the deallocation of the QActions
    // ==========================================================================
    virtual MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>,
                                  SetTerminalNodePolicy>*
    _argmaximiseQactions(
        std::vector<MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>,
                                          SetTerminalNodePolicy>*>& );

    // ==========================================================================
    /// From V(s)* = argmax_a Q*(s,a), this function extract pi*(s)
    /// This function mainly consists in extracting from each ArgMaxSet
    /// presents at the leaves the associated ActionSet
    /// @warning deallocate the argmax optimal value function
    // ==========================================================================
    void _extractOptimalPolicy(
        const MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>,
                                    SetTerminalNodePolicy>* optimalValueFunction );

    private:
    // ==========================================================================
    /// Recursion part for the createArgMaxCopy
    // ==========================================================================
    NodeId
    __recurExtractOptPol( NodeId,
                          const MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>,
                                                      SetTerminalNodePolicy>*,
                          HashTable<NodeId, NodeId>& );

    // ==========================================================================
    /// Extract from an ArgMaxSet the associated ActionSet
    // ==========================================================================
    void __transferActionIds( const ArgMaxSet<GUM_SCALAR, Idx>&, ActionSet& );


    /// @}

    protected:
    // ==========================================================================
    /// The Factored Markov Decision Process describing our planning situation
    /// (NB : this one must have function graph as transitions and reward
    /// functions )
    // ==========================================================================
    const FMDP<GUM_SCALAR>* _fmdp;

    // ==========================================================================
    /// The Value Function computed iteratively
    // ==========================================================================
    MultiDimFunctionGraph<GUM_SCALAR>* _vFunction;

    // ==========================================================================
    /// The associated optimal policy
    /// @warning Leaves are ActionSet which contains the ids of the best actions
    /// While this is sufficient to be exploited, to be understood by a human
    /// somme translation from the _fmdp is required. optimalPolicy2String do
    /// this
    /// job.
    // ==========================================================================
    MultiDimFunctionGraph<ActionSet, SetTerminalNodePolicy>* _optimalPolicy;

    // ==========================================================================
    /// A Set to eleminate primed variables
    // ==========================================================================
    Set<const DiscreteVariable*> _elVarSeq;

    // ==========================================================================
    /// Discount Factor used for infinite horizon planning
    // ==========================================================================
    GUM_SCALAR _discountFactor;

    IOperatorStrategy<GUM_SCALAR>* _operator;

    // ==========================================================================
    /// Boolean used to indcates whether or not iteration informations should be
    /// displayed on terminal
    // ==========================================================================
    bool _verbose;


    private:
    // ==========================================================================
    /// The threshold value
    /// Whenever | V^{n} - V^{n+1} | < threshold, we consider that V ~ V*
    // ==========================================================================
    GUM_SCALAR __threshold;
    bool       __firstTime;
  };

} /* namespace gum */


#include <agrum/FMDP/planning/structuredPlaner_tpl.h>

#endif  // GUM_STRUCTURED_PLANNING_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
