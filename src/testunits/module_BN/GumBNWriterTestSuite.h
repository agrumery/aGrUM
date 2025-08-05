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

namespace gum_tests {
  class [[maybe_unused]] GumBNWriterTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(SimpleTestForWriter) {
      _simpleTextFroWriter_(false);
      _simpleTextFroWriter_(true);
    }

    private:
    void _simpleTextFroWriter_(bool isbinary) {
      auto bn = gum::BayesNet< double >::fastPrototype("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A");
      const auto path = isbinary
                          ? GET_RESSOURCES_PATH("outputs/test.bgum")
                          : GET_RESSOURCES_PATH("outputs/test.jgum");

      try {
        gum::GumBNWriter< double > writer(isbinary);
        writer.write(path, bn);
      } catch (const std::exception& e) {
        TS_FAIL("GumBNWriter constructor failed: " + std::string(e.what()));
      }

      gum::BayesNet< double > bn2;
      auto                    reader = gum::GumBNReader< double >(&bn2, path, isbinary);
      TS_ASSERT_EQUALS(reader.proceed(), 0u);
      TS_ASSERT_EQUALS(bn2, bn);
    }

    public:
    GUM_ACTIVE_TEST(CheckMetaData) {
      _CheckMetaData_(false);
      _CheckMetaData_(true);
    }

    private:
    void _CheckMetaData_(bool isbinary) {
      auto bn = gum::BayesNet< double >::fastPrototype("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A");
      const auto path = isbinary
                          ? GET_RESSOURCES_PATH("outputs/test.bgum")
                          : GET_RESSOURCES_PATH("outputs/test.jgum");
      try {
        gum::GumBNWriter< double > writer(isbinary, 2);
        writer.write(path, bn);
      } catch (const std::exception& e) {
        TS_FAIL("GumBNWriter constructor failed: " + std::string(e.what()));
      }
      {
        gum::BayesNet< double > bn2;
        auto                    reader = gum::GumBNReader< double >(&bn2, path, isbinary);
        TS_ASSERT_EQUALS(reader.proceed(), 0u);
        TS_ASSERT_EQUALS(bn2, bn);

        TS_ASSERT(bn2.existsProperty("software"));
        TS_ASSERT_EQUALS(bn2.property("software"), "aGrUM " GUM_VERSION);
        TS_ASSERT(bn2.existsProperty("creation"));
        TS_ASSERT(bn2.existsProperty("lastModification"));
        TS_ASSERT_EQUALS(
            bn2.property("creation"),
            bn2.property("lastModification")); // should be the same as we just created it
      }

      // sleep one second
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      {
        try {
          gum::GumBNWriter< double > writer(isbinary, 2);
          writer.write(path, bn); // overwrite the file
        } catch (const std::exception& e) {
          TS_FAIL("GumBNWriter constructor failed: " + std::string(e.what()));
        }

        gum::BayesNet< double > bn2;
        auto                    reader = gum::GumBNReader< double >(&bn2, path, isbinary);
        TS_ASSERT_EQUALS(reader.proceed(), 0u);
        TS_ASSERT_EQUALS(bn2, bn);

        TS_ASSERT(bn2.existsProperty("software"));
        TS_ASSERT_EQUALS(bn2.property("software"), "aGrUM " GUM_VERSION);
        TS_ASSERT(bn2.existsProperty("creation"));
        TS_ASSERT(bn2.existsProperty("lastModification"));
        TS_ASSERT_DIFFERS(
            bn2.property("creation"),
            bn2.property("lastModification")); // should be different due to the sleep(2seconds)
      }
    }

    public:
    GUM_ACTIVE_TEST(WithBigFiles) {
      _WithBigFiles_(false, -1);
      _WithBigFiles_(false, 2);
      _WithBigFiles_(true);
    }

    private:
    void _WithBigFiles_(bool isbinary, int indent = 0) {
      const auto src    = GET_RESSOURCES_PATH("bifxml/Diabetes.bifxml");
      const auto dstxml = GET_RESSOURCES_PATH("outputs/Diabetes.bifxml");
      const auto dst    = isbinary
                            ? GET_RESSOURCES_PATH("outputs/Diabetes.bgum")
                            : ((indent < 0)
                                 ? GET_RESSOURCES_PATH("outputs/Diabetes_comp.jgum")
                                 : GET_RESSOURCES_PATH("outputs/Diabetes.jgum"));

      gum::BayesNet< double > bn;

      gum::BIFXMLBNReader< double > reader(&bn, src);
      TS_ASSERT_EQUALS(reader.proceed(), 0u);

      gum::GumBNWriter< double > writer(isbinary, indent);
      writer.write(dst, bn);

      gum::BayesNet< double > bn2;
      auto                    reader2 = gum::GumBNReader< double >(&bn2, dst, isbinary);
      TS_ASSERT_EQUALS(reader2.proceed(), 0u);

      TS_ASSERT_EQUALS(bn2, bn);
    }

    public:
    GUM_ACTIVE_TEST(toString) {
      auto bn = gum::BayesNet< double >::fastPrototype("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A");
      gum::GumBNWriter< double > writer(false, 2);
      std::string str = writer.toString(bn);;

      std::string   tempFileName = std::tmpnam(nullptr);
      std::ofstream tempFile(tempFileName.c_str(), std::ios_base::trunc);
      tempFile << str;
      tempFile.close();

      gum::BayesNet< double > bn2;
      auto                    reader = gum::GumBNReader< double >(&bn2, tempFileName, false);
      TS_ASSERT_EQUALS(reader.proceed(), 0u);
      TS_ASSERT_EQUALS(bn2, bn);
    }
  };
} // namespace gum_tests
