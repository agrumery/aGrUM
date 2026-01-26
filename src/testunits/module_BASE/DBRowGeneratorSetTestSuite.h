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
#include <agrum/base/database/DBRowGeneratorEM.h>
#include <agrum/base/database/DBRowGeneratorIdentity.h>
#include <agrum/base/database/DBRowGeneratorSet.h>
#include <agrum/BN/inference/lazyPropagation.h>

#include <ressources/include/evenDebugGenerator.h>
#include <ressources/include/simpleDebugGenerator.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DBRowGeneratorSet
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct DBRowGeneratorSetTestSuite {
    private:
    static gum::Tensor< double >
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
    static void test_simple() {
      const std::vector< gum::learning::DBTranslatedValueType > col_types{
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::CONTINUOUS,
          gum::learning::DBTranslatedValueType::DISCRETE};

      gum::learning::SimpleDebugGenerator generator1(col_types, 3);
      CHECK(!generator1.hasRows());
      gum::learning::SimpleDebugGenerator generator2(col_types, 4);
      CHECK(!generator2.hasRows());

      gum::learning::DBRowGeneratorSet genset;
      genset.insertGenerator(generator1);
      genset.insertGenerator(std::move(generator2));
      CHECK((genset.nbGenerators()) == (std::size_t(2)));
      CHECK((genset.size()) == (std::size_t(2)));
      CHECK(!genset.hasRows());

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

        CHECK((row[0].discr_val) == (std::size_t(0)));
        CHECK((row[1].discr_val) == (std::size_t(4)));
        CHECK((row[2].cont_val) == (4.5f));
        CHECK((row[3].discr_val) == (std::size_t(7)));
      }

      CHECK((nb_dup) == (std::size_t(12)));

      genset.reset();
      CHECK(!genset.hasRows());

      const std::vector< std::size_t > cols_of_interest{std::size_t(0),
                                                        std::size_t(2),
                                                        std::size_t(3)};
      genset.setColumnsOfInterest(cols_of_interest);
      genset.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset.hasRows()) {
        const auto& row = genset.generate().row();
        ++nb_dup;

        CHECK((row[0].discr_val) == (std::size_t(0)));
        CHECK((row[2].cont_val) == (4.5f));
        CHECK((row[3].discr_val) == (std::size_t(7)));
      }
      CHECK((nb_dup) == (std::size_t(12)));

      genset.setInputRow(input_row1);
      CHECK_THROWS_AS(genset.setColumnsOfInterest(cols_of_interest),
                      const gum::OperationNotAllowed&);
      genset.reset();

      genset.setColumnsOfInterest(std::move(cols_of_interest));
      genset.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset.hasRows()) {
        const auto& row = genset.generate().row();
        ++nb_dup;

        CHECK((row[0].discr_val) == (std::size_t(0)));
        CHECK((row[2].cont_val) == (4.5f));
        CHECK((row[3].discr_val) == (std::size_t(7)));
      }
      CHECK((nb_dup) == (std::size_t(12)));

      const auto& cols = genset.columnsOfInterest();
      CHECK((cols[0]) == (std::size_t(0)));
      CHECK((cols[1]) == (std::size_t(2)));
      CHECK((cols[2]) == (std::size_t(3)));


      gum::learning::DBRowGeneratorSet genset2(genset);
      genset2.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset2.hasRows()) {
        const auto& row = genset2.generate().row();
        ++nb_dup;

        CHECK((row[0].discr_val) == (std::size_t(0)));
        CHECK((row[2].cont_val) == (4.5f));
        CHECK((row[3].discr_val) == (std::size_t(7)));
      }
      CHECK((nb_dup) == (std::size_t(12)));
      const auto& cols2 = genset2.columnsOfInterest();
      CHECK((cols2[0]) == (std::size_t(0)));
      CHECK((cols2[1]) == (std::size_t(2)));
      CHECK((cols2[2]) == (std::size_t(3)));

      gum::learning::DBRowGeneratorSet genset3(std::move(genset2));
      genset3.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset3.hasRows()) {
        const auto& row = genset3.generate().row();
        ++nb_dup;

        CHECK((row[0].discr_val) == (std::size_t(0)));
        CHECK((row[2].cont_val) == (4.5f));
        CHECK((row[3].discr_val) == (std::size_t(7)));
      }
      CHECK((nb_dup) == (std::size_t(12)));
      const auto& cols3 = genset3.columnsOfInterest();
      CHECK((cols3[0]) == (std::size_t(0)));
      CHECK((cols3[1]) == (std::size_t(2)));
      CHECK((cols3[2]) == (std::size_t(3)));

      gum::learning::DBRowGeneratorSet* genset4 = genset3.clone();
      genset4->setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset4->hasRows()) {
        const auto& row = genset4->generate().row();
        ++nb_dup;

        CHECK((row[0].discr_val) == (std::size_t(0)));
        CHECK((row[2].cont_val) == (4.5f));
        CHECK((row[3].discr_val) == (std::size_t(7)));
      }
      CHECK((nb_dup) == (std::size_t(12)));
      const auto& cols4 = genset4->columnsOfInterest();
      CHECK((cols4[0]) == (std::size_t(0)));
      CHECK((cols4[1]) == (std::size_t(2)));
      CHECK((cols4[2]) == (std::size_t(3)));

      delete genset4;

      gum::learning::DBRowGeneratorSet genset5;
      CHECK((genset5.nbGenerators()) == (std::size_t(0)));
      genset5 = genset;
      CHECK((genset5.nbGenerators()) == (std::size_t(2)));
      genset5.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset5.hasRows()) {
        const auto& row = genset5.generate().row();
        ++nb_dup;

        CHECK((row[0].discr_val) == (std::size_t(0)));
        CHECK((row[2].cont_val) == (4.5f));
        CHECK((row[3].discr_val) == (std::size_t(7)));
      }
      CHECK((nb_dup) == (std::size_t(12)));
      const auto& cols5 = genset5.columnsOfInterest();
      CHECK((cols5[0]) == (std::size_t(0)));
      CHECK((cols5[1]) == (std::size_t(2)));
      CHECK((cols5[2]) == (std::size_t(3)));

      gum::learning::DBRowGeneratorSet genset6;
      CHECK((genset6.nbGenerators()) == (std::size_t(0)));
      genset6 = std::move(genset5);
      CHECK((genset6.nbGenerators()) == (std::size_t(2)));
      genset6.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset6.hasRows()) {
        const auto& row = genset6.generate().row();
        ++nb_dup;

        CHECK((row[0].discr_val) == (std::size_t(0)));
        CHECK((row[2].cont_val) == (4.5f));
        CHECK((row[3].discr_val) == (std::size_t(7)));
      }
      CHECK((nb_dup) == (std::size_t(12)));
      const auto& cols6 = genset6.columnsOfInterest();
      CHECK((cols6[0]) == (std::size_t(0)));
      CHECK((cols6[1]) == (std::size_t(2)));
      CHECK((cols6[2]) == (std::size_t(3)));

      auto& gen1 = dynamic_cast< gum::learning::SimpleDebugGenerator& >(genset6[0]);
      CHECK((gen1.columnsOfInterest().size()) == (std::size_t(3)));

      genset.setInputRow(input_row1);
      genset.clear();
      CHECK(!genset.hasRows());
    }

    static void test_simple2() {
      const std::vector< gum::learning::DBTranslatedValueType > col_types{
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::DISCRETE,
          gum::learning::DBTranslatedValueType::CONTINUOUS,
          gum::learning::DBTranslatedValueType::DISCRETE};

      gum::learning::EvenDebugGenerator generator1(col_types, 5);
      CHECK(!generator1.hasRows());
      gum::learning::EvenDebugGenerator generator2(col_types, 4);
      CHECK(!generator2.hasRows());

      gum::learning::DBRowGeneratorSet genset;
      genset.insertGenerator(generator1);
      genset.insertGenerator(std::move(generator2));
      CHECK((genset.nbGenerators()) == (std::size_t(2)));
      CHECK((genset.size()) == (std::size_t(2)));
      CHECK(!genset.hasRows());

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

        CHECK((row[0].discr_val) == (std::size_t(0)));
        CHECK((row[1].discr_val) == (std::size_t(4)));
        CHECK((row[2].cont_val) == (4.5f));
        CHECK((row[3].discr_val) == (std::size_t(7)));
      }

      CHECK((nb_dup) == (std::size_t(12)));

      genset.setInputRow(input_row1);
      CHECK(!genset.hasRows());

      genset.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset.hasRows()) {
        const auto& row = genset.generate().row();
        ++nb_dup;

        CHECK((row[0].discr_val) == (std::size_t(0)));
        CHECK((row[1].discr_val) == (std::size_t(4)));
        CHECK((row[2].cont_val) == (4.5f));
        CHECK((row[3].discr_val) == (std::size_t(7)));
      }
    }

    static void test_incomplete() {
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
      CHECK((genset.nbGenerators()) == (std::size_t(2)));
      CHECK((genset.size()) == (std::size_t(2)));
      CHECK(!genset.hasRows());

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

        CHECK((row[0].discr_val) == (std::size_t(0)));
        CHECK((row[1].discr_val) == (std::size_t(4)));
        CHECK((row[2].cont_val) == (4.5f));
        CHECK((row[3].discr_val) == (std::size_t(7)));
      }

      CHECK((nb_dup) == (std::size_t(1)));

      genset.reset();
      CHECK(!genset.hasRows());

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

        CHECK((row[0].discr_val) == (std::size_t(0)));
        CHECK((row[2].cont_val) == (4.5f));
      }

      CHECK((nb_dup) == (std::size_t(1)));
    }

    static void test_basic() {
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

      CHECK((&(generator2.getBayesNet())) == (&bn0));

      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D");
      bn.cpt("A").fillWith({0.3, 0.7});
      bn.cpt("B").fillWith({0.4, 0.6, 0.7, 0.3});
      bn.cpt("C").fillWith({0.2, 0.8, 0.3, 0.7, 0.4, 0.6, 0.5, 0.5});
      bn.cpt("D").fillWith({0.8, 0.2});

      genset.setBayesNet(bn);

      auto& genEM1 = dynamic_cast< gum::learning::DBRowGeneratorEM<>& >(genset[1]);
      CHECK((&(genEM1.getBayesNet())) == (&bn));

      auto& genEM3 = dynamic_cast< gum::learning::DBRowGeneratorEM<>& >(genset[3]);
      CHECK((&(genEM3.getBayesNet())) == (&bn));

      CHECK((genset.nbGenerators()) == (std::size_t(4)));
      CHECK((genset.size()) == (std::size_t(4)));
      CHECK(!genset.hasRows());


      genset.setInputRow(input_row1);
      CHECK(genset.hasRows());
      {
        const auto& row = genset.generate().row();
        CHECK((row[0].discr_val) == (std::size_t(0)));
        CHECK((row[1].discr_val) == (std::size_t(1)));
        CHECK((row[2].discr_val) == (std::size_t(1)));
        CHECK((row[3].discr_val) == (std::size_t(0)));
      }

      const std::vector< std::size_t > cols_of_interest1{std::size_t(0), std::size_t(3)};
      const std::vector< std::size_t > cols_of_interest2{std::size_t(0), std::size_t(1)};

      genset.setColumnsOfInterest(cols_of_interest1);
      {
        const auto& xcols = genset.columnsOfInterest();
        CHECK((xcols.size()) == (std::size_t(2)));
        CHECK((xcols[0]) == (std::size_t(0)));
        CHECK((xcols[1]) == (std::size_t(3)));
        genset.setInputRow(input_row1);
        CHECK(genset.hasRows());

        const auto& row = genset.generate().row();
        CHECK((row[0].discr_val) == (std::size_t(0)));
        CHECK((row[3].discr_val) == (std::size_t(0)));

        genset.setInputRow(input_row1);
        CHECK(genset.hasRows());

        genset.setInputRow(input_row2);
        CHECK(genset.hasRows());
        const auto& row2 = genset.generate().row();
        CHECK((row2[0].discr_val) == (std::size_t(0)));
        CHECK((row2[3].discr_val) == (std::size_t(0)));
      }

      genset.setColumnsOfInterest(cols_of_interest2);
      {
        gum::Tensor< double > proba = _infer_(bn, {std::size_t(1)}, input_row2);
        gum::Instantiation    inst(proba);

        genset.setInputRow(input_row2);
        CHECK(genset.hasRows());

        const auto& fill_row1  = genset.generate();
        const auto& xfill_row1 = fill_row1.row();
        CHECK((xfill_row1[0].discr_val) == (std::size_t(0)));
        CHECK((xfill_row1[1].discr_val) == (std::size_t(0)));
        CHECK((fill_row1.weight()) == (proba.get(inst)));

        ++inst;
        const auto& fill_row2  = genset.generate();
        const auto& xfill_row2 = fill_row2.row();
        CHECK((xfill_row2[0].discr_val) == (std::size_t(0)));
        CHECK((xfill_row2[1].discr_val) == (std::size_t(1)));
        CHECK((fill_row2.weight()) == (proba.get(inst)));
      }

      {
        const auto& xcols = genset.columnsOfInterest();
        CHECK((xcols.size()) == (std::size_t(2)));
        CHECK((xcols[0]) == (std::size_t(0)));
        CHECK((xcols[1]) == (std::size_t(1)));

        gum::Tensor< double > proba = _infer_(bn, {std::size_t(1)}, input_row3);
        gum::Instantiation    inst(proba);

        genset.setInputRow(input_row3);
        CHECK(genset.hasRows());

        const auto& fill_row1  = genset.generate();
        const auto& xfill_row1 = fill_row1.row();
        CHECK((xfill_row1[0].discr_val) == (std::size_t(0)));
        CHECK((xfill_row1[1].discr_val) == (std::size_t(0)));
        CHECK((fill_row1.weight()) == (proba.get(inst)));

        ++inst;
        const auto& fill_row2  = genset.generate();
        const auto& xfill_row2 = fill_row2.row();
        CHECK((xfill_row2[0].discr_val) == (std::size_t(0)));
        CHECK((xfill_row2[1].discr_val) == (std::size_t(1)));
        CHECK((fill_row2.weight()) == (proba.get(inst)));
      }

      {
        const auto& xcols = genset.columnsOfInterest();
        CHECK((xcols.size()) == (std::size_t(2)));
        CHECK((xcols[0]) == (std::size_t(0)));
        CHECK((xcols[1]) == (std::size_t(1)));

        gum::Tensor< double > proba = _infer_(bn, {std::size_t(1), std::size_t(0)}, input_row4);
        gum::Instantiation    inst(proba);

        genset.setInputRow(input_row4);
        CHECK(genset.hasRows());

        int nb = 0;
        while (genset.hasRows()) {
          const auto& fill_row1  = genset.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          CHECK((fill_row1.weight()) == doctest::Approx(proba.get(inst)).epsilon(0.001));
        }

        CHECK((nb) == (4));
      }

      {
        const auto& xcols = genset.columnsOfInterest();
        CHECK((xcols.size()) == static_cast< std::size_t >(2));
        CHECK((xcols[0]) == static_cast< std::size_t >(0));
        CHECK((xcols[1]) == static_cast< std::size_t >(1));

        gum::Tensor< double > proba
            = _infer_(bn,
                      {static_cast< std::size_t >(1), static_cast< std::size_t >(0)},
                      input_row5);
        gum::Instantiation inst(proba);

        genset.setInputRow(input_row5);
        CHECK(genset.hasRows());

        int nb = 0;
        while (genset.hasRows()) {
          const auto& fill_row1  = genset.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          CHECK((fill_row1.weight()) == doctest::Approx(proba.get(inst)).epsilon(0.001));
        }

        CHECK((nb) == (4));
      }

      {
        gum::Tensor< double > proba = _infer_(bn, {static_cast< std::size_t >(1)}, input_row2);
        gum::Instantiation    inst(proba);

        genset.setInputRow(input_row2);
        CHECK(genset.hasRows());

        const auto& fill_row1  = genset.generate();
        const auto& xfill_row1 = fill_row1.row();
        CHECK((xfill_row1[0].discr_val) == static_cast< std::size_t >(0));
        CHECK((xfill_row1[1].discr_val) == static_cast< std::size_t >(0));
        CHECK((fill_row1.weight()) == (proba.get(inst)));

        ++inst;
        const auto& fill_row2  = genset.generate();
        const auto& xfill_row2 = fill_row2.row();
        CHECK((xfill_row2[0].discr_val) == static_cast< std::size_t >(0));
        CHECK((xfill_row2[1].discr_val) == static_cast< std::size_t >(1));
        CHECK((fill_row2.weight()) == (proba.get(inst)));
      }
    }
  };

  GUM_TEST_ACTIF(_simple)
  GUM_TEST_ACTIF(_simple2)
  GUM_TEST_ACTIF(_incomplete)
  GUM_TEST_ACTIF(_basic)

} /* namespace gum_tests */
