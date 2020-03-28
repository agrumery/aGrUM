
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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

#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/tools/database/DBRowGeneratorEM.h>

namespace gum_tests {

  class DBRowGeneratorEMTestSuite: public CxxTest::TestSuite {
    private:
    gum::Potential< double > __infer(
       const gum::BayesNet< double >&                                  bn,
       const std::vector< std::size_t >&                               targets,
       const gum::learning::DBRow< gum::learning::DBTranslatedValue >& row) {
      gum::LazyPropagation< double > ve(&bn);

      gum::NodeSet target_set;
      for (auto target: targets)
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
    void test_basic() {
      const std::vector< gum::learning::DBTranslatedValueType > col_types{
         gum::learning::DBTranslatedValueType::DISCRETE,
         gum::learning::DBTranslatedValueType::DISCRETE,
         gum::learning::DBTranslatedValueType::DISCRETE,
         gum::learning::DBTranslatedValueType::DISCRETE};

      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D");
      bn.cpt("A").fillWith({0.3, 0.7});
      bn.cpt("B").fillWith({0.4, 0.6, 0.7, 0.3});
      bn.cpt("C").fillWith({0.2, 0.8, 0.3, 0.7, 0.4, 0.6, 0.5, 0.5});
      bn.cpt("D").fillWith({0.8, 0.2});

      gum::learning::DBRowGeneratorEM<> generator(col_types, bn);
      TS_ASSERT(!generator.hasRows());

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


      {
        generator.setInputRow(input_row1);
        TS_ASSERT(generator.hasRows());
        TS_ASSERT(generator.hasRows());

        const auto& output_row1 = generator.generate();
        const auto& xoutput_row1 = output_row1.row();
        TS_ASSERT(xoutput_row1[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row1[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator.hasRows());
      }

      {
        generator.setInputRow(input_row1);
        TS_ASSERT(generator.hasRows());
        TS_ASSERT(generator.hasRows());

        const auto& output_row2 = generator.generate();
        const auto& xoutput_row2 = output_row2.row();
        TS_ASSERT(xoutput_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator.hasRows());
      }

      const std::vector< std::size_t > cols_of_interest1{std::size_t(0),
                                                         std::size_t(3)};
      const std::vector< std::size_t > cols_of_interest2{std::size_t(0),
                                                         std::size_t(1)};

      generator.setColumnsOfInterest(cols_of_interest1);
      {
        const auto& xcols = generator.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(3));
        generator.setInputRow(input_row1);
        TS_ASSERT(generator.hasRows());

        const auto& output_row3 = generator.generate();
        const auto& xoutput_row3 = output_row3.row();
        TS_ASSERT(xoutput_row3[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3[3].discr_val == std::size_t(0));

        generator.setInputRow(input_row1);
        TS_ASSERT(generator.hasRows());

        generator.setInputRow(input_row2);
        TS_ASSERT(generator.hasRows());
        const auto& output_row3b = generator.generate();
        const auto& xoutput_row3b = output_row3b.row();
        TS_ASSERT(xoutput_row3b[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3b[3].discr_val == std::size_t(0));
      }

      generator.setColumnsOfInterest(cols_of_interest2);


      {
        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row2);
        gum::Instantiation       inst(proba);

        generator.setInputRow(input_row2);
        TS_ASSERT(generator.hasRows());

        const auto& fill_row1 = generator.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT(xfill_row1[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row1[1].discr_val == std::size_t(0));
        TS_ASSERT(fill_row1.weight() == proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row3);
        gum::Instantiation       inst(proba);

        generator.setInputRow(input_row3);
        TS_ASSERT(generator.hasRows());

        const auto& fill_row1 = generator.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(fill_row1.weight(), proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row4);
        gum::Instantiation inst(proba);

        generator.setInputRow(input_row4);
        TS_ASSERT(generator.hasRows());

        int nb = 0;
        while (generator.hasRows()) {
          const auto& fill_row1 = generator.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      {
        const auto& xcols = generator.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row5);
        gum::Instantiation inst(proba);

        generator.setInputRow(input_row5);
        TS_ASSERT(generator.hasRows());

        int nb = 0;
        while (generator.hasRows()) {
          const auto& fill_row1 = generator.generate();
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

        generator.setInputRow(input_row2);
        TS_ASSERT(generator.hasRows());

        const auto& fill_row1 = generator.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT(xfill_row1[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row1[1].discr_val == std::size_t(0));
        TS_ASSERT(fill_row1.weight() == proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      generator.setColumnsOfInterest(cols_of_interest1);


      std::allocator< gum::learning::DBTranslatedValue > alloc;
      gum::learning::DBRowGeneratorEM<> generator2(generator, alloc);
      TS_ASSERT(!generator2.hasRows());
      TS_ASSERT(generator2.columnsOfInterest().size() == std::size_t(2));
      TS_ASSERT(generator2.columnsOfInterest()[0] == std::size_t(0));
      TS_ASSERT(generator2.columnsOfInterest()[1] == std::size_t(3));

      {
        generator2.setInputRow(input_row1);
        TS_ASSERT(generator2.hasRows());
        TS_ASSERT(generator2.hasRows());

        const auto& output_row1 = generator2.generate();
        const auto& xoutput_row1 = output_row1.row();
        TS_ASSERT(xoutput_row1[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row1[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator2.hasRows());
      }

      {
        generator2.setInputRow(input_row1);
        TS_ASSERT(generator2.hasRows());
        TS_ASSERT(generator2.hasRows());

        const auto& output_row2 = generator2.generate();
        const auto& xoutput_row2 = output_row2.row();
        TS_ASSERT(xoutput_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator2.hasRows());
      }

      generator2.setColumnsOfInterest(cols_of_interest1);

      {
        const auto& xcols = generator2.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(3));
        generator2.setInputRow(input_row1);
        TS_ASSERT(generator2.hasRows());

        const auto& output_row3 = generator2.generate();
        const auto& xoutput_row3 = output_row3.row();
        TS_ASSERT(xoutput_row3[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3[3].discr_val == std::size_t(0));

        generator2.setInputRow(input_row1);
        TS_ASSERT(generator2.hasRows());

        generator2.setInputRow(input_row2);
        TS_ASSERT(generator2.hasRows());
        const auto& output_row3b = generator2.generate();
        const auto& xoutput_row3b = output_row3b.row();
        TS_ASSERT(xoutput_row3b[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3b[3].discr_val == std::size_t(0));
      }

      generator2.setColumnsOfInterest(cols_of_interest2);

      {
        const auto& xcols = generator2.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row2);
        gum::Instantiation       inst(proba);

        generator2.setInputRow(input_row2);
        TS_ASSERT(generator2.hasRows());

        const auto& fill_row1 = generator2.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT(fill_row1.weight() == proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator2.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator2.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row3);
        gum::Instantiation       inst(proba);

        generator2.setInputRow(input_row3);
        TS_ASSERT(generator2.hasRows());

        const auto& fill_row1 = generator2.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(fill_row1.weight(), proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator2.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator2.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row4);
        gum::Instantiation inst(proba);

        generator2.setInputRow(input_row4);
        TS_ASSERT(generator2.hasRows());

        int nb = 0;
        while (generator2.hasRows()) {
          const auto& fill_row1 = generator2.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      {
        const auto& xcols = generator2.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row5);
        gum::Instantiation inst(proba);

        generator2.setInputRow(input_row5);
        TS_ASSERT(generator2.hasRows());

        int nb = 0;
        while (generator2.hasRows()) {
          const auto& fill_row1 = generator2.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      generator2.setColumnsOfInterest(cols_of_interest1);


      gum::learning::DBRowGeneratorEM<> generator3(generator);
      TS_ASSERT(!generator3.hasRows());
      TS_ASSERT(generator3.columnsOfInterest().size() == std::size_t(2));
      TS_ASSERT(generator3.columnsOfInterest()[0] == std::size_t(0));
      TS_ASSERT(generator3.columnsOfInterest()[1] == std::size_t(3));

      {
        generator3.setInputRow(input_row1);
        TS_ASSERT(generator3.hasRows());
        TS_ASSERT(generator3.hasRows());

        const auto& output_row1 = generator3.generate();
        const auto& xoutput_row1 = output_row1.row();
        TS_ASSERT(xoutput_row1[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row1[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator3.hasRows());
      }

      {
        generator3.setInputRow(input_row1);
        TS_ASSERT(generator3.hasRows());
        TS_ASSERT(generator3.hasRows());

        const auto& output_row2 = generator3.generate();
        const auto& xoutput_row2 = output_row2.row();
        TS_ASSERT(xoutput_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator3.hasRows());
      }

      generator3.setColumnsOfInterest(cols_of_interest1);

      {
        const auto& xcols = generator3.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(3));
        generator3.setInputRow(input_row1);
        TS_ASSERT(generator3.hasRows());

        const auto& output_row3 = generator3.generate();
        const auto& xoutput_row3 = output_row3.row();
        TS_ASSERT(xoutput_row3[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3[3].discr_val == std::size_t(0));

        generator3.setInputRow(input_row1);
        TS_ASSERT(generator3.hasRows());

        generator3.setInputRow(input_row2);
        TS_ASSERT(generator3.hasRows());
        const auto& output_row3b = generator3.generate();
        const auto& xoutput_row3b = output_row3b.row();
        TS_ASSERT(xoutput_row3b[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3b[3].discr_val == std::size_t(0));
      }

      generator3.setColumnsOfInterest(cols_of_interest2);

      {
        const auto& xcols = generator3.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row2);
        gum::Instantiation       inst(proba);

        generator3.setInputRow(input_row2);
        TS_ASSERT(generator3.hasRows());

        const auto& fill_row1 = generator3.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT(fill_row1.weight() == proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator3.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator3.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row3);
        gum::Instantiation       inst(proba);

        generator3.setInputRow(input_row3);
        TS_ASSERT(generator3.hasRows());

        const auto& fill_row1 = generator3.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(fill_row1.weight(), proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator3.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator3.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row4);
        gum::Instantiation inst(proba);

        generator3.setInputRow(input_row4);
        TS_ASSERT(generator3.hasRows());

        int nb = 0;
        while (generator3.hasRows()) {
          const auto& fill_row1 = generator3.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      {
        const auto& xcols = generator3.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row5);
        gum::Instantiation inst(proba);

        generator3.setInputRow(input_row5);
        TS_ASSERT(generator3.hasRows());

        int nb = 0;
        while (generator3.hasRows()) {
          const auto& fill_row1 = generator3.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      generator3.setColumnsOfInterest(cols_of_interest1);


      gum::learning::DBRowGeneratorEM<> generator4(std::move(generator2), alloc);
      TS_ASSERT(!generator4.hasRows());
      TS_ASSERT(generator4.columnsOfInterest().size() == std::size_t(2));
      TS_ASSERT(generator4.columnsOfInterest()[0] == std::size_t(0));
      TS_ASSERT(generator4.columnsOfInterest()[1] == std::size_t(3));

      {
        generator4.setInputRow(input_row1);
        TS_ASSERT(generator4.hasRows());
        TS_ASSERT(generator4.hasRows());

        const auto& output_row1 = generator4.generate();
        const auto& xoutput_row1 = output_row1.row();
        TS_ASSERT(xoutput_row1[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row1[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator4.hasRows());
      }

      {
        generator4.setInputRow(input_row1);
        TS_ASSERT(generator4.hasRows());
        TS_ASSERT(generator4.hasRows());

        const auto& output_row2 = generator4.generate();
        const auto& xoutput_row2 = output_row2.row();
        TS_ASSERT(xoutput_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator4.hasRows());
      }

      generator4.setColumnsOfInterest(cols_of_interest1);

      {
        const auto& xcols = generator4.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(3));
        generator4.setInputRow(input_row1);
        TS_ASSERT(generator4.hasRows());

        const auto& output_row3 = generator4.generate();
        const auto& xoutput_row3 = output_row3.row();
        TS_ASSERT(xoutput_row3[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3[3].discr_val == std::size_t(0));

        generator4.setInputRow(input_row1);
        TS_ASSERT(generator4.hasRows());

        generator4.setInputRow(input_row2);
        TS_ASSERT(generator4.hasRows());
        const auto& output_row3b = generator4.generate();
        const auto& xoutput_row3b = output_row3b.row();
        TS_ASSERT(xoutput_row3b[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3b[3].discr_val == std::size_t(0));
      }

      generator4.setColumnsOfInterest(cols_of_interest2);

      {
        const auto& xcols = generator4.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row2);
        gum::Instantiation       inst(proba);

        generator4.setInputRow(input_row2);
        TS_ASSERT(generator4.hasRows());

        const auto& fill_row1 = generator4.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT(fill_row1.weight() == proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator4.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator4.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row3);
        gum::Instantiation       inst(proba);

        generator4.setInputRow(input_row3);
        TS_ASSERT(generator4.hasRows());

        const auto& fill_row1 = generator4.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(fill_row1.weight(), proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator4.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator4.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row4);
        gum::Instantiation inst(proba);

        generator4.setInputRow(input_row4);
        TS_ASSERT(generator4.hasRows());

        int nb = 0;
        while (generator4.hasRows()) {
          const auto& fill_row1 = generator4.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      {
        const auto& xcols = generator4.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row5);
        gum::Instantiation inst(proba);

        generator4.setInputRow(input_row5);
        TS_ASSERT(generator4.hasRows());

        int nb = 0;
        while (generator4.hasRows()) {
          const auto& fill_row1 = generator4.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      generator4.setColumnsOfInterest(cols_of_interest1);


      gum::learning::DBRowGeneratorEM<> generator5(std::move(generator3));
      TS_ASSERT(!generator5.hasRows());
      TS_ASSERT(generator5.columnsOfInterest().size() == std::size_t(2));
      TS_ASSERT(generator5.columnsOfInterest()[0] == std::size_t(0));
      TS_ASSERT(generator5.columnsOfInterest()[1] == std::size_t(3));

      {
        generator5.setInputRow(input_row1);
        TS_ASSERT(generator5.hasRows());
        TS_ASSERT(generator5.hasRows());

        const auto& output_row1 = generator5.generate();
        const auto& xoutput_row1 = output_row1.row();
        TS_ASSERT(xoutput_row1[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row1[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator5.hasRows());
      }

      {
        generator5.setInputRow(input_row1);
        TS_ASSERT(generator5.hasRows());
        TS_ASSERT(generator5.hasRows());

        const auto& output_row2 = generator5.generate();
        const auto& xoutput_row2 = output_row2.row();
        TS_ASSERT(xoutput_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator5.hasRows());
      }

      generator5.setColumnsOfInterest(cols_of_interest1);

      {
        const auto& xcols = generator5.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(3));
        generator5.setInputRow(input_row1);
        TS_ASSERT(generator5.hasRows());

        const auto& output_row3 = generator5.generate();
        const auto& xoutput_row3 = output_row3.row();
        TS_ASSERT(xoutput_row3[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3[3].discr_val == std::size_t(0));

        generator5.setInputRow(input_row1);
        TS_ASSERT(generator5.hasRows());

        generator5.setInputRow(input_row2);
        TS_ASSERT(generator5.hasRows());
        const auto& output_row3b = generator5.generate();
        const auto& xoutput_row3b = output_row3b.row();
        TS_ASSERT(xoutput_row3b[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3b[3].discr_val == std::size_t(0));
      }

      generator5.setColumnsOfInterest(cols_of_interest2);

      {
        const auto& xcols = generator5.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row2);
        gum::Instantiation       inst(proba);

        generator5.setInputRow(input_row2);
        TS_ASSERT(generator5.hasRows());

        const auto& fill_row1 = generator5.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT(fill_row1.weight() == proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator5.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator5.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row3);
        gum::Instantiation       inst(proba);

        generator5.setInputRow(input_row3);
        TS_ASSERT(generator5.hasRows());

        const auto& fill_row1 = generator5.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(fill_row1.weight(), proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator5.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator5.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row4);
        gum::Instantiation inst(proba);

        generator5.setInputRow(input_row4);
        TS_ASSERT(generator5.hasRows());

        int nb = 0;
        while (generator5.hasRows()) {
          const auto& fill_row1 = generator5.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      {
        const auto& xcols = generator5.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row5);
        gum::Instantiation inst(proba);

        generator5.setInputRow(input_row5);
        TS_ASSERT(generator5.hasRows());

        int nb = 0;
        while (generator5.hasRows()) {
          const auto& fill_row1 = generator5.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      generator5.setColumnsOfInterest(cols_of_interest1);


      gum::learning::DBRowGeneratorEM<>* generator6 = generator4.clone(alloc);
      TS_ASSERT(!generator6->hasRows());
      TS_ASSERT(generator6->columnsOfInterest().size() == std::size_t(2));
      TS_ASSERT(generator6->columnsOfInterest()[0] == std::size_t(0));
      TS_ASSERT(generator6->columnsOfInterest()[1] == std::size_t(3));

      {
        generator6->setInputRow(input_row1);
        TS_ASSERT(generator6->hasRows());
        TS_ASSERT(generator6->hasRows());

        const auto& output_row1 = generator6->generate();
        const auto& xoutput_row1 = output_row1.row();
        TS_ASSERT(xoutput_row1[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row1[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator6->hasRows());
      }

      {
        generator6->setInputRow(input_row1);
        TS_ASSERT(generator6->hasRows());
        TS_ASSERT(generator6->hasRows());

        const auto& output_row2 = generator6->generate();
        const auto& xoutput_row2 = output_row2.row();
        TS_ASSERT(xoutput_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator6->hasRows());
      }

      generator6->setColumnsOfInterest(cols_of_interest1);

      {
        const auto& xcols = generator6->columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(3));
        generator6->setInputRow(input_row1);
        TS_ASSERT(generator6->hasRows());

        const auto& output_row3 = generator6->generate();
        const auto& xoutput_row3 = output_row3.row();
        TS_ASSERT(xoutput_row3[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3[3].discr_val == std::size_t(0));

        generator6->setInputRow(input_row1);
        TS_ASSERT(generator6->hasRows());

        generator6->setInputRow(input_row2);
        TS_ASSERT(generator6->hasRows());
        const auto& output_row3b = generator6->generate();
        const auto& xoutput_row3b = output_row3b.row();
        TS_ASSERT(xoutput_row3b[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3b[3].discr_val == std::size_t(0));
      }

      generator6->setColumnsOfInterest(cols_of_interest2);

      {
        const auto& xcols = generator6->columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row2);
        gum::Instantiation       inst(proba);

        generator6->setInputRow(input_row2);
        TS_ASSERT(generator6->hasRows());

        const auto& fill_row1 = generator6->generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT(fill_row1.weight() == proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator6->generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator6->columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row3);
        gum::Instantiation       inst(proba);

        generator6->setInputRow(input_row3);
        TS_ASSERT(generator6->hasRows());

        const auto& fill_row1 = generator6->generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(fill_row1.weight(), proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator6->generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator6->columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row4);
        gum::Instantiation inst(proba);

        generator6->setInputRow(input_row4);
        TS_ASSERT(generator6->hasRows());

        int nb = 0;
        while (generator6->hasRows()) {
          const auto& fill_row1 = generator6->generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      {
        const auto& xcols = generator6->columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row5);
        gum::Instantiation inst(proba);

        generator6->setInputRow(input_row5);
        TS_ASSERT(generator6->hasRows());

        int nb = 0;
        while (generator6->hasRows()) {
          const auto& fill_row1 = generator6->generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      generator6->setColumnsOfInterest(cols_of_interest1);
      delete generator6;


      gum::learning::DBRowGeneratorEM<>* generator7 = generator5.clone();
      TS_ASSERT(!generator7->hasRows());
      TS_ASSERT(generator7->columnsOfInterest().size() == std::size_t(2));
      TS_ASSERT(generator7->columnsOfInterest()[0] == std::size_t(0));
      TS_ASSERT(generator7->columnsOfInterest()[1] == std::size_t(3));

      {
        generator7->setInputRow(input_row1);
        TS_ASSERT(generator7->hasRows());
        TS_ASSERT(generator7->hasRows());

        const auto& output_row1 = generator7->generate();
        const auto& xoutput_row1 = output_row1.row();
        TS_ASSERT(xoutput_row1[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row1[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator7->hasRows());
      }

      {
        generator7->setInputRow(input_row1);
        TS_ASSERT(generator7->hasRows());
        TS_ASSERT(generator7->hasRows());

        const auto& output_row2 = generator7->generate();
        const auto& xoutput_row2 = output_row2.row();
        TS_ASSERT(xoutput_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator7->hasRows());
      }

      generator7->setColumnsOfInterest(cols_of_interest1);

      {
        const auto& xcols = generator7->columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(3));
        generator7->setInputRow(input_row1);
        TS_ASSERT(generator7->hasRows());

        const auto& output_row3 = generator7->generate();
        const auto& xoutput_row3 = output_row3.row();
        TS_ASSERT(xoutput_row3[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3[3].discr_val == std::size_t(0));

        generator7->setInputRow(input_row1);
        TS_ASSERT(generator7->hasRows());

        generator7->setInputRow(input_row2);
        TS_ASSERT(generator7->hasRows());
        const auto& output_row3b = generator7->generate();
        const auto& xoutput_row3b = output_row3b.row();
        TS_ASSERT(xoutput_row3b[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3b[3].discr_val == std::size_t(0));
      }

      generator7->setColumnsOfInterest(cols_of_interest2);

      {
        const auto& xcols = generator7->columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row2);
        gum::Instantiation       inst(proba);

        generator7->setInputRow(input_row2);
        TS_ASSERT(generator7->hasRows());

        const auto& fill_row1 = generator7->generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT(fill_row1.weight() == proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator7->generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator7->columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row3);
        gum::Instantiation       inst(proba);

        generator7->setInputRow(input_row3);
        TS_ASSERT(generator7->hasRows());

        const auto& fill_row1 = generator7->generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(fill_row1.weight(), proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator7->generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator7->columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row4);
        gum::Instantiation inst(proba);

        generator7->setInputRow(input_row4);
        TS_ASSERT(generator7->hasRows());

        int nb = 0;
        while (generator7->hasRows()) {
          const auto& fill_row1 = generator7->generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      {
        const auto& xcols = generator7->columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row5);
        gum::Instantiation inst(proba);

        generator7->setInputRow(input_row5);
        TS_ASSERT(generator7->hasRows());

        int nb = 0;
        while (generator7->hasRows()) {
          const auto& fill_row1 = generator7->generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      generator7->setColumnsOfInterest(cols_of_interest1);
      delete generator7;


      auto bn2 = gum::BayesNet< double >::fastPrototype("B;A");


      gum::learning::DBRowGeneratorEM<> generator8(col_types, bn2);
      TS_ASSERT(!generator8.hasRows());
      TS_ASSERT(generator8.columnsOfInterest().size() == std::size_t(0));

      generator.setInputRow(input_row1);
      TS_ASSERT(!generator8.hasRows());
      TS_ASSERT(generator8.columnsOfInterest().size() == std::size_t(0));
      generator8 = generator;

      {
        generator8.setInputRow(input_row1);
        TS_ASSERT(generator8.hasRows());
        TS_ASSERT(generator8.hasRows());

        const auto& output_row1 = generator8.generate();
        const auto& xoutput_row1 = output_row1.row();
        TS_ASSERT(xoutput_row1[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row1[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator8.hasRows());
      }

      {
        generator8.setInputRow(input_row1);
        TS_ASSERT(generator8.hasRows());
        TS_ASSERT(generator8.hasRows());

        const auto& output_row2 = generator8.generate();
        const auto& xoutput_row2 = output_row2.row();
        TS_ASSERT(xoutput_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator8.hasRows());
      }

      generator8.setColumnsOfInterest(cols_of_interest1);
      {
        const auto& xcols = generator8.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(3));
        generator8.setInputRow(input_row1);
        TS_ASSERT(generator8.hasRows());

        const auto& output_row3 = generator8.generate();
        const auto& xoutput_row3 = output_row3.row();
        TS_ASSERT(xoutput_row3[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3[3].discr_val == std::size_t(0));

        generator8.setInputRow(input_row1);
        TS_ASSERT(generator8.hasRows());

        generator8.setInputRow(input_row2);
        TS_ASSERT(generator8.hasRows());
        const auto& output_row3b = generator8.generate();
        const auto& xoutput_row3b = output_row3b.row();
        TS_ASSERT(xoutput_row3b[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3b[3].discr_val == std::size_t(0));
      }

      generator8.setColumnsOfInterest(cols_of_interest2);


      {
        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row2);
        gum::Instantiation       inst(proba);

        generator8.setInputRow(input_row2);
        TS_ASSERT(generator8.hasRows());

        const auto& fill_row1 = generator8.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT(xfill_row1[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row1[1].discr_val == std::size_t(0));
        TS_ASSERT(fill_row1.weight() == proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator8.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator8.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row3);
        gum::Instantiation       inst(proba);

        generator8.setInputRow(input_row3);
        TS_ASSERT(generator8.hasRows());

        const auto& fill_row1 = generator8.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(fill_row1.weight(), proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator8.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator8.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row4);
        gum::Instantiation inst(proba);

        generator8.setInputRow(input_row4);
        TS_ASSERT(generator8.hasRows());

        int nb = 0;
        while (generator8.hasRows()) {
          const auto& fill_row1 = generator8.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      {
        const auto& xcols = generator8.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row5);
        gum::Instantiation inst(proba);

        generator8.setInputRow(input_row5);
        TS_ASSERT(generator8.hasRows());

        int nb = 0;
        while (generator8.hasRows()) {
          const auto& fill_row1 = generator8.generate();
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

        generator8.setInputRow(input_row2);
        TS_ASSERT(generator8.hasRows());

        const auto& fill_row1 = generator8.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT(xfill_row1[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row1[1].discr_val == std::size_t(0));
        TS_ASSERT(fill_row1.weight() == proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator8.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      generator8.setColumnsOfInterest(cols_of_interest1);


      gum::learning::DBRowGeneratorEM<> generator9(col_types, bn2);
      TS_ASSERT(!generator9.hasRows());
      TS_ASSERT(generator9.columnsOfInterest().size() == std::size_t(0));

      generator.setInputRow(input_row1);
      TS_ASSERT(!generator9.hasRows());
      TS_ASSERT(generator9.columnsOfInterest().size() == std::size_t(0));
      generator9 = std::move(generator);

      {
        generator9.setInputRow(input_row1);
        TS_ASSERT(generator9.hasRows());
        TS_ASSERT(generator9.hasRows());

        const auto& output_row1 = generator9.generate();
        const auto& xoutput_row1 = output_row1.row();
        TS_ASSERT(xoutput_row1[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row1[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row1[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator9.hasRows());
      }

      {
        generator9.setInputRow(input_row1);
        TS_ASSERT(generator9.hasRows());
        TS_ASSERT(generator9.hasRows());

        const auto& output_row2 = generator9.generate();
        const auto& xoutput_row2 = output_row2.row();
        TS_ASSERT(xoutput_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[2].discr_val == std::size_t(1));
        TS_ASSERT(xoutput_row2[3].discr_val == std::size_t(0));
        TS_ASSERT(!generator9.hasRows());
      }

      generator9.setColumnsOfInterest(cols_of_interest1);
      {
        const auto& xcols = generator9.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(3));
        generator9.setInputRow(input_row1);
        TS_ASSERT(generator9.hasRows());

        const auto& output_row3 = generator9.generate();
        const auto& xoutput_row3 = output_row3.row();
        TS_ASSERT(xoutput_row3[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3[3].discr_val == std::size_t(0));

        generator9.setInputRow(input_row1);
        TS_ASSERT(generator9.hasRows());

        generator9.setInputRow(input_row2);
        TS_ASSERT(generator9.hasRows());
        const auto& output_row3b = generator9.generate();
        const auto& xoutput_row3b = output_row3b.row();
        TS_ASSERT(xoutput_row3b[0].discr_val == std::size_t(0));
        TS_ASSERT(xoutput_row3b[3].discr_val == std::size_t(0));
      }

      generator9.setColumnsOfInterest(cols_of_interest2);


      {
        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row2);
        gum::Instantiation       inst(proba);

        generator9.setInputRow(input_row2);
        TS_ASSERT(generator9.hasRows());

        const auto& fill_row1 = generator9.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT(xfill_row1[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row1[1].discr_val == std::size_t(0));
        TS_ASSERT(fill_row1.weight() == proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator9.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator9.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba = __infer(bn, {std::size_t(1)}, input_row3);
        gum::Instantiation       inst(proba);

        generator9.setInputRow(input_row3);
        TS_ASSERT(generator9.hasRows());

        const auto& fill_row1 = generator9.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(fill_row1.weight(), proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator9.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      {
        const auto& xcols = generator9.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row4);
        gum::Instantiation inst(proba);

        generator9.setInputRow(input_row4);
        TS_ASSERT(generator9.hasRows());

        int nb = 0;
        while (generator9.hasRows()) {
          const auto& fill_row1 = generator9.generate();
          const auto& xfill_row1 = fill_row1.row();

          ++nb;
          inst.chgVal(inst.variable("A"), xfill_row1[0].discr_val);
          inst.chgVal(inst.variable("B"), xfill_row1[1].discr_val);

          TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst), 0.001);
        }

        TS_ASSERT(nb == 4);
      }

      {
        const auto& xcols = generator9.columnsOfInterest();
        TS_ASSERT(xcols.size() == std::size_t(2));
        TS_ASSERT(xcols[0] == std::size_t(0));
        TS_ASSERT(xcols[1] == std::size_t(1));

        gum::Potential< double > proba =
           __infer(bn, {std::size_t(1), std::size_t(0)}, input_row5);
        gum::Instantiation inst(proba);

        generator9.setInputRow(input_row5);
        TS_ASSERT(generator9.hasRows());

        int nb = 0;
        while (generator9.hasRows()) {
          const auto& fill_row1 = generator9.generate();
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

        generator9.setInputRow(input_row2);
        TS_ASSERT(generator9.hasRows());

        const auto& fill_row1 = generator9.generate();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT(xfill_row1[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row1[1].discr_val == std::size_t(0));
        TS_ASSERT(fill_row1.weight() == proba.get(inst));

        ++inst;
        const auto& fill_row2 = generator9.generate();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT(xfill_row2[0].discr_val == std::size_t(0));
        TS_ASSERT(xfill_row2[1].discr_val == std::size_t(1));
        TS_ASSERT(fill_row2.weight() == proba.get(inst));
      }

      generator9.setColumnsOfInterest(cols_of_interest1);
    }
  };


} /* namespace gum_tests */
