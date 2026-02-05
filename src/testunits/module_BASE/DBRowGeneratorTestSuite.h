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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/database/DBRowGeneratorIdentity.h>

#include <ressources/include/simpleDebugGenerator.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DBRowGenerator
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct DBRowGeneratorTestSuite {
    public:
    static void test_identity() {
      const std::vector< gum::learning::DBTranslatedValueType > col_types{
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::CONTINUOUS,
          gum::learning::DBTranslatedValueType::DISCRETE};

      gum::learning::DBRowGeneratorIdentity generator(col_types);
      CHECK(!generator.hasRows());

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row1{
          gum::learning::DBTranslatedValue{std::size_t(0)},
          gum::learning::DBTranslatedValue{std::size_t(4)},
          gum::learning::DBTranslatedValue{4.5f},
          gum::learning::DBTranslatedValue{std::size_t(7)}};
      generator.setInputRow(input_row1);
      CHECK(generator.hasRows());
      CHECK(generator.hasRows());

      const auto& output_row1  = generator.generate();
      const auto& xoutput_row1 = output_row1.row();
      CHECK((xoutput_row1[0].discr_val) == (std::size_t(0)));
      CHECK((xoutput_row1[1].discr_val) == (std::size_t(4)));
      CHECK((xoutput_row1[2].cont_val) == (4.5f));
      CHECK((xoutput_row1[3].discr_val) == (std::size_t(7)));
      CHECK(!generator.hasRows());

      generator.setInputRow(input_row1);
      CHECK(generator.hasRows());
      CHECK(generator.hasRows());

      const auto& output_row2  = generator.generate();
      const auto& xoutput_row2 = output_row2.row();
      CHECK((xoutput_row2[0].discr_val) == (std::size_t(0)));
      CHECK((xoutput_row2[1].discr_val) == (std::size_t(4)));
      CHECK((xoutput_row2[2].cont_val) == (4.5f));
      CHECK((xoutput_row2[3].discr_val) == (std::size_t(7)));
      CHECK(!generator.hasRows());

      const std::vector< std::size_t > cols_of_interest{std::size_t(0), std::size_t(2)};
      generator.setColumnsOfInterest(cols_of_interest);
      const auto& xcols = generator.columnsOfInterest();
      CHECK((xcols.size()) == (std::size_t(2)));
      CHECK((xcols[0]) == (std::size_t(0)));
      CHECK((xcols[1]) == (std::size_t(2)));
      generator.setInputRow(input_row1);
      CHECK(generator.hasRows());
      const auto& output_row3  = generator.generate();
      const auto& xoutput_row3 = output_row3.row();
      CHECK((xoutput_row3[0].discr_val) == (std::size_t(0)));
      CHECK((xoutput_row3[2].cont_val) == (4.5f));

      generator.setInputRow(input_row1);
      CHECK(generator.hasRows());

      gum::learning::DBRowGeneratorIdentity generator2(generator);
      CHECK(generator2.hasRows());
      CHECK((generator2.columnsOfInterest().size()) == (std::size_t(2)));
      CHECK((generator2.columnsOfInterest()[0]) == (std::size_t(0)));
      CHECK((generator2.columnsOfInterest()[1]) == (std::size_t(2)));
      const auto& youtput_row2 = generator2.generate();
      const auto& zoutput_row2 = youtput_row2.row();
      CHECK((zoutput_row2[0].discr_val) == (std::size_t(0)));
      CHECK((zoutput_row2[2].cont_val) == (4.5f));
      CHECK(!generator2.hasRows());
      generator2.setInputRow(input_row1);

      gum::learning::DBRowGeneratorIdentity generator3(generator);
      CHECK(generator3.hasRows());
      CHECK((generator3.columnsOfInterest().size()) == (std::size_t(2)));
      CHECK((generator3.columnsOfInterest()[0]) == (std::size_t(0)));
      CHECK((generator3.columnsOfInterest()[1]) == (std::size_t(2)));
      const auto& youtput_row3 = generator3.generate();
      const auto& zoutput_row3 = youtput_row3.row();
      CHECK((zoutput_row3[0].discr_val) == (std::size_t(0)));
      CHECK((zoutput_row3[2].cont_val) == (4.5f));
      CHECK(!generator3.hasRows());
      generator3.setInputRow(input_row1);

      gum::learning::DBRowGeneratorIdentity generator4(std::move(generator2));
      CHECK(generator4.hasRows());
      CHECK((generator4.columnsOfInterest().size()) == (std::size_t(2)));
      CHECK((generator4.columnsOfInterest()[0]) == (std::size_t(0)));
      CHECK((generator4.columnsOfInterest()[1]) == (std::size_t(2)));
      const auto& youtput_row4 = generator4.generate();
      const auto& zoutput_row4 = youtput_row4.row();
      CHECK((zoutput_row4[0].discr_val) == (std::size_t(0)));
      CHECK((zoutput_row4[2].cont_val) == (4.5f));
      CHECK(!generator4.hasRows());
      generator4.setInputRow(input_row1);

      gum::learning::DBRowGeneratorIdentity generator5(std::move(generator3));
      CHECK(generator5.hasRows());
      CHECK((generator5.columnsOfInterest().size()) == (std::size_t(2)));
      CHECK((generator5.columnsOfInterest()[0]) == (std::size_t(0)));
      CHECK((generator5.columnsOfInterest()[1]) == (std::size_t(2)));
      const auto& youtput_row5 = generator5.generate();
      const auto& zoutput_row5 = youtput_row5.row();
      CHECK((zoutput_row5[0].discr_val) == (std::size_t(0)));
      CHECK((zoutput_row5[2].cont_val) == (4.5f));
      CHECK(!generator5.hasRows());
      generator5.setInputRow(input_row1);

      gum::learning::DBRowGeneratorIdentity* generator6 = generator4.clone();
      CHECK(generator6->hasRows());
      CHECK((generator6->columnsOfInterest().size()) == (std::size_t(2)));
      CHECK((generator6->columnsOfInterest()[0]) == (std::size_t(0)));
      CHECK((generator6->columnsOfInterest()[1]) == (std::size_t(2)));
      const auto& youtput_row6 = generator6->generate();
      const auto& zoutput_row6 = youtput_row6.row();
      CHECK((zoutput_row6[0].discr_val) == (std::size_t(0)));
      CHECK((zoutput_row6[2].cont_val) == (4.5f));
      CHECK(!generator6->hasRows());
      delete generator6;

      gum::learning::DBRowGeneratorIdentity* generator7 = generator5.clone();
      CHECK(generator7->hasRows());
      CHECK((generator7->columnsOfInterest().size()) == (std::size_t(2)));
      CHECK((generator7->columnsOfInterest()[0]) == (std::size_t(0)));
      CHECK((generator7->columnsOfInterest()[1]) == (std::size_t(2)));
      const auto& youtput_row7 = generator7->generate();
      const auto& zoutput_row7 = youtput_row7.row();
      CHECK((zoutput_row7[0].discr_val) == (std::size_t(0)));
      CHECK((zoutput_row7[2].cont_val) == (4.5f));
      CHECK(!generator7->hasRows());
      delete generator7;

      gum::learning::DBRowGeneratorIdentity generator8(col_types);
      CHECK(!generator8.hasRows());
      CHECK((generator8.columnsOfInterest().size()) == (std::size_t(0)));

      generator.setInputRow(input_row1);
      CHECK(!generator8.hasRows());
      CHECK((generator8.columnsOfInterest().size()) == (std::size_t(0)));
      generator8 = generator;
      CHECK(generator8.hasRows());
      CHECK((generator8.columnsOfInterest().size()) == (std::size_t(2)));
      CHECK((generator8.columnsOfInterest()[0]) == (std::size_t(0)));
      CHECK((generator8.columnsOfInterest()[1]) == (std::size_t(2)));
      const auto& youtput_row8 = generator8.generate();
      const auto& zoutput_row8 = youtput_row8.row();
      CHECK((zoutput_row8[0].discr_val) == (std::size_t(0)));
      CHECK((zoutput_row8[2].cont_val) == (4.5f));
      CHECK(!generator8.hasRows());

      gum::learning::DBRowGeneratorIdentity generator9(col_types);
      CHECK(!generator9.hasRows());
      CHECK((generator9.columnsOfInterest().size()) == (std::size_t(0)));

      generator.setInputRow(input_row1);
      CHECK(!generator9.hasRows());
      CHECK((generator9.columnsOfInterest().size()) == (std::size_t(0)));
      generator9 = std::move(generator);
      CHECK(generator9.hasRows());
      CHECK((generator9.columnsOfInterest().size()) == (std::size_t(2)));
      CHECK((generator9.columnsOfInterest()[0]) == (std::size_t(0)));
      CHECK((generator9.columnsOfInterest()[1]) == (std::size_t(2)));
      const auto& youtput_row9 = generator9.generate();
      const auto& zoutput_row9 = youtput_row9.row();
      CHECK((zoutput_row9[0].discr_val) == (std::size_t(0)));
      CHECK((zoutput_row9[2].cont_val) == (4.5f));
      CHECK(!generator9.hasRows());
    }   // namespace gum_tests

    static void test_duplications() {
      const std::vector< gum::learning::DBTranslatedValueType > col_types{
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::CONTINUOUS,
          gum::learning::DBTranslatedValueType::DISCRETE};

      gum::learning::SimpleDebugGenerator generator(col_types, 3);
      CHECK(!generator.hasRows());

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row1{
          gum::learning::DBTranslatedValue{std::size_t(0)},
          gum::learning::DBTranslatedValue{std::size_t(4)},
          gum::learning::DBTranslatedValue{4.5f},
          gum::learning::DBTranslatedValue{std::size_t(7)}};
      generator.setInputRow(input_row1);
      CHECK(generator.hasRows());

      CHECK(generator.hasRows());
      const auto& output_row1  = generator.generate();
      const auto& xoutput_row1 = output_row1.row();
      CHECK((xoutput_row1[0].discr_val) == (std::size_t(0)));
      CHECK((xoutput_row1[1].discr_val) == (std::size_t(4)));
      CHECK((xoutput_row1[2].cont_val) == (4.5f));
      CHECK((xoutput_row1[3].discr_val) == (std::size_t(7)));

      CHECK(generator.hasRows());
      const auto& output_row2  = generator.generate();
      const auto& xoutput_row2 = output_row2.row();
      CHECK((xoutput_row2[0].discr_val) == (std::size_t(0)));
      CHECK((xoutput_row2[1].discr_val) == (std::size_t(4)));
      CHECK((xoutput_row2[2].cont_val) == (4.5f));
      CHECK((xoutput_row2[3].discr_val) == (std::size_t(7)));

      CHECK(generator.hasRows());
      const auto& output_row3  = generator.generate();
      const auto& xoutput_row3 = output_row3.row();
      CHECK((xoutput_row3[0].discr_val) == (std::size_t(0)));
      CHECK((xoutput_row3[1].discr_val) == (std::size_t(4)));
      CHECK((xoutput_row3[2].cont_val) == (4.5f));
      CHECK((xoutput_row3[3].discr_val) == (std::size_t(7)));

      CHECK(!generator.hasRows());
    }
  };

  GUM_TEST_ACTIF(_identity)
  GUM_TEST_ACTIF(_duplications)

} /* namespace gum_tests */
