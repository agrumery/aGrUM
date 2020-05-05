# -*- encoding: UTF-8 -*-
import unittest

import numpy
import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests

class BayesNetTestCase(pyAgrumTestCase):

  def _fill(self, mn):
    for i in [11,21,31,41]:
      mn.add(str(i),3)
    mn.add(str(51),7)
    mn.addFactor({"11", "31"})
       .fillWith({0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8});
    mn.addFactor({"11", "41"})
       .fillWith({0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0});
    mn.addFactor({"31", "51"}).fillWith(0.97);
    mn.addFactor({"21", "41", "51"}).fillWith(0.03);

    public:
    void setUp() {}

    void tearDown() {}

    void testSimpleConstructor(){TS_GUM_ASSERT_THROWS_NOTHING({
      gum::MarkovNet< double > mn;
      GUM_UNUSED(mn);
    })};

    void testConstructor() {
      gum::MarkovNet< double > mn;
      _fill(mn);

      TS_ASSERT_EQUALS(mn.size(), (gum::Idx)5);
      TS_ASSERT_EQUALS(mn.sizeEdges(), (gum::Idx)6);
      TS_ASSERT_EQUALS(mn.dim(), (gum::Idx)(3 * 3 + 3 * 3 + 3 * 7 + 3 * 3 * 7));
      TS_ASSERT_EQUALS(mn.toString(),
                       "MN{nodes: 5, edges: 6, domainSize: 567, dim: 102}");
      TS_ASSERT_EQUALS(mn.neighbours("41"), gum::NodeSet({0, 1, 4}));
      TS_ASSERT_EQUALS(mn.neighbours(3), gum::NodeSet({0, 1, 4}));

      TS_ASSERT_EQUALS(mn.variable(1).name(), "21");
      TS_ASSERT_EQUALS(mn.nodeId(mn.variable(2)), gum::NodeId(2));
      TS_ASSERT_EQUALS(mn.idFromName("31"), gum::NodeId(2));
      TS_ASSERT_EQUALS(mn.variableFromName("41").name(), "41");

      TS_ASSERT_EQUALS(mn.maxVarDomainSize(), gum::Size(7));
      TS_ASSERT_EQUALS(mn.minParam(), 0.0);
      TS_ASSERT_EQUALS(mn.minNonZeroParam(), 0.03);
      TS_ASSERT_EQUALS(mn.maxParam(), 1.0);
      TS_ASSERT_EQUALS(mn.maxNonOneParam(), 0.97);
    }

    void testCopyConstructor() {
      gum::MarkovNet< double > mn;
      _fill(mn);
      gum::MarkovNet< double > mn2(mn);
      TS_ASSERT_EQUALS(mn2.toString(),
                       "MN{nodes: 5, edges: 6, domainSize: 567, dim: 102}");

      TS_GUM_ASSERT_THROWS_NOTHING({
        for (const auto n: mn.nodes()) {
          TS_ASSERT_EQUALS(mn.variable(n).name(), mn2.variable(n).name());
          TS_ASSERT_EQUALS(mn.neighbours(n), mn2.neighbours(n));
        }
      });
    }

    void testCopyOperator() {
      gum::MarkovNet< double > mn;
      gum::MarkovNet< double > mn2;
      _fill(mn);
      mn2 = mn;
      TS_ASSERT_EQUALS(mn2.toString(),
                       "MN{nodes: 5, edges: 6, domainSize: 567, dim: 102}");
      for (const auto n: mn.nodes()) {
        TS_ASSERT_EQUALS(mn.variable(n).name(), mn2.variable(n).name());
        TS_ASSERT_EQUALS(mn.neighbours(n), mn2.neighbours(n));
      }
    }

    void testEqualityOperators() {
      gum::MarkovNet< double > mn1;
      _fill(mn1);
      gum::MarkovNet< double > mn2;
      TS_ASSERT(mn1 != mn2);
      _fill(mn2);
      TS_ASSERT(mn1 == mn2);
      mn2.generateFactors();
      TS_ASSERT(mn1 != mn2);
    }

    void testInsertion() {
      gum::MarkovNet< double > mn;
      _fill(mn);
      TS_ASSERT_THROWS(mn.addFactor(gum::Potential< double >()),
                       gum::InvalidArgument);   // no empty factor
      TS_ASSERT_THROWS(mn.addFactor({"11", "31"}),
                       gum::InvalidArgument);   // already exists

      {
        gum::MarkovNet< double > mn1;
        _fill(mn1);
        gum::Potential< double > pot;
        pot.add(mn1.variable("11"));
        pot.add(mn1.variable("21"));
        pot.randomDistribution();
        TS_GUM_ASSERT_THROWS_NOTHING(mn1.addFactor(pot));
        TS_ASSERT_EQUALS(pot.toString(), mn1.factor({"11", "21"}).toString());
      }
      {
        gum::MarkovNet< double > mn1;
        _fill(mn1);
        gum::Potential< double > pot;
        pot.add(mn1.variable("21"));
        pot.add(mn1.variable("11"));
        pot.randomDistribution();
        TS_GUM_ASSERT_THROWS_NOTHING(mn1.addFactor(pot));

        // should be different because of the sorting by order of the vars in pot.
        TS_ASSERT_DIFFERS(pot.toString(), mn1.factor({"11", "21"}).toString());

        // but the data should be the same
        gum::Instantiation I(pot);
        const auto&        factor = mn1.factor({"21", "11"});
        for (I.setFirst(); !I.end(); I.inc()) {
          TS_ASSERT_DELTA(pot.get(I), factor.get(I), 1e-10);
        }
      }
    }

    void testIterations() {
      gum::MarkovNet< double > mn;
      _fill(mn);
      gum::Size cpt = (gum::Size)0;

      for (const auto node: mn.nodes()) {
        GUM_UNUSED(node);
        cpt++;
      }

      TS_ASSERT_EQUALS(cpt, mn.size());

      cpt = (gum::Size)0;

      for (const auto& edg: mn.edges()) {
        GUM_UNUSED(edg);
        cpt++;
      }

      TS_ASSERT_EQUALS(cpt, mn.sizeEdges());
    }

    void testEraseFactor() {
      gum::MarkovNet< double > mn;
      _fill(mn);
      TS_ASSERT_THROWS(mn.eraseFactor({12, 14}), gum::InvalidArgument);
      TS_GUM_ASSERT_THROWS_NOTHING(mn.eraseFactor({2, 4}));

      TS_ASSERT_EQUALS(mn.size(), (gum::Idx)5);
      TS_ASSERT_EQUALS(mn.sizeEdges(), (gum::Idx)5);
      TS_ASSERT_EQUALS(mn.dim(), (gum::Idx)(3 * 3 + 3 * 3 + 3 * 3 * 7));
      TS_ASSERT_EQUALS(mn.toString(),
                       "MN{nodes: 5, edges: 5, domainSize: 567, dim: 81}");
    }

    void testEraseFactorWithNames() {
      gum::MarkovNet< double > mn;
      _fill(mn);
      TS_ASSERT_THROWS(mn.eraseFactor({"31", "21"}), gum::InvalidArgument);
      TS_GUM_ASSERT_THROWS_NOTHING(mn.eraseFactor({"31", "51"}));

      TS_ASSERT_EQUALS(mn.size(), (gum::Idx)5);
      TS_ASSERT_EQUALS(mn.sizeEdges(), (gum::Idx)5);
      TS_ASSERT_EQUALS(mn.dim(), (gum::Idx)(3 * 3 + 3 * 3 + 3 * 3 * 7));
      TS_ASSERT_EQUALS(mn.toString(),
                       "MN{nodes: 5, edges: 5, domainSize: 567, dim: 81}");
    }

    void testErase() {
      gum::MarkovNet< double > mn;
      _fill(mn);
      TS_ASSERT_THROWS(mn.erase(36), gum::InvalidArgument);
      TS_GUM_ASSERT_THROWS_NOTHING(mn.erase(3));

      TS_ASSERT_EQUALS(mn.size(), (gum::Idx)4);
      TS_ASSERT_EQUALS(mn.sizeEdges(), (gum::Idx)3);
      TS_ASSERT_EQUALS(mn.dim(), (gum::Idx)(3 * 3 + 3 * 7 + 3 * 7));
      TS_ASSERT_EQUALS(mn.toString(),
                       "MN{nodes: 4, edges: 3, domainSize: 189, dim: 51}");
    }

    void testEraseWithNames() {
      gum::MarkovNet< double > mn;
      _fill(mn);
      TS_ASSERT_THROWS(mn.erase("36"), gum::NotFound);
      TS_GUM_ASSERT_THROWS_NOTHING(mn.erase("41"));

      TS_ASSERT_EQUALS(mn.size(), (gum::Idx)4);
      TS_ASSERT_EQUALS(mn.sizeEdges(), (gum::Idx)3);
      TS_ASSERT_EQUALS(mn.dim(), (gum::Idx)(3 * 3 + 3 * 7 + 3 * 7));
      TS_ASSERT_EQUALS(mn.toString(),
                       "MN{nodes: 4, edges: 3, domainSize: 189, dim: 51}");
    }

    void testToDot() {
      gum::MarkovNet< double > mn =
         gum::MarkovNet< double >::fastPrototype("A-B-C;B-D;C-E;D-E-F");
      const auto s1 = mn.toDot();
      const auto s2 = mn.toDotAsFactorGraph();
      GUM_UNUSED(s1);
      GUM_UNUSED(s2);
    }

    void testFromBN() {
      auto bn =
         gum::BayesNet< double >::fastPrototype("A->B->C<-D;C<-E->F<-G;F<-A");
      auto mn = gum::MarkovNet< double >::fromBN(bn);

      gum::Potential< double > pbn;
      pbn.fill(1);
      for (gum::NodeId nod: bn.nodes()) {
        TS_ASSERT_EQUALS(bn.variable(nod).toString(), mn.variable(nod).toString());

        pbn *= bn.cpt(nod);
      }

      gum::Potential< double > pmn;
      pmn.fill(1);
      for (const auto& key: mn.factors()) {
        pmn *= *key.second;
      }
      pmn.normalize();

      gum::Potential< double > ppmn(pbn);
      ppmn.fillWith(pmn);   // copy of pmn using pbn's variables
      auto diff = (pbn - ppmn).abs();
      TS_ASSERT_EQUALS(pbn.domainSize(), diff.domainSize());
      TS_ASSERT_LESS_THAN(diff.max(), 1e-10);

      TS_ASSERT_EQUALS(mn.graph(), bn.moralGraph())
    }

    void testExistsEdge() {
      auto mn = gum::MarkovNet< double >::fastPrototype("A-B-C;C-D;E-F-G");

      TS_ASSERT(mn.existsEdge(0,1));
      TS_ASSERT(mn.existsEdge("A","B"));
      TS_ASSERT(mn.existsEdge(1,0));
      TS_ASSERT(mn.existsEdge("B","A"));
      TS_ASSERT(mn.existsEdge(0,2));
      TS_ASSERT(mn.existsEdge("A","C"));
      TS_ASSERT(mn.existsEdge(3,7));
      TS_ASSERT(mn.existsEdge("C","G"));
    }
  };

}   // namespace gum_tests