
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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


#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <ressources/include/myalloc.h>
#include <ressources/include/learningAlloc.h>
#include <iostream>

#include <agrum/tools/database/DBRow.h>
#include <agrum/tools/database/DBCell.h>

namespace gum_tests {

  class DBRowTestSuite: public CxxTest::TestSuite {
    public:
    void test_row1() {
      {
        gum::learning::DBRow< double > row;
        TS_ASSERT(row.weight() == 1.0f);

        gum::learning::DBRow< double > xrow1(3, 2.0);
        TS_ASSERT(xrow1.weight() == 2.0f);
        TS_ASSERT(xrow1.size() == std::size_t(3));

        gum::learning::DBRow< double > xrow2(3, 4, 2.0);
        TS_ASSERT(xrow2.weight() == 2.0f);
        TS_ASSERT(xrow2.size() == std::size_t(3));
        TS_ASSERT(xrow2[1] == 4.0f);

        row.setWeight(3);
        TS_ASSERT(row.weight() == 3.0f);

        std::vector< double > xrow{1, 2, 3};
        row.setRow(xrow);
        TS_ASSERT(row.row() == xrow);
        TS_ASSERT(row.size() == 3);

        gum::learning::DBRow< double > row2(row);
        TS_ASSERT(row2.size() == 3);

        std::allocator< double >       alloc;
        gum::learning::DBRow< double > row2bis(row, alloc);
        TS_ASSERT(row2bis.size() == 3);

        gum::learning::DBRow< double > row3(std::move(row2));
        TS_ASSERT(row3.size() == 3);
        TS_ASSERT(row3[2] == 3.0f);

        row3.resize(5);
        TS_ASSERT(row3.size() == 5);

        row2 = row3;
        TS_ASSERT(row2.size() == 5);
        TS_ASSERT(row3.row() == row2.row());

        gum::learning::DBRow< double, MyAlloc > row4(4, 1, 2);
        TS_ASSERT(row4.size() == 4);
        TS_ASSERT(row4[3] == 1);
        TS_ASSERT(row4.weight() == 2);

        gum::learning::DBRow< double, MyAlloc > row5{2, 3, 1, 4};
        TS_ASSERT(row5.size() == 4);
        TS_ASSERT(row5[2] == 1);
        TS_ASSERT(row5.weight() == 1);

        std::vector< double >                   vect{1, 2, 3};
        gum::learning::DBRow< double, MyAlloc > row6(vect, 4);
        TS_ASSERT(row6.size() == 3);
        TS_ASSERT(row6[1] == 2);
        TS_ASSERT(row6.weight() == 4);

        gum::learning::DBRow< double > row7(vect, 4);
        TS_ASSERT(row7.size() == 3);
        TS_ASSERT(row7[1] == 2);
        TS_ASSERT(row7.weight() == 4);

        gum::learning::DBRow< double > row8(std::vector< double >{1, 2, 3}, 4);
        TS_ASSERT(row8.size() == 3);
        TS_ASSERT(row8[1] == 2);
        TS_ASSERT(row8.weight() == 4);

        gum::learning::DBRow< double, MyAlloc > row9(row4, MyAlloc< double >());
        TS_ASSERT(row9.size() == 4);
        TS_ASSERT(row9[3] == 1);
        TS_ASSERT(row9.weight() == 2);

        row = row7;
        TS_ASSERT(row.size() == 3);
        TS_ASSERT(row[1] == 2);
        TS_ASSERT(row.weight() == 4);

        row9 = row6;
        TS_ASSERT(row9.size() == 3);
        TS_ASSERT(row9[1] == 2);
        TS_ASSERT(row9.weight() == 4);

        row6 = row9;
        TS_ASSERT(row6.size() == 3);
        TS_ASSERT(row6[1] == 2);
        TS_ASSERT(row6.weight() == 4);

        row2.resize(10);
        TS_ASSERT(row2.size() == 10);
      }

      TS_ASSERT(MyAllocCount::hasMeroryLeak() == false);
    }


    void test_row2() {
      {
        gum::learning::DBRow< gum::learning::DBCell > row;
        TS_ASSERT(row.weight() == 1.0f);

        gum::learning::DBRow< gum::learning::DBCell > xrow1(3, 2.0);
        TS_ASSERT(xrow1.weight() == 2.0f);
        TS_ASSERT(xrow1.size() == std::size_t(3));

        gum::learning::DBRow< gum::learning::DBCell > xrow2(
           3, gum::learning::DBCell(4), 2.0);
        TS_ASSERT(xrow2.weight() == 2.0f);
        TS_ASSERT(xrow2.size() == std::size_t(3));
        TS_ASSERT(xrow2[1].integer() == 4);

        row.setWeight(3);
        TS_ASSERT(row.weight() == 3.0f);

        std::vector< gum::learning::DBCell > xrow{gum::learning::DBCell(1),
                                                  gum::learning::DBCell(2),
                                                  gum::learning::DBCell(3)};
        row.setRow(xrow);
        TS_ASSERT(row.row() == xrow);
        TS_ASSERT(row.size() == 3);

        gum::learning::DBRow< gum::learning::DBCell > row2(row);
        TS_ASSERT(row2.size() == 3);

        gum::learning::DBRow< gum::learning::DBCell > row3(std::move(row2));
        TS_ASSERT(row3.size() == 3);
        TS_ASSERT(row3[2] == gum::learning::DBCell(3));

        row3.resize(5);
        TS_ASSERT(row3.size() == 5);

        row2 = row3;
        TS_ASSERT(row2.size() == 5);
        TS_ASSERT(row3.row() == row2.row());

        gum::learning::DBRow< gum::learning::DBCell, MyAlloc > row4(
           4, gum::learning::DBCell(1), 2);
        TS_ASSERT(row4.size() == 4);
        TS_ASSERT(row4[3] == gum::learning::DBCell(1));
        TS_ASSERT(row4.weight() == 2);

        gum::learning::DBRow< gum::learning::DBCell, MyAlloc > row5{
           gum::learning::DBCell(2),
           gum::learning::DBCell(3),
           gum::learning::DBCell(1),
           gum::learning::DBCell(4)};
        TS_ASSERT(row5.size() == 4);
        TS_ASSERT(row5[2] == gum::learning::DBCell(1));
        TS_ASSERT(row5.weight() == 1);

        std::vector< gum::learning::DBCell > vect{gum::learning::DBCell(1),
                                                  gum::learning::DBCell(2),
                                                  gum::learning::DBCell(3)};
        gum::learning::DBRow< gum::learning::DBCell, MyAlloc > row6(vect, 4);
        TS_ASSERT(row6.size() == 3);
        TS_ASSERT(row6[1] == gum::learning::DBCell(2));
        TS_ASSERT(row6.weight() == 4);

        gum::learning::DBRow< gum::learning::DBCell > row7(vect, 4);
        TS_ASSERT(row7.size() == 3);
        TS_ASSERT(row7[1] == gum::learning::DBCell(2));
        TS_ASSERT(row7.weight() == 4);

        gum::learning::DBRow< gum::learning::DBCell > row8(
           std::vector< gum::learning::DBCell >{gum::learning::DBCell(1),
                                                gum::learning::DBCell(2),
                                                gum::learning::DBCell(3)},
           4);
        TS_ASSERT(row8.size() == 3);
        TS_ASSERT(row8[1] == gum::learning::DBCell(2));
        TS_ASSERT(row8.weight() == 4);

        gum::learning::DBRow< gum::learning::DBCell, MyAlloc > row9(
           row6, MyAlloc< gum::learning::DBCell >());
        TS_ASSERT(row9.size() == 3);
        TS_ASSERT(row9[1] == gum::learning::DBCell(2));
        TS_ASSERT(row9.weight() == 4);

        row = row7;
        TS_ASSERT(row.size() == 3);
        TS_ASSERT(row[1] == gum::learning::DBCell(2));
        TS_ASSERT(row.weight() == 4);

        row7 = row8;
        TS_ASSERT(row7.size() == 3);
        TS_ASSERT(row7[1] == gum::learning::DBCell(2));
        TS_ASSERT(row7.weight() == 4);

        row2.resize(10);
        TS_ASSERT(row2.size() == 10);
      }

      TS_ASSERT(MyAllocCount::hasMeroryLeak() == false);
    }

    void test_row3() {
      LearningAlloc< gum::learning::DBCell > alloc(1000);

      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row1(
         4, gum::learning::DBCell(1), 2, alloc);
      TS_ASSERT(row1.size() == 4);
      TS_ASSERT(row1[3] == gum::learning::DBCell(1));
      TS_ASSERT(row1.weight() == 2);
      TS_ASSERT(alloc.allocatedSize() == 1);

      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row2(
         4, 2, alloc);
      TS_ASSERT(row2.size() == 4);
      TS_ASSERT(row2.weight() == 2);
      TS_ASSERT(alloc.allocatedSize() == 2);

      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row3(row2);
      TS_ASSERT(row3.size() == 4);
      TS_ASSERT(row3.weight() == 2);
      TS_ASSERT(alloc.allocatedSize() == 3);

      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row4(alloc);
      TS_ASSERT(alloc.allocatedSize() == 3);
      row4 = row3;
      TS_ASSERT(alloc.allocatedSize() == 4);

      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row5;
      TS_ASSERT(alloc.allocatedSize() == 4);

      gum::learning::DBRow< gum::learning::DBCell > row6(
         4, gum::learning::DBCell(1), 2);
      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row7(row4,
                                                                        alloc);
      TS_ASSERT(alloc.allocatedSize() == 5);
      TS_ASSERT(row7.size() == 4);
      TS_ASSERT(row7.weight() == 2);

      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row8(
         std::move(row7));
      TS_ASSERT(alloc.allocatedSize() == 6);
      TS_ASSERT(row8.size() == 4);
      TS_ASSERT(row8[3] == gum::learning::DBCell());
      TS_ASSERT(row8.weight() == 2);

      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row9(row8);
      TS_ASSERT(alloc.allocatedSize() == 7);

      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row10(
         {gum::learning::DBCell(1),
          gum::learning::DBCell(2),
          gum::learning::DBCell(3)},
         1.0,
         alloc);
      TS_ASSERT(alloc.allocatedSize() == 8);

      std::vector< gum::learning::DBCell > vect{gum::learning::DBCell(1),
                                                gum::learning::DBCell(2),
                                                gum::learning::DBCell(3)};
      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row11(
         vect, 1.0, alloc);
      TS_ASSERT(alloc.allocatedSize() == 9);

      std::vector< gum::learning::DBCell, LearningAlloc< gum::learning::DBCell > >
         vect2({gum::learning::DBCell(1),
                gum::learning::DBCell(2),
                gum::learning::DBCell(3)},
               alloc);
      TS_ASSERT(alloc.allocatedSize() == 10);
      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row12(
         std::move(vect2), 1.0, alloc);
      TS_ASSERT(alloc.allocatedSize() == 10);
    }
  };

} /* namespace gum_tests */