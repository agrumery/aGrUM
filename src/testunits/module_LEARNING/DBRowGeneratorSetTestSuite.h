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
#include <ressources/mygenerator.h>
#include <ressources/mygenerator2.h>
#include <iostream>

#include <agrum/learning/database/DBRowGeneratorSet.h>

namespace gum_tests {

  class DBRowGeneratorSetTestSuite : public CxxTest::TestSuite {
  public:
    void test_simple() {
      const std::vector<gum::learning::DBTranslatedValueType>
        col_types { gum::learning::DBTranslatedValueType::DISCRETE,
                    gum::learning::DBTranslatedValueType::DISCRETE,
                    gum::learning::DBTranslatedValueType::CONTINUOUS,
                    gum::learning::DBTranslatedValueType::DISCRETE };

      gum::learning::MyGenerator<> generator1 ( col_types, 3 );
      TS_ASSERT ( ! generator1.hasRows() );
      gum::learning::MyGenerator<> generator2 ( col_types, 4 );
      TS_ASSERT ( ! generator2.hasRows() );
      
      gum::learning::DBRowGeneratorSet<> genset;
      genset.insertGenerator ( generator1 );
      genset.insertGenerator ( std::move ( generator2 ) );
      TS_ASSERT ( genset.nbGenerators() == std::size_t(2) );
      TS_ASSERT ( genset.size() == std::size_t(2) );
      TS_ASSERT ( ! genset.hasRows() );
      
      const gum::learning::DBRow<gum::learning::DBTranslatedValue>
        input_row1 { gum::learning::DBTranslatedValue { std::size_t(0) },
                     gum::learning::DBTranslatedValue { std::size_t(4) },
                     gum::learning::DBTranslatedValue { 4.5f },
                     gum::learning::DBTranslatedValue { std::size_t(7) } };

      genset.setInputRow( input_row1 );
      std::size_t nb_dup = std::size_t(0);
      while ( genset.hasRows () ) {
        const auto& row = genset.generate ().row ();
        ++nb_dup;

        TS_ASSERT ( row[0].discr_val == std::size_t(0) );
        TS_ASSERT ( row[1].discr_val == std::size_t(4) );
        TS_ASSERT ( row[2].cont_val  == 4.5f );
        TS_ASSERT ( row[3].discr_val == std::size_t(7) );
      }

      TS_ASSERT ( nb_dup == std::size_t (12) );

      genset.reset ();
      TS_ASSERT ( ! genset.hasRows() );

      const std::vector<std::size_t> cols_of_interest { std::size_t(0),
                                                        std::size_t(2),
                                                        std::size_t(3) };
      genset.setColumnsOfInterest ( cols_of_interest );
      genset.setInputRow( input_row1 );
      nb_dup = std::size_t(0);
      while ( genset.hasRows () ) {
        const auto& row = genset.generate ().row ();
        ++nb_dup;

        TS_ASSERT ( row[0].discr_val == std::size_t(0) );
        TS_ASSERT ( row[2].cont_val  == 4.5f );
        TS_ASSERT ( row[3].discr_val == std::size_t(7) );
      }
      TS_ASSERT ( nb_dup == std::size_t (12) );

      genset.setInputRow( input_row1 );
      TS_ASSERT_THROWS( genset.setColumnsOfInterest ( cols_of_interest ),
                        gum::OperationNotAllowed );
      genset.reset ();

      genset.setColumnsOfInterest ( std::move ( cols_of_interest ) );
      genset.setInputRow( input_row1 );
      nb_dup = std::size_t(0);
      while ( genset.hasRows () ) {
        const auto& row = genset.generate ().row ();
        ++nb_dup;

        TS_ASSERT ( row[0].discr_val == std::size_t(0) );
        TS_ASSERT ( row[2].cont_val  == 4.5f );
        TS_ASSERT ( row[3].discr_val == std::size_t(7) );
      }
      TS_ASSERT ( nb_dup == std::size_t (12) );

      const auto& cols = genset.columnsOfInterest ();
      TS_ASSERT ( cols[0] == std::size_t(0) );
      TS_ASSERT ( cols[1] == std::size_t(2) );
      TS_ASSERT ( cols[2] == std::size_t(3) );


      gum::learning::DBRowGeneratorSet<> genset2 ( genset );
      genset2.setInputRow( input_row1 );
      nb_dup = std::size_t(0);
      while ( genset2.hasRows () ) {
        const auto& row = genset2.generate ().row ();
        ++nb_dup;

        TS_ASSERT ( row[0].discr_val == std::size_t(0) );
        TS_ASSERT ( row[2].cont_val  == 4.5f );
        TS_ASSERT ( row[3].discr_val == std::size_t(7) );
      }
      TS_ASSERT ( nb_dup == std::size_t (12) );
      const auto& cols2 = genset2.columnsOfInterest ();
      TS_ASSERT ( cols2[0] == std::size_t(0) );
      TS_ASSERT ( cols2[1] == std::size_t(2) );
      TS_ASSERT ( cols2[2] == std::size_t(3) );

      gum::learning::DBRowGeneratorSet<> genset3 ( std::move ( genset2 ) );
      genset3.setInputRow( input_row1 );
      nb_dup = std::size_t(0);
      while ( genset3.hasRows () ) {
        const auto& row = genset3.generate ().row ();
        ++nb_dup;

        TS_ASSERT ( row[0].discr_val == std::size_t(0) );
        TS_ASSERT ( row[2].cont_val  == 4.5f );
        TS_ASSERT ( row[3].discr_val == std::size_t(7) );
      }
      TS_ASSERT ( nb_dup == std::size_t (12) );
      const auto& cols3 = genset3.columnsOfInterest ();
      TS_ASSERT ( cols3[0] == std::size_t(0) );
      TS_ASSERT ( cols3[1] == std::size_t(2) );
      TS_ASSERT ( cols3[2] == std::size_t(3) );

      gum::learning::DBRowGeneratorSet<>* genset4 = genset3.clone ();
      genset4->setInputRow( input_row1 );
      nb_dup = std::size_t(0);
      while ( genset4->hasRows () ) {
        const auto& row = genset4->generate ().row ();
        ++nb_dup;

        TS_ASSERT ( row[0].discr_val == std::size_t(0) );
        TS_ASSERT ( row[2].cont_val  == 4.5f );
        TS_ASSERT ( row[3].discr_val == std::size_t(7) );
      }
      TS_ASSERT ( nb_dup == std::size_t (12) );
      const auto& cols4 = genset4->columnsOfInterest ();
      TS_ASSERT ( cols4[0] == std::size_t(0) );
      TS_ASSERT ( cols4[1] == std::size_t(2) );
      TS_ASSERT ( cols4[2] == std::size_t(3) );
      
      delete genset4;

      gum::learning::DBRowGeneratorSet<> genset5;
      TS_ASSERT ( genset5.nbGenerators () == std::size_t(0) );
      genset5 = genset;
      TS_ASSERT ( genset5.nbGenerators () == std::size_t(2) );
      genset5.setInputRow( input_row1 );
      nb_dup = std::size_t(0);
      while ( genset5.hasRows () ) {
        const auto& row = genset5.generate ().row ();
        ++nb_dup;

        TS_ASSERT ( row[0].discr_val == std::size_t(0) );
        TS_ASSERT ( row[2].cont_val  == 4.5f );
        TS_ASSERT ( row[3].discr_val == std::size_t(7) );
      }
      TS_ASSERT ( nb_dup == std::size_t (12) );
      const auto& cols5 = genset5.columnsOfInterest ();
      TS_ASSERT ( cols5[0] == std::size_t(0) );
      TS_ASSERT ( cols5[1] == std::size_t(2) );
      TS_ASSERT ( cols5[2] == std::size_t(3) );

      gum::learning::DBRowGeneratorSet<> genset6;
      TS_ASSERT ( genset6.nbGenerators () == std::size_t(0) );
      genset6 = std::move ( genset5 );
      TS_ASSERT ( genset6.nbGenerators () == std::size_t(2) );
      genset6.setInputRow( input_row1 );
      nb_dup = std::size_t(0);
      while ( genset6.hasRows () ) {
        const auto& row = genset6.generate ().row ();
        ++nb_dup;

        TS_ASSERT ( row[0].discr_val == std::size_t(0) );
        TS_ASSERT ( row[2].cont_val  == 4.5f );
        TS_ASSERT ( row[3].discr_val == std::size_t(7) );
      }
      TS_ASSERT ( nb_dup == std::size_t (12) );
      const auto& cols6 = genset6.columnsOfInterest ();
      TS_ASSERT ( cols6[0] == std::size_t(0) );
      TS_ASSERT ( cols6[1] == std::size_t(2) );
      TS_ASSERT ( cols6[2] == std::size_t(3) );

      gum::learning::MyGenerator<>& gen1 =
        dynamic_cast<gum::learning::MyGenerator<>&> ( genset6[0] );
      TS_ASSERT ( gen1.columnsOfInterest().size() == std::size_t(3) );

      genset.setInputRow( input_row1 );
      genset.clear ();
      TS_ASSERT ( ! genset.hasRows() );
      
    }

    void test_simple2() {
      const std::vector<gum::learning::DBTranslatedValueType>
        col_types { gum::learning::DBTranslatedValueType::DISCRETE,
                    gum::learning::DBTranslatedValueType::DISCRETE,
                    gum::learning::DBTranslatedValueType::CONTINUOUS,
                    gum::learning::DBTranslatedValueType::DISCRETE };

      gum::learning::MyGenerator2<> generator1 ( col_types, 5 );
      TS_ASSERT ( ! generator1.hasRows() );
      gum::learning::MyGenerator2<> generator2 ( col_types, 4 );
      TS_ASSERT ( ! generator2.hasRows() );
      
      gum::learning::DBRowGeneratorSet<> genset;
      genset.insertGenerator ( generator1 );
      genset.insertGenerator ( std::move ( generator2 ) );
      TS_ASSERT ( genset.nbGenerators() == std::size_t(2) );
      TS_ASSERT ( genset.size() == std::size_t(2) );
      TS_ASSERT ( ! genset.hasRows() );
      
      const gum::learning::DBRow<gum::learning::DBTranslatedValue>
        input_row1 { gum::learning::DBTranslatedValue { std::size_t(0) },
                     gum::learning::DBTranslatedValue { std::size_t(4) },
                     gum::learning::DBTranslatedValue { 4.5f },
                     gum::learning::DBTranslatedValue { std::size_t(7) } };

      genset.setInputRow( input_row1 );
      std::size_t nb_dup = std::size_t(0);
      while ( genset.hasRows () ) {
        const auto& row = genset.generate ().row ();
        ++nb_dup;

        TS_ASSERT ( row[0].discr_val == std::size_t(0) );
        TS_ASSERT ( row[1].discr_val == std::size_t(4) );
        TS_ASSERT ( row[2].cont_val  == 4.5f );
        TS_ASSERT ( row[3].discr_val == std::size_t(7) );
      }

      TS_ASSERT ( nb_dup == std::size_t (12) );

      genset.setInputRow( input_row1 );
      TS_ASSERT ( ! genset.hasRows() );

      genset.setInputRow( input_row1 );
      nb_dup = std::size_t(0);
      while ( genset.hasRows () ) {
        const auto& row = genset.generate ().row ();
        ++nb_dup;

        TS_ASSERT ( row[0].discr_val == std::size_t(0) );
        TS_ASSERT ( row[1].discr_val == std::size_t(4) );
        TS_ASSERT ( row[2].cont_val  == 4.5f );
        TS_ASSERT ( row[3].discr_val == std::size_t(7) );
      }

    }

  };

  
} /* namespace gum_tests */
