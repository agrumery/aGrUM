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

#include <cstdio>git c
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/ID/inference/tools/decisionPotential.h>

namespace gum_tests {

  class DecisionPotentialTestSuite: public CxxTest::TestSuite {
    public:
    void testConstruction() {
      gum::DecisionPotential< double > d1, d2;
      TS_ASSERT(d1 == d2);
      TS_ASSERT(d1 == d1 * d2);

/*      d1=std::move<gum::DecisionPotential< double >>(
         static_cast< gum::DecisionPotential< double >&& >(d2));

      gum::DecisionPotential<double> d3(static_cast< gum::DecisionPotential< double >&& >(d2));*/
    }
    void testConstruction2() {

    }
  };
}   // namespace gum_tests
