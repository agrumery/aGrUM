/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/PRM/elements/formattribute.h>
#include <agrum/PRM/elements/referenceSlot.h>
#include <agrum/PRM/elements/class.h>

#include <module_PRM/ClassElementTestSuite.h>

/**
 * This class is used to test gum::prm::ClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of 
 * gum::prm::ClassElement.
 */
namespace gum_tests {

  class ReferenceSlotTestSuite : public CxxTest::TestSuite {
    private:
      typedef gum::prm::ReferenceSlot<double> ReferenceSlot;
      ClassElementTestSuite* __classEltTestSuite;
      gum::prm::Class<double> *__A;
      gum::prm::Class<double> *__B;
      gum::prm::Type<double> *__boolean;

    public:

      void setUp() {
        __classEltTestSuite = new ClassElementTestSuite();
        __A = new gum::prm::Class<double>("A");
        __B = new gum::prm::Class<double>("B");
        __boolean = gum::prm::Type<double>::boolean();
      }

      void tearDown() {
        delete __classEltTestSuite;
        delete __A;
        delete __B;
        delete __boolean;
      }

      /// ClassElement Tests 
      /// @{
      void testIsReferenceSlot() {
        // Arrange
        ReferenceSlot ref( "ref", *__A);
        bool expected = true;
        // Act & Assert
        __classEltTestSuite->testIsReferenceSlot(ref, expected);
      }

      void testIsAttribute() {
        // Arrange
        ReferenceSlot ref( "ref", *__A);
        bool expected = false;
        // Act & Assert
        __classEltTestSuite->testIsAttribute(ref, expected);
      }

      void testIsSlotChain() {
        // Arrange
        ReferenceSlot ref( "ref", *__A);
        bool expected = false;
        // Act & Assert
        __classEltTestSuite->testIsSlotChain(ref, expected);
      }

      void testSetNodeId() {
        // Arrange
        ReferenceSlot ref( "ref", *__A);
        // Act & Assert
        __classEltTestSuite->testSetNodeId(ref);
      }

      void testObjType() {
        // Arrange
        ReferenceSlot ref( "ref", *__A);
        // Act & Assert
        __classEltTestSuite->test_obj_type(ref);
      }

      void testSafeName() {
        try {
        // Arrange
        ReferenceSlot ref( "ref", *__A);
        // Act & Assert
        __classEltTestSuite->testSafeName(ref);
        } catch (gum::Exception &e) {
          GUM_TRACE( e.errorContent() );
          GUM_TRACE( e.errorCallStack() );
          TS_ASSERT( false );
        } 

      }

      void testCast_NotAllowed() {
        // Arrange
        ReferenceSlot ref( "ref", *__A );
        // Act & Assert
        __classEltTestSuite->testCast_NotAllowed(ref);
      }

      //void testCast() {
      //  // Arrange
      //  ReferenceSlot ref( "ref", *__A );
      //  // Act & Assert
      //  __classEltTestSuite->testCast(ref, *__boolean);
      //}
      /// @}

      /// Constructors & destructors
      /// @{
      /// @}


      /// Methods
      /// @{
      /// @}

      /// Add parents and children
      /// @{
      /// @}

  };

} // gum_tests
