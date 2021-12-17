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

#include <agrum/tools/graphicalModels/inference/scheduler/scheduleProjection.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  class ScheduleProjectionTestSuite: public CxxTest::TestSuite {
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

      gum::Potential< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[3]) << *(vars[4]);
      randomInit(pot1);
      gum::ScheduleMultiDim< gum::Potential< double > > f1(pot1, true);
      gum::Set< const gum::DiscreteVariable* >          del_vars1;
      del_vars1 << vars[0] << vars[3];

      gum::Potential< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]);
      randomInit(pot2);
      gum::ScheduleMultiDim< gum::Potential< double > > f2(pot2, true);
      gum::Set< const gum::DiscreteVariable* >          del_vars2;
      del_vars2 << vars[0] << vars[3] << vars[2];

      gum::ScheduleProjection< gum::Potential< double > > myproj(f1, del_vars1, myProjectMax);
      TS_ASSERT(!myproj.implyDeletion());

      const gum::ScheduleMultiDim< gum::Potential< double > >& res = myproj.result();
      TS_ASSERT(res.isAbstract());

      const gum::ScheduleProjection< gum::Potential< double > >& myprojconst(myproj);
      TS_ASSERT(!myprojconst.implyDeletion());
      const gum::ScheduleMultiDim< gum::Potential< double > >& resconst = myprojconst.result();
      TS_ASSERT(resconst.isAbstract());

      TS_ASSERT(myproj.nbOperations() == 16.0);
      std::pair< double, double > xxx = myproj.memoryUsage();
      TS_ASSERT(xxx.first == 4.0 * sizeof(double) + sizeof(gum::Potential< double >));
      TS_ASSERT(xxx.second == 4.0 * sizeof(double) + sizeof(gum::Potential< double >));

      TS_ASSERT(myproj.arg() == f1);

      std::stringstream s1;
      s1 << res.toString() << " = project ( " << f1.toString() << " , " << del_vars1.toString()
         << " )";
      TS_ASSERT(s1.str() == myproj.toString());

      gum::ScheduleProjection< gum::Potential< double > > myproj2(myproj);
      TS_ASSERT(!myproj2.implyDeletion());
      TS_ASSERT(myproj2.result().isAbstract());
      TS_ASSERT(!myproj2.isExecuted());
      TS_ASSERT(myproj2 == myproj);
      TS_ASSERT(!(myproj2 != myproj));

      myproj.execute();
      TS_ASSERT(!myproj.implyDeletion());
      TS_ASSERT(!res.isAbstract());
      TS_ASSERT(myproj2.result().isAbstract());
      myproj2.execute();
      TS_ASSERT(!myproj2.result().isAbstract());
      TS_ASSERT(myproj2.isExecuted());
      myproj2.undo();
      TS_ASSERT(myproj2.result().isAbstract());
      TS_ASSERT(!myproj2.isExecuted());

      gum::Potential< double > res2 = proj(pot1, del_vars1, 0);
      myproj2.execute();
      const gum::ScheduleMultiDim< gum::Potential< double > >& xres2 = myproj2.result();
      TS_ASSERT(xres2.multiDim() == res.multiDim());
      TS_ASSERT(xres2.multiDim() == res2);

      gum::ScheduleProjection< gum::Potential< double > > myproj3(f2, del_vars2, myProjectMin);
      TS_ASSERT(!myproj3.implyDeletion());
      TS_ASSERT(myproj3 != myproj);
      const gum::ScheduleMultiDim< gum::Potential< double > >& xxres3 = myproj3.result();
      TS_ASSERT(xxres3.isAbstract());
      myproj3.execute();
      TS_ASSERT(!xxres3.isAbstract());
      gum::Potential< double > res3
         = gum::Potential< double >(gum::projectMin(*(pot2.content()), del_vars2));
      TS_ASSERT(xxres3.multiDim() == res3);

      myproj3 = myproj;
      TS_ASSERT(!myproj3.implyDeletion());
      TS_ASSERT(myproj3 == myproj);
      myproj3.execute();
      const gum::ScheduleMultiDim< gum::Potential< double > >& xres3 = myproj3.result();
      TS_ASSERT(xres3.multiDim() == res.multiDim());

      const gum::Sequence< const gum::IScheduleMultiDim* >& results = myproj3.results();
      TS_ASSERT(results.size() == gum::Size(1));
      TS_ASSERT(res.hasSameContent(*(results[0])));
      TS_ASSERT(res.hasSameVariables(*(results[0])));

      gum::ScheduleProjection< gum::Potential< double > > myproj4x(f1, del_vars1, myProjectMax);
      TS_ASSERT(!myproj4x.implyDeletion());

      gum::ScheduleProjection< gum::Potential< double > > myproj4(std::move(myproj4x));
      TS_ASSERT(!myproj4.implyDeletion());
      TS_ASSERT(myproj4.result().isAbstract());

      myproj4.execute();
      TS_ASSERT(myproj4.result().multiDim() == res2);
      gum::ScheduleProjection< gum::Potential< double > > myproj4b(std::move(myproj4));
      TS_ASSERT(!myproj4b.implyDeletion());
      TS_ASSERT(myproj4b.result().multiDim() == res2);

      myproj3.setProjectionFunction(myProjectMin);
      gum::Potential< double > res3b
         = gum::Potential< double >(gum::projectMin(*(pot1.content()), del_vars1));
      TS_ASSERT(!myproj3.isExecuted());
      myproj3.execute();
      TS_ASSERT(myproj3.result().multiDim() == res3b);

      TS_ASSERT(myproj4b != myproj3);
      TS_ASSERT(myproj4b == myproj);
      gum::ScheduleProjection< gum::Potential< double > > myproj3x(myproj3);
      myproj4b = std::move(myproj3x);
      TS_ASSERT(!myproj4b.implyDeletion());
      TS_ASSERT(myproj4b != myproj);
      myproj4b.execute();
      TS_ASSERT(myproj4b.result().multiDim() == res3b);

      myproj3 = myproj;
      TS_ASSERT(!myproj3.implyDeletion());
      TS_ASSERT(myproj3.result().multiDim() == res.multiDim());

      gum::ScheduleProjection< gum::Potential< double > > myproj5(f1, del_vars1, myProjectMin);
      TS_ASSERT(!myproj5.implyDeletion());
      gum::ScheduleProjection< gum::Potential< double > >* myproj6 = myproj5.clone();
      TS_ASSERT(!myproj6->implyDeletion());
      TS_ASSERT(*myproj6 == myproj5);
      myproj6->execute();
      TS_ASSERT(myproj4b.result().multiDim() == res3b);

      const gum::Sequence< const gum::IScheduleMultiDim* >& args = myproj6->args();
      TS_ASSERT(args.size() == gum::Size(1));
      TS_ASSERT(f1 == *(args[0]));
      delete myproj6;

      myproj5.execute();
      myproj5.execute();

      const gum::Sequence< const gum::IScheduleMultiDim* >& xresults = myproj5.results();
      TS_ASSERT(xresults.size() == gum::Size(1));
      TS_ASSERT(myproj5.result() == *(xresults[0]));

      const auto&                                           myproj5const  = myproj5;
      const gum::Sequence< const gum::IScheduleMultiDim* >& xresultsconst = myproj5const.results();
      TS_ASSERT(!myproj5const.implyDeletion());
      TS_ASSERT(xresultsconst.size() == gum::Size(1));
      TS_ASSERT(myproj5.result() == *(xresultsconst[0]));
      TS_ASSERT(myproj5const.hasSameArguments(myproj5));
      TS_ASSERT(myproj5const.hasSimilarArguments(myproj5));
      TS_ASSERT(myproj5const.hasSameArguments(dynamic_cast< gum::ScheduleOperation& >(myproj5)));
      TS_ASSERT(myproj5const.hasSimilarArguments(dynamic_cast< gum::ScheduleOperation& >(myproj5)));
      TS_ASSERT(myproj5const == myproj5);
      TS_ASSERT(!(myproj5const != myproj5));

      const gum::ScheduleOperation& proj5op = myproj5;
      TS_ASSERT(!proj5op.implyDeletion());
      TS_ASSERT(myproj5 == proj5op);
      TS_ASSERT(!(myproj5 != proj5op));
      TS_ASSERT(myproj5.isSameOperation(proj5op));
      TS_ASSERT(myproj5.hasSameArguments(proj5op));
      TS_ASSERT(myproj5.hasSimilarArguments(proj5op));

      gum::ScheduleProjection< gum::Potential< double > > myproj7(f1, del_vars1, myProjectMax);
      TS_ASSERT(!myproj5.isSameOperation(myproj7));
      TS_ASSERT(myproj5.hasSameArguments(myproj7));
      TS_ASSERT(myproj5.hasSimilarArguments(myproj7));

      gum::ScheduleProjection< gum::Potential< double > > myproj8(f2, del_vars2, myProjectMax);
      TS_ASSERT(myproj8.isSameOperation(myproj7));
      TS_ASSERT(!myproj8.hasSameArguments(myproj7));
      TS_ASSERT(!myproj8.hasSimilarArguments(myproj7));

      gum::ScheduleProjection< gum::Potential< double > > myproj9(f2, del_vars1, myProjectMax);
      TS_ASSERT(myproj9.isSameOperation(myproj7));
      TS_ASSERT(!myproj9.hasSameArguments(myproj7));
      TS_ASSERT(!myproj9.hasSimilarArguments(myproj7));


      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void testConstants() {
      gum::IScheduleMultiDim::resetIdGenerator();

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
      gum::ScheduleMultiDim< gum::Potential< double > > f1(pot1, false);

      gum::Set< const gum::DiscreteVariable* >            del_vars1;
      gum::ScheduleProjection< gum::Potential< double > > myproj1(f1, del_vars1, myProjectMax);
      TS_ASSERT(myproj1.result().domainSize() == 16)
      TS_ASSERT(myproj1.nbOperations() == 16.0)
      auto mem_usage
         = std::pair< double, double >(16.0 * sizeof(double) + sizeof(gum::Potential< double >),
                                       16.0 * sizeof(double) + sizeof(gum::Potential< double >));
      TS_ASSERT(myproj1.memoryUsage() == mem_usage)
      TS_ASSERT(myproj1.result().isAbstract())
      myproj1.execute();
      TS_ASSERT(myproj1.result().multiDim() == pot1);
      TS_ASSERT(&myproj1.result().multiDim() != &pot1);

      del_vars1 << vars[0] << vars[2] << vars[3] << vars[4];
      gum::ScheduleProjection< gum::Potential< double > > myproj1b(f1, del_vars1, myProjectMax);
      TS_ASSERT(myproj1b.result().domainSize() == 1)
      TS_ASSERT(myproj1b.nbOperations() == 16.0)
      mem_usage
         = std::pair< double, double >(1.0 * sizeof(double) + sizeof(gum::Potential< double >),
                                       1.0 * sizeof(double) + sizeof(gum::Potential< double >));
      TS_ASSERT(myproj1b.memoryUsage() == mem_usage)
      TS_ASSERT(myproj1b.result().isAbstract())
      myproj1b.execute();
      TS_ASSERT(!myproj1b.result().isAbstract())
      TS_ASSERT(myproj1b.result().multiDim().domainSize() == 1);
      gum::Potential< double > pot1b
         = const_cast< gum::ScheduleMultiDim< gum::Potential< double > >& >(myproj1b.result())
              .exportMultiDim();
      TS_ASSERT(pot1b.domainSize() == 1);

      del_vars1.clear();
      del_vars1 << vars[0];
      gum::ScheduleProjection< gum::Potential< double > > myproj2(f1, del_vars1, myProjectMax);
      TS_ASSERT(myproj2.result().domainSize() == 8)
      myproj2.execute();
      gum::Potential< double > pot2 = myProjectMax(pot1, del_vars1);
      TS_ASSERT(pot2 == myproj2.result().multiDim())
      gum::Sequence< const gum::IScheduleMultiDim* >    seq;
      gum::ScheduleMultiDim< gum::Potential< double > > xpot2(pot2, false);
      seq << &xpot2;
      myproj2.updateArgs(seq);
      TS_ASSERT(myproj2.result().domainSize() == 8)
      TS_ASSERT(myproj2.nbOperations() == 8.0)
      mem_usage
         = std::pair< double, double >(8.0 * sizeof(double) + sizeof(gum::Potential< double >),
                                       8.0 * sizeof(double) + sizeof(gum::Potential< double >));
      TS_ASSERT(myproj2.memoryUsage() == mem_usage)
      TS_ASSERT(myproj2.result().isAbstract())
      myproj2.execute();
      TS_ASSERT(myproj2.result().multiDim() == pot2);

      gum::Potential< double > pot3;
      pot3 << *(vars[2]) << *(vars[3]) << *(vars[4]);
      randomInit(pot3);
      gum::ScheduleMultiDim< gum::Potential< double > > f3(pot3, false);
      seq.clear();
      seq << &f3;
      myproj2.updateArgs(seq);
      myproj2.execute();
      TS_ASSERT(myproj2.result().multiDim() == pot3)

      gum::Potential< double > pot4;
      pot4.fillWith({3.0});
      gum::ScheduleMultiDim< gum::Potential< double > > f4(pot4, false);
      seq.clear();
      seq << &f4;
      myproj2.updateArgs(seq);
      TS_ASSERT(!myproj2.result().isAbstract())
      myproj2.execute();
      TS_ASSERT(!myproj2.result().isAbstract())
      TS_ASSERT(myproj2.result().multiDim() == pot4)
      TS_ASSERT(myproj2.result().domainSize() == 1)
      mem_usage
         = std::pair< double, double >(1.0 * sizeof(double) + sizeof(gum::Potential< double >),
                                       1.0 * sizeof(double) + sizeof(gum::Potential< double >));
      TS_ASSERT(myproj2.nbOperations() == 1.0)
      TS_ASSERT(myproj2.memoryUsage() == mem_usage)
      myproj2.execute();
      TS_ASSERT(!myproj2.result().isAbstract())
      TS_ASSERT(myproj2.result().multiDim() == pot4)
      TS_ASSERT(myproj2.result().domainSize() == 1)
      TS_ASSERT(myproj2.nbOperations() == 1.0)
      TS_ASSERT(myproj2.memoryUsage() == mem_usage)

      gum::ScheduleProjection< gum::Potential< double > > myproj3(f4, del_vars1, myProjectMax);
      TS_ASSERT(!myproj3.result().isAbstract())
      TS_ASSERT(myproj3.result().multiDim() == pot4)
      TS_ASSERT(myproj3.result().domainSize() == 1)
      TS_ASSERT(myproj3.nbOperations() == 1.0)
      TS_ASSERT(myproj3.memoryUsage() == mem_usage)
      myproj3.execute();
      TS_ASSERT(!myproj3.result().isAbstract())
      TS_ASSERT(myproj3.result().multiDim() == pot4)
      TS_ASSERT(myproj3.result().domainSize() == 1)
      TS_ASSERT(myproj3.nbOperations() == 1.0)
      TS_ASSERT(myproj3.memoryUsage() == mem_usage)

      del_vars1.clear();
      gum::ScheduleProjection< gum::Potential< double > > myproj4(f4, del_vars1, myProjectMax);
      TS_ASSERT(!myproj4.result().isAbstract())
      TS_ASSERT(myproj4.result().multiDim() == pot4)
      TS_ASSERT(myproj4.result().domainSize() == 1)
      TS_ASSERT(myproj4.nbOperations() == 1.0)
      TS_ASSERT(myproj4.memoryUsage() == mem_usage)
      myproj3.execute();
      TS_ASSERT(!myproj4.result().isAbstract())
      TS_ASSERT(myproj4.result().multiDim() == pot4)
      TS_ASSERT(myproj4.result().domainSize() == 1)
      TS_ASSERT(myproj4.nbOperations() == 1.0)
      TS_ASSERT(myproj4.memoryUsage() == mem_usage)

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    private:
    static gum::Potential< double >
       myProjectMax(const gum::Potential< double >&                 pot,
                    const gum::Set< const gum::DiscreteVariable* >& del_vars) {
      return gum::Potential< double >(gum::projectMax(*(pot.content()), del_vars));
    }

    static gum::Potential< double >
       myProjectMin(const gum::Potential< double >&                 pot,
                    const gum::Set< const gum::DiscreteVariable* >& del_vars) {
      return gum::Potential< double >(gum::projectMin(*(pot.content()), del_vars));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInit(gum::Potential< double >& t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, rand() * 100000.0f / RAND_MAX);
    }

    // projection of a table over a set
    gum::Potential< double > proj(const gum::Potential< double >&                 table,
                                  const gum::Set< const gum::DiscreteVariable* >& del_vars,
                                  float                                           neutral_elt) {
      gum::Potential< double >                             result;
      const gum::Sequence< const gum::DiscreteVariable* >& vars = table.variablesSequence();
      result.beginMultipleChanges();

      for (const auto var: vars)
        if (!del_vars.exists(var)) result << *var;

      result.endMultipleChanges();
      result.fill(neutral_elt);

      gum::Instantiation inst(table);

      for (inst.setFirst(); !inst.end(); ++inst) {
        float xxx = result.get(inst);
        float yyy = table[inst];
        result.set(inst, xxx > yyy ? xxx : yyy);
      }

      return result;
    }
  };

} /* namespace gum_tests */
