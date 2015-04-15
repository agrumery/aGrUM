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

#include <agrum/BN/io/net/netReader.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class NetReaderTestSuite : public CxxTest::TestSuite {

    public:
    void testConstuctor() {
      std::string file = GET_PATH_STR("test1.net");

      gum::BayesNet<float> net;

      gum::NetReader<float> *reader = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(reader = new gum::NetReader<float>(&net, file));

      TS_GUM_ASSERT_THROWS_NOTHING(delete reader);
    }

    void testRead_not_existing_file() {
      std::string file = GET_PATH_STR("test.net");

      gum::BayesNet<float> *net = new gum::BayesNet<float>();

      gum::NetReader<float> reader(net, file);

      TS_ASSERT_THROWS(reader.trace(false), gum::IOError);

      int nbrErr = 0;

      TS_ASSERT_THROWS(nbrErr = reader.proceed(), gum::IOError);

      TS_ASSERT(!nbrErr);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      TS_ASSERT(net != 0);

      if (net != 0) {
        TS_ASSERT(net->empty());

        delete net;
      }
    }

    void testRead_file1() {
      std::string file = GET_PATH_STR("test1.net");

      gum::BayesNet<float> *net = new gum::BayesNet<float>();

      gum::NetReader<float> reader(net, file);

      reader.trace(false);

      int nbrErr = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());

      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      TS_ASSERT(net != 0);

      if (net != 0) {
        TS_ASSERT(!net->empty());

        delete net;
      }
    }

    void testRead_file2() {
      std::string file = GET_PATH_STR("test2.net");

      gum::BayesNet<float> *net = new gum::BayesNet<float>();

      gum::NetReader<float> reader(net, file);

      reader.trace(false);

      int nbrErr = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());

      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      TS_ASSERT(net != 0);

      if (net != 0) {
        TS_ASSERT(!net->empty());

        delete net;
      }
    }

    void testRead_file3() {
      std::string file = GET_PATH_STR("test3.net");

      gum::BayesNet<float> *net = new gum::BayesNet<float>();

      gum::NetReader<float> reader(net, file);

      reader.trace(false);

      int nbrErr = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());

      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

      TS_ASSERT(net != 0);

      if (net != 0) {
        TS_ASSERT(!net->empty());

        delete net;
      }
    }

    void testRead_file_with_xp() {
      std::string file = GET_PATH_STR("bn_with_xp.net");

      gum::BayesNet<float> *net = new gum::BayesNet<float>();

      gum::NetReader<float> reader(net, file);

      reader.trace(false);

      int nbrErr = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());

      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      // 0 warnings : no properties
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
      reader.showElegantErrors();

      TS_ASSERT(net != 0);

      if (net != 0) {
        TS_ASSERT(!net->empty());

        delete net;
      }
    }
  };
}
