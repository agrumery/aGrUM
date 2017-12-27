/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/variables/labelizedVariable.h>

#include <agrum/learning/database/BNDatabaseGenerator.h>



namespace gum_tests {

  class BNDatabaseGeneratorTestSuite : public CxxTest::TestSuite {

    public:
    gum::BayesNet< double >* bn;

    void setUp() {
      try {
        bn = new gum::BayesNet< double >("Asia");
        std::string file = GET_RESSOURCES_PATH("survey.bif");
        auto        reader = gum::BIFReader< double >(bn, file);
        try {
          reader.proceed();
        } catch (gum::IOError& e) {
          TS_FAIL(e.errorContent());
        }
      } catch (gum::Exception& e) {
        TS_FAIL(e.errorContent());
      }
    }

    void tearDown() { delete bn; }

    void testConstuctor() {
      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
        dbgen = new gum::learning::BNDatabaseGenerator< double >(*bn));

      auto varOrder = dbgen->varOrder();
      TS_ASSERT_EQUALS(varOrder.size(), (gum::Size)6);
      TS_ASSERT_EQUALS(varOrder.at(0), (gum::Idx)0);
      TS_ASSERT_EQUALS(varOrder.at(1), (gum::Idx)1);
      TS_ASSERT_EQUALS(varOrder.at(2), (gum::Idx)2);
      TS_ASSERT_EQUALS(varOrder.at(3), (gum::Idx)3);
      TS_ASSERT_EQUALS(varOrder.at(4), (gum::Idx)4);

      auto varOrderNames = dbgen->varOrderNames();
      TS_ASSERT_EQUALS(varOrderNames.size(), (gum::Size)6);
      TS_ASSERT_EQUALS(varOrderNames.at(0), "A");
      TS_ASSERT_EQUALS(varOrderNames.at(1), "S");
      TS_ASSERT_EQUALS(varOrderNames.at(2), "E");
      TS_ASSERT_EQUALS(varOrderNames.at(3), "O");
      TS_ASSERT_EQUALS(varOrderNames.at(4), "R");
      TS_ASSERT_EQUALS(varOrderNames.at(5), "T");

      delete dbgen;
    }

    void testSetVarOrder(){

      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
        dbgen = new gum::learning::BNDatabaseGenerator< double >(*bn));

      std::vector< gum::Idx >    goodOrder1 = {1, 0, 3, 2, 5, 4};
      std::vector< std::string > goodOrder2 = {"A", "E", "O", "R", "S", "T"};
      std::vector< gum::Idx >    badOrder1 = {5, 1, 0, 3, 2, 5, 4};
      std::vector< std::string > badOrder2 = {"A", "E", "O", "R", "A", "S", "T"};
      std::vector< gum::Idx >    badOrder3 = {1, 0, 3, 5, 4};
      std::vector< std::string > badOrder4 = {"A", "O", "R", "S", "T"};
      std::vector< gum::Idx >    badOrder5 = {1, 0, 3, 6, 5, 4};
      std::vector< std::string > badOrder6 = {"A", "O", "R", "S", "T", "X"};

      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(goodOrder1));
      auto varOrderNames = dbgen->varOrderNames();
      TS_ASSERT_EQUALS(varOrderNames.size(), (gum::Size)6);
      TS_ASSERT_EQUALS(varOrderNames.at(1), "A");
      TS_ASSERT_EQUALS(varOrderNames.at(0), "S");
      TS_ASSERT_EQUALS(varOrderNames.at(3), "E");
      TS_ASSERT_EQUALS(varOrderNames.at(2), "O");
      TS_ASSERT_EQUALS(varOrderNames.at(5), "R");
      TS_ASSERT_EQUALS(varOrderNames.at(4), "T");

      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(goodOrder2));
      auto varOrder = dbgen->varOrder();
      TS_ASSERT_EQUALS(varOrder.size(), (gum::Size)6);
      TS_ASSERT_EQUALS(varOrder.at(0), (gum::Idx)0);
      TS_ASSERT_EQUALS(varOrder.at(1), (gum::Idx)2);
      TS_ASSERT_EQUALS(varOrder.at(2), (gum::Idx)3);
      TS_ASSERT_EQUALS(varOrder.at(3), (gum::Idx)4);
      TS_ASSERT_EQUALS(varOrder.at(4), (gum::Idx)1);
      TS_ASSERT_EQUALS(varOrder.at(5), (gum::Idx)5);

      //TODO setVarOrderFromCSV
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setTopologicalVarOrder());
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setAntiTopologicalVarOrder());
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setRandomVarOrder());
      TS_ASSERT_THROWS(dbgen->setVarOrder(badOrder1), gum::FatalError);
      TS_ASSERT_THROWS(dbgen->setVarOrder(badOrder2), gum::FatalError);
      TS_ASSERT_THROWS(dbgen->setVarOrder(badOrder3), gum::FatalError);
      TS_ASSERT_THROWS(dbgen->setVarOrder(badOrder4), gum::FatalError);

      delete dbgen;
    }
    void testDrawSamples() {

      gum::Size domSizeA = 3;
      gum::Size domSizeS = 2;
      gum::Size domSizeE = 2;
      gum::Size domSizeO = 2;
      gum::Size domSizeR = 2;
      gum::Size domSizeT = 3;
      gum::Size nbSamples = 100;

      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
        dbgen = new gum::learning::BNDatabaseGenerator< double >(*bn));
      TS_ASSERT_THROWS(dbgen->database(), gum::OperationNotAllowed);
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples));
      std::vector< std::vector< gum::Idx > > database;
      TS_GUM_ASSERT_THROWS_NOTHING(database = dbgen->database());

      for (const auto& row : database) {
        TS_ASSERT_LESS_THAN(row.at(0), domSizeA);
        TS_ASSERT_LESS_THAN(row.at(1), domSizeS);
        TS_ASSERT_LESS_THAN(row.at(2), domSizeE);
        TS_ASSERT_LESS_THAN(row.at(3), domSizeO);
        TS_ASSERT_LESS_THAN(row.at(4), domSizeR);
        TS_ASSERT_LESS_THAN(row.at(5), domSizeT);
      }

      std::vector< std::string > vOrder1 = {"S", "E", "T", "R", "A", "O"};
      TS_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(vOrder1));
      TS_GUM_ASSERT_THROWS_NOTHING(database = dbgen->database());
      for (const auto& row : database) {
        TS_ASSERT_LESS_THAN(row.at(0), domSizeS);
        TS_ASSERT_LESS_THAN(row.at(1), domSizeE);
        TS_ASSERT_LESS_THAN(row.at(2), domSizeT);
        TS_ASSERT_LESS_THAN(row.at(3), domSizeR);
        TS_ASSERT_LESS_THAN(row.at(4), domSizeA);
        TS_ASSERT_LESS_THAN(row.at(5), domSizeO);
      }

      std::vector< std::string > vOrder2 = {"S", "T", "E", "R", "O", "A"};
      TS_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(vOrder2));
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples));
      TS_GUM_ASSERT_THROWS_NOTHING(database = dbgen->database());
      for (const auto& row : database) {
        TS_ASSERT_LESS_THAN(row.at(0), domSizeS);
        TS_ASSERT_LESS_THAN(row.at(1), domSizeT);
        TS_ASSERT_LESS_THAN(row.at(2), domSizeE);
        TS_ASSERT_LESS_THAN(row.at(3), domSizeR);
        TS_ASSERT_LESS_THAN(row.at(4), domSizeO);
        TS_ASSERT_LESS_THAN(row.at(5), domSizeA);
      }

    }

    void testToCSV(){}

    void testToDatabaseVectInRam(){

    }


  };
}
