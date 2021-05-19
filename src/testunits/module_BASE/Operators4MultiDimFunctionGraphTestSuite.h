/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


// ============================================================================
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sstream>
#include <string>
// =============================================================================
#include <gumtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
// =============================================================================
#include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>
#include <agrum/tools/multidim/implementations/multiDimFunctionGraphGenerator.h>
#include <agrum/tools/multidim/instantiation.h>
#include <agrum/tools/multidim/potential.h>
// =============================================================================
#include <agrum/tools/variables/labelizedVariable.h>
// =============================================================================

namespace gum_tests {

  class Operators4MultiDimFunctionGraphTestSuite: public CxxTest::TestSuite {
    private:
    /// Defines the maximal number of modalities for a var (which is choose
    /// randomly).
    /// The domain size will be of 2 + rand()%maxVarDomainSize.
    static const gum::Idx maxVarDomainSize = 5;

    /// Defines the minimal and maximal number of var in a diagram.
    static const gum::Size minNbVarInDiagram = 6, maxNbVarInDiagram = 10;

    /// Defines how many different operations we perform (from 2 to 5 )
    static const gum::Idx nbOperation = 4;

    /// Defines the number of iteration on randomly generated diagram we perform
    static const gum::Idx nbIter = 100;


    // *****************************************************************************************************
    /// Génération fixe d'une liste de variable
    // *****************************************************************************************************
    gum::Sequence< const gum::DiscreteVariable* >* generateFixVarList__() {
      gum::Sequence< const gum::DiscreteVariable* >* ret
         = new gum::Sequence< const gum::DiscreteVariable* >();
      ret->insert(new gum::LabelizedVariable("A", "", 2));
      ret->insert(new gum::LabelizedVariable("B", "", 2));
      ret->insert(new gum::LabelizedVariable("C", "", 2));

      return ret;
    }


    // *****************************************************************************************************
    /// Génération aléatoire d'une liste de 10 variables
    // *****************************************************************************************************
    gum::Sequence< const gum::DiscreteVariable* >*
       generateRandomVarList__(gum::Idx i) {
      gum::Sequence< const gum::DiscreteVariable* >* ret
         = new gum::Sequence< const gum::DiscreteVariable* >();

      for (gum::Idx j = 0; j < maxNbVarInDiagram; j++) {
        std::stringstream varName;
        varName << "var" << j;
        ret->insert(new gum::LabelizedVariable(varName.str(),
                                               "",
                                               2 + rand() % maxVarDomainSize));
      }

      return ret;
    }


    // *****************************************************************************************************
    /// Brassage aléatoire d'une liste de 10 variables
    // *****************************************************************************************************
    void shuffleVarList__(gum::Sequence< const gum::DiscreteVariable* >* varList,
                          gum::Idx                                       i) {
      for (gum::Idx j = 0; j < 10; j++)
        varList->swap(rand() % (varList->size()), rand() % (varList->size()));
    }


    // *****************************************************************************************************
    /// Génération fixe de diagramme de décision
    // *****************************************************************************************************
    gum::MultiDimFunctionGraph< double >* generateFunctionGraph1__(
       const gum::Sequence< const gum::DiscreteVariable* >* varList) {
      gum::MultiDimFunctionGraph< double >* generatedFunctionGraph
         = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();

      for (gum::SequenceIterator< const gum::DiscreteVariable* > varIter
           = varList->begin();
           varIter != varList->end();
           ++varIter)
        generatedFunctionGraph->add(**varIter);

      gum::NodeId a
         = generatedFunctionGraph->manager()->addInternalNode(varList->atPos(0));
      gum::NodeId b
         = generatedFunctionGraph->manager()->addInternalNode(varList->atPos(1));
      gum::NodeId c
         = generatedFunctionGraph->manager()->addInternalNode(varList->atPos(2));

      gum::NodeId d = generatedFunctionGraph->manager()->addTerminalNode(6);
      gum::NodeId e = generatedFunctionGraph->manager()->addTerminalNode(2);
      gum::NodeId g = generatedFunctionGraph->manager()->addTerminalNode(3);

      generatedFunctionGraph->manager()->setSon(a, 0, b);
      generatedFunctionGraph->manager()->setSon(a, 1, c);

      generatedFunctionGraph->manager()->setSon(b, 0, d);
      generatedFunctionGraph->manager()->setSon(b, 1, c);

      generatedFunctionGraph->manager()->setSon(c, 0, e);
      generatedFunctionGraph->manager()->setSon(c, 1, g);

      generatedFunctionGraph->manager()->setRootNode(a);

      return generatedFunctionGraph;
    }


    // *****************************************************************************************************
    /// Génération fixe de diagramme de décision
    // *****************************************************************************************************
    gum::MultiDimFunctionGraph< double >* generateFunctionGraph2__(
       const gum::Sequence< const gum::DiscreteVariable* >* varList) {
      gum::MultiDimFunctionGraph< double >* generatedFunctionGraph
         = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();

      for (gum::SequenceIterator< const gum::DiscreteVariable* > varIter
           = varList->begin();
           varIter != varList->end();
           ++varIter)
        generatedFunctionGraph->add(**varIter);

      gum::NodeId a
         = generatedFunctionGraph->manager()->addInternalNode(varList->atPos(0));
      gum::NodeId c
         = generatedFunctionGraph->manager()->addInternalNode(varList->atPos(2));

      gum::NodeId d = generatedFunctionGraph->manager()->addTerminalNode(4);
      gum::NodeId e = generatedFunctionGraph->manager()->addTerminalNode(5);
      gum::NodeId g = generatedFunctionGraph->manager()->addTerminalNode(1);

      generatedFunctionGraph->manager()->setSon(a, 0, d);
      generatedFunctionGraph->manager()->setSon(a, 1, c);

      generatedFunctionGraph->manager()->setSon(c, 0, e);
      generatedFunctionGraph->manager()->setSon(c, 1, g);

      generatedFunctionGraph->manager()->setRootNode(a);

      return generatedFunctionGraph;
    }


    // *****************************************************************************************************
    /// Génération aléatoire de diagramme de décision
    // *****************************************************************************************************
    gum::MultiDimFunctionGraph< double >* generateRandomFunctionGraph__(
       const gum::Sequence< const gum::DiscreteVariable* >* varList,
       double                                               lowLimit  = -100,
       double                                               highLimit = 100) {
      gum::MultiDimFunctionGraphGenerator gene(2, 5, *varList);

      return gene.generate();
    }


    // *****************************************************************************************************
    /// Sauvegarde des diagrammes générant une erreur dans un fichier log
    // *****************************************************************************************************
    void saveDiagrams__(gum::MultiDimFunctionGraph< double >* a1,
                        gum::MultiDimFunctionGraph< double >* a2,
                        gum::MultiDimFunctionGraph< double >* a3) {
      std::string   dotfile = GET_RESSOURCES_PATH("FunctionGraphError.log");
      std::ofstream output(dotfile.c_str(), std::ios::out);

      if (!output.good())
        GUM_ERROR(gum::IOError, "Stream states flags are not all unset.")

      output << std::endl;

      for (gum::SequenceIterator< const gum::DiscreteVariable* > ite
           = a1->variablesSequence().begin();
           ite != a1->variablesSequence().end();
           ++ite)
        output << (*ite)->toString() << " - ";

      output << std::endl;

      output << a1->toDot();

      output << std::endl;

      for (gum::SequenceIterator< const gum::DiscreteVariable* > ite
           = a2->variablesSequence().begin();
           ite != a2->variablesSequence().end();
           ++ite)
        output << (*ite)->toString() << " - ";

      output << std::endl;

      output << a2->toDot();

      if (a3 != nullptr) {
        output << std::endl;

        for (gum::SequenceIterator< const gum::DiscreteVariable* > ite
             = a3->variablesSequence().begin();
             ite != a3->variablesSequence().end();
             ++ite)
          output << (*ite)->toString() << " - ";

        output << std::endl;

        output << a3->toDot();
      }

      output.flush();

      output.close();

      if (output.fail()) GUM_ERROR(gum::IOError, "Writting in the ostream failed.")
    }


    // *****************************************************************************************************
    /// Evals given in parameter operation. Returned boolean parameter indicates
    /// if all went well or not
    // *****************************************************************************************************
    bool evalOperation__(gum::Idx                              operationId,
                         gum::MultiDimFunctionGraph< double >* a1,
                         gum::MultiDimFunctionGraph< double >* a2,
                         double&                               tempsCalcul,
                         double&                               tempsEval,
                         double                                delta = 0.01) {
      bool                                  hasNoError = true;
      gum::MultiDimFunctionGraph< double >* a3         = nullptr;

      gum::Timer timy;
      timy.reset();

      // ###################################################
      // Generation du diagramme résultat

      switch (operationId) {
        case 1:   // Test addition
          TS_GUM_ASSERT_THROWS_NOTHING(a3 = add2MultiDimFunctionGraphs(a1, a2));
          break;

        case 2:   // Test Substraction
          TS_GUM_ASSERT_THROWS_NOTHING(a3
                                       = subtract2MultiDimFunctionGraphs(a1, a2));
          break;

        case 3:   // Test Multiplication
          TS_GUM_ASSERT_THROWS_NOTHING(a3
                                       = multiply2MultiDimFunctionGraphs(a1, a2));
          break;

        case 4:   // Test Maximum
          TS_GUM_ASSERT_THROWS_NOTHING(a3
                                       = maximize2MultiDimFunctionGraphs(a1, a2));
          break;

        default:   // Should not happen
          GUM_ERROR(gum::OperationNotAllowed, "HEU .....")
      }

      // ###################################################

      tempsCalcul += timy.step();

      //      std::cout << a3->toDot() << std::endl;

      timy.reset();

      // ###################################################
      if (a3 != nullptr) {
        gum::Instantiation inst(a3);

        for (inst.setFirst(); !inst.end() && hasNoError; ++inst) {
          switch (operationId) {
            case 1:   // Test addition
              TS_ASSERT_DELTA(a3->get(inst), a1->get(inst) + a2->get(inst), delta);
              if (a3->get(inst) != a1->get(inst) + a2->get(inst))
                hasNoError = false;
              break;

            case 2:   // Test Substraction
              TS_ASSERT_DELTA(a3->get(inst), a1->get(inst) - a2->get(inst), delta);

              if (a3->get(inst) != a1->get(inst) - a2->get(inst))
                hasNoError = false;

              break;

            case 3:   // Test Multiplication
              TS_ASSERT_DELTA(a3->get(inst), a1->get(inst) * a2->get(inst), delta);

              if (a3->get(inst) != a1->get(inst) * a2->get(inst))
                hasNoError = false;

              break;

            case 4:   // Test Maximum
              TS_ASSERT_DELTA(a3->get(inst),
                              a1->get(inst) > a2->get(inst) ? a1->get(inst)
                                                            : a2->get(inst),
                              delta);

              if (a3->get(inst)
                  != (a1->get(inst) > a2->get(inst) ? a1->get(inst)
                                                    : a2->get(inst)))
                hasNoError = false;

              break;

            default:   // Should not happen
              GUM_ERROR(gum::OperationNotAllowed, "HEU .....")
          }
        }

        if (!hasNoError) saveDiagrams__(a1, a2, a3);
        delete a3;
      } else {
        saveDiagrams__(a1, a2, a3);
        hasNoError = false;
      }

      tempsEval += timy.step();

      return hasNoError;
    }

    public:
    // *****************************************************************************************************
    /// Test sur les fonctions avec valeurs exactes
    // *****************************************************************************************************
    void test_Operators_Functions_on_MultiDimFunctionGraphs() {
      gum::Timer time;
      double     tempsGene   = 0;
      double     tempsCalcul = 0;
      double     tempsEval   = 0;


      // First we try with a predefine structure
      {
        time.reset();

        gum::Sequence< const gum::DiscreteVariable* >* varList
           = generateFixVarList__();

        gum::MultiDimFunctionGraph< double >* a1 = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING(a1 = generateFunctionGraph1__(varList));

        gum::MultiDimFunctionGraph< double >* a2 = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING(a2 = generateFunctionGraph2__(varList));

        //          std::cout << a1->toDot() << std::endl;

        //          std::cout << a2->toDot() << std::endl;

        tempsGene += time.step();

        bool evalRes = true;

        for (gum::Idx opId = 1; opId < nbOperation && evalRes; opId++)
          evalRes = evalOperation__(opId, a1, a2, tempsCalcul, tempsEval);

        delete a1;
        delete a2;

        for (gum::SequenceIterator< const gum::DiscreteVariable* > ite
             = varList->begin();
             ite != varList->end();
             ++ite)
          delete *ite;
        delete varList;

        if (!evalRes) {
          //          std::cout << "An error has occured! Aborting test." <<
          //          std::endl;
          return;
        }

        std::fflush(stdout);
      }


      // Then we try with random structure
      //        gum::Idx i = 5;
    }

    // *****************************************************************************************************
    /// The aim of these function is to reproduce
    /// specific diagrams which generated bug previously
    /// The aim is to ensure these bugs do not occur again
    // *****************************************************************************************************
    void est_Reproducteur() {
      gum::LabelizedVariable* v0 = new gum::LabelizedVariable("v0", "", 2);
      gum::LabelizedVariable* v1 = new gum::LabelizedVariable("v1", "", 2);
      gum::LabelizedVariable* v2 = new gum::LabelizedVariable("v2", "", 2);
      gum::LabelizedVariable* v3 = new gum::LabelizedVariable("v3", "", 3);
      gum::LabelizedVariable* v4 = new gum::LabelizedVariable("4", "", 2);
      gum::LabelizedVariable* v5 = new gum::LabelizedVariable("5", "", 3);
      gum::LabelizedVariable* v6 = new gum::LabelizedVariable("6", "", 2);
      gum::LabelizedVariable* v7 = new gum::LabelizedVariable("7", "", 2);
      gum::LabelizedVariable* v8 = new gum::LabelizedVariable("8", "", 3);
      gum::LabelizedVariable* v9 = new gum::LabelizedVariable("9", "", 2);

      gum::NodeId n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12, n13,
         n14, n15, n16, n17, n18, n19, n20, n21,
         n22;   //,n23, n24, n25, n26;


      gum::MultiDimFunctionGraph< double >* dg1
         = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();

      dg1->add(*v3);
      dg1->add(*v1);
      dg1->add(*v0);
      dg1->add(*v2);

      n01 = dg1->manager()->addInternalNode(v3);
      n02 = dg1->manager()->addInternalNode(v0);
      n03 = dg1->manager()->addInternalNode(v1);
      n04 = dg1->manager()->addInternalNode(v1);
      n05 = dg1->manager()->addInternalNode(v2);
      n06 = dg1->manager()->addInternalNode(v2);
      n07 = dg1->manager()->addInternalNode(v0);
      n08 = dg1->manager()->addTerminalNode(19.0836);
      n09 = dg1->manager()->addTerminalNode(88.4791);
      n10 = dg1->manager()->addTerminalNode(16.2301);
      n11 = dg1->manager()->addTerminalNode(30.7077);
      n12 = dg1->manager()->addInternalNode(v2);
      n13 = dg1->manager()->addTerminalNode(90.9853);
      n14 = dg1->manager()->addTerminalNode(28.7184);
      n15 = dg1->manager()->addTerminalNode(16.5207);
      n16 = dg1->manager()->addTerminalNode(55.5485);
      n17 = dg1->manager()->addTerminalNode(8.60651);
      n18 = dg1->manager()->addTerminalNode(81.2182);
      n19 = dg1->manager()->addTerminalNode(84.3831);
      n20 = dg1->manager()->addTerminalNode(6.12946);
      n21 = dg1->manager()->addTerminalNode(83.9071);
      n22 = dg1->manager()->addTerminalNode(7.85962);

      dg1->manager()->setSon(n01, 0, n02);
      dg1->manager()->setSon(n01, 1, n03);
      dg1->manager()->setSon(n01, 2, n04);

      dg1->manager()->setSon(n02, 0, n05);
      dg1->manager()->setSon(n02, 1, n06);
      dg1->manager()->setSon(n02, 2, n07);

      dg1->manager()->setSon(n03, 0, n05);
      dg1->manager()->setSon(n03, 1, n06);

      dg1->manager()->setSon(n04, 0, n07);
      dg1->manager()->setSon(n04, 1, n05);

      dg1->manager()->setSon(n05, 0, n10);
      dg1->manager()->setSon(n05, 1, n11);
      dg1->manager()->setSon(n05, 2, n16);

      dg1->manager()->setSon(n06, 0, n08);
      dg1->manager()->setSon(n06, 1, n09);
      dg1->manager()->setSon(n06, 2, n11);

      dg1->manager()->setSon(n07, 0, n05);
      dg1->manager()->setSon(n07, 1, n12);

      dg1->manager()->setSon(n08, 0, n14);
      dg1->manager()->setSon(n08, 1, n15);
      dg1->manager()->setSon(n08, 2, n05);

      dg1->manager()->setSon(n09, 0, n10);
      dg1->manager()->setSon(n09, 1, n11);

      dg1->manager()->setSon(n12, 0, n13);
      dg1->manager()->setSon(n12, 1, n14);
      dg1->manager()->setSon(n12, 2, n19);

      dg1->manager()->setSon(n13, 0, n20);
      dg1->manager()->setSon(n13, 1, n21);
      dg1->manager()->setSon(n13, 2, n22);

      dg1->manager()->setRootNode(n01);

      gum::MultiDimFunctionGraph< double >* dg2
         = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();

      dg2->add(*v0);
      dg2->add(*v3);
      dg2->add(*v2);
      dg2->add(*v1);

      n01 = dg2->manager()->addInternalNode(v0);
      n02 = dg2->manager()->addInternalNode(v1);
      n03 = dg2->manager()->addInternalNode(v3);
      n04 = dg2->manager()->addTerminalNode(27.0957);
      n05 = dg2->manager()->addTerminalNode(8.01978);
      n06 = dg2->manager()->addInternalNode(v2);
      n07 = dg2->manager()->addInternalNode(v1);
      n08 = dg2->manager()->addInternalNode(v1);
      n09 = dg2->manager()->addTerminalNode(16.2307);
      n10 = dg2->manager()->addTerminalNode(30.7077);
      n11 = dg2->manager()->addTerminalNode(26.1679);
      n12 = dg2->manager()->addTerminalNode(84.2562);
      n13 = dg2->manager()->addTerminalNode(94.3744);
      n14 = dg2->manager()->addTerminalNode(0.666253);
      n15 = dg2->manager()->addTerminalNode(16.5207);
      n16 = dg2->manager()->addTerminalNode(55.5485);
      n17 = dg2->manager()->addTerminalNode(8.60651);
      n18 = dg2->manager()->addTerminalNode(81.2182);
      n19 = dg2->manager()->addTerminalNode(84.3831);
      n20 = dg2->manager()->addTerminalNode(6.12946);
      n21 = dg2->manager()->addTerminalNode(83.9071);
      n22 = dg2->manager()->addTerminalNode(7.85962);

      dg2->manager()->setSon(n01, 0, n02);
      dg2->manager()->setSon(n01, 1, n03);
      dg2->manager()->setSon(n01, 2, n04);

      dg2->manager()->setSon(n02, 0, n04);
      dg2->manager()->setSon(n02, 1, n05);
      dg2->manager()->setSon(n02, 2, n07);

      dg2->manager()->setSon(n03, 0, n02);
      dg2->manager()->setSon(n03, 1, n06);
      dg2->manager()->setSon(n03, 2, n02);

      dg2->manager()->setSon(n04, 0, n06);
      dg2->manager()->setSon(n04, 1, n05);

      dg2->manager()->setSon(n05, 0, n08);
      dg2->manager()->setSon(n05, 1, n09);
      dg2->manager()->setSon(n05, 2, n16);

      dg2->manager()->setSon(n06, 0, n07);
      dg2->manager()->setSon(n06, 1, n08);
      dg2->manager()->setSon(n06, 2, n11);

      dg2->manager()->setSon(n07, 0, n09);
      dg2->manager()->setSon(n07, 1, n10);

      dg2->manager()->setSon(n08, 0, n11);
      dg2->manager()->setSon(n08, 1, n12);
      dg2->manager()->setSon(n08, 2, n05);

      dg2->manager()->setSon(n09, 0, n12);
      dg2->manager()->setSon(n09, 1, n13);

      dg2->manager()->setSon(n12, 0, n17);
      dg2->manager()->setSon(n12, 1, n18);
      dg2->manager()->setSon(n12, 2, n19);

      dg2->manager()->setSon(n13, 0, n20);
      dg2->manager()->setSon(n13, 1, n21);
      dg2->manager()->setSon(n13, 2, n22);

      dg2->manager()->setRootNode(n01);

      double foo = 0.0, bar = 0.0;
      TS_GUM_ASSERT_THROWS_NOTHING(evalOperation__(1, dg1, dg2, foo, bar, 0.0));

      delete dg1;
      delete dg2;

      delete v0;
      delete v1;
      delete v2;
      delete v3;
      delete v4;
      delete v5;
      delete v6;
      delete v7;
      delete v8;
      delete v9;
    }
  };
}   // namespace gum_tests


// =================================================================================
// Code for doing random operation
// I let it here just in case
// =================================================================================
// for ( gum::Idx i = 0; i < 1; i++ ) {

//  time.reset();

//  gum::Sequence<const gum::DiscreteVariable*>* varList =
//      generateRandomVarList__( i + 1 );
//  shuffleVarList__( varList, i + 2 );

//  //          std::cout << std::endl << " gum::Sequence variable : ";
//  //          for( gum::SequenceIterator< const gum::DiscreteVariable* >
//  //          iter = varList->begin(); iter != varList->end(); ++iter )
//  //            std::cout << (*iter)->toString() << " | ";
//  //          std::cout << std::endl;

//  gum::MultiDimFunctionGraph<double>* a1 = nullptr;
//  TS_GUM_ASSERT_THROWS_NOTHING(
//      a1 = generateRandomFunctionGraph__( varList, i + 3 ) );
//  //          std::cout << a1->toDot() << std::endl;

//  shuffleVarList__( varList, i + 4 );

//  //          std::cout << std::endl << " gum::Sequence variable : ";
//  //          for( gum::SequenceIterator< const gum::DiscreteVariable* >
//  //          iter = varList->begin(); iter != varList->end(); ++iter )
//  //          std::cout << (*iter)->toString() << " | ";
//  //          std::cout << std::endl;

//  gum::MultiDimFunctionGraph<double>* a2 = nullptr;
//  TS_GUM_ASSERT_THROWS_NOTHING(
//      a2 = generateRandomFunctionGraph__( varList, i + 5 ) );
//  //          std::cout << a2->toDot();

//  tempsGene += time.step();

//  bool evalRes = true;

//  //          gum::Idx j = 1;
//  for ( gum::Idx nbOp = 1; nbOp < nbOperation && evalRes; nbOp++ )
//    TS_GUM_ASSERT_THROWS_NOTHING(
//        evalRes =
//            evalOperation__( nbOp, a1, a2, tempsCalcul, tempsEval ) );

//  delete a1;
//  delete a2;

//  for ( gum::SequenceIterator<const gum::DiscreteVariable*> ite =
//            varList->begin();
//        ite != varList->end();
//        ++ite )
//    delete *ite;
//  delete varList;

//  if ( !evalRes ) {
////          std::cout << "An error has occured! Aborting test." << std::endl;
//    break;
//  }

//  test_waiting( i );
//}

// end_test_waiting();
////      std::cout << std::endl
////                << "Temps Génération : " << tempsGene
////                << "s - Temps Calcul : " << tempsCalcul
////                << "s - Temps Evaluation " << tempsEval << "s " <<
/// std::endl;
//}
