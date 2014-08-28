/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                         *
 *                                     *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or   *
 *   (at your option) any later version.                   *
 *                                     *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of    *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the     *
 *   GNU General Public License for more details.              *
 *                                     *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the             *
 *   Free Software Foundation, Inc.,                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.       *
 ***************************************************************************/

// ==============================================================================
#include <iostream>
#include <string>
#include <cstdarg>
// ==============================================================================
#include <agrum/agrum.h>
// ==============================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/io/dat/FMDPDatReader.h>
#include <agrum/FMDP/simulation/simulator.h>
#include <agrum/FMDP/learning/spimddi.h>
// ==============================================================================

#define xstrfy(s) strfy(s)
#define strfy(x) #x
#define GET_PATH_STR(x) xstrfy(GUM_SRC_PATH) "/run/ressources/FMDP/" #x

void run( const std::string sourceFilePath, const std::string traceFilePath, gum::Idx nbVar, ... ) {

  for( gum::Idx nbTest = 0; nbTest < 50; ++nbTest) {

    gum::Idx nbIte = 0;
    double rDisc = 0.0;

    // *********************************************************************************************
    // Chargement du fmdp servant de base
    // *********************************************************************************************
    gum::FactoredMarkovDecisionProcess<double> fmdp(true);
    gum::FMDPDatReader<double> reader ( &fmdp, sourceFilePath );
    reader.trace ( false );
    reader.proceed( );

    std::cout << fmdp.size() << std::endl;
    exit(0);
    // *********************************************************************************************
    // Initialisation des divers objets
    // *********************************************************************************************
    gum::Simulator sim(&fmdp);
    gum::SPIMDDI spim;
    gum::Instantiation initialState, endState;

    // Enregistrement des actions possibles auprès de SPIMDDI
    for( auto actionIter = fmdp.beginActions(); actionIter != fmdp.endActions(); ++actionIter)
      spim.addAction( *actionIter, fmdp.actionName(*actionIter));

    // Enregistrement des variables caractérisant les états auprès de SPIMDDI
    for(auto varIter = fmdp.beginVariables(); varIter != fmdp.endVariables(); ++varIter){
      spim.addVariable(*varIter);
      initialState.add(**varIter);
    }

    // Récupération des caractéristiques d'états finaux
    va_list ap;
    va_start(ap, nbVar);
    for(gum::Idx nbArg = 0; nbArg < nbVar; ++nbArg){
      std::string varName( va_arg(ap, char*) );
      std::string varModa( va_arg(ap, char*) );

      const gum::DiscreteVariable* varPtr = nullptr;
      for(auto varIter = fmdp.beginVariables(); varIter != fmdp.endVariables(); ++varIter)
        if(!varName.compare( (*varIter)->name())){
          varPtr= *varIter;
          break;
        }

      endState.add(*varPtr);

      gum::Idx varModaIdx = 0;
      for(gum::Idx varm = 0; varm < varPtr->domainSize(); ++varm)
        if(!varModa.compare( varPtr->label(varm))){
          varModaIdx = varm;
          break;
        }
      endState.chgVal(*varPtr, varModaIdx);
    }
    va_end(ap);
    sim.setEndState(endState);


    sim.setInitialStateRandomly();

    // Création de la variables récompenses (to be deprecated)
    const gum::MultiDimDecisionGraph<double>* reward = reinterpret_cast<const gum::MultiDimDecisionGraph<double>*>(fmdp.reward());
    for( auto rewardIter = reward->values().beginSafe(); rewardIter != reward->values().endSafe(); ++rewardIter)
      spim.addReward(rewardIter.second());

    spim.initialize();

    // ======================================================================================
    // Création du checker détat visité
    // ======================================================================================
    gum::Idx nbVisitedState = 0;
    gum::MultiDimDecisionGraph<bool>* visitedStatesCheck = new gum::MultiDimDecisionGraph<bool>();
    gum::NodeId falseId = visitedStatesCheck->manager()->addTerminalNode(false);
    gum::NodeId trueId = visitedStatesCheck->manager()->addTerminalNode(true);
    gum::NodeId parId = 0;
    gum::NodeId parModa = 0;
    for( auto varIter = fmdp.beginVariables(); varIter != fmdp.endVariables(); ++varIter ){

  //    std::cout << "Nouvelle variable : " << (*varIter)->name() << std::endl;

      visitedStatesCheck->add(**varIter);

  //    std::cout << "\tAjout noeud" << std::endl;
      gum::NodeId varId = visitedStatesCheck->manager()->addNonTerminalNode(*varIter);

  //    std::cout << "\tPar check" << std::endl;
      if(parId)
        visitedStatesCheck->manager()->setSon(parId, parModa, varId);
      else
        visitedStatesCheck->manager()->setRootNode(varId);

  //    std::cout << "\tBranching" << std::endl;
      for ( gum::Idx moda = 0; moda < (*varIter)->domainSize(); ++moda ){
  //      std::cout << "Moda : " << moda << " - State : " << sim.currentState().valFromPtr(*varIter) << std::endl;
        if( moda == sim.currentState().valFromPtr(*varIter))
          parModa = moda;
        else
          visitedStatesCheck->manager()->setSon(varId, moda, falseId);
      }
  //    std::cout << "TADA" << std::endl;
      parId = varId;
    }
    visitedStatesCheck->manager()->setSon(parId, parModa, trueId);



    std::ofstream traceFile;
    std::stringstream traceFileName;
    traceFileName << traceFilePath << "." << nbTest << ".csv";
    traceFile.open ( traceFileName.str(), std::ios::out | std::ios::trunc );
    if ( !traceFile ) {
      return;
    }

    for( gum::Idx nbRun = 0; nbRun < 1000; ++nbRun ){

      spim.setCurrentState(sim.currentState());

      while(!sim.hasReachEnd()){

        // Normal Iteration Part
        gum::Idx actionChosenId = spim.takeAction();
        sim.perform( actionChosenId );

        spim.feedback(sim.currentState(), sim.reward());

        rDisc = sim.reward() + 0.99*rDisc;

        if(! visitedStatesCheck->get(sim.currentState())){
          nbVisitedState++;

          gum::NodeId parId = visitedStatesCheck->root();
          gum::Idx parModa = sim.currentState().valFromPtr( visitedStatesCheck->node( parId )->nodeVar() );
          while( visitedStatesCheck->node( parId )->son( parModa ) != falseId ){
            parId = visitedStatesCheck->node( parId )->son( parModa );
            parModa = sim.currentState().valFromPtr( visitedStatesCheck->node( parId )->nodeVar() );
          }
          for( auto varIter = fmdp.beginVariables(); varIter != fmdp.endVariables(); ++varIter ){
            visitedStatesCheck->add(**varIter);
            gum::NodeId varId = visitedStatesCheck->manager()->addNonTerminalNode(*varIter);
            if(parId)
              visitedStatesCheck->manager()->setSon(parId, parModa, varId);
            else
              visitedStatesCheck->manager()->setRootNode(varId);
            for ( gum::Idx moda = 0; moda < (*varIter)->domainSize(); ++moda ){
              if( moda == sim.currentState().valFromPtr(*varIter))
                parModa = moda;
              else
                visitedStatesCheck->manager()->setSon(varId, moda, falseId);
            }
            parId = varId;
          }
          visitedStatesCheck->manager()->setSon(parId, parModa, trueId);
        }

        traceFile << nbTest << "\t" << nbIte << "\t" << nbRun << "\t" << nbVisitedState
                  << "\t" << spim.learnerSize() << "\t" << spim.modelSize() << "\t" << spim.optimalPolicySize()
                  << std::setprecision(15)  << "\t" << rDisc << std::endl;
        nbIte++;
      }
      sim.setInitialStateRandomly();
    }
    traceFile.close();

//    std::ofstream modelFile;
//    std::stringstream modelFileName;
//    modelFileName << GET_PATH_STR ( modelfactory ) << "." << nbTest << ".dot";
//    modelFile.open ( modelFileName.str(), std::ios::out | std::ios::trunc );
//    if ( !traceFile ) {
//      return;
//    }

//    modelFile << spim.toString();

//    modelFile.close();
 }

  std::cout << "FIN EVALUATION" << std::endl;

}


// *******************************************************************************
// Run the tests on a Coffee FMDP
// *******************************************************************************
void runCoffee(){
  std::string varName("huc");
  std::string varModa("yes");
  run ( GET_PATH_STR ( coffee/coffee.dat ), GET_PATH_STR ( trace.Coffee ), 1, varName.c_str(), varModa.c_str() );
}


// *******************************************************************************
// Run the tests on a Factory FMDP
// *******************************************************************************
void runFactory(){
  run ( GET_PATH_STR ( factory/factory.dat ), GET_PATH_STR ( trace.Factory ), 0 );
}


// *******************************************************************************
// The main function
// *******************************************************************************
int main ( int argc, char* argv[] ) {

  runCoffee();
//  runFactory();
  return EXIT_SUCCESS;
}
