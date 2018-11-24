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

#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBRowGeneratorParser.h>
#include <agrum/learning/database/DBInitializerFromCSV.h>
#include <agrum/learning/database/databaseTable.h>
#include <agrum/learning/database/DBTranslatorSet.h>
#include <agrum/learning/scores_and_tests/correctedMutualInformation.h>
#include <agrum/learning/aprioris/aprioriNoApriori.h>

namespace gum_tests {

  class CorrectedMutualInformationTestSuite : public CxxTest::TestSuite {
    private:
    double __entropy(const std::vector< double >& vect) {
      double res = 0.0;
      double sum = 0.0;
      for (auto nb : vect) {
        if (nb) {
          res -= nb * std::log2(nb);
          sum += nb;
        }
      }
      res += sum * std::log2(sum);
      return res;
    }

    double __H(const gum::learning::DBRowGeneratorParser<>& parser,
               const gum::NodeId                            id) {
      gum::learning::RecordCounter<> counter(parser);
      return __entropy(counter.counts(gum::learning::IdSet<>(id, {})));
    }

    double __H(const gum::learning::DBRowGeneratorParser<>& parser,
               const gum::NodeId                            id1,
               const gum::NodeId                            id2) {
      gum::learning::RecordCounter<> counter(parser);
      return __entropy(counter.counts(gum::learning::IdSet<>(id1, id2, {}, true)));
    }

    double __I(const gum::learning::DBRowGeneratorParser<>& parser,
               const gum::NodeId                            id1,
               const gum::NodeId                            id2) {
      return __H(parser, id1) + __H(parser, id2) - __H(parser, id1, id2);
    }

    double __H(const gum::learning::DBRowGeneratorParser<>& parser,
               const std::vector< gum::NodeId >&            cond) {
      gum::learning::RecordCounter<> counter(parser);
      return __entropy(counter.counts(gum::learning::IdSet<>(cond, true, true)));
    }

    double __H(const gum::learning::DBRowGeneratorParser<>& parser,
               const gum::NodeId                            id,
               const std::vector< gum::NodeId >&            cond) {
      gum::learning::RecordCounter<> counter(parser);
      return __entropy(counter.counts(gum::learning::IdSet<>(id, cond, true)))
             - __entropy(
                 counter.counts(gum::learning::IdSet<>(cond, false, true)));
    }

    double __H(const gum::learning::DBRowGeneratorParser<>& parser,
               const gum::NodeId                            id1,
               const gum::NodeId                            id2,
               const std::vector< gum::NodeId >&            cond) {
      gum::learning::RecordCounter<> counter(parser);
      return __entropy(
               counter.counts(gum::learning::IdSet<>(id1, id2, cond, true, true)))
             - __entropy(
                 counter.counts(gum::learning::IdSet<>(cond, false, true)));
    }

    double __I(const gum::learning::DBRowGeneratorParser<>& parser,
               const gum::NodeId                            id1,
               const gum::NodeId                            id2,
               const std::vector< gum::NodeId >&            cond) {
      return __H(parser, id1, cond) + __H(parser, id2, cond)
             - __H(parser, id1, id2, cond);
    }

    double __I(const gum::learning::DBRowGeneratorParser<>& parser,
               const gum::NodeId                            id1,
               const gum::NodeId                            id2,
               const gum::NodeId                            id3) {
      return __I(parser, id1, id2)
             - __I(parser, id1, id2, std::vector< gum::NodeId >{id3});
    }

    double __I(const gum::learning::DBRowGeneratorParser<>& parser,
               const gum::NodeId                            id1,
               const gum::NodeId                            id2,
               const gum::NodeId                            id3,
               const std::vector< gum::NodeId >&            cond) {
      std::vector< gum::NodeId > xcond(cond);
      xcond.push_back(id3);
      return __I(parser, id1, id2, cond) - __I(parser, id1, id2, xcond);
    }


    public:
    void test_Ixy_NoCorr() {
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
      gum::learning::AprioriNoApriori<>     apriori(database);

      gum::learning::CorrectedMutualInformation<> score(parser, apriori);
      score.useNoCorr();

      TS_ASSERT_DELTA(score.score(2, 6), __I(parser, 2, 6), 0.001);
      TS_ASSERT_DELTA(score.score(4, 7), __I(parser, 4, 7), 0.001);
      TS_ASSERT_DELTA(score.score(1, 6, std::vector< gum::NodeId >{0}),
                      __I(parser, 1, 6, std::vector< gum::NodeId >{0}),
                      0.001);
      TS_ASSERT_DELTA(
        score.score(2, 6, std::vector< gum::NodeId >{}), __I(parser, 2, 6), 0.001);

      score.clear();
      TS_ASSERT_DELTA(score.score(6, 2), __I(parser, 6, 2), 0.001);
      TS_ASSERT_DELTA(score.score(2, 6), __I(parser, 2, 6), 0.001);

      score.clear();
      TS_ASSERT_DELTA(score.score(0, 1, 5), __I(parser, 0, 1, 5), 0.01);
      TS_ASSERT_DELTA(score.score(2, 6, 5, std::vector< gum::NodeId >{1}),
                      __I(parser, 2, 6, 5, std::vector< gum::NodeId >{1}),
                      0.001);

      score.clear();
      TS_ASSERT_DELTA(score.score(4, 5, 7), __I(parser, 4, 5, 7), 0.001);
      TS_ASSERT_DELTA(score.score(4, 7, 5), __I(parser, 4, 7, 5), 0.001);
      TS_ASSERT_DELTA(score.score(5, 4, 7), __I(parser, 5, 4, 7), 0.001);
      TS_ASSERT_DELTA(score.score(5, 7, 4), __I(parser, 5, 7, 4), 0.001);
      TS_ASSERT_DELTA(score.score(7, 5, 4), __I(parser, 7, 5, 4), 0.001);
      TS_ASSERT_DELTA(score.score(7, 4, 5), __I(parser, 7, 4, 5), 0.001);

      score.clear();
      TS_ASSERT_DELTA(score.score(0, 6, 5, std::vector< gum::NodeId >{1, 4}),
                      __I(parser, 0, 6, 5, std::vector< gum::NodeId >{1, 4}),
                      1e-4);
      TS_ASSERT_DELTA(score.score(6, 0, 5, std::vector< gum::NodeId >{1, 4}),
                      __I(parser, 6, 0, 5, std::vector< gum::NodeId >{1, 4}),
                      1e-4);
    }


    void test_Ixy_Kmdl() {
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
      gum::learning::AprioriNoApriori<>     apriori(database);

      std::vector< gum::Size > modalities(nb_vars, 2);

      gum::learning::CorrectedMutualInformation<> score(parser, apriori);
      score.useMDL();

      const double cst = 0.5 * std::log2(10000);

      TS_ASSERT_DELTA(score.score(2, 6), __I(parser, 6, 2) - cst, 1e-4);
      TS_ASSERT_DELTA(score.score(4, 7), __I(parser, 4, 7) - cst, 1e-4);
      TS_ASSERT_DELTA(score.score(4, 7, std::vector< gum::NodeId >{5}),
                      __I(parser, 4, 7, std::vector< gum::NodeId >{5}) - 2 * cst,
                      1e-5);
      TS_ASSERT_DELTA(score.score(1, 6, std::vector< gum::NodeId >{0}),
                      __I(parser, 1, 6, std::vector< gum::NodeId >{0}) - 2 * cst,
                      1e-4);
      TS_ASSERT_DELTA(score.score(2, 6, std::vector< gum::NodeId >{}),
                      __I(parser, 2, 6) - cst,
                      1e-4);

      score.clear();
      TS_ASSERT_DELTA(score.score(6, 2), __I(parser, 6, 2) - cst, 1e-4);
      TS_ASSERT_DELTA(score.score(2, 6), __I(parser, 2, 6) - cst, 1e-4);

      score.clear();
      TS_ASSERT_DELTA(score.score(0, 1, 5), __I(parser, 0, 1, 5) + cst, 1e-4);
      TS_ASSERT_DELTA(score.score(2, 6, 5, std::vector< gum::NodeId >{1}),
                      __I(parser, 2, 6, 5, std::vector< gum::NodeId >{1})
                        + 2 * cst,
                      1e-4);


      score.clear();
      TS_ASSERT_DELTA(score.score(4, 5, 7), __I(parser, 4, 5, 7) + cst, 1e-4);
      TS_ASSERT_DELTA(score.score(4, 7, 5), __I(parser, 4, 7, 5) + cst, 1e-4);
      TS_ASSERT_DELTA(score.score(5, 4, 7), __I(parser, 5, 4, 7) + cst, 1e-4);
      TS_ASSERT_DELTA(score.score(5, 7, 4), __I(parser, 5, 7, 4) + cst, 1e-4);
      TS_ASSERT_DELTA(score.score(7, 5, 4), __I(parser, 7, 5, 4) + cst, 1e-4);
      TS_ASSERT_DELTA(score.score(7, 4, 5), __I(parser, 7, 4, 5) + cst, 1e-4);
    }
  };

} /* namespace gum_tests */
