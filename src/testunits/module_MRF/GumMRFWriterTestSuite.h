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
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include <agrum/MRF/io/GUM/GumMRFReader.h>
#include <agrum/MRF/io/GUM/GumMRFWriter.h>
#include <agrum/MRF/MarkovRandomField.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  GumMRFWriter
#define GUM_CURRENT_MODULE MRF

namespace gum_tests {
  struct GumMRFWriterTestSuite {
    private:
    static gum::MarkovRandomField< double > _buildSimpleMRF_() {
      gum::MarkovRandomField< double > mrf;
      mrf.add("A[2]");
      mrf.add("B[2]");
      mrf.add("C[2]");
      mrf.addFactor({"A", "B"});
      mrf.addFactor({"B", "C"});
      mrf.factor({"A", "B"}).fillWith({0.5, 0.2, 0.3, 0.8});
      mrf.factor({"B", "C"}).fillWith({0.9, 0.1, 0.4, 0.6});
      return mrf;
    }

    static void _simpleTestForWriter_(bool isbinary) {
      auto       mrf  = _buildSimpleMRF_();
      const auto path = isbinary ? GET_RESSOURCES_PATH("outputs/test.bmrf")
                                 : GET_RESSOURCES_PATH("outputs/test.jmrf");

      GUM_CHECK_ASSERT_THROWS_NOTHING({
        gum::GumMRFWriter< double > writer(isbinary);
        writer.write(path, mrf);
      });

      gum::MarkovRandomField< double > mrf2;
      auto                             reader = gum::GumMRFReader< double >(&mrf2, path, isbinary);
      CHECK_EQ(reader.proceed(), 0u);
      CHECK_EQ(mrf2, mrf);
    }

    static void _checkMetaData_(bool isbinary) {
      auto       mrf  = _buildSimpleMRF_();
      const auto path = isbinary ? GET_RESSOURCES_PATH("outputs/test.bmrf")
                                 : GET_RESSOURCES_PATH("outputs/test.jmrf");

      CHECK_NOTHROW({
        gum::GumMRFWriter< double > writer(isbinary, 2);
        writer.write(path, mrf);
      });

      {
        gum::MarkovRandomField< double > mrf2;
        auto reader = gum::GumMRFReader< double >(&mrf2, path, isbinary);
        CHECK_EQ(reader.proceed(), 0u);
        CHECK_EQ(mrf2, mrf);

        CHECK(mrf2.existsProperty("software"));
        CHECK_EQ(mrf2.property("software"), "aGrUM " GUM_VERSION);
        CHECK(mrf2.existsProperty("creation"));
        CHECK(mrf2.existsProperty("lastModification"));
        CHECK_EQ(mrf2.property("creation"), mrf2.property("lastModification"));
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(500));

      {
        CHECK_NOTHROW({
          gum::GumMRFWriter< double > writer(isbinary, 2);
          writer.write(path, mrf);
        });

        gum::MarkovRandomField< double > mrf2;
        auto reader = gum::GumMRFReader< double >(&mrf2, path, isbinary);
        CHECK_EQ(reader.proceed(), 0u);
        CHECK_EQ(mrf2, mrf);

        CHECK(mrf2.existsProperty("software"));
        CHECK_EQ(mrf2.property("software"), "aGrUM " GUM_VERSION);
        CHECK(mrf2.existsProperty("creation"));
        CHECK(mrf2.existsProperty("lastModification"));
        CHECK_NE(mrf2.property("creation"), mrf2.property("lastModification"));
      }
    }

    public:
    static void testSimpleTestForWriter() {
      _simpleTestForWriter_(false);
      _simpleTestForWriter_(true);
    }

    static void testCheckMetaData() {
      _checkMetaData_(false);
      _checkMetaData_(true);
    }

    static void testBinaryFileIntegrity() {
      // Verify the binary file layout: [8-byte LE uint64 payload size][payload].
      // If the file is opened in text mode on Windows, the CRLF translation corrupts
      // the binary payload and the size prefix no longer matches the actual data.
      auto       mrf  = _buildSimpleMRF_();
      const auto path = GET_RESSOURCES_PATH("outputs/test_integrity.bmrf");

      gum::GumMRFWriter< double > writer(true);
      writer.write(path, mrf);

      std::ifstream file(path, std::ios::binary | std::ios::ate);
      CHECK(file.is_open());
      const auto fileSize = static_cast< uint64_t >(file.tellg());
      file.seekg(0);
      uint64_t payloadSize = 0;
      file.read(reinterpret_cast< char* >(&payloadSize), sizeof(payloadSize));
      file.close();

      // Exactly 8 bytes of prefix + payloadSize bytes of msgpack data
      CHECK_EQ(fileSize, payloadSize + 8u);
    }

    static void testNoFactors() {
      // MRF with nodes but zero factors — "factors" section must still exist in JSON
      gum::MarkovRandomField< double > mrf;
      mrf.add("A[2]");
      mrf.add("B[2]");

      gum::GumMRFWriter< double > writer(false, 2);
      const std::string           str = writer.toString(mrf);

      gum::MarkovRandomField< double > mrf2;
      auto                             reader = gum::GumMRFReader< double >(&mrf2);
      CHECK_EQ(reader.proceedFromString(str), 0u);
      CHECK_EQ(mrf2.size(), 2u);
      CHECK_EQ(mrf2.factors().size(), 0u);
    }

    static void testSingleVariable() {
      // MRF with a single node and a unary factor — no edges
      gum::MarkovRandomField< double > mrf;
      mrf.add("A[2]");
      mrf.addFactor({"A"});
      mrf.factor({"A"}).fillWith({0.3, 0.7});

      gum::GumMRFWriter< double > writer(false, 2);
      const std::string           str = writer.toString(mrf);

      gum::MarkovRandomField< double > mrf2;
      auto                             reader = gum::GumMRFReader< double >(&mrf2);
      CHECK_EQ(reader.proceedFromString(str), 0u);
      CHECK_EQ(mrf2, mrf);
      CHECK_EQ(mrf2.size(), 1u);
    }

    static void testToString() {
      auto                        mrf = _buildSimpleMRF_();
      gum::GumMRFWriter< double > writer(false, 2);
      std::string                 str = writer.toString(mrf);

      std::string   tempFileName = std::tmpnam(nullptr);
      std::ofstream tempFile(tempFileName.c_str(), std::ios_base::trunc);
      tempFile << str;
      tempFile.close();

      gum::MarkovRandomField< double > mrf2;
      auto reader = gum::GumMRFReader< double >(&mrf2, tempFileName, false);
      CHECK_EQ(reader.proceed(), 0u);
      CHECK_EQ(mrf2, mrf);
    }
  };

  GUM_TEST_ACTIF(SimpleTestForWriter)
  GUM_TEST_ACTIF(CheckMetaData)
  GUM_TEST_ACTIF(BinaryFileIntegrity)
  GUM_TEST_ACTIF(NoFactors)
  GUM_TEST_ACTIF(SingleVariable)
  GUM_TEST_ACTIF(ToString)
}   // namespace gum_tests
