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

#include <agrum/base/database/DBTranslator4LabelizedVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DBTranslator4LabelizedVariable
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct DBTranslator4LabelizedVariableTestSuite {
    public:
    static void test_trans1() {
      gum::learning::DBTranslator4LabelizedVariable translator;
      CHECK(translator.isLossless());
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("toto"));
      CHECK((translator.translate("toto").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("toto").discr_val) == ((std::size_t)0));

      CHECK((translator.missingValue().discr_val) == (std::numeric_limits< std::size_t >::max()));

      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("titi"));
      CHECK((translator.translate("toto").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("titi").discr_val) == ((std::size_t)1));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
            == ("toto"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
            == ("titi"));

      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable translator2(missing);
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator2.translate("toto"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator2.translate("titi"));
      CHECK((translator2.translate("toto").discr_val) == ((std::size_t)0));
      CHECK((translator2.translate("titi").discr_val) == ((std::size_t)1));
      CHECK((translator2.translate("N/A").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator2.translate("?").discr_val) == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator2.translate("???").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator2.translate("??").discr_val) == ((std::size_t)2));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
            == ("toto"));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
            == ("titi"));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
            == ("??"));

      gum::learning::DBTranslator4LabelizedVariable translator3(missing, 3);
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator3.translate("toto"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator3.translate("titi"));
      CHECK((translator3.translate("toto").discr_val) == ((std::size_t)0));
      CHECK((translator3.translate("titi").discr_val) == ((std::size_t)1));
      CHECK((translator3.translate("N/A").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator3.translate("?").discr_val) == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator3.translate("???").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator3.translate("??").discr_val) == ((std::size_t)2));
      CHECK_THROWS_AS(translator3.translate("a"), const gum::SizeError&);

      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
            == ("toto"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
            == ("titi"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
            == ("??"));
      CHECK_THROWS_AS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}),
                      const gum::UnknownLabelInDatabase&);
      const std::string back = translator3.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()});
      CHECK(translator3.missingSymbols().exists(back));
    }   // namespace gum_tests

    static void test_trans2() {
      gum::learning::DBTranslator4LabelizedVariable translator;
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("toto"));
      CHECK((translator.translate("toto").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("toto").discr_val) == ((std::size_t)0));

      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("titi"));
      CHECK((translator.translate("toto").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("titi").discr_val) == ((std::size_t)1));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
            == ("toto"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
            == ("titi"));

      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable translator2(missing);
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator2.translate("toto"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator2.translate("titi"));
      CHECK((translator2.translate("toto").discr_val) == ((std::size_t)0));
      CHECK((translator2.translate("titi").discr_val) == ((std::size_t)1));
      CHECK((translator2.translate("N/A").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator2.translate("?").discr_val) == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator2.translate("???").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator2.translate("??").discr_val) == ((std::size_t)2));

      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
            == ("toto"));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
            == ("titi"));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
            == ("??"));

      gum::learning::DBTranslator4LabelizedVariable translator3(missing, 3);
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator3.translate("toto"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator3.translate("titi"));
      CHECK((translator3.translate("toto").discr_val) == ((std::size_t)0));
      CHECK((translator3.translate("titi").discr_val) == ((std::size_t)1));
      CHECK((translator3.translate("N/A").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator3.translate("?").discr_val) == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator3.translate("???").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator3.translate("??").discr_val) == ((std::size_t)2));
      CHECK_THROWS_AS(translator3.translate("a"), const gum::SizeError&);

      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
            == ("toto"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
            == ("titi"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
            == ("??"));
      CHECK_THROWS_AS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}),
                      const gum::UnknownLabelInDatabase&);
      CHECK(translator3.translateBack(
                gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()})
            == "?");
    }

    static void test_trans3() {
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("toto");
      var.addLabel("titi");
      var.addLabel("tutu");

      gum::learning::DBTranslator4LabelizedVariable translator(var);
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("toto"));
      CHECK((translator.translate("toto").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("toto").discr_val) == ((std::size_t)0));

      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("tutu"));
      CHECK((translator.translate("toto").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("tutu").discr_val) == ((std::size_t)2));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
            == ("toto"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
            == ("titi"));

      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable translator2(var, missing);
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator2.translate("toto"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator2.translate("titi"));
      CHECK((translator2.translate("toto").discr_val) == ((std::size_t)0));
      CHECK((translator2.translate("titi").discr_val) == ((std::size_t)1));
      CHECK((translator2.translate("N/A").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator2.translate("?").discr_val) == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator2.translate("???").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK_THROWS_AS(translator2.translate("??"), const gum::UnknownLabelInDatabase&);
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
            == ("toto"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
            == ("titi"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
            == ("tutu"));

      gum::learning::DBTranslator4LabelizedVariable translator3(var, missing, true, 3);
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator3.translate("toto"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator3.translate("titi"));
      CHECK((translator3.translate("toto").discr_val) == ((std::size_t)0));
      CHECK((translator3.translate("titi").discr_val) == ((std::size_t)1));
      CHECK((translator3.translate("N/A").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator3.translate("?").discr_val) == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator3.translate("???").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK_THROWS_AS(translator3.translate("??"), const gum::SizeError&);
      CHECK_THROWS_AS(translator3.translate("a"), const gum::SizeError&);

      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
            == ("toto"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
            == ("titi"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
            == ("tutu"));
      CHECK_THROWS_AS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}),
                      const gum::UnknownLabelInDatabase&);
      CHECK((translator3.translateBack(
                gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()}))
            == ("?"));

      CHECK((translator3.domainSize()) == (static_cast< gum::Size >(3)));

      CHECK_THROWS_AS(
          gum::learning::DBTranslator4LabelizedVariable translator4(var, missing, true, 2),
          const gum::SizeError&);
    }

    static void test_trans4() {
      {
        gum::LabelizedVariable var("X1", "", 0);
        var.addLabel("1");
        var.addLabel("4");
        var.addLabel("2");

        gum::learning::DBTranslator4LabelizedVariable translator(var);
        CHECK((translator.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator.translate("4").discr_val) == ((std::size_t)1));
        CHECK((translator.translate("2").discr_val) == ((std::size_t)2));
        CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("1"));
        CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("4"));
        CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("2"));
        CHECK((translator.variable()->toString()) == ("X1:Labelized({1|4|2})"));

        gum::LabelizedVariable var2("X2", "", 0);
        var2.addLabel("1");
        var2.addLabel("2");
        var2.addLabel("4");

        gum::learning::DBTranslator4LabelizedVariable translator2(var2);
        CHECK((translator2.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator2.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator2.translate("4").discr_val) == ((std::size_t)2));
        CHECK(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "1");
        CHECK(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}) == "2");
        CHECK(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}) == "4");
        CHECK((translator2.variable()->toString()) == ("X2:Labelized({1|2|4})"));

        gum::learning::DBTranslator4LabelizedVariable translator3(translator);
        CHECK((translator3.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator3.translate("4").discr_val) == ((std::size_t)1));
        CHECK((translator3.translate("2").discr_val) == ((std::size_t)2));
        CHECK(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "1");
        CHECK(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}) == "4");
        CHECK(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}) == "2");
        CHECK((translator3.variable()->toString()) == ("X1:Labelized({1|4|2})"));

        gum::learning::DBTranslator4LabelizedVariable translator4(translator2);
        CHECK((translator4.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator4.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator4.translate("4").discr_val) == ((std::size_t)2));
        CHECK(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "1");
        CHECK(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}) == "2");
        CHECK(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}) == "4");
        CHECK((translator4.variable()->toString()) == ("X2:Labelized({1|2|4})"));

        gum::learning::DBTranslator4LabelizedVariable translator5(std::move(translator3));
        CHECK((translator5.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator5.translate("4").discr_val) == ((std::size_t)1));
        CHECK((translator5.translate("2").discr_val) == ((std::size_t)2));
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "1");
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}) == "4");
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}) == "2");
        CHECK((translator5.variable()->toString()) == ("X1:Labelized({1|4|2})"));

        gum::learning::DBTranslator4LabelizedVariable translator6(std::move(translator4));
        CHECK((translator6.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator6.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator6.translate("4").discr_val) == ((std::size_t)2));
        CHECK(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "1");
        CHECK(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}) == "2");
        CHECK(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}) == "4");
        CHECK((translator6.variable()->toString()) == ("X2:Labelized({1|2|4})"));

        gum::learning::DBTranslator4LabelizedVariable* translator7 = translator6.clone();
        CHECK((translator7->translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator7->translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator7->translate("4").discr_val) == ((std::size_t)2));
        CHECK(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "1");
        CHECK(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}) == "2");
        CHECK(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}) == "4");
        CHECK((translator7->variable()->toString()) == ("X2:Labelized({1|2|4})"));

        delete translator7;

        translator5 = translator6;
        CHECK((translator5.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator5.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator5.translate("4").discr_val) == ((std::size_t)2));
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "1");
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}) == "2");
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}) == "4");
        CHECK((translator5.variable()->toString()) == ("X2:Labelized({1|2|4})"));

        translator3 = translator5;
        CHECK((translator3.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator3.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator3.translate("4").discr_val) == ((std::size_t)2));
        CHECK(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "1");
        CHECK(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}) == "2");
        CHECK(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}) == "4");
        CHECK((translator3.variable()->toString()) == ("X2:Labelized({1|2|4})"));

        translator5 = std::move(translator3);
        CHECK((translator5.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator5.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator5.translate("4").discr_val) == ((std::size_t)2));
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "1");
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}) == "2");
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}) == "4");
        CHECK((translator5.variable()->toString()) == ("X2:Labelized({1|2|4})"));
      }
    }

    void xtest_trans5() {
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("1");
      var.addLabel("4");
      var.addLabel("2");

      gum::learning::DBTranslator4LabelizedVariable translator(var, false, 1000);

      CHECK((translator.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator.translate("2").discr_val) == ((std::size_t)2));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("1"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("4"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("2"));
      CHECK((translator.variable()->toString()) == ("X1:Labelized({1|4|2})"));

      gum::LabelizedVariable var2("X2", "", 0);
      var2.addLabel("1");
      var2.addLabel("2");
      var2.addLabel("4");

      gum::learning::DBTranslator4LabelizedVariable translator2(var2);
      CHECK((translator2.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator2.translate("2").discr_val) == ((std::size_t)1));
      CHECK((translator2.translate("4").discr_val) == ((std::size_t)2));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("1"));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("2"));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("4"));
      CHECK((translator2.variable()->toString()) == ("X2:Labelized({1|2|4})"));

      gum::learning::DBTranslator4LabelizedVariable translator3(translator);
      CHECK((translator3.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator3.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator3.translate("2").discr_val) == ((std::size_t)2));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("1"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("4"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("2"));
      CHECK((translator3.variable()->toString()) == ("X1:Labelized({1|4|2})"));

      gum::learning::DBTranslator4LabelizedVariable translator4(translator2);
      CHECK((translator4.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator4.translate("2").discr_val) == ((std::size_t)1));
      CHECK((translator4.translate("4").discr_val) == ((std::size_t)2));
      CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("1"));
      CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("2"));
      CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("4"));
      CHECK((translator4.variable()->toString()) == ("X2:Labelized({1|2|4})"));

      gum::learning::DBTranslator4LabelizedVariable translator5(std::move(translator3));
      CHECK((translator5.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator5.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator5.translate("2").discr_val) == ((std::size_t)2));
      CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("1"));
      CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("4"));
      CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("2"));
      CHECK((translator5.variable()->toString()) == ("X1:Labelized({1|4|2})"));

      gum::learning::DBTranslator4LabelizedVariable translator6(std::move(translator4));
      CHECK((translator6.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator6.translate("2").discr_val) == ((std::size_t)1));
      CHECK((translator6.translate("4").discr_val) == ((std::size_t)2));
      CHECK((translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("1"));
      CHECK((translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("2"));
      CHECK((translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("4"));
      CHECK((translator6.variable()->toString()) == ("X2:Labelized({1|2|4})"));

      gum::learning::DBTranslator4LabelizedVariable* translator7 = translator6.clone();
      CHECK((translator7->translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator7->translate("2").discr_val) == ((std::size_t)1));
      CHECK((translator7->translate("4").discr_val) == ((std::size_t)2));
      CHECK(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "1");
      CHECK(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}) == "2");
      CHECK(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}) == "4");
      CHECK((translator7->variable()->toString()) == ("X2:Labelized({1|2|4})"));

      delete translator7;

      translator5 = translator6;
      CHECK((translator5.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator5.translate("2").discr_val) == ((std::size_t)1));
      CHECK((translator5.translate("4").discr_val) == ((std::size_t)2));
      CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("1"));
      CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("2"));
      CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("4"));
      CHECK((translator5.variable()->toString()) == ("X2:Labelized({1|2|4})"));

      translator3 = translator5;
      CHECK((translator3.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator3.translate("2").discr_val) == ((std::size_t)1));
      CHECK((translator3.translate("4").discr_val) == ((std::size_t)2));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("1"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("2"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("4"));
      CHECK((translator3.variable()->toString()) == ("X2:Labelized({1|2|4})"));

      translator5 = std::move(translator3);
      CHECK((translator5.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator5.translate("2").discr_val) == ((std::size_t)1));
      CHECK((translator5.translate("4").discr_val) == ((std::size_t)2));
      CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("1"));
      CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("2"));
      CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("4"));
      CHECK((translator5.variable()->toString()) == ("X2:Labelized({1|2|4})"));
    }

    static void test_trans6() {
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("toto");
      var.addLabel("titi");
      var.addLabel("???");

      std::vector< std::string > missing{"?", "N/A", "???"};

      gum::learning::DBTranslator4LabelizedVariable translator(var, missing);
      CHECK((translator.translate("toto").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("toto").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("N/A").discr_val) == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator.translate("?").discr_val) == (std::numeric_limits< std::size_t >::max()));
      CHECK((translator.translate("???").discr_val) == ((std::size_t)2));

      CHECK(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "toto");
      CHECK(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}) == "titi");
      CHECK(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}) == "???");
    }

    static void test_trans7() {
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("7");
      var.addLabel("1");
      var.addLabel("3");

      std::vector< std::string > missing{"7", "N/A", "toto"};

      gum::learning::DBTranslator4LabelizedVariable translator(var, missing);
      CHECK(translator.needsReordering());
      CHECK((translator.domainSize()) == (static_cast< gum::Size >(3)));

      CHECK(((translator << "7").discr_val) == ((std::size_t)0));
      CHECK(((translator >> gum::learning::DBTranslatedValue{std::size_t{0}})) == ("7"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("7"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("1"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("3"));

      std::string back = translator.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()});

      gum::Set< std::string > missing_kept{"N/A", "toto"};
      CHECK(missing_kept.exists(back));
      CHECK((translator.missingSymbols()) == (missing_kept));

      auto new_order = translator.reorder();

      CHECK((new_order[0]) == ((std::size_t)2));
      CHECK((new_order[1]) == ((std::size_t)0));
      CHECK((new_order[2]) == ((std::size_t)1));

      CHECK((translator.translate("7").discr_val) == ((std::size_t)2));
      CHECK((translator.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("3").discr_val) == ((std::size_t)1));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("1"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("3"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("7"));


      gum::LabelizedVariable var2("X2", "", 0);
      var2.addLabel("1");
      var2.addLabel("3");
      var2.addLabel("7");

      std::vector< std::string > missing2{"7", "N/A", "toto"};


      gum::learning::DBTranslator4LabelizedVariable translator2(var2, missing2);
      CHECK((translator2.needsReordering()) == (false));
      CHECK((translator2.domainSize()) == (static_cast< gum::Size >(3)));

      CHECK(((translator2 << "7").discr_val) == ((std::size_t)2));
      CHECK(((translator2 >> gum::learning::DBTranslatedValue{std::size_t{2}})) == ("7"));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("1"));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("3"));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("7"));
      back = translator2.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()});
      CHECK(translator2.missingSymbols().exists(back));

      gum::Set< std::string > missing_kept2{"N/A", "toto"};
      CHECK((translator2.missingSymbols()) == (missing_kept2));

      auto new_order2 = translator2.reorder();
      CHECK((new_order2.size()) == (static_cast< gum::Size >(0)));
    }
  };

  GUM_TEST_ACTIF(_trans1)
  GUM_TEST_ACTIF(_trans2)
  GUM_TEST_ACTIF(_trans3)
  GUM_TEST_ACTIF(_trans4)
  GUM_TEST_ACTIF(_trans6)
  GUM_TEST_ACTIF(_trans7)

} /* namespace gum_tests */
