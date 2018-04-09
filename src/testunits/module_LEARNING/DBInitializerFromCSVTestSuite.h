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
#include <ressources/myalloc.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ressources/myalloc.h>
#include <ressources/learningAlloc.h>

#include <agrum/learning/database/DBInitializerFromCSV.h>
#include <agrum/learning/database/databaseTable.h>
#include <agrum/learning/database/rawDatabaseTable.h>
#include <agrum/learning/database/DBTranslatorSet.h>
#include <agrum/learning/database/DBTranslator4ContinuousVariable.h>


namespace gum_tests {

  class DBInitializerFromCSVTestSuite : public CxxTest::TestSuite {
    public:
    void test_init1 () {
      gum::learning::DBInitializerFromCSV<>
        initializer ( GET_RESSOURCES_PATH( "asia.csv" ) );

      const auto& var_names = initializer.variableNames ();
      const std::size_t nb_vars = var_names.size ();
      TS_ASSERT_EQUALS ( nb_vars, std::size_t(8) );
      
      gum::learning::DBTranslatorSet<> translator_set;
      gum::learning::DBTranslator4ContinuousVariable<> translator;
      for ( std::size_t i = 0; i < nb_vars; ++i ) {
        translator_set.insertTranslator ( translator, i );
      }
      
      gum::learning::DatabaseTable<> database ( translator_set );

      database.setVariableNames( initializer.variableNames () );
      TS_ASSERT_EQUALS ( database.size (), std::size_t(0) );
      initializer.fillDatabase ( database );
      TS_ASSERT_EQUALS ( database.size (), std::size_t(10000) );

      gum::learning::DBInitializerFromCSV<> initializer2 ( initializer );
      gum::learning::DatabaseTable<> database2;
      database2.insertTranslator ( translator, 1 );
      database2.insertTranslator ( translator, 3 );
      database2.insertTranslator ( translator, 4 );
      database2.setVariableNames( initializer2.variableNames (), true );
      TS_ASSERT ( database2.variableNames().size() == 3 );
      {
        const auto ignored_vect = database2.ignoredColumns ();
        TS_ASSERT ( ignored_vect.size() == std::size_t(3) );
        TS_ASSERT ( ignored_vect[0] == 0 );
        TS_ASSERT ( ignored_vect[1] == 2 );
        TS_ASSERT ( ignored_vect[2] == 5 );
      }

      initializer2.fillDatabase ( database2 );
      TS_ASSERT_EQUALS ( database2.size (), std::size_t(10000) );
      const auto& content = database2.content ();

      const auto& row0 = content[0];
      TS_ASSERT ( row0[0].cont_val == 0.0f );
      TS_ASSERT ( row0[1].cont_val == 1.0f );
      TS_ASSERT ( row0[2].cont_val == 1.0f );
      const auto& row1 = content[1];
      TS_ASSERT ( row1[0].cont_val == 1.0f );
      TS_ASSERT ( row1[1].cont_val == 1.0f );
      TS_ASSERT ( row1[2].cont_val == 1.0f );

      gum::learning::DBInitializerFromCSV<>
        initializer3 ( initializer, std::allocator<std::string> () );
      gum::learning::DatabaseTable<> database3;
      database3.ignoreColumn ( 0 );
      database3.ignoreColumn ( 2 );
      database3.ignoreColumn ( 5 );
      
      for ( std::size_t i = 0; i < nb_vars; ++i ) {
        try {
          database3.insertTranslator ( translator, i );
        }
        catch ( gum::OperationNotAllowed& ) {}
      }
      const std::vector<std::string> names {"x1", "x3", "x4", "x6", "x7" };
      database3.setVariableNames( names, false );
      TS_ASSERT ( database3.variableNames().size() == std::size_t(5) );

      {
        const auto ignored_vect = database3.ignoredColumns ();
        TS_ASSERT ( ignored_vect.size() == std::size_t(4) );
        TS_ASSERT ( ignored_vect[0] == 0 );
        TS_ASSERT ( ignored_vect[1] == 2 );
        TS_ASSERT ( ignored_vect[2] == 5 );
        TS_ASSERT ( ignored_vect[3] == 8 );
      }

      initializer3.fillDatabase ( database3 );
      TS_ASSERT_EQUALS ( database3.size (), std::size_t(10000) );
      const auto& content3 = database3.content ();

      const auto& row30 = content3[0];
      TS_ASSERT ( row30.size() == std::size_t(5) );
      TS_ASSERT ( row30[0].cont_val == 0.0f );
      TS_ASSERT ( row30[1].cont_val == 1.0f );
      TS_ASSERT ( row30[2].cont_val == 1.0f );
      TS_ASSERT ( row30[3].cont_val == 0.0f );
      TS_ASSERT ( row30[4].cont_val == 0.0f );
      const auto& row31 = content3[1];
      TS_ASSERT ( row31.size() == std::size_t(5) );
      TS_ASSERT ( row31[0].cont_val == 1.0f );
      TS_ASSERT ( row31[1].cont_val == 1.0f );
      TS_ASSERT ( row31[2].cont_val == 1.0f );
      TS_ASSERT ( row31[3].cont_val == 0.0f );
      TS_ASSERT ( row31[4].cont_val == 1.0f );

      gum::learning::DBInitializerFromCSV<>
        initializer4 ( std::move ( initializer ),
                       std::allocator<std::string> () );
      gum::learning::DatabaseTable<> database4;
      database4.insertTranslator ( translator, 1 );
      database4.insertTranslator ( translator, 3 );
      database4.insertTranslator ( translator, 4 );
      database4.setVariableNames( initializer4.variableNames (), true );
      TS_ASSERT ( database4.variableNames().size() == 3 );
      initializer4.fillDatabase ( database4 );
      TS_ASSERT_EQUALS ( database4.size (), std::size_t(10000) );

      gum::learning::DBInitializerFromCSV<>
        initializer5 ( std::move ( initializer ) );
      gum::learning::DatabaseTable<> database5;
      database5.insertTranslator ( translator, 1 );
      database5.insertTranslator ( translator, 3 );
      database5.insertTranslator ( translator, 4 );
      database5.setVariableNames( initializer5.variableNames (), true );
      TS_ASSERT ( database5.variableNames().size() == 3 );
      initializer5.fillDatabase ( database5 );
      TS_ASSERT_EQUALS ( database5.size (), std::size_t(10000) );

      gum::learning::DBInitializerFromCSV<>*
        initializer6 = initializer4.clone ();
      gum::learning::DatabaseTable<> database6;
      database6.insertTranslator ( translator, 1 );
      database6.insertTranslator ( translator, 3 );
      database6.insertTranslator ( translator, 4 );
      database6.setVariableNames( initializer6->variableNames (), true );
      TS_ASSERT ( database6.variableNames().size() == 3 );
      initializer6->fillDatabase ( database6 );
      TS_ASSERT_EQUALS ( database6.size (), std::size_t(10000) );

      delete initializer6;

      gum::learning::DBInitializerFromCSV<>*
        initializer7 = initializer4.clone ();
      gum::learning::DatabaseTable<> database7;
      database7.insertTranslator ( translator, 1 );
      database7.insertTranslator ( translator, 3 );
      database7.insertTranslator ( translator, 4 );
      database7.setVariableNames( initializer7->variableNames (), true );
      TS_ASSERT ( database7.variableNames().size() == 3 );
      initializer7->fillDatabase ( database7 );
      TS_ASSERT_EQUALS ( database7.size (), std::size_t(10000) );

      delete initializer7;

      initializer = initializer5;
      gum::learning::DatabaseTable<> database8;
      database8.insertTranslator ( translator, 1 );
      database8.insertTranslator ( translator, 3 );
      database8.insertTranslator ( translator, 4 );
      database8.setVariableNames( initializer.variableNames (), true );
      TS_ASSERT ( database8.variableNames().size() == 3 );
      initializer.fillDatabase ( database8 );
      TS_ASSERT_EQUALS ( database8.size (), std::size_t(10000) );

      initializer = std::move ( initializer5 );
      gum::learning::DatabaseTable<> database9;
      database9.insertTranslator ( translator, 1 );
      database9.insertTranslator ( translator, 3 );
      database9.insertTranslator ( translator, 4 );
      database9.setVariableNames( initializer.variableNames (), true );
      TS_ASSERT ( database9.variableNames().size() == 3 );
      initializer.fillDatabase ( database9 );
      TS_ASSERT_EQUALS ( database9.size (), std::size_t(10000) );
    }

  };

} /* namespace gum_tests */
