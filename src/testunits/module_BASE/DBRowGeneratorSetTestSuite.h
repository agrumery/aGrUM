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
#include <agrum/base/database/DBRowGeneratorEM.h>
#include <agrum/base/database/DBRowGeneratorIdentity.h>
#include <agrum/base/database/DBRowGeneratorSet.h>
#include <agrum/BN/inference/lazyPropagation.h>

#include <ressources/include/evenDebugGenerator.h>
#include <ressources/include/simpleDebugGenerator.h>

namespace gum_tests {

  class GUM_TEST_SUITE(DBRowGeneratorSet) {
    private:
    gum::Tensor< double >
        _infer_(const gum::BayesNet< double >&                                  bn,
                const std::vector< std::size_t >&                               targets,
                const gum::learning::DBRow< gum::learning::DBTranslatedValue >& row) {
      gum::LazyPropagation< double > ve(&bn);

      gum::NodeSet target_set;
      for (auto target: targets)
        target_set.insert(gum::NodeId(target));
      ve.addJointTarget(target_set);

      const auto xrow     = row.row();
      const auto row_size = xrow.size();
      for (std::size_t col = std::size_t(0); col < row_size; ++col) {
        if (xrow[col].discr_val != std::numeric_limits< std::size_t >::max()) {
          ve.addEvidence(gum::NodeId(col), xrow[col].discr_val);
        }
      }

      gum::Tensor< double > prob = ve.jointPosterior(target_set);
      return prob;
    }   // namespace gum_tests


    public:
    GUM_ACTIVE_TEST(_simple) {
      const std::vector< gum::learning::DBTranslatedValueType > col_types{
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::CONTINUOUS,
          gum::learning::DBTranslatedValueType::DISCRETE};

      gum::learning::SimpleDebugGenerator generator1(col_types, 3);
      TS_ASSERT(!generator1.hasRows())
      gum::learning::SimpleDebugGenerator generator2(col_types, 4);
      TS_ASSERT(!generator2.hasRows())

      gum::learning::DBRowGeneratorSet genset;
      genset.insertGenerator(generator1);
      genset.insertGenerator(std::move(generator2));
      TS_ASSERT_EQUALS(genset.nbGenerators(), std::size_t(2))
      TS_ASSERT_EQUALS(genset.size(), std::size_t(2))
      TS_ASSERT(!genset.hasRows())

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row1{
          gum::learning::DBTranslatedValue{std::size_t(0)},
          gum::learning::DBTranslatedValue{std::size_t(4)},
          gum::learning::DBTranslatedValue{4.5f},
          gum::learning::DBTranslatedValue{std::size_t(7)}};

      genset.setInputRow(input_row1);
      std::size_t nb_dup = std::size_t(0);
      while (genset.hasRows()) {
        const auto& row = genset.generate().row();
        ++nb_dup;

        TS_ASSERT_EQUALS(row[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(row[1].discr_val, std::size_t(4))
        TS_ASSERT_EQUALS(row[2].cont_val, 4.5f)
        TS_ASSERT_EQUALS(row[3].discr_val, std::size_t(7))
      }

      TS_ASSERT_EQUALS(nb_dup, std::size_t(12))

      genset.reset();
      TS_ASSERT(!genset.hasRows())

      const std::vector< std::size_t > cols_of_interest{std::size_t(0),
                                                        std::size_t(2),
                                                        std::size_t(3)};
      genset.setColumnsOfInterest(cols_of_interest);
      genset.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset.hasRows()) {
        const auto& row = genset.generate().row();
        ++nb_dup;

        TS_ASSERT_EQUALS(row[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(row[2].cont_val, 4.5f)
        TS_ASSERT_EQUALS(row[3].discr_val, std::size_t(7))
      }
      TS_ASSERT_EQUALS(nb_dup, std::size_t(12))

      genset.setInputRow(input_row1);
      TS_ASSERT_THROWS(genset.setColumnsOfInterest(cols_of_interest),
                       const gum::OperationNotAllowed&)
      genset.reset();

      genset.setColumnsOfInterest(std::move(cols_of_interest));
      genset.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset.hasRows()) {
        const auto& row = genset.generate().row();
        ++nb_dup;

        TS_ASSERT_EQUALS(row[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(row[2].cont_val, 4.5f)
        TS_ASSERT_EQUALS(row[3].discr_val, std::size_t(7))
      }
      TS_ASSERT_EQUALS(nb_dup, std::size_t(12))

      const auto& cols = genset.columnsOfInterest();
      TS_ASSERT_EQUALS(cols[0], std::size_t(0))
      TS_ASSERT_EQUALS(cols[1], std::size_t(2))
      TS_ASSERT_EQUALS(cols[2], std::size_t(3))


      gum::learning::DBRowGeneratorSet genset2(genset);
      genset2.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset2.hasRows()) {
        const auto& row = genset2.generate().row();
        ++nb_dup;

        TS_ASSERT_EQUALS(row[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(row[2].cont_val, 4.5f)
        TS_ASSERT_EQUALS(row[3].discr_val, std::size_t(7))
      }
      TS_ASSERT_EQUALS(nb_dup, std::size_t(12))
      const auto& cols2 = genset2.columnsOfInterest();
      TS_ASSERT_EQUALS(cols2[0], std::size_t(0))
      TS_ASSERT_EQUALS(cols2[1], std::size_t(2))
      TS_ASSERT_EQUALS(cols2[2], std::size_t(3))

      gum::learning::DBRowGeneratorSet genset3(std::move(genset2));
      genset3.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset3.hasRows()) {
        const auto& row = genset3.generate().row();
        ++nb_dup;

        TS_ASSERT_EQUALS(row[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(row[2].cont_val, 4.5f)
        TS_ASSERT_EQUALS(row[3].discr_val, std::size_t(7))
      }
      TS_ASSERT_EQUALS(nb_dup, std::size_t(12))
      const auto& cols3 = genset3.columnsOfInterest();
      TS_ASSERT_EQUALS(cols3[0], std::size_t(0))
      TS_ASSERT_EQUALS(cols3[1], std::size_t(2))
      TS_ASSERT_EQUALS(cols3[2], std::size_t(3))

      gum::learning::DBRowGeneratorSet* genset4 = genset3.clone();
      genset4->setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset4->hasRows()) {
        const auto& row = genset4->generate().row();
        ++nb_dup;

        TS_ASSERT_EQUALS(row[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(row[2].cont_val, 4.5f)
        TS_ASSERT_EQUALS(row[3].discr_val, std::size_t(7))
      }
      TS_ASSERT_EQUALS(nb_dup, std::size_t(12))
      const auto& cols4 = genset4->columnsOfInterest();
      TS_ASSERT_EQUALS(cols4[0], std::size_t(0))
      TS_ASSERT_EQUALS(cols4[1], std::size_t(2))
      TS_ASSERT_EQUALS(cols4[2], std::size_t(3))

      delete genset4;

      gum::learning::DBRowGeneratorSet genset5;
      TS_ASSERT_EQUALS(genset5.nbGenerators(), std::size_t(0))
      genset5 = genset;
      TS_ASSERT_EQUALS(genset5.nbGenerators(), std::size_t(2))
      genset5.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset5.hasRows()) {
        const auto& row = genset5.generate().row();
        ++nb_dup;

        TS_ASSERT_EQUALS(row[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(row[2].cont_val, 4.5f)
        TS_ASSERT_EQUALS(row[3].discr_val, std::size_t(7))
      }
      TS_ASSERT_EQUALS(nb_dup, std::size_t(12))
      const auto& cols5 = genset5.columnsOfInterest();
      TS_ASSERT_EQUALS(cols5[0], std::size_t(0))
      TS_ASSERT_EQUALS(cols5[1], std::size_t(2))
      TS_ASSERT_EQUALS(cols5[2], std::size_t(3))

      gum::learning::DBRowGeneratorSet genset6;
      TS_ASSERT_EQUALS(genset6.nbGenerators(), std::size_t(0))
      genset6 = std::move(genset5);
      TS_ASSERT_EQUALS(genset6.nbGenerators(), std::size_t(2))
      genset6.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset6.hasRows()) {
        const auto& row = genset6.generate().row();
        ++nb_dup;

        TS_ASSERT_EQUALS(row[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(row[2].cont_val, 4.5f)
        TS_ASSERT_EQUALS(row[3].discr_val, std::size_t(7))
      }
      TS_ASSERT_EQUALS(nb_dup, std::size_t(12))
      const auto& cols6 = genset6.columnsOfInterest();
      TS_ASSERT_EQUALS(cols6[0], std::size_t(0))
      TS_ASSERT_EQUALS(cols6[1], std::size_t(2))
      TS_ASSERT_EQUALS(cols6[2], std::size_t(3))

      auto& gen1 = dynamic_cast< gum::learning::SimpleDebugGenerator& >(genset6[0]);
      TS_ASSERT_EQUALS(gen1.columnsOfInterest().size(), std::size_t(3))

      genset.setInputRow(input_row1);
      genset.clear();
      TS_ASSERT(!genset.hasRows())
    }

    GUM_ACTIVE_TEST(_simple2) {
      const std::vector< gum::learning::DBTranslatedValueType > col_types{
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::CONTINUOUS,
          gum::learning::DBTranslatedValueType::DISCRETE};

      gum::learning::EvenDebugGenerator generator1(col_types, 5);
      TS_ASSERT(!generator1.hasRows())
      gum::learning::EvenDebugGenerator generator2(col_types, 4);
      TS_ASSERT(!generator2.hasRows())

      gum::learning::DBRowGeneratorSet genset;
      genset.insertGenerator(generator1);
      genset.insertGenerator(std::move(generator2));
      TS_ASSERT_EQUALS(genset.nbGenerators(), std::size_t(2))
      TS_ASSERT_EQUALS(genset.size(), std::size_t(2))
      TS_ASSERT(!genset.hasRows())

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row1{
          gum::learning::DBTranslatedValue{std::size_t(0)},
          gum::learning::DBTranslatedValue{std::size_t(4)},
          gum::learning::DBTranslatedValue{4.5f},
          gum::learning::DBTranslatedValue{std::size_t(7)}};

      genset.setInputRow(input_row1);
      std::size_t nb_dup = std::size_t(0);
      while (genset.hasRows()) {
        const auto& row = genset.generate().row();
        ++nb_dup;

        TS_ASSERT_EQUALS(row[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(row[1].discr_val, std::size_t(4))
        TS_ASSERT_EQUALS(row[2].cont_val, 4.5f)
        TS_ASSERT_EQUALS(row[3].discr_val, std::size_t(7))
      }

      TS_ASSERT_EQUALS(nb_dup, std::size_t(12))

      genset.setInputRow(input_row1);
      TS_ASSERT(!genset.hasRows())

      genset.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset.hasRows()) {
        const auto& row = genset.generate().row();
        ++nb_dup;

        TS_ASSERT_EQUALS(row[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(row[1].discr_val, std::size_t(4))
        TS_ASSERT_EQUALS(row[2].cont_val, 4.5f)
        TS_ASSERT_EQUALS(row[3].discr_val, std::size_t(7))
      }
    }

    GUM_ACTIVE_TEST(_incomplete) {
      const std::vector< gum::learning::DBTranslatedValueType > col_types{
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::CONTINUOUS,
          gum::learning::DBTranslatedValueType::DISCRETE};

      gum::learning::DBRowGeneratorIdentity      generator1(col_types);
      gum::learning::DBRowGenerator4CompleteRows generator2(col_types);

      gum::learning::DBRowGeneratorSet genset;
      genset.insertGenerator(generator1);
      genset.insertGenerator(std::move(generator2));
      TS_ASSERT_EQUALS(genset.nbGenerators(), std::size_t(2))
      TS_ASSERT_EQUALS(genset.size(), std::size_t(2))
      TS_ASSERT(!genset.hasRows())

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row1{
          gum::learning::DBTranslatedValue{std::size_t(0)},
          gum::learning::DBTranslatedValue{std::size_t(4)},
          gum::learning::DBTranslatedValue{4.5f},
          gum::learning::DBTranslatedValue{std::size_t(7)}};

      genset.setInputRow(input_row1);
      std::size_t nb_dup = std::size_t(0);
      while (genset.hasRows()) {
        const auto& row = genset.generate().row();
        ++nb_dup;

        TS_ASSERT_EQUALS(row[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(row[1].discr_val, std::size_t(4))
        TS_ASSERT_EQUALS(row[2].cont_val, 4.5f)
        TS_ASSERT_EQUALS(row[3].discr_val, std::size_t(7))
      }

      TS_ASSERT_EQUALS(nb_dup, std::size_t(1))

      genset.reset();
      TS_ASSERT(!genset.hasRows())

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row2{
          gum::learning::DBTranslatedValue{std::size_t(0)},
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()},
          gum::learning::DBTranslatedValue{4.5f},
          gum::learning::DBTranslatedValue{std::size_t(7)}};
      const std::vector< std::size_t > cols_of_interest{std::size_t(0), std::size_t(2)};

      genset.setColumnsOfInterest(cols_of_interest);
      genset.setInputRow(input_row2);
      nb_dup = std::size_t(0);
      while (genset.hasRows()) {
        const auto& row = genset.generate().row();
        ++nb_dup;

        TS_ASSERT_EQUALS(row[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(row[2].cont_val, 4.5f)
      }

      TS_ASSERT_EQUALS(nb_dup, std::size_t(1))
    }

    GUM_ACTIVE_TEST(_basic) {
      const std::vector< gum::learning::DBTranslatedValueType > col_types{
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::DISCRETE};

      auto bn0 = gum::BayesNet< double >::fastPrototype("A;B;C;D");
      bn0.cpt("A").fillWith({0.3, 0.7});
      bn0.cpt("B").fillWith({0.3, 0.7});
      bn0.cpt("C").fillWith({0.3, 0.7});
      bn0.cpt("D").fillWith({0.3, 0.7});

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row1{
          gum::learning::DBTranslatedValue{std::size_t(0)},
          gum::learning::DBTranslatedValue{std::size_t(1)},
          gum::learning::DBTranslatedValue{std::size_t(1)},
          gum::learning::DBTranslatedValue{std::size_t(0)}};

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row2{
          gum::learning::DBTranslatedValue{std::size_t(0)},
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()},
          gum::learning::DBTranslatedValue{std::size_t(1)},
          gum::learning::DBTranslatedValue{std::size_t(0)}};

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row3{
          gum::learning::DBTranslatedValue{std::size_t(0)},
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()},
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()},
          gum::learning::DBTranslatedValue{std::size_t(0)}};

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row4{
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()},
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()},
          gum::learning::DBTranslatedValue{std::size_t(1)},
          gum::learning::DBTranslatedValue{std::size_t(0)}};

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row5{
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()},
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()},
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()},
          gum::learning::DBTranslatedValue{std::numeric_limits< std::size_t >::max()}};


      gum::learning::DBRowGeneratorIdentity generator1(col_types);
      gum::learning::DBRowGeneratorEM       generator2(col_types, bn0);
      gum::learning::DBRowGenerator&        gen2 = generator2;   // fix for g++-4.8
      gum::learning::DBRowGeneratorIdentity generator3(col_types);
      gum::learning::DBRowGeneratorEM       generator4(col_types, bn0);
      gum::learning::DBRowGenerator&        gen4 = generator4;   // fix for g++-4.8

      gum::learning::DBRowGeneratorSet genset;
      genset.insertGenerator(generator1);
      genset.insertGenerator(gen2);
      genset.insertGenerator(generator3);
      genset.insertGenerator(gen4);

      TS_ASSERT_EQUALS(&(generator2.getBayesNet()), &bn0)

      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D");
      bn.cpt("A").fillWith({0.3, 0.7});
      bn.cpt("B").fillWith({0.4, 0.6, 0.7, 0.3});
      bn.cpt("C").fillWith({0.2, 0.8, 0.3, 0.7, 0.4, 0.6, 0.5, 0.5});
      bn.cpt("D").fillWith({0.8, 0.2});

      genset.setBayesNet(bn);

      auto& genEM1 = dynamic_cast< gum::learning::DBRowGeneratorEM<>& >(genset[1]);
      TS_ASSERT_EQUALS(&(genEM1.getBayesNet()), &bn)

      auto& genEM3 = dynamic_cast< gum::learning::DBRowGeneratorEM<>& >(genset[3]);
      TS_ASSERT_EQUALS(&(genEM3.getBayesNet()), &bn)

      TS_ASSERT_EQUALS(genset.nbGenerators(), std::size_t(4))
      TS_ASSERT_EQUALS(genset.size(), std::size_t(4))
      TS_ASSERT(!genset.hasRows())


      genset.setInputRow(input_row1);
      TS_ASSERT(genset.hasRows())
      {
        const auto& row = genset.generate().row();
        TS_ASSERT_EQUALS(row[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(row[1].discr_val, std::size_t(1))
        TS_ASSERT_EQUALS(row[2].discr_val, std::size_t(1))
        TS_ASSERT_EQUALS(row[3].discr_val, std::size_t(0))
      }

      const std::vector< std::size_t > cols_of_interest1{std::size_t(0), std::size_t(3)};
      const std::vector< std::size_t > cols_of_interest2{std::size_t(0), std::size_t(1)};

      genset.setColumnsOfInterest(cols_of_interest1);
      {
        const auto& xcols = genset.columnsOfInterest();
        TS_ASSERT_EQUALS(xcols.size(), std::size_t(2))
        TS_ASSERT_EQUALS(xcols[0], std::size_t(0))
        TS_ASSERT_EQUALS(xcols[1], std::size_t(3))
        genset.setInputRow(input_row1);
        TS_ASSERT(genset.hasRows())

        const auto& row = genset.generate().row();
        TS_ASSERT_EQUALS(row[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(row[3].discr_val, std::size_t(0))

        genset.setInputRow(input_row1);
        TS_ASSERT(genset.hasRows())

        genset.setInputRow(input_row2);
        TS_ASSERT(genset.hasRows())
        const auto& row2 = genset.generate().row();
        TS_ASSERT_EQUALS(row2[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(row2[3].discr_val, std::size_t(0))
      }

      genset.setColumnsOfInterest(cols_of_interest2);
      {
        gum::Tensor< double > proba = _infer_(bn, {std::size_t(1)}, input_row2);
        gum::Instantiation    inst(proba);

        genset.setInputRow(input_row2);
        TS_ASSERT(genset.hasRows())

        const auto& fill_row1  = genset.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(fill_row1.weight(), proba.get(inst))

        ++inst;
        const auto& fill_row2  = genset.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT_EQUALS(xfill_row2[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(xfill_row2[1].discr_val, std::size_t(1))
        TS_ASSERT_EQUALS(fill_row2.weight(), proba.get(inst))
      }

      {
        const auto& xcols = genset.columnsOfInterest();
        TS_ASSERT_EQUALS(xcols.size(), std::size_t(2))
        TS_ASSERT_EQUALS(xcols[0], std::size_t(0))
        TS_ASSERT_EQUALS(xcols[1], std::size_t(1))

        gum::Tensor< double > proba = _infer_(bn, {std::size_t(1)}, input_row3);
        gum::Instantiation    inst(proba);

        genset.setInputRow(input_row3);
        TS_ASSERT(genset.hasRows())

        const auto& fill_row1  = genset.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(fill_row1.weight(), proba.get(inst))

        ++inst;
        const auto& fill_row2  = genset.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT_EQUALS(xfill_row2[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(xfill_row2[1].discr_val, std::size_t(1))
        TS_ASSERT_EQUALS(fill_row2.weight(), proba.get(inst))
      }

      {
        const auto& xcols = genset.columnsOfInterest();
        TS_ASSERT_EQUALS(xcols.size(), std::size_t(2))
        TS_ASSERT_EQUALS(xcols[0], std::size_t(0))
        TS_ASSERT_EQUALS(xcols[1], std::size_t(1))

        gum::Tensor< double > proba = _infer_(bn, {std::size_t(1), std::size_t(0)}, input_row4);
        gum::Instantiation    inst(proba);

        genset.setInputRow(input_row4);
        TS_ASSERT(genset.hasRows())

        int nb = 0;
        while (genset.hasRows()) {
          const auto& fill_row1  = genset.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001)
        }

        TS_ASSERT_EQUALS(nb, 4)
      }

      {
        const auto& xcols = genset.columnsOfInterest();
        TS_ASSERT_EQUALS(xcols.size(), std::size_t(2))
        TS_ASSERT_EQUALS(xcols[0], std::size_t(0))
        TS_ASSERT_EQUALS(xcols[1], std::size_t(1))

        gum::Tensor< double > proba = _infer_(bn, {std::size_t(1), std::size_t(0)}, input_row5);
        gum::Instantiation    inst(proba);

        genset.setInputRow(input_row5);
        TS_ASSERT(genset.hasRows())

        int nb = 0;
        while (genset.hasRows()) {
          const auto& fill_row1  = genset.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001)
        }

        TS_ASSERT_EQUALS(nb, 4)
      }

      {
        gum::Tensor< double > proba = _infer_(bn, {std::size_t(1)}, input_row2);
        gum::Instantiation    inst(proba);

        genset.setInputRow(input_row2);
        TS_ASSERT(genset.hasRows())

        const auto& fill_row1  = genset.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(fill_row1.weight(), proba.get(inst))

        ++inst;
        const auto& fill_row2  = genset.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT_EQUALS(xfill_row2[0].discr_val, std::size_t(0))
        TS_ASSERT_EQUALS(xfill_row2[1].discr_val, std::size_t(1))
        TS_ASSERT_EQUALS(fill_row2.weight(), proba.get(inst))
      }
    }
  };


} /* namespace gum_tests */
