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

#include <agrum/base/core/set.h>
#include <agrum/base/graphicalModels/inference/scheduler/scheduleProjection.h>
#include <agrum/base/graphicalModels/inference/scheduler/scheduleStorage.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>

#include <agrum/base/core/utils_random.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  ScheduleStorage
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct ScheduleStorageTestSuite {
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

      gum::Tensor< double > pot1, pot2;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[3]) << *(vars[4]);
      pot2 << *(vars[0]) << *(vars[2]);
      pot1.random();
      pot2.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f1(pot1, true);
      gum::ScheduleMultiDim< gum::Tensor< double > > f1b(pot1, false);
      gum::ScheduleMultiDim< gum::Tensor< double > > f2(pot2, true);
      gum::ScheduleMultiDim< gum::Tensor< double > > f2b(pot2, false);

      std::vector< gum::Tensor< double > >                       v1;
      gum::ScheduleStorage< gum::Tensor< double >, std::vector > store1(f1, v1);
      gum::ScheduleStorage< gum::Tensor< double >, std::vector > store1bis(f1b, v1);

      gum::Set< gum::Tensor< double >* >                       set2;
      gum::ScheduleStorage< gum::Tensor< double >*, gum::Set > store2(f2, set2);
      gum::Set< gum::Tensor< double >* >                       set2b;
      gum::ScheduleStorage< gum::Tensor< double >*, gum::Set > store2bis(f2, set2b);
      gum::ScheduleStorage< gum::Tensor< double >*, gum::Set > store2ter(f2b, set2b);


      CHECK(store1.implyDeletion());
      CHECK(store1bis.implyDeletion());
      CHECK(store2.implyDeletion());
      CHECK(store2bis.implyDeletion());

      CHECK((store1) != (store1bis));
      CHECK(store1.isSameOperator(store1bis));
      CHECK(!(store1 == store1bis));
      CHECK(store1.hasSimilarArguments(store1bis));
      CHECK(store1.hasSameArguments(store1bis));

      CHECK((store2) != (store2bis));
      CHECK(store2.isSameOperator(store2bis));
      CHECK(!(store2 == store2bis));
      CHECK(store2.hasSimilarArguments(store2bis));
      CHECK(!store2.hasSameArguments(store2bis));

      const gum::ScheduleOperator& store1const = store1;
      CHECK(store1const.implyDeletion());
      CHECK(store1 == store1const);
      CHECK(!(store1 != store1const));
      CHECK(store1.isSameOperator(store1const));
      CHECK(store1.hasSimilarArguments(store1const));
      CHECK(store1.hasSameArguments(store1const));

      const gum::Sequence< const gum::IScheduleMultiDim* >& res1 = store1.results();
      CHECK(res1.empty());
      CHECK(store1const.implyDeletion());
      const gum::Sequence< const gum::IScheduleMultiDim* >& xres1 = store1const.results();
      CHECK(xres1.empty());

      const auto& arg1 = store1.arg();
      CHECK((&arg1) == (&f1));
      CHECK((arg1) == (f1));

      const auto& args1 = store1.args();
      CHECK((args1.size()) == (gum::Size(1)));
      CHECK(arg1 == *args1[0]);

      store1.execute();
      CHECK(f1.isAbstract());
      CHECK(store1.arg().isAbstract());
      CHECK((v1.size()) == (1u));
      CHECK((pot1) == (v1[0]));
      CHECK_THROWS_AS(store1.undo(), const gum::OperationNotAllowed&);

      store1bis.execute();
      CHECK(f1b.isAbstract());
      CHECK((v1.size()) == (2u));
      CHECK((v1[0]) == (v1[1]));

      store2.execute();
      CHECK(store2.isExecuted());
      CHECK(f2.isAbstract());
      CHECK(store2.arg().isAbstract());
      CHECK((set2.size()) == (gum::Size(1)));
      CHECK((**(set2.begin())) == (pot2));
      CHECK_THROWS_AS(store2bis.execute(), const gum::NullElement&);

      store2ter.execute();
      CHECK(store2ter.isExecuted());
      CHECK(f2b.isAbstract());
      CHECK(store2ter.arg().isAbstract());
      CHECK((set2b.size()) == (gum::Size(1)));
      CHECK((**(set2.begin())) == (**(set2b.begin())));

      CHECK(store2.nbOperations() == 1.0);

      const auto [xfirst, xsecond] = store2.memoryUsage();
      CHECK((xfirst) == (-4.0 * sizeof(double) - sizeof(gum::Tensor< double >*)));
      CHECK((xsecond) == (-4.0 * sizeof(double) - sizeof(gum::Tensor< double >*)));

      const std::pair< double, double > xxx1 = store1.memoryUsage();
      CHECK((xxx1.first) == (-16.0 * sizeof(double) - sizeof(gum::Tensor< double >)));
      CHECK((xxx1.second) == (-16.0 * sizeof(double) - sizeof(gum::Tensor< double >)));

      gum::ScheduleMultiDim< gum::Tensor< double > >             f1c(pot1, true);
      gum::ScheduleStorage< gum::Tensor< double >, std::vector > store4(f1c, v1);
      gum::ScheduleStorage< gum::Tensor< double >, std::vector > store4b(store4);
      CHECK(store4b.implyDeletion());
      CHECK(!store4b.arg().isAbstract());

      gum::ScheduleStorage< gum::Tensor< double >, std::vector > store5(std::move(store4));
      CHECK(store5.implyDeletion());
      CHECK(!store5.arg().isAbstract());
      store5.execute();
      CHECK(store5.arg().isAbstract());

      gum::ScheduleMultiDim< gum::Tensor< double > > f3(pot2, true);
      CHECK(store1.isExecuted());
      store1.updateArgs({&f3});
      CHECK(!store1.arg().isAbstract());
      CHECK(!store1.isExecuted());
      CHECK(store1.implyDeletion());

      gum::ScheduleMultiDim< gum::Tensor< double > > f4(pot2, true);
      store2.updateArgs({&f4});
      gum::ScheduleOperator& xstore2 = store2;
      CHECK(store1 != xstore2);

      store2bis.updateArgs({&f4});
      gum::ScheduleStorage< gum::Tensor< double >*, gum::Set > store3bis = store2bis;
      store3bis                                                          = store2ter;
      CHECK(store3bis.implyDeletion());
      CHECK((store3bis) == (store2ter));
      CHECK(store3bis != store2bis);

      gum::ScheduleStorage< gum::Tensor< double >*, gum::Set > store6 = store2bis;
      store3bis                                                       = std::move(store2bis);
      CHECK((store6) == (store3bis));

      gum::ScheduleStorage< gum::Tensor< double >*, gum::Set >* store7 = store6.clone();
      CHECK(store7->implyDeletion());
      CHECK((store6) == (*store7));
      delete store7;

      std::stringstream s;
      s << "store ( " << f1b.toString() << " )";
      CHECK((s.str()) == (store1bis.toString()));

      gum::VariableSet del_vars1;
      del_vars1 << vars[0] << vars[3];
      gum::ScheduleProjection< gum::Tensor< double > > myproj(f1b, del_vars1, myProjectMax);
      CHECK(!store1bis.isSameOperator(myproj));
      CHECK(!store1bis.hasSimilarArguments(myproj));

      for (const auto ptr: set2)
        delete ptr;
      for (const auto ptr: set2b)
        delete ptr;

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
