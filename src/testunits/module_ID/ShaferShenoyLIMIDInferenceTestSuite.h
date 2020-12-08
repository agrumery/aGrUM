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

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/ID/generator/influenceDiagramGenerator.h>
#include <agrum/ID/inference/ShaferShenoyLIMIDInference.h>
#include <agrum/ID/influenceDiagram.h>
#include <agrum/ID/io/BIFXML/BIFXMLIDWriter.h>
#include <agrum/tools/graphs/graphElements.h>
#include <agrum/tools/variables/discreteVariable.h>
#include <agrum/tools/variables/labelizedVariable.h>
#include <agrum/ID/io/BIFXML/BIFXMLIDReader.h>
#include <agrum/ID/io/BIFXML/BIFXMLIDWriter.h>

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
std::string toTab(const gum::Potential< double >& p);

namespace gum_tests {

  class ShaferShenoyLIMIDInferenceTestSuite: public CxxTest::TestSuite {
    private:
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
    // Dec-Asia test variable
    gum::LabelizedVariable *HospitalizeVar, *TakeXRayVar;
    gum::LabelizedVariable *SmokingVar, *BronchitisVar, *LungCancerVar, *EitherVar,
       *DyspnoeaVar, *PositiveXRayVar, *TubercolisisVar, *VisitAsiaVar;
    gum::LabelizedVariable *HospitalizationUtilityVar, *TakingXRayUtilityVar;

    void setUp() final {
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

    void testConstructor() {
      std::string                     file = GET_RESSOURCES_PATH("ID/decAsia.xml");
      gum::InfluenceDiagram< double > net;
      gum::BIFXMLIDReader< double >   reader(&net, file);
      reader.proceed();

      gum::ShaferShenoyLIMIDInference< double >* dIDI = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
         dIDI = new gum::ShaferShenoyLIMIDInference< double >(&net))
      TS_GUM_ASSERT_THROWS_NOTHING(if (dIDI != nullptr) delete dIDI)
    }

#define TS_ASSERT_EQUALS_SHOW(x, y) \
  { GUM_TRACE_VAR((x)) GUM_TRACE_VAR(y) TS_ASSERT_EQUALS(x, y) }

    void testInferenceWithOilWildCater() {
      std::string file = GET_RESSOURCES_PATH("ID/OilWildcatter.xml");
      gum::InfluenceDiagram< double > net;
      gum::BIFXMLIDReader< double >   reader(&net, file);
      reader.proceed();

      gum::ShaferShenoyLIMIDInference< double >* dIDI;
      dIDI = new gum::ShaferShenoyLIMIDInference< double >(&net);

      TS_GUM_ASSERT_THROWS_NOTHING(dIDI->makeInference())

      TS_ASSERT_EQUALS(
         dIDI->posterior("OilContents"),
         (gum::Potential< double >() << net.variableFromName("OilContents"))
            .fillWith({0.5, 0.3, 0.2}))
      TS_ASSERT_EQUALS(
         dIDI->posteriorUtility("OilContents"),
         (gum::Potential< double >() << net.variableFromName("OilContents"))
            .fillWith({-38, 25, 170}))

      TS_ASSERT_EQUALS(
         dIDI->posterior("TestResult"),
         (gum::Potential< double >() << net.variableFromName("TestResult"))
            .fillWith({0.24, 0.35, 0.41}))
      TS_ASSERT_EQUALS(
         dIDI->posteriorUtility("TestResult"),
         (gum::Potential< double >() << net.variableFromName("TestResult"))
            .fillWith({77.5, 22.8571, -10}))

      TS_ASSERT_EQUALS(
         dIDI->posterior("Testing"),
         (gum::Potential< double >() << net.variableFromName("Testing"))
            .fillWith({1, 0}))
      TS_ASSERT_EQUALS(
         dIDI->posteriorUtility("Testing"),
         (gum::Potential< double >() << net.variableFromName("Testing"))
            .fillWith({22.5, 20}))

      TS_ASSERT_EQUALS(
         dIDI->posterior("Drilling"),
         (gum::Potential< double >() << net.variableFromName("Drilling"))
            .fillWith({0.59, 0.41}))
      TS_ASSERT_EQUALS(
         dIDI->posteriorUtility("Drilling"),
         (gum::Potential< double >() << net.variableFromName("Drilling"))
            .fillWith({45.0847, -10}))

      TS_ASSERT_DELTA(dIDI->meanVar("Cost").first, -10, 1e-5)
      TS_ASSERT_DELTA(dIDI->meanVar("Cost").second, 0, 1e-5)
      TS_ASSERT_DELTA(dIDI->meanVar("Reward").first, 32.5, 1e-5)
      TS_ASSERT_DELTA(dIDI->meanVar("Reward").second, 7648.750, 1e-5)

      delete dIDI;
    }

    /* void XXXXInferenceWithDecAsia() {
      gum::InfluenceDiagram< double >* topology;
      gum::List< gum::NodeId >         idList;
      topology = new gum::InfluenceDiagram< double >();
      populateDecAsia(*topology, idList);
      gum::NullStream devnull;

      gum::ShaferShenoyLIMIDInference< double >* dIDI;
      TS_GUM_ASSERT_THROWS_NOTHING(
         dIDI = new gum::ShaferShenoyLIMIDInference< double >(topology))

      TS_ASSERT_THROWS(dIDI->MEU(), gum::OperationNotAllowed)
      TS_ASSERT_THROWS(dIDI->optimalDecision(idList[0]), gum::OperationNotAllowed)
      TS_GUM_ASSERT_THROWS_NOTHING(dIDI->makeInference())
      TS_GUM_ASSERT_THROWS_NOTHING(dIDI->MEU())
      TS_GUM_ASSERT_THROWS_NOTHING(dIDI->optimalDecision(idList[0]))
      TS_ASSERT_THROWS(dIDI->optimalDecision(idList[2]), gum::InvalidNode)

      TS_GUM_ASSERT_THROWS_NOTHING(dIDI->makeInference())

      delete dIDI;
      delete topology;
    }

    /* void XXXXInferenceWithOilWildCaterAndEvidence() {
      auto                     topology = new gum::InfluenceDiagram< double >();
      gum::List< gum::NodeId > idList;
      // populateOilWildcater(*topology, idList);

      auto evidence1 = new gum::Potential< double >();
      auto evidence2 = new gum::Potential< double >();

      gum::List< const gum::Potential< double >* > e_list;
      e_list.insert(evidence1);
      e_list.insert(evidence2);

      gum::ShaferShenoyLIMIDInference< double > inf(topology);

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

    /* void XXXXWithNames() {
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

    /* void XXXXFromBug() {
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
        gum::ShaferShenoyLIMIDInference< double > inf(&tst_id);
        inf.makeInference();
        TS_ASSERT_EQUALS(inf.optimalDecision(d), 1u)
        TS_ASSERT_EQUALS(inf.MEU(), 110.5)
      }
      {
        gum::ShaferShenoyLIMIDInference< double > inf(&tst_id);
        gum::Potential< double >                  evidence;
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

    /* void XXXXBugFromNeapolitan() {
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
        gum::ShaferShenoyLIMIDInference< double > inf(&model);
        inf.makeInference();
        GUM_TRACE_VAR(inf.MEU())
        GUM_TRACE_VAR(inf.optimalDecision("Buy"))
      }
      /*{
        gum::ShaferShenoyLIMIDInference< double > inf(&model);

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
        GUM_TRACE_VAR(inf.MEU())
        GUM_TRACE_VAR(inf.optimalDecision("Buy"))
      }
    }

    /* void XXXXNewStructure() {
      {
        auto infdiag =
           gum::InfluenceDiagram< double >::fastPrototype("*D1->Z->*D2->X->$U");
        auto ieid = gum::ShaferShenoyLIMIDInference< double >(&infdiag);
        auto res = ieid.reversePartialOrder();
        TS_ASSERT_EQUALS(res.size(), 2U);
        TS_ASSERT_EQUALS(res[0], gum::NodeSet({infdiag.idFromName("D2")}));
        TS_ASSERT_EQUALS(res[1], gum::NodeSet({infdiag.idFromName("D1")}));
      }
      {
        auto infdiag =
           gum::InfluenceDiagram< double >::fastPrototype("D1->Z->D2->X->$U");
        auto ieid = gum::ShaferShenoyLIMIDInference< double >(&infdiag);
        auto res = ieid.reversePartialOrder();
        TS_ASSERT_EQUALS(res.size(), 0U);
      }

      {
        auto infdiag = gum::InfluenceDiagram< double >::fastPrototype(
           "*D1->Z->*D2->U->*D3->V->*D4<-W<-*D5<-L;"
           "*D3<-M<-*D6->N->*D4<-*D2;X<-*D1->Y->D3;D5->$Q1<-W;"
           "U->$Q2<-D4;N->$Q3;X->$Q4<-D2;Q2<-*D7->Q4");
        auto ieid = gum::ShaferShenoyLIMIDInference< double >(&infdiag);
        auto res = ieid.reversePartialOrder();
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

    /* void XXXXSolvability() {
      {
        auto infdiag =
           gum::InfluenceDiagram< double >::fastPrototype("*D1->Z->*D2->X->$U<-Y");
        auto ieid = gum::ShaferShenoyLIMIDInference< double >(&infdiag);
        TS_ASSERT(ieid.isSolvable());
      }
      {
        auto infdiag = gum::InfluenceDiagram< double >::fastPrototype(
           "*D1->Z->*D2->X->$U<-Y<-*D3");
        auto ieid = gum::ShaferShenoyLIMIDInference< double >(&infdiag);
        TS_ASSERT(!ieid.isSolvable());
      }
      {
        auto infdiag = gum::InfluenceDiagram< double >::fastPrototype(
           "*D1->Z->*D2->X->$U<-Y<-*D3");
        auto ieid = gum::ShaferShenoyLIMIDInference< double >(&infdiag);
        ieid.addNoForgettingAssumption({"D1", "D3", "D2"});
        TS_ASSERT(ieid.isSolvable());
      }
    }

    /* void XXXXNoForgettingAssumption() {
      // From Evaluating IDs using LIMIDS, Nillson et Lauritzen, 2000
      auto infdiag = gum::InfluenceDiagram< double >::fastPrototype(
         "*D1->$U3<-R1->R2->R3<-*D4->$U4<-R4<-R1<-*D2;"
         "R4->D4<-*D3<-D2<-D1;"
         "D3->$U1<-R2;R3->$U2");
      auto ieid = gum::ShaferShenoyLIMIDInference< double >(&infdiag);

      TS_ASSERT(!ieid.hasNoForgettingAssumption());
      const auto revord = ieid.reversePartialOrder();
      auto       dag = ieid.reducedGraph();
      TS_ASSERT(ieid.isSolvable());

      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D1")), infdiag.nodeset({}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D2")),
                       infdiag.nodeset({"D1"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D3")),
                       infdiag.nodeset({"D2"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D4")),
                       infdiag.nodeset({"D3", "R4"}))

      TS_ASSERT_THROWS(ieid.addNoForgettingAssumption({"D11"}), gum::NotFound);
      TS_ASSERT_THROWS(ieid.addNoForgettingAssumption({"D4", "D1", "D2", "D3"}),
                       gum::InvalidDirectedCycle);
      TS_ASSERT_THROWS(ieid.addNoForgettingAssumption({"D1", "D2", "D3"}),
                       gum::SizeError);

      TS_GUM_ASSERT_THROWS_NOTHING(
         ieid.addNoForgettingAssumption({"D1", "D2", "D3", "D4"}));
      TS_ASSERT(ieid.hasNoForgettingAssumption());

      dag = ieid.reducedGraph();

      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D1")), infdiag.nodeset({}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D2")),
                       infdiag.nodeset({"D1"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D3")),
                       infdiag.nodeset({"D2"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D4")),
                       infdiag.nodeset({"D2", "R4"}))


      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("R1")),
                       infdiag.nodeset({"D2"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("R2")),
                       infdiag.nodeset({"R1"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("R3")),
                       infdiag.nodeset({"R2", "D4"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("R4")),
                       infdiag.nodeset({"R1"}))

      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("U1")),
                       infdiag.nodeset({"D3", "R2"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("U2")),
                       infdiag.nodeset({"R3"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("U3")),
                       infdiag.nodeset({"R1", "D1"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("U4")),
                       infdiag.nodeset({"R4", "D4"}))
    }

    /* void XXXXNoForgettingAssumption2() {
      // from LIMIDS of decision Problems, Lauritzen et Nilsson, 1999
      // p33
      auto limids = gum::InfluenceDiagram< double >::fastPrototype(
         "a->c->e->g->*d4->l->$u4;c<-b->d->f->h->k<-*d3->$u2;$u1<-*d1->d->e->*d2->"
         "i->l;g->i;f->d2;b->d1;h->j->$u3<-k");
      std::vector< std::string > order({"d1", "d2", "d3", "d4"});

      auto       ieid = gum::ShaferShenoyLIMIDInference< double >(&limids);
      const auto revord1 = ieid.reversePartialOrder();
      TS_ASSERT_EQUALS(revord1.size(), gum::Size(2))
      TS_ASSERT_EQUALS(revord1[0], limids.nodeset({"d4", "d2", "d3"}))
      TS_ASSERT_EQUALS(revord1[1], limids.nodeset({"d1"}))

      TS_ASSERT(!ieid.isSolvable());

      ieid.addNoForgettingAssumption(order);
      TS_ASSERT(ieid.isSolvable());
      const auto revord2 = ieid.reversePartialOrder();
      TS_ASSERT_EQUALS(revord2.size(), gum::Size(4))
      for (gum::Idx i = 0; i < gum::Size(4); i++) {
        TS_ASSERT_EQUALS(revord2[i].size(), gum::Size(1));
        TS_ASSERT_EQUALS(limids.variable(*(revord2[i].begin())).name(),
                         order[3 - i]);
      }
      auto noForgetting = ieid.reducedLIMID();

      TS_ASSERT_EQUALS(noForgetting.parents("d1"), limids.nodeset({"b"}))
      TS_ASSERT_EQUALS(noForgetting.parents("d2"), limids.nodeset({"e"}))
      TS_ASSERT_EQUALS(noForgetting.parents("d3"), limids.nodeset({"f"}))
      TS_ASSERT_EQUALS(noForgetting.parents("d4"), limids.nodeset({"d2", "g"}))
      // GUM_TRACE_VAR(noForgetting.toDot());
    }

    /* void XXXXJunctionTree() {
      // From Evaluating IDs using LIMIDS, Nillson et Lauritzen, 2000
      auto infdiag = gum::InfluenceDiagram< double >::fastPrototype(
         "*D1->$U3<-R1->R2->R3<-*D4->$U4<-R4<-R1<-*D2;"
         "R4->D4<-*D3<-D2<-D1;"
         "D3->$U1<-R2;R3->$U2");
      auto ieid = gum::ShaferShenoyLIMIDInference< double >(&infdiag);

      ieid.addNoForgettingAssumption({"D1", "D2", "D3", "D4"});
      GUM_TRACE_VAR(ieid.reducedLIMID().toDot())

      auto jt = ieid.junctionTree();
      TS_ASSERT_EQUALS(jt->size(), gum::Size(5));
      GUM_TRACE_VAR(jt->toString())
      for (const auto& n: jt->nodes()) {
        GUM_TRACE(n << " : " << infdiag.names(jt->clique(n)))
      }
      try {
        ieid.makeInference();
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }
    /* void XXXXPinball() {
      std::string                     file = GET_RESSOURCES_PATH("ID/Pinball.xml");
      gum::InfluenceDiagram< double > net;
      gum::BIFXMLIDReader< double >   reader(&net, file);
      reader.proceed();

      auto ieid = gum::ShaferShenoyLIMIDInference< double >(&net);
      GUM_TRACE_VAR(ieid.reversePartialOrder())
      // ieid.addNoForgettingAssumption({std::string()})
      try {
        ieid.makeInference();
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    /* void XXXXDavidAndescavage() {
      std::string file = GET_RESSOURCES_PATH("ID/testFromDavidAndescavage.xml");
      gum::InfluenceDiagram< double > net;
      gum::BIFXMLIDReader< double >   reader(&net, file);
      reader.proceed();

      auto ieid = gum::ShaferShenoyLIMIDInference< double >(&net);
      GUM_TRACE_VAR(ieid.reversePartialOrder())
      // ieid.addNoForgettingAssumption({std::string()})
      ieid.addEvidence("DoTest", "Both");
      ieid.addEvidence("FirstTest", "Positive");
      ieid.addEvidence("SecondTest", "Positive");
      ieid.makeInference();
      GUM_TRACE_VAR(ieid.posterior("DoTest"))
      GUM_TRACE_VAR(ieid.MEU())
    }

    /* void XXXXBugWithEvidence() {
      auto tst_id =
         gum::InfluenceDiagram< double >::fastPrototype("c1<-c->$u<-*d");

      tst_id.cpt("c").fillWith({0.5, 0.5});
      tst_id.cpt("c1").fillWith({1, 0, 0, 1});
      tst_id.utility("u").fillWith({10, 21, 100, 200});
      {
        auto ie = gum::ShaferShenoyLIMIDInference< double >(&tst_id);
        ie.makeInference();
        TS_ASSERT_EQUALS(ie.optimalDecision("d"), 1);
        TS_ASSERT_EQUALS(ie.MEU(), 110.5);
        TS_ASSERT_EQUALS(
           ie.posterior("c1"),
           (gum::Potential< double >() << tst_id.variableFromName("c1"))
              .fillWith({0.5, 0.5}))
        TS_ASSERT_EQUALS(
           ie.posterior("d"),
           (gum::Potential< double >() << tst_id.variableFromName("d"))
              .fillWith({0, 110.5}))
      }
      {
        auto ie = gum::ShaferShenoyLIMIDInference< double >(&tst_id);
        ie.addEvidence("c", 1);
        ie.makeInference();
        TS_ASSERT_EQUALS(ie.optimalDecision("d"), 1);
        TS_ASSERT_EQUALS(ie.MEU(), 200);
        GUM_TRACE_VAR(ie.MEU())
        TS_ASSERT_EQUALS(
           ie.posterior("c1"),
           (gum::Potential< double >() << tst_id.variableFromName("c1"))
              .fillWith({0, 1}))
        TS_ASSERT_EQUALS(
           ie.posterior("d"),
           (gum::Potential< double >() << tst_id.variableFromName("d"))
              .fillWith({0, 200}))
      }
      {
        auto ie = gum::ShaferShenoyLIMIDInference< double >(&tst_id);
        ie.addEvidence("d", 1);
        ie.makeInference();
        TS_ASSERT_EQUALS(ie.optimalDecision("d"), 1);
        TS_ASSERT_EQUALS(ie.MEU(), 110.5);
        TS_ASSERT_EQUALS(
           ie.posterior("c1"),
           (gum::Potential< double >() << tst_id.variableFromName("c1"))
              .fillWith({0.5, 0.5}))
        TS_ASSERT_EQUALS(
           ie.posterior("d"),
           (gum::Potential< double >() << tst_id.variableFromName("d"))
              .fillWith({0, 110.5}))
      }
      {
        auto ie = gum::ShaferShenoyLIMIDInference< double >(&tst_id);
        ie.addEvidence("c1", std::vector< double >{0.8, 0.2});
        ie.makeInference();
        TS_ASSERT_EQUALS(ie.optimalDecision("d"), 1);
        TS_ASSERT_EQUALS(ie.MEU(), 56.8);
        GUM_TRACE_VAR(ie.MEU())
        TS_ASSERT_EQUALS(
           ie.posterior("c"),
           (gum::Potential< double >() << tst_id.variableFromName("c"))
              .fillWith({0.8, 0.2}))
        TS_ASSERT_EQUALS(
           ie.posterior("d"),
           (gum::Potential< double >() << tst_id.variableFromName("d"))
              .fillWith({0, 56.8}))
      }
    }
     */
  };
}   // namespace gum_tests
