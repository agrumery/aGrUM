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
#include <agrum/learning/scores_and_tests/recordCounter.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.h>

namespace gum_tests {

  class RecordCounterTestSuite : public CxxTest::TestSuite {
    public:
    void test1() {
      gum::learning::DatabaseFromCSV database(GET_PATH_STR("asia.csv"));

      auto translators = gum::learning::make_translators(gum::learning::Create<
          gum::learning::CellTranslatorCompactIntId, gum::learning::Col<0>, 8>());

      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());

      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector<unsigned int> modalities(8, 2);

      gum::learning::RecordCounter<> counter(filter, modalities);

      std::vector<unsigned int> set1{0};
      std::vector<unsigned int> set2{1};
      std::vector<unsigned int> set3{1, 0};

      {
        unsigned int id1 = counter.addNodeSet(set1);
        unsigned int id2 = counter.addNodeSet(set2);
        unsigned int id3 = counter.addNodeSet(set3);

        counter.count();

        const std::vector<float> &vect1 = counter.getCounts(id1);
        const std::vector<float> &vect2 = counter.getCounts(id2);
        const std::vector<float> &vect3 = counter.getCounts(id3);

        TS_ASSERT(vect1[0] == 5028);
        TS_ASSERT(vect1[1] == 4972);
        TS_ASSERT(vect2[0] == 538);
        TS_ASSERT(vect2[1] == 9462);

        TS_ASSERT(vect3[0] == 498);
        TS_ASSERT(vect3[1] == 4530);
        TS_ASSERT(vect3[2] == 40);
        TS_ASSERT(vect3[3] == 4932);
      }

      counter.clearNodeSets();
      std::vector<unsigned int> set4{1, 2, 3};
      {
        counter.addNodeSet(set2);
        unsigned int id2 = counter.addNodeSet(set4);
        counter.addNodeSet(set1);
        unsigned int id4 = counter.addNodeSet(set4);

        counter.count();

        TS_ASSERT(counter.getCounts(id2) == counter.getCounts(id4));
      }

      counter.clearNodeSets();
      unsigned int id = counter.addNodeSet(set4);
      counter.count();
      std::vector<float> vect = counter.getCounts(id);

      counter.clearNodeSets();
      {
        std::vector<unsigned int> set5{2, 1, 3};
        std::vector<unsigned int> set6{3, 1, 2};
        std::vector<unsigned int> set7{3, 2, 1};
        std::vector<unsigned int> set8{1, 3, 2};
        unsigned int id1 = counter.addNodeSet(set4);
        unsigned int id2 = counter.addNodeSet(set5);
        unsigned int id3 = counter.addNodeSet(set6);
        unsigned int id4 = counter.addNodeSet(set7);
        unsigned int id5 = counter.addNodeSet(set8);

        counter.count();

        TS_ASSERT(compare_vect3(vect, counter.getCounts(id1), {1, 2, 3}));
        TS_ASSERT(compare_vect3(vect, counter.getCounts(id2), {2, 1, 3}));
        TS_ASSERT(compare_vect3(vect, counter.getCounts(id3), {3, 1, 2}));
        TS_ASSERT(compare_vect3(vect, counter.getCounts(id4), {3, 2, 1}));
        TS_ASSERT(compare_vect3(vect, counter.getCounts(id5), {1, 3, 2}));
      }

      counter.clearNodeSets();
      std::vector<unsigned int> setx13{1, 3};
      std::vector<unsigned int> setx23{2, 3};
      std::vector<unsigned int> setx2{2};
      std::vector<unsigned int> setx3{3};
      unsigned int idx13 = counter.addNodeSet(setx13);
      unsigned int idx23 = counter.addNodeSet(setx23);
      unsigned int idx2 = counter.addNodeSet(setx2);
      unsigned int idx3 = counter.addNodeSet(setx3);
      counter.count();
      std::vector<float> vectx13 = counter.getCounts(idx13);
      std::vector<float> vectx23 = counter.getCounts(idx23);
      std::vector<float> vectx2 = counter.getCounts(idx2);
      std::vector<float> vectx3 = counter.getCounts(idx3);

      counter.clearNodeSets();
      {
        std::vector<unsigned int> set5{1, 3};
        std::vector<unsigned int> set6{3, 1};
        std::vector<unsigned int> set7{3, 2};
        std::vector<unsigned int> set8{2, 3};
        std::vector<unsigned int> set9{3};
        std::vector<unsigned int> set10{2};
        unsigned int id1 = counter.addNodeSet(set4);
        unsigned int id2 = counter.addNodeSet(set5);
        unsigned int id3 = counter.addNodeSet(set6);
        unsigned int id4 = counter.addNodeSet(set7);
        unsigned int id5 = counter.addNodeSet(set8);
        unsigned int id6 = counter.addNodeSet(set9);
        unsigned int id7 = counter.addNodeSet(set10);

        counter.count();

        TS_ASSERT(compare_vect3(vect, counter.getCounts(id1), {1, 2, 3}));
        TS_ASSERT(compare_vect2(vectx13, counter.getCounts(id2), {1, 2}));
        TS_ASSERT(compare_vect2(vectx13, counter.getCounts(id3), {2, 1}));
        TS_ASSERT(compare_vect2(vectx23, counter.getCounts(id4), {2, 1}));
        TS_ASSERT(compare_vect2(vectx23, counter.getCounts(id5), {1, 2}));
        TS_ASSERT(vectx3 == counter.getCounts(id6));
        TS_ASSERT(vectx2 == counter.getCounts(id7));
      }
    }

    void test_copy_move() {
      gum::learning::DatabaseFromCSV database(GET_PATH_STR("asia.csv"));

      auto translators = gum::learning::make_translators(gum::learning::Create<
          gum::learning::CellTranslatorCompactIntId, gum::learning::Col<0>, 8>());

      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());

      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector<unsigned int> modalities(8, 2);

      gum::learning::RecordCounter<> counter(filter, modalities);

      std::vector<unsigned int> set1{0};
      std::vector<unsigned int> set2{1};
      std::vector<unsigned int> set3{1, 0};

      {
        unsigned int id1 = counter.addNodeSet(set1);
        unsigned int id2 = counter.addNodeSet(set2);
        unsigned int id3 = counter.addNodeSet(set3);

        counter.count();

        gum::learning::RecordCounter<> counter2(counter);

        const std::vector<float> &vect1 = counter2.getCounts(id1);
        const std::vector<float> &vect2 = counter2.getCounts(id2);
        const std::vector<float> &vect3 = counter2.getCounts(id3);

        TS_ASSERT(vect1[0] == 5028);
        TS_ASSERT(vect1[1] == 4972);
        TS_ASSERT(vect2[0] == 538);
        TS_ASSERT(vect2[1] == 9462);

        TS_ASSERT(vect3[0] == 498);
        TS_ASSERT(vect3[1] == 4530);
        TS_ASSERT(vect3[2] == 40);
        TS_ASSERT(vect3[3] == 4932);

        gum::learning::RecordCounter<> counter3(std::move(counter2));

        const std::vector<float> &vvect1 = counter3.getCounts(id1);
        const std::vector<float> &vvect2 = counter3.getCounts(id2);
        const std::vector<float> &vvect3 = counter3.getCounts(id3);

        TS_ASSERT(vvect1[0] == 5028);
        TS_ASSERT(vvect1[1] == 4972);
        TS_ASSERT(vvect2[0] == 538);
        TS_ASSERT(vvect2[1] == 9462);

        TS_ASSERT(vvect3[0] == 498);
        TS_ASSERT(vvect3[1] == 4530);
        TS_ASSERT(vvect3[2] == 40);
        TS_ASSERT(vvect3[3] == 4932);
      }

      counter.clearNodeSets();
      std::vector<unsigned int> set4{1, 2, 3};
      {
        counter.addNodeSet(set2);
        unsigned int id2 = counter.addNodeSet(set4);
        counter.addNodeSet(set1);
        unsigned int id4 = counter.addNodeSet(set4);

        counter.count();

        TS_ASSERT(counter.getCounts(id2) == counter.getCounts(id4));
      }

      counter.clearNodeSets();
      unsigned int id = counter.addNodeSet(set4);
      counter.count();
      std::vector<float> vect = counter.getCounts(id);

      counter.clearNodeSets();
      {
        std::vector<unsigned int> set5{2, 1, 3};
        std::vector<unsigned int> set6{3, 1, 2};
        std::vector<unsigned int> set7{3, 2, 1};
        std::vector<unsigned int> set8{1, 3, 2};
        unsigned int id1 = counter.addNodeSet(set4);
        unsigned int id2 = counter.addNodeSet(set5);
        unsigned int id3 = counter.addNodeSet(set6);
        unsigned int id4 = counter.addNodeSet(set7);
        unsigned int id5 = counter.addNodeSet(set8);

        gum::learning::RecordCounter<> counter2(counter);
        gum::learning::RecordCounter<> counter2bis(counter);
        gum::learning::RecordCounter<> counter3(std::move(counter2bis));

        counter2.count();
        counter3.count();

        TS_ASSERT(compare_vect3(vect, counter2.getCounts(id1), {1, 2, 3}));
        TS_ASSERT(compare_vect3(vect, counter2.getCounts(id2), {2, 1, 3}));
        TS_ASSERT(compare_vect3(vect, counter2.getCounts(id3), {3, 1, 2}));
        TS_ASSERT(compare_vect3(vect, counter2.getCounts(id4), {3, 2, 1}));
        TS_ASSERT(compare_vect3(vect, counter2.getCounts(id5), {1, 3, 2}));

        TS_ASSERT(compare_vect3(vect, counter3.getCounts(id1), {1, 2, 3}));
        TS_ASSERT(compare_vect3(vect, counter3.getCounts(id2), {2, 1, 3}));
        TS_ASSERT(compare_vect3(vect, counter3.getCounts(id3), {3, 1, 2}));
        TS_ASSERT(compare_vect3(vect, counter3.getCounts(id4), {3, 2, 1}));
        TS_ASSERT(compare_vect3(vect, counter3.getCounts(id5), {1, 3, 2}));
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
