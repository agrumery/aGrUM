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
#include <agrum/FMDP/simulation/FMDPSimulator.h>
#include <agrum/FMDP/simulation/taxiSimulator.h>
#include <agrum/FMDP/simulation/statesChecker.h>
#include <agrum/FMDP/learning/spimddi.h>
// ==============================================================================

#define xstrfy(s) strfy(s)
#define strfy(x) #x
#define GET_PATH_STR(x) xstrfy(GUM_SRC_PATH) "/run/ressources/" #x

void run( gum::AbstractSimulator& sim, const std::string traceFilePath){

  for( gum::Idx nbTest = 0; nbTest < 50; ++nbTest) {

    gum::Idx nbIte = 0;
    double rDisc = 0.0;

    // *********************************************************************************************
    // Initialisation des divers objets
    // *********************************************************************************************
    gum::SPIMDDI spim(0.9,0.0001,0.99,0.33,100,10,0.1);
    gum::Instantiation initialState;

    // Enregistrement des actions possibles auprès de SPIMDDI
    for( auto actionIter = sim.beginActions(); actionIter != sim.endActions(); ++actionIter)
      spim.addAction( *actionIter, sim.actionName(*actionIter));

    // Enregistrement des variables caractérisant les états auprès de SPIMDDI
    for(auto varIter = sim.beginVariables(); varIter != sim.endVariables(); ++varIter){
      spim.addVariable(*varIter);
      initialState.add(**varIter);
    }


    sim.setInitialStateRandomly();
    spim.initialize();

    // ======================================================================================
    // Création du checker détat visité
    // ======================================================================================
    gum::StatesChecker sc;
    sc.reset(sim.currentState());

    // ======================================================================================
    // Ouverture des fichiers de traces
    // ======================================================================================
    std::ofstream traceFile;
    std::stringstream traceFileName;
    traceFileName << traceFilePath << "." << nbTest << ".csv";
    traceFile.open ( traceFileName.str(), std::ios::out | std::ios::trunc );
    if ( !traceFile ) {
      return;
    }

    std::cout << "Initialisation done. Now performing test ..." <<std::endl;
    for( gum::Idx nbRun = 0; nbRun < 1000; ++nbRun ){

      spim.setCurrentState(sim.currentState());


      for(gum::Idx nbDec = 0; nbDec < 25 && !sim.hasReachEnd(); nbDec++){

        // Normal Iteration Part
        std::cout << "\n*********************************************\n";
        std::cout << "RUN n° " << nbRun << " - Status : " << sim.currentState().toString() << " - Decision n° " << nbDec << " : ";

        gum::Idx actionChosenId = spim.takeAction();

        std::cout << "By doing " << sim.actionName(actionChosenId) << std::endl;

        sim.perform( actionChosenId );
        rDisc = sim.reward() + 0.9*rDisc;

        std::cout << "New State : " << sim.currentState() << " - Discounted Reward : " << rDisc;

        spim.feedback(sim.currentState(), sim.reward());


        if( !sc.checkState(sim.currentState()) ) {
          sc.addState(sim.currentState());
        }

        traceFile << nbTest << "\t" << nbIte << "\t" << nbRun << "\t" << nbDec << "\t" << sc.nbVisitedStates()
                  /*<< "\t" << fmdp.size()*/ << "\t" << spim.learnerSize() << "\t" << spim.modelSize() << "\t" << spim.optimalPolicySize()
                  << std::setprecision(15)  << "\t" << rDisc << std::endl;

        nbIte++;
      }
      sim.setInitialStateRandomly();
    }
    std::cout << spim.toString() << std::endl;
    exit(1);
    traceFile.close();

//    std::ofstream modelFile;
//    std::stringstream modelFileName;
//    modelFileName << GET_PATH_STR ( modelfactory ) << "." << nbTest << ".dot";
//    modelFile.open ( modelFileName.str(), std::ios::out | std::ios::trunc );
//    if ( !modelFile ) {
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

  // *********************************************************************************************
  // Chargement du fmdp servant de base
  // *********************************************************************************************

  gum::FMDPSimulator sim(GET_PATH_STR ( FMDP/coffee/coffee.dat ));

  run ( sim, GET_PATH_STR ( FMDP/trace.Coffee ));
}


// *******************************************************************************
// Run the tests on a Factory FMDP
// *******************************************************************************
//void runFactory(){
//  run ( GET_PATH_STR ( FMDP/factory/factory.dat ), GET_PATH_STR ( FMDP/trace.Factory ));
//}


// *******************************************************************************
// Run the tests on a Coffee FMDP
// *******************************************************************************
void runTaxi(){

  gum::TaxiSimulator sim;

  run ( sim, GET_PATH_STR ( FMDP/trace.Coffee ));
}

// *******************************************************************************
// The main function
// *******************************************************************************
int main ( int argc, char* argv[] ) {

  srand(time(NULL));

//  runCoffee();
//  runFactory();
  runTaxi();

  return EXIT_SUCCESS;
}

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
