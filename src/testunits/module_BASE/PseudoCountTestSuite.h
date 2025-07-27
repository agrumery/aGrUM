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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
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
#include <agrum/base/stattests/pseudoCount.h>
#include <agrum/BN/learning/priors/noPrior.h>
#include <agrum/BN/learning/priors/smoothingPrior.h>

namespace gum_tests {

  class [[maybe_unused]] PseudoCountTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_simple_counts) {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/minimal.csv"));
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
      {
        gum::learning::NoPrior     prior(database);
        gum::learning::PseudoCount counts(parser, prior);

        TS_ASSERT_EQUALS(counts.get({1}), std::vector< double >({4, 3}))
        TS_ASSERT_EQUALS(counts.get({2}), std::vector< double >({3, 2, 2}))
        TS_ASSERT_EQUALS(counts.get({0, 2}), std::vector< double >({2, 1, 1, 1, 0, 2}))
      }
      {
        gum::learning::SmoothingPrior prior(database);
        gum::learning::PseudoCount    counts(parser, prior);

        TS_ASSERT_EQUALS(counts.get({1}), std::vector< double >({5, 4}))
        TS_ASSERT_EQUALS(counts.get({2}), std::vector< double >({4, 3, 3}))
        TS_ASSERT_EQUALS(counts.get({0, 2}), std::vector< double >({3, 2, 2, 2, 1, 3}))
      }
      {
        gum::learning::SmoothingPrior prior(database);
        prior.setWeight(0.1);
        gum::learning::PseudoCount counts(parser, prior);

        TS_ASSERT_EQUALS(counts.get({1}), std::vector< double >({4.1, 3.1}))
        TS_ASSERT_EQUALS(counts.get({2}), std::vector< double >({3.1, 2.1, 2.1}))
        TS_ASSERT_EQUALS(counts.get({0, 2}), std::vector< double >({2.1, 1.1, 1.1, 1.1, 0.1, 2.1}))
      }
    }
  };
} /* namespace gum_tests */
