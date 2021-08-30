/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

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
namespace gum_tests {

  class ShaferShenoyLIMIDInferenceTestSuite: public CxxTest::TestSuite {
    public:
    void testConstructor() {
      std::string                     file = GET_RESSOURCES_PATH("ID/decAsia.xml");
      gum::InfluenceDiagram< double > net;
      gum::BIFXMLIDReader< double >   reader(&net, file);
      reader.proceed();

      gum::ShaferShenoyLIMIDInference< double >* dIDI = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(dIDI = new gum::ShaferShenoyLIMIDInference< double >(&net))
      TS_GUM_ASSERT_THROWS_NOTHING(if (dIDI != nullptr) delete dIDI)
    }

    void testInferenceWithOilWildCater() {
      std::string                     file = GET_RESSOURCES_PATH("ID/OilWildcatter.xml");
      gum::InfluenceDiagram< double > net;
      gum::BIFXMLIDReader< double >   reader(&net, file);
      reader.proceed();

      gum::ShaferShenoyLIMIDInference< double > dIDI(&net);

      TS_GUM_ASSERT_THROWS_NOTHING(dIDI.makeInference())

      TS_ASSERT_EQUALS(dIDI.posterior("OilContents"),
                       (gum::Potential< double >() << net.variableFromName("OilContents"))
                          .fillWith({0.5, 0.3, 0.2}))
      TS_ASSERT_EQUALS(dIDI.posteriorUtility("OilContents"),
                       (gum::Potential< double >() << net.variableFromName("OilContents"))
                          .fillWith({-38, 25, 170}))

      TS_ASSERT_EQUALS(dIDI.posterior("TestResult"),
                       (gum::Potential< double >() << net.variableFromName("TestResult"))
                          .fillWith({0.24, 0.35, 0.41}))
      TS_ASSERT_EQUALS(dIDI.posteriorUtility("TestResult"),
                       (gum::Potential< double >() << net.variableFromName("TestResult"))
                          .fillWith({77.5, 22.8571, -10}))

      TS_ASSERT_EQUALS(
         dIDI.posterior("Testing"),
         (gum::Potential< double >() << net.variableFromName("Testing")).fillWith({1, 0}))
      TS_ASSERT_EQUALS(
         dIDI.posteriorUtility("Testing"),
         (gum::Potential< double >() << net.variableFromName("Testing")).fillWith({22.5, 20}))

      TS_ASSERT_EQUALS(
         dIDI.posterior("Drilling"),
         (gum::Potential< double >() << net.variableFromName("Drilling")).fillWith({0.59, 0.41}))
      TS_ASSERT_EQUALS(
         dIDI.posteriorUtility("Drilling"),
         (gum::Potential< double >() << net.variableFromName("Drilling")).fillWith({45.0847, -10}))

      TS_ASSERT_DELTA(dIDI.meanVar("Cost").first, -10, TS_GUM_SMALL_ERROR)
      TS_ASSERT_DELTA(dIDI.meanVar("Cost").second, 0, TS_GUM_SMALL_ERROR)
      TS_ASSERT_DELTA(dIDI.meanVar("Reward").first, 32.5, TS_GUM_SMALL_ERROR)
      TS_ASSERT_DELTA(dIDI.meanVar("Reward").second, 7648.750, TS_GUM_SMALL_ERROR)
    }

    void testInferenceWithOilWildCaterWithEvidenceOnDecisionNode() {
      std::string                     file = GET_RESSOURCES_PATH("ID/OilWildcatter.xml");
      gum::InfluenceDiagram< double > net;
      gum::BIFXMLIDReader< double >   reader(&net, file);
      reader.proceed();

      gum::ShaferShenoyLIMIDInference< double > dIDI(&net);

      dIDI.addEvidence("Testing", "No");
      TS_GUM_ASSERT_THROWS_NOTHING(dIDI.makeInference())

      TS_GUM_POTENTIAL_DELTA(dIDI.posterior("OilContents"),
                             (gum::Potential< double >() << net.variableFromName("OilContents"))
                                .fillWith({0.5, 0.3, 0.2}),
                             TS_GUM_SMALL_ERROR)
      TS_GUM_POTENTIAL_DELTA(dIDI.posteriorUtility("OilContents"),
                             (gum::Potential< double >() << net.variableFromName("OilContents"))
                                .fillWith({-70, 50, 200}),
                             TS_GUM_SMALL_ERROR)

      TS_GUM_POTENTIAL_DELTA(dIDI.posterior("TestResult"),
                             (gum::Potential< double >() << net.variableFromName("TestResult"))
                                .fillWith({0.333333, 0.333333, 0.333333}),
                             TS_GUM_SMALL_ERROR)
      TS_GUM_POTENTIAL_DELTA(
         dIDI.posteriorUtility("TestResult"),
         (gum::Potential< double >() << net.variableFromName("TestResult")).fillWith({20, 20, 20}),
         TS_GUM_SMALL_ERROR)

      TS_GUM_POTENTIAL_DELTA(
         dIDI.posterior("Testing"),
         (gum::Potential< double >() << net.variableFromName("Testing")).fillWith({0, 1}),
         TS_GUM_SMALL_ERROR)
      TS_GUM_POTENTIAL_DELTA(
         dIDI.posteriorUtility("Testing"),
         (gum::Potential< double >() << net.variableFromName("Testing")).fillWith({0, 20}),
         TS_GUM_SMALL_ERROR)

      TS_GUM_POTENTIAL_DELTA(
         dIDI.posterior("Drilling"),
         (gum::Potential< double >() << net.variableFromName("Drilling")).fillWith({1, 0}),
         TS_GUM_SMALL_ERROR)
      TS_GUM_POTENTIAL_DELTA(
         dIDI.posteriorUtility("Drilling"),
         (gum::Potential< double >() << net.variableFromName("Drilling")).fillWith({20, 0}),
         TS_GUM_SMALL_ERROR)

      TS_ASSERT_DELTA(dIDI.meanVar("Cost").first, 0, TS_GUM_SMALL_ERROR)
      TS_ASSERT_DELTA(dIDI.meanVar("Cost").second, 0, TS_GUM_SMALL_ERROR)
      TS_ASSERT_DELTA(dIDI.meanVar("Reward").first, 20, TS_GUM_SMALL_ERROR)
      TS_ASSERT_DELTA(dIDI.meanVar("Reward").second, 10800, TS_GUM_SMALL_ERROR)
      TS_ASSERT_DELTA(dIDI.MEU().first, 20, TS_GUM_SMALL_ERROR)
      TS_ASSERT_DELTA(dIDI.MEU().second, 10800, TS_GUM_SMALL_ERROR)
    }
    void testInferenceWithOilWildCaterWithEvidenceOnChanceNode() {
      std::string                     file = GET_RESSOURCES_PATH("ID/OilWildcatter.xml");
      gum::InfluenceDiagram< double > net;
      gum::BIFXMLIDReader< double >   reader(&net, file);
      reader.proceed();

      gum::ShaferShenoyLIMIDInference< double > dIDI(&net);

      dIDI.addEvidence("OilContents", "Wet");
      TS_GUM_ASSERT_THROWS_NOTHING(dIDI.makeInference())

      TS_GUM_POTENTIAL_DELTA(dIDI.posterior("OilContents"),
                             (gum::Potential< double >() << net.variableFromName("OilContents"))
                                .fillWith({0.0, 1.0, 0.0}),
                             TS_GUM_SMALL_ERROR)
      TS_GUM_POTENTIAL_DELTA(
         dIDI.posteriorUtility("OilContents"),
         (gum::Potential< double >() << net.variableFromName("OilContents")).fillWith({0, 50, 0}),
         TS_GUM_SMALL_ERROR)

      TS_GUM_POTENTIAL_DELTA(dIDI.posterior("TestResult"),
                             (gum::Potential< double >() << net.variableFromName("TestResult"))
                                .fillWith({0.333333, 0.333333, 0.333333}),
                             TS_GUM_SMALL_ERROR)
      TS_GUM_POTENTIAL_DELTA(
         dIDI.posteriorUtility("TestResult"),
         (gum::Potential< double >() << net.variableFromName("TestResult")).fillWith({50, 50, 50}),
         TS_GUM_SMALL_ERROR)

      TS_GUM_POTENTIAL_DELTA(
         dIDI.posterior("Testing"),
         (gum::Potential< double >() << net.variableFromName("Testing")).fillWith({0, 1}),
         TS_GUM_SMALL_ERROR)
      TS_GUM_POTENTIAL_DELTA(
         dIDI.posteriorUtility("Testing"),
         (gum::Potential< double >() << net.variableFromName("Testing")).fillWith({40, 50}),
         TS_GUM_SMALL_ERROR)

      TS_GUM_POTENTIAL_DELTA(
         dIDI.posterior("Drilling"),
         (gum::Potential< double >() << net.variableFromName("Drilling")).fillWith({1, 0}),
         TS_GUM_SMALL_ERROR)
      TS_GUM_POTENTIAL_DELTA(
         dIDI.posteriorUtility("Drilling"),
         (gum::Potential< double >() << net.variableFromName("Drilling")).fillWith({50, 0}),
         TS_GUM_SMALL_ERROR)

      TS_ASSERT_DELTA(dIDI.meanVar("Cost").first, 0, TS_GUM_SMALL_ERROR)
      TS_ASSERT_DELTA(dIDI.meanVar("Cost").second, 0, TS_GUM_SMALL_ERROR)
      TS_ASSERT_DELTA(dIDI.meanVar("Reward").first, 50, TS_GUM_SMALL_ERROR)
      TS_ASSERT_DELTA(dIDI.meanVar("Reward").second, 0, TS_GUM_SMALL_ERROR)
      TS_ASSERT_DELTA(dIDI.MEU().first, 50, TS_GUM_SMALL_ERROR)
      TS_ASSERT_DELTA(dIDI.MEU().second, 0, TS_GUM_SMALL_ERROR)
    }
    void testInferenceWithDecAsia() {
      std::string                     file = GET_RESSOURCES_PATH("ID/decAsia.xml");
      gum::InfluenceDiagram< double > net;
      gum::BIFXMLIDReader< double >   reader(&net, file);
      reader.proceed();
      gum::ShaferShenoyLIMIDInference< double >* dIDI = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(dIDI = new gum::ShaferShenoyLIMIDInference< double >(&net))

      TS_ASSERT_THROWS(dIDI->MEU(), gum::OperationNotAllowed)
      TS_ASSERT_THROWS(dIDI->optimalDecision(0), gum::OperationNotAllowed)

      TS_GUM_ASSERT_THROWS_NOTHING(dIDI->makeInference())
      TS_GUM_ASSERT_THROWS_NOTHING(dIDI->MEU())
      TS_GUM_ASSERT_THROWS_NOTHING(dIDI->optimalDecision(0))
      TS_ASSERT_THROWS(dIDI->optimalDecision(2), gum::InvalidNode)
      TS_GUM_ASSERT_THROWS_NOTHING(dIDI->makeInference())

      delete dIDI;
    }

    void testFromBug() {
      gum::InfluenceDiagram< double > net;

      auto c  = net.add(gum::LabelizedVariable("c", "chance variable", 2));
      auto c1 = net.add(gum::LabelizedVariable("c1", "chance variable 1", 2));
      auto d  = net.addDecisionNode(gum::LabelizedVariable("d", "decision variable", 2));
      auto u  = net.addUtilityNode(gum::LabelizedVariable("u", "decision variable", 1));

      net.addArc(c, u);
      net.addArc(c, c1);
      net.addArc(d, u);

      net.cpt(c).populate({0.5f, 0.5f});
      net.cpt(c1).populate({1, 0, 0, 1});
      net.utility(u).populate({10, 100, 21, 200});
      {
        gum::ShaferShenoyLIMIDInference< double > inf(&net);
        inf.makeInference();
        TS_GUM_POTENTIAL_DELTA(inf.optimalDecision(d),
                               (gum::Potential< double >() << net.variable(d)).fillWith({0, 1}),
                               TS_GUM_SMALL_ERROR)
        TS_ASSERT_EQUALS(inf.MEU().first, 110.5)
      }
      {
        gum::ShaferShenoyLIMIDInference< double > inf(&net);
        gum::Potential< double >                  evidence;
        evidence.add(net.variableFromName("c"));
        evidence.populate({1, 0});
        gum::List< const gum::Potential< double >* > l;
        l.insert(&evidence);
        inf.addListOfEvidence(l);
        inf.makeInference();
        TS_GUM_POTENTIAL_DELTA(inf.optimalDecision(d),
                               (gum::Potential< double >() << net.variable(d)).fillWith({0, 1}),
                               TS_GUM_SMALL_ERROR)
        TS_ASSERT_EQUALS(inf.MEU().first, 21)
      }
    }

    void testBugFromNeapolitan() {
      gum::InfluenceDiagram< double > model;

      model.addDecisionNode(
         gum::LabelizedVariable("DoTest", "which tests to run", {"None", "First", "Both"}));
      model.addDecisionNode(gum::LabelizedVariable("Buy", "Buy the car", {"No", "Yes"}));

      model.addChanceNode(gum::LabelizedVariable("Condition", "Car Condition", {"Good", "Lemon"}));
      model.addChanceNode(
         gum::LabelizedVariable("FirstTest", "Test Results", {"NotDone", "Positive", "Negative"}));
      model.addChanceNode(
         gum::LabelizedVariable("SecondTest", "Test Results", {"NotDone", "Positive", "Negative"}));

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
        TS_ASSERT_DELTA(inf.MEU().first, 32.9777777, TS_GUM_SMALL_ERROR)
        TS_ASSERT_EQUALS(inf.optimalDecision("Buy").sum(), 9)
      }
      {
        gum::ShaferShenoyLIMIDInference< double > inf(&model);

        gum::Potential< double > eDoTest;
        eDoTest.add(model.variableFromName("DoTest"));
        eDoTest.fillWith({0, 0, 1});   // both

        gum::Potential< double > eFirstTest;
        eFirstTest.add(model.variableFromName("FirstTest"));
        eFirstTest.fillWith({0, 1, 0});   // positive

        gum::Potential< double > eSecondTest;
        eSecondTest.add(model.variableFromName("SecondTest"));
        eSecondTest.fillWith({0, 1, 0});   // positive

        gum::List< const gum::Potential< double >* > l;
        l.insert(&eDoTest);
        l.insert(&eFirstTest);
        l.insert(&eSecondTest);
        inf.addListOfEvidence(l);

        inf.makeInference();
        TS_ASSERT_DELTA(inf.MEU().first, 40.6, TS_GUM_SMALL_ERROR)
        TS_ASSERT_EQUALS(inf.optimalDecision("Buy").sum(), 9)
      }
    }

    void testNewStructure() {
      {
        auto infdiag = gum::InfluenceDiagram< double >::fastPrototype("*D1->Z->*D2->X->$U");
        auto ieid    = gum::ShaferShenoyLIMIDInference< double >(&infdiag);
        auto res     = ieid.reversePartialOrder();
        TS_ASSERT_EQUALS(res.size(), 2U)
        TS_ASSERT_EQUALS(res[0], gum::NodeSet({infdiag.idFromName("D2")}))
        TS_ASSERT_EQUALS(res[1], gum::NodeSet({infdiag.idFromName("D1")}))
      }
      {
        auto infdiag = gum::InfluenceDiagram< double >::fastPrototype("D1->Z->D2->X->$U");
        auto ieid    = gum::ShaferShenoyLIMIDInference< double >(&infdiag);
        auto res     = ieid.reversePartialOrder();
        TS_ASSERT_EQUALS(res.size(), 0U)
      }

      {
        auto infdiag = gum::InfluenceDiagram< double >::fastPrototype(
           "*D1->Z->*D2->U->*D3->V->*D4<-W<-*D5<-L;"
           "*D3<-M<-*D6->N->*D4<-*D2;X<-*D1->Y->D3;D5->$Q1<-W;"
           "U->$Q2<-D4;N->$Q3;X->$Q4<-D2;Q2<-*D7->Q4");
        auto ieid = gum::ShaferShenoyLIMIDInference< double >(&infdiag);
        auto res  = ieid.reversePartialOrder();
        TS_ASSERT_EQUALS(res.size(), 4U)
        TS_ASSERT_EQUALS(res[0], gum::NodeSet({infdiag.idFromName("D4"), infdiag.idFromName("D7")}))
        TS_ASSERT_EQUALS(res[1], gum::NodeSet({infdiag.idFromName("D3"), infdiag.idFromName("D5")}))
        TS_ASSERT_EQUALS(res[2], gum::NodeSet({infdiag.idFromName("D2"), infdiag.idFromName("D6")}))
        TS_ASSERT_EQUALS(res[3], gum::NodeSet({infdiag.idFromName("D1")}))
      }
    }

    void testSolvability() {
      {
        auto infdiag = gum::InfluenceDiagram< double >::fastPrototype("*D1->Z->*D2->X->$U<-Y");
        auto ieid    = gum::ShaferShenoyLIMIDInference< double >(&infdiag);
        TS_ASSERT(ieid.isSolvable())
      }
      {
        auto infdiag = gum::InfluenceDiagram< double >::fastPrototype("*D1->Z->*D2->X->$U<-Y<-*D3");
        auto ieid    = gum::ShaferShenoyLIMIDInference< double >(&infdiag);
        TS_ASSERT(!ieid.isSolvable())
      }
      {
        auto infdiag = gum::InfluenceDiagram< double >::fastPrototype("*D1->Z->*D2->X->$U<-Y<-*D3");
        auto ieid    = gum::ShaferShenoyLIMIDInference< double >(&infdiag);
        ieid.addNoForgettingAssumption({"D1", "D3", "D2"});
        TS_ASSERT(ieid.isSolvable())
      }
    }

    void testNoForgettingAssumption() {
      // From Evaluating IDs using LIMIDS, Nillson et Lauritzen, 2000
      auto infdiag = gum::InfluenceDiagram< double >::fastPrototype(
         "*D1->$U3<-R1->R2->R3<-*D4->$U4<-R4<-R1<-*D2;"
         "R4->D4<-*D3<-D2<-D1;"
         "D3->$U1<-R2;R3->$U2");
      auto ieid = gum::ShaferShenoyLIMIDInference< double >(&infdiag);

      TS_ASSERT(!ieid.hasNoForgettingAssumption())
      const auto revord = ieid.reversePartialOrder();
      auto       dag    = ieid.reducedGraph();
      TS_ASSERT(ieid.isSolvable())

      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D1")), infdiag.nodeset({}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D2")), infdiag.nodeset({"D1"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D3")), infdiag.nodeset({"D2"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D4")), infdiag.nodeset({"D3", "R4"}))

      TS_ASSERT_THROWS(ieid.addNoForgettingAssumption({"D11"}), gum::NotFound)
      TS_ASSERT_THROWS(ieid.addNoForgettingAssumption({"D4", "D1", "D2", "D3"}),
                       gum::InvalidDirectedCycle)
      TS_ASSERT_THROWS(ieid.addNoForgettingAssumption({"D1", "D2", "D3"}), gum::SizeError)

      TS_GUM_ASSERT_THROWS_NOTHING(ieid.addNoForgettingAssumption({"D1", "D2", "D3", "D4"}))
      TS_ASSERT(ieid.hasNoForgettingAssumption())

      dag = ieid.reducedGraph();

      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D1")), infdiag.nodeset({}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D2")), infdiag.nodeset({"D1"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D3")), infdiag.nodeset({"D2"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("D4")), infdiag.nodeset({"D2", "R4"}))


      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("R1")), infdiag.nodeset({"D2"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("R2")), infdiag.nodeset({"R1"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("R3")), infdiag.nodeset({"R2", "D4"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("R4")), infdiag.nodeset({"R1"}))

      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("U1")), infdiag.nodeset({"D3", "R2"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("U2")), infdiag.nodeset({"R3"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("U3")), infdiag.nodeset({"R1", "D1"}))
      TS_ASSERT_EQUALS(dag.parents(infdiag.idFromName("U4")), infdiag.nodeset({"R4", "D4"}))
    }

    void testNoForgettingAssumption2() {
      // from LIMIDS of decision Problems, Lauritzen et Nilsson, 1999
      // p33
      auto limids = gum::InfluenceDiagram< double >::fastPrototype(
         "a->c->e->g->*d4->l->$u4;c<-b->d->f->h->k<-*d3->$u2;$u1<-*d1->d->e->*d2->"
         "i->l;g->i;f->d2;b->d1;h->j->$u3<-k");
      std::vector< std::string > order({"d1", "d2", "d3", "d4"});

      auto       ieid    = gum::ShaferShenoyLIMIDInference< double >(&limids);
      const auto revord1 = ieid.reversePartialOrder();
      TS_ASSERT_EQUALS(revord1.size(), (gum::Size)2)
      TS_ASSERT_EQUALS(revord1[0], limids.nodeset({"d4", "d2", "d3"}))
      TS_ASSERT_EQUALS(revord1[1], limids.nodeset({"d1"}))

      TS_ASSERT(!ieid.isSolvable())

      ieid.addNoForgettingAssumption(order);
      TS_ASSERT(ieid.isSolvable())
      const auto revord2 = ieid.reversePartialOrder();
      TS_ASSERT_EQUALS(revord2.size(), (gum::Size)4)
      for (gum::Idx i = 0; i < (gum::Size)4; i++) {
        TS_ASSERT_EQUALS(revord2[i].size(), (gum::Size)1)
        TS_ASSERT_EQUALS(limids.variable(*(revord2[i].begin())).name(), order[3 - i])
      }
      auto noForgetting = ieid.reducedLIMID();

      TS_ASSERT_EQUALS(noForgetting.parents("d1"), limids.nodeset({"b"}))
      TS_ASSERT_EQUALS(noForgetting.parents("d2"), limids.nodeset({"e"}))
      TS_ASSERT_EQUALS(noForgetting.parents("d3"), limids.nodeset({"f"}))
      TS_ASSERT_EQUALS(noForgetting.parents("d4"), limids.nodeset({"d2", "g"}))
      // GUM_TRACE_VAR(noForgetting.toDot());
    }

    void testJunctionTree() {
      // From Evaluating IDs using LIMIDS, Nillson et Lauritzen, 2000
      auto infdiag = gum::InfluenceDiagram< double >::fastPrototype(
         "*D1->$U3<-R1->R2->R3<-*D4->$U4<-R4<-R1<-*D2;"
         "R4->D4<-*D3<-D2<-D1;"
         "D3->$U1<-R2;R3->$U2");
      auto ieid = gum::ShaferShenoyLIMIDInference< double >(&infdiag);

      ieid.addNoForgettingAssumption({"D1", "D2", "D3", "D4"});

      auto jt = ieid.junctionTree();
      TS_ASSERT_EQUALS(jt->size(), (gum::Size)5)
      ieid.makeInference();
    }

    void testPinball() {
      std::string                     file = GET_RESSOURCES_PATH("ID/Pinball.xml");
      gum::InfluenceDiagram< double > net;
      gum::BIFXMLIDReader< double >   reader(&net, file);
      reader.proceed();

      auto ieid = gum::ShaferShenoyLIMIDInference< double >(&net);
      ieid.makeInference();

      TS_ASSERT_EQUALS(ieid.posteriorUtility("BusinessDecision"),
                       (gum::Potential< double >() << net.variableFromName("BusinessDecision"))
                          .fillWith({181.0879, 276.25, 200}))


      TS_ASSERT_EQUALS(
         ieid.posterior("RentalRate"),
         (gum::Potential< double >() << net.variableFromName("RentalRate")).fillWith({0.25, 0.75}))

      TS_ASSERT_EQUALS(ieid.posterior("RevenueLevel"),
                       (gum::Potential< double >() << net.variableFromName("RevenueLevel"))
                          .fillWith({0.185, 0.63, 0.185}))
    }

    void testDavidAndescavage() {
      std::string                     file = GET_RESSOURCES_PATH("ID/testFromDavidAndescavage.xml");
      gum::InfluenceDiagram< double > net;
      gum::BIFXMLIDReader< double >   reader(&net, file);
      reader.proceed();

      auto ieid = gum::ShaferShenoyLIMIDInference< double >(&net);
      TS_ASSERT_THROWS(ieid.addEvidence("U", 0), gum::InvalidNode)
      TS_ASSERT_THROWS(
         ieid.addEvidence(
            (gum::Potential< double >() << net.variableFromName("DoTest")).fillWith({0.5, 1, 0})),
         gum::InvalidNode)
      TS_ASSERT_THROWS(ieid.addEvidence("DoTest", "Both"), gum::InvalidArgument)
      ieid.eraseAllEvidence();
      ieid.addEvidence("DoTest", "Both");
      ieid.addEvidence("FirstTest", "Positive");
      ieid.addEvidence("SecondTest", "Positive");
      ieid.makeInference();
      try {
        TS_ASSERT_EQUALS(
           ieid.posterior("DoTest"),
           (gum::Potential< double >() << net.variableFromName("DoTest")).fillWith({0, 0, 1}))
        // are this values correct ?
        TS_ASSERT_DELTA(ieid.MEU().first, -10.600002592, TS_GUM_SMALL_ERROR)
        TS_ASSERT_DELTA(ieid.MEU().second, 138.2398, 1e-4)
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    void testBugWithEvidence() {
      auto net = gum::InfluenceDiagram< double >::fastPrototype("c1<-c->$u<-*d");
      net.cpt("c").fillWith({0.5, 0.5});
      net.cpt("c1").fillWith({1, 0, 0, 1});
      net.utility("u").fillWith({10, 100, 21, 200});
      /*
            {
              auto ie = gum::ShaferShenoyLIMIDInference< double >(&net);
              ie.makeInference();
              TS_ASSERT_EQUALS(ie.optimalDecision("d"),
                               (gum::Potential< double >() <<
         net.variableFromName("d")) .fillWith({0, 1}))
              TS_ASSERT_EQUALS(ie.MEU().first, 110.5)
              TS_ASSERT_EQUALS(ie.posterior("c1"),
                               (gum::Potential< double >() <<
         net.variableFromName("c1")) .fillWith({0.5, 0.5})) TS_GUM_POTENTIAL_DELTA(
                 ie.posteriorUtility("d"),
                 (gum::Potential< double >() << net.variableFromName("d"))
                    .fillWith({55.0, 110.5}),
                 TS_GUM_SMALL_ERROR)
            }
            {
              auto ie = gum::ShaferShenoyLIMIDInference< double >(&net);
              ie.addEvidence("c", 1);
              ie.makeInference();
              TS_ASSERT_EQUALS(ie.optimalDecision("d"),
                               (gum::Potential< double >() <<
         net.variableFromName("d")) .fillWith({0, 1}))
              TS_ASSERT_EQUALS(ie.MEU().first, 200)
              TS_ASSERT_EQUALS(ie.posterior("c1"),
                               (gum::Potential< double >() <<
         net.variableFromName("c1")) .fillWith({0, 1})) TS_GUM_POTENTIAL_DELTA(
                 ie.posteriorUtility("d"),
                 (gum::Potential< double >() << net.variableFromName("d"))
                    .fillWith({100, 200}),
                 TS_GUM_SMALL_ERROR)
            }
            {
              auto ie = gum::ShaferShenoyLIMIDInference< double >(&net);
              ie.addEvidence("d", 1);
              ie.makeInference();

              TS_GUM_POTENTIAL_DELTA(
                 ie.optimalDecision("d"),
                 (gum::Potential< double >() << net.variableFromName("d"))
                    .fillWith({0, 1}),
                 TS_GUM_SMALL_ERROR)
              TS_ASSERT_EQUALS(ie.MEU().first, 110.5)
              TS_ASSERT_EQUALS(ie.posterior("c1"),
                               (gum::Potential< double >() <<
         net.variableFromName("c1")) .fillWith({0.5, 0.5})) TS_GUM_POTENTIAL_DELTA(
                 ie.posteriorUtility("d"),
                 (gum::Potential< double >() << net.variableFromName("d"))
                    .fillWith({0, 110.5}),
                 TS_GUM_SMALL_ERROR)
            }*/
      {
        auto ie = gum::ShaferShenoyLIMIDInference< double >(&net);
        ie.addEvidence("c1", std::vector< double >{0.8, 0.2});
        ie.makeInference();
        TS_ASSERT_EQUALS(ie.optimalDecision("d"),
                         (gum::Potential< double >() << net.variableFromName("d")).fillWith({0, 1}))
        TS_ASSERT_EQUALS(ie.MEU().first, 56.8)
        TS_ASSERT_DELTA(ie.MEU().second, 5126.56, TS_GUM_SMALL_ERROR)
        TS_GUM_POTENTIAL_DELTA(
           ie.posterior("c"),
           (gum::Potential< double >() << net.variableFromName("c")).fillWith({0.8, 0.2}),
           TS_GUM_SMALL_ERROR)
        TS_GUM_POTENTIAL_DELTA(
           ie.posteriorUtility("d"),
           (gum::Potential< double >() << net.variableFromName("d")).fillWith({28, 56.8}),
           TS_GUM_SMALL_ERROR)
      }
    }

    void testInferenceWithClemenFigure4_12() {
      try {
        std::string                     file = GET_RESSOURCES_PATH("ID/ClemenFigure04.12.xml");
        gum::InfluenceDiagram< double > net;
        gum::BIFXMLIDReader< double >   reader(&net, file);
        reader.proceed();

        gum::ShaferShenoyLIMIDInference< double > ie(&net);
        TS_GUM_ASSERT_THROWS_NOTHING(ie.makeInference())
        TS_GUM_POTENTIAL_DELTA(ie.posteriorUtility("Accept2B"),
                               (gum::Potential< double >() << net.variableFromName("Accept2B"))
                                  .fillWith({2.000, 4.6348}),
                               TS_GUM_SMALL_ERROR)
        TS_GUM_POTENTIAL_DELTA(
           ie.posterior("Court_Decision"),
           (gum::Potential< double >() << net.variableFromName("Court_Decision"))
              .fillWith({0.2, 0.5, 0.3}),
           TS_GUM_SMALL_ERROR)
        TS_GUM_POTENTIAL_DELTA(
           ie.posterior("Texaco_Reaction"),
           (gum::Potential< double >() << net.variableFromName("Texaco_Reaction"))
              .fillWith({0.17, 0.5, 0.33}),
           TS_GUM_SMALL_ERROR)
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    void testNegativeUtilityNonRegression() {
      auto fs = gum::InfluenceDiagram< double >::fastPrototype(
         "ActualUtility{U1|U2}->$Utility<-*Options{Opt0|Opt1}");
      fs.cpt("ActualUtility").fillWith(1).normalize();
      fs.utility("Utility").fillWith({-10, -10, -50, -50});

      auto ie = gum::ShaferShenoyLIMIDInference< double >(&fs);
      ie.makeInference();

      TS_GUM_POTENTIAL_DELTA(
         ie.optimalDecision("Options"),
         (gum::Potential< double >() << fs.variableFromName("Options")).fillWith({1, 0}),
         TS_GUM_SMALL_ERROR)
    }

    void testUtilityForDeterministicDecision() {
      auto defer = gum::InfluenceDiagram< double >::fastPrototype(
         "*D{D1|D2}->$L<-A{A1|A2}<-H->E{E1|E2|E3}->A;E->L");
      defer.cpt("H").fillWith(1).normalize();   // uniform for H
      defer.cpt("E").fillWith({0.9, 0.09, 0.01, 0.2, 0.09, 0.71});
      defer.cpt("A").fillWith({1, 0, 0, 1, 0.5, 0.5, 0.5, 0.5, 0, 1, 1, 0});
      defer.utility("L").fillWith({0, -0.1, -1, 0, -0.01, 0, -0.01, 0, 0, -1, -0.1, 0});

      auto ie = gum::ShaferShenoyLIMIDInference< double >(&defer);
      ie.makeInference();

      TS_GUM_POTENTIAL_DELTA(
         ie.posteriorUtility("D"),
         (gum::Potential< double >() << defer.variableFromName("D")).fillWith({-0.1014, -0.400}),
         1e-4)

      ie.addEvidence("E", "E1");
      ie.makeInference();
      TS_GUM_POTENTIAL_DELTA(
         ie.posteriorUtility("D"),
         (gum::Potential< double >() << defer.variableFromName("D")).fillWith({-0.1818, -0.0818}),
         1e-4)

      gum::InfluenceDiagram< double > defer2(defer);
      defer2.addArc("E", "D");
      auto ie2 = gum::ShaferShenoyLIMIDInference< double >(&defer2);
      ie2.addEvidence("E", "E1");
      ie2.makeInference();

      TS_GUM_POTENTIAL_DELTA(
         ie2.posteriorUtility("D"),
         (gum::Potential< double >() << defer2.variableFromName("D")).fillWith({-0.1818, -0.0818}),
         1e-4)
    }


    void testOtherNonRegression() {
      auto infdiag
         = gum::InfluenceDiagram< double >::fastPrototype("C1->*D1->C2->$U<-C3<-*D2<-C1->U");
      infdiag.cpt("C1").fillWith({0.6, 0.4});
      infdiag.cpt("C2").fillWith({0.85, 0.15, 0.7, 0.3});
      infdiag.cpt("C3").fillWith({0.45, 0.55, 0.80718672, 0.19281328});
      infdiag.utility("U").fillWith({6.7, 22.6, 28.5, 27, 24, -0.33, 3.67, 33.7});

      auto ie = gum::ShaferShenoyLIMIDInference< double >(&infdiag);
      ie.addNoForgettingAssumption(std::vector< std::string >{"D1", "D2"});
      ie.makeInference();
      auto q  = ie.posterior("U") * ie.posteriorUtility("U");
      auto qq = q * ie.posteriorUtility("U");

      auto m   = q.sum();
      auto m2  = qq.sum();
      auto meu = ie.MEU();
      TS_ASSERT_DELTA(meu.first, m, TS_GUM_SMALL_ERROR)
      TS_ASSERT_DELTA(meu.second, m2 - m * m, TS_GUM_SMALL_ERROR)
    }
  };
}   // namespace gum_tests
