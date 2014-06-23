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
#include <agrum/learning/constraints/structuralConstraintTabuList.h>
#include <agrum/learning/constraints/structuralConstraintSet.h>
#include <agrum/learning/structureLearningUtils/graphChangesGenerator4DiGraph.h>
#include <agrum/learning/paramLearningUtils/paramEstimatorML.h>
#include <agrum/learning/localSearchWithTabuList.h>

#define MY_ALARM GET_PATH_STR( "alarm.csv" )

namespace gum_tests {

  class LocalSearchWithTabuListTestSuite: public CxxTest::TestSuite {
  public:
    

    class SimpleGenerator : public gum::learning::FilteredRowGenerator {
    public:
      gum::learning::FilteredRow& generate () {
        decreaseRemainingRows ();
        return *_input_row;
      }
      unsigned int _computeRows () { return 1; }
    };

    


    void test_asia () {
      gum::learning::DatabaseFromCSV database ( GET_PATH_STR( "asia.csv" ) );
      
      auto translators1 = gum::learning::make_translators
        ( gum::learning::Create<gum::learning::CellTranslatorCompactIntId,
                                gum::learning::Col<0>, 8 > () );

      //auto translators = translators1;

      gum::learning::DBRowTranslatorSetDynamic<gum::learning::CellTranslatorCompactIntId> translators;
      translators.insertTranslator ( gum::learning::Col<0> (), 8 );
      
      auto generators1 =  gum::learning::make_generators ( SimpleGenerator () );

      auto generators = generators1;
      
      auto filter1 = gum::learning::make_DB_row_filter ( database, translators,
                                                         generators );

      auto filter = filter1;

      auto filter2 = gum::learning::make_DB_row_filter ( database, translators,
                                                         generators1 );
      
      filter = std::move ( filter2 );
      
      std::vector<unsigned int> modalities = filter.modalities ();
      
      gum::learning::ScoreK2<decltype ( filter ) >
        score ( filter, modalities );

      gum::learning::StructuralConstraintSet<
        gum::learning::StructuralConstraintDAG,
        gum::learning::StructuralConstraintIndegree,
        gum::learning::StructuralConstraint2TimeSlice,
        gum::learning::StructuralConstraintTabuList>
        struct_constraint; 

      struct_constraint.setDefaultIndegree ( 2 );
      struct_constraint.setTabuListSize ( 100 );

      gum::NodeProperty<bool> slices {
        std::make_pair( gum::NodeId ( 0 ), 0 ),
          std::make_pair( gum::NodeId ( 1 ), 0 ) };
      struct_constraint.setSlices ( slices );
      struct_constraint.setDefaultSlice ( 1 );

      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      gum::learning::GraphChangesGenerator4DiGraph
        < decltype ( struct_constraint ) >
        op_set ( struct_constraint );
    
      gum::learning::GraphChangesSelector4DiGraph<
        decltype ( score ),
        decltype ( struct_constraint ),
        gum::learning::GraphChangesGenerator4DiGraph >
      selector ( score, struct_constraint, op_set );
 
    gum::learning::LocalSearchWithTabuList search;

    try {
      gum::BayesNet<float> bn = search.learnBN ( selector, estimator,
                                                 database.variableNames (),
                                                 modalities, 10 );
      gum::BayesNet<double> bn2 =
        search.learnBN<double> ( selector, estimator,
                                 database.variableNames (),
                                 modalities, 10 );
      std::cout << bn << std::endl << bn.dag () << std::endl;
    }
    catch ( gum::Exception& e ) {
      GUM_SHOWERROR ( e ); }
      
  }

    void test_asia2 () {
      gum::learning::LocalSearchWithTabuList search;
       gum::BayesNet<float> bn =
         search.learnBNFromCSV ( GET_PATH_STR( "asia.csv" ) );
       std::cout << bn << std::endl << bn.dag () << std::endl;

       gum::BayesNet<double> bn2 =
         search.learnBNFromCSV<double> ( GET_PATH_STR( "asia.csv" ) );
       std::cout << bn2 << std::endl << bn2.dag () << std::endl;
    }
    

    void test_alarm1 () {
      gum::learning::DatabaseFromCSV database ( MY_ALARM );
      
      auto translators = gum::learning::make_translators
        ( gum::learning::Create<gum::learning::CellTranslatorCompactIntId,
                                gum::learning::Col<0>, 37> () );
     
      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );
      
      std::vector<unsigned int> modalities = filter.modalities ();
      
      gum::learning::ScoreK2<decltype ( filter ) >
        score ( filter, modalities );

      gum::learning::StructuralConstraintSet<
        gum::learning::StructuralConstraintDAG,
        gum::learning::StructuralConstraintIndegree,
        gum::learning::StructuralConstraint2TimeSlice,
        gum::learning::StructuralConstraintTabuList>
        struct_constraint; 
      
      struct_constraint.setDefaultIndegree ( 4 );
      struct_constraint.setTabuListSize ( 100 );
      
      gum::NodeProperty<bool> slices {
        std::make_pair( gum::NodeId ( 0 ), 0 ),
        std::make_pair( gum::NodeId ( 1 ), 0 ) };
      struct_constraint.setSlices ( slices );
      struct_constraint.setDefaultSlice ( 1 );

      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      gum::learning::GraphChangesGenerator4DiGraph
        < decltype ( struct_constraint ) >
        op_set ( struct_constraint );
    
      gum::learning::GraphChangesSelector4DiGraph<
        decltype ( score ),
        decltype ( struct_constraint ),
        gum::learning::GraphChangesGenerator4DiGraph >
      selector ( score, struct_constraint, op_set );
 
      gum::learning::LocalSearchWithTabuList search;

      try {
        gum::Timer timer;
        gum::DAG bn = search.learnStructure ( selector, modalities, 2 );
        std::cout << timer.step () << " : " << std::endl;
        std::cout << bn << std::endl;
      }
      catch ( gum::Exception& e ) {
        GUM_SHOWERROR ( e );
      }
      
    }


    void test_alarm2 () {
      gum::learning::DatabaseFromCSV database ( MY_ALARM );
      
      gum::learning::DBRowTranslatorSetDynamic
        <gum::learning::CellTranslatorCompactIntId> translators;
      translators.insertTranslator ( gum::learning::CellTranslatorCompactIntId(),
                                     gum::learning::Col<0> (), 37 );
      
      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );
      
      std::vector<unsigned int> modalities = filter.modalities ();
      
      gum::learning::ScoreK2<decltype ( filter ) >
        score ( filter, modalities );

      gum::learning::StructuralConstraintSet<
        gum::learning::StructuralConstraintDAG,
        gum::learning::StructuralConstraintIndegree,
        gum::learning::StructuralConstraint2TimeSlice,
        gum::learning::StructuralConstraintTabuList>
        struct_constraint; 
      
      struct_constraint.setDefaultIndegree ( 4 );
      struct_constraint.setTabuListSize ( 100 );
      
      gum::NodeProperty<bool> slices {
        std::make_pair( gum::NodeId ( 0 ), 0 ),
        std::make_pair( gum::NodeId ( 1 ), 0 ) };
      struct_constraint.setSlices ( slices );
      struct_constraint.setDefaultSlice ( 1 );

      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      gum::learning::GraphChangesGenerator4DiGraph
        < decltype ( struct_constraint ) >
        op_set ( struct_constraint );
    
      gum::learning::GraphChangesSelector4DiGraph<
        decltype ( score ),
        decltype ( struct_constraint ),
        gum::learning::GraphChangesGenerator4DiGraph >
      selector ( score, struct_constraint, op_set );
 
      gum::learning::LocalSearchWithTabuList search;

      try {
        gum::Timer timer;
        gum::DAG bn = search.learnStructure ( selector, modalities, 2 );
        std::cout << timer.step () << " : " << std::endl;
        std::cout << bn << std::endl;
      }
      catch ( gum::Exception& e ) {
        GUM_SHOWERROR ( e );
      }
      
    }


    void test_alarm3 () {
      gum::learning::DatabaseFromCSV database ( MY_ALARM );
      
      gum::learning::DBRowTranslatorSetDynamic< gum::learning::DBCellTranslator<1,1> >
        translators;
      translators.insertTranslator ( gum::learning::CellTranslatorCompactIntId (),
                                     gum::learning::Col<0> (), 37 );
      
           
      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );
      
      std::vector<unsigned int> modalities = filter.modalities ();
      
      gum::learning::ScoreK2<decltype ( filter ) >
        score ( filter, modalities );

      gum::learning::StructuralConstraintSet<
        gum::learning::StructuralConstraintDAG,
        gum::learning::StructuralConstraintIndegree,
        gum::learning::StructuralConstraint2TimeSlice,
        gum::learning::StructuralConstraintTabuList>
        struct_constraint; 
      
      struct_constraint.setDefaultIndegree ( 4 );
      struct_constraint.setTabuListSize ( 100 );
      
      gum::NodeProperty<bool> slices {
        std::make_pair( gum::NodeId ( 0 ), 0 ),
        std::make_pair( gum::NodeId ( 1 ), 0 ) };
      struct_constraint.setSlices ( slices );
      struct_constraint.setDefaultSlice ( 1 );

      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      gum::learning::GraphChangesGenerator4DiGraph
        < decltype ( struct_constraint ) >
        op_set ( struct_constraint );
    
      gum::learning::GraphChangesSelector4DiGraph<
        decltype ( score ),
        decltype ( struct_constraint ),
        gum::learning::GraphChangesGenerator4DiGraph >
      selector ( score, struct_constraint, op_set );
 
      gum::learning::LocalSearchWithTabuList search;

      try {
        gum::Timer timer;
        gum::DAG bn = search.learnStructure ( selector, modalities, 2 );
        std::cout << timer.step () << " : " << std::endl;
        std::cout << bn << std::endl;
      }
      catch ( gum::Exception& e ) {
        GUM_SHOWERROR ( e );
      }
      
    }


    
  };


} /* namespace gum_tests */
