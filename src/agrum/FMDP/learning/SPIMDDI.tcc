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
 * @brief Headers of the SPIMDDI class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */


// =========================================================================
#include <agrum/FMDP/learning/SPIMDDI.h>
// =========================================================================

namespace gum {

  // ==========================================================================
  // Constructor & destructor.
  // ==========================================================================

    // ###################################################################
    // Default constructor
    // ###################################################################
    template <typename GUM_SCALAR>
    SPIMDDI<GUM_SCALAR>::SPIMDDI( ){

      GUM_CONSTRUCTOR(SPIMDDI);

      __fmdp = new FactoredMarkovDecisionProcess<GUM_SCALAR>();
      __planer = new SPUDDPlanning<GUM_SCALAR>(__fmdp);
    }


    // ###################################################################
    // Default desstructor
    // ###################################################################
    template <typename GUM_SCALAR>
    SPIMDDI<GUM_SCALAR>::~SPIMDDI(){

      delete __planer;
      delete __fmdp;

      for( auto actionIter = __actionLearners.beginSafe(); actionIter != __actionLearners.endSafe(); ++actionIter ){
          for( auto learnerIter = (*actionIter)->beginSafe(); learnerIter = (*actionIter)->endSafe(); ++learnerIter)
              delete *learnerIter;
          delete *actionIter;
      }

      for(auto varIter = __primedVariables.beginSafe(); varIter != __primedVariables.endSafe(); ++varIter)
          delete varIter;

      GUM_DESTRUCTOR(SPIMDDI);
    }



  // ==========================================================================
  //
  // ==========================================================================

    // ###################################################################
    //
    // ###################################################################
    template <typename GUM_SCALAR>
    void SPIMDDI<GUM_SCALAR>::addVariable( const DiscreteVariable* var){
        __fmdp->addVariable(var);
        DiscreteVariable* prime = new LabelizedVariable(var);
        prime->setName( var->name() + "'");
        __fmdp->addPrimedVariable(prime);
        __primedVariables.insert(prime);

    }

    // ###################################################################
    //
    // ###################################################################
    template <typename GUM_SCALAR>
    void SPIMDDI<GUM_SCALAR>::addAction(std::string name){

        Idx actionId = __fmdp->addAction(name);
        __actionLearners[action] = new List<IMDDI<GUM_SCALAR>*>();
        __fmdp->resetVariablesIterator();
        while(__fmdp->hasVariable()){
            MultiDimDecisionGraph<GUM_SCALAR> varTrans = new MultiDimDecisionGraph<GUM_SCALAR>();
            __fmdp->addTransitionForAction(__fmdp->variable(), varTrans, __fmdp->actionName(actionId));
            __actionLearners[actionId]->insert(new IMDDI<GUM_SCALAR>(varTrans,0.95,__mainVariables,__fmdp->variable(),false));
        }
    }



  // ==========================================================================
  //
  // ==========================================================================

    // ###################################################################
    //
    // ###################################################################
    template <typename GUM_SCALAR>
    void SPIMDDI<GUM_SCALAR>::addObservation( const Observation* newObs){
    }
} // End of namespace gum
