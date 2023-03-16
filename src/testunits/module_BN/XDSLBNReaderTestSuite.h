/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/XDSL/XDSLBNReader.h>
#include <agrum/tools/variables/labelizedVariable.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class [[maybe_unused]] XDSLBNReaderTestSuite: public CxxTest::TestSuite {
    public:
    GUM_TEST(Constuctor) {
      std::string             file = GET_RESSOURCES_PATH("xdsl/benefits.xdsl");
      gum::BayesNet< double > net;

      gum::XDSLBNReader< double >* reader = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(reader = new gum::XDSLBNReader< double >(&net, file))
      TS_GUM_ASSERT_THROWS_NOTHING(delete reader)
    }

    GUM_TEST(Read_file1) {
      std::string              file = GET_RESSOURCES_PATH("xdsl/benefits.xdsl");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();

      TS_ASSERT_DIFFERS(net, nullptr)

      gum::XDSLBNReader< double > reader(net, file);
      TS_GUM_ASSERT_THROWS_NOTHING(reader.proceed())

      TS_ASSERT_EQUALS(net->propertyWithDefault("name", ""), "Network3")

      if (net != nullptr) {
        TS_ASSERT(!net->empty())
        delete net;
      }
    }
  };
}   // namespace gum_tests
