/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/tools/variables/labelizedVariable.h>
#include <agrum/ID/inference/tools/decisionPotential.h>

namespace gum_tests {

  class DecisionPotentialTestSuite: public CxxTest::TestSuite {
    public:
    void testConstruction() {
      gum::DecisionPotential< double > d1, d2;
      TS_ASSERT(d1 == d2);
      TS_ASSERT(d1 == d1 * d2);

      d1 = std::move(d2);
      d2 = std::move(d1);

      gum::DecisionPotential<double> d3(std::move(d2));

      gum::LabelizedVariable a("a", "first var", 2), b("b", "second var", 4);
      gum::LabelizedVariable c("c", "first var", 2), d("d", "second var", 4);
      gum::Potential< double > P1, U1;
      P1 << a << b;
      U1 << c << d;

      gum::DecisionPotential<double> d4(P1,U1);
      gum::DecisionPotential<double> d5(d4);
      TS_ASSERT(d4 == d5);

      gum::DecisionPotential<double> d6 = d5;
      TS_ASSERT(d6 == d4);

      gum::DecisionPotential<double> d7(std::move(d4));
      TS_ASSERT(d7 == d6);

      d1 = std::move(d7);
      TS_ASSERT(d1 == d6);

      d5 = d1;
      TS_ASSERT(d1 == d5);

      /*
      d4 = d1;
      TS_ASSERT(d1 == d4);
      */
    }
    void testConstruction2() {

    }
  };
}   // namespace gum_tests
