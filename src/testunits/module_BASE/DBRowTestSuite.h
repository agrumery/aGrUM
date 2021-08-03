/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>
#include <ressources/include/countedAlloc.h>
#include <ressources/include/poolAlloc.h>
#include <iostream>

#include <agrum/tools/database/DBRow.h>
#include <agrum/tools/database/DBCell.h>

namespace gum_tests {

  class DBRowTestSuite: public CxxTest::TestSuite {
    public:
    void test_row1() {
      {
        gum::learning::DBRow< double > row;
        TS_ASSERT_EQUALS(row.weight(), 1.0f);

        gum::learning::DBRow< double > xrow1(3, 2.0);
        TS_ASSERT_EQUALS(xrow1.weight(), 2.0f);
        TS_ASSERT_EQUALS(xrow1.size(), (gum::Size)std::size_t(3));

        gum::learning::DBRow< double > xrow2(3, 4, 2.0);
        TS_ASSERT_EQUALS(xrow2.weight(), 2.0f);
        TS_ASSERT_EQUALS(xrow2.size(), (gum::Size)std::size_t(3));
        TS_ASSERT_EQUALS(xrow2[1], 4.0f);

        row.setWeight(3);
        TS_ASSERT_EQUALS(row.weight(), 3.0f);

        std::vector< double > xrow{1, 2, 3};
        row.setRow(xrow);
        TS_ASSERT_EQUALS(row.row(), xrow);
        TS_ASSERT_EQUALS(row.size(), (gum::Size)3);

        gum::learning::DBRow< double > row2(row);
        TS_ASSERT_EQUALS(row2.size(), (gum::Size)3);

        std::allocator< double >       alloc;
        gum::learning::DBRow< double > row2bis(row, alloc);
        TS_ASSERT_EQUALS(row2bis.size(), (gum::Size)3);

        gum::learning::DBRow< double > row3(std::move(row2));
        TS_ASSERT_EQUALS(row3.size(), (gum::Size)3);
        TS_ASSERT_EQUALS(row3[2], 3.0f);

        row3.resize(5);
        TS_ASSERT_EQUALS(row3.size(), (gum::Size)5);

        row2 = row3;
        TS_ASSERT_EQUALS(row2.size(), (gum::Size)5);
        TS_ASSERT_EQUALS(row3.row(), row2.row());

        gum::learning::DBRow< double, DebugCountedAlloc > row4(4, 1, 2);
        TS_ASSERT_EQUALS(row4.size(), (gum::Size)4);
        TS_ASSERT_EQUALS(row4[3], 1);
        TS_ASSERT_EQUALS(row4.weight(), 2);

        gum::learning::DBRow< double, DebugCountedAlloc > row5{2, 3, 1, 4};
        TS_ASSERT_EQUALS(row5.size(), (gum::Size)4);
        TS_ASSERT_EQUALS(row5[2], 1);
        TS_ASSERT_EQUALS(row5.weight(), 1);

        std::vector< double >                             vect{1, 2, 3};
        gum::learning::DBRow< double, DebugCountedAlloc > row6(vect, 4);
        TS_ASSERT_EQUALS(row6.size(), (gum::Size)3);
        TS_ASSERT_EQUALS(row6[1], 2);
        TS_ASSERT_EQUALS(row6.weight(), 4);

        gum::learning::DBRow< double > row7(vect, 4);
        TS_ASSERT_EQUALS(row7.size(), (gum::Size)3);
        TS_ASSERT_EQUALS(row7[1], 2);
        TS_ASSERT_EQUALS(row7.weight(), 4);

        gum::learning::DBRow< double > row8(std::vector< double >{1, 2, 3}, 4);
        TS_ASSERT_EQUALS(row8.size(), (gum::Size)3);
        TS_ASSERT_EQUALS(row8[1], 2);
        TS_ASSERT_EQUALS(row8.weight(), 4);

        gum::learning::DBRow< double, DebugCountedAlloc > row9(row4, DebugCountedAlloc< double >());
        TS_ASSERT_EQUALS(row9.size(), (gum::Size)4);
        TS_ASSERT_EQUALS(row9[3], 1);
        TS_ASSERT_EQUALS(row9.weight(), 2);

        row = row7;
        TS_ASSERT_EQUALS(row.size(), (gum::Size)3);
        TS_ASSERT_EQUALS(row[1], 2);
        TS_ASSERT_EQUALS(row.weight(), 4);

        row9 = row6;
        TS_ASSERT_EQUALS(row9.size(), (gum::Size)3);
        TS_ASSERT_EQUALS(row9[1], 2);
        TS_ASSERT_EQUALS(row9.weight(), 4);

        row6 = row9;
        TS_ASSERT_EQUALS(row6.size(), (gum::Size)3);
        TS_ASSERT_EQUALS(row6[1], 2);
        TS_ASSERT_EQUALS(row6.weight(), 4);

        row2.resize(10);
        TS_ASSERT_EQUALS(row2.size(), (gum::Size)10);
      }

      TS_ASSERT_EQUALS(CountedAlloc::hasMemoryLeak(), false);
    }


    void test_row2() {
      {
        gum::learning::DBRow< gum::learning::DBCell > row;
        TS_ASSERT_EQUALS(row.weight(), 1.0f);

        gum::learning::DBRow< gum::learning::DBCell > xrow1(3, 2.0);
        TS_ASSERT_EQUALS(xrow1.weight(), 2.0f);
        TS_ASSERT_EQUALS(xrow1.size(), (gum::Size)std::size_t(3));

        gum::learning::DBRow< gum::learning::DBCell > xrow2(3, gum::learning::DBCell(4), 2.0);
        TS_ASSERT_EQUALS(xrow2.weight(), 2.0f);
        TS_ASSERT_EQUALS(xrow2.size(), (gum::Size)std::size_t(3));
        TS_ASSERT_EQUALS(xrow2[1].integer(), 4);

        row.setWeight(3);
        TS_ASSERT_EQUALS(row.weight(), 3.0f);

        std::vector< gum::learning::DBCell > xrow{gum::learning::DBCell(1),
                                                  gum::learning::DBCell(2),
                                                  gum::learning::DBCell(3)};
        row.setRow(xrow);
        TS_ASSERT_EQUALS(row.row(), xrow);
        TS_ASSERT_EQUALS(row.size(), (gum::Size)3);

        gum::learning::DBRow< gum::learning::DBCell > row2(row);
        TS_ASSERT_EQUALS(row2.size(), (gum::Size)3);

        gum::learning::DBRow< gum::learning::DBCell > row3(std::move(row2));
        TS_ASSERT_EQUALS(row3.size(), (gum::Size)3);
        TS_ASSERT_EQUALS(row3[2], gum::learning::DBCell(3));

        row3.resize(5);
        TS_ASSERT_EQUALS(row3.size(), (gum::Size)5);

        row2 = row3;
        TS_ASSERT_EQUALS(row2.size(), (gum::Size)5);
        TS_ASSERT_EQUALS(row3.row(), row2.row());

        gum::learning::DBRow< gum::learning::DBCell, DebugCountedAlloc > row4(
           4,
           gum::learning::DBCell(1),
           2);
        TS_ASSERT_EQUALS(row4.size(), (gum::Size)4);
        TS_ASSERT_EQUALS(row4[3], gum::learning::DBCell(1));
        TS_ASSERT_EQUALS(row4.weight(), 2);

        gum::learning::DBRow< gum::learning::DBCell, DebugCountedAlloc > row5{
           gum::learning::DBCell(2),
           gum::learning::DBCell(3),
           gum::learning::DBCell(1),
           gum::learning::DBCell(4)};
        TS_ASSERT_EQUALS(row5.size(), (gum::Size)4);
        TS_ASSERT_EQUALS(row5[2], gum::learning::DBCell(1));
        TS_ASSERT_EQUALS(row5.weight(), 1);

        std::vector< gum::learning::DBCell > vect{gum::learning::DBCell(1),
                                                  gum::learning::DBCell(2),
                                                  gum::learning::DBCell(3)};
        gum::learning::DBRow< gum::learning::DBCell, DebugCountedAlloc > row6(vect, 4);
        TS_ASSERT_EQUALS(row6.size(), (gum::Size)3);
        TS_ASSERT_EQUALS(row6[1], gum::learning::DBCell(2));
        TS_ASSERT_EQUALS(row6.weight(), 4);

        gum::learning::DBRow< gum::learning::DBCell > row7(vect, 4);
        TS_ASSERT_EQUALS(row7.size(), (gum::Size)3);
        TS_ASSERT_EQUALS(row7[1], gum::learning::DBCell(2));
        TS_ASSERT_EQUALS(row7.weight(), 4);

        gum::learning::DBRow< gum::learning::DBCell > row8(
           std::vector< gum::learning::DBCell >{gum::learning::DBCell(1),
                                                gum::learning::DBCell(2),
                                                gum::learning::DBCell(3)},
           4);
        TS_ASSERT_EQUALS(row8.size(), (gum::Size)3);
        TS_ASSERT_EQUALS(row8[1], gum::learning::DBCell(2));
        TS_ASSERT_EQUALS(row8.weight(), 4);

        gum::learning::DBRow< gum::learning::DBCell, DebugCountedAlloc > row9(
           row6,
           DebugCountedAlloc< gum::learning::DBCell >());
        TS_ASSERT_EQUALS(row9.size(), (gum::Size)3);
        TS_ASSERT_EQUALS(row9[1], gum::learning::DBCell(2));
        TS_ASSERT_EQUALS(row9.weight(), 4);

        row = row7;
        TS_ASSERT_EQUALS(row.size(), (gum::Size)3);
        TS_ASSERT_EQUALS(row[1], gum::learning::DBCell(2));
        TS_ASSERT_EQUALS(row.weight(), 4);

        row7 = row8;
        TS_ASSERT_EQUALS(row7.size(), (gum::Size)3);
        TS_ASSERT_EQUALS(row7[1], gum::learning::DBCell(2));
        TS_ASSERT_EQUALS(row7.weight(), 4);

        row2.resize(10);
        TS_ASSERT_EQUALS(row2.size(), (gum::Size)10);
      }

      TS_ASSERT_EQUALS(CountedAlloc::hasMemoryLeak(), false);
    }

    void test_row3() {
      LearningAlloc< gum::learning::DBCell > alloc(1000);

      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row1(4,
                                                                        gum::learning::DBCell(1),
                                                                        2,
                                                                        alloc);
      TS_ASSERT_EQUALS(row1.size(), (gum::Size)4);
      TS_ASSERT_EQUALS(row1[3], gum::learning::DBCell(1));
      TS_ASSERT_EQUALS(row1.weight(), 2);
      TS_ASSERT_EQUALS(alloc.allocatedSize(), (gum::Size)1);

      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row2(4, 2, alloc);
      TS_ASSERT_EQUALS(row2.size(), (gum::Size)4);
      TS_ASSERT_EQUALS(row2.weight(), 2);
      TS_ASSERT_EQUALS(alloc.allocatedSize(), (gum::Size)2);

      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row3(row2);
      TS_ASSERT_EQUALS(row3.size(), (gum::Size)4);
      TS_ASSERT_EQUALS(row3.weight(), 2);
      TS_ASSERT_EQUALS(alloc.allocatedSize(), (gum::Size)3);

      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row4(alloc);
      TS_ASSERT_EQUALS(alloc.allocatedSize(), (gum::Size)3);
      row4 = row3;
      TS_ASSERT_EQUALS(alloc.allocatedSize(), (gum::Size)4);

      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row5;
      TS_ASSERT_EQUALS(alloc.allocatedSize(), (gum::Size)4);

      gum::learning::DBRow< gum::learning::DBCell > row6(4, gum::learning::DBCell(1), 2);
      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row7(row4, alloc);
      TS_ASSERT_EQUALS(alloc.allocatedSize(), (gum::Size)5);
      TS_ASSERT_EQUALS(row7.size(), (gum::Size)4);
      TS_ASSERT_EQUALS(row7.weight(), 2);

      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row8(std::move(row7));
      TS_ASSERT_EQUALS(alloc.allocatedSize(), (gum::Size)6);
      TS_ASSERT_EQUALS(row8.size(), (gum::Size)4);
      TS_ASSERT_EQUALS(row8[3], gum::learning::DBCell());
      TS_ASSERT_EQUALS(row8.weight(), 2);

      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row9(row8);
      TS_ASSERT_EQUALS(alloc.allocatedSize(), (gum::Size)7);

      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row10(
         {gum::learning::DBCell(1), gum::learning::DBCell(2), gum::learning::DBCell(3)},
         1.0,
         alloc);
      TS_ASSERT_EQUALS(alloc.allocatedSize(), (gum::Size)8);

      std::vector< gum::learning::DBCell >                         vect{gum::learning::DBCell(1),
                                                gum::learning::DBCell(2),
                                                gum::learning::DBCell(3)};
      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row11(vect, 1.0, alloc);
      TS_ASSERT_EQUALS(alloc.allocatedSize(), (gum::Size)9);

      std::vector< gum::learning::DBCell, LearningAlloc< gum::learning::DBCell > > vect2(
         {gum::learning::DBCell(1), gum::learning::DBCell(2), gum::learning::DBCell(3)},
         alloc);
      TS_ASSERT_EQUALS(alloc.allocatedSize(), (gum::Size)10);
      gum::learning::DBRow< gum::learning::DBCell, LearningAlloc > row12(std::move(vect2),
                                                                         1.0,
                                                                         alloc);
      TS_ASSERT_EQUALS(alloc.allocatedSize(), (gum::Size)10);
    }
  };

} /* namespace gum_tests */
