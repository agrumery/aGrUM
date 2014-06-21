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
#include <agrum/learning/database/DBCellTranslators/cellTranslatorNumber.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorString.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorUniversal.h>
#include <agrum/learning/scores_and_tests/scoreK2.h>
#include <agrum/learning/scores_and_tests/scoreBDeu.h>
#include <agrum/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/learning/constraints/structuralConstraintDAG.h>
#include <agrum/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/learning/constraints/structuralConstraint2TimeSlice.h>
#include <agrum/learning/constraints/structuralConstraintSet.h>
#include <agrum/learning/graphChangesGenerator.h>
#include <agrum/learning/graphChangesGeneratorOnSubGraph.h>
#include <agrum/learning/greedyHillClimbing.h>
#include <agrum/learning/paramEstimatorML.h>

#define MY_ALARM GET_PATH_STR( "alarm.csv" )

namespace gum_tests {

  class GreedyHillClimbingTestSuite: public CxxTest::TestSuite {
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

      gum::learning::ScoreK2<decltype ( filter ) >
        score ( filter, modalities );

      gum::learning::StructuralConstraintSet<
        gum::learning::StructuralConstraintDAG,
        gum::learning::StructuralConstraintIndegree,
        gum::learning::StructuralConstraint2TimeSlice>
        struct_constraint; 

      struct_constraint.setDefaultIndegree ( 2 );

      gum::NodeProperty<bool> slices {
        std::make_pair( gum::NodeId ( 0 ), 0 ),
        std::make_pair( gum::NodeId ( 1 ), 0 ) };
      struct_constraint.setSlices ( slices );
      struct_constraint.setDefaultSlice ( 1 );

      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      gum::learning::GraphChangesGenerator
        < decltype ( struct_constraint ) >
        op_set ( struct_constraint );
    
      gum::learning::GraphChangesSelector<
        decltype ( score ),
        decltype ( struct_constraint ),
        gum::learning::GraphChangesGenerator >
      selector ( score, struct_constraint, op_set );
 
      gum::learning::GreedyHillClimbing search;
 
      gum::BayesNet<double> bn =
        search.learnBN<double> ( selector, estimator,
                                 database.variableNames (),
                                 modalities );

      gum::BayesNet<float> bn2 =
        search.learnBN ( selector, estimator,
                         database.variableNames (),
                         modalities );

      std::cout << bn << std::endl << bn.dag () << std::endl;
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
      
      gum::learning::ScoreBDeu<decltype ( filter ) >
        score ( filter, modalities );

      gum::learning::StructuralConstraintSet<
        gum::learning::StructuralConstraintDAG>
        struct_constraint; 
      
      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      gum::learning::GraphChangesGenerator
        < decltype ( struct_constraint ) >
        op_set ( struct_constraint );
    
      gum::learning::GraphChangesSelector<
        decltype ( score ),
        decltype ( struct_constraint ),
        gum::learning::GraphChangesGenerator >
      selector ( score, struct_constraint, op_set );
 
      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG bn = search.learnStructure ( selector, modalities );
        std::cout << timer.step () << " : " << std::endl;
        std::cout << bn << std::endl;
      }
      catch ( gum::Exception& e ) {
        GUM_SHOWERROR ( e );
      }
      
    }

    
    void test_alarm1bis () {
      gum::learning::DatabaseFromCSV database ( MY_ALARM );
      
      auto translators = gum::learning::make_translators
        ( gum::learning::Create<gum::learning::CellTranslatorNumber,
                                gum::learning::Col<0>, 37> () );
     
      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );
      
      std::vector<unsigned int> modalities = filter.modalities ();
      
      gum::learning::ScoreBDeu<decltype ( filter ) >
        score ( filter, modalities );

      gum::learning::StructuralConstraintSet<
        gum::learning::StructuralConstraintDAG>
        struct_constraint; 
      
      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      gum::learning::GraphChangesGenerator
        < decltype ( struct_constraint ) >
        op_set ( struct_constraint );
    
      gum::learning::GraphChangesSelector<
        decltype ( score ),
        decltype ( struct_constraint ),
        gum::learning::GraphChangesGenerator >
      selector ( score, struct_constraint, op_set );
 
      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG bn = search.learnStructure ( selector, modalities );
        std::cout << timer.step () << " : " << std::endl;
        std::cout << bn << std::endl;
      }
      catch ( gum::Exception& e ) {
        GUM_SHOWERROR ( e );
      }
      
    }

    
    void test_alarm1ter () {
      gum::learning::DatabaseFromCSV database ( MY_ALARM );
      
      auto translators = gum::learning::make_translators
        ( gum::learning::Create<gum::learning::CellTranslatorUniversal,
                                gum::learning::Col<0>, 37> () );
     
      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );
      
      std::vector<unsigned int> modalities = filter.modalities ();
      
      gum::learning::ScoreBDeu<decltype ( filter ) >
        score ( filter, modalities );

      gum::learning::StructuralConstraintSet<
        gum::learning::StructuralConstraintDAG>
        struct_constraint; 
      
      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      gum::learning::GraphChangesGenerator
        < decltype ( struct_constraint ) >
        op_set ( struct_constraint );
    
      gum::learning::GraphChangesSelector<
        decltype ( score ),
        decltype ( struct_constraint ),
        gum::learning::GraphChangesGenerator >
      selector ( score, struct_constraint, op_set );
 
      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG bn = search.learnStructure ( selector, modalities );
        std::cout << timer.step () << " : " << std::endl;
        std::cout << bn << std::endl;
      }
      catch ( gum::Exception& e ) {
        GUM_SHOWERROR ( e );
      }
      
    }


    void test_alarm2 () {
      gum::learning::DatabaseFromCSV database ( MY_ALARM );
      
      gum::learning::DBRowTranslatorVector
        <gum::learning::CellTranslatorCompactIntId> translators;
      translators.insertTranslator ( gum::learning::CellTranslatorCompactIntId(),
                                     gum::learning::Col<0> (), 37 );
      
      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );
      
      std::vector<unsigned int> modalities = filter.modalities ();
      
      gum::learning::ScoreBDeu<decltype ( filter ) >
        score ( filter, modalities );

      gum::learning::StructuralConstraintSet<
        gum::learning::StructuralConstraintDAG>
        struct_constraint; 
      
      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      gum::learning::GraphChangesGenerator
        < decltype ( struct_constraint ) >
        op_set ( struct_constraint );
    
      gum::learning::GraphChangesSelector<
        decltype ( score ),
        decltype ( struct_constraint ),
        gum::learning::GraphChangesGenerator >
      selector ( score, struct_constraint, op_set );
 
      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG bn = search.learnStructure ( selector, modalities );
        std::cout << timer.step () << " : " << std::endl;
        std::cout << bn << std::endl;
      }
      catch ( gum::Exception& e ) {
        GUM_SHOWERROR ( e );
      }
      
    }


    void test_alarm3 () {
      gum::learning::DatabaseFromCSV database ( MY_ALARM );
      
      gum::learning::DBRowTranslatorVector< gum::learning::DBCellTranslator<1,1> >
        translators;
      translators.insertTranslator ( gum::learning::CellTranslatorCompactIntId (),
                                     gum::learning::Col<0> (),
                                     database.content()[0].size () );
      
           
      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );
      
      std::vector<unsigned int> modalities = filter.modalities ();
      
      gum::learning::ScoreBDeu<decltype ( filter ) >
        score ( filter, modalities );

      gum::learning::StructuralConstraintSet<
        gum::learning::StructuralConstraintDAG>
        struct_constraint; 
      
      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      gum::learning::GraphChangesGenerator
        < decltype ( struct_constraint ) >
        op_set ( struct_constraint );
    
      gum::learning::GraphChangesSelector<
        decltype ( score ),
        decltype ( struct_constraint ),
        gum::learning::GraphChangesGenerator >
      selector ( score, struct_constraint, op_set );
 
      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG bn = search.learnStructure ( selector, modalities );
        std::cout << timer.step () << " : " << std::endl;
        std::cout << bn << std::endl;
      }
      catch ( gum::Exception& e ) {
        GUM_SHOWERROR ( e );
      }
      
    }

    
    void test_alarm4 () {
      gum::learning::DatabaseFromCSV database ( MY_ALARM );
      
      gum::learning::DBRowTranslatorVector< gum::learning::DBCellTranslator<1,1> >
        translators;
      translators.insertTranslator ( gum::learning::CellTranslatorCompactIntId (),
                                     gum::learning::Col<0> (),
                                     database.content()[0].size () );
      
           
      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );
      
      std::vector<unsigned int> modalities = filter.modalities ();
      
      gum::learning::ScoreBDeu<decltype ( filter ) >
        score ( filter, modalities );

      gum::learning::StructuralConstraintSet<
        gum::learning::StructuralConstraintDAG>
        struct_constraint; 
      
      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      gum::learning::GraphChangesGeneratorOnSubGraph
        < decltype ( struct_constraint ) >
        op_set ( struct_constraint );

      gum::NodeSet targets { 0, 1, 2 };
      op_set.setTargets ( targets );
      op_set.setTails ( modalities.size () );
    
      gum::learning::GraphChangesSelector<
        decltype ( score ),
        decltype ( struct_constraint ),
        gum::learning::GraphChangesGeneratorOnSubGraph >
      selector ( score, struct_constraint, op_set );
 
      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG bn = search.learnStructure ( selector, modalities );
        std::cout << timer.step () << " : " << std::endl;
        std::cout << bn << std::endl;
      }
      catch ( gum::Exception& e ) {
        GUM_SHOWERROR ( e );
      }
      
    }



  };


} /* namespace gum_tests */
