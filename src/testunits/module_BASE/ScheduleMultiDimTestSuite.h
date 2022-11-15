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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/graphicalModels/inference/scheduler/scheduleMultiDim.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  class [[maybe_unused]] ScheduleMultiDimTestSuite: public CxxTest::TestSuite {
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
        vars[i]       = new gum::LabelizedVariable(s, s, 10);
      }

      gum::Sequence< const gum::DiscreteVariable* > seq;
      seq << vars[0] << vars[2] << vars[4];

      gum::ScheduleMultiDim< gum::Potential< double > > f1(seq);
      TS_ASSERT(f1.isAbstract());
      TS_ASSERT_THROWS(f1.multiDim(), const gum::NullElement&);
      TS_ASSERT(f1.id() == gum::Idx(1));

      std::string       s1 = f1.toString();
      std::stringstream s2;
      s2 << "<id: " << f1.id() << ", table: -->";
      TS_ASSERT(s2.str() == s1);

      gum::ScheduleMultiDim< gum::Potential< double > > f1bis(seq, 10);
      TS_ASSERT_THROWS(f1bis.multiDim(), const gum::NullElement&);
      TS_ASSERT(f1.variablesSequence() == f1bis.variablesSequence());
      TS_ASSERT(f1bis.id() == gum::Idx(10));
      TS_ASSERT(f1bis.domainSize() == gum::Size(1000));

      gum::ScheduleMultiDim< gum::Potential< double > > f1ter(seq, 10);
      TS_ASSERT(f1.variablesSequence() == f1ter.variablesSequence());
      TS_ASSERT(f1ter.id() == gum::Idx(10));
      TS_ASSERT(f1ter.domainSize() == gum::Size(1000));

      gum::ScheduleMultiDim< gum::Potential< double > > f1quart(seq);
      TS_ASSERT(f1.variablesSequence() == f1quart.variablesSequence());
      TS_ASSERT(f1quart.id() == gum::Idx(11));
      TS_ASSERT(f1quart.domainSize() == gum::Size(1000));

      gum::ScheduleMultiDim< gum::Potential< double > > f2(f1);
      TS_ASSERT(f2.isAbstract());
      TS_ASSERT(f2.containsMultiDim() == false);
      TS_ASSERT_THROWS(f2.multiDim(), const gum::NullElement&);
      TS_ASSERT(f1 == f2);
      TS_ASSERT(!(f1 != f2));
      TS_ASSERT(f1 == dynamic_cast< gum::IScheduleMultiDim& >(f2));
      TS_ASSERT(!(f1 != dynamic_cast< gum::IScheduleMultiDim& >(f2)));
      TS_ASSERT(f1.id() == f2.id());
      TS_ASSERT(f1.variablesSequence() == f2.variablesSequence());
      TS_ASSERT(f2.domainSize() == gum::Size(1000));

      f2.makeAbstract();
      TS_ASSERT(f2.isAbstract());
      TS_ASSERT(f2.containsMultiDim() == false);
      TS_ASSERT(f2.domainSize() == gum::Size(1000));
      TS_ASSERT(f2.variablesSequence() == seq);

      gum::ScheduleMultiDim< gum::Potential< double > > f3(seq);
      TS_ASSERT(f1 != f3);
      TS_ASSERT(f1.hasSameVariables(f3));
      TS_ASSERT(f1.hasSameContent(f3));
      TS_ASSERT(f1.hasSameVariables(dynamic_cast< gum::IScheduleMultiDim& >(f3)));
      TS_ASSERT(f1.hasSameContent(dynamic_cast< gum::IScheduleMultiDim& >(f3)));
      TS_ASSERT(f1.variablesSequence() == f3.variablesSequence());
      TS_ASSERT(f3.domainSize() == gum::Size(1000));

      gum::ScheduleMultiDim< gum::Potential< double > > f3b(std::move(f3));
      TS_ASSERT(f1 != f3b);
      TS_ASSERT(f1.hasSameVariables(f3b));
      TS_ASSERT(f1.hasSameContent(f3b));
      TS_ASSERT(f1.variablesSequence() == f3b.variablesSequence())

      gum::Potential< double > pot;
      pot << *(vars[0]) << *(vars[2]) << *(vars[4]);
      const std::vector< double > cpt(1000, 10);
      pot.fillWith(cpt);

      gum::ScheduleMultiDim< gum::Potential< double > > f4(pot, true, 5);
      TS_ASSERT(!f4.isAbstract());
      TS_ASSERT(f1 != f4);
      TS_ASSERT(f1.hasSameVariables(f4));
      TS_ASSERT(!f1.hasSameContent(f4));
      const gum::Potential< double >& res_pot = f4.multiDim();
      TS_ASSERT(pot == res_pot);
      TS_ASSERT(f4.containsMultiDim());
      TS_ASSERT(f4.domainSize() == gum::Size(1000));

      gum::ScheduleMultiDim< gum::Potential< double > > f4bis(pot, true);
      TS_ASSERT(!f4bis.isAbstract());
      TS_ASSERT(f4.hasSameVariables(f4bis));
      TS_ASSERT(f4.hasSameContent(f4bis));
      TS_ASSERT(f4 != f4bis);
      TS_ASSERT(!(f4 == f4bis));
      const gum::Potential< double >& res_pot_bis = f4bis.multiDim();
      TS_ASSERT(res_pot == res_pot_bis);
      TS_ASSERT(f4bis.id() == gum::Idx(13));
      TS_ASSERT(f4bis.domainSize() == gum::Size(1000));

      gum::ScheduleMultiDim< gum::Potential< double > > f4ter(pot, true, 2);
      TS_ASSERT(!f4ter.isAbstract());
      const gum::Potential< double >& res_pot_ter = f4ter.multiDim();
      TS_ASSERT(res_pot == res_pot_ter);
      TS_ASSERT(f4ter.id() == gum::Idx(2));
      TS_ASSERT(f4ter.domainSize() == gum::Size(1000));

      gum::ScheduleMultiDim< gum::Potential< double > > f4b(pot, false);
      TS_ASSERT(!f4b.isAbstract());
      TS_ASSERT(f4.multiDim() == f4b.multiDim());
      TS_ASSERT(f4 != f4b);
      TS_ASSERT(f4.hasSameVariables(f4b));
      TS_ASSERT(!f4b.containsMultiDim());
      TS_ASSERT(f4b.id() == gum::Idx(14));
      TS_ASSERT(f4b.domainSize() == gum::Size(1000));

      gum::Potential< double >                          xxpot = res_pot;
      gum::ScheduleMultiDim< gum::Potential< double > > f4bt(xxpot, false, 5);
      TS_ASSERT(!f4bt.isAbstract());
      TS_ASSERT(f4bt.multiDim() == f4.multiDim());
      TS_ASSERT(f4ter.hasSameVariables(f4bt));
      TS_ASSERT(!f4bt.containsMultiDim());
      TS_ASSERT(f4bt.id() == gum::Idx(5));
      TS_ASSERT(f4bt.domainSize() == gum::Size(1000));

      gum::Potential< double >                          potx = res_pot;
      gum::ScheduleMultiDim< gum::Potential< double > > f4c(std::move(potx));
      TS_ASSERT(!f4c.isAbstract());
      TS_ASSERT(f4c.containsMultiDim());
      TS_ASSERT(f4.multiDim() == f4c.multiDim());
      TS_ASSERT(f4.hasSameVariables(f4c));
      TS_ASSERT(f4c.id() == gum::Idx(15));
      TS_ASSERT(f4c.domainSize() == gum::Size(1000));

      gum::Potential< double >                          potxx = res_pot;
      gum::ScheduleMultiDim< gum::Potential< double > > f4d(std::move(potxx), 10);
      TS_ASSERT(!f4d.isAbstract());
      TS_ASSERT(f4d.containsMultiDim());
      TS_ASSERT(f4.multiDim() == f4d.multiDim());
      TS_ASSERT(f4.hasSameVariables(f4d));
      TS_ASSERT(f4 != f4d);
      TS_ASSERT(f4d.id() == gum::Idx(10));

      f4d.makeAbstract();
      TS_ASSERT(f4d.isAbstract());
      TS_ASSERT(f4d.containsMultiDim() == false);
      TS_ASSERT(f4d.domainSize() == gum::Size(1000));
      TS_ASSERT(f4d.variablesSequence() == seq);

      gum::ScheduleMultiDim< gum::Potential< double > > f5(f4);
      TS_ASSERT(!f5.isAbstract());
      TS_ASSERT(f5 == f4);
      TS_ASSERT(f5.hasSameContent(f4));
      TS_ASSERT(f4.id() == f5.id());
      TS_ASSERT(f4.multiDim() == res_pot);

      std::string       s5 = f5.toString();
      std::stringstream s6;
      s6 << "<id: 5, table: " << f5.multiDim().content() << ">";
      TS_ASSERT(s6.str() == s5);

      gum::Potential< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[4]);
      const std::vector< double > cpt2(1000, 42);
      pot2.fillWith(cpt2);
      f4.setMultiDim(pot2, true);
      gum::ScheduleMultiDim< gum::Potential< double > > f6(f4);
      TS_ASSERT(f6.multiDim() == pot2);
      f6.setMultiDim(pot, true);
      TS_ASSERT(f6.multiDim() == pot);
      TS_ASSERT(f6.multiDim() != f4.multiDim());

      TS_ASSERT(f4.containsMultiDim());
      f6 = f4;
      TS_ASSERT(f6 == f4);
      TS_ASSERT(f6.hasSameVariables(f4));
      TS_ASSERT(f6.hasSameContent(f4));

      gum::ScheduleMultiDim< gum::Potential< double > > f7(pot, false, 5);
      f7 = f4;
      TS_ASSERT(f7 == f4);

      gum::ScheduleMultiDim< gum::Potential< double > > f8(pot, false, 8);
      gum::ScheduleMultiDim< gum::Potential< double > > f9(pot2, true, 9);
      TS_ASSERT(f9.multiDim() != f8.multiDim());
      f9 = f8;
      TS_ASSERT(!f8.containsMultiDim());
      TS_ASSERT(!f9.containsMultiDim());
      TS_ASSERT(f8 == f9);
      TS_ASSERT(f8.hasSameVariables(f9));
      TS_ASSERT(f8.hasSameContent(f9));

      gum::ScheduleMultiDim< gum::Potential< double > > f10(pot, false, 10);
      gum::ScheduleMultiDim< gum::Potential< double > > f11(pot2, true, 10);
      f10 = std::move(f11);
      TS_ASSERT(f10.containsMultiDim());
      TS_ASSERT(f10.variablesSequence() == pot2.variablesSequence());

      auto pot3 = f10.exportMultiDim();
      TS_ASSERT(f10.isAbstract());
      TS_ASSERT(!f10.containsMultiDim());
      TS_ASSERT(*pot3 == pot2);
      f10.makeAbstract();
      TS_ASSERT(f10.isAbstract());
      delete pot3;

      const auto pot4 = pot2;
      f10.setMultiDim(std::move(pot2));
      TS_ASSERT(f10.containsMultiDim());
      TS_ASSERT(f10.multiDim() == pot4);

      gum::ScheduleMultiDim< gum::Potential< double > >* f12 = f10.clone();
      TS_ASSERT(*f12 == f10);
      TS_ASSERT(f10.hasSameVariables(*f12));
      TS_ASSERT(f10.hasSameContent(*f12));

      delete f12;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void testConstants() {
      gum::Potential< double > pot;
      pot.fillWith({42.42});

      gum::ScheduleMultiDim< gum::Potential< double > > f1(pot, false);
      TS_ASSERT(!f1.containsMultiDim());
      TS_ASSERT(f1.domainSize() == 1);

      gum::ScheduleMultiDim< gum::Potential< double > > f2(pot, true);
      TS_ASSERT(f2.containsMultiDim());
      TS_ASSERT(f2.domainSize() == 1);
      gum::Potential< double >* pot2 = f2.exportMultiDim();
      TS_ASSERT(pot2->max() == pot2->min());
      TS_ASSERT(pot2->min() == 42.42);
      delete pot2;

      gum::Sequence< const gum::DiscreteVariable* >     seq;
      gum::ScheduleMultiDim< gum::Potential< double > > f3(seq, 0);
      TS_ASSERT(!f3.containsMultiDim());
      TS_ASSERT(f3.domainSize() == 1);
      TS_ASSERT(f3.isAbstract());
    }
  };

} /* namespace gum_tests */
