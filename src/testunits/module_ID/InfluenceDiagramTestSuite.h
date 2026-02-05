/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/variables/discreteVariable.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/ID/generator/influenceDiagramGenerator.h>
#include <agrum/ID/influenceDiagram.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  InfluenceDiagram
#define GUM_CURRENT_MODULE ID

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

  struct InfluenceDiagramTestSuite {
    private:
    void fillTopo(gum::InfluenceDiagram< double >& id, gum::List< gum::NodeId >& idList) const {
      try {
        idList.insert(id.addDecisionNode(*decisionVar1));   // 0
        idList.insert(id.addDecisionNode(*decisionVar2));   // 1
        idList.insert(id.addDecisionNode(*decisionVar3));   // 2
        idList.insert(id.addDecisionNode(*decisionVar4));   // 3
        idList.insert(id.addChanceNode(*chanceVar1));       // 4
        idList.insert(id.addChanceNode(*chanceVar2));       // 5
        idList.insert(id.addChanceNode(*chanceVar3));       // 6
        idList.insert(id.addChanceNode(*chanceVar4));       // 7
        idList.insert(id.addChanceNode(*chanceVar5));       // 8
        idList.insert(id.addUtilityNode(*utilityVar1));     // 9
        idList.insert(id.addUtilityNode(*utilityVar2));     // 10

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

      } catch (const gum::Exception& e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    void fill(gum::InfluenceDiagram< double >& id, gum::List< gum::NodeId >& idList) {
      fillTopo(id, idList);

      try {
        id.cpt(idList[4]).populate({0.2f, 0.8f, 0.5f, 0.5f});                           // C1
        id.cpt(idList[5]).populate({0.1f, 0.9f, 0.9f, 0.1f});                           // C2
        id.cpt(idList[6]).populate({0.35f, 0.65f, 0.19f, 0.81f});                       // C3
        id.cpt(idList[7]).populate({0.4f, 0.6f, 0.5f, 0.5f});                           // C4
        id.cpt(idList[8]).populate({0.4f, 0.6f, 0.8f, 0.2f, 0.4f, 0.6f, 0.3f, 0.7f});   // C5

        id.utility(idList[9]).populate({42.0f, 69.0f, 666.0f, 84.0f});                  // U1
        id.utility(idList[10]).populate({42.0f, -69.0f, 666.0f, 84.0f});                // U2
      } catch (const gum::Exception& e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    public:
    gum::LabelizedVariable* decisionVar1;
    gum::LabelizedVariable* decisionVar2;
    gum::LabelizedVariable* decisionVar3;
    gum::LabelizedVariable* decisionVar4;
    gum::LabelizedVariable* chanceVar1;
    gum::LabelizedVariable* chanceVar2;
    gum::LabelizedVariable* chanceVar3;
    gum::LabelizedVariable* chanceVar4;
    gum::LabelizedVariable* chanceVar5;
    gum::LabelizedVariable* utilityVar1;
    gum::LabelizedVariable* utilityVar2;

    InfluenceDiagramTestSuite() {
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

    ~InfluenceDiagramTestSuite() {
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
      gum::InfluenceDiagram< double >* topology = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(topology = new gum::InfluenceDiagram< double >());

      CHECK_THROWS_AS(topology->addArc(1, 2), const gum::InvalidNode&);
      gum::List< gum::NodeId > idList;
      GUM_CHECK_ASSERT_THROWS_NOTHING(fill(*topology, idList));

      CHECK_THROWS_AS(topology->add(*decisionVar1), const gum::DuplicateLabel&);

      GUM_CHECK_ASSERT_THROWS_NOTHING(delete topology);
    }

    void testNodeType() {
      gum::InfluenceDiagram< double > topology;
      gum::List< gum::NodeId >        idList;
      GUM_CHECK_ASSERT_THROWS_NOTHING(fill(topology, idList));

      CHECK(topology.isDecisionNode(idList[0]));
      CHECK(topology.isDecisionNode(idList[1]));
      CHECK(topology.isDecisionNode(idList[2]));
      CHECK(topology.isDecisionNode(idList[3]));
      CHECK(!topology.isDecisionNode(idList[4]));
      CHECK(!topology.isDecisionNode(idList[5]));
      CHECK(!topology.isDecisionNode(idList[6]));
      CHECK(!topology.isDecisionNode(idList[7]));
      CHECK(!topology.isDecisionNode(idList[8]));
      CHECK(!topology.isDecisionNode(idList[9]));
      CHECK(!topology.isDecisionNode(idList[10]));

      CHECK(!topology.isChanceNode(idList[0]));
      CHECK(!topology.isChanceNode(idList[1]));
      CHECK(!topology.isChanceNode(idList[2]));
      CHECK(!topology.isChanceNode(idList[3]));
      CHECK(topology.isChanceNode(idList[4]));
      CHECK(topology.isChanceNode(idList[5]));
      CHECK(topology.isChanceNode(idList[6]));
      CHECK(topology.isChanceNode(idList[7]));
      CHECK(topology.isChanceNode(idList[8]));
      CHECK(!topology.isChanceNode(idList[9]));
      CHECK(!topology.isChanceNode(idList[10]));

      CHECK(!topology.isUtilityNode(idList[0]));
      CHECK(!topology.isUtilityNode(idList[1]));
      CHECK(!topology.isUtilityNode(idList[2]));
      CHECK(!topology.isUtilityNode(idList[3]));
      CHECK(!topology.isUtilityNode(idList[4]));
      CHECK(!topology.isUtilityNode(idList[5]));
      CHECK(!topology.isUtilityNode(idList[6]));
      CHECK(!topology.isUtilityNode(idList[7]));
      CHECK(!topology.isUtilityNode(idList[8]));
      CHECK(topology.isUtilityNode(idList[9]));
      CHECK(topology.isUtilityNode(idList[10]));

      CHECK((topology.chanceNodeSize()) == (static_cast< gum::Size >(5)));
      CHECK((topology.decisionNodeSize()) == (static_cast< gum::Size >(4)));
      CHECK((topology.utilityNodeSize()) == (static_cast< gum::Size >(2)));
    }

    void testToDot() {
      gum::InfluenceDiagram< double > id;
      gum::List< gum::NodeId >        idList;
      fill(id, idList);

      // For comparison with what readers will return
      std::string   dotfile = GET_RESSOURCES_PATH("outputs/IDToDotWriter.dot");
      std::ofstream output(dotfile.c_str(), std::ios::out | std::ios::trunc);

      if (!output.good()) { GUM_ERROR(gum::IOError, "Stream states flags are not all unset.") }

      output << id.toDot();

      output.flush();

      output.close();

      if (output.fail()) { GUM_ERROR(gum::IOError, "Writing in the ostream failed.") }
    }

    void testCopyConstructor() {
      gum::InfluenceDiagram< double > source;
      gum::List< gum::NodeId >        idList;
      GUM_CHECK_ASSERT_THROWS_NOTHING(fill(source, idList));

      gum::InfluenceDiagram< double >* copy = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(copy = new gum::InfluenceDiagram< double >(source));

      CHECK((source.dag().size()) == (copy->dag().size()));
      CHECK((source.dag().sizeArcs()) == (copy->dag().sizeArcs()));

      for (const auto node: source.dag().nodes()) {
        CHECK(copy->dag().exists(node));

        const gum::DiscreteVariable& srcVar = source.variable(node);
        const gum::DiscreteVariable& cpVar  = copy->variable(node);
        CHECK((srcVar.name()) == (cpVar.name()));

        if (srcVar.domainSize() == cpVar.domainSize()) {
          for (gum::Idx i = 0; i < srcVar.domainSize(); i++)
            CHECK((srcVar.label(i)) == (cpVar.label(i)));
        } else CHECK(false);

        for (const auto parent: source.dag().parents(node)) {
          CHECK(copy->dag().existsArc(parent, node));
        }

        if (source.isChanceNode(node)) {
          const gum::Tensor< double >& srcCPT = source.cpt(node);

          const gum::Tensor< double >& cpCPT = copy->cpt(node);

          gum::Instantiation srcInst(srcCPT);

          gum::Instantiation cpInst(cpCPT);

          for (cpInst.setFirst(); !cpInst.end(); cpInst.inc()) {
            for (gum::Idx i = 0; i < cpInst.nbrDim(); i++) {
              gum::NodeId id = copy->nodeId(cpInst.variable(i));
              srcInst.chgVal(source.variable(id), cpInst.val(i));
            }

            CHECK((cpCPT[cpInst]) == (srcCPT[srcInst]));
          }

        } else if (source.isUtilityNode(node)) {
          const gum::Tensor< double >& srcUT = source.utility(node);
          const gum::Tensor< double >& cpUT  = copy->utility(node);

          gum::Instantiation srcInst(srcUT);
          gum::Instantiation cpInst(cpUT);

          for (cpInst.setFirst(); !cpInst.end(); cpInst.inc()) {
            for (gum::Idx i = 0; i < cpInst.nbrDim(); i++) {
              gum::NodeId id = copy->nodeId(cpInst.variable(i));
              srcInst.chgVal(source.variable(id), cpInst.val(i));
            }

            CHECK((cpUT[cpInst]) == (srcUT[srcInst]));
          }
        }
      }

      if (copy != nullptr) delete copy;
    }

    void testInsertion_1() {
      gum::InfluenceDiagram< double > id;
      gum::List< gum::NodeId >        idList;

      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(id.addDecisionNode(*decisionVar1)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(id.addDecisionNode(*decisionVar3)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(id.addChanceNode(*chanceVar3)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(id.addChanceNode(*chanceVar5)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(id.addUtilityNode(*utilityVar1)));

      CHECK((id.size()) == (static_cast< gum::Size >(5)));
      CHECK((id.dag().size()) == (static_cast< gum::Size >(5)));

      // Test for uniqueness of the ids

      for (gum::Idx i = 0; i < idList.size() - 1; i++) {
        for (gum::Idx j = i + 1; j < idList.size(); j++) {
          if (idList[i] != idList[j]) {
            CHECK(true);
          } else {
            CHECK(false);
          }
        }
      }

      gum::LabelizedVariable const* varPtr = nullptr;

      GUM_CHECK_ASSERT_THROWS_NOTHING(varPtr = (gum::LabelizedVariable*)&id.variable(idList[0]));
      CHECK((*varPtr) == (*decisionVar1));

      CHECK((*((gum::LabelizedVariable*)&id.variable(idList[0]))) == (*decisionVar1));
      CHECK((*((gum::LabelizedVariable*)&id.variable(idList[1]))) == (*decisionVar3));
      CHECK((*((gum::LabelizedVariable*)&id.variable(idList[2]))) == (*chanceVar3));
      CHECK((*((gum::LabelizedVariable*)&id.variable(idList[3]))) == (*chanceVar5));
      CHECK((*((gum::LabelizedVariable*)&id.variable(idList[4]))) == (*utilityVar1));
    }

    void testInsertion_2() {
      gum::InfluenceDiagram< double > id;
      gum::List< gum::NodeId >        idList;

      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(id.addDecisionNode(*decisionVar2)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(id.addDecisionNode(*decisionVar4)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(id.addChanceNode(*chanceVar1)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(id.addChanceNode(*chanceVar4)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(id.addUtilityNode(*utilityVar2)));

      CHECK((id.size()) == (static_cast< gum::Size >(5)));
      CHECK((id.dag().size()) == (static_cast< gum::Size >(5)));

      gum::LabelizedVariable const* varPtr = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(varPtr = (gum::LabelizedVariable*)&id.variable(idList[0]));
      CHECK((*varPtr) == (*decisionVar2));

      CHECK((*((gum::LabelizedVariable*)&id.variable(idList[0]))) == (*decisionVar2));
      CHECK((*((gum::LabelizedVariable*)&id.variable(idList[1]))) == (*decisionVar4));
      CHECK((*((gum::LabelizedVariable*)&id.variable(idList[2]))) == (*chanceVar1));
      CHECK((*((gum::LabelizedVariable*)&id.variable(idList[3]))) == (*chanceVar4));
      CHECK((*((gum::LabelizedVariable*)&id.variable(idList[4]))) == (*utilityVar2));

      auto utilityVar3 = new gum::LabelizedVariable("utilityVar3", "U3", 2);
      CHECK_THROWS_AS(id.addUtilityNode(*utilityVar3), const gum::InvalidArgument&);
      delete utilityVar3;
    }

    void testArcInsertion() {
      gum::InfluenceDiagram< double > id;
      gum::List< gum::NodeId >        idList;

      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(id.addDecisionNode(*decisionVar2)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(id.addDecisionNode(*decisionVar4)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(id.addChanceNode(*chanceVar1)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(id.addChanceNode(*chanceVar4)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(id.addUtilityNode(*utilityVar2)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(id.addArc(idList[0], idList[2]));
      GUM_CHECK_ASSERT_THROWS_NOTHING(id.addArc(idList[2], idList[4]));
      GUM_CHECK_ASSERT_THROWS_NOTHING(id.addArc(idList[1], idList[3]));
      GUM_CHECK_ASSERT_THROWS_NOTHING(id.addArc(idList[0], idList[3]));
      GUM_CHECK_ASSERT_THROWS_NOTHING(id.addArc(idList[3], idList[4]));
      GUM_CHECK_ASSERT_THROWS_NOTHING(id.addArc(idList[1], idList[4]));

      CHECK_THROWS_AS(id.addArc(idList[4], idList[0]), const gum::InvalidArc&);
      CHECK_THROWS_AS(id.addArc(idList[4], idList[3]), const gum::InvalidArc&);

      CHECK((id.dag().sizeArcs()) == (static_cast< gum::Size >(6)));
    }

    void testEraseVar() {
      gum::InfluenceDiagram< double > id;
      gum::List< gum::NodeId >        idList;

      CHECK(id.empty());
      CHECK(id.dag().emptyArcs());

      fill(id, idList);

      CHECK(!id.empty());
      CHECK(!id.dag().emptyArcs());

      CHECK((id.size()) == (static_cast< gum::Size >(11)));
      CHECK((id.dag().size()) == (static_cast< gum::Size >(11)));
      CHECK((id.dag().sizeArcs()) == (static_cast< gum::Size >(12)));

      id.erase(idList[0]);

      for (const auto node: idList)
        id.erase(node);

      CHECK(id.empty());

      CHECK(id.dag().emptyArcs());

      CHECK((id.size()) == (static_cast< gum::Size >(0)));
      CHECK((id.dag().size()) == (static_cast< gum::Size >(0)));
      CHECK((id.dag().sizeArcs()) == (static_cast< gum::Size >(0)));

      CHECK((id.decisionNodeSize()) == (static_cast< gum::Size >(0)));
      CHECK((id.utilityNodeSize()) == (static_cast< gum::Size >(0)));
      CHECK((id.chanceNodeSize()) == (static_cast< gum::Size >(0)));

      idList.clear();
      GUM_CHECK_ASSERT_THROWS_NOTHING(fill(id, idList));

      CHECK(!id.empty());
      CHECK(!id.dag().emptyArcs());

      CHECK((id.size()) == (static_cast< gum::Size >(11)));
      CHECK((id.dag().size()) == (static_cast< gum::Size >(11)));
      CHECK((id.dag().sizeArcs()) == (static_cast< gum::Size >(12)));

      CHECK((id.decisionNodeSize()) == (static_cast< gum::Size >(4)));
      CHECK((id.utilityNodeSize()) == (static_cast< gum::Size >(2)));
      CHECK((id.chanceNodeSize()) == (static_cast< gum::Size >(5)));
    }

    void testEraseArc() {
      gum::InfluenceDiagram< double > id;
      gum::List< gum::NodeId >        idList;

      CHECK(id.empty());
      CHECK(id.dag().emptyArcs());

      fill(id, idList);

      CHECK(!id.empty());
      CHECK(!id.dag().emptyArcs());

      CHECK((id.size()) == (static_cast< gum::Size >(11)));
      CHECK((id.dag().size()) == (static_cast< gum::Size >(11)));
      CHECK((id.dag().sizeArcs()) == (static_cast< gum::Size >(12)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[0], idList[4])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[4], idList[9])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[4], idList[5])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[5], idList[1])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[1], idList[7])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[3], idList[10])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[5], idList[2])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[1], idList[9])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[2], idList[6])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[6], idList[8])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[7], idList[8])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[8], idList[10])));

      CHECK(!id.empty());
      CHECK(id.dag().emptyArcs());

      GUM_CHECK_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[0], idList[4])));
    }

    void testIterator() {
      gum::InfluenceDiagram< double > id;
      gum::List< gum::NodeId >        idList;

      for (const auto node: id.nodes()) {
        CHECK(idList.exists(node));
      }
    }

    void testTopologicalOrder() {
      gum::InfluenceDiagram< double > id;
      gum::List< gum::NodeId >        idList;

      fill(id, idList);

      gum::Sequence< gum::NodeId > topoOrder;
      GUM_CHECK_ASSERT_THROWS_NOTHING(topoOrder = id.topologicalOrder());

      CHECK((topoOrder.size()) == (static_cast< gum::Size >(11)));
    }

    void testTable() {
      gum::InfluenceDiagram< double > id;
      gum::List< gum::NodeId >        idList;

      fill(id, idList);
      CHECK((id.cpt(idList[4]).domainSize()) == (static_cast< gum::Size >(4)));
      CHECK((id.cpt(idList[5]).domainSize()) == (static_cast< gum::Size >(4)));
      CHECK((id.cpt(idList[6]).domainSize()) == (static_cast< gum::Size >(4)));
      CHECK((id.cpt(idList[7]).domainSize()) == (static_cast< gum::Size >(4)));
      CHECK((id.cpt(idList[8]).domainSize()) == (static_cast< gum::Size >(8)));
      CHECK((id.utility(idList[9]).domainSize()) == (static_cast< gum::Size >(4)));
      CHECK((id.utility(idList[10]).domainSize()) == (static_cast< gum::Size >(4)));
    }

    void testTableCoherencyVarRemoval() {
      gum::InfluenceDiagram< double > id;
      gum::List< gum::NodeId >        idList;

      fill(id, idList);

      CHECK((id.dag().parents(idList[8]).size()) == (static_cast< gum::Size >(2)));
      CHECK(id.dag().existsArc(idList[6], idList[8]));
      CHECK(id.dag().existsArc(idList[7], idList[8]));

      gum::Size dmnSize_1
          = id.variable(idList[7]).domainSize() * id.variable(idList[6]).domainSize();
      dmnSize_1 *= id.variable(idList[8]).domainSize();
      CHECK((id.cpt(idList[8]).domainSize()) == (dmnSize_1));

      CHECK(id.cpt(idList[8]).contains(id.variable(idList[6])));
      CHECK(id.cpt(idList[8]).contains(id.variable(idList[7])));
      CHECK(id.cpt(idList[8]).contains(id.variable(idList[8])));

      GUM_CHECK_ASSERT_THROWS_NOTHING(id.erase(idList[7]));

      CHECK((id.dag().parents(idList[8]).size()) == (static_cast< gum::Size >(1)));
      CHECK(id.dag().existsArc(idList[6], idList[8]));
      CHECK(!id.dag().existsArc(idList[7], idList[8]));

      gum::Size dmnSize_2 = id.variable(idList[6]).domainSize();
      dmnSize_2 *= id.variable(idList[8]).domainSize();
      CHECK((id.cpt(idList[8]).domainSize()) == (dmnSize_2));

      CHECK(id.cpt(idList[8]).contains(id.variable(idList[6])));
      CHECK_THROWS(id.cpt(idList[8]).contains(id.variable(idList[7])));
      CHECK(id.cpt(idList[8]).contains(id.variable(idList[8])));

      // Testing coherence on utility table
      CHECK((id.dag().parents(idList[10]).size()) == (static_cast< gum::Size >(2)));
      CHECK(id.dag().existsArc(idList[3], idList[10]));
      CHECK(id.dag().existsArc(idList[8], idList[10]));

      gum::Size dmnSize_3
          = id.variable(idList[3]).domainSize() * id.variable(idList[8]).domainSize();
      dmnSize_3 *= id.variable(idList[10]).domainSize();
      CHECK((id.utility(idList[10]).domainSize()) == (dmnSize_3));

      CHECK(id.utility(idList[10]).contains(id.variable(idList[3])));
      CHECK(id.utility(idList[10]).contains(id.variable(idList[8])));
      CHECK(id.utility(idList[10]).contains(id.variable(idList[10])));

      GUM_CHECK_ASSERT_THROWS_NOTHING(id.erase(idList[3]));

      CHECK((id.dag().parents(idList[10]).size()) == (static_cast< gum::Size >(1)));
      CHECK(id.dag().existsArc(idList[8], idList[10]));
      CHECK(!id.dag().existsArc(idList[3], idList[10]));

      gum::Size dmnSize_4 = id.variable(idList[8]).domainSize();
      dmnSize_4 *= id.variable(idList[10]).domainSize();
      CHECK((id.utility(idList[10]).domainSize()) == (dmnSize_4));

      CHECK(id.utility(idList[10]).contains(id.variable(idList[8])));
      CHECK_THROWS(id.utility(idList[10]).contains(id.variable(idList[3])));
      CHECK(id.utility(idList[10]).contains(id.variable(idList[10])));
    }

    void testTableCoherencyArcRemoval() {
      gum::InfluenceDiagram< double > id;
      gum::List< gum::NodeId >        idList;

      fill(id, idList);

      // Testing Coherence on cpt
      CHECK((id.dag().parents(idList[8]).size()) == (static_cast< gum::Size >(2)));
      CHECK(id.dag().existsArc(idList[6], idList[8]));
      CHECK(id.dag().existsArc(idList[7], idList[8]));

      gum::Size dmnSize_1
          = id.variable(idList[7]).domainSize() * id.variable(idList[6]).domainSize();
      dmnSize_1 *= id.variable(idList[8]).domainSize();
      CHECK((id.cpt(idList[8]).domainSize()) == (dmnSize_1));

      CHECK(id.cpt(idList[8]).contains(id.variable(idList[6])));
      CHECK(id.cpt(idList[8]).contains(id.variable(idList[7])));
      CHECK(id.cpt(idList[8]).contains(id.variable(idList[8])));

      GUM_CHECK_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[7], idList[8])));

      CHECK((id.dag().parents(idList[8]).size()) == (static_cast< gum::Size >(1)));
      CHECK(id.dag().existsArc(idList[6], idList[8]));
      CHECK(!id.dag().existsArc(idList[7], idList[8]));

      gum::Size dmnSize_2 = id.variable(idList[6]).domainSize();
      dmnSize_2 *= id.variable(idList[8]).domainSize();
      CHECK((id.cpt(idList[8]).domainSize()) == (dmnSize_2));

      CHECK(id.cpt(idList[8]).contains(id.variable(idList[6])));
      CHECK(!id.cpt(idList[8]).contains(id.variable(idList[7])));
      CHECK(id.cpt(idList[8]).contains(id.variable(idList[8])));

      // Testing coherence on utility table
      CHECK((id.dag().parents(idList[10]).size()) == (static_cast< gum::Size >(2)));
      CHECK(id.dag().existsArc(idList[3], idList[10]));
      CHECK(id.dag().existsArc(idList[8], idList[10]));

      gum::Size dmnSize_3
          = id.variable(idList[3]).domainSize() * id.variable(idList[8]).domainSize();
      dmnSize_3 *= id.variable(idList[10]).domainSize();
      CHECK((id.utility(idList[10]).domainSize()) == (dmnSize_3));

      CHECK(id.utility(idList[10]).contains(id.variable(idList[3])));
      CHECK(id.utility(idList[10]).contains(id.variable(idList[8])));
      CHECK(id.utility(idList[10]).contains(id.variable(idList[10])));

      GUM_CHECK_ASSERT_THROWS_NOTHING(id.eraseArc(gum::Arc(idList[3], idList[10])));

      CHECK((id.dag().parents(idList[10]).size()) == (static_cast< gum::Size >(1)));
      CHECK(id.dag().existsArc(idList[8], idList[10]));
      CHECK(!id.dag().existsArc(idList[3], idList[10]));

      gum::Size dmnSize_4 = id.variable(idList[8]).domainSize();
      dmnSize_4 *= id.variable(idList[10]).domainSize();
      CHECK((id.utility(idList[10]).domainSize()) == (dmnSize_4));

      CHECK(id.utility(idList[10]).contains(id.variable(idList[8])));
      CHECK(!id.utility(idList[10]).contains(id.variable(idList[3])));
      CHECK(id.utility(idList[10]).contains(id.variable(idList[10])));
    }

    void testAccessorByName() {
      gum::InfluenceDiagram< double > id;
      gum::List< gum::NodeId >        idList;

      fill(id, idList);

      for (const auto node: id.nodes()) {
        CHECK((id.idFromName(id.variable(node).name())) == (node));
        CHECK((&id.variableFromName(id.variable(node).name())) == (&id.variable(node)));
      }

      CHECK_THROWS_AS(id.idFromName("chorizo"), const gum::NotFound&);

      CHECK_THROWS_AS(id.variableFromName("chorizo"), const gum::NotFound&);

      GUM_CHECK_ASSERT_THROWS_NOTHING(id.idFromName("decisionVar1"));
      id.erase(id.idFromName("decisionVar1"));
      CHECK_THROWS_AS(id.idFromName("decisionVar1"), const gum::NotFound&);
    }

    void testDecisionPath() {
      gum::InfluenceDiagram< double > id;
      gum::List< gum::NodeId >        idList;

      fill(id, idList);

      CHECK(!id.decisionOrderExists());
      CHECK_THROWS_AS(id.decisionOrder(), const gum::NotFound&);
      id.addArc(idList[2], idList[1]);
      id.addArc(idList[7], idList[3]);
      CHECK(id.decisionOrderExists());
      CHECK_NOTHROW(id.decisionOrder());
      CHECK_NOTHROW(id.getPartialTemporalOrder());
      gum::List< gum::NodeSet > partialTemporalOrder = id.getPartialTemporalOrder();

      gum::Size resultat[7][3] = {{8, 6, 4}, {3}, {7}, {1}, {2}, {5}, {0}};
      int       i              = 0;

      for (const auto& nodeset: partialTemporalOrder) {
        int j = 0;

        for (const auto node: nodeset) {
          CHECK((node) == (resultat[i][j]));
          j += 1;
        }

        i += 1;
      }
    }

    void testMoralisation() {
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

      gum::UndiGraph moralGraph;
      CHECK_NOTHROW(moralGraph = id.moralGraph());

      // For comparison with what readers will return
      std::string   dotfile = GET_RESSOURCES_PATH("outputs/IDBeforeMoralisation.dot");
      std::ofstream idOutput(dotfile.c_str(), std::ios::out | std::ios::trunc);

      if (!idOutput.good()) { GUM_ERROR(gum::IOError, "Stream states flags are not all unset.") }

      idOutput << id.toDot();

      idOutput.flush();

      idOutput.close();

      if (idOutput.fail()) { GUM_ERROR(gum::IOError, "Writing in the ostream failed.") }

      // For comparison with what readers will return
      dotfile = GET_RESSOURCES_PATH("outputs/IDAfterMoralisation.dot");

      std::ofstream moralOutput(dotfile.c_str(), std::ios::out | std::ios::trunc);

      if (!moralOutput.good()) { GUM_ERROR(gum::IOError, "Stream states flags are not all unset.") }

      moralOutput << moralGraph.toDot();

      moralOutput.flush();

      moralOutput.close();

      if (moralOutput.fail()) { GUM_ERROR(gum::IOError, "Writing in the ostream failed.") }
    }

    void testWithNames() {
      gum::InfluenceDiagram< double > diag;
      diag.add(gum::LabelizedVariable("A", "A", 2));
      diag.addDecisionNode(gum::LabelizedVariable("D", "D", 2));
      diag.addUtilityNode(gum::LabelizedVariable("U", "U", 1));

      diag.changeVariableName(0, "O");
      CHECK((diag.variable(0).name()) == ("O"));
      diag.changeVariableName("O", "I");
      CHECK((diag.variable(0).name()) == ("I"));

      diag.addArc(0, 1);
      CHECK(diag.existsPathBetween(0, 1));
      CHECK(diag.existsPathBetween("I", "D"));
      diag.eraseArc(0, 1);
      CHECK(!diag.existsPathBetween(0, 1));
      CHECK(!diag.existsPathBetween("I", "D"));

      diag.addArc("I", "D");
      CHECK(diag.existsPathBetween(0, 1));
      CHECK(diag.existsPathBetween("I", "D"));
      diag.eraseArc("I", "D");
      CHECK(!diag.existsPathBetween(0, 1));
      CHECK(!diag.existsPathBetween("I", "D"));

      CHECK_THROWS_AS(diag.addArc("foo", "bar"), const gum::NotFound&);
    }

    void testFastPrototype() {
      auto infdiag = gum::InfluenceDiagram< double >::fastPrototype("A->*B<-C;E<-B->$D");
      CHECK((infdiag.size()) == (static_cast< gum::Size >(5)));
      CHECK((infdiag.chanceNodeSize()) == (static_cast< gum::Size >(3)));
      CHECK((infdiag.utilityNodeSize()) == (static_cast< gum::Size >(1)));
      CHECK((infdiag.decisionNodeSize()) == (static_cast< gum::Size >(1)));
      CHECK((infdiag.sizeArcs()) == (static_cast< gum::Size >(4)));
    }

    void testFastPrototypeVarType() {
      auto infdiag = gum::InfluenceDiagram< float >::fastPrototype(
          "a{1|4|6}->b{1|-4|6}->c{1|toto|6}->d{1.0|-4.0|6.0}->e{1|-4|6.0}->f{1.0|-4.0|+6.0}");
      CHECK((infdiag.variable("a").varType()) == (gum::VarType::INTEGER));
      CHECK((infdiag.variable("b").varType()) == (gum::VarType::INTEGER));
      CHECK((infdiag.variable("c").varType()) == (gum::VarType::LABELIZED));
      CHECK((infdiag.variable("d").varType()) == (gum::VarType::NUMERICAL));
      CHECK((infdiag.variable("e").varType()) == (gum::VarType::NUMERICAL));
      CHECK((infdiag.variable("f").varType()) == (gum::VarType::NUMERICAL));
    }

    void testFastVariable() {
      {
        gum::InfluenceDiagram< float > infdiag;
        infdiag.addChanceNode("a{1|4|6}");
        infdiag.addChanceNode("b", 4);
        infdiag.addDecisionNode("c[1,2,3,4,5.5]");
        infdiag.addUtilityNode("d");
        infdiag.addUtilityNode("e{degre}");

        CHECK((infdiag.variable("a").toString()) == ("a:Integer({1|4|6})"));
        CHECK((infdiag.variable("b").toString()) == ("b:Range([0,3])"));
        CHECK((infdiag.variable("c").toString()) == ("c:Discretized(<[1;2[,[2;3[,[3;4[,[4;5.5]>)"));
        CHECK((infdiag.variable("d").toString()) == ("d:Range([0,0])"));
        CHECK((infdiag.variable("e").toString()) == ("e:Labelized({degre})"));

        CHECK(infdiag.isChanceNode("a"));
        CHECK(infdiag.isChanceNode("b"));
        CHECK(infdiag.isDecisionNode("c"));
        CHECK(infdiag.isUtilityNode("d"));
        CHECK(infdiag.isUtilityNode("e"));
      }

      {
        gum::InfluenceDiagram< float > infdiag;
        infdiag.add("a{1|4|6}");
        infdiag.add("b", 4);
        infdiag.add("*c[1,2,3,4,5.5]");
        infdiag.add("$d");
        infdiag.add("$e{degre}");

        CHECK((infdiag.variable("a").toString()) == ("a:Integer({1|4|6})"));
        CHECK((infdiag.variable("b").toString()) == ("b:Range([0,3])"));
        CHECK((infdiag.variable("c").toString()) == ("c:Discretized(<[1;2[,[2;3[,[3;4[,[4;5.5]>)"));
        CHECK((infdiag.variable("d").toString()) == ("d:Range([0,0])"));
        CHECK((infdiag.variable("e").toString()) == ("e:Labelized({degre})"));

        CHECK(infdiag.isChanceNode("a"));
        CHECK(infdiag.isChanceNode("b"));
        CHECK(infdiag.isDecisionNode("c"));
        CHECK(infdiag.isUtilityNode("d"));
        CHECK(infdiag.isUtilityNode("e"));
      }
    }

    void testoperatorEqual() {
      auto model  = gum::InfluenceDiagram< double >::fastPrototype("C<-A->*B<-C->D->$U<-B;*E->B");
      auto model2 = gum::InfluenceDiagram< double >::fastPrototype(
          "C{yes|no}<-A{yes|no}->*B{yes|no}<-C->D{yes|no}->$U<-B;*E{yes|no}->B");
      auto model3 = gum::InfluenceDiagram< double >::fastPrototype("C<-A->*B<-C->D->$U<-B;*E->B");

      for (const auto i: model.nodes()) {
        if (model.isChanceNode(i)) {
          model.cpt(i).fillWith(1);
          model2.cpt(i).fillWith(1);
          model3.cpt(i).fillWith(1);
        } else if (model.isUtilityNode(i)) {
          model.utility(i).fillWith(1);
          model2.utility(i).fillWith(1);
          model3.utility(i).fillWith(1);
        }
      }

      CHECK((model) == (model));
      CHECK((model) != (model2));
      CHECK((model) == (model3));
    }
  };

  GUM_TEST_ACTIF(Constructor)
  GUM_TEST_ACTIF(NodeType)
  GUM_TEST_ACTIF(ToDot)
  GUM_TEST_ACTIF(CopyConstructor)
  GUM_TEST_ACTIF(Insertion_1)
  GUM_TEST_ACTIF(Insertion_2)
  GUM_TEST_ACTIF(ArcInsertion)
  GUM_TEST_ACTIF(EraseVar)
  GUM_TEST_ACTIF(EraseArc)
  GUM_TEST_ACTIF(Iterator)
  GUM_TEST_ACTIF(TopologicalOrder)
  GUM_TEST_ACTIF(Table)
  GUM_TEST_ACTIF(TableCoherencyVarRemoval)
  GUM_TEST_ACTIF(TableCoherencyArcRemoval)
  GUM_TEST_ACTIF(AccessorByName)
  GUM_TEST_ACTIF(DecisionPath)
  GUM_TEST_ACTIF(Moralisation)
  GUM_TEST_ACTIF(WithNames)
  GUM_TEST_ACTIF(FastPrototype)
  GUM_TEST_ACTIF(FastPrototypeVarType)
  GUM_TEST_ACTIF(FastVariable)
  GUM_TEST_ACTIF(operatorEqual)
}   // namespace gum_tests
