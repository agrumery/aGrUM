/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 * (_at_AMU) info_at_agrum_dot_org
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


#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include "agrum/MN/MarkovNet.h"
#include "agrum/MN/inference/ShaferShenoyMNInference.h"

#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/labelizedVariable.h>
#include <agrum/tools/variables/rangeVariable.h>
#include <agrum/tools/variables/discretizedVariable.h>

// The graph used for the tests:
//          1   2_
//         / \ / /
//        3   4 /
//         \ / /
//          5_/
//

namespace gum_tests {
  class [[maybe_unused]] MarkovNetLegacyTestSuite: public CxxTest::TestSuite {
    private:
    void _fill(gum::MarkovNet< double >& mn) {
      for (const auto i: {11, 21, 31, 41}) {
        mn.add(std::to_string(i), 3);
      }
      mn.add(std::to_string(51), 7);
      mn.addFactor({"11", "31"}).fillWith({0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8});
      mn.addFactor({"11", "41"}).fillWith({0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0});
      mn.addFactor({"31", "51"}).fillWith(0.97);
      mn.addFactor({"21", "41", "51"}).fillWith(0.03);
    }

    public:
    void setUp() {}

    void tearDown() {}

    GUM_ACTIVE_TEST(SimpleConstructor){TS_GUM_ASSERT_THROWS_NOTHING({
      gum::MarkovNet< double > mn;
      GUM_UNUSED(mn);
    })};

    GUM_ACTIVE_TEST(Constructor) {
      gum::MarkovNet< double > mn;
      _fill(mn);

      TS_ASSERT_EQUALS(mn.size(), (gum::Idx)5)
      TS_ASSERT_EQUALS(mn.sizeEdges(), (gum::Idx)6)
      TS_ASSERT_EQUALS(mn.dim(), (gum::Idx)(3 * 3 + 3 * 3 + 3 * 7 + 3 * 3 * 7))
      TS_ASSERT_EQUALS(mn.toString(), "MRF{nodes: 5, edges: 6, domainSize: 567, dim: 102}")
      TS_ASSERT_EQUALS(mn.neighbours("41"), gum::NodeSet({0, 1, 4}))
      TS_ASSERT_EQUALS(mn.neighbours(3), gum::NodeSet({0, 1, 4}))

      TS_ASSERT_EQUALS(mn.variable(1).name(), "21")
      TS_ASSERT_EQUALS(mn.nodeId(mn.variable(2)), gum::NodeId(2))
      TS_ASSERT_EQUALS(mn.idFromName("31"), gum::NodeId(2))
      TS_ASSERT_EQUALS(mn.variableFromName("41").name(), "41")

      TS_ASSERT_EQUALS(mn.maxVarDomainSize(), (gum::Size)7)
      TS_ASSERT_EQUALS(mn.minParam(), 0.0)
      TS_ASSERT_EQUALS(mn.minNonZeroParam(), 0.03)
      TS_ASSERT_EQUALS(mn.maxParam(), 1.0)
      TS_ASSERT_EQUALS(mn.maxNonOneParam(), 0.97)
    }

    GUM_ACTIVE_TEST(CopyConstructor) {
      gum::MarkovNet< double > mn;
      _fill(mn);
      gum::MarkovNet< double > mn2(mn);
      TS_ASSERT_EQUALS(mn2.toString(), "MRF{nodes: 5, edges: 6, domainSize: 567, dim: 102}")

      TS_GUM_ASSERT_THROWS_NOTHING({
        for (const auto n: mn.nodes()) {
          TS_ASSERT_EQUALS(mn.variable(n).name(), mn2.variable(n).name())
          TS_ASSERT_EQUALS(mn.neighbours(n), mn2.neighbours(n))
        }
      });
    }

    GUM_ACTIVE_TEST(CopyOperator) {
      gum::MarkovNet< double > mn;
      gum::MarkovNet< double > mn2;
      _fill(mn);
      mn2 = mn;
      TS_ASSERT_EQUALS(mn2.toString(), "MRF{nodes: 5, edges: 6, domainSize: 567, dim: 102}")
      for (const auto n: mn.nodes()) {
        TS_ASSERT_EQUALS(mn.variable(n).name(), mn2.variable(n).name())
        TS_ASSERT_EQUALS(mn.neighbours(n), mn2.neighbours(n))
      }
    }

    GUM_ACTIVE_TEST(EqualityOperators) {
      gum::MarkovNet< double > mn1;
      _fill(mn1);
      gum::MarkovNet< double > mn2;
      TS_ASSERT_DIFFERS(mn1, mn2)
      _fill(mn2);
      TS_ASSERT_EQUALS(mn1, mn2)
      mn2.generateFactors();
      TS_ASSERT_DIFFERS(mn1, mn2)
    }

    GUM_ACTIVE_TEST(OrderInsertion) {
      gum::MarkovNet< double > mn;
      mn.add("V0", 2);
      mn.add("V1", 2);
      mn.add("V2", 2);
      mn.add("V3", 2);

      // V0 should be the first
      mn.addFactor({"V0", "V1"});
      TS_ASSERT_EQUALS(mn.factor({"V0", "V1"}).variable(0).name(), "V0")
      TS_ASSERT_EQUALS(mn.factor({"V1", "V0"}).variable(0).name(), "V0")

      // V2 should be the first
      mn.addFactor({"V2", "V1"});
      TS_ASSERT_EQUALS(mn.factor({"V2", "V1"}).variable(0).name(), "V2")
      TS_ASSERT_EQUALS(mn.factor({"V1", "V2"}).variable(0).name(), "V2")

      // 2 should be the first
      gum::NodeSet s1{2, 3};
      mn.addFactor(s1);
      TS_ASSERT_EQUALS(mn.factor(s1).variable(0).name(), mn.variable(2).name())

      // 1 should be the first
      gum::NodeSet s2{3, 1};
      mn.addFactor(s2);
      TS_ASSERT_EQUALS(mn.factor(s2).variable(0).name(), mn.variable(1).name())
    }

    GUM_ACTIVE_TEST(InsertionFromPotential) {
      gum::MarkovNet< double > mn;
      _fill(mn);
      TS_ASSERT_THROWS(mn.addFactor(gum::Potential< double >()),
                       const gum::InvalidArgument&)   // no empty factor
      TS_ASSERT_THROWS(mn.addFactor({"11", "31"}),
                       const gum::InvalidArgument&)   // already exists

      {
        gum::MarkovNet< double > mn1;
        _fill(mn1);
        gum::Potential< double > pot;
        pot.add(mn1.variable("11"));
        pot.add(mn1.variable("21"));
        pot.randomDistribution();
        TS_GUM_ASSERT_THROWS_NOTHING(mn1.addFactor(pot))
        TS_ASSERT_EQUALS(pot.toString(), mn1.factor({"11", "21"}).toString())
      }
      {
        gum::MarkovNet< double > mn1;
        _fill(mn1);
        gum::Potential< double > pot;
        pot.add(mn1.variable("21"));
        pot.add(mn1.variable("11"));
        pot.randomDistribution();
        TS_GUM_ASSERT_THROWS_NOTHING(mn1.addFactor(pot))

        // should be equal because no sorting by order of the vars in pot.
        TS_ASSERT_EQUALS(pot.toString(), mn1.factor({"11", "21"}).toString())

        // but the data should be the same
        gum::Instantiation I(pot);
        const auto&        factor = mn1.factor({"21", "11"});
        for (I.setFirst(); !I.end(); I.inc()) {
          TS_ASSERT_DELTA(pot.get(I), factor.get(I), 1e-10)
        }
      }
    }

    GUM_ACTIVE_TEST(Iterations) {
      gum::MarkovNet< double > mn;
      _fill(mn);
      gum::Size cpt = (gum::Size)0;

      for (const auto node: mn.nodes()) {
        GUM_UNUSED(node);
        cpt++;
      }

      TS_ASSERT_EQUALS(cpt, mn.size())

      cpt = (gum::Size)0;

      for (const auto& edg: mn.edges()) {
        GUM_UNUSED(edg);
        cpt++;
      }

      TS_ASSERT_EQUALS(cpt, mn.sizeEdges())
    }

    GUM_ACTIVE_TEST(EraseFactor) {
      gum::MarkovNet< double > mn;
      _fill(mn);
      TS_ASSERT_THROWS(mn.eraseFactor({12, 14}), const gum::InvalidArgument&)
      TS_GUM_ASSERT_THROWS_NOTHING(mn.eraseFactor({2, 4}))

      TS_ASSERT_EQUALS(mn.size(), (gum::Idx)5)
      TS_ASSERT_EQUALS(mn.sizeEdges(), (gum::Idx)5)
      TS_ASSERT_EQUALS(mn.dim(), (gum::Idx)(3 * 3 + 3 * 3 + 3 * 3 * 7))
      TS_ASSERT_EQUALS(mn.toString(), "MRF{nodes: 5, edges: 5, domainSize: 567, dim: 81}")
    }

    GUM_ACTIVE_TEST(EraseFactorWithNames) {
      gum::MarkovNet< double > mn;
      _fill(mn);
      TS_ASSERT_THROWS(mn.eraseFactor({"31", "21"}), const gum::InvalidArgument&)
      TS_GUM_ASSERT_THROWS_NOTHING(mn.eraseFactor({"31", "51"}))

      TS_ASSERT_EQUALS(mn.size(), (gum::Idx)5)
      TS_ASSERT_EQUALS(mn.sizeEdges(), (gum::Idx)5)
      TS_ASSERT_EQUALS(mn.dim(), (gum::Idx)(3 * 3 + 3 * 3 + 3 * 3 * 7))
      TS_ASSERT_EQUALS(mn.toString(), "MRF{nodes: 5, edges: 5, domainSize: 567, dim: 81}")
    }

    GUM_ACTIVE_TEST(Erase) {
      gum::MarkovNet< double > mn;
      _fill(mn);
      TS_ASSERT_THROWS(mn.erase(36), const gum::InvalidArgument&)
      TS_GUM_ASSERT_THROWS_NOTHING(mn.erase(3))

      TS_ASSERT_EQUALS(mn.size(), (gum::Idx)4)
      TS_ASSERT_EQUALS(mn.sizeEdges(), (gum::Idx)3)
      TS_ASSERT_EQUALS(mn.dim(), (gum::Idx)(3 * 3 + 3 * 7 + 3 * 7))
      TS_ASSERT_EQUALS(mn.toString(), "MRF{nodes: 4, edges: 3, domainSize: 189, dim: 51}")
    }

    GUM_ACTIVE_TEST(EraseWithNames) {
      gum::MarkovNet< double > mn;
      _fill(mn);
      TS_ASSERT_THROWS(mn.erase("36"), const gum::NotFound&)
      TS_GUM_ASSERT_THROWS_NOTHING(mn.erase("41"))

      TS_ASSERT_EQUALS(mn.size(), (gum::Idx)4)
      TS_ASSERT_EQUALS(mn.sizeEdges(), (gum::Idx)3)
      TS_ASSERT_EQUALS(mn.dim(), (gum::Idx)(3 * 3 + 3 * 7 + 3 * 7))
      TS_ASSERT_EQUALS(mn.toString(), "MRF{nodes: 4, edges: 3, domainSize: 189, dim: 51}")
    }

    GUM_ACTIVE_TEST(ToDot) {
      auto       mn = gum::MarkovNet< double >::fastPrototype("A--B--C;B--D;C--E;D--E--F");
      const auto s1 = mn.toDot();
      const auto s2 = mn.toDotAsFactorGraph();
      GUM_UNUSED(s1);
      GUM_UNUSED(s2);
    }

    GUM_ACTIVE_TEST(FromBN) {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D;C<-E->F<-G;F<-A");
      auto mn = gum::MarkovNet< double >::fromBN(bn);

      gum::Potential< double > pbn;
      pbn.fill(1);
      for (gum::NodeId nod: bn.nodes()) {
        TS_ASSERT_EQUALS(bn.variable(nod).toString(), mn.variable(nod).toString())

        pbn *= bn.cpt(nod);
      }

      gum::Potential< double > pmn;
      pmn.fill(1);
      for (const auto& key: mn.factors()) {
        TS_ASSERT_EQUALS(mn.factor(key.first), *key.second)
        pmn *= *key.second;
      }
      pmn.normalize();

      gum::NodeSet s;
      s.clear();
      s << 0 << 4 << 5 << 6;
      TS_GUM_ASSERT_THROWS_NOTHING(mn.factor(s))
      s.clear();
      s << 4 << 0 << 6 << 5;
      TS_GUM_ASSERT_THROWS_NOTHING(mn.factor(s))

      gum::Potential< double > ppmn(pbn);
      ppmn.fillWith(pmn);   // copy of pmn using pbn's variables
      auto diff = (pbn - ppmn).new_abs();
      TS_ASSERT_EQUALS(pbn.domainSize(), diff.domainSize())
      TS_ASSERT_LESS_THAN(diff.max(), 1e-10)

      TS_ASSERT_EQUALS(mn.graph(), bn.moralGraph())
    }

    GUM_ACTIVE_TEST(ExistsEdge) {
      auto mn = gum::MarkovNet< double >::fastPrototype("A--B--C;C--D;E--F--G");

      TS_ASSERT(mn.existsEdge(0, 1))
      TS_ASSERT(mn.existsEdge("A", "B"))
      TS_ASSERT(mn.existsEdge(1, 0))
      TS_ASSERT(mn.existsEdge("B", "A"))
      TS_ASSERT(mn.existsEdge(0, 2))
      TS_ASSERT(mn.existsEdge("A", "C"))
      TS_ASSERT(!mn.existsEdge(3, 7))
      TS_ASSERT(!mn.existsEdge("C", "G"))
    }

    GUM_ACTIVE_TEST(MinimalCondSet) {
      auto mn = gum::MarkovNet< double >::fastPrototype("A--B--C;C--D;E--F--G;B--E");

      TS_ASSERT_EQUALS(mn.minimalCondSet(0, {1, 2, 3, 4, 5, 6}), gum::NodeSet({1, 2}))
      TS_ASSERT_EQUALS(mn.minimalCondSet({0, 6}, {1, 2, 3, 4, 5}), gum::NodeSet({1, 2, 4, 5}))
      TS_ASSERT_EQUALS(mn.minimalCondSet(3, {0, 4, 5, 6}), gum::NodeSet({0, 4}))
    }

    GUM_ACTIVE_TEST(Independence) {
      auto mn = gum::MarkovNet< double >::fastPrototype("A--B--C;C--D;E--F--G;B--E;D--G;X");
      TS_ASSERT(mn.isIndependent("D", "X", {}))
      TS_ASSERT(!mn.isIndependent("D", "A", {"C"}))
      TS_ASSERT(mn.isIndependent("D", "A", {"C", "G"}))
      TS_ASSERT(!mn.isIndependent("G", "A", {"C", "F"}))
      TS_ASSERT(mn.isIndependent("G", "A", {"D", "E"}))
    }


    GUM_ACTIVE_TEST(FastPrototypeVarType) {
      auto mn = gum::MarkovNet< float >::fastPrototype(
         "a{1|4|6}--b{1|-4|6};c{1|toto|6}--d{1.0|-4.0|6.0};e{1|-4|6.0}--f{1.0|-4.0|+6.0}");
      TS_ASSERT_EQUALS(mn.variable("a").varType(), gum::VarType::Integer)
      TS_ASSERT_EQUALS(mn.variable("b").varType(), gum::VarType::Integer)
      TS_ASSERT_EQUALS(mn.variable("c").varType(), gum::VarType::Labelized)
      TS_ASSERT_EQUALS(mn.variable("d").varType(), gum::VarType::Numerical)
      TS_ASSERT_EQUALS(mn.variable("e").varType(), gum::VarType::Numerical)
      TS_ASSERT_EQUALS(mn.variable("f").varType(), gum::VarType::Numerical)
    }

    GUM_ACTIVE_TEST(MonoClique) {
      auto mn2 = gum::MarkovNet< float >::fastPrototype("A--B");
      TS_GUM_ASSERT_THROWS_NOTHING(mn2.factor({"A", "B"}))
    }

    GUM_ACTIVE_TEST(ShortCutAddFastVar) {
      gum::MarkovNet< double > mn;

      gum::NodeId i1, i2, i3;

      TS_ASSERT_THROWS_NOTHING(i1 = mn.add("A", 2))
      TS_ASSERT_THROWS_NOTHING(i2 = mn.add("B", 3))
      TS_ASSERT_EQUALS(i1, gum::NodeId(0))
      TS_ASSERT_EQUALS(i2, gum::NodeId(1))

      TS_ASSERT_THROWS(i3 = mn.add("A", 5), const gum::DuplicateLabel&)
      // the variable "C",1 can be created but the BN does not allow to add such a variable
      TS_ASSERT_THROWS(i3 = mn.add("C", 1), const gum::OperationNotAllowed&)
      GUM_UNUSED(i3);

      TS_ASSERT_THROWS_NOTHING(mn.add("X{top|middle|bottom}"))
      TS_ASSERT_EQUALS(mn.variable("X").toString(), "X:Labelized({top|middle|bottom})")

      // a mono-label with default 4 is impossible
      TS_ASSERT_THROWS(mn.add("Y[1,1]", 4), const gum::ArgumentError&)

      // a mono-label with default 1 is possible but can not be integrated in the model
      TS_ASSERT_THROWS(mn.add("Y[1,1]", 1), const gum::OperationNotAllowed&)
    }

    GUM_ACTIVE_TEST(Inference) {
      {
        auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B;B--C");
        gum::ShaferShenoyMNInference< double > ie(&mn);
        ie.makeInference();
        TS_ASSERT_THROWS(auto p = ie.jointPosterior({0, 2}), const gum::UndefinedElement&)
      }
      {
        auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B;B--C");
        gum::ShaferShenoyMNInference< double > ie(&mn);
        ie.addJointTarget({0, 2});
        ie.makeInference();
        auto p = ie.jointPosterior({0, 2});
      }
    }
  };


}   // namespace gum_tests
