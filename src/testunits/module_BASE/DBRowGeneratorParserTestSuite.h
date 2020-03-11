/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
 *   info_at_agrum_dot_org                                                  *
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
#include <vector>
#include <ressources/mygenerator.h>
#include <ressources/mygenerator2.h>

#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/tools/database/DBTranslator4LabelizedVariable.h>
#include <agrum/tools/database/DBTranslator4ContinuousVariable.h>
#include <agrum/tools/database/DBRowGeneratorParser.h>
#include <agrum/tools/database/DBRowGeneratorEM.h>

namespace gum_tests {

  class DBRowGeneratorParserTestSuite : public CxxTest::TestSuite {
  private:
    gum::Potential<double>
    __infer( const gum::BayesNet<double>& bn,
             const std::vector<std::size_t>& targets,
             const gum::learning::DBRow< gum::learning::DBTranslatedValue >& row) {
      gum::LazyPropagation<double> ve(&bn);
      
      gum::NodeSet target_set;
      for (auto target : targets) target_set.insert(gum::NodeId(target));
      ve.addJointTarget(target_set);

      const auto xrow = row.row();
      const auto row_size = xrow.size();
      for (std::size_t col = std::size_t(0); col < row_size; ++col) {
        if ( xrow[col].discr_val != std::numeric_limits<std::size_t>::max() ) {
          ve.addEvidence(gum::NodeId(col), xrow[col].discr_val);
        }
      }

      gum::Potential<double> prob = ve.jointPosterior(target_set);
      return prob;
    }

    
    public:
    void test_simple() {
      gum::learning::DBTranslator4LabelizedVariable<>  translator_lab;
      gum::learning::DBTranslator4ContinuousVariable<> translator_cont;
      gum::learning::DBTranslatorSet<> set;
      set.insertTranslator ( translator_lab,  0 );
      set.insertTranslator ( translator_lab,  1 );
      set.insertTranslator ( translator_cont, 2 );
      set.insertTranslator ( translator_lab,  3 );
      set[0].setVariableName ( "v0" );
      set[1].setVariableName ( "v1" );
      set[2].setVariableName ( "v2" );
      set[3].setVariableName ( "v3" );

      gum::learning::DatabaseTable<> database ( set );
      std::vector<std::string> row { "A0", "B0", "3.003", "C0" };
      database.insertRow( row );

      row[0] = "A1";
      row[1] = "B1";
      row[2] = "3.113";
      row[3] = "C1";
      database.insertRow( row );

      row[0] = "A2";
      row[1] = "B2";
      row[2] = "3.223";
      row[3] = "C2";
      database.insertRow( row );

      row[0] = "A3";
      row[1] = "B3";
      row[2] = "3.333";
      row[3] = "C3";
      database.insertRow( row );

      const std::vector<gum::learning::DBTranslatedValueType>
        col_types { gum::learning::DBTranslatedValueType::DISCRETE,
                    gum::learning::DBTranslatedValueType::DISCRETE,
                    gum::learning::DBTranslatedValueType::CONTINUOUS,
                    gum::learning::DBTranslatedValueType::DISCRETE };

      gum::learning::MyGenerator<>  generator1 ( col_types, 6 );
      gum::learning::MyGenerator2<> generator2 ( col_types, 4 );
      gum::learning::DBRowGeneratorSet<> genset;
      genset.insertGenerator ( generator1 );
      genset.insertGenerator ( generator2 );

      gum::learning::DBRowGeneratorParser<>
        parser ( database.handler (), genset );

      std::size_t nb_rows = std::size_t(0);
      while ( parser.hasRows () ) {
        const auto& row = parser.row().row();
        TS_ASSERT ( row[0].discr_val == nb_rows / 12 );
        TS_ASSERT ( row[1].discr_val == nb_rows / 12 );
        TS_ASSERT ( row[2].cont_val  == 3.003f + (nb_rows / 12) * 11 / 100.0f );
        TS_ASSERT ( row[3].discr_val == nb_rows / 12 );
        ++nb_rows;
      }
      TS_ASSERT ( nb_rows == std::size_t( 48 ) );

      nb_rows = std::size_t(0);
      while ( parser.hasRows () ) {
        parser.row().row();
         ++nb_rows;
      }
      TS_ASSERT ( nb_rows == std::size_t( 0 ) );

      parser.reset ();
      nb_rows = std::size_t(0);
      while ( parser.hasRows () ) {
        parser.row().row();
         ++nb_rows;
      }
      TS_ASSERT ( nb_rows == std::size_t( 48 ) );

      TS_ASSERT ( ( parser.handler().range() ==
                    std::make_pair<std::size_t,std::size_t> (0,4) ) );

      const auto& xgenset = parser.generatorSet ();
      TS_ASSERT ( xgenset.nbGenerators() == std::size_t(2) );


      const std::vector<std::size_t> cols_of_interest { std::size_t(0),
                                                        std::size_t(2),
                                                        std::size_t(3) };
      parser.setColumnsOfInterest ( cols_of_interest );
      parser.reset ();
      const auto& cols0 = xgenset[0].columnsOfInterest ();
      const auto& cols1 = xgenset[1].columnsOfInterest ();
      TS_ASSERT ( cols_of_interest == cols0 );
      TS_ASSERT ( cols_of_interest == cols1 );

      gum::learning::DBRowGeneratorParser<> parser2 ( parser );
      parser2.reset ();
      nb_rows = std::size_t(0);
      while ( parser2.hasRows () ) {
        const auto& row = parser2.row().row();
        TS_ASSERT ( row[0].discr_val == nb_rows / 12 );
        TS_ASSERT ( row[1].discr_val == nb_rows / 12 );
        TS_ASSERT ( row[2].cont_val  == 3.003f + (nb_rows / 12) * 11 / 100.0f );
        TS_ASSERT ( row[3].discr_val == nb_rows / 12 );
         ++nb_rows;
      }
      TS_ASSERT ( nb_rows == std::size_t( 48 ) );
      const auto& ygenset = parser2.generatorSet ();
      const auto& ycols0  = ygenset[0].columnsOfInterest ();
      const auto& ycols1  = ygenset[1].columnsOfInterest ();
      TS_ASSERT ( cols_of_interest == ycols0 );
      TS_ASSERT ( cols_of_interest == ycols1 );

      gum::learning::DBRowGeneratorParser<>
        parser3 ( parser, std::allocator<gum::learning::DBTranslatedValue> () );
      parser3.reset ();
      nb_rows = std::size_t(0);
      while ( parser3.hasRows () ) {
        const auto& row = parser3.row().row();
        TS_ASSERT ( row[0].discr_val == nb_rows / 12 );
        TS_ASSERT ( row[1].discr_val == nb_rows / 12 );
        TS_ASSERT ( row[2].cont_val  == 3.003f + (nb_rows / 12) * 11 / 100.0f );
        TS_ASSERT ( row[3].discr_val == nb_rows / 12 );
         ++nb_rows;
      }
      TS_ASSERT ( nb_rows == std::size_t( 48 ) );
      const auto& zgenset = parser3.generatorSet ();
      const auto& zcols0  = zgenset[0].columnsOfInterest ();
      const auto& zcols1  = zgenset[1].columnsOfInterest ();
      TS_ASSERT ( cols_of_interest == zcols0 );
      TS_ASSERT ( cols_of_interest == zcols1 );
      
      gum::learning::DBRowGeneratorParser<>
        parser4 ( std::move ( parser3 ),
                  std::allocator<gum::learning::DBTranslatedValue> () );
      parser4.reset ();
      nb_rows = std::size_t(0);
      while ( parser4.hasRows () ) {
        const auto& row = parser4.row().row();
        TS_ASSERT ( row[0].discr_val == nb_rows / 12 );
        TS_ASSERT ( row[1].discr_val == nb_rows / 12 );
        TS_ASSERT ( row[2].cont_val  == 3.003f + (nb_rows / 12) * 11 / 100.0f );
        TS_ASSERT ( row[3].discr_val == nb_rows / 12 );
         ++nb_rows;
      }
      TS_ASSERT ( nb_rows == std::size_t( 48 ) );
      const auto& pgenset = parser4.generatorSet ();
      const auto& pcols0  = pgenset[0].columnsOfInterest ();
      const auto& pcols1  = pgenset[1].columnsOfInterest ();
      TS_ASSERT ( cols_of_interest == pcols0 );
      TS_ASSERT ( cols_of_interest == pcols1 );

      gum::learning::DBRowGeneratorParser<> parser5 ( std::move ( parser4 ) );
      parser5.reset ();
      nb_rows = std::size_t(0);
      while ( parser5.hasRows () ) {
        const auto& row = parser5.row().row();
        TS_ASSERT ( row[0].discr_val == nb_rows / 12 );
        TS_ASSERT ( row[1].discr_val == nb_rows / 12 );
        TS_ASSERT ( row[2].cont_val  == 3.003f + (nb_rows / 12) * 11 / 100.0f );
        TS_ASSERT ( row[3].discr_val == nb_rows / 12 );
         ++nb_rows;
      }
      TS_ASSERT ( nb_rows == std::size_t( 48 ) );
      const auto& qgenset = parser5.generatorSet ();
      const auto& qcols0  = qgenset[0].columnsOfInterest ();
      const auto& qcols1  = qgenset[1].columnsOfInterest ();
      TS_ASSERT ( cols_of_interest == qcols0 );
      TS_ASSERT ( cols_of_interest == qcols1 );
      
      gum::learning::DBRowGeneratorParser<>* parser6 = parser.clone ();
      parser6->reset ();
      nb_rows = std::size_t(0);
      while ( parser6->hasRows () ) {
        const auto& row = parser6->row().row();
        TS_ASSERT ( row[0].discr_val == nb_rows / 12 );
        TS_ASSERT ( row[1].discr_val == nb_rows / 12 );
        TS_ASSERT ( row[2].cont_val  == 3.003f + (nb_rows / 12) * 11 / 100.0f );
        TS_ASSERT ( row[3].discr_val == nb_rows / 12 );
         ++nb_rows;
      }
      TS_ASSERT ( nb_rows == std::size_t( 48 ) );
      const auto& rgenset = parser6->generatorSet ();
      const auto& rcols0  = rgenset[0].columnsOfInterest ();
      const auto& rcols1  = rgenset[1].columnsOfInterest ();
      TS_ASSERT ( cols_of_interest == rcols0 );
      TS_ASSERT ( cols_of_interest == rcols1 );

      delete parser6;

      gum::learning::DBRowGeneratorParser<>* parser7 =
        parser.clone (std::allocator<gum::learning::DBTranslatedValue>());
      parser7->reset ();
      nb_rows = std::size_t(0);
      while ( parser7->hasRows () ) {
        const auto& row = parser7->row().row();
        TS_ASSERT ( row[0].discr_val == nb_rows / 12 );
        TS_ASSERT ( row[1].discr_val == nb_rows / 12 );
        TS_ASSERT ( row[2].cont_val  == 3.003f + (nb_rows / 12) * 11 / 100.0f );
        TS_ASSERT ( row[3].discr_val == nb_rows / 12 );
         ++nb_rows;
      }
      TS_ASSERT ( nb_rows == std::size_t( 48 ) );
      const auto& sgenset = parser7->generatorSet ();
      const auto& scols0  = sgenset[0].columnsOfInterest ();
      const auto& scols1  = sgenset[1].columnsOfInterest ();
      TS_ASSERT ( cols_of_interest == scols0 );
      TS_ASSERT ( cols_of_interest == scols1 );

      delete parser7;

      gum::learning::DBRowGeneratorParser<>
        parser8( database.handler (), gum::learning::DBRowGeneratorSet<> () );
      nb_rows = std::size_t(0);
      while ( parser8.hasRows () ) {
        const auto& row = parser8.row().row();
        TS_ASSERT ( row[0].discr_val == nb_rows );
        TS_ASSERT ( row[1].discr_val == nb_rows );
        TS_ASSERT ( row[2].cont_val  == 3.003f + (nb_rows * 11) / 100.0f );
        TS_ASSERT ( row[3].discr_val == nb_rows );
        ++nb_rows;
      }
      TS_ASSERT ( nb_rows == std::size_t( 4 ) );
      const auto& tgenset = parser8.generatorSet ();
      TS_ASSERT( tgenset.size() == std::size_t(0) );

      gum::learning::DBRowGeneratorParser<> parser9 ( parser8 ); 
      parser9.reset ();
      nb_rows = std::size_t(0);
      while ( parser9.hasRows () ) {
        const auto& row = parser9.row().row();
        TS_ASSERT ( row[0].discr_val == nb_rows );
        TS_ASSERT ( row[1].discr_val == nb_rows );
        TS_ASSERT ( row[2].cont_val  == 3.003f + (nb_rows * 11) / 100.0f );
        TS_ASSERT ( row[3].discr_val == nb_rows );
        ++nb_rows;
      }
      TS_ASSERT ( nb_rows == std::size_t( 4 ) );
      TS_ASSERT( parser9.generatorSet().size() == std::size_t(0) );

      parser8 = parser2;
      parser8.reset ();
      nb_rows = std::size_t(0);
      while ( parser8.hasRows () ) {
        const auto& row = parser8.row().row();
        TS_ASSERT ( row[0].discr_val == nb_rows / 12 );
        TS_ASSERT ( row[1].discr_val == nb_rows / 12 );
        TS_ASSERT ( row[2].cont_val  == 3.003f + (nb_rows / 12) * 11 / 100.0f );
        TS_ASSERT ( row[3].discr_val == nb_rows / 12 );
         ++nb_rows;
      }
      TS_ASSERT ( nb_rows == std::size_t( 48 ) );
      const auto& agenset = parser8.generatorSet ();
      const auto& acols0  = agenset[0].columnsOfInterest ();
      const auto& acols1  = agenset[1].columnsOfInterest ();
      TS_ASSERT ( cols_of_interest == acols0 );
      TS_ASSERT ( cols_of_interest == acols1 );

      parser2 = std::move ( parser9 );
      
      parser2.reset ();
      nb_rows = std::size_t(0);
      while ( parser2.hasRows () ) {
        const auto& row = parser2.row().row();
        TS_ASSERT ( row[0].discr_val == nb_rows );
        TS_ASSERT ( row[1].discr_val == nb_rows );
        TS_ASSERT ( row[2].cont_val  == 3.003f + (nb_rows * 11) / 100.0f );
        TS_ASSERT ( row[3].discr_val == nb_rows );
        ++nb_rows;
      }
      TS_ASSERT ( nb_rows == std::size_t( 4 ) );
      TS_ASSERT( parser2.generatorSet().size() == std::size_t(0) );

    }


    void testEM () {
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

      gum::LabelizedVariable var("x", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      const std::vector<std::string> miss {"N/A","?"};
      gum::learning::DBTranslator4LabelizedVariable<> translator(var,miss);
      gum::learning::DBTranslatorSet<> set;
      for ( std::size_t i = std::size_t(0); i < std::size_t(4); ++i)
        set.insertTranslator ( translator, i );

      set[0].setVariableName ( "v0" );
      set[1].setVariableName ( "v1" );
      set[2].setVariableName ( "v2" );
      set[3].setVariableName ( "v3" );

      gum::learning::DatabaseTable<> database ( set );
      std::vector<std::string> row1 { "0", "1", "1", "0" };
      std::vector<std::string> row2 { "0", "?", "1", "0" };
      std::vector<std::string> row3 { "0", "?", "?", "0" };
      std::vector<std::string> row4 { "?", "?", "1", "0" };
      std::vector<std::string> row5 { "?", "?", "?", "?" };
      database.insertRow( row1 );
      database.insertRow( row2 );
      database.insertRow( row3 );
      database.insertRow( row4 );
      database.insertRow( row5 );

      auto handler = database.handler();

      gum::learning::DBRowGeneratorIdentity<> generator1(col_types);
      gum::learning::DBRowGeneratorEM<>       generator2(col_types,bn0);
      gum::learning::DBRowGenerator<>&        gen2 = generator2; // fix for g++-4.8
      gum::learning::DBRowGeneratorIdentity<> generator3(col_types);
      gum::learning::DBRowGeneratorEM<>       generator4(col_types,bn0);
      gum::learning::DBRowGenerator<>&        gen4 = generator4; // fix for g++-4.8
     
      gum::learning::DBRowGeneratorSet<> genset;
      genset.insertGenerator(generator1);
      genset.insertGenerator(gen2);
      genset.insertGenerator(generator3);
      genset.insertGenerator(gen4);

      gum::learning::DBRowGeneratorParser<>
        parser ( database.handler (), genset );

      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D");
      bn.cpt("A").fillWith({0.3, 0.7});
      bn.cpt("B").fillWith({0.4, 0.6, 0.7, 0.3});
      bn.cpt("C").fillWith({0.2, 0.8, 0.3, 0.7, 0.4, 0.6, 0.5, 0.5});
      bn.cpt("D").fillWith({0.8, 0.2});

      parser.setBayesNet(bn);
      
      const std::vector< std::size_t > cols_of_interest{std::size_t(0),
                                                        std::size_t(1)};

      parser.setColumnsOfInterest(cols_of_interest);
      TS_ASSERT(parser.hasRows());
      {
        const auto& row = parser.row();
        const auto& xrow = row.row();

        TS_ASSERT_EQUALS(row.weight(), 1.0);
        TS_ASSERT_EQUALS(xrow[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xrow[1].discr_val, std::size_t(1));
      }

      for (int i = 0; i < 2; ++i ) {
        ++handler;
        TS_ASSERT(parser.hasRows());
      
        gum::Potential<double> proba = __infer(bn, {std::size_t(1)},
                                               handler.row());
        gum::Instantiation inst(proba);

        const auto& fill_row1  = parser.row();
        const auto& xfill_row1 = fill_row1.row();
        TS_ASSERT_EQUALS(xfill_row1[0].discr_val,std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row1[1].discr_val, std::size_t(0));
        TS_ASSERT_DELTA(fill_row1.weight(), proba.get(inst),0.001);

        ++inst;
        const auto& fill_row2  = parser.row();
        const auto& xfill_row2 = fill_row2.row();
        TS_ASSERT_EQUALS(xfill_row2[0].discr_val, std::size_t(0));
        TS_ASSERT_EQUALS(xfill_row2[1].discr_val, std::size_t(1));
        TS_ASSERT_DELTA(fill_row2.weight(), proba.get(inst), 0.001);
      }

      for (int i = 0; i < 2; ++i ) {
        ++handler;
        TS_ASSERT(parser.hasRows());
      
        gum::Potential<double> proba = __infer(bn, {std::size_t(0),std::size_t(1)},
                                               handler.row());

        std::vector<double> xproba (4);
        std::vector<bool> observed(4, false);
        std::size_t idx;
       for (gum::Instantiation inst(proba); !inst.end(); ++inst) {
         if ( proba.variablesSequence()[0]->name() == "A")
           idx = inst.val(0) + std::size_t(2) * inst.val(1);
         else
           idx = inst.val(1) + std::size_t(2) * inst.val(0);
          xproba[idx] = proba.get(inst);
        }
        
        const auto& fill_row1  = parser.row();
        const auto& xfill_row1 = fill_row1.row();
        idx = xfill_row1[0].discr_val + std::size_t(2) * xfill_row1[1].discr_val;
        observed[idx] = true;
        TS_ASSERT_DELTA(fill_row1.weight(), xproba[idx], 0.001);

        const auto& fill_row2  = parser.row();
        const auto& xfill_row2 = fill_row2.row();
        idx = xfill_row2[0].discr_val + std::size_t(2) * xfill_row2[1].discr_val;
        observed[idx] = true;
        TS_ASSERT_DELTA(fill_row2.weight(), xproba[idx], 0.001);

        const auto& fill_row3  = parser.row();
        const auto& xfill_row3 = fill_row3.row();
        idx = xfill_row3[0].discr_val + std::size_t(2) * xfill_row3[1].discr_val;
        observed[idx] = true;
        TS_ASSERT_DELTA(fill_row3.weight(), xproba[idx],0.001);

        const auto& fill_row4  = parser.row();
        const auto& xfill_row4 = fill_row4.row();
        idx = xfill_row4[0].discr_val + std::size_t(2) * xfill_row4[1].discr_val;
        observed[idx] = true;
        TS_ASSERT_DELTA(fill_row4.weight(), xproba[idx],0.001);

        int nb_observed = 0;
        for ( auto obs : observed)
          if (obs) ++nb_observed;
        TS_ASSERT_EQUALS(nb_observed, 4);
      }
      
    }
    
  };

  
} /* namespace gum_tests */
