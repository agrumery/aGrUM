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
#include <iostream>

#include <agrum/BN/BayesNet.h>
#include <agrum/graphs/DAG.h>
#include <agrum/variables/labelizedVariable.h>

#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBRowGeneratorParser.h>
#include <agrum/learning/database/DBInitializerFromCSV.h>
#include <agrum/learning/database/databaseTable.h>
#include <agrum/learning/database/DBTranslatorSet.h>

#include <agrum/learning/scores_and_tests/scoreBDeu.h>
#include <agrum/learning/scores_and_tests/scoreK2.h>

#include <agrum/learning/aprioris/aprioriSmoothing.h>

#include <agrum/learning/constraints/structuralConstraintDAG.h>
#include <agrum/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/learning/constraints/structuralConstraintSetStatic.h>
#include <agrum/learning/constraints/structuralConstraintSliceOrder.h>

#include <agrum/learning/structureUtils/graphChangesGenerator4K2.h>
#include <agrum/learning/structureUtils/graphChangesSelector4DiGraph.h>

#include <agrum/learning/K2.h>
#include <agrum/learning/paramUtils/paramEstimatorML.h>

namespace gum_tests {

  class K2TestSuite : public CxxTest::TestSuite {
    public:
    void test_k2_asia() {
      gum::learning::DBInitializerFromCSV<> initializer(
        GET_RESSOURCES_PATH("asia.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars = var_names.size();

      gum::learning::DBTranslatorSet<>                translator_set;
      gum::learning::DBTranslator4LabelizedVariable<> translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable<> database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);
      gum::learning::AprioriSmoothing<>    apriori(database);
      gum::learning::ScoreK2<>             score(parser, apriori);

      gum::learning::StructuralConstraintDAG struct_constraint;

      gum::learning::ParamEstimatorML<> estimator(parser, apriori,
                                                   score.internalApriori());

      std::vector< gum::NodeId > order(database.nbVariables());
      for (gum::NodeId i = 0; i < order.size(); ++i) {
        order[i] = i;
      }

      gum::learning::GraphChangesGenerator4K2< decltype(struct_constraint) >
        op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph< decltype(struct_constraint),
                                                    decltype(op_set) >
        selector(score, struct_constraint, op_set);

      gum::learning::K2 k2;
      k2.setOrder(order);
      k2.approximationScheme().setEpsilon(1000);

      try {
        gum::BayesNet< float > bn = k2.learnBN< float >(selector, estimator);

        gum::BayesNet< double > bn2 =
          k2.learnBN< double >(selector, estimator);
        TS_ASSERT(bn.dag().arcs().size() == 8);
        TS_ASSERT(bn2.dag().arcs().size() == 8);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

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

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreK2<> real_score ( filter, modalities, apriori );
      gum::learning::Score<>& score = real_score;

      gum::learning::StructuralConstraintDAG
        struct_constraint ( modalities.size () );

      gum::learning::ParamEstimatorML<> real_estimator ( filter, modalities );
      gum::learning::ParamEstimator<>& estimator = real_estimator;

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

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreK2<> score ( filter, modalities, apriori );

      gum::learning::StructuralConstraintDAG
        struct_constraint ( modalities.size () );

      gum::learning::ParamEstimatorML<> estimator ( filter, modalities );

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

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreK2<> score ( filter, modalities, apriori );

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

      gum::learning::ParamEstimatorML<> estimator ( filter, modalities );

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

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreK2<> score ( filter, modalities, apriori );

      gum::learning::StructuralConstraintIndegree
        struct_constraint ( modalities.size (), 6 );

      gum::learning::ParamEstimatorML<> estimator ( filter, modalities );

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
