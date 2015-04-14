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
 * @brief Headers of the ModelLearner class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#include <random>
#include <cstdlib>
// =========================================================================
#include <agrum/FMDP/learning/SDyna/spiti.h>
// =========================================================================

namespace gum {

  // ==========================================================================
  /// @name Constructor & destructor.
  // ==========================================================================

    // ###################################################################
    /**
     * Constructor
     * @param observationPhaseLenght : see SDYNA
     * @param nbValueIterationStep : see SDYNA
     * @param discountFactor : the \gamma used for the plannings
     * @param epsilon : the epsilon under which we consider V to be \epsilon-optimal
     * @param learningThreshold : threshold under which a variable is not install
     * on a node
     * @param similarityThreshold : threshold above which two leaves won't be merged
     * @param exploThreshold : threshold under which we explore another action than
     * the optimal one
     */
    // ###################################################################
    SPITI::SPITI (Idx observationPhaseLenght,
                  Idx nbValueIterationStep,
                  double discountFactor,
                  double epsilon,
                  double learningThreshold,
                  double similarityThreshold,
                  double exploThreshold ) : SDYNA(observationPhaseLenght,nbValueIterationStep, discountFactor)/*, __exploThreshold(exploThreshold)*/{

      GUM_CONSTRUCTOR(SPITI)

      __learner = new FMDPLearner<CHI2TEST, CHI2TEST, ITILEARNER>(this->_fmdp, learningThreshold, similarityThreshold );

      __planer = new SVI<double>(this->_fmdp, epsilon);

      __sss = 1.0;
    }


    // ###################################################################
    /**
     * Returns
     * @return a string describing the learned FMDP, and the associated
     * optimal policy.
     * Both in DOT language.
     */
    // ###################################################################
    SPITI::~SPITI (){

      delete __learner;

      delete __planer;

      GUM_DESTRUCTOR(SPITI)
    }



  // ==========================================================================
  /// @name Problem specification methods
  // ==========================================================================

    // ###################################################################
    /**
     * Inserts a new action in the SPITI.
     * @warning Without effect until method initialize is called
     * @param actionId : an id to identify the action
     * @param actionName : its humanly understandable name
     */
    // ###################################################################
    void SPITI::addAction(const Idx actionId, const std::string &actionName ){
      SDYNA::addAction(actionId, actionName);
      __learner->addAction(actionId, actionName);
    }


    // ###################################################################
    /**
     * Inserts a new variable in the SDyna instance.
     * @warning Without effect until method initialize is called
     * @param var : the var to be added.
     * Note that variable may or may not have all its modalities given.
     * If not they will be discovered by the SDyna architecture during the process
     */
    // ###################################################################
    void SPITI::addVariable( const DiscreteVariable* var ){
      __learner->addVariable(var);
    }



  // ==========================================================================
  /// @name Initialization
  // ==========================================================================

    // ###################################################################
    /**
     * Initializes the Sdyna instance internal satastructure; i.e. the planer and
     * the learner.
     */
    // ###################################################################
    void SPITI::initialize(  ){
      __learner->initialize();
      __planer->initialize();
      for( auto actionIter = this->_fmdp->beginActions(); actionIter != this->_fmdp->endActions(); ++actionIter )
        __explorething += *actionIter;
      for( auto varIter = this->_fmdp->beginVariables(); varIter != this->_fmdp->endVariables(); ++varIter )
        __sss *= (double) (*varIter)->domainSize();
    }



  // ==========================================================================
  /// @name Incremental methods
  // ==========================================================================

    // ###################################################################
    /**
     * Performs a feedback on the last transition.
     * In extenso, learn from the transition.
     * @param reachedState : the state reached after the transition
     * @param obtainedReward : the reward obtained during the transition
     * @warning Uses the __originalState and __performedAction stored in cache
     * If you want to specify the original state and the performed action, see below
     */
    // ###################################################################
    void SPITI::feedback(const Instantiation & reachedState, double obtainedReward){
      SDYNA::feedback(reachedState,obtainedReward);

      if( __statecpt.nbVisitedStates() == 0 )
        __statecpt.reset(reachedState);
      else
        if( !__statecpt.checkState(reachedState) )
            __statecpt.addState(reachedState);
    }

    // ###################################################################
    /**
     * @param the state in which we currently are
     * @return a set containing every optimal actions on that state
     */
    // ###################################################################
    ActionSet SPITI::_stateActionSet(const Instantiation& curState){

      double explo = (double)std::rand( ) / (double)RAND_MAX;
      double temp = std::pow( (__sss - (double) __statecpt.nbVisitedStates() ) / __sss, 3.0);
      double exploThreshold = temp < 0.1 ? 0.1 : temp;

      //std::cout << << exploThreshold << std::endl;

      if( __planer->optimalPolicy()->realSize() ){
        ActionSet optimalSet = __planer->optimalPolicy()->get(curState);
        if( explo > exploThreshold ) {
          //std::cout << "Exploit : " << optimalSet << std::endl;
          return optimalSet;
        } else {
          if( __explorething.size() > optimalSet.size()){
            ActionSet ret(__explorething);
            ret -= optimalSet;
            //std::cout << << "Explore : " << ret << std::endl;
            return ret;
          }
        }
      }

      //std::cout << "Explore : " << __explorething << std::endl;
      return __explorething;
    }

    // ###################################################################
    /**
     * Gives to the learner a new transition
     * @param actionId : the action on which the transition was made
     * @param obs : the observed transition
     * @return true if learning this transition implies structural changes
     * (can trigger a new planning)
     */
    // ###################################################################
    bool SPITI::_learnTransition( Idx actionId, const Observation* obs ){
      __learner->addObservation( actionId, obs );
      return false;
    }


    // ###################################################################
    /**
     * Starts a new planning
     * @param Idx : the maximal number of value iteration performed in this planning
     */
    // ###################################################################
    void SPITI::_makePlanning( Idx nbValueIterationStep){
      //std::cout << << "Updating decision trees ..." << std::endl;
      __learner->updateFMDP();
      //std::cout << << "Done" << std::endl;

      //std::cout << << "Planning ..." << std::endl;
      __planer->makePlanning(nbValueIterationStep);
      //std::cout << << "Done" << std::endl;
    }



  // ###################################################################
  //
  // ###################################################################
  std::string SPITI::toString( ){
    std::stringstream description;

    description << SDYNA::toString();

    if(__planer->optimalPolicy() )
        description << __planer->optimalPolicy2String() << std::endl;


    return description.str();
  }

} // End of namespace gum
