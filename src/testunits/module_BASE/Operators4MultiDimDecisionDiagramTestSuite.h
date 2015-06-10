
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

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include "../../agrum/core/sequence.h"

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/core/algorithms/approximationPolicy/approximationPolicy.h>
#include <agrum/core/algorithms/approximationPolicy/linearApproximationPolicy.h>

#include <agrum/multidim/potential.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimDecisionDiagram.h>
#include <agrum/multidim/multiDimDecisionDiagramFactory.h>

namespace gum_tests {

  class Operators4MultiDimDecisionDiagramTestSuite : public CxxTest::TestSuite {

    private:
    /// Defines the maximal number of modalities for a var (which is choose
    /// randomly). The domain size will be of 2 + rand()%maxVarDomainSize.
    static const gum::Idx maxVarDomainSize = 3;

    /// Defines the minimal and maximal number of var in a diagram.
    static const gum::Size minNbVarInDiagram = 7, maxNbVarInDiagram = 10;

    /// Defines how many different operations we perform (from 2 to 5 )
    static const gum::Idx nbOperation = 2;

    /// Defines the number of iteration on randomly generated diagram we perform
    static const int nbIter = 100;

    // Génération fixe d'une liste de variable

    gum::Sequence<const gum::DiscreteVariable *> *__generateFixVarList() {

      gum::Sequence<const gum::DiscreteVariable *> *ret =
          new gum::Sequence<const gum::DiscreteVariable *>();
      ret->insert(new gum::LabelizedVariable("A", "", 2));
      ret->insert(new gum::LabelizedVariable("B", "", 2));
      ret->insert(new gum::LabelizedVariable("C", "", 2));

      return ret;
    }

    // Génération aléatoire d'une liste de 10 variables

    gum::Sequence<const gum::DiscreteVariable *> *__generateRandomVarList(int i) {
      gum::Sequence<const gum::DiscreteVariable *> *ret =
          new gum::Sequence<const gum::DiscreteVariable *>();

      for (int j = 0; j < 10; j++) {
        std::stringstream varName;
        varName << "var" << j;
        ret->insert(new gum::LabelizedVariable(varName.str(), "",
                                               2 + rand() % maxVarDomainSize));
      }

      return ret;
    }

    // Brassage aléatoire d'une liste de 10 variables

    void __shuffleVarList(gum::Sequence<const gum::DiscreteVariable *> *varList,
                          int i) {
      for (int j = 0; j < 10; j++)
        varList->swap(rand() % (varList->size()), rand() % (varList->size()));
    }

    // Génération fixe de diagramme de décision
    gum::MultiDimDecisionDiagramBase<double> *__generateDecisionDiagram1(
        const gum::Sequence<const gum::DiscreteVariable *> *varList,
        gum::MultiDimDecisionDiagramFactoryBase<double> *facto = nullptr) {

      gum::MultiDimDecisionDiagramBase<double> *ret = nullptr;
      bool factoryCreatedHere = false;

      if (facto == nullptr) {
        factoryCreatedHere = true;
        facto = new gum::MultiDimDecisionDiagramFactory<double>();
      }

      facto->setVariablesSequence(*varList);

      gum::NodeId a = facto->addNonTerminalNode(varList->atPos(0));
      gum::NodeId b = facto->addNonTerminalNode(varList->atPos(1));
      gum::NodeId c = facto->addNonTerminalNode(varList->atPos(2));

      gum::NodeId d = facto->addTerminalNode(6);
      gum::NodeId e = facto->addTerminalNode(2);
      gum::NodeId g = facto->addTerminalNode(3);

      facto->addArc(a, b, 0);
      facto->addArc(a, c, 1);

      facto->addArc(b, d, 0);
      facto->addArc(b, c, 1);

      facto->addArc(c, e, 0);
      facto->addArc(c, g, 1);

      ret = facto->getMultiDimDecisionDiagram();

      if (factoryCreatedHere)
        delete facto;

      return ret;
    }

    // Génération fixe de diagramme de décision

    gum::MultiDimDecisionDiagramBase<double> *__generateDecisionDiagram2(
        const gum::Sequence<const gum::DiscreteVariable *> *varList,
        gum::MultiDimDecisionDiagramFactoryBase<double> *facto = nullptr) {

      gum::MultiDimDecisionDiagramBase<double> *ret = nullptr;
      bool factoryCreatedHere = false;

      if (facto == nullptr) {
        factoryCreatedHere = true;
        facto = new gum::MultiDimDecisionDiagramFactory<double>();
      }

      facto->setVariablesSequence(*varList);

      gum::NodeId a = facto->addNonTerminalNode(varList->atPos(0));
      gum::NodeId c = facto->addNonTerminalNode(varList->atPos(2));

      gum::NodeId d = facto->addTerminalNode(4);
      gum::NodeId e = facto->addTerminalNode(5);
      gum::NodeId g = facto->addTerminalNode(1);

      facto->addArc(a, d, 0);
      facto->addArc(a, c, 1);

      facto->addArc(c, e, 0);
      facto->addArc(c, g, 1);

      ret = facto->getMultiDimDecisionDiagram();

      if (factoryCreatedHere)
        delete facto;

      return ret;
    }

    // Génération aléatoire de diagramme de décision

    gum::MultiDimDecisionDiagramBase<double> *__generateRandomdoubleDecisionDiagram(
        const gum::Sequence<const gum::DiscreteVariable *> *varList, int i,
        gum::MultiDimDecisionDiagramFactoryBase<double> *f = nullptr,
        double lowLimit = -100, double highLimit = 100) {

      gum::MultiDimDecisionDiagramBase<double> *ret = nullptr;
      bool factoryCreatedHere = false;

      while (ret == nullptr || (ret->diagramVarSize() < minNbVarInDiagram) ||
             (ret->diagramVarSize() > maxNbVarInDiagram)) {

        if (ret != nullptr)
          delete ret;

        if (f == nullptr) {
          factoryCreatedHere = true;
          f = new gum::MultiDimDecisionDiagramFactory<double>();
        }

        f->clear();

        f->setVariablesSequence(*varList);

        // ***********************************************************************
        // Creation of table var - list of associated node id in the diagram
        // with initialization of those lists
        gum::HashTable<const gum::DiscreteVariable *, gum::List<gum::NodeId> *>
            var2NodeIdMap;

        for (gum::Size numVar = 0; numVar < (*varList).size(); ++numVar)
          var2NodeIdMap.insert((*varList)[numVar], new gum::List<gum::NodeId>());

        // ***********************************************************************
        // Creation of a list containing terminal node possible value
        gum::List<double> tnList;

        double interval = highLimit - lowLimit;

        gum::Idx i = 0;

        while (i < 25) {
          double newVal = (rand() % ((int)interval)) - interval / 2;
          tnList.insert(newVal);
          i++;
        }

        // ***********************************************************************
        // Selection (randomly) of the root var in the diagram
        // all var that are before this one won't be in the diagram
        // for order on var purpose
        gum::Size initVar = rand() % (varList->size() / 2);

        // ***********************************************************************
        // So for all remaining variable
        for (gum::Size numVar = initVar; numVar < varList->size(); ++numVar) {

          // if this is the root we add it
          if (numVar == initVar)
            var2NodeIdMap[(*varList)[initVar]]->insert(
                f->unsafeAddNonTerminalNode((*varList)[initVar]));

          // then we check if another variable goes on it
          //(or if this is the root). It's done implicitly by the size of the list of
          // node associated to that var.
          // if not we move on
          // else
          if (!var2NodeIdMap[(*varList)[numVar]]->empty()) {
            // for each node associated to that var
            for (const auto numNode : *var2NodeIdMap[(*varList)[numVar]]) {

              // we determine a random number of arc starting from that node
              gum::Idx nbArc = 1 + rand() % ((*varList)[numVar]->domainSize());

              // for those arcs
              for (gum::Idx label = 0; label < nbArc; label++) {

                // We first determine if it goes directly to a terminal node or node
                if (numVar != (varList->size() - 1) && (rand() % 100) >= 42) {

                  // if not, we determine randomly to which var it could go
                  gum::NodeId indToVar =
                      (rand() % (varList->size() - 1 - numVar)) + numVar + 1;
                  const gum::DiscreteVariable *toVar = (*varList)[indToVar];
                  // // std::cout << "Variable : " << (*varList)[ numVar
                  // ]->toString() << " d'indice : " << numVar << " se lie à " <<
                  // toVar->toString() << " d'indice : " << indToVar << std::endl;

                  // then we take the list of  node id associated to that var and
                  // we determine randomly if we add another node to that list or not
                  gum::NodeId desiredNode = (gum::NodeId)rand() % 5;

                  if (desiredNode >= var2NodeIdMap[toVar]->size()) {
                    var2NodeIdMap[toVar]->insert(f->unsafeAddNonTerminalNode(toVar));
                    desiredNode = var2NodeIdMap[toVar]->size() - 1;
                  }

                  // then we add an arc between our current var associated node id
                  // and the considered var random node id
                  f->unsafeAddArc(numNode, (*var2NodeIdMap[toVar])[desiredNode],
                                  label);

                } else {
                  // if we add
                  gum::NodeId toVal =
                      f->addTerminalNode(tnList[rand() % tnList.size()]);
                  f->unsafeAddArc(numNode, toVal, label);
                }
              }
            }
          }
        }

        for (const auto &elt : var2NodeIdMap)
          delete elt.second;

        ret = f->getMultiDimDecisionDiagram(true, 0, true);
      }

      if (factoryCreatedHere)
        delete f;

      return ret;
    }

    // Sauvegarde des diagrammes générant une erreur dans un fichier log

    void __saveDiagrams(gum::MultiDimDecisionDiagramBase<double> *a1,
                        gum::MultiDimDecisionDiagramBase<double> *a2,
                        gum::MultiDimDecisionDiagramBase<double> *a3) {

      std::string dotfile = GET_PATH_STR("DecisionDiagramError.log");
      std::ofstream output(dotfile.c_str(), std::ios::out);

      if (!output.good())
        GUM_ERROR(gum::IOError, "Stream states flags are not all unset.");

      output << std::endl;

      for (const auto var : a1->variablesSequence())
        output << var->toString() << " - ";

      output << std::endl;
      output << a1->toDot();
      output << std::endl;

      for (const auto var : a2->variablesSequence())
        output << var->toString() << " - ";

      output << std::endl;
      output << a2->toDot();

      if (a3 != nullptr) {
        output << std::endl;

        for (const auto var : a3->variablesSequence())
          output << var->toString() << " - ";

        output << std::endl;
        output << a3->toDot();
      }

      output.flush();
      output.close();

      if (output.fail())
        GUM_ERROR(gum::IOError, "Writting in the ostream failed.");
    }

    // Evals  given in parameter operation. Returned boolean parameter indicates if
    // all went well or not

    bool __evalOperation(gum::Idx operationId,
                         gum::MultiDimDecisionDiagramBase<double> *a1,
                         gum::MultiDimDecisionDiagramBase<double> *a2,
                         double &tempsCalcul, double &tempsEval,
                         double delta = 0.01) {

      bool hasNoError = true;
      gum::MultiDimDecisionDiagramBase<double> *a3 = nullptr;

      gum::Timer timy;
      timy.reset();

      // ******************************************************************************************************
      // Generation du diagramme résultat

      switch (operationId) {
        case 1: // Test addition
          TS_GUM_ASSERT_THROWS_NOTHING(a3 = add2MultiDimDecisionDiagrams(a1, a2));
          break;

        case 2: // Test Substraction
          TS_GUM_ASSERT_THROWS_NOTHING(
              a3 = subtract2MultiDimDecisionDiagrams(a1, a2));
          break;

        case 3: // Test Multiplication
          TS_GUM_ASSERT_THROWS_NOTHING(
              a3 = multiply2MultiDimDecisionDiagrams(a1, a2));
          break;

        case 4: // Test Maximum
          TS_GUM_ASSERT_THROWS_NOTHING(
              a3 = maximize2MultiDimDecisionDiagrams(a1, a2));
          break;

        default:
          GUM_ERROR(gum::OperationNotAllowed, "HEU .....");
      }

      // ******************************************************************************************************

      tempsCalcul += timy.step();

      //  std::cout << a3->toDot() << std::endl;

      timy.reset();

      // ******************************************************************************************************
      if (a3 != nullptr) {
        gum::Instantiation inst(a3);

        for (inst.setFirst(); !inst.end() && hasNoError; ++inst) {

          switch (operationId) {
            case 1: // Test addition
              TS_ASSERT_DELTA(a3->get(inst), a1->get(inst) + a2->get(inst), delta);

              if (a3->get(inst) != a1->get(inst) + a2->get(inst)) {
                std::cout << "Instantiation : " << inst.toString() << std::endl;
                hasNoError = false;
              }

              break;

            case 2: // Test Substraction
              TS_ASSERT_DELTA(a3->get(inst), a1->get(inst) - a2->get(inst), delta);

              if (a3->get(inst) != a1->get(inst) - a2->get(inst))
                hasNoError = false;

              break;

            case 3: // Test Multiplication
              TS_ASSERT_DELTA(a3->get(inst), a1->get(inst) * a2->get(inst), delta);

              if (a3->get(inst) != a1->get(inst) * a2->get(inst))
                hasNoError = false;

              break;

            case 4: // Test Maximum
              TS_ASSERT_DELTA(a3->get(inst),
                              a1->get(inst) > a2->get(inst) ? a1->get(inst)
                                                            : a2->get(inst),
                              delta);

              if (a3->get(inst) !=
                  (a1->get(inst) > a2->get(inst) ? a1->get(inst) : a2->get(inst)))
                hasNoError = false;

              break;

            default:
              GUM_ERROR(gum::OperationNotAllowed, "HEU .....");
          }
        }

        if (!hasNoError)
          __saveDiagrams(a1, a2, a3);

        delete a3;
      } else {
        __saveDiagrams(a1, a2, a3);
        hasNoError = false;
      }

      tempsEval += timy.step();

      return hasNoError;
    }

    public:
    //
    // Test sur les fonctions avec valeurs exactes
    //

    void test_Operators_Functions_on_MultiDimDecisionDiagrams() {

      gum::Timer time;
      double tempsGene = 0;
      double tempsCalcul = 0;
      double tempsEval = 0;

      // First we try with a predefine structure

      //         {
      //         time.reset();
      //
      //         gum::Sequence< const gum::DiscreteVariable* >* varList =
      //         __generateFixVarList();
      //
      //         gum::MultiDimDecisionDiagramBase<double>* a1 = nullptr;
      //         TS_GUM_ASSERT_THROWS_NOTHING ( a1 = __generateDecisionDiagram1 (
      //         varList ) );
      //
      //         gum::MultiDimDecisionDiagramBase<double>* a2 = nullptr;
      //         TS_GUM_ASSERT_THROWS_NOTHING ( a2 = __generateDecisionDiagram2 (
      //         varList ) );
      //
      //  std::cout << a1->toDot() << std::endl;
      //
      //  std::cout << a2->toDot() << std::endl;
      //
      //         tempsGene += time.step();
      //
      //         bool evalRes = true;
      //
      //         for ( gum::Idx i = 1; i < nbOperation && evalRes; i++ )
      //           evalRes = __evalOperation ( i, a1, a2, tempsCalcul, tempsEval );
      //
      //         delete a1;
      //
      //         delete a2;
      //
      //         for ( gum::SequenceIteratorSafe< const gum::DiscreteVariable*> ite =
      //         varList->begin(); ite != varList->end(); ++ite )
      //           delete *ite;
      //
      //         delete varList;
      //
      //         if ( !evalRes ) {
      //           std::cout << "An error has occured! Aborting test." <<std::endl;
      //           return;
      //         }
      //
      //         std::cout << ".";
      //
      //         std::fflush ( stdout );
      //         }

      // Then we try with random structure

      //

      for (int i = 0; i < nbIter; i++) {

        time.reset();

        gum::Sequence<const gum::DiscreteVariable *> *varList =
            __generateRandomVarList(i + 1);
        __shuffleVarList(varList, i + 2);

        //             std::cout << std::endl << " gum::Sequence variable : ";
        //             for( gum::SequenceIteratorSafe< const gum::DiscreteVariable* >
        //             iter = varList->begin(); iter != varList->end(); ++iter )
        //        std::cout << (*iter)->toString() << " | ";
        //             std::cout << std::endl;

        gum::MultiDimDecisionDiagramBase<double> *a1 = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING(
            a1 = __generateRandomdoubleDecisionDiagram(varList, i + 3));
        //             std::cout << a1->toDot();

        //             __shuffleVarList ( varList, i + 4 );

        //             std::cout << std::endl << " gum::Sequence variable : ";
        //             for( gum::SequenceIteratorSafe< const gum::DiscreteVariable* >
        //             iter = varList->begin(); iter != varList->end(); ++iter )
        //        std::cout << (*iter)->toString() << " | ";
        //             std::cout << std::endl;

        gum::MultiDimDecisionDiagramBase<double> *a2 = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING(
            a2 = __generateRandomdoubleDecisionDiagram(varList, i + 5));
        //             std::cout << a2->toDot();

        tempsGene += time.step();

        bool evalRes = true;

        for (gum::Idx j = 1; j < nbOperation && evalRes; j++)
          TS_GUM_ASSERT_THROWS_NOTHING(
              evalRes = __evalOperation(j, a1, a2, tempsCalcul, tempsEval));

        delete a1;
        delete a2;

        for (const auto var : *varList)
          delete var;

        delete varList;

        if (!evalRes) {
          std::cout << "An error has occured! Aborting test." << std::endl;
          break;
        }

        test_waiting(i);
      }

      end_test_waiting();

      // std::cout << std::endl << "Temps Génération : " << tempsGene << "s - Temps
      // Calcul : " << tempsCalcul << "s - Temps Evaluation " << tempsEval << "s " <<
      // std::endl;
    }

    //
    // Test sur les opérateurs avec valeurs exactes
    //

    void test_Operators_on_MultiDimDecisionDiagrams() {
      gum::operators4MultiDimInit<double>();

      // First we try with a predefine structure

      gum::MultiDimDecisionDiagram<double> a1, a2;
      gum::MultiDimDecisionDiagramBase<double> *a3 = nullptr;

      // Test addition
      TS_GUM_ASSERT_THROWS_NOTHING(
          a3 = (gum::MultiDimDecisionDiagramBase<double> *)(a1 + a2));

      if (a3 != nullptr)
        delete a3;

      // Test subtraction
      TS_GUM_ASSERT_THROWS_NOTHING(
          a3 = (gum::MultiDimDecisionDiagramBase<double> *)(a1 - a2));

      if (a3 != nullptr)
        delete a3;

      // Test multiplication
      TS_GUM_ASSERT_THROWS_NOTHING(
          a3 = (gum::MultiDimDecisionDiagramBase<double> *)(a1 * a2));

      if (a3 != nullptr)
        delete a3;

      // Test division
      TS_GUM_ASSERT_THROWS_NOTHING(
          a3 = (gum::MultiDimDecisionDiagramBase<double> *)(a1 / a2));

      if (a3 != nullptr)
        delete a3;
    }

    //
    // Test sur les fonctions avec approximation
    //

    void _Operation_in_MultiDimDecisionDiagrams_With_LinearApproximation() {

      // First we try with a predefine structure

      //       gum::Sequence< const gum::DiscreteVariable* >* varList =
      //       __generateFixVarList();
      //
      //       gum::MultiDimDecisionDiagramFactory<double,LinearApproximationPolicy>
      //       f;
      //       f.setEpsilon( 0.9 );
      //       f.setLowLimit( 0 );
      //       f.setHighLimit( 4.5 );
      //
      //       gum::MultiDimDecisionDiagramBase<double>* a1 = nullptr;
      //       TS_GUM_ASSERT_THROWS_NOTHING( a1 = __generateDecisionDiagram1(varList,
      //       &f ) );
      //
      //       f.clear();
      //       f.setEpsilon( 1.3 );
      //       f.setLowLimit( 3.5 );
      //       f.setHighLimit( 9 );
      //
      //       gum::MultiDimDecisionDiagramBase<double>* a2 = nullptr;
      //       TS_GUM_ASSERT_THROWS_NOTHING( a2 = __generateDecisionDiagram2(varList,
      //       &f ) );
      //
      //       bool evalRes = true;
      //
      //       for( int i = 1; i < 5 && evalRes; i++)
      //    evalRes = __evalOperation( i, a1, a2, 0.9 );
      //
      //       delete a1;
      //       delete a2;
      //
      //       for( gum::SequenceIteratorSafe< const gum::DiscreteVariable*> ite =
      //       varList->begin(); ite != varList->end(); ++ite )
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
      //    TS_GUM_ASSERT_THROWS_NOTHING( a1 =
      //    __generateRandomdoubleDecisionDiagram(varList, i + 3, &f ) );
      //
      //    __shuffleVarList( varList, i + 4 );
      //    f.clear();
      //    f.setEpsilon( 11 );
      //    f.setLowLimit( -53 );
      //    f.setHighLimit( 79 );
      //
      //    TS_GUM_ASSERT_THROWS_NOTHING( a2 =
      //    __generateRandomdoubleDecisionDiagram(varList, i + 5, &f ) );
      //
      //    evalRes = true;
      //
      //    for( int j = 1; j < 5 && evalRes; j++)
      //        TS_GUM_ASSERT_THROWS_NOTHING( evalRes = __evalOperation( j, a1, a2, 7
      //        ) );
      //
      //    delete a1;
      //    delete a2;
      //
      //    for( gum::SequenceIteratorSafe< const gum::DiscreteVariable*> ite =
      //    varList->begin(); ite != varList->end(); ++ite )
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
    // For debug purpose only, the aim of this function is to reproduce specific
    // diagrams in order to find out where the hell the matter is
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


            gum::MultiDimDecisionDiagramFactory<double> facto;

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
            for( gum::SequenceIteratorSafe< const gum::DiscreteVariable* > iter =
         seq.begin(); iter != seq.end(); ++iter )
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

            gum::MultiDimDecisionDiagramBase<double>* a1 =
         facto.getMultiDimDecisionDiagram();
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
            for( gum::SequenceIteratorSafe< const gum::DiscreteVariable* > iter =
         seq.begin(); iter != seq.end(); ++iter )
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

            gum::MultiDimDecisionDiagramBase<double>* a2 =
         facto.getMultiDimDecisionDiagram();
            // std::cout << std::endl << a2->toDot() << std::endl;

            bool evalRes = true;
            double fuck = 0.0, you = 0.0;
            TS_GUM_ASSERT_THROWS_NOTHING( evalRes = __evalOperation( 1, a1,
         a2,fuck,you,0.0 ) );

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
