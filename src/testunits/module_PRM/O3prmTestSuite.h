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
#include <cmath>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/PRM/o3prm/O3prmReader.h>

#include <agrum/PRM/elements/formAttribute.h>

namespace gum_tests {

  class O3prmTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {
      // std::cerr << std::endl;
    }

    void tearDown() {}

    void testTypes() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        TS_GUM_ASSERT_THROWS_NOTHING(
            reader.readFile( GET_RESSOURCES_PATH( "o3prm/types.o3prm" ) ) );
        TS_ASSERT_EQUALS( reader.warnings(), (gum::Size)0 );
        TS_ASSERT_EQUALS( reader.errors(), (gum::Size)0 );
        reader.showElegantErrorsAndWarnings( std::cout );
        gum::prm::PRM<double>* prm = reader.prm();
        TS_ASSERT_EQUALS( prm->type( "t_state" ).variable().domainSize(),
                          (gum::Size)2 );
        TS_ASSERT_EQUALS( prm->type( "t_ink" ).variable().domainSize(),
                          (gum::Size)2 );
        TS_ASSERT_EQUALS( prm->type( "t_degraded" ).variable().domainSize(),
                          (gum::Size)3 );
        TS_ASSERT_EQUALS( prm->type( "t_bw_p" ).variable().domainSize(),
                          (gum::Size)4 );
        TS_ASSERT_EQUALS( prm->type( "t_color_p" ).variable().domainSize(),
                          (gum::Size)5 );
        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    void testSubTypes() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        TS_GUM_ASSERT_THROWS_NOTHING(
            reader.readFile( GET_RESSOURCES_PATH( "o3prm/types.o3prm" ) ) );
        gum::prm::PRM<double>* prm = reader.prm();
        TS_ASSERT( prm->type( "t_state" ).isSubType() );
        TS_ASSERT(
            prm->type( "t_state" ).isSubTypeOf( prm->type( "boolean" ) ) );
        TS_ASSERT( prm->type( "t_ink" ).isSubType() );
        TS_ASSERT( prm->type( "t_ink" ).isSubTypeOf( prm->type( "boolean" ) ) );
        TS_ASSERT( prm->type( "t_ink" ).isSubTypeOf( prm->type( "t_state" ) ) );
        TS_ASSERT( prm->type( "t_paper" ).isSubType() );
        TS_ASSERT(
            prm->type( "t_paper" ).isSubTypeOf( prm->type( "boolean" ) ) );
        TS_ASSERT(
            prm->type( "t_paper" ).isSubTypeOf( prm->type( "t_state" ) ) );
        TS_ASSERT( prm->type( "t_degraded" ).isSubType() );
        TS_ASSERT(
            prm->type( "t_degraded" ).isSubTypeOf( prm->type( "boolean" ) ) );
        TS_ASSERT(
            prm->type( "t_degraded" ).isSubTypeOf( prm->type( "t_state" ) ) );
        TS_ASSERT( prm->type( "t_bw_p" ).isSubType() );
        TS_ASSERT(
            prm->type( "t_bw_p" ).isSubTypeOf( prm->type( "boolean" ) ) );
        TS_ASSERT(
            prm->type( "t_bw_p" ).isSubTypeOf( prm->type( "t_state" ) ) );
        TS_ASSERT(
            prm->type( "t_bw_p" ).isSubTypeOf( prm->type( "t_degraded" ) ) );
        TS_ASSERT( prm->type( "t_color_p" ).isSubType() );
        TS_ASSERT(
            prm->type( "t_color_p" ).isSubTypeOf( prm->type( "boolean" ) ) );
        TS_ASSERT(
            prm->type( "t_color_p" ).isSubTypeOf( prm->type( "t_state" ) ) );
        TS_ASSERT(
            prm->type( "t_color_p" ).isSubTypeOf( prm->type( "t_degraded" ) ) );
        TS_ASSERT_EQUALS( prm->types().size(), (gum::Size)7 );
        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    void testClasses() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        TS_GUM_ASSERT_THROWS_NOTHING(
            reader.readFile( GET_RESSOURCES_PATH( "o3prm/printers.o3prm" ) ) );
        TS_ASSERT_EQUALS( reader.warnings(), (gum::Size)0 );
        TS_ASSERT_EQUALS( reader.errors(), (gum::Size)0 );
        reader.showElegantErrorsAndWarnings();
        gum::prm::PRM<double>* prm = reader.prm();
        TS_ASSERT_EQUALS( prm->classes().size(), (gum::Size)5 );
        TS_GUM_ASSERT_THROWS_NOTHING( prm->getClass( "PowerSupply" ) );
        TS_GUM_ASSERT_THROWS_NOTHING( prm->getClass( "Room" ) );
        TS_GUM_ASSERT_THROWS_NOTHING( prm->getClass( "Equipment" ) );
        TS_GUM_ASSERT_THROWS_NOTHING( prm->getClass( "Printer" ) );
        TS_GUM_ASSERT_THROWS_NOTHING( prm->getClass( "Computer" ) );
        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    void testPrintersPowerSupply() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        TS_GUM_ASSERT_THROWS_NOTHING(
            reader.readFile( GET_RESSOURCES_PATH( "o3prm/printers.o3prm" ) ) );
        gum::prm::PRM<double>* prm = reader.prm();
        TS_GUM_ASSERT_THROWS_NOTHING( prm->getClass( "PowerSupply" ) );
        gum::prm::Class<double>& c = prm->getClass( "PowerSupply" );
        TS_ASSERT_EQUALS( c.attributes().size(), (gum::Size)1 );
        TS_ASSERT_EQUALS( c.referenceSlots().size(), (gum::Size)0 );
        TS_ASSERT_EQUALS( c.aggregates().size(), (gum::Size)0 );
        TS_ASSERT_EQUALS( c.slotChains().size(), (gum::Size)0 );
        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    void testPrintersRoom() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        TS_GUM_ASSERT_THROWS_NOTHING(
            reader.readFile( GET_RESSOURCES_PATH( "o3prm/printers.o3prm" ) ) );
        gum::prm::PRM<double>* prm = reader.prm();
        TS_GUM_ASSERT_THROWS_NOTHING( prm->getClass( "Room" ) );
        gum::prm::Class<double>& c = prm->getClass( "Room" );
        TS_ASSERT_EQUALS( c.attributes().size(), (gum::Size)0 );
        TS_ASSERT_EQUALS( c.referenceSlots().size(), (gum::Size)1 );
        TS_ASSERT_EQUALS( c.aggregates().size(), (gum::Size)0 );
        TS_ASSERT_EQUALS( c.slotChains().size(), (gum::Size)0 );
        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    void testPrintersEquipment() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        TS_GUM_ASSERT_THROWS_NOTHING(
            reader.readFile( GET_RESSOURCES_PATH( "o3prm/printers.o3prm" ) ) );
        gum::prm::PRM<double>* prm = reader.prm();
        TS_GUM_ASSERT_THROWS_NOTHING( prm->getClass( "Equipment" ) );
        gum::prm::Class<double>& c = prm->getClass( "Equipment" );
        TS_ASSERT_EQUALS( c.attributes().size(), (gum::Size)1 );
        TS_ASSERT_EQUALS( c.referenceSlots().size(), (gum::Size)1 );
        TS_ASSERT_EQUALS( c.aggregates().size(), (gum::Size)0 );
        TS_ASSERT_EQUALS( c.slotChains().size(), (gum::Size)1 );
        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    void testPrintersPrinter() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        TS_GUM_ASSERT_THROWS_NOTHING(
            reader.readFile( GET_RESSOURCES_PATH( "o3prm/printers.o3prm" ) ) );
        gum::prm::PRM<double>* prm = reader.prm();
        TS_GUM_ASSERT_THROWS_NOTHING( prm->getClass( "Printer" ) );
        gum::prm::Class<double>& c = prm->getClass( "Printer" );
        TS_ASSERT_EQUALS( c.attributes().size(), (gum::Size)3 );
        TS_ASSERT_EQUALS( c.referenceSlots().size(), (gum::Size)1 );
        TS_ASSERT_EQUALS( c.aggregates().size(), (gum::Size)0 );
        TS_ASSERT_EQUALS( c.slotChains().size(), (gum::Size)1 );
        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    void testPrintersComputer() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        TS_GUM_ASSERT_THROWS_NOTHING(
            reader.readFile( GET_RESSOURCES_PATH( "o3prm/printers.o3prm" ) ) );
        gum::prm::PRM<double>* prm = reader.prm();
        TS_GUM_ASSERT_THROWS_NOTHING( prm->getClass( "Computer" ) );
        gum::prm::Class<double>& c = prm->getClass( "Computer" );
        TS_ASSERT_EQUALS( c.attributes().size(), (gum::Size)2 );
        TS_ASSERT_EQUALS( c.referenceSlots().size(), (gum::Size)2 );
        TS_ASSERT_EQUALS( c.aggregates().size(), (gum::Size)1 );
        TS_ASSERT_EQUALS( c.slotChains().size(), (gum::Size)2 );
        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    /// Testing classes and interfaces
    void testComplexPrinters_1() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm" );
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        TS_ASSERT_EQUALS( reader.warnings(), (gum::Size)0 );
        TS_ASSERT_EQUALS( reader.errors(), (gum::Size)0 );
        reader.showElegantErrorsAndWarnings();
        gum::prm::PRM<double>* prm = 0;
        TS_GUM_ASSERT_THROWS_NOTHING( prm = reader.prm() );
        if ( prm ) {
          TS_ASSERT_EQUALS( prm->classes().size(), (gum::Size)7 );
          TS_GUM_ASSERT_THROWS_NOTHING(
              prm->getClass( "fr.lip6.printers.PowerSupply" ) );
          TS_GUM_ASSERT_THROWS_NOTHING(
              prm->getClass( "fr.lip6.printers.Room" ) );
          TS_GUM_ASSERT_THROWS_NOTHING(
              prm->getClass( "fr.lip6.printers.BWPrinter" ) );
          TS_GUM_ASSERT_THROWS_NOTHING(
              prm->getClass( "fr.lip6.printers.ColorPrinter" ) );
          TS_GUM_ASSERT_THROWS_NOTHING(
              prm->getClass( "fr.lip6.printers.Computer" ) );
          TS_ASSERT_EQUALS( prm->interfaces().size(), (gum::Size)2 );
          TS_GUM_ASSERT_THROWS_NOTHING(
              prm->interface( "fr.lip6.printers.Equipment" ) );
          TS_GUM_ASSERT_THROWS_NOTHING(
              prm->interface( "fr.lip6.printers.Printer" ) );
          delete prm;
        }
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    /// Testing classes and interfaces inheritance with subtypes methods
    void testComplexPrinters_2() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm" );
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        gum::prm::PRM<double>* prm = reader.prm();
        // Classes
        gum::prm::Class<double>& PowerSupply =
            prm->getClass( "fr.lip6.printers.PowerSupply" );
        gum::prm::Class<double>& Room =
            prm->getClass( "fr.lip6.printers.Room" );
        gum::prm::Class<double>& BWPrinter =
            prm->getClass( "fr.lip6.printers.BWPrinter" );
        gum::prm::Class<double>& ColorPrinter =
            prm->getClass( "fr.lip6.printers.ColorPrinter" );
        gum::prm::Class<double>& Computer =
            prm->getClass( "fr.lip6.printers.Computer" );
        // Interfaces
        gum::prm::Interface<double>& Equipment =
            prm->interface( "fr.lip6.printers.Equipment" );
        gum::prm::Interface<double>& Printer =
            prm->interface( "fr.lip6.printers.Printer" );
        // Testing PowerSupply
        TS_ASSERT( PowerSupply.isSubTypeOf( PowerSupply ) );
        TS_ASSERT( not PowerSupply.isSubTypeOf( Room ) );
        TS_ASSERT( not PowerSupply.isSubTypeOf( Equipment ) );
        TS_ASSERT( not PowerSupply.isSubTypeOf( Printer ) );
        TS_ASSERT( not PowerSupply.isSubTypeOf( BWPrinter ) );
        TS_ASSERT( not PowerSupply.isSubTypeOf( ColorPrinter ) );
        TS_ASSERT( not PowerSupply.isSubTypeOf( Computer ) );
        // Testing Room
        TS_ASSERT( not Room.isSubTypeOf( PowerSupply ) );
        TS_ASSERT( Room.isSubTypeOf( Room ) );
        TS_ASSERT( not Room.isSubTypeOf( Equipment ) );
        TS_ASSERT( not Room.isSubTypeOf( Printer ) );
        TS_ASSERT( not Room.isSubTypeOf( BWPrinter ) );
        TS_ASSERT( not Room.isSubTypeOf( ColorPrinter ) );
        TS_ASSERT( not Room.isSubTypeOf( Computer ) );
        // Testing Equipment
        TS_ASSERT( not Equipment.isSubTypeOf( PowerSupply ) );
        TS_ASSERT( not Equipment.isSubTypeOf( Room ) );
        TS_ASSERT( Equipment.isSubTypeOf( Equipment ) );
        TS_ASSERT( not Equipment.isSubTypeOf( Printer ) );
        TS_ASSERT( not Equipment.isSubTypeOf( BWPrinter ) );
        TS_ASSERT( not Equipment.isSubTypeOf( ColorPrinter ) );
        TS_ASSERT( not Equipment.isSubTypeOf( Computer ) );
        // Testing Printer
        TS_ASSERT( not Printer.isSubTypeOf( PowerSupply ) );
        TS_ASSERT( not Printer.isSubTypeOf( Room ) );
        TS_ASSERT( Printer.isSubTypeOf( Equipment ) );
        TS_ASSERT( Printer.isSubTypeOf( Printer ) );
        TS_ASSERT( not Printer.isSubTypeOf( BWPrinter ) );
        TS_ASSERT( not Printer.isSubTypeOf( ColorPrinter ) );
        TS_ASSERT( not Printer.isSubTypeOf( Computer ) );
        // Testing BWPrinter
        TS_ASSERT( not BWPrinter.isSubTypeOf( PowerSupply ) );
        TS_ASSERT( not BWPrinter.isSubTypeOf( Room ) );
        TS_ASSERT( BWPrinter.isSubTypeOf( Equipment ) );
        TS_ASSERT( BWPrinter.isSubTypeOf( Printer ) );
        TS_ASSERT( BWPrinter.isSubTypeOf( BWPrinter ) );
        TS_ASSERT( not BWPrinter.isSubTypeOf( ColorPrinter ) );
        TS_ASSERT( not BWPrinter.isSubTypeOf( Computer ) );
        // Testing ColorPrinter
        TS_ASSERT( not ColorPrinter.isSubTypeOf( PowerSupply ) );
        TS_ASSERT( not ColorPrinter.isSubTypeOf( Room ) );
        TS_ASSERT( ColorPrinter.isSubTypeOf( Equipment ) );
        TS_ASSERT( ColorPrinter.isSubTypeOf( Printer ) );
        TS_ASSERT( not ColorPrinter.isSubTypeOf( BWPrinter ) );
        TS_ASSERT( ColorPrinter.isSubTypeOf( ColorPrinter ) );
        TS_ASSERT( not ColorPrinter.isSubTypeOf( Computer ) );
        // Testing Computer
        TS_ASSERT( not Computer.isSubTypeOf( PowerSupply ) );
        TS_ASSERT( not Computer.isSubTypeOf( Room ) );
        TS_ASSERT( Computer.isSubTypeOf( Equipment ) );
        TS_ASSERT( not Computer.isSubTypeOf( Printer ) );
        TS_ASSERT( not Computer.isSubTypeOf( BWPrinter ) );
        TS_ASSERT( not Computer.isSubTypeOf( ColorPrinter ) );
        TS_ASSERT( Computer.isSubTypeOf( Computer ) );
        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    /// Testing classes and interfaces inheritance with supertypes methods
    void testComplexPrinters_3() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm" );
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        gum::prm::PRM<double>* prm = reader.prm();
        // Classes
        gum::prm::Class<double>& PowerSupply =
            prm->getClass( "fr.lip6.printers.PowerSupply" );
        gum::prm::Class<double>& Room =
            prm->getClass( "fr.lip6.printers.Room" );
        gum::prm::Class<double>& BWPrinter =
            prm->getClass( "fr.lip6.printers.BWPrinter" );
        gum::prm::Class<double>& ColorPrinter =
            prm->getClass( "fr.lip6.printers.ColorPrinter" );
        gum::prm::Class<double>& Computer =
            prm->getClass( "fr.lip6.printers.Computer" );
        // Interfaces
        gum::prm::Interface<double>& Equipment =
            prm->interface( "fr.lip6.printers.Equipment" );
        gum::prm::Interface<double>& Printer =
            prm->interface( "fr.lip6.printers.Printer" );
        // Testing PowerSupply
        TS_ASSERT( PowerSupply.isSuperTypeOf( PowerSupply ) );
        TS_ASSERT( not PowerSupply.isSuperTypeOf( Room ) );
        TS_ASSERT( not PowerSupply.isSuperTypeOf( Equipment ) );
        TS_ASSERT( not PowerSupply.isSuperTypeOf( Printer ) );
        TS_ASSERT( not PowerSupply.isSuperTypeOf( BWPrinter ) );
        TS_ASSERT( not PowerSupply.isSuperTypeOf( ColorPrinter ) );
        TS_ASSERT( not PowerSupply.isSuperTypeOf( Computer ) );
        // Testing Room
        TS_ASSERT( not Room.isSuperTypeOf( PowerSupply ) );
        TS_ASSERT( Room.isSuperTypeOf( Room ) );
        TS_ASSERT( not Room.isSuperTypeOf( Equipment ) );
        TS_ASSERT( not Room.isSuperTypeOf( Printer ) );
        TS_ASSERT( not Room.isSuperTypeOf( BWPrinter ) );
        TS_ASSERT( not Room.isSuperTypeOf( ColorPrinter ) );
        TS_ASSERT( not Room.isSuperTypeOf( Computer ) );
        // Testing Equipment
        TS_ASSERT( not Equipment.isSuperTypeOf( PowerSupply ) );
        TS_ASSERT( not Equipment.isSuperTypeOf( Room ) );
        TS_ASSERT( Equipment.isSuperTypeOf( Equipment ) );
        TS_ASSERT( Equipment.isSuperTypeOf( Printer ) );
        TS_ASSERT( Equipment.isSuperTypeOf( BWPrinter ) );
        TS_ASSERT( Equipment.isSuperTypeOf( ColorPrinter ) );
        TS_ASSERT( Equipment.isSuperTypeOf( Computer ) );
        // Testing Printer
        TS_ASSERT( not Printer.isSuperTypeOf( PowerSupply ) );
        TS_ASSERT( not Printer.isSuperTypeOf( Room ) );
        TS_ASSERT( not Printer.isSuperTypeOf( Equipment ) );
        TS_ASSERT( Printer.isSuperTypeOf( Printer ) );
        TS_ASSERT( Printer.isSuperTypeOf( BWPrinter ) );
        TS_ASSERT( Printer.isSuperTypeOf( ColorPrinter ) );
        TS_ASSERT( not Printer.isSuperTypeOf( Computer ) );
        // Testing BWPrinter
        TS_ASSERT( not BWPrinter.isSuperTypeOf( PowerSupply ) );
        TS_ASSERT( not BWPrinter.isSuperTypeOf( Room ) );
        TS_ASSERT( not BWPrinter.isSuperTypeOf( Equipment ) );
        TS_ASSERT( not BWPrinter.isSuperTypeOf( Printer ) );
        TS_ASSERT( BWPrinter.isSuperTypeOf( BWPrinter ) );
        TS_ASSERT( not BWPrinter.isSuperTypeOf( ColorPrinter ) );
        TS_ASSERT( not BWPrinter.isSuperTypeOf( Computer ) );
        // Testing ColorPrinter
        TS_ASSERT( not ColorPrinter.isSuperTypeOf( PowerSupply ) );
        TS_ASSERT( not ColorPrinter.isSuperTypeOf( Room ) );
        TS_ASSERT( not ColorPrinter.isSuperTypeOf( Equipment ) );
        TS_ASSERT( not ColorPrinter.isSuperTypeOf( Printer ) );
        TS_ASSERT( not ColorPrinter.isSuperTypeOf( BWPrinter ) );
        TS_ASSERT( ColorPrinter.isSuperTypeOf( ColorPrinter ) );
        TS_ASSERT( not ColorPrinter.isSuperTypeOf( Computer ) );
        // Testing Computer
        TS_ASSERT( not Computer.isSuperTypeOf( PowerSupply ) );
        TS_ASSERT( not Computer.isSuperTypeOf( Room ) );
        TS_ASSERT( not Computer.isSuperTypeOf( Equipment ) );
        TS_ASSERT( not Computer.isSuperTypeOf( Printer ) );
        TS_ASSERT( not Computer.isSuperTypeOf( BWPrinter ) );
        TS_ASSERT( not Computer.isSuperTypeOf( ColorPrinter ) );
        TS_ASSERT( Computer.isSuperTypeOf( Computer ) );
        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    // Testing class PowerSupply
    void testComplexPrinters_4() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm" );
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        gum::prm::PRM<double>* prm = reader.prm();
        // Classes
        gum::prm::Class<double>& PowerSupply =
            prm->getClass( "fr.lip6.printers.PowerSupply" );
        TS_ASSERT_EQUALS( PowerSupply.attributes().size(), (gum::Size)2 );
        TS_ASSERT_EQUALS( PowerSupply.dag().sizeArcs(), (gum::Size)1 );
        TS_GUM_ASSERT_THROWS_NOTHING( PowerSupply["(boolean)state"] );
        TS_GUM_ASSERT_THROWS_NOTHING( PowerSupply["state"] );
        TS_ASSERT( PowerSupply["state"].type().isSubTypeOf(
            PowerSupply["(boolean)state"].type() ) );
        gum::NodeId n1 = 0;
        gum::NodeId n2 = 0;
        gum::NodeId n3 = 0;
        TS_GUM_ASSERT_THROWS_NOTHING( n1 = PowerSupply["(boolean)state"].id() );
        TS_GUM_ASSERT_THROWS_NOTHING( n2 = PowerSupply["state"].id() );
        TS_GUM_ASSERT_THROWS_NOTHING(
            n3 = PowerSupply["(fr.lip6.printers.t_state)state"].id() );
        TS_ASSERT_EQUALS( n2, n3 );
        TS_ASSERT( PowerSupply.dag().existsArc( n2, n1 ) );
        TS_ASSERT_EQUALS( PowerSupply.referenceSlots().size(), (gum::Size)0 );
        TS_ASSERT_EQUALS( PowerSupply.aggregates().size(), (gum::Size)0 );
        TS_ASSERT_EQUALS( PowerSupply.slotChains().size(), (gum::Size)0 );
        // Testing interfaces for the state attribute and its cast descendant
        TS_ASSERT(
            not PowerSupply.isOutputNode( PowerSupply["(boolean)state"] ) );
        TS_ASSERT(
            not PowerSupply.isInputNode( PowerSupply["(boolean)state"] ) );
        TS_ASSERT( PowerSupply.isInnerNode( PowerSupply["(boolean)state"] ) );
        TS_ASSERT( PowerSupply.isOutputNode( PowerSupply["state"] ) );
        TS_ASSERT( not PowerSupply.isInputNode( PowerSupply["state"] ) );
        TS_ASSERT( not PowerSupply.isInnerNode( PowerSupply["state"] ) );
        TS_ASSERT( PowerSupply.isOutputNode(
            PowerSupply["(fr.lip6.printers.t_state)state"] ) );
        TS_ASSERT( not PowerSupply.isInputNode(
            PowerSupply["(fr.lip6.printers.t_state)state"] ) );
        TS_ASSERT( not PowerSupply.isInnerNode(
            PowerSupply["(fr.lip6.printers.t_state)state"] ) );
        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    // Testing class Room
    void testComplexPrinters_5() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm" );
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        gum::prm::PRM<double>* prm = reader.prm();
        gum::prm::Class<double>& Room =
            prm->getClass( "fr.lip6.printers.Room" );
        TS_ASSERT_EQUALS( Room.attributes().size(), (gum::Size)0 );
        TS_ASSERT_EQUALS( Room.referenceSlots().size(), (gum::Size)1 );
        TS_ASSERT_EQUALS( Room.aggregates().size(), (gum::Size)0 );
        TS_ASSERT_EQUALS( Room.slotChains().size(), (gum::Size)0 );
        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    // Testing interface Equipment
    void testComplexPrinters_6() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm" );
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        gum::prm::PRM<double>* prm = reader.prm();
        gum::prm::Interface<double>& Equipment =
            prm->interface( "fr.lip6.printers.Equipment" );
        TS_ASSERT_EQUALS( Equipment.referenceSlots().size(), (gum::Size)1 );
        TS_ASSERT_EQUALS( Equipment.attributes().size(), (gum::Size)3 );
        TS_GUM_ASSERT_THROWS_NOTHING( Equipment["equipState"] );
        TS_ASSERT_EQUALS( Equipment["equipState"].type().name(),
                          "fr.lip6.printers.t_degraded" );
        TS_GUM_ASSERT_THROWS_NOTHING(
            Equipment["(fr.lip6.printers.t_state)equipState"] );
        TS_ASSERT_EQUALS(
            Equipment["(fr.lip6.printers.t_state)equipState"].type().name(),
            "fr.lip6.printers.t_state" );
        TS_GUM_ASSERT_THROWS_NOTHING( Equipment["(boolean)equipState"] );
        TS_ASSERT_EQUALS( Equipment["(boolean)equipState"].type().name(),
                          "boolean" );
        // Testing gum::NodeId, inputs and outputs
        gum::NodeId st = Equipment["equipState"].id();
        gum::NodeId deg_st =
            Equipment["(fr.lip6.printers.t_degraded)equipState"].id();
        gum::NodeId st_st =
            Equipment["(fr.lip6.printers.t_state)equipState"].id();
        gum::NodeId bool_st = Equipment["(boolean)equipState"].id();
        TS_ASSERT_EQUALS( st, deg_st );
        TS_ASSERT_DIFFERS( st, st_st );
        TS_ASSERT_DIFFERS( st, bool_st );
        TS_ASSERT_DIFFERS( st_st, bool_st );
        // t_degraded equipState
        TS_ASSERT( not Equipment.isOutputNode( Equipment["equipState"] ) );
        TS_ASSERT( not Equipment.isInputNode( Equipment["equipState"] ) );
        TS_ASSERT( Equipment.isInnerNode( Equipment["equipState"] ) );
        // t_state equipState
        TS_ASSERT( not Equipment.isOutputNode(
            Equipment["(fr.lip6.printers.t_state)equipState"] ) );
        TS_ASSERT( not Equipment.isInputNode(
            Equipment["(fr.lip6.printers.t_state)equipState"] ) );
        TS_ASSERT( Equipment.isInnerNode(
            Equipment["(fr.lip6.printers.t_state)equipState"] ) );
        // boolean equipState
        TS_ASSERT(
            not Equipment.isOutputNode( Equipment["(boolean)equipState"] ) );
        TS_ASSERT(
            not Equipment.isInputNode( Equipment["(boolean)equipState"] ) );
        TS_ASSERT( Equipment.isInnerNode( Equipment["(boolean)equipState"] ) );
        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    // Testing interface Printer
    void testComplexPrinters_7() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm" );
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        gum::prm::PRM<double>* prm = reader.prm();
        TS_GUM_ASSERT_THROWS_NOTHING(
            prm->interface( "fr.lip6.printers.Printer" ) );
        gum::prm::Interface<double>& Printer =
            prm->interface( "fr.lip6.printers.Printer" );
        TS_ASSERT_EQUALS( Printer.referenceSlots().size(), (gum::Size)1 );
        TS_ASSERT_EQUALS( Printer.attributes().size(), (gum::Size)5 );
        TS_GUM_ASSERT_THROWS_NOTHING( Printer["room"] );
        TS_GUM_ASSERT_THROWS_NOTHING( Printer["equipState"] );
        TS_GUM_ASSERT_THROWS_NOTHING( Printer["hasPaper"] );
        TS_GUM_ASSERT_THROWS_NOTHING( Printer["hasInk"] );
        TS_ASSERT_EQUALS( Printer["equipState"].type().name(),
                          "fr.lip6.printers.t_degraded" );
        TS_ASSERT_EQUALS( Printer["hasPaper"].type().name(), "boolean" );
        TS_ASSERT_EQUALS( Printer["hasInk"].type().name(), "boolean" );
        // Testing gum::NodeId, inputs and outputs
        gum::NodeId st = Printer["equipState"].id();
        gum::NodeId deg_st =
            Printer["(fr.lip6.printers.t_degraded)equipState"].id();
        gum::NodeId st_st =
            Printer["(fr.lip6.printers.t_state)equipState"].id();
        gum::NodeId bool_st = Printer["(boolean)equipState"].id();
        gum::NodeId hp = Printer["hasPaper"].id();
        gum::NodeId bool_hp = Printer["(boolean)hasPaper"].id();
        gum::NodeId hi = Printer["hasInk"].id();
        gum::NodeId bool_hi = Printer["(boolean)hasInk"].id();
        TS_ASSERT_EQUALS( st, deg_st );
        TS_ASSERT_DIFFERS( st, st_st );
        TS_ASSERT_DIFFERS( st, bool_st );
        TS_ASSERT_DIFFERS( st_st, bool_st );
        TS_ASSERT_EQUALS( hp, bool_hp );
        TS_ASSERT_EQUALS( hi, bool_hi );
        // t_degraded equipState
        TS_ASSERT( Printer.isOutputNode(
            Printer["(fr.lip6.printers.t_degraded)equipState"] ) );
        TS_ASSERT( not Printer.isInputNode(
            Printer["(fr.lip6.printers.t_degraded)equipState"] ) );
        TS_ASSERT( not Printer.isInnerNode(
            Printer["(fr.lip6.printers.t_degraded)equipState"] ) );
        // t_state equipState
        TS_ASSERT( Printer.isOutputNode(
            Printer["(fr.lip6.printers.t_state)equipState"] ) );
        TS_ASSERT( not Printer.isInputNode(
            Printer["(fr.lip6.printers.t_state)equipState"] ) );
        TS_ASSERT( not Printer.isInnerNode(
            Printer["(fr.lip6.printers.t_state)equipState"] ) );
        // boolean equipState
        TS_ASSERT( not Printer.isOutputNode( Printer["(boolean)equipState"] ) );
        TS_ASSERT( not Printer.isInputNode( Printer["(boolean)equipState"] ) );
        TS_ASSERT( Printer.isInnerNode( Printer["(boolean)equipState"] ) );
        // Boolean hasPaper
        TS_ASSERT( not Printer.isOutputNode( Printer["hasPaper"] ) );
        TS_ASSERT( not Printer.isInputNode( Printer["hasPaper"] ) );
        TS_ASSERT( Printer.isInnerNode( Printer["hasPaper"] ) );
        // Boolean hasInk
        TS_ASSERT( not Printer.isOutputNode( Printer["hasInk"] ) );
        TS_ASSERT( not Printer.isInputNode( Printer["hasInk"] ) );
        TS_ASSERT( Printer.isInnerNode( Printer["hasInk"] ) );
        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    /// @TODO Make this test past for IO flags optimisation
    // Testing class BWPrinter
    // void testComplexPrinters_8() {
    //   try {
    //     gum::prm::o3prm::O3prmReader<double> reader;
    //     std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm"
    //     );
    //     std::string package = "fr.lip6.printers";
    //     TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
    //     gum::prm::PRM<double>* prm = reader.prm();
    //     TS_GUM_ASSERT_THROWS_NOTHING(
    //         prm->getClass( "fr.lip6.printers.BWPrinter" ) );
    //     gum::prm::Class<double>& BWPrinter =
    //         prm->getClass( "fr.lip6.printers.BWPrinter" );
    //     TS_ASSERT_EQUALS( BWPrinter.referenceSlots().size(), (gum::Size)1 );
    //     TS_ASSERT_EQUALS( BWPrinter.attributes().size(), (gum::Size)9 );
    //     TS_ASSERT_EQUALS( BWPrinter.slotChains().size(), (gum::Size)1 );
    //     TS_ASSERT_EQUALS( BWPrinter.aggregates().size(), (gum::Size)0 );
    //     TS_GUM_ASSERT_THROWS_NOTHING( BWPrinter["room"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING( BWPrinter["(boolean)equipState"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_state)equipState"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_degraded)equipState"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING( BWPrinter["equipState"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING( BWPrinter["(boolean)hasPaper"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_state)hasPaper"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_paper)hasPaper"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING( BWPrinter["hasPaper"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING( BWPrinter["(boolean)hasInk"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_state)hasInk"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING(
    //         BWPrinter["(fr.lip6.printers.t_ink)hasInk"] );
    //     TS_GUM_ASSERT_THROWS_NOTHING( BWPrinter["hasInk"] );
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
    //     TS_ASSERT_EQUALS( st, deg_st );
    //     TS_ASSERT_DIFFERS( st, st_st );
    //     TS_ASSERT_DIFFERS( st, bool_st );
    //     TS_ASSERT_DIFFERS( st_st, bool_st );
    //     TS_ASSERT_EQUALS( hp, p_hp );
    //     TS_ASSERT_DIFFERS( hp, st_hp );
    //     TS_ASSERT_DIFFERS( hp, bool_hp );
    //     TS_ASSERT_EQUALS( hi, i_hi );
    //     TS_ASSERT_DIFFERS( hi, st_hi );
    //     TS_ASSERT_DIFFERS( hi, bool_hi );
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
    //     TS_ASSERT( BWPrinter.isInnerNode( BWPrinter["(boolean)hasPaper"] ) );
    //     // Boolean hasInk
    //     TS_ASSERT( not BWPrinter.isOutputNode( BWPrinter["(boolean)hasInk"] )
    //     );
    //     TS_ASSERT( not BWPrinter.isInputNode( BWPrinter["(boolean)hasInk"] )
    //     );
    //     TS_ASSERT( BWPrinter.isInnerNode( BWPrinter["hasInk"] ) );
    //     delete prm;
    //   } catch ( gum::Exception ) {
    //     TS_ASSERT( false );
    //   }
    // }

    /// @TODO Make this test pass for IO Flags optimisation
    // Testing class ColorPrinter
    // void testComplexPrinters_9() {
    //  try {
    //    gum::prm::o3prm::O3prmReader<double> reader;
    //    std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm"
    //    );
    //    std::string package = "fr.lip6.printers";
    //    TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
    //    gum::prm::PRM<double>* prm = reader.prm();
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        prm->getClass( "fr.lip6.printers.ColorPrinter" ) );
    //    gum::prm::Class<double>& ColorPrinter =
    //        prm->getClass( "fr.lip6.printers.ColorPrinter" );
    //    TS_ASSERT_EQUALS( ColorPrinter.referenceSlots().size(), (gum::Size)1
    //    );
    //    TS_ASSERT_EQUALS( ColorPrinter.attributes().size(), (gum::Size)19 );
    //    TS_ASSERT_EQUALS( ColorPrinter.aggregates().size(), (gum::Size)0 );
    //    TS_ASSERT_EQUALS( ColorPrinter.slotChains().size(), (gum::Size)1 );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["room"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)equipState"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)equipState"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_degraded)equipState"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["equipState"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)hasPaper"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)hasPaper"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_paper)hasPaper"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["hasPaper"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)hasInk"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)black"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)black"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_ink)black"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["black"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)magenta"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)magenta"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_ink)magenta"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["magenta"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)yellow"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)yellow"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_ink)yellow"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["yellow"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["(boolean)cyan"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_state)cyan"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING(
    //        ColorPrinter["(fr.lip6.printers.t_ink)cyan"] );
    //    TS_GUM_ASSERT_THROWS_NOTHING( ColorPrinter["cyan"] );
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
    //    TS_ASSERT_EQUALS( st, deg_st );
    //    TS_ASSERT_DIFFERS( st, st_st );
    //    TS_ASSERT_DIFFERS( st, bool_st );
    //    TS_ASSERT_DIFFERS( st_st, bool_st );
    //    TS_ASSERT_EQUALS( hp, p_hp );
    //    TS_ASSERT_DIFFERS( hp, st_hp );
    //    TS_ASSERT_DIFFERS( hp, bool_hp );
    //    TS_ASSERT_EQUALS( hi, bool_hi );
    //    TS_ASSERT_EQUALS( bl, i_bl );
    //    TS_ASSERT_DIFFERS( bl, st_bl );
    //    TS_ASSERT_DIFFERS( bl, bool_bl );
    //    TS_ASSERT_DIFFERS( st_bl, bool_bl );
    //    TS_ASSERT_EQUALS( ma, i_ma );
    //    TS_ASSERT_DIFFERS( ma, st_ma );
    //    TS_ASSERT_DIFFERS( ma, bool_ma );
    //    TS_ASSERT_DIFFERS( st_ma, bool_ma );
    //    TS_ASSERT_EQUALS( ye, i_ye );
    //    TS_ASSERT_DIFFERS( ye, st_ye );
    //    TS_ASSERT_DIFFERS( ye, bool_ye );
    //    TS_ASSERT_DIFFERS( st_ye, bool_ye );
    //    TS_ASSERT_EQUALS( ce, i_ce );
    //    TS_ASSERT_DIFFERS( ce, st_ce );
    //    TS_ASSERT_DIFFERS( ce, bool_ce );
    //    TS_ASSERT_DIFFERS( st_ce, bool_ce );
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
    //    TS_ASSERT( false );
    //  }
    //}

    // Testing class Computer
    void testComplexPrinters_10() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm" );
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        gum::prm::PRM<double>* prm = reader.prm();
        gum::prm::Class<double>& Computer =
            prm->getClass( "fr.lip6.printers.Computer" );
        TS_ASSERT_EQUALS( Computer.attributes().size(), (gum::Size)6 );
        TS_ASSERT_EQUALS( Computer.referenceSlots().size(), (gum::Size)2 );
        TS_ASSERT_EQUALS( Computer.aggregates().size(), (gum::Size)2 );
        TS_ASSERT_EQUALS( Computer.slotChains().size(), (gum::Size)2 );
        TS_GUM_ASSERT_THROWS_NOTHING( Computer["(boolean)functional_printer"] );
        TS_GUM_ASSERT_THROWS_NOTHING( Computer["functional_printer"] );
        TS_GUM_ASSERT_THROWS_NOTHING( Computer["(boolean)degraded_printer"] );
        TS_GUM_ASSERT_THROWS_NOTHING( Computer["degraded_printer"] );
        TS_GUM_ASSERT_THROWS_NOTHING( Computer["(boolean)working_printer"] );
        TS_GUM_ASSERT_THROWS_NOTHING( Computer["working_printer"] );
        TS_GUM_ASSERT_THROWS_NOTHING( Computer["(boolean)equipState"] );
        TS_GUM_ASSERT_THROWS_NOTHING(
            Computer["(fr.lip6.printers.t_state)equipState"] );
        TS_GUM_ASSERT_THROWS_NOTHING(
            Computer["(fr.lip6.printers.t_degraded)equipState"] );
        TS_GUM_ASSERT_THROWS_NOTHING( Computer["equipState"] );
        TS_GUM_ASSERT_THROWS_NOTHING( Computer["(boolean)can_print"] );
        TS_GUM_ASSERT_THROWS_NOTHING( Computer["can_print"] );
        TS_GUM_ASSERT_THROWS_NOTHING( Computer["printers.equipState"] );
        TS_GUM_ASSERT_THROWS_NOTHING( Computer["room.power.state"] );
        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    // Testing class Computer
    void testComplexPrinters_11() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm" );
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        gum::prm::PRM<double>* prm = reader.prm();
        gum::prm::Class<double>& SafeComputer =
            prm->getClass( "fr.lip6.printers.SafeComputer" );
        TS_ASSERT_EQUALS( SafeComputer.attributes().size(), (gum::Size)5 );
        TS_ASSERT_EQUALS( SafeComputer.referenceSlots().size(), (gum::Size)2 );
        TS_ASSERT_EQUALS( SafeComputer.aggregates().size(), (gum::Size)2 );
        TS_ASSERT_EQUALS( SafeComputer.slotChains().size(), (gum::Size)3 );
        TS_GUM_ASSERT_THROWS_NOTHING(
            SafeComputer["(boolean)functional_printer"] );
        TS_GUM_ASSERT_THROWS_NOTHING( SafeComputer["functional_printer"] );
        TS_GUM_ASSERT_THROWS_NOTHING(
            SafeComputer["(boolean)degraded_printer"] );
        TS_GUM_ASSERT_THROWS_NOTHING( SafeComputer["degraded_printer"] );
        TS_GUM_ASSERT_THROWS_NOTHING(
            SafeComputer["(boolean)working_printer"] );
        TS_GUM_ASSERT_THROWS_NOTHING( SafeComputer["working_printer"] );
        TS_GUM_ASSERT_THROWS_NOTHING( SafeComputer["(boolean)equipState"] );
        TS_GUM_ASSERT_THROWS_NOTHING(
            SafeComputer["(fr.lip6.printers.t_state)equipState"] );
        TS_GUM_ASSERT_THROWS_NOTHING(
            SafeComputer["(fr.lip6.printers.t_degraded)equipState"] );
        TS_GUM_ASSERT_THROWS_NOTHING( SafeComputer["equipState"] );
        TS_GUM_ASSERT_THROWS_NOTHING( SafeComputer["(boolean)can_print"] );
        TS_GUM_ASSERT_THROWS_NOTHING( SafeComputer["can_print"] );
        TS_GUM_ASSERT_THROWS_NOTHING(
            SafeComputer["printers.(fr.lip6.printers.t_state)equipState"] );
        TS_GUM_ASSERT_THROWS_NOTHING(
            SafeComputer["printers.(fr.lip6.printers.t_degraded)equipState"] );
        TS_GUM_ASSERT_THROWS_NOTHING(
            SafeComputer["room.power.(fr.lip6.printers.t_state)state"] );
        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    void testPrintersSystems1() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile(
            GET_RESSOURCES_PATH( "o3prm/printers_systems.o3prm" ) ) );
        TS_ASSERT_EQUALS( reader.warnings(), (gum::Size)0 );
        TS_ASSERT_EQUALS( reader.errors(), (gum::Size)0 );
        reader.showElegantErrorsAndWarnings();
        gum::prm::PRM<double>* prm = 0;
        TS_GUM_ASSERT_THROWS_NOTHING( prm = reader.prm() );

        if ( prm ) {
          delete prm;
        }
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    void testComplexPrintersSystems1() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile(
            GET_RESSOURCES_PATH( "o3prm/complexprinters_system.o3prm" ) ) );
        TS_ASSERT_EQUALS( reader.warnings(), (gum::Size)0 );
        TS_ASSERT_EQUALS( reader.errors(), (gum::Size)0 );

        if ( reader.errors() ) {
          reader.showElegantErrorsAndWarnings();
        }

        gum::prm::PRM<double>* prm = 0;
        TS_GUM_ASSERT_THROWS_NOTHING( prm = reader.prm() );
        gum::prm::System<double>* sys = 0;
        TS_GUM_ASSERT_THROWS_NOTHING( sys = &( prm->system( "aSys" ) ) );
        gum::prm::Class<double>& Power = prm->getClass( "PowerSupply" );
        gum::prm::Class<double>& Room = prm->getClass( "Room" );
        gum::prm::Class<double>& BWPrinter = prm->getClass( "BWPrinter" );
        gum::prm::Class<double>& ColorPrinter = prm->getClass( "ColorPrinter" );
        gum::prm::Class<double>& Computer = prm->getClass( "Computer" );
        try {
          sys->get( Power ).size();
        } catch ( gum::Exception& e ) {
          std::cout << e.errorType();
          std::cout << e.errorContent();
          std::cout << e.errorCallStack();
        }
        TS_ASSERT_EQUALS( sys->get( Power ).size(), (gum::Size)1 );
        TS_ASSERT_EQUALS( sys->get( Room ).size(), (gum::Size)1 );
        TS_ASSERT_EQUALS( sys->get( BWPrinter ).size(), (gum::Size)10 );
        TS_ASSERT_EQUALS( sys->get( ColorPrinter ).size(), (gum::Size)2 );
        TS_ASSERT_EQUALS( sys->get( Computer ).size(), (gum::Size)2 );
        int count = 0;

        for ( auto iter = sys->begin(); iter != sys->end(); ++iter ) {
          ++count;

          for ( auto jter = ( *( iter.val() ) ).begin();
                jter != ( *( iter.val() ) ).end();
                ++jter ) {
            if ( ( *( jter.val() ) ).cpf().nbrDim() == 0 ) {
              std::stringstream sBuff;
              sBuff << ( *( iter.val() ) ).name() << "."
                    << ( *( jter.val() ) ).safeName();
              GUM_TRACE( sBuff.str() );
            }

            if ( gum::prm::ClassElement<double>::isAggregate(
                     ( *( iter.val() ) )
                         .type()
                         .get( ( *( jter.val() ) ).id() ) ) ) {
              if ( ( *( jter.val() ) ).cpf().nbrDim() == 1 ) {
                std::stringstream sBuff;
                sBuff << ( *( iter.val() ) ).name() << "."
                      << ( *( jter.val() ) ).safeName();
                GUM_TRACE( sBuff.str() );
              }
            }
          }
        }

        TS_ASSERT_EQUALS( count, 18 );

        if ( prm ) {
          delete prm;
        }
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    void testCPF() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        TS_GUM_ASSERT_THROWS_NOTHING(
            reader.readFile( GET_RESSOURCES_PATH( "o3prm/inference.o3prm" ) ) );
        TS_ASSERT_EQUALS( reader.warnings(), (gum::Size)0 );
        TS_ASSERT_EQUALS( reader.errors(), (gum::Size)0 );
        gum::prm::PRM<double>* prm = 0;
        TS_GUM_ASSERT_THROWS_NOTHING( prm = reader.prm() );
        gum::prm::System<double>* sys = 0;
        TS_GUM_ASSERT_THROWS_NOTHING( sys = &( prm->system( "aSys" ) ) );

        for ( gum::prm::System<double>::iterator iter = sys->begin();
              iter != sys->end();
              ++iter ) {
          for ( gum::prm::Instance<double>::iterator jter =
                    ( *( iter.val() ) ).begin();
                jter != ( *( iter.val() ) ).end();
                ++jter ) {
            gum::Instantiation i( ( *( jter.val() ) ).cpf() ), var;
            var.add( ( *( jter.val() ) ).type().variable() );

            for ( i.setFirstOut( var ); not i.end(); i.incOut( var ) ) {
              gum::prm::prm_float f = 0.0;

              for ( i.setFirstIn( var ); not i.end(); i.incIn( var ) ) {
                f += ( *( jter.val() ) ).cpf().get( i );
              }

              TS_ASSERT_DELTA( f, 1.0, 1e-9 );
              i.unsetOverflow();
            }
          }
        }

        if ( prm ) {
          delete prm;
        }
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    void testImports() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        reader.addClassPath( GET_RESSOURCES_PATH( "o3prmr" ) );
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile(
            GET_RESSOURCES_PATH( "o3prmr/systems/MySystem.o3prm" ),
            "systems.MySystem" ) );
        TS_ASSERT_EQUALS( reader.warnings(), (gum::Size)0 );
        TS_ASSERT_EQUALS( reader.errors(), (gum::Size)0 );
        reader.showElegantErrorsAndWarnings();

        gum::prm::PRM<double>* prm = nullptr;

        TS_GUM_ASSERT_THROWS_NOTHING( prm = reader.prm() );
        gum::Size class_count = prm->classes().size();
        class_count += prm->interfaces().size();
        TS_ASSERT_EQUALS( class_count,
                          (gum::Size)11 );  // Don't forget param subclasses !

        TS_ASSERT_EQUALS( prm->systems().size(), (gum::Size)1 );
        gum::prm::System<double>* sys = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(
            sys = &( prm->system( "systems.MySystem.MySystem" ) ) );
        TS_ASSERT_EQUALS( sys->size(), (gum::Size)16 );

        // Checking that all class DAG are generated
        for ( auto c : prm->classes() ) {
          for ( auto node = c->dag().begin(); node != c->dag().end(); ++node ) {
            TS_ASSERT( c->exists( *node ) );
            TS_ASSERT_THROWS_NOTHING( c->get( *node ) );
            for ( auto prnt : c->dag().parents( *node ) ) {
              TS_ASSERT( c->exists( prnt ) );
              TS_ASSERT_THROWS_NOTHING( c->get( prnt ) );
            }

            for ( auto child : c->dag().children( *node ) ) {
              TS_ASSERT( c->exists( child ) );
              TS_ASSERT_THROWS_NOTHING( c->get( child ) );
            }
          }
          // checking parameters
          for ( auto elt : c->parameters() ) {
            c->dag().exists( elt->id() );
          }
          for ( auto elt : c->referenceSlots() ) {
            c->dag().exists( elt->id() );
          }
          for ( auto elt : c->attributes() ) {
            c->dag().exists( elt->id() );
          }
          for ( auto elt : c->aggregates() ) {
            c->dag().exists( elt->id() );
          }
          for ( auto elt : c->slotChains() ) {
            c->dag().exists( elt->id() );
          }
        }

        // Checking that all instances attributes are generated
        for ( auto i = sys->begin(); i != sys->end(); ++i ) {
          for ( auto attr = i.val()->begin(); attr != i.val()->end(); ++attr ) {
            try {
              auto inst = gum::Instantiation( attr.val()->cpf() );
              auto sum = 0.0;
              for ( inst.begin(); not inst.end(); inst.inc() ) {
                sum += attr.val()->cpf()[inst];
              }
              auto card = 1;
              for ( auto var : attr.val()->cpf().variablesSequence() ) {
                card *= var->domainSize();
              }
              card /= attr.val()->type()->domainSize();

              TS_ASSERT_DELTA( sum / card, 1.0, 1e-6 );
            } catch ( gum::Exception& e ) {
              TS_FAIL( e.errorContent() );
            }
          }
        }

        if ( prm ) {
          delete prm;
        }
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    void testParamClass() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm" );
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        gum::prm::PRM<double>* prm = reader.prm();
        gum::prm::Class<double>& ParamClass =
            prm->getClass( "fr.lip6.printers.ParamClass" );
        TS_ASSERT_EQUALS( ParamClass.attributes().size(), (gum::Size)9 );
        TS_ASSERT_EQUALS( ParamClass.referenceSlots().size(), (gum::Size)1 );
        TS_ASSERT_EQUALS( ParamClass.slotChains().size(), (gum::Size)1 );
        TS_GUM_ASSERT_THROWS_NOTHING( ParamClass["lambda"] );
        TS_GUM_ASSERT_THROWS_NOTHING( ParamClass["t"] );
        TS_GUM_ASSERT_THROWS_NOTHING( ParamClass["room"] );
        TS_GUM_ASSERT_THROWS_NOTHING( ParamClass["hasInk"] );
        TS_GUM_ASSERT_THROWS_NOTHING( ParamClass["hasPaper"] );
        TS_GUM_ASSERT_THROWS_NOTHING( ParamClass["equipState"] );

        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    void testSystemParamClass() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file =
            GET_RESSOURCES_PATH( "o3prm/complexprinters_system.o3prm" );
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        gum::prm::PRM<double>* prm = reader.prm();

        TS_GUM_ASSERT_THROWS_NOTHING(
            prm->getClass( "fr.lip6.printers.ParamClass" ) );
        TS_GUM_ASSERT_THROWS_NOTHING(
            prm->getClass( "fr.lip6.printers.ParamClass<lambda=0.4,t=4>" ) );
        TS_GUM_ASSERT_THROWS_NOTHING(
            prm->getClass( "fr.lip6.printers.ParamClass<lambda=0.001,t=4>" ) );

        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    void testParamClassHasInk() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm" );
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );

        auto prm = reader.prm();
        auto& ParamClass = prm->getClass( "fr.lip6.printers.ParamClass" );

        const auto& hasInk =
            static_cast<gum::prm::Attribute<double>&>( ParamClass["hasInk"] );

        gum::Instantiation inst( hasInk.cpf() );
        std::vector<double> values;
        for ( inst.begin(); not inst.end(); inst.inc() ) {
          values.push_back( hasInk.cpf()[inst] );
        }

        TS_ASSERT_EQUALS( values.size(), (size_t)2 );
        TS_ASSERT_DELTA( values[0], 0.4, 1e-6 );
        TS_ASSERT_DELTA( values[1], 0.6, 1e-6 );

        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    void testParamClassHasPaper() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/complexprinters.o3prm" );
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );

        auto prm = reader.prm();
        auto& ParamClass = prm->getClass( "fr.lip6.printers.ParamClass" );

        const auto& hasPaper =
            static_cast<gum::prm::Attribute<double>&>( ParamClass["hasPaper"] );

        gum::Instantiation inst( hasPaper.cpf() );
        std::vector<double> values;
        for ( inst.begin(); not inst.end(); inst.inc() ) {
          values.push_back( hasPaper.cpf()[inst] );
        }

        TS_ASSERT_EQUALS( values.size(), (size_t)3 );
        TS_ASSERT_DELTA( values[0], std::exp( -0.4 * 4 ), 1e-6 );
        TS_ASSERT_DELTA( values[1], 1 - std::exp( -0.4 * 4 ), 1e-6 );
        TS_ASSERT_DELTA( values[2], 0.0, 1e-6 );

        delete prm;
      } catch ( gum::Exception ) {
        TS_ASSERT( false );
      }
    }

    void testInstanceParamClassHasInk() {
      try {
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file =
            GET_RESSOURCES_PATH( "o3prm/complexprinters_system.o3prm" );
        std::string package = "fr.lip6.printers";
        TS_GUM_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );

        auto prm = reader.prm();
        auto& sys = prm->system( "fr.lip6.printers.aSys" );
        auto& paramBis = sys.get( "paramBis" );

        const auto& hasInk = paramBis.get( "hasInk" );

        gum::Instantiation inst( hasInk.cpf() );
        std::vector<double> values;
        for ( inst.begin(); not inst.end(); inst.inc() ) {
          values.push_back( hasInk.cpf()[inst] );
        }

        TS_ASSERT_EQUALS( values.size(), (size_t)2 );
        TS_ASSERT_DELTA( values[0], 0.001, 1e-6 );
        TS_ASSERT_DELTA( values[1], 0.999, 1e-6 );

        delete prm;
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
    }

    void testAsiaBN() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/Asia.o3prm" );
        std::string package = "";
        reader.readFile( file, package );
        auto prm = reader.prm();
        gum::prm::Class<double> const* asia = nullptr;
        // Act
        TS_ASSERT_THROWS_NOTHING( asia = &( prm->getClass( "Asia" ) ) );
        // Assert
        TS_ASSERT_EQUALS( asia->attributes().size(), (gum::Size)8 );
        TS_ASSERT_EQUALS( asia->dag().sizeArcs(), (gum::Size)8 );
        delete prm;
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
    }

    void testAsiaBNGrd() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/Asia.o3prm" );
        std::string package = "";
        reader.readFile( file, package );
        auto prm = reader.prm();
        auto& asia = prm->getClass( "Asia" );
        gum::prm::System<double> sys( "Asia" );
        auto i = new gum::prm::Instance<double>( "asia", asia );
        sys.add( i );
        sys.instantiate();
        auto bn = new gum::BayesNet<double>( "Asia" );
        gum::BayesNetFactory<double> factory( bn );
        // Act
        TS_ASSERT_THROWS_NOTHING( sys.groundedBN( factory ) );
        // Assert
        TS_ASSERT_EQUALS( bn->size(), (gum::Size)8 );
        TS_ASSERT_EQUALS( bn->sizeArcs(), (gum::Size)8 );
        delete prm;
        delete bn;
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
    }

    void testVisitAsiaTwoClassesBN() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH(
            "o3prm/AsiaClassAndSystemWithTwoClasses.o3prm" );
        std::string package = "";
        reader.readFile( file, package );
        auto prm = reader.prm();
        gum::prm::Class<double> const* asia = nullptr;
        // Act
        TS_ASSERT_THROWS_NOTHING( asia = &( prm->getClass( "VisitAsia" ) ) );
        // Assert
        TS_ASSERT_EQUALS( asia->attributes().size(), (gum::Size)2 );
        TS_ASSERT_EQUALS( asia->dag().sizeArcs(), (gum::Size)1 );
        delete prm;
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
    }

    void testAsiaTwoClassesBN() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH(
            "o3prm/AsiaClassAndSystemWithTwoClasses.o3prm" );
        std::string package = "";
        reader.readFile( file, package );
        auto prm = reader.prm();
        gum::prm::Class<double> const* asia = nullptr;
        // Act
        TS_ASSERT_THROWS_NOTHING( asia = &( prm->getClass( "Asia" ) ) );
        // Assert
        TS_ASSERT_EQUALS( asia->attributes().size(), (gum::Size)6 );
        TS_ASSERT_EQUALS( asia->referenceSlots().size(), (gum::Size)1 );
        TS_ASSERT_EQUALS( asia->dag().sizeArcs(), (gum::Size)7 );
        delete prm;
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
    }

    void testAsiaTwoClassBNGrd() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH(
            "o3prm/AsiaClassAndSystemWithTwoClasses.o3prm" );
        std::string package = "";
        reader.readFile( file, package );
        auto prm = reader.prm();
        auto& sys = prm->system( "Asia" );
        // sys.instantiate();
        auto bn = new gum::BayesNet<double>( "Asia" );
        gum::BayesNetFactory<double> factory( bn );
        // Act
        TS_ASSERT_THROWS_NOTHING( sys.groundedBN( factory ) );
        // Assert
        TS_ASSERT_EQUALS( bn->size(), (gum::Size)8 );
        TS_ASSERT_EQUALS( bn->sizeArcs(), (gum::Size)8 );
        delete prm;
        delete bn;
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
        GUM_TRACE( e.errorContent() );
        GUM_TRACE( e.errorCallStack() );
      }
    }

    void testFileNotFound() {
      // Arrange
      gum::prm::o3prm::O3prmReader<double> reader;
      std::string file = "ThisFileDoesNotExist.o3prm";
      std::string package = "";
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
      TS_ASSERT_DIFFERS( reader.errors(), (gum::Size)0 );
      if ( reader.prm() ) {
        delete reader.prm();
      }
    }

    void testFileNotFoundInResDir() {
      // Arrange
      gum::prm::o3prm::O3prmReader<double> reader;
      std::string file =
          GET_RESSOURCES_PATH( "o3prm/ThisFileDoesNotExist.o3prm" );
      std::string package = "";
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
      TS_ASSERT_DIFFERS( reader.errors(), (gum::Size)0 );
      if ( reader.prm() ) {
        delete reader.prm();
      }
    }

    void testAsiaWithErrors() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/AsiaWithError.o3prm" );
        std::string package = "";
        // Act
        TS_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        // Assert
        TS_ASSERT_EQUALS( reader.errors(), (gum::Size)1 );
        TS_ASSERT_DIFFERS( reader.prm(), nullptr );
        delete reader.prm();
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
        GUM_TRACE( e.errorContent() );
        GUM_TRACE( e.errorCallStack() );
      }
    }

    void testAggOrAnd() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/aggregates.o3prm" );
        std::string package = "";
        // Act
        TS_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        // Assert
        TS_ASSERT_EQUALS( reader.errors(), (gum::Size)0 );
        if ( reader.errors() ) {
          reader.showElegantErrorsAndWarnings();
        }
        TS_ASSERT_DIFFERS( reader.prm(), nullptr );
        delete reader.prm();
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
        GUM_TRACE( e.errorContent() );
        GUM_TRACE( e.errorCallStack() );
      }
    }

    void testActors() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file = GET_RESSOURCES_PATH( "o3prm/acteurs.o3prm" );
        std::string package = "";
        // Act
        TS_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        // Assert
        TS_ASSERT_EQUALS( reader.errors(), (gum::Size)0 );
        if ( reader.errors() ) {
          reader.showElegantErrorsAndWarnings();
        }
        TS_ASSERT_DIFFERS( reader.prm(), nullptr );
        delete reader.prm();
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
        GUM_TRACE( e.errorContent() );
        GUM_TRACE( e.errorCallStack() );
      }
    }

    void testStudents() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file =
            GET_RESSOURCES_PATH( "o3prmr/University/fr/base.o3prm" );
        std::string package = "fr.base";
        // Act
        TS_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        // Assert
        TS_ASSERT_EQUALS( reader.errors(), (gum::Size)0 );
        if ( reader.errors() ) {
          reader.showElegantErrorsAndWarnings();
        }
        TS_ASSERT_DIFFERS( reader.prm(), nullptr );

        auto sys = &( reader.prm()->system( "fr.base.Work" ) );
        for ( auto iter : *sys ) {
          auto inst = iter.second;
          for ( auto node : inst->type().dag() ) {
            // TS_ASSERT( inst->exists( node ) );
            if ( ( not inst->exists( node ) ) and
                 inst->type().exists( node ) ) {
              auto elt = &( inst->type().get( node ) );
              if ( gum::prm::ClassElement<double>::isAttribute( *elt ) or
                   gum::prm::ClassElement<double>::isAggregate( *elt ) ) {

                GUM_TRACE( inst->type().name()
                           << "."
                           << inst->type().get( node ).safeName() );
              }
            }
          }
        }

        delete reader.prm();
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
        GUM_TRACE( e.errorContent() );
        GUM_TRACE( e.errorCallStack() );
      }
    }

    void testReferenceID() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file =
            GET_RESSOURCES_PATH( "o3prmr/University/fr/base.o3prm" );
        std::string package = "";
        // Act
        TS_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        // Assert
        TS_ASSERT_EQUALS( reader.errors(), (gum::Size)0 );
        if ( reader.errors() ) {
          reader.showElegantErrorsAndWarnings();
        }

        TS_ASSERT_DIFFERS( reader.prm(), nullptr );

        auto prm = reader.prm();
        for ( auto c : prm->classes() ) {
          try {
            for ( auto i : c->implements() ) {
              for ( auto r : i->referenceSlots() ) {
                TS_ASSERT_EQUALS( r->name(), c->get( r->id() ).name() );
                TS_ASSERT_EQUALS( r->safeName(), c->get( r->id() ).safeName() );
                TS_ASSERT_EQUALS( r->id(), c->get( r->name() ).id() );
              }
              for ( auto r : c->referenceSlots() ) {
                if ( i->exists( r->name() ) ) {
                  TS_ASSERT_EQUALS( r->name(), i->get( r->id() ).name() );
                  TS_ASSERT_EQUALS( r->safeName(),
                                    i->get( r->id() ).safeName() );
                  TS_ASSERT_EQUALS( r->id(), i->get( r->name() ).id() );
                }
              }
            }
          } catch ( gum::NotFound& e ) {
            // No Interface
          }
        }

        delete prm;
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
        GUM_TRACE( e.errorContent() );
        GUM_TRACE( e.errorCallStack() );
      }
    }

    void testAttributeID() {
      try {
        // Arrange
        gum::prm::o3prm::O3prmReader<double> reader;
        std::string file =
            GET_RESSOURCES_PATH( "o3prmr/University/fr/base.o3prm" );
        std::string package = "";
        // Act
        TS_ASSERT_THROWS_NOTHING( reader.readFile( file, package ) );
        // Assert
        TS_ASSERT_EQUALS( reader.errors(), (gum::Size)0 );
        if ( reader.errors() ) {
          reader.showElegantErrorsAndWarnings();
        }

        TS_ASSERT_DIFFERS( reader.prm(), nullptr );

        auto prm = reader.prm();
        for ( auto c : prm->classes() ) {
          try {
            for ( auto i : c->implements() ) {

              for ( auto a : i->attributes() ) {
                TS_ASSERT( c->exists( a->name() ) );
                TS_ASSERT( c->exists( a->safeName() ) );
                TS_ASSERT( c->exists( a->id() ) );
                TS_ASSERT_EQUALS( a->safeName(), c->get( a->id() ).safeName() );
                TS_ASSERT_EQUALS( a->id(), c->get( a->id() ).id() );
              }
            }
          } catch ( gum::NotFound& e ) {
            // No interface
          }
        }

        delete prm;
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
        GUM_TRACE( e.errorContent() );
        GUM_TRACE( e.errorCallStack() );
      }
    }

  };

}  // namespace gum_tests
