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

#include <agrum/base/database/DBTranslator4LabelizedVariable.h>

namespace gum_tests {

  class [[maybe_unused]] DBTranslator4LabelizedVariableTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_trans1) {
      gum::learning::DBTranslator4LabelizedVariable translator;
      TS_ASSERT(translator.isLossless())
      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("toto"))
      TS_ASSERT_EQUALS(translator.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator.translate("toto").discr_val, (std::size_t)0)

      TS_ASSERT_EQUALS(translator.missingValue().discr_val,
                       std::numeric_limits< std::size_t >::max());

      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("titi"))
      TS_ASSERT_EQUALS(translator.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator.translate("titi").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "toto");
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "titi");

      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable translator2(missing);
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("toto"))
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("titi"))
      TS_ASSERT_EQUALS(translator2.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator2.translate("titi").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator2.translate("N/A").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator2.translate("?").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator2.translate("???").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator2.translate("??").discr_val, (std::size_t)2)
      TS_ASSERT_EQUALS(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "toto");
      TS_ASSERT_EQUALS(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "titi");
      TS_ASSERT_EQUALS(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "??");

      gum::learning::DBTranslator4LabelizedVariable translator3(missing, 3);
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("toto"))
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("titi"))
      TS_ASSERT_EQUALS(translator3.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator3.translate("titi").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator3.translate("N/A").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator3.translate("?").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator3.translate("???").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator3.translate("??").discr_val, (std::size_t)2)
      TS_ASSERT_THROWS(translator3.translate("a"), const gum::SizeError&)

      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "toto");
      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "titi");
      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "??");
      TS_ASSERT_THROWS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}),
                       const gum::UnknownLabelInDatabase&)
      const std::string back = translator3.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()});
      TS_ASSERT(translator3.missingSymbols().exists(back))
    }

    GUM_ACTIVE_TEST(_trans2) {
      gum::learning::DBTranslator4LabelizedVariable translator;
      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("toto"))
      TS_ASSERT_EQUALS(translator.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator.translate("toto").discr_val, (std::size_t)0)

      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("titi"))
      TS_ASSERT_EQUALS(translator.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator.translate("titi").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "toto");
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "titi");

      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable translator2(missing);
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("toto"))
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("titi"))
      TS_ASSERT_EQUALS(translator2.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator2.translate("titi").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator2.translate("N/A").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator2.translate("?").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator2.translate("???").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator2.translate("??").discr_val, (std::size_t)2)

      TS_ASSERT_EQUALS(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "toto");
      TS_ASSERT_EQUALS(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "titi");
      TS_ASSERT_EQUALS(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "??");

      gum::learning::DBTranslator4LabelizedVariable translator3(missing, 3);
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("toto"))
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("titi"))
      TS_ASSERT_EQUALS(translator3.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator3.translate("titi").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator3.translate("N/A").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator3.translate("?").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator3.translate("???").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator3.translate("??").discr_val, (std::size_t)2)
      TS_ASSERT_THROWS(translator3.translate("a"), const gum::SizeError&)

      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "toto");
      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "titi");
      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "??");
      TS_ASSERT_THROWS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}),
                       const gum::UnknownLabelInDatabase&)
      TS_ASSERT(translator3.translateBack(
                    gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()})
                == "?");
    }

    GUM_ACTIVE_TEST(_trans3) {
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("toto");
      var.addLabel("titi");
      var.addLabel("tutu");

      gum::learning::DBTranslator4LabelizedVariable translator(var);
      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("toto"))
      TS_ASSERT_EQUALS(translator.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator.translate("toto").discr_val, (std::size_t)0)

      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("tutu"))
      TS_ASSERT_EQUALS(translator.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator.translate("tutu").discr_val, (std::size_t)2)
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "toto");
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "titi");

      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable translator2(var, missing);
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("toto"))
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("titi"))
      TS_ASSERT_EQUALS(translator2.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator2.translate("titi").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator2.translate("N/A").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator2.translate("?").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator2.translate("???").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_THROWS(translator2.translate("??"), const gum::UnknownLabelInDatabase&)
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "toto");
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "titi");
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "tutu");

      gum::learning::DBTranslator4LabelizedVariable translator3(var, missing, true, 3);
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("toto"))
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("titi"))
      TS_ASSERT_EQUALS(translator3.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator3.translate("titi").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator3.translate("N/A").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator3.translate("?").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator3.translate("???").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_THROWS(translator3.translate("??"), const gum::SizeError&)
      TS_ASSERT_THROWS(translator3.translate("a"), const gum::SizeError&)

      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "toto");
      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "titi");
      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "tutu");
      TS_ASSERT_THROWS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{3}}),
                       const gum::UnknownLabelInDatabase&)
      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{
                           std::numeric_limits< std::size_t >::max()}),
                       "?");

      TS_ASSERT_EQUALS(translator3.domainSize(), (gum::Size)3)

      TS_ASSERT_THROWS(
          gum::learning::DBTranslator4LabelizedVariable translator4(var, missing, true, 2),
          const gum::SizeError&)
    }

    GUM_ACTIVE_TEST(_trans4) {
      {
        gum::LabelizedVariable var("X1", "", 0);
        var.addLabel("1");
        var.addLabel("4");
        var.addLabel("2");

        gum::learning::DBTranslator4LabelizedVariable translator(var);
        TS_ASSERT_EQUALS(translator.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator.translate("4").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator.translate("2").discr_val, (std::size_t)2)
        TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                         "1");
        TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                         "4");
        TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                         "2");
        TS_ASSERT_EQUALS(translator.variable()->toString(), "X1:Labelized({1|4|2})")

        gum::LabelizedVariable var2("X2", "", 0);
        var2.addLabel("1");
        var2.addLabel("2");
        var2.addLabel("4");

        gum::learning::DBTranslator4LabelizedVariable translator2(var2);
        TS_ASSERT_EQUALS(translator2.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator2.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator2.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "4");
        TS_ASSERT_EQUALS(translator2.variable()->toString(), "X2:Labelized({1|2|4})")

        gum::learning::DBTranslator4LabelizedVariable translator3(translator);
        TS_ASSERT_EQUALS(translator3.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator3.translate("4").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator3.translate("2").discr_val, (std::size_t)2)
        TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "4");
        TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "2");
        TS_ASSERT_EQUALS(translator3.variable()->toString(), "X1:Labelized({1|4|2})")

        gum::learning::DBTranslator4LabelizedVariable translator4(translator2);
        TS_ASSERT_EQUALS(translator4.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator4.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator4.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "4");
        TS_ASSERT_EQUALS(translator4.variable()->toString(), "X2:Labelized({1|2|4})")

        gum::learning::DBTranslator4LabelizedVariable translator5(std::move(translator3));
        TS_ASSERT_EQUALS(translator5.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator5.translate("4").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator5.translate("2").discr_val, (std::size_t)2)
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "4");
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "2");
        TS_ASSERT_EQUALS(translator5.variable()->toString(), "X1:Labelized({1|4|2})")

        gum::learning::DBTranslator4LabelizedVariable translator6(std::move(translator4));
        TS_ASSERT_EQUALS(translator6.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator6.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator6.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "4");
        TS_ASSERT_EQUALS(translator6.variable()->toString(), "X2:Labelized({1|2|4})")

        gum::learning::DBTranslator4LabelizedVariable* translator7 = translator6.clone();
        TS_ASSERT_EQUALS(translator7->translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator7->translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator7->translate("4").discr_val, (std::size_t)2)
        TS_ASSERT(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "4");
        TS_ASSERT_EQUALS(translator7->variable()->toString(), "X2:Labelized({1|2|4})")

        delete translator7;

        translator5 = translator6;
        TS_ASSERT_EQUALS(translator5.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator5.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator5.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "4");
        TS_ASSERT_EQUALS(translator5.variable()->toString(), "X2:Labelized({1|2|4})")

        translator3 = translator5;
        TS_ASSERT_EQUALS(translator3.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator3.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator3.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "4");
        TS_ASSERT_EQUALS(translator3.variable()->toString(), "X2:Labelized({1|2|4})")

        translator5 = std::move(translator3);
        TS_ASSERT_EQUALS(translator5.translate("1").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(translator5.translate("2").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(translator5.translate("4").discr_val, (std::size_t)2)
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "4");
        TS_ASSERT_EQUALS(translator5.variable()->toString(), "X2:Labelized({1|2|4})")
      }
    }

    void xtest_trans5() {
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("1");
      var.addLabel("4");
      var.addLabel("2");

      gum::learning::DBTranslator4LabelizedVariable translator(var, false, 1000);

      TS_ASSERT_EQUALS(translator.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator.translate("4").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator.translate("2").discr_val, (std::size_t)2)
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "1");
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "4");
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "2");
      TS_ASSERT_EQUALS(translator.variable()->toString(), "X1:Labelized({1|4|2})")

      gum::LabelizedVariable var2("X2", "", 0);
      var2.addLabel("1");
      var2.addLabel("2");
      var2.addLabel("4");

      gum::learning::DBTranslator4LabelizedVariable translator2(var2);
      TS_ASSERT_EQUALS(translator2.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator2.translate("2").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator2.translate("4").discr_val, (std::size_t)2)
      TS_ASSERT_EQUALS(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "1");
      TS_ASSERT_EQUALS(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "2");
      TS_ASSERT_EQUALS(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "4");
      TS_ASSERT_EQUALS(translator2.variable()->toString(), "X2:Labelized({1|2|4})")

      gum::learning::DBTranslator4LabelizedVariable translator3(translator);
      TS_ASSERT_EQUALS(translator3.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator3.translate("4").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator3.translate("2").discr_val, (std::size_t)2)
      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "1");
      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "4");
      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "2");
      TS_ASSERT_EQUALS(translator3.variable()->toString(), "X1:Labelized({1|4|2})")

      gum::learning::DBTranslator4LabelizedVariable translator4(translator2);
      TS_ASSERT_EQUALS(translator4.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator4.translate("2").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator4.translate("4").discr_val, (std::size_t)2)
      TS_ASSERT_EQUALS(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "1");
      TS_ASSERT_EQUALS(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "2");
      TS_ASSERT_EQUALS(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "4");
      TS_ASSERT_EQUALS(translator4.variable()->toString(), "X2:Labelized({1|2|4})")

      gum::learning::DBTranslator4LabelizedVariable translator5(std::move(translator3));
      TS_ASSERT_EQUALS(translator5.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator5.translate("4").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator5.translate("2").discr_val, (std::size_t)2)
      TS_ASSERT_EQUALS(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "1");
      TS_ASSERT_EQUALS(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "4");
      TS_ASSERT_EQUALS(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "2");
      TS_ASSERT_EQUALS(translator5.variable()->toString(), "X1:Labelized({1|4|2})")

      gum::learning::DBTranslator4LabelizedVariable translator6(std::move(translator4));
      TS_ASSERT_EQUALS(translator6.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator6.translate("2").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator6.translate("4").discr_val, (std::size_t)2)
      TS_ASSERT_EQUALS(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "1");
      TS_ASSERT_EQUALS(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "2");
      TS_ASSERT_EQUALS(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "4");
      TS_ASSERT_EQUALS(translator6.variable()->toString(), "X2:Labelized({1|2|4})")

      gum::learning::DBTranslator4LabelizedVariable* translator7 = translator6.clone();
      TS_ASSERT_EQUALS(translator7->translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator7->translate("2").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator7->translate("4").discr_val, (std::size_t)2)
      TS_ASSERT(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                == "1");
      TS_ASSERT(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                == "2");
      TS_ASSERT(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                == "4");
      TS_ASSERT_EQUALS(translator7->variable()->toString(), "X2:Labelized({1|2|4})")

      delete translator7;

      translator5 = translator6;
      TS_ASSERT_EQUALS(translator5.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator5.translate("2").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator5.translate("4").discr_val, (std::size_t)2)
      TS_ASSERT_EQUALS(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "1");
      TS_ASSERT_EQUALS(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "2");
      TS_ASSERT_EQUALS(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "4");
      TS_ASSERT_EQUALS(translator5.variable()->toString(), "X2:Labelized({1|2|4})")

      translator3 = translator5;
      TS_ASSERT_EQUALS(translator3.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator3.translate("2").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator3.translate("4").discr_val, (std::size_t)2)
      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "1");
      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "2");
      TS_ASSERT_EQUALS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "4");
      TS_ASSERT_EQUALS(translator3.variable()->toString(), "X2:Labelized({1|2|4})")

      translator5 = std::move(translator3);
      TS_ASSERT_EQUALS(translator5.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator5.translate("2").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator5.translate("4").discr_val, (std::size_t)2)
      TS_ASSERT_EQUALS(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "1");
      TS_ASSERT_EQUALS(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "2");
      TS_ASSERT_EQUALS(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "4");
      TS_ASSERT_EQUALS(translator5.variable()->toString(), "X2:Labelized({1|2|4})")
    }

    GUM_ACTIVE_TEST(_trans6) {
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("toto");
      var.addLabel("titi");
      var.addLabel("???");

      std::vector< std::string > missing{"?", "N/A", "???"};

      gum::learning::DBTranslator4LabelizedVariable translator(var, missing);
      TS_ASSERT_EQUALS(translator.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator.translate("N/A").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator.translate("?").discr_val,
                       std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(translator.translate("???").discr_val, (std::size_t)2)

      TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                == "toto");
      TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                == "titi");
      TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                == "???");
    }

    GUM_ACTIVE_TEST(_trans7) {
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("7");
      var.addLabel("1");
      var.addLabel("3");

      std::vector< std::string > missing{"7", "N/A", "toto"};

      gum::learning::DBTranslator4LabelizedVariable translator(var, missing);
      TS_ASSERT(translator.needsReordering())
      TS_ASSERT_EQUALS(translator.domainSize(), (gum::Size)3)

      TS_ASSERT_EQUALS((translator << "7").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS((translator >> gum::learning::DBTranslatedValue{std::size_t{0}}), "7")
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "7");
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "1");
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "3");

      std::string back = translator.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()});

      gum::Set< std::string > missing_kept{"N/A", "toto"};
      TS_ASSERT(missing_kept.exists(back))
      TS_ASSERT_EQUALS(translator.missingSymbols(), missing_kept)

      auto new_order = translator.reorder();

      TS_ASSERT_EQUALS(new_order[0], (std::size_t)2)
      TS_ASSERT_EQUALS(new_order[1], (std::size_t)0)
      TS_ASSERT_EQUALS(new_order[2], (std::size_t)1)

      TS_ASSERT_EQUALS(translator.translate("7").discr_val, (std::size_t)2)
      TS_ASSERT_EQUALS(translator.translate("1").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(translator.translate("3").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "1");
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "3");
      TS_ASSERT_EQUALS(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "7");


      gum::LabelizedVariable var2("X2", "", 0);
      var2.addLabel("1");
      var2.addLabel("3");
      var2.addLabel("7");

      std::vector< std::string > missing2{"7", "N/A", "toto"};


      gum::learning::DBTranslator4LabelizedVariable translator2(var2, missing2);
      TS_ASSERT_EQUALS(translator2.needsReordering(), false)
      TS_ASSERT_EQUALS(translator2.domainSize(), (gum::Size)3)

      TS_ASSERT_EQUALS((translator2 << "7").discr_val, (std::size_t)2)
      TS_ASSERT_EQUALS((translator2 >> gum::learning::DBTranslatedValue{std::size_t{2}}), "7")
      TS_ASSERT_EQUALS(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}),
                       "1");
      TS_ASSERT_EQUALS(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}}),
                       "3");
      TS_ASSERT_EQUALS(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}),
                       "7");
      back = translator2.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()});
      TS_ASSERT(translator2.missingSymbols().exists(back))

      gum::Set< std::string > missing_kept2{"N/A", "toto"};
      TS_ASSERT_EQUALS(translator2.missingSymbols(), missing_kept2)

      auto new_order2 = translator2.reorder();
      TS_ASSERT_EQUALS(new_order2.size(), (gum::Size)0)
    }
  };

} /* namespace gum_tests */
