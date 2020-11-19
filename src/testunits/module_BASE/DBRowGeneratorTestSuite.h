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
#include <ressources/include/mygenerator.h>

#include <agrum/tools/database/DBRowGeneratorIdentity.h>

namespace gum_tests {

  class DBRowGeneratorTestSuite: public CxxTest::TestSuite {
    public:
    void test_identity() {
      const std::vector< gum::learning::DBTranslatedValueType > col_types{
         gum::learning::DBTranslatedValueType::DISCRETE,
         gum::learning::DBTranslatedValueType::DISCRETE,
         gum::learning::DBTranslatedValueType::CONTINUOUS,
         gum::learning::DBTranslatedValueType::DISCRETE};

      gum::learning::DBRowGeneratorIdentity<> generator(col_types);
      TS_ASSERT(!generator.hasRows());

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row1{
         gum::learning::DBTranslatedValue{std::size_t(0)},
         gum::learning::DBTranslatedValue{std::size_t(4)},
         gum::learning::DBTranslatedValue{4.5f},
         gum::learning::DBTranslatedValue{std::size_t(7)}};
      generator.setInputRow(input_row1);
      TS_ASSERT(generator.hasRows());
      TS_ASSERT(generator.hasRows());

      const auto& output_row1 = generator.generate();
      const auto& xoutput_row1 = output_row1.row();
      TS_ASSERT(xoutput_row1[0].discr_val == std::size_t(0));
      TS_ASSERT(xoutput_row1[1].discr_val == std::size_t(4));
      TS_ASSERT(xoutput_row1[2].cont_val == 4.5f);
      TS_ASSERT(xoutput_row1[3].discr_val == std::size_t(7));
      TS_ASSERT(!generator.hasRows());

      generator.setInputRow(input_row1);
      TS_ASSERT(generator.hasRows());
      TS_ASSERT(generator.hasRows());

      const auto& output_row2 = generator.generate();
      const auto& xoutput_row2 = output_row2.row();
      TS_ASSERT(xoutput_row2[0].discr_val == std::size_t(0));
      TS_ASSERT(xoutput_row2[1].discr_val == std::size_t(4));
      TS_ASSERT(xoutput_row2[2].cont_val == 4.5f);
      TS_ASSERT(xoutput_row2[3].discr_val == std::size_t(7));
      TS_ASSERT(!generator.hasRows());

      const std::vector< std::size_t > cols_of_interest{std::size_t(0),
                                                        std::size_t(2)};
      generator.setColumnsOfInterest(cols_of_interest);
      const auto& xcols = generator.columnsOfInterest();
      TS_ASSERT(xcols.size() == std::size_t(2));
      TS_ASSERT(xcols[0] == std::size_t(0));
      TS_ASSERT(xcols[1] == std::size_t(2));
      generator.setInputRow(input_row1);
      TS_ASSERT(generator.hasRows());
      const auto& output_row3 = generator.generate();
      const auto& xoutput_row3 = output_row3.row();
      TS_ASSERT(xoutput_row3[0].discr_val == std::size_t(0));
      TS_ASSERT(xoutput_row3[2].cont_val == 4.5f);

      generator.setInputRow(input_row1);
      TS_ASSERT(generator.hasRows());

      std::allocator< gum::learning::DBTranslatedValue > alloc;
      gum::learning::DBRowGeneratorIdentity<> generator2(generator, alloc);
      TS_ASSERT(generator2.hasRows());
      TS_ASSERT(generator2.columnsOfInterest().size() == std::size_t(2));
      TS_ASSERT(generator2.columnsOfInterest()[0] == std::size_t(0));
      TS_ASSERT(generator2.columnsOfInterest()[1] == std::size_t(2));
      const auto& youtput_row2 = generator2.generate();
      const auto& zoutput_row2 = youtput_row2.row();
      TS_ASSERT(zoutput_row2[0].discr_val == std::size_t(0));
      TS_ASSERT(zoutput_row2[2].cont_val == 4.5f);
      TS_ASSERT(!generator2.hasRows());
      generator2.setInputRow(input_row1);

      gum::learning::DBRowGeneratorIdentity<> generator3(generator);
      TS_ASSERT(generator3.hasRows());
      TS_ASSERT(generator3.columnsOfInterest().size() == std::size_t(2));
      TS_ASSERT(generator3.columnsOfInterest()[0] == std::size_t(0));
      TS_ASSERT(generator3.columnsOfInterest()[1] == std::size_t(2));
      const auto& youtput_row3 = generator3.generate();
      const auto& zoutput_row3 = youtput_row3.row();
      TS_ASSERT(zoutput_row3[0].discr_val == std::size_t(0));
      TS_ASSERT(zoutput_row3[2].cont_val == 4.5f);
      TS_ASSERT(!generator3.hasRows());
      generator3.setInputRow(input_row1);

      gum::learning::DBRowGeneratorIdentity<> generator4(std::move(generator2),
                                                         alloc);
      TS_ASSERT(generator4.hasRows());
      TS_ASSERT(generator4.columnsOfInterest().size() == std::size_t(2));
      TS_ASSERT(generator4.columnsOfInterest()[0] == std::size_t(0));
      TS_ASSERT(generator4.columnsOfInterest()[1] == std::size_t(2));
      const auto& youtput_row4 = generator4.generate();
      const auto& zoutput_row4 = youtput_row4.row();
      TS_ASSERT(zoutput_row4[0].discr_val == std::size_t(0));
      TS_ASSERT(zoutput_row4[2].cont_val == 4.5f);
      TS_ASSERT(!generator4.hasRows());
      generator4.setInputRow(input_row1);

      gum::learning::DBRowGeneratorIdentity<> generator5(std::move(generator3));
      TS_ASSERT(generator5.hasRows());
      TS_ASSERT(generator5.columnsOfInterest().size() == std::size_t(2));
      TS_ASSERT(generator5.columnsOfInterest()[0] == std::size_t(0));
      TS_ASSERT(generator5.columnsOfInterest()[1] == std::size_t(2));
      const auto& youtput_row5 = generator5.generate();
      const auto& zoutput_row5 = youtput_row5.row();
      TS_ASSERT(zoutput_row5[0].discr_val == std::size_t(0));
      TS_ASSERT(zoutput_row5[2].cont_val == 4.5f);
      TS_ASSERT(!generator5.hasRows());
      generator5.setInputRow(input_row1);

      gum::learning::DBRowGeneratorIdentity<>* generator6 =
         generator4.clone(alloc);
      TS_ASSERT(generator6->hasRows());
      TS_ASSERT(generator6->columnsOfInterest().size() == std::size_t(2));
      TS_ASSERT(generator6->columnsOfInterest()[0] == std::size_t(0));
      TS_ASSERT(generator6->columnsOfInterest()[1] == std::size_t(2));
      const auto& youtput_row6 = generator6->generate();
      const auto& zoutput_row6 = youtput_row6.row();
      TS_ASSERT(zoutput_row6[0].discr_val == std::size_t(0));
      TS_ASSERT(zoutput_row6[2].cont_val == 4.5f);
      TS_ASSERT(!generator6->hasRows());
      delete generator6;

      gum::learning::DBRowGeneratorIdentity<>* generator7 = generator5.clone();
      TS_ASSERT(generator7->hasRows());
      TS_ASSERT(generator7->columnsOfInterest().size() == std::size_t(2));
      TS_ASSERT(generator7->columnsOfInterest()[0] == std::size_t(0));
      TS_ASSERT(generator7->columnsOfInterest()[1] == std::size_t(2));
      const auto& youtput_row7 = generator7->generate();
      const auto& zoutput_row7 = youtput_row7.row();
      TS_ASSERT(zoutput_row7[0].discr_val == std::size_t(0));
      TS_ASSERT(zoutput_row7[2].cont_val == 4.5f);
      TS_ASSERT(!generator7->hasRows());
      delete generator7;

      gum::learning::DBRowGeneratorIdentity<> generator8(col_types);
      TS_ASSERT(!generator8.hasRows());
      TS_ASSERT(generator8.columnsOfInterest().size() == std::size_t(0));

      generator.setInputRow(input_row1);
      TS_ASSERT(!generator8.hasRows());
      TS_ASSERT(generator8.columnsOfInterest().size() == std::size_t(0));
      generator8 = generator;
      TS_ASSERT(generator8.hasRows());
      TS_ASSERT(generator8.columnsOfInterest().size() == std::size_t(2));
      TS_ASSERT(generator8.columnsOfInterest()[0] == std::size_t(0));
      TS_ASSERT(generator8.columnsOfInterest()[1] == std::size_t(2));
      const auto& youtput_row8 = generator8.generate();
      const auto& zoutput_row8 = youtput_row8.row();
      TS_ASSERT(zoutput_row8[0].discr_val == std::size_t(0));
      TS_ASSERT(zoutput_row8[2].cont_val == 4.5f);
      TS_ASSERT(!generator8.hasRows());

      gum::learning::DBRowGeneratorIdentity<> generator9(col_types);
      TS_ASSERT(!generator9.hasRows());
      TS_ASSERT(generator9.columnsOfInterest().size() == std::size_t(0));

      generator.setInputRow(input_row1);
      TS_ASSERT(!generator9.hasRows());
      TS_ASSERT(generator9.columnsOfInterest().size() == std::size_t(0));
      generator9 = std::move(generator);
      TS_ASSERT(generator9.hasRows());
      TS_ASSERT(generator9.columnsOfInterest().size() == std::size_t(2));
      TS_ASSERT(generator9.columnsOfInterest()[0] == std::size_t(0));
      TS_ASSERT(generator9.columnsOfInterest()[1] == std::size_t(2));
      const auto& youtput_row9 = generator9.generate();
      const auto& zoutput_row9 = youtput_row9.row();
      TS_ASSERT(zoutput_row9[0].discr_val == std::size_t(0));
      TS_ASSERT(zoutput_row9[2].cont_val == 4.5f);
      TS_ASSERT(!generator9.hasRows());
    }


    void test_duplications() {
      const std::vector< gum::learning::DBTranslatedValueType > col_types{
         gum::learning::DBTranslatedValueType::DISCRETE,
         gum::learning::DBTranslatedValueType::DISCRETE,
         gum::learning::DBTranslatedValueType::CONTINUOUS,
         gum::learning::DBTranslatedValueType::DISCRETE};

      gum::learning::MyGenerator<> generator(col_types, 3);
      TS_ASSERT(!generator.hasRows());

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row1{
         gum::learning::DBTranslatedValue{std::size_t(0)},
         gum::learning::DBTranslatedValue{std::size_t(4)},
         gum::learning::DBTranslatedValue{4.5f},
         gum::learning::DBTranslatedValue{std::size_t(7)}};
      generator.setInputRow(input_row1);
      TS_ASSERT(generator.hasRows());

      TS_ASSERT(generator.hasRows());
      const auto& output_row1 = generator.generate();
      const auto& xoutput_row1 = output_row1.row();
      TS_ASSERT(xoutput_row1[0].discr_val == std::size_t(0));
      TS_ASSERT(xoutput_row1[1].discr_val == std::size_t(4));
      TS_ASSERT(xoutput_row1[2].cont_val == 4.5f);
      TS_ASSERT(xoutput_row1[3].discr_val == std::size_t(7));

      TS_ASSERT(generator.hasRows());
      const auto& output_row2 = generator.generate();
      const auto& xoutput_row2 = output_row2.row();
      TS_ASSERT(xoutput_row2[0].discr_val == std::size_t(0));
      TS_ASSERT(xoutput_row2[1].discr_val == std::size_t(4));
      TS_ASSERT(xoutput_row2[2].cont_val == 4.5f);
      TS_ASSERT(xoutput_row2[3].discr_val == std::size_t(7));

      TS_ASSERT(generator.hasRows());
      const auto& output_row3 = generator.generate();
      const auto& xoutput_row3 = output_row3.row();
      TS_ASSERT(xoutput_row3[0].discr_val == std::size_t(0));
      TS_ASSERT(xoutput_row3[1].discr_val == std::size_t(4));
      TS_ASSERT(xoutput_row3[2].cont_val == 4.5f);
      TS_ASSERT(xoutput_row3[3].discr_val == std::size_t(7));

      TS_ASSERT(!generator.hasRows());
    }
  };


} /* namespace gum_tests */
