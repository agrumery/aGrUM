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
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/database/DBTranslator4ContinuousVariable.h>

namespace gum_tests {

  class GUM_TEST_SUITE(DBTranslator4ContinuousVariable) {
    public:
    GUM_ACTIVE_TEST(_trans1) {
      gum::learning::DBTranslator4ContinuousVariable translator;
      TS_ASSERT(translator.isLossless())
      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("3"))
      TS_ASSERT_EQUALS(translator.translate("3").cont_val, 3)
      TS_ASSERT_EQUALS(std::stof(translator.translateBack(gum::learning::DBTranslatedValue{3.0f})),
                       3);

      TS_ASSERT_EQUALS(translator.missingValue().cont_val, std::numeric_limits< float >::max())

      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("5"))
      TS_ASSERT_EQUALS(translator.translate("4.22").cont_val, 4.22f)
      TS_ASSERT_EQUALS(translator.translate("-5.34").cont_val, -5.34f)
      TS_ASSERT_EQUALS(translator.translate("-5.34e3").cont_val, -5.34e3f)
      TS_ASSERT_EQUALS(translator.translate("-5.34e-34").cont_val, -5.34e-34f)
      TS_ASSERT(std::stof(translator.translateBack(gum::learning::DBTranslatedValue{-5.34f}))
                == -5.34f);
      TS_ASSERT(std::stof(translator.translateBack(gum::learning::DBTranslatedValue{-5.34e3f}))
                == -5.34e3f);
      TS_ASSERT(std::stof(translator.translateBack(gum::learning::DBTranslatedValue{-5.34e-34f}))
                == -5.34e-34f);
      TS_ASSERT_THROWS(translator.translate("4.22x"), const gum::TypeError&)
      TS_ASSERT_THROWS(translator.translate("0x422"), const gum::TypeError&)

      std::vector< std::string >                     missing{"2", "N/A", "20", "4", "xxx"};
      gum::learning::DBTranslator4ContinuousVariable translator2(missing);
      TS_ASSERT(!translator2.missingSymbols().exists("2"))
      TS_ASSERT(!translator2.missingSymbols().exists("20"))
      TS_ASSERT(!translator2.missingSymbols().exists("4"))
      TS_ASSERT_EQUALS(translator.translate("4.22").cont_val, 4.22f)
      TS_ASSERT_THROWS(translator2.translate("yyy"), const gum::TypeError&)
      TS_ASSERT_EQUALS(translator2.translate("N/A").cont_val, std::numeric_limits< float >::max())
      TS_ASSERT_EQUALS(translator2.translate("xxx").cont_val, std::numeric_limits< float >::max())

      gum::ContinuousVariable< double >              var("X2", "", -2, 10);
      gum::learning::DBTranslator4ContinuousVariable translator3(var, missing);
      TS_ASSERT(!translator3.missingSymbols().exists("2"))
      TS_ASSERT(!translator3.missingSymbols().exists("4"))
      TS_ASSERT(translator3.missingSymbols().exists("20"))
      TS_ASSERT(translator3.missingSymbols().exists("N/A"))
      TS_ASSERT(translator3.missingSymbols().exists("xxx"))

      TS_ASSERT_EQUALS(translator3.translate("4.22").cont_val, 4.22f)
      TS_ASSERT_EQUALS(translator3.translate("-2").cont_val, -2.0f)
      TS_ASSERT_EQUALS(translator3.translate("10").cont_val, 10.0f)
      TS_ASSERT_THROWS(translator3.translate("11"), const gum::UnknownLabelInDatabase&)
      TS_ASSERT_EQUALS(translator3.translate("N/A").cont_val, std::numeric_limits< float >::max())
      TS_ASSERT_EQUALS(translator3.translate("xxx").cont_val, std::numeric_limits< float >::max())

      const auto& tr_var    = *(translator3.variable());
      int         good_cont = 1;
      try {
        const gum::ContinuousVariable< double >& xvar_cont
            = dynamic_cast< const gum::ContinuousVariable< double >& >(tr_var);
        TS_ASSERT_EQUALS(xvar_cont.lowerBound(), -2.0)
        TS_ASSERT_EQUALS(xvar_cont.upperBound(), 10.0)
      } catch (std::bad_cast&) { good_cont = 0; }
      TS_ASSERT_EQUALS(good_cont, 1)

      std::vector< std::string >                     missing4{"2", "N/A", "20", "4", "xxx", "-10"};
      gum::learning::DBTranslator4ContinuousVariable translator4(var, missing4, true);
      TS_ASSERT(!translator4.missingSymbols().exists("2"))
      TS_ASSERT(!translator4.missingSymbols().exists("4"))
      TS_ASSERT(translator4.missingSymbols().exists("20"))
      TS_ASSERT(translator4.missingSymbols().exists("N/A"))
      TS_ASSERT(translator4.missingSymbols().exists("xxx"))
      TS_ASSERT_EQUALS(translator4.translate("4.22").cont_val, 4.22f)
      TS_ASSERT_EQUALS(translator4.translate("-2").cont_val, -2.0f)
      TS_ASSERT_EQUALS(translator4.translate("10").cont_val, 10.0f)
      TS_ASSERT_EQUALS(translator4.translate("18").cont_val, 18.0f)
      TS_ASSERT_EQUALS(translator4.translate("20").cont_val, std::numeric_limits< float >::max())
      TS_ASSERT_EQUALS(translator4.translate("19.5").cont_val, 19.5f)
      TS_ASSERT_THROWS(translator4.translate("21"), const gum::OperationNotAllowed&)
      TS_ASSERT_EQUALS(translator4.translate("-12").cont_val, -12.0f)
      TS_ASSERT_EQUALS(translator4.translate("-10").cont_val, -10.0f)
      TS_ASSERT(std::stof(translator4.translateBack(gum::learning::DBTranslatedValue{5.3f}))
                == 5.3f);
      TS_ASSERT_THROWS(translator4.translateBack(gum::learning::DBTranslatedValue{50.0f}),
                       const gum::UnknownLabelInDatabase&)
      TS_ASSERT_THROWS(translator4.translateBack(gum::learning::DBTranslatedValue{-50.0f}),
                       const gum::UnknownLabelInDatabase&)
      TS_ASSERT(translator4.missingSymbols().exists(translator4.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));


      gum::learning::DBTranslator4ContinuousVariable translator5(var, missing4, true);
      TS_ASSERT(!translator5.missingSymbols().exists("2"))
      TS_ASSERT(!translator5.missingSymbols().exists("4"))
      TS_ASSERT_EQUALS(translator5.translate("-10").cont_val, std::numeric_limits< float >::max())
      TS_ASSERT_THROWS(translator5.translate("-11"), const gum::OperationNotAllowed&)
      TS_ASSERT_EQUALS(translator5.translate("220").cont_val, 220.0f)
      TS_ASSERT_EQUALS(translator5.translate("20").cont_val, 20.0f)

      TS_ASSERT_EQUALS(translator5.needsReordering(), false)
      TS_ASSERT(translator5.reorder().empty())
    }   // namespace gum_tests

    GUM_ACTIVE_TEST(_trans2) {
      {
        gum::ContinuousVariable< double > var("X2", "", -2, 10);
        std::vector< std::string >        missing{"2", "N/A", "20", "4", "xxx", "-10"};

        gum::learning::DBTranslator4ContinuousVariable translator(var, missing, true);
        TS_ASSERT(!translator.missingSymbols().exists("2"))
        TS_ASSERT(!translator.missingSymbols().exists("4"))
        TS_ASSERT_EQUALS(translator.translate("-10").cont_val, std::numeric_limits< float >::max())

        TS_ASSERT(std::stof(translator.translateBack(gum::learning::DBTranslatedValue{5.3f}))
                  == 5.3f);
        TS_ASSERT_THROWS(translator.translateBack(gum::learning::DBTranslatedValue{50.0f}),
                         const gum::UnknownLabelInDatabase&)
        TS_ASSERT_THROWS(translator.translateBack(gum::learning::DBTranslatedValue{-50.0f}),
                         const gum::UnknownLabelInDatabase&)
        TS_ASSERT(translator.missingSymbols().exists(translator.translateBack(
            gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));

        gum::learning::DBTranslator4ContinuousVariable translator2(translator);
        TS_ASSERT(!translator2.missingSymbols().exists("2"))
        TS_ASSERT(!translator2.missingSymbols().exists("4"))
        TS_ASSERT_EQUALS(translator2.translate("-10").cont_val,
                         std::numeric_limits< float >::max());

        TS_ASSERT(std::stof(translator2.translateBack(gum::learning::DBTranslatedValue{5.3f}))
                  == 5.3f);
        TS_ASSERT_THROWS(translator2.translateBack(gum::learning::DBTranslatedValue{50.0f}),
                         const gum::UnknownLabelInDatabase&)
        TS_ASSERT_THROWS(translator2.translateBack(gum::learning::DBTranslatedValue{-50.0f}),
                         const gum::UnknownLabelInDatabase&)
        TS_ASSERT(translator2.missingSymbols().exists(translator2.translateBack(
            gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));

        gum::learning::DBTranslator4ContinuousVariable translator3(translator2);
        TS_ASSERT(!translator3.missingSymbols().exists("2"))
        TS_ASSERT(!translator3.missingSymbols().exists("4"))
        TS_ASSERT_EQUALS(translator3.translate("-10").cont_val,
                         std::numeric_limits< float >::max());

        TS_ASSERT(std::stof(translator3.translateBack(gum::learning::DBTranslatedValue{5.3f}))
                  == 5.3f);
        TS_ASSERT_THROWS(translator3.translateBack(gum::learning::DBTranslatedValue{50.0f}),
                         const gum::UnknownLabelInDatabase&)
        TS_ASSERT_THROWS(translator3.translateBack(gum::learning::DBTranslatedValue{-50.0f}),
                         const gum::UnknownLabelInDatabase&)
        TS_ASSERT(translator3.missingSymbols().exists(translator3.translateBack(
            gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));

        gum::learning::DBTranslator4ContinuousVariable translator4(std::move(translator3));
        TS_ASSERT(!translator4.missingSymbols().exists("2"))
        TS_ASSERT(!translator4.missingSymbols().exists("4"))
        TS_ASSERT_EQUALS(translator4.translate("-10").cont_val,
                         std::numeric_limits< float >::max());

        TS_ASSERT(std::stof(translator4.translateBack(gum::learning::DBTranslatedValue{5.3f}))
                  == 5.3f);
        TS_ASSERT_THROWS(translator4.translateBack(gum::learning::DBTranslatedValue{50.0f}),
                         const gum::UnknownLabelInDatabase&)
        TS_ASSERT_THROWS(translator4.translateBack(gum::learning::DBTranslatedValue{-50.0f}),
                         const gum::UnknownLabelInDatabase&)
        TS_ASSERT(translator4.missingSymbols().exists(translator4.translateBack(
            gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));

        gum::learning::DBTranslator4ContinuousVariable* translator5 = translator4.clone();

        TS_ASSERT(!translator5->missingSymbols().exists("2"))
        TS_ASSERT(!translator5->missingSymbols().exists("4"))
        TS_ASSERT_EQUALS(translator5->translate("-10").cont_val,
                         std::numeric_limits< float >::max());

        TS_ASSERT(std::stof(translator5->translateBack(gum::learning::DBTranslatedValue{5.3f}))
                  == 5.3f);
        TS_ASSERT_THROWS(translator5->translateBack(gum::learning::DBTranslatedValue{50.0f}),
                         const gum::UnknownLabelInDatabase&)
        TS_ASSERT_THROWS(translator5->translateBack(gum::learning::DBTranslatedValue{-50.0f}),
                         const gum::UnknownLabelInDatabase&)
        TS_ASSERT(translator5->missingSymbols().exists(translator5->translateBack(
            gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));

        delete translator5;

        TS_ASSERT_EQUALS(translator4.translate("12").cont_val, 12.0f)
        TS_ASSERT_EQUALS(translator4.variable()->lowerBoundAsDouble(), -2.0f)
        TS_ASSERT_EQUALS(translator4.variable()->upperBoundAsDouble(), 12.0f)
        translator4 = translator2;
        TS_ASSERT_EQUALS(translator4.variable()->lowerBoundAsDouble(), -2.0f)
        TS_ASSERT_EQUALS(translator4.variable()->upperBoundAsDouble(), 10.0f)

        TS_ASSERT_EQUALS(translator.translate("12").cont_val, 12.0f)
        TS_ASSERT_EQUALS(translator.variable()->lowerBoundAsDouble(), -2.0f)
        TS_ASSERT_EQUALS(translator.variable()->upperBoundAsDouble(), 12.0f)
        translator = translator2;
        TS_ASSERT_EQUALS(translator.variable()->lowerBoundAsDouble(), -2.0f)
        TS_ASSERT_EQUALS(translator.variable()->upperBoundAsDouble(), 10.0f)

        TS_ASSERT_EQUALS(translator.translate("12").cont_val, 12.0f)
        TS_ASSERT_EQUALS(translator.variable()->lowerBoundAsDouble(), -2.0f)
        TS_ASSERT_EQUALS(translator.variable()->upperBoundAsDouble(), 12.0f)
        translator = std::move(translator2);
        TS_ASSERT_EQUALS(translator.variable()->lowerBoundAsDouble(), -2.0f)
        TS_ASSERT_EQUALS(translator.variable()->upperBoundAsDouble(), 10.0f)
      }
    }
  };

} /* namespace gum_tests */
