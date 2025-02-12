/**
 *
 *   Copyright (c) 2005-2024 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
#include <gumtest/utils.h>

#include <agrum/tools/variables/labelizedVariable.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/net/netReader.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class [[maybe_unused]] NetReaderTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(Constuctor) {
      std::string file = GET_RESSOURCES_PATH("net/test1.net");

      gum::BayesNet< double > net;

      gum::NetReader< double >* reader = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(reader = new gum::NetReader< double >(&net, file))

      TS_GUM_ASSERT_THROWS_NOTHING(delete reader)
    }

    GUM_ACTIVE_TEST(Read_not_existing_file) {
      std::string file = GET_RESSOURCES_PATH("net/test.net");

      gum::BayesNet< double >* net = new gum::BayesNet< double >();

      gum::NetReader< double > reader(net, file);

      TS_ASSERT_THROWS(reader.trace(false), const gum::IOError&)

      gum::Size nbrErr = 0;

      TS_ASSERT_THROWS(nbrErr = reader.proceed(), const gum::IOError&)

      TS_ASSERT(!nbrErr)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      TS_ASSERT_DIFFERS(net, nullptr)

      if (net != nullptr) {
        TS_ASSERT(net->empty())

        delete net;
      }
    }

    GUM_ACTIVE_TEST(Read_file1) {
      std::string file = GET_RESSOURCES_PATH("net/test1.net");

      gum::BayesNet< double >* net = new gum::BayesNet< double >();

      gum::NetReader< double > reader(net, file);

      reader.trace(false);

      gum::Size nbrErr = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())

      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      TS_ASSERT_DIFFERS(net, nullptr)

      if (net != nullptr) {
        TS_ASSERT(!net->empty())

        delete net;
      }
    }

    GUM_ACTIVE_TEST(Read_file2) {
      std::string file = GET_RESSOURCES_PATH("net/test2.net");

      gum::BayesNet< double >* net = new gum::BayesNet< double >();

      gum::NetReader< double > reader(net, file);

      reader.trace(false);

      gum::Size nbrErr = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())

      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      TS_ASSERT_DIFFERS(net, nullptr)

      if (net != nullptr) {
        TS_ASSERT(!net->empty())

        delete net;
      }
    }

    GUM_ACTIVE_TEST(Read_file3) {
      std::string file = GET_RESSOURCES_PATH("net/test3.net");

      gum::BayesNet< double >* net = new gum::BayesNet< double >();

      gum::NetReader< double > reader(net, file);

      reader.trace(false);

      gum::Size nbrErr = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())

      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      TS_ASSERT_DIFFERS(net, nullptr)

      if (net != nullptr) {
        TS_ASSERT(!net->empty())

        delete net;
      }
    }

    GUM_ACTIVE_TEST(Read_file_with_xp) {
      std::string file = GET_RESSOURCES_PATH("net/bn_with_xp.net");

      gum::BayesNet< double >* net = new gum::BayesNet< double >();

      gum::NetReader< double > reader(net, file);

      reader.trace(false);

      gum::Size nbrErr = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())

      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
      reader.showElegantErrors();

      TS_ASSERT_DIFFERS(net, nullptr)

      if (net != nullptr) {
        TS_ASSERT(!net->empty())

        delete net;
      }
    }
  };
}   // namespace gum_tests
