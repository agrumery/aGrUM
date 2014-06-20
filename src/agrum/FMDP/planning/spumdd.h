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
 * @brief Headers of the SPUMDD planer class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_SPUMDD_H
#define GUM_SPUMDD_H
// =========================================================================
#include <agrum/multidim/multiDimDecisionGraph.h>
// =========================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
// =========================================================================

namespace gum {

  /**
   * @class SPUMDD spumdd.h <agrum/FMDP/planning/spumdd.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   *
   * Perform a SPUDD planning on given in parameter factored markov decision process
   *
   */

  template<typename GUM_SCALAR>
  class SPUMDD {

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

       /**
        * Default constructor
        */
        SPUMDD ( FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp, GUM_SCALAR epsilon = 0.00001 );

       /**
        * Default destructor
        */
        ~SPUMDD();

      /// @}

      INLINE const FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp() {return __fmdp;}

      // ==========================================================================
      /// @name Planning Methods
      // ==========================================================================
      /// @{

        /**
         * Makes a spudd planning on FMDP
         */
         void initialize();

        /**
         * Makes a spudd planning on FMDP
         */
         MultiDimDecisionGraph< GUM_SCALAR >* makePlanning(Idx nbStep);

        /**
         * Returns an iterator reference to the beginning of the var eleminstation sequence
         * @warning in reverse mode (from end to beginning)
         */
         INLINE SequenceIteratorSafe<const DiscreteVariable*>& beginVarElimination() { return __elVarSeq.rbeginSafe(); }

        /**
         * Returns an iterator to the end
         * @warning in reverse mode (from end to beginning)
         */
         INLINE SequenceIteratorSafe<const DiscreteVariable*>& endVarElemination() {return __elVarSeq.rendSafe();}

         /// Add computed Qaction to table
         void addQaction(Idx, MultiDimDecisionGraph<GUM_SCALAR>*);

    private:

       /**
        * Evals value function for a given action
        */
//        MultiDimDecisionGraph< GUM_SCALAR >* __evalActionValue ( const MultiDimDecisionGraph< GUM_SCALAR >* Vold, Idx actionId );

       /**
        * Evals final value function by multiplying by discount reward and adding reward
        */
        MultiDimDecisionGraph< GUM_SCALAR >* __addReward ( const MultiDimDecisionGraph< GUM_SCALAR >* Vold );

        /**
         * Evals the policy corresponding to the given value function
         */
         void __evalPolicy ( const MultiDimDecisionGraph< GUM_SCALAR >* V );

      /// @}

      // ==========================================================================
      /// @name Optimal policy evaluation methods
      // ==========================================================================
      /// @{

       /**
        * Performs one last step of the algorithm to obtain the arg max equivalent of the so far computed value function
        */
//        MultiDimDecisionGraph< std::pair< double, long > >* __argMaxValueFunction ( const MultiDimDecisionGraph< GUM_SCALAR >* V );

       /**
        * Creates a copy of given in parameter decision diagram and replaces leaves of that diagram by a pair containing value of the leaf and
        * action to which is bind this diagram (given in parameter).
        */
//        MultiDimDecisionGraph< std::pair< double, long > >* __createArgMaxCopy ( const MultiDimDecisionGraph<GUM_SCALAR>* Vaction, Idx actionId );

       /**
        * Once final V is computed upon arg max on last Vactions, this function creates a diagram where all leaves tied to the same action are merged together.
        * Since this function is a recursive one to ease the merge of all identic nodes to converge toward a cannonical policy, a factory and the current node are needed to build
        * resulting diagram. Also we need an exploration table to avoid exploration of already visited sub-graph part.
        */
//        NodeId __makeOptimalPolicyDecisionDiagram ( const MultiDimDecisionGraph< std::pair< double, long > >* V, const gum::NodeId& currentNode, MultiDimDecisionDiagramFactoryBase< Idx >* factory, HashTable< NodeId, NodeId >& explorationTable );

       /**
        * Displays the optimal computed policy diagram
        */
//        void __displayOptimalPolicy ( MultiDimDecisionGraph< Idx >* op );

       /**
        * Computed arg max of two vactions given in parameter
        */
//        MultiDimDecisionGraph< std::pair< double, long > >* __argMaxOn2MultiDimDecisionDiagrams ( const MultiDimDecisionGraph< std::pair< double, long > >* Vaction1, const MultiDimDecisionGraph< std::pair< double, long > >* Vaction2 );



       /**
        * Computed arg max of two vactions given in parameter
        */
//        MultiDimDecisionGraph< std::pair< double, long > >* __differenceOnPolicy ( const MultiDimDecisionGraph< std::pair< double, long > >* Vaction1, const MultiDimDecisionGraph< std::pair< double, long > >* Vaction2 );

      /// @}

      /// The Factored Markov Decision Process describing our planning situation
      /// (NB : this one must have decision graph as transitions and reward functions )
      const FactoredMarkovDecisionProcess<GUM_SCALAR>* __fmdp;

      /// The Value Function computed iteratively
      MultiDimDecisionGraph<GUM_SCALAR>* __vFunction;

      /// A table giving for every action the associated Q function
      HashTable<Idx, MultiDimDecisionGraph<GUM_SCALAR>*> __qFunctionTable;
//      Set<MultiDimDecisionGraph<GUM_SCALAR>*> __qFunctionTable;

      /// A locker on the above table for multitreading purposes
      std::mutex __qTableMutex;

      /// The hyperparameter determining when we do stop value iteration.
      GUM_SCALAR __threshold;

      /// A sequence to eleminate primed variables
      Sequence< const DiscreteVariable* > __elVarSeq;
  };

  template<typename GUM_SCALAR>
  void evalQaction( SPUMDD<GUM_SCALAR>* planer, const MultiDimDecisionGraph<GUM_SCALAR>* Vold, Idx actionId );

  extern template class SPUMDD<float>;
  extern template class SPUMDD<double>;
} /* namespace gum */


#include <agrum/FMDP/planning/spumdd.tcc>

#endif // GUM_SPUMDD_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

