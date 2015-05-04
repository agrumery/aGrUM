/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <sstream>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/graphs/graphElements.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/potential.h>
#include <agrum/graphicalModels/inference/scheduleCliqueStoreMultiDim.h>

namespace gum_tests {

  class ScheduleCliqueStoreMultiDimTestSuite : public CxxTest::TestSuite {
    public:
    void test_construct() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Potential<float> pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[3]) << *(vars[4]);
      gum::ScheduleMultiDim<float> f1(pot1);
      gum::Potential<float> pot2;
      pot2 << *(vars[0]) << *(vars[2]) << *(vars[3]) << *(vars[4]);
      gum::ScheduleMultiDim<float> f2(pot2);

      gum::NodeProperty<gum::Set<const gum::MultiDimImplementation<float> *>> set;
      TS_ASSERT(set.size() == 0);
      gum::ScheduleCliqueStoreMultiDim<float> store1(f1, set, 3);
      gum::ScheduleCliqueStoreMultiDim<float> store2(f2, set, 3);
      gum::ScheduleCliqueStoreMultiDim<float> store3(f2, set, 2);
      TS_ASSERT(store1 != store2);
      TS_ASSERT(store2 != store3);
      TS_ASSERT(!(store2 == store3));

      TS_ASSERT(store1.nbOperations() == 1);
      std::pair<long, long> xxx = store1.memoryUsage();
      TS_ASSERT(xxx.first == 0);

      gum::Sequence<const gum::ScheduleMultiDim<float> *> seq =
          store1.multiDimArgs();
      TS_ASSERT(seq.size() == 1);
      TS_ASSERT(*(seq.atPos(0)) == f1);

      store1.execute();
      TS_ASSERT_THROWS_NOTHING(store1.execute());
      store2.execute();
      store3.execute();
      TS_ASSERT(set.size() == 2);

      std::stringstream s;
      s << "store ( " << f1.toString() << ", clique " << 3 << " )";
      TS_ASSERT(store1.toString() == s.str());

      gum::ScheduleCliqueStoreMultiDim<float> store4(store3);
      TS_ASSERT(store4 == store3);
      TS_ASSERT(store4 != store1);
      store4 = store1;
      TS_ASSERT(store4 == store1);

      TS_ASSERT(store4.type() ==
                gum::ScheduleOperation<float>::Type::CLIQUE_STORE_MULTIDIM);

      gum::ScheduleCliqueStoreMultiDim<float> *store5 = store4.newFactory();
      TS_ASSERT(*store5 == store4);
      delete store5;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }
  };

} /* namespace gum_tests */
