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

#include <agrum/base/database/DBTranslator4NumericalDiscreteVariable.h>

namespace gum_tests {

  class GUM_TEST_SUITE(DBTransl4NumerDiscrVaria) {
    public:
    GUM_ACTIVE_TEST(_trans1) {
      gum::NumericalDiscreteVariable var("X1", "");
      var.addValue(1);
      var.addValue(3.5);
      var.addValue(10);
      var.addValue(12);

      gum::learning::DBTranslator4NumericalDiscreteVariable translator(var);
      TS_ASSERT(translator.isLossless())
      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("1"))
      TS_ASSERT_EQUALS(translator.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator.translate("3.5").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator.translate("10").discr_val, (std::size_t)2)
      TS_ASSERT_THROWS(translator.translate("0"), const gum::UnknownLabelInDatabase&)
      TS_ASSERT_THROWS(translator.translate("11"), const gum::UnknownLabelInDatabase&)
      TS_ASSERT_THROWS(translator.translate("aaa"), const gum::TypeError&)

      TS_ASSERT_EQUALS(translator.missingValue().discr_val,
                       std::numeric_limits< std::size_t >::max());

      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("12"))
      TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "1")
      TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}) == "3.5")

      const auto& tr_var = *(translator.variable());
      int         good   = 1;
      try {
        const gum::NumericalDiscreteVariable& xvar_discr
            = dynamic_cast< const gum::NumericalDiscreteVariable& >(tr_var);
        TS_ASSERT_EQUALS(xvar_discr.domainSize(), static_cast< gum::Size >(4))
        TS_ASSERT_EQUALS(xvar_discr.label(0), "1")
        TS_ASSERT_EQUALS(xvar_discr.label(1), "3.5")
        TS_ASSERT_EQUALS(xvar_discr.label(2), "10")
        TS_ASSERT_EQUALS(xvar_discr.label(3), "12")
      } catch (std::bad_cast&) { good = 0; }
      TS_ASSERT_EQUALS(good, 1)

      std::vector< std::string >                            missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4NumericalDiscreteVariable translator2(var, missing);
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("1"))
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("12"))
      TS_ASSERT_EQUALS(translator2.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator2.translate("3.5").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator2.translate("N/A").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator2.translate("?").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator2.translate("???").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_THROWS(translator2.translate("??"), const gum::TypeError&)
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "1");
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "10");

      gum::learning::DBTranslator4NumericalDiscreteVariable translator3(var, missing, 4);
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("1"))
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("10"))
      TS_ASSERT_EQUALS(translator3.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator3.translate("3.5").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator3.translate("N/A").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator3.translate("?").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator3.translate("???").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_THROWS(translator3.translate("??"), const gum::TypeError&)
      TS_ASSERT_THROWS(translator3.translate("a"), const gum::TypeError&)

      TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}) == "1")
      TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                == "3.5")
      TS_ASSERT_THROWS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}}),
                       const gum::UnknownLabelInDatabase&)
      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{
                           std::numeric_limits< std::size_t >::max()}),
                       "?");

      TS_ASSERT_EQUALS(translator3.domainSize(), static_cast< gum::Size >(4))

      TS_ASSERT_THROWS(
          gum::learning::DBTranslator4NumericalDiscreteVariable translator4(var, missing, 1),
          const gum::SizeError&)

      TS_ASSERT_EQUALS(translator3.variable()->toString(), "X1:NumericalDiscrete({1|3.5|10|12})")
      TS_ASSERT_EQUALS(translator3.domainSize(), static_cast< gum::Size >(4))
      TS_ASSERT(!translator3.hasEditableDictionary())
      translator3.setEditableDictionaryMode(true);
      TS_ASSERT(!translator3.hasEditableDictionary())
      TS_ASSERT(!translator3.needsReordering())
      TS_ASSERT(translator3.reorder().empty())
    }   // namespace gum_tests

    GUM_ACTIVE_TEST(_trans2) {
      {
        gum::NumericalDiscreteVariable var("X1", "");
        var.addValue(1);
        var.addValue(3.5);
        var.addValue(10);
        var.addValue(12);

        gum::learning::DBTranslator4NumericalDiscreteVariable translator(var);
        TS_ASSERT_EQUALS(translator.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator.translate("12").discr_val, (std::size_t)3)
        TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                         "1");
        TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                         "3.5");
        TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                         "10");
        TS_ASSERT_EQUALS(translator.variable()->toString(), "X1:NumericalDiscrete({1|3.5|10|12})")

        TS_ASSERT_EQUALS(
            translator.translate(translator.translateBack(translator.translate("1"))).discr_val,
            (std::size_t)0);

        gum::NumericalDiscreteVariable var2("X2", "");
        var2.addValue(1);
        var2.addValue(2);
        var2.addValue(3.5);
        var2.addValue(4);

        gum::learning::DBTranslator4NumericalDiscreteVariable translator2(var2);
        TS_ASSERT_EQUALS(translator2.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator2.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator2.translate("4").discr_val, (std::size_t)3)
        TS_ASSERT_EQUALS(
            translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "1");
        TS_ASSERT_EQUALS(
            translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "2");
        TS_ASSERT_EQUALS(
            translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "3.5");
        TS_ASSERT_EQUALS(translator2.variable()->toString(), "X2:NumericalDiscrete({1|2|3.5|4})")

        gum::learning::DBTranslator4NumericalDiscreteVariable translator3(translator);
        TS_ASSERT_EQUALS(translator3.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator3.translate("10").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(translator3.translate("12").discr_val, (std::size_t)3)
        TS_ASSERT_EQUALS(
            translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "1");
        TS_ASSERT_EQUALS(
            translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "3.5");
        TS_ASSERT_EQUALS(
            translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "10");
        TS_ASSERT_EQUALS(translator3.variable()->toString(), "X1:NumericalDiscrete({1|3.5|10|12})")

        gum::learning::DBTranslator4NumericalDiscreteVariable translator4(translator2);
        TS_ASSERT_EQUALS(translator4.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator4.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator4.translate("4").discr_val, (std::size_t)3)
        TS_ASSERT_EQUALS(
            translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "1");
        TS_ASSERT_EQUALS(
            translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "2");
        TS_ASSERT_EQUALS(
            translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "3.5");
        TS_ASSERT_EQUALS(translator4.variable()->toString(), "X2:NumericalDiscrete({1|2|3.5|4})")

        gum::learning::DBTranslator4NumericalDiscreteVariable translator5(std::move(translator3));
        TS_ASSERT_EQUALS(translator5.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator5.translate("10").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(translator5.translate("12").discr_val, (std::size_t)3)
        TS_ASSERT_EQUALS(
            translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "1");
        TS_ASSERT_EQUALS(
            translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "3.5");
        TS_ASSERT_EQUALS(
            translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "10");
        TS_ASSERT_EQUALS(translator5.variable()->toString(), "X1:NumericalDiscrete({1|3.5|10|12})")

        gum::learning::DBTranslator4NumericalDiscreteVariable translator6(std::move(translator4));
        TS_ASSERT_EQUALS(translator6.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator6.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator6.translate("4").discr_val, (std::size_t)3)
        TS_ASSERT_EQUALS(
            translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "1");
        TS_ASSERT_EQUALS(
            translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "2");
        TS_ASSERT_EQUALS(
            translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "3.5");
        TS_ASSERT_EQUALS(translator6.variable()->toString(), "X2:NumericalDiscrete({1|2|3.5|4})")

        gum::learning::DBTranslator4NumericalDiscreteVariable* translator7 = translator6.clone();
        TS_ASSERT_EQUALS(translator7->translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator7->translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator7->translate("4").discr_val, (std::size_t)3)
        TS_ASSERT_EQUALS(
            translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "1");
        TS_ASSERT_EQUALS(
            translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "2");
        TS_ASSERT_EQUALS(
            translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "3.5");
        TS_ASSERT_EQUALS(translator7->variable()->toString(), "X2:NumericalDiscrete({1|2|3.5|4})")

        delete translator7;

        gum::learning::DBTranslator4NumericalDiscreteVariable translator8(var);
        translator8 = translator6;
        TS_ASSERT_EQUALS(translator8.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator8.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator8.translate("4").discr_val, (std::size_t)3)
        TS_ASSERT_EQUALS(
            translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "1");
        TS_ASSERT_EQUALS(
            translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "2");
        TS_ASSERT_EQUALS(
            translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "3.5");
        TS_ASSERT_EQUALS(translator8.variable()->toString(), "X2:NumericalDiscrete({1|2|3.5|4})")

        gum::learning::DBTranslator4NumericalDiscreteVariable translator8bis(var);
        gum::learning::DBTranslator4NumericalDiscreteVariable translator9(var2);
        translator9 = translator8bis;
        TS_ASSERT_EQUALS(translator9.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator9.translate("3.5").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator9.translate("10").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(
            translator9.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "1");
        TS_ASSERT_EQUALS(
            translator9.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "3.5");
        TS_ASSERT_EQUALS(
            translator9.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "10");
        TS_ASSERT_EQUALS(translator9.variable()->toString(), "X1:NumericalDiscrete({1|3.5|10|12})")

        translator8 = std::move(translator9);
        TS_ASSERT_EQUALS(translator8.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator8.translate("3.5").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator8.translate("10").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(
            translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
            "1");
        TS_ASSERT_EQUALS(
            translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
            "3.5");
        TS_ASSERT_EQUALS(
            translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
            "10");
        TS_ASSERT_EQUALS(translator8.variable()->toString(), "X1:NumericalDiscrete({1|3.5|10|12})")

        TS_ASSERT_THROWS(translator8.translate("5"), gum::UnknownLabelInDatabase&)
        TS_GUM_ASSERT_THROWS_NOTHING(translator8.translate("1.0"))
      }
    }
  };

} /* namespace gum_tests */
