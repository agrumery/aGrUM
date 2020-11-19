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


#include <iostream>
#include <string>

#include <cstdlib>   // rand

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/CN/LpInterface.h>

namespace gum_tests {

  class LpInterfacetTestSuite: public CxxTest::TestSuite {
    public:
    void testLpExpr() {
      gum::credal::lp::LpInterface< double > pl;
      auto                                   v = pl.addCols(3);
      const gum::credal::lp::LpCol&          a = v[0];

      gum::credal::lp::LpExpr expr = 2 + a <= 5;
      gum::credal::lp::LpRow  row(expr, v);
    }
    void testLpCreation() {
      gum::credal::lp::LpInterface< double > pl;

      auto A = pl.addCols(3);   // 3 variables for the lp
      pl.addRow(0.1 <= A[0] <= 0.7);
      pl.addRow(0.1 <= A[1] <= 0.5);
      pl.addRow(0.1 <= A[2] <= 0.6);
    }
  };   // end of class LpInterfacetTestSuite

}   // end of namespace gum_tests
