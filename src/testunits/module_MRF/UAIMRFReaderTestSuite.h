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
#include <agrum/MRF/io/UAI/UAIMRFReader.h>
#include <agrum/MRF/MarkovRandomField.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  UAIMRFReader
#define GUM_CURRENT_MODULE MRF

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  struct UAIMRFReaderTestSuite {
    public:
    static void testConstructor() {
      std::string                      file = GET_RESSOURCES_PATH("uai/markov_example.uai");
      gum::MarkovRandomField< double > net;

      gum::UAIMRFReader< double >* reader = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(reader = new gum::UAIMRFReader< double >(&net, file));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete reader);
    }   // namespace gum_tests

    static void testRead_file1() {
      std::string                       file = GET_RESSOURCES_PATH("uai/markov_example.uai");
      gum::MarkovRandomField< double >* net  = new gum::MarkovRandomField< double >();

      CHECK((net) != (nullptr));

      gum::UAIMRFReader< double > reader(net, file);
      reader.proceed();

      if (net != nullptr) {
        CHECK(!net->empty());
        delete net;
      }
    }

    static void testRead_file2() {
      std::string                       file = GET_RESSOURCES_PATH("uai/markov_example.uai");
      gum::MarkovRandomField< double >* net  = new gum::MarkovRandomField< double >();
      gum::UAIMRFReader< double >       reader(net, file);

      gum::Size nbErr = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbErr = reader.proceed());
      CHECK((nbErr) == (static_cast< gum::Size >(0)));

      CHECK((net) != (nullptr));

      if (net != nullptr) {
        CHECK(!net->empty());
        CHECK((net->size()) == (static_cast< gum::Size >(3)));

        gum::NodeId node_0 = 0, node_1 = 0, node_2 = 0;
        node_0 = net->idFromName("0");
        node_1 = net->idFromName("1");
        node_2 = net->idFromName("2");

        const gum::DiscreteVariable& var_0 = net->variable(node_0);
        CHECK((var_0.name()) == ("0"));
        CHECK((var_0.domainSize()) == (static_cast< gum::Size >(2)));

        const gum::DiscreteVariable& var_2 = net->variable(node_2);
        CHECK((var_2.name()) == ("2"));
        CHECK((var_2.domainSize()) == (static_cast< gum::Size >(3)));

        CHECK((net->factors().size()) == (static_cast< gum::Size >(2)));

        const auto& factor_01 = net->factor({node_0, node_1});
        CHECK((factor_01.domainSize()) == (static_cast< gum::Size >(4)));
        CHECK((factor_01.nbrDim()) == (static_cast< gum::Size >(2)));

        gum::Instantiation inst_01(factor_01);
        inst_01.setFirst();
        CHECK((factor_01[inst_01]) == doctest::Approx(0.128).epsilon(0.001));
        inst_01.setLast();
        CHECK((factor_01[inst_01]) == doctest::Approx(0.08).epsilon(0.001));

        const auto& factor_12 = net->factor({node_2, node_1});
        CHECK((factor_12.domainSize()) == (static_cast< gum::Size >(6)));
        CHECK((factor_12.nbrDim()) == (static_cast< gum::Size >(2)));

        gum::Instantiation inst_12(factor_12);
        inst_12.setFirst();
        CHECK((factor_12[inst_12]) == doctest::Approx(0.21).epsilon(0.001));
        inst_12.setLast();
        CHECK((factor_12[inst_12]) == doctest::Approx(0.189).epsilon(0.001));

        delete net;
      }
    }

    static void testReadInFilledMN() {
      std::string                      file = GET_RESSOURCES_PATH("uai/markov_example.uai");
      gum::MarkovRandomField< double > net;
      gum::UAIMRFReader< double >      reader(&net, file);

      gum::Size nbErr = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbErr = reader.proceed());
      CHECK((nbErr) == (static_cast< gum::Size >(0)));

      CHECK(!net.empty());
      CHECK((net.size()) == (static_cast< gum::Size >(3)));

      net = gum::MarkovRandomField< double >::fastPrototype("A--B");
      gum::UAIMRFReader< double > reader2(&net, file);

      GUM_CHECK_ASSERT_THROWS_NOTHING(nbErr = reader2.proceed());
      CHECK((nbErr) == (static_cast< gum::Size >(0)));

      CHECK(!net.empty());
      CHECK((net.size()) == (static_cast< gum::Size >(3)));
    }
  };

  GUM_TEST_ACTIF(Constructor)
  GUM_TEST_ACTIF(Read_file1)
  GUM_TEST_ACTIF(Read_file2)
  GUM_TEST_ACTIF(ReadInFilledMN)
}   // namespace gum_tests
