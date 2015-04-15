/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/variables/discretizedVariable.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/setInst.h>
#include <agrum/multidim/instantiation.h>

namespace gum_tests {

  class SetInstTestSuite : public CxxTest::TestSuite {
    void setUp() {}

    void tearDown() {}

    public:
    void testCreation() {
      gum::Instantiation in;
      gum::SetInst i;
      TS_ASSERT_EQUALS(i.nbrDim(), (gum::Size)0);
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      in << a << c << b;
      TS_GUM_ASSERT_THROWS_NOTHING(gum::SetInst i2(in));
    }

    void testInsertSupprVariables() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      gum::SetInst i;

      TS_GUM_ASSERT_THROWS_NOTHING(i << a << b << c);
      TS_ASSERT_THROWS(i << a, gum::DuplicateElement);
      TS_ASSERT_EQUALS(i.nbrDim(), (gum::Size)3);
      TS_ASSERT_EQUALS(i.domainSize(), (gum::Size)(2 * 4 * 5));

      TS_ASSERT(i.contains(a));

      TS_ASSERT_EQUALS(i.pos(b), (gum::Size)1);
      TS_ASSERT_EQUALS(&(i.variable(1)), &b);

      i.chgVal(a, 1).chgVal(b, 2).chgVal(c, 4);
      TS_ASSERT_EQUALS(i.toString(), "<a:10|b:0100|c:10000>");

      TS_GUM_ASSERT_THROWS_NOTHING(i >> b);
      TS_ASSERT_THROWS(i >> b, gum::NotFound);
      TS_ASSERT_EQUALS(i.nbrDim(), (gum::Size)2);
      TS_ASSERT_EQUALS(i.domainSize(), (gum::Size)(2 * 5));

      TS_ASSERT_EQUALS(i.toString(), "<a:10|c:10000>");

      TS_ASSERT(!i.contains(b));
    }

    void testRemValues() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      gum::SetInst i;

      TS_GUM_ASSERT_THROWS_NOTHING(i << a << b << c);
      TS_ASSERT_EQUALS(i.domainSize(), (gum::Size)(2 * 4 * 5));

      i.chgVal(a, 1).chgVal(b, 2).chgVal(c, 4);
      i.addVal(a, 0).addVal(b, 1).addVal(c, 2);
      TS_ASSERT_EQUALS(i.toString(), "<a:11|b:0110|c:10100>");
      i.remVal(a, 1).remVal(b, 2).remVal(c, 4);
      TS_ASSERT_EQUALS(i.toString(), "<a:01|b:0010|c:00100>");
    }

    void test_ou_et_Values() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      gum::SetInst i, j;

      i << a << b << c;
      j << a << b << c;
      TS_ASSERT_EQUALS(i.domainSize(), (gum::Size)(2 * 4 * 5));

      i.chgVals(a, 2).chgVals(b, 14).chgVals(c, 28);
      i.interVals(a, 1);
      i.interVals(b, 7);
      i.interVals(c, 7);
      TS_ASSERT_EQUALS(i.toString(), "<a:00|b:0110|c:00100>");

      i.addVal(a, 0).addVal(b, 3).addVal(c, 1);

      TS_ASSERT_EQUALS(i.toString(), "<a:01|b:1110|c:00110>");
      i.remVal(a, 1).remVal(b, 2).remVal(c, 2);
      TS_ASSERT_EQUALS(i.toString(), "<a:01|b:1010|c:00010>");

      j.chgVals(a, 2).chgVals(b, 14).chgVals(c, 28);
      TS_ASSERT_EQUALS(j.toString(), "<a:10|b:1110|c:11100>");
      j.remVals(a, 1).remVals(b, 8).remVals(c, 14);
      TS_ASSERT_EQUALS(j.toString(), "<a:10|b:0110|c:10000>");
    }

    void testReordering() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      gum::SetInst i;
      i << a << b << c;
      gum::SetInst j;
      j << c << a;

      // reordering in {in|de}crementation
      TS_ASSERT_EQUALS(i.toString(), "<a:01|b:0001|c:00001>");
      // reordering
      i.reorder(j);
      TS_ASSERT_EQUALS(i.toString(), "<c:00001|a:01|b:0001>");
    }

    void testChgValIn() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5), d("d", "fourth var", 2);

      gum::SetInst i;
      i << b << c << d;
      gum::SetInst j;
      j << b << a << c;

      i.chgVal(b, 2).chgVal(c, 3).chgVal(d, 1);
      j.chgVal(b, 1).chgVal(a, 0).chgVal(c, 1);
      TS_ASSERT_EQUALS(i.toString(), "<b:0100|c:01000|d:10>");
      TS_ASSERT_EQUALS(j.toString(), "<b:0010|a:01|c:00010>");

      j.chgValIn(i);
      TS_ASSERT_EQUALS(j.toString(), "<b:0100|a:01|c:01000>");
    }

    void testOperatorEgal() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5), d("d", "fourth var", 2);
      gum::MultiDimArray<char> p, q;
      p << a << b << c;
      q << c << d;

      gum::SetInst ip(p), ip2(p), iq(q), j, k;

      //   ++( ++ip );

      // free = slave
      TS_GUM_ASSERT_THROWS_NOTHING(j = ip);
      // TS_ASSERT_EQUALS( p.toOffset( j ), p.toOffset( ip ) );
      TS_ASSERT_EQUALS(j.toString(), ip.toString());

      // slave_same_master=slave_same_master
      TS_GUM_ASSERT_THROWS_NOTHING(ip2 = ip);
      // TS_ASSERT_EQUALS( p.toOffset( ip2 ), p.toOffset( ip ) );
      TS_ASSERT_EQUALS(ip2.toString(), ip.toString());

      // slave = free - same variables set
      k << b << a << c;
      // ++k;
      TS_GUM_ASSERT_THROWS_NOTHING(ip2 = k);
      // TS_ASSERT_EQUALS( p.toOffset( k ), p.toOffset( ip2 ) );
      TS_ASSERT_EQUALS(k.val(a), ip2.val(a));
      TS_ASSERT_EQUALS(k.val(b), ip2.val(b));
      TS_ASSERT_EQUALS(k.val(c), ip2.val(c));

      // slave = free- not same variables set
      // TS_ASSERT_THROWS( iq = k, gum::OperationNotAllowed );

      // slave = slave - not same master
      // TS_ASSERT_THROWS( iq = ip, gum::OperationNotAllowed );
    }
  };
}
