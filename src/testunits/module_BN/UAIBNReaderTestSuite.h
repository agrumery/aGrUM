
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/UAI/UAIBNReader.h>
#include <agrum/tools/variables/labelizedVariable.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class UAIBNReaderTestSuite: public CxxTest::TestSuite {
    public:
    void testConstuctor() {
      std::string file = GET_RESSOURCES_PATH("uai/BNUAIReader_file1.uai");
      gum::BayesNet< double > net;

      gum::UAIBNReader< double >* reader = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(reader =
                                      new gum::UAIBNReader< double >(&net, file));
      TS_GUM_ASSERT_THROWS_NOTHING(delete reader);
    }

    void testRead_file1() {
      std::string file = GET_RESSOURCES_PATH("uai/BNUAIReader_file1.uai");
      gum::BayesNet< double >* net = new gum::BayesNet< double >();

      TS_ASSERT(net != nullptr);

      gum::UAIBNReader< double > reader(net, file);
      reader.proceed();

      if (net != nullptr) {
        TS_ASSERT(net->empty());
        delete net;
      }
    }

    void testRead_file2_float() {
      std::string file = GET_RESSOURCES_PATH("uai/BNUAIReader_file2.uai");
      gum::BayesNet< double >* net = new gum::BayesNet< double >();
      gum::UAIBNReader< double > reader(net, file);

      gum::Size nbErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbErr = reader.proceed());
      TS_ASSERT_EQUALS(nbErr, gum::Size(0));

      TS_ASSERT(net != nullptr);

      if (net != nullptr) {
        TS_ASSERT(!net->empty());
        TS_ASSERT_EQUALS(net->size(), (gum::Size)5);

        gum::NodeId node_1 = 0, node_2 = 0;
        node_1 = net->idFromName("0");
        node_2 = net->idFromName("1");

        const gum::DiscreteVariable& var_1 = net->variable(node_1);

        TS_ASSERT_EQUALS(var_1.name(), "0");
        TS_ASSERT_EQUALS(var_1.domainSize(), (gum::Size)2);

        const gum::Potential< double >& proba_1 = net->cpt(node_1);
        TS_ASSERT_EQUALS(proba_1.domainSize(), (gum::Size)2);

        gum::Instantiation inst_1(proba_1);
        inst_1.setFirst();
        TS_ASSERT_DELTA(proba_1[inst_1], 0.2, 0.001);
        inst_1.setLast();
        TS_ASSERT_DELTA(proba_1[inst_1], 0.8, 0.001);

        const gum::DiscreteVariable& var_2 = net->variable(node_2);
        TS_ASSERT_EQUALS(var_2.name(), "1");
        TS_ASSERT_EQUALS(var_2.domainSize(), (gum::Size)2);

        const gum::Potential< double >& proba_2 = net->cpt(node_2);
        TS_ASSERT_EQUALS(proba_2.domainSize(), (gum::Size)2);

        gum::Instantiation inst_2(proba_2);
        inst_2.setFirst();
        TS_ASSERT_DELTA(proba_2[inst_2], 0.3, 0.001);
        inst_2.setLast();
        TS_ASSERT_DELTA(proba_2[inst_2], 0.7, 0.001);
        delete net;
      }
    }

    void testRead_dog_double() {
      // from Charniak, Bayesian Networks Without Tears, AI Magazine, 1991
      std::string              file = GET_RESSOURCES_PATH("uai/dog.uai");
      gum::BayesNet< double >* net = new gum::BayesNet< double >();
      gum::UAIBNReader< double > reader(net, file);

      gum::Size nbErr = false;
      TS_GUM_ASSERT_THROWS_NOTHING(nbErr = reader.proceed());
      TS_ASSERT_EQUALS(nbErr, gum::Size(0));

      TS_ASSERT(net != nullptr);

      if (net != nullptr) {
        TS_ASSERT_EQUALS(net->size(), (gum::Size)5);

        const gum::Potential< double >& proba = net->cpt(net->idFromName("2"));

        TS_ASSERT_EQUALS(proba.domainSize(), (gum::Size)8);

        delete (net);
      }
    }

    void testRead_file3() {
      std::string file = GET_RESSOURCES_PATH("uai/BNUAIReader_file3.uai");
      gum::BayesNet< double >* net = new gum::BayesNet< double >();
      gum::UAIBNReader< double > reader(net, file);

      gum::Size nbErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbErr = reader.proceed());
      TS_ASSERT_EQUALS(nbErr, gum::Size(0));
      TS_ASSERT(net != nullptr);

      if (net != nullptr) {
        gum::HashTable< std::string, gum::NodeId > idMap;

        for (const auto node: net->nodes())
          idMap.insert(net->variable(node).name(), node);

        const gum::DiscreteVariable& var_1 = net->variable(idMap["4"]);

        TS_ASSERT_EQUALS(var_1.name(), "4");

        TS_ASSERT_EQUALS(var_1.domainSize(), (gum::Size)2);

        TS_ASSERT_EQUALS(var_1.label(0), "0");

        TS_ASSERT_EQUALS(var_1.label(1), "1");

        const gum::Potential< double >& proba_1 = net->cpt(idMap["4"]);

        TS_ASSERT_EQUALS(proba_1.domainSize(), (gum::Size)2);

        gum::Instantiation inst_1(proba_1);

        inst_1.setFirst();

        TS_ASSERT_DELTA(proba_1[inst_1], 0.2, 0.001);

        inst_1.setLast();

        TS_ASSERT_DELTA(proba_1[inst_1], 0.8, 0.001);

        const gum::DiscreteVariable& var_2 = net->variable(idMap["2"]);

        TS_ASSERT_EQUALS(var_2.name(), "2");

        TS_ASSERT_EQUALS(var_2.domainSize(), (gum::Size)2);

        TS_ASSERT_EQUALS(var_2.label(0), "0");

        TS_ASSERT_EQUALS(var_2.label(1), "1");

        const gum::Potential< double >& proba_2 = net->cpt(idMap["2"]);

        TS_ASSERT_EQUALS(proba_2.domainSize(), (gum::Size)2);

        gum::Instantiation inst_2(proba_2);

        inst_2.setFirst();

        TS_ASSERT_DELTA(proba_2[inst_2], 0.3, 0.001);

        inst_2.setLast();

        TS_ASSERT_DELTA(proba_2[inst_2], 0.7, 0.001);

        const gum::DiscreteVariable& var_3 = net->variable(idMap["0"]);

        TS_ASSERT_EQUALS(var_3.name(), "0");

        TS_ASSERT_EQUALS(var_3.domainSize(), (gum::Size)2);

        TS_ASSERT_EQUALS(var_3.label(0), "0");

        TS_ASSERT_EQUALS(var_3.label(1), "1");

        TS_ASSERT(net->dag().existsArc(idMap["4"], idMap["0"]));

        const gum::Potential< double >& proba_3 = net->cpt(idMap["0"]);

        TS_ASSERT_EQUALS(proba_3.domainSize(), (gum::Size)4);

        gum::Instantiation inst_3(proba_3);

        inst_3.chgVal(var_1, 0);

        inst_3.chgVal(var_3, 0);

        TS_ASSERT_DELTA(proba_3[inst_3], 0.1, 0.001);

        inst_3.chgVal(var_3, 1);

        TS_ASSERT_DELTA(proba_3[inst_3], 0.9, 0.001);

        inst_3.chgVal(var_1, 1);

        inst_3.chgVal(var_3, 0);

        TS_ASSERT_DELTA(proba_3[inst_3], 0.9, 0.001);

        inst_3.chgVal(var_3, 1);

        TS_ASSERT_DELTA(proba_3[inst_3], 0.1, 0.001);

        const gum::DiscreteVariable& var_4 = net->variable(idMap["3"]);

        TS_ASSERT_EQUALS(var_4.name(), "3");

        TS_ASSERT_EQUALS(var_4.domainSize(), (gum::Size)2);

        TS_ASSERT_EQUALS(var_4.label(0), "0");

        TS_ASSERT_EQUALS(var_4.label(1), "1");

        TS_ASSERT(net->dag().existsArc(idMap["4"], idMap["3"]));

        TS_ASSERT(net->dag().existsArc(idMap["2"], idMap["3"]));

        const gum::Potential< double >& proba_4 = net->cpt(idMap["3"]);

        TS_ASSERT_EQUALS(proba_4.domainSize(), (gum::Size)8);

        gum::Instantiation inst_4(proba_4);

        inst_4.chgVal(var_1, 0);

        inst_4.chgVal(var_2, 0);

        inst_4.chgVal(var_4, 0);

        TS_ASSERT_DELTA(proba_4[inst_4], 0.4, 0.001);

        inst_4.chgVal(var_4, 1);

        TS_ASSERT_DELTA(proba_4[inst_4], 0.6, 0.001);

        inst_4.chgVal(var_1, 1);

        inst_4.chgVal(var_2, 0);

        inst_4.chgVal(var_4, 0);

        TS_ASSERT_DELTA(proba_4[inst_4], 0.5, 0.001);

        inst_4.chgVal(var_4, 1);

        TS_ASSERT_DELTA(proba_4[inst_4], 0.5, 0.001);

        inst_4.chgVal(var_1, 0);

        inst_4.chgVal(var_2, 1);

        inst_4.chgVal(var_4, 0);

        TS_ASSERT_DELTA(proba_4[inst_4], 0.5, 0.001);

        inst_4.chgVal(var_4, 1);

        TS_ASSERT_DELTA(proba_4[inst_4], 0.5, 0.001);

        inst_4.chgVal(var_1, 1);

        inst_4.chgVal(var_2, 1);

        inst_4.chgVal(var_4, 0);

        TS_ASSERT(proba_4[inst_4] == 1);

        inst_4.chgVal(var_4, 1);

        TS_ASSERT(proba_4[inst_4] == 0);

        const gum::DiscreteVariable& var_5 = net->variable(idMap["1"]);

        TS_ASSERT_EQUALS(var_5.name(), "1");

        TS_ASSERT_EQUALS(var_5.domainSize(), (gum::Size)3);

        TS_ASSERT_EQUALS(var_5.label(0), "0");

        TS_ASSERT_EQUALS(var_5.label(1), "1");

        TS_ASSERT_EQUALS(var_5.label(2), "2");

        TS_ASSERT(net->dag().existsArc(idMap["2"], idMap["1"]));

        TS_ASSERT(net->dag().existsArc(idMap["0"], idMap["1"]));

        const gum::Potential< double >& proba_5 = net->cpt(idMap["1"]);

        TS_ASSERT_EQUALS(proba_5.domainSize(), (gum::Size)12);

        gum::Instantiation inst_5(proba_5);

        inst_5.chgVal(var_3, 0);

        inst_5.chgVal(var_2, 0);

        inst_5.chgVal(var_5, 0);

        TS_ASSERT_DELTA(proba_5[inst_5], 0.3, 0.001);

        inst_5.chgVal(var_5, 1);

        TS_ASSERT_DELTA(proba_5[inst_5], 0.6, 0.001);

        inst_5.chgVal(var_5, 2);

        TS_ASSERT_DELTA(proba_5[inst_5], 0.1, 0.001);

        inst_5.chgVal(var_2, 0);

        inst_5.chgVal(var_3, 1);

        inst_5.chgVal(var_5, 0);

        TS_ASSERT_DELTA(proba_5[inst_5], 0.5, 0.001);

        inst_5.chgVal(var_5, 1);

        TS_ASSERT_DELTA(proba_5[inst_5], 0.5, 0.001);

        inst_5.chgVal(var_5, 2);

        TS_ASSERT_DELTA(proba_5[inst_5], 0.0, 0.001);

        inst_5.chgVal(var_2, 1);

        inst_5.chgVal(var_3, 0);

        inst_5.chgVal(var_5, 0);

        TS_ASSERT_DELTA(proba_5[inst_5], 0.4, 0.001);

        inst_5.chgVal(var_5, 1);

        TS_ASSERT_DELTA(proba_5[inst_5], 0.6, 0.001);

        inst_5.chgVal(var_5, 2);

        TS_ASSERT_DELTA(proba_5[inst_5], 0.0, 0.001);

        inst_5.chgVal(var_2, 1);

        inst_5.chgVal(var_3, 1);

        inst_5.chgVal(var_5, 0);

        TS_ASSERT_DELTA(proba_5[inst_5], 0.5, 0.001);

        inst_5.chgVal(var_5, 1);

        TS_ASSERT_DELTA(proba_5[inst_5], 0.5, 0.001);

        inst_5.chgVal(var_5, 2);

        TS_ASSERT(proba_5[inst_5] == 0);

        const gum::DiscreteVariable& var_6 = net->variable(idMap["5"]);

        TS_ASSERT_EQUALS(var_6.name(), "5");

        TS_ASSERT_EQUALS(var_6.domainSize(), (gum::Size)2);

        TS_ASSERT_EQUALS(var_6.label(0), "0");

        TS_ASSERT_EQUALS(var_6.label(1), "1");

        TS_ASSERT(net->dag().existsArc(idMap["4"], idMap["5"]));

        TS_ASSERT(net->dag().existsArc(idMap["1"], idMap["5"]));

        const gum::Potential< double >& proba_6 = net->cpt(idMap["5"]);

        TS_ASSERT_EQUALS(proba_6.domainSize(), (gum::Size)12);

        gum::Instantiation inst_6(proba_6);

        inst_6.chgVal(var_6, 0);

        inst_6.chgVal(var_1, 0);

        inst_6.chgVal(var_5, 0);

        TS_ASSERT_DELTA(proba_6[inst_6], 0.1, 0.001);

        inst_6.chgVal(var_5, 1);

        TS_ASSERT_DELTA(proba_6[inst_6], 0.2, 0.001);

        inst_6.chgVal(var_5, 2);

        TS_ASSERT_DELTA(proba_6[inst_6], 0.3, 0.001);

        inst_6.chgVal(var_1, 1);

        inst_6.chgVal(var_5, 0);

        TS_ASSERT_DELTA(proba_6[inst_6], 0.4, 0.001);

        inst_6.chgVal(var_5, 1);

        TS_ASSERT_DELTA(proba_6[inst_6], 0.5, 0.001);

        inst_6.chgVal(var_5, 2);

        TS_ASSERT_DELTA(proba_6[inst_6], 0.6, 0.001);

        inst_6.chgVal(var_6, 1);

        inst_6.chgVal(var_1, 0);

        inst_6.chgVal(var_5, 0);

        TS_ASSERT_DELTA(proba_6[inst_6], 0.7, 0.001);

        inst_6.chgVal(var_5, 1);

        TS_ASSERT_DELTA(proba_6[inst_6], 0.8, 0.001);

        inst_6.chgVal(var_5, 2);

        TS_ASSERT_DELTA(proba_6[inst_6], 0.9, 0.001);

        inst_6.chgVal(var_1, 1);

        inst_6.chgVal(var_5, 0);

        TS_ASSERT(proba_6[inst_6] == 1);

        inst_6.chgVal(var_5, 1);

        TS_ASSERT(proba_6[inst_6] == 0);

        inst_6.chgVal(var_5, 2);

        TS_ASSERT(proba_6[inst_6] == 0);

        delete net;
      }
    }

    void testDogRead() {
      std::string              file = GET_RESSOURCES_PATH("uai/dog.uai");
      gum::BayesNet< double >  net;
      gum::UAIBNReader< double > reader(&net, file);

      gum::Size nbErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbErr = reader.proceed());
      TS_ASSERT_EQUALS(nbErr, gum::Size(0));
    }
    void testAsiaRead() {
      std::string              file = GET_RESSOURCES_PATH("uai/asia.uai");
      gum::BayesNet< double >  net;
      gum::UAIBNReader< double > reader(&net, file);

      gum::Size nbErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbErr = reader.proceed());
      TS_ASSERT_EQUALS(nbErr, gum::Size(0));
    }
  };
}   // namespace gum_tests
