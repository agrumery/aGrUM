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

#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/discretizedVariable.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/base/variables/rangeVariable.h>
#include <agrum/MRF/MarkovRandomField.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  MarkovRandomField
#define GUM_CURRENT_MODULE MRF

// The graph used for the tests:
//          1   2_
//         / \ / /
//        3   4 /
//         \ / /
//          5_/
//

namespace gum_tests {
  struct MarkovRandomFieldTestSuite {
    private:
    static void _fill(gum::MarkovRandomField< double >& mn) {
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
    static void testSimpleConstructor() {
      GUM_CHECK_ASSERT_THROWS_NOTHING({
        gum::MarkovRandomField< double > mn;
        GUM_UNUSED(mn);
      });
    };

    static void testConstructor() {
      gum::MarkovRandomField< double > mn;
      _fill(mn);

      CHECK((mn.size()) == (static_cast< gum::Idx >(5)));
      CHECK((mn.sizeEdges()) == (static_cast< gum::Idx >(6)));
      CHECK((mn.dim()) == ((gum::Idx)(3 * 3 + 3 * 3 + 3 * 7 + 3 * 3 * 7)));
      CHECK((mn.toString()) == ("MRF{nodes: 5, edges: 6, domainSize: 567, dim: 102}"));
      CHECK((mn.neighbours("41")) == (gum::NodeSet({0, 1, 4})));
      CHECK((mn.neighbours(3)) == (gum::NodeSet({0, 1, 4})));

      CHECK((mn.variable(1).name()) == ("21"));
      CHECK((mn.nodeId(mn.variable(2))) == (gum::NodeId(2)));
      CHECK((mn.idFromName("31")) == (gum::NodeId(2)));
      CHECK((mn.variableFromName("41").name()) == ("41"));

      CHECK((mn.maxVarDomainSize()) == (static_cast< gum::Size >(7)));
      CHECK((mn.minParam()) == (0.0));
      CHECK((mn.minNonZeroParam()) == (0.03));
      CHECK((mn.maxParam()) == (1.0));
      CHECK((mn.maxNonOneParam()) == (0.97));
    }

    static void testCopyConstructor() {
      gum::MarkovRandomField< double > mn;
      _fill(mn);
      gum::MarkovRandomField< double > mn2(mn);
      CHECK((mn2.toString()) == ("MRF{nodes: 5, edges: 6, domainSize: 567, dim: 102}"));

      GUM_CHECK_ASSERT_THROWS_NOTHING({
        for (const auto n: mn.nodes()) {
          CHECK((mn.variable(n).name()) == (mn2.variable(n).name()));
          CHECK((mn.neighbours(n)) == (mn2.neighbours(n)));
        }
      });
    }

    static void testCopyOperator() {
      gum::MarkovRandomField< double > mn;
      gum::MarkovRandomField< double > mn2;
      _fill(mn);
      mn2 = mn;
      CHECK((mn2.toString()) == ("MRF{nodes: 5, edges: 6, domainSize: 567, dim: 102}"));
      for (const auto n: mn.nodes()) {
        CHECK((mn.variable(n).name()) == (mn2.variable(n).name()));
        CHECK((mn.neighbours(n)) == (mn2.neighbours(n)));
      }
    }

    static void testEqualityOperators() {
      gum::MarkovRandomField< double > mn1;
      _fill(mn1);
      gum::MarkovRandomField< double > mn2;
      CHECK((mn1) != (mn2));
      _fill(mn2);
      CHECK((mn1) == (mn2));
      mn2.generateFactors();
      CHECK((mn1) != (mn2));
    }

    static void testOrderInsertion() {
      gum::MarkovRandomField< double > mn;
      mn.add("V0", 2);
      mn.add("V1", 2);
      mn.add("V2", 2);
      mn.add("V3", 2);

      // V0 should be the first
      mn.addFactor({"V0", "V1"});
      CHECK((mn.factor({"V0", "V1"}).variable(0).name()) == ("V0"));
      CHECK((mn.factor({"V1", "V0"}).variable(0).name()) == ("V0"));

      // V2 should be the first
      mn.addFactor({"V2", "V1"});
      CHECK((mn.factor({"V2", "V1"}).variable(0).name()) == ("V2"));
      CHECK((mn.factor({"V1", "V2"}).variable(0).name()) == ("V2"));

      // 2 should be the first
      gum::NodeSet s1{2, 3};
      mn.addFactor(s1);
      CHECK((mn.factor(s1).variable(0).name()) == (mn.variable(2).name()));

      // 1 should be the first
      gum::NodeSet s2{3, 1};
      mn.addFactor(s2);
      CHECK((mn.factor(s2).variable(0).name()) == (mn.variable(1).name()));
    }

    static void testInsertionFromTensor() {
      gum::MarkovRandomField< double > mn;
      _fill(mn);
      CHECK_THROWS_AS(mn.addFactor(gum::Tensor< double >()),
                      const gum::InvalidArgument&);   // no empty factor
      CHECK_THROWS_AS(mn.addFactor({"11", "31"}),
                      const gum::InvalidArgument&);   // already exists

      {
        gum::MarkovRandomField< double > mn1;
        _fill(mn1);
        gum::Tensor< double > pot;
        pot.add(mn1.variable("11"));
        pot.add(mn1.variable("21"));
        pot.randomDistribution();
        GUM_CHECK_ASSERT_THROWS_NOTHING(mn1.addFactor(pot));
        CHECK((pot.toString()) == (mn1.factor({"11", "21"}).toString()));
      }
      {
        gum::MarkovRandomField< double > mn1;
        _fill(mn1);
        gum::Tensor< double > pot;
        pot.add(mn1.variable("21"));
        pot.add(mn1.variable("11"));
        pot.randomDistribution();
        GUM_CHECK_ASSERT_THROWS_NOTHING(mn1.addFactor(pot));

        // should be equal because no sorting by order of the vars in pot.
        CHECK((pot.toString()) == (mn1.factor({"11", "21"}).toString()));

        // but the data should be the same
        gum::Instantiation I(pot);
        const auto&        factor = mn1.factor({"21", "11"});
        for (I.setFirst(); !I.end(); I.inc()) {
          CHECK((pot.get(I)) == doctest::Approx(factor.get(I)).epsilon(1e-10));
        }
      }
    }

    static void testIterations() {
      gum::MarkovRandomField< double > mn;
      _fill(mn);
      gum::Size cpt = static_cast< gum::Size >(0);

      for (const auto node: mn.nodes()) {
        GUM_UNUSED(node);
        cpt++;
      }

      CHECK((cpt) == (mn.size()));

      cpt = static_cast< gum::Size >(0);

      for (const auto& edg: mn.edges()) {
        GUM_UNUSED(edg);
        cpt++;
      }

      CHECK((cpt) == (mn.sizeEdges()));
    }

    static void testEraseFactor() {
      gum::MarkovRandomField< double > mn;
      _fill(mn);
      CHECK_THROWS_AS(mn.eraseFactor({12, 14}), const gum::InvalidArgument&);
      GUM_CHECK_ASSERT_THROWS_NOTHING(mn.eraseFactor({2, 4}));

      CHECK((mn.size()) == (static_cast< gum::Idx >(5)));
      CHECK((mn.sizeEdges()) == (static_cast< gum::Idx >(5)));
      CHECK((mn.dim()) == ((gum::Idx)(3 * 3 + 3 * 3 + 3 * 3 * 7)));
      CHECK((mn.toString()) == ("MRF{nodes: 5, edges: 5, domainSize: 567, dim: 81}"));
    }

    static void testEraseFactorWithNames() {
      gum::MarkovRandomField< double > mn;
      _fill(mn);
      CHECK_THROWS_AS(mn.eraseFactor({"31", "21"}), const gum::InvalidArgument&);
      GUM_CHECK_ASSERT_THROWS_NOTHING(mn.eraseFactor({"31", "51"}));

      CHECK((mn.size()) == (static_cast< gum::Idx >(5)));
      CHECK((mn.sizeEdges()) == (static_cast< gum::Idx >(5)));
      CHECK((mn.dim()) == ((gum::Idx)(3 * 3 + 3 * 3 + 3 * 3 * 7)));
      CHECK((mn.toString()) == ("MRF{nodes: 5, edges: 5, domainSize: 567, dim: 81}"));
    }

    static void testErase() {
      gum::MarkovRandomField< double > mn;
      _fill(mn);
      CHECK_THROWS_AS(mn.erase(36), const gum::InvalidArgument&);
      GUM_CHECK_ASSERT_THROWS_NOTHING(mn.erase(3));

      CHECK((mn.size()) == (static_cast< gum::Idx >(4)));
      CHECK((mn.sizeEdges()) == (static_cast< gum::Idx >(3)));
      CHECK((mn.dim()) == ((gum::Idx)(3 * 3 + 3 * 7 + 3 * 7)));
      CHECK((mn.toString()) == ("MRF{nodes: 4, edges: 3, domainSize: 189, dim: 51}"));
    }

    static void testEraseWithNames() {
      gum::MarkovRandomField< double > mn;
      _fill(mn);
      CHECK_THROWS_AS(mn.erase("36"), const gum::NotFound&);
      GUM_CHECK_ASSERT_THROWS_NOTHING(mn.erase("41"));

      CHECK((mn.size()) == (static_cast< gum::Idx >(4)));
      CHECK((mn.sizeEdges()) == (static_cast< gum::Idx >(3)));
      CHECK((mn.dim()) == ((gum::Idx)(3 * 3 + 3 * 7 + 3 * 7)));
      CHECK((mn.toString()) == ("MRF{nodes: 4, edges: 3, domainSize: 189, dim: 51}"));
    }

    static void testToDot() {
      gum::MarkovRandomField< double > mn
          = gum::MarkovRandomField< double >::fastPrototype("A--B--C;B--D;C--E;D--E--F");
      const auto s1 = mn.toDot();
      const auto s2 = mn.toDotAsFactorGraph();
      GUM_UNUSED(s1);
      GUM_UNUSED(s2);
    }

    static void testFromBN() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D;C<-E->F<-G;F<-A");
      auto mn = gum::MarkovRandomField< double >::fromBN(bn);

      gum::Tensor< double > pbn;
      pbn.fill(1);
      for (gum::NodeId nod: bn.nodes()) {
        CHECK((bn.variable(nod).toString()) == (mn.variable(nod).toString()));

        pbn *= bn.cpt(nod);
      }

      gum::Tensor< double > pmn;
      pmn.fill(1);
      for (const auto& key: mn.factors()) {
        CHECK((mn.factor(key.first)) == (*key.second));
        pmn *= *key.second;
      }
      pmn.normalize();

      gum::NodeSet s;
      s.clear();
      s << 0 << 4 << 5 << 6;
      GUM_CHECK_ASSERT_THROWS_NOTHING(mn.factor(s));
      s.clear();
      s << 4 << 0 << 6 << 5;
      GUM_CHECK_ASSERT_THROWS_NOTHING(mn.factor(s));

      gum::Tensor< double > ppmn(pbn);
      ppmn.fillWith(pmn);   // copy of pmn using pbn's variables
      auto diff = (pbn - ppmn).new_abs();
      CHECK((pbn.domainSize()) == (diff.domainSize()));
      CHECK((diff.max()) < (1e-10));

      CHECK((mn.graph()) == (bn.moralGraph()));
    }

    static void testExistsEdge() {
      auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B--C;C--D;E--F--G");

      CHECK(mn.existsEdge(0, 1));
      CHECK(mn.existsEdge("A", "B"));
      CHECK(mn.existsEdge(1, 0));
      CHECK(mn.existsEdge("B", "A"));
      CHECK(mn.existsEdge(0, 2));
      CHECK(mn.existsEdge("A", "C"));
      CHECK(!mn.existsEdge(3, 7));
      CHECK(!mn.existsEdge("C", "G"));
    }

    static void testMinimalCondSet() {
      auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B--C;C--D;E--F--G;B--E");

      CHECK((mn.minimalCondSet(0, {1, 2, 3, 4, 5, 6})) == (gum::NodeSet({1, 2})));
      CHECK((mn.minimalCondSet({0, 6}, {1, 2, 3, 4, 5})) == (gum::NodeSet({1, 2, 4, 5})));
      CHECK((mn.minimalCondSet(3, {0, 4, 5, 6})) == (gum::NodeSet({0, 4})));
    }

    static void testIndependence() {
      auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B--C;C--D;E--F--G;B--E;D--G;X");
      CHECK(mn.isIndependent("D", "X", {}));
      CHECK(!mn.isIndependent("D", "A", {"C"}));
      CHECK(mn.isIndependent("D", "A", {"C", "G"}));
      CHECK(!mn.isIndependent("G", "A", {"C", "F"}));
      CHECK(mn.isIndependent("G", "A", {"D", "E"}));
    }

    static void testFastPrototypeVarType() {
      auto mn = gum::MarkovRandomField< float >::fastPrototype(
          "a{1|4|6}--b{1|-4|6};c{1|toto|6}--d{1.0|-4.0|6.0};e{1|-4|6.0}--f{1.0|-4.0|+6.0}");
      CHECK((mn.variable("a").varType()) == (gum::VarType::INTEGER));
      CHECK((mn.variable("b").varType()) == (gum::VarType::INTEGER));
      CHECK((mn.variable("c").varType()) == (gum::VarType::LABELIZED));
      CHECK((mn.variable("d").varType()) == (gum::VarType::NUMERICAL));
      CHECK((mn.variable("e").varType()) == (gum::VarType::NUMERICAL));
      CHECK((mn.variable("f").varType()) == (gum::VarType::NUMERICAL));
    }

    static void testMonoClique() {
      auto mn2 = gum::MarkovRandomField< float >::fastPrototype("A--B");
      GUM_CHECK_ASSERT_THROWS_NOTHING(mn2.factor({"A", "B"}));
    }

    static void testShortCutAddFastVar() {
      gum::MarkovRandomField< double > mn;

      gum::NodeId i1, i2, i3;

      CHECK_NOTHROW(i1 = mn.add("A", 2));
      CHECK_NOTHROW(i2 = mn.add("B", 3));
      CHECK((i1) == (gum::NodeId(0)));
      CHECK((i2) == (gum::NodeId(1)));

      CHECK_THROWS_AS(i3 = mn.add("A", 5), const gum::DuplicateLabel&);
      // the variable "C",1 can be created but the BN does not allow to add such a variable
      CHECK_THROWS_AS(i3 = mn.add("C", 1), const gum::OperationNotAllowed&);
      GUM_UNUSED(i3);

      CHECK_NOTHROW(mn.add("X{top|middle|bottom}"));
      CHECK((mn.variable("X").toString()) == ("X:Labelized({top|middle|bottom})"));

      // a mono-label with default 4 is impossible
      CHECK_THROWS_AS(mn.add("Y[1,1]", 4), const gum::ArgumentError&);

      // a mono-label with default 1 is possible but can not be integrated in the model
      CHECK_THROWS_AS(mn.add("Y[1,1]", 1), const gum::OperationNotAllowed&);
    }
  };

  GUM_TEST_ACTIF(SimpleConstructor)
  GUM_TEST_ACTIF(Constructor)
  GUM_TEST_ACTIF(CopyConstructor)
  GUM_TEST_ACTIF(CopyOperator)
  GUM_TEST_ACTIF(EqualityOperators)
  GUM_TEST_ACTIF(OrderInsertion)
  GUM_TEST_ACTIF(InsertionFromTensor)
  GUM_TEST_ACTIF(Iterations)
  GUM_TEST_ACTIF(EraseFactor)
  GUM_TEST_ACTIF(EraseFactorWithNames)
  GUM_TEST_ACTIF(Erase)
  GUM_TEST_ACTIF(EraseWithNames)
  GUM_TEST_ACTIF(ToDot)
  GUM_TEST_ACTIF(FromBN)
  GUM_TEST_ACTIF(ExistsEdge)
  GUM_TEST_ACTIF(MinimalCondSet)
  GUM_TEST_ACTIF(Independence)
  GUM_TEST_ACTIF(FastPrototypeVarType)
  GUM_TEST_ACTIF(MonoClique)
  GUM_TEST_ACTIF(ShortCutAddFastVar)

}   // namespace gum_tests
