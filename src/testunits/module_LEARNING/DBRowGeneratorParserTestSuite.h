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
#include <vector>
#include <ressources/mygenerator.h>
#include <ressources/mygenerator2.h>

#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBTranslator4ContinuousVariable.h>
#include <agrum/learning/database/DBRowGeneratorParser.h>

namespace gum_tests {

  class DBRowGeneratorParserTestSuite : public CxxTest::TestSuite {
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
       return;
      
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
    
  };

  
} /* namespace gum_tests */
