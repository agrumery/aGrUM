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
#include <ressources/mygenerator.h>
#include <ressources/mygenerator2.h>
#include <iostream>

#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/learning/database/DBRowGeneratorIdentity.h>
#include <agrum/learning/database/DBRowGenerator4CompleteRows.h>
#include <agrum/learning/database/DBRowGeneratorSet.h>
#include <agrum/learning/database/DBRowGeneratorEM.h>

namespace gum_tests {

  class DBRowGeneratorSetTestSuite : public CxxTest::TestSuite {
    private:
    gum::Potential< double > __infer(
      const gum::BayesNet< double >&                                  bn,
      const std::vector< std::size_t >&                               targets,
      const gum::learning::DBRow< gum::learning::DBTranslatedValue >& row) {
      gum::LazyPropagation< double > ve(&bn);

      gum::NodeSet target_set;
      for (auto target : targets)
        target_set.insert(gum::NodeId(target));
      ve.addJointTarget(target_set);

      const auto xrow = row.row();
      const auto row_size = xrow.size();
      for (std::size_t col = std::size_t(0); col < row_size; ++col) {
        if (xrow[col].discr_val != std::numeric_limits< std::size_t >::max()) {
          ve.addEvidence(gum::NodeId(col), xrow[col].discr_val);
        }
      }

      gum::Potential< double > prob = ve.jointPosterior(target_set);
      return prob;
    }


    public:
    void test_simple() {
      const std::vector< gum::learning::DBTranslatedValueType > col_types{
        gum::learning::DBTranslatedValueType::DISCRETE,
        gum::learning::DBTranslatedValueType::DISCRETE,
        gum::learning::DBTranslatedValueType::CONTINUOUS,
        gum::learning::DBTranslatedValueType::DISCRETE};

      gum::learning::MyGenerator<> generator1(col_types, 3);
      TS_ASSERT(!generator1.hasRows());
      gum::learning::MyGenerator<> generator2(col_types, 4);
      TS_ASSERT(!generator2.hasRows());

      gum::learning::DBRowGeneratorSet<> genset;
      genset.insertGenerator(generator1);
      genset.insertGenerator(std::move(generator2));
      TS_ASSERT(genset.nbGenerators() == std::size_t(2));
      TS_ASSERT(genset.size() == std::size_t(2));
      TS_ASSERT(!genset.hasRows());

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

        TS_ASSERT(row[0].discr_val == std::size_t(0));
        TS_ASSERT(row[1].discr_val == std::size_t(4));
        TS_ASSERT(row[2].cont_val == 4.5f);
        TS_ASSERT(row[3].discr_val == std::size_t(7));
      }

      TS_ASSERT(nb_dup == std::size_t(12));

      genset.reset();
      TS_ASSERT(!genset.hasRows());

      const std::vector< std::size_t > cols_of_interest{
        std::size_t(0), std::size_t(2), std::size_t(3)};
      genset.setColumnsOfInterest(cols_of_interest);
      genset.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset.hasRows()) {
        const auto& row = genset.generate().row();
        ++nb_dup;

        TS_ASSERT(row[0].discr_val == std::size_t(0));
        TS_ASSERT(row[2].cont_val == 4.5f);
        TS_ASSERT(row[3].discr_val == std::size_t(7));
      }
      TS_ASSERT(nb_dup == std::size_t(12));

      genset.setInputRow(input_row1);
      TS_ASSERT_THROWS(genset.setColumnsOfInterest(cols_of_interest),
                       gum::OperationNotAllowed);
      genset.reset();

      genset.setColumnsOfInterest(std::move(cols_of_interest));
      genset.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset.hasRows()) {
        const auto& row = genset.generate().row();
        ++nb_dup;

        TS_ASSERT(row[0].discr_val == std::size_t(0));
        TS_ASSERT(row[2].cont_val == 4.5f);
        TS_ASSERT(row[3].discr_val == std::size_t(7));
      }
      TS_ASSERT(nb_dup == std::size_t(12));

      const auto& cols = genset.columnsOfInterest();
      TS_ASSERT(cols[0] == std::size_t(0));
      TS_ASSERT(cols[1] == std::size_t(2));
      TS_ASSERT(cols[2] == std::size_t(3));


      gum::learning::DBRowGeneratorSet<> genset2(genset);
      genset2.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset2.hasRows()) {
        const auto& row = genset2.generate().row();
        ++nb_dup;

        TS_ASSERT(row[0].discr_val == std::size_t(0));
        TS_ASSERT(row[2].cont_val == 4.5f);
        TS_ASSERT(row[3].discr_val == std::size_t(7));
      }
      TS_ASSERT(nb_dup == std::size_t(12));
      const auto& cols2 = genset2.columnsOfInterest();
      TS_ASSERT(cols2[0] == std::size_t(0));
      TS_ASSERT(cols2[1] == std::size_t(2));
      TS_ASSERT(cols2[2] == std::size_t(3));

      gum::learning::DBRowGeneratorSet<> genset3(std::move(genset2));
      genset3.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset3.hasRows()) {
        const auto& row = genset3.generate().row();
        ++nb_dup;

        TS_ASSERT(row[0].discr_val == std::size_t(0));
        TS_ASSERT(row[2].cont_val == 4.5f);
        TS_ASSERT(row[3].discr_val == std::size_t(7));
      }
      TS_ASSERT(nb_dup == std::size_t(12));
      const auto& cols3 = genset3.columnsOfInterest();
      TS_ASSERT(cols3[0] == std::size_t(0));
      TS_ASSERT(cols3[1] == std::size_t(2));
      TS_ASSERT(cols3[2] == std::size_t(3));

      gum::learning::DBRowGeneratorSet<>* genset4 = genset3.clone();
      genset4->setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset4->hasRows()) {
        const auto& row = genset4->generate().row();
        ++nb_dup;

        TS_ASSERT(row[0].discr_val == std::size_t(0));
        TS_ASSERT(row[2].cont_val == 4.5f);
        TS_ASSERT(row[3].discr_val == std::size_t(7));
      }
      TS_ASSERT(nb_dup == std::size_t(12));
      const auto& cols4 = genset4->columnsOfInterest();
      TS_ASSERT(cols4[0] == std::size_t(0));
      TS_ASSERT(cols4[1] == std::size_t(2));
      TS_ASSERT(cols4[2] == std::size_t(3));

      delete genset4;

      gum::learning::DBRowGeneratorSet<> genset5;
      TS_ASSERT(genset5.nbGenerators() == std::size_t(0));
      genset5 = genset;
      TS_ASSERT(genset5.nbGenerators() == std::size_t(2));
      genset5.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset5.hasRows()) {
        const auto& row = genset5.generate().row();
        ++nb_dup;

        TS_ASSERT(row[0].discr_val == std::size_t(0));
        TS_ASSERT(row[2].cont_val == 4.5f);
        TS_ASSERT(row[3].discr_val == std::size_t(7));
      }
      TS_ASSERT(nb_dup == std::size_t(12));
      const auto& cols5 = genset5.columnsOfInterest();
      TS_ASSERT(cols5[0] == std::size_t(0));
      TS_ASSERT(cols5[1] == std::size_t(2));
      TS_ASSERT(cols5[2] == std::size_t(3));

      gum::learning::DBRowGeneratorSet<> genset6;
      TS_ASSERT(genset6.nbGenerators() == std::size_t(0));
      genset6 = std::move(genset5);
      TS_ASSERT(genset6.nbGenerators() == std::size_t(2));
      genset6.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset6.hasRows()) {
        const auto& row = genset6.generate().row();
        ++nb_dup;

        TS_ASSERT(row[0].discr_val == std::size_t(0));
        TS_ASSERT(row[2].cont_val == 4.5f);
        TS_ASSERT(row[3].discr_val == std::size_t(7));
      }
      TS_ASSERT(nb_dup == std::size_t(12));
      const auto& cols6 = genset6.columnsOfInterest();
      TS_ASSERT(cols6[0] == std::size_t(0));
      TS_ASSERT(cols6[1] == std::size_t(2));
      TS_ASSERT(cols6[2] == std::size_t(3));

      gum::learning::MyGenerator<>& gen1 =
        dynamic_cast< gum::learning::MyGenerator<>& >(genset6[0]);
      TS_ASSERT(gen1.columnsOfInterest().size() == std::size_t(3));

      genset.setInputRow(input_row1);
      genset.clear();
      TS_ASSERT(!genset.hasRows());
    }

    void test_simple2() {
      const std::vector< gum::learning::DBTranslatedValueType > col_types{
        gum::learning::DBTranslatedValueType::DISCRETE,
        gum::learning::DBTranslatedValueType::DISCRETE,
        gum::learning::DBTranslatedValueType::CONTINUOUS,
        gum::learning::DBTranslatedValueType::DISCRETE};

      gum::learning::MyGenerator2<> generator1(col_types, 5);
      TS_ASSERT(!generator1.hasRows());
      gum::learning::MyGenerator2<> generator2(col_types, 4);
      TS_ASSERT(!generator2.hasRows());

      gum::learning::DBRowGeneratorSet<> genset;
      genset.insertGenerator(generator1);
      genset.insertGenerator(std::move(generator2));
      TS_ASSERT(genset.nbGenerators() == std::size_t(2));
      TS_ASSERT(genset.size() == std::size_t(2));
      TS_ASSERT(!genset.hasRows());

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

        TS_ASSERT(row[0].discr_val == std::size_t(0));
        TS_ASSERT(row[1].discr_val == std::size_t(4));
        TS_ASSERT(row[2].cont_val == 4.5f);
        TS_ASSERT(row[3].discr_val == std::size_t(7));
      }

      TS_ASSERT(nb_dup == std::size_t(12));

      genset.setInputRow(input_row1);
      TS_ASSERT(!genset.hasRows());

      genset.setInputRow(input_row1);
      nb_dup = std::size_t(0);
      while (genset.hasRows()) {
        const auto& row = genset.generate().row();
        ++nb_dup;

        TS_ASSERT(row[0].discr_val == std::size_t(0));
        TS_ASSERT(row[1].discr_val == std::size_t(4));
        TS_ASSERT(row[2].cont_val == 4.5f);
        TS_ASSERT(row[3].discr_val == std::size_t(7));
      }
    }


    void test_incomplete() {
      const std::vector< gum::learning::DBTranslatedValueType > col_types{
        gum::learning::DBTranslatedValueType::DISCRETE,
        gum::learning::DBTranslatedValueType::DISCRETE,
        gum::learning::DBTranslatedValueType::CONTINUOUS,
        gum::learning::DBTranslatedValueType::DISCRETE};

      gum::learning::DBRowGeneratorIdentity<>      generator1(col_types);
      gum::learning::DBRowGenerator4CompleteRows<> generator2(col_types);

      gum::learning::DBRowGeneratorSet<> genset;
      genset.insertGenerator(generator1);
      genset.insertGenerator(std::move(generator2));
      TS_ASSERT(genset.nbGenerators() == std::size_t(2));
      TS_ASSERT(genset.size() == std::size_t(2));
      TS_ASSERT(!genset.hasRows());

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

        TS_ASSERT(row[0].discr_val == std::size_t(0));
        TS_ASSERT(row[1].discr_val == std::size_t(4));
        TS_ASSERT(row[2].cont_val == 4.5f);
        TS_ASSERT(row[3].discr_val == std::size_t(7));
      }

      TS_ASSERT(nb_dup == std::size_t(1));

      genset.reset();
      TS_ASSERT(!genset.hasRows());

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row2{
        gum::learning::DBTranslatedValue{std::size_t(0)},
        gum::learning::DBTranslatedValue{
          std::numeric_limits< std::size_t >::max()},
        gum::learning::DBTranslatedValue{4.5f},
        gum::learning::DBTranslatedValue{std::size_t(7)}};
      const std::vector< std::size_t > cols_of_interest{std::size_t(0),
                                                        std::size_t(2)};

      genset.setColumnsOfInterest(cols_of_interest);
      genset.setInputRow(input_row2);
      nb_dup = std::size_t(0);
      while (genset.hasRows()) {
        const auto& row = genset.generate().row();
        ++nb_dup;

        TS_ASSERT(row[0].discr_val == std::size_t(0));
        TS_ASSERT(row[2].cont_val == 4.5f);
      }

      TS_ASSERT(nb_dup == std::size_t(1));
    }


    void test_basic() {
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
        gum::learning::DBTranslatedValue{
          std::numeric_limits< std::size_t >::max()},
        gum::learning::DBTranslatedValue{std::size_t(1)},
        gum::learning::DBTranslatedValue{std::size_t(0)}};

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row3{
        gum::learning::DBTranslatedValue{std::size_t(0)},
        gum::learning::DBTranslatedValue{
          std::numeric_limits< std::size_t >::max()},
        gum::learning::DBTranslatedValue{
          std::numeric_limits< std::size_t >::max()},
        gum::learning::DBTranslatedValue{std::size_t(0)}};

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row4{
        gum::learning::DBTranslatedValue{
          std::numeric_limits< std::size_t >::max()},
        gum::learning::DBTranslatedValue{
          std::numeric_limits< std::size_t >::max()},
        gum::learning::DBTranslatedValue{std::size_t(1)},
        gum::learning::DBTranslatedValue{std::size_t(0)}};

      const gum::learning::DBRow< gum::learning::DBTranslatedValue > input_row5{
        gum::learning::DBTranslatedValue{
          std::numeric_limits< std::size_t >::max()},
        gum::learning::DBTranslatedValue{
          std::numeric_limits< std::size_t >::max()},
        gum::learning::DBTranslatedValue{
          std::numeric_limits< std::size_t >::max()},
        gum::learning::DBTranslatedValue{
          std::numeric_limits< std::size_t >::max()}};


      gum::learning::DBRowGeneratorIdentity<> generator1(col_types);
      gum::learning::DBRowGeneratorEM<>       generator2(col_types, bn0);
      gum::learning::DBRowGenerator<>&        gen2 = generator2; // fix for g++-4.8
      gum::learning::DBRowGeneratorIdentity<> generator3(col_types);
      gum::learning::DBRowGeneratorEM<>       generator4(col_types, bn0);
      gum::learning::DBRowGenerator<>&        gen4 = generator4; // fix for g++-4.8

      gum::learning::DBRowGeneratorSet<> genset;
      genset.insertGenerator(generator1);
      genset.insertGenerator(gen2);
      genset.insertGenerator(generator3);
      genset.insertGenerator(gen4);

      TS_ASSERT_EQUALS(&(generator2.getBayesNet()), &bn0);

      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D");
      bn.cpt("A").fillWith({0.3, 0.7});
      bn.cpt("B").fillWith({0.4, 0.6, 0.7, 0.3});
      bn.cpt("C").fillWith({0.2, 0.8, 0.3, 0.7, 0.4, 0.6, 0.5, 0.5});
      bn.cpt("D").fillWith({0.8, 0.2});

      genset.setBayesNet(bn);

      gum::learning::DBRowGeneratorEM<>& genEM1 =
        dynamic_cast< gum::learning::DBRowGeneratorEM<>& >(genset[1]);
      TS_ASSERT_EQUALS(&(genEM1.getBayesNet()), &bn);

      gum::learning::DBRowGeneratorEM<>& genEM3 =
        dynamic_cast< gum::learning::DBRowGeneratorEM<>& >(genset[3]);
      TS_ASSERT_EQUALS(&(genEM3.getBayesNet()), &bn);

      TS_ASSERT(genset.nbGenerators() == std::size_t(4));
      TS_ASSERT(genset.size() == std::size_t(4));
      TS_ASSERT(!genset.hasRows());


      genset.setInputRow(input_row1);
      TS_ASSERT(genset.hasRows());
      {
        const auto& row = genset.generate().row();
        TS_ASSERT(row[0].discr_val == std::size_t(0));
        TS_ASSERT(row[1].discr_val == std::size_t(1));
        TS_ASSERT(row[2].discr_val == std::size_t(1));
        TS_ASSERT(row[3].discr_val == std::size_t(0));
      }

      const std::vector< std::size_t > cols_of_interest1{std::size_t(0),
                                                         std::size_t(3)};
      const std::vector< std::size_t > cols_of_interest2{std::size_t(0),
                                                         std::size_t(1)};

      genset.setColumnsOfInterest(cols_of_interest1);
      {
        const auto& xcols = genset.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(3));
        genset.setInputRow(input_row1);
        TS_ASSERT(genset.hasRows());

        const auto& row = genset.generate().row();
        TS_ASSERT(row[0].discr_val == std::size_t(0));
        TS_ASSERT(row[3].discr_val == std::size_t(0));

        genset.setInputRow(input_row1);
        TS_ASSERT(genset.hasRows());

        genset.setInputRow(input_row2);
        TS_ASSERT(genset.hasRows());
        const auto& row2 = genset.generate().row();
        TS_ASSERT(row2[0].discr_val == std::size_t(0));
        TS_ASSERT(row2[3].discr_val == std::size_t(0));
      }

      genset.setColumnsOfInterest(cols_of_interest2);
      {
        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row2);
        gum::Instantiation       inst(proba);

        genset.setInputRow(input_row2);
        TS_ASSERT(genset.hasRows());

        const auto& fill_row1 = genset.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(fill_row1.weight(), proba.get(inst));

        ++inst;
        const auto& fill_row2 = genset.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT_EQUALS(xfill_row2[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row2[1].discr_val, std::size_t(1));
        TS_ASSERT_EQUALS(fill_row2.weight(), proba.get(inst));
      }

      {
        const auto& xcols = genset.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row3);
        gum::Instantiation       inst(proba);

        genset.setInputRow(input_row3);
        TS_ASSERT(genset.hasRows());

        const auto& fill_row1 = genset.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(fill_row1.weight(), proba.get(inst));

        ++inst;
        const auto& fill_row2 = genset.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = genset.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
          __infer(bn, {std::size_t(1), std::size_t(0)}, input_row4);
        gum::Instantiation inst(proba);

        genset.setInputRow(input_row4);
        TS_ASSERT(genset.hasRows());

        int nb = 0;
        while (genset.hasRows()) {
          const auto& fill_row1 = genset.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      {
        const auto& xcols = genset.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
          __infer(bn, {std::size_t(1), std::size_t(0)}, input_row5);
        gum::Instantiation inst(proba);

        genset.setInputRow(input_row5);
        TS_ASSERT(genset.hasRows());

        int nb = 0;
        while (genset.hasRows()) {
          const auto& fill_row1 = genset.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      {
        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row2);
        gum::Instantiation       inst(proba);

        genset.setInputRow(input_row2);
        TS_ASSERT(genset.hasRows());

        const auto& fill_row1 = genset.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT(xfill_row1[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row1[1].discr_val == std::size_t(0));
        TS_ASSERT(fill_row1.weight() == proba.get(inst));

        ++inst;
        const auto& fill_row2 = genset.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }
    }
  };


} /* namespace gum_tests */
