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

#include <agrum/base/core/math/chi2.h>
#include <agrum/base/database/databaseTable.h>
#include <agrum/base/database/DBInitializerFromCSV.h>
#include <agrum/base/database/DBRowGeneratorParser.h>
#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>
#include <agrum/base/stattests/indepTestG2.h>
#include <agrum/BN/learning/priors/noPrior.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  IndepTestG2
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct IndepTestG2TestSuite {
    public:
    static void test_G2() {
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

      // strongly dependent pair: G2 statistic > 0, p-value << 0.001
      auto s01 = score.statistics(0, 1);
      CHECK(s01.first > 0.0);
      CHECK(s01.second < 0.001);

      // symmetry: statistics(x,y) == statistics(y,x)
      auto s10 = score.statistics(1, 0);
      CHECK(s10.first == doctest::Approx(s01.first).epsilon(1e-9));

      // independent pair: p-value > 0.05
      auto s31 = score.statistics(3, 1);
      CHECK(s31.second > 0.05);

      // strongly dependent: p-value << 0.001, symmetry
      auto s02 = score.statistics(0, 2);
      CHECK(s02.second < 0.001);
      auto s20 = score.statistics(2, 0);
      CHECK(s20.first == doctest::Approx(s02.first).epsilon(1e-9));

      // conditionally independent given {4}: p-value > 0.05
      auto s13_4 = score.statistics(1, 3, std::vector< gum::NodeId >{4});
      CHECK(s13_4.second > 0.05);

      // conditionally dependent given {4}: p-value << 0.001
      auto s02_4 = score.statistics(0, 2, std::vector< gum::NodeId >{4});
      CHECK(s02_4.second < 0.001);

      // conditionally independent given {1,2}: p-value > 0.05
      auto s36_12 = score.statistics(3, 6, std::vector< gum::NodeId >{1, 2});
      CHECK(s36_12.second > 0.05);

      // stability after clear()
      score.clear();
      CHECK(score.statistics(0, 1).first == doctest::Approx(s01.first).epsilon(1e-9));
      CHECK(score.statistics(3, 1).first == doctest::Approx(s31.first).epsilon(1e-9));
      CHECK(score.statistics(2, 0).first == doctest::Approx(s20.first).epsilon(1e-9));
    }

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
      gum::learning::IndepTestG2          score(parser, prior);

      // establish reference values
      auto ref01   = score.statistics(0, 1);
      auto ref13   = score.statistics(1, 3);
      auto ref36   = score.statistics(3, 6, std::vector< gum::NodeId >{1, 2});
      auto ref20   = score.statistics(2, 0);
      auto ref31_4 = score.statistics(3, 1, std::vector< gum::NodeId >{4});
      auto ref13_4 = score.statistics(1, 3, std::vector< gum::NodeId >{4});

      // repeated calls must return identical values
      for (gum::Idx i = 0; i < 100; ++i) {
        CHECK(score.statistics(0, 1).first == doctest::Approx(ref01.first).epsilon(1e-9));
        CHECK(score.statistics(1, 3).first == doctest::Approx(ref13.first).epsilon(1e-9));
        CHECK(score.statistics(3, 6, std::vector< gum::NodeId >{1, 2}).first
              == doctest::Approx(ref36.first).epsilon(1e-9));
        CHECK(score.statistics(2, 0).first == doctest::Approx(ref20.first).epsilon(1e-9));
        CHECK(score.statistics(3, 1, std::vector< gum::NodeId >{4}).first
              == doctest::Approx(ref31_4.first).epsilon(1e-9));
        CHECK(score.statistics(1, 3, std::vector< gum::NodeId >{4}).first
              == doctest::Approx(ref13_4.first).epsilon(1e-9));
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
      gum::learning::IndepTestG2          score(parser, prior);

      // establish reference values before any clear()
      auto ref01   = score.statistics(0, 1);
      auto ref13   = score.statistics(1, 3);
      auto ref36   = score.statistics(3, 6, std::vector< gum::NodeId >{1, 2});
      auto ref20   = score.statistics(2, 0);
      auto ref31_4 = score.statistics(3, 1, std::vector< gum::NodeId >{4});
      auto ref13_4 = score.statistics(1, 3, std::vector< gum::NodeId >{4});

      // clear() must not corrupt recomputed results
      for (gum::Idx i = 0; i < 4; ++i) {
        score.clear();
        CHECK(score.statistics(0, 1).first == doctest::Approx(ref01.first).epsilon(1e-9));
        CHECK(score.statistics(1, 3).first == doctest::Approx(ref13.first).epsilon(1e-9));
        CHECK(score.statistics(3, 6, std::vector< gum::NodeId >{1, 2}).first
              == doctest::Approx(ref36.first).epsilon(1e-9));
        CHECK(score.statistics(2, 0).first == doctest::Approx(ref20.first).epsilon(1e-9));
        CHECK(score.statistics(3, 1, std::vector< gum::NodeId >{4}).first
              == doctest::Approx(ref31_4.first).epsilon(1e-9));
        CHECK(score.statistics(1, 3, std::vector< gum::NodeId >{4}).first
              == doctest::Approx(ref13_4.first).epsilon(1e-9));
      }
    }

    static void test_copy_constructor() {
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
      gum::learning::IndepTestG2          orig(parser, prior);

      auto ref01   = orig.statistics(0, 1);
      auto ref13_4 = orig.statistics(1, 3, std::vector< gum::NodeId >{4});
      auto ref36   = orig.statistics(3, 6, std::vector< gum::NodeId >{1, 2});

      // copy constructor (_domain_sizes_ is now in IndependenceTest, copied by the base class)
      gum::learning::IndepTestG2 copy(orig);
      CHECK(copy.statistics(0, 1).first == doctest::Approx(ref01.first).epsilon(1e-9));
      CHECK(copy.statistics(1, 3, std::vector< gum::NodeId >{4}).first
            == doctest::Approx(ref13_4.first).epsilon(1e-9));
      CHECK(copy.statistics(3, 6, std::vector< gum::NodeId >{1, 2}).first
            == doctest::Approx(ref36.first).epsilon(1e-9));

      // clone (virtual copy constructor — calls the same copy constructor)
      auto* cloned = orig.clone();
      CHECK(cloned->statistics(0, 1).first == doctest::Approx(ref01.first).epsilon(1e-9));
      CHECK(cloned->statistics(3, 6, std::vector< gum::NodeId >{1, 2}).first
            == doctest::Approx(ref36.first).epsilon(1e-9));
      delete cloned;

      // copy assignment
      gum::learning::IndepTestG2 assigned(parser, prior);
      assigned = orig;
      CHECK(assigned.statistics(0, 1).first == doctest::Approx(ref01.first).epsilon(1e-9));
      CHECK(assigned.statistics(1, 3, std::vector< gum::NodeId >{4}).first
            == doctest::Approx(ref13_4.first).epsilon(1e-9));
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
      gum::learning::IndepTestG2          score(parser, prior);

      // A:0, B:1, C:2, D:3
      auto stat = score.statistics(0, 2);
      CHECK(stat.first == doctest::Approx(0.000761).epsilon(1e-3));
      CHECK(stat.second == doctest::Approx(0.9780).epsilon(1e-3));

      stat = score.statistics(1, 0);
      CHECK(stat.first == doctest::Approx(21.5847).epsilon(1e-3));
      CHECK(stat.second == doctest::Approx(3.39e-6).epsilon(GUM_SMALL_ERROR));

      stat = score.statistics(0, 1);
      CHECK(stat.first == doctest::Approx(21.5847).epsilon(1e-3));
      CHECK(stat.second == doctest::Approx(3.39e-6).epsilon(GUM_SMALL_ERROR));

      stat = score.statistics(1, 3);
      CHECK(stat.first == doctest::Approx(0.9027).epsilon(1e-3));
      CHECK(stat.second == doctest::Approx(0.3421).epsilon(1e-3));

      stat = score.statistics(0, 2, std::vector< gum::NodeId >{1});
      CHECK(stat.first == doctest::Approx(16.3470).epsilon(1e-3));
      CHECK(stat.second == doctest::Approx(0.000282).epsilon(1e-4));
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
      gum::learning::IndepTestG2          score(parser, prior);

      auto stats = score.statistics(0, 1);
      CHECK((stats.first) == doctest::Approx(16.6066).epsilon(1e-3));
      CHECK((stats.second) == doctest::Approx(0.0108433).epsilon(1e-3));
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
      gum::learning::IndepTestG2          score(parser, prior);

      auto stats = score.statistics(0, 1, {2, 3});
      CHECK(stats.first == doctest::Approx(2.7726).epsilon(1e-3));
      // indepXYZ1Z2.csv has many silent cells (sparse, 6 rows); df_effective=1
      CHECK(stats.second == doctest::Approx(gum::Chi2::probaChi2(stats.first, 1)).epsilon(1e-3));
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
      gum::learning::IndepTestG2          score(parser, prior);

      auto stats = score.statistics(0, 1, {2});
      CHECK(stats.first == doctest::Approx(2.7726).epsilon(1e-3));
      // indepXYZ.csv has many silent cells (sparse, 6 rows); df_effective=1
      CHECK(stats.second == doctest::Approx(gum::Chi2::probaChi2(stats.first, 1)).epsilon(1e-3));
    }

    // sparse_XYZ.csv: X(A,B,C) x Y(a,b) | Z(0,1)
    // C never appears with Z=0 → 2 silent cells (tmp2=0) in the Z=0 slice.
    // df_theoretical = 2*(3-1)*(2-1) = 4, n_skipped = 2 → df_effective = 2.
    // G2 = 2*(12*log(3/2)+4*log(1/2) + 6*log(3/2)+2*log(1/2)) ≈ 6.279
    // With df=2: p-value = exp(-G2/2) ≈ 0.04327, vs uncorrected ≈ 0.179.
    static void test_df_skipped_cells() {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/sparse_XYZ.csv"));
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

      // node 0=X, node 1=Y, node 2=Z
      auto stat = score.statistics(0, 1, std::vector< gum::NodeId >{2});

      // G2 = 2*(12*log(3/2)+4*log(1/2) + 6*log(3/2)+2*log(1/2)) ≈ 6.2790
      CHECK(stat.first == doctest::Approx(6.2790).epsilon(1e-3));

      // p-value must match Chi2(df=2), not Chi2(df=4)
      CHECK(stat.second == doctest::Approx(gum::Chi2::probaChi2(stat.first, 2)).epsilon(1e-3));

      // verify the correction tightens the test: corrected p < uncorrected p
      CHECK(stat.second < gum::Chi2::probaChi2(stat.first, 4));
    }
  };

  GUM_TEST_ACTIF(_G2)
  GUM_TEST_ACTIF(_cache)
  GUM_TEST_ACTIF(_clearcache)
  GUM_TEST_ACTIF(_copy_constructor)
  GUM_TEST_ACTIF(_statistics)
  GUM_TEST_ACTIF(_statistics_2)
  GUM_TEST_ACTIF(_statistics_3)
  GUM_TEST_ACTIF(_statistics_4)
  GUM_TEST_ACTIF(_df_skipped_cells)

} /* namespace gum_tests */
