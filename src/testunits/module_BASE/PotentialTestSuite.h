/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
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
#include <agrum/multidim/potential.h>

namespace gum_tests {

  class PotentialTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testCreation() {
      gum::Potential<double> p(new gum::MultiDimArray<double>());
      TS_ASSERT(p.empty());

      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c);
    }

    void testNormalisation() {

      gum::Potential<double> p(new gum::MultiDimArray<double>());

      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);

      p << a << b << c;

      gum::Size taille = p.domainSize();

      try {
        gum::Instantiation i(&p);

        p.fill((double)0);

        int cpt = 1;
        for (i.setFirst(); !i.end(); ++i)
          p.set(i, (double)(cpt++));

        p.normalize();

        i.setLast();
        TS_ASSERT_EQUALS(p[i], 2.0 / (taille + 1));

        i.setFirst();
        TS_ASSERT_EQUALS(p[i], 2.0 / (taille * (taille + 1)));

      } catch (gum::Exception e) {
        GUM_SHOWERROR(e);
      }
    }

    void testMarginalisation() {

      try {
        gum::Potential<double> p(new gum::MultiDimArray<double>());
        gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
            c("c", "third var", 5);
        p << a << b << c;
        gum::Instantiation i(&p);

        for (i.setFirst(); !i.end(); ++i)
          p.set(i, 1 - i.val(a));

        gum::Potential<double> q(new gum::MultiDimArray<double>());

        gum::DiscretizedVariable<int> d("d", "fourth var");
        d.addTick(3).addTick(5).addTick(4);

        q << a << c << d;

        TS_ASSERT_THROWS(q.marginalize(p), gum::OperationNotAllowed);

        // q>>d;

        gum::Potential<double> qq(new gum::MultiDimArray<double>());

        qq << a << c;

        TS_GUM_ASSERT_THROWS_NOTHING(qq.marginalize(p));

        i.setLast();
        TS_ASSERT_EQUALS(qq[i], 0.0);

        i.setFirst();
        TS_ASSERT_EQUALS(qq[i], 4.0);

      } catch (gum::Exception e) {
        GUM_SHOWERROR(e);
      }
    }

    void testMultiplicate() {
      gum::Potential<double> p1(new gum::MultiDimArray<double>());
      gum::Potential<double> p2(new gum::MultiDimArray<double>());
      gum::Potential<double> p3(new gum::MultiDimArray<double>());
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      gum::DiscretizedVariable<int> d("d", "fourth var");
      d.addTick(3).addTick(5).addTick(4);

      try {
        p1 << a << b << c;
        p2 << b << c << d;
        p3 << a << d;

        gum::Instantiation i;
        i << a << b << c << d;

        for (i.setFirst(); !i.end(); ++i) {
          p1.set(i, i.val(c));
          p2.set(i, i.val(d));
          p3.set(i, i.val(a));
        }

        // the content of p3 should be erased
        p3.multiplicate(p1, p2);

        TS_ASSERT_EQUALS(p3.nbrDim(), (gum::Size)4);

        gum::Instantiation j(p3);

        TS_ASSERT_EQUALS(j.toString(), "<a:0|b:0|c:0|d:[3;4[>");

        int nbr_err = 0;

        for (j.setFirst(); !j.end(); ++j)
          if (p3[j] != j.val(c) * j.val(d))
            nbr_err++;

        TS_ASSERT_EQUALS(nbr_err, 0);

      } catch (gum::Exception e) {
        GUM_SHOWERROR(e);
      }
    }

    void testDomainSizeChanges() {
      gum::Potential<double> p1(new gum::MultiDimArray<double>());
      gum::LabelizedVariable var1("var1", "first var", 2),
          var2("var2", "second var", 2), var3("var3", "third var", 2);
      p1 << var1 << var2 << var3;

      TS_ASSERT_EQUALS(p1.domainSize(),
                       (var1.domainSize() * var2.domainSize() * var3.domainSize()));
    }

    void testAddAnyNumber() {
      gum::Potential<double> proba(new gum::MultiDimArray<double>());
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      proba << a << b << c;

      gum::Instantiation i(proba);

      for (i.setFirst(); !i.end(); ++i)
        TS_GUM_ASSERT_THROWS_NOTHING(proba.set(i, (double)-1));
    }

    void testCopyProba() {
      gum::Potential<double> m(new gum::MultiDimArray<double>());
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);

      m << a << b << c;
      gum::Instantiation i(m);

      gum::Size cpt = 0;

      for (i.setFirst(); !i.end(); ++i) {
        m.set(i, cpt);
        cpt++;
      }

      gum::Potential<double> mm(new gum::MultiDimArray<double>());

      gum::LabelizedVariable x("x", "first var", 2), y("y", "second var", 4),
          z("z", "third var", 5);
      mm << x << z;
      TS_ASSERT_THROWS_ANYTHING(mm.copyFrom(m));
      mm << y;
      TS_GUM_ASSERT_THROWS_NOTHING(mm.copyFrom(m));

      gum::Instantiation j(mm);
      i.chgVal(a, 0).chgVal(b, 2).chgVal(c, 2);
      j.chgVal(x, 0).chgVal(y, 2).chgVal(z, 0);
      TS_ASSERT_EQUALS(m[i], mm[j]);
    }

    void testRegressionCopy() {
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);

      {
        gum::Potential<double> P1, P2;
        P1 << a << b << c;
        P2 << a << c;
        TS_GUM_ASSERT_THROWS_NOTHING(P2.copy(P1));
      }

      {
        gum::Potential<double> P1, P2;
        P1 << a << b << c;
        P2 << a << c;
        TS_GUM_ASSERT_THROWS_NOTHING(P1.copy(P2));
      }

      {
        gum::Potential<double> P1, P2;
        P1 << b;
        P2 << a << c;
        TS_GUM_ASSERT_THROWS_NOTHING(P1.copy(P2));
      }
    }
  };
}
