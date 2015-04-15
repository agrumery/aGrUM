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
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/operators/multiDimProjection.h>
#include <agrum/graphicalModels/inference/scheduleProjectionBasic.h>

namespace gum_tests {

  static gum::MultiDimImplementation<float> *
  schedule_proj_mysum(const gum::MultiDimImplementation<float> &t,
                      const gum::Set<const gum::DiscreteVariable *> &del_vars) {
    return projectSumMultiDimArray(&t, del_vars);
  }

  static gum::MultiDimImplementation<float> *
  schedule_proj_mymin(const gum::MultiDimImplementation<float> &t,
                      const gum::Set<const gum::DiscreteVariable *> &del_vars) {
    return projectMinMultiDimArray(&t, del_vars);
  }

  class ScheduleProjectionBasicTestSuite : public CxxTest::TestSuite {
    public:
    void test_construct1() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential<float> t1;
      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);
      gum::Set<const gum::DiscreteVariable *> proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimProjection<float, gum::MultiDimImplementation> mymultiproj(
          schedule_proj_mysum);
      gum::MultiDimImplementation<float> *t2 =
          mymultiproj.project(*(t1.content()), del_vars);

      gum::ScheduleProjectionBasic<float> myproj(schedule_proj_mysum);
      gum::Schedule<float> schedule;
      gum::ScheduleMultiDim<float> tt2 = myproj.project(t1, del_vars, schedule);
      const gum::NodeSet &available = schedule.availableOperations();

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                 available.beginSafe(); // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      TS_ASSERT(tt2.multiDim() == *t2);
      gum::ScheduleDeleteMultiDim<float> del2(tt2);
      del2.execute();
      float dom = t1.domainSize();
      TS_ASSERT(myproj.nbOperations(t1, del_vars, schedule) == dom);
      delete t2;

      mymultiproj.setProjectFunction(schedule_proj_mymin);
      t2 = mymultiproj.project(*(t1.content()), del_vars);
      myproj.setProjectFunction(schedule_proj_mymin);
      gum::ScheduleMultiDim<float> t3(*(t1.content()));
      tt2 = myproj.project(t3, del_vars, schedule);

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                 available.beginSafe(); // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      TS_ASSERT(tt2.multiDim() == *t2);
      TS_ASSERT(myproj.nbOperations(t3, del_vars, schedule) == dom);
      gum::ScheduleDeleteMultiDim<float> del3(tt2);
      del3.execute();

      tt2 = myproj.project(*(t1.content()), del_vars, schedule);

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                 available.beginSafe(); // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      TS_ASSERT(tt2.multiDim() == *t2);
      TS_ASSERT(myproj.nbOperations(*(t1.content()), del_vars, schedule) == dom);
      gum::ScheduleDeleteMultiDim<float> del4(tt2);
      del4.execute();

      gum::ScheduleProjectionBasic<float> xxx = myproj;
      tt2 = xxx.project(t3, del_vars, schedule);

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                 available.beginSafe(); // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      TS_ASSERT(tt2.multiDim() == *t2);
      TS_ASSERT(xxx.nbOperations(t3, del_vars, schedule) == dom);
      gum::ScheduleDeleteMultiDim<float> del5(tt2);
      del5.execute();

      gum::ScheduleProjectionBasic<float> *yyy = myproj.newFactory();
      tt2 = yyy->project(t3, del_vars, schedule);

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                 available.beginSafe(); // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      TS_ASSERT(tt2.multiDim() == *t2);
      TS_ASSERT(yyy->nbOperations(t3, del_vars, schedule) == dom);
      gum::ScheduleDeleteMultiDim<float> del6(tt2);
      del6.execute();

      delete t2;
      delete yyy;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_construct2() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential<float> t1;
      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);
      gum::Set<const gum::DiscreteVariable *> proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::ScheduleProjectionBasic<float> myproj(schedule_proj_mysum);
      gum::Schedule<float> schedule;
      std::pair<long, long> xxx = myproj.memoryUsage(t1, del_vars, schedule);
      TS_ASSERT(xxx.first == 16384);
      TS_ASSERT(xxx.second == 16384);

      xxx = myproj.memoryUsage(*(t1.content()), del_vars, schedule);
      TS_ASSERT(xxx.first == 16384);
      TS_ASSERT(xxx.second == 16384);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    private:
    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInit(gum::Potential<float> *t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t->set(i, (int)(((float)rand() / RAND_MAX) * 100000));
    }
  };

} /* namespace gum_tests */
