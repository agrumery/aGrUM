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
#include <sstream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNReader.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNWriter.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  BIFXMLBNReader
#define GUM_CURRENT_MODULE BN

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  struct BIFXMLBNReaderTestSuite {
    public:
    static void testConstuctor() {
      std::string             file = GET_RESSOURCES_PATH("bifxml/BNBIFXMLReader_file1.bifxml");
      gum::BayesNet< double > net;

      gum::BIFXMLBNReader< double >* reader = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(reader = new gum::BIFXMLBNReader< double >(&net, file));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete reader);
    }   // namespace gum_tests

    static void testRead_file1() {
      std::string              file = GET_RESSOURCES_PATH("bifxml/BNBIFXMLReader_file1.bifxml");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();

      CHECK_NE(net, nullptr);

      gum::BIFXMLBNReader< double > reader(net, file);
      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      if (net != nullptr) {
        CHECK(net->empty());
        delete net;
      }
    }

    static void testRead_file2_float() {
      std::string              file = GET_RESSOURCES_PATH("bifxml/BNBIFXMLReader_file2.bifxml");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, file);

      gum::Size isOK = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(isOK = reader.proceed());
      CHECK_EQ(isOK, static_cast< gum::Size >(0));

      CHECK_NE(net, nullptr);

      if (net != nullptr) {
        CHECK(!net->empty());
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
        CHECK_EQ(proba_1[inst_1], doctest::Approx(0.2f).epsilon(0.001f));
        inst_1.setLast();
        CHECK_EQ(proba_1[inst_1], doctest::Approx(0.8f).epsilon(0.001f));

        const gum::DiscreteVariable& var_2 = net->variable(node_2);
        CHECK_EQ(var_2.name(), "n2");
        CHECK_EQ(var_2.domainSize(), static_cast< gum::Size >(2));

        const gum::Tensor< double >& proba_2 = net->cpt(node_2);
        CHECK_EQ(proba_2.domainSize(), static_cast< gum::Size >(2));

        gum::Instantiation inst_2(proba_2);
        inst_2.setFirst();
        CHECK_EQ(proba_2[inst_2], doctest::Approx(0.3f).epsilon(0.001f));
        inst_2.setLast();
        CHECK_EQ(proba_2[inst_2], doctest::Approx(0.7f).epsilon(0.001f));
        delete net;
      }
    }

    static void testRead_dog_double() {
      // from Charniak, Bayesian networks Without Tears, AI Magazine, 1991
      std::string                   file = GET_RESSOURCES_PATH("bifxml/dog.bifxml");
      auto                          net  = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, file);

      gum::Size isOK = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(isOK = reader.proceed());
      CHECK_EQ(isOK, static_cast< gum::Size >(0));

      CHECK_NE(net, nullptr);

      if (net != nullptr) {
        CHECK_EQ(net->size(), static_cast< gum::Size >(5));

        const gum::Tensor< double >& proba = net->cpt(net->idFromName("dog-out"));

        CHECK_EQ(proba.domainSize(), static_cast< gum::Size >(8));

        delete (net);
      }
    }

    static void testRead_file2_double() {
      std::string              file = GET_RESSOURCES_PATH("bifxml/BNBIFXMLReader_file2.bifxml");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, file);

      gum::Size isOK = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(isOK = reader.proceed());
      CHECK_EQ(isOK, static_cast< gum::Size >(0));

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

        CHECK_EQ((proba_1[inst_1]), doctest::Approx(0.2f).epsilon(0.001f));

        inst_1.setLast();

        CHECK_EQ((proba_1[inst_1]), doctest::Approx(0.8f).epsilon(0.001f));

        const gum::DiscreteVariable& var_2 = net->variable(node_2);

        CHECK_EQ(var_2.name(), "n2");

        CHECK_EQ(var_2.domainSize(), static_cast< gum::Size >(2));

        const gum::Tensor< double >& proba_2 = net->cpt(node_2);

        CHECK_EQ(proba_2.domainSize(), static_cast< gum::Size >(2));

        gum::Instantiation inst_2(proba_2);

        inst_2.setFirst();

        CHECK_EQ((proba_2[inst_2]), doctest::Approx(0.3f).epsilon(0.001f));

        inst_2.setLast();

        CHECK_EQ((proba_2[inst_2]), doctest::Approx(0.7f).epsilon(0.001f));

        delete net;
      }
    }

    static void testRead_file3() {
      std::string              file = GET_RESSOURCES_PATH("bifxml/BNBIFXMLReader_file3.bifxml");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, file);

      gum::Size isOK = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(isOK = reader.proceed());
      CHECK_EQ(isOK, static_cast< gum::Size >(0));
      CHECK_NE(net, nullptr);

      if (net != nullptr) {
        gum::HashTable< std::string, gum::NodeId > idMap;

        for (const auto node: net->nodes())
          idMap.insert(net->variable(node).name(), node);

        const gum::DiscreteVariable& var_1 = net->variable(idMap["n1"]);

        CHECK_EQ(var_1.name(), "n1");

        CHECK_EQ(var_1.domainSize(), static_cast< gum::Size >(2));

        CHECK_EQ(var_1.label(0), "0");

        CHECK_EQ(var_1.label(1), "1");

        const gum::Tensor< double >& proba_1 = net->cpt(idMap["n1"]);

        CHECK_EQ(proba_1.domainSize(), static_cast< gum::Size >(2));

        gum::Instantiation inst_1(proba_1);

        inst_1.setFirst();

        CHECK_EQ((proba_1[inst_1]), doctest::Approx(0.2f).epsilon(0.001f));

        inst_1.setLast();

        CHECK_EQ((proba_1[inst_1]), doctest::Approx(0.8f).epsilon(0.001f));

        const gum::DiscreteVariable& var_2 = net->variable(idMap["n2"]);

        CHECK_EQ(var_2.name(), "n2");

        CHECK_EQ(var_2.domainSize(), static_cast< gum::Size >(2));

        CHECK_EQ(var_2.label(0), "foo");

        CHECK_EQ(var_2.label(1), "bar");

        const gum::Tensor< double >& proba_2 = net->cpt(idMap["n2"]);

        CHECK_EQ(proba_2.domainSize(), static_cast< gum::Size >(2));

        gum::Instantiation inst_2(proba_2);

        inst_2.setFirst();

        CHECK_EQ((proba_2[inst_2]), doctest::Approx(0.3f).epsilon(0.001f));

        inst_2.setLast();

        CHECK_EQ((proba_2[inst_2]), doctest::Approx(0.7f).epsilon(0.001f));

        const gum::DiscreteVariable& var_3 = net->variable(idMap["n3"]);

        CHECK_EQ(var_3.name(), "n3");

        CHECK_EQ(var_3.domainSize(), static_cast< gum::Size >(2));

        CHECK_EQ(var_3.label(0), "0");

        CHECK_EQ(var_3.label(1), "1");

        CHECK(net->dag().existsArc(idMap["n1"], idMap["n3"]));

        const gum::Tensor< double >& proba_3 = net->cpt(idMap["n3"]);

        CHECK_EQ(proba_3.domainSize(), static_cast< gum::Size >(4));

        gum::Instantiation inst_3(proba_3);

        inst_3.chgVal(var_1, 0);

        inst_3.chgVal(var_3, 0);

        CHECK_EQ((proba_3[inst_3]), doctest::Approx(0.1f).epsilon(0.001f));

        inst_3.chgVal(var_3, 1);

        CHECK_EQ((proba_3[inst_3]), doctest::Approx(0.9f).epsilon(0.001f));

        inst_3.chgVal(var_1, 1);

        inst_3.chgVal(var_3, 0);

        CHECK_EQ((proba_3[inst_3]), doctest::Approx(0.9f).epsilon(0.001f));

        inst_3.chgVal(var_3, 1);

        CHECK_EQ((proba_3[inst_3]), doctest::Approx(0.1f).epsilon(0.001f));

        const gum::DiscreteVariable& var_4 = net->variable(idMap["n4"]);

        CHECK_EQ(var_4.name(), "n4");

        CHECK_EQ(var_4.domainSize(), static_cast< gum::Size >(2));

        CHECK_EQ(var_4.label(0), "0");

        CHECK_EQ(var_4.label(1), "1");

        CHECK(net->dag().existsArc(idMap["n1"], idMap["n4"]));

        CHECK(net->dag().existsArc(idMap["n2"], idMap["n4"]));

        const gum::Tensor< double >& proba_4 = net->cpt(idMap["n4"]);

        CHECK_EQ(proba_4.domainSize(), static_cast< gum::Size >(8));

        gum::Instantiation inst_4(proba_4);

        inst_4.chgVal(var_1, 0);

        inst_4.chgVal(var_2, 0);

        inst_4.chgVal(var_4, 0);

        CHECK_EQ((proba_4[inst_4]), doctest::Approx(0.4f).epsilon(0.001f));

        inst_4.chgVal(var_4, 1);

        CHECK_EQ((proba_4[inst_4]), doctest::Approx(0.6f).epsilon(0.001f));

        inst_4.chgVal(var_1, 1);

        inst_4.chgVal(var_2, 0);

        inst_4.chgVal(var_4, 0);

        CHECK_EQ((proba_4[inst_4]), doctest::Approx(0.5f).epsilon(0.001f));

        inst_4.chgVal(var_4, 1);

        CHECK_EQ((proba_4[inst_4]), doctest::Approx(0.5f).epsilon(0.001f));

        inst_4.chgVal(var_1, 0);

        inst_4.chgVal(var_2, 1);

        inst_4.chgVal(var_4, 0);

        CHECK_EQ((proba_4[inst_4]), doctest::Approx(0.5f).epsilon(0.001f));

        inst_4.chgVal(var_4, 1);

        CHECK_EQ((proba_4[inst_4]), doctest::Approx(0.5f).epsilon(0.001f));

        inst_4.chgVal(var_1, 1);

        inst_4.chgVal(var_2, 1);

        inst_4.chgVal(var_4, 0);

        CHECK_EQ(proba_4[inst_4], 1);

        inst_4.chgVal(var_4, 1);

        CHECK_EQ(proba_4[inst_4], 0);

        const gum::DiscreteVariable& var_5 = net->variable(idMap["n5"]);

        CHECK_EQ(var_5.name(), "n5");

        CHECK_EQ(var_5.domainSize(), static_cast< gum::Size >(3));

        CHECK_EQ(var_5.label(0), "space");

        CHECK_EQ(var_5.label(1), "final");

        CHECK_EQ(var_5.label(2), "frontiere");

        CHECK(net->dag().existsArc(idMap["n2"], idMap["n5"]));

        CHECK(net->dag().existsArc(idMap["n3"], idMap["n5"]));

        const gum::Tensor< double >& proba_5 = net->cpt(idMap["n5"]);

        CHECK_EQ(proba_5.domainSize(), static_cast< gum::Size >(12));

        gum::Instantiation inst_5(proba_5);

        inst_5.chgVal(var_3, 0);

        inst_5.chgVal(var_2, 0);

        inst_5.chgVal(var_5, 0);

        CHECK_EQ((proba_5[inst_5]), doctest::Approx(0.3f).epsilon(0.001f));

        inst_5.chgVal(var_5, 1);

        CHECK_EQ((proba_5[inst_5]), doctest::Approx(0.6f).epsilon(0.001f));

        inst_5.chgVal(var_5, 2);

        CHECK_EQ((proba_5[inst_5]), doctest::Approx(0.1f).epsilon(0.001f));

        inst_5.chgVal(var_2, 0);

        inst_5.chgVal(var_3, 1);

        inst_5.chgVal(var_5, 0);

        CHECK_EQ((proba_5[inst_5]), doctest::Approx(0.5f).epsilon(0.001f));

        inst_5.chgVal(var_5, 1);

        CHECK_EQ((proba_5[inst_5]), doctest::Approx(0.5f).epsilon(0.001f));

        inst_5.chgVal(var_5, 2);

        CHECK_EQ((proba_5[inst_5]), doctest::Approx(0.0f).epsilon(0.001f));

        inst_5.chgVal(var_2, 1);

        inst_5.chgVal(var_3, 0);

        inst_5.chgVal(var_5, 0);

        CHECK_EQ((proba_5[inst_5]), doctest::Approx(0.4f).epsilon(0.001f));

        inst_5.chgVal(var_5, 1);

        CHECK_EQ((proba_5[inst_5]), doctest::Approx(0.6f).epsilon(0.001f));

        inst_5.chgVal(var_5, 2);

        CHECK_EQ((proba_5[inst_5]), doctest::Approx(0.0f).epsilon(0.001f));

        inst_5.chgVal(var_2, 1);

        inst_5.chgVal(var_3, 1);

        inst_5.chgVal(var_5, 0);

        CHECK_EQ((proba_5[inst_5]), doctest::Approx(0.5f).epsilon(0.001f));

        inst_5.chgVal(var_5, 1);

        CHECK_EQ((proba_5[inst_5]), doctest::Approx(0.5f).epsilon(0.001f));

        inst_5.chgVal(var_5, 2);

        CHECK_EQ(proba_5[inst_5], 0);

        const gum::DiscreteVariable& var_6 = net->variable(idMap["n6"]);

        CHECK_EQ(var_6.name(), "n6");

        CHECK_EQ(var_6.domainSize(), static_cast< gum::Size >(2));

        CHECK_EQ(var_6.label(0), "0");

        CHECK_EQ(var_6.label(1), "1");

        CHECK(net->dag().existsArc(idMap["n1"], idMap["n6"]));

        CHECK(net->dag().existsArc(idMap["n5"], idMap["n6"]));

        const gum::Tensor< double >& proba_6 = net->cpt(idMap["n6"]);

        CHECK_EQ(proba_6.domainSize(), static_cast< gum::Size >(12));

        gum::Instantiation inst_6(proba_6);

        inst_6.chgVal(var_6, 0);

        inst_6.chgVal(var_1, 0);

        inst_6.chgVal(var_5, 0);

        CHECK_EQ((proba_6[inst_6]), doctest::Approx(0.1f).epsilon(0.001f));

        inst_6.chgVal(var_5, 1);

        CHECK_EQ((proba_6[inst_6]), doctest::Approx(0.2f).epsilon(0.001f));

        inst_6.chgVal(var_5, 2);

        CHECK_EQ((proba_6[inst_6]), doctest::Approx(0.3f).epsilon(0.001f));

        inst_6.chgVal(var_1, 1);

        inst_6.chgVal(var_5, 0);

        CHECK_EQ((proba_6[inst_6]), doctest::Approx(0.4f).epsilon(0.001f));

        inst_6.chgVal(var_5, 1);

        CHECK_EQ((proba_6[inst_6]), doctest::Approx(0.5f).epsilon(0.001f));

        inst_6.chgVal(var_5, 2);

        CHECK_EQ((proba_6[inst_6]), doctest::Approx(0.6f).epsilon(0.001f));

        inst_6.chgVal(var_6, 1);

        inst_6.chgVal(var_1, 0);

        inst_6.chgVal(var_5, 0);

        CHECK_EQ((proba_6[inst_6]), doctest::Approx(0.7f).epsilon(0.001f));

        inst_6.chgVal(var_5, 1);

        CHECK_EQ((proba_6[inst_6]), doctest::Approx(0.8f).epsilon(0.001f));

        inst_6.chgVal(var_5, 2);

        CHECK_EQ((proba_6[inst_6]), doctest::Approx(0.9f).epsilon(0.001f));

        inst_6.chgVal(var_1, 1);

        inst_6.chgVal(var_5, 0);

        CHECK_EQ(proba_6[inst_6], 1);

        inst_6.chgVal(var_5, 1);

        CHECK_EQ(proba_6[inst_6], 0);

        inst_6.chgVal(var_5, 2);

        CHECK_EQ(proba_6[inst_6], 0);

        delete net;
      }
    }

    static void testAlarm() {
      std::string                   file = GET_RESSOURCES_PATH("bifxml/alarm.bifxml");
      auto                          net  = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, file);

      gum::Size isOK = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(isOK = reader.proceed());
      CHECK_EQ(isOK, static_cast< gum::Size >(0));

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
        historyInst.chgVal(history, 0);
        historyInst.chgVal(net->variable(idMap["LVFAILURE"]), 0);
        CHECK_EQ((historyCPT[historyInst]), doctest::Approx(0.9f).epsilon(0.0001f));
        historyInst.chgVal(history, 1);
        historyInst.chgVal(net->variable(idMap["LVFAILURE"]), 0);
        CHECK_EQ((historyCPT[historyInst]), doctest::Approx(0.1f).epsilon(0.0001f));
        historyInst.chgVal(history, 0);
        historyInst.chgVal(net->variable(idMap["LVFAILURE"]), 1);
        CHECK_EQ((historyCPT[historyInst]), doctest::Approx(0.01f).epsilon(0.0001f));
        historyInst.chgVal(history, 1);
        historyInst.chgVal(net->variable(idMap["LVFAILURE"]), 1);
        CHECK_EQ((historyCPT[historyInst]), doctest::Approx(0.99f).epsilon(0.0001f));
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
        CHECK_EQ((errlowoutputCPT[errlowoutputInst]), doctest::Approx(0.05f).epsilon(0.001f));
        errlowoutputInst.chgVal(errlowoutput, 1);
        CHECK_EQ((errlowoutputCPT[errlowoutputInst]), doctest::Approx(0.95f).epsilon(0.001f));
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

        gum::Instantiation var_inst;
        var_inst << lvedvolume;

        inst.chgVal(hypovolemia, 0);
        inst.chgVal(lvfailure, 0);
        float  array_1[] = {0.95f, 0.04f, 0.01f};
        size_t i         = 0;

        for (inst.setFirstIn(var_inst); !inst.end(); inst.incIn(var_inst), ++i) {
          CHECK_EQ((cpt[inst]), doctest::Approx(array_1[i]).epsilon(0.001f));
        }

        inst.chgVal(hypovolemia, 1);
        inst.chgVal(lvfailure, 0);
        float array_2[] = {0.98f, 0.01f, 0.01f};
        i               = 0;

        for (inst.setFirstIn(var_inst); !inst.end(); inst.incIn(var_inst), ++i) {
          CHECK_EQ((cpt[inst]), doctest::Approx(array_2[i]).epsilon(0.001f));
        }

        inst.chgVal(hypovolemia, 0);
        inst.chgVal(lvfailure, 1);
        float array_3[] = {0.01f, 0.09f, 0.9f};
        i               = 0;

        for (inst.setFirstIn(var_inst); !inst.end(); inst.incIn(var_inst), ++i) {
          CHECK_EQ((cpt[inst]), doctest::Approx(array_3[i]).epsilon(0.001f));
        }

        inst.chgVal(hypovolemia, 1);
        inst.chgVal(lvfailure, 1);
        float array_4[] = {0.05f, 0.9f, 0.05f};
        i               = 0;

        for (inst.setFirstIn(var_inst); !inst.end(); inst.incIn(var_inst), ++i) {
          CHECK_EQ((cpt[inst]), doctest::Approx(array_4[i]).epsilon(0.001f));
        }
      }

      delete net;
    }

    static void testRead_file2_from_ifstream() {
      std::string              file = GET_RESSOURCES_PATH("bifxml/BNBIFXMLReader_file2.bifxml");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      std::ifstream            ifs(file);
      gum::BIFXMLBNReader< double > reader(net, ifs);

      gum::Size isOK = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(isOK = reader.proceed());
      CHECK_EQ((isOK), (static_cast< gum::Size >(0)));
      CHECK((net) != (nullptr));

      if (net != nullptr) {
        CHECK_EQ((net->size()), (static_cast< gum::Size >(2)));

        gum::NodeId node_1 = 0, node_2 = 0;
        for (const auto node: net->nodes())
          if (net->variable(node).name() == "n1") node_1 = node;
          else node_2 = node;

        gum::Instantiation inst_1(net->cpt(node_1));
        inst_1.setFirst();
        CHECK_EQ((net->cpt(node_1)[inst_1]), doctest::Approx(0.2f).epsilon(0.001f));
        inst_1.setLast();
        CHECK_EQ((net->cpt(node_1)[inst_1]), doctest::Approx(0.8f).epsilon(0.001f));

        gum::Instantiation inst_2(net->cpt(node_2));
        inst_2.setFirst();
        CHECK_EQ((net->cpt(node_2)[inst_2]), doctest::Approx(0.3f).epsilon(0.001f));
        inst_2.setLast();
        CHECK_EQ((net->cpt(node_2)[inst_2]), doctest::Approx(0.7f).epsilon(0.001f));

        delete net;
      }
    }

    static void testConstructorFromStream() {
      std::string             file = GET_RESSOURCES_PATH("bifxml/BNBIFXMLReader_file1.bifxml");
      gum::BayesNet< double > net;

      std::ifstream                  ifs(file);
      gum::BIFXMLBNReader< double >* reader = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(reader = new gum::BIFXMLBNReader< double >(&net, ifs));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete reader);
    }

    static void testRead_file2() {
      std::string              file = GET_RESSOURCES_PATH("bifxml/BNBIFXMLReader_file2.bifxml");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();
      std::ifstream            ifs(file);
      gum::BIFXMLBNReader< double > reader(net, ifs);

      gum::Size isOK = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(isOK = reader.proceed());
      CHECK((isOK) == (static_cast< gum::Size >(0)));
      CHECK((net) != (nullptr));

      if (net != nullptr) {
        CHECK((net->size()) == (static_cast< gum::Size >(2)));

        gum::NodeId node_1 = 0, node_2 = 0;
        for (const auto node: net->nodes())
          if (net->variable(node).name() == "n1") node_1 = node;
          else node_2 = node;

        gum::Instantiation inst_1(net->cpt(node_1));
        inst_1.setFirst();
        CHECK((net->cpt(node_1)[inst_1]) == doctest::Approx(0.2f).epsilon(0.001f));
        inst_1.setLast();
        CHECK((net->cpt(node_1)[inst_1]) == doctest::Approx(0.8f).epsilon(0.001f));

        gum::Instantiation inst_2(net->cpt(node_2));
        inst_2.setFirst();
        CHECK((net->cpt(node_2)[inst_2]) == doctest::Approx(0.3f).epsilon(0.001f));
        inst_2.setLast();
        CHECK((net->cpt(node_2)[inst_2]) == doctest::Approx(0.7f).epsilon(0.001f));

        delete net;
      }
    }

    static void testRead_file2_from_istringstream() {
      std::string        file = GET_RESSOURCES_PATH("bifxml/BNBIFXMLReader_file2.bifxml");
      std::ifstream      ifs(file);
      std::ostringstream oss;
      oss << ifs.rdbuf();
      std::istringstream iss(oss.str());

      gum::BayesNet< double >*      net = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, iss);

      gum::Size isOK = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(isOK = reader.proceed());
      CHECK((isOK) == (static_cast< gum::Size >(0)));
      CHECK((net) != (nullptr));

      if (net != nullptr) {
        CHECK((net->size()) == (static_cast< gum::Size >(2)));

        gum::NodeId node_1 = 0, node_2 = 0;
        for (const auto node: net->nodes())
          if (net->variable(node).name() == "n1") node_1 = node;
          else node_2 = node;

        gum::Instantiation inst_1(net->cpt(node_1));
        inst_1.setFirst();
        CHECK((net->cpt(node_1)[inst_1]) == doctest::Approx(0.2f).epsilon(0.001f));
        inst_1.setLast();
        CHECK((net->cpt(node_1)[inst_1]) == doctest::Approx(0.8f).epsilon(0.001f));

        gum::Instantiation inst_2(net->cpt(node_2));
        inst_2.setFirst();
        CHECK((net->cpt(node_2)[inst_2]) == doctest::Approx(0.3f).epsilon(0.001f));
        inst_2.setLast();
        CHECK((net->cpt(node_2)[inst_2]) == doctest::Approx(0.7f).epsilon(0.001f));

        delete net;
      }
    }

    static void testRead_from_inline_string() {
      // Minimal BIFXML: two independent binary variables A (0.4/0.6) and B (0.1/0.9)
      const std::string xml = R"(<?xml version="1.0" ?>
<BIF VERSION="0.3">
<NETWORK>
<VARIABLE TYPE="nature">
  <NAME>A</NAME>
  <OUTCOME>false</OUTCOME>
  <OUTCOME>true</OUTCOME>
</VARIABLE>
<VARIABLE TYPE="nature">
  <NAME>B</NAME>
  <OUTCOME>false</OUTCOME>
  <OUTCOME>true</OUTCOME>
</VARIABLE>
<DEFINITION>
  <FOR>A</FOR>
  <TABLE>0.4 0.6</TABLE>
</DEFINITION>
<DEFINITION>
  <FOR>B</FOR>
  <TABLE>0.1 0.9</TABLE>
</DEFINITION>
</NETWORK>
</BIF>)";

      gum::BayesNet< double >*      net = new gum::BayesNet< double >();
      std::istringstream            iss(xml);
      gum::BIFXMLBNReader< double > reader(net, iss);

      gum::Size isOK = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(isOK = reader.proceed());
      CHECK((isOK) == (static_cast< gum::Size >(0)));
      CHECK((net->size()) == (static_cast< gum::Size >(2)));

      gum::NodeId idA = net->idFromName("A");
      gum::NodeId idB = net->idFromName("B");

      gum::Instantiation instA(net->cpt(idA));
      instA.setFirst();
      CHECK((net->cpt(idA)[instA]) == doctest::Approx(0.4f).epsilon(0.001f));
      instA.setLast();
      CHECK((net->cpt(idA)[instA]) == doctest::Approx(0.6f).epsilon(0.001f));

      gum::Instantiation instB(net->cpt(idB));
      instB.setFirst();
      CHECK((net->cpt(idB)[instB]) == doctest::Approx(0.1f).epsilon(0.001f));
      instB.setLast();
      CHECK((net->cpt(idB)[instB]) == doctest::Approx(0.9f).epsilon(0.001f));

      delete net;
    }

    static void testRead_network_name_from_file() {
      std::string                   file = GET_RESSOURCES_PATH("bifxml/BN_with_name.bifxml");
      auto                          net  = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, file);
      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());
      CHECK((net->propertyWithDefault("name", "")) == ("fastPrototype"));
      delete net;
    }

    static void testRead_network_name_from_string() {
      const std::string xml = R"(<?xml version="1.0" ?>
<BIF VERSION="0.3">
<NETWORK>
<NAME>myNetwork</NAME>
<VARIABLE TYPE="nature">
  <NAME>X</NAME>
  <OUTCOME>0</OUTCOME>
  <OUTCOME>1</OUTCOME>
</VARIABLE>
<DEFINITION>
  <FOR>X</FOR>
  <TABLE>0.5 0.5</TABLE>
</DEFINITION>
</NETWORK>
</BIF>)";

      auto                          net = new gum::BayesNet< double >();
      std::istringstream            iss(xml);
      gum::BIFXMLBNReader< double > reader(net, iss);
      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());
      CHECK((net->propertyWithDefault("name", "")) == ("myNetwork"));
      delete net;
    }

    static void testRead_from_stream_invalid_xml() {
      std::istringstream            iss("<not valid bifxml>");
      auto                          net = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, iss);
      CHECK_THROWS_AS(reader.proceed(), const gum::IOError&);
      delete net;
    }

    static void testUnexisting() {
      std::string              file = "Schmurtz";
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();

      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::BIFXMLBNReader< double > reader(net, file));
      gum::BIFXMLBNReader< double > reader(net, file);
      CHECK_THROWS_AS(reader.proceed(), const gum::IOError&);

      if (net) delete net;
    }

    void Carpo() {
      std::string                   file = GET_RESSOURCES_PATH("bifxml/carpo.bifxml");
      auto                          net  = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, file);

      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      if (net) delete net;
    }

    static void testDiabetes() {
      std::string                   file = GET_RESSOURCES_PATH("bifxml/Diabetes.bifxml");
      auto                          net  = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, file);

      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      if (net) delete net;
    }

    static void testHailfinder() {
      std::string                   file = GET_RESSOURCES_PATH("bifxml/hailfinder.bifxml");
      auto                          net  = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, file);

      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      if (net) delete net;
    }

    static void testInsurance() {
      std::string                   file = GET_RESSOURCES_PATH("bifxml/insurance.bifxml");
      auto                          net  = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, file);

      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      if (net) delete net;
    }

    static void testLink() {
      std::string                   file = GET_RESSOURCES_PATH("bifxml/Link.bifxml");
      auto                          net  = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, file);

      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      if (net) delete net;
    }

    static void testMildew() {
      std::string                   file = GET_RESSOURCES_PATH("bifxml/Mildew.bifxml");
      auto                          net  = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, file);

      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      if (net) delete net;
    }

    static void testMunin1() {
      std::string                   file = GET_RESSOURCES_PATH("bifxml/Munin1.bifxml");
      auto                          net  = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, file);

      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      if (net) delete net;
    }

    static void testPigs() {
      std::string                   file = GET_RESSOURCES_PATH("bifxml/Pigs.bifxml");
      auto                          net  = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, file);

      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      if (net) delete net;
    }

    static void testWater() {
      std::string                   file = GET_RESSOURCES_PATH("bifxml/Water.bifxml");
      auto                          net  = new gum::BayesNet< double >();
      gum::BIFXMLBNReader< double > reader(net, file);

      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      if (net) delete net;
    }

    static void testOmi() {
      auto savedBN = gum::BayesNet< double >::fastPrototype("X->Y<-Z", "{Yes|No}");
      savedBN.setProperty("name", "SavedBN");

      std::string filename{GET_RESSOURCES_PATH("outputs/SavedBN.bifxml")};

      gum::BIFXMLBNWriter< double > writer;
      writer.write(filename, savedBN);

      gum::BayesNet< double > loadedBN("LoadedBN");
      auto                    reader = gum::BIFXMLBNReader< double >(&loadedBN, filename);
      reader.proceed();

      std::string savedBNName{savedBN.property("name")};
      std::string loadedBNName{loadedBN.property("name")};
      CHECK_EQ(savedBNName, loadedBNName);
    }
  };

  GUM_TEST_ACTIF(Constuctor)
  GUM_TEST_ACTIF(Read_file1)
  GUM_TEST_ACTIF(Read_file2_float)
  GUM_TEST_ACTIF(Read_dog_double)
  GUM_TEST_ACTIF(Read_file2_double)
  GUM_TEST_ACTIF(Read_file3)
  GUM_TEST_ACTIF(Alarm)
  GUM_TEST_ACTIF(ConstructorFromStream)
  GUM_TEST_ACTIF(Read_file2_from_ifstream)
  GUM_TEST_ACTIF(Read_file2_from_istringstream)
  GUM_TEST_ACTIF(Read_network_name_from_file)
  GUM_TEST_ACTIF(Read_network_name_from_string)
  GUM_TEST_ACTIF(Read_from_inline_string)
  GUM_TEST_ACTIF(Read_from_stream_invalid_xml)
  GUM_TEST_ACTIF(Unexisting)
  GUM_TEST_ACTIF(Diabetes)
  GUM_TEST_ACTIF(Hailfinder)
  GUM_TEST_ACTIF(Insurance)
  GUM_TEST_ACTIF(Link)
  GUM_TEST_ACTIF(Mildew)
  GUM_TEST_ACTIF(Munin1)
  GUM_TEST_ACTIF(Pigs)
  GUM_TEST_ACTIF(Water)
  GUM_TEST_ACTIF(Omi)
}   // namespace gum_tests
