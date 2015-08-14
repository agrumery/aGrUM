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
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si =
            new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode( true );
        si->addPath( "../../../src/testunits/ressources/o3prmr/" );

        TS_GUM_ASSERT_THROWS_NOTHING(
            si->interpretFile( "../../../src/testunits/ressources/o3prmr/"
                               "requests/query1.o3prmr" ) );

        TS_ASSERT_EQUALS( si->errors(), 0 );
        if ( si->errors() > 0 ) {
          si->showElegantErrorsAndWarnings();
        }
        TS_ASSERT_EQUALS( si->warnings(), 0 );

        delete si;
      } catch ( gum::Exception& ) {
        TS_ASSERT( false );
      }
    }

    void testParseFile2() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si =
            new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode( true );
        si->addPath( "../../../src/testunits/ressources/o3prmr/" );

        TS_GUM_ASSERT_THROWS_NOTHING(
            si->interpretFile( "../../../src/testunits/ressources/o3prmr/"
                               "requests/query2.o3prmr" ) );

        TS_ASSERT_EQUALS( si->errors(), 1 );
        TS_ASSERT_EQUALS( si->warnings(), 0 );

        delete si;
      } catch ( gum::Exception& ) {
        TS_ASSERT( false );
      }
    }

    void testInference() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si =
            new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode( false );
        si->addPath( "../../../src/testunits/ressources/o3prmr/" );

        TS_GUM_ASSERT_THROWS_NOTHING(
            si->interpretFile( "../../../src/testunits/ressources/o3prmr/"
                               "requests/query1.o3prmr" ) );

        TS_ASSERT_EQUALS( si->errors(), 0 );
        TS_ASSERT_EQUALS( si->warnings(), 0 );

        delete si;
      } catch ( gum::Exception& ) {
        TS_ASSERT( false );
      }
    }

    void testObserve() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si =
            new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode( false );
        si->addPath( "../../../src/testunits/ressources/o3prmr/" );

        TS_GUM_ASSERT_THROWS_NOTHING(
            si->interpretFile( "../../../src/testunits/ressources/o3prmr/"
                               "requests/queryObserveTest.o3prmr" ) );

        TS_ASSERT_EQUALS( si->errors(), 0 );
        TS_ASSERT_EQUALS( si->warnings(), 0 );

        // Observe correctly
        const auto& c1 =
            si->prm()->system( "systems.MySystem.MySystem" ).get( "c1" );
        const auto& c2 =
            si->prm()->system( "systems.MySystem.MySystem" ).get( "c2" );

        TS_ASSERT(
            si->inference()->hasEvidence( gum::prm::PRMInference<double>::Chain(
                &c1, &c1.get( "can_print" ) ) ) );
        TS_ASSERT(
            si->inference()->hasEvidence( gum::prm::PRMInference<double>::Chain(
                &c2, &c2.get( "equipState" ) ) ) );
        TS_ASSERT( !si->inference()->hasEvidence(
            gum::prm::PRMInference<double>::Chain(
                &c1, &c1.get( "equipState" ) ) ) );
        TS_ASSERT( !si->inference()->hasEvidence(
            gum::prm::PRMInference<double>::Chain( &c2,
                                                   &c2.get( "can_print" ) ) ) );

        const auto& e = si->inference()->evidence( c2 );
        const auto p = e[c1.get( "equipState" ).id()];

        gum::Instantiation j( *p );
        const auto& c2_equipState = c2.get( "equipState" );

        for ( j.setFirst(); !j.end(); j.inc() ) {
          if ( c2_equipState.type().variable().label( j.val(
                   c2_equipState.type().variable() ) ) == "Dysfunctional" ) {
            TS_ASSERT_EQUALS( p->get( j ), 1.0 );
          } else {
            TS_ASSERT_EQUALS( p->get( j ), 0.0 );
          }
        }

        delete si;
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
    }

    void testUnobserve() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si =
            new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode( false );
        si->addPath( "../../../src/testunits/ressources/o3prmr/" );

        TS_GUM_ASSERT_THROWS_NOTHING(
            si->interpretFile( "../../../src/testunits/ressources/o3prmr/"
                               "requests/queryUnobserveTest.o3prmr" ) );

        TS_ASSERT_EQUALS( si->errors(), 0 );
        TS_ASSERT_EQUALS( si->warnings(), 0 );

        // Unobserve correctly
        const gum::prm::Instance<double>& c1 =
            si->prm()->system( "systems.MySystem.MySystem" ).get( "c1" );
        const gum::prm::Instance<double>& c2 =
            si->prm()->system( "systems.MySystem.MySystem" ).get( "c2" );

        TS_ASSERT( !si->inference()->hasEvidence(
            gum::prm::PRMInference<double>::Chain( &c1,
                                                   &c1.get( "can_print" ) ) ) );
        TS_ASSERT( !si->inference()->hasEvidence(
            gum::prm::PRMInference<double>::Chain(
                &c2, &c2.get( "equipState" ) ) ) );
        TS_ASSERT( !si->inference()->hasEvidence(
            gum::prm::PRMInference<double>::Chain(
                &c1, &c1.get( "equipState" ) ) ) );
        TS_ASSERT( !si->inference()->hasEvidence(
            gum::prm::PRMInference<double>::Chain( &c2,
                                                   &c2.get( "can_print" ) ) ) );

        delete si;
      } catch ( gum::Exception& ) {
        TS_ASSERT( false );
      }
    }

    void testQuery() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter* si =
            new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode( false );
        si->addPath( "../../../src/testunits/ressources/o3prmr/" );

        TS_GUM_ASSERT_THROWS_NOTHING(
            si->interpretFile( "../../../src/testunits/ressources/o3prmr/"
                               "requests/query1.o3prmr" ) );

        si->showElegantErrorsAndWarnings();
        TS_ASSERT_EQUALS( si->errors(), 0 );
        TS_ASSERT_EQUALS( si->warnings(), 0 );

        const gum::prm::Instance<double>& c1 =
            si->prm()->system( "systems.MySystem.MySystem" ).get( "c1" );
        const gum::prm::Instance<double>& c2 =
            si->prm()->system( "systems.MySystem.MySystem" ).get( "c2" );

        TS_ASSERT( !si->inference()->hasEvidence(
            gum::prm::PRMInference<double>::Chain( &c1,
                                                   &c1.get( "can_print" ) ) ) );
        TS_ASSERT(
            si->inference()->hasEvidence( gum::prm::PRMInference<double>::Chain(
                &c2, &c2.get( "equipState" ) ) ) );
        TS_ASSERT( !si->inference()->hasEvidence(
            gum::prm::PRMInference<double>::Chain(
                &c1, &c1.get( "equipState" ) ) ) );
        TS_ASSERT( !si->inference()->hasEvidence(
            gum::prm::PRMInference<double>::Chain( &c2,
                                                   &c2.get( "can_print" ) ) ) );

        // Est-ce que la valeur Dysfonctionnal de l'attribut can_print de
        // l'instance c1 est à 1.0

        const gum::prm::PRMInference<double>::EMap& e =
            si->inference()->evidence( c2 );
        const gum::Potential<double>* p = e[c1.get( "equipState" ).id()];

        gum::Instantiation j( *p );
        const gum::prm::Attribute<double>& c2_equipState =
            c2.get( "equipState" );

        for ( j.setFirst(); !j.end(); j.inc() ) {
          if ( c2_equipState.type().variable().label( j.val(
                   c2_equipState.type().variable() ) ) == "Dysfunctional" ) {
            TS_ASSERT_EQUALS( p->get( j ), 1.0 );
          } else {
            TS_ASSERT_EQUALS( p->get( j ), 0.0 );
          }
        }

        delete si;
      } catch ( gum::Exception& ) {
        TS_ASSERT( false );
      }
    }

    void testAsiaBN() {
      try {
        // Arrange
        auto si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode( false );
        si->addPath( "../../../src/testunits/ressources/o3prmr/Asia/" );
        si->interpretFile(
            "../../../src/testunits/ressources/o3prmr/Asia/myRequest.o3prmr" );
        auto prm = si->prm();
        gum::prm::Class<double> const* asia = nullptr;
        // Act
        TS_ASSERT_THROWS_NOTHING( asia = &( prm->getClass( "Asia.Asia" ) ) );
        // Assert
        TS_ASSERT_EQUALS( asia->attributes().size(), (gum::Size)8 );
        TS_ASSERT_EQUALS( asia->dag().sizeArcs(), (gum::Size)8 );
        delete si;
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
    }

    void testAsiaBNGrd() {
      try {
        // Arrange
        auto si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode( false );
        si->addPath( "../../../src/testunits/ressources/o3prmr/Asia/" );
        si->interpretFile(
            "../../../src/testunits/ressources/o3prmr/Asia/myRequest.o3prmr" );
        auto prm = si->prm();
        const auto& sys = prm->system( "system.Asia" );
        auto bn = new gum::BayesNet<double>( "plop" );
        gum::BayesNetFactory<double> factory( bn );
        // Act
        TS_ASSERT_THROWS_NOTHING( sys.groundedBN( factory ) );
        // Assert
        TS_ASSERT_EQUALS( bn->size(), (gum::Size)8 );
        TS_ASSERT_EQUALS( bn->sizeArcs(), (gum::Size)8 );
        // GUM_TRACE( bn->toDot() );
        delete si;
        delete bn;
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
    }

    void testAsiaOneFileBNGrd() {
      try {
        // Arrange
        auto si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode( false );
        si->addPath( "../../../src/testunits/ressources/o3prmr/AsiaOneFile/" );
        si->interpretFile( "../../../src/testunits/ressources/o3prmr/"
                           "AsiaOneFile/myRequest.o3prmr" );
        // si->showElegantErrorsAndWarnings();
        TS_ASSERT_EQUALS( si->count(), 0 );
        if ( not si->count() ) {
          auto prm = si->prm();
          const auto& sys = prm->system( "Asia.Asia" );
          auto bn = new gum::BayesNet<double>( "plop" );
          gum::BayesNetFactory<double> factory( bn );
          // Act
          TS_ASSERT_THROWS_NOTHING( sys.groundedBN( factory ) );
          // Assert
          TS_ASSERT_EQUALS( bn->size(), (gum::Size)8 );
          TS_ASSERT_EQUALS( bn->sizeArcs(), (gum::Size)8 );
          delete bn;
        }
        // GUM_TRACE( bn->toDot() );
        delete si;
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
    }

    void testComplexPrintersGrd() {
      try {
        // Arrange
        auto si = new gum::prm::o3prmr::O3prmrInterpreter();
        si->setSyntaxMode( false );
        si->addPath(
            "../../../src/testunits/ressources/o3prmr/ComplexPrinters/" );
        si->interpretFile( "../../../src/testunits/ressources/o3prmr/"
                           "ComplexPrinters/fr/lip6/printers/request.o3prmr" );
        auto prm = si->prm();
        const auto& sys = prm->system( "fr.lip6.printers.system.Work" );
        auto bn = new gum::BayesNet<double>( "plop" );
        gum::BayesNetFactory<double> factory( bn );
        // Act
        TS_ASSERT_THROWS_NOTHING( sys.groundedBN( factory ) );
        // Assert
        TS_ASSERT_EQUALS( bn->size(), (gum::Size)144 );
        TS_ASSERT_EQUALS( bn->sizeArcs(), (gum::Size)193 );
        // GUM_TRACE( bn->toDot() );
        delete si;
        delete bn;
      } catch ( gum::Exception& e ) {
        TS_ASSERT( false );
      }
    }

    void testADDWithoutSlash() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter si;
        si.setSyntaxMode( false );
        si.addPath( "../../../src/testunits/ressources/o3prmr/ADD" );

        TS_GUM_ASSERT_THROWS_NOTHING( si.interpretFile(
            "../../../src/testunits/ressources/o3prmr/ADD/Request.o3prmr" ) );

        si.showElegantErrorsAndWarnings();
        TS_ASSERT_EQUALS( si.errors(), 0 );
        TS_ASSERT_EQUALS( si.warnings(), 0 );

        TS_ASSERT_EQUALS( si.results().size(), (gum::Size)1 );

      } catch ( gum::Exception& ) {
        TS_ASSERT( false );
      }
    }

    void testADD() {
      try {
        gum::prm::o3prmr::O3prmrInterpreter si;
        si.setSyntaxMode( false );
        si.addPath( "../../../src/testunits/ressources/o3prmr/ADD/" );

        TS_GUM_ASSERT_THROWS_NOTHING( si.interpretFile(
            "../../../src/testunits/ressources/o3prmr/ADD/Request.o3prmr" ) );

        si.showElegantErrorsAndWarnings();
        TS_ASSERT_EQUALS( si.errors(), 0 );
        TS_ASSERT_EQUALS( si.warnings(), 0 );

        TS_ASSERT_EQUALS( si.results().size(), (gum::Size)1 );

        auto result = si.results()[0];

      } catch ( gum::Exception& ) {
        TS_ASSERT( false );
      }
    }
  };

}  // namespace gum_tests
