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

#include <agrum/base/graphicalModels/inference/scheduler/scheduleBinaryCombination.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  ScheduleBinaryCombination
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct ScheduleBinaryCombinationTestSuite {
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
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
      pot1.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f1(pot1, true);

      gum::Tensor< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
      pot2.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f2(pot2, true);

      gum::Tensor< double > pot3;
      pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
      pot3.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f3(pot3, true);

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
          comb1(f1, f2, myadd);
      CHECK(!comb1.implyDeletion());

      const gum::ScheduleMultiDim< gum::Tensor< double > >& result1 = comb1.result();
      CHECK(result1.isAbstract());

      const auto&                                           comb1const   = comb1;
      const gum::ScheduleMultiDim< gum::Tensor< double > >& result1const = comb1const.result();
      CHECK(result1const.isAbstract());
      CHECK(!comb1const.implyDeletion());

      CHECK(comb1.nbOperations() == 32.0);
      std::pair< double, double > xxx = comb1.memoryUsage();
      CHECK(xxx.first == 32.0 * sizeof(double) + sizeof(gum::Tensor< double >));
      CHECK(xxx.second == 32.0 * sizeof(double) + sizeof(gum::Tensor< double >));

      std::stringstream s1;
      s1 << result1.toString() << " = combine ( " << f1.toString() << " , " << f2.toString()
         << " )";
      CHECK(s1.str() == comb1.toString());

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
                                                            comb2(result1, f3, myadd);
      const gum::ScheduleMultiDim< gum::Tensor< double > >& result2 = comb2.result();

      CHECK(!comb2.implyDeletion());
      CHECK(result1.isAbstract());
      CHECK(!comb1.isExecuted());
      comb1.execute();
      CHECK(comb1.isExecuted());
      CHECK(!result1.isAbstract());
      comb2.execute();
      CHECK(!result2.isAbstract());

      gum::Tensor< double > pot4(pot1 + pot2);
      gum::Tensor< double > pot5(pot4 + pot3);

      CHECK(result1.multiDim() == pot4);
      CHECK(result2.multiDim() == pot5);

      CHECK(comb2.arg1() == result1);
      CHECK(comb2.arg2() == f3);

      const auto& args = comb2.args();
      CHECK(args.size() == gum::Size(2));
      CHECK(result1 == *(args[0]));
      CHECK(f3 == *(args[1]));

      const auto& results = comb2.results();
      CHECK(results.size() == gum::Size(1));
      CHECK(result2 == *(results[0]));

      const auto&                                          comb2const   = comb2;
      const gum::Sequence< const gum::IScheduleMultiDim* > resultsconst = comb2const.results();
      CHECK(!comb2const.implyDeletion());
      CHECK(resultsconst.size() == gum::Size(1));
      CHECK(result2.hasSameVariables(*(resultsconst[0])));
      CHECK(comb2const.hasSameArguments(comb2));
      CHECK(comb2const.hasSimilarArguments(comb2));
      CHECK(comb2const.isSameOperator(comb2));
      CHECK(comb2const == comb2);
      CHECK(!(comb2const != comb2));

      const gum::ScheduleOperator& comb2op = comb2;
      CHECK(!comb2op.implyDeletion());
      CHECK(comb2const == comb2op);
      CHECK(!(comb2const != comb2op));
      CHECK(comb2const.hasSameArguments(comb2op));
      CHECK(comb2const.isSameOperator(comb2op));

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
          comb3(comb2);
      CHECK(!comb3.implyDeletion());
      CHECK(comb3 == comb2);
      CHECK(comb3 != comb1);
      CHECK(&(comb3.result()) != &(comb2.result()));
      CHECK(comb3.result().hasSameVariables(comb2.result()));
      CHECK(comb3.result().hasSameContent(comb2.result()));
      CHECK(comb3.isExecuted());

      comb3.undo();
      CHECK(comb3.result() == comb2.result());
      CHECK(!comb3.result().hasSameContent(comb2.result()));
      CHECK(comb3.result().isAbstract());
      CHECK(!comb3.isExecuted());
      comb3.execute();
      CHECK(comb3.result().hasSameContent(comb2.result()));
      CHECK(&(comb3.result()) != &(comb2.result()));

      comb3.undo();
      comb3 = comb1;
      CHECK(!comb3.implyDeletion());
      CHECK(comb3 == comb1);
      CHECK(comb3 != comb2);
      CHECK(comb3.result().hasSameContent(result1));
      CHECK(comb3.isExecuted());

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
          comb3b = comb1;
      CHECK(!comb3b.implyDeletion());
      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
          comb3c = comb2;
      CHECK(!comb3c.implyDeletion());
      CHECK(comb3b == comb1);
      CHECK(comb3b.isExecuted());
      CHECK(comb3b.result().hasSameContent(result1));

      comb3b = std::move(comb3c);
      CHECK(!comb3b.implyDeletion());
      CHECK(comb3b == comb2);
      CHECK(comb3b != comb1);
      comb3b.undo();
      comb2.undo();
      CHECK(!comb3b.implyDeletion());

      comb3.execute();
      CHECK(comb3.result().multiDim() == pot4);

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
          comb4(std::move(comb3));
      CHECK(!comb4.implyDeletion());
      CHECK((comb4) == (comb1));
      CHECK((comb4) != (comb2));
      CHECK((comb4.result().multiDim()) == (pot4));

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >* comb5
          = comb4.clone();
      CHECK(!comb5->implyDeletion());
      CHECK(*comb5 == comb4);
      CHECK(comb5->result().multiDim() == pot4);
      comb5->undo();
      delete comb5;

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >* comb6
          = comb4.clone();
      CHECK(!comb6->implyDeletion());
      CHECK((*comb6) == (comb4));
      CHECK((comb6->result().multiDim()) == (pot4));
      delete comb6;

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
          comb7 = comb1;

      const gum::Sequence< const gum::IScheduleMultiDim* > new_args32{&f3, &f2};
      comb1.updateArgs(new_args32);
      CHECK(!comb1.isExecuted());
      comb1.execute();
      gum::Tensor< double > pot32(pot3 + pot2);
      CHECK(comb1.result().multiDim() == pot32);

      CHECK(!comb1.hasSameArguments(comb7));
      CHECK(!comb1.hasSimilarArguments(comb7));
      CHECK(!comb1.hasSimilarArguments(dynamic_cast< gum::ScheduleOperator& >(comb7)));
      CHECK(comb1.isSameOperator(comb7));
      CHECK(comb1.isSameOperator(dynamic_cast< gum::ScheduleOperator& >(comb7)));
      CHECK((comb1) != (comb7));

      comb1.setCombinationFunction(mymult);
      CHECK(!comb1.isExecuted());
      comb1.execute();
      gum::Tensor< double > mult32(pot3 * pot2);
      CHECK((comb1.result().multiDim()) == (mult32));
      CHECK(!comb1.implyDeletion());
      CHECK(!comb1.isSameOperator(comb7));


      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }   // namespace gum_tests

    static void testConstants() {
      gum::IScheduleMultiDim::resetIdGenerator();

      gum::Tensor< double > p1;
      p1.fillWith({3.0});
      gum::Tensor< double > p2;
      p2.fillWith({5.5});

      gum::ScheduleMultiDim< gum::Tensor< double > > f1(p1, false);
      gum::ScheduleMultiDim< gum::Tensor< double > > f2(p2, false);
      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
                                                            comb1(f1, f2, myadd);
      const gum::ScheduleMultiDim< gum::Tensor< double > >& result1 = comb1.result();
      CHECK(result1.isAbstract());

      comb1.execute();
      CHECK(comb1.isExecuted());
      CHECK(!result1.isAbstract());
      CHECK((result1.multiDim().max()) == (8.5));
      CHECK((result1.multiDim().min()) == (8.5));
      CHECK((result1.domainSize()) == (1u));
      CHECK((result1.multiDim().domainSize()) == (1u));


      std::vector< gum::LabelizedVariable* > vars(10);
      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Tensor< double > p3;
      p3 << *(vars[0]);
      p3.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f3(p3, false);

      gum::Tensor< double > p4;
      p4 << *(vars[1]) << *(vars[2]);
      p4.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f4(p4, true);

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
          comb2(f1, f3, myadd);
      comb2.execute();
      const gum::Tensor< double >& result2 = comb2.result().multiDim();
      CHECK((result2.domainSize()) == (2u));
      CHECK((comb2.result().domainSize()) == (2u));

      gum::Instantiation i2(result2);
      for (i2.setFirst(); !i2.end(); ++i2) {
        CHECK((result2[i2]) == (p3[i2] + 3.0));
      }

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
          comb3(f1, f4, myadd);
      comb3.execute();
      const gum::Tensor< double >& result3 = comb3.result().multiDim();
      CHECK((result3.domainSize()) == (4u));
      CHECK((comb3.result().domainSize()) == (4u));

      gum::Instantiation i3(result3);
      for (i3.setFirst(); !i3.end(); ++i3) {
        CHECK(result3[i3] == p4[i3] + 3.0);
      }

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    private:
    static gum::Tensor< double > myadd(const gum::Tensor< double >& f1,
                                       const gum::Tensor< double >& f2) {
      return f1 + f2;
    }

    static gum::Tensor< double > mymult(const gum::Tensor< double >& f1,
                                        const gum::Tensor< double >& f2) {
      return f1 * f2;
    }
  };

  GUM_TEST_ACTIF(_construct)
  GUM_TEST_ACTIF(Constants)

} /* namespace gum_tests */
