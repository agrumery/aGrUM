/****************************************************************************
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
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
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

#include <agrum/base/database/databaseTable.h>
#include <agrum/base/database/DBInitializerFromCSV.h>
#include <agrum/base/database/DBRowGeneratorParser.h>
#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>
#include <agrum/base/stattests/indepTestChi2.h>
#include <agrum/BN/learning/priors/noPrior.h>

namespace gum_tests {

  class [[maybe_unused]] IndepTestChi2TestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_chi2) {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/asia.csv"));
      const auto&                         var_names = initializer.variableNames();
      const std::size_t                   nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet                translator_set;
      gum::learning::DBTranslator4LabelizedVariable translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::NoPrior              prior(database);
      gum::learning::IndepTestChi2        score(parser, prior);

      TS_ASSERT_DELTA(score.score(0, 3), -0.999, 0.01)
      TS_ASSERT_DELTA(score.score(3, 1), -0.389, 0.01)
      TS_ASSERT_DELTA(score.score(0, 2), 227.09, 0.01)
      TS_ASSERT_DELTA(score.score(2, 0), 227.09, 0.01)
      TS_ASSERT_DELTA(score.score(1, 3, std::vector< gum::NodeId >{4}), -0.648, 0.01)
      TS_ASSERT_DELTA(score.score(0, 2, std::vector< gum::NodeId >{4}), 145.376, 0.01)
      TS_ASSERT_DELTA(score.score(3, 6, std::vector< gum::NodeId >{1, 2}), -0.692, 0.01)

      score.clear();
      TS_ASSERT_DELTA(score.score(1, 3), -0.389, 0.01)
      TS_ASSERT_DELTA(score.score(0, 1), 104.865, 0.01)
      TS_ASSERT_DELTA(score.score(3, 6, std::vector< gum::NodeId >{1, 2}), -0.692, 0.01)
      TS_ASSERT_DELTA(score.score(2, 0), 227.09, 0.01)
      TS_ASSERT_DELTA(score.score(3, 1, std::vector< gum::NodeId >{4}), -0.648, 0.01)
      TS_ASSERT_DELTA(score.score(2, 0), 227.09, 0.01)
      TS_ASSERT_DELTA(score.score(1, 3, std::vector< gum::NodeId >{4}), -0.648, 0.01)
    }

    GUM_ACTIVE_TEST(_cache) {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/asia.csv"));
      const auto&                         var_names = initializer.variableNames();
      const std::size_t                   nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet                translator_set;
      gum::learning::DBTranslator4LabelizedVariable translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::NoPrior              prior(database);
      gum::learning::IndepTestChi2        score(parser, prior);
      // score.useCache ( false );

      for (gum::Idx i = 0; i < 100; ++i) {
        score.clear();

        TS_ASSERT_DELTA(score.score(0, 1), 104.865, 0.01)
        TS_ASSERT_DELTA(score.score(1, 3), -0.389, 0.01)
        TS_ASSERT_DELTA(score.score(3, 6, std::vector< gum::NodeId >{1, 2}), -0.692, 0.01)
        TS_ASSERT_DELTA(score.score(2, 0), 227.09, 0.01)
        TS_ASSERT_DELTA(score.score(3, 1, std::vector< gum::NodeId >{4}), -0.648, 0.01)
        TS_ASSERT_DELTA(score.score(2, 0), 227.09, 0.01)
        TS_ASSERT_DELTA(score.score(1, 3, std::vector< gum::NodeId >{4}), -0.648, 0.01)
      }
    }

    GUM_ACTIVE_TEST(_clearcache) {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/asia.csv"));
      const auto&                         var_names = initializer.variableNames();
      const std::size_t                   nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet                translator_set;
      gum::learning::DBTranslator4LabelizedVariable translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::NoPrior              prior(database);
      gum::learning::IndepTestChi2        score(parser, prior);
      // score.useCache ( false );

      for (gum::Idx i = 0; i < 4; ++i) {
        score.clear();
        TS_ASSERT_DELTA(score.score(0, 1), 104.865, 0.01)
        TS_ASSERT_DELTA(score.score(1, 3), -0.389, 0.01)
        TS_ASSERT_DELTA(score.score(3, 6, std::vector< gum::NodeId >{1, 2}), -0.692, 0.01)
        TS_ASSERT_DELTA(score.score(2, 0), 227.09, 0.01)
        TS_ASSERT_DELTA(score.score(3, 1, std::vector< gum::NodeId >{4}), -0.648, 0.01)
        TS_ASSERT_DELTA(score.score(2, 0), 227.09, 0.01)
        TS_ASSERT_DELTA(score.score(1, 3, std::vector< gum::NodeId >{4}), -0.648, 0.01)
      }
    }

    GUM_ACTIVE_TEST(_statistics) {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/chi2.csv"));
      const auto&                         var_names = initializer.variableNames();
      const std::size_t                   nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet                translator_set;
      gum::learning::DBTranslator4LabelizedVariable translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::NoPrior              prior(database);
      gum::learning::IndepTestChi2        score(parser, prior);

      // A:0, B:1, C:2, D:3
      auto stat = score.statistics(0, 2);
      TS_ASSERT_DELTA(stat.first, 0.0007, 1e-3)
      TS_ASSERT_DELTA(stat.second, 0.978, 1e-3)

      stat = score.statistics(1, 0);
      TS_ASSERT_DELTA(stat.first, 21.4348, 1e-3)
      TS_ASSERT_DELTA(stat.second, 3.6e-6, TS_GUM_SMALL_ERROR)

      stat = score.statistics(0, 1);
      TS_ASSERT_DELTA(stat.first, 21.4348, 1e-3)
      TS_ASSERT_DELTA(stat.second, 3.6e-6, TS_GUM_SMALL_ERROR)

      stat = score.statistics(1, 3);
      TS_ASSERT_DELTA(stat.first, 0.903, 1e-3)
      TS_ASSERT_DELTA(stat.second, 0.341, 1e-3)

      stat = score.statistics(0, 2, std::vector< gum::NodeId >{1});
      TS_ASSERT_DELTA(stat.first, 15.2205, 1e-3)
      TS_ASSERT_DELTA(stat.second, 0.0005, 1e-4)
    }

    GUM_ACTIVE_TEST(_statistics_2) {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/testXYbase.csv"));
      const auto&                         var_names = initializer.variableNames();
      const std::size_t                   nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet                translator_set;
      gum::learning::DBTranslator4LabelizedVariable translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::NoPrior              prior(database);
      gum::learning::IndepTestChi2        score(parser, prior);

      auto stats = score.statistics(0, 1);
      TS_ASSERT_DELTA(stats.first, 15.3389, 1e-3)
      TS_ASSERT_DELTA(stats.second, 0.0177784, 1e-3)
    }

    GUM_ACTIVE_TEST(_statistics_3) {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/indepXYZ1Z2.csv"));
      const auto&                         var_names = initializer.variableNames();
      const std::size_t                   nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet                translator_set;
      gum::learning::DBTranslator4LabelizedVariable translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::NoPrior              prior(database);
      gum::learning::IndepTestChi2        score(parser, prior);

      auto stats = score.statistics(0, 1, {2, 3});
      TS_ASSERT_DELTA(stats.first, 2.0, 1e-3)
      TS_ASSERT_DELTA(stats.second, 0.7357588823428847, 1e-3)
    }

    GUM_ACTIVE_TEST(_statistics_4) {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/indepXYZ.csv"));
      const auto&                         var_names = initializer.variableNames();
      const std::size_t                   nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet                translator_set;
      gum::learning::DBTranslator4LabelizedVariable translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::NoPrior              prior(database);
      gum::learning::IndepTestChi2        score(parser, prior);

      auto stats = score.statistics(0, 1, {2});
      TS_ASSERT_DELTA(stats.first, 2.0, 1e-3)
      TS_ASSERT_DELTA(stats.second, 0.5724067044783774, 1e-3)
    }
  };

} /* namespace gum_tests */
