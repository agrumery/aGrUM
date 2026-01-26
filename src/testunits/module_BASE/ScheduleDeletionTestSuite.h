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

#include <agrum/base/graphicalModels/inference/scheduler/scheduleDeletion.h>
#include <agrum/base/graphicalModels/inference/scheduler/scheduleProjection.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>

#include <agrum/base/core/utils_random.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  ScheduleDeletion
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct ScheduleDeletionTestSuite {
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
      gum::Tensor< double > pot2;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[3]) << *(vars[4]);
      pot2 << *(vars[0]) << *(vars[2]);
      pot1.random();
      pot2.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f1(pot1, false);
      gum::ScheduleMultiDim< gum::Tensor< double > > f1b(pot1, true);
      gum::ScheduleMultiDim< gum::Tensor< double > > f2(pot2, true);

      gum::ScheduleDeletion< gum::Tensor< double > > del1(f1);
      gum::ScheduleDeletion< gum::Tensor< double > > del1b(f1b);
      gum::ScheduleDeletion< gum::Tensor< double > > del2(f2);

      CHECK(del1.implyDeletion());
      CHECK(del1b.implyDeletion());
      CHECK(del2.implyDeletion());

      CHECK((del1) != (del1b));
      CHECK(del1.isSameOperator(del1b));
      CHECK((del1) != (del1b));
      CHECK(del1.hasSimilarArguments(del1b));
      CHECK(del1.hasSameArguments(del1b));

      const gum::ScheduleOperator& del1const = del1;
      CHECK(del1const.implyDeletion());
      CHECK(del1 == del1const);
      CHECK(!(del1 != del1const));
      CHECK(del1.isSameOperator(del1const));
      CHECK(del1.hasSimilarArguments(del1const));
      CHECK(del1.hasSameArguments(del1const));

      const gum::Sequence< const gum::IScheduleMultiDim* >& res1 = del1.results();
      CHECK(res1.empty());
      const gum::ScheduleDeletion< gum::Tensor< double > > xdel1const(del1);
      CHECK(xdel1const.implyDeletion());
      const gum::Sequence< const gum::IScheduleMultiDim* >& xres1 = xdel1const.results();
      CHECK(xres1.empty());

      const auto& arg1 = del1.arg();
      CHECK((&arg1) == (&f1));
      CHECK((arg1) == (f1));

      const auto& args1 = del1.args();
      CHECK((args1.size()) == (gum::Size(1)));
      CHECK(arg1 == *args1[0]);

      del1b.execute();
      CHECK(!f1.isAbstract());
      CHECK(del1b.arg().isAbstract());
      CHECK(del1b.implyDeletion());

      del1.execute();
      CHECK(del1.isExecuted());
      CHECK(f1.isAbstract());
      CHECK(arg1.isAbstract());

      CHECK_THROWS_AS(del1.undo(), const gum::OperationNotAllowed&);

      CHECK((del2.nbOperations()) == (1.0));
      const auto [xfirst, xsecond] = del2.memoryUsage();
      CHECK((xfirst) == (-4.0 * sizeof(double) - sizeof(gum::Tensor< double >)));
      CHECK((xsecond) == (-4.0 * sizeof(double) - sizeof(gum::Tensor< double >)));

      const auto [xxfirst, xxsecond] = del1.memoryUsage();
      CHECK((xxfirst) == (-16.0 * sizeof(double) - sizeof(gum::Tensor< double >)));
      CHECK((xxsecond) == (-16.0 * sizeof(double) - sizeof(gum::Tensor< double >)));

      gum::ScheduleDeletion< gum::Tensor< double > > del4(del2);
      CHECK(del4.implyDeletion());
      CHECK(!del4.arg().isAbstract());

      gum::ScheduleDeletion< gum::Tensor< double > > del5(std::move(del4));
      CHECK(del5.implyDeletion());
      CHECK(!del5.arg().isAbstract());

      del5.execute();

      CHECK(del5.arg().isAbstract());

      gum::ScheduleMultiDim< gum::Tensor< double > > f3(pot2, true);
      CHECK(del1.isExecuted());

      del1.updateArgs({&f3});

      CHECK(!del1.arg().isAbstract());
      CHECK(!del1.isExecuted());
      CHECK(del1.implyDeletion());

      gum::ScheduleMultiDim< gum::Tensor< double > > f4(pot2, true);
      del2.updateArgs({&f4});
      CHECK((del2) != (del1));
      CHECK(del1.hasSameArguments(del2));

      del1 = del2;
      CHECK(del1.implyDeletion());
      CHECK(del2.implyDeletion());
      CHECK((del2) == (del1));
      CHECK((del1) != (del1b));
      gum::ScheduleDeletion< gum::Tensor< double > > del6 = del1b;
      CHECK(del6.implyDeletion());
      del1 = std::move(del6);
      CHECK((del1) == (del1b));

      auto del7 = del1.clone();
      CHECK(del7->implyDeletion());
      CHECK((del1) == (*del7));
      CHECK((*del7) != (del2));
      delete del7;

      std::stringstream s;
      s << "delete ( " << f1b.toString() << " )";
      CHECK((s.str()) == (del1.toString()));

      gum::VariableSet del_vars1;
      del_vars1 << vars[0] << vars[3];
      gum::ScheduleProjection myproj(f1b, del_vars1, myProjectMax);
      CHECK(!del2.isSameOperator(myproj));
      CHECK(!del2.hasSimilarArguments(myproj));

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }   // namespace gum_tests

    private:
    static gum::Tensor< double > myProjectMax(const gum::Tensor< double >& pot,
                                              const gum::VariableSet&      del_vars) {
      return gum::Tensor< double >(gum::projectMax(*(pot.content()), del_vars));
    }
  };

  GUM_TEST_ACTIF(_construct)

} /* namespace gum_tests */
