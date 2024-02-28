/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <iostream>
#include <sstream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/tools/core/list.h>
#include <agrum/tools/variables/discretizedVariable.h>

namespace gum_tests {

  class [[maybe_unused]] DiscretizedVariableTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(Creation){
        TS_GUM_ASSERT_THROWS_NOTHING(gum::DiscretizedVariable< int > v("var", "a var"))}

    GUM_ACTIVE_TEST(DiscreteVariableProperties) {
      gum::DiscretizedVariable< int > v("var", "a var");
      TS_ASSERT_EQUALS(v.name(), "var")
      v.setName("toto");
      TS_ASSERT_EQUALS(v.name(), "toto")

      TS_ASSERT_EQUALS(v.description(), "a var")
      v.setDescription("toto");
      TS_ASSERT_EQUALS(v.description(), "toto")

      TS_ASSERT_EQUALS(v.varType(), gum::VarType::Discretized)

      const gum::DiscretizedVariable< int >& w = v;
      w.setDescription("Lol");   // change description does not change a variable
    }

    GUM_ACTIVE_TEST(ConstructorWithTicks) {
      gum::DiscretizedVariable< double > d("d", "Discretized variable", {3.1, 2.0, 4.0});
      TS_GUM_ASSERT_THROWS_NOTHING(d["2.5"])
      TS_ASSERT_THROWS(d["0.5"], const gum::OutOfBounds&)
      TS_ASSERT_THROWS(d["4.5"], const gum::OutOfBounds&)

      TS_ASSERT_EQUALS(d.numerical(0), (2.0 + 3.1) / 2)
      TS_ASSERT_EQUALS(d.numerical(1), (4.0 + 3.1) / 2)

      TS_ASSERT_THROWS(d.addTick(-std::numeric_limits< double >::infinity()),
                       const gum::DefaultInLabel&);
      TS_ASSERT_THROWS(d.addTick(std::numeric_limits< double >::infinity()),
                       const gum::DefaultInLabel&);

      TS_ASSERT_THROWS(d["0.5"], const gum::OutOfBounds&)
      TS_ASSERT_THROWS(d["4.5"], const gum::OutOfBounds&)

      d.setEmpirical(true);

      TS_GUM_ASSERT_THROWS_NOTHING(d["0.5"])
      TS_GUM_ASSERT_THROWS_NOTHING(d["4.5"])
    }

    GUM_ACTIVE_TEST(AddTicks) {
      gum::DiscretizedVariable< int > v("var", "a var");

      TS_ASSERT_THROWS(v.tick((gum::Idx)1), const gum::OutOfBounds&)

      TS_ASSERT(v.empty())
      TS_ASSERT_EQUALS(v.domainSize(), (gum::Idx)0)
      v.addTick(1);
      TS_ASSERT(v.empty())
      TS_ASSERT_EQUALS(v.domainSize(), (gum::Idx)0)
      v.addTick(3);
      TS_ASSERT(v.empty())
      TS_ASSERT_EQUALS(v.domainSize(), (gum::Idx)1)
      v.addTick(2);
      TS_ASSERT(!v.empty())
      TS_ASSERT_EQUALS(v.domainSize(), (gum::Idx)2)

      TS_ASSERT_THROWS(v.addTick(2), const gum::DefaultInLabel&)

      std::stringstream s;
      s << v;
      TS_ASSERT_EQUALS(s.str(), "var:Discretized(<[1;2[,[2;3]>)")
      TS_ASSERT_EQUALS(v.toString(), "var:Discretized(<[1;2[,[2;3]>)")

      TS_ASSERT_EQUALS(v.tick((gum::Idx)0), 1)
      TS_ASSERT_EQUALS(v.tick((gum::Idx)1), 2)
      TS_ASSERT_EQUALS(v.tick((gum::Idx)2), 3)

      TS_ASSERT_THROWS(v.tick((gum::Idx)3), const gum::OutOfBounds&)
    }

    GUM_ACTIVE_TEST(NormalLimits) {
      gum::DiscretizedVariable< unsigned int > v("var", "a var");
      v.addTick(1).addTick(5).addTick(3).addTick(7);

      TS_ASSERT_EQUALS(v.toString(), "var:Discretized(<[1;3[,[3;5[,[5;7]>)")

      gum::Size vv = (gum::Size)0;

      TS_ASSERT_THROWS(v["0"], const gum::OutOfBounds&)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["1"])
      TS_ASSERT_EQUALS(vv, (gum::Size)0)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["2"])
      TS_ASSERT_EQUALS(vv, (gum::Size)0)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["3"])
      TS_ASSERT_EQUALS(vv, (gum::Size)1)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["4"])
      TS_ASSERT_EQUALS(vv, (gum::Size)1)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["5"])
      TS_ASSERT_EQUALS(vv, (gum::Size)2)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["6"])
      TS_ASSERT_EQUALS(vv, (gum::Size)2)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["7"])
      TS_ASSERT_EQUALS(vv, (gum::Size)2)

      TS_ASSERT_THROWS(v["8"], const gum::OutOfBounds&)
    }

    GUM_ACTIVE_TEST(NormalLimitsWithEmpirical) {
      gum::DiscretizedVariable< unsigned int > v("var", "a var");
      v.addTick(1).addTick(5).addTick(3).addTick(7);

      v.setEmpirical(true);

      TS_ASSERT_EQUALS(v.toString(), "var:Discretized(<(1;3[,[3;5[,[5;7)>)")

      gum::Size vv = (gum::Size)0;

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["0"])
      TS_ASSERT_EQUALS(vv, (gum::Size)0)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["1"])
      TS_ASSERT_EQUALS(vv, (gum::Size)0)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["2"])
      TS_ASSERT_EQUALS(vv, (gum::Size)0)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["3"])
      TS_ASSERT_EQUALS(vv, (gum::Size)1)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["4"])
      TS_ASSERT_EQUALS(vv, (gum::Size)1)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["5"])
      TS_ASSERT_EQUALS(vv, (gum::Size)2)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["6"])
      TS_ASSERT_EQUALS(vv, (gum::Size)2)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["7"])
      TS_ASSERT_EQUALS(vv, (gum::Size)2)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["8"])
      TS_ASSERT_EQUALS(vv, (gum::Size)2)
    }

    GUM_ACTIVE_TEST(NormalLimitsWithEmpiricalInConstructor) {
      gum::DiscretizedVariable< unsigned int > v("var",
                                                 "a var",
                                                 {1, 3, 5, 7},
                                                 true);   // empirical is true

      TS_ASSERT_EQUALS(v.toString(), "var:Discretized(<(1;3[,[3;5[,[5;7)>)")

      gum::Size vv = (gum::Size)0;

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["0"])
      TS_ASSERT_EQUALS(vv, (gum::Size)0)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["1"])
      TS_ASSERT_EQUALS(vv, (gum::Size)0)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["2"])
      TS_ASSERT_EQUALS(vv, (gum::Size)0)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["3"])
      TS_ASSERT_EQUALS(vv, (gum::Size)1)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["4"])
      TS_ASSERT_EQUALS(vv, (gum::Size)1)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["5"])
      TS_ASSERT_EQUALS(vv, (gum::Size)2)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["6"])
      TS_ASSERT_EQUALS(vv, (gum::Size)2)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["7"])
      TS_ASSERT_EQUALS(vv, (gum::Size)2)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["8"])
      TS_ASSERT_EQUALS(vv, (gum::Size)2)
    }

    GUM_ACTIVE_TEST(NormalLimitsWithNoEmpiricalInConstructor) {
      gum::DiscretizedVariable< unsigned int > v("var",
                                                 "a var",
                                                 {1, 3, 5, 7});   // empirical is false

      TS_ASSERT_EQUALS(v.toString(), "var:Discretized(<[1;3[,[3;5[,[5;7]>)")

      auto vv = (gum::Size)0;

      TS_ASSERT_THROWS(vv = v["0"], gum::OutOfBounds&)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["1"])
      TS_ASSERT_EQUALS(vv, (gum::Size)0)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["2"])
      TS_ASSERT_EQUALS(vv, (gum::Size)0)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["3"])
      TS_ASSERT_EQUALS(vv, (gum::Size)1)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["4"])
      TS_ASSERT_EQUALS(vv, (gum::Size)1)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["5"])
      TS_ASSERT_EQUALS(vv, (gum::Size)2)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["6"])
      TS_ASSERT_EQUALS(vv, (gum::Size)2)

      TS_GUM_ASSERT_THROWS_NOTHING(vv = v["7"])
      TS_ASSERT_EQUALS(vv, (gum::Size)2)

      TS_ASSERT_THROWS(vv = v["8"], gum::OutOfBounds&)
    }

    GUM_ACTIVE_TEST(OrderTicks) {
      for (int i = 1; i < 7; i++)
        for (int j = 1; j < 7; j++)
          for (int k = 1; k < 7; k++)
            for (int l = 1; l < 7; l++)
              for (int m = 1; m < 7; m++)
                for (int n = 1; n < 7; n++) {
                  if ((i + j + k + l + m + n == 21) && (i * j * k * l * m * n == 720)) {
                    gum::DiscretizedVariable< int > d("d", "Discretized variable");
                    d.addTick(i).addTick(j).addTick(k).addTick(l).addTick(m).addTick(n);
                    TS_ASSERT_EQUALS(d.domainSize(), (gum::Size)5)
                    TS_ASSERT_EQUALS(d.toString(),
                                     "d:Discretized(<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>)");
                    int s = 1;
                    for (auto tick: d.ticks()) {
                      TS_ASSERT_EQUALS(tick, s++)
                    }
                  }
                }
    }

    GUM_ACTIVE_TEST(FloatLimits) {
      gum::DiscretizedVariable< double > d("d", "Discretized variable");
      d.addTick(3.1).addTick(2.0).addTick(4.0);

      TS_GUM_ASSERT_THROWS_NOTHING(d["2.5"])
      TS_ASSERT_THROWS(d["0.5"], const gum::OutOfBounds&)
      TS_ASSERT_THROWS(d["4.5"], const gum::OutOfBounds&)

      d.setEmpirical(true);

      TS_GUM_ASSERT_THROWS_NOTHING(d["2.5"])
      TS_GUM_ASSERT_THROWS_NOTHING(d["0.5"])
      TS_GUM_ASSERT_THROWS_NOTHING(d["4.5"])
    }

    GUM_ACTIVE_TEST(FloatLimitsEmpirical) {
      gum::DiscretizedVariable< double > d("d", "Discretized variable");
      d.addTick(3.1).addTick(2.0).addTick(4.0);

      d.setEmpirical(true);

      TS_GUM_ASSERT_THROWS_NOTHING(d["2.5"])
      TS_GUM_ASSERT_THROWS_NOTHING(d["0.5"])
      TS_GUM_ASSERT_THROWS_NOTHING(d["4.5"])
    }

    GUM_ACTIVE_TEST(Numerical) {
      gum::DiscretizedVariable< double > d("d", "Discretized variable");
      d.addTick(3.1).addTick(2.0).addTick(4.0);

      TS_ASSERT_EQUALS(d.numerical(0), (2.0 + 3.1) / 2)
      TS_ASSERT_EQUALS(d.numerical(1), (4.0 + 3.1) / 2)
    }

    GUM_ACTIVE_TEST(CopyEmptyVariableWithZeros) {
      gum::DiscretizedVariable< double > source("angle", "");

      auto copy = source;

      TS_ASSERT_THROWS_NOTHING(copy.addTick(0))
      TS_ASSERT_THROWS_NOTHING(copy.addTick(90))
      TS_ASSERT_THROWS_NOTHING(copy.addTick(180))

      TS_ASSERT_EQUALS(copy.domainSize(), 2u)
      TS_ASSERT_EQUALS(copy.toString(), "angle:Discretized(<[0;90[,[90;180]>)")
      TS_ASSERT(!copy.empty())
      TS_ASSERT_EQUALS(copy["[0;90["], (gum::Size)0)
      TS_ASSERT_EQUALS(copy.index("[0;90["), (gum::Size)0)
    }

    GUM_ACTIVE_TEST(CopyEmptyVariableWithoutZeros) {
      gum::DiscretizedVariable< double > source("angle", "");
      auto                               copy = source;

      TS_ASSERT_THROWS_NOTHING(copy.addTick(1))
      TS_ASSERT_THROWS_NOTHING(copy.addTick(90))
      TS_ASSERT_THROWS_NOTHING(copy.addTick(180))

      TS_ASSERT_EQUALS(copy.domainSize(), 2u)
      TS_ASSERT_EQUALS(copy.toString(), "angle:Discretized(<[1;90[,[90;180]>)")
      TS_ASSERT(!copy.empty())
    }
  };
}   // namespace gum_tests
