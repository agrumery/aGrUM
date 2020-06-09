
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

#include <agrum/tools/core/utils_random.h>
#include <agrum/tools/graphicalModels/inference/scheduleDeleteMultiDim.h>
#include <agrum/tools/graphicalModels/inference/scheduleProject.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  class ScheduleDeleteMultiDimTestSuite: public CxxTest::TestSuite {
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
      randomInit(pot1);
      gum::ScheduleMultiDim< double >          f1(pot1);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars << vars[0] << vars[3];
      gum::ScheduleProject< double >         myproj(f1, del_vars, gum::projectMax);
      const gum::ScheduleMultiDim< double >& res = myproj.result();

      gum::ScheduleDeleteMultiDim< double > del1(f1);
      gum::ScheduleDeleteMultiDim< double > del2(res);
      gum::ScheduleDeleteMultiDim< double > del3(del2);
      TS_ASSERT(!(del1 == del2));
      TS_ASSERT(del1 != del2);
      TS_ASSERT_THROWS(del2.execute(), gum::NotFound);
      myproj.execute();
      TS_GUM_ASSERT_THROWS_NOTHING(del2.execute());
      TS_ASSERT_THROWS(del3.execute(), gum::NotFound);

      TS_ASSERT(del1.nbOperations() == 1);
      std::pair< long, long > xxx = del1.memoryUsage();
      TS_ASSERT(xxx.first == -16);
      TS_ASSERT(xxx.second == -16);

      gum::Sequence< const gum::ScheduleMultiDim< double >* > seq =
         del2.multiDimArgs();
      TS_ASSERT(seq.size() == 1);
      TS_ASSERT(*(seq.atPos(0)) == res);

      del3 = del1;
      TS_ASSERT(del1 == del3);
      TS_ASSERT(del3 != del2);

      std::stringstream s;
      s << "delete ( " << f1.toString() << " )";
      TS_ASSERT(s.str() == del1.toString());

      gum::ScheduleDeleteMultiDim< double >* del4 = del3.newFactory();
      TS_ASSERT(*del4 == del3);
      delete del4;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    private:
    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInit(gum::Potential< double >& t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, rand() * 100000.0f / RAND_MAX);
    }
  };

} /* namespace gum_tests */
