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

#include <agrum/base/database/DBTranslator4DiscretizedVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DBTranslator4DiscretizedVariable
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct DBTranslator4DiscretizedVariableTestSuite {
    public:
    static void test_trans1() {
      gum::DiscretizedVariable< int > var("X1", "");
      var.addTick(1);
      var.addTick(3);
      var.addTick(10);

      gum::learning::DBTranslator4DiscretizedVariable translator(var);
      CHECK(!translator.isLossless());
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("1.3"));
      CHECK((translator.translate("1.2").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("2.4").discr_val) == ((std::size_t)0));
      CHECK_THROWS_AS(translator.translate("0"), const gum::UnknownLabelInDatabase&);
      CHECK_THROWS_AS(translator.translate("11"), const gum::UnknownLabelInDatabase&);
      CHECK_THROWS_AS(translator.translate("aaa"), const gum::TypeError&);

      CHECK((translator.missingValue().discr_val)
            == ((std::size_t)std::numeric_limits< std::size_t >::max()));

      GUM_CHECK_ASSERT_THROWS_NOTHING(translator.translate("7"));
      CHECK((translator.translate("10").discr_val) == ((std::size_t)1));
      CHECK((translator.translate("9").discr_val) == ((std::size_t)1));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
            == ("[1;3["));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
            == ("[3;10]"));

      const auto& tr_var     = *(translator.variable());
      int         good_discr = 1;
      try {
        const gum::DiscretizedVariable< int >& xvar_discr
            = dynamic_cast< const gum::DiscretizedVariable< int >& >(tr_var);
        CHECK((xvar_discr.domainSize()) == ((gum::Size) static_cast< gum::Size >(2)));
        CHECK((xvar_discr.label(0)) == ("[1;3["));
        CHECK((xvar_discr.label(1)) == ("[3;10]"));
      } catch (std::bad_cast&) { good_discr = 0; }
      CHECK((good_discr) == (1));

      std::vector< std::string >                      missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4DiscretizedVariable translator2(var, missing);
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator2.translate("1.3"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator2.translate("3"));
      CHECK((translator2.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator2.translate("3").discr_val) == ((std::size_t)1));
      CHECK(translator2.translate("N/A").discr_val == std::numeric_limits< std::size_t >::max());
      CHECK((translator2.translate("?").discr_val)
            == ((std::size_t)std::numeric_limits< std::size_t >::max()));
      CHECK((translator2.translate("???").discr_val)
            == (std::numeric_limits< std::size_t >::max()));
      CHECK_THROWS_AS(translator2.translate("??"), const gum::TypeError&);
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
            == ("[1;3["));
      CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
            == ("[3;10]"));

      gum::learning::DBTranslator4DiscretizedVariable translator3(var, missing, 3);
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator3.translate("1"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(translator3.translate("10"));
      CHECK((translator3.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator3.translate("9.9").discr_val) == ((std::size_t)1));
      CHECK(translator3.translate("N/A").discr_val == std::numeric_limits< std::size_t >::max());
      CHECK((translator3.translate("?").discr_val)
            == ((std::size_t)std::numeric_limits< std::size_t >::max()));
      CHECK(translator3.translate("???").discr_val == std::numeric_limits< std::size_t >::max());
      CHECK_THROWS_AS(translator3.translate("??"), const gum::TypeError&);
      CHECK_THROWS_AS(translator3.translate("a"), const gum::TypeError&);

      CHECK(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "[1;3[");
      CHECK(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
            == "[3;10]");
      CHECK_THROWS_AS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                      const gum::UnknownLabelInDatabase&);
      CHECK(translator3.translateBack(
                gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()})
            == "?");

      CHECK((translator3.domainSize()) == (static_cast< gum::Size >(2)));

      CHECK_THROWS_AS(gum::learning::DBTranslator4DiscretizedVariable translator4(var, missing, 1),
                      const gum::SizeError&);
    }   // namespace gum_tests

    static void test_trans2() {
      {
        gum::DiscretizedVariable< double > var("X1", "");
        var.addTick(1);
        var.addTick(4);
        var.addTick(2);
        var.addTick(3);

        gum::learning::DBTranslator4DiscretizedVariable translator(var);
        CHECK((translator.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator.translate("4").discr_val) == ((std::size_t)2));
        CHECK((translator.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("[1;2["));
        CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("[2;3["));
        CHECK((translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("[3;4]"));
        CHECK((translator.variable()->toString()) == ("X1:Discretized(<[1;2[,[2;3[,[3;4]>)"));

        CHECK((translator.translate(translator.translateBack(translator.translate("1"))).discr_val)
              == ((std::size_t)0));

        gum::DiscretizedVariable< double > var2("X2", "");
        var2.addTick(1);
        var2.addTick(2);
        var2.addTick(3);
        var2.addTick(4);

        gum::learning::DBTranslator4DiscretizedVariable translator2(var2);
        CHECK((translator2.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator2.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator2.translate("4").discr_val) == ((std::size_t)2));
        CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("[1;2["));
        CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("[2;3["));
        CHECK((translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("[3;4]"));
        CHECK((translator2.variable()->toString()) == ("X2:Discretized(<[1;2[,[2;3[,[3;4]>)"));

        gum::learning::DBTranslator4DiscretizedVariable translator3(translator);
        CHECK((translator3.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator3.translate("4").discr_val) == ((std::size_t)2));
        CHECK((translator3.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("[1;2["));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("[2;3["));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("[3;4]"));
        CHECK((translator3.variable()->toString()) == ("X1:Discretized(<[1;2[,[2;3[,[3;4]>)"));

        gum::learning::DBTranslator4DiscretizedVariable translator4(translator2);
        CHECK((translator4.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator4.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator4.translate("4").discr_val) == ((std::size_t)2));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("[1;2["));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("[2;3["));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("[3;4]"));
        CHECK((translator4.variable()->toString()) == ("X2:Discretized(<[1;2[,[2;3[,[3;4]>)"));

        gum::learning::DBTranslator4DiscretizedVariable translator5(std::move(translator3));
        CHECK((translator5.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator5.translate("4").discr_val) == ((std::size_t)2));
        CHECK((translator5.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("[1;2["));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("[2;3["));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("[3;4]"));
        CHECK((translator5.variable()->toString()) == ("X1:Discretized(<[1;2[,[2;3[,[3;4]>)"));

        gum::learning::DBTranslator4DiscretizedVariable translator6(std::move(translator4));
        CHECK((translator6.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator6.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator6.translate("4").discr_val) == ((std::size_t)2));
        CHECK((translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("[1;2["));
        CHECK((translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("[2;3["));
        CHECK((translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("[3;4]"));
        CHECK((translator6.variable()->toString()) == ("X2:Discretized(<[1;2[,[2;3[,[3;4]>)"));

        gum::learning::DBTranslator4DiscretizedVariable* translator7 = translator6.clone();
        CHECK((translator7->translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator7->translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator7->translate("4").discr_val) == ((std::size_t)2));
        CHECK((translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("[1;2["));
        CHECK((translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("[2;3["));
        CHECK((translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("[3;4]"));
        CHECK((translator7->variable()->toString()) == ("X2:Discretized(<[1;2[,[2;3[,[3;4]>)"));

        delete translator7;


        translator4 = translator6;
        CHECK((translator4.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator4.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator4.translate("4").discr_val) == ((std::size_t)2));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("[1;2["));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("[2;3["));
        CHECK((translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("[3;4]"));
        CHECK((translator4.variable()->toString()) == ("X2:Discretized(<[1;2[,[2;3[,[3;4]>)"));

        translator3 = translator5;
        CHECK((translator3.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator3.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator3.translate("4").discr_val) == ((std::size_t)2));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("[1;2["));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("[2;3["));
        CHECK((translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("[3;4]"));
        CHECK((translator3.variable()->toString()) == ("X1:Discretized(<[1;2[,[2;3[,[3;4]>)"));

        translator5 = std::move(translator3);
        CHECK((translator5.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator5.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator5.translate("4").discr_val) == ((std::size_t)2));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}))
              == ("[1;2["));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}))
              == ("[2;3["));
        CHECK((translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}))
              == ("[3;4]"));
        CHECK((translator5.variable()->toString()) == ("X1:Discretized(<[1;2[,[2;3[,[3;4]>)"));
      }
    }

    void xtest_trans5() {
      {
        gum::DiscretizedVariable< double > var("X1", "");
        var.addTick(1);
        var.addTick(2);
        var.addTick(3);
        var.addTick(4);

        gum::learning::DBTranslator4DiscretizedVariable translator(var, 1000);

        CHECK((translator.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator.translate("4").discr_val) == ((std::size_t)2));
        CHECK(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
              == "[1;2[");
        CHECK(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
              == "[2;3[");
        CHECK(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
              == "[3;4]");
        CHECK((translator.variable()->toString()) == ("X1<[1;2[,[2;3[,[3;4]>"));

        gum::DiscretizedVariable< double > var2("X2", "");
        var2.addTick(1);
        var2.addTick(2);
        var2.addTick(3);
        var2.addTick(4);

        gum::learning::DBTranslator4DiscretizedVariable translator2(var2);
        CHECK((translator2.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator2.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator2.translate("4").discr_val) == ((std::size_t)2));
        CHECK(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
              == "[1;2[");
        CHECK(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
              == "[2;3[");
        CHECK(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
              == "[3;4]");
        CHECK((translator2.variable()->toString()) == ("X2<[1;2[,[2;3[,[3;4]>"));

        gum::learning::DBTranslator4DiscretizedVariable translator3(translator);
        CHECK((translator3.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator3.translate("4").discr_val) == ((std::size_t)2));
        CHECK((translator3.translate("2").discr_val) == ((std::size_t)1));
        CHECK(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
              == "[1;2[");
        CHECK(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
              == "[2;3[");
        CHECK(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
              == "[3;4]");
        CHECK((translator3.variable()->toString()) == ("X1<[1;2[,[2;3[,[3;4]>"));

        gum::learning::DBTranslator4DiscretizedVariable translator4(translator2);
        CHECK((translator4.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator4.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator4.translate("4").discr_val) == ((std::size_t)2));
        CHECK(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
              == "[1;2[");
        CHECK(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
              == "[2;3[");
        CHECK(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
              == "[3;4]");
        CHECK((translator4.variable()->toString()) == ("X2<[1;2[,[2;3[,[3;4]>"));

        gum::learning::DBTranslator4DiscretizedVariable translator5(std::move(translator3));
        CHECK((translator5.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator5.translate("4").discr_val) == ((std::size_t)2));
        CHECK((translator5.translate("2").discr_val) == ((std::size_t)1));
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
              == "[1;2[");
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
              == "[2;3[");
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
              == "[3;4]");
        CHECK((translator5.variable()->toString()) == ("X1<[1;2[,[2;3[,[3;4]>"));

        gum::learning::DBTranslator4DiscretizedVariable translator6(std::move(translator4));
        CHECK((translator6.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator6.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator6.translate("4").discr_val) == ((std::size_t)2));
        CHECK(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
              == "[1;2[");
        CHECK(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
              == "[2;3[");
        CHECK(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
              == "[3;4]");
        CHECK((translator6.variable()->toString()) == ("X2<[1;2[,[2;3[,[3;4]>"));

        gum::learning::DBTranslator4DiscretizedVariable* translator7 = translator6.clone();
        CHECK((translator7->translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator7->translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator7->translate("4").discr_val) == ((std::size_t)2));
        CHECK(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
              == "[1;2[");
        CHECK(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
              == "[2;3[");
        CHECK(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
              == "[3;4]");
        CHECK((translator7->variable()->toString()) == ("X2<[1;2[,[2;3[,[3;4]>"));

        delete translator7;

        translator5 = translator6;
        CHECK((translator5.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator5.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator5.translate("4").discr_val) == ((std::size_t)2));
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
              == "[1;2[");
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
              == "[2;3[");
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
              == "[3;4]");
        CHECK((translator5.variable()->toString()) == ("X2<[1;2[,[2;3[,[3;4]>"));

        translator3 = translator5;
        CHECK((translator3.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator3.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator3.translate("4").discr_val) == ((std::size_t)2));
        CHECK(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
              == "[1;2[");
        CHECK(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
              == "[2;3[");
        CHECK(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
              == "[3;4]");
        CHECK((translator3.variable()->toString()) == ("X2<[1;2[,[2;3[,[3;4]>"));

        translator5 = std::move(translator3);
        CHECK((translator5.translate("1").discr_val) == ((std::size_t)0));
        CHECK((translator5.translate("2").discr_val) == ((std::size_t)1));
        CHECK((translator5.translate("4").discr_val) == ((std::size_t)2));
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
              == "[1;2[");
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
              == "[2;3[");
        CHECK(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
              == "[3;4]");
        CHECK((translator5.variable()->toString()) == ("X2<[1;2[,[2;3[,[3;4]>"));
      }
    }

    static void test_trans6() {
      gum::DiscretizedVariable< double > var("X1", "");
      var.addTick(1);
      var.addTick(3);
      var.addTick(7);
      var.addTick(9);

      std::vector< std::string > missing{"[1;3[", "NA", "[7;9]", "1", "4.3", "10", "?"};

      gum::learning::DBTranslator4DiscretizedVariable translator(var, missing);

      CHECK((translator.translate("1").discr_val) == ((std::size_t)0));
      CHECK((translator.translate("3.2").discr_val) == ((std::size_t)1));
      CHECK((translator.translate("NA").discr_val)
            == ((std::size_t)std::numeric_limits< std::size_t >::max()));
      CHECK((translator.translate("?").discr_val)
            == ((std::size_t)std::numeric_limits< std::size_t >::max()));
      CHECK((translator.translate("7").discr_val) == ((std::size_t)2));

      CHECK(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "[1;3[");
      CHECK(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}) == "[3;7[");
      CHECK(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}) == "[7;9]");

      std::string back = translator.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()});
      CHECK((back) == (*(translator.missingSymbols().begin())));
      gum::Set< std::string > missing_kept{"NA", "10", "?"};
      CHECK((translator.missingSymbols()) == (missing_kept));
      CHECK((translator.needsReordering()) == (false));

      auto new_order = translator.reorder();
      CHECK((new_order.size()) == (gum::Size(0)));
    }
  };

  GUM_TEST_ACTIF(_trans1)
  GUM_TEST_ACTIF(_trans2)
  GUM_TEST_ACTIF(_trans6)

} /* namespace gum_tests */
