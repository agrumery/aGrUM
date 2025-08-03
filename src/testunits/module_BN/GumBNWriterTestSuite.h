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


#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/GUM/GumBNReader.h>
#include <agrum/BN/io/GUM/GumBNWriter.h>

namespace gum_tests {
  class [[maybe_unused]] BNGumWriterTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(SimpleTestForWriter) {
      auto bn = gum::BayesNet< double >::fastPrototype("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A");
      const auto path = GET_RESSOURCES_PATH("outputs/test.jgum");
      try {
        gum::GumBNWriter< double > writer;
        writer.write(path, bn);
      } catch (const std::exception& e) {
        TS_FAIL("GumBNWriter constructor failed: " + std::string(e.what()));
      }

      gum::BayesNet< double > bn2;
      auto                    reader = gum::GumBNReader< double >(&bn2, path);
      TS_ASSERT_EQUALS(reader.proceed(), 0u);
      TS_ASSERT_EQUALS(bn2, bn);
    }

    GUM_ACTIVE_TEST(CheckMetaData) {
      auto bn = gum::BayesNet< double >::fastPrototype("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A");
      const auto path = GET_RESSOURCES_PATH("outputs/test.jgum");
      try {
        gum::GumBNWriter< double > writer;
        writer.write(path, bn);
      } catch (const std::exception& e) {
        TS_FAIL("GumBNWriter constructor failed: " + std::string(e.what()));
      }
      {
        gum::BayesNet< double > bn2;
        auto                    reader = gum::GumBNReader< double >(&bn2, path);
        TS_ASSERT_EQUALS(reader.proceed(), 0u);
        TS_ASSERT_EQUALS(bn2, bn);

        TS_ASSERT(bn2.existsProperty("software"));
        TS_ASSERT_EQUALS(bn2.property("software"), "aGrUM " GUM_VERSION);
        TS_ASSERT(bn2.existsProperty("creation"));
        TS_ASSERT(bn2.existsProperty("lastModification"));
        TS_ASSERT_EQUALS(
            bn2.property("creation"),
            bn2.property("lastModification"));   // should be the same as we just created it
      }

      // sleep one second
      std::this_thread::sleep_for(std::chrono::seconds(2));
      {
        try {
          gum::GumBNWriter< double > writer;
          writer.write(path, bn);
        } catch (const std::exception& e) {
          TS_FAIL("GumBNWriter constructor failed: " + std::string(e.what()));
        }

        gum::BayesNet< double > bn2;
        auto                    reader = gum::GumBNReader< double >(&bn2, path);
        TS_ASSERT_EQUALS(reader.proceed(), 0u);
        TS_ASSERT_EQUALS(bn2, bn);

        TS_ASSERT(bn2.existsProperty("software"));
        TS_ASSERT_EQUALS(bn2.property("software"), "aGrUM " GUM_VERSION);
        TS_ASSERT(bn2.existsProperty("creation"));
        TS_ASSERT(bn2.existsProperty("lastModification"));
        TS_ASSERT_DIFFERS(
            bn2.property("creation"),
            bn2.property("lastModification"));   // should be different due to the sleep(2seconds)
      }
    }
    GUM_ACTIVE_TEST(WithBigFiles) {

    }
  };
}   // namespace gum_tests
