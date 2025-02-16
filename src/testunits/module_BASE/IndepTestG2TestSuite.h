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

#include <agrum/base/database/databaseTable.h>
#include <agrum/base/database/DBInitializerFromCSV.h>
#include <agrum/base/database/DBRowGeneratorParser.h>
#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>
#include <agrum/base/stattests/indepTestG2.h>
#include <agrum/BN/learning/priors/noPrior.h>

namespace gum_tests {

  class [[maybe_unused]] IndepTestG2TestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_G2) {
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
      gum::learning::IndepTestG2          score(parser, prior);

      TS_ASSERT_DELTA(score.score(0, 1), 123.3614, 1e-3)
      TS_ASSERT_DELTA(score.score(3, 1), -0.2345, 1e-3)

      TS_ASSERT_DELTA(score.score(0, 2), 230.7461, 1e-3)
      TS_ASSERT_DELTA(score.score(2, 0), 230.7461, 1e-3)

      TS_ASSERT_DELTA(score.score(1, 3, std::vector< gum::NodeId >{4}), -0.5569, 1e-3)
      TS_ASSERT_DELTA(score.score(0, 2, std::vector< gum::NodeId >{4}), 147.7192, 1e-3)

      TS_ASSERT_DELTA(score.score(3, 6, std::vector< gum::NodeId >{1, 2}), -0.6153, 1e-3)

      TS_ASSERT_DELTA(score.score(0, 1), 123.3614, 1e-3)
      TS_ASSERT_DELTA(score.score(1, 3), -0.2345, 1e-3)
      TS_ASSERT_DELTA(score.score(3, 6, std::vector< gum::NodeId >{1, 2}), -0.6153, 1e-3)
      TS_ASSERT_DELTA(score.score(2, 0), 230.7461, 1e-3)
      TS_ASSERT_DELTA(score.score(3, 1, std::vector< gum::NodeId >{4}), -0.5569, 1e-3)
      TS_ASSERT_DELTA(score.score(2, 0), 230.7461, 1e-3)
      TS_ASSERT_DELTA(score.score(1, 3, std::vector< gum::NodeId >{4}), -0.5569, 1e-3)
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
      gum::learning::IndepTestG2          score(parser, prior);
      // score.useCache ( false );

      for (gum::Idx i = 0; i < 1000; ++i) {
        TS_ASSERT_DELTA(score.score(0, 1), 123.3614, 1e-3)
        TS_ASSERT_DELTA(score.score(1, 3), -0.2345, 1e-3)
        TS_ASSERT_DELTA(score.score(3, 6, std::vector< gum::NodeId >{1, 2}), -0.6153, 1e-3)
        TS_ASSERT_DELTA(score.score(2, 0), 230.7461, 1e-3)
        TS_ASSERT_DELTA(score.score(3, 1, std::vector< gum::NodeId >{4}), -0.5569, 1e-3)
        TS_ASSERT_DELTA(score.score(2, 0), 230.7461, 1e-3)
        TS_ASSERT_DELTA(score.score(1, 3, std::vector< gum::NodeId >{4}), -0.5569, 1e-3)
      }
    }

    void xtest_clearcache() {
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
      gum::learning::IndepTestG2          score(parser, prior);
      // score.useCache ( false );

      for (gum::Idx i = 0; i < 4; ++i) {
        score.clearCache();
        TS_ASSERT_DELTA(score.score(0, 1), 61.181, 1e-3)
        TS_ASSERT_DELTA(score.score(1, 3), 0.617, 1e-3)
        TS_ASSERT_DELTA(score.score(3, 6, std::vector< gum::NodeId >{1, 2}), -0.6153, 1e-3)
        TS_ASSERT_DELTA(score.score(2, 0), 230.7461, 1e-3)
        TS_ASSERT_DELTA(score.score(3, 1, std::vector< gum::NodeId >{4}), 0.778, 1e-3)
        TS_ASSERT_DELTA(score.score(2, 0), 230.7461, 1e-3)
        TS_ASSERT_DELTA(score.score(1, 3, std::vector< gum::NodeId >{4}), 0.778, 1e-3)
      }
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
      gum::learning::IndepTestG2          score(parser, prior);

      auto stats = score.statistics(0, 1);
      TS_ASSERT_DELTA(stats.first, 16.6066, 1e-3)
      TS_ASSERT_DELTA(stats.second, 0.0108433, 1e-3)
    }
  };

} /* namespace gum_tests */
