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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/PRM/o3prm/O3prmBNReader.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  O3prmBNReader
#define GUM_CURRENT_MODULE PRM

namespace gum_tests {

  struct O3prmBNReaderTestSuite {
    public:
    static void testClassWithoutSystem() {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
      gum::Size                    res = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(res = reader.proceed());
      CHECK((bn.property("name")) == ("Asia"));
      CHECK((res) == (static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(1)));   // no system
      CHECK((bn.size()) == (static_cast< gum::Size >(8)));
    }   // namespace gum_tests

    static void testClassWithoutSystemWithOtherClassName() {
      try {
        gum::BayesNet< double >      bn;
        gum::O3prmBNReader< double > reader(&bn,
                                            GET_RESSOURCES_PATH("o3prm/AsiaOtherClassName.o3prm"));
        gum::Size                    res = 0;
        GUM_CHECK_ASSERT_THROWS_NOTHING(res = reader.proceed());
        CHECK((bn.property("name")) == ("Asia"));
        CHECK((res) == (static_cast< gum::Size >(0)));
        CHECK((reader.warnings()) == (static_cast< gum::Size >(1)));   // no system
        CHECK((bn.size()) == (static_cast< gum::Size >(8)));
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    static void testDoublingClassWithoutSystem() {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
      gum::Size                    res = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(res = reader.proceed());
      CHECK((res) == (static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(1)));   // no system
      CHECK((bn.size()) == (static_cast< gum::Size >(8)));


      gum::BayesNet< double >      bn2;
      gum::O3prmBNReader< double > reader2(&bn2, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
      gum::Size                    res2 = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(res2 = reader2.proceed());
      CHECK((res2) == (static_cast< gum::Size >(0)));
      CHECK((reader2.warnings()) == (static_cast< gum::Size >(1)));   // no system
      CHECK((bn2.size()) == (static_cast< gum::Size >(8)));
    }

    static void testClassWithoutSystemAfterDeletingReader() {
      gum::BayesNet< double > bn;
      {
        gum::O3prmBNReader< double > reader(&bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
        gum::Size                    res = 0;
        GUM_CHECK_ASSERT_THROWS_NOTHING(res = reader.proceed());
        CHECK((res) == (static_cast< gum::Size >(0)));
        CHECK((reader.warnings()) == (static_cast< gum::Size >(1)));   // no system
        CHECK((bn.size()) == (static_cast< gum::Size >(8)));
      }
    }

    static void testClassesWithSystem() {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(
          &bn,
          GET_RESSOURCES_PATH("o3prm/AsiaClassAndSystemWithTwoClasses.o3prm"),
          "Asia");
      gum::Size res = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(res = reader.proceed());
      CHECK((bn.property("name")) == ("Asia"));
      CHECK((res) == (static_cast< gum::Size >(0)));
      CHECK((bn.size()) == (static_cast< gum::Size >(8)));
    }

    static void testWithError() {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn,
                                          GET_RESSOURCES_PATH("o3prm/DoesNotExists.o3prm"),
                                          "Asia");
      gum::Size                    res = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(res = reader.proceed());
      CHECK((res) == (static_cast< gum::Size >(1)));               // file not found
      CHECK((reader.errors()) == (static_cast< gum::Size >(1)));   // file not found
      CHECK((bn.size()) == (static_cast< gum::Size >(0)));

      gum::O3prmBNReader< double > reader2(&bn,
                                           GET_RESSOURCES_PATH("o3prm/AsiaWithError.o3prm"),
                                           "Asia");
      res = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(res = reader2.proceed());     // class plop not existing
      CHECK((reader2.errors()) == (static_cast< gum::Size >(1)));   // class plop not existing
      CHECK((bn.size()) == (static_cast< gum::Size >(0)));
    }

    static void testWithCplxFile() {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn,
                                          GET_RESSOURCES_PATH("o3prm/inference.o3prm"),
                                          "aSys");
      gum::Size                    res = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(res = reader.proceed());
      CHECK((res) == (static_cast< gum::Size >(0)));
      CHECK((bn.size()) == (static_cast< gum::Size >(72)));
    }

    static void testWithClassPathAndSystem() {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(
          &bn,
          GET_RESSOURCES_PATH("/o3prmr/ComplexPrinters/fr/lip6/printers/system.o3prm"),
          "Work",
          GET_RESSOURCES_PATH("o3prmr/ComplexPrinters"));
      gum::Size res = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(res = reader.proceed());
      CHECK((res) == (static_cast< gum::Size >(0)));
      CHECK((bn.size()) == (static_cast< gum::Size >(144)));
      reader.showElegantErrorsAndWarnings();
    }

    static void testNameWithOrWithoutSystem() {
      // in a file with only one class and no system, there should not be any "."
      // in the names
      {
        gum::BayesNet< double >      bn;
        gum::O3prmBNReader< double > reader(&bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
        gum::Size                    res = 0;
        GUM_CHECK_ASSERT_THROWS_NOTHING(res = reader.proceed());
        CHECK((res) == (static_cast< gum::Size >(0)));
        CHECK((reader.warnings()) == (static_cast< gum::Size >(1)));   // no system
        for (auto n: bn.nodes()) {
          CHECK((bn.variable(n).name().find(".")) == (std::string::npos));
        }
      }
      {
        gum::BayesNet< double >      bn;
        gum::O3prmBNReader< double > reader(
            &bn,
            GET_RESSOURCES_PATH("o3prm/AsiaClassAndSystemWithTwoClasses.o3prm"),
            "Asia");
        gum::Size res = 0;
        GUM_CHECK_ASSERT_THROWS_NOTHING(res = reader.proceed());
        CHECK((res) == (static_cast< gum::Size >(0)));
        CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));
        for (auto n: bn.nodes()) {
          CHECK((bn.variable(n).name().find(".")) != (std::string::npos));
        }
      }
    }

    static void testReadAndWriteAndRead() {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
      gum::Size                    res = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(res = reader.proceed());
      CHECK((res) == (static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(1)));   // no system
      CHECK((bn.size()) == (static_cast< gum::Size >(8)));
    }
  };

  GUM_TEST_ACTIF(ClassWithoutSystem)
  GUM_TEST_ACTIF(ClassWithoutSystemWithOtherClassName)
  GUM_TEST_ACTIF(DoublingClassWithoutSystem)
  GUM_TEST_ACTIF(ClassWithoutSystemAfterDeletingReader)
  GUM_TEST_ACTIF(ClassesWithSystem)
  GUM_TEST_ACTIF(WithError)
  GUM_TEST_ACTIF(WithCplxFile)
  GUM_TEST_ACTIF(WithClassPathAndSystem)
  GUM_TEST_ACTIF(NameWithOrWithoutSystem)
  GUM_TEST_ACTIF(ReadAndWriteAndRead)
}   // namespace gum_tests
