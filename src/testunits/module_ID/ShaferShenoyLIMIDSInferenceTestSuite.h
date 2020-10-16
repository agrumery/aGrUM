
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/ID/generator/influenceDiagramGenerator.h>
#include <agrum/ID/inference/ShaferShenoyLIMIDSInference.h>
#include <agrum/ID/influenceDiagram.h>
#include <agrum/ID/io/BIFXML/BIFXMLIDWriter.h>
#include <agrum/tools/graphs/graphElements.h>
#include <agrum/tools/variables/discreteVariable.h>
#include <agrum/tools/variables/labelizedVariable.h>

// The graph used for the tests:
//           D1
//           |   D1 -> C1
//           C1
//          /  \  C1 -> C2 & C1 -> U1
//        C2   U1
//       /  \ /  C2 -> D3 & C2 -> D2 & D2 -> U1
//      D3  D2
//      |    |  D3 -> C3 & D2 -> C4
//     C3    C4
//       \  /  C3 -> C5 & C4 -> C5
//        C5   D4
//         \  /  C5 -> U2 & D4 -> U2
//          U2

namespace gum_tests {

  class ShaferShenoyLIMIDSInferenceTestSuite: public CxxTest::TestSuite {
    private:
    void fillTopoOilWildcater(gum::InfluenceDiagram< double >& id,
                              gum::List< gum::NodeId >&        idList) const {
      try {
        idList.insert(id.addDecisionNode(*TestVar));             // 0
        idList.insert(id.addDecisionNode(*DrillVar));            // 1
        idList.insert(id.addChanceNode(*ResultTestVar));         // 2
        idList.insert(id.addChanceNode(*OilAmountVar));          // 3
        idList.insert(id.addUtilityNode(*TestUtilityVar));       // 4
        idList.insert(id.addUtilityNode(*DrillingUtilityVar));   // 5

        id.addArc(idList[0], idList[4]);
        id.addArc(idList[0], idList[2]);
        id.addArc(idList[0], idList[1]);
        id.addArc(idList[1], idList[5]);
        id.addArc(idList[2], idList[1]);
        id.addArc(idList[3], idList[2]);
        id.addArc(idList[3], idList[5]);

      } catch (gum::Exception& e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    void populateOilWildcater(gum::InfluenceDiagram< double >& id,
                              gum::List< gum::NodeId >&        idList) {
      fillTopoOilWildcater(id, idList);

      try {
        id.cpt(idList[3]).populate({0.5f, 0.3f, 0.2f});
        id.cpt(idList[2]).populate(   // clang-format off
                              {0.6f, 0.3f, 0.1f, 0,
                               0  , 0  , 0  , 1,
                               0.3f, 0.4f, 0.3f, 0,
                               0  , 0  , 0  , 1,
                               0.1f, 0.4f, 0.5f, 0,
                               0  , 0  , 0  , 1});   // clang-format on
        id.utility(idList[4]).populate({-10, 0});
        id.utility(idList[5]).populate({-70, 0, 50, 0, 200, 0});
      } catch (gum::Exception& e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    void fillTopoDecAsia(gum::InfluenceDiagram< double >& id,
                         gum::List< gum::NodeId >&        idList) const {
      try {
        idList.insert(id.addDecisionNode(*HospitalizeVar));             // 0
        idList.insert(id.addDecisionNode(*TakeXRayVar));                // 1
        idList.insert(id.addChanceNode(*SmokingVar));                   // 2
        idList.insert(id.addChanceNode(*BronchitisVar));                // 3
        idList.insert(id.addChanceNode(*LungCancerVar));                // 4
        idList.insert(id.addChanceNode(*EitherVar));                    // 5
        idList.insert(id.addChanceNode(*DyspnoeaVar));                  // 6
        idList.insert(id.addChanceNode(*PositiveXRayVar));              // 7
        idList.insert(id.addChanceNode(*TubercolisisVar));              // 8
        idList.insert(id.addChanceNode(*VisitAsiaVar));                 // 9
        idList.insert(id.addUtilityNode(*HospitalizationUtilityVar));   // 10
        idList.insert(id.addUtilityNode(*TakingXRayUtilityVar));        // 11

        id.addArc(idList[0], idList[10]);
        id.addArc(idList[0], idList[1]);

        id.addArc(idList[1], idList[7]);
        id.addArc(idList[1], idList[11]);

        id.addArc(idList[2], idList[3]);
        id.addArc(idList[2], idList[4]);

        id.addArc(idList[3], idList[6]);

        id.addArc(idList[4], idList[10]);
        id.addArc(idList[4], idList[5]);

        id.addArc(idList[5], idList[6]);
        id.addArc(idList[5], idList[7]);

        id.addArc(idList[8], idList[10]);
        id.addArc(idList[8], idList[5]);
        id.addArc(idList[8], idList[11]);

        id.addArc(idList[9], idList[8]);

      } catch (gum::Exception& e) {
        std::cerr << std::endl << e.errorCallStack() << std::endl;
        throw;
      }
    }

    void populateDecAsia(gum::InfluenceDiagram< double >& id,
                         gum::List< gum::NodeId >&        idList) {
      fillTopoDecAsia(id, idList);

      try {
        id.cpt(idList[9]).populate({0.01f, 0.99f});
        id.cpt(idList[4]).populate({0.1f, 0.9f, 0.01f, 0.99f});
        id.cpt(idList[3]).populate({0.6f, 0.4f, 0.3f, 0.7f});
        id.cpt(idList[2]).populate({0.5f, 0.5f});
        id.cpt(idList[6]).populate(
           {0.9f, 0.1f, 0.7f, 0.3f, 0.8f, 0.2f, 0.1f, 0.9f});
        id.cpt(idList[8]).populate({0.05f, 0.95f, 0.01f, 0.99f});
        id.cpt(idList[5]).populate({1, 0, 1, 0, 1, 0, 0, 1});
        id.cpt(idList[7]).populate(
           {0.98f, 0.02f, 0.5f, 0.5f, 0.05f, 0.95f, 0.5f, 0.5f});

        id.utility(idList[10]).populate({180, 2, 120, 4, 160, 0, 15, 40});
        id.utility(idList[11]).populate({0, 10, 1, 10});
      } catch (gum::Exception& e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    public:
    // OilWildcatter test variable
    gum::LabelizedVariable *TestVar, *DrillVar;
    gum::LabelizedVariable *ResultTestVar, *OilAmountVar;
    gum::LabelizedVariable *TestUtilityVar, *DrillingUtilityVar;

    // Dec-Asia test variable
    gum::LabelizedVariable *HospitalizeVar, *TakeXRayVar;
    gum::LabelizedVariable *SmokingVar, *BronchitisVar, *LungCancerVar, *EitherVar,
       *DyspnoeaVar, *PositiveXRayVar, *TubercolisisVar, *VisitAsiaVar;
    gum::LabelizedVariable *HospitalizationUtilityVar, *TakingXRayUtilityVar;

    void setUp() final {
      TestVar = new gum::LabelizedVariable("T", "Test", 2);
      TestVar->eraseLabels();
      TestVar->addLabel("Yes");
      TestVar->addLabel("No");
      DrillVar = new gum::LabelizedVariable("D", "Drill", 2);
      DrillVar->eraseLabels();
      DrillVar->addLabel("Yes");
      DrillVar->addLabel("No");
      ResultTestVar = new gum::LabelizedVariable("R", "Result of test", 4);
      ResultTestVar->eraseLabels();
      ResultTestVar->addLabel("NoS");
      ResultTestVar->addLabel("OpS");
      ResultTestVar->addLabel("ClS");
      ResultTestVar->addLabel("NoR");
      OilAmountVar = new gum::LabelizedVariable("O", "Oil Amount", 3);
      OilAmountVar->eraseLabels();
      OilAmountVar->addLabel("Dry");
      OilAmountVar->addLabel("Wet");
      OilAmountVar->addLabel("Soak");
      TestUtilityVar = new gum::LabelizedVariable("Ut", "Utility of Testing", 1);
      DrillingUtilityVar =
         new gum::LabelizedVariable("Ud", "Utility of Drilling", 1);

      HospitalizeVar = new gum::LabelizedVariable("H", "Hospitalize?", 2);
      HospitalizeVar->eraseLabels();
      HospitalizeVar->addLabel("Yes");
      HospitalizeVar->addLabel("No");
      TakeXRayVar = new gum::LabelizedVariable("Ta", "Take X-Ray?", 2);
      TakeXRayVar->eraseLabels();
      TakeXRayVar->addLabel("Yes");
      TakeXRayVar->addLabel("No");
      SmokingVar = new gum::LabelizedVariable("S", "Smoking?", 2);
      SmokingVar->eraseLabels();
      SmokingVar->addLabel("Yes");
      SmokingVar->addLabel("No");
      BronchitisVar = new gum::LabelizedVariable("B", "Bronchitis?", 2);
      BronchitisVar->eraseLabels();
      BronchitisVar->addLabel("Yes");
      BronchitisVar->addLabel("No");
      LungCancerVar = new gum::LabelizedVariable("L", "Lung Cancer?", 2);
      LungCancerVar->eraseLabels();
      LungCancerVar->addLabel("Yes");
      LungCancerVar->addLabel("No");
      EitherVar =
         new gum::LabelizedVariable("E", "Either tuberculosis or lung cancer?", 2);
      EitherVar->eraseLabels();
      EitherVar->addLabel("Yes");
      EitherVar->addLabel("No");
      DyspnoeaVar = new gum::LabelizedVariable("D", "Dyspnoea?", 2);
      DyspnoeaVar->eraseLabels();
      DyspnoeaVar->addLabel("Yes");
      DyspnoeaVar->addLabel("No");
      PositiveXRayVar = new gum::LabelizedVariable("P", "Positive X-Ray?", 2);
      PositiveXRayVar->eraseLabels();
      PositiveXRayVar->addLabel("Yes");
      PositiveXRayVar->addLabel("No");
      TubercolisisVar = new gum::LabelizedVariable("Tu", "Tuberculosis?", 2);
      TubercolisisVar->eraseLabels();
      TubercolisisVar->addLabel("Yes");
      TubercolisisVar->addLabel("No");
      VisitAsiaVar = new gum::LabelizedVariable("A", "Visit to Asia?", 2);
      VisitAsiaVar->eraseLabels();
      VisitAsiaVar->addLabel("Yes");
      VisitAsiaVar->addLabel("No");
      HospitalizationUtilityVar =
         new gum::LabelizedVariable("Uh", "Utility of Hospitalization", 1);
      TakingXRayUtilityVar =
         new gum::LabelizedVariable("Ut", "Utility of Taking XRay", 1);
    }

    void tearDown() final {
      delete TestVar;
      delete DrillVar;
      delete ResultTestVar;
      delete OilAmountVar;
      delete TestUtilityVar;
      delete DrillingUtilityVar;

      delete HospitalizeVar;
      delete TakeXRayVar;
      delete SmokingVar;
      delete BronchitisVar;
      delete LungCancerVar;
      delete EitherVar;
      delete DyspnoeaVar;
      delete PositiveXRayVar;
      delete TubercolisisVar;
      delete VisitAsiaVar;
      delete HospitalizationUtilityVar;
      delete TakingXRayUtilityVar;
    }

    void /*test*/ Constructor() {
      gum::InfluenceDiagram< double >* topology;
      gum::List< gum::NodeId >         idList;
      TS_GUM_ASSERT_THROWS_NOTHING(topology =
                                      new gum::InfluenceDiagram< double >())
      TS_GUM_ASSERT_THROWS_NOTHING(populateDecAsia(*topology, idList))

      gum::ShaferShenoyLIMIDSInference< double >* dIDI = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
         dIDI = new gum::ShaferShenoyLIMIDSInference< double >(topology))
      TS_GUM_ASSERT_THROWS_NOTHING(if (dIDI != nullptr) delete dIDI)
      if (topology != nullptr) delete topology;
    }

    void /*test*/ StrongJunctionTree() {
      gum::InfluenceDiagram< double > id;
      gum::List< gum::NodeId >        idList;

      gum::LabelizedVariable dVar1("decisionVar1", "D1", 2);
      gum::LabelizedVariable dVar2("decisionVar2", "D2", 2);
      gum::LabelizedVariable dVar3("decisionVar3", "D3", 2);
      gum::LabelizedVariable dVar4("decisionVar4", "D4", 2);
      gum::LabelizedVariable cVar1("A", "a", 2);
      gum::LabelizedVariable cVar2("B", "b", 2);
      gum::LabelizedVariable cVar3("C", "c", 2);
      gum::LabelizedVariable cVar4("D", "d", 2);
      gum::LabelizedVariable cVar5("E", "e", 2);
      gum::LabelizedVariable cVar6("F", "f", 2);
      gum::LabelizedVariable cVar7("G", "g", 2);
      gum::LabelizedVariable cVar8("H", "h", 2);
      gum::LabelizedVariable cVar9("I", "i", 2);
      gum::LabelizedVariable cVar10("J", "j", 2);
      gum::LabelizedVariable cVar11("K", "k", 2);
      gum::LabelizedVariable cVar12("L", "l", 2);
      gum::LabelizedVariable uVar1("utilityVar1", "U1", 1);
      gum::LabelizedVariable uVar2("utilityVar2", "U2", 1);
      gum::LabelizedVariable uVar3("utilityVar3", "U3", 1);
      gum::LabelizedVariable uVar4("utilityVar4", "U4", 1);

      idList.insert(id.addDecisionNode(dVar1));   // 0
      idList.insert(id.addDecisionNode(dVar2));   // 1
      idList.insert(id.addDecisionNode(dVar3));   // 2
      idList.insert(id.addDecisionNode(dVar4));   // 3
      idList.insert(id.addChanceNode(cVar1));     // 4
      idList.insert(id.addChanceNode(cVar2));     // 5
      idList.insert(id.addChanceNode(cVar3));     // 6
      idList.insert(id.addChanceNode(cVar4));     // 7
      idList.insert(id.addChanceNode(cVar5));     // 8
      idList.insert(id.addChanceNode(cVar6));     // 9
      idList.insert(id.addChanceNode(cVar7));     // 10
      idList.insert(id.addChanceNode(cVar8));     // 11
      idList.insert(id.addChanceNode(cVar9));     // 12
      idList.insert(id.addChanceNode(cVar10));    // 13
      idList.insert(id.addChanceNode(cVar11));    // 14
      idList.insert(id.addChanceNode(cVar12));    // 15
      idList.insert(id.addUtilityNode(uVar1));    // 16
      idList.insert(id.addUtilityNode(uVar2));    // 17
      idList.insert(id.addUtilityNode(uVar3));    // 18
      idList.insert(id.addUtilityNode(uVar4));    // 19

      id.addArc(idList[4], idList[6]);
      id.addArc(idList[5], idList[6]);
      id.addArc(idList[5], idList[7]);
      id.addArc(idList[5], idList[0]);
      id.addArc(idList[0], idList[7]);
      id.addArc(idList[0], idList[16]);
      id.addArc(idList[6], idList[8]);
      id.addArc(idList[7], idList[8]);
      id.addArc(idList[7], idList[9]);
      id.addArc(idList[8], idList[10]);
      id.addArc(idList[8], idList[1]);
      id.addArc(idList[9], idList[1]);
      id.addArc(idList[9], idList[11]);
      id.addArc(idList[10], idList[3]);
      id.addArc(idList[10], idList[12]);
      id.addArc(idList[1], idList[12]);
      id.addArc(idList[1], idList[2]);
      id.addArc(idList[3], idList[15]);
      id.addArc(idList[12], idList[15]);
      id.addArc(idList[11], idList[13]);
      id.addArc(idList[11], idList[14]);
      id.addArc(idList[2], idList[14]);
      id.addArc(idList[2], idList[17]);
      id.addArc(idList[2], idList[3]);
      id.addArc(idList[13], idList[18]);
      id.addArc(idList[14], idList[18]);
      id.addArc(idList[15], idList[19]);

      gum::NullStream                            devnull;
      gum::ShaferShenoyLIMIDSInference< double > dIDI(&id);
      auto                                       jt = dIDI.junctionTree();
      GUM_TRACE_VAR(jt);
    }

    void /*test*/ InferenceWithOilWildCater() {
      gum::InfluenceDiagram< double >* topology;
      gum::List< gum::NodeId >         idList;
      topology = new gum::InfluenceDiagram< double >();
      populateOilWildcater(*topology, idList);

      gum::ShaferShenoyLIMIDSInference< double >* dIDI;
      dIDI = new gum::ShaferShenoyLIMIDSInference< double >(topology);

      TS_GUM_ASSERT_THROWS_NOTHING(dIDI->makeInference())

      delete dIDI;
      delete topology;
    }

    void /*test*/ InferenceWithDecAsia() {
      gum::InfluenceDiagram< double >* topology;
      gum::List< gum::NodeId >         idList;
      topology = new gum::InfluenceDiagram< double >();
      populateDecAsia(*topology, idList);
      gum::NullStream devnull;

      gum::ShaferShenoyLIMIDSInference< double >* dIDI;
      TS_GUM_ASSERT_THROWS_NOTHING(
         dIDI = new gum::ShaferShenoyLIMIDSInference< double >(topology))

      TS_ASSERT_THROWS(dIDI->MEU(), gum::OperationNotAllowed)
      TS_ASSERT_THROWS(dIDI->optimalDecision(idList[0]), gum::OperationNotAllowed)
      TS_ASSERT_THROWS(dIDI->optimalDecisions(), gum::OperationNotAllowed)
      TS_GUM_ASSERT_THROWS_NOTHING(dIDI->makeInference())
      TS_GUM_ASSERT_THROWS_NOTHING(dIDI->MEU())
      TS_GUM_ASSERT_THROWS_NOTHING(dIDI->optimalDecision(idList[0]))
      TS_ASSERT_THROWS(dIDI->optimalDecision(idList[2]), gum::InvalidNode)
      TS_GUM_ASSERT_THROWS_NOTHING(dIDI->optimalDecisions())

      TS_GUM_ASSERT_THROWS_NOTHING(dIDI->makeInference())
      TS_GUM_ASSERT_THROWS_NOTHING(dIDI->optimalDecisions())

      delete dIDI;
      delete topology;
    }

    void /*test*/ InferenceWithOilWildCaterAndEvidence() {
      auto                     topology = new gum::InfluenceDiagram< double >();
      gum::List< gum::NodeId > idList;
      populateOilWildcater(*topology, idList);

      auto evidence1 = new gum::Potential< double >();
      auto evidence2 = new gum::Potential< double >();

      gum::List< const gum::Potential< double >* > e_list;
      e_list.insert(evidence1);
      e_list.insert(evidence2);

      gum::ShaferShenoyLIMIDSInference< double > inf(topology);

      evidence1->add(topology->variable(idList[2]));
      evidence1->add(topology->variable(idList[3]));
      TS_ASSERT_THROWS(inf.addListOfEvidence(e_list), gum::OperationNotAllowed)
      evidence1->erase(topology->variable(idList[3]));
      evidence2->add(topology->variable(idList[3]));

      TS_GUM_ASSERT_THROWS_NOTHING(evidence1->populate({0.2f, 0.3f, 0.1f, 0.4f}))
      evidence2->populate({0.2f, 0.3f, 0.5f});

      TS_GUM_ASSERT_THROWS_NOTHING(inf.addListOfEvidence(e_list))

      TS_GUM_ASSERT_THROWS_NOTHING(inf.makeInference())

      TS_GUM_ASSERT_THROWS_NOTHING(
         inf.eraseEvidence(idList[2]))   // remove evidence1

      TS_GUM_ASSERT_THROWS_NOTHING(inf.eraseAllEvidence())

      delete topology;
      delete evidence1;
      delete evidence2;
    }

    void /*test*/ WithNames() {
      gum::InfluenceDiagram< double > diag;
      diag.add(gum::LabelizedVariable("A", "A", 2));
      diag.addDecisionNode(gum::LabelizedVariable("D", "D", 2));
      diag.addUtilityNode(gum::LabelizedVariable("U", "U", 1));

      diag.changeVariableName(0, "O");
      TS_ASSERT_EQUALS(diag.variable(0).name(), "O")
      diag.changeVariableName("O", "I");
      TS_ASSERT_EQUALS(diag.variable(0).name(), "I")

      diag.addArc(0, 1);
      TS_ASSERT(diag.existsPathBetween(0, 1))
      TS_ASSERT(diag.existsPathBetween("I", "D"))
      diag.eraseArc(0, 1);
      TS_ASSERT(!diag.existsPathBetween(0, 1))
      TS_ASSERT(!diag.existsPathBetween("I", "D"))

      diag.addArc("I", "D");
      TS_ASSERT(diag.existsPathBetween(0, 1))
      TS_ASSERT(diag.existsPathBetween("I", "D"))
      diag.eraseArc("I", "D");
      TS_ASSERT(!diag.existsPathBetween(0, 1))
      TS_ASSERT(!diag.existsPathBetween("I", "D"))

      TS_ASSERT_THROWS(diag.addArc("foo", "bar"), gum::NotFound)
    }

    void /*test*/ FromBug() {
      gum::InfluenceDiagram< double > tst_id;

      auto c = tst_id.add(gum::LabelizedVariable("c", "chance variable", 2));
      auto c1 = tst_id.add(gum::LabelizedVariable("c1", "chance variable 1", 2));
      auto d = tst_id.addDecisionNode(
         gum::LabelizedVariable("d", "decision variable", 2));
      auto u = tst_id.addUtilityNode(
         gum::LabelizedVariable("u", "decision variable", 1));

      tst_id.addArc(c, u);
      tst_id.addArc(c, c1);
      tst_id.addArc(d, u);

      tst_id.cpt(c).populate({0.5f, 0.5f});
      tst_id.cpt(c1).populate({1, 0, 0, 1});
      tst_id.utility(u).populate({10, 100, 21, 200});
      {
        gum::ShaferShenoyLIMIDSInference< double > inf(&tst_id);
        inf.makeInference();
        TS_ASSERT_EQUALS(inf.optimalDecision(d), 1u)
        TS_ASSERT_EQUALS(inf.MEU(), 110.5)
      }
      {
        gum::ShaferShenoyLIMIDSInference< double > inf(&tst_id);
        gum::Potential< double >                   evidence;
        evidence.add(tst_id.variableFromName("c"));
        evidence.populate({1, 0});
        gum::List< const gum::Potential< double >* > l;
        l.insert(&evidence);
        inf.addListOfEvidence(l);
        inf.makeInference();
        TS_ASSERT_EQUALS(inf.optimalDecision(d), 1u)
        TS_ASSERT_EQUALS(inf.MEU(), 21)
      }
    }

    void /*test*/ BugFromNeapolitan() {
      gum::InfluenceDiagram< double > model;

      model.addDecisionNode(gum::LabelizedVariable(
         "DoTest", "which tests to run", {"None", "First", "Both"}));
      model.addDecisionNode(
         gum::LabelizedVariable("Buy", "Buy the car", {"No", "Yes"}));

      model.addChanceNode(
         gum::LabelizedVariable("Condition", "Car Condition", {"Good", "Lemon"}));
      model.addChanceNode(gum::LabelizedVariable(
         "FirstTest", "Test Results", {"NotDone", "Positive", "Negative"}));
      model.addChanceNode(gum::LabelizedVariable(
         "SecondTest", "Test Results", {"NotDone", "Positive", "Negative"}));

      model.addUtilityNode(gum::LabelizedVariable("U", "test satisfaction", 1));
      model.addUtilityNode(gum::LabelizedVariable("V", "car satisfaction", 1));

      model.addArc("Condition", "V");
      model.addArc("Buy", "V");
      model.addArc("DoTest", "U");
      model.addArc("FirstTest", "Buy");
      model.addArc("SecondTest", "Buy");
      model.addArc("Condition", "FirstTest");
      model.addArc("DoTest", "FirstTest");
      model.addArc("FirstTest", "SecondTest");
      model.addArc("Condition", "SecondTest");
      model.addArc("DoTest", "SecondTest");

      model.utility("U").fillWith({0, -9, -13});

      model.utility("V").fillWith({0, 0, 60, -100});
      GUM_TRACE_VAR(model.utility("V"))

      model.cpt("Condition").fillWith({0.8, 0.2});

      model.cpt("FirstTest")
         .fillWith(
            // clang-format off
                   {1,0,0,
                    1,0,0,
                    0,.90,.10,
                    0,.40,.60,
                    0,.90,.10,
                    0,.40,.60});   // clang-format on

      model.cpt("SecondTest")
         .fillWith(
            // clang-format off
                   {1, 0, 0,
                    1, 0, 0,
                    1, 0, 0,
                    1, 0, 0,
                    1, 0, 0,
                    1, 0, 0,
                    1, 0, 0,
                    1, 0, 0,
                    1, 0, 0,
                    1, 0, 0,
                    1, 0, 0,
                    1, 0, 0,
                    1, 0, 0,
                    0, 8.0 / 9.0, 1.0 / 9.0,
                    0, 1, 0,
                    1, 0, 0,
                    0, 1.0 / 3.0, 2.0 / 3.0,
                    0, 4.0 / 9.0, 5.0 / 9.0});   // clang-format on

      {
        gum::ShaferShenoyLIMIDSInference< double > inf(&model);
        inf.makeInference();
        GUM_TRACE_VAR(inf.optimalDecisions())
        GUM_TRACE_VAR(inf.MEU())
        GUM_TRACE_VAR(inf.optimalDecision("Buy"))
      }
      /*{
        gum::ShaferShenoyLIMIDSInference< double > inf(&model);

        gum::Potential< double > eDoTest;
        eDoTest.add(model.variableFromName("DoTest"));
        eDoTest.fillWith({0, 0, 1}); // both

        gum::Potential< double > eFirstTest;
        eFirstTest.add(model.variableFromName("FirstTest"));
        eFirstTest.fillWith({0, 1, 0}); // positive

        gum::Potential< double > eSecondTest;
        eSecondTest.add(model.variableFromName("SecondTest"));
        eSecondTest.fillWith({0, 1, 0}); // positive

        gum::List< const gum::Potential< double >* > l;
        l.insert(&eDoTest);
        l.insert(&eFirstTest);
        l.insert(&eSecondTest);
        inf.addListOfEvidence(l);

        inf.makeInference();
        GUM_TRACE_VAR(inf.optimalDecisions())
        GUM_TRACE_VAR(inf.MEU())
        GUM_TRACE_VAR(inf.optimalDecision("Buy"))
      }*/
    }

    void testNewStructure() {
      {
        auto infdiag =
           gum::InfluenceDiagram< double >::fastPrototype("*D1->Z->*D2->X->$U");
        auto ieid = gum::ShaferShenoyLIMIDSInference< double >(&infdiag);
        auto res = ieid.partialOrder();
        TS_ASSERT_EQUALS(res.size(), 2U);
        TS_ASSERT_EQUALS(res[0], gum::NodeSet({infdiag.idFromName("D2")}));
        TS_ASSERT_EQUALS(res[1], gum::NodeSet({infdiag.idFromName("D1")}));
      }
      {
        auto infdiag =
           gum::InfluenceDiagram< double >::fastPrototype("D1->Z->D2->X->$U");
        auto ieid = gum::ShaferShenoyLIMIDSInference< double >(&infdiag);
        auto res = ieid.partialOrder();
        TS_ASSERT_EQUALS(res.size(), 0U);
      }

      {
        auto infdiag = gum::InfluenceDiagram< double >::fastPrototype(
           "*D1->Z->*D2->U->*D3->V->*D4<-W<-*D5<-L;"
           "*D3<-M<-*D6->N->*D4<-*D2;X<-*D1->Y->D3;D5->$Q1<-W;"
           "U->$Q2<-D4;N->$Q3;X->$Q4<-D2;Q2<-*D7->Q4");
        auto ieid = gum::ShaferShenoyLIMIDSInference< double >(&infdiag);
        auto res = ieid.partialOrder();
        TS_ASSERT_EQUALS(res.size(), 4U);
        TS_ASSERT_EQUALS(
           res[0],
           gum::NodeSet({infdiag.idFromName("D4"), infdiag.idFromName("D7")}));
        TS_ASSERT_EQUALS(
           res[1],
           gum::NodeSet({infdiag.idFromName("D3"), infdiag.idFromName("D5")}));
        TS_ASSERT_EQUALS(
           res[2],
           gum::NodeSet({infdiag.idFromName("D2"), infdiag.idFromName("D6")}));
        TS_ASSERT_EQUALS(res[3], gum::NodeSet({infdiag.idFromName("D1")}));
      }
    }

    void testNoForgettingAssumption() {
      auto infdiag = gum::InfluenceDiagram< double >::fastPrototype(
         "*D1->$U3<-R1->R2->R3<-*D4->$U4<-R4<-R1<-*D2;"
         "R4->D4<-*D3<-D2<-D1;"
         "D3->$U1<-R2;R3->$U2");
      auto ieid = gum::ShaferShenoyLIMIDSInference< double >(&infdiag);

      TS_ASSERT(!ieid.isNoForgettingAssumption());

      auto dag = ieid.reducedGraph();
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D1")).size(), 0u);
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D2")).size(), 1u);
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D3")).size(), 1u);
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D4")).size(), 2u);

      TS_ASSERT_THROWS(ieid.addNoForgettingAssumption({"D11"}), gum::NotFound);
      TS_ASSERT_THROWS(ieid.addNoForgettingAssumption({"D4", "D1", "D2", "D3"}),
                       gum::InvalidDirectedCycle);
      TS_ASSERT_THROWS(ieid.addNoForgettingAssumption({"D1", "D2", "D3"}),
                       gum::SizeError);

      TS_GUM_ASSERT_THROWS_NOTHING(
         ieid.addNoForgettingAssumption({"D1", "D2", "D3", "D4"}));
      TS_ASSERT(ieid.isNoForgettingAssumption());

      dag = ieid.reducedGraph();
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D1")).size(), 0u);
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D2")).size(), 1u);
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D3")).size(), 1u);
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D4")).size(), 2u);
    }

    void testNoForgettingAssumption2() {
      auto limids = gum::InfluenceDiagram< double >::fastPrototype(
         "a->c->e->g->*d4->l->$u4;c<-b->d->f->h->k<-*d3->$u2;$u1<-*d1->d->e->*d2->"
         "i->l;g->i;f->d2;b->d1;h->j->$u3<-k");
      auto ieid = gum::ShaferShenoyLIMIDSInference< double >(&limids);
      auto forgetting = ieid.reducedLIMID();
      GUM_TRACE_VAR(ieid.partialOrder());

      std::vector< std::string > order({"d1", "d2", "d3", "d4"});
      ieid.addNoForgettingAssumption(order);
      GUM_TRACE_VAR(ieid.partialOrder());
      auto noForgetting = ieid.reducedLIMID();
      GUM_TRACE(limids.size()
                << "-" << forgetting.size() << "-" << noForgetting.size());
      GUM_TRACE(limids.sizeArcs()
                << "-" << forgetting.sizeArcs() << "-" << noForgetting.sizeArcs());
      GUM_TRACE_VAR(noForgetting.toDot());
    }
  };
}   // namespace gum_tests
