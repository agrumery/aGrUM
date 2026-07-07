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


#include <fstream>
#include <iostream>
#include <string>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/DSL/DSLReader.h>
#include <agrum/BN/io/DSL/DSLWriter.h>

#include <agrum/base/core/math/math_utils.h>
#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DSLReader
#define GUM_CURRENT_MODULE BN

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5
namespace gum_tests {
  struct DSLReaderTestSuite {
    public:
    static void testConstuctor() {
      std::string             file = GET_RESSOURCES_PATH("DSL/DSLReader_file1.txt");
      gum::BayesNet< double > net;

      gum::DSLReader< double >* reader = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(reader = new gum::DSLReader< double >(&net, file));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete reader);
    }   // namespace gum_tests

    static void testRead_file1() {
      std::string              file = GET_RESSOURCES_PATH("DSL/DSLReader_file1.txt");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);

      reader.trace(false);

      gum::Size nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
      CHECK_EQ(reader.warnings(), static_cast< gum::Size >(0));
      // 0 warnings : no properties
      CHECK_EQ(reader.errors(), static_cast< gum::Size >(0));
      CHECK_NE(net, nullptr);

      if (net != nullptr) {
        CHECK(net->empty());
        delete net;
      }
    }

    static void testRead_file2_float() {
      std::string              file = GET_RESSOURCES_PATH("DSL/DSLReader_file2.txt");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
      CHECK_EQ(reader.warnings(), static_cast< gum::Size >(0));
      // 0 warnings : no properties
      CHECK_EQ(reader.errors(), static_cast< gum::Size >(0));

      CHECK_NE(net, nullptr);

      if (net != nullptr) {
        CHECK_EQ(net->size(), static_cast< gum::Size >(2));
        gum::NodeId node_1 = 0, node_2 = 0;

        for (const auto node: net->nodes())
          if (net->variable(node).name() == "n1") node_1 = node;
          else node_2 = node;

        const gum::DiscreteVariable& var_1 = net->variable(node_1);

        CHECK_EQ(var_1.name(), "n1");
        CHECK_EQ(var_1.domainSize(), static_cast< gum::Size >(2));
        const gum::Tensor< double >& proba_1 = net->cpt(node_1);
        CHECK_EQ(proba_1.domainSize(), static_cast< gum::Size >(2));
        gum::Instantiation inst_1(proba_1);
        inst_1.setFirst();
        CHECK(std::abs((proba_1[inst_1] - 0.2f)) < 0.001f);
        inst_1.setLast();
        CHECK(std::abs((proba_1[inst_1] - 0.8f)) < 0.001f);

        const gum::DiscreteVariable& var_2 = net->variable(node_2);
        CHECK_EQ(var_2.name(), "n2");
        CHECK_EQ(var_2.domainSize(), static_cast< gum::Size >(2));
        const gum::Tensor< double >& proba_2 = net->cpt(node_2);
        CHECK_EQ(proba_2.domainSize(), static_cast< gum::Size >(2));
        gum::Instantiation inst_2(proba_2);
        inst_2.setFirst();
        CHECK(std::abs((proba_2[inst_2] - 0.3f)) < 0.001f);
        inst_2.setLast();
        CHECK(std::abs((proba_2[inst_2] - 0.7f)) < 0.001f);

        delete net;
      }
    }

    static void testRead_file2_double() {
      std::string              file = GET_RESSOURCES_PATH("DSL/DSLReader_file2.txt");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);

      gum::Size nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
      CHECK_EQ(reader.warnings(), static_cast< gum::Size >(0));
      CHECK_EQ(reader.errors(), static_cast< gum::Size >(0));

      CHECK_NE(net, nullptr);

      if (net != nullptr) {
        CHECK_EQ(net->size(), static_cast< gum::Size >(2));
        gum::NodeId node_1 = 0, node_2 = 0;

        for (const auto node: net->nodes())
          if (net->variable(node).name() == "n1") node_1 = node;
          else node_2 = node;

        const gum::DiscreteVariable& var_1 = net->variable(node_1);

        CHECK_EQ(var_1.name(), "n1");
        CHECK_EQ(var_1.domainSize(), static_cast< gum::Size >(2));
        const gum::Tensor< double >& proba_1 = net->cpt(node_1);
        CHECK_EQ(proba_1.domainSize(), static_cast< gum::Size >(2));
        gum::Instantiation inst_1(proba_1);
        inst_1.setFirst();
        CHECK(std::abs((proba_1[inst_1] - 0.2)) < 0.001);
        inst_1.setLast();
        CHECK(std::abs((proba_1[inst_1] - 0.8)) < 0.001);

        const gum::DiscreteVariable& var_2 = net->variable(node_2);
        CHECK_EQ(var_2.name(), "n2");
        CHECK_EQ(var_2.domainSize(), static_cast< gum::Size >(2));
        const gum::Tensor< double >& proba_2 = net->cpt(node_2);
        CHECK_EQ(proba_2.domainSize(), static_cast< gum::Size >(2));
        gum::Instantiation inst_2(proba_2);
        inst_2.setFirst();
        CHECK(std::abs((proba_2[inst_2] - 0.3)) < 0.001);
        inst_2.setLast();
        CHECK(std::abs((proba_2[inst_2] - 0.7)) < 0.001);

        delete net;
      }
    }

    static void testRead_file3() {
      std::string              file = GET_RESSOURCES_PATH("DSL/DSLReader_file3.txt");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
      CHECK_EQ(reader.warnings(), static_cast< gum::Size >(0));
      // 0 warnings : no properties
      CHECK_EQ(reader.errors(), static_cast< gum::Size >(0));
      CHECK_EQ(net->size(), static_cast< gum::Size >(6));
      CHECK_NE(net, nullptr);
      gum::DSLWriter< double > writer;
      std::string              file2 = GET_RESSOURCES_PATH("outputs/DSLWriter_TestFile3.txt");
      GUM_CHECK_ASSERT_THROWS_NOTHING(writer.write(file2, *net));

      if (net) delete net;
    }

    static void testRead_file_completeDSL() {
      std::string              file = GET_RESSOURCES_PATH("DSL/Ling.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      reader.trace(true);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
      CHECK_EQ(reader.warnings(), static_cast< gum::Size >(0));
      // 0 warnings : no properties
      CHECK_EQ(reader.errors(), static_cast< gum::Size >(0));
      CHECK_EQ(net->size(), static_cast< gum::Size >(13));
      CHECK_NE(net, nullptr);
      gum::DSLWriter< double > writer;
      std::string              file2 = GET_RESSOURCES_PATH("outputs/DSLWriter_Ling.txt");
      GUM_CHECK_ASSERT_THROWS_NOTHING(writer.write(file2, *net));

      if (net) delete net;
    }

    static void testAlarm() {
      std::string              file = GET_RESSOURCES_PATH("DSL/alarm.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);

      gum::Size nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
      CHECK_EQ(reader.warnings(), static_cast< gum::Size >(0));
      CHECK_EQ(reader.errors(), static_cast< gum::Size >(0));

      gum::HashTable< std::string, gum::NodeId > idMap;

      for (const auto node: net->nodes())
        idMap.insert(net->variable(node).name(), node);

      // The node wich we'll test
      CHECK(idMap.exists("HISTORY"));
      // It's parent
      CHECK(idMap.exists("LVFAILURE"));

      if (idMap.exists("HISTORY") && idMap.exists("LVFAILURE")) {
        const gum::DiscreteVariable& history = net->variable(idMap["HISTORY"]);
        CHECK_EQ(history.domainSize(), static_cast< gum::Size >(2));
        CHECK_EQ(history.label(0), "TRUE");
        CHECK_EQ(history.label(1), "FALSE");
        CHECK(net->dag().existsArc(idMap["LVFAILURE"], idMap["HISTORY"]));
        const gum::Tensor< double >& historyCPT = net->cpt(idMap["HISTORY"]);
        CHECK_EQ(historyCPT.domainSize(), static_cast< gum::Size >(4));
        CHECK(historyCPT.contains(net->variable(idMap["HISTORY"])));
        CHECK(historyCPT.contains(net->variable(idMap["LVFAILURE"])));
        gum::Instantiation historyInst(historyCPT);
        // (TRUE | TRUE)
        historyInst.chgVal(history, 0);
        historyInst.chgVal(net->variable(idMap["LVFAILURE"]), 0);
        CHECK(std::abs(historyCPT[historyInst] - 0.9f) < 0.0001f);
        // (FALSE | TRUE)
        historyInst.chgVal(history, 1);
        historyInst.chgVal(net->variable(idMap["LVFAILURE"]), 0);
        CHECK(std::abs(historyCPT[historyInst] - 0.1f) < 0.0001f);
        // (TRUE | FALSE)
        historyInst.chgVal(history, 0);
        historyInst.chgVal(net->variable(idMap["LVFAILURE"]), 1);
        CHECK(std::abs(historyCPT[historyInst] - 0.01f) < 0.0001f);
        // (FALSE | FALSE)
        historyInst.chgVal(history, 1);
        historyInst.chgVal(net->variable(idMap["LVFAILURE"]), 1);
        CHECK(std::abs(historyCPT[historyInst] - 0.99f) < 0.0001f);
      }

      // The node wich we'll test
      CHECK(idMap.exists("ERRLOWOUTPUT"));
      // It's Children
      CHECK(idMap.exists("HRBP"));

      if (idMap.exists("ERRLOWOUTPUT") && idMap.exists("HRBP")) {
        const gum::DiscreteVariable& errlowoutput = net->variable(idMap["ERRLOWOUTPUT"]);
        CHECK_EQ(errlowoutput.domainSize(), static_cast< gum::Size >(2));
        CHECK_EQ(errlowoutput.label(0), "TRUE");
        CHECK_EQ(errlowoutput.label(1), "FALSE");
        CHECK(net->dag().existsArc(idMap["ERRLOWOUTPUT"], idMap["HRBP"]));
        const gum::Tensor< double >& errlowoutputCPT = net->cpt(idMap["ERRLOWOUTPUT"]);
        CHECK_EQ(errlowoutputCPT.domainSize(), static_cast< gum::Size >(2));
        CHECK(errlowoutputCPT.contains(errlowoutput));
        gum::Instantiation errlowoutputInst(errlowoutputCPT);
        errlowoutputInst.chgVal(errlowoutput, 0);
        CHECK(std::abs(errlowoutputCPT[errlowoutputInst] - 0.05f) < 0.001f);
        errlowoutputInst.chgVal(errlowoutput, 1);
        CHECK(std::abs(errlowoutputCPT[errlowoutputInst] - 0.95f) < 0.001f);
      }

      // The nide wich we'll test
      CHECK(idMap.exists("LVEDVOLUME"));
      // It's parents
      CHECK(idMap.exists("HYPOVOLEMIA"));
      CHECK(idMap.exists("LVFAILURE"));

      if (idMap.exists("LVEDVOLUME") && idMap.exists("HYPOVOLEMIA") && idMap.exists("LVFAILURE")) {
        const gum::DiscreteVariable& lvedvolume  = net->variable(idMap["LVEDVOLUME"]);
        const gum::DiscreteVariable& hypovolemia = net->variable(idMap["HYPOVOLEMIA"]);
        const gum::DiscreteVariable& lvfailure   = net->variable(idMap["LVFAILURE"]);
        // checking label order
        CHECK_EQ(lvedvolume.label(0), "LOW");
        CHECK_EQ(lvedvolume.label(1), "NORMAL");
        CHECK_EQ(lvedvolume.label(2), "HIGH");
        CHECK_EQ(hypovolemia.label(0), "TRUE");
        CHECK_EQ(hypovolemia.label(1), "FALSE");
        CHECK_EQ(lvfailure.label(0), "TRUE");
        CHECK_EQ(lvfailure.label(1), "FALSE");
        const gum::Tensor< double >& cpt = net->cpt(idMap["LVEDVOLUME"]);
        gum::Instantiation           inst(cpt);
        gum::Instantiation           var_inst;
        var_inst << lvedvolume;
        inst.chgVal(hypovolemia, 0);
        inst.chgVal(lvfailure, 0);
        float  array_1[] = {0.95f, 0.04f, 0.01f};
        size_t i         = 0;

        for (inst.setFirstIn(var_inst); !inst.end(); inst.incIn(var_inst), ++i) {
          CHECK((cpt[inst]) == doctest::Approx(array_1[i]).epsilon(0.001f));
        }

        inst.chgVal(hypovolemia, 1);
        inst.chgVal(lvfailure, 0);
        float array_2[] = {0.98f, 0.01f, 0.01f};
        i               = 0;

        for (inst.setFirstIn(var_inst); !inst.end(); inst.incIn(var_inst), ++i) {
          CHECK((cpt[inst]) == doctest::Approx(array_2[i]).epsilon(0.001f));
        }

        inst.chgVal(hypovolemia, 0);
        inst.chgVal(lvfailure, 1);
        float array_3[] = {0.01f, 0.09f, 0.9f};
        i               = 0;

        for (inst.setFirstIn(var_inst); !inst.end(); inst.incIn(var_inst), ++i) {
          CHECK((cpt[inst]) == doctest::Approx(array_3[i]).epsilon(0.001f));
        }

        inst.chgVal(hypovolemia, 1);
        inst.chgVal(lvfailure, 1);
        float array_4[] = {0.05f, 0.9f, 0.05f};
        i               = 0;

        for (inst.setFirstIn(var_inst); !inst.end(); inst.incIn(var_inst), ++i) {
          CHECK((cpt[inst]) == doctest::Approx(array_4[i]).epsilon(0.001f));
        }
      }

      delete net;
    }

    static void testUnexisting() {
      std::string              file = "Schmurtz";
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::DSLReader< double > reader(net, file));
      gum::DSLReader< double > reader(net, file);
      CHECK_THROWS_AS(reader.proceed(), const gum::IOError&);

      if (net) delete net;
    }

    static void testBarley() {
      std::string              file = GET_RESSOURCES_PATH("DSL/Barley.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
      CHECK_EQ(reader.warnings(), static_cast< gum::Size >(0));
      CHECK_EQ(reader.errors(), static_cast< gum::Size >(0));

      if (net) delete net;
    }

    static void testCarpo() {
      std::string              file = GET_RESSOURCES_PATH("DSL/carpo.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
      CHECK_EQ(reader.warnings(), static_cast< gum::Size >(0));
      CHECK_EQ(reader.errors(), static_cast< gum::Size >(0));

      if (net) delete net;
    }

    static void testDiabetes() {
      std::string              file = GET_RESSOURCES_PATH("DSL/Diabetes.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
      CHECK_EQ(reader.warnings(), static_cast< gum::Size >(0));
      CHECK_EQ(reader.errors(), static_cast< gum::Size >(0));

      if (net) delete net;
    }

    static void testHailfinder() {
      std::string              file = GET_RESSOURCES_PATH("DSL/hailfinder.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
      CHECK_EQ(reader.warnings(), static_cast< gum::Size >(0));
      CHECK_EQ(reader.errors(), static_cast< gum::Size >(0));

      if (net) delete net;
    }

    static void testInsurance() {
      std::string              file = GET_RESSOURCES_PATH("DSL/insurance.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
      CHECK_EQ(reader.warnings(), static_cast< gum::Size >(0));
      CHECK_EQ(reader.errors(), static_cast< gum::Size >(0));

      if (net) delete net;
    }

    static void testLink() {
      std::string              file = GET_RESSOURCES_PATH("DSL/Link.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
      CHECK_EQ(reader.warnings(), static_cast< gum::Size >(0));
      CHECK_EQ(reader.errors(), static_cast< gum::Size >(0));

      if (net) delete net;
    }

    // Mildew is not readable (because of the VERY LARGE FLOAT_LIST in foto_4
    // (at
    // least))...
    void Mildew() {
      std::string              file = GET_RESSOURCES_PATH("DSL/Mildew.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
      CHECK_EQ(reader.warnings(), static_cast< gum::Size >(0));
      CHECK_EQ(reader.errors(), static_cast< gum::Size >(0));

      if (net) delete net;
    }

    static void testMunin1() {
      std::string              file = GET_RESSOURCES_PATH("DSL/Munin1.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
      CHECK_EQ(reader.warnings(), static_cast< gum::Size >(0));
      CHECK_EQ(reader.errors(), static_cast< gum::Size >(0));

      if (net) delete net;
    }

    static void testPigs() {
      std::string              file = GET_RESSOURCES_PATH("DSL/Pigs.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
      CHECK_EQ(reader.warnings(), static_cast< gum::Size >(0));
      CHECK_EQ(reader.errors(), static_cast< gum::Size >(0));

      if (net) delete net;
    }

    static void testWater() {
      std::string              file = GET_RESSOURCES_PATH("DSL/Water.dsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::DSLReader< double > reader(net, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
      CHECK_EQ(reader.warnings(), static_cast< gum::Size >(0));
      CHECK_EQ(reader.errors(), static_cast< gum::Size >(0));

      if (net) delete net;
    }

    static void testDSLNameRoundtrip() {
      // Regression: DSLReader STRING rule was not stripping surrounding quotes
      // from the network name token, producing e.g. '"SavedBN"' instead of 'SavedBN'.
      std::string file = GET_RESSOURCES_PATH("outputs/DSLReader_name_roundtrip.txt");
      {
        std::ofstream out(file);
        out << "net \"SavedBN\"\n{\n};\n";
      }
      gum::BayesNet< double >  loadedBN;
      gum::DSLReader< double > reader(&loadedBN, file);
      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());
      CHECK_EQ(loadedBN.property("name"), "SavedBN");
    }
  };

  GUM_TEST_ACTIF(Constuctor)
  GUM_TEST_ACTIF(Read_file1)
  GUM_TEST_ACTIF(Read_file2_float)
  GUM_TEST_ACTIF(Read_file2_double)
  GUM_TEST_ACTIF(Read_file3)
  GUM_TEST_ACTIF(Read_file_completeDSL)
  GUM_TEST_ACTIF(Alarm)
  GUM_TEST_ACTIF(Unexisting)
  GUM_TEST_ACTIF(Barley)
  GUM_TEST_ACTIF(Carpo)
  GUM_TEST_ACTIF(Diabetes)
  GUM_TEST_ACTIF(Hailfinder)
  GUM_TEST_ACTIF(Insurance)
  GUM_TEST_ACTIF(Link)
  GUM_TEST_ACTIF(Munin1)
  GUM_TEST_ACTIF(Pigs)
  GUM_TEST_ACTIF(Water)
  GUM_TEST_ACTIF(DSLNameRoundtrip)
}   // namespace gum_tests
