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
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/ID/influenceDiagram.h>
#include <agrum/ID/io/BIFXML/BIFXMLIDReader.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  BIFXMLIDReader
#define GUM_CURRENT_MODULE ID

// The graph used for the tests:
//           D1
//           |          D1 -> C1
//           C1
//          /  \        C1 -> C2 & C1 -> U1
//        C2   U1
//       /  \ /         C2 -> D3 & C2 -> D2 & D2 -> U1
//      D3  D2
//      |    |          D3 -> C3 & D2 -> C4
//     C3    C4
//       \  /           C3 -> C5 & C4 -> C5
//        C5   D4
//         \  /         C5 -> U2 & D4 -> U2
//          U2

namespace gum_tests {

  class aSimpleListener: public gum::Listener {
    private:
    int _nbr_;

    public:
    aSimpleListener() : _nbr_(0) {};

    void whenProceeding(const void* buffer, int percent, std::string status) {
      _nbr_ = percent;
      // std::cout << "Progress : " << percent << "%" << " Status : " << status
      // <<
      // std::endl;
    }

    int getNbr() { return _nbr_; };
  };

  struct BIFXMLIDReaderTestSuite {
    public:
    static void testConstuctor() {
      std::string file = GET_RESSOURCES_PATH("ID/IDBIFXMLIO_file.xml");

      gum::InfluenceDiagram< double > net;

      gum::BIFXMLIDReader< double >* reader = 0;

      GUM_CHECK_ASSERT_THROWS_NOTHING(reader = new gum::BIFXMLIDReader< double >(&net, file));

      GUM_CHECK_ASSERT_THROWS_NOTHING(delete reader);

      // delete reader;
    }   // namespace gum_tests

    static void testRead_file1() {
      std::string file = GET_RESSOURCES_PATH("ID/IDBIFXMLIO_file.xml");

      gum::InfluenceDiagram< double >* net = new gum::InfluenceDiagram< double >();

      gum::BIFXMLIDReader< double > reader(net, file);

      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      CHECK((net) != (nullptr));

      // For comparison with what have wrote Writer
      std::string   dotfile = GET_RESSOURCES_PATH("outputs/IDToDotReader.dot");
      std::ofstream output(dotfile.c_str(), std::ios::out | std::ios::trunc);

      if (!output.good()) { GUM_ERROR(gum::IOError, "Stream states flags are not all unset.") }

      output << net->toDot();
      output.flush();
      output.close();

      if (output.fail()) { GUM_ERROR(gum::IOError, "Writing in the ostream failed.") }

      if (net != nullptr) { CHECK(!net->empty()); }

      delete net;
    }

    static void testRead_file2_float() {
      std::string file = GET_RESSOURCES_PATH("ID/IDBIFXMLIO_file.xml");

      gum::InfluenceDiagram< double >* net = new gum::InfluenceDiagram< double >();

      gum::BIFXMLIDReader< double > reader(net, file);

      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      CHECK((net) != (nullptr));

      if (net != nullptr) {
        gum::HashTable< std::string, gum::NodeId > idMap;

        for (const auto node: net->nodes())
          idMap.insert(net->variable(node).name(), node);

        gum::NodeId decisionVar1Id = idMap["decisionVar1"];
        CHECK(net->isDecisionNode(decisionVar1Id));
        CHECK(!net->isUtilityNode(decisionVar1Id));
        CHECK(!net->isChanceNode(decisionVar1Id));
        const gum::DiscreteVariable& decisionVar1 = net->variable(decisionVar1Id);
        CHECK((decisionVar1.name()) == ("decisionVar1"));
        CHECK((decisionVar1.domainSize()) == (static_cast< gum::Size >(2)));
        CHECK((decisionVar1.label(0)) == ("0"));
        CHECK((decisionVar1.label(1)) == ("1"));

        gum::NodeId decisionVar2Id = idMap["decisionVar2"];
        CHECK(net->isDecisionNode(decisionVar2Id));
        CHECK(!net->isUtilityNode(decisionVar2Id));
        CHECK(!net->isChanceNode(decisionVar2Id));
        const gum::DiscreteVariable& decisionVar2 = net->variable(decisionVar2Id);
        CHECK((decisionVar2.name()) == ("decisionVar2"));
        CHECK((decisionVar2.domainSize()) == (static_cast< gum::Size >(2)));
        CHECK((decisionVar2.label(0)) == ("0"));
        CHECK((decisionVar2.label(1)) == ("1"));

        gum::NodeId decisionVar3Id = idMap["decisionVar3"];
        CHECK(net->isDecisionNode(decisionVar3Id));
        CHECK(!net->isUtilityNode(decisionVar3Id));
        CHECK(!net->isChanceNode(decisionVar3Id));
        const gum::DiscreteVariable& decisionVar3 = net->variable(decisionVar3Id);
        CHECK((decisionVar3.name()) == ("decisionVar3"));
        CHECK((decisionVar3.domainSize()) == (static_cast< gum::Size >(2)));
        CHECK((decisionVar3.label(0)) == ("0"));
        CHECK((decisionVar3.label(1)) == ("1"));

        gum::NodeId decisionVar4Id = idMap["decisionVar4"];
        CHECK(net->isDecisionNode(decisionVar4Id));
        CHECK(!net->isUtilityNode(decisionVar4Id));
        CHECK(!net->isChanceNode(decisionVar4Id));
        const gum::DiscreteVariable& decisionVar4 = net->variable(decisionVar4Id);
        CHECK((decisionVar4.name()) == ("decisionVar4"));
        CHECK((decisionVar4.domainSize()) == (static_cast< gum::Size >(2)));
        CHECK((decisionVar4.label(0)) == ("0"));
        CHECK((decisionVar4.label(1)) == ("1"));

        gum::NodeId chanceVar1Id = idMap["chanceVar1"];
        CHECK(!net->isDecisionNode(chanceVar1Id));
        CHECK(!net->isUtilityNode(chanceVar1Id));
        CHECK(net->isChanceNode(chanceVar1Id));
        const gum::DiscreteVariable& chanceVar1 = net->variable(chanceVar1Id);
        CHECK((chanceVar1.name()) == ("chanceVar1"));
        CHECK((chanceVar1.domainSize()) == (static_cast< gum::Size >(2)));
        CHECK((chanceVar1.label(0)) == ("0"));
        CHECK((chanceVar1.label(1)) == ("1"));

        gum::NodeId chanceVar2Id = idMap["chanceVar2"];
        CHECK(!net->isDecisionNode(chanceVar2Id));
        CHECK(!net->isUtilityNode(chanceVar2Id));
        CHECK(net->isChanceNode(chanceVar2Id));
        const gum::DiscreteVariable& chanceVar2 = net->variable(chanceVar2Id);
        CHECK((chanceVar2.name()) == ("chanceVar2"));
        CHECK((chanceVar2.domainSize()) == (static_cast< gum::Size >(2)));
        CHECK((chanceVar2.label(0)) == ("0"));
        CHECK((chanceVar2.label(1)) == ("1"));

        gum::NodeId chanceVar3Id = idMap["chanceVar3"];
        CHECK(!net->isDecisionNode(chanceVar3Id));
        CHECK(!net->isUtilityNode(chanceVar3Id));
        CHECK(net->isChanceNode(chanceVar3Id));
        const gum::DiscreteVariable& chanceVar3 = net->variable(chanceVar3Id);
        CHECK((chanceVar3.name()) == ("chanceVar3"));
        CHECK((chanceVar3.domainSize()) == (static_cast< gum::Size >(2)));
        CHECK((chanceVar3.label(0)) == ("0"));
        CHECK((chanceVar3.label(1)) == ("1"));

        gum::NodeId chanceVar4Id = idMap["chanceVar4"];
        CHECK(!net->isDecisionNode(chanceVar4Id));
        CHECK(!net->isUtilityNode(chanceVar4Id));
        CHECK(net->isChanceNode(chanceVar4Id));
        const gum::DiscreteVariable& chanceVar4 = net->variable(chanceVar4Id);
        CHECK((chanceVar4.name()) == ("chanceVar4"));
        CHECK((chanceVar4.domainSize()) == (static_cast< gum::Size >(2)));
        CHECK((chanceVar4.label(0)) == ("0"));
        CHECK((chanceVar4.label(1)) == ("1"));

        gum::NodeId chanceVar5Id = idMap["chanceVar5"];
        CHECK(!net->isDecisionNode(chanceVar5Id));
        CHECK(!net->isUtilityNode(chanceVar5Id));
        CHECK(net->isChanceNode(chanceVar5Id));
        const gum::DiscreteVariable& chanceVar5 = net->variable(chanceVar5Id);
        CHECK((chanceVar5.name()) == ("chanceVar5"));
        CHECK((chanceVar5.domainSize()) == (static_cast< gum::Size >(2)));
        CHECK((chanceVar5.label(0)) == ("0"));
        CHECK((chanceVar5.label(1)) == ("1"));

        gum::NodeId utilityVar1Id = idMap["utilityVar1"];
        CHECK(!net->isDecisionNode(utilityVar1Id));
        CHECK(net->isUtilityNode(utilityVar1Id));
        CHECK(!net->isChanceNode(utilityVar1Id));
        const gum::DiscreteVariable& utilityVar1 = net->variable(utilityVar1Id);
        CHECK((utilityVar1.name()) == ("utilityVar1"));
        CHECK((utilityVar1.domainSize()) == (static_cast< gum::Size >(1)));
        CHECK((utilityVar1.label(0)) == ("0"));

        gum::NodeId utilityVar2Id = idMap["utilityVar2"];
        CHECK(!net->isDecisionNode(utilityVar2Id));
        CHECK(net->isUtilityNode(utilityVar2Id));
        CHECK(!net->isChanceNode(utilityVar2Id));
        const gum::DiscreteVariable& utilityVar2 = net->variable(utilityVar2Id);
        CHECK((utilityVar2.name()) == ("utilityVar2"));
        CHECK((utilityVar2.domainSize()) == (static_cast< gum::Size >(1)));
        CHECK((utilityVar2.label(0)) == ("0"));

        const gum::Tensor< double >& cptChanceVar1 = net->cpt(idMap["chanceVar1"]);
        CHECK((cptChanceVar1.domainSize()) == (static_cast< gum::Size >(4)));
        gum::Instantiation instChanceVar1(cptChanceVar1);
        instChanceVar1.chgVal(decisionVar1, 0);
        instChanceVar1.chgVal(chanceVar1, 0);
        CHECK((cptChanceVar1[instChanceVar1]) == doctest::Approx(0.2).epsilon(0.001));
        instChanceVar1.chgVal(chanceVar1, 1);
        CHECK((cptChanceVar1[instChanceVar1]) == doctest::Approx(0.8).epsilon(0.001));
        instChanceVar1.chgVal(decisionVar1, 1);
        instChanceVar1.chgVal(chanceVar1, 0);
        CHECK((cptChanceVar1[instChanceVar1]) == doctest::Approx(0.5).epsilon(0.001));
        instChanceVar1.chgVal(chanceVar1, 1);
        CHECK((cptChanceVar1[instChanceVar1]) == doctest::Approx(0.5).epsilon(0.001));

        const gum::Tensor< double >& cptChanceVar2 = net->cpt(idMap["chanceVar2"]);
        CHECK((cptChanceVar2.domainSize()) == (static_cast< gum::Size >(4)));
        gum::Instantiation instChanceVar2(cptChanceVar2);
        instChanceVar2.chgVal(chanceVar1, 0);
        instChanceVar2.chgVal(chanceVar2, 0);
        CHECK((cptChanceVar2[instChanceVar2]) == doctest::Approx(0.1).epsilon(0.001));
        instChanceVar2.chgVal(chanceVar2, 1);
        CHECK((cptChanceVar2[instChanceVar2]) == doctest::Approx(0.9).epsilon(0.001));
        instChanceVar2.chgVal(chanceVar1, 1);
        instChanceVar2.chgVal(chanceVar2, 0);
        CHECK((cptChanceVar2[instChanceVar2]) == doctest::Approx(0.9).epsilon(0.001));
        instChanceVar2.chgVal(chanceVar2, 1);
        CHECK((cptChanceVar2[instChanceVar2]) == doctest::Approx(0.1).epsilon(0.001));

        const gum::Tensor< double >& cptChanceVar3 = net->cpt(idMap["chanceVar3"]);
        CHECK((cptChanceVar3.domainSize()) == (static_cast< gum::Size >(4)));
        gum::Instantiation instChanceVar3(cptChanceVar3);
        instChanceVar3.chgVal(decisionVar3, 0);
        instChanceVar3.chgVal(chanceVar3, 0);
        CHECK((cptChanceVar3[instChanceVar3]) == doctest::Approx(0.35).epsilon(0.001));
        instChanceVar3.chgVal(chanceVar3, 1);
        CHECK((cptChanceVar3[instChanceVar3]) == doctest::Approx(0.65).epsilon(0.001));
        instChanceVar3.chgVal(decisionVar3, 1);
        instChanceVar3.chgVal(chanceVar3, 0);
        CHECK((cptChanceVar3[instChanceVar3]) == doctest::Approx(0.19).epsilon(0.001));
        instChanceVar3.chgVal(chanceVar3, 1);
        CHECK((cptChanceVar3[instChanceVar3]) == doctest::Approx(0.81).epsilon(0.001));

        const gum::Tensor< double >& cptChanceVar4 = net->cpt(idMap["chanceVar4"]);
        CHECK((cptChanceVar4.domainSize()) == (static_cast< gum::Size >(4)));
        gum::Instantiation instChanceVar4(cptChanceVar4);
        instChanceVar4.chgVal(decisionVar2, 0);
        instChanceVar4.chgVal(chanceVar4, 0);
        CHECK((cptChanceVar4[instChanceVar4]) == doctest::Approx(0.4).epsilon(0.001));
        instChanceVar4.chgVal(chanceVar4, 1);
        CHECK((cptChanceVar4[instChanceVar4]) == doctest::Approx(0.6).epsilon(0.001));
        instChanceVar4.chgVal(decisionVar2, 1);
        instChanceVar4.chgVal(chanceVar4, 0);
        CHECK((cptChanceVar4[instChanceVar4]) == doctest::Approx(0.5).epsilon(0.001));
        instChanceVar4.chgVal(chanceVar4, 1);
        CHECK((cptChanceVar4[instChanceVar4]) == doctest::Approx(0.5).epsilon(0.001));

        const gum::Tensor< double >& cptChanceVar5 = net->cpt(idMap["chanceVar5"]);
        CHECK((cptChanceVar5.domainSize()) == (static_cast< gum::Size >(8)));
        gum::Instantiation instChanceVar5(cptChanceVar5);
        instChanceVar5.chgVal(chanceVar4, 0);
        instChanceVar5.chgVal(chanceVar3, 0);
        instChanceVar5.chgVal(chanceVar5, 0);
        CHECK((cptChanceVar5[instChanceVar5]) == doctest::Approx(0.4).epsilon(0.001));
        instChanceVar5.chgVal(chanceVar5, 1);
        CHECK((cptChanceVar5[instChanceVar5]) == doctest::Approx(0.6).epsilon(0.001));
        instChanceVar5.chgVal(chanceVar3, 1);
        instChanceVar5.chgVal(chanceVar5, 0);
        CHECK((cptChanceVar5[instChanceVar5]) == doctest::Approx(0.8).epsilon(0.001));
        instChanceVar5.chgVal(chanceVar5, 1);
        CHECK((cptChanceVar5[instChanceVar5]) == doctest::Approx(0.2).epsilon(0.001));
        instChanceVar5.chgVal(chanceVar4, 1);
        instChanceVar5.chgVal(chanceVar3, 0);
        instChanceVar5.chgVal(chanceVar5, 0);
        CHECK((cptChanceVar5[instChanceVar5]) == doctest::Approx(0.4).epsilon(0.001));
        instChanceVar5.chgVal(chanceVar5, 1);
        CHECK((cptChanceVar5[instChanceVar5]) == doctest::Approx(0.6).epsilon(0.001));
        instChanceVar5.chgVal(chanceVar3, 1);
        instChanceVar5.chgVal(chanceVar5, 0);
        CHECK((cptChanceVar5[instChanceVar5]) == doctest::Approx(0.3).epsilon(0.001));
        instChanceVar5.chgVal(chanceVar5, 1);
        CHECK((cptChanceVar5[instChanceVar5]) == doctest::Approx(0.7).epsilon(0.001));

        const gum::Tensor< double >& utUtilityVar1 = net->utility(idMap["utilityVar1"]);
        CHECK((utUtilityVar1.domainSize()) == (static_cast< gum::Size >(4)));
        gum::Instantiation instUtilityVar1(utUtilityVar1);
        instUtilityVar1.chgVal(utilityVar1, 0);
        instUtilityVar1.chgVal(decisionVar2, 0);
        instUtilityVar1.chgVal(chanceVar1, 0);
        CHECK((utUtilityVar1[instUtilityVar1]) == doctest::Approx(42).epsilon(0.001));
        instUtilityVar1.chgVal(chanceVar1, 1);
        CHECK((utUtilityVar1[instUtilityVar1]) == doctest::Approx(666).epsilon(0.001));
        instUtilityVar1.chgVal(decisionVar2, 1);
        instUtilityVar1.chgVal(chanceVar1, 0);
        CHECK((utUtilityVar1[instUtilityVar1]) == doctest::Approx(69).epsilon(0.001));
        instUtilityVar1.chgVal(chanceVar1, 1);
        CHECK((utUtilityVar1[instUtilityVar1]) == doctest::Approx(84).epsilon(0.001));

        const gum::Tensor< double >& utUtilityVar2 = net->utility(idMap["utilityVar2"]);
        CHECK((utUtilityVar2.domainSize()) == (static_cast< gum::Size >(4)));
        gum::Instantiation instUtilityVar2(utUtilityVar2);
        instUtilityVar2.chgVal(utilityVar2, 0);
        instUtilityVar2.chgVal(decisionVar4, 0);
        instUtilityVar2.chgVal(chanceVar5, 0);
        CHECK((utUtilityVar2[instUtilityVar2]) == doctest::Approx(42).epsilon(0.001));
        instUtilityVar2.chgVal(chanceVar5, 1);
        CHECK((utUtilityVar2[instUtilityVar2]) == doctest::Approx(-69).epsilon(0.001));
        instUtilityVar2.chgVal(decisionVar4, 1);
        instUtilityVar2.chgVal(chanceVar5, 0);
        CHECK((utUtilityVar2[instUtilityVar2]) == doctest::Approx(666).epsilon(0.001));
        instUtilityVar2.chgVal(chanceVar5, 1);
        CHECK((utUtilityVar2[instUtilityVar2]) == doctest::Approx(84).epsilon(0.001));
      }

      delete net;
    }

    static void testRead_file3_float() {
      std::string file = GET_RESSOURCES_PATH("ID/IDBIFXMLIO_file.xml");

      gum::InfluenceDiagram< double >* net = new gum::InfluenceDiagram< double >();

      gum::BIFXMLIDReader< double > reader(net, file);

      aSimpleListener asl;
      GUM_CONNECT(reader, onProceed, asl, aSimpleListener::whenProceeding);

      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      CHECK((asl.getNbr()) == (100));
      delete net;
    }
  };

  GUM_TEST_ACTIF(Constuctor)
  GUM_TEST_ACTIF(Read_file1)
  GUM_TEST_ACTIF(Read_file2_float)
  GUM_TEST_ACTIF(Read_file3_float)
}   // namespace gum_tests
