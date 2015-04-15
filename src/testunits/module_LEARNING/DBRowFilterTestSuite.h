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

#include <agrum/graphs/DAG.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/learning/database/databaseFromCSV.h>
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>
#include <agrum/learning/database/DBCellTranslator.h>
#include <agrum/learning/database/DBRowTranslatorSetStatic.h>
#include <agrum/learning/database/DBRowTranslatorSetDynamic.h>
#include <agrum/learning/database/CSVParser.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.h>
#include <agrum/learning/scores_and_tests/scoreK2.h>

namespace gum_tests {

  class TruncatedGenerator : public gum::learning::FilteredRowGenerator {
    public:
    gum::learning::FilteredRow &generate() {
      decreaseRemainingRows();
      return *_input_row;
    }
    inline unsigned int _computeRows() {
      if (nb) {
        --nb;
        return 1;
      } else
        return 0;
    }

    unsigned int nb{9500};
  };

  class ThreeGenerator : public gum::learning::FilteredRowGenerator {
    public:
    gum::learning::FilteredRow &generate() {
      decreaseRemainingRows();
      return *_input_row;
    }
    inline unsigned int _computeRows() { return 3; }
  };

  class TwoGenerator : public gum::learning::FilteredRowGenerator {
    public:
    gum::learning::FilteredRow &generate() {
      decreaseRemainingRows();
      return *_input_row;
    }
    inline unsigned int _computeRows() { return 2; }
  };

  class DBRowTestSuite : public CxxTest::TestSuite {
    public:
    void test_asia1() {
      gum::learning::DatabaseFromCSV database(GET_PATH_STR("asia.csv"));

      auto translators = gum::learning::make_translators(gum::learning::Create<
          gum::learning::CellTranslatorCompactIntId, gum::learning::Col<0>, 8>());

      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());

      auto filter1 =
          gum::learning::make_DB_row_filter(database, translators, generators);

      unsigned nb1 = 0;
      while (filter1.hasRows()) {
        ++nb1;
        filter1.row();
      }
      TS_ASSERT(nb1 == 10000);

      auto filter2 = filter1;
      TS_ASSERT(filter1.hasRows() == false);
      TS_ASSERT(filter2.hasRows() == false);

      filter1.reset();
      filter2.reset();

      nb1 = 0;
      while (filter1.hasRows() && filter2.hasRows()) {
        gum::learning::FilteredRow &row1 = filter1.row();
        gum::learning::FilteredRow &row2 = filter2.row();
        TS_ASSERT(row1 == row2);
        ++nb1;
      }
      TS_ASSERT(nb1 == 10000);

      filter1.reset();
      filter2.reset();
      auto filter3(std::move(filter2));
      nb1 = 0;
      while (filter1.hasRows() && filter3.hasRows()) {
        gum::learning::FilteredRow &row1 = filter1.row();
        gum::learning::FilteredRow &row3 = filter3.row();
        TS_ASSERT(row1 == row3);
        ++nb1;
      }
      TS_ASSERT(nb1 == 10000);
    }

    void test_asia2() {
      gum::learning::DatabaseFromCSV database(GET_PATH_STR("asia.csv"));

      auto translators = gum::learning::make_translators(gum::learning::Create<
          gum::learning::CellTranslatorCompactIntId, gum::learning::Col<0>, 8>());

      auto generators = gum::learning::make_generators(
          TruncatedGenerator(), gum::learning::RowGeneratorIdentity());

      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      unsigned nb1 = 0;
      try {
        while (filter.hasRows()) {
          filter.row();
          ++nb1;
        }
      } catch (gum::NotFound &) {
      }

      TS_ASSERT(nb1 == 9500);
    }

    void test_asia3() {
      gum::learning::DatabaseFromCSV database(GET_PATH_STR("asia.csv"));

      auto translators = gum::learning::make_translators(gum::learning::Create<
          gum::learning::CellTranslatorCompactIntId, gum::learning::Col<0>, 8>());

      auto generators = gum::learning::make_generators(
          ThreeGenerator(), TruncatedGenerator(), TwoGenerator());

      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      unsigned nb1 = 0;
      try {
        while (filter.hasRows()) {
          filter.row();
          ++nb1;
        }
      } catch (gum::NotFound &) {
      }

      TS_ASSERT(nb1 == 19000);
    }
  };

} /* namespace gum_tests */
