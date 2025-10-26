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
#pragma once


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/PRM/o3prm/O3prmBNReader.h>

namespace gum_tests {

  class GUM_TEST_SUITE(O3prmBNReader) {
    public:
    GUM_ACTIVE_TEST(ClassWithoutSystem) {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
      gum::Size                    res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
      TS_ASSERT_EQUALS(bn.property("name"), "Asia")
      TS_ASSERT_EQUALS(res, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(1));   // no system
      TS_ASSERT_EQUALS(bn.size(), static_cast< gum::Size >(8))
    }   // namespace gum_tests

    GUM_ACTIVE_TEST(ClassWithoutSystemWithOtherClassName) {
      try {
        gum::BayesNet< double >      bn;
        gum::O3prmBNReader< double > reader(&bn,
                                            GET_RESSOURCES_PATH("o3prm/AsiaOtherClassName.o3prm"));
        gum::Size                    res = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
        TS_ASSERT_EQUALS(bn.property("name"), "Asia")
        TS_ASSERT_EQUALS(res, static_cast< gum::Size >(0))
        TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(1));   // no system
        TS_ASSERT_EQUALS(bn.size(), static_cast< gum::Size >(8))
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    GUM_ACTIVE_TEST(DoublingClassWithoutSystem) {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
      gum::Size                    res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
      TS_ASSERT_EQUALS(res, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(1));   // no system
      TS_ASSERT_EQUALS(bn.size(), static_cast< gum::Size >(8))


      gum::BayesNet< double >      bn2;
      gum::O3prmBNReader< double > reader2(&bn2, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
      gum::Size                    res2 = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res2 = reader2.proceed())
      TS_ASSERT_EQUALS(res2, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(reader2.warnings(), static_cast< gum::Size >(1));   // no system
      TS_ASSERT_EQUALS(bn2.size(), static_cast< gum::Size >(8))
    }

    GUM_ACTIVE_TEST(ClassWithoutSystemAfterDeletingReader) {
      gum::BayesNet< double > bn;
      {
        gum::O3prmBNReader< double > reader(&bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
        gum::Size                    res = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
        TS_ASSERT_EQUALS(res, static_cast< gum::Size >(0))
        TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(1));   // no system
        TS_ASSERT_EQUALS(bn.size(), static_cast< gum::Size >(8))
      }
    }

    GUM_ACTIVE_TEST(ClassesWithSystem) {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(
          &bn,
          GET_RESSOURCES_PATH("o3prm/AsiaClassAndSystemWithTwoClasses.o3prm"),
          "Asia");
      gum::Size res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
      TS_ASSERT_EQUALS(bn.property("name"), "Asia")
      TS_ASSERT_EQUALS(res, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(bn.size(), static_cast< gum::Size >(8))
    }

    GUM_ACTIVE_TEST(WithError) {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn,
                                          GET_RESSOURCES_PATH("o3prm/DoesNotExists.o3prm"),
                                          "Asia");
      gum::Size                    res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
      TS_ASSERT_EQUALS(res, static_cast< gum::Size >(1));               // file not found
      TS_ASSERT_EQUALS(reader.errors(), static_cast< gum::Size >(1));   // file not found
      TS_ASSERT_EQUALS(bn.size(), static_cast< gum::Size >(0))

      gum::O3prmBNReader< double > reader2(&bn,
                                           GET_RESSOURCES_PATH("o3prm/AsiaWithError.o3prm"),
                                           "Asia");
      res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader2.proceed())   // class plop not existing
      TS_ASSERT_EQUALS(reader2.errors(),
                       static_cast< gum::Size >(1));          // class plop not existing
      TS_ASSERT_EQUALS(bn.size(), static_cast< gum::Size >(0))
    }

    GUM_ACTIVE_TEST(WithCplxFile) {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn,
                                          GET_RESSOURCES_PATH("o3prm/inference.o3prm"),
                                          "aSys");
      gum::Size                    res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
      TS_ASSERT_EQUALS(res, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(bn.size(), static_cast< gum::Size >(72))
    }

    GUM_ACTIVE_TEST(WithClassPathAndSystem) {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(
          &bn,
          GET_RESSOURCES_PATH("/o3prmr/ComplexPrinters/fr/lip6/printers/system.o3prm"),
          "Work",
          GET_RESSOURCES_PATH("o3prmr/ComplexPrinters"));
      gum::Size res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
      TS_ASSERT_EQUALS(res, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(bn.size(), static_cast< gum::Size >(144))
      reader.showElegantErrorsAndWarnings();
    }

    GUM_ACTIVE_TEST(NameWithOrWithoutSystem) {
      // in a file with only one class and no system, there should not be any "."
      // in the names
      {
        gum::BayesNet< double >      bn;
        gum::O3prmBNReader< double > reader(&bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
        gum::Size                    res = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
        TS_ASSERT_EQUALS(res, static_cast< gum::Size >(0))
        TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(1));   // no system
        for (auto n: bn.nodes()) {
          TS_ASSERT_EQUALS(bn.variable(n).name().find("."), std::string::npos)
        }
      }
      {
        gum::BayesNet< double >      bn;
        gum::O3prmBNReader< double > reader(
            &bn,
            GET_RESSOURCES_PATH("o3prm/AsiaClassAndSystemWithTwoClasses.o3prm"),
            "Asia");
        gum::Size res = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
        TS_ASSERT_EQUALS(res, static_cast< gum::Size >(0))
        TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(0))
        for (auto n: bn.nodes()) {
          TS_ASSERT_DIFFERS(bn.variable(n).name().find("."), std::string::npos)
        }
      }
    }

    GUM_ACTIVE_TEST(ReadAndWriteAndRead) {
      gum::BayesNet< double >      bn;
      gum::O3prmBNReader< double > reader(&bn, GET_RESSOURCES_PATH("o3prm/Asia.o3prm"));
      gum::Size                    res = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(res = reader.proceed())
      TS_ASSERT_EQUALS(res, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(1));   // no system
      TS_ASSERT_EQUALS(bn.size(), static_cast< gum::Size >(8))
    }
  };
}   // namespace gum_tests
