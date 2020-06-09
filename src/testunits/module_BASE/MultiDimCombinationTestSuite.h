
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/multidim/utils/operators/multiDimCombinationDefault.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  class MultiDimCombinationTestSuite: public CxxTest::TestSuite {
    private:
    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInitP(gum::Potential< double >& t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, rand() * 100000.0f / RAND_MAX);
    }

    // the function used to combine two tables
    static gum::Potential< double >*
       addPotential(const gum::Potential< double >& t1,
                    const gum::Potential< double >& t2) {
      return new gum::Potential< double >(t1 + t2);
    }

    // the function used to combine two tables
    static gum::Potential< double >*
       multPotential(const gum::Potential< double >& t1,
                     const gum::Potential< double >& t2) {
      return new gum::Potential< double >(t1 * t2);
    }

    public:
    void test_op_multidimArray() {
      try {
        std::vector< gum::LabelizedVariable* > vars(10);

        for (gum::Idx i = 0; i < 10; ++i) {
          std::stringstream str;
          str << "x" << i;
          std::string s = str.str();
          vars[i] = new gum::LabelizedVariable(s, s, 4);
        }

        gum::Potential< double > t1, t2, t3;

        t1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
        t2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
        t3 << *(vars[6]) << *(vars[4]) << *(vars[3]);

        randomInitP(t1);
        randomInitP(t2);
        randomInitP(t3);

        gum::Potential< double >*t4, *t5, *t6;
        t4 = new gum::Potential< double >(t1 + t2);
        t5 = new gum::Potential< double >(t3 + *t4);

        gum::Set< const gum::Potential< double >* > set;
        set << &t1 << &t2 << &t3;

        gum::MultiDimCombinationDefault< double, gum::Potential > xxx(
           addPotential);
        t6 = xxx.combine(set);
        TS_ASSERT(t6);
        TS_ASSERT(*t6 == *t5);

        delete t4;
        delete t5;
        delete t6;

        TS_ASSERT(xxx.nbOperations(set) == 16640);
        std::pair< long, long > yyy = xxx.memoryUsage(set);
        TS_ASSERT(yyy.first == 16640);
        TS_ASSERT(yyy.second == 16384);

        t4 = new gum::Potential< double >(t1 * t2);
        t5 = new gum::Potential< double >(t3 * (*t4));
        xxx.setCombineFunction(multPotential);
        t6 = xxx.combine(set);
        TS_ASSERT(t6);
        TS_ASSERT(*t6 == *t5);

        delete t4;
        delete t5;
        delete t6;

        for (gum::Idx i = 0; i < vars.size(); ++i)
          delete vars[i];
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }


    void testConstants() {
      gum::Potential< double > t1, t2;
      gum::Instantiation       inst1(t1), inst2(t2);
      t1.set(inst1, 3.0);
      t2.set(inst2, 4.0);

      gum::MultiDimCombinationDefault< double, gum::Potential > xxx(multPotential);
      gum::Set< const gum::Potential< double >* >               set;
      set << &t1 << &t2;

      gum::Potential< double >* t3 = xxx.combine(set);
      {
        gum::Instantiation inst3(t3);
        TS_ASSERT((*t3)[inst3] == 12.0);
      }
      delete t3;


      std::vector< gum::LabelizedVariable* > vars(2);

      for (gum::Idx i = 0; i < 2; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Potential< double > t4;
      t4 << *(vars[0]) << *(vars[1]);
      randomInitP(t4);
      t4.normalize();

      set.clear();
      set << &t1 << &t4;
      t3 = xxx.combine(set);
      {
        gum::Instantiation inst3(t3);
        double             x = 0;
        for (inst3.setFirst(); !inst3.end(); ++inst3)
          x += (*t3)[inst3];
        TS_ASSERT_DELTA(x, 3.0, 0.001);
      }
      delete t3;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }
  };

} /* namespace gum_tests */
