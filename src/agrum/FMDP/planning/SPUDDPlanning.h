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
 * @brief Headers of the SPUDDPlanning class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// #define  TRACE_ALGO

#ifndef GUM_SPUDD_PLANNING_H
#define GUM_SPUDD_PLANNING_H

#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>

#include <agrum/multidim/multiDimDecisionDiagramBase.h>

#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>

namespace gum {

  /**
   * @class SPUDDPlanning SPUDDPlanning.h <agrum/FMDP/planning/SPUDDPlanning.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   * Perform a SPUDD planning on given in parameter factored markov decision process
   *
   */

  template <typename GUM_SCALAR> class SPUDDPlanning {

    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    /**
    * Default constructor
    */
    SPUDDPlanning(FactoredMarkovDecisionProcess<GUM_SCALAR> *fmdp,
                  GUM_SCALAR epsilon = 0.00001);

    /**
    * Default destructor
    */
    ~SPUDDPlanning();

    /// @}
    // ==========================================================================
    /// @name Planning Methods
    // ==========================================================================
    /// @{

    /**
     * Makes a spudd planning on FMDP
     */
    MultiDimDecisionDiagramBase<GUM_SCALAR> *makePlanning();

    private:
    /**
     * Evals the policy corresponding to the given value function
     */
    void __evalPolicy(const MultiDimDecisionDiagramBase<GUM_SCALAR> *V);

    /**
     * Evals value function for a given action
     */
    MultiDimDecisionDiagramBase<GUM_SCALAR> *
    __evalActionValue(const MultiDimDecisionDiagramBase<GUM_SCALAR> *Vold,
                      Sequence<const DiscreteVariable *> &elVarSeq);

    /**
     * Evals final value function by multiplying by discount reward and adding reward
     */
    MultiDimDecisionDiagramBase<GUM_SCALAR> *
    __addReward(const MultiDimDecisionDiagramBase<GUM_SCALAR> *Vold);

    /// @}
    // ==========================================================================
    /// @name Planning Algorithm Evaluation Methods
    // ==========================================================================
    /// @{

    public:
    /**
     * Method to eval the efficiency of makePlanning algorithm
     */
    MultiDimDecisionDiagramBase<GUM_SCALAR> *
    makePlanningAlgoEvaluation(const std::string saveFilesName, Idx mode = 1);

    private:
    /**
     * Method to eval the efficiency of Vaction evaluation
     */
    MultiDimDecisionDiagramBase<GUM_SCALAR> *__evalActionValueAlgoEvaluation(
        const MultiDimDecisionDiagramBase<GUM_SCALAR> *Vold,
        const std::string saveFilesName, Idx mode);

    /**
     * Method to eval addition the multiplication by discount reward and the addition
     * of reward to computed value function
     */
    MultiDimDecisionDiagramBase<GUM_SCALAR> *
    __addRewardAlgoEvaluation(const MultiDimDecisionDiagramBase<GUM_SCALAR> *Vold,
                              const std::string saveFilesName, Idx mode);

    /**
     * Evals the policy corresponding to the given value function
     */
    std::pair<Idx, Idx>
    __evalNbRetrogradeEvaluation(const MultiDimDecisionDiagramBase<GUM_SCALAR> *t1,
                                 const MultiDimDecisionDiagramBase<GUM_SCALAR> *t2);

    /// @}
    // ==========================================================================
    /// @name Optimal policy evaluation methods
    // ==========================================================================
    /// @{

    /**
      * Performs one last step of the algorithm to obtain the arg max equivalent of
     * the so far computed value function
      */
    MultiDimDecisionDiagramBase<std::pair<double, long>> *
    __argMaxValueFunction(const MultiDimDecisionDiagramBase<GUM_SCALAR> *V);

    /**
     * Creates a copy of given in parameter decision diagram and replaces leaves of
     * that diagram by a pair containing value of the leaf and
     * action to which is bind this diagram (given in parameter).
     */
    MultiDimDecisionDiagramBase<std::pair<double, long>> *
    __createArgMaxCopy(const MultiDimDecisionDiagramBase<GUM_SCALAR> *Vaction,
                       Idx actionId);

    /**
     * Once final V is computed upon arg max on last Vactions, this function creates
     * a diagram where all leaves tied to the same action are merged together.
     * Since this function is a recursive one to ease the merge of all identic nodes
     * to converge toward a cannonical policy, a factory and the current node are
     * needed to build
     * resulting diagram. Also we need an exploration table to avoid exploration of
     * already visited sub-graph part.
     */
    NodeId __makeOptimalPolicyDecisionDiagram(
        const MultiDimDecisionDiagramBase<std::pair<double, long>> *V,
        const gum::NodeId &currentNode,
        MultiDimDecisionDiagramFactoryBase<Idx> *factory,
        HashTable<NodeId, NodeId> &explorationTable);

    /**
     * Displays the optimal computed policy diagram
     */
    void __displayOptimalPolicy(MultiDimDecisionDiagramBase<Idx> *op);

    /**
     * Computed arg max of two vactions given in parameter
     */
    MultiDimDecisionDiagramBase<std::pair<double, long>> *
    __argMaxOn2MultiDimDecisionDiagrams(
        const MultiDimDecisionDiagramBase<std::pair<double, long>> *Vaction1,
        const MultiDimDecisionDiagramBase<std::pair<double, long>> *Vaction2);

    /**
     * Computed arg max of two vactions given in parameter
     */
    MultiDimDecisionDiagramBase<std::pair<double, long>> *__differenceOnPolicy(
        const MultiDimDecisionDiagramBase<std::pair<double, long>> *Vaction1,
        const MultiDimDecisionDiagramBase<std::pair<double, long>> *Vaction2);

    /// @}

    /// The Factored Markov Decision Process on which we do our planning (NB : this
    /// one must have decision diagram as transitions and reward functions )
    FactoredMarkovDecisionProcess<GUM_SCALAR> *__fmdp;

    /// The hyperparameter determining when we do stop value iteration.
    GUM_SCALAR __epsilon;

    /// RetroVar number save file
    std::ofstream __traceAlgoSaveFile;
  };

  extern template class SPUDDPlanning<float>;
  extern template class SPUDDPlanning<double>;
} /* namespace gum */

#include <agrum/FMDP/planning/SPUDDPlanning.tcc>

#endif // GUM_SPUDD_PLANNING_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
