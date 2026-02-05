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

#include <agrum/PRM/elements/PRMClassElement.h>
#include <agrum/PRM/elements/PRMParameter.h>
#include <agrum/PRM/PRMFactory.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  PRMFactory
#define GUM_CURRENT_MODULE PRM

namespace gum_tests {
  struct PRMFactoryTestSuite {
    public:
    static void testInit() {
      gum::prm::PRMFactory< double > f;
      delete f.prm();
    }   // namespace gum_tests

    static void testAddParameter() {
      try {
        gum::prm::PRMFactory< double > f;
        auto                           prm = f.prm();

        f.startClass("MyClass");
        f.addParameter("real", "lambda", 0.001);
        f.endClass();

        const auto& c = prm->getClass("MyClass");
        CHECK((static_cast< gum::Size >(1)) == (c.parameters().size()));
        const auto& elt = c.get("lambda");
        CHECK(gum::prm::PRMClassElement< double >::isParameter(elt));
        const auto& lambda = static_cast< const gum::prm::PRMParameter< double >& >(elt);
        CHECK((lambda.valueType()) == (gum::prm::PRMParameter< double >::REAL));
        CHECK((lambda.value()) == (0.001));

        delete prm;

      } catch (gum::Exception& e) {
        std::cout << e.errorType() << std::endl;
        std::cout << e.errorContent() << std::endl;
        std::cout << e.errorCallStack() << std::endl;

        CHECK(false);
      }
    }

    static void testParameterSubClass() {
      try {
        gum::prm::PRMFactory< double > f;
        auto                           prm = f.prm();

        f.startClass("MyClass");
        f.addParameter("real", "lambda", 0.001);
        f.endClass();

        CHECK((prm->classes().size()) == (static_cast< gum::Size >(1)));

        f.startSystem("MySystem");
        f.addInstance("MyClass", "i");
        f.endSystem();

        CHECK((prm->classes().size()) == (static_cast< gum::Size >(2)));

        const auto& super_c = prm->getClass("MyClass");
        const auto& c       = prm->getClass("MyClass<lambda=0.001>");

        CHECK(c.isSubTypeOf(super_c));

        CHECK((static_cast< gum::Size >(1)) == (c.parameters().size()));
        const auto& elt = c.get("lambda");
        CHECK(gum::prm::PRMClassElement< double >::isParameter(elt));
        const auto& lambda = static_cast< const gum::prm::PRMParameter< double >& >(elt);
        CHECK((lambda.valueType()) == (gum::prm::PRMParameter< double >::REAL));
        CHECK((lambda.value()) == (0.001));

        delete prm;

      } catch (gum::Exception& e) {
        std::cout << e.errorType() << std::endl;
        std::cout << e.errorContent() << std::endl;
        std::cout << e.errorCallStack() << std::endl;

        CHECK(false);
      }
    }

    static void testParameterInstantiation() {
      try {
        gum::prm::PRMFactory< double > f;
        auto                           prm = f.prm();

        f.startClass("MyClass");
        f.addParameter("real", "lambda", 0.001);
        f.endClass();

        f.startSystem("MySystem");
        f.addInstance("MyClass", "i");
        f.endSystem();

        const auto& s = prm->getSystem("MySystem");

        CHECK(s.exists("i"));

        const auto& i = s.get("i");

        const auto& c = prm->getClass("MyClass<lambda=0.001>");
        CHECK(c == i.type());

        delete prm;

      } catch (gum::Exception& e) {
        std::cout << e.errorType() << std::endl;
        std::cout << e.errorContent() << std::endl;
        std::cout << e.errorCallStack() << std::endl;

        CHECK(false);
      }
    }

    static void testParameterSpecificInstantiation() {
      try {
        gum::prm::PRMFactory< double > f;
        auto                           prm = f.prm();

        f.startClass("MyClass");
        f.addParameter("real", "lambda", 0.001);
        f.endClass();

        f.startSystem("MySystem");
        gum::HashTable< std::string, double > params;
        params.insert("lambda", 0.009);
        f.addInstance("MyClass", "i", params);
        f.addInstance("MyClass", "j");
        f.addInstance("MyClass", "k", params);

        CHECK((prm->classes().size()) == (static_cast< gum::Size >(3)));

        f.endSystem();
        const auto& s = prm->getSystem("MySystem");

        CHECK(s.exists("i"));

        const auto& i = s.get("i");
        const auto& j = s.get("j");
        const auto& k = s.get("k");

        const auto& super_c = prm->getClass("MyClass");
        const auto& c       = prm->getClass("MyClass<lambda=0.009>");

        const auto& c_default = prm->getClass("MyClass<lambda=0.001>");

        CHECK(c.isSubTypeOf(super_c));
        CHECK(c == i.type());
        CHECK(c == k.type());

        CHECK(c_default.isSubTypeOf(super_c));
        CHECK(c_default == j.type());

        delete prm;

      } catch (gum::Exception& e) {
        std::cout << e.errorType() << std::endl;
        std::cout << e.errorContent() << std::endl;
        std::cout << e.errorCallStack() << std::endl;

        CHECK(false);
      }
    }
  };

  GUM_TEST_ACTIF(Init)
  GUM_TEST_ACTIF(AddParameter)
  GUM_TEST_ACTIF(ParameterSubClass)
  GUM_TEST_ACTIF(ParameterInstantiation)
  GUM_TEST_ACTIF(ParameterSpecificInstantiation)
}   // namespace gum_tests
