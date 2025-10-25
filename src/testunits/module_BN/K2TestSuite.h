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

#include <agrum/base/database/databaseTable.h>
#include <agrum/base/database/DBInitializerFromCSV.h>
#include <agrum/base/database/DBRowGeneratorParser.h>
#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>
#include <agrum/base/graphs/DAG.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/learning/constraints/structuralConstraintDAG.h>
#include <agrum/BN/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/BN/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/BN/learning/constraints/structuralConstraintSetStatic.h>
#include <agrum/BN/learning/constraints/structuralConstraintSliceOrder.h>
#include <agrum/BN/learning/K2.h>
#include <agrum/BN/learning/paramUtils/paramEstimatorML.h>
#include <agrum/BN/learning/priors/smoothingPrior.h>
#include <agrum/BN/learning/structureUtils/graphChangesGenerator4K2.h>
#include <agrum/BN/learning/structureUtils/graphChangesSelector4DiGraph.h>

#include <agrum/BN/learning/scores_and_tests/scoreBDeu.h>
#include <agrum/BN/learning/scores_and_tests/scoreK2.h>

namespace gum_tests {

  class GUM_TEST_SUITE(K2) {
    public:
    GUM_ACTIVE_TEST(_k2_asia) {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/asia.csv"));
      const auto&                         var_names = initializer.variableNames();
      const std::size_t                   nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet                translator_set;
      gum::learning::DBTranslator4LabelizedVariable translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::SmoothingPrior       prior(database);
      gum::learning::ScoreK2              score(parser, prior);

      gum::learning::StructuralConstraintDAG struct_constraint;

      gum::learning::ParamEstimatorML estimator(parser, prior, score.internalPrior());

      std::vector< gum::NodeId > order(database.nbVariables());
      for (gum::NodeId i = 0; i < order.size(); ++i) {
        order[i] = i;
      }

      gum::learning::GraphChangesGenerator4K2< decltype(struct_constraint) > op_set(
          struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph< decltype(struct_constraint), decltype(op_set) >
          selector(score, struct_constraint, op_set);

      gum::learning::K2 k2;
      k2.setOrder(order);
      k2.approximationScheme().setEpsilon(1000);

      try {
        gum::BayesNet< double > bn = k2.learnBN< double >(selector, estimator);

        gum::BayesNet< double > bn2 = k2.learnBN< double >(selector, estimator);
        TS_ASSERT_EQUALS(bn.dag().arcs().size(), static_cast< gum::Size >(8))
        TS_ASSERT_EQUALS(bn2.dag().arcs().size(), static_cast< gum::Size >(8))
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }   // namespace gum_tests

    //@beforeMerging why is this code commented ?
    /*
    void xtest_k2_asia_bis () {
      gum::learning::DatabaseFromCSV database ( GET_RESSOURCES_PATH( "asia.csv"
    ) );
    gum::learning::DBRowTranslatorSet<gum::learning::CellTranslatorCompactIntId>
    translators;
      translators.insertTranslator ( 0, 8 );

    gum::learning::FilteredRowGeneratorSet<gum::learning::RowGeneratorIdentity>
        generators;
      generators.insertGenerator ();
    );

      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );

      std::vector<gum::Idx> modalities = filter.modalities ();

      gum::learning::SmoothingPrior prior;
      gum::learning::ScoreK2 real_score ( filter, modalities, prior );
      gum::learning::Score& score = real_score;

      gum::learning::StructuralConstraintDAG
        struct_constraint ( modalities.size () );

      gum::learning::ParamEstimatorML real_estimator ( filter, modalities );
      gum::learning::ParamEstimator& estimator = real_estimator;

      std::vector<gum::Idx> order ( filter.modalities ().size() );
      for ( gum::Idx i = 0; i < order.size(); ++i ) {
        order[i] = i;
      }

      gum::learning::K2 k2;

      gum::BayesNet<float> bn = k2.learnBN ( score, struct_constraint,
    estimator,
                                             database.variableNames (),
                                             modalities, order );

      gum::BayesNet<double> bn2 = k2.learnBN<double>
        ( score, struct_constraint, estimator,
          database.variableNames (),
          modalities, order );

      std::cout << bn << std::endl << bn.dag () << std::endl;
    }


    void xtest_K2_asia2 () {
      gum::learning::K2 k2;
      std::vector<gum::Idx> order { 3,2 };

      gum::BayesNet<float> bn =
        k2.learnBNFromCSV ( GET_RESSOURCES_PATH( "asia.csv" ), order );
      std::cout << bn << std::endl << bn.dag () << std::endl;

      gum::BayesNet<double> bn2 =
        k2.learnBNFromCSV<double> ( GET_RESSOURCES_PATH( "asia.csv" ), order );
      std::cout << bn2 << std::endl << bn2.dag () << std::endl;
    }


    void xxtest_k2_asia_constraint_DAG () {
      K2 k2;

      gum::learning::DatabaseFromCSV database ( GET_RESSOURCES_PATH( "asia.csv"
    ) );

      gum::learning::DBRowTranslatorSet<gum::learning::CellTranslatorCompactIntId>
    translators;
      translators.insertTranslator ( 0, 8 );

      gum::learning::FilteredRowGeneratorSet<gum::learning::RowGeneratorIdentity>
        generators;
      generators.insertGenerator ();

      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );

      std::vector<gum::Idx> modalities = filter.modalities ();

      gum::learning::SmoothingPrior prior;
      gum::learning::ScoreK2 score ( filter, modalities, prior );

      gum::learning::StructuralConstraintDAG
        struct_constraint ( modalities.size () );

      gum::learning::ParamEstimatorML estimator ( filter, modalities );

      std::vector<gum::Size> order ( filter.modalities ().size() );
      for ( gum::Idx i = 0; i < order.size(); ++i ) {
        order[i] = i;
      }

      gum::DAG dag = k2.learnDAG ( score, order, struct_constraint );
      gum::BayesNet<float> bn = k2.learnBN ( estimator, dag,
                                             database.variableNames (),
                                             modalities );

      std::cout << bn << std::endl << bn.dag () << std::endl;
    }


    void xxtest_k2_asia_constraint_SliceOrder () {
      K2 k2;

      gum::learning::DatabaseFromCSV database ( GET_RESSOURCES_PATH( "asia.csv"
    ) );

      gum::learning::DBRowTranslatorSet<gum::learning::CellTranslatorCompactIntId>
    translators;
      translators.insertTranslator ( 0, 8 );

      gum::learning::FilteredRowGeneratorSet<gum::learning::RowGeneratorIdentity>
        generators;
      generators.insertGenerator ();


      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );

      std::vector<gum::Size> modalities = filter.modalities ();

      gum::learning::SmoothingPrior prior;
      gum::learning::ScoreK2 score ( filter, modalities, prior );

      gum::NodeProperty<bool> slices;
      for ( gum::Idx i = 0; i < modalities.size (); ++i ) {
        if ( i % 2 ) {
          slices.insert ( i, false );
        }
        else {
          slices.insert ( i, true );
        }
      }

      gum::learning::StructuralConstraintSliceOrder
        struct_constraint ( slices );

      gum::learning::ParamEstimatorML estimator ( filter, modalities );

      std::vector<gum::Size> order ( filter.modalities ().size() );
      for ( gum::Idx i = 0; i < order.size(); ++i ) {
        order[i] = i;
      }

      gum::DAG dag = k2.learnDAG ( score, order, struct_constraint );
      gum::BayesNet<float> bn = k2.learnBN ( estimator, dag,
                                             database.variableNames (),
                                             modalities );

      std::cout << bn << std::endl << bn.dag () << std::endl;
    }


    void xxtest_k2 () {
      K2 k2;

      gum::learning::DatabaseFromCSV database ( GET_RESSOURCES_PATH( "alarm.csv"
    ) );

      gum::learning::DBRowTranslatorSet<gum::learning::CellTranslatorCompactIntId>
    translators;
      translators.insertTranslator ( 0, 37 );

      gum::learning::FilteredRowGeneratorSet<gum::learning::RowGeneratorIdentity>
        generators;
      generators.insertGenerator ();

      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );

      std::vector<gum::Size> modalities = filter.modalities ();

      gum::learning::SmoothingPrior prior;
      gum::learning::ScoreK2 score ( filter, modalities, prior );

      gum::learning::StructuralConstraintIndegree
        struct_constraint ( modalities.size (), 6 );

      gum::learning::ParamEstimatorML estimator ( filter, modalities );

      std::vector<gum::Size> order ( filter.modalities ().size() );
      for ( gum::Idx i = 0; i < order.size(); ++i ) {
        order[i] = i;
      }

      gum::Timer timer;

      gum::DAG dag = k2.learnDAG ( score, order, struct_constraint );
      gum::BayesNet<float> bn = k2.learnBN ( estimator, dag,
                                             database.variableNames (),
                                             modalities );
      std::cout << "learning time = " << timer.step () << std::endl;

      std::cout << dag << " " << bn << std::endl;
    }
    */
  };

} /* namespace gum_tests */
