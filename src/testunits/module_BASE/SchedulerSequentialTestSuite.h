/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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

#include <agrum/base/graphicalModels/inference/scheduler/schedule.h>
#include <agrum/base/graphicalModels/inference/scheduler/schedulerSequential.h>
#include <agrum/base/multidim/potential.h>
#include <agrum/base/variables/labelizedVariable.h>

namespace gum_tests {

  class [[maybe_unused]] SchedulerSequentialTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_construct1) {
      // reset the ids of the ScheduleMultiDim to avoid conflicts with other
      // testunits
      gum::IScheduleMultiDim::resetIdGenerator();

      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 10);
      }

      gum::Potential< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
      pot1.random();
      gum::ScheduleMultiDim< gum::Potential< double > > f1(pot1, false);

      gum::Potential< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
      pot2.random();
      gum::ScheduleMultiDim< gum::Potential< double > > f2(pot2, false);

      gum::Potential< double > pot3;
      pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
      pot3.random();
      gum::ScheduleMultiDim< gum::Potential< double > > f3(pot3, false);

      gum::Potential< double > pot4;
      pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      pot4.random();
      gum::ScheduleMultiDim< gum::Potential< double > > f4(pot4, false);

      gum::ScheduleBinaryCombination< gum::Potential< double >,
                                      gum::Potential< double >,
                                      gum::Potential< double > >
                                                               comb1(f1, f2, myadd);
      const gum::ScheduleMultiDim< gum::Potential< double > >& result1 = comb1.result();

      gum::ScheduleBinaryCombination< gum::Potential< double >,
                                      gum::Potential< double >,
                                      gum::Potential< double > >
                                                               comb2(f2, f3, myadd);
      const gum::ScheduleMultiDim< gum::Potential< double > >& result2 = comb2.result();

      gum::ScheduleBinaryCombination< gum::Potential< double >,
                                      gum::Potential< double >,
                                      gum::Potential< double > >
                                                               comb3(result2, f4, myadd);
      const gum::ScheduleMultiDim< gum::Potential< double > >& result3 = comb3.result();

      gum::ScheduleBinaryCombination< gum::Potential< double >,
                                      gum::Potential< double >,
                                      gum::Potential< double > >
                                                               comb4(result1, result3, myadd);
      const gum::ScheduleMultiDim< gum::Potential< double > >& result4 = comb4.result();

      gum::ScheduleDeletion< gum::Potential< double > > del1(result1);
      gum::ScheduleDeletion< gum::Potential< double > > del2(result2);
      gum::ScheduleDeletion< gum::Potential< double > > del3(result3);

      gum::Schedule schedule;

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

      gum::Schedule schedule2 = schedule;
      gum::Schedule schedule3 = schedule;
      gum::Schedule schedule4 = schedule;

      gum::SchedulerSequential scheduler;
      TS_ASSERT_DELTA(scheduler.nbOperations(schedule), 2200000.0, 10);
      scheduler.execute(schedule);

      auto&       op4 = const_cast< gum::ScheduleOperator& >(schedule.operation(gum::NodeId(4)));
      const auto& op4_res
          = dynamic_cast< const gum::ScheduleMultiDim< gum::Potential< double > >& >(
              *op4.results()[0]);
      TS_ASSERT(result4.hasSameVariables(op4_res));
      TS_ASSERT(result4.hasSameContent(op4_res));
      TS_ASSERT(!result4.isAbstract());
      TS_ASSERT(!op4_res.isAbstract());

      scheduler.setMaxMemory(2.15 * sizeof(double));
      TS_ASSERT_DELTA(scheduler.nbOperations(schedule2), 2200000.0, 10);

      bool fail = false;
      try {
        scheduler.execute(schedule2);
      } catch (std::bad_alloc&) { fail = true; }
      TS_ASSERT(!fail);

      scheduler.setMaxMemory(2.5 * sizeof(double));
      TS_GUM_ASSERT_THROWS_NOTHING(scheduler.execute(schedule3))
      TS_ASSERT_DELTA(scheduler.memoryUsage(schedule4).first,
                      2100000.0 * sizeof(double) + 3 * sizeof(gum::Potential< double >),
                      10);
      TS_ASSERT_DELTA(scheduler.memoryUsage(schedule4).second,
                      1000000.0 * sizeof(double) + 1 * sizeof(gum::Potential< double >),
                      10);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    private:
    static gum::Potential< double > myadd(const gum::Potential< double >& f1,
                                          const gum::Potential< double >& f2) {
      return f1 + f2;
    }
  };

} /* namespace gum_tests */
