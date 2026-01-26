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


#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/variables/discreteVariable.h>
#include <agrum/base/variables/discretizedVariable.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/base/variables/rangeVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/maxParentsMCBayesNetGenerator.h>
#include <agrum/BN/generator/simpleBayesNetGenerator.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNReader.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  BayesNet
#define GUM_CURRENT_MODULE BN

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {
  struct BayesNetTestSuite {
    private:
    void fillTopo(gum::BayesNet< double >& bn, gum::List< gum::NodeId >& idList) const {
      try {
        idList.insert(bn.add(*var1));
        idList.insert(bn.add(*var2));
        idList.insert(bn.add(*var3));
        idList.insert(bn.add(*var4));
        idList.insert(bn.add(*var5));

        bn.addArc(idList[0], idList[2]);
        bn.addArc(idList[2], idList[4]);
        bn.addArc(idList[1], idList[3]);
        bn.addArc(idList[0], idList[3]);
        bn.addArc(idList[3], idList[4]);
        bn.addArc(idList[1], idList[4]);
      } catch (const gum::Exception& e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    void fill(gum::BayesNet< double >& bn, gum::List< gum::NodeId >& idList) {
      fillTopo(bn, idList);

      try {
        bn.cpt(idList[0]).fillWith({0.2f, 0.8f});

        bn.cpt(idList[1]).fillWith({0.3f, 0.7f});

        bn.cpt(idList[2]).fillWith({0.1f, 0.9f, 0.9f, 0.1f});

        bn.cpt(idList[3]).fillWith(   // clang-format off
            {0.4f, 0.6f,
             0.5f, 0.5f,
             0.5f, 0.5f,
             1.0f, 0.0f});                  // clang-format on

        bn.cpt(idList[4]).fillWith(   // clang-format off
            {0.3f, 0.6f, 0.1f,
             0.5f, 0.5f, 0.0f,
             0.5f, 0.5f, 0.0f,
             1.0f, 0.0f, 0.0f,
             0.4f, 0.6f, 0.0f,
             0.5f, 0.5f, 0.0f,
             0.5f, 0.5f, 0.0f,
             0.0f, 0.0f, 1.0f});   // clang-format on
      } catch (const gum::Exception& e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    gum::UndiGraph getRealMoralGraph(const gum::BayesNet< double >&  bn,
                                     const gum::List< gum::NodeId >& idList) {
      gum::UndiGraph graph;
      graph.populateNodes(bn.dag());

      graph.addEdge(idList[0], idList[2]);
      graph.addEdge(idList[0], idList[3]);
      graph.addEdge(idList[0], idList[1]);
      graph.addEdge(idList[2], idList[4]);
      graph.addEdge(idList[3], idList[4]);
      graph.addEdge(idList[2], idList[3]);
      graph.addEdge(idList[1], idList[3]);
      graph.addEdge(idList[1], idList[4]);
      graph.addEdge(idList[1], idList[2]);

      return graph;
    }

    public:
    gum::LabelizedVariable* var1;
    gum::LabelizedVariable* var2;
    gum::LabelizedVariable* var3;
    gum::LabelizedVariable* var4;
    gum::LabelizedVariable* var5;

    BayesNetTestSuite() {
      var1 = new gum::LabelizedVariable("var1", "1", 2);
      var2 = new gum::LabelizedVariable("var2", "2", 2);
      var3 = new gum::LabelizedVariable("var3", "3", 2);
      var4 = new gum::LabelizedVariable("var4", "4", 2);
      var5 = new gum::LabelizedVariable("var5", "(gum::Size) 3", 3);
    }

    ~BayesNetTestSuite() {
      delete var1;
      delete var2;
      delete var3;
      delete var4;
      delete var5;
    }

    void testConstructor() {
      gum::BayesNet< double >* topology = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(topology = new gum::BayesNet< double >());
      CHECK((topology->size()) == (static_cast< gum::Idx >(0)));
      CHECK((topology->sizeArcs()) == (static_cast< gum::Idx >(0)));
      CHECK((topology->dim()) == (static_cast< gum::Idx >(0)));

      CHECK_THROWS_AS(topology->addArc(1, 2), const gum::InvalidNode&);
      gum::List< gum::NodeId > idList;
      GUM_CHECK_ASSERT_THROWS_NOTHING(fill(*topology, idList));

      CHECK_THROWS_AS(topology->add(*var1), const gum::DuplicateLabel&);

      CHECK((topology->toString())
            == ("BN{nodes: 5, arcs: 6, domainSize: 48, "
                "dim: 24, mem: 320o}"));
      CHECK((topology->size()) == (static_cast< gum::Idx >(5)));
      CHECK((topology->sizeArcs()) == (static_cast< gum::Idx >(6)));
      CHECK((topology->dim()) == (static_cast< gum::Idx >(24)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(delete topology);
    }

    void testCopyConstructor() {
      gum::BayesNet< double >  source;
      gum::List< gum::NodeId > idList;
      GUM_CHECK_ASSERT_THROWS_NOTHING(fill(source, idList));

      gum::BayesNet< double >* copy = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(copy = new gum::BayesNet< double >(source));

      CHECK((source.dag().size()) == (copy->dag().size()));
      CHECK((source.dag().sizeArcs()) == (copy->dag().sizeArcs()));

      for (const gum::DAG dag = source.dag(); const auto node: dag.nodes()) {
        CHECK(copy->dag().exists(node));

        const gum::DiscreteVariable& srcVar = source.variable(node);
        const gum::DiscreteVariable& cpVar  = copy->variable(node);
        CHECK((srcVar.name()) == (cpVar.name()));

        if (srcVar.domainSize() == cpVar.domainSize()) {
          for (gum::Idx i = 0; i < srcVar.domainSize(); i++) {
            CHECK((srcVar.label(i)) == (cpVar.label(i)));
          }
        } else {
          CHECK(false);
        }

        for (const auto parent: source.dag().parents(node)) {
          CHECK(copy->dag().existsArc(gum::Arc(parent, node)));
        }

        const gum::Tensor< double >& srcCPT = source.cpt(node);
        const gum::Tensor< double >& cpCPT  = copy->cpt(node);

        gum::Instantiation srcInst(srcCPT);
        gum::Instantiation cpInst(cpCPT);

        for (cpInst.setFirst(); !cpInst.end(); cpInst.inc()) {
          for (gum::Idx i = 0; i < cpInst.nbrDim(); i++) {
            gum::NodeId id = copy->nodeId(cpInst.variable(i));
            srcInst.chgVal(source.variable(id), cpInst.val(i));
          }

          CHECK((cpCPT[cpInst]) == (srcCPT[srcInst]));
        }
      }

      if (copy) { delete copy; }
    }

    void testCopyOperator() {
      gum::BayesNet< double >  source;
      gum::BayesNet< double >  copy;
      gum::List< gum::NodeId > idList;
      GUM_CHECK_ASSERT_THROWS_NOTHING(fill(source, idList));

      GUM_CHECK_ASSERT_THROWS_NOTHING(copy = source);

      CHECK((source.dag().size()) == (copy.dag().size()));
      CHECK((source.dag().sizeArcs()) == (copy.dag().sizeArcs()));

      for (const gum::DAG dag = source.dag(); const auto node: dag.nodes()) {
        CHECK(copy.dag().exists(node));

        const gum::DiscreteVariable& srcVar = source.variable(node);
        const gum::DiscreteVariable& cpVar  = copy.variable(node);
        CHECK((srcVar.name()) == (cpVar.name()));

        if (srcVar.domainSize() == cpVar.domainSize()) {
          for (gum::Idx i = 0; i < srcVar.domainSize(); i++) {
            CHECK((srcVar.label(i)) == (cpVar.label(i)));
          }
        } else {
          CHECK(false);
        }

        for (const auto parent: source.dag().parents(node)) {
          CHECK(copy.dag().existsArc(gum::Arc(parent, node)));
        }

        const gum::Tensor< double >& srcCPT = source.cpt(node);
        const gum::Tensor< double >& cpCPT  = copy.cpt(node);
        gum::Instantiation           srcInst(srcCPT);
        gum::Instantiation           cpInst(cpCPT);

        for (cpInst.setFirst(); !cpInst.end(); cpInst.inc()) {
          for (gum::Idx i = 0; i < cpInst.nbrDim(); i++) {
            gum::NodeId id = copy.nodeId(cpInst.variable(i));
            srcInst.chgVal(source.variable(id), cpInst.val(i));
          }

          CHECK((cpCPT[cpInst]) == (srcCPT[srcInst]));
        }
      }

      for (const auto node: copy.nodes()) {
        std::stringstream c_str;
        std::stringstream s_str;

        for (const auto var: source.cpt(node).variablesSequence()) {
          s_str << *var << ",";
        }

        for (const auto var: copy.cpt(node).variablesSequence()) {
          c_str << *var << ",";
        }

        CHECK((c_str.str()) == (s_str.str()));
      }
    }

    void testInsertion_1() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var1)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var2)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var3)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var4)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var5)));

      CHECK((bn.size()) == (static_cast< gum::Size >(5)));
      CHECK((bn.dag().size()) == (static_cast< gum::Size >(5)));

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

      GUM_CHECK_ASSERT_THROWS_NOTHING(varPtr = (gum::LabelizedVariable*)&bn.variable(idList[0]));
      CHECK((*varPtr) == (*var1));

      CHECK((*((gum::LabelizedVariable*)&bn.variable(idList[0]))) == (*var1));
      CHECK((*((gum::LabelizedVariable*)&bn.variable(idList[1]))) == (*var2));
      CHECK((*((gum::LabelizedVariable*)&bn.variable(idList[2]))) == (*var3));
      CHECK((*((gum::LabelizedVariable*)&bn.variable(idList[3]))) == (*var4));
      CHECK((*((gum::LabelizedVariable*)&bn.variable(idList[4]))) == (*var5));
    }

    void testInsertion_2() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var1)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var2)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var3)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var4)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var5)));

      CHECK((bn.size()) == (static_cast< gum::Size >(5)));
      CHECK((bn.dag().size()) == (static_cast< gum::Size >(5)));

      gum::LabelizedVariable const* varPtr = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(varPtr = (gum::LabelizedVariable*)&bn.variable(idList[0]));
      CHECK((*varPtr) == (*var1));

      CHECK((*((gum::LabelizedVariable*)&bn.variable(idList[0]))) == (*var1));
      CHECK((*((gum::LabelizedVariable*)&bn.variable(idList[1]))) == (*var2));
      CHECK((*((gum::LabelizedVariable*)&bn.variable(idList[2]))) == (*var3));
      CHECK((*((gum::LabelizedVariable*)&bn.variable(idList[3]))) == (*var4));
      CHECK((*((gum::LabelizedVariable*)&bn.variable(idList[4]))) == (*var5));
    }

    void testIterations() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;
      GUM_CHECK_ASSERT_THROWS_NOTHING(fill(bn, idList));

      auto cpt = static_cast< gum::Size >(0);

      for (const auto node: bn.nodes()) {
        GUM_UNUSED(node);
        cpt++;
      }

      CHECK((cpt) == (bn.size()));

      cpt = static_cast< gum::Size >(0);
      for (const auto& arc: bn.arcs()) {
        GUM_UNUSED(arc);
        cpt++;
      }

      CHECK((cpt) == (bn.sizeArcs()));
    }

    void testArcInsertion() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      idList.insert(bn.add(*var1));
      idList.insert(bn.add(*var2));
      idList.insert(bn.add(*var3));
      idList.insert(bn.add(*var4));
      idList.insert(bn.add(*var5));

      GUM_CHECK_ASSERT_THROWS_NOTHING(bn.addArc(idList[0], idList[2]));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bn.addArc(idList[2], idList[4]));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bn.addArc(idList[1], idList[3]));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bn.addArc(idList[0], idList[3]));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bn.addArc(idList[3], idList[4]));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bn.addArc(idList[1], idList[4]));

      CHECK_THROWS_AS(bn.addArc(idList[3], idList[4]), const gum::DuplicateElement&);

      CHECK((bn.dag().sizeArcs()) == (static_cast< gum::Size >(6)));
    }

    void testEraseVar() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      CHECK(bn.empty());
      CHECK(bn.dag().emptyArcs());

      fill(bn, idList);

      CHECK(!bn.empty());
      CHECK(!bn.dag().emptyArcs());

      CHECK((bn.size()) == (static_cast< gum::Size >(5)));
      CHECK((bn.dag().size()) == (static_cast< gum::Size >(5)));
      CHECK((bn.dag().sizeArcs()) == (static_cast< gum::Size >(6)));

      bn.erase(idList[0]);

      for (const auto i: idList) {
        bn.erase(i);
      }

      CHECK(bn.empty());

      CHECK(bn.dag().emptyArcs());

      CHECK((bn.size()) == (static_cast< gum::Size >(0)));
      CHECK((bn.dag().size()) == (static_cast< gum::Size >(0)));
      CHECK((bn.dag().sizeArcs()) == (static_cast< gum::Size >(0)));

      idList.clear();
      GUM_CHECK_ASSERT_THROWS_NOTHING(fill(bn, idList));

      CHECK(!bn.empty());
      CHECK(!bn.dag().emptyArcs());

      CHECK((bn.size()) == (static_cast< gum::Size >(5)));
      CHECK((bn.dag().size()) == (static_cast< gum::Size >(5)));
      CHECK((bn.dag().sizeArcs()) == (static_cast< gum::Size >(6)));
    }

    void testEraseArc() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      CHECK(bn.empty());
      CHECK(bn.dag().emptyArcs());

      fill(bn, idList);

      CHECK(!bn.empty());
      CHECK(!bn.dag().emptyArcs());

      CHECK((bn.size()) == (static_cast< gum::Size >(5)));
      CHECK((bn.dag().size()) == (static_cast< gum::Size >(5)));
      CHECK((bn.dag().sizeArcs()) == (static_cast< gum::Size >(6)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(bn.eraseArc(gum::Arc(idList[0], idList[2])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bn.eraseArc(gum::Arc(idList[2], idList[4])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bn.eraseArc(gum::Arc(idList[1], idList[3])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bn.eraseArc(gum::Arc(idList[0], idList[3])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bn.eraseArc(gum::Arc(idList[3], idList[4])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bn.eraseArc(gum::Arc(idList[1], idList[4])));

      CHECK(!bn.empty());
      CHECK(bn.dag().emptyArcs());
    }

    void testStringAccessors() {
      try {
        gum::BayesNet< double > bn;
        for (const auto& x: {"A", "B", "C"}) {
          bn.add(gum::LabelizedVariable(x, x, 2));
        }
        for (const auto& [first, second]: {std::make_pair("A", "C"), std::make_pair("B", "C")}) {
          bn.addArc(first, second);
        }
        CHECK_THROWS_AS(bn.addArc("A", "C"), const gum::DuplicateElement&);

        CHECK((bn.toString())
              == ("BN{nodes: 3, arcs: 2, domainSize: 8, "
                  "dim: 6, mem: 96o}"));

        bn.cpt("A").fillWith(1.0f).normalize();
        bn.generateCPT("B");
        bn.generateCPT("C");
        CHECK_THROWS_AS(bn.cpt("XXX"), const gum::NotFound&);

        bn.reverseArc("A", "C");
        CHECK((bn.toString())
              == ("BN{nodes: 3, arcs: 3, domainSize: 8, "
                  "dim: 7, mem: 112o}"));

        CHECK_THROWS_AS(bn.reverseArc("A", "C"), const gum::InvalidArc&);
        CHECK_THROWS_AS(bn.reverseArc("A", "C"), const gum::GraphError&);
        CHECK_THROWS_AS(bn.reverseArc("A", "X"), const gum::NotFound&);

        bn.erase("A");
        CHECK((bn.toString())
              == ("BN{nodes: 2, arcs: 1, domainSize: 4, "
                  "dim: 3, mem: 48o}"));

        CHECK_THROWS_AS(bn.erase("A"), const gum::NotFound&);

        bn.eraseArc("B", "C");
        CHECK((bn.toString())
              == ("BN{nodes: 2, arcs: 0, domainSize: 4, "
                  "dim: 2, mem: 32o}"));

        CHECK_THROWS_AS(bn.eraseArc("B", "C"), const gum::NotFound&);
      } catch (const gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    void testIterator() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      for (const auto node: bn.nodes()) {
        CHECK(idList.exists(node));
      }
    }

    void testRandomlyFilled() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);

      for (const auto node: bn.nodes()) {
        std::stringstream s1;
        std::stringstream s2;
        s1 << bn.cpt(node);

        bn.generateCPTs();
        s2 << bn.cpt(node);
        CHECK((s1.str()) != (s2.str()));
      }
    }

    void testMoralGraph() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);

      gum::UndiGraph graph;
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph = bn.moralGraph());
      CHECK((graph) == (getRealMoralGraph(bn, idList)));
    }

    void testTopologicalOrder() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);

      gum::Sequence< gum::NodeId > topoOrder;
      GUM_CHECK_ASSERT_THROWS_NOTHING(topoOrder = bn.topologicalOrder());

      CHECK((topoOrder.size()) == (static_cast< gum::Size >(5)));
    }

    void testcpt() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);
      CHECK((bn.cpt(idList[0]).domainSize()) == (static_cast< gum::Size >(2)));
      CHECK((bn.cpt(idList[1]).domainSize()) == (static_cast< gum::Size >(2)));
      CHECK((bn.cpt(idList[2]).domainSize()) == (static_cast< gum::Size >(4)));
      CHECK((bn.cpt(idList[3]).domainSize()) == (static_cast< gum::Size >(8)));
      CHECK((bn.cpt(idList[4]).domainSize()) == (static_cast< gum::Size >(24)));
    }

    void testCPTCoherencyVarRemoval() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);

      CHECK((bn.dag().parents(idList[3]).size()) == (static_cast< gum::Size >(2)));
      CHECK(bn.dag().existsArc(idList[0], idList[3]));
      CHECK(bn.dag().existsArc(idList[1], idList[3]));

      gum::Size dmnSize_1
          = bn.variable(idList[0]).domainSize() * bn.variable(idList[1]).domainSize();
      dmnSize_1 *= bn.variable(idList[3]).domainSize();
      CHECK((bn.cpt(idList[3]).domainSize()) == (dmnSize_1));

      CHECK(bn.cpt(idList[3]).contains(bn.variable(idList[0])));
      CHECK(bn.cpt(idList[3]).contains(bn.variable(idList[1])));
      CHECK(bn.cpt(idList[3]).contains(bn.variable(idList[3])));

      GUM_CHECK_ASSERT_THROWS_NOTHING(bn.erase(idList[1]));

      CHECK((bn.dag().parents(idList[3]).size()) == (static_cast< gum::Size >(1)));
      CHECK(bn.dag().existsArc(idList[0], idList[3]));
      CHECK(!bn.dag().existsArc(idList[1], idList[3]));

      gum::Size dmnSize_2 = bn.variable(idList[0]).domainSize();
      dmnSize_2 *= bn.variable(idList[3]).domainSize();
      CHECK((bn.cpt(idList[3]).domainSize()) == (dmnSize_2));

      CHECK(bn.cpt(idList[3]).contains(bn.variable(idList[0])));
      CHECK_THROWS(bn.cpt(idList[3]).contains(bn.variable(idList[1])));
      CHECK(bn.cpt(idList[3]).contains(bn.variable(idList[3])));
    }

    void testCPTCoherencyArcRemoval() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);

      CHECK((bn.dag().parents(idList[3]).size()) == (static_cast< gum::Size >(2)));
      CHECK(bn.dag().existsArc(idList[0], idList[3]));
      CHECK(bn.dag().existsArc(idList[1], idList[3]));

      gum::Size dmnSize_1
          = bn.variable(idList[0]).domainSize() * bn.variable(idList[1]).domainSize();
      dmnSize_1 *= bn.variable(idList[3]).domainSize();
      CHECK((bn.cpt(idList[3]).domainSize()) == (dmnSize_1));

      CHECK(bn.cpt(idList[3]).contains(bn.variable(idList[0])));
      CHECK(bn.cpt(idList[3]).contains(bn.variable(idList[1])));
      CHECK(bn.cpt(idList[3]).contains(bn.variable(idList[3])));

      GUM_CHECK_ASSERT_THROWS_NOTHING(bn.eraseArc(gum::Arc(idList[1], idList[3])));

      CHECK((bn.dag().parents(idList[3]).size()) == (static_cast< gum::Size >(1)));
      CHECK(bn.dag().existsArc(idList[0], idList[3]));
      CHECK(!bn.dag().existsArc(idList[1], idList[3]));

      gum::Size dmnSize_2 = bn.variable(idList[0]).domainSize();
      dmnSize_2 *= bn.variable(idList[3]).domainSize();
      CHECK((bn.cpt(idList[3]).domainSize()) == (dmnSize_2));

      CHECK(bn.cpt(idList[3]).contains(bn.variable(idList[0])));
      CHECK(!bn.cpt(idList[3]).contains(bn.variable(idList[1])));
      CHECK(bn.cpt(idList[3]).contains(bn.variable(idList[3])));
    }

    void testStreamOperator() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);
      std::stringstream sBuff;
      GUM_CHECK_ASSERT_THROWS_NOTHING(sBuff << bn);
    }

    void testAccessorByName() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);

      for (const auto node: bn.nodes()) {
        CHECK((bn.idFromName(bn.variable(node).name())) == (node));
        CHECK((&bn.variableFromName(bn.variable(node).name())) == (&bn.variable(node)));
        CHECK((&bn.variableFromName(bn.variable(node).name())) == (&bn.variable(node)));
      }

      CHECK_THROWS_AS(bn.idFromName("choucroute"), const gum::NotFound&);

      CHECK_THROWS_AS(bn.variableFromName("choucroute"), const gum::NotFound&);

      GUM_CHECK_ASSERT_THROWS_NOTHING(bn.idFromName("var1"));
      bn.erase(bn.idFromName("var1"));
      CHECK_THROWS_AS(bn.idFromName("var1"), const gum::NotFound&);
    }

    void testCopyAndEqualityOperators() {
      auto                                   bn_1 = new gum::BayesNet< double >();
      gum::SimpleBayesNetGenerator< double > generator(20, 30, 4);
      generator.generateBN(*bn_1);

      auto bn_2 = new gum::BayesNet< double >();

      generator.generateBN(*bn_2);

      gum::BayesNet< double > bn_cpy_1 = *bn_1;
      gum::BayesNet< double > bn_cpy_2 = *bn_2;

      CHECK((bn_cpy_1) == (bn_cpy_1));
      CHECK((bn_cpy_1) == ((*bn_1)));
      CHECK((bn_cpy_2) == (bn_cpy_2));
      CHECK((bn_cpy_2) == ((*bn_2)));
      CHECK(((*bn_1)) != ((*bn_2)));
      CHECK((bn_cpy_1) != (bn_cpy_2));

      CHECK((bn_cpy_1) == (bn_cpy_1));
      CHECK((bn_cpy_1) == ((*bn_1)));
      CHECK((bn_cpy_2) == (bn_cpy_2));
      CHECK((bn_cpy_2) == ((*bn_2)));
      CHECK(((*bn_1)) != ((*bn_2)));
      CHECK((bn_cpy_1) != (bn_cpy_2));

      gum::BayesNet< double > cpy = *bn_1;
      CHECK((cpy) == ((*bn_1)));
      CHECK((cpy) != ((*bn_2)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(cpy = *bn_2);

      CHECK((cpy) == ((*bn_2)));

      CHECK((cpy) == ((*bn_2)));
      CHECK((cpy) != ((*bn_1)));
      CHECK((cpy) != ((*bn_1)));

      CHECK_NOTHROW(cpy = cpy);

      CHECK((cpy) == ((*bn_2)));

      CHECK((cpy) == ((*bn_2)));
      CHECK((cpy) != ((*bn_1)));
      CHECK((cpy) != ((*bn_1)));

      std::string s1 = cpy.toString();
      delete bn_2;
      delete bn_1;
      std::string s2 = cpy.toString();
      CHECK((s1) == (s2));
    }

    void testAggregatorNodes() {
      {
        gum::BayesNet< double > bn;
        gum::Idx                i1 = 0;

        GUM_CHECK_ASSERT_THROWS_NOTHING(i1 = bn.addOR(*var1));
        CHECK_THROWS_AS(bn.addOR(*var5), const gum::SizeError&);

        bn.addArc(bn.add(*var3), i1);
        bn.addArc(bn.add(*var4), i1);
        bn.addArc(bn.add(*var5), i1);
      }
      {
        gum::BayesNet< double > bn;
        gum::Idx                i2 = 0;

        GUM_CHECK_ASSERT_THROWS_NOTHING(i2 = bn.addAND(*var2));
        CHECK_THROWS_AS(bn.addAND(*var5), const gum::SizeError&);

        bn.addArc(bn.add(*var3), i2);
        bn.addArc(bn.add(*var4), i2);
        bn.addArc(bn.add(*var5), i2);
      }
    }

    void testJointProbability() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);

      auto i = bn.completeInstantiation();
      CHECK((i.nbrDim()) == (bn.size()));

      CHECK((i.domainSize()) == ((gum::Size)(2 * 2 * 2 * 2 * 3)));

      double a = 0.0;
      CHECK_NOTHROW(a = bn.jointProbability(i));

      CHECK((a) == doctest::Approx(0.00072).epsilon(1e-4));

      gum::Instantiation j;
      for (auto n: bn.nodes()) {
        CHECK_THROWS_AS(bn.jointProbability(j), const gum::InvalidArgument&);
        j.add(bn.variable(n));
      }
      double b = 0.0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(b = bn.jointProbability(j));

      CHECK((a) == (b));
    }

    void testArcReversal() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;
      fill(bn, idList);

      std::vector< double > joint;
      joint.reserve(2 * bn.dim());
      auto i = bn.completeInstantiation();

      for (i.setFirst(); !i.end(); i.inc()) {
        joint.push_back(bn.jointProbability(i));
      }

      bn.reverseArc(0, 2);
      bn.reverseArc(gum::Arc(3, 4));
      CHECK_THROWS_AS(bn.reverseArc(gum::Arc(3, 4)), const gum::InvalidArc&);
      CHECK_THROWS_AS(bn.reverseArc(gum::Arc(3, 5)), const gum::InvalidArc&);
      CHECK_THROWS_AS(bn.reverseArc(gum::Arc(2, 4)), const gum::InvalidArc&);

      unsigned int j;
      for (j = 0, i.setFirst(); !i.end(); i.inc(), ++j) {
        CHECK((bn.jointProbability(i)) == doctest::Approx(joint[j]).epsilon(GUM_SMALL_ERROR));
      }
    }

    void testChangeLabelOfVariable() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;
      fill(bn, idList);

      CHECK((bn.variable(0).toString()) == ("var1:Labelized({0|1})"));
      CHECK_NOTHROW(
          dynamic_cast< const gum::LabelizedVariable& >(bn.variable(0)).changeLabel(0, "x"));
      CHECK((bn.variable(0).toString()) == ("var1:Labelized({x|1})"));
    }

    void testShortCutAddFastVar() {
      gum::BayesNet< double > bn;

      gum::NodeId i1;
      gum::NodeId i2;
      gum::NodeId i3;

      CHECK_NOTHROW(i1 = bn.add("A", 2));
      CHECK_NOTHROW(i2 = bn.add("B", 3));
      CHECK((i1) == (gum::NodeId(0)));
      CHECK((i2) == (gum::NodeId(1)));

      CHECK_THROWS_AS(i3 = bn.add("A", 5), const gum::DuplicateLabel&);
      // the variable "C",1 can be created but the BN does not allow to add such a variable
      CHECK_THROWS_AS(i3 = bn.add("C", 1), const gum::OperationNotAllowed&);
      GUM_UNUSED(i3);

      CHECK_NOTHROW(bn.addArc(i1, i2));
      CHECK((bn.log10DomainSize()) == (std::log10(2.0 * 3.0)));

      CHECK_NOTHROW(bn.add("X{top|middle|bottom}"));
      CHECK((bn.variable("X").toString()) == ("X:Labelized({top|middle|bottom})"));

      // a mono-label with default 4 is impossible
      CHECK_THROWS_AS(bn.add("Y[1,1]", 4), const gum::ArgumentError&);

      // a mono-label with default 1 is possible but can not be integrated in the model
      CHECK_THROWS_AS(bn.add("Y[1,1]", 1), const gum::OperationNotAllowed&);
    }

    void testSomeFunctions() {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;
      GUM_CHECK_ASSERT_THROWS_NOTHING(fill(bn, idList));

      CHECK((bn.maxVarDomainSize()) == (static_cast< gum::Size >(3)));
      CHECK((bn.minParam()) == (0.0f));
      CHECK((bn.maxParam()) == (1.0f));
      CHECK((bn.minNonZeroParam()) == (0.1f));
      CHECK((bn.maxNonOneParam()) == (0.9f));
    }

    void testMinimalCondSet() {
      /*
       A   B        0   1
        \ / \        \ / \
      H  C   D     7  2   3
       \ |   |      \ |   |
         E   |        4   |
          \ /          \ /
           F            5
           |            |
           G            6
       */
      gum::BayesNet< int > bn;

      auto a = bn.add("A", 2);
      auto b = bn.add("B", 2);
      auto c = bn.add("C", 2);
      auto d = bn.add("D", 2);
      auto e = bn.add("E", 2);
      auto f = bn.add("F", 2);
      auto g = bn.add("G", 2);
      auto h = bn.add("H", 2);

      bn.addArc(a, c);
      bn.addArc(b, c);
      bn.addArc(b, d);
      bn.addArc(c, e);
      bn.addArc(d, f);
      bn.addArc(e, f);
      bn.addArc(f, g);
      bn.addArc(h, e);

      CHECK((bn.minimalCondSet(c, {a, b, c, d, e, f, g, h})) == (gum::NodeSet({c})));
      CHECK((bn.minimalCondSet(c, {a, b, d, e, f, g, h})) == (gum::NodeSet({a, b, e, h})));
      CHECK((bn.minimalCondSet(c, {e, f, g})) == (gum::NodeSet({e, f})));
      CHECK((bn.minimalCondSet(c, {b, e, f, g})) == (gum::NodeSet({e, b})));
      CHECK((bn.minimalCondSet(c, {a, e, f, g})) == (gum::NodeSet({a, e, f})));

      // for set of targets
      CHECK((bn.minimalCondSet({e, d}, {a, b, c, d, e, f, g, h})) == (gum::NodeSet({d, e})));
      CHECK((bn.minimalCondSet({e, d}, {a, b, c, d, f, g, h})) == (gum::NodeSet({c, d, h, f})));
      CHECK((bn.minimalCondSet({e, d}, {a, b, c, f, g, h})) == (gum::NodeSet({b, c, f, h})));
    }

    void testMinimalCondSet2() {
      /*
      F  A
      \ / \
       B   |
       |   E
       C   |
      / \ /
      H  D
      */
      gum::BayesNet< int > bn;
      //                         0    1    2    3    4    5    6
      for (const auto& item: {"A", "B", "C", "D", "E", "F", "H"})
        bn.add(item, 2);
      bn.addArc("A", "B");
      bn.addArc("A", "E");
      bn.addArc("B", "C");
      bn.addArc("C", "D");
      bn.addArc("C", "H");
      bn.addArc("E", "D");
      bn.addArc("F", "B");

      CHECK((bn.minimalCondSet(4, gum::NodeSet({0, 1, 2, 3, 5, 6}))) == (gum::NodeSet({0, 2, 3})));

      CHECK((bn.minimalCondSet(4, gum::NodeSet({0, 1, 5}))) == (gum::NodeSet({0})));

      CHECK((bn.minimalCondSet(4, gum::NodeSet({0, 1, 3, 5}))) == (gum::NodeSet({0, 1, 3})));
    }

    void testFastPrototype() {
      try {
        auto bn = gum::BayesNet< float >::fastPrototype("a->b->c;a->c");
        CHECK((bn.size()) == (static_cast< gum::Size >(3)));
        CHECK((bn.sizeArcs()) == (static_cast< gum::Size >(3)));
        CHECK((bn.dim()) == (gum::Size((2 - 1) + (2 * (2 - 1)) + (2 * 2 * (2 - 1)))));

        bn = gum::BayesNet< float >::fastPrototype("a->b->c;a->c", 3);
        CHECK((bn.size()) == (static_cast< gum::Size >(3)));
        CHECK((bn.sizeArcs()) == (static_cast< gum::Size >(3)));
        CHECK((bn.dim()) == (gum::Size((3 - 1) + (3 * (3 - 1)) + (3 * 3 * (3 - 1)))));

        bn = gum::BayesNet< float >::fastPrototype("a->b[5]->c;a->c");
        CHECK((bn.size()) == (static_cast< gum::Size >(3)));
        CHECK((bn.sizeArcs()) == (static_cast< gum::Size >(3)));
        CHECK((bn.dim()) == (gum::Size((2 - 1) + (2 * (5 - 1)) + (5 * 2 * (2 - 1)))));

        bn = gum::BayesNet< float >::fastPrototype("a->b->c;a[1000]->c");
        CHECK((bn.size()) == (static_cast< gum::Size >(3)));
        CHECK((bn.sizeArcs()) == (static_cast< gum::Size >(3)));
        CHECK((bn.dim()) == (gum::Size((2 - 1) + (2 * (2 - 1)) + (2 * 2 * (2 - 1)))));

        CHECK_THROWS_AS(bn = gum::BayesNet< float >::fastPrototype("a->b->c->a"),
                        const gum::InvalidDirectedCycle&);

        bn = gum::BayesNet< float >::fastPrototype("a{yes|maybe|no}->b->c;a->c");
        CHECK((bn.size()) == (static_cast< gum::Size >(3)));
        CHECK((bn.sizeArcs()) == (static_cast< gum::Size >(3)));
        CHECK((bn.dim()) == (gum::Size((3 - 1) + (3 * (2 - 1)) + (3 * 2 * (2 - 1)))));

        CHECK_THROWS_AS(gum::BayesNet< float >::fastPrototype("a{yes}->b->c;a->c"),
                        const gum::InvalidArgument&);
        CHECK_THROWS_AS(gum::BayesNet< float >::fastPrototype("a{yes|no|yes}->b->c;a->c"),
                        const gum::InvalidArgument&);

        bn = gum::BayesNet< float >::fastPrototype("a->b->c->d->e->f");
        CHECK((bn.size()) == (static_cast< gum::Size >(6)));
        CHECK((bn.sizeArcs()) == (static_cast< gum::Size >(5)));
        CHECK((bn.dim()) == (gum::Size(1 + 5 * 2)));

        bn = gum::BayesNet< float >::fastPrototype("a<-b<-c<-d<-e<-f");
        CHECK((bn.size()) == (static_cast< gum::Size >(6)));
        CHECK((bn.sizeArcs()) == (static_cast< gum::Size >(5)));
        CHECK((bn.dim()) == (gum::Size(1 + 5 * 2)));

        bn = gum::BayesNet< float >::fastPrototype("a<-b->c<-d->e<-f");
        CHECK((bn.size()) == (static_cast< gum::Size >(6)));
        CHECK((bn.sizeArcs()) == (static_cast< gum::Size >(5)));
        CHECK((bn.dim()) == (gum::Size(3 * 1 + 2 + 2 * 4)));

        bn = gum::BayesNet< float >::fastPrototype("a->b<-c->d<-e->f");
        CHECK((bn.size()) == (static_cast< gum::Size >(6)));
        CHECK((bn.sizeArcs()) == (static_cast< gum::Size >(5)));
        CHECK((bn.dim()) == (gum::Size(3 * 1 + 2 + 2 * 4)));

        bn = gum::BayesNet< float >::fastPrototype("a->b<-c;c->d<-e->f");
        CHECK((bn.size()) == (static_cast< gum::Size >(6)));
        CHECK((bn.sizeArcs()) == (static_cast< gum::Size >(5)));
        CHECK((bn.dim()) == (gum::Size(3 * 1 + 2 + 2 * 4)));

        bn = gum::BayesNet< float >::fastPrototype("a->b<-c->d;d<-e->f");
        CHECK((bn.size()) == (static_cast< gum::Size >(6)));
        CHECK((bn.sizeArcs()) == (static_cast< gum::Size >(5)));
        CHECK((bn.dim()) == (gum::Size(3 * 1 + 2 + 2 * 4)));

        bn = gum::BayesNet< float >::fastPrototype("a->b;b<-c;c->d;d<-e;e->f");
        CHECK((bn.size()) == (static_cast< gum::Size >(6)));
        CHECK((bn.sizeArcs()) == (static_cast< gum::Size >(5)));
        CHECK((bn.dim()) == (gum::Size(3 * 1 + 2 + 2 * 4)));
      } catch (const gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    void testFastPrototypeVarType() {
      auto bn = gum::BayesNet< float >::fastPrototype("a");
      CHECK((bn.variable("a").toString()) == ("a:Range([0,1])"));

      bn = gum::BayesNet< float >::fastPrototype("a[0,1]");
      CHECK((bn.variable("a").toString()) == ("a:Range([0,1])"));

      CHECK_THROWS_AS(gum::BayesNet< float >::fastPrototype("a[0,0]"), const gum::InvalidArgument&);
      CHECK_THROWS_AS(gum::BayesNet< float >::fastPrototype("a[1,0]"), const gum::InvalidArgument&);
      CHECK_THROWS_AS(gum::BayesNet< float >::fastPrototype("a[1,1]"), const gum::InvalidArgument&);

      bn = gum::BayesNet< float >::fastPrototype("a[5]");
      CHECK((bn.variable("a").toString()) == ("a:Range([0,4])"));

      bn = gum::BayesNet< float >::fastPrototype("a[2,5]");
      CHECK((bn.variable("a").toString()) == ("a:Range([2,5])"));

      bn = gum::BayesNet< float >::fastPrototype("a[-2,2]");
      CHECK((bn.variable("a").toString()) == ("a:Range([-2,2])"));

      bn = gum::BayesNet< float >::fastPrototype("a[-0.4,0.1,0.5,3.14,10]");
      CHECK((bn.variable("a").toString())
            == ("a:Discretized(<[-0.4;0.1[,[0.1;0.5[,[0.5;3.14[,[3.14;10]>)"));

      bn = gum::BayesNet< float >::fastPrototype(
          "a{1|4|6}->b{1|-4|6}->c{1|toto|6}->d{1.0|-4.0|6.0}->e{1|-4|6.0}->f{1.0|-4.0|+6.0}");
      CHECK((bn.variable("a").varType()) == (gum::VarType::INTEGER));
      CHECK((bn.variable("b").varType()) == (gum::VarType::INTEGER));
      CHECK((bn.variable("c").varType()) == (gum::VarType::LABELIZED));
      CHECK((bn.variable("d").varType()) == (gum::VarType::NUMERICAL));
      CHECK((bn.variable("e").varType()) == (gum::VarType::NUMERICAL));
      CHECK((bn.variable("f").varType()) == (gum::VarType::NUMERICAL));
    }

    void testNonRegressionCPTinReverseArc() {
      const auto bn1 = gum::BayesNet< double >::fastPrototype("A->B->C");
      auto       bn2 = gum::BayesNet< double >(bn1);
      bn2.reverseArc("A", "B");
      CHECK((bn1.cpt("A").variable(0).name()) == ("A"));
      CHECK((bn2.cpt("A").variable(0).name()) == ("A"));
      CHECK((bn1.cpt("B").variable(0).name()) == ("B"));
      CHECK((bn2.cpt("B").variable(0).name()) == ("B"));
    }

    void testClearBN() {
      const auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D->E<-A<-G->F");
      {
        // by hand
        auto bn_instance = gum::BayesNet< double >(bn);
        if (!bn_instance.empty()) {
          auto l = bn_instance.nodes();
          for (const auto no: l) {
            bn_instance.erase(no);
          }
        }
        CHECK(bn_instance.empty());
      }
      {
        // with clear method
        auto bn_instance = gum::BayesNet< double >(bn);
        bn_instance.clear();
        CHECK(bn_instance.empty());
      }
    }

    void testExistsArc() {
      const auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D->E<-A<-G->F");

      CHECK(bn.existsArc(0, 1));
      CHECK(bn.existsArc("A", "B"));
      CHECK(!bn.existsArc(1, 0));
      CHECK(!bn.existsArc("B", "A"));
      CHECK(!bn.existsArc(0, 2));
      CHECK(!bn.existsArc("A", "C"));
    }

    void testCheck() {
      {
        const auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D->E<-A<-G->F");
        CHECK((bn.check().size()) == (gum::Size(0)));

        bn.cpt("B").fillWith(-1);
        CHECK((bn.check().size()) == (gum::Size(2)));
      }
      {
        const auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D->E<-A<-G->F");
        CHECK((bn.check().size()) == (gum::Size(0)));

        bn.cpt("B").fillWith(0);
        CHECK((bn.check().size()) == (gum::Size(1)));

        bn.cpt("C").fillWith(2);
        CHECK((bn.check().size()) == (gum::Size(3)));
      }
      {
        const auto bn
            = gum::BayesNet< double >::fastPrototype("Y->X->T1;Z2->X;Z1->X;Z1->T1;Z1->Z3->T2");
        CHECK((bn.check().size()) == (gum::Size(0)));

        bn.cpt("X").scale(12);
        CHECK((bn.check().size()) == (gum::Size(2)));

        bn.cpt("Z1").scale(12);
        CHECK((bn.check().size()) == (gum::Size(4)));
      }
    }

    void testThreads() {
      const std::string bn_model_path = GET_RESSOURCES_PATH("bifxml/hailfinder.bifxml");
      const std::string bn_name       = "myBN";

      try {
        gum::BayesNet< double > bn(bn_name);
        auto                    reader = gum::BIFXMLBNReader(&bn, bn_model_path);
        reader.proceed();

        std::atomic< gum::Size > nbth{0};
        auto opExecute = [&bn, &nbth](const std::size_t this_thread, const std::size_t nb_threads) {
          nbth += bn.maxVarDomainSize();
        };
        gum::ThreadExecutor::execute(20, opExecute);

        CHECK((gum::Size(nbth)) == (20 * bn.maxVarDomainSize()));
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    void testoperatorEqual() {
      auto bn  = gum::BayesNet< double >::fastPrototype("A->B->C");
      auto bn2 = gum::BayesNet< double >::fastPrototype("A{yes|no}->B{yes|no}->C{yes|no}");
      auto bn3 = gum::BayesNet< double >::fastPrototype("A->B->C");
      for (const auto i: bn.nodes()) {
        bn.cpt(i).fillWith(1);
        bn2.cpt(i).fillWith(1);
        bn3.cpt(i).fillWith(1);
      }
      CHECK((bn) == (bn));
      CHECK((bn) != (bn2));
      CHECK((bn) == (bn3));
    }

    void testcontextulizedBN() {
      auto bn = gum::BayesNet< double >::fastPrototype("E<-A{chaud|froid}->B<-C<-D->A<-F;E<-D->B");
      gum::Instantiation Iobs;
      Iobs.add(bn.variable("A"));
      Iobs.chgVal("A", "froid");
      gum::Instantiation Iint;
      Iint.add(bn.variable("C"));
      Iint.chgVal("C", 1);

      auto bn2 = bn.contextualize(Iobs, Iint);

      CHECK((bn2.size()) == (gum::Size(6)));
      CHECK((bn2.sizeArcs()) == (gum::Size(4)));
    }

    void testcontextulizedBN2() {
      auto bn = gum::BayesNet< double >::fastPrototype("E<-A{chaud|froid}->B<-C<-D->A<-F;E<-D->B");
      gum::Instantiation Iobs;
      Iobs.add(bn.variable("D"));
      Iobs.chgVal("D", 0);
      gum::Instantiation Iint;
      Iint.add(bn.variable("A"));
      Iint.chgVal("A", 1);

      auto bn2 = bn.contextualize(Iobs, Iint);

      CHECK((bn2.size()) == (gum::Size(6)));
      CHECK((bn2.sizeArcs()) == (gum::Size(1)));
    }
  };

  GUM_TEST_ACTIF(Constructor)
  GUM_TEST_ACTIF(CopyConstructor)
  GUM_TEST_ACTIF(CopyOperator)
  GUM_TEST_ACTIF(Insertion_1)
  GUM_TEST_ACTIF(Insertion_2)
  GUM_TEST_ACTIF(Iterations)
  GUM_TEST_ACTIF(ArcInsertion)
  GUM_TEST_ACTIF(EraseVar)
  GUM_TEST_ACTIF(EraseArc)
  GUM_TEST_ACTIF(StringAccessors)
  GUM_TEST_ACTIF(Iterator)
  GUM_TEST_ACTIF(RandomlyFilled)
  GUM_TEST_ACTIF(MoralGraph)
  GUM_TEST_ACTIF(TopologicalOrder)
  GUM_TEST_ACTIF(cpt)
  GUM_TEST_ACTIF(CPTCoherencyVarRemoval)
  GUM_TEST_ACTIF(CPTCoherencyArcRemoval)
  GUM_TEST_ACTIF(StreamOperator)
  GUM_TEST_ACTIF(AccessorByName)
  GUM_TEST_ACTIF(CopyAndEqualityOperators)
  GUM_TEST_ACTIF(AggregatorNodes)
  GUM_TEST_ACTIF(JointProbability)
  GUM_TEST_ACTIF(ArcReversal)
  GUM_TEST_ACTIF(ChangeLabelOfVariable)
  GUM_TEST_ACTIF(ShortCutAddFastVar)
  GUM_TEST_ACTIF(SomeFunctions)
  GUM_TEST_ACTIF(MinimalCondSet)
  GUM_TEST_ACTIF(MinimalCondSet2)
  GUM_TEST_ACTIF(FastPrototype)
  GUM_TEST_ACTIF(FastPrototypeVarType)
  GUM_TEST_ACTIF(NonRegressionCPTinReverseArc)
  GUM_TEST_ACTIF(ClearBN)
  GUM_TEST_ACTIF(ExistsArc)
  GUM_TEST_ACTIF(Check)
  GUM_TEST_ACTIF(Threads)
  GUM_TEST_ACTIF(operatorEqual)
  GUM_TEST_ACTIF(contextulizedBN)
  GUM_TEST_ACTIF(contextulizedBN2)
}   // namespace gum_tests
