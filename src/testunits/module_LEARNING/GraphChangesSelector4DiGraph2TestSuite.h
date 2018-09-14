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
#include <sstream>

#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBTranslatorSet.h>
#include <agrum/learning/aprioris/aprioriSmoothing2.h>
#include <agrum/learning/scores_and_tests/scoreBIC2.h>
#include <agrum/learning/structureUtils/graphChangesSelector4DiGraph2.h>

#include <agrum/learning/constraints/structuralConstraintDAG.h>
#include <agrum/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/learning/structureUtils/graphChangesGenerator4DiGraph.h>

namespace gum_tests {

  class GraphChangesSelector4DiGraph2TestSuite : public CxxTest::TestSuite {
    public:
    void test_bic() {
      // create the translator set
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      var.addLabel("2");

      gum::learning::DBTranslatorSet<> trans_set;
      {
        const std::vector< std::string >                miss;
        gum::learning::DBTranslator4LabelizedVariable<> translator(var, miss);
        std::vector< std::string > names{"A", "B", "C", "D", "E", "F"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      // create the database
      gum::learning::DatabaseTable<> database(trans_set);
      std::vector< std::string >     row0{"0", "1", "0", "2", "1", "1"};
      std::vector< std::string >     row1{"1", "2", "0", "1", "2", "2"};
      std::vector< std::string >     row2{"2", "1", "0", "1", "1", "0"};
      std::vector< std::string >     row3{"1", "0", "0", "0", "0", "0"};
      std::vector< std::string >     row4{"0", "0", "0", "1", "1", "1"};
      for (int i = 0; i < 1000; ++i)
        database.insertRow(row0);
      for (int i = 0; i < 50; ++i)
        database.insertRow(row1);
      for (int i = 0; i < 75; ++i)
        database.insertRow(row2);
      for (int i = 0; i < 75; ++i)
        database.insertRow(row3);
      for (int i = 0; i < 200; ++i)
        database.insertRow(row4);

      // create the parser
      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);

      gum::learning::AprioriSmoothing2<> apriori(database);
      gum::learning::ScoreBIC2<>         score(parser, apriori);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG> struct_constraint;

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
        op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph2< decltype(struct_constraint),
                                                    decltype(op_set) >
        selector(score, struct_constraint, op_set);

      gum::DAG graph;
      selector.setGraph ( graph );

      TS_ASSERT ( ! selector.empty () );
      for ( const auto node : graph ) {
        TS_ASSERT ( ! selector.empty ( node ) );
      }

      gum::learning::GraphChange
        change ( gum::learning::GraphChangeType::ARC_DELETION, 0, 1);
      TS_ASSERT ( ! selector.isChangeValid ( change ) );

      for ( const auto node : graph ) {
        const auto& change = selector.bestChange ( node );
        TS_ASSERT ( change.type() == gum::learning::GraphChangeType::ARC_ADDITION);
      }
      TS_ASSERT ( selector.bestChange().type() ==
                  gum::learning::GraphChangeType::ARC_ADDITION);

      const double best_score = selector.bestScore ();
      gum::NodeProperty<double> scores;
      for ( const auto node : graph ) {
        const double sc = selector.bestScore ( node );
        scores.insert ( node, sc);
        TS_ASSERT ( sc <= best_score );
      }

      gum::learning::GraphChange
        change2 ( gum::learning::GraphChangeType::ARC_ADDITION, 3, 1);
      selector.applyChangeWithoutScoreUpdate( change2 );
      selector.updateScoresAfterAppliedChanges();

      for ( const auto node : graph ) {
        if ( ( node != 1 ) && ( node != 3 ) ) {
          TS_ASSERT ( selector.bestScore ( node ) == scores[node] );
        }
        else {
          TS_ASSERT ( selector.bestScore ( node ) != scores[node] );
        }
      }

      scores[1] = selector.bestScore ( 1 );
      scores[3] = selector.bestScore ( 3 );
      gum::learning::GraphChange
        change3 ( gum::learning::GraphChangeType::ARC_ADDITION, 3, 2);
      selector.applyChange(change3);
      for ( const auto node : graph ) {
        if ( ( node != 2 ) ) {
          TS_ASSERT ( selector.bestScore ( node ) == scores[node] );
        }
        else {
          TS_ASSERT ( selector.bestScore ( node ) != scores[node] );
        }
      }

      const auto xnodes = selector.nodesSortedByBestScore();
      TS_ASSERT ( xnodes.size () == std::size_t(6) );
    }

  };


} /* namespace gum_tests */
