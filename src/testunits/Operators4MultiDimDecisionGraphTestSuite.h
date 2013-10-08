
/********************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES *
*   {prenom.nom}_at_lip6.fr           *
*               *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by *
*   the Free Software Foundation; either version 2 of the License, or   *
*   (at your option) any later version.        *
*                *
*   This program is distributed in the hope that it will be useful,   *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of   *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   *
*   GNU General Public License for more details.      *
*                *
*   You should have received a copy of the GNU General Public License  *
*   along with this program; if not, write to the       *
*   Free Software Foundation, Inc.,         *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.    *
*********************************************************************************/

// ============================================================================
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
// =============================================================================
#include <cxxtest/AgrumTestSuite.h>
#include "testsuite_utils.h"
// =============================================================================
#include <agrum/core/approximationPolicy/approximationPolicy.h>
#include <agrum/core/approximationPolicy/linearApproximationPolicy.h>
// =============================================================================
#include <agrum/multidim/potential.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimDecisionGraph.h>
// =============================================================================
#include <agrum/variables/labelizedVariable.h>
// =============================================================================

namespace gum_tests {

  class Operators4MultiDimDecisionGraphTestSuite: public CxxTest::TestSuite {

    private :

      /// Defines the maximal number of modalities for a var (which is choose randomly). The domain size will be of 2 + rand()%maxVarDomainSize.
      static const gum::Idx maxVarDomainSize = 3;

      /// Defines the minimal and maximal number of var in a diagram.
      static const gum::Size minNbVarInDiagram = 7, maxNbVarInDiagram = 10;

      /// Defines how many different operations we perform (from 2 to 5 )
      static const gum::Idx nbOperation = 2;

      /// Defines the number of iteration on randomly generated diagram we perform
      static const int nbIter = 100;


      // Génération fixe d'une liste de variable

      gum::Sequence< const gum::DiscreteVariable* >* __generateFixVarList() {

        gum::Sequence< const gum::DiscreteVariable* >* ret = new gum::Sequence< const gum::DiscreteVariable* >();
        ret->insert ( new gum::LabelizedVariable ( "A", "", 2 ) );
        ret->insert ( new gum::LabelizedVariable ( "B", "", 2 ) );
        ret->insert ( new gum::LabelizedVariable ( "C", "", 2 ) );

        return ret;
      }




      // Génération aléatoire d'une liste de 10 variables

      gum::Sequence< const gum::DiscreteVariable* >* __generateRandomVarList ( int i ) {
        gum::Sequence< const gum::DiscreteVariable* >* ret = new gum::Sequence< const gum::DiscreteVariable* >();

        for ( int j = 0; j < 10; j++ ) {
          std::stringstream varName;
          varName << "var" << j;
          ret->insert ( new gum::LabelizedVariable ( varName.str(), "", 2  + rand() % maxVarDomainSize ) );
        }

        return ret;
      }




      // Brassage aléatoire d'une liste de 10 variables

      void __shuffleVarList ( gum::Sequence< const gum::DiscreteVariable* >* varList, int i ) {
        for ( int j = 0; j < 10; j++ )
          varList->swap ( rand() % ( varList->size() ), rand() % ( varList->size() ) );
      }




      // Génération fixe de diagramme de décision
      gum::MultiDimDecisionGraph<double>* __generateDecisionGraph1 ( const gum::Sequence< const gum::DiscreteVariable* >* varList ) {

        gum::MultiDimDecisionGraph<double>* generatedDecisionGraph = new gum::MultiDimDecisionGraph<double>();

        for ( gum::SequenceIterator< const gum::DiscreteVariable* > varIter = varList->begin(); varIter != varList->end(); ++varIter )
              generatedDecisionGraph->add(**varIter);

        gum::NodeId a = generatedDecisionGraph->manager()->addNonTerminalNode ( varList->atPos ( 0 ) );
        gum::NodeId b = generatedDecisionGraph->manager()->addNonTerminalNode ( varList->atPos ( 1 ) );
        gum::NodeId c = generatedDecisionGraph->manager()->addNonTerminalNode ( varList->atPos ( 2 ) );

        gum::NodeId d = generatedDecisionGraph->manager()->addTerminalNode ( 6 );
        gum::NodeId e = generatedDecisionGraph->manager()->addTerminalNode ( 2 );
        gum::NodeId g = generatedDecisionGraph->manager()->addTerminalNode ( 3 );

        generatedDecisionGraph->manager()->insertArc( a, b, 0 );
        generatedDecisionGraph->manager()->insertArc( a, c, 1 );

        generatedDecisionGraph->manager()->insertArc( b, d, 0 );
        generatedDecisionGraph->manager()->insertArc( b, c, 1 );

        generatedDecisionGraph->manager()->insertArc( c, e, 0 );
        generatedDecisionGraph->manager()->insertArc( c, g, 1 );

        generatedDecisionGraph->manager()->setRootNode(a);

        return generatedDecisionGraph;
      }




      // Génération fixe de diagramme de décision

      gum::MultiDimDecisionGraph<double>* __generateDecisionGraph2 ( const gum::Sequence< const gum::DiscreteVariable* >* varList ) {

        gum::MultiDimDecisionGraph<double>* generatedDecisionGraph = new gum::MultiDimDecisionGraph<double>();

        for ( gum::SequenceIterator< const gum::DiscreteVariable* > varIter = varList->begin(); varIter != varList->end(); ++varIter )
              generatedDecisionGraph->add(**varIter);

        gum::NodeId a = generatedDecisionGraph->manager()->addNonTerminalNode ( varList->atPos ( 0 ) );
        gum::NodeId c = generatedDecisionGraph->manager()->addNonTerminalNode ( varList->atPos ( 2 ) );

        gum::NodeId d = generatedDecisionGraph->manager()->addTerminalNode ( 4 );
        gum::NodeId e = generatedDecisionGraph->manager()->addTerminalNode ( 5 );
        gum::NodeId g = generatedDecisionGraph->manager()->addTerminalNode ( 1 );

        generatedDecisionGraph->manager()->insertArc( a, d, 0 );
        generatedDecisionGraph->manager()->insertArc( a, c, 1 );

        generatedDecisionGraph->manager()->insertArc( c, e, 0 );
        generatedDecisionGraph->manager()->insertArc( c, g, 1 );

        generatedDecisionGraph->manager()->setRootNode(a);

        return generatedDecisionGraph;

      }




      // Génération aléatoire de diagramme de décision

      gum::MultiDimDecisionGraph<double>* __generateRandomDecisionGraph ( const gum::Sequence< const gum::DiscreteVariable* >* varList,
                                                                          double lowLimit = -100,
                                                                          double highLimit = 100 ) {

        srand( time(NULL) );
        gum::MultiDimDecisionGraph<double>* generatedDecisionGraph = new gum::MultiDimDecisionGraph<double>();

        for ( gum::SequenceIterator< const gum::DiscreteVariable* > varIter = varList->begin(); varIter != varList->end(); ++varIter )
              generatedDecisionGraph->add(**varIter);

        while ( ( generatedDecisionGraph->variablesSequence().size() < minNbVarInDiagram ) || ( generatedDecisionGraph->variablesSequence().size() > maxNbVarInDiagram ) ) {

          gum::PriorityQueue< gum::NodeId > fifo;

          generatedDecisionGraph->manager()->setRootNode( generatedDecisionGraph->manager()->addNonTerminalNode( varList->atPos(0) ) );
          fifo.insert(1,generatedDecisionGraph->root());

          while( ! fifo.empty() ){
              gum::NodeId currentId = fifo.pop();
              for( gum::Idx modality = 0; modality < generatedDecisionGraph->node(currentId)->nodeVar->domainSize(); ++modality ){
                  const gum::DiscreteVariable* sonVar = varList->atPos( rand() % ( varList->size() ) );
                  gum::NodeId sonId = generatedDecisionGraph->manager()->addNonTerminalNode( sonVar );
                  fifo.insert(sonId);
                  generatedDecisionGraph->manager()->insertArc( currentId, sonId, modality );
              }
          }
        }
        
        generatedDecisionGraph->reduce();

        return generatedDecisionGraph;
      }




      // Sauvegarde des diagrammes générant une erreur dans un fichier log

      void __saveDiagrams ( gum::MultiDimDecisionGraph<double>* a1, gum::MultiDimDecisionGraph<double>* a2, gum::MultiDimDecisionGraph<double>* a3 ) {

        std::string dotfile = GET_PATH_STR ( DecisionGraphError.log );
        std::ofstream output ( dotfile.c_str(), std::ios::out );

        if ( ! output.good() )
          GUM_ERROR ( gum::IOError, "Stream states flags are not all unset." );

        output << std::endl;

        for ( gum::SequenceIterator< const gum::DiscreteVariable*> ite = a1->variablesSequence().begin(); ite != a1->variablesSequence().end(); ++ite )
          output << ( *ite )->toString() << " - ";

        output << std::endl;

        output << a1->toDot();

        output << std::endl;

        for ( gum::SequenceIterator< const gum::DiscreteVariable*> ite = a2->variablesSequence().begin(); ite != a2->variablesSequence().end(); ++ite )
          output << ( *ite )->toString() << " - ";

        output << std::endl;

        output << a2->toDot();

        if ( a3 != nullptr ) {
          output << std::endl;

          for ( gum::SequenceIterator< const gum::DiscreteVariable*> ite = a3->variablesSequence().begin(); ite != a3->variablesSequence().end(); ++ite )
            output << ( *ite )->toString() << " - ";

          output << std::endl;

          output << a3->toDot();
        }

        output.flush();

        output.close();

        if ( output.fail() )
          GUM_ERROR ( gum::IOError, "Writting in the ostream failed." );
      }




      // Evals  given in parameter operation. Returned boolean parameter indicates if all went well or not

      bool __evalOperation ( gum::Idx operationId, gum::MultiDimDecisionGraph<double>* a1, gum::MultiDimDecisionGraph<double>* a2, double& tempsCalcul, double& tempsEval, double delta = 0.01 ) {

        bool hasNoError = true;
        gum::MultiDimDecisionGraph<double>* a3 = nullptr;

        gum::Timer timy;
        timy.reset();

        // ******************************************************************************************************
        // Generation du diagramme résultat

        switch ( operationId ) {
          case 1 : // Test addition
            TS_GUM_ASSERT_THROWS_NOTHING ( a3 = add2MultiDimDecisionGraphs ( a1 , a2 ) );
            break;

          case 2 : // Test Substraction
            TS_GUM_ASSERT_THROWS_NOTHING ( a3 = subtract2MultiDimDecisionGraphs ( a1 , a2 ) );
            break;

          case 3 : // Test Multiplication
            TS_GUM_ASSERT_THROWS_NOTHING ( a3 = multiply2MultiDimDecisionGraphs ( a1 , a2 ) );
            break;

          case 4 :  // Test Maximum
            TS_GUM_ASSERT_THROWS_NOTHING ( a3 = maximize2MultiDimDecisionGraphs ( a1 , a2 ) );
            break;

          default :
            GUM_ERROR ( gum::OperationNotAllowed, "HEU ....." );
        }

        // ******************************************************************************************************

        tempsCalcul += timy.step();

//  std::cout << a3->toDot() << std::endl;

        timy.reset();

        // ******************************************************************************************************
        if ( a3 != nullptr ) {
          gum::Instantiation inst ( a3 );

          for ( inst.setFirst(); ! inst.end() && hasNoError; ++inst ) {

            switch ( operationId ) {
              case 1 : // Test addition
                TS_ASSERT_DELTA ( a3->get ( inst ), a1->get ( inst ) + a2->get ( inst ), delta );

                if ( a3->get ( inst ) != a1->get ( inst ) + a2->get ( inst ) ) {
                  std::cout << "Instantiation : " << inst.toString() << std::endl;
                  hasNoError = false;
                }

                break;

              case 2 : // Test Substraction
                TS_ASSERT_DELTA ( a3->get ( inst ), a1->get ( inst ) - a2->get ( inst ), delta );

                if ( a3->get ( inst ) != a1->get ( inst ) - a2->get ( inst ) )
                  hasNoError = false;

                break;

              case 3 : // Test Multiplication
                TS_ASSERT_DELTA ( a3->get ( inst ), a1->get ( inst ) * a2->get ( inst ), delta );

                if ( a3->get ( inst ) != a1->get ( inst ) * a2->get ( inst ) )
                  hasNoError = false;

                break;

              case 4 :  // Test Maximum
                TS_ASSERT_DELTA ( a3->get ( inst ), a1->get ( inst ) > a2->get ( inst ) ? a1->get ( inst ) : a2->get ( inst ), delta );

                if ( a3->get ( inst ) != ( a1->get ( inst ) > a2->get ( inst ) ? a1->get ( inst ) : a2->get ( inst ) ) )
                  hasNoError = false;

                break;

              default :
                GUM_ERROR ( gum::OperationNotAllowed, "HEU ....." );
            }
          }

          if ( !hasNoError )
            __saveDiagrams ( a1, a2, a3 );

          delete a3;
        } else {
          __saveDiagrams ( a1, a2, a3 );
          hasNoError = false;
        }

        tempsEval +=  timy.step();

        return hasNoError;

      }

    public :


      //
      // Test sur les fonctions avec valeurs exactes
      //

      void test_Operators_Functions_on_MultiDimDecisionGraphs() {

        gum::Timer time;
        double tempsGene = 0;
        double tempsCalcul = 0;
        double tempsEval = 0;


        // First we try with a predefine structure

        {
//          time.reset();

//          gum::Sequence< const gum::DiscreteVariable* >* varList = __generateFixVarList();

//          gum::MultiDimDecisionGraph<double>* a1 = nullptr;
//          TS_GUM_ASSERT_THROWS_NOTHING ( a1 = __generateDecisionGraph1 ( varList ) );

//          gum::MultiDimDecisionGraph<double>* a2 = nullptr;
//          TS_GUM_ASSERT_THROWS_NOTHING ( a2 = __generateDecisionGraph2 ( varList ) );

////          std::cout << a1->toDot() << std::endl;

////          std::cout << a2->toDot() << std::endl;

//          tempsGene += time.step();

//          bool evalRes = true;

////          for ( gum::Idx opId = 1; opId < nbOperation && evalRes; i++ )
//          evalRes = __evalOperation ( 1, a1, a2, tempsCalcul, tempsEval );

//          delete a1;

//          delete a2;

//          for ( gum::SequenceIterator< const gum::DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
//            delete *ite;

//          delete varList;

//          if ( !evalRes ) {
//            std::cout << "An error has occured! Aborting test." <<std::endl;
//            return;
//          }

//          std::cout << ".";

//          std::fflush ( stdout );
        }


        // Then we try with random structure

        //

        for ( int i = 0; i < nbIter; i++ ) {

          time.reset();

          gum::Sequence< const gum::DiscreteVariable* >* varList = __generateRandomVarList ( i + 1 );
          __shuffleVarList ( varList, i + 2 );

//             std::cout << std::endl << " gum::Sequence variable : ";
//             for( gum::SequenceIterator< const gum::DiscreteVariable* > iter = varList->begin(); iter != varList->end(); ++iter )
//        std::cout << (*iter)->toString() << " | ";
//             std::cout << std::endl;

          gum::MultiDimDecisionGraph<double>* a1 = nullptr;
          TS_GUM_ASSERT_THROWS_NOTHING ( a1 = __generateRandomDecisionGraph ( varList, i + 3 ) );
//             std::cout << a1->toDot();

//             __shuffleVarList ( varList, i + 4 );

//             std::cout << std::endl << " gum::Sequence variable : ";
//             for( gum::SequenceIterator< const gum::DiscreteVariable* > iter = varList->begin(); iter != varList->end(); ++iter )
//        std::cout << (*iter)->toString() << " | ";
//             std::cout << std::endl;

          gum::MultiDimDecisionGraph<double>* a2 = nullptr;
          TS_GUM_ASSERT_THROWS_NOTHING ( a2 = __generateRandomDecisionGraph ( varList, i + 5 ) );
//             std::cout << a2->toDot();

          tempsGene += time.step();

          bool evalRes = true;

          for ( gum::Idx j = 1; j < nbOperation && evalRes; j++ )
            TS_GUM_ASSERT_THROWS_NOTHING ( evalRes = __evalOperation ( j, a1, a2, tempsCalcul, tempsEval ) );

          delete a1;
          delete a2;

          for ( gum::SequenceIterator< const gum::DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
            delete *ite;

          delete varList;

          if ( !evalRes ) {
            std::cout << "An error has occured! Aborting test." << std::endl;
            break;
          }

          test_waiting ( i );
        }

        end_test_waiting();

        //std::cout << std::endl << "Temps Génération : " << tempsGene << "s - Temps Calcul : " << tempsCalcul << "s - Temps Evaluation " << tempsEval << "s " << std::endl;
      }




      //
      // Test sur les opérateurs avec valeurs exactes
      //

      void test_Operators_on_MultiDimDecisionGraphs() {
//        gum::operators4MultiDimInit<double> ();


//        // First we try with a predefine structure


//        gum::MultiDimDecisionGraph<double> a1, a2;
//        gum::MultiDimDecisionGraph<double>* a3 = nullptr;

//        //Test addition
//        TS_GUM_ASSERT_THROWS_NOTHING ( a3 = ( gum::MultiDimDecisionGraph<double>* ) ( a1 + a2 ) );

//        if ( a3 != nullptr )
//          delete a3;

//        //Test subtraction
//        TS_GUM_ASSERT_THROWS_NOTHING ( a3 = ( gum::MultiDimDecisionGraph<double>* ) ( a1 - a2 ) );

//        if ( a3 != nullptr )
//          delete a3;

//        //Test multiplication
//        TS_GUM_ASSERT_THROWS_NOTHING ( a3 = ( gum::MultiDimDecisionGraph<double>* ) ( a1 * a2 ) );

//        if ( a3 != nullptr )
//          delete a3;

//        //Test division
//        TS_GUM_ASSERT_THROWS_NOTHING ( a3 = ( gum::MultiDimDecisionGraph<double>* ) ( a1 / a2 ) );

//        if ( a3 != nullptr )
//          delete a3;
      }


      //
      // Test sur les fonctions avec approximation
      //

      void _Operation_in_MultiDimDecisionGraphs_With_LinearApproximation() {


        // First we try with a predefine structure

//       gum::Sequence< const gum::DiscreteVariable* >* varList = __generateFixVarList();
//
//       gum::MultiDimDecisionGraphFactory<double,LinearApproximationPolicy> f;
//       f.setEpsilon( 0.9 );
//       f.setLowLimit( 0 );
//       f.setHighLimit( 4.5 );
//
//       gum::MultiDimDecisionGraph<double>* a1 = nullptr;
//       TS_GUM_ASSERT_THROWS_NOTHING( a1 = __generateDecisionGraph1(varList, &f ) );
//
//       f.clear();
//       f.setEpsilon( 1.3 );
//       f.setLowLimit( 3.5 );
//       f.setHighLimit( 9 );
//
//       gum::MultiDimDecisionGraph<double>* a2 = nullptr;
//       TS_GUM_ASSERT_THROWS_NOTHING( a2 = __generateDecisionGraph2(varList, &f ) );
//
//       bool evalRes = true;
//
//       for( int i = 1; i < 5 && evalRes; i++)
//    evalRes = __evalOperation( i, a1, a2, 0.9 );
//
//       delete a1;
//       delete a2;
//
//       for( gum::SequenceIterator< const gum::DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
//    delete *ite;
//       delete varList;
//
//       if( !evalRes ){
//    // std::cout << "An error has occured! Aborting test." <<std::endl;
//    return;
//       }
//
//       // std::cout << ".";
//       std::fflush( stdout );


        // Then we try with random structure

        //
//       for(int i = 0; i < 100; i++ ){
//    varList = __generateRandomVarList( i + 1 );
//
//    __shuffleVarList( varList, i + 2 );
//    f.clear();
//    f.setEpsilon( 7 );
//    f.setLowLimit( -42 );
//    f.setHighLimit( 69 );
//
//    TS_GUM_ASSERT_THROWS_NOTHING( a1 = __generateRandomDecisionGraph(varList, i + 3, &f ) );
//
//    __shuffleVarList( varList, i + 4 );
//    f.clear();
//    f.setEpsilon( 11 );
//    f.setLowLimit( -53 );
//    f.setHighLimit( 79 );
//
//    TS_GUM_ASSERT_THROWS_NOTHING( a2 = __generateRandomDecisionGraph(varList, i + 5, &f ) );
//
//    evalRes = true;
//
//    for( int j = 1; j < 5 && evalRes; j++)
//        TS_GUM_ASSERT_THROWS_NOTHING( evalRes = __evalOperation( j, a1, a2, 7 ) );
//
//    delete a1;
//    delete a2;
//
//    for( gum::SequenceIterator< const gum::DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
//        delete *ite;
//    delete varList;
//
//    if( !evalRes ){
//        // std::cout << "An error has occured! Aborting test." <<std::endl;
//        return;
//    }
        /*
              if( i%5 == 0 ) test_waiting(i/5));
        */
//       }
//  end_test_waiting();
      }



      //
      // For debug purpose only, the aim of this function is to reproduce specific diagrams in order to find out where the hell the matter is
      //

      void est_Reproducteur() {
        /*
              gum::LabelizedVariable* v0 = new gum::LabelizedVariable( "0", "", 3 );
              gum::LabelizedVariable* v1 = new gum::LabelizedVariable( "1", "", 3 );
              gum::LabelizedVariable* v2 = new gum::LabelizedVariable( "2", "", 2 );
              gum::LabelizedVariable* v3 = new gum::LabelizedVariable( "3", "", 2 );
              gum::LabelizedVariable* v4 = new gum::LabelizedVariable( "4", "", 2 );
              gum::LabelizedVariable* v5 = new gum::LabelizedVariable( "5", "", 3 );
              gum::LabelizedVariable* v6 = new gum::LabelizedVariable( "6", "", 2 );
              gum::LabelizedVariable* v7 = new gum::LabelizedVariable( "7", "", 2 );
              gum::LabelizedVariable* v8 = new gum::LabelizedVariable( "8", "", 3 );
              gum::LabelizedVariable* v9 = new gum::LabelizedVariable( "9", "", 2 );


              gum::MultiDimDecisionGraphFactory<double> facto;

              gum::Sequence< const gum::DiscreteVariable* > seq;
              seq.insert( v3 );
              seq.insert( v1 );
              seq.insert( v0 );
              seq.insert( v4 );
              seq.insert( v5 );
              seq.insert( v8 );
              seq.insert( v9 );
              seq.insert( v2 );
              seq.insert( v7 );
              seq.insert( v6 );

              // std::cout << std::endl << " gum::Sequence variable : ";
              for( gum::SequenceIterator< const gum::DiscreteVariable* > iter = seq.begin(); iter != seq.end(); ++iter )
              // std::cout << (*iter)->toString() << " - ";
              // std::cout << std::endl;

              facto.setVariablesSequence( seq );

              gum::NodeId n10 = facto.addNonTerminalNode( v0 );
              gum::NodeId n17 = facto.addNonTerminalNode( v7 );
              gum::NodeId n16 = facto.addNonTerminalNode( v6 );
              gum::NodeId n1tm90 = facto.addTerminalNode( -90 );
              gum::NodeId n1t41 = facto.addTerminalNode( 41 );
              gum::NodeId n1tm45 = facto.addTerminalNode( -45 );
              gum::NodeId n1t0 = facto.addTerminalNode( 0 );

              facto.addArc( n10, n17, 0 );
              facto.addArc( n10, n1tm90, 1 );
              facto.insertDefaultArc( n10, n1t0 );

              facto.addArc( n17, n16, 0 );
              facto.insertDefaultArc( n17, n1t0 );

              facto.addArc( n16, n1t41, 0 );
              facto.addArc( n16, n1tm45, 1 );

              gum::MultiDimDecisionGraph<double>* a1 = facto.getMultiDimDecisionGraph();
              // std::cout << std::endl << a1->toDot();

              facto.clear();
              seq.clear();

              seq.insert( v6 );
              seq.insert( v0 );
              seq.insert( v1 );
              seq.insert( v2 );
              seq.insert( v5 );
              seq.insert( v7 );
              seq.insert( v8 );
              seq.insert( v3 );
              seq.insert( v9 );
              seq.insert( v4 );

              // std::cout << std::endl << " gum::Sequence variable : ";
              for( gum::SequenceIterator< const gum::DiscreteVariable* > iter = seq.begin(); iter != seq.end(); ++iter )
              // std::cout << (*iter)->toString() << " - ";
              // std::cout << std::endl;

              facto.setVariablesSequence( seq );

              gum::NodeId n21 = facto.addNonTerminalNode( v1 );
              gum::NodeId n241 = facto.addNonTerminalNode( v4 );
              gum::NodeId n242 = facto.addNonTerminalNode( v4 );
              gum::NodeId n29 = facto.addNonTerminalNode( v9 );
              gum::NodeId n2t0 = facto.addTerminalNode( 0 );
              gum::NodeId n2t16 = facto.addTerminalNode( 16 );
              gum::NodeId n2tm99 = facto.addTerminalNode( -99 );
              gum::NodeId n2tm3 = facto.addTerminalNode( -3 );

              facto.addArc( n21, n241, 0 );
              facto.addArc( n21, n29, 1 );
              facto.addArc( n21, n2t16, 2 );

              facto.addArc( n241, n2tm99, 0 );
              facto.addArc( n241, n2tm3, 1 );

              facto.addArc( n29, n242, 0 );
              facto.insertDefaultArc( n29, n2t0 );

              facto.addArc( n242, n2tm3, 0 );
              facto.insertDefaultArc( n242, n2t0 );

              gum::MultiDimDecisionGraph<double>* a2 = facto.getMultiDimDecisionGraph();
              // std::cout << std::endl << a2->toDot() << std::endl;

              bool evalRes = true;
              double fuck = 0.0, you = 0.0;
              TS_GUM_ASSERT_THROWS_NOTHING( evalRes = __evalOperation( 1, a1, a2,fuck,you,0.0 ) );

              delete a1;
              delete a2;

              delete  v0;
              delete  v1;
              delete  v2;
              delete  v3;
              delete  v4;
              delete  v5;
              delete  v6;
              delete  v7;
              delete  v8;
              delete  v9;*/
      }



  };
}
