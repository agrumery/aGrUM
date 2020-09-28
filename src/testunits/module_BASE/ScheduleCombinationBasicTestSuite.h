
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

#include <agrum/tools/graphicalModels/inference/scheduler/scheduleCombinationBasic.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/multidim/utils/operators/multiDimCombinationDefault.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  static gum::MultiDimImplementation< double >*
     schedule_comb_myadd(const gum::MultiDimImplementation< double >& f1,
                         const gum::MultiDimImplementation< double >& f2) {
    return f1 + f2;
  }

  class ScheduleCombinationTestSuite: public CxxTest::TestSuite {
    public:
    void test_construct1() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Potential< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
      randomInit(pot1);
      gum::ScheduleMultiDim< double > f1(pot1);

      gum::Potential< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
      randomInit(pot2);
      gum::ScheduleMultiDim< double > f2(pot2);
      gum::ScheduleMultiDim< double > f2bis(pot2);

      gum::Potential< double > pot3;
      pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
      randomInit(pot3);
      gum::ScheduleMultiDim< double > f3(pot3);

      gum::Potential< double > pot4;
      pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      randomInit(pot4);
      gum::ScheduleMultiDim< double > f4(pot4);

      gum::ScheduleCombinationBasic< double >            comb(schedule_comb_myadd);
      gum::Set< const gum::ScheduleMultiDim< double >* > set;
      set << &f1 << &f2 << &f2bis << &f3 << &f4;
      gum::Schedule< double >               schedule;
      const gum::ScheduleMultiDim< double > result = comb.combine(set, schedule);

      TS_ASSERT(comb.nbOperations(set, schedule) == 120);

      const gum::NodeSet& available = schedule.availableOperations();

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                available.beginSafe();   // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      gum::ScheduleCombine< double > comb11(f1, f2, schedule_comb_myadd);
      comb11.execute();
      const gum::ScheduleMultiDim< double >& result11 = comb11.result();
      gum::ScheduleCombine< double >         comb12(f2, f3, schedule_comb_myadd);
      comb12.execute();
      const gum::ScheduleMultiDim< double >& result12 = comb12.result();
      gum::ScheduleCombine< double > comb13(result12, f4, schedule_comb_myadd);
      comb13.execute();
      const gum::ScheduleMultiDim< double >& result13 = comb13.result();
      gum::ScheduleCombine< double >         comb14(
         result11, result13, schedule_comb_myadd);
      comb14.execute();
      const gum::ScheduleMultiDim< double >& result14 = comb14.result();

      TS_ASSERT(result14.multiDim() == result.multiDim());

      gum::ScheduleDeleteMultiDim< double > del4(result);
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
        vars[i] = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Potential< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
      randomInit(pot1);

      gum::Potential< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
      randomInit(pot2);

      gum::Potential< double > pot3;
      pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
      randomInit(pot3);

      gum::Potential< double > pot4;
      pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      randomInit(pot4);

      gum::ScheduleCombinationBasic< double > comb(schedule_comb_myadd);
      gum::Set< const gum::MultiDimImplementation< double >* > set;
      set << pot1.content() << pot2.content() << pot3.content() << pot4.content();
      gum::Schedule< double >               schedule;
      const gum::ScheduleMultiDim< double > result = comb.combine(set, schedule);

      TS_ASSERT(comb.nbOperations(set, schedule) == 112);

      const gum::NodeSet& available = schedule.availableOperations();

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                available.beginSafe();   // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      gum::MultiDimCombinationDefault< double, gum::MultiDimImplementation >
                                             comb11(schedule_comb_myadd);
      gum::MultiDimImplementation< double >* res11 = comb11.combine(set);

      TS_ASSERT(*res11 == result.multiDim());

      gum::ScheduleDeleteMultiDim< double > del4(result);
      del4.execute();
      delete res11;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_construct3() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Potential< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
      randomInit(pot1);

      gum::Potential< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
      randomInit(pot2);

      gum::Potential< double > pot3;
      pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
      randomInit(pot3);

      gum::Potential< double > pot4;
      pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      randomInit(pot4);

      gum::ScheduleCombinationBasic< double >     comb(schedule_comb_myadd);
      gum::Set< const gum::Potential< double >* > set;
      set << &pot1 << &pot2 << &pot3 << &pot4;
      gum::Schedule< double >               schedule;
      const gum::ScheduleMultiDim< double > result = comb.combine(set, schedule);

      TS_ASSERT(comb.nbOperations(set, schedule) == 112);

      const gum::NodeSet& available = schedule.availableOperations();

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                available.beginSafe();   // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      gum::MultiDimCombinationDefault< double, gum::MultiDimImplementation >
                                                               comb11(schedule_comb_myadd);
      gum::Set< const gum::MultiDimImplementation< double >* > set2;
      set2 << pot1.content() << pot2.content() << pot3.content() << pot4.content();
      gum::MultiDimImplementation< double >* res11 = comb11.combine(set2);

      TS_ASSERT(*res11 == result.multiDim());

      gum::ScheduleDeleteMultiDim< double > del4(result);
      del4.execute();
      delete res11;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_construct4() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential< double > t1, t2, t3;
      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
      t2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
      t3 << *(vars[6]) << *(vars[4]) << *(vars[3]);

      randomInit(t1);
      randomInit(t2);
      randomInit(t3);

      gum::ScheduleCombinationBasic< double >     comb(schedule_comb_myadd);
      gum::Set< const gum::Potential< double >* > set;
      set << &t1 << &t2 << &t3;
      gum::Set< const gum::MultiDimImplementation< double >* > set2;
      set2 << t1.content() << t2.content() << t3.content();

      gum::Schedule< double > schedule;

      TS_ASSERT(comb.nbOperations(set, schedule) == 16640);
      std::pair< long, long > yyy = comb.memoryUsage(set, schedule);
      TS_ASSERT(yyy.first == 16640);
      TS_ASSERT(yyy.second == 16384);
      std::pair< long, long > zzz = comb.memoryUsage(set2, schedule);
      TS_ASSERT(zzz.first == 16640);
      TS_ASSERT(zzz.second == 16384);

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
        t.set(i, rand() * 100.0f / RAND_MAX);
    }
  };

} /* namespace gum_tests */
