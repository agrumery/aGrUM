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


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphs/graphElements.h>
#include <agrum/PRM/elements/PRMFormAttribute.h>
#include <agrum/PRM/o3prm/O3prmReader.h>

#include <agrum/base/core/math/math_utils.h>

namespace gum_tests {

  class [[maybe_unused]] O3prmTestSuite: public CxxTest::TestSuite {
    public:
    void setUp() {
      // std::cerr << std::endl;
    }

    void tearDown() {}

    GUM_ACTIVE_TEST(Types) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(GET_RESSOURCES_PATH("o3prm/types.o3prm")))
        TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
        TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
        reader.showElegantErrorsAndWarnings(std::cout);
        gum::prm::PRM< double >* prm = reader.prm();
        TS_ASSERT_EQUALS(prm->type("t_state").variable().domainSize(), (gum::Size)2)
        TS_ASSERT_EQUALS(prm->type("t_ink").variable().domainSize(), (gum::Size)2)
        TS_ASSERT_EQUALS(prm->type("t_degraded").variable().domainSize(), (gum::Size)3)
        TS_ASSERT_EQUALS(prm->type("t_bw_p").variable().domainSize(), (gum::Size)4)
        TS_ASSERT_EQUALS(prm->type("t_color_p").variable().domainSize(), (gum::Size)5)
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(SubTypes) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(GET_RESSOURCES_PATH("o3prm/types.o3prm")))
        gum::prm::PRM< double >* prm = reader.prm();
        TS_ASSERT(prm->type("t_state").isSubType())
        TS_ASSERT(prm->type("t_state").isSubTypeOf(prm->type("boolean")))
        TS_ASSERT(prm->type("t_ink").isSubType())
        TS_ASSERT(prm->type("t_ink").isSubTypeOf(prm->type("boolean")))
        TS_ASSERT(prm->type("t_ink").isSubTypeOf(prm->type("t_state")))
        TS_ASSERT(prm->type("t_paper").isSubType())
        TS_ASSERT(prm->type("t_paper").isSubTypeOf(prm->type("boolean")))
        TS_ASSERT(prm->type("t_paper").isSubTypeOf(prm->type("t_state")))
        TS_ASSERT(prm->type("t_degraded").isSubType())
        TS_ASSERT(prm->type("t_degraded").isSubTypeOf(prm->type("boolean")))
        TS_ASSERT(prm->type("t_degraded").isSubTypeOf(prm->type("t_state")))
        TS_ASSERT(prm->type("t_bw_p").isSubType())
        TS_ASSERT(prm->type("t_bw_p").isSubTypeOf(prm->type("boolean")))
        TS_ASSERT(prm->type("t_bw_p").isSubTypeOf(prm->type("t_state")))
        TS_ASSERT(prm->type("t_bw_p").isSubTypeOf(prm->type("t_degraded")))
        TS_ASSERT(prm->type("t_color_p").isSubType())
        TS_ASSERT(prm->type("t_color_p").isSubTypeOf(prm->type("boolean")))
        TS_ASSERT(prm->type("t_color_p").isSubTypeOf(prm->type("t_state")))
        TS_ASSERT(prm->type("t_color_p").isSubTypeOf(prm->type("t_degraded")))
        TS_ASSERT_EQUALS(prm->types().size(), (gum::Size)7)
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(Classes) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(GET_RESSOURCES_PATH("o3prm/printers.o3prm")))
        TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
        TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
        reader.showElegantErrorsAndWarnings();
        gum::prm::PRM< double >* prm = reader.prm();
        TS_ASSERT_EQUALS(prm->classes().size(), (gum::Size)5)
        TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("PowerSupply"))
        TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Room"))
        TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Equipment"))
        TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Printer"))
        TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Computer"))
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(PrintersPowerSupply) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(GET_RESSOURCES_PATH("o3prm/printers.o3prm")))
        gum::prm::PRM< double >* prm = reader.prm();
        TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("PowerSupply"))
        gum::prm::PRMClass< double >& c = prm->getClass("PowerSupply");
        TS_ASSERT_EQUALS(c.attributes().size(), (gum::Size)1)
        TS_ASSERT_EQUALS(c.referenceSlots().size(), (gum::Size)0)
        TS_ASSERT_EQUALS(c.aggregates().size(), (gum::Size)0)
        TS_ASSERT_EQUALS(c.slotChains().size(), (gum::Size)0)
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(PrintersRoom) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(GET_RESSOURCES_PATH("o3prm/printers.o3prm")))
        gum::prm::PRM< double >* prm = reader.prm();
        TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Room"))
        gum::prm::PRMClass< double >& c = prm->getClass("Room");
        TS_ASSERT_EQUALS(c.attributes().size(), (gum::Size)0)
        TS_ASSERT_EQUALS(c.referenceSlots().size(), (gum::Size)1)
        TS_ASSERT_EQUALS(c.aggregates().size(), (gum::Size)0)
        TS_ASSERT_EQUALS(c.slotChains().size(), (gum::Size)0)
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(PrintersEquipment) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(GET_RESSOURCES_PATH("o3prm/printers.o3prm")))
        gum::prm::PRM< double >* prm = reader.prm();
        TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Equipment"))
        gum::prm::PRMClass< double >& c = prm->getClass("Equipment");
        TS_ASSERT_EQUALS(c.attributes().size(), (gum::Size)1)
        TS_ASSERT_EQUALS(c.referenceSlots().size(), (gum::Size)1)
        TS_ASSERT_EQUALS(c.aggregates().size(), (gum::Size)0)
        TS_ASSERT_EQUALS(c.slotChains().size(), (gum::Size)1)
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(PrintersPrinter) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(GET_RESSOURCES_PATH("o3prm/printers.o3prm")))
        gum::prm::PRM< double >* prm = reader.prm();
        TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Printer"))
        gum::prm::PRMClass< double >& c = prm->getClass("Printer");
        TS_ASSERT_EQUALS(c.attributes().size(), (gum::Size)3)
        TS_ASSERT_EQUALS(c.referenceSlots().size(), (gum::Size)1)
        TS_ASSERT_EQUALS(c.aggregates().size(), (gum::Size)0)
        TS_ASSERT_EQUALS(c.slotChains().size(), (gum::Size)1)
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(PrintersComputer) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(GET_RESSOURCES_PATH("o3prm/printers.o3prm")))
        gum::prm::PRM< double >* prm = reader.prm();
        TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("Computer"))
        gum::prm::PRMClass< double >& c = prm->getClass("Computer");
        TS_ASSERT_EQUALS(c.attributes().size(), (gum::Size)2)
        TS_ASSERT_EQUALS(c.referenceSlots().size(), (gum::Size)2)
        TS_ASSERT_EQUALS(c.aggregates().size(), (gum::Size)1)
        TS_ASSERT_EQUALS(c.slotChains().size(), (gum::Size)2)
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    /// Testing classes and interfaces
    GUM_ACTIVE_TEST(ComplexPrinters_1) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
        TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
        reader.showElegantErrorsAndWarnings();
        gum::prm::PRM< double >* prm = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(prm = reader.prm())
        if (prm) {
          TS_ASSERT_EQUALS(prm->classes().size(), (gum::Size)7)
          TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("fr.lip6.printers.PowerSupply"))
          TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("fr.lip6.printers.Room"))
          TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("fr.lip6.printers.BWPrinter"))
          TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("fr.lip6.printers.ColorPrinter"))
          TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("fr.lip6.printers.Computer"))
          TS_ASSERT_EQUALS(prm->interfaces().size(), (gum::Size)2)
          TS_GUM_ASSERT_THROWS_NOTHING(prm->getInterface("fr.lip6.printers.Equipment"))
          TS_GUM_ASSERT_THROWS_NOTHING(prm->getInterface("fr.lip6.printers.Printer"))
          delete prm;
        }
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    /// Testing classes and interfaces inheritance with subtypes methods
    GUM_ACTIVE_TEST(ComplexPrinters_2) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
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
        TS_ASSERT(PowerSupply.isSubTypeOf(PowerSupply))
        TS_ASSERT(!PowerSupply.isSubTypeOf(Room))
        TS_ASSERT(!PowerSupply.isSubTypeOf(Equipment))
        TS_ASSERT(!PowerSupply.isSubTypeOf(Printer))
        TS_ASSERT(!PowerSupply.isSubTypeOf(BWPrinter))
        TS_ASSERT(!PowerSupply.isSubTypeOf(ColorPrinter))
        TS_ASSERT(!PowerSupply.isSubTypeOf(Computer))
        // Testing Room
        TS_ASSERT(!Room.isSubTypeOf(PowerSupply))
        TS_ASSERT(Room.isSubTypeOf(Room))
        TS_ASSERT(!Room.isSubTypeOf(Equipment))
        TS_ASSERT(!Room.isSubTypeOf(Printer))
        TS_ASSERT(!Room.isSubTypeOf(BWPrinter))
        TS_ASSERT(!Room.isSubTypeOf(ColorPrinter))
        TS_ASSERT(!Room.isSubTypeOf(Computer))
        // Testing Equipment
        TS_ASSERT(!Equipment.isSubTypeOf(PowerSupply))
        TS_ASSERT(!Equipment.isSubTypeOf(Room))
        TS_ASSERT(Equipment.isSubTypeOf(Equipment))
        TS_ASSERT(!Equipment.isSubTypeOf(Printer))
        TS_ASSERT(!Equipment.isSubTypeOf(BWPrinter))
        TS_ASSERT(!Equipment.isSubTypeOf(ColorPrinter))
        TS_ASSERT(!Equipment.isSubTypeOf(Computer))
        // Testing Printer
        TS_ASSERT(!Printer.isSubTypeOf(PowerSupply))
        TS_ASSERT(!Printer.isSubTypeOf(Room))
        TS_ASSERT(Printer.isSubTypeOf(Equipment))
        TS_ASSERT(Printer.isSubTypeOf(Printer))
        TS_ASSERT(!Printer.isSubTypeOf(BWPrinter))
        TS_ASSERT(!Printer.isSubTypeOf(ColorPrinter))
        TS_ASSERT(!Printer.isSubTypeOf(Computer))
        // Testing BWPrinter
        TS_ASSERT(!BWPrinter.isSubTypeOf(PowerSupply))
        TS_ASSERT(!BWPrinter.isSubTypeOf(Room))
        TS_ASSERT(BWPrinter.isSubTypeOf(Equipment))
        TS_ASSERT(BWPrinter.isSubTypeOf(Printer))
        TS_ASSERT(BWPrinter.isSubTypeOf(BWPrinter))
        TS_ASSERT(!BWPrinter.isSubTypeOf(ColorPrinter))
        TS_ASSERT(!BWPrinter.isSubTypeOf(Computer))
        // Testing ColorPrinter
        TS_ASSERT(!ColorPrinter.isSubTypeOf(PowerSupply))
        TS_ASSERT(!ColorPrinter.isSubTypeOf(Room))
        TS_ASSERT(ColorPrinter.isSubTypeOf(Equipment))
        TS_ASSERT(ColorPrinter.isSubTypeOf(Printer))
        TS_ASSERT(!ColorPrinter.isSubTypeOf(BWPrinter))
        TS_ASSERT(ColorPrinter.isSubTypeOf(ColorPrinter))
        TS_ASSERT(!ColorPrinter.isSubTypeOf(Computer))
        // Testing Computer
        TS_ASSERT(!Computer.isSubTypeOf(PowerSupply))
        TS_ASSERT(!Computer.isSubTypeOf(Room))
        TS_ASSERT(Computer.isSubTypeOf(Equipment))
        TS_ASSERT(!Computer.isSubTypeOf(Printer))
        TS_ASSERT(!Computer.isSubTypeOf(BWPrinter))
        TS_ASSERT(!Computer.isSubTypeOf(ColorPrinter))
        TS_ASSERT(Computer.isSubTypeOf(Computer))
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    /// Testing classes and interfaces inheritance with supertypes methods
    GUM_ACTIVE_TEST(ComplexPrinters_3) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
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
        TS_ASSERT(PowerSupply.isSuperTypeOf(PowerSupply))
        TS_ASSERT(!PowerSupply.isSuperTypeOf(Room))
        TS_ASSERT(!PowerSupply.isSuperTypeOf(Equipment))
        TS_ASSERT(!PowerSupply.isSuperTypeOf(Printer))
        TS_ASSERT(!PowerSupply.isSuperTypeOf(BWPrinter))
        TS_ASSERT(!PowerSupply.isSuperTypeOf(ColorPrinter))
        TS_ASSERT(!PowerSupply.isSuperTypeOf(Computer))
        // Testing Room
        TS_ASSERT(!Room.isSuperTypeOf(PowerSupply))
        TS_ASSERT(Room.isSuperTypeOf(Room))
        TS_ASSERT(!Room.isSuperTypeOf(Equipment))
        TS_ASSERT(!Room.isSuperTypeOf(Printer))
        TS_ASSERT(!Room.isSuperTypeOf(BWPrinter))
        TS_ASSERT(!Room.isSuperTypeOf(ColorPrinter))
        TS_ASSERT(!Room.isSuperTypeOf(Computer))
        // Testing Equipment
        TS_ASSERT(!Equipment.isSuperTypeOf(PowerSupply))
        TS_ASSERT(!Equipment.isSuperTypeOf(Room))
        TS_ASSERT(Equipment.isSuperTypeOf(Equipment))
        TS_ASSERT(Equipment.isSuperTypeOf(Printer))
        TS_ASSERT(Equipment.isSuperTypeOf(BWPrinter))
        TS_ASSERT(Equipment.isSuperTypeOf(ColorPrinter))
        TS_ASSERT(Equipment.isSuperTypeOf(Computer))
        // Testing Printer
        TS_ASSERT(!Printer.isSuperTypeOf(PowerSupply))
        TS_ASSERT(!Printer.isSuperTypeOf(Room))
        TS_ASSERT(!Printer.isSuperTypeOf(Equipment))
        TS_ASSERT(Printer.isSuperTypeOf(Printer))
        TS_ASSERT(Printer.isSuperTypeOf(BWPrinter))
        TS_ASSERT(Printer.isSuperTypeOf(ColorPrinter))
        TS_ASSERT(!Printer.isSuperTypeOf(Computer))
        // Testing BWPrinter
        TS_ASSERT(!BWPrinter.isSuperTypeOf(PowerSupply))
        TS_ASSERT(!BWPrinter.isSuperTypeOf(Room))
        TS_ASSERT(!BWPrinter.isSuperTypeOf(Equipment))
        TS_ASSERT(!BWPrinter.isSuperTypeOf(Printer))
        TS_ASSERT(BWPrinter.isSuperTypeOf(BWPrinter))
        TS_ASSERT(!BWPrinter.isSuperTypeOf(ColorPrinter))
        TS_ASSERT(!BWPrinter.isSuperTypeOf(Computer))
        // Testing ColorPrinter
        TS_ASSERT(!ColorPrinter.isSuperTypeOf(PowerSupply))
        TS_ASSERT(!ColorPrinter.isSuperTypeOf(Room))
        TS_ASSERT(!ColorPrinter.isSuperTypeOf(Equipment))
        TS_ASSERT(!ColorPrinter.isSuperTypeOf(Printer))
        TS_ASSERT(!ColorPrinter.isSuperTypeOf(BWPrinter))
        TS_ASSERT(ColorPrinter.isSuperTypeOf(ColorPrinter))
        TS_ASSERT(!ColorPrinter.isSuperTypeOf(Computer))
        // Testing Computer
        TS_ASSERT(!Computer.isSuperTypeOf(PowerSupply))
        TS_ASSERT(!Computer.isSuperTypeOf(Room))
        TS_ASSERT(!Computer.isSuperTypeOf(Equipment))
        TS_ASSERT(!Computer.isSuperTypeOf(Printer))
        TS_ASSERT(!Computer.isSuperTypeOf(BWPrinter))
        TS_ASSERT(!Computer.isSuperTypeOf(ColorPrinter))
        TS_ASSERT(Computer.isSuperTypeOf(Computer))
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    // Testing class PowerSupply
    GUM_ACTIVE_TEST(ComplexPrinters_4) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        gum::prm::PRM< double >* prm = reader.prm();
        // Classes
        gum::prm::PRMClass< double >& PowerSupply = prm->getClass("fr.lip6.printers.PowerSupply");
        TS_ASSERT_EQUALS(PowerSupply.attributes().size(), (gum::Size)2)
        TS_ASSERT_EQUALS(PowerSupply.containerDag().sizeArcs(), (gum::Size)1)
        TS_GUM_ASSERT_THROWS_NOTHING(PowerSupply["(boolean)state"])
        TS_GUM_ASSERT_THROWS_NOTHING(PowerSupply["state"])
        TS_ASSERT(PowerSupply["state"].type().isSubTypeOf(PowerSupply["(boolean)state"].type()))
        gum::NodeId n1 = 0;
        gum::NodeId n2 = 0;
        gum::NodeId n3 = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(n1 = PowerSupply["(boolean)state"].id())
        TS_GUM_ASSERT_THROWS_NOTHING(n2 = PowerSupply["state"].id())
        TS_GUM_ASSERT_THROWS_NOTHING(n3 = PowerSupply["(fr.lip6.printers.t_state)state"].id())
        TS_ASSERT_EQUALS(n2, n3)
        TS_ASSERT(PowerSupply.containerDag().existsArc(n2, n1))
        TS_ASSERT_EQUALS(PowerSupply.referenceSlots().size(), (gum::Size)0)
        TS_ASSERT_EQUALS(PowerSupply.aggregates().size(), (gum::Size)0)
        TS_ASSERT_EQUALS(PowerSupply.slotChains().size(), (gum::Size)0)
        // Testing interfaces for the state attribute and its cast descendant
        TS_ASSERT(!PowerSupply.isOutputNode(PowerSupply["(boolean)state"]))
        TS_ASSERT(!PowerSupply.isInputNode(PowerSupply["(boolean)state"]))
        TS_ASSERT(PowerSupply.isInnerNode(PowerSupply["(boolean)state"]))
        TS_ASSERT(PowerSupply.isOutputNode(PowerSupply["state"]))
        TS_ASSERT(!PowerSupply.isInputNode(PowerSupply["state"]))
        TS_ASSERT(!PowerSupply.isInnerNode(PowerSupply["state"]))
        TS_ASSERT(PowerSupply.isOutputNode(PowerSupply["(fr.lip6.printers.t_state)state"]))
        TS_ASSERT(!PowerSupply.isInputNode(PowerSupply["(fr.lip6.printers.t_state)state"]))
        TS_ASSERT(!PowerSupply.isInnerNode(PowerSupply["(fr.lip6.printers.t_state)state"]))
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    // Testing class Room
    GUM_ACTIVE_TEST(ComplexPrinters_5) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        gum::prm::PRM< double >*      prm  = reader.prm();
        gum::prm::PRMClass< double >& Room = prm->getClass("fr.lip6.printers.Room");
        TS_ASSERT_EQUALS(Room.attributes().size(), (gum::Size)0)
        TS_ASSERT_EQUALS(Room.referenceSlots().size(), (gum::Size)1)
        TS_ASSERT_EQUALS(Room.aggregates().size(), (gum::Size)0)
        TS_ASSERT_EQUALS(Room.slotChains().size(), (gum::Size)0)
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    // Testing interface Equipment
    GUM_ACTIVE_TEST(ComplexPrinters_6) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        gum::prm::PRM< double >*          prm = reader.prm();
        gum::prm::PRMInterface< double >& Equipment
            = prm->getInterface("fr.lip6.printers.Equipment");
        TS_ASSERT_EQUALS(Equipment.referenceSlots().size(), (gum::Size)1)
        TS_ASSERT_EQUALS(Equipment.attributes().size(), (gum::Size)3)
        TS_GUM_ASSERT_THROWS_NOTHING(Equipment["equipState"])
        TS_ASSERT_EQUALS(Equipment["equipState"].type().name(), "fr.lip6.printers.t_degraded")
        TS_GUM_ASSERT_THROWS_NOTHING(Equipment["(fr.lip6.printers.t_state)equipState"])
        TS_ASSERT_EQUALS(Equipment["(fr.lip6.printers.t_state)equipState"].type().name(),
                         "fr.lip6.printers.t_state");
        TS_GUM_ASSERT_THROWS_NOTHING(Equipment["(boolean)equipState"])
        TS_ASSERT_EQUALS(Equipment["(boolean)equipState"].type().name(), "boolean")
        // Testing gum::NodeId, inputs and outputs
        gum::NodeId st      = Equipment["equipState"].id();
        gum::NodeId deg_st  = Equipment["(fr.lip6.printers.t_degraded)equipState"].id();
        gum::NodeId st_st   = Equipment["(fr.lip6.printers.t_state)equipState"].id();
        gum::NodeId bool_st = Equipment["(boolean)equipState"].id();
        TS_ASSERT_EQUALS(st, deg_st)
        TS_ASSERT_DIFFERS(st, st_st)
        TS_ASSERT_DIFFERS(st, bool_st)
        TS_ASSERT_DIFFERS(st_st, bool_st)
        // t_degraded equipState
        TS_ASSERT(!Equipment.isOutputNode(Equipment["equipState"]))
        TS_ASSERT(!Equipment.isInputNode(Equipment["equipState"]))
        TS_ASSERT(Equipment.isInnerNode(Equipment["equipState"]))
        // t_state equipState
        TS_ASSERT(!Equipment.isOutputNode(Equipment["(fr.lip6.printers.t_state)equipState"]))
        TS_ASSERT(!Equipment.isInputNode(Equipment["(fr.lip6.printers.t_state)equipState"]))
        TS_ASSERT(Equipment.isInnerNode(Equipment["(fr.lip6.printers.t_state)equipState"]))
        // boolean equipState
        TS_ASSERT(!Equipment.isOutputNode(Equipment["(boolean)equipState"]))
        TS_ASSERT(!Equipment.isInputNode(Equipment["(boolean)equipState"]))
        TS_ASSERT(Equipment.isInnerNode(Equipment["(boolean)equipState"]))
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    // Testing interface Printer
    GUM_ACTIVE_TEST(ComplexPrinters_7) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        gum::prm::PRM< double >* prm = reader.prm();
        TS_GUM_ASSERT_THROWS_NOTHING(prm->getInterface("fr.lip6.printers.Printer"))
        gum::prm::PRMInterface< double >& Printer = prm->getInterface("fr.lip6.printers.Printer");
        TS_ASSERT_EQUALS(Printer.referenceSlots().size(), (gum::Size)1)
        TS_ASSERT_EQUALS(Printer.attributes().size(), (gum::Size)5)
        TS_GUM_ASSERT_THROWS_NOTHING(Printer["room"])
        TS_GUM_ASSERT_THROWS_NOTHING(Printer["equipState"])
        TS_GUM_ASSERT_THROWS_NOTHING(Printer["hasPaper"])
        TS_GUM_ASSERT_THROWS_NOTHING(Printer["hasInk"])
        TS_ASSERT_EQUALS(Printer["equipState"].type().name(), "fr.lip6.printers.t_degraded")
        TS_ASSERT_EQUALS(Printer["hasPaper"].type().name(), "boolean")
        TS_ASSERT_EQUALS(Printer["hasInk"].type().name(), "boolean")
        // Testing gum::NodeId, inputs and outputs
        gum::NodeId st      = Printer["equipState"].id();
        gum::NodeId deg_st  = Printer["(fr.lip6.printers.t_degraded)equipState"].id();
        gum::NodeId st_st   = Printer["(fr.lip6.printers.t_state)equipState"].id();
        gum::NodeId bool_st = Printer["(boolean)equipState"].id();
        gum::NodeId hp      = Printer["hasPaper"].id();
        gum::NodeId bool_hp = Printer["(boolean)hasPaper"].id();
        gum::NodeId hi      = Printer["hasInk"].id();
        gum::NodeId bool_hi = Printer["(boolean)hasInk"].id();
        TS_ASSERT_EQUALS(st, deg_st)
        TS_ASSERT_DIFFERS(st, st_st)
        TS_ASSERT_DIFFERS(st, bool_st)
        TS_ASSERT_DIFFERS(st_st, bool_st)
        TS_ASSERT_EQUALS(hp, bool_hp)
        TS_ASSERT_EQUALS(hi, bool_hi)
        // t_degraded equipState
        TS_ASSERT(Printer.isOutputNode(Printer["(fr.lip6.printers.t_degraded)equipState"]))
        TS_ASSERT(!Printer.isInputNode(Printer["(fr.lip6.printers.t_degraded)equipState"]))
        TS_ASSERT(!Printer.isInnerNode(Printer["(fr.lip6.printers.t_degraded)equipState"]))
        // t_state equipState
        TS_ASSERT(Printer.isOutputNode(Printer["(fr.lip6.printers.t_state)equipState"]))
        TS_ASSERT(!Printer.isInputNode(Printer["(fr.lip6.printers.t_state)equipState"]))
        TS_ASSERT(!Printer.isInnerNode(Printer["(fr.lip6.printers.t_state)equipState"]))
        // boolean equipState
        TS_ASSERT(!Printer.isOutputNode(Printer["(boolean)equipState"]))
        TS_ASSERT(!Printer.isInputNode(Printer["(boolean)equipState"]))
        TS_ASSERT(Printer.isInnerNode(Printer["(boolean)equipState"]))
        // Boolean hasPaper
        TS_ASSERT(!Printer.isOutputNode(Printer["hasPaper"]))
        TS_ASSERT(!Printer.isInputNode(Printer["hasPaper"]))
        TS_ASSERT(Printer.isInnerNode(Printer["hasPaper"]))
        // Boolean hasInk
        TS_ASSERT(!Printer.isOutputNode(Printer["hasInk"]))
        TS_ASSERT(!Printer.isInputNode(Printer["hasInk"]))
        TS_ASSERT(Printer.isInnerNode(Printer["hasInk"]))
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    /// @TODO Make this test past for IO flags optimisation
    // Testing class BWPrinter
    // GUM_ACTIVE_TEST(ComplexPrinters_8) {
    //   try {
    //     gum::prm::o3prm::O3prmReader<double> reader;
    //     std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm"
    //     );
    //     std::string package = "fr.lip6.printers";
    //     TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) )
    //     gum::prm::PRM<double>* prm = reader.prm();
    //     TS_GUM_ASSERT_THROWS_NOTHING(
    //         prm->getClass( "fr.lip6.printers.BWPrinter" ) );
    //     gum::prm::Class<double>& BWPrinter =
    //         prm->getClass( "fr.lip6.printers.BWPrinter" );
    //     TS_ASSERT_EQUALS( BWPrinter.referenceSlots().size(), (gum::Size)1 )
    //     TS_ASSERT_EQUALS( BWPrinter.attributes().size(), (gum::Size)9 )
    //     TS_ASSERT_EQUALS( BWPrinter.slotChains().size(), (gum::Size)1 )
    //     TS_ASSERT_EQUALS( BWPrinter.aggregates().size(), (gum::Size)0 )
    //     TS_GUM_ASSERT_THROWS_NOTHING( BWPrinter["room"] )
    //     TS_GUM_ASSERT_THROWS_NOTHING( BWPrinter["(boolean)equipState"] )
    //     TS_GUM_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_state)equipState"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_degraded)equipState"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING( BWPrinter["equipState"] )
    //     TS_GUM_ASSERT_THROWS_NOTHING( BWPrinter["(boolean)hasPaper"] )
    //     TS_GUM_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_state)hasPaper"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_paper)hasPaper"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING( BWPrinter["hasPaper"] )
    //     TS_GUM_ASSERT_THROWS_NOTHING( BWPrinter["(boolean)hasInk"] )
    //     TS_GUM_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_state)hasInk"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_ink)hasInk"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING( BWPrinter["hasInk"] )
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
    //     TS_ASSERT_EQUALS( st, deg_st )
    //     TS_ASSERT_DIFFERS( st, st_st )
    //     TS_ASSERT_DIFFERS( st, bool_st )
    //     TS_ASSERT_DIFFERS( st_st, bool_st )
    //     TS_ASSERT_EQUALS( hp, p_hp )
    //     TS_ASSERT_DIFFERS( hp, st_hp )
    //     TS_ASSERT_DIFFERS( hp, bool_hp )
    //     TS_ASSERT_EQUALS( hi, i_hi )
    //     TS_ASSERT_DIFFERS( hi, st_hi )
    //     TS_ASSERT_DIFFERS( hi, bool_hi )
    //     // t_degraded equipState
    //     TS_ASSERT( BWPrinter.isOutputNode(
    //         BWPrinter["(fr.lip6.printers.t_degraded)equipState"] ) );
    //     TS_ASSERT( BWPrinter.isInputNode(
    //         BWPrinter["(fr.lip6.printers.t_degraded)equipState"] ) );
    //     TS_ASSERT( not BWPrinter.isInnerNode(
    //         BWPrinter["(fr.lip6.printers.t_degraded)equipState"] ) );
    //     // t_state equipState
    //     TS_ASSERT( BWPrinter.isOutputNode(
    //         BWPrinter["(fr.lip6.printers.t_state)equipState"] ) );
    //     TS_ASSERT( not BWPrinter.isInputNode(
    //         BWPrinter["(fr.lip6.printers.t_state)equipState"] ) );
    //     TS_ASSERT( not BWPrinter.isInnerNode(
    //         BWPrinter["(fr.lip6.printers.t_state)equipState"] ) );
    //     // boolean equipState
    //     TS_ASSERT(
    //         not BWPrinter.isOutputNode( BWPrinter["(boolean)equipState"] ) );
    //     TS_ASSERT(
    //         not BWPrinter.isInputNode( BWPrinter["(boolean)equipState"] ) );
    //     TS_ASSERT( BWPrinter.isInnerNode( BWPrinter["(boolean)equipState"] )
    //     );
    //     // t_paper hasPaper
    //     TS_ASSERT( not BWPrinter.isOutputNode(
    //         BWPrinter["(fr.lip6.printers.t_paper)hasPaper"] ) );
    //     TS_ASSERT( not BWPrinter.isInputNode(
    //         BWPrinter["(fr.lip6.printers.t_paper)hasPaper"] ) );
    //     TS_ASSERT( BWPrinter.isInnerNode(
    //         BWPrinter["(fr.lip6.printers.t_paper)hasPaper"] ) );
    //     // t_state hasInk
    //     TS_ASSERT( not BWPrinter.isOutputNode(
    //         BWPrinter["(fr.lip6.printers.t_ink)hasInk"] ) );
    //     TS_ASSERT( not BWPrinter.isInputNode(
    //         BWPrinter["(fr.lip6.printers.t_ink)hasInk"] ) );
    //     TS_ASSERT( BWPrinter.isInnerNode(
    //         BWPrinter["(fr.lip6.printers.t_ink)hasInk"] ) );
    //     // Boolean hasPaper
    //     TS_ASSERT(
    //         not BWPrinter.isOutputNode( BWPrinter["(boolean)hasPaper"] ) );
    //     TS_ASSERT(
    //         not BWPrinter.isInputNode( BWPrinter["(boolean)hasPaper"] ) );
    //     TS_ASSERT( BWPrinter.isInnerNode( BWPrinter["(boolean)hasPaper"] ) )
    //     // Boolean hasInk
    //     TS_ASSERT( not BWPrinter.isOutputNode( BWPrinter["(boolean)hasInk"] )
    //     );
    //     TS_ASSERT( not BWPrinter.isInputNode( BWPrinter["(boolean)hasInk"] )
    //     );
    //     TS_ASSERT( BWPrinter.isInnerNode( BWPrinter["hasInk"] ) )
    //     delete prm;
    //   } catch ( gum::Exception ) {
    //     TS_ASSERT( false )
    //   }
    // }

    /// @TODO Make this test pass for IO Flags optimisation
    // Testing class ColorPrinter
    // GUM_ACTIVE_TEST(ComplexPrinters_9) {
    //  try {
    //    gum::prm::o3prm::O3prmReader<double> reader;
    //    std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm"
    //    );
    //    std::string package = "fr.lip6.printers";
    //    TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) )
    //    gum::prm::PRM<double>* prm = reader.prm();
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        prm->getClass( "fr.lip6.printers.ColorPrinter" ) );
    //    gum::prm::Class<double>& ColorPrinter =
    //        prm->getClass( "fr.lip6.printers.ColorPrinter" );
    //    TS_ASSERT_EQUALS( ColorPrinter.referenceSlots().size(), (gum::Size)1
    //    );
    //    TS_ASSERT_EQUALS( ColorPrinter.attributes().size(), (gum::Size)19 )
    //    TS_ASSERT_EQUALS( ColorPrinter.aggregates().size(), (gum::Size)0 )
    //    TS_ASSERT_EQUALS( ColorPrinter.slotChains().size(), (gum::Size)1 )
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["room"] )
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)equipState"] )
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)equipState"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_degraded)equipState"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["equipState"] )
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)hasPaper"] )
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)hasPaper"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_paper)hasPaper"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["hasPaper"] )
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)hasInk"] )
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)black"] )
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)black"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_ink)black"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["black"] )
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)magenta"] )
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)magenta"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_ink)magenta"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["magenta"] )
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)yellow"] )
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)yellow"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_ink)yellow"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["yellow"] )
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)cyan"] )
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)cyan"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_ink)cyan"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["cyan"] )
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
    //    TS_ASSERT_EQUALS( st, deg_st )
    //    TS_ASSERT_DIFFERS( st, st_st )
    //    TS_ASSERT_DIFFERS( st, bool_st )
    //    TS_ASSERT_DIFFERS( st_st, bool_st )
    //    TS_ASSERT_EQUALS( hp, p_hp )
    //    TS_ASSERT_DIFFERS( hp, st_hp )
    //    TS_ASSERT_DIFFERS( hp, bool_hp )
    //    TS_ASSERT_EQUALS( hi, bool_hi )
    //    TS_ASSERT_EQUALS( bl, i_bl )
    //    TS_ASSERT_DIFFERS( bl, st_bl )
    //    TS_ASSERT_DIFFERS( bl, bool_bl )
    //    TS_ASSERT_DIFFERS( st_bl, bool_bl )
    //    TS_ASSERT_EQUALS( ma, i_ma )
    //    TS_ASSERT_DIFFERS( ma, st_ma )
    //    TS_ASSERT_DIFFERS( ma, bool_ma )
    //    TS_ASSERT_DIFFERS( st_ma, bool_ma )
    //    TS_ASSERT_EQUALS( ye, i_ye )
    //    TS_ASSERT_DIFFERS( ye, st_ye )
    //    TS_ASSERT_DIFFERS( ye, bool_ye )
    //    TS_ASSERT_DIFFERS( st_ye, bool_ye )
    //    TS_ASSERT_EQUALS( ce, i_ce )
    //    TS_ASSERT_DIFFERS( ce, st_ce )
    //    TS_ASSERT_DIFFERS( ce, bool_ce )
    //    TS_ASSERT_DIFFERS( st_ce, bool_ce )
    //    // t_degraded equipState
    //    TS_ASSERT( ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_degraded)equipState"] ) );
    //    TS_ASSERT( ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_degraded)equipState"] ) );
    //    TS_ASSERT( not ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_degraded)equipState"] ) );
    //    // t_state equipState
    //    TS_ASSERT( ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)equipState"] ) );
    //    TS_ASSERT( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)equipState"] ) );
    //    TS_ASSERT( not ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)equipState"] ) );
    //    // boolean equipState
    //    TS_ASSERT( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(boolean)equipState"] ) );
    //    TS_ASSERT( not ColorPrinter.isInputNode(
    //        ColorPrinter["(boolean)equipState"] ) );
    //    TS_ASSERT(
    //        ColorPrinter.isInnerNode( ColorPrinter["(boolean)equipState"] ) );
    //    // t_paper hasPaper
    //    TS_ASSERT( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_paper)hasPaper"] ) );
    //    TS_ASSERT( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_paper)hasPaper"] ) );
    //    TS_ASSERT( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_paper)hasPaper"] ) );
    //    // Boolean hasPaper
    //    TS_ASSERT( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(boolean)hasPaper"] ) );
    //    TS_ASSERT(
    //        not ColorPrinter.isInputNode( ColorPrinter["(boolean)hasPaper"] )
    //        );
    //    TS_ASSERT(
    //        ColorPrinter.isInnerNode( ColorPrinter["(boolean)hasPaper"] ) );
    //    // Boolean hasInk
    //    TS_ASSERT(
    //        not ColorPrinter.isOutputNode( ColorPrinter["(boolean)hasInk"] )
    //        );
    //    TS_ASSERT(
    //        not ColorPrinter.isInputNode( ColorPrinter["(boolean)hasInk"] ) );
    //    TS_ASSERT(
    //        ColorPrinter.isInnerNode( ColorPrinter["(boolean)hasInk"] ) );
    //    // t_ink black
    //    TS_ASSERT( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)black"] ) );
    //    TS_ASSERT( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)black"] ) );
    //    TS_ASSERT( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)black"] ) );
    //    // t_state black
    //    TS_ASSERT( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)black"] ) );
    //    TS_ASSERT( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)black"] ) );
    //    TS_ASSERT( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)black"] ) );
    //    // boolean black
    //    TS_ASSERT(
    //        not ColorPrinter.isOutputNode( ColorPrinter["(boolean)black"] ) );
    //    TS_ASSERT(
    //        not ColorPrinter.isInputNode( ColorPrinter["(boolean)black"] ) );
    //    TS_ASSERT( ColorPrinter.isInnerNode( ColorPrinter["(boolean)black"] )
    //    );
    //    // t_ink magenta
    //    TS_ASSERT( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)magenta"] ) );
    //    TS_ASSERT( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)magenta"] ) );
    //    TS_ASSERT( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)magenta"] ) );
    //    // t_state magenta
    //    TS_ASSERT( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)magenta"] ) );
    //    TS_ASSERT( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)magenta"] ) );
    //    TS_ASSERT( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)magenta"] ) );
    //    // boolean magenta
    //    TS_ASSERT(
    //        not ColorPrinter.isOutputNode( ColorPrinter["(boolean)magenta"] )
    //        );
    //    TS_ASSERT(
    //        not ColorPrinter.isInputNode( ColorPrinter["(boolean)magenta"] )
    //        );
    //    TS_ASSERT(
    //        ColorPrinter.isInnerNode( ColorPrinter["(boolean)magenta"] ) );
    //    // t_ink yellow
    //    TS_ASSERT( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)yellow"] ) );
    //    TS_ASSERT( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)yellow"] ) );
    //    TS_ASSERT( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)yellow"] ) );
    //    // t_state yellow
    //    TS_ASSERT( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)yellow"] ) );
    //    TS_ASSERT( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)yellow"] ) );
    //    TS_ASSERT( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)yellow"] ) );
    //    // boolean yellow
    //    TS_ASSERT(
    //        not ColorPrinter.isOutputNode( ColorPrinter["(boolean)yellow"] )
    //        );
    //    TS_ASSERT(
    //        not ColorPrinter.isInputNode( ColorPrinter["(boolean)yellow"] ) );
    //    TS_ASSERT(
    //        ColorPrinter.isInnerNode( ColorPrinter["(boolean)yellow"] ) );
    //    // t_ink cyan
    //    TS_ASSERT( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)cyan"] ) );
    //    TS_ASSERT( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)cyan"] ) );
    //    TS_ASSERT( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_ink)cyan"] ) );
    //    // t_state cyan
    //    TS_ASSERT( not ColorPrinter.isOutputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)cyan"] ) );
    //    TS_ASSERT( not ColorPrinter.isInputNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)cyan"] ) );
    //    TS_ASSERT( ColorPrinter.isInnerNode(
    //        ColorPrinter["(fr.lip6.printers.t_state)cyan"] ) );
    //    // boolean cyan
    //    TS_ASSERT(
    //        not ColorPrinter.isOutputNode( ColorPrinter["(boolean)cyan"] ) );
    //    TS_ASSERT(
    //        not ColorPrinter.isInputNode( ColorPrinter["(boolean)cyan"] ) );
    //    TS_ASSERT( ColorPrinter.isInnerNode( ColorPrinter["(boolean)cyan"] )
    //    );
    //    delete prm;
    //  } catch ( gum::Exception ) {
    //    TS_ASSERT( false )
    //  }
    //}

    // Testing class Computer
    GUM_ACTIVE_TEST(ComplexPrinters_10) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        gum::prm::PRM< double >*      prm      = reader.prm();
        gum::prm::PRMClass< double >& Computer = prm->getClass("fr.lip6.printers.Computer");
        TS_ASSERT_EQUALS(Computer.attributes().size(), (gum::Size)4)
        TS_ASSERT_EQUALS(Computer.referenceSlots().size(), (gum::Size)2)
        TS_ASSERT_EQUALS(Computer.aggregates().size(), (gum::Size)4)
        TS_ASSERT_EQUALS(Computer.slotChains().size(), (gum::Size)2)
        TS_GUM_ASSERT_THROWS_NOTHING(Computer["(boolean)functional_printer"])
        TS_GUM_ASSERT_THROWS_NOTHING(Computer["functional_printer"])
        TS_GUM_ASSERT_THROWS_NOTHING(Computer["(boolean)degraded_printer"])
        TS_GUM_ASSERT_THROWS_NOTHING(Computer["degraded_printer"])
        TS_GUM_ASSERT_THROWS_NOTHING(Computer["(boolean)working_printer"])
        TS_GUM_ASSERT_THROWS_NOTHING(Computer["working_printer"])
        TS_GUM_ASSERT_THROWS_NOTHING(Computer["(boolean)equipState"])
        TS_GUM_ASSERT_THROWS_NOTHING(Computer["(fr.lip6.printers.t_state)equipState"])
        TS_GUM_ASSERT_THROWS_NOTHING(Computer["(fr.lip6.printers.t_degraded)equipState"])
        TS_GUM_ASSERT_THROWS_NOTHING(Computer["equipState"])
        TS_GUM_ASSERT_THROWS_NOTHING(Computer["(boolean)can_print"])
        TS_GUM_ASSERT_THROWS_NOTHING(Computer["can_print"])
        TS_GUM_ASSERT_THROWS_NOTHING(Computer["printers.equipState"])
        TS_GUM_ASSERT_THROWS_NOTHING(Computer["room.power.state"])
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    // Testing class Computer
    GUM_ACTIVE_TEST(ComplexPrinters_11) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        gum::prm::PRM< double >*      prm          = reader.prm();
        gum::prm::PRMClass< double >& SafeComputer = prm->getClass("fr.lip6.printers.SafeComputer");
        TS_ASSERT_EQUALS(SafeComputer.attributes().size(), (gum::Size)4)
        TS_ASSERT_EQUALS(SafeComputer.referenceSlots().size(), (gum::Size)2)
        TS_ASSERT_EQUALS(SafeComputer.aggregates().size(), (gum::Size)3)
        TS_ASSERT_EQUALS(SafeComputer.slotChains().size(), (gum::Size)3)
        TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["(boolean)functional_printer"])
        TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["functional_printer"])
        TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["(boolean)degraded_printer"])
        TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["degraded_printer"])
        TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["(boolean)working_printer"])
        TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["working_printer"])
        TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["(boolean)equipState"])
        TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["(fr.lip6.printers.t_state)equipState"])
        TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["(fr.lip6.printers.t_degraded)equipState"])
        TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["equipState"])
        TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["(boolean)can_print"])
        TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["can_print"])
        TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["printers.(fr.lip6.printers.t_state)equipState"])
        TS_GUM_ASSERT_THROWS_NOTHING(
            SafeComputer["printers.(fr.lip6.printers.t_degraded)equipState"]);
        TS_GUM_ASSERT_THROWS_NOTHING(SafeComputer["room.power.(fr.lip6.printers.t_state)state"])
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    // Testing class Computer
    GUM_ACTIVE_TEST(ComplexPrinters_12) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        gum::prm::PRM< double >*      prm          = reader.prm();
        gum::prm::PRMClass< double >& SafeComputer = prm->getClass("fr.lip6.printers.SafeComputer");
        TS_ASSERT_EQUALS(SafeComputer.attributes().size(), (gum::Size)4)
        TS_ASSERT(SafeComputer.exists("can_print"))
        const auto& cpf = SafeComputer.get("can_print").cpf();
        auto        set = gum::VariableSet();
        set.insert(&(SafeComputer.get("can_print").type().variable()));
        set.insert(&(SafeComputer.get("working_printer").type().variable()));
        set.insert(&(SafeComputer.get("equipState").type().variable()));
        for (auto var: cpf.variablesSequence()) {
          if (set.contains(var)) { set.erase(var); }
        }
        TS_ASSERT_EQUALS(set.size(), (gum::Size)0)
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    // Testing class Computer
    GUM_ACTIVE_TEST(ComplexPrinters_13) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        gum::prm::PRM< double >*      prm          = reader.prm();
        gum::prm::PRMClass< double >& SafeComputer = prm->getClass("fr.lip6.printers.Computer");
        TS_ASSERT_EQUALS(SafeComputer.attributes().size(), (gum::Size)4)
        TS_ASSERT(SafeComputer.exists("equipState"))
        const auto& cpf = SafeComputer.get("equipState").cpf();
        auto        set = gum::VariableSet();
        set.insert(&(SafeComputer.get("equipState").type().variable()));
        set.insert(&(SafeComputer.get("room.power.state").type().variable()));
        for (auto var: cpf.variablesSequence()) {
          if (set.contains(var)) { set.erase(var); }
        }
        TS_ASSERT_EQUALS(set.size(), (gum::Size)0)
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    // Testing class Computer
    GUM_ACTIVE_TEST(ComplexPrinters_14) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        gum::prm::PRM< double >*      prm          = reader.prm();
        gum::prm::PRMClass< double >& SafeComputer = prm->getClass("fr.lip6.printers.SafeComputer");
        TS_ASSERT_EQUALS(SafeComputer.attributes().size(), (gum::Size)4)
        TS_ASSERT(SafeComputer.exists("equipState"))
        const auto& cpf = SafeComputer.get("equipState").cpf();
        auto        set = gum::VariableSet();
        set.insert(&(SafeComputer.get("equipState").type().variable()));
        set.insert(
            &(SafeComputer.get("room.power.(fr.lip6.printers.t_state)state").type().variable()));
        for (auto var: cpf.variablesSequence()) {
          if (set.contains(var)) { set.erase(var); }
        }
        TS_ASSERT_EQUALS(set.size(), (gum::Size)0)
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(PrintersSystems1) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        TS_GUM_ASSERT_THROWS_NOTHING(
            reader.readFile(GET_RESSOURCES_PATH("o3prm/printers_systems.o3prm")));
        TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
        TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
        reader.showElegantErrorsAndWarnings();
        gum::prm::PRM< double >* prm = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(prm = reader.prm())

        if (prm) { delete prm; }
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(ComplexPrintersSystems1) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        TS_GUM_ASSERT_THROWS_NOTHING(
            reader.readFile(GET_RESSOURCES_PATH("o3prm/complexprinters_system.o3prm")));
        TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
        TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)

        if (reader.errors()) { reader.showElegantErrorsAndWarnings(); }

        gum::prm::PRM< double >* prm = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING(prm = reader.prm())
        gum::prm::PRMSystem< double >* sys = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING(sys = &(prm->getSystem("aSys")))
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
        TS_ASSERT_EQUALS(sys->get(Power).size(), (gum::Size)1)
        TS_ASSERT_EQUALS(sys->get(Room).size(), (gum::Size)1)
        TS_ASSERT_EQUALS(sys->get(BWPrinter).size(), (gum::Size)10)
        TS_ASSERT_EQUALS(sys->get(ColorPrinter).size(), (gum::Size)2)
        TS_ASSERT_EQUALS(sys->get(Computer).size(), (gum::Size)2)
        int count = 0;

        for (auto iter = sys->begin(); iter != sys->end(); ++iter) {
          ++count;
        }

        TS_ASSERT_EQUALS(count, 18)

        if (prm) { delete prm; }
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(CPF) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(GET_RESSOURCES_PATH("o3prm/inference.o3prm")))
        TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
        TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
        gum::prm::PRM< double >* prm = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING(prm = reader.prm())
        gum::prm::PRMSystem< double >* sys = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING(sys = &(prm->getSystem("aSys")))

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

              TS_ASSERT_DELTA(f, 1.0, TS_GUM_SMALL_ERROR)
              i.unsetOverflow();
            }
          }
        }

        if (prm) { delete prm; }
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(Imports) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        reader.addClassPath(GET_RESSOURCES_PATH("o3prmr"));
        TS_GUM_ASSERT_THROWS_NOTHING(
            reader.readFile(GET_RESSOURCES_PATH("o3prmr/systems/MySystem.o3prm"),
                            "systems.MySystem"));
        TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
        TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
        reader.showElegantErrorsAndWarnings();

        gum::prm::PRM< double >* prm = nullptr;

        TS_GUM_ASSERT_THROWS_NOTHING(prm = reader.prm())
        gum::Size class_count = prm->classes().size();
        class_count += prm->interfaces().size();
        TS_ASSERT_EQUALS(class_count,
                         (gum::Size)11);   // Don't forget param subclasses !

        TS_ASSERT_EQUALS(prm->systems().size(), (gum::Size)1)
        gum::prm::PRMSystem< double >* sys = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING(sys = &(prm->getSystem("systems.MySystem.MySystem")))
        TS_ASSERT_EQUALS(sys->size(), (gum::Size)16)

        // Checking that all class DAG are generated
        for (auto c: prm->classes()) {
          for (auto node = c->containerDag().begin(); node != c->containerDag().end(); ++node) {
            TS_ASSERT(c->exists(*node))
            TS_ASSERT_THROWS_NOTHING(c->get(*node))
            for (auto prnt: c->containerDag().parents(*node)) {
              TS_ASSERT(c->exists(prnt))
              TS_ASSERT_THROWS_NOTHING(c->get(prnt))
            }

            for (auto child: c->containerDag().children(*node)) {
              TS_ASSERT(c->exists(child))
              TS_ASSERT_THROWS_NOTHING(c->get(child))
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

              TS_ASSERT_DELTA(sum / card, 1.0, 1e-6)
            } catch (gum::Exception& e) { TS_FAIL(e.errorContent()); }
          }
        }

        if (prm) { delete prm; }
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(ParamClass) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        gum::prm::PRM< double >*      prm        = reader.prm();
        gum::prm::PRMClass< double >& ParamClass = prm->getClass("fr.lip6.printers.ParamClass");
        TS_ASSERT_EQUALS(ParamClass.attributes().size(), (gum::Size)9)
        TS_ASSERT_EQUALS(ParamClass.referenceSlots().size(), (gum::Size)1)
        TS_ASSERT_EQUALS(ParamClass.slotChains().size(), (gum::Size)1)
        TS_GUM_ASSERT_THROWS_NOTHING(ParamClass["lambda"])
        TS_GUM_ASSERT_THROWS_NOTHING(ParamClass["t"])
        TS_GUM_ASSERT_THROWS_NOTHING(ParamClass["room"])
        TS_GUM_ASSERT_THROWS_NOTHING(ParamClass["hasInk"])
        TS_GUM_ASSERT_THROWS_NOTHING(ParamClass["hasPaper"])
        TS_GUM_ASSERT_THROWS_NOTHING(ParamClass["equipState"])

        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(SystemParamClass) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters_system.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        gum::prm::PRM< double >* prm = reader.prm();

        TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("fr.lip6.printers.ParamClass"))
        TS_GUM_ASSERT_THROWS_NOTHING(prm->getClass("fr.lip6.printers.ParamClass<lambda=0.4,t=4>"))
        TS_GUM_ASSERT_THROWS_NOTHING(
            prm->getClass("fr.lip6.printers.ParamClass<lambda=0.001,t=4>"));

        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(ParamClassHasInk) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))

        auto  prm        = reader.prm();
        auto& ParamClass = prm->getClass("fr.lip6.printers.ParamClass");

        const auto& hasInk = static_cast< gum::prm::PRMAttribute< double >& >(ParamClass["hasInk"]);

        gum::Instantiation    inst(hasInk.cpf());
        std::vector< double > values;
        for (inst.setFirst(); !inst.end(); inst.inc()) {
          values.push_back(hasInk.cpf()[inst]);
        }

        TS_ASSERT_EQUALS(values.size(), (size_t)2)
        TS_ASSERT_DELTA(values[0], 0.4, 1e-6)
        TS_ASSERT_DELTA(values[1], 0.6, 1e-6)

        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(ParamClassHasPaper) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))

        auto  prm        = reader.prm();
        auto& ParamClass = prm->getClass("fr.lip6.printers.ParamClass");

        const auto& hasPaper
            = static_cast< gum::prm::PRMAttribute< double >& >(ParamClass["hasPaper"]);

        gum::Instantiation    inst(hasPaper.cpf());
        std::vector< double > values;
        for (inst.setFirst(); !inst.end(); inst.inc()) {
          values.push_back(hasPaper.cpf()[inst]);
        }

        TS_ASSERT_EQUALS(values.size(), (size_t)3)
        TS_ASSERT_DELTA(values[0], std::exp(-0.4 * 4), 1e-6)
        TS_ASSERT_DELTA(values[1], 1 - std::exp(-0.4 * 4), 1e-6)
        TS_ASSERT_DELTA(values[2], 0.0, 1e-6)

        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(InstanceParamClassHasInk) {
      try {
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/complexprinters_system.o3prm");
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING(reader.readFile(file, package))

        auto  prm      = reader.prm();
        auto& sys      = prm->getSystem("fr.lip6.printers.aSys");
        auto& paramBis = sys.get("paramBis");

        const auto& hasInk = paramBis.get("hasInk");

        gum::Instantiation    inst(hasInk.cpf());
        std::vector< double > values;
        for (inst.setFirst(); !inst.end(); inst.inc()) {
          values.push_back(hasInk.cpf()[inst]);
        }

        TS_ASSERT_EQUALS(values.size(), (size_t)2)
        TS_ASSERT_DELTA(values[0], 0.001, 1e-6)
        TS_ASSERT_DELTA(values[1], 0.999, 1e-6)

        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(AsiaBN) {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string                            file    = GET_RESSOURCES_PATH("o3prm/Asia.o3prm");
        std::string                            package = "";
        reader.readFile(file, package);
        auto                                prm  = reader.prm();
        gum::prm::PRMClass< double > const* asia = nullptr;
        // Act
        TS_ASSERT_THROWS_NOTHING(asia = &(prm->getClass("Asia")))
        // Assert
        TS_ASSERT_EQUALS(asia->attributes().size(), (gum::Size)8)
        TS_ASSERT_EQUALS(asia->containerDag().sizeArcs(), (gum::Size)8)
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(AsiaBNGrd) {
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
        TS_ASSERT_THROWS_NOTHING(sys.groundedBN(factory))
        // Assert
        TS_ASSERT_EQUALS(bn->size(), (gum::Size)8)
        TS_ASSERT_EQUALS(bn->sizeArcs(), (gum::Size)8)
        delete prm;
        delete bn;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(VisitAsiaTwoClassesBN) {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/AsiaClassAndSystemWithTwoClasses.o3prm");
        std::string package = "";
        reader.readFile(file, package);
        auto                                prm  = reader.prm();
        gum::prm::PRMClass< double > const* asia = nullptr;
        // Act
        TS_ASSERT_THROWS_NOTHING(asia = &(prm->getClass("VisitAsia")))
        // Assert
        TS_ASSERT_EQUALS(asia->attributes().size(), (gum::Size)2)
        TS_ASSERT_EQUALS(asia->containerDag().sizeArcs(), (gum::Size)1)
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(AsiaTwoClassesBN) {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/AsiaClassAndSystemWithTwoClasses.o3prm");
        std::string package = "";
        reader.readFile(file, package);
        auto                                prm  = reader.prm();
        gum::prm::PRMClass< double > const* asia = nullptr;
        // Act
        TS_ASSERT_THROWS_NOTHING(asia = &(prm->getClass("Asia")))
        // Assert
        TS_ASSERT_EQUALS(asia->attributes().size(), (gum::Size)6)
        TS_ASSERT_EQUALS(asia->referenceSlots().size(), (gum::Size)1)
        TS_ASSERT_EQUALS(asia->containerDag().sizeArcs(), (gum::Size)7)
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(AsiaTwoClassBNGrd) {
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
        TS_ASSERT_THROWS_NOTHING(sys.groundedBN(factory))
        // Assert
        TS_ASSERT_EQUALS(bn->size(), (gum::Size)8)
        TS_ASSERT_EQUALS(bn->sizeArcs(), (gum::Size)8)
        delete prm;
        delete bn;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(FileNotFound) {
      // Arrange
      gum::prm::o3prm::O3prmReader< double > reader;
      std::string                            file    = "ThisFileDoesNotExist.o3prm";
      std::string                            package = "";
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
      TS_ASSERT_DIFFERS(reader.errors(), (gum::Size)0)
      if (reader.prm()) { delete reader.prm(); }
    }

    GUM_ACTIVE_TEST(FileNotFoundInResDir) {
      // Arrange
      gum::prm::o3prm::O3prmReader< double > reader;
      std::string file    = GET_RESSOURCES_PATH("o3prm/ThisFileDoesNotExist.o3prm");
      std::string package = "";
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
      TS_ASSERT_DIFFERS(reader.errors(), (gum::Size)0)
      if (reader.prm()) { delete reader.prm(); }
    }

    GUM_ACTIVE_TEST(AsiaWithErrors) {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prm/AsiaWithError.o3prm");
        std::string package = "";
        // Act
        TS_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        // Assert
        TS_ASSERT_EQUALS(reader.errors(), (gum::Size)1)
        TS_ASSERT_DIFFERS(reader.prm(), nullptr)
        delete reader.prm();
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(AggOrAnd) {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string                            file = GET_RESSOURCES_PATH("o3prm/aggregates.o3prm");
        std::string                            package = "";
        // Act
        TS_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        // Assert
        TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
        if (reader.errors()) { reader.showElegantErrorsAndWarnings(); }
        TS_ASSERT_DIFFERS(reader.prm(), nullptr)
        delete reader.prm();
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(Actors) {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string                            file    = GET_RESSOURCES_PATH("o3prm/acteurs.o3prm");
        std::string                            package = "";
        // Act
        TS_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        // Assert
        TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
        if (reader.errors()) { reader.showElegantErrorsAndWarnings(); }
        TS_ASSERT_DIFFERS(reader.prm(), nullptr)
        delete reader.prm();
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(Students) {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prmr/University/fr/base.o3prm");
        std::string package = "fr.base";
        // Act
        TS_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        // Assert
        TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
        if (reader.errors()) { reader.showElegantErrorsAndWarnings(); }
        TS_ASSERT_DIFFERS(reader.prm(), nullptr)

        auto sys = &(reader.prm()->getSystem("fr.base.Work"));
        for (auto iter: *sys) {
          auto inst = iter.second;
          for (auto node: inst->type().containerDag()) {
            // TS_ASSERT( inst->exists( node ) )
            if ((!inst->exists(node)) && inst->type().exists(node)) {
              auto elt = &(inst->type().get(node));
              if (gum::prm::PRMClassElement< double >::isAttribute(*elt)
                  || gum::prm::PRMClassElement< double >::isAggregate(*elt)) {
                TS_ASSERT(false)
              }
            }
          }
        }

        delete reader.prm();
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(ReferenceID) {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prmr/University/fr/base.o3prm");
        std::string package = "";
        // Act
        TS_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        // Assert
        TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
        if (reader.errors()) { reader.showElegantErrorsAndWarnings(); }

        TS_ASSERT_DIFFERS(reader.prm(), nullptr)

        auto prm = reader.prm();
        for (auto c: prm->classes()) {
          try {
            for (auto i: c->implements()) {
              for (auto r: i->referenceSlots()) {
                TS_ASSERT_EQUALS(r->name(), c->get(r->id()).name())
                TS_ASSERT_EQUALS(r->safeName(), c->get(r->id()).safeName())
                TS_ASSERT_EQUALS(r->id(), c->get(r->name()).id())
              }
              for (auto r: c->referenceSlots()) {
                if (i->exists(r->name())) {
                  TS_ASSERT_EQUALS(r->name(), i->get(r->id()).name())
                  TS_ASSERT_EQUALS(r->safeName(), i->get(r->id()).safeName())
                  TS_ASSERT_EQUALS(r->id(), i->get(r->name()).id())
                }
              }
            }
          } catch (gum::NotFound&) {
            // No PRMInterface
          }
        }

        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(AttributeID) {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string file    = GET_RESSOURCES_PATH("o3prmr/University/fr/base.o3prm");
        std::string package = "";
        // Act
        TS_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        // Assert
        TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
        if (reader.errors()) { reader.showElegantErrorsAndWarnings(); }

        TS_ASSERT_DIFFERS(reader.prm(), nullptr)

        auto prm = reader.prm();
        for (auto c: prm->classes()) {
          try {
            for (auto i: c->implements()) {
              for (auto a: i->attributes()) {
                TS_ASSERT(c->exists(a->name()))
                TS_ASSERT(c->exists(a->safeName()))
                TS_ASSERT(c->exists(a->id()))
                TS_ASSERT_EQUALS(a->safeName(), c->get(a->id()).safeName())
                TS_ASSERT_EQUALS(a->id(), c->get(a->id()).id())
              }
            }
          } catch (gum::NotFound&) {
            // No interface
          }
        }

        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_ACTIVE_TEST(Arrays) {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader< double > reader;
        std::string                            file    = GET_RESSOURCES_PATH("o3prm/arrays.o3prm");
        std::string                            package = "";
        // Act
        TS_ASSERT_THROWS_NOTHING(reader.readFile(file, package))
        // Assert
        TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
        if (reader.errors()) { reader.showElegantErrorsAndWarnings(); }

        TS_ASSERT_DIFFERS(reader.prm(), nullptr)

        if (reader.prm() != nullptr) {
          delete reader.prm();
        } else {
          TS_ASSERT(false)
        }

      } catch (gum::Exception&) { TS_ASSERT(false) }
    }
  };

}   // namespace gum_tests
