/***************************************************************************
 *  Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN   *
 *  {prenom.nom}_at_lip6.fr                                                *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the                          *
 *  Free Software Foundation, Inc.,                                        *
 *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.               *
 ***************************************************************************/
/**
 * @file
 * @brief Headers of the FMDPLearner class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#include <agrum/FMDP/learning/FMDPLearner.h>
// =========================================================================

namespace gum {

  // ==========================================================================
  // Constructor & destructor.
  // ==========================================================================

    // ###################################################################
    // Default constructor
    // ###################################################################
    template <typename GUM_SCALAR>
    FMDPLearner<GUM_SCALAR>::FMDPLearner(FactoredMarkovDecisionProcess<GUM_SCALAR> *target, double lT ) : __fmdp(target),
                                                                                                          __learningThreshold(lT){
      GUM_CONSTRUCTOR(FMDPLearner);
    }


    // ###################################################################
    // Default destructor
    // ###################################################################
    template <typename GUM_SCALAR>
    FMDPLearner<GUM_SCALAR>::~FMDPLearner(){

      for( auto actionIter = __actionLearners.beginSafe(); actionIter != __actionLearners.endSafe(); ++actionIter ){
          for( auto learnerIter = actionIter.val()->beginSafe(); learnerIter != actionIter.val()->endSafe(); ++learnerIter)
              delete *learnerIter;
          delete actionIter.val();
      }

      delete __rewardLearner;

      GUM_DESTRUCTOR(FMDPLearner);
    }



  // ==========================================================================
  //
  // ==========================================================================

    // ###################################################################
    //
    // ###################################################################
    template <typename GUM_SCALAR>
    void FMDPLearner<GUM_SCALAR>::addVariable( const DiscreteVariable* var ){
        __fmdp->addVariable(var);
        __mainVariables.insert(var);
    }

    // ###################################################################
    //
    // ###################################################################
    template <typename GUM_SCALAR>
    void FMDPLearner<GUM_SCALAR>::addAction(const Idx actionId, const std::string &actionName ){
        __fmdp->addAction(actionId, actionName);
        __actionLearners.insert( actionId, new Set<IMDDI<GUM_SCALAR>*>());
    }

    // ###################################################################
    //
    // ###################################################################
    template <typename GUM_SCALAR>
    void FMDPLearner<GUM_SCALAR>::addReward( const DiscreteVariable* var ){
        __rewardVar = var;
    }



  // ==========================================================================
  //
  // ==========================================================================

    // ###################################################################
    //
    // ###################################################################
    template <typename GUM_SCALAR>
    void FMDPLearner<GUM_SCALAR>::initialize(){

      for( auto actionIter = __fmdp->beginActions(); actionIter != __fmdp->endActions(); ++actionIter ){
        for( auto varIter = __fmdp->beginVariables(); varIter != __fmdp->endVariables(); ++varIter ){
            MultiDimDecisionGraph<GUM_SCALAR>* varTrans = new MultiDimDecisionGraph<GUM_SCALAR>();
            varTrans->setTableName("ACTION : " + __fmdp->actionName(*actionIter) + " - VARIABLE : " + (*varIter)->name());
            __fmdp->addTransitionForAction(__fmdp->actionName(*actionIter), *varIter, varTrans);
            __actionLearners[*actionIter]->insert(new IMDDI<GUM_SCALAR>(varTrans,__learningThreshold,__mainVariables,__fmdp->main2prime(*varIter)));
        }
      }

      MultiDimDecisionGraph<GUM_SCALAR>* reward = new MultiDimDecisionGraph<GUM_SCALAR>();
      __fmdp->addReward(reward);
      __rewardLearner = new IMDDI<GUM_SCALAR>(reward,__learningThreshold,__mainVariables,__rewardVar,true);
    }

    // ###################################################################
    //
    // ###################################################################
    template <typename GUM_SCALAR>
    void FMDPLearner<GUM_SCALAR>::addObservation( Idx actionId, const Observation* newObs ){

        for(auto learnerIter = __actionLearners[actionId]->beginSafe(); learnerIter != __actionLearners[actionId]->endSafe(); ++learnerIter){
            (*learnerIter)->addObservation(newObs);
            (*learnerIter)->updateOrderedTree();
        }

        __rewardLearner->addObservation(newObs);
        __rewardLearner->updateOrderedTree();
    }

    // ###################################################################
    //
    // ###################################################################
    template <typename GUM_SCALAR>
    void FMDPLearner<GUM_SCALAR>::updateFMDP(){

//      std::cout << "Beginning update"  << std::endl;
      for( auto actionIter = __fmdp->beginActions(); actionIter != __fmdp->endActions(); ++actionIter ){
//        std::cout << "Update for action : " << *actionIter << std::endl;
        for(auto learnerIter = __actionLearners[*actionIter]->beginSafe();
            learnerIter != __actionLearners[*actionIter]->endSafe(); ++learnerIter){
//          std::cout << "Update " << std::endl;
          (*learnerIter)->toDG();
        }
      }

//      std::cout << "Reward update"  << std::endl;
      __rewardLearner->toDG();
//      std::cout << "Fin"  << std::endl;
    }
} // End of namespace gum
