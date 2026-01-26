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

#include <agrum/base/database/DBRowGenerator4CompleteRows.h>

#include <ressources/include/simpleDebugGenerator.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DBRowGenerator4CompleteRows
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct DBRowGenerator4CompleteRowsTestSuite {
    public:
    static void test_identity() {
      const std::vector< gum::learning::DBTranslatedValueType > col_types{
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::CONTINUOUS,
          gum::learning::DBTranslatedValueType::DISCRETE};

      gum::learning::DBRowGenerator4CompleteRows generator(col_types);
      CHECK(!generator.hasRows());

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row1{
          gum::learning::DBTranslatedValue{std::size_t(0)},
          gum::learning::DBTranslatedValue{std::size_t(4)},
          gum::learning::DBTranslatedValue{4.5f},
          gum::learning::DBTranslatedValue{std::size_t(7)}};

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row2{
          gum::learning::DBTranslatedValue{std::size_t(0)},
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()},
          gum::learning::DBTranslatedValue{4.5f},
          gum::learning::DBTranslatedValue{std::size_t(7)}};

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row3{
          gum::learning::DBTranslatedValue{std::size_t(0)},
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()},
          gum::learning::DBTranslatedValue{std::numeric_limits< float >::max()},
          gum::learning::DBTranslatedValue{std::size_t(7)}};

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row4{
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()},
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()},
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

      const std::vector< std::size_t > cols_of_interest1{std::size_t(0), std::size_t(2)};
      const std::vector< std::size_t > cols_of_interest2{std::size_t(0), std::size_t(1)};

      generator.setColumnsOfInterest(cols_of_interest1);
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

      generator.setInputRow(input_row2);
      CHECK(generator.hasRows());
      const auto& output_row3b  = generator.generate();
      const auto& xoutput_row3b = output_row3b.row();
      CHECK((xoutput_row3b[0].discr_val) == (std::size_t(0)));
      CHECK((xoutput_row3b[2].cont_val) == (4.5f));

      generator.setInputRow(input_row3);
      CHECK(!generator.hasRows());

      generator.setInputRow(input_row4);
      CHECK(!generator.hasRows());

      generator.setColumnsOfInterest(cols_of_interest2);
      generator.setInputRow(input_row2);
      CHECK(!generator.hasRows());

      generator.setInputRow(input_row3);
      CHECK(!generator.hasRows());

      generator.setInputRow(input_row4);
      CHECK(!generator.hasRows());

      generator.setInputRow(input_row1);
      CHECK(generator.hasRows());
      generator.setColumnsOfInterest(cols_of_interest1);


      gum::learning::DBRowGenerator4CompleteRows generator2(generator);
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

      generator2.setInputRow(input_row1);
      CHECK(generator2.hasRows());

      generator2.setInputRow(input_row2);
      CHECK(generator2.hasRows());
      const auto& youtput_row3b  = generator2.generate();
      const auto& yxoutput_row3b = youtput_row3b.row();
      CHECK((yxoutput_row3b[0].discr_val) == (std::size_t(0)));
      CHECK((yxoutput_row3b[2].cont_val) == (4.5f));

      generator2.setInputRow(input_row3);
      CHECK(!generator2.hasRows());

      generator2.setInputRow(input_row4);
      CHECK(!generator2.hasRows());

      generator2.setColumnsOfInterest(cols_of_interest2);
      generator2.setInputRow(input_row2);
      CHECK(!generator2.hasRows());

      generator2.setInputRow(input_row3);
      CHECK(!generator2.hasRows());

      generator2.setInputRow(input_row4);
      CHECK(!generator2.hasRows());

      generator2.setInputRow(input_row1);
      CHECK(generator2.hasRows());
      generator2.setColumnsOfInterest(cols_of_interest1);


      gum::learning::DBRowGenerator4CompleteRows generator3(generator);
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

      generator3.setInputRow(input_row1);
      CHECK(generator3.hasRows());

      generator3.setInputRow(input_row2);
      CHECK(generator3.hasRows());
      const auto& aoutput_row3b  = generator3.generate();
      const auto& axoutput_row3b = aoutput_row3b.row();
      CHECK((axoutput_row3b[0].discr_val) == (std::size_t(0)));
      CHECK((axoutput_row3b[2].cont_val) == (4.5f));

      generator3.setInputRow(input_row3);
      CHECK(!generator3.hasRows());

      generator3.setInputRow(input_row4);
      CHECK(!generator3.hasRows());

      generator3.setColumnsOfInterest(cols_of_interest2);
      generator3.setInputRow(input_row2);
      CHECK(!generator3.hasRows());

      generator3.setInputRow(input_row3);
      CHECK(!generator3.hasRows());

      generator3.setInputRow(input_row4);
      CHECK(!generator3.hasRows());

      generator3.setInputRow(input_row1);
      CHECK(generator3.hasRows());
      generator3.setColumnsOfInterest(cols_of_interest1);


      gum::learning::DBRowGenerator4CompleteRows generator4(std::move(generator2));
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

      generator4.setInputRow(input_row2);
      CHECK(generator4.hasRows());
      const auto& boutput_row3b  = generator4.generate();
      const auto& bxoutput_row3b = boutput_row3b.row();
      CHECK((bxoutput_row3b[0].discr_val) == (std::size_t(0)));
      CHECK((bxoutput_row3b[2].cont_val) == (4.5f));

      generator4.setInputRow(input_row3);
      CHECK(!generator4.hasRows());

      generator4.setInputRow(input_row4);
      CHECK(!generator4.hasRows());

      generator4.setColumnsOfInterest(cols_of_interest2);
      generator4.setInputRow(input_row2);
      CHECK(!generator4.hasRows());

      generator4.setInputRow(input_row3);
      CHECK(!generator4.hasRows());

      generator4.setInputRow(input_row4);
      CHECK(!generator4.hasRows());

      generator4.setInputRow(input_row1);
      CHECK(generator4.hasRows());
      generator4.setColumnsOfInterest(cols_of_interest1);


      gum::learning::DBRowGenerator4CompleteRows generator5(std::move(generator3));
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

      generator.setInputRow(input_row2);
      CHECK(generator.hasRows());
      const auto& coutput_row3b  = generator.generate();
      const auto& cxoutput_row3b = coutput_row3b.row();
      CHECK((cxoutput_row3b[0].discr_val) == (std::size_t(0)));
      CHECK((cxoutput_row3b[2].cont_val) == (4.5f));

      generator5.setInputRow(input_row3);
      CHECK(!generator5.hasRows());

      generator5.setInputRow(input_row4);
      CHECK(!generator5.hasRows());

      generator5.setColumnsOfInterest(cols_of_interest2);
      generator5.setInputRow(input_row2);
      CHECK(!generator5.hasRows());

      generator5.setInputRow(input_row3);
      CHECK(!generator5.hasRows());

      generator5.setInputRow(input_row4);
      CHECK(!generator5.hasRows());

      generator5.setInputRow(input_row1);
      CHECK(generator5.hasRows());
      generator5.setColumnsOfInterest(cols_of_interest1);


      gum::learning::DBRowGenerator4CompleteRows* generator6 = generator4.clone();
      CHECK(generator6->hasRows());
      CHECK((generator6->columnsOfInterest().size()) == (std::size_t(2)));
      CHECK((generator6->columnsOfInterest()[0]) == (std::size_t(0)));
      CHECK((generator6->columnsOfInterest()[1]) == (std::size_t(2)));
      const auto& youtput_row6 = generator6->generate();
      const auto& zoutput_row6 = youtput_row6.row();
      CHECK((zoutput_row6[0].discr_val) == (std::size_t(0)));
      CHECK((zoutput_row6[2].cont_val) == (4.5f));
      CHECK(!generator6->hasRows());

      generator6->setInputRow(input_row2);
      CHECK(generator6->hasRows());
      const auto& doutput_row3b  = generator6->generate();
      const auto& dxoutput_row3b = doutput_row3b.row();
      CHECK((dxoutput_row3b[0].discr_val) == (std::size_t(0)));
      CHECK((dxoutput_row3b[2].cont_val) == (4.5f));

      generator6->setInputRow(input_row3);
      CHECK(!generator6->hasRows());

      generator6->setInputRow(input_row4);
      CHECK(!generator6->hasRows());

      generator6->setColumnsOfInterest(cols_of_interest2);
      generator6->setInputRow(input_row2);
      CHECK(!generator6->hasRows());

      generator6->setInputRow(input_row3);
      CHECK(!generator6->hasRows());

      generator6->setInputRow(input_row4);
      CHECK(!generator6->hasRows());
      delete generator6;


      gum::learning::DBRowGenerator4CompleteRows* generator7 = generator5.clone();
      CHECK(generator7->hasRows());
      CHECK((generator7->columnsOfInterest().size()) == (std::size_t(2)));
      CHECK((generator7->columnsOfInterest()[0]) == (std::size_t(0)));
      CHECK((generator7->columnsOfInterest()[1]) == (std::size_t(2)));
      const auto& youtput_row7 = generator7->generate();
      const auto& zoutput_row7 = youtput_row7.row();
      CHECK((zoutput_row7[0].discr_val) == (std::size_t(0)));
      CHECK((zoutput_row7[2].cont_val) == (4.5f));
      CHECK(!generator7->hasRows());

      generator7->setInputRow(input_row2);
      CHECK(generator7->hasRows());
      const auto& eoutput_row3b  = generator7->generate();
      const auto& exoutput_row3b = eoutput_row3b.row();
      CHECK((exoutput_row3b[0].discr_val) == (std::size_t(0)));
      CHECK((exoutput_row3b[2].cont_val) == (4.5f));

      generator7->setInputRow(input_row3);
      CHECK(!generator7->hasRows());

      generator7->setInputRow(input_row4);
      CHECK(!generator7->hasRows());

      generator7->setColumnsOfInterest(cols_of_interest2);
      generator7->setInputRow(input_row2);
      CHECK(!generator7->hasRows());

      generator7->setInputRow(input_row3);
      CHECK(!generator7->hasRows());

      generator7->setInputRow(input_row4);
      CHECK(!generator7->hasRows());
      delete generator7;


      gum::learning::DBRowGenerator4CompleteRows generator8(col_types);
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

      generator8.setInputRow(input_row2);
      CHECK(generator8.hasRows());
      const auto& foutput_row3b  = generator8.generate();
      const auto& fxoutput_row3b = foutput_row3b.row();
      CHECK((fxoutput_row3b[0].discr_val) == (std::size_t(0)));
      CHECK((fxoutput_row3b[2].cont_val) == (4.5f));

      generator8.setInputRow(input_row3);
      CHECK(!generator8.hasRows());

      generator8.setInputRow(input_row4);
      CHECK(!generator8.hasRows());

      generator8.setColumnsOfInterest(cols_of_interest2);
      generator8.setInputRow(input_row2);
      CHECK(!generator8.hasRows());

      generator8.setInputRow(input_row3);
      CHECK(!generator8.hasRows());

      generator8.setInputRow(input_row4);
      CHECK(!generator8.hasRows());


      gum::learning::DBRowGenerator4CompleteRows generator9(col_types);
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

      generator9.setInputRow(input_row2);
      CHECK(generator9.hasRows());
      const auto& goutput_row3b  = generator9.generate();
      const auto& gxoutput_row3b = goutput_row3b.row();
      CHECK((gxoutput_row3b[0].discr_val) == (std::size_t(0)));
      CHECK((gxoutput_row3b[2].cont_val) == (4.5f));

      generator9.setInputRow(input_row3);
      CHECK(!generator9.hasRows());

      generator9.setInputRow(input_row4);
      CHECK(!generator9.hasRows());

      generator9.setColumnsOfInterest(cols_of_interest2);
      generator9.setInputRow(input_row2);
      CHECK(!generator9.hasRows());

      generator9.setInputRow(input_row3);
      CHECK(!generator9.hasRows());

      generator9.setInputRow(input_row4);
      CHECK(!generator9.hasRows());
    }   // namespace gum_tests
  };

  GUM_TEST_ACTIF(_identity)


} /* namespace gum_tests */
