/***************************************************************************
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
 *   (gumSize) 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.   *
 ***************************************************************************/
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/ID/influenceDiagram.h>
#include <agrum/ID/generator/influenceDiagramGenerator.h>
#include <agrum/variables/discreteVariable.h>
#include <agrum/graphs/graphElements.h>
#include <agrum/variables/labelizedVariable.h>

#include <testsuite_utils.h>

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

  class InfluenceDiagramTestSuite : public CxxTest::TestSuite {

    private:
    void fillTopo(gum::InfluenceDiagram<float> &id, gum::List<gum::NodeId> &idList) {
      try {
        idList.insert(id.addDecisionNode(*decisionVar1)); // 0
        idList.insert(id.addDecisionNode(*decisionVar2)); // 1
        idList.insert(id.addDecisionNode(*decisionVar3)); // 2
        idList.insert(id.addDecisionNode(*decisionVar4)); // 3
        idList.insert(id.addChanceNode(*chanceVar1));     // 4
        idList.insert(id.addChanceNode(*chanceVar2));     // 5
        idList.insert(id.addChanceNode(*chanceVar3));     // 6
        idList.insert(id.addChanceNode(*chanceVar4));     // 7
        idList.insert(id.addChanceNode(*chanceVar5));     // 8
        idList.insert(id.addUtilityNode(*utilityVar1));   // 9
        idList.insert(id.addUtilityNode(*utilityVar2));   // 10

        id.addArc(idList[0], idList[4]);
        id.addArc(idList[4], idList[9]);
        id.addArc(idList[4], idList[5]);
        id.addArc(idList[5], idList[1]);
        id.addArc(idList[5], idList[2]);
        id.addArc(idList[1], idList[9]);
        id.addArc(idList[1], idList[7]);
        id.addArc(idList[2], idList[6]);
        id.addArc(idList[6], idList[8]);
        id.addArc(idList[7], idList[8]);
        id.addArc(idList[8], idList[10]);
        id.addArc(idList[3], idList[10]);

      } catch (gum::Exception &e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    void fill(gum::InfluenceDiagram<float> &id, gum::List<gum::NodeId> &idList) {
      fillTopo(id, idList);

      try {
        const gum::Potential<float> &p1 = id.cpt(idList[4]);
        {
          // FILLING PARAMS
          const float t[4] = {0.2, 0.8, 0.5, 0.5};
          int n = 4;
          const std::vector<float> v(t, t + n);
          p1.fillWith(v);
        }

        const gum::Potential<float> &p2 = id.cpt(idList[5]);
        {
          // FILLING PARAMS
          const float t[4] = {0.1, 0.9, 0.9, 0.1};
          int n = 4;
          const std::vector<float> v(t, t + n);
          p2.fillWith(v);
        }

        const gum::Potential<float> &p3 = id.cpt(idList[6]);
        {
          // FILLING PARAMS
          const float t[4] = {0.35, 0.65, 0.19, 0.81};
          int n = 4;
          const std::vector<float> v(t, t + n);
          p3.fillWith(v);
        }

        const gum::Potential<float> &p4 = id.cpt(idList[7]);
        {
          // FILLING PARAMS
          const float t[4] = {0.4, 0.6, 0.5, 0.5};
          int n = 4;
          const std::vector<float> v(t, t + n);
          p4.fillWith(v);
        }

        const gum::Potential<float> &p5 = id.cpt(idList[8]);
        {
          // FILLING PARAMS
          const float t[8] = {0.4, 0.6, 0.8, 0.2, 0.4, 0.6, 0.3, 0.7};
          int n = 8;
          const std::vector<float> v(t, t + n);
          p5.fillWith(v);
        }

        const gum::UtilityTable<float> &u1 = id.utility(idList[9]);
        {
          // FILLING PARAMS
          const float t[4] = {42, 69, 666, 84};
          int n = 4;
          const std::vector<float> v(t, t + n);
          u1.fillWith(v);
        }

        const gum::UtilityTable<float> &u2 = id.utility(idList[10]);
        {
          // FILLING PARAMS
          const float t[4] = {42, -69, 666, 84};
          int n = 4;
          const std::vector<float> v(t, t + n);
          u2.fillWith(v);
        }
      } catch (gum::Exception &e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    public:
    gum::LabelizedVariable *decisionVar1, *decisionVar2, *decisionVar3,
        *decisionVar4;
    gum::LabelizedVariable *chanceVar1, *chanceVar2, *chanceVar3, *chanceVar4,
        *chanceVar5;
    gum::LabelizedVariable *utilityVar1, *utilityVar2;

    void setUp() {
      decisionVar1 = new gum::LabelizedVariable("decisionVar1", "D1", 2);
      decisionVar2 = new gum::LabelizedVariable("decisionVar2", "D2", 2);
      decisionVar3 = new gum::LabelizedVariable("decisionVar3", "D3", 2);
      decisionVar4 = new gum::LabelizedVariable("decisionVar4", "D4", 2);
      chanceVar1 = new gum::LabelizedVariable("chanceVar1", "C1", 2);
      chanceVar2 = new gum::LabelizedVariable("chanceVar2", "C2", 2);
      chanceVar3 = new gum::LabelizedVariable("chanceVar3", "C3", 2);
      chanceVar4 = new gum::LabelizedVariable("chanceVar4", "C4", 2);
      chanceVar5 = new gum::LabelizedVariable("chanceVar5", "C5", 2);
      utilityVar1 = new gum::LabelizedVariable("utilityVar1", "U1", 1);
      utilityVar2 = new gum::LabelizedVariable("utilityVar2", "U2", 1);
    }

    void tearDown() {
      delete decisionVar1;
      delete decisionVar2;
      delete decisionVar3;
      delete decisionVar4;
      delete chanceVar1;
      delete chanceVar2;
      delete chanceVar3;
      delete chanceVar4;
      delete chanceVar5;
      delete utilityVar1;
      delete utilityVar2;
    }

    void testConstructor() {
      gum::InfluenceDiagram<float> *topology = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(topology = new gum::InfluenceDiagram<float>());

      TS_ASSERT_THROWS(topology->addArc(1, 2), gum::InvalidNode);
      gum::List<gum::NodeId> idList;
      TS_GUM_ASSERT_THROWS_NOTHING(fill(*topology, idList));

      TS_ASSERT_THROWS(topology->add(*decisionVar1), gum::DuplicateLabel);

      TS_GUM_ASSERT_THROWS_NOTHING(delete topology);
    }

    void testNodeType() {
      gum::InfluenceDiagram<float> topology;
      gum::List<gum::NodeId> idList;
      TS_GUM_ASSERT_THROWS_NOTHING(fill(topology, idList));

      TS_ASSERT(topology.isDecisionNode(idList[0]));
      TS_ASSERT(topology.isDecisionNode(idList[1]));
      TS_ASSERT(topology.isDecisionNode(idList[2]));
      TS_ASSERT(topology.isDecisionNode(idList[3]));
      TS_ASSERT(!topology.isDecisionNode(idList[4]));
      TS_ASSERT(!topology.isDecisionNode(idList[5]));
      TS_ASSERT(!topology.isDecisionNode(idList[6]));
      TS_ASSERT(!topology.isDecisionNode(idList[7]));
      TS_ASSERT(!topology.isDecisionNode(idList[8]));
      TS_ASSERT(!topology.isDecisionNode(idList[9]));
      TS_ASSERT(!topology.isDecisionNode(idList[10]));

      TS_ASSERT(!topology.isChanceNode(idList[0]));
      TS_ASSERT(!topology.isChanceNode(idList[1]));
      TS_ASSERT(!topology.isChanceNode(idList[2]));
      TS_ASSERT(!topology.isChanceNode(idList[3]));
      TS_ASSERT(topology.isChanceNode(idList[4]));
      TS_ASSERT(topology.isChanceNode(idList[5]));
      TS_ASSERT(topology.isChanceNode(idList[6]));
      TS_ASSERT(topology.isChanceNode(idList[7]));
      TS_ASSERT(topology.isChanceNode(idList[8]));
      TS_ASSERT(!topology.isChanceNode(idList[9]));
      TS_ASSERT(!topology.isChanceNode(idList[10]));

      TS_ASSERT(!topology.isUtilityNode(idList[0]));
      TS_ASSERT(!topology.isUtilityNode(idList[1]));
      TS_ASSERT(!topology.isUtilityNode(idList[2]));
      TS_ASSERT(!topology.isUtilityNode(idList[3]));
      TS_ASSERT(!topology.isUtilityNode(idList[4]));
      TS_ASSERT(!topology.isUtilityNode(idList[5]));
      TS_ASSERT(!topology.isUtilityNode(idList[6]));
      TS_ASSERT(!topology.isUtilityNode(idList[7]));
      TS_ASSERT(!topology.isUtilityNode(idList[8]));
      TS_ASSERT(topology.isUtilityNode(idList[9]));
      TS_ASSERT(topology.isUtilityNode(idList[10]));

      TS_ASSERT_EQUALS(topology.chanceNodeSize(), (gum::Size)5);
      TS_ASSERT_EQUALS(topology.decisionNodeSize(), (gum::Size)4);
      TS_ASSERT_EQUALS(topology.utilityNodeSize(), (gum::Size)2);
    }

    void testToDot() {
      gum::InfluenceDiagram<float> id;
      gum::List<gum::NodeId> idList;
      fill(id, idList);

      // For comparison with what readers will return
      std::string dotfile = GET_PATH_STR("IDToDotWriter.dot");
      std::ofstream output(dotfile.c_str(), std::ios::out | std::ios::trunc);

      if (!output.good()) {
        GUM_ERROR(gum::IOError, "Stream states flags are not all unset.");
      }

      output << id.toDot();

      output.flush();

      output.close();

      if (output.fail()) {
        GUM_ERROR(gum::IOError, "Writting in the ostream failed.");
      }
    }

    void testCopyConstructor() {
      gum::InfluenceDiagram<float> source;
      gum::List<gum::NodeId> idList;
      TS_GUM_ASSERT_THROWS_NOTHING(fill(source, idList));

      gum::InfluenceDiagram<float> *copy = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(copy = new gum::InfluenceDiagram<float>(source));

      TS_ASSERT_EQUALS(source.dag().size(), copy->dag().size());
      TS_ASSERT_EQUALS(source.dag().sizeArcs(), copy->dag().sizeArcs());

      for (const auto node : source.dag().nodes()) {
        TS_ASSERT(copy->dag().exists(node));

        const gum::DiscreteVariable &srcVar = source.variable(node);
        const gum::DiscreteVariable &cpVar = copy->variable(node);
        TS_ASSERT_EQUALS(srcVar.name(), cpVar.name());

        if (srcVar.domainSize() == cpVar.domainSize()) {
          for (gum::Idx i = 0; i < srcVar.domainSize(); i++)
            TS_ASSERT_EQUALS(srcVar.label(i), cpVar.label(i));
        } else
          TS_ASSERT(false);

        for (const auto parent : source.dag().parents(node)) {
          TS_ASSERT(copy->dag().existsArc(parent, node));
        }

        if (source.isChanceNode(node)) {
          const gum::Potential<float> &srcCPT = source.cpt(node);

          const gum::Potential<float> &cpCPT = copy->cpt(node);

          gum::Instantiation srcInst(srcCPT);

          gum::Instantiation cpInst(cpCPT);

          for (cpInst.setFirst(); !cpInst.end(); cpInst.inc()) {
            for (gum::Idx i = 0; i < cpInst.nbrDim(); i++) {
              gum::Id id = copy->nodeId(cpInst.variable(i));
              srcInst.chgVal(source.variable(id), cpInst.val(i));
            }

            TS_ASSERT_EQUALS(cpCPT[cpInst], srcCPT[srcInst]);
          }

        } else if (source.isUtilityNode(node)) {
          const gum::UtilityTable<float> &srcUT = source.utility(node);
          const gum::UtilityTable<float> &cpUT = copy->utility(node);

          gum::Instantiation srcInst(srcUT);
          gum::Instantiation cpInst(cpUT);

          for (cpInst.setFirst(); !cpInst.end(); cpInst.inc()) {
            for (gum::Idx i = 0; i < cpInst.nbrDim(); i++) {
              gum::Id id = copy->nodeId(cpInst.variable(i));
              srcInst.chgVal(source.variable(id), cpInst.val(i));
            }

            TS_ASSERT_EQUALS(cpUT[cpInst], srcUT[srcInst]);
          }
        }
      }

      if (copy)
        delete copy;
    }

    void testInsertion_1() {

      gum::InfluenceDiagram<float> id;
      gum::List<gum::NodeId> idList;

      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(id.addDecisionNode(*decisionVar1)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(id.addDecisionNode(*decisionVar3)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(id.addChanceNode(*chanceVar3)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(id.addChanceNode(*chanceVar5)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(id.addUtilityNode(*utilityVar1)));

      TS_ASSERT_EQUALS(id.size(), (gum::Size)5);
      TS_ASSERT_EQUALS(id.dag().size(), (gum::Size)5);

      // Test for uniqueness of the ids

      for (gum::Idx i = 0; i < idList.size() - 1; i++) {
        for (gum::Idx j = i + 1; j < idList.size(); j++) {
          if (idList[i] != idList[j]) {
            TS_ASSERT(true);
          } else {
            TS_ASSERT(false);
          }
        }
      }

      gum::LabelizedVariable *varPtr = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          varPtr = (gum::LabelizedVariable *)&id.variable(idList[0]));
      TS_ASSERT_EQUALS(*varPtr, *decisionVar1);

      TS_ASSERT_EQUALS(*((gum::LabelizedVariable *)&id.variable(idList[0])),
                       *decisionVar1);
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable *)&id.variable(idList[1])),
                       *decisionVar3);
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable *)&id.variable(idList[2])),
                       *chanceVar3);
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable *)&id.variable(idList[3])),
                       *chanceVar5);
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable *)&id.variable(idList[4])),
                       *utilityVar1);
    }

    void testInsertion_2() {
      gum::InfluenceDiagram<float> id;
      gum::List<gum::NodeId> idList;

      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(id.addDecisionNode(*decisionVar2)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(id.addDecisionNode(*decisionVar4)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(id.addChanceNode(*chanceVar1)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(id.addChanceNode(*chanceVar4)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(id.addUtilityNode(*utilityVar2)));

      TS_ASSERT_EQUALS(id.size(), (gum::Size)5);
      TS_ASSERT_EQUALS(id.dag().size(), (gum::Size)5);

      gum::LabelizedVariable *varPtr = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
          varPtr = (gum::LabelizedVariable *)&id.variable(idList[0]));
      TS_ASSERT_EQUALS(*varPtr, *decisionVar2);

      TS_ASSERT_EQUALS(*((gum::LabelizedVariable *)&id.variable(idList[0])),
                       *decisionVar2);
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable *)&id.variable(idList[1])),
                       *decisionVar4);
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable *)&id.variable(idList[2])),
                       *chanceVar1);
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable *)&id.variable(idList[3])),
                       *chanceVar4);
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable *)&id.variable(idList[4])),
                       *utilityVar2);

      gum::LabelizedVariable *utilityVar3 =
          new gum::LabelizedVariable("utilityVar3", "U3", 2);
      TS_ASSERT_THROWS(id.addUtilityNode(*utilityVar3), gum::InvalidArgument);
      delete utilityVar3;
    }

    void testArcInsertion() {
      gum::InfluenceDiagram<float> id;
      gum::List<gum::NodeId> idList;

      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(id.addDecisionNode(*decisionVar2)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(id.addDecisionNode(*decisionVar4)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(id.addChanceNode(*chanceVar1)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(id.addChanceNode(*chanceVar4)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(id.addUtilityNode(*utilityVar2)));

      TS_GUM_ASSERT_THROWS_NOTHING(id.addArc(idList[0], idList[2]));
      TS_GUM_ASSERT_THROWS_NOTHING(id.addArc(idList[2], idList[4]));
      TS_GUM_ASSERT_THROWS_NOTHING(id.addArc(idList[1], idList[3]));
      TS_GUM_ASSERT_THROWS_NOTHING(id.addArc(idList[0], idList[3]));
      TS_GUM_ASSERT_THROWS_NOTHING(id.addArc(idList[3], idList[4]));
      TS_GUM_ASSERT_THROWS_NOTHING(id.addArc(idList[1], idList[4]));

      TS_ASSERT_THROWS(id.addArc(idList[4], idList[0]), gum::InvalidArc);
      TS_ASSERT_THROWS(id.addArc(idList[4], idList[3]), gum::InvalidArc);

      TS_ASSERT_EQUALS(id.dag().sizeArcs(), (gum::Size)6);
    }

    void testEraseVar() {
      gum::InfluenceDiagram<float> id;
      gum::List<gum::NodeId> idList;

      TS_ASSERT(id.empty());
      TS_ASSERT(id.dag().emptyArcs());

      fill(id, idList);

      TS_ASSERT(!id.empty());
      TS_ASSERT(!id.dag().emptyArcs());

      TS_ASSERT_EQUALS(id.size(), (gum::Size)11);
      TS_ASSERT_EQUALS(id.dag().size(), (gum::Size)11);
      TS_ASSERT_EQUALS(id.dag().sizeArcs(), (gum::Size)12);

      id.erase(idList[0]);

      for (const auto node : idList)
        id.erase(node);

      TS_ASSERT(id.empty());

      TS_ASSERT(id.dag().emptyArcs());

      TS_ASSERT_EQUALS(id.size(), (gum::Size)0);
      TS_ASSERT_EQUALS(id.dag().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(id.dag().sizeArcs(), (gum::Size)0);

      TS_ASSERT_EQUALS(id.decisionNodeSize(), (gum::Size)0);
      TS_ASSERT_EQUALS(id.utilityNodeSize(), (gum::Size)0);
      TS_ASSERT_EQUALS(id.chanceNodeSize(), (gum::Size)0);

      idList.clear();
      TS_GUM_ASSERT_THROWS_NOTHING(fill(id, idList));

      TS_ASSERT(!id.empty());
      TS_ASSERT(!id.dag().emptyArcs());

      TS_ASSERT_EQUALS(id.size(), (gum::Size)11);
      TS_ASSERT_EQUALS(id.dag().size(), (gum::Size)11);
      TS_ASSERT_EQUALS(id.dag().sizeArcs(), (gum::Size)12);

      TS_ASSERT_EQUALS(id.decisionNodeSize(), (gum::Size)4);
      TS_ASSERT_EQUALS(id.utilityNodeSize(), (gum::Size)2);
      TS_ASSERT_EQUALS(id.chanceNodeSize(), (gum::Size)5);
    }

    void testEraseArc() {
      gum::InfluenceDiagram<float> id;
      gum::List<gum::NodeId> idList;

      TS_ASSERT(id.empty());
      TS_ASSERT(id.dag().emptyArcs());

      fill(id, idList);

      TS_ASSERT(!id.empty());
      TS_ASSERT(!id.dag().emptyArcs());

      TS_ASSERT_EQUALS(id.size(), (gum::Size)11);
      TS_ASSERT_EQUALS(id.dag().size(), (gum::Size)11);
      TS_ASSERT_EQUALS(id.dag().sizeArcs(), (gum::Size)12);

      TS_GUM_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[0], idList[4])));
      TS_GUM_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[4], idList[9])));
      TS_GUM_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[4], idList[5])));
      TS_GUM_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[5], idList[1])));
      TS_GUM_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[1], idList[7])));
      TS_GUM_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[3], idList[10])));
      TS_GUM_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[5], idList[2])));
      TS_GUM_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[1], idList[9])));
      TS_GUM_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[2], idList[6])));
      TS_GUM_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[6], idList[8])));
      TS_GUM_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[7], idList[8])));
      TS_GUM_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[8], idList[10])));

      TS_ASSERT(!id.empty());
      TS_ASSERT(id.dag().emptyArcs());

      TS_GUM_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[0], idList[4])));
    }

    void testIterator() {
      gum::InfluenceDiagram<float> id;
      gum::List<gum::NodeId> idList;

      for (const auto node : id.nodes()) {
        TS_ASSERT(idList.exists(node));
      }
    }

    void testTopologicalOrder() {
      gum::InfluenceDiagram<float> id;
      gum::List<gum::NodeId> idList;

      fill(id, idList);

      gum::Sequence<gum::NodeId> topoOrder;
      TS_GUM_ASSERT_THROWS_NOTHING(topoOrder = id.topologicalOrder());

      TS_ASSERT_EQUALS(topoOrder.size(), (gum::Size)11);
    }

    void testTable() {
      gum::InfluenceDiagram<float> id;
      gum::List<gum::NodeId> idList;

      fill(id, idList);
      TS_ASSERT_EQUALS(id.cpt(idList[4]).domainSize(), (gum::Size)4);
      TS_ASSERT_EQUALS(id.cpt(idList[5]).domainSize(), (gum::Size)4);
      TS_ASSERT_EQUALS(id.cpt(idList[6]).domainSize(), (gum::Size)4);
      TS_ASSERT_EQUALS(id.cpt(idList[7]).domainSize(), (gum::Size)4);
      TS_ASSERT_EQUALS(id.cpt(idList[8]).domainSize(), (gum::Size)8);
      TS_ASSERT_EQUALS(id.utility(idList[9]).domainSize(), (gum::Size)4);
      TS_ASSERT_EQUALS(id.utility(idList[10]).domainSize(), (gum::Size)4);
    }

    void testTableCoherencyVarRemoval() {
      gum::InfluenceDiagram<float> id;
      gum::List<gum::NodeId> idList;

      fill(id, idList);

      TS_ASSERT_EQUALS(id.dag().parents(idList[8]).size(), (gum::Size)2);
      TS_ASSERT(id.dag().existsArc(idList[6], idList[8]));
      TS_ASSERT(id.dag().existsArc(idList[7], idList[8]));

      gum::Size dmnSize_1 =
          id.variable(idList[7]).domainSize() * id.variable(idList[6]).domainSize();
      dmnSize_1 *= id.variable(idList[8]).domainSize();
      TS_ASSERT_EQUALS(id.cpt(idList[8]).domainSize(), dmnSize_1);

      TS_ASSERT(id.cpt(idList[8]).contains(id.variable(idList[6])));
      TS_ASSERT(id.cpt(idList[8]).contains(id.variable(idList[7])));
      TS_ASSERT(id.cpt(idList[8]).contains(id.variable(idList[8])));

      TS_GUM_ASSERT_THROWS_NOTHING(id.erase(idList[7]));

      TS_ASSERT_EQUALS(id.dag().parents(idList[8]).size(), (gum::Size)1);
      TS_ASSERT(id.dag().existsArc(idList[6], idList[8]));
      TS_ASSERT(!id.dag().existsArc(idList[7], idList[8]));

      gum::Size dmnSize_2 = id.variable(idList[6]).domainSize();
      dmnSize_2 *= id.variable(idList[8]).domainSize();
      TS_ASSERT_EQUALS(id.cpt(idList[8]).domainSize(), dmnSize_2);

      TS_ASSERT(id.cpt(idList[8]).contains(id.variable(idList[6])));
      TS_ASSERT_THROWS_ANYTHING(id.cpt(idList[8]).contains(id.variable(idList[7])));
      TS_ASSERT(id.cpt(idList[8]).contains(id.variable(idList[8])));

      // Testing coherence on utility table
      TS_ASSERT_EQUALS(id.dag().parents(idList[10]).size(), (gum::Size)2);
      TS_ASSERT(id.dag().existsArc(idList[3], idList[10]));
      TS_ASSERT(id.dag().existsArc(idList[8], idList[10]));

      gum::Size dmnSize_3 =
          id.variable(idList[3]).domainSize() * id.variable(idList[8]).domainSize();
      dmnSize_3 *= id.variable(idList[10]).domainSize();
      TS_ASSERT_EQUALS(id.utility(idList[10]).domainSize(), dmnSize_3);

      TS_ASSERT(id.utility(idList[10]).contains(id.variable(idList[3])));
      TS_ASSERT(id.utility(idList[10]).contains(id.variable(idList[8])));
      TS_ASSERT(id.utility(idList[10]).contains(id.variable(idList[10])));

      TS_GUM_ASSERT_THROWS_NOTHING(id.erase(idList[3]));

      TS_ASSERT_EQUALS(id.dag().parents(idList[10]).size(), (gum::Size)1);
      TS_ASSERT(id.dag().existsArc(idList[8], idList[10]));
      TS_ASSERT(!id.dag().existsArc(idList[3], idList[10]));

      gum::Size dmnSize_4 = id.variable(idList[8]).domainSize();
      dmnSize_4 *= id.variable(idList[10]).domainSize();
      TS_ASSERT_EQUALS(id.utility(idList[10]).domainSize(), dmnSize_4);

      TS_ASSERT(id.utility(idList[10]).contains(id.variable(idList[8])));
      TS_ASSERT_THROWS_ANYTHING(
          id.utility(idList[10]).contains(id.variable(idList[3])));
      TS_ASSERT(id.utility(idList[10]).contains(id.variable(idList[10])));
    }

    void testTableCoherencyArcRemoval() {
      gum::InfluenceDiagram<float> id;
      gum::List<gum::NodeId> idList;

      fill(id, idList);

      // Testing Coherence on cpt
      TS_ASSERT_EQUALS(id.dag().parents(idList[8]).size(), (gum::Size)2);
      TS_ASSERT(id.dag().existsArc(idList[6], idList[8]));
      TS_ASSERT(id.dag().existsArc(idList[7], idList[8]));

      gum::Size dmnSize_1 =
          id.variable(idList[7]).domainSize() * id.variable(idList[6]).domainSize();
      dmnSize_1 *= id.variable(idList[8]).domainSize();
      TS_ASSERT_EQUALS(id.cpt(idList[8]).domainSize(), dmnSize_1);

      TS_ASSERT(id.cpt(idList[8]).contains(id.variable(idList[6])));
      TS_ASSERT(id.cpt(idList[8]).contains(id.variable(idList[7])));
      TS_ASSERT(id.cpt(idList[8]).contains(id.variable(idList[8])));

      TS_GUM_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[7], idList[8])));

      TS_ASSERT_EQUALS(id.dag().parents(idList[8]).size(), (gum::Size)1);
      TS_ASSERT(id.dag().existsArc(idList[6], idList[8]));
      TS_ASSERT(!id.dag().existsArc(idList[7], idList[8]));

      gum::Size dmnSize_2 = id.variable(idList[6]).domainSize();
      dmnSize_2 *= id.variable(idList[8]).domainSize();
      TS_ASSERT_EQUALS(id.cpt(idList[8]).domainSize(), dmnSize_2);

      TS_ASSERT(id.cpt(idList[8]).contains(id.variable(idList[6])));
      TS_ASSERT(!id.cpt(idList[8]).contains(id.variable(idList[7])));
      TS_ASSERT(id.cpt(idList[8]).contains(id.variable(idList[8])));

      // Testing coherence on utility table
      TS_ASSERT_EQUALS(id.dag().parents(idList[10]).size(), (gum::Size)2);
      TS_ASSERT(id.dag().existsArc(idList[3], idList[10]));
      TS_ASSERT(id.dag().existsArc(idList[8], idList[10]));

      gum::Size dmnSize_3 =
          id.variable(idList[3]).domainSize() * id.variable(idList[8]).domainSize();
      dmnSize_3 *= id.variable(idList[10]).domainSize();
      TS_ASSERT_EQUALS(id.utility(idList[10]).domainSize(), dmnSize_3);

      TS_ASSERT(id.utility(idList[10]).contains(id.variable(idList[3])));
      TS_ASSERT(id.utility(idList[10]).contains(id.variable(idList[8])));
      TS_ASSERT(id.utility(idList[10]).contains(id.variable(idList[10])));

      TS_GUM_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[3], idList[10])));

      TS_ASSERT_EQUALS(id.dag().parents(idList[10]).size(), (gum::Size)1);
      TS_ASSERT(id.dag().existsArc(idList[8], idList[10]));
      TS_ASSERT(!id.dag().existsArc(idList[3], idList[10]));

      gum::Size dmnSize_4 = id.variable(idList[8]).domainSize();
      dmnSize_4 *= id.variable(idList[10]).domainSize();
      TS_ASSERT_EQUALS(id.utility(idList[10]).domainSize(), dmnSize_4);

      TS_ASSERT(id.utility(idList[10]).contains(id.variable(idList[8])));
      TS_ASSERT(!id.utility(idList[10]).contains(id.variable(idList[3])));
      TS_ASSERT(id.utility(idList[10]).contains(id.variable(idList[10])));
    }

    void testAccessorByName() {
      gum::InfluenceDiagram<float> id;
      gum::List<gum::NodeId> idList;

      fill(id, idList);

      for (const auto node : id.nodes()) {
        TS_ASSERT_EQUALS(id.idFromName(id.variable(node).name()), node);
        TS_ASSERT_EQUALS(&id.variableFromName(id.variable(node).name()),
                         &id.variable(node));
      }

      TS_ASSERT_THROWS(id.idFromName("choucroute"), gum::NotFound);

      TS_ASSERT_THROWS(id.variableFromName("choucroute"), gum::NotFound);

      TS_GUM_ASSERT_THROWS_NOTHING(id.idFromName("decisionVar1"));
      id.erase(id.idFromName("decisionVar1"));
      TS_ASSERT_THROWS(id.idFromName("decisionVar1"), gum::NotFound);
    }

    void testDecisionPath() {
      gum::InfluenceDiagram<float> id;
      gum::List<gum::NodeId> idList;

      fill(id, idList);

      TS_ASSERT(!id.decisionOrderExists());
      TS_ASSERT_THROWS(id.getDecisionOrder(), gum::NotFound);
      id.addArc(idList[2], idList[1]);
      id.addArc(idList[7], idList[3]);
      TS_ASSERT(id.decisionOrderExists());
      TS_ASSERT_THROWS_NOTHING(id.getDecisionOrder());
      TS_ASSERT_THROWS_NOTHING(id.getPartialTemporalOrder());
      gum::List<gum::NodeSet> partialTemporalOrder = id.getPartialTemporalOrder();

      gum::Size resultat[7][3] = {{8, 6, 4}, {3}, {7}, {1}, {2}, {5}, {0}};
      int i = 0;

      for (const auto nodeset : partialTemporalOrder) {
        int j = 0;

        for (const auto node : nodeset) {
          TS_ASSERT_EQUALS(node, resultat[i][j]);
          j += 1;
        }

        i += 1;
      }
    }

    void testMoralisation() {
      gum::InfluenceDiagram<float> id;
      gum::List<gum::NodeId> idList;

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

      idList.insert(id.addDecisionNode(dVar1)); // 0
      idList.insert(id.addDecisionNode(dVar2)); // 1
      idList.insert(id.addDecisionNode(dVar3)); // 2
      idList.insert(id.addDecisionNode(dVar4)); // 3
      idList.insert(id.addChanceNode(cVar1));   // 4
      idList.insert(id.addChanceNode(cVar2));   // 5
      idList.insert(id.addChanceNode(cVar3));   // 6
      idList.insert(id.addChanceNode(cVar4));   // 7
      idList.insert(id.addChanceNode(cVar5));   // 8
      idList.insert(id.addChanceNode(cVar6));   // 9
      idList.insert(id.addChanceNode(cVar7));   // 10
      idList.insert(id.addChanceNode(cVar8));   // 11
      idList.insert(id.addChanceNode(cVar9));   // 12
      idList.insert(id.addChanceNode(cVar10));  // 13
      idList.insert(id.addChanceNode(cVar11));  // 14
      idList.insert(id.addChanceNode(cVar12));  // 15
      idList.insert(id.addUtilityNode(uVar1));  // 16
      idList.insert(id.addUtilityNode(uVar2));  // 17
      idList.insert(id.addUtilityNode(uVar3));  // 18
      idList.insert(id.addUtilityNode(uVar4));  // 19

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

      gum::UndiGraph moralGraph;
      TS_ASSERT_THROWS_NOTHING(moralGraph = id.moralGraph(true));

      // For comparison with what readers will return
      std::string dotfile = GET_PATH_STR("IDBeforeMoralisation.dot");
      std::ofstream idOutput(dotfile.c_str(), std::ios::out | std::ios::trunc);

      if (!idOutput.good()) {
        GUM_ERROR(gum::IOError, "Stream states flags are not all unset.");
      }

      idOutput << id.toDot();

      idOutput.flush();

      idOutput.close();

      if (idOutput.fail()) {
        GUM_ERROR(gum::IOError, "Writting in the ostream failed.");
      }

      // For comparison with what readers will return
      dotfile = GET_PATH_STR("IDAfterMoralisation.dot");

      std::ofstream moralOutput(dotfile.c_str(), std::ios::out | std::ios::trunc);

      if (!moralOutput.good()) {
        GUM_ERROR(gum::IOError, "Stream states flags are not all unset.");
      }

      moralOutput << moralGraph.toDot();

      moralOutput.flush();

      moralOutput.close();

      if (moralOutput.fail()) {
        GUM_ERROR(gum::IOError, "Writting in the ostream failed.");
      }
    }
  };
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
