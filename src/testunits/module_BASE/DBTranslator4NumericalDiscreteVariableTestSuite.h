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

#include <agrum/base/database/DBTranslator4NumericalDiscreteVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DBTransl4NumerDiscrVaria
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct DBTransl4NumerDiscrVariaTestSuite {
    public:
    static void test_trans1() {
      gum::NumericalDiscreteVariable var("X1", "");
      var.addValue(1);
      var.addValue(3.5);
      var.addValue(10);
      var.addValue(12);

      gum::learning::DBTranslator4NumericalDiscreteVariable translator(var);
      CHECK(translator.isLossless());
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("1"));
      CHECK((translator.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("3.5").discr_val) == ((std::size_t)1));
      CHECK((translator.translate("10").discr_val) == ((std::size_t)2));
      CHECK_THROWS_AS(translator.translate("0"), const gum::UnknownLabelInDatabase&);
      CHECK_THROWS_AS(translator.translate("11"), const gum::UnknownLabelInDatabase&);
      CHECK_THROWS_AS(translator.translate("aaa"), const gum::TypeError&);

      CHECK((translator.missingValue().discr_val) == (std::numeric_limits< std::size_t >::max()));

      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("12"));
      CHECK(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "1");
      CHECK(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}) == "3.5");

      const auto& tr_var = *(translator.variable());
      int         good   = 1;
      try {
        const gum::NumericalDiscreteVariable& xvar_discr
            = dynamic_cast< const gum::NumericalDiscreteVariable& >(tr_var);
        CHECK((xvar_discr.domainSize()) == (static_cast< gum::Size >(4)));
        CHECK((xvar_discr.label(0)) == ("1"));
        CHECK((xvar_discr.label(1)) == ("3.5"));
        CHECK((xvar_discr.label(2)) == ("10"));
        CHECK((xvar_discr.label(3)) == ("12"));
      } catch (std::bad_cast&) { good = 0; }
      CHECK((good) == (1));

      std::vector< std::string >                            missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4NumericalDiscreteVariable translator2(var, missing);
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator2.translate("1"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator2.translate("12"));
      CHECK((translator2.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator2.translate("3.5").discr_val) == ((std::size_t)1));
      CHECK((translator2.translate("N/A").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator2.translate("?").discr_val) == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator2.translate("???").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK_THROWS_AS(translator2.translate("??"), const gum::TypeError&);
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("1"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("10"));

      gum::learning::DBTranslator4NumericalDiscreteVariable translator3(var, missing, 4);
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator3.translate("1"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator3.translate("10"));
      CHECK((translator3.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator3.translate("3.5").discr_val) == ((std::size_t)1));
      CHECK((translator3.translate("N/A").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator3.translate("?").discr_val) == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator3.translate("???").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK_THROWS_AS(translator3.translate("??"), const gum::TypeError&);
      CHECK_THROWS_AS(translator3.translate("a"), const gum::TypeError&);

      CHECK(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "1");
      CHECK(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}) == "3.5");
      CHECK_THROWS_AS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}}),
                      const gum::UnknownLabelInDatabase&);
      CHECK((translator3.translateBack(
                gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()}))
            == ("?"));

      CHECK((translator3.domainSize()) == (static_cast< gum::Size >(4)));

      CHECK_THROWS_AS(
          gum::learning::DBTranslator4NumericalDiscreteVariable translator4(var, missing, 1),
          const gum::SizeError&);

      CHECK((translator3.variable()->toString()) == ("X1:NumericalDiscrete({1|3.5|10|12})"));
      CHECK((translator3.domainSize()) == (static_cast< gum::Size >(4)));
      CHECK(!translator3.hasEditableDictionary());
      translator3.setEditableDictionaryMode(true);
      CHECK(!translator3.hasEditableDictionary());
      CHECK(!translator3.needsReordering());
      CHECK(translator3.reorder().empty());
    }   // namespace gum_tests

    static void test_trans2() {
      {
        gum::NumericalDiscreteVariable var("X1", "");
        var.addValue(1);
        var.addValue(3.5);
        var.addValue(10);
        var.addValue(12);

        gum::learning::DBTranslator4NumericalDiscreteVariable translator(var);
        CHECK((translator.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator.translate("12").discr_val) == ((std::size_t)3));
        CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("1"));
        CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("3.5"));
        CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("10"));
        CHECK((translator.variable()->toString()) == ("X1:NumericalDiscrete({1|3.5|10|12})"));

        CHECK((translator.translate(translator.translateBack(translator.translate("1"))).discr_val)
              == ((std::size_t)0));

        gum::NumericalDiscreteVariable var2("X2", "");
        var2.addValue(1);
        var2.addValue(2);
        var2.addValue(3.5);
        var2.addValue(4);

        gum::learning::DBTranslator4NumericalDiscreteVariable translator2(var2);
        CHECK((translator2.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator2.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator2.translate("4").discr_val) == ((std::size_t)3));
        CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("1"));
        CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("2"));
        CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("3.5"));
        CHECK((translator2.variable()->toString()) == ("X2:NumericalDiscrete({1|2|3.5|4})"));

        gum::learning::DBTranslator4NumericalDiscreteVariable translator3(translator);
        CHECK((translator3.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator3.translate("10").discr_val) == ((std::size_t)2));
        CHECK((translator3.translate("12").discr_val) == ((std::size_t)3));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("1"));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("3.5"));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("10"));
        CHECK((translator3.variable()->toString()) == ("X1:NumericalDiscrete({1|3.5|10|12})"));

        gum::learning::DBTranslator4NumericalDiscreteVariable translator4(translator2);
        CHECK((translator4.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator4.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator4.translate("4").discr_val) == ((std::size_t)3));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("1"));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("2"));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("3.5"));
        CHECK((translator4.variable()->toString()) == ("X2:NumericalDiscrete({1|2|3.5|4})"));

        gum::learning::DBTranslator4NumericalDiscreteVariable translator5(std::move(translator3));
        CHECK((translator5.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator5.translate("10").discr_val) == ((std::size_t)2));
        CHECK((translator5.translate("12").discr_val) == ((std::size_t)3));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("1"));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("3.5"));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("10"));
        CHECK((translator5.variable()->toString()) == ("X1:NumericalDiscrete({1|3.5|10|12})"));

        gum::learning::DBTranslator4NumericalDiscreteVariable translator6(std::move(translator4));
        CHECK((translator6.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator6.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator6.translate("4").discr_val) == ((std::size_t)3));
        CHECK((translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("1"));
        CHECK((translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("2"));
        CHECK((translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("3.5"));
        CHECK((translator6.variable()->toString()) == ("X2:NumericalDiscrete({1|2|3.5|4})"));

        gum::learning::DBTranslator4NumericalDiscreteVariable* translator7 = translator6.clone();
        CHECK((translator7->translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator7->translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator7->translate("4").discr_val) == ((std::size_t)3));
        CHECK((translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("1"));
        CHECK((translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("2"));
        CHECK((translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("3.5"));
        CHECK((translator7->variable()->toString()) == ("X2:NumericalDiscrete({1|2|3.5|4})"));

        delete translator7;

        gum::learning::DBTranslator4NumericalDiscreteVariable translator8(var);
        translator8 = translator6;
        CHECK((translator8.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator8.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator8.translate("4").discr_val) == ((std::size_t)3));
        CHECK((translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("1"));
        CHECK((translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("2"));
        CHECK((translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("3.5"));
        CHECK((translator8.variable()->toString()) == ("X2:NumericalDiscrete({1|2|3.5|4})"));

        gum::learning::DBTranslator4NumericalDiscreteVariable translator8bis(var);
        gum::learning::DBTranslator4NumericalDiscreteVariable translator9(var2);
        translator9 = translator8bis;
        CHECK((translator9.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator9.translate("3.5").discr_val) == ((std::size_t)1));
        CHECK((translator9.translate("10").discr_val) == ((std::size_t)2));
        CHECK((translator9.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("1"));
        CHECK((translator9.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("3.5"));
        CHECK((translator9.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("10"));
        CHECK((translator9.variable()->toString()) == ("X1:NumericalDiscrete({1|3.5|10|12})"));

        translator8 = std::move(translator9);
        CHECK((translator8.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator8.translate("3.5").discr_val) == ((std::size_t)1));
        CHECK((translator8.translate("10").discr_val) == ((std::size_t)2));
        CHECK((translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("1"));
        CHECK((translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("3.5"));
        CHECK((translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("10"));
        CHECK((translator8.variable()->toString()) == ("X1:NumericalDiscrete({1|3.5|10|12})"));

        CHECK_THROWS_AS(translator8.translate("5"), gum::UnknownLabelInDatabase&);
        GUM_CHECK_ASSERT_THROWS_NOTHING(translator8.translate("1.0"));
      }
    }
  };

  GUM_TEST_ACTIF(_trans1)
  GUM_TEST_ACTIF(_trans2)

} /* namespace gum_tests */
