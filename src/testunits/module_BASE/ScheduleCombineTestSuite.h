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
#include <agrum/graphicalModels/inference/scheduleCombine.h>

namespace gum_tests {

  static gum::MultiDimImplementation<float> *
  myadd(const gum::MultiDimImplementation<float> &f1,
        const gum::MultiDimImplementation<float> &f2) {
    return f1 + f2;
  }

  class ScheduleCombineTestSuite : public CxxTest::TestSuite {
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

      gum::ScheduleCombine<float> comb1(f1, f2, myadd);
      const gum::ScheduleMultiDim<float> &result1 = comb1.result();

      TS_ASSERT(comb1.nbOperations() == 32);
      std::pair<long, long> xxx = comb1.memoryUsage();
      TS_ASSERT(xxx.first == 32);

      std::stringstream s1;
      s1 << result1.toString() << " = combine ( " << f1.toString() << " , "
         << f2.toString() << " )";
      TS_ASSERT(s1.str() == comb1.toString());

      gum::ScheduleCombine<float> comb2(result1, f3, myadd);
      const gum::ScheduleMultiDim<float> &result2 = comb2.result();

      TS_ASSERT(result1.isAbstract());
      comb1.execute();
      TS_ASSERT(!result1.isAbstract());
      comb2.execute();
      TS_ASSERT(!result2.isAbstract());

      gum::Potential<float> pot4(pot1 + pot2);
      gum::Potential<float> pot5(pot4 + pot3);

      TS_ASSERT(result1.multiDim() == *pot4.content());
      TS_ASSERT(result2.multiDim() == *pot5.content());

      gum::Sequence<const gum::ScheduleMultiDim<float> *> seq = comb2.multiDimArgs();
      gum::SequenceIteratorSafe<const gum::ScheduleMultiDim<float> *> iter =
          seq.begin();
      TS_ASSERT(**iter == result1);
      ++iter;
      TS_ASSERT(**iter == f3);

      gum::ScheduleCombine<float> comb3(comb2);
      TS_ASSERT(comb3 == comb2);
      TS_ASSERT(comb3 != comb1);

      comb3 = comb1;
      TS_ASSERT(comb3 == comb1);
      TS_ASSERT(comb3 != comb2);

      delete &(comb2.result().multiDim());

      comb3.execute();
      TS_ASSERT(comb3.result().multiDim() == *pot4.content());
      delete &(comb3.result().multiDim());

      gum::ScheduleCombine<float> *comb4 = comb3.newFactory();
      TS_ASSERT(*comb4 == comb3);
      delete comb4;

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
