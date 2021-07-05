/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>
#include <ressources/include/myalloc.h>
#include <ressources/include/learningAlloc.h>
#include <iostream>

#include <agrum/tools/core/thread.h>
#include <agrum/tools/database/DBTranslator4IntegerVariable.h>

namespace gum_tests {

  class DBTranslator4IntegerVariableTestSuite: public CxxTest::TestSuite {
    public:
    void test_trans1() {
      gum::IntegerVariable var("X1", "");
      var.addValue(1);
      var.addValue(3);
      var.addValue(10);
      var.addValue(12);

      gum::learning::DBTranslator4IntegerVariable< MyAlloc > translator(var);
      TS_ASSERT(translator.isLossless());
      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("1"));
      TS_ASSERT(translator.translate("1").discr_val == 0);
      TS_ASSERT(translator.translate("3").discr_val == 1);
      TS_ASSERT(translator.translate("10").discr_val == 2);
      TS_ASSERT_THROWS(translator.translate("0"), gum::UnknownLabelInDatabase);
      TS_ASSERT_THROWS(translator.translate("11"), gum::UnknownLabelInDatabase);
      TS_ASSERT_THROWS(translator.translate("aaa"), gum::TypeError);

      TS_ASSERT(translator.missingValue().discr_val == std::numeric_limits< std::size_t >::max());

      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("12"));
      TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                == "1");
      TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                == "3");

      const auto& tr_var = *(translator.variable());
      int         good   = 1;
      try {
        const gum::IntegerVariable& xvar_discr =
           dynamic_cast< const gum::IntegerVariable& >(tr_var);
        TS_ASSERT(xvar_discr.domainSize() == 4);
        TS_ASSERT(xvar_discr.label(0) == "1");
        TS_ASSERT(xvar_discr.label(1) == "3");
        TS_ASSERT(xvar_discr.label(2) == "10");
        TS_ASSERT(xvar_discr.label(3) == "12");
      } catch (std::bad_cast&) { good = 0; }
      TS_ASSERT(good == 1);

      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4IntegerVariable<> translator2(var, missing);
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("1"));
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("12"));
      TS_ASSERT(translator2.translate("1").discr_val == 0);
      TS_ASSERT(translator2.translate("3").discr_val == 1);
      TS_ASSERT(translator2.translate("N/A").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator2.translate("?").discr_val == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator2.translate("???").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT_THROWS(translator2.translate("??"), gum::TypeError);
      TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                == "1");
      TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                == "10");

      gum::learning::DBTranslator4IntegerVariable< MyAlloc > translator3(var, missing, 4);
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("1"));
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("10"));
      TS_ASSERT(translator3.translate("1").discr_val == 0);
      TS_ASSERT(translator3.translate("3").discr_val == 1);
      TS_ASSERT(translator3.translate("N/A").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator3.translate("?").discr_val == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator3.translate("???").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT_THROWS(translator3.translate("??"), gum::TypeError);
      TS_ASSERT_THROWS(translator3.translate("a"), gum::TypeError);

      TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                == "1");
      TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                == "3");
      TS_ASSERT_THROWS(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{4}}),
                       gum::UnknownLabelInDatabase);
      TS_ASSERT(translator3.translateBack(
                   gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()})
                == "?");

      TS_ASSERT(translator3.domainSize() == 4);

      TS_ASSERT_THROWS(
         gum::learning::DBTranslator4IntegerVariable<> translator4(var, missing, 1),
         gum::SizeError);

      TS_ASSERT(translator3.variable()->toString() == "X1<1,3,10,12>");
      TS_ASSERT(translator3.domainSize() == 4);
      TS_ASSERT(!translator3.hasEditableDictionary());
      translator3.setEditableDictionaryMode(true);
      TS_ASSERT(!translator3.hasEditableDictionary());
      TS_ASSERT(!translator3.needsReordering());
      TS_ASSERT(translator3.reorder().empty());
    }


    void test_trans2() {
      {
        gum::IntegerVariable var("X1", "");
        var.addValue(1);
        var.addValue(3);
        var.addValue(10);
        var.addValue(12);

        gum::learning::DBTranslator4IntegerVariable<> translator(var);
        TS_ASSERT(translator.translate("1").discr_val == 0);
        TS_ASSERT(translator.translate("12").discr_val == 3);
        TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "3");
        TS_ASSERT(translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "10");
        TS_ASSERT(translator.variable()->toString() == "X1<1,3,10,12>");

        TS_ASSERT(
           translator.translate(translator.translateBack(translator.translate("1"))).discr_val
           == 0);

        gum::IntegerVariable var2("X2", "");
        var2.addValue(1);
        var2.addValue(2);
        var2.addValue(3);
        var2.addValue(4);

        gum::learning::DBTranslator4IntegerVariable< MyAlloc > translator2(var2);
        TS_ASSERT(translator2.translate("1").discr_val == 0);
        TS_ASSERT(translator2.translate("2").discr_val == 1);
        TS_ASSERT(translator2.translate("4").discr_val == 3);
        TS_ASSERT(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator2.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "3");
        TS_ASSERT(translator2.variable()->toString() == "X2<1,2,3,4>");

        gum::learning::DBTranslator4IntegerVariable<> translator3(translator);
        TS_ASSERT(translator3.translate("1").discr_val == 0);
        TS_ASSERT(translator3.translate("10").discr_val == 2);
        TS_ASSERT(translator3.translate("12").discr_val == 3);
        TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "3");
        TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "10");
        TS_ASSERT(translator3.variable()->toString() == "X1<1,3,10,12>");

        gum::learning::DBTranslator4IntegerVariable< MyAlloc > translator4(translator2);
        TS_ASSERT(translator4.translate("1").discr_val == 0);
        TS_ASSERT(translator4.translate("2").discr_val == 1);
        TS_ASSERT(translator4.translate("4").discr_val == 3);
        TS_ASSERT(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator4.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "3");
        TS_ASSERT(translator4.variable()->toString() == "X2<1,2,3,4>");

        gum::learning::DBTranslator4IntegerVariable<> translator5(std::move(translator3));
        TS_ASSERT(translator5.translate("1").discr_val == 0);
        TS_ASSERT(translator5.translate("10").discr_val == 2);
        TS_ASSERT(translator5.translate("12").discr_val == 3);
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "3");
        TS_ASSERT(translator5.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "10");
        TS_ASSERT(translator5.variable()->toString() == "X1<1,3,10,12>");

        gum::learning::DBTranslator4IntegerVariable< MyAlloc > translator6(
           std::move(translator4));
        TS_ASSERT(translator6.translate("1").discr_val == 0);
        TS_ASSERT(translator6.translate("2").discr_val == 1);
        TS_ASSERT(translator6.translate("4").discr_val == 3);
        TS_ASSERT(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator6.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "3");
        TS_ASSERT(translator6.variable()->toString() == "X2<1,2,3,4>");

        gum::learning::DBTranslator4IntegerVariable< MyAlloc >* translator7
           = translator6.clone();
        TS_ASSERT(translator7->translate("1").discr_val == 0);
        TS_ASSERT(translator7->translate("2").discr_val == 1);
        TS_ASSERT(translator7->translate("4").discr_val == 3);
        TS_ASSERT(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator7->translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "3");
        TS_ASSERT(translator7->variable()->toString() == "X2<1,2,3,4>");

        MyAlloc< gum::learning::DBTranslator4IntegerVariable< MyAlloc > > allocator(
           translator7->getAllocator());
        allocator.destroy(translator7);
        allocator.deallocate(translator7, 1);

        gum::learning::DBTranslator4IntegerVariable< MyAlloc > translator8(var);
        translator8 = translator6;
        TS_ASSERT(translator8.translate("1").discr_val == 0);
        TS_ASSERT(translator8.translate("2").discr_val == 1);
        TS_ASSERT(translator8.translate("4").discr_val == 3);
        TS_ASSERT(translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "3");
        TS_ASSERT(translator8.variable()->toString() == "X2<1,2,3,4>");

        gum::learning::DBTranslator4IntegerVariable< MyAlloc > translator8bis(var);
        gum::learning::DBTranslator4IntegerVariable< MyAlloc > translator9(var2);
        translator9 = translator8bis;
        TS_ASSERT(translator9.translate("1").discr_val == 0);
        TS_ASSERT(translator9.translate("3").discr_val == 1);
        TS_ASSERT(translator9.translate("10").discr_val == 2);
        TS_ASSERT(translator9.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator9.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "3");
        TS_ASSERT(translator9.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "10");
        TS_ASSERT(translator9.variable()->toString() == "X1<1,3,10,12>");

        translator8 = std::move(translator9);
        TS_ASSERT(translator8.translate("1").discr_val == 0);
        TS_ASSERT(translator8.translate("3").discr_val == 1);
        TS_ASSERT(translator8.translate("10").discr_val == 2);
        TS_ASSERT(translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "3");
        TS_ASSERT(translator8.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "10");
        TS_ASSERT(translator8.variable()->toString() == "X1<1,3,10,12>");
      }

      TS_ASSERT(MyAllocCount::hasMeroryLeak() == false);
    }


  };

} /* namespace gum_tests */
