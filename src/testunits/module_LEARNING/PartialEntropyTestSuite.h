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
 *   59 Temple Place, Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBRowGeneratorParser.h>
#include <agrum/learning/database/DBInitializerFromCSV.h>
#include <agrum/learning/database/databaseTable.h>
#include <agrum/learning/database/DBTranslatorSet.h>
#include <agrum/learning/scores_and_tests/partialEntropy.h>

namespace gum_tests {

  class PartialEntropyTestSuite : public CxxTest::TestSuite {
    public:
    void test_entropy() {
      gum::learning::DBInitializerFromCSV<> initializer(
        GET_RESSOURCES_PATH("asia.csv"));
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

      std::vector< gum::Size > modalities(nb_vars, 2);

      gum::learning::PartialEntropy<> score(parser, modalities);
      /*
       * H de [3, 4] vaut 0.13457707360460336
       * H de [4, 0] vaut 0.759802701155244
       * H de [1, 2, 5, 6] vaut 1.366351889386632
       * H1 de [3] vaut 0.06850409760350079
       * H de [0, 1, 4, 6] vaut 1.5920762480353452
       * H de [0, 1, 4, 6, 7] vaut 1.785332833196645
       */
      gum::Idx id1 = score.addNodeSet(3);
      gum::Idx id2 = score.addNodeSet(std::vector< gum::Idx >{1, 2, 5, 6});
      gum::Idx id3 = score.addNodeSet(std::vector< gum::Idx >{3, 4});
      gum::Idx id4 = score.addNodeSet(3, 4);
      gum::Idx id5 = score.addNodeSet(std::vector< gum::Idx >{3});

      TS_ASSERT_DELTA(score.score(id1), 0.0685, 0.01);
      TS_ASSERT_DELTA(score.score(id2), 1.366, 0.01);
      TS_ASSERT_DELTA(score.score(id3), 0.134, 0.01);
      TS_ASSERT_DELTA(score.score(id4), 0.134, 0.01);
      TS_ASSERT_DELTA(score.score(id5), 0.0685, 0.01);

      score.clear();
      id1 = score.addNodeSet(3, 4);
      id2 = score.addNodeSet(4, 3);
      TS_ASSERT_DELTA(score.score(id1), 0.134, 0.01);
      TS_ASSERT_DELTA(score.score(id2), 0.134, 0.01);

      score.clear();
      id1 = score.addNodeSet(std::vector< gum::Idx >{1, 4, 0, 6});
      id2 = score.addNodeSet(std::vector< gum::Idx >{1, 4, 6, 0});
      id3 = score.addNodeSet(std::vector< gum::Idx >{0, 1, 4, 6});
      id4 = score.addNodeSet(std::vector< gum::Idx >{4, 6, 0, 1});
      id5 = score.addNodeSet(std::vector< gum::Idx >{6, 0, 4, 1});
      TS_ASSERT_DELTA(score.score(id1), 1.592, 0.01);
      TS_ASSERT_DELTA(score.score(id2), 1.592, 0.01);
      TS_ASSERT_DELTA(score.score(id3), 1.592, 0.01);
      TS_ASSERT_DELTA(score.score(id4), 1.592, 0.01);
      TS_ASSERT_DELTA(score.score(id5), 1.592, 0.01);

      score.clear();
      id1 = score.addNodeSet(std::vector< gum::Idx >{1, 4, 0, 6, 5});
      id2 = score.addNodeSet(std::vector< gum::Idx >{1, 4, 6, 0, 5});
      id3 = score.addNodeSet(std::vector< gum::Idx >{1, 4, 0, 5, 6});
      id4 = score.addNodeSet(std::vector< gum::Idx >{1, 4, 6, 5, 0});
      TS_ASSERT_DELTA(score.score(id1), 1.592, 0.01);
      TS_ASSERT_DELTA(score.score(id2), 1.592, 0.01);
      TS_ASSERT_DELTA(score.score(id3), 1.592, 0.01);
      TS_ASSERT_DELTA(score.score(id4), 1.592, 0.01);

      score.clear();
      id1 = score.addNodeSet(std::vector< gum::Idx >{1, 4, 0, 6, 7});
      id2 = score.addNodeSet(std::vector< gum::Idx >{1, 4, 6, 0, 7});
      id3 = score.addNodeSet(std::vector< gum::Idx >{7, 0, 1, 4, 6});
      id4 = score.addNodeSet(std::vector< gum::Idx >{4, 6, 7, 0, 1});
      id5 = score.addNodeSet(std::vector< gum::Idx >{6, 7, 0, 4, 1});
      TS_ASSERT_DELTA(score.score(id1), 1.785, 0.01);
      TS_ASSERT_DELTA(score.score(id2), 1.785, 0.01);
      TS_ASSERT_DELTA(score.score(id3), 1.785, 0.01);
      TS_ASSERT_DELTA(score.score(id4), 1.785, 0.01);
      TS_ASSERT_DELTA(score.score(id5), 1.785, 0.01);
    }
  };

} /* namespace gum_tests */
