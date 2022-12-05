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

#include <agrum/tools/core/set.h>
#include <agrum/tools/core/utils_random.h>
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleStorage.h>
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleProjection.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  class [[maybe_unused]] ScheduleStorageTestSuite: public CxxTest::TestSuite {
    public:
    GUM_TEST(_construct) {
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
      gum::ScheduleMultiDim< gum::Potential< double > > f1(pot1, true);
      gum::ScheduleMultiDim< gum::Potential< double > > f1b(pot1, false);
      gum::ScheduleMultiDim< gum::Potential< double > > f2(pot2, true);
      gum::ScheduleMultiDim< gum::Potential< double > > f2b(pot2, false);

      std::vector< gum::Potential< double > >                       v1;
      gum::ScheduleStorage< gum::Potential< double >, std::vector > store1(f1, v1);
      gum::ScheduleStorage< gum::Potential< double >, std::vector > store1bis(f1b, v1);

      gum::Set< gum::Potential< double >* >                       set2;
      gum::ScheduleStorage< gum::Potential< double >*, gum::Set > store2(f2, set2);
      gum::Set< gum::Potential< double >* >                       set2b;
      gum::ScheduleStorage< gum::Potential< double >*, gum::Set > store2bis(f2, set2b);
      gum::ScheduleStorage< gum::Potential< double >*, gum::Set > store2ter(f2b, set2b);


      TS_ASSERT(store1.implyDeletion());
      TS_ASSERT(store1bis.implyDeletion());
      TS_ASSERT(store2.implyDeletion());
      TS_ASSERT(store2bis.implyDeletion());

      TS_ASSERT(store1 != store1bis);
      TS_ASSERT(store1.isSameOperator(store1bis));
      TS_ASSERT(!(store1 == store1bis));
      TS_ASSERT(store1.hasSimilarArguments(store1bis));
      TS_ASSERT(store1.hasSameArguments(store1bis));

      TS_ASSERT(store2 != store2bis);
      TS_ASSERT(store2.isSameOperator(store2bis));
      TS_ASSERT(!(store2 == store2bis));
      TS_ASSERT(store2.hasSimilarArguments(store2bis));
      TS_ASSERT(!store2.hasSameArguments(store2bis));

      const gum::ScheduleOperator& store1const = store1;
      TS_ASSERT(store1const.implyDeletion());
      TS_ASSERT(store1 == store1const);
      TS_ASSERT(!(store1 != store1const));
      TS_ASSERT(store1.isSameOperator(store1const));
      TS_ASSERT(store1.hasSimilarArguments(store1const));
      TS_ASSERT(store1.hasSameArguments(store1const));

      const gum::Sequence< const gum::IScheduleMultiDim* >& res1 = store1.results();
      TS_ASSERT(res1.empty());
      TS_ASSERT(store1const.implyDeletion());
      const gum::Sequence< const gum::IScheduleMultiDim* >& xres1 = store1const.results();
      TS_ASSERT(xres1.empty());

      const auto& arg1 = store1.arg();
      TS_ASSERT(&arg1 == &f1);
      TS_ASSERT(arg1 == f1);

      const auto& args1 = store1.args();
      TS_ASSERT(args1.size() == gum::Size(1));
      TS_ASSERT(arg1 == *args1[0]);

      store1.execute();
      TS_ASSERT(f1.isAbstract());
      TS_ASSERT(store1.arg().isAbstract());
      TS_ASSERT(v1.size() == std::size_t(1));
      TS_ASSERT(pot1 == v1[0]);
      TS_ASSERT_THROWS(store1.undo(), const gum::OperationNotAllowed&);

      store1bis.execute();
      TS_ASSERT(f1b.isAbstract());
      TS_ASSERT(v1.size() == std::size_t(2));
      TS_ASSERT(v1[0] == v1[1]);

      store2.execute();
      TS_ASSERT(store2.isExecuted());
      TS_ASSERT(f2.isAbstract());
      TS_ASSERT(store2.arg().isAbstract());
      TS_ASSERT(set2.size() == gum::Size(1));
      TS_ASSERT(**(set2.begin()) == pot2);
      TS_ASSERT_THROWS(store2bis.execute(), const gum::NullElement&);

      store2ter.execute();
      TS_ASSERT(store2ter.isExecuted());
      TS_ASSERT(f2b.isAbstract());
      TS_ASSERT(store2ter.arg().isAbstract());
      TS_ASSERT(set2b.size() == gum::Size(1));
      TS_ASSERT(**(set2.begin()) == **(set2b.begin()));

      TS_ASSERT(store2.nbOperations() == 1.0);
      const std::pair< double, double > xxx2 = store2.memoryUsage();
      TS_ASSERT(xxx2.first == -4.0 * sizeof(double) - sizeof(gum::Potential< double >*));
      TS_ASSERT(xxx2.second == -4.0 * sizeof(double) - sizeof(gum::Potential< double >*));

      const std::pair< double, double > xxx1 = store1.memoryUsage();
      TS_ASSERT(xxx1.first == -16.0 * sizeof(double) - sizeof(gum::Potential< double >));
      TS_ASSERT(xxx1.second == -16.0 * sizeof(double) - sizeof(gum::Potential< double >));

      gum::ScheduleMultiDim< gum::Potential< double > >             f1c(pot1, true);
      gum::ScheduleStorage< gum::Potential< double >, std::vector > store4(f1c, v1);
      gum::ScheduleStorage< gum::Potential< double >, std::vector > store4b(store4);
      TS_ASSERT(store4b.implyDeletion());
      TS_ASSERT(!store4b.arg().isAbstract());

      gum::ScheduleStorage< gum::Potential< double >, std::vector > store5(std::move(store4));
      TS_ASSERT(store5.implyDeletion());
      TS_ASSERT(!store5.arg().isAbstract());
      store5.execute();
      TS_ASSERT(store5.arg().isAbstract());

      gum::ScheduleMultiDim< gum::Potential< double > > f3(pot2, true);
      TS_ASSERT(store1.isExecuted());
      store1.updateArgs({&f3});
      TS_ASSERT(!store1.arg().isAbstract());
      TS_ASSERT(!store1.isExecuted());
      TS_ASSERT(store1.implyDeletion());

      gum::ScheduleMultiDim< gum::Potential< double > > f4(pot2, true);
      store2.updateArgs({&f4});
      gum::ScheduleOperator& xstore2 = store2;
      TS_ASSERT(store1 != xstore2);

      store2bis.updateArgs({&f4});
      gum::ScheduleStorage< gum::Potential< double >*, gum::Set > store3bis = store2bis;
      store3bis                                                             = store2ter;
      TS_ASSERT(store3bis.implyDeletion());
      TS_ASSERT(store3bis == store2ter);
      TS_ASSERT(store3bis != store2bis);

      gum::ScheduleStorage< gum::Potential< double >*, gum::Set > store6 = store2bis;
      store3bis                                                          = std::move(store2bis);
      TS_ASSERT(store6 == store3bis);

      gum::ScheduleStorage< gum::Potential< double >*, gum::Set >* store7 = store6.clone();
      TS_ASSERT(store7->implyDeletion());
      TS_ASSERT(store6 == *store7);
      delete store7;

      std::stringstream s;
      s << "store ( " << f1b.toString() << " )";
      TS_ASSERT(s.str() == store1bis.toString());

      gum::Set< const gum::DiscreteVariable* > del_vars1;
      del_vars1 << vars[0] << vars[3];
      gum::ScheduleProjection< gum::Potential< double > > myproj(f1b, del_vars1, myProjectMax);
      TS_ASSERT(!store1bis.isSameOperator(myproj));
      TS_ASSERT(!store1bis.hasSimilarArguments(myproj));

      for (const auto ptr: set2)
        delete ptr;
      for (const auto ptr: set2b)
        delete ptr;

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
