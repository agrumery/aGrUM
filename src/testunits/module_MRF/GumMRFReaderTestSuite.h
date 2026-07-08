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

#include <sstream>
#include <string>

#include <agrum/base/io/GumBinaryIO.h>
#include <agrum/MRF/io/GUM/GumMRFReader.h>
#include <agrum/MRF/io/GUM/GumMRFWriter.h>
#include <agrum/MRF/MarkovRandomField.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  GumMRFReader
#define GUM_CURRENT_MODULE MRF

namespace gum_tests {
  struct GumMRFReaderTestSuite {
    public:
    static void testBuildingMRFFromJson() {
      // Read minimal.mrf.jgum: nodes A[2], B[2], C[2], factors {A,B} and {B,C}
      const std::string                filename = GET_RESSOURCES_PATH("jsonGum/minimal.mrf.jgum");
      gum::MarkovRandomField< double > mrf;
      auto                             reader = gum::GumMRFReader< double >(&mrf, filename);
      CHECK_EQ(reader.proceed(), 0u);

      CHECK_EQ(mrf.size(), 3u);
      CHECK_EQ(mrf.factors().size(), 2u);

      // verify nodes exist (idFromName throws if not found)
      CHECK_NOTHROW(mrf.idFromName("A"));
      CHECK_NOTHROW(mrf.idFromName("B"));
      CHECK_NOTHROW(mrf.idFromName("C"));

      CHECK_EQ(mrf.variable("A").domainSize(), 2u);
      CHECK_EQ(mrf.variable("B").domainSize(), 2u);
      CHECK_EQ(mrf.variable("C").domainSize(), 2u);

      CHECK(mrf.existsEdge(mrf.idFromName("A"), mrf.idFromName("B")));
      CHECK(mrf.existsEdge(mrf.idFromName("B"), mrf.idFromName("C")));

      // factor {A,B}: [0.5, 0.2, 0.3, 0.8]
      const auto& factorAB = mrf.factor({"A", "B"});
      CHECK_EQ(factorAB.domainSize(), 4u);

      // factor {B,C}: [0.9, 0.1, 0.4, 0.6]
      const auto& factorBC = mrf.factor({"B", "C"});
      CHECK_EQ(factorBC.domainSize(), 4u);

      // properties from file
      CHECK(mrf.existsProperty("software"));
      CHECK_EQ(mrf.property("software"), "aGrUM test");
    }

    static void testInvalidJsonString() {
      gum::MarkovRandomField< double > mrf;
      auto                             reader = gum::GumMRFReader< double >(&mrf);
      CHECK_NE(reader.proceedFromString("not valid json at all"), 0u);
      CHECK_EQ(reader.count(), 1u);
      CHECK_NE(reader.error(0).msg.find("Invalid JSON"), std::string::npos);
    }

    static void testFileNotFound() {
      gum::MarkovRandomField< double > mrf;
      auto reader = gum::GumMRFReader< double >(&mrf, "/no/such/file.jgum");
      CHECK_NE(reader.proceed(), 0u);
      CHECK_EQ(reader.count(), 1u);
      CHECK_NE(reader.error(0).msg.find("No such file"), std::string::npos);
    }

    static void testBinaryRoundtrip() {
      const auto                       path = GET_RESSOURCES_PATH("outputs/reader_mrf.bgum");
      gum::MarkovRandomField< double > mrf;
      {
        auto r = gum::GumMRFReader< double >(&mrf, GET_RESSOURCES_PATH("jsonGum/minimal.mrf.jgum"));
        CHECK_EQ(r.proceed(), 0u);
      }
      gum::GumMRFWriter< double > writer(true, 2);
      writer.write(path, mrf);
      gum::MarkovRandomField< double > mrf2;
      auto                             reader = gum::GumMRFReader< double >(&mrf2, path, true);
      CHECK_EQ(reader.proceed(), 0u);
      CHECK_EQ(mrf, mrf2);
    }

    static void testWrongModelType() {
      const std::string                filename = GET_RESSOURCES_PATH("jsonGum/minimal.id.jgum");
      gum::MarkovRandomField< double > mrf;
      auto                             reader = gum::GumMRFReader< double >(&mrf, filename);
      CHECK_NE(reader.proceed(), 0u);
      CHECK_EQ(reader.count(), 1u);
      CHECK_NE(reader.error(0).msg.find("expected 'MRF'"), std::string::npos);
      CHECK_NE(reader.error(0).msg.find("got 'ID'"), std::string::npos);
    }

    static void testProceedWithoutFilename() {
      gum::MarkovRandomField< double > mrf;
      auto                             reader = gum::GumMRFReader< double >(&mrf);
      CHECK_THROWS_AS(reader.proceed(), const gum::OperationNotAllowed&);
    }

    static void testProceedFromString() {
      gum::MarkovRandomField< double > mrf;
      mrf.add("A[2]");
      mrf.add("B[2]");
      mrf.add("C[2]");
      mrf.addFactor({"A", "B"});
      mrf.addFactor({"B", "C"});
      mrf.factor({"A", "B"}).fillWith({0.5, 0.2, 0.3, 0.8});
      mrf.factor({"B", "C"}).fillWith({0.9, 0.1, 0.4, 0.6});

      gum::GumMRFWriter< double > writer(false, 2);
      const std::string           str = writer.toString(mrf);

      gum::MarkovRandomField< double > mrf2;
      auto                             reader = gum::GumMRFReader< double >(&mrf2);
      CHECK_EQ(reader.proceedFromString(str), 0u);
      CHECK_EQ(mrf2, mrf);
    }

    static void testJgumPreservesStateOnMidParseError() {
      gum::MarkovRandomField< double > mrf;
      mrf.add("existing[2]");

      // Valid JSON, correct type, but factor values count is wrong (1 instead of 4)
      const std::string bad    = R"({
        "type": "MRF",
        "nodes": ["X[2]", "Y[2]"],
        "factors": [{"vars": ["X", "Y"], "values": [0.5, 0.2, 0.3]}]
      })";
      auto              reader = gum::GumMRFReader< double >(&mrf);
      CHECK_NE(reader.proceedFromString(bad), 0u);
      CHECK_EQ(mrf.size(), 1u);
      CHECK(mrf.exists("existing"));
    }

    static void testBgumPreservesStateOnCorruptBytes() {
      const auto                       path = GET_RESSOURCES_PATH("outputs/corrupt_mrf.bgum");
      gum::MarkovRandomField< double > mrf;
      mrf.add("existing[2]");

      {
        std::ofstream out(path, std::ios::binary);
        uint64_t      size = 10;
        out.write(reinterpret_cast< const char* >(&size), sizeof(size));
        out.write("GARBAGE!!!", 10);
      }
      auto reader = gum::GumMRFReader< double >(&mrf, path, true);
      CHECK_NE(reader.proceed(), 0u);
      CHECK_EQ(reader.count(), 1u);
      CHECK_EQ(mrf.size(), 1u);
      CHECK(mrf.exists("existing"));
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

  GUM_TEST_ACTIF(BuildingMRFFromJson)
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
