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

#include <agrum/ID/influenceDiagram.h>
#include <agrum/ID/io/GUM/GumIDReader.h>
#include <agrum/ID/io/GUM/GumIDWriter.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  GumIDWriter
#define GUM_CURRENT_MODULE ID

namespace gum_tests {
  struct GumIDWriterTestSuite {
    private:
    static void _simpleTestForWriter_(bool isbinary) {
      // A -> C -> *D -> $U, A -> $U  (chance A, chance C, decision D, utility U)
      auto       id   = gum::InfluenceDiagram< double >::fastPrototype("A->C->*D->$U;A->$U");
      const auto path = isbinary ? GET_RESSOURCES_PATH("outputs/test.bid")
                                 : GET_RESSOURCES_PATH("outputs/test.jid");

      GUM_CHECK_ASSERT_THROWS_NOTHING({
        gum::GumIDWriter< double > writer(isbinary);
        writer.write(path, id);
      });

      gum::InfluenceDiagram< double > id2;
      gum::GumIDReader< double >      reader(&id2, path, isbinary);
      reader.proceed();
      CHECK_EQ(reader.count(), 0u);
      CHECK_EQ(id2, id);
    }

    static void _checkMetaData_(bool isbinary) {
      auto       id   = gum::InfluenceDiagram< double >::fastPrototype("A->C->*D->$U;A->$U");
      const auto path = isbinary ? GET_RESSOURCES_PATH("outputs/test.bid")
                                 : GET_RESSOURCES_PATH("outputs/test.jid");

      CHECK_NOTHROW({
        gum::GumIDWriter< double > writer(isbinary, 2);
        writer.write(path, id);
      });

      {
        gum::InfluenceDiagram< double > id2;
        gum::GumIDReader< double >      reader(&id2, path, isbinary);
        reader.proceed();
        CHECK_EQ(reader.count(), 0u);
        CHECK_EQ(id2, id);

        CHECK(id2.existsProperty("software"));
        CHECK_EQ(id2.property("software"), "aGrUM " GUM_VERSION);
        CHECK(id2.existsProperty("creation"));
        CHECK(id2.existsProperty("lastModification"));
        CHECK_EQ(id2.property("creation"), id2.property("lastModification"));
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(500));

      {
        CHECK_NOTHROW({
          gum::GumIDWriter< double > writer(isbinary, 2);
          writer.write(path, id);
        });

        gum::InfluenceDiagram< double > id2;
        gum::GumIDReader< double >      reader(&id2, path, isbinary);
        reader.proceed();
        CHECK_EQ(reader.count(), 0u);
        CHECK_EQ(id2, id);

        CHECK(id2.existsProperty("software"));
        CHECK_EQ(id2.property("software"), "aGrUM " GUM_VERSION);
        CHECK(id2.existsProperty("creation"));
        CHECK(id2.existsProperty("lastModification"));
        CHECK_NE(id2.property("creation"), id2.property("lastModification"));
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
      auto       id   = gum::InfluenceDiagram< double >::fastPrototype("A->C->*D->$U;A->$U");
      const auto path = GET_RESSOURCES_PATH("outputs/test_integrity.bid");

      gum::GumIDWriter< double > writer(true);
      writer.write(path, id);

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

    static void testAllRootNodesInBinary() {
      // ID with only isolated nodes (no arcs) — every node has an empty parent list.
      // Before the fix, parentList was default-initialised to null instead of [],
      // producing invalid msgpack when serialised in binary mode.
      auto       id   = gum::InfluenceDiagram< double >::fastPrototype("A[2];*D[2];$U");
      const auto path = GET_RESSOURCES_PATH("outputs/test_allroots.bid");

      gum::GumIDWriter< double > writer(true);
      writer.write(path, id);

      gum::InfluenceDiagram< double > id2;
      gum::GumIDReader< double >      reader(&id2, path, true);
      reader.proceed();
      CHECK_EQ(reader.count(), 0u);
      CHECK_EQ(id2.size(), id.size());
    }

    static void testSingleVariable() {
      // ID with a single chance node and no arcs
      auto                       id = gum::InfluenceDiagram< double >::fastPrototype("A[2]");
      gum::GumIDWriter< double > writer(false, 2);
      const std::string          str = writer.toString(id);

      gum::InfluenceDiagram< double > id2;
      gum::GumIDReader< double >      reader(&id2);
      CHECK_EQ(reader.proceedFromString(str), 0u);
      CHECK_EQ(id2, id);
      CHECK_EQ(id2.size(), 1u);
    }

    static void testToString() {
      auto                       id = gum::InfluenceDiagram< double >::fastPrototype("A->*D->$U");
      gum::GumIDWriter< double > writer(false, 2);
      std::string                str = writer.toString(id);

      std::string   tempFileName = gum_tests::getTempFilePath();
      std::ofstream tempFile(tempFileName.c_str(), std::ios_base::trunc);
      tempFile << str;
      tempFile.close();

      gum::InfluenceDiagram< double > id2;
      gum::GumIDReader< double >      reader(&id2, tempFileName, false);
      reader.proceed();
      CHECK_EQ(reader.count(), 0u);
      CHECK_EQ(id2, id);
    }
  };

  GUM_TEST_ACTIF(SimpleTestForWriter)
  GUM_TEST_ACTIF(CheckMetaData)
  GUM_TEST_ACTIF(BinaryFileIntegrity)
  GUM_TEST_ACTIF(AllRootNodesInBinary)
  GUM_TEST_ACTIF(SingleVariable)
  GUM_TEST_ACTIF(ToString)
}   // namespace gum_tests
