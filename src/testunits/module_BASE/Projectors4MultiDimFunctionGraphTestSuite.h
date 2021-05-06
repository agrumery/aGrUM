/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


// ==========================================================================
#include <fstream>
// ==========================================================================
#include <gumtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
// ==========================================================================
#include <agrum/tools/core/math/math_utils.h>
#include <agrum/tools/core/timer.h>
// ==========================================================================
#include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>
#include <agrum/tools/multidim/implementations/multiDimFunctionGraphGenerator.h>
#include <agrum/tools/multidim/potential.h>
// ==========================================================================
#include <agrum/tools/variables/labelizedVariable.h>
// ==========================================================================

namespace gum_tests {

  class MultiDimProjectors4FunctionGraphTestSuite: public CxxTest::TestSuite {
    private:
    // ************************************************************************************************
    /// Génération fixe d'une liste de variable
    // ************************************************************************************************
    gum::Sequence< const gum::DiscreteVariable* >* generateFixVarList__() {
      gum::Sequence< const gum::DiscreteVariable* >* ret
         = new gum::Sequence< const gum::DiscreteVariable* >();
      ret->insert(new gum::LabelizedVariable("A", "", 2));
      ret->insert(new gum::LabelizedVariable("B", "", 2));
      ret->insert(new gum::LabelizedVariable("C", "", 2));
      ret->insert(new gum::LabelizedVariable("D", "", 4));

      return ret;
    }


    // ************************************************************************************************
    /// Génération aléatoire d'une liste de 10 variables
    // ************************************************************************************************
    gum::Sequence< const gum::DiscreteVariable* >* generateRandomVarList__(int i) {
      //        gum::Sequence< const gum::DiscreteVariable* >* ret = new
      //        gum::Sequence< const gum::DiscreteVariable* >();
      //        ret->insert ( new gum::LabelizedVariable ( "A", "", 3 ) );
      //        ret->insert ( new gum::LabelizedVariable ( "B", "", 5 ) );
      //        ret->insert ( new gum::LabelizedVariable ( "C", "", 2 ) );
      //        ret->insert ( new gum::LabelizedVariable ( "D", "", 4 ) );

      //        return ret;

      gum::Sequence< const gum::DiscreteVariable* >* ret
         = new gum::Sequence< const gum::DiscreteVariable* >();

      for (int j = 0; j < 10; j++) {
        std::stringstream varName;
        varName << "var" << j;
        ret->insert(new gum::LabelizedVariable(varName.str(), "", 2 + rand() % 4));
      }

      return ret;
    }


    // ************************************************************************************************
    /// Brassage aléatoire d'une liste de 10 variables
    // ************************************************************************************************
    void shuffleVarList__(gum::Sequence< const gum::DiscreteVariable* >* varList,
                          int                                            i) {
      for (int j = 0; j < 10; j++)
        varList->swap(rand() % (varList->size()), rand() % (varList->size()));
    }


    // ************************************************************************************************
    /// Génération fixe de diagramme de décision
    // ************************************************************************************************
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

      generatedFunctionGraph->manager()->clean();

      return generatedFunctionGraph;
    }


    // ************************************************************************************************
    /// Génération aléatoire de diagramme de décision
    // ************************************************************************************************
    gum::MultiDimFunctionGraph< double >* generateRandomFunctionGraph__(
       const gum::Sequence< const gum::DiscreteVariable* >* varList,
       double                                               lowLimit  = -100,
       double                                               highLimit = 100) {
      gum::MultiDimFunctionGraphGenerator gene(2, 5, *varList);

      return gene.generate();
    }


    // ************************************************************************************************
    /// Sauvegarde des diagrammes générant une erreur dans un fichier log
    // ************************************************************************************************
    void saveDiagrams__(gum::MultiDimFunctionGraph< double >*    a1,
                        gum::MultiDimFunctionGraph< double >*    a3,
                        gum::Set< const gum::DiscreteVariable* > delVars) {
      std::string   dotfile = GET_RESSOURCES_PATH("FunctionGraphError.log");
      std::ofstream output(dotfile.c_str(), std::ios::out);

      if (!output.good())
        GUM_ERROR(gum::IOError, "Stream states flags are not all unset.")

      output << std::endl;

      for (gum::SequenceIteratorSafe< const gum::DiscreteVariable* > ite
           = a1->variablesSequence().beginSafe();
           ite != a1->variablesSequence().endSafe();
           ++ite)
        output << (*ite)->toString() << " - ";

      output << std::endl;

      output << a1->toDot();

      if (a3 != nullptr) {
        output << std::endl;

        for (gum::SequenceIteratorSafe< const gum::DiscreteVariable* > ite
             = a3->variablesSequence().beginSafe();
             ite != a3->variablesSequence().endSafe();
             ++ite)
          output << (*ite)->toString() << " - ";

        output << std::endl;

        output << a3->toDot();
      }

      output << std::endl;

      for (gum::SetIteratorSafe< const gum::DiscreteVariable* > ite
           = delVars.beginSafe();
           ite != delVars.endSafe();
           ++ite)
        output << (*ite)->toString() << " - ";

      output << std::endl;

      output.flush();

      output.close();

      if (output.fail()) GUM_ERROR(gum::IOError, "Writting in the ostream failed.")
    }


    // ************************************************************************************************
    /// Evals given in parameter operation. Returned boolean parameter indicates
    /// if all went well or not
    // ************************************************************************************************
    bool evalOperation__(int                                      operationId,
                         gum::MultiDimFunctionGraph< double >*    a1,
                         gum::Set< const gum::DiscreteVariable* > del_vars,
                         double&                                  tempsCalcul,
                         double&                                  tempsEval,
                         double                                   delta = 0.01) {
      bool                                  hasNoError = true;
      gum::MultiDimFunctionGraph< double >* a3         = nullptr;

      gum::Timer timy;
      timy.reset();

      // ******************************************************************************************************
      // Generation du diagramme résultat

      switch (operationId) {
        case 1:   // Test Addition
          TS_GUM_ASSERT_THROWS_NOTHING(
             a3 = projectSumMultiDimFunctionGraph(a1, del_vars));
          break;

        case 2:   // Test Multiplication
          TS_GUM_ASSERT_THROWS_NOTHING(
             a3 = projectProductMultiDimFunctionGraph(a1, del_vars));
          break;

        case 3:   // Test Minimisation
          TS_GUM_ASSERT_THROWS_NOTHING(
             a3 = projectMinMultiDimFunctionGraph(a1, del_vars));
          break;

        case 4:   // Test Maximisation
          TS_GUM_ASSERT_THROWS_NOTHING(
             a3 = projectMaxMultiDimFunctionGraph(a1, del_vars));
          break;

        default:   // Should Not Happen
          GUM_ERROR(gum::OperationNotAllowed, "HEU .....")
      }

      // ******************************************************************************************************

      tempsCalcul += timy.step();

      timy.reset();

      // ******************************************************************************************************
      if (a3 != nullptr) {
        gum::Instantiation instEleminatedVar;
        for (gum::SequenceIteratorSafe< const gum::DiscreteVariable* > varIter
             = a1->variablesSequence().beginSafe();
             varIter != a1->variablesSequence().endSafe();
             ++varIter)
          instEleminatedVar.add(**varIter);
        for (gum::SetIteratorSafe< const gum::DiscreteVariable* > varIter
             = del_vars.beginSafe();
             varIter != del_vars.endSafe();
             ++varIter) {
          if (!instEleminatedVar.variablesSequence().exists(*varIter))
            TS_GUM_ASSERT_THROWS_NOTHING(instEleminatedVar.add(**varIter));
        }
        gum::Instantiation instRemainingVar;
        for (gum::SequenceIteratorSafe< const gum::DiscreteVariable* > varIter
             = a3->variablesSequence().beginSafe();
             varIter != a3->variablesSequence().endSafe();
             ++varIter)
          instRemainingVar.add(**varIter);


        for (instRemainingVar.setFirst(); !instRemainingVar.end() && hasNoError;
             ++instRemainingVar) {
          switch (operationId) {
            case 1: {   // Test Addition
              double sum = 0;

              for (instEleminatedVar.setVals(instRemainingVar);
                   !instEleminatedVar.end();
                   instEleminatedVar.incOut(instRemainingVar))
                sum += a1->get(instEleminatedVar);

              TS_ASSERT_DELTA(a3->get(instRemainingVar), sum, delta);

              if (std::abs(a3->get(instRemainingVar) - sum) >= delta) {
                std::cout << a3->get(instRemainingVar) << " - " << sum << " - "
                          << std::abs(a3->get(instRemainingVar) - sum) << " - "
                          << delta << std::endl;
                hasNoError = false;
              }
            }

            break;

            case 2: {   // Test Multiplication
                        // Hard to test because of Huge numbers
                        //                        double product = 1;

              //                        for ( instEleminatedVar.setVals (
              //                        instRemainingVar ); !
              //                        instEleminatedVar.end();
              //                        instEleminatedVar.incOut (
              //                        instRemainingVar ) )
              //                          product *= a1->get ( instEleminatedVar
              //                          );

              //                        TS_ASSERT_DELTA ( a3->get (
              //                        instRemainingVar ), product, delta );

              //                        if ( std::abs( a3->get (
              //                        instRemainingVar ) - product ) >= delta
              //                        )
              //                          hasNoError = false;
            }

            break;

            case 3: {   // Test Minimisation
              double min = std::numeric_limits< double >::max();

              for (instEleminatedVar.setVals(instRemainingVar);
                   !instEleminatedVar.end();
                   instEleminatedVar.incOut(instRemainingVar))
                min = min >= a1->get(instEleminatedVar)
                       ? a1->get(instEleminatedVar)
                       : min;

              TS_ASSERT_DELTA(a3->get(instRemainingVar), min, delta);

              if (std::abs(a3->get(instRemainingVar) - min) >= delta)
                hasNoError = false;
            }

            break;

            case 4: {   // Test Maximisation
              double max = -1 * (std::numeric_limits< double >::max());

              for (instEleminatedVar.setVals(instRemainingVar);
                   !instEleminatedVar.end();
                   instEleminatedVar.incOut(instRemainingVar))
                max = max >= a1->get(instEleminatedVar)
                       ? max
                       : a1->get(instEleminatedVar);

              TS_ASSERT_DELTA(a3->get(instRemainingVar), max, delta);

              if (std::abs(a3->get(instRemainingVar) - max) >= delta)
                hasNoError = false;
            }

            break;

            default:   // Should Not Happen
              GUM_ERROR(gum::OperationNotAllowed, "HEU .....")
          }
        }

        if (!hasNoError) { saveDiagrams__(a1, a3, del_vars); }

        delete a3;
      } else {
        saveDiagrams__(a1, a3, del_vars);
        hasNoError = false;
      }

      tempsEval += timy.step();

      return hasNoError;
    }


    public:
    // ************************************************************************************************
    /// Test sur les fonctions avec valeurs exactes
    // ************************************************************************************************
    void test_Projections_Functions_on_MultiDimFunctionGraphs() {
      gum::Timer time;
      double     tempsGene   = 0;
      double     tempsCalcul = 0;
      double     tempsEval   = 0;

      // *************************************************************************************
      // First we try with a predefine structure
      {
        time.reset();

        gum::Sequence< const gum::DiscreteVariable* >* varList
           = generateFixVarList__();

        gum::MultiDimFunctionGraph< double >* a1 = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING(a1 = generateFunctionGraph1__(varList));

        gum::Set< const gum::DiscreteVariable* > del_vars;
        del_vars << varList->atPos(0);
        //        del_vars << varList->atPos( 1 );

        tempsGene += time.step();

        bool evalRes = true;

        for (int operationId = 1; operationId < 5 && evalRes; operationId++)
          evalRes
             = evalOperation__(operationId, a1, del_vars, tempsCalcul, tempsEval);

        delete a1;

        del_vars.clear();

        for (gum::SequenceIteratorSafe< const gum::DiscreteVariable* > ite
             = varList->beginSafe();
             ite != varList->endSafe();
             ++ite)
          delete *ite;

        delete varList;

        if (!evalRes) return;
      }
    }


    //  ************************************************************************************************
    ///  Fonction de reproduction des tests amenant à des bugs/erreurs (rayer
    ///  les mentions inutiles)
    ///  pour débogage
    //  ************************************************************************************************
    void est_reproduction() {
      {
        gum::LabelizedVariable* vA = new gum::LabelizedVariable("A", "", 3);
        gum::LabelizedVariable* vB = new gum::LabelizedVariable("B", "", 5);
        gum::LabelizedVariable* vC = new gum::LabelizedVariable("C", "", 2);
        gum::LabelizedVariable* vD = new gum::LabelizedVariable("D", "", 4);

        gum::MultiDimFunctionGraph< double >* dg1
           = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();

        dg1->add(*vA);
        dg1->add(*vB);
        dg1->add(*vC);
        dg1->add(*vD);

        gum::NodeId n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12,
           n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26,
           n27, n28, n29, n30, n31, n32;


        n01 = dg1->manager()->addInternalNode(vA);
        n02 = dg1->manager()->addInternalNode(vC);
        n03 = dg1->manager()->addInternalNode(vB);
        n04 = dg1->manager()->addInternalNode(vB);
        n05 = dg1->manager()->addInternalNode(vC);
        n06 = dg1->manager()->addInternalNode(vC);
        n07 = dg1->manager()->addInternalNode(vD);
        n08 = dg1->manager()->addInternalNode(vD);
        n09 = dg1->manager()->addTerminalNode(31.7147);
        n10 = dg1->manager()->addTerminalNode(97.9935);
        n11 = dg1->manager()->addTerminalNode(8.98557);
        n12 = dg1->manager()->addTerminalNode(31.4826);
        n13 = dg1->manager()->addTerminalNode(39.8611);
        n14 = dg1->manager()->addTerminalNode(61.5625);
        n15 = dg1->manager()->addTerminalNode(14.4735);
        n16 = dg1->manager()->addTerminalNode(14.3695);
        n17 = dg1->manager()->addInternalNode(vD);
        n18 = dg1->manager()->addTerminalNode(41.5692);
        n19 = dg1->manager()->addTerminalNode(51.4208);
        n20 = dg1->manager()->addTerminalNode(38.6588);
        n21 = dg1->manager()->addTerminalNode(83.9144);
        n22 = dg1->manager()->addInternalNode(vC);
        n23 = dg1->manager()->addInternalNode(vD);
        n24 = dg1->manager()->addTerminalNode(70.573);
        n25 = dg1->manager()->addTerminalNode(53.3385);
        n26 = dg1->manager()->addTerminalNode(48.1635);
        n27 = dg1->manager()->addTerminalNode(74.1502);
        n28 = dg1->manager()->addInternalNode(vD);
        n29 = dg1->manager()->addTerminalNode(87.4298);
        n30 = dg1->manager()->addTerminalNode(45.782);
        n31 = dg1->manager()->addTerminalNode(83.9706);
        n32 = dg1->manager()->addTerminalNode(73.7171);

        dg1->manager()->setSon(n01, 0, n02);
        dg1->manager()->setSon(n01, 1, n03);
        dg1->manager()->setSon(n01, 2, n04);

        dg1->manager()->setSon(n02, 0, n28);
        dg1->manager()->setSon(n02, 1, n23);

        dg1->manager()->setSon(n03, 0, n06);
        dg1->manager()->setSon(n03, 1, n17);
        dg1->manager()->setSon(n03, 2, n05);
        dg1->manager()->setSon(n03, 3, n06);
        dg1->manager()->setSon(n03, 4, n22);

        dg1->manager()->setSon(n04, 0, n02);
        dg1->manager()->setSon(n04, 1, n05);
        dg1->manager()->setSon(n04, 2, n06);
        dg1->manager()->setSon(n04, 3, n02);
        dg1->manager()->setSon(n04, 4, n02);

        dg1->manager()->setSon(n05, 0, n07);
        dg1->manager()->setSon(n05, 1, n17);

        dg1->manager()->setSon(n06, 0, n07);
        dg1->manager()->setSon(n06, 1, n08);

        dg1->manager()->setSon(n07, 0, n13);
        dg1->manager()->setSon(n07, 1, n14);
        dg1->manager()->setSon(n07, 2, n15);
        dg1->manager()->setSon(n07, 3, n16);

        dg1->manager()->setSon(n08, 0, n09);
        dg1->manager()->setSon(n08, 1, n10);
        dg1->manager()->setSon(n08, 2, n11);
        dg1->manager()->setSon(n08, 3, n12);

        dg1->manager()->setSon(n17, 0, n18);
        dg1->manager()->setSon(n17, 1, n19);
        dg1->manager()->setSon(n17, 2, n20);
        dg1->manager()->setSon(n17, 3, n21);

        dg1->manager()->setSon(n22, 0, n07);
        dg1->manager()->setSon(n22, 1, n23);

        dg1->manager()->setSon(n23, 0, n24);
        dg1->manager()->setSon(n23, 1, n25);
        dg1->manager()->setSon(n23, 2, n26);
        dg1->manager()->setSon(n23, 3, n27);

        dg1->manager()->setSon(n28, 0, n29);
        dg1->manager()->setSon(n28, 1, n30);
        dg1->manager()->setSon(n28, 2, n31);
        dg1->manager()->setSon(n28, 3, n32);

        dg1->manager()->setRootNode(n01);

        gum::Set< const gum::DiscreteVariable* > del_vars;
        del_vars << vB;

        double a = 0.0, b = 0.0, c = 0.0;
        TS_GUM_ASSERT_THROWS_NOTHING(evalOperation__(1, dg1, del_vars, a, b, c));

        delete dg1;
        del_vars.clear();

        delete vA;
        delete vB;
        delete vC;
        delete vD;
      }

      {
        gum::LabelizedVariable* vA = new gum::LabelizedVariable("A", "", 3);
        gum::LabelizedVariable* vB = new gum::LabelizedVariable("B", "", 5);
        gum::LabelizedVariable* vC = new gum::LabelizedVariable("C", "", 2);
        gum::LabelizedVariable* vD = new gum::LabelizedVariable("D", "", 4);

        gum::MultiDimFunctionGraph< double >* dg1
           = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();

        dg1->add(*vA);
        dg1->add(*vB);
        dg1->add(*vC);
        dg1->add(*vD);

        gum::NodeId n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12,
           n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26,
           n27, n28, n29;


        n01 = dg1->manager()->addInternalNode(vA);
        n02 = dg1->manager()->addInternalNode(vC);
        n03 = dg1->manager()->addInternalNode(vB);
        n04 = dg1->manager()->addInternalNode(vB);
        n05 = dg1->manager()->addInternalNode(vC);
        n06 = dg1->manager()->addInternalNode(vC);
        n07 = dg1->manager()->addInternalNode(vC);
        n08 = dg1->manager()->addInternalNode(vC);
        n09 = dg1->manager()->addInternalNode(vD);
        n10 = dg1->manager()->addTerminalNode(45.89);
        n11 = dg1->manager()->addTerminalNode(15.0559);
        n12 = dg1->manager()->addTerminalNode(90.7523);
        n13 = dg1->manager()->addTerminalNode(34.7122);
        n14 = dg1->manager()->addInternalNode(vD);
        n15 = dg1->manager()->addTerminalNode(11.6741);
        n16 = dg1->manager()->addTerminalNode(38.1024);
        n17 = dg1->manager()->addTerminalNode(85.8589);
        n18 = dg1->manager()->addTerminalNode(31.6379);
        n19 = dg1->manager()->addTerminalNode(74.0838);
        n20 = dg1->manager()->addInternalNode(vD);
        n21 = dg1->manager()->addTerminalNode(76.801);
        n22 = dg1->manager()->addTerminalNode(58.4967);
        n23 = dg1->manager()->addTerminalNode(74.5338);
        n24 = dg1->manager()->addTerminalNode(67.0369);
        n25 = dg1->manager()->addInternalNode(vD);
        n26 = dg1->manager()->addTerminalNode(10.8237);
        n27 = dg1->manager()->addTerminalNode(36.3299);
        n28 = dg1->manager()->addTerminalNode(96.3603);
        n29 = dg1->manager()->addTerminalNode(31.1385);

        dg1->manager()->setSon(n01, 0, n02);
        dg1->manager()->setSon(n01, 1, n03);
        dg1->manager()->setSon(n01, 2, n04);

        dg1->manager()->setSon(n02, 0, n14);
        dg1->manager()->setSon(n02, 1, n25);

        dg1->manager()->setSon(n03, 0, n06);
        dg1->manager()->setSon(n03, 1, n09);
        dg1->manager()->setSon(n03, 2, n25);
        dg1->manager()->setSon(n03, 3, n08);
        dg1->manager()->setSon(n03, 4, n06);

        dg1->manager()->setSon(n04, 0, n05);
        dg1->manager()->setSon(n04, 1, n06);
        dg1->manager()->setSon(n04, 2, n07);
        dg1->manager()->setSon(n04, 3, n08);
        dg1->manager()->setSon(n04, 4, n09);

        dg1->manager()->setSon(n05, 0, n25);
        dg1->manager()->setSon(n05, 1, n20);

        dg1->manager()->setSon(n06, 0, n14);
        dg1->manager()->setSon(n06, 1, n25);

        dg1->manager()->setSon(n07, 0, n20);
        dg1->manager()->setSon(n07, 1, n14);

        dg1->manager()->setSon(n08, 0, n14);
        dg1->manager()->setSon(n08, 1, n15);

        dg1->manager()->setSon(n09, 0, n10);
        dg1->manager()->setSon(n09, 1, n11);
        dg1->manager()->setSon(n09, 2, n12);
        dg1->manager()->setSon(n09, 3, n13);

        dg1->manager()->setSon(n14, 0, n16);
        dg1->manager()->setSon(n14, 1, n17);
        dg1->manager()->setSon(n14, 2, n18);
        dg1->manager()->setSon(n14, 3, n19);

        dg1->manager()->setSon(n20, 0, n21);
        dg1->manager()->setSon(n20, 1, n22);
        dg1->manager()->setSon(n20, 2, n23);
        dg1->manager()->setSon(n20, 3, n24);

        dg1->manager()->setSon(n25, 0, n26);
        dg1->manager()->setSon(n25, 1, n27);
        dg1->manager()->setSon(n25, 2, n28);
        dg1->manager()->setSon(n25, 3, n29);

        dg1->manager()->setRootNode(n01);

        gum::Set< const gum::DiscreteVariable* > del_vars;
        del_vars << vD;
        del_vars << vB;
        del_vars << vC;

        double a = 0.0, b = 0.0, c = 0.01;
        TS_GUM_ASSERT_THROWS_NOTHING(evalOperation__(1, dg1, del_vars, a, b, c));

        delete dg1;
        del_vars.clear();

        delete vA;
        delete vB;
        delete vC;
        delete vD;
      }

      {
        gum::LabelizedVariable* v0 = new gum::LabelizedVariable("var0", "", 4);
        gum::LabelizedVariable* v1 = new gum::LabelizedVariable("var1", "", 3);
        gum::LabelizedVariable* v2 = new gum::LabelizedVariable("var2", "", 4);
        gum::LabelizedVariable* v3 = new gum::LabelizedVariable("var3", "", 4);
        gum::LabelizedVariable* v4 = new gum::LabelizedVariable("var4", "", 4);
        gum::LabelizedVariable* v5 = new gum::LabelizedVariable("var5", "", 5);
        gum::LabelizedVariable* v6 = new gum::LabelizedVariable("var6", "", 4);

        gum::MultiDimFunctionGraph< double >* dg1
           = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();

        dg1->add(*v0);
        dg1->add(*v1);
        dg1->add(*v2);
        dg1->add(*v3);
        dg1->add(*v4);
        dg1->add(*v5);

        gum::NodeId n01, n02, n03, n04, n05, n06, n07, n08, n09, n10, n11, n12,
           n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26,
           n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n38, n39, n40,
           n41, n42, n43, n44, n45, n46, n47, n48, n49, n50, n51, n52, n53, n54,
           n55, n56, n57, n58, n59, n60;


        n01 = dg1->manager()->addInternalNode(v0);
        n02 = dg1->manager()->addInternalNode(v1);
        n03 = dg1->manager()->addInternalNode(v1);
        n04 = dg1->manager()->addInternalNode(v1);
        n05 = dg1->manager()->addInternalNode(v1);
        n06 = dg1->manager()->addInternalNode(v2);
        n07 = dg1->manager()->addInternalNode(v2);
        n08 = dg1->manager()->addInternalNode(v2);
        n09 = dg1->manager()->addTerminalNode(84.5643);
        n10 = dg1->manager()->addInternalNode(v3);
        n11 = dg1->manager()->addInternalNode(v3);
        n12 = dg1->manager()->addTerminalNode(49.3108);
        n13 = dg1->manager()->addInternalNode(v4);
        n14 = dg1->manager()->addInternalNode(v4);
        n15 = dg1->manager()->addInternalNode(v4);
        n16 = dg1->manager()->addInternalNode(v4);
        n17 = dg1->manager()->addInternalNode(v5);
        n18 = dg1->manager()->addInternalNode(v5);
        n19 = dg1->manager()->addInternalNode(v5);
        n20 = dg1->manager()->addInternalNode(v5);
        n21 = dg1->manager()->addTerminalNode(55.5268);
        n22 = dg1->manager()->addTerminalNode(68.4431);
        n23 = dg1->manager()->addTerminalNode(56.7923);
        n24 = dg1->manager()->addTerminalNode(39.332);
        n25 = dg1->manager()->addTerminalNode(1.64187);
        n26 = dg1->manager()->addTerminalNode(36.5575);
        n27 = dg1->manager()->addTerminalNode(8.77858);
        n28 = dg1->manager()->addTerminalNode(72.7644);
        n29 = dg1->manager()->addTerminalNode(16.0111);
        n30 = dg1->manager()->addTerminalNode(32.4846);
        n31 = dg1->manager()->addTerminalNode(50.0024);
        n32 = dg1->manager()->addTerminalNode(46.7687);
        n33 = dg1->manager()->addTerminalNode(52.5437);
        n34 = dg1->manager()->addTerminalNode(36.8317);
        n35 = dg1->manager()->addTerminalNode(51.3657);
        n36 = dg1->manager()->addTerminalNode(68.1283);
        n37 = dg1->manager()->addTerminalNode(54.5789);
        n38 = dg1->manager()->addTerminalNode(79.6937);
        n39 = dg1->manager()->addTerminalNode(71.1486);
        n40 = dg1->manager()->addTerminalNode(72.4847);
        n41 = dg1->manager()->addInternalNode(v5);
        n42 = dg1->manager()->addTerminalNode(60.9934);
        n43 = dg1->manager()->addTerminalNode(98.8586);
        n44 = dg1->manager()->addTerminalNode(83.1984);
        n45 = dg1->manager()->addTerminalNode(77.9651);
        n46 = dg1->manager()->addTerminalNode(12.2614);
        n47 = dg1->manager()->addInternalNode(v5);
        n48 = dg1->manager()->addTerminalNode(72.9156);
        n49 = dg1->manager()->addTerminalNode(69.239);
        n50 = dg1->manager()->addTerminalNode(6.6246);
        n51 = dg1->manager()->addTerminalNode(54.4424);
        n52 = dg1->manager()->addTerminalNode(64.0095);
        n53 = dg1->manager()->addInternalNode(v5);
        n54 = dg1->manager()->addTerminalNode(67.1832);
        n55 = dg1->manager()->addTerminalNode(53.2715);
        n56 = dg1->manager()->addTerminalNode(5.94047);
        n57 = dg1->manager()->addTerminalNode(43.0718);
        n58 = dg1->manager()->addTerminalNode(37.3713);
        n59 = dg1->manager()->addInternalNode(v3);
        n60 = dg1->manager()->addInternalNode(v3);


        dg1->manager()->setSon(n01, 0, n02);
        dg1->manager()->setSon(n01, 1, n03);
        dg1->manager()->setSon(n01, 2, n04);
        dg1->manager()->setSon(n01, 3, n05);

        dg1->manager()->setSon(n02, 0, n10);
        dg1->manager()->setSon(n02, 1, n06);
        dg1->manager()->setSon(n02, 2, n60);

        dg1->manager()->setSon(n03, 0, n59);
        dg1->manager()->setSon(n03, 1, n08);
        dg1->manager()->setSon(n03, 2, n16);

        dg1->manager()->setSon(n04, 0, n19);
        dg1->manager()->setSon(n04, 1, n15);
        dg1->manager()->setSon(n04, 2, n13);

        dg1->manager()->setSon(n05, 0, n06);
        dg1->manager()->setSon(n05, 1, n07);
        dg1->manager()->setSon(n05, 2, n08);

        dg1->manager()->setSon(n06, 0, n19);
        dg1->manager()->setSon(n06, 1, n15);
        dg1->manager()->setSon(n06, 2, n11);
        dg1->manager()->setSon(n06, 3, n20);

        dg1->manager()->setSon(n07, 0, n47);
        dg1->manager()->setSon(n07, 1, n16);
        dg1->manager()->setSon(n07, 2, n20);
        dg1->manager()->setSon(n07, 3, n47);

        dg1->manager()->setSon(n08, 0, n09);
        dg1->manager()->setSon(n08, 1, n10);
        dg1->manager()->setSon(n08, 2, n11);
        dg1->manager()->setSon(n08, 3, n12);

        dg1->manager()->setSon(n10, 0, n14);
        dg1->manager()->setSon(n10, 1, n13);
        dg1->manager()->setSon(n10, 2, n20);
        dg1->manager()->setSon(n10, 3, n16);

        dg1->manager()->setSon(n11, 0, n13);
        dg1->manager()->setSon(n11, 1, n14);
        dg1->manager()->setSon(n11, 2, n15);
        dg1->manager()->setSon(n11, 3, n16);

        dg1->manager()->setSon(n13, 0, n53);
        dg1->manager()->setSon(n13, 1, n18);
        dg1->manager()->setSon(n13, 2, n19);
        dg1->manager()->setSon(n13, 3, n17);

        dg1->manager()->setSon(n14, 0, n19);
        dg1->manager()->setSon(n14, 1, n47);
        dg1->manager()->setSon(n14, 2, n20);
        dg1->manager()->setSon(n14, 3, n20);

        dg1->manager()->setSon(n15, 0, n41);
        dg1->manager()->setSon(n15, 1, n18);
        dg1->manager()->setSon(n15, 2, n18);
        dg1->manager()->setSon(n15, 3, n19);

        dg1->manager()->setSon(n16, 0, n17);
        dg1->manager()->setSon(n16, 1, n18);
        dg1->manager()->setSon(n16, 2, n19);
        dg1->manager()->setSon(n16, 3, n20);

        dg1->manager()->setSon(n17, 0, n36);
        dg1->manager()->setSon(n17, 1, n37);
        dg1->manager()->setSon(n17, 2, n38);
        dg1->manager()->setSon(n17, 3, n39);
        dg1->manager()->setSon(n17, 4, n40);

        dg1->manager()->setSon(n18, 0, n31);
        dg1->manager()->setSon(n18, 1, n32);
        dg1->manager()->setSon(n18, 2, n33);
        dg1->manager()->setSon(n18, 3, n34);
        dg1->manager()->setSon(n18, 4, n35);

        dg1->manager()->setSon(n19, 0, n26);
        dg1->manager()->setSon(n19, 1, n27);
        dg1->manager()->setSon(n19, 2, n28);
        dg1->manager()->setSon(n19, 3, n29);
        dg1->manager()->setSon(n19, 4, n30);

        dg1->manager()->setSon(n20, 0, n21);
        dg1->manager()->setSon(n20, 1, n22);
        dg1->manager()->setSon(n20, 2, n23);
        dg1->manager()->setSon(n20, 3, n24);
        dg1->manager()->setSon(n20, 4, n25);

        dg1->manager()->setSon(n41, 0, n42);
        dg1->manager()->setSon(n41, 1, n43);
        dg1->manager()->setSon(n41, 2, n44);
        dg1->manager()->setSon(n41, 3, n45);
        dg1->manager()->setSon(n41, 4, n46);

        dg1->manager()->setSon(n47, 0, n48);
        dg1->manager()->setSon(n47, 1, n49);
        dg1->manager()->setSon(n47, 2, n50);
        dg1->manager()->setSon(n47, 3, n51);
        dg1->manager()->setSon(n47, 4, n52);

        dg1->manager()->setSon(n53, 0, n54);
        dg1->manager()->setSon(n53, 1, n55);
        dg1->manager()->setSon(n53, 2, n56);
        dg1->manager()->setSon(n53, 3, n57);
        dg1->manager()->setSon(n53, 4, n58);

        dg1->manager()->setSon(n59, 0, n13);
        dg1->manager()->setSon(n59, 1, n13);
        dg1->manager()->setSon(n59, 2, n17);
        dg1->manager()->setSon(n59, 3, n19);

        dg1->manager()->setSon(n60, 0, n19);
        dg1->manager()->setSon(n60, 1, n16);
        dg1->manager()->setSon(n60, 2, n15);
        dg1->manager()->setSon(n60, 3, n14);

        dg1->manager()->setRootNode(n01);

        gum::Set< const gum::DiscreteVariable* > del_vars;
        del_vars << v4;
        del_vars << v2;
        del_vars << v5;
        del_vars << v6;
        del_vars << v0;
        del_vars << v1;

        double a = 0.0, b = 0.0, c = 0.01;
        TS_GUM_ASSERT_THROWS_NOTHING(evalOperation__(1, dg1, del_vars, a, b, c));

        delete dg1;
        del_vars.clear();

        delete v0;
        delete v1;
        delete v2;
        delete v3;
        delete v4;
        delete v5;
        delete v6;
      }
    }
  };
}   // namespace gum_tests


// ===================================================================================
// Code for testing on random structure
// I let it here just in case
// ===================================================================================


// *************************************************************************************
// Then we try with random structure
//      for ( int nbLoop = 0; nbLoop < 1; nbLoop++ ) {

//        time.reset();

//        gum::Sequence<const gum::DiscreteVariable*>* varList =
//            generateRandomVarList__( nbLoop + 1 );

//        gum::MultiDimFunctionGraph<double>* a1 = nullptr;
//        TS_GUM_ASSERT_THROWS_NOTHING(
//            a1 = generateRandomFunctionGraph__( varList, nbLoop + 2 ) );

//        gum::Idx maxSwapVar = ( rand() % ( varList->size() - 2 ) ) + 1;

//        gum::Set<const gum::DiscreteVariable*> del_vars;
//        bool notOk = true;
//        while ( notOk ) {
//          for ( gum::Idx i = 0; i < maxSwapVar; ++i )
//            del_vars << varList->atPos( rand() % varList->size() );
//          gum::Idx theNumber = 0;
//          for ( gum::SetIteratorSafe<const gum::DiscreteVariable*> varIter =
//                    del_vars.beginSafe();
//                varIter != del_vars.endSafe();
//                ++varIter )
//            if ( a1->variablesSequence().exists( *varIter ) ) theNumber++;
//          if ( theNumber != a1->variablesSequence().size() &&
//               del_vars.size() > 0 )
//            notOk = false;
//        }

//        //        std::cout << a1->toDot() << std::endl;
//        //        for( gum::SetIteratorSafe<const gum::DiscreteVariable*>
//        //        varIter = del_vars.beginSafe(); varIter !=
//        del_vars.endSafe();
//        //        ++varIter )
//        //            std::cout << (*varIter)->name() << "     ";
//        //        std::cout << std::endl;

//        tempsGene += time.step();

//        bool evalRes = true;

//        //        int operationId = 1;
//        for ( int operationId = 1; operationId < 5 && evalRes; operationId++ )
//          evalRes = evalOperation__(
//              operationId, a1, del_vars, tempsCalcul, tempsEval );

//        delete a1;

//        del_vars.clear();

//        for ( gum::SequenceIteratorSafe<const gum::DiscreteVariable*> ite =
//                  varList->beginSafe();
//              ite != varList->endSafe();
//              ++ite )
//          delete *ite;

//        delete varList;

//        if ( !evalRes ) {
//          std::cout << "An error has occured! Aborting test." << std::endl;
//          return;
//        }

//        if ( nbLoop % 5 == 0 ) test_waiting( nbLoop / 5 );
//      }

//      end_test_waiting();
//      std::cout << std::endl
//                << "Temps Génération : " << tempsGene
//                << "s - Temps Calcul : " << tempsCalcul
//                << "s - Temps Evaluation " << tempsEval << "s " << std::endl;
