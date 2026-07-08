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

#include <iostream>
#include <sstream>
#include <string>

#include <agrum/base/io/GumBinaryIO.h>
#include <agrum/ID/influenceDiagram.h>
#include <agrum/ID/io/GUM/GumIDReader.h>
#include <agrum/ID/io/GUM/GumIDWriter.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  GumIDReader
#define GUM_CURRENT_MODULE ID

namespace gum_tests {
  struct GumIDReaderTestSuite {
    public:
    static void testBuildingIDFromJson() {
      // Read minimal.id.jgum which has: chance A[2], C[2]; utility U[1]; decision D[2]
      // parents: A->{}, C->{A}, D->{}, U->{C,D}
      const std::string               filename = GET_RESSOURCES_PATH("jsonGum/minimal.id.jgum");
      gum::InfluenceDiagram< double > id;
      gum::GumIDReader< double >      reader(&id, filename);
      reader.proceed();
      CHECK_EQ(reader.count(), 0u);

      CHECK_EQ(id.size(), 4u);
      CHECK_EQ(id.chanceNodeSize(), 2u);
      CHECK_EQ(id.utilityNodeSize(), 1u);
      CHECK_EQ(id.decisionNodeSize(), 1u);

      CHECK(id.isChanceNode(id.idFromName("A")));
      CHECK(id.isChanceNode(id.idFromName("C")));
      CHECK(id.isUtilityNode(id.idFromName("U")));
      CHECK(id.isDecisionNode(id.idFromName("D")));

      CHECK_EQ(id.variable("A").domainSize(), 2u);
      CHECK_EQ(id.variable("C").domainSize(), 2u);
      CHECK_EQ(id.variable("D").domainSize(), 2u);
      CHECK_EQ(id.variable("U").domainSize(), 1u);

      CHECK_EQ(id.parents(id.idFromName("A")).size(), 0u);
      CHECK_EQ(id.parents(id.idFromName("D")).size(), 0u);
      CHECK(id.existsArc("A", "C"));
      CHECK(id.existsArc("C", "U"));
      CHECK(id.existsArc("D", "U"));

      // CPT values for A: [0.4, 0.6]
      const auto& cptA = id.cpt("A");
      CHECK_EQ(cptA.domainSize(), 2u);
      gum::Instantiation IA(cptA);
      IA.setFirst();
      CHECK_EQ(cptA[IA], doctest::Approx(0.4));
      ++IA;
      CHECK_EQ(cptA[IA], doctest::Approx(0.6));

      // utility values for U: [10.0, -5.0, 20.0, 5.0]
      const auto& utU = id.utility("U");
      CHECK_EQ(utU.domainSize(), 4u);
      gum::Instantiation IU(utU);
      IU.setFirst();
      CHECK_EQ(utU[IU], doctest::Approx(10.0));

      // properties from file
      CHECK(id.existsProperty("software"));
      CHECK_EQ(id.property("software"), "aGrUM test");
    }

    static void testInvalidJsonString() {
      gum::InfluenceDiagram< double > id;
      auto                            reader = gum::GumIDReader< double >(&id);
      CHECK_NE(reader.proceedFromString("not valid json at all"), 0u);
      CHECK_EQ(reader.count(), 1u);
      CHECK_NE(reader.error(0).msg.find("Invalid JSON"), std::string::npos);
    }

    static void testFileNotFound() {
      gum::InfluenceDiagram< double > id;
      auto reader = gum::GumIDReader< double >(&id, "/no/such/file.jgum");
      CHECK_NE(reader.proceed(), 0u);
      CHECK_EQ(reader.count(), 1u);
      CHECK_NE(reader.error(0).msg.find("No such file"), std::string::npos);
    }

    static void testBinaryRoundtrip() {
      const auto                      path = GET_RESSOURCES_PATH("outputs/reader_id.bgum");
      gum::InfluenceDiagram< double > id;
      {
        auto r = gum::GumIDReader< double >(&id, GET_RESSOURCES_PATH("jsonGum/minimal.id.jgum"));
        CHECK_EQ(r.proceed(), 0u);
      }
      gum::GumIDWriter< double > writer(true, 2);
      writer.write(path, id);
      gum::InfluenceDiagram< double > id2;
      auto                            reader = gum::GumIDReader< double >(&id2, path, true);
      CHECK_EQ(reader.proceed(), 0u);
      CHECK_EQ(id, id2);
    }

    static void testWrongModelType() {
      const std::string               filename = GET_RESSOURCES_PATH("jsonGum/minimal.mrf.jgum");
      gum::InfluenceDiagram< double > id;
      auto                            reader = gum::GumIDReader< double >(&id, filename);
      CHECK_NE(reader.proceed(), 0u);
      CHECK_EQ(reader.count(), 1u);
      CHECK_NE(reader.error(0).msg.find("expected 'ID'"), std::string::npos);
      CHECK_NE(reader.error(0).msg.find("got 'MRF'"), std::string::npos);
    }

    static void testProceedWithoutFilename() {
      gum::InfluenceDiagram< double > id;
      auto                            reader = gum::GumIDReader< double >(&id);
      CHECK_THROWS_AS(reader.proceed(), const gum::OperationNotAllowed&);
    }

    static void testProceedFromString() {
      // A -> C -> *D -> $U, A -> $U  (chance A, chance C, decision D, utility U)
      auto id = gum::InfluenceDiagram< double >::fastPrototype("A->C->*D->$U;A->$U");
      gum::GumIDWriter< double > writer(false, 2);
      const std::string          str = writer.toString(id);

      gum::InfluenceDiagram< double > id2;
      gum::GumIDReader< double >      reader(&id2);
      CHECK_EQ(reader.proceedFromString(str), 0u);
      CHECK_EQ(id2, id);
    }

    static void testJgumPreservesStateOnMidParseError() {
      gum::InfluenceDiagram< double > id;
      id.addChanceNode("existing[2]");

      // Valid JSON, correct type, but reward for U has 1 value instead of 4
      const std::string bad    = R"({
        "type": "ID",
        "chance": ["A[2]"],
        "utility": ["U[1]"],
        "decision": ["D[2]"],
        "parents": {"A": [], "D": [], "U": ["A", "D"]},
        "cpt": {"A": [0.4, 0.6]},
        "reward": {"U": [1.0, 2.0, 3.0]}
      })";
      auto              reader = gum::GumIDReader< double >(&id);
      CHECK_NE(reader.proceedFromString(bad), 0u);
      CHECK_EQ(id.size(), 1u);
      CHECK_NOTHROW(id.idFromName("existing"));
    }

    static void testBgumPreservesStateOnCorruptBytes() {
      const auto                      path = GET_RESSOURCES_PATH("outputs/corrupt_id.bgum");
      gum::InfluenceDiagram< double > id;
      id.addChanceNode("existing[2]");

      {
        std::ofstream out(path, std::ios::binary);
        uint64_t      size = 10;
        out.write(reinterpret_cast< const char* >(&size), sizeof(size));
        out.write("GARBAGE!!!", 10);
      }
      auto reader = gum::GumIDReader< double >(&id, path, true);
      CHECK_NE(reader.proceed(), 0u);
      CHECK_EQ(reader.count(), 1u);
      CHECK_EQ(id.size(), 1u);
      CHECK_NOTHROW(id.idFromName("existing"));
    }

    // regression tests for CRIT-16: _readVector_ size validation
    static void testReadVectorTruncatedSizeField() {
      // Only 4 bytes instead of 8 for the size field
      std::istringstream iss(std::string(4, '\0'));
      CHECK_THROWS(gum::_readVector_(iss));
    }

    static void testReadVectorOversizedField() {
      // Size = UINT64_MAX → far above 256 MB guard
      std::ostringstream oss;
      uint64_t           huge = 0xFFFFFFFFFFFFFFFFULL;
      oss.write(reinterpret_cast< const char* >(&huge), sizeof(huge));
      std::istringstream iss(oss.str());
      CHECK_THROWS(gum::_readVector_(iss));
    }

    static void testReadVectorTruncatedData() {
      // Announce 100 bytes but only write 10
      std::ostringstream oss;
      uint64_t           size = 100;
      oss.write(reinterpret_cast< const char* >(&size), sizeof(size));
      oss.write(std::string(10, 'x').c_str(), 10);
      std::istringstream iss(oss.str());
      CHECK_THROWS(gum::_readVector_(iss));
    }
  };

  GUM_TEST_ACTIF(BuildingIDFromJson)
  GUM_TEST_ACTIF(InvalidJsonString)
  GUM_TEST_ACTIF(FileNotFound)
  GUM_TEST_ACTIF(BinaryRoundtrip)
  GUM_TEST_ACTIF(WrongModelType)
  GUM_TEST_ACTIF(ProceedWithoutFilename)
  GUM_TEST_ACTIF(ProceedFromString)
  GUM_TEST_ACTIF(JgumPreservesStateOnMidParseError)
  GUM_TEST_ACTIF(BgumPreservesStateOnCorruptBytes)
  GUM_TEST_ACTIF(ReadVectorTruncatedSizeField)
  GUM_TEST_ACTIF(ReadVectorOversizedField)
  GUM_TEST_ACTIF(ReadVectorTruncatedData)
}   // namespace gum_tests
