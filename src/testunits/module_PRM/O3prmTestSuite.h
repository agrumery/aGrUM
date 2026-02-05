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

#include <agrum/base/graphs/graphElements.h>
#include <agrum/PRM/elements/PRMFormAttribute.h>
#include <agrum/PRM/o3prm/O3prmReader.h>

#include <agrum/base/core/math/math_utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  O3prm
#define GUM_CURRENT_MODULE PRM

namespace gum_tests {

  struct O3prmTestSuite {
    public:
    static void testTypes() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(GET_RESSOURCES_PATH("o3prm/types.o3prm")));
        CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));
        CHECK((reader.errors()) == (static_cast< gum::Size >(0)));
        reader.showElegantErrorsAndWarnings(std::cout);
        gum::prm::PRM< double >* prm = reader.prm();
        CHECK((prm->type("t_state").variable().domainSize()) == (static_cast< gum::Size >(2)));
        CHECK((prm->type("t_ink").variable().domainSize()) == (static_cast< gum::Size >(2)));
        CHECK((prm->type("t_degraded").variable().domainSize()) == (static_cast< gum::Size >(3)));
        CHECK((prm->type("t_bw_p").variable().domainSize()) == (static_cast< gum::Size >(4)));
        CHECK((prm->type("t_color_p").variable().domainSize()) == (static_cast< gum::Size >(5)));
        delete prm;
      }   // namespace gum_tests

      catch (gum::Exception&) {
        CHECK(false);
      }
    }

    O3prmTestSuite() {
      // std::cerr << std::endl;
    }

    static void testSubTypes() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(GET_RESSOURCES_PATH("o3prm/types.o3prm")));
        gum::prm::PRM< double >* prm = reader.prm();
        CHECK(prm->type("t_state").isSubType());
        CHECK(prm->type("t_state").isSubTypeOf(prm->type("boolean")));
        CHECK(prm->type("t_ink").isSubType());
        CHECK(prm->type("t_ink").isSubTypeOf(prm->type("boolean")));
        CHECK(prm->type("t_ink").isSubTypeOf(prm->type("t_state")));
        CHECK(prm->type("t_paper").isSubType());
        CHECK(prm->type("t_paper").isSubTypeOf(prm->type("boolean")));
        CHECK(prm->type("t_paper").isSubTypeOf(prm->type("t_state")));
        CHECK(prm->type("t_degraded").isSubType());
        CHECK(prm->type("t_degraded").isSubTypeOf(prm->type("boolean")));
        CHECK(prm->type("t_degraded").isSubTypeOf(prm->type("t_state")));
        CHECK(prm->type("t_bw_p").isSubType());
        CHECK(prm->type("t_bw_p").isSubTypeOf(prm->type("boolean")));
        CHECK(prm->type("t_bw_p").isSubTypeOf(prm->type("t_state")));
        CHECK(prm->type("t_bw_p").isSubTypeOf(prm->type("t_degraded")));
        CHECK(prm->type("t_color_p").isSubType());
        CHECK(prm->type("t_color_p").isSubTypeOf(prm->type("boolean")));
        CHECK(prm->type("t_color_p").isSubTypeOf(prm->type("t_state")));
        CHECK(prm->type("t_color_p").isSubTypeOf(prm->type("t_degraded")));
        CHECK((prm->types().size()) == (static_cast< gum::Size >(7)));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testClasses() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        GUM_CHECK_ASSERT_THROWS_NOTHING(
            reader.readFile(GET_RESSOURCES_PATH("o3prm/printers.o3prm")));
        CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));
        CHECK((reader.errors()) == (static_cast< gum::Size >(0)));
        reader.showElegantErrorsAndWarnings();
        gum::prm::PRM< double >* prm = reader.prm();
        CHECK((prm->classes().size()) == (static_cast< gum::Size >(5)));
        GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getClass("PowerSupply"));
        GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getClass("Room"));
        GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getClass("Equipment"));
        GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getClass("Printer"));
        GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getClass("Computer"));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testPrintersPowerSupply() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        GUM_CHECK_ASSERT_THROWS_NOTHING(
            reader.readFile(GET_RESSOURCES_PATH("o3prm/printers.o3prm")));
        gum::prm::PRM< double >* prm = reader.prm();
        GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getClass("PowerSupply"));
        gum::prm::PRMClass< double >& c = prm->getClass("PowerSupply");
        CHECK((c.attributes().size()) == (static_cast< gum::Size >(1)));
        CHECK((c.referenceSlots().size()) == (static_cast< gum::Size >(0)));
        CHECK((c.aggregates().size()) == (static_cast< gum::Size >(0)));
        CHECK((c.slotChains().size()) == (static_cast< gum::Size >(0)));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testPrintersRoom() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        GUM_CHECK_ASSERT_THROWS_NOTHING(
            reader.readFile(GET_RESSOURCES_PATH("o3prm/printers.o3prm")));
        gum::prm::PRM< double >* prm = reader.prm();
        GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getClass("Room"));
        gum::prm::PRMClass< double >& c = prm->getClass("Room");
        CHECK((c.attributes().size()) == (static_cast< gum::Size >(0)));
        CHECK((c.referenceSlots().size()) == (static_cast< gum::Size >(1)));
        CHECK((c.aggregates().size()) == (static_cast< gum::Size >(0)));
        CHECK((c.slotChains().size()) == (static_cast< gum::Size >(0)));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testPrintersEquipment() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        GUM_CHECK_ASSERT_THROWS_NOTHING(
            reader.readFile(GET_RESSOURCES_PATH("o3prm/printers.o3prm")));
        gum::prm::PRM< double >* prm = reader.prm();
        GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getClass("Equipment"));
        gum::prm::PRMClass< double >& c = prm->getClass("Equipment");
        CHECK((c.attributes().size()) == (static_cast< gum::Size >(1)));
        CHECK((c.referenceSlots().size()) == (static_cast< gum::Size >(1)));
        CHECK((c.aggregates().size()) == (static_cast< gum::Size >(0)));
        CHECK((c.slotChains().size()) == (static_cast< gum::Size >(1)));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testPrintersPrinter() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        GUM_CHECK_ASSERT_THROWS_NOTHING(
            reader.readFile(GET_RESSOURCES_PATH("o3prm/printers.o3prm")));
        gum::prm::PRM< double >* prm = reader.prm();
        GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getClass("Printer"));
        gum::prm::PRMClass< double >& c = prm->getClass("Printer");
        CHECK((c.attributes().size()) == (static_cast< gum::Size >(3)));
        CHECK((c.referenceSlots().size()) == (static_cast< gum::Size >(1)));
        CHECK((c.aggregates().size()) == (static_cast< gum::Size >(0)));
        CHECK((c.slotChains().size()) == (static_cast< gum::Size >(1)));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testPrintersComputer() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        GUM_CHECK_ASSERT_THROWS_NOTHING(
            reader.readFile(GET_RESSOURCES_PATH("o3prm/printers.o3prm")));
        gum::prm::PRM< double >* prm = reader.prm();
        GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getClass("Computer"));
        gum::prm::PRMClass< double >& c = prm->getClass("Computer");
        CHECK((c.attributes().size()) == (static_cast< gum::Size >(2)));
        CHECK((c.referenceSlots().size()) == (static_cast< gum::Size >(2)));
        CHECK((c.aggregates().size()) == (static_cast< gum::Size >(1)));
        CHECK((c.slotChains().size()) == (static_cast< gum::Size >(2)));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    /// Testing classes and interfaces
    static void testComplexPrinters_1() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));
        CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));
        CHECK((reader.errors()) == (static_cast< gum::Size >(0)));
        reader.showElegantErrorsAndWarnings();
        gum::prm::PRM< double >* prm = 0;
        GUM_CHECK_ASSERT_THROWS_NOTHING(prm = reader.prm());
        if (prm) {
          CHECK((prm->classes().size()) == (static_cast< gum::Size >(7)));
          GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getClass("fr.lip6.printers.PowerSupply"));
          GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getClass("fr.lip6.printers.Room"));
          GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getClass("fr.lip6.printers.BWPrinter"));
          GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getClass("fr.lip6.printers.ColorPrinter"));
          GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getClass("fr.lip6.printers.Computer"));
          CHECK((prm->interfaces().size()) == (static_cast< gum::Size >(2)));
          GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getInterface("fr.lip6.printers.Equipment"));
          GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getInterface("fr.lip6.printers.Printer"));
          delete prm;
        }
      } catch (gum::Exception&) { CHECK(false); }
    }

    /// Testing classes and interfaces inheritance with subtypes methods
    static void testComplexPrinters_2() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));
        gum::prm::PRM< double >* prm = reader.prm();
        // Classes
        gum::prm::PRMClass< double >& PowerSupply  = prm->getClass("fr.lip6.printers.PowerSupply");
        gum::prm::PRMClass< double >& Room         = prm->getClass("fr.lip6.printers.Room");
        gum::prm::PRMClass< double >& BWPrinter    = prm->getClass("fr.lip6.printers.BWPrinter");
        gum::prm::PRMClass< double >& ColorPrinter = prm->getClass("fr.lip6.printers.ColorPrinter");
        gum::prm::PRMClass< double >& Computer     = prm->getClass("fr.lip6.printers.Computer");
        // Interfaces
        gum::prm::PRMInterface< double >& Equipment
            = prm->getInterface("fr.lip6.printers.Equipment");
        gum::prm::PRMInterface< double >& Printer = prm->getInterface("fr.lip6.printers.Printer");
        // Testing PowerSupply
        CHECK(PowerSupply.isSubTypeOf(PowerSupply));
        CHECK(!PowerSupply.isSubTypeOf(Room));
        CHECK(!PowerSupply.isSubTypeOf(Equipment));
        CHECK(!PowerSupply.isSubTypeOf(Printer));
        CHECK(!PowerSupply.isSubTypeOf(BWPrinter));
        CHECK(!PowerSupply.isSubTypeOf(ColorPrinter));
        CHECK(!PowerSupply.isSubTypeOf(Computer));
        // Testing Room
        CHECK(!Room.isSubTypeOf(PowerSupply));
        CHECK(Room.isSubTypeOf(Room));
        CHECK(!Room.isSubTypeOf(Equipment));
        CHECK(!Room.isSubTypeOf(Printer));
        CHECK(!Room.isSubTypeOf(BWPrinter));
        CHECK(!Room.isSubTypeOf(ColorPrinter));
        CHECK(!Room.isSubTypeOf(Computer));
        // Testing Equipment
        CHECK(!Equipment.isSubTypeOf(PowerSupply));
        CHECK(!Equipment.isSubTypeOf(Room));
        CHECK(Equipment.isSubTypeOf(Equipment));
        CHECK(!Equipment.isSubTypeOf(Printer));
        CHECK(!Equipment.isSubTypeOf(BWPrinter));
        CHECK(!Equipment.isSubTypeOf(ColorPrinter));
        CHECK(!Equipment.isSubTypeOf(Computer));
        // Testing Printer
        CHECK(!Printer.isSubTypeOf(PowerSupply));
        CHECK(!Printer.isSubTypeOf(Room));
        CHECK(Printer.isSubTypeOf(Equipment));
        CHECK(Printer.isSubTypeOf(Printer));
        CHECK(!Printer.isSubTypeOf(BWPrinter));
        CHECK(!Printer.isSubTypeOf(ColorPrinter));
        CHECK(!Printer.isSubTypeOf(Computer));
        // Testing BWPrinter
        CHECK(!BWPrinter.isSubTypeOf(PowerSupply));
        CHECK(!BWPrinter.isSubTypeOf(Room));
        CHECK(BWPrinter.isSubTypeOf(Equipment));
        CHECK(BWPrinter.isSubTypeOf(Printer));
        CHECK(BWPrinter.isSubTypeOf(BWPrinter));
        CHECK(!BWPrinter.isSubTypeOf(ColorPrinter));
        CHECK(!BWPrinter.isSubTypeOf(Computer));
        // Testing ColorPrinter
        CHECK(!ColorPrinter.isSubTypeOf(PowerSupply));
        CHECK(!ColorPrinter.isSubTypeOf(Room));
        CHECK(ColorPrinter.isSubTypeOf(Equipment));
        CHECK(ColorPrinter.isSubTypeOf(Printer));
        CHECK(!ColorPrinter.isSubTypeOf(BWPrinter));
        CHECK(ColorPrinter.isSubTypeOf(ColorPrinter));
        CHECK(!ColorPrinter.isSubTypeOf(Computer));
        // Testing Computer
        CHECK(!Computer.isSubTypeOf(PowerSupply));
        CHECK(!Computer.isSubTypeOf(Room));
        CHECK(Computer.isSubTypeOf(Equipment));
        CHECK(!Computer.isSubTypeOf(Printer));
        CHECK(!Computer.isSubTypeOf(BWPrinter));
        CHECK(!Computer.isSubTypeOf(ColorPrinter));
        CHECK(Computer.isSubTypeOf(Computer));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    /// Testing classes and interfaces inheritance with supertypes methods
    static void testComplexPrinters_3() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));
        gum::prm::PRM< double >* prm = reader.prm();
        // Classes
        gum::prm::PRMClass< double >& PowerSupply  = prm->getClass("fr.lip6.printers.PowerSupply");
        gum::prm::PRMClass< double >& Room         = prm->getClass("fr.lip6.printers.Room");
        gum::prm::PRMClass< double >& BWPrinter    = prm->getClass("fr.lip6.printers.BWPrinter");
        gum::prm::PRMClass< double >& ColorPrinter = prm->getClass("fr.lip6.printers.ColorPrinter");
        gum::prm::PRMClass< double >& Computer     = prm->getClass("fr.lip6.printers.Computer");
        // Interfaces
        gum::prm::PRMInterface< double >& Equipment
            = prm->getInterface("fr.lip6.printers.Equipment");
        gum::prm::PRMInterface< double >& Printer = prm->getInterface("fr.lip6.printers.Printer");
        // Testing PowerSupply
        CHECK(PowerSupply.isSuperTypeOf(PowerSupply));
        CHECK(!PowerSupply.isSuperTypeOf(Room));
        CHECK(!PowerSupply.isSuperTypeOf(Equipment));
        CHECK(!PowerSupply.isSuperTypeOf(Printer));
        CHECK(!PowerSupply.isSuperTypeOf(BWPrinter));
        CHECK(!PowerSupply.isSuperTypeOf(ColorPrinter));
        CHECK(!PowerSupply.isSuperTypeOf(Computer));
        // Testing Room
        CHECK(!Room.isSuperTypeOf(PowerSupply));
        CHECK(Room.isSuperTypeOf(Room));
        CHECK(!Room.isSuperTypeOf(Equipment));
        CHECK(!Room.isSuperTypeOf(Printer));
        CHECK(!Room.isSuperTypeOf(BWPrinter));
        CHECK(!Room.isSuperTypeOf(ColorPrinter));
        CHECK(!Room.isSuperTypeOf(Computer));
        // Testing Equipment
        CHECK(!Equipment.isSuperTypeOf(PowerSupply));
        CHECK(!Equipment.isSuperTypeOf(Room));
        CHECK(Equipment.isSuperTypeOf(Equipment));
        CHECK(Equipment.isSuperTypeOf(Printer));
        CHECK(Equipment.isSuperTypeOf(BWPrinter));
        CHECK(Equipment.isSuperTypeOf(ColorPrinter));
        CHECK(Equipment.isSuperTypeOf(Computer));
        // Testing Printer
        CHECK(!Printer.isSuperTypeOf(PowerSupply));
        CHECK(!Printer.isSuperTypeOf(Room));
        CHECK(!Printer.isSuperTypeOf(Equipment));
        CHECK(Printer.isSuperTypeOf(Printer));
        CHECK(Printer.isSuperTypeOf(BWPrinter));
        CHECK(Printer.isSuperTypeOf(ColorPrinter));
        CHECK(!Printer.isSuperTypeOf(Computer));
        // Testing BWPrinter
        CHECK(!BWPrinter.isSuperTypeOf(PowerSupply));
        CHECK(!BWPrinter.isSuperTypeOf(Room));
        CHECK(!BWPrinter.isSuperTypeOf(Equipment));
        CHECK(!BWPrinter.isSuperTypeOf(Printer));
        CHECK(BWPrinter.isSuperTypeOf(BWPrinter));
        CHECK(!BWPrinter.isSuperTypeOf(ColorPrinter));
        CHECK(!BWPrinter.isSuperTypeOf(Computer));
        // Testing ColorPrinter
        CHECK(!ColorPrinter.isSuperTypeOf(PowerSupply));
        CHECK(!ColorPrinter.isSuperTypeOf(Room));
        CHECK(!ColorPrinter.isSuperTypeOf(Equipment));
        CHECK(!ColorPrinter.isSuperTypeOf(Printer));
        CHECK(!ColorPrinter.isSuperTypeOf(BWPrinter));
        CHECK(ColorPrinter.isSuperTypeOf(ColorPrinter));
        CHECK(!ColorPrinter.isSuperTypeOf(Computer));
        // Testing Computer
        CHECK(!Computer.isSuperTypeOf(PowerSupply));
        CHECK(!Computer.isSuperTypeOf(Room));
        CHECK(!Computer.isSuperTypeOf(Equipment));
        CHECK(!Computer.isSuperTypeOf(Printer));
        CHECK(!Computer.isSuperTypeOf(BWPrinter));
        CHECK(!Computer.isSuperTypeOf(ColorPrinter));
        CHECK(Computer.isSuperTypeOf(Computer));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    // Testing class PowerSupply
    static void testComplexPrinters_4() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));
        gum::prm::PRM< double >* prm = reader.prm();
        // Classes
        gum::prm::PRMClass< double >& PowerSupply = prm->getClass("fr.lip6.printers.PowerSupply");
        CHECK((PowerSupply.attributes().size()) == (static_cast< gum::Size >(2)));
        CHECK((PowerSupply.containerDag().sizeArcs()) == (static_cast< gum::Size >(1)));
        GUM_CHECK_ASSERT_THROWS_NOTHING(PowerSupply["(boolean)state"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(PowerSupply["state"]);
        CHECK(PowerSupply["state"].type().isSubTypeOf(PowerSupply["(boolean)state"].type()));
        gum::NodeId n1 = 0;
        gum::NodeId n2 = 0;
        gum::NodeId n3 = 0;
        GUM_CHECK_ASSERT_THROWS_NOTHING(n1 = PowerSupply["(boolean)state"].id());
        GUM_CHECK_ASSERT_THROWS_NOTHING(n2 = PowerSupply["state"].id());
        GUM_CHECK_ASSERT_THROWS_NOTHING(n3 = PowerSupply["(fr.lip6.printers.t_state)state"].id());
        CHECK((n2) == (n3));
        CHECK(PowerSupply.containerDag().existsArc(n2, n1));
        CHECK((PowerSupply.referenceSlots().size()) == (static_cast< gum::Size >(0)));
        CHECK((PowerSupply.aggregates().size()) == (static_cast< gum::Size >(0)));
        CHECK((PowerSupply.slotChains().size()) == (static_cast< gum::Size >(0)));
        // Testing interfaces for the state attribute and its cast descendant
        CHECK(!PowerSupply.isOutputNode(PowerSupply["(boolean)state"]));
        CHECK(!PowerSupply.isInputNode(PowerSupply["(boolean)state"]));
        CHECK(PowerSupply.isInnerNode(PowerSupply["(boolean)state"]));
        CHECK(PowerSupply.isOutputNode(PowerSupply["state"]));
        CHECK(!PowerSupply.isInputNode(PowerSupply["state"]));
        CHECK(!PowerSupply.isInnerNode(PowerSupply["state"]));
        CHECK(PowerSupply.isOutputNode(PowerSupply["(fr.lip6.printers.t_state)state"]));
        CHECK(!PowerSupply.isInputNode(PowerSupply["(fr.lip6.printers.t_state)state"]));
        CHECK(!PowerSupply.isInnerNode(PowerSupply["(fr.lip6.printers.t_state)state"]));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    // Testing class Room
    static void testComplexPrinters_5() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));
        gum::prm::PRM< double >*      prm  = reader.prm();
        gum::prm::PRMClass< double >& Room = prm->getClass("fr.lip6.printers.Room");
        CHECK((Room.attributes().size()) == (static_cast< gum::Size >(0)));
        CHECK((Room.referenceSlots().size()) == (static_cast< gum::Size >(1)));
        CHECK((Room.aggregates().size()) == (static_cast< gum::Size >(0)));
        CHECK((Room.slotChains().size()) == (static_cast< gum::Size >(0)));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    // Testing interface Equipment
    static void testComplexPrinters_6() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));
        gum::prm::PRM< double >*          prm = reader.prm();
        gum::prm::PRMInterface< double >& Equipment
            = prm->getInterface("fr.lip6.printers.Equipment");
        CHECK((Equipment.referenceSlots().size()) == (static_cast< gum::Size >(1)));
        CHECK((Equipment.attributes().size()) == (static_cast< gum::Size >(3)));
        GUM_CHECK_ASSERT_THROWS_NOTHING(Equipment["equipState"]);
        CHECK((Equipment["equipState"].type().name()) == ("fr.lip6.printers.t_degraded"));
        GUM_CHECK_ASSERT_THROWS_NOTHING(Equipment["(fr.lip6.printers.t_state)equipState"]);
        CHECK((Equipment["(fr.lip6.printers.t_state)equipState"].type().name())
              == ("fr.lip6.printers.t_state"));
        GUM_CHECK_ASSERT_THROWS_NOTHING(Equipment["(boolean)equipState"]);
        CHECK((Equipment["(boolean)equipState"].type().name()) == ("boolean"));
        // Testing gum::NodeId, inputs and outputs
        gum::NodeId st      = Equipment["equipState"].id();
        gum::NodeId deg_st  = Equipment["(fr.lip6.printers.t_degraded)equipState"].id();
        gum::NodeId st_st   = Equipment["(fr.lip6.printers.t_state)equipState"].id();
        gum::NodeId bool_st = Equipment["(boolean)equipState"].id();
        CHECK((st) == (deg_st));
        CHECK((st) != (st_st));
        CHECK((st) != (bool_st));
        CHECK((st_st) != (bool_st));
        // t_degraded equipState
        CHECK(!Equipment.isOutputNode(Equipment["equipState"]));
        CHECK(!Equipment.isInputNode(Equipment["equipState"]));
        CHECK(Equipment.isInnerNode(Equipment["equipState"]));
        // t_state equipState
        CHECK(!Equipment.isOutputNode(Equipment["(fr.lip6.printers.t_state)equipState"]));
        CHECK(!Equipment.isInputNode(Equipment["(fr.lip6.printers.t_state)equipState"]));
        CHECK(Equipment.isInnerNode(Equipment["(fr.lip6.printers.t_state)equipState"]));
        // boolean equipState
        CHECK(!Equipment.isOutputNode(Equipment["(boolean)equipState"]));
        CHECK(!Equipment.isInputNode(Equipment["(boolean)equipState"]));
        CHECK(Equipment.isInnerNode(Equipment["(boolean)equipState"]));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    // Testing interface Printer
    static void testComplexPrinters_7() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));
        gum::prm::PRM< double >* prm = reader.prm();
        GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getInterface("fr.lip6.printers.Printer"));
        gum::prm::PRMInterface< double >& Printer = prm->getInterface("fr.lip6.printers.Printer");
        CHECK((Printer.referenceSlots().size()) == (static_cast< gum::Size >(1)));
        CHECK((Printer.attributes().size()) == (static_cast< gum::Size >(5)));
        GUM_CHECK_ASSERT_THROWS_NOTHING(Printer["room"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(Printer["equipState"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(Printer["hasPaper"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(Printer["hasInk"]);
        CHECK((Printer["equipState"].type().name()) == ("fr.lip6.printers.t_degraded"));
        CHECK((Printer["hasPaper"].type().name()) == ("boolean"));
        CHECK((Printer["hasInk"].type().name()) == ("boolean"));
        // Testing gum::NodeId, inputs and outputs
        gum::NodeId st      = Printer["equipState"].id();
        gum::NodeId deg_st  = Printer["(fr.lip6.printers.t_degraded)equipState"].id();
        gum::NodeId st_st   = Printer["(fr.lip6.printers.t_state)equipState"].id();
        gum::NodeId bool_st = Printer["(boolean)equipState"].id();
        gum::NodeId hp      = Printer["hasPaper"].id();
        gum::NodeId bool_hp = Printer["(boolean)hasPaper"].id();
        gum::NodeId hi      = Printer["hasInk"].id();
        gum::NodeId bool_hi = Printer["(boolean)hasInk"].id();
        CHECK((st) == (deg_st));
        CHECK((st) != (st_st));
        CHECK((st) != (bool_st));
        CHECK((st_st) != (bool_st));
        CHECK((hp) == (bool_hp));
        CHECK((hi) == (bool_hi));
        // t_degraded equipState
        CHECK(Printer.isOutputNode(Printer["(fr.lip6.printers.t_degraded)equipState"]));
        CHECK(!Printer.isInputNode(Printer["(fr.lip6.printers.t_degraded)equipState"]));
        CHECK(!Printer.isInnerNode(Printer["(fr.lip6.printers.t_degraded)equipState"]));
        // t_state equipState
        CHECK(Printer.isOutputNode(Printer["(fr.lip6.printers.t_state)equipState"]));
        CHECK(!Printer.isInputNode(Printer["(fr.lip6.printers.t_state)equipState"]));
        CHECK(!Printer.isInnerNode(Printer["(fr.lip6.printers.t_state)equipState"]));
        // boolean equipState
        CHECK(!Printer.isOutputNode(Printer["(boolean)equipState"]));
        CHECK(!Printer.isInputNode(Printer["(boolean)equipState"]));
        CHECK(Printer.isInnerNode(Printer["(boolean)equipState"]));
        // Boolean hasPaper
        CHECK(!Printer.isOutputNode(Printer["hasPaper"]));
        CHECK(!Printer.isInputNode(Printer["hasPaper"]));
        CHECK(Printer.isInnerNode(Printer["hasPaper"]));
        // Boolean hasInk
        CHECK(!Printer.isOutputNode(Printer["hasInk"]));
        CHECK(!Printer.isInputNode(Printer["hasInk"]));
        CHECK(Printer.isInnerNode(Printer["hasInk"]));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    /// @TODO Make this test past for IO flags optimisation
    // Testing class BWPrinter
    // static void testComplexPrinters_8() {
    //   try {
    //     gum::prm::o3prm::O3prmReader<double> reader;
    //     std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm"
    //     );
    //     std::string package = "fr.lip6.printers";
    //     GUM_CHECK_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
    //     gum::prm::PRM<double>* prm = reader.prm();
    //     GUM_CHECK_ASSERT_THROWS_NOTHING(
    //         prm->getClass( "fr.lip6.printers.BWPrinter" ) );
    //     gum::prm::Class<double>& BWPrinter =
    //         prm->getClass( "fr.lip6.printers.BWPrinter" );
    //     CHECK((BWPrinter.referenceSlots().size()) == (static_cast<gum::Size>(1)));
    //     CHECK((BWPrinter.attributes().size()) == (static_cast<gum::Size>(9)));
    //     CHECK((BWPrinter.slotChains().size()) == (static_cast<gum::Size>(1)));
    //     CHECK((BWPrinter.aggregates().size()) == (static_cast<gum::Size>(0)));
    //     GUM_CHECK_ASSERT_THROWS_NOTHING( BWPrinter["room"] );
    //     GUM_CHECK_ASSERT_THROWS_NOTHING( BWPrinter["(boolean)equipState"] );
    //     GUM_CHECK_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_state)equipState"] );
    //     GUM_CHECK_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_degraded)equipState"] );
    //     GUM_CHECK_ASSERT_THROWS_NOTHING( BWPrinter["equipState"] );
    //     GUM_CHECK_ASSERT_THROWS_NOTHING( BWPrinter["(boolean)hasPaper"] );
    //     GUM_CHECK_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_state)hasPaper"] );
    //     GUM_CHECK_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_paper)hasPaper"] );
    //     GUM_CHECK_ASSERT_THROWS_NOTHING( BWPrinter["hasPaper"] );
    //     GUM_CHECK_ASSERT_THROWS_NOTHING( BWPrinter["(boolean)hasInk"] );
    //     GUM_CHECK_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_state)hasInk"] );
    //     GUM_CHECK_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_ink)hasInk"] );
    //     GUM_CHECK_ASSERT_THROWS_NOTHING( BWPrinter["hasInk"] );
    //     // Testing gum::NodeId, inputs and outputs
    //     gum::NodeId st = BWPrinter["equipState"].id();
    //     gum::NodeId deg_st =
    //         BWPrinter["(fr.lip6.printers.t_degraded)equipState"].id();
    //     gum::NodeId st_st =
    //         BWPrinter["(fr.lip6.printers.t_state)equipState"].id();
    //     gum::NodeId bool_st = BWPrinter["(boolean)equipState"].id();
    //     gum::NodeId hp = BWPrinter["hasPaper"].id();
    //     gum::NodeId bool_hp = BWPrinter["(boolean)hasPaper"].id();
    //     gum::NodeId st_hp =
    //         BWPrinter["(fr.lip6.printers.t_state)hasPaper"].id();
    //     gum::NodeId p_hp =
    //     BWPrinter["(fr.lip6.printers.t_paper)hasPaper"].id();
    //     gum::NodeId hi = BWPrinter["hasInk"].id();
    //     gum::NodeId bool_hi = BWPrinter["(boolean)hasInk"].id();
    //     gum::NodeId st_hi =
    //     BWPrinter["(fr.lip6.printers.t_state)hasInk"].id();
    //     gum::NodeId i_hi = BWPrinter["(fr.lip6.printers.t_ink)hasInk"].id();
    //     CHECK((st) == (deg_st));
    //     CHECK((st) != (st_st));
    //     CHECK((st) != (bool_st));
    //     CHECK((st_st) != (bool_st));
    //     CHECK((hp) == (p_hp));
    //     CHECK((hp) != (st_hp));
    //     CHECK((hp) != (bool_hp));
    //     CHECK((hi) == (i_hi));
    //     CHECK((hi) != (st_hi));
    //     CHECK((hi) != (bool_hi));
    //     // t_degraded equipState
    //     CHECK( BWPrinter.isOutputNode(
    //         BWPrinter["(fr.lip6.printers.t_degraded)equipState"] ) );
    //     CHECK( BWPrinter.isInputNode(
    //         BWPrinter["(fr.lip6.printers.t_degraded)equipState"] ) );
    //     CHECK( not BWPrinter.isInnerNode(
    //         BWPrinter["(fr.lip6.printers.t_degraded)equipState"] ) );
    //     // t_state equipState
    //     CHECK( BWPrinter.isOutputNode(
    //         BWPrinter["(fr.lip6.printers.t_state)equipState"] ) );
    //     CHECK( not BWPrinter.isInputNode(
    //         BWPrinter["(fr.lip6.printers.t_state)equipState"] ) );
    //     CHECK( not BWPrinter.isInnerNode(
    //         BWPrinter["(fr.lip6.printers.t_state)equipState"] ) );
    //     // boolean equipState
    //     CHECK(
    //         not BWPrinter.isOutputNode( BWPrinter["(boolean)equipState"] ) );
    //     CHECK(
    //         not BWPrinter.isInputNode( BWPrinter["(boolean)equipState"] ) );
    //     CHECK( BWPrinter.isInnerNode( BWPrinter["(boolean)equipState"] )
    //     );
    //     // t_paper hasPaper
    //     CHECK( not BWPrinter.isOutputNode(
    //         BWPrinter["(fr.lip6.printers.t_paper)hasPaper"] ) );
    //     CHECK( not BWPrinter.isInputNode(
    //         BWPrinter["(fr.lip6.printers.t_paper)hasPaper"] ) );
    //     CHECK( BWPrinter.isInnerNode(
    //         BWPrinter["(fr.lip6.printers.t_paper)hasPaper"] ) );
    //     // t_state hasInk
    //     CHECK( not BWPrinter.isOutputNode(
    //         BWPrinter["(fr.lip6.printers.t_ink)hasInk"] ) );
    //     CHECK( not BWPrinter.isInputNode(
    //         BWPrinter["(fr.lip6.printers.t_ink)hasInk"] ) );
    //     CHECK( BWPrinter.isInnerNode(
    //         BWPrinter["(fr.lip6.printers.t_ink)hasInk"] ) );
    //     // Boolean hasPaper
    //     CHECK(
    //         not BWPrinter.isOutputNode( BWPrinter["(boolean)hasPaper"] ) );
    //     CHECK(
    //         not BWPrinter.isInputNode( BWPrinter["(boolean)hasPaper"] ) );
    //     CHECK( BWPrinter.isInnerNode( BWPrinter["(boolean)hasPaper"] ) );
    //     // Boolean hasInk
    //     CHECK( not BWPrinter.isOutputNode( BWPrinter["(boolean)hasInk"] )
    //     );
    //     CHECK( not BWPrinter.isInputNode( BWPrinter["(boolean)hasInk"] )
    //     );
    //     CHECK( BWPrinter.isInnerNode( BWPrinter["hasInk"] ) );
    //     delete prm;
    //   } catch ( gum::Exception ) {
    //     CHECK( false );
    //   }
    // }

    /// @TODO Make this test pass for IO Flags optimisation
    // Testing class ColorPrinter
    // static void testComplexPrinters_9() {
    //  try {
    //    gum::prm::o3prm::O3prmReader<double> reader;
    //    std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm"
    //    );
    //    std::string package = "fr.lip6.printers";
    //    GUM_CHECK_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
    //    gum::prm::PRM<double>* prm = reader.prm();
    //    GUM_CHECK_ASSERT_THROWS_NOTHING(
    //        prm->getClass( "fr.lip6.printers.ColorPrinter" ) );
    //    gum::prm::Class<double>& ColorPrinter =
    //        prm->getClass( "fr.lip6.printers.ColorPrinter" );
    //    CHECK((ColorPrinter.referenceSlots().size()) == (static_cast<gum::Size>(1)
    //));
    //    CHECK((ColorPrinter.attributes().size()) == (static_cast<gum::Size>(19)));
    //    CHECK((ColorPrinter.aggregates().size()) == (static_cast<gum::Size>(0)));
    //    CHECK((ColorPrinter.slotChains().size()) == (static_cast<gum::Size>(1)));
    //    GUM_CHECK_ASSERT_THROWS_NOTHING( ColorPrinter["room"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)equipState"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)equipState"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_degraded)equipState"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING( ColorPrinter["equipState"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)hasPaper"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)hasPaper"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_paper)hasPaper"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING( ColorPrinter["hasPaper"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)hasInk"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)black"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)black"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_ink)black"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING( ColorPrinter["black"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)magenta"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)magenta"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_ink)magenta"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING( ColorPrinter["magenta"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)yellow"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)yellow"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_ink)yellow"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING( ColorPrinter["yellow"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)cyan"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)cyan"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_ink)cyan"] );
    //    GUM_CHECK_ASSERT_THROWS_NOTHING( ColorPrinter["cyan"] );
    //    // Testing gum::NodeId, inputs and outputs
    //    gum::NodeId st = ColorPrinter["equipState"].id();
    //    gum::NodeId deg_st =
    //        ColorPrinter["(fr.lip6.printers.t_degraded)equipState"].id();
    //    gum::NodeId st_st =
    //        ColorPrinter["(fr.lip6.printers.t_state)equipState"].id();
    //    gum::NodeId bool_st = ColorPrinter["(boolean)equipState"].id();
    //    gum::NodeId hp = ColorPrinter["hasPaper"].id();
    //    gum::NodeId bool_hp = ColorPrinter["(boolean)hasPaper"].id();
    //    gum::NodeId st_hp =
    //        ColorPrinter["(fr.lip6.printers.t_state)hasPaper"].id();
    //    gum::NodeId p_hp =
    //        ColorPrinter["(fr.lip6.printers.t_paper)hasPaper"].id();
    //    gum::NodeId hi = ColorPrinter["hasInk"].id();
    //    gum::NodeId bool_hi = ColorPrinter["(boolean)hasInk"].id();
    //    gum::NodeId bl = ColorPrinter["black"].id();
    //    gum::NodeId bool_bl = ColorPrinter["(boolean)black"].id();
    //    gum::NodeId st_bl =
    //        ColorPrinter["(fr.lip6.printers.t_state)black"].id();
    //    gum::NodeId i_bl = ColorPrinter["(fr.lip6.printers.t_ink)black"].id();
    //    gum::NodeId ma = ColorPrinter["magenta"].id();
    //    gum::NodeId bool_ma = ColorPrinter["(boolean)magenta"].id();
    //    gum::NodeId st_ma =
    //        ColorPrinter["(fr.lip6.printers.t_state)magenta"].id();
    //    gum::NodeId i_ma =
    //    ColorPrinter["(fr.lip6.printers.t_ink)magenta"].id();
    //    gum::NodeId ye = ColorPrinter["yellow"].id();
    //    gum::NodeId bool_ye = ColorPrinter["(boolean)yellow"].id();
    //    gum::NodeId st_ye =
    //        ColorPrinter["(fr.lip6.printers.t_state)yellow"].id();
    //    gum::NodeId i_ye =
    //    ColorPrinter["(fr.lip6.printers.t_ink)yellow"].id();
    //    gum::NodeId ce = ColorPrinter["cyan"].id();
    //    gum::NodeId bool_ce = ColorPrinter["(boolean)cyan"].id();
    //    gum::NodeId st_ce =
    //    ColorPrinter["(fr.lip6.printers.t_state)cyan"].id();
    //    gum::NodeId i_ce = ColorPrinter["(fr.lip6.printers.t_ink)cyan"].id();
    //    CHECK((st) == (deg_st));
    //    CHECK((st) != (st_st));
    //    CHECK((st) != (bool_st));
    //    CHECK((st_st) != (bool_st));
    //    CHECK((hp) == (p_hp));
    //    CHECK((hp) != (st_hp));
    //    CHECK((hp) != (bool_hp));
    //    CHECK((hi) == (bool_hi));
    //    CHECK((bl) == (i_bl));
    //    CHECK((bl) != (st_bl));
    //    CHECK((bl) != (bool_bl));
    //    CHECK((st_bl) != (bool_bl));
    //    CHECK((ma) == (i_ma));
    //    CHECK((ma) != (st_ma));
    //    CHECK((ma) != (bool_ma));
    //    CHECK((st_ma) != (bool_ma));
    //    CHECK((ye) == (i_ye));
    //    CHECK((ye) != (st_ye));
    //    CHECK((ye) != (bool_ye));
    //    CHECK((st_ye) != (bool_ye));
    //    CHECK((ce) == (i_ce));
    //    CHECK((ce) != (st_ce));
    //    CHECK((ce) != (bool_ce));
    //    CHECK((st_ce) != (bool_ce));
    //    // t_degraded equipState
    //    CHECK( ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_degraded)equipState"] ) );
    //    CHECK( ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_degraded)equipState"] ) );
    //    CHECK( not ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_degraded)equipState"] ) );
    //    // t_state equipState
    //    CHECK( ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)equipState"] ) );
    //    CHECK( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)equipState"] ) );
    //    CHECK( not ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)equipState"] ) );
    //    // boolean equipState
    //    CHECK( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(boolean)equipState"] ) );
    //    CHECK( not ColorPrinter.isInputNode(
    //        ColorPrinter["(boolean)equipState"] ) );
    //    CHECK(
    //        ColorPrinter.isInnerNode( ColorPrinter["(boolean)equipState"] ) );
    //    // t_paper hasPaper
    //    CHECK( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_paper)hasPaper"] ) );
    //    CHECK( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_paper)hasPaper"] ) );
    //    CHECK( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_paper)hasPaper"] ) );
    //    // Boolean hasPaper
    //    CHECK( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(boolean)hasPaper"] ) );
    //    CHECK(
    //        not ColorPrinter.isInputNode( ColorPrinter["(boolean)hasPaper"] )
    //        );
    //    CHECK(
    //        ColorPrinter.isInnerNode( ColorPrinter["(boolean)hasPaper"] ) );
    //    // Boolean hasInk
    //    CHECK(
    //        not ColorPrinter.isOutputNode( ColorPrinter["(boolean)hasInk"] )
    //        );
    //    CHECK(
    //        not ColorPrinter.isInputNode( ColorPrinter["(boolean)hasInk"] ) );
    //    CHECK(
    //        ColorPrinter.isInnerNode( ColorPrinter["(boolean)hasInk"] ) );
    //    // t_ink black
    //    CHECK( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)black"] ) );
    //    CHECK( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)black"] ) );
    //    CHECK( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)black"] ) );
    //    // t_state black
    //    CHECK( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)black"] ) );
    //    CHECK( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)black"] ) );
    //    CHECK( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)black"] ) );
    //    // boolean black
    //    CHECK(
    //        not ColorPrinter.isOutputNode( ColorPrinter["(boolean)black"] ) );
    //    CHECK(
    //        not ColorPrinter.isInputNode( ColorPrinter["(boolean)black"] ) );
    //    CHECK( ColorPrinter.isInnerNode( ColorPrinter["(boolean)black"] )
    //    );
    //    // t_ink magenta
    //    CHECK( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)magenta"] ) );
    //    CHECK( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)magenta"] ) );
    //    CHECK( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)magenta"] ) );
    //    // t_state magenta
    //    CHECK( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)magenta"] ) );
    //    CHECK( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)magenta"] ) );
    //    CHECK( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)magenta"] ) );
    //    // boolean magenta
    //    CHECK(
    //        not ColorPrinter.isOutputNode( ColorPrinter["(boolean)magenta"] )
    //        );
    //    CHECK(
    //        not ColorPrinter.isInputNode( ColorPrinter["(boolean)magenta"] )
    //        );
    //    CHECK(
    //        ColorPrinter.isInnerNode( ColorPrinter["(boolean)magenta"] ) );
    //    // t_ink yellow
    //    CHECK( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)yellow"] ) );
    //    CHECK( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)yellow"] ) );
    //    CHECK( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)yellow"] ) );
    //    // t_state yellow
    //    CHECK( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)yellow"] ) );
    //    CHECK( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)yellow"] ) );
    //    CHECK( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)yellow"] ) );
    //    // boolean yellow
    //    CHECK(
    //        not ColorPrinter.isOutputNode( ColorPrinter["(boolean)yellow"] )
    //        );
    //    CHECK(
    //        not ColorPrinter.isInputNode( ColorPrinter["(boolean)yellow"] ) );
    //    CHECK(
    //        ColorPrinter.isInnerNode( ColorPrinter["(boolean)yellow"] ) );
    //    // t_ink cyan
    //    CHECK( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)cyan"] ) );
    //    CHECK( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)cyan"] ) );
    //    CHECK( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)cyan"] ) );
    //    // t_state cyan
    //    CHECK( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)cyan"] ) );
    //    CHECK( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)cyan"] ) );
    //    CHECK( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)cyan"] ) );
    //    // boolean cyan
    //    CHECK(
    //        not ColorPrinter.isOutputNode( ColorPrinter["(boolean)cyan"] ) );
    //    CHECK(
    //        not ColorPrinter.isInputNode( ColorPrinter["(boolean)cyan"] ) );
    //    CHECK( ColorPrinter.isInnerNode( ColorPrinter["(boolean)cyan"] )
    //    );
    //    delete prm;
    //  } catch ( gum::Exception ) {
    //    CHECK( false );
    //  }
    //}

    // Testing class Computer
    static void testComplexPrinters_10() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));
        gum::prm::PRM< double >*      prm      = reader.prm();
        gum::prm::PRMClass< double >& Computer = prm->getClass("fr.lip6.printers.Computer");
        CHECK((Computer.attributes().size()) == (static_cast< gum::Size >(4)));
        CHECK((Computer.referenceSlots().size()) == (static_cast< gum::Size >(2)));
        CHECK((Computer.aggregates().size()) == (static_cast< gum::Size >(4)));
        CHECK((Computer.slotChains().size()) == (static_cast< gum::Size >(2)));
        GUM_CHECK_ASSERT_THROWS_NOTHING(Computer["(boolean)functional_printer"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(Computer["functional_printer"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(Computer["(boolean)degraded_printer"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(Computer["degraded_printer"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(Computer["(boolean)working_printer"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(Computer["working_printer"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(Computer["(boolean)equipState"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(Computer["(fr.lip6.printers.t_state)equipState"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(Computer["(fr.lip6.printers.t_degraded)equipState"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(Computer["equipState"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(Computer["(boolean)can_print"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(Computer["can_print"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(Computer["printers.equipState"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(Computer["room.power.state"]);
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    // Testing class Computer
    static void testComplexPrinters_11() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));
        gum::prm::PRM< double >*      prm          = reader.prm();
        gum::prm::PRMClass< double >& SafeComputer = prm->getClass("fr.lip6.printers.SafeComputer");
        CHECK((SafeComputer.attributes().size()) == (static_cast< gum::Size >(4)));
        CHECK((SafeComputer.referenceSlots().size()) == (static_cast< gum::Size >(2)));
        CHECK((SafeComputer.aggregates().size()) == (static_cast< gum::Size >(3)));
        CHECK((SafeComputer.slotChains().size()) == (static_cast< gum::Size >(3)));
        GUM_CHECK_ASSERT_THROWS_NOTHING(SafeComputer["(boolean)functional_printer"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(SafeComputer["functional_printer"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(SafeComputer["(boolean)degraded_printer"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(SafeComputer["degraded_printer"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(SafeComputer["(boolean)working_printer"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(SafeComputer["working_printer"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(SafeComputer["(boolean)equipState"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(SafeComputer["(fr.lip6.printers.t_state)equipState"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(SafeComputer["(fr.lip6.printers.t_degraded)equipState"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(SafeComputer["equipState"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(SafeComputer["(boolean)can_print"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(SafeComputer["can_print"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(
            SafeComputer["printers.(fr.lip6.printers.t_state)equipState"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(
            SafeComputer["printers.(fr.lip6.printers.t_degraded)equipState"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(SafeComputer["room.power.(fr.lip6.printers.t_state)state"]);
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    // Testing class Computer
    static void testComplexPrinters_12() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));
        gum::prm::PRM< double >*      prm          = reader.prm();
        gum::prm::PRMClass< double >& SafeComputer = prm->getClass("fr.lip6.printers.SafeComputer");
        CHECK((SafeComputer.attributes().size()) == (static_cast< gum::Size >(4)));
        CHECK(SafeComputer.exists("can_print"));
        const auto& cpf = SafeComputer.get("can_print").cpf();
        auto        set = gum::VariableSet();
        set.insert(&(SafeComputer.get("can_print").type().variable()));
        set.insert(&(SafeComputer.get("working_printer").type().variable()));
        set.insert(&(SafeComputer.get("equipState").type().variable()));
        for (auto var: cpf.variablesSequence()) {
          if (set.contains(var)) { set.erase(var); }
        }
        CHECK((set.size()) == (static_cast< gum::Size >(0)));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    // Testing class Computer
    static void testComplexPrinters_13() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));
        gum::prm::PRM< double >*      prm          = reader.prm();
        gum::prm::PRMClass< double >& SafeComputer = prm->getClass("fr.lip6.printers.Computer");
        CHECK((SafeComputer.attributes().size()) == (static_cast< gum::Size >(4)));
        CHECK(SafeComputer.exists("equipState"));
        const auto& cpf = SafeComputer.get("equipState").cpf();
        auto        set = gum::VariableSet();
        set.insert(&(SafeComputer.get("equipState").type().variable()));
        set.insert(&(SafeComputer.get("room.power.state").type().variable()));
        for (auto var: cpf.variablesSequence()) {
          if (set.contains(var)) { set.erase(var); }
        }
        CHECK((set.size()) == (static_cast< gum::Size >(0)));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    // Testing class Computer
    static void testComplexPrinters_14() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));
        gum::prm::PRM< double >*      prm          = reader.prm();
        gum::prm::PRMClass< double >& SafeComputer = prm->getClass("fr.lip6.printers.SafeComputer");
        CHECK((SafeComputer.attributes().size()) == (static_cast< gum::Size >(4)));
        CHECK(SafeComputer.exists("equipState"));
        const auto& cpf = SafeComputer.get("equipState").cpf();
        auto        set = gum::VariableSet();
        set.insert(&(SafeComputer.get("equipState").type().variable()));
        set.insert(
            &(SafeComputer.get("room.power.(fr.lip6.printers.t_state)state").type().variable()));
        for (auto var: cpf.variablesSequence()) {
          if (set.contains(var)) { set.erase(var); }
        }
        CHECK((set.size()) == (static_cast< gum::Size >(0)));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testPrintersSystems1() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        GUM_CHECK_ASSERT_THROWS_NOTHING(
            reader.readFile(GET_RESSOURCES_PATH("o3prm/printers_systems.o3prm")));
        CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));
        CHECK((reader.errors()) == (static_cast< gum::Size >(0)));
        reader.showElegantErrorsAndWarnings();
        gum::prm::PRM< double >* prm = 0;
        GUM_CHECK_ASSERT_THROWS_NOTHING(prm = reader.prm());

        if (prm) { delete prm; }
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testComplexPrintersSystems1() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        GUM_CHECK_ASSERT_THROWS_NOTHING(
            reader.readFile(GET_RESSOURCES_PATH("o3prm/complexprinters_system.o3prm")));
        CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));
        CHECK((reader.errors()) == (static_cast< gum::Size >(0)));

        if (reader.errors()) { reader.showElegantErrorsAndWarnings(); }

        gum::prm::PRM< double >* prm = nullptr;
        GUM_CHECK_ASSERT_THROWS_NOTHING(prm = reader.prm());
        gum::prm::PRMSystem< double >* sys = nullptr;
        GUM_CHECK_ASSERT_THROWS_NOTHING(sys = &(prm->getSystem("aSys")));
        gum::prm::PRMClass< double >& Power        = prm->getClass("PowerSupply");
        gum::prm::PRMClass< double >& Room         = prm->getClass("Room");
        gum::prm::PRMClass< double >& BWPrinter    = prm->getClass("BWPrinter");
        gum::prm::PRMClass< double >& ColorPrinter = prm->getClass("ColorPrinter");
        gum::prm::PRMClass< double >& Computer     = prm->getClass("Computer");
        try {
          sys->get(Power).size();
        } catch (gum::Exception& e) {
          std::cout << e.errorType();
          std::cout << e.errorContent();
          std::cout << e.errorCallStack();
        }
        CHECK((sys->get(Power).size()) == (static_cast< gum::Size >(1)));
        CHECK((sys->get(Room).size()) == (static_cast< gum::Size >(1)));
        CHECK((sys->get(BWPrinter).size()) == (static_cast< gum::Size >(10)));
        CHECK((sys->get(ColorPrinter).size()) == (static_cast< gum::Size >(2)));
        CHECK((sys->get(Computer).size()) == (static_cast< gum::Size >(2)));
        int count = 0;

        for (auto iter = sys->begin(); iter != sys->end(); ++iter) {
          ++count;
        }

        CHECK((count) == (18));

        if (prm) { delete prm; }
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testCPF() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        GUM_CHECK_ASSERT_THROWS_NOTHING(
            reader.readFile(GET_RESSOURCES_PATH("o3prm/inference.o3prm")));
        CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));
        CHECK((reader.errors()) == (static_cast< gum::Size >(0)));
        gum::prm::PRM< double >* prm = nullptr;
        GUM_CHECK_ASSERT_THROWS_NOTHING(prm = reader.prm());
        gum::prm::PRMSystem< double >* sys = nullptr;
        GUM_CHECK_ASSERT_THROWS_NOTHING(sys = &(prm->getSystem("aSys")));

        for (gum::prm::PRMSystem< double >::iterator iter = sys->begin(); iter != sys->end();
             ++iter) {
          for (gum::prm::PRMInstance< double >::iterator jter = (*(iter.val())).begin();
               jter != (*(iter.val())).end();
               ++jter) {
            gum::Instantiation i((*(jter.val())).cpf()), var;
            var.add((*(jter.val())).type().variable());

            for (i.setFirstOut(var); !i.end(); i.incOut(var)) {
              double f = 0.0;

              for (i.setFirstIn(var); !i.end(); i.incIn(var)) {
                f += (*(jter.val())).cpf().get(i);
              }

              CHECK((f) == doctest::Approx(1.0).epsilon(GUM_SMALL_ERROR));
              i.unsetOverflow();
            }
          }
        }

        if (prm) { delete prm; }
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testImports() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        reader.addClassPath(GET_RESSOURCES_PATH("o3prmr"));
        GUM_CHECK_ASSERT_THROWS_NOTHING(
            reader.readFile(GET_RESSOURCES_PATH("o3prmr/systems/MySystem.o3prm"),
                            "systems.MySystem"));
        CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));
        CHECK((reader.errors()) == (static_cast< gum::Size >(0)));
        reader.showElegantErrorsAndWarnings();

        gum::prm::PRM< double >* prm = nullptr;

        GUM_CHECK_ASSERT_THROWS_NOTHING(prm = reader.prm());
        gum::Size class_count = prm->classes().size();
        class_count += prm->interfaces().size();
        CHECK((class_count) == (static_cast< gum::Size >(11)));   // Don't forget param subclasses !

        CHECK((prm->systems().size()) == (static_cast< gum::Size >(1)));
        gum::prm::PRMSystem< double >* sys = nullptr;
        GUM_CHECK_ASSERT_THROWS_NOTHING(sys = &(prm->getSystem("systems.MySystem.MySystem")));
        CHECK((sys->size()) == (static_cast< gum::Size >(16)));

        // Checking that all class DAG are generated
        for (auto c: prm->classes()) {
          for (auto node = c->containerDag().begin(); node != c->containerDag().end(); ++node) {
            CHECK(c->exists(*node));
            CHECK_NOTHROW(c->get(*node));
            for (auto prnt: c->containerDag().parents(*node)) {
              CHECK(c->exists(prnt));
              CHECK_NOTHROW(c->get(prnt));
            }

            for (auto child: c->containerDag().children(*node)) {
              CHECK(c->exists(child));
              CHECK_NOTHROW(c->get(child));
            }
          }
          // checking parameters
          for (auto elt: c->parameters()) {
            c->containerDag().exists(elt->id());
          }
          for (auto elt: c->referenceSlots()) {
            c->containerDag().exists(elt->id());
          }
          for (auto elt: c->attributes()) {
            c->containerDag().exists(elt->id());
          }
          for (auto elt: c->aggregates()) {
            c->containerDag().exists(elt->id());
          }
          for (auto elt: c->slotChains()) {
            c->containerDag().exists(elt->id());
          }
        }

        // Checking that all instances attributes are generated
        for (auto i = sys->begin(); i != sys->end(); ++i) {
          for (auto attr = i.val()->begin(); attr != i.val()->end(); ++attr) {
            try {
              auto inst = gum::Instantiation(attr.val()->cpf());
              auto sum  = 0.0;
              for (inst.setFirst(); !inst.end(); inst.inc()) {
                sum += attr.val()->cpf()[inst];
              }
              gum::Size card = 1;
              for (auto var: attr.val()->cpf().variablesSequence()) {
                card *= var->domainSize();
              }
              card /= attr.val()->type()->domainSize();

              CHECK((sum / card) == doctest::Approx(1.0).epsilon(1e-6));
            } catch (gum::Exception& e) { FAIL(e.errorContent()); }
          }
        }

        if (prm) { delete prm; }
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testParamClass() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));
        gum::prm::PRM< double >*      prm        = reader.prm();
        gum::prm::PRMClass< double >& ParamClass = prm->getClass("fr.lip6.printers.ParamClass");
        CHECK((ParamClass.attributes().size()) == (static_cast< gum::Size >(9)));
        CHECK((ParamClass.referenceSlots().size()) == (static_cast< gum::Size >(1)));
        CHECK((ParamClass.slotChains().size()) == (static_cast< gum::Size >(1)));
        GUM_CHECK_ASSERT_THROWS_NOTHING(ParamClass["lambda"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(ParamClass["t"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(ParamClass["room"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(ParamClass["hasInk"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(ParamClass["hasPaper"]);
        GUM_CHECK_ASSERT_THROWS_NOTHING(ParamClass["equipState"]);

        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testSystemParamClass() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters_system.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));
        gum::prm::PRM< double >* prm = reader.prm();

        GUM_CHECK_ASSERT_THROWS_NOTHING(prm->getClass("fr.lip6.printers.ParamClass"));
        GUM_CHECK_ASSERT_THROWS_NOTHING(
            prm->getClass("fr.lip6.printers.ParamClass<lambda=0.4,t=4>"));
        GUM_CHECK_ASSERT_THROWS_NOTHING(
            prm->getClass("fr.lip6.printers.ParamClass<lambda=0.001,t=4>"));

        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testParamClassHasInk() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));

        auto  prm        = reader.prm();
        auto& ParamClass = prm->getClass("fr.lip6.printers.ParamClass");

        const auto& hasInk = static_cast< gum::prm::PRMAttribute< double >& >(ParamClass["hasInk"]);

        gum::Instantiation    inst(hasInk.cpf());
        std::vector< double > values;
        for (inst.setFirst(); !inst.end(); inst.inc()) {
          values.push_back(hasInk.cpf()[inst]);
        }

        CHECK((values.size()) == ((size_t)2));
        CHECK((values[0]) == doctest::Approx(0.4).epsilon(1e-6));
        CHECK((values[1]) == doctest::Approx(0.6).epsilon(1e-6));

        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testParamClassHasPaper() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));

        auto  prm        = reader.prm();
        auto& ParamClass = prm->getClass("fr.lip6.printers.ParamClass");

        const auto& hasPaper
            = static_cast< gum::prm::PRMAttribute< double >& >(ParamClass["hasPaper"]);

        gum::Instantiation    inst(hasPaper.cpf());
        std::vector< double > values;
        for (inst.setFirst(); !inst.end(); inst.inc()) {
          values.push_back(hasPaper.cpf()[inst]);
        }

        CHECK((values.size()) == ((size_t)3));
        CHECK((values[0]) == doctest::Approx(std::exp(-0.4 * 4)).epsilon(1e-6));
        CHECK((values[1]) == doctest::Approx(1 - std::exp(-0.4 * 4)).epsilon(1e-6));
        CHECK((values[2]) == doctest::Approx(0.0).epsilon(1e-6));

        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testInstanceParamClassHasInk() {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters_system.o3prm");
        std::string package = "fr.lip6.printers";
        GUM_CHECK_ASSERT_THROWS_NOTHING(reader.readFile(file, package));

        auto  prm      = reader.prm();
        auto& sys      = prm->getSystem("fr.lip6.printers.aSys");
        auto& paramBis = sys.get("paramBis");

        const auto& hasInk = paramBis.get("hasInk");

        gum::Instantiation    inst(hasInk.cpf());
        std::vector< double > values;
        for (inst.setFirst(); !inst.end(); inst.inc()) {
          values.push_back(hasInk.cpf()[inst]);
        }

        CHECK((values.size()) == ((size_t)2));
        CHECK((values[0]) == doctest::Approx(0.001).epsilon(1e-6));
        CHECK((values[1]) == doctest::Approx(0.999).epsilon(1e-6));

        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testAsiaBN() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string                            file    = GET_RESSOURCES_PATH("o3prm/Asia.o3prm");
        std::string                            package = "";
        reader.readFile(file, package);
        auto                                prm  = reader.prm();
        gum::prm::PRMClass< double > const* asia = nullptr;
        // Act
        CHECK_NOTHROW(asia = &(prm->getClass("Asia")));
        // Assert
        CHECK((asia->attributes().size()) == (static_cast< gum::Size >(8)));
        CHECK((asia->containerDag().sizeArcs()) == (static_cast< gum::Size >(8)));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testAsiaBNGrd() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string                            file    = GET_RESSOURCES_PATH("o3prm/Asia.o3prm");
        std::string                            package = "";
        reader.readFile(file, package);
        auto                          prm  = reader.prm();
        auto&                         asia = prm->getClass("Asia");
        gum::prm::PRMSystem< double > sys("Asia");
        auto                          i = new gum::prm::PRMInstance< double >("asia", asia);
        sys.add(i);
        sys.instantiate();
        auto                           bn = new gum::BayesNet< double >("Asia");
        gum::BayesNetFactory< double > factory(bn);
        // Act
        CHECK_NOTHROW(sys.groundedBN(factory));
        // Assert
        CHECK((bn->size()) == (static_cast< gum::Size >(8)));
        CHECK((bn->sizeArcs()) == (static_cast< gum::Size >(8)));
        delete prm;
        delete bn;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testVisitAsiaTwoClassesBN() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/AsiaClassAndSystemWithTwoClasses.o3prm");
        std::string package = "";
        reader.readFile(file, package);
        auto                                prm  = reader.prm();
        gum::prm::PRMClass< double > const* asia = nullptr;
        // Act
        CHECK_NOTHROW(asia = &(prm->getClass("VisitAsia")));
        // Assert
        CHECK((asia->attributes().size()) == (static_cast< gum::Size >(2)));
        CHECK((asia->containerDag().sizeArcs()) == (static_cast< gum::Size >(1)));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testAsiaTwoClassesBN() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/AsiaClassAndSystemWithTwoClasses.o3prm");
        std::string package = "";
        reader.readFile(file, package);
        auto                                prm  = reader.prm();
        gum::prm::PRMClass< double > const* asia = nullptr;
        // Act
        CHECK_NOTHROW(asia = &(prm->getClass("Asia")));
        // Assert
        CHECK((asia->attributes().size()) == (static_cast< gum::Size >(6)));
        CHECK((asia->referenceSlots().size()) == (static_cast< gum::Size >(1)));
        CHECK((asia->containerDag().sizeArcs()) == (static_cast< gum::Size >(7)));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testAsiaTwoClassBNGrd() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/AsiaClassAndSystemWithTwoClasses.o3prm");
        std::string package = "";
        reader.readFile(file, package);
        auto  prm = reader.prm();
        auto& sys = prm->getSystem("Asia");
        // sys.instantiate();
        auto                           bn = new gum::BayesNet< double >("Asia");
        gum::BayesNetFactory< double > factory(bn);
        // Act
        CHECK_NOTHROW(sys.groundedBN(factory));
        // Assert
        CHECK((bn->size()) == (static_cast< gum::Size >(8)));
        CHECK((bn->sizeArcs()) == (static_cast< gum::Size >(8)));
        delete prm;
        delete bn;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testFileNotFound() {
      // Arrange
      gum::prm::o3prm::O3prmReader< double > reader;
      std::string                            file    = "ThisFileDoesNotExist.o3prm";
      std::string                            package = "";
      // Act & Assert
      CHECK_NOTHROW(reader.readFile(file, package));
      CHECK((reader.errors()) != (static_cast< gum::Size >(0)));
      if (reader.prm()) { delete reader.prm(); }
    }

    static void testFileNotFoundInResDir() {
      // Arrange
      gum::prm::o3prm::O3prmReader< double > reader;
      std::string file    = GET_RESSOURCES_PATH("o3prm/ThisFileDoesNotExist.o3prm");
      std::string package = "";
      // Act & Assert
      CHECK_NOTHROW(reader.readFile(file, package));
      CHECK((reader.errors()) != (static_cast< gum::Size >(0)));
      if (reader.prm()) { delete reader.prm(); }
    }

    static void testAsiaWithErrors() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/AsiaWithError.o3prm");
        std::string package = "";
        // Act
        CHECK_NOTHROW(reader.readFile(file, package));
        // Assert
        CHECK((reader.errors()) == (static_cast< gum::Size >(1)));
        CHECK((reader.prm()) != (nullptr));
        delete reader.prm();
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testAggOrAnd() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string                            file = GET_RESSOURCES_PATH("o3prm/aggregates.o3prm");
        std::string                            package = "";
        // Act
        CHECK_NOTHROW(reader.readFile(file, package));
        // Assert
        CHECK((reader.errors()) == (static_cast< gum::Size >(0)));
        if (reader.errors()) { reader.showElegantErrorsAndWarnings(); }
        CHECK((reader.prm()) != (nullptr));
        delete reader.prm();
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testActors() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string                            file    = GET_RESSOURCES_PATH("o3prm/acteurs.o3prm");
        std::string                            package = "";
        // Act
        CHECK_NOTHROW(reader.readFile(file, package));
        // Assert
        CHECK((reader.errors()) == (static_cast< gum::Size >(0)));
        if (reader.errors()) { reader.showElegantErrorsAndWarnings(); }
        CHECK((reader.prm()) != (nullptr));
        delete reader.prm();
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testStudents() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prmr/University/fr/base.o3prm");
        std::string package = "fr.base";
        // Act
        CHECK_NOTHROW(reader.readFile(file, package));
        // Assert
        CHECK((reader.errors()) == (static_cast< gum::Size >(0)));
        if (reader.errors()) { reader.showElegantErrorsAndWarnings(); }
        CHECK((reader.prm()) != (nullptr));

        auto sys = &(reader.prm()->getSystem("fr.base.Work"));
        for (auto iter: *sys) {
          auto inst = iter.second;
          for (auto node: inst->type().containerDag()) {
            // CHECK( inst->exists( node ) );
            if ((!inst->exists(node)) && inst->type().exists(node)) {
              auto elt = &(inst->type().get(node));
              if (gum::prm::PRMClassElement< double >::isAttribute(*elt)
                  || gum::prm::PRMClassElement< double >::isAggregate(*elt)) {
                CHECK(false);
              }
            }
          }
        }

        delete reader.prm();
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testReferenceID() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prmr/University/fr/base.o3prm");
        std::string package = "";
        // Act
        CHECK_NOTHROW(reader.readFile(file, package));
        // Assert
        CHECK((reader.errors()) == (static_cast< gum::Size >(0)));
        if (reader.errors()) { reader.showElegantErrorsAndWarnings(); }

        CHECK((reader.prm()) != (nullptr));

        auto prm = reader.prm();
        for (auto c: prm->classes()) {
          try {
            for (auto i: c->implements()) {
              for (auto r: i->referenceSlots()) {
                CHECK((r->name()) == (c->get(r->id()).name()));
                CHECK((r->safeName()) == (c->get(r->id()).safeName()));
                CHECK((r->id()) == (c->get(r->name()).id()));
              }
              for (auto r: c->referenceSlots()) {
                if (i->exists(r->name())) {
                  CHECK((r->name()) == (i->get(r->id()).name()));
                  CHECK((r->safeName()) == (i->get(r->id()).safeName()));
                  CHECK((r->id()) == (i->get(r->name()).id()));
                }
              }
            }
          } catch (gum::NotFound&) {
            // No PRMInterface
          }
        }

        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testAttributeID() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prmr/University/fr/base.o3prm");
        std::string package = "";
        // Act
        CHECK_NOTHROW(reader.readFile(file, package));
        // Assert
        CHECK((reader.errors()) == (static_cast< gum::Size >(0)));
        if (reader.errors()) { reader.showElegantErrorsAndWarnings(); }

        CHECK((reader.prm()) != (nullptr));

        auto prm = reader.prm();
        for (auto c: prm->classes()) {
          try {
            for (auto i: c->implements()) {
              for (auto a: i->attributes()) {
                CHECK(c->exists(a->name()));
                CHECK(c->exists(a->safeName()));
                CHECK(c->exists(a->id()));
                CHECK((a->safeName()) == (c->get(a->id()).safeName()));
                CHECK((a->id()) == (c->get(a->id()).id()));
              }
            }
          } catch (gum::NotFound&) {
            // No interface
          }
        }

        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testArrays() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string                            file    = GET_RESSOURCES_PATH("o3prm/arrays.o3prm");
        std::string                            package = "";
        // Act
        CHECK_NOTHROW(reader.readFile(file, package));
        // Assert
        CHECK((reader.errors()) == (static_cast< gum::Size >(0)));
        if (reader.errors()) { reader.showElegantErrorsAndWarnings(); }

        CHECK((reader.prm()) != (nullptr));

        if (reader.prm() != nullptr) {
          delete reader.prm();
        } else {
          CHECK(false);
        }

      } catch (gum::Exception&) { CHECK(false); }
    }
  };

  GUM_TEST_ACTIF(Types)
  GUM_TEST_ACTIF(SubTypes)
  GUM_TEST_ACTIF(Classes)
  GUM_TEST_ACTIF(PrintersPowerSupply)
  GUM_TEST_ACTIF(PrintersRoom)
  GUM_TEST_ACTIF(PrintersEquipment)
  GUM_TEST_ACTIF(PrintersPrinter)
  GUM_TEST_ACTIF(PrintersComputer)
  GUM_TEST_ACTIF(ComplexPrinters_1)
  GUM_TEST_ACTIF(ComplexPrinters_2)
  GUM_TEST_ACTIF(ComplexPrinters_3)
  GUM_TEST_ACTIF(ComplexPrinters_4)
  GUM_TEST_ACTIF(ComplexPrinters_5)
  GUM_TEST_ACTIF(ComplexPrinters_6)
  GUM_TEST_ACTIF(ComplexPrinters_7)
  GUM_TEST_ACTIF(ComplexPrinters_8)
  GUM_TEST_ACTIF(ComplexPrinters_9)
  GUM_TEST_ACTIF(ComplexPrinters_10)
  GUM_TEST_ACTIF(ComplexPrinters_11)
  GUM_TEST_ACTIF(ComplexPrinters_12)
  GUM_TEST_ACTIF(ComplexPrinters_13)
  GUM_TEST_ACTIF(ComplexPrinters_14)
  GUM_TEST_ACTIF(PrintersSystems1)
  GUM_TEST_ACTIF(ComplexPrintersSystems1)
  GUM_TEST_ACTIF(CPF)
  GUM_TEST_ACTIF(Imports)
  GUM_TEST_ACTIF(ParamClass)
  GUM_TEST_ACTIF(SystemParamClass)
  GUM_TEST_ACTIF(ParamClassHasInk)
  GUM_TEST_ACTIF(ParamClassHasPaper)
  GUM_TEST_ACTIF(InstanceParamClassHasInk)
  GUM_TEST_ACTIF(AsiaBN)
  GUM_TEST_ACTIF(AsiaBNGrd)
  GUM_TEST_ACTIF(VisitAsiaTwoClassesBN)
  GUM_TEST_ACTIF(AsiaTwoClassesBN)
  GUM_TEST_ACTIF(AsiaTwoClassBNGrd)
  GUM_TEST_ACTIF(FileNotFound)
  GUM_TEST_ACTIF(FileNotFoundInResDir)
  GUM_TEST_ACTIF(AsiaWithErrors)
  GUM_TEST_ACTIF(AggOrAnd)
  GUM_TEST_ACTIF(Actors)
  GUM_TEST_ACTIF(Students)
  GUM_TEST_ACTIF(ReferenceID)
  GUM_TEST_ACTIF(AttributeID)
  GUM_TEST_ACTIF(Arrays)

}   // namespace gum_tests
