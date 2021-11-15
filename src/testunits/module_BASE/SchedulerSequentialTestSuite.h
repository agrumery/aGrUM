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


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/graphicalModels/inference/scheduler/schedule.h>
#include <agrum/tools/graphicalModels/inference/scheduler/schedulerSequential.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  class SchedulerSequentialTestSuite: public CxxTest::TestSuite {
    public:
    void test_construct1() {
      // reset the ids of the ScheduleMultiDim to avoid conflicts with other
      // testunits
      gum::IScheduleMultiDim<>::resetIdGenerator();

      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 10);
      }

      gum::Potential< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
      randomInit(pot1);
      gum::ScheduleMultiDim< gum::Potential< double > > f1(pot1, false);

      gum::Potential< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
      randomInit(pot2);
      gum::ScheduleMultiDim< gum::Potential< double > > f2(pot2, false);

      gum::Potential< double > pot3;
      pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
      randomInit(pot3);
      gum::ScheduleMultiDim< gum::Potential< double > > f3(pot3, false);

      gum::Potential< double > pot4;
      pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      randomInit(pot4);
      gum::ScheduleMultiDim< gum::Potential< double > > f4(pot4, false);

      gum::ScheduleBinaryCombination< gum::Potential< double >,
                                      gum::Potential< double >,
                                      gum::Potential< double > > comb1(f1, f2, myadd);
      const gum::ScheduleMultiDim< gum::Potential< double > >&   result1 = comb1.result();

      gum::ScheduleBinaryCombination< gum::Potential< double >,
                                      gum::Potential< double >,
                                      gum::Potential< double > > comb2(f2, f3, myadd);
      const gum::ScheduleMultiDim< gum::Potential< double > >&   result2 = comb2.result();

      gum::ScheduleBinaryCombination< gum::Potential< double >,
                                      gum::Potential< double >,
                                      gum::Potential< double > > comb3(result2, f4, myadd);
      const gum::ScheduleMultiDim< gum::Potential< double > >&   result3 = comb3.result();

      gum::ScheduleBinaryCombination< gum::Potential< double >,
                                      gum::Potential< double >,
                                      gum::Potential< double > > comb4(result1, result3, myadd);
      const gum::ScheduleMultiDim< gum::Potential< double > >&   result4 = comb4.result();

      gum::ScheduleDeletion< gum::Potential< double > > del1(result1);
      gum::ScheduleDeletion< gum::Potential< double > > del2(result2);
      gum::ScheduleDeletion< gum::Potential< double > > del3(result3);

      gum::Schedule<> schedule;

      schedule.insertScheduleMultiDim(f1);
      schedule.insertScheduleMultiDim(f2);
      schedule.insertScheduleMultiDim(f3);
      schedule.insertScheduleMultiDim(f4);

      schedule.insertOperation(comb1);
      schedule.insertOperation(comb2);
      schedule.insertOperation(comb3);
      schedule.insertOperation(comb4);
      schedule.insertOperation(del1);
      schedule.insertOperation(del2);
      schedule.insertOperation(del3);

      comb1.execute();
      comb2.execute();
      comb3.execute();
      comb4.execute();
      del1.execute();
      del2.execute();
      del3.execute();

      gum::Schedule<> schedule2 = schedule;
      gum::Schedule<> schedule3 = schedule;
      gum::Schedule<> schedule4 = schedule;

      gum::SchedulerSequential<> scheduler;
      TS_ASSERT_DELTA(scheduler.nbOperations(schedule), 2200000.0, 10);
      scheduler.execute(schedule);

      gum::ScheduleOperation<>& op4 =
         const_cast<gum::ScheduleOperation<>&>(
            schedule.operation(gum::NodeId(4)));
      const gum::ScheduleMultiDim< gum::Potential< double > >& op4_res =
         dynamic_cast< const gum::ScheduleMultiDim< gum::Potential< double > >& >(
            *op4.results()[0]);
      TS_ASSERT(result4.hasSameVariables(op4_res));
      TS_ASSERT(result4.hasSameContent(op4_res));
      TS_ASSERT(!result4.isAbstract());
      TS_ASSERT(!op4_res.isAbstract());

      scheduler.setMaxMemory(2.15 * sizeof(double));
      TS_ASSERT_DELTA(scheduler.nbOperations(schedule2), 2200000.0, 10);

      bool fail = false;
      try { scheduler.execute(schedule2); }
      catch (std::bad_alloc&) { fail = true; }
      TS_ASSERT(!fail);

      scheduler.setMaxMemory(2.5 * sizeof(double));
      TS_GUM_ASSERT_THROWS_NOTHING(scheduler.execute(schedule3));

      TS_ASSERT_DELTA(scheduler.memoryUsage(schedule4).first, 2100000.0 * sizeof(double), 10);
      TS_ASSERT_DELTA(scheduler.memoryUsage(schedule4).second, 1000000.0 * sizeof(double), 10);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    private:
    static gum::Potential< double > myadd(const gum::Potential< double >& f1,
                                          const gum::Potential< double >& f2) {
      return f1 + f2;
    }

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
