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

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.h>
#include <agrum/learning/database/databaseFromCSV.h>
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>
#include <agrum/learning/scores_and_tests/counter.h>

namespace gum_tests {

  class CounterTestSuite : public CxxTest::TestSuite {
    public:
    struct MyCounter : public gum::learning::Counter<> {
      template < typename RowFilter >
      MyCounter(const RowFilter&               filter,
                const std::vector< gum::Idx >& var_modalities)
          : gum::learning::Counter<>(filter, var_modalities) {}

      const std::vector< double >& getAllCounts(gum::Idx index) {
        return gum::learning::Counter<>::_getAllCounts(index);
      }

      const std::vector< double >& getConditioningCounts(gum::Idx index) {
        return gum::learning::Counter<>::_getConditioningCounts(index);
      }

      std::vector< std::vector< double > >& getCounts() noexcept {
        return gum::learning::Counter<>::_getCounts();
      }

      const std::vector< gum::Idx >& getAllNodes(gum::Idx index) const noexcept {
        return gum::learning::Counter<>::_getAllNodes(index);
      }

      const std::vector< std::pair< std::vector< gum::Idx >, gum::Idx >* >&
      getAllNodes() const noexcept {
        return gum::learning::Counter<>::_getAllNodes();
      }

      /// returns the conditioning nodes (nullptr if there are no such nodes)
      const std::vector< gum::Idx >* getConditioningNodes(gum::Idx index) const
        noexcept {
        return gum::learning::Counter<>::_getConditioningNodes(index);
      }

      const std::vector< std::pair< std::vector< gum::Idx >, gum::Idx >* >&
      getConditioningNodes() const noexcept {
        return gum::learning::Counter<>::_getConditioningNodes();
      }
    };

    void test1() {
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("asia.csv"));

      gum::learning::DBRowTranslatorSet<
        gum::learning::CellTranslatorCompactIntId >
        translators;
      translators.insertTranslator(0, 8);

      gum::learning::FilteredRowGeneratorSet< gum::learning::RowGeneratorIdentity >
        generators;
      generators.insertGenerator();

      auto filter =
        gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector< gum::Size > modalities(8, 2);

      MyCounter counter(filter, modalities);

      std::vector< gum::Idx > set1{0};
      std::vector< gum::Idx > set2{1};
      std::vector< gum::Idx > set3{1, 0};

      {
        gum::Idx id1 = counter.addNodeSet(0);
        gum::Idx id2 = counter.addNodeSet(1);
        gum::Idx id3 = counter.addNodeSet(0, set2);

        const std::vector< double >& vect1 = counter.getAllCounts(id1);
        const std::vector< double >& vect2 = counter.getAllCounts(id2);
        const std::vector< double >& vect3 = counter.getAllCounts(id3);

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
      std::vector< gum::Idx > set4{1, 2, 3};
      {
        counter.addNodeSet(0, set2);
        gum::Idx id2 = counter.addNodeSet(0, set4);
        counter.addNodeSet(2, set1);
        gum::Idx id4 = counter.addNodeSet(0, set4);

        TS_ASSERT(counter.getAllCounts(id2) == counter.getAllCounts(id4));
        TS_ASSERT(counter.getConditioningCounts(id2) ==
                  counter.getConditioningCounts(id4));
      }

      counter.clear();
      gum::Idx              id = counter.addNodeSet(0, set4);
      std::vector< double > vectx = counter.getAllCounts(id);
      std::vector< double > vecty = counter.getConditioningCounts(id);

      counter.clear();
      {
        std::vector< gum::Idx > set5{2, 1, 3};
        std::vector< gum::Idx > set6{3, 1, 2};
        std::vector< gum::Idx > set7{3, 2, 1};
        std::vector< gum::Idx > set8{1, 3, 2};
        gum::Idx                id1 = counter.addNodeSet(0, set4);
        counter.addEmptyNodeSet();
        gum::Idx id2 = counter.addNodeSet(0, set5);
        gum::Idx id3 = counter.addNodeSet(0, set6);
        counter.addEmptyNodeSet();
        counter.addEmptyNodeSet();
        gum::Idx id4 = counter.addNodeSet(0, set7);
        gum::Idx id5 = counter.addNodeSet(0, set8);

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
        std::vector< gum::Idx > set5{2, 1, 3};
        std::vector< gum::Idx > set6{3, 1, 2};
        std::vector< gum::Idx > set7{3, 2, 1};
        std::vector< gum::Idx > set8{1, 3, 2};
        gum::Idx                id1 = counter.addNodeSet(0, set4);
        counter.addEmptyNodeSet();
        gum::Idx id2 = counter.addNodeSet(0, set5);
        gum::Idx id3 = counter.addNodeSet(0, set6);
        counter.addEmptyNodeSet();
        counter.addEmptyNodeSet();
        gum::Idx id4 = counter.addNodeSet(0, set7);
        gum::Idx id5 = counter.addNodeSet(0, set8);

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
        std::vector< gum::Idx > set5{2, 1, 3};
        std::vector< gum::Idx > set6{3, 1, 2};
        std::vector< gum::Idx > set7{3, 2, 1};
        std::vector< gum::Idx > set8{1, 3, 2};
        gum::Idx                id1 = counter.addNodeSet(0, set4);
        counter.addEmptyNodeSet();
        gum::Idx id2 = counter.addNodeSet(0, set5);
        gum::Idx id3 = counter.addNodeSet(0, set6);
        counter.addEmptyNodeSet();
        counter.addEmptyNodeSet();
        gum::Idx id4 = counter.addNodeSet(0, set7);
        gum::Idx id5 = counter.addNodeSet(0, set8);

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

    bool compare_vect3(const std::vector< double >&   v1,
                       const std::vector< double >&   v2,
                       const std::vector< gum::Idx >& order) {
      gum::Idx index1, index2;
      for (gum::Idx k = 0; k < 2; ++k) {
        for (gum::Idx j = 0; j < 2; ++j) {
          for (gum::Idx i = 0; i < 2; ++i) {
            index2 = i + j * 2 + k * 4;
            index1 = i * (1 << (order[0] - 1)) + j * (1 << (order[1] - 1)) +
                     k * (1 << (order[2] - 1));
            if (v1[index1] != v2[index2]) return false;
          }
        }
      }

      return true;
    }

    bool compare_vect2(const std::vector< double >&   v1,
                       const std::vector< double >&   v2,
                       const std::vector< gum::Idx >& order) {
      gum::Idx index1, index2;
      for (gum::Idx j = 0; j < 2; ++j) {
        for (gum::Idx i = 0; i < 2; ++i) {
          index2 = i + j * 2;
          index1 = i * (1 << (order[0] - 1)) + j * (1 << (order[1] - 1));
          if (v1[index1] != v2[index2]) return false;
        }
      }

      return true;
    }
  };

} /* namespace gum_tests */
