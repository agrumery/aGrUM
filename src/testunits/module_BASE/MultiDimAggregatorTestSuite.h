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


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/exceptions.h>
#include <agrum/base/multidim/aggregators/amplitude.h>
#include <agrum/base/multidim/aggregators/and.h>
#include <agrum/base/multidim/aggregators/count.h>
#include <agrum/base/multidim/aggregators/exists.h>
#include <agrum/base/multidim/aggregators/forall.h>
#include <agrum/base/multidim/aggregators/max.h>
#include <agrum/base/multidim/aggregators/median.h>
#include <agrum/base/multidim/aggregators/min.h>
#include <agrum/base/multidim/aggregators/or.h>
#include <agrum/base/multidim/aggregators/sum.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/base/variables/rangeVariable.h>
#include <agrum/BN/BayesNet.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  MultiDimAggregrators
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct MultiDimAggregratorsTestSuite {
    private:
    static float _is_min_(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d) {
      gum::Idx tmp = b;

      if (tmp > c) tmp = c;

      if (tmp > d) tmp = d;

      return (tmp == a) ? static_cast< float >(1) : static_cast< float >(0);
    }   // namespace gum_tests

    static float _is_max_(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d) {
      gum::Idx tmp;
      tmp = b;

      if (tmp < c) tmp = c;

      if (tmp < d) tmp = d;

      return (tmp == a) ? static_cast< float >(1) : static_cast< float >(0);
    }

    static float _is_median3_(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d) {
      gum::Idx tmp = 0;

      if (b <= c) {
        if (c <= d) {
          tmp = c;     // b<=c<=d
        } else {
          if (b <= d) {
            tmp = d;   // b<=d<c
          } else {
            tmp = b;   // d<b<c
          }
        }
      } else {         // b>c
        if (c >= d) {
          tmp = c;     // b>c>=d
        } else {       // d>b
          if (b >= d) {
            tmp = d;   // b>=d>c
          } else {
            tmp = b;   // c<b<d
          }
        }
      }

      return (tmp == a) ? static_cast< float >(1) : static_cast< float >(0);
    }

    static float _is_median4_(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d, gum::Idx e) {
      gum::Idx tmp = 0;
      gum::Idx l1, h1, l2, h2, m1, m2;
      // gum::Idx hh, ll;

      if (b < c) {
        l1 = b;
        h1 = c;
      } else {
        l1 = c;
        h1 = b;
      }

      if (d < e) {
        l2 = d;
        h2 = e;
      } else {
        l2 = e;
        h2 = d;
      }

      if (l1 < l2) {   // we do not keep ll : lowest value
        // ll = l1;
        m1 = l2;
      } else {
        // ll = l2;
        m1 = l1;
      }

      if (h1 > h2) {   // we do not keep hh : highest value
        // hh = h1;
        m2 = h2;
      } else {
        // hh = h2;
        m2 = h1;
      }

      if (m1 > m2) {   // (ll,m2,m1,hh)
        tmp = m2;
        m2  = m1;
        m1  = tmp;
      }

      // the order is now ll<=m1<=m2<=hh
      tmp = (m2 + m1) / 2;
      return (tmp == a) ? static_cast< float >(1) : static_cast< float >(0);
    }

    static float _is_amplitude_(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d, gum::Idx e) {
      gum::Idx tmp = 0;
      gum::Idx l1, h1, l2, h2;
      // gum::Idx m1, m2;
      gum::Idx hh, ll;

      if (b < c) {
        l1 = b;
        h1 = c;
      } else {
        l1 = c;
        h1 = b;
      }

      if (d < e) {
        l2 = d;
        h2 = e;
      } else {
        l2 = e;
        h2 = d;
      }

      if (l1 < l2) {   // we do not keep median values
        ll = l1;
        // m1 = l2;
      } else {
        ll = l2;
        // m1 = l1;
      }

      if (h1 > h2) {   // we do not keep median valeus
        hh = h1;
        // m2 = h2;
      } else {
        hh = h2;
        // m2 = h1;
      }

      // the order is now ll<=(m1<=m2)<=hh
      tmp = hh - ll;
      return (tmp == a) ? static_cast< float >(1) : static_cast< float >(0);
    }

    static float _is_count_2_(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d) {
      gum::Idx tmp;
      tmp = static_cast< gum::Idx >(0);

      if (b == static_cast< gum::Idx >(2)) tmp++;

      if (c == static_cast< gum::Idx >(2)) tmp++;

      if (d == static_cast< gum::Idx >(2)) tmp++;

      return (tmp == a) ? static_cast< float >(1) : static_cast< float >(0);
    }

    static float _is_forall_2_(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d) {
      gum::Idx tmp;

      tmp = ((b == static_cast< gum::Idx >(2)) && (c == static_cast< gum::Idx >(2))
             && (d == static_cast< gum::Idx >(2)))
              ? static_cast< gum::Idx >(1)
              : static_cast< gum::Idx >(0);

      return (tmp == a) ? static_cast< float >(1) : static_cast< float >(0);
    }

    static float _is_exists_2_(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d) {
      gum::Idx tmp;

      tmp = ((b == static_cast< gum::Idx >(2)) || (c == static_cast< gum::Idx >(2))
             || (d == static_cast< gum::Idx >(2)))
              ? static_cast< gum::Idx >(1)
              : static_cast< gum::Idx >(0);

      return (tmp == a) ? static_cast< float >(1) : static_cast< float >(0);
    }

    static float _is_or_(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d) {
      gum::Idx tmp;

      tmp = ((b == static_cast< gum::Idx >(1)) || (c == static_cast< gum::Idx >(1))
             || (d == static_cast< gum::Idx >(1)))
              ? static_cast< gum::Idx >(1)
              : static_cast< gum::Idx >(0);

      return (tmp == a) ? static_cast< float >(1) : static_cast< float >(0);
    }

    static float _is_and_(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d) {
      gum::Idx tmp;

      tmp = ((b == static_cast< gum::Idx >(1)) && (c == static_cast< gum::Idx >(1))
             && (d == static_cast< gum::Idx >(1)))
              ? static_cast< gum::Idx >(1)
              : static_cast< gum::Idx >(0);

      return (tmp == a) ? static_cast< float >(1) : static_cast< float >(0);
    }

    static float _is_sum_(gum::Idx a, gum::Idx b, gum::Idx c, gum::Idx d, gum::Idx maxA) {
      gum::Idx tmp = b + c + d;
      if (tmp > maxA) { tmp = maxA; }
      return (tmp == a) ? static_cast< float >(1) : static_cast< float >(0);
    }

    public:
    static void testCreationMin() {
      gum::RangeVariable a("a", "", 0, 3), b("b", "", 0, 3), c("c", "", 0, 3), d("d", "", 0, 3);
      gum::aggregator::Min< double > p;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p << a << b << c << d);
      CHECK((p.toString()) == ("a:Range([0,3])=min(b:Range([0,3]),c:Range([0,3]),d:Range([0,3]))"));

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        CHECK((p[i]) == (_is_min_(i.val(a), i.val(b), i.val(c), i.val(d))));
      }
    }

    static void testCreationMax() {
      gum::RangeVariable a("a", "", 0, 3), b("b", "", 0, 3), c("c", "", 0, 3), d("d", "", 0, 3);
      gum::aggregator::Max< double > p;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p << a << b << c << d);
      CHECK((p.toString()) == ("a:Range([0,3])=max(b:Range([0,3]),c:Range([0,3]),d:Range([0,3]))"));

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        CHECK((p[i]) == (_is_max_(i.val(a), i.val(b), i.val(c), i.val(d))));
      }
    }

    static void testCreationCount() {
      gum::RangeVariable a("a", "", 0, 3), b("b", "", 0, 3), c("c", "", 0, 3), d("d", "", 0, 3);
      gum::aggregator::Count< double > p(static_cast< gum::Idx >(2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(p << a << b << c << d);
      CHECK((p.toString())
            == ("a:Range([0,3])=count[2](b:Range([0,3]),c:Range([0,3]),d:Range([0,3]))"));

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        CHECK((p[i]) == (_is_count_2_(i.val(a), i.val(b), i.val(c), i.val(d))));
      }
    }

    static void testCreationForall() {
      gum::RangeVariable a("a", "", 0, 3), b("b", "", 0, 3), c("c", "", 0, 3), d("d", "", 0, 3);
      gum::aggregator::Forall< double > p(static_cast< gum::Idx >(2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(p << a << b << c << d);
      CHECK((p.toString())
            == ("a:Range([0,3])=forall[2](b:Range([0,3]),c:Range([0,3]),d:Range([0,3]))"));

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        CHECK((p[i]) == (_is_forall_2_(i.val(a), i.val(b), i.val(c), i.val(d))));
      }
    }

    static void testCreationExists() {
      gum::RangeVariable a("a", "", 0, 3), b("b", "", 0, 3), c("c", "", 0, 3), d("d", "", 0, 3);
      gum::aggregator::Exists< double > p(static_cast< gum::Idx >(2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(p << a << b << c << d);
      CHECK((p.toString())
            == ("a:Range([0,3])=exists[2](b:Range([0,3]),c:Range([0,3]),d:Range([0,3]))"));

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        CHECK((p[i]) == (_is_exists_2_(i.val(a), i.val(b), i.val(c), i.val(d))));
      }
    }

    static void testCreationOR() {
      gum::LabelizedVariable        a("a", "", 2), b("b", "", 4), c("c", "", 2), d("d", "", 2);
      gum::aggregator::Or< double > p;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p << a << b << c << d);
      CHECK((p.toString())
            == ("a:Labelized({0|1})=or(b:Labelized({0|1|2|3}),c:Labelized({0|1}),d:Labelized({0|1})"
                ")"));

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        CHECK((p[i]) == (_is_or_(i.val(a), i.val(b), i.val(c), i.val(d))));
      }
    }

    static void testCreationAND() {
      gum::LabelizedVariable         a("a", "", 2), b("b", "", 4), c("c", "", 2), d("d", "", 2);
      gum::aggregator::And< double > p;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p << a << b << c << d);
      CHECK((p.toString())
            == ("a:Labelized({0|1})=and(b:Labelized({0|1|2|3}),c:Labelized({0|1}),d:Labelized({0|1}"
                "))"));

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        CHECK((p[i]) == (_is_and_(i.val(a), i.val(b), i.val(c), i.val(d))));
      }
    }

    static void testCreationMedian3() {
      gum::LabelizedVariable            a("a", "", 4), b("b", "", 4), c("c", "", 4), d("d", "", 4);
      gum::aggregator::Median< double > p;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p << a << b << c << d);
      CHECK((p.toString())
            == ("a:Labelized({0|1|2|3})=median(b:Labelized({0|1|2|3}),c:Labelized({0|1|2|3})"
                ",d:Labelized({0|1|2|3}))"));

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        CHECK((_is_median3_(i.val(a), i.val(b), i.val(c), i.val(d))) == (p[i]));
      }
    }

    static void testCreationMedian4() {
      gum::LabelizedVariable a("a", "", 4), b("b", "", 4), c("c", "", 4), d("d", "", 4),
          e("e", "", 4);
      gum::aggregator::Median< double > p;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p << a << b << c << d << e);
      CHECK((p.toString())
            == ("a:Labelized({0|1|2|3})=median(b:Labelized({0|1|2|3}),c:Labelized({0|1|2|3})"
                ",d:Labelized({0|1|2|3}),e:Labelized({0|1|2|3}))"));

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        CHECK((_is_median4_(i.val(a), i.val(b), i.val(c), i.val(d), i.val(e))) == (p[i]));
      }
    }

    static void testCreationAmplitude() {
      gum::LabelizedVariable a("a", "", 4), b("b", "", 4), c("c", "", 4), d("d", "", 4),
          e("e", "", 4);
      gum::aggregator::Amplitude< double > p;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p << a << b << c << d << e);
      CHECK((p.toString())
            == ("a:Labelized({0|1|2|3})=amplitude(b:Labelized({0|1|2|3}),c:Labelized({0|1|2|"
                "3}),d:Labelized({0|1|2|3}),e:Labelized({0|1|2|3}))"));

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        CHECK((_is_amplitude_(i.val(a), i.val(b), i.val(c), i.val(d), i.val(e))) == (p[i]));
      }
    }

    static void testTensorMin() {
      gum::RangeVariable a("a", "", 0, 3), b("b", "", 0, 3), c("c", "", 0, 3), d("d", "", 0, 3);

      gum::Tensor< int > p(new gum::aggregator::Min< int >());
      GUM_CHECK_ASSERT_THROWS_NOTHING(p << a << b << c << d);

      gum::Instantiation i(p);

      for (i.setFirst(); !i.end(); ++i) {
        CHECK((p[i]) == (_is_min_(i.val(a), i.val(b), i.val(c), i.val(d))));
      }

      // it is not allowed to change a value but can only be detected at the
      // next
      // access
      i.setFirst();

      CHECK_THROWS_AS(p.set(i, 3), const gum::OperationNotAllowed&);

      CHECK_THROWS_AS(p.fill(0), const gum::OperationNotAllowed&);
    }

    static void testCreationSum() {
      try {
        gum::RangeVariable a("a", "", 0, 8), b("b", "", 0, 3), c("c", "", 0, 3), d("d", "", 0, 3);
        gum::aggregator::Sum< double > p;
        GUM_CHECK_ASSERT_THROWS_NOTHING(p << a << b << c << d);
        CHECK((p.toString())
              == ("a:Range([0,8])=sum(b:Range([0,3]),c:Range([0,3]),d:Range([0,3]))"));

        gum::Instantiation i(p);

        for (i.setFirst(); !i.end(); ++i) {
          CHECK((p[i])
                == (_is_sum_(i.val(a), i.val(b), i.val(c), i.val(d), static_cast< gum::Idx >(8))));
        }
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    private:
    static std::string pot2arr_(const gum::aggregator::MultiDimAggregator< double >& p) {
      std::stringstream v;
      bool              first = true;

      gum::Instantiation Ind(p);
      for (Ind.setFirst(); !Ind.end(); ++Ind) {
        if (!first) v << "-";
        else first = false;

        v << p.get(Ind);
      }

      return v.str();
    }

    public:
    static void testOr_ZeroParent() {
      gum::LabelizedVariable a("a", "", 2), b("b", "", 2);

      std::string                   res0 = "1-0";
      std::string                   res1 = "1-0-0-1";
      gum::aggregator::Or< double > p;

      gum::Instantiation ind(p);
      std::string        s;

      CHECK_THROWS_AS(p.get(ind), const gum::NotFound&);

      p << a;
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res0));

      p << b;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p.toString());
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res1));
    }

    static void testAnd_ZeroParent() {
      gum::LabelizedVariable a("a", "", 2), b("b", "", 2);

      std::string                    res0 = "0-1";
      std::string                    res1 = "1-0-0-1";
      gum::aggregator::And< double > p;

      gum::Instantiation ind(p);
      std::string        s;

      CHECK_THROWS_AS(p.get(ind), const gum::NotFound&);

      p << a;
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res0));

      p << b;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p.toString());
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res1));
    }

    static void testExists_ZeroParent() {
      gum::LabelizedVariable a("a", "", 2), b("b", "", 4);

      std::string                       res0 = "1-0";
      std::string                       res1 = "1-0-1-0-0-1-1-0";
      gum::aggregator::Exists< double > p(2);

      gum::Instantiation ind(p);
      std::string        s;

      CHECK_THROWS_AS(p.get(ind), const gum::NotFound&);

      p << a;
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res0));

      p << b;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p.toString());
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res1));
    }

    static void testForall_ZeroParent() {
      gum::LabelizedVariable a("a", "", 2), b("b", "", 4);

      std::string                       res0 = "0-1";
      std::string                       res1 = "1-0-1-0-0-1-1-0";
      gum::aggregator::Forall< double > p(2);

      gum::Instantiation ind(p);
      std::string        s;

      CHECK_THROWS_AS(p.get(ind), const gum::NotFound&);

      p << a;
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res0));

      p << b;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p.toString());
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res1));
    }

    static void testMin_ZeroParent() {
      gum::LabelizedVariable a("a", "", 4), b("b", "", 4);

      std::string                    res0 = "0-0-0-1";   // min of zero value is +infinity
      std::string                    res1 = "1-0-0-0-0-1-0-0-0-0-1-0-0-0-0-1";
      gum::aggregator::Min< double > p;

      gum::Instantiation ind(p);
      std::string        s;

      CHECK_THROWS_AS(p.get(ind), const gum::NotFound&);

      p << a;
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res0));

      p << b;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p.toString());
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res1));
    }

    static void testMax_ZeroParent() {
      gum::LabelizedVariable a("a", "", 4), b("b", "", 4);

      std::string                    res0 = "1-0-0-0";   // max of zero value is -infinity
      std::string                    res1 = "1-0-0-0-0-1-0-0-0-0-1-0-0-0-0-1";
      gum::aggregator::Max< double > p;

      gum::Instantiation ind(p);
      std::string        s;

      CHECK_THROWS_AS(p.get(ind), const gum::NotFound&);

      p << a;
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res0));

      p << b;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p.toString());
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res1));
    }

    static void testCount_ZeroParent() {
      gum::LabelizedVariable a("a", "", 4), b("b", "", 2);

      std::string                      res0 = "1-0-0-0";
      std::string                      res1 = "1-0-0-0-0-1-0-0";
      gum::aggregator::Count< double > p(1);

      gum::Instantiation ind(p);
      std::string        s;

      CHECK_THROWS_AS(p.get(ind), const gum::NotFound&);

      p << a;
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res0));

      p << b;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p.toString());
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res1));
    }

    static void testAmplitude_ZeroParent() {
      gum::LabelizedVariable a("a", "", 4), b("b", "", 2);

      std::string                          res0 = "1-0-0-0";
      std::string                          res1 = "1-0-0-0-1-0-0-0";
      gum::aggregator::Amplitude< double > p;

      gum::Instantiation ind(p);
      std::string        s;

      CHECK_THROWS_AS(p.get(ind), const gum::NotFound&);

      p << a;
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res0));

      p << b;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p.toString());
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res1));
    }

    static void testMedian_ZeroParent() {
      gum::LabelizedVariable a("a", "", 4), b("b", "", 2);

      std::string                       res0 = "1-0-0-0";
      std::string                       res1 = "1-0-0-0-0-1-0-0";
      gum::aggregator::Median< double > p;

      gum::Instantiation ind(p);
      std::string        s;

      CHECK_THROWS_AS(p.get(ind), const gum::NotFound&);

      p << a;
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res0));

      p << b;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p.toString());
      GUM_CHECK_ASSERT_THROWS_NOTHING(s = pot2arr_(p));
      CHECK((s) == (res1));
    }
  };

  GUM_TEST_ACTIF(CreationMin)
  GUM_TEST_ACTIF(CreationMax)
  GUM_TEST_ACTIF(CreationCount)
  GUM_TEST_ACTIF(CreationForall)
  GUM_TEST_ACTIF(CreationExists)
  GUM_TEST_ACTIF(CreationOR)
  GUM_TEST_ACTIF(CreationAND)
  GUM_TEST_ACTIF(CreationMedian3)
  GUM_TEST_ACTIF(CreationMedian4)
  GUM_TEST_ACTIF(CreationAmplitude)
  GUM_TEST_ACTIF(TensorMin)
  GUM_TEST_ACTIF(CreationSum)
  GUM_TEST_ACTIF(Or_ZeroParent)
  GUM_TEST_ACTIF(And_ZeroParent)
  GUM_TEST_ACTIF(Exists_ZeroParent)
  GUM_TEST_ACTIF(Forall_ZeroParent)
  GUM_TEST_ACTIF(Min_ZeroParent)
  GUM_TEST_ACTIF(Max_ZeroParent)
  GUM_TEST_ACTIF(Count_ZeroParent)
  GUM_TEST_ACTIF(Amplitude_ZeroParent)
  GUM_TEST_ACTIF(Median_ZeroParent)
}   // namespace gum_tests
