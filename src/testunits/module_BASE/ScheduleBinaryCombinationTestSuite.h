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

#include <agrum/base/graphicalModels/inference/scheduler/scheduleBinaryCombination.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>

#include "cxxtest/TestSuite.h"

namespace gum_tests {

  class [[maybe_unused]] ScheduleBinaryCombinationTestSuite: public CxxTest::TestSuite {
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
      TS_ASSERT(!comb1.implyDeletion());

      const gum::ScheduleMultiDim< gum::Tensor< double > >& result1 = comb1.result();
      TS_ASSERT(result1.isAbstract());

      const auto&                                              comb1const   = comb1;
      const gum::ScheduleMultiDim< gum::Tensor< double > >& result1const = comb1const.result();
      TS_ASSERT(result1const.isAbstract());
      TS_ASSERT(!comb1const.implyDeletion());

      TS_ASSERT(comb1.nbOperations() == 32.0);
      std::pair< double, double > xxx = comb1.memoryUsage();
      TS_ASSERT(xxx.first == 32.0 * sizeof(double) + sizeof(gum::Tensor< double >));
      TS_ASSERT(xxx.second == 32.0 * sizeof(double) + sizeof(gum::Tensor< double >));

      std::stringstream s1;
      s1 << result1.toString() << " = combine ( " << f1.toString() << " , " << f2.toString()
         << " )";
      TS_ASSERT(s1.str() == comb1.toString());

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
                                                               comb2(result1, f3, myadd);
      const gum::ScheduleMultiDim< gum::Tensor< double > >& result2 = comb2.result();

      TS_ASSERT(!comb2.implyDeletion());
      TS_ASSERT(result1.isAbstract());
      TS_ASSERT(!comb1.isExecuted());
      comb1.execute();
      TS_ASSERT(comb1.isExecuted());
      TS_ASSERT(!result1.isAbstract());
      comb2.execute();
      TS_ASSERT(!result2.isAbstract());

      gum::Tensor< double > pot4(pot1 + pot2);
      gum::Tensor< double > pot5(pot4 + pot3);

      TS_ASSERT(result1.multiDim() == pot4);
      TS_ASSERT(result2.multiDim() == pot5);

      TS_ASSERT(comb2.arg1() == result1);
      TS_ASSERT(comb2.arg2() == f3);

      const auto& args = comb2.args();
      TS_ASSERT(args.size() == gum::Size(2));
      TS_ASSERT(result1 == *(args[0]));
      TS_ASSERT(f3 == *(args[1]));

      const auto& results = comb2.results();
      TS_ASSERT(results.size() == gum::Size(1));
      TS_ASSERT(result2 == *(results[0]));

      const auto&                                          comb2const   = comb2;
      const gum::Sequence< const gum::IScheduleMultiDim* > resultsconst = comb2const.results();
      TS_ASSERT(!comb2const.implyDeletion());
      TS_ASSERT(resultsconst.size() == gum::Size(1));
      TS_ASSERT(result2.hasSameVariables(*(resultsconst[0])));
      TS_ASSERT(comb2const.hasSameArguments(comb2));
      TS_ASSERT(comb2const.hasSimilarArguments(comb2));
      TS_ASSERT(comb2const.isSameOperator(comb2));
      TS_ASSERT(comb2const == comb2);
      TS_ASSERT(!(comb2const != comb2));

      const gum::ScheduleOperator& comb2op = comb2;
      TS_ASSERT(!comb2op.implyDeletion());
      TS_ASSERT(comb2const == comb2op);
      TS_ASSERT(!(comb2const != comb2op));
      TS_ASSERT(comb2const.hasSameArguments(comb2op));
      TS_ASSERT(comb2const.isSameOperator(comb2op));

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
          comb3(comb2);
      TS_ASSERT(!comb3.implyDeletion());
      TS_ASSERT(comb3 == comb2);
      TS_ASSERT(comb3 != comb1);
      TS_ASSERT(&(comb3.result()) != &(comb2.result()));
      TS_ASSERT(comb3.result().hasSameVariables(comb2.result()));
      TS_ASSERT(comb3.result().hasSameContent(comb2.result()));
      TS_ASSERT(comb3.isExecuted());

      comb3.undo();
      TS_ASSERT(comb3.result() == comb2.result());
      TS_ASSERT(!comb3.result().hasSameContent(comb2.result()));
      TS_ASSERT(comb3.result().isAbstract());
      TS_ASSERT(!comb3.isExecuted());
      comb3.execute();
      TS_ASSERT(comb3.result().hasSameContent(comb2.result()));
      TS_ASSERT(&(comb3.result()) != &(comb2.result()));

      comb3.undo();
      comb3 = comb1;
      TS_ASSERT(!comb3.implyDeletion());
      TS_ASSERT(comb3 == comb1);
      TS_ASSERT(comb3 != comb2);
      TS_ASSERT(comb3.result().hasSameContent(result1));
      TS_ASSERT(comb3.isExecuted());

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
          comb3b = comb1;
      TS_ASSERT(!comb3b.implyDeletion());
      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
          comb3c = comb2;
      TS_ASSERT(!comb3c.implyDeletion());
      TS_ASSERT(comb3b == comb1);
      TS_ASSERT(comb3b.isExecuted());
      TS_ASSERT(comb3b.result().hasSameContent(result1));

      comb3b = std::move(comb3c);
      TS_ASSERT(!comb3b.implyDeletion());
      TS_ASSERT(comb3b == comb2);
      TS_ASSERT(comb3b != comb1);
      comb3b.undo();
      comb2.undo();
      TS_ASSERT(!comb3b.implyDeletion());

      comb3.execute();
      TS_ASSERT(comb3.result().multiDim() == pot4);

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
          comb4(std::move(comb3));
      TS_ASSERT(!comb4.implyDeletion());
      TS_ASSERT_EQUALS(comb4, comb1)
      TS_ASSERT_DIFFERS(comb4, comb2)
      TS_ASSERT_EQUALS(comb4.result().multiDim(), pot4)

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >* comb5
          = comb4.clone();
      TS_ASSERT(!comb5->implyDeletion());
      TS_ASSERT(*comb5 == comb4);
      TS_ASSERT(comb5->result().multiDim() == pot4);
      comb5->undo();
      delete comb5;

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >* comb6
          = comb4.clone();
      TS_ASSERT(!comb6->implyDeletion())
      TS_ASSERT_EQUALS(*comb6, comb4)
      TS_ASSERT_EQUALS(comb6->result().multiDim(), pot4)
      delete comb6;

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
          comb7 = comb1;

      const gum::Sequence< const gum::IScheduleMultiDim* > new_args32{&f3, &f2};
      comb1.updateArgs(new_args32);
      TS_ASSERT(!comb1.isExecuted());
      comb1.execute();
      gum::Tensor< double > pot32(pot3 + pot2);
      TS_ASSERT(comb1.result().multiDim() == pot32)

      TS_ASSERT(!comb1.hasSameArguments(comb7))
      TS_ASSERT(!comb1.hasSimilarArguments(comb7))
      TS_ASSERT(!comb1.hasSimilarArguments(dynamic_cast< gum::ScheduleOperator& >(comb7)))
      TS_ASSERT(comb1.isSameOperator(comb7))
      TS_ASSERT(comb1.isSameOperator(dynamic_cast< gum::ScheduleOperator& >(comb7)))
      TS_ASSERT_DIFFERS(comb1, comb7)

      comb1.setCombinationFunction(mymult);
      TS_ASSERT(!comb1.isExecuted())
      comb1.execute();
      gum::Tensor< double > mult32(pot3 * pot2);
      TS_ASSERT_EQUALS(comb1.result().multiDim(), mult32)
      TS_ASSERT(!comb1.implyDeletion())
      TS_ASSERT(!comb1.isSameOperator(comb7))


      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    GUM_ACTIVE_TEST(Constants) {
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
      TS_ASSERT(result1.isAbstract())

      comb1.execute();
      TS_ASSERT(comb1.isExecuted())
      TS_ASSERT(!result1.isAbstract())
      TS_ASSERT_EQUALS(result1.multiDim().max(), 8.5)
      TS_ASSERT_EQUALS(result1.multiDim().min(), 8.5)
      TS_ASSERT_EQUALS(result1.domainSize(), 1u)
      TS_ASSERT_EQUALS(result1.multiDim().domainSize(), 1u)


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
      TS_ASSERT_EQUALS(result2.domainSize(), 2u)
      TS_ASSERT_EQUALS(comb2.result().domainSize(), 2u)

      gum::Instantiation i2(result2);
      for (i2.setFirst(); !i2.end(); ++i2) {
        TS_ASSERT_EQUALS(result2[i2], p3[i2] + 3.0)
      }

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
          comb3(f1, f4, myadd);
      comb3.execute();
      const gum::Tensor< double >& result3 = comb3.result().multiDim();
      TS_ASSERT_EQUALS(result3.domainSize(), 4u)
      TS_ASSERT_EQUALS(comb3.result().domainSize(), 4u)

      gum::Instantiation i3(result3);
      for (i3.setFirst(); !i3.end(); ++i3) {
        TS_ASSERT(result3[i3] == p4[i3] + 3.0)
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

} /* namespace gum_tests */
