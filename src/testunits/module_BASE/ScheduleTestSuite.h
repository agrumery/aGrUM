/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
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

#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/potential.h>
#include <agrum/graphicalModels/inference/schedule.h>

namespace gum_tests {

  static gum::MultiDimImplementation<float> *
  schedule_myadd(const gum::MultiDimImplementation<float> &f1,
                 const gum::MultiDimImplementation<float> &f2) {
    return f1 + f2;
  }

  class ScheduleTestSuite : public CxxTest::TestSuite {
    public:
    void test_construct1() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Potential<float> pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
      randomInit(pot1);
      gum::ScheduleMultiDim<float> f1(pot1);

      gum::Potential<float> pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
      randomInit(pot2);
      gum::ScheduleMultiDim<float> f2(pot2);

      gum::Potential<float> pot3;
      pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
      randomInit(pot3);
      gum::ScheduleMultiDim<float> f3(pot3);

      gum::Potential<float> pot4;
      pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      randomInit(pot4);
      gum::ScheduleMultiDim<float> f4(pot4);

      gum::ScheduleCombine<float> comb1(f1, f2, schedule_myadd);
      const gum::ScheduleMultiDim<float> &result1 = comb1.result();

      gum::ScheduleCombine<float> comb2(f2, f3, schedule_myadd);
      const gum::ScheduleMultiDim<float> &result2 = comb2.result();

      gum::ScheduleCombine<float> comb3(result2, f4, schedule_myadd);
      const gum::ScheduleMultiDim<float> &result3 = comb3.result();

      gum::ScheduleCombine<float> comb4(result1, result3, schedule_myadd);
      const gum::ScheduleMultiDim<float> &result4 = comb4.result();

      gum::ScheduleDeleteMultiDim<float> del1(result1);
      gum::ScheduleDeleteMultiDim<float> del2(result2);
      gum::ScheduleDeleteMultiDim<float> del3(result3);
      gum::ScheduleDeleteMultiDim<float> del4(result4);

      gum::Schedule<float> schedule;

      schedule.insert(comb4);
      TS_ASSERT(schedule.availableOperations().size() == 0);

      schedule.insert(comb2);
      TS_ASSERT(schedule.availableOperations().size() == 1);

      schedule.insert(comb3);
      TS_ASSERT(schedule.availableOperations().size() == 1);

      schedule.insert(comb1);
      TS_ASSERT(schedule.availableOperations().size() == 2);

      TS_ASSERT(schedule.dag().sizeArcs() == 3);
      TS_ASSERT(comb1 == schedule.operation(3));
      TS_ASSERT(3 == schedule.nodeId(comb1));

      const gum::NodeSet &ops1 = schedule.operationsInvolving(result1);
      gum::NodeId del1_id = schedule.insert(del1);
      schedule.forceAfter(del1_id, ops1);

      schedule.insert(del2);
      const gum::NodeSet &ops2 = schedule.operationsInvolving(result2.id());
      gum::Set<const gum::ScheduleOperation<float> *> oops2;

      for (gum::NodeSet::const_iterator_safe iter =
               ops2.beginSafe(); // safe iterator needed here
           iter != ops2.endSafe();
           ++iter) {
        oops2.insert(&(schedule.operation(*iter)));
      }

      schedule.forceAfter(del2, oops2);

      schedule.insert(del3);
      schedule.forceAfter(del3, comb4);

      schedule.execute(comb1);
      gum::Schedule<float> schedule2 = schedule;
      TS_ASSERT(schedule2.availableOperations().size() == 1);

      const gum::NodeSet &available = schedule.availableOperations();

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                 available.beginSafe(); // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      gum::ScheduleCombine<float> comb11(f1, f2, schedule_myadd);
      comb11.execute();
      const gum::ScheduleMultiDim<float> &result11 = comb11.result();
      gum::ScheduleCombine<float> comb12(f2, f3, schedule_myadd);
      comb12.execute();
      const gum::ScheduleMultiDim<float> &result12 = comb12.result();
      gum::ScheduleCombine<float> comb13(result12, f4, schedule_myadd);
      comb13.execute();
      const gum::ScheduleMultiDim<float> &result13 = comb13.result();
      gum::ScheduleCombine<float> comb14(result11, result13, schedule_myadd);
      comb14.execute();
      const gum::ScheduleMultiDim<float> &result14 = comb14.result();

      TS_ASSERT(result14.multiDim() == result4.multiDim());

      gum::ScheduleDeleteMultiDim<float> del11(result11);
      gum::ScheduleDeleteMultiDim<float> del12(result12);
      gum::ScheduleDeleteMultiDim<float> del13(result13);
      gum::ScheduleDeleteMultiDim<float> del14(result14);
      del4.execute();
      del11.execute();
      del12.execute();
      del13.execute();
      del14.execute();

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_construct2() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Potential<float> pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
      randomInit(pot1);
      gum::ScheduleMultiDim<float> f1(pot1);

      gum::Potential<float> pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
      randomInit(pot2);
      gum::ScheduleMultiDim<float> f2(pot2);

      gum::Potential<float> pot3;
      pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
      randomInit(pot3);
      gum::ScheduleMultiDim<float> f3(pot3);

      gum::Potential<float> pot4;
      pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      randomInit(pot4);
      gum::ScheduleMultiDim<float> f4(pot4);

      gum::ScheduleCombine<float> comb1(f1, f2, schedule_myadd);
      const gum::ScheduleMultiDim<float> &result1 = comb1.result();

      gum::ScheduleCombine<float> comb2(f2, f3, schedule_myadd);
      const gum::ScheduleMultiDim<float> &result2 = comb2.result();

      gum::ScheduleCombine<float> comb3(result2, f4, schedule_myadd);
      const gum::ScheduleMultiDim<float> &result3 = comb3.result();

      gum::ScheduleCombine<float> comb4(result1, result3, schedule_myadd);
      const gum::ScheduleMultiDim<float> &result4 = comb4.result();

      gum::ScheduleDeleteMultiDim<float> del1(result1);
      gum::ScheduleDeleteMultiDim<float> del2(result2);
      gum::ScheduleDeleteMultiDim<float> del3(result3);
      gum::ScheduleDeleteMultiDim<float> del4(result4);

      gum::Schedule<float> schedule;

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

      TS_ASSERT(schedule.dag().sizeArcs() == 3);
      TS_ASSERT(comb1 == schedule.operation(3));
      TS_ASSERT(3 == schedule.nodeId(comb1));

      gum::NodeId del1_id = schedule.insert(del1);
      schedule.forceBefore(schedule.nodeId(comb4), del1_id);

      schedule.insert(del2);
      schedule.forceBefore(comb3, del2);

      schedule.insert(del3);
      schedule.forceAfter(del3, comb4);

      schedule.execute(comb1);
      gum::Schedule<float> schedule2 = schedule;
      TS_ASSERT(schedule2.availableOperations().size() == 1);

      const gum::NodeSet &available = schedule.availableOperations();

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                 available.beginSafe(); // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      gum::ScheduleCombine<float> comb11(f1, f2, schedule_myadd);
      comb11.execute();
      const gum::ScheduleMultiDim<float> &result11 = comb11.result();
      gum::ScheduleCombine<float> comb12(f2, f3, schedule_myadd);
      comb12.execute();
      const gum::ScheduleMultiDim<float> &result12 = comb12.result();
      gum::ScheduleCombine<float> comb13(result12, f4, schedule_myadd);
      comb13.execute();
      const gum::ScheduleMultiDim<float> &result13 = comb13.result();
      gum::ScheduleCombine<float> comb14(result11, result13, schedule_myadd);
      comb14.execute();
      const gum::ScheduleMultiDim<float> &result14 = comb14.result();

      TS_ASSERT(result14.multiDim() == result4.multiDim());

      gum::ScheduleDeleteMultiDim<float> del11(result11);
      gum::ScheduleDeleteMultiDim<float> del12(result12);
      gum::ScheduleDeleteMultiDim<float> del13(result13);
      gum::ScheduleDeleteMultiDim<float> del14(result14);
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
    void randomInit(gum::Potential<float> &t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, (int)(((float)rand() / RAND_MAX) * 100000));
    }
  };

} /* namespace gum_tests */
