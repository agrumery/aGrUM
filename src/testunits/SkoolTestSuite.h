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

    /// Testing classes and interfaces
    void testComplexPrinters_1() {
      SkoolReader reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile("../../../src/testunits/ressources/skool/complexprinters.skool"));
      PRM* prm = reader.prm();
      TS_ASSERT_EQUALS(prm->classes().size(), 5);
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("agrum.test.PowerSupply"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("agrum.test.Room"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("agrum.test.BWPrinter"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("agrum.test.ColorPrinter"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("agrum.test.Computer"));
      TS_ASSERT_EQUALS(prm->interfaces().size(), 2);
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getInterface("agrum.test.Equipment"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getInterface("agrum.test.Printer"));
      delete prm;
    }

    /// Testing classes and interfaces inheritance with subtypes methods
    void testComplexPrinters_2() {
      SkoolReader reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile("../../../src/testunits/ressources/skool/complexprinters.skool"));
      PRM* prm = reader.prm();
      // Classes
      Class& PowerSupply = prm->getClass("agrum.test.PowerSupply");
      Class& Room = prm->getClass("agrum.test.Room");
      Class& BWPrinter = prm->getClass("agrum.test.BWPrinter");
      Class& ColorPrinter = prm->getClass("agrum.test.ColorPrinter");
      Class& Computer = prm->getClass("agrum.test.Computer");
      // Interfaces
      Interface& Equipment = prm->getInterface("agrum.test.Equipment");
      Interface& Printer = prm->getInterface("agrum.test.Printer");
      // Testing PowerSupply
      TS_ASSERT(PowerSupply.isSubTypeOf(PowerSupply));
      TS_ASSERT(not PowerSupply.isSubTypeOf(Room));
      TS_ASSERT(not PowerSupply.isSubTypeOf(Equipment));
      TS_ASSERT(not PowerSupply.isSubTypeOf(Printer));
      TS_ASSERT(not PowerSupply.isSubTypeOf(BWPrinter));
      TS_ASSERT(not PowerSupply.isSubTypeOf(ColorPrinter));
      TS_ASSERT(not PowerSupply.isSubTypeOf(Computer));
      // Testing Room
      TS_ASSERT(not Room.isSubTypeOf(PowerSupply));
      TS_ASSERT(Room.isSubTypeOf(Room));
      TS_ASSERT(not Room.isSubTypeOf(Equipment));
      TS_ASSERT(not Room.isSubTypeOf(Printer));
      TS_ASSERT(not Room.isSubTypeOf(BWPrinter));
      TS_ASSERT(not Room.isSubTypeOf(ColorPrinter));
      TS_ASSERT(not Room.isSubTypeOf(Computer));
      // Testing Equipment
      TS_ASSERT(not Equipment.isSubTypeOf(PowerSupply));
      TS_ASSERT(not Equipment.isSubTypeOf(Room));
      TS_ASSERT(Equipment.isSubTypeOf(Equipment));
      TS_ASSERT(not Equipment.isSubTypeOf(Printer));
      TS_ASSERT(not Equipment.isSubTypeOf(BWPrinter));
      TS_ASSERT(not Equipment.isSubTypeOf(ColorPrinter));
      TS_ASSERT(not Equipment.isSubTypeOf(Computer));
      // Testing Printer
      TS_ASSERT(not Printer.isSubTypeOf(PowerSupply));
      TS_ASSERT(not Printer.isSubTypeOf(Room));
      TS_ASSERT(Printer.isSubTypeOf(Equipment));
      TS_ASSERT(Printer.isSubTypeOf(Printer));
      TS_ASSERT(not Printer.isSubTypeOf(BWPrinter));
      TS_ASSERT(not Printer.isSubTypeOf(ColorPrinter));
      TS_ASSERT(not Printer.isSubTypeOf(Computer));
      // Testing BWPrinter
      TS_ASSERT(not BWPrinter.isSubTypeOf(PowerSupply));
      TS_ASSERT(not BWPrinter.isSubTypeOf(Room));
      TS_ASSERT(BWPrinter.isSubTypeOf(Equipment));
      TS_ASSERT(BWPrinter.isSubTypeOf(Printer));
      TS_ASSERT(BWPrinter.isSubTypeOf(BWPrinter));
      TS_ASSERT(not BWPrinter.isSubTypeOf(ColorPrinter));
      TS_ASSERT(not BWPrinter.isSubTypeOf(Computer));
      // Testing ColorPrinter
      TS_ASSERT(not ColorPrinter.isSubTypeOf(PowerSupply));
      TS_ASSERT(not ColorPrinter.isSubTypeOf(Room));
      TS_ASSERT(ColorPrinter.isSubTypeOf(Equipment));
      TS_ASSERT(ColorPrinter.isSubTypeOf(Printer));
      TS_ASSERT(not ColorPrinter.isSubTypeOf(BWPrinter));
      TS_ASSERT(ColorPrinter.isSubTypeOf(ColorPrinter));
      TS_ASSERT(not ColorPrinter.isSubTypeOf(Computer));
      // Testing Computer
      TS_ASSERT(not Computer.isSubTypeOf(PowerSupply));
      TS_ASSERT(not Computer.isSubTypeOf(Room));
      TS_ASSERT(Computer.isSubTypeOf(Equipment));
      TS_ASSERT(not Computer.isSubTypeOf(Printer));
      TS_ASSERT(not Computer.isSubTypeOf(BWPrinter));
      TS_ASSERT(not Computer.isSubTypeOf(ColorPrinter));
      TS_ASSERT(Computer.isSubTypeOf(Computer));
      delete prm;
    }

    /// Testing classes and interfaces inheritance with supertypes methods
    void testComplexPrinters_3() {
      SkoolReader reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile("../../../src/testunits/ressources/skool/complexprinters.skool"));
      PRM* prm = reader.prm();
      // Classes
      Class& PowerSupply = prm->getClass("agrum.test.PowerSupply");
      Class& Room = prm->getClass("agrum.test.Room");
      Class& BWPrinter = prm->getClass("agrum.test.BWPrinter");
      Class& ColorPrinter = prm->getClass("agrum.test.ColorPrinter");
      Class& Computer = prm->getClass("agrum.test.Computer");
      // Interfaces
      Interface& Equipment = prm->getInterface("agrum.test.Equipment");
      Interface& Printer = prm->getInterface("agrum.test.Printer");
      // Testing PowerSupply
      TS_ASSERT(PowerSupply.isSuperTypeOf(PowerSupply));
      TS_ASSERT(not PowerSupply.isSuperTypeOf(Room));
      TS_ASSERT(not PowerSupply.isSuperTypeOf(Equipment));
      TS_ASSERT(not PowerSupply.isSuperTypeOf(Printer));
      TS_ASSERT(not PowerSupply.isSuperTypeOf(BWPrinter));
      TS_ASSERT(not PowerSupply.isSuperTypeOf(ColorPrinter));
      TS_ASSERT(not PowerSupply.isSuperTypeOf(Computer));
      // Testing Room
      TS_ASSERT(not Room.isSuperTypeOf(PowerSupply));
      TS_ASSERT(Room.isSuperTypeOf(Room));
      TS_ASSERT(not Room.isSuperTypeOf(Equipment));
      TS_ASSERT(not Room.isSuperTypeOf(Printer));
      TS_ASSERT(not Room.isSuperTypeOf(BWPrinter));
      TS_ASSERT(not Room.isSuperTypeOf(ColorPrinter));
      TS_ASSERT(not Room.isSuperTypeOf(Computer));
      // Testing Equipment
      TS_ASSERT(not Equipment.isSuperTypeOf(PowerSupply));
      TS_ASSERT(not Equipment.isSuperTypeOf(Room));
      TS_ASSERT(Equipment.isSuperTypeOf(Equipment));
      TS_ASSERT(Equipment.isSuperTypeOf(Printer));
      TS_ASSERT(Equipment.isSuperTypeOf(BWPrinter));
      TS_ASSERT(Equipment.isSuperTypeOf(ColorPrinter));
      TS_ASSERT(Equipment.isSuperTypeOf(Computer));
      // Testing Printer
      TS_ASSERT(not Printer.isSuperTypeOf(PowerSupply));
      TS_ASSERT(not Printer.isSuperTypeOf(Room));
      TS_ASSERT(not Printer.isSuperTypeOf(Equipment));
      TS_ASSERT(Printer.isSuperTypeOf(Printer));
      TS_ASSERT(Printer.isSuperTypeOf(BWPrinter));
      TS_ASSERT(Printer.isSuperTypeOf(ColorPrinter));
      TS_ASSERT(not Printer.isSuperTypeOf(Computer));
      // Testing BWPrinter
      TS_ASSERT(not BWPrinter.isSuperTypeOf(PowerSupply));
      TS_ASSERT(not BWPrinter.isSuperTypeOf(Room));
      TS_ASSERT(not BWPrinter.isSuperTypeOf(Equipment));
      TS_ASSERT(not BWPrinter.isSuperTypeOf(Printer));
      TS_ASSERT(BWPrinter.isSuperTypeOf(BWPrinter));
      TS_ASSERT(not BWPrinter.isSuperTypeOf(ColorPrinter));
      TS_ASSERT(not BWPrinter.isSuperTypeOf(Computer));
      // Testing ColorPrinter
      TS_ASSERT(not ColorPrinter.isSuperTypeOf(PowerSupply));
      TS_ASSERT(not ColorPrinter.isSuperTypeOf(Room));
      TS_ASSERT(not ColorPrinter.isSuperTypeOf(Equipment));
      TS_ASSERT(not ColorPrinter.isSuperTypeOf(Printer));
      TS_ASSERT(not ColorPrinter.isSuperTypeOf(BWPrinter));
      TS_ASSERT(ColorPrinter.isSuperTypeOf(ColorPrinter));
      TS_ASSERT(not ColorPrinter.isSuperTypeOf(Computer));
      // Testing Computer
      TS_ASSERT(not Computer.isSuperTypeOf(PowerSupply));
      TS_ASSERT(not Computer.isSuperTypeOf(Room));
      TS_ASSERT(not Computer.isSuperTypeOf(Equipment));
      TS_ASSERT(not Computer.isSuperTypeOf(Printer));
      TS_ASSERT(not Computer.isSuperTypeOf(BWPrinter));
      TS_ASSERT(not Computer.isSuperTypeOf(ColorPrinter));
      TS_ASSERT(Computer.isSuperTypeOf(Computer));
      delete prm;
    }
};

} // namespace tests
} // namespace gum
// ============================================================================
