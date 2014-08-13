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

    FMDPLearner::FMDPLearner(FactoredMarkovDecisionProcess<double> *target, double lT ) : __fmdp(target),
                                                                                                          __learningThreshold(lT){
      GUM_CONSTRUCTOR(FMDPLearner);
    }


    // ###################################################################
    // Default destructor
    // ###################################################################

    FMDPLearner::~FMDPLearner(){

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

    void FMDPLearner::addVariable( const DiscreteVariable* var ){
        __fmdp->addVariable(var);
        __mainVariables.insert(var);
    }

    // ###################################################################
    //
    // ###################################################################

    void FMDPLearner::addAction(const Idx actionId, const std::string &actionName ){
        __fmdp->addAction(actionId, actionName);
        __actionLearners.insert( actionId, new Set<IMDDI*>());
    }

    // ###################################################################
    //
    // ###################################################################

    void FMDPLearner::addReward( const DiscreteVariable* var ){
        __rewardVar = var;
    }



  // ==========================================================================
  //
  // ==========================================================================

    // ###################################################################
    //
    // ###################################################################

    void FMDPLearner::initialize(){

      for( auto actionIter = __fmdp->beginActions(); actionIter != __fmdp->endActions(); ++actionIter ){
        for( auto varIter = __fmdp->beginVariables(); varIter != __fmdp->endVariables(); ++varIter ){
            MultiDimDecisionGraph<double>* varTrans = new MultiDimDecisionGraph<double>();
            varTrans->setTableName("ACTION : " + __fmdp->actionName(*actionIter) + " - VARIABLE : " + (*varIter)->name());
            __fmdp->addTransitionForAction(__fmdp->actionName(*actionIter), *varIter, varTrans);
            __actionLearners[*actionIter]->insert(new IMDDI(varTrans,__learningThreshold, 1 - __learningThreshold,__mainVariables,__fmdp->main2prime(*varIter)));
        }
      }

      MultiDimDecisionGraph<double>* reward = new MultiDimDecisionGraph<double>();
      Set<const DiscreteVariable*> primeVariables;
      for (auto varIter = __fmdp->beginVariables(); varIter != __fmdp->endVariables(); ++varIter)
        primeVariables.insert(__fmdp->main2prime(*varIter));
      __fmdp->addReward(reward);
      __rewardLearner = new IMDDI(reward,__learningThreshold, 1 - __learningThreshold,primeVariables,__rewardVar,true);
    }

    // ###################################################################
    //
    // ###################################################################

    void FMDPLearner::addObservation( Idx actionId, const Observation* newObs ){

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

    Size FMDPLearner::size( ){

        Size s = 0;
        for( auto actionIter = __actionLearners.beginSafe(); actionIter != __actionLearners.endSafe(); ++actionIter )
            for( auto learnerIter = actionIter.val()->beginSafe(); learnerIter != actionIter.val()->endSafe(); ++learnerIter)
                s+= (*learnerIter)->size();

        s += __rewardLearner->size();

        return s;
    }



    // ###################################################################
    //
    // ###################################################################

    void FMDPLearner::updateFMDP(){

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
      __rewardLearner->toDG(__fmdp->mapMainPrime());
//      std::cout << "Fin"  << std::endl;
    }
} // End of namespace gum
