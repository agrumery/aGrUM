/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <iostream>
#include <vector>
#include <string>
#include <ressources/include/myalloc.h>
#include <ressources/include/learningAlloc.h>

#include <agrum/tools/database/DBTranslatorSet.h>
#include <agrum/tools/database/DBTranslator4LabelizedVariable.h>
#include <agrum/tools/database/DBTranslator4DiscretizedVariable.h>
#include <agrum/tools/database/DBTranslator4RangeVariable.h>
#include <agrum/tools/database/DBTranslator4ContinuousVariable.h>


namespace gum_tests {

  class DBTranslatorSetTestSuite: public CxxTest::TestSuite {
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
      TS_ASSERT(pos == std::size_t(1));

      gum::learning::DBTranslator4LabelizedVariable<> translator2;
      pos = set.insertTranslator(translator2, 2);
      TS_ASSERT(pos == std::size_t(2));

      std::vector< std::string > row1{".33", "toto", "titi"};
      TS_ASSERT(set.translate(row1, 0).discr_val == 0);
      TS_ASSERT(set.translate(row1, 1).cont_val == 0.33f);
      TS_ASSERT(set.translate(row1, 2).discr_val == 0);

      std::vector< std::string > row2{".22", "titi", "toto"};
      TS_ASSERT(set.translate(row2, 0).discr_val == 1);
      TS_ASSERT(set.translate(row2, 1).cont_val == 0.22f);
      TS_ASSERT(set.translate(row2, 2).discr_val == 1);

      std::vector< std::string > row3{".33", "toto", "toto"};
      TS_ASSERT(set.translate(row3, 0).discr_val == 0);
      TS_ASSERT(set.translate(row3, 1).cont_val == 0.33f);
      TS_ASSERT(set.translate(row3, 2).discr_val == 1);

      std::vector< std::string > row4{"???", "???", "???"};
      TS_ASSERT(set.translate(row4, 0).discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT_THROWS(set.translate(row4, 1), gum::TypeError);

      std::vector< std::string > row5{"4.22x", "???", "???"};
      TS_ASSERT_THROWS(set.translate(row5, 1), gum::TypeError);
      TS_ASSERT(set.translate(row5, 2).discr_val == 2);

      TS_ASSERT_THROWS(set.translateSafe(row5, 3), gum::UndefinedElement);

      TS_ASSERT(
         set.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}, 0)
         == "toto");
      TS_ASSERT(
         std::stof(set.translateBack(gum::learning::DBTranslatedValue{7.42f}, 1))
         == 7.42f);
      TS_ASSERT(
         set.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}, 2)
         == "???");
      TS_ASSERT_THROWS(set.translateBackSafe(
                          gum::learning::DBTranslatedValue{std::size_t{0}}, 3),
                       gum::UndefinedElement);

      TS_ASSERT(set.domainSize(0) == 2);
      TS_ASSERT(set.domainSize(2) == 3);
      TS_ASSERT(set.domainSize(1) == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(set.domainSizeSafe(0) == 2);
      TS_ASSERT(set.domainSizeSafe(2) == 3);
      TS_ASSERT(set.domainSizeSafe(1)
                == std::numeric_limits< std::size_t >::max());

      gum::learning::DBTranslator4LabelizedVariable<> xtrans =
         dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable<>& >(
            set.translator(0));
      TS_ASSERT(xtrans.translate("toto").discr_val == 0);
      TS_ASSERT(xtrans.translate("titi").discr_val == 1);
      TS_ASSERT(xtrans.translate("???").discr_val
                == std::numeric_limits< std::size_t >::max());

      gum::learning::DBTranslator4LabelizedVariable<> xtransb =
         dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable<>& >(
            set.translatorSafe(0));
      TS_ASSERT(xtransb.translate("toto").discr_val == 0);
      TS_ASSERT(xtransb.translate("titi").discr_val == 1);
      TS_ASSERT(xtransb.translate("???").discr_val
                == std::numeric_limits< std::size_t >::max());

      TS_ASSERT(set.nbTranslators() == 3);
      TS_ASSERT(set.size() == 3);
      TS_ASSERT(set.getAllocator() == std::allocator< std::size_t >());

      gum::LabelizedVariable var =
         dynamic_cast< const gum::LabelizedVariable& >(set.variable(0));
      TS_ASSERT(var.domainSize() == 2);
      TS_ASSERT(var.label(0) == "toto");
      TS_ASSERT(var.label(1) == "titi");

      gum::LabelizedVariable varb =
         dynamic_cast< const gum::LabelizedVariable& >(set.variableSafe(0));
      TS_ASSERT(varb.domainSize() == 2);
      TS_ASSERT(varb.label(0) == "toto");
      TS_ASSERT(varb.label(1) == "titi");

      const gum::learning::DBTranslatedValue miss_disc{
         std::numeric_limits< std::size_t >::max()};
      const gum::learning::DBTranslatedValue miss_cont{
         std::numeric_limits< float >::max()};
      TS_ASSERT(set.isMissingValue(miss_disc, 0));
      TS_ASSERT(set.isMissingValue(miss_cont, 1));
      TS_ASSERT(set.isMissingValue(miss_disc, 2));
      TS_ASSERT(set.isMissingValueSafe(miss_disc, 0));
      TS_ASSERT(set.isMissingValueSafe(miss_cont, 1));
      TS_ASSERT(set.isMissingValueSafe(miss_disc, 2));

      TS_ASSERT(set.needsReordering(0));
      TS_ASSERT(set.needsReorderingSafe(0));
      const auto order = set.reorder(0);
      TS_ASSERT(order[0] == std::size_t(1));
      TS_ASSERT(order[1] == std::size_t(0));

      TS_ASSERT(set.inputColumn(0) == std::size_t(1));
      TS_ASSERT(set.inputColumn(1) == std::size_t(0));
      TS_ASSERT(set.inputColumn(2) == std::size_t(2));
      TS_ASSERT(set.inputColumnSafe(0) == std::size_t(1));
      TS_ASSERT(set.inputColumnSafe(1) == std::size_t(0));
      TS_ASSERT(set.inputColumnSafe(2) == std::size_t(2));
      TS_ASSERT(set.highestInputColumn() == std::size_t(2));

      set.clear();
      TS_ASSERT(set.nbTranslators() == 0);
      TS_ASSERT(set.getAllocator() == std::allocator< std::size_t >());
    }

    void test_trans2() {
      {
        gum::learning::DBTranslatorSet< MyAlloc > set;

        std::vector< std::string > missing{"?", "N/A", "???"};
        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > translator1(
           missing, 3);
        set.insertTranslator(translator1, 1);

        gum::learning::DBTranslator4ContinuousVariable< MyAlloc > translator0;
        set.insertTranslator(translator0, 0);

        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > translator2;
        set.insertTranslator(translator2, 2);

        std::vector< std::string > row1{".33", "toto", "titi"};
        TS_ASSERT(set.translate(row1, 0).discr_val == 0);
        TS_ASSERT(set.translate(row1, 1).cont_val == 0.33f);
        TS_ASSERT(set.translate(row1, 2).discr_val == 0);

        std::vector< std::string > row2{".22", "titi", "toto"};
        TS_ASSERT(set.translate(row2, 0).discr_val == 1);
        TS_ASSERT(set.translate(row2, 1).cont_val == 0.22f);
        TS_ASSERT(set.translate(row2, 2).discr_val == 1);

        std::vector< std::string > row3{".33", "toto", "toto"};
        TS_ASSERT(set.translate(row3, 0).discr_val == 0);
        TS_ASSERT(set.translate(row3, 1).cont_val == 0.33f);
        TS_ASSERT(set.translate(row3, 2).discr_val == 1);

        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > xtrans =
           dynamic_cast<
              const gum::learning::DBTranslator4LabelizedVariable< MyAlloc >& >(
              set.translator(0));
        TS_ASSERT(xtrans.translate("toto").discr_val == 0);
        TS_ASSERT(xtrans.translate("titi").discr_val == 1);
        TS_ASSERT(xtrans.translate("???").discr_val
                  == std::numeric_limits< std::size_t >::max());

        gum::learning::DBTranslatorSet< MyAlloc > set2(set);
        TS_ASSERT(set2.translate(row1, 0).discr_val == 0);
        TS_ASSERT(set2.translate(row1, 1).cont_val == 0.33f);
        TS_ASSERT(set2.translate(row1, 2).discr_val == 0);
        TS_ASSERT(set2.translate(row2, 0).discr_val == 1);
        TS_ASSERT(set2.translate(row2, 1).cont_val == 0.22f);
        TS_ASSERT(set2.translate(row2, 2).discr_val == 1);
        TS_ASSERT(set2.translate(row3, 0).discr_val == 0);
        TS_ASSERT(set2.translate(row3, 1).cont_val == 0.33f);
        TS_ASSERT(set2.translate(row3, 2).discr_val == 1);
        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > xtrans2 =
           dynamic_cast<
              const gum::learning::DBTranslator4LabelizedVariable< MyAlloc >& >(
              set2.translator(0));
        TS_ASSERT(xtrans2.translate("toto").discr_val == 0);
        TS_ASSERT(xtrans2.translate("titi").discr_val == 1);

        gum::learning::DBTranslatorSet< MyAlloc > set3(std::move(set2));
        TS_ASSERT(set3.translate(row1, 0).discr_val == 0);
        TS_ASSERT(set3.translate(row1, 1).cont_val == 0.33f);
        TS_ASSERT(set3.translate(row1, 2).discr_val == 0);
        TS_ASSERT(set3.translate(row2, 0).discr_val == 1);
        TS_ASSERT(set3.translate(row2, 1).cont_val == 0.22f);
        TS_ASSERT(set3.translate(row2, 2).discr_val == 1);
        TS_ASSERT(set3.translate(row3, 0).discr_val == 0);
        TS_ASSERT(set3.translate(row3, 1).cont_val == 0.33f);
        TS_ASSERT(set3.translate(row3, 2).discr_val == 1);
        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > xtrans3 =
           dynamic_cast<
              const gum::learning::DBTranslator4LabelizedVariable< MyAlloc >& >(
              set3.translator(0));
        TS_ASSERT(xtrans3.translate("toto").discr_val == 0);
        TS_ASSERT(xtrans3.translate("titi").discr_val == 1);

        gum::learning::DBTranslatorSet< MyAlloc > set4(
           set, MyAlloc< gum::learning::DBTranslatedValue >());
        TS_ASSERT(set4.translate(row1, 0).discr_val == 0);
        TS_ASSERT(set4.translate(row1, 1).cont_val == 0.33f);
        TS_ASSERT(set4.translate(row1, 2).discr_val == 0);
        TS_ASSERT(set4.translate(row2, 0).discr_val == 1);
        TS_ASSERT(set4.translate(row2, 1).cont_val == 0.22f);
        TS_ASSERT(set4.translate(row2, 2).discr_val == 1);
        TS_ASSERT(set4.translate(row3, 0).discr_val == 0);
        TS_ASSERT(set4.translate(row3, 1).cont_val == 0.33f);
        TS_ASSERT(set4.translate(row3, 2).discr_val == 1);
        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > xtrans4 =
           dynamic_cast<
              const gum::learning::DBTranslator4LabelizedVariable< MyAlloc >& >(
              set4.translator(0));
        TS_ASSERT(xtrans4.translate("toto").discr_val == 0);
        TS_ASSERT(xtrans4.translate("titi").discr_val == 1);

        gum::learning::DBTranslatorSet< MyAlloc > set5(
           std::move(set2), MyAlloc< gum::learning::DBTranslatedValue >());
        TS_ASSERT(set5.translate(row1, 0).discr_val == 0);
        TS_ASSERT(set5.translate(row1, 1).cont_val == 0.33f);
        TS_ASSERT(set5.translate(row1, 2).discr_val == 0);
        TS_ASSERT(set5.translate(row2, 0).discr_val == 1);
        TS_ASSERT(set5.translate(row2, 1).cont_val == 0.22f);
        TS_ASSERT(set5.translate(row2, 2).discr_val == 1);
        TS_ASSERT(set5.translate(row3, 0).discr_val == 0);
        TS_ASSERT(set5.translate(row3, 1).cont_val == 0.33f);
        TS_ASSERT(set5.translate(row3, 2).discr_val == 1);
        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > xtrans5 =
           dynamic_cast<
              const gum::learning::DBTranslator4LabelizedVariable< MyAlloc >& >(
              set5.translator(0));
        TS_ASSERT(xtrans5.translate("toto").discr_val == 0);
        TS_ASSERT(xtrans5.translate("titi").discr_val == 1);

        const auto ordersafe = set.reorderSafe(0);
        TS_ASSERT(ordersafe[0] == std::size_t(1));
        TS_ASSERT(ordersafe[1] == std::size_t(0));
      }

      TS_ASSERT(MyAllocCount::hasMeroryLeak() == false);
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
      TS_ASSERT(set.translate(row1, 0).discr_val == 0);
      TS_ASSERT(set.translate(row1, 1).cont_val == 0.33f);
      TS_ASSERT(set.translate(row1, 2).discr_val == 0);

      std::vector< std::string > row2{".22", "titi", "toto"};
      TS_ASSERT(set.translate(row2, 0).discr_val == 1);
      TS_ASSERT(set.translate(row2, 1).cont_val == 0.22f);
      TS_ASSERT(set.translate(row2, 2).discr_val == 1);

      std::vector< std::string > row3{".33", "toto", "toto"};
      TS_ASSERT(set.translate(row3, 0).discr_val == 0);
      TS_ASSERT(set.translate(row3, 1).cont_val == 0.33f);
      TS_ASSERT(set.translate(row3, 2).discr_val == 1);


      gum::learning::DBTranslatorSet<>* set4 = set.clone();
      TS_ASSERT(set4->translate(row1, 0).discr_val == 0);
      TS_ASSERT(set4->translate(row1, 1).cont_val == 0.33f);
      TS_ASSERT(set4->translate(row1, 2).discr_val == 0);
      TS_ASSERT(set4->translate(row2, 0).discr_val == 1);
      TS_ASSERT(set4->translate(row2, 1).cont_val == 0.22f);
      TS_ASSERT(set4->translate(row2, 2).discr_val == 1);
      TS_ASSERT(set4->translate(row3, 0).discr_val == 0);
      TS_ASSERT(set4->translate(row3, 1).cont_val == 0.33f);
      TS_ASSERT(set4->translate(row3, 2).discr_val == 1);
      gum::learning::DBTranslator4LabelizedVariable<> xtrans4 =
         dynamic_cast< const gum::learning::DBTranslator4LabelizedVariable<>& >(
            set4->translator(0));
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
        TS_ASSERT(set.translate(row1, 0).discr_val == 0);
        TS_ASSERT(set.translate(row1, 1).cont_val == 0.33f);
        TS_ASSERT(set.translate(row1, 2).discr_val == 0);

        std::vector< std::string > row2{".22", "titi", "toto"};
        TS_ASSERT(set.translate(row2, 0).discr_val == 1);
        TS_ASSERT(set.translate(row2, 1).cont_val == 0.22f);
        TS_ASSERT(set.translate(row2, 2).discr_val == 1);

        std::vector< std::string > row3{".33", "toto", "toto"};
        TS_ASSERT(set.translate(row3, 0).discr_val == 0);
        TS_ASSERT(set.translate(row3, 1).cont_val == 0.33f);
        TS_ASSERT(set.translate(row3, 2).discr_val == 1);

        gum::learning::DBTranslatorSet< MyAlloc >* set4 =
           set.clone(MyAlloc< gum::learning::DBTranslatedValue >());
        TS_ASSERT(set4->translate(row1, 0).discr_val == 0);
        TS_ASSERT(set4->translate(row1, 1).cont_val == 0.33f);
        TS_ASSERT(set4->translate(row1, 2).discr_val == 0);
        TS_ASSERT(set4->translate(row2, 0).discr_val == 1);
        TS_ASSERT(set4->translate(row2, 1).cont_val == 0.22f);
        TS_ASSERT(set4->translate(row2, 2).discr_val == 1);
        TS_ASSERT(set4->translate(row3, 0).discr_val == 0);
        TS_ASSERT(set4->translate(row3, 1).cont_val == 0.33f);
        TS_ASSERT(set4->translate(row3, 2).discr_val == 1);
        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > xtrans4 =
           dynamic_cast<
              const gum::learning::DBTranslator4LabelizedVariable< MyAlloc >& >(
              set4->translator(0));
        TS_ASSERT(xtrans4.translate("toto").discr_val == 0);
        TS_ASSERT(xtrans4.translate("titi").discr_val == 1);

        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > xtrans4b =
           dynamic_cast<
              const gum::learning::DBTranslator4LabelizedVariable< MyAlloc >& >(
              set4->operator[](0));
        TS_ASSERT(xtrans4b.translate("toto").discr_val == 0);
        TS_ASSERT(xtrans4b.translate("titi").discr_val == 1);

        gum::learning::DBTranslator4LabelizedVariable< MyAlloc > xtrans4t =
           dynamic_cast<
              const gum::learning::DBTranslator4LabelizedVariable< MyAlloc >& >(
              set4->translatorSafe(0));
        TS_ASSERT(xtrans4t.translate("toto").discr_val == 0);
        TS_ASSERT(xtrans4t.translate("titi").discr_val == 1);

        alloc.destroy(set4);
        alloc.deallocate(set4, 1);

        gum::learning::DBTranslatorSet< MyAlloc > set2;
        TS_ASSERT(set2.nbTranslators() == 0);
        set2 = set;
        TS_ASSERT(set2.translate(row3, 0).discr_val == 0);
        TS_ASSERT(set2.translate(row3, 1).cont_val == 0.33f);
        TS_ASSERT(set2.translate(row3, 2).discr_val == 1);
        TS_ASSERT(set2.translate(row3, 0).discr_val == 0);
        TS_ASSERT(set2.translate(row3, 1).cont_val == 0.33f);
        TS_ASSERT(set2.translate(row3, 2).discr_val == 1);

        gum::learning::DBTranslatorSet< MyAlloc > set3;
        TS_ASSERT(set3.nbTranslators() == 0);
        set3 = std::move(set);
        TS_ASSERT(set3.translate(row3, 0).discr_val == 0);
        TS_ASSERT(set3.translate(row3, 1).cont_val == 0.33f);
        TS_ASSERT(set3.translate(row3, 2).discr_val == 1);
        TS_ASSERT(set3.translate(row3, 0).discr_val == 0);
        TS_ASSERT(set3.translate(row3, 1).cont_val == 0.33f);
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
      pos = set.insertTranslator(var1, 1, missing, false);
      TS_ASSERT(pos == std::size_t(0));

      gum::ContinuousVariable< double > var0("var0", "");
      pos = set.insertTranslator(var0, 0, missing);
      TS_ASSERT(pos == std::size_t(1));

      gum::LabelizedVariable var2("var2", "", 0);
      var2.addLabel("titi");
      var2.addLabel("toto");
      pos = set.insertTranslator(var2, 2, missing);
      TS_ASSERT(pos == std::size_t(2));

      std::vector< std::string > row1{".33", "toto", "titi"};
      TS_ASSERT(set.translate(row1, 0).discr_val == 0);
      TS_ASSERT(set.translate(row1, 1).cont_val == 0.33f);
      TS_ASSERT(set.translate(row1, 2).discr_val == 0);

      std::vector< std::string > row2{".22", "titi", "toto"};
      TS_ASSERT(set.translate(row2, 0).discr_val == 1);
      TS_ASSERT(set.translate(row2, 1).cont_val == 0.22f);
      TS_ASSERT(set.translate(row2, 2).discr_val == 1);

      std::vector< std::string > row3{".33", "toto", "toto"};
      TS_ASSERT(set.translate(row3, 0).discr_val == 0);
      TS_ASSERT(set.translate(row3, 1).cont_val == 0.33f);
      TS_ASSERT(set.translate(row3, 2).discr_val == 1);
    }


    void test_trans6() {
      gum::learning::DBTranslatorSet<> set;

      std::vector< std::string >                      missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable<> translator1(missing, 3);
      std::size_t                                     pos;
      pos = set.insertTranslator(translator1, 1, false);
      TS_ASSERT(pos == std::size_t(0));

      gum::learning::DBTranslator4ContinuousVariable<> translator0;
      pos = set.insertTranslator(translator0, 0);
      TS_ASSERT(pos == std::size_t(1));

      gum::learning::DBTranslator4LabelizedVariable<> translator2;
      pos = set.insertTranslator(translator2, 2);
      TS_ASSERT(pos == std::size_t(2));

      pos = set.insertTranslator(translator1, 1, false);
      TS_ASSERT(pos == std::size_t(3));

      std::vector< std::string > row1{".33", "toto", "titi"};
      TS_ASSERT(set.translate(row1, 0).discr_val == 0);
      TS_ASSERT(set.translate(row1, 1).cont_val == 0.33f);
      TS_ASSERT(set.translate(row1, 2).discr_val == 0);
      TS_ASSERT(set.translate(row1, 3).discr_val == 0);

      std::vector< std::string > row2{".22", "titi", "toto"};
      TS_ASSERT(set.translate(row2, 0).discr_val == 1);
      TS_ASSERT(set.translate(row2, 1).cont_val == 0.22f);
      TS_ASSERT(set.translate(row2, 2).discr_val == 1);
      TS_ASSERT(set.translate(row2, 3).discr_val == 1);

      std::vector< std::string > row3{".33", "toto", "toto"};
      TS_ASSERT(set.translate(row3, 0).discr_val == 0);
      TS_ASSERT(set.translate(row3, 1).cont_val == 0.33f);
      TS_ASSERT(set.translate(row3, 2).discr_val == 1);
      TS_ASSERT(set.translate(row3, 3).discr_val == 0);

      std::vector< std::string > row4{"???", "???", "???"};
      TS_ASSERT(set.translate(row4, 0).discr_val
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT_THROWS(set.translate(row4, 1), gum::TypeError);
      TS_ASSERT(set.translate(row4, 3).discr_val
                == std::numeric_limits< std::size_t >::max());

      std::vector< std::string > row5{"4.22x", "???", "???"};
      TS_ASSERT_THROWS(set.translate(row5, 1), gum::TypeError);
      TS_ASSERT(set.translate(row5, 2).discr_val == 2);

      TS_ASSERT_THROWS(set.translateSafe(row5, 4), gum::UndefinedElement);

      TS_ASSERT(
         set.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}, 0)
         == "toto");
      TS_ASSERT(
         set.translateBack(gum::learning::DBTranslatedValue{std::size_t{0}}, 3)
         == "toto");
      TS_ASSERT(
         std::stof(set.translateBack(gum::learning::DBTranslatedValue{7.42f}, 1))
         == 7.42f);
      TS_ASSERT(
         set.translateBack(gum::learning::DBTranslatedValue{std::size_t{2}}, 2)
         == "???");

      TS_ASSERT(set.domainSize(0) == 2);
      TS_ASSERT(set.domainSize(2) == 3);
      TS_ASSERT(set.domainSize(1) == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(set.domainSize(3) == 2);
      TS_ASSERT(set.domainSizeSafe(0) == 2);
      TS_ASSERT(set.domainSizeSafe(2) == 3);
      TS_ASSERT(set.domainSizeSafe(1)
                == std::numeric_limits< std::size_t >::max());
      TS_ASSERT(set.domainSizeSafe(3) == 2);
    }

    void test_trans7() {
      gum::learning::DBTranslatorSet<> set;
      std::size_t                      pos;

      gum::LabelizedVariable var1("var1", "", 0);
      var1.addLabel("toto");
      var1.addLabel("titi");
      pos = set.insertTranslator(var1, 1, false);
      TS_ASSERT(pos == std::size_t(0));

      gum::ContinuousVariable< double > var0("var0", "");
      pos = set.insertTranslator(var0, 0);
      TS_ASSERT(pos == std::size_t(1));

      gum::LabelizedVariable var2("var2", "", 0);
      var2.addLabel("titi");
      var2.addLabel("toto");
      pos = set.insertTranslator(var2, 2);
      TS_ASSERT(pos == std::size_t(2));

      std::vector< std::string > row1{".33", "toto", "titi"};
      TS_ASSERT(set.translate(row1, 0).discr_val == 0);
      TS_ASSERT(set.translate(row1, 1).cont_val == 0.33f);
      TS_ASSERT(set.translate(row1, 2).discr_val == 0);
    }


    void test_trans8() {
      gum::learning::DBTranslatorSet<> set;

      std::vector< std::string >                      missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable<> translator1(missing, 3);
      translator1.setVariableName("X1");

      std::size_t pos;
      pos = set.insertTranslator(translator1, 1);
      TS_ASSERT(pos == std::size_t(0));

      gum::learning::DBTranslator4ContinuousVariable<> translator0;
      translator0.setVariableName("X0");
      pos = set.insertTranslator(translator0, 0);
      TS_ASSERT(pos == std::size_t(1));

      gum::learning::DBTranslator4LabelizedVariable<> translator2;
      translator2.setVariableName("X2");
      pos = set.insertTranslator(translator2, 2);
      TS_ASSERT(pos == std::size_t(2));

      translator1.setVariableName("Y1");
      pos = set.insertTranslator(translator1, 1, false);
      TS_ASSERT(pos == std::size_t(3));

      translator0.setVariableName("Y0");
      pos = set.insertTranslator(translator0, 0, false);
      TS_ASSERT(pos == std::size_t(4));

      translator0.setVariableName("Z0");
      pos = set.insertTranslator(translator0, 0, false);
      TS_ASSERT(pos == std::size_t(5));

      translator1.setVariableName("Z1");
      pos = set.insertTranslator(translator1, 1, false);
      TS_ASSERT(pos == std::size_t(6));

      const auto& vect_trans = set.translators();
      TS_ASSERT(vect_trans.size() == std::size_t(7));

      TS_ASSERT(vect_trans[0]->variable()->name() == "X1");
      TS_ASSERT(vect_trans[1]->variable()->name() == "X0");
      TS_ASSERT(vect_trans[2]->variable()->name() == "X2");
      TS_ASSERT(vect_trans[3]->variable()->name() == "Y1");
      TS_ASSERT(vect_trans[4]->variable()->name() == "Y0");
      TS_ASSERT(vect_trans[5]->variable()->name() == "Z0");
      TS_ASSERT(vect_trans[6]->variable()->name() == "Z1");
      TS_ASSERT(set.highestInputColumn() == std::size_t(2));

      set.eraseTranslator(4, false);
      TS_ASSERT(vect_trans.size() == std::size_t(6));
      TS_ASSERT(vect_trans[0]->variable()->name() == "X1");
      TS_ASSERT(vect_trans[1]->variable()->name() == "X0");
      TS_ASSERT(vect_trans[2]->variable()->name() == "X2");
      TS_ASSERT(vect_trans[3]->variable()->name() == "Y1");
      TS_ASSERT(vect_trans[4]->variable()->name() == "Z0");
      TS_ASSERT(vect_trans[5]->variable()->name() == "Z1");
      TS_ASSERT(set.highestInputColumn() == std::size_t(2));

      set.eraseTranslator(0, true);
      TS_ASSERT(vect_trans.size() == std::size_t(4));
      TS_ASSERT(vect_trans[0]->variable()->name() == "X1");
      TS_ASSERT(vect_trans[1]->variable()->name() == "X2");
      TS_ASSERT(vect_trans[2]->variable()->name() == "Y1");
      TS_ASSERT(vect_trans[3]->variable()->name() == "Z1");
      TS_ASSERT(set.highestInputColumn() == std::size_t(2));

      set.eraseTranslator(2, true);
      TS_ASSERT(vect_trans.size() == std::size_t(3));
      TS_ASSERT(vect_trans[0]->variable()->name() == "X1");
      TS_ASSERT(vect_trans[1]->variable()->name() == "Y1");
      TS_ASSERT(vect_trans[2]->variable()->name() == "Z1");
      TS_ASSERT(set.highestInputColumn() == std::size_t(1));

      set.eraseTranslator(1, true);
      TS_ASSERT(vect_trans.size() == std::size_t(0));
      TS_ASSERT(set.highestInputColumn() == std::size_t(0));
    }
  };

} /* namespace gum_tests */
