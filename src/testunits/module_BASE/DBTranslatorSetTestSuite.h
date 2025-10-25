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
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/database/DBTranslator4ContinuousVariable.h>
#include <agrum/base/database/DBTranslator4DiscretizedVariable.h>
#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslator4RangeVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>

namespace gum_tests {

  class GUM_TEST_SUITE(DBTranslatorSet) {
    public:
    GUM_ACTIVE_TEST(_trans1) {
      gum::learning::DBTranslatorSet set;

      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable translator1(missing, 3);
      std::size_t                                   pos;
      pos = set.insertTranslator(translator1, 1);
      TS_ASSERT_EQUALS(pos, std::size_t(0))

      gum::learning::DBTranslator4ContinuousVariable translator0;
      pos = set.insertTranslator(translator0, 0);
      TS_ASSERT_EQUALS(pos, std::size_t(1))

      gum::learning::DBTranslator4LabelizedVariable translator2;
      pos = set.insertTranslator(translator2, 2);
      TS_ASSERT_EQUALS(pos, std::size_t(2))

      std::vector< std::string > row1{".33", "toto", "titi"};
      TS_ASSERT_EQUALS(set.translate(row1, 0).discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(set.translate(row1, 1).cont_val, 0.33f)
      TS_ASSERT_EQUALS(set.translate(row1, 2).discr_val, (std::size_t)0)

      std::vector< std::string > row2{".22", "titi", "toto"};
      TS_ASSERT_EQUALS(set.translate(row2, 0).discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(set.translate(row2, 1).cont_val, 0.22f)
      TS_ASSERT_EQUALS(set.translate(row2, 2).discr_val, (std::size_t)1)

      std::vector< std::string > row3{".33", "toto", "toto"};
      TS_ASSERT_EQUALS(set.translate(row3, 0).discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(set.translate(row3, 1).cont_val, 0.33f)
      TS_ASSERT_EQUALS(set.translate(row3, 2).discr_val, (std::size_t)1)

      std::vector< std::string > row4{"???", "???", "???"};
      TS_ASSERT_EQUALS(set.translate(row4, 0).discr_val, std::numeric_limits< std::size_t >::max())
      TS_ASSERT_THROWS(set.translate(row4, 1), const gum::TypeError&)

      std::vector< std::string > row5{"4.22x", "???", "???"};
      TS_ASSERT_THROWS(set.translate(row5, 1), const gum::TypeError&)
      TS_ASSERT_EQUALS(set.translate(row5, 2).discr_val, (std::size_t)2)

      TS_ASSERT_THROWS(set.translateSafe(row5, 3), const gum::UndefinedElement&)

      TS_ASSERT_EQUALS(set.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}, 0),
                       "toto");
      TS_ASSERT_EQUALS(std::stof(set.translateBack(gum::learning::DBTranslatedValue{7.42f}, 1)),
                       7.42f);
      TS_ASSERT_EQUALS(set.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}, 2),
                       "???");
      TS_ASSERT_THROWS(set.translateBackSafe(gum::learning::DBTranslatedValue{std::size_t{0}}, 3),
                       const gum::UndefinedElement&)

      TS_ASSERT_EQUALS(set.domainSize(0), (std::size_t)2)
      TS_ASSERT_EQUALS(set.domainSize(2), (std::size_t)3)
      TS_ASSERT_EQUALS(set.domainSize(1), std::numeric_limits< std::size_t >::max())
      TS_ASSERT_EQUALS(set.domainSizeSafe(0), (std::size_t)2)
      TS_ASSERT_EQUALS(set.domainSizeSafe(2), (std::size_t)3)
      TS_ASSERT_EQUALS(set.domainSizeSafe(1), std::numeric_limits< std::size_t >::max())

      gum::learning::DBTranslator4LabelizedVariable xtrans
          = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(set.translator(0));
      TS_ASSERT_EQUALS(xtrans.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(xtrans.translate("titi").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(xtrans.translate("???").discr_val,
                       std::numeric_limits< std::size_t >::max());

      gum::learning::DBTranslator4LabelizedVariable xtransb
          = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
              set.translatorSafe(0));
      TS_ASSERT_EQUALS(xtransb.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(xtransb.translate("titi").discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(xtransb.translate("???").discr_val,
                       std::numeric_limits< std::size_t >::max());

      TS_ASSERT_EQUALS(set.nbTranslators(), (std::size_t)3)
      TS_ASSERT_EQUALS(set.size(), (std::size_t)3)

      gum::LabelizedVariable var = dynamic_cast< const gum::LabelizedVariable& >(set.variable(0));
      TS_ASSERT_EQUALS(var.domainSize(), static_cast< gum::Size >(2))
      TS_ASSERT_EQUALS(var.label(0), "toto")
      TS_ASSERT_EQUALS(var.label(1), "titi")

      gum::LabelizedVariable varb
          = dynamic_cast< const gum::LabelizedVariable& >(set.variableSafe(0));
      TS_ASSERT_EQUALS(varb.domainSize(), static_cast< gum::Size >(2))
      TS_ASSERT_EQUALS(varb.label(0), "toto")
      TS_ASSERT_EQUALS(varb.label(1), "titi")

      const gum::learning::DBTranslatedValue miss_disc{std::numeric_limits< std::size_t >::max()};
      const gum::learning::DBTranslatedValue miss_cont{std::numeric_limits< float >::max()};
      TS_ASSERT(set.isMissingValue(miss_disc, 0))
      TS_ASSERT(set.isMissingValue(miss_cont, 1))
      TS_ASSERT(set.isMissingValue(miss_disc, 2))
      TS_ASSERT(set.isMissingValueSafe(miss_disc, 0))
      TS_ASSERT(set.isMissingValueSafe(miss_cont, 1))
      TS_ASSERT(set.isMissingValueSafe(miss_disc, 2))

      TS_ASSERT(set.needsReordering(0))
      TS_ASSERT(set.needsReorderingSafe(0))
      const auto order = set.reorder(0);
      TS_ASSERT_EQUALS(order[0], std::size_t(1))
      TS_ASSERT_EQUALS(order[1], std::size_t(0))

      TS_ASSERT_EQUALS(set.inputColumn(0), std::size_t(1))
      TS_ASSERT_EQUALS(set.inputColumn(1), std::size_t(0))
      TS_ASSERT_EQUALS(set.inputColumn(2), std::size_t(2))
      TS_ASSERT_EQUALS(set.inputColumnSafe(0), std::size_t(1))
      TS_ASSERT_EQUALS(set.inputColumnSafe(1), std::size_t(0))
      TS_ASSERT_EQUALS(set.inputColumnSafe(2), std::size_t(2))
      TS_ASSERT_EQUALS(set.highestInputColumn(), std::size_t(2))

      set.clear();
      TS_ASSERT_EQUALS(set.nbTranslators(), (std::size_t)0)
    }   // namespace gum_tests

    GUM_ACTIVE_TEST(_trans2) {
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
        TS_ASSERT_EQUALS(set.translate(row1, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set.translate(row1, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set.translate(row1, 2).discr_val, (std::size_t)0)

        std::vector< std::string > row2{".22", "titi", "toto"};
        TS_ASSERT_EQUALS(set.translate(row2, 0).discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(set.translate(row2, 1).cont_val, 0.22f)
        TS_ASSERT_EQUALS(set.translate(row2, 2).discr_val, (std::size_t)1)

        std::vector< std::string > row3{".33", "toto", "toto"};
        TS_ASSERT_EQUALS(set.translate(row3, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set.translate(row3, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set.translate(row3, 2).discr_val, (std::size_t)1)

        gum::learning::DBTranslator4LabelizedVariable xtrans
            = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
                set.translator(0));
        TS_ASSERT_EQUALS(xtrans.translate("toto").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(xtrans.translate("titi").discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(xtrans.translate("???").discr_val,
                         std::numeric_limits< std::size_t >::max());

        gum::learning::DBTranslatorSet set2(set);
        TS_ASSERT_EQUALS(set2.translate(row1, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set2.translate(row1, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set2.translate(row1, 2).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set2.translate(row2, 0).discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(set2.translate(row2, 1).cont_val, 0.22f)
        TS_ASSERT_EQUALS(set2.translate(row2, 2).discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(set2.translate(row3, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set2.translate(row3, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set2.translate(row3, 2).discr_val, (std::size_t)1)
        gum::learning::DBTranslator4LabelizedVariable xtrans2
            = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
                set2.translator(0));
        TS_ASSERT_EQUALS(xtrans2.translate("toto").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(xtrans2.translate("titi").discr_val, (std::size_t)1)

        gum::learning::DBTranslatorSet set2bis(set2);
        gum::learning::DBTranslatorSet set3(std::move(set2));
        TS_ASSERT_EQUALS(set3.translate(row1, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set3.translate(row1, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set3.translate(row1, 2).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set3.translate(row2, 0).discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(set3.translate(row2, 1).cont_val, 0.22f)
        TS_ASSERT_EQUALS(set3.translate(row2, 2).discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(set3.translate(row3, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set3.translate(row3, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set3.translate(row3, 2).discr_val, (std::size_t)1)
        gum::learning::DBTranslator4LabelizedVariable xtrans3
            = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
                set3.translator(0));
        TS_ASSERT_EQUALS(xtrans3.translate("toto").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(xtrans3.translate("titi").discr_val, (std::size_t)1)

        gum::learning::DBTranslatorSet set4(set);
        TS_ASSERT_EQUALS(set4.translate(row1, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set4.translate(row1, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set4.translate(row1, 2).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set4.translate(row2, 0).discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(set4.translate(row2, 1).cont_val, 0.22f)
        TS_ASSERT_EQUALS(set4.translate(row2, 2).discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(set4.translate(row3, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set4.translate(row3, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set4.translate(row3, 2).discr_val, (std::size_t)1)
        gum::learning::DBTranslator4LabelizedVariable xtrans4
            = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
                set4.translator(0));
        TS_ASSERT_EQUALS(xtrans4.translate("toto").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(xtrans4.translate("titi").discr_val, (std::size_t)1)

        gum::learning::DBTranslatorSet set5(std::move(set2bis));
        TS_ASSERT_EQUALS(set5.translate(row1, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set5.translate(row1, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set5.translate(row1, 2).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set5.translate(row2, 0).discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(set5.translate(row2, 1).cont_val, 0.22f)
        TS_ASSERT_EQUALS(set5.translate(row2, 2).discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(set5.translate(row3, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set5.translate(row3, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set5.translate(row3, 2).discr_val, (std::size_t)1)
        gum::learning::DBTranslator4LabelizedVariable xtrans5
            = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
                set5.translator(0));
        TS_ASSERT_EQUALS(xtrans5.translate("toto").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(xtrans5.translate("titi").discr_val, (std::size_t)1)

        const auto ordersafe = set.reorderSafe(0);
        TS_ASSERT_EQUALS(ordersafe[0], std::size_t(1))
        TS_ASSERT_EQUALS(ordersafe[1], std::size_t(0))
      }
    }

    GUM_ACTIVE_TEST(_trans3) {
      gum::learning::DBTranslatorSet set;

      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable translator1(missing, 3);
      set.insertTranslator(translator1, 1);

      gum::learning::DBTranslator4ContinuousVariable translator0;
      set.insertTranslator(translator0, 0);

      gum::learning::DBTranslator4LabelizedVariable translator2;
      set.insertTranslator(translator2, 2);

      std::vector< std::string > row1{".33", "toto", "titi"};
      TS_ASSERT_EQUALS(set.translate(row1, 0).discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(set.translate(row1, 1).cont_val, 0.33f)
      TS_ASSERT_EQUALS(set.translate(row1, 2).discr_val, (std::size_t)0)

      std::vector< std::string > row2{".22", "titi", "toto"};
      TS_ASSERT_EQUALS(set.translate(row2, 0).discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(set.translate(row2, 1).cont_val, 0.22f)
      TS_ASSERT_EQUALS(set.translate(row2, 2).discr_val, (std::size_t)1)

      std::vector< std::string > row3{".33", "toto", "toto"};
      TS_ASSERT_EQUALS(set.translate(row3, 0).discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(set.translate(row3, 1).cont_val, 0.33f)
      TS_ASSERT_EQUALS(set.translate(row3, 2).discr_val, (std::size_t)1)


      gum::learning::DBTranslatorSet* set4 = set.clone();
      TS_ASSERT_EQUALS(set4->translate(row1, 0).discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(set4->translate(row1, 1).cont_val, 0.33f)
      TS_ASSERT_EQUALS(set4->translate(row1, 2).discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(set4->translate(row2, 0).discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(set4->translate(row2, 1).cont_val, 0.22f)
      TS_ASSERT_EQUALS(set4->translate(row2, 2).discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(set4->translate(row3, 0).discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(set4->translate(row3, 1).cont_val, 0.33f)
      TS_ASSERT_EQUALS(set4->translate(row3, 2).discr_val, (std::size_t)1)
      gum::learning::DBTranslator4LabelizedVariable xtrans4
          = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
              set4->translator(0));
      TS_ASSERT_EQUALS(xtrans4.translate("toto").discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(xtrans4.translate("titi").discr_val, (std::size_t)1)

      delete set4;
    }

    GUM_ACTIVE_TEST(_trans4) {
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
        TS_ASSERT_EQUALS(set.translate(row1, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set.translate(row1, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set.translate(row1, 2).discr_val, (std::size_t)0)

        std::vector< std::string > row2{".22", "titi", "toto"};
        TS_ASSERT_EQUALS(set.translate(row2, 0).discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(set.translate(row2, 1).cont_val, 0.22f)
        TS_ASSERT_EQUALS(set.translate(row2, 2).discr_val, (std::size_t)1)

        std::vector< std::string > row3{".33", "toto", "toto"};
        TS_ASSERT_EQUALS(set.translate(row3, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set.translate(row3, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set.translate(row3, 2).discr_val, (std::size_t)1)

        gum::learning::DBTranslatorSet* set4 = set.clone();
        TS_ASSERT_EQUALS(set4->translate(row1, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set4->translate(row1, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set4->translate(row1, 2).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set4->translate(row2, 0).discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(set4->translate(row2, 1).cont_val, 0.22f)
        TS_ASSERT_EQUALS(set4->translate(row2, 2).discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(set4->translate(row3, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set4->translate(row3, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set4->translate(row3, 2).discr_val, (std::size_t)1)
        gum::learning::DBTranslator4LabelizedVariable xtrans4
            = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
                set4->translator(0));
        TS_ASSERT_EQUALS(xtrans4.translate("toto").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(xtrans4.translate("titi").discr_val, (std::size_t)1)

        gum::learning::DBTranslator4LabelizedVariable xtrans4b
            = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
                set4->operator[](0));
        TS_ASSERT_EQUALS(xtrans4b.translate("toto").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(xtrans4b.translate("titi").discr_val, (std::size_t)1)

        gum::learning::DBTranslator4LabelizedVariable xtrans4t
            = dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable& >(
                set4->translatorSafe(0));
        TS_ASSERT_EQUALS(xtrans4t.translate("toto").discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(xtrans4t.translate("titi").discr_val, (std::size_t)1)

        delete set4;

        gum::learning::DBTranslatorSet set2;
        TS_ASSERT_EQUALS(set2.nbTranslators(), (std::size_t)0)
        set2 = set;
        TS_ASSERT_EQUALS(set2.translate(row3, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set2.translate(row3, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set2.translate(row3, 2).discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(set2.translate(row3, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set2.translate(row3, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set2.translate(row3, 2).discr_val, (std::size_t)1)

        gum::learning::DBTranslatorSet set3;
        TS_ASSERT_EQUALS(set3.nbTranslators(), (std::size_t)0)
        set3 = std::move(set);
        TS_ASSERT_EQUALS(set3.translate(row3, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set3.translate(row3, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set3.translate(row3, 2).discr_val, (std::size_t)1)
        TS_ASSERT_EQUALS(set3.translate(row3, 0).discr_val, (std::size_t)0)
        TS_ASSERT_EQUALS(set3.translate(row3, 1).cont_val, 0.33f)
        TS_ASSERT_EQUALS(set3.translate(row3, 2).discr_val, (std::size_t)1)
      }
    }

    GUM_ACTIVE_TEST(_trans5) {
      gum::learning::DBTranslatorSet set;
      std::size_t                    pos;

      std::vector< std::string > missing{"?", "N/A", "???"};
      gum::LabelizedVariable     var1("var1", "", 0);
      var1.addLabel("toto");
      var1.addLabel("titi");
      pos = set.insertTranslator(var1, 1, missing, false);
      TS_ASSERT_EQUALS(pos, std::size_t(0))

      gum::ContinuousVariable< double > var0("var0", "");
      pos = set.insertTranslator(var0, 0, missing);
      TS_ASSERT_EQUALS(pos, std::size_t(1))

      gum::LabelizedVariable var2("var2", "", 0);
      var2.addLabel("titi");
      var2.addLabel("toto");
      pos = set.insertTranslator(var2, 2, missing);
      TS_ASSERT_EQUALS(pos, std::size_t(2))

      std::vector< std::string > row1{".33", "toto", "titi"};
      TS_ASSERT_EQUALS(set.translate(row1, 0).discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(set.translate(row1, 1).cont_val, 0.33f)
      TS_ASSERT_EQUALS(set.translate(row1, 2).discr_val, (std::size_t)0)

      std::vector< std::string > row2{".22", "titi", "toto"};
      TS_ASSERT_EQUALS(set.translate(row2, 0).discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(set.translate(row2, 1).cont_val, 0.22f)
      TS_ASSERT_EQUALS(set.translate(row2, 2).discr_val, (std::size_t)1)

      std::vector< std::string > row3{".33", "toto", "toto"};
      TS_ASSERT_EQUALS(set.translate(row3, 0).discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(set.translate(row3, 1).cont_val, 0.33f)
      TS_ASSERT_EQUALS(set.translate(row3, 2).discr_val, (std::size_t)1)
    }

    GUM_ACTIVE_TEST(_trans6) {
      gum::learning::DBTranslatorSet set;

      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable translator1(missing, 3);
      std::size_t                                   pos;
      pos = set.insertTranslator(translator1, 1, false);
      TS_ASSERT_EQUALS(pos, std::size_t(0))

      gum::learning::DBTranslator4ContinuousVariable translator0;
      pos = set.insertTranslator(translator0, 0);
      TS_ASSERT_EQUALS(pos, std::size_t(1))

      gum::learning::DBTranslator4LabelizedVariable translator2;
      pos = set.insertTranslator(translator2, 2);
      TS_ASSERT_EQUALS(pos, std::size_t(2))

      pos = set.insertTranslator(translator1, 1, false);
      TS_ASSERT_EQUALS(pos, std::size_t(3))

      std::vector< std::string > row1{".33", "toto", "titi"};
      TS_ASSERT_EQUALS(set.translate(row1, 0).discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(set.translate(row1, 1).cont_val, 0.33f)
      TS_ASSERT_EQUALS(set.translate(row1, 2).discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(set.translate(row1, 3).discr_val, (std::size_t)0)

      std::vector< std::string > row2{".22", "titi", "toto"};
      TS_ASSERT_EQUALS(set.translate(row2, 0).discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(set.translate(row2, 1).cont_val, 0.22f)
      TS_ASSERT_EQUALS(set.translate(row2, 2).discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(set.translate(row2, 3).discr_val, (std::size_t)1)

      std::vector< std::string > row3{".33", "toto", "toto"};
      TS_ASSERT_EQUALS(set.translate(row3, 0).discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(set.translate(row3, 1).cont_val, 0.33f)
      TS_ASSERT_EQUALS(set.translate(row3, 2).discr_val, (std::size_t)1)
      TS_ASSERT_EQUALS(set.translate(row3, 3).discr_val, (std::size_t)0)

      std::vector< std::string > row4{"???", "???", "???"};
      TS_ASSERT_EQUALS(set.translate(row4, 0).discr_val, std::numeric_limits< std::size_t >::max())
      TS_ASSERT_THROWS(set.translate(row4, 1), const gum::TypeError&)
      TS_ASSERT_EQUALS(set.translate(row4, 3).discr_val, std::numeric_limits< std::size_t >::max())

      std::vector< std::string > row5{"4.22x", "???", "???"};
      TS_ASSERT_THROWS(set.translate(row5, 1), const gum::TypeError&)
      TS_ASSERT_EQUALS(set.translate(row5, 2).discr_val, (std::size_t)2)

      TS_ASSERT_THROWS(set.translateSafe(row5, 4), const gum::UndefinedElement&)

      TS_ASSERT_EQUALS(set.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}, 0),
                       "toto");
      TS_ASSERT_EQUALS(set.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}, 3),
                       "toto");
      TS_ASSERT_EQUALS(std::stof(set.translateBack(gum::learning::DBTranslatedValue{7.42f}, 1)),
                       7.42f);
      TS_ASSERT_EQUALS(set.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}, 2),
                       "???");

      TS_ASSERT_EQUALS(set.domainSize(0), (std::size_t)2)
      TS_ASSERT_EQUALS(set.domainSize(2), (std::size_t)3)
      TS_ASSERT_EQUALS(set.domainSize(1), std::numeric_limits< std::size_t >::max())
      TS_ASSERT_EQUALS(set.domainSize(3), (std::size_t)2)
      TS_ASSERT_EQUALS(set.domainSizeSafe(0), (std::size_t)2)
      TS_ASSERT_EQUALS(set.domainSizeSafe(2), (std::size_t)3)
      TS_ASSERT_EQUALS(set.domainSizeSafe(1), std::numeric_limits< std::size_t >::max())
      TS_ASSERT_EQUALS(set.domainSizeSafe(3), (std::size_t)2)
    }

    GUM_ACTIVE_TEST(_trans7) {
      gum::learning::DBTranslatorSet set;
      std::size_t                    pos;

      gum::LabelizedVariable var1("var1", "", 0);
      var1.addLabel("toto");
      var1.addLabel("titi");
      pos = set.insertTranslator(var1, 1, false);
      TS_ASSERT_EQUALS(pos, std::size_t(0))

      gum::ContinuousVariable< double > var0("var0", "");
      pos = set.insertTranslator(var0, 0);
      TS_ASSERT_EQUALS(pos, std::size_t(1))

      gum::LabelizedVariable var2("var2", "", 0);
      var2.addLabel("titi");
      var2.addLabel("toto");
      pos = set.insertTranslator(var2, 2);
      TS_ASSERT_EQUALS(pos, std::size_t(2))

      std::vector< std::string > row1{".33", "toto", "titi"};
      TS_ASSERT_EQUALS(set.translate(row1, 0).discr_val, (std::size_t)0)
      TS_ASSERT_EQUALS(set.translate(row1, 1).cont_val, 0.33f)
      TS_ASSERT_EQUALS(set.translate(row1, 2).discr_val, (std::size_t)0)
    }

    GUM_ACTIVE_TEST(_trans8) {
      gum::learning::DBTranslatorSet set;

      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable translator1(missing, 3);
      translator1.setVariableName("X1");

      std::size_t pos;
      pos = set.insertTranslator(translator1, 1);
      TS_ASSERT_EQUALS(pos, std::size_t(0))

      gum::learning::DBTranslator4ContinuousVariable translator0;
      translator0.setVariableName("X0");
      pos = set.insertTranslator(translator0, 0);
      TS_ASSERT_EQUALS(pos, std::size_t(1))

      gum::learning::DBTranslator4LabelizedVariable translator2;
      translator2.setVariableName("X2");
      pos = set.insertTranslator(translator2, 2);
      TS_ASSERT_EQUALS(pos, std::size_t(2))

      translator1.setVariableName("Y1");
      pos = set.insertTranslator(translator1, 1, false);
      TS_ASSERT_EQUALS(pos, std::size_t(3))

      translator0.setVariableName("Y0");
      pos = set.insertTranslator(translator0, 0, false);
      TS_ASSERT_EQUALS(pos, std::size_t(4))

      translator0.setVariableName("Z0");
      pos = set.insertTranslator(translator0, 0, false);
      TS_ASSERT_EQUALS(pos, std::size_t(5))

      translator1.setVariableName("Z1");
      pos = set.insertTranslator(translator1, 1, false);
      TS_ASSERT_EQUALS(pos, std::size_t(6))

      const auto& vect_trans = set.translators();
      TS_ASSERT_EQUALS(vect_trans.size(), std::size_t(7))

      TS_ASSERT_EQUALS(vect_trans[0]->variable()->name(), "X1")
      TS_ASSERT_EQUALS(vect_trans[1]->variable()->name(), "X0")
      TS_ASSERT_EQUALS(vect_trans[2]->variable()->name(), "X2")
      TS_ASSERT_EQUALS(vect_trans[3]->variable()->name(), "Y1")
      TS_ASSERT_EQUALS(vect_trans[4]->variable()->name(), "Y0")
      TS_ASSERT_EQUALS(vect_trans[5]->variable()->name(), "Z0")
      TS_ASSERT_EQUALS(vect_trans[6]->variable()->name(), "Z1")
      TS_ASSERT_EQUALS(set.highestInputColumn(), std::size_t(2))

      set.eraseTranslator(4, false);
      TS_ASSERT_EQUALS(vect_trans.size(), std::size_t(6))
      TS_ASSERT_EQUALS(vect_trans[0]->variable()->name(), "X1")
      TS_ASSERT_EQUALS(vect_trans[1]->variable()->name(), "X0")
      TS_ASSERT_EQUALS(vect_trans[2]->variable()->name(), "X2")
      TS_ASSERT_EQUALS(vect_trans[3]->variable()->name(), "Y1")
      TS_ASSERT_EQUALS(vect_trans[4]->variable()->name(), "Z0")
      TS_ASSERT_EQUALS(vect_trans[5]->variable()->name(), "Z1")
      TS_ASSERT_EQUALS(set.highestInputColumn(), std::size_t(2))

      set.eraseTranslator(0, true);
      TS_ASSERT_EQUALS(vect_trans.size(), std::size_t(4))
      TS_ASSERT_EQUALS(vect_trans[0]->variable()->name(), "X1")
      TS_ASSERT_EQUALS(vect_trans[1]->variable()->name(), "X2")
      TS_ASSERT_EQUALS(vect_trans[2]->variable()->name(), "Y1")
      TS_ASSERT_EQUALS(vect_trans[3]->variable()->name(), "Z1")
      TS_ASSERT_EQUALS(set.highestInputColumn(), std::size_t(2))

      set.eraseTranslator(2, true);
      TS_ASSERT_EQUALS(vect_trans.size(), std::size_t(3))
      TS_ASSERT_EQUALS(vect_trans[0]->variable()->name(), "X1")
      TS_ASSERT_EQUALS(vect_trans[1]->variable()->name(), "Y1")
      TS_ASSERT_EQUALS(vect_trans[2]->variable()->name(), "Z1")
      TS_ASSERT_EQUALS(set.highestInputColumn(), std::size_t(1))

      set.eraseTranslator(1, true);
      TS_ASSERT_EQUALS(vect_trans.size(), std::size_t(0))
      TS_ASSERT_EQUALS(set.highestInputColumn(), std::size_t(0))
    }
  };

} /* namespace gum_tests */
