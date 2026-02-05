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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/list.h>
#include <agrum/base/variables/discretizedVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DiscretizedVariable
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct DiscretizedVariableTestSuite {
    public:
    static void testCreation() {
      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::DiscretizedVariable< int > v("var", "a var"));
    }

    static void testDiscreteVariableProperties() {
      gum::DiscretizedVariable< int > v("var", "a var");
      CHECK((v.name()) == ("var"));
      v.setName("toto");
      CHECK((v.name()) == ("toto"));

      CHECK((v.description()) == ("a var"));
      v.setDescription("toto");
      CHECK((v.description()) == ("toto"));

      CHECK((v.varType()) == (gum::VarType::DISCRETIZED));

      const gum::DiscretizedVariable< int >& w = v;
      w.setDescription("Lol");   // change description does not change a variable
    }   // namespace gum_tests

    static void testConstructorWithTicks() {
      gum::DiscretizedVariable< double > d("d", "Discretized variable", {3.1, 2.0, 4.0});
      GUM_CHECK_ASSERT_THROWS_NOTHING(d["2.5"]);
      CHECK_THROWS_AS(d["0.5"], const gum::OutOfBounds&);
      CHECK_THROWS_AS(d["4.5"], const gum::OutOfBounds&);

      CHECK((d.numerical(0)) == ((2.0 + 3.1) / 2));
      CHECK((d.numerical(1)) == ((4.0 + 3.1) / 2));

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

    static void testAccessorWithIntervallsForGaspard() {
      gum::DiscretizedVariable< double > d("d", "Discretized variable", {3.1, 2.0, 4.0, 10.0});

      GUM_CHECK_ASSERT_THROWS_NOTHING(d["[2.0,3.1]"]);
      GUM_CHECK_ASSERT_THROWS_NOTHING(d["[3.1,4.0]"]);
      GUM_CHECK_ASSERT_THROWS_NOTHING(d["[4.0,10]"]);

      GUM_CHECK_ASSERT_THROWS_NOTHING(d["[2.0;3.1]"]);
      GUM_CHECK_ASSERT_THROWS_NOTHING(d["[3.1;4.0]"]);
      GUM_CHECK_ASSERT_THROWS_NOTHING(d["[4.0;10]"]);

      CHECK_THROWS_AS(d["x10y20.0z"], const gum::NotFound&);
      CHECK_THROWS_AS(d["[10,20]"], const gum::NotFound&);

      CHECK((d["[2.0,3.1]"]) == (d["2.5"]));
      CHECK((d["[3.1,4.0]"]) == (d["3.5"]));
      CHECK((d["[4.0,10]"]) == (d["6.5"]));

      d.setEmpirical(true);
      GUM_CHECK_ASSERT_THROWS_NOTHING(d["[2.0,3.1]"]);
      GUM_CHECK_ASSERT_THROWS_NOTHING(d["[3.1,4.0]"]);
      GUM_CHECK_ASSERT_THROWS_NOTHING(d["[4.0,10]"]);

      CHECK((d["[2.0,3.1]"]) == (d["2.5"]));
      CHECK((d["[3.1,4.0]"]) == (d["3.5"]));
      CHECK((d["[4.0,10]"]) == (d["6.5"]));
    }

    static void testAddTicks() {
      gum::DiscretizedVariable< int > v("var", "a var");

      CHECK_THROWS_AS(v.tick(static_cast< gum::Idx >(1)), const gum::OutOfBounds&);

      CHECK(v.empty());
      CHECK((v.domainSize()) == (static_cast< gum::Idx >(0)));
      v.addTick(1);
      CHECK(v.empty());
      CHECK((v.domainSize()) == (static_cast< gum::Idx >(0)));
      v.addTick(3);
      CHECK(v.empty());
      CHECK((v.domainSize()) == (static_cast< gum::Idx >(1)));
      v.addTick(2);
      CHECK(!v.empty());
      CHECK((v.domainSize()) == (static_cast< gum::Idx >(2)));

      CHECK_THROWS_AS(v.addTick(2), const gum::DefaultInLabel&);

      std::stringstream s;
      s << v;
      CHECK((s.str()) == ("var:Discretized(<[1;2[,[2;3]>)"));
      CHECK((v.toString()) == ("var:Discretized(<[1;2[,[2;3]>)"));

      CHECK((v.tick(static_cast< gum::Idx >(0))) == (1));
      CHECK((v.tick(static_cast< gum::Idx >(1))) == (2));
      CHECK((v.tick(static_cast< gum::Idx >(2))) == (3));

      CHECK_THROWS_AS(v.tick(static_cast< gum::Idx >(3)), const gum::OutOfBounds&);
    }

    static void testNormalLimits() {
      gum::DiscretizedVariable< unsigned int > v("var", "a var");
      v.addTick(1).addTick(5).addTick(3).addTick(7);

      CHECK((v.toString()) == ("var:Discretized(<[1;3[,[3;5[,[5;7]>)"));

      gum::Size vv = static_cast< gum::Size >(0);

      CHECK_THROWS_AS(v["0"], const gum::OutOfBounds&);

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["1"]);
      CHECK((vv) == (static_cast< gum::Size >(0)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["2"]);
      CHECK((vv) == (static_cast< gum::Size >(0)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["3"]);
      CHECK((vv) == (static_cast< gum::Size >(1)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["4"]);
      CHECK((vv) == (static_cast< gum::Size >(1)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["5"]);
      CHECK((vv) == (static_cast< gum::Size >(2)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["6"]);
      CHECK((vv) == (static_cast< gum::Size >(2)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["7"]);
      CHECK((vv) == (static_cast< gum::Size >(2)));

      CHECK_THROWS_AS(v["8"], const gum::OutOfBounds&);
    }

    static void testNormalLimitsWithEmpirical() {
      gum::DiscretizedVariable< unsigned int > v("var", "a var");
      v.addTick(1).addTick(5).addTick(3).addTick(7);

      v.setEmpirical(true);

      CHECK((v.toString()) == ("var:Discretized(<(1;3[,[3;5[,[5;7)>)"));

      gum::Size vv = static_cast< gum::Size >(0);

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["0"]);
      CHECK((vv) == (static_cast< gum::Size >(0)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["1"]);
      CHECK((vv) == (static_cast< gum::Size >(0)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["2"]);
      CHECK((vv) == (static_cast< gum::Size >(0)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["3"]);
      CHECK((vv) == (static_cast< gum::Size >(1)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["4"]);
      CHECK((vv) == (static_cast< gum::Size >(1)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["5"]);
      CHECK((vv) == (static_cast< gum::Size >(2)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["6"]);
      CHECK((vv) == (static_cast< gum::Size >(2)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["7"]);
      CHECK((vv) == (static_cast< gum::Size >(2)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["8"]);
      CHECK((vv) == (static_cast< gum::Size >(2)));
    }

    static void testNormalLimitsWithEmpiricalInConstructor() {
      gum::DiscretizedVariable< unsigned int > v("var",
                                                 "a var",
                                                 {1, 3, 5, 7},
                                                 true);   // empirical is true

      CHECK((v.toString()) == ("var:Discretized(<(1;3[,[3;5[,[5;7)>)"));

      gum::Size vv = static_cast< gum::Size >(0);

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["0"]);
      CHECK((vv) == (static_cast< gum::Size >(0)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["1"]);
      CHECK((vv) == (static_cast< gum::Size >(0)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["2"]);
      CHECK((vv) == (static_cast< gum::Size >(0)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["3"]);
      CHECK((vv) == (static_cast< gum::Size >(1)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["4"]);
      CHECK((vv) == (static_cast< gum::Size >(1)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["5"]);
      CHECK((vv) == (static_cast< gum::Size >(2)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["6"]);
      CHECK((vv) == (static_cast< gum::Size >(2)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["7"]);
      CHECK((vv) == (static_cast< gum::Size >(2)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["8"]);
      CHECK((vv) == (static_cast< gum::Size >(2)));
    }

    static void testNormalLimitsWithNoEmpiricalInConstructor() {
      gum::DiscretizedVariable< unsigned int > v("var",
                                                 "a var",
                                                 {1, 3, 5, 7});   // empirical is false

      CHECK((v.toString()) == ("var:Discretized(<[1;3[,[3;5[,[5;7]>)"));

      auto vv = static_cast< gum::Size >(0);

      CHECK_THROWS_AS(vv = v["0"], gum::OutOfBounds&);

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["1"]);
      CHECK((vv) == (static_cast< gum::Size >(0)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["2"]);
      CHECK((vv) == (static_cast< gum::Size >(0)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["3"]);
      CHECK((vv) == (static_cast< gum::Size >(1)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["4"]);
      CHECK((vv) == (static_cast< gum::Size >(1)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["5"]);
      CHECK((vv) == (static_cast< gum::Size >(2)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["6"]);
      CHECK((vv) == (static_cast< gum::Size >(2)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(vv = v["7"]);
      CHECK((vv) == (static_cast< gum::Size >(2)));

      CHECK_THROWS_AS(vv = v["8"], gum::OutOfBounds&);
    }

    static void testOrderTicks() {
      for (int i = 1; i < 7; i++)
        for (int j = 1; j < 7; j++)
          for (int k = 1; k < 7; k++)
            for (int l = 1; l < 7; l++)
              for (int m = 1; m < 7; m++)
                for (int n = 1; n < 7; n++) {
                  if ((i + j + k + l + m + n == 21) && (i * j * k * l * m * n == 720)) {
                    gum::DiscretizedVariable< int > d("d", "Discretized variable");
                    d.addTick(i).addTick(j).addTick(k).addTick(l).addTick(m).addTick(n);
                    CHECK((d.domainSize()) == (static_cast< gum::Size >(5)));
                    CHECK((d.toString()) == ("d:Discretized(<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>)"));
                    int s = 1;
                    for (auto tick: d.ticks()) {
                      CHECK((tick) == (s++));
                    }
                  }
                }
    }

    static void testFloatLimits() {
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

    static void testFloatLimitsEmpirical() {
      gum::DiscretizedVariable< double > d("d", "Discretized variable");
      d.addTick(3.1).addTick(2.0).addTick(4.0);

      d.setEmpirical(true);

      GUM_CHECK_ASSERT_THROWS_NOTHING(d["2.5"]);
      GUM_CHECK_ASSERT_THROWS_NOTHING(d["0.5"]);
      GUM_CHECK_ASSERT_THROWS_NOTHING(d["4.5"]);
    }

    static void testNumerical() {
      gum::DiscretizedVariable< double > d("d", "Discretized variable");
      d.addTick(3.1).addTick(2.0).addTick(4.0);

      CHECK((d.numerical(0)) == ((2.0 + 3.1) / 2));
      CHECK((d.numerical(1)) == ((4.0 + 3.1) / 2));
    }

    static void testCopyEmptyVariableWithZeros() {
      gum::DiscretizedVariable< double > source("angle", "");

      auto copy = source;

      CHECK_NOTHROW(copy.addTick(0));
      CHECK_NOTHROW(copy.addTick(90));
      CHECK_NOTHROW(copy.addTick(180));

      CHECK((copy.domainSize()) == (2u));
      CHECK((copy.toString()) == ("angle:Discretized(<[0;90[,[90;180]>)"));
      CHECK(!copy.empty());
      CHECK((copy["[0,90["]) == (static_cast< gum::Size >(0)));
      CHECK((copy.index("[0,90[")) == (static_cast< gum::Size >(0)));
    }

    static void testCopyEmptyVariableWithoutZeros() {
      gum::DiscretizedVariable< double > source("angle", "");
      auto                               copy = source;

      CHECK_NOTHROW(copy.addTick(1));
      CHECK_NOTHROW(copy.addTick(90));
      CHECK_NOTHROW(copy.addTick(180));

      CHECK((copy.domainSize()) == (2u));
      CHECK((copy.toString()) == ("angle:Discretized(<[1;90[,[90;180]>)"));
      CHECK(!copy.empty());
    }
  };

  GUM_TEST_ACTIF(Creation)
  GUM_TEST_ACTIF(DiscreteVariableProperties)
  GUM_TEST_ACTIF(ConstructorWithTicks)
  GUM_TEST_ACTIF(AccessorWithIntervallsForGaspard)
  GUM_TEST_ACTIF(AddTicks)
  GUM_TEST_ACTIF(NormalLimits)
  GUM_TEST_ACTIF(NormalLimitsWithEmpirical)
  GUM_TEST_ACTIF(NormalLimitsWithEmpiricalInConstructor)
  GUM_TEST_ACTIF(NormalLimitsWithNoEmpiricalInConstructor)
  GUM_TEST_ACTIF(OrderTicks)
  GUM_TEST_ACTIF(FloatLimits)
  GUM_TEST_ACTIF(FloatLimitsEmpirical)
  GUM_TEST_ACTIF(Numerical)
  GUM_TEST_ACTIF(CopyEmptyVariableWithZeros)
  GUM_TEST_ACTIF(CopyEmptyVariableWithoutZeros)
}   // namespace gum_tests
