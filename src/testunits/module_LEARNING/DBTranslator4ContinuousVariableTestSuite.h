/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <ressources/myalloc.h>
#include <ressources/learningAlloc.h>
#include <iostream>
#include <string>

#include <agrum/core/thread.h>
#include <agrum/learning/database/DBTranslator4ContinuousVariable.h>

namespace gum_tests {

  class DBTranslator4ContinuousVariableTestSuite : public CxxTest::TestSuite {
    public:
    void test_trans1() {
      gum::learning::DBTranslator4ContinuousVariable<> translator;
      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("3"));
      TS_ASSERT(translator.translate("3").cont_val == 3);
      TS_ASSERT(
        std::stof(translator.translateBack(gum::learning::DBTranslatedValue{3.0f}))
        == 3);
      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("5"));
      TS_ASSERT(translator.translate("4.22").cont_val == 4.22f);
      TS_ASSERT(translator.translate("-5.34").cont_val == -5.34f);
      TS_ASSERT(translator.translate("-5.34e3").cont_val == -5.34e3f);
      TS_ASSERT(translator.translate("-5.34e-34").cont_val == -5.34e-34f);
      TS_ASSERT(std::stof(translator.translateBack(
                  gum::learning::DBTranslatedValue{-5.34f}))
                == -5.34f);
      TS_ASSERT(std::stof(translator.translateBack(
                  gum::learning::DBTranslatedValue{-5.34e3f}))
                == -5.34e3f);
      TS_ASSERT(std::stof(translator.translateBack(
                  gum::learning::DBTranslatedValue{-5.34e-34f}))
                == -5.34e-34f);
      TS_ASSERT_THROWS(translator.translate("4.22x"), gum::TypeError);
      TS_ASSERT_THROWS(translator.translate("0x422"), gum::TypeError);

      std::vector< std::string > missing{"2", "N/A", "20", "4", "xxx"};
      gum::learning::DBTranslator4ContinuousVariable<> translator2(missing);
      TS_ASSERT(!translator2.missingSymbols().exists("2"));
      TS_ASSERT(!translator2.missingSymbols().exists("20"));
      TS_ASSERT(!translator2.missingSymbols().exists("4"));
      TS_ASSERT(translator.translate("4.22").cont_val == 4.22f);
      TS_ASSERT_THROWS(translator2.translate("yyy"), gum::TypeError);
      TS_ASSERT(translator2.translate("N/A").cont_val
                == std::numeric_limits< float >::max());
      TS_ASSERT(translator2.translate("xxx").cont_val
                == std::numeric_limits< float >::max());

      gum::ContinuousVariable< double >                var("X2", "", -2, 10);
      gum::learning::DBTranslator4ContinuousVariable<> translator3(var, missing);
      TS_ASSERT(!translator3.missingSymbols().exists("2"));
      TS_ASSERT(!translator3.missingSymbols().exists("4"));
      TS_ASSERT(translator3.missingSymbols().exists("20"));
      TS_ASSERT(translator3.missingSymbols().exists("N/A"));
      TS_ASSERT(translator3.missingSymbols().exists("xxx"));

      TS_ASSERT(translator3.translate("4.22").cont_val == 4.22f);
      TS_ASSERT(translator3.translate("-2").cont_val == -2.0f);
      TS_ASSERT(translator3.translate("10").cont_val == 10.0f);
      TS_ASSERT_THROWS(translator3.translate("11"), gum::UnknownLabelInDatabase);
      TS_ASSERT(translator3.translate("N/A").cont_val
                == std::numeric_limits< float >::max());
      TS_ASSERT(translator3.translate("xxx").cont_val
                == std::numeric_limits< float >::max());

      std::vector< std::string > missing4{"2", "N/A", "20", "4", "xxx", "-10"};
      gum::learning::DBTranslator4ContinuousVariable<> translator4(
        var, missing4, true);
      TS_ASSERT(!translator4.missingSymbols().exists("2"));
      TS_ASSERT(!translator4.missingSymbols().exists("4"));
      TS_ASSERT(translator4.missingSymbols().exists("20"));
      TS_ASSERT(translator4.missingSymbols().exists("N/A"));
      TS_ASSERT(translator4.missingSymbols().exists("xxx"));
      TS_ASSERT(translator4.translate("4.22").cont_val == 4.22f);
      TS_ASSERT(translator4.translate("-2").cont_val == -2.0f);
      TS_ASSERT(translator4.translate("10").cont_val == 10.0f);
      TS_ASSERT(translator4.translate("18").cont_val == 18.0f);
      TS_ASSERT(translator4.translate("20").cont_val
                == std::numeric_limits< float >::max());
      TS_ASSERT(translator4.translate("19.5").cont_val == 19.5f);
      TS_ASSERT_THROWS(translator4.translate("21"), gum::OperationNotAllowed);
      TS_ASSERT(translator4.translate("-12").cont_val == -12.0f);
      TS_ASSERT(translator4.translate("-10").cont_val == -10.0f);
      TS_ASSERT(std::stof(translator4.translateBack(
                  gum::learning::DBTranslatedValue{5.3f}))
                == 5.3f);
      TS_ASSERT_THROWS(
        translator4.translateBack(gum::learning::DBTranslatedValue{50.0f}),
        gum::UnknownLabelInDatabase);
      TS_ASSERT_THROWS(
        translator4.translateBack(gum::learning::DBTranslatedValue{-50.0f}),
        gum::UnknownLabelInDatabase);
      TS_ASSERT(translator4.missingSymbols().exists(translator4.translateBack(
        gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));


      gum::learning::DBTranslator4ContinuousVariable<> translator5(
        var, missing4, true);
      TS_ASSERT(!translator5.missingSymbols().exists("2"));
      TS_ASSERT(!translator5.missingSymbols().exists("4"));
      TS_ASSERT(translator5.translate("-10").cont_val
                == std::numeric_limits< float >::max());
      TS_ASSERT_THROWS(translator5.translate("-11"), gum::OperationNotAllowed);
      TS_ASSERT(translator5.translate("220").cont_val == 220.0f);
      TS_ASSERT(translator5.translate("20").cont_val == 20.0f);

      TS_ASSERT(translator5.needsReordering() == false);
      TS_ASSERT(translator5.reorder().empty());
    }


    void test_trans2() {
      {
        gum::ContinuousVariable< double > var("X2", "", -2, 10);
        std::vector< std::string > missing{"2", "N/A", "20", "4", "xxx", "-10"};

        gum::learning::DBTranslator4ContinuousVariable< MyAlloc > translator(
          var, missing, true);
        TS_ASSERT(!translator.missingSymbols().exists("2"));
        TS_ASSERT(!translator.missingSymbols().exists("4"));
        TS_ASSERT(translator.translate("-10").cont_val
                  == std::numeric_limits< float >::max());

        TS_ASSERT(std::stof(translator.translateBack(
                    gum::learning::DBTranslatedValue{5.3f}))
                  == 5.3f);
        TS_ASSERT_THROWS(
          translator.translateBack(gum::learning::DBTranslatedValue{50.0f}),
          gum::UnknownLabelInDatabase);
        TS_ASSERT_THROWS(
          translator.translateBack(gum::learning::DBTranslatedValue{-50.0f}),
          gum::UnknownLabelInDatabase);
        TS_ASSERT(translator.missingSymbols().exists(translator.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));

        gum::learning::DBTranslator4ContinuousVariable< MyAlloc > translator2(
          translator);
        TS_ASSERT(!translator2.missingSymbols().exists("2"));
        TS_ASSERT(!translator2.missingSymbols().exists("4"));
        TS_ASSERT(translator2.translate("-10").cont_val
                  == std::numeric_limits< float >::max());

        TS_ASSERT(std::stof(translator2.translateBack(
                    gum::learning::DBTranslatedValue{5.3f}))
                  == 5.3f);
        TS_ASSERT_THROWS(
          translator2.translateBack(gum::learning::DBTranslatedValue{50.0f}),
          gum::UnknownLabelInDatabase);
        TS_ASSERT_THROWS(
          translator2.translateBack(gum::learning::DBTranslatedValue{-50.0f}),
          gum::UnknownLabelInDatabase);
        TS_ASSERT(translator2.missingSymbols().exists(translator2.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));

        gum::learning::DBTranslator4ContinuousVariable< MyAlloc > translator3(
          translator2);
        TS_ASSERT(!translator3.missingSymbols().exists("2"));
        TS_ASSERT(!translator3.missingSymbols().exists("4"));
        TS_ASSERT(translator3.translate("-10").cont_val
                  == std::numeric_limits< float >::max());

        TS_ASSERT(std::stof(translator3.translateBack(
                    gum::learning::DBTranslatedValue{5.3f}))
                  == 5.3f);
        TS_ASSERT_THROWS(
          translator3.translateBack(gum::learning::DBTranslatedValue{50.0f}),
          gum::UnknownLabelInDatabase);
        TS_ASSERT_THROWS(
          translator3.translateBack(gum::learning::DBTranslatedValue{-50.0f}),
          gum::UnknownLabelInDatabase);
        TS_ASSERT(translator3.missingSymbols().exists(translator3.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));

        gum::learning::DBTranslator4ContinuousVariable< MyAlloc > translator4(
          std::move(translator3));
        TS_ASSERT(!translator4.missingSymbols().exists("2"));
        TS_ASSERT(!translator4.missingSymbols().exists("4"));
        TS_ASSERT(translator4.translate("-10").cont_val
                  == std::numeric_limits< float >::max());

        TS_ASSERT(std::stof(translator4.translateBack(
                    gum::learning::DBTranslatedValue{5.3f}))
                  == 5.3f);
        TS_ASSERT_THROWS(
          translator4.translateBack(gum::learning::DBTranslatedValue{50.0f}),
          gum::UnknownLabelInDatabase);
        TS_ASSERT_THROWS(
          translator4.translateBack(gum::learning::DBTranslatedValue{-50.0f}),
          gum::UnknownLabelInDatabase);
        TS_ASSERT(translator4.missingSymbols().exists(translator4.translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));

        gum::learning::DBTranslator4ContinuousVariable< MyAlloc >* translator5 =
          translator4.clone();

        TS_ASSERT(!translator5->missingSymbols().exists("2"));
        TS_ASSERT(!translator5->missingSymbols().exists("4"));
        TS_ASSERT(translator5->translate("-10").cont_val
                  == std::numeric_limits< float >::max());

        TS_ASSERT(std::stof(translator5->translateBack(
                    gum::learning::DBTranslatedValue{5.3f}))
                  == 5.3f);
        TS_ASSERT_THROWS(
          translator5->translateBack(gum::learning::DBTranslatedValue{50.0f}),
          gum::UnknownLabelInDatabase);
        TS_ASSERT_THROWS(
          translator5->translateBack(gum::learning::DBTranslatedValue{-50.0f}),
          gum::UnknownLabelInDatabase);
        TS_ASSERT(translator5->missingSymbols().exists(translator5->translateBack(
          gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()})));

        MyAlloc< gum::learning::DBTranslator4ContinuousVariable< MyAlloc > >
          allocator(translator5->getAllocator());
        allocator.destroy(translator5);
        allocator.deallocate(translator5, 1);


        TS_ASSERT(translator4.translate("12").cont_val == 12.0f);
        TS_ASSERT(translator4.variable()->lowerBound() == -2.0);
        TS_ASSERT(translator4.variable()->upperBound() == 12.0);
        translator4 = translator2;
        TS_ASSERT(translator4.variable()->lowerBound() == -2.0);
        TS_ASSERT(translator4.variable()->upperBound() == 10.0);

        TS_ASSERT(translator.translate("12").cont_val == 12.0f);
        TS_ASSERT(translator.variable()->lowerBound() == -2.0);
        TS_ASSERT(translator.variable()->upperBound() == 12.0);
        translator = translator2;
        TS_ASSERT(translator.variable()->lowerBound() == -2.0);
        TS_ASSERT(translator.variable()->upperBound() == 10.0);

        TS_ASSERT(translator.translate("12").cont_val == 12.0f);
        TS_ASSERT(translator.variable()->lowerBound() == -2.0);
        TS_ASSERT(translator.variable()->upperBound() == 12.0);
        translator = std::move(translator2);
        TS_ASSERT(translator.variable()->lowerBound() == -2.0);
        TS_ASSERT(translator.variable()->upperBound() == 10.0);
      }

      TS_ASSERT(MyAllocCount::hasMeroryLeak() == false);
    }
  };

} /* namespace gum_tests */