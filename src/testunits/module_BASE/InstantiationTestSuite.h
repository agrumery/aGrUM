/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/variables/discretizedVariable.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/BN/BayesNet.h>

namespace gum_tests {

  class InstantiationTestSuite : public CxxTest::TestSuite {
    gum::BayesNet<double> *bn;
    gum::Id i1, i2, i3, i4, i5;

    void setUp() {
      bn = new gum::BayesNet<double>();

      gum::LabelizedVariable n1("1", "", 2), n2("2", "", 2), n3("3", "", 2);
      gum::LabelizedVariable n4("4", "", 2), n5("5", "", 2);

      i1 = bn->add(n1);
      i2 = bn->add(n2);
      i3 = bn->add(n3);
      i4 = bn->add(n4);
      i5 = bn->add(n5);

      bn->addArc(i1, i3);
      bn->addArc(i1, i4);
      bn->addArc(i3, i5);
      bn->addArc(i4, i5);
      bn->addArc(i2, i4);
      bn->addArc(i2, i5);

      fill(*bn);
    }

    void tearDown() { delete bn; }

    public:
    void testCreation() {
      gum::Instantiation i;
      TS_ASSERT_EQUALS(i.nbrDim(), (gum::Size)0);
    }

    void testInsertSupprVariables() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      gum::Instantiation i;

      TS_GUM_ASSERT_THROWS_NOTHING(i << a << b << c);
      TS_ASSERT_THROWS(i << a, gum::DuplicateElement);
      TS_ASSERT_EQUALS(i.nbrDim(), (gum::Size)3);
      TS_ASSERT_EQUALS(i.domainSize(), (gum::Size)(2 * 4 * 5));

      TS_ASSERT(i.contains(a));

      TS_ASSERT_EQUALS(i.pos(b), (gum::Size)1);
      TS_ASSERT_EQUALS(&(i.variable(1)), &b);

      i.chgVal(a, 1).chgVal(b, 2).chgVal(c, 4);
      TS_ASSERT_EQUALS(i.toString(), "<a:1|b:2|c:4>");

      TS_GUM_ASSERT_THROWS_NOTHING(i >> b);
      TS_ASSERT_THROWS(i >> b, gum::NotFound);
      TS_ASSERT_EQUALS(i.nbrDim(), (gum::Size)2);
      TS_ASSERT_EQUALS(i.domainSize(), (gum::Size)(2 * 5));

      TS_ASSERT_EQUALS(i.toString(), "<a:1|c:4>");

      TS_ASSERT(!i.contains(b));
    }

    void testIncrementations() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      gum::Instantiation i;
      i << a << b << c;
      gum::Instantiation j;
      j << c << b;
      ;
      gum::Size nb;

      for (nb = 0, i.setFirst(); !i.end(); ++i)
        nb++;

      TS_ASSERT_EQUALS(nb, i.domainSize());

      for (nb = 0, i.setFirstNotVar(b); !i.end(); i.incNotVar(b))
        nb++;

      TS_ASSERT_EQUALS(nb, (gum::Size)(2 * 5));

      for (nb = 0, i.setFirstVar(b); !i.end(); i.incVar(b))
        nb++;

      TS_ASSERT_EQUALS(nb, (gum::Size)4);

      for (nb = 0, i.setFirstIn(j); !i.end(); i.incIn(j))
        nb++;

      TS_ASSERT_EQUALS(nb, (gum::Size)(4 * 5));

      for (nb = 0, i.setFirstOut(j); !i.end(); i.incOut(j))
        nb++;

      TS_ASSERT_EQUALS(nb, (gum::Size)2);
    }

    void testDecrementations() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      gum::Instantiation i;
      i << a << b << c;
      gum::Instantiation j;
      j << c << b;
      gum::Size nb;

      for (nb = 0, i.setLast(); !i.rend(); --i)
        nb++;

      TS_ASSERT_EQUALS(nb, i.domainSize());

      for (nb = 0, i.setLastNotVar(b); !i.rend(); i.decNotVar(b))
        nb++;

      TS_ASSERT_EQUALS(nb, (gum::Size)(2 * 5));

      for (nb = 0, i.setLastVar(b); !i.rend(); i.decVar(b))
        nb++;

      TS_ASSERT_EQUALS(nb, (gum::Size)4);

      for (nb = 0, i.setLastIn(j); !i.rend(); i.decIn(j))
        nb++;

      TS_ASSERT_EQUALS(nb, (gum::Size)(4 * 5));

      for (nb = 0, i.setLastOut(j); !i.rend(); i.decOut(j))
        nb++;

      TS_ASSERT_EQUALS(nb, (gum::Size)2);
    }

    void testReordering() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      gum::Instantiation i;
      i << a << b << c;
      gum::Instantiation j;
      j << c << a;

      // reordering in {in|de}crementation
      i.setFirst();
      TS_ASSERT_EQUALS(i.toString(), "<a:0|b:0|c:0>");
      i.inc();
      TS_ASSERT_EQUALS(i.toString(), "<a:1|b:0|c:0>");
      i.incIn(j);
      TS_ASSERT_EQUALS(i.toString(), "<a:1|b:0|c:1>");
      i.decIn(j);
      TS_ASSERT_EQUALS(i.toString(), "<a:1|b:0|c:0>");

      // reordering
      i.reorder(j);
      TS_ASSERT_EQUALS(i.toString(), "<c:0|a:1|b:0>");
    }

    void testReordering_cpt() {

      gum::Instantiation Order;

      for (const auto node : bn->topologicalOrder())
        Order.add(bn->variable(node));

      const gum::Potential<double> &pot = bn->cpt(i5);

      gum::Instantiation inst(pot);
      gum::Instantiation instcomp(inst);
      TS_ASSERT_THROWS(instcomp.reorder(Order), gum::OperationNotAllowed);

      inst.forgetMaster();

      for (inst.setFirst(); !inst.end(); ++inst) {
        gum::Instantiation instcomp(inst);

        instcomp.reorder(Order);
        TS_ASSERT_EQUALS(pot[inst], pot[instcomp]);
      }

      gum::Instantiation inst2(pot);
      inst2.forgetMaster();
      inst2.reorder(Order);

      for (inst2.setFirst(); !inst2.end(); ++inst2) {
        inst.setVals(inst2);
        TS_ASSERT_EQUALS(pot[inst2], pot[inst]);
      }
    }

    void testSlavery() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      gum::MultiDimArray<double> t;
      t << a << b << c;
      gum::MultiDimArray<double> t2;
      t2 << b << a << c;

      gum::Instantiation i(t);
      i.setFirst();
      TS_ASSERT_EQUALS(i.toString(), "<a:0|b:0|c:0>");
      TS_ASSERT_THROWS(i.actAsSlave(t2), gum::OperationNotAllowed);

      gum::Instantiation j;
      j << b << c;
      TS_ASSERT(!j.actAsSlave(t));
      j << a;
      j.chgVal(a, 1);
      TS_ASSERT(j.actAsSlave(t));
      TS_ASSERT_EQUALS(j.toString(), "<a:1|b:0|c:0>");
      j.forgetMaster();
      TS_ASSERT(j.actAsSlave(t2));
      TS_ASSERT_EQUALS(j.toString(), "<b:0|a:1|c:0>");

      gum::DiscretizedVariable<double> d("d", "Discretized variable");
      ;
      TS_GUM_ASSERT_THROWS_NOTHING(d.addTick(3.1).addTick(2.0).addTick(4.0));
      TS_ASSERT_THROWS(j << d, gum::OperationNotAllowed);
      j.forgetMaster();
      TS_GUM_ASSERT_THROWS_NOTHING(j << d);
      j.chgVal(d, d[2.5]);
      TS_ASSERT(!j.actAsSlave(t));
    }

    void testChgValIn() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5), d("d", "fourth var", 2);

      gum::Instantiation i;
      i << b << c << d;
      gum::Instantiation j;
      j << b << a << c;

      i.chgVal(b, 2).chgVal(c, 3).chgVal(d, 1);
      j.chgVal(b, 1).chgVal(a, 0).chgVal(c, 1);
      TS_ASSERT_EQUALS(i.toString(), "<b:2|c:3|d:1>");
      TS_ASSERT_EQUALS(j.toString(), "<b:1|a:0|c:1>");

      j.setVals(i);
      TS_ASSERT_EQUALS(j.toString(), "<b:2|a:0|c:3>");
    }

    void testOperatorEgal() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5), d("d", "fourth var", 2);
      gum::MultiDimArray<char> p, q;
      p << a << b << c;
      q << c << d;

      gum::Instantiation ip(p), ip2(p), iq(q), j, k;

      ++(++ip);

      // free = slave
      TS_GUM_ASSERT_THROWS_NOTHING(j = ip);
      TS_ASSERT_EQUALS(p.toOffset(j), p.toOffset(ip));
      TS_ASSERT_EQUALS(j.toString(), ip.toString());

      // slave_same_master=slave_same_master
      TS_GUM_ASSERT_THROWS_NOTHING(ip2 = ip);
      TS_ASSERT_EQUALS(p.toOffset(ip2), p.toOffset(ip));
      TS_ASSERT_EQUALS(ip2.toString(), ip.toString());

      // slave = free - same variables set
      k << b << a << c;
      ++k;
      TS_GUM_ASSERT_THROWS_NOTHING(ip2 = k);
      TS_ASSERT_EQUALS(p.toOffset(k), p.toOffset(ip2));
      TS_ASSERT_EQUALS(k.val(a), ip2.val(a));
      TS_ASSERT_EQUALS(k.val(b), ip2.val(b));
      TS_ASSERT_EQUALS(k.val(c), ip2.val(c));

      // slave = free- not same variables set
      TS_ASSERT_THROWS(iq = k, gum::OperationNotAllowed);

      // slave = slave - not same master
      TS_ASSERT_THROWS(iq = ip, gum::OperationNotAllowed);
    }

    void testOffsetOperation() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5), d("d", "fourth var", 2);
      gum::MultiDimArray<char> p;
      p << a << b << c;

      gum::Instantiation i1(p), i2(p);

      for (i1.setFirst(); !i1.end(); ++i1) {
        gum::Size l = p.toOffset(i1);
        p.fromOffset(i2, l);
        TS_ASSERT_EQUALS(p.toOffset(i1), p.toOffset(i2));
      }
    }

    private:
    // Builds a BN to test the inference
    void fill(gum::BayesNet<double> &bn) {
      const gum::Potential<double> &p1 = bn.cpt(i1);
      {
        // FILLING PARAMS
        const double t[2] = {0.5, 0.5};
        int n = 2;
        const std::vector<double> v(t, t + n);
        p1.fillWith(v);
      }

      const gum::Potential<double> &p2 = bn.cpt(i2);
      {
        // FILLING PARAMS
        const double t[2] = {0.3, 0.7};
        int n = 2;
        const std::vector<double> v(t, t + n);
        p2.fillWith(v);
      }

      const gum::Potential<double> &p3 = bn.cpt(i3);
      {
        // FILLING PARAMS
        const double t[4] = {0.1, 0.9, 0.9, 0.1};
        int n = 4;
        const std::vector<double> v(t, t + n);
        p3.fillWith(v);
      }

      const gum::Potential<double> &p4 = bn.cpt(i4);
      {
        // FILLING PARAMS
        const double t[8] = {0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0};
        int n = 8;
        const std::vector<double> v(t, t + n);
        p4.fillWith(v);
      }

      const gum::Potential<double> &p5 = bn.cpt(i5);
      {
        // FILLING PARAMS
        const double t[16] = {1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0,
                              0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0};
        int n = 16;
        const std::vector<double> v(t, t + n);
        p5.fillWith(v);
      }
    }
  };
}
