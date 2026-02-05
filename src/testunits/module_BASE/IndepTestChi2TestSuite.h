/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


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

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  IndepTestChi2
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct IndepTestChi2TestSuite {
    public:
    static void test_chi2() {
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

      CHECK((score.score(0, 3)) == doctest::Approx(-0.999).epsilon(0.01));
      CHECK((score.score(3, 1)) == doctest::Approx(-0.389).epsilon(0.01));
      CHECK((score.score(0, 2)) == doctest::Approx(227.09).epsilon(0.01));
      CHECK((score.score(2, 0)) == doctest::Approx(227.09).epsilon(0.01));
      CHECK((score.score(1, 3, std::vector< gum::NodeId >{4}))
            == doctest::Approx(-0.648).epsilon(0.01));
      CHECK((score.score(0, 2, std::vector< gum::NodeId >{4}))
            == doctest::Approx(145.376).epsilon(0.01));
      CHECK((score.score(3, 6, std::vector< gum::NodeId >{1, 2}))
            == doctest::Approx(-0.692).epsilon(0.01));

      score.clear();
      CHECK((score.score(1, 3)) == doctest::Approx(-0.389).epsilon(0.01));
      CHECK((score.score(0, 1)) == doctest::Approx(104.865).epsilon(0.01));
      CHECK((score.score(3, 6, std::vector< gum::NodeId >{1, 2}))
            == doctest::Approx(-0.692).epsilon(0.01));
      CHECK((score.score(2, 0)) == doctest::Approx(227.09).epsilon(0.01));
      CHECK((score.score(3, 1, std::vector< gum::NodeId >{4}))
            == doctest::Approx(-0.648).epsilon(0.01));
      CHECK((score.score(2, 0)) == doctest::Approx(227.09).epsilon(0.01));
      CHECK((score.score(1, 3, std::vector< gum::NodeId >{4}))
            == doctest::Approx(-0.648).epsilon(0.01));
    }   // namespace gum_tests

    static void test_cache() {
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

        CHECK((score.score(0, 1)) == doctest::Approx(104.865).epsilon(0.01));
        CHECK((score.score(1, 3)) == doctest::Approx(-0.389).epsilon(0.01));
        CHECK((score.score(3, 6, std::vector< gum::NodeId >{1, 2}))
              == doctest::Approx(-0.692).epsilon(0.01));
        CHECK((score.score(2, 0)) == doctest::Approx(227.09).epsilon(0.01));
        CHECK((score.score(3, 1, std::vector< gum::NodeId >{4}))
              == doctest::Approx(-0.648).epsilon(0.01));
        CHECK((score.score(2, 0)) == doctest::Approx(227.09).epsilon(0.01));
        CHECK((score.score(1, 3, std::vector< gum::NodeId >{4}))
              == doctest::Approx(-0.648).epsilon(0.01));
      }
    }

    static void test_clearcache() {
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
        CHECK((score.score(0, 1)) == doctest::Approx(104.865).epsilon(0.01));
        CHECK((score.score(1, 3)) == doctest::Approx(-0.389).epsilon(0.01));
        CHECK((score.score(3, 6, std::vector< gum::NodeId >{1, 2}))
              == doctest::Approx(-0.692).epsilon(0.01));
        CHECK((score.score(2, 0)) == doctest::Approx(227.09).epsilon(0.01));
        CHECK((score.score(3, 1, std::vector< gum::NodeId >{4}))
              == doctest::Approx(-0.648).epsilon(0.01));
        CHECK((score.score(2, 0)) == doctest::Approx(227.09).epsilon(0.01));
        CHECK((score.score(1, 3, std::vector< gum::NodeId >{4}))
              == doctest::Approx(-0.648).epsilon(0.01));
      }
    }

    static void test_statistics() {
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
      CHECK((stat.first) == doctest::Approx(0.0007).epsilon(1e-3));
      CHECK((stat.second) == doctest::Approx(0.978).epsilon(1e-3));

      stat = score.statistics(1, 0);
      CHECK((stat.first) == doctest::Approx(21.4348).epsilon(1e-3));
      CHECK((stat.second) == doctest::Approx(3.6e-6).epsilon(GUM_SMALL_ERROR));

      stat = score.statistics(0, 1);
      CHECK((stat.first) == doctest::Approx(21.4348).epsilon(1e-3));
      CHECK((stat.second) == doctest::Approx(3.6e-6).epsilon(GUM_SMALL_ERROR));

      stat = score.statistics(1, 3);
      CHECK((stat.first) == doctest::Approx(0.903).epsilon(1e-3));
      CHECK((stat.second) == doctest::Approx(0.341).epsilon(1e-3));

      stat = score.statistics(0, 2, std::vector< gum::NodeId >{1});
      CHECK((stat.first) == doctest::Approx(15.2205).epsilon(1e-3));
      CHECK((stat.second) == doctest::Approx(0.0005).epsilon(1e-4));
    }

    static void test_statistics_2() {
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
      CHECK((stats.first) == doctest::Approx(15.3389).epsilon(1e-3));
      CHECK((stats.second) == doctest::Approx(0.0177784).epsilon(1e-3));
    }

    static void test_statistics_3() {
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
      CHECK((stats.first) == doctest::Approx(2.0).epsilon(1e-3));
      CHECK((stats.second) == doctest::Approx(0.7357588823428847).epsilon(1e-3));
    }

    static void test_statistics_4() {
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
      CHECK((stats.first) == doctest::Approx(2.0).epsilon(1e-3));
      CHECK((stats.second) == doctest::Approx(0.5724067044783774).epsilon(1e-3));
    }
  };

  GUM_TEST_ACTIF(_chi2)
  GUM_TEST_ACTIF(_cache)
  GUM_TEST_ACTIF(_clearcache)
  GUM_TEST_ACTIF(_statistics)
  GUM_TEST_ACTIF(_statistics_2)
  GUM_TEST_ACTIF(_statistics_3)
  GUM_TEST_ACTIF(_statistics_4)

} /* namespace gum_tests */
