/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


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

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {
  class [[maybe_unused]] BayesNetTestSuite: public CxxTest::TestSuite {
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

    static gum::UndiGraph getRealMoralGraph(const gum::BayesNet< double >&  bn,
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

    void setUp() override {
      var1 = new gum::LabelizedVariable("var1", "1", 2);
      var2 = new gum::LabelizedVariable("var2", "2", 2);
      var3 = new gum::LabelizedVariable("var3", "3", 2);
      var4 = new gum::LabelizedVariable("var4", "4", 2);
      var5 = new gum::LabelizedVariable("var5", "(gum::Size) 3", 3);
    }

    void tearDown() override {
      delete var1;
      delete var2;
      delete var3;
      delete var4;
      delete var5;
    }

    GUM_ACTIVE_TEST(Constructor) {
      gum::BayesNet< double >* topology = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(topology = new gum::BayesNet< double >())
      TS_ASSERT_EQUALS(topology->size(), (gum::Idx)0)
      TS_ASSERT_EQUALS(topology->sizeArcs(), (gum::Idx)0)
      TS_ASSERT_EQUALS(topology->dim(), (gum::Idx)0)

      TS_ASSERT_THROWS(topology->addArc(1, 2), const gum::InvalidNode&)
      gum::List< gum::NodeId > idList;
      TS_GUM_ASSERT_THROWS_NOTHING(fill(*topology, idList))

      TS_ASSERT_THROWS(topology->add(*var1), const gum::DuplicateLabel&)

      TS_ASSERT_EQUALS(topology->toString(),
                       "BN{nodes: 5, arcs: 6, domainSize: 48, "
                       "dim: 24, mem: 320o}")
      TS_ASSERT_EQUALS(topology->size(), (gum::Idx)5)
      TS_ASSERT_EQUALS(topology->sizeArcs(), (gum::Idx)6)
      TS_ASSERT_EQUALS(topology->dim(), (gum::Idx)24)

      TS_GUM_ASSERT_THROWS_NOTHING(delete topology)
    }

    GUM_ACTIVE_TEST(CopyConstructor) {
      gum::BayesNet< double >  source;
      gum::List< gum::NodeId > idList;
      TS_GUM_ASSERT_THROWS_NOTHING(fill(source, idList))

      gum::BayesNet< double >* copy = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(copy = new gum::BayesNet< double >(source))

      TS_ASSERT_EQUALS(source.dag().size(), copy->dag().size())
      TS_ASSERT_EQUALS(source.dag().sizeArcs(), copy->dag().sizeArcs())

      for (const gum::DAG dag = source.dag(); const auto node: dag.nodes()) {
        TS_ASSERT(copy->dag().exists(node))

        const gum::DiscreteVariable& srcVar = source.variable(node);
        const gum::DiscreteVariable& cpVar  = copy->variable(node);
        TS_ASSERT_EQUALS(srcVar.name(), cpVar.name())

        if (srcVar.domainSize() == cpVar.domainSize()) {
          for (gum::Idx i = 0; i < srcVar.domainSize(); i++) {
            TS_ASSERT_EQUALS(srcVar.label(i), cpVar.label(i))
          }
        } else {
          TS_ASSERT(false)
        }

        for (const auto parent: source.dag().parents(node)) {
          TS_ASSERT(copy->dag().existsArc(gum::Arc(parent, node)))
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

          TS_ASSERT_EQUALS(cpCPT[cpInst], srcCPT[srcInst])
        }
      }

      if (copy) { delete copy; }
    }

    GUM_ACTIVE_TEST(CopyOperator) {
      gum::BayesNet< double >  source;
      gum::BayesNet< double >  copy;
      gum::List< gum::NodeId > idList;
      TS_GUM_ASSERT_THROWS_NOTHING(fill(source, idList))

      TS_GUM_ASSERT_THROWS_NOTHING(copy = source)

      TS_ASSERT_EQUALS(source.dag().size(), copy.dag().size())
      TS_ASSERT_EQUALS(source.dag().sizeArcs(), copy.dag().sizeArcs())

      for (const gum::DAG dag = source.dag(); const auto node: dag.nodes()) {
        TS_ASSERT(copy.dag().exists(node))

        const gum::DiscreteVariable& srcVar = source.variable(node);
        const gum::DiscreteVariable& cpVar  = copy.variable(node);
        TS_ASSERT_EQUALS(srcVar.name(), cpVar.name())

        if (srcVar.domainSize() == cpVar.domainSize()) {
          for (gum::Idx i = 0; i < srcVar.domainSize(); i++) {
            TS_ASSERT_EQUALS(srcVar.label(i), cpVar.label(i))
          }
        } else {
          TS_ASSERT(false)
        }

        for (const auto parent: source.dag().parents(node)) {
          TS_ASSERT(copy.dag().existsArc(gum::Arc(parent, node)))
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

          TS_ASSERT_EQUALS(cpCPT[cpInst], srcCPT[srcInst])
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

        TS_ASSERT_EQUALS(c_str.str(), s_str.str())
      }
    }

    GUM_ACTIVE_TEST(Insertion_1) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var1)))
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var2)))
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var3)))
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var4)))
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var5)))

      TS_ASSERT_EQUALS(bn.size(), (gum::Size)5)
      TS_ASSERT_EQUALS(bn.dag().size(), (gum::Size)5)

      // Test for uniqueness of the ids

      for (gum::Idx i = 0; i < idList.size() - 1; i++) {
        for (gum::Idx j = i + 1; j < idList.size(); j++) {
          if (idList[i] != idList[j]) {
            TS_ASSERT(true)
          } else {
            TS_ASSERT(false)
          }
        }
      }

      gum::LabelizedVariable const* varPtr = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(varPtr = (gum::LabelizedVariable*)&bn.variable(idList[0]))
      TS_ASSERT_EQUALS(*varPtr, *var1)

      TS_ASSERT_EQUALS(*((gum::LabelizedVariable*)&bn.variable(idList[0])), *var1)
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable*)&bn.variable(idList[1])), *var2)
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable*)&bn.variable(idList[2])), *var3)
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable*)&bn.variable(idList[3])), *var4)
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable*)&bn.variable(idList[4])), *var5)
    }

    GUM_ACTIVE_TEST(Insertion_2) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var1)))
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var2)))
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var3)))
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var4)))
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(bn.add(*var5)))

      TS_ASSERT_EQUALS(bn.size(), (gum::Size)5)
      TS_ASSERT_EQUALS(bn.dag().size(), (gum::Size)5)

      gum::LabelizedVariable const* varPtr = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(varPtr = (gum::LabelizedVariable*)&bn.variable(idList[0]))
      TS_ASSERT_EQUALS(*varPtr, *var1)

      TS_ASSERT_EQUALS(*((gum::LabelizedVariable*)&bn.variable(idList[0])), *var1)
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable*)&bn.variable(idList[1])), *var2)
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable*)&bn.variable(idList[2])), *var3)
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable*)&bn.variable(idList[3])), *var4)
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable*)&bn.variable(idList[4])), *var5)
    }

    GUM_ACTIVE_TEST(Iterations) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;
      TS_GUM_ASSERT_THROWS_NOTHING(fill(bn, idList))

      auto cpt = (gum::Size)0;

      for (const auto node: bn.nodes()) {
        GUM_UNUSED(node);
        cpt++;
      }

      TS_ASSERT_EQUALS(cpt, bn.size())

      cpt = (gum::Size)0;
      for (const auto& arc: bn.arcs()) {
        GUM_UNUSED(arc);
        cpt++;
      }

      TS_ASSERT_EQUALS(cpt, bn.sizeArcs())
    }

    GUM_ACTIVE_TEST(ArcInsertion) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      idList.insert(bn.add(*var1));
      idList.insert(bn.add(*var2));
      idList.insert(bn.add(*var3));
      idList.insert(bn.add(*var4));
      idList.insert(bn.add(*var5));

      TS_GUM_ASSERT_THROWS_NOTHING(bn.addArc(idList[0], idList[2]))
      TS_GUM_ASSERT_THROWS_NOTHING(bn.addArc(idList[2], idList[4]))
      TS_GUM_ASSERT_THROWS_NOTHING(bn.addArc(idList[1], idList[3]))
      TS_GUM_ASSERT_THROWS_NOTHING(bn.addArc(idList[0], idList[3]))
      TS_GUM_ASSERT_THROWS_NOTHING(bn.addArc(idList[3], idList[4]))
      TS_GUM_ASSERT_THROWS_NOTHING(bn.addArc(idList[1], idList[4]))

      TS_ASSERT_THROWS(bn.addArc(idList[3], idList[4]), const gum::DuplicateElement&)

      TS_ASSERT_EQUALS(bn.dag().sizeArcs(), (gum::Size)6)
    }

    GUM_ACTIVE_TEST(EraseVar) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      TS_ASSERT(bn.empty())
      TS_ASSERT(bn.dag().emptyArcs())

      fill(bn, idList);

      TS_ASSERT(!bn.empty())
      TS_ASSERT(!bn.dag().emptyArcs())

      TS_ASSERT_EQUALS(bn.size(), (gum::Size)5)
      TS_ASSERT_EQUALS(bn.dag().size(), (gum::Size)5)
      TS_ASSERT_EQUALS(bn.dag().sizeArcs(), (gum::Size)6)

      bn.erase(idList[0]);

      for (const auto i: idList) {
        bn.erase(i);
      }

      TS_ASSERT(bn.empty())

      TS_ASSERT(bn.dag().emptyArcs())

      TS_ASSERT_EQUALS(bn.size(), (gum::Size)0)
      TS_ASSERT_EQUALS(bn.dag().size(), (gum::Size)0)
      TS_ASSERT_EQUALS(bn.dag().sizeArcs(), (gum::Size)0)

      idList.clear();
      TS_GUM_ASSERT_THROWS_NOTHING(fill(bn, idList))

      TS_ASSERT(!bn.empty())
      TS_ASSERT(!bn.dag().emptyArcs())

      TS_ASSERT_EQUALS(bn.size(), (gum::Size)5)
      TS_ASSERT_EQUALS(bn.dag().size(), (gum::Size)5)
      TS_ASSERT_EQUALS(bn.dag().sizeArcs(), (gum::Size)6)
    }

    GUM_ACTIVE_TEST(EraseArc) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      TS_ASSERT(bn.empty())
      TS_ASSERT(bn.dag().emptyArcs())

      fill(bn, idList);

      TS_ASSERT(!bn.empty())
      TS_ASSERT(!bn.dag().emptyArcs())

      TS_ASSERT_EQUALS(bn.size(), (gum::Size)5)
      TS_ASSERT_EQUALS(bn.dag().size(), (gum::Size)5)
      TS_ASSERT_EQUALS(bn.dag().sizeArcs(), (gum::Size)6)

      TS_GUM_ASSERT_THROWS_NOTHING(bn.eraseArc(gum::Arc(idList[0], idList[2])))
      TS_GUM_ASSERT_THROWS_NOTHING(bn.eraseArc(gum::Arc(idList[2], idList[4])))
      TS_GUM_ASSERT_THROWS_NOTHING(bn.eraseArc(gum::Arc(idList[1], idList[3])))
      TS_GUM_ASSERT_THROWS_NOTHING(bn.eraseArc(gum::Arc(idList[0], idList[3])))
      TS_GUM_ASSERT_THROWS_NOTHING(bn.eraseArc(gum::Arc(idList[3], idList[4])))
      TS_GUM_ASSERT_THROWS_NOTHING(bn.eraseArc(gum::Arc(idList[1], idList[4])))

      TS_ASSERT(!bn.empty())
      TS_ASSERT(bn.dag().emptyArcs())
    }

    GUM_ACTIVE_TEST(StringAccessors) {
      try {
        gum::BayesNet< double > bn;
        for (const auto& x: {"A", "B", "C"}) {
          bn.add(gum::LabelizedVariable(x, x, 2));
        }
        for (const auto& [first, second]: {std::make_pair("A", "C"), std::make_pair("B", "C")}) {
          bn.addArc(first, second);
        }
        TS_ASSERT_THROWS(bn.addArc("A", "C"), const gum::DuplicateElement&)

        TS_ASSERT_EQUALS(bn.toString(),
                         "BN{nodes: 3, arcs: 2, domainSize: 8, "
                         "dim: 6, mem: 96o}")

        bn.cpt("A").fillWith(1.0f).normalize();
        bn.generateCPT("B");
        bn.generateCPT("C");
        TS_ASSERT_THROWS(bn.cpt("XXX"), const gum::NotFound&)

        bn.reverseArc("A", "C");
        TS_ASSERT_EQUALS(bn.toString(),
                         "BN{nodes: 3, arcs: 3, domainSize: 8, "
                         "dim: 7, mem: 112o}")

        TS_ASSERT_THROWS(bn.reverseArc("A", "C"), const gum::InvalidArc&)
        TS_ASSERT_THROWS(bn.reverseArc("A", "C"), const gum::GraphError&)
        TS_ASSERT_THROWS(bn.reverseArc("A", "X"), const gum::NotFound&)

        bn.erase("A");
        TS_ASSERT_EQUALS(bn.toString(),
                         "BN{nodes: 2, arcs: 1, domainSize: 4, "
                         "dim: 3, mem: 48o}")

        TS_ASSERT_THROWS(bn.erase("A"), const gum::NotFound&)

        bn.eraseArc("B", "C");
        TS_ASSERT_EQUALS(bn.toString(),
                         "BN{nodes: 2, arcs: 0, domainSize: 4, "
                         "dim: 2, mem: 32o}")

        TS_ASSERT_THROWS(bn.eraseArc("B", "C"), const gum::NotFound&)
      } catch (const gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(Iterator) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      for (const auto node: bn.nodes()) {
        TS_ASSERT(idList.exists(node))
      }
    }

    GUM_ACTIVE_TEST(RandomlyFilled) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);

      for (const auto node: bn.nodes()) {
        std::stringstream s1;
        std::stringstream s2;
        s1 << bn.cpt(node);

        bn.generateCPTs();
        s2 << bn.cpt(node);
        TS_ASSERT_DIFFERS(s1.str(), s2.str())
      }
    }

    GUM_ACTIVE_TEST(MoralGraph) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);

      gum::UndiGraph graph;
      TS_GUM_ASSERT_THROWS_NOTHING(graph = bn.moralGraph())
      TS_ASSERT_EQUALS(graph, getRealMoralGraph(bn, idList))
    }

    GUM_ACTIVE_TEST(TopologicalOrder) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);

      gum::Sequence< gum::NodeId > topoOrder;
      TS_GUM_ASSERT_THROWS_NOTHING(topoOrder = bn.topologicalOrder())

      TS_ASSERT_EQUALS(topoOrder.size(), (gum::Size)5)
    }

    GUM_ACTIVE_TEST(cpt) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);
      TS_ASSERT_EQUALS(bn.cpt(idList[0]).domainSize(), (gum::Size)2)
      TS_ASSERT_EQUALS(bn.cpt(idList[1]).domainSize(), (gum::Size)2)
      TS_ASSERT_EQUALS(bn.cpt(idList[2]).domainSize(), (gum::Size)4)
      TS_ASSERT_EQUALS(bn.cpt(idList[3]).domainSize(), (gum::Size)8)
      TS_ASSERT_EQUALS(bn.cpt(idList[4]).domainSize(), (gum::Size)24)
    }

    GUM_ACTIVE_TEST(CPTCoherencyVarRemoval) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);

      TS_ASSERT_EQUALS(bn.dag().parents(idList[3]).size(), (gum::Size)2)
      TS_ASSERT(bn.dag().existsArc(idList[0], idList[3]))
      TS_ASSERT(bn.dag().existsArc(idList[1], idList[3]))

      gum::Size dmnSize_1
          = bn.variable(idList[0]).domainSize() * bn.variable(idList[1]).domainSize();
      dmnSize_1 *= bn.variable(idList[3]).domainSize();
      TS_ASSERT_EQUALS(bn.cpt(idList[3]).domainSize(), dmnSize_1)

      TS_ASSERT(bn.cpt(idList[3]).contains(bn.variable(idList[0])))
      TS_ASSERT(bn.cpt(idList[3]).contains(bn.variable(idList[1])))
      TS_ASSERT(bn.cpt(idList[3]).contains(bn.variable(idList[3])))

      TS_GUM_ASSERT_THROWS_NOTHING(bn.erase(idList[1]))

      TS_ASSERT_EQUALS(bn.dag().parents(idList[3]).size(), (gum::Size)1)
      TS_ASSERT(bn.dag().existsArc(idList[0], idList[3]))
      TS_ASSERT(!bn.dag().existsArc(idList[1], idList[3]))

      gum::Size dmnSize_2 = bn.variable(idList[0]).domainSize();
      dmnSize_2 *= bn.variable(idList[3]).domainSize();
      TS_ASSERT_EQUALS(bn.cpt(idList[3]).domainSize(), dmnSize_2)

      TS_ASSERT(bn.cpt(idList[3]).contains(bn.variable(idList[0])))
      TS_ASSERT_THROWS_ANYTHING(bn.cpt(idList[3]).contains(bn.variable(idList[1])))
      TS_ASSERT(bn.cpt(idList[3]).contains(bn.variable(idList[3])))
    }

    GUM_ACTIVE_TEST(CPTCoherencyArcRemoval) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);

      TS_ASSERT_EQUALS(bn.dag().parents(idList[3]).size(), (gum::Size)2)
      TS_ASSERT(bn.dag().existsArc(idList[0], idList[3]))
      TS_ASSERT(bn.dag().existsArc(idList[1], idList[3]))

      gum::Size dmnSize_1
          = bn.variable(idList[0]).domainSize() * bn.variable(idList[1]).domainSize();
      dmnSize_1 *= bn.variable(idList[3]).domainSize();
      TS_ASSERT_EQUALS(bn.cpt(idList[3]).domainSize(), dmnSize_1)

      TS_ASSERT(bn.cpt(idList[3]).contains(bn.variable(idList[0])))
      TS_ASSERT(bn.cpt(idList[3]).contains(bn.variable(idList[1])))
      TS_ASSERT(bn.cpt(idList[3]).contains(bn.variable(idList[3])))

      TS_GUM_ASSERT_THROWS_NOTHING(bn.eraseArc(gum::Arc(idList[1], idList[3])))

      TS_ASSERT_EQUALS(bn.dag().parents(idList[3]).size(), (gum::Size)1)
      TS_ASSERT(bn.dag().existsArc(idList[0], idList[3]))
      TS_ASSERT(!bn.dag().existsArc(idList[1], idList[3]))

      gum::Size dmnSize_2 = bn.variable(idList[0]).domainSize();
      dmnSize_2 *= bn.variable(idList[3]).domainSize();
      TS_ASSERT_EQUALS(bn.cpt(idList[3]).domainSize(), dmnSize_2)

      TS_ASSERT(bn.cpt(idList[3]).contains(bn.variable(idList[0])))
      TS_ASSERT(!bn.cpt(idList[3]).contains(bn.variable(idList[1])))
      TS_ASSERT(bn.cpt(idList[3]).contains(bn.variable(idList[3])))
    }

    GUM_ACTIVE_TEST(StreamOperator) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);
      std::stringstream sBuff;
      TS_GUM_ASSERT_THROWS_NOTHING(sBuff << bn)
    }

    GUM_ACTIVE_TEST(AccessorByName) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);

      for (const auto node: bn.nodes()) {
        TS_ASSERT_EQUALS(bn.idFromName(bn.variable(node).name()), node)
        TS_ASSERT_EQUALS(&bn.variableFromName(bn.variable(node).name()), &bn.variable(node))
        TS_ASSERT_EQUALS(&bn.variableFromName(bn.variable(node).name()), &bn.variable(node))
      }

      TS_ASSERT_THROWS(bn.idFromName("choucroute"), const gum::NotFound&)

      TS_ASSERT_THROWS(bn.variableFromName("choucroute"), const gum::NotFound&)

      TS_GUM_ASSERT_THROWS_NOTHING(bn.idFromName("var1"))
      bn.erase(bn.idFromName("var1"));
      TS_ASSERT_THROWS(bn.idFromName("var1"), const gum::NotFound&)
    }

    GUM_ACTIVE_TEST(CopyAndEqualityOperators) {
      auto                                   bn_1 = new gum::BayesNet< double >();
      gum::SimpleBayesNetGenerator< double > generator(20, 30, 4);
      generator.generateBN(*bn_1);

      auto bn_2 = new gum::BayesNet< double >();

      generator.generateBN(*bn_2);

      gum::BayesNet< double > bn_cpy_1 = *bn_1;
      gum::BayesNet< double > bn_cpy_2 = *bn_2;

      TS_ASSERT_EQUALS(bn_cpy_1, bn_cpy_1)
      TS_ASSERT_EQUALS(bn_cpy_1, (*bn_1))
      TS_ASSERT_EQUALS(bn_cpy_2, bn_cpy_2)
      TS_ASSERT_EQUALS(bn_cpy_2, (*bn_2))
      TS_ASSERT_DIFFERS((*bn_1), (*bn_2))
      TS_ASSERT_DIFFERS(bn_cpy_1, bn_cpy_2)

      TS_ASSERT_EQUALS(bn_cpy_1, bn_cpy_1)
      TS_ASSERT_EQUALS(bn_cpy_1, (*bn_1))
      TS_ASSERT_EQUALS(bn_cpy_2, bn_cpy_2)
      TS_ASSERT_EQUALS(bn_cpy_2, (*bn_2))
      TS_ASSERT_DIFFERS((*bn_1), (*bn_2))
      TS_ASSERT_DIFFERS(bn_cpy_1, bn_cpy_2)

      gum::BayesNet< double > cpy = *bn_1;
      TS_ASSERT_EQUALS(cpy, (*bn_1))
      TS_ASSERT_DIFFERS(cpy, (*bn_2))

      TS_GUM_ASSERT_THROWS_NOTHING(cpy = *bn_2)

      TS_ASSERT_EQUALS(cpy, (*bn_2))

      TS_ASSERT_EQUALS(cpy, (*bn_2))
      TS_ASSERT_DIFFERS(cpy, (*bn_1))
      TS_ASSERT_DIFFERS(cpy, (*bn_1))

      TS_ASSERT_THROWS_NOTHING(cpy = cpy;)

      TS_ASSERT_EQUALS(cpy, (*bn_2))

      TS_ASSERT_EQUALS(cpy, (*bn_2))
      TS_ASSERT_DIFFERS(cpy, (*bn_1))
      TS_ASSERT_DIFFERS(cpy, (*bn_1))

      std::string s1 = cpy.toString();
      delete bn_2;
      delete bn_1;
      std::string s2 = cpy.toString();
      TS_ASSERT_EQUALS(s1, s2)
    }

    GUM_ACTIVE_TEST(AggregatorNodes) {
      {
        gum::BayesNet< double > bn;
        gum::Idx                i1 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(i1 = bn.addOR(*var1))
        TS_ASSERT_THROWS(bn.addOR(*var5), const gum::SizeError&)

        bn.addArc(bn.add(*var3), i1);
        bn.addArc(bn.add(*var4), i1);
        bn.addArc(bn.add(*var5), i1);
      }
      {
        gum::BayesNet< double > bn;
        gum::Idx                i2 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(i2 = bn.addAND(*var2))
        TS_ASSERT_THROWS(bn.addAND(*var5), const gum::SizeError&)

        bn.addArc(bn.add(*var3), i2);
        bn.addArc(bn.add(*var4), i2);
        bn.addArc(bn.add(*var5), i2);
      }
    }

    GUM_ACTIVE_TEST(JointProbability) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;

      fill(bn, idList);

      auto i = bn.completeInstantiation();
      TS_ASSERT_EQUALS(i.nbrDim(), bn.size())

      TS_ASSERT_EQUALS(i.domainSize(), (gum::Size)(2 * 2 * 2 * 2 * 3))

      double a = 0.0;
      TS_ASSERT_THROWS_NOTHING(a = bn.jointProbability(i))

      TS_ASSERT_DELTA(a, 0.00072, 1e-4)

      gum::Instantiation j;
      for (auto n: bn.nodes()) {
        TS_ASSERT_THROWS(bn.jointProbability(j), const gum::InvalidArgument&)
        j.add(bn.variable(n));
      }
      double b = 0.0;
      TS_GUM_ASSERT_THROWS_NOTHING(b = bn.jointProbability(j))

      TS_ASSERT_EQUALS(a, b)
    }

    GUM_ACTIVE_TEST(ArcReversal) {
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
      TS_ASSERT_THROWS(bn.reverseArc(gum::Arc(3, 4)), const gum::InvalidArc&)
      TS_ASSERT_THROWS(bn.reverseArc(gum::Arc(3, 5)), const gum::InvalidArc&)
      TS_ASSERT_THROWS(bn.reverseArc(gum::Arc(2, 4)), const gum::InvalidArc&)

      unsigned int j;
      for (j = 0, i.setFirst(); !i.end(); i.inc(), ++j) {
        TS_ASSERT_DELTA(bn.jointProbability(i), joint[j], TS_GUM_SMALL_ERROR)
      }
    }

    GUM_ACTIVE_TEST(ChangeLabelOfVariable) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;
      fill(bn, idList);

      TS_ASSERT_EQUALS(bn.variable(0).toString(), "var1:Labelized({0|1})")
      TS_ASSERT_THROWS_NOTHING(
          dynamic_cast< const gum::LabelizedVariable& >(bn.variable(0)).changeLabel(0, "x"))
      TS_ASSERT_EQUALS(bn.variable(0).toString(), "var1:Labelized({x|1})")
    }

    GUM_ACTIVE_TEST(ShortCutAddFastVar) {
      gum::BayesNet< double > bn;

      gum::NodeId i1;
      gum::NodeId i2;
      gum::NodeId i3;

      TS_ASSERT_THROWS_NOTHING(i1 = bn.add("A", 2))
      TS_ASSERT_THROWS_NOTHING(i2 = bn.add("B", 3))
      TS_ASSERT_EQUALS(i1, gum::NodeId(0))
      TS_ASSERT_EQUALS(i2, gum::NodeId(1))

      TS_ASSERT_THROWS(i3 = bn.add("A", 5), const gum::DuplicateLabel&)
      // the variable "C",1 can be created but the BN does not allow to add such a variable
      TS_ASSERT_THROWS(i3 = bn.add("C", 1), const gum::OperationNotAllowed&)
      GUM_UNUSED(i3);

      TS_ASSERT_THROWS_NOTHING(bn.addArc(i1, i2))
      TS_ASSERT_EQUALS(bn.log10DomainSize(), std::log10(2.0 * 3.0))

      TS_ASSERT_THROWS_NOTHING(bn.add("X{top|middle|bottom}"))
      TS_ASSERT_EQUALS(bn.variable("X").toString(), "X:Labelized({top|middle|bottom})")

      // a mono-label with default 4 is impossible
      TS_ASSERT_THROWS(bn.add("Y[1,1]", 4), const gum::ArgumentError&)

      // a mono-label with default 1 is possible but can not be integrated in the model
      TS_ASSERT_THROWS(bn.add("Y[1,1]", 1), const gum::OperationNotAllowed&)
    }

    GUM_ACTIVE_TEST(SomeFunctions) {
      gum::BayesNet< double >  bn;
      gum::List< gum::NodeId > idList;
      TS_GUM_ASSERT_THROWS_NOTHING(fill(bn, idList))

      TS_ASSERT_EQUALS(bn.maxVarDomainSize(), (gum::Size)3)
      TS_ASSERT_EQUALS(bn.minParam(), 0.0f)
      TS_ASSERT_EQUALS(bn.maxParam(), 1.0f)
      TS_ASSERT_EQUALS(bn.minNonZeroParam(), 0.1f)
      TS_ASSERT_EQUALS(bn.maxNonOneParam(), 0.9f)
    }

    GUM_ACTIVE_TEST(MinimalCondSet) {
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

      TS_ASSERT_EQUALS(bn.minimalCondSet(c, {a, b, c, d, e, f, g, h}), gum::NodeSet({c}))
      TS_ASSERT_EQUALS(bn.minimalCondSet(c, {a, b, d, e, f, g, h}), gum::NodeSet({a, b, e, h}))
      TS_ASSERT_EQUALS(bn.minimalCondSet(c, {e, f, g}), gum::NodeSet({e, f}))
      TS_ASSERT_EQUALS(bn.minimalCondSet(c, {b, e, f, g}), gum::NodeSet({e, b}))
      TS_ASSERT_EQUALS(bn.minimalCondSet(c, {a, e, f, g}), gum::NodeSet({a, e, f}))

      // for set of targets
      TS_ASSERT_EQUALS(bn.minimalCondSet({e, d}, {a, b, c, d, e, f, g, h}), gum::NodeSet({d, e}))
      TS_ASSERT_EQUALS(bn.minimalCondSet({e, d}, {a, b, c, d, f, g, h}), gum::NodeSet({c, d, h, f}))
      TS_ASSERT_EQUALS(bn.minimalCondSet({e, d}, {a, b, c, f, g, h}), gum::NodeSet({b, c, f, h}))
    }

    GUM_ACTIVE_TEST(MinimalCondSet2) {
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

      TS_ASSERT_EQUALS(bn.minimalCondSet(4, gum::NodeSet({0, 1, 2, 3, 5, 6})),
                       gum::NodeSet({0, 2, 3}))

      TS_ASSERT_EQUALS(bn.minimalCondSet(4, gum::NodeSet({0, 1, 5})), gum::NodeSet({0}))

      TS_ASSERT_EQUALS(bn.minimalCondSet(4, gum::NodeSet({0, 1, 3, 5})), gum::NodeSet({0, 1, 3}))
    }

    GUM_ACTIVE_TEST(FastPrototype) {
      try {
        auto bn = gum::BayesNet< float >::fastPrototype("a->b->c;a->c");
        TS_ASSERT_EQUALS(bn.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)3)
        TS_ASSERT_EQUALS(bn.dim(), gum::Size((2 - 1) + (2 * (2 - 1)) + (2 * 2 * (2 - 1))))

        bn = gum::BayesNet< float >::fastPrototype("a->b->c;a->c", 3);
        TS_ASSERT_EQUALS(bn.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)3)
        TS_ASSERT_EQUALS(bn.dim(), gum::Size((3 - 1) + (3 * (3 - 1)) + (3 * 3 * (3 - 1))))

        bn = gum::BayesNet< float >::fastPrototype("a->b[5]->c;a->c");
        TS_ASSERT_EQUALS(bn.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)3)
        TS_ASSERT_EQUALS(bn.dim(), gum::Size((2 - 1) + (2 * (5 - 1)) + (5 * 2 * (2 - 1))))

        bn = gum::BayesNet< float >::fastPrototype("a->b->c;a[1000]->c");
        TS_ASSERT_EQUALS(bn.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)3)
        TS_ASSERT_EQUALS(bn.dim(), gum::Size((2 - 1) + (2 * (2 - 1)) + (2 * 2 * (2 - 1))))

        TS_ASSERT_THROWS(bn = gum::BayesNet< float >::fastPrototype("a->b->c->a"),
                         const gum::InvalidDirectedCycle&)

        bn = gum::BayesNet< float >::fastPrototype("a{yes|maybe|no}->b->c;a->c");
        TS_ASSERT_EQUALS(bn.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)3)
        TS_ASSERT_EQUALS(bn.dim(), gum::Size((3 - 1) + (3 * (2 - 1)) + (3 * 2 * (2 - 1))))

        TS_ASSERT_THROWS(gum::BayesNet< float >::fastPrototype("a{yes}->b->c;a->c"),
                         const gum::InvalidArgument&)
        TS_ASSERT_THROWS(gum::BayesNet< float >::fastPrototype("a{yes|no|yes}->b->c;a->c"),
                         const gum::InvalidArgument&)

        bn = gum::BayesNet< float >::fastPrototype("a->b->c->d->e->f");
        TS_ASSERT_EQUALS(bn.size(), (gum::Size)6)
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)5)
        TS_ASSERT_EQUALS(bn.dim(), gum::Size(1 + 5 * 2))

        bn = gum::BayesNet< float >::fastPrototype("a<-b<-c<-d<-e<-f");
        TS_ASSERT_EQUALS(bn.size(), (gum::Size)6)
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)5)
        TS_ASSERT_EQUALS(bn.dim(), gum::Size(1 + 5 * 2))

        bn = gum::BayesNet< float >::fastPrototype("a<-b->c<-d->e<-f");
        TS_ASSERT_EQUALS(bn.size(), (gum::Size)6)
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)5)
        TS_ASSERT_EQUALS(bn.dim(), gum::Size(3 * 1 + 2 + 2 * 4))

        bn = gum::BayesNet< float >::fastPrototype("a->b<-c->d<-e->f");
        TS_ASSERT_EQUALS(bn.size(), (gum::Size)6)
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)5)
        TS_ASSERT_EQUALS(bn.dim(), gum::Size(3 * 1 + 2 + 2 * 4))

        bn = gum::BayesNet< float >::fastPrototype("a->b<-c;c->d<-e->f");
        TS_ASSERT_EQUALS(bn.size(), (gum::Size)6)
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)5)
        TS_ASSERT_EQUALS(bn.dim(), gum::Size(3 * 1 + 2 + 2 * 4))

        bn = gum::BayesNet< float >::fastPrototype("a->b<-c->d;d<-e->f");
        TS_ASSERT_EQUALS(bn.size(), (gum::Size)6)
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)5)
        TS_ASSERT_EQUALS(bn.dim(), gum::Size(3 * 1 + 2 + 2 * 4))

        bn = gum::BayesNet< float >::fastPrototype("a->b;b<-c;c->d;d<-e;e->f");
        TS_ASSERT_EQUALS(bn.size(), (gum::Size)6)
        TS_ASSERT_EQUALS(bn.sizeArcs(), (gum::Size)5)
        TS_ASSERT_EQUALS(bn.dim(), gum::Size(3 * 1 + 2 + 2 * 4))
      } catch (const gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(FastPrototypeVarType) {
      auto bn = gum::BayesNet< float >::fastPrototype("a");
      TS_ASSERT_EQUALS(bn.variable("a").toString(), "a:Range([0,1])")

      bn = gum::BayesNet< float >::fastPrototype("a[0,1]");
      TS_ASSERT_EQUALS(bn.variable("a").toString(), "a:Range([0,1])")

      TS_ASSERT_THROWS(gum::BayesNet< float >::fastPrototype("a[0,0]"), const gum::InvalidArgument&)
      TS_ASSERT_THROWS(gum::BayesNet< float >::fastPrototype("a[1,0]"), const gum::InvalidArgument&)
      TS_ASSERT_THROWS(gum::BayesNet< float >::fastPrototype("a[1,1]"), const gum::InvalidArgument&)

      bn = gum::BayesNet< float >::fastPrototype("a[5]");
      TS_ASSERT_EQUALS(bn.variable("a").toString(), "a:Range([0,4])")

      bn = gum::BayesNet< float >::fastPrototype("a[2,5]");
      TS_ASSERT_EQUALS(bn.variable("a").toString(), "a:Range([2,5])")

      bn = gum::BayesNet< float >::fastPrototype("a[-2,2]");
      TS_ASSERT_EQUALS(bn.variable("a").toString(), "a:Range([-2,2])")

      bn = gum::BayesNet< float >::fastPrototype("a[-0.4,0.1,0.5,3.14,10]");
      TS_ASSERT_EQUALS(bn.variable("a").toString(),
                       "a:Discretized(<[-0.4;0.1[,[0.1;0.5[,[0.5;3.14[,[3.14;10]>)")

      bn = gum::BayesNet< float >::fastPrototype(
          "a{1|4|6}->b{1|-4|6}->c{1|toto|6}->d{1.0|-4.0|6.0}->e{1|-4|6.0}->f{1.0|-4.0|+6.0}");
      TS_ASSERT_EQUALS(bn.variable("a").varType(), gum::VarType::INTEGER)
      TS_ASSERT_EQUALS(bn.variable("b").varType(), gum::VarType::INTEGER)
      TS_ASSERT_EQUALS(bn.variable("c").varType(), gum::VarType::LABELIZED)
      TS_ASSERT_EQUALS(bn.variable("d").varType(), gum::VarType::NUMERICAL)
      TS_ASSERT_EQUALS(bn.variable("e").varType(), gum::VarType::NUMERICAL)
      TS_ASSERT_EQUALS(bn.variable("f").varType(), gum::VarType::NUMERICAL)
    }

    GUM_ACTIVE_TEST(NonRegressionCPTinReverseArc) {
      const auto bn1 = gum::BayesNet< double >::fastPrototype("A->B->C");
      auto       bn2 = gum::BayesNet< double >(bn1);
      bn2.reverseArc("A", "B");
      TS_ASSERT_EQUALS(bn1.cpt("A").variable(0).name(), "A")
      TS_ASSERT_EQUALS(bn2.cpt("A").variable(0).name(), "A")
      TS_ASSERT_EQUALS(bn1.cpt("B").variable(0).name(), "B")
      TS_ASSERT_EQUALS(bn2.cpt("B").variable(0).name(), "B")
    }

    GUM_ACTIVE_TEST(ClearBN) {
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
        TS_ASSERT(bn_instance.empty())
      }
      {
        // with clear method
        auto bn_instance = gum::BayesNet< double >(bn);
        bn_instance.clear();
        TS_ASSERT(bn_instance.empty())
      }
    }

    GUM_ACTIVE_TEST(ExistsArc) {
      const auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D->E<-A<-G->F");

      TS_ASSERT(bn.existsArc(0, 1))
      TS_ASSERT(bn.existsArc("A", "B"))
      TS_ASSERT(!bn.existsArc(1, 0))
      TS_ASSERT(!bn.existsArc("B", "A"))
      TS_ASSERT(!bn.existsArc(0, 2))
      TS_ASSERT(!bn.existsArc("A", "C"))
    }

    GUM_ACTIVE_TEST(Check) {
      {
        const auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D->E<-A<-G->F");
        TS_ASSERT_EQUALS(bn.check().size(), gum::Size(0))

        bn.cpt("B").fillWith(-1);
        TS_ASSERT_EQUALS(bn.check().size(), gum::Size(2))
      }
      {
        const auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D->E<-A<-G->F");
        TS_ASSERT_EQUALS(bn.check().size(), gum::Size(0))

        bn.cpt("B").fillWith(0);
        TS_ASSERT_EQUALS(bn.check().size(), gum::Size(1))

        bn.cpt("C").fillWith(2);
        TS_ASSERT_EQUALS(bn.check().size(), gum::Size(3))
      }
      {
        const auto bn
            = gum::BayesNet< double >::fastPrototype("Y->X->T1;Z2->X;Z1->X;Z1->T1;Z1->Z3->T2");
        TS_ASSERT_EQUALS(bn.check().size(), gum::Size(0))

        bn.cpt("X").scale(12);
        TS_ASSERT_EQUALS(bn.check().size(), gum::Size(2))

        bn.cpt("Z1").scale(12);
        TS_ASSERT_EQUALS(bn.check().size(), gum::Size(4))
      }
    }

    GUM_ACTIVE_TEST(Threads) {
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

        TS_ASSERT_EQUALS(gum::Size(nbth), 20 * bn.maxVarDomainSize())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(operatorEqual) {
      auto bn  = gum::BayesNet< double >::fastPrototype("A->B->C");
      auto bn2 = gum::BayesNet< double >::fastPrototype("A{yes|no}->B{yes|no}->C{yes|no}");
      auto bn3 = gum::BayesNet< double >::fastPrototype("A->B->C");
      for (const auto i: bn.nodes()) {
        bn.cpt(i).fillWith(1);
        bn2.cpt(i).fillWith(1);
        bn3.cpt(i).fillWith(1);
      }
      TS_ASSERT_EQUALS(bn, bn);
      TS_ASSERT_DIFFERS(bn, bn2);
      TS_ASSERT_EQUALS(bn, bn3);
    }

    GUM_ACTIVE_TEST(contextulizedBN) {
      auto bn = gum::BayesNet< double >::fastPrototype("E<-A{chaud|froid}->B<-C<-D->A<-F;E<-D->B");
      gum::Instantiation Iobs;
      Iobs.add(bn.variable("A"));
      Iobs.chgVal("A", "froid");
      gum::Instantiation Iint;
      Iint.add(bn.variable("C"));
      Iint.chgVal("C", 1);

      auto bn2 = bn.contextualize(Iobs, Iint);

      TS_ASSERT_EQUALS(bn2.size(), gum::Size(6));
      TS_ASSERT_EQUALS(bn2.sizeArcs(), gum::Size(4))
    }

    GUM_ACTIVE_TEST(contextulizedBN2) {
      auto bn = gum::BayesNet< double >::fastPrototype("E<-A{chaud|froid}->B<-C<-D->A<-F;E<-D->B");
      gum::Instantiation Iobs;
      Iobs.add(bn.variable("D"));
      Iobs.chgVal("D", 0);
      gum::Instantiation Iint;
      Iint.add(bn.variable("A"));
      Iint.chgVal("A", 1);

      auto bn2 = bn.contextualize(Iobs, Iint);

      TS_ASSERT_EQUALS(bn2.size(), gum::Size(6));
      TS_ASSERT_EQUALS(bn2.sizeArcs(), gum::Size(1));
    }
  };
}   // namespace gum_tests
