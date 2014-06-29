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

#include <iostream>
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/graphs/DAG.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/learning/database/databaseFromCSV.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.h>
#include <agrum/learning/scores_and_tests/scoreK2.h>
#include <agrum/learning/scores_and_tests/scoreBDeu.h>
#include <agrum/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/learning/constraints/structuralConstraintDAG.h>
#include <agrum/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/learning/constraints/structuralConstraint2TimeSlice.h>
#include <agrum/learning/constraints/structuralConstraintSet.h>
#include <agrum/learning/paramLearningUtils/paramEstimatorML.h>
#include <agrum/learning/K2.h>

namespace gum_tests {

  class K2TestSuite: public CxxTest::TestSuite {
  public:

    class SimpleGenerator : public gum::learning::FilteredRowGenerator {
    public:
      gum::learning::FilteredRow& generate () {
        decreaseRemainingRows ();
        return *_input_row;
      }
      unsigned int _computeRows () { return 1; }
    };


    void test_k2_asia () {
       gum::learning::DatabaseFromCSV database ( GET_PATH_STR( "asia.csv" ) );
      
      auto translators = gum::learning::make_translators
        ( gum::learning::Create<gum::learning::CellTranslatorCompactIntId,
                                gum::learning::Col<0>, 8 > () );

      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      
      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );

      std::vector<unsigned int> modalities = filter.modalities ();

      gum::learning::ScoreK2<> score ( filter, modalities );

      gum::learning::StructuralConstraintDAG
        struct_constraint ( modalities.size () );

      gum::learning::ParamEstimatorML<> estimator ( filter, modalities );

      std::vector<unsigned int> order ( filter.modalities ().size() );
      for ( unsigned int i = 0; i < order.size(); ++i ) {
        order[i] = i;
      }

      gum::learning::K2 k2;
     
      gum::BayesNet<float> bn = k2.learnBN ( score, struct_constraint, estimator,
                                             database.variableNames (),
                                             modalities, order );

      gum::BayesNet<double> bn2 = k2.learnBN<double>
        ( score, struct_constraint, estimator,
          database.variableNames (),
          modalities, order );

      std::cout << bn << std::endl << bn.dag () << std::endl;
    }


    void test_K2_asia2 () {
      gum::learning::K2 k2;
      std::vector<unsigned int> order { 3,2 };

      gum::BayesNet<float> bn =
        k2.learnBNFromCSV ( GET_PATH_STR( "asia.csv" ), order );
      std::cout << bn << std::endl << bn.dag () << std::endl;
      
      gum::BayesNet<double> bn2 =
        k2.learnBNFromCSV<double> ( GET_PATH_STR( "asia.csv" ), order );
      std::cout << bn2 << std::endl << bn2.dag () << std::endl;
    }


    /*
    void xxtest_k2_asia_constraint_DAG () {
      K2 k2;

      gum::learning::DatabaseFromCSV database ( GET_PATH_STR( "asia.csv" ) );
      
      auto translators = gum::learning::make_translators
        ( gum::learning::Create<gum::learning::CellTranslatorCompactIntId, gum::learning::Col<0>, 8 > () );

      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      
      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );

      std::vector<unsigned int> modalities = filter.modalities ();

      gum::learning::ScoreK2<> score ( filter, modalities );

      gum::learning::StructuralConstraintDAG
        struct_constraint ( modalities.size () );

      gum::learning::ParamEstimatorML<> estimator ( filter, modalities );

      std::vector<unsigned int> order ( filter.modalities ().size() );
      for ( unsigned int i = 0; i < order.size(); ++i ) {
        order[i] = i;
      }
      
      gum::DAG dag = k2.learnDAG ( score, order, struct_constraint );
      gum::BayesNet<float> bn = k2.learnBN ( estimator, dag,
                                             database.variableNames (),
                                             modalities );

      std::cout << bn << std::endl << bn.dag () << std::endl;
    }

    
    void xxtest_k2_asia_constraint_2TimeSlice () {
      K2 k2;

      gum::learning::DatabaseFromCSV database ( GET_PATH_STR( "asia.csv" ) );
      
      auto translators = gum::learning::make_translators
        ( gum::learning::Create<gum::learning::CellTranslatorCompactIntId, gum::learning::Col<0>, 8 > () );

      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      
      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );

      std::vector<unsigned int> modalities = filter.modalities ();

      gum::learning::ScoreK2<> score ( filter, modalities );

      gum::NodeProperty<bool> slices;
      for ( unsigned int i = 0; i < modalities.size (); ++i ) {
        if ( i % 2 ) {
          slices.insert ( i, false );
        }
        else {
          slices.insert ( i, true );
        }
      }
      
      gum::learning::StructuralConstraint2TimeSlice
        struct_constraint ( slices );

      gum::learning::ParamEstimatorML<> estimator ( filter, modalities );

      std::vector<unsigned int> order ( filter.modalities ().size() );
      for ( unsigned int i = 0; i < order.size(); ++i ) {
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

      gum::learning::DatabaseFromCSV database ( GET_PATH_STR( "alarm.csv" ) );
      
      auto translators = gum::learning::make_translators
        ( gum::learning::Create<gum::learning::CellTranslatorCompactIntId, gum::learning::Col<0>, 37 > () );

      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      
      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );

      std::vector<unsigned int> modalities = filter.modalities ();

      gum::learning::ScoreK2<> score ( filter, modalities );

      gum::learning::StructuralConstraintIndegree
        struct_constraint ( modalities.size (), 6 );

      gum::learning::ParamEstimatorML<> estimator ( filter, modalities );

      std::vector<unsigned int> order ( filter.modalities ().size() );
      for ( unsigned int i = 0; i < order.size(); ++i ) {
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
