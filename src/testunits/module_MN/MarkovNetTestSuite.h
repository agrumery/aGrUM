
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


#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/MN/MarkovNet.h>
#include <agrum/tools/variables/labelizedVariable.h>
#include <agrum/tools/variables/rangeVariable.h>
#include <agrum/tools/variables/discretizedVariable.h>

// The graph used for the tests:
//          1   2_
//         / \ / /
//        3   4 /
//         \ / /
//          5_/
//

namespace gum_tests {
  class MarkovNetTestSuite: public CxxTest::TestSuite {
    private:
    void _fill(gum::MarkovNet< double >& mn) {
      try {
        for (const auto i: {1, 2, 3, 4, 5}) {
          mn.add(std::to_string(i), 3);
        }
        mn.addFactor({"1", "3"});
        mn.addFactor({"1", "4"});
        mn.addFactor({"3", "5"});
        mn.addFactor({"2", "4", "5"});
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    public:
    void setUp() {}

    void tearDown() {}

    void /*test*/Constructor() {
      gum::MarkovNet< double > mn;
      _fill(mn);
      try {
        TS_ASSERT_EQUALS(mn.size(), (gum::Idx)5);
        TS_ASSERT_EQUALS(mn.sizeEdges(), (gum::Idx)6);
        TS_ASSERT_EQUALS(mn.dim(), (gum::Idx)(3 * 3 + 3 * 3 + 3 * 3 + 3 * 3 * 3));
        TS_ASSERT_EQUALS(mn.toString(),
                         "MN{nodes: 5, edges: 6, domainSize: 243, "
                         "dim: 54}");
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    void /*test*/CopyConstructor() {}

    void /*test*/CopyOperator() {}

    void /*test*/Insertion() {}

    void /*test*/Iterations() {
      gum::MarkovNet< double > mn;
      _fill(mn);
      gum::Size cpt = (gum::Size)0;

      for (const auto node: mn.nodes()) {
        GUM_UNUSED(node);
        cpt++;
      }

      TS_ASSERT_EQUALS(cpt, mn.size());

      cpt = (gum::Size)0;

      for (const auto arc: mn.edges()) {
        cpt++;
      }

      TS_ASSERT_EQUALS(cpt, mn.sizeEdges());
    }
  };
}   // namespace gum_tests
