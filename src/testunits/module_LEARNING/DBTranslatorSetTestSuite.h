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
#include <iostream>
#include <vector>
#include <string>
#include <ressources/myalloc.h>
#include <ressources/learningAlloc.h>

#include <agrum/learning/database/DBTranslatorSet.h>
#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBTranslator4DiscretizedVariable.h>
#include <agrum/learning/database/DBTranslator4RangeVariable.h>
#include <agrum/learning/database/DBTranslator4ContinuousVariable.h>


namespace gum_tests {

  class DBTranslatorSetTestSuite : public CxxTest::TestSuite {
    public:
    void test_trans1() {
      gum::learning::DBTranslatorSet<> set;

      std::vector< std::string >                      missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable<> translator1(missing, 3);
      std::size_t                                     pos;
      pos = set.insertTranslator(translator1, 1);
      TS_ASSERT(pos == std::size_t(0));

      gum::learning::DBTranslator4ContinuousVariable<> translator0;
      pos = set.insertTranslator(translator0, 0);
      TS_ASSERT(pos == std::size_t(0));

      gum::learning::DBTranslator4LabelizedVariable<> translator2;
      pos = set.insertTranslator(translator2, 2);
      TS_ASSERT(pos == std::size_t(2));

      std::vector< std::string > row1{".33", "toto", "titi"};
      TS_ASSERT(set.translate(row1, 1).discr_val == 0);
      TS_ASSERT(set.translate(row1, 0).cont_val == 0.33f);
      TS_ASSERT(set.translate(row1, 2).discr_val == 0);

      std::vector< std::string > row2{".22", "titi", "toto"};
      TS_ASSERT(set.translate(row2, 1).discr_val == 1);
      TS_ASSERT(set.translate(row2, 0).cont_val == 0.22f);
      TS_ASSERT(set.translate(row2, 2).discr_val == 1);

      std::vector< std::string > row3{".33", "toto", "toto"};
      TS_ASSERT(set.translate(row3, 1).discr_val == 0);
      TS_ASSERT(set.translate(row3, 0).cont_val == 0.33f);
      TS_ASSERT(set.translate(row3, 2).discr_val == 1);

      std::vector< std::string > row4{"???", "???", "???"};
      TS_ASSERT(set.translate(row4, 1).discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT_THROWS(set.translate(row4, 0), gum::TypeError);

      std::vector< std::string > row5{"4.22x", "???", "???"};
      TS_ASSERT_THROWS(set.translate(row5, 0), gum::TypeError);
      TS_ASSERT(set.translate(row5, 2).discr_val == 2);

      TS_ASSERT_THROWS(set.translateSafe(row5, 3), gum::UndefinedElement);

      TS_ASSERT(
        set.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}, 1)
        == "toto");
      TS_ASSERT(
        std::stof(set.translateBack(gum::learning::DBTranslatedValue{7.42f}, 0))
        == 7.42f);
      TS_ASSERT(
        set.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}, 2)
        == "???");
      TS_ASSERT_THROWS(
        set.translateBackSafe(gum::learning::DBTranslatedValue{std::size_t{0}}, 3),
        gum::UndefinedElement);

      TS_ASSERT(set.domainSize(1) == 2);
      TS_ASSERT(set.domainSize(2) == 3);
      TS_ASSERT(set.domainSize(0) == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(set.domainSizeSafe(1) == 2);
      TS_ASSERT(set.domainSizeSafe(2) == 3);
      TS_ASSERT(set.domainSizeSafe(0)
                == std::numeric_limits< std::size_t >::max());

      gum::learning::DBTranslator4LabelizedVariable<> xtrans =
        dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable<>& >(
          set.translator(1));
      TS_ASSERT(xtrans.translate("toto").discr_val == 0);
      TS_ASSERT(xtrans.translate("titi").discr_val == 1);
      TS_ASSERT(xtrans.translate("???").discr_val
                == std::numeric_limits< std::size_t >::max());

      gum::learning::DBTranslator4LabelizedVariable<> xtransb =
        dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable<>& >(
          set.translatorSafe(1));
      TS_ASSERT(xtransb.translate("toto").discr_val == 0);
      TS_ASSERT(xtransb.translate("titi").discr_val == 1);
      TS_ASSERT(xtransb.translate("???").discr_val
                == std::numeric_limits< std::size_t >::max());

      TS_ASSERT(set.nbTranslators() == 3);
      TS_ASSERT(set.getAllocator() == std::allocator< std::size_t >());

      gum::LabelizedVariable var =
        dynamic_cast< const gum::LabelizedVariable& >(set.variable(1));
      TS_ASSERT(var.domainSize() == 2);
      TS_ASSERT(var.label(0) == "toto");
      TS_ASSERT(var.label(1) == "titi");

      gum::LabelizedVariable varb =
        dynamic_cast< const gum::LabelizedVariable& >(set.variableSafe(1));
      TS_ASSERT(varb.domainSize() == 2);
      TS_ASSERT(varb.label(0) == "toto");
      TS_ASSERT(varb.label(1) == "titi");

      set.clear();
      TS_ASSERT(set.nbTranslators() == 0);
      TS_ASSERT(set.getAllocator() == std::allocator< std::size_t >());
    }

    void test_trans2() {
      gum::learning::DBTranslatorSet< MyAlloc > set;

      std::vector< std::string > missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable< MyAlloc > translator1(missing,
                                                                           3);
      set.insertTranslator(translator1, 1);

      gum::learning::DBTranslator4ContinuousVariable< MyAlloc > translator0;
      set.insertTranslator(translator0, 0);

      gum::learning::DBTranslator4LabelizedVariable< MyAlloc > translator2;
      set.insertTranslator(translator2, 2);

      std::vector< std::string > row1{".33", "toto", "titi"};
      TS_ASSERT(set.translate(row1, 1).discr_val == 0);
      TS_ASSERT(set.translate(row1, 0).cont_val == 0.33f);
      TS_ASSERT(set.translate(row1, 2).discr_val == 0);

      std::vector< std::string > row2{".22", "titi", "toto"};
      TS_ASSERT(set.translate(row2, 1).discr_val == 1);
      TS_ASSERT(set.translate(row2, 0).cont_val == 0.22f);
      TS_ASSERT(set.translate(row2, 2).discr_val == 1);

      std::vector< std::string > row3{".33", "toto", "toto"};
      TS_ASSERT(set.translate(row3, 1).discr_val == 0);
      TS_ASSERT(set.translate(row3, 0).cont_val == 0.33f);
      TS_ASSERT(set.translate(row3, 2).discr_val == 1);

      gum::learning::DBTranslator4LabelizedVariable< MyAlloc > xtrans =
        dynamic_cast<
          const gum::learning::DBTranslator4LabelizedVariable< MyAlloc >& >(
          set.translator(1));
      TS_ASSERT(xtrans.translate("toto").discr_val == 0);
      TS_ASSERT(xtrans.translate("titi").discr_val == 1);
      TS_ASSERT(xtrans.translate("???").discr_val
                == std::numeric_limits< std::size_t >::max());

      gum::learning::DBTranslatorSet< MyAlloc > set2(set);
      TS_ASSERT(set2.translate(row1, 1).discr_val == 0);
      TS_ASSERT(set2.translate(row1, 0).cont_val == 0.33f);
      TS_ASSERT(set2.translate(row1, 2).discr_val == 0);
      TS_ASSERT(set2.translate(row2, 1).discr_val == 1);
      TS_ASSERT(set2.translate(row2, 0).cont_val == 0.22f);
      TS_ASSERT(set2.translate(row2, 2).discr_val == 1);
      TS_ASSERT(set2.translate(row3, 1).discr_val == 0);
      TS_ASSERT(set2.translate(row3, 0).cont_val == 0.33f);
      TS_ASSERT(set2.translate(row3, 2).discr_val == 1);
      gum::learning::DBTranslator4LabelizedVariable< MyAlloc > xtrans2 =
        dynamic_cast<
          const gum::learning::DBTranslator4LabelizedVariable< MyAlloc >& >(
          set2.translator(1));
      TS_ASSERT(xtrans2.translate("toto").discr_val == 0);
      TS_ASSERT(xtrans2.translate("titi").discr_val == 1);

      gum::learning::DBTranslatorSet< MyAlloc > set3(std::move(set2));
      TS_ASSERT(set3.translate(row1, 1).discr_val == 0);
      TS_ASSERT(set3.translate(row1, 0).cont_val == 0.33f);
      TS_ASSERT(set3.translate(row1, 2).discr_val == 0);
      TS_ASSERT(set3.translate(row2, 1).discr_val == 1);
      TS_ASSERT(set3.translate(row2, 0).cont_val == 0.22f);
      TS_ASSERT(set3.translate(row2, 2).discr_val == 1);
      TS_ASSERT(set3.translate(row3, 1).discr_val == 0);
      TS_ASSERT(set3.translate(row3, 0).cont_val == 0.33f);
      TS_ASSERT(set3.translate(row3, 2).discr_val == 1);
      gum::learning::DBTranslator4LabelizedVariable< MyAlloc > xtrans3 =
        dynamic_cast<
          const gum::learning::DBTranslator4LabelizedVariable< MyAlloc >& >(
          set3.translator(1));
      TS_ASSERT(xtrans3.translate("toto").discr_val == 0);
      TS_ASSERT(xtrans3.translate("titi").discr_val == 1);

      gum::learning::DBTranslatorSet< MyAlloc > set4(
        set, MyAlloc< gum::learning::DBTranslatedValue >());
      TS_ASSERT(set4.translate(row1, 1).discr_val == 0);
      TS_ASSERT(set4.translate(row1, 0).cont_val == 0.33f);
      TS_ASSERT(set4.translate(row1, 2).discr_val == 0);
      TS_ASSERT(set4.translate(row2, 1).discr_val == 1);
      TS_ASSERT(set4.translate(row2, 0).cont_val == 0.22f);
      TS_ASSERT(set4.translate(row2, 2).discr_val == 1);
      TS_ASSERT(set4.translate(row3, 1).discr_val == 0);
      TS_ASSERT(set4.translate(row3, 0).cont_val == 0.33f);
      TS_ASSERT(set4.translate(row3, 2).discr_val == 1);
      gum::learning::DBTranslator4LabelizedVariable< MyAlloc > xtrans4 =
        dynamic_cast<
          const gum::learning::DBTranslator4LabelizedVariable< MyAlloc >& >(
          set4.translator(1));
      TS_ASSERT(xtrans4.translate("toto").discr_val == 0);
      TS_ASSERT(xtrans4.translate("titi").discr_val == 1);
    }


    void test_trans3() {
      gum::learning::DBTranslatorSet<> set;

      std::vector< std::string >                      missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable<> translator1(missing, 3);
      set.insertTranslator(translator1, 1);

      gum::learning::DBTranslator4ContinuousVariable<> translator0;
      set.insertTranslator(translator0, 0);

      gum::learning::DBTranslator4LabelizedVariable<> translator2;
      set.insertTranslator(translator2, 2);

      std::vector< std::string > row1{".33", "toto", "titi"};
      TS_ASSERT(set.translate(row1, 1).discr_val == 0);
      TS_ASSERT(set.translate(row1, 0).cont_val == 0.33f);
      TS_ASSERT(set.translate(row1, 2).discr_val == 0);

      std::vector< std::string > row2{".22", "titi", "toto"};
      TS_ASSERT(set.translate(row2, 1).discr_val == 1);
      TS_ASSERT(set.translate(row2, 0).cont_val == 0.22f);
      TS_ASSERT(set.translate(row2, 2).discr_val == 1);

      std::vector< std::string > row3{".33", "toto", "toto"};
      TS_ASSERT(set.translate(row3, 1).discr_val == 0);
      TS_ASSERT(set.translate(row3, 0).cont_val == 0.33f);
      TS_ASSERT(set.translate(row3, 2).discr_val == 1);


      gum::learning::DBTranslatorSet<>* set4 = set.clone();
      TS_ASSERT(set4->translate(row1, 1).discr_val == 0);
      TS_ASSERT(set4->translate(row1, 0).cont_val == 0.33f);
      TS_ASSERT(set4->translate(row1, 2).discr_val == 0);
      TS_ASSERT(set4->translate(row2, 1).discr_val == 1);
      TS_ASSERT(set4->translate(row2, 0).cont_val == 0.22f);
      TS_ASSERT(set4->translate(row2, 2).discr_val == 1);
      TS_ASSERT(set4->translate(row3, 1).discr_val == 0);
      TS_ASSERT(set4->translate(row3, 0).cont_val == 0.33f);
      TS_ASSERT(set4->translate(row3, 2).discr_val == 1);
      gum::learning::DBTranslator4LabelizedVariable<> xtrans4 =
        dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable<>& >(
          set4->translator(1));
      TS_ASSERT(xtrans4.translate("toto").discr_val == 0);
      TS_ASSERT(xtrans4.translate("titi").discr_val == 1);

      delete set4;
    }


    void test_trans4() {
      {
        MyAlloc< gum::learning::DBTranslatorSet< MyAlloc > > alloc;
        gum::learning::DBTranslatorSet< MyAlloc >            set(alloc);

        std::vector< std::string > missing{"?", "N/A", "???"};
        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > translator1(
          missing, 3);
        set.insertTranslator(translator1, 1);

        gum::learning::DBTranslator4ContinuousVariable< MyAlloc > translator0;
        set.insertTranslator(translator0, 0);

        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > translator2;
        set.insertTranslator(translator2, 2);

        std::vector< std::string > row1{".33", "toto", "titi"};
        TS_ASSERT(set.translate(row1, 1).discr_val == 0);
        TS_ASSERT(set.translate(row1, 0).cont_val == 0.33f);
        TS_ASSERT(set.translate(row1, 2).discr_val == 0);

        std::vector< std::string > row2{".22", "titi", "toto"};
        TS_ASSERT(set.translate(row2, 1).discr_val == 1);
        TS_ASSERT(set.translate(row2, 0).cont_val == 0.22f);
        TS_ASSERT(set.translate(row2, 2).discr_val == 1);

        std::vector< std::string > row3{".33", "toto", "toto"};
        TS_ASSERT(set.translate(row3, 1).discr_val == 0);
        TS_ASSERT(set.translate(row3, 0).cont_val == 0.33f);
        TS_ASSERT(set.translate(row3, 2).discr_val == 1);

        gum::learning::DBTranslatorSet< MyAlloc >* set4 = set.clone();
        TS_ASSERT(set4->translate(row1, 1).discr_val == 0);
        TS_ASSERT(set4->translate(row1, 0).cont_val == 0.33f);
        TS_ASSERT(set4->translate(row1, 2).discr_val == 0);
        TS_ASSERT(set4->translate(row2, 1).discr_val == 1);
        TS_ASSERT(set4->translate(row2, 0).cont_val == 0.22f);
        TS_ASSERT(set4->translate(row2, 2).discr_val == 1);
        TS_ASSERT(set4->translate(row3, 1).discr_val == 0);
        TS_ASSERT(set4->translate(row3, 0).cont_val == 0.33f);
        TS_ASSERT(set4->translate(row3, 2).discr_val == 1);
        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > xtrans4 =
          dynamic_cast<
            const gum::learning::DBTranslator4LabelizedVariable< MyAlloc >& >(
            set4->translator(1));
        TS_ASSERT(xtrans4.translate("toto").discr_val == 0);
        TS_ASSERT(xtrans4.translate("titi").discr_val == 1);

        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > xtrans4b =
          dynamic_cast<
            const gum::learning::DBTranslator4LabelizedVariable< MyAlloc >& >(
            set4->operator[](1));
        TS_ASSERT(xtrans4b.translate("toto").discr_val == 0);
        TS_ASSERT(xtrans4b.translate("titi").discr_val == 1);

        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > xtrans4t =
          dynamic_cast<
            const gum::learning::DBTranslator4LabelizedVariable< MyAlloc >& >(
            set4->translatorSafe(1));
        TS_ASSERT(xtrans4t.translate("toto").discr_val == 0);
        TS_ASSERT(xtrans4t.translate("titi").discr_val == 1);

        alloc.destroy(set4);
        alloc.deallocate(set4, 1);

        gum::learning::DBTranslatorSet< MyAlloc > set2;
        TS_ASSERT(set2.nbTranslators() == 0);
        set2 = set;
        TS_ASSERT(set2.translate(row3, 1).discr_val == 0);
        TS_ASSERT(set2.translate(row3, 0).cont_val == 0.33f);
        TS_ASSERT(set2.translate(row3, 2).discr_val == 1);
        TS_ASSERT(set2.translate(row3, 1).discr_val == 0);
        TS_ASSERT(set2.translate(row3, 0).cont_val == 0.33f);
        TS_ASSERT(set2.translate(row3, 2).discr_val == 1);

        gum::learning::DBTranslatorSet< MyAlloc > set3;
        TS_ASSERT(set3.nbTranslators() == 0);
        set3 = std::move(set);
        TS_ASSERT(set3.translate(row3, 1).discr_val == 0);
        TS_ASSERT(set3.translate(row3, 0).cont_val == 0.33f);
        TS_ASSERT(set3.translate(row3, 2).discr_val == 1);
        TS_ASSERT(set3.translate(row3, 1).discr_val == 0);
        TS_ASSERT(set3.translate(row3, 0).cont_val == 0.33f);
        TS_ASSERT(set3.translate(row3, 2).discr_val == 1);
      }

      TS_ASSERT(MyAllocCount::hasMeroryLeak() == false);
    }


    void test_trans5() {
      gum::learning::DBTranslatorSet<> set;
      std::size_t                      pos;

      std::vector< std::string > missing{"?", "N/A", "???"};
      gum::LabelizedVariable     var1("var1", "", 0);
      var1.addLabel("toto");
      var1.addLabel("titi");
      pos = set.insertTranslator<>(var1, 1, missing);
      TS_ASSERT(pos == std::size_t(0));

      gum::ContinuousVariable< double > var0("var0", "");
      pos = set.insertTranslator< double >(var0, 0, missing);
      TS_ASSERT(pos == std::size_t(0));

      gum::LabelizedVariable var2("var2", "", 0);
      var2.addLabel("titi");
      var2.addLabel("toto");
      pos = set.insertTranslator(var2, 2, missing);
      TS_ASSERT(pos == std::size_t(2));

      std::vector< std::string > row1{".33", "toto", "titi"};
      TS_ASSERT(set.translate(row1, 1).discr_val == 0);
      TS_ASSERT(set.translate(row1, 0).cont_val == 0.33f);
      TS_ASSERT(set.translate(row1, 2).discr_val == 0);

      std::vector< std::string > row2{".22", "titi", "toto"};
      TS_ASSERT(set.translate(row2, 1).discr_val == 1);
      TS_ASSERT(set.translate(row2, 0).cont_val == 0.22f);
      TS_ASSERT(set.translate(row2, 2).discr_val == 1);

      std::vector< std::string > row3{".33", "toto", "toto"};
      TS_ASSERT(set.translate(row3, 1).discr_val == 0);
      TS_ASSERT(set.translate(row3, 0).cont_val == 0.33f);
      TS_ASSERT(set.translate(row3, 2).discr_val == 1);
    }
  };

} /* namespace gum_tests */