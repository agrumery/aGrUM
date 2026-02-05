/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


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

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  BNDatabaseGenerator
#define GUM_CURRENT_MODULE BN

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

  struct BNDatabaseGeneratorTestSuite {
    public:
    static gum::BayesNet< double > initAsia() {
      gum::BayesNet< double > bn("Asia");
      try {
        std::string file   = GET_RESSOURCES_PATH("bif/survey.bif");
        auto        reader = gum::BIFReader< double >(&bn, file);
        try {
          reader.proceed();
        } catch (gum::IOError& e) { FAIL(e.errorContent()); }
      } catch (gum::Exception& e) { FAIL(e.errorContent()); }

      return bn;
    }   // namespace gum_tests

    static void testConstuctor() {
      gum::BayesNet< double >                       bn    = initAsia();
      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen = new gum::learning::BNDatabaseGenerator< double >(bn));

      auto varOrder = dbgen->varOrder();
      CHECK((varOrder.size()) == (static_cast< gum::Size >(6)));
      CHECK((varOrder.at(0)) == (static_cast< gum::Idx >(0)));
      CHECK((varOrder.at(1)) == (static_cast< gum::Idx >(1)));
      CHECK((varOrder.at(2)) == (static_cast< gum::Idx >(2)));
      CHECK((varOrder.at(3)) == (static_cast< gum::Idx >(3)));
      CHECK((varOrder.at(4)) == (static_cast< gum::Idx >(4)));

      auto varOrderNames = dbgen->varOrderNames();
      CHECK((varOrderNames.size()) == (static_cast< gum::Size >(6)));
      CHECK((varOrderNames.at(0)) == ("A"));
      CHECK((varOrderNames.at(1)) == ("S"));
      CHECK((varOrderNames.at(2)) == ("E"));
      CHECK((varOrderNames.at(3)) == ("O"));
      CHECK((varOrderNames.at(4)) == ("R"));
      CHECK((varOrderNames.at(5)) == ("T"));

      delete dbgen;
    }

    static void testSetVarOrder() {
      gum::BayesNet< double >                       bn    = initAsia();
      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen = new gum::learning::BNDatabaseGenerator< double >(bn));

      std::vector< gum::Idx >    goodOrder1 = {1, 0, 3, 2, 5, 4};
      std::vector< std::string > goodOrder2 = {"A", "E", "O", "R", "S", "T"};
      std::vector< gum::Idx >    badOrder1  = {5, 1, 0, 3, 2, 5, 4};
      std::vector< std::string > badOrder2  = {"A", "E", "O", "R", "A", "S", "T"};
      std::vector< gum::Idx >    badOrder3  = {1, 0, 3, 5, 4};
      std::vector< std::string > badOrder4  = {"A", "O", "R", "S", "T"};

      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(goodOrder1));
      auto varOrderNames = dbgen->varOrderNames();
      CHECK((varOrderNames.size()) == (static_cast< gum::Size >(6)));
      CHECK((varOrderNames.at(1)) == ("A"));
      CHECK((varOrderNames.at(0)) == ("S"));
      CHECK((varOrderNames.at(3)) == ("E"));
      CHECK((varOrderNames.at(2)) == ("O"));
      CHECK((varOrderNames.at(5)) == ("R"));
      CHECK((varOrderNames.at(4)) == ("T"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen->setVarOrder(goodOrder2));
      auto varOrder = dbgen->varOrder();
      CHECK((varOrder.size()) == (static_cast< gum::Size >(6)));
      CHECK((varOrder.at(0)) == (static_cast< gum::Idx >(0)));
      CHECK((varOrder.at(1)) == (static_cast< gum::Idx >(2)));
      CHECK((varOrder.at(2)) == (static_cast< gum::Idx >(3)));
      CHECK((varOrder.at(3)) == (static_cast< gum::Idx >(4)));
      CHECK((varOrder.at(4)) == (static_cast< gum::Idx >(1)));
      CHECK((varOrder.at(5)) == (static_cast< gum::Idx >(5)));

      std::string csvFile = GET_RESSOURCES_PATH("csv/survey1.csv");
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen->setVarOrderFromCSV(csvFile));
      varOrderNames = dbgen->varOrderNames();
      CHECK((varOrderNames.size()) == (static_cast< gum::Size >(6)));
      CHECK((varOrderNames.at(0)) == ("E"));
      CHECK((varOrderNames.at(1)) == ("A"));
      CHECK((varOrderNames.at(2)) == ("O"));
      CHECK((varOrderNames.at(3)) == ("T"));
      CHECK((varOrderNames.at(4)) == ("R"));
      CHECK((varOrderNames.at(5)) == ("S"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen->setTopologicalVarOrder());
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen->setAntiTopologicalVarOrder());
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen->setRandomVarOrder());
      CHECK_THROWS_AS(dbgen->setVarOrder(badOrder1), const gum::FatalError&);
      CHECK_THROWS_AS(dbgen->setVarOrder(badOrder2), const gum::FatalError&);
      CHECK_THROWS_AS(dbgen->setVarOrder(badOrder3), const gum::FatalError&);
      CHECK_THROWS_AS(dbgen->setVarOrder(badOrder4), const gum::FatalError&);

      delete dbgen;
    }

    static void testDrawSamples() {
      gum::BayesNet< double > bn = initAsia();

      gum::Size domSizeA  = 3;
      gum::Size domSizeS  = 2;
      gum::Size domSizeE  = 2;
      gum::Size domSizeO  = 2;
      gum::Size domSizeR  = 2;
      gum::Size domSizeT  = 3;
      gum::Size nbSamples = 100;

      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen = new gum::learning::BNDatabaseGenerator< double >(bn));

      CHECK_THROWS_AS(dbgen->database(), const gum::OperationNotAllowed&);
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples));

      std::vector< std::vector< gum::Idx > > database;
      GUM_CHECK_ASSERT_THROWS_NOTHING(database = dbgen->database());

      for (const auto& row: database) {
        CHECK((row.at(0)) < (domSizeA));
        CHECK((row.at(1)) < (domSizeS));
        CHECK((row.at(2)) < (domSizeE));
        CHECK((row.at(3)) < (domSizeO));
        CHECK((row.at(4)) < (domSizeR));
        CHECK((row.at(5)) < (domSizeT));
      }

      std::vector< std::string > vOrder1 = {"S", "E", "T", "R", "A", "O"};
      CHECK_NOTHROW(dbgen->setVarOrder(vOrder1));
      GUM_CHECK_ASSERT_THROWS_NOTHING(database = dbgen->database());
      for (const auto& row: database) {
        CHECK((row.at(0)) < (domSizeS));
        CHECK((row.at(1)) < (domSizeE));
        CHECK((row.at(2)) < (domSizeT));
        CHECK((row.at(3)) < (domSizeR));
        CHECK((row.at(4)) < (domSizeA));
        CHECK((row.at(5)) < (domSizeO));
      }

      std::vector< std::string > vOrder2 = {"S", "T", "E", "R", "O", "A"};
      CHECK_NOTHROW(dbgen->setVarOrder(vOrder2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples));
      GUM_CHECK_ASSERT_THROWS_NOTHING(database = dbgen->database());
      for (const auto& row: database) {
        CHECK((row.at(0)) < (domSizeS));
        CHECK((row.at(1)) < (domSizeT));
        CHECK((row.at(2)) < (domSizeE));
        CHECK((row.at(3)) < (domSizeR));
        CHECK((row.at(4)) < (domSizeO));
        CHECK((row.at(5)) < (domSizeA));
      }
      delete dbgen;
    }

    static void testDrawSamplesLog2likelihood() {
      gum::BayesNet< double > bn         = initAsia();
      gum::Size               nbSamples1 = 100;
      gum::Size               nbSamples2 = nbSamples1 * 100;
      gum::Size               nbSamples3 = nbSamples1 * 1000;

      double ll_1      = 0;
      double ll_2      = 0;
      double ll_3      = 0;
      double tolerance = 0.1;

      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen = new gum::learning::BNDatabaseGenerator< double >(bn));
      CHECK_THROWS_AS(dbgen->database(), const gum::OperationNotAllowed&);
      GUM_CHECK_ASSERT_THROWS_NOTHING(ll_1 = dbgen->drawSamples(nbSamples1));
      GUM_CHECK_ASSERT_THROWS_NOTHING(ll_2 = dbgen->drawSamples(nbSamples2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(ll_3 = dbgen->drawSamples(nbSamples3));

      // log2likehood must proportional to number of samples
      CHECK((std::abs(1 - (double)nbSamples2 / (double)nbSamples1 * ll_1 / ll_2)) < (tolerance));
      CHECK((std::abs(1 - (double)nbSamples3 / (double)nbSamples1 * ll_1 / ll_3)) < (tolerance));

      // log2likelihood must be aprox nbSamples * entropy (theorical result)
      double entropy
          = (bn.cpt(0) * bn.cpt(1) * bn.cpt(2) * bn.cpt(3) * bn.cpt(4) * bn.cpt(5)).entropy();
      CHECK((std::abs(1 + entropy * nbSamples1 / ll_1)) < (tolerance));
      CHECK((std::abs(1 + entropy * nbSamples2 / ll_2)) < (tolerance));
      CHECK((std::abs(1 + entropy * nbSamples3 / ll_3)) < (tolerance));

      delete dbgen;
    }

    static void testToCSV_1() {
      gum::BayesNet< double > bn        = initAsia();
      gum::Size               nbSamples = 5;

      std::vector< std::string > domA = {"young", "adult", "old"};
      std::vector< std::string > domS = {"M", "F"};
      std::vector< std::string > domE = {"high", "uni"};
      std::vector< std::string > domO = {"emp", "self"};
      std::vector< std::string > domR = {"small", "big"};
      std::vector< std::string > domT = {"car", "train", "other"};

      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen = new gum::learning::BNDatabaseGenerator< double >(bn));

      std::vector< std::string > vOrder1 = {"S", "E", "T", "R", "A", "O"};
      CHECK_NOTHROW(dbgen->setVarOrder(vOrder1));
      std::string csvFileURL    = GET_RESSOURCES_PATH("outputs/survey_tmp1.csv");
      bool        useLabels     = true;
      bool        append        = false;
      std::string csvSeparator  = " ";
      bool        checkOnAppend = true;

      CHECK_THROWS_AS(dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend),
                      const gum::OperationNotAllowed&);

      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples));
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend));

      std::vector< std::string > vOrder2 = {"S", "T", "E", "R", "O", "A"};
      CHECK_NOTHROW(dbgen->setVarOrder(vOrder2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples));
      append = true;
      CHECK_THROWS_AS(dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend),
                      const gum::OperationNotAllowed&);
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen->setVarOrderFromCSV(csvFileURL, csvSeparator));
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend));

      std::ifstream              csvFile(csvFileURL);
      std::string                line;
      std::vector< std::string > header;
      std::getline(csvFile, line);
      CHECK((line) == ("S E T R A O"));

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

        CHECK((std::find(domA.begin(), domA.end(), valA)) != (domA.end()));
        CHECK((std::find(domS.begin(), domS.end(), valS)) != (domS.end()));
        CHECK((std::find(domE.begin(), domE.end(), valE)) != (domE.end()));
        CHECK((std::find(domO.begin(), domO.end(), valO)) != (domO.end()));
        CHECK((std::find(domR.begin(), domR.end(), valR)) != (domR.end()));
        CHECK((std::find(domT.begin(), domT.end(), valT)) != (domT.end()));
      }
      csvFile.close();

      delete dbgen;
    }

    static void testToCSV_2() {
      gum::BayesNet< double > bn        = initAsia();
      gum::Size               nbSamples = 5;

      gum::Size domSizeA = 3;
      gum::Size domSizeS = 2;
      gum::Size domSizeE = 2;
      gum::Size domSizeO = 2;
      gum::Size domSizeR = 2;
      gum::Size domSizeT = 3;

      gum::learning::BNDatabaseGenerator< double >* dbgen = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen = new gum::learning::BNDatabaseGenerator< double >(bn));

      std::vector< std::string > vOrder1 = {"S", "E", "T", "R", "A", "O"};
      CHECK_NOTHROW(dbgen->setVarOrder(vOrder1));
      std::string csvFileURL    = GET_RESSOURCES_PATH("outputs/survey_tmp2.csv");
      bool        useLabels     = false;
      bool        append        = false;
      std::string csvSeparator  = " ";
      bool        checkOnAppend = true;

      CHECK_THROWS_AS(dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend),
                      const gum::OperationNotAllowed&);

      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples));
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend));

      std::vector< std::string > vOrder2 = {"S", "T", "E", "R", "O", "A"};
      CHECK_NOTHROW(dbgen->setVarOrder(vOrder2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples));
      append = true;
      CHECK_THROWS_AS(dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend),
                      const gum::OperationNotAllowed&);
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen->setVarOrderFromCSV(csvFileURL, csvSeparator));
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          dbgen->toCSV(csvFileURL, useLabels, append, csvSeparator, checkOnAppend));

      std::ifstream              csvFile(csvFileURL);
      std::string                line;
      std::vector< std::string > header;
      std::getline(csvFile, line);
      CHECK((line) == ("S E T R A O"));

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

        CHECK((valS) < (domSizeS));
        CHECK((valE) < (domSizeE));
        CHECK((valT) < (domSizeT));
        CHECK((valR) < (domSizeR));
        CHECK((valA) < (domSizeA));
        CHECK((valO) < (domSizeO));
      }
      csvFile.close();

      delete dbgen;
    }

    static void testToDatabaseTable() {
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
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen = new gum::learning::BNDatabaseGenerator< double >(bn));
      CHECK_THROWS_AS(dbgen->toDatabaseTable(), const gum::OperationNotAllowed&);
      GUM_CHECK_ASSERT_THROWS_NOTHING(dbgen->drawSamples(nbSamples));
      gum::learning::DatabaseTable db;
      bool                         useLabels = true;
      CHECK_NOTHROW(db = dbgen->toDatabaseTable(useLabels));

      auto handler = db.handler();
      while (handler.hasRows()) {
        auto row = handler.row();
        CHECK((std::find(domA.begin(), domA.end(), db.translator(0).translateBack(row[0])))
              != (domA.end()));
        CHECK((std::find(domS.begin(), domS.end(), db.translator(1).translateBack(row[1])))
              != (domS.end()));
        CHECK((std::find(domE.begin(), domE.end(), db.translator(2).translateBack(row[2])))
              != (domE.end()));
        CHECK((std::find(domO.begin(), domO.end(), db.translator(3).translateBack(row[3])))
              != (domO.end()));
        CHECK((std::find(domR.begin(), domR.end(), db.translator(4).translateBack(row[4])))
              != (domR.end()));
        CHECK((std::find(domT.begin(), domT.end(), db.translator(5).translateBack(row[5])))
              != (domT.end()));
        handler.nextRow();
      }

      useLabels = false;
      CHECK_NOTHROW(db = dbgen->toDatabaseTable(useLabels));
      handler = db.handler();
      while (handler.hasRows()) {
        auto row = handler.row();
        CHECK((row[0].discr_val) < (domSizeA));
        CHECK((row[1].discr_val) < (domSizeS));
        CHECK((row[2].discr_val) < (domSizeE));
        CHECK((row[3].discr_val) < (domSizeO));
        CHECK((row[4].discr_val) < (domSizeR));
        CHECK((row[5].discr_val) < (domSizeT));
        handler.nextRow();
      }

      std::vector< std::string > vOrder1 = {"S", "E", "T", "R", "A", "O"};
      CHECK_NOTHROW(dbgen->setVarOrder(vOrder1));

      useLabels = true;
      CHECK_NOTHROW(db = dbgen->toDatabaseTable(useLabels));
      handler = db.handler();
      while (handler.hasRows()) {
        auto row = handler.row();


        CHECK((std::find(domS.begin(), domS.end(), db.translator(0).translateBack(row[0])))
              != (domS.end()));
        CHECK((std::find(domE.begin(), domE.end(), db.translator(1).translateBack(row[1])))
              != (domE.end()));
        CHECK((std::find(domT.begin(), domT.end(), db.translator(2).translateBack(row[2])))
              != (domT.end()));
        CHECK((std::find(domR.begin(), domR.end(), db.translator(3).translateBack(row[3])))
              != (domR.end()));
        CHECK((std::find(domA.begin(), domA.end(), db.translator(4).translateBack(row[4])))
              != (domA.end()));
        CHECK((std::find(domO.begin(), domO.end(), db.translator(5).translateBack(row[5])))
              != (domO.end()));

        handler.nextRow();
      }

      useLabels = false;
      CHECK_NOTHROW(db = dbgen->toDatabaseTable(useLabels));
      handler = db.handler();
      while (handler.hasRows()) {
        auto row = handler.row();
        CHECK((row[0].discr_val) < (domSizeS));
        CHECK((row[1].discr_val) < (domSizeE));
        CHECK((row[2].discr_val) < (domSizeT));
        CHECK((row[3].discr_val) < (domSizeR));
        CHECK((row[4].discr_val) < (domSizeA));
        CHECK((row[5].discr_val) < (domSizeO));
        handler.nextRow();
      }

      delete dbgen;
    }

    static void testListenToDrawSamples() {
      gum::BayesNet< double >                      bn = initAsia();
      gum::learning::BNDatabaseGenerator< double > dbgen(bn);

      ASimpleDBGeneratorListener gener(dbgen);
      CHECK((gener.getNbr()) == (static_cast< gum::Size >(0)));
      CHECK((gener.getMess()) == (""));
      dbgen.drawSamples(100);
      CHECK((gener.getNbr()) == (static_cast< gum::Size >(4950)));
      CHECK((gener.getMess()) != (""));

      ASimpleDBGeneratorListener gener2(dbgen);
      CHECK((gener2.getNbr()) == (static_cast< gum::Size >(0)));
      CHECK((gener2.getMess()) == (""));
      dbgen.drawSamples(1000);
      CHECK((gener2.getNbr()) == (static_cast< gum::Size >(4950)));
      CHECK((gener2.getMess()) != (""));
    }

    static void testDrawingWithEvidence() {
      gum::BayesNet< double >                      bn = initAsia();
      gum::learning::BNDatabaseGenerator< double > dbgen(bn);
      ASimpleDBGeneratorListener                   gener(dbgen);

      gum::Instantiation filter;
      filter.add(bn.variable(0));
      filter.setFirst();
      dbgen.drawSamples(100, filter);
      CHECK((dbgen.samplesNbRows()) < (101u));   // some samples have been rejected
    }

    static void testAccuracy() {
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

      CHECK(((bn2.cpt("A")
              - (gum::Tensor< double >() << bn2.variable("A")).fillWith(ie.posterior("A")))
                 .abs()
                 .max())
            < (1e-2));
      CHECK(((bn2.cpt("B")
              - (gum::Tensor< double >() << bn2.variable("B")).fillWith(ie.posterior("B")))
                 .abs()
                 .max())
            < (1e-2));
    }

    static void testAccuracyWithEvidence() {
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

      CHECK(((bn2.cpt("A")
              - (gum::Tensor< double >() << bn2.variable("A")).fillWith(ie.posterior("A")))
                 .abs()
                 .max())
            < (1e-2));
      CHECK(((bn2.cpt("B")
              - (gum::Tensor< double >() << bn2.variable("B")).fillWith(ie.posterior("B")))
                 .abs()
                 .max())
            < (1e-2));
    }

    static void testDiscretizedVariableTranslation() {
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
          CHECK(gum::isNumericalWithResult(dummyLine, &d));
          CHECK(d >= 1.5);
          CHECK(d <= 14);
        }
        CHECK((pos++) == (nbLines));
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
          CHECK(gum::isNumericalWithResult(dummyLine, &d));
          CHECK(((d == 1.75) || (d == 2.25) || (d == 6.25) || (d == 12)));
        }
        CHECK((pos) == (nbLines));
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
          CHECK((dummyLine.front()) == ('['));
        }
        CHECK((pos) == (nbLines));
      }
    }

    static void testInfiniteRejectionSampling() {
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
      CHECK((timer.step()) < (5.5));
      CHECK((dbgen.samplesNbRows()) == (0u));
    }
  };

  GUM_TEST_ACTIF(Constuctor)
  GUM_TEST_ACTIF(SetVarOrder)
  GUM_TEST_ACTIF(DrawSamples)
  GUM_TEST_ACTIF(DrawSamplesLog2likelihood)
  GUM_TEST_ACTIF(ToCSV_1)
  GUM_TEST_ACTIF(ToCSV_2)
  GUM_TEST_ACTIF(ToDatabaseTable)
  GUM_TEST_ACTIF(ListenToDrawSamples)
  GUM_TEST_ACTIF(DrawingWithEvidence)
  GUM_TEST_ACTIF(Accuracy)
  GUM_TEST_ACTIF(AccuracyWithEvidence)
  GUM_TEST_ACTIF(DiscretizedVariableTranslation)
  GUM_TEST_ACTIF(InfiniteRejectionSampling)
}   // namespace gum_tests
