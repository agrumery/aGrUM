/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/set.h>
#include <agrum/base/graphicalModels/inference/scheduler/scheduleProjection.h>
#include <agrum/base/graphicalModels/inference/scheduler/scheduleStorage.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>

#include <agrum/base/core/utils_random.h>

namespace gum_tests {

  class [[maybe_unused]] ScheduleStorageTestSuite: public CxxTest::TestSuite {
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


      TS_ASSERT(store1.implyDeletion())
      TS_ASSERT(store1bis.implyDeletion())
      TS_ASSERT(store2.implyDeletion())
      TS_ASSERT(store2bis.implyDeletion())

      TS_ASSERT_DIFFERS(store1, store1bis)
      TS_ASSERT(store1.isSameOperator(store1bis))
      TS_ASSERT(!(store1 == store1bis))
      TS_ASSERT(store1.hasSimilarArguments(store1bis))
      TS_ASSERT(store1.hasSameArguments(store1bis))

      TS_ASSERT_DIFFERS(store2, store2bis)
      TS_ASSERT(store2.isSameOperator(store2bis))
      TS_ASSERT(!(store2 == store2bis))
      TS_ASSERT(store2.hasSimilarArguments(store2bis))
      TS_ASSERT(!store2.hasSameArguments(store2bis))

      const gum::ScheduleOperator& store1const = store1;
      TS_ASSERT(store1const.implyDeletion())
      TS_ASSERT(store1 == store1const)
      TS_ASSERT(!(store1 != store1const))
      TS_ASSERT(store1.isSameOperator(store1const))
      TS_ASSERT(store1.hasSimilarArguments(store1const))
      TS_ASSERT(store1.hasSameArguments(store1const))

      const gum::Sequence< const gum::IScheduleMultiDim* >& res1 = store1.results();
      TS_ASSERT(res1.empty())
      TS_ASSERT(store1const.implyDeletion())
      const gum::Sequence< const gum::IScheduleMultiDim* >& xres1 = store1const.results();
      TS_ASSERT(xres1.empty())

      const auto& arg1 = store1.arg();
      TS_ASSERT_EQUALS(&arg1, &f1)
      TS_ASSERT_EQUALS(arg1, f1)

      const auto& args1 = store1.args();
      TS_ASSERT_EQUALS(args1.size(), gum::Size(1))
      TS_ASSERT(arg1 == *args1[0])

      store1.execute();
      TS_ASSERT(f1.isAbstract())
      TS_ASSERT(store1.arg().isAbstract())
      TS_ASSERT_EQUALS(v1.size(), 1u)
      TS_ASSERT_EQUALS(pot1, v1[0])
      TS_ASSERT_THROWS(store1.undo(), const gum::OperationNotAllowed&)

      store1bis.execute();
      TS_ASSERT(f1b.isAbstract())
      TS_ASSERT_EQUALS(v1.size(), 2u)
      TS_ASSERT_EQUALS(v1[0], v1[1])

      store2.execute();
      TS_ASSERT(store2.isExecuted())
      TS_ASSERT(f2.isAbstract())
      TS_ASSERT(store2.arg().isAbstract())
      TS_ASSERT_EQUALS(set2.size(), gum::Size(1))
      TS_ASSERT_EQUALS(**(set2.begin()), pot2)
      TS_ASSERT_THROWS(store2bis.execute(), const gum::NullElement&)

      store2ter.execute();
      TS_ASSERT(store2ter.isExecuted())
      TS_ASSERT(f2b.isAbstract())
      TS_ASSERT(store2ter.arg().isAbstract())
      TS_ASSERT_EQUALS(set2b.size(), gum::Size(1))
      TS_ASSERT_EQUALS(**(set2.begin()), **(set2b.begin()))

      TS_ASSERT(store2.nbOperations() == 1.0)

      const auto [xfirst, xsecond] = store2.memoryUsage();
      TS_ASSERT_EQUALS(xfirst, -4.0 * sizeof(double) - sizeof(gum::Tensor< double >*))
      TS_ASSERT_EQUALS(xsecond, -4.0 * sizeof(double) - sizeof(gum::Tensor< double >*))

      const std::pair< double, double > xxx1 = store1.memoryUsage();
      TS_ASSERT_EQUALS(xxx1.first, -16.0 * sizeof(double) - sizeof(gum::Tensor< double >))
      TS_ASSERT_EQUALS(xxx1.second, -16.0 * sizeof(double) - sizeof(gum::Tensor< double >))

      gum::ScheduleMultiDim< gum::Tensor< double > >             f1c(pot1, true);
      gum::ScheduleStorage< gum::Tensor< double >, std::vector > store4(f1c, v1);
      gum::ScheduleStorage< gum::Tensor< double >, std::vector > store4b(store4);
      TS_ASSERT(store4b.implyDeletion())
      TS_ASSERT(!store4b.arg().isAbstract())

      gum::ScheduleStorage< gum::Tensor< double >, std::vector > store5(std::move(store4));
      TS_ASSERT(store5.implyDeletion())
      TS_ASSERT(!store5.arg().isAbstract())
      store5.execute();
      TS_ASSERT(store5.arg().isAbstract())

      gum::ScheduleMultiDim< gum::Tensor< double > > f3(pot2, true);
      TS_ASSERT(store1.isExecuted())
      store1.updateArgs({&f3});
      TS_ASSERT(!store1.arg().isAbstract())
      TS_ASSERT(!store1.isExecuted())
      TS_ASSERT(store1.implyDeletion())

      gum::ScheduleMultiDim< gum::Tensor< double > > f4(pot2, true);
      store2.updateArgs({&f4});
      gum::ScheduleOperator& xstore2 = store2;
      TS_ASSERT(store1 != xstore2)

      store2bis.updateArgs({&f4});
      gum::ScheduleStorage< gum::Tensor< double >*, gum::Set > store3bis = store2bis;
      store3bis                                                             = store2ter;
      TS_ASSERT(store3bis.implyDeletion())
      TS_ASSERT_EQUALS(store3bis, store2ter)
      TS_ASSERT(store3bis != store2bis)

      gum::ScheduleStorage< gum::Tensor< double >*, gum::Set > store6 = store2bis;
      store3bis                                                          = std::move(store2bis);
      TS_ASSERT_EQUALS(store6, store3bis)

      gum::ScheduleStorage< gum::Tensor< double >*, gum::Set >* store7 = store6.clone();
      TS_ASSERT(store7->implyDeletion())
      TS_ASSERT_EQUALS(store6, *store7)
      delete store7;

      std::stringstream s;
      s << "store ( " << f1b.toString() << " )";
      TS_ASSERT_EQUALS(s.str(), store1bis.toString())

      gum::VariableSet del_vars1;
      del_vars1 << vars[0] << vars[3];
      gum::ScheduleProjection< gum::Tensor< double > > myproj(f1b, del_vars1, myProjectMax);
      TS_ASSERT(!store1bis.isSameOperator(myproj))
      TS_ASSERT(!store1bis.hasSimilarArguments(myproj))

      for (const auto ptr: set2)
        delete ptr;
      for (const auto ptr: set2b)
        delete ptr;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    private:
    static gum::Tensor< double > myProjectMax(const gum::Tensor< double >& pot,
                                                 const gum::VariableSet&         del_vars) {
      return gum::Tensor< double >(gum::projectMax(*(pot.content()), del_vars));
    }
  };

} /* namespace gum_tests */
