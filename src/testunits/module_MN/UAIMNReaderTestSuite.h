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

#include <agrum/MN/MarkovNet.h>
#include <agrum/MN/io/UAI/UAIMNReader.h>
#include <agrum/tools/variables/labelizedVariable.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class UAIMNReaderTestSuite: public CxxTest::TestSuite {
    public:
    void testConstructor() {
      std::string file = GET_RESSOURCES_PATH("uai/markov_example.uai");
      gum::MarkovNet< double > net;

      gum::UAIMNReader< double >* reader = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(reader =
                                      new gum::UAIMNReader< double >(&net, file));
      TS_GUM_ASSERT_THROWS_NOTHING(delete reader);
    }

    void testRead_file1() {
      std::string file = GET_RESSOURCES_PATH("uai/markov_example.uai");
      gum::MarkovNet< double >* net = new gum::MarkovNet< double >();

      TS_ASSERT(net != nullptr);

      gum::UAIMNReader< double > reader(net, file);
      reader.proceed();

      if (net != nullptr) {
        TS_ASSERT(! net->empty());
        delete net;
      }
    }

    void testRead_file2() {
      std::string file = GET_RESSOURCES_PATH("uai/markov_example.uai");
      gum::MarkovNet< double >* net = new gum::MarkovNet< double >();
      gum::UAIMNReader< double > reader(net, file);

      gum::Size nbErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbErr = reader.proceed());
      TS_ASSERT_EQUALS(nbErr, gum::Size(0));

      TS_ASSERT(net != nullptr);

      if (net != nullptr) {
        TS_ASSERT(!net->empty());
        TS_ASSERT_EQUALS(net->size(), (gum::Size)3);

        gum::NodeId node_0 = 0, node_1 = 0, node_2 = 0;
        node_0 = net->idFromName("0");
        node_1 = net->idFromName("1");
        node_2 = net->idFromName("2");

        const gum::DiscreteVariable& var_0 = net->variable(node_0);
        TS_ASSERT_EQUALS(var_0.name(), "0");
        TS_ASSERT_EQUALS(var_0.domainSize(), (gum::Size)2);

        const gum::DiscreteVariable& var_2 = net->variable(node_2);
        TS_ASSERT_EQUALS(var_2.name(), "2");
        TS_ASSERT_EQUALS(var_2.domainSize(), (gum::Size)3);

        TS_ASSERT_EQUALS(net->factors().size(),(gum::Size)2);

        const auto& factor_01 = net->factor({node_0,node_1});
        TS_ASSERT_EQUALS(factor_01.domainSize(), (gum::Size)4);
        TS_ASSERT_EQUALS(factor_01.nbrDim(), (gum::Size)2);

        gum::Instantiation inst_01(factor_01);
        inst_01.setFirst();
        TS_ASSERT_DELTA(factor_01[inst_01], 0.128, 0.001);
        inst_01.setLast();
        TS_ASSERT_DELTA(factor_01[inst_01], 0.08, 0.001);

        const auto& factor_12 = net->factor({node_2,node_1});
        TS_ASSERT_EQUALS(factor_12.domainSize(), (gum::Size)6);
        TS_ASSERT_EQUALS(factor_12.nbrDim(), (gum::Size)2);

        gum::Instantiation inst_12(factor_12);
        inst_12.setFirst();
        TS_ASSERT_DELTA(factor_12[inst_12], 0.21, 0.001);
        inst_12.setLast();
        TS_ASSERT_DELTA(factor_12[inst_12], 0.189, 0.001);

        delete net;
      }
    }
  };
}   // namespace gum_tests
