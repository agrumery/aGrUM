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
#include <sstream>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/variables/rangeVariable.h>
#include <agrum/multidim/potential.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/GibbsInference.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>

#include <agrum/multidim/aggregators/min.h>
#include <agrum/multidim/aggregators/max.h>
#include <agrum/multidim/aggregators/count.h>
#include <agrum/multidim/aggregators/forall.h>
#include <agrum/multidim/aggregators/exists.h>
#include <agrum/multidim/aggregators/or.h>
#include <agrum/multidim/aggregators/and.h>

namespace gum_tests {

  class MultiDimAggregratorsTestSuite : public CxxTest::TestSuite {
    private:
    float __is_min(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d) {
      gum::Idx tmp;
      tmp = b;

      if (tmp > c)
        tmp = c;

      if (tmp > d)
        tmp = d;

      return (tmp == a) ? (float)1 : (float)0;
    }

    float __is_max(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d) {
      gum::Idx tmp;
      tmp = b;

      if (tmp < c)
        tmp = c;

      if (tmp < d)
        tmp = d;

      return (tmp == a) ? (float)1 : (float)0;
    }

    float __is_count_2(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d) {
      gum::Idx tmp;
      tmp = (gum::Idx)0;

      if (b == (gum::Idx)2)
        tmp++;

      if (c == (gum::Idx)2)
        tmp++;

      if (d == (gum::Idx)2)
        tmp++;

      return (tmp == a) ? (float)1 : (float)0;
    }

    float __is_forall_2(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d) {
      gum::Idx tmp;

      tmp = ((b == (gum::Idx)2) && (c == (gum::Idx)2) && (d == (gum::Idx)2))
                ? (gum::Idx)1
                : (gum::Idx)0;

      return (tmp == a) ? (float)1 : (float)0;
    }

    float __is_exists_2(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d) {
      gum::Idx tmp;

      tmp = ((b == (gum::Idx)2) || (c == (gum::Idx)2) || (d == (gum::Idx)2))
                ? (gum::Idx)1
                : (gum::Idx)0;

      return (tmp == a) ? (float)1 : (float)0;
    }

    float __is_or(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d) {
      gum::Idx tmp;

      tmp = ((b == (gum::Idx)1) || (c == (gum::Idx)1) || (d == (gum::Idx)1))
                ? (gum::Idx)1
                : (gum::Idx)0;

      return (tmp == a) ? (float)1 : (float)0;
    }

    float __is_and(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d) {
      gum::Idx tmp;

      tmp = ((b == (gum::Idx)1) && (c == (gum::Idx)1) && (d == (gum::Idx)1))
                ? (gum::Idx)1
                : (gum::Idx)0;

      return (tmp == a) ? (float)1 : (float)0;
    }

    public:
    void testCreationMin() {
      gum::RangeVariable a("a", "", 0, 3), b("b", "", 0, 3), c("c", "", 0, 3),
          d("d", "", 0, 3);
      gum::aggregator::Min<float> p;
      TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c << d);
      TS_ASSERT_EQUALS(p.toString(), "a[0-3]=min(b[0-3],c[0-3],d[0-3])");

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        TS_ASSERT_EQUALS(p[i], __is_min(i.val(a), i.val(b), i.val(c), i.val(d)));
      }
    }

    void testCreationMax() {
      gum::RangeVariable a("a", "", 0, 3), b("b", "", 0, 3), c("c", "", 0, 3),
          d("d", "", 0, 3);
      gum::aggregator::Max<float> p;
      TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c << d);
      TS_ASSERT_EQUALS(p.toString(), "a[0-3]=max(b[0-3],c[0-3],d[0-3])");

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        TS_ASSERT_EQUALS(p[i], __is_max(i.val(a), i.val(b), i.val(c), i.val(d)));
      }
    }

    void testCreationCount() {
      gum::RangeVariable a("a", "", 0, 3), b("b", "", 0, 3), c("c", "", 0, 3),
          d("d", "", 0, 3);
      gum::aggregator::Count<float> p((gum::Idx)2);
      TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c << d);
      TS_ASSERT_EQUALS(p.toString(), "a[0-3]=count[2](b[0-3],c[0-3],d[0-3])");

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        TS_ASSERT_EQUALS(p[i], __is_count_2(i.val(a), i.val(b), i.val(c), i.val(d)));
      }
    }

    void testCreationForall() {
      gum::RangeVariable a("a", "", 0, 3), b("b", "", 0, 3), c("c", "", 0, 3),
          d("d", "", 0, 3);
      gum::aggregator::Forall<float> p((gum::Idx)2);
      TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c << d);
      TS_ASSERT_EQUALS(p.toString(), "a[0-3]=forall[2](b[0-3],c[0-3],d[0-3])");

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        TS_ASSERT_EQUALS(p[i],
                         __is_forall_2(i.val(a), i.val(b), i.val(c), i.val(d)));
      }
    }

    void testCreationExists() {
      gum::RangeVariable a("a", "", 0, 3), b("b", "", 0, 3), c("c", "", 0, 3),
          d("d", "", 0, 3);
      gum::aggregator::Exists<float> p((gum::Idx)2);
      TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c << d);
      TS_ASSERT_EQUALS(p.toString(), "a[0-3]=exists[2](b[0-3],c[0-3],d[0-3])");

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        TS_ASSERT_EQUALS(p[i],
                         __is_exists_2(i.val(a), i.val(b), i.val(c), i.val(d)));
      }
    }

    void testCreationOR() {
      gum::LabelizedVariable a("a", "", 2), b("b", "", 4), c("c", "", 2),
          d("d", "", 2);
      gum::aggregator::Or<float> p;
      TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c << d);
      TS_ASSERT_EQUALS(p.toString(), "a<0,1>=or(b<0,1,2,3>,c<0,1>,d<0,1>)");

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        TS_ASSERT_EQUALS(p[i], __is_or(i.val(a), i.val(b), i.val(c), i.val(d)));
      }
    }

    void testCreationAND() {
      gum::LabelizedVariable a("a", "", 2), b("b", "", 4), c("c", "", 2),
          d("d", "", 2);
      gum::aggregator::And<float> p;
      TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c << d);
      TS_ASSERT_EQUALS(p.toString(), "a<0,1>=and(b<0,1,2,3>,c<0,1>,d<0,1>)");

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        TS_ASSERT_EQUALS(p[i], __is_and(i.val(a), i.val(b), i.val(c), i.val(d)));
      }
    }

    void testPotentialMin() {
      gum::RangeVariable a("a", "", 0, 3), b("b", "", 0, 3), c("c", "", 0, 3),
          d("d", "", 0, 3);

      gum::Potential<int> p(new gum::aggregator::Min<int>());
      TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c << d);

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        TS_ASSERT_EQUALS(p[i], __is_min(i.val(a), i.val(b), i.val(c), i.val(d)));
      }

      // it is not allowed to change a value but can only be detected at the next
      // access
      i.setFirst();

      TS_ASSERT_THROWS(p.set(i, 3), gum::OperationNotAllowed);

      TS_ASSERT_THROWS(p.fill(0), gum::OperationNotAllowed);
    }

    void testBNwithMin() {
      gum::List<gum::NodeId> idList;
      gum::BayesNet<double> bn;

      int nbr = 6; // nbr=nbr of parents

      std::string str;
      const double t[4] = {0.1, 0.1, 0.1, 0.7};
      int n = 4;
      const std::vector<double> v(t, t + n);

      for (int i = 0; i <= nbr; i++) {
        str = "";
        std::stringstream ss;
        ss << "var" << (i + 1);
        ss >> str;

        gum::LabelizedVariable var(str, str, 4);

        if (i == 0) {
          idList.insert(bn.add(var, new gum::aggregator::Min<double>()));
        } else {
          idList.insert(bn.add(var));
          bn.addArc(idList[i], idList[0]);
          bn.cpt(idList[i]).fillWith(v);

          gum::Instantiation i(bn.cpt(idList[0]));

          gum::Idx res;

          for (i.setFirst(); !i.end(); ++i) {
            res = (gum::Idx)10000; // clearly arbitraty choosen

            for (gum::Idx j = 1; j < i.nbrDim(); j++) {
              if (res > i.val(j))
                res = i.val(j);
            }

            TS_ASSERT_EQUALS(bn.cpt(idList[0])[i],
                             (res == i.val((gum::Idx)0)) ? (float)1 : (float)0);
          }
        }
      }

      {
        gum::ShaferShenoyInference<double> inf(bn);

        try {
          // Testing the inference
          inf.makeInference();
        } catch (gum::Exception e) {
          TS_ASSERT(false);
        }

        try {
          TS_ASSERT_EQUALS(inf.posterior(idList[0]).toString(),
                           "<var1:0> :: 0.468559 /<var1:1> :: 0.269297 /<var1:2> :: "
                           "0.144495 /<var1:3> :: 0.117649");
          TS_ASSERT_EQUALS(
              inf.posterior(idList[1]).toString(),
              "<var2:0> :: 0.1 /<var2:1> :: 0.1 /<var2:2> :: 0.1 /<var2:3> :: 0.7");
        } catch (gum::Exception e) {
          TS_ASSERT(false);
        }
      }

      {
        gum::LazyPropagation<double> inf(bn);

        try {
          // Testing the inference
          inf.makeInference();
        } catch (gum::Exception e) {
          TS_ASSERT(false);
        }

        try {
          TS_ASSERT_EQUALS(inf.posterior(idList[0]).toString(),
                           "<var1:0> :: 0.468559 /<var1:1> :: 0.269297 /<var1:2> :: "
                           "0.144495 /<var1:3> :: 0.117649");
          TS_ASSERT_EQUALS(
              inf.posterior(idList[1]).toString(),
              "<var2:0> :: 0.1 /<var2:1> :: 0.1 /<var2:2> :: 0.1 /<var2:3> :: 0.7");
        } catch (gum::Exception e) {
          TS_ASSERT(false);
        }
      }
    }
  };
}
