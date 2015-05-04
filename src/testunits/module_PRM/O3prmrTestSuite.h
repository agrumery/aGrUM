/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/PRM/o3prmr/O3prmrInterpreter.h>

namespace gum_tests {

  class O3prmrTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {
      // std::cerr << std::endl;
    }

    void tearDown() {}

    void testParseFile1() {
      gum::prm::o3prmr::O3prmrInterpreter *si =
          new gum::prm::o3prmr::O3prmrInterpreter();
      si->setSyntaxMode(true);
      si->addPath("../../../src/testunits/ressources/o3prmr/");

      TS_GUM_ASSERT_THROWS_NOTHING(si->interpretFile(
          "../../../src/testunits/ressources/o3prmr/requests/query1.o3prmr"));

      TS_ASSERT_EQUALS(si->errors(), 0);
      TS_ASSERT_EQUALS(si->warnings(), 0);

      delete si;
    }

    void testParseFile2() {
      gum::prm::o3prmr::O3prmrInterpreter *si =
          new gum::prm::o3prmr::O3prmrInterpreter();
      si->setSyntaxMode(true);
      si->addPath("../../../src/testunits/ressources/o3prmr/");

      TS_GUM_ASSERT_THROWS_NOTHING(si->interpretFile(
          "../../../src/testunits/ressources/o3prmr/requests/query2.o3prmr"));

      TS_ASSERT_EQUALS(si->errors(), 1);
      TS_ASSERT_EQUALS(si->warnings(), 0);

      delete si;
    }

    void testInference() {
      gum::prm::o3prmr::O3prmrInterpreter *si =
          new gum::prm::o3prmr::O3prmrInterpreter();
      si->setSyntaxMode(false);
      si->addPath("../../../src/testunits/ressources/o3prmr/");

      TS_GUM_ASSERT_THROWS_NOTHING(si->interpretFile(
          "../../../src/testunits/ressources/o3prmr/requests/query1.o3prmr"));

      TS_ASSERT_EQUALS(si->errors(), 0);
      TS_ASSERT_EQUALS(si->warnings(), 0);

      delete si;
    }

    void testObserve() {
      gum::prm::o3prmr::O3prmrInterpreter *si =
          new gum::prm::o3prmr::O3prmrInterpreter();
      si->setSyntaxMode(false);
      si->addPath("../../../src/testunits/ressources/o3prmr/");

      TS_GUM_ASSERT_THROWS_NOTHING(
          si->interpretFile("../../../src/testunits/ressources/o3prmr/requests/"
                            "queryObserveTest.o3prmr"));

      TS_ASSERT_EQUALS(si->errors(), 0);
      TS_ASSERT_EQUALS(si->warnings(), 0);

      // Observe correctly
      const gum::prm::Instance<double> &c1 =
          si->prm()->system("systems.MyKickAssSystem").get("c1");
      const gum::prm::Instance<double> &c2 =
          si->prm()->system("systems.MyKickAssSystem").get("c2");

      TS_ASSERT(si->inference()->hasEvidence(
          gum::prm::PRMInference<double>::Chain(&c1, &c1.get("can_print"))));
      TS_ASSERT(si->inference()->hasEvidence(
          gum::prm::PRMInference<double>::Chain(&c2, &c2.get("equipState"))));
      TS_ASSERT(!si->inference()->hasEvidence(
          gum::prm::PRMInference<double>::Chain(&c1, &c1.get("equipState"))));
      TS_ASSERT(!si->inference()->hasEvidence(
          gum::prm::PRMInference<double>::Chain(&c2, &c2.get("can_print"))));

      const gum::prm::PRMInference<double>::EMap &e = si->inference()->evidence(c2);
      const gum::Potential<double> *p = e[c1.get("equipState").id()];

      gum::Instantiation j(*p);
      const gum::prm::Attribute<double> &c2_equipState = c2.get("equipState");

      for (j.setFirst(); !j.end(); j.inc()) {
        if (c2_equipState.type().variable().label(
                j.val(c2_equipState.type().variable())) == "Dysfunctional") {
          TS_ASSERT_EQUALS(p->get(j), 1.0);
        } else {
          TS_ASSERT_EQUALS(p->get(j), 0.0);
        }
      }

      delete si;
    }

    void testUnobserve() {
      gum::prm::o3prmr::O3prmrInterpreter *si =
          new gum::prm::o3prmr::O3prmrInterpreter();
      si->setSyntaxMode(false);
      si->addPath("../../../src/testunits/ressources/o3prmr/");

      TS_GUM_ASSERT_THROWS_NOTHING(
          si->interpretFile("../../../src/testunits/ressources/o3prmr/requests/"
                            "queryUnobserveTest.o3prmr"));

      TS_ASSERT_EQUALS(si->errors(), 0);
      TS_ASSERT_EQUALS(si->warnings(), 0);

      // Unobserve correctly
      const gum::prm::Instance<double> &c1 =
          si->prm()->system("systems.MyKickAssSystem").get("c1");
      const gum::prm::Instance<double> &c2 =
          si->prm()->system("systems.MyKickAssSystem").get("c2");

      TS_ASSERT(!si->inference()->hasEvidence(
          gum::prm::PRMInference<double>::Chain(&c1, &c1.get("can_print"))));
      TS_ASSERT(!si->inference()->hasEvidence(
          gum::prm::PRMInference<double>::Chain(&c2, &c2.get("equipState"))));
      TS_ASSERT(!si->inference()->hasEvidence(
          gum::prm::PRMInference<double>::Chain(&c1, &c1.get("equipState"))));
      TS_ASSERT(!si->inference()->hasEvidence(
          gum::prm::PRMInference<double>::Chain(&c2, &c2.get("can_print"))));

      delete si;
    }

    void testQuery() {
      gum::prm::o3prmr::O3prmrInterpreter *si =
          new gum::prm::o3prmr::O3prmrInterpreter();
      si->setSyntaxMode(false);
      si->addPath("../../../src/testunits/ressources/o3prmr/");

      TS_GUM_ASSERT_THROWS_NOTHING(si->interpretFile(
          "../../../src/testunits/ressources/o3prmr/requests/query1.o3prmr"));

      si->showElegantErrorsAndWarnings();
      TS_ASSERT_EQUALS(si->errors(), 0);
      TS_ASSERT_EQUALS(si->warnings(), 0);

      const gum::prm::Instance<double> &c1 =
          si->prm()->system("systems.MyKickAssSystem").get("c1");
      const gum::prm::Instance<double> &c2 =
          si->prm()->system("systems.MyKickAssSystem").get("c2");

      TS_ASSERT(!si->inference()->hasEvidence(
          gum::prm::PRMInference<double>::Chain(&c1, &c1.get("can_print"))));
      TS_ASSERT(si->inference()->hasEvidence(
          gum::prm::PRMInference<double>::Chain(&c2, &c2.get("equipState"))));
      TS_ASSERT(!si->inference()->hasEvidence(
          gum::prm::PRMInference<double>::Chain(&c1, &c1.get("equipState"))));
      TS_ASSERT(!si->inference()->hasEvidence(
          gum::prm::PRMInference<double>::Chain(&c2, &c2.get("can_print"))));

      // Est-ce que la valeur Dysfonctionnal de l'attribut can_print de l'instance c1
      // est à 1.0

      const gum::prm::PRMInference<double>::EMap &e = si->inference()->evidence(c2);
      const gum::Potential<double> *p = e[c1.get("equipState").id()];

      gum::Instantiation j(*p);
      const gum::prm::Attribute<double> &c2_equipState = c2.get("equipState");

      for (j.setFirst(); !j.end(); j.inc()) {
        if (c2_equipState.type().variable().label(
                j.val(c2_equipState.type().variable())) == "Dysfunctional") {
          TS_ASSERT_EQUALS(p->get(j), 1.0);
        } else {
          TS_ASSERT_EQUALS(p->get(j), 0.0);
        }
      }

      delete si;
    }
  };

} // namespace gum_tests
