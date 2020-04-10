
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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


#include <sstream>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/tools/multidim/aggregators/min.h>
#include <agrum/tools/multidim/implementations/multiDimArray.h>
#include <agrum/tools/multidim/implementations/multiDimSparse.h>
#include <agrum/tools/variables/rangeVariable.h>

namespace gum_tests {

  class CrossMultiDimTestSuite: public CxxTest::TestSuite {
    public:
    void testComparaisonMin() {
      gum::RangeVariable a("a", "", 0, 3), b("b", "", 0, 3), c("c", "", 0, 3),
         d("d", "", 0, 3);
      gum::MultiDimArray< double >   full;
      gum::MultiDimSparse< double >  sparse((float)0);
      gum::aggregator::Min< double > agg;
      agg << a << b << c << d;
      full << a << b << c << d;
      sparse << a << b << c << d;

      gum::Instantiation i(agg);

      for (i.setFirst(); !i.end(); ++i) {
        double res = agg[i];
        full.set(i, res);
        sparse.set(i, res);
      }

      for (i.setFirst(); !i.end(); ++i) {
        TS_ASSERT_EQUALS(sparse[i], agg[i]);
        TS_ASSERT_EQUALS(full[i], agg[i]);
      }

      gum::Instantiation j(sparse);

      for (j.setFirst(); !j.end(); ++j) {
        TS_ASSERT_EQUALS(agg[j], sparse[j]);
        TS_ASSERT_EQUALS(full[j], sparse[j]);
      }

      gum::Instantiation k(full);

      for (k.setFirst(); !k.end(); ++k) {
        TS_ASSERT_EQUALS(agg[k], full[k]);
        TS_ASSERT_EQUALS(sparse[k], full[k]);
      }

      TS_ASSERT_EQUALS(agg.compressionRate(), (float)1.0);   // 100% de compression

      TS_ASSERT_EQUALS(
         sparse.compressionRate(),
         (float)0.75);   // deterministic as a sparse : 75% parameters are 0...
      TS_ASSERT_EQUALS(full.compressionRate(), (float)0);   // 0% de compression...
    }
  };
}   // namespace gum_tests
