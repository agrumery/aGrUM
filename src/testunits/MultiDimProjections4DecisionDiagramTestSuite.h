
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

// *********************************************************************
// #include <cstdlib>
// #include <iostream>
// #include <sstream>
#include <fstream>
// #include <string>
// #include <sstream>
// #include <time.h>
// *********************************************************************
#include <cxxtest/AgrumTestSuite.h>
#include "testsuite_utils.h"
// *********************************************************************
#include <agrum/config.h>
#include <agrum/core/timer.h>
// *********************************************************************
#include <agrum/multidim/potential.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/multiDimDecisionDiagram.h>
#include <agrum/multidim/multiDimDecisionDiagramFactory.h>
// *********************************************************************


namespace gum_tests {

  class MultiDimProjections4DecisionDiagramTestSuite: public CxxTest::TestSuite {

    private :

      // ************************************************************************************************
      // Génération fixe d'une liste de variable
      // ************************************************************************************************
      gum::Sequence< const gum::DiscreteVariable* >* __generateFixVarList() {

        gum::Sequence< const gum::DiscreteVariable* >* ret  = new gum::Sequence< const gum::DiscreteVariable* >();
        ret->insert ( new gum::LabelizedVariable ( "A", "", 2 ) );
        ret->insert ( new gum::LabelizedVariable ( "B", "", 2 ) );
        ret->insert ( new gum::LabelizedVariable ( "C", "", 2 ) );
        ret->insert ( new gum::LabelizedVariable ( "D", "", 2 ) );

        return ret;

      }

      // ************************************************************************************************

      // ************************************************************************************************
      // Génération aléatoire d'une liste de 10 variables
      // ************************************************************************************************
      gum::Sequence< const gum::DiscreteVariable* >* __generateRandomVarList ( int i ) {

        srand ( time ( NULL ) + i );

        gum::Sequence< const gum::DiscreteVariable* >* ret  = new gum::Sequence< const gum::DiscreteVariable* >();

        for ( int j  = 0; j < 10; j++ ) {
          std::stringstream varName;
          varName << "var" << j;
          ret->insert ( new gum::LabelizedVariable ( varName.str(), "", 2 + rand() %2 ) );
        }

        return ret;
      }

      // ************************************************************************************************

      // ************************************************************************************************
      // Brassage aléatoire d'une liste de 10 variables
      // ************************************************************************************************
      void __shuffleVarList ( gum::Sequence< const gum::DiscreteVariable* >* varList, int i ) {

        srand ( time ( NULL ) + i );

        for ( int j = 0; j < 10; j++ )
          varList->swap ( rand() % ( varList->size() ), rand() % ( varList->size() ) );
      }

      // ************************************************************************************************

      // ************************************************************************************************
      // Génération fixe de diagramme de décision
      // ************************************************************************************************
      gum::MultiDimDecisionDiagramBase<double>* __generateDecisionDiagram1 ( const gum::Sequence< const gum::DiscreteVariable* >* varList ) {

        gum::MultiDimDecisionDiagramFactory<double> facto;
        facto.setVariablesSequence ( *varList );
        gum::NodeId a  = facto.addNonTerminalNode ( varList->atPos ( 0 ) );
        gum::NodeId b  = facto.addNonTerminalNode ( varList->atPos ( 1 ) );
        gum::NodeId c  = facto.addNonTerminalNode ( varList->atPos ( 2 ) );

        gum::NodeId d  = facto.addTerminalNode ( 6 );
        gum::NodeId e  = facto.addTerminalNode ( 2 );
        gum::NodeId g  = facto.addTerminalNode ( 3 );

        facto.insertArc ( a, b, 0 );
        facto.insertArc ( a, c, 1 );

        facto.insertArc ( b, d, 0 );
        facto.insertArc ( b, c, 1 );

        facto.insertArc ( c, e, 0 );
        facto.insertArc ( c, g, 1 );

        return facto.getMultiDimDecisionDiagram ( false );

      }

      // ************************************************************************************************

      // ************************************************************************************************
      // Génération aléatoire de diagramme de décision
      // ************************************************************************************************
      gum::MultiDimDecisionDiagramBase<double>* __generateRandomdoubleDecisionDiagram (
        const gum::Sequence< const gum::DiscreteVariable* >* varList,
        int i,
        gum::MultiDimDecisionDiagramFactoryBase<double>* f  = NULL ) {

        srand ( time ( NULL ) + i );
        gum::MultiDimDecisionDiagramBase<double>* ret  = NULL;
        bool factoryCreatedHere  = false;

        while ( ret == NULL || ( ret->diagramVarSize() < 7 ) || ( ret->diagramVarSize() > 10 ) ) {

          if ( ret != NULL )
            delete ret;

          if ( f == NULL ) {
            factoryCreatedHere  = true;
            f  = new gum::MultiDimDecisionDiagramFactory<double>();
            f->setLowLimit ( -100 );
            f->setHighLimit ( 100 );
          }

          f->clear();

          f->setVariablesSequence ( *varList );

          // ***********************************************************************
          // Creation of table var - list of associated node id in the diagram
          // with initialization of those lists
          gum::HashTable< const gum::DiscreteVariable*, gum::List<gum::NodeId>* > var2NodeIdMap;

          for ( gum::Size numVar  = 0; numVar < ( *varList ).size(); ++numVar )
            var2NodeIdMap.insert ( ( *varList ) [ numVar ], new gum::List<gum::NodeId>() );

          // ***********************************************************************
          // Creation of a list containing terminal node possible value
          gum::List<double> tnList;

          double interval  = f->highLimit() - f->lowLimit();

          gum::Idx i  = 0;

          while ( i < 25 ) {
            double newVal  = ( rand() % ( ( int ) interval ) ) - interval/2;

            if ( newVal <= f->highLimit() && newVal >= f->lowLimit() ) {
              tnList.insert ( newVal );
              i++;
            }
          }

          // ***********************************************************************
          // Selection (randomly) of the root var in the diagram
          // all var that are before this one won't be in the diagram
          // for order on var purpose
          gum::Size initVar  = rand() % ( varList->size() /2 );

          // ***********************************************************************
          // So for all remaining variable
          for ( gum::Size numVar  = initVar; numVar < varList->size(); ++numVar ) {

            // if this is the root we add it
            if ( numVar == initVar )
              var2NodeIdMap[ ( *varList ) [ initVar ] ]->insert ( f->unsafeAddNonTerminalNode ( ( *varList ) [ initVar ] ) );

            // then we check if another variable goes on it
            //(or if this is the root). It's done implicitly by the size of the list of node associated to that var.
            // if not we move on
            // else
            if ( !var2NodeIdMap[ ( *varList ) [ numVar ] ]->empty() ) {
              // for each node associated to that var
              for ( gum::ListIterator<gum::NodeId> numNode  = var2NodeIdMap[ ( *varList ) [ numVar ] ]->begin(); numNode != var2NodeIdMap[ ( *varList ) [ numVar ] ]->end();  ++numNode ) {

                // we determine a random number of arc starting from that node
                gum::Idx nbArc  = 1 + rand() % ( ( *varList ) [ numVar ]->domainSize() );

                // for those arcs

                for ( gum::Idx label  = 0; label < nbArc; label++ ) {

                  // We first determine if it goes directly to a terminal node or node
                  if ( numVar != ( varList->size() - 1 ) && ( rand() %100 ) >= 42 ) {

                    // if not, we determine randomly to which var it could go
                    gum::NodeId indToVar  = ( rand() % ( varList->size() - 1 - numVar ) ) + numVar + 1;
                    const gum::DiscreteVariable* toVar  = ( *varList ) [ indToVar ];

                    // then we take the list of  node id associated to that var and
                    // we determine randomly if we add another node to that list or not
                    gum::NodeId desiredNode  = ( gum::NodeId ) rand() %5;

                    if ( desiredNode  >= var2NodeIdMap[ toVar ]->size() ) {
                      var2NodeIdMap[ toVar ]->insert ( f->unsafeAddNonTerminalNode ( toVar ) );
                      desiredNode  = var2NodeIdMap[ toVar ]->size() - 1;
                    }

                    // then we add an arc between our current var associated node id and the considered var random node id
                    f->unsafeInsertArc ( *numNode, ( *var2NodeIdMap[ toVar ] ) [desiredNode], label );

                  } else {

                    // if we add
                    gum::NodeId toVal  = f->addTerminalNode ( tnList[ rand() %tnList.size()] );
                    f->unsafeInsertArc ( *numNode, toVal, label );

                  }
                }
              }
            }
          }

          for ( gum::HashTableIterator< const gum::DiscreteVariable*, gum::List<gum::NodeId>* > ht  = var2NodeIdMap.begin(); ht != var2NodeIdMap.end(); ++ht )
            delete *ht;

          ret  = f->getMultiDimDecisionDiagram();
        }

        if ( factoryCreatedHere )
          delete f;

        return ret;
      }

      // ************************************************************************************************

      // ************************************************************************************************
      // Sauvegarde des diagrammes générant une erreur dans un fichier log
      // ************************************************************************************************
      void __saveDiagrams ( gum::MultiDimDecisionDiagramBase<double>* a1, gum::MultiDimDecisionDiagramBase<double>* a3, gum::Set<const gum::DiscreteVariable*> delVars ) {

        std::string dotfile = GET_PATH_STR ( DecisionDiagramError.log );
        std::ofstream output ( dotfile.c_str(), std::ios::out );

        if ( ! output.good() )
          GUM_ERROR ( gum::IOError, "Stream states flags are not all unset." );

        output << std::endl;

        for ( gum::SequenceIterator< const gum::DiscreteVariable*> ite = a1->variablesSequence().begin(); ite != a1->variablesSequence().end(); ++ite )
          output << ( *ite )->toString() << " - ";

        output << std::endl;

        output << a1->toDot();

        if ( a3 != NULL ) {
          output << std::endl;

          for ( gum::SequenceIterator< const gum::DiscreteVariable*> ite = a3->variablesSequence().begin(); ite != a3->variablesSequence().end(); ++ite )
            output << ( *ite )->toString() << " - ";

          output << std::endl;

          output << a3->toDot();
        }

        output << std::endl;

        for ( gum::SetIterator< const gum::DiscreteVariable*> ite = delVars.begin(); ite != delVars.end(); ++ite )
          output << ( *ite )->toString() << " - ";

        output << std::endl;

        output.flush();

        output.close();

        if ( output.fail() )
          GUM_ERROR ( gum::IOError, "Writting in the ostream failed." );
      }

      // ================================================================================================

      // ================================================================================================
      // Evals  given in parameter operation. Returned boolean parameter indicates if all went well or not
      // ================================================================================================
      bool __evalOperation ( int operationId, gum::MultiDimDecisionDiagramBase<double>* a1, gum::Set< const gum::DiscreteVariable* > del_vars, double& tempsCalcul, double& tempsEval, double delta = 0.01 ) {

        bool hasNoError = true;
        gum::MultiDimDecisionDiagramBase<double>* a3 = NULL;

        gum::Timer timy;
        timy.reset();

        // ******************************************************************************************************
        // Generation du diagramme résultat

        switch ( operationId ) {
          case 1 : // Test Addition
            TS_GUM_ASSERT_THROWS_NOTHING ( a3 = projectSumMultiDimDecisionDiagram ( a1, del_vars ) );
            break;
          case 2 : // Test Multiplication
            TS_GUM_ASSERT_THROWS_NOTHING ( a3 = projectProductMultiDimDecisionDiagram ( a1, del_vars ) );
            break;
          case 3 : // Test Minimisation
            TS_GUM_ASSERT_THROWS_NOTHING ( a3 = projectMinMultiDimDecisionDiagram ( a1, del_vars ) );
            break;
          case 4 :  // Test Maximisation
            TS_GUM_ASSERT_THROWS_NOTHING ( a3 = projectMaxMultiDimDecisionDiagram ( a1, del_vars ) );
            break;
          default :
            GUM_ERROR ( gum::OperationNotAllowed, "HEU ....." );
        }

        // ******************************************************************************************************

        tempsCalcul += timy.step();

        timy.reset();

        // ******************************************************************************************************
        if ( a3 != NULL ) {

          gum::Instantiation instEleminatedVar ( a1 );
          gum::Instantiation instRemainingVar ( a3 );

          for ( instRemainingVar.setFirst(); ! instRemainingVar.end() && hasNoError; ++instRemainingVar ) {

            switch ( operationId ) {

              case 1 : {// Test Addition
                  double sum  = 0;

                  for ( instEleminatedVar.chgValIn ( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut ( instRemainingVar ) )
                    sum += a1->get ( instEleminatedVar );

                  TS_ASSERT_DELTA ( a3->get ( instRemainingVar ), sum, delta );

                  if ( a3->get ( instRemainingVar ) != sum )
                    hasNoError = false;
                }

                break;

              case 2 : {// Test Multiplication
                  double product  = 1;

                  for ( instEleminatedVar.chgValIn ( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut ( instRemainingVar ) )
                    product *= a1->get ( instEleminatedVar );

                  TS_ASSERT_DELTA ( a3->get ( instRemainingVar ), product, delta );

                  if ( a3->get ( instRemainingVar ) != product )
                    hasNoError = false;
                }

                break;

              case 3 : {// Test Minimisation
                  double min  = std::numeric_limits<double>::max();

                  for ( instEleminatedVar.chgValIn ( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut ( instRemainingVar ) )
                    min  = min >= a1->get ( instEleminatedVar ) ? a1->get ( instEleminatedVar ) : min;

                  TS_ASSERT_DELTA ( a3->get ( instRemainingVar ), min, delta );

                  if ( a3->get ( instRemainingVar ) != min )
                    hasNoError = false;
                }

                break;

              case 4 :  {// Test Maximisation
                  double max  = -1  * ( std::numeric_limits<double>::max() );

                  for ( instEleminatedVar.chgValIn ( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut ( instRemainingVar ) )
                    max  = max >= a1->get ( instEleminatedVar ) ? max : a1->get ( instEleminatedVar );

                  TS_ASSERT_DELTA ( a3->get ( instRemainingVar ), max, delta );

                  if ( ( double ) a3->get ( instRemainingVar ) >= max + delta || ( double ) a3->get ( instRemainingVar ) <= max - delta )
                    hasNoError = false;
                }

                break;

              default :
                GUM_ERROR ( gum::OperationNotAllowed, "HEU ....." );
            }
          }

          if ( !hasNoError )
            __saveDiagrams ( a1, a3, del_vars );

          delete a3;
        } else {
          __saveDiagrams ( a1, a3, del_vars );
          hasNoError = false;
        }

        tempsEval +=  timy.step();

        return hasNoError;

      }



    public :

      /* ***************************************************************************************************************************** **/
      /*                                                                                                                                                                                             **/
      /*         Test sur les fonctions avec valeurs exactes                                          **/
      /*                                                                                                                                                                                            **/
      /* **************************************************************************************************************************** **/
      void test_Projections_Functions_on_MultiDimDecisionDiagrams() {

        gum::Timer time;
        double tempsGene = 0;
        double tempsCalcul = 0;
        double tempsEval = 0;

        // *************************************************************************************
        // First we try with a predefine structure
        // *************************************************************************************
        time.reset();

        gum::Sequence< const gum::DiscreteVariable* >* varList = __generateFixVarList();

        gum::MultiDimDecisionDiagramBase<double>* a1  = NULL;
        TS_GUM_ASSERT_THROWS_NOTHING ( a1 = __generateDecisionDiagram1 ( varList ) );

        gum::Set< const gum::DiscreteVariable* > del_vars;
        del_vars << varList->atPos ( 0 );
        del_vars << varList->atPos ( 1 );

        tempsGene += time.step();

        bool evalRes = true;

        for ( int operationId = 1; operationId < 5 && evalRes; operationId++ )
          evalRes = __evalOperation ( operationId, a1, del_vars, tempsCalcul, tempsEval );

        delete a1;

        del_vars.clear();

        for ( gum::SequenceIterator< const gum::DiscreteVariable*> ite  = varList->begin(); ite != varList->end(); ++ite )
          delete *ite;

        delete varList;

        if ( !evalRes )
          return;

        // *************************************************************************************
        // Then we try with random structure
        // *************************************************************************************
        for ( int nbLoop  = 0; nbLoop < 100; nbLoop++ ) {

          time.reset();

          varList  = __generateRandomVarList ( nbLoop + 1 );

          TS_GUM_ASSERT_THROWS_NOTHING ( a1  = __generateRandomdoubleDecisionDiagram ( varList, nbLoop + 2 ) );

          gum::Idx maxSwapVar  = rand() %varList->size();

          for ( gum::Idx i  = 0; i < maxSwapVar; ++i )
            del_vars << varList->atPos ( rand() %varList->size() );

          tempsGene += time.step();

          evalRes = true;

          TS_GUM_ASSERT_THROWS_NOTHING ( evalRes = __evalOperation ( 1, a1, del_vars, tempsCalcul, tempsEval ) );

          TS_GUM_ASSERT_THROWS_NOTHING ( evalRes = __evalOperation ( 3, a1, del_vars, tempsCalcul, tempsEval ) );

          TS_GUM_ASSERT_THROWS_NOTHING ( evalRes = __evalOperation ( 4, a1, del_vars, tempsCalcul, tempsEval ) );

          delete a1;

          del_vars.clear();

          for ( gum::SequenceIterator< const gum::DiscreteVariable*> ite  = varList->begin(); ite != varList->end(); ++ite )
            delete *ite;

          delete varList;

          if ( !evalRes ) {
            std::cout << "An error has occured! Aborting test." <<std::endl;
            return;
          }

          if ( nbLoop%25 == 0 )
            std::cout << "#";
          else if ( nbLoop%5 == 0 )
            std::cout << ".";

          std::fflush ( stdout );
        }

//       std::cout << std::endl << "Temps Génération : " << tempsGene << "s - Temps Calcul : " << tempsCalcul << "s - Temps Evaluation " << tempsEval << "s ";
      }

      // ************************************************************************************************


      /* ***************************************************************************************************************************** **/
      /*                                                                                                                                                                                             **/
      /*         Test sur les fonctions avec approximation                                          **/
      /*                                                                                                                                                                                            **/
      /* **************************************************************************************************************************** **/
      void _Operation_in_MultiDimDecisionDiagrams_With_LinearApproximation() {

        // *************************************************************************************
        // First we try with a predefine structure
        // *************************************************************************************
//       gum::Sequence< const gum::DiscreteVariable* >* varList  = __generateFixVarList();
//
//
//       gum::MultiDimDecisionDiagramFactory<double,LinearApproximationPolicy> f;
//       f.setEpsilon( 0.9 );
//       f.setLowLimit( 0 );
//       f.setHighLimit( 4.5 );
//       f.setVariablesSequence( *varList );
//       gum::NodeId a  = f.addNonTerminalNode( *( varList->atPos( 0 ) ) );
//       gum::NodeId b  = f.addNonTerminalNode( *( varList->atPos( 1 ) ) );
//       gum::NodeId c  = f.addNonTerminalNode( *( varList->atPos( 2 ) ) );
//
//       gum::NodeId d  = f.addTerminalNode( 1 );
//       gum::NodeId e  = f.addTerminalNode( 2 );
//       gum::NodeId g  = f.addTerminalNode( 3 );
//
//       f.insertArc( a, b, 0 );
//       f.insertArc( a, c, 1 );
//
//       f.insertArc( b, d, 0 );
//       f.insertArc( b, c, 1 );
//
//       f.insertArc( c, e, 0 );
//       f.insertArc( c, g, 1 );
//
//       gum::MultiDimDecisionDiagramBase<double>* a1  = f.getMultiDimDecisionDiagram( false );
//
//       gum::Set< const gum::DiscreteVariable* > del_vars;
//       del_vars << varList->atPos( 0 );
//       del_vars << varList->atPos( 1 );
//
//       bool evalRes = true;
//
//       for( int operationId = 1; operationId < 5 && evalRes; operationId++)
//    evalRes = __evalOperation( operationId, a1, del_vars, 0.9 );
//
//       delete a1;
//       del_vars.clear();
//
//       for ( gum::SequenceIterator< const gum::DiscreteVariable*> ite  = varList->begin(); ite != varList->end(); ++ite )
//    delete *ite;
//       delete varList;
//
//       if( !evalRes )
//    return;

        // *************************************************************************************
        // Then we try with random structure
        // *************************************************************************************
//       for(int nbLoop  = 0; nbLoop < 10; nbLoop++ ){
//
//       f.clear();
//       f.setEpsilon( 7 );
//       f.setLowLimit( -42 );
//       f.setHighLimit( 69 );
//
//       varList  = generateRandomVarList( nbLoop + 1 );
//
//       TS_GUM_ASSERT_THROWS_NOTHING( a1  = generateRandomdoubleDecisionDiagram( varList, nbLoop + 2, &f ) );
//
//    gum::Idx maxSwapVar  = rand()%varList->size();
//    for( gum::Idx i  = 0; i < maxSwapVar; ++i )
//        del_vars << varList->atPos( rand()%varList->size() );
//
//    evalRes = true;
//
//    for( int operationId = 1; operationId < 5 && evalRes; operationId++)
//        TS_GUM_ASSERT_THROWS_NOTHING( evalRes = __evalOperation( operationId, a1, del_vars ) );
//
//    delete a1;
//    del_vars.clear();
//
//    for( gum::SequenceIterator< const gum::DiscreteVariable*> ite  = varList->begin(); ite != varList->end(); ++ite )
//        delete *ite;
//    delete varList;
//
//    if( !evalRes ){
//        std::cout << "An error has occured! Aborting test." <<std::endl;
//        return;
//    }
//
//    if( nbLoop%25 == 0 )
//        std::cout << "#";
//    else if( nbLoop%5 == 0 )
//        std::cout << ".";
//    std::fflush( stdout );
//       }
      }





//   ************************************************************************************************
//   Fonction de reproduction des tests amenant à des bugs/erreurs (rayer les mentions inutiles)
//   pour débogage
//   ************************************************************************************************
      void _reproduction() {

//       gum::Sequence< const gum::DiscreteVariable* >* varList  = new gum::Sequence< const gum::DiscreteVariable* >();
//
//       varList->insert( new gum::LabelizedVariable( "0", "", 3 ) );
//       varList->insert( new gum::LabelizedVariable( "1", "", 3 ) );
//       varList->insert( new gum::LabelizedVariable( "2", "", 2 ) );
//       varList->insert( new gum::LabelizedVariable( "3", "", 2 ) );
//       varList->insert( new gum::LabelizedVariable( "4", "", 3 ) );
//       varList->insert( new gum::LabelizedVariable( "5", "", 3 ) );
//       varList->insert( new gum::LabelizedVariable( "6", "", 3 ) );
//       varList->insert( new gum::LabelizedVariable( "7", "", 2 ) );
//       varList->insert( new gum::LabelizedVariable( "8", "", 3 ) );
//       varList->insert( new gum::LabelizedVariable( "9", "", 2 ) );
//
//       gum::MultiDimDecisionDiagramFactory<double> facto;
//       facto.setVariablesSequence( *varList );
//
//       gum::NodeId nv1  = facto.addNonTerminalNode( varList->atPos( 1 ) );
//       gum::NodeId nt0  = facto.addTerminalNode( 0 );
//
//       facto.insertArc( nv1, nv3, 0 );
//       facto.insertDefaultArc( nv1, nt0 );
//
//
//       gum::MultiDimDecisionDiagramBase<double>* a1  = NULL;
//       TS_GUM_ASSERT_THROWS_NOTHING( a1  = facto.getMultiDimDecisionDiagram() );
//
//       gum::Set< const gum::DiscreteVariable* > del_vars;
//       del_vars << varList->atPos( 3 );
//
//       TS_GUM_ASSERT_THROWS_NOTHING( __evalOperation( 1, a1, del_vars ) );

//       delete a1;
//       del_vars.clear();
//
//       for( gum::SequenceIterator< const gum::DiscreteVariable*> ite  = varList->begin(); ite != varList->end(); ++ite )
//    delete *ite;
//       delete varList;
      }

  };
}

