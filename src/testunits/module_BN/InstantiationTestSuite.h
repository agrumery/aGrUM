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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/multidim/implementations/multiDimArray.h>
#include <agrum/base/multidim/instantiation.h>
#include <agrum/base/variables/discretizedVariable.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  Instantiation
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  struct InstantiationTestSuite {
    gum::BayesNet< double >* bn;
    gum::NodeId              i1, i2, i3, i4, i5;

    public:
    static void testCreation() {
      gum::Instantiation i;
      CHECK((i.nbrDim()) == (static_cast< gum::Size >(0)));
    }

    InstantiationTestSuite() {
      bn = new gum::BayesNet< double >();

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

    ~InstantiationTestSuite() { delete bn; }

    static void testInsertSupprVariables() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      gum::Instantiation i;

      GUM_CHECK_ASSERT_THROWS_NOTHING(i << a << b << c);
      CHECK_THROWS_AS(i << a, const gum::DuplicateElement&);
      CHECK((i.nbrDim()) == (static_cast< gum::Size >(3)));
      CHECK((i.domainSize()) == ((gum::Size)(2 * 4 * 5)));

      CHECK(i.contains(a));

      CHECK((i.pos(b)) == (static_cast< gum::Size >(1)));
      CHECK((&(i.variable(1))) == (&b));

      i.chgVal(a, 1).chgVal(b, 2).chgVal(c, 4);
      CHECK((i.toString()) == ("<a:1|b:2|c:4>"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(i >> b);
      CHECK_THROWS_AS(i >> b, const gum::NotFound&);
      CHECK((i.nbrDim()) == (static_cast< gum::Size >(2)));
      CHECK((i.domainSize()) == ((gum::Size)(2 * 5)));

      CHECK((i.toString()) == ("<a:1|c:4>"));

      CHECK(!i.contains(b));
    }

    static void testIncrementations() {
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

      CHECK((nb) == (i.domainSize()));

      for (nb = 0, i.setFirstNotVar(b); !i.end(); i.incNotVar(b))
        nb++;

      CHECK((nb) == ((gum::Size)(2 * 5)));

      for (nb = 0, i.setFirstVar(b); !i.end(); i.incVar(b))
        nb++;

      CHECK((nb) == (static_cast< gum::Size >(4)));

      for (nb = 0, i.setFirstIn(j); !i.end(); i.incIn(j))
        nb++;

      CHECK((nb) == ((gum::Size)(4 * 5)));

      for (nb = 0, i.setFirstOut(j); !i.end(); i.incOut(j))
        nb++;

      CHECK((nb) == (static_cast< gum::Size >(2)));
    }

    static void testDecrementations() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      gum::Instantiation i;
      i << a << b << c;
      gum::Instantiation j;
      j << c << b;
      gum::Size nb;

      for (nb = 0, i.setLast(); !i.rend(); --i)
        nb++;

      CHECK((nb) == (i.domainSize()));

      for (nb = 0, i.setLastNotVar(b); !i.rend(); i.decNotVar(b))
        nb++;

      CHECK((nb) == ((gum::Size)(2 * 5)));

      for (nb = 0, i.setLastVar(b); !i.rend(); i.decVar(b))
        nb++;

      CHECK((nb) == (static_cast< gum::Size >(4)));

      for (nb = 0, i.setLastIn(j); !i.rend(); i.decIn(j))
        nb++;

      CHECK((nb) == ((gum::Size)(4 * 5)));

      for (nb = 0, i.setLastOut(j); !i.rend(); i.decOut(j))
        nb++;

      CHECK((nb) == (static_cast< gum::Size >(2)));
    }

    static void testReordering() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      gum::Instantiation i;
      i << a << b << c;
      gum::Instantiation j;
      j << c << a;

      // reordering in {in|de}crementation
      i.setFirst();
      CHECK((i.toString()) == ("<a:0|b:0|c:0>"));
      i.inc();
      CHECK((i.toString()) == ("<a:1|b:0|c:0>"));
      i.incIn(j);
      CHECK((i.toString()) == ("<a:1|b:0|c:1>"));
      i.decIn(j);
      CHECK((i.toString()) == ("<a:1|b:0|c:0>"));

      // reordering
      i.reorder(j);
      CHECK((i.toString()) == ("<c:0|a:1|b:0>"));
    }

    void testReordering_cpt() const {
      gum::Instantiation Order;

      for (const auto node: bn->topologicalOrder())
        Order.add(bn->variable(node));

      const gum::Tensor< double >& pot = bn->cpt(i5);

      gum::Instantiation inst(pot);
      gum::Instantiation instcomp(inst);
      CHECK_THROWS_AS(instcomp.reorder(Order), const gum::OperationNotAllowed&);

      inst.forgetMaster();

      for (inst.setFirst(); !inst.end(); ++inst) {
        gum::Instantiation instcomp(inst);

        instcomp.reorder(Order);
        CHECK((pot[inst]) == (pot[instcomp]));
      }

      gum::Instantiation inst2(pot);
      inst2.forgetMaster();
      inst2.reorder(Order);

      for (inst2.setFirst(); !inst2.end(); ++inst2) {
        inst.setVals(inst2);
        CHECK((pot[inst2]) == (pot[inst]));
      }
    }

    static void testSlavery() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      gum::MultiDimArray< double > t;
      t << a << b << c;
      gum::MultiDimArray< double > t2;
      t2 << b << a << c;

      gum::Instantiation i(t);
      i.setFirst();
      CHECK((i.toString()) == ("<a:0|b:0|c:0>"));
      CHECK_THROWS_AS(i.actAsSlave(t2), const gum::OperationNotAllowed&);

      gum::Instantiation j;
      j << b << c;
      CHECK(!j.actAsSlave(t));
      j << a;
      j.chgVal(a, 1);
      CHECK(j.actAsSlave(t));
      CHECK((j.toString()) == ("<a:1|b:0|c:0>"));
      j.forgetMaster();
      CHECK(j.actAsSlave(t2));
      CHECK((j.toString()) == ("<b:0|a:1|c:0>"));

      gum::DiscretizedVariable< double > d("d", "Discretized variable");
      ;
      GUM_CHECK_ASSERT_THROWS_NOTHING(d.addTick(3.1).addTick(2.0).addTick(4.0));
      CHECK_THROWS_AS(j << d, const gum::OperationNotAllowed&);
      j.forgetMaster();
      GUM_CHECK_ASSERT_THROWS_NOTHING(j << d);
      j.chgVal(d, d["2.5"]);
      CHECK(!j.actAsSlave(t));
    }

    static void testChgValIn() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5), d("d", "fourth var", 2);

      gum::Instantiation i;
      i << b << c << d;
      gum::Instantiation j;
      j << b << a << c;

      i.chgVal(b, 2).chgVal(c, 3).chgVal(d, 1);
      j.chgVal(b, 1).chgVal(a, 0).chgVal(c, 1);
      CHECK((i.toString()) == ("<b:2|c:3|d:1>"));
      CHECK((j.toString()) == ("<b:1|a:0|c:1>"));

      j.setVals(i);
      CHECK((j.toString()) == ("<b:2|a:0|c:3>"));

      j.chgVal("a", "1");
      j.chgVal("b", 0);
      j.chgVal("c", "2");
      CHECK((j.toString()) == ("<b:0|a:1|c:2>"));
    }

    static void testOperatorEgal() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5), d("d", "fourth var", 2);
      gum::MultiDimArray< char > p, q;
      p << a << b << c;
      q << c << d;

      gum::Instantiation ip(p), ip2(p), iq(q), j, k;

      ++(++ip);

      // free = slave
      GUM_CHECK_ASSERT_THROWS_NOTHING(j = ip);
      CHECK((p.toOffset(j)) == (p.toOffset(ip)));
      CHECK((j.toString()) == (ip.toString()));

      // slave_same_master=slave_same_master
      GUM_CHECK_ASSERT_THROWS_NOTHING(ip2 = ip);
      CHECK((p.toOffset(ip2)) == (p.toOffset(ip)));
      CHECK((ip2.toString()) == (ip.toString()));

      // slave = free - same variables set
      k << b << a << c;
      ++k;
      GUM_CHECK_ASSERT_THROWS_NOTHING(ip2 = k);
      CHECK((p.toOffset(k)) == (p.toOffset(ip2)));
      CHECK((k.val(a)) == (ip2.val(a)));
      CHECK((k.val(b)) == (ip2.val(b)));
      CHECK((k.val(c)) == (ip2.val(c)));

      // slave = free- not same variables set
      CHECK_THROWS_AS(iq = k, const gum::OperationNotAllowed&);

      // slave = slave - not same master
      CHECK_THROWS_AS(iq = ip, const gum::OperationNotAllowed&);
    }

    static void testOffsetOperation() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5), d("d", "fourth var", 2);
      gum::MultiDimArray< char > p;
      p << a << b << c;

      gum::Instantiation i1(p), i2(p);

      for (i1.setFirst(); !i1.end(); ++i1) {
        gum::Size l = p.toOffset(i1);
        p.fromOffset(i2, l);
        CHECK((p.toOffset(i1)) == (p.toOffset(i2)));
      }
    }

    static void testOperatorEqual() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5), d("d", "fourth var", 2);
      gum::MultiDimArray< char > p;
      p << a << b << c;

      gum::Instantiation i1(p), i2(p);
      i2.setFirst();
      ++i2;   // for i2 to be !=i1 in the first iteration
      for (i1.setFirst(); !i1.end(); ++i1) {
        CHECK((i1) != (i2));
        p.fromOffset(i2, p.toOffset(i1));
        CHECK((i1) == (i2));
      }

      gum::Instantiation j;
      CHECK((i1) != (j));
      j.inc();              // j is in overflow
      CHECK((i1) == (j));   // both are in overflow => equals
    }

    private:
    // Builds a BN to test the inference
    void fill(gum::BayesNet< double >& bn) const {
      bn.cpt(i1).fillWith({0.5, 0.5});
      bn.cpt(i2).fillWith({0.3, 0.7});
      bn.cpt(i3).fillWith({0.1, 0.9, 0.9, 0.1});
      bn.cpt(i4).fillWith(   // clang-format off
                             {0.4, 0.6,
                              0.5, 0.5,
                              0.5, 0.5,
                              1.0, 0.0});                                   // clang-format on
      bn.cpt(i5).fillWith(   // clang-format off
                             {1.0,0.0,
                              0.0,1.0,
                              0.0,1.0,
                              0.0,1.0,
                              0.0,1.0,
                              0.0,1.0,
                              0.0,1.0,
                              0.0,1.0});   // clang-format on
    }
  };

  GUM_TEST_ACTIF(Creation)
  GUM_TEST_ACTIF(InsertSupprVariables)
  GUM_TEST_ACTIF(Incrementations)
  GUM_TEST_ACTIF(Decrementations)
  GUM_TEST_ACTIF(Reordering)
  GUM_TEST_ACTIF(Reordering_cpt)
  GUM_TEST_ACTIF(Slavery)
  GUM_TEST_ACTIF(ChgValIn)
  GUM_TEST_ACTIF(OperatorEgal)
  GUM_TEST_ACTIF(OffsetOperation)
  GUM_TEST_ACTIF(OperatorEqual)
}   // namespace gum_tests
