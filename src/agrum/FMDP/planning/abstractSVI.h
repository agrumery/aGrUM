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
 * @brief Headers of the AbstractSVI planer class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_ABSTRACT_SVI_H
#define GUM_ABSTRACT_SVI_H
// =========================================================================
#include <thread>
// =========================================================================
#include <agrum/core/argMaxSet.h>
#include <agrum/core/functors.h>
#include <agrum/core/inline.h>
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
// =========================================================================
#include <agrum/multidim/multiDimFunctionGraph.h>
#include <agrum/multidim/FunctionGraphUtilities/terminalNodePolicies/SetTerminalNodePolicy.h>
// =========================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/planning/actionSet.h>
// =========================================================================

namespace gum {

  /**
   * @class AbstractSVI abstractSVI.h <agrum/FMDP/planning/abstractSVI.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   *
   * Perform a structure value iteration planning
   *
   * Pure virtual functions :
   *  _regress, _maximize, _argmaximize, _add and _subtract
   *  are a priori the ones to be respecified according to the used datastructure
   *  (MDDs, DTs, BNs, ...)
   *
   */
  template<typename GUM_SCALAR>
  class AbstractSVI {

      // ###################################################################
      /// @name Constructor & destructor.
      // ###################################################################
      /// @{

  public:

        // ==========================================================================
        /// Default constructor
        // ==========================================================================
        AbstractSVI ( FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp, GUM_SCALAR epsilon = 0.00001 );

        // ==========================================================================
        /// Default destructor
        // ==========================================================================
        virtual ~AbstractSVI();

      /// @}

      // ###################################################################
      /// @name Datastructure access methods
      // ###################################################################
      /// @{

  public:

        // ==========================================================================
        /// Returns a const ptr on the Factored Markov Decision Process on which we're planning
        // ==========================================================================
        INLINE const FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp() { return _fmdp; }

        // ==========================================================================
        /// Returns a const ptr on the value function computed so far
        // ==========================================================================
        INLINE const MultiDimFunctionGraph<GUM_SCALAR>* vFunction() { return _vFunction; }

        // ==========================================================================
        /// Returns the best policy obtained so far
        // ==========================================================================
        INLINE const MultiDimFunctionGraph<ActionSet, SetTerminalNodePolicy>* optimalPolicy() { return _optimalPolicy; }

        // ==========================================================================
        /// Provide a better toDot for the optimal policy where the leaves have the action
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
        virtual void initialize();


        // ==========================================================================
        /**
         * Performs a value iteration
         *
         * @param nbStep : enables you to specify how many value iterations you wish to do.
         * makePlanning will then stop whether when optimal value function is reach or when nbStep have been performed
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
        /// Performs a single step of value iteration
        // ==========================================================================
        virtual MultiDimFunctionGraph< GUM_SCALAR >* _valueIteration();

        // ==========================================================================
        /// Performs the P(s'|s,a).V^{t-1}(s') part of the value itération
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* _evalQaction( const MultiDimFunctionGraph<GUM_SCALAR>*, Idx );

        // ==========================================================================
        /// Performs max_a Q(s,a)
        /// @warning Performs also the deallocation of the QActions
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* _maximiseQactions( std::vector<MultiDimFunctionGraph<GUM_SCALAR>*>& );

        // ==========================================================================
        /// Perform the R(s) + \gamma . function
        /// @warning function is deleted, new one is returned
        // ==========================================================================
        virtual MultiDimFunctionGraph< GUM_SCALAR >* _addReward ( MultiDimFunctionGraph< GUM_SCALAR >* function );

        // ==========================================================================
        /// Indicates if whether or not given var is to be eliminated.
        /// Called by the evalQaction.
        // ==========================================================================
        INLINE bool _shouldEleminateVar( const DiscreteVariable* v ){
          return v==nullptr?false:_fmdp->mapMainPrime().existsSecond(v);
        }

        // ==========================================================================
        /// Returns the last var in the var order for given graph function
        /// Called by the evalQaction.
        // ==========================================================================
        INLINE const DiscreteVariable* _lastVar( const MultiDimFunctionGraph< GUM_SCALAR >* function ){
          return function->variablesSequence().size() == 0 ? nullptr :
              function->variablesSequence().atPos( function->variablesSequence().size() - 1 );
        }

      /// @}



      // ###################################################################
      /// @name Optimal policy extraction methods
      // ###################################################################
      /// @{

  protected:
        // ==========================================================================
        /// Perform the required tasks to extract an optimal policy
        // ==========================================================================
        virtual void _evalPolicy ();

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
        MultiDimFunctionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* _makeArgMax (
                      const MultiDimFunctionGraph<GUM_SCALAR>* Qaction,
                      Idx actionId );

  private:
        // ==========================================================================
        /// Recursion part for the createArgMaxCopy
        // ==========================================================================
        NodeId __recurArgMaxCopy( NodeId,
                                  Idx,
                                  const MultiDimFunctionGraph<GUM_SCALAR>*,
                                  MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>*,
                                  HashTable<NodeId,NodeId>&);
  protected:
        // ==========================================================================
        /// Performs argmax_a Q(s,a)
        /// @warning Performs also the deallocation of the QActions
        // ==========================================================================
        virtual MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>* _argmaximiseQactions(
            std::vector<MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>*>& );

        // ==========================================================================
        /// From V(s)* = argmax_a Q*(s,a), this function extract \pi*(s)
        /// This function mainly consists in extracting from each ArgMaxSet
        /// presents at the leaves the associated ActionSet
        /// @warning deallocate the argmax optimal value function
        // ==========================================================================
        void _extractOptimalPolicy ( const MultiDimFunctionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* optimalValueFunction );

  private:
        // ==========================================================================
        /// Recursion part for the createArgMaxCopy
        // ==========================================================================
        NodeId __recurExtractOptPol(NodeId,
                                    const MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>*,
                                    HashTable<NodeId,NodeId>&);

        // ==========================================================================
        /// Extract from an ArgMaxSet the associated ActionSet
        // ==========================================================================
        void __transferActionIds( const ArgMaxSet<GUM_SCALAR, Idx>&, ActionSet&);


      /// @}



      // ###################################################################
      /// @name Graph Function Operations Methods
      // ###################################################################
      /// @{

  protected:

        // ==========================================================================
        /// Performs a multiplication/projection on given qAction
        /// @param qAction : the computed Q(s,a)
        /// @param pxip : the transition probabilities for the xip variable
        /// @param xip : the variable we eliminate on the projection
        /// @warning given qAction is deleted, return the new one
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* _regress(const MultiDimFunctionGraph< GUM_SCALAR >* qAction,
                                                            const MultiDimFunctionGraph< GUM_SCALAR >* pxi,
                                                            const DiscreteVariable* xi) = 0;

        // ==========================================================================
        /// Maximizes between QAction and VFunction
        /// @param qAction : the computed Q(s,a)
        /// @param vFunction : the vFunction so far
        /// @warning given vFunction and qAction are deleted, returns the new one
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* _maximize(const MultiDimFunctionGraph< GUM_SCALAR >* vFunction,
                                                             const MultiDimFunctionGraph< GUM_SCALAR >* qAction) = 0;

        // ==========================================================================
        /// ArgMaximizes between QAction and VFunction
        /// @param qAction : the computed Q(s,a)
        /// @param vFunction : the vFunction so far
        /// @warning given vFunction and qAction are deleted, returns the new one
        // ==========================================================================
        virtual MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>* _argmaximize(
                            const MultiDimFunctionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* vFunction,
                            const MultiDimFunctionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* qAction) = 0;

        // ==========================================================================
        /// Adds reward to given function( whether a qAction or vFunction)
        /// @param reward : R(s) or R(s,a)
        /// @param function : either V(s) or Q(s,a)
        /// @warning given function is deleted, returns the new one
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* _add(const MultiDimFunctionGraph< GUM_SCALAR >* function,
                                                        const MultiDimFunctionGraph< GUM_SCALAR >* reward) = 0;

        // ==========================================================================
        /// Subtract current VFunction from old VFunction to see if threshold is
        /// reached or not
        /// @param old and new VFuntion
        /// @warning this time, nothing is deleted
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* _subtract(const MultiDimFunctionGraph< GUM_SCALAR >* newVF,
                                                             const MultiDimFunctionGraph< GUM_SCALAR >* oldVF) = 0;


      /// @}

  protected:

        // ==========================================================================
        /// The Factored Markov Decision Process describing our planning situation
        /// (NB : this one must have decision graph as transitions and reward functions )
        // ==========================================================================
        const FactoredMarkovDecisionProcess<GUM_SCALAR>* _fmdp;

        // ==========================================================================
        /// The Value Function computed iteratively
        // ==========================================================================
        MultiDimFunctionGraph<GUM_SCALAR>* _vFunction;

        // ==========================================================================
        /// The associated optimal policy
        /// @warning Leaves are ActionSet which contains the ids of the best actions
        /// While this is sufficient to be exploited, to be understood by a human
        /// somme translation from the _fmdp is required. optimalPolicy2String do this
        /// job.
        // ==========================================================================
        MultiDimFunctionGraph< ActionSet, SetTerminalNodePolicy >* _optimalPolicy;

        // ==========================================================================
        /// A Set to eleminate primed variables
        // ==========================================================================
        Set< const DiscreteVariable* > _elVarSeq;

    private :
        // ==========================================================================
        /// The threshold value
        /// Whenever | V^{n} - V^{n+1} | < threshold, we consider that V ~ V*
        // ==========================================================================
        GUM_SCALAR __threshold;

        bool __firstTime;
  };

} /* namespace gum */


#include <agrum/FMDP/planning/abstractSVI.tcc>

#endif // GUM_ABSTRACT_SVI_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

