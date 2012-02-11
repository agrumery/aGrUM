
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
      Sequence< const DiscreteVariable* >* __generateFixVarList() {

        Sequence< const DiscreteVariable* >* ret  = new Sequence< const DiscreteVariable* >();
        ret->insert( new LabelizedVariable( "A", "", 2 ) );
        ret->insert( new LabelizedVariable( "B", "", 2 ) );
        ret->insert( new LabelizedVariable( "C", "", 2 ) );
        ret->insert( new LabelizedVariable( "D", "", 2 ) );

        return ret;

      }

      // ************************************************************************************************

      // ************************************************************************************************
      // Génération aléatoire d'une liste de 10 variables
      // ************************************************************************************************
      Sequence< const DiscreteVariable* >* __generateRandomVarList( int i ) {

        srand( time( NULL ) + i );

        Sequence< const DiscreteVariable* >* ret  = new Sequence< const DiscreteVariable* >();

        for( int j  = 0; j < 10; j++ ) {
          std::stringstream varName;
          varName << "var" << j;
          ret->insert( new LabelizedVariable( varName.str(), "", 2 + rand()%2 ) );
        }

        return ret;
      }

      // ************************************************************************************************

      // ************************************************************************************************
      // Brassage aléatoire d'une liste de 10 variables
      // ************************************************************************************************
      void __shuffleVarList( Sequence< const DiscreteVariable* >* varList, int i ) {

        srand( time( NULL ) + i );

        for( int j = 0; j < 10; j++ )
          varList->swap( rand()%( varList->size() ), rand()%( varList->size() ) );
      }

      // ************************************************************************************************

      // ************************************************************************************************
      // Génération fixe de diagramme de décision
      // ************************************************************************************************
      MultiDimDecisionDiagramBase<double>* __generateDecisionDiagram1( const Sequence< const DiscreteVariable* >* varList ) {

        MultiDimDecisionDiagramFactory<double> facto;
        facto.setVariablesSequence( *varList );
        NodeId a  = facto.addNonTerminalNode( varList->atPos( 0 ) );
        NodeId b  = facto.addNonTerminalNode( varList->atPos( 1 ) );
        NodeId c  = facto.addNonTerminalNode( varList->atPos( 2 ) );

        NodeId d  = facto.addTerminalNode( 6 );
        NodeId e  = facto.addTerminalNode( 2 );
        NodeId g  = facto.addTerminalNode( 3 );

        facto.insertArc( a, b, 0 );
        facto.insertArc( a, c, 1 );

        facto.insertArc( b, d, 0 );
        facto.insertArc( b, c, 1 );

        facto.insertArc( c, e, 0 );
        facto.insertArc( c, g, 1 );

        return facto.getMultiDimDecisionDiagram( false );

      }

      // ************************************************************************************************

      // ************************************************************************************************
      // Génération aléatoire de diagramme de décision
      // ************************************************************************************************
      MultiDimDecisionDiagramBase<double>* __generateRandomdoubleDecisionDiagram( const Sequence< const DiscreteVariable* >* varList, int i, MultiDimDecisionDiagramFactoryBase<double>* f  = NULL ) {

        srand( time( NULL ) + i );
        MultiDimDecisionDiagramBase<double>* ret  = NULL;
        bool factoryCreatedHere  = false;

        while( ret == NULL || ( ret->diagramVarSize() < 7 ) || ( ret->diagramVarSize() > 10 ) ) {

          if( ret != NULL )
            delete ret;

          if( f == NULL ) {
            factoryCreatedHere  = true;
            f  = new MultiDimDecisionDiagramFactory<double>();
            f->setLowLimit( -100 );
            f->setHighLimit( 100 );
          }

          f->clear();

          f->setVariablesSequence( *varList );

          // ***********************************************************************
          // Creation of table var - list of associated node id in the diagram
          // with initialization of those lists
          HashTable< const DiscreteVariable*, List<NodeId>* > var2NodeIdMap;

          for( Size numVar  = 0; numVar < ( *varList ).size(); ++numVar )
            var2NodeIdMap.insert( ( *varList )[ numVar ], new List<NodeId>() );

          // ***********************************************************************
          // Creation of a list containing terminal node possible value
          List<double> tnList;

          double interval  = f->highLimit() - f->lowLimit();

          Idx i  = 0;

          while( i < 25 ) {
            double newVal  = ( rand()% ( ( int ) interval ) ) - interval/2;

            if( newVal <= f->highLimit() && newVal >= f->lowLimit() ) {
              tnList.insert( newVal );
              i++;
            }
          }

          // ***********************************************************************
          // Selection (randomly) of the root var in the diagram
          // all var that are before this one won't be in the diagram
          // for order on var purpose
          Size initVar  = rand()%( varList->size()/2 );

          // ***********************************************************************
          // So for all remaining variable
          for( Size numVar  = initVar; numVar < varList->size(); ++numVar ) {

            // if this is the root we add it
            if( numVar == initVar )
              var2NodeIdMap[( *varList )[ initVar ] ]->insert( f->unsafeAddNonTerminalNode( ( *varList )[ initVar ] ) );

            // then we check if another variable goes on it
            //(or if this is the root). It's done implicitly by the size of the list of node associated to that var.
            // if not we move on
            // else
            if( !var2NodeIdMap[( *varList )[ numVar ] ]->empty() ) {
              // for each node associated to that var
              for( ListIterator<NodeId> numNode  = var2NodeIdMap[( *varList )[ numVar ] ]->begin(); numNode != var2NodeIdMap[( *varList )[ numVar ] ]->end();  ++numNode ) {

                // we determine a random number of arc starting from that node
                Idx nbArc  = 1 + rand()%( ( *varList )[ numVar ]->domainSize() );

                // for those arcs

                for( Idx label  = 0; label < nbArc; label++ ) {

                  // We first determine if it goes directly to a terminal node or node
                  if( numVar != ( varList->size() - 1 ) && ( rand()%100 ) >= 42 ) {

                    // if not, we determine randomly to which var it could go
                    NodeId indToVar  = ( rand()%( varList->size() - 1 - numVar ) ) + numVar + 1;
                    const DiscreteVariable* toVar  = ( *varList )[ indToVar ];

                    // then we take the list of  node id associated to that var and
                    // we determine randomly if we add another node to that list or not
                    NodeId desiredNode  = ( NodeId ) rand()%5;

                    if( desiredNode  >= var2NodeIdMap[ toVar ]->size() ) {
                      var2NodeIdMap[ toVar ]->insert( f->unsafeAddNonTerminalNode( toVar ) );
                      desiredNode  = var2NodeIdMap[ toVar ]->size() - 1;
                    }

                    // then we add an arc between our current var associated node id and the considered var random node id
                    f->unsafeInsertArc( *numNode, ( *var2NodeIdMap[ toVar ] )[desiredNode], label );

                  } else {

                    // if we add
                    NodeId toVal  = f->addTerminalNode( tnList[ rand()%tnList.size()] );
                    f->unsafeInsertArc( *numNode, toVal, label );

                  }
                }
              }
            }
          }

          for( HashTableIterator< const DiscreteVariable*, List<NodeId>* > ht  = var2NodeIdMap.begin(); ht != var2NodeIdMap.end(); ++ht )
            delete *ht;

          ret  = f->getMultiDimDecisionDiagram();
        }

        if( factoryCreatedHere )
          delete f;

        return ret;
      }

      // ************************************************************************************************

      // ************************************************************************************************
      // Sauvegarde des diagrammes générant une erreur dans un fichier log
      // ************************************************************************************************
      void __saveDiagrams( MultiDimDecisionDiagramBase<double>* a1, MultiDimDecisionDiagramBase<double>* a3, Set<const DiscreteVariable*> delVars ) {

        std::string dotfile = GET_PATH_STR( DecisionDiagramError.log );
        std::ofstream output( dotfile.c_str(), std::ios::out );

        if( ! output.good() )
          GUM_ERROR( IOError, "Stream states flags are not all unset." );

        output << std::endl;

        for( SequenceIterator< const DiscreteVariable*> ite = a1->variablesSequence().begin(); ite != a1->variablesSequence().end(); ++ite )
          output << ( *ite )->toString() << " - ";

        output << std::endl;

        output << a1->toDot();

        if( a3 != NULL ) {
          output << std::endl;

          for( SequenceIterator< const DiscreteVariable*> ite = a3->variablesSequence().begin(); ite != a3->variablesSequence().end(); ++ite )
            output << ( *ite )->toString() << " - ";

          output << std::endl;

          output << a3->toDot();
        }

        output << std::endl;

        for( SetIterator< const DiscreteVariable*> ite = delVars.begin(); ite != delVars.end(); ++ite )
          output << ( *ite )->toString() << " - ";

        output << std::endl;

        output.flush();

        output.close();

        if( output.fail() )
          GUM_ERROR( IOError, "Writting in the ostream failed." );
      }

      // ================================================================================================

      // ================================================================================================
      // Evals  given in parameter operation. Returned boolean parameter indicates if all went well or not
      // ================================================================================================
      bool __evalOperation( int operationId, MultiDimDecisionDiagramBase<double>* a1, Set< const DiscreteVariable* > del_vars, double& tempsCalcul, double& tempsEval, double delta = 0.01 ) {

        bool hasNoError = true;
        MultiDimDecisionDiagramBase<double>* a3 = NULL;

        Timer timy;
        timy.reset();

        // ******************************************************************************************************
        // Generation du diagramme résultat

        switch( operationId ) {
          case 1 : // Test Addition
            TS_GUM_ASSERT_THROWS_NOTHING( a3 = projectSumMultiDimDecisionDiagram( a1, del_vars ) );
            break;
          case 2 : // Test Multiplication
            TS_GUM_ASSERT_THROWS_NOTHING( a3 = projectProductMultiDimDecisionDiagram( a1, del_vars ) );
            break;
          case 3 : // Test Minimisation
            TS_GUM_ASSERT_THROWS_NOTHING( a3 = projectMinMultiDimDecisionDiagram( a1, del_vars ) );
            break;
          case 4 :  // Test Maximisation
            TS_GUM_ASSERT_THROWS_NOTHING( a3 = projectMaxMultiDimDecisionDiagram( a1, del_vars ) );
            break;
          default :
            GUM_ERROR( OperationNotAllowed, "HEU ....." );
        }

        // ******************************************************************************************************

        tempsCalcul += timy.step();

        timy.reset();

        // ******************************************************************************************************
        if( a3 != NULL ) {

          Instantiation instEleminatedVar( a1 );
          Instantiation instRemainingVar( a3 );

          for( instRemainingVar.setFirst(); ! instRemainingVar.end() && hasNoError; ++instRemainingVar ) {

            switch( operationId ) {

              case 1 : {// Test Addition
                double sum  = 0;

                for( instEleminatedVar.chgValIn( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut( instRemainingVar ) )
                  sum += a1->get( instEleminatedVar );

                TS_ASSERT_DELTA( a3->get( instRemainingVar ), sum, delta );

                if( a3->get( instRemainingVar ) != sum )
                  hasNoError = false;
              }

              break;

              case 2 : {// Test Multiplication
                double product  = 1;

                for( instEleminatedVar.chgValIn( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut( instRemainingVar ) )
                  product *= a1->get( instEleminatedVar );

                TS_ASSERT_DELTA( a3->get( instRemainingVar ), product, delta );

                if( a3->get( instRemainingVar ) != product )
                  hasNoError = false;
              }

              break;

              case 3 : {// Test Minimisation
                double min  = std::numeric_limits<double>::max();

                for( instEleminatedVar.chgValIn( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut( instRemainingVar ) )
                  min  = min >= a1->get( instEleminatedVar ) ? a1->get( instEleminatedVar ) : min;

                TS_ASSERT_DELTA( a3->get( instRemainingVar ), min, delta );

                if( a3->get( instRemainingVar ) != min )
                  hasNoError = false;
              }

              break;

              case 4 :  {// Test Maximisation
                double max  = -1  * ( std::numeric_limits<double>::max() );

                for( instEleminatedVar.chgValIn( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut( instRemainingVar ) )
                  max  = max >= a1->get( instEleminatedVar ) ? max : a1->get( instEleminatedVar );

                TS_ASSERT_DELTA( a3->get( instRemainingVar ), max, delta );

                if( ( double )a3->get( instRemainingVar ) >= max + delta || ( double )a3->get( instRemainingVar ) <= max - delta )
                  hasNoError = false;
              }

              break;

              default :
                GUM_ERROR( OperationNotAllowed, "HEU ....." );
            }
          }

          if( !hasNoError )
            __saveDiagrams( a1, a3, del_vars );

          delete a3;
        } else {
          __saveDiagrams( a1, a3, del_vars );
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

        Timer time;
        double tempsGene = 0;
        double tempsCalcul = 0;
        double tempsEval = 0;

        // *************************************************************************************
        // First we try with a predefine structure
        // *************************************************************************************
        time.reset();

        Sequence< const DiscreteVariable* >* varList = __generateFixVarList();

        MultiDimDecisionDiagramBase<double>* a1  = NULL;
        TS_GUM_ASSERT_THROWS_NOTHING( a1 = __generateDecisionDiagram1( varList ) );

        Set< const DiscreteVariable* > del_vars;
        del_vars << varList->atPos( 0 );
        del_vars << varList->atPos( 1 );

        tempsGene += time.step();

        bool evalRes = true;

        for( int operationId = 1; operationId < 5 && evalRes; operationId++ )
          evalRes = __evalOperation( operationId, a1, del_vars, tempsCalcul, tempsEval );

        delete a1;

        del_vars.clear();

        for( SequenceIterator< const DiscreteVariable*> ite  = varList->begin(); ite != varList->end(); ++ite )
          delete *ite;

        delete varList;

        if( !evalRes )
          return;

        // *************************************************************************************
        // Then we try with random structure
        // *************************************************************************************
        for( int nbLoop  = 0; nbLoop < 100; nbLoop++ ) {

          time.reset();

          varList  = __generateRandomVarList( nbLoop + 1 );

          TS_GUM_ASSERT_THROWS_NOTHING( a1  = __generateRandomdoubleDecisionDiagram( varList, nbLoop + 2 ) );

          Idx maxSwapVar  = rand()%varList->size();

          for( Idx i  = 0; i < maxSwapVar; ++i )
            del_vars << varList->atPos( rand()%varList->size() );

          tempsGene += time.step();

          evalRes = true;

          TS_GUM_ASSERT_THROWS_NOTHING( evalRes = __evalOperation( 1, a1, del_vars, tempsCalcul, tempsEval ) );

          TS_GUM_ASSERT_THROWS_NOTHING( evalRes = __evalOperation( 3, a1, del_vars, tempsCalcul, tempsEval ) );

          TS_GUM_ASSERT_THROWS_NOTHING( evalRes = __evalOperation( 4, a1, del_vars, tempsCalcul, tempsEval ) );

          delete a1;

          del_vars.clear();

          for( SequenceIterator< const DiscreteVariable*> ite  = varList->begin(); ite != varList->end(); ++ite )
            delete *ite;

          delete varList;

          if( !evalRes ) {
            std::cout << "An error has occured! Aborting test." <<std::endl;
            return;
          }

          if( nbLoop%25 == 0 )
            std::cout << "#";
          else if( nbLoop%5 == 0 )
            std::cout << ".";

          std::fflush( stdout );
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
//       Sequence< const DiscreteVariable* >* varList  = __generateFixVarList();
//
//
//       MultiDimDecisionDiagramFactory<double,LinearApproximationPolicy> f;
//       f.setEpsilon( 0.9 );
//       f.setLowLimit( 0 );
//       f.setHighLimit( 4.5 );
//       f.setVariablesSequence( *varList );
//       NodeId a  = f.addNonTerminalNode( *( varList->atPos( 0 ) ) );
//       NodeId b  = f.addNonTerminalNode( *( varList->atPos( 1 ) ) );
//       NodeId c  = f.addNonTerminalNode( *( varList->atPos( 2 ) ) );
//
//       NodeId d  = f.addTerminalNode( 1 );
//       NodeId e  = f.addTerminalNode( 2 );
//       NodeId g  = f.addTerminalNode( 3 );
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
//       MultiDimDecisionDiagramBase<double>* a1  = f.getMultiDimDecisionDiagram( false );
//
//       Set< const DiscreteVariable* > del_vars;
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
//       for ( SequenceIterator< const DiscreteVariable*> ite  = varList->begin(); ite != varList->end(); ++ite )
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
//    Idx maxSwapVar  = rand()%varList->size();
//    for( Idx i  = 0; i < maxSwapVar; ++i )
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
//    for( SequenceIterator< const DiscreteVariable*> ite  = varList->begin(); ite != varList->end(); ++ite )
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

//       Sequence< const DiscreteVariable* >* varList  = new Sequence< const DiscreteVariable* >();
//
//       varList->insert( new LabelizedVariable( "0", "", 3 ) );
//       varList->insert( new LabelizedVariable( "1", "", 3 ) );
//       varList->insert( new LabelizedVariable( "2", "", 2 ) );
//       varList->insert( new LabelizedVariable( "3", "", 2 ) );
//       varList->insert( new LabelizedVariable( "4", "", 3 ) );
//       varList->insert( new LabelizedVariable( "5", "", 3 ) );
//       varList->insert( new LabelizedVariable( "6", "", 3 ) );
//       varList->insert( new LabelizedVariable( "7", "", 2 ) );
//       varList->insert( new LabelizedVariable( "8", "", 3 ) );
//       varList->insert( new LabelizedVariable( "9", "", 2 ) );
//
//       MultiDimDecisionDiagramFactory<double> facto;
//       facto.setVariablesSequence( *varList );
//
//       NodeId nv1  = facto.addNonTerminalNode( varList->atPos( 1 ) );
//       NodeId nt0  = facto.addTerminalNode( 0 );
//
//       facto.insertArc( nv1, nv3, 0 );
//       facto.insertDefaultArc( nv1, nt0 );
//
//
//       MultiDimDecisionDiagramBase<double>* a1  = NULL;
//       TS_GUM_ASSERT_THROWS_NOTHING( a1  = facto.getMultiDimDecisionDiagram() );
//
//       Set< const DiscreteVariable* > del_vars;
//       del_vars << varList->atPos( 3 );
//
//       TS_GUM_ASSERT_THROWS_NOTHING( __evalOperation( 1, a1, del_vars ) );

//       delete a1;
//       del_vars.clear();
//
//       for( SequenceIterator< const DiscreteVariable*> ite  = varList->begin(); ite != varList->end(); ++ite )
//    delete *ite;
//       delete varList;
      }

  };
}
