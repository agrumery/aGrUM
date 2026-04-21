/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/base/core/timer.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNReader.h>
#include <agrum/BN/io/GUM/GumBNReader.h>
#include <agrum/BN/io/GUM/GumBNWriter.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  GumBNWriter
#define GUM_CURRENT_MODULE BN

namespace gum_tests {
  struct GumBNWriterTestSuite {
    private:
    static void _simpleTextForWriter_(bool isbinary) {
      auto bn = gum::BayesNet< double >::fastPrototype("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A");
      const auto path = isbinary ? GET_RESSOURCES_PATH("outputs/test.bgum")
                                 : GET_RESSOURCES_PATH("outputs/test.jgum");

      GUM_CHECK_ASSERT_THROWS_NOTHING({
        gum::GumBNWriter< double > writer(isbinary);
        writer.write(path, bn);
      });

      gum::BayesNet< double > bn2;
      auto                    reader = gum::GumBNReader< double >(&bn2, path, isbinary);
      CHECK_EQ(reader.proceed(), 0u);
      CHECK_EQ(bn2, bn);
    }

    static void _checkMetaData_(bool isbinary) {
      auto bn = gum::BayesNet< double >::fastPrototype("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A");
      const auto path = isbinary ? GET_RESSOURCES_PATH("outputs/test.bgum")
                                 : GET_RESSOURCES_PATH("outputs/test.jgum");

      CHECK_NOTHROW({
        gum::GumBNWriter< double > writer(isbinary, 2);
        writer.write(path, bn);
      });
      {
        gum::BayesNet< double > bn2;
        auto                    reader = gum::GumBNReader< double >(&bn2, path, isbinary);
        CHECK_EQ(reader.proceed(), 0u);
        CHECK_EQ(bn2, bn);

        CHECK(bn2.existsProperty("software"));
        CHECK_EQ(bn2.property("software"), "aGrUM " GUM_VERSION);
        CHECK(bn2.existsProperty("creation"));
        CHECK(bn2.existsProperty("lastModification"));
        // creation == lastModification since file was just created
        CHECK_EQ(bn2.property("creation"), bn2.property("lastModification"));
      }

      // slight delay to ensure timestamps differ on second write
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      {
        CHECK_NOTHROW({
          gum::GumBNWriter< double > writer(isbinary, 2);
          writer.write(path, bn);   // overwrite the file
        });

        gum::BayesNet< double > bn2;
        auto                    reader = gum::GumBNReader< double >(&bn2, path, isbinary);
        CHECK_EQ(reader.proceed(), 0u);
        CHECK_EQ(bn2, bn);

        CHECK(bn2.existsProperty("software"));
        CHECK_EQ(bn2.property("software"), "aGrUM " GUM_VERSION);
        CHECK(bn2.existsProperty("creation"));
        CHECK(bn2.existsProperty("lastModification"));
        // creation != lastModification because of the sleep above
        CHECK_NE(bn2.property("creation"), bn2.property("lastModification"));
      }
    }

    static void _withBigFiles_(bool isbinary, int indent = 0) {
      const auto src    = GET_RESSOURCES_PATH("bifxml/Diabetes.bifxml");
      const auto dstxml = GET_RESSOURCES_PATH("outputs/Diabetes.bifxml");
      const auto dst    = isbinary ? GET_RESSOURCES_PATH("outputs/Diabetes.bgum")
                                   : ((indent < 0) ? GET_RESSOURCES_PATH("outputs/Diabetes_comp.jgum")
                                                   : GET_RESSOURCES_PATH("outputs/Diabetes.jgum"));

      gum::BayesNet< double > bn;

      gum::BIFXMLBNReader< double > reader(&bn, src);
      CHECK_EQ(reader.proceed(), 0u);

      gum::GumBNWriter< double > writer(isbinary, indent);
      writer.write(dst, bn);

      gum::BayesNet< double > bn2;
      auto                    reader2 = gum::GumBNReader< double >(&bn2, dst, isbinary);
      CHECK_EQ(reader2.proceed(), 0u);

      CHECK_EQ(bn2, bn);
    }

    public:
    static void testSimpleTestForWriter() {
      _simpleTextForWriter_(false);
      _simpleTextForWriter_(true);
    }

    static void testCheckMetaData() {
      _checkMetaData_(false);
      _checkMetaData_(true);
    }

    static void testWithBigFiles() {
      _withBigFiles_(false, -1);
      _withBigFiles_(false, 2);
      _withBigFiles_(true);
    }

    static void testBinaryFileIntegrity() {
      // Verify the binary file layout: [8-byte LE uint64 payload size][payload].
      // If the file is opened in text mode on Windows, the CRLF translation corrupts
      // the binary payload and the size prefix no longer matches the actual data.
      auto bn = gum::BayesNet< double >::fastPrototype("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A");
      const auto path = GET_RESSOURCES_PATH("outputs/test_integrity.bgum");

      gum::GumBNWriter< double > writer(true);
      writer.write(path, bn);

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

    static void testSingleVariable() {
      // BN with a single node and no arcs — "parents" section must still exist in JSON
      auto bn = gum::BayesNet< double >::fastPrototype("A{Yes|No}");
      gum::GumBNWriter< double > writer(false, 2);
      const std::string          str = writer.toString(bn);

      gum::BayesNet< double > bn2;
      auto reader = gum::GumBNReader< double >(&bn2);
      CHECK_EQ(reader.proceedFromString(str), 0u);
      CHECK_EQ(bn2, bn);
      CHECK_EQ(bn2.size(), 1u);
    }

    static void testEmptyBN() {
      // BN with no nodes — "nodes", "parents", "cpt" sections must still exist in JSON
      gum::BayesNet< double >    bn;
      gum::GumBNWriter< double > writer(false, 2);
      const std::string          str = writer.toString(bn);

      gum::BayesNet< double > bn2;
      auto                    reader = gum::GumBNReader< double >(&bn2);
      CHECK_EQ(reader.proceedFromString(str), 0u);
      CHECK_EQ(bn2.size(), 0u);
    }

    static void testToString() {
      auto bn = gum::BayesNet< double >::fastPrototype("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A");
      gum::GumBNWriter< double > writer(false, 2);
      std::string                str = writer.toString(bn);

      std::string   tempFileName = std::tmpnam(nullptr);
      std::ofstream tempFile(tempFileName.c_str(), std::ios_base::trunc);
      tempFile << str;
      tempFile.close();

      gum::BayesNet< double > bn2;
      auto                    reader = gum::GumBNReader< double >(&bn2, tempFileName, false);
      CHECK_EQ(reader.proceed(), 0u);
      CHECK_EQ(bn2, bn);
    }
  };

  GUM_TEST_ACTIF(SimpleTestForWriter)
  GUM_TEST_ACTIF(CheckMetaData)
  GUM_TEST_ACTIF(WithBigFiles)
  GUM_TEST_ACTIF(BinaryFileIntegrity)
  GUM_TEST_ACTIF(SingleVariable)
  GUM_TEST_ACTIF(EmptyBN)
  GUM_TEST_ACTIF(ToString)
}   // namespace gum_tests
