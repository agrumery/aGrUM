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


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/PRM/o3prmr/O3prmrInterpreter.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  O3prmr
#define GUM_CURRENT_MODULE PRM

namespace gum_tests {

  struct O3prmrTestSuite {
    public:
    static void testParseFile1() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(true);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/"));

        GUM_CHECK_ASSERT_THROWS_NOTHING(
            si->interpretFile(GET_RESSOURCES_PATH("o3prmr/requests/query1.o3prmr")));

        CHECK((si->errors()) == (static_cast< gum::Size >(0)));
        if (si->errors() > 0) { si->showElegantErrorsAndWarnings(); }
        CHECK((si->warnings()) == (static_cast< gum::Size >(0)));

        delete si;
      }   // namespace gum_tests

      catch (gum::Exception&) {
        CHECK(false);
      }
    }

    O3prmrTestSuite() {
      // std::cerr << std::endl;
    }

    static void testParseFile2() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(true);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/"));

        GUM_CHECK_ASSERT_THROWS_NOTHING(
            si->interpretFile(GET_RESSOURCES_PATH("o3prmr/requests/query2.o3prmr")));

        CHECK((si->errors()) == (static_cast< gum::Size >(1)));
        CHECK((si->warnings()) == (static_cast< gum::Size >(0)));

        delete si;
      } catch (gum::Exception&) { CHECK(false); }
    }

    // ->
    static void testInference() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(false);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/"));

        GUM_CHECK_ASSERT_THROWS_NOTHING(
            si->interpretFile(GET_RESSOURCES_PATH("o3prmr/requests/query1.o3prmr")));

        CHECK((si->errors()) == (static_cast< gum::Size >(0)));
        CHECK((si->warnings()) == (static_cast< gum::Size >(0)));

        delete si;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testObserve() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(false);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/"));

        GUM_CHECK_ASSERT_THROWS_NOTHING(
            si->interpretFile(GET_RESSOURCES_PATH("o3prmr/requests/queryObserveTest.o3prmr")));

        CHECK((si->errors()) == (static_cast< gum::Size >(0)));
        CHECK((si->warnings()) == (static_cast< gum::Size >(0)));

        // Observe correctly
        const auto& c1 = si->prm()->getSystem("systems.MySystem.MySystem").get("c1");
        const auto& c2 = si->prm()->getSystem("systems.MySystem.MySystem").get("c2");

        CHECK(si->inference()->hasEvidence(
            gum::prm::PRMInference< double >::Chain(&c1, &c1.get("can_print"))));
        CHECK(si->inference()->hasEvidence(
            gum::prm::PRMInference< double >::Chain(&c2, &c2.get("equipState"))));
        CHECK(!si->inference()->hasEvidence(
            gum::prm::PRMInference< double >::Chain(&c1, &c1.get("equipState"))));
        CHECK(!si->inference()->hasEvidence(
            gum::prm::PRMInference< double >::Chain(&c2, &c2.get("can_print"))));

        const auto& e = si->inference()->evidence(c2);
        const auto  p = e[c1.get("equipState").id()];

        gum::Instantiation j(*p);
        const auto&        c2_equipState = c2.get("equipState");

        for (j.setFirst(); !j.end(); j.inc()) {
          if (c2_equipState.type().variable().label(j.val(c2_equipState.type().variable()))
              == "Dysfunctional") {
            CHECK((p->get(j)) == (1.0));
          } else {
            CHECK((p->get(j)) == (0.0));
          }
        }

        delete si;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testUnobserve() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(false);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/"));

        GUM_CHECK_ASSERT_THROWS_NOTHING(
            si->interpretFile(GET_RESSOURCES_PATH("o3prmr/requests/queryUnobserveTest.o3prmr")));

        CHECK((si->errors()) == (static_cast< gum::Size >(0)));
        CHECK((si->warnings()) == (static_cast< gum::Size >(0)));

        // Unobserve correctly
        const gum::prm::PRMInstance< double >& c1
            = si->prm()->getSystem("systems.MySystem.MySystem").get("c1");
        const gum::prm::PRMInstance< double >& c2
            = si->prm()->getSystem("systems.MySystem.MySystem").get("c2");

        CHECK(!si->inference()->hasEvidence(
            gum::prm::PRMInference< double >::Chain(&c1, &c1.get("can_print"))));
        CHECK(!si->inference()->hasEvidence(
            gum::prm::PRMInference< double >::Chain(&c2, &c2.get("equipState"))));
        CHECK(!si->inference()->hasEvidence(
            gum::prm::PRMInference< double >::Chain(&c1, &c1.get("equipState"))));
        CHECK(!si->inference()->hasEvidence(
            gum::prm::PRMInference< double >::Chain(&c2, &c2.get("can_print"))));

        delete si;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testQuery() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(false);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/"));

        GUM_CHECK_ASSERT_THROWS_NOTHING(
            si->interpretFile(GET_RESSOURCES_PATH("o3prmr/requests/query1.o3prmr")));

        si->showElegantErrorsAndWarnings();
        CHECK((si->errors()) == (static_cast< gum::Size >(0)));
        CHECK((si->warnings()) == (static_cast< gum::Size >(0)));

        const gum::prm::PRMInstance< double >& c1
            = si->prm()->getSystem("systems.MySystem.MySystem").get("c1");
        const gum::prm::PRMInstance< double >& c2
            = si->prm()->getSystem("systems.MySystem.MySystem").get("c2");

        CHECK(!si->inference()->hasEvidence(
            gum::prm::PRMInference< double >::Chain(&c1, &c1.get("can_print"))));
        CHECK(si->inference()->hasEvidence(
            gum::prm::PRMInference< double >::Chain(&c2, &c2.get("equipState"))));
        CHECK(!si->inference()->hasEvidence(
            gum::prm::PRMInference< double >::Chain(&c1, &c1.get("equipState"))));
        CHECK(!si->inference()->hasEvidence(
            gum::prm::PRMInference< double >::Chain(&c2, &c2.get("can_print"))));

        // Est-ce que la valeur Dysfonctionnal de l'attribut can_print de
        // l'instance c1 est à 1.0

        const gum::prm::PRMInference< double >::EMap& e = si->inference()->evidence(c2);
        const gum::Tensor< double >*                  p = e[c1.get("equipState").id()];

        gum::Instantiation                      j(*p);
        const gum::prm::PRMAttribute< double >& c2_equipState = c2.get("equipState");

        for (j.setFirst(); !j.end(); j.inc()) {
          if (c2_equipState.type().variable().label(j.val(c2_equipState.type().variable()))
              == "Dysfunctional") {
            CHECK((p->get(j)) == (1.0));
          } else {
            CHECK((p->get(j)) == (0.0));
          }
        }

        delete si;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testAsiaBN() {
      try {
        // Arrange
        auto si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(false);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/Asia/"));
        si->interpretFile(GET_RESSOURCES_PATH("o3prmr/Asia/myRequest.o3prmr"));
        auto                                prm  = si->prm();
        gum::prm::PRMClass< double > const* asia = nullptr;
        // Act
        CHECK_NOTHROW(asia = &(prm->getClass("Asia.Asia")));
        // Assert
        CHECK((asia->attributes().size()) == (static_cast< gum::Size >(8)));
        CHECK((asia->containerDag().sizeArcs()) == (static_cast< gum::Size >(8)));
        delete si;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testAsiaBNGrd() {
      try {
        // Arrange
        auto si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(false);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/Asia/"));
        si->interpretFile(GET_RESSOURCES_PATH("o3prmr/Asia/myRequest.o3prmr"));
        auto                           prm = si->prm();
        const auto&                    sys = prm->getSystem("system.Asia");
        auto                           bn  = new gum::BayesNet< double >("plop");
        gum::BayesNetFactory< double > factory(bn);
        // Act
        CHECK_NOTHROW(sys.groundedBN(factory));
        // Assert
        CHECK((bn->size()) == (static_cast< gum::Size >(8)));
        CHECK((bn->sizeArcs()) == (static_cast< gum::Size >(8)));
        delete si;
        delete bn;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testAsiaOneFileBNGrd() {
      try {
        // Arrange
        auto si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(false);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/AsiaOneFile/"));
        si->interpretFile(GET_RESSOURCES_PATH("o3prmr/AsiaOneFile/myRequest.o3prmr"));
        // si->showElegantErrorsAndWarnings();
        CHECK((si->count()) == (static_cast< gum::Size >(0)));
        if (!si->count()) {
          auto                           prm = si->prm();
          const auto&                    sys = prm->getSystem("Asia.Asia");
          auto                           bn  = new gum::BayesNet< double >("plop");
          gum::BayesNetFactory< double > factory(bn);
          // Act
          CHECK_NOTHROW(sys.groundedBN(factory));
          // Assert
          CHECK((bn->size()) == (static_cast< gum::Size >(8)));
          CHECK((bn->sizeArcs()) == (static_cast< gum::Size >(8)));
          delete bn;
        }
        delete si;
      } catch (gum::Exception&) { CHECK(false); }
    }

    // ->
    static void testComplexPrintersGrd() {
      try {
        // Arrange
        auto si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(false);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/ComplexPrinters/"));
        si->interpretFile(
            GET_RESSOURCES_PATH("o3prmr/ComplexPrinters/fr/lip6/printers/request.o3prmr"));
        auto                           prm = si->prm();
        const auto&                    sys = prm->getSystem("fr.lip6.printers.system.Work");
        auto                           bn  = new gum::BayesNet< double >("plop");
        gum::BayesNetFactory< double > factory(bn);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(sys.groundedBN(factory));
        // Assert
        CHECK((bn->size()) == (static_cast< gum::Size >(144)));
        CHECK((bn->sizeArcs()) == (static_cast< gum::Size >(193)));
        delete si;
        delete bn;
      } catch (gum::Exception&) { CHECK(false); }
    }

    // ->
    static void testADDWithoutSlash() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter si;
        si.setSyntaxMode(false);
        si.addPath(GET_RESSOURCES_PATH("o3prmr/ADD"));

        GUM_CHECK_ASSERT_THROWS_NOTHING(
            si.interpretFile(GET_RESSOURCES_PATH("o3prmr/ADD/Request.o3prmr")));

        si.showElegantErrorsAndWarnings();
        CHECK((si.errors()) == (static_cast< gum::Size >(0)));
        CHECK((si.warnings()) == (static_cast< gum::Size >(0)));

        CHECK((si.results().size()) == (static_cast< gum::Size >(1)));

      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testADD() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter si;
        si.setSyntaxMode(false);
        si.addPath(GET_RESSOURCES_PATH("o3prmr/ADD/"));

        GUM_CHECK_ASSERT_THROWS_NOTHING(
            si.interpretFile(GET_RESSOURCES_PATH("o3prmr/ADD/Request.o3prmr")));

        si.showElegantErrorsAndWarnings();
        CHECK((si.errors()) == (static_cast< gum::Size >(0)));
        CHECK((si.warnings()) == (static_cast< gum::Size >(0)));

        CHECK((si.results().size()) == (static_cast< gum::Size >(1)));

        auto result = si.results()[0];

      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testOrAnd() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter si;
        si.setSyntaxMode(false);
        si.addPath(GET_RESSOURCES_PATH("o3prmr/aggregates/"));

        GUM_CHECK_ASSERT_THROWS_NOTHING(
            si.interpretFile(GET_RESSOURCES_PATH("o3prmr/aggregates/request.o3prmr")));

        si.showElegantErrorsAndWarnings();
        CHECK((si.errors()) == (static_cast< gum::Size >(0)));
        CHECK((si.warnings()) == (static_cast< gum::Size >(0)));

        CHECK((si.results().size()) == (static_cast< gum::Size >(7)));

        auto result = si.results()[0];

      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testStudents() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter si;
        si.setSyntaxMode(false);
        si.addPath(GET_RESSOURCES_PATH("o3prmr/University/"));

        GUM_CHECK_ASSERT_THROWS_NOTHING(
            si.interpretFile(GET_RESSOURCES_PATH("o3prmr/University/fr/request.o3prmr")));

        si.showElegantErrorsAndWarnings();
        CHECK((si.errors()) == (static_cast< gum::Size >(0)));
        CHECK((si.warnings()) == (static_cast< gum::Size >(0)));

        CHECK((si.results().size()) == (static_cast< gum::Size >(1)));

      } catch (gum::Exception&) { CHECK(false); }
    }
  };

  GUM_TEST_ACTIF(ParseFile1)
  GUM_TEST_ACTIF(ParseFile2)
  GUM_TEST_ACTIF(Inference)
  GUM_TEST_ACTIF(Observe)
  GUM_TEST_ACTIF(Unobserve)
  GUM_TEST_ACTIF(Query)
  GUM_TEST_ACTIF(AsiaBN)
  GUM_TEST_ACTIF(AsiaBNGrd)
  GUM_TEST_ACTIF(AsiaOneFileBNGrd)
  GUM_TEST_ACTIF(ComplexPrintersGrd)
  GUM_TEST_ACTIF(ADDWithoutSlash)
  GUM_TEST_ACTIF(ADD)
  GUM_TEST_ACTIF(OrAnd)
  GUM_TEST_ACTIF(Students)

}   // namespace gum_tests
