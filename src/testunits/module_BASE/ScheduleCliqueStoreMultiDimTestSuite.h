
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


#include <sstream>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/tools/graphicalModels/inference/scheduler/scheduleCliqueStoreMultiDim.h>
#include <agrum/tools/graphs/graphElements.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  class ScheduleCliqueStoreMultiDimTestSuite: public CxxTest::TestSuite {
    public:
    void test_construct() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Potential< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[3]) << *(vars[4]);
      gum::ScheduleMultiDim< double > f1(pot1);
      gum::Potential< double >        pot2;
      pot2 << *(vars[0]) << *(vars[2]) << *(vars[3]) << *(vars[4]);
      gum::ScheduleMultiDim< double > f2(pot2);

      gum::NodeProperty< gum::Set< const gum::MultiDimImplementation< double >* > >
         set;
      TS_ASSERT(set.size() == 0);
      gum::ScheduleCliqueStoreMultiDim< double > store1(f1, set, 3);
      gum::ScheduleCliqueStoreMultiDim< double > store2(f2, set, 3);
      gum::ScheduleCliqueStoreMultiDim< double > store3(f2, set, 2);
      TS_ASSERT(store1 != store2);
      TS_ASSERT(store2 != store3);
      TS_ASSERT(!(store2 == store3));

      TS_ASSERT(store1.nbOperations() == 1);
      std::pair< long, long > xxx = store1.memoryUsage();
      TS_ASSERT(xxx.first == 0);

      gum::Sequence< const gum::ScheduleMultiDim< double >* > seq =
         store1.multiDimArgs();
      TS_ASSERT(seq.size() == 1);
      TS_ASSERT(*(seq.atPos(0)) == f1);

      store1.execute();
      TS_ASSERT_THROWS_NOTHING(store1.execute());
      store2.execute();
      store3.execute();
      TS_ASSERT(set.size() == 2);

      std::stringstream s;
      s << "store ( " << f1.toString() << ", clique " << 3 << " )";
      TS_ASSERT(store1.toString() == s.str());

      gum::ScheduleCliqueStoreMultiDim< double > store4(store3);
      TS_ASSERT(store4 == store3);
      TS_ASSERT(store4 != store1);
      store4 = store1;
      TS_ASSERT(store4 == store1);

      TS_ASSERT(store4.type()
                == gum::ScheduleOperation< double >::Type::CLIQUE_STORE_MULTIDIM);

      gum::ScheduleCliqueStoreMultiDim< double >* store5 = store4.newFactory();
      TS_ASSERT(*store5 == store4);
      delete store5;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }
  };

} /* namespace gum_tests */
