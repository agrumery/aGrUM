/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
// =========================================================================
#include <vector>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
// =========================================================================
#include <cxxtest/AgrumTestSuite.h>
#include "testsuite_utils.h"
// =========================================================================
#include <agrum/multidim/multiDimFunctionGraph.h>
#include <agrum/multidim/multiDimFunctionGraphManager.h>
#include <agrum/multidim/multiDimFunctionGraphGenerator.h>
#include <agrum/multidim/instantiation.h>
// =========================================================================
#include <agrum/variables/labelizedVariable.h>
// =========================================================================

namespace gum_tests {

  class MultiDimFunctionGraphTestSuite: public CxxTest::TestSuite {

    private:
      void __fillFactory ( gum::MultiDimFunctionGraph< float >* factory, gum::List<gum::NodeId>* idList ) {


          factory->add( *Cprimevar );
          factory->add( *Cvar );
          factory->add( *PLvar );
          factory->add( *APUvar );
          factory->add( *BPUvar );
          factory->add( *ADRvar );
          factory->add( *BDRvar );
          factory->add( *BOvar );


          idList->insert ( factory->manager()->addNonTerminalNode ( Cprimevar ) ); //1
          factory->manager()->setRootNode((*idList)[0]);
          idList->insert ( factory->manager()->addNonTerminalNode ( Cvar ) );      //2
          idList->insert ( factory->manager()->addNonTerminalNode ( PLvar ) );     //3
          idList->insert ( factory->manager()->addNonTerminalNode ( APUvar ) );    //4
          idList->insert ( factory->manager()->addNonTerminalNode ( BPUvar ) );    //5
          idList->insert ( factory->manager()->addNonTerminalNode ( ADRvar ) );    //6
          idList->insert ( factory->manager()->addNonTerminalNode ( BDRvar ) );    //7
          idList->insert ( factory->manager()->addNonTerminalNode ( BOvar ) );     //8

          idList->insert ( factory->manager()->addNonTerminalNode ( Cvar ) );      //9
          idList->insert ( factory->manager()->addNonTerminalNode ( PLvar ) );     //10
          idList->insert ( factory->manager()->addNonTerminalNode ( APUvar ) );    //11
          idList->insert ( factory->manager()->addNonTerminalNode ( BPUvar ) );    //12
          idList->insert ( factory->manager()->addNonTerminalNode ( ADRvar ) );    //13
          idList->insert ( factory->manager()->addNonTerminalNode ( BDRvar ) );    //14
          idList->insert ( factory->manager()->addNonTerminalNode ( BOvar ) );     //15

          idList->insert ( factory->manager()->addTerminalNode ( 0 ) );    //16
          idList->insert ( factory->manager()->addTerminalNode ( 10 ) );   //17
          idList->insert ( factory->manager()->addTerminalNode ( 9 ) );    //18
          idList->insert ( factory->manager()->addTerminalNode ( 1 ) );    //19


          factory->manager()->setSon ( ( *idList ) [0], 1, ( *idList ) [1] );
          factory->manager()->setSon ( ( *idList ) [0], 0, ( *idList ) [8] );

          factory->manager()->setSon ( ( *idList ) [1], 1, ( *idList ) [17] );
          factory->manager()->setSon ( ( *idList ) [1], 0, ( *idList ) [2] );
          factory->manager()->setSon ( ( *idList ) [2], 1, ( *idList ) [3] );
          factory->manager()->setSon ( ( *idList ) [2], 0, ( *idList ) [5] );
          factory->manager()->setSon ( ( *idList ) [3], 1, ( *idList ) [4] );
          factory->manager()->setSon ( ( *idList ) [3], 0, ( *idList ) [5] );
          factory->manager()->setSon ( ( *idList ) [4], 1, ( *idList ) [7] );
          factory->manager()->setSon ( ( *idList ) [4], 0, ( *idList ) [15] );
          factory->manager()->setSon ( ( *idList ) [5], 1, ( *idList ) [6] );
          factory->manager()->setSon ( ( *idList ) [5], 0, ( *idList ) [15] );
          factory->manager()->setSon ( ( *idList ) [6], 1, ( *idList ) [7] );
          factory->manager()->setSon ( ( *idList ) [6], 0, ( *idList ) [15] );
          factory->manager()->setSon ( ( *idList ) [7], 1, ( *idList ) [17] );
          factory->manager()->setSon ( ( *idList ) [7], 0, ( *idList ) [15] );


          factory->manager()->setSon ( ( *idList ) [8], 1, ( *idList ) [18] );
          factory->manager()->setSon ( ( *idList ) [8], 0, ( *idList ) [9] );
          factory->manager()->setSon ( ( *idList ) [9], 1, ( *idList ) [10] );
          factory->manager()->setSon ( ( *idList ) [9], 0, ( *idList ) [12] );
          factory->manager()->setSon ( ( *idList ) [10], 1, ( *idList ) [11] );
          factory->manager()->setSon ( ( *idList ) [10], 0, ( *idList ) [12] );
          factory->manager()->setSon ( ( *idList ) [11], 1, ( *idList ) [14] );
          factory->manager()->setSon ( ( *idList ) [11], 0, ( *idList ) [16] );
          factory->manager()->setSon ( ( *idList ) [12], 1, ( *idList ) [13] );
          factory->manager()->setSon ( ( *idList ) [12], 0, ( *idList ) [16] );
          factory->manager()->setSon ( ( *idList ) [13], 1, ( *idList ) [14] );
          factory->manager()->setSon ( ( *idList ) [13], 0, ( *idList ) [16] );
          factory->manager()->setSon ( ( *idList ) [14], 1, ( *idList ) [18] );
          factory->manager()->setSon ( ( *idList ) [14], 0, ( *idList ) [16] );
        }

    public:
      gum::LabelizedVariable* Cvar, *PLvar, *APUvar, *BPUvar, *ADRvar, *BDRvar, *BOvar, *Cprimevar;

      void setUp() {
        Cvar = new gum::LabelizedVariable ( "C", "C", 2 );
        PLvar = new gum::LabelizedVariable ( "PL", "PL", 2 );
        APUvar = new gum::LabelizedVariable ( "APU", "APU", 2 );
        BPUvar = new gum::LabelizedVariable ( "BPU", "BPU", 2 );
        ADRvar = new gum::LabelizedVariable ( "ADR", "ADR", 2 );
        BDRvar = new gum::LabelizedVariable ( "BDR", "BDR", 2 );
        BOvar = new gum::LabelizedVariable ( "BO", "BO", 2 );
        Cprimevar = new gum::LabelizedVariable ( "C'", "C'", 2 );
      }

      void tearDown() {
        delete Cvar;
        delete Cprimevar;
        delete PLvar;
        delete APUvar;
        delete BPUvar;
        delete ADRvar;
        delete BDRvar;
        delete BOvar;
      }

      /** *************************************************************************************/
      /**  Test sur la construction, le remplissage et la destruction d'une factory  */
      /** *************************************************************************************/
      void test_Simple_Creation() {

        // *********************************************************************
        // Création du multidim
        // *********************************************************************
        gum::MultiDimFunctionGraph<float>* FunctionGraph = NULL;

        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph = new gum::MultiDimFunctionGraph<float>() );

        // *********************************************************************
        // Remplissage du multidim ( donc accessoirement des fonctions d'insertion
        // de noeuds et d'arcs sans risques de levage d'exception)
        // *********************************************************************
        gum::List<gum::NodeId> idList;
        TS_GUM_ASSERT_THROWS_NOTHING ( __fillFactory ( FunctionGraph, &idList ) );

        // *********************************************************************
        // Destruction du multidim
        // *********************************************************************
        TS_GUM_ASSERT_THROWS_NOTHING ( delete FunctionGraph );
      }



      void test_MultiDimFunctionGraph_Diagram_Handlers_Methods() {

        // *********************************************************************
        // Création du multidim
        // *********************************************************************
        gum::MultiDimFunctionGraph<float>* FunctionGraph = new gum::MultiDimFunctionGraph<float>();
        gum::List<gum::NodeId> idList;
        __fillFactory ( FunctionGraph, &idList );

        // *********************************************************************
        // Tests
        // *********************************************************************
        gum::LabelizedVariable* Banditovar = new gum::LabelizedVariable ( "Bandito", "Desperado", 2 );

        // Test is terminal node
        TS_ASSERT_EQUALS ( FunctionGraph->isTerminalNode ( 16 ), true );
        TS_ASSERT_EQUALS ( FunctionGraph->isTerminalNode ( 6 ), false );

        // Test Root
        TS_ASSERT_EQUALS ( FunctionGraph->root(), ( gum::NodeId ) 1 );

        // test node value
        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph->nodeValue ( 16 ) );
        TS_ASSERT_EQUALS ( FunctionGraph->nodeValue ( 16 ), 0 );
        TS_ASSERT_THROWS ( FunctionGraph->nodeValue ( 6 ), gum::NotFound );

        // Test node variable
        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph->node ( 6 ) );
        TS_ASSERT_THROWS ( FunctionGraph->node ( 16 ), gum::NotFound );

        // *********************************************************************
        // Cleaning
        // *********************************************************************
        TS_GUM_ASSERT_THROWS_NOTHING ( delete FunctionGraph );
        delete Banditovar;

      }

      void est_toDot() {

        // *********************************************************************
        // Création du multidim
        // *********************************************************************
          gum::MultiDimFunctionGraph<float>* FunctionGraph = new gum::MultiDimFunctionGraph<float>();
          gum::List<gum::NodeId> idList;
          __fillFactory ( FunctionGraph, &idList );

        // For comparison with what readers will return
        std::string dotfile = GET_PATH_STR ( "FunctionGraph.dot" );
        std::ofstream output ( dotfile.c_str(), std::ios::out | std::ios::trunc );

        if ( ! output.good() ) {
          GUM_ERROR ( gum::IOError, "Stream states flags are not all unset." );
        }

        output << FunctionGraph->toDot();

        output.flush();

        output.close();

        if ( output.fail() ) {
          GUM_ERROR ( gum::IOError, "Writting in the ostream failed." );
        }

        delete FunctionGraph;
      }



      /** *************************************************************************************/
      /**  Test sur des modifications apportées au remplissage du factory     */
      /** *************************************************************************************/
      void est_Manager_Graphical_Functions() {

        // *********************************************************************
        // Création du multidim
        // *********************************************************************
        gum::MultiDimFunctionGraph<float>* FunctionGraph = new gum::MultiDimFunctionGraph<float>();
        gum::List<gum::NodeId> idList;
        __fillFactory ( FunctionGraph, &idList );


        // *********************************************************************
        // Test sur les fonctions d'ajout et retrait de noeud
        // *********************************************************************

        // Ajout d'un noeud terminal ayant la même valeur qu'un autre noeud terminal
        // (la factory doit renvoyer l'id de ce node)
        TS_ASSERT_EQUALS ( FunctionGraph->manager()->addTerminalNode ( 10 ), idList[16] );
//        std::cout << idList << std::endl;
//        FunctionGraph->toDot();

        // Test de retrait d'un noeud non terminal
        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph->manager()->eraseNode ( idList[12] ) );

        // Test de retrait d'un noeud terminal
        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph->manager()->eraseNode ( idList[15] ) );

        // Test de retrait du même noeud, pour s'assurer du levage d'exception
        TS_ASSERT_THROWS ( FunctionGraph->manager()->eraseNode ( idList[15] ), gum::NotFound );


        // *********************************************************************
        // Test sur les fonctions d'ajout et retrait d'arc
        // *********************************************************************

//        // Test d'ajout d'un arc ayant pour départ un noeud inexistant (doit lever l'exception gum::NotFound)
//        // (le noeud 15 a été détruit un peu plus haut)
//        TS_ASSERT_THROWS ( FunctionGraph->manager()->setSon ( idList[15], 2, idList[1] ), gum::NotFound );

//        // Test d'ajout d'un arc ayant pour départ un noeud terminal (doit lever l'exception gum::InvalidNode
//        // vu que les noeuds terminaux sont ... terminaux)
//        TS_ASSERT_THROWS ( FunctionGraph->manager()->setSon ( idList[16], 3, idList[1] ), gum::InvalidNode );

//        // Test d'ajout d'un arc entre 2 noeuds dépassant le nb de modlaity de la variable originelle
//        TS_ASSERT_THROWS ( FunctionGraph->manager()->setSon ( idList[8], 4, idList[18] ), gum::InvalidArgument );

//        // Test d'ajout d'un arc entre 2 noeuds déjà reliés par un autre arc de valeur égale(doit DuplicateElement)
//        TS_ASSERT_THROWS ( FunctionGraph->manager()->setSon ( idList[8], 1, idList[18] ), gum::DuplicateElement );

//        // Test d'ajout d'un arc qui viole l'ordre sur les variables (doit lever l'exception gum::OperationNotAllowed)
//        TS_ASSERT_THROWS ( FunctionGraph->manager()->setSon ( idList[8], 0, idList[1] ), gum::OperationNotAllowed );

//        // Test de retrait d'un arc existant (ne doit lever aucune exception)
//        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph->manager()->eraseArc ( idList[3], idList[4] ) );

//        // Test de retrait d'un arc avec valeur spécifiée
//        TS_GUM_ASSERT_THROWS_NOTHING( FunctionGraph->manager()->eraseSpecificArc( idList[8], 1, idList[18] ) );
          
//        // Test de retrait d'un arc après une tentative ayant eu pour but d'ajouter un autre arc reliant les deux noeuds
//        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph->manager()->setSon ( idList[8], 1, idList[18] ) );
//        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph->manager()->eraseArc( idList[8], idList[18] ) );

//        // Test de retrait de retrait de l'arc qui violait la contrainte d'ordre et donc n'existe pas
//        TS_ASSERT_THROWS ( FunctionGraph->manager()->eraseArc ( idList[8], idList[1] ), gum::InvalidArc );

//        // Test de retrait d'un arc dont un des noeuds n'existe plus (doit lever l'exception InvalidArc)
//        TS_ASSERT_THROWS ( FunctionGraph->manager()->eraseArc ( idList[15], idList[1] ), gum::NotFound );


        // *********************************************************************
        // Test sur les fonctions d'ajout et retrait d'arc par defaut
        // *********************************************************************

//        // Test d'ajout d'un arc par défaut sans risque
//        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph->manager()->insertDefaultArc ( idList[5], idList[7] ) );
//        //factory->showProperties();

//        // Test d'ajout d'un arc par défaut ayant pour départ un noeud inexistant (doit lever l'exception gum::InvalidNode)
//        // (le noeud 15 a été détruit un peu plus haut
//        TS_ASSERT_THROWS ( FunctionGraph->manager()->insertDefaultArc ( idList[15], idList[1] ), gum::NotFound );

//        // Test d'ajout d'un arc par défaut ayant pour départ un noeud terminal (doit lever l'exception gum::OperationNotAllowed
//        // vu que les noeuds terminaux sont ... terminaux)
//        TS_ASSERT_THROWS ( FunctionGraph->manager()->insertDefaultArc ( idList[16], idList[1] ), gum::InvalidNode );

//        // Test d'ajout d'un arc par défaut entre 2 noeuds déjà reliés par un autre arc (doit rien lever)
//        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph->manager()->insertDefaultArc ( idList[0], idList[1] ) );

//        // Test d'ajout d'un arc par défaut entre 2 noeuds déjà reliés par un autre arc par defaut(doit lever DuplicateElement)
//        TS_ASSERT_THROWS ( FunctionGraph->manager()->insertDefaultArc ( idList[0], idList[3] ), gum::DuplicateElement );

//        // Test d'ajout d'un arc par défaut qui viole l'ordre sur les variables (doit lever l'exception InvalidArc)
//        TS_ASSERT_THROWS ( FunctionGraph->manager()->insertDefaultArc ( idList[8], idList[1] ), gum::OperationNotAllowed );

        TS_GUM_ASSERT_THROWS_NOTHING ( delete FunctionGraph );
      }


      /** *************************************************************************************/
      /**   Test des fonctions d'accès et de modification du multidim     */
      /** *************************************************************************************/
      void test_MultiDimFunctionGraph_Accessors_Modifiers_Methods() {

        // *********************************************************************
        // Création du multidim
        // *********************************************************************
        gum::MultiDimFunctionGraph<float>* FunctionGraph = new gum::MultiDimFunctionGraph<float>();
        gum::List<gum::NodeId> idList;
        __fillFactory ( FunctionGraph, &idList );


        // *********************************************************************
        // Test variablesSequence(), et si toutes les variables sont
        // accesoirement bien présentent
        // *********************************************************************
        gum::Sequence< const gum::DiscreteVariable* > varSeq;
        TS_GUM_ASSERT_THROWS_NOTHING ( varSeq = FunctionGraph->variablesSequence( ));
        TS_ASSERT_EQUALS ( varSeq.exists ( Cprimevar ), true );
        TS_ASSERT_EQUALS ( varSeq.exists ( Cvar ), true );
        TS_ASSERT_EQUALS ( varSeq.exists ( PLvar ), true );
        TS_ASSERT_EQUALS ( varSeq.exists ( APUvar ), true );
        TS_ASSERT_EQUALS ( varSeq.exists ( BPUvar ), true );
        TS_ASSERT_EQUALS ( varSeq.exists ( ADRvar ), true );
        TS_ASSERT_EQUALS ( varSeq.exists ( BDRvar ), true );
        TS_ASSERT_EQUALS ( varSeq.exists ( BOvar ), true );


        // *********************************************************************
        // Test name() et basename()
        // *********************************************************************
        TS_ASSERT_EQUALS ( FunctionGraph->name(), "MultiDimFunctionGraph" );

        TS_ASSERT_EQUALS ( FunctionGraph->basename(), "MultiDimImplementation" );


        // *********************************************************************
        // Test set(), fill() et fillWith()
        // *********************************************************************
        gum::Instantiation inst ( *FunctionGraph );

        TS_ASSERT_THROWS ( FunctionGraph->set ( inst, 14.0 ), gum::OperationNotAllowed );

        TS_ASSERT_THROWS ( FunctionGraph->fill ( 14.0 ), gum::OperationNotAllowed );

        std::vector<float> v;

        for ( float i = 0; i < 128; i++ )
          v.push_back ( i );

        TS_ASSERT_THROWS ( FunctionGraph->fillWith ( v ), gum::OperationNotAllowed );

        for ( float i = 128; i < 256; i++ )
          v.push_back ( i );

        TS_ASSERT_THROWS ( FunctionGraph->fillWith ( v ), gum::OperationNotAllowed );


        // *********************************************************************
        // Test get(), [], et l'accès aux valeurs
        // *********************************************************************
        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph->get ( inst ) );
        TS_GUM_ASSERT_THROWS_NOTHING ( ( *FunctionGraph ) [inst] );

        for ( inst.setFirst(); ! inst.end(); ++inst )
          if ( inst.val ( *Cvar ) == 1 ) {
            if ( inst.val ( *Cprimevar ) == 1 ) {
              TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 9, 0.001 );
            } else {
              TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 1, 0.001 );
            }
          } else {

            if ( inst.val ( *PLvar ) == 1 ) {

              if ( inst.val ( *APUvar ) == 1 ) {

                if ( inst.val ( *BPUvar ) == 1 ) {

                  if ( inst.val ( *BOvar ) == 1 ) {

                    if ( inst.val ( *Cprimevar ) == 1 ) {
                      TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 9, 0.001 );
                    } else {
                      TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 1, 0.001 );
                    }

                  } else {

                    if ( inst.val ( *Cprimevar ) == 1 ) {
                      TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 0, 0.001 );
                    } else {
                      TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 10, 0.001 );
                    }

                  }
                } else {

                  if ( inst.val ( *Cprimevar ) == 1 ) {
                    TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 0, 0.001 );
                  } else {
                    TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 10, 0.001 );
                  }

                }
              } else {

                if ( inst.val ( *ADRvar ) == 1 ) {

                  if ( inst.val ( *BDRvar ) == 1 ) {

                    if ( inst.val ( *BOvar ) == 1 ) {

                      if ( inst.val ( *Cprimevar ) == 1 ) {
                        TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 9, 0.001 );
                      } else {
                        TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 1, 0.001 );
                      }

                    } else {

                      if ( inst.val ( *Cprimevar ) == 1 ) {
                        TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 0, 0.001 );
                      } else {
                        TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 10, 0.001 );
                      }

                    }

                  } else {

                    if ( inst.val ( *Cprimevar ) == 1 ) {
                      TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 0, 0.001 );
                    } else {
                      TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 10, 0.001 );
                    }

                  }

                } else {

                  if ( inst.val ( *Cprimevar ) == 1 ) {
                    TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 0, 0.001 );
                  } else {
                    TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 10, 0.001 );
                  }

                }
              }
            } else {

              if ( inst.val ( *ADRvar ) == 1 ) {

                if ( inst.val ( *BDRvar ) == 1 ) {

                  if ( inst.val ( *BOvar ) == 1 ) {

                    if ( inst.val ( *Cprimevar ) == 1 ) {
                      TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 9, 0.001 );
                    } else {
                      TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 1, 0.001 );
                    }

                  } else {

                    if ( inst.val ( *Cprimevar ) == 1 ) {
                      TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 0, 0.001 );
                    } else {
                      TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 10, 0.001 );
                    }

                  }

                } else {

                  if ( inst.val ( *Cprimevar ) == 1 ) {
                    TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 0, 0.001 );
                  } else {
                    TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 10, 0.001 );
                  }

                }

              } else {

                if ( inst.val ( *Cprimevar ) == 1 ) {
                  TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 0, 0.001 );
                } else {
                  TS_ASSERT_DELTA ( FunctionGraph->get ( inst ), 10, 0.001 );
                }

              }
            }
          }


        // *********************************************************************
        // Test swap()
        // *********************************************************************
        gum::LabelizedVariable* lv = new gum::LabelizedVariable ( "Test", "Cornichon", 2 );
        TS_ASSERT_THROWS ( FunctionGraph->swap ( *Cvar, *lv ), gum::OperationNotAllowed );
        delete lv;

        TS_GUM_ASSERT_THROWS_NOTHING ( delete FunctionGraph );
      }



      void test_MultiDimFunctionGraph_Implementation_Methods() {


        // *********************************************************************
        // Création du multidim
        // *********************************************************************
        gum::MultiDimFunctionGraph<float>* FunctionGraph = new gum::MultiDimFunctionGraph<float>();
        gum::List<gum::NodeId> idList;
        __fillFactory ( FunctionGraph, &idList );
        gum::LabelizedVariable* lv = new gum::LabelizedVariable ( "Test", "Cornichon", 2 );

        // *********************************************************************
        // Test nbrDim(), domainSize(), realSize()
        // *********************************************************************

        TS_ASSERT_EQUALS ( FunctionGraph->nbrDim( ) , ( gum::Idx ) 8 );

        TS_ASSERT_EQUALS ( FunctionGraph->domainSize( ), ( gum::Size ) 256 );

        TS_ASSERT_EQUALS ( FunctionGraph->realSize( ), ( gum::Size ) 15 );//19 );

        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph->compressionRate( ) );

        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph->variable ( 0 ) );

        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph->pos ( *Cvar ) );

        TS_ASSERT_EQUALS ( FunctionGraph->contains ( *Cprimevar ), true );
        TS_ASSERT_EQUALS ( FunctionGraph->contains ( *lv ), false );

        TS_ASSERT_EQUALS ( FunctionGraph->empty( ), false );

        // *********************************************************************
        // Test add(), erase()
        // *********************************************************************

        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph->add ( *lv ) );


        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph->erase ( *Cvar ) );


        TS_GUM_ASSERT_THROWS_NOTHING ( delete FunctionGraph );
        delete lv;
      }



      void test_MultiDimFunctionGraph_Copy_Methods() {

        // *********************************************************************
        // Création du multidim
        // *********************************************************************
          gum::MultiDimFunctionGraph<float>* FunctionGraph = new gum::MultiDimFunctionGraph<float>();
          gum::List<gum::NodeId> idList;
          __fillFactory ( FunctionGraph, &idList );


        gum::MultiDimFunctionGraph<float>* FunctionGraph2 = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph2 = new gum::MultiDimFunctionGraph<float>( *FunctionGraph ) );

        TS_ASSERT_THROWS ( FunctionGraph2->copyFrom ( *FunctionGraph ) , gum::OperationNotAllowed );

        TS_GUM_ASSERT_THROWS_NOTHING ( FunctionGraph2->copy(*FunctionGraph); );


        TS_GUM_ASSERT_THROWS_NOTHING ( delete FunctionGraph );

        TS_GUM_ASSERT_THROWS_NOTHING ( delete FunctionGraph2 );

      }



      void test_MultiDimFunctionGraph_Various_Methods() {

        // *********************************************************************
        // Création du multidim
        // *********************************************************************
        gum::MultiDimFunctionGraph<float>* dg1 = new gum::MultiDimFunctionGraph<float>();
        gum::MultiDimFunctionGraph<float>* dg2 = new gum::MultiDimFunctionGraph<float>();
        gum::MultiDimFunctionGraph<float>* dg3 = new gum::MultiDimFunctionGraph<float>();
        gum::List<gum::NodeId> idList;
        __fillFactory ( dg1, &idList );
        __fillFactory ( dg2, &idList );
        __fillFactory ( dg3, &idList );

//        dg3->manager()->eraseArc ( 3,6 );
        dg3->manager()->setSon ( 3,0,16 );
        // *********************************************************************
        // test tostring() et tostring(gum::Instantiation*)
        // *********************************************************************
//        TS_GUM_ASSERT_THROWS_NOTHING ( dg1->toString() );

        gum::Instantiation inst ( *dg1 );
        inst.setFirst();
        TS_GUM_ASSERT_THROWS_NOTHING ( dg1->toString ( &inst ) );


        // *********************************************************************
        // test == et !=
        // *********************************************************************
        TS_ASSERT_EQUALS ( *dg1 == *dg2, true );
        TS_ASSERT_EQUALS ( *dg1 == *dg3, false );

        TS_ASSERT_EQUALS ( *dg1 != *dg2, false );
        TS_ASSERT_EQUALS ( *dg1 != *dg3, true );



        TS_GUM_ASSERT_THROWS_NOTHING ( delete dg1 );
        TS_GUM_ASSERT_THROWS_NOTHING ( delete dg2 );
        TS_GUM_ASSERT_THROWS_NOTHING ( delete dg3 );


      }

      void test_MoveTo(){

        // *********************************************************************
        // Création du multidim
        // *********************************************************************
        gum::MultiDimFunctionGraph<float>* FunctionGraph = new gum::MultiDimFunctionGraph<float>();
        gum::List<gum::NodeId> idList;
        __fillFactory ( FunctionGraph, &idList );

        TS_GUM_ASSERT_THROWS_NOTHING( FunctionGraph->manager()->moveTo(Cprimevar, FunctionGraph->variablesSequence().pos( BOvar ) ) );

        // For comparison with what readers will return
        std::string dotfile = GET_PATH_STR ( "FunctionGraphMVBOvar.dot" );
        std::ofstream output ( dotfile.c_str(), std::ios::out | std::ios::trunc );

        if ( ! output.good() ) {
          GUM_ERROR ( gum::IOError, "Stream states flags are not all unset." );
        }

        output << FunctionGraph->toDot(true);

        output.flush();

        output.close();

        if ( output.fail() ) {
          GUM_ERROR ( gum::IOError, "Writting in the ostream failed." );
        }

        delete FunctionGraph;

      }

      void test_Generator(){

        for(gum::Idx i = 0; i < 100; i++){

          gum::Sequence< const gum::DiscreteVariable* >* varList = new gum::Sequence< const gum::DiscreteVariable* >();

          for ( int j = 0; j < 10; j++ ) {
            std::stringstream varName;
            varName << "var" << j;
            varList->insert ( new gum::LabelizedVariable ( varName.str(), "", 2 + rand() % 4 ) );
          }

          gum::MultiDimFunctionGraphGenerator gene( 2, 5, *varList);

          gum::MultiDimFunctionGraph<double>* dg1 = nullptr;
          TS_GUM_ASSERT_THROWS_NOTHING( dg1 = gene.generate());

          gum::MultiDimFunctionGraph<double>* dg2 = new gum::MultiDimFunctionGraph<double>();

          TS_GUM_ASSERT_THROWS_NOTHING( dg2->copy(*dg1) );
          TS_GUM_ASSERT_THROWS_NOTHING( dg2->manager()->reduce() );

          gum::Instantiation inst(dg1);
          for ( inst.setFirst(); ! inst.end(); ++inst ){
            TS_ASSERT_DELTA ( dg1->get(inst), dg2->get(inst), 0.001 );
          }


          dg2->manager()->clean();
          for ( inst.setFirst(); ! inst.end(); ++inst ){
            TS_ASSERT_DELTA ( dg1->get(inst), dg2->get(inst), 0.001 );
          }

          delete dg1;
          delete dg2;

          for ( gum::SequenceIterator< const gum::DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
            delete *ite;
          delete varList;
        }

      }
  };
}
