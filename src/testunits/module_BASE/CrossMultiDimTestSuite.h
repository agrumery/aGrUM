/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/multidim/aggregators/min.h>
#include <agrum/base/multidim/implementations/multiDimArray.h>
#include <agrum/base/multidim/implementations/multiDimSparse.h>
#include <agrum/base/variables/rangeVariable.h>

namespace gum_tests {

  class [[maybe_unused]] CrossMultiDimTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(ComparaisonMin) {
      gum::RangeVariable a("a", "", 0, 3), b("b", "", 0, 3), c("c", "", 0, 3), d("d", "", 0, 3);
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
        TS_ASSERT_EQUALS(sparse[i], agg[i])
        TS_ASSERT_EQUALS(full[i], agg[i])
      }

      gum::Instantiation j(sparse);

      for (j.setFirst(); !j.end(); ++j) {
        TS_ASSERT_EQUALS(agg[j], sparse[j])
        TS_ASSERT_EQUALS(full[j], sparse[j])
      }

      gum::Instantiation k(full);

      for (k.setFirst(); !k.end(); ++k) {
        TS_ASSERT_EQUALS(agg[k], full[k])
        TS_ASSERT_EQUALS(sparse[k], full[k])
      }

      TS_ASSERT_EQUALS(agg.compressionRate(), (float)1.0);   // 100% de compression

      TS_ASSERT_EQUALS(sparse.compressionRate(),
                       (float)0.75);   // deterministic as a sparse : 75% parameters are 0...
      TS_ASSERT_EQUALS(full.compressionRate(), (float)0);   // 0% de compression...
    }
  };
}   // namespace gum_tests
