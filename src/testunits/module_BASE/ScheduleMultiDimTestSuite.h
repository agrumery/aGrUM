/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
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

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/tools/graphicalModels/inference/scheduler/scheduleMultiDim.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/labelizedVariable.h>
namespace gum_tests {

  class ScheduleMultiDimTestSuite: public CxxTest::TestSuite {
    public:
    void test_construct() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Sequence< const gum::DiscreteVariable* > seq;
      seq << vars[0] << vars[2] << vars[4];

      gum::ScheduleMultiDim< double > f1(seq);
      TS_ASSERT(f1.isAbstract());
      TS_ASSERT_THROWS(f1.multiDim(), gum::NotFound);

      std::string       s1 = f1.toString();
      std::stringstream s2;
      s2 << "<" << f1.id() << ">";
      TS_ASSERT(s2.str() == s1);

      gum::ScheduleMultiDim< double > f2(f1);
      TS_ASSERT(f2.isAbstract());
      TS_ASSERT_THROWS(f2.multiDim(), gum::NotFound);
      TS_ASSERT(f1 == f2);
      TS_ASSERT(f1.id() == f2.id());
      TS_ASSERT(f1.variablesSequence() == f2.variablesSequence());

      gum::ScheduleMultiDim< double > f3(seq);
      TS_ASSERT(f1 != f3);
      TS_ASSERT(f1.id() != f3.id());
      TS_ASSERT(f1.variablesSequence() == f3.variablesSequence());

      gum::Potential< double > pot;
      pot << *(vars[0]) << *(vars[2]) << *(vars[4]);
      gum::ScheduleMultiDim< double > f4(pot);
      TS_ASSERT(!f4.isAbstract());
      TS_ASSERT(f1 != f4);

      gum::ScheduleMultiDim< double > f5(f4);
      TS_ASSERT(!f5.isAbstract());
      TS_ASSERT(f5 == f4);
      TS_ASSERT(f4.id() == f5.id());
      TS_ASSERT(f4.multiDim() == pot);

      std::string       s5 = f5.toString();
      std::stringstream s6;
      s6 << "<" << pot.content() << ">";
      TS_ASSERT(s6.str() == s5);

      gum::Potential< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[4]);
      f4.setMultiDim(pot2);
      gum::ScheduleMultiDim< double > f6(f4);
      TS_ASSERT(f5.multiDim() == *pot2.content());
      TS_ASSERT_THROWS(f3.setMultiDim(*pot2.content()), gum::DuplicateElement);
      TS_ASSERT(f6.multiDim() == *pot2.content());
      TS_ASSERT(f6.multiDim() == *pot2.content());

      std::string str = f3.toString();
      f3 = f5;
      TS_ASSERT(f5 == f3);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }
  };

} /* namespace gum_tests */
