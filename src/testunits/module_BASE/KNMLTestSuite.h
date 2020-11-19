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


#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/tools/database/DBTranslator4LabelizedVariable.h>
#include <agrum/tools/database/DBRowGeneratorParser.h>
#include <agrum/tools/database/DBInitializerFromCSV.h>
#include <agrum/tools/database/databaseTable.h>
#include <agrum/tools/database/DBTranslatorSet.h>
#include <agrum/BN/learning/aprioris/aprioriNoApriori.h>
#include <agrum/tools/stattests/kNML.h>

namespace gum_tests {

  class KNMLTestSuite: public CxxTest::TestSuite {
    public:
    void test1() {
      gum::learning::DBInitializerFromCSV<> initializer(
         GET_RESSOURCES_PATH("csv/asia.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars = var_names.size();

      gum::learning::DBTranslatorSet<>                translator_set;
      gum::learning::DBTranslator4LabelizedVariable<> translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable<> database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);

      // std::vector< gum::Size > modalities(nb_vars, 2);
      gum::learning::AprioriNoApriori<> apriori(database);

      gum::learning::KNML<> score(parser, apriori);
      /* 3-4 K 2.6844818514806183
       * 2-6 K 4.1414644088786756
       * 4-7|5 K 3.763846399915938
       */
      // gum::Idx id1 = score.addNodeSet(3, 4);
      // gum::Idx id2 = score.addNodeSet(2, 6);
      // gum::Idx id3 = score.addNodeSet(4, 7, std::vector< gum::Idx >{5});
      TS_ASSERT_DELTA(score.score(3, 4), 3.87288, 1e-2);
      TS_ASSERT_DELTA(score.score(2, 6), 5.97477, 1e-2);
      TS_ASSERT_DELTA(
         score.score(4, 7, std::vector< gum::NodeId >{5}), 5.43007, 1e-2);

      score.clear();
      // id1 = score.addNodeSet(6, 7, std::vector< gum::Idx >{5, 1, 4});
      // id2 = score.addNodeSet(6, 7, std::vector< gum::Idx >{1, 4});
      TS_ASSERT_DELTA(score.score(6, 7, std::vector< gum::NodeId >{5, 1, 4})
                         - score.score(6, 7, std::vector< gum::NodeId >{1, 4}),
                      0.0,
                      1e-2);
    }
  };

} /* namespace gum_tests */
