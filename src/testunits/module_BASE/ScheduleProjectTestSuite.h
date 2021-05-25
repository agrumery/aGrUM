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

#include <agrum/tools/graphicalModels/inference/scheduler/scheduleProject.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  class ScheduleProjectTestSuite: public CxxTest::TestSuite {
    public:
    void test_construct() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Potential< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[3]) << *(vars[4]);
      randomInit(pot1);
      gum::ScheduleMultiDim< double >          f1(pot1);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars << vars[0] << vars[3];

      gum::ScheduleProject< double >         myproj(f1, del_vars, gum::projectMax);
      const gum::ScheduleMultiDim< double >& res = myproj.result();
      TS_ASSERT(res.isAbstract());

      TS_ASSERT(myproj.nbOperations() == 16);
      std::pair< long, long > xxx = myproj.memoryUsage();
      TS_ASSERT(xxx.first == 4);

      gum::Sequence< const gum::ScheduleMultiDim< double >* > multidims = myproj.multiDimArgs();
      TS_ASSERT(multidims.size() == 1);
      TS_ASSERT(*(multidims.atPos(0)) == f1);

      std::stringstream s1;
      s1 << res.toString() << " = project ( " << f1.toString() << " , " << del_vars.toString()
         << " )";
      TS_ASSERT(s1.str() == myproj.toString());

      gum::ScheduleProject< double > myproj2 = myproj;
      TS_ASSERT(myproj2.result().isAbstract());
      TS_ASSERT(myproj2 == myproj);
      TS_ASSERT(!(myproj2 != myproj));

      myproj.execute();
      TS_ASSERT(!res.isAbstract());
      TS_ASSERT(!myproj2.result().isAbstract());
      gum::Potential< double >* res2 = proj(pot1, del_vars, 0);
      TS_ASSERT(*(res2->content()) == res.multiDim());

      gum::ScheduleProject< double > myproj3(f1, del_vars, gum::projectMin);
      TS_ASSERT(myproj3 != myproj);
      const gum::ScheduleMultiDim< double >& res3 = myproj3.result();
      TS_ASSERT(res3.isAbstract());
      myproj3 = myproj;
      myproj3.execute();
      TS_ASSERT(res3.multiDim() == res.multiDim());

      gum::ScheduleProject< double >* myproj4 = myproj3.newFactory();
      TS_ASSERT(*myproj4 == myproj3);
      delete myproj4;

      delete res2;
      delete &(res.multiDim());

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

    // projection of a table over a set
    gum::Potential< double >* proj(const gum::Potential< double >&                 table,
                                   const gum::Set< const gum::DiscreteVariable* >& del_vars,
                                   double                                          neutral_elt) {
      gum::Potential< double >*                            result = new gum::Potential< double >;
      const gum::Sequence< const gum::DiscreteVariable* >& vars   = table.variablesSequence();
      result->beginMultipleChanges();

      for (const auto var: vars)
        if (!del_vars.exists(var)) *result << *var;

      result->endMultipleChanges();
      result->fill(neutral_elt);

      gum::Instantiation inst(table);

      for (inst.setFirst(); !inst.end(); ++inst) {
        double xxx = result->get(inst);
        double yyy = table[inst];
        result->set(inst, xxx > yyy ? xxx : yyy);
      }

      return result;
    }
  };

} /* namespace gum_tests */
