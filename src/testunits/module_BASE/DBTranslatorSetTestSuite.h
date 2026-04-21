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
#include <vector>

#include <agrum/base/database/DBTranslator4ContinuousVariable.h>
#include <agrum/base/database/DBTranslator4DiscretizedVariable.h>
#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslator4RangeVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DBTranslatorSet
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct DBTranslatorSetTestSuite {
    public:
    static void testTrans1() {
      gum::learning::DBTranslatorSet set;

      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable translator1(missing, 3);
      std::size_t                                   pos;
      pos = set.insertTranslator(translator1, 1);
      CHECK_EQ(pos, std::size_t(0));

      gum::learning::DBTranslator4ContinuousVariable translator0;
      pos = set.insertTranslator(translator0, 0);
      CHECK_EQ(pos, std::size_t(1));

      gum::learning::DBTranslator4LabelizedVariable translator2;
      pos = set.insertTranslator(translator2, 2);
      CHECK_EQ(pos, std::size_t(2));

      std::vector< std::string > row1{".33", "toto", "titi"};
      CHECK_EQ(set.translate(row1, 0).discr_val, (std::size_t)0);
      CHECK_EQ(set.translate(row1, 1).cont_val, 0.33f);
      CHECK_EQ(set.translate(row1, 2).discr_val, (std::size_t)0);

      std::vector< std::string > row2{".22", "titi", "toto"};
      CHECK_EQ(set.translate(row2, 0).discr_val, (std::size_t)1);
      CHECK_EQ(set.translate(row2, 1).cont_val, 0.22f);
      CHECK_EQ(set.translate(row2, 2).discr_val, (std::size_t)1);

      std::vector< std::string > row3{".33", "toto", "toto"};
      CHECK_EQ(set.translate(row3, 0).discr_val, (std::size_t)0);
      CHECK_EQ(set.translate(row3, 1).cont_val, 0.33f);
      CHECK_EQ(set.translate(row3, 2).discr_val, (std::size_t)1);

      std::vector< std::string > row4{"???", "???", "???"};
      CHECK_EQ(set.translate(row4, 0).discr_val, std::numeric_limits< std::size_t >::max());
      CHECK_THROWS_AS(set.translate(row4, 1), const gum::TypeError&);

      std::vector< std::string > row5{"4.22x", "???", "???"};
      CHECK_THROWS_AS(set.translate(row5, 1), const gum::TypeError&);
      CHECK_EQ(set.translate(row5, 2).discr_val, (std::size_t)2);

      CHECK_THROWS_AS(set.translateSafe(row5, 3), const gum::UndefinedElement&);

      CHECK_EQ(set.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}, 0), "toto");
      CHECK_EQ(std::stof(set.translateBack(gum::learning::DBTranslatedValue{7.42f}, 1)), 7.42f);
      CHECK_EQ(set.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}, 2), "???");
      CHECK_THROWS_AS(set.translateBackSafe(gum::learning::DBTranslatedValue{std::size_t{0}}, 3),
                      const gum::UndefinedElement&);

      CHECK_EQ(set.domainSize(0), (std::size_t)2);
      CHECK_EQ(set.domainSize(2), (std::size_t)3);
      CHECK_EQ(set.domainSize(1), std::numeric_limits< std::size_t >::max());
      CHECK_EQ(set.domainSizeSafe(0), (std::size_t)2);
      CHECK_EQ(set.domainSizeSafe(2), (std::size_t)3);
      CHECK_EQ(set.domainSizeSafe(1), std::numeric_limits< std::size_t >::max());

      gum::learning::DBTranslator4LabelizedVariable xtrans
          = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(set.translator(0));
      CHECK_EQ(xtrans.translate("toto").discr_val, (std::size_t)0);
      CHECK_EQ(xtrans.translate("titi").discr_val, (std::size_t)1);
      CHECK_EQ(xtrans.translate("???").discr_val, std::numeric_limits< std::size_t >::max());

      gum::learning::DBTranslator4LabelizedVariable xtransb
          = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
              set.translatorSafe(0));
      CHECK_EQ(xtransb.translate("toto").discr_val, (std::size_t)0);
      CHECK_EQ(xtransb.translate("titi").discr_val, (std::size_t)1);
      CHECK_EQ(xtransb.translate("???").discr_val, std::numeric_limits< std::size_t >::max());

      CHECK_EQ(set.nbTranslators(), (std::size_t)3);
      CHECK_EQ(set.size(), (std::size_t)3);

      gum::LabelizedVariable var = dynamic_cast< const gum::LabelizedVariable& >(set.variable(0));
      CHECK_EQ(var.domainSize(), static_cast< gum::Size >(2));
      CHECK_EQ(var.label(0), "toto");
      CHECK_EQ(var.label(1), "titi");

      gum::LabelizedVariable varb
          = dynamic_cast< const gum::LabelizedVariable& >(set.variableSafe(0));
      CHECK_EQ(varb.domainSize(), static_cast< gum::Size >(2));
      CHECK_EQ(varb.label(0), "toto");
      CHECK_EQ(varb.label(1), "titi");

      const gum::learning::DBTranslatedValue miss_disc{std::numeric_limits< std::size_t >::max()};
      const gum::learning::DBTranslatedValue miss_cont{std::numeric_limits< float >::max()};
      CHECK(set.isMissingValue(miss_disc, 0));
      CHECK(set.isMissingValue(miss_cont, 1));
      CHECK(set.isMissingValue(miss_disc, 2));
      CHECK(set.isMissingValueSafe(miss_disc, 0));
      CHECK(set.isMissingValueSafe(miss_cont, 1));
      CHECK(set.isMissingValueSafe(miss_disc, 2));

      CHECK(set.needsReordering(0));
      CHECK(set.needsReorderingSafe(0));
      const auto order = set.reorder(0);
      CHECK_EQ(order[0], std::size_t(1));
      CHECK_EQ(order[1], std::size_t(0));

      CHECK_EQ(set.inputColumn(0), std::size_t(1));
      CHECK_EQ(set.inputColumn(1), std::size_t(0));
      CHECK_EQ(set.inputColumn(2), std::size_t(2));
      CHECK_EQ(set.inputColumnSafe(0), std::size_t(1));
      CHECK_EQ(set.inputColumnSafe(1), std::size_t(0));
      CHECK_EQ(set.inputColumnSafe(2), std::size_t(2));
      CHECK_EQ(set.highestInputColumn(), std::size_t(2));

      set.clear();
      CHECK_EQ(set.nbTranslators(), (std::size_t)0);
    }   // namespace gum_tests

    static void testTrans2() {
      {
        gum::learning::DBTranslatorSet set;

        std::vector< std::string >                    missing{"?", "N/A", "???"};
        gum::learning::DBTranslator4LabelizedVariable translator1(missing, 3);
        set.insertTranslator(translator1, 1);

        gum::learning::DBTranslator4ContinuousVariable translator0;
        set.insertTranslator(translator0, 0);

        gum::learning::DBTranslator4LabelizedVariable translator2;
        set.insertTranslator(translator2, 2);

        std::vector< std::string > row1{".33", "toto", "titi"};
        CHECK_EQ(set.translate(row1, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set.translate(row1, 1).cont_val, 0.33f);
        CHECK_EQ(set.translate(row1, 2).discr_val, (std::size_t)0);

        std::vector< std::string > row2{".22", "titi", "toto"};
        CHECK_EQ(set.translate(row2, 0).discr_val, (std::size_t)1);
        CHECK_EQ(set.translate(row2, 1).cont_val, 0.22f);
        CHECK_EQ(set.translate(row2, 2).discr_val, (std::size_t)1);

        std::vector< std::string > row3{".33", "toto", "toto"};
        CHECK_EQ(set.translate(row3, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set.translate(row3, 1).cont_val, 0.33f);
        CHECK_EQ(set.translate(row3, 2).discr_val, (std::size_t)1);

        gum::learning::DBTranslator4LabelizedVariable xtrans
            = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
                set.translator(0));
        CHECK_EQ(xtrans.translate("toto").discr_val, (std::size_t)0);
        CHECK_EQ(xtrans.translate("titi").discr_val, (std::size_t)1);
        CHECK_EQ(xtrans.translate("???").discr_val, std::numeric_limits< std::size_t >::max());

        gum::learning::DBTranslatorSet set2(set);
        CHECK_EQ(set2.translate(row1, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set2.translate(row1, 1).cont_val, 0.33f);
        CHECK_EQ(set2.translate(row1, 2).discr_val, (std::size_t)0);
        CHECK_EQ(set2.translate(row2, 0).discr_val, (std::size_t)1);
        CHECK_EQ(set2.translate(row2, 1).cont_val, 0.22f);
        CHECK_EQ(set2.translate(row2, 2).discr_val, (std::size_t)1);
        CHECK_EQ(set2.translate(row3, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set2.translate(row3, 1).cont_val, 0.33f);
        CHECK_EQ(set2.translate(row3, 2).discr_val, (std::size_t)1);
        gum::learning::DBTranslator4LabelizedVariable xtrans2
            = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
                set2.translator(0));
        CHECK_EQ(xtrans2.translate("toto").discr_val, (std::size_t)0);
        CHECK_EQ(xtrans2.translate("titi").discr_val, (std::size_t)1);

        gum::learning::DBTranslatorSet set2bis(set2);
        gum::learning::DBTranslatorSet set3(std::move(set2));
        CHECK_EQ(set3.translate(row1, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set3.translate(row1, 1).cont_val, 0.33f);
        CHECK_EQ(set3.translate(row1, 2).discr_val, (std::size_t)0);
        CHECK_EQ(set3.translate(row2, 0).discr_val, (std::size_t)1);
        CHECK_EQ(set3.translate(row2, 1).cont_val, 0.22f);
        CHECK_EQ(set3.translate(row2, 2).discr_val, (std::size_t)1);
        CHECK_EQ(set3.translate(row3, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set3.translate(row3, 1).cont_val, 0.33f);
        CHECK_EQ(set3.translate(row3, 2).discr_val, (std::size_t)1);
        gum::learning::DBTranslator4LabelizedVariable xtrans3
            = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
                set3.translator(0));
        CHECK_EQ(xtrans3.translate("toto").discr_val, (std::size_t)0);
        CHECK_EQ(xtrans3.translate("titi").discr_val, (std::size_t)1);

        gum::learning::DBTranslatorSet set4(set);
        CHECK_EQ(set4.translate(row1, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set4.translate(row1, 1).cont_val, 0.33f);
        CHECK_EQ(set4.translate(row1, 2).discr_val, (std::size_t)0);
        CHECK_EQ(set4.translate(row2, 0).discr_val, (std::size_t)1);
        CHECK_EQ(set4.translate(row2, 1).cont_val, 0.22f);
        CHECK_EQ(set4.translate(row2, 2).discr_val, (std::size_t)1);
        CHECK_EQ(set4.translate(row3, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set4.translate(row3, 1).cont_val, 0.33f);
        CHECK_EQ(set4.translate(row3, 2).discr_val, (std::size_t)1);
        gum::learning::DBTranslator4LabelizedVariable xtrans4
            = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
                set4.translator(0));
        CHECK_EQ(xtrans4.translate("toto").discr_val, (std::size_t)0);
        CHECK_EQ(xtrans4.translate("titi").discr_val, (std::size_t)1);

        gum::learning::DBTranslatorSet set5(std::move(set2bis));
        CHECK_EQ(set5.translate(row1, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set5.translate(row1, 1).cont_val, 0.33f);
        CHECK_EQ(set5.translate(row1, 2).discr_val, (std::size_t)0);
        CHECK_EQ(set5.translate(row2, 0).discr_val, (std::size_t)1);
        CHECK_EQ(set5.translate(row2, 1).cont_val, 0.22f);
        CHECK_EQ(set5.translate(row2, 2).discr_val, (std::size_t)1);
        CHECK_EQ(set5.translate(row3, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set5.translate(row3, 1).cont_val, 0.33f);
        CHECK_EQ(set5.translate(row3, 2).discr_val, (std::size_t)1);
        gum::learning::DBTranslator4LabelizedVariable xtrans5
            = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
                set5.translator(0));
        CHECK_EQ(xtrans5.translate("toto").discr_val, (std::size_t)0);
        CHECK_EQ(xtrans5.translate("titi").discr_val, (std::size_t)1);

        const auto ordersafe = set.reorderSafe(0);
        CHECK_EQ(ordersafe[0], std::size_t(1));
        CHECK_EQ(ordersafe[1], std::size_t(0));
      }
    }

    static void testTrans3() {
      gum::learning::DBTranslatorSet set;

      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable translator1(missing, 3);
      set.insertTranslator(translator1, 1);

      gum::learning::DBTranslator4ContinuousVariable translator0;
      set.insertTranslator(translator0, 0);

      gum::learning::DBTranslator4LabelizedVariable translator2;
      set.insertTranslator(translator2, 2);

      std::vector< std::string > row1{".33", "toto", "titi"};
      CHECK_EQ(set.translate(row1, 0).discr_val, (std::size_t)0);
      CHECK_EQ(set.translate(row1, 1).cont_val, 0.33f);
      CHECK_EQ(set.translate(row1, 2).discr_val, (std::size_t)0);

      std::vector< std::string > row2{".22", "titi", "toto"};
      CHECK_EQ(set.translate(row2, 0).discr_val, (std::size_t)1);
      CHECK_EQ(set.translate(row2, 1).cont_val, 0.22f);
      CHECK_EQ(set.translate(row2, 2).discr_val, (std::size_t)1);

      std::vector< std::string > row3{".33", "toto", "toto"};
      CHECK_EQ(set.translate(row3, 0).discr_val, (std::size_t)0);
      CHECK_EQ(set.translate(row3, 1).cont_val, 0.33f);
      CHECK_EQ(set.translate(row3, 2).discr_val, (std::size_t)1);


      gum::learning::DBTranslatorSet* set4 = set.clone();
      CHECK_EQ(set4->translate(row1, 0).discr_val, (std::size_t)0);
      CHECK_EQ(set4->translate(row1, 1).cont_val, 0.33f);
      CHECK_EQ(set4->translate(row1, 2).discr_val, (std::size_t)0);
      CHECK_EQ(set4->translate(row2, 0).discr_val, (std::size_t)1);
      CHECK_EQ(set4->translate(row2, 1).cont_val, 0.22f);
      CHECK_EQ(set4->translate(row2, 2).discr_val, (std::size_t)1);
      CHECK_EQ(set4->translate(row3, 0).discr_val, (std::size_t)0);
      CHECK_EQ(set4->translate(row3, 1).cont_val, 0.33f);
      CHECK_EQ(set4->translate(row3, 2).discr_val, (std::size_t)1);
      gum::learning::DBTranslator4LabelizedVariable xtrans4
          = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
              set4->translator(0));
      CHECK_EQ(xtrans4.translate("toto").discr_val, (std::size_t)0);
      CHECK_EQ(xtrans4.translate("titi").discr_val, (std::size_t)1);

      delete set4;
    }

    static void testTrans4() {
      {
        gum::learning::DBTranslatorSet set;

        std::vector< std::string >                    missing{"?", "N/A", "???"};
        gum::learning::DBTranslator4LabelizedVariable translator1(missing, 3);
        set.insertTranslator(translator1, 1);

        gum::learning::DBTranslator4ContinuousVariable translator0;
        set.insertTranslator(translator0, 0);

        gum::learning::DBTranslator4LabelizedVariable translator2;
        set.insertTranslator(translator2, 2);

        std::vector< std::string > row1{".33", "toto", "titi"};
        CHECK_EQ(set.translate(row1, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set.translate(row1, 1).cont_val, 0.33f);
        CHECK_EQ(set.translate(row1, 2).discr_val, (std::size_t)0);

        std::vector< std::string > row2{".22", "titi", "toto"};
        CHECK_EQ(set.translate(row2, 0).discr_val, (std::size_t)1);
        CHECK_EQ(set.translate(row2, 1).cont_val, 0.22f);
        CHECK_EQ(set.translate(row2, 2).discr_val, (std::size_t)1);

        std::vector< std::string > row3{".33", "toto", "toto"};
        CHECK_EQ(set.translate(row3, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set.translate(row3, 1).cont_val, 0.33f);
        CHECK_EQ(set.translate(row3, 2).discr_val, (std::size_t)1);

        gum::learning::DBTranslatorSet* set4 = set.clone();
        CHECK_EQ(set4->translate(row1, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set4->translate(row1, 1).cont_val, 0.33f);
        CHECK_EQ(set4->translate(row1, 2).discr_val, (std::size_t)0);
        CHECK_EQ(set4->translate(row2, 0).discr_val, (std::size_t)1);
        CHECK_EQ(set4->translate(row2, 1).cont_val, 0.22f);
        CHECK_EQ(set4->translate(row2, 2).discr_val, (std::size_t)1);
        CHECK_EQ(set4->translate(row3, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set4->translate(row3, 1).cont_val, 0.33f);
        CHECK_EQ(set4->translate(row3, 2).discr_val, (std::size_t)1);
        gum::learning::DBTranslator4LabelizedVariable xtrans4
            = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
                set4->translator(0));
        CHECK_EQ(xtrans4.translate("toto").discr_val, (std::size_t)0);
        CHECK_EQ(xtrans4.translate("titi").discr_val, (std::size_t)1);

        gum::learning::DBTranslator4LabelizedVariable xtrans4b
            = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
                set4->operator[](0));
        CHECK_EQ(xtrans4b.translate("toto").discr_val, (std::size_t)0);
        CHECK_EQ(xtrans4b.translate("titi").discr_val, (std::size_t)1);

        gum::learning::DBTranslator4LabelizedVariable xtrans4t
            = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
                set4->translatorSafe(0));
        CHECK_EQ(xtrans4t.translate("toto").discr_val, (std::size_t)0);
        CHECK_EQ(xtrans4t.translate("titi").discr_val, (std::size_t)1);

        delete set4;

        gum::learning::DBTranslatorSet set2;
        CHECK_EQ(set2.nbTranslators(), (std::size_t)0);
        set2 = set;
        CHECK_EQ(set2.translate(row3, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set2.translate(row3, 1).cont_val, 0.33f);
        CHECK_EQ(set2.translate(row3, 2).discr_val, (std::size_t)1);
        CHECK_EQ(set2.translate(row3, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set2.translate(row3, 1).cont_val, 0.33f);
        CHECK_EQ(set2.translate(row3, 2).discr_val, (std::size_t)1);

        gum::learning::DBTranslatorSet set3;
        CHECK_EQ(set3.nbTranslators(), (std::size_t)0);
        set3 = std::move(set);
        CHECK_EQ(set3.translate(row3, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set3.translate(row3, 1).cont_val, 0.33f);
        CHECK_EQ(set3.translate(row3, 2).discr_val, (std::size_t)1);
        CHECK_EQ(set3.translate(row3, 0).discr_val, (std::size_t)0);
        CHECK_EQ(set3.translate(row3, 1).cont_val, 0.33f);
        CHECK_EQ(set3.translate(row3, 2).discr_val, (std::size_t)1);
      }
    }

    static void testTrans5() {
      gum::learning::DBTranslatorSet set;
      std::size_t                    pos;

      std::vector< std::string > missing{"?", "N/A", "???"};
      gum::LabelizedVariable     var1("var1", "", 0);
      var1.addLabel("toto");
      var1.addLabel("titi");
      pos = set.insertTranslator(var1, 1, missing, false);
      CHECK_EQ(pos, std::size_t(0));

      gum::ContinuousVariable< double > var0("var0", "");
      pos = set.insertTranslator(var0, 0, missing);
      CHECK_EQ(pos, std::size_t(1));

      gum::LabelizedVariable var2("var2", "", 0);
      var2.addLabel("titi");
      var2.addLabel("toto");
      pos = set.insertTranslator(var2, 2, missing);
      CHECK_EQ(pos, std::size_t(2));

      std::vector< std::string > row1{".33", "toto", "titi"};
      CHECK_EQ(set.translate(row1, 0).discr_val, (std::size_t)0);
      CHECK_EQ(set.translate(row1, 1).cont_val, 0.33f);
      CHECK_EQ(set.translate(row1, 2).discr_val, (std::size_t)0);

      std::vector< std::string > row2{".22", "titi", "toto"};
      CHECK_EQ(set.translate(row2, 0).discr_val, (std::size_t)1);
      CHECK_EQ(set.translate(row2, 1).cont_val, 0.22f);
      CHECK_EQ(set.translate(row2, 2).discr_val, (std::size_t)1);

      std::vector< std::string > row3{".33", "toto", "toto"};
      CHECK_EQ(set.translate(row3, 0).discr_val, (std::size_t)0);
      CHECK_EQ(set.translate(row3, 1).cont_val, 0.33f);
      CHECK_EQ(set.translate(row3, 2).discr_val, (std::size_t)1);
    }

    static void testTrans6() {
      gum::learning::DBTranslatorSet set;

      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable translator1(missing, 3);
      std::size_t                                   pos;
      pos = set.insertTranslator(translator1, 1, false);
      CHECK_EQ(pos, std::size_t(0));

      gum::learning::DBTranslator4ContinuousVariable translator0;
      pos = set.insertTranslator(translator0, 0);
      CHECK_EQ(pos, std::size_t(1));

      gum::learning::DBTranslator4LabelizedVariable translator2;
      pos = set.insertTranslator(translator2, 2);
      CHECK_EQ(pos, std::size_t(2));

      pos = set.insertTranslator(translator1, 1, false);
      CHECK_EQ(pos, std::size_t(3));

      std::vector< std::string > row1{".33", "toto", "titi"};
      CHECK_EQ(set.translate(row1, 0).discr_val, (std::size_t)0);
      CHECK_EQ(set.translate(row1, 1).cont_val, 0.33f);
      CHECK_EQ(set.translate(row1, 2).discr_val, (std::size_t)0);
      CHECK_EQ(set.translate(row1, 3).discr_val, (std::size_t)0);

      std::vector< std::string > row2{".22", "titi", "toto"};
      CHECK_EQ(set.translate(row2, 0).discr_val, (std::size_t)1);
      CHECK_EQ(set.translate(row2, 1).cont_val, 0.22f);
      CHECK_EQ(set.translate(row2, 2).discr_val, (std::size_t)1);
      CHECK_EQ(set.translate(row2, 3).discr_val, (std::size_t)1);

      std::vector< std::string > row3{".33", "toto", "toto"};
      CHECK_EQ(set.translate(row3, 0).discr_val, (std::size_t)0);
      CHECK_EQ(set.translate(row3, 1).cont_val, 0.33f);
      CHECK_EQ(set.translate(row3, 2).discr_val, (std::size_t)1);
      CHECK_EQ(set.translate(row3, 3).discr_val, (std::size_t)0);

      std::vector< std::string > row4{"???", "???", "???"};
      CHECK_EQ(set.translate(row4, 0).discr_val, std::numeric_limits< std::size_t >::max());
      CHECK_THROWS_AS(set.translate(row4, 1), const gum::TypeError&);
      CHECK_EQ(set.translate(row4, 3).discr_val, std::numeric_limits< std::size_t >::max());

      std::vector< std::string > row5{"4.22x", "???", "???"};
      CHECK_THROWS_AS(set.translate(row5, 1), const gum::TypeError&);
      CHECK_EQ(set.translate(row5, 2).discr_val, (std::size_t)2);

      CHECK_THROWS_AS(set.translateSafe(row5, 4), const gum::UndefinedElement&);

      CHECK_EQ(set.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}, 0), "toto");
      CHECK_EQ(set.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}, 3), "toto");
      CHECK_EQ(std::stof(set.translateBack(gum::learning::DBTranslatedValue{7.42f}, 1)), 7.42f);
      CHECK_EQ(set.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}, 2), "???");

      CHECK_EQ(set.domainSize(0), (std::size_t)2);
      CHECK_EQ(set.domainSize(2), (std::size_t)3);
      CHECK_EQ(set.domainSize(1), std::numeric_limits< std::size_t >::max());
      CHECK_EQ(set.domainSize(3), (std::size_t)2);
      CHECK_EQ(set.domainSizeSafe(0), (std::size_t)2);
      CHECK_EQ(set.domainSizeSafe(2), (std::size_t)3);
      CHECK_EQ(set.domainSizeSafe(1), std::numeric_limits< std::size_t >::max());
      CHECK_EQ(set.domainSizeSafe(3), (std::size_t)2);
    }

    static void testTrans7() {
      gum::learning::DBTranslatorSet set;
      std::size_t                    pos;

      gum::LabelizedVariable var1("var1", "", 0);
      var1.addLabel("toto");
      var1.addLabel("titi");
      pos = set.insertTranslator(var1, 1, false);
      CHECK_EQ(pos, std::size_t(0));

      gum::ContinuousVariable< double > var0("var0", "");
      pos = set.insertTranslator(var0, 0);
      CHECK_EQ(pos, std::size_t(1));

      gum::LabelizedVariable var2("var2", "", 0);
      var2.addLabel("titi");
      var2.addLabel("toto");
      pos = set.insertTranslator(var2, 2);
      CHECK_EQ(pos, std::size_t(2));

      std::vector< std::string > row1{".33", "toto", "titi"};
      CHECK_EQ(set.translate(row1, 0).discr_val, (std::size_t)0);
      CHECK_EQ(set.translate(row1, 1).cont_val, 0.33f);
      CHECK_EQ(set.translate(row1, 2).discr_val, (std::size_t)0);
    }

    static void testTrans8() {
      gum::learning::DBTranslatorSet set;

      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable translator1(missing, 3);
      translator1.setVariableName("X1");

      std::size_t pos;
      pos = set.insertTranslator(translator1, 1);
      CHECK_EQ(pos, std::size_t(0));

      gum::learning::DBTranslator4ContinuousVariable translator0;
      translator0.setVariableName("X0");
      pos = set.insertTranslator(translator0, 0);
      CHECK_EQ(pos, std::size_t(1));

      gum::learning::DBTranslator4LabelizedVariable translator2;
      translator2.setVariableName("X2");
      pos = set.insertTranslator(translator2, 2);
      CHECK_EQ(pos, std::size_t(2));

      translator1.setVariableName("Y1");
      pos = set.insertTranslator(translator1, 1, false);
      CHECK_EQ(pos, std::size_t(3));

      translator0.setVariableName("Y0");
      pos = set.insertTranslator(translator0, 0, false);
      CHECK_EQ(pos, std::size_t(4));

      translator0.setVariableName("Z0");
      pos = set.insertTranslator(translator0, 0, false);
      CHECK_EQ(pos, std::size_t(5));

      translator1.setVariableName("Z1");
      pos = set.insertTranslator(translator1, 1, false);
      CHECK_EQ(pos, std::size_t(6));

      const auto& vectTrans = set.translators();
      CHECK_EQ(vectTrans.size(), std::size_t(7));

      CHECK_EQ(vectTrans[0]->variable()->name(), "X1");
      CHECK_EQ(vectTrans[1]->variable()->name(), "X0");
      CHECK_EQ(vectTrans[2]->variable()->name(), "X2");
      CHECK_EQ(vectTrans[3]->variable()->name(), "Y1");
      CHECK_EQ(vectTrans[4]->variable()->name(), "Y0");
      CHECK_EQ(vectTrans[5]->variable()->name(), "Z0");
      CHECK_EQ(vectTrans[6]->variable()->name(), "Z1");
      CHECK_EQ(set.highestInputColumn(), std::size_t(2));

      set.eraseTranslator(4, false);
      CHECK_EQ(vectTrans.size(), std::size_t(6));
      CHECK_EQ(vectTrans[0]->variable()->name(), "X1");
      CHECK_EQ(vectTrans[1]->variable()->name(), "X0");
      CHECK_EQ(vectTrans[2]->variable()->name(), "X2");
      CHECK_EQ(vectTrans[3]->variable()->name(), "Y1");
      CHECK_EQ(vectTrans[4]->variable()->name(), "Z0");
      CHECK_EQ(vectTrans[5]->variable()->name(), "Z1");
      CHECK_EQ(set.highestInputColumn(), std::size_t(2));

      set.eraseTranslator(0, true);
      CHECK_EQ(vectTrans.size(), std::size_t(4));
      CHECK_EQ(vectTrans[0]->variable()->name(), "X1");
      CHECK_EQ(vectTrans[1]->variable()->name(), "X2");
      CHECK_EQ(vectTrans[2]->variable()->name(), "Y1");
      CHECK_EQ(vectTrans[3]->variable()->name(), "Z1");
      CHECK_EQ(set.highestInputColumn(), std::size_t(2));

      set.eraseTranslator(2, true);
      CHECK_EQ(vectTrans.size(), std::size_t(3));
      CHECK_EQ(vectTrans[0]->variable()->name(), "X1");
      CHECK_EQ(vectTrans[1]->variable()->name(), "Y1");
      CHECK_EQ(vectTrans[2]->variable()->name(), "Z1");
      CHECK_EQ(set.highestInputColumn(), std::size_t(1));

      set.eraseTranslator(1, true);
      CHECK_EQ(vectTrans.size(), std::size_t(0));
      CHECK_EQ(set.highestInputColumn(), std::size_t(0));
    }
  };

  GUM_TEST_ACTIF(Trans1)
  GUM_TEST_ACTIF(Trans2)
  GUM_TEST_ACTIF(Trans3)
  GUM_TEST_ACTIF(Trans4)
  GUM_TEST_ACTIF(Trans5)
  GUM_TEST_ACTIF(Trans6)
  GUM_TEST_ACTIF(Trans7)
  GUM_TEST_ACTIF(Trans8)

} /* namespace gum_tests */
