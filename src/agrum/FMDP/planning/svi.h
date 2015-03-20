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
 * @brief Headers of the SVI planer class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_SVI_H
#define GUM_SVI_H
// =========================================================================
#include <thread>
// =========================================================================
#include <agrum/core/argMaxSet.h>
#include <agrum/core/functors.h>
#include <agrum/core/inline.h>
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
// =========================================================================
#include <agrum/multidim/multiDimDecisionGraph.h>
#include <agrum/multidim/decisionGraphUtilities/terminalNodePolicies/SetTerminalNodePolicy.h>
// =========================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/planning/actionSet.h>
// =========================================================================

namespace gum {

  /**
   * @class SVI svi.h <agrum/FMDP/planning/svi.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   *
   * Perform a SVI planning on given in parameter factored markov decision process
   *
   */
  template<typename GUM_SCALAR>
  class SVI {

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

       /**
        * Default constructor
        */
        SVI ( FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp, GUM_SCALAR epsilon = 0.00001 );

       /**
        * Default destructor
        */
        virtual ~SVI();

      /// @}

      // ==========================================================================
      /// @name Miscelleanous methods
      // ==========================================================================
      /// @{

        /// Returns a const ptr on the Factored Markov Decision Process on which we're planning
        INLINE const FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp() { return _fmdp; }

        /// Returns a const ptr on the value function computed so far
        INLINE const MultiDimDecisionGraph<GUM_SCALAR>* vFunction() { return _vFunction; }

        /// Returns the best policy obtained so far
        INLINE const MultiDimDecisionGraph<ActionSet, SetTerminalNodePolicy>* optimalPolicy() { return __optimalPolicy; }
        std::string optimalPolicy2String();

      /// @}

      // ==========================================================================
      /// @name Planning Methods
      // ==========================================================================
      /// @{

        /**
         * Initializes data structure needed for making the planning
         * @warning No calling this methods before starting the first makePlaninng
         * will surely and definitely result in a crash
         */
        virtual void initialize();

        /**
         * Performs a value iteration
         *
         * @param nbStep : enables you to specify how many value iterations you wish to do.
         * makePlanning will then stop whether when optimal value function is reach or when nbStep have been performed
         */
        void makePlanning(Idx nbStep = 1000000);

  protected:

        /// Performs a single step of value iteration
        virtual MultiDimDecisionGraph< GUM_SCALAR >* _valueIteration();

        virtual MultiDimDecisionGraph<GUM_SCALAR>* _evalQaction( const MultiDimDecisionGraph<GUM_SCALAR>*, Idx );

  public:

        /// Add computed Qaction to table
        void addQaction( MultiDimDecisionGraph<GUM_SCALAR>* );
        void addQaction( MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* );

        INLINE bool shouldEleminateVar( const DiscreteVariable* v ){
          return v==nullptr?false:_fmdp->mapMainPrime().existsSecond(v);
        }

        INLINE const Set< const DiscreteVariable* >* elVarSeq(){
          return &__elVarSeq;
        }

  protected:

        /**
         * Terminates a value iteration by multiplying by discount factor the so far computed value function.
         * Then adds reward function
         */
        MultiDimDecisionGraph< GUM_SCALAR >* _addReward ( const MultiDimDecisionGraph< GUM_SCALAR >* Vold );

      /// @}

      // ==========================================================================
      /// @name Optimal policy evaluation methods
      // ==========================================================================
      /// @{
  private:
        /**
         * Once value ieration is over, this methods is call to find an associated optimal policy
         */
        void __evalPolicy ();

       /**
        * Creates a copy of given in parameter decision diagram and replaces leaves of that diagram by a pair containing value of the leaf and
        * action to which is bind this diagram (given in parameter).
        */
        MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* __createArgMaxCopy (
            const MultiDimDecisionGraph<GUM_SCALAR>* Vaction,
            Idx actionId );

       /**
        * Once final V is computed upon arg max on last Vactions, this function creates a diagram where all leaves tied to the same action are merged together.
        * Since this function is a recursive one to ease the merge of all identic nodes to converge toward a cannonical policy, a factory and the current node are needed to build
        * resulting diagram. Also we need an exploration table to avoid exploration of already visited sub-graph part.
        */
       void __extractOptimalPolicy ( const MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* optimalValueFunction );
       void __transferActionIds( const ArgMaxSet<GUM_SCALAR, Idx>&, ActionSet&);


      /// @}

  protected:
      /// The Factored Markov Decision Process describing our planning situation
      /// (NB : this one must have decision graph as transitions and reward functions )
      const FactoredMarkovDecisionProcess<GUM_SCALAR>* _fmdp;

      /// The Value Function computed iteratively
      MultiDimDecisionGraph<GUM_SCALAR>* _vFunction;

  private:
      /// The associated optimal policy
      /// @warning This decision graph has Idx as leaf.
      /// Indeed, its leaves are a match to the fmdp action ids
      MultiDimDecisionGraph< ActionSet, SetTerminalNodePolicy >* __optimalPolicy;

  protected:
      /// A table giving for every action the associated Q function
      std::vector<MultiDimDecisionGraph<GUM_SCALAR>*> _qFunctionSet;
  private:
      std::vector<MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >*> __argMaxQFunctionSet;

      /// A locker on the above table for multitreading purposes
      std::mutex __qSetMutex;

      /// The threshold value
      /// Whenever | Vn - Vn+1 | < threshold, we consider that V ~ V*
      GUM_SCALAR __threshold;

      /// A Set to eleminate primed variables
      Set< const DiscreteVariable* > __elVarSeq;

      bool __firstTime;
  };

//  template<typename GUM_SCALAR>
//  void evalQactionForArgMax( SPUMDD<GUM_SCALAR>* planer, const MultiDimDecisionGraph<GUM_SCALAR>* Vold, Idx actionId );

  extern template class SVI<float>;
  extern template class SVI<double>;
} /* namespace gum */


#include <agrum/FMDP/planning/svi.tcc>

#endif // GUM_SVI_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

