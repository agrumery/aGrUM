/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  static gum::MultiDimImplementation< double >*
     schedule_myadd(const gum::MultiDimImplementation< double >& f1,
                    const gum::MultiDimImplementation< double >& f2) {
    return f1 + f2;
  }

  class ScheduleTestSuite: public CxxTest::TestSuite {
    public:
    void test_construct1() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Potential< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
      randomInit(pot1);
      gum::ScheduleMultiDim< double > f1(pot1);

      gum::Potential< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
      randomInit(pot2);
      gum::ScheduleMultiDim< double > f2(pot2);

      gum::Potential< double > pot3;
      pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
      randomInit(pot3);
      gum::ScheduleMultiDim< double > f3(pot3);

      gum::Potential< double > pot4;
      pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      randomInit(pot4);
      gum::ScheduleMultiDim< double > f4(pot4);

      gum::ScheduleCombine< double >         comb1(f1, f2, schedule_myadd);
      const gum::ScheduleMultiDim< double >& result1 = comb1.result();

      gum::ScheduleCombine< double >         comb2(f2, f3, schedule_myadd);
      const gum::ScheduleMultiDim< double >& result2 = comb2.result();

      gum::ScheduleCombine< double >         comb3(result2, f4, schedule_myadd);
      const gum::ScheduleMultiDim< double >& result3 = comb3.result();

      gum::ScheduleCombine< double >         comb4(result1, result3, schedule_myadd);
      const gum::ScheduleMultiDim< double >& result4 = comb4.result();

      gum::ScheduleDeleteMultiDim< double > del1(result1);
      gum::ScheduleDeleteMultiDim< double > del2(result2);
      gum::ScheduleDeleteMultiDim< double > del3(result3);
      gum::ScheduleDeleteMultiDim< double > del4(result4);

      gum::Schedule< double > schedule;

      schedule.insert(comb4);
      TS_ASSERT(schedule.availableOperations().size() == 0);

      schedule.insert(comb2);
      TS_ASSERT(schedule.availableOperations().size() == 1);

      schedule.insert(comb3);
      TS_ASSERT(schedule.availableOperations().size() == 1);

      schedule.insert(comb1);
      TS_ASSERT(schedule.availableOperations().size() == 2);

      TS_ASSERT(schedule.scheduling_dag().sizeArcs() == 3);
      TS_ASSERT(comb1 == schedule.operation(3));
      TS_ASSERT(3 == schedule.nodeId(comb1));

      const gum::NodeSet& ops1    = schedule.operationsInvolving(result1);
      gum::NodeId         del1_id = schedule.insert(del1);
      schedule.forceAfter(del1_id, ops1);

      schedule.insert(del2);
      const gum::NodeSet& ops2 = schedule.operationsInvolving(result2.id());
      gum::Set< const gum::ScheduleOperation< double >* > oops2;

      for (gum::NodeSet::const_iterator_safe iter = ops2.beginSafe();   // safe iterator needed here
           iter != ops2.endSafe();
           ++iter) {
        oops2.insert(&(schedule.operation(*iter)));
      }

      schedule.forceAfter(del2, oops2);

      schedule.insert(del3);
      schedule.forceAfter(del3, comb4);

      schedule.execute(comb1);
      gum::Schedule< double > schedule2 = schedule;
      TS_ASSERT(schedule2.availableOperations().size() == 1);

      const gum::NodeSet& available = schedule.availableOperations();

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter
             = available.beginSafe();   // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      gum::ScheduleCombine< double > comb11(f1, f2, schedule_myadd);
      comb11.execute();
      const gum::ScheduleMultiDim< double >& result11 = comb11.result();
      gum::ScheduleCombine< double >         comb12(f2, f3, schedule_myadd);
      comb12.execute();
      const gum::ScheduleMultiDim< double >& result12 = comb12.result();
      gum::ScheduleCombine< double >         comb13(result12, f4, schedule_myadd);
      comb13.execute();
      const gum::ScheduleMultiDim< double >& result13 = comb13.result();
      gum::ScheduleCombine< double >         comb14(result11, result13, schedule_myadd);
      comb14.execute();
      const gum::ScheduleMultiDim< double >& result14 = comb14.result();

      TS_ASSERT(result14.multiDim() == result4.multiDim());

      gum::ScheduleDeleteMultiDim< double > del11(result11);
      gum::ScheduleDeleteMultiDim< double > del12(result12);
      gum::ScheduleDeleteMultiDim< double > del13(result13);
      gum::ScheduleDeleteMultiDim< double > del14(result14);
      del4.execute();
      del11.execute();
      del12.execute();
      del13.execute();
      del14.execute();

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_construct2() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Potential< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
      randomInit(pot1);
      gum::ScheduleMultiDim< double > f1(pot1);

      gum::Potential< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
      randomInit(pot2);
      gum::ScheduleMultiDim< double > f2(pot2);

      gum::Potential< double > pot3;
      pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
      randomInit(pot3);
      gum::ScheduleMultiDim< double > f3(pot3);

      gum::Potential< double > pot4;
      pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      randomInit(pot4);
      gum::ScheduleMultiDim< double > f4(pot4);

      gum::ScheduleCombine< double >         comb1(f1, f2, schedule_myadd);
      const gum::ScheduleMultiDim< double >& result1 = comb1.result();

      gum::ScheduleCombine< double >         comb2(f2, f3, schedule_myadd);
      const gum::ScheduleMultiDim< double >& result2 = comb2.result();

      gum::ScheduleCombine< double >         comb3(result2, f4, schedule_myadd);
      const gum::ScheduleMultiDim< double >& result3 = comb3.result();

      gum::ScheduleCombine< double >         comb4(result1, result3, schedule_myadd);
      const gum::ScheduleMultiDim< double >& result4 = comb4.result();

      gum::ScheduleDeleteMultiDim< double > del1(result1);
      gum::ScheduleDeleteMultiDim< double > del2(result2);
      gum::ScheduleDeleteMultiDim< double > del3(result3);
      gum::ScheduleDeleteMultiDim< double > del4(result4);

      gum::Schedule< double > schedule;

      gum::NodeId id = schedule.insert(comb4);
      TS_ASSERT(schedule.availableOperations().size() == 0);
      TS_ASSERT(schedule.nbOperations(id) == 64);
      TS_ASSERT(schedule.memoryUsage(id).first == 64);
      TS_ASSERT(schedule.memoryUsage(id).second == 64);
      TS_ASSERT(schedule.nbOperations(comb4) == 64);
      TS_ASSERT(schedule.memoryUsage(comb4).first == 64);
      TS_ASSERT(schedule.memoryUsage(comb4).second == 64);

      schedule.insert(comb2);
      TS_ASSERT(schedule.availableOperations().size() == 1);

      schedule.insert(comb3);
      TS_ASSERT(schedule.availableOperations().size() == 1);

      schedule.insert(comb1);
      TS_ASSERT(schedule.availableOperations().size() == 2);

      TS_ASSERT(schedule.scheduling_dag().sizeArcs() == 3);
      TS_ASSERT(comb1 == schedule.operation(3));
      TS_ASSERT(3 == schedule.nodeId(comb1));

      gum::NodeId del1_id = schedule.insert(del1);
      schedule.forceBefore(schedule.nodeId(comb4), del1_id);

      schedule.insert(del2);
      schedule.forceBefore(comb3, del2);

      schedule.insert(del3);
      schedule.forceAfter(del3, comb4);

      schedule.execute(comb1);
      gum::Schedule< double > schedule2 = schedule;
      TS_ASSERT(schedule2.availableOperations().size() == 1);

      const gum::NodeSet& available = schedule.availableOperations();

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter
             = available.beginSafe();   // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      gum::ScheduleCombine< double > comb11(f1, f2, schedule_myadd);
      comb11.execute();
      const gum::ScheduleMultiDim< double >& result11 = comb11.result();
      gum::ScheduleCombine< double >         comb12(f2, f3, schedule_myadd);
      comb12.execute();
      const gum::ScheduleMultiDim< double >& result12 = comb12.result();
      gum::ScheduleCombine< double >         comb13(result12, f4, schedule_myadd);
      comb13.execute();
      const gum::ScheduleMultiDim< double >& result13 = comb13.result();
      gum::ScheduleCombine< double >         comb14(result11, result13, schedule_myadd);
      comb14.execute();
      const gum::ScheduleMultiDim< double >& result14 = comb14.result();

      TS_ASSERT(result14.multiDim() == result4.multiDim());

      gum::ScheduleDeleteMultiDim< double > del11(result11);
      gum::ScheduleDeleteMultiDim< double > del12(result12);
      gum::ScheduleDeleteMultiDim< double > del13(result13);
      gum::ScheduleDeleteMultiDim< double > del14(result14);
      del4.execute();
      del11.execute();
      del12.execute();
      del13.execute();
      del14.execute();

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
