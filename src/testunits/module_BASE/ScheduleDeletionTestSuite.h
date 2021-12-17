/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *  info_at_agrum_dot_org
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

#include <agrum/tools/core/utils_random.h>
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleDeletion.h>
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleProjection.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  class ScheduleDeletionTestSuite: public CxxTest::TestSuite {
    public:
    void test_construct() {
      // reset the ids of the ScheduleMultiDim to avoid conflicts with other
      // testunits
      gum::IScheduleMultiDim::resetIdGenerator();

      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Potential< double > pot1, pot2;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[3]) << *(vars[4]);
      pot2 << *(vars[0]) << *(vars[2]);
      randomInit(pot1);
      randomInit(pot2);
      gum::ScheduleMultiDim< gum::Potential< double > > f1(pot1, false);
      gum::ScheduleMultiDim< gum::Potential< double > > f1b(pot1, true);
      gum::ScheduleMultiDim< gum::Potential< double > > f2(pot2, true);

      gum::ScheduleDeletion< gum::Potential< double > > del1(f1);
      gum::ScheduleDeletion< gum::Potential< double > > del1b(f1b);
      gum::ScheduleDeletion< gum::Potential< double > > del2(f2);

      TS_ASSERT(del1.implyDeletion());
      TS_ASSERT(del1b.implyDeletion());
      TS_ASSERT(del2.implyDeletion());

      TS_ASSERT(del1 != del1b);
      TS_ASSERT(del1.isSameOperation(del1b));
      TS_ASSERT(!(del1 == del1b));
      TS_ASSERT(del1.hasSimilarArguments(del1b));
      TS_ASSERT(del1.hasSameArguments(del1b));

      const gum::ScheduleOperation& del1const = del1;
      TS_ASSERT(del1const.implyDeletion());
      TS_ASSERT(del1 == del1const);
      TS_ASSERT(!(del1 != del1const));
      TS_ASSERT(del1.isSameOperation(del1const));
      TS_ASSERT(del1.hasSimilarArguments(del1const));
      TS_ASSERT(del1.hasSameArguments(del1const));

      const gum::Sequence< const gum::IScheduleMultiDim* >& res1 = del1.results();
      TS_ASSERT(res1.empty());
      const gum::ScheduleDeletion< gum::Potential< double > > xdel1const(del1);
      TS_ASSERT(xdel1const.implyDeletion());
      const gum::Sequence< const gum::IScheduleMultiDim* >& xres1 = xdel1const.results();
      TS_ASSERT(xres1.empty());

      const auto& arg1 = del1.arg();
      TS_ASSERT(&arg1 == &f1);
      TS_ASSERT(arg1 == f1);

      const auto& args1 = del1.args();
      TS_ASSERT(args1.size() == gum::Size(1));
      TS_ASSERT(arg1 == *args1[0]);

      del1b.execute();
      TS_ASSERT(!f1.isAbstract());
      TS_ASSERT(del1b.arg().isAbstract());
      TS_ASSERT(del1b.implyDeletion());

      del1.execute();
      TS_ASSERT(del1.isExecuted());
      TS_ASSERT(f1.isAbstract());
      TS_ASSERT(arg1.isAbstract());

      TS_ASSERT_THROWS(del1.undo(), gum::OperationNotAllowed);

      TS_ASSERT(del2.nbOperations() == 1.0);
      const std::pair< double, double > xxx2 = del2.memoryUsage();
      TS_ASSERT(xxx2.first == -4.0 * sizeof(double) - sizeof(gum::Potential< double >));
      TS_ASSERT(xxx2.second == -4.0 * sizeof(double) - sizeof(gum::Potential< double >));

      const std::pair< double, double > xxx1 = del1.memoryUsage();
      TS_ASSERT(xxx1.first == -16.0 * sizeof(double) - sizeof(gum::Potential< double >));
      TS_ASSERT(xxx1.second == -16.0 * sizeof(double) - sizeof(gum::Potential< double >));

      gum::ScheduleDeletion< gum::Potential< double > > del4(del2);
      TS_ASSERT(del4.implyDeletion());
      TS_ASSERT(!del4.arg().isAbstract());

      gum::ScheduleDeletion< gum::Potential< double > > del5(std::move(del4));
      TS_ASSERT(del5.implyDeletion());
      TS_ASSERT(!del5.arg().isAbstract());
      del5.execute();
      TS_ASSERT(del5.arg().isAbstract());

      gum::ScheduleMultiDim< gum::Potential< double > > f3(pot2, true);
      TS_ASSERT(del1.isExecuted());
      del1.updateArgs({&f3});
      TS_ASSERT(!del1.arg().isAbstract());
      TS_ASSERT(!del1.isExecuted());
      TS_ASSERT(del1.implyDeletion());

      gum::ScheduleMultiDim< gum::Potential< double > > f4(pot2, true);
      del2.updateArgs({&f4});
      TS_ASSERT(del2 != del1);
      TS_ASSERT(del1.hasSameArguments(del2));

      del1 = del2;
      TS_ASSERT(del1.implyDeletion());
      TS_ASSERT(del2.implyDeletion());
      TS_ASSERT(del2 == del1);
      TS_ASSERT(del1 != del1b);
      gum::ScheduleDeletion< gum::Potential< double > > del6 = del1b;
      TS_ASSERT(del6.implyDeletion());
      del1 = std::move(del6);
      TS_ASSERT(del1 == del1b);

      gum::ScheduleDeletion< gum::Potential< double > >* del7 = del1.clone();
      TS_ASSERT(del7->implyDeletion());
      TS_ASSERT(del1 == *del7);
      TS_ASSERT(*del7 != del2);
      delete del7;

      std::stringstream s;
      s << "delete ( " << f1b.toString() << " )";
      TS_ASSERT(s.str() == del1.toString());

      gum::Set< const gum::DiscreteVariable* > del_vars1;
      del_vars1 << vars[0] << vars[3];
      gum::ScheduleProjection< gum::Potential< double > > myproj(f1b, del_vars1, myProjectMax);
      TS_ASSERT(!del2.isSameOperation(myproj));
      TS_ASSERT(!del2.hasSimilarArguments(myproj));

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    private:
    static gum::Potential< double >
       myProjectMax(const gum::Potential< double >&                 pot,
                    const gum::Set< const gum::DiscreteVariable* >& del_vars) {
      return gum::Potential< double >(gum::projectMax(*(pot.content()), del_vars));
    }

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
