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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/database/DBRowGenerator4CompleteRows.h>

#include <ressources/include/simpleDebugGenerator.h>

namespace gum_tests {

  class GUM_TEST_SUITE(DBRowGenerator4CompleteRows) {
    public:
    GUM_ACTIVE_TEST(_identity) {
      const std::vector< gum::learning::DBTranslatedValueType > col_types{
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::CONTINUOUS,
          gum::learning::DBTranslatedValueType::DISCRETE};

      gum::learning::DBRowGenerator4CompleteRows generator(col_types);
      TS_ASSERT(!generator.hasRows())

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
      TS_ASSERT(generator.hasRows())
      TS_ASSERT(generator.hasRows())

      const auto& output_row1  = generator.generate();
      const auto& xoutput_row1 = output_row1.row();
      TS_ASSERT_EQUALS(xoutput_row1[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(xoutput_row1[1].discr_val, std::size_t(4))
      TS_ASSERT_EQUALS(xoutput_row1[2].cont_val, 4.5f)
      TS_ASSERT_EQUALS(xoutput_row1[3].discr_val, std::size_t(7))
      TS_ASSERT(!generator.hasRows())

      generator.setInputRow(input_row1);
      TS_ASSERT(generator.hasRows())
      TS_ASSERT(generator.hasRows())

      const auto& output_row2  = generator.generate();
      const auto& xoutput_row2 = output_row2.row();
      TS_ASSERT_EQUALS(xoutput_row2[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(xoutput_row2[1].discr_val, std::size_t(4))
      TS_ASSERT_EQUALS(xoutput_row2[2].cont_val, 4.5f)
      TS_ASSERT_EQUALS(xoutput_row2[3].discr_val, std::size_t(7))
      TS_ASSERT(!generator.hasRows())

      const std::vector< std::size_t > cols_of_interest1{std::size_t(0), std::size_t(2)};
      const std::vector< std::size_t > cols_of_interest2{std::size_t(0), std::size_t(1)};

      generator.setColumnsOfInterest(cols_of_interest1);
      const auto& xcols = generator.columnsOfInterest();
      TS_ASSERT_EQUALS(xcols.size(), std::size_t(2))
      TS_ASSERT_EQUALS(xcols[0], std::size_t(0))
      TS_ASSERT_EQUALS(xcols[1], std::size_t(2))
      generator.setInputRow(input_row1);
      TS_ASSERT(generator.hasRows())
      const auto& output_row3  = generator.generate();
      const auto& xoutput_row3 = output_row3.row();
      TS_ASSERT_EQUALS(xoutput_row3[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(xoutput_row3[2].cont_val, 4.5f)

      generator.setInputRow(input_row1);
      TS_ASSERT(generator.hasRows())

      generator.setInputRow(input_row2);
      TS_ASSERT(generator.hasRows())
      const auto& output_row3b  = generator.generate();
      const auto& xoutput_row3b = output_row3b.row();
      TS_ASSERT_EQUALS(xoutput_row3b[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(xoutput_row3b[2].cont_val, 4.5f)

      generator.setInputRow(input_row3);
      TS_ASSERT(!generator.hasRows())

      generator.setInputRow(input_row4);
      TS_ASSERT(!generator.hasRows())

      generator.setColumnsOfInterest(cols_of_interest2);
      generator.setInputRow(input_row2);
      TS_ASSERT(!generator.hasRows())

      generator.setInputRow(input_row3);
      TS_ASSERT(!generator.hasRows())

      generator.setInputRow(input_row4);
      TS_ASSERT(!generator.hasRows())

      generator.setInputRow(input_row1);
      TS_ASSERT(generator.hasRows())
      generator.setColumnsOfInterest(cols_of_interest1);


      gum::learning::DBRowGenerator4CompleteRows generator2(generator);
      TS_ASSERT(generator2.hasRows())
      TS_ASSERT_EQUALS(generator2.columnsOfInterest().size(), std::size_t(2))
      TS_ASSERT_EQUALS(generator2.columnsOfInterest()[0], std::size_t(0))
      TS_ASSERT_EQUALS(generator2.columnsOfInterest()[1], std::size_t(2))
      const auto& youtput_row2 = generator2.generate();
      const auto& zoutput_row2 = youtput_row2.row();
      TS_ASSERT_EQUALS(zoutput_row2[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(zoutput_row2[2].cont_val, 4.5f)
      TS_ASSERT(!generator2.hasRows())
      generator2.setInputRow(input_row1);

      generator2.setInputRow(input_row1);
      TS_ASSERT(generator2.hasRows())

      generator2.setInputRow(input_row2);
      TS_ASSERT(generator2.hasRows())
      const auto& youtput_row3b  = generator2.generate();
      const auto& yxoutput_row3b = youtput_row3b.row();
      TS_ASSERT_EQUALS(yxoutput_row3b[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(yxoutput_row3b[2].cont_val, 4.5f)

      generator2.setInputRow(input_row3);
      TS_ASSERT(!generator2.hasRows())

      generator2.setInputRow(input_row4);
      TS_ASSERT(!generator2.hasRows())

      generator2.setColumnsOfInterest(cols_of_interest2);
      generator2.setInputRow(input_row2);
      TS_ASSERT(!generator2.hasRows())

      generator2.setInputRow(input_row3);
      TS_ASSERT(!generator2.hasRows())

      generator2.setInputRow(input_row4);
      TS_ASSERT(!generator2.hasRows())

      generator2.setInputRow(input_row1);
      TS_ASSERT(generator2.hasRows())
      generator2.setColumnsOfInterest(cols_of_interest1);


      gum::learning::DBRowGenerator4CompleteRows generator3(generator);
      TS_ASSERT(generator3.hasRows())
      TS_ASSERT_EQUALS(generator3.columnsOfInterest().size(), std::size_t(2))
      TS_ASSERT_EQUALS(generator3.columnsOfInterest()[0], std::size_t(0))
      TS_ASSERT_EQUALS(generator3.columnsOfInterest()[1], std::size_t(2))
      const auto& youtput_row3 = generator3.generate();
      const auto& zoutput_row3 = youtput_row3.row();
      TS_ASSERT_EQUALS(zoutput_row3[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(zoutput_row3[2].cont_val, 4.5f)
      TS_ASSERT(!generator3.hasRows())
      generator3.setInputRow(input_row1);

      generator3.setInputRow(input_row1);
      TS_ASSERT(generator3.hasRows())

      generator3.setInputRow(input_row2);
      TS_ASSERT(generator3.hasRows())
      const auto& aoutput_row3b  = generator3.generate();
      const auto& axoutput_row3b = aoutput_row3b.row();
      TS_ASSERT_EQUALS(axoutput_row3b[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(axoutput_row3b[2].cont_val, 4.5f)

      generator3.setInputRow(input_row3);
      TS_ASSERT(!generator3.hasRows())

      generator3.setInputRow(input_row4);
      TS_ASSERT(!generator3.hasRows())

      generator3.setColumnsOfInterest(cols_of_interest2);
      generator3.setInputRow(input_row2);
      TS_ASSERT(!generator3.hasRows())

      generator3.setInputRow(input_row3);
      TS_ASSERT(!generator3.hasRows())

      generator3.setInputRow(input_row4);
      TS_ASSERT(!generator3.hasRows())

      generator3.setInputRow(input_row1);
      TS_ASSERT(generator3.hasRows())
      generator3.setColumnsOfInterest(cols_of_interest1);


      gum::learning::DBRowGenerator4CompleteRows generator4(std::move(generator2));
      TS_ASSERT(generator4.hasRows())
      TS_ASSERT_EQUALS(generator4.columnsOfInterest().size(), std::size_t(2))
      TS_ASSERT_EQUALS(generator4.columnsOfInterest()[0], std::size_t(0))
      TS_ASSERT_EQUALS(generator4.columnsOfInterest()[1], std::size_t(2))
      const auto& youtput_row4 = generator4.generate();
      const auto& zoutput_row4 = youtput_row4.row();
      TS_ASSERT_EQUALS(zoutput_row4[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(zoutput_row4[2].cont_val, 4.5f)
      TS_ASSERT(!generator4.hasRows())
      generator4.setInputRow(input_row1);

      generator4.setInputRow(input_row2);
      TS_ASSERT(generator4.hasRows())
      const auto& boutput_row3b  = generator4.generate();
      const auto& bxoutput_row3b = boutput_row3b.row();
      TS_ASSERT_EQUALS(bxoutput_row3b[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(bxoutput_row3b[2].cont_val, 4.5f)

      generator4.setInputRow(input_row3);
      TS_ASSERT(!generator4.hasRows())

      generator4.setInputRow(input_row4);
      TS_ASSERT(!generator4.hasRows())

      generator4.setColumnsOfInterest(cols_of_interest2);
      generator4.setInputRow(input_row2);
      TS_ASSERT(!generator4.hasRows())

      generator4.setInputRow(input_row3);
      TS_ASSERT(!generator4.hasRows())

      generator4.setInputRow(input_row4);
      TS_ASSERT(!generator4.hasRows())

      generator4.setInputRow(input_row1);
      TS_ASSERT(generator4.hasRows())
      generator4.setColumnsOfInterest(cols_of_interest1);


      gum::learning::DBRowGenerator4CompleteRows generator5(std::move(generator3));
      TS_ASSERT(generator5.hasRows())
      TS_ASSERT_EQUALS(generator5.columnsOfInterest().size(), std::size_t(2))
      TS_ASSERT_EQUALS(generator5.columnsOfInterest()[0], std::size_t(0))
      TS_ASSERT_EQUALS(generator5.columnsOfInterest()[1], std::size_t(2))
      const auto& youtput_row5 = generator5.generate();
      const auto& zoutput_row5 = youtput_row5.row();
      TS_ASSERT_EQUALS(zoutput_row5[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(zoutput_row5[2].cont_val, 4.5f)
      TS_ASSERT(!generator5.hasRows())
      generator5.setInputRow(input_row1);

      generator.setInputRow(input_row2);
      TS_ASSERT(generator.hasRows())
      const auto& coutput_row3b  = generator.generate();
      const auto& cxoutput_row3b = coutput_row3b.row();
      TS_ASSERT_EQUALS(cxoutput_row3b[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(cxoutput_row3b[2].cont_val, 4.5f)

      generator5.setInputRow(input_row3);
      TS_ASSERT(!generator5.hasRows())

      generator5.setInputRow(input_row4);
      TS_ASSERT(!generator5.hasRows())

      generator5.setColumnsOfInterest(cols_of_interest2);
      generator5.setInputRow(input_row2);
      TS_ASSERT(!generator5.hasRows())

      generator5.setInputRow(input_row3);
      TS_ASSERT(!generator5.hasRows())

      generator5.setInputRow(input_row4);
      TS_ASSERT(!generator5.hasRows())

      generator5.setInputRow(input_row1);
      TS_ASSERT(generator5.hasRows())
      generator5.setColumnsOfInterest(cols_of_interest1);


      gum::learning::DBRowGenerator4CompleteRows* generator6 = generator4.clone();
      TS_ASSERT(generator6->hasRows())
      TS_ASSERT_EQUALS(generator6->columnsOfInterest().size(), std::size_t(2))
      TS_ASSERT_EQUALS(generator6->columnsOfInterest()[0], std::size_t(0))
      TS_ASSERT_EQUALS(generator6->columnsOfInterest()[1], std::size_t(2))
      const auto& youtput_row6 = generator6->generate();
      const auto& zoutput_row6 = youtput_row6.row();
      TS_ASSERT_EQUALS(zoutput_row6[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(zoutput_row6[2].cont_val, 4.5f)
      TS_ASSERT(!generator6->hasRows())

      generator6->setInputRow(input_row2);
      TS_ASSERT(generator6->hasRows())
      const auto& doutput_row3b  = generator6->generate();
      const auto& dxoutput_row3b = doutput_row3b.row();
      TS_ASSERT_EQUALS(dxoutput_row3b[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(dxoutput_row3b[2].cont_val, 4.5f)

      generator6->setInputRow(input_row3);
      TS_ASSERT(!generator6->hasRows())

      generator6->setInputRow(input_row4);
      TS_ASSERT(!generator6->hasRows())

      generator6->setColumnsOfInterest(cols_of_interest2);
      generator6->setInputRow(input_row2);
      TS_ASSERT(!generator6->hasRows())

      generator6->setInputRow(input_row3);
      TS_ASSERT(!generator6->hasRows())

      generator6->setInputRow(input_row4);
      TS_ASSERT(!generator6->hasRows())
      delete generator6;


      gum::learning::DBRowGenerator4CompleteRows* generator7 = generator5.clone();
      TS_ASSERT(generator7->hasRows())
      TS_ASSERT_EQUALS(generator7->columnsOfInterest().size(), std::size_t(2))
      TS_ASSERT_EQUALS(generator7->columnsOfInterest()[0], std::size_t(0))
      TS_ASSERT_EQUALS(generator7->columnsOfInterest()[1], std::size_t(2))
      const auto& youtput_row7 = generator7->generate();
      const auto& zoutput_row7 = youtput_row7.row();
      TS_ASSERT_EQUALS(zoutput_row7[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(zoutput_row7[2].cont_val, 4.5f)
      TS_ASSERT(!generator7->hasRows())

      generator7->setInputRow(input_row2);
      TS_ASSERT(generator7->hasRows())
      const auto& eoutput_row3b  = generator7->generate();
      const auto& exoutput_row3b = eoutput_row3b.row();
      TS_ASSERT_EQUALS(exoutput_row3b[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(exoutput_row3b[2].cont_val, 4.5f)

      generator7->setInputRow(input_row3);
      TS_ASSERT(!generator7->hasRows())

      generator7->setInputRow(input_row4);
      TS_ASSERT(!generator7->hasRows())

      generator7->setColumnsOfInterest(cols_of_interest2);
      generator7->setInputRow(input_row2);
      TS_ASSERT(!generator7->hasRows())

      generator7->setInputRow(input_row3);
      TS_ASSERT(!generator7->hasRows())

      generator7->setInputRow(input_row4);
      TS_ASSERT(!generator7->hasRows())
      delete generator7;


      gum::learning::DBRowGenerator4CompleteRows generator8(col_types);
      TS_ASSERT(!generator8.hasRows())
      TS_ASSERT_EQUALS(generator8.columnsOfInterest().size(), std::size_t(0))

      generator.setInputRow(input_row1);
      TS_ASSERT(!generator8.hasRows())
      TS_ASSERT_EQUALS(generator8.columnsOfInterest().size(), std::size_t(0))
      generator8 = generator;
      TS_ASSERT(generator8.hasRows())
      TS_ASSERT_EQUALS(generator8.columnsOfInterest().size(), std::size_t(2))
      TS_ASSERT_EQUALS(generator8.columnsOfInterest()[0], std::size_t(0))
      TS_ASSERT_EQUALS(generator8.columnsOfInterest()[1], std::size_t(2))
      const auto& youtput_row8 = generator8.generate();
      const auto& zoutput_row8 = youtput_row8.row();
      TS_ASSERT_EQUALS(zoutput_row8[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(zoutput_row8[2].cont_val, 4.5f)
      TS_ASSERT(!generator8.hasRows())

      generator8.setInputRow(input_row2);
      TS_ASSERT(generator8.hasRows())
      const auto& foutput_row3b  = generator8.generate();
      const auto& fxoutput_row3b = foutput_row3b.row();
      TS_ASSERT_EQUALS(fxoutput_row3b[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(fxoutput_row3b[2].cont_val, 4.5f)

      generator8.setInputRow(input_row3);
      TS_ASSERT(!generator8.hasRows())

      generator8.setInputRow(input_row4);
      TS_ASSERT(!generator8.hasRows())

      generator8.setColumnsOfInterest(cols_of_interest2);
      generator8.setInputRow(input_row2);
      TS_ASSERT(!generator8.hasRows())

      generator8.setInputRow(input_row3);
      TS_ASSERT(!generator8.hasRows())

      generator8.setInputRow(input_row4);
      TS_ASSERT(!generator8.hasRows())


      gum::learning::DBRowGenerator4CompleteRows generator9(col_types);
      TS_ASSERT(!generator9.hasRows())
      TS_ASSERT_EQUALS(generator9.columnsOfInterest().size(), std::size_t(0))

      generator.setInputRow(input_row1);
      TS_ASSERT(!generator9.hasRows())
      TS_ASSERT_EQUALS(generator9.columnsOfInterest().size(), std::size_t(0))
      generator9 = std::move(generator);
      TS_ASSERT(generator9.hasRows())
      TS_ASSERT_EQUALS(generator9.columnsOfInterest().size(), std::size_t(2))
      TS_ASSERT_EQUALS(generator9.columnsOfInterest()[0], std::size_t(0))
      TS_ASSERT_EQUALS(generator9.columnsOfInterest()[1], std::size_t(2))
      const auto& youtput_row9 = generator9.generate();
      const auto& zoutput_row9 = youtput_row9.row();
      TS_ASSERT_EQUALS(zoutput_row9[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(zoutput_row9[2].cont_val, 4.5f)
      TS_ASSERT(!generator9.hasRows())

      generator9.setInputRow(input_row2);
      TS_ASSERT(generator9.hasRows())
      const auto& goutput_row3b  = generator9.generate();
      const auto& gxoutput_row3b = goutput_row3b.row();
      TS_ASSERT_EQUALS(gxoutput_row3b[0].discr_val, std::size_t(0))
      TS_ASSERT_EQUALS(gxoutput_row3b[2].cont_val, 4.5f)

      generator9.setInputRow(input_row3);
      TS_ASSERT(!generator9.hasRows())

      generator9.setInputRow(input_row4);
      TS_ASSERT(!generator9.hasRows())

      generator9.setColumnsOfInterest(cols_of_interest2);
      generator9.setInputRow(input_row2);
      TS_ASSERT(!generator9.hasRows())

      generator9.setInputRow(input_row3);
      TS_ASSERT(!generator9.hasRows())

      generator9.setInputRow(input_row4);
      TS_ASSERT(!generator9.hasRows())
    }   // namespace gum_tests
  };


} /* namespace gum_tests */
