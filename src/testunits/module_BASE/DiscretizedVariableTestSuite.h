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
#include <sstream>
#include <string>

#include <agrum/base/core/list.h>
#include <agrum/base/variables/discretizedVariable.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct DiscretizedVariableTestSuite {
    public:
    // namespace gum_tests


    // regression test for HIGH-14: copy ctor/assign did not copy _is_empirical
  };

  GUM_TEST(Creation) {
    GUM_CHECK_ASSERT_THROWS_NOTHING(gum::DiscretizedVariable< int > v("var", "a var"));
  }

  GUM_TEST(DiscreteVariableProperties) {
    gum::DiscretizedVariable< int > v("var", "a var");
    CHECK_EQ(v.name(), "var");
    v.setName("toto");
    CHECK_EQ(v.name(), "toto");

    CHECK_EQ(v.description(), "a var");
    v.setDescription("toto");
    CHECK_EQ(v.description(), "toto");

    CHECK_EQ(v.varType(), gum::VarType::DISCRETIZED);

    const gum::DiscretizedVariable< int >& w = v;
    w.setDescription("Lol");   // change description does not change a variable
  }

  GUM_TEST(ConstructorWithTicks) {
    gum::DiscretizedVariable< double > d("d", "Discretized variable", {3.1, 2.0, 4.0});
    GUM_CHECK_ASSERT_THROWS_NOTHING(d["2.5"]);
    CHECK_THROWS_AS(d["0.5"], const gum::OutOfBounds&);
    CHECK_THROWS_AS(d["4.5"], const gum::OutOfBounds&);

    CHECK_EQ(d.numerical(0), (2.0 + 3.1) / 2);
    CHECK_EQ(d.numerical(1), (4.0 + 3.1) / 2);

    CHECK_THROWS_AS(d.addTick(-std::numeric_limits< double >::infinity()),
                    const gum::DefaultInLabel&);
    CHECK_THROWS_AS(d.addTick(std::numeric_limits< double >::infinity()),
                    const gum::DefaultInLabel&);

    CHECK_THROWS_AS(d["0.5"], const gum::OutOfBounds&);
    CHECK_THROWS_AS(d["4.5"], const gum::OutOfBounds&);

    d.setEmpirical(true);

    GUM_CHECK_ASSERT_THROWS_NOTHING(d["0.5"]);
    GUM_CHECK_ASSERT_THROWS_NOTHING(d["4.5"]);
  }

  GUM_TEST(AccessorWithIntervallsForGaspard) {
    gum::DiscretizedVariable< double > d("d", "Discretized variable", {3.1, 2.0, 4.0, 10.0});

    GUM_CHECK_ASSERT_THROWS_NOTHING(d["[2.0,3.1]"]);
    GUM_CHECK_ASSERT_THROWS_NOTHING(d["[3.1,4.0]"]);
    GUM_CHECK_ASSERT_THROWS_NOTHING(d["[4.0,10]"]);

    GUM_CHECK_ASSERT_THROWS_NOTHING(d["[2.0;3.1]"]);
    GUM_CHECK_ASSERT_THROWS_NOTHING(d["[3.1;4.0]"]);
    GUM_CHECK_ASSERT_THROWS_NOTHING(d["[4.0;10]"]);

    CHECK_THROWS_AS(d["x10y20.0z"], const gum::NotFound&);
    CHECK_THROWS_AS(d["[10,20]"], const gum::NotFound&);

    CHECK_EQ(d["[2.0,3.1]"], d["2.5"]);
    CHECK_EQ(d["[3.1,4.0]"], d["3.5"]);
    CHECK_EQ(d["[4.0,10]"], d["6.5"]);

    d.setEmpirical(true);
    GUM_CHECK_ASSERT_THROWS_NOTHING(d["[2.0,3.1]"]);
    GUM_CHECK_ASSERT_THROWS_NOTHING(d["[3.1,4.0]"]);
    GUM_CHECK_ASSERT_THROWS_NOTHING(d["[4.0,10]"]);

    CHECK_EQ(d["[2.0,3.1]"], d["2.5"]);
    CHECK_EQ(d["[3.1,4.0]"], d["3.5"]);
    CHECK_EQ(d["[4.0,10]"], d["6.5"]);
  }

  GUM_TEST(AddTicks) {
    gum::DiscretizedVariable< int > v("var", "a var");

    CHECK_THROWS_AS(v.tick(static_cast< gum::Idx >(1)), const gum::OutOfBounds&);

    CHECK(v.empty());
    CHECK_EQ(v.domainSize(), static_cast< gum::Idx >(0));
    v.addTick(1);
    CHECK(v.empty());
    CHECK_EQ(v.domainSize(), static_cast< gum::Idx >(0));
    v.addTick(3);
    CHECK(v.empty());
    CHECK_EQ(v.domainSize(), static_cast< gum::Idx >(1));
    v.addTick(2);
    CHECK(!v.empty());
    CHECK_EQ(v.domainSize(), static_cast< gum::Idx >(2));

    CHECK_THROWS_AS(v.addTick(2), const gum::DefaultInLabel&);

    std::stringstream s;
    s << v;
    CHECK_EQ(s.str(), "var:Discretized(<[1;2[,[2;3]>)");
    CHECK_EQ(v.toString(), "var:Discretized(<[1;2[,[2;3]>)");

    CHECK_EQ(v.tick(static_cast< gum::Idx >(0)), 1);
    CHECK_EQ(v.tick(static_cast< gum::Idx >(1)), 2);
    CHECK_EQ(v.tick(static_cast< gum::Idx >(2)), 3);

    CHECK_THROWS_AS(v.tick(static_cast< gum::Idx >(3)), const gum::OutOfBounds&);
  }

  GUM_TEST(NormalLimits) {
    gum::DiscretizedVariable< unsigned int > v("var", "a var");
    v.addTick(1).addTick(5).addTick(3).addTick(7);

    CHECK_EQ(v.toString(), "var:Discretized(<[1;3[,[3;5[,[5;7]>)");

    gum::Size vv = static_cast< gum::Size >(0);

    CHECK_THROWS_AS(v["0"], const gum::OutOfBounds&);

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["1"]);
    CHECK_EQ(vv, static_cast< gum::Size >(0));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["2"]);
    CHECK_EQ(vv, static_cast< gum::Size >(0));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["3"]);
    CHECK_EQ(vv, static_cast< gum::Size >(1));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["4"]);
    CHECK_EQ(vv, static_cast< gum::Size >(1));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["5"]);
    CHECK_EQ(vv, static_cast< gum::Size >(2));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["6"]);
    CHECK_EQ(vv, static_cast< gum::Size >(2));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["7"]);
    CHECK_EQ(vv, static_cast< gum::Size >(2));

    CHECK_THROWS_AS(v["8"], const gum::OutOfBounds&);
  }

  GUM_TEST(NormalLimitsWithEmpirical) {
    gum::DiscretizedVariable< unsigned int > v("var", "a var");
    v.addTick(1).addTick(5).addTick(3).addTick(7);

    v.setEmpirical(true);

    CHECK_EQ(v.toString(), "var:Discretized(<(1;3[,[3;5[,[5;7)>)");

    gum::Size vv = static_cast< gum::Size >(0);

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["0"]);
    CHECK_EQ(vv, static_cast< gum::Size >(0));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["1"]);
    CHECK_EQ(vv, static_cast< gum::Size >(0));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["2"]);
    CHECK_EQ(vv, static_cast< gum::Size >(0));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["3"]);
    CHECK_EQ(vv, static_cast< gum::Size >(1));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["4"]);
    CHECK_EQ(vv, static_cast< gum::Size >(1));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["5"]);
    CHECK_EQ(vv, static_cast< gum::Size >(2));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["6"]);
    CHECK_EQ(vv, static_cast< gum::Size >(2));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["7"]);
    CHECK_EQ(vv, static_cast< gum::Size >(2));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["8"]);
    CHECK_EQ(vv, static_cast< gum::Size >(2));
  }

  GUM_TEST(NormalLimitsWithEmpiricalInConstructor) {
    gum::DiscretizedVariable< unsigned int > v("var",
                                               "a var",
                                               {1, 3, 5, 7},
                                               true);   // empirical is true

    CHECK_EQ(v.toString(), "var:Discretized(<(1;3[,[3;5[,[5;7)>)");

    gum::Size vv = static_cast< gum::Size >(0);

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["0"]);
    CHECK_EQ(vv, static_cast< gum::Size >(0));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["1"]);
    CHECK_EQ(vv, static_cast< gum::Size >(0));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["2"]);
    CHECK_EQ(vv, static_cast< gum::Size >(0));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["3"]);
    CHECK_EQ(vv, static_cast< gum::Size >(1));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["4"]);
    CHECK_EQ(vv, static_cast< gum::Size >(1));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["5"]);
    CHECK_EQ(vv, static_cast< gum::Size >(2));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["6"]);
    CHECK_EQ(vv, static_cast< gum::Size >(2));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["7"]);
    CHECK_EQ(vv, static_cast< gum::Size >(2));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["8"]);
    CHECK_EQ(vv, static_cast< gum::Size >(2));
  }

  GUM_TEST(NormalLimitsWithNoEmpiricalInConstructor) {
    gum::DiscretizedVariable< unsigned int > v("var",
                                               "a var",
                                               {1, 3, 5, 7});   // empirical is false

    CHECK_EQ(v.toString(), "var:Discretized(<[1;3[,[3;5[,[5;7]>)");

    auto vv = static_cast< gum::Size >(0);

    CHECK_THROWS_AS(vv = v["0"], gum::OutOfBounds&);

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["1"]);
    CHECK_EQ(vv, static_cast< gum::Size >(0));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["2"]);
    CHECK_EQ(vv, static_cast< gum::Size >(0));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["3"]);
    CHECK_EQ(vv, static_cast< gum::Size >(1));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["4"]);
    CHECK_EQ(vv, static_cast< gum::Size >(1));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["5"]);
    CHECK_EQ(vv, static_cast< gum::Size >(2));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["6"]);
    CHECK_EQ(vv, static_cast< gum::Size >(2));

    GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["7"]);
    CHECK_EQ(vv, static_cast< gum::Size >(2));

    CHECK_THROWS_AS(vv = v["8"], gum::OutOfBounds&);
  }

  GUM_TEST(OrderTicks) {
    for (int i = 1; i < 7; i++)
      for (int j = 1; j < 7; j++)
        for (int k = 1; k < 7; k++)
          for (int l = 1; l < 7; l++)
            for (int m = 1; m < 7; m++)
              for (int n = 1; n < 7; n++) {
                if ((i + j + k + l + m + n == 21) && (i * j * k * l * m * n == 720)) {
                  gum::DiscretizedVariable< int > d("d", "Discretized variable");
                  d.addTick(i).addTick(j).addTick(k).addTick(l).addTick(m).addTick(n);
                  CHECK_EQ(d.domainSize(), static_cast< gum::Size >(5));
                  CHECK_EQ(d.toString(), "d:Discretized(<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>)");
                  int s = 1;
                  for (auto tick: d.ticks()) {
                    CHECK_EQ(tick, s++);
                  }
                }
              }
  }

  GUM_TEST(FloatLimits) {
    gum::DiscretizedVariable< double > d("d", "Discretized variable");
    d.addTick(3.1).addTick(2.0).addTick(4.0);

    GUM_CHECK_ASSERT_THROWS_NOTHING(d["2.5"]);
    CHECK_THROWS_AS(d["0.5"], const gum::OutOfBounds&);
    CHECK_THROWS_AS(d["4.5"], const gum::OutOfBounds&);

    d.setEmpirical(true);

    GUM_CHECK_ASSERT_THROWS_NOTHING(d["2.5"]);
    GUM_CHECK_ASSERT_THROWS_NOTHING(d["0.5"]);
    GUM_CHECK_ASSERT_THROWS_NOTHING(d["4.5"]);
  }

  GUM_TEST(FloatLimitsEmpirical) {
    gum::DiscretizedVariable< double > d("d", "Discretized variable");
    d.addTick(3.1).addTick(2.0).addTick(4.0);

    d.setEmpirical(true);

    GUM_CHECK_ASSERT_THROWS_NOTHING(d["2.5"]);
    GUM_CHECK_ASSERT_THROWS_NOTHING(d["0.5"]);
    GUM_CHECK_ASSERT_THROWS_NOTHING(d["4.5"]);
  }

  GUM_TEST(Numerical) {
    gum::DiscretizedVariable< double > d("d", "Discretized variable");
    d.addTick(3.1).addTick(2.0).addTick(4.0);

    CHECK_EQ(d.numerical(0), (2.0 + 3.1) / 2);
    CHECK_EQ(d.numerical(1), (4.0 + 3.1) / 2);
  }

  GUM_TEST(CopyEmptyVariableWithZeros) {
    gum::DiscretizedVariable< double > source("angle", "");

    auto copy = source;

    CHECK_NOTHROW(copy.addTick(0));
    CHECK_NOTHROW(copy.addTick(90));
    CHECK_NOTHROW(copy.addTick(180));

    CHECK_EQ(copy.domainSize(), 2u);
    CHECK_EQ(copy.toString(), "angle:Discretized(<[0;90[,[90;180]>)");
    CHECK(!copy.empty());
    CHECK_EQ(copy["[0,90["], static_cast< gum::Size >(0));
    CHECK_EQ(copy.index("[0,90["), static_cast< gum::Size >(0));
  }

  GUM_TEST(CopyEmptyVariableWithoutZeros) {
    gum::DiscretizedVariable< double > source("angle", "");
    auto                               copy = source;

    CHECK_NOTHROW(copy.addTick(1));
    CHECK_NOTHROW(copy.addTick(90));
    CHECK_NOTHROW(copy.addTick(180));

    CHECK_EQ(copy.domainSize(), 2u);
    CHECK_EQ(copy.toString(), "angle:Discretized(<[1;90[,[90;180]>)");
    CHECK(!copy.empty());
  }

  GUM_TEST(IsNumerical) {
    gum::DiscretizedVariable< double > var("var", "var");
    var.addTick(0.0).addTick(1.0).addTick(2.0);
    CHECK(var.isNumerical());
  }

  GUM_TEST(CopyPreservesEmpirical) {
    gum::DiscretizedVariable< double > src("x", "");
    src.addTick(0.0).addTick(1.0).addTick(2.0);
    src.setEmpirical(true);

    gum::DiscretizedVariable< double > byCopy(src);
    CHECK(byCopy.isEmpirical());

    gum::DiscretizedVariable< double > byAssign("y", "");
    byAssign.addTick(0.0).addTick(1.0);
    byAssign = src;
    CHECK(byAssign.isEmpirical());

    src.setEmpirical(false);
    gum::DiscretizedVariable< double > byCopyFalse(src);
    CHECK_FALSE(byCopyFalse.isEmpirical());
  }
}   // namespace gum_tests
