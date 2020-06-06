
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/PRM/o3prmr/O3prmrInterpreter.h>

namespace gum_tests {

  class O3prmrTestSuite: public CxxTest::TestSuite {
    public:
    void setUp() {
      // std::cerr << std::endl;
    }

    void tearDown() {}

    void testParseFile1() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si =
           new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(true);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/"));

        TS_GUM_ASSERT_THROWS_NOTHING(si->interpretFile(
           GET_RESSOURCES_PATH("o3prmr/requests/query1.o3prmr")));

        TS_ASSERT_EQUALS(si->errors(), (gum::Size)0);
        if (si->errors() > 0) { si->showElegantErrorsAndWarnings(); }
        TS_ASSERT_EQUALS(si->warnings(), (gum::Size)0);

        delete si;
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testParseFile2() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si =
           new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(true);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/"));

        TS_GUM_ASSERT_THROWS_NOTHING(si->interpretFile(
           GET_RESSOURCES_PATH("o3prmr/requests/query2.o3prmr")));

        TS_ASSERT_EQUALS(si->errors(), (gum::Size)1);
        TS_ASSERT_EQUALS(si->warnings(), (gum::Size)0);

        delete si;
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    // ->
    void testInference() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si =
           new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(false);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/"));

        TS_GUM_ASSERT_THROWS_NOTHING(si->interpretFile(
           GET_RESSOURCES_PATH("o3prmr/requests/query1.o3prmr")));

        TS_ASSERT_EQUALS(si->errors(), (gum::Size)0);
        TS_ASSERT_EQUALS(si->warnings(), (gum::Size)0);

        delete si;
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testObserve() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si =
           new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(false);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/"));

        TS_GUM_ASSERT_THROWS_NOTHING(si->interpretFile(
           GET_RESSOURCES_PATH("o3prmr/requests/queryObserveTest.o3prmr")));

        TS_ASSERT_EQUALS(si->errors(), (gum::Size)0);
        TS_ASSERT_EQUALS(si->warnings(), (gum::Size)0);

        // Observe correctly
        const auto& c1 =
           si->prm()->getSystem("systems.MySystem.MySystem").get("c1");
        const auto& c2 =
           si->prm()->getSystem("systems.MySystem.MySystem").get("c2");

        TS_ASSERT(si->inference()->hasEvidence(
           gum::prm::PRMInference< double >::Chain(&c1, &c1.get("can_print"))));
        TS_ASSERT(si->inference()->hasEvidence(
           gum::prm::PRMInference< double >::Chain(&c2, &c2.get("equipState"))));
        TS_ASSERT(!si->inference()->hasEvidence(
           gum::prm::PRMInference< double >::Chain(&c1, &c1.get("equipState"))));
        TS_ASSERT(!si->inference()->hasEvidence(
           gum::prm::PRMInference< double >::Chain(&c2, &c2.get("can_print"))));

        const auto& e = si->inference()->evidence(c2);
        const auto  p = e[c1.get("equipState").id()];

        gum::Instantiation j(*p);
        const auto&        c2_equipState = c2.get("equipState");

        for (j.setFirst(); !j.end(); j.inc()) {
          if (c2_equipState.type().variable().label(
                 j.val(c2_equipState.type().variable()))
              == "Dysfunctional") {
            TS_ASSERT_EQUALS(p->get(j), 1.0);
          } else {
            TS_ASSERT_EQUALS(p->get(j), 0.0);
          }
        }

        delete si;
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testUnobserve() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si =
           new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(false);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/"));

        TS_GUM_ASSERT_THROWS_NOTHING(si->interpretFile(
           GET_RESSOURCES_PATH("o3prmr/requests/queryUnobserveTest.o3prmr")));

        TS_ASSERT_EQUALS(si->errors(), (gum::Size)0);
        TS_ASSERT_EQUALS(si->warnings(), (gum::Size)0);

        // Unobserve correctly
        const gum::prm::PRMInstance< double >& c1 =
           si->prm()->getSystem("systems.MySystem.MySystem").get("c1");
        const gum::prm::PRMInstance< double >& c2 =
           si->prm()->getSystem("systems.MySystem.MySystem").get("c2");

        TS_ASSERT(!si->inference()->hasEvidence(
           gum::prm::PRMInference< double >::Chain(&c1, &c1.get("can_print"))));
        TS_ASSERT(!si->inference()->hasEvidence(
           gum::prm::PRMInference< double >::Chain(&c2, &c2.get("equipState"))));
        TS_ASSERT(!si->inference()->hasEvidence(
           gum::prm::PRMInference< double >::Chain(&c1, &c1.get("equipState"))));
        TS_ASSERT(!si->inference()->hasEvidence(
           gum::prm::PRMInference< double >::Chain(&c2, &c2.get("can_print"))));

        delete si;
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testQuery() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si =
           new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(false);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/"));

        TS_GUM_ASSERT_THROWS_NOTHING(si->interpretFile(
           GET_RESSOURCES_PATH("o3prmr/requests/query1.o3prmr")));

        si->showElegantErrorsAndWarnings();
        TS_ASSERT_EQUALS(si->errors(), (gum::Size)0);
        TS_ASSERT_EQUALS(si->warnings(), (gum::Size)0);

        const gum::prm::PRMInstance< double >& c1 =
           si->prm()->getSystem("systems.MySystem.MySystem").get("c1");
        const gum::prm::PRMInstance< double >& c2 =
           si->prm()->getSystem("systems.MySystem.MySystem").get("c2");

        TS_ASSERT(!si->inference()->hasEvidence(
           gum::prm::PRMInference< double >::Chain(&c1, &c1.get("can_print"))));
        TS_ASSERT(si->inference()->hasEvidence(
           gum::prm::PRMInference< double >::Chain(&c2, &c2.get("equipState"))));
        TS_ASSERT(!si->inference()->hasEvidence(
           gum::prm::PRMInference< double >::Chain(&c1, &c1.get("equipState"))));
        TS_ASSERT(!si->inference()->hasEvidence(
           gum::prm::PRMInference< double >::Chain(&c2, &c2.get("can_print"))));

        // Est-ce que la valeur Dysfonctionnal de l'attribut can_print de
        // l'instance c1 est à 1.0

        const gum::prm::PRMInference< double >::EMap& e =
           si->inference()->evidence(c2);
        const gum::Potential< double >* p = e[c1.get("equipState").id()];

        gum::Instantiation                      j(*p);
        const gum::prm::PRMAttribute< double >& c2_equipState =
           c2.get("equipState");

        for (j.setFirst(); !j.end(); j.inc()) {
          if (c2_equipState.type().variable().label(
                 j.val(c2_equipState.type().variable()))
              == "Dysfunctional") {
            TS_ASSERT_EQUALS(p->get(j), 1.0);
          } else {
            TS_ASSERT_EQUALS(p->get(j), 0.0);
          }
        }

        delete si;
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testAsiaBN() {
      try {
        // Arrange
        auto si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(false);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/Asia/"));
        si->interpretFile(GET_RESSOURCES_PATH("o3prmr/Asia/myRequest.o3prmr"));
        auto                                prm = si->prm();
        gum::prm::PRMClass< double > const* asia = nullptr;
        // Act
        TS_ASSERT_THROWS_NOTHING(asia = &(prm->getClass("Asia.Asia")));
        // Assert
        TS_ASSERT_EQUALS(asia->attributes().size(), (gum::Size)8);
        TS_ASSERT_EQUALS(asia->containerDag().sizeArcs(), (gum::Size)8);
        delete si;
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testAsiaBNGrd() {
      try {
        // Arrange
        auto si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(false);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/Asia/"));
        si->interpretFile(GET_RESSOURCES_PATH("o3prmr/Asia/myRequest.o3prmr"));
        auto                           prm = si->prm();
        const auto&                    sys = prm->getSystem("system.Asia");
        auto                           bn = new gum::BayesNet< double >("plop");
        gum::BayesNetFactory< double > factory(bn);
        // Act
        TS_ASSERT_THROWS_NOTHING(sys.groundedBN(factory));
        // Assert
        TS_ASSERT_EQUALS(bn->size(), (gum::Size)8);
        TS_ASSERT_EQUALS(bn->sizeArcs(), (gum::Size)8);
        delete si;
        delete bn;
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testAsiaOneFileBNGrd() {
      try {
        // Arrange
        auto si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(false);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/AsiaOneFile/"));
        si->interpretFile(
           GET_RESSOURCES_PATH("o3prmr/AsiaOneFile/myRequest.o3prmr"));
        // si->showElegantErrorsAndWarnings();
        TS_ASSERT_EQUALS(si->count(), (gum::Size)0);
        if (!si->count()) {
          auto                           prm = si->prm();
          const auto&                    sys = prm->getSystem("Asia.Asia");
          auto                           bn = new gum::BayesNet< double >("plop");
          gum::BayesNetFactory< double > factory(bn);
          // Act
          TS_ASSERT_THROWS_NOTHING(sys.groundedBN(factory));
          // Assert
          TS_ASSERT_EQUALS(bn->size(), (gum::Size)8);
          TS_ASSERT_EQUALS(bn->sizeArcs(), (gum::Size)8);
          delete bn;
        }
        delete si;
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    // ->
    void testComplexPrintersGrd() {
      try {
        // Arrange
        auto si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode(false);
        si->addPath(GET_RESSOURCES_PATH("o3prmr/ComplexPrinters/"));
        si->interpretFile(GET_RESSOURCES_PATH(
           "o3prmr/ComplexPrinters/fr/lip6/printers/request.o3prmr"));
        auto        prm = si->prm();
        const auto& sys = prm->getSystem("fr.lip6.printers.system.Work");
        auto        bn = new gum::BayesNet< double >("plop");
        gum::BayesNetFactory< double > factory(bn);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(sys.groundedBN(factory));
        // Assert
        TS_ASSERT_EQUALS(bn->size(), (gum::Size)144);
        TS_ASSERT_EQUALS(bn->sizeArcs(), (gum::Size)193);
        delete si;
        delete bn;
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    // ->
    void testADDWithoutSlash() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter si;
        si.setSyntaxMode(false);
        si.addPath(GET_RESSOURCES_PATH("o3prmr/ADD"));

        TS_GUM_ASSERT_THROWS_NOTHING(
           si.interpretFile(GET_RESSOURCES_PATH("o3prmr/ADD/Request.o3prmr")));

        si.showElegantErrorsAndWarnings();
        TS_ASSERT_EQUALS(si.errors(), (gum::Size)0);
        TS_ASSERT_EQUALS(si.warnings(), (gum::Size)0);

        TS_ASSERT_EQUALS(si.results().size(), (gum::Size)1);

      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testADD() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter si;
        si.setSyntaxMode(false);
        si.addPath(GET_RESSOURCES_PATH("o3prmr/ADD/"));

        TS_GUM_ASSERT_THROWS_NOTHING(
           si.interpretFile(GET_RESSOURCES_PATH("o3prmr/ADD/Request.o3prmr")));

        si.showElegantErrorsAndWarnings();
        TS_ASSERT_EQUALS(si.errors(), (gum::Size)0);
        TS_ASSERT_EQUALS(si.warnings(), (gum::Size)0);

        TS_ASSERT_EQUALS(si.results().size(), (gum::Size)1);

        auto result = si.results()[0];

      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testOrAnd() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter si;
        si.setSyntaxMode(false);
        si.addPath(GET_RESSOURCES_PATH("o3prmr/aggregates/"));

        TS_GUM_ASSERT_THROWS_NOTHING(si.interpretFile(
           GET_RESSOURCES_PATH("o3prmr/aggregates/request.o3prmr")));

        si.showElegantErrorsAndWarnings();
        TS_ASSERT_EQUALS(si.errors(), (gum::Size)0);
        TS_ASSERT_EQUALS(si.warnings(), (gum::Size)0);

        TS_ASSERT_EQUALS(si.results().size(), (gum::Size)7);

        auto result = si.results()[0];

      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testStudents() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter si;
        si.setSyntaxMode(false);
        si.addPath(GET_RESSOURCES_PATH("o3prmr/University/"));

        TS_GUM_ASSERT_THROWS_NOTHING(si.interpretFile(
           GET_RESSOURCES_PATH("o3prmr/University/fr/request.o3prmr")));

        si.showElegantErrorsAndWarnings();
        TS_ASSERT_EQUALS(si.errors(), (gum::Size)0);
        TS_ASSERT_EQUALS(si.warnings(), (gum::Size)0);

        TS_ASSERT_EQUALS(si.results().size(), (gum::Size)1);

      } catch (gum::Exception&) { TS_ASSERT(false); }
    }
  };

}   // namespace gum_tests
