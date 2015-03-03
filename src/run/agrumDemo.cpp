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
//#include <agrum/agrum.h>
// ==============================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/io/dat/FMDPDatReader.h>
#include <agrum/FMDP/simulation/simulator.h>
#include <agrum/FMDP/learning/spimddi.h>
// ==============================================================================

#define xstrfy(s) strfy(s)
#define strfy(x) #x
#define GET_PATH_STR(x) xstrfy(GUM_SRC_PATH) "/run/ressources/" #x

void run( const std::string sourceFilePath, const std::string traceFilePath, gum::Idx nbVar, ... ) {

//  for( gum::Idx nbTest = 0; nbTest < 50; ++nbTest) {

    gum::Idx nbIte = 0;
    double rDisc = 0.0;

    // *********************************************************************************************
    // Chargement du fmdp servant de base
    // *********************************************************************************************
    gum::FactoredMarkovDecisionProcess<double> fmdp(true);
    gum::FMDPDatReader<double> reader ( &fmdp, sourceFilePath );
    reader.trace ( false );
    reader.proceed( );

    // *********************************************************************************************
    // Initialisation des divers objets
    // *********************************************************************************************
    gum::Simulator sim(&fmdp);
    gum::SPIMDDI spim(0.9,0.0001,0.9,0.1,100,10,0.1);
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
    //const gum::MultiDimDecisionGraph<double>* reward = reinterpret_cast<const gum::MultiDimDecisionGraph<double>*>(fmdp.reward());
    //for( auto rewardIter = reward->beginValues(); rewardIter != reward->endValues(); ++rewardIter)
    //  for( reward->beginValues(); reward->hasValue(); reward->nextValue() )
    //    spim.addReward(reward->value());

    spim.initialize();

    // ======================================================================================
    // Création du checker détat visité
    // ======================================================================================
    //gum::Idx nbVisitedState = 0;
    //gum::MultiDimDecisionGraph<bool>* visitedStatesCheck = new gum::MultiDimDecisionGraph<bool>();
    //gum::NodeId falseId = visitedStatesCheck->manager()->addTerminalNode(false);
    //gum::NodeId trueId = visitedStatesCheck->manager()->addTerminalNode(true);
    //gum::NodeId parId = 0;
    //gum::NodeId parModa = 0;
    //for( auto varIter = fmdp.beginVariables(); varIter != fmdp.endVariables(); ++varIter ){

//      std::cout << "Nouvelle variable : " << (*varIter)->name() << std::endl;

    //  visitedStatesCheck->add(**varIter);

//      std::cout << "\tAjout noeud" << std::endl;
    //  gum::NodeId varId = visitedStatesCheck->manager()->addNonTerminalNode(*varIter);

//      std::cout << "\tPar check" << std::endl;
    //  if(parId)
    //    visitedStatesCheck->manager()->setSon(parId, parModa, varId);
    //  else
    //    visitedStatesCheck->manager()->setRootNode(varId);

//      std::cout << "\tBranching" << std::endl;
    //  for ( gum::Idx moda = 0; moda < (*varIter)->domainSize(); ++moda ){
//        std::cout << "Moda : " << moda << " - State : " << sim.currentState().valFromPtr(*varIter) << std::endl;
    //    if( moda == sim.currentState().valFromPtr(*varIter))
    //      parModa = moda;
    //    else
    //      visitedStatesCheck->manager()->setSon(varId, moda, falseId);
    //  }
//      std::cout << "TADA" << std::endl;
    //  parId = varId;
    //}
    //visitedStatesCheck->manager()->setSon(parId, parModa, trueId);



//    std::ofstream traceFile;
//    std::stringstream traceFileName;
//    traceFileName << traceFilePath << "." << nbTest << ".csv";
//    traceFile.open ( traceFileName.str(), std::ios::out | std::ios::trunc );
//    if ( !traceFile ) {
//      return;
//    }

//    std::ofstream fmdpFile;
//    std::stringstream fmdpFileName;
//    fmdpFileName << traceFilePath << "." << nbTest << ".TOSTRING.dot";
//    fmdpFile.open ( fmdpFileName.str(), std::ios::out | std::ios::trunc );
//    if ( !fmdpFile ) {
//      return;
//    }

    for( gum::Idx nbRun = 0; nbRun < 1000; ++nbRun ){

      spim.setCurrentState(sim.currentState());

      while(!sim.hasReachEnd()){

        // Normal Iteration Part
        gum::Idx actionChosenId = spim.takeAction();
        sim.perform( actionChosenId );

        try{
          spim.feedback(sim.currentState(), sim.reward());
        } catch(gum::Exception e){
          std::cout << e.errorCallStack() << std::endl;
          exit(1);
        }

        rDisc = sim.reward() + 0.9*rDisc;

//        if(! visitedStatesCheck->get(sim.currentState())){
//          nbVisitedState++;

//          gum::NodeId parId = visitedStatesCheck->root();
//          gum::Idx parModa = sim.currentState().valFromPtr( visitedStatesCheck->node( parId )->nodeVar() );
//          while( visitedStatesCheck->node( parId )->son( parModa ) != falseId ){
//            parId = visitedStatesCheck->node( parId )->son( parModa );
//            parModa = sim.currentState().valFromPtr( visitedStatesCheck->node( parId )->nodeVar() );
//          }
//          for( auto varIter = fmdp.beginVariables(); varIter != fmdp.endVariables(); ++varIter ){
//            visitedStatesCheck->add(**varIter);
//            gum::NodeId varId = visitedStatesCheck->manager()->addNonTerminalNode(*varIter);
//            if(parId)
//              visitedStatesCheck->manager()->setSon(parId, parModa, varId);
//            else
//              visitedStatesCheck->manager()->setRootNode(varId);
//            for ( gum::Idx moda = 0; moda < (*varIter)->domainSize(); ++moda ){
//              if( moda == sim.currentState().valFromPtr(*varIter))
//                parModa = moda;
//              else
//                visitedStatesCheck->manager()->setSon(varId, moda, falseId);
//            }
//            parId = varId;
//          }
//          visitedStatesCheck->manager()->setSon(parId, parModa, trueId);
//        }

//        traceFile << nbTest << "\t" << nbIte << "\t" << nbRun << "\t" << nbVisitedState
//                  << "\t" << spim.learnerSize() << "\t" << spim.modelSize() << "\t" << spim.optimalPolicySize()
//                  << std::setprecision(15)  << "\t" << rDisc << std::endl;

//        if(nbIte == 100 || nbIte == 1000)
//          fmdpFile << spim.toString() << std::endl << "========================================================================================" << std::endl;
        nbIte++;
      }
      sim.setInitialStateRandomly();
    }
//    traceFile.close();

//    std::ofstream modelFile;
//    std::stringstream modelFileName;
//    modelFileName << GET_PATH_STR ( modelfactory ) << "." << nbTest << ".dot";
//    modelFile.open ( modelFileName.str(), std::ios::out | std::ios::trunc );
//    if ( !traceFile ) {
//      return;
//    }

//    modelFile << spim.toString();

//    modelFile.close();
//    fmdpFile << spim.toString() << std::endl;
// }

  std::cout << "FIN EVALUATION" << std::endl;

}


// *******************************************************************************
// Run the tests on a Coffee FMDP
// *******************************************************************************
void runCoffee(){
  std::string varName("huc");
  std::string varModa("yes");
  std::cout << "hello" << std::endl;
  run ( GET_PATH_STR ( FMDP/coffee/coffee.dat ), GET_PATH_STR ( FMDP/trace.Coffee ), 1, varName.c_str(), varModa.c_str() );
}


// *******************************************************************************
// Run the tests on a Factory FMDP
// *******************************************************************************
void runFactory(){
  std::string varName("connected");
  std::string varModa("yes");
  std::cout << "hello" << std::endl;
  run ( GET_PATH_STR ( FMDP/factory/factory.dat ), GET_PATH_STR ( FMDP/trace.Factory ), 0 );
}

// *******************************************************************************
// The main function
// *******************************************************************************
int main ( int argc, char* argv[] ) {

  srand(time(NULL));

  runCoffee();
//  runFactory();

//for(gum::Idx nbTest=1; nbTest < 27; nbTest++ ){
//  std::vector<std::string> vnv;
//  std::vector<gum::Idx> vmv;

//  std::stringstream inputFileName;
//  inputFileName << GET_PATH_STR(dataBNs/)  <<  nbTest << "-10000.csv";


//  // ############################################################################
//  // Informations Récupération
//  // ############################################################################
//  std::ifstream fichier(inputFileName.str(), std::ios::in);  // on ouvre le fichier en lecture

//  if(!fichier) { // si l'ouverture a réussi
//    std::cerr << "Impossible d'ouvrir le fichier : " << inputFileName.str() << "!" << std::endl;
//    return 1;
//  }

//  std::string ligne;
//  std::istringstream iss1;
//  std::string mot;

//  std::cout << "DEBUT CARACTERISATION VARIABLES" << std::endl;
//  std::getline(fichier, ligne);
//  iss1.str(ligne);
//  while( std::getline(iss1, mot, ',') ){
//    vnv.push_back( mot );
//    vmv.push_back( 1 );
//  }

//  while( std::getline(fichier, ligne) ){
//      std::istringstream iss2(ligne);
//      std::string moda;

//      int index = 0;
//      while( std::getline(iss2, moda, ',') ){
////              std::cout << moda << " - " << std::stoi(moda) << std::endl;
//        gum::Idx value = std::stoi(moda);
//        if( value > vmv[index])
//          vmv[index] = value;
//        index++;
//      }
//  }

//  fichier.close();  // on ferme le fichier

//  std::cout << "FIN CARACTERISATION VARIABLES" << std::endl;


//  // REMBOBINAGE
////  fichier.seekg(0, std::ios::beg);
////  if(fichier.eof())
////    std::cout << "Oh Dear we're in trouble!" << std::endl;
////  std::getline(fichier,ligne);

//  // ############################################################################
//  // Learning phase :
//  // ############################################################################
//  gum::MultiDimDecisionGraph<double>* target = new gum::MultiDimDecisionGraph<double>();
//  gum::Set<const gum::DiscreteVariable*> varList;
//  std::vector<const gum::DiscreteVariable*> vv(vnv.size());
//  for(gum::Idx i = 0; i < vnv.size() - 1; i++ ){
//    vv[i] = new gum::LabelizedVariable(vnv[i],"",vmv[i]+1);
//    varList << vv[i];
//  }
//  gum::LabelizedVariable* value = new gum::LabelizedVariable(vnv[vnv.size()-1],"",vmv[vmv.size()-1]+1);
//  gum::IMDDI learner(target, 0.95, 0.95, varList, value);

//  fichier.open(inputFileName.str(), std::ios::in);  // on ouvre le fichier en lecture

//  if(!fichier) { // si l'ouverture a réussi
//    std::cerr << "Impossible d'ouvrir le fichier : " << inputFileName.str() << "!" << std::endl;
//    return 1;
//  }
//  std::getline(fichier, ligne);

//  std::ofstream traceFile;
//  std::stringstream traceFileName;
//  traceFileName << GET_PATH_STR(/) << nbTest << ".csv";
//  traceFile.open ( traceFileName.str(), std::ios::out | std::ios::trunc );
//  if ( !traceFile ) {
//    return 2;
//  }


//  gum::Idx nbIte = 0;
//  while( std::getline(fichier, ligne) ){
//    std::istringstream iss2(ligne);
//    std::string moda;

//    gum::Idx index = 0;
//    std::cout << "Taking new Observation into account : " << nbIte << std::endl;
//    gum::Observation* obs = new gum::Observation();
//    while( std::getline(iss2, moda, ',') ){
////        std::cout << index << "-" << std::stoi(moda) << std::endl;
//      if( index < vv.size() - 1 ){
////        std::cout << "attribut " << vv[index]->toString() << std::endl;
//        obs->setModality(vv[index],std::stoi(moda));
//      }else{
////        std::cout << "value " << value->toString() << std::endl;
//        obs->setModality(value, std::stoi(moda));
//      }
//      index++;
//    }
////    std::cout << obs->toString() << std::endl;
//    gum::Timer timy;
//    timy.reset();
//    learner.addObservation( obs );
//    double tempAdd = timy.step();
//    learner.updateOrderedTree();
//    double tempUp = timy.step();
//    learner.toDG();
//    double tempCon = timy.step();
//    traceFile << nbIte << ";" << tempAdd << ";" << tempUp << ";" << tempCon << std::endl;
//    nbIte++;
//  }

//  fichier.close();  // on ferme le fichier
//  traceFile.close();
//}

//  return 0;
  return EXIT_SUCCESS;
}
