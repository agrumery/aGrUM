
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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

#include <agrum/BN/BayesNet.h>


namespace gum_tests {
  class DAGModelTestSuite: public CxxTest::TestSuite {
    public:
    void testEquality() {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b->c;a->c");

      TS_ASSERT(bn.hasSameStructure(bn));
      TS_ASSERT(bn.hasSameStructure(
         gum::BayesNet< float >::fastPrototype("a->b->c;a->c")));
      TS_ASSERT(!bn.hasSameStructure(
         gum::BayesNet< float >::fastPrototype("b->a->c;b->c")));
      TS_ASSERT(
         !bn.hasSameStructure(gum::BayesNet< float >::fastPrototype("b->a")));
      TS_ASSERT(
         !bn.hasSameStructure(gum::BayesNet< double >::fastPrototype("b->a")));
      TS_ASSERT(
         !bn.hasSameStructure(gum::BayesNet< float >::fastPrototype("b->a->d")));
    }

    void testAncestors() {
      auto bn = gum::BayesNet< float >::fastPrototype(
         "A->B<-C->D->E<-A->F;G->A;D->H;G<-I->C<-J");
      TS_ASSERT_EQUALS(bn.descendants(6), gum::NodeSet({0, 1, 4, 5}));
      TS_ASSERT_EQUALS(bn.descendants("G"), gum::NodeSet({0, 1, 4, 5}));

      TS_ASSERT_EQUALS(bn.descendants(1), gum::NodeSet());
      TS_ASSERT_EQUALS(bn.descendants("B"), gum::NodeSet());

      TS_ASSERT_EQUALS(bn.ancestors(1), gum::NodeSet({0, 2,6,8,9}));
      TS_ASSERT_EQUALS(bn.ancestors("B"), gum::NodeSet({0, 2,6,8,9}));

      TS_ASSERT_EQUALS(bn.ancestors(9), gum::NodeSet());
      TS_ASSERT_EQUALS(bn.ancestors("J"), gum::NodeSet());


    }
  };
}   // namespace gum_tests
