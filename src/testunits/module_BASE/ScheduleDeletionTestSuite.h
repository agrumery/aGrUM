/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphicalModels/inference/scheduler/scheduleDeletion.h>
#include <agrum/base/graphicalModels/inference/scheduler/scheduleProjection.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>

#include <agrum/base/core/utils_random.h>

namespace gum_tests {

  class [[maybe_unused]] ScheduleDeletionTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_construct) {
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

      TS_ASSERT(del1.implyDeletion())
      TS_ASSERT(del1b.implyDeletion())
      TS_ASSERT(del2.implyDeletion())

      TS_ASSERT_DIFFERS(del1, del1b)
      TS_ASSERT(del1.isSameOperator(del1b))
      TS_ASSERT_DIFFERS(del1, del1b)
      TS_ASSERT(del1.hasSimilarArguments(del1b))
      TS_ASSERT(del1.hasSameArguments(del1b))

      const gum::ScheduleOperator& del1const = del1;
      TS_ASSERT(del1const.implyDeletion())
      TS_ASSERT(del1 == del1const)
      TS_ASSERT(!(del1 != del1const))
      TS_ASSERT(del1.isSameOperator(del1const))
      TS_ASSERT(del1.hasSimilarArguments(del1const))
      TS_ASSERT(del1.hasSameArguments(del1const))

      const gum::Sequence< const gum::IScheduleMultiDim* >& res1 = del1.results();
      TS_ASSERT(res1.empty());
      const gum::ScheduleDeletion< gum::Tensor< double > > xdel1const(del1);
      TS_ASSERT(xdel1const.implyDeletion());
      const gum::Sequence< const gum::IScheduleMultiDim* >& xres1 = xdel1const.results();
      TS_ASSERT(xres1.empty())

      const auto& arg1 = del1.arg();
      TS_ASSERT_EQUALS(&arg1, &f1)
      TS_ASSERT_EQUALS(arg1, f1)

      const auto& args1 = del1.args();
      TS_ASSERT_EQUALS(args1.size(), gum::Size(1))
      TS_ASSERT(arg1 == *args1[0])

      del1b.execute();
      TS_ASSERT(!f1.isAbstract())
      TS_ASSERT(del1b.arg().isAbstract())
      TS_ASSERT(del1b.implyDeletion())

      del1.execute();
      TS_ASSERT(del1.isExecuted())
      TS_ASSERT(f1.isAbstract())
      TS_ASSERT(arg1.isAbstract())

      TS_ASSERT_THROWS(del1.undo(), const gum::OperationNotAllowed&);

      TS_ASSERT_EQUALS(del2.nbOperations(), 1.0)
      const auto [xfirst, xsecond] = del2.memoryUsage();
      TS_ASSERT_EQUALS(xfirst, -4.0 * sizeof(double) - sizeof(gum::Tensor< double >))
      TS_ASSERT_EQUALS(xsecond, -4.0 * sizeof(double) - sizeof(gum::Tensor< double >))

      const auto [xxfirst, xxsecond] = del1.memoryUsage();
      TS_ASSERT_EQUALS(xxfirst, -16.0 * sizeof(double) - sizeof(gum::Tensor< double >))
      TS_ASSERT_EQUALS(xxsecond, -16.0 * sizeof(double) - sizeof(gum::Tensor< double >))

      gum::ScheduleDeletion< gum::Tensor< double > > del4(del2);
      TS_ASSERT(del4.implyDeletion())
      TS_ASSERT(!del4.arg().isAbstract())

      gum::ScheduleDeletion< gum::Tensor< double > > del5(std::move(del4));
      TS_ASSERT(del5.implyDeletion())
      TS_ASSERT(!del5.arg().isAbstract())

      del5.execute();

      TS_ASSERT(del5.arg().isAbstract())

      gum::ScheduleMultiDim< gum::Tensor< double > > f3(pot2, true);
      TS_ASSERT(del1.isExecuted())

      del1.updateArgs({&f3});

      TS_ASSERT(!del1.arg().isAbstract())
      TS_ASSERT(!del1.isExecuted())
      TS_ASSERT(del1.implyDeletion())

      gum::ScheduleMultiDim< gum::Tensor< double > > f4(pot2, true);
      del2.updateArgs({&f4});
      TS_ASSERT_DIFFERS(del2, del1)
      TS_ASSERT(del1.hasSameArguments(del2))

      del1 = del2;
      TS_ASSERT(del1.implyDeletion())
      TS_ASSERT(del2.implyDeletion())
      TS_ASSERT_EQUALS(del2, del1)
      TS_ASSERT_DIFFERS(del1, del1b)
      gum::ScheduleDeletion< gum::Tensor< double > > del6 = del1b;
      TS_ASSERT(del6.implyDeletion())
      del1 = std::move(del6);
      TS_ASSERT_EQUALS(del1, del1b)

      auto del7 = del1.clone();
      TS_ASSERT(del7->implyDeletion())
      TS_ASSERT_EQUALS(del1, *del7)
      TS_ASSERT_DIFFERS(*del7, del2)
      delete del7;

      std::stringstream s;
      s << "delete ( " << f1b.toString() << " )";
      TS_ASSERT_EQUALS(s.str(), del1.toString())

      gum::VariableSet del_vars1;
      del_vars1 << vars[0] << vars[3];
      gum::ScheduleProjection myproj(f1b, del_vars1, myProjectMax);
      TS_ASSERT(!del2.isSameOperator(myproj));
      TS_ASSERT(!del2.hasSimilarArguments(myproj));

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    private:
    static gum::Tensor< double > myProjectMax(const gum::Tensor< double >& pot,
                                              const gum::VariableSet&      del_vars) {
      return gum::Tensor< double >(gum::projectMax(*(pot.content()), del_vars));
    }
  };

} /* namespace gum_tests */
