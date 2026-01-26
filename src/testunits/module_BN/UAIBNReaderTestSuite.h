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
#include <agrum/BN/io/UAI/UAIBNReader.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  UAIBNReader
#define GUM_CURRENT_MODULE BN

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  struct UAIBNReaderTestSuite {
    public:
    static void testConstuctor() {
      std::string             file = GET_RESSOURCES_PATH("uai/BNUAIReader_file1.uai");
      gum::BayesNet< double > net;

      gum::UAIBNReader< double >* reader = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(reader = new gum::UAIBNReader< double >(&net, file));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete reader);
    }   // namespace gum_tests

    static void testRead_file1() {
      std::string              file = GET_RESSOURCES_PATH("uai/BNUAIReader_file1.uai");
      gum::BayesNet< double >* net  = new gum::BayesNet< double >();

      CHECK((net) != (nullptr));

      gum::UAIBNReader< double > reader(net, file);
      reader.proceed();

      if (net != nullptr) {
        CHECK(net->empty());
        delete net;
      }
    }

    static void testRead_file2_float() {
      std::string                file = GET_RESSOURCES_PATH("uai/BNUAIReader_file2.uai");
      gum::BayesNet< double >*   net  = new gum::BayesNet< double >();
      gum::UAIBNReader< double > reader(net, file);

      gum::Size nbErr = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbErr = reader.proceed());
      CHECK((nbErr) == (static_cast< gum::Size >(0)));

      CHECK((net) != (nullptr));

      if (net != nullptr) {
        CHECK(!net->empty());
        CHECK((net->size()) == (static_cast< gum::Size >(5)));

        gum::NodeId node_1 = 0, node_2 = 0;
        node_1 = net->idFromName("0");
        node_2 = net->idFromName("1");

        const gum::DiscreteVariable& var_1 = net->variable(node_1);

        CHECK((var_1.name()) == ("0"));
        CHECK((var_1.domainSize()) == (static_cast< gum::Size >(2)));

        const gum::Tensor< double >& proba_1 = net->cpt(node_1);
        CHECK((proba_1.domainSize()) == (static_cast< gum::Size >(2)));

        gum::Instantiation inst_1(proba_1);
        inst_1.setFirst();
        CHECK((proba_1[inst_1]) == doctest::Approx(0.2).epsilon(0.001));
        inst_1.setLast();
        CHECK((proba_1[inst_1]) == doctest::Approx(0.8).epsilon(0.001));

        const gum::DiscreteVariable& var_2 = net->variable(node_2);
        CHECK((var_2.name()) == ("1"));
        CHECK((var_2.domainSize()) == (static_cast< gum::Size >(2)));

        const gum::Tensor< double >& proba_2 = net->cpt(node_2);
        CHECK((proba_2.domainSize()) == (static_cast< gum::Size >(2)));

        gum::Instantiation inst_2(proba_2);
        inst_2.setFirst();
        CHECK((proba_2[inst_2]) == doctest::Approx(0.3).epsilon(0.001));
        inst_2.setLast();
        CHECK((proba_2[inst_2]) == doctest::Approx(0.7).epsilon(0.001));
        delete net;
      }
    }

    static void testRead_dog_double() {
      // from Charniak, Bayesian networks Without Tears, AI Magazine, 1991
      std::string                file = GET_RESSOURCES_PATH("uai/dog.uai");
      gum::BayesNet< double >*   net  = new gum::BayesNet< double >();
      gum::UAIBNReader< double > reader(net, file);

      gum::Size nbErr = false;
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbErr = reader.proceed());
      CHECK((nbErr) == (static_cast< gum::Size >(0)));

      CHECK((net) != (nullptr));

      if (net != nullptr) {
        CHECK((net->size()) == (static_cast< gum::Size >(5)));

        const gum::Tensor< double >& proba = net->cpt(net->idFromName("2"));

        CHECK((proba.domainSize()) == (static_cast< gum::Size >(8)));

        delete (net);
      }
    }

    static void testRead_file3() {
      std::string                file = GET_RESSOURCES_PATH("uai/BNUAIReader_file3.uai");
      gum::BayesNet< double >*   net  = new gum::BayesNet< double >();
      gum::UAIBNReader< double > reader(net, file);

      gum::Size nbErr = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbErr = reader.proceed());
      CHECK((nbErr) == (static_cast< gum::Size >(0)));
      CHECK((net) != (nullptr));

      if (net != nullptr) {
        gum::HashTable< std::string, gum::NodeId > idMap;

        for (const auto node: net->nodes())
          idMap.insert(net->variable(node).name(), node);

        const gum::DiscreteVariable& var_1 = net->variable(idMap["4"]);

        CHECK((var_1.name()) == ("4"));

        CHECK((var_1.domainSize()) == (static_cast< gum::Size >(2)));

        CHECK((var_1.label(0)) == ("0"));

        CHECK((var_1.label(1)) == ("1"));

        const gum::Tensor< double >& proba_1 = net->cpt(idMap["4"]);

        CHECK((proba_1.domainSize()) == (static_cast< gum::Size >(2)));

        gum::Instantiation inst_1(proba_1);

        inst_1.setFirst();

        CHECK((proba_1[inst_1]) == doctest::Approx(0.2).epsilon(0.001));

        inst_1.setLast();

        CHECK((proba_1[inst_1]) == doctest::Approx(0.8).epsilon(0.001));

        const gum::DiscreteVariable& var_2 = net->variable(idMap["2"]);

        CHECK((var_2.name()) == ("2"));

        CHECK((var_2.domainSize()) == (static_cast< gum::Size >(2)));

        CHECK((var_2.label(0)) == ("0"));

        CHECK((var_2.label(1)) == ("1"));

        const gum::Tensor< double >& proba_2 = net->cpt(idMap["2"]);

        CHECK((proba_2.domainSize()) == (static_cast< gum::Size >(2)));

        gum::Instantiation inst_2(proba_2);

        inst_2.setFirst();

        CHECK((proba_2[inst_2]) == doctest::Approx(0.3).epsilon(0.001));

        inst_2.setLast();

        CHECK((proba_2[inst_2]) == doctest::Approx(0.7).epsilon(0.001));

        const gum::DiscreteVariable& var_3 = net->variable(idMap["0"]);

        CHECK((var_3.name()) == ("0"));

        CHECK((var_3.domainSize()) == (static_cast< gum::Size >(2)));

        CHECK((var_3.label(0)) == ("0"));

        CHECK((var_3.label(1)) == ("1"));

        CHECK(net->dag().existsArc(idMap["4"], idMap["0"]));

        const gum::Tensor< double >& proba_3 = net->cpt(idMap["0"]);

        CHECK((proba_3.domainSize()) == (static_cast< gum::Size >(4)));

        gum::Instantiation inst_3(proba_3);

        inst_3.chgVal(var_1, 0);

        inst_3.chgVal(var_3, 0);

        CHECK((proba_3[inst_3]) == doctest::Approx(0.1).epsilon(0.001));

        inst_3.chgVal(var_3, 1);

        CHECK((proba_3[inst_3]) == doctest::Approx(0.9).epsilon(0.001));

        inst_3.chgVal(var_1, 1);

        inst_3.chgVal(var_3, 0);

        CHECK((proba_3[inst_3]) == doctest::Approx(0.9).epsilon(0.001));

        inst_3.chgVal(var_3, 1);

        CHECK((proba_3[inst_3]) == doctest::Approx(0.1).epsilon(0.001));

        const gum::DiscreteVariable& var_4 = net->variable(idMap["3"]);

        CHECK((var_4.name()) == ("3"));

        CHECK((var_4.domainSize()) == (static_cast< gum::Size >(2)));

        CHECK((var_4.label(0)) == ("0"));

        CHECK((var_4.label(1)) == ("1"));

        CHECK(net->dag().existsArc(idMap["4"], idMap["3"]));

        CHECK(net->dag().existsArc(idMap["2"], idMap["3"]));

        const gum::Tensor< double >& proba_4 = net->cpt(idMap["3"]);

        CHECK((proba_4.domainSize()) == (static_cast< gum::Size >(8)));

        gum::Instantiation inst_4(proba_4);

        inst_4.chgVal(var_1, 0);

        inst_4.chgVal(var_2, 0);

        inst_4.chgVal(var_4, 0);

        CHECK((proba_4[inst_4]) == doctest::Approx(0.4).epsilon(0.001));

        inst_4.chgVal(var_4, 1);

        CHECK((proba_4[inst_4]) == doctest::Approx(0.6).epsilon(0.001));

        inst_4.chgVal(var_1, 1);

        inst_4.chgVal(var_2, 0);

        inst_4.chgVal(var_4, 0);

        CHECK((proba_4[inst_4]) == doctest::Approx(0.5).epsilon(0.001));

        inst_4.chgVal(var_4, 1);

        CHECK((proba_4[inst_4]) == doctest::Approx(0.5).epsilon(0.001));

        inst_4.chgVal(var_1, 0);

        inst_4.chgVal(var_2, 1);

        inst_4.chgVal(var_4, 0);

        CHECK((proba_4[inst_4]) == doctest::Approx(0.5).epsilon(0.001));

        inst_4.chgVal(var_4, 1);

        CHECK((proba_4[inst_4]) == doctest::Approx(0.5).epsilon(0.001));

        inst_4.chgVal(var_1, 1);

        inst_4.chgVal(var_2, 1);

        inst_4.chgVal(var_4, 0);

        CHECK((proba_4[inst_4]) == (1));

        inst_4.chgVal(var_4, 1);

        CHECK((proba_4[inst_4]) == (0));

        const gum::DiscreteVariable& var_5 = net->variable(idMap["1"]);

        CHECK((var_5.name()) == ("1"));

        CHECK((var_5.domainSize()) == (static_cast< gum::Size >(3)));

        CHECK((var_5.label(0)) == ("0"));

        CHECK((var_5.label(1)) == ("1"));

        CHECK((var_5.label(2)) == ("2"));

        CHECK(net->dag().existsArc(idMap["2"], idMap["1"]));

        CHECK(net->dag().existsArc(idMap["0"], idMap["1"]));

        const gum::Tensor< double >& proba_5 = net->cpt(idMap["1"]);

        CHECK((proba_5.domainSize()) == (static_cast< gum::Size >(12)));

        gum::Instantiation inst_5(proba_5);

        inst_5.chgVal(var_3, 0);

        inst_5.chgVal(var_2, 0);

        inst_5.chgVal(var_5, 0);

        CHECK((proba_5[inst_5]) == doctest::Approx(0.3).epsilon(0.001));

        inst_5.chgVal(var_5, 1);

        CHECK((proba_5[inst_5]) == doctest::Approx(0.6).epsilon(0.001));

        inst_5.chgVal(var_5, 2);

        CHECK((proba_5[inst_5]) == doctest::Approx(0.1).epsilon(0.001));

        inst_5.chgVal(var_2, 0);

        inst_5.chgVal(var_3, 1);

        inst_5.chgVal(var_5, 0);

        CHECK((proba_5[inst_5]) == doctest::Approx(0.5).epsilon(0.001));

        inst_5.chgVal(var_5, 1);

        CHECK((proba_5[inst_5]) == doctest::Approx(0.5).epsilon(0.001));

        inst_5.chgVal(var_5, 2);

        CHECK((proba_5[inst_5]) == doctest::Approx(0.0).epsilon(0.001));

        inst_5.chgVal(var_2, 1);

        inst_5.chgVal(var_3, 0);

        inst_5.chgVal(var_5, 0);

        CHECK((proba_5[inst_5]) == doctest::Approx(0.4).epsilon(0.001));

        inst_5.chgVal(var_5, 1);

        CHECK((proba_5[inst_5]) == doctest::Approx(0.6).epsilon(0.001));

        inst_5.chgVal(var_5, 2);

        CHECK((proba_5[inst_5]) == doctest::Approx(0.0).epsilon(0.001));

        inst_5.chgVal(var_2, 1);

        inst_5.chgVal(var_3, 1);

        inst_5.chgVal(var_5, 0);

        CHECK((proba_5[inst_5]) == doctest::Approx(0.5).epsilon(0.001));

        inst_5.chgVal(var_5, 1);

        CHECK((proba_5[inst_5]) == doctest::Approx(0.5).epsilon(0.001));

        inst_5.chgVal(var_5, 2);

        CHECK((proba_5[inst_5]) == (0));

        const gum::DiscreteVariable& var_6 = net->variable(idMap["5"]);

        CHECK((var_6.name()) == ("5"));

        CHECK((var_6.domainSize()) == (static_cast< gum::Size >(2)));

        CHECK((var_6.label(0)) == ("0"));

        CHECK((var_6.label(1)) == ("1"));

        CHECK(net->dag().existsArc(idMap["4"], idMap["5"]));

        CHECK(net->dag().existsArc(idMap["1"], idMap["5"]));

        const gum::Tensor< double >& proba_6 = net->cpt(idMap["5"]);

        CHECK((proba_6.domainSize()) == (static_cast< gum::Size >(12)));

        gum::Instantiation inst_6(proba_6);

        inst_6.chgVal(var_6, 0);

        inst_6.chgVal(var_1, 0);

        inst_6.chgVal(var_5, 0);

        CHECK((proba_6[inst_6]) == doctest::Approx(0.1).epsilon(0.001));

        inst_6.chgVal(var_5, 1);

        CHECK((proba_6[inst_6]) == doctest::Approx(0.2).epsilon(0.001));

        inst_6.chgVal(var_5, 2);

        CHECK((proba_6[inst_6]) == doctest::Approx(0.3).epsilon(0.001));

        inst_6.chgVal(var_1, 1);

        inst_6.chgVal(var_5, 0);

        CHECK((proba_6[inst_6]) == doctest::Approx(0.4).epsilon(0.001));

        inst_6.chgVal(var_5, 1);

        CHECK((proba_6[inst_6]) == doctest::Approx(0.5).epsilon(0.001));

        inst_6.chgVal(var_5, 2);

        CHECK((proba_6[inst_6]) == doctest::Approx(0.6).epsilon(0.001));

        inst_6.chgVal(var_6, 1);

        inst_6.chgVal(var_1, 0);

        inst_6.chgVal(var_5, 0);

        CHECK((proba_6[inst_6]) == doctest::Approx(0.7).epsilon(0.001));

        inst_6.chgVal(var_5, 1);

        CHECK((proba_6[inst_6]) == doctest::Approx(0.8).epsilon(0.001));

        inst_6.chgVal(var_5, 2);

        CHECK((proba_6[inst_6]) == doctest::Approx(0.9).epsilon(0.001));

        inst_6.chgVal(var_1, 1);

        inst_6.chgVal(var_5, 0);

        CHECK((proba_6[inst_6]) == (1));

        inst_6.chgVal(var_5, 1);

        CHECK((proba_6[inst_6]) == (0));

        inst_6.chgVal(var_5, 2);

        CHECK((proba_6[inst_6]) == (0));

        delete net;
      }
    }

    static void testDogRead() {
      std::string                file = GET_RESSOURCES_PATH("uai/dog.uai");
      gum::BayesNet< double >    net;
      gum::UAIBNReader< double > reader(&net, file);

      gum::Size nbErr = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbErr = reader.proceed());
      CHECK((nbErr) == (static_cast< gum::Size >(0)));
    }

    static void testAsiaRead() {
      std::string                file = GET_RESSOURCES_PATH("uai/asia.uai");
      gum::BayesNet< double >    net;
      gum::UAIBNReader< double > reader(&net, file);

      gum::Size nbErr = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbErr = reader.proceed());
      CHECK((nbErr) == (static_cast< gum::Size >(0)));
    }
  };

  GUM_TEST_ACTIF(Constuctor)
  GUM_TEST_ACTIF(Read_file1)
  GUM_TEST_ACTIF(Read_file2_float)
  GUM_TEST_ACTIF(Read_dog_double)
  GUM_TEST_ACTIF(Read_file3)
  GUM_TEST_ACTIF(DogRead)
  GUM_TEST_ACTIF(AsiaRead)
}   // namespace gum_tests
