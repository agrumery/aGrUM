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


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/net/netReader.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  NetReader
#define GUM_CURRENT_MODULE BN

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  struct NetReaderTestSuite {
    public:
    static void testConstuctor() {
      std::string file = GET_RESSOURCES_PATH("net/test1.net");

      gum::BayesNet< double > net;

      gum::NetReader< double >* reader = 0;

      GUM_CHECK_ASSERT_THROWS_NOTHING(reader = new gum::NetReader< double >(&net, file));

      GUM_CHECK_ASSERT_THROWS_NOTHING(delete reader);
    }   // namespace gum_tests

    static void testRead_not_existing_file() {
      std::string file = GET_RESSOURCES_PATH("net/test.net");

      gum::BayesNet< double >* net = new gum::BayesNet< double >();

      gum::NetReader< double > reader(net, file);

      CHECK_THROWS_AS(reader.trace(false), const gum::IOError&);

      gum::Size nbrErr = 0;

      CHECK_THROWS_AS(nbrErr = reader.proceed(), const gum::IOError&);

      CHECK(!nbrErr);
      CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));
      // 0 warnings : no properties
      CHECK((reader.errors()) == (static_cast< gum::Size >(0)));

      CHECK((net) != (nullptr));

      if (net != nullptr) {
        CHECK(net->empty());

        delete net;
      }
    }

    static void testRead_file1() {
      std::string file = GET_RESSOURCES_PATH("net/test1.net");

      gum::BayesNet< double >* net = new gum::BayesNet< double >();

      gum::NetReader< double > reader(net, file);

      reader.trace(false);

      gum::Size nbrErr = 0;

      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());

      CHECK((nbrErr) == (static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));
      // 0 warnings : no properties
      CHECK((reader.errors()) == (static_cast< gum::Size >(0)));

      CHECK((net) != (nullptr));

      if (net != nullptr) {
        CHECK(!net->empty());

        delete net;
      }
    }

    static void testRead_file2() {
      std::string file = GET_RESSOURCES_PATH("net/test2.net");

      gum::BayesNet< double >* net = new gum::BayesNet< double >();

      gum::NetReader< double > reader(net, file);

      reader.trace(false);

      gum::Size nbrErr = 0;

      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());

      CHECK((nbrErr) == (static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));
      // 0 warnings : no properties
      CHECK((reader.errors()) == (static_cast< gum::Size >(0)));

      CHECK((net) != (nullptr));

      if (net != nullptr) {
        CHECK(!net->empty());

        delete net;
      }
    }

    static void testRead_file3() {
      std::string file = GET_RESSOURCES_PATH("net/test3.net");

      gum::BayesNet< double >* net = new gum::BayesNet< double >();

      gum::NetReader< double > reader(net, file);

      reader.trace(false);

      gum::Size nbrErr = 0;

      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());

      CHECK((nbrErr) == (static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));
      // 0 warnings : no properties
      CHECK((reader.errors()) == (static_cast< gum::Size >(0)));

      CHECK((net) != (nullptr));

      if (net != nullptr) {
        CHECK(!net->empty());

        delete net;
      }
    }

    static void testRead_file_with_xp() {
      std::string file = GET_RESSOURCES_PATH("net/bn_with_xp.net");

      gum::BayesNet< double >* net = new gum::BayesNet< double >();

      gum::NetReader< double > reader(net, file);

      reader.trace(false);

      gum::Size nbrErr = 0;

      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());

      CHECK((nbrErr) == (static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));
      // 0 warnings : no properties
      CHECK((reader.errors()) == (static_cast< gum::Size >(0)));
      reader.showElegantErrors();

      CHECK((net) != (nullptr));

      if (net != nullptr) {
        CHECK(!net->empty());

        delete net;
      }
    }
  };

  GUM_TEST_ACTIF(Constuctor)
  GUM_TEST_ACTIF(Read_not_existing_file)
  GUM_TEST_ACTIF(Read_file1)
  GUM_TEST_ACTIF(Read_file2)
  GUM_TEST_ACTIF(Read_file3)
  GUM_TEST_ACTIF(Read_file_with_xp)
}   // namespace gum_tests
