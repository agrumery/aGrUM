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
#pragma once


#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/database/DBTranslator4DiscretizedVariable.h>

namespace gum_tests {

  class GUM_TEST_SUITE(DBTranslator4DiscretizedVariable) {
    public:
    GUM_ACTIVE_TEST(_trans1) {
      gum::DiscretizedVariable< int > var("X1", "");
      var.addTick(1);
      var.addTick(3);
      var.addTick(10);

      gum::learning::DBTranslator4DiscretizedVariable translator(var);
      TS_ASSERT(!translator.isLossless())
      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("1.3"))
      TS_ASSERT_EQUALS(translator.translate("1.2").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator.translate("2.4").discr_val, (std::size_t)0)
      TS_ASSERT_THROWS(translator.translate("0"), const gum::UnknownLabelInDatabase&)
      TS_ASSERT_THROWS(translator.translate("11"), const gum::UnknownLabelInDatabase&)
      TS_ASSERT_THROWS(translator.translate("aaa"), const gum::TypeError&)

      TS_ASSERT_EQUALS(translator.missingValue().discr_val,
                       (std::size_t)std::numeric_limits< std::size_t >::max());

      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("7"))
      TS_ASSERT_EQUALS(translator.translate("10").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator.translate("9").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "[1;3[");
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "[3;10]");

      const auto& tr_var     = *(translator.variable());
      int         good_discr = 1;
      try {
        const gum::DiscretizedVariable< int >& xvar_discr
            = dynamic_cast< const gum::DiscretizedVariable< int >& >(tr_var);
        TS_ASSERT_EQUALS(xvar_discr.domainSize(), (gum::Size) static_cast< gum::Size >(2))
        TS_ASSERT_EQUALS(xvar_discr.label(0), "[1;3[")
        TS_ASSERT_EQUALS(xvar_discr.label(1), "[3;10]")
      } catch (std::bad_cast&) { good_discr = 0; }
      TS_ASSERT_EQUALS(good_discr, 1)

      std::vector< std::string >                      missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4DiscretizedVariable translator2(var, missing);
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("1.3"))
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("3"))
      TS_ASSERT_EQUALS(translator2.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator2.translate("3").discr_val, (std::size_t)1)
      TS_ASSERT(translator2.translate("N/A").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator2.translate("?").discr_val,
                       (std::size_t)std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator2.translate("???").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_THROWS(translator2.translate("??"), const gum::TypeError&)
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "[1;3[");
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "[3;10]");

      gum::learning::DBTranslator4DiscretizedVariable translator3(var, missing, 3);
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("1"))
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("10"))
      TS_ASSERT_EQUALS(translator3.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator3.translate("9.9").discr_val, (std::size_t)1)
      TS_ASSERT(translator3.translate("N/A").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator3.translate("?").discr_val,
                       (std::size_t)std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator3.translate("???").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT_THROWS(translator3.translate("??"), const gum::TypeError&)
      TS_ASSERT_THROWS(translator3.translate("a"), const gum::TypeError&)

      TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                == "[1;3[");
      TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                == "[3;10]");
      TS_ASSERT_THROWS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       const gum::UnknownLabelInDatabase&)
      TS_ASSERT(translator3.translateBack(
                    gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()})
                == "?");

      TS_ASSERT_EQUALS(translator3.domainSize(), static_cast< gum::Size >(2))

      TS_ASSERT_THROWS(gum::learning::DBTranslator4DiscretizedVariable translator4(var, missing, 1),
                       const gum::SizeError&)
    }   // namespace gum_tests

    GUM_ACTIVE_TEST(_trans2) {
      {
        gum::DiscretizedVariable< double > var("X1", "");
        var.addTick(1);
        var.addTick(4);
        var.addTick(2);
        var.addTick(3);

        gum::learning::DBTranslator4DiscretizedVariable translator(var);
        TS_ASSERT_EQUALS(translator.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(translator.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                         "[1;2[");
        TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                         "[2;3[");
        TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                         "[3;4]");
        TS_ASSERT_EQUALS(translator.variable()->toString(), "X1:Discretized(<[1;2[,[2;3[,[3;4]>)")

        TS_ASSERT_EQUALS(
            translator.translate(translator.translateBack(translator.translate("1"))).discr_val,
            (std::size_t)0);

        gum::DiscretizedVariable< double > var2("X2", "");
        var2.addTick(1);
        var2.addTick(2);
        var2.addTick(3);
        var2.addTick(4);

        gum::learning::DBTranslator4DiscretizedVariable translator2(var2);
        TS_ASSERT_EQUALS(translator2.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator2.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator2.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(
            translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "[1;2[");
        TS_ASSERT_EQUALS(
            translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "[2;3[");
        TS_ASSERT_EQUALS(
            translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "[3;4]");
        TS_ASSERT_EQUALS(translator2.variable()->toString(), "X2:Discretized(<[1;2[,[2;3[,[3;4]>)")

        gum::learning::DBTranslator4DiscretizedVariable translator3(translator);
        TS_ASSERT_EQUALS(translator3.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator3.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(translator3.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(
            translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "[1;2[");
        TS_ASSERT_EQUALS(
            translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "[2;3[");
        TS_ASSERT_EQUALS(
            translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "[3;4]");
        TS_ASSERT_EQUALS(translator3.variable()->toString(), "X1:Discretized(<[1;2[,[2;3[,[3;4]>)")

        gum::learning::DBTranslator4DiscretizedVariable translator4(translator2);
        TS_ASSERT_EQUALS(translator4.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator4.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator4.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(
            translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "[1;2[");
        TS_ASSERT_EQUALS(
            translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "[2;3[");
        TS_ASSERT_EQUALS(
            translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "[3;4]");
        TS_ASSERT_EQUALS(translator4.variable()->toString(), "X2:Discretized(<[1;2[,[2;3[,[3;4]>)")

        gum::learning::DBTranslator4DiscretizedVariable translator5(std::move(translator3));
        TS_ASSERT_EQUALS(translator5.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator5.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(translator5.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(
            translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "[1;2[");
        TS_ASSERT_EQUALS(
            translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "[2;3[");
        TS_ASSERT_EQUALS(
            translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "[3;4]");
        TS_ASSERT_EQUALS(translator5.variable()->toString(), "X1:Discretized(<[1;2[,[2;3[,[3;4]>)")

        gum::learning::DBTranslator4DiscretizedVariable translator6(std::move(translator4));
        TS_ASSERT_EQUALS(translator6.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator6.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator6.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(
            translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "[1;2[");
        TS_ASSERT_EQUALS(
            translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "[2;3[");
        TS_ASSERT_EQUALS(
            translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "[3;4]");
        TS_ASSERT_EQUALS(translator6.variable()->toString(), "X2:Discretized(<[1;2[,[2;3[,[3;4]>)")

        gum::learning::DBTranslator4DiscretizedVariable* translator7 = translator6.clone();
        TS_ASSERT_EQUALS(translator7->translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator7->translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator7->translate("4").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(
            translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "[1;2[");
        TS_ASSERT_EQUALS(
            translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "[2;3[");
        TS_ASSERT_EQUALS(
            translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "[3;4]");
        TS_ASSERT_EQUALS(translator7->variable()->toString(),
                         "X2:Discretized(<[1;2[,[2;3[,[3;4]>)");

        delete translator7;


        translator4 = translator6;
        TS_ASSERT_EQUALS(translator4.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator4.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator4.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(
            translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "[1;2[");
        TS_ASSERT_EQUALS(
            translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "[2;3[");
        TS_ASSERT_EQUALS(
            translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "[3;4]");
        TS_ASSERT_EQUALS(translator4.variable()->toString(), "X2:Discretized(<[1;2[,[2;3[,[3;4]>)")

        translator3 = translator5;
        TS_ASSERT_EQUALS(translator3.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator3.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator3.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(
            translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "[1;2[");
        TS_ASSERT_EQUALS(
            translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "[2;3[");
        TS_ASSERT_EQUALS(
            translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "[3;4]");
        TS_ASSERT_EQUALS(translator3.variable()->toString(), "X1:Discretized(<[1;2[,[2;3[,[3;4]>)")

        translator5 = std::move(translator3);
        TS_ASSERT_EQUALS(translator5.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator5.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator5.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(
            translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "[1;2[");
        TS_ASSERT_EQUALS(
            translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "[2;3[");
        TS_ASSERT_EQUALS(
            translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "[3;4]");
        TS_ASSERT_EQUALS(translator5.variable()->toString(), "X1:Discretized(<[1;2[,[2;3[,[3;4]>)")
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

        TS_ASSERT_EQUALS(translator.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "[1;2[");
        TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "[2;3[");
        TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "[3;4]");
        TS_ASSERT_EQUALS(translator.variable()->toString(), "X1<[1;2[,[2;3[,[3;4]>")

        gum::DiscretizedVariable< double > var2("X2", "");
        var2.addTick(1);
        var2.addTick(2);
        var2.addTick(3);
        var2.addTick(4);

        gum::learning::DBTranslator4DiscretizedVariable translator2(var2);
        TS_ASSERT_EQUALS(translator2.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator2.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator2.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "[1;2[");
        TS_ASSERT(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "[2;3[");
        TS_ASSERT(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "[3;4]");
        TS_ASSERT_EQUALS(translator2.variable()->toString(), "X2<[1;2[,[2;3[,[3;4]>")

        gum::learning::DBTranslator4DiscretizedVariable translator3(translator);
        TS_ASSERT_EQUALS(translator3.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator3.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(translator3.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "[1;2[");
        TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "[2;3[");
        TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "[3;4]");
        TS_ASSERT_EQUALS(translator3.variable()->toString(), "X1<[1;2[,[2;3[,[3;4]>")

        gum::learning::DBTranslator4DiscretizedVariable translator4(translator2);
        TS_ASSERT_EQUALS(translator4.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator4.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator4.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "[1;2[");
        TS_ASSERT(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "[2;3[");
        TS_ASSERT(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "[3;4]");
        TS_ASSERT_EQUALS(translator4.variable()->toString(), "X2<[1;2[,[2;3[,[3;4]>")

        gum::learning::DBTranslator4DiscretizedVariable translator5(std::move(translator3));
        TS_ASSERT_EQUALS(translator5.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator5.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(translator5.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "[1;2[");
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "[2;3[");
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "[3;4]");
        TS_ASSERT_EQUALS(translator5.variable()->toString(), "X1<[1;2[,[2;3[,[3;4]>")

        gum::learning::DBTranslator4DiscretizedVariable translator6(std::move(translator4));
        TS_ASSERT_EQUALS(translator6.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator6.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator6.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "[1;2[");
        TS_ASSERT(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "[2;3[");
        TS_ASSERT(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "[3;4]");
        TS_ASSERT_EQUALS(translator6.variable()->toString(), "X2<[1;2[,[2;3[,[3;4]>")

        gum::learning::DBTranslator4DiscretizedVariable* translator7 = translator6.clone();
        TS_ASSERT_EQUALS(translator7->translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator7->translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator7->translate("4").discr_val, (std::size_t)2)
        TS_ASSERT(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "[1;2[");
        TS_ASSERT(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "[2;3[");
        TS_ASSERT(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "[3;4]");
        TS_ASSERT_EQUALS(translator7->variable()->toString(), "X2<[1;2[,[2;3[,[3;4]>")

        delete translator7;

        translator5 = translator6;
        TS_ASSERT_EQUALS(translator5.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator5.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator5.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "[1;2[");
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "[2;3[");
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "[3;4]");
        TS_ASSERT_EQUALS(translator5.variable()->toString(), "X2<[1;2[,[2;3[,[3;4]>")

        translator3 = translator5;
        TS_ASSERT_EQUALS(translator3.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator3.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator3.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "[1;2[");
        TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "[2;3[");
        TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "[3;4]");
        TS_ASSERT_EQUALS(translator3.variable()->toString(), "X2<[1;2[,[2;3[,[3;4]>")

        translator5 = std::move(translator3);
        TS_ASSERT_EQUALS(translator5.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator5.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator5.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "[1;2[");
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "[2;3[");
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "[3;4]");
        TS_ASSERT_EQUALS(translator5.variable()->toString(), "X2<[1;2[,[2;3[,[3;4]>")
      }
    }

    GUM_ACTIVE_TEST(_trans6) {
      gum::DiscretizedVariable< double > var("X1", "");
      var.addTick(1);
      var.addTick(3);
      var.addTick(7);
      var.addTick(9);

      std::vector< std::string > missing{"[1;3[", "NA", "[7;9]", "1", "4.3", "10", "?"};

      gum::learning::DBTranslator4DiscretizedVariable translator(var, missing);

      TS_ASSERT_EQUALS(translator.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator.translate("3.2").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator.translate("NA").discr_val,
                       (std::size_t)std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator.translate("?").discr_val,
                       (std::size_t)std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator.translate("7").discr_val, (std::size_t)2)

      TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                == "[1;3[");
      TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                == "[3;7[");
      TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                == "[7;9]");

      std::string back = translator.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()});
      TS_ASSERT_EQUALS(back, *(translator.missingSymbols().begin()))
      gum::Set< std::string > missing_kept{"NA", "10", "?"};
      TS_ASSERT_EQUALS(translator.missingSymbols(), missing_kept)
      TS_ASSERT_EQUALS(translator.needsReordering(), false)

      auto new_order = translator.reorder();
      TS_ASSERT_EQUALS(new_order.size(), gum::Size(0))
    }
  };

} /* namespace gum_tests */
