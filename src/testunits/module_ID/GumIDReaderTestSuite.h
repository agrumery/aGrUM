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

#include <agrum/ID/influenceDiagram.h>
#include <agrum/ID/io/GUM/GumIDReader.h>
#include <agrum/ID/io/GUM/GumIDWriter.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {
  struct GumIDReaderTestSuite {};

  GUM_TEST(BuildingIDFromJson) {
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
    // CHECK_EQ(id.property("software"), "aGrUM test");
    CHECK(id.property("software").starts_with("aGrUM"));
  }

  GUM_TEST(InvalidJsonString) {
    gum::InfluenceDiagram< double > id;
    auto                            reader = gum::GumIDReader< double >(&id);
    CHECK_NE(reader.proceedFromString("not valid json at all"), 0u);
    CHECK_EQ(reader.count(), 1u);
    CHECK_NE(reader.error(0).msg.find("Invalid JSON"), std::string::npos);
  }

  GUM_TEST(FileNotFound) {
    gum::InfluenceDiagram< double > id;
    auto                            reader = gum::GumIDReader< double >(&id, "/no/such/file.jgum");
    CHECK_NE(reader.proceed(), 0u);
    CHECK_EQ(reader.count(), 1u);
    CHECK_NE(reader.error(0).msg.find("No such file"), std::string::npos);
  }

  GUM_TEST(BinaryRoundtrip) {
    constexpr auto                  path = GET_RESSOURCES_PATH("outputs/reader_id.bgum");
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

  GUM_TEST(WrongModelType) {
    const std::string               filename = GET_RESSOURCES_PATH("jsonGum/minimal.mrf.jgum");
    gum::InfluenceDiagram< double > id;
    auto                            reader = gum::GumIDReader< double >(&id, filename);
    CHECK_NE(reader.proceed(), 0u);
    CHECK_EQ(reader.count(), 1u);
    CHECK_NE(reader.error(0).msg.find("expected 'ID'"), std::string::npos);
    CHECK_NE(reader.error(0).msg.find("got 'MRF'"), std::string::npos);
  }

  GUM_TEST(GumJsonVersion) {
    // the same major version is read (whatever the minor one), another major version is refused
    const auto id = gum::InfluenceDiagram< double >::fastPrototype("A->*D->$U;A->$U");
    gum::GumIDWriter< double > writer(false, 2);
    const std::string          str         = writer.toString(id);
    const auto                 withVersion = [&str](const std::string& version) {
      std::string s     = str;
      const auto  key   = s.find("\"GumJsonVersion\"");
      const auto  start = s.find('"', s.find(':', key)) + 1;
      return s.replace(start, s.find('"', start) - start, version);
    };
    REQUIRE_NE(str.find("\"GumJsonVersion\""), std::string::npos);
    for (const auto& version: {"1.0", "1.9", "1"}) {
      gum::InfluenceDiagram< double > model;
      gum::GumIDReader< double >      reader(&model);
      CHECK_EQ(reader.proceedFromString(withVersion(version)), 0u);
    }
    gum::InfluenceDiagram< double > model;
    gum::GumIDReader< double >      reader(&model);
    CHECK_NE(reader.proceedFromString(withVersion("2.0")), 0u);
    REQUIRE_EQ(reader.count(), 1u);
    CHECK_NE(reader.error(0).msg.find("Unsupported GumJsonVersion '2.0'"), std::string::npos);
  }

  GUM_TEST(KeyWrittenTwiceIsRefused) {
    // the JSON library keeps the last value of a key written twice: the reader refuses it
    auto id = gum::InfluenceDiagram< double >::fastPrototype("A->*D->$U;A->$U");
    gum::GumIDWriter< double > writer(false, 2);
    const std::string          str = writer.toString(id);
    REQUIRE_EQ(str[0], '{');
    gum::InfluenceDiagram< double > id2;
    gum::GumIDReader< double >      reader(&id2);
    CHECK_NE(reader.proceedFromString("{\"type\": \"ID\", " + str.substr(1)), 0u);
    REQUIRE_EQ(reader.count(), 1u);
    CHECK_NE(reader.error(0).msg.find("written twice"), std::string::npos);
  }

  GUM_TEST(ProceedWithoutFilename) {
    gum::InfluenceDiagram< double > id;
    auto                            reader = gum::GumIDReader< double >(&id);
    CHECK_THROWS_AS(reader.proceed(), const gum::OperationNotAllowed&);
  }

  GUM_TEST(ProceedFromString) {
    // A -> C -> *D -> $U, A -> $U  (chance A, chance C, decision D, utility U)
    auto id = gum::InfluenceDiagram< double >::fastPrototype("A->C->*D->$U;A->$U");
    gum::GumIDWriter< double > writer(false, 2);
    const std::string          str = writer.toString(id);

    gum::InfluenceDiagram< double > id2;
    gum::GumIDReader< double >      reader(&id2);
    CHECK_EQ(reader.proceedFromString(str), 0u);
    CHECK_EQ(id2, id);
  }

  GUM_TEST(JgumPreservesStateOnMidParseError) {
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

  GUM_TEST(BgumPreservesStateOnCorruptBytes) {
    const TemporaryFile             file(".bgum");   // removed at the end of the test
    const auto&                     path = file.path;
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

  GUM_TEST(BgumRefusesAWrongLength) {
    const TemporaryFile file(".bgum");   // removed at the end of the test
    const auto&         path = file.path;
    // a truncated length field (4 bytes instead of 8), a length far above the size of the file,
    // a length above the size of the payload (truncated data)
    const std::string contents[] = {std::string(4, '\0'),
                                    std::string(8, '\xFF'),
                                    std::string("\x64\0\0\0\0\0\0\0", 8) + std::string(10, 'x')};
    for (const auto& content: contents) {
      {
        std::ofstream out(path, std::ios::binary);
        out << content;
      }
      gum::InfluenceDiagram< double > id;
      id.addChanceNode("existing[2]");
      auto reader = gum::GumIDReader< double >(&id, path, true);
      CHECK_NE(reader.proceed(), 0u);
      CHECK_EQ(reader.count(), 1u);
      CHECK_EQ(id.size(), 1u);
      CHECK_NOTHROW(id.idFromName("existing"));
    }
  }
}   // namespace gum_tests
