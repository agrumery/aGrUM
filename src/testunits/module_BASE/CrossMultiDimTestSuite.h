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

#include <agrum/variables/rangeVariable.h>
#include <agrum/multidim/aggregators/min.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/multiDimSparse.h>

namespace gum_tests {

  class CrossMultiDimTestSuite : public CxxTest::TestSuite {
    public:
    void testComparaisonMin() {
      gum::RangeVariable a("a", "", 0, 3), b("b", "", 0, 3), c("c", "", 0, 3),
          d("d", "", 0, 3);
      gum::MultiDimArray<float> full;
      gum::MultiDimSparse<float> sparse((float)0);
      gum::aggregator::Min<float> agg;
      agg << a << b << c << d;
      full << a << b << c << d;
      sparse << a << b << c << d;

      gum::Instantiation i(agg);

      for (i.setFirst(); !i.end(); ++i) {
        float res = agg[i];
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

      TS_ASSERT_EQUALS(agg.compressionRate(), (float)1.0); // 100% de compression

      TS_ASSERT_EQUALS(
          sparse.compressionRate(),
          (float)0.75); // deterministic as a sparse : 75% parameters are 0...
      TS_ASSERT_EQUALS(full.compressionRate(), (float)0); // 0% de compression...
    }
  };
}
