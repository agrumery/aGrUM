/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/database/BNDatabaseGenerator.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/learning/BNLearner.h>

namespace gum_tests {

  class ASimpleDBGeneratorListener: public gum::ProgressListener {
    private:
    gum::Size   _nbr_  = 0;
    std::string _mess_ = "";

    public:
    explicit ASimpleDBGeneratorListener(gum::ProgressNotifier& notif) :
        gum::ProgressListener(notif) {};

    void whenProgress(const void* buffer, const gum::Size a, const double c) override {
      _nbr_ += a;
    }

    void whenStop(const void* buffer, const std::string& s) override { _mess_ = s; }

    gum::Size getNbr() const { return _nbr_; }

    std::string getMess() const { return _mess_; }
  };

  class [[maybe_unused]] BNDatabaseGeneratorTestSuite: public CxxTest::TestSuite {
    public:
    static gum::BayesNet< double > initAsia() {
      gum::BayesNet< double > bn("Asia");
      try {
        std::string file   = GET_RESSOURCES_PATH("bif/survey.bif");
        auto        reader = gum::BIFReader< double >(&bn, file);
        try {
          reader.proceed();
        } catch (gum::IOError& e) { TS_FAIL(e.errorContent()); }
      } catch (gum::Exception& e) { TS_FAIL(e.errorContent()); }

      return bn;
    }

    GUM_ACTIVE_TEST(Constuctor) {
      gum::BayesNet< double >                       bn    = initAsia();
      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen = new gum::learning::BNDatabaseGenerator< double >(bn))

      auto varOrder = dbgen->varOrder();
      TS_ASSERT_EQUALS(varOrder.size(), (gum::Size)6)
      TS_ASSERT_EQUALS(varOrder.at(0), (gum::Idx)0)
      TS_ASSERT_EQUALS(varOrder.at(1), (gum::Idx)1)
      TS_ASSERT_EQUALS(varOrder.at(2), (gum::Idx)2)
      TS_ASSERT_EQUALS(varOrder.at(3), (gum::Idx)3)
      TS_ASSERT_EQUALS(varOrder.at(4), (gum::Idx)4)

      auto varOrderNames = dbgen->varOrderNames();
      TS_ASSERT_EQUALS(varOrderNames.size(), (gum::Size)6)
      TS_ASSERT_EQUALS(varOrderNames.at(0), "A")
      TS_ASSERT_EQUALS(varOrderNames.at(1), "S")
      TS_ASSERT_EQUALS(varOrderNames.at(2), "E")
      TS_ASSERT_EQUALS(varOrderNames.at(3), "O")
      TS_ASSERT_EQUALS(varOrderNames.at(4), "R")
      TS_ASSERT_EQUALS(varOrderNames.at(5), "T")

      delete dbgen;
    }

    GUM_ACTIVE_TEST(SetVarOrder) {
      gum::BayesNet< double >                       bn    = initAsia();
      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen = new gum::learning::BNDatabaseGenerator< double >(bn))

      std::vector< gum::Idx >    goodOrder1 = {1, 0, 3, 2, 5, 4};
      std::vector< std::string > goodOrder2 = {"A", "E", "O", "R", "S", "T"};
      std::vector< gum::Idx >    badOrder1  = {5, 1, 0, 3, 2, 5, 4};
      std::vector< std::string > badOrder2  = {"A", "E", "O", "R", "A", "S", "T"};
      std::vector< gum::Idx >    badOrder3  = {1, 0, 3, 5, 4};
      std::vector< std::string > badOrder4  = {"A", "O", "R", "S", "T"};

      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(goodOrder1))
      auto varOrderNames = dbgen->varOrderNames();
      TS_ASSERT_EQUALS(varOrderNames.size(), (gum::Size)6)
      TS_ASSERT_EQUALS(varOrderNames.at(1), "A")
      TS_ASSERT_EQUALS(varOrderNames.at(0), "S")
      TS_ASSERT_EQUALS(varOrderNames.at(3), "E")
      TS_ASSERT_EQUALS(varOrderNames.at(2), "O")
      TS_ASSERT_EQUALS(varOrderNames.at(5), "R")
      TS_ASSERT_EQUALS(varOrderNames.at(4), "T")

      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(goodOrder2))
      auto varOrder = dbgen->varOrder();
      TS_ASSERT_EQUALS(varOrder.size(), (gum::Size)6)
      TS_ASSERT_EQUALS(varOrder.at(0), (gum::Idx)0)
      TS_ASSERT_EQUALS(varOrder.at(1), (gum::Idx)2)
      TS_ASSERT_EQUALS(varOrder.at(2), (gum::Idx)3)
      TS_ASSERT_EQUALS(varOrder.at(3), (gum::Idx)4)
      TS_ASSERT_EQUALS(varOrder.at(4), (gum::Idx)1)
      TS_ASSERT_EQUALS(varOrder.at(5), (gum::Idx)5)

      std::string csvFile = GET_RESSOURCES_PATH("csv/survey1.csv");
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setVarOrderFromCSV(csvFile))
      varOrderNames = dbgen->varOrderNames();
      TS_ASSERT_EQUALS(varOrderNames.size(), (gum::Size)6)
      TS_ASSERT_EQUALS(varOrderNames.at(0), "E")
      TS_ASSERT_EQUALS(varOrderNames.at(1), "A")
      TS_ASSERT_EQUALS(varOrderNames.at(2), "O")
      TS_ASSERT_EQUALS(varOrderNames.at(3), "T")
      TS_ASSERT_EQUALS(varOrderNames.at(4), "R")
      TS_ASSERT_EQUALS(varOrderNames.at(5), "S")

      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setTopologicalVarOrder())
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setAntiTopologicalVarOrder())
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setRandomVarOrder())
      TS_ASSERT_THROWS(dbgen->setVarOrder(badOrder1), const gum::FatalError&)
      TS_ASSERT_THROWS(dbgen->setVarOrder(badOrder2), const gum::FatalError&)
      TS_ASSERT_THROWS(dbgen->setVarOrder(badOrder3), const gum::FatalError&)
      TS_ASSERT_THROWS(dbgen->setVarOrder(badOrder4), const gum::FatalError&)

      delete dbgen;
    }

    GUM_ACTIVE_TEST(DrawSamples) {
      gum::BayesNet< double > bn = initAsia();

      gum::Size domSizeA  = 3;
      gum::Size domSizeS  = 2;
      gum::Size domSizeE  = 2;
      gum::Size domSizeO  = 2;
      gum::Size domSizeR  = 2;
      gum::Size domSizeT  = 3;
      gum::Size nbSamples = 100;

      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen = new gum::learning::BNDatabaseGenerator< double >(bn))

      TS_ASSERT_THROWS(dbgen->database(), const gum::OperationNotAllowed&)
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples))

      std::vector< std::vector< gum::Idx > > database;
      TS_GUM_ASSERT_THROWS_NOTHING(database = dbgen->database())

      for (const auto& row: database) {
        TS_ASSERT_LESS_THAN(row.at(0), domSizeA)
        TS_ASSERT_LESS_THAN(row.at(1), domSizeS)
        TS_ASSERT_LESS_THAN(row.at(2), domSizeE)
        TS_ASSERT_LESS_THAN(row.at(3), domSizeO)
        TS_ASSERT_LESS_THAN(row.at(4), domSizeR)
        TS_ASSERT_LESS_THAN(row.at(5), domSizeT)
      }

      std::vector< std::string > vOrder1 = {"S", "E", "T", "R", "A", "O"};
      TS_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(vOrder1))
      TS_GUM_ASSERT_THROWS_NOTHING(database = dbgen->database())
      for (const auto& row: database) {
        TS_ASSERT_LESS_THAN(row.at(0), domSizeS)
        TS_ASSERT_LESS_THAN(row.at(1), domSizeE)
        TS_ASSERT_LESS_THAN(row.at(2), domSizeT)
        TS_ASSERT_LESS_THAN(row.at(3), domSizeR)
        TS_ASSERT_LESS_THAN(row.at(4), domSizeA)
        TS_ASSERT_LESS_THAN(row.at(5), domSizeO)
      }

      std::vector< std::string > vOrder2 = {"S", "T", "E", "R", "O", "A"};
      TS_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(vOrder2))
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples))
      TS_GUM_ASSERT_THROWS_NOTHING(database = dbgen->database())
      for (const auto& row: database) {
        TS_ASSERT_LESS_THAN(row.at(0), domSizeS)
        TS_ASSERT_LESS_THAN(row.at(1), domSizeT)
        TS_ASSERT_LESS_THAN(row.at(2), domSizeE)
        TS_ASSERT_LESS_THAN(row.at(3), domSizeR)
        TS_ASSERT_LESS_THAN(row.at(4), domSizeO)
        TS_ASSERT_LESS_THAN(row.at(5), domSizeA)
      }
      delete dbgen;
    }

    GUM_ACTIVE_TEST(DrawSamplesLog2likelihood) {
      gum::BayesNet< double > bn         = initAsia();
      gum::Size               nbSamples1 = 100;
      gum::Size               nbSamples2 = nbSamples1 * 100;
      gum::Size               nbSamples3 = nbSamples1 * 1000;

      double ll_1      = 0;
      double ll_2      = 0;
      double ll_3      = 0;
      double tolerance = 0.1;

      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen = new gum::learning::BNDatabaseGenerator< double >(bn))
      TS_ASSERT_THROWS(dbgen->database(), const gum::OperationNotAllowed&)
      TS_GUM_ASSERT_THROWS_NOTHING(ll_1 = dbgen->drawSamples(nbSamples1))
      TS_GUM_ASSERT_THROWS_NOTHING(ll_2 = dbgen->drawSamples(nbSamples2))
      TS_GUM_ASSERT_THROWS_NOTHING(ll_3 = dbgen->drawSamples(nbSamples3))

      // log2likehood must proportional to number of samples
      TS_ASSERT_LESS_THAN(std::abs(1 - (double)nbSamples2 / (double)nbSamples1 * ll_1 / ll_2),
                          tolerance);
      TS_ASSERT_LESS_THAN(std::abs(1 - (double)nbSamples3 / (double)nbSamples1 * ll_1 / ll_3),
                          tolerance);

      // log2likelihood must be aprox nbSamples * entropy (theorical result)
      double entropy
          = (bn.cpt(0) * bn.cpt(1) * bn.cpt(2) * bn.cpt(3) * bn.cpt(4) * bn.cpt(5)).entropy();
      TS_ASSERT_LESS_THAN(std::abs(1 + entropy * nbSamples1 / ll_1), tolerance)
      TS_ASSERT_LESS_THAN(std::abs(1 + entropy * nbSamples2 / ll_2), tolerance)
      TS_ASSERT_LESS_THAN(std::abs(1 + entropy * nbSamples3 / ll_3), tolerance)

      delete dbgen;
    }

    GUM_ACTIVE_TEST(ToCSV_1) {
      gum::BayesNet< double > bn        = initAsia();
      gum::Size               nbSamples = 5;

      std::vector< std::string > domA = {"young", "adult", "old"};
      std::vector< std::string > domS = {"M", "F"};
      std::vector< std::string > domE = {"high", "uni"};
      std::vector< std::string > domO = {"emp", "self"};
      std::vector< std::string > domR = {"small", "big"};
      std::vector< std::string > domT = {"car", "train", "other"};

      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen = new gum::learning::BNDatabaseGenerator< double >(bn))

      std::vector< std::string > vOrder1 = {"S", "E", "T", "R", "A", "O"};
      TS_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(vOrder1))
      std::string csvFileURL    = GET_RESSOURCES_PATH("outputs/survey_tmp1.csv");
      bool        useLabels     = true;
      bool        append        = false;
      std::string csvSeparator  = " ";
      bool        checkOnAppend = true;

      TS_ASSERT_THROWS(dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend),
                       const gum::OperationNotAllowed&)

      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples))
      TS_GUM_ASSERT_THROWS_NOTHING(
          dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend));

      std::vector< std::string > vOrder2 = {"S", "T", "E", "R", "O", "A"};
      TS_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(vOrder2))
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples))
      append = true;
      TS_ASSERT_THROWS(dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend),
                       const gum::OperationNotAllowed&)
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setVarOrderFromCSV(csvFileURL, csvSeparator))
      TS_GUM_ASSERT_THROWS_NOTHING(
          dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend));

      std::ifstream              csvFile(csvFileURL);
      std::string                line;
      std::vector< std::string > header;
      std::getline(csvFile, line);
      TS_ASSERT_EQUALS(line, "S E T R A O")

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

        TS_ASSERT_DIFFERS(std::find(domA.begin(), domA.end(), valA), domA.end())
        TS_ASSERT_DIFFERS(std::find(domS.begin(), domS.end(), valS), domS.end())
        TS_ASSERT_DIFFERS(std::find(domE.begin(), domE.end(), valE), domE.end())
        TS_ASSERT_DIFFERS(std::find(domO.begin(), domO.end(), valO), domO.end())
        TS_ASSERT_DIFFERS(std::find(domR.begin(), domR.end(), valR), domR.end())
        TS_ASSERT_DIFFERS(std::find(domT.begin(), domT.end(), valT), domT.end())
      }
      csvFile.close();

      delete dbgen;
    }

    GUM_ACTIVE_TEST(ToCSV_2) {
      gum::BayesNet< double > bn        = initAsia();
      gum::Size               nbSamples = 5;

      gum::Size domSizeA = 3;
      gum::Size domSizeS = 2;
      gum::Size domSizeE = 2;
      gum::Size domSizeO = 2;
      gum::Size domSizeR = 2;
      gum::Size domSizeT = 3;

      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen = new gum::learning::BNDatabaseGenerator< double >(bn))

      std::vector< std::string > vOrder1 = {"S", "E", "T", "R", "A", "O"};
      TS_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(vOrder1))
      std::string csvFileURL    = GET_RESSOURCES_PATH("outputs/survey_tmp2.csv");
      bool        useLabels     = false;
      bool        append        = false;
      std::string csvSeparator  = " ";
      bool        checkOnAppend = true;

      TS_ASSERT_THROWS(dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend),
                       const gum::OperationNotAllowed&)

      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples))
      TS_GUM_ASSERT_THROWS_NOTHING(
          dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend));

      std::vector< std::string > vOrder2 = {"S", "T", "E", "R", "O", "A"};
      TS_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(vOrder2))
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples))
      append = true;
      TS_ASSERT_THROWS(dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend),
                       const gum::OperationNotAllowed&)
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->setVarOrderFromCSV(csvFileURL, csvSeparator))
      TS_GUM_ASSERT_THROWS_NOTHING(
          dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend));

      std::ifstream              csvFile(csvFileURL);
      std::string                line;
      std::vector< std::string > header;
      std::getline(csvFile, line);
      TS_ASSERT_EQUALS(line, "S E T R A O")

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

        TS_ASSERT_LESS_THAN(valS, domSizeS)
        TS_ASSERT_LESS_THAN(valE, domSizeE)
        TS_ASSERT_LESS_THAN(valT, domSizeT)
        TS_ASSERT_LESS_THAN(valR, domSizeR)
        TS_ASSERT_LESS_THAN(valA, domSizeA)
        TS_ASSERT_LESS_THAN(valO, domSizeO)
      }
      csvFile.close();

      delete dbgen;
    }

    GUM_ACTIVE_TEST(ToDatabaseTable) {
      gum::BayesNet< double > bn        = initAsia();
      gum::Size               domSizeA  = 3;
      gum::Size               domSizeS  = 2;
      gum::Size               domSizeE  = 2;
      gum::Size               domSizeO  = 2;
      gum::Size               domSizeR  = 2;
      gum::Size               domSizeT  = 3;
      gum::Size               nbSamples = 100;

      std::vector< std::string > domA = {"young", "adult", "old"};
      std::vector< std::string > domS = {"M", "F"};
      std::vector< std::string > domE = {"high", "uni"};
      std::vector< std::string > domO = {"emp", "self"};
      std::vector< std::string > domR = {"small", "big"};
      std::vector< std::string > domT = {"car", "train", "other"};

      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen = new gum::learning::BNDatabaseGenerator< double >(bn))
      TS_ASSERT_THROWS(dbgen->toDatabaseTable(), const gum::OperationNotAllowed&)
      TS_GUM_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples))
      gum::learning::DatabaseTable db;
      bool                         useLabels = true;
      TS_ASSERT_THROWS_NOTHING(db = dbgen->toDatabaseTable(useLabels))

      auto handler = db.handler();
      while (handler.hasRows()) {
        auto row = handler.row();
        TS_ASSERT_DIFFERS(
            std::find(domA.begin(), domA.end(), db.translator(0).translateBack(row[0])),
            domA.end())
        TS_ASSERT_DIFFERS(
            std::find(domS.begin(), domS.end(), db.translator(1).translateBack(row[1])),
            domS.end())
        TS_ASSERT_DIFFERS(
            std::find(domE.begin(), domE.end(), db.translator(2).translateBack(row[2])),
            domE.end())
        TS_ASSERT_DIFFERS(
            std::find(domO.begin(), domO.end(), db.translator(3).translateBack(row[3])),
            domO.end())
        TS_ASSERT_DIFFERS(
            std::find(domR.begin(), domR.end(), db.translator(4).translateBack(row[4])),
            domR.end())
        TS_ASSERT_DIFFERS(
            std::find(domT.begin(), domT.end(), db.translator(5).translateBack(row[5])),
            domT.end())
        handler.nextRow();
      }

      useLabels = false;
      TS_ASSERT_THROWS_NOTHING(db = dbgen->toDatabaseTable(useLabels))
      handler = db.handler();
      while (handler.hasRows()) {
        auto row = handler.row();
        TS_ASSERT_LESS_THAN(row[0].discr_val, domSizeA)
        TS_ASSERT_LESS_THAN(row[1].discr_val, domSizeS)
        TS_ASSERT_LESS_THAN(row[2].discr_val, domSizeE)
        TS_ASSERT_LESS_THAN(row[3].discr_val, domSizeO)
        TS_ASSERT_LESS_THAN(row[4].discr_val, domSizeR)
        TS_ASSERT_LESS_THAN(row[5].discr_val, domSizeT)
        handler.nextRow();
      }

      std::vector< std::string > vOrder1 = {"S", "E", "T", "R", "A", "O"};
      TS_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(vOrder1))

      useLabels = true;
      TS_ASSERT_THROWS_NOTHING(db = dbgen->toDatabaseTable(useLabels))
      handler = db.handler();
      while (handler.hasRows()) {
        auto row = handler.row();


        TS_ASSERT_DIFFERS(
            std::find(domS.begin(), domS.end(), db.translator(0).translateBack(row[0])),
            domS.end())
        TS_ASSERT_DIFFERS(
            std::find(domE.begin(), domE.end(), db.translator(1).translateBack(row[1])),
            domE.end())
        TS_ASSERT_DIFFERS(
            std::find(domT.begin(), domT.end(), db.translator(2).translateBack(row[2])),
            domT.end())
        TS_ASSERT_DIFFERS(
            std::find(domR.begin(), domR.end(), db.translator(3).translateBack(row[3])),
            domR.end())
        TS_ASSERT_DIFFERS(
            std::find(domA.begin(), domA.end(), db.translator(4).translateBack(row[4])),
            domA.end())
        TS_ASSERT_DIFFERS(
            std::find(domO.begin(), domO.end(), db.translator(5).translateBack(row[5])),
            domO.end())

        handler.nextRow();
      }

      useLabels = false;
      TS_ASSERT_THROWS_NOTHING(db = dbgen->toDatabaseTable(useLabels))
      handler = db.handler();
      while (handler.hasRows()) {
        auto row = handler.row();
        TS_ASSERT_LESS_THAN(row[0].discr_val, domSizeS)
        TS_ASSERT_LESS_THAN(row[1].discr_val, domSizeE)
        TS_ASSERT_LESS_THAN(row[2].discr_val, domSizeT)
        TS_ASSERT_LESS_THAN(row[3].discr_val, domSizeR)
        TS_ASSERT_LESS_THAN(row[4].discr_val, domSizeA)
        TS_ASSERT_LESS_THAN(row[5].discr_val, domSizeO)
        handler.nextRow();
      }

      delete dbgen;
    }

    GUM_ACTIVE_TEST(ListenToDrawSamples) {
      gum::BayesNet< double >                      bn = initAsia();
      gum::learning::BNDatabaseGenerator< double > dbgen(bn);

      ASimpleDBGeneratorListener gener(dbgen);
      TS_ASSERT_EQUALS(gener.getNbr(), (gum::Size)0)
      TS_ASSERT_EQUALS(gener.getMess(), "")
      dbgen.drawSamples(100);
      TS_ASSERT_EQUALS(gener.getNbr(), (gum::Size)4950)
      TS_ASSERT_DIFFERS(gener.getMess(), "")

      ASimpleDBGeneratorListener gener2(dbgen);
      TS_ASSERT_EQUALS(gener2.getNbr(), (gum::Size)0)
      TS_ASSERT_EQUALS(gener2.getMess(), "")
      dbgen.drawSamples(1000);
      TS_ASSERT_EQUALS(gener2.getNbr(), (gum::Size)4950)
      TS_ASSERT_DIFFERS(gener2.getMess(), "")
    }

    GUM_ACTIVE_TEST(DrawingWithEvidence) {
      gum::BayesNet< double >                      bn = initAsia();
      gum::learning::BNDatabaseGenerator< double > dbgen(bn);
      ASimpleDBGeneratorListener                   gener(dbgen);

      gum::Instantiation filter;
      filter.add(bn.variable(0));
      filter.setFirst();
      dbgen.drawSamples(100, filter);
      TS_ASSERT_LESS_THAN(dbgen.samplesNbRows(), 101u)   // some samples have been rejected
    }

    GUM_ACTIVE_TEST(Accuracy) {
      std::string csvFileURL = GET_RESSOURCES_PATH("outputs/dbgen2.csv");
      auto        bn         = gum::BayesNet< double >::fastPrototype("A->B");
      gum::learning::BNDatabaseGenerator< double > dbgen(bn);
      dbgen.drawSamples(100000);
      dbgen.toCSV(csvFileURL);

      auto                     marginals = gum::BayesNet< double >::fastPrototype("A;B");
      gum::learning::BNLearner learn(csvFileURL, marginals);
      auto                     bn2 = learn.learnParameters(marginals.dag());

      gum::LazyPropagation ie(&bn);
      ie.makeInference();

      TS_ASSERT_LESS_THAN(
          (bn2.cpt("A")
           - (gum::Potential< double >() << bn2.variable("A")).fillWith(ie.posterior("A")))
              .abs()
              .max(),
          1e-2)
      TS_ASSERT_LESS_THAN(
          (bn2.cpt("B")
           - (gum::Potential< double >() << bn2.variable("B")).fillWith(ie.posterior("B")))
              .abs()
              .max(),
          1e-2)
    }

    GUM_ACTIVE_TEST(AccuracyWithEvidence) {
      std::string csvFileURL = GET_RESSOURCES_PATH("outputs/dbgen2.csv");
      auto        bn         = gum::BayesNet< double >::fastPrototype("A->B");
      gum::learning::BNDatabaseGenerator< double > dbgen(bn);
      gum::Instantiation                           filter;
      filter.add(bn.variable("B"));
      filter.setFirst();   // evidence B:0
      dbgen.drawSamples(100000, filter);
      dbgen.toCSV(csvFileURL);

      auto                     marginals = gum::BayesNet< double >::fastPrototype("A;B");
      gum::learning::BNLearner learn(csvFileURL, marginals);
      auto                     bn2 = learn.learnParameters(marginals.dag());

      gum::LazyPropagation ie(&bn);
      ie.addEvidence("B", 0);
      ie.makeInference();

      TS_ASSERT_LESS_THAN(
          (bn2.cpt("A")
           - (gum::Potential< double >() << bn2.variable("A")).fillWith(ie.posterior("A")))
              .abs()
              .max(),
          1e-2)
      TS_ASSERT_LESS_THAN(
          (bn2.cpt("B")
           - (gum::Potential< double >() << bn2.variable("B")).fillWith(ie.posterior("B")))
              .abs()
              .max(),
          1e-2)
    }

    GUM_ACTIVE_TEST(DiscretizedVariableTranslation) {
      std::string csvFileURL = GET_RESSOURCES_PATH("outputs/dbgen_discretized.csv");
      auto        bn         = gum::BayesNet< double >::fastPrototype("A[1.5,2,2.5,10,14]");

      gum::learning::BNDatabaseGenerator< double > dbgen(bn);

      const auto  nbLines = 100;
      std::string dummyLine;
      double      d;
      auto        pos = 0;

      dbgen.drawSamples(nbLines);
      // RANDOM VALUES (default)
      dbgen.toCSV(csvFileURL);

      {
        std::ifstream stream(csvFileURL);
        getline(stream, dummyLine);   // skip header
        pos = 0;
        while (getline(stream, dummyLine)) {
          pos++;
          TS_ASSERT(gum::isNumericalWithResult(dummyLine, &d))
          TS_ASSERT(d >= 1.5)
          TS_ASSERT(d <= 14)
        }
        TS_ASSERT_EQUALS(pos++, nbLines)
      }

      // MEDIAN
      dbgen.setDiscretizedLabelModeMedian();
      dbgen.toCSV(csvFileURL);
      {
        std::ifstream stream(csvFileURL);
        getline(stream, dummyLine);   // skip header
        pos = 0;
        while (getline(stream, dummyLine)) {
          pos++;
          TS_ASSERT(gum::isNumericalWithResult(dummyLine, &d))
          TS_ASSERT((d == 1.75) || (d == 2.25) || (d == 6.25) || (d == 12))
        }
        TS_ASSERT_EQUALS(pos, nbLines)
      }

      // INTERVALLS
      dbgen.setDiscretizedLabelModeInterval();
      dbgen.toCSV(csvFileURL);
      {
        std::ifstream stream(csvFileURL);
        getline(stream, dummyLine);   // skip header
        pos = 0;
        while (getline(stream, dummyLine)) {
          pos++;
          TS_ASSERT_EQUALS(dummyLine.front(), '[')
        }
        TS_ASSERT_EQUALS(pos, nbLines)
      }
    }

    GUM_ACTIVE_TEST(InfiniteRejectionSampling) {
      gum::Timer timer;

      auto bn = gum::BayesNet< double >::fastPrototype("A->B<-C->D");
      bn.cpt("B").fillWith({0, 1, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5});   // {B=0|A=0,C=0} impossible
      gum::learning::BNDatabaseGenerator< double > dbgen(bn);

      gum::Instantiation filter;

      filter.add(bn.variable("A"));
      filter.add(bn.variable("B"));
      filter.add(bn.variable("C"));
      filter.setFirst();   // evs={A:0,B=0,C:0} imposible

      const auto nbLines = 10;
      timer.reset();
      dbgen.drawSamples(nbLines, filter, 5);
      TS_ASSERT_LESS_THAN(timer.step(), 5.5)
      TS_ASSERT_EQUALS(dbgen.samplesNbRows(), 0u)
    }
  };
}   // namespace gum_tests
