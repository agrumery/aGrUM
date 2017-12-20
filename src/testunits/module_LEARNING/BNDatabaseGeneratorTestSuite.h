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
#include <stdio.h>
#include <string.h>
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
        std::string file = GET_RESSOURCES_PATH("asia4.bif");
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
      TS_ASSERT_EQUALS(varOrder.size(), (gum::Size)8);
      TS_ASSERT_EQUALS(varOrder.at(0), (gum::Idx)0);
      TS_ASSERT_EQUALS(varOrder.at(1), (gum::Idx)1);
      TS_ASSERT_EQUALS(varOrder.at(2), (gum::Idx)2);
      TS_ASSERT_EQUALS(varOrder.at(3), (gum::Idx)3);
      TS_ASSERT_EQUALS(varOrder.at(4), (gum::Idx)4);
      TS_ASSERT_EQUALS(varOrder.at(5), (gum::Idx)5);
      TS_ASSERT_EQUALS(varOrder.at(6), (gum::Idx)6);
      TS_ASSERT_EQUALS(varOrder.at(7), (gum::Idx)7);

      auto varOrderNames = dbgen->varOrderNames();
      TS_ASSERT_EQUALS(varOrderNames.size(), (gum::Size)8);
      TS_ASSERT_EQUALS(varOrderNames.at(0), "asia");
      TS_ASSERT_EQUALS(varOrderNames.at(1), "tub");
      TS_ASSERT_EQUALS(varOrderNames.at(2), "smoke");
      TS_ASSERT_EQUALS(varOrderNames.at(3), "lung");
      TS_ASSERT_EQUALS(varOrderNames.at(4), "bronc");
      TS_ASSERT_EQUALS(varOrderNames.at(5), "either");
      TS_ASSERT_EQUALS(varOrderNames.at(6), "xray");
      TS_ASSERT_EQUALS(varOrderNames.at(7), "dysp");

      delete dbgen;
    }

    void testSetVarOrder(){

      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
        dbgen = new gum::learning::BNDatabaseGenerator< double >(*bn));

      std::vector< gum::Idx >    goodOrder1 = {1, 0, 3, 2, 7, 6, 5, 4};
      std::vector< std::string > goodOrder2 = {
        "xray", "dysp", "either", "bronc", "smoke", "lung", "tub", "asia"};


      std::vector< gum::Idx >    badOrder1 = {5, 1, 0, 3, 2, 7, 6, 5, 4};
      std::vector< std::string > badOrder2 = {
        "xray", "lung", "dysp", "either", "bronc", "smoke", "lung", "tub", "asia"};

      std::vector< gum::Idx >    badOrder3 = {1, 0, 3, 7, 6, 5, 4};
      std::vector< std::string > badOrder4 = {
        "xray", "dysp", "either", "bronc", "lung", "tub", "asia"};

      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(goodOrder1));
      auto varOrderNames = dbgen->varOrderNames();
      TS_ASSERT_EQUALS(varOrderNames.size(), (gum::Size)8);
      TS_ASSERT_EQUALS(varOrderNames.at(1), "asia");
      TS_ASSERT_EQUALS(varOrderNames.at(0), "tub");
      TS_ASSERT_EQUALS(varOrderNames.at(3), "smoke");
      TS_ASSERT_EQUALS(varOrderNames.at(2), "lung");
      TS_ASSERT_EQUALS(varOrderNames.at(7), "bronc");
      TS_ASSERT_EQUALS(varOrderNames.at(6), "either");
      TS_ASSERT_EQUALS(varOrderNames.at(5), "xray");
      TS_ASSERT_EQUALS(varOrderNames.at(4), "dysp");

      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(goodOrder2));
      auto varOrder = dbgen->varOrder();
      TS_ASSERT_EQUALS(varOrder.size(), (gum::Size)8);
      TS_ASSERT_EQUALS(varOrder.at(0), (gum::Idx)6);
      TS_ASSERT_EQUALS(varOrder.at(1), (gum::Idx)7);
      TS_ASSERT_EQUALS(varOrder.at(2), (gum::Idx)5);
      TS_ASSERT_EQUALS(varOrder.at(3), (gum::Idx)4);
      TS_ASSERT_EQUALS(varOrder.at(4), (gum::Idx)2);
      TS_ASSERT_EQUALS(varOrder.at(5), (gum::Idx)3);
      TS_ASSERT_EQUALS(varOrder.at(6), (gum::Idx)1);
      TS_ASSERT_EQUALS(varOrder.at(7), (gum::Idx)0);

      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setTopologicalVarOrder());
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setAntiTopologicalVarOrder());
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setRandomVarOrder());
      TS_ASSERT_THROWS(dbgen->setVarOrder(badOrder1), gum::FatalError);
      TS_ASSERT_THROWS(dbgen->setVarOrder(badOrder2), gum::FatalError);
      TS_ASSERT_THROWS(dbgen->setVarOrder(badOrder3), gum::FatalError);
      TS_ASSERT_THROWS(dbgen->setVarOrder(badOrder4), gum::FatalError);

      delete dbgen;
    }

    void testToCSV(){}

    void testToDatabaseVectInRam(){

    }


  };
}
