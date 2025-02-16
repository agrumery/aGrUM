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


#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/database/DBCell.h>
#include <agrum/base/database/DBRow.h>

namespace gum_tests {

  class [[maybe_unused]] DBRowTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_row1) {
      {
        gum::learning::DBRow< double > row;
        TS_ASSERT_EQUALS(row.weight(), 1.0f)

        gum::learning::DBRow< double > xrow1(3, 2.0);
        TS_ASSERT_EQUALS(xrow1.weight(), 2.0f)
        TS_ASSERT_EQUALS(xrow1.size(), (gum::Size)std::size_t(3))

        gum::learning::DBRow< double > xrow2(3, 4, 2.0);
        TS_ASSERT_EQUALS(xrow2.weight(), 2.0f)
        TS_ASSERT_EQUALS(xrow2.size(), (gum::Size)std::size_t(3))
        TS_ASSERT_EQUALS(xrow2[1], 4.0f)

        row.setWeight(3);
        TS_ASSERT_EQUALS(row.weight(), 3.0f)

        std::vector< double > xrow{1, 2, 3};
        row.setRow(xrow);
        TS_ASSERT_EQUALS(row.row(), xrow)
        TS_ASSERT_EQUALS(row.size(), (gum::Size)3)

        gum::learning::DBRow< double > row2(row);
        TS_ASSERT_EQUALS(row2.size(), (gum::Size)3)

        gum::learning::DBRow< double > row2bis(row);
        TS_ASSERT_EQUALS(row2bis.size(), (gum::Size)3)

        gum::learning::DBRow< double > row3(std::move(row2));
        TS_ASSERT_EQUALS(row3.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(row3[2], 3.0f)

        row3.resize(5);
        TS_ASSERT_EQUALS(row3.size(), (gum::Size)5)

        row2 = row3;
        TS_ASSERT_EQUALS(row2.size(), (gum::Size)5)
        TS_ASSERT_EQUALS(row3.row(), row2.row())

        gum::learning::DBRow< double > row4(4, 1, 2);
        TS_ASSERT_EQUALS(row4.size(), (gum::Size)4)
        TS_ASSERT_EQUALS(row4[3], 1)
        TS_ASSERT_EQUALS(row4.weight(), 2)

        gum::learning::DBRow< double > row5{2, 3, 1, 4};
        TS_ASSERT_EQUALS(row5.size(), (gum::Size)4)
        TS_ASSERT_EQUALS(row5[2], 1)
        TS_ASSERT_EQUALS(row5.weight(), 1)

        std::vector< double >          vect{1, 2, 3};
        gum::learning::DBRow< double > row6(vect, 4);
        TS_ASSERT_EQUALS(row6.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(row6[1], 2)
        TS_ASSERT_EQUALS(row6.weight(), 4)

        gum::learning::DBRow< double > row7(vect, 4);
        TS_ASSERT_EQUALS(row7.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(row7[1], 2)
        TS_ASSERT_EQUALS(row7.weight(), 4)

        gum::learning::DBRow< double > row8(std::vector< double >{1, 2, 3}, 4);
        TS_ASSERT_EQUALS(row8.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(row8[1], 2)
        TS_ASSERT_EQUALS(row8.weight(), 4)

        gum::learning::DBRow< double > row9(row4);
        TS_ASSERT_EQUALS(row9.size(), (gum::Size)4)
        TS_ASSERT_EQUALS(row9[3], 1)
        TS_ASSERT_EQUALS(row9.weight(), 2)

        row = row7;
        TS_ASSERT_EQUALS(row.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(row[1], 2)
        TS_ASSERT_EQUALS(row.weight(), 4)

        row9 = row6;
        TS_ASSERT_EQUALS(row9.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(row9[1], 2)
        TS_ASSERT_EQUALS(row9.weight(), 4)

        row6 = row9;
        TS_ASSERT_EQUALS(row6.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(row6[1], 2)
        TS_ASSERT_EQUALS(row6.weight(), 4)

        row2.resize(10);
        TS_ASSERT_EQUALS(row2.size(), (gum::Size)10)
      }
    }

    GUM_ACTIVE_TEST(_row2) {
      {
        gum::learning::DBRow< gum::learning::DBCell > row;
        TS_ASSERT_EQUALS(row.weight(), 1.0f)

        gum::learning::DBRow< gum::learning::DBCell > xrow1(3, 2.0);
        TS_ASSERT_EQUALS(xrow1.weight(), 2.0f)
        TS_ASSERT_EQUALS(xrow1.size(), (gum::Size)std::size_t(3))

        gum::learning::DBRow< gum::learning::DBCell > xrow2(3, gum::learning::DBCell(4), 2.0);
        TS_ASSERT_EQUALS(xrow2.weight(), 2.0f)
        TS_ASSERT_EQUALS(xrow2.size(), (gum::Size)std::size_t(3))
        TS_ASSERT_EQUALS(xrow2[1].integer(), 4)

        row.setWeight(3);
        TS_ASSERT_EQUALS(row.weight(), 3.0f)

        std::vector< gum::learning::DBCell > xrow{gum::learning::DBCell(1),
                                                  gum::learning::DBCell(2),
                                                  gum::learning::DBCell(3)};
        row.setRow(xrow);
        TS_ASSERT_EQUALS(row.row(), xrow)
        TS_ASSERT_EQUALS(row.size(), (gum::Size)3)

        gum::learning::DBRow< gum::learning::DBCell > row2(row);
        TS_ASSERT_EQUALS(row2.size(), (gum::Size)3)

        gum::learning::DBRow< gum::learning::DBCell > row3(std::move(row2));
        TS_ASSERT_EQUALS(row3.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(row3[2], gum::learning::DBCell(3))

        row3.resize(5);
        TS_ASSERT_EQUALS(row3.size(), (gum::Size)5)

        row2 = row3;
        TS_ASSERT_EQUALS(row2.size(), (gum::Size)5)
        TS_ASSERT_EQUALS(row3.row(), row2.row())

        gum::learning::DBRow< gum::learning::DBCell > row4(4, gum::learning::DBCell(1), 2);
        TS_ASSERT_EQUALS(row4.size(), (gum::Size)4)
        TS_ASSERT_EQUALS(row4[3], gum::learning::DBCell(1))
        TS_ASSERT_EQUALS(row4.weight(), 2)

        gum::learning::DBRow< gum::learning::DBCell > row5{gum::learning::DBCell(2),
                                                           gum::learning::DBCell(3),
                                                           gum::learning::DBCell(1),
                                                           gum::learning::DBCell(4)};
        TS_ASSERT_EQUALS(row5.size(), (gum::Size)4)
        TS_ASSERT_EQUALS(row5[2], gum::learning::DBCell(1))
        TS_ASSERT_EQUALS(row5.weight(), 1)

        std::vector< gum::learning::DBCell >          vect{gum::learning::DBCell(1),
                                                  gum::learning::DBCell(2),
                                                  gum::learning::DBCell(3)};
        gum::learning::DBRow< gum::learning::DBCell > row6(vect, 4);
        TS_ASSERT_EQUALS(row6.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(row6[1], gum::learning::DBCell(2))
        TS_ASSERT_EQUALS(row6.weight(), 4)

        gum::learning::DBRow< gum::learning::DBCell > row7(vect, 4);
        TS_ASSERT_EQUALS(row7.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(row7[1], gum::learning::DBCell(2))
        TS_ASSERT_EQUALS(row7.weight(), 4)

        gum::learning::DBRow< gum::learning::DBCell > row8(
            std::vector< gum::learning::DBCell >{gum::learning::DBCell(1),
                                                 gum::learning::DBCell(2),
                                                 gum::learning::DBCell(3)},
            4);
        TS_ASSERT_EQUALS(row8.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(row8[1], gum::learning::DBCell(2))
        TS_ASSERT_EQUALS(row8.weight(), 4)

        gum::learning::DBRow< gum::learning::DBCell > row9(row6);
        TS_ASSERT_EQUALS(row9.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(row9[1], gum::learning::DBCell(2))
        TS_ASSERT_EQUALS(row9.weight(), 4)

        row = row7;
        TS_ASSERT_EQUALS(row.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(row[1], gum::learning::DBCell(2))
        TS_ASSERT_EQUALS(row.weight(), 4)

        row7 = row8;
        TS_ASSERT_EQUALS(row7.size(), (gum::Size)3)
        TS_ASSERT_EQUALS(row7[1], gum::learning::DBCell(2))
        TS_ASSERT_EQUALS(row7.weight(), 4)

        row2.resize(10);
        TS_ASSERT_EQUALS(row2.size(), (gum::Size)10)
      }
    }

    GUM_ACTIVE_TEST(_row3) {
      gum::learning::DBRow< gum::learning::DBCell > row1(4, gum::learning::DBCell(1), 2);
      TS_ASSERT_EQUALS(row1.size(), (gum::Size)4)
      TS_ASSERT_EQUALS(row1[3], gum::learning::DBCell(1))
      TS_ASSERT_EQUALS(row1.weight(), 2)

      gum::learning::DBRow< gum::learning::DBCell > row2(4, 2);
      TS_ASSERT_EQUALS(row2.size(), (gum::Size)4)
      TS_ASSERT_EQUALS(row2.weight(), 2)

      gum::learning::DBRow< gum::learning::DBCell > row3(row2);
      TS_ASSERT_EQUALS(row3.size(), (gum::Size)4)
      TS_ASSERT_EQUALS(row3.weight(), 2)

      gum::learning::DBRow< gum::learning::DBCell > row4;
      row4 = row3;

      gum::learning::DBRow< gum::learning::DBCell > row5;

      gum::learning::DBRow< gum::learning::DBCell > row6(4, gum::learning::DBCell(1), 2);
      gum::learning::DBRow< gum::learning::DBCell > row7(row4);
      TS_ASSERT_EQUALS(row7.size(), (gum::Size)4)
      TS_ASSERT_EQUALS(row7.weight(), 2)

      gum::learning::DBRow< gum::learning::DBCell > row8(std::move(row7));
      TS_ASSERT_EQUALS(row8.size(), (gum::Size)4)
      TS_ASSERT_EQUALS(row8[3], gum::learning::DBCell())
      TS_ASSERT_EQUALS(row8.weight(), 2)

      gum::learning::DBRow< gum::learning::DBCell > row9(row8);

      gum::learning::DBRow< gum::learning::DBCell > row10(
          {gum::learning::DBCell(1), gum::learning::DBCell(2), gum::learning::DBCell(3)},
          1.0);

      std::vector< gum::learning::DBCell >          vect{gum::learning::DBCell(1),
                                                gum::learning::DBCell(2),
                                                gum::learning::DBCell(3)};
      gum::learning::DBRow< gum::learning::DBCell > row11(vect, 1.0);

      std::vector< gum::learning::DBCell > vect2(
          {gum::learning::DBCell(1), gum::learning::DBCell(2), gum::learning::DBCell(3)});
      gum::learning::DBRow< gum::learning::DBCell > row12(std::move(vect2), 1.0);
    }
  };

} /* namespace gum_tests */
