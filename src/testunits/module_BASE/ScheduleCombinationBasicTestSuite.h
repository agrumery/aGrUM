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
#include <agrum/multidim/operators/multiDimCombinationDefault.h>
#include <agrum/graphicalModels/inference/scheduleCombinationBasic.h>

namespace gum_tests {

  static gum::MultiDimImplementation<float> *
  schedule_comb_myadd(const gum::MultiDimImplementation<float> &f1,
                      const gum::MultiDimImplementation<float> &f2) {
    return f1 + f2;
  }

  class ScheduleCombinationTestSuite : public CxxTest::TestSuite {
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
      gum::ScheduleMultiDim<float> f2bis(pot2);

      gum::Potential<float> pot3;
      pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
      randomInit(pot3);
      gum::ScheduleMultiDim<float> f3(pot3);

      gum::Potential<float> pot4;
      pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      randomInit(pot4);
      gum::ScheduleMultiDim<float> f4(pot4);

      gum::ScheduleCombinationBasic<float> comb(schedule_comb_myadd);
      gum::Set<const gum::ScheduleMultiDim<float> *> set;
      set << &f1 << &f2 << &f2bis << &f3 << &f4;
      gum::Schedule<float> schedule;
      const gum::ScheduleMultiDim<float> result = comb.combine(set, schedule);

      TS_ASSERT(comb.nbOperations(set, schedule) == 120);

      const gum::NodeSet &available = schedule.availableOperations();

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                 available.beginSafe(); // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      gum::ScheduleCombine<float> comb11(f1, f2, schedule_comb_myadd);
      comb11.execute();
      const gum::ScheduleMultiDim<float> &result11 = comb11.result();
      gum::ScheduleCombine<float> comb12(f2, f3, schedule_comb_myadd);
      comb12.execute();
      const gum::ScheduleMultiDim<float> &result12 = comb12.result();
      gum::ScheduleCombine<float> comb13(result12, f4, schedule_comb_myadd);
      comb13.execute();
      const gum::ScheduleMultiDim<float> &result13 = comb13.result();
      gum::ScheduleCombine<float> comb14(result11, result13, schedule_comb_myadd);
      comb14.execute();
      const gum::ScheduleMultiDim<float> &result14 = comb14.result();

      TS_ASSERT(result14.multiDim() == result.multiDim());

      gum::ScheduleDeleteMultiDim<float> del4(result);
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

      gum::Potential<float> pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
      randomInit(pot2);

      gum::Potential<float> pot3;
      pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
      randomInit(pot3);

      gum::Potential<float> pot4;
      pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      randomInit(pot4);

      gum::ScheduleCombinationBasic<float> comb(schedule_comb_myadd);
      gum::Set<const gum::MultiDimImplementation<float> *> set;
      set << pot1.content() << pot2.content() << pot3.content() << pot4.content();
      gum::Schedule<float> schedule;
      const gum::ScheduleMultiDim<float> result = comb.combine(set, schedule);

      TS_ASSERT(comb.nbOperations(set, schedule) == 112);

      const gum::NodeSet &available = schedule.availableOperations();

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                 available.beginSafe(); // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      gum::MultiDimCombinationDefault<float, gum::MultiDimImplementation> comb11(
          schedule_comb_myadd);
      gum::MultiDimImplementation<float> *res11 = comb11.combine(set);

      TS_ASSERT(*res11 == result.multiDim());

      gum::ScheduleDeleteMultiDim<float> del4(result);
      del4.execute();
      delete res11;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_construct3() {
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

      gum::Potential<float> pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
      randomInit(pot2);

      gum::Potential<float> pot3;
      pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
      randomInit(pot3);

      gum::Potential<float> pot4;
      pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      randomInit(pot4);

      gum::ScheduleCombinationBasic<float> comb(schedule_comb_myadd);
      gum::Set<const gum::Potential<float> *> set;
      set << &pot1 << &pot2 << &pot3 << &pot4;
      gum::Schedule<float> schedule;
      const gum::ScheduleMultiDim<float> result = comb.combine(set, schedule);

      TS_ASSERT(comb.nbOperations(set, schedule) == 112);

      const gum::NodeSet &available = schedule.availableOperations();

      while (!available.empty()) {
        for (gum::NodeSet::const_iterator_safe iter =
                 available.beginSafe(); // safe iterator needed here
             iter != available.endSafe();
             ++iter) {
          schedule.execute(*iter);
        }
      }

      gum::MultiDimCombinationDefault<float, gum::MultiDimImplementation> comb11(
          schedule_comb_myadd);
      gum::Set<const gum::MultiDimImplementation<float> *> set2;
      set2 << pot1.content() << pot2.content() << pot3.content() << pot4.content();
      gum::MultiDimImplementation<float> *res11 = comb11.combine(set2);

      TS_ASSERT(*res11 == result.multiDim());

      gum::ScheduleDeleteMultiDim<float> del4(result);
      del4.execute();
      delete res11;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_construct4() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential<float> t1, t2, t3;
      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
      t2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
      t3 << *(vars[6]) << *(vars[4]) << *(vars[3]);

      randomInit(t1);
      randomInit(t2);
      randomInit(t3);

      gum::ScheduleCombinationBasic<float> comb(schedule_comb_myadd);
      gum::Set<const gum::Potential<float> *> set;
      set << &t1 << &t2 << &t3;
      gum::Set<const gum::MultiDimImplementation<float> *> set2;
      set2 << t1.content() << t2.content() << t3.content();

      gum::Schedule<float> schedule;

      TS_ASSERT(comb.nbOperations(set, schedule) == 16640);
      std::pair<long, long> yyy = comb.memoryUsage(set, schedule);
      TS_ASSERT(yyy.first == 16640);
      TS_ASSERT(yyy.second == 16384);
      std::pair<long, long> zzz = comb.memoryUsage(set2, schedule);
      TS_ASSERT(zzz.first == 16640);
      TS_ASSERT(zzz.second == 16384);

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
        t.set(i, (int)(((float)rand() / RAND_MAX) * 100));
    }
  };

} /* namespace gum_tests */
