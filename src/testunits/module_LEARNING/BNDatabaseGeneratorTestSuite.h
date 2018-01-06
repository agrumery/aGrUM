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
#include <sstream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/variables/labelizedVariable.h>

#include <agrum/learning/database/BNDatabaseGenerator.h>

namespace gum_tests {

  class ASimpleDBGeneratorListener : public gum::ProgressListener {
    private:
    gum::Size   __nbr;
    std::string __mess;

    public:
    explicit ASimpleDBGeneratorListener(gum::ProgressNotifier& notif)
        : gum::ProgressListener(notif)
        , __nbr(0)
        , __mess(""){};

    void whenProgress(const void* buffer, const gum::Size a, const double c) {
      __nbr += a;
    }
    void whenStop(const void* buffer, const std::string& s) { __mess = s; }

    gum::Size   getNbr() { return __nbr; }
    std::string getMess() { return __mess; }
  };


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

    void testSetVarOrder() {

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

      std::string csvFile = GET_RESSOURCES_PATH("survey1.csv");
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setVarOrderFromCSV(csvFile));
      varOrderNames = dbgen->varOrderNames();
      TS_ASSERT_EQUALS(varOrderNames.size(), (gum::Size)6);
      TS_ASSERT_EQUALS(varOrderNames.at(0), "E");
      TS_ASSERT_EQUALS(varOrderNames.at(1), "A");
      TS_ASSERT_EQUALS(varOrderNames.at(2), "O");
      TS_ASSERT_EQUALS(varOrderNames.at(3), "T");
      TS_ASSERT_EQUALS(varOrderNames.at(4), "R");
      TS_ASSERT_EQUALS(varOrderNames.at(5), "S");

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

    void testDrawSamplesLog2likelihood() {
      gum::Size nbSamples1 = 100;
      gum::Size nbSamples2 = nbSamples1 * 100;
      gum::Size nbSamples3 = nbSamples1 * 1000;

      double ll_1, ll_2, ll_3;
      double tolerance = 0.1;

      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
        dbgen = new gum::learning::BNDatabaseGenerator< double >(*bn));
      TS_ASSERT_THROWS(dbgen->database(), gum::OperationNotAllowed);
      TS_GUM_ASSERT_THROWS_NOTHING(ll_1 = dbgen->drawSamples(nbSamples1));
      TS_GUM_ASSERT_THROWS_NOTHING(ll_2 = dbgen->drawSamples(nbSamples2));
      TS_GUM_ASSERT_THROWS_NOTHING(ll_3 = dbgen->drawSamples(nbSamples3));

      // log2likehood must proportional to number of samples
      TS_ASSERT_LESS_THAN(
        std::abs(1 - (double)nbSamples2 / (double)nbSamples1 * ll_1 / ll_2),
        tolerance);
      TS_ASSERT_LESS_THAN(
        std::abs(1 - (double)nbSamples3 / (double)nbSamples1 * ll_1 / ll_3),
        tolerance);

      // log2likelihood must be aprox nbSamples * entropy (theorical result)
      double entropy = (bn->cpt(0) * bn->cpt(1) * bn->cpt(2) * bn->cpt(3) *
                        bn->cpt(4) * bn->cpt(5))
                         .entropy();
      TS_ASSERT_LESS_THAN(std::abs(1 + entropy * nbSamples1 / ll_1), tolerance);
      TS_ASSERT_LESS_THAN(std::abs(1 + entropy * nbSamples2 / ll_2), tolerance);
      TS_ASSERT_LESS_THAN(std::abs(1 + entropy * nbSamples3 / ll_3), tolerance);
    }

    void testToCSV_1() {

      gum::Size nbSamples = 5;

      std::vector< std::string > domA = {"young", "adult", "old"};
      std::vector< std::string > domS = {"M", "F"};
      std::vector< std::string > domE = {"high", "uni"};
      std::vector< std::string > domO = {"emp", "self"};
      std::vector< std::string > domR = {"small", "big"};
      std::vector< std::string > domT = {"car", "train", "other"};

      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
        dbgen = new gum::learning::BNDatabaseGenerator< double >(*bn));

      std::vector< std::string > vOrder1 = {"S", "E", "T", "R", "A", "O"};
      TS_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(vOrder1));
      std::string csvFileURL = GET_RESSOURCES_PATH("survey_tmp1.csv");
      bool        useLabels = true;
      bool        append = false;
      std::string csvSeparator = " ";
      bool        checkOnAppend = true;

      TS_ASSERT_THROWS(
        dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend),
        gum::OperationNotAllowed);

      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples));
      TS_GUM_ASSERT_THROWS_NOTHING(
        dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend));

      std::vector< std::string > vOrder2 = {"S", "T", "E", "R", "O", "A"};
      TS_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(vOrder2));
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples));
      append = true;
      TS_ASSERT_THROWS(
        dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend),
        gum::OperationNotAllowed);
      TS_GUM_ASSERT_THROWS_NOTHING(
        dbgen->setVarOrderFromCSV(csvFileURL, csvSeparator));
      TS_GUM_ASSERT_THROWS_NOTHING(
        dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend));

      std::ifstream              csvFile(csvFileURL);
      std::string                line;
      std::vector< std::string > header;
      std::getline(csvFile, line);
      TS_ASSERT_EQUALS(line, "S E T R A O");

      while (std::getline(csvFile, line)) {

        std::istringstream iss(line);
        std::string        valS;
        std::string        valE;
        std::string        valT;
        std::string        valR;
        std::string        valA;
        std::string        valO;
        iss >> valS;
        iss >> valE;
        iss >> valT;
        iss >> valR;
        iss >> valA;
        iss >> valO;

        TS_ASSERT_DIFFERS(std::find(domA.begin(), domA.end(), valA), domA.end());
        TS_ASSERT_DIFFERS(std::find(domS.begin(), domS.end(), valS), domS.end());
        TS_ASSERT_DIFFERS(std::find(domE.begin(), domE.end(), valE), domE.end());
        TS_ASSERT_DIFFERS(std::find(domO.begin(), domO.end(), valO), domO.end());
        TS_ASSERT_DIFFERS(std::find(domR.begin(), domR.end(), valR), domR.end());
        TS_ASSERT_DIFFERS(std::find(domT.begin(), domT.end(), valT), domT.end());
      }
      csvFile.close();
    }

    void testToCSV_2() {

      gum::Size nbSamples = 5;

      gum::Size domSizeA = 3;
      gum::Size domSizeS = 2;
      gum::Size domSizeE = 2;
      gum::Size domSizeO = 2;
      gum::Size domSizeR = 2;
      gum::Size domSizeT = 3;

      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
        dbgen = new gum::learning::BNDatabaseGenerator< double >(*bn));

      std::vector< std::string > vOrder1 = {"S", "E", "T", "R", "A", "O"};
      TS_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(vOrder1));
      std::string csvFileURL = GET_RESSOURCES_PATH("survey_tmp2.csv");
      bool        useLabels = false;
      bool        append = false;
      std::string csvSeparator = " ";
      bool        checkOnAppend = true;

      TS_ASSERT_THROWS(
        dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend),
        gum::OperationNotAllowed);

      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples));
      TS_GUM_ASSERT_THROWS_NOTHING(
        dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend));

      std::vector< std::string > vOrder2 = {"S", "T", "E", "R", "O", "A"};
      TS_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(vOrder2));
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples));
      append = true;
      TS_ASSERT_THROWS(
        dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend),
        gum::OperationNotAllowed);
      TS_GUM_ASSERT_THROWS_NOTHING(
        dbgen->setVarOrderFromCSV(csvFileURL, csvSeparator));
      TS_GUM_ASSERT_THROWS_NOTHING(
        dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend));

      std::ifstream              csvFile(csvFileURL);
      std::string                line;
      std::vector< std::string > header;
      std::getline(csvFile, line);
      TS_ASSERT_EQUALS(line, "S E T R A O");

      while (std::getline(csvFile, line)) {

        std::istringstream iss(line);
        gum::Idx           valS;
        gum::Idx           valE;
        gum::Idx           valT;
        gum::Idx           valR;
        gum::Idx           valA;
        gum::Idx           valO;
        iss >> valS;
        iss >> valE;
        iss >> valT;
        iss >> valR;
        iss >> valA;
        iss >> valO;

        TS_ASSERT_LESS_THAN(valS, domSizeS);
        TS_ASSERT_LESS_THAN(valE, domSizeE);
        TS_ASSERT_LESS_THAN(valT, domSizeT);
        TS_ASSERT_LESS_THAN(valR, domSizeR);
        TS_ASSERT_LESS_THAN(valA, domSizeA);
        TS_ASSERT_LESS_THAN(valO, domSizeO);
        break;
      }
      csvFile.close();
    }

    void testToDatabaseVectInRam() {

      gum::Size domSizeA = 3;
      gum::Size domSizeS = 2;
      gum::Size domSizeE = 2;
      gum::Size domSizeO = 2;
      gum::Size domSizeR = 2;
      gum::Size domSizeT = 3;
      gum::Size nbSamples = 100;

      std::vector< std::string > domA = {"young", "adult", "old"};
      std::vector< std::string > domS = {"M", "F"};
      std::vector< std::string > domE = {"high", "uni"};
      std::vector< std::string > domO = {"emp", "self"};
      std::vector< std::string > domR = {"small", "big"};
      std::vector< std::string > domT = {"car", "train", "other"};

      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
        dbgen = new gum::learning::BNDatabaseGenerator< double >(*bn));
      TS_ASSERT_THROWS(dbgen->toDatabaseVectInRAM(), gum::OperationNotAllowed);
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples));
      gum::learning::DatabaseVectInRAM db;
      bool                             useLabels = true;
      TS_ASSERT_THROWS_NOTHING(db = dbgen->toDatabaseVectInRAM(useLabels));

      auto handler = db.handler();
      while (handler.hasRows()) {
        auto row = handler.row();
        TS_ASSERT_DIFFERS(std::find(domA.begin(), domA.end(), row[0].getString()),
                          domA.end());
        TS_ASSERT_DIFFERS(std::find(domS.begin(), domS.end(), row[1].getString()),
                          domS.end());
        TS_ASSERT_DIFFERS(std::find(domE.begin(), domE.end(), row[2].getString()),
                          domE.end());
        TS_ASSERT_DIFFERS(std::find(domO.begin(), domO.end(), row[3].getString()),
                          domO.end());
        TS_ASSERT_DIFFERS(std::find(domR.begin(), domR.end(), row[4].getString()),
                          domR.end());
        TS_ASSERT_DIFFERS(std::find(domT.begin(), domT.end(), row[5].getString()),
                          domT.end());
        handler.nextRow();
      }

      useLabels = false;
      TS_ASSERT_THROWS_NOTHING(db = dbgen->toDatabaseVectInRAM(useLabels));
      handler = db.handler();
      while (handler.hasRows()) {
        auto row = handler.row();
        TS_ASSERT_LESS_THAN(row[0].getReal(), domSizeA);
        TS_ASSERT_LESS_THAN(row[1].getReal(), domSizeS);
        TS_ASSERT_LESS_THAN(row[2].getReal(), domSizeE);
        TS_ASSERT_LESS_THAN(row[3].getReal(), domSizeO);
        TS_ASSERT_LESS_THAN(row[4].getReal(), domSizeR);
        TS_ASSERT_LESS_THAN(row[5].getReal(), domSizeT);
        handler.nextRow();
      }

      std::vector< std::string > vOrder1 = {"S", "E", "T", "R", "A", "O"};
      TS_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(vOrder1));

      useLabels = true;
      TS_ASSERT_THROWS_NOTHING(db = dbgen->toDatabaseVectInRAM(useLabels));
      handler = db.handler();
      while (handler.hasRows()) {
        auto row = handler.row();
        TS_ASSERT_DIFFERS(std::find(domS.begin(), domS.end(), row[0].getString()),
                          domS.end());
        TS_ASSERT_DIFFERS(std::find(domE.begin(), domE.end(), row[1].getString()),
                          domE.end());
        TS_ASSERT_DIFFERS(std::find(domT.begin(), domT.end(), row[2].getString()),
                          domT.end());
        TS_ASSERT_DIFFERS(std::find(domR.begin(), domR.end(), row[3].getString()),
                          domR.end());
        TS_ASSERT_DIFFERS(std::find(domA.begin(), domA.end(), row[4].getString()),
                          domA.end());
        TS_ASSERT_DIFFERS(std::find(domO.begin(), domO.end(), row[5].getString()),
                          domO.end());
        handler.nextRow();
      }

      useLabels = false;
      TS_ASSERT_THROWS_NOTHING(db = dbgen->toDatabaseVectInRAM(useLabels));
      handler = db.handler();
      while (handler.hasRows()) {
        auto row = handler.row();
        TS_ASSERT_LESS_THAN(row[0].getReal(), domSizeS);
        TS_ASSERT_LESS_THAN(row[1].getReal(), domSizeE);
        TS_ASSERT_LESS_THAN(row[2].getReal(), domSizeT);
        TS_ASSERT_LESS_THAN(row[3].getReal(), domSizeR);
        TS_ASSERT_LESS_THAN(row[4].getReal(), domSizeA);
        TS_ASSERT_LESS_THAN(row[5].getReal(), domSizeO);
        handler.nextRow();
      }
    }

    void testListenToDrawSamples() {
      gum::learning::BNDatabaseGenerator< double > dbgen(*bn);

      ASimpleDBGeneratorListener gener(dbgen);
      TS_ASSERT_EQUALS(gener.getNbr(), gum::Size(0));
      TS_ASSERT_EQUALS(gener.getMess(), "");
      dbgen.drawSamples(100);
      TS_ASSERT_EQUALS(gener.getNbr(), gum::Size(4950));
      TS_ASSERT_DIFFERS(gener.getMess(), "");

      ASimpleDBGeneratorListener gener2(dbgen);
      TS_ASSERT_EQUALS(gener2.getNbr(), gum::Size(0));
      TS_ASSERT_EQUALS(gener2.getMess(), "");
      dbgen.drawSamples(1000);
      TS_ASSERT_EQUALS(gener2.getNbr(), gum::Size(4950));
      TS_ASSERT_DIFFERS(gener2.getMess(), "");
    }
  };
}
