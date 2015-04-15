/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
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

#include <iostream>
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/learning/database/databaseFromCSV.h>
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.h>
#include <agrum/learning/scores_and_tests/counter.h>

namespace gum_tests {

  class CounterTestSuite : public CxxTest::TestSuite {
    public:
    struct MyCounter : public gum::learning::Counter<> {
      template <typename RowFilter>
      MyCounter(const RowFilter &filter,
                const std::vector<unsigned int> &var_modalities)
          : gum::learning::Counter<>(filter, var_modalities) {}

      const std::vector<float> &getAllCounts(unsigned int index) {
        return gum::learning::Counter<>::_getAllCounts(index);
      }

      const std::vector<float> &getConditioningCounts(unsigned int index) {
        return gum::learning::Counter<>::_getConditioningCounts(index);
      }

      std::vector<std::vector<float>> &getCounts() noexcept {
        return gum::learning::Counter<>::_getCounts();
      }

      const std::vector<unsigned int> &getAllNodes(unsigned int index) const
          noexcept {
        return gum::learning::Counter<>::_getAllNodes(index);
      }

      const std::vector<std::pair<std::vector<unsigned int>, unsigned int> *> &
      getAllNodes() const noexcept {
        return gum::learning::Counter<>::_getAllNodes();
      }

      /// returns the conditioning nodes (nullptr if there are no such nodes)
      const std::vector<unsigned int> *getConditioningNodes(unsigned int index) const
          noexcept {
        return gum::learning::Counter<>::_getConditioningNodes(index);
      }

      const std::vector<std::pair<std::vector<unsigned int>, unsigned int> *> &
      getConditioningNodes() const noexcept {
        return gum::learning::Counter<>::_getConditioningNodes();
      }
    };

    void test1() {
      gum::learning::DatabaseFromCSV database(GET_PATH_STR("asia.csv"));

      auto translators = gum::learning::make_translators(gum::learning::Create<
          gum::learning::CellTranslatorCompactIntId, gum::learning::Col<0>, 8>());

      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());

      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector<unsigned int> modalities(8, 2);

      MyCounter counter(filter, modalities);

      std::vector<unsigned int> set1{0};
      std::vector<unsigned int> set2{1};
      std::vector<unsigned int> set3{1, 0};

      {
        unsigned int id1 = counter.addNodeSet(0);
        unsigned int id2 = counter.addNodeSet(1);
        unsigned int id3 = counter.addNodeSet(0, set2);

        const std::vector<float> &vect1 = counter.getAllCounts(id1);
        const std::vector<float> &vect2 = counter.getAllCounts(id2);
        const std::vector<float> &vect3 = counter.getAllCounts(id3);

        TS_ASSERT(vect1[0] == 5028);
        TS_ASSERT(vect1[1] == 4972);
        TS_ASSERT(vect2[0] == 538);
        TS_ASSERT(vect2[1] == 9462);

        TS_ASSERT(vect3[0] == 498);
        TS_ASSERT(vect3[1] == 4530);
        TS_ASSERT(vect3[2] == 40);
        TS_ASSERT(vect3[3] == 4932);
      }

      counter.clear();
      std::vector<unsigned int> set4{1, 2, 3};
      {
        counter.addNodeSet(0, set2);
        unsigned int id2 = counter.addNodeSet(0, set4);
        counter.addNodeSet(2, set1);
        unsigned int id4 = counter.addNodeSet(0, set4);

        TS_ASSERT(counter.getAllCounts(id2) == counter.getAllCounts(id4));
        TS_ASSERT(counter.getConditioningCounts(id2) ==
                  counter.getConditioningCounts(id4));
      }

      counter.clear();
      unsigned int id = counter.addNodeSet(0, set4);
      std::vector<float> vectx = counter.getAllCounts(id);
      std::vector<float> vecty = counter.getConditioningCounts(id);

      counter.clear();
      {
        std::vector<unsigned int> set5{2, 1, 3};
        std::vector<unsigned int> set6{3, 1, 2};
        std::vector<unsigned int> set7{3, 2, 1};
        std::vector<unsigned int> set8{1, 3, 2};
        unsigned int id1 = counter.addNodeSet(0, set4);
        counter.addEmptyNodeSet();
        unsigned int id2 = counter.addNodeSet(0, set5);
        unsigned int id3 = counter.addNodeSet(0, set6);
        counter.addEmptyNodeSet();
        counter.addEmptyNodeSet();
        unsigned int id4 = counter.addNodeSet(0, set7);
        unsigned int id5 = counter.addNodeSet(0, set8);

        TS_ASSERT(compare_vect3(vectx, counter.getAllCounts(id1), {1, 2, 3}));
        TS_ASSERT(compare_vect3(vectx, counter.getAllCounts(id2), {2, 1, 3}));
        TS_ASSERT(compare_vect3(vectx, counter.getAllCounts(id3), {3, 1, 2}));
        TS_ASSERT(compare_vect3(vectx, counter.getAllCounts(id4), {3, 2, 1}));
        TS_ASSERT(compare_vect3(vectx, counter.getAllCounts(id5), {1, 3, 2}));

        TS_ASSERT(
            compare_vect3(vecty, counter.getConditioningCounts(id1), {1, 2, 3}));
        TS_ASSERT(
            compare_vect3(vecty, counter.getConditioningCounts(id2), {2, 1, 3}));
        TS_ASSERT(
            compare_vect3(vecty, counter.getConditioningCounts(id3), {3, 1, 2}));
        TS_ASSERT(
            compare_vect3(vecty, counter.getConditioningCounts(id4), {3, 2, 1}));
        TS_ASSERT(
            compare_vect3(vecty, counter.getConditioningCounts(id5), {1, 3, 2}));
      }

      counter.clear();
      {
        std::vector<unsigned int> set5{2, 1, 3};
        std::vector<unsigned int> set6{3, 1, 2};
        std::vector<unsigned int> set7{3, 2, 1};
        std::vector<unsigned int> set8{1, 3, 2};
        unsigned int id1 = counter.addNodeSet(0, set4);
        counter.addEmptyNodeSet();
        unsigned int id2 = counter.addNodeSet(0, set5);
        unsigned int id3 = counter.addNodeSet(0, set6);
        counter.addEmptyNodeSet();
        counter.addEmptyNodeSet();
        unsigned int id4 = counter.addNodeSet(0, set7);
        unsigned int id5 = counter.addNodeSet(0, set8);

        MyCounter counter2(counter);

        TS_ASSERT(compare_vect3(vectx, counter2.getAllCounts(id1), {1, 2, 3}));
        TS_ASSERT(compare_vect3(vectx, counter2.getAllCounts(id2), {2, 1, 3}));
        TS_ASSERT(compare_vect3(vectx, counter2.getAllCounts(id3), {3, 1, 2}));
        TS_ASSERT(compare_vect3(vectx, counter2.getAllCounts(id4), {3, 2, 1}));
        TS_ASSERT(compare_vect3(vectx, counter2.getAllCounts(id5), {1, 3, 2}));

        TS_ASSERT(
            compare_vect3(vecty, counter2.getConditioningCounts(id1), {1, 2, 3}));
        TS_ASSERT(
            compare_vect3(vecty, counter2.getConditioningCounts(id2), {2, 1, 3}));
        TS_ASSERT(
            compare_vect3(vecty, counter2.getConditioningCounts(id3), {3, 1, 2}));
        TS_ASSERT(
            compare_vect3(vecty, counter2.getConditioningCounts(id4), {3, 2, 1}));
        TS_ASSERT(
            compare_vect3(vecty, counter2.getConditioningCounts(id5), {1, 3, 2}));

        MyCounter counter3(counter2);

        TS_ASSERT(compare_vect3(vectx, counter3.getAllCounts(id1), {1, 2, 3}));
        TS_ASSERT(compare_vect3(vectx, counter3.getAllCounts(id2), {2, 1, 3}));
        TS_ASSERT(compare_vect3(vectx, counter3.getAllCounts(id3), {3, 1, 2}));
        TS_ASSERT(compare_vect3(vectx, counter3.getAllCounts(id4), {3, 2, 1}));
        TS_ASSERT(compare_vect3(vectx, counter3.getAllCounts(id5), {1, 3, 2}));

        TS_ASSERT(
            compare_vect3(vecty, counter3.getConditioningCounts(id1), {1, 2, 3}));
        TS_ASSERT(
            compare_vect3(vecty, counter3.getConditioningCounts(id2), {2, 1, 3}));
        TS_ASSERT(
            compare_vect3(vecty, counter3.getConditioningCounts(id3), {3, 1, 2}));
        TS_ASSERT(
            compare_vect3(vecty, counter3.getConditioningCounts(id4), {3, 2, 1}));
        TS_ASSERT(
            compare_vect3(vecty, counter3.getConditioningCounts(id5), {1, 3, 2}));

        MyCounter counter4(std::move(counter2));

        TS_ASSERT(compare_vect3(vectx, counter4.getAllCounts(id1), {1, 2, 3}));
        TS_ASSERT(compare_vect3(vectx, counter4.getAllCounts(id2), {2, 1, 3}));
        TS_ASSERT(compare_vect3(vectx, counter4.getAllCounts(id3), {3, 1, 2}));
        TS_ASSERT(compare_vect3(vectx, counter4.getAllCounts(id4), {3, 2, 1}));
        TS_ASSERT(compare_vect3(vectx, counter4.getAllCounts(id5), {1, 3, 2}));

        TS_ASSERT(
            compare_vect3(vecty, counter4.getConditioningCounts(id1), {1, 2, 3}));
        TS_ASSERT(
            compare_vect3(vecty, counter4.getConditioningCounts(id2), {2, 1, 3}));
        TS_ASSERT(
            compare_vect3(vecty, counter4.getConditioningCounts(id3), {3, 1, 2}));
        TS_ASSERT(
            compare_vect3(vecty, counter4.getConditioningCounts(id4), {3, 2, 1}));
        TS_ASSERT(
            compare_vect3(vecty, counter4.getConditioningCounts(id5), {1, 3, 2}));
      }

      counter.clear();
      {
        std::vector<unsigned int> set5{2, 1, 3};
        std::vector<unsigned int> set6{3, 1, 2};
        std::vector<unsigned int> set7{3, 2, 1};
        std::vector<unsigned int> set8{1, 3, 2};
        unsigned int id1 = counter.addNodeSet(0, set4);
        counter.addEmptyNodeSet();
        unsigned int id2 = counter.addNodeSet(0, set5);
        unsigned int id3 = counter.addNodeSet(0, set6);
        counter.addEmptyNodeSet();
        counter.addEmptyNodeSet();
        unsigned int id4 = counter.addNodeSet(0, set7);
        unsigned int id5 = counter.addNodeSet(0, set8);

        MyCounter counter2(std::move(counter));

        TS_ASSERT(compare_vect3(vectx, counter2.getAllCounts(id1), {1, 2, 3}));
        TS_ASSERT(compare_vect3(vectx, counter2.getAllCounts(id2), {2, 1, 3}));
        TS_ASSERT(compare_vect3(vectx, counter2.getAllCounts(id3), {3, 1, 2}));
        TS_ASSERT(compare_vect3(vectx, counter2.getAllCounts(id4), {3, 2, 1}));
        TS_ASSERT(compare_vect3(vectx, counter2.getAllCounts(id5), {1, 3, 2}));

        TS_ASSERT(
            compare_vect3(vecty, counter2.getConditioningCounts(id1), {1, 2, 3}));
        TS_ASSERT(
            compare_vect3(vecty, counter2.getConditioningCounts(id2), {2, 1, 3}));
        TS_ASSERT(
            compare_vect3(vecty, counter2.getConditioningCounts(id3), {3, 1, 2}));
        TS_ASSERT(
            compare_vect3(vecty, counter2.getConditioningCounts(id4), {3, 2, 1}));
        TS_ASSERT(
            compare_vect3(vecty, counter2.getConditioningCounts(id5), {1, 3, 2}));
      }
    }

    bool compare_vect3(const std::vector<float> &v1, const std::vector<float> &v2,
                       const std::vector<unsigned int> &order) {
      unsigned int index1, index2;
      for (unsigned int k = 0; k < 2; ++k) {
        for (unsigned int j = 0; j < 2; ++j) {
          for (unsigned int i = 0; i < 2; ++i) {
            index2 = i + j * 2 + k * 4;
            index1 = i * (1 << (order[0] - 1)) + j * (1 << (order[1] - 1)) +
                     k * (1 << (order[2] - 1));
            if (v1[index1] != v2[index2])
              return false;
          }
        }
      }

      return true;
    }

    bool compare_vect2(const std::vector<float> &v1, const std::vector<float> &v2,
                       const std::vector<unsigned int> &order) {
      unsigned int index1, index2;
      for (unsigned int j = 0; j < 2; ++j) {
        for (unsigned int i = 0; i < 2; ++i) {
          index2 = i + j * 2;
          index1 = i * (1 << (order[0] - 1)) + j * (1 << (order[1] - 1));
          if (v1[index1] != v2[index2])
            return false;
        }
      }

      return true;
    }
  };

} /* namespace gum_tests */
