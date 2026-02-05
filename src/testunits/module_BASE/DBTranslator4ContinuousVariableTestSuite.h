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
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/database/DBTranslator4ContinuousVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DBTranslator4ContinuousVariable
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct DBTranslator4ContinuousVariableTestSuite {
    public:
    static void test_trans1() {
      gum::learning::DBTranslator4ContinuousVariable translator;
      CHECK(translator.isLossless());
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("3"));
      CHECK((translator.translate("3").cont_val) == (3));
      CHECK((std::stof(translator.translateBack(gum::learning::DBTranslatedValue{3.0f}))) == (3));

      CHECK((translator.missingValue().cont_val) == (std::numeric_limits< float >::max()));

      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("5"));
      CHECK((translator.translate("4.22").cont_val) == (4.22f));
      CHECK((translator.translate("-5.34").cont_val) == (-5.34f));
      CHECK((translator.translate("-5.34e3").cont_val) == (-5.34e3f));
      CHECK((translator.translate("-5.34e-34").cont_val) == (-5.34e-34f));
      CHECK(std::stof(translator.translateBack(gum::learning::DBTranslatedValue{-5.34f}))
            == -5.34f);
      CHECK(std::stof(translator.translateBack(gum::learning::DBTranslatedValue{-5.34e3f}))
            == -5.34e3f);
      CHECK(std::stof(translator.translateBack(gum::learning::DBTranslatedValue{-5.34e-34f}))
            == -5.34e-34f);
      CHECK_THROWS_AS(translator.translate("4.22x"), const gum::TypeError&);
      CHECK_THROWS_AS(translator.translate("0x422"), const gum::TypeError&);

      std::vector< std::string >                     missing{"2", "N/A", "20", "4", "xxx"};
      gum::learning::DBTranslator4ContinuousVariable translator2(missing);
      CHECK(!translator2.missingSymbols().exists("2"));
      CHECK(!translator2.missingSymbols().exists("20"));
      CHECK(!translator2.missingSymbols().exists("4"));
      CHECK((translator.translate("4.22").cont_val) == (4.22f));
      CHECK_THROWS_AS(translator2.translate("yyy"), const gum::TypeError&);
      CHECK((translator2.translate("N/A").cont_val) == (std::numeric_limits< float >::max()));
      CHECK((translator2.translate("xxx").cont_val) == (std::numeric_limits< float >::max()));

      gum::ContinuousVariable< double >              var("X2", "", -2, 10);
      gum::learning::DBTranslator4ContinuousVariable translator3(var, missing);
      CHECK(!translator3.missingSymbols().exists("2"));
      CHECK(!translator3.missingSymbols().exists("4"));
      CHECK(translator3.missingSymbols().exists("20"));
      CHECK(translator3.missingSymbols().exists("N/A"));
      CHECK(translator3.missingSymbols().exists("xxx"));

      CHECK((translator3.translate("4.22").cont_val) == (4.22f));
      CHECK((translator3.translate("-2").cont_val) == (-2.0f));
      CHECK((translator3.translate("10").cont_val) == (10.0f));
      CHECK_THROWS_AS(translator3.translate("11"), const gum::UnknownLabelInDatabase&);
      CHECK((translator3.translate("N/A").cont_val) == (std::numeric_limits< float >::max()));
      CHECK((translator3.translate("xxx").cont_val) == (std::numeric_limits< float >::max()));

      const auto& tr_var    = *(translator3.variable());
      int         good_cont = 1;
      try {
        const gum::ContinuousVariable< double >& xvar_cont
            = dynamic_cast< const gum::ContinuousVariable< double >& >(tr_var);
        CHECK((xvar_cont.lowerBound()) == (-2.0));
        CHECK((xvar_cont.upperBound()) == (10.0));
      } catch (std::bad_cast&) { good_cont = 0; }
      CHECK((good_cont) == (1));

      std::vector< std::string >                     missing4{"2", "N/A", "20", "4", "xxx", "-10"};
      gum::learning::DBTranslator4ContinuousVariable translator4(var, missing4, true);
      CHECK(!translator4.missingSymbols().exists("2"));
      CHECK(!translator4.missingSymbols().exists("4"));
      CHECK(translator4.missingSymbols().exists("20"));
      CHECK(translator4.missingSymbols().exists("N/A"));
      CHECK(translator4.missingSymbols().exists("xxx"));
      CHECK((translator4.translate("4.22").cont_val) == (4.22f));
      CHECK((translator4.translate("-2").cont_val) == (-2.0f));
      CHECK((translator4.translate("10").cont_val) == (10.0f));
      CHECK((translator4.translate("18").cont_val) == (18.0f));
      CHECK((translator4.translate("20").cont_val) == (std::numeric_limits< float >::max()));
      CHECK((translator4.translate("19.5").cont_val) == (19.5f));
      CHECK_THROWS_AS(translator4.translate("21"), const gum::OperationNotAllowed&);
      CHECK((translator4.translate("-12").cont_val) == (-12.0f));
      CHECK((translator4.translate("-10").cont_val) == (-10.0f));
      CHECK(std::stof(translator4.translateBack(gum::learning::DBTranslatedValue{5.3f})) == 5.3f);
      CHECK_THROWS_AS(translator4.translateBack(gum::learning::DBTranslatedValue{50.0f}),
                      const gum::UnknownLabelInDatabase&);
      CHECK_THROWS_AS(translator4.translateBack(gum::learning::DBTranslatedValue{-50.0f}),
                      const gum::UnknownLabelInDatabase&);
      CHECK(translator4.missingSymbols().exists(translator4.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));


      gum::learning::DBTranslator4ContinuousVariable translator5(var, missing4, true);
      CHECK(!translator5.missingSymbols().exists("2"));
      CHECK(!translator5.missingSymbols().exists("4"));
      CHECK((translator5.translate("-10").cont_val) == (std::numeric_limits< float >::max()));
      CHECK_THROWS_AS(translator5.translate("-11"), const gum::OperationNotAllowed&);
      CHECK((translator5.translate("220").cont_val) == (220.0f));
      CHECK((translator5.translate("20").cont_val) == (20.0f));

      CHECK((translator5.needsReordering()) == (false));
      CHECK(translator5.reorder().empty());
    }   // namespace gum_tests

    static void test_trans2() {
      {
        gum::ContinuousVariable< double > var("X2", "", -2, 10);
        std::vector< std::string >        missing{"2", "N/A", "20", "4", "xxx", "-10"};

        gum::learning::DBTranslator4ContinuousVariable translator(var, missing, true);
        CHECK(!translator.missingSymbols().exists("2"));
        CHECK(!translator.missingSymbols().exists("4"));
        CHECK((translator.translate("-10").cont_val) == (std::numeric_limits< float >::max()));

        CHECK(std::stof(translator.translateBack(gum::learning::DBTranslatedValue{5.3f})) == 5.3f);
        CHECK_THROWS_AS(translator.translateBack(gum::learning::DBTranslatedValue{50.0f}),
                        const gum::UnknownLabelInDatabase&);
        CHECK_THROWS_AS(translator.translateBack(gum::learning::DBTranslatedValue{-50.0f}),
                        const gum::UnknownLabelInDatabase&);
        CHECK(translator.missingSymbols().exists(translator.translateBack(
            gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));

        gum::learning::DBTranslator4ContinuousVariable translator2(translator);
        CHECK(!translator2.missingSymbols().exists("2"));
        CHECK(!translator2.missingSymbols().exists("4"));
        CHECK((translator2.translate("-10").cont_val) == (std::numeric_limits< float >::max()));

        CHECK(std::stof(translator2.translateBack(gum::learning::DBTranslatedValue{5.3f})) == 5.3f);
        CHECK_THROWS_AS(translator2.translateBack(gum::learning::DBTranslatedValue{50.0f}),
                        const gum::UnknownLabelInDatabase&);
        CHECK_THROWS_AS(translator2.translateBack(gum::learning::DBTranslatedValue{-50.0f}),
                        const gum::UnknownLabelInDatabase&);
        CHECK(translator2.missingSymbols().exists(translator2.translateBack(
            gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));

        gum::learning::DBTranslator4ContinuousVariable translator3(translator2);
        CHECK(!translator3.missingSymbols().exists("2"));
        CHECK(!translator3.missingSymbols().exists("4"));
        CHECK((translator3.translate("-10").cont_val) == (std::numeric_limits< float >::max()));

        CHECK(std::stof(translator3.translateBack(gum::learning::DBTranslatedValue{5.3f})) == 5.3f);
        CHECK_THROWS_AS(translator3.translateBack(gum::learning::DBTranslatedValue{50.0f}),
                        const gum::UnknownLabelInDatabase&);
        CHECK_THROWS_AS(translator3.translateBack(gum::learning::DBTranslatedValue{-50.0f}),
                        const gum::UnknownLabelInDatabase&);
        CHECK(translator3.missingSymbols().exists(translator3.translateBack(
            gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));

        gum::learning::DBTranslator4ContinuousVariable translator4(std::move(translator3));
        CHECK(!translator4.missingSymbols().exists("2"));
        CHECK(!translator4.missingSymbols().exists("4"));
        CHECK((translator4.translate("-10").cont_val) == (std::numeric_limits< float >::max()));

        CHECK(std::stof(translator4.translateBack(gum::learning::DBTranslatedValue{5.3f})) == 5.3f);
        CHECK_THROWS_AS(translator4.translateBack(gum::learning::DBTranslatedValue{50.0f}),
                        const gum::UnknownLabelInDatabase&);
        CHECK_THROWS_AS(translator4.translateBack(gum::learning::DBTranslatedValue{-50.0f}),
                        const gum::UnknownLabelInDatabase&);
        CHECK(translator4.missingSymbols().exists(translator4.translateBack(
            gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));

        gum::learning::DBTranslator4ContinuousVariable* translator5 = translator4.clone();

        CHECK(!translator5->missingSymbols().exists("2"));
        CHECK(!translator5->missingSymbols().exists("4"));
        CHECK((translator5->translate("-10").cont_val) == (std::numeric_limits< float >::max()));

        CHECK(std::stof(translator5->translateBack(gum::learning::DBTranslatedValue{5.3f}))
              == 5.3f);
        CHECK_THROWS_AS(translator5->translateBack(gum::learning::DBTranslatedValue{50.0f}),
                        const gum::UnknownLabelInDatabase&);
        CHECK_THROWS_AS(translator5->translateBack(gum::learning::DBTranslatedValue{-50.0f}),
                        const gum::UnknownLabelInDatabase&);
        CHECK(translator5->missingSymbols().exists(translator5->translateBack(
            gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));

        delete translator5;

        CHECK((translator4.translate("12").cont_val) == (12.0f));
        CHECK((translator4.variable()->lowerBoundAsDouble()) == (-2.0f));
        CHECK((translator4.variable()->upperBoundAsDouble()) == (12.0f));
        translator4 = translator2;
        CHECK((translator4.variable()->lowerBoundAsDouble()) == (-2.0f));
        CHECK((translator4.variable()->upperBoundAsDouble()) == (10.0f));

        CHECK((translator.translate("12").cont_val) == (12.0f));
        CHECK((translator.variable()->lowerBoundAsDouble()) == (-2.0f));
        CHECK((translator.variable()->upperBoundAsDouble()) == (12.0f));
        translator = translator2;
        CHECK((translator.variable()->lowerBoundAsDouble()) == (-2.0f));
        CHECK((translator.variable()->upperBoundAsDouble()) == (10.0f));

        CHECK((translator.translate("12").cont_val) == (12.0f));
        CHECK((translator.variable()->lowerBoundAsDouble()) == (-2.0f));
        CHECK((translator.variable()->upperBoundAsDouble()) == (12.0f));
        translator = std::move(translator2);
        CHECK((translator.variable()->lowerBoundAsDouble()) == (-2.0f));
        CHECK((translator.variable()->upperBoundAsDouble()) == (10.0f));
      }
    }
  };

  GUM_TEST_ACTIF(_trans1)
  GUM_TEST_ACTIF(_trans2)

} /* namespace gum_tests */
