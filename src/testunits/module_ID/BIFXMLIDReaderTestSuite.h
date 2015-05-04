/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#define TIXML_USE_TICPP

#include <iostream>
#include <string>
#include <fstream>
#include <iostream>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/ID/influenceDiagram.h>
#include <agrum/ID/io/BIFXML/BIFXMLIDReader.h>

// The graph used for the tests:
//           D1
//           |          D1 -> C1
//           C1
//          /  \        C1 -> C2 & C1 -> U1
//        C2   U1
//       /  \ /         C2 -> D3 & C2 -> D2 & D2 -> U1
//      D3  D2
//      |    |          D3 -> C3 & D2 -> C4
//     C3    C4
//       \  /           C3 -> C5 & C4 -> C5
//        C5   D4
//         \  /         C5 -> U2 & D4 -> U2
//          U2

namespace gum_tests {

  class aSimpleListener : public gum::Listener {
    private:
    int __nbr;

    public:
    aSimpleListener() : __nbr(0){};
    void whenProceeding(const void *buffer, int percent, std::string status) {
      __nbr = percent;
      // std::cout << "Progress : " << percent << "%" << " Status : " << status <<
      // std::endl;
    }
    int getNbr() { return __nbr; };
  };

  class BIFXMLIDReaderTestSuite : public CxxTest::TestSuite {
    public:
    void testConstuctor() {
      std::string file = GET_PATH_STR("IDBIFXMLIO_file.xml");

      gum::InfluenceDiagram<float> net;

      gum::BIFXMLIDReader<float> *reader = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(reader =
                                       new gum::BIFXMLIDReader<float>(&net, file));

      TS_GUM_ASSERT_THROWS_NOTHING(delete reader);

      // delete reader;
    }

    void testRead_file1() {
      std::string file = GET_PATH_STR("IDBIFXMLIO_file.xml");

      gum::InfluenceDiagram<float> *net = new gum::InfluenceDiagram<float>();

      gum::BIFXMLIDReader<float> reader(net, file);

      TS_GUM_ASSERT_THROWS_NOTHING(reader.proceed());

      TS_ASSERT(net != 0);

      // For comparison with what have wrote Writer
      std::string dotfile = GET_PATH_STR("IDToDotReader.dot");
      std::ofstream output(dotfile.c_str(), std::ios::out | std::ios::trunc);

      if (!output.good()) {
        GUM_ERROR(gum::IOError, "Stream states flags are not all unset.");
      }

      output << net->toDot();
      output.flush();
      output.close();

      if (output.fail()) {
        GUM_ERROR(gum::IOError, "Writting in the ostream failed.");
      }

      if (net != 0) {
        TS_ASSERT(!net->empty());
      }

      delete net;
    }

    void testRead_file2_float() {
      std::string file = GET_PATH_STR("IDBIFXMLIO_file.xml");

      gum::InfluenceDiagram<float> *net = new gum::InfluenceDiagram<float>();

      gum::BIFXMLIDReader<float> reader(net, file);

      TS_GUM_ASSERT_THROWS_NOTHING(reader.proceed());

      TS_ASSERT(net != 0);

      if (net != 0) {
        gum::HashTable<std::string, gum::Id> idMap;

        for (const auto node : net->nodes())
          idMap.insert(net->variable(node).name(), node);

        gum::Id decisionVar1Id = idMap["decisionVar1"];
        TS_ASSERT(net->isDecisionNode(decisionVar1Id));
        TS_ASSERT(!net->isUtilityNode(decisionVar1Id));
        TS_ASSERT(!net->isChanceNode(decisionVar1Id));
        const gum::DiscreteVariable &decisionVar1 = net->variable(decisionVar1Id);
        TS_ASSERT_EQUALS(decisionVar1.name(), "decisionVar1");
        TS_ASSERT_EQUALS(decisionVar1.domainSize(), (gum::Size)2);
        TS_ASSERT_EQUALS(decisionVar1.label(0), "0");
        TS_ASSERT_EQUALS(decisionVar1.label(1), "1");

        gum::Id decisionVar2Id = idMap["decisionVar2"];
        TS_ASSERT(net->isDecisionNode(decisionVar2Id));
        TS_ASSERT(!net->isUtilityNode(decisionVar2Id));
        TS_ASSERT(!net->isChanceNode(decisionVar2Id));
        const gum::DiscreteVariable &decisionVar2 = net->variable(decisionVar2Id);
        TS_ASSERT_EQUALS(decisionVar2.name(), "decisionVar2");
        TS_ASSERT_EQUALS(decisionVar2.domainSize(), (gum::Size)2);
        TS_ASSERT_EQUALS(decisionVar2.label(0), "0");
        TS_ASSERT_EQUALS(decisionVar2.label(1), "1");

        gum::Id decisionVar3Id = idMap["decisionVar3"];
        TS_ASSERT(net->isDecisionNode(decisionVar3Id));
        TS_ASSERT(!net->isUtilityNode(decisionVar3Id));
        TS_ASSERT(!net->isChanceNode(decisionVar3Id));
        const gum::DiscreteVariable &decisionVar3 = net->variable(decisionVar3Id);
        TS_ASSERT_EQUALS(decisionVar3.name(), "decisionVar3");
        TS_ASSERT_EQUALS(decisionVar3.domainSize(), (gum::Size)2);
        TS_ASSERT_EQUALS(decisionVar3.label(0), "0");
        TS_ASSERT_EQUALS(decisionVar3.label(1), "1");

        gum::Id decisionVar4Id = idMap["decisionVar4"];
        TS_ASSERT(net->isDecisionNode(decisionVar4Id));
        TS_ASSERT(!net->isUtilityNode(decisionVar4Id));
        TS_ASSERT(!net->isChanceNode(decisionVar4Id));
        const gum::DiscreteVariable &decisionVar4 = net->variable(decisionVar4Id);
        TS_ASSERT_EQUALS(decisionVar4.name(), "decisionVar4");
        TS_ASSERT_EQUALS(decisionVar4.domainSize(), (gum::Size)2);
        TS_ASSERT_EQUALS(decisionVar4.label(0), "0");
        TS_ASSERT_EQUALS(decisionVar4.label(1), "1");

        gum::Id chanceVar1Id = idMap["chanceVar1"];
        TS_ASSERT(!net->isDecisionNode(chanceVar1Id));
        TS_ASSERT(!net->isUtilityNode(chanceVar1Id));
        TS_ASSERT(net->isChanceNode(chanceVar1Id));
        const gum::DiscreteVariable &chanceVar1 = net->variable(chanceVar1Id);
        TS_ASSERT_EQUALS(chanceVar1.name(), "chanceVar1");
        TS_ASSERT_EQUALS(chanceVar1.domainSize(), (gum::Size)2);
        TS_ASSERT_EQUALS(chanceVar1.label(0), "0");
        TS_ASSERT_EQUALS(chanceVar1.label(1), "1");

        gum::Id chanceVar2Id = idMap["chanceVar2"];
        TS_ASSERT(!net->isDecisionNode(chanceVar2Id));
        TS_ASSERT(!net->isUtilityNode(chanceVar2Id));
        TS_ASSERT(net->isChanceNode(chanceVar2Id));
        const gum::DiscreteVariable &chanceVar2 = net->variable(chanceVar2Id);
        TS_ASSERT_EQUALS(chanceVar2.name(), "chanceVar2");
        TS_ASSERT_EQUALS(chanceVar2.domainSize(), (gum::Size)2);
        TS_ASSERT_EQUALS(chanceVar2.label(0), "0");
        TS_ASSERT_EQUALS(chanceVar2.label(1), "1");

        gum::Id chanceVar3Id = idMap["chanceVar3"];
        TS_ASSERT(!net->isDecisionNode(chanceVar3Id));
        TS_ASSERT(!net->isUtilityNode(chanceVar3Id));
        TS_ASSERT(net->isChanceNode(chanceVar3Id));
        const gum::DiscreteVariable &chanceVar3 = net->variable(chanceVar3Id);
        TS_ASSERT_EQUALS(chanceVar3.name(), "chanceVar3");
        TS_ASSERT_EQUALS(chanceVar3.domainSize(), (gum::Size)2);
        TS_ASSERT_EQUALS(chanceVar3.label(0), "0");
        TS_ASSERT_EQUALS(chanceVar3.label(1), "1");

        gum::Id chanceVar4Id = idMap["chanceVar4"];
        TS_ASSERT(!net->isDecisionNode(chanceVar4Id));
        TS_ASSERT(!net->isUtilityNode(chanceVar4Id));
        TS_ASSERT(net->isChanceNode(chanceVar4Id));
        const gum::DiscreteVariable &chanceVar4 = net->variable(chanceVar4Id);
        TS_ASSERT_EQUALS(chanceVar4.name(), "chanceVar4");
        TS_ASSERT_EQUALS(chanceVar4.domainSize(), (gum::Size)2);
        TS_ASSERT_EQUALS(chanceVar4.label(0), "0");
        TS_ASSERT_EQUALS(chanceVar4.label(1), "1");

        gum::Id chanceVar5Id = idMap["chanceVar5"];
        TS_ASSERT(!net->isDecisionNode(chanceVar5Id));
        TS_ASSERT(!net->isUtilityNode(chanceVar5Id));
        TS_ASSERT(net->isChanceNode(chanceVar5Id));
        const gum::DiscreteVariable &chanceVar5 = net->variable(chanceVar5Id);
        TS_ASSERT_EQUALS(chanceVar5.name(), "chanceVar5");
        TS_ASSERT_EQUALS(chanceVar5.domainSize(), (gum::Size)2);
        TS_ASSERT_EQUALS(chanceVar5.label(0), "0");
        TS_ASSERT_EQUALS(chanceVar5.label(1), "1");

        gum::Id utilityVar1Id = idMap["utilityVar1"];
        TS_ASSERT(!net->isDecisionNode(utilityVar1Id));
        TS_ASSERT(net->isUtilityNode(utilityVar1Id));
        TS_ASSERT(!net->isChanceNode(utilityVar1Id));
        const gum::DiscreteVariable &utilityVar1 = net->variable(utilityVar1Id);
        TS_ASSERT_EQUALS(utilityVar1.name(), "utilityVar1");
        TS_ASSERT_EQUALS(utilityVar1.domainSize(), (gum::Size)1);
        TS_ASSERT_EQUALS(utilityVar1.label(0), "0");

        gum::Id utilityVar2Id = idMap["utilityVar2"];
        TS_ASSERT(!net->isDecisionNode(utilityVar2Id));
        TS_ASSERT(net->isUtilityNode(utilityVar2Id));
        TS_ASSERT(!net->isChanceNode(utilityVar2Id));
        const gum::DiscreteVariable &utilityVar2 = net->variable(utilityVar2Id);
        TS_ASSERT_EQUALS(utilityVar2.name(), "utilityVar2");
        TS_ASSERT_EQUALS(utilityVar2.domainSize(), (gum::Size)1);
        TS_ASSERT_EQUALS(utilityVar2.label(0), "0");

        const gum::Potential<float> &cptChanceVar1 = net->cpt(idMap["chanceVar1"]);
        TS_ASSERT_EQUALS(cptChanceVar1.domainSize(), (gum::Size)4);
        gum::Instantiation instChanceVar1(cptChanceVar1);
        instChanceVar1.chgVal(decisionVar1, 0);
        instChanceVar1.chgVal(chanceVar1, 0);
        TS_ASSERT_DELTA(cptChanceVar1[instChanceVar1], 0.2, 0.001);
        instChanceVar1.chgVal(chanceVar1, 1);
        TS_ASSERT_DELTA(cptChanceVar1[instChanceVar1], 0.8, 0.001);
        instChanceVar1.chgVal(decisionVar1, 1);
        instChanceVar1.chgVal(chanceVar1, 0);
        TS_ASSERT_DELTA(cptChanceVar1[instChanceVar1], 0.5, 0.001);
        instChanceVar1.chgVal(chanceVar1, 1);
        TS_ASSERT_DELTA(cptChanceVar1[instChanceVar1], 0.5, 0.001);

        const gum::Potential<float> &cptChanceVar2 = net->cpt(idMap["chanceVar2"]);
        TS_ASSERT_EQUALS(cptChanceVar2.domainSize(), (gum::Size)4);
        gum::Instantiation instChanceVar2(cptChanceVar2);
        instChanceVar2.chgVal(chanceVar1, 0);
        instChanceVar2.chgVal(chanceVar2, 0);
        TS_ASSERT_DELTA(cptChanceVar2[instChanceVar2], 0.1, 0.001);
        instChanceVar2.chgVal(chanceVar2, 1);
        TS_ASSERT_DELTA(cptChanceVar2[instChanceVar2], 0.9, 0.001);
        instChanceVar2.chgVal(chanceVar1, 1);
        instChanceVar2.chgVal(chanceVar2, 0);
        TS_ASSERT_DELTA(cptChanceVar2[instChanceVar2], 0.9, 0.001);
        instChanceVar2.chgVal(chanceVar2, 1);
        TS_ASSERT_DELTA(cptChanceVar2[instChanceVar2], 0.1, 0.001);

        const gum::Potential<float> &cptChanceVar3 = net->cpt(idMap["chanceVar3"]);
        TS_ASSERT_EQUALS(cptChanceVar3.domainSize(), (gum::Size)4);
        gum::Instantiation instChanceVar3(cptChanceVar3);
        instChanceVar3.chgVal(decisionVar3, 0);
        instChanceVar3.chgVal(chanceVar3, 0);
        TS_ASSERT_DELTA(cptChanceVar3[instChanceVar3], 0.35, 0.001);
        instChanceVar3.chgVal(chanceVar3, 1);
        TS_ASSERT_DELTA(cptChanceVar3[instChanceVar3], 0.65, 0.001);
        instChanceVar3.chgVal(decisionVar3, 1);
        instChanceVar3.chgVal(chanceVar3, 0);
        TS_ASSERT_DELTA(cptChanceVar3[instChanceVar3], 0.19, 0.001);
        instChanceVar3.chgVal(chanceVar3, 1);
        TS_ASSERT_DELTA(cptChanceVar3[instChanceVar3], 0.81, 0.001);

        const gum::Potential<float> &cptChanceVar4 = net->cpt(idMap["chanceVar4"]);
        TS_ASSERT_EQUALS(cptChanceVar4.domainSize(), (gum::Size)4);
        gum::Instantiation instChanceVar4(cptChanceVar4);
        instChanceVar4.chgVal(decisionVar2, 0);
        instChanceVar4.chgVal(chanceVar4, 0);
        TS_ASSERT_DELTA(cptChanceVar4[instChanceVar4], 0.4, 0.001);
        instChanceVar4.chgVal(chanceVar4, 1);
        TS_ASSERT_DELTA(cptChanceVar4[instChanceVar4], 0.6, 0.001);
        instChanceVar4.chgVal(decisionVar2, 1);
        instChanceVar4.chgVal(chanceVar4, 0);
        TS_ASSERT_DELTA(cptChanceVar4[instChanceVar4], 0.5, 0.001);
        instChanceVar4.chgVal(chanceVar4, 1);
        TS_ASSERT_DELTA(cptChanceVar4[instChanceVar4], 0.5, 0.001);

        const gum::Potential<float> &cptChanceVar5 = net->cpt(idMap["chanceVar5"]);
        TS_ASSERT_EQUALS(cptChanceVar5.domainSize(), (gum::Size)8);
        gum::Instantiation instChanceVar5(cptChanceVar5);
        instChanceVar5.chgVal(chanceVar4, 0);
        instChanceVar5.chgVal(chanceVar3, 0);
        instChanceVar5.chgVal(chanceVar5, 0);
        TS_ASSERT_DELTA(cptChanceVar5[instChanceVar5], 0.4, 0.001);
        instChanceVar5.chgVal(chanceVar5, 1);
        TS_ASSERT_DELTA(cptChanceVar5[instChanceVar5], 0.6, 0.001);
        instChanceVar5.chgVal(chanceVar3, 1);
        instChanceVar5.chgVal(chanceVar5, 0);
        TS_ASSERT_DELTA(cptChanceVar5[instChanceVar5], 0.8, 0.001);
        instChanceVar5.chgVal(chanceVar5, 1);
        TS_ASSERT_DELTA(cptChanceVar5[instChanceVar5], 0.2, 0.001);
        instChanceVar5.chgVal(chanceVar4, 1);
        instChanceVar5.chgVal(chanceVar3, 0);
        instChanceVar5.chgVal(chanceVar5, 0);
        TS_ASSERT_DELTA(cptChanceVar5[instChanceVar5], 0.4, 0.001);
        instChanceVar5.chgVal(chanceVar5, 1);
        TS_ASSERT_DELTA(cptChanceVar5[instChanceVar5], 0.6, 0.001);
        instChanceVar5.chgVal(chanceVar3, 1);
        instChanceVar5.chgVal(chanceVar5, 0);
        TS_ASSERT_DELTA(cptChanceVar5[instChanceVar5], 0.3, 0.001);
        instChanceVar5.chgVal(chanceVar5, 1);
        TS_ASSERT_DELTA(cptChanceVar5[instChanceVar5], 0.7, 0.001);

        const gum::UtilityTable<float> &utUtilityVar1 =
            net->utility(idMap["utilityVar1"]);
        TS_ASSERT_EQUALS(utUtilityVar1.domainSize(), (gum::Size)4);
        gum::Instantiation instUtilityVar1(utUtilityVar1);
        instUtilityVar1.chgVal(utilityVar1, 0);
        instUtilityVar1.chgVal(decisionVar2, 0);
        instUtilityVar1.chgVal(chanceVar1, 0);
        TS_ASSERT_DELTA(utUtilityVar1[instUtilityVar1], 42, 0.001);
        instUtilityVar1.chgVal(chanceVar1, 1);
        TS_ASSERT_DELTA(utUtilityVar1[instUtilityVar1], 666, 0.001);
        instUtilityVar1.chgVal(decisionVar2, 1);
        instUtilityVar1.chgVal(chanceVar1, 0);
        TS_ASSERT_DELTA(utUtilityVar1[instUtilityVar1], 69, 0.001);
        instUtilityVar1.chgVal(chanceVar1, 1);
        TS_ASSERT_DELTA(utUtilityVar1[instUtilityVar1], 84, 0.001);

        const gum::UtilityTable<float> &utUtilityVar2 =
            net->utility(idMap["utilityVar2"]);
        TS_ASSERT_EQUALS(utUtilityVar2.domainSize(), (gum::Size)4);
        gum::Instantiation instUtilityVar2(utUtilityVar2);
        instUtilityVar2.chgVal(utilityVar2, 0);
        instUtilityVar2.chgVal(decisionVar4, 0);
        instUtilityVar2.chgVal(chanceVar5, 0);
        TS_ASSERT_DELTA(utUtilityVar2[instUtilityVar2], 42, 0.001);
        instUtilityVar2.chgVal(chanceVar5, 1);
        TS_ASSERT_DELTA(utUtilityVar2[instUtilityVar2], -69, 0.001);
        instUtilityVar2.chgVal(decisionVar4, 1);
        instUtilityVar2.chgVal(chanceVar5, 0);
        TS_ASSERT_DELTA(utUtilityVar2[instUtilityVar2], 666, 0.001);
        instUtilityVar2.chgVal(chanceVar5, 1);
        TS_ASSERT_DELTA(utUtilityVar2[instUtilityVar2], 84, 0.001);
      }

      delete net;
    }

    void testRead_file3_float() {
      std::string file = GET_PATH_STR("IDBIFXMLIO_file.xml");

      gum::InfluenceDiagram<float> *net = new gum::InfluenceDiagram<float>();

      gum::BIFXMLIDReader<float> reader(net, file);

      aSimpleListener asl;
      GUM_CONNECT(reader, onProceed, asl, aSimpleListener::whenProceeding);

      TS_GUM_ASSERT_THROWS_NOTHING(reader.proceed());

      TS_ASSERT_EQUALS(asl.getNbr(), 100);
      delete net;
    }
  };
}
