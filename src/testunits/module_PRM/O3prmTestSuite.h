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

#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum_tests {

  class O3prmTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {
      // std::cerr << std::endl;
    }

    void tearDown() {}

    void testTypes() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(
          reader.readFile("../../../src/testunits/ressources/o3prm/types.o3prm"));
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);
      gum::prm::PRM<double> *prm = reader.prm();
      TS_ASSERT_EQUALS(prm->type("t_state").variable().domainSize(), (gum::Size)2);
      TS_ASSERT_EQUALS(prm->type("t_ink").variable().domainSize(), (gum::Size)2);
      TS_ASSERT_EQUALS(prm->type("t_degraded").variable().domainSize(),
                       (gum::Size)3);
      TS_ASSERT_EQUALS(prm->type("t_bw_p").variable().domainSize(), (gum::Size)4);
      TS_ASSERT_EQUALS(prm->type("t_color_p").variable().domainSize(), (gum::Size)5);
      delete prm;
    }

    void testSubTypes() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(
          reader.readFile("../../../src/testunits/ressources/o3prm/types.o3prm"));
      gum::prm::PRM<double> *prm = reader.prm();
      TS_ASSERT(prm->type("t_state").isSubType());
      TS_ASSERT(prm->type("t_state").isSubTypeOf(prm->type("boolean")));
      TS_ASSERT(prm->type("t_ink").isSubType());
      TS_ASSERT(prm->type("t_ink").isSubTypeOf(prm->type("boolean")));
      TS_ASSERT(prm->type("t_ink").isSubTypeOf(prm->type("t_state")));
      TS_ASSERT(prm->type("t_paper").isSubType());
      TS_ASSERT(prm->type("t_paper").isSubTypeOf(prm->type("boolean")));
      TS_ASSERT(prm->type("t_paper").isSubTypeOf(prm->type("t_state")));
      TS_ASSERT(prm->type("t_degraded").isSubType());
      TS_ASSERT(prm->type("t_degraded").isSubTypeOf(prm->type("boolean")));
      TS_ASSERT(prm->type("t_degraded").isSubTypeOf(prm->type("t_state")));
      TS_ASSERT(prm->type("t_bw_p").isSubType());
      TS_ASSERT(prm->type("t_bw_p").isSubTypeOf(prm->type("boolean")));
      TS_ASSERT(prm->type("t_bw_p").isSubTypeOf(prm->type("t_state")));
      TS_ASSERT(prm->type("t_bw_p").isSubTypeOf(prm->type("t_degraded")));
      TS_ASSERT(prm->type("t_color_p").isSubType());
      TS_ASSERT(prm->type("t_color_p").isSubTypeOf(prm->type("boolean")));
      TS_ASSERT(prm->type("t_color_p").isSubTypeOf(prm->type("t_state")));
      TS_ASSERT(prm->type("t_color_p").isSubTypeOf(prm->type("t_degraded")));
      TS_ASSERT_EQUALS(prm->types().size(), (gum::Size)7);
      delete prm;
    }

    void testClasses() {
      gum::prm::o3prm::O3prmReader<double> reader;
      // reader.readFile("../../../src/testunits/ressources/o3prm/printers.o3prm");
      TS_GUM_ASSERT_THROWS_NOTHING(
          reader.readFile("../../../src/testunits/ressources/o3prm/printers.o3prm"));
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);
      gum::prm::PRM<double> *prm = reader.prm();
      TS_ASSERT_EQUALS(prm->classes().size(), (gum::Size)5);
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("PowerSupply"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Room"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Equipment"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Printer"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Computer"));
      delete prm;
    }

    void testPrintersPowerSupply() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(
          reader.readFile("../../../src/testunits/ressources/o3prm/printers.o3prm"));
      gum::prm::PRM<double> *prm = reader.prm();
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("PowerSupply"));
      gum::prm::Class<double> &c = prm->getClass("PowerSupply");
      TS_ASSERT_EQUALS(c.attributes().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(c.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(c.aggregates().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(c.slotChains().size(), (gum::Size)0);
      delete prm;
    }

    void testPrintersRoom() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(
          reader.readFile("../../../src/testunits/ressources/o3prm/printers.o3prm"));
      gum::prm::PRM<double> *prm = reader.prm();
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Room"));
      gum::prm::Class<double> &c = prm->getClass("Room");
      TS_ASSERT_EQUALS(c.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(c.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(c.aggregates().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(c.slotChains().size(), (gum::Size)0);
      delete prm;
    }

    void testPrintersEquipment() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(
          reader.readFile("../../../src/testunits/ressources/o3prm/printers.o3prm"));
      gum::prm::PRM<double> *prm = reader.prm();
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Equipment"));
      gum::prm::Class<double> &c = prm->getClass("Equipment");
      TS_ASSERT_EQUALS(c.attributes().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(c.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(c.aggregates().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(c.slotChains().size(), (gum::Size)1);
      delete prm;
    }

    void testPrintersPrinter() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(
          reader.readFile("../../../src/testunits/ressources/o3prm/printers.o3prm"));
      gum::prm::PRM<double> *prm = reader.prm();
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Printer"));
      gum::prm::Class<double> &c = prm->getClass("Printer");
      TS_ASSERT_EQUALS(c.attributes().size(), (gum::Size)3);
      TS_ASSERT_EQUALS(c.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(c.aggregates().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(c.slotChains().size(), (gum::Size)1);
      delete prm;
    }

    void testPrintersComputer() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(
          reader.readFile("../../../src/testunits/ressources/o3prm/printers.o3prm"));
      gum::prm::PRM<double> *prm = reader.prm();
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Computer"));
      gum::prm::Class<double> &c = prm->getClass("Computer");
      TS_ASSERT_EQUALS(c.attributes().size(), (gum::Size)2);
      TS_ASSERT_EQUALS(c.referenceSlots().size(), (gum::Size)2);
      TS_ASSERT_EQUALS(c.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(c.slotChains().size(), (gum::Size)2);
      delete prm;
    }

    /// Testing classes and interfaces
    void testComplexPrinters_1() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(
          "../../../src/testunits/ressources/o3prm/complexprinters.o3prm"));
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);
      gum::prm::PRM<double> *prm = reader.prm();
      TS_ASSERT_EQUALS(prm->classes().size(), (gum::Size)6);
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("PowerSupply"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Room"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("BWPrinter"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("ColorPrinter"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Computer"));
      TS_ASSERT_EQUALS(prm->interfaces().size(), (gum::Size)2);
      TS_GUM_ASSERT_THROWS_NOTHING(prm->interface("Equipment"));
      TS_GUM_ASSERT_THROWS_NOTHING(prm->interface("Printer"));
      delete prm;
    }

    /// Testing classes and interfaces inheritance with subtypes methods
    void testComplexPrinters_2() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(
          "../../../src/testunits/ressources/o3prm/complexprinters.o3prm"));
      gum::prm::PRM<double> *prm = reader.prm();
      // Classes
      gum::prm::Class<double> &PowerSupply = prm->getClass("PowerSupply");
      gum::prm::Class<double> &Room = prm->getClass("Room");
      gum::prm::Class<double> &BWPrinter = prm->getClass("BWPrinter");
      gum::prm::Class<double> &ColorPrinter = prm->getClass("ColorPrinter");
      gum::prm::Class<double> &Computer = prm->getClass("Computer");
      // Interfaces
      gum::prm::Interface<double> &Equipment = prm->interface("Equipment");
      gum::prm::Interface<double> &Printer = prm->interface("Printer");
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
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(
          "../../../src/testunits/ressources/o3prm/complexprinters.o3prm"));
      gum::prm::PRM<double> *prm = reader.prm();
      // Classes
      gum::prm::Class<double> &PowerSupply = prm->getClass("PowerSupply");
      gum::prm::Class<double> &Room = prm->getClass("Room");
      gum::prm::Class<double> &BWPrinter = prm->getClass("BWPrinter");
      gum::prm::Class<double> &ColorPrinter = prm->getClass("ColorPrinter");
      gum::prm::Class<double> &Computer = prm->getClass("Computer");
      // Interfaces
      gum::prm::Interface<double> &Equipment = prm->interface("Equipment");
      gum::prm::Interface<double> &Printer = prm->interface("Printer");
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

    // Testing class PowerSupply
    void testComplexPrinters_4() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(
          "../../../src/testunits/ressources/o3prm/complexprinters.o3prm"));
      gum::prm::PRM<double> *prm = reader.prm();
      // Classes
      gum::prm::Class<double> &PowerSupply = prm->getClass("PowerSupply");
      TS_ASSERT_EQUALS(PowerSupply.attributes().size(), (gum::Size)2);
      TS_ASSERT_EQUALS(PowerSupply.dag().sizeArcs(), (gum::Size)1);
      TS_GUM_ASSERT_THROWS_NOTHING(PowerSupply["(boolean)state"]);
      TS_GUM_ASSERT_THROWS_NOTHING(PowerSupply["state"]);
      TS_ASSERT(PowerSupply["state"].type().isSubTypeOf(
          PowerSupply["(boolean)state"].type()));
      gum::NodeId n1 = 0;
      gum::NodeId n2 = 0;
      gum::NodeId n3 = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(n1 = PowerSupply["(boolean)state"].id());
      TS_GUM_ASSERT_THROWS_NOTHING(n2 = PowerSupply["state"].id());
      TS_GUM_ASSERT_THROWS_NOTHING(n3 = PowerSupply["(t_state)state"].id());
      TS_ASSERT_EQUALS(n2, n3);
      TS_ASSERT(PowerSupply.dag().existsArc(n2, n1));
      TS_ASSERT_EQUALS(PowerSupply.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(PowerSupply.aggregates().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(PowerSupply.slotChains().size(), (gum::Size)0);
      // Testing interfaces for the state attribute and its cast descendant
      TS_ASSERT(not PowerSupply.isOutputNode(PowerSupply["(boolean)state"]));
      TS_ASSERT(not PowerSupply.isInputNode(PowerSupply["(boolean)state"]));
      TS_ASSERT(PowerSupply.isInnerNode(PowerSupply["(boolean)state"]));
      TS_ASSERT(PowerSupply.isOutputNode(PowerSupply["state"]));
      TS_ASSERT(not PowerSupply.isInputNode(PowerSupply["state"]));
      TS_ASSERT(not PowerSupply.isInnerNode(PowerSupply["state"]));
      TS_ASSERT(PowerSupply.isOutputNode(PowerSupply["(t_state)state"]));
      TS_ASSERT(not PowerSupply.isInputNode(PowerSupply["(t_state)state"]));
      TS_ASSERT(not PowerSupply.isInnerNode(PowerSupply["(t_state)state"]));
      delete prm;
    }

    // Testing class Room
    void testComplexPrinters_5() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(
          "../../../src/testunits/ressources/o3prm/complexprinters.o3prm"));
      gum::prm::PRM<double> *prm = reader.prm();
      gum::prm::Class<double> &Room = prm->getClass("Room");
      TS_ASSERT_EQUALS(Room.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(Room.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(Room.aggregates().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(Room.slotChains().size(), (gum::Size)0);
      delete prm;
    }

    // Testing interface Equipment
    void testComplexPrinters_6() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(
          "../../../src/testunits/ressources/o3prm/complexprinters.o3prm"));
      gum::prm::PRM<double> *prm = reader.prm();
      gum::prm::Interface<double> &Equipment = prm->interface("Equipment");
      TS_ASSERT_EQUALS(Equipment.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(Equipment.attributes().size(), (gum::Size)3);
      TS_GUM_ASSERT_THROWS_NOTHING(Equipment["equipState"]);
      TS_ASSERT_EQUALS(Equipment["equipState"].type().name(), "t_degraded");
      TS_GUM_ASSERT_THROWS_NOTHING(Equipment["(t_state)equipState"]);
      TS_ASSERT_EQUALS(Equipment["(t_state)equipState"].type().name(), "t_state");
      TS_GUM_ASSERT_THROWS_NOTHING(Equipment["(boolean)equipState"]);
      TS_ASSERT_EQUALS(Equipment["(boolean)equipState"].type().name(), "boolean");
      // Testing gum::NodeId, inputs and outputs
      gum::NodeId st = Equipment["equipState"].id();
      gum::NodeId deg_st = Equipment["(t_degraded)equipState"].id();
      gum::NodeId st_st = Equipment["(t_state)equipState"].id();
      gum::NodeId bool_st = Equipment["(boolean)equipState"].id();
      TS_ASSERT_EQUALS(st, deg_st);
      TS_ASSERT_DIFFERS(st, st_st);
      TS_ASSERT_DIFFERS(st, bool_st);
      TS_ASSERT_DIFFERS(st_st, bool_st);
      // t_degraded equipState
      TS_ASSERT(not Equipment.isOutputNode(Equipment["equipState"]));
      TS_ASSERT(not Equipment.isInputNode(Equipment["equipState"]));
      TS_ASSERT(Equipment.isInnerNode(Equipment["equipState"]));
      // t_state equipState
      TS_ASSERT(not Equipment.isOutputNode(Equipment["(t_state)equipState"]));
      TS_ASSERT(not Equipment.isInputNode(Equipment["(t_state)equipState"]));
      TS_ASSERT(Equipment.isInnerNode(Equipment["(t_state)equipState"]));
      // boolean equipState
      TS_ASSERT(not Equipment.isOutputNode(Equipment["(boolean)equipState"]));
      TS_ASSERT(not Equipment.isInputNode(Equipment["(boolean)equipState"]));
      TS_ASSERT(Equipment.isInnerNode(Equipment["(boolean)equipState"]));
      delete prm;
    }

    // Testing interface Printer
    void testComplexPrinters_7() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(
          "../../../src/testunits/ressources/o3prm/complexprinters.o3prm"));
      gum::prm::PRM<double> *prm = reader.prm();
      TS_GUM_ASSERT_THROWS_NOTHING(prm->interface("Printer"));
      gum::prm::Interface<double> &Printer = prm->interface("Printer");
      TS_ASSERT_EQUALS(Printer.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(Printer.attributes().size(), (gum::Size)5);
      TS_GUM_ASSERT_THROWS_NOTHING(Printer["room"]);
      TS_GUM_ASSERT_THROWS_NOTHING(Printer["equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(Printer["hasPaper"]);
      TS_GUM_ASSERT_THROWS_NOTHING(Printer["hasInk"]);
      TS_ASSERT_EQUALS(Printer["equipState"].type().name(), "t_degraded");
      TS_ASSERT_EQUALS(Printer["hasPaper"].type().name(), "boolean");
      TS_ASSERT_EQUALS(Printer["hasInk"].type().name(), "boolean");
      // Testing gum::NodeId, inputs and outputs
      gum::NodeId st = Printer["equipState"].id();
      gum::NodeId deg_st = Printer["(t_degraded)equipState"].id();
      gum::NodeId st_st = Printer["(t_state)equipState"].id();
      gum::NodeId bool_st = Printer["(boolean)equipState"].id();
      gum::NodeId hp = Printer["hasPaper"].id();
      gum::NodeId bool_hp = Printer["(boolean)hasPaper"].id();
      gum::NodeId hi = Printer["hasInk"].id();
      gum::NodeId bool_hi = Printer["(boolean)hasInk"].id();
      TS_ASSERT_EQUALS(st, deg_st);
      TS_ASSERT_DIFFERS(st, st_st);
      TS_ASSERT_DIFFERS(st, bool_st);
      TS_ASSERT_DIFFERS(st_st, bool_st);
      TS_ASSERT_EQUALS(hp, bool_hp);
      TS_ASSERT_EQUALS(hi, bool_hi);
      // t_degraded equipState
      TS_ASSERT(Printer.isOutputNode(Printer["(t_degraded)equipState"]));
      TS_ASSERT(not Printer.isInputNode(Printer["(t_degraded)equipState"]));
      TS_ASSERT(not Printer.isInnerNode(Printer["(t_degraded)equipState"]));
      // t_state equipState
      TS_ASSERT(Printer.isOutputNode(Printer["(t_state)equipState"]));
      TS_ASSERT(not Printer.isInputNode(Printer["(t_state)equipState"]));
      TS_ASSERT(not Printer.isInnerNode(Printer["(t_state)equipState"]));
      // boolean equipState
      TS_ASSERT(not Printer.isOutputNode(Printer["(boolean)equipState"]));
      TS_ASSERT(not Printer.isInputNode(Printer["(boolean)equipState"]));
      TS_ASSERT(Printer.isInnerNode(Printer["(boolean)equipState"]));
      // Boolean hasPaper
      TS_ASSERT(not Printer.isOutputNode(Printer["hasPaper"]));
      TS_ASSERT(not Printer.isInputNode(Printer["hasPaper"]));
      TS_ASSERT(Printer.isInnerNode(Printer["hasPaper"]));
      // Boolean hasInk
      TS_ASSERT(not Printer.isOutputNode(Printer["hasInk"]));
      TS_ASSERT(not Printer.isInputNode(Printer["hasInk"]));
      TS_ASSERT(Printer.isInnerNode(Printer["hasInk"]));
      delete prm;
    }

    // Testing class BWPrinter
    void testComplexPrinters_8() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(
          "../../../src/testunits/ressources/o3prm/complexprinters.o3prm"));
      gum::prm::PRM<double> *prm = reader.prm();
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("BWPrinter"));
      gum::prm::Class<double> &BWPrinter = prm->getClass("BWPrinter");
      TS_ASSERT_EQUALS(BWPrinter.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(BWPrinter.attributes().size(), (gum::Size)9);
      TS_ASSERT_EQUALS(BWPrinter.slotChains().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(BWPrinter.aggregates().size(), (gum::Size)0);
      TS_GUM_ASSERT_THROWS_NOTHING(BWPrinter["room"]);
      TS_GUM_ASSERT_THROWS_NOTHING(BWPrinter["(boolean)equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(BWPrinter["(t_state)equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(BWPrinter["(t_degraded)equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(BWPrinter["equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(BWPrinter["(boolean)hasPaper"]);
      TS_GUM_ASSERT_THROWS_NOTHING(BWPrinter["(t_state)hasPaper"]);
      TS_GUM_ASSERT_THROWS_NOTHING(BWPrinter["(t_paper)hasPaper"]);
      TS_GUM_ASSERT_THROWS_NOTHING(BWPrinter["hasPaper"]);
      TS_GUM_ASSERT_THROWS_NOTHING(BWPrinter["(boolean)hasInk"]);
      TS_GUM_ASSERT_THROWS_NOTHING(BWPrinter["(t_state)hasInk"]);
      TS_GUM_ASSERT_THROWS_NOTHING(BWPrinter["(t_ink)hasInk"]);
      TS_GUM_ASSERT_THROWS_NOTHING(BWPrinter["hasInk"]);
      // Testing gum::NodeId, inputs and outputs
      gum::NodeId st = BWPrinter["equipState"].id();
      gum::NodeId deg_st = BWPrinter["(t_degraded)equipState"].id();
      gum::NodeId st_st = BWPrinter["(t_state)equipState"].id();
      gum::NodeId bool_st = BWPrinter["(boolean)equipState"].id();
      gum::NodeId hp = BWPrinter["hasPaper"].id();
      gum::NodeId bool_hp = BWPrinter["(boolean)hasPaper"].id();
      gum::NodeId st_hp = BWPrinter["(t_state)hasPaper"].id();
      gum::NodeId p_hp = BWPrinter["(t_paper)hasPaper"].id();
      gum::NodeId hi = BWPrinter["hasInk"].id();
      gum::NodeId bool_hi = BWPrinter["(boolean)hasInk"].id();
      gum::NodeId st_hi = BWPrinter["(t_state)hasInk"].id();
      gum::NodeId i_hi = BWPrinter["(t_ink)hasInk"].id();
      TS_ASSERT_EQUALS(st, deg_st);
      TS_ASSERT_DIFFERS(st, st_st);
      TS_ASSERT_DIFFERS(st, bool_st);
      TS_ASSERT_DIFFERS(st_st, bool_st);
      TS_ASSERT_EQUALS(hp, p_hp);
      TS_ASSERT_DIFFERS(hp, st_hp);
      TS_ASSERT_DIFFERS(hp, bool_hp);
      TS_ASSERT_EQUALS(hi, i_hi);
      TS_ASSERT_DIFFERS(hi, st_hi);
      TS_ASSERT_DIFFERS(hi, bool_hi);
      // t_degraded equipState
      TS_ASSERT(BWPrinter.isOutputNode(BWPrinter["(t_degraded)equipState"]));
      TS_ASSERT(BWPrinter.isInputNode(BWPrinter["(t_degraded)equipState"]));
      TS_ASSERT(not BWPrinter.isInnerNode(BWPrinter["(t_degraded)equipState"]));
      // t_state equipState
      TS_ASSERT(BWPrinter.isOutputNode(BWPrinter["(t_state)equipState"]));
      TS_ASSERT(not BWPrinter.isInputNode(BWPrinter["(t_state)equipState"]));
      TS_ASSERT(not BWPrinter.isInnerNode(BWPrinter["(t_state)equipState"]));
      // boolean equipState
      TS_ASSERT(not BWPrinter.isOutputNode(BWPrinter["(boolean)equipState"]));
      TS_ASSERT(not BWPrinter.isInputNode(BWPrinter["(boolean)equipState"]));
      TS_ASSERT(BWPrinter.isInnerNode(BWPrinter["(boolean)equipState"]));
      // t_paper hasPaper
      TS_ASSERT(not BWPrinter.isOutputNode(BWPrinter["(t_paper)hasPaper"]));
      TS_ASSERT(not BWPrinter.isInputNode(BWPrinter["(t_paper)hasPaper"]));
      TS_ASSERT(BWPrinter.isInnerNode(BWPrinter["(t_paper)hasPaper"]));
      // t_state hasPaper
      TS_ASSERT(not BWPrinter.isOutputNode(BWPrinter["(t_state)hasPaper"]));
      TS_ASSERT(not BWPrinter.isInputNode(BWPrinter["(t_state)hasPaper"]));
      TS_ASSERT(BWPrinter.isInnerNode(BWPrinter["(t_state)hasPaper"]));
      // Boolean hasPaper
      TS_ASSERT(not BWPrinter.isOutputNode(BWPrinter["(boolean)hasPaper"]));
      TS_ASSERT(not BWPrinter.isInputNode(BWPrinter["(boolean)hasPaper"]));
      TS_ASSERT(BWPrinter.isInnerNode(BWPrinter["(boolean)hasPaper"]));
      // Boolean hasInk
      TS_ASSERT(not BWPrinter.isOutputNode(BWPrinter["hasInk"]));
      TS_ASSERT(not BWPrinter.isInputNode(BWPrinter["hasInk"]));
      TS_ASSERT(BWPrinter.isInnerNode(BWPrinter["hasInk"]));
      delete prm;
    }

    // Testing class ColorPrinter
    void testComplexPrinters_9() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(
          "../../../src/testunits/ressources/o3prm/complexprinters.o3prm"));
      gum::prm::PRM<double> *prm = reader.prm();
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("ColorPrinter"));
      gum::prm::Class<double> &ColorPrinter = prm->getClass("ColorPrinter");
      TS_ASSERT_EQUALS(ColorPrinter.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(ColorPrinter.attributes().size(), (gum::Size)19);
      TS_ASSERT_EQUALS(ColorPrinter.aggregates().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(ColorPrinter.slotChains().size(), (gum::Size)1);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["room"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(boolean)equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(t_state)equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(t_degraded)equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(boolean)hasPaper"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(t_state)hasPaper"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(t_paper)hasPaper"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["hasPaper"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(boolean)hasInk"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(boolean)black"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(t_state)black"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(t_ink)black"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["black"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(boolean)magenta"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(t_state)magenta"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(t_ink)magenta"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["magenta"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(boolean)yellow"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(t_state)yellow"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(t_ink)yellow"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["yellow"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(boolean)cyan"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(t_state)cyan"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["(t_ink)cyan"]);
      TS_GUM_ASSERT_THROWS_NOTHING(ColorPrinter["cyan"]);
      // Testing gum::NodeId, inputs and outputs
      gum::NodeId st = ColorPrinter["equipState"].id();
      gum::NodeId deg_st = ColorPrinter["(t_degraded)equipState"].id();
      gum::NodeId st_st = ColorPrinter["(t_state)equipState"].id();
      gum::NodeId bool_st = ColorPrinter["(boolean)equipState"].id();
      gum::NodeId hp = ColorPrinter["hasPaper"].id();
      gum::NodeId bool_hp = ColorPrinter["(boolean)hasPaper"].id();
      gum::NodeId st_hp = ColorPrinter["(t_state)hasPaper"].id();
      gum::NodeId p_hp = ColorPrinter["(t_paper)hasPaper"].id();
      gum::NodeId hi = ColorPrinter["hasInk"].id();
      gum::NodeId bool_hi = ColorPrinter["(boolean)hasInk"].id();
      gum::NodeId bl = ColorPrinter["black"].id();
      gum::NodeId bool_bl = ColorPrinter["(boolean)black"].id();
      gum::NodeId st_bl = ColorPrinter["(t_state)black"].id();
      gum::NodeId i_bl = ColorPrinter["(t_ink)black"].id();
      gum::NodeId ma = ColorPrinter["magenta"].id();
      gum::NodeId bool_ma = ColorPrinter["(boolean)magenta"].id();
      gum::NodeId st_ma = ColorPrinter["(t_state)magenta"].id();
      gum::NodeId i_ma = ColorPrinter["(t_ink)magenta"].id();
      gum::NodeId ye = ColorPrinter["yellow"].id();
      gum::NodeId bool_ye = ColorPrinter["(boolean)yellow"].id();
      gum::NodeId st_ye = ColorPrinter["(t_state)yellow"].id();
      gum::NodeId i_ye = ColorPrinter["(t_ink)yellow"].id();
      gum::NodeId ce = ColorPrinter["cyan"].id();
      gum::NodeId bool_ce = ColorPrinter["(boolean)cyan"].id();
      gum::NodeId st_ce = ColorPrinter["(t_state)cyan"].id();
      gum::NodeId i_ce = ColorPrinter["(t_ink)cyan"].id();
      TS_ASSERT_EQUALS(st, deg_st);
      TS_ASSERT_DIFFERS(st, st_st);
      TS_ASSERT_DIFFERS(st, bool_st);
      TS_ASSERT_DIFFERS(st_st, bool_st);
      TS_ASSERT_EQUALS(hp, p_hp);
      TS_ASSERT_DIFFERS(hp, st_hp);
      TS_ASSERT_DIFFERS(hp, bool_hp);
      TS_ASSERT_EQUALS(hi, bool_hi);
      TS_ASSERT_EQUALS(bl, i_bl);
      TS_ASSERT_DIFFERS(bl, st_bl);
      TS_ASSERT_DIFFERS(bl, bool_bl);
      TS_ASSERT_DIFFERS(st_bl, bool_bl);
      TS_ASSERT_EQUALS(ma, i_ma);
      TS_ASSERT_DIFFERS(ma, st_ma);
      TS_ASSERT_DIFFERS(ma, bool_ma);
      TS_ASSERT_DIFFERS(st_ma, bool_ma);
      TS_ASSERT_EQUALS(ye, i_ye);
      TS_ASSERT_DIFFERS(ye, st_ye);
      TS_ASSERT_DIFFERS(ye, bool_ye);
      TS_ASSERT_DIFFERS(st_ye, bool_ye);
      TS_ASSERT_EQUALS(ce, i_ce);
      TS_ASSERT_DIFFERS(ce, st_ce);
      TS_ASSERT_DIFFERS(ce, bool_ce);
      TS_ASSERT_DIFFERS(st_ce, bool_ce);
      // t_degraded equipState
      TS_ASSERT(ColorPrinter.isOutputNode(ColorPrinter["(t_degraded)equipState"]));
      TS_ASSERT(ColorPrinter.isInputNode(ColorPrinter["(t_degraded)equipState"]));
      TS_ASSERT(
          not ColorPrinter.isInnerNode(ColorPrinter["(t_degraded)equipState"]));
      // t_state equipState
      TS_ASSERT(ColorPrinter.isOutputNode(ColorPrinter["(t_state)equipState"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(t_state)equipState"]));
      TS_ASSERT(not ColorPrinter.isInnerNode(ColorPrinter["(t_state)equipState"]));
      // boolean equipState
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(boolean)equipState"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(boolean)equipState"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(boolean)equipState"]));
      // t_paper hasPaper
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(t_paper)hasPaper"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(t_paper)hasPaper"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(t_paper)hasPaper"]));
      // t_state hasPaper
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(t_state)hasPaper"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(t_state)hasPaper"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(t_state)hasPaper"]));
      // Boolean hasPaper
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(boolean)hasPaper"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(boolean)hasPaper"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(boolean)hasPaper"]));
      // Boolean hasInk
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(boolean)hasInk"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(boolean)hasInk"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(boolean)hasInk"]));
      // t_ink black
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(t_ink)black"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(t_ink)black"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(t_ink)black"]));
      // t_state black
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(t_state)black"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(t_state)black"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(t_state)black"]));
      // boolean black
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(boolean)black"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(boolean)black"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(boolean)black"]));
      // t_ink magenta
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(t_ink)magenta"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(t_ink)magenta"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(t_ink)magenta"]));
      // t_state magenta
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(t_state)magenta"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(t_state)magenta"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(t_state)magenta"]));
      // boolean magenta
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(boolean)magenta"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(boolean)magenta"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(boolean)magenta"]));
      // t_ink yellow
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(t_ink)yellow"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(t_ink)yellow"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(t_ink)yellow"]));
      // t_state yellow
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(t_state)yellow"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(t_state)yellow"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(t_state)yellow"]));
      // boolean yellow
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(boolean)yellow"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(boolean)yellow"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(boolean)yellow"]));
      // t_ink cyan
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(t_ink)cyan"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(t_ink)cyan"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(t_ink)cyan"]));
      // t_state cyan
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(t_state)cyan"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(t_state)cyan"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(t_state)cyan"]));
      // boolean cyan
      TS_ASSERT(not ColorPrinter.isOutputNode(ColorPrinter["(boolean)cyan"]));
      TS_ASSERT(not ColorPrinter.isInputNode(ColorPrinter["(boolean)cyan"]));
      TS_ASSERT(ColorPrinter.isInnerNode(ColorPrinter["(boolean)cyan"]));
      delete prm;
    }

    // Testing class Computer
    void testComplexPrinters_10() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(
          "../../../src/testunits/ressources/o3prm/complexprinters.o3prm"));
      gum::prm::PRM<double> *prm = reader.prm();
      gum::prm::Class<double> &Computer = prm->getClass("Computer");
      TS_ASSERT_EQUALS(Computer.attributes().size(), (gum::Size)5);
      TS_ASSERT_EQUALS(Computer.referenceSlots().size(), (gum::Size)2);
      TS_ASSERT_EQUALS(Computer.aggregates().size(), (gum::Size)2);
      TS_ASSERT_EQUALS(Computer.slotChains().size(), (gum::Size)2);
      TS_GUM_ASSERT_THROWS_NOTHING(Computer["(boolean)functional_printer"]);
      TS_GUM_ASSERT_THROWS_NOTHING(Computer["functional_printer"]);
      TS_GUM_ASSERT_THROWS_NOTHING(Computer["(boolean)degraded_printer"]);
      TS_GUM_ASSERT_THROWS_NOTHING(Computer["degraded_printer"]);
      TS_GUM_ASSERT_THROWS_NOTHING(Computer["(boolean)working_printer"]);
      TS_GUM_ASSERT_THROWS_NOTHING(Computer["working_printer"]);
      TS_GUM_ASSERT_THROWS_NOTHING(Computer["(boolean)equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(Computer["(t_state)equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(Computer["(t_degraded)equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(Computer["equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(Computer["(boolean)can_print"]);
      TS_GUM_ASSERT_THROWS_NOTHING(Computer["can_print"]);
      TS_GUM_ASSERT_THROWS_NOTHING(Computer["printers.equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(Computer["room.power.state"]);
      delete prm;
    }

    // Testing class Computer
    void testComplexPrinters_11() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(
          "../../../src/testunits/ressources/o3prm/complexprinters.o3prm"));
      gum::prm::PRM<double> *prm = reader.prm();
      gum::prm::Class<double> &SafeComputer = prm->getClass("SafeComputer");
      TS_ASSERT_EQUALS(SafeComputer.attributes().size(), (gum::Size)5);
      TS_ASSERT_EQUALS(SafeComputer.referenceSlots().size(), (gum::Size)2);
      TS_ASSERT_EQUALS(SafeComputer.aggregates().size(), (gum::Size)2);
      TS_ASSERT_EQUALS(SafeComputer.slotChains().size(), (gum::Size)3);
      TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["(boolean)functional_printer"]);
      TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["functional_printer"]);
      TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["(boolean)degraded_printer"]);
      TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["degraded_printer"]);
      TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["(boolean)working_printer"]);
      TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["working_printer"]);
      TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["(boolean)equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["(t_state)equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["(t_degraded)equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["(boolean)can_print"]);
      TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["can_print"]);
      TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["printers.(t_state)equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["printers.(t_degraded)equipState"]);
      TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["room.power.(t_state)state"]);
      delete prm;
    }

    void testPrintersSystems1() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(
          "../../../src/testunits/ressources/o3prm/printers_systems.o3prm"));
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);
      gum::prm::PRM<double> *prm = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(prm = reader.prm());

      if (prm) {
        delete prm;
      }
    }

    void testComplexPrintersSystems1() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(
          "../../../src/testunits/ressources/o3prm/complexprinters_system.o3prm"));
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);
      gum::prm::PRM<double> *prm = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(prm = reader.prm());
      gum::prm::System<double> *sys = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(sys = &(prm->system("aSys")));
      gum::prm::Class<double> &power = prm->getClass("PowerSupply");
      gum::prm::Class<double> &room = prm->getClass("Room");
      gum::prm::Class<double> &BWPrinter = prm->getClass("BWPrinter");
      gum::prm::Class<double> &ColorPrinter = prm->getClass("ColorPrinter");
      gum::prm::Class<double> &Computer = prm->getClass("Computer");
      TS_ASSERT_EQUALS(sys->get(power).size(), (gum::Size)1);
      TS_ASSERT_EQUALS(sys->get(room).size(), (gum::Size)1);
      TS_ASSERT_EQUALS(sys->get(BWPrinter).size(), (gum::Size)10);
      TS_ASSERT_EQUALS(sys->get(ColorPrinter).size(), (gum::Size)2);
      TS_ASSERT_EQUALS(sys->get(Computer).size(), (gum::Size)2);
      int count = 0;

      for (auto iter = sys->begin(); iter != sys->end(); ++iter) {
        ++count;

        for (auto jter = (*(iter.val())).begin(); jter != (*(iter.val())).end();
             ++jter) {
          if ((*(jter.val())).cpf().nbrDim() == 0) {
            std::stringstream sBuff;
            sBuff << (*(iter.val())).name() << "." << (*(jter.val())).safeName();
            GUM_TRACE(sBuff.str());
          }

          if (gum::prm::ClassElement<double>::isAggregate(
                  (*(iter.val())).type().get((*(jter.val())).id()))) {
            if ((*(jter.val())).cpf().nbrDim() == 1) {
              std::stringstream sBuff;
              sBuff << (*(iter.val())).name() << "." << (*(jter.val())).safeName();
              GUM_TRACE(sBuff.str());
            }
          }
        }
      }

      TS_ASSERT_EQUALS(count, 17);

      if (prm) {
        delete prm;
      }
    }

    void testParameters() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(
          "../../../src/testunits/ressources/o3prm/complexprinters_system.o3prm"));
      gum::prm::PRM<double> *prm = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(prm = reader.prm());
      gum::prm::System<double> *sys = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(sys = &(prm->system("aSys")));
      gum::prm::Class<double> &param_class = prm->getClass("ParamClass");
      gum::prm::Instance<double> &param = sys->get("param");
      std::string attr = "(boolean)aBoolParam";
      gum::prm::Attribute<double> *class_attr =
          &(static_cast<gum::prm::Attribute<double> &>(param_class.get(attr)));
      gum::prm::Attribute<double> *inst_attr = &(param.get(attr));
      {
        TS_ASSERT_EQUALS(class_attr->cpf().nbrDim(), inst_attr->cpf().nbrDim());
        TS_ASSERT_EQUALS(class_attr->cpf().domainSize(),
                         inst_attr->cpf().domainSize());
        gum::Instantiation i(class_attr->cpf()), j(inst_attr->cpf());

        for (i.setFirst(), j.setFirst(); not(i.end() or j.end()); i.inc(), j.inc()) {
          TS_ASSERT_DIFFERS(class_attr->cpf().get(i), inst_attr->cpf().get(j));
        }
      }
      attr = "(t_state)aParameter";
      class_attr =
          &(static_cast<gum::prm::Attribute<double> &>(param_class.get(attr)));
      inst_attr = &(param.get(attr));
      {
        TS_ASSERT_EQUALS(class_attr->cpf().nbrDim(), inst_attr->cpf().nbrDim());
        TS_ASSERT_EQUALS(class_attr->cpf().domainSize(),
                         inst_attr->cpf().domainSize());
        gum::Instantiation i(class_attr->cpf()), j(inst_attr->cpf());

        for (i.setFirst(), j.setFirst(); not(i.end() or j.end()); i.inc(), j.inc()) {
          TS_ASSERT_DIFFERS(class_attr->cpf().get(i), inst_attr->cpf().get(j));
        }
      }
      attr = "(boolean)aBoolParamWithDefault";
      class_attr =
          &(static_cast<gum::prm::Attribute<double> &>(param_class.get(attr)));
      inst_attr = &(param.get(attr));
      {
        TS_ASSERT_EQUALS(class_attr->cpf().nbrDim(), inst_attr->cpf().nbrDim());
        TS_ASSERT_EQUALS(class_attr->cpf().domainSize(),
                         inst_attr->cpf().domainSize());
        gum::Instantiation i(class_attr->cpf()), j(inst_attr->cpf());

        for (i.setFirst(), j.setFirst(); not(i.end() or j.end()); i.inc(), j.inc()) {
          TS_ASSERT_EQUALS(class_attr->cpf().get(i), inst_attr->cpf().get(j));
        }
      }
      attr = "(t_state)anotherParameter";
      class_attr =
          &(static_cast<gum::prm::Attribute<double> &>(param_class.get(attr)));
      inst_attr = &(param.get(attr));
      {
        TS_ASSERT_EQUALS(class_attr->cpf().nbrDim(), inst_attr->cpf().nbrDim());
        TS_ASSERT_EQUALS(class_attr->cpf().domainSize(),
                         inst_attr->cpf().domainSize());
        gum::Instantiation i(class_attr->cpf()), j(inst_attr->cpf());

        for (i.setFirst(), j.setFirst(); not(i.end() or j.end()); i.inc(), j.inc()) {
          TS_ASSERT_EQUALS(class_attr->cpf().get(i), inst_attr->cpf().get(j));
        }
      }

      if (prm) {
        delete prm;
      }
    }

    void testCPF() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(
          "../../../src/testunits/ressources/o3prm/inference.o3prm"));
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);
      gum::prm::PRM<double> *prm = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(prm = reader.prm());
      gum::prm::System<double> *sys = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(sys = &(prm->system("aSys")));

      for (gum::prm::System<double>::iterator iter = sys->begin();
           iter != sys->end(); ++iter) {
        for (gum::prm::Instance<double>::iterator jter = (*(iter.val())).begin();
             jter != (*(iter.val())).end(); ++jter) {
          gum::Instantiation i((*(jter.val())).cpf()), var;
          var.add((*(jter.val())).type().variable());

          for (i.setFirstOut(var); not i.end(); i.incOut(var)) {
            gum::prm::prm_float f = 0.0;

            for (i.setFirstIn(var); not i.end(); i.incIn(var)) {
              f += (*(jter.val())).cpf().get(i);
            }

            TS_ASSERT_DELTA(f, 1.0, 1e-9);
            i.unsetOverflow();
          }
        }
      }

      if (prm)
        delete prm;
    }

    void testEasyWriting() {
      gum::prm::o3prm::O3prmReader<double> reader;
      reader.addClassPath(GET_PATH_STR("o3prmr"));
      TS_GUM_ASSERT_THROWS_NOTHING(
          reader.readFile(GET_PATH_STR("o3prmr/systems/MyKickAssSystem.o3prm")));

      gum::prm::PRM<double> *prm = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(prm = reader.prm());
      gum::Size plop = prm->classes().size();
      plop += prm->interfaces().size();
      TS_ASSERT_EQUALS(plop, (gum::Size)8);

      gum::prm::System<double> *sys = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(sys = &(prm->system("systems.MyKickAssSystem")));
      TS_ASSERT_EQUALS(sys->size(), (gum::Size)13);

      if (prm)
        delete prm;
    }

    // Do not uncommment these unless you want to debug or test with errors
    // (it dumps a lot of stuff)

    void testErrorsAndWarnings() {
      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(
          reader.readFile(GET_PATH_STR("o3prm/Printer_with_errors.o3prm")));
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)4);

      if (reader.prm())
        ;

      delete reader.prm();
    }

    void testReadString1() {
      std::ifstream stream(GET_PATH_STR("o3prm/types.o3prm"));
      std::string buffer;
      char tampon[255];
      TS_ASSERT(stream.good());

      while (stream.good()) {
        stream.getline(tampon, 255);
        buffer += tampon;
        buffer += "\n";
      }

      stream.close();

      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readString(buffer));
      gum::prm::PRM<double> *prm = reader.prm();
      TS_ASSERT_EQUALS(prm->type("t_state").variable().domainSize(), (gum::Size)2);
      TS_ASSERT_EQUALS(prm->type("t_ink").variable().domainSize(), (gum::Size)2);
      TS_ASSERT_EQUALS(prm->type("t_degraded").variable().domainSize(),
                       (gum::Size)3);
      TS_ASSERT_EQUALS(prm->type("t_bw_p").variable().domainSize(), (gum::Size)4);
      TS_ASSERT_EQUALS(prm->type("t_color_p").variable().domainSize(), (gum::Size)5);
      delete prm;
    }

    void testReadString2() {
      std::ifstream stream(GET_PATH_STR("o3prm/Printer_with_errors.o3prm"));
      std::string buffer;
      char tampon[255];
      TS_ASSERT(stream.good());

      while (stream.good()) {
        stream.getline(tampon, 255);
        buffer += tampon;
        buffer += "\n";
      }
      stream.close();

      gum::prm::o3prm::O3prmReader<double> reader;
      TS_GUM_ASSERT_THROWS_NOTHING(reader.readString(buffer));
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)4);
      if (reader.prm())
        ;
      delete reader.prm();
    }
  };

} // namespace gum_tests
