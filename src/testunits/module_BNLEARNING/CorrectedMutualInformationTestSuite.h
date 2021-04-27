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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>
#include <iostream>

#include <agrum/tools/database/DBTranslator4LabelizedVariable.h>
#include <agrum/tools/database/DBRowGeneratorParser.h>
#include <agrum/tools/database/DBInitializerFromCSV.h>
#include <agrum/tools/database/databaseTable.h>
#include <agrum/tools/database/DBTranslatorSet.h>
#include <agrum/tools/stattests/correctedMutualInformation.h>
#include <agrum/BN/learning/aprioris/aprioriNoApriori.h>

namespace gum_tests {

  class CorrectedMutualInformationTestSuite: public CxxTest::TestSuite {
    private:
    double entropy__(const std::vector< double >& vect) {
      double res = 0.0;
      double sum = 0.0;
      for (auto nb: vect) {
        if (nb) {
          res -= nb * std::log2(nb);
          sum += nb;
        }
      }
      res += sum * std::log2(sum);
      return res;
    }

    double H__(const gum::learning::DBRowGeneratorParser<>& parser,
               const gum::NodeId                            id) {
      gum::learning::RecordCounter<> counter(parser);
      return entropy__(counter.counts(gum::learning::IdCondSet<>(id, {})));
    }

    double H__(const gum::learning::DBRowGeneratorParser<>& parser,
               const gum::NodeId                            id1,
               const gum::NodeId                            id2) {
      gum::learning::RecordCounter<> counter(parser);
      return entropy__(
         counter.counts(gum::learning::IdCondSet<>(id1, id2, {}, true)));
    }

    double I__(const gum::learning::DBRowGeneratorParser<>& parser,
               const gum::NodeId                            id1,
               const gum::NodeId                            id2) {
      return H__(parser, id1) + H__(parser, id2) - H__(parser, id1, id2);
    }

    double H__(const gum::learning::DBRowGeneratorParser<>& parser,
               const std::vector< gum::NodeId >&            cond) {
      gum::learning::RecordCounter<> counter(parser);
      return entropy__(
         counter.counts(gum::learning::IdCondSet<>(cond, true, true)));
    }

    double H__(const gum::learning::DBRowGeneratorParser<>& parser,
               const gum::NodeId                            id,
               const std::vector< gum::NodeId >&            cond) {
      gum::learning::RecordCounter<> counter(parser);
      const auto v1 = counter.counts(gum::learning::IdCondSet<>(id, cond, true));
      const auto v2
         = counter.counts(gum::learning::IdCondSet<>(cond, false, true));
      return entropy__(v1) - entropy__(v2);
    }

    double H__(const gum::learning::DBRowGeneratorParser<>& parser,
               const gum::NodeId                            id1,
               const gum::NodeId                            id2,
               const std::vector< gum::NodeId >&            cond) {
      gum::learning::RecordCounter<> counter(parser);
      const auto                     v1
         = counter.counts(gum::learning::IdCondSet<>(id1, id2, cond, true, true));
      const auto v2
         = counter.counts(gum::learning::IdCondSet<>(cond, false, true));
      return entropy__(v1) - entropy__(v2);
    }

    double I__(const gum::learning::DBRowGeneratorParser<>& parser,
               const gum::NodeId                            id1,
               const gum::NodeId                            id2,
               const std::vector< gum::NodeId >&            cond) {
      const auto h1  = H__(parser, id1, cond);
      const auto h2  = H__(parser, id2, cond);
      const auto h12 = H__(parser, id1, id2, cond);
      return h1 + h2 - h12;
    }

    double I__(const gum::learning::DBRowGeneratorParser<>& parser,
               const gum::NodeId                            id1,
               const gum::NodeId                            id2,
               const gum::NodeId                            id3) {
      return I__(parser, id1, id2)
           - I__(parser, id1, id2, std::vector< gum::NodeId >{id3});
    }

    double I__(const gum::learning::DBRowGeneratorParser<>& parser,
               const gum::NodeId                            id1,
               const gum::NodeId                            id2,
               const gum::NodeId                            id3,
               const std::vector< gum::NodeId >&            cond) {
      std::vector< gum::NodeId > xcond(cond);
      xcond.push_back(id3);
      return I__(parser, id1, id2, cond) - I__(parser, id1, id2, xcond);
    }


    public:
    void test_Ixy_NoCorr() {
      gum::learning::DBInitializerFromCSV<> initializer(
         GET_RESSOURCES_PATH("csv/asia.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars   = var_names.size();

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

      TS_ASSERT_DELTA(score.score(1, 6, std::vector< gum::NodeId >{0}),
                      I__(parser, 1, 6, std::vector< gum::NodeId >{0}),
                      0.001);

      TS_ASSERT_DELTA(score.score(2, 6, std::vector< gum::NodeId >{}),
                      I__(parser, 2, 6),
                      0.001);
      TS_ASSERT_DELTA(score.score(2, 6), I__(parser, 2, 6), 0.001);
      TS_ASSERT_DELTA(score.score(4, 7), I__(parser, 4, 7), 0.001);

      score.clear();
      TS_ASSERT_DELTA(score.score(6, 2), I__(parser, 6, 2), 0.001);
      TS_ASSERT_DELTA(score.score(2, 6), I__(parser, 2, 6), 0.001);

      score.clear();
      TS_ASSERT_DELTA(score.score(0, 1, 5), I__(parser, 0, 1, 5), 0.01);
      TS_ASSERT_DELTA(score.score(2, 6, 5, std::vector< gum::NodeId >{1}),
                      I__(parser, 2, 6, 5, std::vector< gum::NodeId >{1}),
                      0.001);

      score.clear();
      TS_ASSERT_DELTA(score.score(4, 5, 7), I__(parser, 4, 5, 7), 0.001);
      TS_ASSERT_DELTA(score.score(4, 7, 5), I__(parser, 4, 7, 5), 0.001);
      TS_ASSERT_DELTA(score.score(5, 4, 7), I__(parser, 5, 4, 7), 0.001);
      TS_ASSERT_DELTA(score.score(5, 7, 4), I__(parser, 5, 7, 4), 0.001);
      TS_ASSERT_DELTA(score.score(7, 5, 4), I__(parser, 7, 5, 4), 0.001);
      TS_ASSERT_DELTA(score.score(7, 4, 5), I__(parser, 7, 4, 5), 0.001);

      score.clear();
      TS_ASSERT_DELTA(score.score(0, 6, 5, std::vector< gum::NodeId >{1, 4}),
                      I__(parser, 0, 6, 5, std::vector< gum::NodeId >{1, 4}),
                      1e-4);
      TS_ASSERT_DELTA(score.score(6, 0, 5, std::vector< gum::NodeId >{1, 4}),
                      I__(parser, 6, 0, 5, std::vector< gum::NodeId >{1, 4}),
                      1e-4);
    }


    void test_Ixy_Kmdl() {
      gum::learning::DBInitializerFromCSV<> initializer(
         GET_RESSOURCES_PATH("csv/asia.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars   = var_names.size();

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

      TS_ASSERT_DELTA(score.score(2, 6), I__(parser, 6, 2) - cst, 1e-4);
      TS_ASSERT_DELTA(score.score(4, 7), I__(parser, 4, 7) - cst, 1e-4);
      TS_ASSERT_DELTA(score.score(4, 7, std::vector< gum::NodeId >{5}),
                      I__(parser, 4, 7, std::vector< gum::NodeId >{5}) - 2 * cst,
                      TS_GUM_SMALL_ERROR);
      TS_ASSERT_DELTA(score.score(1, 6, std::vector< gum::NodeId >{0}),
                      I__(parser, 1, 6, std::vector< gum::NodeId >{0}) - 2 * cst,
                      1e-4);
      TS_ASSERT_DELTA(score.score(2, 6, std::vector< gum::NodeId >{}),
                      I__(parser, 2, 6) - cst,
                      1e-4);

      score.clear();
      TS_ASSERT_DELTA(score.score(6, 2), I__(parser, 6, 2) - cst, 1e-4);
      TS_ASSERT_DELTA(score.score(2, 6), I__(parser, 2, 6) - cst, 1e-4);

      score.clear();
      TS_ASSERT_DELTA(score.score(0, 1, 5), I__(parser, 0, 1, 5) + cst, 1e-4);
      TS_ASSERT_DELTA(score.score(2, 6, 5, std::vector< gum::NodeId >{1}),
                      I__(parser, 2, 6, 5, std::vector< gum::NodeId >{1})
                         + 2 * cst,
                      1e-4);


      score.clear();
      TS_ASSERT_DELTA(score.score(4, 5, 7), I__(parser, 4, 5, 7) + cst, 1e-4);
      TS_ASSERT_DELTA(score.score(4, 7, 5), I__(parser, 4, 7, 5) + cst, 1e-4);
      TS_ASSERT_DELTA(score.score(5, 4, 7), I__(parser, 5, 4, 7) + cst, 1e-4);
      TS_ASSERT_DELTA(score.score(5, 7, 4), I__(parser, 5, 7, 4) + cst, 1e-4);
      TS_ASSERT_DELTA(score.score(7, 5, 4), I__(parser, 7, 5, 4) + cst, 1e-4);
      TS_ASSERT_DELTA(score.score(7, 4, 5), I__(parser, 7, 4, 5) + cst, 1e-4);
    }
  };

} /* namespace gum_tests */
