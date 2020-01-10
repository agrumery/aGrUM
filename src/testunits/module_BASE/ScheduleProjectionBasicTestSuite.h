
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
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/graphicalModels/inference/scheduleProjectionBasic.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/utils/operators/multiDimProjection.h>
#include <agrum/variables/labelizedVariable.h>

namespace gum_tests {

  static gum::MultiDimImplementation< double >* schedule_proj_mysum(
     const gum::MultiDimImplementation< double >&    t,
     const gum::Set< const gum::DiscreteVariable* >& del_vars) {
    return projectSumMultiDimArray(&t, del_vars);
  }

  static gum::MultiDimImplementation< double >* schedule_proj_mymin(
     const gum::MultiDimImplementation< double >&    t,
     const gum::Set< const gum::DiscreteVariable* >& del_vars) {
    return projectMinMultiDimArray(&t, del_vars);
  }

  class ScheduleProjectionBasicTestSuite: public CxxTest::TestSuite {
    public:
    void test_construct1() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential< double > t1;
      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);
      gum::Set< const gum::DiscreteVariable* > proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimProjection< double, gum::MultiDimImplementation > mymultiproj(
         schedule_proj_mysum);
      gum::MultiDimImplementation< double >* t2 =
         mymultiproj.project(*(t1.content()), del_vars);

      gum::ScheduleProjectionBasic< double > myproj(schedule_proj_mysum);
      gum::Schedule< double >                schedule;
      gum::ScheduleMultiDim< double > tt2 = myproj.project(t1, del_vars, schedule);
      const gum::NodeSet&             available = schedule.availableOperations();

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                available.beginSafe();   // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      TS_ASSERT(tt2.multiDim() == *t2);
      gum::ScheduleDeleteMultiDim< double > del2(tt2);
      del2.execute();
      auto dom = t1.domainSize();
      TS_ASSERT(myproj.nbOperations(t1, del_vars, schedule) == dom);
      delete t2;

      mymultiproj.setProjectFunction(schedule_proj_mymin);
      t2 = mymultiproj.project(*(t1.content()), del_vars);
      myproj.setProjectFunction(schedule_proj_mymin);
      gum::ScheduleMultiDim< double > t3(*(t1.content()));
      tt2 = myproj.project(t3, del_vars, schedule);

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                available.beginSafe();   // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      TS_ASSERT(tt2.multiDim() == *t2);
      TS_ASSERT(myproj.nbOperations(t3, del_vars, schedule) == dom);
      gum::ScheduleDeleteMultiDim< double > del3(tt2);
      del3.execute();

      tt2 = myproj.project(*(t1.content()), del_vars, schedule);

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                available.beginSafe();   // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      TS_ASSERT(tt2.multiDim() == *t2);
      TS_ASSERT(myproj.nbOperations(*(t1.content()), del_vars, schedule) == dom);
      gum::ScheduleDeleteMultiDim< double > del4(tt2);
      del4.execute();

      gum::ScheduleProjectionBasic< double > xxx = myproj;
      tt2 = xxx.project(t3, del_vars, schedule);

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                available.beginSafe();   // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      TS_ASSERT(tt2.multiDim() == *t2);
      TS_ASSERT(xxx.nbOperations(t3, del_vars, schedule) == dom);
      gum::ScheduleDeleteMultiDim< double > del5(tt2);
      del5.execute();

      gum::ScheduleProjectionBasic< double >* yyy = myproj.newFactory();
      tt2 = yyy->project(t3, del_vars, schedule);

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                available.beginSafe();   // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      TS_ASSERT(tt2.multiDim() == *t2);
      TS_ASSERT(yyy->nbOperations(t3, del_vars, schedule) == dom);
      gum::ScheduleDeleteMultiDim< double > del6(tt2);
      del6.execute();

      delete t2;
      delete yyy;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_construct2() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential< double > t1;
      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);
      gum::Set< const gum::DiscreteVariable* > proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::ScheduleProjectionBasic< double > myproj(schedule_proj_mysum);
      gum::Schedule< double >                schedule;
      std::pair< long, long > xxx = myproj.memoryUsage(t1, del_vars, schedule);
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
    void randomInit(gum::Potential< double >* t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t->set(i, rand() * 100000.0f / RAND_MAX);
    }
  };

} /* namespace gum_tests */
