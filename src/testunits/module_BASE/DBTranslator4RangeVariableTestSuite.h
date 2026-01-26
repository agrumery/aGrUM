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

#include <agrum/base/database/DBTranslator4RangeVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DBTranslator4RangeVariable
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct DBTranslator4RangeVariableTestSuite {
    public:
    static void test_trans1() {
      gum::learning::DBTranslator4RangeVariable translator;
      CHECK(translator.isLossless());
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("3"));
      CHECK((translator.translate("3").discr_val) == ((std::size_t)0));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("3"));

      CHECK((translator.missingValue().discr_val) == (std::numeric_limits< std::size_t >::max()));

      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("5"));
      CHECK((translator.translate("5").discr_val) == ((std::size_t)2));
      CHECK((translator.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("3"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("4"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("5"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("1"));
      CHECK((translator.translate("1").discr_val) == ((std::size_t)3));
      CHECK((translator.translate("2").discr_val) == ((std::size_t)4));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}})) == ("1"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}})) == ("2"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("-1"));
      CHECK((translator.translate("-1").discr_val) == ((std::size_t)5));
      CHECK((translator.translate("0").discr_val) == ((std::size_t)6));
      CHECK((translator.translate("1").discr_val) == ((std::size_t)3));
      CHECK((translator.translate("2").discr_val) == ((std::size_t)4));
      CHECK((translator.translate("3").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator.translate("5").discr_val) == ((std::size_t)2));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("3"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("4"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("5"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}})) == ("1"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}})) == ("2"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}})) == ("-1"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{6}})) == ("0"));

      std::vector< std::string > missing{"2", "N/A", "20", "4", "xxx"};

      gum::learning::DBTranslator4RangeVariable translator2(missing);
      CHECK((translator2.translate("3").discr_val) == ((std::size_t)0));
      CHECK((translator2.translate("2").discr_val) == (std::numeric_limits< std::size_t >::max()));
      CHECK(translator2.translate("N/A").discr_val == std::numeric_limits< std::size_t >::max());
      CHECK(translator2.translate("xxx").discr_val == std::numeric_limits< std::size_t >::max());
      CHECK_THROWS_AS(translator2.translate("yyy"), const gum::TypeError&);
      CHECK_THROWS_AS(translator2.translate("1"), const gum::OperationNotAllowed&);
      CHECK((translator2.translate("5").discr_val) == ((std::size_t)2));
      CHECK((translator2.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("3"));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("4"));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("5"));

      gum::Set< std::string > possible_translated_miss{"N/A", "xxx"};
      CHECK(possible_translated_miss.exists(translator2.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()})));
      CHECK((translator2.needsReordering()) == (false));
      const auto new_order = translator2.reorder();
      CHECK((new_order.size()) == (static_cast< gum::Size >(0)));

      gum::RangeVariable                        var("X2", "", 2, 3);
      gum::learning::DBTranslator4RangeVariable translator3(var, missing, true);
      CHECK((translator3.translate("2").discr_val) == ((std::size_t)0));
      CHECK((translator3.translate("5").discr_val) == ((std::size_t)3));
      CHECK((translator3.translate("4").discr_val) == ((std::size_t)2));
      CHECK((translator3.translate("0").discr_val) == ((std::size_t)4));
      CHECK((translator3.translate("1").discr_val) == ((std::size_t)5));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("2"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("3"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("4"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}})) == ("5"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}})) == ("0"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}})) == ("1"));
      CHECK(translator3.needsReordering());
      const auto new_order3 = translator3.reorder();
      CHECK((new_order3[0]) == ((std::size_t)2));
      CHECK((new_order3[1]) == ((std::size_t)3));
      CHECK((new_order3[2]) == ((std::size_t)4));
      CHECK((new_order3[3]) == ((std::size_t)5));
      CHECK((new_order3[4]) == ((std::size_t)0));
      CHECK((new_order3[5]) == ((std::size_t)1));

      CHECK((translator3.translate("0").discr_val) == ((std::size_t)0));
      CHECK((translator3.translate("1").discr_val) == ((std::size_t)1));
      CHECK((translator3.translate("2").discr_val) == ((std::size_t)2));
      CHECK((translator3.translate("3").discr_val) == ((std::size_t)3));
      CHECK((translator3.translate("4").discr_val) == ((std::size_t)4));
      CHECK((translator3.translate("5").discr_val) == ((std::size_t)5));


      std::vector< std::string >                missing2;
      gum::learning::DBTranslator4RangeVariable translator4(var, missing2, true, 3);
      CHECK((translator4.translate("2").discr_val) == ((std::size_t)0));
      CHECK_THROWS_AS(translator4.translate("5"), const gum::SizeError&);
      CHECK((translator4.translate("4").discr_val) == ((std::size_t)2));
      CHECK_THROWS_AS(translator4.translate("0"), const gum::SizeError&);
      CHECK_THROWS_AS(translator4.translate("1"), const gum::SizeError&);
      CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("2"));
      CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("3"));
      CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("4"));
      CHECK_THROWS_AS(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}),
                      const gum::UnknownLabelInDatabase&);
    }   // namespace gum_tests

    static void test_trans2() {
      gum::learning::DBTranslator4RangeVariable translator;
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("3"));
      CHECK((translator.translate("3").discr_val) == ((std::size_t)0));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("3"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("5"));
      CHECK((translator.translate("5").discr_val) == ((std::size_t)2));
      CHECK((translator.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("3"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("4"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("5"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("1"));
      CHECK((translator.translate("1").discr_val) == ((std::size_t)3));
      CHECK((translator.translate("2").discr_val) == ((std::size_t)4));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}})) == ("1"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}})) == ("2"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("-1"));
      CHECK((translator.translate("-1").discr_val) == ((std::size_t)5));
      CHECK((translator.translate("0").discr_val) == ((std::size_t)6));
      CHECK((translator.translate("1").discr_val) == ((std::size_t)3));
      CHECK((translator.translate("2").discr_val) == ((std::size_t)4));
      CHECK((translator.translate("3").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator.translate("5").discr_val) == ((std::size_t)2));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("3"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("4"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("5"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}})) == ("1"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}})) == ("2"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}})) == ("-1"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{6}})) == ("0"));

      std::vector< std::string > missing{"2", "N/A", "20", "4", "xxx"};

      gum::learning::DBTranslator4RangeVariable translator2(missing);
      CHECK((translator2.translate("3").discr_val) == ((std::size_t)0));
      CHECK((translator2.translate("2").discr_val) == (std::numeric_limits< std::size_t >::max()));
      CHECK(translator2.translate("N/A").discr_val == std::numeric_limits< std::size_t >::max());
      CHECK(translator2.translate("xxx").discr_val == std::numeric_limits< std::size_t >::max());
      CHECK_THROWS_AS(translator2.translate("yyy"), const gum::TypeError&);
      CHECK_THROWS_AS(translator2.translate("1"), const gum::OperationNotAllowed&);
      CHECK((translator2.translate("5").discr_val) == ((std::size_t)2));
      CHECK((translator2.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("3"));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("4"));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("5"));

      gum::Set< std::string > possible_translated_miss{"N/A", "xxx"};
      CHECK(possible_translated_miss.exists(translator2.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()})));
      CHECK((translator2.needsReordering()) == (false));
      const auto new_order = translator2.reorder();
      CHECK((new_order.size()) == (static_cast< gum::Size >(0)));

      gum::RangeVariable                        var("X2", "", 2, 3);
      gum::learning::DBTranslator4RangeVariable translator3(var, missing, true);
      CHECK((translator3.translate("2").discr_val) == ((std::size_t)0));
      CHECK((translator3.translate("5").discr_val) == ((std::size_t)3));
      CHECK((translator3.translate("4").discr_val) == ((std::size_t)2));
      CHECK((translator3.translate("0").discr_val) == ((std::size_t)4));
      CHECK((translator3.translate("1").discr_val) == ((std::size_t)5));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("2"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("3"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("4"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}})) == ("5"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}})) == ("0"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}})) == ("1"));
      CHECK(translator3.needsReordering());
      const auto new_order3 = translator3.reorder();
      CHECK((new_order3[0]) == ((std::size_t)2));
      CHECK((new_order3[1]) == ((std::size_t)3));
      CHECK((new_order3[2]) == ((std::size_t)4));
      CHECK((new_order3[3]) == ((std::size_t)5));
      CHECK((new_order3[4]) == ((std::size_t)0));
      CHECK((new_order3[5]) == ((std::size_t)1));

      CHECK((translator3.translate("0").discr_val) == ((std::size_t)0));
      CHECK((translator3.translate("1").discr_val) == ((std::size_t)1));
      CHECK((translator3.translate("2").discr_val) == ((std::size_t)2));
      CHECK((translator3.translate("4").discr_val) == ((std::size_t)4));
      CHECK((translator3.translate("5").discr_val) == ((std::size_t)5));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("0"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("1"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("2"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}})) == ("3"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}})) == ("4"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}})) == ("5"));


      std::vector< std::string >                missing2;
      gum::learning::DBTranslator4RangeVariable translator4(var, missing2, true, 3);
      CHECK((translator4.translate("2").discr_val) == ((std::size_t)0));
      CHECK_THROWS_AS(translator4.translate("5"), const gum::SizeError&);
      CHECK((translator4.translate("4").discr_val) == ((std::size_t)2));
      CHECK_THROWS_AS(translator4.translate("0"), const gum::SizeError&);
      CHECK_THROWS_AS(translator4.translate("1"), const gum::SizeError&);
      CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("2"));
      CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("3"));
      CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("4"));
      CHECK_THROWS_AS(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}),
                      const gum::UnknownLabelInDatabase&);
    }

    static void test_trans4() {
      {
        gum::RangeVariable var("X1", "", 2, 3);

        std::vector< std::string >                missing;
        gum::learning::DBTranslator4RangeVariable translator(var, missing, true);
        CHECK((translator.translate("2").discr_val) == ((std::size_t)0));
        CHECK((translator.translate("5").discr_val) == ((std::size_t)3));
        CHECK((translator.translate("4").discr_val) == ((std::size_t)2));
        CHECK((translator.translate("0").discr_val) == ((std::size_t)4));
        CHECK((translator.translate("1").discr_val) == ((std::size_t)5));
        CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("2"));
        CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("3"));
        CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("4"));
        CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}))
              == ("5"));
        CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}}))
              == ("0"));
        CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}}))
              == ("1"));
        CHECK(translator.needsReordering());
        const auto new_order = translator.reorder();
        CHECK((new_order[0]) == ((std::size_t)2));
        CHECK((new_order[1]) == ((std::size_t)3));
        CHECK((new_order[2]) == ((std::size_t)4));
        CHECK((new_order[3]) == ((std::size_t)5));
        CHECK((new_order[4]) == ((std::size_t)0));
        CHECK((new_order[5]) == ((std::size_t)1));
        CHECK((translator.variable()->toString()) == ("X1:Range([0,5])"));

        gum::learning::DBTranslator4RangeVariable translator2(var, missing, true);
        CHECK((translator2.translate("2").discr_val) == ((std::size_t)0));
        CHECK((translator2.translate("5").discr_val) == ((std::size_t)3));
        CHECK((translator2.translate("4").discr_val) == ((std::size_t)2));
        CHECK((translator2.translate("0").discr_val) == ((std::size_t)4));
        CHECK((translator2.translate("1").discr_val) == ((std::size_t)5));
        CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("2"));
        CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("3"));
        CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("4"));
        CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}))
              == ("5"));
        CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}}))
              == ("0"));
        CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}}))
              == ("1"));
        CHECK(translator2.needsReordering());
        const auto new_order2 = translator2.reorder();
        CHECK((new_order2[0]) == ((std::size_t)2));
        CHECK((new_order2[1]) == ((std::size_t)3));
        CHECK((new_order2[2]) == ((std::size_t)4));
        CHECK((new_order2[3]) == ((std::size_t)5));
        CHECK((new_order2[4]) == ((std::size_t)0));
        CHECK((new_order2[5]) == ((std::size_t)1));
        CHECK((translator2.variable()->toString()) == ("X1:Range([0,5])"));

        gum::learning::DBTranslator4RangeVariable translator3(translator);
        CHECK((translator3.translate("0").discr_val) == ((std::size_t)0));
        CHECK((translator3.translate("1").discr_val) == ((std::size_t)1));
        CHECK((translator3.translate("2").discr_val) == ((std::size_t)2));
        CHECK((translator3.translate("4").discr_val) == ((std::size_t)4));
        CHECK((translator3.translate("5").discr_val) == ((std::size_t)5));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("0"));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("1"));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("2"));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}))
              == ("3"));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}}))
              == ("4"));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}}))
              == ("5"));

        gum::learning::DBTranslator4RangeVariable translator4(translator2);
        CHECK((translator4.translate("0").discr_val) == ((std::size_t)0));
        CHECK((translator4.translate("1").discr_val) == ((std::size_t)1));
        CHECK((translator4.translate("2").discr_val) == ((std::size_t)2));
        CHECK((translator4.translate("4").discr_val) == ((std::size_t)4));
        CHECK((translator4.translate("5").discr_val) == ((std::size_t)5));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("0"));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("1"));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("2"));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}))
              == ("3"));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}}))
              == ("4"));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}}))
              == ("5"));

        gum::learning::DBTranslator4RangeVariable translator5(std::move(translator3));
        CHECK((translator5.translate("0").discr_val) == ((std::size_t)0));
        CHECK((translator5.translate("1").discr_val) == ((std::size_t)1));
        CHECK((translator5.translate("2").discr_val) == ((std::size_t)2));
        CHECK((translator5.translate("4").discr_val) == ((std::size_t)4));
        CHECK((translator5.translate("5").discr_val) == ((std::size_t)5));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("0"));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("1"));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("2"));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}))
              == ("3"));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}}))
              == ("4"));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}}))
              == ("5"));

        gum::learning::DBTranslator4RangeVariable translator6(std::move(translator4));
        CHECK((translator6.translate("0").discr_val) == ((std::size_t)0));
        CHECK((translator6.translate("1").discr_val) == ((std::size_t)1));
        CHECK((translator6.translate("2").discr_val) == ((std::size_t)2));
        CHECK((translator6.translate("4").discr_val) == ((std::size_t)4));
        CHECK((translator6.translate("5").discr_val) == ((std::size_t)5));
        CHECK((translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("0"));
        CHECK((translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("1"));
        CHECK((translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("2"));
        CHECK((translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}))
              == ("3"));
        CHECK((translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}}))
              == ("4"));
        CHECK((translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}}))
              == ("5"));

        gum::learning::DBTranslator4RangeVariable* translator7 = translator6.clone();
        CHECK((translator7->translate("0").discr_val) == ((std::size_t)0));
        CHECK((translator7->translate("1").discr_val) == ((std::size_t)1));
        CHECK((translator7->translate("2").discr_val) == ((std::size_t)2));
        CHECK((translator7->translate("4").discr_val) == ((std::size_t)4));
        CHECK((translator7->translate("5").discr_val) == ((std::size_t)5));
        CHECK((translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("0"));
        CHECK((translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("1"));
        CHECK((translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("2"));
        CHECK((translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}))
              == ("3"));
        CHECK((translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{4}}))
              == ("4"));
        CHECK((translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{5}}))
              == ("5"));

        delete translator7;

        translator4 = translator6;
        CHECK((translator4.translate("0").discr_val) == ((std::size_t)0));
        CHECK((translator4.translate("1").discr_val) == ((std::size_t)1));
        CHECK((translator4.translate("2").discr_val) == ((std::size_t)2));
        CHECK((translator4.translate("4").discr_val) == ((std::size_t)4));
        CHECK((translator4.translate("5").discr_val) == ((std::size_t)5));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("0"));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("1"));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("2"));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}))
              == ("3"));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}}))
              == ("4"));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}}))
              == ("5"));

        translator3 = translator5;
        CHECK((translator3.translate("0").discr_val) == ((std::size_t)0));
        CHECK((translator3.translate("1").discr_val) == ((std::size_t)1));
        CHECK((translator3.translate("2").discr_val) == ((std::size_t)2));
        CHECK((translator3.translate("4").discr_val) == ((std::size_t)4));
        CHECK((translator3.translate("5").discr_val) == ((std::size_t)5));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("0"));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("1"));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("2"));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}))
              == ("3"));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}}))
              == ("4"));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}}))
              == ("5"));

        translator5 = std::move(translator3);
        CHECK((translator5.translate("0").discr_val) == ((std::size_t)0));
        CHECK((translator5.translate("1").discr_val) == ((std::size_t)1));
        CHECK((translator5.translate("2").discr_val) == ((std::size_t)2));
        CHECK((translator5.translate("4").discr_val) == ((std::size_t)4));
        CHECK((translator5.translate("5").discr_val) == ((std::size_t)5));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("0"));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("1"));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("2"));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}))
              == ("3"));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}}))
              == ("4"));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}}))
              == ("5"));
      }
    }

    void xtest_trans5() {
      gum::learning::DBTranslator4RangeVariable translator(1000);
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("3"));
      CHECK((translator.translate("3").discr_val) == ((std::size_t)0));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("3"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("5"));
      CHECK((translator.translate("5").discr_val) == ((std::size_t)2));
      CHECK((translator.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("3"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("4"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("5"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("1"));
      CHECK((translator.translate("1").discr_val) == ((std::size_t)3));
      CHECK((translator.translate("2").discr_val) == ((std::size_t)4));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}})) == ("1"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}})) == ("2"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("-1"));
      CHECK((translator.translate("-1").discr_val) == ((std::size_t)5));
      CHECK((translator.translate("0").discr_val) == ((std::size_t)6));
      CHECK((translator.translate("1").discr_val) == ((std::size_t)3));
      CHECK((translator.translate("2").discr_val) == ((std::size_t)4));
      CHECK((translator.translate("3").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator.translate("5").discr_val) == ((std::size_t)2));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("3"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("4"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("5"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}})) == ("1"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}})) == ("2"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}})) == ("-1"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{6}})) == ("0"));

      std::vector< std::string > missing{"2", "N/A", "20", "4", "xxx"};

      gum::learning::DBTranslator4RangeVariable translator2(missing);
      CHECK((translator2.translate("3").discr_val) == ((std::size_t)0));
      CHECK((translator2.translate("2").discr_val) == (std::numeric_limits< std::size_t >::max()));
      CHECK(translator2.translate("N/A").discr_val == std::numeric_limits< std::size_t >::max());
      CHECK(translator2.translate("xxx").discr_val == std::numeric_limits< std::size_t >::max());
      CHECK_THROWS_AS(translator2.translate("yyy"), const gum::TypeError&);
      CHECK_THROWS_AS(translator2.translate("1"), const gum::OperationNotAllowed&);
      CHECK((translator2.translate("5").discr_val) == ((std::size_t)2));
      CHECK((translator2.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("3"));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("4"));
      CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("5"));

      gum::Set< std::string > possible_translated_miss{"N/A", "xxx"};
      CHECK(possible_translated_miss.exists(translator2.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()})));
      CHECK((translator2.needsReordering()) == (false));
      const auto new_order = translator2.reorder();
      CHECK((new_order[0]) == ((std::size_t)0));
      CHECK((new_order[1]) == ((std::size_t)1));
      CHECK((new_order[2]) == ((std::size_t)2));

      gum::RangeVariable                        var("X2", "", 2, 3);
      gum::learning::DBTranslator4RangeVariable translator3(var, missing, true);
      CHECK((translator3.translate("2").discr_val) == ((std::size_t)0));
      CHECK((translator3.translate("5").discr_val) == ((std::size_t)3));
      CHECK((translator3.translate("4").discr_val) == ((std::size_t)2));
      CHECK((translator3.translate("0").discr_val) == ((std::size_t)4));
      CHECK((translator3.translate("1").discr_val) == ((std::size_t)5));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("2"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("3"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("4"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}})) == ("5"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}})) == ("0"));
      CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}})) == ("1"));
      CHECK(translator3.needsReordering());
      const auto new_order3 = translator3.reorder();
      CHECK((new_order3[0]) == ((std::size_t)2));
      CHECK((new_order3[1]) == ((std::size_t)3));
      CHECK((new_order3[2]) == ((std::size_t)4));
      CHECK((new_order3[3]) == ((std::size_t)5));
      CHECK((new_order3[4]) == ((std::size_t)0));
      CHECK((new_order3[5]) == ((std::size_t)1));

      std::vector< std::string >                missing2;
      gum::learning::DBTranslator4RangeVariable translator4(var, missing2, true, 3);
      CHECK((translator4.translate("2").discr_val) == ((std::size_t)0));
      CHECK_THROWS_AS(translator4.translate("5"), const gum::SizeError&);
      CHECK((translator4.translate("4").discr_val) == ((std::size_t)2));
      CHECK_THROWS_AS(translator4.translate("0"), const gum::SizeError&);
      CHECK_THROWS_AS(translator4.translate("1"), const gum::SizeError&);
      CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("2"));
      CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("3"));
      CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("4"));
      CHECK_THROWS_AS(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}),
                      const gum::UnknownLabelInDatabase&);

      translator4 = translator3;
      CHECK((translator4.translate("2").discr_val) == ((std::size_t)0));
      CHECK((translator4.translate("5").discr_val) == ((std::size_t)3));
      CHECK((translator4.translate("4").discr_val) == ((std::size_t)2));
      CHECK((translator4.translate("0").discr_val) == ((std::size_t)4));
      CHECK((translator4.translate("1").discr_val) == ((std::size_t)5));

      translator4 = std::move(translator3);
      CHECK((translator4.translate("2").discr_val) == ((std::size_t)0));
      CHECK((translator4.translate("5").discr_val) == ((std::size_t)3));
      CHECK((translator4.translate("4").discr_val) == ((std::size_t)2));
      CHECK((translator4.translate("0").discr_val) == ((std::size_t)4));
      CHECK((translator4.translate("1").discr_val) == ((std::size_t)5));

      gum::learning::DBTranslator4RangeVariable translator5(translator);
      CHECK((translator5.translate("-1").discr_val) == ((std::size_t)5));
      CHECK((translator5.translate("0").discr_val) == ((std::size_t)6));
      CHECK((translator5.translate("1").discr_val) == ((std::size_t)3));
      CHECK((translator5.translate("2").discr_val) == ((std::size_t)4));
      CHECK((translator5.translate("3").discr_val) == ((std::size_t)0));
      CHECK((translator5.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator5.translate("5").discr_val) == ((std::size_t)2));

      gum::learning::DBTranslator4RangeVariable translator6(std::move(translator));
      CHECK((translator6.translate("-1").discr_val) == ((std::size_t)5));
      CHECK((translator6.translate("0").discr_val) == ((std::size_t)6));
      CHECK((translator6.translate("1").discr_val) == ((std::size_t)3));
      CHECK((translator6.translate("2").discr_val) == ((std::size_t)4));
      CHECK((translator6.translate("3").discr_val) == ((std::size_t)0));
      CHECK((translator6.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator6.translate("5").discr_val) == ((std::size_t)2));

      gum::learning::DBTranslator4RangeVariable* translator7 = translator6.clone();
      CHECK((translator7->translate("-1").discr_val) == ((std::size_t)5));
      CHECK((translator7->translate("0").discr_val) == ((std::size_t)6));
      CHECK((translator7->translate("1").discr_val) == ((std::size_t)3));
      CHECK((translator7->translate("2").discr_val) == ((std::size_t)4));
      CHECK((translator7->translate("3").discr_val) == ((std::size_t)0));
      CHECK((translator7->translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator7->translate("5").discr_val) == ((std::size_t)2));

      delete translator7;

      translator5 = translator6;
      CHECK((translator5.translate("-1").discr_val) == ((std::size_t)5));
      CHECK((translator5.translate("0").discr_val) == ((std::size_t)6));
      CHECK((translator5.translate("1").discr_val) == ((std::size_t)3));
      CHECK((translator5.translate("2").discr_val) == ((std::size_t)4));
      CHECK((translator5.translate("3").discr_val) == ((std::size_t)0));
      CHECK((translator5.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator5.translate("5").discr_val) == ((std::size_t)2));

      translator3 = translator5;
      CHECK((translator3.translate("-1").discr_val) == ((std::size_t)5));
      CHECK((translator3.translate("0").discr_val) == ((std::size_t)6));
      CHECK((translator3.translate("1").discr_val) == ((std::size_t)3));
      CHECK((translator3.translate("2").discr_val) == ((std::size_t)4));
      CHECK((translator3.translate("3").discr_val) == ((std::size_t)0));
      CHECK((translator3.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator3.translate("5").discr_val) == ((std::size_t)2));

      translator5 = std::move(translator3);
      CHECK((translator5.translate("-1").discr_val) == ((std::size_t)5));
      CHECK((translator5.translate("0").discr_val) == ((std::size_t)6));
      CHECK((translator5.translate("1").discr_val) == ((std::size_t)3));
      CHECK((translator5.translate("2").discr_val) == ((std::size_t)4));
      CHECK((translator5.translate("3").discr_val) == ((std::size_t)0));
      CHECK((translator5.translate("4").discr_val) == ((std::size_t)1));
      CHECK((translator5.translate("5").discr_val) == ((std::size_t)2));
    }

    static void test_trans6() {
      gum::RangeVariable         var("X1", "", 2, 4);
      std::vector< std::string > missing{"N/A", "3", "toto"};

      gum::learning::DBTranslator4RangeVariable translator(var, missing, true);
      CHECK((translator.needsReordering()) == (false));
      CHECK((translator.domainSize()) == (static_cast< gum::Size >(3)));

      CHECK(((translator << "7").discr_val) == ((std::size_t)5));
      CHECK(((translator >> gum::learning::DBTranslatedValue{std::size_t{5}})) == ("7"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})) == ("2"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})) == ("3"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})) == ("4"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}})) == ("5"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}})) == ("6"));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{5}})) == ("7"));
      CHECK(((translator.translateBack(
                  gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()})
              == "N/A")
             || (translator.translateBack(
                     gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()})
                 == "toto")));

      gum::Set< std::string > missing_kept{"N/A", "toto"};
      CHECK((translator.missingSymbols()) == (missing_kept));
    }
  };

  GUM_TEST_ACTIF(_trans1)
  GUM_TEST_ACTIF(_trans2)
  GUM_TEST_ACTIF(_trans4)
  GUM_TEST_ACTIF(_trans6)

} /* namespace gum_tests */
