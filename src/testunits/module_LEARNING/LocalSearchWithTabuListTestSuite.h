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
#include <agrum/learning/scores+tests/scoreK2.h>
#include <agrum/learning/scores+tests/scoreBDeu.h>
#include <agrum/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/learning/constraints/structuralConstraintDAG.h>
#include <agrum/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/learning/constraints/structuralConstraint2TimeSlice.h>
#include <agrum/learning/constraints/structuralConstraintTabuList.h>
#include <agrum/learning/constraints/structuralConstraintSet.h>
#include <agrum/learning/graphChangesGeneratorOnceForAll.h>
#include <agrum/learning/localSearchWithTabuList.h>
#include <agrum/learning/paramEstimatorML.h>

namespace gum_tests {

  class LocalSearchWithTabuListTestSuite: public CxxTest::TestSuite {
  public:

    class CellTranslator : public gum::learning::DBCellTranslator<1,1> {
    public:
      void translate () { out (0) = in (0).getFloat (); }
      void initialize () {
        unsigned int nb = in(0).getFloat ();
        if ( ! __values.exists ( nb ) ) __values.insert ( nb );
      }
      void modalities ( std::vector<unsigned int>& modal ) const noexcept {
        modal.push_back ( __values.size () );
      }
      bool requiresInitialization () const noexcept { return true; }
      std::string translateBack ( unsigned int col,
                                  unsigned int translated_val ) {
        std::stringstream str;
        str << translated_val;
        return  str.str ();
      }
      void variableNames ( const std::vector<std::string>& db_var,
                           std::vector<std::string>& output_vars ) const {
        output_vars.push_back ( db_var[_input_cols[0]] );
      }

    private:
      gum::Set<unsigned int> __values;
      
    };

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
      
      auto handler = database.handler ();
      
      auto translators = gum::learning::make_translators
        ( gum::learning::Create<CellTranslator, gum::learning::Col<0>, 8 > () );

      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      
      auto filter = gum::learning::make_DB_row_filter ( handler, translators,
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

      struct_constraint.setDefaultIndegree ( 2 );
      struct_constraint.setTabuListSize ( 100 );

      gum::NodeProperty<bool> slices {
        std::make_pair( gum::NodeId ( 0 ), 0 ),
        std::make_pair( gum::NodeId ( 1 ), 0 ) };
      struct_constraint.setSlices ( slices );
      struct_constraint.setDefaultSlice ( 1 );

      gum::learning::ParamEstimatorML<decltype ( filter ) >
        estimator ( filter, modalities );

      gum::learning::GraphChangesGeneratorOnceForAll
        < decltype ( struct_constraint ) >
        op_set ( struct_constraint );
    
      gum::learning::GraphChangesSelector<
        decltype ( score ),
        decltype ( struct_constraint ),
        gum::learning::GraphChangesGeneratorOnceForAll >
      selector ( score, struct_constraint, op_set );
 
      gum::learning::LocalSearchWithTabuList search;

      try {
        gum::BayesNet<float> bn = search.learnBN ( selector, estimator,
                                                   database.variableNames (),
                                                   modalities, 10 );
        std::cout << bn << std::endl << bn.dag () << std::endl;
      }
      catch ( gum::Exception& e ) {
        GUM_SHOWERROR ( e ); }
      
    }

  };


} /* namespace gum_tests */
