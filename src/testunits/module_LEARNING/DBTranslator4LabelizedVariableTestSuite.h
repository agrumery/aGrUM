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

#include <agrum/core/thread.h>
#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>

namespace gum_tests {

  class DBTranslator4LabelizedVariableTestSuite : public CxxTest::TestSuite {
    public:
    void test_trans1() {
      gum::learning::DBTranslator4LabelizedVariable<> translator;
      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("toto"));
      TS_ASSERT(translator.translate("toto").discr_val == 0);
      TS_ASSERT(translator.translate("toto").discr_val == 0);

      TS_ASSERT(translator.missingValue().discr_val
                == std::numeric_limits< std::size_t >::max());

      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("titi"));
      TS_ASSERT(translator.translate("toto").discr_val == 0);
      TS_ASSERT(translator.translate("titi").discr_val == 1);
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
         == "toto");
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
         == "titi");

      std::vector< std::string >                      missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable<> translator2(missing);
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("toto"));
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("titi"));
      TS_ASSERT(translator2.translate("toto").discr_val == 0);
      TS_ASSERT(translator2.translate("titi").discr_val == 1);
      TS_ASSERT(translator2.translate("N/A").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator2.translate("?").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator2.translate("???").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator2.translate("??").discr_val == 2);
      TS_ASSERT(translator2.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{0}})
                == "toto");
      TS_ASSERT(translator2.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{1}})
                == "titi");
      TS_ASSERT(translator2.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{2}})
                == "??");

      gum::learning::DBTranslator4LabelizedVariable<> translator3(missing, 3);
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("toto"));
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("titi"));
      TS_ASSERT(translator3.translate("toto").discr_val == 0);
      TS_ASSERT(translator3.translate("titi").discr_val == 1);
      TS_ASSERT(translator3.translate("N/A").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator3.translate("?").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator3.translate("???").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator3.translate("??").discr_val == 2);
      TS_ASSERT_THROWS(translator3.translate("a"), gum::SizeError);

      TS_ASSERT(translator3.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{0}})
                == "toto");
      TS_ASSERT(translator3.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{1}})
                == "titi");
      TS_ASSERT(translator3.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{2}})
                == "??");
      TS_ASSERT_THROWS(translator3.translateBack(
                          gum::learning::DBTranslatedValue{std::size_t{3}}),
                       gum::UnknownLabelInDatabase);
      const std::string back =
         translator3.translateBack(gum::learning::DBTranslatedValue{
            std::numeric_limits< std::size_t >::max()});
      TS_ASSERT(translator3.missingSymbols().exists(back));
    }


    void test_trans2() {
      gum::learning::DBTranslator4LabelizedVariable< MyAlloc > translator;
      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("toto"));
      TS_ASSERT(translator.translate("toto").discr_val == 0);
      TS_ASSERT(translator.translate("toto").discr_val == 0);

      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("titi"));
      TS_ASSERT(translator.translate("toto").discr_val == 0);
      TS_ASSERT(translator.translate("titi").discr_val == 1);
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
         == "toto");
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
         == "titi");

      std::vector< std::string >                      missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable<> translator2(missing);
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("toto"));
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("titi"));
      TS_ASSERT(translator2.translate("toto").discr_val == 0);
      TS_ASSERT(translator2.translate("titi").discr_val == 1);
      TS_ASSERT(translator2.translate("N/A").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator2.translate("?").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator2.translate("???").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator2.translate("??").discr_val == 2);

      TS_ASSERT(translator2.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{0}})
                == "toto");
      TS_ASSERT(translator2.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{1}})
                == "titi");
      TS_ASSERT(translator2.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{2}})
                == "??");

      gum::learning::DBTranslator4LabelizedVariable< MyAlloc > translator3(missing,
                                                                           3);
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("toto"));
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("titi"));
      TS_ASSERT(translator3.translate("toto").discr_val == 0);
      TS_ASSERT(translator3.translate("titi").discr_val == 1);
      TS_ASSERT(translator3.translate("N/A").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator3.translate("?").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator3.translate("???").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator3.translate("??").discr_val == 2);
      TS_ASSERT_THROWS(translator3.translate("a"), gum::SizeError);

      TS_ASSERT(translator3.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{0}})
                == "toto");
      TS_ASSERT(translator3.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{1}})
                == "titi");
      TS_ASSERT(translator3.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{2}})
                == "??");
      TS_ASSERT_THROWS(translator3.translateBack(
                          gum::learning::DBTranslatedValue{std::size_t{3}}),
                       gum::UnknownLabelInDatabase);
      TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{
                   std::numeric_limits< std::size_t >::max()})
                == "?");
    }


    void test_trans3() {
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("toto");
      var.addLabel("titi");
      var.addLabel("tutu");

      gum::learning::DBTranslator4LabelizedVariable< MyAlloc > translator(var);
      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("toto"));
      TS_ASSERT(translator.translate("toto").discr_val == 0);
      TS_ASSERT(translator.translate("toto").discr_val == 0);

      TS_GUM_ASSERT_THROWS_NOTHING(translator.translate("tutu"));
      TS_ASSERT(translator.translate("toto").discr_val == 0);
      TS_ASSERT(translator.translate("tutu").discr_val == 2);
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
         == "toto");
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
         == "titi");

      std::vector< std::string >                      missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable<> translator2(var, missing);
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("toto"));
      TS_GUM_ASSERT_THROWS_NOTHING(translator2.translate("titi"));
      TS_ASSERT(translator2.translate("toto").discr_val == 0);
      TS_ASSERT(translator2.translate("titi").discr_val == 1);
      TS_ASSERT(translator2.translate("N/A").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator2.translate("?").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator2.translate("???").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT_THROWS(translator2.translate("??"), gum::UnknownLabelInDatabase);
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
         == "toto");
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
         == "titi");
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
         == "tutu");

      gum::learning::DBTranslator4LabelizedVariable< MyAlloc > translator3(
         var, missing, true, 3);
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("toto"));
      TS_GUM_ASSERT_THROWS_NOTHING(translator3.translate("titi"));
      TS_ASSERT(translator3.translate("toto").discr_val == 0);
      TS_ASSERT(translator3.translate("titi").discr_val == 1);
      TS_ASSERT(translator3.translate("N/A").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator3.translate("?").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator3.translate("???").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT_THROWS(translator3.translate("??"), gum::SizeError);
      TS_ASSERT_THROWS(translator3.translate("a"), gum::SizeError);

      TS_ASSERT(translator3.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{0}})
                == "toto");
      TS_ASSERT(translator3.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{1}})
                == "titi");
      TS_ASSERT(translator3.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{2}})
                == "tutu");
      TS_ASSERT_THROWS(translator3.translateBack(
                          gum::learning::DBTranslatedValue{std::size_t{3}}),
                       gum::UnknownLabelInDatabase);
      TS_ASSERT(translator3.translateBack(gum::learning::DBTranslatedValue{
                   std::numeric_limits< std::size_t >::max()})
                == "?");

      TS_ASSERT(translator3.domainSize() == 3);

      TS_ASSERT_THROWS(gum::learning::DBTranslator4LabelizedVariable<> translator4(
                          var, missing, true, 2),
                       gum::SizeError);
    }


    void test_trans4() {
      {
        gum::LabelizedVariable var("X1", "", 0);
        var.addLabel("1");
        var.addLabel("4");
        var.addLabel("2");

        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > translator(var);
        TS_ASSERT(translator.translate("1").discr_val == 0);
        TS_ASSERT(translator.translate("4").discr_val == 1);
        TS_ASSERT(translator.translate("2").discr_val == 2);
        TS_ASSERT(translator.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "4");
        TS_ASSERT(translator.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "2");
        TS_ASSERT(translator.variable()->toString() == "X1<1,4,2>");

        gum::LabelizedVariable var2("X2", "", 0);
        var2.addLabel("1");
        var2.addLabel("2");
        var2.addLabel("4");

        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > translator2(var2);
        TS_ASSERT(translator2.translate("1").discr_val == 0);
        TS_ASSERT(translator2.translate("2").discr_val == 1);
        TS_ASSERT(translator2.translate("4").discr_val == 2);
        TS_ASSERT(translator2.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator2.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator2.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "4");
        TS_ASSERT(translator2.variable()->toString() == "X2<1,2,4>");

        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > translator3(
           translator);
        TS_ASSERT(translator3.translate("1").discr_val == 0);
        TS_ASSERT(translator3.translate("4").discr_val == 1);
        TS_ASSERT(translator3.translate("2").discr_val == 2);
        TS_ASSERT(translator3.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator3.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "4");
        TS_ASSERT(translator3.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "2");
        TS_ASSERT(translator3.variable()->toString() == "X1<1,4,2>");

        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > translator4(
           translator2);
        TS_ASSERT(translator4.translate("1").discr_val == 0);
        TS_ASSERT(translator4.translate("2").discr_val == 1);
        TS_ASSERT(translator4.translate("4").discr_val == 2);
        TS_ASSERT(translator4.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator4.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator4.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "4");
        TS_ASSERT(translator4.variable()->toString() == "X2<1,2,4>");

        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > translator5(
           std::move(translator3));
        TS_ASSERT(translator5.translate("1").discr_val == 0);
        TS_ASSERT(translator5.translate("4").discr_val == 1);
        TS_ASSERT(translator5.translate("2").discr_val == 2);
        TS_ASSERT(translator5.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator5.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "4");
        TS_ASSERT(translator5.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "2");
        TS_ASSERT(translator5.variable()->toString() == "X1<1,4,2>");

        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > translator6(
           std::move(translator4));
        TS_ASSERT(translator6.translate("1").discr_val == 0);
        TS_ASSERT(translator6.translate("2").discr_val == 1);
        TS_ASSERT(translator6.translate("4").discr_val == 2);
        TS_ASSERT(translator6.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator6.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator6.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "4");
        TS_ASSERT(translator6.variable()->toString() == "X2<1,2,4>");

        gum::learning::DBTranslator4LabelizedVariable< MyAlloc >* translator7 =
           translator6.clone();
        TS_ASSERT(translator7->translate("1").discr_val == 0);
        TS_ASSERT(translator7->translate("2").discr_val == 1);
        TS_ASSERT(translator7->translate("4").discr_val == 2);
        TS_ASSERT(translator7->translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator7->translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator7->translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "4");
        TS_ASSERT(translator7->variable()->toString() == "X2<1,2,4>");

        MyAlloc< gum::learning::DBTranslator4LabelizedVariable< MyAlloc > >
           allocator(translator7->getAllocator());
        allocator.destroy(translator7);
        allocator.deallocate(translator7, 1);

        translator5 = translator6;
        TS_ASSERT(translator5.translate("1").discr_val == 0);
        TS_ASSERT(translator5.translate("2").discr_val == 1);
        TS_ASSERT(translator5.translate("4").discr_val == 2);
        TS_ASSERT(translator5.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator5.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator5.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "4");
        TS_ASSERT(translator5.variable()->toString() == "X2<1,2,4>");

        translator3 = translator5;
        TS_ASSERT(translator3.translate("1").discr_val == 0);
        TS_ASSERT(translator3.translate("2").discr_val == 1);
        TS_ASSERT(translator3.translate("4").discr_val == 2);
        TS_ASSERT(translator3.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator3.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator3.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "4");
        TS_ASSERT(translator3.variable()->toString() == "X2<1,2,4>");

        translator5 = std::move(translator3);
        TS_ASSERT(translator5.translate("1").discr_val == 0);
        TS_ASSERT(translator5.translate("2").discr_val == 1);
        TS_ASSERT(translator5.translate("4").discr_val == 2);
        TS_ASSERT(translator5.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{0}})
                  == "1");
        TS_ASSERT(translator5.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{1}})
                  == "2");
        TS_ASSERT(translator5.translateBack(
                     gum::learning::DBTranslatedValue{std::size_t{2}})
                  == "4");
        TS_ASSERT(translator5.variable()->toString() == "X2<1,2,4>");
      }

      TS_ASSERT(MyAllocCount::hasMeroryLeak() == false);
    }


    void xtest_trans5() {
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("1");
      var.addLabel("4");
      var.addLabel("2");

      LearningAlloc< double > alloc(10000);

      gum::learning::DBTranslator4LabelizedVariable< LearningAlloc > translator(
         var, false, 1000, alloc);

      std::cout << alloc.toString() << std::endl;
      std::cout << translator.getAllocator().toString() << std::endl;

      TS_ASSERT(translator.translate("1").discr_val == 0);
      TS_ASSERT(translator.translate("4").discr_val == 1);
      TS_ASSERT(translator.translate("2").discr_val == 2);
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
         == "1");
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
         == "4");
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
         == "2");
      TS_ASSERT(translator.variable()->toString() == "X1<1,4,2>");

      const auto& allocator = translator.getAllocator();
      std::cout << (allocator == alloc) << std::endl;
      std::cout << alloc.toString() << std::endl;

      gum::LabelizedVariable var2("X2", "", 0);
      var2.addLabel("1");
      var2.addLabel("2");
      var2.addLabel("4");

      gum::learning::DBTranslator4LabelizedVariable< LearningAlloc > translator2(
         var2);
      TS_ASSERT(translator2.translate("1").discr_val == 0);
      TS_ASSERT(translator2.translate("2").discr_val == 1);
      TS_ASSERT(translator2.translate("4").discr_val == 2);
      TS_ASSERT(translator2.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{0}})
                == "1");
      TS_ASSERT(translator2.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{1}})
                == "2");
      TS_ASSERT(translator2.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{2}})
                == "4");
      TS_ASSERT(translator2.variable()->toString() == "X2<1,2,4>");

      gum::learning::DBTranslator4LabelizedVariable< LearningAlloc > translator3(
         translator);
      TS_ASSERT(translator3.translate("1").discr_val == 0);
      TS_ASSERT(translator3.translate("4").discr_val == 1);
      TS_ASSERT(translator3.translate("2").discr_val == 2);
      TS_ASSERT(translator3.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{0}})
                == "1");
      TS_ASSERT(translator3.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{1}})
                == "4");
      TS_ASSERT(translator3.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{2}})
                == "2");
      TS_ASSERT(translator3.variable()->toString() == "X1<1,4,2>");

      gum::learning::DBTranslator4LabelizedVariable< LearningAlloc > translator4(
         translator2);
      TS_ASSERT(translator4.translate("1").discr_val == 0);
      TS_ASSERT(translator4.translate("2").discr_val == 1);
      TS_ASSERT(translator4.translate("4").discr_val == 2);
      TS_ASSERT(translator4.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{0}})
                == "1");
      TS_ASSERT(translator4.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{1}})
                == "2");
      TS_ASSERT(translator4.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{2}})
                == "4");
      TS_ASSERT(translator4.variable()->toString() == "X2<1,2,4>");

      gum::learning::DBTranslator4LabelizedVariable< LearningAlloc > translator5(
         std::move(translator3));
      TS_ASSERT(translator5.translate("1").discr_val == 0);
      TS_ASSERT(translator5.translate("4").discr_val == 1);
      TS_ASSERT(translator5.translate("2").discr_val == 2);
      TS_ASSERT(translator5.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{0}})
                == "1");
      TS_ASSERT(translator5.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{1}})
                == "4");
      TS_ASSERT(translator5.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{2}})
                == "2");
      TS_ASSERT(translator5.variable()->toString() == "X1<1,4,2>");

      gum::learning::DBTranslator4LabelizedVariable< LearningAlloc > translator6(
         std::move(translator4));
      TS_ASSERT(translator6.translate("1").discr_val == 0);
      TS_ASSERT(translator6.translate("2").discr_val == 1);
      TS_ASSERT(translator6.translate("4").discr_val == 2);
      TS_ASSERT(translator6.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{0}})
                == "1");
      TS_ASSERT(translator6.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{1}})
                == "2");
      TS_ASSERT(translator6.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{2}})
                == "4");
      TS_ASSERT(translator6.variable()->toString() == "X2<1,2,4>");

      gum::learning::DBTranslator4LabelizedVariable< LearningAlloc >* translator7 =
         translator6.clone();
      TS_ASSERT(translator7->translate("1").discr_val == 0);
      TS_ASSERT(translator7->translate("2").discr_val == 1);
      TS_ASSERT(translator7->translate("4").discr_val == 2);
      TS_ASSERT(translator7->translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{0}})
                == "1");
      TS_ASSERT(translator7->translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{1}})
                == "2");
      TS_ASSERT(translator7->translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{2}})
                == "4");
      TS_ASSERT(translator7->variable()->toString() == "X2<1,2,4>");

      LearningAlloc<
         gum::learning::DBTranslator4LabelizedVariable< LearningAlloc > >
         allocator2(translator7->getAllocator());
      allocator2.destroy(translator7);
      allocator2.deallocate(translator7, 1);

      translator5 = translator6;
      TS_ASSERT(translator5.translate("1").discr_val == 0);
      TS_ASSERT(translator5.translate("2").discr_val == 1);
      TS_ASSERT(translator5.translate("4").discr_val == 2);
      TS_ASSERT(translator5.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{0}})
                == "1");
      TS_ASSERT(translator5.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{1}})
                == "2");
      TS_ASSERT(translator5.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{2}})
                == "4");
      TS_ASSERT(translator5.variable()->toString() == "X2<1,2,4>");

      translator3 = translator5;
      TS_ASSERT(translator3.translate("1").discr_val == 0);
      TS_ASSERT(translator3.translate("2").discr_val == 1);
      TS_ASSERT(translator3.translate("4").discr_val == 2);
      TS_ASSERT(translator3.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{0}})
                == "1");
      TS_ASSERT(translator3.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{1}})
                == "2");
      TS_ASSERT(translator3.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{2}})
                == "4");
      TS_ASSERT(translator3.variable()->toString() == "X2<1,2,4>");

      translator5 = std::move(translator3);
      TS_ASSERT(translator5.translate("1").discr_val == 0);
      TS_ASSERT(translator5.translate("2").discr_val == 1);
      TS_ASSERT(translator5.translate("4").discr_val == 2);
      TS_ASSERT(translator5.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{0}})
                == "1");
      TS_ASSERT(translator5.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{1}})
                == "2");
      TS_ASSERT(translator5.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{2}})
                == "4");
      TS_ASSERT(translator5.variable()->toString() == "X2<1,2,4>");
    }


    void test_trans6() {
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("toto");
      var.addLabel("titi");
      var.addLabel("???");

      std::vector< std::string > missing{"?", "N/A", "???"};

      gum::learning::DBTranslator4LabelizedVariable<> translator(var, missing);
      TS_ASSERT(translator.translate("toto").discr_val == 0);
      TS_ASSERT(translator.translate("toto").discr_val == 0);
      TS_ASSERT(translator.translate("N/A").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator.translate("?").discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(translator.translate("???").discr_val == 2);

      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
         == "toto");
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
         == "titi");
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
         == "???");
    }

    void test_trans7() {
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("7");
      var.addLabel("1");
      var.addLabel("3");

      std::vector< std::string > missing{"7", "N/A", "toto"};

      gum::learning::DBTranslator4LabelizedVariable<> translator(var, missing);
      TS_ASSERT(translator.needsReordering());
      TS_ASSERT(translator.domainSize() == 3);

      TS_ASSERT((translator << "7").discr_val == 0);
      TS_ASSERT((translator >> gum::learning::DBTranslatedValue{std::size_t{0}})
                == "7");
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
         == "7");
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
         == "1");
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
         == "3");

      std::string back = translator.translateBack(gum::learning::DBTranslatedValue{
         std::numeric_limits< std::size_t >::max()});

      gum::Set< std::string > missing_kept{"N/A", "toto"};
      TS_ASSERT(missing_kept.exists(back));
      TS_ASSERT(translator.missingSymbols() == missing_kept);

      auto new_order = translator.reorder();

      TS_ASSERT(new_order[0] == 2);
      TS_ASSERT(new_order[1] == 0);
      TS_ASSERT(new_order[2] == 1);

      TS_ASSERT(translator.translate("7").discr_val == 2);
      TS_ASSERT(translator.translate("1").discr_val == 0);
      TS_ASSERT(translator.translate("3").discr_val == 1);
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}})
         == "1");
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{1}})
         == "3");
      TS_ASSERT(
         translator.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}})
         == "7");


      gum::LabelizedVariable var2("X2", "", 0);
      var2.addLabel("1");
      var2.addLabel("3");
      var2.addLabel("7");

      std::vector< std::string > missing2{"7", "N/A", "toto"};


      gum::learning::DBTranslator4LabelizedVariable<> translator2(var2, missing2);
      TS_ASSERT(translator2.needsReordering() == false);
      TS_ASSERT(translator2.domainSize() == 3);

      TS_ASSERT((translator2 << "7").discr_val == 2);
      TS_ASSERT((translator2 >> gum::learning::DBTranslatedValue{std::size_t{2}})
                == "7");
      TS_ASSERT(translator2.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{0}})
                == "1");
      TS_ASSERT(translator2.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{1}})
                == "3");
      TS_ASSERT(translator2.translateBack(
                   gum::learning::DBTranslatedValue{std::size_t{2}})
                == "7");
      back = translator2.translateBack(gum::learning::DBTranslatedValue{
         std::numeric_limits< std::size_t >::max()});
      TS_ASSERT(translator2.missingSymbols().exists(back));

      gum::Set< std::string > missing_kept2{"N/A", "toto"};
      TS_ASSERT(translator2.missingSymbols() == missing_kept2);

      auto new_order2 = translator2.reorder();
      TS_ASSERT(new_order2.size() == 0);
    }
  };

} /* namespace gum_tests */
