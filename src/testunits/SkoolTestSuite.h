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
// ============================================================================
#include <cxxtest/AgrumTestSuite.h>
// ============================================================================
#include <agrum/prm/skool/SkoolReader.h>
// ============================================================================

namespace gum {
namespace tests {

using namespace gum;
using namespace gum::prm;
using namespace gum::prm::skool;

class SkoolTestSuite: public CxxTest::TestSuite {
  public:
    void setUp() {
    }

    void tearDown() {
    }

    void testTypes() {
      SkoolReader reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile("../../../src/testunits/ressources/skool/types.skool"));
      PRM* prm = reader.prm();
      TS_ASSERT_EQUALS(prm->getType("agrum.test.t_state").variable().domainSize(), 2);
      TS_ASSERT_EQUALS(prm->getType("agrum.test.t_ink").variable().domainSize(), 2);
      TS_ASSERT_EQUALS(prm->getType("agrum.test.t_degraded").variable().domainSize(), 3);
      TS_ASSERT_EQUALS(prm->getType("agrum.test.t_bw_p").variable().domainSize(), 4);
      TS_ASSERT_EQUALS(prm->getType("agrum.test.t_color_p").variable().domainSize(), 5);
      delete prm;
    }

    void testSubTypes() {
      SkoolReader reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile("../../../src/testunits/ressources/skool/types.skool"));
      PRM* prm = reader.prm();
      TS_ASSERT(prm->getType("agrum.test.t_state").isSubType());
      TS_ASSERT(prm->getType("agrum.test.t_state").isSubTypeOf(prm->getType("boolean")));
      TS_ASSERT(prm->getType("agrum.test.t_ink").isSubType());
      TS_ASSERT(prm->getType("agrum.test.t_ink").isSubTypeOf(prm->getType("boolean")));
      TS_ASSERT(prm->getType("agrum.test.t_ink").isSubTypeOf(prm->getType("agrum.test.t_state")));
      TS_ASSERT(prm->getType("agrum.test.t_paper").isSubType());
      TS_ASSERT(prm->getType("agrum.test.t_paper").isSubTypeOf(prm->getType("boolean")));
      TS_ASSERT(prm->getType("agrum.test.t_paper").isSubTypeOf(prm->getType("agrum.test.t_state")));
      TS_ASSERT(prm->getType("agrum.test.t_degraded").isSubType());
      TS_ASSERT(prm->getType("agrum.test.t_degraded").isSubTypeOf(prm->getType("boolean")));
      TS_ASSERT(prm->getType("agrum.test.t_degraded").isSubTypeOf(prm->getType("agrum.test.t_state")));
      TS_ASSERT(prm->getType("agrum.test.t_bw_p").isSubType());
      TS_ASSERT(prm->getType("agrum.test.t_bw_p").isSubTypeOf(prm->getType("boolean")));
      TS_ASSERT(prm->getType("agrum.test.t_bw_p").isSubTypeOf(prm->getType("agrum.test.t_state")));
      TS_ASSERT(prm->getType("agrum.test.t_bw_p").isSubTypeOf(prm->getType("agrum.test.t_degraded")));
      TS_ASSERT(prm->getType("agrum.test.t_color_p").isSubType());
      TS_ASSERT(prm->getType("agrum.test.t_color_p").isSubTypeOf(prm->getType("boolean")));
      TS_ASSERT(prm->getType("agrum.test.t_color_p").isSubTypeOf(prm->getType("agrum.test.t_state")));
      TS_ASSERT(prm->getType("agrum.test.t_color_p").isSubTypeOf(prm->getType("agrum.test.t_degraded")));
      TS_ASSERT_EQUALS(prm->types().size(), 7);
      delete prm;
    }

    void testClasses() {
      SkoolReader reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile("../../../src/testunits/ressources/skool/printers.skool"));
      PRM* prm = reader.prm();
      TS_ASSERT_EQUALS(prm->classes().size(), 5);
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("agrum.test.PowerSupply"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("agrum.test.Room"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("agrum.test.Equipment"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("agrum.test.Printer"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("agrum.test.Computer"));
      delete prm;
    }

    void testPrintersPowerSupply() {
      SkoolReader reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile("../../../src/testunits/ressources/skool/printers.skool"));
      PRM* prm = reader.prm();
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("agrum.test.PowerSupply"));
      Class& c = prm->getClass("agrum.test.PowerSupply");
      TS_ASSERT_EQUALS(c.attributes().size(), 1);
      TS_ASSERT_EQUALS(c.referenceSlots().size(), 0);
      TS_ASSERT_EQUALS(c.aggregates().size(), 0);
      TS_ASSERT_EQUALS(c.slotChains().size(), 0);
      delete prm;
    }

    void testPrintersRoom() {
      SkoolReader reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile("../../../src/testunits/ressources/skool/printers.skool"));
      PRM* prm = reader.prm();
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("agrum.test.Room"));
      Class& c = prm->getClass("agrum.test.Room");
      TS_ASSERT_EQUALS(c.attributes().size(), 0);
      TS_ASSERT_EQUALS(c.referenceSlots().size(), 1);
      TS_ASSERT_EQUALS(c.aggregates().size(), 0);
      TS_ASSERT_EQUALS(c.slotChains().size(), 0);
      delete prm;
    }

    void testPrintersEquipment() {
      SkoolReader reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile("../../../src/testunits/ressources/skool/printers.skool"));
      PRM* prm = reader.prm();
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("agrum.test.Equipment"));
      Class& c = prm->getClass("agrum.test.Equipment");
      TS_ASSERT_EQUALS(c.attributes().size(), 1);
      TS_ASSERT_EQUALS(c.referenceSlots().size(), 1);
      TS_ASSERT_EQUALS(c.aggregates().size(), 0);
      TS_ASSERT_EQUALS(c.slotChains().size(), 1);
      delete prm;
    }

    void testPrintersPrinter() {
      SkoolReader reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile("../../../src/testunits/ressources/skool/printers.skool"));
      PRM* prm = reader.prm();
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("agrum.test.Printer"));
      Class& c = prm->getClass("agrum.test.Printer");
      TS_ASSERT_EQUALS(c.attributes().size(), 3);
      TS_ASSERT_EQUALS(c.referenceSlots().size(), 1);
      TS_ASSERT_EQUALS(c.aggregates().size(), 0);
      TS_ASSERT_EQUALS(c.slotChains().size(), 1);
      delete prm;
    }

    void testPrintersComputer() {
      SkoolReader reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile("../../../src/testunits/ressources/skool/printers.skool"));
      PRM* prm = reader.prm();
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("agrum.test.Computer"));
      Class& c = prm->getClass("agrum.test.Computer");
      TS_ASSERT_EQUALS(c.attributes().size(), 2);
      TS_ASSERT_EQUALS(c.referenceSlots().size(), 2);
      TS_ASSERT_EQUALS(c.aggregates().size(), 1);
      TS_ASSERT_EQUALS(c.slotChains().size(), 2);
      delete prm;
    }

    void testComplexPrinters() {
      SkoolReader reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile("../../../src/testunits/ressources/skool/complexprinters.skool"));
      PRM* prm = reader.prm();
      delete prm;
    }
};

} // namespace tests
} // namespace gum
// ============================================================================
