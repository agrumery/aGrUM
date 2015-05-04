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
#include <agrum/multidim/utilityTable.h>

namespace gum_tests {

  class UtilityTestSuite : public CxxTest::TestSuite {
    public:
    void testCreation() {
      gum::UtilityTable<double> p(new gum::MultiDimArray<double>());
      TS_ASSERT(p.empty());

      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5);
      TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c);
    }

    void testSumBy() {
      gum::UtilityTable<double> p1(new gum::MultiDimArray<double>());
      gum::UtilityTable<double> p2(new gum::MultiDimArray<double>());
      gum::UtilityTable<double> p3(new gum::MultiDimArray<double>());
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 2),
          c("c", "third var", 2), d("d", "fourth var", 2);
      p1 << c << a;
      p2 << d << b;
      p3 << a << b;

      gum::Instantiation i;
      i << a << b << c << d;

      for (i.setFirst(); !i.end(); ++i) {
        p1.set(i, i.val(c));
        p2.set(i, i.val(d));
        p3.set(i, i.val(a));
      }

      p1.sumBy(p2);

      p1.sumBy(p3);

      gum::Instantiation j(p1);

      int nbr_err = 0;

      for (j.setFirst(); !j.end(); ++j)
        if (p1[j] != j.val(c) + j.val(a) + j.val(d)) {
          nbr_err++;
        }

      TS_ASSERT_EQUALS(nbr_err, 0);
    }

    void testSum() {
      gum::UtilityTable<double> p1(new gum::MultiDimArray<double>());
      gum::UtilityTable<double> p2(new gum::MultiDimArray<double>());
      gum::UtilityTable<double> p3(new gum::MultiDimArray<double>());
      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4),
          c("c", "third var", 5), d("d", "fourth var", 5);

      try {
        p1 << c;
        p2 << b << c << d;
        p3 << a << d;

        gum::UtilityTable<double> res;

        gum::Instantiation i;
        i << a << b << c << d;

        for (i.setFirst(); !i.end(); ++i) {
          p1.set(i, i.val(c));
          p2.set(i, i.val(d));
          p3.set(i, i.val(a));
        }

        res.sum(p1, p2);

        TS_ASSERT_EQUALS(res.nbrDim(), (gum::Size)3);

        gum::Instantiation j2(res);

        TS_ASSERT_EQUALS(j2.toString(), "<c:0|b:0|d:0>");

        int nbr_err = 0;

        gum::UtilityTable<double> res2;

        gum::Instantiation j(res2);
        res2.sum(res, p3);

        for (j.setFirst(); !j.end(); ++j)
          if (res2[j] != j.val(c) + j.val(a) + j.val(d)) {

            nbr_err++;
          }

        TS_ASSERT_EQUALS(nbr_err, 0);

      } catch (gum::Exception e) {
        GUM_SHOWERROR(e);
      }
    }

    void testFactory() {
      gum::UtilityTable<double> p1(new gum::MultiDimArray<double>());
      TS_GUM_ASSERT_THROWS_NOTHING(delete p1.newFactory());
    }
  };
}
