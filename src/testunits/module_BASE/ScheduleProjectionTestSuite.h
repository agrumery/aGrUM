/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphicalModels/inference/scheduler/scheduleProjection.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  ScheduleProjection
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct ScheduleProjectionTestSuite {
    public:
    static void test_construct() {
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

      gum::Tensor< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[3]) << *(vars[4]);
      pot1.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f1(pot1, true);
      gum::VariableSet                               del_vars1;
      del_vars1 << vars[0] << vars[3];

      gum::Tensor< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]);
      pot2.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f2(pot2, true);
      gum::VariableSet                               del_vars2;
      del_vars2 << vars[0] << vars[3] << vars[2];

      gum::ScheduleProjection< gum::Tensor< double > > myproj(f1, del_vars1, myProjectMax);
      CHECK(!myproj.implyDeletion());

      const auto& res = myproj.result();
      CHECK(res.isAbstract());

      const gum::ScheduleProjection< gum::Tensor< double > >& myprojconst(myproj);
      CHECK(!myprojconst.implyDeletion());
      const gum::ScheduleMultiDim< gum::Tensor< double > >& resconst = myprojconst.result();
      CHECK(resconst.isAbstract());

      CHECK(myproj.nbOperations() == 16.0);
      const auto [xfirst, xsecond] = myproj.memoryUsage();
      CHECK((xfirst) == (4.0 * sizeof(double) + sizeof(gum::Tensor< double >)));
      CHECK((xsecond) == (4.0 * sizeof(double) + sizeof(gum::Tensor< double >)));

      CHECK(myproj.arg() == f1);

      std::stringstream s1;
      s1 << res.toString() << " = project ( " << f1.toString() << " , " << del_vars1.toString()
         << " )";
      CHECK(s1.str() == myproj.toString());

      gum::ScheduleProjection< gum::Tensor< double > > myproj2(myproj);
      CHECK(!myproj2.implyDeletion());
      CHECK(myproj2.result().isAbstract());
      CHECK(!myproj2.isExecuted());
      CHECK((myproj2) == (myproj));
      CHECK(!(myproj2 != myproj));

      myproj.execute();
      CHECK(!myproj.implyDeletion());
      CHECK(!res.isAbstract());
      CHECK(myproj2.result().isAbstract());
      myproj2.execute();
      CHECK(!myproj2.result().isAbstract());
      CHECK(myproj2.isExecuted());
      myproj2.undo();
      CHECK(myproj2.result().isAbstract());
      CHECK(!myproj2.isExecuted());

      gum::Tensor< double > res2 = proj(pot1, del_vars1, 0);
      myproj2.execute();
      const gum::ScheduleMultiDim< gum::Tensor< double > >& xres2 = myproj2.result();
      CHECK(xres2.multiDim() == res.multiDim());
      CHECK(xres2.multiDim() == res2);

      gum::ScheduleProjection< gum::Tensor< double > > myproj3(f2, del_vars2, myProjectMin);
      CHECK(!myproj3.implyDeletion());
      CHECK(myproj3 != myproj);
      const gum::ScheduleMultiDim< gum::Tensor< double > >& xxres3 = myproj3.result();
      CHECK(xxres3.isAbstract());
      myproj3.execute();
      CHECK(!xxres3.isAbstract());
      gum::Tensor< double > res3
          = gum::Tensor< double >(gum::projectMin(*(pot2.content()), del_vars2));
      CHECK(xxres3.multiDim() == res3);

      myproj3 = myproj;
      CHECK(!myproj3.implyDeletion());
      CHECK(myproj3 == myproj);
      myproj3.execute();
      const gum::ScheduleMultiDim< gum::Tensor< double > >& xres3 = myproj3.result();
      CHECK(xres3.multiDim() == res.multiDim());

      const gum::Sequence< const gum::IScheduleMultiDim* >& results = myproj3.results();
      CHECK(results.size() == gum::Size(1));
      CHECK(res.hasSameContent(*(results[0])));
      CHECK(res.hasSameVariables(*(results[0])));

      gum::ScheduleProjection< gum::Tensor< double > > myproj4x(f1, del_vars1, myProjectMax);
      CHECK(!myproj4x.implyDeletion());

      gum::ScheduleProjection< gum::Tensor< double > > myproj4(std::move(myproj4x));
      CHECK(!myproj4.implyDeletion());
      CHECK(myproj4.result().isAbstract());

      myproj4.execute();
      CHECK(myproj4.result().multiDim() == res2);
      gum::ScheduleProjection< gum::Tensor< double > > myproj4b(std::move(myproj4));
      CHECK(!myproj4b.implyDeletion());
      CHECK(myproj4b.result().multiDim() == res2);

      myproj3.setProjectionFunction(myProjectMin);
      gum::Tensor< double > res3b
          = gum::Tensor< double >(gum::projectMin(*(pot1.content()), del_vars1));
      CHECK(!myproj3.isExecuted());
      myproj3.execute();
      CHECK(myproj3.result().multiDim() == res3b);

      CHECK(myproj4b != myproj3);
      CHECK(myproj4b == myproj);
      gum::ScheduleProjection< gum::Tensor< double > > myproj3x(myproj3);
      myproj4b = std::move(myproj3x);
      CHECK(!myproj4b.implyDeletion());
      CHECK(myproj4b != myproj);
      myproj4b.execute();
      CHECK(myproj4b.result().multiDim() == res3b);

      myproj3 = myproj;
      CHECK(!myproj3.implyDeletion());
      CHECK(myproj3.result().multiDim() == res.multiDim());

      gum::ScheduleProjection< gum::Tensor< double > > myproj5(f1, del_vars1, myProjectMin);
      CHECK(!myproj5.implyDeletion());
      gum::ScheduleProjection< gum::Tensor< double > >* myproj6 = myproj5.clone();
      CHECK(!myproj6->implyDeletion());
      CHECK(*myproj6 == myproj5);
      myproj6->execute();
      CHECK(myproj4b.result().multiDim() == res3b);

      const gum::Sequence< const gum::IScheduleMultiDim* >& args = myproj6->args();
      CHECK(args.size() == gum::Size(1));
      CHECK(f1 == *(args[0]));
      delete myproj6;

      myproj5.execute();
      myproj5.execute();

      const gum::Sequence< const gum::IScheduleMultiDim* >& xresults = myproj5.results();
      CHECK(xresults.size() == gum::Size(1));
      CHECK(myproj5.result() == *(xresults[0]));

      const auto&                                           myproj5const  = myproj5;
      const gum::Sequence< const gum::IScheduleMultiDim* >& xresultsconst = myproj5const.results();
      CHECK(!myproj5const.implyDeletion());
      CHECK(xresultsconst.size() == gum::Size(1));
      CHECK(myproj5.result() == *(xresultsconst[0]));
      CHECK(myproj5const.hasSameArguments(myproj5));
      CHECK(myproj5const.hasSimilarArguments(myproj5));
      CHECK(myproj5const.hasSameArguments(dynamic_cast< gum::ScheduleOperator& >(myproj5)));
      CHECK(myproj5const.hasSimilarArguments(dynamic_cast< gum::ScheduleOperator& >(myproj5)));
      CHECK(myproj5const == myproj5);
      CHECK(!(myproj5const != myproj5));

      const gum::ScheduleOperator& proj5op = myproj5;
      CHECK(!proj5op.implyDeletion());
      CHECK(myproj5 == proj5op);
      CHECK(!(myproj5 != proj5op));
      CHECK(myproj5.isSameOperator(proj5op));
      CHECK(myproj5.hasSameArguments(proj5op));
      CHECK(myproj5.hasSimilarArguments(proj5op));

      gum::ScheduleProjection< gum::Tensor< double > > myproj7(f1, del_vars1, myProjectMax);
      CHECK(!myproj5.isSameOperator(myproj7));
      CHECK(myproj5.hasSameArguments(myproj7));
      CHECK(myproj5.hasSimilarArguments(myproj7));

      gum::ScheduleProjection< gum::Tensor< double > > myproj8(f2, del_vars2, myProjectMax);
      CHECK(myproj8.isSameOperator(myproj7));
      CHECK(!myproj8.hasSameArguments(myproj7));
      CHECK(!myproj8.hasSimilarArguments(myproj7));

      gum::ScheduleProjection< gum::Tensor< double > > myproj9(f2, del_vars1, myProjectMax);
      CHECK(myproj9.isSameOperator(myproj7));
      CHECK(!myproj9.hasSameArguments(myproj7));
      CHECK(!myproj9.hasSimilarArguments(myproj7));


      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }   // namespace gum_tests

    static void testConstants() {
      gum::IScheduleMultiDim::resetIdGenerator();

      std::vector< gum::LabelizedVariable* > vars(10);
      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Tensor< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[3]) << *(vars[4]);
      pot1.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f1(pot1, false);

      gum::VariableSet                                 del_vars1;
      gum::ScheduleProjection< gum::Tensor< double > > myproj1(f1, del_vars1, myProjectMax);
      CHECK(myproj1.result().domainSize() == 16);
      CHECK(myproj1.nbOperations() == 16.0);
      auto mem_usage
          = std::pair< double, double >(16.0 * sizeof(double) + sizeof(gum::Tensor< double >),
                                        16.0 * sizeof(double) + sizeof(gum::Tensor< double >));
      CHECK(myproj1.memoryUsage() == mem_usage);
      CHECK(myproj1.result().isAbstract());
      myproj1.execute();
      CHECK(myproj1.result().multiDim() == pot1);
      CHECK(&myproj1.result().multiDim() != &pot1);

      del_vars1 << vars[0] << vars[2] << vars[3] << vars[4];
      gum::ScheduleProjection< gum::Tensor< double > > myproj1b(f1, del_vars1, myProjectMax);
      CHECK(myproj1b.result().domainSize() == 1);
      CHECK(myproj1b.nbOperations() == 16.0);
      mem_usage = std::pair< double, double >(1.0 * sizeof(double) + sizeof(gum::Tensor< double >),
                                              1.0 * sizeof(double) + sizeof(gum::Tensor< double >));
      CHECK(myproj1b.memoryUsage() == mem_usage);
      CHECK(myproj1b.result().isAbstract());
      myproj1b.execute();
      CHECK(!myproj1b.result().isAbstract());
      CHECK(myproj1b.result().multiDim().domainSize() == 1);
      gum::Tensor< double >* pot1b
          = const_cast< gum::ScheduleMultiDim< gum::Tensor< double > >& >(myproj1b.result())
                .exportMultiDim();
      CHECK(pot1b->domainSize() == 1);
      delete pot1b;

      del_vars1.clear();
      del_vars1 << vars[0];
      gum::ScheduleProjection< gum::Tensor< double > > myproj2(f1, del_vars1, myProjectMax);
      CHECK(myproj2.result().domainSize() == 8);
      myproj2.execute();
      gum::Tensor< double > pot2 = myProjectMax(pot1, del_vars1);
      CHECK(pot2 == myproj2.result().multiDim());
      gum::Sequence< const gum::IScheduleMultiDim* > seq;
      gum::ScheduleMultiDim< gum::Tensor< double > > xpot2(pot2, false);
      seq << &xpot2;
      myproj2.updateArgs(seq);
      CHECK(myproj2.result().domainSize() == 8);
      CHECK(myproj2.nbOperations() == 8.0);
      mem_usage = std::pair< double, double >(8.0 * sizeof(double) + sizeof(gum::Tensor< double >),
                                              8.0 * sizeof(double) + sizeof(gum::Tensor< double >));
      CHECK(myproj2.memoryUsage() == mem_usage);
      CHECK(myproj2.result().isAbstract());
      myproj2.execute();
      CHECK(myproj2.result().multiDim() == pot2);

      gum::Tensor< double > pot3;
      pot3 << *(vars[2]) << *(vars[3]) << *(vars[4]);
      pot3.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f3(pot3, false);
      seq.clear();
      seq << &f3;
      myproj2.updateArgs(seq);
      myproj2.execute();
      CHECK(myproj2.result().multiDim() == pot3);

      gum::Tensor< double > pot4;
      pot4.fillWith({3.0});
      gum::ScheduleMultiDim< gum::Tensor< double > > f4(pot4, false);
      seq.clear();
      seq << &f4;
      myproj2.updateArgs(seq);
      CHECK(!myproj2.result().isAbstract());
      myproj2.execute();
      CHECK(!myproj2.result().isAbstract());
      CHECK(myproj2.result().multiDim() == pot4);
      CHECK(myproj2.result().domainSize() == 1);
      mem_usage = std::pair< double, double >(1.0 * sizeof(double) + sizeof(gum::Tensor< double >),
                                              1.0 * sizeof(double) + sizeof(gum::Tensor< double >));
      CHECK(myproj2.nbOperations() == 1.0);
      CHECK(myproj2.memoryUsage() == mem_usage);
      myproj2.execute();
      CHECK(!myproj2.result().isAbstract());
      CHECK(myproj2.result().multiDim() == pot4);
      CHECK(myproj2.result().domainSize() == 1);
      CHECK(myproj2.nbOperations() == 1.0);
      CHECK(myproj2.memoryUsage() == mem_usage);

      gum::ScheduleProjection< gum::Tensor< double > > myproj3(f4, del_vars1, myProjectMax);
      CHECK(!myproj3.result().isAbstract());
      CHECK(myproj3.result().multiDim() == pot4);
      CHECK(myproj3.result().domainSize() == 1);
      CHECK(myproj3.nbOperations() == 1.0);
      CHECK(myproj3.memoryUsage() == mem_usage);
      myproj3.execute();
      CHECK(!myproj3.result().isAbstract());
      CHECK(myproj3.result().multiDim() == pot4);
      CHECK(myproj3.result().domainSize() == 1);
      CHECK(myproj3.nbOperations() == 1.0);
      CHECK(myproj3.memoryUsage() == mem_usage);

      del_vars1.clear();
      gum::ScheduleProjection< gum::Tensor< double > > myproj4(f4, del_vars1, myProjectMax);
      CHECK(!myproj4.result().isAbstract());
      CHECK(myproj4.result().multiDim() == pot4);
      CHECK(myproj4.result().domainSize() == 1);
      CHECK(myproj4.nbOperations() == 1.0);
      CHECK(myproj4.memoryUsage() == mem_usage);
      myproj3.execute();
      CHECK(!myproj4.result().isAbstract());
      CHECK(myproj4.result().multiDim() == pot4);
      CHECK(myproj4.result().domainSize() == 1);
      CHECK(myproj4.nbOperations() == 1.0);
      CHECK(myproj4.memoryUsage() == mem_usage);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    private:
    static gum::Tensor< double > myProjectMax(const gum::Tensor< double >& pot,
                                              const gum::VariableSet&      del_vars) {
      return gum::Tensor< double >(gum::projectMax(*(pot.content()), del_vars));
    }

    static gum::Tensor< double > myProjectMin(const gum::Tensor< double >& pot,
                                              const gum::VariableSet&      del_vars) {
      return gum::Tensor< double >(gum::projectMin(*(pot.content()), del_vars));
    }

    // projection of a table over a set
    static gum::Tensor< double > proj(const gum::Tensor< double >& table,
                                      const gum::VariableSet&      del_vars,
                                      float                        neutral_elt) {
      gum::Tensor< double >                                result;
      const gum::Sequence< const gum::DiscreteVariable* >& vars = table.variablesSequence();
      result.beginMultipleChanges();

      for (const auto var: vars)
        if (!del_vars.exists(var)) result << *var;

      result.endMultipleChanges();
      result.fill(neutral_elt);

      gum::Instantiation inst(table);

      for (inst.setFirst(); !inst.end(); ++inst) {
        auto xxx = result.get(inst);
        auto yyy = table[inst];
        result.set(inst, xxx > yyy ? xxx : yyy);
      }

      return result;
    }
  };

  GUM_TEST_ACTIF(_construct)
  GUM_TEST_ACTIF(Constants)

} /* namespace gum_tests */
