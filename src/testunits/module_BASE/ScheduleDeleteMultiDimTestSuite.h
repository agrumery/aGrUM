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

#include <agrum/core/utils_random.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/potential.h>
#include <agrum/graphicalModels/inference/scheduleProject.h>
#include <agrum/graphicalModels/inference/scheduleDeleteMultiDim.h>

namespace gum_tests {

  class ScheduleDeleteMultiDimTestSuite : public CxxTest::TestSuite {
    public:
    void test_construct() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Potential<float> pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[3]) << *(vars[4]);
      randomInit(pot1);
      gum::ScheduleMultiDim<float> f1(pot1);
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars << vars[0] << vars[3];
      gum::ScheduleProject<float> myproj(f1, del_vars, gum::projectMax);
      const gum::ScheduleMultiDim<float> &res = myproj.result();

      gum::ScheduleDeleteMultiDim<float> del1(f1);
      gum::ScheduleDeleteMultiDim<float> del2(res);
      gum::ScheduleDeleteMultiDim<float> del3(del2);
      TS_ASSERT(!(del1 == del2));
      TS_ASSERT(del1 != del2);
      TS_ASSERT_THROWS(del2.execute(), gum::NotFound);
      myproj.execute();
      TS_GUM_ASSERT_THROWS_NOTHING(del2.execute());
      TS_ASSERT_THROWS(del3.execute(), gum::NotFound);

      TS_ASSERT(del1.nbOperations() == 1);
      std::pair<long, long> xxx = del1.memoryUsage();
      TS_ASSERT(xxx.first == -16);
      TS_ASSERT(xxx.second == -16);

      gum::Sequence<const gum::ScheduleMultiDim<float> *> seq = del2.multiDimArgs();
      TS_ASSERT(seq.size() == 1);
      TS_ASSERT(*(seq.atPos(0)) == res);

      del3 = del1;
      TS_ASSERT(del1 == del3);
      TS_ASSERT(del3 != del2);

      std::stringstream s;
      s << "delete ( " << f1.toString() << " )";
      TS_ASSERT(s.str() == del1.toString());

      gum::ScheduleDeleteMultiDim<float> *del4 = del3.newFactory();
      TS_ASSERT(*del4 == del3);
      delete del4;

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
        t.set(i, (int)(gum::randomProba() * 100000));
    }
  };

} /* namespace gum_tests */
