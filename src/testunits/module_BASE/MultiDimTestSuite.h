
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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

#include <agrum/tools/multidim/implementations/multiDimArray.h>
#include <agrum/tools/multidim/instantiation.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  class MultidimTestSuite: public CxxTest::TestSuite {
    public:
    void testCreation() {
      gum::MultiDimArray< double > m;
      TS_ASSERT_EQUALS(m.nbrDim(), (gum::Size)0);
      TS_ASSERT_EQUALS(m.domainSize(), (gum::Size)1);

      gum::LabelizedVariable a("a", "", 4), b("b", "", 5);
      m << a;
      TS_ASSERT_EQUALS(m.nbrDim(), (gum::Size)1);
      TS_ASSERT_EQUALS(m.domainSize(), (gum::Size)4);
      m << b;
      TS_ASSERT_EQUALS(m.nbrDim(), (gum::Size)2);
      TS_ASSERT_EQUALS(m.domainSize(), (gum::Size)20);

      gum::MultiDimArray< double > mm = m;
      TS_ASSERT_EQUALS(mm.nbrDim(), (gum::Size)2);
      TS_ASSERT_EQUALS(mm.domainSize(), (gum::Size)20);
    }

    void testMemoryCrash() {
      gum::MultiDimArray< double > m;
      gum::LabelizedVariable*      v[100];

      for (int i = 0; i < 100; i++)
        v[i] = new gum::LabelizedVariable("x" + std::to_string(i), "x");

      TS_ASSERT_THROWS(feedMultiDimUntilOverflow(v, m), gum::OutOfBounds);

      for (int i = 0; i < 100; i++)
        delete (v[i]);
    }

    private:
    void feedMultiDimUntilOverflow(gum::LabelizedVariable*       v[],
                                   gum::MultiDimArray< double >& t) {
      t.beginMultipleChanges();

      for (int i = 0; i < 100; i++)
        t << *v[i];

      t.endMultipleChanges();
    }
  };
}   // namespace gum_tests
