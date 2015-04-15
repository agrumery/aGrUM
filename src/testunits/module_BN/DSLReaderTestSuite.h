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
#include <iostream>
#include <string>
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/DSL/DSLReader.h>
#include <agrum/BN/io/DSL/DSLWriter.h>
// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5
namespace gum_tests {
  class DSLReaderTestSuite : public CxxTest::TestSuite {
    public:
    void testConstuctor() {
      std::string file = GET_PATH_STR("DSL/DSLReader_file1.txt");
      gum::BayesNet<float> net;

      gum::DSLReader<float> *reader = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(reader = new gum::DSLReader<float>(&net, file));
      TS_GUM_ASSERT_THROWS_NOTHING(delete reader);
    }
    void testRead_file1() {
      std::string file = GET_PATH_STR("DSL/DSLReader_file1.txt");
      gum::BayesNet<float> *net = new gum::BayesNet<float>();
      gum::DSLReader<float> reader(net, file);

      reader.trace(false);

      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
      TS_ASSERT(net != 0);

      if (net != 0) {
        TS_ASSERT(net->empty());
        delete net;
      }
    }
    void testRead_file2_float() {
      std::string file = GET_PATH_STR("DSL/DSLReader_file2.txt");
      gum::BayesNet<float> *net = new gum::BayesNet<float>();
      gum::DSLReader<float> reader(net, file);
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);

      TS_ASSERT(net != 0);

      if (net != 0) {
        TS_ASSERT_EQUALS(net->size(), (gum::Size)2);
        gum::NodeId node_1 = 0, node_2 = 0;

        for (const auto node : net->nodes())
          if (net->variable(node).name() == "n1")
            node_1 = node;
          else
            node_2 = node;

        const gum::DiscreteVariable &var_1 = net->variable(node_1);

        TS_ASSERT_EQUALS(var_1.name(), "n1");
        TS_ASSERT_EQUALS(var_1.domainSize(), (gum::Size)2);
        const gum::Potential<float> &proba_1 = net->cpt(node_1);
        TS_ASSERT_EQUALS(proba_1.domainSize(), (gum::Size)2);
        gum::Instantiation inst_1(proba_1);
        inst_1.setFirst();
        TS_ASSERT(abs((proba_1[inst_1] - 0.2)) < 0.001);
        inst_1.setLast();
        TS_ASSERT(abs((proba_1[inst_1] - 0.8)) < 0.001);

        const gum::DiscreteVariable &var_2 = net->variable(node_2);
        TS_ASSERT_EQUALS(var_2.name(), "n2");
        TS_ASSERT_EQUALS(var_2.domainSize(), (gum::Size)2);
        const gum::Potential<float> &proba_2 = net->cpt(node_2);
        TS_ASSERT_EQUALS(proba_2.domainSize(), (gum::Size)2);
        gum::Instantiation inst_2(proba_2);
        inst_2.setFirst();
        TS_ASSERT(abs((proba_2[inst_2] - 0.3)) < 0.001);
        inst_2.setLast();
        TS_ASSERT(abs((proba_2[inst_2] - 0.7)) < 0.001);

        delete net;
      }
    }
    void testRead_file2_double() {
      std::string file = GET_PATH_STR("DSL/DSLReader_file2.txt");
      gum::BayesNet<double> *net = new gum::BayesNet<double>();
      gum::DSLReader<double> reader(net, file);

      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);

      TS_ASSERT(net != 0);

      if (net != 0) {
        TS_ASSERT_EQUALS(net->size(), (gum::Size)2);
        gum::NodeId node_1 = 0, node_2 = 0;

        for (const auto node : net->nodes())
          if (net->variable(node).name() == "n1")
            node_1 = node;
          else
            node_2 = node;

        const gum::DiscreteVariable &var_1 = net->variable(node_1);

        TS_ASSERT_EQUALS(var_1.name(), "n1");
        TS_ASSERT_EQUALS(var_1.domainSize(), (gum::Size)2);
        const gum::Potential<double> &proba_1 = net->cpt(node_1);
        TS_ASSERT_EQUALS(proba_1.domainSize(), (gum::Size)2);
        gum::Instantiation inst_1(proba_1);
        inst_1.setFirst();
        TS_ASSERT(abs((proba_1[inst_1] - 0.2)) < 0.001);
        inst_1.setLast();
        TS_ASSERT(abs((proba_1[inst_1] - 0.8)) < 0.001);

        const gum::DiscreteVariable &var_2 = net->variable(node_2);
        TS_ASSERT_EQUALS(var_2.name(), "n2");
        TS_ASSERT_EQUALS(var_2.domainSize(), (gum::Size)2);
        const gum::Potential<double> &proba_2 = net->cpt(node_2);
        TS_ASSERT_EQUALS(proba_2.domainSize(), (gum::Size)2);
        gum::Instantiation inst_2(proba_2);
        inst_2.setFirst();
        TS_ASSERT(abs((proba_2[inst_2] - 0.3)) < 0.001);
        inst_2.setLast();
        TS_ASSERT(abs((proba_2[inst_2] - 0.7)) < 0.001);

        delete net;
      }
    }
    void testRead_file3() {
      std::string file = GET_PATH_STR("DSL/DSLReader_file3.txt");
      gum::BayesNet<float> *net = new gum::BayesNet<float>();
      gum::DSLReader<float> reader(net, file);
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);
      TS_ASSERT_EQUALS(net->size(), (gum::Size)6);
      TS_ASSERT(net != 0);
      gum::DSLWriter<float> writer;
      std::string file2 = GET_PATH_STR("DSL/DSLWriter_TestFile3.txt");
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file2, *net));

      if (net)
        delete net;
    }
    void testRead_file_completeDSL() {
      std::string file = GET_PATH_STR("DSL/Ling.dsl");
      gum::BayesNet<float> *net = new gum::BayesNet<float>();
      gum::DSLReader<float> reader(net, file);
      int nbrErr = 0;
      reader.trace(true);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);
      TS_ASSERT_EQUALS(net->size(), (gum::Size)13);
      TS_ASSERT(net != 0);
      gum::DSLWriter<float> writer;
      std::string file2 = GET_PATH_STR("DSL/DSLWriter_Ling.txt");
      TS_GUM_ASSERT_THROWS_NOTHING(writer.write(file2, *net));

      if (net)
        delete net;
    }
    void testAlarm() {
      std::string file = GET_PATH_STR("DSL/alarm.dsl");
      gum::BayesNet<float> *net = new gum::BayesNet<float>();
      gum::DSLReader<float> reader(net, file);

      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);

      gum::HashTable<std::string, gum::Id> idMap;

      for (const auto node : net->nodes())
        idMap.insert(net->variable(node).name(), node);

      // The node wich we'll test
      TS_ASSERT(idMap.exists("HISTORY"));
      // It's parent
      TS_ASSERT(idMap.exists("LVFAILURE"));

      if (idMap.exists("HISTORY") and idMap.exists("LVFAILURE")) {
        const gum::DiscreteVariable &history = net->variable(idMap["HISTORY"]);
        TS_ASSERT_EQUALS(history.domainSize(), (gum::Size)2);
        TS_ASSERT_EQUALS(history.label(0), "TRUE");
        TS_ASSERT_EQUALS(history.label(1), "FALSE");
        TS_ASSERT(net->dag().existsArc(idMap["LVFAILURE"], idMap["HISTORY"]));
        const gum::Potential<float> &historyCPT = net->cpt(idMap["HISTORY"]);
        TS_ASSERT_EQUALS(historyCPT.domainSize(), (gum::Size)4);
        TS_ASSERT(historyCPT.contains(net->variable(idMap["HISTORY"])));
        TS_ASSERT(historyCPT.contains(net->variable(idMap["LVFAILURE"])));
        gum::Instantiation historyInst(historyCPT);
        // (TRUE | TRUE)
        historyInst.chgVal(history, 0);
        historyInst.chgVal(net->variable(idMap["LVFAILURE"]), 0);
        TS_ASSERT(abs(historyCPT[historyInst] - 0.9) < 0.0001);
        // (FALSE | TRUE)
        historyInst.chgVal(history, 1);
        historyInst.chgVal(net->variable(idMap["LVFAILURE"]), 0);
        TS_ASSERT(abs(historyCPT[historyInst] - 0.1) < 0.0001);
        // (TRUE | FALSE)
        historyInst.chgVal(history, 0);
        historyInst.chgVal(net->variable(idMap["LVFAILURE"]), 1);
        TS_ASSERT(abs(historyCPT[historyInst] - 0.01) < 0.0001);
        // (FALSE | FALSE)
        historyInst.chgVal(history, 1);
        historyInst.chgVal(net->variable(idMap["LVFAILURE"]), 1);
        TS_ASSERT(abs(historyCPT[historyInst] - 0.99) < 0.0001);
      }

      // The node wich we'll test
      TS_ASSERT(idMap.exists("ERRLOWOUTPUT"));
      // It's Children
      TS_ASSERT(idMap.exists("HRBP"));

      if (idMap.exists("ERRLOWOUTPUT") and idMap.exists("HRBP")) {
        const gum::DiscreteVariable &errlowoutput =
            net->variable(idMap["ERRLOWOUTPUT"]);
        TS_ASSERT_EQUALS(errlowoutput.domainSize(), (gum::Size)2);
        TS_ASSERT_EQUALS(errlowoutput.label(0), "TRUE");
        TS_ASSERT_EQUALS(errlowoutput.label(1), "FALSE");
        TS_ASSERT(net->dag().existsArc(idMap["ERRLOWOUTPUT"], idMap["HRBP"]));
        const gum::Potential<float> &errlowoutputCPT =
            net->cpt(idMap["ERRLOWOUTPUT"]);
        TS_ASSERT_EQUALS(errlowoutputCPT.domainSize(), (gum::Size)2);
        TS_ASSERT(errlowoutputCPT.contains(errlowoutput));
        gum::Instantiation errlowoutputInst(errlowoutputCPT);
        errlowoutputInst.chgVal(errlowoutput, 0);
        TS_ASSERT(abs(errlowoutputCPT[errlowoutputInst] - 0.05) < 0.001);
        errlowoutputInst.chgVal(errlowoutput, 1);
        TS_ASSERT(abs(errlowoutputCPT[errlowoutputInst] - 0.95) < 0.001);
      }

      // The nide wich we'll test
      TS_ASSERT(idMap.exists("LVEDVOLUME"));
      // It's parents
      TS_ASSERT(idMap.exists("HYPOVOLEMIA"));
      TS_ASSERT(idMap.exists("LVFAILURE"));

      if (idMap.exists("LVEDVOLUME") and idMap.exists("HYPOVOLEMIA") and
          idMap.exists("LVFAILURE")) {
        const gum::DiscreteVariable &lvedvolume = net->variable(idMap["LVEDVOLUME"]);
        const gum::DiscreteVariable &hypovolemia =
            net->variable(idMap["HYPOVOLEMIA"]);
        const gum::DiscreteVariable &lvfailure = net->variable(idMap["LVFAILURE"]);
        // checking label order
        TS_ASSERT_EQUALS(lvedvolume.label(0), "LOW");
        TS_ASSERT_EQUALS(lvedvolume.label(1), "NORMAL");
        TS_ASSERT_EQUALS(lvedvolume.label(2), "HIGH");
        TS_ASSERT_EQUALS(hypovolemia.label(0), "TRUE");
        TS_ASSERT_EQUALS(hypovolemia.label(1), "FALSE");
        TS_ASSERT_EQUALS(lvfailure.label(0), "TRUE");
        TS_ASSERT_EQUALS(lvfailure.label(1), "FALSE");
        const gum::Potential<float> &cpt = net->cpt(idMap["LVEDVOLUME"]);
        gum::Instantiation inst(cpt);
        gum::Instantiation var_inst;
        var_inst << lvedvolume;
        inst.chgVal(hypovolemia, 0);
        inst.chgVal(lvfailure, 0);
        float array_1[] = {0.95, 0.04, 0.01};
        size_t i = 0;

        for (inst.setFirstIn(var_inst); !inst.end(); inst.incIn(var_inst), ++i) {
          TS_ASSERT_DELTA(cpt[inst], array_1[i], 0.001);
        }

        inst.chgVal(hypovolemia, 1);
        inst.chgVal(lvfailure, 0);
        float array_2[] = {0.98, 0.01, 0.01};
        i = 0;

        for (inst.setFirstIn(var_inst); !inst.end(); inst.incIn(var_inst), ++i) {
          TS_ASSERT_DELTA(cpt[inst], array_2[i], 0.001);
        }

        inst.chgVal(hypovolemia, 0);
        inst.chgVal(lvfailure, 1);
        float array_3[] = {0.01, 0.09, 0.9};
        i = 0;

        for (inst.setFirstIn(var_inst); !inst.end(); inst.incIn(var_inst), ++i) {
          TS_ASSERT_DELTA(cpt[inst], array_3[i], 0.001);
        }

        inst.chgVal(hypovolemia, 1);
        inst.chgVal(lvfailure, 1);
        float array_4[] = {0.05, 0.9, 0.05};
        i = 0;

        for (inst.setFirstIn(var_inst); !inst.end(); inst.incIn(var_inst), ++i) {
          TS_ASSERT_DELTA(cpt[inst], array_4[i], 0.001);
        }
      }

      delete net;
    }
    void testUnexisting() {
      std::string file = "Schmurtz";
      gum::BayesNet<float> *net = new gum::BayesNet<float>();
      TS_GUM_ASSERT_THROWS_NOTHING(gum::DSLReader<float> reader(net, file));
      gum::DSLReader<float> reader(net, file);
      TS_ASSERT_THROWS(reader.proceed(), gum::IOError);

      if (net)
        delete net;
    }
    void testBarley() {
      std::string file = GET_PATH_STR("DSL/Barley.dsl");
      gum::BayesNet<float> *net = new gum::BayesNet<float>();
      gum::DSLReader<float> reader(net, file);
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);

      if (net)
        delete net;
    }
    void testCarpo() {
      std::string file = GET_PATH_STR("DSL/carpo.dsl");
      gum::BayesNet<float> *net = new gum::BayesNet<float>();
      gum::DSLReader<float> reader(net, file);
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);

      if (net)
        delete net;
    }
    void testDiabetes() {
      std::string file = GET_PATH_STR("DSL/Diabetes.dsl");
      gum::BayesNet<float> *net = new gum::BayesNet<float>();
      gum::DSLReader<float> reader(net, file);
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);

      if (net)
        delete net;
    }
    void testHailfinder() {
      std::string file = GET_PATH_STR("DSL/hailfinder.dsl");
      gum::BayesNet<float> *net = new gum::BayesNet<float>();
      gum::DSLReader<float> reader(net, file);
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);

      if (net)
        delete net;
    }
    void testInsurance() {
      std::string file = GET_PATH_STR("DSL/insurance.dsl");
      gum::BayesNet<float> *net = new gum::BayesNet<float>();
      gum::DSLReader<float> reader(net, file);
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);

      if (net)
        delete net;
    }
    void testLink() {
      std::string file = GET_PATH_STR("DSL/Link.dsl");
      gum::BayesNet<float> *net = new gum::BayesNet<float>();
      gum::DSLReader<float> reader(net, file);
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);

      if (net)
        delete net;
    }
    // Mildew is not readable (because of the VERY LARGE FLOAT_LIST in foto_4 (at
    // least))...
    void Mildew() {
      std::string file = GET_PATH_STR("DSL/Mildew.dsl");
      gum::BayesNet<float> *net = new gum::BayesNet<float>();
      gum::DSLReader<float> reader(net, file);
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);

      if (net)
        delete net;
    }
    void testMunin1() {
      std::string file = GET_PATH_STR("DSL/Munin1.dsl");
      gum::BayesNet<float> *net = new gum::BayesNet<float>();
      gum::DSLReader<float> reader(net, file);
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);

      if (net)
        delete net;
    }
    void testPigs() {
      std::string file = GET_PATH_STR("DSL/Pigs.dsl");
      gum::BayesNet<float> *net = new gum::BayesNet<float>();
      gum::DSLReader<float> reader(net, file);
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);

      if (net)
        delete net;
    }
    void testWater() {
      std::string file = GET_PATH_STR("DSL/Water.dsl");
      gum::BayesNet<float> *net = new gum::BayesNet<float>();
      gum::DSLReader<float> reader(net, file);
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);

      if (net)
        delete net;
    }

    private:
    float abs(float d) {
      if (d < 0)
        return (d * (float)-1);

      return d;
    }
  };
}
